#define _REENTRANT

#include <pthread.h>
#include "typedef.h"
#include "InnerDataMng.h"
#include "GlobFunc.h"

#undef MSG_HEAD
#define MSG_HEAD ("Global-InnerData ")

CInnerDataMng::CInnerDataMng( DWORD v_dwSpaceSize /* = 10000 */, DWORD v_dwInvalidPeriod /* = 1000000 */ )
{
	m_dwSpaceSize = v_dwSpaceSize;
	m_dwSpaceUse = 0;
	m_dwPacketNum = 0;
	m_dwInvalidPeriod = v_dwInvalidPeriod;
	m_pHead = m_pTail = NULL;

	pthread_mutex_init(&m_DataMutex, NULL);
}

CInnerDataMng::~CInnerDataMng()
{
	Release();

	pthread_mutex_destroy( &m_DataMutex );
}

/// 寻找第1个匹配的数据并弹出,匹配函数由调用者传入
int CInnerDataMng::PopSymbData( JUGSYMBDATAFUNC v_pJugSymbFunc, void* v_pSymb, DWORD v_dwSymbSize, BYTE &v_bytLvl,
	DWORD v_dwBufSize, DWORD &v_dwDataLen, char * v_pDataBuf, DWORD &v_dwPushTm,
	BYTE* v_p_bytSymb /*= NULL*/, char* v_szReserv /*= NULL*/, size_t v_sizReserv /*= 0*/ )
{
	int iRet = ERR_QUEUE_SYMBNOFIND; // 初始设为没找到应答
	tagInnerData *pCur = NULL, *pPre = NULL;
	DWORD dwCur = 0;
	bool bNeedDelUnit = false;
	bool bEnd = false;

	if( !v_pJugSymbFunc ) return ERR_PAR;
	
	pthread_mutex_lock( &m_DataMutex );

	if( !m_pTail )
	{
		if( !m_pHead )
		{
			iRet = ERR_QUEUE_SYMBNOFIND;
		}
		else
		{
			XUN_ASSERT_VALID( false, "" );
			iRet = ERR_QUEUE;
		}
		goto SYMBPOPDATA_END;
	}
	else if( !m_pHead )
	{
		iRet = ERR_QUEUE;
		XUN_ASSERT_VALID( false, "" );
		goto SYMBPOPDATA_END;
	}

	pCur = m_pTail;
	do
	{
		pPre = pCur->m_pPre;
		bNeedDelUnit = false;
		bEnd = false;
		dwCur = GetTickCount();

		if( !v_pJugSymbFunc(pCur->m_pData, v_pSymb, v_dwSymbSize) )
		{
			// 输出数据
			memcpy( v_pDataBuf, pCur->m_pData, pCur->m_dwDataLen );
			v_dwDataLen = pCur->m_dwDataLen;
			v_bytLvl = pCur->m_bytLvl;
			if( v_p_bytSymb ) *v_p_bytSymb = pCur->m_bytSymb;
			if( v_szReserv )
			{
				if( v_sizReserv > sizeof(pCur->m_szReserve) )
				{
					memcpy( v_szReserv, pCur->m_szReserve, sizeof(pCur->m_szReserve) );
					v_szReserv[ sizeof(pCur->m_szReserve) ] = 0;
				}
				else
				{
					memcpy( v_szReserv, pCur->m_szReserve, v_sizReserv );
				}				
			}

			bEnd = true; // 找到一个即可
			bNeedDelUnit = true;
		}
		else if( dwCur - pCur->m_dwPushTm >= m_dwInvalidPeriod || pCur->m_dwDataLen > v_dwBufSize )
		{
			bNeedDelUnit = true;
		}

		if( bNeedDelUnit )
		{
			// 在链表中删除该单元
			if( pCur->m_pPre ) pCur->m_pPre->m_pNext = pCur->m_pNext;
			if( pCur->m_pNext ) pCur->m_pNext->m_pPre = pCur->m_pPre;
			else m_pTail = pCur->m_pPre;
			if( !m_pTail ) m_pHead = NULL;

			delete pCur;


			// 更新链表的特征参数
			if( m_dwSpaceUse > pCur->m_dwDataLen + sizeof(*pCur) )
				m_dwSpaceUse -= pCur->m_dwDataLen + sizeof(*pCur);
			else
				m_dwSpaceUse = 0;

			if( m_dwPacketNum > 0 ) m_dwPacketNum --;

		}

		if( bEnd )
		{
			iRet = 0; // 成功应答
			break;
		}

		pCur = pPre;

	} while( pCur );

	if( !m_pTail ) m_pHead = NULL; // 保险操作

SYMBPOPDATA_END:
	pthread_mutex_unlock( &m_DataMutex );
	return iRet;
}

/// 寻找所有匹配的数据并删除,匹配函数由调用者传入
int CInnerDataMng::DelSymbData( JUGSYMBDATAFUNC v_pJugSymbFunc, void* v_pSymb, DWORD v_dwSymbSize )
{
	int iRet = 0;

	tagInnerData *pCur = NULL, *pPre = NULL;

	if( !v_pJugSymbFunc ) return ERR_PAR;
	
	pthread_mutex_lock( &m_DataMutex );

	if( !m_pTail )
	{
		if( !m_pHead )
		{
			iRet = 0;
		}
		else
		{
			XUN_ASSERT_VALID( false, "" );
			iRet = ERR_QUEUE;
		}
		goto DELSYMBDATA_END;
	}
	else if( !m_pHead )
	{
		iRet = ERR_QUEUE;
		XUN_ASSERT_VALID( false, "" );
		goto DELSYMBDATA_END;
	}

	pCur = m_pTail;
	do
	{
		pPre = pCur->m_pPre;
		if( !v_pJugSymbFunc(pCur->m_pData, v_pSymb, v_dwSymbSize) )
		{
			// 在链表中删除该单元
			if( pCur->m_pPre ) pCur->m_pPre->m_pNext = pCur->m_pNext;
			if( pCur->m_pNext ) pCur->m_pNext->m_pPre = pCur->m_pPre;
			else m_pTail = pCur->m_pPre;
			if( !m_pTail ) m_pHead = NULL;

			delete pCur;


			// 更新链表的特征参数
			if( m_dwSpaceUse > pCur->m_dwDataLen + sizeof(*pCur) )
				m_dwSpaceUse -= pCur->m_dwDataLen + sizeof(*pCur);
			else
				m_dwSpaceUse = 0;

			if( m_dwPacketNum > 0 ) m_dwPacketNum --;

			// 继续寻找下一个来删除
		}
		pCur = pPre;

	} while( pCur );

	if( !m_pTail ) m_pHead = NULL; // 保险操作

DELSYMBDATA_END:
	pthread_mutex_unlock( &m_DataMutex );
	return iRet;
}

/// 从尾部弹出一个数据包
int CInnerDataMng::PopData(BYTE &v_bytLvl, DWORD v_dwBufSize, DWORD &v_dwDataLen, char *v_pDataBuf,
	DWORD &v_dwPushTm, BYTE* v_p_bytSymb /*= NULL*/, char* v_szReserv /*=NULL*/, size_t v_sizReserv /*=0*/ )
{
	int iRet = 0;
	DWORD dwDiscardPackets = 0; // 丢弃太旧的数据包计数

	tagInnerData* pTemp = NULL;
	
	pthread_mutex_lock( &m_DataMutex );

	DWORD dwCur = GetTickCount();

	bool bContinue = false;
	do
	{
		if( !m_pTail )
		{
			if( !m_pHead )
			{
				iRet = ERR_QUEUE_EMPTY;
				goto POPDATA_END;
			}
			else
			{
				XUN_ASSERT_VALID( false, "" );
				iRet = ERR_QUEUE;
				goto POPDATA_END;
			}
		}
		else if( !m_pHead )
		{
			iRet = ERR_QUEUE;
			XUN_ASSERT_VALID( false, "" );
			goto POPDATA_END;
		}

		pTemp = m_pTail;
		m_pTail = m_pTail->m_pPre;
		if( m_pTail ) m_pTail->m_pNext = NULL;
		else m_pHead = NULL;

		if( dwCur - pTemp->m_dwPushTm >= m_dwInvalidPeriod || pTemp->m_dwDataLen > v_dwBufSize )
		{
			// 丢弃过时数据包
			dwDiscardPackets ++;
			bContinue = true;
		}
		else
		{
			memcpy( v_pDataBuf, pTemp->m_pData, pTemp->m_dwDataLen );
			v_dwDataLen = pTemp->m_dwDataLen;
			v_bytLvl = pTemp->m_bytLvl;
			if( v_p_bytSymb ) *v_p_bytSymb = pTemp->m_bytSymb;
			if( v_szReserv )
			{
				if( v_sizReserv > sizeof(pTemp->m_szReserve) )
				{
					memcpy( v_szReserv, pTemp->m_szReserve, sizeof(pTemp->m_szReserve) );
					v_szReserv[ sizeof(pTemp->m_szReserve) ] = 0;
				}
				else
				{
					memcpy( v_szReserv, pTemp->m_szReserve, v_sizReserv );
				}				
			}
			bContinue = false;
		}

		v_dwPushTm = pTemp->m_dwPushTm;

		if( m_dwSpaceUse > pTemp->m_dwDataLen + sizeof(*pTemp) )
			m_dwSpaceUse -= pTemp->m_dwDataLen + sizeof(*pTemp);
		else
			m_dwSpaceUse = 0;

		if( m_dwPacketNum > 0 ) m_dwPacketNum --;

		delete pTemp;

	} while( bContinue );

POPDATA_END:
	pthread_mutex_unlock( &m_DataMutex );

	if( dwDiscardPackets > 0 ) PRTMSG( MSG_DBG, "Discard %d packets when pop\n", dwDiscardPackets );
	return iRet;
}

// 输入参数v_szSmsTel,以NULL结尾
int CInnerDataMng::PushData(BYTE v_bytLvl, DWORD v_dwDataLen, char *v_pDataBuf, DWORD &v_dwPacketNum, BYTE v_bytSymb/*=0*/, char* v_szReserv/*=NULL*/, size_t v_sizReserv/*=0*/ )
{	
	int iRet = 0;
	tagInnerData* pNewData = NULL;
	bool bNeedDelOld = false;

	pthread_mutex_lock( &m_DataMutex );

	DWORD dwCur = GetTickCount();

	if( m_dwSpaceUse + v_dwDataLen + sizeof(tagInnerData) + 200 > m_dwSpaceSize ) // 若缓冲区即将满
	{
		iRet = ERR_QUEUE_FULL;
		bNeedDelOld = true;
		goto PUSHDATA_END;
	}

	/// 分配Data及保存Data的对象的空间
	pNewData = new tagInnerData;
	if( !pNewData )
	{
		iRet = ERR_MEMLACK;
		goto PUSHDATA_END;
	}
	pNewData->m_dwPushTm = dwCur;
	pNewData->m_bytSymb = v_bytSymb;
	if( v_dwDataLen > 0 )
	{
		pNewData->m_pData = malloc( v_dwDataLen ); // 必须用malloc分配,与tagInnerData析构函数中的free对应
		if( !(pNewData->m_pData) )
		{
			PRTMSG(MSG_DBG, "malloc err, v_dwDataLen = %d\n", v_dwDataLen);
			iRet = ERR_MEMLACK;
			goto PUSHDATA_END;
		}
		memcpy( pNewData->m_pData, v_pDataBuf, v_dwDataLen );
	}
	else
	{
		pNewData->m_pData = NULL;
	}
	pNewData->m_dwDataLen = v_dwDataLen;
	pNewData->m_bytLvl = v_bytLvl;
	if( v_szReserv )
	{
		memset( pNewData->m_szReserve, 0, sizeof(pNewData->m_szReserve) );
		memcpy( pNewData->m_szReserve, v_szReserv,
			v_sizReserv < sizeof(pNewData->m_szReserve) ? v_sizReserv : sizeof(pNewData->m_szReserve) );
	}

	/// 插入到链表
	if( 1 == v_bytLvl) // 若普通级别的
	{
		/// 从头插入

		pNewData->m_pNext = m_pHead;
		if( m_pHead ) m_pHead->m_pPre = pNewData;
		m_pHead = pNewData;

		if( !m_pTail ) m_pTail = pNewData;
	}
	else
	{
		/// 从尾插入,但是跳过级别一样的或更高的

		if( !m_pTail )
		{
			m_pTail = pNewData;
			XUN_ASSERT_VALID( (!m_pHead), "" );
			m_pHead = pNewData;
		}
		else
		{
			XUN_ASSERT_VALID( m_pHead, "" );

			tagInnerData* pTemp = m_pTail;
			while( pTemp )
			{
				if( pTemp->m_bytLvl < v_bytLvl )
				{
					pNewData->m_pNext = pTemp->m_pNext;
					pNewData->m_pPre = pTemp;
					pTemp->m_pNext = pNewData;

					if( m_pTail == pTemp ) m_pTail = pNewData;

					break;
				}
				else pTemp = pTemp->m_pPre;
			}

			if( !pTemp )
			{
				pNewData->m_pNext = m_pHead;
				if( m_pHead ) m_pHead->m_pPre = pNewData;
				m_pHead = pNewData;
			}
		}
	}

	m_dwSpaceUse += pNewData->m_dwDataLen + sizeof(*pNewData);
	m_dwPacketNum ++;

	pNewData = NULL;

PUSHDATA_END:
	if( pNewData ) delete pNewData;
	
	pthread_mutex_unlock( &m_DataMutex );

	if( bNeedDelOld )
	{
		DelOldData();
	}

	v_dwPacketNum = m_dwPacketNum;

	return iRet;
}

/// 数据回存到队列中 (插入到尾)
int CInnerDataMng::SaveBackData( BYTE v_bytLvl, DWORD v_dwDataLen, char *v_pDataBuf, BYTE v_bytSymb, char* v_szReserv )
{
	int iRet = 0;
	tagInnerData* pNewData = NULL;

	pthread_mutex_lock( &m_DataMutex );

	/// 分配Data及保存Data的对象的空间
	pNewData = new tagInnerData;
	if( !pNewData )
	{
		iRet = ERR_MEMLACK;
		goto SAVEDATA_END;
	}
	if( v_dwDataLen > 0 )
	{
		pNewData->m_pData = malloc( v_dwDataLen );
		if( !(pNewData->m_pData) )
		{
			iRet = ERR_MEMLACK;
			goto SAVEDATA_END;
		}
		memcpy( pNewData->m_pData, v_pDataBuf, v_dwDataLen );
	}
	else
	{
		pNewData->m_pData = NULL;
	}
	pNewData->m_dwDataLen = v_dwDataLen;
	pNewData->m_bytLvl = v_bytLvl;
	pNewData->m_bytSymb = v_bytSymb;
	if( v_szReserv ) strncpy( pNewData->m_szReserve, v_szReserv, sizeof(pNewData->m_szReserve) - 1 );

	/// 插入到链表尾
	if( m_pTail )
	{
		XUN_ASSERT_VALID( m_pHead, "" );
		m_pTail->m_pNext = pNewData;
		pNewData->m_pPre = m_pTail;
		m_pTail = pNewData;
	}
	else
	{
		XUN_ASSERT_VALID( (!m_pHead), "" );
		m_pHead = m_pTail = pNewData;
	}
	
	m_dwSpaceUse += pNewData->m_dwDataLen + sizeof(*pNewData);
	m_dwPacketNum ++;

	pNewData = NULL;

SAVEDATA_END:
	if( pNewData ) delete pNewData;
	pthread_mutex_unlock( &m_DataMutex );
	return iRet;
}

int CInnerDataMng::DelOldData()
{
	int iRet = 0;
	tagInnerData *pCur = NULL, *pTemp = NULL;
	DWORD dwDelBytes = 0, dwDelCount = 0;
	
	pthread_mutex_lock( &m_DataMutex );

	DWORD dwCur = GetTickCount();

	if( !m_pTail )
	{
		if( !m_pHead )
		{
			iRet = 0;
		}
		else
		{
			XUN_ASSERT_VALID( false, "" );
			iRet = ERR_QUEUE;
		}
		goto DELOLDDATA_END;
	}
	else if( !m_pHead )
	{
		iRet = ERR_QUEUE;
		XUN_ASSERT_VALID( false, "" );
		goto DELOLDDATA_END;
	}

	dwDelBytes = 0;
	dwDelCount = 0;
	pCur = m_pTail;
	do
	{
		pTemp = pCur->m_pPre;
		if( dwCur - pCur->m_dwPushTm >= m_dwInvalidPeriod )
		{
			dwDelBytes += pCur->m_dwDataLen;
			dwDelCount ++;
			
			if( m_dwSpaceUse > pCur->m_dwDataLen + sizeof(*pCur) )
				m_dwSpaceUse -= pCur->m_dwDataLen + sizeof(*pCur);
			else
				m_dwSpaceUse = 0;

			if( m_dwPacketNum > 0 ) m_dwPacketNum --;

			if( pCur->m_pPre ) pCur->m_pPre->m_pNext = pCur->m_pNext;
			if( pCur->m_pNext ) pCur->m_pNext->m_pPre = pCur->m_pPre;
			else m_pTail = pCur->m_pPre;

			delete pCur;

			if( !m_pTail )
			{
				m_pHead = NULL;
				break;
			}
		}
		pCur = pTemp;

	} while( pCur );

DELOLDDATA_END:
	pthread_mutex_unlock( &m_DataMutex );
	return iRet;
}

int CInnerDataMng::DelSpecData( BYTE v_bytSymb )
{
	tagInnerData *pCur = NULL, *pTemp = NULL;
	DWORD dwDelBytes = 0, dwDelCount = 0;
	int iRet = 0;

	pthread_mutex_lock( &m_DataMutex );

	if( !m_pTail )
	{
		if( !m_pHead )
		{
			iRet = 0;
		}
		else
		{
			XUN_ASSERT_VALID( false, "" );
			iRet = ERR_QUEUE;
		}
		goto DELSPECDATA_END;
	}
	else if( !m_pHead )
	{
		iRet = ERR_QUEUE;
		XUN_ASSERT_VALID( false, "" );
		goto DELSPECDATA_END;
	}

	dwDelBytes = 0;
	dwDelCount = 0;
	pCur = m_pTail;
	do
	{
		pTemp = pCur->m_pPre;
		if( v_bytSymb & pCur->m_bytSymb )
		{
			dwDelBytes += pCur->m_dwDataLen;
			dwDelCount ++;
			
			if( m_dwSpaceUse > pCur->m_dwDataLen + sizeof(*pCur) )
				m_dwSpaceUse -= pCur->m_dwDataLen + sizeof(*pCur);
			else
				m_dwSpaceUse = 0;

			if( m_dwPacketNum > 0 ) m_dwPacketNum --;

			if( pCur->m_pPre ) pCur->m_pPre->m_pNext = pCur->m_pNext;
			if( pCur->m_pNext ) pCur->m_pNext->m_pPre = pCur->m_pPre;
			else m_pTail = pCur->m_pPre;

			delete pCur;

			if( !m_pTail )
			{
				m_pHead = NULL;
				break;
			}
		}
		pCur = pTemp;

	} while( pCur );

DELSPECDATA_END:
	pthread_mutex_unlock( &m_DataMutex );
	return iRet;
}

bool CInnerDataMng::IsSpecDataExist( BYTE v_bytSymb )
{
	tagInnerData* pTemp = NULL;
	bool bRet = false;
	int iRet = 0;

	pthread_mutex_lock( &m_DataMutex );

	if( !m_pTail )
	{
		if( m_pHead )
		{
			XUN_ASSERT_VALID( false, "" );
		}
		goto SPECEXIST_END;
	}
	else if( !m_pHead )
	{
		XUN_ASSERT_VALID( false, "" );
		goto SPECEXIST_END;
	}

	pTemp = m_pTail;
	while( pTemp )
	{
		if( v_bytSymb & pTemp->m_bytSymb )
		{
			bRet = true;
			goto SPECEXIST_END;
		}
		pTemp = pTemp->m_pPre;
	}

SPECEXIST_END:
	pthread_mutex_unlock( &m_DataMutex );

	return bRet;
}

void CInnerDataMng::InitCfg( DWORD v_dwSpaceSize, DWORD v_dwInvalidPeriod )
{
	pthread_mutex_lock( &m_DataMutex );

	m_dwSpaceSize = v_dwSpaceSize;
	m_dwInvalidPeriod = v_dwInvalidPeriod;

	pthread_mutex_unlock( &m_DataMutex );
}

void CInnerDataMng::GetCfg( DWORD& v_dwSpaceSize, DWORD& v_dwInvalidPeriod )
{
	pthread_mutex_lock( &m_DataMutex );

	v_dwSpaceSize = m_dwSpaceSize;
	v_dwInvalidPeriod = m_dwInvalidPeriod;

	pthread_mutex_unlock( &m_DataMutex );
}

void CInnerDataMng::Release()
{
	pthread_mutex_lock( &m_DataMutex );

	tagInnerData* pTemp;
	while( m_pHead )
	{
		pTemp = m_pHead->m_pNext;
		delete m_pHead;
		m_pHead = pTemp;
	}
	m_pTail = NULL;

	m_dwSpaceUse = 0;
	m_dwPacketNum = 0;

	pthread_mutex_unlock( &m_DataMutex );
}

bool CInnerDataMng::IsEmpty()
{
	bool bRet = true;

	pthread_mutex_lock( &m_DataMutex );
	
	if( !m_pTail || !m_pHead ) bRet = true;
	else bRet = false;

	pthread_mutex_unlock( &m_DataMutex );

	return bRet;
}

DWORD CInnerDataMng::GetPacketCount()
{
	DWORD dwRet = 0;

	pthread_mutex_lock( &m_DataMutex );

	dwRet = m_dwPacketNum;

	pthread_mutex_unlock( &m_DataMutex );

	return dwRet;
}
