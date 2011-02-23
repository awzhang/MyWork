// AcdentDlg.cpp : implementation file
//

#include "yx_QianStdAfx.h"
#include "yx_Acdent.h"
//1
#if USE_ACDENT == 1 || USE_ACDENT == 2

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
// #endif

// enum
// {
// 	WM_ACDENTSAV = WM_USER + 38
// };

// enum
// {   
// 	ACDUPLOAD_TIMER = 1
// };

#define ACDREAD_UNIT	5	// 每次读取事故疑点的记录数 (每个记录376 Bytes) (别太大了,否则栈里面的数组会太大,造成溢出)
const UINT ACDENTCHK_INTERVAL		= 198; // 事故疑点处理间隔,ms
const UINT ACDENTUPLOAD_INTERVAL	= 1000; // 定时上传事故疑点数据间隔 (毫秒)


void *G_ChkAcdent( void* v_pVoid )
{
	g_objAcdent.P_LoopChkAcdent();
}

void *G_AcdentSave( void* v_pVoid )
{
	g_objAcdent.P_AcdentSave();
}

void G_TmUploadAcdent(void *arg, int len)
{
	g_objAcdent.P_TmUploadAcdent();
}


/////////////////////////////////////////////////////////////////////////////
// CAcdent dialog


CAcdent::CAcdent()
{
	m_hThdAcdent = 0;
	m_hThdSave = 0;
	m_bThdToExit = false;
	
	m_objAcdentDataMng.InitCfg( QUEUE_ACDENT_SIZE, QUEUE_ACDENT_PERIOD );
	memset( m_aryAcdQueryPeriod, 0, sizeof(m_aryAcdQueryPeriod) );
	m_bytAcdQueryPeridCount = 0;
	memset( m_aryAcdQueryArea, 0, sizeof(m_aryAcdQueryArea) );
	m_bytAcdQueryAreaCount = 0;
	memset( &m_objAcdQuerySta, 0, sizeof(m_objAcdQuerySta) );

	if( 0 != sem_init(&m_semSave, 0, 0) )
	{
		PRTMSG( MSG_ERR, "CAcdent() - sem_init fail %d", errno );
	}

	strcpy( m_szAcdPath, ACD_SAVE_PATH );
}

CAcdent::~CAcdent()
{
	Release(); // 保险起见

	sem_destroy( &m_semSave );
}

int CAcdent::Init()
{
	int iRet = 0;

	// 清除线程退出标志
	m_bThdToExit = false;

	// 设置事故疑点线程属性
	int iResult = 0;
	pthread_attr_t objThreadAttr;
	iResult = pthread_attr_init( &objThreadAttr );
	if( iResult )
	{
		PRTMSG( MSG_ERR, "Init - pthread_attr_init fail %d\n", iResult );
		iRet = ERR_THREAD;
		goto INIT_END;
	}
	iResult = pthread_attr_setdetachstate( &objThreadAttr, PTHREAD_CREATE_DETACHED ); // 设置为分离线程
	if( iResult )
	{
		PRTMSG( MSG_ERR, "Init - pthread_setdetachstate fail %d\n", iResult );
		iRet = ERR_THREAD;
		goto INIT_END;
	}
	
	// 启动事故疑点检测线程
	iResult = pthread_create( &m_hThdAcdent, &objThreadAttr, G_ChkAcdent, NULL );
	if( iResult || 0 == m_hThdAcdent )
	{
		PRTMSG( MSG_ERR, "Init - pthread_create(Acdent Thread) fail %d\n", iResult );
		iRet = ERR_THREAD;
		goto INIT_END;
	}


	// 设置保存线程属性
	// 不设置了，直接使用事故疑点线程的属性
	
	// 启动保存线程
	iResult = pthread_create( &m_hThdSave, &objThreadAttr, G_AcdentSave, NULL );
	if( iResult || 0 == m_hThdSave )
	{
		PRTMSG( MSG_ERR, "Init - pthread_create(Save Thread) fail %d\n", iResult );
		iRet = ERR_THREAD;
		goto INIT_END;
	}
	
INIT_END:
	return iRet;
}

void CAcdent::Release()
{
	// 关闭事故疑点检测线程
	m_bThdToExit = true;
	sem_post( &m_semSave );
	usleep(100000); // 为了让子模块繁忙的子线程正常退出，本线程sleep等待一下较好

	if( m_hThdAcdent )
	{
		void* pThdRet;
		int iResult = 0;
		
		DWORD dwCt = 0;
		while( 1 )
		{
			iResult = pthread_kill( m_hThdAcdent, 0);
			if( ESRCH == iResult || EINVAL == iResult )
			{
				break;
			}
			else
			{
				if( dwCt ++ >= 60 ) break;
				usleep( 50000 );
			}
		}
		
		m_hThdAcdent = 0;
	}

	if( m_hThdSave )
	{
		void* pThdRet;
		int iResult = 0;
		
		DWORD dwCt = 0;
		while( 1 )
		{
			iResult = pthread_kill( m_hThdSave, 0);
			if( ESRCH == iResult || EINVAL == iResult )
			{
				break;
			}
			else
			{
				if( dwCt ++ >= 60 ) break;
				usleep( 50000 );
			}
		}
		
		m_hThdSave = 0;
	}
}

void CAcdent::P_AcdentSave()
{
	tagAcdentRec objAcdentRec;
	DWORD dwDataLen = 0;
	DWORD dwPushTm = 0;
	BYTE bytLvl = 0;
	FILE* fpAcdent = NULL;
	char szFileName[100] = { 0 };
	char szLst[100] = { 0 };

	while( !m_bThdToExit )
	{
		sem_wait( &m_semSave );

		while( !m_bThdToExit && !m_objAcdentDataMng.PopData(bytLvl, sizeof(objAcdentRec), dwDataLen, (char*)&objAcdentRec, dwPushTm ) )
		{
			if( dwDataLen != sizeof(objAcdentRec) ) continue;

			// 生成疑点数据文件名
			sprintf( szFileName, "%sAcd_%02d%02d_%d.dat", m_szAcdPath, objAcdentRec.m_bytStopYear,
				objAcdentRec.m_bytStopMon, objAcdentRec.m_bytStopDay / DSK2_ONEFILE_DAYS + 1 );
			szFileName[ sizeof(szFileName) - 1 ] = 0;

			// 保存事故疑点数据
			if( strcmp(szLst, szFileName) )
			{
				if( fpAcdent ) fclose( fpAcdent );
				
				//g_objFlashPart4Mng.DelOldFile(); // 生成新文件前,先删除旧文件

				if( access(m_szAcdPath, F_OK) != 0 )
				{
					struct stat lDirStat;
					if( 0 == mkdir(m_szAcdPath, lDirStat.st_mode) )
					{	
						chown(m_szAcdPath, lDirStat.st_uid, lDirStat.st_gid);
					}
					else
					{
						continue;
					}
				}

				fpAcdent = fopen( szFileName, "ab" );
			}
			if( fpAcdent )
			{
				strcpy( szLst, szFileName );
				fwrite( &objAcdentRec, sizeof(objAcdentRec), 1, fpAcdent );
			}
		}
	}

	if( fpAcdent ) fclose( fpAcdent );
}

// 子线程函数
void CAcdent::P_LoopChkAcdent()
{
	tagGPSData objGps(0);
	tagAcdentRec objAcdentRec;
	const BYTE ACDENTREC_SECONDS = sizeof(objAcdentRec.m_aryAcdentSec) / sizeof(objAcdentRec.m_aryAcdentSec[0]); // 一个事故疑点记录所包括的秒数
	const BYTE STOP_CONTSECS = 10; // 速度低于一定值,持续达到多少秒时认为是静止 (不要超过59秒)
	const BYTE ACDENTREC_BUFSIZE = ACDENTREC_SECONDS + STOP_CONTSECS; 
	tagAcdentSec	aryAcdentSecBuf[ ACDENTREC_BUFSIZE ];
	double dCurSpeed = 0; // 当前实时的速度, 米/秒
	double dMeterPerCycle = 0; // 每里程脉冲多少米 (标定值换算得到)
	DWORD dwPacketNum = 0;
	DWORD dwStopSecs = 0; // 连续多少次静止
	DWORD dwIoSta = 0;
	DWORD dwCyclePerSec = 0; // 每秒脉冲数
	WORD wCyclePerKm = 0; // 每公里脉冲数 (标定值)
	BYTE bytCt = 0; // 1秒内每0.2s记录一次的序号,0～4
	BYTE bytLstSecNo = 0; // 当前要进行记录的编号,ACDENTREC_BUFSIZE-1,循环使用,每秒1次
	BYTE bytSrc = 0, bytDes = 0;
	BYTE bytLoopCt = 0; // 必须=0
	bool bSpdDone = false;
	bool bSpdSilent = true;
	memset( aryAcdentSecBuf, 0, sizeof(aryAcdentSecBuf) );

	DWORD dwLstChk = ::GetTickCount();
	while( !m_bThdToExit )
	{
#ifdef DIS_SPD_USEIO
		// 每100秒中更新一次标定值 (初始时也在此更新)
		if( 0 == bytLoopCt % 100 )
		{
			::GetImpCfg( &wCyclePerKm, sizeof(wCyclePerKm),
				offsetof(tagImportantCfg, m_uni1QBiaodingCfg.m_obj1QBiaodingCfg.m_wCyclePerKm), sizeof(wCyclePerKm) );
			if( wCyclePerKm > 50 )
			{
				dMeterPerCycle = 1000.0 / wCyclePerKm;  // 注意保证wCyclePerKm大于0,以防止除0异常
			}
			else
			{
				dMeterPerCycle = 0;
			}
			bytLoopCt = 0;
		}
#endif

		 // 保险起见
		if( bytLstSecNo >= ACDENTREC_BUFSIZE )
			bytLstSecNo = ACDENTREC_BUFSIZE - 1;
		if( bytCt >= sizeof(aryAcdentSecBuf[0].m_aryIOSta) / sizeof(aryAcdentSecBuf[0].m_aryIOSta[0]) )
			bytCt = 0;

		/// 填充每0.2s的数据

		// 填充传感器,协议定义: D7～D0：其中0表示无信号，1表示有信号
		//	D7--脚刹信号
		//  D6--手刹信号
		//  D5--左转向灯信号
		//  D4--右转向灯信号
		//  D3--前车门信号
		//  D2--后车门信号
		//  D1--电子喇叭信号
		//  D0--ACC信号
		g_objQianIO.IN_QueryIOSta( dwIoSta );
		aryAcdentSecBuf[ bytLstSecNo ].m_aryIOSta[ bytCt ] = ~BYTE(dwIoSta % 0x100); // 最低字节全部取反之后即为需要值

		// 填充速度
#ifdef DIS_SPD_USEIO
		bSpdDone = false;
		dwCyclePerSec = 0; // 先初始化

		// 先使用里程脉冲
		if( g_objQianIO.IN_QueryIOSpeed(dwCyclePerSec) )
		{
			if( dMeterPerCycle > 0.01 ) // 若标定值有效
			{
				//if( g_pView ) g_pView->DbgShowMsg( L"IO Spd = %d", dwCyclePerSec );
				dCurSpeed = dwCyclePerSec * dMeterPerCycle;
				bSpdSilent = dCurSpeed <= MOVE_MINSPEED_IO;
				bSpdDone = true;
			}
		}
		
		// 失败,再使用GPS
		if( !bSpdDone )
		{
			objGps.Init();
			::GetCurGps( &objGps, sizeof(objGps), GPS_REAL );
			dCurSpeed = objGps.speed;
			bSpdSilent = dCurSpeed <= MOVE_MINSPEED_GPS;
		}
#else
		objGps.Init();
		::GetCurGps( &objGps, sizeof(objGps), GPS_REAL );
		dCurSpeed = objGps.speed;
		bSpdSilent = dCurSpeed <= MOVE_MINSPEED_GPS;
#endif
		aryAcdentSecBuf[ bytLstSecNo ].m_arySpeed[ bytCt ] = BYTE(dCurSpeed * MIMIAO_TO_HAIHOUR);

		// 每秒的中间时刻,保存经纬度
		if( 2 == bytCt )
		{
			// 获取经纬度
			::GetCurGps( &objGps, sizeof(objGps), GPS_REAL );

			::G_Lonlat_DTo4Char( objGps.longitude, aryAcdentSecBuf[ bytLstSecNo ].m_szLon );
			::G_Lonlat_DTo4Char( objGps.latitude, aryAcdentSecBuf[ bytLstSecNo ].m_szLat );
		}

		// 填充每秒一次的数据
		if( ++ bytCt >= 5 ) // 判断该秒的记录是否结束
		{
			// 下一秒重新开始填充每0.2s的数据
			bytCt = 0;

			// 静止判断
			if( bSpdSilent )
			{
				++ dwStopSecs; // 递增静止秒数

				// 判断是否可以作为一个疑点记录
				if( STOP_CONTSECS == dwStopSecs ) // 若停止时间达到设定值
				{
					// 获取GPS信息 (为了得到时间)
					::GetCurGps( &objGps, sizeof(objGps), GPS_REAL );

					// 生成事故疑点记录
					memset( &objAcdentRec, 0, sizeof(objAcdentRec) );
					if( bytLstSecNo >= STOP_CONTSECS )
						bytSrc = bytLstSecNo - STOP_CONTSECS;
					else
						bytSrc = bytLstSecNo + ACDENTREC_BUFSIZE - STOP_CONTSECS;
					bytDes = ACDENTREC_SECONDS - 1;
					while(1) 
					{
						objAcdentRec.m_aryAcdentSec[ bytDes ] = aryAcdentSecBuf[ bytSrc ];
						if( 0 == bytDes ) break;
						bytDes --;

						if( 0 == bytSrc ) bytSrc = ACDENTREC_BUFSIZE - 1;
						else -- bytSrc;
					}
					objAcdentRec.m_lStopLon = ::G_Lonlat_4CharToL( objAcdentRec.m_aryAcdentSec[ACDENTREC_SECONDS - 1].m_szLon );
					objAcdentRec.m_lStopLat = ::G_Lonlat_4CharToL( objAcdentRec.m_aryAcdentSec[ACDENTREC_SECONDS - 1].m_szLat );
					objAcdentRec.m_wDriverNo = g_objCarSta.GetDriverNo(); // 司机编号
					// 疑点时刻
					{
						tagGPSData objGpsTm = objGps;
						if( objGpsTm.nSecond >= STOP_CONTSECS ) objGpsTm.nSecond -= STOP_CONTSECS;
						else
						{
							objGpsTm.nSecond += 60;
							objGpsTm.nSecond -= STOP_CONTSECS;

							if( objGpsTm.nMinute > 0 ) objGpsTm.nMinute --;
							else
							{
								objGpsTm.nMinute = 59;

								if( objGpsTm.nHour > 0 ) objGpsTm.nHour --;
								else
								{
									objGpsTm.nHour = 59;

									if( objGpsTm.nDay > 1 ) objGpsTm.nDay --;
									else
									{
										if( objGpsTm.nMonth > 1 ) objGpsTm.nMonth --;
										else
										{
											objGpsTm.nMonth = 12;
											objGpsTm.nYear --;
										}

										if( 2 == objGpsTm.nMonth )
										{
											if( 0 == objGpsTm.nYear % 400 
												|| (0 == objGpsTm.nYear % 4 && 0 != objGpsTm.nMonth % 100) ) // 闰年
											{
												objGpsTm.nDay = 29;
											}
											else objGpsTm.nDay = 28;
										}
										else if( objGpsTm.nMonth <= 7 )
										{
											if( 0 == objGpsTm.nMonth % 2 ) objGpsTm.nDay = 30;
										}
										else
										{
											if( 1 == objGpsTm.nMonth % 2 ) objGpsTm.nDay = 30;
 										}
									}
								}
							}
						}

						// 赋值
						objAcdentRec.m_bytStopYear = BYTE( objGpsTm.nYear % 100 );
						objAcdentRec.m_bytStopMon = BYTE( objGpsTm.nMonth );
						objAcdentRec.m_bytStopDay = BYTE( objGpsTm.nDay );
						objAcdentRec.m_bytStopHour = BYTE( objGpsTm.nHour );
						objAcdentRec.m_bytStopMin = BYTE( objGpsTm.nMinute );
						objAcdentRec.m_bytStopSec = BYTE( objGpsTm.nSecond );
					}

					m_objAcdentDataMng.PushData( LV8, sizeof(objAcdentRec), (char*)&objAcdentRec, dwPacketNum );
					sem_post( &m_semSave );
				}
			}
			else
			{
				dwStopSecs = 0; // 静止秒数清0
			}

			// 递增记录编号,注意循环使用
			if( ++ bytLstSecNo >= ACDENTREC_BUFSIZE )
				bytLstSecNo = 0;
		}

		// 递增循环次数
		++ bytLoopCt;
	}
}

void CAcdent::P_TmUploadAcdent()
{
	DWORD dwBegin = ::GetTickCount();
	FILE* fp = NULL;
	char szFile[ MAX_PATH ];
	size_t sizRead = 0, siz = 0;
	tagAcdentRec aryAcdRec[ ACDREAD_UNIT ];
	tag3644 ary3644[4];
	bool bExist3644 = false;
	BYTE byt = 0;
	bool bCurFileFindFitPeriod = false; // 当前文件是否曾经找到了符合时间段条件的记录
	bool bFindPause = false; // 查找是暂停

	while( m_objAcdQuerySta.m_wDoneFileCount < m_objAcdQuerySta.m_wFileTotal ) 
	{
		// 打开文件
		strcpy( szFile, "/mnt/Flash/part4/" );
		strcat( szFile, m_objAcdQuerySta.m_aryAcdentFile[ m_objAcdQuerySta.m_wDoneFileCount] );
		fp = fopen( szFile, "rb" );
		while( fp )
		{
			// 清除状态
			bCurFileFindFitPeriod = false;

			// 跳过已处理的记录
			if( fseek(fp, m_objAcdQuerySta.m_dwLstFileDoneCt * sizeof(tagAcdentRec), SEEK_SET) )
				break;

			// 循环读取,每次一组
			while( true )
			{
				sizRead = fread( aryAcdRec, sizeof(aryAcdRec[0]), sizeof(aryAcdRec) / sizeof(aryAcdRec[0]), fp );
				for( siz = 0; siz < sizRead; siz ++ )
				{
					// 判断该记录是否满足时间段条件
					if( m_bytAcdQueryPeridCount > 0 ) // 若有时间段条件
					{
						for( byt = 0; byt < m_bytAcdQueryPeridCount; byt ++ )
						{
							if( *(DWORD*)&(aryAcdRec[siz].m_bytStopHour) < *(DWORD*)&(m_aryAcdQueryPeriod[byt].m_bytBeginHour)
								|| *(DWORD*)&(aryAcdRec[siz].m_bytStopHour) > *(DWORD*)&(m_aryAcdQueryPeriod[byt].m_bytEndHour)
								)
								continue;

							if( *(DWORD*)&(aryAcdRec[siz].m_bytStopHour) == *(DWORD*)&(m_aryAcdQueryPeriod[byt].m_bytBeginHour)
								&& aryAcdRec[siz].m_bytStopMin < m_aryAcdQueryPeriod[byt].m_wBeginMin )
								continue;

							if( *(DWORD*)&(aryAcdRec[siz].m_bytStopHour) == *(DWORD*)&(m_aryAcdQueryPeriod[byt].m_bytEndHour)
								&& aryAcdRec[siz].m_bytStopMin > m_aryAcdQueryPeriod[byt].m_wEndMin )
								continue;

							break; // 找到满足的时段条件
						}

						// 若没有找到满足的时段条件
						if( byt >= m_bytAcdQueryPeridCount )
						{
							if( bCurFileFindFitPeriod ) // 若该文件曾经找到了满足时段条件的记录
								goto ONEACDFILE_READEND; // 该文件不可能再有满足时段条件的记录了
							else
								continue; // 跳过该记录
						}
						else bCurFileFindFitPeriod = true;
					}

					// 判断该记录是否满足范围条件
					if( m_bytAcdQueryAreaCount > 0 ) // 若有范围条件
					{
						for( byt = 0; byt < m_bytAcdQueryAreaCount; byt ++ )
						{
							if( ::JugPtInRect(aryAcdRec[siz].m_lStopLon, aryAcdRec[siz].m_lStopLat,
								m_aryAcdQueryArea[byt].m_lLonMin, m_aryAcdQueryArea[byt].m_lLatMin,
								m_aryAcdQueryArea[byt].m_lLonMax, m_aryAcdQueryArea[byt].m_lLatMax) )
							{
								break;
							}
						}

						// 若没有找到满足的范围条件,跳过该记录
						if( byt >= m_bytAcdQueryAreaCount ) continue;
					}

					
					// 运行到这里，说明本次循环有符合条件的记录，将要发送

					// 若之前有缓存帧,则先发送前一帧
					if( bExist3644 )
					{
						_Snd3644Frames( ary3644 );
					}

					/// { 组帧,缓存
					memset( ary3644, 0, sizeof(ary3644) );

					// 通用信息
					for( byt = 0; byt < sizeof(ary3644) / sizeof(ary3644[0]); byt ++ )
					{
						ary3644[ byt ].m_bytRestype = 1; // 成功应答
#if USE_ACDENT == 1
						ary3644[ byt ].m_bytDriverNo = ( aryAcdRec[ siz ].m_wDriverNo ) % 0x100;
#endif
#if USE_ACDENT == 2
						ary3644[ byt ].m_szDriverNo[0] = ( aryAcdRec[ siz ].m_wDriverNo ) / 0x100;
						ary3644[ byt ].m_szDriverNo[1] = ( aryAcdRec[ siz ].m_wDriverNo ) % 0x100;
#endif
						ary3644[ byt ].m_bytFrameNo = m_objAcdQuerySta.m_bytFindAcdCt;
						ary3644[ byt ].m_szStopTime[0] = aryAcdRec[ siz ].m_bytStopYear;
						ary3644[ byt ].m_szStopTime[1] = aryAcdRec[ siz ].m_bytStopMon;
						ary3644[ byt ].m_szStopTime[2] = aryAcdRec[ siz ].m_bytStopDay;
						ary3644[ byt ].m_szStopTime[3] = aryAcdRec[ siz ].m_bytStopHour;
						ary3644[ byt ].m_szStopTime[4] = aryAcdRec[ siz ].m_bytStopMin;
						ary3644[ byt ].m_szStopTime[5] = aryAcdRec[ siz ].m_bytStopSec;
					}

					// 第1帧: 第一个10s的速度和IO
					ary3644[ 0 ].m_bytType = 0;
					for( byt = 0; byt < 10; byt ++ )
					{
						ary3644[0].m_uniAcdentData.m_arySpeedIO[5*byt][0] = aryAcdRec[siz].m_aryAcdentSec[byt].m_arySpeed[0];
						ary3644[0].m_uniAcdentData.m_arySpeedIO[5*byt][1] = aryAcdRec[siz].m_aryAcdentSec[byt].m_aryIOSta[0];

						ary3644[0].m_uniAcdentData.m_arySpeedIO[5*byt+1][0] = aryAcdRec[siz].m_aryAcdentSec[byt].m_arySpeed[1];
						ary3644[0].m_uniAcdentData.m_arySpeedIO[5*byt+1][1] = aryAcdRec[siz].m_aryAcdentSec[byt].m_aryIOSta[1];

						ary3644[0].m_uniAcdentData.m_arySpeedIO[5*byt+2][0] = aryAcdRec[siz].m_aryAcdentSec[byt].m_arySpeed[2];
						ary3644[0].m_uniAcdentData.m_arySpeedIO[5*byt+2][1] = aryAcdRec[siz].m_aryAcdentSec[byt].m_aryIOSta[2];

						ary3644[0].m_uniAcdentData.m_arySpeedIO[5*byt+3][0] = aryAcdRec[siz].m_aryAcdentSec[byt].m_arySpeed[3];
						ary3644[0].m_uniAcdentData.m_arySpeedIO[5*byt+3][1] = aryAcdRec[siz].m_aryAcdentSec[byt].m_aryIOSta[3];

						ary3644[0].m_uniAcdentData.m_arySpeedIO[5*byt+4][0] = aryAcdRec[siz].m_aryAcdentSec[byt].m_arySpeed[4];
						ary3644[0].m_uniAcdentData.m_arySpeedIO[5*byt+4][1] = aryAcdRec[siz].m_aryAcdentSec[byt].m_aryIOSta[4];
					}

					// 第2帧: 第二个10s的速度和IO
					ary3644[ 1 ].m_bytType = 1;
					for( byt = 0; byt < 10; byt ++ )
					{
						ary3644[1].m_uniAcdentData.m_arySpeedIO[5*byt][0] = aryAcdRec[siz].m_aryAcdentSec[byt+10].m_arySpeed[0];
						ary3644[1].m_uniAcdentData.m_arySpeedIO[5*byt][1] = aryAcdRec[siz].m_aryAcdentSec[byt+10].m_aryIOSta[0];

						ary3644[1].m_uniAcdentData.m_arySpeedIO[5*byt+1][0] = aryAcdRec[siz].m_aryAcdentSec[byt+10].m_arySpeed[1];
						ary3644[1].m_uniAcdentData.m_arySpeedIO[5*byt+1][1] = aryAcdRec[siz].m_aryAcdentSec[byt+10].m_aryIOSta[1];

						ary3644[1].m_uniAcdentData.m_arySpeedIO[5*byt+2][0] = aryAcdRec[siz].m_aryAcdentSec[byt+10].m_arySpeed[2];
						ary3644[1].m_uniAcdentData.m_arySpeedIO[5*byt+2][1] = aryAcdRec[siz].m_aryAcdentSec[byt+10].m_aryIOSta[2];

						ary3644[1].m_uniAcdentData.m_arySpeedIO[5*byt+3][0] = aryAcdRec[siz].m_aryAcdentSec[byt+10].m_arySpeed[3];
						ary3644[1].m_uniAcdentData.m_arySpeedIO[5*byt+3][1] = aryAcdRec[siz].m_aryAcdentSec[byt+10].m_aryIOSta[3];

						ary3644[1].m_uniAcdentData.m_arySpeedIO[5*byt+4][0] = aryAcdRec[siz].m_aryAcdentSec[byt+10].m_arySpeed[4];
						ary3644[1].m_uniAcdentData.m_arySpeedIO[5*byt+4][1] = aryAcdRec[siz].m_aryAcdentSec[byt+10].m_aryIOSta[4];
					}

					// 第3帧: 第1个10s的位置
					ary3644[ 2 ].m_bytType = 2;
					for( byt = 0; byt < 10; byt ++ )
					{
						memcpy( &(ary3644[2].m_uniAcdentData.m_aryLonlat[byt]), aryAcdRec[siz].m_aryAcdentSec[byt].m_szLon, 8);
					}

					// 第4帧: 第2个10s的位置
					ary3644[ 3 ].m_bytType = 3;
					for( byt = 0; byt < 10; byt ++ )
					{
						memcpy( &(ary3644[3].m_uniAcdentData.m_aryLonlat[byt]), aryAcdRec[siz].m_aryAcdentSec[byt+10].m_szLon, 8);
					}

					// 等到下一帧时再发送
					bExist3644 = true;
					m_objAcdQuerySta.m_bFindAcd = true;

					// 累加找到的记录计数
					++ m_objAcdQuerySta.m_bytFindAcdCt;
					if( 255 == m_objAcdQuerySta.m_bytFindAcdCt ) goto UPLOADACDENT_END; // 超过最大表示范围
					/// } 组帧,缓存
				}

				// 累加当前文件已处理的记录数
				m_objAcdQuerySta.m_dwLstFileDoneCt += siz;

				// 若本次处理耗时较大，则下次再处理
				if( ::GetTickCount() - dwBegin >= TIMER_MAXPERIOD )
				{
					bFindPause = true;
					goto UPLOADACDENT_END;
				}

				// 文件读取结束判断
				if( sizRead < sizeof(aryAcdRec) / sizeof(aryAcdRec[0]) )
					break;
			}

			// 关闭文件
			::fclose( fp );
			fp = NULL; // 不能丢!
		}

ONEACDFILE_READEND:
		// 递增已处理的文件计数
		++ m_objAcdQuerySta.m_wDoneFileCount;

		// 清除当前文件的处理状态
		m_objAcdQuerySta.m_dwLstFileDoneCt = 0;
	}

UPLOADACDENT_END:
	if( fp ) ::fclose( fp );

	if( bExist3644 )
	{
		if( !bFindPause )
		{
			ary3644[ 3 ].m_bytRestype = 2; // 查询结束
			_KillTimer(&g_objTimerMng, ACDUPLOAD_TIMER ); // 关闭发送定时
		}
		
		// 发送本次定时最后一帧
		_Snd3644Frames( ary3644 );
	}
	else if( m_objAcdQuerySta.m_wDoneFileCount >= m_objAcdQuerySta.m_wFileTotal )
	{
		_KillTimer(&g_objTimerMng, ACDUPLOAD_TIMER ); // 关闭发送定时

		if( !m_objAcdQuerySta.m_bFindAcd )
		{
			int iBufLen = 0;
			char buf[100] = { 0 };
			BYTE bytResType = 0x7f;

			if( !g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x36, 0x44, buf, sizeof(buf), iBufLen, CVT_8TO7 ) )
			{
				g_objSock.TcpSendSmsData( buf, iBufLen, LV9 );
			}
		}
		
	}
}

// 发送一组事故疑点数据
int CAcdent::_Snd3644Frames( tag3644* v_ary3644Frame )
{
	if( !v_ary3644Frame ) return ERR_PTR;

	const BYTE FRAME3644_LEN1 = offsetof(tag3644, m_uniAcdentData)
		+ sizeof(v_ary3644Frame[0].m_uniAcdentData.m_arySpeedIO);
	const BYTE FRAME3644_LEN2 = offsetof(tag3644, m_uniAcdentData)
		+ sizeof(v_ary3644Frame[0].m_uniAcdentData.m_aryLonlat);

	int iRet = 0;
	int iBufLen = 0;
	char buf[SOCK_MAXSIZE];
	for( BYTE byt = 0; byt < 4; byt ++ )
	{
		iRet = g_objSms.MakeSmsFrame( (char*)(v_ary3644Frame + byt),
			byt < 2 ? FRAME3644_LEN1 : FRAME3644_LEN2,
			0x36, 0x44, buf, sizeof(buf), iBufLen, CVT_8TO7 );
		if( !iRet ) iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9 );
	}
	return iRet;
}

/// 事故疑点查询请求: 查询字（1）＋查询时间段个数（1）＋【查询时间段】＋查询范围个数（1）＋【位置范围】
int CAcdent::Deal3604(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen )
{
	int iRet = 0;
	int iBufLen = 0;
	DWORD dw = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytQueryType = 0;
	BYTE bytPeridCount = 0, bytAreaCount = 0;
	BYTE bytResType = 0;
	BYTE byt = 0;
	tagAcdentQueryPeriod aryPeriod[ sizeof(m_aryAcdQueryPeriod) / sizeof(m_aryAcdQueryPeriod[0]) ];
	tagAcdentQueryArea aryArea[ sizeof(m_aryAcdQueryArea) / sizeof(m_aryAcdQueryArea) ];
// 	HANDLE hFind = 0;
// 	WIN32_FIND_DATA objFind;
// 	wchar_t wszFind[ 100 ] = { 0 };

	struct dirent **ppNameList = NULL;
	int iFileCount = 0;
// 	struct   dirent*   pDirlist = NULL;
 	struct   stat   filestat;
 	char   szFile[1024] = { 0 };

	//if( g_pView ) g_pView->DbgShowDataBuf( "Rcv3604", v_szData, v_dwDataLen );

	// 停止上次的处理,并清除状态
	_KillTimer(&g_objTimerMng, ACDUPLOAD_TIMER );
	m_bytAcdQueryAreaCount = m_bytAcdQueryPeridCount = 0;
	memset( &m_objAcdQuerySta, 0, sizeof(m_objAcdQuerySta) );

	// 参数长度检查
	if( v_dwDataLen < 3 )
	{
		bytResType = 3;
		iRet = ERR_PAR;
		goto DEAL3604_FAILED;
	}

	//查询字
	bytQueryType = BYTE( v_szData[0] );

	// 时间段个数
	bytPeridCount = BYTE( v_szData[1] );
	if( 0x7f == bytPeridCount ) bytPeridCount = 0;
	if( bytPeridCount > sizeof(aryPeriod) / sizeof(aryPeriod[0]) )
	{
		bytResType = 3;
		iRet = ERR_PAR;
		goto DEAL3604_FAILED;
	}

	// 参数长度检查
	if( v_dwDataLen < 2 + 10 * DWORD(bytPeridCount) )
	{
		bytResType = 3;
		iRet = ERR_PAR;
		goto DEAL3604_FAILED;
	}

	// 时间段条件
	dw = 2;
	for( byt = 0; byt < bytPeridCount; byt ++ )
	{
		aryPeriod[ byt ].m_bytBeginYear = BYTE( v_szData[dw] );
		aryPeriod[ byt ].m_bytBeginMon = BYTE( v_szData[dw + 1] );
		aryPeriod[ byt ].m_bytBeginDay = BYTE( v_szData[dw + 2] );
		if( 0x7f == v_szData[dw + 3] ) v_szData[dw + 3] = 0;
		aryPeriod[ byt ].m_bytBeginHour = BYTE( v_szData[dw + 3] );
		if( 0x7f == v_szData[dw + 4] ) v_szData[dw + 4] = 0;
		aryPeriod[ byt ].m_wBeginMin = BYTE( v_szData[dw + 4] );
		aryPeriod[ byt ].m_bytEndYear = BYTE( v_szData[dw + 5] );
		aryPeriod[ byt ].m_bytEndMon = BYTE( v_szData[dw + 6] );
		aryPeriod[ byt ].m_bytEndDay = BYTE( v_szData[dw + 7] );
		if( 0x7f == v_szData[dw + 8] ) v_szData[dw + 8] = 0;
		aryPeriod[ byt ].m_bytEndHour = BYTE( v_szData[dw + 8] );
		if( 0x7f == v_szData[dw + 9] ) v_szData[dw + 9] = 0;
		aryPeriod[ byt ].m_wEndMin = BYTE( v_szData[dw + 9] );

		dw += 10;
	}

	// 范围个数
	bytAreaCount = BYTE( v_szData[dw] );
	if( 0x7f == bytAreaCount ) bytAreaCount = 0;
	if( bytAreaCount > 5 )
	{
		bytResType = 3;
		iRet = ERR_PAR;
		goto DEAL3604_FAILED;
	}
	dw ++;

	// 参数长度检查
	if( v_dwDataLen < dw + 16 * DWORD(bytAreaCount) )
	{
		bytResType = 3;
		iRet = ERR_PAR;
		goto DEAL3604_FAILED;
	}

	// 范围条件
	for( byt = 0; byt < bytAreaCount; byt ++ )
	{
		aryArea[ byt ].m_lLonMin = G_Lonlat_4CharToL( v_szData + dw );
		aryArea[ byt ].m_lLatMin = G_Lonlat_4CharToL( v_szData + dw + 4 );
		aryArea[ byt ].m_lLonMax = G_Lonlat_4CharToL( v_szData + dw + 8 );
		aryArea[ byt ].m_lLatMax = G_Lonlat_4CharToL( v_szData + dw + 12 );

		dw += 16;
	}

	// 保存查询条件
	memcpy( m_aryAcdQueryPeriod, aryPeriod, sizeof(m_aryAcdQueryPeriod) );
	memcpy( m_aryAcdQueryArea, aryArea, sizeof(m_aryAcdQueryArea) );
	m_bytAcdQueryPeridCount = bytPeridCount;
	m_bytAcdQueryAreaCount = bytAreaCount;

	// 筛选文件
	iFileCount = scandir( m_szAcdPath, &ppNameList, 0, alphasort );
	if( iFileCount <= 0 || !ppNameList )
	{
		bytResType = 0x7f;
		iRet = 0;
		goto DEAL3604_FAILED;
	}

	while( iFileCount -- > 0 )
	{
		sprintf( szFile, "%s%s", m_szAcdPath, ppNameList[ iFileCount ]->d_name );
		stat( szFile, &filestat );

		if( S_ISDIR(filestat.st_mode) ) goto _ONEFILE_CHKEND;
		if( !S_ISREG(filestat.st_mode) ) goto _ONEFILE_CHKEND;
		if( 14 != strlen(ppNameList[ iFileCount ]->d_name) ) goto _ONEFILE_CHKEND;
		if( strncasecmp("Acd_", ppNameList[ iFileCount ]->d_name, 4) ) goto _ONEFILE_CHKEND;
		
		if( m_objAcdQuerySta.m_wFileTotal < sizeof(m_objAcdQuerySta.m_aryAcdentFile) / sizeof(m_objAcdQuerySta.m_aryAcdentFile[0]) )
		{
			strncpy( (char*)&m_objAcdQuerySta.m_aryAcdentFile[ m_objAcdQuerySta.m_wFileTotal ], szFile, sizeof(m_objAcdQuerySta.m_aryAcdentFile[0]) - 1 );
			++ m_objAcdQuerySta.m_wFileTotal;
		}

_ONEFILE_CHKEND:
		free( ppNameList[ iFileCount ]);
	}
	free( ppNameList );

	//wcscpy( wszFind, DIR2NAME_W );
	//wcscat( wszFind, L"Acd_*.dat" );
// 	pDir = opendir( m_szAcdPath );
// 	if( NULL == pDir )
// 	{
// 		bytResType = 0x7f;
// 		iRet = 0;
// 		goto DEAL3604_FAILED;
// 	}
// 	while( NULL != (pDirlist = readdir(pDir)) )
// 	{
// 		if( 0 == strcmp(pDirlist->d_name, ".") || 0 == strcmp(pDirlist->d_name, "..") )   
// 			continue;
// 
// 		sprintf( szFile, "%s%s", m_szAcdPath, pDirlist->d_name );
// 		stat( szFile, &filestat );
// 
// 		if( S_IDIR(filestat.st_mode) ) continue;
// 		if( !S_ISREG(filestat.st_mode) ) continue;
// 		if( 14 != strlen(pDirlist->d_name) ) continue;
// 		if( strnicmp("Acd_", pDirlist->d_name, 4) ) continue;
// 
// 		if( m_objAcdQuerySta.m_wFileTotal >= sizeof(m_objAcdQuerySta.m_aryAcdentFile) / sizeof(m_objAcdQuerySta.m_aryAcdentFile[0]) )
// 			break;
// 		
// 		strncpy( (char*)&m_objAcdQuerySta.m_aryAcdentFile[ m_objAcdQuerySta.m_wFileTotal ], szFile, sizeof(m_objAcdQuerySta.m_aryAcdentFile[0]) - 1 );
// 		++ m_objAcdQuerySta.m_wFileTotal;
// 	}
// 	closedir( pDir );
// 	pDir = NULL;

	_SetTimer(&g_objTimerMng, ACDUPLOAD_TIMER, ACDENTUPLOAD_INTERVAL, G_TmUploadAcdent );
	P_TmUploadAcdent(); // 立即启动定时器

	return 0;

DEAL3604_FAILED:
//	if( pDir ) closedir( pDir );
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x36, 0x44, buf, sizeof(buf), iBufLen, CVT_8TO7 ); 
//	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, DATASYMB_SMSBACKUP );
	// 080917,根据任务单080899修改,只允许报警数据、位置查询应答和短信通道下发设置的应答走短信通道
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );

	return iRet;
}

#endif
