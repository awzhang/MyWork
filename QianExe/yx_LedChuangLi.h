#ifndef _YX_LED_CHUANGLI_H_
#define _YX_LED_CHUANGLI_H_

class CLedChuangLi
{
public:
	CLedChuangLi();
	virtual ~CLedChuangLi();

	int Init();
	int Release();

	bool ComOpen();
	bool ComClose();
	int  ComWrite(char *v_szBuf, int v_iLen);

	int  Deal0x42(BYTE v_bytDataType,char *v_szData, DWORD v_dwDataLen);
	int  Deal4208(char *v_szBuf, int v_iLen);
	int  TranData(char *v_szBuf, int v_iLen);
	void MakeLedFrame(char v_szCmd, char v_szType, char v_szInfoNo, char *v_szInfoBuf, int v_iInfoLen, char *v_p_szFrameBuf, int &v_p_iFrameLen);
	bool SendCmdForAns(char *v_szbuf, int v_iLen);
	void P_ThreadRecv();
	void P_ThreadWork();

	void DealEvent(char v_szCmd, char v_szType, char v_szInfoNo);

	bool check_crc(const byte *buf, const int len);
	byte get_crc(const byte *buf, const int len);

public:
	CInnerDataMng	m_objWorkMng;
	CInnerDataMng	m_objReadMng;

private:
	int		m_iComPort;

	byte	m_bytInfoNo;	// 中心下发信息的信息号

	pthread_t m_pthreadRecv;
	pthread_t m_pthreadWork;
};

#endif

