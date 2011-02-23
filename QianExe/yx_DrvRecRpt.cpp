#include "yx_QianStdAfx.h"
#include "yx_DrvRecRpt.h"

#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 || USE_DRVRECRPT == 11

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-DrvRec   ")

const UINT DRVRECWIN_REUPLOAD_INTERVAL = 30000; // 行驶记录窗口重传间隔,ms
const DWORD DRVWINSNDMAXPERIOD_FSTCHKGPRS = DRVRECWIN_REUPLOAD_INTERVAL * 3 + 8000; // 行驶记录上传多久之后若没有应答,则要第一次检查GPRS网络,ms
const DWORD DRVWINSNDMAXPERIOD_CHKGPRS = 300000; // 行驶记录上传多久之后若没有应答,第一次检查GPRS网络后仍没有应答,要求底层检查GPRS的间隔,ms
const DWORD CHKDRV_INTERVAL = 1000; // 行驶记录检测间隔,1000ms,勿改动
const DWORD AVGSPEEDSAVE_INTERVAL = 60000; // 多久生成一个平均速度,ms

#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 11
const BYTE DRVRECFRAME_DATATYPE = 0x45;
#endif
#if USE_DRVRECRPT == 2
const BYTE DRVRECFRAME_DATATYPE = 0x46;
#endif
#if USE_DRVRECRPT == 3
const BYTE DRVRECFRAME_DATATYPE = 0x47;
#endif
#if USE_DRVRECRPT == 4
const BYTE DRVRECFRAME_DATATYPE = 0x48;
#endif

#pragma pack(4)

struct tagAvgSpeed
{
	float	m_fAvgSpeed; // 米/秒
	char	m_szDate[3]; // 年月日 (仅用于内存,文件中不存在此项)
	char	m_szTime[2]; // 时分
};

#pragma pack()

void *G_ThreadDrvRecSample(void *arg)
{
	g_objDrvRecRpt.P_ThreadDrvRecSample();
}

void G_TmReRptDrvRecWin(void *arg, int len)
{
	g_objDrvRecRpt.P_TmReRptDrvRecWin();
}

void G_TmStopUploadDrvrec(void *arg, int len)
{
	g_objDrvRecRpt.P_TmStopUploadDrvrec();
}

CDrvRecRpt::CDrvRecRpt()
{
	pthread_mutex_init( &m_MutexDrvRec, NULL );
	
	m_objWaitSndDrvRecMng.InitCfg( QUEUE_WAITSNDDRVREC_SIZE, QUEUE_WAITSNDDRVREC_PERIOD );
	memset( &m_objDrvRecSampleCfg, 0, sizeof(m_objDrvRecSampleCfg) );
	
	m_dwDrvCurWinFstSndTime = GetTickCount();
	m_bDrvCurWinChkGprs = false;
	m_bytCurSndDrvRecFrameNum = 0;
	m_bCurSndNeedAns = false;
	memset( m_aryCurSndDrvRecFrameInfo, 0, sizeof(m_aryCurSndDrvRecFrameInfo) );
}

CDrvRecRpt::~CDrvRecRpt()
{
	pthread_mutex_destroy( &m_MutexDrvRec );
}

int CDrvRecRpt::Init()
{
	int iRet = 0;
	Release();
	
	// 先启动行驶记录数据采样线程
	m_bStopDrvSample = false;
	m_bEndDrvSample = false;
	m_bForceRptDrv = false;
		
	if( pthread_create(&m_pthreadDrvSample, NULL, G_ThreadDrvRecSample, NULL) != 0 )
	{
		iRet = ERR_THREAD;
		goto INIT_END;
	}
	
	// 再开启特定的行驶记录采样
	_BeginDrvRecSample( (BOOL)(g_objQianIO.GetDeviceSta() & DEVSTA_ACCVALID) ); // 该函数要在CVeSta对象_Init之后调用
	
INIT_END:
	return iRet;
}

int CDrvRecRpt::Release()
{
	m_bEndDrvSample = true;
	m_bStopDrvSample = true;
}

void CDrvRecRpt::P_ThreadDrvRecSample()
{
	char buf[SOCK_MAXSIZE];
	DWORD dwBufLen = 0;

	tagDrvSampleDatas objSampleDatas; // 保存的行驶数据集,满发送周期后生成一条行驶记录,然后重新采样行驶数据
	tagDrvRecSampleCfg objCurSampleCfg, objNewSampleCfg; // 行驶数据采样的当前配置和新配置

#if USE_DRVRECRPT == 2
	tagSampleData_3746 objOneSampleData; // 保存一次采样的行驶数据
	tagSampleData_3746* arySampleData = objSampleDatas.m_uniSampleDatas.m_arySampleData_3746;
#endif
#if USE_DRVRECRPT == 3
	tagSampleData_3747 objOneSampleData; // 保存一次采样的行驶数据
	tagSampleData_3747* arySampleData = objSampleDatas.m_uniSampleDatas.m_arySampleData_3747;
#endif
#if USE_DRVRECRPT == 11
	tagSampleData_3745_KJ2 objOneSampleData; // 保存一次采样的行驶数据
	tagSampleData_3745_KJ2* arySampleData = objSampleDatas.m_uniSampleDatas.m_arySampleData_3745_KJ2;
#endif
	
	tagGPSData gps(0);
	double dLstLon = 0; // 上次循环时记录的经纬度
	double dLstLat = 0;
	double dDrvDis = 0; // 上次循环->本次循环所行驶的里程
	double dCurSavSpdDis = 0; // 本次若需要计算1分钟平均速度时的1分钟内累加里程
	double dMeterPerCycle = 0; // 每里程脉冲多少米 (标定值换算得到)
	double dAvgSpeed = 0; // 本次循环时生成的平均速度,米/秒
	DWORD dwWait = CHKDRV_INTERVAL; // 线程等待时间,必须为该常量值
	DWORD dwCur = GetTickCount(); // 不要更改
	DWORD dwLstSavSpdTm = dwCur; // 上次保存1分钟平均速度的时刻
	DWORD dwLstSavRecTm = dwCur; // 上次采样行驶数据的时刻
	DWORD dwDeltTm = 1; // 尽可能防止除0异常
	DWORD dwPacketNum = 0; // 推入1条行驶记录到发送队列后得到的队列内记录总数
	DWORD dwLstMeterCycle = 0; // 上次循环时读取的里程脉冲总数
	DWORD dwCurMeterCycle = 0; // 当前循环时读取的里程脉冲总数
	DWORD dwCyclePerSec = 0; // 当前循环时读取的每秒脉冲数
	DWORD dwIoSta = 0;
	BOOL blResult = FALSE;
	WORD wCyclePerKm = 0; // 每公里脉冲数 (标定值)
	BYTE bytAvgSpeed = 0; // 本次循环时生成的平均速度,海里/小时
	BYTE bytLoopCt = 0; // 必须=0
	BYTE bytSta = 0; // 0,初始态; 1,采样态; 2,采样暂停态(第1次检测到); 3,采样暂停态
	bool bMeterDone = false;
	bool bSpdDone = false;

	// 添加行驶数据时的容量限制常量
	const DWORD AVGSPD_MAXCOUNT = sizeof(objSampleDatas.m_aryAvgSpeed) / sizeof(objSampleDatas.m_aryAvgSpeed[0]);
	const DWORD SAMPLEDATA_MAXCOUNT = sizeof(objSampleDatas.m_uniSampleDatas.m_arySampleData_3746)
		/ sizeof(objSampleDatas.m_uniSampleDatas.m_arySampleData_3746[0]);

	// 线程循环前的初始化 (重要)
	dLstLon = dLstLat = 0;
	bytLoopCt = 0;
	bytSta = 0;
	memset( &objSampleDatas, 0, sizeof(objSampleDatas) );
	{
		// 读取复位前的最后有效经纬度
		double dLichen = 0;
		long lLon = 0, lLat = 0;
		g_objCarSta.GetLstValidLonlatAndLichen( lLon, lLat, dLichen );
		dLstLon = lLon / LONLAT_DO_TO_L;
		dLstLat = lLat / LONLAT_DO_TO_L;
#if USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4
		objSampleDatas.m_dDrvDis = dLichen;
#endif
	}

	// 线程循环处理
	while( !m_bEndDrvSample ) 
	{
		usleep(dwWait*1000);	// 固定的循环间隔

		// (1)每次循环前的初始化
		dwCur = GetTickCount();
		dwDeltTm = 1000; // 初始化尽可能防止除0异常
		bytAvgSpeed = 0;
		memset( &objOneSampleData, 0, sizeof(objOneSampleData) );
		memset( &gps, 0, sizeof(gps) );
		GetCurGps( &gps, sizeof(gps), GPS_REAL ); // 获取GPS实时数据

		// (重要) 若ACC无效，强制设为无效GPS，使得后面不参与里程统计，不更新为下次循环时使用的“上次有效位置”
		if( !(g_objQianIO.GetDeviceSta() & DEVSTA_ACCVALID) )
			gps.valid = 'V';
		else
		{
			// 第一次获取到有效GPS时,要与Flash中记录的最后有效位置进行比较,偏移10km以上,则Flash中的记录不使用.
			static bool sta_bFstJugLstValid = true;
			if( sta_bFstJugLstValid && 'A' == gps.valid )
			{
				if( G_CalGeoDis2(dLstLon, dLstLat, gps.longitude, gps.latitude) > 10000 )
				{
					dLstLon = dLstLat = 0;
				}

				sta_bFstJugLstValid = false;
			}
		}

		if( 0 == bytSta ) // 若是初始状态
		{			
			// 清除旧的行驶记录数据,但是保留累积行驶里程 (重要)
			double dDis = objSampleDatas.m_dDrvDis;
			memset( &objSampleDatas, 0, sizeof(objSampleDatas) );
			objSampleDatas.m_dDrvDis = dDis;

			dwLstSavSpdTm = dwLstSavRecTm = dwCur; // 重新初始化各处理时刻
			_GetDrvRecSampleCfg( objNewSampleCfg ); // 更新一次采样配置
			objCurSampleCfg = objNewSampleCfg; // 当前配置使用最新的配置
		}

		if( 0 == bytLoopCt % 20 ) // 否则,每20秒
		{
#ifdef DIS_SPD_USEIO
			// 更新一次标定值 (初始时也在此更新)
			GetImpCfg( &wCyclePerKm, sizeof(wCyclePerKm),
				offsetof(tagImportantCfg, m_uni1QBiaodingCfg.m_obj1QBiaodingCfg.m_wCyclePerKm), sizeof(wCyclePerKm) );
			if( wCyclePerKm > 50 )
			{
				dMeterPerCycle = 1000.0 / wCyclePerKm;  // 注意保证wCyclePerKm大于0,以防止除0异常
			}
			else
			{
				dMeterPerCycle = 0;
			}
#endif
			// 若是采样态,则更新一次采样配置
			if( 1 == bytSta ) _GetDrvRecSampleCfg( objNewSampleCfg );
		}


		// (2)更新当前状态
		if( !m_bStopDrvSample ) // 若不需要中止采样
		{
			switch( bytSta )
			{
			case 0:
			case 1:
				bytSta = 1; // 采样态
				break;

			case 2: // 第一次暂停态
			case 3: // 完全暂停态
			default:
				bytSta = 0; // 初始态
			}
		}
		else
		{
			m_bStopDrvSample = false;

			// 判断是否第一次暂停
			switch( bytSta )
			{
			case 2:
			case 3:
				bytSta = 3;
				break;

			default:
				bytSta = 2;
			}
		}


		// (3)计算自上次循环到现在的里程
		dDrvDis = 0;
#ifdef DIS_SPD_USEIO
		bMeterDone = false;

		// 先使用里程脉冲
		blResult = g_objQianIO.IN_QueryMeterCycle( dwCurMeterCycle );
		if( blResult )
		{
			if( dMeterPerCycle > 0.01 ) // 若标定值有效
			{
				dDrvDis = (dwCurMeterCycle - dwLstMeterCycle) * dMeterPerCycle;
				bMeterDone = true;
			}
			dwLstMeterCycle = dwCurMeterCycle;
		}
#endif
		
		// 从里程脉冲获取失败,再考虑GPS
		if( !bMeterDone && 'A' == gps.valid && dLstLon > 0.0001 && dLstLat > 0.0001 )
			// 若上次经纬度有效,且本次GPS有效(此时ACC有效,因为前面的代码在ACC无效时把GPS也置为无效了)
		{
			dDrvDis = G_CalGeoDis2( dLstLon, dLstLat, gps.longitude, gps.latitude );
		}


		// 现在更新上次位置 (dLstLon、dLstLat在本次循环的后面不用了)
		if( 'A' == gps.valid )
			// 当前GPS有效,且ACC有效时
		{
			dLstLon = gps.longitude;
			dLstLat = gps.latitude;

			static DWORD sta_dwCt = 0;
			if( 0 == ++ sta_dwCt % 600 ) // ACC有效和GPS有效状态下，每10分钟，保存一次最后有效位置，以最大可能确保里程累加准确
			{
				g_objCarSta.SavLstValidLonlatAndLichen(
					long(gps.longitude * LONLAT_DO_TO_L),
					long(gps.latitude * LONLAT_DO_TO_L),
					objSampleDatas.m_dDrvDis );
				
				// 让主线程做文件的写入工作
				QIANMSG msg;
				msg.m_lMsgType = MSG_SAVRECSTA;
				if( msgsnd(g_MsgID, (void*)&msg, MSG_MAXLEN, 0) == -1 )
					perror("send MSG_SAVRECSTA faile:");
				else
					PRTMSG(MSG_DBG, "send MSG_SAVRECSTA\n");
			}
		}


		// (4)累加行驶里程
		objSampleDatas.m_dDrvDis += dDrvDis;


		// (5)判断是否要采样
		if( 1 == bytSta || 2 == bytSta ) // 若是采样态或是第一次暂停采样态
		{
			// (5_1)判断是否达到采样周期,达到则要生成1条行驶数据
			dwDeltTm = dwCur - dwLstSavRecTm;
			if( objCurSampleCfg.m_wSampleCycle > 0 // 若需要采样
				&&
				dwDeltTm >= objCurSampleCfg.m_wSampleCycle * DWORD(1000) ) // 且达到采样周期
			{
				if( dwDeltTm < 1 ) dwDeltTm = 1; // 防止除0错误

				// 生成1条行驶数据
				objOneSampleData.m_bytYear = BYTE(gps.nYear % 1000);
				objOneSampleData.m_bytMon = BYTE(gps.nMonth);
				objOneSampleData.m_bytDay = BYTE(gps.nDay);
				objOneSampleData.m_bytHour = BYTE(gps.nHour);
				objOneSampleData.m_bytMin = BYTE(gps.nMinute);
				objOneSampleData.m_bytSec = BYTE(gps.nSecond);
				{
					WORD wCurDriverNo = g_objCarSta.GetDriverNo();
					objOneSampleData.m_szDriverNo[0] = BYTE(wCurDriverNo / 0x100 );
					objOneSampleData.m_szDriverNo[1] = BYTE(wCurDriverNo % 0x100 );
				}
#ifdef DIS_SPD_USEIO
				bSpdDone = false;

				// 先通过里程脉冲
				if( dMeterPerCycle > 0.01 ) // 若标定值有效
				{
					if( g_objQianIO.IN_QueryIOSpeed(dwCyclePerSec) )
					{
						objOneSampleData.m_bytSpeed = BYTE(dwCyclePerSec * dMeterPerCycle * MIMIAO_TO_HAIHOUR);
						bSpdDone = true;
					}
				}
				
				// 失败再使用GPS
				if( !bSpdDone )
				{
					objOneSampleData.m_bytSpeed = BYTE(gps.speed * MIMIAO_TO_HAIHOUR);
				}
#else
				objOneSampleData.m_bytSpeed = BYTE(gps.speed * MIMIAO_TO_HAIHOUR);
#endif
				objOneSampleData.m_bytDir = BYTE(gps.direction / 3) + 1;

				g_objQianIO.IN_QueryIOSta( dwIoSta );				
				objOneSampleData.m_bytIO = ~BYTE(dwIoSta % 0x100); 

#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 4
				if( g_objCarSta.GetVeSta() & VESTA_HEAVY )
					objOneSampleData.m_bytIO |= 0x08;
				else
					objOneSampleData.m_bytIO &= 0xf7;
#endif
				// 获取状态信息
				{
					// 协议定义:
					// 状态字：H7 H6 H5 H4 H3 H2 H1 H0
					//		   L7 L6 L5 L4 L3 L2 L1 L0
					// H0：GPRS在线情况  0不在线        1在线
					// H1：发生碰撞       0 无碰撞       1 碰撞
					// H2：发生侧翻       0无侧翻       1 侧翻
					// H3：欠压状态       0 无欠压       1欠压
					// H4：主电断电状态   0 主电未断电   1主电断电
					// H5：抢劫报警状态   0无抢劫报警   1发生抢劫报警
					// H6：非法启动状态   0 无非法启动   1 发生非法启动
					// H7：保留
					// 
					// L1、L0经纬度坐标:
					// 0  东经北纬  1  东经南纬  2  西经北纬  3  西经南纬
					// L3、L2 GPS模块定位状态：
					// 0：未定位    1：2D定位    2：3D定位   3 保留
					// L5、L4 GPS模块工作状态：
					// 0：正常      1：省电       2：通讯异常  3 保留
					// L6、L7 保留
					// 其他位保留待定，设置为0。
					DWORD dwAlertSta = g_objMonAlert.GetAlertSta();
					DWORD dwDevSta = g_objQianIO.GetDeviceSta();
					BYTE bytH = 0, bytL = 0;
					if( g_objSock.IsOnline() ) bytH |= 0x01; // 网络在线情况
					if( dwAlertSta & ALERT_TYPE_BUMP ) bytH |= 0x02; // 碰撞报警
					if( dwAlertSta & ALERT_TYPE_OVERTURN ) bytH |= 0x04; // 侧翻报警
					if( dwAlertSta & ALERT_TYPE_POWBROWNOUT ) bytH |= 0x08; // 欠压状态
					if( dwAlertSta & ALERT_TYPE_POWOFF ) bytH |= 0x10; // 断电状态
					if( dwAlertSta & ALERT_TYPE_FOOT ) bytH |= 0x20; // 抢劫报警
					if( dwAlertSta & ALERT_TYPE_DRIVE ) bytH |= 0x40; // 非法启动报警
					bytL |= 0; // 暂时都强制使用"东经北纬"
					if( '3' == gps.m_cFixType ) bytL |= 0x08; // 定位类型
					else if( '2' == gps.m_cFixType ) bytL |= 0x04;
					else bytL |= 0;
					if( dwDevSta & DEVSTA_GPSON ) bytL |= 0; // GPS模块状态
					else bytL |= 0x10;

					objOneSampleData.m_bytSta1 = bytH;
					objOneSampleData.m_bytSta2 = bytL;
				}

				G_Lonlat_DTo4Char( gps.longitude, objOneSampleData.m_szLon);
				G_Lonlat_DTo4Char( gps.latitude, objOneSampleData.m_szLat);

#if USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3
				// 协议定义
				// bit7~Bit0: 0表示无效，1表示有效
				// Bit0: 碰撞检测位
				// Bit1：侧翻检测位
				// Bit2：欠压检测位
				// Bit3：断电检测位
				// Bit4：抢劫按钮检测位
				// (不保险,放弃) objOneSampleData.m_bytAlertIO = BYTE( ~(dwIoSta >> 9) & 0x1f ); // 根据驱动返回值定义和协议定义,可这样快速处理
				objOneSampleData.m_bytAlertIO = 0;
				if( !(dwIoSta & 0x200) ) objOneSampleData.m_bytAlertIO |= 0x01; //碰撞IO
				if( !(dwIoSta & 0x400) ) objOneSampleData.m_bytAlertIO |= 0x02; //侧翻IO
				if( !(dwIoSta & 0x800) ) objOneSampleData.m_bytAlertIO |= 0x04; //欠压IO
				if( dwIoSta & 0x1000 ) objOneSampleData.m_bytAlertIO |= 0x08; //断电IO
				if( dwIoSta & 0x2000 ) objOneSampleData.m_bytAlertIO |= 0x10; //抢劫IO
				objOneSampleData.m_bytHeightCount = 0; // 暂时不考虑载重传感器
				memset( objOneSampleData.m_aryHeightIO, 0, sizeof(objOneSampleData.m_aryHeightIO) ); // 传感器数据暂清0
#endif
			
#if USE_DRVRECRPT == 11
				objOneSampleData.m_bytPDop = BYTE(gps.m_bytPDop);
				objOneSampleData.m_bytHDop = BYTE(gps.m_bytHDop);
				objOneSampleData.m_bytVDop = BYTE(gps.m_bytVDop);
#endif

				// 保存行驶数据
				if( objSampleDatas.m_bytSampleDataCount < SAMPLEDATA_MAXCOUNT ) // 保险起见
				{
					arySampleData[ objSampleDatas.m_bytSampleDataCount ++ ]
						= objOneSampleData;
				}

				dwLstSavRecTm = dwCur; // 更新上次采样行驶数据的时刻
			}

			// (5_2)累加平均速度计算周期内的里程,并判断是否要生成平均速度
			dCurSavSpdDis += dDrvDis;
			dwDeltTm = dwCur - dwLstSavSpdTm;
			if( dwDeltTm >= AVGSPEEDSAVE_INTERVAL ) // 若距离上次计算平均速度的时间达到了计算周期
			{
				if( dwDeltTm < 1 ) dwDeltTm = 1; // 防止除0错误
				
				// 生成1分钟平均速度并保存
				dAvgSpeed = dCurSavSpdDis / (dwDeltTm / 1000.0);
				bytAvgSpeed = BYTE(dAvgSpeed * MIMIAO_TO_HAIHOUR);
				//if( objSampleDatas.m_bytAvgSpeedCount < AVGSPD_MAXCOUNT ) // 保险起见
				{
					objSampleDatas.m_aryAvgSpeed[ objSampleDatas.m_bytAvgSpeedCount ++ ] = bytAvgSpeed;
				}

				dCurSavSpdDis = 0; // 清零平均速度累加里程
				dwLstSavSpdTm = dwCur; // 更新上次平均速度生成时刻
			}
		}


		// (6)判断是否要生成一条行驶记录帧
		if( objCurSampleCfg.m_bytSndCycle > 0 ) // 若要发送
		{
			// 判断是否要强制组帧并发送
			bool bForceMakeWin =
				2 == bytSta // 第1次暂停采样态
				||
				m_bForceRptDrv // 或 外部请求立刻生成
				||
				(objNewSampleCfg.m_wSampleCycle != objCurSampleCfg.m_wSampleCycle
				|| objNewSampleCfg.m_bytSndCycle != objCurSampleCfg.m_bytSndCycle
				|| objNewSampleCfg.m_bytSndWinSize != objCurSampleCfg.m_bytSndWinSize); // 或 配置要发生更改

			if( m_bForceRptDrv == true ) m_bForceRptDrv = false;

			if( objSampleDatas.m_bytSampleDataCount >= objCurSampleCfg.m_bytSndCycle // 若到达发送周期
				|| bForceMakeWin ) // 或是需要强行组帧并发送
			{
#if USE_DRVRECRPT == 3 || USE_DRVRECRPT == 11

#if USE_DRVRECRPT == 3
				const double OIL_MVESPD = 1.852 * 2 / 3.6; // 米/秒
#endif
#if USE_DRVRECRPT == 11
				const double OIL_MVESPD = 15 / 3.6; // 米/秒
#endif
				
				// 获取当前相关状态
				static bool sta_bAccValid = true;
				bool bGpsValid = gps.valid == 'A' ? true : false;
				bool bAccValid = g_objQianIO.GetDeviceSta() & DEVSTA_ACCVALID ? true : false;

				
#if USE_OIL == 1

				// 确定油耗状态
#if USE_DRVRECRPT == 3
				// 千里眼协议: 油耗值的状态 1 D7～D0，表示油耗采样时的状态 (以下各种状态只有一位有效)
				// D0：0正常行驶，1车辆点火
				// D1：0表示正常，1 GPS定位且速度大于2海里/小时
				// D2：0表示定位，1不定位
				// D3：0表示正常，1 GPS定位且速度小于2海里/小时
				// D4：0表示正常，1停车（ACC无效）或ACC有效仪表盘却没有上电
				// D5：0表示正常，1油耗盒没接
				// D6：0表示正常，1油耗值不在设置范围内（即值无效）

				// 部分信号不能采集,不处理
				if( !bAccValid ) objSampleDatas.m_bytOilSta = 0x10;
				else if( !bGpsValid ) objSampleDatas.m_bytOilSta = 0x04;
				else
				{
					if( gps.speed >= OIL_MVESPD ) objSampleDatas.m_bytOilSta = 0x02;
					else objSampleDatas.m_bytOilSta = 0x08;
				}
#endif

#if USE_DRVRECRPT == 11
				// 2客协议
				// D7～D0，表示油耗采样时的状态
				// 0x01: 点火 (事件触发)
				// 0x02: 已通电，已定位，速度>15
				// 0x04: 已通电，未定位
				// 0x08: 已通电，已定位，速度>15
				// 0x10: 数据无效的，未通电
				if( !bAccValid ) objSampleDatas.m_bytOilSta = 0x10;
				else if( bAccValid && !sta_bAccValid ) objSampleDatas.m_bytOilSta = 0x01;
				else if( bAccValid && !bGpsValid ) objSampleDatas.m_bytOilSta = 0x04;
				else if( bAccValid && bGpsValid && gps.speed >= OIL_MVESPD ) objSampleDatas.m_bytOilSta = 0x02;
				else if( bAccValid && bGpsValid && gps.speed < OIL_MVESPD ) objSampleDatas.m_bytOilSta = 0x08;
#endif

#endif

				// 获取油耗值
				BOOL blGetOilSucc = FALSE; // 初始化为获取失败
// 				if( bAccValid )
// 				{
#if USE_OIL == 1
					blGetOilSucc = g_objQianIO.IN_QueryOil( objSampleDatas.m_bytOilAD, objSampleDatas.m_bytPowAD );
#endif
#if USE_OIL == 2
					blGetOilSucc = g_objOil.GetOilPowAndSta( &objSampleDatas.m_bytOilAD,
						&objSampleDatas.m_bytPowAD, &objSampleDatas.m_bytOilSta );
#endif
// 				}
 				if( blGetOilSucc )
				{
#if USE_OIL == 1
					// 保存最新状态
					g_objCarSta.SavOil( objSampleDatas.m_bytOilAD, objSampleDatas.m_bytPowAD, objSampleDatas.m_bytOilSta );
#endif
				}
				else
				{
					// 提取状态中保存的上次有效油耗数据
					g_objCarSta.GetOil( &objSampleDatas.m_bytOilAD, &objSampleDatas.m_bytPowAD, &objSampleDatas.m_bytOilSta );
				}

				// 根据油耗配置作调整
				{
					tag1QBiaoding objBd;	
					if( !GetImpCfg( &objBd, sizeof(objBd), offsetof(tagImportantCfg, m_uni1QBiaodingCfg.m_obj1QBiaodingCfg), sizeof(objBd) ) )
					{
						if( objBd.m_bReverse )
						{
							objSampleDatas.m_bytOilAD =  255 - objSampleDatas.m_bytOilAD; // xun，要反转也仅仅反转油耗AD
						}
						
						if(  objBd.m_bFiltrate && (objSampleDatas.m_bytOilAD > objBd.m_bytHiVal || objSampleDatas.m_bytOilAD < objBd.m_bytLoVal) )
						{
							objSampleDatas.m_bytOilSta = 0x40; // 范围无效
						}
					}
				}

				// 更新ACC状态记录
				sta_bAccValid = bAccValid;
#endif
				
				// 当前行驶数据组成1个行驶记录帧,推入发送队列
				if( !_MakeOneDrvrecFrame(objSampleDatas, objCurSampleCfg.m_wSampleCycle, buf, sizeof(buf), dwBufLen) )
					// 若组帧成功
				{
					m_objWaitSndDrvRecMng.PushData(LV8, dwBufLen, buf, dwPacketNum);
					PRTMSG(MSG_DBG, "m_objWaitSndDrvRecMng push one frame\n");
				}
				else // 若不成功
				{ // 不作处理 (将丢弃该帧,因为若是内存不足,再缓存没有意义;否则,可能是空数据,也不需要缓存)
				}
				// 无论是否组帧成功都要通知处理,可能上次组帧后到现在还没有采样过,也是正常
				
				QIANMSG msg;
				msg.m_lMsgType = MSG_DRVECDTOSND;
				msg.m_uszMsgBuf[0] = (char)(bForceMakeWin ? 1 : 0);
				if( msgsnd(g_MsgID, (void*)&msg, MSG_MAXLEN, 0) == -1 )
					perror("send MSG_DRVECDTOSND faile:");
				else
					PRTMSG(MSG_DBG, "send MSG_DRVECDTOSND\n");

				double dLeaveDis = 0;
#if USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3
				// 保留总里程
				dLeaveDis = objSampleDatas.m_dDrvDis;
#endif
#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 11
				// 保留组帧时因为精度原因而"丢失"的里程
				dLeaveDis = objSampleDatas.m_dDrvDis - WORD(objSampleDatas.m_dDrvDis);
#endif
				
				// 清除当前采样记录集
				memset( &objSampleDatas, 0, sizeof(objSampleDatas) );
				
				// 恢复总里程,或补充组帧时丢失的里程
				objSampleDatas.m_dDrvDis = dLeaveDis;

				// 生成行驶记录帧后,要更新当前的采样配置 (因为采样配置发生变化时会先把旧的行驶数据生成行驶记录帧)
				objCurSampleCfg = objNewSampleCfg;
			}
		}

		// 当前配置使用最新的配置,如果有更新的话
		objCurSampleCfg = objNewSampleCfg;

		// 修正等待时间 (为了尽可能的确保定时准确)
		{
			DWORD dwDelt = GetTickCount() - dwCur;
			if( dwDelt + 200 < CHKDRV_INTERVAL ) // 至少预留200ms
				dwWait = CHKDRV_INTERVAL - dwDelt;
			else
				dwWait = CHKDRV_INTERVAL;
		}

		// 递增循环次数
		++ bytLoopCt;
	}

	// 线程退出前,将最后的有效经纬度和总里程保存到状态模块中
	g_objCarSta.SavLstValidLonlatAndLichen( 
					long(dLstLon * LONLAT_DO_TO_L),
					long(dLstLat * LONLAT_DO_TO_L),
					objSampleDatas.m_dDrvDis );
}

void CDrvRecRpt::P_TmReRptDrvRecWin()
{
	_SndDrvrecWin( true );
}

void CDrvRecRpt::P_TmStopUploadDrvrec()
{
	_KillTimer(&g_objTimerMng, DRVREC_STOPUPLOAD_TIMER );
	_StopDrvRecSample();
}

void CDrvRecRpt::DrvRecToSnd( bool v_bNeedSndWin )
{
	#if DRVREC_SNDTYPE == 1
///////// { 方式1: 积满一个窗口才发送
	// 若还有未发送完的窗口,则退出发送流程 (此时已由定时器控制重传)
	if( !_IsCurSndDrvRecWinEmpty() ) return;

	// 以下是没有正在发送的窗口的处理,此时重传定时器应该已经关闭


	// 读取配置
	tagDrvRecSampleCfg objDrvRecSampleCfg;
	_GetDrvRecSampleCfg( objDrvRecSampleCfg );

	// 到达发送窗口判断
	bool bNeedSndWin = false;
	if( v_bNeedSndWin ) bNeedSndWin = true; // 若强制作为一个窗口发送
	else if( m_objWaitSndDrvRecMng.GetPacketCount() >= objDrvRecSampleCfg.m_bytSndWinSize ) bNeedSndWin = true;

	// 若到达发送窗口
	if( bNeedSndWin )
	{
		// 生成一个新窗口数据
		_FillCurSndDrvRecWin();

		// 发送一个窗口的数据
		_SndDrvrecWin( false );
	}
///////// } 方式1: 积满一个窗口才发送
#endif


#if DRVREC_SNDTYPE == 2
///////// { 方式2: 每帧发送,满一个窗口等待应答
	static tagDrvRecSampleCfg sta_objDrvRecSampleCfg;
	static BYTE sta_bytWinId = 0;

	if( m_bytCurSndDrvRecFrameNum < 1 ) // 若此时既没有未传完的旧窗口,也没有发送过新的帧
	{
		// 刷新配置
		_GetDrvRecSampleCfg( sta_objDrvRecSampleCfg );
		m_bCurSndNeedAns = sta_objDrvRecSampleCfg.m_bytSndWinSize > 0;

		// 保险操作
		_KillTimer(&g_objTimerMng, DRVRECWIN_REUPLOAD_TIMER );
	}
	else if( m_bCurSndNeedAns && m_bytCurSndDrvRecFrameNum >= sta_objDrvRecSampleCfg.m_bytSndWinSize )
		// 若当前窗口要可靠传输且正在等待应答
	{
		return;
	}

	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	char* pSndBuf = NULL;
	DWORD dwLen = 0;
	DWORD dwPushTm = 0;
	int iResult = 0;
	BYTE bytLvl = 0;
	BYTE bytFillNum = 0;

	while( m_bytCurSndDrvRecFrameNum < sta_objDrvRecSampleCfg.m_bytSndWinSize // 若还未发满1个窗口
		|| 0 == sta_objDrvRecSampleCfg.m_bytSndWinSize ) // 或不需要可靠传输
	{
		// 提取一帧数据
		if( m_objWaitSndDrvRecMng.PopData( bytLvl, sizeof(buf), dwLen, buf, dwPushTm ) )
		{
			PRTMSG(MSG_DBG, "m_objWaitSndDrvRecMng pop no data\n");
			break;
		}

		if( dwLen < 1 )
			continue;

		pSndBuf = new char[dwLen];
		if( !pSndBuf ) break;
		memcpy( pSndBuf, buf, dwLen );

		// 将剩余信息填充完毕,若是可靠传输要缓存,并且若是最后一帧还要打开重传定时器
		if( m_bCurSndNeedAns ) // 若需要可靠传输
		{
			pSndBuf[0] |= 0x42; // 可靠传输
			pSndBuf[0] |= sta_bytWinId ? 0x08 : 0; // 窗口标志ID
			if( m_bytCurSndDrvRecFrameNum + 1 >= sta_objDrvRecSampleCfg.m_bytSndWinSize ) // 若是本窗口最后1帧
			{
				pSndBuf[0] |= 0x01; // 最后: 末帧要应答
				sta_bytWinId = ~sta_bytWinId; // 更换窗口标志

				// 打开重传定时器
				_SetTimer(&g_objTimerMng, DRVRECWIN_REUPLOAD_TIMER, DRVRECWIN_REUPLOAD_INTERVAL, G_TmReRptDrvRecWin);
				m_dwDrvCurWinFstSndTime = GetTickCount();
				m_bDrvCurWinChkGprs = false;
			}
			pSndBuf[1] = sta_objDrvRecSampleCfg.m_bytSndWinSize; // 窗口大小
			pSndBuf[2] = m_bytCurSndDrvRecFrameNum; // 帧序号

			// 可靠传输时要缓存已发送帧
			if( m_bytCurSndDrvRecFrameNum < sizeof(m_aryCurSndDrvRecFrameInfo) / sizeof(m_aryCurSndDrvRecFrameInfo[0]) )
			{
				m_aryCurSndDrvRecFrameInfo[m_bytCurSndDrvRecFrameNum][0] = dwLen;
				m_aryCurSndDrvRecFrameInfo[m_bytCurSndDrvRecFrameNum][1] = DWORD(pSndBuf);
				++ m_bytCurSndDrvRecFrameNum;
			}
		}
		else
		{
			pSndBuf[0] |= 0x40;
			pSndBuf[1] = 0;
			pSndBuf[2] = 0;
		}

		// 立即发送该帧
		iResult = g_objSms.MakeSmsFrame( pSndBuf, int(dwLen), 0x37, DRVRECFRAME_DATATYPE,
			buf, sizeof(buf), iBufLen, CVT_8TO7 );
		if( !iResult )
		{
			iResult = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV8 );
		}
	}
///////// } 方式2: 每帧发送,满一个窗口等待应答
#endif
}

void CDrvRecRpt::RecStaSav()
{
	g_objCarSta.SavDrvRecSta( false );
}

void CDrvRecRpt::_BeginDrvRecSample( BOOL v_blAccOn )
{
	// 获取最新配置
	tag2QDriveRecCfg objDrvRecCfg;
	objDrvRecCfg.Init();
	GetSecCfg( &objDrvRecCfg, sizeof(objDrvRecCfg),
		offsetof(tagSecondCfg, m_uni2QDriveRecCfg.m_obj2QDriveRecCfg), sizeof(objDrvRecCfg) );
	
	// (1)修改内存中的采样发送配置
	tagDrvRecSampleCfg objDrvSampleCfg;
	WORD wSndPeriod = 1; // (分钟)
	if( v_blAccOn )
	{
		objDrvSampleCfg.m_wSampleCycle = objDrvRecCfg.m_wOnSampCycle;
		objDrvSampleCfg.m_bytSndCycle = objDrvRecCfg.m_bytOnSndCycle;
		wSndPeriod = objDrvRecCfg.m_wOnPeriod;
	}
	else
	{
		objDrvSampleCfg.m_wSampleCycle = objDrvRecCfg.m_wOffSampCycle;
		objDrvSampleCfg.m_bytSndCycle = objDrvRecCfg.m_bytOffSndCycle;
		wSndPeriod = objDrvRecCfg.m_wOffPeriod;
	}
	objDrvSampleCfg.m_bytSndWinSize = objDrvRecCfg.m_bytSndWinSize;
	_SetDrvRecSampleCfg( objDrvSampleCfg );
	m_bForceRptDrv = true; // 强制生成3745行驶记录帧并上传

	// (2)判断是否要进行采样
	bool bNeedSample = false;
	if( v_blAccOn && (objDrvRecCfg.m_wOnSampCycle > 0 && objDrvRecCfg.m_bytOnSndCycle > 0) )
	{
		bNeedSample = true;
	}
	else if( !v_blAccOn && (objDrvRecCfg.m_wOffSampCycle > 0 && objDrvRecCfg.m_bytOffSndCycle > 0) )
	{
		bNeedSample = true;
	}
	else
	{
		bNeedSample = false;
	}
	
	if( bNeedSample )
	{
		m_bStopDrvSample = false;
		_KillTimer(&g_objTimerMng, DRVREC_STOPUPLOAD_TIMER );
		if( 0xffff != wSndPeriod )
			_SetTimer(&g_objTimerMng, DRVREC_STOPUPLOAD_TIMER, DWORD(wSndPeriod) * 60000, G_TmStopUploadDrvrec);
	}
	else
	{
		m_bStopDrvSample = true;
	}
}

void CDrvRecRpt::_StopDrvRecSample()
{
	m_bStopDrvSample = true;
}

void CDrvRecRpt::_GetDrvRecSampleCfg( tagDrvRecSampleCfg &v_objDrvRecSampleCfg )
{
	pthread_mutex_lock( &m_MutexDrvRec );

	v_objDrvRecSampleCfg = m_objDrvRecSampleCfg;

	pthread_mutex_unlock( &m_MutexDrvRec );
}

void CDrvRecRpt::_SetDrvRecSampleCfg( const tagDrvRecSampleCfg &v_objDrvRecSampleCfg  )
{
	pthread_mutex_lock( &m_MutexDrvRec );

	m_objDrvRecSampleCfg = v_objDrvRecSampleCfg;

	pthread_mutex_unlock( &m_MutexDrvRec );
}

#if DRVREC_SNDTYPE == 1
bool CDrvRecRpt::_IsCurSndDrvRecWinEmpty()
{
	return m_bytCurSndDrvRecFrameNum < 1;
}

// 填充一个窗口的数据,窗口大小根据设置,但是以实际能从队列中读到的为准,并且不超过发送窗口缓冲区的范围
// 如果不是可靠传输,则填充的是一组发送数据,在不超过发送缓冲区的范围内,尽可能多的读取
void CDrvRecRpt::_FillCurSndDrvRecWin()
{
	// 保险操作,清除发送状态,实际上进入该函数前发送窗口应该是空的或者该窗口是已发送完毕的
	_ClearCurSndDrvRecWin();
	
	// 读取配置,更新当前发送状态
	tagDrvRecSampleCfg objDrvSampleCfg;
	_GetDrvRecSampleCfg( objDrvSampleCfg );
	m_bCurSndNeedAns = objDrvSampleCfg.m_bytSndWinSize > 0 ? true : false;
	
	// 确定本次要处理的帧数(可靠传输时就是一个窗口)
	BYTE bytFillNum = 0;
	if( m_bCurSndNeedAns )
	{
		bytFillNum = objDrvSampleCfg.m_bytSndWinSize;
		if( bytFillNum > sizeof(m_aryCurSndDrvRecFrameInfo) / sizeof(m_aryCurSndDrvRecFrameInfo[0]) )
			bytFillNum = sizeof(m_aryCurSndDrvRecFrameInfo) / sizeof(m_aryCurSndDrvRecFrameInfo[0]); // 保险操作
	}
	else bytFillNum = 32;
	
	// 队列中的数据填充至当前发送窗口缓冲区
	char buf[ SOCK_MAXSIZE ];
	BYTE bytLvl = 0;
	DWORD dwLen = 0;
	DWORD dwPushTm = 0;
	char* pTemp = NULL;
	for( BYTE byt = 0; byt < bytFillNum; byt ++ )
	{
		if( m_objWaitSndDrvRecMng.PopData( bytLvl, sizeof(buf), dwLen, buf, dwPushTm ) ) break;
		if( dwLen < 1 ) continue;
		
		if( byt < sizeof(m_aryCurSndDrvRecFrameInfo) / sizeof(m_aryCurSndDrvRecFrameInfo[0]) )
		{
			pTemp = new char[dwLen];
			if( !pTemp ) break;
			
			memcpy( pTemp, buf, dwLen );
			m_aryCurSndDrvRecFrameInfo[byt][0] = dwLen;
			m_aryCurSndDrvRecFrameInfo[byt][1] = DWORD(pTemp);
		}
	}
	
	// 当前发送的总帧数等于实际填充的帧数
	m_bytCurSndDrvRecFrameNum = byt;
}
#endif

void CDrvRecRpt::_ClearCurSndDrvRecWin()
{
	for( DWORD dw = 0; dw < sizeof(m_aryCurSndDrvRecFrameInfo) / sizeof(m_aryCurSndDrvRecFrameInfo[0]); dw ++ )
	{
		delete[] (char*) (m_aryCurSndDrvRecFrameInfo[dw][1]);
	}
	memset( m_aryCurSndDrvRecFrameInfo, 0, sizeof(m_aryCurSndDrvRecFrameInfo) );
	m_bytCurSndDrvRecFrameNum = 0;
	m_bCurSndNeedAns = false;
}

int CDrvRecRpt::_MakeOneDrvrecFrame( const tagDrvSampleDatas &v_objDrvSampleDatas, const WORD v_wSampleCycle,
			char *const v_szBuf, const size_t v_sizBuf, DWORD &v_dwLen )
{
	DWORD dw = 0;
	DWORD dwTemp = 0;
	WORD wTemp = 0;
	BYTE byt = 0;
	BYTE bytOilSta = 0;

	// 预清除
	memset( v_szBuf, 0, v_sizBuf );
	v_dwLen = 0;

	// 判断是否存在行驶数据
	if( v_objDrvSampleDatas.m_bytSampleDataCount < 1 )
	{
		PRTMSG(MSG_ERR, "no SampleData\n");
		return ERR_PAR;
	}
	
	// 缓存空间检查
	if( v_sizBuf < 1000 ) // 下限为一个估计上的极大值
	{
		return ERR_SPACELACK;
	}

	
	// 生成一帧 (处理油耗标记,然后从里程开始赋值，前面的其他值在发送时赋值)

	// 里程
#if USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4
	dwTemp = htonl( DWORD(v_objDrvSampleDatas.m_dDrvDis) );
	memcpy( v_szBuf + 3, &dwTemp, 4); // 总里程
	dw = 7;
#endif
#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 11
	wTemp = htons( WORD(v_objDrvSampleDatas.m_dDrvDis) );
	memcpy( v_szBuf + 3, &wTemp, 2); // 里程
	dw = 5;
#endif

	// 油耗
#if USE_DRVRECRPT == 3 || USE_DRVRECRPT == 11
	bytOilSta = v_objDrvSampleDatas.m_bytOilSta;
#if USE_DRVRECRPT == 11
	v_szBuf[ 0 ] |= char( 0x80 ); // 油耗标志位置1
#endif
	v_szBuf[ dw ++ ] = char( v_objDrvSampleDatas.m_bytOilAD );
	v_szBuf[ dw ++ ] = char( v_objDrvSampleDatas.m_bytPowAD );
	v_szBuf[ dw ++ ] = char( bytOilSta );
#endif

	// 采样间隔
	wTemp = htons( v_wSampleCycle );
	memcpy( v_szBuf + dw, &wTemp, 2 );
	dw += 2;

	// 行驶数据个数
	v_szBuf[ dw ++ ] = char( v_objDrvSampleDatas.m_bytSampleDataCount );
	
	// 行驶数据
	for( byt = 0; byt < v_objDrvSampleDatas.m_bytSampleDataCount; byt ++ )
	{
#if USE_DRVRECRPT == 2
		// 基础大小
		dwTemp = sizeof(tagSampleData_3746);

		// 去除多余的载重字段
		if( v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746[byt].m_bytHeightCount
			>
			sizeof(v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746[byt].m_aryHeightIO)
			/
			sizeof(v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746[byt].m_aryHeightIO[0]) )
		{ // 保险起见
			memset( (char*)&v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746[byt].m_bytHeightCount, 1,
				sizeof(v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746[byt].m_bytHeightCount) );
		}
		dwTemp -= sizeof(v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746[byt].m_aryHeightIO)
			-
			v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746[byt].m_bytHeightCount
			* sizeof(v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746[byt].m_aryHeightIO[0]);

		// 存入发送缓存
		if( byt > 0 )
		{
			dwTemp -= offsetof(tagSampleData_3746, m_szDriverNo); // 非第一点,去除日期时间
			memcpy( v_szBuf + dw, v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746[byt].m_szDriverNo, dwTemp );
			dw += dwTemp;
		}
		else
		{
			// 第一个点含日期时间
			memcpy( v_szBuf + dw, v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746 + byt, dwTemp );
			dw += dwTemp;
		}
#endif

#if USE_DRVRECRPT == 3
		// 基础大小
		dwTemp = sizeof(tagSampleData_3747);

		// 去除多余的载重字段
		if( v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3747[byt].m_bytHeightCount
			>
			sizeof(v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3747[byt].m_aryHeightIO)
			/
			sizeof(v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3747[byt].m_aryHeightIO[0]) )
		{ // 保险起见
			memset( (char*)&v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3747[byt].m_bytHeightCount, 1,
				sizeof(v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3747[byt].m_bytHeightCount) );
		}
		dwTemp -= sizeof(v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3747[byt].m_aryHeightIO)
			-
			v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3747[byt].m_bytHeightCount
			* sizeof(v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3747[byt].m_aryHeightIO[0]);

		// 存入发送缓存
		if( byt > 0 )
		{
			dwTemp -= offsetof(tagSampleData_3747, m_szDriverNo); // 非第一点,去除日期时间
			memcpy( v_szBuf + dw, v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746[byt].m_szDriverNo, dwTemp );
			dw += dwTemp;
		}
		else
		{
			// 第一个点含日期时间
			memcpy( v_szBuf + dw, v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3746 + byt, dwTemp );
			dw += dwTemp;
		}
#endif

#if USE_DRVRECRPT == 11
		// 基础大小
		dwTemp = sizeof(tagSampleData_3745_KJ2);

		// 存入发送缓存
		if( byt > 0 )
		{
			dwTemp -= offsetof(tagSampleData_3745_KJ2, m_szDriverNo); // 非第一点,去除日期时间
			memcpy( v_szBuf + dw, v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3745_KJ2[byt].m_szDriverNo,
				dwTemp );
			dw += dwTemp;
		}
		else
		{
			// 第一个点含日期时间
			memcpy( v_szBuf + dw, v_objDrvSampleDatas.m_uniSampleDatas.m_arySampleData_3745_KJ2 + byt, dwTemp );
			dw += dwTemp;
		}
#endif
	}

	v_szBuf[ dw ++ ] = char( v_objDrvSampleDatas.m_bytAvgSpeedCount ); // 平均速度个数

	// 平均速度
	for( byt = 0; byt < v_objDrvSampleDatas.m_bytAvgSpeedCount; byt ++ )
	{
		memcpy( v_szBuf + dw, v_objDrvSampleDatas.m_aryAvgSpeed, sizeof(v_objDrvSampleDatas.m_aryAvgSpeed[0]) );
		dw += sizeof(v_objDrvSampleDatas.m_aryAvgSpeed[0]);
	}
	
	v_dwLen = dw;

	return 0;
}

void CDrvRecRpt::_SndDrvrecWin(bool v_bReSnd)
{
	static BYTE sta_bytWinId = 0;
	
	int iResult = 0;
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	DWORD dwLen = 0;
	char* pSndBuf = NULL;
	BYTE byt = 0;

	if( !v_bReSnd ) // 如果不是重传(即是上传一个新窗口,包括收到3705应答后重新上传部分帧)
	{
		for( byt = 0; byt < m_bytCurSndDrvRecFrameNum; byt ++ )
		{
			// 提取一帧
			dwLen = m_aryCurSndDrvRecFrameInfo[byt][0];
			pSndBuf = (char*)(m_aryCurSndDrvRecFrameInfo[byt][1]);
			if( dwLen < 1 || !pSndBuf )
			{
				PRTMSG(MSG_ERR, "Snd 3745 Win, find a empty frame\n" );
				continue;
			}

			// 发送前将剩余信息填充完毕 (在这里填充,是因为这时窗口内的帧数确定)
			if( m_bCurSndNeedAns ) // 若需要可靠传输
			{
				pSndBuf[0] |= 0x42; // 先: 可靠传输
				pSndBuf[0] |= sta_bytWinId ? 0x08 : 0; // 再: 窗口标志ID
				if( byt + 1 == m_bytCurSndDrvRecFrameNum ) // 若是窗口最后1帧
				{
					pSndBuf[0] |= 0x01; // 最后: 末帧要应答
					sta_bytWinId = ~sta_bytWinId; // 更换窗口标志
				}

				pSndBuf[1] = m_bytCurSndDrvRecFrameNum; // 窗口大小
				pSndBuf[2] = byt; // 帧序号
			}
			else
			{
				pSndBuf[0] |= 0x40;
				pSndBuf[1] = 0;
				pSndBuf[2] = 0;
			}

			// 发送该帧
			iResult = g_objSms.MakeSmsFrame( pSndBuf, int(dwLen), 0x37, DRVRECFRAME_DATATYPE, buf, sizeof(buf), iBufLen, CVT_8TO7 );
			if( !iResult )
			{
				iResult = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV8 );
			}
		}

		// 若需要可靠传输,且至少发送了一帧
		if( m_bCurSndNeedAns && byt > 0 )
		{
			// 打开重传定时器
			_KillTimer(&g_objTimerMng, DRVRECWIN_REUPLOAD_TIMER );
			_SetTimer(&g_objTimerMng, DRVRECWIN_REUPLOAD_TIMER, DRVRECWIN_REUPLOAD_INTERVAL, G_TmReRptDrvRecWin);

			m_dwDrvCurWinFstSndTime = GetTickCount();
			m_bDrvCurWinChkGprs = false;
		}
		else
		{
			// 发送完毕就清除当前发送的记录数组 
			_ClearCurSndDrvRecWin();

			// 关闭重传定时器,保险起见
			_KillTimer(&g_objTimerMng, DRVRECWIN_REUPLOAD_TIMER );
		}
	}
	else // 否则,即是重传
	{
		if( m_bCurSndNeedAns && m_bytCurSndDrvRecFrameNum > 0 ) // 若需要可靠传输,且当前窗口有缓存帧
		{
			if( m_bytCurSndDrvRecFrameNum < sizeof(m_aryCurSndDrvRecFrameInfo) / sizeof(m_aryCurSndDrvRecFrameInfo[0])
				&& m_bytCurSndDrvRecFrameNum > 0 ) // 保险起见
			{
				dwLen = m_aryCurSndDrvRecFrameInfo[m_bytCurSndDrvRecFrameNum - 1][0];
				pSndBuf = (char*)(m_aryCurSndDrvRecFrameInfo[m_bytCurSndDrvRecFrameNum - 1][1]);
				if( dwLen < 1 || !pSndBuf )
				{
					PRTMSG(MSG_ERR, "Resnd last 3745 of a win, but a empty frame\n" );
					return;
				}

				// 发送该窗口最后1帧
				iResult = g_objSms.MakeSmsFrame( pSndBuf, int(dwLen), 0x37, DRVRECFRAME_DATATYPE, buf, sizeof(buf), iBufLen, CVT_8TO7 );
				if( !iResult )
				{
					iResult = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV8 );
				}
			}

			// 判断是否要底层检查网络
			DWORD dwCur = GetTickCount();
			if( !m_bDrvCurWinChkGprs && dwCur - m_dwDrvCurWinFstSndTime >= DRVWINSNDMAXPERIOD_FSTCHKGPRS ) // 若该窗口第一次检查
			{
				g_objSock.LetComuChkGPRS();
				m_dwDrvCurWinFstSndTime = GetTickCount();
				m_bDrvCurWinChkGprs = true;
			}
			else if( m_bDrvCurWinChkGprs && dwCur - m_dwDrvCurWinFstSndTime >= DRVWINSNDMAXPERIOD_CHKGPRS ) // 若该窗口非第一次检查
			{
				g_objSock.LetComuChkGPRS();
				m_dwDrvCurWinFstSndTime = GetTickCount();
			}
		}
		else
		{
			// 关闭重传定时器,保险起见
			_KillTimer(&g_objTimerMng, DRVRECWIN_REUPLOAD_TIMER );
		}
	}
}

void CDrvRecRpt::DealAccChg( BOOL v_blAccOn )
{
	if( !v_blAccOn )
	{
		char buf[100];
		buf[0] = 0x01;
		buf[1] = 0x00;
		strcat( buf, "ACC断开" );
		DataPush(buf, strlen(buf), DEV_QIAN, DEV_DVR, LV2);
	}

	_BeginDrvRecSample( v_blAccOn );
}

// 设置行驶记录参数
// 千里眼协议(内部模拟协议): 窗口大小（1）＋【发送时长(2)+采样周期（2）＋发送周期（1）】（ACC ON）＋【发送时长(2)+采样周期（2）＋发送周期（1）】（ACC OFF）
// KJ2协议: 窗口大小（1）＋【采样周期（2）＋发送周期（1）】（ACC ON）＋【采样周期（2）＋发送周期（1）】（ACC OFF）
int CDrvRecRpt::Deal103e(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen )
{
	tag2QDriveRecCfg objDrvRecCfg, objDrvRecCfgBkp;
	tag103e req;
	int iRet = 0;
	int iBufLen = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytResType;

	// 参数检查
	if( sizeof(tag103e) != v_dwDataLen )
	{
		iRet = ERR_DATA_INVALID;
		goto DEAL103e_FAILED;
	}

	// 获取原始参数
	iRet = GetSecCfg( &objDrvRecCfg, sizeof(objDrvRecCfg),
		offsetof(tagSecondCfg, m_uni2QDriveRecCfg.m_obj2QDriveRecCfg), sizeof(objDrvRecCfg) );
	if( iRet && ERR_IO != iRet ) goto DEAL103e_FAILED;
	else iRet = 0; // 忽略其他错误
	objDrvRecCfgBkp = objDrvRecCfg; // 先备份

	// 解帧,更新设置
	memcpy( &req, v_szData, sizeof(req) );
	if( 0x7f == req.m_bytSndWinSize ) req.m_bytSndWinSize = 0;
	if( 0x7f == req.m_bytOnSampCycleMin ) req.m_bytOnSampCycleMin = 0;
	if( 0x7f == req.m_bytOnSampCycleSec ) req.m_bytOnSampCycleSec = 0;
	if( 0x7f == req.m_bytOnSndCycle ) req.m_bytOnSndCycle = 0;
	if( 0x7f == req.m_bytOffSampCycleMin ) req.m_bytOffSampCycleMin = 0;
	if( 0x7f == req.m_bytOffSampCycleSec ) req.m_bytOffSampCycleSec = 0;
	if( 0x7f == req.m_bytOffSndCycle ) req.m_bytOffSndCycle = 0;

	objDrvRecCfg.m_bytSndWinSize = req.m_bytSndWinSize;
	objDrvRecCfg.m_bytOnSndCycle = req.m_bytOnSndCycle;
	objDrvRecCfg.m_bytOffSndCycle = req.m_bytOffSndCycle;
	objDrvRecCfg.m_wOnSampCycle = req.m_bytOnSampCycleMin * 60 + req.m_bytOnSampCycleSec;
	objDrvRecCfg.m_wOffSampCycle = req.m_bytOffSampCycleMin * 60 + req.m_bytOffSampCycleSec;

#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4
	if( 0x7f == req.m_bytOnSndHour ) req.m_bytOnSndHour = 0;
	if( 0x7f == req.m_bytOnSndMin ) req.m_bytOnSndMin = 0;
	if( 0x7f == req.m_bytOffSndHour ) req.m_bytOffSndHour = 0;
	if( 0x7f == req.m_bytOffSndMin ) req.m_bytOffSndMin = 0;

	objDrvRecCfg.m_wOnPeriod = WORD(req.m_bytOnSndHour) * 60 + req.m_bytOnSndMin;
	objDrvRecCfg.m_wOffPeriod = WORD(req.m_bytOffSndHour) * 60 + req.m_bytOffSndMin;
	
	if( 0 == objDrvRecCfg.m_wOnPeriod ) objDrvRecCfg.m_wOnPeriod = 0xffff; // 表示永久上报
	if( 0 == objDrvRecCfg.m_wOffPeriod ) objDrvRecCfg.m_wOffPeriod = 0xffff;
#endif

#if USE_DRVRECRPT == 11
	objDrvRecCfg.m_wOffPeriod = objDrvRecCfg.m_wOnPeriod = 0xffff; // 表示永久上报
#endif

	if( objDrvRecCfg.m_bytSndWinSize > 32
		|| objDrvRecCfg.m_bytSndWinSize > sizeof(m_aryCurSndDrvRecFrameInfo) / sizeof(m_aryCurSndDrvRecFrameInfo[0]) )
		// 若发送窗口超过协议定义或缓存的最大范围
	{
		iRet = ERR_DATA_INVALID;
		goto DEAL103e_FAILED;
	}

	// 组帧
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x7e, buf, sizeof(buf), iBufLen);
	if( !iRet )
	{
		// 保存新设置
		iRet = SetSecCfg( &objDrvRecCfg, offsetof(tagSecondCfg, m_uni2QDriveRecCfg.m_obj2QDriveRecCfg), sizeof(objDrvRecCfg) );
		if( iRet ) goto DEAL103e_FAILED;

		// 发送应答
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );
		if( !iRet )
		{
			// 使用新的设置处理行驶记录
			_BeginDrvRecSample( (BOOL)(g_objQianIO.GetDeviceSta() & DEVSTA_ACCVALID) );
		}
		else
		{
			// 恢复原设置
			SetSecCfg( &objDrvRecCfgBkp, offsetof(tagSecondCfg, m_uni2QDriveRecCfg.m_obj2QDriveRecCfg), sizeof(objDrvRecCfgBkp) );
			
			goto DEAL103e_FAILED;
		}
	}
	else goto DEAL103e_FAILED;

	return 0;

DEAL103e_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x7e, buf, sizeof(buf), iBufLen);
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );

	return iRet;
}

// 行驶数据上传应答: 应答类型（1）＋帧序号掩码（4）
int CDrvRecRpt::Deal3705(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen )
{
	BYTE byt = 0;

	if( v_dwDataLen < 1 ) return ERR_PAR;

	if( 1 == v_szData[0] ) // 若是成功应答
	{
		// 先清除旧窗口数据
		_ClearCurSndDrvRecWin();

		// 再继续上传
		DrvRecToSnd(0);

		return 0;
	}
	else if( 0 == v_szData[0] ) // 若是需要重传应答
	{
		if( v_dwDataLen < 5 ) return ERR_PAR;

		// 获取重传帧序号掩码
		DWORD dwRcvCode = 0;
		memcpy( &dwRcvCode, v_szData + 1, sizeof(dwRcvCode) );
		dwRcvCode = ntohl( dwRcvCode );

		// 寻找需要的最后一帧编号
		DWORD dwSymb = 1;
		BYTE bytLstFrameNo = 0xff;
		for( byt = 0; byt < sizeof(m_aryCurSndDrvRecFrameInfo) / sizeof(m_aryCurSndDrvRecFrameInfo[0])
			&& byt < m_bytCurSndDrvRecFrameNum; byt ++ )
		{
			if( dwSymb & dwRcvCode ) // 若该序号的帧未成功接收
				bytLstFrameNo = byt;
			dwSymb = dwSymb << 1;
		}
		if( bytLstFrameNo >= m_bytCurSndDrvRecFrameNum ) return ERR_PAR;

		// 重传处理
		char buf[ SOCK_MAXSIZE ];
		int iBufLen = 0;
		int iResult = 0;
		DWORD dwLen = 0;
		char* pSndBuf = NULL;
		char cTemp = 0;
		dwSymb = 1;
		for( byt = 0; byt < sizeof(m_aryCurSndDrvRecFrameInfo) / sizeof(m_aryCurSndDrvRecFrameInfo[0])
			&& byt < m_bytCurSndDrvRecFrameNum; byt ++ )
		{
			if( dwSymb & dwRcvCode ) // 若该序号的帧未成功接收
			{
				dwLen = m_aryCurSndDrvRecFrameInfo[byt][0];
				pSndBuf = (char*)(m_aryCurSndDrvRecFrameInfo[byt][1]);

				if( dwLen < 1 || !pSndBuf )
				{
					PRTMSG(MSG_ERR, "Deal3705,resend 3745 frame,find an empty frame\n" );
					continue;
				}

				// 如果是最后一帧,发送前第一个字节的应答标志位修改为要应答
				if( bytLstFrameNo == byt )
				{
					cTemp = pSndBuf[0];
					pSndBuf[0] |= 1;
				}

				iResult = g_objSms.MakeSmsFrame( pSndBuf, int(dwLen), 0x37, DRVRECFRAME_DATATYPE,
					buf, sizeof(buf), iBufLen, CVT_8TO7 );
				if( !iResult ) iResult = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV8 );

				// 如果是最后一帧,发送完后第一个字节要恢复为原值
				if( bytLstFrameNo == byt )
				{
					pSndBuf[0] = cTemp;
				}
			}

			dwSymb = dwSymb << 1;
		}

		return iResult;
	}
	else
		return ERR_PAR;
}

#endif //#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 || USE_DRVRECRPT == 11


