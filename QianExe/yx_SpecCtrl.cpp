// SpecCtrl.cpp: implementation of the CSpecCtrl class.
//
//////////////////////////////////////////////////////////////////////
//#include "stdafx.h"
//#include "qianexe.h"
#include "yx_QianStdAfx.h"
#include "yx_SpecCtrl.h"

// #ifdef _DEBUG
// #undef THIS_FILE
// static char THIS_FILE[]=__FILE__;
// #define new DEBUG_NEW
// #endif

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-SpecCtrl	")
 
#if USE_LANGUAGE == 0
#define LANG_OILTOCUT ("秒后完全切断油路,请靠边停车,并注意安全!")
#define LANG_OILCUT_SUCC ("油路已被切断!!")
#define LANG_OILCONN_SUCC ("油路已接通！！")
#endif

#if USE_LANGUAGE == 11
#define LANG_OILTOCUT (" Secs Later Cut Oil Supply, Stop and Keep Safe!")
#define LANG_OILCUT_SUCC ("Oil Supply Cutted !!")
#define LANG_OILCONN_SUCC ("Oil Supply Resumed !!")
#endif


void *G_CloseOilElec(void *arg)
{
	g_objSpecCtrl.DisconnOilElec();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpecCtrl::CSpecCtrl()
{
 	m_hThdCtrl = 0;
 	m_bThdCtrlExit = false;

// 	m_hThdCtrl = NULL;
// 	m_hEvtThdCtrlExit = ::CreateEvent( NULL, TRUE, TRUE, NULL ); // 手工复位(必须),初始为信号态
}

CSpecCtrl::~CSpecCtrl()
{
	// 不要在这里调用Release;
}

void CSpecCtrl::Init()
{
#if USE_DISCONNOILELEC == 1
	unsigned char ucResult = 0;
	if( !IOGet(IOS_ACC, &ucResult) )
	{
		AccChg( IO_ACC_ON == ucResult );
	}	
#endif
}

void CSpecCtrl::Release()
{
	// 通知子线程退出
	m_bThdCtrlExit = true;
	usleep(100000); // 为了让子模块繁忙的子线程正常退出，主线程sleep等待一下较好

	// 关闭断油断电线程
	if( m_hThdCtrl )
	{
		void* pThdRet;
		int iResult = 0;
		
		DWORD dwCt = 0;
		while( 1 )
		{
			iResult = pthread_kill( m_hThdCtrl, 0);
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
		
		m_hThdCtrl = 0;
	}

// 	// 回收资源
// 	if( m_hEvtThdCtrlExit )
// 	{
// 		::CloseHandle( m_hEvtThdCtrlExit );
// 		m_hEvtThdCtrlExit = NULL;
// 	}
}

/// 远程遥控请求 (业务处理优先)
int CSpecCtrl::Deal0601( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen )
{
	char buf[100] = { 0 };
	int iBufLen = 0;
	int iRet = 0;
	BYTE bytResType = 0;
	bool bOilCut = false;
	unsigned char ucResult = 0;

	// 参数检查
	if( 2 != v_dwDataLen )
	{
		iRet = ERR_PAR;
		goto DEAL0601_FAILED;
	}

#if USE_DISCONNOILELEC == 1

	// 预生成成功应答帧
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x06, 0x41, buf, sizeof(buf), iBufLen);
	if( iRet ) goto DEAL0601_FAILED;

	// 断合油电处理
	if( !(0x40 & v_szData[0]) ) // 若要进行油电控制
	{
		if( !IOGet(IOS_ACC, &ucResult) )
		{
			if( IO_ACC_ON == ucResult)
			{
				if( 0x40 & v_szData[1] ) // 若要进行切断
				{
					PRTMSG( MSG_NOR, "Deal0601 - To Disconnect Oil Task\n" );

					if( !m_hThdCtrl || (m_hThdCtrl && ESRCH == pthread_kill(m_hThdCtrl, 0)) ) // 若关闭油电线程没有运行
					{
// 						// 关闭上次线程句柄 (保险起见)
// 						if( m_hThdCtrl ) ::CloseHandle( m_hThdCtrl );

						// 设置线程属性
						int iResult = 0;
						pthread_attr_t objThreadAttr;
						iResult = pthread_attr_init( &objThreadAttr );
						if( iResult )
						{
							PRTMSG( MSG_ERR, "Deal0601 - pthread_attr_init fail %d\n", iResult );
							iRet = ERR_THREAD;
							goto DEAL0601_FAILED;
						}
 						iResult = pthread_attr_setdetachstate( &objThreadAttr, PTHREAD_CREATE_DETACHED ); // 设置为分离线程
 						if( iResult )
 						{
							PRTMSG( MSG_ERR, "Deal0601 - pthread_setdetachstate fail %d\n", iResult );
							iRet = ERR_THREAD;
							goto DEAL0601_FAILED;
						}

						// 清除线程退出标志
						m_bThdCtrlExit = false;

						// 重建新线程
						iResult = pthread_create( &m_hThdCtrl, &objThreadAttr, G_CloseOilElec, NULL );
						if( iResult || 0 == m_hThdCtrl )
						{
							PRTMSG( MSG_ERR, "Deal0601 - pthread_create fail %d\n", iResult );
							iRet = ERR_THREAD;
							goto DEAL0601_FAILED;
						}

						// 启动新线程
// 						::SetThreadPriority( m_hThdCtrl, THREAD_PRIORITY_TIME_CRITICAL );
// 						::ResumeThread( m_hThdCtrl );
					}

					bOilCut = true;
				}
				else // 否则,即是要进行连通
				{
					PRTMSG( MSG_NOR, "Deal0601 - To Connect Oil Task\n" );

					if( m_hThdCtrl && ESRCH != pthread_kill(m_hThdCtrl, 0) ) // 若正在运行关闭油电线程
					{
						// 设置线程退出标志
						m_bThdCtrlExit = true;

						// 等待线程退出
						DWORD dwCt = 0;
						while( ESRCH != pthread_kill(m_hThdCtrl, 0) )
						{
							if( dwCt ++ > 25 )
							{
								PRTMSG( MSG_ERR, "Deal0601 - Wait CtrlThread Exit Timeout!\n" );
								iRet = ERR_THREAD;
								goto DEAL0601_FAILED;
								break;
							}

							usleep( 200000 );
						}
					}

					// 合油合电
					g_objQianIO.ConnectOilElec();

					char buf[200] = {0};
					buf[0] = (char)0x01;
					buf[1] = (char)0x72;
					buf[2] = (char)0x02;
					sprintf(buf+3, "%s", LANG_OILCONN_SUCC);
					DataPush( buf, strlen(buf), DEV_QIAN,  DEV_DIAODU, 1 );
					
#if USE_TTS == 1
					memset( buf, 0, sizeof(buf) );
					buf[0] = 0x01;
					buf[1] = 0x01;
					strcpy(buf+2, LANG_OILCONN_SUCC);
					DataPush( buf, strlen(buf) + 2, DEV_QIAN, DEV_DVR, 1 );
#endif

					bOilCut = false;
				}
			}
		}
	}
	else
	{
		PRTMSG( MSG_NOR, "acc invalid ,keep old station\n" );
	}


	// 断油断电配置保存
	tagSpecCtrlCfg objCfg;
	objCfg.Init();
	objCfg.m_bOilCut = bOilCut;
	SetSecCfg( &objCfg.m_bOilCut, offsetof(tagSecondCfg, m_uni2QSpecCtrlCfg.m_obj2QSpecCtrlCfg.m_bOilCut),
		sizeof(objCfg.m_bOilCut) );

	// 发送成功应答
	// 080917,根据任务单080899修改,只允许报警数据、位置查询应答和短信通道下发设置的应答走短信通道
	g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV14, 0 );

	return 0;

#endif

DEAL0601_FAILED:

	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x06, 0x41, buf, sizeof(buf), iBufLen);
	// 080917,根据任务单080899修改,只允许报警数据、位置查询应答和短信通道下发设置的应答走短信通道
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV15, 0 );

	return iRet;
}

void CSpecCtrl::AccChg(bool v_bOn )
{
#if USE_DISCONNOILELEC == 1
	if( v_bOn ) // 若ACC有效
	{
		tagSpecCtrlCfg objCfg;
		objCfg.Init();
		::GetSecCfg( &objCfg, sizeof(objCfg), offsetof(tagSecondCfg, m_uni2QSpecCtrlCfg.m_obj2QSpecCtrlCfg),
			sizeof(objCfg) );

		if( objCfg.m_bOilCut )
		{
			g_objQianIO.DisconnOilElec();
		}
		else
		{
			g_objQianIO.ConnectOilElec();
		}
	}
	else
	{
		g_objQianIO.ConnectOilElec();
	}
#endif
}

// 子线程运行的函数，是线程安全的，不过也不与其他线程共享数据
void CSpecCtrl::DisconnOilElec()
{
	DWORD dwBegin = GetTickCount();
	DWORD dwCur = dwBegin;
	DWORD dwLeave = 30;
	WORD wCt = 0;
	bool bDisconn = true;
	BYTE bytSta = 0; // 0,初始态; 1,提示态; 2,执行态

	char frm72[200] = {0};
	frm72[0] = (char)0x01;
	frm72[1] = (char)0x72;
	frm72[2] = (char)0x02;

	char frmTTS[200] = { 0 };
	frmTTS[0] = (char)0x01;
	frmTTS[1] = (char)0x01;

	pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, 0 ); // 忽略取消消息

	while( !m_bThdCtrlExit )
	{
		switch( bytSta )
		{
		case 0: // 初始态
#if USE_TTS == 1
			sprintf( frmTTS+2, "%d秒后将切断油路，请靠边停车！", DISCONNOILELEC_PERIOD / 1000 );
			DataPush( frmTTS, strlen(frmTTS), DEV_QIAN, DEV_DVR, 1 ); 
			sleep(10);
			dwBegin += 10000;
#endif			
			sprintf(frm72+3, "%d%s", DISCONNOILELEC_PERIOD / 1000, LANG_OILTOCUT );
			DataPush( frm72, strlen(frm72), DEV_QIAN, DEV_DIAODU, 1 );

			bytSta = 2; // 转入执行态
			break;

		case 1: // 提示态
			{
				if( GetTickCount() - dwBegin >= 14000 ) // 增加的时间是为了计算播放提示的时间
				{
					dwBegin = GetTickCount(); // 重置起始时间
					bytSta = 2; // 转入执行态
				}
				else
				{
					usleep( 200000 );
				}
			}
			break;

		case 2: // 执行态
			{
				if( bDisconn )
				{
					dwCur = GetTickCount();

					if( 0 == ++wCt%10 )
					{
						if( dwCur-dwBegin<DISCONNOILELEC_PERIOD )
						{
							dwLeave = (DISCONNOILELEC_PERIOD - (dwCur - dwBegin)) / 1000;
							if(dwLeave)
							{
#if USE_TTS == 1
								sprintf(frmTTS+2, "%d秒后将切断油路，请靠边停车！", dwLeave );
 								DataPush( frmTTS, strlen(frmTTS), DEV_QIAN, DEV_DVR, 1 );
 								sleep(10);
 								dwCur += 10000;
								dwBegin += 10000;
#endif								
								sprintf(frm72+3, "%d%s", dwLeave, LANG_OILTOCUT );
								DataPush( frm72, strlen(frm72), DEV_QIAN, DEV_DIAODU, 1 );
							}
						}
						else
						{
							dwLeave = 0;
						}
					}

					/// ！！！特别注意：断油断电动作改为实际只作一次,以免反复开闭造成继电器振荡
					//g_objIO.DisconnOilElec();
					if( dwCur - dwBegin >= DISCONNOILELEC_PERIOD )
					{
						g_objQianIO.DisconnOilElec();
						goto DISCONNOILELEC_END;
					}
					usleep( 700000 );
				}
				else
				{
					/// ！！！特别注意：断油断电动作改为实际只作一次,以免反复开闭造成继电器振荡
					//g_objIO.ConnectOilElec();
					usleep( 300000 );
				}

				bDisconn = !bDisconn;
			}
			break;

		default:
			g_objQianIO.ConnectOilElec();
			goto DISCONNOILELEC_END;
		}
	}

DISCONNOILELEC_END:
	if( m_bThdCtrlExit ) // 若是被中止线程
	{
		g_objQianIO.ConnectOilElec();
	}
	else
	{
		sprintf(frm72+3, "%s", LANG_OILCUT_SUCC );
		DataPush( frm72, strlen(frm72), DEV_QIAN,  DEV_DIAODU, 1 );

#if USE_TTS == 1
		strcpy(frmTTS+2, LANG_OILCUT_SUCC);
		DataPush( frmTTS, strlen(frmTTS), DEV_QIAN, DEV_DVR, 1 );
#endif
	}
}
