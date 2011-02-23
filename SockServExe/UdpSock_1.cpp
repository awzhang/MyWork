#include "StdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD	("SockServ－UdpSock  ")

void *G_ThreadUdpSend(void *arg)
{
	DWORD v_dwSymb = *((DWORD*)arg);
	
	switch(v_dwSymb)
	{
		case DEV_DVR:
			g_objDvrUdp.P_ThreadSend();
			break;
	
		case DEV_UPDATE:
			g_objUpdateUdp.P_ThreadSend();
			break;
	
		case DEV_QIAN:
			g_objQianUdp.P_ThreadSend();
			break;
	
		default:
			break;
	}
	
	pthread_exit(0);
}

void *G_ThreadUdpRecv(void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	DWORD v_dwSymb = *((DWORD*)arg);
	
	switch(v_dwSymb)
	{
		case DEV_DVR:
			g_objDvrUdp.P_ThreadRecv();
			break;	
			
		case DEV_UPDATE:
			g_objUpdateUdp.P_ThreadRecv();
			break;	
			
		case DEV_QIAN:
			g_objQianUdp.P_ThreadRecv();
			break;	
	
		default:
			break;
	}
	
	pthread_exit(0);
}

//////////////////////////////////////////////////////////////////////////

CUdpSocket::CUdpSocket(DWORD v_dwSymb)
{
	m_sockUdp = -1;
	m_dwSymb = v_dwSymb;
	m_pthreadSend = m_pthreadRecv = 0;
//	m_bInit = false;
	m_bToRelease = false;
	m_bToCreate = false;

	switch(m_dwSymb)
	{
		case DEV_DVR:
			m_objSendMng.InitCfg(1024*1024, 30000);
			break;
	
		case DEV_UPDATE:
			m_objSendMng.InitCfg(10*1024, 10000);
			break;
	
		case DEV_QIAN:
			m_objSendMng.InitCfg(100*1024, 10000);
			break;
	
		default:
			break;
	}
	
// 	pthread_mutex_init( &m_mtxSock );
}

CUdpSocket::~CUdpSocket()
{
// 	pthread_mutex_destroy( &m_mtxSock );
}

int CUdpSocket::Init()
{
	if( pthread_create( &m_pthreadSend, NULL, G_ThreadUdpSend, (void*)(&m_dwSymb) )
		|| 
		pthread_create( &m_pthreadRecv, NULL, G_ThreadUdpRecv, (void*)(&m_dwSymb) ) )
	{
		PRTMSG(MSG_ERR, "Create udp send/recv thread failed\n");
		perror("");

		return ERR_THREAD;
	}
	
	return 0;
}

void CUdpSocket::Release()
{
	if( -1 != m_sockUdp )
	{
		shutdown( m_sockUdp, 2 ); // 参数2表示终止读取及传送操作
		close( m_sockUdp );
		m_sockUdp = -1;
	}
}

int CUdpSocket::_CreateSock()
{
	int iRet = 0;
	BOOL blVal;
	int iVal;
	LINGER ling;
	SOCKADDR_IN localaddr;
	unsigned long ulIP = INADDR_ANY;
	
	tag1QIpCfg obj1QIpCfg[2];
	if( GetImpCfg( (void*)&obj1QIpCfg, sizeof(obj1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(obj1QIpCfg) ) ) 
		return ERR_READCFGFAILED;

	tag1LComuCfg obj1LComuCfg;
	if( GetImpCfg( (void*)&obj1LComuCfg, sizeof(obj1LComuCfg), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(obj1LComuCfg) ) )
		return ERR_READCFGFAILED;

	
	_DetroySock();


	switch(m_dwSymb)
	{
		case DEV_UPDATE: // 流媒体升级中心
			m_ulIp = obj1LComuCfg.m_ulLiuIP;
			m_usPort = obj1LComuCfg.m_usLiuPort;
			break;
			
		case DEV_DVR: // king中心 视频上传
			m_ulIp = obj1QIpCfg[0].m_ulVUdpIP;
			m_usPort = obj1QIpCfg[0].m_usVUdpPort;

			localaddr.sin_addr.s_addr = m_ulIp;
			PRTMSG(MSG_NOR, "Video IP: %s, Port: %d\n", inet_ntoa(localaddr.sin_addr), htons(m_usPort));
			
			break;
	
		case DEV_QIAN: // King中心 照相数据上传
			m_ulIp = obj1QIpCfg[0].m_ulQianUdpIP;
			m_usPort = obj1QIpCfg[0].m_usQianUdpPort;
			break;
			
		default:
			break;
	}

	localaddr.sin_family = AF_INET;
	switch(m_dwSymb)
	{
	case DEV_UPDATE:
		localaddr.sin_port = htons(LIU_UDP_LOCAL_PORT);
		break;
		
	case DEV_DVR:
		localaddr.sin_port = htons(DVR_UDP_LOCAL_PROT);
		break;
		
	case DEV_QIAN:
		localaddr.sin_port = htons(PHO_UDP_LOCAL_PROT);
		break;
		
	default:
		return ERR_PAR;
	}	
	localaddr.sin_addr.s_addr = ulIP;

	
// 	pthread_mutex_lock( &m_mtxSock );

	m_sockUdp = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( -1 == m_sockUdp )
	{
		iRet = ERR_SOCK;
		goto _CREATE_SOCK_END;
	}
	
	ling.l_onoff = 1; // 强制关闭且立即释放资源
	ling.l_linger = 0;
	setsockopt( m_sockUdp, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling) );
	blVal = FALSE; // 禁止带外数据
	setsockopt( m_sockUdp, SOL_SOCKET, SO_OOBINLINE, (char*)&blVal, sizeof(blVal) );
	iVal = UDP_RECVSIZE;
	setsockopt( m_sockUdp, SOL_SOCKET, SO_RCVBUF, (char*)&iVal, sizeof(iVal) );
	iVal = UDP_SENDSIZE;
	setsockopt( m_sockUdp, SOL_SOCKET, SO_SNDBUF, (char*)&iVal, sizeof(iVal) );

	// 设置为非阻塞
	int iFlag = fcntl(m_sockUdp, F_GETFL, 0);
	fcntl(m_sockUdp, F_SETFL, iFlag | O_NONBLOCK);

	// DVR的发送缓冲设大一些 （其实设的是否不要太大，可能会快一点，待）
	if( m_dwSymb == DEV_DVR )
	{
		iVal = 1024*32;
		if( setsockopt(m_sockUdp, SOL_SOCKET, SO_SNDBUF, (char*)&iVal, sizeof(iVal)) )
		{
			PRTMSG(MSG_ERR, "udp setsockopt failed!\n");
			perror("");
		}
	}

	if( bind( m_sockUdp, (SOCKADDR*)&localaddr, sizeof(localaddr) ) )
	{
		iRet = ERR_SOCK;
		goto _CREATE_SOCK_END;
	}

	m_bToRelease = false;
	m_bToCreate = false;
//	m_bInit = true;

_CREATE_SOCK_END:
// 	thread_mutex_unlock( &m_mtxSock );

	if( 0 == iRet && m_dwSymb == DEV_UPDATE )
	{
		char szbuf[2];
		szbuf[0] = 0x01;	// 0x01 表示非中心协议帧
		szbuf[1] = 0x02;	// 0x02 表示UDP套接字可用
		DataPush(szbuf, 2, DEV_SOCK, DEV_UPDATE, LV3);
	}

	if( iRet )
	{
		usleep( 1000 * 1000 );
	}

	return iRet;
}

int CUdpSocket::_DetroySock()
{
//	thread_mutex_lock( &m_mtxSock );

//	m_bInit = false;
	if( -1 != m_sockUdp )
	{
		shutdown( m_sockUdp, 2 );
		close( m_sockUdp );
		m_sockUdp = -1;

		//PRTMSG( MSG_NOR, "Destroy UDP Socket! (symb = %d)\n", m_dwSymb );
	}

	m_bToRelease = false;

//	thread_mutex_unlock( &m_mtxSock );

	usleep(500000);
}

void CUdpSocket::ReqDestroySock()
{
	m_bToRelease = true;
}

void CUdpSocket::ReqCreateSock()
{
	m_bToCreate = true;
}

// 发送UDP队列中的数据
void CUdpSocket::P_ThreadSend()
{
	char szSendBuf[ UDP_SENDSIZE ] = {0};
	DWORD dwSendLen = 0;
	int iResult;
	BYTE bytLvl;
	BYTE bytSymb = 0;	
	DWORD dwPushTm = 0;
	struct sockaddr_in adrTo;
	int iPack = 0;
	int iBytes = 0;
	int i = 0;

	fd_set wset, eset;
	timeval tval;

// 	byte bytTest = 0;
// 	DWORD dwLastTestTm = GetTickCount();

// 	FILE *fp = NULL, *fp2 = NULL;
// 	char szTempbuf[100] = {0};
// 	fp = fopen("111.264", "wb+");
// 	fp2 = fopen("222.264", "wb+");

	DWORD dw1, dw2, dw3, dw4;
	
	DWORD dwSleep = 200000;
	bool bNeedRestSndControl = false;
	while(!g_bProgExit )
	{
		static DWORD sta_dwLstSndTm = ::GetTickCount();
		static DWORD sta_dwSndTotal = 0;

		if( DEV_DVR == m_dwSymb )
		{
#if NETWORK_TYPE == NETWORK_EVDO
			dwSleep = 45000;
#elif NETWORK_TYPE == NETWORK_TD
			dwSleep = 140000;
#else
			dwSleep = 200000;
#endif
		}
		else
		{
			dwSleep = 200000;
		}

//		thread_mutex_lock( &m_mtxSock );

		if( m_sockUdp < 0 )
		{
			dwSleep = 500000;
			goto _ONE_SENDLOOP_END;
		}

		if( m_ulIp == 0xffffffff || m_usPort == 0xffff)
		{
			PRTMSG(MSG_NOR, "Ip or Port incorrect, can not send!\n");
			continue;
		}

		//dw1 = GetTickCount();

		FD_ZERO(&wset);
		FD_ZERO(&eset);
		FD_SET(m_sockUdp, &wset);
		FD_SET(m_sockUdp, &eset);

		tval.tv_sec = 0;
		tval.tv_usec = 0;
		
		iResult = select( m_sockUdp + 1, NULL, &wset, &eset, &tval );
		if( iResult > 0 )
		{
			if( FD_ISSET(m_sockUdp, &eset) )
			{
				PRTMSG(MSG_NOR, "Udp Socket select write except!\n");
				goto _WRITE_SOCK_EXCEPT;
			}
		}
		else if( 0 == iResult )
		{
			PRTMSG(MSG_NOR, "Udp Socket select write block!\n");
			goto _ONE_SENDLOOP_END;
		}
		else
		{
			PRTMSG(MSG_NOR, "Udp Socket select write except -2!\n");
			goto _WRITE_SOCK_EXCEPT;
		}

// 		// 测试用，发送34协议帧
// 		if(m_dwSymb == DEV_DVR)
// 		{
// 			szSendBuf[0] = 0x34;
// 			szSendBuf[1] = bytTest;
// 			if(  GetTickCount() - dwLastTestTm > 5000 )
// 			{
// 				adrTo.sin_family = AF_INET;
// 				adrTo.sin_port = m_usPort;
// 				adrTo.sin_addr.s_addr = m_ulIp;
// 				iResult = sendto( m_sockUdp, szSendBuf, 2, 0, (struct sockaddr*)&adrTo, sizeof(adrTo) );
// 				
// 				dwLastTestTm = GetTickCount();
// 				bytTest++;
// 				PRTMSG(MSG_DBG, "send 0x34 Test: %2x, %2x, tickTount = %d\n", szSendBuf[0], szSendBuf[1], GetTickCount());
// 			}
// 		}

		//dw2 = GetTickCount();

		if( bNeedRestSndControl )
		{
			sta_dwLstSndTm = GetTickCount();
			sta_dwSndTotal = 0;
			bNeedRestSndControl = false;
		}

		for(i=0; i<2; i++)
		{
			iResult = m_objSendMng.PopData( bytLvl, sizeof(szSendBuf), dwSendLen, szSendBuf, dwPushTm, &bytSymb);
			if( iResult || !dwSendLen || dwSendLen > sizeof(szSendBuf) )
			{
				if( iResult != ERR_QUEUE_EMPTY) PRTMSG( MSG_DBG, "UDP Sock send pop data Err! %d, %d\n", iResult, dwSendLen );
				goto _ONE_SENDLOOP_END;
			}

			//dw3 = GetTickCount();

			adrTo.sin_family = AF_INET;
			adrTo.sin_port = m_usPort;
			adrTo.sin_addr.s_addr = m_ulIp;
			iResult = sendto( m_sockUdp, szSendBuf, int(dwSendLen), 0, (struct sockaddr*)&adrTo, sizeof(adrTo) );

			//dw4 = GetTickCount();

// 			if( 0 == i )
// 			{
// 				PRTMSG( MSG_DBG, "!!!000: %d, %d, %d\n", dw2-dw1, dw3-dw2, dw4-dw3 );
// 			}
				
			if( iResult == (int)dwSendLen )
			{
				if( m_dwSymb == DEV_DVR )
				{
					iPack++;
					iBytes += iResult;
					if(iPack%20 == 0)
					{
						PRTMSG(MSG_DBG, "DVR UDP Send %d packs, %d bytes, Tickcount:%d\n", iPack, iBytes, GetTickCount());
					}

					// 保存所发送的数据到文件中
// 					fwrite((void*)&iResult, 1, 4, fp);
// 					fwrite(szSendBuf, 1, iResult, fp);
// 					fflush(fp); 
// 					fwrite(szSendBuf+55, iResult-57, 1, fp2);
// 					fflush(fp2);

					//memset(szTempbuf, 0, sizeof(szTempbuf));
// 					sprintf(szTempbuf, "win:%2x %2x %2x %2x, pakcet:%d\r\n",
// 						szSendBuf[20], szSendBuf[21], szSendBuf[22], szSendBuf[23], szSendBuf[28]);
// 					fwrite(szTempbuf, 1, strlen(szTempbuf), fp);
//  					fflush(fp); 
				}			
// 	 			else if( m_dwSymb == DEV_UPDATE )
// 	 			{
// 	 				PRTMSG(MSG_DBG, "Liu UDP send:");
// 	 				PrintString(szSendBuf, iResult);
// 	 			}
				else if( m_dwSymb == DEV_QIAN )
				{
					iPack++;
					iBytes += iResult;
					PRTMSG(MSG_DBG, "Qian UDP Send %d packs, %d bytes\n", iPack, iBytes);
				}
			}
			else if( iResult > 0 && iResult < (int)dwSendLen )
			{
				PRTMSG(MSG_DBG, "send succ, but send len incorrect, iResult = %d, dwSendLen = %d\n", iResult, dwSendLen);
			}
			else if( iResult < 0 )
			{
				PRTMSG(MSG_ERR, "Udp Send Fail\n");
				perror("");
				
				goto _WRITE_SOCK_EXCEPT;
			}

			sta_dwSndTotal += iResult;
		}
		
		goto _ONE_SENDLOOP_END;

_WRITE_SOCK_EXCEPT:

		PRTMSG( MSG_DBG, "%d UDP Send Thread Except!\n", m_dwSymb );
		
		m_bToRelease = true;

		dwSleep = 500000;

		sta_dwLstSndTm = GetTickCount();
		sta_dwSndTotal = 0;

		// 发送出错，通知TCP套接字
		{
			char szdata = 0x01;
			DWORD dwPacketNum = 0;
			g_objQianTcp.m_objSockStaMng.PushData(LV1, 1, &szdata, dwPacketNum);
		}
		goto _ONE_SENDLOOP_END2;

_ONE_SENDLOOP_END:
//		thread_mutex_unlock( &m_mtxSock );
		{
#if NETWORK_TYPE == NETWORK_TD
			const DWORD SENDLMT_PERSEC = 50000;
#endif
#if NETWORK_TYPE == NETWORK_EVDO
			const DWORD SENDLMT_PERSEC = 280000;
#endif
#if NETWORK_TYPE == NETWORK_GSM
			const DWORD SENDLMT_PERSEC = 30000;
#endif
			DWORD dwCur = GetTickCount();
			DWORD dwDelt = dwCur - sta_dwLstSndTm;
// 			if( sta_dwSndTotal > 0 )
// 			{
// 				PRTMSG( MSG_DBG, "!!!!%d, %d\n", sta_dwSndTotal, dwDelt );
// 			}
			if( sta_dwSndTotal >= SENDLMT_PERSEC || dwDelt >= 2000 )
			{
				//PRTMSG( MSG_DBG, "1: \n");

				//DWORD dwNeedSleep = DWORD( sta_dwSndTotal * (dwDelt / 1000.0) );
				DWORD dwNeedSleep = DWORD( double(sta_dwSndTotal) / SENDLMT_PERSEC * 1000.0 ); // ms

				if( dwNeedSleep >= dwDelt )
				{
					dwSleep = ( dwNeedSleep - dwDelt ) * 1000;
					if( dwSleep < 20000 ) dwSleep = 20000;

					PRTMSG( MSG_DBG, "2: %d,%d,%d,%d\n", dwSleep, dwNeedSleep, dwDelt, sta_dwSndTotal );
				}
				else
				{
					dwSleep = 5000;

					PRTMSG( MSG_DBG, "3: %d,%d,%d\n", dwNeedSleep, dwDelt, sta_dwSndTotal );
				}

				bNeedRestSndControl = true;
			}
			else if( sta_dwSndTotal > 0 )
			{
				dwSleep = 5000;
				bNeedRestSndControl = false;

				//PRTMSG( MSG_DBG, "4: %d\n", dwDelt );
				//PRTMSG( MSG_DBG, "4:\n" );
			}
		}

_ONE_SENDLOOP_END2:
		//if( bNeedRestSndControl ) PRTMSG( MSG_DBG, "5:\n" );
		if( dwSleep )
		{
			usleep( dwSleep );
		}
	}
}

void CUdpSocket::P_ThreadRecv()
{
	int	  iResult;
	char  buf[ UDP_RECVSIZE ];
	struct sockaddr_in addrFrom;
	int   iLen = sizeof(addrFrom);

	buf[0] = 0x02;		// 0x02 表示中心UDP协议帧

	fd_set rset, eset;
	timeval tvl;
	
	DWORD dwSleep = 50000;
	while(!g_bProgExit)
	{
		usleep( dwSleep );

		dwSleep = 50000;

		if( m_bToCreate )
		{
			//PRTMSG( MSG_DBG, "%d UDP Rcv Thread Here -1!\n", m_dwSymb );
			if( _CreateSock() )
			{
				continue;
			}
		}
		else if( m_bToRelease )
		{
			//PRTMSG( MSG_DBG, "%d UDP Rcv Thread Here -2!\n", m_dwSymb );
			_DetroySock();
			continue;
		}

		if( m_sockUdp < 0 )
		{
			dwSleep = 500000;
			continue;
		}

		FD_ZERO(&rset);
		FD_ZERO(&eset);
		FD_SET(m_sockUdp, &rset);
		FD_SET(m_sockUdp, &eset);

		tvl.tv_sec = 1;
		tvl.tv_usec = 0;

		iResult = select( m_sockUdp + 1, &rset, NULL, &eset, &tvl);
		if( iResult > 0 )
		{
			if( FD_ISSET(m_sockUdp, &eset) )
			{
				PRTMSG(MSG_NOR, "Udp Socket select read except!\n");
				goto _RCV_SOCK_EXCEPT;
			}
		}
		else if( iResult < 0 )
		{
			PRTMSG(MSG_NOR, "Udp Socket select read except -2!\n");
			goto _RCV_SOCK_EXCEPT;
		}
		else
		{
			continue;
		}

		iResult = recvfrom( m_sockUdp, buf+1, sizeof(buf)-1, 0, (struct sockaddr*)&addrFrom, (socklen_t*)&iLen );
		if( iResult > 0 )
		{
// 			if( m_dwSymb == DEV_UPDATE )
// 			{
// 				PRTMSG(MSG_NOR, "Liu recv: ");
// 				PrintString(buf+6, 6);
// 			}

// 			if( m_dwSymb == DEV_DVR )
// 			{
// 				PRTMSG(MSG_NOR, "Dvr Udp recv: ");
// 				PrintString(buf, iResult+1);
// 
// 				// 测试用
// 				if( buf[1] == 0x34 )
// 				{
// 					PRTMSG(MSG_NOR, "Recv 0x34 Test: %2x, %2x, TickCount = %d\n", buf[1], buf[2], GetTickCount());
// 					continue;
// 				}
// 			}
			
			DataPush(buf, iResult+1, DEV_SOCK, m_dwSymb, LV2);
		}
		else if( iResult < 0 )
		{
			PRTMSG(MSG_ERR, "Udp Read Err\n" );
			perror("");
			
			// 接收出错，通知TCP套接字
			char szdata = 0x01;
			DWORD dwPacketNum = 0;
			g_objQianTcp.m_objSockStaMng.PushData(LV1, 1, &szdata, dwPacketNum);

			goto _RCV_SOCK_EXCEPT;
		}

		continue;

_RCV_SOCK_EXCEPT:
		PRTMSG( MSG_DBG, "%d UDP Rcv Thread Except!\n", m_dwSymb );
		m_bToRelease = true;
		dwSleep = 500000;
	}
}

bool CUdpSocket::SetIPandPort(unsigned long v_ulIp, unsigned short v_usPort)
{
	m_ulIp = v_ulIp;
	m_usPort = v_usPort;
}


