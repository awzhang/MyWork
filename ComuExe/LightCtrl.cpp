#include "ComuStdAfx.h"

#if VEHICLE_TYPE == VEHICLE_M

void *G_ThreadLightCtrl(void *arg)
{
	g_objLightCtrl.P_ThreadLightCtrl();
}

#undef MSG_HEAD
#define MSG_HEAD	("Comu-LightCtrl")

//////////////////////////////////////////////////////////////////////////

CLightCtrl::CLightCtrl()
{
	m_bytLightSta = ChangLiang;
	m_pthreadLightCtrl = 0;
}

CLightCtrl::~CLightCtrl()
{

}

int CLightCtrl::Init()
{
	// 创建控制灯线程
	if( pthread_create(&m_pthreadLightCtrl, NULL, G_ThreadLightCtrl, NULL) != 0 )
	{
		PRTMSG(MSG_ERR, "create light thread failed!\n");
		return ERR_THREAD;
	}

	PRTMSG(MSG_ERR, "create light thread succ!\n");
	return 0;
}

int CLightCtrl::Release()
{

}

void CLightCtrl::P_ThreadLightCtrl()
{
	int iSleepTime = 1000;
	static int sta_iManShanCnt = 0;
	static int sta_iKuaiShanCnt = 0;

	BYTE  bytLvl = 0;
	char  szbuf[10] = {0};
	DWORD dwLen = 0;
	DWORD dwPushTm;
	
	while( !g_bProgExit )
	{
		if( !m_objRecvCtrlMng.PopData(bytLvl, sizeof(szbuf), dwLen, szbuf, dwPushTm) )
		{
			if( 1 == dwLen )
			{
				switch( szbuf[0] )
				{
				case 0x01:		// 请求常亮
					m_bytLightSta = ChangLiang;
					PRTMSG(MSG_DBG, "Light ChangLiang!\n");
					break;
					
				case 0x02:		// 请求快闪
					m_bytLightSta = KuaiShan;
					PRTMSG(MSG_DBG, "Light Kuaishan!\n");
					break;
					
				case 0x03:		// 请求慢闪
					m_bytLightSta = ManShan;
					PRTMSG(MSG_DBG, "Light Manshan!\n");
					break;
					
				default:
					m_bytLightSta = ChangLiang;
					break;				
				}
			}
		}

		switch( m_bytLightSta )
		{
		case ChangLiang:
			{
				IOSet(IOS_APPLEDPOW, IO_APPLEDPOW_ON, NULL, 0);

				iSleepTime = 1000;
			}
			break;
			
		case KuaiShan://半秒亮半秒灭
			{
				if( sta_iKuaiShanCnt++ % 2 == 0 )
					IOSet(IOS_APPLEDPOW, IO_APPLEDPOW_ON, NULL, 0);
				else
					IOSet(IOS_APPLEDPOW, IO_APPLEDPOW_OFF, NULL, 0);

				iSleepTime = 500;
			}
			break;
			
		case ManShan://两秒亮两秒灭
			{
				if( sta_iManShanCnt++ % 2 == 0 )
					IOSet(IOS_APPLEDPOW, IO_APPLEDPOW_ON, NULL, 0);
				else
					IOSet(IOS_APPLEDPOW, IO_APPLEDPOW_OFF, NULL, 0);
				
				iSleepTime = 1000;
			}
			break;
			
		default:
			m_bytLightSta = KuaiShan;
			iSleepTime = 1000;
			break;
		}

		usleep(iSleepTime*1000);
	}
}

void CLightCtrl::SetLightSta(char v_szSta)
{
	DWORD	dwPackNum;
	m_objRecvCtrlMng.PushData(LV1, 1, &v_szSta, dwPackNum);
}

#endif
