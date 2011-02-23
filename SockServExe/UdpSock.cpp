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
	m_bToRelease = false;
	m_bToCreate = false;

	switch(m_dwSymb)
	{
		case DEV_DVR:
			m_objSendMng.InitCfg(1024*1024, 300000);
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

	if (NETWORK_TYPE == NETWORK_TD)
	{
		//m_dwSendBytesPerTime = 1500;
		m_dwSleepTm = 80000;
	}
	else if (NETWORK_TYPE == NETWORK_EVDO)
	{
		m_dwSendBytesPerTime = 20000;
		m_dwSleepTm = 30000;
	}
	else if (NETWORK_TYPE == NETWORK_WCDMA)
	{
		m_dwSendBytesPerTime = 20000;
		m_dwSleepTm = 30000;
	}
	else if (NETWORK_TYPE == NETWORK_GSM)
	{
		m_dwSendBytesPerTime = 1000;
		m_dwSleepTm = 300000;
	}
}

CUdpSocket::~CUdpSocket()
{

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
	iVal = 0;
	setsockopt( m_sockUdp, SOL_SOCKET, SO_SNDBUF, (char*)&iVal, sizeof(iVal) );

	if( bind( m_sockUdp, (SOCKADDR*)&localaddr, sizeof(localaddr) ) )
	{
		iRet = ERR_SOCK;
		goto _CREATE_SOCK_END;
	}

	m_bToRelease = false;
	m_bToCreate = false;

_CREATE_SOCK_END:

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
	if( -1 != m_sockUdp )
	{
		shutdown( m_sockUdp, 2 );
		close( m_sockUdp );
		m_sockUdp = -1;
	}

	m_bToRelease = false;

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

	int iSendPacks = 0;
	int iLastPrint = 0;
	int iTotalSendBytes = 0;
	int iOneLoopSendBytes = 0;
	bool bSend = false;
	int i = 0;
	static DWORD sta_dwLstSndTm = GetTickCount();
	static DWORD sta_dwLstSndByte = 0;

	// 测试用，发送34协议帧
// 	byte bytTest = 0;
// 	DWORD dwLastTestTm = GetTickCount();
	
	while(!g_bProgExit )
	{	
		if( m_bToCreate )
		{
			if( _CreateSock() )
			{
				continue;
			}
		}
		else if( m_bToRelease )
		{
			_DetroySock();
			continue;
		}
		
		if( m_sockUdp < 0 )
		{
			usleep(100000);
			continue;
		}

		if( m_ulIp == 0xffffffff || m_usPort == 0xffff)
		{
			PRTMSG(MSG_NOR, "Ip or Port incorrect, can not send!\n");
			usleep(100000);
			continue;
		}

		// 测试用，发送34协议帧
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
		
		int iCnt;
		if (NETWORK_TYPE == NETWORK_EVDO || NETWORK_TYPE == NETWORK_WCDMA)
			iCnt = 100;
		else if (NETWORK_TYPE == NETWORK_TD)
			iCnt = 2;

		for(i=0; i<iCnt; i++)
		{
			iResult = m_objSendMng.PopData( bytLvl, sizeof(szSendBuf), dwSendLen, szSendBuf, dwPushTm, &bytSymb);
			if( iResult || !dwSendLen || dwSendLen > sizeof(szSendBuf) )
			{
				goto _ONE_SENDLOOP_END;
			}

			adrTo.sin_family = AF_INET;
			adrTo.sin_port = m_usPort;
			adrTo.sin_addr.s_addr = m_ulIp;
			iResult = sendto( m_sockUdp, szSendBuf, int(dwSendLen), 0, (struct sockaddr*)&adrTo, sizeof(adrTo) );
				
			if( iResult > 0 )
			{
				if( iResult < (int)dwSendLen )
				{
					PRTMSG(MSG_DBG, "send succ, but send len incorrect, iResult = %d, dwSendLen = %d\n", iResult, dwSendLen);
				}
								
				if( m_dwSymb == DEV_DVR )
				{
					iSendPacks++;
					iTotalSendBytes += iResult;
					iOneLoopSendBytes += iResult;
					bSend = true;

					if (NETWORK_TYPE == NETWORK_EVDO || NETWORK_TYPE == NETWORK_WCDMA)
					{
						if( iOneLoopSendBytes > m_dwSendBytesPerTime )
							break;
					}
				}
				else if(m_dwSymb == DEV_QIAN)
				{
					iSendPacks++;
					iTotalSendBytes += iResult;
#if USE_LIAONING_SANQI == 0
					PRTMSG(MSG_DBG, "Qian Udp send %d frames, Total %d bytes!\n", iSendPacks, iTotalSendBytes);
#endif
#if USE_LIAONING_SANQI == 1
					PRTMSG(MSG_DBG, "Qian Udp send: ");
					if(iResult > 512)
					{
						PRTMSG(MSG_DBG, "Qian Udp send: %d B:Head 50 B:\n", iResult);
						PrintString(szSendBuf, 50);
					}
					else
					{
						PrintString(szSendBuf, iResult);
					}
#endif
				}
// 				else if(m_dwSymb == DEV_UPDATE)
// 				{
// 					PRTMSG(MSG_DBG, "Update Udp ip: %s port: %d, send: ", inet_ntoa(adrTo.sin_addr), htons(adrTo.sin_port));
// 					PrintString(szSendBuf, iResult);
// 				}
			}
			else if( iResult < 0 )
			{
				PRTMSG(MSG_ERR, "Udp Send Fail\n");
				perror("");
				
				goto _WRITE_SOCK_EXCEPT;
			}
		}
		
		goto _ONE_SENDLOOP_END;

_WRITE_SOCK_EXCEPT:

		PRTMSG( MSG_DBG, "%d UDP Send Thread Except!\n", m_dwSymb );
		
		m_bToRelease = true;

		sta_dwLstSndTm = GetTickCount();

		// 发送出错，通知TCP套接字
		{
			char szdata = 0x01;
			DWORD dwPacketNum = 0;
			g_objQianTcp.m_objSockStaMng.PushData(LV1, 1, &szdata, dwPacketNum);
		}

_ONE_SENDLOOP_END:
		if( m_dwSymb == DEV_DVR && bSend )
		{ 			
			if( iSendPacks - iLastPrint > 20 )
			{
				DWORD dwSndSpd = (iTotalSendBytes-sta_dwLstSndByte)/(GetTickCount() - sta_dwLstSndTm);

				PRTMSG(MSG_DBG, "DVR UDP Send %d packs, %d bytes, Tickcount:%d, Spd:%d K/s\n", iSendPacks, iTotalSendBytes, GetTickCount(), dwSndSpd);

				iLastPrint = iSendPacks;
				sta_dwLstSndByte = iTotalSendBytes;
				sta_dwLstSndTm = GetTickCount();
			}

			iOneLoopSendBytes = 0;
			bSend = false;
		}

		usleep(m_dwSleepTm);
	}
}

void CUdpSocket::P_ThreadRecv()
{
	int	  iResult;
	char  buf[ UDP_RECVSIZE ];
	struct sockaddr_in addrFrom;
	int   iLen = sizeof(addrFrom);

	buf[0] = 0x02;		// 0x02 表示中心UDP协议帧
	
	while(!g_bProgExit)
	{
		if( m_sockUdp < 0 )
		{
			usleep(100000);
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
// 				//PRTMSG(MSG_NOR, "Dvr Udp recv: ");
// 				//PrintString(buf, iResult+1);
// 
// 				// 测试用, 接收34协议帧应答
// 				if( buf[1] == 0x34 )
// 				{
// 					PRTMSG(MSG_NOR, "Recv 0x34 Test: %2x, %2x, TickCount = %d\n", buf[1], buf[2], GetTickCount());
// 					continue;
// 				}
// 			}

			if( m_dwSymb == DEV_QIAN )
  			{
				if(iResult > 512)
				{
				//	PRTMSG(MSG_NOR, "Qian Udp Recv: %d biger than 512B!!\n", iResult);
				//	PrintString(buf+1, 50);
				}
				else
				{
					PRTMSG(MSG_NOR, "Qian Udp Recv: ");
					PrintString(buf+1, iResult);
//  			}
				}
 			}

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
	}
}

bool CUdpSocket::SetIPandPort(unsigned long v_ulIp, unsigned short v_usPort)
{
	m_ulIp = v_ulIp;
	m_usPort = v_usPort;
}


