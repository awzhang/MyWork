#include "yx_UpdateStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD	("Update-DownLoad")

void *G_ThreadDownLoad(void *arg)
{
	g_objDownLoad.P_ThreadDownLoad();
}

void G_TmRetran(void *arg, int len)
{
	g_objDownLoad.P_TmRetran();
}

CDownLoad::CDownLoad()
{
	m_bThreadExit = false;
	m_bNetFined = false;
	m_bytSta = Idlesse;
	m_bLoginNow = true;
	m_bReturnIdl = false;
	m_bRtcTimeChecked = false;
	m_bRtcNeedChk	= true;

#if WIRELESS_UPDATE_TYPE == 1
	m_bNeedUpdate = false;
#endif

	memset(m_bytRecvBuf, 0, sizeof(m_bytRecvBuf));
	memset((void*)&m_objFileList, 0, sizeof(m_objFileList));

	m_dwLastLoginTm = GetTickCount()-LOGIN_INTERVAL+60*1000;	//开机后延迟60s登陆

	m_dwFileId = 0;
	m_dwFileSize = 0;
	m_dwDownSize = 0;
	m_dwDownFrameCt = 0;
	memset(m_bytMd5, 0, sizeof(m_bytMd5));
	m_bMd5ok = false;

	m_pDownBuf = NULL;
	m_usBlockSize = 0;
	m_usFrameSize = 0;
	memset(m_bytBlockSta, 0, sizeof(m_bytBlockSta));
	m_usBlockNo = 0;
}

CDownLoad::~CDownLoad()
{
	
}

int CDownLoad::Init()
{
	if( access(LIU_DOWNLOAD_PATH, F_OK) != 0 )
	{
		mkdir(LIU_DOWNLOAD_PATH, 777);
		chmod(LIU_DOWNLOAD_PATH, S_IRWXU|S_IRWXG|S_IRWXO);
	}

#if WIRELESS_DOWNLOAD_TYPE == 0
	// 读取文件列表
	char szFileName[MAX_PATH] = {0};
	sprintf(szFileName, "%sFileLst.dat", LIU_DOWNLOAD_PATH);

	FILE *fp = NULL; 
	if( (fp = fopen(szFileName, "rb+")) != NULL )
	{
		fread((void*)&m_objFileList, 1, sizeof(m_objFileList), fp);
	}
	else
	{
		if( NULL == (fp = fopen(szFileName, "wb+")) )
		{
			PRTMSG(MSG_ERR, "create LstFile failed!\n");
			perror("");
			return ERR_FILE;
		}
	}
	fclose(fp);	
#endif

	// 读取车辆ID
	tag1PComuCfg cfg;
	GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
	char str[10] = {0};
	if( strlen(cfg.m_szTel) >= 2 )
		sprintf(str, "%.9s", cfg.m_szTel+2);
	sscanf(str, "%u", &m_ulCarId);
	if(m_ulCarId)
		m_ulCarId = m_ulCarId | 0x80000000;

	// 创建下载线程
	if( pthread_create(&m_pThreadDown, NULL, G_ThreadDownLoad, NULL) != 0 )
		return ERR_THREAD;

	PRTMSG(MSG_NOR, "Create DownLoad thread succ!\n");
	return 0;
}

int CDownLoad::Release()
{
	m_bThreadExit = true;

	return 0;
}

void CDownLoad::P_ThreadDownLoad()
{
	DWORD	dwSleepTm = 0;
	int		iLoginFailCnt = 0;		// 登陆无应答次数
	byte	bytLvl = 0;
	DWORD	dwPushTm = 0;

	while( !m_bThreadExit )
	{
		switch( m_bytSta )
		{
			case Idlesse:	
				{
					dwSleepTm = 200000;

					if( !m_bNetFined )
						break;

#if WIRELESS_UPDATE_TYPE == 1
					if( !m_bNeedUpdate )	// 使用维护中心进行远程升级，是否要升级
						break;
#endif

					// 每隔30分钟登陆一次中心，或者强制登陆
					if( m_bLoginNow || (GetTickCount()-m_dwLastLoginTm) > LOGIN_INTERVAL )
					{
						m_dwLastLoginTm = GetTickCount();
						m_bLoginNow = false;
						m_bReturnIdl = false;

						// 以下发送登陆帧
						// 车台手机号
						Frm0181 frm;
						tag1PComuCfg objComuCfg;
						GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );					
						long long int64 = yx_atoi64(objComuCfg.m_szTel);
						{
							byte *p = NULL;
							p = (byte*)&int64;
							frm.m_bytTel[0] = *(p+5);
							frm.m_bytTel[1] = *(p+4); 
							frm.m_bytTel[2] = *(p+3);
							frm.m_bytTel[3] = *(p+2); 
							frm.m_bytTel[4] = *(p+1);
							frm.m_bytTel[5] = *(p+0);
						}
						
						// 时间
						struct tm pCurrentTime;
						G_GetTime(&pCurrentTime);

						frm.m_bytDate[0] = (byte)(pCurrentTime.tm_year-100);
						frm.m_bytDate[1] = (byte)(pCurrentTime.tm_mon+1);
						frm.m_bytDate[2] = (byte)pCurrentTime.tm_yday;
						frm.m_bytDate[3] = (byte)pCurrentTime.tm_hour;
						frm.m_bytDate[4] = (byte)pCurrentTime.tm_min;
						frm.m_bytDate[5] = (byte)pCurrentTime.tm_sec;
						
						// 版本号
						frm.m_ulProVer = PROTOCOL_VER;

						// 发送登陆帧
						PRTMSG(MSG_NOR, "Send Log Frame!\n");
						_SendOneFrame(0x01, 0x81, (char*)&frm, sizeof(frm), false, 0);

						m_bytSta = Logining;
						break;
					}
				}
				break;

			case NetWick:
				{
					dwSleepTm = 50000;

					//m_bNetFined = false;
					m_bLoginNow = true;

// 					// 请求重新连接, 为了SockServExe处理方便，此处用DEV_QIAN作为推入标示
// 					char buf = 0x01;
// 					DataPush((void*)&buf, 1, DEV_QIAN, DEV_SOCK, LV3);

					sleep(60);	// 等待一分钟再重新登陆
					m_bytSta = Idlesse;
				}
				break;

			case Logining:	
				{
					dwSleepTm = 50000;

					// 等待登陆应答
					int iret = _WaitForAnswer(WAIT_ANS_TIME, 0x01, 0x01);
					
					// 收到登陆应答
					if( 1 == iret )
					{
						iLoginFailCnt = 0;

						Frm0101 *frm = (Frm0101*)(m_bytRecvBuf+2);
						if( 0 == frm->m_bytType )	// 登陆成功
						{
							PRTMSG(MSG_NOR, "Login Liu succ!\n");
							m_bytSta = ChkDiskExp;							

							//校时
							PRTMSG(MSG_DBG, "Liu Time: Year:%d Month:%d Day:%d Hour:%d Minute:%d Second:%d\n", frm->m_bytYear, frm->m_bytMonth, frm->m_bytDay, frm->m_bytHour, frm->m_bytMinute, frm->m_bytSecond);
							if( true == m_bRtcNeedChk )
							{
								struct tm objSetTime;
								objSetTime.tm_sec = frm->m_bytSecond;
								objSetTime.tm_min = frm->m_bytMinute;
								objSetTime.tm_hour = frm->m_bytHour;
								objSetTime.tm_mday = frm->m_bytDay;
								objSetTime.tm_mon = frm->m_bytMonth - 1;    //0~11
								objSetTime.tm_year = frm->m_bytYear + 100;
								G_GetUTCTime(&objSetTime);
								if (false == m_bRtcTimeChecked)
								{
									if( true == _SetTimeToRtc(&objSetTime) )
									{
										m_bRtcTimeChecked = true;
									}
								}
								SetCurTime(&objSetTime);
							}
						}
						else // 登陆失败
						{
							PRTMSG(MSG_NOR, "Liu Center refuse login!\n");
							m_bytSta = Idlesse;
						}
					}
					else if( 2 == iret )
					{
						iLoginFailCnt++;

						// 若无应答次数已达到上限，则说明网络异常
						if( iLoginFailCnt >= MAX_RELOGIN )
						{
							iLoginFailCnt = 0;							
							m_bytSta = NetWick;						
						}
						else
						{
							m_bLoginNow = true;		// 退回空闲，再次登陆
							m_bytSta = Idlesse;
						}
					}
					else	//其他都返回Idlessse状态
					{
						m_bytSta = Idlesse;
					}
				}
				break;

			case ChkDiskExp:	//待讨论，是否要在此处检查磁盘
				{
					char szSoftVer[64] = {0};
					ChkSoftVer(szSoftVer);

					char szSysVer[64] = {0};
					ReadVerFile(szSysVer);

					dwSleepTm = 50000;

					Frm1281 frm;
					memcpy(frm.m_bytCurSys, szSysVer, sizeof(SYSTEM_VER));
					memcpy(frm.m_bytCurSft, szSoftVer, sizeof(SOFT_VER));
					frm.m_ulSysStat = 0;

					// 发送任务请求
					_SendOneFrame(0x12, 0x81, (char*)&frm, sizeof(frm), true, RETRAN_INTERVAL);
					
					m_bytSta = ReqTask;
				}
				break;

			case ReqTask:
				{
					dwSleepTm = 50000;

					// 等待任务请求应答
					int iret = _WaitForAnswer(WAIT_ANS_TIME, 0x12, 0x01);

					if( iret == 1 )		// 应答成功
					{
						Frm1201 *frm = (Frm1201*)(m_bytRecvBuf+2);

						if( frm->m_bytRegFlag == 1 )
						{
							m_dwSoftId = ntohl(frm->m_ulSoftID);
							m_dwLineId = ntohl(frm->m_ulLineID);
							m_bytSta = DispTask;
						}
						else
						{ 
							PRTMSG(MSG_NOR, "Car is not register!\n"); 
							m_bytSta = Idlesse;
						}
					}
					else if( iret == 2 )
					{
						m_bytSta = NetWick;
					}
					else
					{
						m_bytSta = Idlesse;
						PRTMSG(MSG_NOR, "No download task, turn to Idlesse\n");
					}					
				}
				break;

			case DispTask:
				{
					// 有应用软件升级包需要下载
					if( m_dwSoftId )
					{
						//通知DvrExe开始远程升级，防止进入省电状态
						char by = 0x03;
						DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
	
						m_dwFileId = m_dwSoftId;
						m_bytFlstNo = SOFT;//此处应区分为SOFT、ROOT、KERN、BOOT
						m_bytFileType = 1;

						// 若该文件已存在，且文件ID相同，且已下载一部分，但未下完，则断点续传
						if( m_objFileList.m_objFile[m_bytFlstNo].m_bExist
							&& m_objFileList.m_objFile[m_bytFlstNo].m_ulID == m_dwSoftId
							&& m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize > 0
							&& m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize < m_objFileList.m_objFile[m_bytFlstNo].m_ulFileSize)
						{
							PRTMSG(MSG_NOR, "Continue download soft\n");

							m_dwFileSize = m_objFileList.m_objFile[m_bytFlstNo].m_ulFileSize;
							m_dwDownSize = m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize;
							memcpy(m_bytMd5, m_objFileList.m_objFile[m_bytFlstNo].m_bytMd5, sizeof(m_bytMd5));
							
							m_bytSta = ChkFree;
						} 
						// 否则是未下载过的，或需要重新下载
						else 
						{
							_DeleteFile(m_dwFileId);

							m_dwFileSize = 0;
							m_dwDownSize = 0;
							m_dwDownFrameCt = 0;
							memset(m_bytMd5, 0, sizeof(m_bytMd5));

							Frm2181 frm;
							frm.m_bytType = m_bytFileType;
							frm.m_ulID = htonl(m_dwFileId);

							_SendOneFrame(0x21, 0x81, (char*)&frm, sizeof(frm), true, RETRAN_INTERVAL);
							m_bytSta = ReqAttr;
						}	
						break;
					}

					// 有线路文件需要下载
					if( m_dwLineId )
					{
						//通知DvrExe开始远程下载，防止进入省电状态
						char by = 0x03;
						DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
						
						m_dwFileId = m_dwLineId;
						m_bytFlstNo = LINE;
						m_bytFileType = 6;
						
						// 若该文件已存在，且文件ID相同，且已下载一部分，但未下完，则断点续传
						if( m_objFileList.m_objFile[m_bytFlstNo].m_bExist
							&& m_objFileList.m_objFile[m_bytFlstNo].m_ulID == m_dwLineId
							&& m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize > 0
							&& m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize < m_objFileList.m_objFile[m_bytFlstNo].m_ulFileSize)
						{
							PRTMSG(MSG_NOR, "Continue download LineFile\n");
							
							m_dwFileSize = m_objFileList.m_objFile[m_bytFlstNo].m_ulFileSize;
							m_dwDownSize = m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize;
							memcpy(m_bytMd5, m_objFileList.m_objFile[m_bytFlstNo].m_bytMd5, sizeof(m_bytMd5));
							
							m_bytSta = ChkFree;
						} 
						// 否则是未下载过的，或需要重新下载
						else 
						{
							_DeleteFile(m_dwFileId);
							
							m_dwFileSize = 0;
							m_dwDownSize = 0;
							m_dwDownFrameCt = 0;
							memset(m_bytMd5, 0, sizeof(m_bytMd5));
							
							Frm2181 frm;
							frm.m_bytType = m_bytFileType;
							frm.m_ulID = htonl(m_dwFileId);
							
							_SendOneFrame(0x21, 0x81, (char*)&frm, sizeof(frm), true, RETRAN_INTERVAL);
							m_bytSta = ReqAttr;
						}	
						break;
					}

					// 没有下载任务，退回空闲
					PRTMSG(MSG_NOR, "No download task, turn to Idlesse\n");
					m_bytSta = Idlesse; 
				}
				break;

			case ReqAttr:
				{
					dwSleepTm = 50000;

					int iret = _WaitForAnswer(WAIT_ANS_TIME, 0x21, 0x01);

					if(iret == 1)	
					{
						Frm2101 *frm = (Frm2101*)(m_bytRecvBuf+2);

						if(frm->m_bytFlag == 0) 
						{
							ulong id = ntohl(frm->m_ulID);
							
							if(id == m_dwFileId) 
							{
								// 更新下载属性
								m_dwDownSize = 0;
								m_dwFileSize = ntohl(frm->m_ulFileSize);
								memcpy(m_bytMd5, frm->m_bytMd5, 32);

								if(0 == strcmp((char*)m_bytMd5, ""))
									PRTMSG(MSG_NOR, "The downfile's Md5 is null!\n");

#if WIRELESS_DOWNLOAD_TYPE == 0
								// 创建文件
								char szFileName[MAX_PATH] = {0};

								if( m_bytFlstNo == SOFT )
									sprintf(szFileName, "/mnt/Flash/part3/Down/exe.bin");
								if( m_bytFlstNo == LINE )
									sprintf(szFileName, "/mnt/Flash/part3/Down/Line.dat");
								
								FILE *fp = fopen(szFileName, "wb+");
								if(!fp)
								{ 
									PRTMSG(MSG_ERR, "Create downfile fail:"); 
									perror(" ");
									m_bytSta=Idlesse; 
									break; 
								}
								else
								{
									fclose(fp);
								}
#endif
#if WIRELESS_DOWNLOAD_TYPE == 1
								// 创建下载缓存
								m_pAllDownBuf = (byte*)malloc(m_dwFileSize);
								if(m_pAllDownBuf == NULL)
								{
									m_bytSta=Idlesse; 
									break; 
								}
								memset(m_pAllDownBuf, 0, m_dwFileSize);
#endif
								// 更新文件列表
								memset((void*)&m_objFileList.m_objFile[m_bytFlstNo], 0, sizeof(FileAttr));
								m_objFileList.m_objFile[m_bytFlstNo].m_bExist = true;
								m_objFileList.m_objFile[m_bytFlstNo].m_ulID = m_dwFileId;
								m_objFileList.m_objFile[m_bytFlstNo].m_ulFileSize = m_dwFileSize;
								m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize = m_dwDownSize;
								memcpy(m_objFileList.m_objFile[m_bytFlstNo].m_bytMd5, m_bytMd5, 32);

								_RenewFileLst();

								// 清空下载缓冲
								if(m_pDownBuf)
								{
									free(m_pDownBuf);
									m_pDownBuf = NULL; 
								}
								m_usBlockSize = 0;
								m_usFrameSize = 0;
								memset(m_bytBlockSta, 0, 1024);
								
								m_bytSta = ChkFree;
								break;
							} 
							else 
							{ 
								PRTMSG(MSG_NOR, "The Id of File is not match!\n"); 
								m_bytSta=Idlesse; 
								break; 
							}
						} 
						else if(frm->m_bytFlag == 1)	//1表示失败，中心没有该车台ID
						{			
							PRTMSG(MSG_NOR, "The car id is unlaw\n"); 
							m_bytSta = Idlesse; 
							break;
						} 
						else if(frm->m_bytFlag == 2)	//2表示失败，节目不存在
						{			
							PRTMSG(MSG_NOR, "File(%08X) is not exist, get attr fail\n", m_dwFileId);	
							_DeleteFile(m_dwFileId);
							m_bytSta = Idlesse;
							break;
						}
						else if(frm->m_bytFlag == 3)	//3表示失败，服务器忙
						{			
							PRTMSG(MSG_NOR, "Server busy, get attr fail\n"); 
							m_bytSta = Idlesse; 
							break;
						}
						else
						{
							PRTMSG(MSG_NOR, "Other reply(%02X), get attr fail\n", frm->m_bytFlag);
							m_bytSta = Idlesse; 
							break;
						}
					}
					else if(iret == 2)
					{
						m_bytSta = NetWick;
					}
					else
					{ 
						m_bytSta = Idlesse; 
					}
				}
				break;

			case ChkFree:	//待讨论，是否需要检查磁盘空间
				{
					dwSleepTm = 0;

					// 发送下载请求
					Frm2182 frm;
					frm.m_ulID = htonl(m_dwFileId);
					frm.m_ulBeginLoation = htonl(m_dwDownSize);

					_SendOneFrame(0x21, 0x82, (char*)&frm, sizeof(frm), true, RETRAN_INTERVAL);

					m_bytSta = ReqDown;
				}
				break;

			case ReqDown:
				{
					dwSleepTm = 50000;

					int iret = _WaitForAnswer(WAIT_ANS_TIME, 0x21, 0x02);

					if(iret == 1) 
					{
						Frm2102 *pfrm = (Frm2102*)(m_bytRecvBuf+2);

						ulong id = ntohl(pfrm->m_ulID);

						if(id == m_dwFileId) 
						{
							byte flag = pfrm->m_bytFlag;

							if(flag == 0) 
							{
								ushort blksize1 = ntohs(pfrm->m_usBlockSize);
								ushort frmsize1 = ntohs(pfrm->m_usFrameSize);
								
								//块帧大小发生改变
								if( blksize1 != m_usBlockSize || frmsize1 != m_usFrameSize ) 
								{	
									m_usBlockSize = blksize1;		
									m_usFrameSize = frmsize1;

									if(m_pDownBuf != NULL)
									{ 
										free(m_pDownBuf); 
										m_pDownBuf=NULL; 
									}
									m_pDownBuf = (byte*)malloc(m_usBlockSize*m_usFrameSize);
									
									if(m_pDownBuf==NULL)
									{ 
										m_bytSta=Idlesse; 
										break; 
									}
									
									memset(m_pDownBuf, 0, m_usBlockSize*m_usFrameSize);
									memset(m_bytBlockSta, 0, 1024);
								}
								
								// 发送文件传输请求
								ushort usCurBlockSize = (ushort)min( (m_dwFileSize - m_dwDownSize - 1)/m_usFrameSize + 1, m_usBlockSize);
								
								Frm2183 frm;
								frm.m_usBlockNo = htons(m_usBlockNo);
								int iStaLen = _ConvertBlockSta(m_bytBlockSta, frm.m_bytBlockSta, usCurBlockSize);
								
								_SendOneFrame(0x21, 0x83, (char*)&frm, 2+iStaLen, true, RETRAN_INTERVAL);
								
								m_timeout = GetTickCount();
								m_bytSta = WaitData;
								break;
							}
							else if(flag == 1)		//服务器忙
							{	
								PRTMSG(MSG_NOR, "Server busy! Down fail!\n");
								m_bytSta = Idlesse;
								break;
							}
							else				//表示节目不存在
							{	
								PRTMSG(MSG_NOR, "Ad(%08X) is not exist! Down fail!\n", m_dwFileId);
								_DeleteFile(m_dwFileId);
								m_bytSta = Idlesse;
								break;
							}							
						} 
						else	//请求与应答的节目ID不匹配
						{
							PRTMSG(MSG_NOR, "The Id of Ad is not match2!\n");
							m_bytSta = Idlesse;	
							break;
						}
					}
					else if(iret == 2)
					{ 
						m_bytSta = NetWick; 
					}
					else
					{
						m_bytSta = Idlesse;
					}
				}
				break;

			case WaitData:
				{
					dwSleepTm = 50000;

					ulong t1 = GetTickCount() - m_timeout;
					
					//超过3秒没有收到数据,则往中心发送链路维护帧,防止端口失效
					static int cnt = 0;
					if(t1 > 3000) 
					{	
						m_timeout = GetTickCount();
						_SendOneFrame(0x21, 0x86, NULL, 0, false ,0); 
						cnt++;
					}
					
					if(cnt > 12)
					{ 
						cnt = 0; 
						m_bytSta = NetWick; 
						PRTMSG(MSG_NOR, "Rcv block overtime\n"); 
						break; 
					}

					if(m_bReturnIdl)
					{ 
						cnt = 0; 
						m_bytSta=Idlesse; 
						m_bReturnIdl = false;
						break; 
					}

					if( !m_objRecvMsg.PopData(bytLvl, sizeof(m_bytRecvBuf), m_dwRecvLen, (char*)m_bytRecvBuf, dwPushTm) )
					{
						_KillTimer(&g_objTimerMng, RETRAN_TIMER);

						cnt = 0;
						m_timeout = GetTickCount();

						// 若是数据帧
						if( m_bytRecvBuf[0] == 0x21 && m_bytRecvBuf[1] == 0x03 )
						{	
							m_bytSta = RcvData; 
							dwSleepTm = 0;
							break; 
						}

						// 若是块状态查询帧
						if(m_bytRecvBuf[0] == 0x21 && m_bytRecvBuf[1] == 0x04)
						{
							Frm2104 *pfrm = (Frm2104*)(m_bytRecvBuf+2);

							// 如当前块已接收完毕,m_usBlockNo会加1,则会跳到else if执行
							if( m_usBlockNo == ntohs(pfrm->m_usBlockNo) )		// 块序号相同(说明此块还未满)
							{	
								ushort usCurBlockSize = (ushort)min( (m_dwFileSize-m_dwDownSize-1)/m_usFrameSize + 1, m_usBlockSize);
								Frm2184 frm;

								frm.m_bytQueryNo = pfrm->m_bytQueryNo;
								frm.m_usBlockNo = htons(m_usBlockNo);

								int iStaLen = _ConvertBlockSta(m_bytBlockSta, frm.m_bytBlockSta, usCurBlockSize);
								_SendOneFrame(0x21, 0x84, (char*)&frm, 3+iStaLen, true, RETRAN_INTERVAL);
							}
							else if( (m_usBlockNo-1) == ntohs(pfrm->m_usBlockNo))	// 块序号不同(说明此块已满,则发送下一块)
							{
								Frm2184 frm;
								frm.m_bytQueryNo = pfrm->m_bytQueryNo;
								frm.m_usBlockNo = htons(m_usBlockNo);

								_SendOneFrame(0x21, 0x84, (char*)&frm, sizeof(frm), true, RETRAN_INTERVAL);
							} 
							else
							{
								PRTMSG(MSG_ERR, "localblk=%d, remoteblk=%d", m_usBlockNo, ntohs(pfrm->m_usBlockNo));
							}
							break;
						}
						// 其他不处理
					}
				}
				break;	

			case RcvData:	//将接收的数据存入块缓冲区中
				{
					dwSleepTm = 50000;

					Frm2103 *pfrm = (Frm2103*)(m_bytRecvBuf+2);					
					int iFrameID = ntohs(pfrm->m_usFrameID);

					// 若收到错误的数据帧，则丢弃
					if( ntohs(pfrm->m_usBlockNo) != m_usBlockNo || iFrameID+1 > m_usBlockSize)
					{
						m_bytSta = WaitData;
						PRTMSG(MSG_NOR, "recv wrong frame!\n");
						break;
					}
					
					// 将帧数据copy至块缓冲区
					memcpy(m_pDownBuf+iFrameID*m_usFrameSize, pfrm->m_bytFrameBuf, m_usFrameSize);
					m_bytBlockSta[iFrameID] = 1;
					m_dwDownFrameCt++;

					PRTMSG(MSG_NOR, "%d k/%d k\n", (m_dwDownSize+m_dwDownFrameCt*m_usFrameSize)/1024, m_dwFileSize/1024);
					dwSleepTm = 0;

					m_bytSta = ChkBuf;
				}
				break;

			case ChkBuf:
				{
					// 获得当前块的实际大小
					ulong ulCurBlockSize = min( (m_dwFileSize-m_dwDownSize-1)/m_usFrameSize + 1, m_usBlockSize);
					
					// 判断缓冲区是否满
					bool bDownBufFul = true;
					for(ulong i = 0; i < ulCurBlockSize; i++) 
					{
						if(m_bytBlockSta[i]==0)	
						{
							bDownBufFul=false; 
							break; 
						}
					}
					
					// 缓冲区满，写入文件当中
					if(bDownBufFul)
					{
#if WIRELESS_DOWNLOAD_TYPE == 0
						char szFileName[100] = {0};

						if( m_bytFlstNo == SOFT )
							sprintf(szFileName, "/mnt/Flash/part3/Down/exe.bin");
						if( m_bytFlstNo == LINE )
							sprintf(szFileName, "/mnt/Flash/part3/Down/Line.dat");
						
						FILE *fp = fopen(szFileName, "rb+");
						if(fp)
#endif
#if WIRELESS_DOWNLOAD_TYPE == 1
						if(m_pAllDownBuf)
#endif
						{
							// 算出最后一个块的数据大小
							ulong ulLastSize = min(ulCurBlockSize*m_usFrameSize, m_dwFileSize-m_dwDownSize);
#if WIRELESS_DOWNLOAD_TYPE == 0
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
#endif
#if WIRELESS_DOWNLOAD_TYPE == 1
							memcpy(m_pAllDownBuf+m_dwDownSize, m_pDownBuf, ulLastSize);
#endif
							// 更新下载标志
							m_usBlockNo++;
							m_dwDownSize += ulLastSize;
							m_dwDownFrameCt = 0;
							m_objFileList.m_objFile[m_bytFlstNo].m_ulDownSize = m_dwDownSize;
							_RenewFileLst();
							
							// 清空缓冲区
							memset(m_pDownBuf, 0, m_usBlockSize*m_usFrameSize);
							memset(m_bytBlockSta, 0, 1024);
							
							m_bytSta = WaitData;							
						} 
						else 
						{
							PRTMSG(MSG_ERR, "Open file fail when write:");
							perror(" ");
							_DeleteFile(m_dwFileId);
							m_bytSta=Idlesse;

							break; 
						}

						// 文件下载完毕
						if(m_dwDownSize == m_dwFileSize) 
						{
#if WIRELESS_DOWNLOAD_TYPE == 1
							// 创建文件
							char szFileName[MAX_PATH] = {0};
							
							if( m_bytFlstNo == SOFT )
								sprintf(szFileName, "/mnt/Flash/part3/Down/exe.bin");
							if( m_bytFlstNo == LINE )
								sprintf(szFileName, "/mnt/Flash/part3/Down/Line.dat");
								
							FILE *fp = fopen(szFileName, "wb+");
							if(fp)
							{
								// 将数据写入文件
								size_t ret = fwrite(m_pAllDownBuf, m_dwFileSize, 1, fp);
								if(ret != 1)
								{ 
									fclose(fp); 
									_DeleteFile(m_dwFileId);
									m_bytSta = Idlesse;
									break; 
								}
								fclose(fp);
								
								if(m_pAllDownBuf)
								{
									free(m_pAllDownBuf);
									m_pAllDownBuf = NULL;
								}
							}
							else
							{
								PRTMSG(MSG_ERR, "Open file fail when write:");
								perror(" ");
								_DeleteFile(m_dwFileId);
								m_bytSta = Idlesse;
								break; 
							}
#endif
							memset(m_bytBlockSta, 0, 1024);
							m_bytSta = ChkMd5;
						}
					}
					else	// 缓冲未满，则继续等待接收
					{ 
						m_bytSta = WaitData;
					}
				}
				break;
			
			case ChkMd5:
				{
					// 获取下载文件实际的md5值
					CMd5 cmd5;
					char md5_new[32] = {0};

					char szFileName[100] = {0};

					if( m_bytFlstNo == SOFT )
						sprintf(szFileName, "/mnt/Flash/part3/Down/exe.bin");
					if( m_bytFlstNo == LINE )
						sprintf(szFileName, "/mnt/Flash/part3/Down/Line.dat");

					if(!cmd5.MD5File(szFileName, md5_new))
						cmd5.MD5File(szFileName, md5_new);	//为何要算两次？
					
					// 比较两个md5是否相等
					m_bMd5ok = (0 == strncmp(md5_new,(char*)m_bytMd5,32)) ? true : false;
				
					if(!m_bMd5ok)
					{	
						PRTMSG(MSG_ERR, "Check Md5 Err(%08X), delete the file\n", m_dwFileId);
						_DeleteFile(m_dwFileId);	
					}	
					
					// 发送文件传输结果
					Frm2185 frm;
					frm.m_bytType = (byte)m_bytFileType;
					frm.m_ulID = htonl(m_dwFileId);
					frm.m_bytFlag = byte(m_bMd5ok ? 0 : 1);

					_SendOneFrame(0x21, 0x85, (char*)&frm, sizeof(frm), true, RETRAN_INTERVAL);
					
					m_bytSta = TranResult;
				}
				break;	
				
			case TranResult:
				{
					dwSleepTm = 50000;

					int iret = _WaitForAnswer(WAIT_ANS_TIME, 0x21, 0x05);

					if(iret == 1) 
					{
						if( m_bMd5ok && SOFT == m_bytFlstNo )
						{ 
							m_bytSta = SoftUpdate; 
							break; 
						}
						if( m_bMd5ok && LINE == m_bytFlstNo )
						{
							m_bytSta = LineFile;
							break;
						}
						
						// 其他则退回空闲
						m_bytSta = Idlesse;
					}
					else if(iret == 2)
					{ 
						m_bytSta = NetWick;
					}
					else if(iret == 3)	
					{
						m_bytSta = Idlesse; 
					}
				}
				break;

			case SoftUpdate:
				{
// 					m_bytSta = Idlesse;
// 					break;

					dwSleepTm = 0;

					PRTMSG(MSG_NOR, "Begin soft update\n");

					char szFileName[MAX_PATH] = {0};
					//sprintf(szFileName, "%s%08X.dat", LIU_DOWNLOAD_PATH, m_dwFileId);
					sprintf(szFileName, "/mnt/Flash/part3/Down/exe.bin");

					if( AppIntact(szFileName) == true && G_ExeUpdate(szFileName, 0))
					{
						// 升级成功后程序从分区5启动
						DelErrLog();
		
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

			case LineFile:
				{
					PRTMSG(MSG_NOR, "Rcv Line File OK!\n" );

					char szFileName[MAX_PATH] = {0};
					sprintf(szFileName, "/mnt/Flash/part3/Down/Line.dat");

					if( access(szFileName, F_OK) == 0 )
						PRTMSG(MSG_DBG, "Line file: %s\n", szFileName);
					
					BYTE aryBuf[20] = {0};

					aryBuf[0] = 0x8e;
					aryBuf[1] = 0xf2;
					DataPush(aryBuf, 2, DEV_DIAODU, DEV_QIAN, LV2);
// 					aryBuf[2] = BYTE(m_dwFileId/256/256/256);
// 					aryBuf[3] = BYTE(m_dwFileId/256/256%256);
// 					aryBuf[4] = BYTE(m_dwFileId/256%256);
// 					aryBuf[5] = BYTE(m_dwFileId%256);
// 					aryBuf[6] = 0;
// 					DataPush(aryBuf, 7, DEV_DIAODU, DEV_QIAN, LV2);

					m_bytSta = Idlesse;	
				}
				break;

			default:
				break;
		}

		usleep(dwSleepTm);
	}
}

void CDownLoad::P_TmRetran()
{
	DWORD dwPacketNum = 0;
	m_objSendMsg.PushData(1, m_dwSendLen, (char*)m_bytSendBuf, dwPacketNum);

	UPDATEMSG msg;
	msg.m_lMsgType = MSG_SOCK_SEND;
	msgsnd(g_iMsgId, (void*)&msg, MSG_MAXLEN, 0);
}

// 此函数并不进行真正的发送操作，只将数据帧推入发送队列，由SendSockData来实际发送
void CDownLoad::_SendOneFrame(char v_szTranType, char v_szDataType, char *v_szFrameBuf, int v_iFrameLen, bool v_bNeedRetran, DWORD v_dwRetranTm)
{
	char szSendBuf[UDP_SENDSIZE] = {0};
	DWORD  dwSendLen = 0;
	DWORD dwPacketNum = 0;
	DWORD dwCpyBytes = 0;

	// 组帧
	ulong ulCarId = htonl(m_ulCarId);
	memcpy(szSendBuf+1, (void*)&ulCarId, sizeof(ulCarId));
	szSendBuf[5] = v_szTranType;
	szSendBuf[6] = v_szDataType;

	dwCpyBytes = DWORD(v_iFrameLen) < sizeof(szSendBuf) - 7 ? DWORD(v_iFrameLen) : sizeof(szSendBuf) - 7;
	memcpy(szSendBuf+7, v_szFrameBuf, dwCpyBytes);
	dwSendLen = v_iFrameLen+7;
	szSendBuf[0] = (char)_GetCrc((byte*)szSendBuf+1, v_iFrameLen+6);

	// 推入发送队列
	m_objSendMsg.PushData(1, dwSendLen, szSendBuf, dwPacketNum);	
	// 同时保存一份以备重传
	memcpy(m_bytSendBuf, szSendBuf, dwSendLen);
	m_dwSendLen = dwSendLen;
	// 通知发送
	UPDATEMSG msg;
	msg.m_lMsgType = MSG_SOCK_SEND;
	msgsnd(g_iMsgId, (void*)&msg, MSG_MAXLEN, 0);

	// 开始重传定时
	if(v_bNeedRetran)
		_SetTimer(&g_objTimerMng, RETRAN_TIMER, RETRAN_INTERVAL, G_TmRetran);
}

void CDownLoad::AnalyseRecvData(char *v_szRecvBuf, DWORD v_dwRecvLen)
{
//	PRTMSG(MSG_NOR, "updata recv from sock: ");
//	PrintString(v_szRecvBuf, v_dwRecvLen);

// 	// 非中心协议帧
// 	if( v_szRecvBuf[0] == 0x01 )
// 	{
// 		switch( v_szRecvBuf[1] )
// 		{
// 			case 0x01:		// SockServExe通知网络可用
// 				m_bNetFined = true;
// 				m_bLoginNow = true;
// 				break;
// 	
// 			case 0x02:		// 网络不可用
// 				m_bNetFined = false;
// 				break;
// 	
// 			case 0x03:		// 流媒体立即登陆请求
// 				m_bLoginNow = true;
// 				break;
// 	
// 			default:
// 				break;
// 		}
// 
// 		return;
// 	}
// 	// 中心协议帧
// 	else if( v_szRecvBuf[0] == 0x02 )

	{
		byte bytCrc;
		ulong ulCarId;
		
		// 检查校验和
		bytCrc = _GetCrc((byte*)v_szRecvBuf+2, v_dwRecvLen-2);
		if( bytCrc != v_szRecvBuf[1] )
		{
			PRTMSG(MSG_ERR, "liu check crc err!\n");
			return;
		}

		// 检查车辆ID
		memcpy((void*)&ulCarId, v_szRecvBuf+2, 4);
		if( m_ulCarId != htonl(ulCarId) )
			return;

		// 推入接收队列
		DWORD dwPacketNum = 0;
		m_objRecvMsg.PushData((byte)LV1, v_dwRecvLen-6, v_szRecvBuf+6, dwPacketNum);
	}
}

void CDownLoad::SendSockData()
{
	byte	bytLvl = 0;
	DWORD	dwPushTm = 0;
	char	szBuf[MAX_FRAME_LEN] = {0};
	DWORD	dwLen = 0;	

	szBuf[0] = 0x04;	// 0x04 标示Update的udp数据帧

	while( !m_objSendMsg.PopData(bytLvl, sizeof(szBuf)-1, dwLen, szBuf+1, dwPushTm) )
	{
		DataPush(szBuf, dwLen+1, DEV_UPDATE, DEV_SOCK, 2);

		usleep(10000);
	}
}

// 等待中心应答, 类型：1——应答到， 2——应答未到， 3——退回空闲
int  CDownLoad::_WaitForAnswer(DWORD v_dwWaitTm, char v_szTranType, char v_szDataType)
{
	DWORD dwBeginWaiTm = GetTickCount();
	byte  bytLvl;
	DWORD dwPushTm;
	int	  iResult = 2; // 缺省未应答未到

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
			if( v_szTranType == m_bytRecvBuf[0] && v_szDataType == m_bytRecvBuf[1] )
			{
				iResult = 1;
				goto _WAIT_END;
			}
		}
		else
		{
			usleep(20000);
		}
	}

_WAIT_END:
	_KillTimer(&g_objTimerMng, RETRAN_TIMER);
	return iResult;
}

bool CDownLoad::_SetTimeToRtc(struct tm* v_SetTime)
{
    struct rtc_time objRtcTime;

    memset(&objRtcTime, 0, sizeof(objRtcTime));

    int iDevRtc = open("/dev/misc/rtc", O_NONBLOCK);

    if(-1 == iDevRtc)
    {
        PRTMSG(MSG_ERR,"open rtc dev err\n");
        return false;
    }
        
    objRtcTime.tm_sec = v_SetTime->tm_sec;
    objRtcTime.tm_min = v_SetTime->tm_min;
    objRtcTime.tm_hour = v_SetTime->tm_hour;
    objRtcTime.tm_mday = v_SetTime->tm_mday;
    objRtcTime.tm_mon = v_SetTime->tm_mon;
    objRtcTime.tm_year = v_SetTime->tm_year+1900;

    if(0 != ioctl(iDevRtc, RTC_SET_TIME, &objRtcTime))
    {
        close(iDevRtc);
        iDevRtc = -1;
        return false;
    }

    PRTMSG(MSG_DBG,"set server time to rtc\n");

    close(iDevRtc);
    iDevRtc = -1;

    return true;
}

bool CDownLoad::_DeleteFile(DWORD v_dwID)
{
	int i = 0;
	char szFileName[MAX_PATH] = {0};

	for(i = 0; i < FILE_TYPE_COUNT; i++) 
	{
		if( m_objFileList.m_objFile[i].m_ulID == v_dwID && m_objFileList.m_objFile[i].m_bExist )  
		{
			sprintf(szFileName, "%s%08x.dat", LIU_DOWNLOAD_PATH,v_dwID);
			
			// 若文件存在，则删除该文件
			if( access(szFileName, F_OK) == 0 )
			{
				unlink(szFileName);
			}

#if WIRELESS_DOWNLOAD_TYPE == 1
			if(m_pAllDownBuf)
			{
				free(m_pAllDownBuf);
				m_pAllDownBuf = NULL;
			}
#endif

			memset((void*)&m_objFileList.m_objFile[i], 0, sizeof(FileAttr));
			m_objFileList.m_objFile[i].m_bExist = false;

			// 更新文件列表
			_RenewFileLst();
			return true;
		}
	}

	return false;
}

bool CDownLoad::_RenewFileLst()
{
#if WIRELESS_DOWNLOAD_TYPE == 0
	char szFileName[MAX_PATH] = {0};

	sprintf(szFileName, "%sFileLst.dat", LIU_DOWNLOAD_PATH);
	
	FILE *fp = fopen(szFileName, "rb+");
	
	if(!fp)
		return false;

	fseek(fp, 0, SEEK_SET);
	fwrite(&m_objFileList, sizeof(m_objFileList), 1, fp);	
	fclose(fp);
#endif

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------
// 转换块状态
int CDownLoad::_ConvertBlockSta(byte *v_bytSta_in, byte *v_bytSta_out, ushort v_usCurBlockSize)
{
	byte *b = v_bytSta_in;

	if(!v_bytSta_in || !v_bytSta_out)
		return 0;

	memset(b+v_usCurBlockSize, 1, 1024-v_usCurBlockSize);

	int istalen = (v_usCurBlockSize%8) ? (v_usCurBlockSize/8 + 1) : (v_usCurBlockSize/8) ;

	for(int i = 0; i < istalen; i++) 
	{
		v_bytSta_out[i] = (byte)( b[7+i*8]<<7 | b[6+i*8]<<6 | b[5+i*8]<<5 | b[4+i*8]<<4 | b[3+i*8]<<3 | b[2+i*8]<<2 | b[1+i*8]<<1 | b[0+i*8] ) ;
	}

	return istalen;
}

// 获取车辆ID（需补充）
void CDownLoad::_GetCarId(ulong &v_ulCarId)
{
	return;
}

byte CDownLoad::_GetCrc(const byte *v_bytCarId, const int v_iLen)
{
	byte sum = 0xff;
	int	j = 0;

	for( int i =v_iLen; i > 0; i-- ) 
	{
		sum = CrcTable[sum ^ v_bytCarId[j++]];
	}

	return (byte)(0xff-sum);
}


