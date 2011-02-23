// Gps.cpp: implementation of the CGps class.
//
//////////////////////////////////////////////////////////////////////

#include "Gps.h"
#include "ComuServDef.h"
#include "../GlobalShare/GlobFunc.h"

#undef MSG_HEAD
#define MSG_HEAD ("ComuServ-GPS")

struct tagGpsShare
{
	DWORD m_dwSetGpsTm; // 最新设置GPS的时刻
	tagGPSData m_objRealGps; // 实时GPS
	tagGPSData m_objValidGps; // 最后一个有效GPS数据的值
};

const size_t GPS_MEMSHARE_SIZE = sizeof(tagGpsShare);

CGps::CGps()
{
	m_iSemID = -1;
	m_iShmID = -1;
	m_pShareMem = NULL;

	_SemCreate(); // 先
	_ShmCreate(); // 后
}

CGps::~CGps()
{
}

int CGps::GetCurGps( void* v_pGps, const unsigned int v_uiSize, BYTE v_bytGetType )
{
	// 可以同时设置实时GPS和有效GPS

	if( v_uiSize < sizeof(tagGPSData) ) return ERR_BUFLACK;
	
	int iRet = 0;

	tagGpsShare objGpsShare;
	tagGPSData gps(0);

	_SemP();

	memcpy( &objGpsShare, m_pShareMem, sizeof(objGpsShare) );

	_SemV();

	switch( v_bytGetType )
	{
	case GPS_REAL:
		{
			gps = objGpsShare.m_objRealGps;
			if( gps.speed > BELIV_MAXSPEED ) // 若实时GPS数据速度超过最大可信速度,则认为不可信,使用上次有效数据
			{
				gps = objGpsShare.m_objValidGps;
				gps.valid = 'V';
				gps.m_cFixType = '1';
			}
		}
		break;

	case GPS_LSTVALID:
	default:
		gps = objGpsShare.m_objValidGps;
	}

	if( GetTickCount() - objGpsShare.m_dwSetGpsTm >= 5000 // 若连续一段时间没有设置GPS数据
		||
		objGpsShare.m_objRealGps.valid != 'A' ) // 或GPS无效
	{
		// 用系统时间修正无效的GPS数据,并且GPS数据强制为无效的
		
		struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);
		
		gps.valid = 'V';
		gps.m_cFixType = '1';
		gps.nYear = pCurrentTime.tm_year + 1900;
		gps.nMonth = pCurrentTime.tm_mon + 1;
		gps.nDay = pCurrentTime.tm_mday;
		gps.nHour = pCurrentTime.tm_hour;
		gps.nMinute = pCurrentTime.tm_min;
		gps.nSecond = pCurrentTime.tm_sec;
		
	}

	if( gps.nYear < 2001 ) 
	{
		gps.nYear = 2001; // 前置机有bug,不支持2000年以前的,而2000年会使得上发的数据中存在0
	}

	if( gps.speed >= BELIV_MAXSPEED ) // 若速度超过最大可信速度,则都设为无效数据
	{
		gps.valid = 'V';
		gps.m_cFixType = '1';
	}
	else if( gps.speed < 0 ) // 若速度小于0,则强制修正速度,并且都设为无效数据
	{
		gps.speed = 0;
		gps.valid = 'V';
		gps.m_cFixType = '1';
	}

	memcpy( v_pGps, &gps, sizeof(tagGPSData) );

	return iRet;
}

int CGps::SetCurGps( const void* v_pGps, BYTE v_bytSetType )
{
	int iRet = 0;
	tagGpsShare objGpsShare;
	tagGPSData gps(0);
	memcpy( &gps, v_pGps, sizeof(gps) );

	_SemP();

	memcpy( &objGpsShare, m_pShareMem, sizeof(objGpsShare) );
	
	if( GPS_REAL & v_bytSetType )
	{
		objGpsShare.m_objRealGps = gps;
		objGpsShare.m_dwSetGpsTm = GetTickCount();

		if( !(GPS_LSTVALID & v_bytSetType) )
		{
			if( 'A' == gps.valid && gps.speed <= BELIV_MAXSPEED ) // (要满足速度不超过最大可信速度)
			{
				objGpsShare.m_objValidGps = gps;
			}
		}
	}

	if( (GPS_LSTVALID & v_bytSetType) && gps.speed <= BELIV_MAXSPEED ) // (要满足速度不超过最大可信速度)
	{
		objGpsShare.m_objValidGps = gps;
	}

	memcpy( m_pShareMem, &objGpsShare, sizeof(objGpsShare) );

	_SemV();

	return iRet;
}

int CGps::_SemCreate()
{
	int iRet = 0;
	
	bool bFstCreate = false;
	m_iSemID = semget( (key_t)KEY_SEM_GPS, 1, 0666 | IPC_CREAT | IPC_EXCL );
	if( -1 == m_iSemID )
	{
		m_iSemID = semget( (key_t)KEY_SEM_GPS, 1, 0666 | IPC_CREAT );
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

int CGps::_SemV()
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

int CGps::_SemP()
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

int CGps::_ShmCreate()
{
	int iRet = 0;
	void* pShare = NULL;
	bool bFirstCreate = false;
	
	m_iShmID = shmget( (key_t)KEY_SHM_GPS, GPS_MEMSHARE_SIZE, 0666 | IPC_CREAT | IPC_EXCL );
	if( -1 == m_iShmID )
	{
		m_iShmID = shmget( (key_t)KEY_SHM_GPS, GPS_MEMSHARE_SIZE, 0666 | IPC_CREAT );
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
		tagGpsShare objGpsShare;
		
		struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);
	
		memset( (void*)&objGpsShare, 0, sizeof(objGpsShare) );
		objGpsShare.m_dwSetGpsTm = GetTickCount();
		objGpsShare.m_objRealGps.valid = 'V';
		objGpsShare.m_objRealGps.m_cFixType = '1';

		objGpsShare.m_objRealGps.nYear = pCurrentTime.tm_year + 1900;
		objGpsShare.m_objRealGps.nMonth = pCurrentTime.tm_mon + 1;
		objGpsShare.m_objRealGps.nDay = pCurrentTime.tm_mday;
		objGpsShare.m_objRealGps.nHour = pCurrentTime.tm_hour;
		objGpsShare.m_objRealGps.nMinute = pCurrentTime.tm_min;
		objGpsShare.m_objRealGps.nSecond = pCurrentTime.tm_sec;

		if( objGpsShare.m_objRealGps.nYear < 2001 ) 
			objGpsShare.m_objRealGps.nYear = 2001; // 因为前置机有bug,不支持2000年以前的

		objGpsShare.m_objValidGps = objGpsShare.m_objRealGps;
		
		_SemP();
		memset( m_pShareMem, 0, GPS_MEMSHARE_SIZE );
		memcpy( m_pShareMem, &objGpsShare, GPS_MEMSHARE_SIZE );
		_SemV();
	}
	
_SHM_CREATE_END:
	return iRet;
}

void CGps::_BjTimeToGlwzTime(tm *v_Time, int *v_iTime)
{
	bool bCarry = false; // 时间计算时的退位标志
	
	v_iTime[0] = v_Time->tm_year + 1900;
	v_iTime[1] = v_Time->tm_mon + 1;
	v_iTime[2] = v_Time->tm_mday;
	v_iTime[3] = v_Time->tm_hour;
	v_iTime[4] = v_Time->tm_min;
	v_iTime[5] = v_Time->tm_sec;
	
	// 小时减8
	v_iTime[3] -= 8;
	if( v_iTime[3] < 0 )
	{
		v_iTime[3] += 24;
		bCarry = true;
	}
	
	// 计算天数
	int nMaxDay = 0;	
	switch( v_iTime[1] )
	{
	case 2:		
	case 4:
	case 5:
	case 8:
	case 9:
	case 11:
	case 1:		// 1,3,5,7,8,10,12月有31天， 注意：这个天数是前一个月的天数
		nMaxDay = 31;
		break;
		
	case 3:		// 2月有29天
		nMaxDay = 28;
		
	default:	// 其他30天
		nMaxDay = 30;
		break;
	}
	
	// 若是闰年的2月，则有29天
	if( 2 == v_iTime[1] && ( 0 == v_iTime[0] % 400 || (0 == v_iTime[0] % 4 && 0 != v_iTime[0] % 100) ) )
	{
		nMaxDay = 29;
	}
	
	v_iTime[2] -= (bCarry ? 1 : 0);
	if( v_iTime[2] <= 0 )
	{
		v_iTime[2] = nMaxDay;
		bCarry = true;
	}
	else
	{
		bCarry = false;
	}
	
	// 计算月份
	v_iTime[1] -= (bCarry ? 1 : 0);
	if( v_iTime[1] <= 0 )
	{
		v_iTime[1] += 12;
		bCarry = true;
	}
	else
	{
		bCarry = false;
	}
	
	// 更新年份
	v_iTime[0] -= (bCarry ? 1 : 0);
}



