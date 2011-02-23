// MsgQueue.cpp: implementation of the CMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "../GlobalShare/ComuServExport.h"
#include "MsgQueue.h"
#include "ComuServDef.h"

#undef MSG_HEAD
#define MSG_HEAD ("ComuServ-MsgQueue")

CMsgQueue::CMsgQueue()
{
	m_iSemID = -1;
	m_iMsgID = -1;
	m_bInit = false;
}

CMsgQueue::~CMsgQueue()
{
}

int CMsgQueue::Init( int v_iKeySem, int v_iKeyMsg, int v_iKeyShm1, unsigned long v_ulQueSize1,
	int v_iKeyShm2, unsigned long v_ulQueSize2, int v_iKeyShm3, unsigned long v_ulQueSize3 )
{
	int iRet = 0;
	
	iRet = _SemCreate( v_iKeySem );
	if( iRet ) goto INIT_END;
	
	iRet = _MsgCreate( v_iKeyMsg );
	if( iRet ) goto INIT_END;
	
	_SemP();
	
	iRet = m_objQue1.Init( v_iKeyShm1, v_ulQueSize1 );
	if( iRet ) goto INITQUEUE_END;
	
	iRet = m_objQue2.Init( v_iKeyShm2, v_ulQueSize2 );
	if( iRet ) goto INITQUEUE_END;
	
	iRet = m_objQue3.Init( v_iKeyShm3, v_ulQueSize3 );
	if( iRet ) goto INITQUEUE_END;

	m_bInit = true;
	
INITQUEUE_END:
	_SemV();
	
INIT_END:
	return iRet;
}

int CMsgQueue::DataPush( void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb, BYTE v_bytLvl )
{
	int iRet = 0;
	if( !m_bInit ) return ERR_QUEUE;
	
	iRet = _SemP();
	if( iRet ) return iRet;
	
	if( v_bytLvl < 2 ) iRet = m_objQue1.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb );
	else if( 2 == v_bytLvl ) iRet = m_objQue2.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb );
	else iRet = m_objQue3.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb );
	
	_SemV();
	
	if( !iRet )
	{
		msgst msg_st;
		msg_st.msg_type = 1;
		msg_st.buf[0] = 0;
		if( -1 == msgsnd( m_iMsgID, (void*)&msg_st, 1, IPC_NOWAIT ) )
		{
			PRTMSG( MSG_ERR, "msgsnd fail\n" );
			iRet = ERR_MSGSND;
		}
	}
	
	return iRet;
}

int CMsgQueue::DataPop( void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPushSymb, BYTE *v_p_bytLvl )
{
	int iRet = 0;
	if( !m_bInit ) return ERR_QUEUE;
	
	iRet = _SemP();
	if( iRet ) return iRet;

	iRet = m_objQue3.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb );
	if( !iRet )
		*v_p_bytLvl = 3;
	else
	{
		iRet = m_objQue2.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb );
		if( !iRet )
			*v_p_bytLvl = 2;
		else
		{
			iRet = m_objQue1.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb );
			*v_p_bytLvl = 1;
		}
	}

	_SemV();
	
	return iRet;
}

int CMsgQueue::DataDel( void* v_pCmpDataFunc )
{
	int iRet = 0;
	if( !m_bInit ) return ERR_QUEUE;
	
	iRet = _SemP();
	if( iRet ) return iRet;

	m_objQue3.DataDel( v_pCmpDataFunc );
	m_objQue2.DataDel( v_pCmpDataFunc );
	m_objQue1.DataDel( v_pCmpDataFunc );

	_SemV();
	
	return iRet;
}

int CMsgQueue::DataWaitPop()
{
	if( !m_bInit ) return ERR_QUEUE;

	msgst msg_st;
	if( -1 == msgrcv( m_iMsgID, (void*)&msg_st, 1, 0, 0 ) )
	{
		PRTMSG( MSG_ERR, "msgrcv fail\n" );
		return ERR_MSGRCV;
	}
	else
	{
		if( (char)0xff == msg_st.buf[0] )
		{
			return ERR_MSGSKIPBLOCK;
		}
		else
		{
			return 0;
		}
	}
}

int CMsgQueue::DataSkipWait()
{
	if( !m_bInit ) return ERR_QUEUE;

	int iRet = 0;
	msgst msg_st;
	msg_st.msg_type = 1;
	msg_st.buf[0] = 0xff;
	if( -1 == msgsnd( m_iMsgID, (void*)&msg_st, 1, IPC_NOWAIT ) )
	{
		PRTMSG( MSG_ERR, "msgsnd fail\n" );
		iRet = ERR_MSGSND;
	}
	
	return iRet;
}

int CMsgQueue::_SemCreate( int v_iKeySem )
{
	int iRet = 0;
	
	bool bFstCreate = false;
	m_iSemID = semget( (key_t)v_iKeySem, 1, 0666 | IPC_CREAT | IPC_EXCL );
	if( -1 == m_iSemID )
	{
		m_iSemID = semget( (key_t)v_iKeySem, 1, 0666 | IPC_CREAT );
		if( -1 == m_iSemID )
		{
			iRet = ERR_SEMGET;
			PRTMSG( MSG_ERR, "semget fail\n" );
			goto _SEM_CREATE_END;
		}
		else
		{
			PRTMSG( MSG_DBG, "semget Succ\n" );
		}
	}
	else
	{
		bFstCreate = true;
		PRTMSG( MSG_DBG, "Semget succ(first)\n" );
	}
	
	if( bFstCreate )
	{
		semun sem_union;
		sem_union.val = 1;
		if( -1 == semctl( m_iSemID, 0, SETVAL, sem_union ) )
		{
			iRet = ERR_SEMCTL;
			PRTMSG( MSG_ERR, "semctl SETVAL fail\n" );
			goto _SEM_CREATE_END;
		}
		else
		{
			PRTMSG( MSG_DBG, "semctl SETVAL Succ\n" );
		}
	}
	
_SEM_CREATE_END:
	return iRet;
}

int CMsgQueue::_SemV()
{
	sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;
	
	if( -1 == semop(m_iSemID, &sem_b, 1) )
	{
		PRTMSG( MSG_ERR, "semop V falied\n");
		return ERR_SEMOP;
	}
	
	return 0;
}

int CMsgQueue::_SemP()
{
	sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	
	if( -1 == semop(m_iSemID, &sem_b, 1) )
	{
		PRTMSG( MSG_ERR, "semop P falied\n");
		return ERR_SEMOP;
	}
	
	return 0;
}

int CMsgQueue::_MsgCreate( int v_iKeyMsg )
{
	m_iMsgID = msgget( (key_t)v_iKeyMsg, 0666 | IPC_CREAT );
	if( -1 == m_iMsgID )
	{
		PRTMSG( MSG_ERR, "msgget fail\n" );
		return ERR_MSGGET;
	}
	else
	{
		PRTMSG( MSG_DBG, "msgget succ\n" );
	}
	
	return 0;
}
