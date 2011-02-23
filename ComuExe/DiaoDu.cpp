#include "ComuStdAfx.h"


#undef MSG_HEAD
#define MSG_HEAD ("ComuExe-DiaoDu:")

#if CHK_VER == 1
	#define ALL_PASS	""				//调度屏通用密码
	#define LIU_PASS	"4312896654"	//流媒体参数配置密码
	#define EXP_PASS	"1122334455"	//调试模式密码(工程菜单输入此密码即进入调试模式)
#else 
	#define ALL_PASS	"4312897765"	//调度屏通用密码
	#define LIU_PASS	"4312896654"	//流媒体参数配置密码
	#define EXP_PASS	"1122334455"	//调试模式密码(工程菜单输入此密码即进入调试模式)
#endif


#define build_frm(a, b, c)	if(a)	{ strcpy(frm+len, b); len+=strlen(b); }  \
							else	{ strcpy(frm+len, c); len+=strlen(c); }

#define build_frm2(a, b, c, d)	if(a)	{ sprintf(frm+len, "%s:%s,", b, c); len+=strlen(b)+strlen(c)+2; }  \
								else	{ sprintf(frm+len, "%s:%s,", b, d); len+=strlen(b)+strlen(d)+2; }


bool UDiskMount(void *Event)
{
	if( access("/mnt/UDisk/part1/flash", F_OK)!=0 && access("/mnt/UDisk/part1/data/disk", F_OK)==0 )
		return true;
	else
		return false;
}

// void *G_DiaoduReadThread(void *arg)
// {
// 	g_objDiaodu.P_DiaoduReadThread();
// }

void *G_DiaoDuWorkThread(void *arg)
{
	g_objDiaodu.P_DiaoduWorkThread();
}

void *G_ExeUpdateThread(void *arg)
{
	g_objDiaodu.P_ExeUpdateThread();
}

void *G_ImageUpdateThread(void *arg)
{
	g_objDiaodu.P_ImageUpdateThread();
}

void *G_AppUpdateThread(void *arg)
{
	g_objDiaodu.P_AppUpdateThread();
}

void *G_SysUpdateThread(void *arg)
{
	g_objDiaodu.P_SysUpdateThread();
}

void *G_DataOutPutThread(void *arg)
{
	//g_objDiaodu.P_DataOutPutThread();
	g_objDiaodu.P_VideoOutPutThread();
}

void *G_VideoOutPutThread(void *arg)
{
	g_objDiaodu.P_VideoOutPutThread();
}

void *G_IoDetect(void *arg)
{
	g_objDiaodu._IoDetectThread();
}

//////////////////////////////////////////////////////////////////////////
CDiaodu::CDiaodu()
{
//	m_iComPort = 0;
	m_pthreadDiaoduWork = 0;
//	m_pthreadDiaoduRead = 0;

	m_gpsdect = 0;
	m_iodect = 0;

	memset(m_szComFrameBuf,0, sizeof(m_szComFrameBuf));
	m_iComFrameLen = 0;
}

CDiaodu::~CDiaodu()
{
	
}

int CDiaodu::Init()
{
	IOSet(IOS_SCHEDULEPOW, IO_SCHEDULEPOW_ON, NULL, 0);//调度屏上电

// #if USE_COMBUS == 0		// 若使用串口扩展盒，则不打开串口
// 	// 打开串口
// 	if( false == ComOpen() )
// 		return ERR_COM;
// 
// 	// 创建调度接收线程
// 	if( pthread_create(&m_pthreadDiaoduRead, NULL, G_DiaoduReadThread, NULL) != 0 )
// 	{
// 		PRTMSG(MSG_ERR, "Create Diaodu read thread failed!\n");
// 		perror(" ");
// 
// 		return ERR_THREAD;
// 	}
// #endif

	// 创建调度处理线程
	if( pthread_create(&m_pthreadDiaoduWork, NULL, G_DiaoDuWorkThread, NULL) != 0 )
	{
		PRTMSG(MSG_ERR, "Create Diaodu thread failed!\n");
		perror(" ");
		
		return ERR_THREAD;
	}

	return 0;
}

int CDiaodu::Release()
{
// #if USE_COMBUS == 0
// 	ComClose();		// 串口关闭之后，读串口函数将返回错误，读线程退出
// #endif
	return 0;
}

// void CDiaodu::P_DiaoduReadThread()
// {
// 	char szReadBuf[1024] = {0};
// 	int  iBufLen = sizeof(szReadBuf);
// 	int  iReadLen = 0;
// 	int  i = 0;
// 	byte bytLvl = 0;
// 	DWORD dwPushTm;
// 
// 	while(!g_bProgExit) 
// 	{
// 		iReadLen = read(m_iComPort, szReadBuf, iBufLen);
// 
// //		PRTMSG(MSG_DBG, "diao recv data\n");
// //		PrintString(szReadBuf, iReadLen);
// 
// 		if(iReadLen > iBufLen)
// 		{
// 			PRTMSG(MSG_ERR,"Com Read Over Buf!\n");
// 			memset(szReadBuf, 0, sizeof(szReadBuf));
// 			iReadLen = 0;
// 
// 			continue;
// 		}		
// 
// 		if( iReadLen > 0 )
// 		{
// 			AnalyseComFrame(szReadBuf, iReadLen);
// 		}
// 	}
// }

void CDiaodu::P_DiaoduWorkThread()
{
	byte  bytLvl = 0;
	char  szRecvBuf[1024] = {0};
	char  szSendBuf[1024] = {0};
	DWORD dwRecvLen = 0;
	int   iSendLen = 0;
	DWORD dwPushTm;
	DWORD dwBufSize = (DWORD)sizeof(szRecvBuf);

	while( !g_bProgExit )
	{
		usleep(50000);

		dwRecvLen = 0;
		memset(szRecvBuf, 0, sizeof(szRecvBuf));

		// 先处理串口接收到的数据
		if( !m_objDiaoduReadMng.PopData(bytLvl, dwBufSize, dwRecvLen, szRecvBuf, dwPushTm))
		{
			//PRTMSG(MSG_DBG, "m_objDiaoduReadMng PopData:");
			//PrintString(szRecvBuf, dwRecvLen);

			if( dwRecvLen <= dwBufSize )
				DealDiaoduFrame(szRecvBuf, (int)dwRecvLen);
		}

		// 若当前不是使用调度屏，则continue
		if (g_iTerminalType != 2)
			continue;

		// 再向串口发送数据
		if( !g_objDiaoduOrHandWorkMng.PopData(bytLvl, dwBufSize-1, dwRecvLen, szRecvBuf+1, dwPushTm))
		{
			// 计算校验和
			szRecvBuf[0] = get_crc((byte*)(szRecvBuf+1), int(dwRecvLen));
			
			iSendLen = 0;
#if USE_COMBUS == 1
			szSendBuf[iSendLen++] = 0xff;	// 使用串口扩展盒时，发往调度屏或手柄的数据要由QianExe里面的CComBus转发
			szSendBuf[iSendLen++] = 0xf3;	// 0xf3表示由扩展盒的物理串口3进行发送
#endif
#if USE_COMBUS == 2
			szSendBuf[iSendLen++] = 0xff;	// 使用150TR时，发往调度屏或手柄的数据要由QianExe里面的CCom150TR转发
			szSendBuf[iSendLen++] = 0xA0;	// 0xA0表示由150TR的扩展串口进行发送
			szSendBuf[iSendLen++] = 0x01;	// 扩展串口1
#endif
			// 对数据进行转义(头尾加0x7e)
			szSendBuf[iSendLen++] = 0x7e;

			for(DWORD i=0; i<dwRecvLen+1; i++) 
			{
				switch(szRecvBuf[i])
				{
				case 0x7e:	
					szSendBuf[iSendLen++]=0x7d;  
					szSendBuf[iSendLen++]=0x02;  
					break;

				case 0x7d:	
					szSendBuf[iSendLen++]=0x7d; 
					szSendBuf[iSendLen++]=0x01; 
					break;

				default:
					szSendBuf[iSendLen++]=szRecvBuf[i];
					break;
				}
			}
			szSendBuf[iSendLen++] = 0x7e;
			
			// 发送数据帧		
			g_objComAdjust.ComWrite(szSendBuf, iSendLen);
		}

		// 测试用，1秒钟打印一次GPS状态信息
// 		static DWORD test = 0;
// 		if(test==0)	test = GetTickCount();
// 		if((GetTickCount()-test)>1000) 
// 		{
// 			test = 0;
// 			tagSatelliteInfo info;
// 			char str[100] = {0};
// 			if(g_objGpsSrc.GetSatellitesInfo(info)) 
// 			{
// 				int cnt = 0;
// 				int i = 0;
// 				for( i=0; i<info.m_bytSatelliteCount; i++) 
// 				{
// 					if(info.m_arySatelliteInfo[i][1]>35)	cnt++;
// 				}
// 				sprintf(str, "NUM:%02d SNR>35:%d ", info.m_bytSatelliteCount, cnt);
// 				for(i=0; i<info.m_bytSatelliteCount; i++) 
// 				{
// 					char tmp[20] = {0};
// 					sprintf(tmp, "%d/%d ", info.m_arySatelliteInfo[i][0], info.m_arySatelliteInfo[i][1]);
// 					if((strlen(str)+strlen(tmp))>=45)	break;
// 					strcat(str, tmp);
// 				}
// 			}
// 
// 			PRTMSG(MSG_DBG, "%s\n", str);
// 		}

		// 再处理调度屏信息更新
		static DWORD t1 = 0;
		if(t1==0)	t1 = GetTickCount();
		if((GetTickCount()-t1)>2500) 
		{ 
			t1 = 0;
			
			if(g_diskfind) 
			{
				show_diaodu(LANG_CHKING_U);
			}
			
			if(g_SysUpdate) 
			{
				show_diaodu(LANG_NK_UPGRADING);
			}

			if (g_AppUpdate)
			{
				show_diaodu(LANG_SFT_UPGRADING);
			}
			
			if (g_ExeUpdate)
			{
				show_diaodu(LANG_SFT_UPGRADING);
			}
			
			if (g_ImageUpdate)
			{
				show_diaodu(LANG_NK_UPGRADING);
			}
			
			if(g_DataOutPut)
			{
				show_diaodu(LANG_DATAOUTPUT);
			}
			
			if(m_gpsdect) 
			{		// 实时显示GPS显示信息
				static DWORD t2 = 0;
				if(m_gpsdect==1)	{ m_gpsdect = 2; t2 = GetTickCount(); }
				if( (GetTickCount()-t2) > 30*1000 ) 
				{
					t2 = 0;
					m_gpsdect = 0;
					show_diaodu(LANG_GPS_CHKMODE_CLOSE);
				} else 
				{
					tagSatelliteInfo info;
					char str[100] = {0};
					if(g_objGpsSrc.GetSatellitesInfo(info)) 
					{
						int cnt = 0;
						int i = 0;
						for( i=0; i<info.m_bytSatelliteCount; i++) 
						{
							if(info.m_arySatelliteInfo[i][1]>35)	cnt++;
						}
						sprintf(str, "NUM:%02d SNR>35:%d ", info.m_bytSatelliteCount, cnt);
						for(i=0; i<info.m_bytSatelliteCount; i++) 
						{
							char tmp[20] = {0};
							sprintf(tmp, "%d/%d ", info.m_arySatelliteInfo[i][0], info.m_arySatelliteInfo[i][1]);
							if((strlen(str)+strlen(tmp))>=45)	break;
							strcat(str, tmp);
						}
						show_diaodu(str);
					} 
					else 
					{
						show_diaodu(LANG_GPS_NOSIGNAL);
					}
				}	
			}

			if(m_iodect) 
			{	//传感器实时检测
				static DWORD t2 = 0;
				if(m_iodect==1)		
				{ 
					m_iodect = 2; 
					t2 = GetTickCount();
// 					//创建传感器检测线程,(在IoSta中检测)
// 					pthread_t h_ThIoDetect;
// 					if( 0 != pthread_create(&h_ThIoDetect, NULL, G_IoDetect, NULL) )
// 					{
// 						PRTMSG(MSG_ERR,"创建RecvPhoneData数据接收线程失败！\n");
// 						show_diaodu(LANG_IO_CHKMODE_CLOSE);
// 					}
				}
				if( (GetTickCount()-t2) > 5*60*1000 ) 
				{
					t2 = 0;
					m_iodect = 0;
					show_diaodu(LANG_IO_CHKMODE_CLOSE);
					
					char by = 0x0a;
					DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);		
				} 
			}
		}
	}
}

//不使用，使用IoSta中的中断信号检测
void CDiaodu::_IoDetectThread()
{
	DWORD dwIoSta = g_objIoSta.GetIoSta();
	DWORD dwResult = 0;
	while(g_objDiaodu.m_iodect > 0)
	{
//		dwIoSta = g_objIoSta.GetIoSta();

		//ACC
		dwResult = g_objIoSta.GetIoSta();
		if( (IOSTA_ACC_ON&dwResult) != (IOSTA_ACC_ON&dwIoSta) )
		{
			if (IOSTA_ACC_ON&dwResult)
			{
				dwIoSta |= IOSTA_ACC_ON;
				show_diaodu(LANG_IOCHK_ACCON);
			}
			else
			{
				dwIoSta &= ~IOSTA_ACC_ON;
				show_diaodu(LANG_IOCHK_ACCOFF);
			}
			usleep(300);
		}
		
		//报警信号
		dwResult = g_objIoSta.GetIoSta();
		if ( (IOSTA_ALARM_ON&dwResult) != (IOSTA_ALARM_ON&dwIoSta) )
		{
			if (IOSTA_ALARM_ON&dwResult)
			{
				dwIoSta |= IOSTA_ALARM_ON;
				show_diaodu(LANG_IOCHK_ALERMFOOT);
			}
			else
			{
				dwIoSta &= ~IOSTA_ALARM_ON;
				show_diaodu(LANG_IOCHK_ALERMFOOT);
			}
			usleep(300);
		}

		//空重车/计价器
		dwResult = g_objIoSta.GetIoSta();
		if( (IOSTA_JIJIA_HEAVY&dwResult) != (IOSTA_JIJIA_HEAVY&dwIoSta) )
		{
			if (IOSTA_JIJIA_HEAVY&dwResult)
			{
				dwIoSta |= IOSTA_JIJIA_HEAVY;
				show_diaodu(LANG_IOCHK_JIJIA_VALID);
			}
			else
			{
				dwIoSta &= ~IOSTA_JIJIA_HEAVY;
				show_diaodu(LANG_IOCHK_JIJIA_INVALID);
			}
			usleep(300);
		}
		
		//前车门信号
		dwResult = g_objIoSta.GetIoSta();
		if( (IOSTA_FDOOR_OPEN&dwResult) != (IOSTA_FDOOR_OPEN&dwIoSta) )
		{
			if (IOSTA_FDOOR_OPEN&dwResult)
			{
				dwIoSta |= IOSTA_FDOOR_OPEN;
				show_diaodu(LANG_IOCHK_DOOROPEN);
			}
			else
			{
				dwIoSta &= ~IOSTA_FDOOR_OPEN;
				show_diaodu(LANG_IOCHK_DOORCLOSE);
			}
			usleep(300);
		}
		
		//后车门信号
		dwResult = g_objIoSta.GetIoSta();
		if( (IOSTA_BDOOR_OPEN&dwResult) != (IOSTA_BDOOR_OPEN&dwIoSta) )
		{
			if (IOSTA_BDOOR_OPEN&dwResult)
			{
				dwIoSta |= IOSTA_BDOOR_OPEN;
				show_diaodu(LANG_IOCHK_BACKDOOROPEN);
			}
			else
			{
				dwIoSta &= ~IOSTA_BDOOR_OPEN;
				show_diaodu(LANG_IOCHK_BACKDOORCLOSE);
			}
			usleep(300);
		}
		
		//脚刹信号
		dwResult = g_objIoSta.GetIoSta();
		if ( (IOSTA_JIAOSHA_VALID&dwResult) != (IOSTA_JIAOSHA_VALID&dwIoSta) )
		{
			if (IOSTA_JIAOSHA_VALID&dwResult)
			{
				dwIoSta |= IOSTA_JIAOSHA_VALID;
				show_diaodu(LANG_IOCHK_FOOTBRAKE);
			}
			else
			{
				dwIoSta &= ~IOSTA_JIAOSHA_VALID;
				show_diaodu(LANG_IOCHK_FOOTBRAKE_CANCEL);
			}
			usleep(300);
		}
		
		//手刹信号
		dwResult = g_objIoSta.GetIoSta();
		if ( (IOSTA_SHOUSHA_VALID&dwResult) != (IOSTA_SHOUSHA_VALID&dwIoSta) )
		{
			if (IOSTA_SHOUSHA_VALID&dwResult)
			{
				dwIoSta |= IOSTA_SHOUSHA_VALID;
				show_diaodu(LANG_IOCHK_HANDBRAKE);
			}
			else
			{
				dwIoSta &= ~IOSTA_SHOUSHA_VALID;
				show_diaodu(LANG_IOCHK_HANDBRAKE_CANCEL);
			}
			usleep(300);
		}

		//载客信号1
		dwResult = g_objIoSta.GetIoSta();
		if( (IOSTA_PASSENGER1_VALID&dwResult) != (IOSTA_PASSENGER1_VALID&dwIoSta) )
		{
			if (IOSTA_PASSENGER1_VALID&dwResult)
			{
				dwIoSta |= IOSTA_PASSENGER1_VALID;
				show_diaodu(LANG_IOCHK_PASSENGER1_VALID);
			}
			else
			{
				dwIoSta &= ~IOSTA_PASSENGER1_VALID;
				show_diaodu(LANG_IOCHK_PASSENGER1_INVALID);
			}
			usleep(300);
		}
		
		//载客信号2
		dwResult = g_objIoSta.GetIoSta();
		if( (IOSTA_PASSENGER2_VALID& dwResult) != (IOSTA_PASSENGER2_VALID& dwIoSta) )
		{
			if (IOSTA_PASSENGER2_VALID& dwResult)
			{
				dwIoSta |= IOSTA_PASSENGER2_VALID;
				show_diaodu(LANG_IOCHK_PASSENGER2_VALID);
			}
			else
			{
				dwIoSta &= ~IOSTA_PASSENGER2_VALID;
				show_diaodu(LANG_IOCHK_PASSENGER2_INVALID);
			}			
			usleep(300);
		}
		
		//载客信号3
		dwResult = g_objIoSta.GetIoSta();
		if( (IOSTA_PASSENGER3_VALID&dwResult) != (IOSTA_PASSENGER3_VALID&dwIoSta) )
		{
			if (IOSTA_PASSENGER3_VALID&dwResult)
			{
				dwIoSta |= IOSTA_PASSENGER3_VALID;
				show_diaodu(LANG_IOCHK_PASSENGER3_VALID);
			}
			else
			{
				dwIoSta &= ~IOSTA_PASSENGER3_VALID;
				show_diaodu(LANG_IOCHK_PASSENGER3_INVALID);
			}
			usleep(300);
		}
	}
}

bool CDiaodu::P_ExeUpdateThread()
{
	char by;
	
	if (true == g_bProgExit)
	{
		return false;
	}
	
	// 避免同时多个线程进行拷贝
	static bool copying = false;
	if(copying)		return 0;
	copying = true;
	
	bool finded = false;
	char szShowBuf[1024] = {0};
	char str[128] = {0};
	char const szUpdateFile[32] = "exe.bin";
	char diskPathUpfile[128] = {0};	//升级路径+升级文件
	int len = 0;
	
	// 先检测U盘
	show_diaodu(LANG_CHKING_U);
	
	g_diskfind = true;
	if( WaitForEvent(UDiskMount, NULL, 15000) )
	{
		g_diskfind = false;
		show_diaodu(LANG_CHKU_FAIL);
		copying = false;
		return false;
	}
	g_diskfind = false;

	//开始升级文件
	show_diaodu(LANG_SFT_UPGRADING);
	
	by = 0x03;
	DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
	
	g_ExeUpdate = true;
	
	sprintf(diskPathUpfile, "/mnt/%s/%s", UDISK_UP_PATH, szUpdateFile);

	// 将升级文件exe.bin复制到part3的下载目录下，以作为升级备份
	if( AppIntact(diskPathUpfile) == false || 0 != system("cp -f /mnt/UDisk/part1/exe.bin /mnt/Flash/part3/Down/exe.bin") )
	{
		show_diaodu("升级失败");
		copying = false;
		return false;
	}
	PRTMSG(MSG_DBG, "cp exe.bin to part3 succ!\n");

#if VEHICLE_TYPE == VEHICLE_M
	if( G_ExeUpdate(diskPathUpfile, 1) == true )
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	if( G_ExeUpdate(diskPathUpfile, 0) == true )
#endif
	{
		strcpy(str, "exe.bin ");
			
		strcpy(szShowBuf+len, str);
		len += strlen(str);
		finded = true;
		PRTMSG(MSG_DBG, "%s update succ\n", szUpdateFile);

// 		// 升级成功后，将恢复出厂配置，但要保留手机号、IP、端口
// 		char szbuf[40] = {0};
// 		sprintf(szbuf, "%s", NEED_RUSEMUCFG);
// 		SetImpCfg(szbuf, offsetof(tagImportantCfg, m_szSpecCode), sizeof(szbuf));
// 		SetSecCfg(szbuf, offsetof(tagSecondCfg, m_szSpecCode), sizeof(szbuf));

		DelErrLog();	// 升级成功后程序从分区5启动

		sleep(1);
	}
		
	sleep(2);
	g_ExeUpdate = false;
	
	by = 0x08;
	DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);

	sprintf(str, "升级%s!", finded ? LANG_SUCC : LANG_FAIL);
	strcpy(szShowBuf+len, str);
	len += strlen(str);
	szShowBuf[len] = 0;
	sleep(1);
	
	// 升级失败，则不重启
	if( !finded )
	{
		show_diaodu(szShowBuf);
		copying = false;
		return false;
	}
	
	// 显示即将重启的提示,直至重启
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu("%s",LANG_SYS_TORESET );
	sleep(2);

	G_ResetSystem();
}

bool CDiaodu::P_ImageUpdateThread()
{
	char by;
	
	if (true == g_bProgExit)
	{
		return false;
	}
	
	// 避免同时多个线程进行拷贝
	static bool copying = false;
	if(copying)		return 0;
	copying = true;
	
	bool finded = false;
	char szShowBuf[1024] = {0};
	char str[128] = {0};
	char const szUpdateFile[32] = "image.bin";
	char diskPathUpfile[128] = {0};	//升级路径+升级文件
	int len = 0;
	
	// 先检测U盘
	show_diaodu(LANG_CHKING_U);
	
	g_diskfind = true;
	if( WaitForEvent(UDiskMount, NULL, 15000) )
	{
		g_diskfind = false;
		show_diaodu(LANG_CHKU_FAIL);
		copying = false;
		return false;
	}
	g_diskfind = false;

	//开始升级文件
	show_diaodu(LANG_NK_UPGRADING);
	
	by = 0x03;
	DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
	
	g_ImageUpdate = true;
	
	sprintf(diskPathUpfile, "/mnt/%s/%s", UDISK_UP_PATH, szUpdateFile);

#if VEHICLE_TYPE == VEHICLE_M
	if( G_ImageUpdate(diskPathUpfile, 1)==true )
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	if( G_ImageUpdate(diskPathUpfile, 0)==true )
#endif
	{
		strcpy(str, "image.bin ");
			
		strcpy(szShowBuf+len, str);
		len += strlen(str);
		finded = true;
		PRTMSG(MSG_DBG, "%s update succ\n", szUpdateFile);

// 		// 升级成功后，将恢复出厂配置，但要保留手机号、IP、端口
// 		char szbuf[40] = {0};
// 		sprintf(szbuf, "%s", NEED_RUSEMUCFG);
// 		SetImpCfg(szbuf, offsetof(tagImportantCfg, m_szSpecCode), sizeof(szbuf));
// 		SetSecCfg(szbuf, offsetof(tagSecondCfg, m_szSpecCode), sizeof(szbuf));

		DelErrLog();	// 升级成功后程序从分区5启动
		DelVerFile();	// 升级成功后修改系统版本号
		
		sleep(1);
	}
		
	sleep(2);
	g_ImageUpdate = false;
	
	by = 0x08;
	DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);

	sprintf(str, "升级%s!", finded ? LANG_SUCC : LANG_FAIL);
	strcpy(szShowBuf+len, str);
	len += strlen(str);
	szShowBuf[len] = 0;
	sleep(1);
	
	// 升级失败，则不重启
	if( !finded )
	{
		show_diaodu(szShowBuf);
		copying = false;
		return false;
	}
	
	// 显示即将重启的提示,直至重启
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu("%s",LANG_SYS_TORESET );
	sleep(2);

	G_ResetSystem();
}

bool CDiaodu::P_AppUpdateThread()
{
	char by;
	
	if (true == g_bProgExit)
	{
		return false;
	}
	
	// 避免同时多个线程进行拷贝
	static bool copying = false;
	if(copying)		return 0;
	copying = true;
	
	char *pBegin = NULL;
	char *pEnd = NULL;
	bool finded = false;
	char szShowBuf[1024] = {0};
	char str[128] = {0};
	char const szUpdateFile[][32] = {"QianExe", "ComuExe", "SockServExe", "IOExe", "UpdateExe", "DvrExe", "libComuServ.so"};
	char diskPathUpfile[128] = {0};	//升级路径+升级文件
	char flashPathUpfile[128] = {0};
	int len = 0;
	int i;
	
	// 先检测U盘
	show_diaodu(LANG_CHKING_U);
	
	g_diskfind = true;
	if( WaitForEvent(UDiskMount, NULL, 15000) )
	{
		g_diskfind = false;
		show_diaodu(LANG_CHKU_FAIL);
		copying = false;
		return false;
	}
	g_diskfind = false;

	//开始升级文件
	show_diaodu(LANG_SFT_UPGRADING);
	
	by = 0x03;
	DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
	
	g_AppUpdate = true;
	for(i=0; i<(sizeof(szUpdateFile)/sizeof(szUpdateFile[0])); i++)
	{
		sprintf(diskPathUpfile, "/mnt/%s/%s", UDISK_UP_PATH, szUpdateFile[i]);
		sprintf(flashPathUpfile, "/mnt/Flash/part5/%s", szUpdateFile[i]);
		
#if VEHICLE_TYPE == VEHICLE_M
		if( G_AppUpdate(diskPathUpfile, flashPathUpfile, 1)==0 )
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		if( G_AppUpdate(diskPathUpfile, flashPathUpfile, 0)==0 )
#endif
		{
// 		升级成功后，将恢复出厂配置，但要保留手机号、IP、端口
//		char szbuf[40] = {0};
//		sprintf(szbuf, "%s", NEED_RUSEMUCFG);
//		SetImpCfg(szbuf, offsetof(tagImportantCfg, m_szSpecCode), sizeof(szbuf));
//		SetSecCfg(szbuf, offsetof(tagSecondCfg, m_szSpecCode), sizeof(szbuf));

			DelErrLog();	// 升级成功后程序从分区5启动

			switch(i)
			{
				case 0:
					strcpy(str, "Qian ");
					break;
				case 1:
					strcpy(str, "Comu ");
					break;
				case 2:
					strcpy(str, "Sock ");
					break;
				case 3:
					strcpy(str, "IO ");
					break;
				case 4:
					strcpy(str, "Upd ");
					break;
				case 5:
					strcpy(str, "Dvr ");
					break;
				case 6:
					strcpy(str, "ComuServ ");
					break;
				default:
					break;
			}
			
			strcpy(szShowBuf+len, str);
			len += strlen(str);
			finded = true;
			PRTMSG(MSG_DBG, "%s update succ\n",szUpdateFile[i]);
			sleep(1);
		}
	}
	sleep(2);
	g_AppUpdate = false;
	
	by = 0x08;
	DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);

	sprintf(str, "升级%s!", finded ? LANG_SUCC : LANG_FAIL);
	strcpy(szShowBuf+len, str);
	len += strlen(str);
	szShowBuf[len] = 0;
	sleep(1);
	
	// 升级失败，则不重启
	if( !finded )
	{
		show_diaodu(szShowBuf);
		copying = false;
		return false;
	}
	
	// 显示即将重启的提示,直至重启
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu("%s",LANG_SYS_TORESET );
	sleep(2);

	G_ResetSystem();
}

bool CDiaodu::P_SysUpdateThread()
{
	char by;
	
	if (true == g_bProgExit)
	{
		return false;
	}
	
	bool finded = false;
	char szShowBuf[1024] = {0};
	char szCommand[128] = {0};
	char str[128] = {0};
	char szMtdPath[][32] = {BOOT_PART, KERN_PART, ROOT_PART, UPD_PART, APP_PART};
	char const szUpdateFile[][32] = {"boot.bin", "kernel.bin", "root.bin", "update.bin", "app.bin"};
	char diskPathUpfile[128] = {0};	//升级路径+升级文件
	int len = 0;
	int i;
	
	// 先检测U盘
	show_diaodu(LANG_CHKING_U);
	
	g_diskfind = true;
	if( WaitForEvent(UDiskMount, NULL, 15000) )
	{
		g_diskfind = false;
		show_diaodu(LANG_CHKU_FAIL);
		return false;
	}
	g_diskfind = false;

	//开始升级文件
	show_diaodu(LANG_NK_UPGRADING);
	
	by = 0x03;
	DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
	
	g_SysUpdate = true;
	for(i=0; i<(sizeof(szUpdateFile)/sizeof(szUpdateFile[0])); i++)
	{
		sprintf(diskPathUpfile, "/mnt/%s/%s", UDISK_UP_PATH, szUpdateFile[i]);

#if VEHICLE_TYPE == VEHICLE_M
		if( G_SysUpdate(diskPathUpfile, szMtdPath[i], 1)==0 )
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		if( G_SysUpdate(diskPathUpfile, szMtdPath[i], 0)==0 )
#endif
		{
			if(i == 0 || i == 1 || i == 2)
				DelVerFile();	// 升级成功后修改系统版本号
			else if(i == 3 || i == 4)
				DelErrLog();	// 升级成功后程序从分区5启动
					
			switch(i)
			{
				case 0:
					strcpy(str, "boot.bin ");
					break;
				case 1:
					strcpy(str, "kernel.bin ");
					break;
				case 2:
					strcpy(str, "root.bin ");
					break;
				case 3:
					strcpy(str, "update.bin ");
					break;
				case 4:
					strcpy(str, "app.bin ");
					break;
				default:
					break;
			}
			
			strcpy(szShowBuf+len, str);
			len += strlen(str);		
			finded = true;
			PRTMSG(MSG_DBG, "%s update succ\n", szUpdateFile[i]); 
			sleep(1);
		}
	}
	sleep(2);
	g_SysUpdate = false;
	
	by = 0x08;
	DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);

	sprintf(str, "升级%s!", finded ? LANG_SUCC : LANG_FAIL);
	strcpy(szShowBuf+len, str);
	len += strlen(str);
	szShowBuf[len] = 0;
	sleep(1);
	
	// 升级失败，则不重启
	if( !finded )
	{
		show_diaodu(szShowBuf);
		return false;
	}
	
	// 显示即将重启的提示,直至重启
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu(szShowBuf);
	sleep(3);
	show_diaodu("%s",LANG_SYS_TORESET );
	sleep(2);

	G_ResetSystem();
}

bool CDiaodu::P_DataOutPutThread()
{
	if (true == g_bProgExit)
	{
		return false;
	}
	
	// 避免同时多个线程进行拷贝
	static bool copying = false;
	if(copying)		return 0;
	copying = true;
	
	// 先检测U盘
	show_diaodu(LANG_CHKING_U);
	
	bool bdiskfind = true;
	int  i = 0;
	char szDesFilePath[255] = {0};
	char szDiskPathCheck[100] = {0};
	
	sprintf(szDiskPathCheck, "/mnt/%s/flash", UDISK_UP_PATH);
	for (i=0; i<15; i++) 
	{
		if( 0 == access(szDiskPathCheck, F_OK) )
		{
			sleep(1);
			continue;
		}
		sleep(1);
		break;
	}
	
	g_diskfind = false;
	
	if(i>=15) 
	{
		show_diaodu(LANG_CHKU_FAIL);
		copying = false;
		return false;
	}
	
	show_diaodu(LANG_DATAOUTPUT);
	g_DataOutPut = true;
		
	// 整个part4都拷贝
	if( 0 == access(FLASH_PART4_PATH, F_OK) )
	{ 		
		memset(szDesFilePath, 0, sizeof(szDesFilePath));
		sprintf(szDesFilePath, "/mnt/%s/data/", UDISK_UP_PATH);
		PRTMSG(MSG_DBG, "szDesFilePaht: %s\n", szDesFilePath);
				
		if( 0 != DirCopy(FLASH_PART4_PATH, szDesFilePath) )
		{
			PRTMSG(MSG_DBG, "copy %s succ!\n", FLASH_PART4_PATH);
		}
		else
		{
			PRTMSG(MSG_DBG, "copy %s failed!\n", FLASH_PART4_PATH);
		}
	}
	
	copying = false;
	g_DataOutPut = false;
	show_diaodu("数据导出完成");
}

bool CDiaodu::P_VideoOutPutThread()
{
	if (true == g_bProgExit)
	{
		return false;
	}
	
	// 避免同时多个线程进行拷贝
	static bool copying = false;
	if(copying)		return 0;
	copying = true;
	
	// 先检测U盘
	show_diaodu(LANG_CHKING_U);
	
	bool bdiskfind = true;
	int  i = 0;
	char szDesFilePath[255] = {0};
	char szDiskPathCheck[100] = {0};
	
	sprintf(szDiskPathCheck, "/mnt/%s/flash", UDISK_UP_PATH);
	for (i=0; i<15; i++) 
	{
		if( 0 == access(szDiskPathCheck, F_OK) )
		{
			sleep(1);
			continue;
		}
		sleep(1);
		break;
	}
	
	g_diskfind = false;
	
	if(i>=15) 
	{
		show_diaodu(LANG_CHKU_FAIL);
		copying = false;
		return false;
	}
	
	show_diaodu(LANG_DATAOUTPUT);
	g_DataOutPut = true;
	
	// 照片文件
	if( 0 == access(PHOTO_SAVE_PATH, F_OK) )
	{		
		memset(szDesFilePath, 0, sizeof(szDesFilePath));
		sprintf(szDesFilePath, "/mnt/%s/data/Photo", UDISK_UP_PATH);
		PRTMSG(MSG_DBG, "szDesFilePaht: %s\n", szDesFilePath);
		
		if( 0 != DirCopy(PHOTO_SAVE_PATH, szDesFilePath) )
		{
			PRTMSG(MSG_DBG, "copy %s succ!\n", PHOTO_SAVE_PATH);
		}
		else
		{
			PRTMSG(MSG_DBG, "copy %s failed!\n", PHOTO_SAVE_PATH);
		}
	}
	
	// 视频文件
	if( 0 == access(VIDEO_SAVE_PATH, F_OK) )
	{ 		
		memset(szDesFilePath, 0, sizeof(szDesFilePath));
		sprintf(szDesFilePath, "/mnt/%s/data/Video/", UDISK_UP_PATH);
		PRTMSG(MSG_DBG, "szDesFilePaht: %s\n", szDesFilePath);
		
		if( 0 != DirCopy(VIDEO_SAVE_PATH, szDesFilePath) )
		{
			PRTMSG(MSG_DBG, "copy %s succ!\n", VIDEO_SAVE_PATH);
		}
		else
		{
			PRTMSG(MSG_DBG, "copy %s failed!\n", VIDEO_SAVE_PATH);
		}
	}
	
	copying = false;
	g_DataOutPut = false;
	show_diaodu("数据导出完成");
}

//-----------------------------------------------------------------------------------------------------------------
// 打开调度屏的串口
// 返回: 打开成功或者失败
// bool CDiaodu::ComOpen()
// {
// 	int iResult;
// 	int i;
// 	struct termios options;
// 	const int COMOPEN_TRYMAXTIMES = 5;
// 
// 	for( i = 0; i < COMOPEN_TRYMAXTIMES; i ++ )
// 	{
// //		m_iComPort = open("/dev/ttyAMA0", O_RDWR );//| O_NONBLOCK);
// 		m_iComPort = open("/dev/ttySIM0", O_RDWR );// | O_NONBLOCK);
// 		if( -1 != m_iComPort )
// 		{
// 			PRTMSG(MSG_ERR,"Open diaodu com succ!\n");
// 			break;
// 		}
// 		
// 		PRTMSG(MSG_ERR,"Open diaodu com fail!\n");
// 		sleep(1);
// 	}
// 	
// 	if(tcgetattr(m_iComPort, &options) != 0)
// 	{
// 		perror("GetSerialAttr");
// 		return false;
// 	}
// 	
// 	options.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
// 	options.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
// 	options.c_oflag &= ~OPOST;
// 	
// 	cfsetispeed(&options,B9600);	//设置波特率，调度屏设置波特率为9600
// 	cfsetospeed(&options,B9600);
// 	
// 	if (tcsetattr(m_iComPort,TCSANOW,&options) != 0)   
// 	{ 
// 		perror("Set com Attr"); 
// 		return false;
// 	}
// 	
// 	PRTMSG(MSG_NOR,"Diaodu com open Succ!\n"); 
// 	return true;
// }
// 
// 
// //-----------------------------------------------------------------------------------------------------------------
// // 关闭串口
// void CDiaodu::ComClose()
// {
// 	if(-1 != m_iComPort)	
// 	{
// 		close(m_iComPort);
// 		m_iComPort = -1;
// 	}
// }
// 
// //-----------------------------------------------------------------------------------------------------------------
// // 写串口函数
// // data: 待写数据的指针
// // len:  待写数据的长度
// // 返回: 实际写的字节数 
// int CDiaodu::ComWrite(char *v_szBuf, int v_iLen)
// {
// #if USE_COMBUS == 1
// 	DataPush(v_szBuf, (DWORD)v_iLen, DEV_DIAODU, DEV_QIAN, LV2);
// 	return v_iLen;
// #endif
// 
// 	int iWrited = 0;	// 串口发出的数据长度	
// 	iWrited = write(m_iComPort, v_szBuf, v_iLen);
// 	return iWrited;
// }
// 
// void CDiaodu::AnalyseComFrame(char *v_szBuf, int v_iLen)
// {
// 	int iRet = 0;
// 	if( !v_szBuf || v_iLen <= 0 ) 
// 		return ;
// 	
// 	static int i7ECount = 0;
// 	
// 	for( int i = 0; i < v_iLen; i ++ )
// 	{
// 		if( 0x7e == v_szBuf[ i ] )
// 		{
// 			++ i7ECount;
// 			
// 			if( 0 == i7ECount % 2 ) // 若得到一帧
// 			{
// 				i7ECount = 0;
// 
// 				// 转义
// 				m_iComFrameLen = DetranData(m_szComFrameBuf, m_iComFrameLen);
// 
// 				// 计算校验和，校验正确，送入接收队列
// 				if( check_crc((byte*)m_szComFrameBuf, m_iComFrameLen) )
// 				{	
// 					DWORD dwPacketNum = 0;
// 
// 					iRet = m_objDiaoduReadMng.PushData(LV1, (DWORD)(m_iComFrameLen-1), m_szComFrameBuf+1, dwPacketNum);
// 					if( 0 != iRet )
// 					{
// 						PRTMSG(MSG_ERR, "Push one frame failed:%d\n",iRet);
// 						memset(m_szComFrameBuf, 0, m_iComFrameLen);
// 						m_iComFrameLen = 0;
// 					}					
// 				}
// 				else
// 				{
// 					PRTMSG(MSG_ERR, "Diaodu frame check crc err!\n");
// 					memset(m_szComFrameBuf, 0, m_iComFrameLen);
// 					m_iComFrameLen = 0;
// 				}
// 
// 				//无论校验正确与否，都将缓冲区清0
// 				memset(m_szComFrameBuf, 0, m_iComFrameLen);
// 				m_iComFrameLen = 0;
// 			}
// 		}
// 		else
// 		{
// 			m_szComFrameBuf[m_iComFrameLen++] = v_szBuf[i];
// 		}
// 	}
// }

int CDiaodu::DetranData(char *v_szBuf, int v_iLen)
{
	char szbuf[1024];
	int  iNewLen = 0;
	
	for( int i = 0; i < v_iLen; )
	{
		if( 0x7d != v_szBuf[ i ] || i == v_iLen - 1 )
		{
			szbuf[ iNewLen ++ ] = v_szBuf[ i++ ];
		}
		else
		{
			if( 1 == v_szBuf[ i + 1 ] )
			{
				szbuf[ iNewLen ++ ] = 0x7d;
			}
			else if( 2 == v_szBuf[ i + 1 ] )
			{
				szbuf[ iNewLen ++ ] = 0x7e;
			}
			else
			{
				szbuf[ iNewLen ++ ] = v_szBuf[ i ];
				szbuf[ iNewLen ++ ] = v_szBuf[ i + 1 ];
			}
			i += 2;
		}
	}
	
	memcpy( v_szBuf, szbuf, iNewLen );

	return iNewLen;
}

void CDiaodu::DealDiaoduFrame(char *v_szBuf, int v_iLen)
{
	DWORD dwPktNum;
	
	switch (v_szBuf[0])
	{
		case 0x01:	//0x01是什么？
			break;
	
			// 一般业务 -----------------------------------------------
		case 0x03:	//请求当前GPS位置数据
			{
				GPSDATA gps(0);
				GetCurGps( &gps, sizeof(gps), GPS_LSTVALID);
				
				Frm04 frm;
				frm.gps.valid = (gps.valid=='V') ? (0x60) : (0x40);
	
				frm.gps.lat[0] = (byte)(DWORD)gps.latitude;
				frm.gps.lat[1] = (byte)(((DWORD)(gps.latitude*100))%100);
				frm.gps.lat[2] = (byte)(((DWORD)(gps.latitude*10000))%100);
				frm.gps.lat[3] = (byte)(((DWORD)(gps.latitude*1000000))%100);
				frm.gps.lon[0] = (byte)(DWORD)gps.longitude;
				frm.gps.lon[1] = (byte)(((DWORD)(gps.longitude*100))%100);
				frm.gps.lon[2] = (byte)(((DWORD)(gps.longitude*10000))%100);
				frm.gps.lon[3] = (byte)(((DWORD)(gps.longitude*1000000))%100);
	
				frm.gps.speed = (byte)(gps.speed/1.852);	//转换为海里/时
				frm.gps.dir = (byte)(int)gps.direction;
	
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			break;
			
		case 0x05:	//请求本机手机号
			{
				tag1PComuCfg cfg;
				GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
				
				char frm[100] = {0};
				frm[0] = 0x06;
				memcpy(frm+1, cfg.m_szTel, min(15,strlen(cfg.m_szTel)) );
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, strlen(frm), (char*)&frm, dwPktNum);
			}
			break;
			
		case 0x07:	//请求当前时间
			{
				struct tm pCurrentTime;
				G_GetTime(&pCurrentTime);

				Frm08 frm;
				frm.year = (byte)(pCurrentTime.tm_year +1900 - 2000);
				frm.month = (byte)(pCurrentTime.tm_mon+1);//月份0~~11
				frm.day = (byte)pCurrentTime.tm_mday;
				frm.hour = (byte)pCurrentTime.tm_hour;
				frm.minute = (byte)pCurrentTime.tm_min;
				frm.second = (byte)pCurrentTime.tm_sec;
	
				//PRTMSG(MSG_DBG, "frm.year = %d, pCurrentTime->tm_year = %d\n", frm.year,pCurrentTime->tm_year);			
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			break;
			
		case 0x09:	//请求当前空重车状态
			{	
				Frm0A frm;
				unsigned char uszResult;
#if VEHICLE_TYPE == VEHICLE_M || VEHICLE_TYPE == VEHICLE_V8
				IOGet((byte)IOS_JIJIA, &uszResult);
				frm.full = (IO_JIJIA_HEAVY == uszResult) ? 0x01 : 0x00;
#endif
#if VEHICLE_TYPE == VEHICLE_M2
				IOGet((byte)IOS_BDOOR, &uszResult);
				frm.full = (IO_BDOOR_OPEN == uszResult) ? 0x01 : 0x00;
#endif
	
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			break;
			
		case 0x0B:	//请求查询登录中心状态
			DataPush( v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, LV2);
			break;
			
			
			// 电话业务 -----------------------------------------------
		case 0x10:	//发送DTMF请求
		case 0x12:	//拨号请求		
		case 0x14:	//摘机请求	
		case 0x16:	//挂机请求
		case 0x18:	//振铃指示应答
		case 0x20:	//接通指示应答
		case 0x22:	//空闲指示应答
		case 0x24:	//信号强度请求
		case 0x26:	//免提耳机切换请求
		case 0x28:	//音量调节请求
			g_objPhoneRecvDataMng.PushData((BYTE)LV1, v_iLen, v_szBuf, dwPktNum);
			break;
			
			// 短消息业务 -----------------------------------------------
		case 0x30:	//请求发送短消息
			//show_msg( "发送短消息%x:%60s", pdata[1], pdata+2 );
		case 0x32:	//接收短消息指示应答
			g_objPhoneRecvDataMng.PushData((BYTE)LV1, v_iLen, v_szBuf, dwPktNum);
			break;
			
			// 报警测试 -------------------------------------------------
		case 0x41:	//请求报警测试
		case 0x44:	//报警测试结果发送请求应答
			DataPush( v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
			break;
			
			
			// 防盗报警器学习 -------------------------------------------
		case 0x50:	//防盗报警器配置请求
		case 0x52:	//防盗报警器学习请求
		case 0x54:	//停止防盗报警器学习请求
		case 0x56:	//报警器学习结果发送请求应答
		case 0x58:	//报警器测试请求
		case 0x5A:	//停止报警器测试请求
		case 0x5C:	//报警器测试结果请求发送应答
			DataPush( v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
			break;
	
			
			// 行车记录仪 ------------------------------------------------
		case 0x61:	//请求速度
		case 0x63:	//请求司机登录应答
		case 0x65:	//请求司机编号
		case 0x67:	//请求载重传感器数据
		case 0x69:	//请求传感器状态
		case 0x6B:	//请求文本信息显示应答
		case 0x6C:	//司机登出指示应答
		case 0x6D:	//车辆特征系数指令
		case 0x6F:	//总里程相关指令
			DataPush( v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
			break;
			
			
			// 调度通道	--------------------------------------------------
		case 0x70:	//请求发送调度数据
		case 0x71:	//发送调度指示应答
			DataPush( v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
			break;
			
	/*	case 0x73:	//调度信息TTS语音播报
			PRTMSG(MSG_DBG,"Diaodu: Rcv 0x73!\n");
			break;*/
			
			// 电召业务	--------------------------------------------------
		case 0x7A:	//抢答信息
		case 0x7B:	//电召结果报告
			DataPush( v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
			break;
			
			
			// 紧急求助	--------------------------------------------------
		case 0x80:	//请求发送紧急求助
		case 0x82:	//发送中心紧急求助响应应答
		case 0x84:	//请求发送取消紧急求助
		case 0x86:	//发送中心取消紧急求助响应应答
			{
				DataPush( v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
				break;
			}
		
#if USE_AUTORPTSTATION == 1
		case 0x8e:
			{
				DataPush( v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
			}
			break;
#endif
		
		// GPRS参数配置	--------------------------------------------------
		case 0xA1:	//请求GPRS参数
			{
				char frm[1024] = {0};
				byte len = 0;
				int p = 0;
				char str[100] = {0};
				
				tag1PComuCfg cfg;
				GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
				
				tag1QIpCfg cfg2[2];
				GetImpCfg( &cfg2, sizeof(cfg2), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg2) );
				
				tag1LComuCfg cfg3;
				GetImpCfg( &cfg3, sizeof(cfg3), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg3) );
				
				frm[p++] = (char)0xA0;	//数据类型
				frm[p++] = (char)0x01;	//应答类型
				
				// 手机号码
				frm[p++] = min( 15, strlen(cfg.m_szTel) );		
				memcpy(frm+p, cfg.m_szTel, frm[p-1]);
				p+=frm[p-1];
				
				// APN
				frm[p++] = min( 19, strlen(cfg2[0].m_szApn) );		
				memcpy(frm+p, cfg2[0].m_szApn, frm[p-1]);
				p+=frm[p-1];
				
				// 千里眼 TCP IP 1
				frm[p++] = strlen(inet_ntoa(*(in_addr*)&cfg2[0].m_ulQianTcpIP));
				memcpy(frm+p, inet_ntoa(*(in_addr*)&cfg2[0].m_ulQianTcpIP), frm[p-1]);
				p+=frm[p-1];
				
				// 千里眼 TCP PORT 1
				sprintf(str, "%d", ntohs( cfg2[0].m_usQianTcpPort) );
				frm[p++] = strlen(str);
				memcpy(frm+p, str, frm[p-1]);
				p+=frm[p-1];
				
				// 千里眼 TCP IP 2
				frm[p++] = strlen(inet_ntoa(*(in_addr*)&cfg2[1].m_ulQianTcpIP));
				memcpy(frm+p, inet_ntoa(*(in_addr*)&cfg2[1].m_ulQianTcpIP), frm[p-1]);
				p+=frm[p-1];
				
				// 千里眼 TCP PORT 2
				sprintf(str, "%d", ntohs( cfg2[1].m_usQianTcpPort) );
				frm[p++] = strlen(str);
				memcpy(frm+p, str, frm[p-1]);
				p+=frm[p-1];
				
				// 千里眼 UDP IP 1
				frm[p++] = strlen(inet_ntoa(*(in_addr*)&cfg2[0].m_ulQianUdpIP));
				memcpy(frm+p, inet_ntoa(*(in_addr*)&cfg2[0].m_ulQianUdpIP), frm[p-1]);
				p+=frm[p-1];
				
				// 千里眼 UDP PORT 1
				sprintf(str, "%d", ntohs( cfg2[0].m_usQianUdpPort) );
				frm[p++] = strlen(str);
				memcpy(frm+p, str, frm[p-1]);
				p+=frm[p-1];
				
				// 千里眼 UDP IP 2
				//frm[p++] = strlen(inet_ntoa(*(in_addr*)&cfg2[1].m_ulQianUdpIP));
				//memcpy(frm+p, inet_ntoa(*(in_addr*)&cfg2[1].m_ulQianUdpIP), frm[p-1]);
				frm[p++] = strlen(inet_ntoa(*(in_addr*)&cfg2[0].m_ulVUdpIP)); // 暂时作为视频数据上传UDP连接使用
				memcpy(frm+p, inet_ntoa(*(in_addr*)&cfg2[0].m_ulVUdpIP), frm[p-1]);
				p+=frm[p-1];
				
				// 千里眼 UDP PORT 2
				//sprintf(str, "%d", ntohs( cfg2[1].m_usQianUdpPort) );
				sprintf(str, "%d", ntohs( cfg2[0].m_usVUdpPort) ); // 暂时作为视频数据上传UDP连接使用
				frm[p++] = strlen(str);
				memcpy(frm+p, str, frm[p-1]);
				p+=frm[p-1];
				
				// 流媒体 UDP IP 1
				frm[p++] = strlen(inet_ntoa(*(in_addr*)&cfg3.m_ulLiuIP));
				memcpy(frm+p, inet_ntoa(*(in_addr*)&cfg3.m_ulLiuIP), frm[p-1]);
				p+=frm[p-1];
				
				// 流媒体 UDP PORT 1
				sprintf(str, "%d", ntohs(cfg3.m_usLiuPort) );
				frm[p++] = strlen(str);
				memcpy(frm+p, str, frm[p-1]);
				p+=frm[p-1];
				
				// 流媒体 UDP IP 2
				frm[p++] = strlen(inet_ntoa(*(in_addr*)&cfg3.m_ulLiuIP2));
				memcpy(frm+p, inet_ntoa(*(in_addr*)&cfg3.m_ulLiuIP2), frm[p-1]);
				p+=frm[p-1];
				
				// 流媒体 UDP PORT 2
				sprintf(str, "%d", ntohs(cfg3.m_usLiuPort2) );
				frm[p++] = strlen(str);
				memcpy(frm+p, str, frm[p-1]);
				p+=frm[p-1];
				
				// 千里眼密码
				frm[p++] = strlen(ALL_PASS);
				memcpy(frm+p, ALL_PASS, frm[p-1]);
				p+=frm[p-1];
				
				// 流媒体密码
				frm[p++] = strlen(LIU_PASS);
				memcpy(frm+p, LIU_PASS, frm[p-1]);
				p+=frm[p-1];
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, p, frm, dwPktNum);			
			}
			break;
			
		case 0xA3:	//设置手机号码请求
			{
				char tel[100] = {0};
				memcpy(tel, v_szBuf+2, v_szBuf[1]);
				
				FrmA2 frm;
				
				tag1PComuCfg cfg;
				GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
				memset(cfg.m_szTel, 0, sizeof(cfg.m_szTel) );
				memcpy(cfg.m_szTel, tel, min(15,strlen(tel)) );
				
				frm.reply = 0x02;
				if(strlen(tel)!=0) 
				{ 
					if(!SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) ))	
						frm.reply = 0x01; 
				}
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
				
				Frm35 frm1;		//通知QianExe手机号码改变了
				frm1.reply = 0x01;
				DataPush( (char*)&frm1, sizeof(frm1), DEV_DIAODU, DEV_SOCK, 2);

				char buf[2] = {0x01, 0x03};		//通知UpdateExe手机号码改变了
 				DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);
 				
				tag2DDvrCfg objMvrCfg;
		    GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
				memset(objMvrCfg.IpPortPara.m_szTelNum, 0, sizeof(objMvrCfg.IpPortPara.m_szTelNum) );
				memcpy(objMvrCfg.IpPortPara.m_szTelNum, tel, min(15,strlen(tel)) );
				SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
				char by = 0x0c;//通知DvrExe手机号码改变了
				DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
			}
			break;
			
		case 0xA5:	//设置APN请求
			{
				char apn[100] = {0};
				memcpy(apn, v_szBuf+2, v_szBuf[1]);
				
				FrmA4 frm;
				
				tag1QIpCfg cfg[2];
				GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );
				memset(cfg[0].m_szApn, 0, sizeof(cfg[0].m_szApn) );
				memcpy(cfg[0].m_szApn, apn, min(19,strlen(apn)) );
				
				frm.reply = 0x02;
				if(strlen(apn)!=0) 
				{ 
					if(!SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) ))	
						frm.reply = 0x01; 
				}
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
				
				tag2DDvrCfg objMvrCfg;
		    GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
				memset(objMvrCfg.IpPortPara.m_szApn, 0, sizeof(objMvrCfg.IpPortPara.m_szApn) );
				memcpy(objMvrCfg.IpPortPara.m_szApn, apn, min(19,strlen(apn)) );
				SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
				char by = 0x0c;//通知DvrExe APN改变了
				DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);		
			}
			break;
			
		case 0xA7:	//设置IP地址请求
			{
				tag1QIpCfg cfg[2];
				GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );
				
				tag1LComuCfg cfg2;
				GetImpCfg( &cfg2, sizeof(cfg2), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) );
				
				char str[100] = {0};
				memcpy(str, v_szBuf+3, v_szBuf[2]);
				
				FrmA6 frm;
				frm.flag = v_szBuf[1];
				
				if(frm.flag == 0)
				{
					cfg[0].m_ulQianTcpIP = inet_addr(str);
					frm.reply = 0x02;
					if(inet_addr(str)!=0)
					{ 
						if( !SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) ) )	
						{
							frm.reply = 0x01; 
							
							Frm35 frm1;		//通知QianExe IP改变了
							frm1.reply = 0x01;
							DataPush((char*)&frm1, sizeof(frm1), DEV_DIAODU, DEV_SOCK, 2);
							
							tag2DDvrCfg objMvrCfg;
					    GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
							objMvrCfg.IpPortPara.m_ulCommIP = inet_addr(str);
							SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
							char by = 0x0c;//通知DvrExe IP改变了
							DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);		
						}
					}
				} 
				else if(frm.flag == 1)
				{
					cfg[1].m_ulQianTcpIP = inet_addr(str);
					frm.reply = 0x02;
					
					if(inet_addr(str)!=0)
					{ 
						if( !SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) ) )	
							frm.reply = 0x01; 
					}
				}
				else if(frm.flag == 2)
				{
					cfg[0].m_ulQianUdpIP = inet_addr(str);
					frm.reply = 0x02;
					
					if(inet_addr(str)!=0)
					{ 
						if( !SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) ) )	
						{
							frm.reply = 0x01; 

							Frm35 frm1;		//UDP IP改变了
							frm1.reply = 0x01;
							DataPush((char*)&frm1, sizeof(frm1), DEV_DIAODU, DEV_SOCK, 2);
							
							tag2DDvrCfg objMvrCfg;
					    GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
							objMvrCfg.IpPortPara.m_ulPhotoIP = inet_addr(str);
							SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
							char by = 0x0c;//通知DvrExe IP改变了
							DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);		
						}
					}					
				} 
				else if(frm.flag  == 3) // 调度屏上的千里眼 UDP IP 2
				{
					cfg[1].m_ulQianUdpIP = inet_addr(str);
					frm.reply = 0x02;

					// 暂时使用此项修改King中心的视频连接, 以后最好调度屏上要增加此项设置菜单
					cfg[0].m_ulVUdpIP = inet_addr(str);
					
					if(inet_addr(str)!=0) 
					{ 
						if( !SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) ) )	
						{
							frm.reply = 0x01; 

							Frm35 frm1;		//UDP IP改变了
							frm1.reply = 0x01;
							DataPush((char*)&frm1, sizeof(frm1), DEV_DIAODU, DEV_SOCK, 2);
							
							tag2DDvrCfg objMvrCfg;
					    GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
							objMvrCfg.IpPortPara.m_ulVideoIP = inet_addr(str);
							SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
							char by = 0x0c;//通知DvrExe IP改变了
							DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);		
						}
					}
				} 
				else if(frm.flag == 4) // 流媒体升级中心IP
				{
					cfg2.m_ulLiuIP = inet_addr(str);
					frm.reply = 0x02;
					if(inet_addr(str)!=0)
					{ 
						if( !SetImpCfg(&cfg2, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) ) )	
						{
							frm.reply = 0x01; 

							char buf[2] = {0x01, 0x03};		//通知UpdateExe IP改变了
 							DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);	
 							
 							tag2DDvrCfg objMvrCfg;
					    GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
							objMvrCfg.IpPortPara.m_ulUpdateIP = inet_addr(str);
							SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
 							char by = 0x0c;//通知DvrExe IP改变了
							DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
						}
					}		
				} 
				else if(frm.flag == 5) // 管理平台
				{
					cfg2.m_ulLiuIP2 = inet_addr(str);
					frm.reply = 0x02;
					
					if(inet_addr(str)!=0)
					{ 
						if( !SetImpCfg(&cfg2, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) ) )	
						{
							frm.reply = 0x01; 

							//通知QianExe IP改变了 (待)
						}
					}
				}

				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);	
			}
			break;
			
		case 0xA9:	//设置端口号
			{	
				tag1QIpCfg cfg[2];
				GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );
				
				tag1LComuCfg cfg2;
				GetImpCfg( &cfg2, sizeof(cfg2), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) );
				
				char str[100] = {0};
				memcpy(str, v_szBuf+3, v_szBuf[2]);
				
				FrmA8 frm;
				frm.flag = v_szBuf[1];
				
				if(frm.flag == 0)
				{
					cfg[0].m_usQianTcpPort = htons( (ushort)atol(str) );
					frm.reply = 0x02;
					
					if(atol(str)!=0)
					{ 
						if( !SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) ) )	
						{
							frm.reply = 0x01; 

							Frm35 frm1;		//通知千里眼端口改变了
							frm1.reply = 0x01;
							DataPush((char*)&frm1, sizeof(frm1), DEV_DIAODU, DEV_SOCK, 2);
							
							tag2DDvrCfg objMvrCfg;
					    GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
							objMvrCfg.IpPortPara.m_usCommPort = htons((ushort)atol(str));
							SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
							char by = 0x0c;//通知DvrExe 端口改变了
							DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
						}
					}					
				} 
				else if(frm.flag == 1) 
				{
					cfg[1].m_usQianTcpPort = htons( (ushort)atol(str) );
					frm.reply = 0x02;
					
					if(atol(str)!=0) 
					{ 
						if( !SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) ) )	
							frm.reply = 0x01; 
					}
				} 
				else if(frm.flag == 2) 
				{
					cfg[0].m_usQianUdpPort = htons( (ushort)atol(str) );
					frm.reply = 0x02;
					
					if(atol(str)!=0) 
					{ 
						if( !SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) ) )	
						{
							frm.reply = 0x01;

							Frm35 frm1;		//通知千里眼端口改变了
							frm1.reply = 0x01;
							DataPush((char*)&frm1, sizeof(frm1), DEV_DIAODU, DEV_SOCK, 2);
							
							tag2DDvrCfg objMvrCfg;
					    GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
							objMvrCfg.IpPortPara.m_usPhotoPort = htons((ushort)atol(str));
							SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
							char by = 0x0c;//通知DvrExe 端口改变了
							DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
						}
					}					
				} 
				else if(frm.flag == 3) // 调度屏上的千里眼 UDP port 2
				{
					cfg[1].m_usQianUdpPort = htons( (ushort)atol(str) );
					frm.reply = 0x02;

					// 暂时使用此项修改King中心的视频连接, 以后最好调度屏上要增加此项设置菜单
					cfg[0].m_usVUdpPort = htons( (ushort)atol(str) );

					if(atol(str)!=0)
					{ 
						if( !SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) ) )	
						{
							frm.reply = 0x01; 

							Frm35 frm1;		//通知千里眼端口改变了
							frm1.reply = 0x01;
							DataPush((char*)&frm1, sizeof(frm1), DEV_DIAODU, DEV_SOCK, 2);
							
							tag2DDvrCfg objMvrCfg;
					    GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
							objMvrCfg.IpPortPara.m_usVideoPort = htons((ushort)atol(str));
							SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
							char by = 0x0c;//通知DvrExe 端口改变了
							DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
						}
					}
				} 
				else if(frm.flag == 4) // 流媒体升级中心
				{
					cfg2.m_usLiuPort = htons( (ushort)atol(str) );
					frm.reply = 0x02;
					
					if(atol(str)!=0)
					{ 
						if( !SetImpCfg(&cfg2, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) ) )	
							frm.reply = 0x01; 
					}

					char buf[2] = {0x01, 0x03};		//通知流媒体
 					DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);
 					
 					tag2DDvrCfg objMvrCfg;
					GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
					objMvrCfg.IpPortPara.m_usUpdatePort = htons((ushort)atol(str));
					SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
 					char by = 0x0c;//通知DvrExe 端口改变了
					DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
				} 
				else if(frm.flag == 5) // 管理平台
				{
					cfg2.m_usLiuPort2 = htons( (ushort)atol(str) );
					frm.reply = 0x02;
					if(atol(str)!=0)
					{ 
						if( !SetImpCfg(&cfg2, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) ) )	
							frm.reply = 0x01; 
					}
				}

				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);	
			}
			break;
			
		case 0xAB:	//激活GPRS请求
			{
				FrmAA frm;
				frm.reply = 0x01;	//已经激活
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);

				Frm35 frm1;	
				frm1.reply = 0x01;
				DataPush((char*)&frm1, sizeof(frm1), DEV_DIAODU, DEV_SOCK, 2);
			}
			break;
			
		case 0xAD:	//车台状态查询
			{
				char frm[1024] = {0};
				int len = 0;
				
				frm[len++] = (char)0xE9;
				frm[len++] = (char)0x00;
				
				GPSDATA gps(0);
				GetCurGps( &gps, sizeof(gps), GPS_LSTVALID );
				
				tag1PComuCfg cfg;
				GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
				
				tag2QServCodeCfg cfg2;
				GetSecCfg( &cfg2, sizeof(cfg2), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(cfg2) );
				
				char str[200] = {0};
				
				sprintf(str, "%s:%.21s,", LANG_SMSCENT, cfg.m_szSmsCentID);
				strcpy(frm+len, str);
				len += strlen(str);
				
				sprintf(str, "%s:%.14s,", LANG_SPECCODE, cfg2.m_szQianSmsTel);
				strcpy(frm+len, str);
				len += strlen(str);
				
				if( NETWORK_TYPE==NETWORK_GSM )
					sprintf(str, "Network:%s,", 2 == g_objPhoneGsm.GetPhoneState() ? LANG_ONLINE : LANG_OFFLINE );
				else if( NETWORK_TYPE==NETWORK_TD )
					sprintf(str, "Network:%s,", 2 == g_objPhoneTd.GetPhoneState() ? LANG_ONLINE : LANG_OFFLINE );
				else if( NETWORK_TYPE==NETWORK_EVDO )
					sprintf(str, "Network:%s,", 2 == g_objPhoneEvdo.GetPhoneState() ? LANG_ONLINE : LANG_OFFLINE );
				else if( NETWORK_TYPE==NETWORK_WCDMA )
					sprintf(str, "Network:%s,", 2 == g_objPhoneWcdma.GetPhoneState() ? LANG_ONLINE : LANG_OFFLINE );
				
				
				strcpy(frm+len, str);
				len += strlen(str);
				
				sprintf(str, "GPS:%s", 'A'==gps.valid ? LANG_VALID : LANG_INVALID );
				strcpy(frm+len, str);
				len += strlen(str);
				
				frm[1] = len - 2;	// 长度
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, len, frm, dwPktNum);
			}
			break;
			
			
			// 用户注册	--------------------------------------------------
		case 0x90:	//用户注册请求
		case 0x92:	//用户注销请求
		case 0x94:	//收到用户列表
			DataPush(v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
			break;
			
//  		case 0xAF:	//流媒体参数设置请求（所有IP，端口参数已整合到同一结构体内）
//  			break;

		case 0xAF:	//设置拨号用户名和密码请求
			{
				char userName[100] = {0};
				char passWord[100] = {0};
				memcpy(userName, v_szBuf+2, v_szBuf[1]);
				memcpy(passWord, v_szBuf+2+v_szBuf[1]+1, v_szBuf[(2+v_szBuf[1])]);
				
				FrmAE frm;

				tag1PComuCfg objComuCfg;
				GetImpCfg( &objComuCfg, sizeof(objComuCfg),	offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );
	
// 				tag1QIpCfg cfg[2];
// 				GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );
 				memset(objComuCfg.m_szCdmaUserName, 0, sizeof(objComuCfg.m_szCdmaUserName) );
				memcpy(objComuCfg.m_szCdmaUserName, userName, min(sizeof(objComuCfg.m_szCdmaUserName),strlen(userName)) );
				memset(objComuCfg.m_szCdmaPassWord, 0, sizeof(objComuCfg.m_szCdmaUserName) );
				memcpy(objComuCfg.m_szCdmaPassWord, passWord, min(sizeof(objComuCfg.m_szCdmaUserName),strlen(passWord)) );
				
				frm.reply = 0x02;
				if( (strlen(userName)!=0) && (strlen(passWord)!=0) ) { 
					if(!SetImpCfg(&objComuCfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(objComuCfg) ))	
						frm.reply = 0x01; 
				}
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
				if( NETWORK_TYPE==NETWORK_GSM )
					g_objPhoneGsm.ResetPhone();
				else if( NETWORK_TYPE==NETWORK_TD )
					g_objPhoneTd.ResetPhone();
				else if( NETWORK_TYPE==NETWORK_EVDO )
					g_objPhoneEvdo.ResetPhone();
				else if( NETWORK_TYPE==NETWORK_WCDMA )
					g_objPhoneWcdma.ResetPhone();
			}
			break;

		case 0xb1: // 请求GSM模块的IMEI串号
			{
				char frm[100] = { 0 };
				frm[0] = char(0xb0);
				
				char szImei[30] = { 0 };
				if( NETWORK_TYPE==NETWORK_GSM )
					g_objPhoneGsm.GetPhoneIMEI( szImei, sizeof(szImei) );
				else if( NETWORK_TYPE==NETWORK_TD )
					g_objPhoneTd.GetPhoneIMEI( szImei, sizeof(szImei) );
				else if( NETWORK_TYPE==NETWORK_EVDO )
					g_objPhoneEvdo.GetPhoneIMEI( szImei, sizeof(szImei) );
				else if( NETWORK_TYPE==NETWORK_WCDMA )
					g_objPhoneWcdma.GetPhoneIMEI( szImei, sizeof(szImei) );
				strncpy( frm + 1, szImei, sizeof(frm) - 2 );
				
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, strlen(frm), frm, dwPktNum);
			}
			break;
			
			
			// 管理业务 ----------------------------------------------------------
		case 0xC1: // 计价重车信号设置请求	计价重车信号类型(1) 00 表示低电平   01 表示高电平, 其余保留
			{
				char frm[ 20 ] = { 0 };
				frm[0] = BYTE(0xc2);
				frm[1] = 1;
				
				if( v_iLen < 3 )
				{
					PRTMSG(MSG_ERR, "Err Frm:%02x\n", BYTE(v_szBuf[0]) );
					goto DEALDIAODU_C1_FAIL;
				}
				else
				{
					if( 0 == v_szBuf[1] )
					{
						PRTMSG(MSG_DBG, "Jijia Valid Cfg: Low\n" );
					}
					else if( 1 == v_szBuf[1] )
					{
						PRTMSG(MSG_DBG, "Jijia Valid Cfg: High\n" );
					}
					else
					{
						PRTMSG(MSG_ERR, "Jijia Valid Cfg: Data Err\n" );
						goto DEALDIAODU_C1_FAIL;
					}
				}
				
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, 2, frm, dwPktNum);
				break;
				
DEALDIAODU_C1_FAIL:
				frm[1] = 0;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, 2, frm, dwPktNum);
			}
			break;
			
		case 0xC3: // 中控信号设置请求	中控信号类别(1)+中控信号类型(1)
			// 信号类别: 01表示中控有效信号；02表示中控取消信号；其他保留
			// 信号类型: 00 表示低电平   01 表示高电平, 其余保留
			{
				char frm[ 20 ] = { 0 };
				frm[0] = BYTE(0xc4);
				frm[1] = v_szBuf[1];
				frm[2] = 1;
				
				if( v_iLen < 4 )
				{
					PRTMSG(MSG_ERR, "Err Frm:%02x", BYTE(v_szBuf[0]) );
					goto DEALDIAODU_C3_FAIL;
				}
				else
				{
					if( 1 == v_szBuf[1] )
					{
						if( 0 == v_szBuf[2] )
						{
							PRTMSG(MSG_DBG, "CentCtrl Valid Cfg: Low\n" );
						}
						else if( 1 == v_szBuf[2] )
						{
							PRTMSG(MSG_DBG, "CentCtrl Valid Cfg: High\n" );
						}
						else
						{
							PRTMSG(MSG_ERR, "CentCtrl Valid Cfg: Data Err\n" );
							goto DEALDIAODU_C3_FAIL;
						}
						
					}
					else if( 2 == v_szBuf[1] )
					{
						if( 0 == v_szBuf[2] )
						{
							PRTMSG(MSG_DBG, "CentCtrl Invalid Cfg: Low\n" );
						}
						else if( 1 == v_szBuf[2] )
						{
							PRTMSG(MSG_DBG, "CentCtrl Invalid Cfg: High\n" );
						}
						else
						{
							PRTMSG(MSG_ERR, "CentCtrl Invalid Cfg: Data Err\n" );
							goto DEALDIAODU_C3_FAIL;
						}
					}
					else
					{
						PRTMSG(MSG_ERR, "Err CentCtrl Frm: %02x\n", BYTE(v_szBuf[1]) );
						goto DEALDIAODU_C3_FAIL;
					}
				}
				
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, 3, frm, dwPktNum);
				break;
				
DEALDIAODU_C3_FAIL:
				frm[2] = 0;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, 3, frm, dwPktNum);
			}
			break;
			
		case 0xC5: // 开车门信号设置请求	开车门信号类型(1) 00 表示低电平   01 表示高电平, 其余保留
			{
				char frm[ 20 ] = { 0 };
				frm[0] = BYTE(0xc6);
				frm[1] = 1;
				
				if( v_iLen < 3 )
				{
					PRTMSG(MSG_ERR, "Err Frm:%02x\n", BYTE(v_szBuf[0]) );
					goto DEALDIAODU_C5_FAIL;
				}
				else
				{
					if( 0 == v_szBuf[1] )
					{
						PRTMSG(MSG_DBG, "Door Valid Cfg: Low\n" );
					}
					else if( 1 == v_szBuf[1] )
					{
						PRTMSG(MSG_DBG, "Door Valid Cfg: High\n" );
					}
					else
					{
						PRTMSG(MSG_ERR, "Door Valid Cfg: Data Err\n" );
						goto DEALDIAODU_C5_FAIL;
					}
				}
				
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, 2, frm, dwPktNum);
				break;
				
DEALDIAODU_C5_FAIL:
				frm[1] = 0;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, 2, frm, dwPktNum);
			}
			break;
			
		case 0xC7: // 手刹信号设置请求	手刹信号类型(1) 00 表示低电平   01 表示高电平, 其余保留
			{
				char frm[ 20 ] = { 0 };
				frm[0] = BYTE(0xc8);
				frm[1] = 1;
				
				if( v_iLen < 3 )
				{
					PRTMSG(MSG_ERR, "Err Frm:%02x\n", BYTE(v_szBuf[0]) );
					goto DEALDIAODU_C7_FAIL;
				}
				else
				{
					if( 0 == v_szBuf[1] )
					{
						PRTMSG(MSG_DBG, "Shousha Valid Cfg: Low\n" );
					}
					else if( 1 == v_szBuf[1] )
					{
						PRTMSG(MSG_DBG, "Shousha Valid Cfg: High\n" );
					}
					else
					{
						PRTMSG(MSG_ERR, "Shousha Valid Cfg: Data Err\n" );
						goto DEALDIAODU_C7_FAIL;
					}
				}
				
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, 2, frm, dwPktNum);
				break;
				
DEALDIAODU_C7_FAIL:
				frm[1] = 0;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, 2, frm, dwPktNum);
			}
			break;
			
		case 0xC9: // 脚刹信号设置请求	脚刹信号类型(1) 00 表示低电平   01 表示高电平, 其余保留
			{
				char frm[ 20 ] = { 0 };
				frm[0] = BYTE(0xca);
				frm[1] = 1;
				
				if( v_iLen < 3 )
				{
					PRTMSG(MSG_ERR, "Err Frm:%02x\n", BYTE(v_szBuf[0]) );
					goto DEALDIAODU_C9_FAIL;
				}
				else
				{
					if( 0 == v_szBuf[1] )
					{
						PRTMSG(MSG_DBG, "Jiaosha Valid Cfg: Low\n" );
					}
					else if( 1 == v_szBuf[1] )
					{
						PRTMSG(MSG_DBG, "Jiaosha Valid Cfg: High\n" );
					}
					else
					{
						PRTMSG(MSG_ERR, "Jiaosha Valid Cfg: Data Err\n" );
						goto DEALDIAODU_C9_FAIL;
					}
				}
				
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, 2, frm, dwPktNum);
				break;
				
DEALDIAODU_C9_FAIL:
				frm[1] = 0;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV2, 2, frm, dwPktNum);
			}
			break;
			
		case 0xCD:	//请求车载终端版本号
			{
				char szSoftVer[64] = {0};
				ChkSoftVer(szSoftVer);

				char szSysVer[64] = {0};
				ReadVerFile(szSysVer);

				FrmCC frm;
				sprintf(frm.ver, "%s-%s", szSoftVer, szSysVer);
				
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			break;
			
		case 0xCF:	//探询调度终端应答
			DataPush(v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
			break;
			
			
			// 管理通道业务 ----------------------------------------------------------
		case 0xD1:	//发送管理命令应答
		case 0xD3:	//请求发送管理响应
			DataPush(v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
			break;
			
			// 摄像头调试业务 -------------------------------------------------------
		case 0xE0:	//摄像头调试指示
			{
				char buf[2] = {0x05, v_szBuf[1]-1};
				DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV1);
			}
			break;
			
			// 视频文件导入业务 -------------------------------------------------------
		case 0xE1:	//视频文件导入请求
			{
				char szBuf[2] = {0x8e, 0x10};
				DataPush(szBuf, 2, DEV_DIAODU, DEV_QIAN, LV2);				
			}
			break;
			
			// 数据导出业务 -------------------------------------------------------
		case 0xE2:	//数据导出请求
			{
				pthread_t thDataOutPut;
				pthread_create(&thDataOutPut, NULL, G_DataOutPutThread, (void *)this);
			}
			break;			
			
			// 车台ID密钥业务 ---------------------------------------------------------
 		case 0xE4:	//写车台ID和密钥  车台ID(4)+车台密钥(16)+IP地址(4)+端口(2)+备用IP地址(4)+备用端口(2)
 			break;	//（暂不支持）


		case 0xE6:	//查询车台ID和密钥
 			break;	//（暂不支持）			
			

		case 0xEA:	//U盘系统升级
			pthread_t thSysUpdate;
			if( access("/mnt/UDisk/part1/image.bin", F_OK) == 0 )
				pthread_create(&thSysUpdate, NULL, G_ImageUpdateThread, (void *)this);
			else
				pthread_create(&thSysUpdate, NULL, G_SysUpdateThread, (void *)this);
			break;
			
			
		case 0xEC:	//进入工程菜单请求
			{
				char szPhoneMode[100] = {0};
				
				char pass[100] = {0};
				memcpy(pass, v_szBuf+1, v_iLen-1);
				
				static bool debuging = false;
				static bool bSetLine = false;
				static bool bVideoDebug = false;

				// 设置自动报站线路号
				if( ((v_iLen-2)<16) && bSetLine )
				{
					bSetLine = false;

					int iLen = v_iLen-1;
					BYTE *szBuf = new BYTE[iLen+3];

					szBuf[0] = 0x8e;
					szBuf[1] = 0x99;
					szBuf[2] = iLen;

					for(int i=0;i<iLen;i++)
					{			
						szBuf[3+i] = v_szBuf[1+i];
					}	
					
					DataPush(szBuf, 3+iLen, DEV_DIAODU, DEV_QIAN, LV2);

					delete szBuf; 
					szBuf=NULL;

					//show_diaodu("线路号:%s", v_szBuf+1);

					break;
				}
				
				if(strcmp(pass, ALL_PASS)==0) 
				{
					FrmED frm;
					frm.reply = 0x00;
					g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
					break;					
				} 
				else if(strcmp(pass, EXP_PASS)==0) // 进入测试模式
				{					
					debuging = true;

					//break;
				} 
				else if(strcmp(pass, "1")==0 && debuging)
				{
					// 通知DvrExe进行SD卡状态查询，并向调度屏返回提示
					char buf = 0x07;
					DataPush(&buf, 1, DEV_DIAODU, DEV_DVR, LV1);

					break;
				} 
				else if(strcmp(pass, "2")==0 && debuging)	// 视频文件导出
				{
					pthread_t thDataOutPut;
					pthread_create(&thDataOutPut, NULL, G_VideoOutPutThread, (void *)this);
					
					break;
				}
				else if(strcmp(pass, "3")==0 && debuging)	// 复位手机模块
				{
					if( NETWORK_TYPE==NETWORK_GSM )
						g_objPhoneGsm.ResetPhone();
					else if( NETWORK_TYPE==NETWORK_TD )
						g_objPhoneTd.ResetPhone();
					else if( NETWORK_TYPE==NETWORK_EVDO )
						g_objPhoneEvdo.ResetPhone();
					else if( NETWORK_TYPE==NETWORK_WCDMA )
						g_objPhoneWcdma.ResetPhone();
					
					break;
				} 
				else if(strcmp(pass, "4")==0 && debuging) // 流媒体立即登陆
				{			
					char buf[2] = {0x01, 0x03};
					DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);			
					break;
				} 				
				else if(strcmp(pass, "5")==0 && debuging) // 系统复位
				{						
					G_ResetSystem();	
					break;
				}
				else if(strcmp(pass, "116688")==0 ) // 选择报站线路
				{						
					bSetLine = true;
					show_diaodu("请输入线路号");
					break;
				}
				else if(strcmp(pass, CHOOSE_LC6311_PASS)==0)
				{
					strcpy(szPhoneMode, "LC6311");
				}
				else if(strcmp(pass, CHOOSE_MC703OLD_PASS)==0)
				{
					strcpy(szPhoneMode, "MC703OLD");
				}
				else if(strcmp(pass, CHOOSE_MC703NEW_PASS)==0)
				{
					strcpy(szPhoneMode, "MC703NEW");
				}
				else if(strcmp(pass, CHOOSE_SIM5218_PASS)==0)
				{
					strcpy(szPhoneMode, "SIM5218");
				}
				else if(strcmp(pass, "12345")==0) // 进入视频测试模式
				{
					show_diaodu("开启视频测试模式");
					bVideoDebug = true;
					break;
				}
				else if(strcmp(pass, "1")==0 && bVideoDebug) // 启动通道1
				{
					show_diaodu("启动通道1");

					char buf[] = {0x00,0x7e,0x1b,0x20,0x83,0x68,0x7f,0x00,0x38,0x01,0x31,0x33,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x20,0x20,0x20,0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x20,0x20,0x01,0x05,0x01,0x01,0x02,0x02,0x01,0x01,0x03,0x01,0x7f,0x04,0x01,0x0a,0x05,0x03,0x7f,0x0a,0x7f,0x7f,0x7f,0x7b,0x7e};
					DataPush(buf, sizeof(buf), DEV_SOCK, DEV_QIAN, LV2);
					
					break;
				}
				else if(strcmp(pass, "2")==0 && bVideoDebug) // 启动通道1,2
				{
					show_diaodu("启动通道1,2");

					char buf[] = {0x00,0x7e,0x15,0x20,0x83,0x68,0x7f,0x00,0x38,0x01,0x31,0x33,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x20,0x20,0x20,0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x20,0x20,0x01,0x05,0x01,0x01,0x02,0x02,0x01,0x03,0x03,0x01,0x7f,0x04,0x01,0x05,0x05,0x03,0x7f,0x0a,0x7f,0x7f,0x7f,0x78,0x7e};
					DataPush(buf, sizeof(buf), DEV_SOCK, DEV_QIAN, LV2);

					break;
				}
				else if(strcmp(pass, "3")==0 && bVideoDebug) // 启动通道1,2,3
				{
					show_diaodu("启动通道1,2,3");

					char buf[] = {0x00,0x7e,0x17,0x20,0x83,0x68,0x7f,0x00,0x38,0x01,0x31,0x33,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x20,0x20,0x20,0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x20,0x20,0x01,0x05,0x01,0x01,0x02,0x02,0x01,0x07,0x03,0x01,0x7f,0x04,0x01,0x02,0x05,0x03,0x7f,0x0a,0x7f,0x7f,0x7f,0x79,0x7e};
					DataPush(buf, sizeof(buf), DEV_SOCK, DEV_QIAN, LV2);

					break;
				}
				else if(strcmp(pass, "4")==0 && bVideoDebug) // 启动通道1,2,3,4
				{
					show_diaodu("启动通道1,2,3,4");

					char buf[] = {0x00,0x7e,0xa5,0x20,0x83,0x68,0x7f,0x00,0x38,0x01,0x31,0x33,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x20,0x20,0x20,0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x20,0x20,0x01,0x05,0x01,0x01,0x02,0x02,0x01,0x0f,0x03,0x01,0x7f,0x04,0x01,0x01,0x05,0x03,0x7f,0x0a,0x7f,0x7f,0x7f,0x01,0x7e};
					DataPush(buf, sizeof(buf), DEV_SOCK, DEV_QIAN, LV2);

					break;
				}
				else if(strcmp(pass, "5")==0 && bVideoDebug) // 撤销监控
				{
					show_diaodu("撤销监控");
					char buf[] = {0x00,0x7e,0x3c,0x20,0x83,0x68,0x7f,0x00,0x38,0x02,0x31,0x33,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x20,0x20,0x20,0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x20,0x20,0x4c,0x7e};
					DataPush(buf, sizeof(buf), DEV_SOCK, DEV_QIAN, LV2);
					break;
				}
				
				if( strlen(szPhoneMode)>0 )
				{
					tag2DDvrCfg objMvrCfg;
					GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
					strcpy(objMvrCfg.IpPortPara.m_szPhoneMod, szPhoneMode);
					
					if( SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg))==0 )
					{
						show_diaodu("变更手机模块成功,设备重启后生效");
						break;
					}
				}
				
				// 提示"密码错误"					
				FrmED frm;
				frm.reply = 0x01;	//密码错误
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);				
			} 
			break;			
			
		case 0xEE:	//工程菜单下子菜单请求
			{
				FrmEE *pfrm = (FrmEE*)v_szBuf;
				switch(pfrm->flag) 
				{
				case 0x00:	//传感器状态查询
					{
						char frm[1024] = {0};
						int len = 0;
						
						frm[len++] = (char)0xE9;
						frm[len++] = (char)0x00;
						
						GPSDATA gps(0);
						GetCurGps( &gps, sizeof(gps), GPS_LSTVALID );
						
						unsigned char uszResult;
						IOGet((byte)IOS_ACC, &uszResult );
						build_frm2((IO_ACC_ON == uszResult), "ACC", LANG_VALID, LANG_INVALID);
						
#if VEHICLE_TYPE == VEHICLE_M	
						//空重车/计价器
						IOGet((byte)IOS_JIJIA, &uszResult );
						build_frm2((IO_JIJIA_HEAVY == uszResult),	LANG_IO_METER, LANG_HEAVY, LANG_EMPTY);
						
						//前车门信号
						IOGet((byte)IOS_FDOOR, &uszResult );
						build_frm2((IO_FDOOR_OPEN == uszResult),	LANG_IO_FDOOR, LANG_VALID, LANG_INVALID);
						
						//后车门信号
						IOGet((byte)IOS_BDOOR, &uszResult );
						build_frm2((IO_BDOOR_OPEN == uszResult),	LANG_IO_BDOOR, LANG_VALID, LANG_INVALID);
						
						//载客信号1
						IOGet((byte)IOS_PASSENGER1, &uszResult );
						build_frm2((IO_PASSENGER1_VALID == uszResult),	LANG_IO_LOAD1, LANG_VALID, LANG_INVALID);
						
						//载客信号2
						IOGet((byte)IOS_PASSENGER2, &uszResult );
						build_frm2((IO_PASSENGER2_VALID == uszResult),	LANG_IO_LOAD2, LANG_VALID, LANG_INVALID);
						
						//载客信号3
						IOGet((byte)IOS_PASSENGER3, &uszResult );
						build_frm2((IO_PASSENGER3_VALID == uszResult),	LANG_IO_LOAD3, LANG_VALID, LANG_INVALID);
#endif
						
#if VEHICLE_TYPE == VEHICLE_V8
						//空重车/计价器
						IOGet((byte)IOS_JIJIA, &uszResult );
						build_frm2((IO_JIJIA_HEAVY == uszResult),	LANG_IO_METER, LANG_HEAVY, LANG_EMPTY);
						
						//前车门信号
						IOGet((byte)IOS_FDOOR, &uszResult );
						build_frm2((IO_FDOOR_OPEN == uszResult),	LANG_IO_FDOOR, LANG_VALID, LANG_INVALID);
						
						// 欠压信号
						IOGet((byte)IOS_QIANYA, &uszResult );
						build_frm2((IO_QIANYA_VALID == uszResult), LANG_IO_QIANYA, LANG_VALID, LANG_INVALID);
						
						// 电子喇叭信号
						IOGet((byte)IOS_ELECSPEAKER, &uszResult );
						build_frm2((IO_ELECSPEAKER_VALID == uszResult),	LANG_IO_ELECSPEAKER, LANG_VALID, LANG_INVALID);
						
						// 电源输入检测信号
						IOGet((byte)IOS_POWDETM, &uszResult );
						build_frm2((IO_POWDETM_VALID == uszResult),	LANG_IO_POWDETM, LANG_VALID, LANG_INVALID);
						
						// 车灯输入信号
						IOGet((byte)IOS_CHEDENG, &uszResult );
						build_frm2((IO_CHEDENG_VALID == uszResult),	LANG_IO_CHEDENG, LANG_VALID, LANG_INVALID);
						
						// 振铃信号
						IOGet((byte)IOS_ZHENLING, &uszResult );
						build_frm2((IO_ZHENLING_VALID == uszResult), LANG_IO_ZHENLING, LANG_VALID, LANG_INVALID);
#endif
						
#if VEHICLE_TYPE == VEHICLE_M2
						// 欠压信号
						IOGet((byte)IOS_QIANYA, &uszResult );
						build_frm2((IO_QIANYA_VALID == uszResult), LANG_IO_QIANYA, LANG_VALID, LANG_INVALID);
						
						// 前门报警信号
						IOGet((byte)IOS_FDOOR_ALERT, &uszResult );
						build_frm2((IO_FDOORALERT_VALID == uszResult),	LANG_IO_FDOORALERT, LANG_VALID, LANG_INVALID);
						
						//前车门信号
						IOGet((byte)IOS_FDOOR, &uszResult );
						build_frm2((IO_FDOOR_OPEN == uszResult),	LANG_IO_FDOOR, LANG_VALID, LANG_INVALID);
						
						// 后门报警信号
						IOGet((byte)IOS_BDOOR_ALERT, &uszResult );
						build_frm2((IO_BDOORALERT_VALID == uszResult),	LANG_IO_BDOORALERT, LANG_VALID, LANG_INVALID);
						
						//后车门信号
						IOGet((byte)IOS_BDOOR, &uszResult );
						build_frm2((IO_BDOOR_OPEN == uszResult),	LANG_IO_BDOOR, LANG_VALID, LANG_INVALID);
						
						// 电源输入检测信号
						IOGet((byte)IOS_POWDETM, &uszResult );
						build_frm2((IO_POWDETM_VALID == uszResult),	LANG_IO_POWDETM, LANG_VALID, LANG_INVALID);
						
						// 脚刹信号
						IOGet((byte)IOS_JIAOSHA, &uszResult );
						build_frm2((IO_JIAOSHA_VALID == uszResult),	LANG_IO_JIAOSHA, LANG_VALID, LANG_INVALID);
						
						// 车灯输入信号
						IOGet((byte)IOS_CHEDENG, &uszResult );
						build_frm2((IO_CHEDENG_VALID == uszResult),	LANG_IO_CHEDENG, LANG_VALID, LANG_INVALID);
						
						// 振铃信号
						IOGet((byte)IOS_ZHENLING, &uszResult );
						build_frm2((IO_ZHENLING_VALID == uszResult), LANG_IO_ZHENLING, LANG_VALID, LANG_INVALID);
#endif					
						
						bool meter_link = ( int(GetTickCount()-g_last_heartbeat) <20*1000 ) ? true : false ;
						build_frm2(meter_link, LANG_METER_CONNECTION, LANG_CONNECT,	LANG_DISCONNECT);
						
						
						//电源电平检测，不支持
						
						//报警
						IOGet((byte)IOS_ALARM, &uszResult );
						build_frm2(IO_ALARM_OFF == uszResult, LANG_IO_FOOTALERM, LANG_NORMAL, LANG_ALERM);
						
						//欠压信号，暂不支持
						
						char str[100] = {0};
#if USE_OIL == 3
						sprintf(str, "%s AD:%u,", LANG_IO_OIL, g_usOilAD);
#endif
						strcpy(frm+len, str); 
						len += strlen(str);

						DWORD speed = 0;
						DWORD tick;
						//tick = G_GetMeterTick(&speed);（需补充）
						sprintf(str, "%s:%u,%s:%u", LANG_IO_PULSE, tick, LANG_IO_SPEED, speed);
						strcpy(frm+len, str); 
						len += strlen(str);
						
						frm[1] = len - 2;
						g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, len, frm, dwPktNum);
						
					}
					break;
					
				case 0x01:	//传感器实时检测
					{
						m_iodect = 1;
						show_diaodu(LANG_IO_CHKMODE_OPEN);
						
						char by = 0x09;
						DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);		
					}
					break;
					
				case 0x02:	//PC音频测试
					{			
						show_diaodu("开始检测！");
						
						char by = 0x04;
						DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);		
					}
					break;
					
				case 0x03:	//系统音量加1
					{
						char buf[] = {0x01, 0x01};
						DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV1);
					}
					break;
					
				case 0x04:	//系统音量减1
					{
						char buf[] = {0x01, 0x02};
						DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV1);
					}
					break;
					
				case 0x05:	//启动GPS测试
					{
						unsigned char uszResult;

						if(!IOGet(IOS_ACC, &uszResult))
						{
							if(IO_ACC_ON == uszResult) // ACC有效
							{	
								m_gpsdect = 1;
#if CHK_VER == 0
								show_diaodu(LANG_GPS_CHKMODE_OPEN);	
#endif						
							}
							else
							{
								show_diaodu(LANG_ACC_NEEDVALID);	
							}
						}
					}
					break;
					
				case 0x06:	//串口检测
					{
						show_diaodu("开始检测！");

						char by = 0x02;
						DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
					}
					break;
					
				case 0x07:	//断油断电测试
					{
						show_diaodu("开始检测！");
						
						IOSet(IOS_OILELECCTL, IO_OILELECCTL_CUT, NULL, 0);
#if VEHICLE_TYPE == VEHICLE_M
						IOSet(IOS_TOPLEDPOW, IO_TOPLEDPOW_OFF, NULL, 0);
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
						IOSet(IOS_YUYIN, IO_YUYIN_ON, NULL, 0);
#endif
						sleep(1);
						IOSet(IOS_OILELECCTL, IO_OILELECCTL_RESUME, NULL, 0);
#if VEHICLE_TYPE == VEHICLE_M
						IOSet(IOS_TOPLEDPOW, IO_TOPLEDPOW_ON, NULL, 0);
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
						IOSet(IOS_YUYIN, IO_YUYIN_OFF, NULL, 0);
#endif
					}
					break;
					
				case 0x08:	//GSM检测
					{
						char str[100] = {0};
						BYTE bytSta;

						if( NETWORK_TYPE==NETWORK_GSM )
							bytSta = g_objPhoneGsm.GetPhoneState();
						else if( NETWORK_TYPE==NETWORK_TD )
							bytSta = g_objPhoneTd.GetPhoneState();
						else if( NETWORK_TYPE==NETWORK_EVDO )
							bytSta = g_objPhoneEvdo.GetPhoneState();
						else if( NETWORK_TYPE==NETWORK_WCDMA )
							bytSta = g_objPhoneWcdma.GetPhoneState();
						
						strcat(str, "Net Reg:");
						strcat(str, 1 == bytSta || 2 == bytSta ? LANG_LOGGED : LANG_NOLOG );
						if( NETWORK_TYPE==NETWORK_GSM )
							sprintf(str+strlen(str), " %s:%d ", LANG_SIGNAL, g_objPhoneGsm.GetPhoneSignal());
						else if( NETWORK_TYPE==NETWORK_TD )
							sprintf(str+strlen(str), " %s:%d ", LANG_SIGNAL, g_objPhoneTd.GetPhoneSignal());
						else if( NETWORK_TYPE==NETWORK_EVDO )
							sprintf(str+strlen(str), " %s:%d ", LANG_SIGNAL, g_objPhoneEvdo.GetPhoneSignal());
						else if( NETWORK_TYPE==NETWORK_WCDMA )
							sprintf(str+strlen(str), " %s:%d ", LANG_SIGNAL, g_objPhoneWcdma.GetPhoneSignal());

						strcat(str, "Net:" );
						strcat(str, 2 == bytSta ? LANG_ONLINE : LANG_OFFLINE );
						
						show_diaodu(str);
					}
					break;
					
				case 0x09:	//流媒体立即登陆
					{			
						char buf[2] = {0x01, 0x03};	
 						DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);
					}
					break;
					
				case 0x0C:	//重写LED屏, 改为格式化SD卡或者硬盘
					{
						char by = 0x06;
						DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
					}
					break;
					
				case 0x0d:	//恢复出厂设置
					{
						// 通知QianExe
						BYTE buf[2];
						buf[0] = 0xee;
						buf[1] = 0x0d;
						DataPush((char*)buf, 2, DEV_DIAODU, DEV_QIAN, 2);
						if(0==ResumeCfg(false, false))
						{
							//通知DvrExe
							char by = 0x0b;
							DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);		
							show_diaodu("%s %s!", LANG_RESUMECFG, LANG_SUCC);
						}
						else
							show_diaodu("%s %s!", LANG_RESUMECFG, LANG_FAIL);
					}
					break;
					
				case 0x0f:	//应用拷贝
					{
						pthread_t thCopy;

						if( access("/mnt/UDisk/part1/exe.bin", F_OK) == 0 )
							pthread_create(&thCopy, NULL, G_ExeUpdateThread, (void *)this);
						else
							pthread_create(&thCopy, NULL, G_AppUpdateThread, (void *)this);
					}
					break;
					
				case 0x10:	//视频测试（不支持）
					{
					}
					break;
					
				default:
					PRTMSG(MSG_DBG,"Project menu(%02X)!\n", pfrm->flag);
					break;
				}
			}
			break;

			//zzg add
			//凯天触摸屏通信协议
			//在QianExe里处理
			//		case 0x3c://一键拨号
			// 			{
			// 				show_msg("Rcv Comu3c");
			// 				Frm12 frm;
			// 				frm.len=5;
			// 				sprintf(frm.num,"%s","10086");
			//              DataPush((char*)(&frm), sizeof(frm), DEV_DIAODU, DEV_PHONE, 2);
			// 			}
			//			break;
		case 0x96:
			{
				//show_msg("Rcv Comu96");
				char res[2];
				res[0]=0x97;
				res[1]=0x02;
				DataPush(res, 2, DEV_DIAODU, DEV_DIAODU, 2);
			}			
			break;

		case 0x98:
			{
				//show_msg("Rcv Comu98");
				char res=0x99;
				DataPush(&res, 1, DEV_DIAODU, DEV_DIAODU, 2);
			}	
			break;

// 		case 0xad:
// 			{
// 				char frm[8] = {0};
// 				int len = 0;
// 				
// 				frm[len++] = (char)0xE9;
// 				frm[len++] = (char)0x00;
// 				GPSDATA gps(0);
// 				GetCurGps( &gps, sizeof(gps), GPS_LSTVALID );
// 				//GPS状态
// 				if(gps.valid == 'A') 
// 					frm[len++]=0x01;
// 				else 
// 					frm[len++]=0x02;
// 				//GPRS状态
// 				if(g_objPhone.GetPhoneState())
// 					frm[len++]=0x01;
// 				else 
// 					frm[len++]=0x02;
// 				DataPush(frm, 8, DEV_DIAODU, DEV_QIAN, 2);
// 			}
// 			break;

		case 0x3c:
		case 0x48:
		case 0x4a:
		case 0x73:
		case 0xd5:
		case 0xd7:		
		case 0xd9:
			//show_msg("Rcv ComuD9");
		case 0xda:
		case 0xdb:
			DataPush(v_szBuf, v_iLen, DEV_DIAODU, DEV_QIAN, 2);
			break;
			
		case 0x60:// 录音效果测试->查询设备状态
			{
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
				show_diaodu("GPS:%s;%s:%s;TW2865:%s", 
												g_objLightCtrl2.m_dwDevErrCode&0x00000001 ? "异常" : "正常",
												NETWORK_TYPE == NETWORK_TD?"LC6311":(NETWORK_TYPE == NETWORK_EVDO?"MC703":(NETWORK_TYPE == NETWORK_WCDMA?"SIM5218":"OTHER")),
												g_objLightCtrl2.m_dwDevErrCode&0x00000002 ? "异常" : "正常",
												g_objLightCtrl2.m_dwDevErrCode&0x00000004 ? "异常" : "正常");
#endif
			}
			break;
		
		default:
			break;
	}
}

//在调度屏上显示提示信息
//fmt,..: 格式化输入
void CDiaodu::show_diaodu(char *fmt, ...)
{
	int iRet;
	DWORD dwPktNum;
	va_list ap;
	
	va_start(ap, fmt);
	char str[1024] = {0x72, 0x01};
	vsprintf(str+2, fmt, ap);
	va_end(ap);
	
	iRet = g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, min(47,strlen(str)), str, dwPktNum);
}


//--------------------------------------------------------------------------------------------------------------------------
// 检查校验和(直接取累加和)
// buf: 数据缓冲区指针
// len: 数据的长度
// 返回: 校验是否正确
bool CDiaodu::check_crc(const byte *buf, const int len)
{   
	byte check_sum = get_crc( buf+1, len-1 );
	return (buf[0]==check_sum);
}

//--------------------------------------------------------------------------------------------------------------------------
// 得到校验和(直接取累加和)
// buf: 数据缓冲区指针
// len: 数据的长度
// 返回: 数据的校验和
byte CDiaodu::get_crc(const byte *buf, const int len)
{   
	byte check_sum = 0;
	for(int i=0; i<len; i++) {
		check_sum += *(buf+i);
	}
    return check_sum;
}

