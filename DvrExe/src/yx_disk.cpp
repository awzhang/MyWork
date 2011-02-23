#include "yx_common.h"

bool UCONNECT(void *Event)
{
	if( *((BYTE*)Event)==UCON_UMOU_UVAL )
		return true;
	else
		return false;
}

bool UMOUNT(void *Event)
{
	if( *((BYTE*)Event)==CON_UMOU_UVAL )
		return true;
	else
		return false;
}

bool MOUNT(void *Event)
{
	if( *((BYTE*)Event)==CON_MOU_UVAL || *((BYTE*)Event)==CON_MOU_VAL )
		return true;
	else
		return false;
}

bool UVALID(void *Event)
{
	if( *((BYTE*)Event)!=CON_MOU_VAL )
		return true;
	else
		return false;
}

bool VALID(void *Event)
{
	if( *((BYTE*)Event)==CON_MOU_VAL )
		return true;
	else
		return false;
}

void UmountDisk(int v_iDiskStyle, unsigned int v_uiDelay)
{
	int i;
	
	switch(v_iDiskStyle)
	{
		case SDISK:
		{
			for(i=0; i<PART_NUM_SDISK; i++)
			{
				g_objUsbUmount.SDisk[i] = TRUE;
				WaitForEvent(UMOUNT, &g_objUsbSta.SDisk[i], v_uiDelay);
			}
		}
			break;
			
		case HDISK:
		{
			for(i=0; i<PART_NUM_HDISK; i++)
			{
				g_objUsbUmount.HDisk[i] = TRUE;
				WaitForEvent(UMOUNT, &g_objUsbSta.HDisk[i], v_uiDelay);
			}
		}
			break;
			
		case UDISK:
		{
			for(i=0; i<PART_NUM_UDISK; i++)
			{
				g_objUsbUmount.UDisk[i] = TRUE;
				WaitForEvent(UMOUNT, &g_objUsbSta.UDisk[i], v_uiDelay);
			}
		}
			break;
		
		default:
			break;
	}
}

void MountDisk(int v_iDiskStyle, unsigned int v_uiDelay)
{
	int i;
	
	switch(v_iDiskStyle)
	{
		case SDISK:
		{
			for(i=0; i<PART_NUM_SDISK; i++)
			{
				g_objUsbUmount.SDisk[i] = FALSE;
				WaitForEvent(MOUNT, &g_objUsbSta.SDisk[i], v_uiDelay);
			}
		}
			break;
			
		case HDISK:
		{
			for(i=0; i<PART_NUM_HDISK; i++)
			{
				g_objUsbUmount.HDisk[i] = FALSE;
				WaitForEvent(MOUNT, &g_objUsbSta.HDisk[i], v_uiDelay);
			}
		}
			break;
			
		case UDISK:
		{
			for(i=0; i<PART_NUM_UDISK; i++)
			{
				g_objUsbUmount.UDisk[i] = FALSE;
				WaitForEvent(MOUNT, &g_objUsbSta.UDisk[i], v_uiDelay);
			}
		}
			break;
		
		default:
			break;
	}
}

bool CtrlDiskPart( char *v_szDiskPath, int v_iDiskPart, int v_iDiskSize, char *v_szFileSystem, int v_iInode, int v_iFlag )
{
	int pCmd;
	char szCmd[100];
	
	if( !v_szDiskPath )
	{
		PRTMSG(MSG_ERR, "No Disk Device\n");
		return false;
	}
	
	pCmd = open("/cmd", O_WRONLY|O_CREAT);
	if( pCmd==-1 )
	{
		PRTMSG(MSG_ERR, "Open Cmd File Failed\n");
		return false;
	}
	
	switch(v_iFlag)
	{
		case -1://删除分区
		{
			sprintf(szCmd, "d\n%d\nw\n", v_iDiskPart);
			write(pCmd, szCmd, sizeof(szCmd));
			close(pCmd);
			sleep(1);
			
			if(SYSTEM("fdisk %sdisc </cmd", v_szDiskPath))
			{
				PRTMSG(MSG_ERR, "Del Disk Part Failed\n");
				unlink("/cmd");
				return false;
			}
		}
			break;
		case 0://格式化分区
		{
			if(SYSTEM("mkfs.%s -i %d %spart%d", v_szFileSystem, v_iInode, v_szDiskPath, v_iDiskPart))
			{
				PRTMSG(MSG_ERR, "Format Disk Part Failed\n");
				unlink("/cmd");
				return false;
			}
		}
			break;
		case 1://创建分区
		{
			if(v_iDiskSize)
				sprintf(szCmd, "n\np\n%d\n\n+%dM\nw\n", v_iDiskPart, v_iDiskSize);
			else
				sprintf(szCmd, "n\np\n%d\n\n\nw\n", v_iDiskPart);
			write(pCmd, szCmd, sizeof(szCmd));
			close(pCmd);
			sleep(1);
			
			if(SYSTEM("fdisk %sdisc </cmd", v_szDiskPath))
			{
				PRTMSG(MSG_ERR, "Add Disk Part Failed\n");
				unlink("/cmd");
				return false;
			}
		}
			break;
		default:
			break;
	}
	
	close(pCmd);
	unlink("/cmd");
	return true;
}

int GetDiskSystem( int v_iDiskStyle, int v_iDiskPart, char *v_szFileSystem )
{
	char szDiskPart[128] = {0};
	struct statfs buf = {0};
	
	switch( v_iDiskStyle )
	{
		case SDISK:
		{
			if( MOUNT(&g_objUsbSta.SDisk[v_iDiskPart]) )
			{
				sprintf(szDiskPart, "/mnt/SDisk/part%d", v_iDiskPart+1);
				statfs(szDiskPart, &buf);
				goto FILE_SYSTEM;
			}
		}
			break;
		case HDISK:
		{
			if( MOUNT(&g_objUsbSta.HDisk[v_iDiskPart]) )
			{
				sprintf(szDiskPart, "/mnt/HDisk/part%d", v_iDiskPart+1);
				statfs(szDiskPart, &buf);
				goto FILE_SYSTEM;
			}
		}
			break;
		case UDISK:
		{
			if( MOUNT(&g_objUsbSta.UDisk[v_iDiskPart]) )
			{
				sprintf(szDiskPart, "/mnt/UDisk/part%d", v_iDiskPart+1);
				statfs(szDiskPart, &buf);
				goto FILE_SYSTEM;
			}
		}
			break;
		default:
			break;
	}
	
	return 1;
	
FILE_SYSTEM:
	if( v_szFileSystem )
	{
		switch(buf.f_type)
		{
			case 0xEF53:
				strcpy(v_szFileSystem, "EXT3"); 
				break;
			case 0x4d44:
				strcpy(v_szFileSystem, "FAT32"); 
				break;
			case 0x5346544e:
				strcpy(v_szFileSystem, "NTFS"); 
				break;
			default:
				strcpy(v_szFileSystem, "??????"); 
				break;
		}
	}
	
	return 0;
}

void GetDiskSize( int v_iDiskStyle, int v_iDiskPart, char *v_szTotalSize, char *v_szFreeSize, char *v_szAvailSize )
{
	double fSize = -1;
	char szDiskPart[128] = {0};
	struct statfs buf = {0};
		
	switch( v_iDiskStyle )
	{
		case SDISK:
		{
			if( MOUNT(&g_objUsbSta.SDisk[v_iDiskPart]) )
			{
				sprintf(szDiskPart, "/mnt/SDisk/part%d", v_iDiskPart+1);
				statfs(szDiskPart, &buf);
				goto DISK_SIZE;
			}
		}
			break;
		case HDISK:
		{
			if( MOUNT(&g_objUsbSta.HDisk[v_iDiskPart]) )
			{
				sprintf(szDiskPart, "/mnt/HDisk/part%d", v_iDiskPart+1);
				statfs(szDiskPart, &buf);
				goto DISK_SIZE;
			}
		}
			break;
		case UDISK:
		{
			if( MOUNT(&g_objUsbSta.UDisk[v_iDiskPart]) )
			{
				sprintf(szDiskPart, "/mnt/UDisk/part%d", v_iDiskPart+1);
				statfs(szDiskPart, &buf);
				goto DISK_SIZE;
			}
		}
			break;
		default:
			break;
	}
	
	if(v_szTotalSize) 
		sprintf( v_szTotalSize, "   " );
	if(v_szFreeSize)
		sprintf( v_szFreeSize, "   " );
	if(v_szAvailSize)
		sprintf( v_szAvailSize, "   " );
	return;
	
DISK_SIZE:
	if(v_szTotalSize) 
		sprintf( v_szTotalSize, "%.1fG", ((long long)buf.f_bsize * buf.f_blocks)/(double)(1024*1024*1024) );
	if(v_szFreeSize)
		sprintf( v_szFreeSize, "%.1fG", ((long long)buf.f_bsize * buf.f_bfree)/(double)(1024*1024*1024) );
	if(v_szAvailSize)
		sprintf( v_szAvailSize, "%.1fG", ((long long)buf.f_bsize * buf.f_bavail)/(double)(1024*1024*1024) );
}

int GetFileNum(char *v_szPath, char *v_szName, bool v_bRecur)
{
	int i;
	int iAllFileNum = 0;
	int iFileNum = 0;
	char szPath[1024] = {0};
  struct dirent **objList;
  struct stat objStat;
  
  iAllFileNum = scandir(v_szPath, &objList, 0, alphasort);	

	for(i=0; i<iAllFileNum; i++)
  {
  	ClearThreadDog(1);//防止文件太多时计算时间过长而来不及清看门狗
  	
    STRCPY(szPath, 2, v_szPath, objList[i]->d_name);
		if( 0!=stat(szPath, &objStat) )
		{
			PRTMSG(MSG_ERR, "stat err\n");
			perror("");
			continue;
		}

    if(S_ISREG(objStat.st_mode) || S_ISCHR(objStat.st_mode) || S_ISBLK(objStat.st_mode))//找到一个文件
    {
			if( STRCMP(v_szName, objList[i]->d_name)==0 )// 满足条件
	      iFileNum += 1;
	    
      strcpy(szPath, v_szPath);
    }
    else if(S_ISDIR(objStat.st_mode))//找到一个目录
    {
    	if(strcmp(objList[i]->d_name, ".") && strcmp(objList[i]->d_name, ".."))//忽略这两个目录
    	{
	    	if(v_bRecur)//递归查找
	    	{
	    		strcat(szPath, "/");
		      iFileNum += GetFileNum(szPath, v_szName, v_bRecur);//子目录下查找目标
		    }
	    	else if( STRCMP(v_szName, objList[i]->d_name)==0 )// 满足条件
			    iFileNum += 1;
	    }
    	
    	strcpy(szPath, v_szPath);
    }
	}

  return iFileNum;
}

//磁盘分区未挂载时获取到的是Flash的容量
void GetDiskSpace( int v_iDiskStyle, int v_iDiskPart, long long *v_lTotalSpace, long long *v_lFreeSpace, long long *v_lAvailSpace )
{
	char szDiskPart[128] = {0};
	struct statfs buf = {0};
	
	ClearThreadDog(1);//防止文件太多时计算时间过长而来不及清看门狗
	
	switch( v_iDiskStyle )
	{
		case SDISK:
		{
			if( MOUNT(&g_objUsbSta.SDisk[v_iDiskPart]) )
			{
				statfs(SPRINTF(szDiskPart, "/mnt/SDisk/part%d", v_iDiskPart+1), &buf);
				goto DISK_SIZE;
			}
		}
			break;
		case HDISK:
		{
			if( MOUNT(&g_objUsbSta.HDisk[v_iDiskPart]) )
			{
				statfs(SPRINTF(szDiskPart, "/mnt/HDisk/part%d", v_iDiskPart+1), &buf);
				goto DISK_SIZE;
			}
		}
			break;
		case UDISK:
		{
			if( MOUNT(&g_objUsbSta.UDisk[v_iDiskPart]) )
			{
				statfs(SPRINTF(szDiskPart, "/mnt/UDisk/part%d", v_iDiskPart+1), &buf);
				goto DISK_SIZE;
			}
		}
			break;
		default:
			break;
	}
	
	if(v_lTotalSpace)	
		*v_lTotalSpace = -1;
	if(v_lFreeSpace)	  
		*v_lFreeSpace  = -1;
	if(v_lAvailSpace)	
		*v_lAvailSpace = -1;
	return;
	
DISK_SIZE:
	if(v_lTotalSpace)	
		*v_lTotalSpace = (long long)buf.f_bsize * buf.f_blocks;
	if(v_lFreeSpace)	  
		*v_lFreeSpace  = (long long)buf.f_bsize * buf.f_bfree;
	if(v_lAvailSpace)	
		*v_lAvailSpace = (long long)buf.f_bsize * buf.f_bavail;
}

double GetDiskUsage( int v_iDiskStyle, int v_iDiskPart, char v_cUsageType )
{
	double dDiskUsage;
	int iTotalInode, iUseInode;
	long long lTotalSpace, lAvailSpace;
	
	GetDiskSpace(v_iDiskStyle, v_iDiskPart, &lTotalSpace, NULL, &lAvailSpace);

	//计算磁盘节点使用率
	if( v_cUsageType=='I' )
	{
		switch( v_iDiskStyle )
		{
			case SDISK:
			{
				if( v_iDiskPart==PART1 )
				{
					iTotalInode = lTotalSpace/1048576;
					iUseInode = GetFileNum("/mnt/SDisk/part1/data/", "*", true);
				}
				else if( v_iDiskPart==PART2 )
				{
					iTotalInode = lTotalSpace/4096;
					iUseInode = GetFileNum("/mnt/SDisk/part2/data/", "*", true);
				}
			}
				break;
			case HDISK:
			{
				if( v_iDiskPart==PART1 )
				{
					iTotalInode = lTotalSpace/1048576;
					iUseInode = GetFileNum("/mnt/HDisk/part1/data/", "*", true);
				}
				else if( v_iDiskPart==PART2 )
				{
					iTotalInode = lTotalSpace/4096;
					iUseInode = GetFileNum("/mnt/HDisk/part2/data/", "*", true);
				}
			}
				break;
			case UDISK:
			{
				if( v_iDiskPart==PART1 )
				{
					iTotalInode = lTotalSpace/4096;
					iUseInode = GetFileNum("/mnt/UDisk/part1/data/", "*", true);
				}
				else if( v_iDiskPart==PART2 )
				{
					iTotalInode = lTotalSpace/4096;
					iUseInode = GetFileNum("/mnt/UDisk/part2/data/", "*", true);
				}
			}
				break;
			default:
				break;
		}
		
		dDiskUsage = iUseInode/(double)iTotalInode;
	}
	//计算磁盘容量使用率
	else if( v_cUsageType=='S' )
	{
		dDiskUsage = 1 - lAvailSpace/(double)lTotalSpace;
	}
	
	return dDiskUsage;
}

void FlushAVList(char *v_szAVListFile, FILE_INDEX *v_pAVFileIndex, char *v_szDelFileTime, int v_iFlag)
{
	int iErrTime[6];
	
	pthread_mutex_lock( &g_AVListMutex );
	
	int i;
	int iAVFileNum;
	char szDiskFileTime[9] = {0};
	FILE *pAVListFile = NULL;
	FILE_INDEX objAVFileIndex[48];
	
	if( !v_szAVListFile )
		goto FLUSH_DONE;

	pAVListFile = fopen(v_szAVListFile, "r+b");
	if( !pAVListFile )   
	{
		GetCurDateTime( iErrTime );		
		PRTMSG(MSG_DBG, "读方式打开列表文件出错时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);
			  
		PRTMSG(MSG_ERR, "open av list:%s fail\n", v_szAVListFile);
		goto FLUSH_DONE;
	}
	
	if( v_iFlag == 1 ) //添加一个索引项
	{
		fseek(pAVListFile, 0, SEEK_END);
		
		if(fwrite(v_pAVFileIndex, sizeof(FILE_INDEX), 1, pAVListFile) != 1)
		{
			GetCurDateTime( iErrTime );		
			PRTMSG(MSG_DBG, "添加索引项出错时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);
			  
			PRTMSG(MSG_ERR, "add av list fail\n");
			goto FLUSH_DONE;
		}
	}
	else if( v_iFlag == -1 ) //删除一个索引项
	{
		if( (iAVFileNum = fread(objAVFileIndex, sizeof(FILE_INDEX), 48, pAVListFile)) )
		{
			fclose(pAVListFile);
			pAVListFile = NULL;

			pAVListFile = fopen(v_szAVListFile, "w+b");
			if( !pAVListFile )   
			{
				GetCurDateTime( iErrTime );		
				PRTMSG(MSG_DBG, "写方式打开列表文件出错时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);
			
				PRTMSG(MSG_ERR, "clear av list fail\n");
				goto FLUSH_DONE;
			}

			for( i = 0; i < iAVFileNum; i++ )
			{
				sprintf(szDiskFileTime, "%s-%s-%s", objAVFileIndex[i].StartTime[0], objAVFileIndex[i].StartTime[1], objAVFileIndex[i].StartTime[2]);
				if( strcmp(szDiskFileTime, v_szDelFileTime) )
				{
					if(fwrite(&objAVFileIndex[i], sizeof(FILE_INDEX), 1, pAVListFile) != 1)
					{
						GetCurDateTime( iErrTime );		
						PRTMSG(MSG_DBG, "删除索引项出错时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);
			  
						PRTMSG(MSG_ERR, "renew av list fail\n");
//						goto FLUSH_DONE;
					}
				}
			}
		}
	}
	
FLUSH_DONE:	
	
	if( pAVListFile )
	{
		fclose(pAVListFile);
		pAVListFile = NULL;
	}
	
	pthread_mutex_unlock( &g_AVListMutex );
}

char g_szTipBuf[1024] = {0};
void G_TmDiaoduTip(void *arg, int len)
{
	DataPush(g_szTipBuf, strlen(g_szTipBuf), DEV_DVR, DEV_DIAODU, LV2);
}

int ClearDisk(char *v_szDiskStyle, char *v_szDiskPart)
{
	char szTempBuf[1024] = {0};
	
	int i, j;
	int iDayNum = 0, iTimeNum = 0;
	char szCommand[100] = {0};
	char szStyle[3][10] = {"264", "726", "index"};
	char szChnPath[100] = {0}, szDayPath[100] = {0}, szTimePath[100] = {0}, szListPath[100] = {0};
	LINUX_HANDLE hDayFind, hTimeFind, hListFind;
	LINUX_FIND_DATA objDayFind, objTimeFind, objListFind;

	PRTMSG(MSG_NOR, "Clear %s %s Data\n", v_szDiskStyle, v_szDiskPart);

	// 通知调度屏提示正在删除数据
	char buf[200] = {0};							
	buf[0] = 0x01;
	buf[1] = 0x72;
	buf[2] = 0x01;
	sprintf(buf+3, "%s %s使用率已超过90%%，删除旧文件...", v_szDiskStyle, v_szDiskPart);
	
	memset(g_szTipBuf, 0, sizeof(g_szTipBuf));
	memcpy(g_szTipBuf, buf, strlen(buf));
	_SetTimer(&g_objTimerMng, TIMER_DIAODU_TIP, 3000, G_TmDiaoduTip);

	if(strcmp(v_szDiskPart, "part1") == 0)//录像分区:删除最早一天中的最早一个时段
	{
		for(i = 0; i < 4; i++)
		{
			sprintf(szChnPath, "/mnt/%s/part1/data/chn%d/", v_szDiskStyle, i+1);
			iDayNum = GetFileNum(szChnPath, "??????????", false);// 计算该通道的天数			

			if( iDayNum > 0 ) 
			{
				hDayFind = FindFirstFile(szChnPath, "??????????", &objDayFind, false);// 查找该通道的最早一天
				if( hDayFind.bValid )
				{
					do
					{
						sprintf(szDayPath, "/mnt/%s/part1/data/chn%d/%s/", v_szDiskStyle, i+1, objDayFind.szFileName); 
						iTimeNum = GetFileNum(szDayPath, "????????.264", false);// 计算当日的录像时段数
						if( iTimeNum == 0 )// 若当天无录像文件则删除当天目录
						{
							{
							//	chmod(szDayPath, S_IRWXU|S_IRWXG|S_IRWXO);
							//	if( SYSTEM("rm -r -f %s", szDayPath) ) // system无法判断删除是否成功
							//	{
							//		PRTMSG(MSG_ERR, "Del Dir:%s Fail\n", szDayPath);
							//		msleep(100);
							//	}
							//	else
							//	{
							//		PRTMSG(MSG_NOR, "Del Dir:%s Succ\n", szDayPath);
							//		break;
							//	}
							}
	 						{
	 							// 删除当天音频及索引文件
	 							hTimeFind = FindFirstFile( szDayPath, "????????.*", &objTimeFind, false );
	 							if( hTimeFind.bValid )
	 							{
	 								do
	 								{
	 									sprintf(szTimePath, "/mnt/%s/part1/data/chn%d/%s/%s", v_szDiskStyle, i+1, objDayFind.szFileName, objTimeFind.szFileName); 
	 									chmod(szTimePath, S_IRWXU|S_IRWXG|S_IRWXO);
	 									if( remove(szTimePath) )
	 									{
	 										PRTMSG(MSG_ERR, "Del File:%s Fail\n", szTimePath);
	 										msleep(100);
	 									}
	 								}
	 								while( FindNextFile(&hTimeFind, &objTimeFind, false) );
	 							}
	 
	 							// 删除当天列表文件
	 							hListFind = FindFirstFile( szDayPath, "??????????.list", &objListFind, false );
	 							if( hListFind.bValid )
	 							{
	 								sprintf(szListPath, "/mnt/%s/part1/data/chn%d/%s/%s", v_szDiskStyle, i+1, objDayFind.szFileName, objListFind.szFileName);
	 								chmod(szListPath, S_IRWXU|S_IRWXG|S_IRWXO);
	 								if( remove(szListPath) )
	 								{
	 									PRTMSG(MSG_ERR, "Del File:%s Fail\n", szListPath);
	 									msleep(100);
	 								}
	 							}
	 							
	 							// 删除当天目录
	 							chmod(szDayPath, S_IRWXU|S_IRWXG|S_IRWXO);
	 							if( remove(szDayPath) )
	 							{
	 								PRTMSG(MSG_ERR, "Del Dir:%s Fail\n", szDayPath);
	 								msleep(100);
	 							}
	 							else
	 							{
	 								PRTMSG(MSG_NOR, "Del Dir:%s Succ\n", szDayPath);
	 								break;
	 							}
	 						}
						}
						else if( iTimeNum >= 1 )
						{
							hTimeFind = FindFirstFile(szDayPath, "????????.264", &objTimeFind, false);// 查找当日的最早一个时段		
							if( hTimeFind.bValid )
							{
								do
								{
									for(j = 0; j < 3; j++)
									{
										strncpy(szTempBuf,objTimeFind.szFileName,8);
										sprintf(szTimePath, "/mnt/%s/part1/data/chn%d/%s/%s.%s", v_szDiskStyle, i+1, objDayFind.szFileName, szTempBuf, szStyle[j]); 
									
										if( access(szTimePath, F_OK) == 0 )
										{
											// 删除最早一个时段的音频、视频及索引文件
											chmod(szTimePath, S_IRWXU|S_IRWXG|S_IRWXO);
		//									if( SYSTEM("rm -r -f %s", szTimePath) )// system无法判断删除是否成功
											if( remove(szTimePath) )
											{
												PRTMSG(MSG_ERR, "Del File:%s Fail\n", szTimePath);
												
												if(j == 0) // 若删除视频文件失败则查找下一时段
												{
													msleep(100);
													break;
												}
											}
											else
											{
												PRTMSG(MSG_NOR, "Del File:%s Succ\n", szTimePath);
												
												if(j == 0) // 若删除视频文件成功则更新文件列表
												{
													hListFind = FindFirstFile(szDayPath, "??????????.list", &objListFind, false);// 查找当日的文件列表
													if( hListFind.bValid )
													{
														sprintf(szListPath, "/mnt/%s/part1/data/chn%d/%s/%s", v_szDiskStyle, i+1, objDayFind.szFileName, objListFind.szFileName);
														chmod(szListPath, S_IRWXU|S_IRWXG|S_IRWXO);
														FlushAVList(szListPath, NULL, strncpy(szTempBuf,objTimeFind.szFileName,8), -1);
													}
												}
											}
										}
									}

									if(j == 3)	goto NEXT_CHN;
								
								}while(FindNextFile(&hTimeFind, &objTimeFind, false));
								
								memset(&hTimeFind, 0, sizeof(LINUX_HANDLE));
							}
						}
					}while(FindNextFile(&hDayFind, &objDayFind, false));
					
					memset(&hDayFind, 0, sizeof(LINUX_HANDLE));
				}
			}

NEXT_CHN:
			memset(&hTimeFind, 0, sizeof(LINUX_HANDLE));
			memset(&hDayFind, 0, sizeof(LINUX_HANDLE));
		}
	}
	else if(strcmp(v_szDiskPart, "part2") == 0)//照片分区:删除最早一天
	{
		sprintf(szChnPath, "/mnt/%s/part2/data/", v_szDiskStyle);
		iDayNum = GetFileNum(szChnPath, "??????", false);// 计算拍照天数

		if( iDayNum > 0 ) 
		{
			hDayFind = FindFirstFile(szChnPath, "??????", &objDayFind, false);// 查找最早一天
			if( hDayFind.bValid )
			{
				do
				{
					sprintf(szDayPath, "/mnt/%s/part2/data/%s/", v_szDiskStyle, objDayFind.szFileName); 			
					chmod(szDayPath, S_IRWXU|S_IRWXG|S_IRWXO);
					
					// 删除该子目录下所有文件
					{
	// 				for(;;)
	// 				{
	// 					hTimeFind = FindFirstFile( szDayPath, "*.jpg", &objTimeFind, false );
	// 					if( hTimeFind.bValid )
	// 					{											
	// 						memset(szTimePath, 0, sizeof(szTimePath));
	// 						sprintf(szTimePath, "/mnt/%s/part2/data/%s/%s", v_szDiskStyle, objDayFind.szFileName, objTimeFind.szFileName); 
	// 						chmod(szTimePath, S_IRWXU|S_IRWXG|S_IRWXO);
	// 						
	// 						if( !remove(szTimePath) )
	// 						{
	// 							PRTMSG(MSG_ERR, "remove File:%s succ\n", szTimePath);
	// 						}
	// 						else
	// 						{
	// 							PRTMSG(MSG_NOR, "remove File:%s fail\n", szTimePath);
	// 							
	// 							// unlink删除失败，用rm强删
	// 							if( !SYSTEM("rm -f %s", szTimePath) )
	// 							{
	// 								PRTMSG(MSG_ERR, "rm File:%s succ\n", szTimePath);
	// 							}
	// 							else
	// 							{
	// 								PRTMSG(MSG_NOR, "rm File:%s fail\n", szTimePath);
	// 								return -1;
	// 							}
	// 						}
	// 					}
	// 					else
	// 					{
	// 						break;
	// 					}
	// 				}
					}
									
					// 删除当天目录
					//if( remove(szDayPath) )
					if( SYSTEM("rm -f -r %s", szDayPath) )
					{
						PRTMSG(MSG_ERR, "Del Dir:%s Fail\n", szDayPath);
						msleep(100);
					}
					else
					{
						PRTMSG(MSG_NOR, "Del Dir:%s Succ\n", szDayPath);
						break;
					}
				}while(FindNextFile(&hDayFind, &objDayFind, false));
				
				memset(&hDayFind, 0, sizeof(LINUX_HANDLE));
			}
		}
	}
	
	return 0;
}

int FileSearch(FILE_INDEX **v_ppAVFileIndex, FILE_SEARCH v_objFileSearch)
{
	int i, j = 0;
	FILE *pListFile;
	char szListFile[4][255];
	char szFileStartTime[255];
	char szFileEndTime[255];
	int iFileNum = 0;
	FILE_INDEX objAVFileIndex;

	for(i = 0; i < 4; i++)
	{
		if( v_objFileSearch.SearchStart[i] )
		{
			sprintf(szListFile[i], "/mnt/%s/part1/data/chn%d/%s-%s-%s/%s-%s-%s.list",
							v_objFileSearch.DiskType, i+1,
							v_objFileSearch.CreatDate[0], v_objFileSearch.CreatDate[1], v_objFileSearch.CreatDate[2],
							v_objFileSearch.CreatDate[0], v_objFileSearch.CreatDate[1], v_objFileSearch.CreatDate[2]);

			if((pListFile = fopen(szListFile[i], "r")) == NULL)
				continue;
			
			while(fread(&objAVFileIndex, sizeof(FILE_INDEX), 1, pListFile))
			{
				sprintf( szFileStartTime, "%s:%s:%s", objAVFileIndex.StartTime[0], objAVFileIndex.StartTime[1], objAVFileIndex.StartTime[2] ); 
				sprintf( szFileEndTime, "%s:%s:%s", objAVFileIndex.EndTime[0], objAVFileIndex.EndTime[1], objAVFileIndex.EndTime[2] );
				 	
				if( strlen(objAVFileIndex.VPath) > 0 
					&&( (strcmp(szFileStartTime, v_objFileSearch.StartTime) >= 0 && strcmp(szFileStartTime, v_objFileSearch.EndTime) < 0)
			   		||(strcmp(szFileEndTime, v_objFileSearch.StartTime) > 0 && strcmp(szFileEndTime, v_objFileSearch.EndTime) <= 0) 
			   		||(strcmp(szFileStartTime, v_objFileSearch.StartTime) < 0 && strcmp(szFileEndTime, v_objFileSearch.EndTime) > 0)))
				{
			  	iFileNum++;
				}
			}
			
			fclose(pListFile);
			pListFile = NULL;
		}
	}

	*v_ppAVFileIndex = (FILE_INDEX *)malloc(iFileNum*sizeof(FILE_INDEX));

	if(iFileNum > 0)
	{
		for(i = 0; i < 4; i++)
		{
			if( v_objFileSearch.SearchStart[i] )
			{
				if((pListFile = fopen(szListFile[i], "r")) == NULL)
					continue;
				
				while(fread(&objAVFileIndex, sizeof(FILE_INDEX), 1, pListFile))
				{
					sprintf(szFileStartTime, "%s:%s:%s", objAVFileIndex.StartTime[0], objAVFileIndex.StartTime[1], objAVFileIndex.StartTime[2]); 
					sprintf(szFileEndTime, "%s:%s:%s", objAVFileIndex.EndTime[0], objAVFileIndex.EndTime[1], objAVFileIndex.EndTime[2]);
					 	
					if(	strlen(objAVFileIndex.VPath) > 0 
						&&( (strcmp(szFileStartTime, v_objFileSearch.StartTime) >= 0 && strcmp(szFileStartTime, v_objFileSearch.EndTime) < 0)
				   		||(strcmp(szFileEndTime, v_objFileSearch.StartTime) > 0 && strcmp(szFileEndTime, v_objFileSearch.EndTime) <= 0) 
				   		||(strcmp(szFileStartTime, v_objFileSearch.StartTime) < 0 && strcmp(szFileEndTime, v_objFileSearch.EndTime) > 0)))
					{
						(*v_ppAVFileIndex)[j] = objAVFileIndex;
						j++;
					}
				}
				
				fclose(pListFile);
				pListFile = NULL;
			}
		}
	}
	
	return iFileNum;
}

int DiskCheck(char *v_szDiskStyle)
{
	char szTempBuf[1024];
	
	int i;
	int iDiskNum, iPartNum;
	char szDiskPath[255];
	struct dirent **ppNameList;
								
	if( strcmp("SDisk", v_szDiskStyle)==0 )
	{
#if VEHICLE_TYPE == VEHICLE_M || VEHICLE_TYPE == VEHICLE_M2
		if(access(DEV_SDISK, F_OK) == 0)
		{
			scandir(DEV_SDISK, &ppNameList, 0, alphasort);		
			sprintf(szDiskPath, "/dev/scsi/%s/bus0/target0/lun0/", ppNameList[10]->d_name);	
		}
		else
			return -1;
#endif

#if VEHICLE_TYPE == VEHICLE_V8
		sprintf(szDiskPath, "/dev/mmc/blk0/");	
#endif
	}
	else if( strcmp("HDisk", v_szDiskStyle)==0 )
	{
#if VEHICLE_TYPE == VEHICLE_M || VEHICLE_TYPE == VEHICLE_M2		
		if(access(DEV_HDISK, F_OK) == 0)
		{
			scandir(DEV_HDISK, &ppNameList, 0, alphasort);		
			sprintf(szDiskPath, "/dev/scsi/%s/bus0/target0/lun0/", ppNameList[10]->d_name);	
		}
		else
			return -1;
#endif
	}
	
	//磁盘数
	iDiskNum = GetFileNum(szDiskPath, "disc", false);
	if( iDiskNum!=1 )
	{
		PRTMSG(MSG_ERR, "Disk Num:%d\n", iDiskNum);
		return -1;
	}
						
	//分区数
	iPartNum = GetFileNum(szDiskPath, "part?", false);
	if( iPartNum!=2 )
	{
		PRTMSG(MSG_ERR, "Disk Part Num:%d\n", iPartNum);
		return -2;
	}
		
	return 0;
}

int DiskFormat(char *v_szDiskStyle)
{
	int i;
	int iPartNum;
	char szDiskPath[255];
	struct dirent **ppNameList;
								
	if( strcmp("SDisk", v_szDiskStyle)==0 )
	{
		if( DiskCheck("SDisk")==-1 )
		{
			PRTMSG(MSG_ERR, "Disk Check Failed\n");
			goto SD_ERROR;
		}
		
		//卸载原分区
		for(i=0; i<PART_NUM_SDISK; i++)
		{
			g_objUsbUmount.SDisk[i] = TRUE;
			if( WaitForEvent(UMOUNT, &g_objUsbSta.SDisk[i], 10000) )
			{
				PRTMSG(MSG_ERR, "Umount Disk Part Failed\n");
				goto SD_ERROR;
			}
		}
										
#if VEHICLE_TYPE == VEHICLE_M || VEHICLE_TYPE == VEHICLE_M2
		scandir(DEV_SDISK, &ppNameList, 0, alphasort);		
		sprintf(szDiskPath, "/dev/scsi/%s/bus0/target0/lun0/", ppNameList[10]->d_name);	
#endif

#if VEHICLE_TYPE == VEHICLE_V8
		sprintf(szDiskPath, "/dev/mmc/blk0/");	
#endif
		//删除原分区
		iPartNum = GetFileNum(szDiskPath, "part?", false);
		for(i=0; i<iPartNum; i++)
		{
			if( CtrlDiskPart(szDiskPath, i+1, 0, NULL, 0, -1)==false )
			{
				PRTMSG(MSG_ERR, "Del Disk Part Failed\n");
				goto SD_ERROR;
			}
		}

		//创建新分区
		if( CtrlDiskPart(szDiskPath, 2, 128, NULL, 0, 1)==false )
		{
			PRTMSG(MSG_ERR, "Add Photo Part Failed\n");
			goto SD_ERROR;
		}
											
		if( CtrlDiskPart(szDiskPath, 1, 0, NULL, 0, 1)==false )
		{
			PRTMSG(MSG_ERR, "Add Video Part Failed\n");
			goto SD_ERROR;
		}
		
		//创建文件系统
		if( CtrlDiskPart(szDiskPath, 2, 0, "ext3", 4096, 0)==false )
		{
			PRTMSG(MSG_ERR, "Format Photo Part Failed\n");
			goto SD_ERROR;
		}
											
		if( CtrlDiskPart(szDiskPath, 1, 0, "ext3", 1048576, 0)==false )
		{
			PRTMSG(MSG_ERR, "Format Video Part Failed\n");
			goto SD_ERROR;
		}
										
		for(i=0; i<PART_NUM_SDISK; i++)
		{
			g_objUsbUmount.SDisk[i] = FALSE;
		}
										
		return 0;

SD_ERROR:
		for(i=0; i<PART_NUM_SDISK; i++)
		{
			g_objUsbUmount.SDisk[i] = FALSE;
		}
									
		return -1;
	}
	else if( strcmp("HDisk", v_szDiskStyle)==0 )
	{
#if VEHICLE_TYPE == VEHICLE_M
		if( DiskCheck("HDisk")==-1 )
		{
			PRTMSG(MSG_ERR, "Disk Check Failed\n");
			goto HD_ERROR;
		}
		
		//卸载原分区
		for(i=0; i<PART_NUM_HDISK; i++)
		{
			g_objUsbUmount.HDisk[i] = TRUE;
			if( WaitForEvent(UMOUNT, &g_objUsbSta.HDisk[i], 10000) )
			{
				PRTMSG(MSG_ERR, "Umount Disk Part Failed\n");
				goto HD_ERROR;
			}
		}
										
		//删除原分区
		scandir(DEV_HDISK, &ppNameList, 0, alphasort);		
		sprintf(szDiskPath, "/dev/scsi/%s/bus0/target0/lun0/", ppNameList[10]->d_name);	
		iPartNum = GetFileNum(szDiskPath, "part?", false);
		for(i=0; i<iPartNum; i++)
		{
			if( CtrlDiskPart(szDiskPath, i+1, 0, NULL, 0, -1)==false )
			{
				PRTMSG(MSG_ERR, "Del Disk Part Failed\n");
				goto HD_ERROR;
			}
		}
		
		//创建新分区
		if( CtrlDiskPart(szDiskPath, 2, 128, NULL, 0, 1)==false )
		{
			PRTMSG(MSG_ERR, "Add Photo Part Failed\n");
			goto HD_ERROR;
		}
											
		if( CtrlDiskPart(szDiskPath, 1, 0, NULL, 0, 1)==false )
		{
			PRTMSG(MSG_ERR, "Add Video Part Failed\n");
			goto HD_ERROR;
		}
		
		//创建文件系统
		if( CtrlDiskPart(szDiskPath, 2, 0, "ext3", 4096, 0)==false )
		{
			PRTMSG(MSG_ERR, "Format Photo Part Failed\n");
			goto HD_ERROR;
		}
											
		if( CtrlDiskPart(szDiskPath, 1, 0, "ext3", 1048576, 0)==false )
		{
			PRTMSG(MSG_ERR, "Format Video Part Failed\n");
			goto HD_ERROR;
		}
										
		for(i=0; i<PART_NUM_HDISK; i++)
		{
			g_objUsbUmount.HDisk[i] = FALSE;
		}
										
		return 0;

HD_ERROR:
		for(i=0; i<PART_NUM_HDISK; i++)
		{
			g_objUsbUmount.HDisk[i] = FALSE;
		}
#endif
		return -1;
	}
}

#if VEHICLE_TYPE == VEHICLE_V8
int InitSd()
{
	int	iRet = 0;
	int ifd;
	unsigned long arg = 0;
	
	if( (ifd = open("/dev/misc/sd_host", O_RDWR))==-1 )
	{
		PRTMSG(MSG_ERR, "init sd failed\n");
		iRet = -1;
	}	
	else if( ioctl(ifd, INITIALISE, arg)==0 )
	{
		PRTMSG(MSG_DBG, "init sd succ\n");
		close(ifd);
	}
	
	close(ifd);
	return iRet;
}

int ResetUsb()
{
	int	iRet = 0;
	int ifd;
	unsigned long arg = 0;
	
	if( (ifd = open("/dev/misc/usbstatue", O_RDWR))==-1 )
	{
		PRTMSG(MSG_ERR, "reset usb failed\n");
		iRet = -1;
	}	
	else if( ioctl(ifd, USB_GET_STATUE, arg)==1 && ioctl(ifd, USB_SET_RESET, arg)==0 )
	{
		PRTMSG(MSG_DBG, "reset usb succ\n");
		close(ifd);
		RMMOD(KO_HI3511_USB, 100);
		INSMOD(KO_HI3511_USB, 100);
	}
	
	close(ifd);
	return iRet;
}
#endif

void *G_DiskManage(void *arg)
{
	int iErrTime[6];
	
	char szTempBuf[1024];
	
	int iRet;
	int i,j;
	
	DWORD dwSDiskUconnectCnt[PART_NUM_SDISK] = {0}, dwHDiskUconnectCnt[PART_NUM_HDISK] = {0}, dwUDiskUconnectCnt[PART_NUM_UDISK] = {0};
	DWORD dwSDiskUmountCnt[PART_NUM_SDISK] = {0}, dwHDiskUmountCnt[PART_NUM_HDISK] = {0}, dwUDiskUmountCnt[PART_NUM_UDISK] = {0};
	
	DWORD dwSDiskMountCnt[PART_NUM_SDISK] = {0}, dwHDiskMountCnt[PART_NUM_HDISK] = {0}, dwUDiskMountCnt[PART_NUM_UDISK] = {0};
	DWORD dwSDiskDetectCnt[PART_NUM_SDISK] = {0}, dwHDiskDetectCnt[PART_NUM_HDISK] = {0}, dwUDiskDetectCnt[PART_NUM_UDISK] = {0};
	
	BOOL bSDiskExist[PART_NUM_SDISK] = {FALSE}, bHDiskExist[PART_NUM_HDISK] = {FALSE}, bUDiskExist[PART_NUM_UDISK] = {FALSE};
	BOOL bSDiskSpaceValid[PART_NUM_SDISK] = {FALSE}, bHDiskSpaceValid[PART_NUM_HDISK] = {FALSE}, bUDiskSpaceValid[PART_NUM_UDISK] = {FALSE};
	BOOL bSDiskInodeValid[PART_NUM_SDISK] = {FALSE}, bHDiskInodeValid[PART_NUM_HDISK] = {FALSE}, bUDiskInodeValid[PART_NUM_UDISK] = {FALSE};
	BOOL bSDiskMount[PART_NUM_SDISK] = {TRUE}, bHDiskMount[PART_NUM_HDISK] = {TRUE}, bUDiskMount[PART_NUM_UDISK] = {TRUE};
	
	FILE *pSDiskFile[PART_NUM_SDISK] = {NULL}, *pHDiskFile[PART_NUM_HDISK] = {NULL}, *pUDiskFile[PART_NUM_UDISK] = {NULL};
	struct dirent **ppSDiskList, **ppHDiskList, **ppUDiskList;
	struct statfs objSDiskBuf, objHDiskBuf, objUDiskBuf;
	
	double dSDiskUsage[PART_NUM_SDISK] = {0}, dHDiskUsage[PART_NUM_HDISK] = {0}, dUDiskUsage[PART_NUM_UDISK] = {0};
	double dSDiskInodeUsage[PART_NUM_SDISK] = {0}, dSDiskSpaceUsage[PART_NUM_SDISK] = {0};
	double dHDiskInodeUsage[PART_NUM_HDISK] = {0}, dHDiskSpaceUsage[PART_NUM_HDISK] = {0};
	double dUDiskInodeUsage[PART_NUM_UDISK] = {0}, dUDiskSpaceUsage[PART_NUM_UDISK] = {0};
	
	if( strcmp("SDisk", g_objMvrCfg.AVRecord.DiskType)==0 )
	{
		iRet = DiskCheck("SDisk");
		if( iRet==-1 )
		{
			ShowDiaodu("请插入SD卡");
		}
		else if( iRet==-2 )
		{
			ShowDiaodu("SD卡不可用,请重新格式化");
		}
	}
	else if( strcmp("HDisk", g_objMvrCfg.AVRecord.DiskType)==0 )
	{
		iRet = DiskCheck("HDisk");
		if( iRet==-1 )
		{
			ShowDiaodu("请插入硬盘");
		}
		else if( iRet==-2 )
		{
			ShowDiaodu("硬盘不可用,请重新格式化");
		}
	}
	
	while(1)
	{		
		RenewMemInfoHalfMin(1);
		ClearThreadDog(1);

#if VEHICLE_TYPE == VEHICLE_M || VEHICLE_TYPE == VEHICLE_M2
		/*----------SD卡状态检测----------*/
		for(i=0; i<PART_NUM_SDISK; i++)
		{
			switch(g_objUsbSta.SDisk[i])
			{
				case UCON_UMOU_UVAL:/*未连接-未挂载-未可用*/
				{
					if(access(BUS_SDISK, F_OK) == 0)
					{
						scandir(DEV_SDISK, &ppSDiskList, 0, alphasort);
							
						if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/disc", ppSDiskList[10]->d_name), F_OK) == 0)
						{
							g_objUsbSta.SDisk[i] = CON_UMOU_UVAL;
						}
					}
				}
					break;
				case CON_UMOU_UVAL:/*已连接-未挂载-未可用*/
				{
					if( g_objUsbUmount.SDisk[i] )
						break;

					if(access(BUS_SDISK, F_OK) == 0)
					{
						if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/part%d", ppSDiskList[10]->d_name, i+1), F_OK) == 0 
						 &&SYSTEM("mount -t ext3 -o rw /dev/scsi/%s/bus0/target0/lun0/part%d /mnt/SDisk/part%d", ppSDiskList[10]->d_name, i+1, i+1) == 0 
						 &&access(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/flash", i+1), F_OK) != 0)
						{
							if(access(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data", i+1), F_OK) == 0)
							{
								chmod(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);// 将目录的访问权限设置为所有人可读、可写、可执行
							}
							else
							{   
								if(mkdir(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data", i+1), 0777) == 0)   
								{
									chmod(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);
								}
							}
									
							if((pSDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data/disk", i+1), "w+b")) == NULL)
							{
								dwSDiskMountCnt[i] = 0;
								g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;	
										
								SYSTEM("umount /mnt/SDisk/part%d", i+1);
#if VEHICLE_TYPE == VEHICLE_M									
								IOSET( IOS_HUBPOW, IO_HUBPOW_OFF, 1000 );//重启HUB/SD电源
								IOSET( IOS_HUBPOW, IO_HUBPOW_ON );
#endif
							}
							else
							{
								fclose(pSDiskFile[i]);
								pSDiskFile[i] = NULL;
								bSDiskExist[i] = TRUE;								
										
								memset(&objSDiskBuf, 0, sizeof(objSDiskBuf));
								statfs(SPRINTF(szTempBuf, "/mnt/SDisk/part%d", i+1), &objSDiskBuf);
										
								dSDiskUsage[i] = 1 - objSDiskBuf.f_bavail/(double)objSDiskBuf.f_blocks;
								PRTMSG(MSG_NOR, "SDisk part%d mount succ, usage %.3f%%\n", i+1, 100*dSDiskUsage[i]);
								if( dSDiskUsage[i]>=MAX_SPACE_USAGE_SDISK )
									g_objUsbSta.SDisk[i] = CON_MOU_UVAL;
								else
									g_objUsbSta.SDisk[i] = CON_MOU_VAL;
								
								bool bMount = true;
								for(int c=0; c<PART_NUM_SDISK; c++)
								{
									if( MOUNT(&g_objUsbSta.SDisk[c])==false )
									{
										bMount = false;
										break;
									}
								}
								if( bMount )
									ShowDiaodu("检测到SD卡插入");
							}
						}
						else 
						{
							switch(++dwSDiskMountCnt[i])
							{
								case 5:
								case 10:
								{
	//								SYSTEM("umount /mnt/SDisk/part%d", i+1);
	//								
//									IOSET( IOS_HUBPOW, IO_HUBPOW_OFF, 1000 );//重启HUB/SD电源
// 									IOSET( IOS_HUBPOW, IO_HUBPOW_ON );

	//								 g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;
								}
									 break;
								 case 15:
								 {
									 if( !bSDiskExist[i] )
									 {
										 PRTMSG(MSG_NOR, "SDisk part%d can not dectect\n", i+1);
									 }
									 else
									 {
										 PRTMSG(MSG_NOR, "SDisk part%d can not work\n", i+1);
										//StopVo();
										//SYSTEM("reboot");
									 }
								 }
									 break;
								default:
									break;
							}
						}
					}
					else
					{
						dwSDiskMountCnt[i] = 0;
						g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;	
					}
				}
					break;
				case CON_MOU_UVAL:/*已连接-已挂载-未可用*/
				{
					if( g_objUsbUmount.SDisk[i] )
					{
						if(SYSTEM("umount /mnt/SDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "SDisk part%d umount succ\n", i+1);
						}
							
						dwSDiskMountCnt[i] = 0;
						g_objUsbSta.SDisk[i] = CON_UMOU_UVAL;	
						_KillTimer(&g_objTimerMng, TIMER_DIAODU_TIP);
						break;
					}
					
					if(++dwSDiskDetectCnt[i] == VALID_DECCNT_SDISK)
					{
						dwSDiskDetectCnt[i] = 0;
						if((pSDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data/disk", i+1), "r+b")) != NULL)
						{
							fclose(pSDiskFile[i]);
							pSDiskFile[i] = NULL;
								
							ClearDisk("SDisk", SPRINTF(szTempBuf, "part%d", i+1));/*清理SD卡数据*/
								
							memset(&objSDiskBuf, 0, sizeof(objSDiskBuf));
							statfs(SPRINTF(szTempBuf, "/mnt/SDisk/part%d", i+1), &objSDiskBuf);
							
							dSDiskUsage[i] = 1 - objSDiskBuf.f_bavail/(double)objSDiskBuf.f_blocks;
							if( dSDiskUsage[i]<=MIN_SPACE_USAGE_SDISK )
							{
								PRTMSG(MSG_NOR, "SDisk part%d usage %.3f%%\n", i+1, 100*dSDiskUsage[i]);
								g_objUsbSta.SDisk[i] = CON_MOU_VAL;
								_KillTimer(&g_objTimerMng, TIMER_DIAODU_TIP);

								ShowDiaodu("删除完成，SDisk part%d使用率%.3f%%", i+1, 100*dSDiskUsage[i]);
							}
						}
						else
						{
							dwSDiskMountCnt[i] = 0;
							g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;	
							_KillTimer(&g_objTimerMng, TIMER_DIAODU_TIP);
							
							if(SYSTEM("umount /mnt/SDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "SDisk part%d umount succ\n", i+1);
							}
							
							bool bUconnect = true;
							for(int c=0; c<PART_NUM_SDISK; c++)
							{
								if( UCONNECT(&g_objUsbSta.SDisk[c])==false )
								{
									bUconnect = false;
									break;
								}
							}
							if( bUconnect )
								ShowDiaodu("检测到SD卡拔出");
#if VEHICLE_TYPE == VEHICLE_M
							IOSET( IOS_HUBPOW, IO_HUBPOW_OFF, 1000 );//重启HUB/SD电源
							IOSET( IOS_HUBPOW, IO_HUBPOW_ON );
#endif
						}
					}
				}
					break;
				case CON_MOU_VAL:/*已连接-已挂载-已可用*/
				{
					if( g_objUsbUmount.SDisk[i] )
					{
						if(SYSTEM("umount /mnt/SDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "SDisk part%d umount succ\n", i+1);
						}
							
						dwSDiskMountCnt[i] = 0;
						g_objUsbSta.SDisk[i] = CON_UMOU_UVAL;	
						
						break;
					}
					
					if(++dwSDiskDetectCnt[i] == VALID_DECCNT_SDISK)
					{
						dwSDiskDetectCnt[i] = 0;
						if((pSDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data/disk", i+1), "r+b")) != NULL)
						{
							fclose(pSDiskFile[i]);
							pSDiskFile[i] = NULL;
								
							memset(&objSDiskBuf, 0, sizeof(objSDiskBuf));
							statfs(SPRINTF(szTempBuf, "/mnt/SDisk/part%d", i+1), &objSDiskBuf);
							dSDiskUsage[i] = 1 - objSDiskBuf.f_bavail/(double)objSDiskBuf.f_blocks;
							if(dSDiskUsage[i]>=MAX_SPACE_USAGE_SDISK)
							{
								PRTMSG(MSG_NOR, "SDisk part%d usage %.3f%%\n", i+1, 100*dSDiskUsage[i]);
								g_objUsbSta.SDisk[i] = CON_MOU_UVAL;
							}
						}
						else
						{
							dwSDiskMountCnt[i] = 0;
							g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;	
								
							if(SYSTEM("umount /mnt/SDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "SDisk part%d umount succ\n", i+1);
							}
							
							bool bUconnect = true;
							for(int c=0; c<PART_NUM_SDISK; c++)
							{
								if( UCONNECT(&g_objUsbSta.SDisk[c])==false )
								{
									bUconnect = false;
									break;
								}
							}
							if( bUconnect )
								ShowDiaodu("检测到SD卡拔出");
#if VEHICLE_TYPE == VEHICLE_M							
							IOSET( IOS_HUBPOW, IO_HUBPOW_OFF, 1000 );//重启HUB/SD电源
							IOSET( IOS_HUBPOW, IO_HUBPOW_ON );
#endif
						}
					}
				}
					break;
				default:
					break;
			}
		}

		/*----------硬盘状态检测----------*/
		for(i=0; i<PART_NUM_HDISK; i++)
		{
			switch(g_objUsbSta.HDisk[i])
			{
				case UCON_UMOU_UVAL:
				{
					if(access(BUS_HDISK, F_OK) == 0)
					{
						scandir(DEV_HDISK, &ppHDiskList, 0, alphasort);		

						if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/disc", ppHDiskList[10]->d_name), F_OK) == 0)
						{
							g_objUsbSta.HDisk[i] = CON_UMOU_UVAL;
						}
					}
				}
					break;	
				case CON_UMOU_UVAL:
				{
					if( g_objUsbUmount.HDisk[i] )
						break;
						
					if(access(BUS_HDISK, F_OK) == 0)
					{
						if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/part%d", ppHDiskList[10]->d_name, i+1), F_OK) == 0 
							&&SYSTEM("mount -t ext3 -o rw /dev/scsi/%s/bus0/target0/lun0/part%d /mnt/HDisk/part%d", ppHDiskList[10]->d_name, i+1, i+1) == 0 
						  &&access(SPRINTF(szTempBuf, "/mnt/HDisk/part%d/flash", i+1), F_OK) != 0)
						{
							if(access(SPRINTF(szTempBuf, "/mnt/HDisk/part%d/data", i+1), F_OK) == 0)
							{
								chmod(SPRINTF(szTempBuf, "/mnt/HDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);// 将目录的访问权限设置为所有人可读、可写、可执行
							}
							else
							{   
								if(mkdir(SPRINTF(szTempBuf, "/mnt/HDisk/part%d/data", i+1), 0777) == 0)   
								{   
									chmod(SPRINTF(szTempBuf, "/mnt/HDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);
								}
							}
									
							if((pHDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/HDisk/part%d/data/disk", i+1), "w+b")) == NULL)
							{
								dwHDiskMountCnt[i] = 0;
										
								SYSTEM("umount /mnt/HDisk/part%d", i+1);
										
								IOSET( IOS_HARDPOW, IO_HARDPOW_OFF, 1000 ); //重启硬盘电源
								IOSET( IOS_HARDPOW, IO_HARDPOW_ON, 1000 );
										
								g_objUsbSta.HDisk[i] = UCON_UMOU_UVAL;	
							}
							else
							{
								bHDiskExist[i] = TRUE;
								fclose(pHDiskFile[i]);
								pHDiskFile[i] = NULL;
								memset(&objHDiskBuf, 0, sizeof(objHDiskBuf));
								statfs(SPRINTF(szTempBuf, "/mnt/HDisk/part%d", i+1), &objHDiskBuf);
										
								dHDiskUsage[i] = 1 - objHDiskBuf.f_bavail/(double)objHDiskBuf.f_blocks;
								PRTMSG(MSG_NOR, "HDisk part%d mount succ, usage %.3f%%\n", i+1, 100*dHDiskUsage[i]);
								if(dHDiskUsage[i]>=MAX_SPACE_USAGE_HDISK)
									g_objUsbSta.HDisk[i] = CON_MOU_UVAL;
								else
									g_objUsbSta.HDisk[i] = CON_MOU_VAL;
								
								bool bMount = true;
								for(int c=0; c<PART_NUM_HDISK; c++)
								{
									if( MOUNT(&g_objUsbSta.HDisk[c])==false )
									{
										bMount = false;
										break;
									}
								}
								if( bMount )
									ShowDiaodu("检测到硬盘插入");
							}
						}
						else 
						{
							switch( ++dwHDiskMountCnt[i] )
							{
								case 5:
								case 10:
//									SYSTEM("umount /mnt/HDisk/part%d", i+1);
										
//									IOSET( IOS_HARDPOW, IO_HARDPOW_OFF, 1000 );//重启硬盘电源
//									IOSET( IOS_HARDPOW, IO_HARDPOW_ON, 1000 );
											
//									g_objUsbSta.HDisk[i] = UCON_UMOU_UVAL;
									break;
								case 15:
									if( !bHDiskExist[i] )
									{
										PRTMSG(MSG_NOR, "HDisk part%d can not dectect\n", i+1);
									}
									else
									{
										PRTMSG(MSG_NOR, "HDisk part%d can not work\n", i+1);
										//										StopVo();
										//										SYSTEM("reboot");
									}
									break;
								default:
									break;
							}
						}
					}
					else
					{
						dwHDiskMountCnt[i] = 0;
						g_objUsbSta.HDisk[i] = UCON_UMOU_UVAL;	
					}
				}
					break;
				case CON_MOU_UVAL:
				{
					if( g_objUsbUmount.HDisk[i] )
					{
						if(SYSTEM("umount /mnt/HDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "HDisk part%d umount succ\n", i+1);
						}
							
						dwHDiskMountCnt[i] = 0;
						g_objUsbSta.HDisk[i] = CON_UMOU_UVAL;	
						_KillTimer(&g_objTimerMng, TIMER_DIAODU_TIP);
						break;
					}
					
					if(++dwHDiskDetectCnt[i] == VALID_DECCNT_HDISK)
					{
						dwHDiskDetectCnt[i] = 0;
						if((pHDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/HDisk/part%d/data/disk", i+1), "r+b")) != NULL)
						{
							fclose(pHDiskFile[i]);
							pHDiskFile[i] = NULL;
								
							ClearDisk("HDisk", SPRINTF(szTempBuf, "part%d", i+1));/*清理硬盘数据*/
								
							memset(&objHDiskBuf, 0, sizeof(objHDiskBuf));
							statfs(SPRINTF(szTempBuf, "/mnt/HDisk/part%d", i+1), &objHDiskBuf);
							
							dHDiskUsage[i] = 1 - objHDiskBuf.f_bavail/(double)objHDiskBuf.f_blocks;
							if(dHDiskUsage[i]<=MIN_SPACE_USAGE_HDISK)
							{
								PRTMSG(MSG_NOR, "HDisk part%d usage %.3f%%\n", i+1, 100*dHDiskUsage[i]);
								g_objUsbSta.HDisk[i] = CON_MOU_VAL;
								_KillTimer(&g_objTimerMng, TIMER_DIAODU_TIP);

								ShowDiaodu("删除完成，HDisk part%d使用率%.3f%%", i+1, 100*dSDiskUsage[i]);
							}
						}
						else 
						{
							dwHDiskMountCnt[i] = 0;
							g_objUsbSta.HDisk[i] = UCON_UMOU_UVAL;	
							_KillTimer(&g_objTimerMng, TIMER_DIAODU_TIP);
							
							if(SYSTEM("umount /mnt/HDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "HDisk part%d umount succ\n", i+1);
							}
							
							bool bUconnect = true;
							for(int c=0; c<PART_NUM_HDISK; c++)
							{
								if( UCONNECT(&g_objUsbSta.HDisk[c])==false )
								{
									bUconnect = false;
									break;
								}
							}
							if( bUconnect )
								ShowDiaodu("检测到硬盘拔出");

							if( g_enumSysSta != SLEEP )
							{
								IOSET( IOS_HARDPOW, IO_HARDPOW_OFF, 1000 );//重启硬盘电源
								IOSET( IOS_HARDPOW, IO_HARDPOW_ON, 1000 );
							}
						}
					}
				}
					break;
				case CON_MOU_VAL:
				{
					if( g_objUsbUmount.HDisk[i] )
					{
						if(SYSTEM("umount /mnt/HDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "HDisk part%d umount succ\n", i+1);
						}
							
						dwHDiskMountCnt[i] = 0;
						g_objUsbSta.HDisk[i] = CON_UMOU_UVAL;	
						
						break;
					}
					
					if(++dwHDiskDetectCnt[i] == VALID_DECCNT_HDISK)
					{
						dwHDiskDetectCnt[i] = 0;
						if((pHDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/HDisk/part%d/data/disk", i+1), "r+b")) != NULL)
						{
							fclose(pHDiskFile[i]);
							pHDiskFile[i] = NULL;
								
							memset(&objHDiskBuf, 0, sizeof(objHDiskBuf));
							statfs(SPRINTF(szTempBuf, "/mnt/HDisk/part%d", i+1), &objHDiskBuf);
							
							dHDiskUsage[i] = 1 - objHDiskBuf.f_bavail/(double)objHDiskBuf.f_blocks;
							if(dHDiskUsage[i]>=MAX_SPACE_USAGE_HDISK)
							{
								PRTMSG(MSG_NOR, "HDisk part%d usage %.3f%%\n", i+1, 100*dHDiskUsage[i]);
								g_objUsbSta.HDisk[i] = CON_MOU_UVAL;
							}
						}
						else 
						{
							dwHDiskMountCnt[i] = 0;
							g_objUsbSta.HDisk[i] = UCON_UMOU_UVAL;	
								
							if(SYSTEM("umount /mnt/HDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "HDisk part%d umount succ\n", i+1);
							}
							
							bool bUconnect = true;
							for(int c=0; c<PART_NUM_HDISK; c++)
							{
								if( UCONNECT(&g_objUsbSta.HDisk[c])==false )
								{
									bUconnect = false;
									break;
								}
							}
							if( bUconnect )
								ShowDiaodu("检测到硬盘拔出");
								
							if( g_enumSysSta != SLEEP )
							{
								IOSET( IOS_HARDPOW, IO_HARDPOW_OFF, 1000 );//重启硬盘电源
								IOSET( IOS_HARDPOW, IO_HARDPOW_ON, 1000 );
							}
						}
					}
				}
					break;
				default:
					break;
			}
		}

		/*----------U盘状态检测----------*/
#if VEHICLE_TYPE == VEHICLE_M
		for(i=0; i<PART_NUM_UDISK; i++)
		{
			switch(g_objUsbSta.UDisk[i])
			{
				case UCON_UMOU_UVAL:
				{
					if(access(BUS_UDISK, F_OK) == 0)
					{
						scandir(DEV_UDISK, &ppUDiskList, 0, alphasort);		
							
						if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/disc", ppUDiskList[10]->d_name), F_OK) == 0)
							g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;
					}
				}
					break;
				case CON_UMOU_UVAL:
				{
					if( g_objUsbUmount.UDisk[i] )
						break;
						
					if(access(BUS_UDISK, F_OK) == 0)
					{	
						if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/part%d", ppUDiskList[10]->d_name, i+1), F_OK) == 0 
						 &&SYSTEM("mount -t vfat -o rw /dev/scsi/%s/bus0/target0/lun0/part%d /mnt/UDisk/part%d", ppUDiskList[10]->d_name, i+1, i+1) == 0 
						 &&access(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/flash", i+1), F_OK) != 0)
						{
							if(access(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), F_OK) == 0)
							{
								chmod(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);// 将目录的访问权限设置为所有人可读、可写、可执行
							}
							else
							{   
								if(mkdir(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), 0777) == 0)   
								{
									chmod(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);
								}
							}
									
							if((pUDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data/disk", i+1), "w+b")) == NULL)
							{
								dwUDiskMountCnt[i] = 0;
								SYSTEM("umount /mnt/UDisk/part%d", i+1);
								g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
							}
							else
							{
								fclose(pUDiskFile[i]);
								pUDiskFile[i] = NULL;							
										
								memset(&objUDiskBuf, 0, sizeof(objUDiskBuf));
								statfs(SPRINTF(szTempBuf, "/mnt/UDisk/part%d", i+1), &objUDiskBuf);
										
								dUDiskUsage[i] = 1 - objUDiskBuf.f_bavail/(double)objUDiskBuf.f_blocks;
								PRTMSG(MSG_NOR, "UDisk part%d mount succ, usage %.3f%%\n", i+1, 100*dUDiskUsage[i]);
								if(dUDiskUsage[i]>=MAX_SPACE_USAGE_UDISK)
								{
									g_objUsbSta.UDisk[i] = CON_MOU_UVAL;
								}
								else
								{
									g_objUsbSta.UDisk[i] = CON_MOU_VAL;
								}

								bool bMount = true;
								for(int c=0; c<PART_NUM_UDISK; c++)
								{
									if( MOUNT(&g_objUsbSta.UDisk[c])==false )
									{
										bMount = false;
										break;
									}
								}
								if( bMount )
									ShowDiaodu("检测到U盘插入");
							}
						}
						else 
						{
							switch( ++dwUDiskMountCnt[i] )
							{
								case 5:
								case 10:
									SYSTEM("umount /mnt/UDisk/part%d", i+1);
									g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;
									break;
								case 15:
									PRTMSG(MSG_NOR, "UDisk part%d can not dectect\n", i+1);
									break;
								default:
									break;
							}
						}
					}
					else
					{
						dwUDiskMountCnt[i] = 0;
						g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
					}
				}
					break;
				case CON_MOU_UVAL:
				{		
					if( g_objUsbUmount.UDisk[i] )
					{
						if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
						}
							
						dwUDiskMountCnt[i] = 0;
						g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;	
						
						break;
					}
					
					if(access(BUS_UDISK, F_OK) == 0)
					{
						memset(&objUDiskBuf, 0, sizeof(objUDiskBuf));
						statfs(SPRINTF(szTempBuf, "/mnt/UDisk/part%d", i+1), &objUDiskBuf);
						
						dUDiskUsage[i] = 1 - objUDiskBuf.f_bavail/(double)objUDiskBuf.f_blocks;
						if(dUDiskUsage[i]<=MIN_SPACE_USAGE_UDISK)
						{
							PRTMSG(MSG_NOR, "UDisk part%d usage %.3f%%\n", i+1, 100*dUDiskUsage[i]);
							g_objUsbSta.UDisk[i] = CON_MOU_VAL;
						}
					}
					else
					{
						if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
						}
								
						dwUDiskMountCnt[i] = 0;
						g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
						
						bool bUconnect = true;
						for(int c=0; c<PART_NUM_UDISK; c++)
						{
							if( UCONNECT(&g_objUsbSta.UDisk[c])==false )
							{
								bUconnect = false;
								break;
							}
						}
						if( bUconnect )
							ShowDiaodu("检测到U盘拔出");
					}
				}
					break;
				case CON_MOU_VAL:
				{
					if( g_objUsbUmount.UDisk[i] )
					{
						if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
						}
							
						dwUDiskMountCnt[i] = 0;
						g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;	
						
						break;
					}
					
					if(access(BUS_UDISK, F_OK) == 0)
					{
						memset(&objUDiskBuf, 0, sizeof(objUDiskBuf));
						statfs(SPRINTF(szTempBuf, "/mnt/UDisk/part%d", i+1), &objUDiskBuf);
						
						dUDiskUsage[i] = 1 - objUDiskBuf.f_bavail/(double)objUDiskBuf.f_blocks;
						if(dUDiskUsage[i] >= MAX_SPACE_USAGE_UDISK)
						{
							PRTMSG(MSG_NOR, "UDisk part%d usage %.3f%%\n", i+1, 100*dUDiskUsage[i]);
							g_objUsbSta.UDisk[i] = CON_MOU_UVAL;
						}
					}
					else
					{
						if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
						}
								
						dwUDiskMountCnt[i] = 0;
						g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;
						
						bool bUconnect = true;
						for(int c=0; c<PART_NUM_UDISK; c++)
						{
							if( UCONNECT(&g_objUsbSta.UDisk[c])==false )
							{
								bUconnect = false;
								break;
							}
						}
						if( bUconnect )
							ShowDiaodu("检测到U盘拔出");
					}
				}
					break;
				default:
					break;
			}
		}
#endif

#if VEHICLE_TYPE == VEHICLE_M2
		for(i=0; i<PART_NUM_UDISK; i++)
		{
			switch(g_objUsbSta.UDisk[i])
			{
				case UCON_UMOU_UVAL:
				{
//					ResetUsb();//如果usb otg出错则复位之
					if(access(BUS_UDISK, F_OK) == 0)
					{
						int n = scandir(DEV_UDISK, &ppUDiskList, 0, alphasort);	
						if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/disc", ppUDiskList[10]->d_name), F_OK) == 0)
						{
							bUDiskMount[i] = TRUE;
							g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;
						}
					}
				}
					break;
				case CON_UMOU_UVAL:
				{
					if( g_objUsbUmount.UDisk[i] )
						break;
						
					if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/part%d", ppUDiskList[10]->d_name, i+1), F_OK) == 0)
					{	
						if( bUDiskMount[i] )
						{
							if(SYSTEM("mount -t vfat -o rw /dev/scsi/%s/bus0/target0/lun0/part%d /mnt/UDisk/part%d", ppUDiskList[10]->d_name, i+1, i+1) == 0 
							 &&access(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/flash", i+1), F_OK) != 0)
							{
								if(access(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), F_OK) == 0)
								{
									chmod(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);// 将目录的访问权限设置为所有人可读、可写、可执行
								}
								else
								{   
									if(mkdir(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), 0777) == 0)   
									{   
										chmod(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);
									}
								}
										
								if((pUDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data/disk", i+1), "w+b")) == NULL)
								{
									switch(++dwUDiskDetectCnt[i])
									{
										case 1:
										case 2:
										case 3:
										case 4:
										{
											dwUDiskMountCnt[i] = 0;
											g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
													
											SYSTEM("umount /mnt/UDisk/part%d", i+1);
										}
											 break;
										case 5:
										{
											bUDiskMount[i] = FALSE;//当U盘连续5次读写失败时，停止挂载
										}
											break;
										default:
											break;
									}
								}
								else
								{
									fclose(pUDiskFile[i]);
									pUDiskFile[i] = NULL;
									
									dwUDiskMountCnt[i] = 0;
									dwUDiskDetectCnt[i] = 0;

									g_objUsbSta.UDisk[i] = CON_MOU_UVAL;
									
									dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
									dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');

									if( dUDiskInodeUsage[i]>=MAX_INODE_USAGE_UDISK )
									{
										bUDiskInodeValid[i] = FALSE;
									}
									else
									{
										bUDiskInodeValid[i] = TRUE;
									}
									
									if( dUDiskSpaceUsage[i]>=MAX_SPACE_USAGE_UDISK )
									{
										bUDiskSpaceValid[i] = FALSE;
									}
									else
									{
										bUDiskSpaceValid[i] = TRUE;
									}
									
									if( bUDiskInodeValid[i] && bUDiskSpaceValid[i] )
									{
										g_objUsbSta.UDisk[i] = CON_MOU_VAL;
									}
										
									PRTMSG(MSG_NOR, "UDisk part%d mount succ, inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dUDiskInodeUsage[i], 100*dUDiskSpaceUsage[i]);

									bool bMount = true;
									for(int c=0; c<PART_NUM_UDISK; c++)
									{
										if( MOUNT(&g_objUsbSta.UDisk[c])==false )
										{
											bMount = false;
											break;
										}
									}
									if( bMount )
										ShowDiaodu("检测到U盘插入");
								}
							}
							else 
							{
								switch( ++dwUDiskMountCnt[i] )
								{
									case 2:
									case 4:
//										SYSTEM("umount /mnt/UDisk/part%d", i+1);
//										g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;
										break;
									case 5:
										bUDiskMount[i] = FALSE;//当U盘连续5次挂载失败时，停止挂载
										PRTMSG(MSG_NOR, "UDisk part%d can not dectect\n", i+1);
										break;
									default:
										break;
								}
							}
						}
					}
					else
					{
						dwUDiskMountCnt[i] = 0;
						dwUDiskDetectCnt[i] = 0;
						g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
					}
				}
					break;
				case CON_MOU_UVAL:
				{
					if( g_objUsbUmount.UDisk[i] )
					{
						if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
						}
							
						dwUDiskMountCnt[i] = 0;
						dwUDiskDetectCnt[i] = 0;
						g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;	
						
						break;
					}
					
					++dwUDiskDetectCnt[i];
					
					if((pUDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data/disk", i+1), "r+b")) != NULL)
					{
						fclose(pUDiskFile[i]);
						pUDiskFile[i] = NULL;

						//ClearDisk("UDisk", SPRINTF(szTempBuf, "part%d", i+1));

						if( !bUDiskInodeValid[i] )
						{
							dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
							if( dUDiskInodeUsage[i]<=MIN_INODE_USAGE_UDISK )
							{
								bUDiskInodeValid[i] = TRUE;
							}
						}
						
						if( !bUDiskSpaceValid[i] )
						{
							dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
							if( dUDiskSpaceUsage[i]<=MIN_SPACE_USAGE_UDISK )
							{
								bUDiskSpaceValid[i] = TRUE;
							}
						}
						
						if( bUDiskInodeValid[i] && bUDiskSpaceValid[i] )
						{
//							dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
//							dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
							PRTMSG(MSG_NOR, "UDisk part%d: inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dUDiskInodeUsage[i], 100*dUDiskSpaceUsage[i]);
							
							g_objUsbSta.UDisk[i] = CON_MOU_VAL;
						}
					}
					else
					{
						dwUDiskMountCnt[i] = 0;
						dwUDiskDetectCnt[i] = 0;
						g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
							
						if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
						}
							
						bool bUconnect = true;
						for(int c=0; c<PART_NUM_UDISK; c++)
						{
							if( UCONNECT(&g_objUsbSta.UDisk[c])==false )
							{
								bUconnect = false;
								break;
							}
						}
						if( bUconnect )
							ShowDiaodu("检测到U盘拔出");
					}
				}
					break;
				case CON_MOU_VAL:
				{
					if( g_objUsbUmount.UDisk[i] )
					{
						if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);							
						}
							
						dwUDiskMountCnt[i] = 0;
						dwUDiskDetectCnt[i] = 0;
						g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;	
						
						break;
					}
					
					if(++dwUDiskDetectCnt[i]%VALID_DECCNT_UDISK == 0)
					{
						if((pUDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data/disk", i+1), "r+b")) != NULL)
						{
							fclose(pUDiskFile[i]);
							pUDiskFile[i] = NULL;
							
							if(dwUDiskDetectCnt[i]%INODE_DECCNT_UDISK == 0)
							{
								dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
								if( dUDiskInodeUsage[i]>=MAX_INODE_USAGE_UDISK )
								{
									bUDiskInodeValid[i] = FALSE;
								}
							}
							
							if(dwUDiskDetectCnt[i]%SPACE_DECCNT_UDISK == 0)
							{
								dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
								if( dUDiskSpaceUsage[i]>=MAX_SPACE_USAGE_UDISK )
								{
									bUDiskSpaceValid[i] = FALSE;
								}
							}
							
							if( !bUDiskInodeValid[i] || !bUDiskSpaceValid[i] )
							{
//								dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
//								dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
								PRTMSG(MSG_NOR, "UDisk part%d: inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dUDiskInodeUsage[i], 100*dUDiskSpaceUsage[i]);
								
								g_objUsbSta.UDisk[i] = CON_MOU_UVAL;
							}
						}
						else
						{
							if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
							}
									
							dwUDiskMountCnt[i] = 0;
							dwUDiskDetectCnt[i] = 0;
							g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;
							
							bool bUconnect = true;
							for(int c=0; c<PART_NUM_UDISK; c++)
							{
								if( UCONNECT(&g_objUsbSta.UDisk[c])==false )
								{
									bUconnect = false;
									break;
								}
							}
							if( bUconnect )
								ShowDiaodu("检测到U盘拔出");
						}
					}
				}
					break;
				default:
					break;
			}
		}
#endif
#endif

#if VEHICLE_TYPE == VEHICLE_V8
		/*----------SD卡状态检测----------*/
		for(i=0; i<PART_NUM_SDISK; i++)
		{
			switch(g_objUsbSta.SDisk[i])
			{
				case UCON_UMOU_UVAL:/*未连接-未挂载-未可用*/
				{
					dwSDiskUconnectCnt[i]++;
					dwSDiskUmountCnt[i]++;
					
					if( dwSDiskUconnectCnt[i]==UCONNECT_INIT_SDISK )
					{
						//BYTE ucIOState;
						//IOGet( IOS_SPIDO, &ucIOState );
						//if( ucIOState==IO_SPIDO_CARD_EXIST && IDLESTATE(&g_enumSysSta) && g_iTempState!=2 )
						if( IDLESTATE(&g_enumSysSta) && g_iTempState!=2 )
						{
#if DISK_UCONNECT_INIT == 1
							InitSd();
#endif

							ShowDiaodu("SD卡异常");
							RenewMemInfo(0x02, 0x00, 0x00, 0x03);
						}
						else
						{
							dwSDiskUconnectCnt[i] = 0;//条件不满足时重新计时
						}
					}
					
					if( dwSDiskUmountCnt[i]==UMOUNT_REBOOT_SDISK )
					{
						BYTE ucIOState;
						IOGet( IOS_ACC, &ucIOState );
						if( ucIOState==IO_ACC_ON && IDLESTATE(&g_enumSysSta) && g_iTempState==0 )
						{
#if DISK_UMOUNT_REBOOT == 1
							PRTMSG(MSG_ERR, "Disk umount for 20 minutes, reset system\n");
							ShowDiaodu("SD卡不可用,重启设备");
							G_ResetSystem();
#endif
						}
						else
						{
							dwSDiskUmountCnt[i] = 0;//条件不满足时重新计时
						}
					}

					if(access("/dev/mmc/blk0/disc", F_OK) == 0)
					{
						bSDiskMount[i] = TRUE;
						g_objUsbSta.SDisk[i] = CON_UMOU_UVAL;
					}
				}
					break;
				case CON_UMOU_UVAL:/*已连接-未挂载-未可用*/
				{
					dwSDiskUconnectCnt[i] = 0;
					dwSDiskUmountCnt[i]++;

					if( dwSDiskUmountCnt[i]==UMOUNT_REBOOT_SDISK )
					{
						BYTE ucIOState;
						IOGet( IOS_ACC, &ucIOState );
						if( ucIOState==IO_ACC_ON && IDLESTATE(&g_enumSysSta) && g_iTempState==0 )
						{
#if DISK_UMOUNT_REBOOT == 1							
							PRTMSG(MSG_ERR, "Disk umount for 20 minutes, reset system\n");
							ShowDiaodu("SD卡不可用,重启设备");
							G_ResetSystem();
#endif
						}
						else
						{
							dwSDiskUmountCnt[i] = 0;//条件不满足时重新计时
						}
					}

					if( g_objUsbUmount.SDisk[i] )
						break;
						
					if(access(SPRINTF(szTempBuf, "/dev/mmc/blk0/part%d", i+1), F_OK) == 0)
					{
						if( bSDiskMount[i] )
						{
							if(SYSTEM("mount -t ext3 -o rw /dev/mmc/blk0/part%d /mnt/SDisk/part%d", i+1, i+1) == 0 
							 &&access(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/flash", i+1), F_OK) != 0)
							{
								if(access(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data", i+1), F_OK) == 0)
								{
									chmod(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);// 将目录的访问权限设置为所有人可读、可写、可执行
								}
								else
								{   
									if(mkdir(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data", i+1), 0777) == 0)   
									{   
										chmod(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);
									}
								}
										
								if((pSDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data/disk", i+1), "w+b")) == NULL)
								{
									switch(++dwSDiskDetectCnt[i])
									{
										case 1:
										case 2:
										case 3:
										case 4:
										{
											dwSDiskMountCnt[i] = 0;
											g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;	
													
											SYSTEM("umount /mnt/SDisk/part%d", i+1);
										}
											 break;
										case 5:
										{
											ShowDiaodu("SD卡异常");
											RenewMemInfo(0x02, 0x00, 0x00, 0x03);

											bSDiskMount[i] = FALSE;//当SD卡连续5次读写失败时，停止挂载
										}
											break;
										default:
											break;
									}
								}
								else
								{
									fclose(pSDiskFile[i]);
									pSDiskFile[i] = NULL;
									bSDiskExist[i] = TRUE;
									
									dwSDiskMountCnt[i] = 0;
									dwSDiskDetectCnt[i] = 0;
	
									// SD卡挂载成功后，让CPhoto读取照片索引								
									g_objPhoto.ReadIndex();

									g_objUsbSta.SDisk[i] = CON_MOU_UVAL;

									dSDiskInodeUsage[i] = GetDiskUsage(SDISK, i, 'I');
									dSDiskSpaceUsage[i] = GetDiskUsage(SDISK, i, 'S');

									if( dSDiskInodeUsage[i]>=MAX_INODE_USAGE_SDISK )
									{
										bSDiskInodeValid[i] = FALSE;
									}
									else
									{
										bSDiskInodeValid[i] = TRUE;
									}
									
									if( dSDiskSpaceUsage[i]>=MAX_SPACE_USAGE_SDISK )
									{
										bSDiskSpaceValid[i] = FALSE;
									}
									else
									{
										bSDiskSpaceValid[i] = TRUE;
									}
									
									if( bSDiskInodeValid[i] && bSDiskSpaceValid[i] )
									{
										g_objUsbSta.SDisk[i] = CON_MOU_VAL;
									}
									
									PRTMSG(MSG_NOR, "SDisk part%d mount succ, inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dSDiskInodeUsage[i], 100*dSDiskSpaceUsage[i]);
									
									bool bMount = true;
									for(int c=0; c<PART_NUM_SDISK; c++)
									{
										if( MOUNT(&g_objUsbSta.SDisk[c])==false )
										{
											bMount = false;
											break;
										}
									}
									if( bMount )
									{
										GetCurDateTime( iErrTime );
										PRTMSG(MSG_DBG, "SD卡挂载时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);
										G_RecordDebugInfo("SD卡挂载时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);
																	
										ShowDiaodu("检测到SD卡插入");
										
										char buf = 0x05;
										DataPush((void*)&buf, 1, DEV_DVR, DEV_QIAN, LV2);

										buf = 0x01;
										DataPush((void*)&buf, 1, DEV_DVR, DEV_IO, LV2);
									}
								}
							}
							else 
							{
								switch(++dwSDiskMountCnt[i])
								{
									case 2:
									case 4:
									{
		//								SYSTEM("umount /mnt/SDisk/part%d", i+1);
		//								g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;
										
	//									InitSd();
									}
										 break;
									case 5:
									{
										ShowDiaodu("SD卡异常");
										RenewMemInfo(0x02, 0x00, 0x00, 0x03);

										bSDiskMount[i] = FALSE;//当SD卡连续5次挂载失败时，停止挂载
										
										if( !bSDiskExist[i] )
										{
											PRTMSG(MSG_NOR, "SDisk part%d can not dectect\n", i+1);
										}
										else
										{
											PRTMSG(MSG_NOR, "SDisk part%d can not work\n", i+1);
											//StopVo();
											//SYSTEM("reboot");
										}
									}
										break;
									default:
										break;
								}
							}
						}
					}
					else
					{
						dwSDiskMountCnt[i] = 0;
						dwSDiskDetectCnt[i] = 0;
						g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;	
					}
				}
					break;
				case CON_MOU_UVAL:/*已连接-已挂载-未可用*/
				{
					dwSDiskUconnectCnt[i] = 0;
					dwSDiskUmountCnt[i] = 0;

					if( g_objUsbUmount.SDisk[i] )
					{
						// 先通知，后卸载
						char buf = 0x06;
						DataPush((void*)&buf, 1, DEV_DVR, DEV_QIAN, LV2);						
						buf = 0x02;
						DataPush((void*)&buf, 1, DEV_DVR, DEV_IO, LV2);
						sleep(8);

						for(int n = 0; n < 3; n++)
						{
							if(SYSTEM("umount /mnt/SDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "SDisk part%d umount succ\n", i+1);
								break;
							}
								
							sleep(1);
						}
						
						GetCurDateTime( iErrTime );
						PRTMSG(MSG_DBG, "SD卡卸载时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);				
						G_RecordDebugInfo("SD卡卸载时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);				
						
						dwSDiskMountCnt[i] = 0;
						dwSDiskDetectCnt[i] = 0;
						g_objUsbSta.SDisk[i] = CON_UMOU_UVAL;	
						_KillTimer(&g_objTimerMng, TIMER_DIAODU_TIP);
						break;
					}

					++dwSDiskDetectCnt[i];//SD卡满时应尽快删除文件，故而每秒都要检测
					
					if((pSDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data/disk", i+1), "r+b")) != NULL)
					{
						fclose(pSDiskFile[i]);
						pSDiskFile[i] = NULL;

						ClearDisk("SDisk", SPRINTF(szTempBuf, "part%d", i+1));

						if( !bSDiskInodeValid[i] )
						{
							if( dwSDiskDetectCnt[i]%10==0 )//为加快文件删除速度，每10秒检测一次节点使用率
							{
								dSDiskInodeUsage[i] = GetDiskUsage(SDISK, i, 'I');
								if( dSDiskInodeUsage[i]<=MIN_INODE_USAGE_SDISK )
								{
									bSDiskInodeValid[i] = TRUE;
								}
							}
						}
						
						if( !bSDiskSpaceValid[i] )
						{
							dSDiskSpaceUsage[i] = GetDiskUsage(SDISK, i, 'S');
							if( dSDiskSpaceUsage[i]<=MIN_SPACE_USAGE_SDISK )
							{
								bSDiskSpaceValid[i] = TRUE;
							}
						}
						
						if( bSDiskInodeValid[i] && bSDiskSpaceValid[i] )
						{
//							dSDiskInodeUsage[i] = GetDiskUsage(SDISK, i, 'I');
//							dSDiskSpaceUsage[i] = GetDiskUsage(SDISK, i, 'S');
							PRTMSG(MSG_NOR, "SDisk part%d: inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dSDiskInodeUsage[i], 100*dSDiskSpaceUsage[i]);
							
							g_objUsbSta.SDisk[i] = CON_MOU_VAL;
							_KillTimer(&g_objTimerMng, TIMER_DIAODU_TIP);
							ShowDiaodu("删除完成，SDisk part%d使用率%.3f%%", i+1, 100*dSDiskUsage[i]);
						}
					}
					else
					{
						dwSDiskMountCnt[i] = 0;
						dwSDiskDetectCnt[i] = 0;
						g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;	
						_KillTimer(&g_objTimerMng, TIMER_DIAODU_TIP);
						
						// 先通知，后卸载
						char buf = 0x06;
						DataPush((void*)&buf, 1, DEV_DVR, DEV_QIAN, LV2);						
						buf = 0x02;
						DataPush((void*)&buf, 1, DEV_DVR, DEV_IO, LV2);
						sleep(8);

						for(int n = 0; n < 3; n++)
						{
							if(SYSTEM("umount /mnt/SDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "SDisk part%d umount succ\n", i+1);
								break;
							}
								
							sleep(1);
						}
						
						GetCurDateTime( iErrTime );
						PRTMSG(MSG_DBG, "SD卡卸载时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);
						G_RecordDebugInfo("SD卡卸载时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);
							
						bool bUconnect = true;
						for(int c=0; c<PART_NUM_SDISK; c++)
						{
							if( UCONNECT(&g_objUsbSta.SDisk[c])==false )
							{
								bUconnect = false;
								break;
							}
						}
						if( bUconnect )
						{
							ShowDiaodu("检测到SD卡拔出");							
						}
					}
				}
				break;
				case CON_MOU_VAL:/*已连接-已挂载-已可用*/
				{
					dwSDiskUconnectCnt[i] = 0;
					dwSDiskUmountCnt[i] = 0;
					
					if( g_objUsbUmount.SDisk[i] )
					{
						// 先通知，后卸载
						char buf = 0x06;
						DataPush((void*)&buf, 1, DEV_DVR, DEV_QIAN, LV2);						
						buf = 0x02;
						DataPush((void*)&buf, 1, DEV_DVR, DEV_IO, LV2);
						sleep(8);

						for(int n = 0; n < 3; n++)
						{
							if(SYSTEM("umount /mnt/SDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "SDisk part%d umount succ\n", i+1);
								break;
							}
								
							sleep(1);
						}
						
						GetCurDateTime( iErrTime );
						PRTMSG(MSG_DBG, "SD卡卸载时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);				
							
						dwSDiskMountCnt[i] = 0;
						dwSDiskDetectCnt[i] = 0;
						g_objUsbSta.SDisk[i] = CON_UMOU_UVAL;	
						
						break;
					}
					
					if(++dwSDiskDetectCnt[i]%VALID_DECCNT_SDISK == 0)//SD卡未满时每隔一定时间检测一次
					{
						if((pSDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/SDisk/part%d/data/disk", i+1), "r+b")) != NULL)
						{
							fclose(pSDiskFile[i]);
							pSDiskFile[i] = NULL;
							
							if(dwSDiskDetectCnt[i]%INODE_DECCNT_SDISK == 0)
							{
								dSDiskInodeUsage[i] = GetDiskUsage(SDISK, i, 'I');
								if( dSDiskInodeUsage[i]>=MAX_INODE_USAGE_SDISK )
								{
									bSDiskInodeValid[i] = FALSE;
								}
							}
							
							if(dwSDiskDetectCnt[i]%SPACE_DECCNT_SDISK == 0)
							{
								dSDiskSpaceUsage[i] = GetDiskUsage(SDISK, i, 'S');
								if( dSDiskSpaceUsage[i]>=MAX_SPACE_USAGE_SDISK )
								{
									bSDiskSpaceValid[i] = FALSE;
								}
							}
							
							if( !bSDiskInodeValid[i] || !bSDiskSpaceValid[i] )
							{
//								dSDiskInodeUsage[i] = GetDiskUsage(SDISK, i, 'I');
//								dSDiskSpaceUsage[i] = GetDiskUsage(SDISK, i, 'S');
								PRTMSG(MSG_NOR, "SDisk part%d: inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dSDiskInodeUsage[i], 100*dSDiskSpaceUsage[i]);
								
								g_objUsbSta.SDisk[i] = CON_MOU_UVAL;
							}
						}
						else
						{
							dwSDiskMountCnt[i] = 0;
							dwSDiskDetectCnt[i] = 0;
							g_objUsbSta.SDisk[i] = UCON_UMOU_UVAL;	
							
							// 先通知，后卸载
							char buf = 0x06;
							DataPush((void*)&buf, 1, DEV_DVR, DEV_QIAN, LV2);						
							buf = 0x02;
							DataPush((void*)&buf, 1, DEV_DVR, DEV_IO, LV2);
							sleep(8);

							for(int n = 0; n < 3; n++)
							{
								if(SYSTEM("umount /mnt/SDisk/part%d", i+1) == 0)
								{
									PRTMSG(MSG_NOR, "SDisk part%d umount succ\n", i+1);
									break;
								}
								
								sleep(1);
							}
							
							GetCurDateTime( iErrTime );
							PRTMSG(MSG_DBG, "SD卡卸载时间：%04d-%02d-%02d %02d:%02d:%02d\n", iErrTime[0], iErrTime[1], iErrTime[2], iErrTime[3], iErrTime[4], iErrTime[5]);
							
							bool bUconnect = true;
							for(int c=0; c<PART_NUM_SDISK; c++)
							{
								if( UCONNECT(&g_objUsbSta.SDisk[c])==false )
								{
									bUconnect = false;
									break;
								}
							}
							if( bUconnect )
							{
								ShowDiaodu("检测到SD卡拔出");
							}
						}
					}
				}
					break;
				default:
					break;
			}
		}

		/*----------U盘状态检测----------*/
		for(i=0; i<PART_NUM_UDISK; i++)
		{
			switch(g_objUsbSta.UDisk[i])
			{
				case UCON_UMOU_UVAL:
				{
//					ResetUsb();//如果usb otg出错则复位之
					if(access(BUS_UDISK, F_OK) == 0)
					{
						int n = scandir(DEV_UDISK, &ppUDiskList, 0, alphasort);	
						if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/disc", ppUDiskList[10]->d_name), F_OK) == 0)
						{
							bUDiskMount[i] = TRUE;
							g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;
						}
					}
				}
					break;
				case CON_UMOU_UVAL:
				{
					if( g_objUsbUmount.UDisk[i] )
						break;
						
					if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/part%d", ppUDiskList[10]->d_name, i+1), F_OK) == 0)
					{	
						if( bUDiskMount[i] )
						{
							if(SYSTEM("mount -t vfat -o rw /dev/scsi/%s/bus0/target0/lun0/part%d /mnt/UDisk/part%d", ppUDiskList[10]->d_name, i+1, i+1) == 0 
							 &&access(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/flash", i+1), F_OK) != 0)
							{
								if(access(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), F_OK) == 0)
								{
									chmod(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);// 将目录的访问权限设置为所有人可读、可写、可执行
								}
								else
								{   
									if(mkdir(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), 0777) == 0)   
									{   
										chmod(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);
									}
								}
										
								if((pUDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data/disk", i+1), "w+b")) == NULL)
								{
									switch(++dwUDiskDetectCnt[i])
									{
										case 1:
										case 2:
										case 3:
										case 4:
										{
											dwUDiskMountCnt[i] = 0;
											g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
													
											SYSTEM("umount /mnt/UDisk/part%d", i+1);
										}
											 break;
										case 5:
										{
											bUDiskMount[i] = FALSE;//当U盘连续5次读写失败时，停止挂载
										}
											break;
										default:
											break;
									}
								}
								else
								{
									fclose(pUDiskFile[i]);
									pUDiskFile[i] = NULL;
									
									dwUDiskMountCnt[i] = 0;
									dwUDiskDetectCnt[i] = 0;

									g_objUsbSta.UDisk[i] = CON_MOU_UVAL;
									
									dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
									dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');

									if( dUDiskInodeUsage[i]>=MAX_INODE_USAGE_UDISK )
									{
										bUDiskInodeValid[i] = FALSE;
									}
									else
									{
										bUDiskInodeValid[i] = TRUE;
									}
									
									if( dUDiskSpaceUsage[i]>=MAX_SPACE_USAGE_UDISK )
									{
										bUDiskSpaceValid[i] = FALSE;
									}
									else
									{
										bUDiskSpaceValid[i] = TRUE;
									}
									
									if( bUDiskInodeValid[i] && bUDiskSpaceValid[i] )
									{
										g_objUsbSta.UDisk[i] = CON_MOU_VAL;
									}
										
									PRTMSG(MSG_NOR, "UDisk part%d mount succ, inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dUDiskInodeUsage[i], 100*dUDiskSpaceUsage[i]);

									bool bMount = true;
									for(int c=0; c<PART_NUM_UDISK; c++)
									{
										if( MOUNT(&g_objUsbSta.UDisk[c])==false )
										{
											bMount = false;
											break;
										}
									}
									if( bMount )
										ShowDiaodu("检测到U盘插入");
								}
							}
							else 
							{
								switch( ++dwUDiskMountCnt[i] )
								{
									case 2:
									case 4:
//										SYSTEM("umount /mnt/UDisk/part%d", i+1);
//										g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;
										break;
									case 5:
										bUDiskMount[i] = FALSE;//当U盘连续5次挂载失败时，停止挂载
										PRTMSG(MSG_NOR, "UDisk part%d can not dectect\n", i+1);
										break;
									default:
										break;
								}
							}
						}
					}
					else
					{
						dwUDiskMountCnt[i] = 0;
						dwUDiskDetectCnt[i] = 0;
						g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
					}
				}
					break;
				case CON_MOU_UVAL:
				{
					if( g_objUsbUmount.UDisk[i] )
					{
						if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
						}
							
						dwUDiskMountCnt[i] = 0;
						dwUDiskDetectCnt[i] = 0;
						g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;	
						
						break;
					}
					
					++dwUDiskDetectCnt[i];
					
					if((pUDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data/disk", i+1), "r+b")) != NULL)
					{
						fclose(pUDiskFile[i]);
						pUDiskFile[i] = NULL;

						//ClearDisk("UDisk", SPRINTF(szTempBuf, "part%d", i+1));

						if( !bUDiskInodeValid[i] )
						{
							dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
							if( dUDiskInodeUsage[i]<=MIN_INODE_USAGE_UDISK )
							{
								bUDiskInodeValid[i] = TRUE;
							}
						}
						
						if( !bUDiskSpaceValid[i] )
						{
							dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
							if( dUDiskSpaceUsage[i]<=MIN_SPACE_USAGE_UDISK )
							{
								bUDiskSpaceValid[i] = TRUE;
							}
						}
						
						if( bUDiskInodeValid[i] && bUDiskSpaceValid[i] )
						{
//							dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
//							dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
							PRTMSG(MSG_NOR, "UDisk part%d: inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dUDiskInodeUsage[i], 100*dUDiskSpaceUsage[i]);
							
							g_objUsbSta.UDisk[i] = CON_MOU_VAL;
						}
					}
					else
					{
						dwUDiskMountCnt[i] = 0;
						dwUDiskDetectCnt[i] = 0;
						g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
							
						if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
						}
							
						bool bUconnect = true;
						for(int c=0; c<PART_NUM_UDISK; c++)
						{
							if( UCONNECT(&g_objUsbSta.UDisk[c])==false )
							{
								bUconnect = false;
								break;
							}
						}
						if( bUconnect )
							ShowDiaodu("检测到U盘拔出");
					}
				}
					break;
				case CON_MOU_VAL:
				{
					if( g_objUsbUmount.UDisk[i] )
					{
						if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
						{
							PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);							
						}
							
						dwUDiskMountCnt[i] = 0;
						dwUDiskDetectCnt[i] = 0;
						g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;	
						
						break;
					}
					
					if(++dwUDiskDetectCnt[i]%VALID_DECCNT_UDISK == 0)
					{
						if((pUDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data/disk", i+1), "r+b")) != NULL)
						{
							fclose(pUDiskFile[i]);
							pUDiskFile[i] = NULL;
							
							if(dwUDiskDetectCnt[i]%INODE_DECCNT_UDISK == 0)
							{
								dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
								if( dUDiskInodeUsage[i]>=MAX_INODE_USAGE_UDISK )
								{
									bUDiskInodeValid[i] = FALSE;
								}
							}
							
							if(dwUDiskDetectCnt[i]%SPACE_DECCNT_UDISK == 0)
							{
								dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
								if( dUDiskSpaceUsage[i]>=MAX_SPACE_USAGE_UDISK )
								{
									bUDiskSpaceValid[i] = FALSE;
								}
							}
							
							if( !bUDiskInodeValid[i] || !bUDiskSpaceValid[i] )
							{
//								dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
//								dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
								PRTMSG(MSG_NOR, "UDisk part%d: inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dUDiskInodeUsage[i], 100*dUDiskSpaceUsage[i]);
								
								g_objUsbSta.UDisk[i] = CON_MOU_UVAL;
							}
						}
						else
						{
							if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
							}
									
							dwUDiskMountCnt[i] = 0;
							dwUDiskDetectCnt[i] = 0;
							g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;
							
							bool bUconnect = true;
							for(int c=0; c<PART_NUM_UDISK; c++)
							{
								if( UCONNECT(&g_objUsbSta.UDisk[c])==false )
								{
									bUconnect = false;
									break;
								}
							}
							if( bUconnect )
								ShowDiaodu("检测到U盘拔出");
						}
					}
				}
					break;
				default:
					break;
			}
		}
#endif
		
		sleep(1);
	}
}

int DiskTest(char *v_szDiskStyle)
{
	int i;
	int iDiskCount;
	int iFdisk = -1;
	char szTestFile[64];
	
	if( strcmp("SDisk", v_szDiskStyle)==0 )
		iDiskCount = PART_NUM_SDISK;
	else if( strcmp("HDisk", v_szDiskStyle)==0 )
		iDiskCount = PART_NUM_HDISK;
	else if( strcmp("UDisk", v_szDiskStyle)==0 )
		iDiskCount = PART_NUM_UDISK;
	
	for(i=1; i<=iDiskCount; i++)
	{
		sprintf( szTestFile, "/mnt/%s/part%d/data/DiskFile", v_szDiskStyle, i );
		if( (iFdisk = open(szTestFile, O_RDWR | O_CREAT)) == -1 || write(iFdisk, "123456789", 10) != 10 || close(iFdisk) == -1 )
			return -1;
	}

	return 0;
}
