#include "StdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("Monitor-DOG")

DWORD DOG_RSTVAL = 0XFFFFFFFF & ~DOG_QIAN & ~DOG_COMU & ~DOG_SOCK & ~DOG_IO & ~DOG_UPGD & ~DOG_DVR_T1 & ~DOG_DVR_T2 & ~DOG_DVR_T3;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDog::CDog()
{
	m_iSemID = -1;
	m_iShmID = -1;
	m_pShareMem = NULL;
	
	_SemCreate(); // 先
	_ShmCreate(); // 后
}

CDog::~CDog()
{
	
}

/*void CDog::DogClr( DWORD v_dwSymb )
{
	DWORD dwDogVal = 0;

	_SemP();

	memcpy( &dwDogVal, m_pShareMem, sizeof(dwDogVal) );
	dwDogVal |= v_dwSymb;
	memcpy( m_pShareMem, &dwDogVal, sizeof(DWORD) );

	_SemV();
}*/

DWORD CDog::DogQuery()
{
	DWORD dwRetVal = 0;

	_SemP();
	
	memcpy( &dwRetVal, m_pShareMem, sizeof(dwRetVal) );
	memcpy( m_pShareMem, &DOG_RSTVAL, sizeof(DWORD) );
	
	_SemV();

	return dwRetVal;
}

void CDog::DogInit()
{
	_SemP();
	memset(m_pShareMem, 0xff, sizeof(DWORD) ); // 必须全部置1
	_SemV();
}

int CDog::_SemCreate()
{
	int iRet = 0;
	
	bool bFstCreate = false;
	m_iSemID = semget( (key_t)KEY_SEM_DOG, 1, 0666 | IPC_CREAT | IPC_EXCL );
	if( -1 == m_iSemID )
	{
		m_iSemID = semget( (key_t)KEY_SEM_DOG, 1, 0666 | IPC_CREAT );
		if( -1 == m_iSemID )
		{
			iRet = ERR_SEMGET;
			PRTMSG( MSG_ERR, "semget fail\n" );
			goto _SEM_CREATE_END;
		}
		else
		{
			PRTMSG( MSG_DBG, "semget succ\n" );
		}
	}
	else
	{
		bFstCreate = true;
		PRTMSG( MSG_DBG, "semget succ(first)\n" );
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
			PRTMSG( MSG_DBG, "semctl SETVAL succ\n" );
		}
	}
	
_SEM_CREATE_END:
	return iRet;
}

int CDog::_SemV()
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

int CDog::_SemP()
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

int CDog::_ShmCreate()
{
	int iRet = 0;
	void* pShare = NULL;
	bool bFirstCreate = false;
	
	m_iShmID = shmget( (key_t)KEY_SHM_DOG, sizeof(DWORD), 0666 | IPC_CREAT | IPC_EXCL );
	if( -1 == m_iShmID )
	{
		m_iShmID = shmget( (key_t)KEY_SHM_DOG, sizeof(DWORD), 0666 | IPC_CREAT );
		if( -1 == m_iShmID )
		{
			iRet = ERR_SHMGET;
			PRTMSG( MSG_ERR, "shmget fail\n" ); 
			goto _SHM_CREATE_END;
		}
		else
		{
			PRTMSG( MSG_DBG, "shmget succ\n" ); 
		}
	}
	else
	{
		bFirstCreate = true;
	}
	
	pShare = shmat( m_iShmID, 0, 0 );
	if( (void*)-1 == pShare )
	{
		iRet = ERR_SHMAT;
		PRTMSG( MSG_ERR, "shmat fail\n" );
		goto _SHM_CREATE_END;
	}
	else
	{
		PRTMSG( MSG_DBG, "shmat succ\n" );
		m_pShareMem = (char*)pShare;
	}
	
	if( bFirstCreate )
	{
		_SemP();
		memset( m_pShareMem, 0xff, sizeof(DWORD) );
		_SemV();
	}
	
_SHM_CREATE_END:
	return iRet;
}

