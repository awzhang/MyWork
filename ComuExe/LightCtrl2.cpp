#include "ComuStdAfx.h"

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2

void *G_ThreadLightCtrl2(void *arg)
{
	g_objLightCtrl2.P_ThreadLightCtrl();
}

#undef MSG_HEAD
#define MSG_HEAD	("Comu-LightCtrl2")

//////////////////////////////////////////////////////////////////////////

CLightCtrl2::CLightCtrl2()
{
	memset(m_szReBootTime, 0, sizeof(m_szReBootTime));
	memset(m_szSwiLinkTime, 0, sizeof(m_szSwiLinkTime));
	memset(m_szDiskLinkTime, 0, sizeof(m_szDiskLinkTime));
	memset(m_szMonLinkTime, 0, sizeof(m_szMonLinkTime));
	
	m_iTelOnCnt = 0;
	memset(m_szTelOnTime, 0, sizeof(m_szTelOnTime));
	memset(m_iTelOnSec, 0, sizeof(m_iTelOnSec));
	m_iTelInitCnt = 0;
	memset(m_szTelInitTime, 0, sizeof(m_szTelInitTime));
	memset(m_iTelInitSec, 0, sizeof(m_iTelInitSec));
	m_iLowSigCnt = 0;
	memset(m_szLowSigTime, 0, sizeof(m_szLowSigTime));
	memset(m_iLowSigSec, 0, sizeof(m_iLowSigSec));
	m_iDialNetCnt = 0;
	memset(m_szDialNetTime, 0, sizeof(m_szDialNetTime));
	memset(m_iDialNetSec, 0, sizeof(m_iDialNetSec));
	m_iTcpConCnt = 0;
	memset(m_szTcpConTime, 0, sizeof(m_szTcpConTime));
	memset(m_iTcpConSec, 0, sizeof(m_iTcpConSec));
	m_iLogInCnt = 0;
	memset(m_szLogInTime, 0, sizeof(m_szLogInTime));
	memset(m_iLogInSec, 0, sizeof(m_iLogInSec));

	m_iDeal3801Cnt = 0;
	memset(m_szDeal3801Time, 0, sizeof(m_szDeal3801Time));
	memset(m_iDeal3801Sec, 0, sizeof(m_iDeal3801Sec));
	m_iVideoMonCnt = 0;
	memset(m_szVideoMonTime, 0, sizeof(m_szVideoMonTime));
	memset(m_iVideoMonSec, 0, sizeof(m_iVideoMonSec));
	
	m_dwDevErrCode = 0;
	m_dwCfgErrCode = 0;
	m_dwTelErrCode = 0;
	m_dwMonErrCode = 0;
	
	m_bytDvrState = 1;
	m_bytTcpState = 0;
	
	m_bNetWork = false;
	m_bNetWork = false;
	m_bFoot = false;
	m_bDial = false;
	m_bytLightSta = Red_ManShan;
	m_pthreadLightCtrl = 0;
}

CLightCtrl2::~CLightCtrl2()
{

}

int CLightCtrl2::Init()
{
	// 创建控制灯线程
	if( pthread_create(&m_pthreadLightCtrl, NULL, G_ThreadLightCtrl2, NULL) != 0 )
	{
		PRTMSG(MSG_ERR, "create light thread failed!\n");
		return ERR_THREAD;
	}

	PRTMSG(MSG_ERR, "create light thread succ!\n");
	return 0;
}

int CLightCtrl2::Release()
{

}

void CLightCtrl2::P_ThreadLightCtrl()
{
	int iSleepTime = 1000;
	static int sta_iManShanCnt = 0;
	static int sta_iKuaiShanCnt = 0;

	BYTE  bytLvl = 0;
	char  szbuf[10] = {0};
	DWORD dwLen = 0;
	DWORD dwPushTm;

	RenewMemInfo(0x02, 0x02, 0x00, 0x00);

	sleep(2);
	
	while( !g_bProgExit )
	{
		if( !m_objMemInfoMng.PopData(bytLvl, sizeof(szbuf), dwLen, szbuf, dwPushTm) )
		{
			if( 3 == dwLen )
			{
				switch( szbuf[0] )
				{
					case 0x00:		//程序异常错误码(每种异常最多支持32个错误码)
						{
							switch( szbuf[1] )
							{
								case 0x00:		//芯片外设
									m_dwDevErrCode |= (0x00000001<<szbuf[2]);
									break;
								case 0x01:		//配置参数
									m_dwCfgErrCode |= (0x00000001<<szbuf[2]);
									break;
								case 0x02:		//手机模块
									m_dwTelErrCode |= (0x00000001<<szbuf[2]);
									break;
								case 0x03:		//视频监控
									m_dwMonErrCode |= (0x00000001<<szbuf[2]);
									break;
								default:
									break;
							}
						}
						break;
					
					case 0x01:		//设备各类状态信息
						{
							switch( szbuf[1] )
							{
								case 0x00:		//DvrExe状态
									m_bytDvrState = szbuf[2];
									break;
								case 0x01:		//Qian Tcp状态
									m_bytTcpState = szbuf[2];
									break;
								default:
									break;
							}
						}
						break;
						
					case 0x02:		//统计业务操作时间、次数
						{
							switch( szbuf[1] )
							{
								case 0x00:
									{
										switch( szbuf[2] )
										{
											case 0x00:		//最近一次开机时间
												RecTimeAndCnt(m_szReBootTime);
												break;
											case 0x01:		//最近一次G_SysSwitch线程清狗时间
												RecTimeAndCnt(m_szSwiLinkTime);
												break;
											case 0x02:		//最近一次G_DiskManage线程清狗时间
												RecTimeAndCnt(m_szDiskLinkTime);
												break;
											case 0x03:		//最近一次G_DataUpload线程清狗时间
												RecTimeAndCnt(m_szMonLinkTime);
												break;
											default:
												break;
										}
									}
									break;
								case 0x01:
									{
										switch( szbuf[2] )
										{
											case 0x00:		//最近一次手机模块上电时间和前一小时内上电次数
												RecTimeAndCnt(m_szTelOnTime, &m_iTelOnCnt, m_iTelOnSec, 3600);
												break;
											case 0x01:		//最近一次弱信号时间和前一小时内弱信号次数
												RecTimeAndCnt(m_szLowSigTime, &m_iLowSigCnt, m_iLowSigSec, 3600);
												break;
											case 0x02:		//最近一次拨号时间和前一小时内拨号次数
												RecTimeAndCnt(m_szDialNetTime, &m_iDialNetCnt, m_iDialNetSec, 3600);
												break;
											case 0x03:		//最近一次连接时间和前五分钟内连接次数
												RecTimeAndCnt(m_szTcpConTime, &m_iTcpConCnt, m_iTcpConSec, 300);
												break;
											case 0x04:		//最近一次登陆时间和前五分钟内登陆次数
												RecTimeAndCnt(m_szLogInTime, &m_iLogInCnt, m_iLogInSec, 300);
												break;
											case 0x05:		//最近一次手机模块初始化时间和前一小时内初始化次数
												RecTimeAndCnt(m_szTelInitTime, &m_iTelInitCnt, m_iTelInitSec, 3600);
												break;
											default:
												break;
										}
									}
									break;
								case 0x02:
									{
										switch( szbuf[2] )
										{
											case 0x00:		//最近一次收到视频监控指令时间和五分钟内收到次数
												RecTimeAndCnt(m_szDeal3801Time, &m_iDeal3801Cnt, m_iDeal3801Sec, 300);
												break;
											case 0x01:		//最近一次启动视频监控时间和五分钟内登陆次数
												RecTimeAndCnt(m_szVideoMonTime, &m_iVideoMonCnt, m_iVideoMonSec, 300);
												break;
											default:
												break;
										}
									}
									break;
							}
						}
	
					default:
						break;				
				}
			}
		}
		
		if( !m_objRecvCtrlMng.PopData(bytLvl, sizeof(szbuf), dwLen, szbuf, dwPushTm) )
		{
			if( 1 == dwLen )
			{
				switch( szbuf[0] )
				{
					case 0x01:		// 拨号成功通知
						m_bNetWork = true;
						m_bDial = false;	// 网络拨号成功，或者拨号失败，都是退出拨号过程
						break;
						
					case 0x02:		// 断开网络通知
						m_bNetWork = false;
						m_bDial = false;	// 网络拨号成功，或者拨号失败，都是退出拨号过程
						break;
						
					case 0x03:		// 开始录像通知
						m_bVideoWork = true;
						break;
	
					case 0x04:		// 停止录像通知
						m_bVideoWork = false;
						break;
	
					case 0x05:		// 抢劫报警通知
						m_bFoot = true;
						break;
						
					case 0x06:		// 撤销报警通知
						m_bFoot = false;
						break;
	
					case 0x07:		// 开始拨号通知
						m_bDial = true;
						break;
						
					default:
						break;
				}
			}
		}

		// 上线和录像指示位于最低级别
		if( m_bNetWork && m_bVideoWork )	m_bytLightSta = Green_KuaiShan;
		if( m_bNetWork && !m_bVideoWork )	m_bytLightSta = Green_ManShan;
		if( !m_bNetWork && m_bVideoWork )	m_bytLightSta = Red_KuaiShan;
		if( !m_bNetWork && !m_bVideoWork )	m_bytLightSta = Red_ManShan;

		// 拨号指示位于中间级别
		if( m_bDial )	m_bytLightSta = Green_JiShan;

		// 抢劫报警位于最高级别
		if( m_bFoot )	m_bytLightSta = Red_JiShan;
		
#if CHK_VER == 1
		if( m_dwDevErrCode ) m_bytLightSta = Red_JiShan;
#endif

		switch( m_bytLightSta )
		{
		case Red_ManShan:	// 红灯慢闪
			{
				//关闭绿灯
				IOSet(IOS_SYSLEDPOW_GREEN, IO_SYSLEDPOW_GREEN_OFF, NULL, 0);

				sta_iManShanCnt++;
				if( sta_iManShanCnt % 10 == 0 )
					IOSet(IOS_SYSLEDPOW_RED, IO_SYSLEDPOW_RED_ON, NULL, 0);
				else
					IOSet(IOS_SYSLEDPOW_RED, IO_SYSLEDPOW_RED_OFF, NULL, 0);
				
				iSleepTime = 500;
			}
			break;
			
		case Red_KuaiShan:	// 红灯快闪
			{
				//关闭绿灯
				IOSet(IOS_SYSLEDPOW_GREEN, IO_SYSLEDPOW_GREEN_OFF, NULL, 0);

				if( sta_iKuaiShanCnt++ % 2 == 0 )
					IOSet(IOS_SYSLEDPOW_RED, IO_SYSLEDPOW_RED_ON, NULL, 0);
				else
					IOSet(IOS_SYSLEDPOW_RED, IO_SYSLEDPOW_RED_OFF, NULL, 0);

				iSleepTime = 2000;
			}
			break;
			
		case Green_ManShan:	// 绿灯慢闪
			{
				//关闭红灯
				IOSet(IOS_SYSLEDPOW_RED, IO_SYSLEDPOW_RED_OFF, NULL, 0);
				
				sta_iManShanCnt++;
				if( sta_iManShanCnt % 10 == 0 )
					IOSet(IOS_SYSLEDPOW_GREEN, IO_SYSLEDPOW_GREEN_ON, NULL, 0);
				else
					IOSet(IOS_SYSLEDPOW_GREEN, IO_SYSLEDPOW_GREEN_OFF, NULL, 0);
				
				iSleepTime = 500;
			}
			break;
			
		case Green_KuaiShan:	// 绿灯快闪
			{
				//关闭红灯
				IOSet(IOS_SYSLEDPOW_RED, IO_SYSLEDPOW_RED_OFF, NULL, 0);
				
				if( sta_iKuaiShanCnt++ % 2 == 0 )
					IOSet(IOS_SYSLEDPOW_GREEN, IO_SYSLEDPOW_GREEN_ON, NULL, 0);
				else
					IOSet(IOS_SYSLEDPOW_GREEN, IO_SYSLEDPOW_GREEN_OFF, NULL, 0);
				
				iSleepTime = 2000;
			}
			break;

		case Red_JiShan:
			{
				//关闭绿灯
				IOSet(IOS_SYSLEDPOW_GREEN, IO_SYSLEDPOW_GREEN_OFF, NULL, 0);
				
				if( sta_iKuaiShanCnt++ % 2 == 0 )
					IOSet(IOS_SYSLEDPOW_RED, IO_SYSLEDPOW_RED_ON, NULL, 0);
				else
					IOSet(IOS_SYSLEDPOW_RED, IO_SYSLEDPOW_RED_OFF, NULL, 0);
				
				iSleepTime = 500;
			}
			break;
			
		case Green_JiShan:
			{
				//关闭红灯
				IOSet(IOS_SYSLEDPOW_RED, IO_SYSLEDPOW_RED_OFF, NULL, 0);
				
				if( sta_iKuaiShanCnt++ % 2 == 0 )
					IOSet(IOS_SYSLEDPOW_GREEN, IO_SYSLEDPOW_GREEN_ON, NULL, 0);
				else
					IOSet(IOS_SYSLEDPOW_GREEN, IO_SYSLEDPOW_GREEN_OFF, NULL, 0);
				
				iSleepTime = 500;
			}
			break;

		default:
			iSleepTime = 1000;
			break;
		}

		usleep(iSleepTime*1000);
	}
}

void CLightCtrl2::SetLightSta(char v_szSta)
{
	DWORD	dwPackNum;
	m_objRecvCtrlMng.PushData(LV1, 1, &v_szSta, dwPackNum);
}

#endif
