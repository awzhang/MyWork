
#include "yx_QianStdAfx.h"

#if USE_LIAONING_SANQI == 1

	#undef MSG_HEAD
	#define MSG_HEAD	("LN_update-DownLoad")

void *G_ThreadLNDownLoad(void *arg)
{
	g_objLNDownLoad.P_ThreadDownLoad();
}

CLNDownLoad::CLNDownLoad()
{
	m_bThreadExit = false;
	m_bNetFined = false;
	m_bytSta = Idlesse;
	m_bReturnIdl = false;

	m_bNeedUpdate = true;

	memset(m_bytRecvBuf, 0, sizeof(m_bytRecvBuf));
	memset((void*)&m_objFileList, 0, sizeof(m_objFileList));

	m_dwFileSize = 0;
	m_dwDownSize = 0;
	m_ulPackageLen = 0;
	m_usEndPackageNo = 0;
	memset(m_bytMd5, 0, sizeof(m_bytMd5));
	m_bMd5ok = false;

	m_pDownBuf = NULL;
	memset(m_bytBlockSta, 0, sizeof(m_bytBlockSta));
	m_objRecvMsg.InitCfg( 64000, 30000 );
}

CLNDownLoad::~CLNDownLoad()
{

}

int CLNDownLoad::Init()
{
	if( access(LIU_DOWNLOAD_PATH, F_OK) != 0 )
	{
		mkdir(LIU_DOWNLOAD_PATH, 777);
		chmod(LIU_DOWNLOAD_PATH, S_IRWXU|S_IRWXG|S_IRWXO);
	}

	// 读取文件列表
	char szFileName[MAX_PATH] = {0};
	sprintf(szFileName, "%sFileLst.dat", LIU_DOWNLOAD_PATH);

	FILE *fp = NULL; 
	if( (fp = fopen(szFileName, "rb+")) != NULL)
	{
		fread((void*)&m_objFileList, 1, sizeof(m_objFileList), fp);
	} else
	{
		if( NULL == (fp = fopen(szFileName, "wb+")) )
		{
			PRTMSG(MSG_ERR, "create LstFile failed!\n");
			perror("");
			return ERR_FILE;
		}
	}
	fclose(fp); 


	// 创建下载线程
	if( pthread_create(&m_pThreadDown, NULL, G_ThreadLNDownLoad, NULL) != 0 )
		return ERR_THREAD;

	PRTMSG(MSG_NOR, "Create DownLoad thread succ!\n");
	return 0;
}

int CLNDownLoad::Release()
{
	m_bThreadExit = true;

	return 0;
}

void CLNDownLoad::P_ThreadDownLoad()
{
	DWORD   dwSleepTm = 0;
	int     Cnt = 0;        
	byte    bytLvl = 0;
	DWORD   dwPushTm = 0;
	ulong   iPackageID;
	ushort  usCurBlockSize, usCurPackageNo;
	bool bDownBufFul = true, bSendA3 = false;
	char szSendBuf[100] = {0};
	DWORD dwSendLen = 0;
	static byte errcnt = 0;
	while( !m_bThreadExit )
	{
		switch( m_bytSta )
		{
		case Idlesse:   
			{
				dwSleepTm = 200000;
				if( !m_bNetFined )
					break;
				if(errcnt > 11)
				{
					m_bNeedUpdate = false;
					errcnt = 0;
					ClearPar();
				}
				if( !m_bNeedUpdate )	// 是否要升级
					break;
				m_bytSta = DispTask;
			}
			break;

		case DispTask:
			{
				// 有应用软件升级包需要下载
//						m_bytFlstNo = SOFT;//此处应区分为SOFT、ROOT、KERN、BOOT
				dwSendLen = 0;
				ushort packno;
				for(m_bytFlstNo = SOFT; m_bytFlstNo < FILE_TYPE_COUNT; m_bytFlstNo++)
				{
					// 若该文件已存在，且已下载一部分，但未下完，则断点续传
					if( m_objFileList.m_objFile[m_bytFlstNo].m_bExist
						&& m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize > 0
						&& m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize < m_objFileList.m_objFile[m_bytFlstNo].m_ulFileSize)
					{
						//通知DvrExe开始远程升级，防止进入省电状态
						char by = 0x03;
						DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);

						PRTMSG(MSG_NOR, "Continue download soft\n");
						m_bytFileType = m_bytFlstNo;
						m_dwFileSize = m_objFileList.m_objFile[m_bytFlstNo].m_ulFileSize;
						m_dwDownSize = m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize;
						m_ulPackageLen = m_objFileList.m_objFile[m_bytFlstNo].m_ulPackLen;
						memcpy(m_bytMd5, m_objFileList.m_objFile[m_bytFlstNo].m_bytMd5, sizeof(m_bytMd5));
						// 发送
						memcpy(szSendBuf, m_bytMd5, 16);
						dwSendLen = 16;
						G_LocalToNet((char*)&m_dwFileSize, szSendBuf+dwSendLen, 4);
						dwSendLen += 4;
						G_LocalToNet((char*)&m_ulPackageLen, szSendBuf+dwSendLen, 2);
						dwSendLen += 2;
						packno = (ushort)(m_dwDownSize/m_ulPackageLen);	 //下载完的数据是包大小的整数倍
						G_LocalToNet((char*)&packno, szSendBuf+dwSendLen, 2);
						dwSendLen += 2;
						packno = min(packno + 30, packno + (ushort)((m_dwFileSize - m_dwDownSize + m_ulPackageLen - 1)/m_ulPackageLen) - 1);
						m_usEndPackageNo = packno;
						G_LocalToNet((char*)&packno, szSendBuf+dwSendLen, 2);
						dwSendLen += 2;
						g_objApplication.SendAppFrame(MODE_C, 0xa1, true, 0, szSendBuf, dwSendLen);
						m_bytSta = ReqAttr;
						iPackageID = 0;
						break;
					}
					// 否则是未下载过的，或需要重新下载
				}
				if(m_bytSta == ReqAttr)
				{
					break;
				}
				if(0 == m_dwFileSize || 0 == m_ulPackageLen)
				{
					m_bNeedUpdate = false;
					// 没有下载任务，退回空闲
					PRTMSG(MSG_NOR, "No download task, turn to Idlesse\n");
					m_bytSta = Idlesse; 
					break;
				}
				m_bytFlstNo = m_bytFileType;
				_DeleteFile(m_bytMd5);
				m_dwDownSize = 0;
				memcpy(szSendBuf, m_bytMd5, 16);
				dwSendLen = 16;
				G_LocalToNet((char*)&m_dwFileSize, szSendBuf+dwSendLen, 4);
				dwSendLen += 4;
				G_LocalToNet((char*)&m_ulPackageLen, szSendBuf+dwSendLen, 2);
				dwSendLen += 2;
				packno = (ushort)(m_dwDownSize/m_ulPackageLen);
				G_LocalToNet((char*)&packno, szSendBuf+dwSendLen, 2);
				dwSendLen += 2;
				packno = min(packno + 30, packno + (ushort)((m_dwFileSize - m_dwDownSize + m_ulPackageLen - 1)/m_ulPackageLen) - 1);
				m_usEndPackageNo = packno;
				G_LocalToNet((char*)&packno, szSendBuf+dwSendLen, 2);
				dwSendLen += 2;
				g_objApplication.SendAppFrame(MODE_C, 0xa1, true, 0, szSendBuf, dwSendLen);
				m_bytSta = ReqAttr;
				iPackageID = 0;
			}
			break;

		case ReqAttr:
			{
				dwSleepTm = 50000;

				int iret = _WaitForAnswer(30000, 0x21, 0x00);

				if(iret == 1)
				{

					if(!m_objFileList.m_objFile[m_bytFlstNo].m_bExist)
					{
						if(0 == strcmp((char*)m_bytMd5, ""))
							PRTMSG(MSG_NOR, "The downfile's Md5 is null!\n");

						// 创建文件
						char szFileName[MAX_PATH] = {0};
						//sprintf(szFileName, "%s%08X.dat", LIU_DOWNLOAD_PATH, m_dwFileId);
						sprintf(szFileName, "/mnt/Flash/part3/Down/exe.bin");
						//PRTMSG(MSG_DBG, "Creat file:%s!\n", szFileName);
						FILE *fp = fopen(szFileName, "wb+");
						if(!fp)
						{
							PRTMSG(MSG_ERR, "Create downfile fail:"); 
							perror(" ");
							m_bytSta=Idlesse; 
							break; 
						} else
						{
							fclose(fp);
						}

					}
					// 更新文件列表
					memset((void*)&m_objFileList.m_objFile[m_bytFlstNo], 0, sizeof(FileAttr));
					m_objFileList.m_objFile[m_bytFlstNo].m_bExist = true;
					m_objFileList.m_objFile[m_bytFlstNo].m_ulFileSize = m_dwFileSize;
					m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize = m_dwDownSize;
					m_objFileList.m_objFile[m_bytFlstNo].m_ulPackLen = m_ulPackageLen;
					memcpy(m_objFileList.m_objFile[m_bytFlstNo].m_bytMd5, m_bytMd5, 16);
					m_dwCurpackLen = m_ulPackageLen;
					_RenewFileLst();

					// 清空下载缓冲
					if(m_pDownBuf)
					{
						free(m_pDownBuf);
						m_pDownBuf = NULL; 
					}
					m_pDownBuf = (byte*)malloc(31 * m_ulPackageLen);
					memset(m_bytBlockSta, 0, 31);
					PRTMSG(MSG_ERR, "Start Rcving Data\n"); 
					m_timeout = GetTickCount();
					bSendA3 = false;
					m_bytSta = WaitData;
					break;
				} else
				{
					PRTMSG(MSG_NOR, "Soft Update started err!\n"); 
					errcnt++;
					m_bytSta=Idlesse; 
					break; 
				}
			} 

			break;

		case WaitData:
			{
				dwSleepTm = 10000;
				ushort rpackageno;  
				ulong rdatalen; 
				uint t1 = GetTickCount() - m_timeout;              
				//超过10秒没有收到数据
				if(t1 > 10000)
				{
					if(bSendA3)
					{
						errcnt++;
						m_bytSta = Idlesse;
						PRTMSG(MSG_DBG, "Have not data over 10s!");
					} else
					{
						m_bytSta = ReportChk;
					}
					break;
				}
				if( !m_objRecvMsg.PopData(bytLvl, sizeof(m_bytRecvBuf), m_dwRecvLen, (char*)m_bytRecvBuf, dwPushTm) )
				{
					m_timeout = GetTickCount();
					dwSleepTm = 0;
					bSendA3 = false;
					switch(m_bytRecvBuf[0])
					{
					case 0x22:
						{
							m_bytSta = RcvData;
						}
						break;
					case 0x23:
						{
							//接收结果查询
							// 获得当前块的实际大小
							Cnt = 2;
							rdatalen  = (byte)(m_bytRecvBuf[1]);
							for(;Cnt < 5; Cnt++)
							{
								rdatalen = (rdatalen << 8) + (byte)(m_bytRecvBuf[Cnt]);
							}
							if(rdatalen != m_dwFileSize)
							{
								PRTMSG(MSG_DBG, "Target file len %d != Cur file len %d\n", rdatalen, m_dwFileSize);
								errcnt++;
								m_bytSta = Idlesse;
								break;
							}
							rpackageno  = (byte)(m_bytRecvBuf[5]);
							rpackageno = (rpackageno << 8) + (byte)(m_bytRecvBuf[6]);
							if(rpackageno < (ushort)(m_dwDownSize / m_ulPackageLen))
							{
								PRTMSG(MSG_DBG, "Package NO %d err!! Cur:%d Rcv:%d \n", rpackageno, (usCurBlockSize + (ushort)(m_dwDownSize / m_ulPackageLen)), (ushort)(m_dwDownSize / m_ulPackageLen));
								errcnt++;
								m_bytSta = Idlesse;
								break;
							}
							m_bytSta = ReportChk;
						}
						break;
					default:
						PRTMSG(MSG_DBG, "Unknow data!\n");
						break;
					}
				}
			}
			break;  
		case ReportChk:
			{
				dwSleepTm = 0;
				dwSendLen = 0;
				ushort losepack = 0;
				memcpy(szSendBuf, m_bytMd5, 16);
				dwSendLen = 16;
				G_LocalToNet((char*)&m_usPackageNo, szSendBuf+dwSendLen, 2);
				dwSendLen += 2;
				szSendBuf[dwSendLen++] = 0x02;
				szSendBuf[dwSendLen++] = 0;
				usCurBlockSize = min( (ushort)((m_dwFileSize - m_dwDownSize + m_ulPackageLen - 1)/m_ulPackageLen) - 1, 30);
				// 判断缓冲区
				for(ushort i = 0; i <= usCurBlockSize; i++)
				{
					if(m_bytBlockSta[i]==0)
					{
						++szSendBuf[19];
						losepack = (ushort)((m_dwDownSize + m_ulPackageLen - 1)/m_ulPackageLen) + i;
						G_LocalToNet((char*)&losepack, szSendBuf+dwSendLen, 2);
						dwSendLen += 2;
					}
				}
				if(20 == dwSendLen )
				{
					szSendBuf[18] = 0x01;
					--dwSendLen;
					m_bytSta = ChkBuf;
				} else
				{
					m_bytSta = WaitData;
				}                       
				g_objApplication.SendAppFrame(MODE_C, 0xa3, true, 0, szSendBuf, dwSendLen);
				bSendA3 = true;
			}
			break;
		case RcvData:	//将接收的数据存入块缓冲区中
			{
				dwSleepTm = 0;              
				// 当前包序号
				m_usPackageNo = (byte)(m_bytRecvBuf[1]);
				m_usPackageNo = (m_usPackageNo << 8) + (byte)(m_bytRecvBuf[2]);
				if(m_usPackageNo < (ushort)(m_dwDownSize/m_ulPackageLen))
				{
					PRTMSG(MSG_DBG, "Cur package NO:%d  End package NO:%d\n", m_usPackageNo, m_usEndPackageNo);
					break;
				}
				// 将帧数据copy至块缓冲区
				iPackageID = (m_usPackageNo - (ushort)(m_dwDownSize/m_ulPackageLen)) % 31;
				memcpy(m_pDownBuf + iPackageID * m_ulPackageLen, m_bytRecvBuf + 3, m_dwRecvLen - 3);
				m_bytBlockSta[iPackageID] = 1;
				m_dwCurpackLen = min(m_dwRecvLen - 3, m_dwCurpackLen);
				PRTMSG(MSG_NOR, "%d k/%d k\n", (m_dwDownSize+(iPackageID+1) * m_ulPackageLen)/1024, m_dwFileSize/1024);
				m_bytSta = WaitData;
			}
			break;
		case ChkBuf:
			{
				char szFileName[100] = {0};
				//sprintf(szFileName, "%s%08X.dat", LIU_DOWNLOAD_PATH, m_dwFileId);
				sprintf(szFileName, "/mnt/Flash/part3/Down/exe.bin");
				PRTMSG(MSG_DBG, "Write one frame!\n\n");
				FILE *fp = fopen(szFileName, "rb+");
				if(fp)
				{
					// 算出数据大小
					ulong ulLastSize = usCurBlockSize *  m_ulPackageLen + m_dwCurpackLen;

					// 将数据写入文件
					fseek(fp, m_dwDownSize, 0);
					size_t ret = fwrite(m_pDownBuf, ulLastSize, 1, fp);
					if(ret != 1)
					{
						fclose(fp); 
						m_bytSta=Idlesse;
						break; 
					}
					fclose(fp);

					// 更新下载标志
					m_dwDownSize += ulLastSize;
					m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize = m_dwDownSize;
					m_objFileList.m_objFile[m_bytFlstNo].m_ulPackLen = m_ulPackageLen;
					_RenewFileLst();

					// 清空缓冲区
					memset(m_pDownBuf, 0, 31 * m_ulPackageLen);
					memset(m_bytBlockSta, 0, 31);
					m_bytSta = WaitData;                            
				} else
				{
					PRTMSG(MSG_ERR, "Open file fail when write:");
					perror(" ");
					_DeleteFile(m_bytMd5);
					errcnt++;
					m_bytSta=Idlesse;
					break; 
				}
				// 文件下载完毕
				if(m_dwDownSize==m_dwFileSize)
				{
					memset(m_bytBlockSta, 0, 31);
					m_bytSta = ChkMd5;
				}
			}
			break;

		case ChkMd5:
			{
				// 获取下载文件实际的md5值
				CMd5 cmd5;
				uchar md5_new[16] = {0};
				char szFileName[100] = {0};
				sprintf(szFileName, "/mnt/Flash/part3/Down/exe.bin");

				if(!cmd5.MD5File16(szFileName, md5_new))
					cmd5.MD5File16(szFileName, md5_new);	//为何要算两次？

				// 比较两个md5是否相等
				m_bMd5ok = (0 == memcmp(md5_new, m_bytMd5, 16)) ? true : false;
				memcpy(szSendBuf, m_bytMd5, 16);
				dwSendLen = 16;

				if(!m_bMd5ok)
				{
					PRTMSG(MSG_ERR, "Check Md5 Err, delete the file\n");
					_DeleteFile(m_bytMd5);  
					szSendBuf[dwSendLen++] = 0x01;
				} else
				{
					szSendBuf[dwSendLen++] = 0x00;

				}
				//拆链
				g_objApplication.SendAppFrame(MODE_C, 0xa4, true, 0, szSendBuf, dwSendLen);
				m_bytSta = TranResult;
			}
			break;  
		case TranResult:
			{
				int iret = _WaitForAnswer(30000, 0x24, 0x00);
				if(1 == iret)
				{
					PRTMSG(MSG_DBG, "Destroy link succ!!!\n");  
				} else
				{
					PRTMSG(MSG_DBG, "Destroy link err!!!\n");
				}
				if(!m_bMd5ok)
				{
					errcnt++;
					m_bytSta = Idlesse;
				}
				m_bytSta = SoftUpdate;
			}
			break;

		case SoftUpdate:
			{
				dwSleepTm = 0;

					PRTMSG(MSG_NOR, "Begin soft update\n");

					char szFileName[MAX_PATH] = {0};
					//sprintf(szFileName, "%s%08X.dat", LIU_DOWNLOAD_PATH, m_dwFileId);
					sprintf(szFileName, "/mnt/Flash/part3/Down/exe.bin");

					//if( G_SoftUpdate(m_bytFlstNo, szFileName, 1) )
					if( AppIntact(szFileName) == true && G_ExeUpdate(szFileName, 0))
					{
// 						// 远程升级成功后，将恢复出厂配置，但要保留手机号、IP、端口
// 						char szbuf[40] = {0};
// 						sprintf(szbuf, "%s", NEED_RUSEMUCFG);
// 						SetImpCfg(szbuf, offsetof(tagImportantCfg, m_szSpecCode), sizeof(szbuf));
// 						SetSecCfg(szbuf, offsetof(tagSecondCfg, m_szSpecCode), sizeof(szbuf));

						DelErrLog();	// 升级成功后程序从分区5启动
		
						//通知DvrExe远程升级结束
						char by = 0x08;
						DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
						
						PRTMSG(MSG_NOR, "Update succ, system now is reboot...\n");
						sleep(3);
						G_ResetSystem();
					}

					m_bytSta = Idlesse;
			}
			break;  

		default:
			break;
		}

		usleep(dwSleepTm);
	}
}

// 等待中心应答, 类型：1——应答到， 2——应答未到， 3——退回空闲
int  CLNDownLoad::_WaitForAnswer(DWORD v_dwWaitTm, char v_szDataType, char v_szResult)
{
	DWORD dwBeginWaiTm = GetTickCount();
	byte  bytLvl;
	DWORD dwPushTm;
	int   iResult = 2; // 缺省未应答未到

	while( GetTickCount() - dwBeginWaiTm < v_dwWaitTm )
	{
		if( m_bReturnIdl == true )
		{
			iResult = 3;
			goto _WAIT_END;
		}

		// 从m_objRecvMsg中pop出来的数据已经过校验，且去掉校验和、车辆ID
		if( !m_objRecvMsg.PopData(bytLvl, sizeof(m_bytRecvBuf), m_dwRecvLen, (char*)m_bytRecvBuf, dwPushTm) )
		{
			// 若业务类型和数据类型都匹配，收到应答
			if( v_szDataType == m_bytRecvBuf[0] && v_szResult == m_bytRecvBuf[1])
			{
				iResult = 1;
				goto _WAIT_END;
			}
		} else
		{
			usleep(20000);
		}
	}

	_WAIT_END:
	return iResult;
}

bool CLNDownLoad::_DeleteFile(uchar *md5)
{
	int i = 0;
	char szFileName[MAX_PATH] = {0};

	for(i = 0; i < FILE_TYPE_COUNT; i++)
	{
		if( memcmp(md5, m_objFileList.m_objFile[i].m_bytMd5, 16) == 0 && m_objFileList.m_objFile[i].m_bExist )
		{
			sprintf(szFileName, "/mnt/Flash/part3/Down/exe.bin");

			// 若文件存在，则删除该文件
			if( access(szFileName, F_OK) == 0 )
			{
				unlink(szFileName);
			}

			memset((void*)&m_objFileList.m_objFile[i], 0, sizeof(FileAttr));
			m_objFileList.m_objFile[i].m_bExist = false;

			// 更新文件列表
			_RenewFileLst();
			return true;
		}
	}
	return false;
}

bool CLNDownLoad::_RenewFileLst()
{
	char szFileName[MAX_PATH] = {0};

	sprintf(szFileName, "%sFileLst.dat", LIU_DOWNLOAD_PATH);

	FILE *fp = fopen(szFileName, "rb+");

	if(!fp)
		return false;

	fseek(fp, 0, SEEK_SET);
	fwrite(&m_objFileList, sizeof(m_objFileList), 1, fp);   
	fclose(fp);

	return true;
}

void CLNDownLoad::ClearPar()
{
	m_bNeedUpdate = false;
	m_dwFileSize = 0;
	m_dwDownSize = 0;
	m_ulPackageLen = 0;
	m_usEndPackageNo = 0;
	memset(m_bytMd5, 0, sizeof(m_bytMd5));
	memset(m_bytBlockSta, 0, sizeof(m_bytBlockSta));
	memset(m_bytRecvBuf, 0, sizeof(m_bytRecvBuf));
}
#endif

