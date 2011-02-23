#ifndef _YX_UDPSOCK_H_
#define _YX_UDPSOCK_H_

void *G_ThreadUdpSend(void *arg);
void *G_ThreadUdpRecv(void *arg);

class CUdpSocket
{
public:
	CUdpSocket(DWORD v_dwSymb);
	virtual ~CUdpSocket();
	
public:
	int  Init();
	void Release();

	void ReqDestroySock();
	void ReqCreateSock();

	bool SetIPandPort(unsigned long v_ulIp, unsigned short v_usPort);

	void P_ThreadSend();
	void P_ThreadRecv();
	
private:
	int  _CreateSock();
	int  _DetroySock();

public:
	unsigned long	m_ulIp;
	unsigned short	m_usPort;
	CInnerDataMng	m_objSendMng;

private:
	int				m_sockUdp;

	DWORD		m_dwSymb;
	pthread_t	m_pthreadSend;
	pthread_t	m_pthreadRecv;
	pthread_mutex_t m_mtxSock;

//	bool		m_bInit;
	volatile bool	m_bToRelease;
	volatile bool	m_bToCreate;
};

#endif


