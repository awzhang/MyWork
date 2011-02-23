#include "yx_common.h"

#undef MSG_HEAD
#define MSG_HEAD ("DvrExe-Sock  ")

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


void G_TmTcpReConn(void *arg, int len)
{
	char buf = 0x01;
	DataPush((void*)&buf, 1, DEV_DVR, DEV_SOCK, LV3);
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

	//DataPush(buf, sizeof(tmpbuf)+3, DEV_DVR, DEV_DIAODU, LV2);
}

CSock::CSock()
{
	m_bTcpLogged =  false;	
	m_iTcpLogTimers = 0;
	m_dwTcpSndTm = m_dwTcpLogTm = GetTickCount();

	memset((void*)m_szTcpRecvFrame, 0, sizeof(m_szTcpRecvFrame) );
	m_dwTcpFrameLen = 0;
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
		_KillTimer(&g_objTimerMng, DVRTCP_LOG_TIMER);

		buf[0] = 0x01;	// 请求SockServExe重新连接套接字	
		DataPush(buf, 1, DEV_DVR, DEV_SOCK, LV3);
		PRTMSG(MSG_NOR, "Tcp Log failed to max timers, request Tcp Reconnect!\n");

		// 调度屏进行提示
		G_TmShowMsg(NULL, 0);
		
		return;
	}

	m_bTcpLogged = false;
	
#if USE_LIAONING_SANQI == 1	
	buf[0] = 0x07;
	buf[1] = m_bTcpLogged ? 0x01 : 0x00;
	DataPush(buf, 2, DEV_DVR, DEV_QIAN, LV3);
#endif
	_SetTimer(&g_objTimerMng, DVRTCP_LOG_TIMER, 15000, G_TmTcpLog );
	
#if 2 == USE_PROTOCOL
	tag1PComuCfg obj1PComuCfg;
	obj1PComuCfg.Init(CDMA_NAME,CDMA_PASS);
	GetImpCfg( &obj1PComuCfg, sizeof(obj1PComuCfg),	offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
	memcpy( m_objGPRS01.m_szUserIdenCode, USERIDENCODE, sizeof(m_objGPRS01.m_szUserIdenCode) );
#endif

	GetSelfTel( m_objGPRS01.m_szParData, sizeof(m_objGPRS01.m_szParData) );
	
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

		G_RecordDebugInfo("Dvr Send Tcp Log Frame" );
		
		DataPush(buf, iLen, DEV_DVR, DEV_SOCK, LV3);
		m_iTcpLogTimers++;
	}
}

void CSock::_TcpKeep( BYTE v_bytNextInterval )
{
	if( !m_bTcpLogged ) return;
	
	PRTMSG(MSG_NOR, "Rcv Cent Tcp Link Keep Req: %d m\n", v_bytNextInterval );

	G_RecordDebugInfo("Dvr Tcp Rcv Cent Link Keep Req: %d m", v_bytNextInterval );
	
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
		DataPush(buf, iLen, DEV_DVR, DEV_SOCK, LV2);
	}

 	_SetTimer(&g_objTimerMng, DVRTCP_RECONN_TIMER, 4*v_bytNextInterval*60000 + 50000, G_TmTcpReConn);
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
				TcpLog();				
			}
			break;
			
		case 0x02:		// SoceServExe通知已断开中心
			{
				_KillTimer(&g_objTimerMng,DVRTCP_LOG_TIMER);
				m_bTcpLogged = false;
#if USE_LIAONING_SANQI == 1	
				char buf[2] = {0};
	buf[0] = 0x07;
	buf[1] = m_bTcpLogged ? 0x01 : 0x00;
	DataPush(buf, 2, DEV_DVR, DEV_QIAN, LV3);
#endif
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
								PRTMSG(MSG_NOR, "Dvr Tcp ChkSum err!\n");
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
		switch( v_szRecv[1] )
		{
#if VIDEO_BLACK_TYPE == 1
		case 0xB1:		// 黑匣子文件传输开始帧应答
			Deal38B1((char*)v_szRecv+2, (int)v_iRecvLen-2);
			break;
			
		case 0xB2:		// 黑匣子文件内容传输请求
			Deal38B2((char*)v_szRecv+2, (int)v_iRecvLen-2);
			break;
			
		case 0xB3:		// 黑匣子文件传输结束帧
			Deal38B3((char*)v_szRecv+2, (int)v_iRecvLen-2);								
			break;
#endif
			
#if VIDEO_BLACK_TYPE == 2
		case 0xD1:
			Deal38D1((char*)v_szRecv+2, (int)v_iRecvLen-2);
			break;
			
		case 0xD3:
			Deal38D3((char*)v_szRecv+2, (int)v_iRecvLen-2);
			break;
#endif
			
		case 0x03:
			Deal3803((char*)v_szRecv+2, (int)v_iRecvLen-2);
			break;
			
		case 0x35:
			Deal3835((char*)v_szRecv+2, (int)v_iRecvLen-2);
			break;
			
		case 0x36:
			Deal3836((char*)v_szRecv+2, (int)v_iRecvLen-2);
			break;
			
		default:
			break;
		}
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
			if( v_dwBufLen > 6 )	// 若长度大于6，说明是第一次登陆应答，后面带有IP地址和端口
			{
				if( 1 == v_szBuf[ dwDataPos ] ) // 若是成功应答
				{
					PRTMSG(MSG_NOR,  "TCP First Logged Succ!\n" );

					G_RecordDebugInfo("Dvr Tcp First Logged Succ!" );
					
					_KillTimer(&g_objTimerMng, DVRTCP_LOG_TIMER );					
					_KillTimer(&g_objTimerMng, SHOW_MSG_TIMER);

					// 提取IP端口，送到SockServExe
					char buf[ 200 ] = { 0 };					
					buf[0] = 0x05;	// 请求SockServExe重新连接套接字，且附带有IP端口
					memcpy(buf+1, v_szBuf+dwDataPos+1, v_dwBufLen-dwDataPos-1);
					DataPush(buf, v_dwBufLen-dwDataPos, DEV_DVR, DEV_SOCK, LV3);
				}
				else
				{
					PRTMSG(MSG_NOR, "TCP First Logged Fail\n" );
					m_iTcpLogTimers = 0;
					
					// 通过调度屏提示
					G_TmShowMsg(NULL, 0);
				}
			}
			else					// 否则是第二次登陆后的应答
			{
				if( 1 == v_szBuf[ dwDataPos ] ) // 若是成功应答
				{
					PRTMSG(MSG_NOR,  "TCP Second Logged Succ!\n" );

					G_RecordDebugInfo("Dvr Tcp Second Logged Succ!" );
					
					_KillTimer(&g_objTimerMng, DVRTCP_LOG_TIMER );
					m_bTcpLogged = true;
					
#if USE_LIAONING_SANQI == 1	
					char szbuf[ 2 ] = { 0 };
	szbuf[0] = 0x07;
	szbuf[1] = m_bTcpLogged ? 0x01 : 0x00;
	DataPush(szbuf, 2, DEV_DVR, DEV_QIAN, LV3);
#endif
					DWORD dwCur = GetTickCount();
					
					m_dwTcpLogTm = dwCur; // 确保之后重新进行登陆超时判断
					m_iTcpLogTimers = 0;
					
					char buf = 0x03;	// 通知SockServExe登陆成功	
					DataPush((void*)&buf, 1, DEV_DVR, DEV_SOCK, LV3);
					
					// 关闭提示
					_KillTimer(&g_objTimerMng, SHOW_MSG_TIMER);
					
					// 防止万一TCP登陆后的第一个链路维护没有收到
					_SetTimer(&g_objTimerMng, DVRTCP_RECONN_TIMER, 180000, G_TmTcpReConn); 
				}
				else
				{
					PRTMSG(MSG_NOR, "TCP Second Logged Fail\n" );
					m_iTcpLogTimers = 0;
					
					// 通过调度屏提示
					G_TmShowMsg(NULL, 0);
				}
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
	char  szTel[15] = { 0 };
	GetSelfTel( szTel, sizeof(szTel) );

	DealSmsFrame(v_szBuf, v_dwBufLen, szTel, sizeof(szTel));
}

int CSock::DealSmsFrame( char* v_szBuf, int v_iBufLen, char* v_szVeTel, size_t v_sizTel)
{	
	// 检查数据长度
	if( v_iBufLen < QIANDOWNFRAME_BASELEN )
	{		
		XUN_ASSERT_VALID( false, "" );
		return ERR_PAR;
	}
	
	char* szDesTelInFrm = v_szBuf;
	char* szSrcTelInFrm = v_szBuf;
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
	szDesTelInFrm = v_szBuf + 5;
	szSrcTelInFrm = v_szBuf + 20;
#endif
#if 2 == USE_PROTOCOL
	szDesTelInFrm = v_szBuf + 6;
	szSrcTelInFrm = v_szBuf + 21;
#endif
	
	BYTE bytTranType, bytDataType;
#if 0 == USE_PROTOCOL || 30 == USE_PROTOCOL
	bytTranType = BYTE( v_szBuf[3] );
	bytDataType = BYTE( v_szBuf[4] );
#endif
#if 1 == USE_PROTOCOL
	bytTranType = BYTE( v_szBuf[0] );
	bytDataType = BYTE( v_szBuf[1] );
#endif
#if 2 == USE_PROTOCOL
	bytTranType = BYTE( v_szBuf[4] );
	bytDataType = BYTE( v_szBuf[5] );
#endif
	
	// 检查请求帧的目的手机号
	if( 0x10 != bytTranType || 0x01 != bytDataType ) // 若不是设置短信中心号
	{
		if( v_szVeTel && v_szVeTel[0] && strncmp(szDesTelInFrm, v_szVeTel, v_sizTel) )
		{
			PRTMSG(MSG_DBG, "DesTel is uncorrect!\n");
			PRTMSG(MSG_DBG, "szDesTelInFrm is:%s\n", szDesTelInFrm);
			PRTMSG(MSG_DBG, "v_szVeTel is:%s\n", v_szVeTel);
			return ERR_PAR;
		}
	}
	
	// 检查校验和
	byte bytSum = get_crc_sms((byte*)v_szBuf, v_iBufLen - 1);
	if( bytSum != BYTE( v_szBuf[v_iBufLen - 1] ) )
	{
		PRTMSG(MSG_DBG, "get_crc_sms err!\n");
		return ERR_PAR;
	}
	
	// 转入具体事务处理
	AnalyzeSmsFrame( szSrcTelInFrm, szDesTelInFrm, bytTranType, bytDataType, v_szBuf + QIANDOWNFRAME_BASELEN - 1, v_iBufLen - QIANDOWNFRAME_BASELEN);
	
	return 0;
}


void CSock::AnalyzeSmsFrame( char* v_szSrcHandtel, char* v_szDesHandtel, BYTE v_bytTranType, BYTE v_bytDataType, char* v_szData, DWORD v_dwDataLen)
{
	char buf[2048] = {0};
	DWORD dwPacketNum;

	switch( v_bytTranType )
	{
	case 0x38:
		{
			buf[0] = (char)v_bytDataType;
			memcpy(buf+1, v_szData, v_dwDataLen);

			g_objQianMng.PushData(LV2, v_dwDataLen+1, buf, dwPacketNum);
		}
		break;

	default:
		break;
	}
}

// SMS封装。注意：成功执行后，v_iDesLen才赋值，否则，v_iDesLen不改变
int CSock::MakeSmsFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytTranType, BYTE v_bytDataType, char* v_szDesData, int v_iDesSize, int &v_iDesLen, int v_iCvtType /*= CVT_NONE*/, bool v_bTcp/*=true*/ )
{
// 上行数据格式（车载终端 --→ 网控中心）

// 通用版,公开版_研三
//	区域号	版本号	业务类型	数据类型	车台手机号			用户数据	校验码
//	2 Byte	1 Byte	1 Byte		1 Byte		15(UDP和短信才有)	 n Byte		1 Byte

// 公开版_研一
// 区域号	版本号	业务类型	数据类型	车台手机号			用户数据	校验码
// 2字节	2字节	1字节		1字节		15字节(都有)		n字节		1字节

	int iSrcLen = 0, iLen = 0;
	bool bNeedAddTel = false;
	char szSrc[ SOCK_MAXSIZE ] = { 0 };
	char buf[ SOCK_MAXSIZE ] = { 0 };

	// 用户数据编码转换处理
	switch( v_iCvtType )
	{
	case CVT_8TO6:
		iSrcLen = (int)(Code8To6( szSrc, v_szSrcData, v_iSrcLen, sizeof(szSrc) ) );
		break;

	case CVT_8TO7:
		iSrcLen = Code8To7( v_szSrcData, szSrc, v_iSrcLen, sizeof(szSrc) );
		break;

	default:
		if( sizeof(szSrc) < v_iSrcLen ) return ERR_BUFLACK;
		memcpy( szSrc, v_szSrcData, v_iSrcLen );
		iSrcLen = v_iSrcLen;
	}

	if( v_bTcp )
	{
		if( iSrcLen > sizeof(buf) - QIANUPTCPFRAME_BASELEN )
		{
			XUN_ASSERT_VALID( false, "" );
			return ERR_BUFLACK;
		}
	}
	else
	{
		if( iSrcLen > sizeof(buf) - QIANUPUDPFRAME_BASELEN )
		{
			XUN_ASSERT_VALID( false, "" );
			return ERR_BUFLACK;
		}
	}

	/// 以下开始组帧
	tag2QServCodeCfg objServCodeCfg;
	GetSecCfg( &objServCodeCfg, sizeof(objServCodeCfg),	offsetof(tagSecondCfg, m_uni2QServCodeCfg), sizeof(objServCodeCfg) );

#if 0 == USE_PROTOCOL
	memcpy( buf, objServCodeCfg.m_szAreaCode, 2 ); // 区域号
	if( v_bTcp && 0 == buf[0] ) buf[0] = 0x7f; // 纠正区域号的转义方式
	else if( !v_bTcp && 0x7f == buf[0] ) buf[0] = 0;
	if( v_bTcp && 0 == buf[1] ) buf[1] = 0x7f; // 纠正区域号的转义方式
	else if( !v_bTcp && 0x7f == buf[1] ) buf[1] = 0;
	buf[ 2 ] = 0x31; // 版本号
	buf[ 3 ] = (char)v_bytTranType; // 业务类型
	buf[ 4 ] = (char)v_bytDataType; // 数据类型
	iLen = 5;
#endif

#if 1 == USE_PROTOCOL
	memcpy( buf + 2, objServCodeCfg.m_szAreaCode, 2 ); // 区域号
	if( v_bTcp && 0 == buf[2] ) buf[2] = 0x7f; // 纠正区域号的转义方式
	else if( !v_bTcp && 0x7f == buf[2] ) buf[2] = 0;
	if( v_bTcp && 0 == buf[3] ) buf[3] = 0x7f; // 纠正区域号的转义方式
	else if( !v_bTcp && 0x7f == buf[3] ) buf[3] = 0;
	buf[ 4 ] = 0x31; // 版本号
	buf[ 0 ] = (char)v_bytTranType; // 业务类型
	buf[ 1 ] = (char)v_bytDataType; // 数据类型
	iLen = 5;
#endif

#if 2 == USE_PROTOCOL
	memcpy( buf, objServCodeCfg.m_szAreaCode, 2 ); // 区域号
	if( v_bTcp && 0 == buf[0] ) buf[0] = 0x7f; // 纠正区域号的转义方式
	else if( !v_bTcp && 0x7f == buf[0] ) buf[0] = 0;
	if( v_bTcp && 0 == buf[1] ) buf[1] = 0x7f; // 纠正区域号的转义方式
	else if( !v_bTcp && 0x7f == buf[1] ) buf[1] = 0;
	buf[ 2 ] = 0x10; // 版本号
	buf[ 3 ] = 0x70;
	buf[ 4 ] = (char)v_bytTranType; // 业务类型
	buf[ 5 ] = (char)v_bytDataType; // 数据类型
	iLen = 6;
#endif

	if( v_bTcp )
		bNeedAddTel = false;
	else
		bNeedAddTel = true;

#if USE_PROTOCOL == 2
	bNeedAddTel = true;
#endif

	if( !bNeedAddTel )
	{
		memcpy( buf + iLen, szSrc, iSrcLen ); // 用户数据
		iLen += iSrcLen;
	}
	else
	{
		char szTel[ 15 ] = { 0 };
		memset( szTel, 0x20, sizeof(szTel) );
		GetSelfTel( szTel, sizeof(szTel) );

		memcpy( buf + iLen, szTel, 15 );
		memcpy( buf + iLen + 15, szSrc, iSrcLen ); // 用户数据

		iLen += 15 + iSrcLen;
	}

	// 加入校验和
	buf[ iLen ] = (char)( get_crc_sms((byte*)buf, iLen) );

	// 封装后长度
	iLen ++;

	// 拷贝结果
	if( iLen > v_iDesSize )
	{
		XUN_ASSERT_VALID( false, "" );
		return ERR_BUFLACK;
	}
	memcpy( v_szDesData, buf, iLen );
	v_iDesLen = iLen;
	
	return 0;
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

void CSock::SendTcpData(char *v_szBuf, int v_iLen)
{
	if( !m_bTcpLogged )
		return;
	
	char  szSendBuf[TCP_SENDSIZE] = {0};
	int   iSendLen = 0;

	if( !_MakeTcpFrame(v_szBuf, v_iLen, (byte)0x02, (byte)0x10, (char*)szSendBuf, (int)sizeof(szSendBuf), iSendLen ) )
	{
		// 将数据推到业务支撑子层进行发送
		DataPush((void*)szSendBuf, (DWORD)iSendLen, DEV_DVR, DEV_SOCK, LV2);
	}
}

bool CSock::IsOnline()
{
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

int CSock::GetSelfTel(char *v_szBuf, size_t v_sizTel )
{
	unsigned int uiLen = 0;
	tag1PComuCfg obj1PComuCfg;
	
	GetImpCfg( &obj1PComuCfg, sizeof(obj1PComuCfg),	offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
	
	memset( v_szBuf, 0x20, v_sizTel );
	memcpy( v_szBuf, obj1PComuCfg.m_szTel, strlen(obj1PComuCfg.m_szTel) < v_sizTel ? strlen(obj1PComuCfg.m_szTel) : v_sizTel );
	
	if( 0x20 == v_szBuf[0] || 0x30 == v_szBuf[0] || !strcmp("", v_szBuf) )
	{
		PRTMSG(MSG_NOR, "Get SelfTel, but Incorrect or Empty!\n" );
	}
	
	return 0;
}



