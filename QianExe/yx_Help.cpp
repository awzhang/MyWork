#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-Help     ")

void G_TmAutoDialHelpTel(void *arg, int ien)
{
	g_objHelp.P_TmAutoDialHelpTel();
}

//////////////////////////////////////////////////////////////////////////
CHelp::CHelp()
{
	
}

CHelp::~CHelp()
{

}

// 求助应答: 应答类型（1）+ 求助类型（1）+ 日期（3）+ 时间（3）
int CHelp::Deal0301( char* v_szData, DWORD v_dwDataLen )
{
	if( v_dwDataLen < 8 ) return ERR_PAR;
	
	char buf[ 32 ] = { 0 };
	buf[ 0 ] = BYTE( 0x01 );
	buf[ 1 ] = BYTE( 0x83 );
	memcpy( buf + 2, v_szData, 8 );
	
	// 日期时间可能要转换
	if( 0x7f == BYTE(buf[4]) ) buf[4] = 0;
	if( 0x7f == BYTE(buf[7]) ) buf[7] = 0;
	if( 0x7f == BYTE(buf[8]) ) buf[8] = 0;
	if( 0x7f == BYTE(buf[9]) ) buf[9] = 0;

	DataPush(buf, 10, DEV_QIAN, DEV_DIAODU, LV2);
	return 0;
}

int CHelp::Deal0e01( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen )
{
	tag2QServCodeCfg objServCodeCfg, objServCodeCfgBkp;
	int iRet = 0;
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	BYTE bytResType = 0;
	
	// 数据参数检查
	if( 15 != v_dwDataLen )
	{
		iRet = ERR_PAR;
		goto DEAL0E01_FAILED;
	}
	
	// 读取->修改
	iRet = GetSecCfg( &objServCodeCfg, sizeof(objServCodeCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfg) );
	if( iRet ) goto DEAL0E01_FAILED;
	objServCodeCfgBkp = objServCodeCfg; // 先备份

	memcpy( objServCodeCfg.m_szHelpTel, v_szData, sizeof(objServCodeCfg.m_szHelpTel) );
	
	// 写回和发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x0e, 0x41, buf, sizeof(buf), iBufLen); 
	if( !iRet )
	{
		iRet = SetSecCfg(&objServCodeCfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfg) );
		if( iRet ) 
			goto DEAL0E01_FAILED;
		
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 ); 

		if( iRet ) // 若发送失败
		{
			SetSecCfg( &objServCodeCfgBkp, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfgBkp) ); // 恢复原设置
			goto DEAL0E01_FAILED;
		}
	}
	else goto DEAL0E01_FAILED;
	
	return 0;
	
DEAL0E01_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x0e, 0x41, buf, sizeof(buf), iBufLen);
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );
	
	return iRet;
}

int CHelp::Deal100b( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
	tag2QServCodeCfg objServCodeCfg, objServCodeCfgBkp;
	int iRet = 0;
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	BYTE bytResType = 0;
	
	// 数据参数检查
	if( 15 != v_dwDataLen )
	{
		iRet = ERR_PAR;
		goto DEAL100b_FAILED;
	}
	
	// 读取->修改
	iRet = GetSecCfg( &objServCodeCfg, sizeof(objServCodeCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfg) );
	if( iRet ) goto DEAL100b_FAILED;
	objServCodeCfgBkp = objServCodeCfg; // 先备份

	memcpy( objServCodeCfg.m_szHelpTel, v_szData, sizeof(objServCodeCfg.m_szHelpTel) );
	
	// 写回和发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x4b, buf, sizeof(buf), iBufLen); 
	if( !iRet )
	{
		iRet = SetSecCfg(&objServCodeCfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfg) );
		if( iRet ) goto DEAL100b_FAILED;
		
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL ); // 发送
		if( iRet ) // 若发送失败
		{
			SetSecCfg( &objServCodeCfgBkp, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfgBkp) ); // 恢复原设置
			goto DEAL100b_FAILED;
		}
	}
	else goto DEAL100b_FAILED;
	
	return 0;
	
DEAL100b_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x4b, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );
	
	return iRet;
}

int CHelp::DealComu80( char* v_szData, DWORD v_dwDataLen )
{
	int iRet = 0;
	int iBufLen = 0;
	WORD wPhotoType = 0;
	char buf[SOCK_MAXSIZE];
	tag0341 req;
	tagQianGps objQianGps;
	tag2QGprsCfg objGprsCfg;
	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg), offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );
	
	if( v_dwDataLen < 1 )
	{
		iRet = ERR_PAR;
		goto DEALCOMU80_END;
	}
	
	iRet = g_objMonAlert.GetCurQianGps( objQianGps, true );
	if( iRet ) goto DEALCOMU80_END;
	
	req.m_bytHelpType = v_szData[ 0 ];
	memcpy( &req.m_objQianGps, &objQianGps, sizeof(req.m_objQianGps) );
	
	iRet = g_objSms.MakeSmsFrame( (char*)&req, sizeof(req), 0x03, 0x41, buf, sizeof(buf), iBufLen );
	if( iRet ) goto DEALCOMU80_END;
	
	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV13,
		(objGprsCfg.m_bytChannelBkType_1 & 0x04) ? DATASYMB_SMSBACKUP : 0 );
	
DEALCOMU80_END:
	BYTE bytResType = 0;
	if( !iRet ) bytResType = 1;
	else bytResType = 2;
	
	buf[0] = 0x01;
	buf[1] = char(0x81);
	buf[2] = bytResType;
	DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);
	
#if QIANTYPE_USE == QIANTYPE_KJ2 || QIANTYPE_USE == QIANTYPE_JG2 || QIANTYPE_USE == QIANTYPE_V7
	_SetTimer(&g_objTimerMng, AUTODIALHELPTEL_TIMER, 3000, G_TmAutoDialHelpTel );
#endif
	
	return iRet;
}

void CHelp::P_TmAutoDialHelpTel()
{

}



