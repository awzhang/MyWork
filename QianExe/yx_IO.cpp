#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-QianIO   ")

void G_TmAccChk(void *arg, int len)
{
	g_objQianIO.P_TmAccChk();
}

CQianIO::CQianIO()
{

}

CQianIO::~CQianIO()
{

}

int CQianIO::Init()
{
	m_dwMeterComuCycle = 0;
	m_dwDeviceSta = 0;
	m_bPowLowSta = false;
	m_dwTmRcvDCD=0;
	m_dwAccInvalidTm = GetTickCount();

	pthread_mutex_init(&m_MutexDevSta, NULL);
}

int CQianIO::Release()
{
	pthread_mutex_destroy( &m_MutexDevSta );
}

void CQianIO::P_TmAccChk()
{
	_KillTimer(&g_objTimerMng, ACCCHK_TIMER );
	
	unsigned char uszResult;

	if( !IOGet((byte)IOS_ACC, &uszResult) )
	{
		if( IO_ACC_ON == uszResult )
		{
//			PowerOnGps( true );
			SetDevSta( true, DEVSTA_ACCVALID );
		}
		else if( IO_ACC_ON == uszResult )
		{
			DWORD dwCur = GetTickCount();
			if( dwCur - m_dwAccInvalidTm >= ACCCHK_INTERVAL )
			{
//				PowerOnGps( false );
				//告诉手柄进入省电模式
				char buf[] = {0x01, 0x0c, 0x01};
				DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);
			}
			SetDevSta( false, DEVSTA_ACCVALID );
		} 
	}
	
	if( IO_ACC_ON == uszResult )
		_SetTimer(&g_objTimerMng,  ACCCHK_TIMER, ACCCHK_INTERVAL, G_TmAccChk ); // 不能省略
	else
		_SetTimer(&g_objTimerMng,  ACCCHK_TIMER, 10000, G_TmAccChk );
}

int CQianIO::DealIOMsg(char *v_szbuf, DWORD v_dwlen, BYTE v_bytSrcSymb)
{
	DWORD dwVeSta = g_objCarSta.GetVeSta(); // 先获取当前状态

	switch( v_szbuf[0] )
	{
	case IO_ALARM_ON:
		{
			PRTMSG(MSG_NOR, "IO: Alerm\n" );

#if USE_LIAONING_SANQI == 0
			g_objMonAlert.SetAlertFoot(true);
#endif
#if USE_LIAONING_SANQI == 1
			g_objReport.SetAlert(true, ALERT_TYPE_FOOT, 0);
			g_objLNPhoto.AddPhoto( PHOTOEVT_FOOTALERM );
#endif
		}
		break;

#if CHK_VER == 1
	case IO_ALARM_OFF:
		{
			PRTMSG(MSG_NOR, "IO: Alerm\n" );
			
			g_objMonAlert.P_TmAlertInvalid();// 生产检测版本撤销报警时即停止急闪红灯
		}
		break;
#endif
		
	case IO_ACC_ON:
		{
			PRTMSG(MSG_NOR, "IO: ACC ON\n");

			BYTE buf[] = {0x01, 0xf9, 0x01};
			DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);

			SetDevSta( true, DEVSTA_ACCVALID );

			// 现在未做省电控制，无需开关GPS电源
			//StartAccChk( 3000 );

#if USE_BLK == 1
			g_objBlk.BlkSaveStart();
#endif

#if USE_LEDTYPE == 1
			g_objLedBohai.LedTurnToWork();
#endif
			//g_objMonAlert.SetAlertFrontDoor( true );

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
#if USE_LIAONING_SANQI == 1
			g_objLNPhoto.AddPhoto( PHOTOEVT_ACCON );
#else
			g_objPhoto.AddPhoto( PHOTOEVT_ACCON );
#endif

#endif
#if USE_LIAONING_SANQI == 1
			g_objReport.m_accsta = 0x01;
#endif
		}
		break;

	case IO_ACC_OFF:
		{
			PRTMSG(MSG_NOR, "IO: ACC OFF\n");

			m_dwAccInvalidTm = GetTickCount(); // 先更新开始无效的时刻
			
			BYTE buf[] = {0x01, 0xf9, 0x00};	
			DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);

			SetDevSta( false, DEVSTA_ACCVALID );

			// 现在未做省电控制，无需开关GPS电源
			//StartAccChk( ACCCHK_INTERVAL ); 

#if USE_BLK == 1
			g_objBlk.BlkSaveStop();
#endif
			
#if USE_LEDTYPE == 1
			g_objLedBohai.LedTurnToSleep();
#endif
#if USE_LIAONING_SANQI == 1
			g_objReport.m_accsta = 0x00;
#endif
		}
		break;

#if VEHICLE_TYPE == VEHICLE_M || VEHICLE_TYPE == VEHICLE_V8
	case IO_JIJIA_HEAVY:
		{
			PRTMSG(MSG_NOR, "IO: Jijia Valid\n" );
			
//			dwVeSta |= VESTA_HEAVY;
			dwVeSta &= ~VESTA_HEAVY;
			g_objCarSta.SetVeSta( dwVeSta );
			
			//从重车转为空车，LED顶灯显示'到达'
#if USE_LEDTYPE == 3
			g_objKTLed.SetLedShow('B',1,30*1000);
#endif
			//红外评价开始工作
			g_objKTIrdMod.KTIrdStart();

#if USE_JIJIA == 1
			g_objJijia.BeginJijia( false );
#endif

#if USE_METERTYPE == 2
			//通知触摸屏变为空车
			char buf2[2];
			buf2[0] = 0xdd;
			buf2[1] = 0x02;
			DataPush(buf2, sizeof(buf2), DEV_QIAN, DEV_DIAODU, 2);
#endif
		}
		break;

	case IO_JIJIA_LIGHT:
		{
			PRTMSG(MSG_NOR, "IO: Jijia InValid\n" );

#if USE_METERTYPE == 2			
			//空车转重车，开始记录运营数据位置信息
			g_objKTMeter.BeginMeterRcd();
#endif
//			dwVeSta |= ~VESTA_HEAVY;
			dwVeSta |= VESTA_HEAVY;		
			g_objCarSta.SetVeSta( dwVeSta );

#if USE_JIJIA == 1
//			g_objJijia.BeginJijia( false );
			g_objJijia.BeginJijia( true );//zzg mod
#endif

#if USE_METERTYPE == 2
			//通知触摸屏变为重车
			char buf2[2];
			buf2[0] = 0xdd;
			buf2[1] = 0x01;
			DataPush(buf2, sizeof(buf2), DEV_QIAN, DEV_DIAODU, 2);
#endif
		}
		break;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	case IO_POWDETM_INVALID:		// 无输入电源
		{
			PRTMSG(MSG_NOR, "IO: Power off\n" );
#if USE_LIAONING_SANQI == 1
			g_objReport.SetAlert(true, ALERT_TYPE_POWOFF, 0);
#else
			g_objMonAlert.SetAlertPowOff( true );
#endif
		}
		break;

	case IO_POWDETM_VALID:			// 输入电源正常
		{
			PRTMSG(MSG_NOR, "IO: Power on\n" );
			g_objMonAlert.SetAlertPowOff( false );
		}
		break;

	case IO_QIANYA_VALID:			// 欠压
		{
			// 先检测是否断电，若是断电，则不进行欠压报警，直接进行断电报警
			byte bytSta;
			IOGet(IOS_POWDETM, &bytSta);
			
			if( bytSta == IO_POWDETM_VALID )
			{
				// 再检测是否熄火，若未熄火，则不进行欠压报警
				byte bytSta1;
				IOGet(IOS_ACC, &bytSta1);
				
				if( bytSta1 == IO_ACC_OFF )
				{
					PRTMSG(MSG_NOR, "IO: Pow Brownout!\n" );
					g_objMonAlert.SetAlertPowBrownout( true );
				}
			}
			else
			{
				PRTMSG(MSG_NOR, "But Power off!\n" );
#if USE_LIAONING_SANQI == 1
			g_objReport.SetAlert(true, ALERT_TYPE_POWOFF, 0);
#else
				g_objMonAlert.SetAlertPowOff( true );
#endif
			}
		}
		break;

	case IO_QIANYA_INVALID:			// 电压正常
		{
			PRTMSG(MSG_NOR, "IO: Pow Brownout cancel\n" );
			g_objMonAlert.SetAlertPowBrownout( false );
#if USE_LIAONING_SANQI == 1
			g_objReport.SetAlert(true, ALERT_TYPE_POWOFF, 0);
#else
			g_objMonAlert.SetAlertPowOff( false );
#endif				
		}
		break;
	case IO_FDOOR_OPEN:
		{
			g_objMonAlert.SetAlertFrontDoor(true);
			g_objDriveRecord.m_bNeedClt = true;
		}
		break;
	case IO_FDOOR_CLOSE:
		{
			g_objMonAlert.SetAlertFrontDoor(false);
			g_objDriveRecord.m_bNeedClt = true;
		}
		break;
#if VEHICLE_TYPE == VEHICLE_V8
	case IO_ELECSPEAKER_VALID://改为开后车门
#endif
#if VEHICLE_TYPE == VEHICLE_M2
	case IO_BDOOR_OPEN:
#endif
		{
			g_objMonAlert.SetAlertBackDoor(true);
			g_objDriveRecord.m_bNeedClt = true;
			
		}
		break;
#if VEHICLE_TYPE == VEHICLE_V8
	case IO_ELECSPEAKER_INVALID://改为关后车门
#endif
#if VEHICLE_TYPE == VEHICLE_M2
	case IO_BDOOR_CLOSE:
#endif
		{
			g_objMonAlert.SetAlertBackDoor(false);
			g_objDriveRecord.m_bNeedClt = true;
		}
		break;
		
#endif
		
	default:	
		break;
	}
}

BOOL CQianIO::PowerOnGps( bool v_bVal )
{
/*
	static BYTE sta_bytLstSta = 0; // 1,表示GPS已上电; 2,表示已关电

	if( !v_bVal && 2 != sta_bytLstSta ) // 若要关电,且GPS没有关过电
	{
		IOSet(IOS_GPS, IO_GPSPOW_ON, NULL, 0);
		sta_bytLstSta = 2;

		PRTMSG(MSG_NOR, "Power off GPS\n" );
		
		SetDevSta( false, DEVSTA_GPSON );
	}
	else if( v_bVal && 1 != sta_bytLstSta ) // 若要上电,且GPS没有上过电
	{
		IOSet(IOS_GPS, IO_GPSPOW_OFF, NULL, 0);
		sta_bytLstSta = 1;

		PRTMSG(MSG_NOR, "Power on GPS" );
		
		SetDevSta( true, DEVSTA_GPSON );
	}
	
	return TRUE;
*/
}

void CQianIO::StartAccChk(DWORD v_dwFstInterval)
{
	_KillTimer(&g_objTimerMng, ACCCHK_TIMER );
	_SetTimer(&g_objTimerMng, ACCCHK_TIMER, v_dwFstInterval, G_TmAccChk );
}

void CQianIO::StopAccChk()
{
	_KillTimer(&g_objTimerMng, ACCCHK_TIMER );
}

DWORD CQianIO::GetDeviceSta()
{
	DWORD dwRet = 0;
	
	pthread_mutex_lock( &m_MutexDevSta );
	
	dwRet = m_dwDeviceSta;
	
	pthread_mutex_unlock( &m_MutexDevSta );
	
	return dwRet;
}

void CQianIO::SetDevSta( bool v_bSet, DWORD v_dwVal )
{
	pthread_mutex_lock( &m_MutexDevSta );
	
	if( v_bSet ) m_dwDeviceSta |= v_dwVal;
	else m_dwDeviceSta &= ~v_dwVal;
	
	pthread_mutex_unlock( &m_MutexDevSta );
}

bool CQianIO::GetLowPowSta()
{
	bool bVal = false;
	
	pthread_mutex_lock( &m_MutexDevSta );
	
	bVal = m_bPowLowSta;
	
	pthread_mutex_unlock( &m_MutexDevSta );
	
	return bVal;
}

void CQianIO::SetLowPowSta( bool v_bVal )
{
	pthread_mutex_lock( &m_MutexDevSta );
	
	m_bPowLowSta = v_bVal ;
	
	pthread_mutex_unlock( &m_MutexDevSta );
}

void CQianIO::ConnectOilElec()
{
	IOSet(IOS_OILELECCTL, IO_OILELECCTL_RESUME, NULL, 0);
}

void CQianIO::DisconnOilElec()
{
	IOSet(IOS_OILELECCTL, IO_OILELECCTL_CUT, NULL, 0);
}



