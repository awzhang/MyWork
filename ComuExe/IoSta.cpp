#include "ComuStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("ComuExe-IOSta:")

CIoSta::CIoSta()
{
	_Init();
}

CIoSta::~CIoSta()
{
	_Release();
}

int CIoSta::_Init()
{
	m_dwIoSta = 0;
	pthread_mutex_init(&m_MutexIoSta, NULL);
}

int CIoSta::_Release()
{
	pthread_mutex_destroy( &m_MutexIoSta );
}

int CIoSta::DealIOMsg(char *v_szbuf, DWORD v_dwlen, BYTE v_bytSrcSymb)
{	
	switch( v_szbuf[0] )
	{
	case IO_ACC_ON:
		{
			PRTMSG(MSG_NOR, "IOSta: ACC ON\n");			
			_SetIoSta( true, IOSTA_ACC_ON );
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_ACCON);
			}
		}
		break;
		
	case IO_ACC_OFF:
		{
			PRTMSG(MSG_NOR, "IOSta: ACC OFF\n");		
			_SetIoSta( false, IOSTA_ACC_ON );
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_ACCOFF);
			}
		}
		break;
		
	case IO_ALARM_ON:
		{
			PRTMSG(MSG_NOR, "IOSta: ALARM ON\n");			
			_SetIoSta( true, IOSTA_ALARM_ON );	
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_ALERMFOOT);
			}
		}
		break;
	
	case IO_ALARM_OFF:
		{
			PRTMSG(MSG_NOR, "IOSta: ALARM OFF\n");			
			_SetIoSta( false, IOSTA_ALARM_ON );	
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_ALERMFOOT);
			}
		}
		break;

#if VEHICLE_TYPE == VEHICLE_M || VEHICLE_TYPE == VEHICLE_V8
	case IO_JIJIA_HEAVY:
		{
			PRTMSG(MSG_NOR, "IO: Jijia HEAVY\n" );
			_SetIoSta( true, IOSTA_JIJIA_HEAVY );
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_JIJIA_VALID);
			}			
		}
		break;
		
	case IO_JIJIA_LIGHT:
		{
			PRTMSG(MSG_NOR, "IO: Jijia LIGHT\n" );
			_SetIoSta( false, IOSTA_JIJIA_HEAVY);
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_JIJIA_INVALID);
			}
		}
		break;
#endif

	case IO_FDOOR_OPEN:
		{
			PRTMSG(MSG_NOR, "IO: FDOOR OPEN\n" );
			_SetIoSta( true, IOSTA_FDOOR_OPEN);
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_DOOROPEN);
			}
		}
		break;

	case IO_FDOOR_CLOSE:
		{
			PRTMSG(MSG_NOR, "IO: FDOOR CLOSE\n" );
			_SetIoSta( false, IOSTA_FDOOR_OPEN);
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_DOORCLOSE);
			}
		}
		break;

#if VEHICLE_TYPE == VEHICLE_M
	case IO_BDOOR_OPEN:
		{
			PRTMSG(MSG_NOR, "IO: BDOOR OPEN\n" );
			_SetIoSta( true, IOSTA_BDOOR_OPEN);	
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_BACKDOOROPEN);
			}
		}
		break;
		
	case IO_BDOOR_CLOSE:
		{		
			PRTMSG(MSG_NOR, "IO: BDOOR CLOSE\n" );
			_SetIoSta( false, IOSTA_BDOOR_OPEN);
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_BACKDOORCLOSE);
			}
		}
		break;
		
	case IO_JIAOSHA_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: JIAOSHA VALID\n");		
			_SetIoSta( true, IOSTA_JIAOSHA_VALID );
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_FOOTBRAKE);
			}
		}
		break;
		
	case IO_JIAOSHA_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: JIAOSHA INVALID\n");		
			_SetIoSta( false, IOSTA_JIAOSHA_VALID );
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_FOOTBRAKE_CANCEL);
			}
		}
		break;

	case IO_SHOUSHA_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: SHOUSHA VALID\n");		
			_SetIoSta( true, IOSTA_SHOUSHA_VALID );
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_HANDBRAKE);
			}
		}
		break;

	case IO_SHOUSHA_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: SHOUSHA INVALID\n");		
			_SetIoSta( false, IOSTA_SHOUSHA_VALID );
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_HANDBRAKE_CANCEL);
			}
		}
		break;

	case IO_PASSENGER1_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: PASSENGER1 VALID\n");		
			_SetIoSta( true, IOSTA_PASSENGER1_VALID );
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_PASSENGER1_VALID);
			}
		}
		break;

	case IO_PASSENGER1_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: PASSENGER1 INVALID\n");		
			_SetIoSta( false, IOSTA_PASSENGER1_VALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_PASSENGER1_INVALID);
			}
		}
		break;

	case IO_PASSENGER2_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: PASSENGER2 VALID\n");		
			_SetIoSta( true, IOSTA_PASSENGER2_VALID );	
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_PASSENGER2_VALID);
			}
		}
		break;
		
	case IO_PASSENGER2_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: PASSENGER2 INVALID\n");		
			_SetIoSta( false, IOSTA_PASSENGER2_VALID );	
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_PASSENGER2_INVALID);
			}
		}
		break;

	case IO_PASSENGER3_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: PASSENGER3 VALID\n");		
			_SetIoSta( true, IOSTA_PASSENGER3_VALID );	
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_PASSENGER3_VALID);
			}
		}
		break;
		
	case IO_PASSENGER3_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: PASSENGER3 INVALID\n");		
			_SetIoSta( false, IOSTA_PASSENGER3_VALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_PASSENGER3_INVALID);
			}
		}
		break;
#endif

#if VEHICLE_TYPE == VEHICLE_V8
	case IO_QIANYA_VALID:
		{
			// 先检测是否断电
			byte bytSta;
			IOGet(IOS_POWDETM, &bytSta);

			if( bytSta == IO_POWDETM_VALID )	// 不是断电，则是欠压
			{
				PRTMSG(MSG_NOR, "IOSta: IO_QIANYA_VALID\n");		
				_SetIoSta( false, IO_QIANYA_VALID );		
				if (g_objDiaodu.m_iodect > 0)
				{
					g_objDiaodu.show_diaodu(LANG_IOCHK_QIANYA_VALID);
				}
			}
			else		// 若是断电，直接进行断电处理
			{
#if CHK_VER == 1	
				if (g_objDiaodu.m_iodect > 0)
				{
					//if( g_objPhone.IsPhoneValid() )	// 先检测手机模块是否正常	
					{
						for(int i=0; i<3; i++)
						{
							IOSet(IOS_YUYIN, IO_YUYIN_ON, NULL, 0);
							usleep(500000);
							IOSet(IOS_YUYIN, IO_YUYIN_OFF, NULL, 0);
							usleep(500000);
						}
					}
				}
#endif		
			}
			
		}
		break;

	case IO_QIANYA_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_QIANYA_INVALID\n");		
			_SetIoSta( false, IO_QIANYA_INVALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_QIANYA_INVALID);
			}
		}
		break;

	case IO_ELECSPEAKER_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_ELECSPEAKER_VALID\n");		
			_SetIoSta( false, IO_ELECSPEAKER_VALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_ELECSPEAKER_VALID);
			}
		}
		break;

	case IO_ELECSPEAKER_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_ELECSPEAKER_INVALID\n");		
			_SetIoSta( false, IO_ELECSPEAKER_INVALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_ELECSPEAKER_INVALID);
			}
		}
		break;

	case IO_POWDETM_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_POWDETM_VALID\n");		
			_SetIoSta( false, IO_POWDETM_VALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_POWDETM_VALID);
			}
		}
		break;

	case IO_POWDETM_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_POWDETM_INVALID\n");		
			_SetIoSta( false, IO_POWDETM_INVALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_POWDETM_INVALID);
			}
		}
		break;

	case IO_CHEDENG_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_CHEDENG_VALID\n");		
			_SetIoSta( false, IO_CHEDENG_VALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_CHEDENG_VALID);
			}
		}
		break;

	case IO_CHEDENG_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_CHEDENG_INVALID\n");		
			_SetIoSta( false, IO_CHEDENG_INVALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_CHEDENG_INVALID);
			}
		}
		break;

//	case IO_ZHENLING_VALID:
//		{
//			PRTMSG(MSG_NOR, "IOSta: IO_ZHENLING_VALID\n");		
//			_SetIoSta( false, IO_ZHENLING_VALID );		
//			if (g_objDiaodu.m_iodect > 0)
//			{
//				g_objDiaodu.show_diaodu(LANG_IOCHK_ZHENLING_VALID);
//			}
//		}
//		break;

//	case IO_ZHENLING_INVALID:
//		{
//			PRTMSG(MSG_NOR, "IOSta: IO_ZHENLING_INVALID\n");		
//			_SetIoSta( false, IO_ZHENLING_INVALID );		
//			if (g_objDiaodu.m_iodect > 0)
//			{
//				g_objDiaodu.show_diaodu(LANG_IOCHK_ZHENLING_INVALID);
//			}
//		}
//		break;
#endif

#if VEHICLE_TYPE == VEHICLE_M2
	case IO_QIANYA_VALID:
		{
			// 先检测是否断电
			byte bytSta;
			IOGet(IOS_POWDETM, &bytSta);

			if( bytSta == IO_POWDETM_VALID )	// 不是断电，则是欠压
			{
				PRTMSG(MSG_NOR, "IOSta: IO_QIANYA_VALID\n");		
				_SetIoSta( false, IO_QIANYA_VALID );		
				if (g_objDiaodu.m_iodect > 0)
				{
					g_objDiaodu.show_diaodu(LANG_IOCHK_QIANYA_VALID);
				}
			}
			else		// 若是断电，直接进行断电处理
			{
#if CHK_VER == 1	
				if (g_objDiaodu.m_iodect > 0)
				{
					//if( g_objPhone.IsPhoneValid() )	// 先检测手机模块是否正常	
					{
						for(int i=0; i<3; i++)
						{
							IOSet(IOS_YUYIN, IO_YUYIN_ON, NULL, 0);
							usleep(500000);
							IOSet(IOS_YUYIN, IO_YUYIN_OFF, NULL, 0);
							usleep(500000);
						}
					}
				}
#endif		
			}
			
		}
		break;

	case IO_QIANYA_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_QIANYA_INVALID\n");		
			_SetIoSta( false, IO_QIANYA_INVALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_QIANYA_INVALID);
			}
		}
		break;

	case IO_FDOORALERT_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_FDOORALERT_VALID\n");		
			_SetIoSta( false, IO_FDOORALERT_VALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_FDOORALERT_VALID);
			}
		}
		break;

	case IO_FDOORALERT_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_FDOORALERT_INVALID\n");		
			_SetIoSta( false, IO_FDOORALERT_INVALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_FDOORALERT_INVALID);
			}
		}
		break;

	case IO_BDOORALERT_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_BDOORALERT_VALID\n");		
			_SetIoSta( false, IO_BDOORALERT_VALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_BDOORALERT_VALID);
			}
		}
		break;
		
	case IO_BDOORALERT_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_BDOORALERT_INVALID\n");		
			_SetIoSta( false, IO_BDOORALERT_INVALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_BDOORALERT_INVALID);
			}
		}
		break;

	case IO_BDOOR_OPEN:
		{
			PRTMSG(MSG_NOR, "IO: BDOOR OPEN\n" );
			_SetIoSta( true, IOSTA_BDOOR_OPEN);	
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_BACKDOOROPEN);
			}
		}
		break;
		
	case IO_BDOOR_CLOSE:
		{		
			PRTMSG(MSG_NOR, "IO: BDOOR CLOSE\n" );
			_SetIoSta( false, IOSTA_BDOOR_OPEN);
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_BACKDOORCLOSE);
			}
		}
		break;
		
	case IO_JIAOSHA_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: JIAOSHA VALID\n");		
			_SetIoSta( true, IOSTA_JIAOSHA_VALID );
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_FOOTBRAKE);
			}
		}
		break;
		
	case IO_JIAOSHA_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: JIAOSHA INVALID\n");		
			_SetIoSta( false, IOSTA_JIAOSHA_VALID );
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_FOOTBRAKE_CANCEL);
			}
		}
		break;

	case IO_POWDETM_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_POWDETM_VALID\n");		
			_SetIoSta( false, IO_POWDETM_VALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_POWDETM_VALID);
			}
		}
		break;

	case IO_POWDETM_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_POWDETM_INVALID\n");		
			_SetIoSta( false, IO_POWDETM_INVALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_POWDETM_INVALID);
			}
		}
		break;

	case IO_CHEDENG_VALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_CHEDENG_VALID\n");		
			_SetIoSta( false, IO_CHEDENG_VALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_CHEDENG_VALID);
			}
		}
		break;

	case IO_CHEDENG_INVALID:
		{
			PRTMSG(MSG_NOR, "IOSta: IO_CHEDENG_INVALID\n");		
			_SetIoSta( false, IO_CHEDENG_INVALID );		
			if (g_objDiaodu.m_iodect > 0)
			{
				g_objDiaodu.show_diaodu(LANG_IOCHK_CHEDENG_INVALID);
			}
		}
		break;

//	case IO_ZHENLING_VALID:
//		{
//			PRTMSG(MSG_NOR, "IOSta: IO_ZHENLING_VALID\n");		
//			_SetIoSta( false, IO_ZHENLING_VALID );		
//			if (g_objDiaodu.m_iodect > 0)
//			{
//				g_objDiaodu.show_diaodu(LANG_IOCHK_ZHENLING_VALID);
//			}
//		}
//		break;

//	case IO_ZHENLING_INVALID:
//		{
//			PRTMSG(MSG_NOR, "IOSta: IO_ZHENLING_INVALID\n");		
//			_SetIoSta( false, IO_ZHENLING_INVALID );		
//			if (g_objDiaodu.m_iodect > 0)
//			{
//				g_objDiaodu.show_diaodu(LANG_IOCHK_ZHENLING_INVALID);
//			}
//		}
//		break;
#endif
	
	default:	
		break;
	}
}

//获取io状态
DWORD CIoSta::GetIoSta()
{
	DWORD dwRet = 0;
	
	pthread_mutex_lock( &m_MutexIoSta );
	
	dwRet = m_dwIoSta;
	
	pthread_mutex_unlock( &m_MutexIoSta );
	
	return dwRet;
}

//设置指定io状态
void CIoSta::_SetIoSta( bool v_bSet, DWORD v_dwVal )
{
	pthread_mutex_lock( &m_MutexIoSta );
	
	if( v_bSet ) m_dwIoSta |= v_dwVal;
	else m_dwIoSta &= ~v_dwVal;
	
	pthread_mutex_unlock( &m_MutexIoSta );
}
