#include "yx_QianStdAfx.h"

#undef MSG_HEAD 
#define MSG_HEAD	("QianExe-IrdModKaiTian")

void *G_ThreadIrdModKaiTianRecv(void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	g_objKTIrdMod.P_ThreadRecv();
	
	pthread_exit(0);
}

void *G_ThreadIrdModKaiTianWork(void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	g_objKTIrdMod.P_ThreadWork();
	
	pthread_exit(0);
}

CKTIrdMod::CKTIrdMod()
{
	m_iComPort = -1;
	m_pthreadRecv = 0;
	m_pthreadWork = 0;
	
	m_bEscRecv = false;
	m_bEscWork = false;
	
	memset( &m_objIrda, 0, sizeof(m_objIrda) );
	m_CurSta = stStart;
	m_SerLevel = LEVEL2;
}

CKTIrdMod::~CKTIrdMod()
{

}

int CKTIrdMod::Init()
{
	// 打开红外口
	if( !ComOpen() )
	{
		PRTMSG(MSG_ERR, "Open Ird failed\n");
		return ERR_COM;
	}
	
	// 创建接收线程
	if( pthread_create(&m_pthreadRecv, NULL, G_ThreadIrdModKaiTianRecv, NULL) != 0 )
	{
		Release();
		PRTMSG(MSG_ERR, "Create Recv Thread failed\n");
		return ERR_THREAD;
	}

#if USE_IRD_KAITIAN == 1
	if( pthread_create(&m_pthreadWork, NULL, G_ThreadIrdModKaiTianWork, NULL) != 0 )
	{
		Release();
		PRTMSG(MSG_ERR, "Create Work Thread failed\n");
		return ERR_THREAD;
	}
#endif

	return 0;
}

int CKTIrdMod::Release()
{
	if( m_pthreadRecv )
	{
		m_bEscRecv = true;
		usleep(100000);
		
		if( WaitForEvent(ThreadExit, &m_pthreadRecv, 3000) )
		{
			pthread_cancel(m_pthreadRecv);
			usleep(100000);
		}

		m_pthreadRecv = 0;
	}
	m_bEscRecv = false;
	
	if( m_pthreadWork )
	{
		m_bEscWork = true;
		usleep(100000);
		
		if( WaitForEvent(ThreadExit, &m_pthreadWork, 3000) )
		{
			pthread_cancel(m_pthreadWork);
			usleep(100000);
		}

		m_pthreadWork = 0;
	}
	m_bEscWork = false;
		
	ComClose();

	return 0;
}

bool CKTIrdMod::ComOpen()
{
	m_iComPort = open("/dev/misc/irdadrv", O_RDWR);
	if(m_iComPort == -1)
	{
		PRTMSG(MSG_ERR, "Ird Open failed\n");
		perror("");
		return false;
	}
	
	PRTMSG(MSG_DBG,"Ird Open Succ!\n"); 
	
	return true;
}

bool CKTIrdMod::ComClose()
{
	if(m_iComPort != -1)
	{
		close(m_iComPort);
		m_iComPort = -1;
		
		return true;
	}
	
	return false;
}

void P_PlayTxt(char *fmt)
{
#if USE_TTS == 1
	char str[1024] = {0};
	str[0] = 0x01;		// 0x01表示TTS数据
	str[1] = 0x00;		// 0x01表示立即播放本数据
	strcpy(str+2, fmt);
	DataPush(str, (DWORD)(2+strlen(fmt)), DEV_QIAN, DEV_DVR, LV2);
#endif
}
	
void CKTIrdMod::P_ThreadRecv()
{
	int iReadLen = 0;
	DWORD dwPacketNum;
	
	while( !g_bProgExit )
	{
		if( m_bEscRecv )
			break;

		iReadLen = read( m_iComPort, &m_objIrda, sizeof(IRKEY_INFO_T) ); 
		if( iReadLen>0 )
		{
			switch(m_objIrda.irkey.sys_id_code)
			{
				case 0x1818: //成都凯天服务评价遥控器
				{
					m_objIrdModMng.PushData((BYTE)LV1, 1, (char*)&(m_objIrda.irkey.irkey_code), dwPacketNum);
				}
					break;
				case 0x0000: //OSD菜单遥控器
				{
					char szBuf[2] = {0};
					szBuf[0] = 0x03;
					szBuf[1] = (char)m_objIrda.irkey.irkey_code;
					DataPush(szBuf, 2, DEV_QIAN, DEV_DVR, LV2);
				}
					break;
				default:
					break;
			}
		}
	}
	
	m_bEscRecv = false;
}

void CKTIrdMod::P_ThreadWork()
{
	byte cmd;
	byte bytLvl = 0;
	char szBuf = 0;
	DWORD dwLen = 0;
	DWORD dwPushTm;
	DWORD dwPacketNum;
	static UINT tick = 0;
	
	while( !g_bProgExit )
	{
		if( m_bEscWork )
			break;
		
		szBuf = 0;
		m_objIrdModMng.PopData(bytLvl, 1, dwLen, &szBuf, dwPushTm);
		
		switch(m_CurSta)
		{
			case stStart:
			{
				switch(szBuf)
				{
					case 0x0e:
						P_PlayTxt("感谢乘坐本公司出租车，请对我的服务进行评价");
						tick = GetTickCount();
						m_SerLevel = LEVEL2;
						m_CurSta = stServ;
						break;
					default:
						break;
				}
			}
				break;
			case stServ:
			{
				switch(szBuf)
				{
					case 0x0f:
						P_PlayTxt("确定");
						m_CurSta = stEnd;
						KTIrdEnd();
						break;
					case 0x10:
						P_PlayTxt("不满意");
						m_SerLevel = LEVEL1;
						break;
					case 0x11:
						P_PlayTxt("较满意");
						m_SerLevel = LEVEL2;
						break;
					case 0x12:
						P_PlayTxt("满意");
						m_SerLevel = LEVEL3;
						break;
					case 0x13:
						P_PlayTxt("非常满意");
						m_SerLevel = LEVEL4;
						break;
					default:
						if( GetTickCount()-tick>60*1000 )
						{
							P_PlayTxt( "超时无操作，采用默认评价");
							m_CurSta = stStart;
#if USE_METERTYPE == 2
							g_objKTMeter.SetSerValue(m_SerLevel);
#endif
						}
						break;
				}
			}
				break;
			case stEnd:
			{
				switch(szBuf)
				{
					case 0x14:
						P_PlayTxt("感谢您的评价");
						m_CurSta = stStart;
#if USE_METERTYPE == 2
						g_objKTMeter.SetSerValue(m_SerLevel);
#endif
						break;
					default:
						break;
				}
			}
				break;
			default:
				break;
		}
		
		usleep(50000);
	}
	
	m_bEscWork = false;
}

void CKTIrdMod::KTIrdStart()
{
	byte cmd = 0x0e;
	DWORD dwPacketNum;
	m_objIrdModMng.PushData((BYTE)LV1, 1, (char*)&cmd, dwPacketNum);
}

void CKTIrdMod::KTIrdEnd()
{
	byte cmd = 0x14;
	DWORD dwPacketNum;
	m_objIrdModMng.PushData((BYTE)LV1, 1, (char*)&cmd, dwPacketNum);
}
