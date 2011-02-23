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

	int  TcpSendSmsData( char* v_szBuf, int v_iBufLen, BYTE v_bytLvl, BYTE v_bytSymb = 0, char* v_szSmsTel = NULL );
	void TcpLog();
	void UdpLog();
	int  AnalyseSockData( const char* v_szRecv, const int v_iRecvLen );
	int  DealUdpFrame(char *v_szRecv, const int v_iRecvLen);
	void SendTcpBufData();
	int  MakeUdpFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytType, BYTE v_bytGrade, char* v_szDesData, int v_iDesSize, int &v_iDesLen );

private:	
	void _TcpKeep( BYTE v_bytNextInterval );
	void _DealTcpData( char* v_szBuf, DWORD v_dwBufLen );
	void _DealGprsTran( BYTE v_bytGrade, char* v_szBuf, DWORD v_dwBufLen );
	bool _ChkSum( char* v_szBuf, DWORD v_dwBufLen );
	void _DeTranData( char* v_szBuf, DWORD &v_dwBufLen );

	int  _MakeTcpFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytType, BYTE v_bytGrade, char* v_szDesData, int v_iDesSize, int &v_iDesLen );
	void _TcpBufSave0155();
	void _TcpBufTurnSms();
	int  _SmsSndSockData( char* v_szBuf, DWORD v_dwBufLen, bool v_bFromTcp, char* v_szDesCode, DWORD v_dwDesCodeLen );
	int  _AddSelfTel( char* v_szBuf, DWORD &v_dwLen, size_t v_sizBuf );
	BYTE _GradeToLvl( BYTE v_bytGrade );
	BYTE _LvlToGrade( BYTE v_bytLvl );
	BYTE _LvlToComuLvl( BYTE v_bytLvl );

private:
	char	m_szTcpRecvFrame[ TCP_RECVSIZE ];
	DWORD	m_dwTcpFrameLen;

	tagGPRS01 m_objGPRS01;
	CInnerDataMng m_obj0155DataMng;

	bool	m_bTcpLogged;
	bool	m_bUdpLogged;
	int		m_iTcpLogTimers;
	int		m_iUdpLogTimers;
	DWORD	m_dwTcpLogTm; // TCP初始登陆时刻
	DWORD	m_dwTcpSndTm; // Tcp初始发送成功时刻
#if USE_LIAONING_SANQI == 1
public:
	char m_bDVRTcpLoged;
#endif
};

inline BYTE CSock::_GradeToLvl( BYTE v_bytGrade )
{
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
// 10H	最高优先级
// 20H	普通优先级
// 30H	最低优先级
	switch( v_bytGrade )
	{
	case 0x10:
		return LV13;
	case 0x20:
		return LV9;
	case 0X30:
		return LV4;
	default:
		return LV9;
	}
#endif

#if 2 == USE_PROTOCOL
//（1）0x04：高优先级；（2）0x08：普通优先级；（3）0x0c：低优先级
	switch( v_bytGrade )
	{
	case 0x04:
		return LV13;
	case 0x08:
		return LV9;
	case 0X0c:
		return LV4;
	default:
		return LV9;
	}
#endif
}

inline BYTE CSock::_LvlToGrade( BYTE v_bytLvl )
{
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
// 10H	最高优先级
// 20H	普通优先级
// 30H	最低优先级
	switch( v_bytLvl )
	{
	case LV1:
	case LV2:
	case LV3:
	case LV4:
	case LV5:
		return 0x30;
	case LV6:
	case LV7:
	case LV8:
	case LV9:
	case LV10:
	case LV11:
		return 0x20;
	case LV12:
	case LV13:
	case LV14:
	case LV15:
	case LV16:
		return 0x10;
	default:
		return 0x20;
	}
#endif

#if 2 == USE_PROTOCOL
//（1）0x04：高优先级；（2）0x08：普通优先级；（3）0x0c：低优先级
	switch( v_bytLvl )
	{
	case LV1:
	case LV2:
	case LV3:
	case LV4:
	case LV5:
		return 0x0c;
	case LV6:
	case LV7:
	case LV8:
	case LV9:
	case LV10:
	case LV11:
		return 0x08;
	case LV12:
	case LV13:
	case LV14:
	case LV15:
	case LV16:
		return 0x04;
	default:
		return 0x08;
	}
#endif
}

inline BYTE CSock::_LvlToComuLvl( BYTE v_bytLvl )
{
	switch( v_bytLvl )
	{
	case LV1:
	case LV2:
	case LV3:
	case LV4:
	case LV5:
		return 1;
	case LV6:
	case LV7:
	case LV8:
	case LV9:
	case LV10:
	case LV11:
		return 2;
	case LV12:
	case LV13:
	case LV14:
	case LV15:
	case LV16:
		return 3;
	default:
		return 2;
	}
}

#endif

