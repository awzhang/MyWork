#ifndef _YX_OIL_H_
#define _YX_OIL_H_

class COil
{
public:
	COil();
	virtual ~COil();

public:
	int	Init();
	int Release();

	int ComOpen();
	int ComWrite(char *v_szBuf, int v_iLen);

	void P_ThreadRecv();
	void P_ThreadSend();
	void AnalyseComData(char *v_szBuf, int v_iLen);
	void DealComFrame(char *v_szBuf, int v_iLen);

	int  TranData(char *v_szBuf, int v_iLen);
	int  DetranData(char *v_szBuf, int v_iLen);

	bool check_crc(const byte *buf, const int len);
	byte get_crc(const byte *buf, const int len);

	int  DealDiaoDu(char *v_szData, DWORD v_dwDataLen);

	int  Deal3713(byte *v_szData, int v_dwDataLen);
	int  Deal3712(byte *v_szData, int v_dwDataLen);
	int  Deal3711(byte *v_szData, int v_dwDataLen);
	int  Deal3710(byte *v_szData, int v_dwDataLen);

	int  QueryOilData(byte &v_bytOilSta, unsigned short &v_usOilAD, unsigned short &v_usPowAD);
	int  QueryVersion(char *v_szBuf, int v_iBufLen, int &v_ilen);
	int  QueryStatus();
	int  QueryBiaoDingStart();
	int  QueryBiaoDingEnd();

	bool bConnect();
	bool SetOilAndPowAD(byte v_bytRptType, byte v_bytOilSta, unsigned short v_usOilAD, unsigned short v_usPowAD);
	bool GetOilAndPowAD(byte &v_bytRptType, byte &v_bytOilSta, unsigned short &v_usOilAD, unsigned short &v_usPowAD);

private:
	int _DealComuC4(char *v_szData, DWORD v_dwDataLen);
	int _DealComuC3(char *v_szData, DWORD v_dwDataLen);
	int _DealComuC1(char *v_szData, DWORD v_dwDataLen);
	int _DealComuBDFull(char *v_szData, DWORD v_dwDataLen);
	int _DealComuBDF5(char *v_szData, DWORD v_dwDataLen);
	int _DealComuBDF4(char *v_szData, DWORD v_dwDataLen);
	int _DealComuBDF3(char *v_szData, DWORD v_dwDataLen);
	int _DealComuBDF2(char *v_szData, DWORD v_dwDataLen);
	int _DealComuBDF1(char *v_szData, DWORD v_dwDataLen);
	int _DealComuBD34(char *v_szData, DWORD v_dwDataLen);
	int _DealComuBD14(char *v_szData, DWORD v_dwDataLen);
	int _DealComuBD12(char *v_szData, DWORD v_dwDataLen);
	int _DealComuBD00(char *v_szData, DWORD v_dwDataLen);
	int _DealComu51(char *v_szData, DWORD v_dwDaraLen);

public:
	int		m_iComPort;
	char	m_szComFrameBuf[1024];
	int		m_iComFrameLen;
	bool	m_bComConnect;

	Queue			m_QueRecv;
	CInnerDataMng	m_objSendMng;
	CInnerDataMng	m_objRecvMng;

	byte			m_bytRptType;
	byte			m_bytOilSta;
	unsigned short	m_usOilAD;
	unsigned short	m_usPowAD;
	pthread_mutex_t	m_MutexAD;

	ushort	m_usONRealOilData[5][2];
	ushort  m_usOFFRealOilData[5][2];
};

#endif


