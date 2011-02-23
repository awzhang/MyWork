#ifndef _YX_SOCK_H_
#define _YX_SOCK_H_

void G_TmTcpLog(void *arg, int len);
void G_TmTcpReConn(void *arg, int len);

class CSock
{
public:
	CSock();
	virtual ~CSock();

	void Init();
	void Release();
	bool IsOnline();
	void LetComuChkGPRS();

	void TcpLog();
	int  AnalyseSockData( const char* v_szRecv, const int v_iRecvLen );
	void SendTcpData(char *v_szBuf, int v_iLen);
	int  GetSelfTel(char *v_szBuf, size_t v_sizTel );
	int  MakeSmsFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytTranType, BYTE v_bytDataType, char* v_szDesData, int v_iDesSize, int &v_iDesLen, int v_iCvtType = CVT_NONE, bool v_bTcp=true );
	int  DealSmsFrame( char* v_szBuf, int v_iBufLen, char* v_szVeTel, size_t v_sizTel);
	void AnalyzeSmsFrame( char* v_szSrcHandtel, char* v_szDesHandtel, BYTE v_bytTranType, BYTE v_bytDataType, char* v_szData, DWORD v_dwDataLen);

private:	
	void _TcpKeep( BYTE v_bytNextInterval );
	void _DealTcpData( char* v_szBuf, DWORD v_dwBufLen );
	void _DealGprsTran( BYTE v_bytGrade, char* v_szBuf, DWORD v_dwBufLen );
	bool _ChkSum( char* v_szBuf, DWORD v_dwBufLen );
	void _DeTranData( char* v_szBuf, DWORD &v_dwBufLen );
	int  _MakeTcpFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytType, BYTE v_bytGrade, char* v_szDesData, int v_iDesSize, int &v_iDesLen );

private:
	char	m_szTcpRecvFrame[ TCP_RECVSIZE ];
	DWORD	m_dwTcpFrameLen;

	tagGPRS01 m_objGPRS01;

	bool	m_bTcpLogged;
	int		m_iTcpLogTimers;
	DWORD	m_dwTcpLogTm; // TCP初始登陆时刻
	DWORD	m_dwTcpSndTm; // Tcp初始发送成功时刻
};


#endif

