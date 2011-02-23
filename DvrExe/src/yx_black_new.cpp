#include "yx_common.h"

#if VIDEO_BLACK_TYPE == 2

int UPLOAD_PACK_SIZE;

pthread_t g_pthreadBlack;
UpLoadFileSta g_objUploadFileSta;
BYTE	g_szReUploadBuf[100] = {0};
int		g_iReUploadLen = 0;

// 通道号（1）+ 日期（3）
int Deal3809(char *v_databuf, int v_datalen)
{
	BYTE	szResBuf[TCP_SENDSIZE] = {0};
	int		iResLen = 0;
	BYTE	szResType = 1;

	BYTE	bytChn = 0;
	char	szQueryDate[11] = {0};

	FILE_INDEX objAVFileIndex;
	struct dirent **namelist;
	struct stat objStat;
	int		i = 0, j = 0, k = 0, n = 0;
	char	szChnPath[255] = {0};
	char	szDatePath[255] = {0};
	char	szFileName[255] = {0};
	char	szTempBuf[3] = {0};
	FILE	*pListFile = NULL;
	int		iCount = 0;

	PRTMSG(MSG_DBG, "Recv 3809 query request!\n");

	// 检查参数长度
	if( v_datalen != 4 && v_datalen != 1)
	{
		PRTMSG(MSG_ERR, "3809 uncorrect len\n");
		szResType = 2;
		goto DEAL3809_END;
	}

	// 通道号
	bytChn = (byte)v_databuf[0];

	// 若数据字段是4个字节，说明有带查询日期
	if( v_datalen == 4 )
	{
		// 查询日期
		sprintf(szQueryDate, "%04d-%02d-%02d", 2000+v_databuf[1], v_databuf[2], v_databuf[3]);
		PRTMSG(MSG_DBG, "Query Chn, %02x, Date: %s\n", bytChn, szQueryDate);

		for(i=0; i<4; i++)
		{
			j = 0x01<<i;

			// 若要查询第i个通道
			if( bytChn & j )
			{
				memset(szDatePath, 0, sizeof(szDatePath));
				sprintf(szDatePath, "/mnt/%s/part1/data/chn%d/%s", g_objMvrCfg.AVRecord.DiskType, i+1, szQueryDate);
				
				// 查看该目录是否存在
				if(access(szDatePath, F_OK) != 0)
				{
					PRTMSG(MSG_ERR, "%s is not exist\n", szDatePath);
					continue;
				}

				// 打开该目录的索引文件
				memset(szFileName,0, sizeof(szFileName));
				sprintf(szFileName, "%s/%s.list", szDatePath, szQueryDate);
				if( (pListFile = fopen(szFileName, "rb+")) == NULL )
				{
					PRTMSG(MSG_ERR, "open %s failed\n", szFileName);
					continue;
				}

				// 读取索引文件中的所有记录
				while( fread(&objAVFileIndex, sizeof(FILE_INDEX), 1, pListFile) )
				{
					szResBuf[9+iCount*9] = v_databuf[1];	// 年

					szResBuf[10+iCount*9] = v_databuf[2];	// 月

					szResBuf[11+iCount*9] = v_databuf[3];	// 日

					memcpy(szTempBuf,objAVFileIndex.StartTime[0],2);
					szResBuf[12+iCount*9] = (BYTE)(atoi(szTempBuf));	//起始时
					
					memcpy(szTempBuf,objAVFileIndex.StartTime[1],2);
					szResBuf[13+iCount*9] = (BYTE)(atoi(szTempBuf));	//起始分
					
					memcpy(szTempBuf,objAVFileIndex.StartTime[2],2);
					szResBuf[14+iCount*9] = (BYTE)(atoi(szTempBuf));	//起始秒

					memcpy(szTempBuf,objAVFileIndex.EndTime[0],2);
					szResBuf[15+iCount*9] = (BYTE)(atoi(szTempBuf));	//结束时
					
					memcpy(szTempBuf,objAVFileIndex.EndTime[1],2);
					szResBuf[16+iCount*9] = (BYTE)(atoi(szTempBuf));	//结束分
					
					memcpy(szTempBuf,objAVFileIndex.EndTime[2],2);
					szResBuf[17+iCount*9] = (BYTE)(atoi(szTempBuf));	//结束秒

					// 时间段个数加1
					iCount++;
				}

				if(pListFile)
				{
					fclose(pListFile);
					pListFile = NULL;
				}
			}
		}
	}

	// 若只有一个字节，则说明要查询所有的时间段
	if( v_datalen == 1 )
	{
		for(i=0; i<4; i++)
		{
			j = 0x01<<i;
			
			// 若要查询第i个通道
			if( bytChn & j )
			{
				memset(szChnPath, 0, sizeof(szChnPath));
				sprintf(szChnPath, "/mnt/%s/part1/data/chn%d", g_objMvrCfg.AVRecord.DiskType, i+1);
				
				// 查看该目录是否存在
				if(access(szChnPath, F_OK) != 0)
				{
					PRTMSG(MSG_ERR, "%s is not exist\n", szDatePath);
					continue;
				}

				// 查询该目录下的所有子目录
				n = scandir(szChnPath, &namelist, 0, alphasort);
				for(k=0; k<n; k++)
				{
					if( 0 == strcmp(".", namelist[k]->d_name) || 0 == strcmp("..", namelist[k]->d_name) )
							continue;

					memset(szDatePath, 0, sizeof(szDatePath));
					sprintf(szDatePath, "%s/%s", szChnPath, namelist[k]->d_name);

					if( 0 != stat(szDatePath, &objStat) )
					{
						PRTMSG(MSG_ERR, "stat err\n");
						perror("");
						continue;
					}

					// 找到一个子目录
					if(S_ISDIR(objStat.st_mode))
					{
						// 打开该子目录的索引文件
						memset(szFileName,0, sizeof(szFileName));
						sprintf(szFileName, "%s/%s.list", szDatePath, namelist[k]->d_name);
						if( (pListFile = fopen(szFileName, "rb+")) == NULL )
						{
							PRTMSG(MSG_ERR, "open %s failed\n", szFileName);
							continue;
						}
						
						// 读取索引文件中的所有记录
						while( fread(&objAVFileIndex, sizeof(FILE_INDEX), 1, pListFile) )
						{
							memcpy(szTempBuf,namelist[k]->d_name+2,2);
							szResBuf[9+iCount*9] = (BYTE)(atoi(szTempBuf));	//年

							memcpy(szTempBuf,namelist[k]->d_name+5,2);
							szResBuf[10+iCount*9] = (BYTE)(atoi(szTempBuf));	//月

							memcpy(szTempBuf,namelist[k]->d_name+8,2);
							szResBuf[11+iCount*9] = (BYTE)(atoi(szTempBuf));	//年
							
							memcpy(szTempBuf,objAVFileIndex.StartTime[0],2);
							szResBuf[12+iCount*9] = (BYTE)(atoi(szTempBuf));	//起始时
							
							memcpy(szTempBuf,objAVFileIndex.StartTime[1],2);
							szResBuf[13+iCount*9] = (BYTE)(atoi(szTempBuf));	//起始分
							
							memcpy(szTempBuf,objAVFileIndex.StartTime[2],2);
							szResBuf[14+iCount*9] = (BYTE)(atoi(szTempBuf));	//起始秒
							
							memcpy(szTempBuf,objAVFileIndex.EndTime[0],2);
							szResBuf[15+iCount*9] = (BYTE)(atoi(szTempBuf));	//结束时
							
							memcpy(szTempBuf,objAVFileIndex.EndTime[1],2);
							szResBuf[16+iCount*9] = (BYTE)(atoi(szTempBuf));	//结束分
							
							memcpy(szTempBuf,objAVFileIndex.EndTime[2],2);
							szResBuf[17+iCount*9] = (BYTE)(atoi(szTempBuf));	//结束秒
							
							// 时间段个数加1
							iCount++;
						}
						
						if(pListFile)
						{
							fclose(pListFile);
							pListFile = NULL;
						}
					}
				}
			}
		}
	}

	// 若未查到，则返回失败
	if( 0 == iCount)
		szResType = 2;
	else
	{
		PRTMSG(MSG_DBG, "Find %d Times: \n", iCount);
		for(int k=0; k<iCount; k++)
		{
			PRTMSG(MSG_DBG, "         %d. %04d-%02d-%02d  %02d:%02d:%02d ~ %02d:%02d:%02d \n", k, 2000+szResBuf[9+k*9], szResBuf[10+k*9], szResBuf[11+k*9], szResBuf[12+k*9], szResBuf[13+k*9], szResBuf[14+k*9], szResBuf[15+k*9], szResBuf[16+k*9], szResBuf[17+k*9]);
		}
	}

DEAL3809_END:
	szResBuf[iResLen++] = 0x01;		// 0x01 表示向中心发送的数据帧	
	szResBuf[iResLen++] = 0x38;
	szResBuf[iResLen++] = 0x49;
	szResBuf[iResLen++] = szResType;
	if( szResType == 1 )
	{
		szResBuf[iResLen++] = v_databuf[0];

		memcpy(szResBuf+iResLen, (void*)&iCount, 4);
		iResLen += 4;

		iResLen += 9*iCount;
	}
	
#if USE_VIDEO_TCP == 0
	DataPush(szResBuf, (DWORD)iResLen, DEV_DVR, DEV_QIAN, LV2);
#endif
	
#if USE_VIDEO_TCP == 1
	char szbuf[1024] = {0};
	int  ilen = 0;
	g_objSock.MakeSmsFrame((char*)szResBuf+3, iResLen-3, 0x38, 0x49, (char*)szbuf, sizeof(szbuf), ilen);
	g_objSock.SendTcpData(szbuf, ilen);
#endif
}

// 通道号（1）＋ 事件类型（1）＋ 起始日期（3）＋ 起始时间（3）＋ 结束日期（3）＋结束时间（3）
int Deal380A(char *v_databuf, int v_datalen)
{
	int		i = 0;
	BYTE	szResBuf[100] = {0};
	int		iResLen = 0;
	BYTE	szResType = 1;
	char	szTempBuf[3] = {0};
	
	BLACK_QUERY_PARA  objQueryPara;
	char szStartTime[9] = {0};
	char szEndTime[9] = {0};

	int  iFileCount = 0;

	// 若当前正在进行监控，则不支持黑匣子查询
	if(	g_objWorkStart.VUpload[0] || g_objWorkStart.VUpload[1] || g_objWorkStart.VUpload[2] || g_objWorkStart.VUpload[3] )
	{
		szResType = 5;
		goto DEAL380A_END;
	}

	memset((void*)&g_objUploadFileSta, 0, sizeof(g_objUploadFileSta));
	_KillTimer(&g_objTimerMng, C1C3_REUPLOAD_TIMER);

	iFileCount = g_objUploadFileSta.m_iFileCount;

	// 检查参数长度
	if( v_datalen != 14 )
	{
		PRTMSG(MSG_ERR, "380A uncorrect len\n");
		szResType = 2;
		goto DEAL380A_END;
	}

	// 通道号
	objQueryPara.m_uszChnNo = v_databuf[0];

	// 事件类型
	objQueryPara.m_uszEventType = v_databuf[1];

	// 日期（由中心限定来查询日期只能在同一天）
	sprintf(objQueryPara.m_szDate, "%04d-%02d-%02d", 2000+v_databuf[2], v_databuf[3], v_databuf[4]);

	// 起始时间
	sprintf(objQueryPara.m_szStartTime, "%02d:%02d:%02d", v_databuf[5], v_databuf[6], v_databuf[7]);
	sprintf(szStartTime, "%02d:%02d:%02d", v_databuf[5], v_databuf[6], v_databuf[7]);

	// 结束时间
	sprintf(objQueryPara.m_szEndTime, "%02d:%02d:%02d", v_databuf[11], v_databuf[12], v_databuf[13]);
	sprintf(szEndTime, "%02d:%02d:%02d", v_databuf[11], v_databuf[12], v_databuf[13]);

	// 查询
	iFileCount = QueryBlackData(objQueryPara, szStartTime, szEndTime, iFileCount);

	if( 0 < iFileCount )
	{
		PRTMSG(MSG_NOR, "Query video black succ, Find %d files\n", iFileCount);

		g_objUploadFileSta.m_iFileCount += iFileCount;
		szResType = 1;
	}
	else if( 0 == iFileCount )
	{
		PRTMSG(MSG_DBG, "Query black failed, No such file\n");
		szResType = 3;
	}
	else
	{
		PRTMSG(MSG_DBG, "Query black failed, unknown reason\n");
		szResType = 4;
	}

	for(i=0; i<g_objUploadFileSta.m_iFileCount; i++)
	{
		PRTMSG(MSG_DBG, "The %d file:\n", i);
		PRTMSG(MSG_DBG, "            TempName:%s\n", g_objUploadFileSta.m_szTempFileName[i]);
		PRTMSG(MSG_DBG, "            RealName:%s\n", g_objUploadFileSta.m_szRealFileName[i]);
		PRTMSG(MSG_DBG, "            Start: %d\n", g_objUploadFileSta.m_ulStartOffset[i]);
		PRTMSG(MSG_DBG, "            End:   %d\n", g_objUploadFileSta.m_ulEndOffset[i]);
	}

DEAL380A_END:
	szResBuf[iResLen++] = 0x01;		// 0x01 表示向中心发送的数据帧

	szResBuf[iResLen++] = 0x38;
	szResBuf[iResLen++] = 0x4A;
	szResBuf[iResLen++] = szResType;
	if( szResType == 1)
	{
		szResBuf[iResLen++] = objQueryPara.m_uszEventType;	// 事件类型
		szResBuf[iResLen++] = v_databuf[2];		// 年
		szResBuf[iResLen++] = v_databuf[3];		// 月
		szResBuf[iResLen++] = v_databuf[4];		// 日
		
		memcpy(szTempBuf,szStartTime,2);
		szResBuf[iResLen++] = (BYTE)(atoi(szTempBuf));	//起始时

		memcpy(szTempBuf,szStartTime+3,2);
		szResBuf[iResLen++] = (BYTE)(atoi(szTempBuf));	//起始分

		memcpy(szTempBuf,szStartTime+6,2);
		szResBuf[iResLen++] = (BYTE)(atoi(szTempBuf));	//起始秒

		szResBuf[iResLen++] = v_databuf[2];		// 年
		szResBuf[iResLen++] = v_databuf[3];		// 月
		szResBuf[iResLen++] = v_databuf[4];		// 日

		memcpy(szTempBuf,szEndTime,2);
		szResBuf[iResLen++] = (BYTE)(atoi(szTempBuf));	//终止时

		memcpy(szTempBuf,szEndTime+3,2);
		szResBuf[iResLen++] = (BYTE)(atoi(szTempBuf));	//终止分

		memcpy(szTempBuf,szEndTime+6,2);
		szResBuf[iResLen++] = (BYTE)(atoi(szTempBuf));	//终止秒

		for(i=iResLen-6; i<iResLen; i++)
			if( 0 == szResBuf[i] )
				szResBuf[i] = 0x7f;
	}
	
#if USE_VIDEO_TCP == 0
	DataPush(szResBuf, (DWORD)iResLen, DEV_DVR, DEV_QIAN, LV2);
#endif

#if USE_VIDEO_TCP == 1
	char szbuf[1024] = {0};
	int  ilen = 0;
	g_objSock.MakeSmsFrame((char*)szResBuf+3, iResLen-3, 0x38, 0x4A, (char*)szbuf, sizeof(szbuf), ilen);
	g_objSock.SendTcpData(szbuf, ilen);
#endif

	// 应答后，则开始上传黑匣子文件
	sleep(2);
	BeginUploadFile();
	
	return (int)szResType;
}

// 查询文件函数，若找到相应时间段的文件，则返回所找到的文件个数
int QueryBlackData(	BLACK_QUERY_PARA v_objQueryPara, char *v_szStartTime, char *v_szEndTime, int v_iFileCnt)
{
	int i=0, j=0;
	
	FILE *pListFile = NULL;
	FILE *pIndexFile = NULL;
	
	char szDatePath[255] = {0};
	char szFileName[255] = {0};

	char szFileStartTime[9] = {0};
	char szFileEndTime[9] = {0};
	char szFindStartTime[9] = {0};
	char szFindEndTime[9] = {0};

	int  iVideoStartOffset = 0;
	int  iAudioStartOffset = 0;
	int	iIndexStartOffset = 0;
	int  iVideoEndOffset = 0;
	int  iAudioEndOffset = 0;
	int	iIndexEndOffset = 0;

	FILE_INDEX objAVFileIndex;
	FRAME_INDEX objAVFrameIndex;
	
	for(i=0; i<4; i++)
	{
		j = 0x01<<i;

		// 若要查询第i个通道
		if( v_objQueryPara.m_uszChnNo & j )
		{
			PRTMSG(MSG_DBG, "Query Chn %02x\n", v_objQueryPara.m_uszChnNo);

			memset(szDatePath, 0, sizeof(szDatePath));
			sprintf(szDatePath, "/mnt/%s/part1/data/chn%d/%s", g_objMvrCfg.AVRecord.DiskType, i+1, v_objQueryPara.m_szDate);
			
			// 查看该目录是否存在
			if(access(szDatePath, F_OK) != 0)
			{
				PRTMSG(MSG_ERR, "%s is not exist\n", szDatePath);
				continue;
			}

			// 打开该目录的索引文件
			memset(szFileName,0, sizeof(szFileName));
			sprintf(szFileName, "%s/%s.list", szDatePath, v_objQueryPara.m_szDate);
			if( (pListFile = fopen(szFileName, "rb+")) == NULL )
			{
				PRTMSG(MSG_ERR, "open %s failed\n", szFileName);
				continue;
			}

			// 对每个索引文件都进行判断
			while( fread(&objAVFileIndex, sizeof(FILE_INDEX), 1, pListFile) )
			{
				sprintf(szFileStartTime, "%s:%s:%s", objAVFileIndex.StartTime[0], objAVFileIndex.StartTime[1], objAVFileIndex.StartTime[2]);
				sprintf(szFileEndTime, "%s:%s:%s", objAVFileIndex.EndTime[0], objAVFileIndex.EndTime[1], objAVFileIndex.EndTime[2]);

				// 若该索引文件的时间段与所要查询的黑匣子时间段有交集
				if(
					(strcmp(szFileStartTime, v_objQueryPara.m_szStartTime) >= 0 && strcmp(szFileStartTime, v_objQueryPara.m_szEndTime) <= 0) ||
					(strcmp(szFileStartTime, v_objQueryPara.m_szStartTime) <= 0 && strcmp(szFileEndTime, v_objQueryPara.m_szStartTime) >= 0)
			  	)
				{
					iVideoStartOffset = 0;
					iAudioStartOffset = 0;
					iIndexStartOffset = 0;
					iVideoEndOffset = 0;
					iAudioEndOffset = 0;
					iIndexEndOffset = 0;

					// 选择交集部分
					if(strcmp(szFileStartTime, v_objQueryPara.m_szStartTime) < 0) 
						memcpy(szFileStartTime, v_objQueryPara.m_szStartTime, 9);
					if(strcmp(szFileEndTime, v_objQueryPara.m_szEndTime) > 0)
						memcpy(szFileEndTime, v_objQueryPara.m_szEndTime,9);

					// 打开索引文件
					memset(szFileName, 0, sizeof(szFileName));
					sprintf(szFileName, "/mnt/%s/part1/data/%s", g_objMvrCfg.AVRecord.DiskType,objAVFileIndex.IPath);
					if( NULL == (pIndexFile = fopen(szFileName, "rb+")) )
					{
						PRTMSG(MSG_ERR, "open index file failed: \n");
						continue;
					}

					for( j=0; fread(&objAVFrameIndex, 1, sizeof(FRAME_INDEX), pIndexFile); j++)
					{
						// 视频、音频、索引数据在各自文件中的起始位置
						if( strcmp(objAVFrameIndex.CurTime, szFileStartTime) == 0 )
						{
							iVideoStartOffset = objAVFrameIndex.VStartOffset;
							iAudioStartOffset = objAVFrameIndex.AStartOffset;
							iIndexStartOffset = j*sizeof(FRAME_INDEX);
						}

						// 结束位置
						if( strcmp(objAVFrameIndex.CurTime, szFileEndTime) == 0 )
						{
							iVideoEndOffset = objAVFrameIndex.VEndOffset;
							iAudioEndOffset = objAVFrameIndex.AEndOffset;
							iIndexEndOffset = j*sizeof(FRAME_INDEX);
							break;
						}
					}

					//if( 0 == iVideoEndOffset || 0 == iAudioEndOffset || 0 == iIndexEndOffset )
					if( 0 == iVideoEndOffset || 0 == iIndexEndOffset )
					{
						if(pIndexFile)
						{
							fclose(pIndexFile);
							pIndexFile = NULL;
						}
						continue;
					}

					//关闭索引文件
					if(pIndexFile)
					{
						fclose(pIndexFile);
						pIndexFile = NULL;
					}

					// 把“:”改成“－”
					szFileStartTime[2] = szFileStartTime[5] = '-';
					szFileEndTime[2] = szFileEndTime[5] = '-';

					// 记录相关信息（临时文件名、正式文件名、起始位置、结束位置）
					// 以相应的时间段来命名临时文件名，该临时文件并不存在，只是在上传时会用到该临时文件名
#if USE_PROTOCOL == 0
					sprintf(g_objUploadFileSta.m_szTempFileName[v_iFileCnt], "%d_%s_%s_%s.ind", i, v_objQueryPara.m_szDate, szFileStartTime, szFileEndTime);
					sprintf(g_objUploadFileSta.m_szRealFileName[v_iFileCnt], "/mnt/%s/part1/data/%s", g_objMvrCfg.AVRecord.DiskType, objAVFileIndex.IPath);
					g_objUploadFileSta.m_ulStartOffset[v_iFileCnt] = iIndexStartOffset;
					g_objUploadFileSta.m_ulEndOffset[v_iFileCnt] = iIndexEndOffset;
					v_iFileCnt++;
#endif

					sprintf(g_objUploadFileSta.m_szTempFileName[v_iFileCnt], "%d_%s_%s_%s.264", i, v_objQueryPara.m_szDate, szFileStartTime, szFileEndTime);
					sprintf(g_objUploadFileSta.m_szRealFileName[v_iFileCnt], "/mnt/%s/part1/data/%s", g_objMvrCfg.AVRecord.DiskType, objAVFileIndex.VPath);
					g_objUploadFileSta.m_ulStartOffset[v_iFileCnt] = iVideoStartOffset;
					g_objUploadFileSta.m_ulEndOffset[v_iFileCnt] = iVideoEndOffset;
					v_iFileCnt++;


#if USE_PROTOCOL == 0
					if( 0 != iAudioEndOffset )
					{
						sprintf(g_objUploadFileSta.m_szTempFileName[v_iFileCnt], "%d_%s_%s_%s.726", i, v_objQueryPara.m_szDate, szFileStartTime, szFileEndTime);
						sprintf(g_objUploadFileSta.m_szRealFileName[v_iFileCnt], "/mnt/%s/part1/data/%s", g_objMvrCfg.AVRecord.DiskType, objAVFileIndex.APath);
						g_objUploadFileSta.m_ulStartOffset[v_iFileCnt] = iAudioStartOffset;
						g_objUploadFileSta.m_ulEndOffset[v_iFileCnt] = iAudioEndOffset;
						v_iFileCnt++;
					}
#endif
					// 更新所查询到的时间段（保存所找到的最小的开始时间和最大的结束时间）
					if( strcmp(szFindStartTime, szFileStartTime) < 0 )
						memcpy(szFindStartTime, szFileStartTime, 9);
					if( strcmp(szFileEndTime, szFindEndTime) > 0 )
						memcpy(szFindEndTime, szFileEndTime, 9);

					// 若已找到足够的文件，则返回
					if( v_iFileCnt >= MAX_UPLOAD_FILE_COUNT )
						goto QUERY_BLACK_END;				
				}
			}

			if(pListFile)
			{
				fclose(pListFile);
				pListFile = NULL;
			}
		}
	}

QUERY_BLACK_END:
	
	if( v_iFileCnt > g_objUploadFileSta.m_iFileCount )	// 若有找到文件
	{
		// 将所查询到的时间段返回
		if( strcmp(v_szStartTime, szFindStartTime) < 0 ) 
			memcpy(v_szStartTime, szFindStartTime, 9);
		if( strcmp(v_szEndTime, szFindEndTime) > 0 )
			memcpy(v_szEndTime, szFindEndTime, 9);
	}

	return (v_iFileCnt - g_objUploadFileSta.m_iFileCount );
}

void BeginUploadFile()
{
	int	i = 0;
	int	iLen = 0;
	
	int iCurDateTime[6];

	BYTE	szTempBuf[100] = {0};
	int  iTempLen = 0;

	// 若当前正在监控，则返回
	if(	g_objWorkStart.VUpload[0] || g_objWorkStart.VUpload[1] || g_objWorkStart.VUpload[2] || g_objWorkStart.VUpload[3] )
		return;

	// 更新上传文件的状态
	if( FALSE == UpdateUploadSta(FALSE) ) 
		return;

	// 组帧（C1帧）
	szTempBuf[iTempLen++] = 0xC1;

	// 手机号
	tag1PComuCfg objComuCfg;
	GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );					
	memcpy(szTempBuf+iTempLen, objComuCfg.m_szTel, 15);
	iTempLen += 15;
	
	//文件类型
	szTempBuf[iTempLen++] = g_objUploadFileSta.m_uszFileType;		
	
	//此处黑匣子文件名固定长为34个字节，后面补空格，凑齐40个字节
	iLen = strlen(g_objUploadFileSta.m_szTempFileName[0]);
	memcpy( szTempBuf+iTempLen, g_objUploadFileSta.m_szTempFileName[0], iLen);	
	iTempLen += iLen;
	szTempBuf[iTempLen++] = ' ';szTempBuf[iTempLen++] = ' ';szTempBuf[iTempLen++] = ' ';	
	szTempBuf[iTempLen++] = ' ';szTempBuf[iTempLen++] = ' ';szTempBuf[iTempLen++] = ' ';
	
	// 时间序号
	GetCurDateTime( iCurDateTime );
	
	szTempBuf[iTempLen++] = (char)(iCurDateTime[0]-2000);
	szTempBuf[iTempLen++] = (char)iCurDateTime[1];
	szTempBuf[iTempLen++] = (char)iCurDateTime[2];
	szTempBuf[iTempLen++] = (char)iCurDateTime[3];
	szTempBuf[iTempLen++] = (char)iCurDateTime[4];
	szTempBuf[iTempLen++] = (char)iCurDateTime[5];
	
	// 总包数
	memcpy(szTempBuf+iTempLen, (void*)&g_objUploadFileSta.m_ulPackTotal, 8);
	iTempLen += 8;
		
	// 推到SockServExe发送
	DataPush(szTempBuf, (DWORD)iTempLen, DEV_DVR, DEV_SOCK, LV2);

	PRTMSG(MSG_DBG, "Upload C1 frame\n");
	//PrintString((char*)szTempBuf, iTempLen);

	// 保存到缓冲，以备重传
	memcpy(g_szReUploadBuf, szTempBuf, iTempLen);
	g_iReUploadLen = iTempLen;

 	// 启动超时重传定时器
	_SetTimer(&g_objTimerMng, C1C3_REUPLOAD_TIMER, 5000, Deal_ReUpload);
}

void Deal_ReUpload(void *arg, int len)
{
	if( g_szReUploadBuf[0] == 0xC1 )
		PRTMSG(MSG_DBG, "ReUpload C1 frame\n");
	if( g_szReUploadBuf[0] == 0xC3 )
		PRTMSG(MSG_DBG, "ReUpload C3 frame\n");

	//PrintString((char*)g_szReUploadBuf, g_iReUploadLen);

	DataPush(g_szReUploadBuf, (DWORD)g_iReUploadLen, DEV_DVR, DEV_SOCK, LV2);
}

BOOL UpdateUploadSta( BOOL v_bDeleteFirstFile )
{
	int	i=0;
	FILE	*fp = NULL;
	HI_U64  ulFileSize = 0;
	
	while(1)
	{
		if(g_objUploadFileSta.m_iFileCount <= 0)
			return FALSE;
		
		// 若要删除第一个文件
		if( v_bDeleteFirstFile )
		{
			g_objUploadFileSta.m_iFileCount--;
			
			if(g_objUploadFileSta.m_iFileCount <= 0)
				return FALSE;
			
			for(i=0; i<g_objUploadFileSta.m_iFileCount; i++)
			{
				memcpy(g_objUploadFileSta.m_szTempFileName[i],g_objUploadFileSta.m_szTempFileName[i+1], sizeof(g_objUploadFileSta.m_szTempFileName[i]) );
				memcpy(g_objUploadFileSta.m_szRealFileName[i],g_objUploadFileSta.m_szRealFileName[i+1], sizeof(g_objUploadFileSta.m_szRealFileName[i]) );
				g_objUploadFileSta.m_ulStartOffset[i] = g_objUploadFileSta.m_ulStartOffset[i+1];
				g_objUploadFileSta.m_ulEndOffset[i] = g_objUploadFileSta.m_ulEndOffset[i+1];
			}
		}
		
		// 打开当前要上传的正式文件	
		fp = fopen(g_objUploadFileSta.m_szRealFileName[0],"rb");
		
		if(NULL != fp)
		{
			// 判断文件大小是否为0
			fseek(fp, 0, SEEK_END);
			DWORD dwFileSize = ftell(fp);
			if( 0 == dwFileSize )
			{
				fclose(fp);
				fp = NULL;
				v_bDeleteFirstFile = TRUE;
				continue;
			}
			else
			{
				fseek(fp, 0, SEEK_SET);
			}

			// 判断文件类型
			if( NULL != strstr(g_objUploadFileSta.m_szTempFileName[0], ".264") )
				g_objUploadFileSta.m_uszFileType = 1;
			else if( NULL != strstr(g_objUploadFileSta.m_szTempFileName[0], ".726") )
				g_objUploadFileSta.m_uszFileType = 2;
			else if( NULL != strstr(g_objUploadFileSta.m_szTempFileName[0], ".ind") )
				g_objUploadFileSta.m_uszFileType = 3;
			else
			{
				fclose(fp);
				fp = NULL;
				v_bDeleteFirstFile = TRUE;
				continue;
			}

			// 计算当前要上传临时文件的总包数、总窗口数
			ulFileSize = g_objUploadFileSta.m_ulEndOffset[0] - g_objUploadFileSta.m_ulStartOffset[0];

			g_objUploadFileSta.m_ulPackTotal = ulFileSize / UPLOAD_PACK_SIZE;
			if( 0 != ulFileSize%UPLOAD_PACK_SIZE ) 
				g_objUploadFileSta.m_ulPackTotal++;

			g_objUploadFileSta.m_ulWinTotal = g_objUploadFileSta.m_ulPackTotal/UPLOAD_WINSIZE;
			if( 0 != g_objUploadFileSta.m_ulPackTotal%UPLOAD_WINSIZE )
				g_objUploadFileSta.m_ulWinTotal++;

			PRTMSG(MSG_DBG, "File: %s, PackTotal: %d\n", g_objUploadFileSta.m_szTempFileName[0], g_objUploadFileSta.m_ulPackTotal);

			g_objUploadFileSta.m_ulPackNo = 0;
			g_objUploadFileSta.m_ulWinNo = MAX_UNSIGNED_64;

			g_objUploadFileSta.m_iBufCount = 0;

			fclose(fp);
			fp = NULL;
			return TRUE;
		}
		
		// 若文件无法打开，则从队列中删除该文件
		v_bDeleteFirstFile = TRUE;
		if(NULL != fp)
		{
			fclose(fp);
			fp = NULL;
		}
	}
}

void *G_BlackUploadThread(void* arg)
{
	char	szSendBuf[1500] = {0};
	int		iSendLen = 0;
	HI_U64	ulPackNo = 0;
	int		ireadlen = 0;
	FILE	*fp = NULL;
	
	fp = fopen(g_objUploadFileSta.m_szRealFileName[0],"rb");
	if(NULL == fp)
		return NULL;

	tag1PComuCfg objComuCfg;
	GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );	

	// 将文件定位到所要上传的位置
	fseek(fp, g_objUploadFileSta.m_ulStartOffset[0], SEEK_SET);

	// 分包组帧上传
	for(ulPackNo=0; ulPackNo<g_objUploadFileSta.m_ulPackTotal; ulPackNo++)
	{
		// 包数据
		if( (ireadlen = fread(szSendBuf+C2_HEAD_SIZE, 1, UPLOAD_PACK_SIZE, fp)) < 0 )
			break;

		iSendLen = 0;

		// 帧头
		szSendBuf[iSendLen++] = 0xC2;
		
		// 手机号
		memcpy(szSendBuf+iSendLen, objComuCfg.m_szTel, 15);
		iSendLen += 15;
		
		// 文件类型
		szSendBuf[iSendLen++] = g_objUploadFileSta.m_uszFileType;
		
		// 文件名
		memcpy(szSendBuf+iSendLen, g_objUploadFileSta.m_szTempFileName[0], 40);
		iSendLen += 40;

		// 包序号
		memcpy(szSendBuf+iSendLen, (void*)&ulPackNo, 8);
		iSendLen += 8;

		// 包大小
		szSendBuf[iSendLen++] = ireadlen%256;
		szSendBuf[iSendLen++] = ireadlen/256;

		//推到SockServExe发送
		DataPush(szSendBuf, ireadlen+C2_HEAD_SIZE, DEV_DVR, DEV_SOCK, LV2);

		if( ulPackNo % 10 == 0)
		{
			PRTMSG(MSG_DBG, "Upload C2 frame, ulPackNo = %d\n", ulPackNo);
			usleep(200000);
		}
	}

	if( NULL != fp )
		fclose(fp);

	// 整个文件发送完毕之后，发送C3结束帧：0xC3 ＋ 手机号（15）＋ 文件类型（1）＋ 文件名（40）
	szSendBuf[0] = 0xC3;
	DataPush(szSendBuf, 57, DEV_DVR, DEV_SOCK, LV2);
	PRTMSG(MSG_DBG, "Upload C3 frame!\n");

	// 保存到缓冲，以备重传
	memcpy(g_szReUploadBuf, szSendBuf, 57);
	g_iReUploadLen = 57;
	
	// 启动超时重传定时器
	_SetTimer(&g_objTimerMng, C1C3_REUPLOAD_TIMER, 5000, Deal_ReUpload);
}

// 文件类型（1）＋ 文件名（40）＋ 时间序号（6）＋ 应答类型（1）
int  Deal38D1(char *v_databuf, int v_datalen)
{
	char	szFileName[40] = {0};
	FILE	*fp = NULL;
	int		iFileSize = 0;

	memcpy(szFileName, v_databuf+1, 34);
	PRTMSG(MSG_DBG, "Recv D1 frame:%s \n", szFileName);
	
	// 若文件名一致，则关闭C1重传定时器
	if( strcmp(szFileName, g_objUploadFileSta.m_szTempFileName[0]) == 0 )
	{
		PRTMSG(MSG_NOR, "Close C1_ReUpload Timer, Begin Upload...\n");		
		_KillTimer(&g_objTimerMng, C1C3_REUPLOAD_TIMER);
	}
	else
	{
		PRTMSG(MSG_NOR, "But Upload FileName is %s, quit!\n", g_objUploadFileSta.m_szTempFileName[0] );
		return 1;
	}

	// 文件检查
	fp = fopen(g_objUploadFileSta.m_szRealFileName[0], "rb");
	if( NULL != fp )
	{
		fseek(fp, 0, SEEK_END);
		iFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
	}
	if( 0 == iFileSize )
	{
		UpdateUploadSta(TRUE);
		BeginUploadFile();
		return 1;
	}
	if( NULL != fp )
		fclose(fp);

	// 创建文件上传线程
	pthread_create(&g_pthreadBlack, NULL, G_BlackUploadThread, NULL);	
	
	return 0;
}

// 文件类型（1）＋ 文件名（40）＋ 应答类型（1）+ 【请求重传的包个数（8） + 包序号（8）* M】
int  Deal38D3(char *v_databuf, int v_datalen)
{
	char	szFileName[40] = {0};
	FILE	*fp = NULL;
	HI_U64	ulPackCount = 0;
	HI_U64	ulPackNo = 0;
	HI_U64	i = 0;
	char	szSendBuf[1500] = {0};
	int		iSendLen = 0;
	int		ireadlen = 0;

	tag1PComuCfg objComuCfg;
	GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );	
	
	memcpy(szFileName, v_databuf+1, 34);
	PRTMSG(MSG_DBG, "Recv D3 frame:%s \n", szFileName);
	
	// 若文件名一致，则关闭C3重传定时器
	if( strcmp(szFileName, g_objUploadFileSta.m_szTempFileName[0]) == 0 )
	{
		PRTMSG(MSG_NOR, "Close C3_ReUpload Timer!\n");		
		_KillTimer(&g_objTimerMng, C1C3_REUPLOAD_TIMER);

		// 若文件接收完整
		if( v_databuf[41] == 0x01 )
		{
			UpdateUploadSta(TRUE);
			BeginUploadFile();
		}
		else
		{				
			fp = fopen(g_objUploadFileSta.m_szRealFileName[0],"rb");
			if(NULL == fp)
				return 1;

			// 请求重传包个数
			memcpy((void*)&ulPackCount, v_databuf+42, 8);

			for(i=0; i<ulPackCount; i++)
			{
				// 获取一个重传包序号
				memcpy((void*)&ulPackNo, v_databuf+50+8*i, 8);
				PRTMSG(MSG_DBG, "ReUpload Pack: %ld\n", ulPackNo);

				// 定位到文件中的该位置
				fseek(fp, g_objUploadFileSta.m_ulStartOffset[0]+ulPackNo*UPLOAD_PACK_SIZE, SEEK_SET );

				// 包数据
				if( (ireadlen = fread(szSendBuf+C2_HEAD_SIZE, 1, UPLOAD_PACK_SIZE, fp)) < 0 )
					break;

				iSendLen = 0;
				
				// 帧头
				szSendBuf[iSendLen++] = 0xC2;
				
				// 手机号
				memcpy(szSendBuf+iSendLen, objComuCfg.m_szTel, 15);
				iSendLen += 15;
				
				// 文件类型
				szSendBuf[iSendLen++] = g_objUploadFileSta.m_uszFileType;
				
				// 文件名
				memcpy(szSendBuf+iSendLen, g_objUploadFileSta.m_szTempFileName[0], 40);
				iSendLen += 40;
				
				// 包序号
				memcpy(szSendBuf+iSendLen, (void*)&ulPackNo, 8);
				iSendLen += 8;
				
				// 包大小
				szSendBuf[iSendLen++] = ireadlen%256;
				szSendBuf[iSendLen++] = ireadlen/256;

				//推到SockServExe发送
				DataPush(szSendBuf, ireadlen+C2_HEAD_SIZE, DEV_DVR, DEV_SOCK, LV2);

				if( i%10 == 0)
					usleep(200000);
			}

			if( NULL != fp )
				fclose(fp);

			// 再次发送C3结束帧：0xC3 ＋ 手机号（15）＋ 文件类型（1）＋ 文件名（40）
			szSendBuf[0] = 0xC3;
			DataPush(szSendBuf, 57, DEV_DVR, DEV_SOCK, LV2);
			PRTMSG(MSG_DBG, "Upload C3 frame!\n");
			
			// 保存到缓冲，以备重传
			memcpy(g_szReUploadBuf, szSendBuf, 57);
			g_iReUploadLen = 57;
			
			// 启动超时重传定时器
			_SetTimer(&g_objTimerMng, C1C3_REUPLOAD_TIMER, 5000, Deal_ReUpload);
		}
	}
}

void *G_DealSockData(void *arg)
{
	if(NETWORK_TYPE == NETWORK_EVDO || NETWORK_TYPE == NETWORK_WCDMA)
	{
		UPLOAD_PACK_SIZE = 1000;
	}
	else if(NETWORK_TYPE == NETWORK_TD)
	{
		UPLOAD_PACK_SIZE = 600;
	}
	
	int   iResult;
	char  szBuf[2048] = {0};
	BYTE  bytLvl;
	DWORD dwLen;
	DWORD dwPushTm = 0;
	
	while( !g_bProgExit )
	{
		iResult = g_objSockMng.PopData( bytLvl, sizeof(szBuf), dwLen, szBuf, dwPushTm);
		
		if( iResult || !dwLen || dwLen > sizeof(szBuf) )
		{
			msleep(100);
			continue;
		}

		g_objSock.AnalyseSockData(szBuf, dwLen);
	}
}

int StartBlack()
{
	if( !g_objWorkStart.Black )
	{
		PRTMSG(MSG_NOR, "Start Black\n");
		
// 		int iRet = pthread_create(&g_pthreadBlack, NULL, DealUdpSockData, NULL);
// 		if(iRet) 
// 		{
// 			PRTMSG(MSG_ERR, "start black failed: %08x\n", iRet);
// 			return -1;
// 		}
		
		sleep(1);//延时一秒确保线程成功启动
		return 0;
	}

	return -1;
}

int StopBlack()
{
	if( g_objWorkStart.Black )
	{
		PRTMSG(MSG_NOR, "Stop Black\n");
		
		g_objWorkStop.Black = TRUE;
		/*pthread_join(g_pthreadBlack, NULL);*/
		
		PRTMSG(MSG_NOR, "stop black succ\n");
		return 0;
	}
	
	return -1;
}

#endif

