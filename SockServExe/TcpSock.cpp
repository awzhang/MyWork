#include "StdAfx.h"

void *G_ThreadTcpWork(void *arg)
{
	DWORD v_dwSymb = *((DWORD*)arg);

	switch(v_dwSymb)
	{	
	case DEV_QIAN:
		g_objQianTcp.P_ThreadWork();
		break;

#if USE_VIDEO_TCP == 1
	case DEV_DVR:
		g_objDvrTcp.P_ThreadWork();
		break;
#endif

#if WIRELESS_UPDATE_TYPE == 1
	case DEV_UPDATE:
		g_objUpdateTcp.P_ThreadWork();
		break;
#endif
		
	default:
		break;
	}	
}

void *G_ThreadTcpSend(void *arg)
{
	DWORD v_dwSymb = *((DWORD*)arg);
	
	switch(v_dwSymb)
	{	
	case DEV_QIAN:
		g_objQianTcp.P_ThreadSend();
		break;

#if USE_VIDEO_TCP == 1
	case DEV_DVR:
		g_objDvrTcp.P_ThreadSend();
		break;
#endif
		
#if WIRELESS_UPDATE_TYPE == 1
	case DEV_UPDATE:
		g_objUpdateTcp.P_ThreadSend();
		break;
#endif
		
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

#undef MSG_HEAD
#define MSG_HEAD	("SockServ-TcpSock")

CTcpSock::CTcpSock(DWORD v_dwSymb)
{
	m_sockTcp = -1;
	m_dwSymb = v_dwSymb;
	m_pthreadWork = 0;
	m_bytSockSta = Idle;
	m_bInit = false;
	m_bConnect = false;
	
	m_bFirstConnect = true;
	m_iIPCnt = 0;
	for(int i=0; i<MAX_TEMP_IP_CNT; i++)
	{
		m_bIPEnabled[i] = false;
		m_ulTempIP[MAX_TEMP_IP_CNT] = 0;
		m_usTempTcpPort[MAX_TEMP_IP_CNT] = 0;
		m_usTempUdpPort[MAX_TEMP_IP_CNT] = 0;
	}
	m_objSendMng.InitCfg(100*1024, 10000);
}

CTcpSock::~CTcpSock()
{

}

int CTcpSock::Init()
{
	if( pthread_create(&m_pthreadWork, NULL, G_ThreadTcpWork, (void*)(&m_dwSymb)) != 0
		// || pthread_create(&m_pthreadSend, NULL, G_ThreadTcpSend, (void*)(&m_dwSymb)) != 0
	  )
	{
		PRTMSG(MSG_ERR, "Create Tcpsock Thread failed!\n");
		perror("");

		return ERR_THREAD;
	}

	return 0;
}

int CTcpSock::Release()
{
	return 0;
}

/// 小数据量的收发，集中在一个线程里，不影响效率
void CTcpSock::P_ThreadWork()
{
	byte  bytLvl = 0;
	char  szBuf[100] = {0};
	DWORD dwLen = 0;
	DWORD dwPushTm;
	DWORD dwBufSize = (DWORD)sizeof(szBuf);
	bool  bMustConn = false;
	int   iFlag;
	int   iret;
	int   iConnFailTimes = 0;	// 连续连接失败次数
	static DWORD sta_dwLastConTm = GetTickCount();

	fd_set rset, wset, eset;
	struct timeval tval;

	int  iRecvLen;
	char szRecvbuf[ TCP_RECVSIZE+1 ];

	char  szSendBuf[ TCP_SENDSIZE ] = {0};
	DWORD dwSendLen;
	//DWORD dwPushTm = 0;
	//BYTE  bytLvl;

	szRecvbuf[0] = 0x00;		// 0x00 表示中心TCP协议帧

	DWORD dwSleep = 500; // ms
	while(!g_bProgExit)
	{
		usleep(dwSleep * 1000);

		dwSleep = 500;
		if( 0 == m_objSockStaMng.PopData(bytLvl, dwBufSize, dwLen, szBuf, dwPushTm) )
		{
			switch(szBuf[0])
			{
				case 0x01:
					{
						iConnFailTimes = 0;
						m_bytSockSta = DisConn;
						bMustConn = true;
					}
					break;
	
				default:
					break;
			}
		}

		switch( m_bytSockSta )
		{
			case Idle:
				{
					// 若处于空闲状态，则每隔60秒连接一次中心
					if( GetTickCount() - sta_dwLastConTm >= 60000 )
					{
						m_bytSockSta = Conn;
						break;
					}
	
					if( bMustConn == true )
					{
						m_bytSockSta = Conn;
						bMustConn = false;
						break;
					}
				}
				break;
	
			case Conn:
				{
					sta_dwLastConTm = GetTickCount();

					// 若没有可用的临时IP，说明要连接到登陆服务器，此时则使用配置区中的IP
					if( m_dwSymb == DEV_DVR )
					{
						for(int i=0; i<m_iIPCnt; i++)
						{
							if( m_bIPEnabled[i] )
							{
								m_bIPEnabled[i] = false;	// 使用一次后即置为假
								m_conn.sin_family = AF_INET;
								m_conn.sin_port = m_usTempTcpPort[i];
								m_conn.sin_addr.s_addr = m_ulTempIP[i];
								
								// 且DVR UDP的地址和端口也要相应更改
								g_objDvrUdp.m_ulIp = m_ulTempIP[i];
								g_objDvrUdp.m_usPort = m_usTempUdpPort[i];
								tag1QIpCfg	obj1QIpCfg;
								GetImpCfg( &obj1QIpCfg, sizeof(obj1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
								obj1QIpCfg.m_ulVUdpIP = g_objDvrUdp.m_ulIp;
								obj1QIpCfg.m_usVUdpPort = g_objDvrUdp.m_usPort;
								SetImpCfg( &obj1QIpCfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );

								goto TCP_CONNECT;
							}
						}
					}
					
					// 读取IP、端口配置
					tag1QIpCfg obj1QIpCfg[2];
					tag1PComuCfg obj1PComuCfg;
					tag1LComuCfg obj1LComuCfg;
					if(	GetImpCfg( (void*)&obj1QIpCfg, sizeof(obj1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(obj1QIpCfg) )
						|| GetImpCfg( (void*)&obj1PComuCfg, sizeof(obj1PComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) )
						|| GetImpCfg( (void*)&obj1LComuCfg, sizeof(obj1LComuCfg), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(obj1LComuCfg) )
						)
					{
						PRTMSG(MSG_ERR, "Read IP Cfg Fail\n" );
						m_bytSockSta = Idle;
						break;
					}
					
					m_conn.sin_family = AF_INET;
					switch( m_dwSymb )
					{
					case DEV_QIAN: // king中心
						{
							m_conn.sin_port = obj1QIpCfg[0].m_usQianTcpPort;
	 						m_conn.sin_addr.s_addr = obj1QIpCfg[0].m_ulQianTcpIP;
						}
						break;

					case DEV_DVR:	// 独立视频服务器
						{
							m_conn.sin_port = obj1QIpCfg[1].m_usQianTcpPort;
	 						m_conn.sin_addr.s_addr = obj1QIpCfg[1].m_ulQianTcpIP;
						}
						break;

					case DEV_UPDATE: // 管理平台 （应该增加一个新的标记，待）
						{
							m_conn.sin_port = obj1LComuCfg.m_usLiuPort2;
	 						m_conn.sin_addr.s_addr = obj1LComuCfg.m_ulLiuIP2;
						}
						break;

					default:
						break;
					}

TCP_CONNECT:					
					// 若配置为空，则返回
					if(	0 == m_conn.sin_addr.s_addr	|| 0 == m_conn.sin_port || 0 == strcmp(obj1PComuCfg.m_szTel, ""))
					{
						PRTMSG(MSG_ERR, "IP/Port/Tel Cfg Is Uncompleted!\n");
						G_RecordDebugInfo("IP/Port/Tel Cfg Is Uncompleted!\n");
						m_bytSockSta = Idle;
						break;
					}
	
					// 确认关闭已有连接
					if( -1 != m_sockTcp )
					{
						shutdown(m_sockTcp, 2);
						close(m_sockTcp);
						m_sockTcp = -1;
					}
	
					m_sockTcp = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
					if( -1 == m_sockTcp )
					{
						PRTMSG(MSG_ERR, "Socket err\n" );
						break;
					}
	
					bzero(&(m_conn.sin_zero),8);
	
					// 获取当前socket的属性， 并设置noblocking属性
					iFlag = fcntl(m_sockTcp, F_GETFL, 0);
					fcntl(m_sockTcp, F_SETFL, iFlag | O_NONBLOCK);

					LINGER ling;
					ling.l_onoff = 1; // closesocket时强制关闭并立即释放套接字资源
					ling.l_linger = 0;
					::setsockopt( m_sockTcp, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling) );

					BOOL blVal = TRUE; // 禁止Nagle算法
					setsockopt( m_sockTcp, IPPROTO_TCP, TCP_NODELAY, (char*)&blVal, sizeof(blVal) );
	
					if( DEV_QIAN == m_dwSymb )
					{
						PRTMSG(MSG_DBG, "Qian Tcp connect: Tel: %s, IP: %s, Port: %d\n", obj1PComuCfg.m_szTel, inet_ntoa(m_conn.sin_addr), htons(m_conn.sin_port));
						RenewMemInfo(0x02, 0x02, 0x01, 0x03);

						G_RecordDebugInfo("Qian Tcp connect: Tel: %s, IP: %s, Port: %d", obj1PComuCfg.m_szTel, inet_ntoa(m_conn.sin_addr), htons(m_conn.sin_port));
					}
					if(DEV_DVR == m_dwSymb)
					{
						PRTMSG(MSG_DBG, "Dvr Tcp connect: Tel: %s, IP: %s, Port: %d\n", obj1PComuCfg.m_szTel, inet_ntoa(m_conn.sin_addr), htons(m_conn.sin_port));
					
						G_RecordDebugInfo("Dvr Tcp connect: Tel: %s, IP: %s, Port: %d", obj1PComuCfg.m_szTel, inet_ntoa(m_conn.sin_addr), htons(m_conn.sin_port));
					}
					
					if( (iret = connect( m_sockTcp, (struct sockaddr *)&m_conn, sizeof(struct sockaddr))) < 0 )
					{
						if(errno != EINPROGRESS)
						{
							PRTMSG(MSG_DBG, "connect failed, errno=%d\n", errno);	
							G_RecordDebugInfo("connect failed, errno=%d", errno);	
							perror("");
	
							iConnFailTimes++;
							if( !_JudgeConFailMaxTime(iConnFailTimes) )
							{
								sta_dwLastConTm = GetTickCount() - 57000; // 这样，3秒后可以尝试重新connect，提高效率
							}

							m_bytSockSta = Idle;
							break;
						}
					}
					m_bytSockSta = WaitConn;
				}
				break;
	
			case WaitConn:
				{
					FD_ZERO(&rset);
					FD_ZERO(&wset);
					FD_ZERO(&eset);
					FD_SET(m_sockTcp, &rset);
					FD_SET(m_sockTcp, &wset);
					FD_SET(m_sockTcp, &eset);
	
					tval.tv_sec = 30;
					tval.tv_usec = 0;
	
					iret = select(m_sockTcp+1, &rset, &wset, &eset, &tval);
					if( 0 == iret )
					{
						if( m_dwSymb == DEV_QIAN )
						{
							PRTMSG(MSG_NOR, "Qian Tcp Socket connect time out!\n");
							G_RecordDebugInfo("Tcp Socket connect time out!");
						}

						if( m_dwSymb == DEV_DVR )
						{
							PRTMSG(MSG_NOR, "Dvr Tcp Socket connect time out!\n");
							G_RecordDebugInfo("Dvr Tcp Socket connect time out!");
						}

						goto _CUR_CONNECT_FAIL;
					}
					else if(iret < 0)
					{
						PRTMSG(MSG_NOR, "Tcp Socket select err!\n");
						goto _CUR_CONNECT_FAIL;
					}
					else
					{
						if( FD_ISSET(m_sockTcp, &eset) )
						{
							PRTMSG(MSG_NOR, "Tcp Socket connect except!\n");
							goto _CUR_CONNECT_FAIL;
						}
						else if(FD_ISSET(m_sockTcp, &rset) || FD_ISSET(m_sockTcp, &wset)) 
						{
							m_bConnect = true;
							iConnFailTimes = 0;							

							if( m_dwSymb == DEV_QIAN )
							{
								PRTMSG(MSG_NOR, "Qian Tcp Socket Connect succ!\n");
								G_RecordDebugInfo("Qian Tcp Socket Connect succ!");
							}
							if( m_dwSymb == DEV_DVR )
							{
								PRTMSG(MSG_NOR, "Dvr Socket Connect succ!\n");
								G_RecordDebugInfo("Dvr Tcp Socket Connect succ!");
							}

							if(m_dwSymb == DEV_DVR)
							{
								for(int i=0; i<MAX_TEMP_IP_CNT; i++)
									m_bIPEnabled[i] = false;
							}

							// 通知连接成功
							char szbuf[2];
							szbuf[0] = 0x01;	// 0x01 表示非中心协议帧
							szbuf[1] = 0x01;	// 0x01 表示TCP套接字连接成功
							DataPush((void*)szbuf, 2, DEV_SOCK, m_dwSymb, LV3);
							
							RenewMemInfo(0x02, 0x01, 0x01, 0x01);
							
							m_bytSockSta = Working;
							break;	
						}
						else 
						{
							PRTMSG(MSG_ERR, "Tcp Socket select err: sockfd not set\n" );
							goto _CUR_CONNECT_FAIL;
						}
					}
					break;

_CUR_CONNECT_FAIL:
					close(m_sockTcp);
					m_sockTcp = -1;
					
					iConnFailTimes++;
					if( !_JudgeConFailMaxTime(iConnFailTimes) )
					{
						sta_dwLastConTm = GetTickCount() - 57000; // 这样，3秒后可以尝试重新connect，提高效率
					}
					
					m_bytSockSta = DisConn;
				}
				break;
	
			case Working:
				{
					/// 先写

					FD_ZERO(&wset);
					FD_ZERO(&eset);
					FD_SET(m_sockTcp, &wset);
					FD_SET(m_sockTcp, &eset);
	
					tval.tv_sec = 0;
					tval.tv_usec = 0;

					iret = select(m_sockTcp+1, NULL, &wset, &eset, &tval);
					if( iret > 0 )
					{
						if( FD_ISSET(m_sockTcp, &eset) )
						{
							PRTMSG(MSG_NOR, "Tcp Socket select write except!\n");
							G_RecordDebugInfo("Tcp Socket select write except!");
							goto _CUR_SOCK_EXCEPT;
						}
						else if( FD_ISSET(m_sockTcp, &wset) )
						{
							if( 0 == m_objSendMng.PopData( bytLvl, sizeof(szSendBuf), dwSendLen, szSendBuf, dwPushTm) )
							{
								if( dwSendLen > 0 && dwSendLen <= sizeof(szSendBuf) && -1 != m_sockTcp )
								{
									iret = send( m_sockTcp, szSendBuf, dwSendLen, 0 );
									
									if( iret > 0 )
									{
										if( DEV_QIAN == m_dwSymb )
										{
											PRTMSG(MSG_DBG, "Qian TcpSock send:");
										}
										if( DEV_DVR == m_dwSymb )
										{
											PRTMSG(MSG_DBG, "Dvr TcpSock send:");
										}
										PrintString(szSendBuf,iret);
									}
									else if( iret < 0 )
									{
										goto _CUR_SOCK_EXCEPT;
									}
								}

								dwSleep = 50;
							}
						}
						else
						{
							goto _CUR_SOCK_EXCEPT;
						}
					}
					else if( 0 == iret )
					{
						PRTMSG(MSG_NOR, "Tcp Socket select write block!\n");
						G_RecordDebugInfo("Tcp Socket select write block!");
					}
					else if(iret < 0)
					{
						PRTMSG(MSG_NOR, "Tcp Socket select write err!\n");
						G_RecordDebugInfo("Tcp Socket select write err!");
						goto _CUR_SOCK_EXCEPT;
					}


					/// 后读

					FD_ZERO(&rset);
					FD_ZERO(&eset);
					FD_SET(m_sockTcp, &rset);
					FD_SET(m_sockTcp, &eset);
					
					tval.tv_sec = 0;
					tval.tv_usec = 0;

					iret = select(m_sockTcp+1, &rset, NULL, &eset, &tval);
					if( iret > 0 )
					{
						if( FD_ISSET(m_sockTcp, &eset) )
						{
							PRTMSG(MSG_NOR, "Tcp Socket select read except!\n");
							G_RecordDebugInfo("Tcp Socket select read except!");
							goto _CUR_SOCK_EXCEPT;
						}
						else if( FD_ISSET(m_sockTcp, &rset) )
						{
							iRecvLen = recv( m_sockTcp, szRecvbuf+1, sizeof(szRecvbuf)-1, 0 );
						
							if( iRecvLen > 0 )
							{
								if( DEV_QIAN == m_dwSymb )
								{
									PRTMSG(MSG_NOR, "Qian Rcv Tcp Msg: ");
								}
								if( DEV_DVR == m_dwSymb )
								{
									PRTMSG(MSG_NOR, "Dvr Rcv Tcp Msg: ");
								}
								PrintString(szRecvbuf+1, iRecvLen);
								
								DataPush(szRecvbuf, iRecvLen+1, DEV_SOCK, m_dwSymb, LV2);

								dwSleep = 50;
							}
							else
							{
								if(m_dwSymb == DEV_DVR)
								{
									PRTMSG(MSG_ERR, "Dvr Tcp socket recv err!\n");								
									G_RecordDebugInfo("Dvr Tcp socket recv err, errno=%d", errno);
								}
								if(m_dwSymb == DEV_QIAN)
								{
									PRTMSG(MSG_ERR, "Qian Tcp socket recv err!\n");								
									G_RecordDebugInfo("Qian Tcp socket recv err, errno=%d", errno);
								}
								
								perror("");
								goto _CUR_SOCK_EXCEPT;
							}
						}
					}
					else if( iret < 0 )
					{
						PRTMSG(MSG_ERR, "Tcp socket select err!\n");
						G_RecordDebugInfo("Tcp socket select err!");
						perror("");
						goto _CUR_SOCK_EXCEPT;
					}

					break;

_CUR_SOCK_EXCEPT:
					sta_dwLastConTm = GetTickCount() - 57000; // 这样，3秒后可以尝试重新connect，提高效率
					m_bytSockSta = DisConn;
				}
				break;
	
			case DisConn:
				{
					if( m_dwSymb == DEV_QIAN )
						G_RecordDebugInfo("Qian Tcp socket Disconn!");
					if( m_dwSymb == DEV_DVR )
						G_RecordDebugInfo("Dvr Tcp socket Disconn!");

					if(m_sockTcp > 0)
					{
						shutdown(m_sockTcp, 2);
						close(m_sockTcp);
						m_sockTcp = -1;
					}
	
					// Tcp断开连接，则把Udp也断开
					if( DEV_QIAN == m_dwSymb )
					{
						g_objQianUdp.ReqDestroySock();
					}
					if( DEV_DVR == m_dwSymb )
					{
						g_objDvrUdp.ReqDestroySock();
					}

					// 通知断开连接
					char szbuf[2];
					szbuf[0] = 0x01;	// 0x01 表示非中心协议帧
					szbuf[1] = 0x02;	// 0x02 表示通知网络断开连接
					DataPush((void*)szbuf, 2, DEV_SOCK, m_dwSymb, LV3);
					
					RenewMemInfo(0x02, 0x01, 0x01, 0x00);
	
					m_bytSockSta = Idle;
				}
				break;
	
			default:
				{
					m_bytSockSta = Idle;
				}
				break;
		}
	}
}

bool CTcpSock::_JudgeConFailMaxTime(int &v_iFailTimes)
{
	if( DEV_QIAN == m_dwSymb || DEV_DVR == m_dwSymb)
	{
		if( v_iFailTimes >= 3 )	// 连续三次连接失败，则请求ComuExe重新拨号
		{
			v_iFailTimes = 0;
			
			if(m_dwSymb == DEV_DVR)
			{
				PRTMSG(MSG_ERR, "Dvr Tcp connect failed to max timers! Ask ComuExe to queck net!\n");
				G_RecordDebugInfo("Dvr Tcp connect failed to max timers! Ask ComuExe to queck net!");
			}
			if(m_dwSymb == DEV_QIAN)
			{
				PRTMSG(MSG_ERR, "Qian Tcp connect failed to max timers! Ask ComuExe to queck net!\n");
				G_RecordDebugInfo("Qian Tcp connect failed to max timers! Ask ComuExe to queck net!");
			}
			
			// 请求ComuExe重新拨号
			char buf[] = {0x34, 0x01};
			DataPush((void*)&buf, 2, DEV_SOCK, DEV_PHONE, LV3);
			
			buf[0] = 0x01;
			buf[1] = 0x03;
			if(DEV_QIAN == m_dwSymb)
			{
				// 同时请求QianExe向调度屏发送提示
				DataPush((void*)&buf, 2, DEV_SOCK, DEV_QIAN, LV3);
			}
			if(DEV_DVR == m_dwSymb)
			{
				// 同时请求DvrExe向调度屏发送提示
				DataPush((void*)&buf, 2, DEV_SOCK, DEV_DVR, LV3);
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else if( DEV_UPDATE == m_dwSymb )	// 升级服务器的TCP不去判断失败次数，一直重连
	{
		return false; // 暂不填充
	}
	
	return false;
}


// 本线程暂时不启用
void CTcpSock::P_ThreadSend()
{
	char  szSendBuf[ TCP_SENDSIZE ] = {0};
	DWORD dwSendLen;
	DWORD dwPushTm = 0;
	BYTE  bytLvl;
	
	while(!g_bProgExit)
	{
		if( m_objSendMng.PopData( bytLvl, sizeof(szSendBuf), dwSendLen, szSendBuf, dwPushTm) )
		{
			usleep(100000);
			continue;
		}

		if( !dwSendLen || dwSendLen > sizeof(szSendBuf) ) 
			continue;

		PRTMSG(MSG_DBG, "TcpSock send:");
		PrintString(szSendBuf,dwSendLen);

		if( -1 != m_sockTcp )
		{
			send( m_sockTcp, szSendBuf, dwSendLen, 0 );
		}
	}
}

