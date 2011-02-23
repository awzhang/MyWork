#ifndef _YX_COMBUS_H_
#define _YX_COMBUS_H_

class CComBus
{
public:
	CComBus();
	virtual ~CComBus();

	int Init();
	int Release();

	bool ComOpen();
	bool ComClose();
	int  ComWrite(char *v_szBuf, int v_iLen);

	int  DetranData(char *v_szBuf, int v_iLen);
	int  TranData(char *v_szBuf, int v_iLen);
	void AnalyseComFrame(char *v_szBuf, int v_iLen);
	void DealComBusFrame(char *v_szBuf, int v_iLen);

	void P_ThreadComBusRecv();
	void P_ThreadComBusSend();

	bool check_crc(const byte *buf, const int len);
	byte get_crc(const byte *buf, const int len);

private:
	void _SetComPara();

public:
	CInnerDataMng	m_objComBusWorkMng;

private:
	int		m_iComPort;
	bool	m_bComConnect;
	char	m_szComFrameBuf[1024];
	int		m_iComFrameLen;
	CInnerDataMng	m_objComBusReadMng;

	pthread_t m_pthreadRecv;
	pthread_t m_pthreadSend;
};

#endif

