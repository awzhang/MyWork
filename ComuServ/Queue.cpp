// Queue.cpp: implementation of the CQueue class.
//
//////////////////////////////////////////////////////////////////////
#include "../GlobalShare/ComuServExport.h"
#include "../GlobalShare/GlobFunc.h"
#include "ComuServDef.h"
#include "Queue.h"

#undef MSG_HEAD
#define MSG_HEAD ("ComuServ-Queue")


CQueue::CQueue()
{
	m_ulMemSize = 0;
	m_iShmID = -1;
	m_pShareMem = NULL;
}

CQueue::~CQueue()
{
	//Release();
}

int CQueue::Init( int v_kShm, unsigned long v_ulQueueSize )
{
	int iRet = 0;
	void* pShare = NULL;
	bool bFirstCreate = false;
	
	if( v_kShm <= 0 || 0 == v_ulQueueSize )
	{
		return ERR_PAR;
	}

	m_iShmID = shmget( (key_t)v_kShm, v_ulQueueSize, 0666 | IPC_CREAT | IPC_EXCL );
	if( -1 == m_iShmID )
	{
		m_iShmID = shmget( (key_t)v_kShm, v_ulQueueSize, 0666 | IPC_CREAT );
		if( -1 == m_iShmID )
		{
			iRet = ERR_SHMGET;
			PRTMSG( MSG_ERR, "shmget fail\n" ); 
			goto INIT_END;
		}
		else
		{
			PRTMSG( MSG_DBG, "shmget succ, size:%d\n", int(v_ulQueueSize) ); 
		}
	}
	else
	{
		bFirstCreate = true;
		PRTMSG( MSG_DBG, "shmget succ (first), size:%d\n", int(v_ulQueueSize) ); 
	}

	pShare = shmat( m_iShmID, 0, 0 );
	if( (void*)-1 == pShare )
	{
		iRet = ERR_SHMAT;
		PRTMSG( MSG_ERR, "shmat fail\n" );
		goto INIT_END;
	}
	else
	{
		m_pShareMem = (char*)pShare;
		PRTMSG( MSG_DBG, "shmat succ\n" ); 
	}

	if( bFirstCreate )
	{
		memset( m_pShareMem, 0, v_ulQueueSize );
	}

	m_ulMemSize = v_ulQueueSize;

INIT_END:
	return iRet;
}
 
// void CQueue::Release()
// {
// }

int CQueue::DataPop( void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPushSymb )
{
	if( !m_pShareMem || 0 == m_ulMemSize || -1 == m_iShmID ) return ERR_MEMLACK;

	PQUEUEHEAD pHead = * (PQUEUEHEAD*) (m_pShareMem); // 相对地址
	if( pHead ) pHead = PQUEUEHEAD( (DWORD)pHead + m_pShareMem ); // 绝对地址

	//PQUEUEHEAD pTail = * (PQUEUEHEAD*) (m_pShareMem + 4);
	//XUN_ASSERT_VALID( (pHead && pTail) || (!pHead && !pTail), "" );

	DWORD dwCur = ::GetTickCount();
	bool bEnd = false;
	do 
	{
		// 绝对地址
		if( !pHead ) return ERR_QUEUE_EMPTY;
		if( pHead->m_wDataLen > v_dwPopSize ) return ERR_SPACELACK;

		if( dwCur - pHead->m_dwPushTime <= QUEUEVALID_PERIOD ) // 若数据未超时
		{
			memcpy( v_pPopData, (char*)pHead + sizeof(QUEUEHEAD), pHead->m_wDataLen );
			*v_p_dwPopLen = pHead->m_wDataLen;
			*v_p_dwPushSymb = pHead->m_dwPushSymb;
			bEnd = true;
		}
		else
		{
			PRTMSG( MSG_DBG, "DataPop: Release overdue data\n" );
		}

		pHead = pHead->m_pNext; // 修改头指针 (得到的是相对地址)
		if( pHead )
		{
			* (PQUEUEHEAD*) (m_pShareMem) = (PQUEUEHEAD)((char*)pHead) ; // 更新首指针, 保存的是相对地址
			pHead = PQUEUEHEAD((DWORD)pHead + m_pShareMem); // 转化为绝对地址供后续使用
		}
		else
		{
			// 清除首、尾指针
			* (PQUEUEHEAD*) (m_pShareMem) = NULL;
			* (PQUEUEHEAD*) (m_pShareMem + 4) = NULL;
		}
	} while( !bEnd );

	return 0;
}

int CQueue::DataPush( void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb )
{
	int iRet = 0;

	if( !m_pShareMem || 0 == m_ulMemSize || -1 == m_iShmID ) return ERR_MEMLACK;
	if( 0 == v_dwPushLen ) return ERR_PAR;

	//PRTMSG( MSG_DBG, "Before DataPush: ShareMem Begin:%8x, ", (unsigned int)m_pShareMem );

	PQUEUEHEAD pHead = NULL, pTail = NULL;
	PQUEUEHEAD pNew = NULL, pNext = NULL;

	// 此时得到的仅是相对地址
 	pHead = * (PQUEUEHEAD*) (m_pShareMem);
 	pTail = * (PQUEUEHEAD*) (m_pShareMem + 4);

	XUN_ASSERT_VALID( (pHead && pTail) || (!pHead && !pTail), "" );
	
	//PRTMSG( MSG_DBG, "DataPush: xiangdui Head Ptr:%4d, xiangdui Tail Ptr:%4d\n", (int)pHead, (int)pTail );
	//if( pTail ) PRTMSG( MSG_DBG, "Last Element Len:%d\n", pTail->m_wDataLen );

	// 此时得到的才是绝对地址
	if( pHead ) pHead = (PQUEUEHEAD)( DWORD(pHead) + m_pShareMem );
	if( pTail ) pTail = (PQUEUEHEAD)( DWORD(pTail) + m_pShareMem );

	DWORD dwCirFreeTotal = 0; // 环形分配时释放空间的累积
	DWORD dwCur = GetTickCount();

	if( pHead ) // 若此时为非空队列
	{
		if( !pTail )
		{
			PRTMSG( MSG_ERR, "DataPush: Head Ptr Vaild but Tail Ptr NULL!\n" );
			return ERR_QUEUE;
		}

		// 计算往后继续分配时应该使用的起始地址
		pNew = PQUEUEHEAD( (char*)pTail + sizeof(QUEUEHEAD) + pTail->m_wDataLen ); // 新分配地址,绝对地址
		pNew = PQUEUEHEAD( (char*)pNew + ( (DWORD)pNew % 4 ? 4 - (DWORD)pNew % 4 : 0 ) ); // 确保内存对齐

		if( pTail >= pHead ) // 若现在是顺向分配 (包括了队列此时只有一个元素的情况下)
		{
			//PRTMSG( MSG_DBG, "1\n");
			goto _QUESUN_DEAL;
		}
		else
		{
			//PRTMSG( MSG_DBG, "2\n");
			goto _QUECIR_DEAL;
		}
	}
	else // 说明此时队列为空
	{
		if( pTail )
		{
			PRTMSG( MSG_ERR, "DataPush: Head Ptr NULL but Tail Ptr Invalid!\n" );
			return ERR_QUEUE;
		}

		goto _QUEEMPTY_DEAL;
	}

_QUESUN_DEAL: // 顺向分配
	if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= m_pShareMem + m_ulMemSize ) // 若继续分配后不会超过内存区Top边界
	{
		//PRTMSG( MSG_DBG, "3\n");
		goto _DATAPUSH_END;
	}
	else
	{
		pNew = PQUEUEHEAD( m_pShareMem + 8 ); // 可能从Bottom边界开始分配,转到环形分配方式进一步判断,绝对地址
		//PRTMSG( MSG_DBG, "4\n");
		goto _QUECIR_DEAL;
	}

_QUECIR_DEAL: // 环形分配
	dwCirFreeTotal = 0;
	while( true )
	{
		// 保险判断Head指针
		if( (char*)pHead > m_pShareMem + m_ulMemSize )
		{
			iRet = ERR_QUEUE;
			PRTMSG( MSG_ERR, "DataPush: Head Ptr Over Top Side\n" );
			goto _DATAPUSH_END;
		}

		// 保险判断累积释放空间大小是否超过整个共享内存大小,防止无限循环
		if( dwCirFreeTotal > m_ulMemSize )
		{
			iRet = ERR_QUEUE;
			PRTMSG( MSG_ERR, "DataPush: Cir Alloc Total Free Space Over Space Size\n" );
			goto _DATAPUSH_END;
		}

		// 判断现在是否可以环形分配
		if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= (char*)pHead ) // 若分配后不会超过Head位置
		{
			//PRTMSG( MSG_DBG, "5\n");
			goto _DATAPUSH_END; // 已分配,成功结束循环判断
		}
		
		// 若分配后将超过head位置,即队列已满

		// 判断头数据是否可以删除以腾出空间
		if( dwCur <= pHead->m_dwPushTime + QUEUEVALID_PERIOD ) // 若头数据未超时过期
		{
			iRet = ERR_QUEUE_FULL; // 不能删除,队列满无法插入新元素
			goto _DATAPUSH_END; // 分配失败,结束循环判断
		}
		else
		{
			PRTMSG( MSG_DBG, "DataPush: Release overdue data\n" );
		}
		
		// 否则即此头数据可以释放

		// 释放头数据空间
		pNext = pHead->m_pNext; // 相对地址
		if( pNext ) pNext = (PQUEUEHEAD)( DWORD(pNext) + m_pShareMem ); // 绝对地址
		if( !pNext ) // 若已释放最后一个元素,队列变为空队列
		{
			if( pTail != pHead )
			{
				iRet = ERR_QUEUE;
				PRTMSG( MSG_ERR, "DataPush: For Last Element, Head Ptr != Tail Ptr\n" );
				goto _DATAPUSH_END;
			}

			pHead = pTail = NULL; // 先清空头尾指针
			goto _QUEEMPTY_DEAL; // 转为空队列状态处理 
		}

		// 若释放的不是最后一个元素,即释放后队列仍然非空

		// 判断是否释放的是离Top边界最近的元素
		if( pNext < pHead )
		{
			// 先计算累积释放空间
			dwCirFreeTotal += DWORD( m_pShareMem + m_ulMemSize - (char*)pHead );

			// 再更新头指针,绝对地址
			pHead = pNext;

			// 然后判断继续分配是否超过Top边界
			if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= m_pShareMem + m_ulMemSize ) // 若不超过Top边界
			{
				//PRTMSG( MSG_DBG, "6\n");
				goto _DATAPUSH_END;
			}
			else
			{
				pNew = PQUEUEHEAD( m_pShareMem + 8 ); // 下次从Bottom边界开始判断可否分配
			}
		}
		else
		{
			pHead = pNext; // 更新头指针,绝对地址
			dwCirFreeTotal += DWORD( (char*)pNext - (char*)pHead ); // 计算累积释放空间
		}
	}

_QUEEMPTY_DEAL: // 空队列分配
	pHead = pTail = NULL; // 确保头尾指针为空
	pNew = PQUEUEHEAD( m_pShareMem + 8 ); // 从边界开始分配,绝对地址
	if( (char*)pNew + sizeof(QUEUEHEAD) + v_dwPushLen <= m_pShareMem + m_ulMemSize ) // 若分配后不会超过内存区Top边界
	{
		pHead = pNew; // 这里只更新头指针 (尾指针后续统一更新)
		goto _DATAPUSH_END;
	}
	else
	{
		iRet = ERR_QUEUE_TOOSMALL;
		goto _DATAPUSH_END;
	}

_DATAPUSH_END:
	// 若前面分配成功,则这里进行统一的元素插入操作
	if( !iRet )
	{
		// 新元素赋值
		memcpy( (char*)pNew + sizeof(QUEUEHEAD), v_pPushData, v_dwPushLen );
		pNew->m_dwPushSymb = v_dwPushSymb;
		pNew->m_wDataLen = v_dwPushLen;
		pNew->m_dwPushTime = dwCur;
		pNew->m_pNext = NULL;
		
		// 原尾元素的后向指针指向本次的新尾元素, 保存的是相对地址 (当原尾元素存在时)
		if( pTail ) pTail->m_pNext = PQUEUEHEAD( (char*)pNew - m_pShareMem );
		
		// 更新尾指针,绝对地址
		pTail = pNew;
	}

	// 在共享内存中更新头尾指针 (相对地址)
	*(PQUEUEHEAD*) (m_pShareMem) = PQUEUEHEAD( pHead ? (char*)pHead - m_pShareMem : NULL ); // 修改头指针
	*(PQUEUEHEAD*) (m_pShareMem + 4) = PQUEUEHEAD( pTail ? (char*)pTail - m_pShareMem : NULL ); // 修改尾指针

	if( iRet ) PRTMSG( MSG_DBG, "DataPush: Result %d\n", iRet );

	return iRet;
}

int CQueue::DataDel( void* v_pCmpDataFunc )
{
	if( !m_pShareMem || 0 == m_ulMemSize || -1 == m_iShmID ) return ERR_MEMLACK;
	return 0;
}
