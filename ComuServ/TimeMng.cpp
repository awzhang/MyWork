// TimeMng.cpp: implementation of the CTimeMng class.
//
//////////////////////////////////////////////////////////////////////

#include "TimeMng.h"
#include "ComuServDef.h"

#undef MSG_HEAD
#define MSG_HEAD ("ComuServ-TimeMng")

const size_t TMMNG_MEMSHARE_SIZE = sizeof(long);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeMng::CTimeMng()
{
	m_iSemID = -1;
	m_iShmID = -1;
	m_pMemShare = NULL;

	_SemCreate();
	_ShmCreate();
}

CTimeMng::~CTimeMng()
{

}

// 传入的应是tm类型对象的指针 (UTC时间)
int CTimeMng::SetCurTime( void* v_pTmSet )
{
	int iRet = 0;
	int iResult = 0;
	time_t tSet = -1;
	tm tmSet;
	char buf[100] = { 0 };
	char szMsg[180] = { 0 };
	int iMsgType = 0;

	// 当前时间 (UTC)
	//time( &tCur );

	// 设置后的时间 (UTC)
// 	tmSet.tm_year = v_iYear - 1900;
// 	tmSet.tm_mon = v_iMonth - 1;
// 	tmSet.tm_mday = v_iDay;
// 	tmSet.tm_hour = v_iHour;
// 	tmSet.tm_min = v_iMinute;
// 	tmSet.tm_sec = v_iSecond;
	memcpy( &tmSet, v_pTmSet, sizeof(tmSet) );
	sprintf( buf, "date -s %02d%02d%02d%02d%04d.%02d", tmSet.tm_mon + 1, tmSet.tm_mday, tmSet.tm_hour, tmSet.tm_min, tmSet.tm_year + 1900, tmSet.tm_sec );
	tSet = mktime( &tmSet );

	if( -1 == tSet )
	{
		PRTMSG( MSG_ERR, "SetCurTime fail, tSet(%lu), cmd(%s)\n", (unsigned long)tSet, buf );
		return ERR_TM;
	}


	timeval tmCur;
	if( -1 == gettimeofday( &tmCur, NULL ) )
	{
		PRTMSG( MSG_ERR, "SetCurTime fail, gettimeofday fail!\n" );
		return ERR_TM;
	}

	_SemP();

	iResult = system( buf );

	if( !iResult )
	{
		// 计算前后时间差 (ms)
		long lDelt = long(tSet) * 1000 - tmCur.tv_sec * 1000 - tmCur.tv_usec / 1000;

		// 获取原时间差
		long lDeltOrig = 0;
		memcpy( &lDeltOrig, m_pMemShare, sizeof(lDeltOrig) );

		// 修正时间差
		lDelt += lDeltOrig;
		memcpy( m_pMemShare, &lDelt, TMMNG_MEMSHARE_SIZE );

		sprintf( szMsg, "SetCurTime Succ (%s)\n", buf );
		iMsgType = MSG_DBG;
	}
	else
	{
		sprintf( szMsg, "SetCurTime fail %d (%s)\n", iResult, buf );
		iMsgType = MSG_ERR;
		iRet = ERR_SYSCALL;
	}

	_SemV();

	PRTMSG( iMsgType, szMsg );

	return iRet;
}

unsigned int CTimeMng::GetTickCount() // 返回值: 从1970.1.1凌晨起经历的时间,ms
{
	long lRet = 0;
	long lDelt = 0;
	timeval tm;

	_SemP();

	gettimeofday( &tm, NULL );

	lRet = (tm.tv_sec * 1000 + tm.tv_usec / 1000);

	memcpy( &lDelt, m_pMemShare, sizeof(lDelt) );

	_SemV();

	lRet -= lDelt;
	return (unsigned int)lRet;
}

int CTimeMng::_SemCreate()
{
	int iRet = 0;
	
	bool bFstCreate = false;
	m_iSemID = semget( (key_t)KEY_SEM_TM, 1, 0666 | IPC_CREAT | IPC_EXCL );
	if( -1 == m_iSemID )
	{
		m_iSemID = semget( (key_t)KEY_SEM_TM, 1, 0666 | IPC_CREAT );
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

int CTimeMng::_SemV()
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

int CTimeMng::_SemP()
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

int CTimeMng::_ShmCreate()
{
	bool bFstCreate = false;
	
	if( -1 == m_iShmID )
	{
		m_iShmID = shmget( (key_t)KEY_SHM_TM, TMMNG_MEMSHARE_SIZE, 0666 | IPC_CREAT | IPC_EXCL );
		if( -1 == m_iShmID )
		{
			m_iShmID = shmget( (key_t)KEY_SHM_TM, TMMNG_MEMSHARE_SIZE, 0666 | IPC_CREAT );
			if( -1 == m_iShmID )
			{
				PRTMSG( MSG_ERR, "shm get fail\n" );
				return ERR_SHMGET;
			}
			else
			{
				PRTMSG( MSG_DBG, "shm get succ\n" );
			}
		}
		else
		{
			bFstCreate = true;
			PRTMSG( MSG_DBG, "shm get succ(first)\n" );
		}
	}
	
	if( !m_pMemShare )
	{
		void *pShare = shmat( m_iShmID, 0, 0 );
		if( (void*)-1 != pShare )
		{
			PRTMSG( MSG_DBG, "shmat succ\n" );
			m_pMemShare = (char*)pShare;
			if( bFstCreate )
			{
				memset( pShare, 0, TMMNG_MEMSHARE_SIZE );
			}
		}
		else
		{
			PRTMSG( MSG_ERR, "shmat fail\n" );
			return ERR_SHMAT;
		}
	}
	
	return 0;
}
