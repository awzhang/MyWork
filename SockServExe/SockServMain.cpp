#include "StdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD	("SockServMain-Main    ")

int _InitAll()
{
	g_bProgExit = false;
	
	// 定时器初始化需放在最前面
	g_objTimerMng.Init();	
	
	// 初始化互斥量
	pthread_mutex_init(&g_mutexTimer, NULL);
	pthread_mutex_init(&g_mutexFlowControl, NULL);
	
#if USE_LIAONING_SANQI == 0
	g_objQianTcp.Init();
	
#endif
	g_objQianUdp.Init();

#if USE_VIDEO_TCP == 1
	g_objDvrTcp.Init();
#endif
	
	g_objDvrUdp.Init();
	
#if WIRELESS_UPDATE_TYPE == 1
	g_objUpdateTcp.Init();
#endif
	
	g_objUpdateUdp.Init();
}

int _ReleaseAll()
{
#if USE_LIAONING_SANQI == 0
	g_objQianTcp.Release();
	
#endif
	g_objQianUdp.Release();
	
#if USE_VIDEO_TCP == 1
	g_objDvrTcp.Release();
#endif

	g_objDvrUdp.Release();
	
#if WIRELESS_UPDATE_TYPE == 1
	g_objUpdateTcp.Release();
#endif
	
	g_objUpdateUdp.Release();
	
	g_objTimerMng.Release();
	
	pthread_mutex_destroy( &g_mutexTimer );
	pthread_mutex_destroy( &g_mutexFlowControl );
}

void *G_ClrDogThread(void *arg)
{
	int i = 0;
	while( !g_bProgExit )
	{
		if( i++ % 5 == 0 )
		{
			DogClr( DOG_SOCK );
		}
		
		sleep(1);
	}
}

int main()
{
	ListPhoneInfo();
	
	char szBuf[ 2048 ] = {0};
	BYTE bytLvl;
	DWORD dwPopLen;
	DWORD dwPushSymb;
	DWORD dwPacketNum = 0;
	int iResult;
	pthread_t pthread_ClrDog;
	
	_InitAll();
	
	usleep(100000);
	
	// 创建清看门狗线程
	if( pthread_create(&pthread_ClrDog, NULL, G_ClrDogThread, NULL) != 0 )
	{
		perror("create thread failed: ");
		_ReleaseAll();
		
		return ERR_THREAD;
	}
	
	PRTMSG(MSG_NOR, "SockServExe begin to run!\n");
	
	while( !(iResult = DataWaitPop(DEV_SOCK)) )
	{
		if( g_bProgExit )
			break;
		
		while( !DataPop( szBuf, sizeof(szBuf), &dwPopLen, &dwPushSymb, DEV_SOCK, &bytLvl ) )
		{
			switch(dwPushSymb)
			{
			case DEV_QIAN:
				{
					// 非中心协议帧
					if( szBuf[0] != 0x7e )
					{
						switch(szBuf[0])
						{
						case 0x01:		// QianExe请求重新连接套接字
							{
								PRTMSG(MSG_NOR, "Recv QianExe reconnect inform!\n");
								G_RecordDebugInfo("Recv QianExe reconnect inform!");
								
								szBuf[0] = 0x01;
								g_objQianTcp.m_objSockStaMng.PushData(LV1, 1, szBuf, dwPacketNum);
							}
							break;
							
						case 0x02:		// 视频监控建链的IP和端口
							{
								PRTMSG(MSG_DBG, "Recv Video Mon Link!\n");
								
								char buf[15] = {0};
								strncpy( buf, szBuf + 1, 15 );
								g_objDvrUdp.m_ulIp = inet_addr( buf );
								PRTMSG(MSG_DBG, "IP: %s\n", buf);
								
								memset( buf, 0, sizeof(buf) );
								strncpy( buf, szBuf + 16, 5 );
								g_objDvrUdp.m_usPort = htons( short(atoi(buf)) );
								PRTMSG(MSG_DBG, "Port: %s\n", buf);
#if USE_VIDEO_TCP == 0
								// 同时保存到配置中
								tag1QIpCfg	obj1QIpCfg;
								GetImpCfg( &obj1QIpCfg, sizeof(obj1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
								obj1QIpCfg.m_ulVUdpIP = g_objDvrUdp.m_ulIp;
								obj1QIpCfg.m_usVUdpPort = g_objDvrUdp.m_usPort;
								SetImpCfg( &obj1QIpCfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
#endif
							}
							break;
							
						case 0x03:		// QianExe通知SockServExe TCP登陆成功
							{
								g_objQianUdp.ReqCreateSock();
								usleep(1000);
#if USE_VIDEO_TCP == 0
								g_objDvrUdp.ReqCreateSock();
								usleep(1000);	
#endif
							}
							break;
							
						case 0x04:		// QianExe UDP数据
							{
								g_objQianUdp.m_objSendMng.PushData(bytLvl, dwPopLen-1, szBuf+1, dwPacketNum, 0, NULL, 0 );
							}
							break;
							
						default:
							break;
						}
					}
					else
						g_objQianTcp.m_objSendMng.PushData( bytLvl, dwPopLen, szBuf, dwPacketNum, 0, NULL, 0 );
				}
				break;
				
			case DEV_DVR:
				{
					switch(szBuf[0])
					{
					case 0x07:
						if (NETWORK_TYPE == NETWORK_TD)
						{
							//g_objDvrUdp.m_dwSendBytesPerTime = 1500;
							g_objDvrUdp.m_dwSleepTm = 80000;
						}
						else if (NETWORK_TYPE == NETWORK_EVDO)
						{
							g_objDvrUdp.m_dwSendBytesPerTime = 20000;
							g_objDvrUdp.m_dwSleepTm = 30000;
						}
						else if (NETWORK_TYPE == NETWORK_WCDMA)
						{
							g_objDvrUdp.m_dwSendBytesPerTime = 33000;
							g_objDvrUdp.m_dwSleepTm = 30000;
						}
						else if (NETWORK_TYPE == NETWORK_GSM)
						{
							g_objDvrUdp.m_dwSendBytesPerTime = 1000;
							g_objDvrUdp.m_dwSleepTm = 300000;
						}
						break;
						
					case 0x08:
						if (NETWORK_TYPE == NETWORK_TD)
						{
							//g_objDvrUdp.m_dwSendBytesPerTime = 1500;
							g_objDvrUdp.m_dwSleepTm = 200000;
						}
						else if (NETWORK_TYPE == NETWORK_EVDO)
						{
							g_objDvrUdp.m_dwSendBytesPerTime = 5000;
							g_objDvrUdp.m_dwSleepTm = 100000;
						}
						else if (NETWORK_TYPE == NETWORK_WCDMA)
						{
							g_objDvrUdp.m_dwSendBytesPerTime = 5000;
							g_objDvrUdp.m_dwSleepTm = 100000;
						}
						else if (NETWORK_TYPE == NETWORK_GSM)
						{
							g_objDvrUdp.m_dwSendBytesPerTime = 1000;
							g_objDvrUdp.m_dwSleepTm = 300000;
						}
						break;

					case 0x0a:
						{
							g_objDvrUdp.m_objSendMng.Release();
						}
						break;

#if USE_VIDEO_TCP == 1
					case 0x01:		// DvrExe请求重新连接套接字
						{
							PRTMSG(MSG_NOR, "Recv DvrExe reconnect inform!\n");
							G_RecordDebugInfo("Recv DvrExe reconnect inform!");
							
							szBuf[0] = 0x01;
							g_objDvrTcp.m_objSockStaMng.PushData(LV1, 1, szBuf, dwPacketNum);
						}
						break;

					case 0x03:		// DvrExe通知 TCP登陆成功
						{
							g_objDvrUdp.ReqCreateSock();
							usleep(1000);	
						}
						break;

					case 0x05:		// DvrExe请求SockServExe重新连接套接字，且附带有IP端口
						{
							int i = 0;
							int iCnt = min(szBuf[1], MAX_TEMP_IP_CNT);
							
							g_objDvrTcp.m_bFirstConnect = false;
							g_objDvrTcp.m_iIPCnt = iCnt;
							for(i=0; i<MAX_TEMP_IP_CNT; i++)
							{
								g_objDvrTcp.m_bIPEnabled[i] = false;
							}
							
							for(int i=0; i<iCnt; i++)
							{
								g_objDvrTcp.m_bIPEnabled[i] = true;
								memcpy((void*)&g_objDvrTcp.m_ulTempIP[i], szBuf+2+8*i, 4);
								
								unsigned short shPort = 0;
								memcpy((void*)&shPort, szBuf+2+8*i+4, 2);
								g_objDvrTcp.m_usTempTcpPort[i] = htons(shPort);
								memcpy((void*)&shPort, szBuf+2+8*i+6, 2);
								g_objDvrTcp.m_usTempUdpPort[i] = htons(shPort);
							}
							
							// 调试用，打印IP和端口
							{
								struct sockaddr_in tmp;
								
								for(int j=0; j<iCnt; j++)
								{
									tmp.sin_addr.s_addr = g_objDvrTcp.m_ulTempIP[j];
									PRTMSG(MSG_DBG, "IP: %s, tcp port: %d, udp port: %d\n", inet_ntoa(tmp.sin_addr), htons(g_objDvrTcp.m_usTempTcpPort[j]), htons(g_objDvrTcp.m_usTempUdpPort[j]) ); 
								}
							}
							
							// 成功后TCP再连接新的IP地址
							szBuf[0] = 0x01;
							g_objDvrTcp.m_objSockStaMng.PushData(LV1, 1, szBuf, dwPacketNum);
						}
						break;

					case 0x7e:		// TCP数据
						{
							g_objDvrTcp.m_objSendMng.PushData( bytLvl, dwPopLen, szBuf, dwPacketNum, 0, NULL, 0 );
						}
						break;
						
					case 0x09:		//DvrExe通知清除缓存中指定数据
						{
							PRTMSG(MSG_DBG, "清除通道%d缓存\n", szBuf[1]+1);
							g_objDvrUdp.m_objSendMng.DelSymbData(DelSymbData, &szBuf[1], sizeof(szBuf[1]));
						}
						break;
#endif

					default:		// 其他均当作DVR UDP数据处理
						g_objDvrUdp.m_objSendMng.PushData( LV2, dwPopLen, szBuf, dwPacketNum, 0, NULL, 0 );
						break;
					}
				}
				break;
				
			case DEV_UPDATE:
				{
					// 非中心协议帧
					if( szBuf[0] != 0x7e )
					{
						switch(szBuf[0])
						{
#if WIRELESS_UPDATE_TYPE == 1
						case 0x01:		// UpdateExe请求重新连接套接字
							{
								PRTMSG(MSG_NOR, "Recv UpdateExe reconnect inform!\n");
								
								szBuf[0] = 0x01;
								g_objUpdateTcp.m_objSockStaMng.PushData(LV1, 1, szBuf, dwPacketNum);
							}
							break;
							
						case 0x02:		// 车台维护中心通知远程升级的IP和端口
							{
								PRTMSG(MSG_DBG, "Recv Update request!\n");
								
								char buf[15] = {0};
								strncpy( buf, szBuf + 1, 15 );
								g_objUpdateUdp.m_ulIp = inet_addr( buf );
								PRTMSG(MSG_DBG, "IP: %s\n", buf);
								
								memset( buf, 0, sizeof(buf) );
								strncpy( buf, szBuf + 16, 5 );
								g_objUpdateUdp.m_usPort = htons( short(atoi(buf)) );
								PRTMSG(MSG_DBG, "Port: %s\n", buf);
							}
							break;
#endif
						case 0x04:		// UpdateExe UDP数据
							{
								g_objUpdateUdp.m_objSendMng.PushData(bytLvl, dwPopLen-1, szBuf+1, dwPacketNum, 0, NULL, 0 );
							}
							break;
							
						default:
							break;
						}
					}
#if WIRELESS_UPDATE_TYPE == 1
					else
						g_objUpdateTcp.m_objSendMng.PushData( bytLvl, dwPopLen, szBuf, dwPacketNum, 0, NULL, 0 );
#endif
				}
				break;
				
			case DEV_PHONE:
			case DEV_DIAODU:
				{
					switch(szBuf[0])
					{
					case 0x35:		//  网络状态通知 
						{
							switch(szBuf[1])
							{
							case 0x01:		// 网络恢复
								{
									PRTMSG(MSG_NOR, "Recv ComuExe inform! Connect Socket!\n");
									
#if USE_LIAONING_SANQI == 0									
									szBuf[0] = 0x01;
									g_objQianTcp.m_objSockStaMng.PushData(LV1, 1, szBuf, dwPacketNum);
#endif

#if USE_LIAONING_SANQI == 1
 									char szbuf[2];
 									szbuf[0] = 0x01;	// 0x01 表示非中心协议帧
 									szbuf[1] = 0x01;	// 0x01 表示TCP套接字连接成功
 									DataPush((void*)szbuf, 2, DEV_SOCK, DEV_QIAN, LV3);	
									g_objQianUdp.ReqCreateSock();
#endif

#if USE_VIDEO_TCP == 1
									g_objDvrTcp.m_objSockStaMng.PushData(LV1, 1, szBuf, dwPacketNum);
#endif
									g_objUpdateUdp.ReqCreateSock();
									
#if WIRELESS_UPDATE_TYPE == 1
									g_objUpdateTcp.m_objSockStaMng.PushData(LV1, 1, szBuf, dwPacketNum);
#endif						
								}
								break;
								
							case 0x02:
								break;
								
							default:
								break;
							}
						}
						break;
						
					default:
						break;
					}
				}
				break;
				
			default:
				break;
			}
		}
	}
	
	if( ERR_MSGSKIPBLOCK == iResult ) 
	{
	}
	
	g_bProgExit = true;
	_ReleaseAll();
	
	// 等待子线程退出
	sleep(15);

	PRTMSG(MSG_NOR, "SockServExe is now to quit\n");
	
	return 0;
}
