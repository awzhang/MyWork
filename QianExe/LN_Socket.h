#ifndef _LN_SOCKET_H_
#define _LN_SOCKET_H_

void *G_ThreadSocketSend(void *arg);
void *G_ThreadSocketRecv(void *arg);

class CLN_Socket
{
public:
	CLN_Socket();
	virtual ~CLN_Socket();
	
public:
	int  Init();
	void Release();
	
	int  CreateSock();
	int  DetroySock();
	
	bool SetIPandPort(unsigned long v_ulIp, unsigned short v_usPort);
	void P_ThreadSend();
	void P_ThreadRecv();
	
public:
	CInnerDataMng	m_objSendMng;
	int				m_sockUdp;
	unsigned long	m_ulIp;
	unsigned short	m_usPort;
	
	pthread_t	m_pthreadSend;
	pthread_t	m_pthreadRecv;
	bool		m_bInit;
};

#endif