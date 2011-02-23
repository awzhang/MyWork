#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-Sms      ")

CSms::CSms()
{

}

CSms::~CSms()
{

}

// SMS封装。注意：成功执行后，v_iDesLen才赋值，否则，v_iDesLen不改变
int CSms::MakeSmsFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytTranType, BYTE v_bytDataType, char* v_szDesData, int v_iDesSize, int &v_iDesLen, int v_iCvtType /*= CVT_NONE*/, bool v_bTcp/*=true*/ )
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
		g_objCfg.GetSelfTel( szTel, sizeof(szTel) );

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

int CSms::DealSmsFrame( char* v_szBuf, int v_iBufLen, char* v_szVeTel, size_t v_sizTel, bool v_bFromSM )
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
	
	// 检查区域号
	if( 0x10 != bytTranType || (0x11 != bytDataType && 0x01 != bytDataType) )
	{

	}
	
	// 检查请求帧的目的手机号
	if( 0x10 != bytTranType || 0x01 != bytDataType ) // 若不是设置短信中心号
	{
		if( v_szVeTel && v_szVeTel[0] && strncmp(szDesTelInFrm, v_szVeTel, v_sizTel) )
		{
			return ERR_PAR;
		}
	}
	
	// 检查校验和
	byte bytSum = get_crc_sms((byte*)v_szBuf, v_iBufLen - 1);
	if( bytSum != BYTE( v_szBuf[v_iBufLen - 1] ) )
	{
		PRTMSG(MSG_DBG, "get_crc_sms Err!\n");
		return ERR_PAR;
	}
	
	// 转入具体事务处理
	bool bInputWholePar = false;
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
	if( 0x10 == bytTranType && 0x01 == bytDataType ) // 若是设置短信中心号,则输入完整帧
		bInputWholePar = true;
#endif
#if 2 == USE_PROTOCOL
	if( 0x10 == bytTranType && 0x3f == bytDataType ) // 若是设置短信中心号,则输入完整帧
		bInputWholePar = true;
#endif
	if( bInputWholePar )
	{
		_AnalyzeSmsFrame( szSrcTelInFrm, szDesTelInFrm, bytTranType, bytDataType, v_szBuf, v_iBufLen - 1, v_bFromSM );
	}
	else
	{
		_AnalyzeSmsFrame( szSrcTelInFrm, szDesTelInFrm, bytTranType, bytDataType, v_szBuf + QIANDOWNFRAME_BASELEN - 1, v_iBufLen - QIANDOWNFRAME_BASELEN, v_bFromSM );
	}
	
	return 0;
}

// 视频监控的建链请求，想想还是放在QianExe里面直接处理得了
bool CSms::_Deal3805(char *v_szData, DWORD v_dwDataLen)
{
	int  iRet = 0;
	int  iBufLen;
	BYTE bytResType = 2;
	char buf[100] = {0};
	unsigned long ulDvrUdpIP;
	unsigned short usDvrUdpPort;
	
	// 先进行长度检验
	if( 21 != v_dwDataLen )
	{
		iRet = ERR_PAR;
		PRTMSG(MSG_ERR, "Deal3805 failed! 1\n");
		bytResType = 2;
		goto _DEAL3805_END;
	}
	
	// 解析出IP地址和端口
	switch(v_szData[0])
	{
	case 0x01:
	case 0x02:
	case 0x03:
		{
			// 通知SockServExe更改IP和端口
			buf[0] = 0x02;		// QianExe通知SockServExe视频监控建链的IP和端口
			memcpy(buf+1, v_szData+1, v_dwDataLen-1);
			DataPush(buf, v_dwDataLen, DEV_QIAN, DEV_SOCK, LV2);

			bytResType = 0x01;
		}
		break;
		
	default:
		break;
		
	}
	
_DEAL3805_END:
	int iRet2 = g_objSms.MakeSmsFrame((char*)&bytResType, 1, 0x38, 0x45, buf, sizeof(buf), iBufLen);
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9 );
	
	return ( bytResType == 1 ? true : false);
}

// 数据包是去掉CFG_GPRS封装和SMS封装的,输入的手机号是一个15字节长度非NULL结束符的字符串，
void CSms::_AnalyzeSmsFrame( char* v_szSrcHandtel, char* v_szDesHandtel, BYTE v_bytTranType,
							BYTE v_bytDataType, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
	char szCvt[ SOCK_MAXSIZE ];
	DWORD dwCvtLen = 0;
	
	switch( v_bytTranType )
	{
	case 0x06: // 远程遥控 （慎重处理，注意放在最前面,以免若放在后面时前无break会被误执行）
		switch( v_bytDataType )
		{
		case 0x01:
			g_objSpecCtrl.Deal0601( v_szSrcHandtel, v_szData, v_dwDataLen );
			break;
			
		default:
			;
		}
		break;

	case 0x01: // 监控/位置查询/报警处理
		{
			switch( v_bytDataType )
			{
			case 0x01: // 监控请求
				g_objMonAlert.Deal0101( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
				break;
				
			case 0x02: // 撤销监控请求
				g_objMonAlert.Deal0102( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
				break;
				
			case 0x03: // 修改监控参数
				g_objMonAlert.Deal0103( v_szSrcHandtel, v_szData, v_dwDataLen );
				break;
				
			case 0x04: // 终止监控应答
				break;
				
			case 0x05: // 报警处理指示
				g_objMonAlert.Deal0105( v_szSrcHandtel, v_szData, v_dwDataLen );
				break;
				
			case 0x11: // 位置查询
				g_objMonAlert.Deal0111( v_szSrcHandtel, v_bFromSM );
				break;
				
#if USE_METERTYPE == 1				
			case 0x19:	// 载客不打表应答
				g_objMeter.Deal0119( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
				break;
#endif

			case 0x36: // 区域查车
				g_objMonAlert.Deal0136( v_szSrcHandtel, v_szData, v_dwDataLen );
				break;
				
#if USE_DRVREC == 1
			case 0x17:	// 实时事故疑点上传应答 
			case 0x27:	// 非正式疑点上传请求 
				{
					// 将接收到的数据压入g_drvrcd的tcprcv队列中
					byte tmp[2048] = {0};
					tmp[0] = v_bytTranType;
					tmp[1] = v_bytDataType;
					memcpy(tmp+2, v_szData, min(2046, v_dwDataLen));
					g_objDriveRecord.tcprcv.push(tmp, min(2046, v_dwDataLen)+2);
				}
				break;
#endif
				
			default:
				;
			}
		}
		break;

	case 0x03: // 求助业务
		switch( v_bytDataType )
		{
		case 0x01: // 求助应答
			g_objHelp.Deal0301( v_szData, v_dwDataLen );
			break;
			
		default:
			;
		}
		break;

	case 0x05: // 调度电召
		switch( v_bytDataType )
		{
		case 0x01: // 中心调度指示
			g_objDiaodu.Deal0501( v_szData, v_dwDataLen );
			break;
			
		//zzg add
		case 0x10:
			g_objDiaodu.Deal0510( v_szData, v_dwDataLen );
			break;
		case 0x11:
			g_objDiaodu.Deal0511( v_szData, v_dwDataLen );
			break;
		
		case 0x3c:	// 电召指令(简短电召指令)
			{
				dwCvtLen = DWORD( Code6To8(v_szData, szCvt, int(v_dwDataLen), sizeof(szCvt)) );
				if( dwCvtLen > v_dwDataLen || dwCvtLen > sizeof(szCvt) ) break;
				PRTMSG(MSG_DBG, "Deal 053c dianzhao!\n");
				g_objDiaodu.Deal053c( szCvt, dwCvtLen );
			}
			break;
			
		case 0x3d:	// 抢答确认(详细电召指令)
			{
				dwCvtLen = DWORD( Code6To8(v_szData, szCvt, int(v_dwDataLen), sizeof(szCvt)) );
				if( dwCvtLen > v_dwDataLen || dwCvtLen > sizeof(szCvt) ) break;
				
				g_objDiaodu.Deal053d( szCvt, dwCvtLen );
			}
			break;
			
		default:
			;
		}
		break;

	case 0x07: // 监听业务
		switch( v_bytDataType )
		{
		case 0x20: // 监听请求
			g_objMonAlert.Deal0720( v_szSrcHandtel, v_szData, v_dwDataLen );
			break;
				
		case 0x23: // 监听请求
			g_objMonAlert.Deal0723( v_szSrcHandtel, v_szData, v_dwDataLen );
			break;
				
		default:
			;
		}
		break;

	case 0x0e: // 黑匣子查询
		switch( v_bytDataType )
		{
#if QIANTYPE_USE == QIANTYPE_JG2 || QIANTYPE_USE == QIANTYPE_V7
		case 0x01: // 求助号码设置 (怪了怎么在这里)
			g_objHelp.Deal0e01( v_szSrcHandtel, v_szData, v_dwDataLen );
			break;
#endif
#if USE_BLK == 1
		case 0x10: // 千里眼公开版_研一
		case 0x20: // 黑匣子查询请求
			g_objBlk.Deal0e20( v_szData, v_dwDataLen );
			break;
			
		case 0x21: // 停止黑匣子查询请求
			g_objBlk.Deal0e21( v_szData, v_dwDataLen );
			break;
#endif
		default:
			;
		}
		break;

#if USE_LEDTYPE == 1
	case 0x39:
		g_objLedBohai.Deal0x39(v_bytDataType, v_szData, v_dwDataLen);
		break;
#endif

#if USE_LEDTYPE == 2
	case 0x42:
		g_objLedChuangLi.Deal0x42(v_bytDataType, v_szData, v_dwDataLen);
		break;
#endif

	case 0x10: // 中心设置
		switch( v_bytDataType )
		{
		case 0x01: // 短信中心号
			g_objCfg.Deal1001( v_szSrcHandtel, v_szData, v_dwDataLen, v_szDesHandtel, v_bFromSM );
			break;
			
		case 0x02: // 设置报警、调度、税控手机号
			g_objCfg.Deal1002( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
			
		case 0x03: // 设置监听电话
			g_objCfg.Deal1003( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
			
#if USE_REALPOS == 1
		case 0x07: // 设置实时定位
			g_objRealPos.Deal1007( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
#endif
			
#if USE_TELLIMIT == 0
		case 0x08: // 设置呼叫限制
			g_objCfg.Deal1008( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
#endif
			
		case 0x09: // 报警设置
			g_objMonAlert.Deal1009( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
			
		case 0x0b: // 求助号码设置
			g_objHelp.Deal100b( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
			
		case 0x11: // 设置区域号
			g_objCfg.Deal1011( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
			
#if USE_BLK == 1
		case 0x12: // 黑匣子间隔设置
			g_objBlk.Deal1012( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
#endif

#if USE_METERTYPE == 1
		case 0x15: // 载客不打表
			g_objMeter.Deal1015( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
#endif

		case 0x1a: // 状态查询
			g_objMonAlert.Deal101a( v_szSrcHandtel, v_szData, v_dwDataLen );
			break;
			
		case 0x3b: // 设置CFG_GPRS初始参数
			g_objCfg.Deal103b( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
			
		case 0x3c: // 设置IP地址
			g_objCfg.Deal103c( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
			
		case 0x3d: // 超速报警设置
			g_objMonAlert.Deal103d( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
			
#if 2 == USE_PROTOCOL
		case 0x3e: // 恢复缺省参数
			g_objCfg.Deal103e( v_szSrcHandtel, v_szData, v_dwDataLen, v_bFromSM );
			break;
			
		case 0x3f: // 设置短信中心号和用户识别码
			g_objCfg.Deal103f( v_szSrcHandtel, v_szData, v_dwDataLen, v_szDesHandtel, v_bFromSM );
			break;
#endif
			
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 || USE_DRVRECRPT == 11
		case 0x3e: // 设置主动上传行驶参数
			g_objDrvRecRpt.Deal103e(v_szSrcHandtel, v_szData, v_dwDataLen );
			break;
#endif
#endif
			
		default:
			break;
		}
		break;

#if USE_TELLIMIT == 1
		case 0x33:	// 呼叫限制业务
			{
				g_objPhoneBook.hdl_cent(v_bytDataType, v_szData, v_dwDataLen);	
			}
			break;		
#endif

#if USE_TELLIMIT == 2
		case 0x33: // 呼叫处理业务
			switch( v_bytDataType )
			{
			case 0x01:
				g_objCfg.Deal3301( v_szSrcHandtel, v_szData, v_dwDataLen );
				break;
				
			case 0x02:
				g_objCfg.Deal3302( v_szSrcHandtel, v_szData, v_dwDataLen );
				break;
				
			case 0x04:
				g_objCfg.Deal3304( v_szSrcHandtel, v_szData, v_dwDataLen );
				break;
				
			default:
				;
			}
			break;
#endif
			
#if 3 == USE_TELLIMIT
			case 0x33:
				switch( v_bytDataType )
				{
				case 0x01:
					g_objCfg.Deal3301( v_szSrcHandtel, v_szData, v_dwDataLen );
					break;
					
				case 0x02:
					g_objCfg.Deal3302( v_szSrcHandtel, v_szData, v_dwDataLen );
					break;
					
				default:
					;
				}
				break;
#endif

#if USE_DRVREC == 1
	case 0x09:	// 里程统计业务 
	case 0x36:	// 行车记录仪业务 
	case 0x37:	// 行车记录仪报警业务
	case 0x0f:	// 越界报警参数设置业务
		{
			// 将接收到的数据压入g_drvrcd的tcprcv队列中
			byte tmp[2048] = {0};
			tmp[0] = v_bytTranType;
			tmp[1] = v_bytDataType;
			memcpy(tmp+2, v_szData, min(2046, v_dwDataLen));

			g_objDriveRecord.tcprcv.push(tmp, min(2046, v_dwDataLen)+2);
		}
		break;
#endif

#if USE_DRVREC == 0
	case 0x36: // 行驶记录仪业务1
		{
			dwCvtLen = DWORD( Code7To8(v_szData, szCvt, int(v_dwDataLen), sizeof(szCvt)) );
			if( dwCvtLen > v_dwDataLen || dwCvtLen > sizeof(szCvt) ) break;
			
			switch( v_bytDataType )
			{
#if USE_DRIVERCTRL == 1
			case 0x02: // 驾驶员身份设置请求
				g_objDriverCtrl.Deal3602( v_szSrcHandtel, szCvt, dwCvtLen );
				break;
#endif
				
#if USE_ACDENT == 1 || USE_ACDENT == 2
			case 0x04: // 事故疑点查询请求
				g_objAcdent.Deal3604( v_szSrcHandtel, szCvt, dwCvtLen );
				break;
#endif
				
			default:
				;
			}
		}
		break;
#endif

	case 0x38:	// DVR相关业务，发送到DvrExe进行处理
		{
			if( 0x05 == v_bytDataType )
			{
				_Deal3805(v_szData, v_dwDataLen);
			}
			else
			{
				char buf[1024] = {0};
				buf[0] = 0x02;		// 0x02 表示QianExe向DvrExe转发的中心指令
				buf[1] = v_bytDataType;
				memcpy(buf+2, v_szData, v_dwDataLen);
				DataPush(buf, v_dwDataLen+2, DEV_QIAN, DEV_DVR, LV2);
			}			
		}
		break;

	case 0x20:	// 税控协议
		{
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
			// 将接收到的数据压入g_meter的tcprcv队列中
			byte tmp[2048] = {0};
			tmp[0] = v_bytDataType;
			memcpy(tmp+1, v_szData, min(2047, v_dwDataLen));
#if USE_METERTYPE == 1
			g_objMeter.tcprcv.push(tmp, min(2047, v_dwDataLen)+1);
#endif

#if USE_METERTYPE == 2
			g_objKTMeter.tcprcv.push(tmp,min(2047, v_dwDataLen)+1);
#endif
		}
		break;

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	case 0x41:	// 拍照业务
		{
			switch(v_bytDataType)
			{
			case 0x00:
				g_objPhoto.Deal4100(v_szData, v_dwDataLen);
				break;

			case 0x01:	// 中心抓拍请求
				g_objPhoto.Deal4101(v_szData, v_dwDataLen);
				break;

			case 0x02:	// 设置抓拍参数请求
				g_objPhoto.Deal4102(v_szData, v_dwDataLen);
				break;

			case 0x03:	// 通过中心报表查询图片黑匣子
				g_objPhoto.Deal4103(v_szData, v_dwDataLen);
				break;

			case 0x04:	// 通过时间段和事件查询黑匣子照片请求
				g_objPhoto.Deal4104(v_szData, v_dwDataLen);
				break;

			case 0x10:	// 图片数据传输应答
				g_objPhoto.Deal4110(v_szData, v_dwDataLen);
				break;

			case 0x12:	// 当前窗口超时重传应答
				g_objPhoto.Deal4112(v_szData, v_dwDataLen);
				break;

			case 0x13:	// 请求断点续传图片
				g_objPhoto.Deal4113(v_szData, v_dwDataLen);
				break;
			}
		}
		break;
#endif

    case 0x40://凯天司机管理相关
		{
			switch( v_bytDataType )
			{
				case 0x04:
					g_objKTDriverCtrl.Deal4004(v_szData, v_dwDataLen);
					break;
				case 0x05:
					g_objKTDriverCtrl.Deal4005(v_szData, v_dwDataLen);
					break;
				case 0x06:
					g_objKTDriverCtrl.Deal4006(v_szData, v_dwDataLen);
					break;
				case 0x08:
					g_objKTDriverCtrl.Deal4008(v_szData, v_dwDataLen);
					break;
				default:
					break;
			}
		}
		break;

#if USE_LEDTYPE == 3
	case 0x42://凯天LED顶灯相关
		{
			switch( v_bytDataType )
			{
				case 0x08:
					g_objDiaodu.Deal4208(v_szData, v_dwDataLen);
					break;
				default:
					break;
			}
		}
#endif

#if USE_AUTORPTSTATION == 1
	// 公交自动报站相关的业务
	case 0x42:
		{
			switch( v_bytDataType )
			{
			case 0x16:
				g_objAuoRptStationNew.hdl_4216((byte*)v_szData,v_dwDataLen);
				break;
				
			case 0x17:
				g_objAuoRptStationNew.hdl_4217((byte*)v_szData,v_dwDataLen);
				break;
				
			case 0x18:
				g_objAuoRptStationNew.hdl_4218();
				break;
				
			case 0x19:
				g_objAuoRptStationNew.hdl_4219((byte*)v_szData,v_dwDataLen);
				break;
				
			case 0x20:
				g_objAuoRptStationNew.hdl_4220((byte*)v_szData,v_dwDataLen);
				break;
				
			default:
				break;
			}
		}
		break;
				
	case 0x43:
		{
			switch( v_bytDataType )
			{
			case 0x12:
				g_objDownLine.Deal4312(v_szData,v_dwDataLen);
				break;

			case 0x13:
				g_objDownLine.Deal4313(v_szData,v_dwDataLen);
				break;

			case 0x10:
				g_objDownLine.Deal4310(v_szData,v_dwDataLen);
				break;

			default:
				break;
			}
		}
		break;
#endif

	default:
		break;
	}
}


