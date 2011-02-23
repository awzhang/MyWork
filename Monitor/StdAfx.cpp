#include "StdAfx.h"

CDog g_objDog;

int SYSTEM(char *v_szFormat, ...)
{
	char szCmd[1024] = {0};
	va_list va = NULL;
		
	va_start(va, v_szFormat);
	vsprintf(szCmd, v_szFormat, va);
	va_end(va);
	
	return system(szCmd);
}

bool AllAppExist(char v_szPart)
{
	int i, j;
	FILE *fpSrc = NULL;
	byte bytFileCnt = 0;
	size_t iFileSize = 0;
	char szDesDir[40] = {0};
	char szDesFile[40] = {0};
				
	int iFileNum = 0;
	
	DWORD dwSrcChkSum = 0, dwChkSum = 0;
	char szSrcFileData;
	int  iSrcFileSize = 0;
				
	switch(v_szPart)
	{
		case '2':
			{
				if(access("/SockServExe", F_OK) == 0 && access("/UpdateExe", F_OK) == 0 
				&& access("/IOExe", F_OK) == 0 && access("/ComuExe", F_OK) == 0 
				&& access("/QianExe", F_OK) == 0
				&& access("/libComuServBkp.so", F_OK) == 0)
				{
					return true;
				}
			}
			break;
		case '3':
			{
				// 进行解包处理, 压缩包格式：文件个数(1)＋【文件目录(40) + 文件名称(40) + 文件长度(4) + 文件数据(n)】* N + 校验和(4)
				if((fpSrc = fopen("/mnt/Flash/part3/Down/exe.bin", "rb+")) == NULL)
				{
					PRTMSG(MSG_ERR, "Open Updfile Fail\n");
					break;
				}

				fseek(fpSrc, 0, SEEK_END);
				iSrcFileSize = ftell(fpSrc);
				fseek(fpSrc, 0, SEEK_SET);
				
				for(j = 0; j < iSrcFileSize - 4; j++)
				{
					if(1 != fread((void*)&szSrcFileData, 1, 1, fpSrc))
					{
						PRTMSG(MSG_ERR, "Read Updfile Data Fail\n");
						if(fpSrc)
						{
							fclose(fpSrc);
							fpSrc = NULL;
						}
						return false;
					}
					
					dwChkSum += (BYTE)szSrcFileData;
				}
				
				if(4 != fread((void*)&dwSrcChkSum, 1, 4, fpSrc))
				{
					PRTMSG(MSG_ERR, "Read Updfile ChkSum Fail\n");
					break;
				}
				
				if(dwChkSum != dwSrcChkSum)
				{
					PRTMSG(MSG_ERR, "Updfile is damaged\n");
					break;
				}
				
				fseek(fpSrc, 0, SEEK_SET);

				if(1 != fread((void*)&bytFileCnt, 1, 1, fpSrc))	// 文件个数
				{
					PRTMSG(MSG_ERR, "Read Updfile Number Fail\n");
					break;
				}
				
				for(i = 0; i < bytFileCnt; i++)
				{
					memset(szDesDir, 0, sizeof(szDesDir));
					memset(szDesFile, 0, sizeof(szDesFile));
					
					if(40 != fread(szDesDir, 1, 40, fpSrc) || 40 != fread(szDesFile, 1, 40, fpSrc))
					{
						PRTMSG(MSG_ERR, "Read Updfile Path Fail\n");
						break;
					}
					else
					{
						if(strcmp(szDesDir, "/mnt/Flash/part5/") == 0 
					  && (strcmp(szDesFile, "DvrExe") == 0 || strcmp(szDesFile, "SockServExe") == 0 
					   || strcmp(szDesFile, "IOExe") == 0 || strcmp(szDesFile, "UpdateExe") == 0 
					   || strcmp(szDesFile, "QianExe") == 0 || strcmp(szDesFile, "ComuExe") == 0 
					   || strcmp(szDesFile, "libComuServ.so") == 0))
					  {
					  	iFileNum++;
					  }
					  
					  if(4 != fread((void*)&iFileSize, 1, 4, fpSrc) || 0 != fseek(fpSrc, iFileSize, SEEK_CUR))
					  {
					  	PRTMSG(MSG_ERR, "Read Updfile Size Fail\n");
							break;
					  }
					}
				}
				
				if(iFileNum == 7)
				{
					if(fpSrc)
					{
						fclose(fpSrc);
						fpSrc = NULL;
					}
					return true;
				}
			}
			break;
		case '5':
			{
				if(access("/mnt/Flash/part5/DvrExe", F_OK) == 0 && access("/mnt/Flash/part5/SockServExe", F_OK) == 0 
				&& access("/mnt/Flash/part5/IOExe", F_OK) == 0 && access("/mnt/Flash/part5/UpdateExe", F_OK) == 0 
				&& access("/mnt/Flash/part5/QianExe", F_OK) == 0 && access("/mnt/Flash/part5/ComuExe", F_OK) == 0 
				&& access("/mnt/Flash/part5/libComuServ.so", F_OK) == 0)
				{
					return true;
				}
			}
			break;
		default:
			break;
	}

	if(fpSrc)
	{
		fclose(fpSrc);
		fpSrc = NULL;
	}
	return false;
}

bool G_ExeUpdate(char *v_szFileName, int v_iFlag)
{
	byte bytFileCnt = 0;
	bool bUpdateSucc = false;	// 缺省为升级失败
	
	FILE *fpSrc = NULL;
	FILE *fpDes = NULL;
	size_t iFileSize = 0;
	char *szBuf = NULL;
	char szDesFileToDir[40] = {0};
	char szDesFileToName[40] = {0};
	char szDesFileFromPath[80] = {0};
	char szDesFileToPath[80] = {0};

	if( (fpSrc = fopen(v_szFileName, "rb+"))==NULL )
	{
		PRTMSG(MSG_ERR, "Open updfile failed:");
		perror(" ");
		goto _UPDATE_END;
	}
	
	// 进行解包处理，压缩包格式：文件个数（1）＋【文件目录(40) + 文件名称(40) + 文件长度(4) + 文件数据(n)】* N + 校验和(4)
	if( 1 != fread((void*)&bytFileCnt, 1, 1, fpSrc) )	// 文件个数
		goto _UPDATE_END;
			
	for(int i = 0; i < bytFileCnt; i++)
	{
		if( 40 != fread(szDesFileToDir, 1, 40, fpSrc) || 40 != fread(szDesFileToName, 1, 40, fpSrc) || 4 != fread((void*)&iFileSize, 1, 4, fpSrc) )
			goto _UPDATE_END;
				
		if( NULL == (szBuf = (char*)malloc(iFileSize)) )
		{
			PRTMSG(MSG_ERR, "Malloc buffer failed");
			goto _UPDATE_END;
		}
				
		if( iFileSize != fread(szBuf, 1, iFileSize, fpSrc) )
		{
			PRTMSG(MSG_ERR, "Read updfile failed");
			goto _UPDATE_END;
		}
				
		sprintf(szDesFileFromPath, "/mnt/%s/UpdateFile", LOCAL_SAVE);
		sprintf(szDesFileToPath, "%s%s", szDesFileToDir, szDesFileToName);
				
		if( v_iFlag==0 )//Memory升级
		{
			if( unlink(szDesFileToPath)==-1 )
			{
				PRTMSG(MSG_ERR, "Delete %s fail\n", szDesFileToPath);
			}
			
			if( (fpDes = fopen(szDesFileToPath, "wb+"))==NULL )
			{
				PRTMSG(MSG_ERR, "Open %s failed", szDesFileToPath);
				goto _UPDATE_END;
			}
					
			if( iFileSize != fwrite(szBuf, 1, iFileSize, fpDes) )
			{
				PRTMSG(MSG_ERR, "Write %s failed", szDesFileToPath);
				goto _UPDATE_END;
			}
					
			if(szBuf)
			{
				free(szBuf);
				szBuf = NULL;
			}
					
			if(fpDes)
			{
				fclose(fpDes);
				fpDes = NULL;
			}
		}
		else if( v_iFlag==1 )//Flash升级
		{
			if( (fpDes=fopen(szDesFileFromPath, "wb+"))==NULL )
			{
				PRTMSG(MSG_ERR, "Open %s failed", szDesFileFromPath);
				goto _UPDATE_END;
			}
					
			if( iFileSize != fwrite(szBuf, 1, iFileSize, fpDes) )
			{
				PRTMSG(MSG_ERR, "Write %s failed", szDesFileFromPath);
				goto _UPDATE_END;
			}
					
			if(szBuf)
			{
				free(szBuf);
				szBuf = NULL;
			}
					
			if(fpDes)
			{
				fclose(fpDes);
				fpDes = NULL;
			}
					
			if( unlink(szDesFileToPath)==-1 )
			{
				PRTMSG(MSG_ERR, "Delete %s fail\n", szDesFileToPath);
			}
					
			// 替换原文件
			if( SYSTEM("cp -f %s %s", szDesFileFromPath, szDesFileToPath)!=0 )
			{
				PRTMSG(MSG_ERR, "Copy %s failed", szDesFileToPath);
				goto _UPDATE_END;
			}
		}
				
		// 更改权限
		if( chmod(szDesFileToPath, S_IRWXU|S_IRWXG|S_IRWXO) )
		{
			PRTMSG(MSG_ERR, "Chmod %s failed", szDesFileToPath);
			goto _UPDATE_END;
		}

		if( v_iFlag==1 && unlink(szDesFileFromPath)==-1 )	
		{
			PRTMSG(MSG_ERR, "Delete %s fail\n", szDesFileFromPath);
		}
	}
	
	bUpdateSucc = true;
	
_UPDATE_END:
	if(szBuf)
	{
		free(szBuf);
		szBuf = NULL;
	}
	
	if(fpSrc)
	{
		fclose(fpSrc);
		fpSrc = NULL;
	}
	
	if(fpDes)
	{
		fclose(fpDes);
		fpDes = NULL;
	}
	
	//unlink(szDesFileFromPath);
	
	return bUpdateSucc;
}

int CreateLog()
{
	int i, j;
	FILE *pfErrLog = NULL;
	
	for(i = 0; i < 3; i++)
	{
		if(access(ErrLog, F_OK))
		{
			if(!(pfErrLog = fopen(ErrLog, "w+")))
			{
				PRTMSG(MSG_ERR, "Create ErrLog Fail\n");
				sleep(1);
				continue;
			}
			else
			{
				for(j = 0; j < 3; j++)
				{
					if(sizeof(InitLog) != fwrite(InitLog, 1, sizeof(InitLog), pfErrLog))
					{
						PRTMSG(MSG_ERR, "Write ErrLog Fail\n");
						fseek(pfErrLog, 0, SEEK_SET);
						sleep(1);
						continue;
					}

					fclose(pfErrLog);
					return 0;
				}
				
				fclose(pfErrLog);
				unlink(ErrLog);
				sleep(1);
				continue;
			}
		}
		
		return 0;
	}

	return -1;
}

int DeleteLog()
{
	int i;

	for(i = 0; i < 3; i++)
	{
		if(!access(ErrLog, F_OK))
		{
			if(unlink(ErrLog))
			{
				PRTMSG(MSG_ERR, "Delete ErrLog Fail\n");
				sleep(1);
				continue;
			}
			
			return 0;
		}
		
		return 0;
	}

	return -1;
}

int ReadLog(char &v_szPart, char &v_szError, char &v_szUpdate)
{
	int i, j;
	FILE *pfErrLog = NULL;
	char szBuf[24] = {0};
	
	for(i = 0; i < 3; i++)
	{
		if(!access(ErrLog, F_OK))
		{
			if(!(pfErrLog = fopen(ErrLog, "r+")))
			{
				PRTMSG(MSG_ERR, "Open ErrLog Fail\n");
				sleep(1);
				continue;
			}
			else
			{
				for(j = 0; j < 3; j++)
				{
					if(sizeof(szBuf) != fread(szBuf, 1, sizeof(szBuf), pfErrLog))
					{
						PRTMSG(MSG_ERR, "Read ErrLog Fail\n");
						fseek(pfErrLog, 0, SEEK_SET);
						sleep(1);
						continue;
					}
					
					v_szPart = szBuf[5];
					v_szError = szBuf[13];
					v_szUpdate = szBuf[22];
					
					fclose(pfErrLog);
					return 0;
				}
				
				fclose(pfErrLog);
				sleep(1);
				continue;
			}
		}
		
		return -1;
	}

	return -1;
}

int WriteLog(char &v_szPart, char &v_szError, char &v_szUpdate)
{
	int i, j;
	FILE *pfErrLog = NULL;
	char szBuf[24] = {0};
	
	for(i = 0; i < 3; i++)
	{
		if(!access(ErrLog, F_OK))
		{
			if(!(pfErrLog = fopen(ErrLog, "r+")))
			{
				PRTMSG(MSG_ERR, "Open ErrLog Fail\n");
				sleep(1);
				continue;
			}
			else
			{
				for(j = 0; j < 3; j++)
				{
					sprintf(szBuf, "part:%c error:%c update:%c", v_szPart, v_szError, v_szUpdate);
					
					if(sizeof(szBuf) != fwrite(szBuf, 1, sizeof(szBuf), pfErrLog))
					{
						PRTMSG(MSG_ERR, "Write ErrLog Fail\n");
						fseek(pfErrLog, 0, SEEK_SET);
						sleep(1);
						continue;
					}

					fclose(pfErrLog);
					return 0;
				}
				
				fclose(pfErrLog);
				sleep(1);
				continue;
			}
		}
		
		return -1;
	}

	return -1;
}
