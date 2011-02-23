#include "yx_UpdateStdAfx.h"

void G_TmUpdateTcpLog(void *arg, int len)
{
	g_objUpdateSock.TcpLog();
}

//////////////////////////////////////////////////////////////////////////
#undef MSG_HEAD
#define MSG_HEAD	("Update-Sock")

/* CRC 高位字节值表 */
static unsigned char auchCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
		0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
		0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
		0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
		0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ; 

/* CRC低位字节值表*/
static char auchCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
		0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
		0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
		0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
		0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
		0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
		0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
		0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
		0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
		0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
		0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
		0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
		0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
		0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
		0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
		0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
		0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
		0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
		0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
		0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
		0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
		0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
		0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
		0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
		0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
		0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

unsigned short CSock::CRC16(unsigned char *puchMsg, unsigned short usDataLen)
{
	unsigned char uchCRCHi = 0xFF ; /* 高CRC字节初始化 */
	unsigned char uchCRCLo = 0xFF ; /* 低CRC 字节初始化 */
	unsigned uIndex ; /* CRC循环中的索引 */
	
	while (usDataLen--) /* 传输消息缓冲区 */
	{
		uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex] ;
	}
	return (uchCRCHi << 8 | uchCRCLo) ;
}

CSock::CSock()
{
	memset(m_szTcpRecvFrame, 0, sizeof(m_szTcpRecvFrame));
	m_dwTcpFrameLen = 0;

	m_bTcpLogged = false;
	m_iTcpLogTimers = 0;
}

CSock::~CSock()
{

}

int CSock::Init()
{

}

int CSock::Release()
{

}

int CSock::AnalyseSockData( const char* v_szRecv, const int v_iRecvLen )
{
	if( !v_szRecv || v_iRecvLen <= 0 ) return 0;

	// 非中心协议帧
	if( 0x01 == v_szRecv[0] )
	{
		switch(v_szRecv[1])
		{
		case 0x01:		// TCP连接成功通知
			{
				TcpLog();				
			}
			break;

		case 0x02:		// UDP初始化完成通知
			{
				g_objDownLoad.m_bLoginNow = true;
				g_objDownLoad.m_bNetFined = true;
			}
			break;

		default:
			break;
		}

		return 0;
	}

	// TCP协议帧
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
							if( !_ChkCrc16( m_szTcpRecvFrame, m_dwTcpFrameLen) )
							{
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
				memset((void*)m_szTcpRecvFrame, 0, sizeof(m_szTcpRecvFrame) );
				m_dwTcpFrameLen = 0;
				return 1;
			}
		}
	}
	// 中心UDP协议帧
	else if( 0x02 == v_szRecv[0] )
	{
		g_objDownLoad.AnalyseRecvData((char*)v_szRecv, (DWORD)v_iRecvLen);
	}
	
	return 0;
}

// 7D+1->7D,7D+2->7E,同时去帧头尾7e
void CSock::_DeTranData(char *v_szBuf, DWORD &v_dwBufLen)
{
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
			if( 1 == v_szBuf[ i + 1 ] )	
			{
				buf[ dwLen ++ ] = 0x7d;
			}
			else if( 2 == v_szBuf[ i + 1 ] )
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

void CSock::_DealTcpData( char* v_szBuf, DWORD v_dwBufLen )
{
	char szTerminalID[8] = {0};
	BYTE bytFrmType = 0;
	BYTE bytCtrlFlag = 0;

	memcpy(szTerminalID, v_szBuf+2, 8);		// 移动台ID
	bytFrmType = BYTE( v_szBuf[10] );		// 指令类型
	bytCtrlFlag = BYTE( v_szBuf[11] );		// 控制字
	
	switch( bytFrmType )
	{
	case 0x81:	// 移动台登陆应答
		{
			byte bytAns = bytCtrlFlag & 0x03;

			if( 0x01 == bytAns )	//登陆成功
			{
				m_bTcpLogged = true;
				PRTMSG(MSG_NOR, "Update Tcp Log succ!\n");
			}
			else if( 0x10 == bytAns || 0x11 == bytAns )	// ID号不正确
			{
				PRTMSG(MSG_NOR, "Update Tcp Log succ, but Center alloc new ID!\n");

				tag1PComuCfg objComuCfg;
				GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );					

				memcpy(objComuCfg.m_szTernimalID, szTerminalID, 8);

				SetImpCfg(&objComuCfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );
			}
			else	// 其他则认为失败
			{
				m_bTcpLogged = false;
				PRTMSG(MSG_NOR, "Update Tcp Log failed!\n");
			}
		}
		break;
		
	case 0x82: // 查询扩展状态信息请求
		{
			_Deal0x82(v_szBuf+12, v_dwBufLen-12);
		}
		break;
		
	case 0x83: // 无线下载请求
		{
			_Deal0x83(v_szBuf+12, v_dwBufLen-12);
		}
		break;
		
	case 0x84: // 远程控制设置请求
		{
		}
		break;
		
	default:
		break;
	}
}

void CSock::_Deal0x82(char *v_szBuf, DWORD v_dwLen)
{

}

// 属性（1）+ 模式（1）+ APN（22）+ IP(20) + PORT(5)  +新版本长度n (1)+ 新版本字符串(n) + 【时间范围】(4)
void CSock::_Deal0x83(char *v_szBuf, DWORD v_dwLen)
{
	char szbuf[1024] = {0};
	int  ilen = 0;

	// 检查数据长度
	DWORD dwlen = DWORD(v_szBuf[0]*256 + v_szBuf[1]);
	if(dwlen != v_dwLen - 2)
	{
		PRTMSG(MSG_NOR, "Deal0x83 len err!\n");
		return;
	}

	// IP和端口发送到SockServExe
	szbuf[0] = 0x02;
	memcpy(szbuf+1, v_szBuf+24, 15);
	memcpy(szbuf+16, v_szBuf+44, 5);
	DataPush(szbuf, 21, DEV_UPDATE, DEV_SOCK, LV3);

#if WIRELESS_UPDATE_TYPE == 1
	// 将CDownLoad的升级标志置为真
	g_objDownLoad.m_bNeedUpdate = true;
#endif

	// 应答
	memset(szbuf, 0, sizeof(szbuf));
	char buf = 0x01;	// 成功应答
	if( !_MakeTcpFrame(&buf, 1, 0x03, 0x00, szbuf, sizeof(szbuf), ilen) )
	{		
		DataPush(szbuf, ilen, DEV_UPDATE, DEV_SOCK, LV3);
	}
}

void CSock::TcpLog()
{
	byte bytCtrl = 0;
	char szStateBuf[1024] = {0};
	int  iStateLen = 0;
	char szSendBuf[1024] = {0};
	int  iSendLen = 0;

	if(m_iTcpLogTimers > 0)		//只登陆一次，登陆不成功就重连套接字
	{
		m_iTcpLogTimers = 0;
		_KillTimer(&g_objTimerMng, UPDATE_TCP_LOG_TIMER);
		
		char buf = 0x01;	// 请求SockServExe重新连接套接字	
		DataPush(&buf, 1, DEV_UPDATE, DEV_SOCK, LV3);
		PRTMSG(MSG_NOR, "Update Tcp Log failed to max timers, request Tcp Reconnect!\n");
		
		return;
	}
	
	m_bTcpLogged = false;
	
	_SetTimer(&g_objTimerMng, UPDATE_TCP_LOG_TIMER, 30000, G_TmUpdateTcpLog );

	tag1PComuCfg objComuCfg;
	GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );					

	if( !strcmp("", objComuCfg.m_szTernimalID) )	// 移动台ID为空
		bytCtrl = 0x02;
	else
		bytCtrl = 0x01;

	// 车台基本状态信息
	iStateLen = _GetStateInfo(szStateBuf);

	if( !_MakeTcpFrame(szStateBuf, iStateLen, 0x01, bytCtrl, szSendBuf, sizeof(szSendBuf), iSendLen) )
	{
		PRTMSG(MSG_NOR, "Send Update Log Frame\n" );
		
		DataPush(szSendBuf, iSendLen, DEV_UPDATE, DEV_SOCK, LV3);
		m_iTcpLogTimers++;
	}
}

int CSock::_MakeTcpFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytType, BYTE v_bytCtrl, char* v_szDesData, int v_iDesSize, int &v_iDesLen )
{
// 标志位	校验	移动台ID	指令类型	控制字		数据长度	业务数据	标志位
// 1 Byte	2 Byte	8 Byte		1 Byte		1 Byte		2 Byte		n Byte		1 Byte
	
	char buf[ SOCK_MAXSIZE ] = { 0 };
	int iLen = 0;

	tag1PComuCfg objComuCfg;
	GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );					

	// 组帧
	memcpy(buf+2, objComuCfg.m_szTernimalID, 8);	// 移动台ID
	buf[10] = char( v_bytType );					// 指令类型
	buf[11] = char( v_bytCtrl );					// 控制字
	buf[12] = char( v_iSrcLen / 256 );				// 数据长度
	buf[13] = char( v_iSrcLen % 256 );
	memcpy(buf+14, v_szSrcData, v_iSrcLen);
	iLen = 16 + v_iSrcLen;		// iLen包括校验和
	
	// 加入校验和
	_GetCrc16(buf, buf+2, iLen-2);

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
			v_szDesData[ v_iDesLen ++ ] = buf[ i ] - 0x7d + 1;
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

// 获取基本状态信息
int CSock::_GetStateInfo(char *v_szBuf)
{
	char szSoftVer[64] = {0};
	ChkSoftVer(szSoftVer);

	char szSysVer[64] = {0};
	ReadVerFile(szSysVer);

	char szVer[64] = {0};
	sprintf(szVer, "%s-%s", szSoftVer, szSysVer);

	int len = 0;

	// 版本号
	memset(v_szBuf, 0x20, 30);
	memcpy(v_szBuf, szVer, strlen(szVer));
	len += 30;

	// 本机号码
	tag1PComuCfg objComuCfg;
	GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );					
	memcpy(v_szBuf+len, objComuCfg.m_szTel, 15);
	len += 15;

	// 基本状态标示
	char buf[4] = {0};
	byte bytSta = 0;				// ACC状态
	IOGet(IOS_ACC, &bytSta);
	if( IO_ACC_ON == bytSta )	buf[0] |= 0x01;

	tagGPSData objGps(0);			// GPS定位状态
	GetCurGps(&objGps, sizeof(objGps), GPS_REAL);
	if( 'A' == objGps.valid )	buf[0] |= 0x10;

	buf[1] |= 0x20;					// 网络在线标志

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	IOGet(IOS_QIANYA, &bytSta);		//欠压标示
	if( IO_QIANYA_VALID == bytSta )	buf[1] |= 0x80;
#endif

	memcpy(v_szBuf+len, buf, 4);
	len += 4;

	// 车台系统时间
	struct tm pCurrentTime;
	G_GetTime(&pCurrentTime);

	v_szBuf[len++] = (byte)(pCurrentTime.tm_year-100);
	v_szBuf[len++] = (byte)(pCurrentTime.tm_mon+1);
	v_szBuf[len++] = (byte)pCurrentTime.tm_mday;
	v_szBuf[len++] = (byte)pCurrentTime.tm_hour;
	v_szBuf[len++] = (byte)pCurrentTime.tm_min;
	v_szBuf[len++] = (byte)pCurrentTime.tm_sec;

	// 车台运行统计信息，太麻烦了，先不做
	len += 54;

	// 重要网络参数
	tag1QIpCfg objIpCfg[2];
	GetImpCfg( &objIpCfg, sizeof(objIpCfg),	offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]),sizeof(objIpCfg) );

	v_szBuf[len++] = 0x02;	 // 个数

	for(int i=0; i<2; i++)
	{
		v_szBuf[len++] = i+1;
		
		memset(v_szBuf+len, 0x20, 22);	// APN
		memcpy(v_szBuf+len, objIpCfg[i].m_szApn, strlen(objIpCfg[i].m_szApn));
		len += 22;
		
		memset(v_szBuf+len, 0x20, 20);	//TCP IP
		//memcpy(v_szBuf+len, objIpCfg[i].m_ulQianTcpIP, strlen(objIpCfg[i].m_ulQianTcpIP));
		len += 20;
		
		memset(v_szBuf+len, 0x20, 5);
		//memcpy(v_szBuf+len, objIpCfg[i].m_usQianTcpPort, 5);	// TCP PORT
		len += 5;
		
		memset(v_szBuf+len, 0x20, 20);	//UDP IP
		//memcpy(v_szBuf+len, objIpCfg[i].m_ulQianUdpIP, strlen(objIpCfg[i].m_ulQianUdpIP));
		len += 20;
		
		memset(v_szBuf+len, 0x20, 5);
		//memcpy(v_szBuf+len, objIpCfg[i].m_usQianUdpPort, 5);	// UDP PORT
		len += 5;
	}

	return len;
}

// 获取CRC16校验值
void CSock::_GetCrc16(char *v_szDesBuf, char *v_szSrcBuf, int v_iSrcLen)
{
	if( NULL == v_szDesBuf || NULL == v_szSrcBuf || 0 == v_iSrcLen )
		return;

	unsigned short usCrc = CRC16((byte*)v_szSrcBuf, (unsigned short)v_iSrcLen );
	memcpy(v_szDesBuf, (void*)&usCrc, 2);
}

// CRC16校验
bool CSock::_ChkCrc16(char *v_szBuf, DWORD v_dwBufLen)
{
	unsigned short usCrc = CRC16((byte*)(v_szBuf+2), (unsigned short)v_dwBufLen );

	char buf[2] = {0};
	memcpy(buf, (void*)&usCrc, 2);

	if( v_szBuf[0] != buf[0] || v_szBuf[1] != buf[1] )
		return false;
	else
		return true;
}




