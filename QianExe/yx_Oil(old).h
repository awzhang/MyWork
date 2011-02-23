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
	void P_ThreadRead();
	void P_ThreadAutoRptOil();
	void AnalyseComData(char *v_szBuf, int v_iLen);
	int  DetranData(char *v_szBuf, int v_iLen);
	bool check_crc(const byte *buf, const int len);

	byte oil_setting(char *param, int len);
	void SetOilAndPowAD(WORD v_usOilAD, WORD v_usPowAD, BYTE v_bytOilSta, BYTE v_bytRptType);
	bool bConnect(bool &v_bOilConnect);
	bool sendfrm(BYTE cmd, char *param, int len);
	int  query_ver(char *outbuf);//100216ford,由于现在链路维护和版本查询现在已经分开，所以这个函数里对油耗盒做了查版本
	byte query_state();//100216ford,由于现在链路维护和版本查询现在已经分开，所以这个函数里对油耗盒做了查状态,返回0x55，说明无应答
	bool query_link();//100216ford,由于现在链路维护和版本查询现在已经分开，所以这个函数里对油耗盒做了探寻
	int  OilDataDesire(char *param, char *outbuf, int len);
	int  GetOilAndPowAd(WORD *v_wOilAD, WORD *v_wPowAD, BYTE *v_bytOilSta, BYTE *v_bytRptType);
	byte get_crc(const byte *buf, const int len);
	int  DealDiaoDu(char *v_szData, DWORD v_dwDataLen);
	void DealAccChg();
	int  deal_OilRpt();
	int  Deal3713(byte *v_szData, int v_dwDataLen);
	int  Deal3712(byte *v_szData, int v_dwDataLen);
	int  Deal3711(byte *v_szData, int v_dwDataLen);
	int  Deal3710(byte *v_szData, int v_dwDataLen);
	int  biaoDEnd();
	int  biaodDStart();
	BOOL GetOilPowAndSta( BYTE* v_p_bytOilAD, BYTE *v_p_bytPowAD, BYTE* v_p_bytOilSta );
	void SetOilPowAndSta( BYTE v_bytOilAD, BYTE v_bytPowAD, BYTE v_bytOilSta );
	void SetEvt( BYTE v_bytAccEvt, BYTE v_bytSpdEvt, bool v_bInit );
	
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

private:
	int		m_iComPort;
	Queue	m_QueOilRecv;

	char	m_szComFrameBuf[1024];
	int		m_iComFrameLen;

	ushort	m_usRealOilAD;
	ushort	m_usRealPowAD;
	ushort	m_usValidOilAD;
	ushort	m_usValidPowAD;

	BYTE	m_bytOilAD;
	BYTE	m_bytPowAD;
	BYTE	m_bytOilSta;
	BYTE	m_bytRptType;
	
	bool	m_bOilConnect;	//油耗盒是否已连接

	DWORD	m_dwCmdSetTm;
	BYTE	m_bytEvt;

	pthread_mutex_t m_mutex_Oil;
	pthread_mutex_t m_mutex_OilData;
	pthread_mutex_t m_mutex_COil;
	pthread_mutex_t m_mutex_cmd;

	char	buf_sp[ALABUF];
	char	frm_sp[ALABUF];
	ushort	m_usONRealOilData[5][2];
	ushort  m_usOFFRealOilData[5][2];

	tagDrvRecSampleCfg m_objDrvRecSampleCfg;
};

#endif


