#define _REENTRANT

#include <pthread.h>
#include "../GlobalShare/GlobFunc.h"
#include "../GlobalShare/ComuServExport.h"

#undef MSG_HEAD
#define MSG_HEAD ("IOExe-IOExe    ")

#define USE_IOEVT	1	// 1,IO中断+IO循环判断方式; 2,使用IO循环查询和判断方式

#define IO_NUM  sizeof(g_uiIOPinNo)/sizeof(UINT)

volatile bool g_bProgExit = false;

#if VEHICLE_TYPE == VEHICLE_M
UINT g_uiIOPinNo[] = 
{
	109, 212, 211, 213, 209, 65535, 65535, 216, 217, 218
};

BYTE g_ucIOSymb[IO_NUM] = 
{ 
	IOS_ACC, IOS_ALARM, IOS_JIJIA, IOS_FDOOR, IOS_BDOOR, IOS_JIAOSHA, IOS_SHOUSHA, IOS_PASSENGER1, IOS_PASSENGER2, IOS_PASSENGER3 
}; 

BYTE g_ucIOStateValid[IO_NUM] = 
{
	IO_ACC_ON, IO_ALARM_ON, IO_JIJIA_HEAVY, IO_FDOOR_OPEN, IO_BDOOR_OPEN, IO_JIAOSHA_VALID, IO_SHOUSHA_VALID, IO_PASSENGER1_VALID, IO_PASSENGER2_VALID, IO_PASSENGER3_VALID
};
#endif

#if VEHICLE_TYPE == VEHICLE_V8
UINT g_uiIOPinNo[] = 
{
	247, 221, 237, 238, 239, 240, 242, 248, 122
};

BYTE g_ucIOSymb[IO_NUM] = 
{ 
	IOS_ACC, IOS_QIANYA, IOS_ELECSPEAKER, IOS_FDOOR, IOS_ALARM, IOS_POWDETM, IOS_JIJIA, IOS_CHEDENG, IOS_ZHENLING
}; 

BYTE g_ucIOStateValid[IO_NUM] = 
{
	IO_ACC_ON, IO_QIANYA_VALID, IO_ELECSPEAKER_VALID, IO_FDOOR_OPEN, IO_ALARM_ON, IO_POWDETM_VALID, IO_JIJIA_HEAVY, IO_CHEDENG_VALID, IO_ZHENLING_VALID
};
#endif

#if VEHICLE_TYPE == VEHICLE_M2
UINT g_uiIOPinNo[] = 
{
	247, 221, 237, 238, 147, 242, 239, 146, 240, 248, 122
};

BYTE g_ucIOSymb[IO_NUM] = 
{ 
	IOS_ACC, IOS_QIANYA, IOS_FDOOR_ALERT, IOS_FDOOR, IOS_BDOOR_ALERT, IOS_BDOOR, IOS_ALARM, IOS_JIAOSHA, IOS_POWDETM, IOS_CHEDENG, IOS_ZHENLING
}; 

BYTE g_ucIOStateValid[IO_NUM] = 
{
	IO_ACC_ON, IO_QIANYA_VALID, IO_FDOORALERT_VALID, IO_FDOOR_OPEN, IO_BDOORALERT_VALID, IO_BDOOR_OPEN, IO_ALARM_ON, IO_JIAOSHA_VALID, IO_POWDETM_VALID, IO_CHEDENG_VALID, IO_ZHENLING_VALID
};
#endif

BYTE g_ucIOStateNew[IO_NUM] = {0};
BYTE g_ucIOStateOld[IO_NUM] = {0};

bool g_bIOTrig[IO_NUM] = {0};
DWORD g_ulIOTrigTime[IO_NUM] = {0};

tagIOCfg g_objIOCfg[IO_NUM] = {0};

void G_IOHandler( UINT v_uiIOPinNo, UINT v_uiIOPinLev )
{
	for(int i = 0; i < int(IO_NUM); i++)
	{
		if( g_uiIOPinNo[i] == v_uiIOPinNo )
		{
			g_ucIOStateNew[i] = v_uiIOPinLev ? g_objIOCfg[i].m_ucPinHigApp : g_objIOCfg[i].m_ucPinLowApp;
			//g_ulIOTrigTime[i] = GetTickCount()+g_objIOCfg[i].m_uiContPrid;
			//g_bIOTrig[i] = true; // 放最后比较好
			//PRTMSG( MSG_DBG, "IO Handle: %d, %d\n", v_uiIOPinNo, v_uiIOPinLev );
			break;
		}
	}
}

// 删除7天前的日志文件
void ManageDebugFile()
{
	struct dirent **namelist;
	int i = 0, n = 0;
	char szbuf[100] = {0};
	char szCmd[MAX_PATH] = {0};

	struct tm pCurrentTime;
	G_GetTime(&pCurrentTime);

	// 当前日期往前减7天
	pCurrentTime.tm_mday -= 7;
	if( pCurrentTime.tm_mday <= 0 )
	{
		if( pCurrentTime.tm_mon == 2 )
		{
			if( 0 == (pCurrentTime.tm_year + 1900) % 400 || (0 == (pCurrentTime.tm_year + 1900) % 4 && 0 != (pCurrentTime.tm_year + 1900) % 100) )
				pCurrentTime.tm_mday = 29;
			else 
				pCurrentTime.tm_mday = 28;
		}
		else if( pCurrentTime.tm_mon == 4 || pCurrentTime.tm_mon == 6 || pCurrentTime.tm_mon == 9 ||pCurrentTime.tm_mon == 11 )
			pCurrentTime.tm_mday = 30;
		else
			pCurrentTime.tm_mday = 31;

		pCurrentTime.tm_mon -= 1;
		if( pCurrentTime.tm_mon <= 0 )
		{
			pCurrentTime.tm_mon = 11;
			pCurrentTime.tm_year -= 1;
		}
	}

	sprintf(szbuf, "%04d-%02d-%02d.txt", pCurrentTime.tm_year+1900, pCurrentTime.tm_mon+1, pCurrentTime.tm_mday);
	
	n = scandir(VIDEO_SAVE_PATH, &namelist, 0, alphasort);

	if( n > 0 )
	{
		for(i=0; i<n; i++)
		{
			if( 0 != strstr(namelist[i]->d_name, ".txt") && strcmp(szbuf, namelist[i]->d_name) >= 0)
			{
				memset(szCmd, 0, sizeof(szCmd));
				sprintf(szCmd, "rm -f %s%s", VIDEO_SAVE_PATH, namelist[i]->d_name);
				SYSTEM(szCmd);
			}
		}
	}
}

void *G_RecvDataThread(void *arg)
{
	char  szbuf[2048] = {0};
	DWORD dwLen;
	BYTE  bytLvl;
	DWORD dwPushTm;
	DWORD dwSrcSymb;
	int   iResult;

	char  szFileName[MAX_PATH] = {0};
	FILE  *fp = NULL;

	while( !(iResult = DataWaitPop(DEV_IO)) ) 
	{
		if( g_bProgExit )  
		{ 
			if( NULL != fp )
			{
				fflush(fp);
				fclose(fp);
			}

			break; 
		}

		while( !DataPop(szbuf, 2048, &dwLen, &dwSrcSymb, DEV_IO, &bytLvl) )
		{ 
			if( dwLen < 1 ) continue;	//保险处理
			
			switch( dwSrcSymb )
			{
			case DEV_DVR:
				{
					switch(szbuf[0])
					{
					case 0x01:		// SD卡挂载成功通知
						{
							ManageDebugFile();

							struct tm pCurrentTime;
							G_GetTime(&pCurrentTime);

							memset(szFileName, 0, sizeof(szFileName));
							sprintf(szFileName, "%s%04d-%02d-%02d.txt", VIDEO_SAVE_PATH, pCurrentTime.tm_year+1900, pCurrentTime.tm_mon+1, pCurrentTime.tm_mday);

							// 若当天的日志文件存在，则打开，否则创建该天的日志文件
							if( access(szFileName, F_OK) == 0 )
							{
								fp = fopen(szFileName, "rb+");
								fseek(fp, 0, SEEK_END);
							}
							else
							{
								fp = fopen(szFileName, "wb+");
							}

							// 安全起见
							if( NULL == fp )
							{
								fp = fopen(szFileName, "wb+");
							}
						}
						break;

					case 0x02:		// SD卡卸载通知
						{
							if(fp != NULL)
							{
								fflush(fp);
								fclose(fp);
								fp = NULL;
							}
						}
						break;
					}
				}
				break;

			case DEV_QIAN:
				{
					if( fp != NULL )
					{
						fwrite(szbuf, dwLen, 1, fp);
						fwrite("\r\n", 2, 1, fp);
						fflush(fp);
					}
				}
				break;
				
			default:
				break;
			}
			
			usleep(100000);
		} 
	} 

	PRTMSG( MSG_DBG, "LoopRcvThd Exit!\n" );
	g_bProgExit = true;
}

void *G_ClrDogThread(void *arg)
{
	int i = 0;
	while( !g_bProgExit )
	{
		if( i++ % 5 == 0 )
		{
			DogClr( DOG_IO );
		}

		sleep(1);
	}
}

int main()
{
	ListPhoneInfo();
	
	int i;
	pthread_t pthread_RecvData, pthread_ClrDog;
	
	for(i = 0; i < int(IO_NUM); i++)
	{
		IOCfgGet( g_ucIOSymb[i], &(g_objIOCfg[i]), sizeof(tagIOCfg) );//获取IO配置参数
		IOGet( g_ucIOSymb[i], &g_ucIOStateNew[i] );//获取IO初始状态
		if(g_ucIOStateNew[i] == g_ucIOStateValid[i])
			g_ucIOStateOld[i] = ~g_ucIOStateNew[i];
		else
			g_ucIOStateOld[i] = g_ucIOStateNew[i];
	}

#if USE_IOEVT == 1
	PRTMSG( MSG_NOR, "IO Int Init!\n" );
	IOIntInit( 0xff, (void*)G_IOHandler );//初始化IO中断
#endif

	// 创建消息接收线程、清看门狗线程
	if( pthread_create(&pthread_RecvData, NULL, G_RecvDataThread, NULL) != 0 ||
		pthread_create(&pthread_ClrDog, NULL, G_ClrDogThread, NULL) != 0)
	{
		PRTMSG(MSG_ERR, "Create Recv Thread failed!\n");
		perror(" ");

		return ERR_THREAD;
	}

	PRTMSG(MSG_HEAD, "IOExe begin to run!\n");

// 	DWORD dwTickCount = GetTickCount();
// 	DWORD dwFlag = 0;
	
	// 主线程，循环判断IO
	unsigned ucNewSta = 0;
	while(!g_bProgExit)
	{
// 		if( GetTickCount() - dwTickCount > 2000 )
// 		{
// 			dwTickCount = GetTickCount();
// 			
// 			dwFlag = DogQueryOnly();
// 
// 			if( dwFlag != 0xffffffff )
// 				G_RecordDebugInfo("dwFlag = %08x", dwFlag);
// 		}

		for(i = 0; i < int(IO_NUM); i++)
		{
#if USE_IOEVT == 2
			if( IOGet( g_ucIOSymb[i], &g_ucIOStateNew[i] ) ) continue;
#endif

			ucNewSta = g_ucIOStateNew[i];

			if( g_ucIOStateOld[i] != ucNewSta )
			{
				if( !g_bIOTrig[i] )
				{
					g_ulIOTrigTime[i] = GetTickCount() + g_objIOCfg[i].m_uiContPrid;
					g_bIOTrig[i] = true;
				}
			}
			else
			{
				g_bIOTrig[i] = false;
				continue;
			}
			
			if( GetTickCount() >= g_ulIOTrigTime[i] )//去抖处理
			{
				// 进行去抖处理之后，若前后IO状态不一致，才向上层发送IO通知
				//if( g_ucIOStateOld[i] != g_ucIOStateNew[i] )
				{
					g_bIOTrig[i] = false;
					g_ucIOStateOld[i] = ucNewSta; // 不能直接使用g_ucIOStateNew[i],因为此时可能又被中断回调函数修改了
						
					DataPush(&g_ucIOStateOld[i], sizeof(BYTE), DEV_IO, 0xff & (~DEV_IO), 2);
				
					PRTMSG(MSG_NOR, "IO send one signal: PinNo = %d\n", g_uiIOPinNo[i]);
				}
			}
		}

		usleep(20000);
	}

	g_bProgExit = true;

	// 等待子线程退出
	sleep(15);
	
	PRTMSG(MSG_NOR, "IOExe is now to quit\n");

	return 0;
}

