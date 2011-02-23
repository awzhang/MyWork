#include "StdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("Monitor-Main")

#define APP_IN_PART2	5
#define APP_IN_PART5	6

volatile bool g_bProgExit = false;

void *G_ThreadClearDog(void *arg)
{
	int iDog = 0;

	if((iDog = open("/dev/misc/watchdog", O_WRONLY)) == -1)
	{
		PRTMSG(MSG_ERR, "open watchdog failed\n");
		system("reboot");
	}

	while(!g_bProgExit)
	{
		write(iDog, "\0", 1);
		sleep(5);
	}
}

int main()
{
	// 启动时即创建看门狗线程,一直喂狗,直到主线程通知退出
	pthread_t pthreadDog;
	pthread_create(&pthreadDog, NULL, G_ThreadClearDog, NULL);
	
	g_objDog.DogInit(); // 确保第一次判断看门狗清除标志能成功
	
	int i, j;
	int	iCnt = 0;		// 连续多少次未清狗的次数
	DWORD	dwFlag = 0;
	
	char *szAppInPart2[APP_IN_PART2] = {"/SockServExe", "/IOExe", "/UpdateExe", "/ComuExe", "/QianExe"};
	char *szAppInPart5[APP_IN_PART5] = {"/mnt/Flash/part5/DvrExe", "/mnt/Flash/part5/SockServExe", "/mnt/Flash/part5/IOExe", "/mnt/Flash/part5/UpdateExe", "/mnt/Flash/part5/QianExe", "/mnt/Flash/part5/ComuExe"};
	
	pid_t RunPid[6] = {0};
	int EndStatus = 0;
	int EndSucc = 0;

	char szPart, szError, szUpdate;
	int iAppInPartx;
	char **pszAppInPartx;
	
	CreateLog();
	
	// 如果ErrLog损坏,则默认运行分区2程序
	if(ReadLog(szPart, szError, szUpdate) == -1)
	{
		PRTMSG(MSG_DBG, "ErrLog damage, use default part2\n");
		
		szPart = '2'; szError = '0'; szUpdate = '0';
	}
	
	// 如果ErrLog误码,则默认运行分区5程序
	if((szPart != '2' && szPart != '5') || (szError < '0' || szError > '5') || (szUpdate < '0' || szUpdate > '5'))
	{
		PRTMSG(MSG_DBG, "ErrLog error, use default part5\n");
		
		szPart = '5';	szError = '0'; szUpdate = '0';
		if(WriteLog(szPart, szError, szUpdate) == -1)
		{
			PRTMSG(MSG_DBG, "write errlog fail\n");
			DeleteLog();
			goto EXIT;
		}
	}

ENTRY:
	PRTMSG(MSG_DBG, "Part:%c Error:%c Update:%c\n", szPart, szError, szUpdate);

	// 如果程序齐全且运行出错次数未达最大次数
	if(AllAppExist(szPart) == true && szError <= '8')
	{
		if(szPart == '2')
		{
			PRTMSG(MSG_DBG, "begin to run in part2\n");
			
			DOG_RSTVAL = 0XFFFFFFFF & ~DOG_QIAN & ~DOG_COMU & ~DOG_SOCK & ~DOG_IO & ~DOG_UPGD;// & ~DOG_DVR_T1 & ~DOG_DVR_T2 & ~DOG_DVR_T3;
			iAppInPartx = APP_IN_PART2;
			pszAppInPartx = szAppInPart2;
		}
		else if(szPart == '5')
		{
			PRTMSG(MSG_DBG, "begin to run in part5\n");
			
			DOG_RSTVAL = 0XFFFFFFFF & ~DOG_QIAN & ~DOG_COMU & ~DOG_SOCK & ~DOG_IO & ~DOG_UPGD & ~DOG_DVR & ~DOG_DVR_T1 & ~DOG_DVR_T2 & ~DOG_DVR_T3;
			iAppInPartx = APP_IN_PART5;
			pszAppInPartx = szAppInPart5;
		}
	}
	else
	{
		// 如果升级成功次数大于5次,则切换到分区2运行
		if(szUpdate == '5')
		{
			PRTMSG(MSG_DBG, "update too many times, switch to part2\n");
			
			szPart = '2';	szError = '0'; szUpdate = '0';
			if(WriteLog(szPart, szError, szUpdate) == -1)
			{
				PRTMSG(MSG_DBG, "write errlog fail\n");
				DeleteLog();
				goto EXIT;
			}
	
			goto ENTRY;
		}
		// 否则进行升级操作
		else
		{
			PRTMSG(MSG_DBG, "begin to update from part3\n");
						
			for(i = 0; i < 3; i++)
			{
				if(AllAppExist('3') == true)
				{
					if(G_ExeUpdate("/mnt/Flash/part3/Down/exe.bin", 0))
					{
						PRTMSG(MSG_DBG, "update succ, switch to part5\n");
						
						// 如果升级成功,切换到分区5,出错次数清零,升级次数递增
						szPart = '5';	szError = '0'; szUpdate++;
						if(WriteLog(szPart, szError, szUpdate) == -1)
						{
							PRTMSG(MSG_DBG, "write errlog fail\n");
							DeleteLog();
							goto EXIT;
						}
	
						goto ENTRY;
					}
				}
							
				sleep(1);
			}
			
			PRTMSG(MSG_DBG, "update fail, switch to part2\n");
			
			// 如果升级失败,切换到分区2,出错次数清零,升级次数清零
			szPart = '2';	szError = '0'; szUpdate = '0';
			if(WriteLog(szPart, szError, szUpdate) == -1)
			{
				PRTMSG(MSG_DBG, "write errlog fail\n");
				DeleteLog();
				goto EXIT;
			}
	
			goto ENTRY;
		}
	}

	// 创建进程, 加载程序
	for(i = 0; i < iAppInPartx; i++)
	{
		chmod(pszAppInPartx[i], S_IRWXU|S_IRWXG|S_IRWXO);

		if((RunPid[i] = fork()) == -1)
		{
			PRTMSG(MSG_DBG, "fork process %s fail, exit\n", pszAppInPartx[i]);
			
			// 如果创建子进程失败,递增出错次数
			szError++;
			if(WriteLog(szPart, szError, szUpdate) == -1)
			{
				DeleteLog();
			}
			goto EXIT;
		}
		else if(RunPid[i] == 0)
		{
			if(-1 == execl(pszAppInPartx[i], NULL))
			{
				// 如果子进程加载程序失败,则阻塞
				PRTMSG(MSG_DBG, "execl program %s fail, sleep\n", pszAppInPartx[i]);
				sleep(3600*48); 
			}
		}
	}
	
	while(!g_bProgExit)
	{
		sleep(8);

		dwFlag = g_objDog.DogQuery();

		if(dwFlag == 0xffffffff)
		{
			iCnt = 0;

			// 若持续正常运行时间超过20分钟(即1200秒,即150个循环),则将失败次数清零,升级次数清零
			static DWORD dwRunTime = 0;
			if(++dwRunTime > 150)
			{
				szError = '0'; szUpdate = '0';
				if(WriteLog(szPart, szError, szUpdate) == -1)
				{
					PRTMSG(MSG_DBG, "write errlog fail\n");
					DeleteLog();
					goto EXIT;
				}
			}
		}
		else
		{
			PRTMSG(MSG_DBG, "dwFlag = %08x\n", dwFlag);

			// 若连续5次未有效清狗(一次判断间隔为8秒,5次即为32秒)
			if(++iCnt > 5)
			{
				// 检查是否有子进程退出
				for(i = 0; i < iAppInPartx; i++)
				{
					if(waitpid(RunPid[i], &EndStatus, WNOHANG) == RunPid[i])
					{
						//子进程正常退出
						if(WIFEXITED(EndStatus) != 0)
						{
							EndSucc++;
						}
					}
				}
				
				// 如果所有子进程都正常退出,出错次数清零
				if(EndSucc == iAppInPartx)
				{
					szError = '0';
				}
				// 否则递增出错次数
				else
				{
					szError++;
				}
				
				PRTMSG(MSG_DBG, "%d/%d program end normally\n", EndSucc, iAppInPartx);
				
				if(WriteLog(szPart, szError, szUpdate) == -1)
				{
					DeleteLog();
				}
				goto EXIT;
			}
		}
	}
		
EXIT:
	g_bProgExit = true;
	sleep(3);
	return 0;
}

