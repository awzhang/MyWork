#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-Sock     ")

#if USE_LANGUAGE == 0
#define LANG_PAR ("参数(")
#define LANG_TELIPPORT ("手机号 IP 端口")
#define LANG_AREACODE (" 区域号")
#define LANG_NOSET (")未设置!")
#define LANG_CHKING_U ("正在检测U盘...")
#endif

#if USE_LANGUAGE == 11
#define LANG_PAR ("Config(")
#define LANG_TELIPPORT ("Tel IP Port")
#define LANG_AREACODE (" Areacode")
#define LANG_NOSET (") no established !")
#define LANG_CHKING_U ("U Disk Searching...")
#endif

int G_Sock_JugSmsSndFrameType(void* v_pElement, void* v_pSymb, DWORD v_dwSymbSize )
{
	if( !v_pElement || !v_pSymb || 2 != v_dwSymbSize )
		return 1;
	else
	{
		if( ((char*)v_pSymb)[0] == ((char*)v_pElement)[3]
			&&
			((char*)v_pSymb)[1] == ((char*)v_pElement)[4]  )
		{
			return 0;
		}
		else return 1;
	}
}

void G_TmTcpLog(void *arg, int len)
{
	g_objSock.TcpLog();
}

void G_TmUdpLog(void *arg, int len)
{
	g_objSock.UdpLog();
}

void G_TmTcpReConn(void *arg, int len)
{
	char buf = 0x01;	// QianExe请求SockServExe重新连接套接字
	
	G_RecordDebugInfo("No link keep, ask for reconnn!");

	DataPush((void*)&buf, 1, DEV_QIAN, DEV_SOCK, LV3);
}

void G_TmShowMsg(void *arg, int len)
{
	static DWORD sta_dwLastShowTm = GetTickCount();
	static bool sta_bFirst = true;
	if( GetTickCount() - sta_dwLastShowTm > 60*1000 || sta_bFirst )	// 提示间隔要大于1分钟
	{
		sta_bFirst = false;
		sta_dwLastShowTm = GetTickCount();
	}
	else
		return;

	char tmpbuf[] = {"登陆失败，请检查手机、IP、端口是否配置正确！"};
	char buf[100] = {0};
	
	buf[0] = 0x01;
	buf[1] = 0x72;
	buf[2] = 0x01;
	memcpy(buf+3, tmpbuf, sizeof(tmpbuf));

	//DataPush(buf, sizeof(tmpbuf)+3, DEV_QIAN, DEV_DIAODU, LV2);
}

CSock::CSock()
{
	m_bTcpLogged =  false;	
	m_bUdpLogged = false;
#if USE_LIAONING_SANQI == 1
	m_bDVRTcpLoged = false;
#endif
	m_iTcpLogTimers = 0;
	m_iUdpLogTimers = 0;
	m_dwTcpSndTm = m_dwTcpLogTm = GetTickCount();

	memset((void*)m_szTcpRecvFrame, 0, sizeof(m_szTcpRecvFrame) );
	m_dwTcpFrameLen = 0;

	m_obj0155DataMng.InitCfg( 100000, 7200000 );
}

CSock::~CSock()
{

}

void CSock::Init()
{

}

void CSock::TcpLog()
{
	char buf[ 200 ] = { 0 };
	int iLen = 0;

	// TCP连续登陆失败次数达到上限
	if(m_iTcpLogTimers > TCP_MAX_LOG_TIMERS)
	{
		m_iTcpLogTimers = 0;
		_KillTimer(&g_objTimerMng, TCP_LOG_TIMER);

		buf[0] = 0x01;	// QianExe请求SockServExe重新连接套接字	
		DataPush(buf, 1, DEV_QIAN, DEV_SOCK, LV3);
		PRTMSG(MSG_NOR, "Qian Tcp Log failed to max timers, request Tcp Reconnect!\n");

		// 调度屏进行提示
		G_TmShowMsg(NULL, 0);
		
		return;
	}

	m_bTcpLogged = false;
	
	_SetTimer(&g_objTimerMng, TCP_LOG_TIMER, 15000, G_TmTcpLog );
	
#if 2 == USE_PROTOCOL
	tag1PComuCfg obj1PComuCfg;
	obj1PComuCfg.Init(CDMA_NAME,CDMA_PASS);
	GetImpCfg( &obj1PComuCfg, sizeof(obj1PComuCfg),	offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
	memcpy( m_objGPRS01.m_szUserIdenCode, USERIDENCODE, sizeof(m_objGPRS01.m_szUserIdenCode) );
#endif

	g_objCfg.GetSelfTel( m_objGPRS01.m_szParData, sizeof(m_objGPRS01.m_szParData) );
	
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
	const BYTE GPRSLOGFRM_TYPE = 0x11;
	const BYTE GPRSLOGFRM_GRADE = 0x10;
#endif
#if 2 == USE_PROTOCOL
	const BYTE GPRSLOGFRM_TYPE = 0x01;
	const BYTE GPRSLOGFRM_GRADE = 0x04;
#endif
	if( !_MakeTcpFrame( (char*)&m_objGPRS01, sizeof(m_objGPRS01), GPRSLOGFRM_TYPE, GPRSLOGFRM_GRADE, buf, sizeof(buf), iLen ) )
	{
		PRTMSG(MSG_NOR, "Send Tcp Log Frame\n" );

		G_RecordDebugInfo("Qian Send Tcp Log Frame");

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		RenewMemInfo(0x02, 0x02, 0x01, 0x04);
#endif

		// 将数据推到业务支撑子层进行发送
		DataPush(buf, iLen, DEV_QIAN, DEV_SOCK, LV3);
		m_iTcpLogTimers++;
	}
}

void CSock::UdpLog()
{
	m_iUdpLogTimers++;
	if( m_iUdpLogTimers > 3 )	// udp连续登陆不成功超过三次，则请求重新拨号
	{
		m_iUdpLogTimers = 0;
		_KillTimer(&g_objTimerMng, UDP_LOG_TIMER);
		
		PRTMSG(MSG_NOR, "Phot udp log failed to max timers! Ask ComuExe to queck net!\n");
		
		// 请求ComuExe重新拨号
		char buf[] = {0x34, 0x01};
		DataPush((void*)&buf, 2, DEV_SOCK, DEV_PHONE, LV3);
		
		m_bTcpLogged = false;	
		m_bUdpLogged = false;
		
		// 向调度屏发送提示
		G_TmShowMsg(NULL, 0);		
		return;
	}
	
	PRTMSG(MSG_NOR, "Send Udp Log Frame\n");

	G_RecordDebugInfo("Qian Send Udp Log Frame");
	
	// 组帧发送
	char buf[100] = {0};
	int  ilen = 0;
	tag1PComuCfg obj1PComuCfg;
	GetImpCfg( (void*)&obj1PComuCfg, sizeof(obj1PComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
	
	// 通用版协议,公开版协议_研三
	// 校验		优先级	类型	用户数据
	// 1 Byte	1 Byte	1 Byte	n Byte
	
	// 公开版协议_研一
	// 版本号（1）	校验(1)	优先级(1)	类型(1)	数据(n)
	
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	buf[ 1 ] = 0x10;	// 优先级
	buf[ 2 ] = 0x11;	// 类型
	buf[3] = 0x02;		// 参数个数
	buf[4] = 0x01;		// 参数类型：手机号
	buf[5] = 0x0f;		// 参数长度：手机号长度为15
	memcpy(buf+6, obj1PComuCfg.m_szTel, 15);
	buf[21] = 0x02;		// 参数类型：版本号
	buf[22] = 0x01;		// 参数长度：版本号长度为1
	buf[23] = 0x31;		// 版本号为0x31
	buf[0] = get_crc_gprs((byte*)(buf+1), 23);	//校验和
	
	ilen = 24;
#endif
	
#if USE_PROTOCOL == 2			
	buf[0] = 0x03;	// 版本号
	buf[1] = 0x00;	// 校验和，先置0
	buf[2] = 0x04;	// 优先级
	buf[3] = 0x01;	// 类型
	buf[4] = 0x31;	// 用户识别码
	buf[5] = 0x31;
	buf[6] = 0x31;
	buf[7] = 0x31;
	buf[8] = 0x31;
	buf[9] = 0x31;
	buf[10] = 0x01;	// 参数个数
	buf[11] = 0x01;	// 参数类型：手机号
	buf[12] = 0x0f;	// 参数长度：手机号长度为15
	memcpy(buf+13, obj1PComuCfg.m_szTel, 15);
	buf[1] = get_crc_gprs((byte*)buf+1, 27);
	
	ilen = 28;
#endif
	
	char szSendbuf[100] = {0};
	szSendbuf[0] = 0x04;
	memcpy(szSendbuf+1, buf, ilen);
	DataPush(szSendbuf, ilen+1, DEV_QIAN, DEV_SOCK, LV2);
}

void CSock::_TcpKeep( BYTE v_bytNextInterval )
{
	if( !m_bTcpLogged ) return;
	
	PRTMSG(MSG_NOR, "Rcv Cent Tcp Link Keep Req: %d m\n", v_bytNextInterval );

	G_RecordDebugInfo("Qian Rcv Cent Link Keep Req: %d m", v_bytNextInterval );
	
	char buf[ 100 ] = { 0 };
	int iLen = 0;
	
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
	char* pData = NULL;
	int iDataLen = 0;
	const BYTE FRM04_GRADE = 0x10;
#endif
	
#if 2 == USE_PROTOCOL
	char cAnsType = 1;
	char* pData = &cAnsType;
	int iDataLen = sizeof(cAnsType);
	const BYTE FRM04_GRADE = 0x04;
#endif
	
	if( !_MakeTcpFrame( pData, iDataLen, 0x04, FRM04_GRADE, buf, sizeof(buf), iLen ) )
	{
		// 将数据推到业务支撑子层进行发送
		DataPush(buf, iLen, DEV_QIAN, DEV_SOCK, LV2);
	}

// 	// 测试用
// 	char szbuf = 0x01;
// 	g_objSms.MakeSmsFrame(&szbuf, 1, 0x38, 0x34, buf, sizeof(buf), iLen);
// 	g_objSock.SOCKSNDSMSDATA( buf, iLen, LV9 );

 	_SetTimer(&g_objTimerMng, TCP_RECONN_TIMER, 4*v_bytNextInterval*60000 + 50000, G_TmTcpReConn);
}

int CSock::AnalyseSockData( const char* v_szRecv, const int v_iRecvLen )
{
	if( !v_szRecv || v_iRecvLen <= 0 ) return 0;

	// 非中心协议帧
	if( 0x01 == v_szRecv[0] )
	{
		switch(v_szRecv[1])
		{
		case 0x01:		// SoceServExe通知已连接中心
			{
#if USE_LIAONING_SANQI == 0
				TcpLog();
#endif
#if USE_LIAONING_SANQI == 1
				g_objLogin.Init();
#endif
			}
			break;
			
		case 0x02:		// SoceServExe通知已断开中心
			{
				_KillTimer(&g_objTimerMng,TCP_LOG_TIMER);
				m_bTcpLogged = false;
				m_bUdpLogged = false;
			}
			break;

		case 0x03:		// SoceServExe请求向调度屏发送提示
			{
				G_TmShowMsg(NULL, 0);
				_SetTimer(&g_objTimerMng, SHOW_MSG_TIMER, 30000, G_TmShowMsg);
			}
			break;
			
		default:
			break;
		}

		return 0;
	}

	// 中心TCP协议帧
	else if( 0x00 == v_szRecv[0] )
	{
		int i7ECount = 0;
		if( m_dwTcpFrameLen > 0 ) i7ECount = 1;
		
		int iBegin = 1;
		for( int i = 1; i < v_iRecvLen; i ++ )
		{
			if( 0x7e == v_szRecv[ i ] )
			{
				++ i7ECount;
				
				if( 0 == i7ECount % 2 ) // 若得到一帧
				{
					if( m_dwTcpFrameLen + (i - iBegin + 1) <= sizeof(m_szTcpRecvFrame) )
					{
						memcpy( m_szTcpRecvFrame + m_dwTcpFrameLen, v_szRecv + iBegin, i - iBegin + 1 );
						m_dwTcpFrameLen += (DWORD)(i - iBegin + 1);
						
						if( m_dwTcpFrameLen <= 2 )
						{
							++ i7ECount;
							iBegin = i;
							memset((void*)m_szTcpRecvFrame, 0, sizeof(m_szTcpRecvFrame) );
							m_dwTcpFrameLen = 0;
							continue;
						}
						else
						{
							// 反转义
							_DeTranData( m_szTcpRecvFrame, m_dwTcpFrameLen );
							
							// 计算校验和
							if( !_ChkSum( m_szTcpRecvFrame, m_dwTcpFrameLen) )
							{
								PRTMSG(MSG_DBG, "_ChkSum Err!\n");
								memset((void*)m_szTcpRecvFrame, 0, sizeof(m_szTcpRecvFrame) );
								m_dwTcpFrameLen = 0;
								continue;
							}
							
							// 协议解析和处理
							_DealTcpData( m_szTcpRecvFrame, m_dwTcpFrameLen );
							
							memset((void*)m_szTcpRecvFrame, 0, sizeof(m_szTcpRecvFrame) );
							m_dwTcpFrameLen = 0;
						}
					}
					else
					{
						XUN_ASSERT_VALID( false, "" );
						memset((void*)m_szTcpRecvFrame, 0, sizeof(m_szTcpRecvFrame) );
						m_dwTcpFrameLen = 0;
					}
				}
				else
				{
					iBegin = i;
				}
			}
		}
		
		if( 0 != i7ECount % 2 ) // 若最后有不完整帧数据
		{
			if( m_dwTcpFrameLen + (v_iRecvLen - iBegin) < sizeof(m_szTcpRecvFrame) )
			{
				memcpy( m_szTcpRecvFrame + m_dwTcpFrameLen, v_szRecv + iBegin, v_iRecvLen - iBegin );
				m_dwTcpFrameLen += (DWORD)(v_iRecvLen - iBegin);
			}
			else
			{
				XUN_ASSERT_VALID( false, "" );
				memset((void*)m_szTcpRecvFrame, 0, sizeof(m_szTcpRecvFrame) );
				m_dwTcpFrameLen = 0;
				return 1;
			}
		}
	}
	// 中心UDP协议帧
	else if( 0x02 == v_szRecv[0] )
	{
#if USE_LIAONING_SANQI == 0
		DealUdpFrame((char*)v_szRecv+1, (int)v_iRecvLen-1);
#endif
#if USE_LIAONING_SANQI == 1
		g_objNetWork.DealSocketData((char*)v_szRecv+1, (DWORD)v_iRecvLen-1);
#endif
	}
	
	return 0;
}

int CSock::DealUdpFrame(char *v_szRecv, const int v_iRecvLen)
{
	if( !v_szRecv || v_iRecvLen <= 0 ) return 0;
	
	// 计算校验和
	if( !_ChkSum( (char*)v_szRecv, v_iRecvLen) )
	{
		PRTMSG(MSG_NOR, "Rcv Udp Frame, but Chksum Err!\n" );
		return ERR_PAR;
	}
		
	// 协议解析和处理
	BYTE bytFrmType = 0;
	BYTE bytGrade = 0;
	DWORD dwDataPos = 0;
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	dwDataPos = 3;
	bytGrade = BYTE( v_szRecv[ 1 ] );
	bytFrmType = BYTE( v_szRecv[ 2 ] );
#endif
#if 2 == USE_PROTOCOL
	dwDataPos = 4;
	bytGrade = BYTE( v_szRecv[ 2 ] );
	bytFrmType = BYTE( v_szRecv[ 3 ] );
#endif
	
	switch( bytFrmType )
	{
	case 0x81:
	case 0x91: // 移动台登陆应答
		{			
			if( 1 == v_szRecv[ dwDataPos ] )
			{
				m_bUdpLogged = true;
				PRTMSG(MSG_NOR, "UDP Logged Succ!\n" );

				G_RecordDebugInfo("Qian Udp Log Succ");
				
				_KillTimer(&g_objTimerMng, UDP_LOG_TIMER );
				m_iUdpLogTimers = 0;
			}
			else
				PRTMSG(MSG_NOR, "UDP Logged Fail, Continue Logging...\n" );
		}
		break;	
		
	default:
		break;
	}
		
	return 0;
}

// 7D+1->7D,7D+2->7E,同时去帧头尾7e
void CSock::_DeTranData(char *v_szBuf, DWORD &v_dwBufLen)
{
	XUN_ASSERT_VALID( (v_dwBufLen >= 7), "" );
	XUN_ASSERT_VALID( (0x7e == v_szBuf[0] && 0x7e == v_szBuf[v_dwBufLen - 1]), "" );
	
	char buf[ SOCK_MAXSIZE ];
	DWORD dwLen = 0;
	for( int i = 1; i < (int)(v_dwBufLen) - 1; )
	{
		if( 0x7d != v_szBuf[ i ] || i == (int)(v_dwBufLen) - 2 )
		{
			buf[ dwLen ++ ] = v_szBuf[ i++ ];
		}
		else
		{
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
			if( 1 == v_szBuf[ i + 1 ] )
#endif
#if 2 == USE_PROTOCOL
				if( 0 == v_szBuf[ i + 1 ] )
#endif
				{
					buf[ dwLen ++ ] = 0x7d;
				}
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
				else if( 2 == v_szBuf[ i + 1 ] )
#endif
#if 2 == USE_PROTOCOL
					else if( 1 == v_szBuf[ i + 1 ] )
#endif
				{
					buf[ dwLen ++ ] = 0x7e;
				}
				else
				{
					buf[ dwLen ++ ] = v_szBuf[ i ];
					buf[ dwLen ++ ] = v_szBuf[ i + 1 ];
				}
				i += 2;
		}
	}
	
	memcpy( v_szBuf, buf, dwLen );
	v_dwBufLen = dwLen;
}

// 7D+0->7D,7D+1->7E,去帧头尾7e之后,检查校验和
bool CSock::_ChkSum(char *v_szBuf, DWORD v_dwBufLen)
{
	XUN_ASSERT_VALID( (v_dwBufLen >= 5), "" );
	
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
	byte sum = get_crc_gprs((byte*)(v_szBuf+1), v_dwBufLen-1);

	return sum == (BYTE)( v_szBuf[ 0 ] );
#endif

#if 2 == USE_PROTOCOL
	byte sum = get_crc_gprs((byte*)v_szBuf, v_dwBufLen);

	return sum == (BYTE)( v_szBuf[ 1 ] );
#endif
}

/// 7D+1->7D,7D+2->7E,去帧头尾7e之后,但没有完全解开GPRS封装
void CSock::_DealTcpData( char* v_szBuf, DWORD v_dwBufLen )
{
	if( v_dwBufLen < TCPFRAME_BASELEN )
	{
		XUN_ASSERT_VALID( false, "" );
		return;
	}

	DWORD dwDataPos = 0;
	BYTE bytFrmType = 0;
	BYTE bytGrade = 0;
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	dwDataPos = 3;
	bytGrade = BYTE( v_szBuf[ 1 ] );
	bytFrmType = BYTE( v_szBuf[ 2 ] );
#endif
#if 2 == USE_PROTOCOL
	dwDataPos = 4;
	bytGrade = BYTE( v_szBuf[ 2 ] );
	bytFrmType = BYTE( v_szBuf[ 3 ] );
#endif
	
	switch( bytFrmType )
	{
	case 0x81:
	case 0x91: // 移动台登陆应答
		{
			if( 1 == v_szBuf[ dwDataPos ] ) // 若是成功应答
			{
				PRTMSG(MSG_NOR,  "TCP Logged Succ!\n" );

				G_RecordDebugInfo("Qian Tcp Log Succ");

				_KillTimer(&g_objTimerMng, TCP_LOG_TIMER );
				m_bTcpLogged = true;

				DWORD dwCur = GetTickCount();

				m_dwTcpLogTm = dwCur; // 确保之后重新进行登陆超时判断
				m_iTcpLogTimers = 0;

				// 若首次TCP登陆成功,则启动GPRS上报检查
				static bool sta_bFstLogged = true;
				if( sta_bFstLogged )
				{
#if USE_AUTORPT == 1
					g_objAutoRpt.Init();
#endif
					sta_bFstLogged = false;
				}

				char buf = 0x03;	// QianExe通知SockServExe登陆成功	
				DataPush((void*)&buf, 1, DEV_QIAN, DEV_SOCK, LV3);

				// 关闭提示
				_KillTimer(&g_objTimerMng, SHOW_MSG_TIMER);

				// 防止万一TCP登陆后的第一个链路维护没有收到
				_SetTimer(&g_objTimerMng, TCP_RECONN_TIMER, 180000, G_TmTcpReConn); 

#if 0 == USE_PROTOCOL	
				// 发送登陆帧，并启动登陆超时定时器
 				UdpLog();
 				_SetTimer(&g_objTimerMng, UDP_LOG_TIMER, 20000, G_TmUdpLog);
#endif
				QIANMSG msg;
				msg.m_lMsgType = MSG_TCPTOSEND;
				msgsnd(g_MsgID, (void*)&msg, MSG_MAXLEN, 0);
			}
			else
			{
				PRTMSG(MSG_NOR, "TCP Logged Fail\n" );
				m_iTcpLogTimers = 0;

				// 通过调度屏提示
				G_TmShowMsg(NULL, 0);
			}
		}
		break;

	case 0x82: // 移动台数据传输应答
		{
			if( 0x02 == v_szBuf[ dwDataPos ] ) // 若移动台未登陆
			{
				m_dwTcpLogTm = GetTickCount(); // 先初始化时间,确保重新进行登陆超时判断
				TcpLog();
			}
		}
		break;

	case 0x83: // 业务数据
		{
			_DealGprsTran( bytGrade, v_szBuf + dwDataPos, v_dwBufLen - TCPFRAME_BASELEN );
		}
		break;

	case 0x84: // 前置机链路探询
		{
			_TcpKeep( BYTE(v_szBuf[ dwDataPos ]) );
		}
		break;

	default:
		XUN_ASSERT_VALID( false, "" );
	}
}


/// 处理GPRS数据中的业务帧 (传入的数据是已经解开GPRS封装的)
void CSock::_DealGprsTran( BYTE v_bytGrade, char* v_szBuf, DWORD v_dwBufLen )
{
	DWORD dwPacketNum = 0;

	BYTE bytLvl = _GradeToLvl( v_bytGrade );
	g_objRecvSmsDataMng.PushData( bytLvl, v_dwBufLen, v_szBuf, dwPacketNum, DATASYMB_RCVFROMSOCK );
}

int CSock::_MakeTcpFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytType, BYTE v_bytGrade, char* v_szDesData, int v_iDesSize, int &v_iDesLen )
{
	// 通用版,公开版_研三
	// 标志位	校验	优先级	类型	用户数据	标志位
	// 1 Byte	1 Byte	1 Byte	1 Byte	n Byte		1 Byte
	
	// 公开版_研一
	// 0x7E	版本号（1）	校验(1)	优先级(1)	类型(1)	数据(n)	0x7E	
	
	char buf[ SOCK_MAXSIZE ] = { 0 };
	int iLen = 0;
	
	if( v_iSrcLen > sizeof(buf) - TCPFRAME_BASELEN )
	{
		XUN_ASSERT_VALID( false, "" );
		return ERR_DATA_INVALID;
	}
	
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	// 组帧
	buf[ 1 ] = char( v_bytGrade ); // 优先级
	buf[ 2 ] = char( v_bytType ); // 类型
	memcpy( buf + 3, v_szSrcData, v_iSrcLen );
	iLen = 3 + v_iSrcLen;
	
	// 加入校验和
	buf[0] = char( get_crc_gprs((byte*)(buf+1), iLen-1) );
#endif
	
#if 2 == USE_PROTOCOL
	// 组帧
	buf[ 0 ] = 3; // 版本号
	buf[ 2 ] = char( v_bytGrade ); // 优先级
	buf[ 3 ] = char( v_bytType ); // 类型
	memcpy( buf + 4, v_szSrcData, v_iSrcLen );
	iLen = 4 + v_iSrcLen;
	
	// 加入校验和
	buf[1] = char( get_crc_gprs((byte*)(buf), iLen) );
#endif
	
	// 转义,加头尾7e
	memset( (void*)v_szDesData, 0, v_iDesSize );
	v_iDesLen = 0;
	v_szDesData[ v_iDesLen ++ ] = 0x7e;
	for( int i = 0; i < iLen; i ++ )
	{
		if( 0x7d != buf[ i ] && 0x7e != buf[ i ] )
		{
			if( v_iDesLen >= v_iDesSize )
			{
				XUN_ASSERT_VALID( false, "" );
				return ERR_BUFLACK;
			}
			v_szDesData[ v_iDesLen ++ ] = buf[ i ];
		}
		else
		{
			if( v_iDesLen >= v_iDesSize - 1 )
			{
				XUN_ASSERT_VALID( false, "" );
				return ERR_BUFLACK;
			}
			v_szDesData[ v_iDesLen ++ ] = 0x7d;
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
			v_szDesData[ v_iDesLen ++ ] = buf[ i ] - 0x7d + 1;
#endif
#if 2 == USE_PROTOCOL
			v_szDesData[ v_iDesLen ++ ] = buf[ i ] - 0x7d;
#endif
		}
	}
	if( v_iDesLen >= v_iDesSize - 1 )
	{
		XUN_ASSERT_VALID( false, "" );
		return ERR_BUFLACK;
	}
	v_szDesData[ v_iDesLen ++ ] = 0x7e;
	
	// 拷贝结果
	if( v_iDesLen > v_iDesSize )
	{
		XUN_ASSERT_VALID( false, "" );
		return ERR_BUFLACK;
	}
	
	return 0;
}

/// 生成最终UDP协议帧,即UDP封装
int CSock::MakeUdpFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytType, BYTE v_bytGrade, char* v_szDesData, int v_iDesSize, int &v_iDesLen )
{
	// 通用版协议,公开版协议_研三
	// 校验		优先级	类型	用户数据
	// 1 Byte	1 Byte	1 Byte	n Byte
	
	// 公开版协议_研一
	// 版本号（1）	校验(1)	优先级(1)	类型(1)	数据(n)
	
	char buf[ SOCK_MAXSIZE ] = { 0 };
	int iLen = 0;
	
	if( v_iSrcLen > sizeof(buf) - UDPFRAME_BASELEN )
	{
		XUN_ASSERT_VALID( false, "" );
		return ERR_DATA_INVALID;
	}
	
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	// 组帧
	buf[ 1 ] = char( v_bytGrade );
	buf[ 2 ] = char( v_bytType );
	memcpy( buf + 3, v_szSrcData, v_iSrcLen );
	iLen = 3 + v_iSrcLen;
	
	// 加入校验和
	buf[0] = char( get_crc_gprs((byte*)(buf+1), iLen-1) );

	// 拷贝结果
	if( iLen > v_iDesSize )
	{
		XUN_ASSERT_VALID( false, "" );
		return ERR_BUFLACK;
	}
	memcpy( v_szDesData, buf, iLen );
	v_iDesLen = iLen;
	
	return 0;
#endif
	
#if 2 == USE_PROTOCOL
	// 组帧
	buf[ 0 ] = 3; // 版本号
	buf[ 2 ] = char( v_bytGrade );
	buf[ 3 ] = char( v_bytType );
	memcpy( buf + 4, v_szSrcData, v_iSrcLen );
	iLen = 4 + v_iSrcLen;
	
	// 加入校验和
	buf[1] = char( get_crc_gprs((byte*)buf, iLen) );

	// 转义,加头尾7e
	memset( (void*)v_szDesData, 0, v_iDesSize );
	v_iDesLen = 0;
	v_szDesData[ v_iDesLen ++ ] = 0x7e;
	for( int i = 0; i < iLen; i ++ )
	{
		if( 0x7d != buf[ i ] && 0x7e != buf[ i ] )
		{
			if( v_iDesLen >= v_iDesSize )
			{
				XUN_ASSERT_VALID( false, "" );
				return ERR_BUFLACK;
			}
			v_szDesData[ v_iDesLen ++ ] = buf[ i ];
		}
		else
		{
			if( v_iDesLen >= v_iDesSize - 1 )
			{
				XUN_ASSERT_VALID( false, "" );
				return ERR_BUFLACK;
			}
			v_szDesData[ v_iDesLen ++ ] = 0x7d;
			v_szDesData[ v_iDesLen ++ ] = buf[ i ] - 0x7d;
		}
	}
	if( v_iDesLen >= v_iDesSize - 1 )
	{
		XUN_ASSERT_VALID( false, "" );
		return ERR_BUFLACK;
	}
	v_szDesData[ v_iDesLen ++ ] = 0x7e;
	
	// 拷贝结果
	if( v_iDesLen > v_iDesSize )
	{
		XUN_ASSERT_VALID( false, "" );
		return ERR_BUFLACK;
	}	
	return 0;
#endif
}

int CSock::TcpSendSmsData(char *v_szBuf, int v_iBufLen, BYTE v_bytLvl, BYTE v_bytSymb /*= 0*/, char* v_szSmsTel /*=NULL*/ )
{
	int iResult = 0;
	char szSendBuf[TCP_SENDSIZE] = {0};
	int  iSendLen = 0;
	BYTE bytGrade = 0;

	// 若强制使用短信发送的,则立即使用短信发送
	if( DATASYMB_USESMS & v_bytSymb )
	{
		iResult = _SmsSndSockData( v_szBuf, DWORD(v_iBufLen), true, v_szSmsTel, 15 );
		if( ERR_SMS_DESCODE == iResult )
		{
			v_bytSymb &= ~DATASYMB_USESMS;
			v_bytSymb &= ~DATASYMB_SMSBACKUP;
		}
		else if( !iResult )
		{
			return iResult;
		}
	}

	// 否则推入发送队列(现在并不组成GPRS帧,临发送时才组帧)
	DWORD dwPacketNum = 0;
	iResult = g_objTcpSendMng.PushData( v_bytLvl, v_iBufLen, v_szBuf, dwPacketNum, v_bytSymb, v_szSmsTel, 15 );

	if( iResult ) 
		return iResult;
	else
	{
		if( !IsOnline() )
		{
			_TcpBufTurnSms(); // TCP缓存里的数据转由短信发送
			_TcpBufSave0155(); // TCP缓存里的0154数据转为盲区补传数据保存
		}
		else
		{
			QIANMSG msg;
			msg.m_lMsgType = MSG_TCPTOSEND;
			msgsnd(g_MsgID, (void*)&msg, MSG_MAXLEN, 0);
		}
	}

	return 0;
}

void CSock::SendTcpBufData()
{
	char szOrignBuf[ TCP_SENDSIZE ] = { 0 }, szNewBuf[ TCP_SENDSIZE ] = { 0 }, szReserve[64] = { 0 };
	DWORD dwOrignLen;
	DWORD dwPushTm = 0;
	int dwNewLen;
	int iResult;
	BYTE bytLvl;
	BYTE bytSymb = 0; // 要初始化
	BYTE bytGrade;
	bool b0155Data = false;

	do 
	{
		memset( szReserve, 0, sizeof(szReserve) );

		iResult = g_objTcpSendMng.PopData( bytLvl, sizeof(szOrignBuf), dwOrignLen, szOrignBuf, dwPushTm,
			&bytSymb, szReserve, sizeof(szReserve) );
		
		if( iResult )
		{
			iResult = m_obj0155DataMng.PopData( bytLvl, sizeof(szOrignBuf), dwOrignLen, szOrignBuf, dwPushTm,
				&bytSymb, szReserve, sizeof(szReserve) );
			if( iResult ) break;
			else b0155Data = true;
		}
		else b0155Data = false;

		if( !dwOrignLen || dwOrignLen > sizeof(szOrignBuf) )
			continue;

		if( !(DATASYMB_DONETCPFRAME & bytSymb) )
		{
			bytGrade = _LvlToGrade( bytLvl );
			iResult = _MakeTcpFrame(szOrignBuf, int(dwOrignLen), 0x02, bytGrade, szNewBuf, sizeof(szNewBuf), dwNewLen);
			if( iResult ) continue;
		}
		else
		{
			dwNewLen = dwOrignLen < sizeof(szNewBuf) ? dwOrignLen : sizeof(szNewBuf);
			memcpy( szNewBuf, szOrignBuf, dwNewLen );
		}

		iResult == DataPush(szNewBuf, dwNewLen, DEV_QIAN, DEV_SOCK, LV2);
	}while( iResult == 0 ); 
}

void CSock::_TcpBufTurnSms()
{
	CInnerDataMng objTcpDataMng( QUEUE_TCPSND_SIZE, QUEUE_TCPSND_PERIOD );
	int iResult = 0;
	DWORD dwBufLen = 0;
	DWORD dwPushTm = 0;
	DWORD dwPacketNum = 0;
	char buf[ SOCK_MAXSIZE ];
	char szReserv[ 16 ] = { 0 };
	BYTE bytLvl = 0, bytSymb = 0;
	
	bool bLevData = false;
	do
	{
		memset( buf, 0, sizeof(buf) );
		memset( szReserv, 0, sizeof(szReserv) );
		iResult = g_objTcpSendMng.PopData( bytLvl, sizeof(buf), dwBufLen, buf, dwPushTm, &bytSymb, szReserv, sizeof(szReserv) );
		
		if( iResult )
			break;

		if( !dwBufLen || dwBufLen > sizeof(buf) )
			break;
		
		if( !(DATASYMB_SMSBACKUP & bytSymb) && !(DATASYMB_USESMS & bytSymb) )
		{
			objTcpDataMng.PushData( bytLvl, dwBufLen, buf, dwPacketNum, bytSymb, szReserv, sizeof(szReserv) );
			bLevData = true;
		}
		else
		{
			iResult = _SmsSndSockData( buf, dwBufLen, true, szReserv, sizeof(szReserv) );
			if( ERR_SMS_DESCODE == iResult )
			{
				objTcpDataMng.PushData( bytLvl, dwBufLen, buf, dwPacketNum, 
					(bytSymb & ~DATASYMB_SMSBACKUP) & ~DATASYMB_USESMS, szReserv, sizeof(szReserv) );
				bLevData = true;
			}
		}
	} while( !iResult );
	
	if( bLevData ) g_objTcpSendMng = objTcpDataMng;
}

void CSock::_TcpBufSave0155()
{
	DWORD dwBufLen = 0, dwPushTm, dwPacketNum = 0;
	BYTE bytLvl = 0;
	char buf[200] = { 0 };
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
	const char szCmp[] = {0x01, 0x54};
#endif
#if 2 == USE_PROTOCOL
	const char szCmp[] = {0x01, 0x56};
#endif
	
	while( !g_objTcpSendMng.PopSymbData(G_Sock_JugSmsSndFrameType, (void*)szCmp, sizeof(szCmp), bytLvl, sizeof(buf), dwBufLen, buf, dwPushTm) )
	{
		if( dwBufLen > QIANUPTCPFRAME_BASELEN )
		{
#if 0 == USE_PROTOCOL
			buf[ 4 ] = 0x55; // 修改为盲区补传类型
#endif
#if 1 == USE_PROTOCOL
			buf[ 1 ] = 0x55; // 修改为盲区补传类型
#endif
#if 2 == USE_PROTOCOL
			buf[ 5 ] = 0x55; // 修改为盲区补传类型
			dwBufLen -= 3; // 转为盲区补传数据时去除一些字段
#endif			
			buf[ dwBufLen - 1 ] = char( get_crc_sms((unsigned char*)buf, dwBufLen - 1) ); // 重新计算校验和
			
			m_obj0155DataMng.PushData( bytLvl, dwBufLen, buf, dwPacketNum );
		}
	}
}

int CSock::_SmsSndSockData( char* v_szBuf, DWORD v_dwBufLen, bool v_bFromTcp, char* v_szDesCode, DWORD v_dwDesCodeLen )
{
	// 0x38 + 请求模块（1）＋目的手机号或特服号（16）＋短信内容（n）
	// 注1:请求模块为1,则表示由千里眼模块
	// 请求为2,则表示由流媒体模块请求
	// 注2：目的手机或特服号，以0结尾，不填空格
	
	int iResult = 0;
	bool bNeedAddSelfTel = false;
	char szSnd[ 400 ] = { 0 };
	char szTmp[ 300 ] = { 0 };
	DWORD dwSndLen = 0;
	DWORD dwTmpLen = 0;
	bool bUseSmsCodeInCfg = false; // 必须初始化为false
	szSnd[ 0 ] = 0x38;
	szSnd[ 1 ] = 1;
	
	// 填充接收方特服号
	if( !v_szDesCode || v_dwDesCodeLen < 1 || v_dwDesCodeLen > 16 )
		// 若参数中无法取得接收方特服号,或者特服号参数长度不正确,则使用配置中的特服号
	{
		bUseSmsCodeInCfg = true;
	}
	else
	{
		// 拷贝参数中的特服号
		memcpy( szSnd + 2, v_szDesCode, v_dwDesCodeLen );
		
		// 生成短信应答时的目的手机号
		szSnd[ 2 + 16 - 1 ] = 0;
		char* pBlank = strchr( szSnd + 2, 0x20 );
		if( pBlank ) *pBlank = 0;
		
		if( strlen(szSnd + 2) < 1 )
		{
			bUseSmsCodeInCfg = true;
		}
	}
	if( bUseSmsCodeInCfg )
	{
		g_objCfg.GetSmsTel( szSnd + 2, 16 ); // 返回参数是以NULL结尾的
	}
	if( 0 == szSnd[2] || 0x20 == szSnd[2] || 0 != szSnd[17] )
	{ // 若最后填充的特服号无效
		return ERR_SMS_DESCODE;
	}
	
	// 缓存发送数据,准备处理
	if( v_dwBufLen > sizeof(szTmp) )
		return ERR_BUFLACK;
	memcpy( szTmp, v_szBuf, v_dwBufLen );
	dwTmpLen = v_dwBufLen;
	
	// 若是TCP数据,增加本机手机号
	if( v_bFromTcp )
		bNeedAddSelfTel = true;
	else
		bNeedAddSelfTel = false;
	if( bNeedAddSelfTel )
	{
		iResult = _AddSelfTel( szTmp, dwTmpLen, sizeof(szTmp) ); // tcp数据没有加手机号,转为短信发送时要加上
		if( iResult ) return iResult;
	}
	
	// 7->6
	dwSndLen = Code7To6( szSnd + 18, szTmp, dwTmpLen, sizeof(szSnd) - 18 );
	if( dwSndLen > 0 )
	{
		// 合成好的数据推入发送队列
		iResult = DataPush(szSnd, dwSndLen+18, DEV_QIAN, DEV_PHONE, LV2);
		if(iResult != 0) return iResult;
	}
	
	return 0;
}

int CSock::_AddSelfTel( char* v_szBuf, DWORD &v_dwLen, size_t v_sizBuf )
{
	if( v_dwLen < QIANUPTCPFRAME_BASELEN ) return ERR_PAR;
	if( v_dwLen + 15 > v_sizBuf ) return ERR_SPACELACK;
	
	DWORD dwTelAddPos = 0;
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	dwTelAddPos = 5;
#endif
#if 2 == USE_PROTOCOL
	dwTelAddPos = 6;
#endif
	
	for( DWORD dw = v_dwLen - 1; dw >= dwTelAddPos; dw -- )
		v_szBuf[ dw + 15 ] = v_szBuf[ dw ];
	
	v_dwLen += 15;
	int iRet = g_objCfg.GetSelfTel( v_szBuf + dwTelAddPos, 15 );
	
	if( !iRet )
	{
		// 重算SMS帧的校验和
		v_szBuf[ v_dwLen - 1 ] = char( get_crc_sms( (BYTE*)v_szBuf, v_dwLen - 1 ) );
	}
	return iRet;
}


bool CSock::IsOnline()
{
	//return (bool)(m_bTcpLogged & m_bUdpLogged);
	return m_bTcpLogged;
}

void CSock::LetComuChkGPRS()
{
	static DWORD sta_dwReqTm = 0;
	DWORD dwCur = GetTickCount();
	if( 0 == sta_dwReqTm || dwCur - sta_dwReqTm >= 2500 )
	{
		PRTMSG(MSG_NOR, "Req Comu Chk net\n" );
		char buf[] = {0x34, 0x02};
		DataPush(buf, 2, DEV_QIAN, DEV_PHONE, LV2);
		sta_dwReqTm = dwCur;
	}
}



