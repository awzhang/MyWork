#include "yx_common.h"

#if VIDEO_BLACK_TYPE == 1

int UPLOAD_PACK_SIZE;

pthread_t g_pthreadBlack;
UpLoadFileSta g_objUploadFileSta;
BYTE	g_szA1Buf[100] = {0};
int		g_iA1Len = 0;

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
	// 推到QianExe发送
// 	PRTMSG(MSG_DBG, "Dvr Push to Qian:");
// 	PrintString((char*)szResBuf, iResLen);
	DataPush(szResBuf, (DWORD)iResLen, DEV_DVR, DEV_QIAN, LV2);
#endif

#if USE_VIDEO_TCP == 1
	char szbuf[1024] = {0};
	int  ilen = 0;
	g_objSock.MakeSmsFrame((char*)szResBuf+3, iResLen-3, 0x38, 0x4A, (char*)szbuf, sizeof(szbuf), ilen);
	g_objSock.SendTcpData(szbuf, ilen);
#endif

	// 应答后，则开始上传黑匣子文件
//	sleep(2);
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
					sprintf(g_objUploadFileSta.m_szTempFileName[v_iFileCnt], "%d_%s_%s_%s.ind", i, v_objQueryPara.m_szDate, szFileStartTime, szFileEndTime);
					sprintf(g_objUploadFileSta.m_szRealFileName[v_iFileCnt], "/mnt/%s/part1/data/%s", g_objMvrCfg.AVRecord.DiskType, objAVFileIndex.IPath);
					g_objUploadFileSta.m_ulStartOffset[v_iFileCnt] = iIndexStartOffset;
					g_objUploadFileSta.m_ulEndOffset[v_iFileCnt] = iIndexEndOffset;
					v_iFileCnt++;

					sprintf(g_objUploadFileSta.m_szTempFileName[v_iFileCnt], "%d_%s_%s_%s.264", i, v_objQueryPara.m_szDate, szFileStartTime, szFileEndTime);
					sprintf(g_objUploadFileSta.m_szRealFileName[v_iFileCnt], "/mnt/%s/part1/data/%s", g_objMvrCfg.AVRecord.DiskType, objAVFileIndex.VPath);
					g_objUploadFileSta.m_ulStartOffset[v_iFileCnt] = iVideoStartOffset;
					g_objUploadFileSta.m_ulEndOffset[v_iFileCnt] = iVideoEndOffset;
					v_iFileCnt++;

					if(iAudioEndOffset != 0)//当音频文件为空时不上传
					{
						sprintf(g_objUploadFileSta.m_szTempFileName[v_iFileCnt], "%d_%s_%s_%s.726", i, v_objQueryPara.m_szDate, szFileStartTime, szFileEndTime);
						sprintf(g_objUploadFileSta.m_szRealFileName[v_iFileCnt], "/mnt/%s/part1/data/%s", g_objMvrCfg.AVRecord.DiskType, objAVFileIndex.APath);
						g_objUploadFileSta.m_ulStartOffset[v_iFileCnt] = iAudioStartOffset;
						g_objUploadFileSta.m_ulEndOffset[v_iFileCnt] = iAudioEndOffset;
						v_iFileCnt++;
					}

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

	// 组帧（A1帧）
	szTempBuf[iTempLen++] = 0xA1;

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
	
	// 窗口大小（默认为32）
	szTempBuf[iTempLen++] = UPLOAD_WINSIZE;
	
	// 推到SockServExe发送
	DataPush(szTempBuf, (DWORD)iTempLen, DEV_DVR, DEV_SOCK, LV2);

	// 保存到缓冲，以备重传
	memcpy(g_szA1Buf, szTempBuf, iTempLen);
	g_iA1Len = iTempLen;

 	// 启动超时重传定时器
	_SetTimer(&g_objTimerMng, A1_REUPLOAD_TIMER, 8000, DealA1_ReUpload);
}

void DealA1_ReUpload(void *arg, int len)
{
	PRTMSG(MSG_DBG, "ReUpload A1 frame\n");

	DataPush(g_szA1Buf, (DWORD)g_iA1Len, DEV_DVR, DEV_SOCK, LV2);
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

			PRTMSG(MSG_DBG, "WinTotal : %d,", g_objUploadFileSta.m_ulWinTotal);
			PRTMSG(MSG_DBG, "PackTotal: %d\n", g_objUploadFileSta.m_ulPackTotal);

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

void UploadOneWindow(BOOL v_bReUpload, BOOL v_bAllReUpload, uint v_ReloadSta[UPLOAD_WINCOUNT])
{
	int	i = 0, j = 0, n = 0;
	HI_U64	ulPackNo = 0;
	HI_U64	ulWinNo = 0;
	int		ireadlen = 0;
	uint	u32bitflag[UPLOAD_WINCOUNT];
	BYTE	u8PackToSend = 0;
	BYTE	u8tempbuf[2000] = {0};
	int		s32templen = 0;
	FILE	*fp = NULL;

	for(n = 0; n < UPLOAD_WINCOUNT; n++)
		u32bitflag[n] = 0x00000001;

	// 若当前正在监控，则返回
	if(	g_objWorkStart.VUpload[0] && g_objWorkStart.VUpload[1] && g_objWorkStart.VUpload[2] && g_objWorkStart.VUpload[3] )
		return;

	tag1PComuCfg objComuCfg;
	GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );

	PRTMSG(MSG_DBG, "上传文件:%s\n", g_objUploadFileSta.m_szTempFileName[0]);

	//是否重传
	if(v_bReUpload)
	{
		if((g_objUploadFileSta.m_ulWinNo+1) == g_objUploadFileSta.m_ulWinTotal)
		{
			for(n = 0; n < UPLOAD_WINCOUNT; n++)
			{
				for(i = 0; u32bitflag[n] != 0; u32bitflag[n] = u32bitflag[n] << 1,i++)
				{
					if( !(u32bitflag[n] & v_ReloadSta[n]) )
					{
						if(i+n*UPLOAD_WINUNIT+1 > g_objUploadFileSta.m_ulPackTotal- UPLOAD_WINSIZE*g_objUploadFileSta.m_ulWinNo)
						{
							v_ReloadSta[n] |= u32bitflag[n];
						}
					}
				}
				
				u32bitflag[n] = 0x00000001;
			}
		}

		PRTMSG(MSG_DBG, "ReUpload a Window: ");
		for(n = UPLOAD_WINCOUNT-1; n >= 0; n--)
			PRTMSG(MSG_DBG, "%08x", v_ReloadSta[n]);
		PRTMSG(MSG_DBG, "\n");

		// 开始发送重传包
		for(n = 0; n < UPLOAD_WINCOUNT; n++)
		{
			for(i=0; u32bitflag[n] != 0; u32bitflag[n] = u32bitflag[n] << 1,i++)
			{
				// 若本包需要重传
				if( !(u32bitflag[n] & v_ReloadSta[n]) )
				{
					// 读取缓存的数据包
					memcpy(u8tempbuf, g_objUploadFileSta.m_szPackBuf[i+n*UPLOAD_WINUNIT],g_objUploadFileSta.m_iBufLen[i+n*UPLOAD_WINUNIT]);
					s32templen = g_objUploadFileSta.m_iBufLen[i+n*UPLOAD_WINUNIT];
	
					memcpy((void*)&ulWinNo, u8tempbuf+50+7, 8);
					memcpy((void*)&ulPackNo, u8tempbuf+50+15, 8);
					PRTMSG(MSG_DBG, "ReUpload the WinNo %ld, ", ulWinNo);
					printf("PackNo: %ld\n", ulPackNo);
	
					//推到SockServExe发送
					DataPush(u8tempbuf, (DWORD)s32templen, DEV_DVR, DEV_SOCK, LV2);
				}
			}
		}
	}
	// 若不是重传，则说明要发送新的窗口数据
	else
	{
		PRTMSG(MSG_DBG, "Upload a new Window, WinNo = %d\n", g_objUploadFileSta.m_ulWinNo);

		// 打开待传文件
		fp = fopen(g_objUploadFileSta.m_szRealFileName[0],"rb");
		if(NULL == fp)
			return;
	
		// 判断当前将要发送的窗口序号是否是本文件的最后一个窗口
		if((g_objUploadFileSta.m_ulWinNo+1) == g_objUploadFileSta.m_ulWinTotal)
		{
			u8PackToSend = g_objUploadFileSta.m_ulPackTotal- UPLOAD_WINSIZE*g_objUploadFileSta.m_ulWinNo;
			
			PRTMSG(MSG_DBG, "Now is the final window, WinTotal = %d",g_objUploadFileSta.m_ulWinTotal);
			PRTMSG(MSG_DBG, "WinNo = %d",g_objUploadFileSta.m_ulWinNo);
			PRTMSG(MSG_DBG, "u8PackToSend = %d\n", u8PackToSend);
		}
		else
			u8PackToSend = UPLOAD_WINSIZE;

		// 将文件定位到所要上传的位置
		fseek(fp, g_objUploadFileSta.m_ulStartOffset[0]+g_objUploadFileSta.m_ulWinNo*UPLOAD_WINSIZE*UPLOAD_PACK_SIZE, SEEK_SET);

		// 读取文件数据、发送、并填充到缓冲
		for(i = 0; i < u8PackToSend; i++)
		{
			// 包数据
			if( (ireadlen = fread(u8tempbuf+A2_HEAD_SIZE, 1, UPLOAD_PACK_SIZE, fp)) < 0 )
				return;

			s32templen = 0;

			u8tempbuf[s32templen++] = 0xA2;

			// 手机号
			memcpy(u8tempbuf+s32templen, objComuCfg.m_szTel, 15);
			s32templen += 15;

			// 文件类型
			u8tempbuf[s32templen++] = g_objUploadFileSta.m_uszFileType;

			// 文件名
			memcpy(u8tempbuf+s32templen, g_objUploadFileSta.m_szTempFileName[0], 40);
			s32templen += 40;

			// 窗口序号
			memcpy(u8tempbuf+s32templen, (void*)&g_objUploadFileSta.m_ulWinNo, 8);
			s32templen += 8;

			// 包序号
			PRTMSG(MSG_DBG, "WinNo: %ld, ", g_objUploadFileSta.m_ulWinNo);
			printf("PackNo: %ld\n", g_objUploadFileSta.m_ulPackNo);
			memcpy(u8tempbuf+s32templen, (void*)&g_objUploadFileSta.m_ulPackNo, 8);
			g_objUploadFileSta.m_ulPackNo++;
			s32templen += 8;
			
			// 包大小
			u8tempbuf[s32templen++] = ireadlen%256;
			u8tempbuf[s32templen++] = ireadlen/256;

			// 复制到缓冲中备份，以预备重传
			memcpy(g_objUploadFileSta.m_szPackBuf[i], u8tempbuf, ireadlen+A2_HEAD_SIZE);
			g_objUploadFileSta.m_iBufLen[i] = ireadlen+A2_HEAD_SIZE;
			
			//推到SockServExe发送
			DataPush(u8tempbuf, ireadlen+A2_HEAD_SIZE, DEV_DVR, DEV_SOCK, LV2);
		}

		if(NULL != fp)
		{
			fclose(fp);
			fp = NULL;
		}
	}
}

// 文件类型（1）＋ 文件名（40）＋版本号（6）＋ 应答类型（1）
int Deal38B1(char *v_databuf, int v_datalen)
{
	char szNewFileName[40] = {0};
	memcpy(szNewFileName, v_databuf+1, 34);
	PRTMSG(MSG_DBG, "Recv B1 frame:%s \n", szNewFileName);
	//PrintString(v_databuf+41, v_datalen-41);
	
	int	i = 0;
	char	s8FileName[40] = {0};
	
	memcpy(s8FileName, v_databuf+1, 34);
	
	// 若文件名一致，则关闭A1重传定时器
	if( strcmp(s8FileName, g_objUploadFileSta.m_szTempFileName[0]) == 0 )
	{
		PRTMSG(MSG_NOR, "close A1_ReUpload Timer\n");

		_KillTimer(&g_objTimerMng, A1_REUPLOAD_TIMER);
	}

	return 1;
}

//文件类型（1）＋ 文件名（40）＋ 当前窗口序号（8）＋ 窗口状态（4）
int Deal38B2(char *v_databuf, int v_datalen)
{
	static int iB2Cnt = 0;

	int		i = 0;
	BOOL	bNeedReUpload = FALSE;
	HI_U64  ulWinNo = 0;
	uint	uiWinSta[UPLOAD_WINCOUNT] = {0};
	char	szFileName[40] = {0};
	void	*thread_result;

	char szNewFileName[40] = {0};
	memcpy(szNewFileName, v_databuf+1, 34);
	PRTMSG(MSG_DBG, "Recv B2 frame:%s ", szNewFileName);
	PrintString(v_databuf+41, v_datalen-41);

	// 判断长度
	if( v_datalen != 49+4*UPLOAD_WINCOUNT )
	{
		PRTMSG(MSG_ERR, "B2 frame uncorect data len\n");
		return 0;
	}

	// 判断文件名是否与正在上传的文件名一致
	memcpy(szFileName, v_databuf+1, 34);
	if( strcmp(szFileName, g_objUploadFileSta.m_szTempFileName[0]) != 0 )
	{
		PRTMSG(MSG_NOR, "Deal38B2 failed, uncorrect file name!\n");
		return 0;
	}

	// 窗口序号
	memcpy((void*)&ulWinNo, v_databuf+41, 8);

	// 窗口状态
	for(int n = 0; n < UPLOAD_WINCOUNT; n++)
		memcpy((void*)&(uiWinSta[n]), v_databuf+49+4*n,4);

	// 判断是否需要重传
	if( ulWinNo != g_objUploadFileSta.m_ulWinNo )
		bNeedReUpload = FALSE;
	else
		bNeedReUpload = TRUE;
	
	// 若无需重传，则更新传输状态
	if( FALSE == bNeedReUpload )
	{
		g_objUploadFileSta.m_iBufCount = 0;
		g_objUploadFileSta.m_ulWinNo++;
		
		// 若所请求的窗口序号不等于将要上传的窗口序号
		if( (ulWinNo != g_objUploadFileSta.m_ulWinNo) || ( (ulWinNo+1) > g_objUploadFileSta.m_ulWinTotal) )
		{
			PRTMSG(MSG_DBG, "uncorrect Window No, ");
			PRTMSG(MSG_DBG, "m_ulWinNo = %d," ,g_objUploadFileSta.m_ulWinNo);
			PRTMSG(MSG_DBG, "ulWinNo =%d\n", ulWinNo);

			return 0;
		}
		
		// 发送下一个窗口
		UploadOneWindow(FALSE, FALSE, uiWinSta);		
	}
	// 否则重传中心所需的数据包
	else
	{
		UploadOneWindow(TRUE, FALSE, uiWinSta);
	}

	return 1;
}

// 文件类型（1）＋ 文件名（40）
int Deal38B3(char *v_databuf, int v_datalen)
{
	char szNewFileName[40] = {0};
	memcpy(szNewFileName, v_databuf+1, 34);
	PRTMSG(MSG_DBG, "Recv B3 frame:%s\n", szNewFileName);
	
	int		i = 0;
	char	s8FileName[40] = {' '};
	BYTE	u8ResType = 0;
	BYTE	s8TempBuf[100] = {0};
	int		s32TempLen = 0;
	BYTE	s8SendBuf[100] = {0};
	int		s32SendLen = 0;

	memcpy(s8FileName, v_databuf+1, 34);

	if( strcmp(s8FileName, g_objUploadFileSta.m_szTempFileName[0]) == 0 )
	{
		u8ResType = 1;
	}
	else
	{
		PRTMSG(MSG_DBG, "Recv B3 frame, but file name is not correct!\n");
		PRTMSG(MSG_DBG, "Center file name: %s\n", s8FileName);
		PRTMSG(MSG_DBG, "Local  file name: %s\n", g_objUploadFileSta.m_szTempFileName[0]);
		return 0;
	}

	// 组帧应答（A3）
	s8TempBuf[s32TempLen++] = 0xA3;

	// 手机号
	tag1PComuCfg objComuCfg;
	GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );					
	memcpy(s8TempBuf+s32TempLen, objComuCfg.m_szTel, 15);
	s32TempLen += 15;

	// 文件类型
	s8TempBuf[s32TempLen++] = g_objUploadFileSta.m_uszFileType;

	// 文件名
	memcpy(s8TempBuf+s32TempLen, g_objUploadFileSta.m_szTempFileName[0], 40);
	s32TempLen += 40;

	//应答类型
	s8TempBuf[s32TempLen++] = u8ResType;

	// 传输结束标志
	s8TempBuf[s32TempLen++] = ( (UpdateUploadSta(TRUE) == TRUE) ? 0 : 1);

	//推到SockServExe发送
	DataPush(s8TempBuf, (DWORD)s32TempLen, DEV_DVR, DEV_SOCK, LV2);

	sleep(2);

	// 应答之后发送下一个文件
	BeginUploadFile();

	return 1;
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

