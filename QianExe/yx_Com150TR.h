#ifndef _YX_COM150TR_H_
#define _YX_COM150TR_H_

class CCom150TR
{
public:
	CCom150TR();
	virtual ~CCom150TR();
	
	int Init();
	int Release();
	
	bool ComOpen();
	bool ComClose();
	int  ComWrite(char *v_szBuf, int v_iLen);
	
	int  DetranData(char *v_szBuf, int v_iLen);
	int  TranData(char *v_szBuf, int v_iLen);
	void AnalyseComFrame(char *v_szBuf, int v_iLen);
	void DealCom150TRFrame(char *v_szBuf, int v_iLen);
	
	void P_ThreadCom150TRRecv();
	void P_ThreadCom150TRSend();
	
	byte GetIOSignal();
	DWORD GetCycleCount();
	
	bool check_crc(const byte *buf, const int len);
	byte get_crc(const byte *buf, const int len);
	
private:
	void _SetComPara();
	
public:
	CInnerDataMng	m_objCom150TRWorkMng;
	
private:
	int		m_iComPort;
	bool	m_bComConnect;
	char	m_szComFrameBuf[1024];
	int		m_iComFrameLen;
	CInnerDataMng	m_objCom150TRReadMng;
	byte	m_bytIOSignal;
	DWORD	m_dwCycleCount;
	
	pthread_t m_pthreadRecv;
	pthread_t m_pthreadSend;
	pthread_mutex_t	m_mutexIOSignal;
};

#endif

