#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-CarSta   ")

CCarSta::CCarSta()
{
	m_dwVeSta = 0;
	m_bDrvRecStaChg = false;
	
	pthread_mutex_init(&m_VestaMutex, NULL);
}

CCarSta::~CCarSta()
{
	pthread_mutex_destroy( &m_VestaMutex );
}

void CCarSta::Init()
{
	_ReadDrvRecSta();

	tagGPSData gps(0);
	m_dwVeSta &= ~VESTA_MOVE; // 初始为非移动态 （不能改）
	int iResult = g_objMonAlert.GetCurGPS( gps, false );
	if( !iResult )
	{
		if( 1 == gps.m_bytMoveType )
			m_dwVeSta |= VESTA_MOVE;
	}
	
#if USE_OIL == 2
	BYTE bytOilAD = 0, bytPowAD = 0, bytOilSta = 0;
	GetOil( &bytOilAD, &bytPowAD, &bytOilSta );
	g_objOil.SetOilPowAndSta( bytOilAD, bytPowAD, bytOilSta );
#endif
}

void CCarSta::Release()
{
	// 保存行驶状态
	SavDrvRecSta( true );
}

void CCarSta::_ReadDrvRecSta()
{
	memset( &m_objDrvRecSta, 0, sizeof(m_objDrvRecSta) );

	char szFileName[MAX_PATH];
	sprintf( szFileName, "%s", DRVRECSTA_FILENAME );
	
	pthread_mutex_lock( &m_VestaMutex );

	FILE* fp = fopen( szFileName, "rb" );
	if( fp )
	{
		// 读文件
		fread( &m_objDrvRecSta, sizeof(m_objDrvRecSta), 1, fp );
		fclose( fp );

		// 检查校验和
		BYTE* aryData = (BYTE*)&m_objDrvRecSta;
		BYTE bytSumChk = 0;
		for( size_t siz = 0; siz < sizeof(m_objDrvRecSta) - 1; siz ++ )
		{
			bytSumChk += aryData[ siz ];
		}
		if( m_objDrvRecSta.m_bytChkSum != BYTE(~bytSumChk) )
		{
			// 清除状态
			memset( &m_objDrvRecSta, 0, sizeof(m_objDrvRecSta) );
			printf("m_objDrvRecSta.m_bytChkSum uncorrect!\n");
		}

		// 若不是复位时候自动保存的,清除司机身份
		if( !m_objDrvRecSta.m_bBeforeReset )
			m_objDrvRecSta.m_wCurDriverNo = 0;
	}

	pthread_mutex_unlock( &m_VestaMutex );
}

DWORD CCarSta::GetVeSta()
{
	return m_dwVeSta;
}

void CCarSta::SetVeSta( const DWORD v_dwVeSta )
{
	m_dwVeSta = v_dwVeSta;
}

// 保存行驶状态信息到文件 (应在主线程里调用)
void CCarSta::SavDrvRecSta( bool v_bBeforeReset )
{
	FILE* fp = NULL;
	char szFileName[MAX_PATH];
	sprintf( szFileName, "%s", DRVRECSTA_FILENAME );

	pthread_mutex_lock( &m_VestaMutex );

	if( !m_bDrvRecStaChg )
		goto SAVEDRVRESSTA_END;

	fp = fopen( szFileName, "wb" );
	if( fp )
	{
		// 复位前标志
		m_objDrvRecSta.m_bBeforeReset = v_bBeforeReset;

		// 校验和
		BYTE* aryData = (BYTE*)&m_objDrvRecSta;
		BYTE bytSumChk = 0;
		for( size_t siz = 0; siz < sizeof(m_objDrvRecSta) - 1; siz ++ )
		{
			bytSumChk += aryData[ siz ];
		}
		m_objDrvRecSta.m_bytChkSum = BYTE(~bytSumChk);

		// 存入文件
		fwrite( &m_objDrvRecSta, sizeof(m_objDrvRecSta), 1, fp );
	}

	m_bDrvRecStaChg = false;

SAVEDRVRESSTA_END:
	if( fp ) fclose( fp );
	pthread_mutex_unlock( &m_VestaMutex );
}

void CCarSta::SavLstValidLonlatAndLichen( long v_lLon, long v_lLat, double v_dLichen )
{
	pthread_mutex_lock( &m_VestaMutex );
	
	m_objDrvRecSta.m_lLon = v_lLon;
	m_objDrvRecSta.m_lLat = v_lLat;
	m_objDrvRecSta.m_dTotalLichen = v_dLichen;
	m_objDrvRecSta.m_bBeforeReset = false;

	m_bDrvRecStaChg = true;

	pthread_mutex_unlock( &m_VestaMutex );
}

void CCarSta::GetLstValidLonlatAndLichen( long& v_lLon, long& v_lLat, double& v_dLichen )
{
	pthread_mutex_lock( &m_VestaMutex );
	
	v_lLon = m_objDrvRecSta.m_lLon;
	v_lLat = m_objDrvRecSta.m_lLat;
	v_dLichen = m_objDrvRecSta.m_dTotalLichen;

	pthread_mutex_unlock( &m_VestaMutex );
}

void CCarSta::SavDriverNo( WORD v_wDriverNo )
{
	pthread_mutex_lock( &m_VestaMutex );

	m_objDrvRecSta.m_wCurDriverNo = v_wDriverNo;
	
	m_bDrvRecStaChg = true;

	pthread_mutex_unlock( &m_VestaMutex );
}

WORD CCarSta::GetDriverNo()
{
	WORD wRet = 0;

	pthread_mutex_lock( &m_VestaMutex );

	wRet = m_objDrvRecSta.m_wCurDriverNo;
	
	pthread_mutex_unlock( &m_VestaMutex );

	return wRet;
}

void CCarSta::SavOil( BYTE v_bytOilAD, BYTE v_bytPowAD, BYTE v_bytOilSta )
{
	pthread_mutex_lock( &m_VestaMutex );

	m_objDrvRecSta.m_bytOilAD = v_bytOilAD;
	m_objDrvRecSta.m_bytPowAD = v_bytPowAD;
	m_objDrvRecSta.m_bytOilSta = v_bytOilSta;

	m_bDrvRecStaChg = true;
	
	pthread_mutex_unlock( &m_VestaMutex );
}

void CCarSta::GetOil( BYTE *v_p_bytOilAD, BYTE *v_p_bytPowAD, BYTE *v_p_bytOilSta )
{
	pthread_mutex_lock( &m_VestaMutex );

	if( v_p_bytOilAD ) *v_p_bytOilAD = m_objDrvRecSta.m_bytOilAD;
	if( v_p_bytPowAD ) *v_p_bytPowAD = m_objDrvRecSta.m_bytPowAD;
	if( v_p_bytOilSta ) *v_p_bytOilSta = m_objDrvRecSta.m_bytOilSta;
	
	pthread_mutex_unlock( &m_VestaMutex );
}

// 应在主线程里调用
int CCarSta::ResumeDrvSta()
{
	char szFileName[MAX_PATH];
	sprintf( szFileName, "%s",DRVRECSTA_FILENAME );

	pthread_mutex_lock( &m_VestaMutex );

	m_objDrvRecSta.m_lLon = 0;
	m_objDrvRecSta.m_lLat = 0;

	FILE* fp = fopen( szFileName, "wb" );
	if( fp )
	{
		// 复位前标志
		m_objDrvRecSta.m_bBeforeReset = false;

		// 校验和
		BYTE* aryData = (BYTE*)&m_objDrvRecSta;
		BYTE bytSumChk = 0;
		for( size_t siz = 0; siz < sizeof(m_objDrvRecSta) - 1; siz ++ )
		{
			bytSumChk += aryData[ siz ];
		}
		m_objDrvRecSta.m_bytChkSum = ~bytSumChk;

		// 存入文件
		fwrite( &m_objDrvRecSta, sizeof(m_objDrvRecSta), 1, fp );
		fclose( fp );
	}

	pthread_mutex_unlock( &m_VestaMutex );
	
	return 0;
}
