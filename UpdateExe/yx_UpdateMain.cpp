#include "yx_UpdateStdAfx.h"

#undef  MSG_HEAD
#define MSG_HEAD	("Update-Main")

volatile bool g_bProgExit = false;

int _InitAll()
{
	// 创建消息队列
	g_iMsgId = msgget((key_t)1235, 0666 | IPC_CREAT);
	if( g_iMsgId == -1 )
	{
		perror("msgget error\n");
		return 1;
	}

	g_objTimerMng.Init();
	g_objDownLoad.Init();

	return 0;
}

int _ReleaseAll()
{
	g_objDownLoad.Release();
	g_objTimerMng.Release();
	
	//删除消息队列
	msgctl(g_iMsgId, IPC_RMID, 0);

	return 0;
}

void *G_RecvDataThread(void* arg)
{
	char szbuf[10240] = {0};
	DWORD dwLen;
	BYTE  bytLvl;
	DWORD dwPushTm;
	DWORD dwSrcSymb;
	UPDATEMSG msg;

	int iResult = DataWaitPop(DEV_UPDATE); 
	while( !iResult ) 
	{
		if( g_bProgExit )  
		{ 
			break; 
		} 
		
		while( !DataPop(szbuf, sizeof(szbuf), &dwLen, &dwSrcSymb, DEV_UPDATE, &bytLvl) )
		{ 
			if( dwLen < 1 ) continue;	//保险处理
			
			switch( dwSrcSymb )
			{
				case DEV_SOCK:
					g_objUpdateSock.AnalyseSockData(szbuf, dwLen);
					break;

				case DEV_DIAODU:
					{
						if( 0x01 == szbuf[0] )
						{
							switch( szbuf[1] )
							{
							case 0x03:	// 立即登陆请求
								{
									g_objDownLoad.m_bLoginNow = true;
								}
								break;

							default:
								break;
							}
						}
					}
					break;
				
				case DEV_GPS:
        {
          if( 0x01 == szbuf[0] )
          {
            g_objDownLoad.m_bRtcNeedChk = true;  //GPS没有进行对时，在登录流媒体后对时
         	}
          else if( 0x0 == szbuf[0] )
          {
            g_objDownLoad.m_bRtcNeedChk = false; //GPS已经对过时
          }
        }
          break; 

					
				default:
					break;
			}
		}
		
		iResult = DataWaitPop(DEV_UPDATE); 
	}

	if(ERR_MSGSKIPBLOCK == iResult)
	{
		// DataWaitPop返回出错，可能以后需要添加一些清除操作
	}

	msg.m_lMsgType = MSG_PROGQUIT;
	msgsnd(g_iMsgId, (void*)&msg, MSG_MAXLEN, 0);
}

void *G_ClrDogThread(void *arg)
{
	int i = 0;
	while( !g_bProgExit )
	{
		if( i++ % 5 == 0 )
		{
			DogClr( DOG_UPGD );
		}

		sleep(1);
	}
}

int main()
{
	ListPhoneInfo();
	
	pthread_t pRecvData, pthread_ClrDog;
	UPDATEMSG objMsg;
	
	sleep(3);

	// 所有初始化操作均放在此函数内
	_InitAll();	
	
	// 创建一个数据接收子线程
	if( pthread_create(&pRecvData, NULL, G_RecvDataThread, NULL) != 0
	  ||pthread_create(&pthread_ClrDog, NULL, G_ClrDogThread, NULL) != 0)
	{
		PRTMSG(MSG_ERR, "create deal thread failed:");
		perror(" ");
		_ReleaseAll();

		return -1;
	}
	
	PRTMSG(MSG_NOR, "UpdateExe begin to run\n");
	
	// 主线程
	while(!g_bProgExit)
	{
		if( msgrcv(g_iMsgId, (void*)&objMsg, MSG_MAXLEN, 0, 0) == -1)
		{
			PRTMSG(MSG_ERR, "UpdateExe msgrev failed:")
				perror("");
			
			usleep(100000);
			continue;
		}

		switch( objMsg.m_lMsgType )
		{
			case MSG_PROGQUIT:
				g_bProgExit = true;
				break;
				
			case MSG_SOCK_SEND:
				g_objDownLoad.SendSockData();
				break;
				
			default:
				break;
		}
		
		usleep(100000);
	}

	g_bProgExit = true;
	_ReleaseAll();
	
	//等待子线程退出
	sleep(15);

	PRTMSG(MSG_NOR, "UpdateExe is now to quit\n");
	
	return 0;
}


