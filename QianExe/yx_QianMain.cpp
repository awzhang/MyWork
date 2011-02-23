#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-Main     ")

#define MAX_DEAL_BUF_SIZE	2048

int _InitAll()
{
	// 创建消息队列
	g_MsgID = msgget((key_t)1236, 0666 | IPC_CREAT);
	if( g_MsgID == -1 )
	{
		perror("msgget error:");
		return 1;
	}

	// 定时器初始化需放在最前面
	g_objTimerMng.Init();	

	//g_objFlashPart4Mng.Init();

#if SMALL_VER == 0	
	g_objMonAlert.Init();
#endif
	g_objQianIO.Init();
#if SMALL_VER == 0
	g_objCarSta.Init();
#endif
	g_objSpecCtrl.Init();
#if SMALL_VER == 0	
	g_objPhoto.Init();
#endif	
//	g_objKTDriverCtrl.Init();
//	g_objKTIrdMod.Init();

#if USE_LIAONING_SANQI == 1
	g_objApplication.Init();
	
	g_objTransport.Init();
	
	g_objNetWork.Init();
#if SMALL_VER == 0	
	g_objReport.Init();
#endif	
	g_objLNDownLoad.Init();
#if SMALL_VER == 0
	g_objLNPhoto.Init();
#endif
#endif

#if USE_REALPOS == 1
	g_objRealPos.Init();
#endif

#if USE_DRIVERCTRL == 1
	g_objDriverCtrl.Init();
#endif

#if USE_COMBUS == 1
	g_objComBus.Init();
#endif

#if USE_COMBUS == 2
	g_objCom150TR.Init();
#endif

#if USE_BLK == 1
	g_objBlk.Init();
#endif

#if USE_AUTORPT == 1
	g_objAutoRpt.Init();
#endif

#if USE_LEDTYPE == 1
	g_objLedBohai.Init();
#endif

#if USE_LEDTYPE == 2
	g_objLedChuangLi.Init();
#endif

#if USE_LEDTYPE == 3
	g_objKTLed.Init();
#endif

#if USE_DRVREC == 1
	g_objDriveRecord.Init();
#endif

#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 || USE_DRVRECRPT == 11
	g_objDrvRecRpt.Init();
#endif

#if USE_METERTYPE == 1
	g_objMeter.Init();
#endif

#if USE_METERTYPE == 2
	g_objKTMeter.Init();
#endif

#if USE_ACDENT == 1 || USE_ACDENT == 2
	g_objAcdent.Init();
#endif

#if USE_OIL == 3
	g_objOil.Init();
#endif

	return 0;
}

void _ReleaseAll()
{
#if USE_AUTORPTSTATION == 1
	g_objAuoRptStationNew.Release();
#endif

#if USE_OIL == 3
	g_objOil.Release();
#endif

#if USE_ACDENT == 1 || USE_ACDENT == 2
	g_objAcdent.Release();
#endif

#if USE_METERTYPE == 1
	g_objMeter.Release();
#endif

#if USE_METERTYPE == 2
	g_objKTMeter.Release();
#endif

#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 || USE_DRVRECRPT == 11
	g_objDrvRecRpt.Release();
#endif

#if USE_LEDTYPE == 1
	g_objLedBohai.Release();
#endif

#if USE_LEDTYPE == 2
	g_objLedChuangLi.Release();
#endif

#if USE_LEDTYPE == 3
	g_objKTLed.Release();
#endif

#if USE_COMBUS == 1
	g_objComBus.Release();
#endif

#if USE_COMBUS == 2
	g_objCom150TR.Release();
#endif

#if USE_DRVREC == 1
	g_objDriveRecord.Release();
#endif

#if USE_DRIVERCTRL == 1
	g_objDriverCtrl.Release();
#endif

#if USE_REALPOS == 1
	g_objRealPos.Release();
#endif

//	g_objKTIrdMod.Release();
//	g_objKTDriverCtrl.Release();
	
#if USE_LIAONING_SANQI == 1
#if SMALL_VER == 0
	g_objReport.Release();
#endif	
	g_objApplication.Release();
	
	g_objTransport.Release();
	
	g_objNetWork.Release();

	g_objLNDownLoad.Release();
#if SMALL_VER == 0
	g_objLNPhoto.Release();
#endif
#endif	
	g_objSpecCtrl.Release();
#if SMALL_VER == 0
	g_objCarSta.Release();
#endif
	g_objQianIO.Release();

	g_objTimerMng.Release();

	msgctl(g_MsgID, IPC_RMID, NULL);
}

void *G_RecvDataThread(void* arg)
{
	char szbuf[MAX_DEAL_BUF_SIZE] = {0};
	DWORD dwLen;
	BYTE  bytLvl;
	DWORD dwPushTm;
	DWORD dwSrcSymb;
	QIANMSG msg;
	int iResult;

	while( !(iResult = DataWaitPop(DEV_QIAN)) )
	{
		if( g_bProgExit )  
		{ 
			break; 
		} 
		
		while( !DataPop(szbuf, MAX_DEAL_BUF_SIZE, &dwLen, &dwSrcSymb, DEV_QIAN, &bytLvl) )
		{ 
			if( dwLen < 1 ) continue;	//保险处理
			
			DWORD dwPacketNum = 0;
			
			switch( dwSrcSymb )
			{
			case DEV_SOCK:
				g_objSock.AnalyseSockData(szbuf, (int)dwLen);
				break;
				
			case DEV_DIAODU:
				g_objRecvComuDataMng.PushData((BYTE)LV1, dwLen, (char*)szbuf, dwPacketNum, DATASYMB_DIAODUTOQIAN);
				break;
				
			case DEV_PHONE:
				g_objRecvComuDataMng.PushData((BYTE)LV1, dwLen, (char*)szbuf, dwPacketNum, DATASYMB_PHONETOQIAN);
				break;
				
			case DEV_GPS:
				g_objRecvComuDataMng.PushData((BYTE)LV1, dwLen, (char*)szbuf, dwPacketNum, DATASYMB_GPSTOQIAN);
				break;
				
			case DEV_IO:
				g_objRecvIODataMng.PushData((BYTE)LV1, dwLen, (char*)szbuf, dwPacketNum);
				break;
				
			case DEV_DVR:
				g_objRecvDvrDataMng.PushData((BYTE)LV1, dwLen, (char*)szbuf, dwPacketNum);
				break;
				
			default:
				break;
			}

			usleep(100000);
		} 
	} 
	
	if(ERR_MSGSKIPBLOCK == iResult)
	{
		// DataWaitPop返回出错，可能以后需要添加一些清除操作
	}

	// 若接收线程退出，则整个程序退出
	g_bProgExit = true;
	
	msg.m_lMsgType = MSG_PROGQUIT;
	msgsnd(g_MsgID, (void*)&msg, MSG_MAXLEN, 0);
}

void *G_DealSmsThread(void* arg)
{
	char  szbuf[ MAX_DEAL_BUF_SIZE ];
	DWORD dwLen;
	BYTE  bytLvl;
	BYTE  bytSrcSymb;
	DWORD dwPushTm;
	char  szTel[ 15 ] = { 0 };	
	
	while( !g_bProgExit )
	{
		usleep(100000);
		
		while( !g_objRecvSmsDataMng.PopData(bytLvl, (DWORD)sizeof(szbuf), dwLen, (char*)szbuf, dwPushTm, &bytSrcSymb) )
		{
			if( dwLen < 1 )
				continue;

			g_objCfg.GetSelfTel( szTel, sizeof(szTel) );
			g_objSms.DealSmsFrame((char*)szbuf, (int)dwLen, szTel, sizeof(szTel), bytSrcSymb == DATASYMB_RCVFROMSM);
		}
	}
}

void *G_DealIOThread(void* arg)
{
	char  szbuf[100];
	DWORD dwLen;
	BYTE  bytLvl;
	BYTE  bytSrcSymb;
	DWORD dwPushTm;
	
	while( !g_bProgExit )
	{
		usleep(100000);
		
		while( !g_objRecvIODataMng.PopData(bytLvl, (DWORD)sizeof(szbuf), dwLen, (char*)szbuf, dwPushTm, &bytSrcSymb) )
		{
			if( dwLen != 1 )
				continue;

			g_objQianIO.DealIOMsg(szbuf, dwLen, bytSrcSymb);
		}
	}
}

void *G_DealComuThread(void *arg)
{
	char  szbuf[ MAX_DEAL_BUF_SIZE ];
	DWORD dwLen;
	BYTE  bytLvl;
	BYTE  bytSrcSymb;
	DWORD dwPushTm;

	while( !g_bProgExit )
	{
		usleep(100000);

		while( !g_objRecvComuDataMng.PopData(bytLvl, (DWORD)sizeof(szbuf), dwLen, (char*)szbuf, dwPushTm, &bytSrcSymb) )
		{
			if( dwLen < 1 )
				continue;

			g_objComu.DealComuMsg(szbuf, dwLen, bytSrcSymb);
		}
	}
}

void *G_DealDvrThread(void *arg)
{
	char  szbuf[ MAX_DEAL_BUF_SIZE ];
	DWORD dwLen;
	char  szSendBuf[ TCP_SENDSIZE ];
	int   iSendLen = 0;
	BYTE  bytLvl;
	BYTE  bytSrcSymb;
	DWORD dwPushTm;

	while( !g_bProgExit )
	{
		usleep(100000);
		
		while( !g_objRecvDvrDataMng.PopData(bytLvl, (DWORD)sizeof(szbuf), dwLen, (char*)szbuf, dwPushTm, &bytSrcSymb) )
		{
			if( dwLen < 1 )
				continue;
			
			switch( szbuf[0] )
			{
			case 0x01:	//DvrExe向通信前置机发送的数据
				{
					int iRet = g_objSms.MakeSmsFrame( szbuf+3, (int)dwLen-3, szbuf[1], szbuf[2], szSendBuf, sizeof(szSendBuf), iSendLen); 
					
					if( !iRet )
					{
						g_objSock.SOCKSNDSMSDATA( szSendBuf, iSendLen, LV12 );
					}
				}
				break;

			case 0x02:	// 拍照完成通知
				{
					DWORD dwPacketNum;
#if USE_LIAONING_SANQI == 1
					g_objLNPhoto.m_objDataMngShotDone.PushData(LV1, dwLen-1, szbuf+1, dwPacketNum);
#else
					g_objPhoto.m_objDataMngShotDone.PushData(LV1, dwLen-1, szbuf+1, dwPacketNum);
#endif
					QIANMSG msg;
					msg.m_lMsgType = MSG_PHOTODONE;
					msgsnd(g_MsgID, (void*)&msg, MSG_MAXLEN, 0);
				}
				break;

			case 0x03:	// 系统进入省电通知
				{
					g_objQianIO.SetDevSta(true, DEVSTA_SYSTEMSLEEP);
				}
				break;

			case 0x04:	// 系统退出省电通知
				{
					g_objQianIO.SetDevSta(false, DEVSTA_SYSTEMSLEEP);
				}
				break;

			case 0x05:	// SD卡挂载成功通知
				{
#if USE_BLK == 1
					g_objBlk.BlkSaveStart();
#endif
#if USE_DRVREC == 1
					g_objDriveRecord.ReInit();
#endif
#if USE_AUTORPTSTATION == 1
					sleep(3);
					g_objAuoRptStationNew.init();
#endif

					//初始化手柄存储文件（理应放在ComuExe进行，但在此处进行也可）
					FILE* fp;
					char path[100] = {0};
					sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
					fp = fopen(path, "rb");
					
					//如果文件不存在则创建
					if(fp == NULL) 
					{	
						fp = fopen(path, "wb+");
						
						if (fp != NULL) 
						{
							fwrite(0, 8192*2, 1, fp);	//存储空间大小为11000byte=10.7K
							fclose(fp);
						}
					}
					else
					{
						fclose(fp);
					}
				}
				break;

			case 0x06:	// SD卡卸载成功通知
				{
#if USE_BLK == 1
					g_objBlk.BlkSaveStop();
#endif
				}
				break;
#if USE_LIAONING_SANQI == 1
			case 0x07: //DVR在线状态
				{
					g_objSock.m_bDVRTcpLoged = szbuf[1];
				}
				break;
#endif
			default:
				break;
			}
		}
	}
}

void *G_ClrDogThread(void *arg)
{
	int i = 0;
	while( !g_bProgExit )
	{
		if( i++ % 5 == 0 )
		{
			DogClr( DOG_QIAN );
		}

		sleep(1);
	}
}


int main()
{
	ListPhoneInfo();
	
	pthread_t pRecvData, pDealSms, pDealIO, pDealDiaodu, pDealDvr, pthread_ClrDog;
	QIANMSG objMsg;

 	// 所有初始化操作均放在此函数内
 	_InitAll();	
 
	// 创建一个接收子线程、四个业务处理子线程、清看门狗线程
	if( pthread_create(&pRecvData, NULL, G_RecvDataThread, NULL) != 0
		|| pthread_create(&pDealSms, NULL, G_DealSmsThread, NULL) != 0
		|| pthread_create(&pDealIO, NULL, G_DealIOThread, NULL) != 0
		|| pthread_create(&pDealDiaodu, NULL, G_DealComuThread, NULL) != 0
		|| pthread_create(&pDealDvr, NULL, G_DealDvrThread, NULL) != 0
		|| pthread_create(&pthread_ClrDog, NULL, G_ClrDogThread, NULL) != 0
	  )
	{
		perror("create deal thread failed: ");
		_ReleaseAll();
				
		return ERR_THREAD;
	}

	PRTMSG(MSG_NOR, "QianExe begin to run!\n");

	// 主线程
	while( !g_bProgExit )
	{
		if( msgrcv(g_MsgID, (void*)&objMsg, MSG_MAXLEN, 0, 0) == -1)
		{
			PRTMSG(MSG_ERR, "QianExe msgrev failed:")
			perror("");

			usleep(100000);
			continue;
		}

		switch( objMsg.m_lMsgType )
		{
#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 || USE_DRVRECRPT == 11
		case MSG_SAVRECSTA:
			g_objDrvRecRpt.RecStaSav();
			break;

		case MSG_DRVECDTOSND:
			g_objDrvRecRpt.DrvRecToSnd( (bool)objMsg.m_uszMsgBuf[0] );
			break;
#endif
		case MSG_PROGQUIT:
			PRTMSG(MSG_NOR, "Recv quit msg, program quit!\n");
			g_bProgExit = true;
			break;

		case MSG_TCPTOSEND:
			g_objSock.SendTcpBufData();
			break;

		case MSG_PHOTODONE:
#if USE_LIAONING_SANQI == 1
			g_objLNPhoto.DealOnePhotoEnd();
#else
			g_objPhoto.DealOnePhotoEnd();
#endif
			break;

		default:
			break;
		}

		usleep(100000);
	}

	g_bProgExit = true;
	_ReleaseAll();

	//QianExe退出会使系统立即复位，原因暂不明，此处让其sleep， 等待ComuExe调用reboot复位
	sleep(15);

	PRTMSG(MSG_NOR, "QianExe is now to quit\n");

	return 1;
}
