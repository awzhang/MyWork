#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-Config   ")

// 移除末尾的空格
void StrRemvBlank( char* v_szBuf, size_t v_sizBuf )
{
	if( v_sizBuf < 1 ) return;
	if( 0x20 != v_szBuf[v_sizBuf - 1] && 0 != v_szBuf[v_sizBuf - 1] ) return;
	v_szBuf[v_sizBuf - 1] = 0;
	char* pBlank = strchr( v_szBuf, 0x20 );
	if( pBlank ) *pBlank = 0;
}

CConfig::CConfig()
{

}

CConfig::~CConfig()
{

}

int CConfig::GetSelfTel(char *v_szBuf, size_t v_sizTel )
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

int CConfig::GetSmsTel( char* v_szTel, size_t v_sizTel )
{
	tag2QServCodeCfg objServCodeCfg;
	
	int iRet;
	iRet = GetSecCfg( &objServCodeCfg, sizeof(objServCodeCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfg) );
	
	if( !iRet )
	{
		if( v_sizTel < sizeof(objServCodeCfg.m_szQianSmsTel) ) return ERR_SPACELACK;
		memset( v_szTel, 0, v_sizTel );
		memcpy( v_szTel, objServCodeCfg.m_szQianSmsTel, sizeof(objServCodeCfg.m_szQianSmsTel) );
		v_szTel[ v_sizTel - 1 ] = 0;
		char* pFind = strchr( v_szTel, 0x20 );
		if( pFind ) *pFind = 0;
	}
	else
	{
		memset( v_szTel, 0, v_sizTel );
	}
	return iRet;
}

// 设置短信中心号 (应答成功优先) (多包含了帧头(35))
int CConfig::Deal1001(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen, char* v_szDesHandtel, bool v_bFromSM )
{
// 短信中心号码(22)

	tag1001 req;
	tag1PComuCfg obj1PComuCfg, obj1PComuCfgBkp;
	tag2QServCodeCfg obj2QServCodeCfg;
	char* pBlank = NULL;
	int iRet = 0;
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	BYTE bytResType = 0;

	// 数据参数检查
	if( sizeof(req) + QIANDOWNFRAME_BASELEN - 1 != v_dwDataLen )
	{
		iRet = ERR_PAR;
		PRTMSG(MSG_ERR, "Deal1001 fail 1!, sizeof(tag1001) = %d\n", sizeof(tag1001));
		goto DEAL1001_FAILED;
	}

	// 提取参数
	memcpy( &req, v_szData + QIANDOWNFRAME_BASELEN - 1, sizeof(req) );

	// 先读取,再修改

	iRet = GetImpCfg( &obj1PComuCfg, sizeof(obj1PComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
	if( iRet ) 
		goto DEAL1001_FAILED;

	obj1PComuCfgBkp = obj1PComuCfg; // 先备份

	iRet = GetSecCfg( &obj2QServCodeCfg, sizeof(obj2QServCodeCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(obj2QServCodeCfg) );
	if( iRet ) 
		goto DEAL1001_FAILED;

	memcpy( obj1PComuCfg.m_szSmsCentID, req.m_szSmsCentID, sizeof(obj1PComuCfg.m_szSmsCentID) );
	StrRemvBlank( obj1PComuCfg.m_szSmsCentID, sizeof(obj1PComuCfg.m_szSmsCentID) );

	// 附带修改本机手机号
	memcpy( obj1PComuCfg.m_szTel, v_szDesHandtel, sizeof(obj1PComuCfg.m_szTel) );
	obj1PComuCfg.m_szTel[ sizeof(obj1PComuCfg.m_szTel) - 1 ] = 0;
	pBlank = strchr( obj1PComuCfg.m_szTel, 0x20 );
	if( pBlank ) *pBlank = 0;

	// 附带修改中心ID (即区域号)
#if USE_PROTOCOL == 0 || USE_PROTOCOL == 2
	obj2QServCodeCfg.m_szAreaCode[0] = v_szData[0];
	obj2QServCodeCfg.m_szAreaCode[1] = v_szData[1];
#endif
#if USE_PROTOCOL == 1
	obj2QServCodeCfg.m_szAreaCode[0] = v_szData[2];
	obj2QServCodeCfg.m_szAreaCode[1] = v_szData[3];
#endif

	if( 0x7f == obj2QServCodeCfg.m_szAreaCode[1] ) obj2QServCodeCfg.m_szAreaCode[1] = 0; // 保存时7f还原为0
	
	// 写回和发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x41, buf, sizeof(buf), iBufLen ); 
	if( !iRet )
	{
		// 保存新设置 (附带保存手机号)
		if( iRet = SetImpCfg(&obj1PComuCfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg)) )
		{
			goto DEAL1001_FAILED;
		}

		// 发送
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_szSrcHandtel );
		if( !iRet )
		{
			SetSecCfg( &obj2QServCodeCfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(obj2QServCodeCfg) ); // 附带写入区域号

			// 通知底层立刻生效新的短信中心号
			char buf[] = {0x01, 0x36};
			DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		}
		else // 否则(即发送失败)
		{
			SetImpCfg( &obj1PComuCfgBkp, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfgBkp) ); // 恢复原设置 ((附带恢复手机号))
			goto DEAL1001_FAILED;
		}
	}
	else goto DEAL1001_FAILED;

	return 0;

DEAL1001_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x41, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_szSrcHandtel );

	return iRet;
}

/// 设置报警、调度、税控手机号 (应答成功优先)
int CConfig::Deal1002(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
	tag2QServCodeCfg obj2QServCodeCfg, obj2QServCodeCfgBkp;
	tag1002 req;
	int iRet = 0;
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	BYTE bytResType = 0;

	// 数据参数检查
	if( 15 > v_dwDataLen )
	//if( sizeof(req) != v_dwDataLen )
	{
		iRet = ERR_PAR;
		goto DEAL1002_FAILED;
	}
	memcpy( &req, v_szData, sizeof(req) );

	// 读取->修改
	iRet = GetSecCfg( &obj2QServCodeCfg, sizeof(obj2QServCodeCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(obj2QServCodeCfg) );
	if( iRet )
		goto DEAL1002_FAILED;
	obj2QServCodeCfgBkp = obj2QServCodeCfg; // 先备份

	memcpy( obj2QServCodeCfg.m_szQianSmsTel, req.m_szSmsTel, sizeof(obj2QServCodeCfg.m_szQianSmsTel) );
	StrRemvBlank( obj2QServCodeCfg.m_szQianSmsTel, sizeof(obj2QServCodeCfg.m_szQianSmsTel) );
	
	if (v_dwDataLen>20) 
	{
		memcpy( obj2QServCodeCfg.m_szTaxHandtel, req.m_szTaxHandtel, sizeof(obj2QServCodeCfg.m_szTaxHandtel) );
	}
	if (v_dwDataLen>35) 
	{
		memcpy( obj2QServCodeCfg.m_szDiaoduHandtel, req.m_szDiaoduHandtel, sizeof(obj2QServCodeCfg.m_szDiaoduHandtel) );
	}

	// 写回和发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x42, buf, sizeof(buf), iBufLen); // 组帧
	if( !iRet )
	{
		// 保存新设置
		if( iRet = ::SetSecCfg(&obj2QServCodeCfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(obj2QServCodeCfg)) )
		{
			goto DEAL1002_FAILED;
		}

		// 发送
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );
		if( iRet ) // 若发送失败
		{
			SetSecCfg( &obj2QServCodeCfgBkp, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg),	sizeof(obj2QServCodeCfgBkp) ); // 恢复原设置
			goto DEAL1002_FAILED;
		}
	}
	else goto DEAL1002_FAILED;

	return 0;

DEAL1002_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x42, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );

	return iRet;
}

/// 设置监听电话: 号码个数(1) + 号码(15 * N)
int CConfig::Deal1003(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
	tag2QServCodeCfg obj2QLsnCfg, obj2QLsnCfgBkp;
	int iRet = 0;
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	BYTE bytResType = 0;
	BYTE bytTelCount = 0;
	BYTE byt = 0;

	// 数据参数检查
	if( v_dwDataLen < 16 )
	{
		iRet = ERR_PAR;
		goto DEAL1003_FAILED;
	}
	bytTelCount = v_szData[0];
	if( 0 == bytTelCount || bytTelCount > sizeof(obj2QLsnCfg.m_aryLsnHandtel) / sizeof(obj2QLsnCfg.m_aryLsnHandtel[0]) )
	{
		iRet = ERR_PAR;
		goto DEAL1003_FAILED;
	}
	if( 1 + sizeof(obj2QLsnCfg.m_aryLsnHandtel[0]) * bytTelCount != v_dwDataLen )
	{
		iRet = ERR_PAR;
		goto DEAL1003_FAILED;
	}

	// 读取->修改
	iRet = GetSecCfg( &obj2QLsnCfg, sizeof(obj2QLsnCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(obj2QLsnCfg) );
	if( iRet )
		goto DEAL1003_FAILED;

	obj2QLsnCfgBkp = obj2QLsnCfg; // 先备份

	for( byt = 0; byt < bytTelCount; byt ++ )
		memcpy( obj2QLsnCfg.m_aryLsnHandtel + byt, v_szData + 1 + byt * 15, sizeof(obj2QLsnCfg.m_aryLsnHandtel[0]) );
	obj2QLsnCfg.m_bytListenTelCount = bytTelCount;

	// 发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x43, buf, sizeof(buf), iBufLen );
	if( !iRet )
	{
		// 写入文件
		iRet = SetSecCfg( &obj2QLsnCfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(obj2QLsnCfg) );		
		if( iRet )
			goto DEAL1003_FAILED;

		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );
		if( iRet )
		{
			SetSecCfg( &obj2QLsnCfgBkp, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(obj2QLsnCfgBkp) );
			goto DEAL1003_FAILED;
		}
	}

	return 0;

DEAL1003_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x43, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );

	return iRet;
}

// 设置呼叫限制: 设置控制字节（1）+ 电话个数（1）+ [ 电话号码属性（1）+ 电话号码串 ]  x  N
int CConfig::Deal1008( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
#if USE_TELLIMIT == 0
	tag2QFbidTelCfg obj2QFbidTelCfg, obj2QFbidTelCfgBkp;
	int iRet = 0;
	int iBufLen = 0;
	DWORD dw = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytResType = 0;
	BYTE bytSetCtrl = 0;
	BYTE bytTelCount = 0;
	BYTE bytTelLen = 0;
	BYTE byt = 0;

	// 数据参数检查
	if( v_dwDataLen < 2 )
	{
		iRet = ERR_PAR;
		goto DEAL1008_FAILED;
	}

	// 读取原始配置
	iRet = GetSecCfg( &obj2QFbidTelCfg, sizeof(obj2QFbidTelCfg), offsetof(tagSecondCfg, m_uni2QFbidTelCfg.m_obj2QFbidTelCfg), sizeof(obj2QFbidTelCfg) );
	if( iRet )
		goto DEAL1008_FAILED;

	obj2QFbidTelCfgBkp = obj2QFbidTelCfg; // 先备份

	// 从请求帧获取基本信息
	bytSetCtrl = BYTE( v_szData[0] );
	bytTelCount = BYTE( v_szData[1] );
	if( 0x7f == bytTelCount ) bytTelCount = 0;

	// 根据基本信息再检查
	if( bytTelCount > 16 )
	{
		iRet = ERR_PAR;
		goto DEAL1008_FAILED;
	}

	// 判断是否要对原设置进行清除
	if( 0x20 & bytSetCtrl ) // 若要清除呼出设置
	{
		memset( obj2QFbidTelCfg.m_aryFbidOutHandtel, 0, sizeof(obj2QFbidTelCfg.m_aryFbidOutHandtel) );
		memset( obj2QFbidTelCfg.m_aryAllowOutHandtel, 0, sizeof(obj2QFbidTelCfg.m_aryAllowOutHandtel) );
		obj2QFbidTelCfg.m_bytFbidOutTelCount = 0, obj2QFbidTelCfg.m_bytAllowOutTelCount = 0;
	}
	if( 0x10 & bytSetCtrl ) // 若要清除呼入设置
	{
		memset( obj2QFbidTelCfg.m_aryFbidInHandtel, 0, sizeof(obj2QFbidTelCfg.m_aryFbidInHandtel) );
		memset( obj2QFbidTelCfg.m_aryAllowInHandtel, 0, sizeof(obj2QFbidTelCfg.m_aryAllowInHandtel) );
		obj2QFbidTelCfg.m_bytFbidInTelCount = 0, obj2QFbidTelCfg.m_bytAllowInTelCount = 0;
	}

	// 更新配置
	dw = 2;
	for( byt = 0; byt < bytTelCount; byt ++ )
	{
		// 参数长度检查
		if( dw >= v_dwDataLen )
		{
			iRet = ERR_PAR;
			goto DEAL1008_FAILED;
		}

		// 获取号码长度
		bytTelLen = BYTE(v_szData[dw] & 0x0f);

		// 参数长度检查
		if( dw + 1 + bytTelLen > v_dwDataLen || bytTelLen > 15 || bytTelLen < 1 )
		{
			iRet = ERR_PAR;
			goto DEAL1008_FAILED;
		}

		// 记录该号码
		switch( v_szData[dw] & 0x30 )
		{
		case 0: // 允许呼出
			if( obj2QFbidTelCfg.m_bytAllowOutTelCount < sizeof(obj2QFbidTelCfg.m_aryAllowOutHandtel) / sizeof(obj2QFbidTelCfg.m_aryAllowOutHandtel[0]) )
			{
				memset( obj2QFbidTelCfg.m_aryAllowOutHandtel + obj2QFbidTelCfg.m_bytAllowOutTelCount,
					0x20, sizeof(obj2QFbidTelCfg.m_aryAllowOutHandtel[0]) );
				memcpy( obj2QFbidTelCfg.m_aryAllowOutHandtel + obj2QFbidTelCfg.m_bytAllowOutTelCount,
					v_szData + dw + 1, bytTelLen );
				obj2QFbidTelCfg.m_bytAllowOutTelCount ++;
			}
			break;

		case 0x10: // 禁止呼出
			if( obj2QFbidTelCfg.m_bytFbidOutTelCount < sizeof(obj2QFbidTelCfg.m_aryFbidOutHandtel) / sizeof(obj2QFbidTelCfg.m_aryFbidOutHandtel[0]) )
			{
				memset( obj2QFbidTelCfg.m_aryFbidOutHandtel + obj2QFbidTelCfg.m_bytFbidOutTelCount,
					0x20, sizeof(obj2QFbidTelCfg.m_aryFbidOutHandtel[0]) );
				memcpy( obj2QFbidTelCfg.m_aryFbidOutHandtel + obj2QFbidTelCfg.m_bytFbidOutTelCount,
					v_szData + dw + 1, bytTelLen );
				obj2QFbidTelCfg.m_bytFbidOutTelCount ++;
			}
			break;

		case 0x20: // 允许呼入
			if( obj2QFbidTelCfg.m_bytAllowInTelCount < sizeof(obj2QFbidTelCfg.m_aryAllowInHandtel) / sizeof(obj2QFbidTelCfg.m_aryAllowInHandtel[0]) )
			{
				memset( obj2QFbidTelCfg.m_aryAllowInHandtel + obj2QFbidTelCfg.m_bytAllowInTelCount,
					0x20, sizeof(obj2QFbidTelCfg.m_aryAllowInHandtel[0]) );
				memcpy( obj2QFbidTelCfg.m_aryAllowInHandtel + obj2QFbidTelCfg.m_bytAllowInTelCount,
					v_szData + dw + 1, bytTelLen );
				obj2QFbidTelCfg.m_bytAllowInTelCount ++;
			}
			break;

		case 0x30: // 禁止呼入
			if( obj2QFbidTelCfg.m_bytFbidInTelCount < sizeof(obj2QFbidTelCfg.m_aryFbidInHandtel) / sizeof(obj2QFbidTelCfg.m_aryFbidInHandtel[0]) )
			{
				memset( obj2QFbidTelCfg.m_aryFbidInHandtel + obj2QFbidTelCfg.m_bytFbidInTelCount,
					0x20, sizeof(obj2QFbidTelCfg.m_aryFbidInHandtel[0]) );
				memcpy( obj2QFbidTelCfg.m_aryFbidInHandtel + obj2QFbidTelCfg.m_bytFbidInTelCount,
					v_szData + dw + 1, bytTelLen );
				obj2QFbidTelCfg.m_bytFbidInTelCount ++;
			}
			break;

		default:
			;
		}

		dw += 1 + DWORD(bytTelLen); // 准备处理下一号码
	}
	obj2QFbidTelCfg.m_bytFbidType = BYTE( (bytSetCtrl & 0x03) + (bytSetCtrl & 0x0c) * 4 ); // 呼叫限制类型

	// 发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x48, buf, sizeof(buf), iBufLen);
	if( !iRet )
	{
		// 写入文件
		if( iRet = SetSecCfg(&obj2QFbidTelCfg, offsetof(tagSecondCfg, m_uni2QFbidTelCfg.m_obj2QFbidTelCfg),	sizeof(obj2QFbidTelCfg)) )
		{
			goto DEAL1008_FAILED;
		}

		if( iRet = g_objSock.SOCKSNDSMSDATA(buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL) )
		{
			SetSecCfg( &obj2QFbidTelCfgBkp, offsetof(tagSecondCfg, m_uni2QFbidTelCfg.m_obj2QFbidTelCfg), sizeof(obj2QFbidTelCfgBkp) );
			goto DEAL1008_FAILED;
		}
	}
	else
		goto DEAL1008_FAILED;

	return 0;

DEAL1008_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x48, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );

	return iRet;

#else
	return 0;
#endif
}

/// 设置区域号 （应答成功优先）
int CConfig::Deal1011( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
	tag2QServCodeCfg objServCodeCfg, objServCodeCfgBkp;
	int iRet = 0;
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	BYTE bytResType = 0;

	// 数据参数检查
	if( 2 != v_dwDataLen )
	{
		iRet = ERR_PAR;
		goto DEAL1011_FAILED;
	}

	// 读取->修改
	iRet = GetSecCfg( &objServCodeCfg, sizeof(objServCodeCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfg) );
	if( iRet )
		goto DEAL1011_FAILED;

	objServCodeCfgBkp = objServCodeCfg; // 先备份

	memcpy( objServCodeCfg.m_szAreaCode, v_szData, sizeof(objServCodeCfg.m_szAreaCode) );
	
	// 保存时7f还原为0
	if( 0x7f == objServCodeCfg.m_szAreaCode[1] )
		objServCodeCfg.m_szAreaCode[1] = 0; 

	// 写回和发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x51, buf, sizeof(buf), iBufLen); 
	if( !iRet )
	{
		if( iRet = SetSecCfg(&objServCodeCfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfg)) )
		{
			goto DEAL1011_FAILED;
		}
		
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL ); // 发送
		if( iRet ) // 若发送失败
		{
			SetSecCfg( &objServCodeCfgBkp, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfgBkp) ); // 恢复原设置
			goto DEAL1011_FAILED;
		}
	}
	else goto DEAL1011_FAILED;

	return 0;

DEAL1011_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x51, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );

	return iRet;
}

// 设置GPRS初始参数 （应答成功优先）
int CConfig::Deal103b( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
// 千里眼通用版,公开版_研三
// 参数个数（1）+【参数类型1（1）+参数长度（1）+参数（n）】×N

// 千里眼公开版_研一
// 用户识别码（6）＋参数个数N（1）＋【参数类型（1）＋参数长度n（1）＋参数（n）】×N

	tag2QGprsCfg objGprsCfg, objGprsCfgBkp;
	int iRet = 0;
	int iBufLen = 0;
	DWORD dw = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytResType = 0;
	BYTE bytParTotal = 0;
	BYTE bytParLen = 0;
	BYTE byt = 0;
	bool bChgAutoRpt = false;

#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
	const DWORD BASELEN_103B = 1;
#endif
#if 2 == USE_PROTOCOL
	const DWORD BASELEN_103B = 7;
#endif

	PRTMSG(MSG_DBG, "Deal 103b\n");

	// 获取原始参数
	iRet = GetSecCfg( &objGprsCfg, sizeof(objGprsCfg), offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );
	if( iRet )
	{
		//RenewMemInfo(0x02, 0x00, 0x01, 0x00);
		goto DEAL103B_FAILED;
	}

	objGprsCfgBkp = objGprsCfg; // 先备份

#if 2 == USE_PROTOCOL
	// 比较识别码
	if( memcmp(v_szData, USERIDENCODE, strlen(USERIDENCODE)) )
	{
		PRTMSG(MSG_DBG, "Deal 103b failed 1!\n");
		iRet = ERR_PAR;
		goto DEAL103B_FAILED;
	}
#endif

	// 参数检查
	if( v_dwDataLen < BASELEN_103B )
	{
		PRTMSG(MSG_DBG, "Deal 103b failed 2!\n");
		iRet = ERR_PAR;
		//RenewMemInfo(0x02, 0x00, 0x01, 0x01);
		goto DEAL103B_FAILED;
	}
	bytParTotal = BYTE( v_szData[BASELEN_103B - 1] ); // 参数个数
	if( 0 == bytParTotal || bytParTotal > 4 )
	{
		PRTMSG(MSG_DBG, "Deal 103b failed 3!\n");
		iRet = ERR_PAR;
		//RenewMemInfo(0x02, 0x00, 0x01, 0x02);
		goto DEAL103B_FAILED;
	}

	// 解帧,更新设置
	dw = BASELEN_103B;
	for( byt = 0; byt < bytParTotal; byt ++ )
	{
		if( dw + 2 >= v_dwDataLen )
		{
			PRTMSG(MSG_DBG, "Deal 103b failed 4!\n");
			iRet = ERR_PAR;
			//RenewMemInfo(0x02, 0x00, 0x01, 0x03);
			goto DEAL103B_FAILED;
		}

		bytParLen = BYTE( v_szData[ dw + 1 ] );
		if( dw + 2 + bytParLen > v_dwDataLen )
		{
			PRTMSG(MSG_DBG, "Deal 103b failed 5!\n");
			iRet = ERR_PAR;
			//RenewMemInfo(0x02, 0x00, 0x01, 0x04);
			goto DEAL103B_FAILED;
		}

		switch( BYTE(v_szData[dw]) )
		{
		case 0x01: // CFG_GPRS在线方式
			{
				if( 1 != bytParLen )
				{
					PRTMSG(MSG_DBG, "Deal 103b failed 6!\n");
					iRet = ERR_PAR;
					//RenewMemInfo(0x02, 0x00, 0x01, 0x05);
					goto DEAL103B_FAILED;
				}
				BYTE bytOnLineType = BYTE( v_szData[dw + 2] );
				if( 0x7f == bytOnLineType ) bytOnLineType = 0;
				if( bytOnLineType > 3 )
				{
					PRTMSG(MSG_DBG, "Deal 103b failed 7!\n");
					iRet = ERR_PAR;
					//RenewMemInfo(0x02, 0x00, 0x01, 0x6);
					goto DEAL103B_FAILED;
				}
				objGprsCfg.m_bytGprsOnlineType = bytOnLineType;
			}
			break;

		case 0x02: // 上传GPS数据参数
			{
				// 千里眼通用版，公开版_研三
				// 传输数据标识（1）+ 状态个数（1）
				// + [ 状态标识（1）+监控业务类型（1）+ 监控时间（2）+ 监控周期（2）+ 定距距离（2） ]  x  N

				// 千里眼通用版，公开版_研一
				// 传输数据标识（1）+ 状态个数（1）+ 上传数据时间段（4）
				// + [ 状态标识（1）+监控业务类型（1）+ 监控时间（2）+ 监控周期（2）+ 定距距离（2） ]  x  N

#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
				const DWORD GPRSRPTPAR_BASELEN = 2;
#endif
#if 2 == USE_PROTOCOL
				const DWORD GPRSRPTPAR_BASELEN = 6;
#endif

				// 什么时候有新的上传GPS设置,就什么时候清除旧的GPS上传设置 
				// 且因为该设置只会在1帧里完成,所以只要有新设置,就可以先清除旧的,以便后面逐步设定新值
				memset( objGprsCfg.m_aryGpsUpPar, 0, sizeof(objGprsCfg.m_aryGpsUpPar) );
				objGprsCfg.m_bytGpsUpParCount = 0;
				objGprsCfg.m_bytGpsUpType = 0;
				
				if( bytParLen <= GPRSRPTPAR_BASELEN )
				{
					PRTMSG(MSG_DBG, "Deal 103b failed 8!\n");
					iRet = ERR_PAR;
					//RenewMemInfo(0x02, 0x00, 0x01, 0x07);
					goto DEAL103B_FAILED;
				}
				
				// 传输数据标志
				objGprsCfg.m_bytGpsUpType = BYTE( v_szData[dw + 2] );
				
				// 本帧中包含的状态个数
				BYTE bytStaCount = BYTE( v_szData[dw + 3] );
				if( 0 == bytStaCount || bytStaCount > 4 )
				{
					PRTMSG(MSG_DBG, "Deal 103b failed 9!\n");
					iRet = ERR_PAR;
					//RenewMemInfo(0x02, 0x00, 0x01, 0x08);
					goto DEAL103B_FAILED;
				}
				if( GPRSRPTPAR_BASELEN + 8 * bytStaCount != bytParLen )
				{
					PRTMSG(MSG_DBG, "Deal 103b failed 10!\n");
					iRet = ERR_PAR;
					//RenewMemInfo(0x02, 0x00, 0x01, 0x09);
					goto DEAL103B_FAILED;
				}

#if 2 == USE_PROTOCOL
				// 起止时间段
				objGprsCfg.m_bytRptBeginHour = BYTE( v_szData[dw + 4] );
				objGprsCfg.m_bytRptBeginMin = BYTE( v_szData[dw + 5] );
				objGprsCfg.m_bytRptEndHour = BYTE( v_szData[dw + 6] );
				objGprsCfg.m_bytRptEndMin = BYTE( v_szData[dw + 7] );
				if( 0x7f == objGprsCfg.m_bytRptBeginHour )
				{
					objGprsCfg.m_bytRptBeginHour = 0;
				}
				if( 0x7f == objGprsCfg.m_bytRptBeginMin )
				{
					objGprsCfg.m_bytRptBeginMin = 0;
				}
				if( 0x7f == objGprsCfg.m_bytRptEndHour )
				{
					objGprsCfg.m_bytRptEndHour = 0;
				}
				if( 0x7f == objGprsCfg.m_bytRptEndMin )
				{
					objGprsCfg.m_bytRptEndMin = 0;
				}
				if( objGprsCfg.m_bytRptBeginHour > 23 )
				{
					PRTMSG(MSG_DBG, "Deal 103b failed 11!\n");
					iRet = ERR_PAR;
					goto DEAL103B_FAILED;
				}
				if( objGprsCfg.m_bytRptBeginMin > 59 )
				{
					PRTMSG(MSG_DBG, "Deal 103b failed 12!\n");
					iRet = ERR_PAR;
					goto DEAL103B_FAILED;
				}
				if( objGprsCfg.m_bytRptEndHour > 23 )
				{
					PRTMSG(MSG_DBG, "Deal 103b failed 13!\n");
					iRet = ERR_PAR;
					goto DEAL103B_FAILED;
				}
				if( objGprsCfg.m_bytRptEndMin > 59 )
				{
					PRTMSG(MSG_DBG, "Deal 103b failed 14!\n");
					iRet = ERR_PAR;
					goto DEAL103B_FAILED;
				}
				if( objGprsCfg.m_bytRptEndHour < objGprsCfg.m_bytRptBeginHour )
				{
					iRet = ERR_PAR;
					PRTMSG(MSG_DBG, "Deal 103b failed 15!\n");
					goto DEAL103B_FAILED;
				}
				if( objGprsCfg.m_bytRptBeginHour == objGprsCfg.m_bytRptEndHour 
					&&
					objGprsCfg.m_bytRptEndMin < objGprsCfg.m_bytRptBeginMin )
				{
					iRet = ERR_PAR;
					PRTMSG(MSG_DBG, "Deal 103b failed 16!\n");
					goto DEAL103B_FAILED;
				}
#endif

				tag103e obj103e; // 转发到行驶记录模块的模拟设置帧
				BYTE byt103eSymb = 0; // 模拟帧标志

				// 各个状态的设置
				DWORD dwPos = dw + 2 + GPRSRPTPAR_BASELEN;
				BYTE bytMaxCount = sizeof(objGprsCfg.m_aryGpsUpPar) / sizeof(objGprsCfg.m_aryGpsUpPar[0]);
				BYTE bytParNo = objGprsCfg.m_bytGpsUpParCount;
				for( byt = 0; byt < bytStaCount && bytParNo < bytMaxCount; byt ++ )
				{
					// 状态标识
					objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_bytStaType = BYTE(v_szData[dwPos]);

					// 监控业务类型
					objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_bytMonType = BYTE( v_szData[dwPos + 1] );

					// 监控时间
					if( 0x7f == v_szData[dwPos + 2] ) v_szData[dwPos + 2] = 0;
					if( 0x7f == v_szData[dwPos + 3] ) v_szData[dwPos + 3] = 0;
					if( BYTE(v_szData[dwPos + 2]) > 126 || BYTE(v_szData[dwPos + 3]) > 59 )
					{
						PRTMSG(MSG_DBG, "Deal 103b failed 17!\n");
						iRet = ERR_PAR;
						//RenewMemInfo(0x02, 0x00, 0x01, 0x0a);
						goto DEAL103B_FAILED;
					}
					objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonTime
						= BYTE(v_szData[dwPos + 2]) * WORD(60) + BYTE(v_szData[dwPos + 3]);
					if( 0 == objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonTime )
						objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonTime = 0xffff; // 永久监控

					BYTE bytMonSimpType = 0x18 & objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_bytMonType; // 监控方式

#if USE_DRVREC == 1	
					// 定时间隔
					if( 0 == bytMonSimpType || 0x10 == bytMonSimpType )
					{
						if( 0x7f == v_szData[dwPos + 4] ) v_szData[dwPos + 4] = 0;
						if( 0x7f == v_szData[dwPos + 5] ) v_szData[dwPos + 5] = 0;
						if( BYTE(v_szData[dwPos + 4]) > 126 || BYTE(v_szData[dwPos + 5]) > 59 )
						{
							PRTMSG(MSG_DBG, "Deal 103b failed 18!\n");
							iRet = ERR_PAR;
							//RenewMemInfo(0x02, 0x00, 0x01, 0x0b);
							goto DEAL103B_FAILED;
						}
						objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonInterval
							= BYTE(v_szData[dwPos + 4]) * WORD(60) + BYTE(v_szData[dwPos + 5]);
					}

					// 当监控方式为定时监控时，作为行驶记录上传
					if( 0 == bytMonSimpType ) // 当使用JG2的行驶记录功能时，定时条件的上报设置帧作为行驶记录上传的设置
					{
						if( 0x7f == v_szData[dwPos + 6] ) v_szData[dwPos + 6] = 0;
						if( 0x7f == v_szData[dwPos + 7] ) v_szData[dwPos + 7] = 0;
						if( BYTE(v_szData[dwPos + 6]) > 126 || BYTE(v_szData[dwPos + 7]) > 99 )
						{
							PRTMSG(MSG_DBG, "Deal 103b failed 19!\n");
							iRet = ERR_PAR;
							//RenewMemInfo(0x02, 0x00, 0x01, 0x0c);
							goto DEAL103B_FAILED;
						}
						objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonSpace
							= BYTE(v_szData[dwPos + 6]) * WORD(100) + BYTE(v_szData[dwPos + 7]);
					}
#else
					// 含定时条件时，处理定时间隔
					if( 0 == bytMonSimpType || 0x10 == bytMonSimpType )
					{
						if( 0x7f == v_szData[dwPos + 4] ) v_szData[dwPos + 4] = 0;
						if( 0x7f == v_szData[dwPos + 5] ) v_szData[dwPos + 5] = 0;
						if( BYTE(v_szData[dwPos + 4]) > 126 || BYTE(v_szData[dwPos + 5]) > 59 )
						{
							PRTMSG(MSG_DBG, "Deal 103b failed 20!\n");
							iRet = ERR_PAR;
							goto DEAL103B_FAILED;
						}
						objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonInterval
							= BYTE(v_szData[dwPos + 4]) * WORD(60) + BYTE(v_szData[dwPos + 5]);
						if( 0 == objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonInterval )
						{
							// 保证监控间隔至少为1秒

							//iRet = ERR_PAR;
							//goto DEAL103B_FAILED;

							objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonInterval = 1;
						}
					}
					
					// 含定距条件，处理定距间隔
					if( 0x08 == bytMonSimpType || 0x10 == bytMonSimpType )
					{
						if( 0x7f == v_szData[dwPos + 6] ) v_szData[dwPos + 6] = 0;
						if( 0x7f == v_szData[dwPos + 7] ) v_szData[dwPos + 7] = 0;
						if( BYTE(v_szData[dwPos + 6]) > 126 || BYTE(v_szData[dwPos + 7]) > 99 )
						{
							PRTMSG(MSG_DBG, "Deal 103b failed 21!\n");
							iRet = ERR_PAR;
							goto DEAL103B_FAILED;
						}
						objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonSpace
							= BYTE(v_szData[dwPos + 6]) * WORD(100) + BYTE(v_szData[dwPos + 7]);
						if( 0 == objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonSpace )
						{
							// 保证监控定距至少为10米

							//iRet = ERR_PAR;
							//goto DEAL103B_FAILED;

							objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonSpace = 1;
						}
					}
#endif

					if( 0x18 == bytMonSimpType ) // 这个判断重要,防止不正确的监控参数,导致不停的异常监控处理
					{
						PRTMSG(MSG_DBG, "Deal 103b failed 22!\n");
						iRet = ERR_PAR;
						//RenewMemInfo(0x02, 0x00, 0x01, 0x0d);
						goto DEAL103B_FAILED;
					}
					
#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 // 放在主动上报参数处理之后
					if( 0 == bytMonSimpType )	// 当且仅当定时更新方式时
					{
						// 补充对采样设置参数，也就是原来的定距参数的判断处理
						if( 0x7f == v_szData[dwPos + 7] ) v_szData[dwPos + 7] = 0;
						if( BYTE(v_szData[dwPos + 7]) > 4 )
						{
							PRTMSG(MSG_DBG, "Deal 103b failed 23!\n");
							iRet = ERR_PAR;
							goto DEAL103B_FAILED;
						}
						
						if( 1 == objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_bytStaType ) // 点火设置
						{
							obj103e.m_bytOnSndHour = BYTE(v_szData[dwPos + 2]);
							obj103e.m_bytOnSndMin = BYTE(v_szData[dwPos + 3]); // 上报时长
							obj103e.m_bytOnSampCycleMin = BYTE(v_szData[dwPos + 4]); 
							obj103e.m_bytOnSampCycleSec = BYTE(v_szData[dwPos + 5]); //时间间隔->采样周期
							obj103e.m_bytOnSndCycle = BYTE(v_szData[dwPos + 7]); // 距离间隔->发送周期

							byt103eSymb |= 0x01;
						}
						else if( 2 == objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_bytStaType ) // 熄火设置
						{
							obj103e.m_bytOffSndHour = BYTE(v_szData[dwPos + 2]);
							obj103e.m_bytOffSndMin = BYTE(v_szData[dwPos + 3]); // 上报时长
							obj103e.m_bytOffSampCycleMin = BYTE(v_szData[dwPos + 4]); 
							obj103e.m_bytOffSampCycleSec = BYTE(v_szData[dwPos + 5]); //时间间隔->采样周期
							obj103e.m_bytOffSndCycle = BYTE(v_szData[dwPos + 7]); // 距离间隔->发送周期

							byt103eSymb |= 0x02;
						}						
					}
#endif

#if USE_DRVREC == 1 // 放在主动上报参数处理之后
					if( 0 == bytMonSimpType )	// 当且仅当定时更新方式时
					{
						unsigned char sz103e[32] = { 0 };
						sz103e[0] = 0x10;
						sz103e[1] = 0x3e;
						sz103e[2] = objGprsCfg.m_bytGpsUpType;	//传输数据标识
						sz103e[3] = objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_bytStaType;	//状态标识
						sz103e[4] = BYTE(objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonTime % 256);
						sz103e[5] = BYTE(objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonTime / 256);
						sz103e[6] = BYTE(objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonInterval % 256);	//时间间隔->采样周期
						sz103e[7] = BYTE(objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonInterval / 256);
						if( 0 == sz103e[6] && 0 == sz103e[7] ) sz103e[6] = 1;

						//------
						if( 0x7f == v_szData[dwPos + 6] ) v_szData[dwPos + 6] = 0;
						if( 0x7f == v_szData[dwPos + 7] ) v_szData[dwPos + 7] = 0;
						if( BYTE(v_szData[dwPos + 6]) > 126 || BYTE(v_szData[dwPos + 7]) > 99 )
						{
							PRTMSG(MSG_DBG, "Deal 103b failed 24!\n");
							iRet = ERR_PAR;
							//RenewMemInfo(0x02, 0x00, 0x01, 0x0e);
							goto DEAL103B_FAILED;
						}
						objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonSpace
							= BYTE(v_szData[dwPos + 6]) * WORD(100) + BYTE(v_szData[dwPos + 7]);
// 						if( 0 == objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonSpace )
// 						{
// 							PRTMSG(MSG_DBG, "Deal 103b failed 25!\n");
// 							iRet = ERR_PAR;
// 							goto DEAL103B_FAILED; // 保证监控定距至少为10米
// 						}
						//---------

						sz103e[8] = BYTE(objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonSpace % 256); //距离间隔->发送周期
						sz103e[9] = BYTE(objGprsCfg.m_aryGpsUpPar[ bytParNo ].m_wMonSpace / 256);
						if( 0 == sz103e[8] && 0 == sz103e[9] ) sz103e[8] = 1;
						g_objDriveRecord.tcprcv.push(sz103e, 10);
					}
#endif
					// 递增状态个数
					objGprsCfg.m_bytGpsUpParCount ++;
					bytParNo ++;

					// 准备下次循环
					dwPos += 8;
				}
				
				bChgAutoRpt = true;

#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 // 放在主动上报参数处理之后
				// 千里眼系统,把103b帧作为行驶记录上报设置帧使用,所以模拟一帧103e到行驶记录上报模块
				if( (byt103eSymb & 0x03) == 0x03 )
				{
					obj103e.m_bytSndWinSize = 5; // 固定窗口大小

					iRet = g_objDrvRecRpt.Deal103e( v_szSrcHandtel, (char*)&obj103e, sizeof(obj103e) );
					if( iRet )
						goto DEAL103B_FAILED;
				}
#endif
			}
			break;

		case 0x03: // 在线方式提示
			{
				if( 1 != bytParLen )
				{
					iRet = ERR_PAR;
					//RenewMemInfo(0x02, 0x00, 0x01, 0x0f);
					goto DEAL103B_FAILED;
				}
				BYTE bytTipType = BYTE( v_szData[dw + 2] );
				if( 0 == bytTipType || bytTipType > 2 )
				{
					iRet = ERR_PAR;
					//RenewMemInfo(0x02, 0x00, 0x01, 0x10);
					goto DEAL103B_FAILED;
				}
				objGprsCfg.m_bytOnlineTip = bytTipType;
			}
			break;

		case 0x04: // 数据通道
			{
				// 080917,根据任务单080899修改,只允许报警数据、位置查询应答和短信通道下发设置的应答走短信通道
				// 短信备份功能已在代码里固定，故这里屏蔽对中心设置短信备份的处理。
				
				objGprsCfg.m_bytChannelBkType_1 = objGprsCfg.m_bytChannelBkType_2 = 0; // 强制关闭短信备份，保险起见
			}
			break;

		default:
			break;
		}

		dw += 2 + bytParLen; // 准备下次循环
	}

	// 组帧
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x7b, buf, sizeof(buf), iBufLen);
	if( !iRet )
	{
		// 保存新设置
		if( iRet = SetSecCfg(&objGprsCfg, offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg)) )
		{
			//RenewMemInfo(0x02, 0x00, 0x01, 0x11);
			goto DEAL103B_FAILED;
		}

		// 发送应答
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );
		if( !iRet )
		{
#if USE_AUTORPT == 1
			if( bChgAutoRpt )
			{				
				// 立刻应用新的主动上报设置
				g_objAutoRpt.Init();
			}
#endif
		}
		else
		{
			SetSecCfg( &objGprsCfgBkp, offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfgBkp) ); // 恢复原设置
			//RenewMemInfo(0x02, 0x00, 0x01, 0x12);
			goto DEAL103B_FAILED;
		}
	}
	else 
	{
		//RenewMemInfo(0x02, 0x00, 0x01, 0x13);
		goto DEAL103B_FAILED;
	}

	return 0;

DEAL103B_FAILED:
	bytResType = 0x02;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x7b, buf, sizeof(buf), iBufLen);
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );

	return iRet;
}

/// 设置IP地址
// 属性(1)+ 个数(1)+ [ 编号(1)+APN(20)+ TCP_IP(15)+ TCP_PORT(5)+ VUDP_IP(15)+VUDP_PORT(5)+ BUDP_IP(15)+BUDP_PORT(5) ] × N
int CConfig::Deal103c( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
	int iRet = 0;

#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL	
	tag1QIpCfg objIpCfg[3];
	tag1QIpCfg objIpCfgBkp[3];
	tag107c res;
	int iBufLen = 0;
	DWORD dw = 0;
	char* pBlank = NULL;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytParTotal = 0;
	BYTE bytPro = 0;
	BYTE bytIPNo = 0;
	BYTE byt = 0;
	bool bOneIpSetSucc = false;

	// 参数检查
	if( v_dwDataLen <= 2 )
	{
		return ERR_PAR;
	}
	bytPro = v_szData[0];
	bytParTotal = v_szData[1];
	if( 0 == bytParTotal || bytParTotal > 3 )
	{
		return ERR_PAR;
	}

	// 判断是否要清除原设置
	if( 0x7f == bytPro ) bytPro = 0;
	if( 0 == bytPro )
	{
// 		memset( buf, 0, sizeof(buf) );
// 		::SaveOneQianIpCfg( 0, buf, 80 );
// 		::SaveOneQianIpCfg( 1, buf, 80 );
// 		::SaveOneQianIpCfg( 2, buf, 80 );
	}

	// 解帧,更新设置
	dw = 2;
	for( byt = 0; byt < bytParTotal; byt ++, dw += 81 )
	{
		// 参数检查
		if( dw + 81 > v_dwDataLen )
		{
			iRet = ERR_PAR;
			continue;
		}

		// 编号
		bytIPNo = v_szData[ dw ];
		if( 0x7f == bytIPNo ) bytIPNo = 0;
		if( bytIPNo > 2 )
		{
			iRet = ERR_PAR;
			continue;
		}
		if( bytIPNo > 1 ) continue; // 暂时只处理2组IP设置

		// 预组帧 (放在最前)
		res.m_bytIPNo = bytIPNo;
		if( 0 == res.m_bytIPNo ) res.m_bytIPNo = 0x7f;

		// 获取原始参数
		iRet = GetImpCfg( (void*)objIpCfg, sizeof(tag1QIpCfg)*3, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg),sizeof(tag1QIpCfg)*3 );
		if( iRet ) 
			goto _ONEIP_FAILED;

		//objIpCfgBkp = objIpCfg; // 先备份
		memcpy((void*)objIpCfgBkp, (void*)objIpCfg, sizeof(tag1QIpCfg)*3);

		// APN
		memcpy( objIpCfg[bytIPNo].m_szApn, v_szData + dw + 1, sizeof(objIpCfg[bytIPNo].m_szApn) );
		objIpCfg[bytIPNo].m_szApn[ sizeof(objIpCfg[bytIPNo].m_szApn) - 1 ] = 0;
		pBlank = strchr( objIpCfg[bytIPNo].m_szApn, 0x20 );
		if( pBlank ) *pBlank = 0;
		
		// TCP IP
		memset( buf, 0, sizeof(buf) );
		strncpy( buf, v_szData + dw + 21, 15 );
		objIpCfg[bytIPNo].m_ulQianTcpIP = inet_addr( buf );

		// TCP Port
		memset( buf, 0, sizeof(buf) );
		strncpy( buf, v_szData + dw + 36, 5 );
		objIpCfg[bytIPNo].m_usQianTcpPort = htons( (short)(atoi(buf)) );

		// VUDP IP
		memset( buf, 0, sizeof(buf) );
		strncpy( buf, v_szData + dw + 41, 15 );
		objIpCfg[bytIPNo].m_ulVUdpIP = inet_addr( buf );

		// VUDP Port
		memset( buf, 0, sizeof(buf) );
		strncpy( buf, v_szData + dw + 56, 5 );
		objIpCfg[bytIPNo].m_usVUdpPort = htons( (short)(atoi(buf)) );

		// BUDP IP
		memset( buf, 0, sizeof(buf) );
		strncpy( buf, v_szData + dw + 61, 15 );
		objIpCfg[bytIPNo].m_ulQianUdpIP = inet_addr( buf );

		// BUDP Port
		memset( buf, 0, sizeof(buf) );
		strncpy( buf, v_szData + dw + 76, 5 );
		objIpCfg[bytIPNo].m_usQianUdpPort = htons( (short)(atoi(buf)) );

		// 生成成功应答
		res.m_bytResType = 0x01;
		iRet = g_objSms.MakeSmsFrame( (char*)&res, sizeof(res), 0x10, 0x7c, buf, sizeof(buf), iBufLen);
		if( iRet ) goto _ONEIP_FAILED;
	
		// 保存新设置
		iRet = SetImpCfg( (void*)objIpCfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(tag1QIpCfg)*3 );
		if( iRet ) goto _ONEIP_FAILED;

		// 发送应答
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0 , v_bFromSM ? v_szSrcHandtel : NULL);
		if( iRet )
		{
			// 若发送失败,恢复原设置
			SetImpCfg( (void*)objIpCfgBkp, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(tag1QIpCfg)*3 );
			goto _ONEIP_FAILED;
		}

		bOneIpSetSucc = true;
		continue;

_ONEIP_FAILED:
		res.m_bytResType = 0x02;
		iRet = g_objSms.MakeSmsFrame((char*)&res, sizeof(res), 0x10, 0x7c, buf, sizeof(buf), iBufLen);
		if( !iRet ) iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );
	}

	if( bOneIpSetSucc ) //启用新ip和端口
	{
		char buf = 0x01;	// QianExe请求SockServExe重新连接套接字		
		DataPush((void*)&buf, 1, DEV_QIAN, DEV_SOCK, LV3);
	}
#endif

#if 2 == USE_PROTOCOL
// 用户识别码（6）＋参数个数N（1）＋【参数类型（1）＋参数长度n（1）＋参数（n）】×N
// 参数类型	含义				长度	内容
// 0x01		APN设置						移动提供的APN参数，如：CMnet，则长度是5
// 0x02		Tcp主IP和端口		20		TCP_IP（15）＋ TCP_PORT（5）注：使用字符串的形式下发
// 0x03		Tcp第一备份IP和端口	20		TCP_IP（15）＋ TCP_PORT（5）注：使用字符串的形式下发
// 0x04		Tcp第二备份IP和端口	20		TCP_IP（15）＋ TCP_PORT（5）注：使用字符串的形式下发
// 0x05		UDP 使用的IP和端口	20		TCP_IP（15）＋ TCP_PORT（5）注：使用字符串的形式下发
// 0x06		视频UDP使用的IP和端口	20	TCP_IP（15）＋ TCP_PORT（5）注：使用字符串的形式下发

	char buf[200] = { 0 };
	char szPkt[100] = { 0 };
	int iBufLen = 0;
	BYTE bytParCount = 0;
	BYTE byt = 0;
	BYTE bytParType = 0;
	BYTE bytParLen = 0;
	DWORD dw = 0;
	tag1PComuCfg obj1PComuCfg;
	tag1QIpCfg obj1QIpCfg;
	obj1PComuCfg.Init(CDMA_NAME,CDMA_PASS);

	if( v_dwDataLen < 7 )
	{
		return ERR_PAR;	
	}
	
	// 预生成应答包（都是失败应答）
	szPkt[0] = v_szData[6];
	memset( szPkt + 1, 2, sizeof(szPkt) - 1 );

	bytParCount = BYTE( v_szData[6] );
	if( bytParCount > 5 || bytParCount < 1 )
	{
		bytParCount = 1;
		goto DEAL103C_END;
	}

	// 比较识别码
	if( memcmp(v_szData, USERIDENCODE, strlen(USERIDENCODE)) )
	{
		iRet = ERR_PAR;
		goto DEAL103C_END;
	}

// 	// 修改用户识别号
// 	iRet = ::GetImpCfg( &obj1PComuCfg, sizeof(obj1PComuCfg),
// 		offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
// 	if( iRet ) goto DEAL103C_END;
// 	memcpy( obj1PComuCfg.m_szUserIdentCode, v_szData, sizeof(obj1PComuCfg.m_szUserIdentCode) );
// 	iRet = ::SetImpCfg( &obj1PComuCfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
// 	if( iRet )
// 	{
// 		goto DEAL103C_END;
// 	}
	
	// 修改IP等参数
	dw = 7;
	for( byt = 0; byt < bytParCount; byt ++ )
	{
		if( v_dwDataLen < dw + 2 )
			goto DEAL103C_END;

		bytParType = v_szData[ dw ];
		bytParLen = v_szData[ dw + 1 ];

		if( v_dwDataLen < dw + 2 + bytParLen )
			goto DEAL103C_END;

		switch( bytParType )
		{
		case 1: // APN
			if( bytParLen >= sizeof(obj1QIpCfg.m_szApn) )
				goto DEAL103C_END;

			iRet = GetImpCfg( &obj1QIpCfg, sizeof(obj1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
			if( iRet ) goto DEAL103C_END;

			memset( obj1QIpCfg.m_szApn, 0, sizeof(obj1QIpCfg.m_szApn) ); // 确保NULL结尾
			memcpy( obj1QIpCfg.m_szApn, v_szData + dw + 2, bytParLen );

			iRet = SetImpCfg( &obj1QIpCfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
			if( iRet ) goto DEAL103C_END;

			break;

		case 2: // Tcp IP 端口
			if( 20 != bytParLen )
				goto DEAL103C_END;

			iRet = GetImpCfg( &obj1QIpCfg, sizeof(obj1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
			if( iRet ) goto DEAL103C_END;

			memset( buf, 0, sizeof(buf) );
			memcpy( buf, v_szData + dw + 2, 15 );
			obj1QIpCfg.m_ulQianTcpIP = inet_addr( buf );
			
			memset( buf, 0, sizeof(buf) );
			memcpy( buf, v_szData + dw + 17, 5 );
			obj1QIpCfg.m_usQianTcpPort = htons( (unsigned short)(atoi(buf)) );

			iRet = SetImpCfg( &obj1QIpCfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
			if( iRet ) goto DEAL103C_END;

			break;

		case 3: // Tcp备份1 IP 端口 (暂不启用)
			if( 20 != bytParLen )
				goto DEAL103C_END;
			break;

		case 4: // Tcp备份2 IP 端口 (暂不启用)
			if( 20 != bytParLen )
				goto DEAL103C_END;
			break;

		case 5: // Udp IP 端口
			if( 20 != bytParLen )
				goto DEAL103C_END;

			iRet = GetImpCfg( &obj1QIpCfg, sizeof(obj1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
			if( iRet ) goto DEAL103C_END;

			memset( buf, 0, sizeof(buf) );
			memcpy( buf, v_szData + dw + 2, 15 );
			obj1QIpCfg.m_ulQianUdpIP = inet_addr( buf );
			
			memset( buf, 0, sizeof(buf) );
			memcpy( buf, v_szData + dw + 17, 5 );
			obj1QIpCfg.m_usQianUdpPort = htons( (unsigned short)(atoi(buf)) );

			iRet = SetImpCfg( &obj1QIpCfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
			if( iRet ) goto DEAL103C_END;
			
			break;

		case 6: // 视频Udp IP 端口
			if( 20 != bytParLen )
				goto DEAL103C_END;
			
			iRet = GetImpCfg( &obj1QIpCfg, sizeof(obj1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
			if( iRet ) goto DEAL103C_END;
			
			memset( buf, 0, sizeof(buf) );
			memcpy( buf, v_szData + dw + 2, 15 );
			obj1QIpCfg.m_ulVUdpIP = inet_addr( buf );
			
			memset( buf, 0, sizeof(buf) );
			memcpy( buf, v_szData + dw + 17, 5 );
			obj1QIpCfg.m_usVUdpPort = htons( (unsigned short)(atoi(buf)) );
			
			iRet = SetImpCfg( &obj1QIpCfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]), sizeof(obj1QIpCfg) );
			if( iRet ) goto DEAL103C_END;
			
			break;

		default:
			;
		}

		szPkt[ 1 + byt ] = 1; // 该参数成功处理标志

		dw += 2 + bytParLen;
	}

DEAL103C_END:

	// 若所有参数都设置成功，则启用新的IP和端口
	int i=1;
	for(i=1; i<szPkt[0]; i++)
	{
		if( 0x02 == szPkt[i] )
			break;
	}
	if( i == szPkt[0] )
	{
		char buf = 0x01;	// QianExe请求SockServExe重新连接套接字		
		DataPush((void*)&buf, 1, DEV_QIAN, DEV_SOCK, LV3);
	}

	memset( buf, 0, sizeof(buf) );
	iRet = g_objSms.MakeSmsFrame( szPkt, int(bytParCount) + 1, 0x10, 0x7c, buf, sizeof(buf), iBufLen );
	if( !iRet )
	{
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );
	}
	
#endif

	return iRet;
}

#if 2 == USE_PROTOCOL
int CConfig::Deal103e( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
	int iRet = 0;

	iRet = ResumeCfg( true, false );
	
	char buf[100] = { 0 };
	int iBufLen = 0;
	BYTE bytResult = iRet ? 0x7f : 0;

	iRet = g_objSms.MakeSmsFrame( (char*)&bytResult, sizeof(bytResult), 0x10, 0x7e, buf, sizeof(buf), iBufLen );
	if( !iRet )
	{
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );
	}

	return iRet;
}

int CConfig::Deal103f( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, char* v_szDesHandtel, bool v_bFromSM )
{
// 短信中心号码(22)+用户识别码(6)

	tag103f req;
	tag1PComuCfg obj1PComuCfg, obj1PComuCfgBkp;
	tag2QServCodeCfg obj2QServCodeCfg;
	char* pBlank = NULL;
	int iRet = 0;
	char buf[ 200 ] = { 0 };
	int iBufLen = 0;
	BYTE bytResType = 0;

	// 数据参数检查
	if( sizeof(req) + QIANDOWNFRAME_BASELEN - 1 != v_dwDataLen )
	{
		iRet = ERR_PAR;
		goto DEAL103F_FAILED;
	}

	// 提取设置参数
	memcpy( &req, v_szData + QIANDOWNFRAME_BASELEN - 1, sizeof(req) );

	// 比较识别码
	if( memcmp(req.m_szUserIdentCode, USERIDENCODE, strlen(USERIDENCODE)) )
	{
		iRet = ERR_PAR;
		goto DEAL103F_FAILED;
	}

	// 先读取,再修改

	iRet = GetImpCfg( &obj1PComuCfg, sizeof(obj1PComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
	if( iRet ) goto DEAL103F_FAILED;
	obj1PComuCfgBkp = obj1PComuCfg; // 先备份

	iRet = GetSecCfg( &obj2QServCodeCfg, sizeof(obj2QServCodeCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(obj2QServCodeCfg) );
	if( iRet ) goto DEAL103F_FAILED;
	// 不备份一般配置

	// 修改短信中心号
	memcpy( obj1PComuCfg.m_szSmsCentID, req.m_szSmsCentID, sizeof(obj1PComuCfg.m_szSmsCentID) );
	StrRemvBlank( obj1PComuCfg.m_szSmsCentID, sizeof(obj1PComuCfg.m_szSmsCentID) );

// 	// 附带修改用户识别号
// 	memcpy( obj1PComuCfg.m_szUserIdentCode, req.m_szUserIdentCode, sizeof(obj1PComuCfg.m_szUserIdentCode) );

	// 附带修改本机手机号
	memcpy( obj1PComuCfg.m_szTel, v_szDesHandtel, sizeof(obj1PComuCfg.m_szTel) );
	obj1PComuCfg.m_szTel[ sizeof(obj1PComuCfg.m_szTel) - 1 ] = 0;
	pBlank = strchr( obj1PComuCfg.m_szTel, 0x20 );
	if( pBlank ) *pBlank = 0;

	// 附带修改中心ID (即区域号)
#if USE_PROTOCOL == 0 || USE_PROTOCOL == 2
	obj2QServCodeCfg.m_szAreaCode[0] = v_szData[0];
	obj2QServCodeCfg.m_szAreaCode[1] = v_szData[1];
#endif
#if USE_PROTOCOL == 1
	obj2QServCodeCfg.m_szAreaCode[0] = v_szData[2];
	obj2QServCodeCfg.m_szAreaCode[1] = v_szData[3];
#endif

	if( 0x7f == obj2QServCodeCfg.m_szAreaCode[1] ) obj2QServCodeCfg.m_szAreaCode[1] = 0; // 保存时7f还原为0
	
	// 写回和发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x7f, buf, sizeof(buf), iBufLen ); 
	if( !iRet )
	{
		// 保存新设置 (附带保存手机号、用户识别号)
		if( iRet = SetImpCfg(&obj1PComuCfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg)) )
		{
			goto DEAL103F_FAILED;
		}

		// 发送
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_szSrcHandtel );
		if( !iRet )
		{
			SetSecCfg( &obj2QServCodeCfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(obj2QServCodeCfg) ); // 附带写入区域号

			// 通知底层立刻生效新的短信中心号
			char buf[2] = {0x01, 0x36};
			DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		}
		else // 否则(即发送失败)
		{
			SetImpCfg( &obj1PComuCfgBkp, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfgBkp) ); // 恢复原设置 ((附带恢复手机号))
			goto DEAL103F_FAILED;
		}
	}
	else goto DEAL103F_FAILED;

	return 0;

DEAL103F_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x7f, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_szSrcHandtel );

	return iRet;
}
#endif

/// 设置电话号码本:
int CConfig::Deal3301( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen )
{
#if 2 == USE_TELLIMIT
// 两客通用版协议
// 帧索引(1)+设置字(1) + 设置个数（1）（最多30个）
// +【索引号（1）+ 电话号码（15）+ 姓名（10）+ 权限设置（1）+ 通话时长（2）】*N
	tag2QCallCfg objCallCfg, objCallCfgBkp;
	tag3341 res;
	int iRet = 0;
	int iBufLen = 0;
	DWORD dw = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytSetCtrl = 0;
	BYTE bytSetCount = 0;
	BYTE byt = 0;

	res.m_bytFrameNo = 0x7d; // 初始一个无意义值,避开0和7f,7e

	// 数据参数检查
	if( v_dwDataLen < 1 )
	{
		iRet = ERR_PAR;
		goto DEAL3301_FAILED;
	}
	res.m_bytFrameNo = v_szData[0]; // 立刻填上应答的帧索引

	// 读取原始配置
	iRet = GetSecCfg( &objCallCfg, sizeof(objCallCfg), offsetof(tagSecondCfg, m_uni2QCallCfg.m_obj2QCallCfg), sizeof(objCallCfg) );
	if( iRet ) goto DEAL3301_FAILED;
	objCallCfgBkp = objCallCfg; // 先备份

	// 参数检查
	if( v_dwDataLen < 2 )
	{
		iRet = ERR_PAR;
		goto DEAL3301_FAILED;
	}

	// 从请求帧获取基本信息
	bytSetCtrl = BYTE( v_szData[1] ); // 设置字

	// 长度检查
	if( 0x08 == (bytSetCtrl & 0x0c) || 0x02 == (bytSetCtrl & 0x03) ) // 若呼入或呼出有电话本控制
	{
		if( v_dwDataLen < 3 )
		{
			iRet = ERR_PAR;
			goto DEAL3301_FAILED;
		}
	}

	// 设置个数
	bytSetCount = BYTE( v_szData[2] );
	if( 0x7f == bytSetCount ) bytSetCount = 0;
	if( bytSetCount > sizeof(objCallCfg.m_aryTelBook) / sizeof(objCallCfg.m_aryTelBook[0]) )
	{
		iRet = ERR_PAR;
		goto DEAL3301_FAILED;
	}

	// 长度检查
	if( v_dwDataLen < 3 + 29 * DWORD(bytSetCount) )
	{
		iRet = ERR_PAR;
		goto DEAL3301_FAILED;
	}

	// 判断是否要对原设置进行清除
	if( 0x20 & bytSetCtrl )
	{
		memset( objCallCfg.m_aryTelBook, 0, sizeof(objCallCfg.m_aryTelBook) );
	}

	// 更新配置
	objCallCfg.m_bytSetType = bytSetCtrl & 0x0f;
	dw = 3;
	for( byt = 0; byt < bytSetCount; byt ++ )
	{
		BYTE bytTelIndex = BYTE( v_szData[dw] ); // 电话本索引
		if( 0 == bytTelIndex || bytTelIndex > sizeof(objCallCfg.m_aryTelBook) / sizeof(objCallCfg.m_aryTelBook[0]) )
		{
			iRet = ERR_PAR;
			goto DEAL3301_FAILED;
		}

		memcpy( objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_szTel, v_szData + dw + 1,
			sizeof(objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_szTel) ); // 电话号码
		memcpy( objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_szName, v_szData + dw + 16,
			min(sizeof(objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_szName), 10) ); // 姓名
		objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_bytType = BYTE( v_szData[dw + 26] ); // 权限

		// 通话限时
		if( 0x7f == v_szData[dw + 27] ) v_szData[dw + 27] = 0;
		if( 0x7f == v_szData[dw + 28] ) v_szData[dw + 28] = 0;
		if( BYTE(v_szData[dw + 27]) >= 0x7f || BYTE(v_szData[dw + 28]) > 59 )
		{
			iRet = ERR_PAR;
			goto DEAL3301_FAILED;
		}
		objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_wLmtPeriod = BYTE( v_szData[dw + 27] ) * 60 + BYTE( v_szData[dw + 28] );

		dw += 29;
	}

	// 发送应答
	res.m_bytAnsType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&res, sizeof(res), 0x33, 0x41, buf, sizeof(buf), iBufLen);
	if( !iRet )
	{
		// 写入文件
		if( iRet = SetSecCfg( &objCallCfg, offsetof(tagSecondCfg, m_uni2QCallCfg.m_obj2QCallCfg), sizeof(objCallCfg)) )
		{
			goto DEAL3301_FAILED;
		}

		if( iRet = g_objSock.SOCKSNDSMSDATA(buf, iBufLen, LV12, 0) )
		{
			SetSecCfg( &objCallCfgBkp, offsetof(tagSecondCfg, m_uni2QCallCfg.m_obj2QCallCfg), sizeof(objCallCfgBkp) );
			goto DEAL3301_FAILED;
		}
	}
	else
		goto DEAL3301_FAILED;

	return 0;

DEAL3301_FAILED:
	res.m_bytAnsType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&res, sizeof(res), 0x33, 0x41, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );

	return iRet;

#endif
	
#if 3 == USE_TELLIMIT
// 公开版_研一
// 帧索引(1)+设置个数（1）（最多30个）+（索引号（1）+ 电话号码（15）+ 姓名（20）+ 权限设置（1）+ 通话时长（2））*N
	tag2QCallCfg objCallCfg, objCallCfgBkp;
	tag3341 res;
	int iRet = 0;
	int iBufLen = 0;
	DWORD dw = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytSetCtrl = 0;
	BYTE bytSetCount = 0;
	BYTE byt = 0;

	res.m_bytFrameNo = 0x7d; // 初始一个无意义值,避开0和7f,7e

	// 数据参数检查
	if( v_dwDataLen < 1 )
	{
		iRet = ERR_PAR;
		goto DEAL3301_FAILED;
	}
	res.m_bytFrameNo = v_szData[0]; // 立刻填上应答的帧索引

	// 读取原始配置
	iRet = GetSecCfg( &objCallCfg, sizeof(objCallCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objCallCfg) );
	if( iRet ) goto DEAL3301_FAILED;
	objCallCfgBkp = objCallCfg; // 先备份

	// 参数检查
	if( v_dwDataLen < 2 )
	{
		iRet = ERR_PAR;
		goto DEAL3301_FAILED;
	}

	// 设置个数
	bytSetCount = BYTE( v_szData[1] );
	if( 0x7f == bytSetCount ) bytSetCount = 0;
	if( bytSetCount > sizeof(objCallCfg.m_aryTelBook) / sizeof(objCallCfg.m_aryTelBook[0]) )
	{
		iRet = ERR_PAR;
		goto DEAL3301_FAILED;
	}

	// 长度检查
	if( v_dwDataLen < 2 + 39 * DWORD(bytSetCount) )
	{
		iRet = ERR_PAR;
		goto DEAL3301_FAILED;
	}

	// 判断是否要对原设置进行清除
	if( 0x20 & bytSetCtrl )
	{
		memset( objCallCfg.m_aryTelBook, 0, sizeof(objCallCfg.m_aryTelBook) );
	}

	// 更新配置
	objCallCfg.m_bytSetType = bytSetCtrl & 0x0f;
	dw = 2;
	for( byt = 0; byt < bytSetCount; byt ++ )
	{
		BYTE bytTelIndex = BYTE( v_szData[dw] ); // 电话本索引
		if( 0 == bytTelIndex || bytTelIndex > sizeof(objCallCfg.m_aryTelBook) / sizeof(objCallCfg.m_aryTelBook[0]) )
		{
			iRet = ERR_PAR;
			goto DEAL3301_FAILED;
		}

		memcpy( objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_szTel, v_szData + dw + 1,
			sizeof(objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_szTel) ); // 电话号码
		memcpy( objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_szName, v_szData + dw + 16,
			min(sizeof(objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_szName), 20) ); // 姓名
		objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_bytType = BYTE( v_szData[dw + 36] ); // 权限

		// 通话限时
		if( 0x7f == v_szData[dw + 37] ) v_szData[dw + 37] = 0;
		if( 0x7f == v_szData[dw + 38] ) v_szData[dw + 38] = 0;
		if( BYTE(v_szData[dw + 37]) >= 0x7f || BYTE(v_szData[dw + 38]) > 59 )
		{
			iRet = ERR_PAR;
			goto DEAL3301_FAILED;
		}
		objCallCfg.m_aryTelBook[ bytTelIndex - 1 ].m_wLmtPeriod
			= BYTE( v_szData[dw + 37] ) * 60 + BYTE( v_szData[dw + 38] );

		dw += 39;
	}

	// 发送应答
	res.m_bytAnsType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&res, sizeof(res), 0x33, 0x41, buf, sizeof(buf), iBufLen);
	if( !iRet )
	{
		// 写入文件
		if( iRet = SetSecCfg( &objCallCfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objCallCfg)) )
		{
			goto DEAL3301_FAILED;
		}

		if( iRet = g_objSock.SOCKSNDSMSDATA(buf, iBufLen, LV12, 0) )
		{
			SetSecCfg( &objCallCfgBkp, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objCallCfgBkp) );
			goto DEAL3301_FAILED;
		}
	}
	else
		goto DEAL3301_FAILED;

	return 0;

DEAL3301_FAILED:
	res.m_bytAnsType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&res, sizeof(res), 0x33, 0x41, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );

	return iRet;
#endif

	return 0;
}

/// 设置中心服务电话号码: 个数（1）＋【电话号码（15）】×N
int CConfig::Deal3302( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen )
{
#if 2 == USE_TELLIMIT || 3 == USE_TELLIMIT
	tag2QServCodeCfg objCentServTelCfg, objCentServTelCfgBkp;
	int iRet = 0;
	int iBufLen = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytResType = 0;
	BYTE bytTelCount = 0;
	BYTE bytCanCount = 0;

	// 数据参数检查
	if( v_dwDataLen < 1 )
	{
		iRet = ERR_PAR;
		goto DEAL3302_FAILED;
	}

	// 读取原始配置
	iRet = GetSecCfg( &objCentServTelCfg, sizeof(objCentServTelCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objCentServTelCfg) );
	if( iRet ) goto DEAL3302_FAILED;

	objCentServTelCfgBkp = objCentServTelCfg; // 先备份

	// 号码个数
	bytTelCount = BYTE( v_szData[0] );
	if( 0x7f == bytTelCount ) bytTelCount = 0;
	if( bytTelCount > sizeof(objCentServTelCfg.m_aryCentServTel) / sizeof(objCentServTelCfg.m_aryCentServTel[0]) )
	{
		iRet = ERR_PAR;
		goto DEAL3302_FAILED;
	}

	// 长度检查
	if( v_dwDataLen < 1 + 15 * DWORD(bytTelCount) )
	{
		iRet = ERR_PAR;
		goto DEAL3302_FAILED;
	}

	// 判断是否要清除原设置
	if( 0 == bytTelCount ) memset( &objCentServTelCfg, 0, sizeof(objCentServTelCfg) );

	// 更新配置
	if( objCentServTelCfg.m_bytCentServTelCount > sizeof(objCentServTelCfg.m_aryCentServTel) / sizeof(objCentServTelCfg.m_aryCentServTel[0]) )
		objCentServTelCfg.m_bytCentServTelCount = sizeof(objCentServTelCfg.m_aryCentServTel) / sizeof(objCentServTelCfg.m_aryCentServTel[0]); // 保险处理
	bytCanCount = sizeof(objCentServTelCfg.m_aryCentServTel) / sizeof(objCentServTelCfg.m_aryCentServTel[0]) - objCentServTelCfg.m_bytCentServTelCount;
	if( bytCanCount > bytTelCount ) bytCanCount = bytTelCount; // 实际可再存入的号码个数
	memcpy( objCentServTelCfg.m_aryCentServTel + objCentServTelCfg.m_bytCentServTelCount,
		v_szData + 1, bytCanCount * sizeof(objCentServTelCfg.m_aryCentServTel[0]) );
	objCentServTelCfg.m_bytCentServTelCount += bytCanCount;

	// 发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x33, 0x42, buf, sizeof(buf), iBufLen);
	if( !iRet )
	{
		// 写入文件
		if( iRet = SetSecCfg( &objCentServTelCfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objCentServTelCfg)) )
		{
			goto DEAL3302_FAILED;
		}

		if( iRet = g_objSock.SOCKSNDSMSDATA(buf, iBufLen, LV12, 0) )
		{
			SetSecCfg( &objCentServTelCfgBkp, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objCentServTelCfgBkp) );
			goto DEAL3302_FAILED;
		}
	}
	else
		goto DEAL3302_FAILED;

	return 0;

DEAL3302_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x33, 0x42, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );

	return iRet;

#else
	return 0;
#endif
}

/// 设置UDP激活号码: 设置属性（1）+ 电话个数N（1）（最多10个） + 【号码长度（1） + 电话号码串（1~15）】× N
int CConfig::Deal3304( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen )
{
#if QIANTYPE_USE == QIANTYPE_KJ2
	tag2QServCodeCfg objUdpAtvTelCfg, objUdpAtvTelCfgBkp;
	int iRet = 0;
	int iBufLen = 0;
	DWORD dw = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytResType = 0;
	BYTE bytSetType;
	BYTE bytTelCount = 0;
	BYTE bytCanCount = 0;
	BYTE bytTelLen = 0;
	BYTE byt = 0;

	// 数据参数检查
	if( v_dwDataLen < 2 )
	{
		iRet = ERR_PAR;
		goto DEAL3304_FAILED;
	}

	// 读取原始配置
	iRet = GetSecCfg( &objUdpAtvTelCfg, sizeof(objUdpAtvTelCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objUdpAtvTelCfg) );
	if( iRet ) goto DEAL3304_FAILED;
	objUdpAtvTelCfgBkp = objUdpAtvTelCfg; // 先备份

	// 设置属性
	bytSetType = BYTE( v_szData[0] );

	// 号码个数
	bytTelCount = BYTE( v_szData[1] );
	if( 0x7f == bytTelCount ) bytTelCount = 0;
	if( bytTelCount > sizeof(objUdpAtvTelCfg.m_aryUdpAtvTel) / sizeof(objUdpAtvTelCfg.m_aryUdpAtvTel[0]) )
	{
		iRet = ERR_PAR;
		goto DEAL3304_FAILED;
	}

	// 判断是否要清除原设置
	if( 1 == bytSetType )
	memset( &objUdpAtvTelCfg, 0, sizeof(objUdpAtvTelCfg) );

	// 实际可再存入的号码个数
	if( objUdpAtvTelCfg.m_bytUdpAtvTelCount > sizeof(objUdpAtvTelCfg.m_aryUdpAtvTel) / sizeof(objUdpAtvTelCfg.m_aryUdpAtvTel[0]) )
		objUdpAtvTelCfg.m_bytUdpAtvTelCount = sizeof(objUdpAtvTelCfg.m_aryUdpAtvTel) / sizeof(objUdpAtvTelCfg.m_aryUdpAtvTel[0]); // 保险处理
	bytCanCount = sizeof(objUdpAtvTelCfg.m_aryUdpAtvTel) / sizeof(objUdpAtvTelCfg.m_aryUdpAtvTel[0]) - objUdpAtvTelCfg.m_bytUdpAtvTelCount;
	if( bytCanCount > bytTelCount ) bytCanCount = bytTelCount;

	// 更新配置
	dw = 2;
	for( byt = 0; byt < bytCanCount; byt ++ )
	{
		// 参数长度检查
		if( v_dwDataLen < dw + 2 )
		{
			iRet = ERR_PAR;
			goto DEAL3304_FAILED;
		}

		// 号码长度
		bytTelLen = BYTE( v_szData[ dw ] & 0x0f );
		if( bytTelLen < 1 || bytTelLen > sizeof(objUdpAtvTelCfg.m_aryUdpAtvTel[0]) / sizeof(objUdpAtvTelCfg.m_aryUdpAtvTel[0][0]) )
		{
			iRet = ERR_PAR;
			goto DEAL3304_FAILED;
		}

		// 参数长度检查
		if( v_dwDataLen < dw + 1 + bytTelLen )
		{
			iRet = ERR_PAR;
			goto DEAL3304_FAILED;
		}

		// 保存号码
		memcpy( objUdpAtvTelCfg.m_aryUdpAtvTel + objUdpAtvTelCfg.m_bytUdpAtvTelCount, v_szData + dw + 1, bytTelLen );
		objUdpAtvTelCfg.m_bytUdpAtvTelCount ++;

		dw += 1 + bytTelLen;
	}

	// 发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x33, 0x44, buf, sizeof(buf), iBufLen);
	if( !iRet )
	{
		// 写入文件
		if( iRet = SetSecCfg( &objUdpAtvTelCfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objUdpAtvTelCfg)) )
		{
			goto DEAL3304_FAILED;
		}

		if( iRet = g_objSock.SOCKSNDSMSDATA(buf, iBufLen, LV12, 0) )
		{
			SetSecCfg( &objUdpAtvTelCfgBkp, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objUdpAtvTelCfgBkp) );
			goto DEAL3304_FAILED;
		}
	}
	else
		goto DEAL3304_FAILED;

	return 0;

DEAL3304_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x33, 0x44, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );

	return iRet;

#else
	return 0;
#endif
}

// 车辆特征系数指令
int CConfig::DealComu6d( char* v_szData, DWORD v_dwDataLen )
{
// 请求
// 指令类型（1）
// 01H 查询；
// 02H 设置 + 设定值（2）； 设定值:每公里脉冲数
// 03H 启动检测；
// 04H 停止检测；
// 05H 用户输入里程 ＋ 里程值（1） 里程值单位：公里

// 应答
// 指令号（1）
// 01H 查询应答 ＋ 应答类型（1） ＋ 查询结果（2）；
// 02H 设置应答 ＋ 应答类型（1）；
// 03H 启动检测应答 ＋ 应答类型（1）；
// 04H 结束检测应答 ＋ 应答类型（1）；
// 05H 检测结果应答 ＋ 应答类型（1）；
// 应答类型：01H-成功
//           02H-失败

// 	char buf[64];
// 	buf[0] = 0x6e;
// 	buf[1] = v_szData[0];
// 
// 	if( v_dwDataLen < 1 ) return ERR_PAR;
// 
// 	switch( BYTE(v_szData[0]) )
// 	{
// 	case 1: // 查询
// 		{
// 			WORD wCyclePerKm = 0;
// 			GetImpCfg( &wCyclePerKm, sizeof(wCyclePerKm), offsetof(tagImportantCfg, m_uni1QBiaodingCfg.m_obj1QBiaodingCfg.m_wCyclePerKm), sizeof(wCyclePerKm) );
// 			buf[2] = 1;
// 			buf[3] = wCyclePerKm / 0x100;
// 			buf[4] = wCyclePerKm % 0x100;
// 			DataPush(buf, 5, DEV_QIAN, DEV_DIAODU, LV2);
// 		}		
// 		break;
// 
// 	case 2: // 设置
// 		{
// 			if( v_dwDataLen < 3 ) return ERR_PAR;
// 
// 			WORD wCyclePerKm = 0;
// 			wCyclePerKm = BYTE(v_szData[1]) * 0x100 + BYTE(v_szData[2]);
// 
// 			int iResult = SetImpCfg( &wCyclePerKm, offsetof(tagImportantCfg, m_uni1QBiaodingCfg.m_obj1QBiaodingCfg.m_wCyclePerKm), sizeof(wCyclePerKm) );
// 			if( !iResult ) buf[2] = 1;
// 			else buf[2] = 2;
// 
// 			DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);
// 		}
// 		break;
// 
// 	case 3: // 启动检测
// 		{
// 			m_bytBiaodinSta = 1;
// 			
// 			g_objQianIO.IN_QueryMeterCycle( m_dwBiaodinBeginCycle );
// 			buf[2] = 1;
// 			DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);
// 		}
// 		break;
// 
// 	case 4: // 结束检测
// 		{
// 			if( 1 != m_bytBiaodinSta )
// 				buf[2] = 2;
// 			else
// 			{
// 				m_bytBiaodinSta = 2;
// 				g_objQianIO.IN_QueryMeterCycle( m_dwBiaodinEndCycle );
// 				buf[2] = 1;
// 			}
// 
// 			DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);
// 		}
// 		break;
// 
// 	case 5: // 标定
// 		{
// 			if( v_dwDataLen < 2 ) return ERR_PAR;
// 
// 			if( 2 != m_bytBiaodinSta )
// 			{
// 				buf[2] = 2;
// 			}
// 			else if( 0 == v_szData[1] ) // 防止除0异常
// 			{
// 				buf[2] = 2;
// 			}
// 			else
// 			{
// 				WORD wCyclePerKm = WORD( double(m_dwBiaodinEndCycle - m_dwBiaodinBeginCycle) / BYTE(v_szData[1]) );
// 				int iResult = SetImpCfg( &wCyclePerKm, offsetof(tagImportantCfg, m_uni1QBiaodingCfg.m_obj1QBiaodingCfg.m_wCyclePerKm), sizeof(wCyclePerKm) );
// 				if( !iResult ) buf[2] = 1;
// 				else buf[2] = 2;
// 			}
// 
// 			DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);
// 		}
// 		break;
// 
// 	default:
// 		;
// 	}

	return 0;
}

void CConfig::GetVer( char* v_szTel, size_t v_sizTel )
{
	
}
