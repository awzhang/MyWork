#ifndef _YX_TCPSOCK_H_
#define _YX_TCPSOCK_H_

void *G_ThreadTcpWork(void *arg);
//void *G_ThreadTcpSend(void *arg);

enum TCPSOCKSTA
{
	Idle = 1,
	Conn,
	WaitConn,
	Working,
	DisConn
};

#define MAX_TEMP_IP_CNT	5

class CTcpSock
{
public:
	CTcpSock(DWORD v_dwSymb);
	virtual ~CTcpSock();

	int Init();
	int Release();
	
	void P_ThreadWork();
	void P_ThreadSend();

private:
	bool _JudgeConFailMaxTime(int &v_iFailTimes);

public:
	int			m_sockTcp;
	DWORD		m_dwSymb;
	pthread_t	m_pthreadWork;
	pthread_t	m_pthreadSend;
	byte		m_bytSockSta;

	CInnerDataMng	m_objSockStaMng;
	CInnerDataMng	m_objSendMng;
	struct sockaddr_in m_conn;
	bool		m_bInit;
	bool		m_bConnect;
	bool		m_bLogged;

	// 视频部分独立出来，用于保存临时的IP和端口
	bool	m_bFirstConnect;
	int		m_iIPCnt;
	bool	m_bIPEnabled[MAX_TEMP_IP_CNT];
	unsigned long m_ulTempIP[MAX_TEMP_IP_CNT];
	unsigned short m_usTempTcpPort[MAX_TEMP_IP_CNT];
	unsigned short m_usTempUdpPort[MAX_TEMP_IP_CNT];
};

#endif

