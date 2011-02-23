#ifndef _YX_UPDATE_SOCK_H_
#define _YX_UPDATE_SOCK_H_

class CSock
{
public:
	CSock();
	virtual ~CSock();

public:
	int Init();
	int Release();

	int AnalyseSockData( const char* v_szRecv, const int v_iRecvLen );
	void TcpLog();

private:
	void _DealTcpData( char* v_szBuf, DWORD v_dwBufLen );
	bool _ChkCrc16( char* v_szBuf, DWORD v_dwBufLen );
	void _DeTranData( char* v_szBuf, DWORD &v_dwBufLen );
	int  _MakeTcpFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytType, BYTE v_bytCtrl, char* v_szDesData, int v_iDesSize, int &v_iDesLen );
	int  _GetStateInfo(char *v_szBuf);
	void _GetCrc16(char *v_szDesBuf, char *v_szSrcBuf, int v_iSrcLen);

	void _Deal0x82(char *v_szBuf, DWORD v_dwLen);
	void _Deal0x83(char *v_szBuf, DWORD v_dwLen);

	unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen);

private:
	char	m_szTcpRecvFrame[ TCP_RECVSIZE ];
	DWORD	m_dwTcpFrameLen;

	bool	m_bTcpLogged;
	int		m_iTcpLogTimers;
};

#endif

