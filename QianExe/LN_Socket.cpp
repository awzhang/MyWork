#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD	("QianExe-Socket  ")

void *G_ThreadSocketSend()
{
	g_objSocket.P_ThreadSend();
}

void *G_ThreadSocketRecv()
{
	g_objSocket.P_ThreadRecv();
}

//////////////////////////////////////////////////////////////////////////

CLN_Socket::CLN_Socket()
{
	m_sockUdp = -1;
	m_pthreadSend = m_pthreadRecv = 0;
	m_bInit = false;

	m_objSendMng.InitCfg(100*1024, 10000);
}

CLN_Socket::~CLN_Socket()
{
	
}

int CLN_Socket::Init()
{
	if( pthread_create( &m_pthreadSend, NULL, G_ThreadSocketSend, NULL )
		|| 
		pthread_create( &m_pthreadRecv, NULL, G_ThreadSocketRecv, NULL ) )
	{
		PRTMSG(MSG_ERR, "Create socket send/recv thread failed\n");
		perror("");

		return ERR_THREAD;
	}
	
	return 0;
}

void CLN_Socket::Release()
{
	if( -1 != m_sockUdp )
	{
		shutdown( m_sockUdp, 2 );	//参数2表示终止读取及传送操作
		close( m_sockUdp );
		m_sockUdp = -1;
	}
}

int CLN_Socket::CreateSock()
{
	int iRet = 0;
	BOOL blVal;
	int iVal;
	LINGER ling;
	SOCKADDR_IN localaddr;
	unsigned long ulIP = INADDR_ANY;
	
	tag1QIpCfg obj1QIpCfg;
	if( GetImpCfg( (void*)&obj1QIpCfg, sizeof(obj1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(obj1QIpCfg) ) ) 
		return ERR_READCFGFAILED;

	m_ulIp = obj1QIpCfg.m_ulDvrUdpIP;
	m_usPort = obj1QIpCfg.m_usDvrUdpPort;

	DetroySock();
	
	m_sockUdp = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( -1 == m_sockUdp )
	{
		iRet = ERR_SOCK;
		return iRet;
	}
	
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(DVR_UDP_LOCAL_PROT);
	localaddr.sin_addr.s_addr = ulIP;
	if( bind( m_sockUdp, (SOCKADDR*)&localaddr, sizeof(localaddr) ) )
	{
		return ERR_SOCK;
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

	m_bInit = true;

	// 通知应用层，套接字已初始化

	return 0;
}

int CLN_Socket::DetroySock()
{
	if( -1 != m_sockUdp )
	{
		shutdown( m_sockUdp, 2 );
		close( m_sockUdp );
		m_sockUdp = -1;
	}
	m_bInit = false;
}

// 发送UDP队列中的数据
void CLN_Socket::P_ThreadSend()
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
	
	while(!g_bProgExit)
	{
		if(!m_bInit)
		{
			usleep(20000);
			continue;
		}
	
		iResult = m_objSendMng.PopData( bytLvl, sizeof(szSendBuf), dwSendLen, szSendBuf, dwPushTm, &bytSymb);
		if( iResult )
		{
			usleep(20000);
			continue;
		}
		
		if( !dwSendLen || dwSendLen > sizeof(szSendBuf) ) 
			continue;
		
		adrTo.sin_family = AF_INET;
		adrTo.sin_port = m_usPort;
		adrTo.sin_addr.s_addr = m_ulIp;
		iResult = sendto( m_sockUdp, szSendBuf, int(dwSendLen), 0, (struct sockaddr*)&adrTo, sizeof(adrTo) );
		
		if( iResult < 0 )
		{
			PRTMSG(MSG_ERR, "Udp Send Fail\n");
			perror("");

			// 发送出错，通知应用层
			
			
			// 销毁套接字
			DetroySock();
			continue;
		}
		else
		{		
 			PRTMSG(MSG_DBG, "socket send:");
 			PrintString(szSendBuf, iResult);
		}

#if NETWORK_TYPE == NETWORK_EVDO
		usleep(20000);
#elif NETWORK_TYPE == NETWORK_TD
		usleep(60000);
#else
		usleep(200000);
#endif
	}
}

void CLN_Socket::P_ThreadRecv()
{
	int	  iResult;
	char  buf[ UDP_RECVSIZE ];
	struct sockaddr_in addrFrom;
	int   iLen = sizeof(addrFrom);
	
	while(!g_bProgExit)
	{
		if(!m_bInit)
		{
			usleep(20000);
			continue;
		}

		iResult = recvfrom( m_sockUdp, buf, sizeof(buf), 0, (struct sockaddr*)&addrFrom, (socklen_t*)&iLen );
		if( iResult > 0 )
		{
 			PRTMSG(MSG_NOR, "socket recv: ");
 			PrintString(buf, iResult);

			// 接收的数据调用网络层的数据处理接口
			g_objNetWork.DealSocketData(buf, iResult);
		}
		else if( iResult < 0 )
		{
			PRTMSG(MSG_ERR, "Udp Read Err\n" );
			perror("");
			
			// 接收出错，通知应用层
						
			
			// 销毁套接字
			DetroySock();
			continue;
		}	
	}
}

bool CLN_Socket::SetIPandPort(unsigned long v_ulIp, unsigned short v_usPort)
{
	m_ulIp = v_ulIp;
	m_usPort = v_usPort;
}


