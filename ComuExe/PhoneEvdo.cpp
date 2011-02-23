// PhoneEvdo.cpp: implementation of the CPhoneEvdo class.
//
//////////////////////////////////////////////////////////////////////

#include "ComuStdAfx.h"


#undef MSG_HEAD
#define MSG_HEAD ("ComuExe-PhoneEvdo:")


#if USE_LANGUAGE == 0
#define LANG_NETSEARCH_FAIL ("网络搜寻失败，请检查SIM卡")
#endif

#if USE_LANGUAGE == 11
#define LANG_NETSEARCH_FAIL ("Network search Failure,Please Check SIM Card")
#endif


#define MIN_GPRS_SIGNAL		5				// 能拨GPRS的最小信号值
#define MIN_RESET_TIME		12*3600*1000	// 最小复位时间
#define SMS_POP_COUNT		10


static char	MapTable[64]={   
	0x2e,0x2c,0x30,0x31,0x32,0x33,0x34,0x35,	0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,					
	0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,	0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,						
	0x55,0x56,0x57,0x58,0x59,0x5A,0x61,0x62,	0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,	
	0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,	0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A 
};

static uchar CrcTable[256] = {
	0x00,0x91,0xE3,0x72,0x07,0x96,0xE4,0x75,	0x0E,0x9F,0xED,0x7C,0x09,0x98,0xEA,0x7B,
	0x1C,0x8D,0xFF,0x6E,0x1B,0x8A,0xF8,0x69,	0x12,0x83,0xF1,0x60,0x15,0x84,0xF6,0x67,
	0x38,0xA9,0xDB,0x4A,0x3F,0xAE,0xDC,0x4D,	0x36,0xA7,0xD5,0x44,0x31,0xA0,0xD2,0x43,
	0x24,0xB5,0xC7,0x56,0x23,0xB2,0xC0,0x51,	0x2A,0xBB,0xC9,0x58,0x2D,0xBC,0xCE,0x5F,

	0x70,0xE1,0x93,0x02,0x77,0xE6,0x94,0x05,	0x7E,0xEF,0x9D,0x0C,0x79,0xE8,0x9A,0x0B,
	0x6C,0xFD,0x8F,0x1E,0x6B,0xFA,0x88,0x19,	0x62,0xF3,0x81,0x10,0x65,0xF4,0x86,0x17,
	0x48,0xD9,0xAB,0x3A,0x4F,0xDE,0xAC,0x3D,	0x46,0xD7,0xA5,0x34,0x41,0xD0,0xA2,0x33,
	0x54,0xC5,0xB7,0x26,0x53,0xC2,0xB0,0x21,	0x5A,0xCB,0xB9,0x28,0x5D,0xCC,0xBE,0x2F,

	0xE0,0x71,0x03,0x92,0xE7,0x76,0x04,0x95,	0xEE,0x7F,0x0D,0x9C,0xE9,0x78,0x0A,0x9B,
	0xFC,0x6D,0x1F,0x8E,0xFB,0x6A,0x18,0x89,	0xF2,0x63,0x11,0x80,0xF5,0x64,0x16,0x87,
	0xD8,0x49,0x3B,0xAA,0xDF,0x4E,0x3C,0xAD,	0xD6,0x47,0x35,0xA4,0xD1,0x40,0x32,0xA3,
	0xC4,0x55,0x27,0xB6,0xC3,0x52,0x20,0xB1,	0xCA,0x5B,0x29,0xB8,0xCD,0x5C,0x2E,0xBF,

	0x90,0x01,0x73,0xE2,0x97,0x06,0x74,0xE5,	0x9E,0x0F,0x7D,0xEC,0x99,0x08,0x7A,0xEB,
	0x8C,0x1D,0x6F,0xFE,0x8B,0x1A,0x68,0xF9,	0x82,0x13,0x61,0xF0,0x85,0x14,0x66,0xF7,
	0xA8,0x39,0x4B,0xDA,0xAF,0x3E,0x4C,0xDD,	0xA6,0x37,0x45,0xD4,0xA1,0x30,0x42,0xD3,
	0xB4,0x25,0x57,0xC6,0xB3,0x22,0x50,0xC1,	0xBA,0x2B,0x59,0xC8,0xBD,0x2C,0x5E,0xCF
};


void G_EVDO_PppSigHandler(int v_signo)
{
	G_RecordDebugInfo("Dial Net succ!");

	PRTMSG(MSG_NOR,"收到ppp拨号成功信号通知\n");

	// 控制指示灯
#if VEHICLE_TYPE == VEHICLE_M
	g_objLightCtrl.SetLightSta(0x03);	// 慢闪
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	g_objLightCtrl2.SetLightSta(0x01);	// 拨号成功通知
#endif
	
	g_objPhoneEvdo._PppSigHandler();
}

unsigned long G_EVDO_PhoneHdl( void* v_pPar )
{
	if( v_pPar )
		return ((CPhoneEvdo*)v_pPar)->P_PhoneHdl();
	else
		return ERR_THREAD;
}

unsigned long G_EVDO_OtherHdl( void* v_pPar )
{
	if( v_pPar )
		return ((CPhoneEvdo*)v_pPar)->P_OtherHdl();
	else
		return ERR_THREAD;
}

unsigned long G_EVDO_ComRcv( void* v_pPar )
{
	if( v_pPar )
		return ((CPhoneEvdo*)v_pPar)->P_ComRcv();
	else
		return ERR_THREAD;
}
//暂不实现ping操作
// unsigned long G_EVDO_Ping(void* v_pPar)
// {
// 	if( v_pPar )
// 		return ((CPhoneEvdo*)v_pPar)->P_Ping();
// 	else
// 		return ERR_THREAD;
// }

bool G_EVDO_JugNormalAsc( char v_cToJug )
{
	return (v_cToJug >= '0' && v_cToJug <= '9')
		|| (v_cToJug >= 'a' && v_cToJug <= 'z')
		|| (v_cToJug >= 'A' && v_cToJug <= 'Z');
}

int G_EVDO_GSN_Jugres(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// AT+CGSN
//
// +GSN:a3daaaeb
//
// OK
	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return EVDO_ATANS_OTHER;
	}
	const char STR_RESSYMB[] = "+GSN:";
	if( strstr(v_szAtRes, "ERROR") )
	{
		return EVDO_ATANS_ERROR;
	}

	else if( strstr(v_szAtRes, "OK") )
	{
		char* pAns = strstr(v_szAtRes, STR_RESSYMB);
		if( pAns )
		{
			// 寻找起始判断的位置
			pAns += strlen( STR_RESSYMB ); // 跳过应答标志
			while( 0x20 == *pAns && 0 != *pAns ) // 跳过之后的空格
			{
				pAns ++;
			}

			// 读取IMEI
			if( *pAns )
			{
				DWORD dwLen = 0;
				while( dwLen + 1 < v_sizResult )
				{
					if( !G_EVDO_JugNormalAsc(pAns[dwLen]) ) // 直到非普通ASCII字符
					{
						break;
					}

					((char*)v_pResult)[dwLen] = pAns[dwLen];
					dwLen ++;
				}
				if( dwLen < v_sizResult )
				{
					((char*)v_pResult)[dwLen] = 0;
				}

				return EVDO_ATANS_OK;
			}
			else
			{
				return EVDO_ATANS_OTHER;
			}
		}
		else
		{
			return EVDO_ATANS_OTHER;
		}
	}
	else
	{
		return EVDO_ATANS_RCVNOTALL;
	}
}

int G_EVDO_CSQ_Jugres(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// AT+CSQ
// 
// +CSQ: 11,99
// 
// OK

	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return EVDO_ATANS_OTHER;
	}

	const char STR_RESSYMB[] = "+CSQ:";
	if( strstr(v_szAtRes, "ERROR") )
	{
		return EVDO_ATANS_ERROR;
	}
	else if( strstr(v_szAtRes, "OK") )
	{
		char* pAns = strstr(v_szAtRes, STR_RESSYMB);
		if( pAns )
		{
			// 寻找起始判断的位置
			pAns += strlen( STR_RESSYMB ); // 跳过应答标志
			while( 0x20 == *pAns && 0 != *pAns ) // 跳过之后的空格
			{
				pAns ++;
			}

			// 读取信号
			if( *pAns )
			{
				char* pEnd = strstr(pAns, ",");
				if( pEnd )
				{
					size_t sizSingalLen = pEnd - pAns;
					if( sizSingalLen + 1 > v_sizResult )
					{
						return EVDO_ATANS_OTHER;
					}
					else
					{
						strncpy((char*)v_pResult, pAns, sizSingalLen );
						((char*)v_pResult)[ sizSingalLen ] = 0;
						return EVDO_ATANS_OK;
					}
				}
				else
				{
					return EVDO_ATANS_OTHER;
				}
			}
			else
			{
				return EVDO_ATANS_OTHER;
			}
		}
		else
		{
			return EVDO_ATANS_OTHER;
		}
	}
	else
	{
		return EVDO_ATANS_RCVNOTALL;
	}
}

// que 短信中心号，703at？
int G_EVDO_CSCA_Query_res(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// AT+CSCA?
// 
// +CSCA: "+8613800592500",145
// 
// OK

	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return EVDO_ATANS_OTHER;
	}

	const char STR_RESSYMB[] = "+CSCA:";
	if( strstr(v_szAtRes, "ERROR") )
	{
		return EVDO_ATANS_ERROR;
	}
	else if( strstr(v_szAtRes, "OK") )
	{
		char* pAns = strstr(v_szAtRes, STR_RESSYMB);
		if( pAns )
		{
			pAns += strlen(STR_RESSYMB); // 跳过应答标志

			// 寻找起始位置
			char* pBegin = strstr(pAns, "\"");
			if( !pBegin ) return EVDO_ATANS_OTHER;
			if( pBegin - pAns > 3 ) return EVDO_ATANS_OTHER;

			// 结束位置
			char* pEnd = strstr(pBegin + 1, "\"");
			if( !pEnd ) return EVDO_ATANS_OTHER;
			if( pEnd - pBegin > 15 ) return EVDO_ATANS_OTHER;

			DWORD dwCscaLen = min(DWORD(pEnd - pBegin - 1), v_sizResult - 1);
			memcpy( v_pResult, pBegin + 1, dwCscaLen );
			((char*)v_pResult)[ dwCscaLen ] = 0;
			
			return EVDO_ATANS_OK;
		}
		else
		{
			return EVDO_ATANS_OTHER;
		}
	}
	else
	{
		return EVDO_ATANS_RCVNOTALL;
	}
}

//来电号码显示指示 CLIP 
int G_EVDO_CLIP_ReadTel(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// RING    
// 
// +CLIP: "05922956773",161,"",,"",0  
//+CLIP:15980856182,0,,,,0 

	int iRet = EVDO_ATANS_NOANS;
	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return EVDO_ATANS_OTHER;
	}
	
	const char STR_RESSYMB[] = "+CLIP:";
	char* pClipBegin = NULL;

	if( strstr(v_szAtRes, "RING") )
	{
		if( pClipBegin = strstr(v_szAtRes, STR_RESSYMB) )
		{
			pClipBegin += strlen(STR_RESSYMB); // 跳过应答标志

			// 寻找起始位置

			char* pBegin = pClipBegin;
			while( 0x20 == *pBegin ) // 跳过之后的空格
			{
				pBegin ++;
			}
			if( !pBegin ) return EVDO_ATANS_OTHER;
			if( pBegin - pClipBegin > 3 ) return EVDO_ATANS_OTHER;

			// 寻找终止位置,逗号标志
			char* pEnd = strchr( pBegin + 1, ',' );
			if( !pEnd ) return EVDO_ATANS_OTHER;
			if( pEnd - pBegin > 20 ) return EVDO_ATANS_OTHER;
			DWORD dwTelLen = min( DWORD(pEnd - pBegin ), v_sizResult - 1 );
			memcpy( v_pResult, pBegin, dwTelLen );
			((char*)v_pResult)[ dwTelLen ] = 0;

			iRet = EVDO_ATANS_OK;
		}
		else
		{
			iRet = EVDO_ATANS_RCVNOTALL;	//无来电显示
		}

		// 判断是否有DSCI指示 (待)
	}
	else
	{
		iRet = EVDO_ATANS_NOANS;
	}
	return iRet;
}

//呼叫状态查询命令AT+CLCC，此处用来提取来电号码,通过逗号标志提取
int G_EVDO_CLCC_ReadTel(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// at+clcc       
// +CLCC:1,1,4,0,0,05922956774,255,255,255
// 
// OK

	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return EVDO_ATANS_OTHER;
	}
	
	const char STR_RESSYMB[] = "+CLCC:";
	char* pClccBegin = NULL;

	if( strstr(v_szAtRes, "ERROR") )
	{
		return EVDO_ATANS_ERROR;
	}
	else if( strstr(v_szAtRes, "OK") )
	{
		if( pClccBegin = strstr(v_szAtRes, STR_RESSYMB) )
		{
			pClccBegin += strlen(STR_RESSYMB); // 跳过应答标志
			char* pBegin;
			char* pEnd;
			char* pResEnd;
			int i;
			pBegin = pClccBegin;
			for( i=0; i<5; i++)
			{
				// 寻找起始位置
				pBegin = strchr(pBegin, ',');
				if( !pBegin ) 
				{
					return EVDO_ATANS_OTHER;
				}
				pBegin++;
			}
			
			while( 0x20 == *pBegin && 0 != *pBegin ) // 跳过之后的空格
			{
				pBegin ++;
			}

			// 寻找终止位置
			pEnd = strchr( pBegin, ',' );
			if( !pEnd ) 
			{
				return EVDO_ATANS_OTHER;
			}
			pResEnd = strstr(pClccBegin, "\r");
			if( pEnd - pBegin > 20 || pResEnd < pEnd ) 
			{
				return EVDO_ATANS_OTHER;
			}

			DWORD dwTelLen = min( DWORD(pEnd - pBegin), v_sizResult - 1 );
			memcpy( v_pResult, pBegin, dwTelLen );
			((char*)v_pResult)[ dwTelLen ] = 0;
			return EVDO_ATANS_OK;
		}
		else
		{
			return EVDO_ATANS_NOANS;
		}
	}
	else
	{
		return EVDO_ATANS_RCVNOTALL;
	}
}

//提取短信索引
int G_EVDO_CMGD_SmsList(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	// AT+CMGD=?      
	// +CMGD:(0,1,2,3,4,5,6,8)  //短信列表        
	// +CMGD:()  //无短信 
	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return EVDO_ATANS_OTHER;
	}
	
	const char STR_RESSYMB[] = "+CMGD:";
	char* pCmgdBegin = NULL;
	char sztemp[10] = {0};
	
	if( strstr(v_szAtRes, "ERROR") )
	{
		return EVDO_ATANS_ERROR;
	}

	if( pCmgdBegin = strstr(v_szAtRes, STR_RESSYMB) )
	{
		pCmgdBegin += strlen(STR_RESSYMB); // 跳过应答标志
		char* pIndex;
		char* pEnd;
		char* pResEnd;
		char* pIndex1;
		int i = 0;
		pIndex = pCmgdBegin;

		// 寻找终止位置
		pEnd = strchr( pCmgdBegin, ')' );
		if( !pEnd ) 
		{
			return EVDO_ATANS_OTHER;
		}

		pResEnd = strstr(pCmgdBegin, "\r");
		if( pResEnd < pEnd )
		{
			return EVDO_ATANS_OTHER;
		}

		pIndex = strchr(pCmgdBegin, '(' );

		pIndex1 = strchr(pIndex, ',');
		
		if( (pEnd - pIndex > 1) && !pIndex1 )
		{
			int len = pEnd - pIndex;
			memset(sztemp, 0, sizeof(sztemp));
			memcpy(sztemp, pIndex+1, len-1);
			
			((int*)v_pResult)[i++] = atoi(sztemp);
			
			pIndex += len;
		}
		
		for(; pIndex<pEnd ;)
		{
			pIndex1 = strchr(pIndex, ',');
			
			if( !pIndex1 || (pIndex1 > pEnd) ) 
			{
				return EVDO_ATANS_OK;
			}
			
			memset(sztemp, 0, sizeof(sztemp));
			memcpy(sztemp, pIndex, pIndex1-pIndex);
			
			((int*)v_pResult)[i++] = atoi(sztemp);
			
			pIndex = pIndex1+1;
			while( 0x20 == *pIndex ) // 跳过之后的空格
			{
				pIndex ++;
			}
		}

// 		if ( 1 != (pEnd - pIndex) )
// 		{
// 			((char*)v_pResult)[i++] = *(pIndex+1);
// 		}
// 		
// 		do
// 		{
// 			pIndex = strchr(pIndex, ',' );
// 			if( !pIndex || (pIndex > pEnd) ) 
// 			{
// 				return EVDO_ATANS_OK;
// 			}
// 			pIndex++;
// 			
// 			while( 0x20 == *pIndex && 0 != *pIndex ) // 跳过之后的空格
// 			{
// 				pIndex ++;
// 			}
// 			((char*)v_pResult)[i++] = *pIndex;
// 		}while(pIndex < pEnd);

		((int*)v_pResult)[i] = -1;
		return EVDO_ATANS_OK;
	}
	else
	{
		return EVDO_ATANS_NOANS;
	}

}

//检测是否在振铃状态，用于判断挂机
int G_EVDO_CLCCJugIncoming(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	if( !v_szAtRes )	//	判断需传入的参数
	{
		return EVDO_ATANS_OTHER;
	}
	
	const char STR_RESSYMB[] = "+CLCC:";
	char* pClccBegin = NULL;
	
	if( strstr(v_szAtRes, "ERROR") )
	{
		return EVDO_ATANS_ERROR;
	}
	else if( strstr(v_szAtRes, "OK") )
	{
		if( pClccBegin = strstr(v_szAtRes, STR_RESSYMB) )
		{
			pClccBegin += strlen(STR_RESSYMB); // 跳过应答标志
			char* pTarget = pClccBegin;
			// 寻找逗号标志

			for(int i=0; i<2; i++ )
			{
				pTarget = strchr(pTarget, ',');
				// 寻找逗号标志
				if( !pTarget ) 
				{
					return EVDO_ATANS_OTHER;
				}					
				pTarget++;
			}
			while( 0x20 == *pTarget && 0 != *pTarget ) // 跳过之后的空格
			{
				pTarget ++;
			}			
			char* pResEnd = strstr(pClccBegin, "\r");
			if( pResEnd < pTarget ) return EVDO_ATANS_OTHER;

			if( !pTarget ) return EVDO_ATANS_OTHER;
			
			if('4' != *pTarget)
			{
				return EVDO_ATANS_OTHER;
			}
			else
				return EVDO_ATANS_OK;
		}
		else
		{
			return EVDO_ATANS_NOANS;
		}
	}
	else
	{
		return EVDO_ATANS_RCVNOTALL;
	}
}

int G_EVDO_CLCCGetId(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return EVDO_ATANS_OTHER;
	}
	
	const char STR_RESSYMB[] = "+CLCC:";
	char* pClccBegin = NULL;
	
	if( strstr(v_szAtRes, "ERROR") )
	{
		return EVDO_ATANS_ERROR;
	}
	else if( strstr(v_szAtRes, "OK") )
	{
		if( pClccBegin = strstr(v_szAtRes, STR_RESSYMB) )
		{
			pClccBegin += strlen(STR_RESSYMB); // 跳过应答标志
			char* pTarget = pClccBegin;
			
			while( 0x20 == *pTarget ) // 跳过之后的空格
			{
				pTarget ++;
			}
			if( G_EVDO_JugNormalAsc( *pTarget ) )
			{
				((char*)v_pResult)[0] = *pTarget;
			}

			char* pResEnd = strstr(pClccBegin, "\r");
			if( pResEnd < pTarget ) return EVDO_ATANS_OTHER;
			
			if( !pTarget ) return EVDO_ATANS_OTHER;
			return EVDO_ATANS_OK;
		}
		else
		{
			return EVDO_ATANS_NOANS;
		}
	}
	else
	{
		return EVDO_ATANS_RCVNOTALL;
}
}


//拨通后模块是无法判断对方有没有接听的，这是由目前的网
//络状态决定，在中国GSM模块如果对方接听，会返回OK，但CDMA网络里
//没有相对应的提示

//呼叫接通后，通话状态上报
int G_EVDO_JugRemotePick(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// ^CONN:<call_x>,<call_type>  

	if( strstr(v_szAtRes, "ERROR") )
		return EVDO_ATANS_ERROR;
	else if( strstr(v_szAtRes, "NO CARRIER") )
		return EVDO_ATANS_NOCARRIER;
	else if( strstr(v_szAtRes, "NO DIALTONE") )
		return EVDO_ATANS_NODIALTONE;
	else if( strstr(v_szAtRes, "BUSY") )
		return EVDO_ATANS_BUSY;
	else if( strstr(v_szAtRes, "NO ANSWER") )
		return EVDO_ATANS_NOANSWER;
	else if( strstr(v_szAtRes, "CONN") )
		return EVDO_ATANS_OK;
	else
		return EVDO_ATANS_NOANS;
}

//判断挂断,模块主动上报^CEND:表示对方挂断
//^CEND:1,82,29
int G_EVDO_JugRemoteHangup(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	if( strstr(v_szAtRes, "CEND:") )
		return EVDO_ATANS_OK;
	else if( strstr(v_szAtRes, "NO CARRIER") )
		return EVDO_ATANS_NOCARRIER;
	else if( strstr(v_szAtRes, "NO ERROR") )
		return EVDO_ATANS_ERROR;
// 	else if( strstr(v_szAtRes, "NO DIALTONE") )
// 		return EVDO_ATANS_NODIALTONE;
// 	else if( strstr(v_szAtRes, "BUSY") )
// 		return EVDO_ATANS_BUSY;
// 	else if( strstr(v_szAtRes, "NO ANSWER") )
// 		return EVDO_ATANS_NOANSWER;
// 	else if( strstr(v_szAtRes, "COLP") )
// 		return EVDO_ATANS_OK;
// 	else
// 		return EVDO_ATANS_NOANS;
	else
		return EVDO_ATANS_OTHER;
}

//呼叫状态查询命令,此处用来查询是否在通话状态
//空闲状态会返回OK
//+CLCC:2,0,0,0,0,15980856182,255,255,255 
int G_EVDO_CLCC_JugRes(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// +CLCC: 1,0,2,0,0,"1305526111",129                                 
// OK

	const char STR_RESSYMB[] = "+CLCC:";
	if( strstr(v_szAtRes, "OK") && strstr(v_szAtRes, STR_RESSYMB) )
	{
		return EVDO_ATANS_OK;
	}
	else
	{
		return EVDO_ATANS_NOCALL;
	}
}

//短信发送判断，收到 > 即可发送短信内容
int G_EVDO_HCMGS_JugRes(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	if( strstr(v_szAtRes, "ERROR") )
		return EVDO_ATANS_ERROR;
	else if( strstr(v_szAtRes, ">") )
		return EVDO_ATANS_OK;
	else
		return EVDO_ATANS_NOANSWER;
}

//短信内容发送成功/失败判断
int G_EVDO_HCMGS_JugSend(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
//OK
//^HCMGSS:31 //短信发送成功
	const char STR_RESSYMB[] = "^HCMGSS:";
	if( strstr(v_szAtRes, "ERROR") )
		return EVDO_ATANS_ERROR;
	else if( strstr(v_szAtRes, "OK") || strstr(v_szAtRes, STR_RESSYMB) )
		return EVDO_ATANS_OK;
	else
		return EVDO_ATANS_NOANSWER;
}

//短信内容读取成功/失败判断
int G_EVDO_HCMGR_JugSend(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	//OK
	//^HCMGSS:31 //短信发送成功
	const char STR_RESSYMB[] = "^HCMGR:";
	if( strstr(v_szAtRes, "ERROR") )
		return EVDO_ATANS_ERROR;
	else if( strstr(v_szAtRes, "OK") || strstr(v_szAtRes, STR_RESSYMB) )
		return EVDO_ATANS_OK;
	else
		return EVDO_ATANS_NOANSWER;
}

//evdo,R-UIM 卡在位和PIN1 码状态查询
int G_EVDO_CPIN_Jugres(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{	
//AT+CPIN?
//+CPIN: READY
	if(strstr(v_szAtRes, "+CPIN"))
	{
		if( strstr(v_szAtRes, "READY") || strstr(v_szAtRes, "OK") )
		{
			return EVDO_ATANS_OK;
		}
	}
	else
	{
		return EVDO_ATANS_ERROR;
	}
}

//PPP拨号用户密码查询
int G_EVDO_CPPPCFG_Jugres(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{	
//	AT^PPPCFG?
//	^PPPCFG:"ctnet@mycdma.cn","vnet.mobi"
//	^PPPCFG:"card","card"
	char *pRetBegin = NULL;
	char *pRetEnd = NULL;
	pRetBegin = strstr(v_szAtRes, "^PPPCFG:");
	if(NULL != pRetBegin)
	{
		pRetEnd = strstr(pRetBegin, "\r");
		if ( (NULL == pRetEnd) || (v_sizResult<(pRetEnd-pRetBegin)) )
		{
			return EVDO_ATANS_ERROR;
		}
		memcpy(v_pResult, v_szAtRes, (pRetEnd-v_szAtRes) );
		return EVDO_ATANS_OK;
	}
	else
	{
		return EVDO_ATANS_ERROR;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPhoneEvdo::CPhoneEvdo()
{
	m_iPhonePort = -1;

	m_hThdComRcv = 0;//初始化线程句柄
	m_hThdPhoneHdl = 0;
	m_hThdOtherHdl = 0;
//	m_hThdPing = 0;



	m_objDataMngRcv.InitCfg( 60000, 60000 );
	m_objDataMngSmsSnd.InitCfg(80000, 600000);

// !!注意以下环境状态变量均不要memset,他们有各自的构造函数初始化
// 	memset( &m_objPhoneStaDial, 0, sizeof(m_objPhoneStaDial) );
// 	memset( &m_objPhoneStaDialLsn, 0, sizeof(m_objPhoneStaDialLsn) );
// 	memset( &m_objPhoneStaHold, 0, sizeof(m_objPhoneStaHold) );
// 	memset( &m_objPhoneStaLsning, 0, sizeof(m_objPhoneStaLsning) );
// 	memset( &m_objPhoneStaRing, 0, sizeof(m_objPhoneStaRing) );
// 
// 	memset( &m_objPhoneStaCommon, 0, sizeof(m_objPhoneStaCommon) );

	m_dwClrDogTm = GetTickCount();
	m_iPhoneHdlSta = EVDO_PST_INIT;
	m_bPhoneNetReady = false;
	m_bToClosePhone = false;
	
//	InitPhone();
}

CPhoneEvdo::~CPhoneEvdo()
{

}

void CPhoneEvdo::ResetPhone()
{
	tagEvdoPhoneTask objPhoneTask;
	_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_RESET );
}

// evdo硬件复位，rst引脚拉低3s后拉高
// void CPhoneEvdo::_GsmHardReset()
// {
// 	IOSet(IOS_PHONERST, IO_PHONERST_LO, NULL, 0);		//RESET	
// 	sleep(3);//拉低3s
// 	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET	
// 	sleep(2);
// }

// evdo硬件复位，rst引脚拉低3s后拉高
void CPhoneEvdo::_GsmHardReset()
{
 	_GsmPowOff();
 	sleep(2);//拉低3s
 	IOSet(IOS_PHONEPOW, IO_PHONEPOW_ON, NULL, 0);
 	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET	
 	_GsmPowOn();
}

//evdo上电
void CPhoneEvdo::_GsmPowOn()
{
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET	
	sleep(3);
	IOSet(IOS_PHONERST, IO_PHONERST_LO, NULL, 0);		//RESET	
	sleep(1);		//more than 300ms
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET	
	sleep(3);
	PRTMSG(MSG_DBG,"EVDO power on!\n");
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	RenewMemInfo(0x02, 0x02, 0x01, 0x00);
#endif
}

void CPhoneEvdo::_GsmPowOff()
{
	sleep(1);
	IOSet(IOS_PHONERST, IO_PHONERST_LO, NULL, 0);		//RESET	
	sleep(3);		//3s
	IOSet(IOS_PHONEPOW, IO_PHONEPOW_OFF, NULL, 0);		//power	
	PRTMSG(MSG_DBG,"EVDO power off!\n");
}

void CPhoneEvdo::ReDialNet()
{
	tagEvdoPhoneTask objPhoneTask;
	_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_CUTNET );
}

// 返回值: 0,表示不在线; 1,表示已注册网络; 2,表示已拨号上网
BYTE CPhoneEvdo::GetPhoneState()
{
	switch( m_iPhoneHdlSta )
	{
	case EVDO_PST_INIT:
	case EVDO_PST_RESET:
		return 0;
		break;

	default:
		if( m_bPhoneNetReady ) return 2;
		else return 1;
	}
	return 0;
}

int CPhoneEvdo::GetPhoneHdlSta()
{
	return m_iPhoneHdlSta;
}

int CPhoneEvdo::GetPhoneSignal()
{
	tagEvdoPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	return objPhoneSta.m_iSignal;
}

void CPhoneEvdo::GetPhoneIMEI( char* v_szImei, size_t v_sizImei )
{
	if( v_sizImei < 1 ) return;
	tagEvdoPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	memset( v_szImei, 0, sizeof(v_sizImei) );
	memcpy( v_szImei, objPhoneSta.m_szGsmIMEI, min(v_sizImei, strlen(objPhoneSta.m_szGsmIMEI)) );
	v_szImei[ v_sizImei - 1 ] = 0;
}

int CPhoneEvdo::GetPhoneVolume()
{
	tagEvdoPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	return objPhoneSta.m_iAudioOutVol;
}

unsigned long CPhoneEvdo::P_PhoneHdl()
{
	usleep(200000);
	PRTMSG(MSG_DBG,"**********\n");
	usleep(200000);
	m_iPhoneHdlSta = EVDO_PST_INIT;

	sleep(5);
	_GsmPowOn();
	sleep(5);
	
	_InitCom();	
	
	while(!g_bProgExit) 
	{
		// 该函数内部有休眠
		_PhoneHdl();
	}

	g_bProgExit = true;
	pthread_exit(NULL);

	return 0;
}

//手机模块读线程
unsigned long CPhoneEvdo::P_ComRcv()
{
	int iReaded = 0;
	char szRcv[2048] = {0};
	DWORD dwCurTm = 0;
	DWORD dwPktNum = 0;

	int iResult;

	while(!g_bProgExit)
	{
		iResult = read(m_iPhonePort, szRcv, sizeof(szRcv));	//阻塞读
		if( iResult > 0 )
		{
			dwCurTm = GetTickCount();
			
			if( iResult > sizeof(szRcv) )
			{
				PRTMSG(MSG_ERR,"Com Read Over Buf!\n");
				memset(szRcv, 0, sizeof(szRcv));
				continue;
			}

			szRcv[ sizeof(szRcv) - 1 ] = 0;
			//PRTMSG(MSG_DBG, "PCom Rcv:%s\n", szRcv );

			//注意push的时候，不能以字符串结束符做判断，unicode数据中有0x00
			//que lvl 5->1
//			m_objDataMngRcv.PushData( 1, strlen(szRcv), szRcv, dwPktNum, 0, (char*)&dwCurTm, sizeof(dwCurTm) );
			m_objDataMngRcv.PushData( 1, (DWORD)iResult, szRcv, dwPktNum, 0, (char*)&dwCurTm, sizeof(dwCurTm) );

			memset(szRcv, 0, sizeof(szRcv));
		}
		else
		{
			usleep(20000);
		}
	}

	g_bProgExit = true;
	
	return false;
}

unsigned long CPhoneEvdo::P_OtherHdl()
{
	int iResult;

	while(!g_bProgExit)
	{
		_OtherHdl();
		usleep(20000);//que 怎么没有sleep？
	}
	g_bProgExit = true;
	
	return 0;
}
/*
//que暂不实现
unsigned long CPhoneEvdo::P_Ping()
{
	int iResult = 0; // 0,成功; 1,失败; 2,其他异常; 3,被中止

	HANDLE hIcmp = NULL;
	DWORD ret = 0;
	DWORD ip_serv = 0;
	hostent* phostent = NULL;
	char reply[sizeof(icmp_echo_reply)+8];
	icmp_echo_reply* iep = (icmp_echo_reply*)&reply; 
	iep->RoundTripTime = 0xffffffff;

	hIcmp = IcmpCreateFile();
	if( INVALID_HANDLE_VALUE == hIcmp )
	{
		iResult = 2;
		goto PING_END;
	}

	// 获取网关IP 并进行ping操作
	{
		DWORD dwRetVal = 0;
		DWORD ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);   
		IP_ADAPTER_INFO *pAdapter;
		if(pAdapterInfo==NULL)
		{
			iResult = 2;
			goto PING_END;
		}
		
		if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)==ERROR_BUFFER_OVERFLOW)
		{   
			free(pAdapterInfo); 
			pAdapterInfo = NULL;
			pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen); 
			if(pAdapterInfo==NULL)
			{
				iResult = 2;
				goto PING_END;
			}
		}  
    
		if( (dwRetVal=GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR )
		{  
			pAdapter = pAdapterInfo; 	
			while(pAdapter)
			{
				if( WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtEnd, 0)
					||				
					WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtPingExit, 0))
				{
					iResult = 3;
					goto PING_END;
				}
				char *usbname = strstr(pAdapter->AdapterName, "SC2440 USB Cable");
				char *usbip = strstr(pAdapter->GatewayList.IpAddress.String, "192.168.55.100");
				if(usbname==NULL && usbip==NULL)
				{
					Sleep(2000);

					if( WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtEnd, 0)
						||				
						WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtPingExit, 0))
					{
						iResult = 3;
						goto PING_END;
					}

					ip_serv = inet_addr(pAdapter->GatewayList.IpAddress.String);
					ret = IcmpSendEcho(hIcmp,ip_serv,0,0,NULL,reply,sizeof(icmp_echo_reply)+8,2000);
					if(ret!=0)
					{ 
					//	show_msg("ping gateway succ!");
						free(pAdapterInfo); 
						iResult = 0;
						goto PING_END;
					}
				}
				pAdapter = pAdapter->Next;   
			}   			
		}
		free(pAdapterInfo);
	}

	if( WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtEnd, 0)
		||				
		WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtPingExit, 0))
	{
		iResult = 3;
		goto PING_END;
	}
	else
	{
		::Sleep( 2000 );
	}

	// 获取dns服务器IP 并进行ping操作
	{
		DWORD dwRetVal = 0;
		DWORD ulOutBufLen = sizeof(FIXED_INFO);
		FIXED_INFO *pFixedInfo = (FIXED_INFO*)malloc(ulOutBufLen);   
		IP_ADDR_STRING *pIPAddr;
		if(pFixedInfo==NULL)
		{
			iResult = 2;
			goto PING_END;
		}
		 
		if(GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
		{   
			free(pFixedInfo);  
			pFixedInfo = NULL;	
			pFixedInfo = (FIXED_INFO*)malloc(ulOutBufLen); 
			if(pFixedInfo==NULL)
			{
				iResult = 2;
				goto PING_END;
			}
		}   
    
		if(dwRetVal=GetNetworkParams(pFixedInfo, &ulOutBufLen) == NO_ERROR) 
		{
			ip_serv = inet_addr(pFixedInfo->DnsServerList.IpAddress.String);
			if(ip_serv != 0x0100007f)
			{
				if( WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtEnd, 0)
					||				
					WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtPingExit, 0))
				{
					iResult = 3;
					goto PING_END;
				}
				
				ret = IcmpSendEcho(hIcmp,ip_serv,0,0,NULL,reply,sizeof(icmp_echo_reply)+8,2000);
				if(ret!=0)
				{ 
				//	show_msg("ping dns succ!");
					free(pFixedInfo); 
					iResult = 0;
					goto PING_END;
				}
			}
			pIPAddr = pFixedInfo->DnsServerList.Next;   
			while(pIPAddr)
			{  
				if( WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtEnd, 0)
					||				
					WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtPingExit, 0))
				{
					iResult = 3;
					goto PING_END;
				}

				ip_serv = inet_addr(pIPAddr->IpAddress.String);
				if(ip_serv != 0x0100007f)
				{
					Sleep(2000);

					if( WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtEnd, 0)
						||				
						WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtPingExit, 0))
					{
						iResult = 3;
						goto PING_END;
					}
					
					ret = IcmpSendEcho(hIcmp,ip_serv,0,0,NULL,reply,sizeof(icmp_echo_reply)+8,2000);
					if(ret!=0)
					{ 
					//	show_msg("ping dns succ!");
						free(pFixedInfo); 
						iResult = 0;
						goto PING_END;
					}
				}
				pIPAddr = pIPAddr->Next; 
			} 
		}  
		free(pFixedInfo);
	}
 
	if( WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtEnd, 0)
		||				
		WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtPingExit, 0))
	{
		iResult = 3;
		goto PING_END;
	}
	else
	{
		::Sleep(2000);
	}

	if( WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtEnd, 0)
		||				
		WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtPingExit, 0))
	{
		iResult = 3;
		goto PING_END;
	}

	// 获取 163 IP 并进行ping操作 
	phostent = gethostbyname("www.163.com");
	ip_serv = (phostent==NULL) ? (0) : (*(DWORD*)(*phostent->h_addr_list));
	ret = IcmpSendEcho(hIcmp,ip_serv,0,0,NULL,reply,sizeof(icmp_echo_reply)+8,2000);
	if(ret!=0)
	{ 
		//show_msg("ping 163 succ!");
		iResult = 0;
		goto PING_END;
	}

	if( WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtEnd, 0)
		||				
		WAIT_TIMEOUT == ::WaitForSingleObject(m_hEvtPingExit, 0))
	{
		iResult = 3;
		goto PING_END;
	}

	// 获取 Sina IP 并进行ping操作 
	phostent = gethostbyname("www.sina.com.cn");
	ip_serv = (phostent==NULL) ? (0) : (*(DWORD*)(*phostent->h_addr_list));
	ret = IcmpSendEcho(hIcmp,ip_serv,0,0,NULL,reply,sizeof(icmp_echo_reply)+8,2000);
	if(ret!=0)
	{ 
		//show_msg("ping sina succ!"); 
		iResult = 0;
		goto PING_END;
	}

	iResult = 1;
PING_END:
	if( hIcmp && INVALID_HANDLE_VALUE != hIcmp )
	{
		IcmpCloseHandle( hIcmp );
	}

	if( 0 == iResult )
	{
		show_msg("ping succ!");
	}
	else if( 3 == iResult )
	{
		show_msg("ping is canceled");
	}
	else
	{
		show_msg("ping fail!");

		// 要求断网
		tagEvdoPhoneTask objPhoneTask;
		_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_CUTNET );
	}

	tagEvdoPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	tagEvdoPhoneTask objPhoneTask;
	objPhoneTask.m_bytPingResult = iResult;
	objPhoneTask.m_bytPingSrc = objPhoneSta.m_bytPingSrc;
	_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_PINGRPT );

	return 0;
}
*/
int CPhoneEvdo::InitPhone()
{
	int iResult;

	//初始化互斥量
	if((0 != pthread_mutex_init( &m_crisecRcv, NULL)) || (0 != pthread_mutex_init( &m_crisecPhoneTask, NULL)) ||
		(0 != pthread_mutex_init( &m_crisecSta, NULL)) || (0 != pthread_mutex_init( &m_crisecDog, NULL)))
	{
		PRTMSG(MSG_ERR,"初始化互斥量失败！\n");
		goto _INITPHONE_FAIL;
	}

#if VEHICLE_TYPE == VEHICLE_M
	IOSet(IOS_HUBPOW, IO_HUBPOW_ON, NULL, 0);			//HUB,正式版本不需控制
#endif
	IOSet(IOS_PHONEPOW, IO_PHONEPOW_ON, NULL, 0);		//POWER
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET

	if(system(USB_DEV_INSMOD) != 0)
	{
		PRTMSG(MSG_ERR,"usbserial.ko 加载失败！\n");
	}

	// 创建手机模块串口读取线程
	iResult = pthread_create(&m_hThdComRcv, NULL, (void *(*)(void*))G_EVDO_ComRcv, (void *)this);
	if(0 !=iResult)
	{
		PRTMSG(MSG_ERR,"创建串口读取线程失败！\n");  
		goto _INITPHONE_FAIL;
	}

	// 创建 电话处理 线程
	iResult = pthread_create(&m_hThdPhoneHdl, NULL, (void *(*)(void*))G_EVDO_PhoneHdl, (void *)this);
	if(0 !=iResult)
	{
		PRTMSG(MSG_ERR,"创建电话处理线程失败！\n");  
		goto _INITPHONE_FAIL;
	}

	// 创建 命令处理 线程
	iResult = pthread_create(&m_hThdOtherHdl, NULL, (void *(*)(void*))G_EVDO_OtherHdl, (void *)this);
	if(0 !=iResult)
	{
		PRTMSG(MSG_ERR,"创建命令处理线程失败！\n");  
		goto _INITPHONE_FAIL;
	}

	return true;

_INITPHONE_FAIL:
	ReleasePhone();//ReasePhone中置退出进程标识
	return ERR_THREAD;
}

void CPhoneEvdo::ReleasePhone()
{
	//进程退出标志
#if CHK_VER == 0	
	g_bProgExit = true;
#endif
	// { 同步控制手机模块关闭,使得状态起迅速退出非复位状态进入复位状态,并且在复位状态中仍然能良好关闭
	m_bToClosePhone = true;

	_PhoneCutNet();	
	_CloseCom();

	//销毁互斥量
	pthread_mutex_destroy( &m_crisecRcv );
	pthread_mutex_destroy( &m_crisecPhoneTask );
	pthread_mutex_destroy( &m_crisecSta );
	pthread_mutex_destroy( &m_crisecDog );

	_GsmPowOff();//模块安全关机
}

int CPhoneEvdo::_InitCom()
{
	int iResult;
	struct termios options;
	const int COMOPEN_TRYMAXTIMES = 10;
	char szUsbSerialPathName[256] = {0};
	struct dirent **ppUsbSerialNameList;
	int i;
	for( i = 0; i < COMOPEN_TRYMAXTIMES; i ++ )
	{
#if VEHICLE_TYPE == VEHICLE_M
		iResult = access("/sys/bus/usb/drivers/usbserial_generic/1-2.3:1.0", F_OK);
#endif

#if VEHICLE_TYPE == VEHICLE_M2
		iResult = access("/sys/bus/usb/drivers/usbserial_generic/1-1.2:1.0", F_OK);
#endif
		
#if VEHICLE_TYPE == VEHICLE_V8
		iResult = access("/sys/bus/usb/drivers/usbserial_generic/1-1:1.0", F_OK);
#endif

		if( 0 != iResult )
		{
			PRTMSG(MSG_ERR,"no usb device checked\n");
			G_RecordDebugInfo("MC703 no usb device checked");
			sleep(2);//testc
			continue;
		}

		//目录下文件：
		//bAlternateSetting   bInterfaceSubClass  interface
		//bInterfaceClass     bNumEndpoints       modalias
		//bInterfaceNumber    bus                 ttyUSB0
		//bInterfaceProtocol  driver
		scandir(USB_DEV_CHECKPATH_AT, &ppUsbSerialNameList, 0, alphasort);

		if( (NULL==ppUsbSerialNameList) || (NULL==ppUsbSerialNameList[12]->d_name) || 
			NULL==strstr(ppUsbSerialNameList[12]->d_name,"ttyUSB"))
		{
			PRTMSG(MSG_ERR,"no ttyusb checked\n");
			sleep(1);
			continue;
		}

		sprintf(szUsbSerialPathName, "/dev/usb/tts/%c", *(ppUsbSerialNameList[12]->d_name + 6));
		if( 0 != access(szUsbSerialPathName, F_OK) )
		{
			PRTMSG(MSG_ERR,"usb device port check err\n");
			sleep(1);//testc
			continue;
		}

//		system("mknod /dev/ttyUSB0 c 188 0");
//		m_iPhonePort = open("/dev/usb/tts/2", O_RDWR);
//		m_iPhonePort = open(USB_DEV_PATH, O_RDWR);
		m_iPhonePort = open(szUsbSerialPathName, O_RDWR);
		if( -1 != m_iPhonePort )
		{
			PRTMSG(MSG_DBG,"Open %s succ!\n",szUsbSerialPathName);
			break;
		}

		PRTMSG(MSG_ERR,"Open %s failed!\n",szUsbSerialPathName);
		sleep(1);
	}

	if( i >= COMOPEN_TRYMAXTIMES )
	{
#if CHK_VER == 0
		PRTMSG(MSG_ERR,"usb serial open fail to max times,reset system!\n");
		//g_bProgExit = true;
		G_ResetSystem();
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		g_objDiaodu.show_diaodu("手机模块异常");
		RenewMemInfo(0x02, 0x00, 0x00, 0x01);
#endif

		return false;
	}	

	if(tcgetattr(m_iPhonePort, &options) != 0)
	{
		perror("GetSerialAttr");
		return false;
	}
	
	options.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	options.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	options.c_oflag &= ~OPOST;

//	cfsetispeed(&Opt,B460800);	//设置波特率
//	cfsetospeed(&Opt,B460800);
	
	if (tcsetattr(m_iPhonePort,TCSANOW,&options) != 0)   
	{ 
		perror("SetSerialAttr"); 
		return false;
	}

	PRTMSG(MSG_NOR,"UsbSerial open Succ!\n"); 
	return true;
}

int CPhoneEvdo::_CloseCom()
{
	if( -1 != m_iPhonePort )
	{
		close( m_iPhonePort );
		m_iPhonePort = -1;
	}

	return 0;
}

int CPhoneEvdo::_ComSnd(void* v_pSnd, DWORD v_dwSndLen )
{
	int iWrited = 0;
	const DWORD SNDMINTNTERVAL = 200; // ms,不宜太大
	static DWORD sta_dwLstSnd = 0;
	DWORD dwToLstSnd = GetTickCount() - sta_dwLstSnd;
	if( dwToLstSnd < SNDMINTNTERVAL )
	{
		usleep( (SNDMINTNTERVAL - dwToLstSnd)*1000 );
	}
	iWrited = write(m_iPhonePort, v_pSnd, v_dwSndLen);
	sta_dwLstSnd = GetTickCount();
	if( -1 == iWrited ) return ERR_COMSND;
	if( iWrited != v_dwSndLen ) return ERR_COMSND_UNALL;

	//PRTMSG(MSG_DBG, "PCom Snd:%s\n", v_pSnd);

	return 0;
}

// SetAT中的返回值处理不要轻易改变
int CPhoneEvdo::_SetAT( DWORD v_dwWaitPerTime, BYTE v_bytTryTimes, void* v_pResult, size_t v_sizResult,
	PCHKATRES v_pChkRes, DWORD v_dwSndLen, char* v_szATCmd, ... )
{
	//int euAns = EVDO_ATANS_OTHER;
	int iResult = 0;
	int iRet = EVDO_ATANS_NOANS; // 初始化返回值为无应答
	DWORD dwFstResRcvTm = GetTickCount(); // 初始化为起始时刻
	DWORD dwAtCmdSndTm = 0;
	DWORD dwAtCmdLen = 0;
	//tagComRcvHdl objComRcvHdl;
	char *pAtResBegin = NULL;
	char *pAtResTotalBegin = NULL;
	DWORD dwAtResTotalLen = 0;
	DWORD dwResRcvTm = 0;
	DWORD dwResLen = 0;
	DWORD dwPushTm = 0;
	DWORD dwCpyLen = 0;
	DWORD dwCur = 0;
	BYTE bytLvl = 0;
	bool bToSndNext = false;
	char szAtCmdData[1024] = {0};
	char szAtResData[1024] = {0};
	char szAtResTotalData[2048] = { 0 };

	if( 0 == v_dwSndLen )
	{
		va_list ap;
		va_start(ap, v_szATCmd);
		vsprintf(szAtCmdData, v_szATCmd, ap);
		va_end(ap);
		strcat(szAtCmdData, "\r\n"); // 不能丢

		szAtCmdData[ sizeof(szAtCmdData) - 1 ] = 0;
		dwAtCmdLen = strlen( szAtCmdData );
	}
	else
	{
		dwAtCmdLen = min(v_dwSndLen, sizeof(szAtCmdData));
		memcpy( szAtCmdData, v_szATCmd, dwAtCmdLen );
	}

	pAtResTotalBegin = szAtResTotalData; // 初始化"总接收组起始指针"
	for( BYTE byt = 0; byt < v_bytTryTimes; byt ++ )
	{
		// 发送请求
		dwAtCmdSndTm = GetTickCount();
		iResult = _ComSnd(szAtCmdData, dwAtCmdLen );
		if( iResult )
		{
			usleep(200000);
			continue;
		}
		pAtResBegin = NULL; // 每次发送后重新判断应答数据起始位置

		// 等待应答
		bToSndNext = false;
		while( GetTickCount() - dwAtCmdSndTm <= v_dwWaitPerTime )
		{
			usleep(50000);
			// 尝试提取应答
			DWORD dwPopCumuLen = 0;
			DWORD dwBeforPopTm = GetTickCount();
			while(1)
			{
				memset( szAtResData, 0, sizeof(szAtResData) );
				iResult = m_objDataMngRcv.PopData( bytLvl, sizeof(szAtResData), dwResLen, szAtResData,
					dwPushTm, NULL, (char*)&dwResRcvTm, sizeof(dwResRcvTm) ) ;
				if( 0 == iResult )
				{
					// 有应答先添加到"总接收组"内
					if( dwResLen <= sizeof(szAtResData) )
					{
						szAtResData[ sizeof(szAtResData) - 1 ] = 0; // 保险处理
						dwCpyLen = min(dwResLen, sizeof(szAtResTotalData) - dwAtResTotalLen - 1);
						memcpy( szAtResTotalData + dwAtResTotalLen, szAtResData, dwCpyLen);

						// 先判断是否就是本次请求的应答
						//que dwResRcvTm？
						if( !pAtResBegin && dwResRcvTm > dwAtCmdSndTm ) // 若之前没有得到起始应答, 且本次应答时间迟于请求时间
						{
							pAtResBegin = szAtResTotalData + dwAtResTotalLen;
						}

						// 再累加总接收组数据长度
						dwAtResTotalLen += dwCpyLen;

						// 累加得到数据的总长度
						dwPopCumuLen += dwResLen;
					}
 
					// 防止万一出现无限制的读取就阻塞了
					if( dwPopCumuLen >= 2000 || GetTickCount() - dwBeforPopTm > 6000 ) // (时长限制机制是保险起见,主要是读取量的限制机制)
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			// 判断是否要先处理"总接收组"内的数据
			dwCur = GetTickCount();
			if( dwCur - dwFstResRcvTm >= 3000 && pAtResTotalBegin )
			{
				// 处理模块主动发来的数据
				_PhoneDataDeal( pAtResTotalBegin );

				// 处理完毕后相关状态刷新
				pAtResTotalBegin = szAtResTotalData + dwAtResTotalLen;
 				dwFstResRcvTm = dwCur;
			}

			// 若还未收到本次请求的应答,继续下次循环
			if( !pAtResBegin ) continue;
			// 对本次请求应答的处理
			if( v_pChkRes )
			{
				iResult = v_pChkRes(pAtResBegin, v_pResult, v_sizResult);
			}
			else if( strstr(pAtResBegin, "OK") )
			{
				iResult = EVDO_ATANS_OK;
			}
			else if( strstr(pAtResBegin, "CME ERROR") )
			{
				iResult = EVDO_ATANS_CMEERR;
			}
			else if( strstr(pAtResBegin, "ERROR") )
			{
				iResult = EVDO_ATANS_ERROR;
			}
			else
			{
				iResult = EVDO_ATANS_RCVNOTALL;
			}

			// 根据处理结果决定下步动作
			switch( iResult )
			{
			case EVDO_ATANS_OK:
				iRet = iResult;
				goto _SETAT_END;
				break;

			case EVDO_ATANS_RCVNOTALL:
			case EVDO_ATANS_NOANS:
				// 请处理AT应答的函数注意,如果返回的是以上的应答则将继续等待
				break;

			case EVDO_ATANS_CMEERR:
			case EVDO_ATANS_ERROR:
			case EVDO_ATANS_OTHER:
			default:
				// 请处理AT应答的函数注意,如果返回的是以上的应答则将再次(未超时未达最大次数时)发送请求并等待应答
				bToSndNext = true;
				break;
			}

			if( bToSndNext )
			{
				break;
			}
		}
	}

_SETAT_END:

	// 判断是否要处理遗留的"总接收组"数据
	if( pAtResTotalBegin )
	{
		// 处理模块主动发来的数据
		_PhoneDataDeal( pAtResTotalBegin );
	}

	if( EVDO_ATANS_OK != iRet )
	{
		szAtCmdData[ strlen(szAtCmdData) - 2 ] = 0; // 去除回车换行再打印
	//	PRTMSG(MSG_ERR, "SETAT_END: Set %s not return EVDO_ATANS_OK\n", szAtCmdData );
	}

	return iRet;
}

// ReadAT中的返回值处理不要轻易改变
int CPhoneEvdo::_ReadAT( DWORD v_dwWait, void* v_pResult, size_t v_sizResult, PCHKATRES v_pChkRes )
{
	if( !v_pChkRes )
	{
		return EVDO_ATANS_NOANS;
	}

	int iResult = EVDO_ATANS_NOANS;
	int iRet = EVDO_ATANS_NOANS; // 初始化返回值为无应答
	DWORD dwResLen = 0;
	DWORD dwPushTm = 0;
	DWORD dwResRcvTm = 0;
	DWORD dwCpyLen = 0;
	DWORD dwCur = 0;
	BYTE bytLvl = 0;
	bool bToSndNext = false;
	char szAtResData[1024] = {0};
	char szAtResTotalData[2048] = { 0 };
	DWORD dwAtResTotalLen = 0;
	DWORD dwBegin = GetTickCount();

	while( GetTickCount() - dwBegin <= v_dwWait )
	{
		usleep(50000);

		// 尝试提取AT数据
		DWORD dwPopCumuLen = 0;
		DWORD dwBeforPopTm = GetTickCount();
		memset( szAtResTotalData, 0, sizeof(szAtResTotalData) );
		dwAtResTotalLen = 0;
		while(1)
		{			
			memset( szAtResData, 0, sizeof(szAtResData) );
			iResult = m_objDataMngRcv.PopData( bytLvl, sizeof(szAtResData), dwResLen, szAtResData,
				dwPushTm, NULL, (char*)&dwResRcvTm, sizeof(dwResRcvTm) ) ;
			if( 0 == iResult )
			{
				// 有应答先添加到"总接收组"内
				if( dwResLen <= sizeof(szAtResData) )
				{
					szAtResData[ sizeof(szAtResData) - 1 ] = 0; // 保险处理
					dwCpyLen = min(dwResLen, sizeof(szAtResTotalData) - dwAtResTotalLen - 1);
					memcpy( szAtResTotalData + dwAtResTotalLen, szAtResData, dwCpyLen);

					// 累加总接收组数据长度
					dwAtResTotalLen += dwCpyLen;

					// 累加得到数据的总长度
					dwPopCumuLen += dwResLen;
				}

				// 防止万一出现无限制的读取就阻塞了
				if( dwPopCumuLen >= 2000 || GetTickCount() - dwBeforPopTm > 6000 ) // (时长限制机制是保险起见,主要是读取量的限制机制)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		if( dwPopCumuLen > 0 )
		{
			// 处理本次需要的数据
			if( v_pChkRes )
			{
				iResult = v_pChkRes(szAtResTotalData, v_pResult, v_sizResult);
			}
			else
			{
				iResult = EVDO_ATANS_NOANS;
			}

			// 处理额外收到的其他数据
			_PhoneDataDeal( szAtResTotalData );

			// 根据处理结果决定下步动作
			switch( iResult )
			{
			case EVDO_ATANS_OK:
				iRet = iResult;
				goto _READAT_END;
				break;

			case EVDO_ATANS_RCVNOTALL:
			case EVDO_ATANS_NOANS:
				// 请处理AT数据的函数注意,如果返回的是以上的应答则将继续等待
				iRet = iResult;
				break;

			default:
				// 请处理AT数据的函数注意,如果返回的是以上的应答则将退出等待
				iRet = iResult;
				goto _READAT_END;
			}
		}
	}

_READAT_END:
	return iRet;
}

// 仅在手机模块状态机线程里使用
int CPhoneEvdo::_PhoneDataDeal(char* v_szAtData)
{
	tagEvdoPhoneTask objPhotoTask;
	DWORD dwToSetTaskSymb = 0;


	// 判断是否收到SIM不存在通知 que是否需要工作的过程中主动查询？


	// 在空闲状态下,判断是否有来电
	if( EVDO_PST_IDLE == m_iPhoneHdlSta && strstr(v_szAtData, "RING") )
	{
		dwToSetTaskSymb |= EVDO_PHTASK_RING;
	}

	// 在通话状态下,判断是否对方挂机
	if( (EVDO_PST_HOLD == m_iPhoneHdlSta || EVDO_PST_LSNING == m_iPhoneHdlSta) && strstr(v_szAtData, "CEND:") )
	{
		PRTMSG(MSG_DBG, "Find Remote Hangup!\n" );
		dwToSetTaskSymb |= EVDO_PHTASK_HANGUP;
	}

	// 添加指令
	if( dwToSetTaskSymb )
	{
		_PhoneTaskSet(objPhotoTask, dwToSetTaskSymb);
	}


	// 短信接收处理
	{
		char* pBuf = v_szAtData;
		char* pSmsBegin = NULL;
		char* pUData = NULL;
		char* pComma[13] = {NULL};//第n逗号位置n:0--12
		char* pTelNum = NULL;
		size_t len;
		DWORD dwPktNum;

		while( pSmsBegin = strstr(pBuf, "^HCMGR:") )
		{
			PRTMSG(MSG_DBG, "!!!!!^HCMGR Msg\n" );

//			PRTMSG(MSG_DBG, "Rcv HCMT msg:%s\n",pBuf);
			//号码，年，月，日，时，分，秒，语言，格式，长度，ptr，prv，type，tag
			//^HCMT:15980856182,2009,11,12,15,45,11,0,格式1,长度3,0,0,0,0
			//456 ,内容
			SM_PARAM sm;
			int i;

			pTelNum = strchr(pSmsBegin, ':');
			pTelNum += 1;
			pComma[0] = strchr(pSmsBegin, ',');
			len = pComma[0] - pTelNum;
			memcpy(sm.tpa, pTelNum, len);
		
			for( i=1; i<13; i++)
			{
				// 寻找起始位置
				pComma[i] = strchr((pComma[i-1]+1), ',');
				if( !pComma[i] ) 
				{
					return EVDO_ATANS_OTHER;
				}
			}

			//短信时间
			sm.scts[0] = *(pComma[0]+3);//年
			sm.scts[1] = *(pComma[0]+4);

			sm.scts[2] = *(pComma[1]+1);//月
			sm.scts[3] = *(pComma[1]+2);

			sm.scts[4] = *(pComma[2]+1);//日
			sm.scts[5] = *(pComma[2]+2);

			sm.scts[6] = *(pComma[3]+1);
			sm.scts[7] = *(pComma[3]+2);

			sm.scts[8] = *(pComma[4]+1);
			sm.scts[9] = *(pComma[4]+2);

			sm.scts[10] = '0';
			sm.scts[11] = '0';
			//短信时间

			
			int num = 0;
			sscanf((pComma[7]+1),"%d",&num);
			if (0==num)
			{
				sm.dcs = SMS_7BIT;
			}
			else if(1==num)
			{
				sm.dcs = SMS_7BIT;//QUE ASCI
			}
			else if (6==num)
			{
				sm.dcs = SMS_UCS2;
			}
			else
			{
				PRTMSG(MSG_ERR,"EVDO sms DCS err\n");
				return false;
			}

			//短信长度判断
			num = 0;
			sscanf((pComma[8]+1),"%d",&num);
				
			if (num>160)    return false;
			sm.udl = (size_t)num;


			pUData = strstr( pSmsBegin, "\r\n" );
			if( !pUData )
			{
				break;
			}
			PRTMSG(MSG_NOR,"sms data:0x%x0x%x0x%x0x%x0x%x0x%x0x%x0x%x0x%x0x%x\n",pUData[0],pUData[1],pUData[2],pUData[3],pUData[4],pUData[5],pUData[6],pUData[7]);
			pUData+=2;
			
			size_t dstLen = evdo_decode_pdu(pUData, &sm);//短信内容
			//sms_deal(&sm);
			m_objDataMngSmsRcv.PushData((BYTE)LV1, sizeof(sm), (char*)&sm, dwPktNum);
			sm.udl = dstLen;
			
			pBuf = pUData;
		}
	}

	return true;
}


int CPhoneEvdo::_OtherHdl()
{
	// 检测电话线程的看门狗(如超过5分钟未清看门狗则复位系统)
	bool bResetSys = false;
	pthread_mutex_lock( &m_crisecDog );
	unsigned int uiCur = GetTickCount();

	//uiCur可能会小于m_dwClrDogTm，导致uiCur-m_dwClerDogTm将会是一个超过300000的很大的值，导致系统复位
	//if( uiCur - m_dwClrDogTm > 300000 ) 

	int iDiff = uiCur - m_dwClrDogTm;
	if( iDiff > 300000 ) 
	{
//		PRTMSG(MSG_DBG,"other tick:%lu\n", uiCur);
// 		PRTMSG(MSG_DBG,"other DogTm:%lu\n",m_dwClrDogTm);
		bResetSys = true;
	}
	pthread_mutex_unlock( &m_crisecDog );
	if( true == bResetSys )
	{
		PRTMSG(MSG_DBG,"Thread(PhoneHdl) long time block!\n");

		pthread_mutex_lock( &m_crisecDog );
		m_dwClrDogTm = GetTickCount();
		pthread_mutex_unlock( &m_crisecDog );
		
#if CHK_VER == 0		
		//g_objPhoneEvdo.ResetPhone();
		G_ResetSystem();
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		g_objDiaodu.show_diaodu("手机模块异常");
		RenewMemInfo(0x02, 0x00, 0x00, 0x01);
#endif

		return 0;
	}

	BYTE src = 0;
	BYTE bytLvl = 0;
	DWORD cmdlen = 0;
	char cmd[1024] = {0};
	DWORD dwPushTm;
	DWORD dwPktNum;
	if( (0 != g_objPhoneRecvDataMng.PopData(bytLvl, (DWORD)sizeof(cmd), cmdlen, (char*)cmd, dwPushTm, &src)) || (0==cmdlen))
	{
		usleep(50000);
		return 0;
	}

	int iPhoneHdlSta = m_iPhoneHdlSta; // 这里稍微违反一下规则,不互斥访问了,仅仅读取一次问题不大,不要滥用就好

	switch(byte(cmd[0]))
	{
		//--------------------------------------------------
	case 0x10:	//发送DTMF请求
		{
			char ch = cmd[1];
			bool succ = ((ch>='0'&&ch<='9')||(ch>='A'&&ch<='D')||(ch=='*')||(ch=='#'));
			succ = succ && (EVDO_PST_HOLD == iPhoneHdlSta);
			if(!succ)
			{
				Frm11 frm;
				frm.reply = 0x02;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				tagEvdoPhoneTask objPhoneTask;
				objPhoneTask.m_szDTMF[0] = ch;
				_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_SNDDTMF );
			}
		}
		break;
		

		//--------------------------------------------------
	case 0x12:	// 拨号请求 被叫号码长度(1)+被叫号码(n)
		{
			Frm12 *pfrm = (Frm12*)cmd;

			// 判断号码类型(0,允许呼出 1,车台主动监听号码 2,禁止呼出号码 3,中心服务电话号码)
			WORD wHoldLmtPrid = 0;
			byte type = tel_type(true, pfrm->num, pfrm->len, &wHoldLmtPrid);
			PRTMSG(MSG_DBG,"Dial %.15s(%02x, %u)...\n", pfrm->num, type, wHoldLmtPrid);
			type>>=4;
			
			// 是否是紧急号码
			char num[256] = {0};
			memcpy(num, pfrm->num, pfrm->len);
			bool ishurry =  ( strcmp(num, "110") == 0 ) || ( strcmp(num, "112") == 0 )
							|| ( strcmp(num, "119") == 0 ) ||( strcmp(num, "120") == 0 )
							|| ( strcmp(num, "122") == 0 ) || ( strcmp(num, "10000") == 0 );

			Frm13 frm;
			frm.reply = 0x01;
			if(EVDO_PST_IDLE != iPhoneHdlSta)
			{
				frm.reply = (EVDO_PST_INIT == iPhoneHdlSta) ? 0x04 : 0x02; // 没有搜索到网络:手机正忙
			}
			else if( ishurry )
			{
				frm.reply = 0x01; // 紧急号码呼出
			}
			else if(type==2)
			{
				frm.reply = 0x05; // 被呼叫限制的号码
			} 

// 			// 判断是否在报警状态内(主动监听号码和中心服务电话号码则不判断)
// 			if(1!=type && 3!=type) {
// 				if(m_almtime!=0 && (GetTickCount()-m_almstart)<m_almtime ) {
// 					frm.reply = 0x02;
// 				} else {
// 					m_almtime = 0;
// 					m_almstart = 0;
// 				}
// 			}

			if(0x01 == frm.reply) // 成功则增加拨打电话指令
			{
				tagEvdoPhoneTask objPhoneTask;
				memcpy( objPhoneTask.m_szDialTel, pfrm->num, min(sizeof(objPhoneTask) - 1, pfrm->len) );
				_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_DIAL ); // 此处都认为是用户拨打电话,而非监听电话
			}
			else // 失败则立刻应答
			{
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
		}
		break;


		//--------------------------------------------------
	case 0x14:	//摘机请求
		{
			bool succ = EVDO_PST_RCVRING == iPhoneHdlSta;
			//succ = succ && (!m_simlack);

			if(!succ)
			{
				Frm15 frm;
				frm.reply = 0x02;	//失败
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				tagEvdoPhoneTask objPhoneTask;
				_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_PICKUP );
			}
		}
		break;


		//--------------------------------------------------
	case 0x16:	//挂机请求
		{
			bool succ = (iPhoneHdlSta==EVDO_PST_RCVRING) || (iPhoneHdlSta==EVDO_PST_DIAL)
				|| (iPhoneHdlSta==EVDO_PST_WAITPICK)|| (iPhoneHdlSta==EVDO_PST_HOLD);
				
			if(!succ)
			{
				Frm17 frm;
				frm.reply = 0x02;	//失败
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				PRTMSG(MSG_DBG,"User Hangup!\n");

				tagEvdoPhoneTask objPhoneTask;
				_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_HANGUP );
			}
		}
		break;


		//--------------------------------------------------
	case 0x18:	//振铃指示应答
	case 0x20:	//接通指示应答
	case 0x22:	//空闲指示应答
		break;


		//--------------------------------------------------
	case 0x24:	//信号强度请求
		{
			tagEvdoPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
			int iSignal = objPhoneSta.m_iSignal;

			Frm25 frm;
			if(iSignal>0 && iSignal<MIN_GPRS_SIGNAL)			frm.level = 1;
			else if(iSignal>=MIN_GPRS_SIGNAL && iSignal<=15)	frm.level = 2;
			else if(iSignal>15 && iSignal<=25)	frm.level = 3;
			else if(iSignal>25 && iSignal<=31)	frm.level = 4;
			else if(iSignal>=100 && iSignal<=199)	frm.level = 5;
			else									frm.level = 0;
			g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
		}
		break;


		//--------------------------------------------------
	case 0x26:	//免提耳机切换请求
		{
			bool succ = (iPhoneHdlSta==EVDO_PST_HOLD);

			if(!succ)
			{
				Frm27 frm;
				frm.reply = 0x02;	//失败
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				tagEvdoPhoneTask objPhoneTask;
				_PhoneTaskSet(objPhoneTask, EVDO_PHTASK_CHGAUDIO);
			}
		}
		break;


		//--------------------------------------------------
	case 0x28:	//音量调节请求  调节方向(1)  00H调高 01H调低
		{
			bool succ = (iPhoneHdlSta==EVDO_PST_HOLD || iPhoneHdlSta==EVDO_PST_IDLE);

			if(!succ)
			{
				Frm29 frm;
				frm.reply = 0x02;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				tagEvdoPhoneTask objPhoneTask;
				Frm28 *pfrm = (Frm28*)cmd;
				objPhoneTask.m_iChgVol = (pfrm->dir==0x00) ? (1) : (-1);	// 调高 : 调低
				_PhoneTaskSet(objPhoneTask, EVDO_PHTASK_CHGVOL);
			}
		}
		break;

#if USE_IRD == 0
	case 0x2a:	//通话模式设置请求
		{
			bool succ = (iPhoneHdlSta==EVDO_PST_HOLD);

			if(succ)
			{
				tagEvdoPhoneTask objPhoneTask;
				_PhoneTaskSet(objPhoneTask, EVDO_PHTASK_CHGAUDIO);
			}

// 			tag2QHstCfg obj2QHstCfg;
// 			GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg), 
// 				offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );
// 			m_telmode = obj2QHstCfg.m_HandsfreeChannel;
// 			m_telmodebk = obj2QHstCfg.m_HandsfreeChannel;
// 			if (!m_telmode) {	//耳机
// 				// 设置免提模式
// 				if(!set_audio_mode(1))		{ PRTMSG(MSG_ERR,"set_audio_mode(1) Fail!\n"); }
// 				else	{	PRTMSG(MSG_DBG,"set_audio_mode(1) Succ!");	}
// 			} else {	//免提
// 				// 设置免提模式
// 				if(!set_audio_mode(0))		{ PRTMSG(MSG_ERR,"set_audio_mode(0) Fail!\n"); }
// 				else	{	PRTMSG(MSG_DBG,"set_audio_mode(0) Succ!\n");	}
// 			}
		}
		break;
#endif

		//--------------------------------------------------
	case 0x30:	//发送短消息请求  被叫手机号长度(1)+ 被叫手机号(n)+短消息内容长度(1)+短消息内容(m)
		{
			PRTMSG(MSG_DBG,"User req send sms\n");

			if( true ) //EVDO_PST_IDLE == iPhoneHdlSta )
			{
				int p = 1;
				byte tellen = cmd[p];		p++;
				if(tellen>15)	break;
				char tel[16] = {0};
				memcpy(tel, cmd+p, tellen);	p+=tellen;
				
				byte msglen = cmd[p];		p++;
				if(msglen>160)	break;
				char msg[160] = {0};
				memcpy(msg, cmd+p, msglen);	p+=msglen;

				
				int iResult = _PushSndSms(tel, tellen, msg, msglen, INNERSYMB_PHONETODIAO);
				if( 1 != iResult )
				{
					Frm31 frm;
					frm.reply = 2;
					g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
				}
			}
			else
			{
				PRTMSG(MSG_DBG,"Not In Idle Sta,Refuse Sending SMS.\n");

				Frm31 frm;
				frm.reply = 2;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
		}
		break;


		//--------------------------------------------------
	case 0x32:	//接收短消息指示应答	应答类型：01H发送成功 02H发送失败
		{
		}
		break;


		//--------------------------------------------------
	case 0x34:	//修复网络请求
		{
			// 处于如下状态才允许修复网络
			if( EVDO_PST_IDLE==iPhoneHdlSta)
			{
				PRTMSG(MSG_DBG,"Rcv %s's net wick inform!\n", (0x01==cmd[1]) ? "Liu" : "Qian" );

				tagEvdoPhoneTask objPhoneTask;
				if(0x01==cmd[1])	objPhoneTask.m_bytPingSrc |= 0x02;
				else				objPhoneTask.m_bytPingSrc |= 0x01;
//				if(!m_simlack)		m_gprsdail = true;

				_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_PING );
			}
		}
		break;

	case 0x36: //重置短信中心号
		{
			tagEvdoPhoneTask objPhoneTask;
			_PhoneTaskSet(objPhoneTask, EVDO_PHTASK_SETSMSCENT);
		}
		break;


		//--------------------------------------------------
	case 0x38:	//往中心发送短信请求
		{
			PRTMSG(MSG_DBG,"Other Program req send sms!\n");
			
			Frm38 *pfrm = (Frm38*)cmd;
			int iResult = _PushSndSms(pfrm->tel, strlen(pfrm->tel), pfrm->msg, strlen(pfrm->msg), 
				0x02 == pfrm->source ? INNERSYMB_PHONETOLIU : INNERSYMB_PHONETOQIAN);

			if( 1 != iResult )
			{
				Frm39 frm;
				frm.reply = 2;
					
				if(2 == pfrm->source)	
					DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_UPDATE, 2);
				else		
					DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
			}
		}
		break;

		//--------------------------------------------------
	case 0xF1:	//拨打监听电话请求
		{
			PRTMSG(MSG_DBG,"Rcv req(dail listen)!\n");

			FrmF1 *pfrm = (FrmF1*)cmd;

			//监听号码有误
			bool succ = (pfrm->len!=0);
			if(!succ)
			{
				PRTMSG(MSG_ERR,"listen number is err!\n");

				FrmF2 frm;
				frm.reply = 0x04;	//拨打超时(避免QianExe无限次请求)
				DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
				break;
			}
			
			//正在监听中...
			succ = (EVDO_PST_LSNING == iPhoneHdlSta);
			if(succ)
			{
				PRTMSG(MSG_DBG,"Now is listening...\n");

				FrmF2 frm;
				frm.reply = 0x02;	//正在监听中
				DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
				break;
			}

			//系统忙(可能是长时操作) 或正在拨打监听电话中
			succ =	(iPhoneHdlSta==EVDO_PST_INIT) || (iPhoneHdlSta==EVDO_PST_RESET) || (iPhoneHdlSta==EVDO_PST_DIALNET)
					|| (iPhoneHdlSta==EVDO_PST_HANGUP) || (iPhoneHdlSta==EVDO_PST_DIALLSN) || (iPhoneHdlSta==EVDO_PST_WAITLSNPICK);
			if(succ)
			{
				PRTMSG(MSG_DBG,"The line is busy!\n");

				FrmF2 frm;
				frm.reply = 0x06;	//车台忙
				DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
				break;
			}

			//其他状态(包括空闲,拨打,通话,来电等)
			tagEvdoPhoneTask objPhoneTask;
			memcpy( objPhoneTask.m_szDialLsnTel, pfrm->num, min(sizeof(objPhoneTask.m_szDialLsnTel) - 1, pfrm->len) );
			if( EVDO_PST_IDLE == iPhoneHdlSta ) // 空闲状态下
			{
				_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_DIALSN );
			}
			else // 拨打/通话/来电状态
			{
				_PhoneTaskSet( objPhoneTask, EVDO_PHTASK_HANGUP | EVDO_PHTASK_DIALSN ); // 设置2个指令,实际执行时将是先挂机再拨打监听号码
			}
		}
		break;

		//--------------------------------------------------
	case 0xF3:	//进入报警状态(不允许拨打非中心电话并自动挂断非中心来电)
		{
			tagEvdoPhoneTask objPhotoTask;
			objPhotoTask.m_dwAlertPrid = DWORD(cmd[1])*1000; // 0表示退出报警
			_PhoneTaskSet(objPhotoTask, EVDO_PHTASK_ALERM);

// 			tagEvdoPhoneStaCommon objPhoneStaComm;
// 			objPhoneStaComm.m_dwAlertPrid = DWORD(cmd[1])*1000;
// 			if (0 != objPhoneStaComm.m_dwAlertPrid)
// 			{
// 				objPhoneStaComm.m_dwAlertBegin = GetTickCount();
// 				PRTMSG(MSG_DBG,"Enter alarm state(%d s)!\n",DWORD(cmd[1]));
// 			}
// 			else
// 			{
// 				objPhoneStaComm.m_dwAlertBegin = 0;
// 				PRTMSG(MSG_DBG,"Exit alarm state!\n");
// 			}
// 			_PhoneStaCommonSet(objPhoneStaComm, EVDO_PHTASK_ALERM);
		}
		break;
		
	default:
		break;
	}

	return 0;
}

int CPhoneEvdo::_PhoneHdl()
{
	pthread_mutex_lock( &m_crisecDog );
//	PRTMSG(MSG_DBG,"hdl DogTm:%d\n",m_dwClrDogTm);
	m_dwClrDogTm = GetTickCount();
//	PRTMSG(MSG_DBG,"hdl tick:%d\n",m_dwClrDogTm);
	pthread_mutex_unlock( &m_crisecDog );
	DWORD dwSleep = 50000;

	switch( m_iPhoneHdlSta )
	{
	case EVDO_PST_INIT:
		{
#if VEHICLE_TYPE == VEHICLE_M
			g_objLightCtrl.SetLightSta(0x02);	// 绿灯快闪
#endif

			RenewMemInfo(0x02, 0x02, 0x01, 0x05);

			int iResult = _PhoneStaHdl_Init();
			if(!iResult) //成功
			{
				g_simcard = 1;
				m_iPhoneHdlSta = EVDO_PST_IDLE;

#if CHK_VER == 1
	#if VEHICLE_TYPE == VEHICLE_M
				g_objLightCtrl.SetLightSta(0x03);	// 生产检测版本初始化成功即闪绿灯
	#endif
	#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
				g_objLightCtrl2.SetLightSta(0x01);	// 生产检测版本初始化成功即闪绿灯
	#endif
#endif
			}
			else
			{
				G_RecordDebugInfo("evdo init failed, phone reset!");

				g_simcard = 3;
				m_iPhoneHdlSta = EVDO_PST_RESET;
			}
		}
		break;

		//-----------------------------------------------------------------
	case EVDO_PST_IDLE:	//电话空闲中...
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_Idle( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case EVDO_PST_DIALLSN:	//拨打监听电话
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_DialLsn( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case EVDO_PST_WAITLSNPICK:	//等待监听电话接听
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_WaitLsnPick( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case EVDO_PST_LSNING:	//监听中..
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_Lsning( iNextSta );
			m_iPhoneHdlSta = iNextSta;

			if( EVDO_PST_LSNING == iNextSta )
			{
				// 检查是否要发送短信
				_PhoneJugSmsSnd();
			}
		}
		break;

	case EVDO_PST_DIAL:	//拨打电话
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_Dial( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case EVDO_PST_WAITPICK:	//等待摘机
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_WaitPick( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case EVDO_PST_HOLD:	//通话中
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_Hold( iNextSta );
			m_iPhoneHdlSta = iNextSta;

			if( EVDO_PST_HOLD == iNextSta ) // 若仍然是通话状态中
			{
				// 检查是否要发送短信
				_PhoneJugSmsSnd();
			}
		}
		break;

	case EVDO_PST_RCVRING:	//收到来电..
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_RcvRing( iNextSta );
			if( iNextSta != m_iPhoneHdlSta )
			{
				// 由于Ring任务可能不断产生,所以要多处地方清除
				_PhoneTaskClr( EVDO_PHTASK_RING );
			}
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case EVDO_PST_HANGUP:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_Hangup( iNextSta );
			m_iPhoneHdlSta = iNextSta;

			// 由于Ring任务可能不断产生,所以要多处地方清除
			_PhoneTaskClr( EVDO_PHTASK_RING );
		}
		break;

	case EVDO_PST_RESET:
		_PhoneStaHdl_Reset();
		m_iPhoneHdlSta = EVDO_PST_INIT;

		// 由于Ring任务可能不断产生,所以要多处地方清除
		_PhoneTaskClr( EVDO_PHTASK_RING );
		break;

	case EVDO_PST_DIALNET:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_DialNet( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case EVDO_PST_WAITDIALNET:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_WaitDialNet( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case EVDO_PST_CUTNET:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_CutNet( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	default:
		PRTMSG(MSG_ERR, "Phone Sta Unknown! Reset Phone!\n" );
		m_iPhoneHdlSta = EVDO_PST_RESET;
	}

	// 判断是否要进入/退出报警状态
	//que 无实际操作
	_PhoneJugAlerm();

// 	// 判断是否要处理"ping结果报告"
// 	_PhoneJugSmsPingRpt();

	// 判断是否要进入复位状态 //que
	if( _PhoneJugReset() ) m_iPhoneHdlSta = EVDO_PST_RESET;

	if( dwSleep ) usleep(dwSleep);

	return 0;
}

int CPhoneEvdo::_PhoneStaHdl_Init()
{
	PRTMSG(MSG_DBG,"stInit\n");
	G_RecordDebugInfo("PhoneEvdo stInit");

	static int sta_iSetAtFail = 0;
	char szResult[2048] = { 0 };
	int iResult = 0;
	char cIndicate = 0;

	// 清除不应有的,或没有处理价值,或已过时的任务指令
	_PhoneTaskClr( EVDO_PHTASK_CHGAUDIO | EVDO_PHTASK_CHGVOL | EVDO_PHTASK_CUTNET | EVDO_PHTASK_DIAL | EVDO_PHTASK_DIALSN
		| EVDO_PHTASK_HANGUP | EVDO_PHTASK_RING | EVDO_PHTASK_SNDDTMF | EVDO_PHTASK_RESET );

	// 状态机中单一状态独有的环境变量值统一清除 (注意不要用memset,要调用Init)
	m_objPhoneStaDial.Init();
	m_objPhoneStaDialLsn.Init();
	m_objPhoneStaHold.Init();
	m_objPhoneStaLsning.Init();
	m_objPhoneStaRing.Init();

	// 通用环境变量中可以清除的进行清除
	_PhoneStaCommonClr( EVDO_PHSTVAR_SING | EVDO_PHSTVAR_NET );

	// 选择通话关闭模式
	_PhoneAudioModeSet( EVDO_AUDIOMODE_CLOSE );

	// 若usbserial串口句柄为-1，说明已关闭,重新打开串口
	if( -1 == m_iPhonePort )
	{
		iResult = _InitCom();
		if( false == iResult )
		{
			RenewMemInfo(0x02, 0x00, 0x02, 0x00);
			goto _INITPHONE_FAIL;
		}
	}

	// AT测试
	if( EVDO_ATANS_OK != _SetAT(500, 5, NULL, 0, NULL, 0, "AT") )
	{
		PRTMSG(MSG_NOR,"set at fail!\n");
		G_RecordDebugInfo("PhoneEvdo set at fail!");

		sta_iSetAtFail++;
		if( sta_iSetAtFail >= 3 )
			G_ResetSystem();
		
		RenewMemInfo(0x02, 0x00, 0x02, 0x01);
		goto _INITPHONE_FAIL;
	}
	else
	{
		sta_iSetAtFail = 0;
	}
	//PRTMSG(MSG_NOR,"set at succ!\n");

	// 禁止回显
	if( EVDO_ATANS_OK != _SetAT(500, 5, NULL, 0, NULL, 0, "ATE0") )
	{
		PRTMSG(MSG_NOR,"set ATE0 fail!\n");
		RenewMemInfo(0x02, 0x00, 0x02, 0x02);
		goto _INITPHONE_FAIL;
	}
	//PRTMSG(MSG_NOR,"set ATE0 succ!\n");

	_SetAT(3000, 3, NULL, 0, NULL, 0, "AT+CTA=5");//休眠设置，单位秒，0为MS侧关闭发起主动休眠机制

	// 通知千里眼程序手机模块类型 (用于照片传输流控的设置,因为每个模块的传输带宽不一样)
	{
		FrmF3 frmf3;	
		frmf3.reply = byte(DTTD);
		DataPush((char*)&frmf3, sizeof(frmf3), DEV_PHONE, DEV_QIAN, 2);
	}

	_SetAT(3000, 1, NULL, 0, NULL, 0, "AT+CGMR");

	// 查询IMEI号,AT+GSN
	{
		static bool sta_bGetIMEI = false;
		if( !sta_bGetIMEI )
		{
			tagEvdoPhoneStaCommon objSta;
			if( EVDO_ATANS_OK == _SetAT(1000, 5, objSta.m_szGsmIMEI, sizeof(objSta.m_szGsmIMEI), G_EVDO_GSN_Jugres, 0, "AT+GSN") )
			{
				objSta.m_szGsmIMEI[ sizeof(objSta.m_szGsmIMEI) - 1 ] = 0;
				_PhoneStaCommonSet( objSta, EVDO_PHSTVAR_IMEI );

				sta_bGetIMEI = true;
			}
		}
	}

	iResult = _SetAT(3000, 5, NULL, 0, G_EVDO_CPIN_Jugres, 0, "AT+CPIN?");
	if( EVDO_ATANS_OK != iResult )
	{
		G_RecordDebugInfo("PhoneEvdo AT+CPIN? failed!\n");
		RenewMemInfo(0x02, 0x00, 0x02, 0x03);
		goto _INITPHONE_FAIL;
	}

	// que 开启来电显示

	// que Set SMS Service

// 	//设置短信状态报告，短信优先级，编码方式，（6为UNICODE编码），保密级别
// 	_SetAT(1000, 5, NULL, 0, NULL, 0, "AT^HSMSSS=0,0,6,0");
	
	// 存储区设置,EVDO支持 testc
	_SetAT(3000, 2, NULL, 0, NULL, 0, "AT+CPMS=\"ME\",\"ME\",\"ME\"");

	// 设置短信的PDU/text模式，EVDO目前仅支持text模式
	_SetAT(600, 5, NULL, 0, NULL, 0, "AT+CMGF=1");

	// 
	_SetAT(600, 5, NULL, 0, NULL, 0, "AT+CNMI?");
	_SetAT(600, 5, NULL, 0, NULL, 0, "AT+CNMI=1,3,0,0,0");

// 	//AT^PPPCFG?	查询拨号用户和密码,属于非丢失配置数据
// //	_SetAT(3000, 2, NULL, 0, NULL, 0, "AT^PPPCFG=\"card\",\"card\"");
// 	iResult = _SetAT(3000, 2, NULL, 0, G_EVDO_CPPPCFG_Jugres, 0, "AT^PPPCFG?");
// 	if( EVDO_ATANS_OK != iResult )
// 	{
// 		_SetAT(3000, 2, NULL, 0, NULL, 0, "AT^PPPCFG=\"CARD\",\"CARD\"");
// 		PRTMSG(MSG_DBG, "set ppp dial user name:card,password:card\n");
// 	}

	//que 字符集编码格式

	// que 消除侧音

	// 	que 免提/耳机模式选择

	// 选择网络模式，8为混合模式，4为EVDO，2为CDMA
	//暂时选为混合模式，因为测试在EVDO模式下呼入呼出无法进行，短信收发也不正常
	_SetAT(600, 5, NULL, 0, NULL, 0, "AT^PREFMODE=8");

	sleep(1);

	// 获取信号
	memset( szResult, 0, sizeof(szResult) );
	if( EVDO_ATANS_OK == _SetAT(3000, 2, szResult, sizeof(szResult), G_EVDO_CSQ_Jugres, 0, "AT+CSQ") )
	{
		tagEvdoPhoneStaCommon objSta;
		objSta.m_iSignal = atoi( szResult );
		G_RecordDebugInfo("PhoneEvdo first Signal=%d", objSta.m_iSignal);
		_PhoneStaCommonSet( objSta, EVDO_PHSTVAR_SING );
	}

	PRTMSG(MSG_NOR,"EVDO init succ!\n");
	G_RecordDebugInfo("Evdo init succ!");

	return 0;

_INITPHONE_FAIL:
	return -1;
}

int CPhoneEvdo::_PhoneStaHdl_Idle( int& v_iNextSta )
{
//	PRTMSG(MSG_DBG,"stIdle\n");

	int iRet = 0;
	tagEvdoPhoneTask objPhoneTask;
	tagEvdoPhoneStaCommon objPhoneSta;
	static int iGetCSQFailed = 0;
	DWORD dwPktNum;

	// 清除不应有的,或没有处理价值,或已过时的任务指令
	_PhoneTaskClr( EVDO_PHTASK_CHGAUDIO | EVDO_PHTASK_CHGVOL | EVDO_PHTASK_HANGUP | EVDO_PHTASK_SNDDTMF );

	// 检查是否需要断开网络
	if( _PhoneJugCutNet() )
	{
		v_iNextSta = EVDO_PST_CUTNET;
		goto _PHONEIDLE_END;
	}

	//检查是否有来电
	objPhoneTask = _PhoneTaskGet(EVDO_PHTASK_RING);
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_RING )
	{
		// 进入ring状态前先初始化其下的环境变量
		m_objPhoneStaRing.Init();

		v_iNextSta = EVDO_PST_RCVRING;
		goto _PHONEIDLE_END;
	}

	//检查是否有监听电话需要拨打
	objPhoneTask = _PhoneTaskGet(EVDO_PHTASK_DIALSN);
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_DIALSN )
	{
		// 进入下一状态前输入下一状态独有的初始环境变量
		memcpy( m_objPhoneStaDialLsn.m_szDialLsnTel, objPhoneTask.m_szDialLsnTel,
			min(sizeof(m_objPhoneStaDialLsn.m_szDialLsnTel), sizeof(objPhoneTask.m_szDialLsnTel)) );
		m_objPhoneStaDialLsn.m_szDialLsnTel[ sizeof(m_objPhoneStaDialLsn.m_szDialLsnTel) - 1 ] = 0;
		m_objPhoneStaDialLsn.m_dwDialBeginTm = GetTickCount();
		
		v_iNextSta = EVDO_PST_DIALLSN;
		goto _PHONEIDLE_END;
	}

	//检查是否有拨打电话请求
	objPhoneTask = _PhoneTaskGet(EVDO_PHTASK_DIAL);
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_DIAL )
	{
		// 进入下一状态前输入下一状态独有的初始环境变量
		memcpy( m_objPhoneStaDial.m_szDialTel, objPhoneTask.m_szDialTel,
			min(sizeof(m_objPhoneStaDial.m_szDialTel), sizeof(objPhoneTask.m_szDialTel)) );
		m_objPhoneStaDial.m_szDialTel[ sizeof(m_objPhoneStaDial.m_szDialTel) - 1 ] = 0;
		m_objPhoneStaDial.m_dwDialBeginTm = GetTickCount();
		m_objPhoneStaDial.m_dwHoldLmtPrid = objPhoneTask.m_dwHoldLmtPrid;

		v_iNextSta = EVDO_PST_DIAL;
		goto _PHONEIDLE_END;
	}

	// que 检查是否需要设置短信中心号
//	_PhoneJugSmsCentSet();
	
	// 检查是否要发送短信
	_PhoneJugSmsSnd();

	//空闲时间隔进行短信接收检测,目前只在空闲状态下接收短信
	static DWORD tmMsg = GetTickCount();
	if( GetTickCount()-tmMsg>5*1000 )
	{
		tmMsg = GetTickCount();
		// 检查是否有接收短信
		_PhoneJugSmsRcv();
	}

	// ping并检查ping的结果
	objPhoneTask = _PhoneTaskGet(EVDO_PHTASK_PING);
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_PING )
	{		
		v_iNextSta = EVDO_PST_CUTNET;
		goto _PHONEIDLE_END;
	}

	//空闲时间隔5秒(进行信号检测等等)
	{
		static DWORD sta_dwLst = GetTickCount();
		if( GetTickCount() - sta_dwLst > 5000 ) 
		{
			sta_dwLst = GetTickCount();

			//_SetAT(1000, 3, NULL, 0, NULL, 0, "AT^SYSINFO");//状态查询
			//_SetAT(1000, 3, NULL, 0, NULL, 0, "AT^HDRCSQ");//evdo信号格数

			//定时给调度屏发送空闲指示
			Frm23 frm;
			g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);

// 			//定时选择GSM免提和PC音频输出并关闭功放
// 			_PhoneAudioModeSet( EVDO_AUDIOMODE_CLOSE ); // 选择通话关闭模式

			objPhoneSta = _PhoneStaCommonGet(0);

			char szResult[50] = { 0 };
			int iSignal = 0;
			if( EVDO_ATANS_OK == _SetAT(2000, 3, szResult, sizeof(szResult), G_EVDO_CSQ_Jugres, 0, "AT+CSQ") )
			{
				iGetCSQFailed = 0;

				iSignal = atoi( szResult );
				objPhoneSta.m_iSignal = iSignal;
				_PhoneStaCommonSet(objPhoneSta, EVDO_PHSTVAR_SING);

			//	PRTMSG(MSG_DBG,"Signal:%d\n", iSignal);
				G_RecordDebugInfo("PhoneEvdo Signal:%d", iSignal);

				//检查是否长时间弱信号
				static DWORD sta_dwLstNoWeakTm = GetTickCount();
				static DWORD sta_dwContWeakTimes = 0; // 连续检测弱信号次数
				if( iSignal > 0 && iSignal < MIN_GPRS_SIGNAL ) 
				{
					PRTMSG(MSG_DBG,"Low signal(%d)!\n", iSignal);
					sta_dwContWeakTimes ++;

					if( GetTickCount() - sta_dwLstNoWeakTm >= 180000 && sta_dwContWeakTimes > 1 )
					{ //长时间弱信号，最好复位一下模块(3分钟)
						PRTMSG(MSG_DBG,"Low signal(long time),reset gsm!\n");

						sta_dwContWeakTimes = 0;
						sta_dwLstNoWeakTm = GetTickCount();
						v_iNextSta = EVDO_PST_RESET;

						goto _PHONEIDLE_END;
					}
				}
				else
				{
					sta_dwContWeakTimes = 0;
					sta_dwLstNoWeakTm = GetTickCount();
				}

				//检查是否长时间无信号
				static DWORD sta_dwLstSingNormTm = GetTickCount();
				static DWORD sta_dwContNoSingTimes = 0; // 连续检测无信号次数
				if(0 == iSignal || 99 == iSignal || -1 == iSignal) 
				{
					PRTMSG(MSG_DBG,"No signal(%d)!\n", iSignal);
					sta_dwContNoSingTimes ++;

					if( GetTickCount() - sta_dwLstSingNormTm >= 60000 && sta_dwContNoSingTimes > 1 )
					{ //长时间无信号，最好复位一下模块(1分钟)
						PRTMSG(MSG_DBG,"Low signal(long time),reset gsm!\n");

						sta_dwContNoSingTimes = 0;
						sta_dwLstSingNormTm = GetTickCount();
						v_iNextSta = EVDO_PST_RESET;

						goto _PHONEIDLE_END;
					}
				}
				else
				{
					sta_dwContNoSingTimes = 0;
					sta_dwLstSingNormTm = GetTickCount();
				}
			}
			else
			{
				iGetCSQFailed++;
				if( iGetCSQFailed > 2)
				{
#if CHK_VER == 0		
					//g_bProgExit = true;
					g_objPhoneEvdo.ResetPhone();
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
					g_objDiaodu.show_diaodu("手机模块异常");
					RenewMemInfo(0x02, 0x00, 0x00, 0x01);
#endif

					return false;
				}
			}
		}

//生产检测修改说明: 检测版本不进行GPRS拨号
#if CHK_VER == 0
		//检查是否需要拨号上网
		objPhoneSta = _PhoneStaCommonGet(0);
		if( !objPhoneSta.m_bNetConnected )
		{
			v_iNextSta = EVDO_PST_DIALNET;
			
			goto _PHONEIDLE_END;
		}

#endif
	}

_PHONEIDLE_END:
	return iRet;
}

int CPhoneEvdo::_PhoneStaHdl_DialLsn( int& v_iNextSta )
{
	if( 0 == m_objPhoneStaDialLsn.m_szDialLsnTel[0] )
	{
		v_iNextSta = EVDO_PST_IDLE;
		goto _PHONEDIALLSN_END;
	}

	PRTMSG(MSG_NOR,"stDialListen\n");
	PRTMSG(MSG_DBG,"Dial listen(%.255s)...\n", m_objPhoneStaDialLsn.m_szDialLsnTel);

	//设置audio为监听模式
	_PhoneAudioModeSet(EVDO_AUDIOMODE_LSN);
	
	//发送拨打指令
	if( EVDO_ATANS_OK != _SetAT(4000, 5, NULL, 0, NULL, 0, "AT+CDV%.20s;", m_objPhoneStaDialLsn.m_szDialLsnTel) )
	{
		PRTMSG(MSG_ERR,"Dial fail!\n");

		// 监听请求应答
		FrmF2 frm;
		frm.reply = 0x06;	// 车台手机模块忙
		DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
		
		v_iNextSta = EVDO_PST_HANGUP;
		goto _PHONEDIALLSN_END;
	}
	else
	{
		// 此处不填充WAITLSNPICK状态下的环境变量，直接使用DIALLSN状态下的环境变量
		v_iNextSta = EVDO_PST_WAITLSNPICK;
		goto _PHONEDIALLSN_END;
	}

_PHONEDIALLSN_END:
	return 0;
}

int CPhoneEvdo::_PhoneStaHdl_WaitLsnPick( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stWaitLsnPick\n");

//CDMA无摘机提示，此处用CLCC来判断
	int iResult = _SetAT(1000, 3, NULL, 0, G_EVDO_CLCC_JugRes, 0, "AT+CLCC");

	// 可以使用AT+CLCC检测呼叫状态 (待)

	switch( iResult )
	{
	case EVDO_ATANS_OK:
		{
			PRTMSG(MSG_DBG,"Dial succ,enter listen!\n");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x01;	//成功	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			m_objPhoneStaLsning.m_dwCallChkTm = GetTickCount();
			v_iNextSta = EVDO_PST_LSNING;
		}
		break;

	case EVDO_ATANS_ERROR:
		{
			PRTMSG(MSG_ERR,"Phone Err,hang up!\n");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x03;	//占线或设备忙	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_NOCARRIER:
		{
			PRTMSG(MSG_ERR,"No Carrier,hang up!\n");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x03;	//占线或设备忙	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_NODIALTONE:
		{
			PRTMSG(MSG_ERR,"No Dialtone,hang up!\n");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x03;	//占线或设备忙	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_BUSY:
		{
			PRTMSG(MSG_ERR,"Phone busy,hang up!\n");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x03;	//占线或设备忙	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_NOANSWER:
		{
			PRTMSG(MSG_ERR,"Phone No Answer,hang up!\n");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x04;	// 拨打超时或无应答
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_NOANS:
	case EVDO_ATANS_OTHER:
	default:
		if( GetTickCount() - m_objPhoneStaDialLsn.m_dwDialBeginTm > 90000 )
		{
			PRTMSG(MSG_ERR,"Dial Lsn,but wait pick time up,hang up\n");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x04;	// 拨打超时或无应答
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
			
			v_iNextSta = EVDO_PST_HANGUP;
		}
	}

	return 0;
}

int CPhoneEvdo::_PhoneStaHdl_Lsning(int& v_iNextSta)
{
	PRTMSG(MSG_NOR,"stListening\n");

	// 判断是否要主动挂机 (原因是检测到NO CARRIER,即对方挂机)
	tagEvdoPhoneTask objPhoneTask = _PhoneTaskGet( EVDO_PHTASK_HANGUP );
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_HANGUP )
	{
		v_iNextSta = EVDO_PST_HANGUP;

		FrmF2 frm;
		frm.reply = 0x05;	//监听结束	
		DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

		return 0;
	}

	int iResult = _ReadAT( 200, NULL, 0, G_EVDO_JugRemoteHangup );

	switch( iResult )
	{
	case EVDO_ATANS_OK:
		{
			PRTMSG(MSG_DBG,"the other side hang up\n");
			
			v_iNextSta = EVDO_PST_HANGUP;
			
			FrmF2 frm;
			frm.reply = 0x05;	//监听结束	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
		}
		break;

	case EVDO_ATANS_ERROR:
	case EVDO_ATANS_NOCARRIER:

	default:
		{
			// 隔一段时间检测是否通话中...
			if( GetTickCount() - m_objPhoneStaLsning.m_dwCallChkTm > 10000 )
			{
				m_objPhoneStaLsning.m_dwCallChkTm = GetTickCount();
				if( EVDO_ATANS_OK != _SetAT(1000, 3, NULL, 0, G_EVDO_CLCC_JugRes, 0, "AT+CLCC") )
				{
					PRTMSG(MSG_ERR, "Lsning,but chk call fail,hang up\n" );
					v_iNextSta = EVDO_PST_HANGUP;
				}
			}
		}
	}

	return 0;
}

int CPhoneEvdo::_PhoneStaHdl_Dial( int &v_iNextSta )
{
	DWORD dwPktNum;
	PRTMSG(MSG_NOR,"stDial\n");

	// 判断是否在报警状态内
	tagEvdoPhoneStaCommon objSta = _PhoneStaCommonGet(0);
	if( GetTickCount() - objSta.m_dwAlertBegin < objSta.m_dwAlertPrid )
	{
		PRTMSG(MSG_DBG,"Alert! Stop dial,hang up!\n");

		// 拨号请求应答
		Frm13 frm;
		frm.reply = 0x02;	
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);	//手机忙
		
		v_iNextSta = EVDO_PST_HANGUP;
		return 0;
	}

	if( 0 == m_objPhoneStaDial.m_szDialTel[0] )
	{
		v_iNextSta = EVDO_PST_IDLE;
		return 0;
	}
		
	// 选择拨号模式
	_PhoneAudioModeSet( EVDO_AUDIOMODE_DIAL );
	
// 	// 设置拨号后要等待对方摘机后才返回+COLP和OK(很容易失效,所以要重新设置)
// 	if(!set_at(5, 1, 0, "AT+COLP=1\r\n"))   show_msg("Set COLP Fail!");

	//发送拨打指令
	if( EVDO_ATANS_OK != _SetAT(2000, 5, NULL, 0, NULL, 0, "AT+CDV%.20s;", m_objPhoneStaDial.m_szDialTel) )
	{
		PRTMSG(MSG_ERR,"Dial fail!\n");

		// 拨号请求应答
		Frm13 frm;
		frm.reply = 0x02;	//手机正忙	
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
		
		v_iNextSta = EVDO_PST_HANGUP;
		goto _PHONEDIAL_END;
	}
	else
	{
		// 拨号请求应答
		Frm13 frm13;
		frm13.reply = 0x01;	//拨号成功
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm13), (char*)&frm13, dwPktNum);

		if(1==g_iTerminalType) //如果是手柄模式
		{
			tag2QHstCfg obj2QHstCfg;
			GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg),
				offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );

			// 恢复默认配置
			if( obj2QHstCfg.m_HandsfreeChannel )
			{
				if(_PhoneAudioModeSet(EVDO_AUDIOMODE_HANDFREE))
				{
					PRTMSG(MSG_ERR,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_HANDFREE);
				}
			}
			else
			{
				if(_PhoneAudioModeSet(EVDO_AUDIOMODE_EARPHONE))
				{
					PRTMSG(MSG_ERR,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_EARPHONE);
				}
			}
		}
		else
		{
			// 设置免提音频模式
			if(_PhoneAudioModeSet(EVDO_AUDIOMODE_HANDFREE))
			{
				PRTMSG(MSG_ERR,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_HANDFREE);
			}
		}

		// 此处不填充WAITPICK状态下的环境变量，WAITPICK状态直接使用DIAL状态下的环境变量
		v_iNextSta = EVDO_PST_WAITPICK;
	}

_PHONEDIAL_END:
	return 0;
}

int CPhoneEvdo::_PhoneStaHdl_WaitPick( int &v_iNextSta )
{	
	DWORD dwPktNum;

	PRTMSG(MSG_NOR,"stWaitPick\n");

	// 判断是否在报警状态内
	tagEvdoPhoneStaCommon objSta = _PhoneStaCommonGet(0);
	if( GetTickCount() - objSta.m_dwAlertBegin < objSta.m_dwAlertPrid )
	{
		PRTMSG(MSG_DBG,"Alert! Stop waiting pick,hang up!\n");

		// 拨号请求应答
		Frm13 frm;
		frm.reply = 0x02;	
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);	//手机忙
		
		v_iNextSta = EVDO_PST_HANGUP;
		return 0;
	}

	// 判断是否要主动挂机
	tagEvdoPhoneTask objPhoneTask = _PhoneTaskGet( EVDO_PHTASK_HANGUP );
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_HANGUP )
	{
		v_iNextSta = EVDO_PST_HANGUP;

		return 0;
	}

	char szResult[100] = { 0 };
//用CLCC来判断是否对方摘机
	int iResult = _SetAT(1000, 3, NULL, 0, G_EVDO_CLCC_JugRes, 0, "AT+CLCC");

	// 可以使用AT+CLCC检测呼叫状态 (待)

	switch( iResult )
	{
	case EVDO_ATANS_OK:
		{
			PRTMSG(MSG_DBG,"Dial succ,enter hold!\n");

			Frm21 frm21;		
			g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm21), (char*)&frm21, dwPktNum);	//接通指示

			if(1==g_iTerminalType) //如果是手柄模式
			{
				tag2QHstCfg obj2QHstCfg;
				GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg),
					offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );

				// 恢复默认配置
				if( obj2QHstCfg.m_HandsfreeChannel )
				{
					if( _PhoneAudioModeSet(EVDO_AUDIOMODE_HANDFREE) )
					{
						PRTMSG(MSG_ERR,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_HANDFREE);
					}
				}
				else
				{
					if( _PhoneAudioModeSet(EVDO_AUDIOMODE_EARPHONE) )
					{
						PRTMSG(MSG_ERR,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_EARPHONE);
					}
				}
			}
			else
			{
				// 设置免提音频模式
				if( _PhoneAudioModeSet(EVDO_AUDIOMODE_HANDFREE) )
				{
					PRTMSG(MSG_ERR,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_HANDFREE);
				}
			}

			// 初始化Hold状态的独有的环境变量
			m_objPhoneStaHold.m_dwHoldLmtPrid = m_objPhoneStaDial.m_dwHoldLmtPrid;
			m_objPhoneStaHold.m_dwHoldBeginTm = m_objPhoneStaHold.m_dwCallChkTm = GetTickCount();
			v_iNextSta = EVDO_PST_HOLD;
		}
		break;

	case EVDO_ATANS_ERROR:
		{
			PRTMSG(MSG_ERR,"Phone Err,hang up!\n");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x02;	//手机忙	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_NOCARRIER:
		{
			PRTMSG(MSG_ERR,"No Carrier,hang up!\n");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x04; // 未搜索到网络
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_NODIALTONE:
		{
			PRTMSG(MSG_ERR,"No Dialtone,hang up!\n");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x02;	//手机忙
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_BUSY:
		{
			PRTMSG(MSG_ERR,"Phone busy,hang up!\n");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x02;	//手机忙
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_NOANSWER:
		{
			PRTMSG(MSG_ERR,"Phone No Answer,hang up!\n");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x02;	//手机忙
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_NOANS:
	case EVDO_ATANS_OTHER:
	default:
		if( GetTickCount() - m_objPhoneStaDial.m_dwDialBeginTm > 90000 ) // 拨打超时，应该挂断
		{
			PRTMSG(MSG_ERR,"Dial,but wait pick time up,hang up\n");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x02;	//手机忙
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = EVDO_PST_HANGUP;
		}
	}

	return 0;
}

int CPhoneEvdo::_PhoneStaHdl_Hold( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stHold\n");

	char sCallId;
	int iResult = 0;
	tagEvdoPhoneTask objPhoneTask;
	tagEvdoPhoneStaCommon objSta;
	DWORD dwPktNum;

	// 判断是否在报警状态内
	objSta = _PhoneStaCommonGet(0);
	if( GetTickCount() - objSta.m_dwAlertBegin < objSta.m_dwAlertPrid )
	{
		PRTMSG(MSG_DBG,"Alert! Stop hold,hang up!\n");
		
		v_iNextSta = EVDO_PST_HANGUP;
		goto _PHONEHDLHOLD_END;
	}

	// 判断是否要主动挂机
	objPhoneTask = _PhoneTaskGet( EVDO_PHTASK_HANGUP );
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_HANGUP )
	{
		v_iNextSta = EVDO_PST_HANGUP;

		goto _PHONEHDLHOLD_END;
	}

	// 通话时长限制
	if( m_objPhoneStaHold.m_dwHoldLmtPrid &&
		GetTickCount() - m_objPhoneStaHold.m_dwHoldBeginTm >= m_objPhoneStaHold.m_dwHoldLmtPrid )
	{
		PRTMSG(MSG_DBG,"Hold time is limit(%u),hang up!\n", m_objPhoneStaHold.m_dwHoldLmtPrid / 1000 );

		v_iNextSta = EVDO_PST_HANGUP;
		goto _PHONEHDLHOLD_END;
	}
	
	// 免提耳机切换 (手柄模式下)
	objPhoneTask = _PhoneTaskGet( EVDO_PHTASK_CHGAUDIO );
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_CHGAUDIO )
	{
		objSta = _PhoneStaCommonGet(0);

		if( EVDO_AUDIOMODE_EARPHONE == objSta.m_bytCurAudioMode )
		{
			_PhoneAudioModeSet( EVDO_AUDIOMODE_HANDFREE );
		}
		else
		{
			_PhoneAudioModeSet( EVDO_AUDIOMODE_EARPHONE );
		}

		// 免提/耳机切换应答
		Frm27 frm;
		frm.reply = 0x01;
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
	}
	// 发送DTMF
	objPhoneTask = _PhoneTaskGet( EVDO_PHTASK_SNDDTMF );
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_SNDDTMF )
	{
		//查询呼叫ID，通过AT+CLCC来查询，测试中sCallId在0~6循环取值
		int iResult = _SetAT( 1000, 1, &sCallId, sizeof(sCallId), G_EVDO_CLCCGetId, 0, "AT+CLCC" );
		if(EVDO_ATANS_OK == iResult)
		{
			if('0' <= sCallId && '9' >= sCallId )
			//发送dtmf指令
			//AT^DTMF=<call_x>,<dtmf_digit>[,<on_length>[,<off_length>] ]
			iResult = _SetAT( strlen(objPhoneTask.m_szDTMF) * 1000, 2, NULL, 0, NULL, 0,
				"AT^DTMF=%c,%s", sCallId,objPhoneTask.m_szDTMF );
		}

		// DTMF发送请求应答
		Frm11 frm;
		frm.reply = EVDO_ATANS_OK == iResult ? 0x01 : 0x02;
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
	}

	// 调节音量
	objPhoneTask = _PhoneTaskGet( EVDO_PHTASK_CHGVOL );
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_CHGVOL )
	{
		// 或者调整audio scene ? (待)

		iResult = _PhoneAudioOutVolSet( objPhoneTask.m_iChgVol );

		// 音量调节请求应答
		Frm29 frm;
		frm.reply = (!iResult ? 0x01 : 0x02);
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);		
	}

	//CEND
	iResult = _ReadAT( 200, NULL, 0, G_EVDO_JugRemoteHangup );

	switch( iResult )
	{
	case EVDO_ATANS_OK:
		{
			PRTMSG(MSG_DBG,"the other side hang up\n");
			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	case EVDO_ATANS_ERROR:
	case EVDO_ATANS_NOCARRIER:
		{
			PRTMSG(MSG_ERR,"Holding,but no carrier or err,hang up\n");
			
			v_iNextSta = EVDO_PST_HANGUP;
		}
		break;

	default:
		{
			// 隔一段时间检测是否通话中... (检查的可以频繁一点)
			if( GetTickCount() - m_objPhoneStaHold.m_dwCallChkTm > 10000 )
			{
				m_objPhoneStaHold.m_dwCallChkTm = GetTickCount();
				if( EVDO_ATANS_OK != _SetAT(1000, 3, NULL, 0, G_EVDO_CLCC_JugRes, 0, "AT+CLCC") )
				{
					PRTMSG(MSG_ERR,"Holding,but check call fail,hang up\n");
					v_iNextSta = EVDO_PST_HANGUP;
				}
			}
		}
		;
	}

_PHONEHDLHOLD_END:
	return 0;
}

int CPhoneEvdo::_PhoneStaHdl_RcvRing( int& v_iNextSta )
{
	PRTMSG(MSG_NOR,"stRcvRing\n");

	char szRingTel[23] = { 0 };
	byte type = 0;
	WORD wLmtPrid = 0;
	int iResult = 0;
	DWORD dwClipWait = 500;
	bool bToJugRingActive = true;
	DWORD dwPktNum;

	// 读取来电号码
	if( 0 == m_objPhoneStaRing.m_szRingTel[0] ) // 若来电号码还未获取到
	{
		dwClipWait = 1700;
	}
	iResult = _ReadAT( dwClipWait, szRingTel, sizeof(szRingTel), G_EVDO_CLIP_ReadTel );
	if( EVDO_ATANS_OK != iResult && EVDO_ATANS_NOANS != iResult && EVDO_ATANS_RCVNOTALL != iResult )
	{
		// 挂机
		v_iNextSta = EVDO_PST_HANGUP;
		return 0;
	}
	// 判断是否读取到ring
	if( EVDO_ATANS_OK == iResult || EVDO_ATANS_RCVNOTALL == iResult )
	{
		bToJugRingActive = false; // 能取得来电信息则不需要检查振铃状态
	}

	if( 0 == m_objPhoneStaRing.m_szRingTel[0] ) // 若来电号码还未获取到
	{
		// 来电号码处理
		if( 0 == szRingTel[0] ) // 若本次从clip获取来电号码失败
		{
			// 主动提取来电号码
			_SetAT( 1800, 1, szRingTel, sizeof(szRingTel), G_EVDO_CLCC_ReadTel, 0, "AT+CLCC" );
		}
		if( strlen(szRingTel) > 2 && strlen(szRingTel) < sizeof(szRingTel) )
		{ // 若获取的来电号码有效

			type = tel_type(false, szRingTel, strlen(szRingTel), &wLmtPrid );
			type &= 0x0f;

			m_objPhoneStaRing.m_dwHoldLmtPrid = wLmtPrid;
			m_objPhoneStaRing.m_bytCallInType = type;
			memset( m_objPhoneStaRing.m_szRingTel, 0, sizeof(m_objPhoneStaRing.m_szRingTel) );
			memcpy( m_objPhoneStaRing.m_szRingTel, szRingTel, min(sizeof(m_objPhoneStaRing.m_szRingTel) - 1, strlen(szRingTel)) );

			bToJugRingActive = false; // 能取得来电号码则不需要检查振铃状态
		}
	}
	else
	{
		// 来电号码及相关的呼入呼出限制信息使用环境变量中的缓存值 (因此在Idle状态判断出有新来电时这些缓存值都要先清除)

		type = m_objPhoneStaRing.m_bytCallInType;
		wLmtPrid = WORD(m_objPhoneStaRing.m_dwHoldLmtPrid);
		memset( szRingTel, 0, sizeof(szRingTel) );
		memcpy( szRingTel, m_objPhoneStaRing.m_szRingTel, min(sizeof(szRingTel) - 1, strlen(m_objPhoneStaRing.m_szRingTel)) );
	}
	// 判断是否要检查振铃状态
	{
		DWORD dwCur = GetTickCount();
		if( !bToJugRingActive ) m_objPhoneStaRing.m_dwLstChkRingTm = dwCur; // 若不需要检查振铃,则更新"上次发现来电信息时刻"
		if( dwCur - m_objPhoneStaRing.m_dwLstChkRingTm >= 4000 ) // 若连续一段时间未发现来电信息
		{
			m_objPhoneStaRing.m_dwLstChkRingTm = dwCur;

			// 主动检测是否在振铃
			iResult = _SetAT(1000, 2, NULL, 0, G_EVDO_CLCCJugIncoming, 0, "AT+CLCC");
			if( EVDO_ATANS_OK != iResult )
			{
				// 挂机
				v_iNextSta = EVDO_PST_HANGUP;
				return 0;
			}
		}
	}

	PRTMSG(MSG_DBG,"Ring %.15s(%02x, %u)...\n", szRingTel, type, wLmtPrid);

	tagEvdoPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);

	// 呼入类型：0,允许呼入 1,中心主动监听号码 2,禁止呼入号码 3,中心服务电话号码 4,UDP激活号码
	if(type==1) // 若是中心主动监听
	{
		PRTMSG(MSG_DBG,"Ring(%.25s) listen...\n", szRingTel);

		if( EVDO_ATANS_OK == _SetAT(1000, 3, NULL, 0, NULL, 0, "ATA") )
		{
			PRTMSG(MSG_DBG,"Auto pick up, Listening...!\n");
			if( 0 == _PhoneAudioModeSet(EVDO_AUDIOMODE_LSN) )
			{
				objPhoneSta.m_bytCurAudioMode = EVDO_AUDIOMODE_LSN;
				_PhoneStaCommonSet( objPhoneSta, EVDO_PHSTVAR_AUDIOMODE );
			}
			
			// 下一状态环境变量初始化
			m_objPhoneStaLsning.m_dwCallChkTm = GetTickCount();

			v_iNextSta = EVDO_PST_LSNING;
		}
		else
		{
			v_iNextSta = EVDO_PST_HANGUP; // 保险起见还是挂机一下较好
		}
	}
	else if(type==4) // UDP激活号码
	{
		PRTMSG(MSG_DBG,"UDP active(%.25s),hang up!\n", szRingTel);

		Frm35 frm;	//网络恢复通知帧
		frm.reply = 0x01;	//拨号成功
		DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_SOCK, 2);	//通知千里眼(KJ2)UDP重登

		v_iNextSta = EVDO_PST_HANGUP;
	}
	else if( GetTickCount()-objPhoneSta.m_dwAlertBegin < objPhoneSta.m_dwAlertPrid )
	{ // 判断是否在报警状态内(非监听电话则挂断)
		PRTMSG(MSG_DBG,"In Alarm Period,hang up ring(%.25s)!\n", szRingTel);
		v_iNextSta = EVDO_PST_HANGUP;
	}
	else if( 3 == type ) // 中心服务电话号码
	{
		PRTMSG(MSG_DBG,"Ring(%.25s) CentServ Tel..\n", szRingTel);

		Frm19 frm;
		frm.len = min(strlen(szRingTel), 25);
		memcpy(frm.num, szRingTel, frm.len);
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, 2+frm.len, (char*)&frm, dwPktNum);
	}
	else if(type==2) // 禁止呼入号码
	{
		PRTMSG(MSG_DBG,"Ring(%.25s) forbid,hang up!\n", szRingTel);
		v_iNextSta = EVDO_PST_HANGUP;
	}
	else
	{
		PRTMSG(MSG_DBG,"Ring(%.25s)...\n", szRingTel);
	}

	if( EVDO_PST_RCVRING == v_iNextSta ) // 若前面的判断处理未改变状态
	{
		tagEvdoPhoneTask objPhoneTask = _PhoneTaskGet( EVDO_PHTASK_HANGUP | EVDO_PHTASK_PICKUP );
		if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_HANGUP ) // 若要主动挂断
		{
			v_iNextSta = EVDO_PST_HANGUP;
		}
		else if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_PICKUP ) // 若用户要接听
		{
			PRTMSG(MSG_DBG,"User Pick up\n");

			if( EVDO_ATANS_OK == _SetAT(1000, 3, NULL, 0, NULL, 0, "ATA") )
			{
				PRTMSG(MSG_DBG,"Pick up Succ, Holding..!\n");

				if(1==g_iTerminalType) //如果是手柄模式
				{
					tag2QHstCfg obj2QHstCfg;
					GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg),
						offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );

					// 恢复默认配置
					if( obj2QHstCfg.m_HandsfreeChannel )
					{
						if( _PhoneAudioModeSet(EVDO_AUDIOMODE_HANDFREE) )
						{
							PRTMSG(MSG_DBG,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_HANDFREE);
						}
					}
					else
					{
						if( _PhoneAudioModeSet(EVDO_AUDIOMODE_EARPHONE) )
						{
							PRTMSG(MSG_DBG,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_EARPHONE);
						}
					}
				}
				else
				{
					// 设置免提音频模式
					if( _PhoneAudioModeSet(EVDO_AUDIOMODE_HANDFREE) )
					{
						PRTMSG(MSG_DBG,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_HANDFREE);
					}
				}
				
				// 下一状态环境变量初始化
				m_objPhoneStaHold.m_dwHoldBeginTm = m_objPhoneStaHold.m_dwCallChkTm = GetTickCount();
				m_objPhoneStaHold.m_dwHoldLmtPrid = m_objPhoneStaRing.m_dwHoldLmtPrid;

				v_iNextSta = EVDO_PST_HOLD;

				// 摘机请求应答
				Frm15 frm;
				frm.reply = 1;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				PRTMSG(MSG_ERR,"Pick up Fail, Hang up\n");
				v_iNextSta = EVDO_PST_HANGUP; // (为保险起见也还是挂机一下较好)

				// 摘机请求应答
				Frm15 frm;
				frm.reply = 2;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
		}
		else
		{
			// 发送振铃指示
			Frm19 frm;
			frm.len = min(strlen(szRingTel), 25);
			memcpy(frm.num, szRingTel, frm.len);
			g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, 2+frm.len, (char*)&frm, dwPktNum);
		}
	}

	return 0;
}


int CPhoneEvdo::_PhoneStaHdl_Hangup( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stHangup\n");
	DWORD dwPktNum;

	// 清除不应有的,或没有处理价值,或已过时的任务指令
	_PhoneTaskClr( EVDO_PHTASK_CHGAUDIO | EVDO_PHTASK_CHGVOL | EVDO_PHTASK_HANGUP | EVDO_PHTASK_RING | EVDO_PHTASK_SNDDTMF );

	v_iNextSta = EVDO_PST_IDLE; // 预置处理后的新状态值

	//que ATH会不会挂断拨号NET？
	int iResult = _SetAT(15000, 3, NULL, 0, NULL, 0, "AT+CHV"); // 仅挂断语音
	if( EVDO_ATANS_OK != iResult )
	{
		v_iNextSta = EVDO_PST_RESET;
		// 不能return, 否则后面的处理就漏了		return 0;
	}

	Frm23 frm;
	g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);	// 给调度屏发送空闲指示

	Frm35 frm2;
	frm2.reply = 0x04;//挂断电话后，通知sock检查网络
	DataPush((char*)&frm2, sizeof(frm2), DEV_PHONE, DEV_SOCK, 2);

	Frm17 frm3;
	frm3.reply = 0x01;
	g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm3), (char*)&frm3, dwPktNum);

	// 选择通话关闭模式
	_PhoneAudioModeSet( EVDO_AUDIOMODE_CLOSE );

// 	if(1==g_iTerminalType) //如果是手柄模式
// 	{
// 		tag2QHstCfg obj2QHstCfg;
// 		GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg),
// 			offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );
// 
// 		// 恢复默认配置
// 		if( obj2QHstCfg.m_HandsfreeChannel )
// 		{
// 			if(_PhoneAudioModeSet(EVDO_AUDIOMODE_HANDFREE))
// 			{
// 				PRTMSG(MSG_DBG,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_HANDFREE);
// 			}
// 		}
// 		else
// 		{
// 			if(_PhoneAudioModeSet(EVDO_AUDIOMODE_EARPHONE))
// 			{
// 				PRTMSG(MSG_DBG,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_EARPHONE);
// 			}
// 		}
// 	}
// 	else
// 	{	
// 		// 设置免提音频模式
// 		if(_PhoneAudioModeSet(EVDO_AUDIOMODE_HANDFREE))
// 		{
// 			PRTMSG(MSG_DBG,"_PhoneAudioModeSet(%d) Fail!\n", EVDO_AUDIOMODE_HANDFREE);
// 		}
// 	}

	return 0;
}

int CPhoneEvdo::_PhoneStaHdl_Reset()
{
	PRTMSG(MSG_NOR,"stReseting\n");
	G_RecordDebugInfo("PhoneEvdo Reseting...");

	// 选择通话关闭模式
	_PhoneAudioModeSet( EVDO_AUDIOMODE_CLOSE );

	// 更新状态
	tagEvdoPhoneStaCommon objPhoneSta;
	objPhoneSta.m_bNetConnected = false;
	_PhoneStaCommonSet( objPhoneSta, EVDO_PHSTVAR_NET );

	// 程序关闭的同步控制, que 暂未实现

	// 手机模块初始化或复位中
	char cIndicate = 0x01;
	DataPush((char*)&cIndicate, 1, DEV_PHONE, DEV_LED, 2);

	Frm35 frm;
	frm.reply = 0x03;		// 网络断开了(通知千里眼程序)
	DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_SOCK, 2);

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	g_objLightCtrl2.SetLightSta(0x02);	// 复位等同于网络断开
#endif

	// 清除信号值和网络连接状态
	_PhoneStaCommonClr( EVDO_PHSTVAR_SING | EVDO_PHSTVAR_NET );

	_PhoneCutNet();

	_SetAT(1000, 1, NULL, 0, NULL, 0, "ATH"); // 挂断语音 (保险处理)
			
			
	//关闭usbserial
	_CloseCom();

	//同步控制手机模块关闭
	if( m_bToClosePhone )
	{
		usleep(5000);
		return 0;
	}

	//GSM模块复位
	_GsmHardReset();	// 模块的硬件复位

	PRTMSG(MSG_DBG,"EVDO wait ready...\n");
	sleep(3);

	return 0;
}

int CPhoneEvdo::_PhoneStaHdl_DialNet( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stDialNet\n");
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	RenewMemInfo(0x02, 0x02, 0x01, 0x02);
	
	tagEvdoPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	if(objPhoneSta.m_iSignal < 10)
		RenewMemInfo(0x02, 0x02, 0x01, 0x01);
#endif

	static BYTE sta_bytDialNetFailTime = 0;
	if( 0 == _PhoneDialNet() ) // 若拨号动作成功
	{
		m_dwDialNetTm = GetTickCount();
		sta_bytDialNetFailTime = 0;
		v_iNextSta = EVDO_PST_WAITDIALNET;
	}
	else
	{
		sta_bytDialNetFailTime ++;
		if( sta_bytDialNetFailTime >= 2 ) // 若连续2次拨号动作失败
		{
			v_iNextSta = EVDO_PST_RESET;
		}
		else
		{
			sleep(1);
			v_iNextSta = EVDO_PST_CUTNET;
		}
	}

	return 0;
}

int CPhoneEvdo::_PhoneStaHdl_WaitDialNet( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stWaitDialNet\n");

	if( true == m_bPhoneNetReady )
	{
		//网络恢复通知帧
		Frm35 frm;
		frm.reply = 0x01;	//拨号成功
		DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_SOCK, 2);
		
		/// { 清除只与网络相关的任务指令
		// 取消断开网络任务
		_PhoneTaskClr( EVDO_PHTASK_CUTNET );
		
		// 取消当前和将要进行的ping任务
		_PhoneTaskClr( EVDO_PHTASK_PING );
		
		/// } 清除只与网络相关的任务指令
		
		// 更新状态
		tagEvdoPhoneStaCommon objPhoneSta;
		objPhoneSta.m_bNetConnected = true;
		_PhoneStaCommonSet( objPhoneSta, EVDO_PHSTVAR_NET );
		
		// 回到空闲状态
		v_iNextSta = EVDO_PST_IDLE;
		//		v_iNextSta = EVDO_PST_RESET;//testc
	}
	else
	{
		if ((GetTickCount()-m_dwDialNetTm) > 1*60*1000)
		{
			PRTMSG(MSG_NOR, "Dial net Timeout!\n");
			G_RecordDebugInfo("Dial net Timeout!");
			
			//网络断开通知帧
			Frm35 frm;
			frm.reply = 0x03;
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_SOCK, 2);
			
			// 控制指示灯
#if VEHICLE_TYPE == VEHICLE_M
			g_objLightCtrl.SetLightSta(0x02);	// 绿灯快闪
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
			g_objLightCtrl2.SetLightSta(0x02);	// 网络断开通知
#endif
			
			// 更新状态
			tagEvdoPhoneStaCommon objPhoneSta;
			objPhoneSta.m_bNetConnected = false;
			_PhoneStaCommonSet( objPhoneSta, EVDO_PHSTVAR_NET );
			
			v_iNextSta = EVDO_PST_RESET; // 拨号失败就复位模块
			sleep( 12 ); //等待一段时间,以便手机模块有时间收取短信(避免APN设错都没有时间接收配置短信)
		}
		else
		{
			usleep(1000 * 500);
		}
	}
}

void CPhoneEvdo::_PhoneStaHdl_CutNet( int &v_iNextSta )
{
	/// { 放在断网操作的前面
	//网络断开通知帧
	Frm35 frm;
	frm.reply = 0x03;
	DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_SOCK, 2);

	// 控制指示灯
#if VEHICLE_TYPE == VEHICLE_M
	g_objLightCtrl.SetLightSta(0x02);	// 绿灯快闪
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	g_objLightCtrl2.SetLightSta(0x02);	// 网络断开通知
#endif

	// 更新状态
	tagEvdoPhoneStaCommon objPhoneSta;
	objPhoneSta.m_bNetConnected = false;
	_PhoneStaCommonSet( objPhoneSta, EVDO_PHSTVAR_NET );
	/// } 放在断网操作的前面

	_PhoneCutNet();

	/// { 清除只与网络相关的任务指令 (要放在断网操作的后面,因为断开网络的执行需要一段时间)
	// 取消断开网络任务
	_PhoneTaskClr( EVDO_PHTASK_CUTNET );

	// 取消当前和将要进行的ping任务
	_PhoneTaskClr( EVDO_PHTASK_PING );

	/// } 清除只与网络相关的任务指令 (要放在断网操作的后面,因为断开网络的执行需要一段时间)

	if( EVDO_ATANS_OK == _SetAT(1000, 3, NULL, 0, NULL, 0, "AT") )
	{
		v_iNextSta = EVDO_PST_IDLE;
	}
	else
	{
		v_iNextSta = EVDO_PST_RESET;
	}
}

void CPhoneEvdo::_ClearPhoneTask( DWORD v_dwToGetTaskSymb, tagEvdoPhoneTask& v_objPhoneTask )
{
	v_objPhoneTask.m_dwTaskSymb &= ~v_dwToGetTaskSymb;

	if( v_dwToGetTaskSymb & EVDO_PHTASK_DIALSN )
	{
		memset( v_objPhoneTask.m_szDialLsnTel, 0, sizeof(v_objPhoneTask.m_szDialLsnTel) );
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_DIAL )
	{
		memset( v_objPhoneTask.m_szDialTel, 0, sizeof(v_objPhoneTask.m_szDialTel) );
		v_objPhoneTask.m_dwHoldLmtPrid = 0;
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_RING )
	{
		//memset( v_objPhoneTask.m_szRingTel, 0, sizeof(v_objPhoneTask.m_szRingTel) );
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_SETSMSCENT )
	{
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_SNDDTMF )
	{
		memset( v_objPhoneTask.m_szDTMF, 0, sizeof(v_objPhoneTask.m_szDTMF) );
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_HANGUP )
	{
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_PICKUP )
	{
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_CHGAUDIO )
	{
		v_objPhoneTask.m_bytChgAudio = 0;
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_CHGVOL )
	{
		v_objPhoneTask.m_iChgVol = 0;
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_PING )
	{
		v_objPhoneTask.m_bytPingSrc = 0;
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_PINGRPT )
	{
		v_objPhoneTask.m_bytPingResult = 0;
		v_objPhoneTask.m_bytPingRptSrc = 0;
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_CUTNET )
	{
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_ALERM )
	{
		v_objPhoneTask.m_dwAlertPrid = 0;
	}
	if( v_dwToGetTaskSymb & EVDO_PHTASK_RESET )
	{
	}
}

// 在取任务的同时就要清除该任务标志
tagEvdoPhoneTask CPhoneEvdo::_PhoneTaskGet(DWORD v_dwToGetTaskSymb)
{
	tagEvdoPhoneTask objRet;
	
	pthread_mutex_lock( &m_crisecPhoneTask );
	objRet = m_objPhoneTask; // 先填充返回值
	_ClearPhoneTask(v_dwToGetTaskSymb, m_objPhoneTask); // 再清除对应类型
	pthread_mutex_unlock( &m_crisecPhoneTask );

	return objRet;
}

void CPhoneEvdo::_PhoneTaskSet( const tagEvdoPhoneTask& v_objPhoneTask, DWORD v_dwToSetTaskSymb )
{
	pthread_mutex_lock( &m_crisecPhoneTask );

	// 设置命令标志位
	m_objPhoneTask.m_dwTaskSymb |= v_dwToSetTaskSymb;
	
	// 设置具体的命令参数
	if( v_dwToSetTaskSymb & EVDO_PHTASK_DIALSN )
	{
		memcpy( m_objPhoneTask.m_szDialLsnTel, v_objPhoneTask.m_szDialLsnTel, sizeof(m_objPhoneTask.m_szDialLsnTel) );
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_DIAL )
	{
		memcpy( m_objPhoneTask.m_szDialTel, v_objPhoneTask.m_szDialTel, sizeof(m_objPhoneTask.m_szDialTel) );
		m_objPhoneTask.m_dwHoldLmtPrid = v_objPhoneTask.m_dwHoldLmtPrid;
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_RING )
	{
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_SETSMSCENT )
	{
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_SNDDTMF )
	{
		// 这是收到多个DTMF请求可同时发出的模式
		if( strlen(m_objPhoneTask.m_szDTMF) + 1 < sizeof(m_objPhoneTask.m_szDTMF) )
		{
			strncat( m_objPhoneTask.m_szDTMF, v_objPhoneTask.m_szDTMF,
				sizeof(m_objPhoneTask.m_szDTMF) - strlen(m_objPhoneTask.m_szDTMF) - 1 );
		}
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_HANGUP )
	{
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_PICKUP )
	{
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_CHGAUDIO )
	{
		m_objPhoneTask.m_bytChgAudio = v_objPhoneTask.m_bytChgAudio;
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_CHGVOL )
	{
		m_objPhoneTask.m_iChgVol = v_objPhoneTask.m_iChgVol;
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_PING )
	{
		m_objPhoneTask.m_bytPingSrc = v_objPhoneTask.m_bytPingSrc;
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_PINGRPT )
	{
		m_objPhoneTask.m_bytPingRptSrc = v_objPhoneTask.m_bytPingRptSrc;
		m_objPhoneTask.m_bytPingResult = v_objPhoneTask.m_bytPingResult;
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_CUTNET )
	{
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_ALERM )
	{
		m_objPhoneTask.m_dwAlertPrid = v_objPhoneTask.m_dwAlertPrid;
	}
	if( v_dwToSetTaskSymb & EVDO_PHTASK_RESET )
	{
	}

	pthread_mutex_unlock( &m_crisecPhoneTask );
}

void CPhoneEvdo::_PhoneTaskClr(DWORD v_dwToGetTaskSymb)
{
	// 事后清除指定的任务状态标志
	pthread_mutex_lock( &m_crisecPhoneTask );
	_ClearPhoneTask(v_dwToGetTaskSymb, m_objPhoneTask);
	pthread_mutex_unlock( &m_crisecPhoneTask );
}

tagEvdoPhoneStaCommon CPhoneEvdo::_PhoneStaCommonGet( DWORD v_dwToClrStaSymb )
{
	tagEvdoPhoneStaCommon objRet;
	pthread_mutex_lock( &m_crisecSta );
	objRet = m_objPhoneStaCommon;
	_ClearPhoneStaCommon( m_objPhoneStaCommon, v_dwToClrStaSymb );
	pthread_mutex_unlock( &m_crisecSta );

	return objRet;
}

void CPhoneEvdo::_PhoneStaCommonSet( const tagEvdoPhoneStaCommon& v_objPhoneStaCommon, DWORD v_dwToSetStaSymb )
{
	pthread_mutex_lock( &m_crisecSta );
	if( v_dwToSetStaSymb & EVDO_PHSTVAR_IMEI )
	{
		memcpy( m_objPhoneStaCommon.m_szGsmIMEI, v_objPhoneStaCommon.m_szGsmIMEI, sizeof(m_objPhoneStaCommon.m_szGsmIMEI) );
	}

	if( v_dwToSetStaSymb & EVDO_PHSTVAR_SING )
	{
		m_objPhoneStaCommon.m_iSignal = v_objPhoneStaCommon.m_iSignal;
	}

	if( v_dwToSetStaSymb & EVDO_PHSTVAR_ALERT )
	{
		m_objPhoneStaCommon.m_dwAlertBegin = v_objPhoneStaCommon.m_dwAlertBegin;
		m_objPhoneStaCommon.m_dwAlertPrid = v_objPhoneStaCommon.m_dwAlertPrid;
	}

	if( v_dwToSetStaSymb & EVDO_PHSTVAR_NET )
	{
		m_objPhoneStaCommon.m_bNetConnected = v_objPhoneStaCommon.m_bNetConnected;
	}

	if( v_dwToSetStaSymb & EVDO_PHSTVAR_PING )
	{
		m_objPhoneStaCommon.m_bytPingSrc |= v_objPhoneStaCommon.m_bytPingSrc;
	}

	if( v_dwToSetStaSymb & EVDO_PHSTVAR_AUDIOMODE )
	{
		m_objPhoneStaCommon.m_bytCurAudioMode = v_objPhoneStaCommon.m_bytCurAudioMode;
	}

	if( v_dwToSetStaSymb & EVDO_PHSTVAR_AUDIOOUTLVL )
	{
		m_objPhoneStaCommon.m_iAudioOutVol = v_objPhoneStaCommon.m_iAudioOutVol;
	}

	pthread_mutex_unlock( &m_crisecSta );
}

void CPhoneEvdo::_PhoneStaCommonClr(DWORD v_dwToClrStaSymb)
{
	pthread_mutex_lock( &m_crisecSta );

	_ClearPhoneStaCommon( m_objPhoneStaCommon, v_dwToClrStaSymb );

	pthread_mutex_unlock( &m_crisecSta );
}

//	注意：此处在_PhoneStaCommonGet中已经使用互斥访问，不可再用
void CPhoneEvdo::_ClearPhoneStaCommon(tagEvdoPhoneStaCommon& v_objPhoneStaCommon, DWORD v_dwToClrStaSymb)
{
	if( v_dwToClrStaSymb & EVDO_PHSTVAR_IMEI )
	{
		memset( v_objPhoneStaCommon.m_szGsmIMEI, 0, sizeof(v_objPhoneStaCommon.m_szGsmIMEI) );
	}

	if( v_dwToClrStaSymb & EVDO_PHSTVAR_SING )
	{
		v_objPhoneStaCommon.m_iSignal = 0;
	}

	if( v_dwToClrStaSymb & EVDO_PHSTVAR_ALERT )
	{
		v_objPhoneStaCommon.m_dwAlertBegin = 0;
		v_objPhoneStaCommon.m_dwAlertPrid = 0;
	}

	if( v_dwToClrStaSymb & EVDO_PHSTVAR_NET )
	{
		v_objPhoneStaCommon.m_bNetConnected = false;
	}

	if( v_dwToClrStaSymb & EVDO_PHSTVAR_PING )
	{
		v_objPhoneStaCommon.m_bytPingSrc = 0;
	}

	if( v_dwToClrStaSymb & EVDO_PHSTVAR_AUDIOMODE )
	{
		v_objPhoneStaCommon.m_bytCurAudioMode = 0;
	}

	if( v_dwToClrStaSymb & EVDO_PHSTVAR_AUDIOOUTLVL )
	{
		v_objPhoneStaCommon.m_iAudioOutVol = 4;
	}
}

void CPhoneEvdo::_PhoneJugSmsSnd()
{
	DWORD dwLen = 0;
	DWORD dwPushTm = 0;
	BYTE bytLvl = 0;
	BYTE bytSymb = 0;
	BYTE bytSrcType = 0;
	BYTE bytSndTimes = 0;
	BYTE bytSndCt = 0;
	char szSndTel[16] = { 0 };
	char szSndData[200] = { 0 };
	SM_PARAM objSmPar;
	int iResult = 0;
	DWORD dwPktNum;
	while( !m_objDataMngSmsSnd.PopData(bytLvl, sizeof(szSndData), dwLen, szSndData, dwPushTm,
		&bytSymb, szSndTel, sizeof(szSndTel)) )
	{
		if( dwLen > sizeof(szSndData) ) continue;

		bytSndCt ++;
		//pop的对象的定义方式有改变，需调整保存方式
		bytSndTimes = bytSymb >> 4;
		bytSrcType = bytSymb & 0x0f;

		memset( &objSmPar, 0, sizeof(objSmPar) );
		memcpy( objSmPar.tpa, szSndTel, min(sizeof(objSmPar.tpa), sizeof(szSndTel)) );
		memcpy( objSmPar.ud, szSndData, min(sizeof(objSmPar.ud), dwLen) );

		iResult = _PhoneSmsSnd( objSmPar );
		if( EVDO_ATANS_OK != iResult )
		{
			//PRTMSG(MSG_ERR, "Send SM fail\n" );

			bytSndTimes ++;
			if( bytSndTimes < 2 ) // 发送失败次数未达2次,存回待重发
			{
				bytSymb = bytSndTimes << 4 | bytSrcType;
				m_objDataMngSmsSnd.SaveBackData( bytLvl, dwLen, (char*)szSndData, bytSymb, (char*)szSndTel );
			}
			else
			{
				PRTMSG(MSG_ERR,"Sms Snd Fail 2 times, Discard!\n");

				if( INNERSYMB_PHONETODIAO == bytSrcType )
				{
					Frm31 frm;
					frm.reply = 2;
					g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
				}
			}
		}
		else
		{
			if( INNERSYMB_PHONETODIAO == bytSrcType )
			{
				Frm31 frm;
				frm.reply = 1;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
		}

		if( bytSndCt >= 2 ) // 每次最多尝试发送2条,超过则下次再发送
		{
			break;
		}

		memset( szSndData, 0, sizeof(szSndData) );
		memset(szSndTel, 0, sizeof(szSndTel) );

		sleep(1);
	}
}

int CPhoneEvdo::_PhoneJugSmsRcv()
{
	_QueryRcvSMS();
}

//检测新短消息，保存短消息的索引号，返回短消息的条数
//注意：传入的地址需要足够大
int CPhoneEvdo::_QueryRcvSMS()
{
	int szSmsInx[64] = {0};
	int iLen = 0;
	BYTE bytLvl = 0;
	SM_PARAM sm;
	DWORD dwResLen;
	DWORD dwPushTm;
	int i,j;

	for(i=0; i<64; i++)	szSmsInx[i] = -1;

	_SetAT( 500, 1, szSmsInx, sizeof(szSmsInx), G_EVDO_CMGD_SmsList, 0, "AT+CMGD=?" );
	//iLen = strlen(szSmsInx);

//	for (i=0; i<iLen; i++)
	for(i=0; i<64; i++)
	{
		if( -1 == szSmsInx[i] )
			continue;

		if( EVDO_ATANS_OK == _SetAT(1000, 5, NULL, 0, G_EVDO_HCMGR_JugSend, 0, "AT^HCMGR=%d",szSmsInx[i]) ) 
		{
			for(j=0; j<SMS_POP_COUNT; j++)
			{
				if( 0 != m_objDataMngSmsRcv.PopData( bytLvl, (DWORD)sizeof(sm), dwResLen, (char*)&sm, dwPushTm ,NULL, NULL, 0) )
				{
					usleep(200000);
					continue;
				}
				else break;
			}
			if ( j < SMS_POP_COUNT )
			{
				sms_deal(&sm);
				_SetAT(1000, 2, NULL, 0, NULL, 0, "AT+CMGD=%d",szSmsInx[i]); 
			}
		}
	}
	return 0;
}

//que 不支持
void CPhoneEvdo::_PhoneJugSmsCentSet()
{
	tagEvdoPhoneTask objPhoneTask = _PhoneTaskGet( EVDO_PHTASK_SETSMSCENT );
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_SETSMSCENT )
	{
		tag1PComuCfg cfg;
		GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );

		if(strcmp(cfg.m_szSmsCentID, "")!=0)
		{
			// 先查询SIM卡内的短信中心号
			char szSmsCentID[100] = {0};
			/////nnnnnnn
			_SetAT(5000, 2, szSmsCentID, sizeof(szSmsCentID), G_EVDO_CSCA_Query_res, 0, "AT+CSCA?");

			if( strcmp(cfg.m_szSmsCentID, szSmsCentID) )
			{
				_SetAT(6000, 2, NULL, 0, NULL, 0, "AT+CSCA=\"%.21s\"", cfg.m_szSmsCentID);
			}
			else
			{
				PRTMSG(MSG_DBG,"SmsCent in cfg is equal to in SimCard !\n");
			}
		}
		else
		{
			PRTMSG(MSG_ERR,"SmsCent in cfg is empty!\n");
		}
	}
}
//	que ping暂未实现
void CPhoneEvdo::_PhoneJugSmsPingRpt()
{
	tagEvdoPhoneTask objPhoneTask = _PhoneTaskGet( EVDO_PHTASK_PINGRPT );
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_PINGRPT )
	{
		if( 0 == objPhoneTask.m_bytPingResult || 1 == objPhoneTask.m_bytPingResult
			|| 2 == objPhoneTask.m_bytPingResult ) // ping成功或失败或异常
		{
			Frm35 frm;
			frm.reply = (0 == objPhoneTask.m_bytPingResult ? 2 : 3);
			if(objPhoneTask.m_bytPingSrc & 0x01 )
			{
				DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
			}
			if( objPhoneTask.m_bytPingSrc & 0x02 )
			{
				DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_UPDATE, 2);
			}
		}
		else if( 3 == objPhoneTask.m_bytPingResult ) // 若ping被取消
		{
			// (ping取消不发送通知)
		}
	}
}

void CPhoneEvdo::_PhoneJugAlerm()
{
	tagEvdoPhoneTask objPhoneTask = _PhoneTaskGet(EVDO_PHTASK_ALERM);
	if( objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_ALERM )
	{
		tagEvdoPhoneStaCommon objPhoneSta;
		objPhoneSta.m_dwAlertBegin = GetTickCount() - 1;
		objPhoneSta.m_dwAlertPrid = objPhoneTask.m_dwAlertPrid;

		_PhoneStaCommonSet(objPhoneSta, EVDO_PHSTVAR_ALERT);
	}
}

BOOL CPhoneEvdo::_PhoneJugReset()
{
	tagEvdoPhoneTask objPhoneTask = _PhoneTaskGet(EVDO_PHTASK_RESET);
	return m_bToClosePhone ? TRUE: (objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_RESET ? TRUE : FALSE);
}

BOOL CPhoneEvdo::_PhoneJugCutNet()
{
	tagEvdoPhoneTask objPhoneTask = _PhoneTaskGet(EVDO_PHTASK_CUTNET);
	return objPhoneTask.m_dwTaskSymb & EVDO_PHTASK_CUTNET ? TRUE : FALSE;
}

int CPhoneEvdo::_PhoneSmsSnd( SM_PARAM& v_objSmPar )
{
	int iRet = 0;
	char smsbuf[512] = {0};
	int smslen;
	int i;
	bool unicode = false;
	for( i=0; i<strlen(v_objSmPar.ud); i++) 
	{
		if((byte)v_objSmPar.ud[i] > 0x80)
		{ 
			unicode=true;  
			break; 
		}
	}
	if(unicode)		v_objSmPar.dcs = SMS_UCS2;
	else			v_objSmPar.dcs = SMS_7BIT;

	evdo_encode_pdu(&v_objSmPar,smsbuf,smslen);
	PRTMSG(MSG_NOR,"testc smslen: %d\n",smslen);

//	v_objSmPar->tpa[strlen(v_objSmPar.tpa)-1] = 0;
//	char cmd[64] = {0};
	char cmdSMS[1024] = {0};
	int cmdLen = 0;

	char smsEnd[64]={0};//短消息结束发送ctrl+z，对应0x1a
	switch(v_objSmPar.dcs)
	{
	case SMS_7BIT:
		{
			//设置短信状态报告，短信优先级，编码方式，（1为ASCII编码），保密级别
			_SetAT(1000, 5, NULL, 0, NULL, 0, "AT^HSMSSS=0,0,1,0");
			smsEnd[0] = 0x1a;
			smsEnd[1] = '\0';
		}
		break;
		
	case SMS_UCS2:
		{
			//设置短信状态报告，短信优先级，编码方式，（1为ASCII编码），保密级别
			_SetAT(1000, 5, NULL, 0, NULL, 0, "AT^HSMSSS=0,0,6,0");
// 			smsEnd[0] = 0x0;//unicode
// 			smsEnd[1] = 0x1a;//测试按此方式发送不行
// 			smsEnd[2] = '\0';
			smsEnd[0] = 0x1a;
			smsEnd[1] = '\0';
		}
		break;
		
	case SMS_8BIT:
		{
			//设置短信状态报告，短信优先级，编码方式，（1为ASCII编码），保密级别
			_SetAT(1000, 5, NULL, 0, NULL, 0, "AT^HSMSSS=0,0,1,0");
			smsEnd[0] = 0x1a;
			smsEnd[1] = '\0';
		}
		break;
		
	default:
		{
			return false;
		}
		break;
	}

	smsbuf[smslen] = '\r';
//	smsbuf[smslen+1] = '\n';
	//			smsbuf[smslen+2] = 0x0;//0x001a,ctrl+z
	//			smsbuf[smslen+3] = 0x1a;//
	cmdLen = smslen+1;
	//sprintf(cmdSMS,"%s",smsbuf);
	memcpy(cmdSMS,smsbuf,cmdLen);  //unicode不能用sprintf,有00数据

	//AT^HCMGS="手机号码"
	iRet = _SetAT( 20000, 1, NULL, 0, G_EVDO_HCMGS_JugRes, 0, "AT^HCMGS=\"%s\"\r\n",v_objSmPar.tpa );
	if( EVDO_ATANS_OK == iRet )
	{
		//发送短信内容，无应答信息
		_ComSnd(cmdSMS, size_t(cmdLen) );
		//短信结束标志ctrl+z
		iRet = _SetAT(1000, 2, NULL, 0, G_EVDO_HCMGS_JugSend, 0, "%s",smsEnd);
	}

	if( EVDO_ATANS_OK == iRet )
	{
		PRTMSG(MSG_NOR,"Send SM succ!\n");
	}
	else
	{
		PRTMSG(MSG_NOR,"Send SM fail!\n");
	}
	
	return iRet;
}


#if 0 == USE_TELLIMIT
int CPhoneEvdo::JugTelType( char* v_szTel, unsigned char v_ucTelLen, unsigned char* v_p_cTelType )
{
	*v_p_cTelType = 0; // 初始化输出值
	BYTE bytCtrl = 0;
	BYTE byt1 = 0, byt2 = 0;

	tag2QFbidTelCfg obj2QFbidTelCfg;
	int iRet = GetSecCfg( &obj2QFbidTelCfg, sizeof(obj2QFbidTelCfg), offsetof(tagSecondCfg, m_uni2QFbidTelCfg.m_obj2QFbidTelCfg),
		sizeof(obj2QFbidTelCfg) );
	if( iRet )
	{
		*v_p_cTelType = 0; // 允许呼入呼出
		return iRet;
	}

	tag2QServCodeCfg objListenCfg;
	iRet = GetSecCfg( &objListenCfg, sizeof(objListenCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg),
		sizeof(objListenCfg) );
	if( iRet )
	{
		*v_p_cTelType = 0; // 允许呼入呼出
		return iRet;
	}

	// 输入参数判断
	if( NULL == v_szTel || v_ucTelLen > 15 || v_ucTelLen < 3 )
	{
		*v_p_cTelType |= 0x20; // 禁止呼出

		bytCtrl = obj2QFbidTelCfg.m_bytFbidType & BYTE(0x0f);
		if( 0x01 == bytCtrl ) *v_p_cTelType |= 0x02; // 禁止呼入号码
		else *v_p_cTelType |= 0; // 允许呼入
		return ERR_PAR;
	}

	// 去除输入号码的尾空格
	for( unsigned char uc = v_ucTelLen - 1; uc > 0; uc -- )
	{
		if( 0x20 == v_p_cTelType[ uc ] ) v_p_cTelType[ uc ] = 0;
		else break;
	}	
	char szJugTel[16] = { 0 };
	char szCompTel[16] = { 0 };
	memcpy( szJugTel, v_szTel, v_ucTelLen < 14 ? v_ucTelLen : 14 );
	char* pos = strchr( szJugTel, 0x20 );
	if( pos ) *pos = 0;


	// 呼出类型判断
	memcpy( szCompTel, objListenCfg.m_aryLsnHandtel[0], 15 );
	pos = strchr( szCompTel, 0x20 );
	if( pos ) *pos = 0;
	if( objListenCfg.m_bytListenTelCount > 0 && 0 == strcmp(szJugTel, szCompTel) )
	{
		*v_p_cTelType |= 0x10; // 车台主动监听号码(呼出)
	}
	else
	{
		bytCtrl = obj2QFbidTelCfg.m_bytFbidType & BYTE(0xf0);

		if( 0 == bytCtrl ) *v_p_cTelType |= 0; // 允许呼出
		else if( 0x10 == bytCtrl ) *v_p_cTelType |= 0x20; // 禁止呼出号码
		else if( 0x20 == bytCtrl ) // 若控制字是: 允许部分呼出但又禁止某些呼出
		{
			for( byt1 = 0; byt1 < obj2QFbidTelCfg.m_bytAllowOutTelCount; byt1 ++ )
			{
				memcpy( szCompTel, obj2QFbidTelCfg.m_aryAllowOutHandtel[ byt1 ], 15 );
				pos = strchr( szCompTel, 0x20 );
				if( pos ) *pos = 0;

				if( 0 == strncmp(szJugTel, szCompTel, strlen(szCompTel)) ) // 若属于允许呼出号码
				{
					for( byt2 = 0; byt2 < obj2QFbidTelCfg.m_bytFbidOutTelCount; byt2 ++ )
					{
						memcpy( szCompTel, obj2QFbidTelCfg.m_aryFbidOutHandtel[ byt2 ], 15 );
						pos = strchr( szCompTel, 0x20 );
						if( pos ) *pos = 0;

						if( 0 == strncmp(szJugTel, szCompTel, strlen(szCompTel)) ) // 若属于又禁止呼出的号码
							break;
					}

					if( byt2 >= obj2QFbidTelCfg.m_bytFbidOutTelCount ) *v_p_cTelType |= 0; // 允许呼出
					else *v_p_cTelType |= 0x20; // 禁止呼出

					break;
				}
			}

			if( byt1 >= obj2QFbidTelCfg.m_bytAllowOutTelCount ) *v_p_cTelType |= 0x20; // 禁止呼出
		}
		else if( 0x30 == bytCtrl ) // 若控制字是: 禁止部分呼出但又允许某些呼出
		{
			for( byt1 = 0; byt1 < obj2QFbidTelCfg.m_bytFbidOutTelCount; byt1 ++ )
			{
				memcpy( szCompTel, obj2QFbidTelCfg.m_aryFbidOutHandtel[ byt1 ], 15 );
				pos = strchr( szCompTel, 0x20 );
				if( pos ) *pos = 0;

				if( 0 == strncmp(szJugTel, szCompTel, strlen(szCompTel)) ) // 若属于禁止呼出号码
				{
					for( byt2 = 0; byt2 < obj2QFbidTelCfg.m_bytAllowOutTelCount; byt2 ++ )
					{
						memcpy( szCompTel, obj2QFbidTelCfg.m_aryAllowOutHandtel[ byt2 ], 15 );
						pos = strchr( szCompTel, 0x20 );
						if( pos ) *pos = 0;

						if( 0 == strncmp(szJugTel, szCompTel, strlen(szCompTel)) ) // 若属于又允许呼出的号码
							break;
					}

					if( byt2 >= obj2QFbidTelCfg.m_bytAllowOutTelCount ) *v_p_cTelType |= 0x20; // 禁止呼出
					else *v_p_cTelType |= 0; // 允许呼出

					break;
				}
			}

			if( byt1 >= obj2QFbidTelCfg.m_bytFbidOutTelCount ) *v_p_cTelType |= 0; // 允许呼出
		}
	}

	// 呼入类型判断
	for( byt1 = 0; byt1 < objListenCfg.m_bytListenTelCount; byt1 ++ )
	{
		memcpy( szCompTel, objListenCfg.m_aryLsnHandtel[ byt1 ], 15 );
		pos = strchr( szCompTel, 0x20 );
		if( pos ) *pos = 0;

		if( 0 == strcmp(szJugTel, szCompTel) ) break;
	}
	if( byt1 < objListenCfg.m_bytListenTelCount ) *v_p_cTelType |= 0x01; // 中心主动监听号码
	else
	{
		bytCtrl = obj2QFbidTelCfg.m_bytFbidType & BYTE(0x0f);

		if( 0 == bytCtrl ) *v_p_cTelType |= 0; // 允许呼入
		else if( 0x01 == bytCtrl ) *v_p_cTelType |= 0x02; // 禁止呼入号码
		else if( 0x02 == bytCtrl ) // 若控制字是: 允许部分呼入但又禁止某些呼入
		{
			for( byt1 = 0; byt1 < obj2QFbidTelCfg.m_bytAllowInTelCount; byt1 ++ )
			{
				memcpy( szCompTel, obj2QFbidTelCfg.m_aryAllowInHandtel[ byt1 ], 15 );
				pos = strchr( szCompTel, 0x20 );
				if( pos ) *pos = 0;

				if( 0 == strncmp(szJugTel, szCompTel, strlen(szCompTel)) ) // 若属于允许呼入号码
				{
					for( byt2 = 0; byt2 < obj2QFbidTelCfg.m_bytFbidInTelCount; byt2 ++ )
					{
						memcpy( szCompTel, obj2QFbidTelCfg.m_aryFbidInHandtel[ byt2 ], 15 );
						pos = strchr( szCompTel, 0x20 );
						if( pos ) *pos = 0;

						if( 0 == strncmp(szJugTel, szCompTel, strlen(szCompTel)) ) // 若属于又禁止呼入的号码
							break;
					}

					if( byt2 >= obj2QFbidTelCfg.m_bytFbidInTelCount ) *v_p_cTelType |= 0; // 允许呼入
					else *v_p_cTelType |= 0x02; // 禁止呼入

					break;
				}
			}

			if( byt1 >= obj2QFbidTelCfg.m_bytAllowInTelCount ) *v_p_cTelType |= 0x02; // 禁止呼入
		}
		else if( 0x03 == bytCtrl ) // 若控制字是: 禁止部分呼入但又允许某些呼入
		{
			for( byt1 = 0; byt1 < obj2QFbidTelCfg.m_bytFbidInTelCount; byt1 ++ )
			{
				memcpy( szCompTel, obj2QFbidTelCfg.m_aryFbidInHandtel[ byt1 ], 15 );
				pos = strchr( szCompTel, 0x20 );
				if( pos ) *pos = 0;

				if( 0 == strncmp(szJugTel, szCompTel, strlen(szCompTel)) ) // 若属于禁止呼入号码
				{
					for( byt2 = 0; byt2 < obj2QFbidTelCfg.m_bytAllowInTelCount; byt2 ++ )
					{
						memcpy( szCompTel, obj2QFbidTelCfg.m_aryAllowInHandtel[ byt2 ], 15 );
						pos = strchr( szCompTel, 0x20 );
						if( pos ) *pos = 0;

						if( 0 == strncmp(szJugTel, szCompTel, strlen(szCompTel)) ) // 若属于又允许呼入的号码
							break;
					}

					if( byt2 >= obj2QFbidTelCfg.m_bytAllowInTelCount ) *v_p_cTelType |= 0x02; // 禁止呼入
					else *v_p_cTelType |= 0; // 允许呼入

					break;
				}
			}

			if( byt1 >= obj2QFbidTelCfg.m_bytFbidInTelCount ) *v_p_cTelType |= 0; // 允许呼入
		}
	}
	
	return 0;
}
#endif


#if 2 == USE_TELLIMIT
int CPhoneEvdo::JugTelType2( char* v_szTel, unsigned char v_ucTelLen, unsigned char* v_p_cTelType,
	WORD* v_p_wLmtOutPeriod, WORD* v_p_wLmtInPeriod )
{
	*v_p_cTelType = 0x22; // 初始化输出值为禁止呼入呼出
	*v_p_wLmtOutPeriod = * v_p_wLmtInPeriod = 0; // 初始化为不限时

	int iRet = 0;
	char* pos = NULL;
	bool bSkipOut = false, bSkipIn = false;
	BYTE byt = 0;
	char szJugTel[16] = { 0 };
	char szCompTel[16] = { 0 };
	
	// 输入参数判断
	if( NULL == v_szTel || v_ucTelLen > 15 || v_ucTelLen < 3 )
	{
		*v_p_cTelType |= 0x22; // 禁止呼入呼出
		return ERR_PAR;
	}

	// 去除输入号码的尾空格
	memcpy( szJugTel, v_szTel, v_ucTelLen < sizeof(szJugTel) ? v_ucTelLen : sizeof(szJugTel) );
	szJugTel[ sizeof(szJugTel) - 1 ] = 0;
	pos = strchr( szJugTel, 0x20 );
	if( pos ) *pos = 0;

	// 读取配置
	tag2QCallCfg objCallCfg;
	objCallCfg.Init();
	tag2QTelBookPar* pTelBook = NULL;
	iRet = GetSecCfg( &objCallCfg, sizeof(objCallCfg),
		offsetof(tagSecondCfg, m_uni2QCallCfg.m_obj2QCallCfg), sizeof(objCallCfg) );

	tag2QServCodeCfg objServCodeCfg;
	objServCodeCfg.Init();
	iRet = GetSecCfg( &objServCodeCfg, sizeof(objServCodeCfg),
		offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfg) );

// 	tag2QListenCfg objListenCfg;
// 	objListenCfg.Init();
// 	iRet = GetSecCfg( &objListenCfg, sizeof(objListenCfg), offsetof(tagSecondCfg, m_uni2QListenCfg.m_obj2QListenCfg),
// 		sizeof(objListenCfg) );

// 	tag2QCentServTelCfg objCentTelCfg;
// 	objCentTelCfg.Init();
// 	iRet = GetSecCfg( &objCentTelCfg, sizeof(objCentTelCfg),
// 		offsetof(tagSecondCfg, m_uni2QCentServTelCfg.m_obj2QCentServTelCfg), sizeof(objCentTelCfg) );
// 
// 	tag2QUdpAtvTelCfg objUdpActCfg;
// 	objUdpActCfg.Init();
// 	iRet = GetSecCfg( &objUdpActCfg, sizeof(objUdpActCfg),
// 		offsetof(tagSecondCfg, m_uni2QUdpAtvTelCfg.m_obj2QUdpAtvTelCfg), sizeof(objUdpActCfg) );

	
	// (1)最先,监听号码判断

	// 主动监听号码判断 (呼出)
	memset( szCompTel, 0, sizeof(szCompTel) );
	memcpy( szCompTel, objServCodeCfg.m_aryLsnHandtel[0],
		sizeof(objServCodeCfg.m_aryLsnHandtel[0]) < sizeof(szCompTel)
		? sizeof(objServCodeCfg.m_aryLsnHandtel[0])
		: sizeof(szCompTel) );
	szCompTel[ sizeof(szCompTel) - 1 ] = 0;
	pos = strchr( szCompTel, 0x20 );
	if( pos ) *pos = 0;
	if( objServCodeCfg.m_bytListenTelCount > 0 && 0 == strcmp(szJugTel, szCompTel) )
	{
		*v_p_cTelType = *v_p_cTelType & 0x0f | 0x10; // 修改为车台主动监听号码(呼出)
		bSkipOut = true;
	}

	// 中心监听号码判断 (呼入)
	for( byt = 0; byt < objServCodeCfg.m_bytListenTelCount; byt ++ )
	{
		memset( szCompTel, 0, sizeof(szCompTel) );
		memcpy( szCompTel, objServCodeCfg.m_aryLsnHandtel[ byt ],
			sizeof(objServCodeCfg.m_aryLsnHandtel[0]) < sizeof(szCompTel)
			? sizeof(objServCodeCfg.m_aryLsnHandtel[0])
			: sizeof(szCompTel) );
		szCompTel[ sizeof(szCompTel) - 1 ] = 0;
		pos = strchr( szCompTel, 0x20 );
		if( pos ) *pos = 0;

		if( 0 == strcmp(szJugTel, szCompTel) )
		{
			*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x01; // 修改为中心主动监听号码(呼入)
			bSkipIn = true;
			break;
		}
	}


	// (2)中心服务电话号码判断
	for( byt = 0; byt < objServCodeCfg.m_bytCentServTelCount; byt ++ )
	{
		memset( szCompTel, 0, sizeof(szCompTel) );
		memcpy( szCompTel, objServCodeCfg.m_aryCentServTel[ byt ],
			sizeof(objServCodeCfg.m_aryCentServTel[0]) < sizeof(szCompTel)
			? sizeof(objServCodeCfg.m_aryCentServTel[0])
			: sizeof(szCompTel) );
		szCompTel[ sizeof(szCompTel) - 1 ] = 0;
		pos = strchr( szCompTel, 0x20 );
		if( pos ) *pos = 0;

		if( 0 == strcmp(szJugTel, szCompTel) )
		{
			if( !bSkipOut )
			{
				*v_p_cTelType = *v_p_cTelType & 0x0f | 0x30; // 修改为中心服务号码(呼出)
				bSkipOut = true;
			}
			if( !bSkipIn )
			{
				*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x03; // 修改为中心服务号码(呼入)
				bSkipIn = true;
			}
			break;
		}
	}


	// (3)UDP激活电话号码判断 (仅涉及呼入)
	for( byt = 0; byt < objServCodeCfg.m_bytUdpAtvTelCount; byt ++ )
	{
		memset( szCompTel, 0, sizeof(szCompTel) );
		memcpy( szCompTel, objServCodeCfg.m_aryUdpAtvTel[ byt ],
			sizeof(objServCodeCfg.m_aryUdpAtvTel[0]) < sizeof(szCompTel)
			? sizeof(objServCodeCfg.m_aryUdpAtvTel[0])
			: sizeof(szCompTel) );
		szCompTel[ sizeof(szCompTel) - 1 ] = 0;
		pos = strchr( szCompTel, 0x20 );
		if( pos ) *pos = 0;

		if( 0 == strcmp(szJugTel, szCompTel) )
		{
			if( !bSkipIn )
			{
				*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x04; // 修改为UDP激活号码(呼入)
				bSkipIn = true;
			}
			break;
		}
	}

	
	// (4)最后,号码本判断

	// 如果统一设置了呼入或呼出
	if( 0 == (objCallCfg.m_bytSetType & 0x0c) && !bSkipOut ) // 若统一设置为允许呼出
	{
		*v_p_cTelType = *v_p_cTelType & 0x0f | 0;
		bSkipOut = true;
	}
	else if( 0x04 == (objCallCfg.m_bytSetType & 0x0c) && !bSkipOut ) // 若统一设置为禁止呼出
	{
		*v_p_cTelType = *v_p_cTelType & 0x0f | 0x20;
		bSkipOut = true;
	}
	if( 0 == (objCallCfg.m_bytSetType & 0x03) && !bSkipIn  ) // 若统一设置为允许呼入
	{
		*v_p_cTelType = *v_p_cTelType & 0xf0 | 0;
		bSkipIn = true;
	}
	else if( 0x01 == (objCallCfg.m_bytSetType & 0x03) && !bSkipIn ) // 若统一设置为禁止呼入
	{
		*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x02;
		bSkipIn = true;
	}

	// 根据配置判断
	for( byt = 0; byt < sizeof(objCallCfg.m_aryTelBook) / sizeof(objCallCfg.m_aryTelBook[0]); byt ++ )
	{
		pTelBook = objCallCfg.m_aryTelBook + byt;
		memset( szCompTel, 0, sizeof(szCompTel) );
		memcpy( szCompTel, pTelBook->m_szTel,
			sizeof(pTelBook->m_szTel) < sizeof(szCompTel) ? sizeof(pTelBook->m_szTel) : sizeof(szCompTel) );
		szCompTel[ sizeof(szCompTel) - 1 ] = 0;
		pos = strchr( szCompTel, 0x20 );
		if( pos ) *pos = 0;

		if( !strcmp(szJugTel, szCompTel) )
		{
			switch( pTelBook->m_bytType )
			{
			case 0x01: // 不允许呼入，允许呼出
				if( !bSkipOut )
				{
					*v_p_cTelType = *v_p_cTelType & 0x0f | 0;
					*v_p_wLmtOutPeriod = pTelBook->m_wLmtPeriod;
				}
				if( !bSkipIn )
				{
					*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x02; 
					*v_p_wLmtInPeriod = pTelBook->m_wLmtPeriod;
				}
				break;

			case 0x02: // 允许呼入，不许呼出
				if( !bSkipOut )
				{
					*v_p_cTelType = *v_p_cTelType & 0x0f | 0x20;
					*v_p_wLmtOutPeriod = pTelBook->m_wLmtPeriod;
				}
				if( !bSkipIn )
				{
					*v_p_cTelType = *v_p_cTelType & 0xf0 | 0;
					*v_p_wLmtInPeriod = pTelBook->m_wLmtPeriod;
				}
				break;

			case 0x03: // 允许呼入和呼出
				if( !bSkipOut )
				{
					*v_p_cTelType = *v_p_cTelType & 0x0f | 0;
					*v_p_wLmtOutPeriod = pTelBook->m_wLmtPeriod;
				}
				if( !bSkipIn )
				{
					*v_p_cTelType = *v_p_cTelType & 0xf0 | 0;
					*v_p_wLmtInPeriod = pTelBook->m_wLmtPeriod;
				}
				break;

			case 0: // 不允许呼入和呼出
			default:
				if( !bSkipOut )
				{
					*v_p_cTelType = *v_p_cTelType & 0x0f | 0x20;
					*v_p_wLmtOutPeriod = pTelBook->m_wLmtPeriod;
				}
				if( !bSkipIn )
				{
					*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x02;
					*v_p_wLmtInPeriod = pTelBook->m_wLmtPeriod;
				}
			}

			break;
		}
	}

	return 0;
}
#endif


#if 1==USE_TELLIMIT
//--------------------------------------------------------------------------------------------------------------------------
// 根据电话本判断电话类型
int CPhoneEvdo::JugTelType3(char* v_szTel, unsigned char v_ucTelLen, unsigned char* v_p_cTelType,
	WORD* v_p_wLmtOutPeriod, WORD* v_p_wLmtInPeriod)
{
	*v_p_cTelType = 0x22; // 初始化输出值为禁止呼入呼出
	*v_p_wLmtOutPeriod = * v_p_wLmtInPeriod = 0; // 初始化为不限时

	int iRet = 0;
	char* pos = NULL;
	bool bSkipOut = false, bSkipIn = false;
	BYTE byt = 0;
	char szJugTel[16] = { 0 };
	char szCompTel[16] = { 0 };
	

	// 输入参数判断
	if( NULL == v_szTel || v_ucTelLen > 15 || v_ucTelLen < 3 )
	{
		*v_p_cTelType |= 0x22; // 禁止呼入呼出
		return ERR_PAR;
	}

	// 去除输入号码的尾空格
	memcpy( szJugTel, v_szTel, v_ucTelLen < sizeof(szJugTel) ? v_ucTelLen : sizeof(szJugTel) );
	szJugTel[ sizeof(szJugTel) - 1 ] = 0;
	pos = strchr( szJugTel, 0x20 );
	if( pos ) *pos = 0;

	// 读取配置
// 	tag2QCallCfg objCallCfg;
// 	objCallCfg.Init();
// 	tag2QTelBookPar* pTelBook = NULL;
// 	iRet = GetSecCfg( &objCallCfg, sizeof(objCallCfg),
// 		offsetof(tagSecondCfg, m_uni2QCallCfg.m_obj2QCallCfg), sizeof(objCallCfg) );

	tag2QServCodeCfg objServCodeCfg;
	objServCodeCfg.Init();
	iRet = GetSecCfg( &objServCodeCfg, sizeof(objServCodeCfg),
		offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfg) );
	
	BookCfg cfg = {0};
	uint offset = offsetof(tagSecondCfg, m_uniBookCfg.m_szReservered);
	GetSecCfg(&cfg, sizeof(cfg), offset, sizeof(cfg) );

// 	tag2QServCodeCfg cfg2 = {0};
// 	offset = offsetof(tagSecondCfg,m_uni2QServCodeCfg.m_obj2QServCodeCfg);
// 	GetSecCfg( &cfg2, sizeof(cfg2), offset, sizeof(cfg2) );

	// (1)最先,监听号码判断

	// 主动监听号码判断 (呼出)
	memset( szCompTel, 0, sizeof(szCompTel) );
	memcpy( szCompTel, objServCodeCfg.m_aryLsnHandtel[0],
		sizeof(objServCodeCfg.m_aryLsnHandtel[0]) < sizeof(szCompTel)
		? sizeof(objServCodeCfg.m_aryLsnHandtel[0])
		: sizeof(szCompTel) );
	szCompTel[ sizeof(szCompTel) - 1 ] = 0;
	pos = strchr( szCompTel, 0x20 );
	if( pos ) *pos = 0;
	if( objServCodeCfg.m_bytListenTelCount > 0 && 0 == strcmp(szJugTel, szCompTel) )
	{
		*v_p_cTelType = *v_p_cTelType & 0x0f | 0x10; // 修改为车台主动监听号码(呼出)
		bSkipOut = true;
	}

	// 中心监听号码判断 (呼入)
	for( byt = 0; byt < objServCodeCfg.m_bytListenTelCount; byt ++ )
	{
		memset( szCompTel, 0, sizeof(szCompTel) );
		memcpy( szCompTel, objServCodeCfg.m_aryLsnHandtel[ byt ],
			sizeof(objServCodeCfg.m_aryLsnHandtel[0]) < sizeof(szCompTel)
			? sizeof(objServCodeCfg.m_aryLsnHandtel[0])
			: sizeof(szCompTel) );
		szCompTel[ sizeof(szCompTel) - 1 ] = 0;
		pos = strchr( szCompTel, 0x20 );
		if( pos ) *pos = 0;

		if( 0 == strcmp(szJugTel, szCompTel) )
		{
			*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x01; // 修改为中心主动监听号码(呼入)
			bSkipIn = true;
			break;
		}
	}

// 	// 先检查是否是监听电话
// 	for(int i=0; i<min(8,cfg2.m_bytListenTelCount); i++) {
// 		if( 0 == memcmp(&cfg2.m_aryLsnHandtel[i][0], tel, len) ) {
// 			*type = 0x11;
// 			*outtime = 0;
// 			*intime = 0;			
// 			return true;
// 		}
// 	}

	// (2)中心服务电话号码判断
	for( byt = 0; byt < sizeof(cfg.tel) / sizeof(cfg.tel[0]); byt ++ )
	{
		memset( szCompTel, 0, sizeof(szCompTel) );
		memcpy( szCompTel, cfg.tel[byt],
			sizeof(cfg.tel[0]) < sizeof(szCompTel) ? sizeof(cfg.tel[0]) : sizeof(szCompTel) );
		szCompTel[ sizeof(szCompTel) - 1 ] = 0;
		pos = strchr( szCompTel, 0x20 );
		if( pos ) *pos = 0;

		if( 0 == strcmp(szJugTel, szCompTel) )
		{
			if( !bSkipOut )
			{
				*v_p_cTelType = *v_p_cTelType & 0x0f | 0x30; // 修改为中心服务号码(呼出)
				bSkipOut = true;
			}
			if( !bSkipIn )
			{
				*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x03; // 修改为中心服务号码(呼入)
				bSkipIn = true;
			}
			break;
		}
	}

// 	// 先检查中心服务号码电话本
// 	for(i=0; i<5; i++) {
// 		if( 0 == memcmp(&cfg.tel[i][0], tel, len) ) {
// 			*type = 0x33;
// 			*outtime = 0;
// 			*intime = 0;			
// 			return true;
// 		}
// 	}

// 
// 	// (3)UDP激活电话号码判断 (仅涉及呼入)
// 	for( byt = 0; byt < objServCodeCfg.m_bytUdpAtvTelCount; byt ++ )
// 	{
// 		memset( szCompTel, 0, sizeof(szCompTel) );
// 		memcpy( szCompTel, objServCodeCfg.m_aryUdpAtvTel[ byt ],
// 			sizeof(objServCodeCfg.m_aryUdpAtvTel[0]) < sizeof(szCompTel)
// 			? sizeof(objServCodeCfg.m_aryUdpAtvTel[0])
// 			: sizeof(szCompTel) );
// 		szCompTel[ sizeof(szCompTel) - 1 ] = 0;
// 		pos = strchr( szCompTel, 0x20 );
// 		if( pos ) *pos = 0;
// 
// 		if( 0 == strcmp(szJugTel, szCompTel) )
// 		{
// 			if( !bSkipIn )
// 			{
// 				*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x04; // 修改为UDP激活号码(呼入)
// 				bSkipIn = true;
// 			}
// 			break;
// 		}
// 	}

	
	// (3)最后,号码本判断

// 	// 如果统一设置了呼入或呼出
// 	if( 0 == (objCallCfg.m_bytSetType & 0x0c) && !bSkipOut ) // 若统一设置为允许呼出
// 	{
// 		*v_p_cTelType = *v_p_cTelType & 0x0f | 0;
// 		bSkipOut = true;
// 	}
// 	else if( 0x04 == (objCallCfg.m_bytSetType & 0x0c) && !bSkipOut ) // 若统一设置为禁止呼出
// 	{
// 		*v_p_cTelType = *v_p_cTelType & 0x0f | 0x20;
// 		bSkipOut = true;
// 	}
// 	if( 0 == (objCallCfg.m_bytSetType & 0x03) && !bSkipIn  ) // 若统一设置为允许呼入
// 	{
// 		*v_p_cTelType = *v_p_cTelType & 0xf0 | 0;
// 		bSkipIn = true;
// 	}
// 	else if( 0x01 == (objCallCfg.m_bytSetType & 0x03) && !bSkipIn ) // 若统一设置为禁止呼入
// 	{
// 		*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x02;
// 		bSkipIn = true;
// 	}

	// 根据配置判断
	for( byt = 0; byt < sizeof(cfg.book) / sizeof(cfg.book[0]); byt ++ )
	{
		memset( szCompTel, 0, sizeof(szCompTel) );
		memcpy( szCompTel, cfg.book[byt].tel,
			sizeof(cfg.book[0].tel) < sizeof(szCompTel) ? sizeof(cfg.book[0].tel) : sizeof(szCompTel) );
		szCompTel[ sizeof(szCompTel) - 1 ] = 0;
		pos = strchr( szCompTel, 0x20 );
		if( pos ) *pos = 0;

		if( !strcmp(szJugTel, szCompTel) )
		{
			switch( cfg.book[byt].limit & 0x0f )
			{
			case 0x01: // 不允许呼入，允许呼出
				if( !bSkipOut )
				{
					*v_p_cTelType = *v_p_cTelType & 0x0f | 0;
					*v_p_wLmtOutPeriod = WORD(cfg.book[byt].tmhold[0]*60) + cfg.book[byt].tmhold[1];
				}
				if( !bSkipIn )
				{
					*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x02; 
					*v_p_wLmtInPeriod = WORD(cfg.book[byt].tmhold[0]*60) + cfg.book[byt].tmhold[1];
				}
				break;

			case 0x02: // 允许呼入，不许呼出
				if( !bSkipOut )
				{
					*v_p_cTelType = *v_p_cTelType & 0x0f | 0x20;
					*v_p_wLmtOutPeriod = WORD(cfg.book[byt].tmhold[0]*60) + cfg.book[byt].tmhold[1];
				}
				if( !bSkipIn )
				{
					*v_p_cTelType = *v_p_cTelType & 0xf0 | 0;
					*v_p_wLmtInPeriod = WORD(cfg.book[byt].tmhold[0]*60) + cfg.book[byt].tmhold[1];
				}
				break;

			case 0x03: // 允许呼入和呼出
				if( !bSkipOut )
				{
					*v_p_cTelType = *v_p_cTelType & 0x0f | 0;
					*v_p_wLmtOutPeriod = WORD(cfg.book[byt].tmhold[0]*60) + cfg.book[byt].tmhold[1];
				}
				if( !bSkipIn )
				{
					*v_p_cTelType = *v_p_cTelType & 0xf0 | 0;
					*v_p_wLmtInPeriod = WORD(cfg.book[byt].tmhold[0]*60) + cfg.book[byt].tmhold[1];
				}
				break;

			case 0x00: // 不允许呼入和呼出
			default:
				if( !bSkipOut )
				{
					*v_p_cTelType = *v_p_cTelType & 0x0f | 0x20;
					*v_p_wLmtOutPeriod = WORD(cfg.book[byt].tmhold[0]*60) + cfg.book[byt].tmhold[1];
				}
				if( !bSkipIn )
				{
					*v_p_cTelType = *v_p_cTelType & 0xf0 | 0x02;
					*v_p_wLmtInPeriod = WORD(cfg.book[byt].tmhold[0]*60) + cfg.book[byt].tmhold[1];
				}
			}

			break;
		}
	}

	return 0;

// 	// 后检查普通电话本
// 	for(i=0; i<30; i++) {
// 		if( 0 == memcmp(cfg.book[i].tel, tel, len) ) {
// 			if(0==cfg.book[i].limit)		*type = 0x22;
// 			else if(1==cfg.book[i].limit)	*type = 0x02;
// 			else if(2==cfg.book[i].limit)	*type = 0x20;
// 			else							*type = 0x00;
// 			*outtime = *intime = cfg.book[i].tmhold[0]*60 + cfg.book[i].tmhold[1];
// 			return true;
// 		}
// 	}
// 
// 	*type = 0x22; // 什么号码都不是，则禁止呼入呼出
// 	*outtime = 0;
// 	*intime = 0;
// 	return false;
}
#endif

//--------------------------------------------------------------------------------------------------------------------------
// 判断电话号码类型
// callout: true 呼出电话, false 呼入电话
// tel: 电话号码
// len: 电话号码长度
// holdtime: 返回的通话时长
// 返回: 电话号码类型 A1 A2 A3 A4 B1 B2 B3 B4
// A1 A2 A3 A4——呼出类型：0,允许呼出; 1,车台主动监听号码; 2,禁止呼出号码; 3,中心服务电话号码;
// B1 B2 B3 B4——呼入类型：0,允许呼入; 1,中心主动监听号码; 2,禁止呼入号码; 3,中心服务电话号码; 4,UDP激活号码
byte CPhoneEvdo::tel_type(bool callout, char *tel, byte len, ushort *holdtime)
{
	byte type = 0;
#if 0==USE_TELLIMIT
	JugTelType(tel, len, &type);
	*holdtime = 0;
#endif
			
#if 2==USE_TELLIMIT
	ushort outtime = 0, intime = 0;
	JugTelType2(tel, len, &type, &outtime, &intime);
	*holdtime = callout ? outtime : intime;
#endif

#if 1==USE_TELLIMIT
	ushort outtime = 0, intime = 0;
	JugTelType3(tel, len, &type, &outtime, &intime);
	*holdtime = callout ? outtime : intime;
#endif

	return type;
}

// que DVR音频模式待定
// 设置音频模式 
// mode: 音频模式	0 免提
//					1 耳机
//					2 监听	
// 返回: 设置是否成功
int CPhoneEvdo::_PhoneAudioModeSet(int v_iChgToMode)
{
	DWORD dwIoSetSymb = DEV_PHONE;
	switch( v_iChgToMode )
	{
	case EVDO_AUDIOMODE_HANDFREE: //免提模式
		{
			//AT+CLVL,耳机增益调节
			//////nnnnnnn
			int iResult = _SetAT(5000, 3, NULL, 0, NULL, 0, "AT+CLVL=3" );
			if( EVDO_ATANS_OK != iResult )
			{
				return -1;
			}

			iResult = _PhoneAudioOutVolSet(0);
			if( iResult ) return -1;

			PRTMSG(MSG_DBG,"Audio Mode switch to handfree!\n");

			IOSet(IOS_EARPHONESEL, IO_EARPHONESEL_OFF, &dwIoSetSymb, sizeof(dwIoSetSymb));
			IOSet(IOS_AUDIOSEL, IO_AUDIOSEL_PHONE, NULL, 0);
			IOSet(IOS_TRUMPPOW, IO_TRUMPPOW_ON, &dwIoSetSymb, sizeof(dwIoSetSymb));
		}
		break;

	case EVDO_AUDIOMODE_EARPHONE: //耳机模式
		{
			int iResult = _SetAT( 5000, 3, NULL, 0, NULL, 0, "AT+CLVL=3" );
			if( EVDO_ATANS_OK != iResult )
			{
				return -1;
			}

			iResult = _PhoneAudioOutVolSet(0);
			if( iResult ) return -1;

			PRTMSG(MSG_DBG,"Audio Mode switch to earphone!\n");

			IOSet(IOS_EARPHONESEL, IO_EARPHONESEL_ON, &dwIoSetSymb, sizeof(dwIoSetSymb));
			IOSet(IOS_AUDIOSEL, IO_AUDIOSEL_PHONE, NULL, 0);
			IOSet(IOS_TRUMPPOW, IO_TRUMPPOW_ON, &dwIoSetSymb, sizeof(dwIoSetSymb));
	
		}
		break;

	case EVDO_AUDIOMODE_LSN: //监听模式
		{
			PRTMSG(MSG_DBG,"Audio Mode switch to lsn!\n");

			IOSet(IOS_TRUMPPOW, IO_TRUMPPOW_OFF, &dwIoSetSymb, sizeof(dwIoSetSymb));
			IOSet(IOS_AUDIOSEL, IO_AUDIOSEL_PHONE, NULL, 0);
			IOSet(IOS_EARPHONESEL, IO_EARPHONESEL_OFF, &dwIoSetSymb, sizeof(dwIoSetSymb));

// 硬件上不支持对手机模块自身的双通道切换,而是通过手机模块的外围电路通道切换实现的
// 			int iResult = _SetAT( 5000, 3, NULL, 0, NULL, 0, "AT^DAUDO=2,5" );
// 			if( EVDO_ATANS_OK != iResult )
// 			{
// 				return -1;
// 			}
// 
// 			iResult = _SetAT( 5000, 3, NULL, 0, NULL, 0, "AT^DAUDI=1,2" );
// 			if( EVDO_ATANS_OK != iResult )
// 			{
// 				return -1;
// 			}

// 		if(!_SetAT(5, 1, 0, "AT$AUVOL=0\r\n"))			return false;	// vol 增益
// 		if(!_SetAT(5, 1, 0, "AT$AUGAIN=1,9\r\n"))		return false;	// mic 增益
		}
		break;

	case EVDO_AUDIOMODE_DIAL: // 拨号模式(非监听拨号)
		{
			PRTMSG(MSG_DBG,"Audio Mode switch to dial!\n");

			// (耳机还是免提并不指定)

			IOSet(IOS_AUDIOSEL, IO_AUDIOSEL_PHONE, NULL, 0);
			IOSet(IOS_TRUMPPOW, IO_TRUMPPOW_ON, &dwIoSetSymb, sizeof(dwIoSetSymb));
		}
		break;

	case EVDO_AUDIOMODE_CLOSE: // 通话关闭模式
		{
			PRTMSG(MSG_DBG,"Audio Mode switch to close!\n");

			IOSet(IOS_TRUMPPOW, IO_TRUMPPOW_OFF, &dwIoSetSymb, sizeof(dwIoSetSymb));
			IOSet(IOS_AUDIOSEL, IO_AUDIOSEL_PC, NULL, 0);
			// (耳机还是免提并不指定)
		}
		break;

	default:
		return -1; // 必须return
	}

	tagEvdoPhoneStaCommon objSta = _PhoneStaCommonGet(0);
	objSta.m_bytCurAudioMode = v_iChgToMode;
	_PhoneStaCommonSet( objSta, EVDO_PHSTVAR_AUDIOMODE);

	return 0;
}

int CPhoneEvdo::_PhoneAudioOutVolSet(int v_iChgDegree)
{
	tagEvdoPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	
	objPhoneSta.m_iAudioOutVol += v_iChgDegree;
	if( objPhoneSta.m_iAudioOutVol < 0 ) objPhoneSta.m_iAudioOutVol = 0;
	if( objPhoneSta.m_iAudioOutVol > 4 ) objPhoneSta.m_iAudioOutVol = 4;
//m_iAudioOutVol==0时，为静音，需注意
	int iResult = _SetAT( 2500, 2, NULL, 0, NULL, 0, "AT+CLVL=%d", objPhoneSta.m_iAudioOutVol );

	if( EVDO_ATANS_OK == iResult )
	{
		_PhoneStaCommonSet( objPhoneSta, EVDO_PHSTVAR_AUDIOOUTLVL );
		return 0;
	}
	else
	{
		return -1;
	}
}

//-----------------------------------------------------------------------------------------------------------------
// 发送短信(将待发送的短信压入发送队列)
// to: 目的手机号字符串指针
// tolen: 目的手机号字符串长度
// msg: 待发短信内容指针
// msglen: 待发短信内容长度
// 返回: 1 成功
//		 2 失败
//		 3 短信内容太长
int CPhoneEvdo::_PushSndSms(char *to, byte tolen, char *msg, byte msglen, BYTE v_bytSrcType)
{
	// 判断读入的字符串长度
//	if(m_simlack)								{ return 2; }
	if(tolen==0 || tolen>15 || msglen==0)		{ return 2;	}
	if(msglen>160)								{ return 3; }

	//去掉号码前的"+"
	char tel[16] = {0};
	if(to[0]=='+')		memcpy(tel, to+1, min(tolen-1,16));
	else 				memcpy(tel, to, min(tolen,16));

	//在号码前加"86"
	if(tel[0]!='8' && tel[1]!='6')
	{
		char tmp[16] = {0};
		tmp[0] = '8';	
		tmp[1] = '6';
		memcpy(tmp+2, tel, min(strlen(tel),16));
		memset(tel, 0, 16);
		memcpy(tel, tmp, min(strlen(tmp),16));
	}
	tel[ sizeof(tel) - 1 ] = 0;

	// 将短信存入发送队列
	DWORD dwPktNum = 0;
	//que lvl 5->1
	int iResult = m_objDataMngSmsSnd.PushData(1, msglen, msg, dwPktNum, v_bytSrcType, tel, strlen(tel) );

	return iResult ? 2 : 1;

// 	uchar tomsg[16+161] = {0};
// 	memcpy(tomsg, tel, 16);
// 	memcpy(tomsg+16, msg, min(msglen,161));
// 	if(m_stack.push(tomsg, 16+161))	return 1;
// 	else							return 2;
}

//-----------------------------------------------------------------------------------------------------------------
// 处理短信的函数
// psm: 短信的结构指针
bool CPhoneEvdo::sms_deal(SM_PARAM *psm)
{
	// 非未读短信不处理
	if( 0 != psm->type )
	{
		return true;
	}

	byte len1 = min(strlen(psm->tpa),16);
	byte len2 = min(strlen(psm->ud),161);
	if( 0 == len1 )	
	{ 
		PRTMSG(MSG_ERR,"Err's SM: tpa=0\n"); 
		return false; 
	}
	if( 0 ==len2 )	
	{ 
		PRTMSG(MSG_ERR,"Err's SM: ud=0\n"); 
		return false; 
	}

	char buf[1024] = {0};
	ushort buflen = 0;
	unsigned char uszResult;

	// 进行6转8,然后根据校验和判断是否是流媒体短信
	buflen = min(convert6to8(psm->ud, buf, len2),1024);
	if(buflen>0) {
		if(check_crc1((uchar*)buf, buflen))	{
			PRTMSG(MSG_DBG,"Rcv Liu SM!\n");
			buf[0] = 0x37;	//数据类型
			DataPush(buf, buflen+1, DEV_PHONE, DEV_UPDATE, 2);	//发往流媒体
			return true;
		}
	}

	// 进行6转7,然后根据校验和判断是否是千里眼短信
	memset(buf, 0, 1024);
	buflen = min(convert6to7(psm->ud, buf+1, len2),1024);
	if(buflen>0) {
		if(check_crc2((uchar*)(buf+1), buflen))	{
			PRTMSG(MSG_DBG,"Rcv Qian SM!!\n");
			buf[0] = 0x37;	//数据类型
			DataPush(buf, buflen+1, DEV_PHONE, DEV_QIAN, 2);	//发往千里眼
			return true;
		}
	}

	// 检查是否是普通配置短信(通过手机直接配置的短信)
	UserSM *pusm = (UserSM*)psm->ud;
	if( pusm->add1=='+' && pusm->add2=='+' && pusm->add3=='+' && pusm->add4=='+' && pusm->add5=='+' && pusm->add6=='+' && pusm->add7=='+'
		&& pusm->add8 =='+' && pusm->add9=='+' && pusm->add10=='+' && pusm->add11=='+' ) 
	{
		PRTMSG(MSG_DBG, "Rcv Cfg SM!\n");

		tag1PComuCfg cfg;
		GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
		tag1QIpCfg cfg2[2];
		GetImpCfg( &cfg2, sizeof(cfg2), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg2) );
		tag1LComuCfg cfg3;
		GetImpCfg( &cfg3, sizeof(cfg3), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg3) );

		// 填充手机号
		memset(cfg.m_szTel, 0, sizeof(cfg.m_szTel) );
		memcpy(cfg.m_szTel, pusm->mytel, sizeof(pusm->mytel));

		// 填充千里眼参数
		DWORD ipScan[4] = {0};
		DWORD ip = 0;
		sscanf(pusm->ip1, "%03d%03d%03d%03d", ipScan, ipScan+1, ipScan+2, ipScan+3);

		ip = ipScan[0] | ipScan[1]<<8 | ipScan[2]<<16 | ipScan[3]<<24;
		cfg2[0].m_ulQianTcpIP = ip;
		cfg2[0].m_usQianTcpPort = htons( (ushort)atol(pusm->port1) );

		sscanf(pusm->ip2, "%03d%03d%03d%03d", ipScan, ipScan+1, ipScan+2, ipScan+3);
		ip = ipScan[0] | (ipScan[1]<<8) | (ipScan[2]<<16 | (ipScan[3]<<24) );
		cfg2[0].m_ulQianUdpIP = ip;
		cfg2[0].m_usQianUdpPort = htons( (ushort)atol(pusm->port2) );

		// 填充DVR参数
		sscanf(pusm->ip3, "%03d%03d%03d%03d", ipScan, ipScan+1, ipScan+2, ipScan+3);
		ip = ipScan[0] | (ipScan[1]<<8) | (ipScan[2]<<16 | (ipScan[3]<<24) );
		cfg2[1].m_ulQianTcpIP = ip;
		cfg2[1].m_usQianTcpPort = htons( (ushort)atol(pusm->port3) );

		// 填充流媒体参数
		sscanf(pusm->ip4, "%03d%03d%03d%03d", ipScan, ipScan+1, ipScan+2, ipScan+3);
		ip = ipScan[0] | (ipScan[1]<<8) | (ipScan[2]<<16 | (ipScan[3]<<24) );
		cfg3.m_ulLiuIP = ip;
		cfg3.m_usLiuPort = htons( (ushort)atol(pusm->port4) );

		// 填充维护中心参数
		sscanf(pusm->ip5, "%03d%03d%03d%03d", ipScan, ipScan+1, ipScan+2, ipScan+3);
		ip = ipScan[0] | (ipScan[1]<<8) | (ipScan[2]<<16 | (ipScan[3]<<24) );
		cfg3.m_ulLiuIP2 = ip;
		cfg3.m_usLiuPort2 = htons( (ushort)atol(pusm->port5) );
		
		// 填充APN
		memset(cfg2[0].m_szApn, 0, sizeof(cfg2[0].m_szApn) );
		memcpy(cfg2[0].m_szApn, pusm->apn, strlen(pusm->apn));

		// 保存配置
		bool succ = true;
		if(SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) ))		succ = false;
		if(SetImpCfg(&cfg2, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg2) ))		succ = false;
		if(SetImpCfg(&cfg3, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg3) ))		succ = false;


		// 通知流媒体和千里眼参数有变化
		char frm1 = 0x43;	
		DataPush(&frm1, 1, DEV_PHONE, DEV_UPDATE, 2);
		Frm35 frm2;			
		frm2.reply = 0x01;
		DataPush((char*)&frm2, sizeof(frm2), DEV_PHONE, DEV_SOCK, 2);
		
		//通知DvrExe手机号码改变了
		char by = 0x0c;
		DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);

		// 给手机返回配置应答
		char msg[1024] = {0};

		// 添加车台手机号和软件版本
		sprintf(msg, "Set %.15s Succ! Ver:", cfg.m_szTel);
		GetAppVer( msg+strlen(msg), sizeof(msg)-strlen(msg) - 200 );

		in_addr addr1, addr2, addr3, addr4, addr5;
		addr1.s_addr = cfg2[0].m_ulQianTcpIP;
		addr2.s_addr = cfg2[0].m_ulQianUdpIP;
		addr3.s_addr = cfg2[1].m_ulQianTcpIP;
		addr4.s_addr = cfg3.m_ulLiuIP;
		addr5.s_addr = cfg3.m_ulLiuIP2;

		char ip1[32] = {0};
		char ip2[32] = {0};
		char ip3[32] = {0};
		char ip4[32] = {0};
		char ip5[32] = {0};

		strcpy( ip1, inet_ntoa(addr1) );
		strcpy( ip2, inet_ntoa(addr2) );
		strcpy( ip3, inet_ntoa(addr3) );
		strcpy( ip4, inet_ntoa(addr4) );
		strcpy( ip5, inet_ntoa(addr5) );

		// 添加千里眼和流媒体的IP和端口
		sprintf(msg+strlen(msg), " Q:%s,%d;%s,%d;%s,%d", ip1, ntohs(cfg2[0].m_usQianTcpPort),
			ip2, ntohs(cfg2[0].m_usQianUdpPort), ip3, ntohs(cfg2[1].m_usQianTcpPort) );
		sprintf( msg+strlen(msg), " L:%s,%d", ip4, ntohs(cfg3.m_usLiuPort) );
		sprintf( msg+strlen(msg), " M:%s,%d", ip5, ntohs(cfg3.m_usLiuPort2) );
		sprintf(msg+strlen(msg), " A:%s", cfg2[0].m_szApn);

		_PushSndSms( psm->tpa, strlen(psm->tpa), msg, min(strlen(msg), 160), 0 );

		// 添加GSM信号、GPRS在线情况、ACC状态、GPS定位情况
		memset( msg, 0, sizeof(msg) );
		tagEvdoPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
		GPSDATA gps(0);
		GetCurGps( &gps, sizeof(gps), GPS_LSTVALID );
		sprintf(msg+strlen(msg), " Sig:%d", objPhoneSta.m_iSignal);
		sprintf(msg+strlen(msg), " NET:%s", objPhoneSta.m_bNetConnected ? "on" : "off");
		IOGet((byte)IOS_ACC,&uszResult);
		sprintf(msg+strlen(msg), " Acc:%s", IO_ACC_ON == uszResult ? "on" : "off");
		sprintf(msg+strlen(msg), " Gps:%c", gps.valid);
		
		_PushSndSms( psm->tpa, strlen(psm->tpa), msg, min(strlen(msg), 160), 0 );	//目前 strlen(msg)==149
		return true;
	}

	UserSM2 *pusm2 = (UserSM2*)psm->ud;

	// 检查是否是普通查询短信(通过手机直接查询的短信)
	if( 0==strncmp(pusm2->pass,"4312896654",10) && 0==strncmp(pusm2->cmd,"+query",6) ) 
	{
		PRTMSG(MSG_DBG, "Rcv Query SM!!\n");

		tag1PComuCfg cfg;
		GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
		tag1QIpCfg cfg2[2];
		GetImpCfg( &cfg2, sizeof(cfg2), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg2) );
		tag1LComuCfg cfg3;
		GetImpCfg( &cfg3, sizeof(cfg3), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg3) );

		char msg[1024] = {0};

		// 添加车台手机号和软件版本
		sprintf(msg, "%.15s Ver:", cfg.m_szTel);
		GetAppVer( msg+strlen(msg), sizeof(msg)-strlen(msg) - 200);

		in_addr addr1, addr2, addr3, addr4, addr5;
		addr1.s_addr = cfg2[0].m_ulQianTcpIP;
		addr2.s_addr = cfg2[0].m_ulQianUdpIP;
		addr3.s_addr = cfg2[1].m_ulQianTcpIP;
		addr4.s_addr = cfg3.m_ulLiuIP;
		addr5.s_addr = cfg3.m_ulLiuIP2;

		char ip1[32] = {0};
		char ip2[32] = {0};
		char ip3[32] = {0};
		char ip4[32] = {0};
		char ip5[32] = {0};

		strcpy( ip1, inet_ntoa(addr1) );
		strcpy( ip2, inet_ntoa(addr2) );
		strcpy( ip3, inet_ntoa(addr3) );
		strcpy( ip4, inet_ntoa(addr4) );
		strcpy( ip5, inet_ntoa(addr5) );
		
		// 添加千里眼和流媒体的IP和端口
		sprintf(msg+strlen(msg), " Q:%s,%d;%s,%d;%s,%d", ip1, ntohs(cfg2[0].m_usQianTcpPort),
			ip2, ntohs(cfg2[0].m_usQianUdpPort), ip3, ntohs(cfg2[1].m_usQianTcpPort) );
		sprintf(msg+strlen(msg), " L:%s,%d", ip4, ntohs(cfg3.m_usLiuPort));
		sprintf(msg+strlen(msg), " M:%s,%d", ip5, ntohs(cfg3.m_usLiuPort2));
		sprintf(msg+strlen(msg), " A:%s", cfg2[0].m_szApn);

		_PushSndSms( psm->tpa, strlen(psm->tpa), msg, min(strlen(msg), 160), 0 );

		char tpa[50] = { 0 };
		memcpy( tpa, psm->tpa, min( strlen(psm->tpa), 25) );
		PRTMSG( MSG_NOR, "Send Query Res Msg to %s: \"%s\"\n", tpa, msg );
		
		return true;
	}
	
	// 检查是否是状态查询短信(通过手机查询状态的短信)
	if( 0==strncmp(pusm2->pass,"4312896654",10) && 0==strncmp(pusm2->cmd,"+state",6) ) 
	{
		PRTMSG(MSG_DBG, "Rcv State SM!!\n");

		char msg[1024] = {0};

		// 添加GSM信号、GPRS在线情况、ACC状态、GPS定位情况
		tagEvdoPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
		sprintf(msg+strlen(msg), " Sig:%d", objPhoneSta.m_iSignal);
		sprintf(msg+strlen(msg), " Net:%s", objPhoneSta.m_bNetConnected ? "on" : "off");
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		sprintf(msg+strlen(msg), " Tcp:%s", g_objLightCtrl2.m_bytTcpState ? "on" : "off");
#endif
		IOGet((byte)IOS_ACC,&uszResult);
		sprintf(msg+strlen(msg), " Acc:%s", IO_ACC_ON == uszResult ? "on" : "off");
		GPSDATA gps(0);
		GetCurGps( &gps, sizeof(gps), GPS_LSTVALID );
		sprintf(msg+strlen(msg), " Gps:%c", gps.valid);	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		IOGet((byte)IOS_POWDETM, &uszResult);
		sprintf(msg+strlen(msg), " Pow:%s", IO_POWDETM_VALID == uszResult ? "on" : "off");	
		IOGet((byte)IOS_QIANYA, &uszResult);
		sprintf(msg+strlen(msg), " Vol:%s", IO_QIANYA_INVALID == uszResult ? "on" : "off");	
		sprintf(msg+strlen(msg), " Sys:%s", g_objLightCtrl2.m_bytDvrState == 0x00 ? "sleep" : 
																						(g_objLightCtrl2.m_bytDvrState == 0x01 ? "idle" : 
																							(g_objLightCtrl2.m_bytDvrState == 0x02 ? "work" : 
																								(g_objLightCtrl2.m_bytDvrState == 0x03 ? "ctrl" : "idle"))));
			
		//添加GPS模块、手机模块、TW2865模块、SD卡状态
		sprintf(msg+strlen(msg), " DevErr:0x%08x", g_objLightCtrl2.m_dwDevErrCode);
		sprintf(msg+strlen(msg), " CfgErr:0x%08x", g_objLightCtrl2.m_dwCfgErrCode);
		sprintf(msg+strlen(msg), " TelErr:0x%08x", g_objLightCtrl2.m_dwTelErrCode);
		sprintf(msg+strlen(msg), " MonErr:0x%08x", g_objLightCtrl2.m_dwMonErrCode);
			
		_PushSndSms( psm->tpa, strlen(psm->tpa), msg, min(strlen(msg), 160), 0 );
		
		memset( msg, 0, sizeof(msg) );
		sprintf(msg+strlen(msg), " ReBoot:%s", g_objLightCtrl2.m_szReBootTime);
		sprintf(msg+strlen(msg), " SwiLink:%s", g_objLightCtrl2.m_szSwiLinkTime);
		sprintf(msg+strlen(msg), " DiskLink:%s", g_objLightCtrl2.m_szDiskLinkTime);
		sprintf(msg+strlen(msg), " MonLink:%s", g_objLightCtrl2.m_szMonLinkTime);
		sprintf(msg+strlen(msg), " Deal3801:%s(%d)", g_objLightCtrl2.m_szDeal3801Time, g_objLightCtrl2.m_iDeal3801Cnt);
		sprintf(msg+strlen(msg), " VideoMon:%s(%d)", g_objLightCtrl2.m_szVideoMonTime, g_objLightCtrl2.m_iVideoMonCnt);
		
		_PushSndSms( psm->tpa, strlen(psm->tpa), msg, min(strlen(msg), 160), 0 );
		
		memset( msg, 0, sizeof(msg) );
		sprintf(msg+strlen(msg), " TelOn:%s(%d)", g_objLightCtrl2.m_szTelOnTime, g_objLightCtrl2.m_iTelOnCnt);
		sprintf(msg+strlen(msg), " TelInit:%s(%d)", g_objLightCtrl2.m_szTelInitTime, g_objLightCtrl2.m_iTelInitCnt);
		sprintf(msg+strlen(msg), " LowSig:%s(%d)", g_objLightCtrl2.m_szLowSigTime, g_objLightCtrl2.m_iLowSigCnt);
		sprintf(msg+strlen(msg), " DialNet:%s(%d)", g_objLightCtrl2.m_szDialNetTime, g_objLightCtrl2.m_iDialNetCnt);
		sprintf(msg+strlen(msg), " TcpCon:%s(%d)", g_objLightCtrl2.m_szTcpConTime, g_objLightCtrl2.m_iTcpConCnt);
		sprintf(msg+strlen(msg), " LogIn:%s(%d)", g_objLightCtrl2.m_szLogInTime, g_objLightCtrl2.m_iLogInCnt);

#endif
// 		// 添加车台ID和密钥
// 		char str[21] = {0};
// 		GetDevID(str, sizeof(str)); 
// 		sprintf(msg+strlen(msg), " Id:%d", *(ulong*)str);
// 		sprintf(msg+strlen(msg), " Key:%.16s", str+4);
		
		_PushSndSms( psm->tpa, strlen(psm->tpa), msg, min(strlen(msg), 160), 0 );

		char tpa[50] = { 0 };
		memcpy( tpa, psm->tpa, min( strlen(psm->tpa), 25) );
		PRTMSG( MSG_NOR, "Send State Res Msg to %s: \"%s\"\n", tpa, msg );
		
		return true;
	}

	// 检查是否是复位系统的短信(通过手机复位的短信)
	if( 0==strncmp(pusm2->pass,"4312896654",10) && 0==strncmp(pusm2->cmd,"+reset",6) ) 
	{
		PRTMSG(MSG_DBG,"Rcv SysReset SM!!\n");
		G_ResetSystem();//子线程内部请求复位（退出进程）
		return true;
	}

	// 检查是否恢复出厂配置的短信
	if( 0==strncmp(pusm2->pass,"4312896654",10) && 0==strncmp(pusm2->cmd,"+resume",7) ) 
	{
		PRTMSG(MSG_DBG,"Rcv ResumeCfg SM!!\n");
		
		// 给手机返回配置应答
		char msg[1024] = {0};
		
		if( 0 == ResumeCfg(true, true) )
		{		
			g_objDiaodu.show_diaodu("手机短信恢复出厂配置成功！");
			
			// 通知流媒体和千里眼参数有变化
			char frm1 = 0x43;	
			DataPush(&frm1, 1, DEV_PHONE, DEV_UPDATE, 2);
			Frm35 frm2;			
			frm2.reply = 0x01;
			DataPush((char*)&frm2, sizeof(frm2), DEV_PHONE, DEV_SOCK, 2);
			
			//通知DvrExe手机号码改变了
			char by = 0x0c;
			DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);

			tag1PComuCfg cfg;
			GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
			tag1QIpCfg cfg2[2];
			GetImpCfg( &cfg2, sizeof(cfg2), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg2) );
			tag1LComuCfg cfg3;
			GetImpCfg( &cfg3, sizeof(cfg3), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg3) );
			
			// 添加车台手机号和软件版本
			sprintf(msg, "ResumeCfg %.15s Succ! Ver:", cfg.m_szTel);
			GetAppVer( msg+strlen(msg), sizeof(msg)-strlen(msg) - 200 );
			
			in_addr addr1, addr2, addr3, addr4, addr5;
			addr1.s_addr = cfg2[0].m_ulQianTcpIP;
			addr2.s_addr = cfg2[0].m_ulQianUdpIP;
			addr3.s_addr = cfg2[1].m_ulQianTcpIP;
			addr4.s_addr = cfg3.m_ulLiuIP;
			addr5.s_addr = cfg3.m_ulLiuIP2;
			
			char ip1[32] = {0};
			char ip2[32] = {0};
			char ip3[32] = {0};
			char ip4[32] = {0};
			char ip5[32] = {0};
			
			strcpy( ip1, inet_ntoa(addr1) );
			strcpy( ip2, inet_ntoa(addr2) );
			strcpy( ip3, inet_ntoa(addr3) );
			strcpy( ip4, inet_ntoa(addr4) );
			strcpy( ip5, inet_ntoa(addr5) );
			
			// 添加千里眼和流媒体的IP和端口
			sprintf(msg+strlen(msg), " Q:%s,%d;%s,%d;%s,%d", ip1, ntohs(cfg2[0].m_usQianTcpPort),
				ip2, ntohs(cfg2[0].m_usQianUdpPort), ip3, ntohs(cfg2[1].m_usQianTcpPort) );
			sprintf( msg+strlen(msg), " L:%s,%d", ip4, ntohs(cfg3.m_usLiuPort) );
			sprintf( msg+strlen(msg), " M:%s,%d", ip5, ntohs(cfg3.m_usLiuPort2) );
			sprintf(msg+strlen(msg), " A:%s", cfg2[0].m_szApn);		
		}
		else
		{
			sprintf(msg, "ResumeCfg Failed!");
		}
		
		_PushSndSms( psm->tpa, strlen(psm->tpa), msg, min(strlen(msg), 160), 0 );
		
		return true;
	}

	// 都不是,则认为是用户短信,发往调度屏
	PRTMSG(MSG_DBG,"User's SM!\n");
	char frm[1024] = {0};
	int p = 0;
	DWORD dwPktNum;
	frm[p] = 0x33;					p++;		//接收短消息指示
	if( psm->tpa[0] == '8' && psm->tpa[1] == '6' )
	{
		frm[p] = len1-2;					p++;		//主叫手机号码长度
		memcpy(frm+p, psm->tpa+2, len1-2);	p+=len1-2;	//主叫手机号码
	}
	else if( psm->tpa[0] == '+' && psm->tpa[1] == '8' && psm->tpa[2] == '6' )
	{
		frm[p] = len1-3;					p++;
		memcpy(frm+p, psm->tpa+3, len1-3);	p+=len1-3;
	}
	else
	{
		frm[p] = len1;					p++;
		memcpy(frm+p, psm->tpa, len1);	p+=len1;
	}
	memcpy(frm+p, psm->scts, 12);	p+=12;		//接收时间
	frm[p] = len2;					p++;		//短消息内容长度
	memcpy(frm+p, psm->ud, len2);	p+=len2;	//短消息内容
	g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, p, frm, dwPktNum);	//发往调度屏

	return true;
}

//手机模块拨号上网
int CPhoneEvdo::_PhoneDialNet()
{	
	if (SIG_ERR == G_Signal(SIGUSR1, G_EVDO_PppSigHandler) )
	{
		PRTMSG(MSG_ERR,"failed to set the SIGUSR1 handler function for ppp dial\n");
		return -1;
	}

	if( 0 != _ForkPppd())
	{
		return ERR_THREAD;
	}

//	sleep(3);

	//	que 此处需加ppp拨号完成情况的判断,但拨号时间太长，会不会占用EVDO_PST_DIALNET状态太久，影响idle状态的检测
	return 0;
}

//创建拨号进程pppd
int CPhoneEvdo::_ForkPppd()
{
	int iRet;
	pid_t pidPppd;
	char szUsbSerialPathName[256] = {0};
	struct dirent **ppUsbSerialNameList;
	scandir(USB_DEV_CHECKPATH_PPP, &ppUsbSerialNameList, 0, alphasort);
	sprintf(szUsbSerialPathName, "/dev/usb/tts/%c", *(ppUsbSerialNameList[12]->d_name + 6));
	if( 0 != access(szUsbSerialPathName, F_OK) )
	{
		PRTMSG(MSG_ERR,"usb device port for ppp check err\n");
		return ERR_COM;
	}

	// 获取APN
	char szUserName[64] = { 0 };
	char szPassWord[64] = { 0 };

	tag1PComuCfg objComuCfg;
	int iResult = GetImpCfg( &objComuCfg, sizeof(objComuCfg),	
		offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );

	if( !iResult )
	{
		strncpy( szUserName, objComuCfg.m_szCdmaUserName, min(strlen(objComuCfg.m_szCdmaUserName), sizeof(szUserName) - 1) );
		strncpy( szPassWord, objComuCfg.m_szCdmaPassWord, min(strlen(objComuCfg.m_szCdmaPassWord), sizeof(szPassWord) - 1) );
	}
	else
	{
		strcpy( szUserName, "CARD" );
		strcpy( szPassWord, "CARD" );
	}
	
	char szResult[64] = {0};
	char *pTargetUs = NULL;
	char *pTargetPw = NULL;
	char *pBegin = NULL;
	char szCfgUs[64] = { 0 };
	char szCfgPw[64] = { 0 };
	int i=0,j=0;
	
	//	AT^PPPCFG?	查询拨号用户和密码,属于非丢失配置数据
	//	^PPPCFG:"ctnet@mycdma.cn","vnet.mobi"
	//	^PPPCFG:"card","card"
	
	iResult = _SetAT(3000, 2, szResult, sizeof(szResult), G_EVDO_CPPPCFG_Jugres, 0, "AT^PPPCFG?");
	if( EVDO_ATANS_OK != iResult )
	{
		_SetAT(3000, 2, NULL, 0, NULL, 0, "AT^PPPCFG=\"CARD\",\"CARD\"");
		PRTMSG(MSG_DBG, "set ppp dial user name1:card,password:card\n");
	}
	else
	{
		pBegin = strstr(szResult, "^PPPCFG:");
		pTargetUs = pBegin+strlen("^PPPCFG:");//user name
		pTargetUs += 1;
		pTargetPw = strstr(szResult, ",");//逗号标志
		pTargetPw += 2;//password
		if ( (NULL != pBegin) && (NULL != pTargetPw) && (pBegin<pTargetPw) )
		{
			while( ('"' != *pTargetUs) && i<sizeof(szCfgUs))
			{
				szCfgUs[i++] = *pTargetUs;
				pTargetUs++;
			}
			while( ('"' != *pTargetPw) && j<sizeof(szCfgPw) )
			{
				szCfgPw[j++] = *pTargetPw;
				pTargetPw++;
			}
			if( 0==strncasecmp(szCfgUs, szUserName, max(strlen(szUserName),strlen(szCfgUs))) && 
				0==strncasecmp(szCfgPw, szPassWord, max(strlen(szPassWord),strlen(szCfgPw))) )
			{
				PRTMSG(MSG_DBG, "PPPCFG no need to set\n");
			}
			else
			{
				_SetAT(3000, 2, NULL, 0, NULL, 0, "AT^PPPCFG=\"%s\",\"%s\"", szUserName, szPassWord);
				PRTMSG(MSG_DBG, "set ppp dial user name2:%s,password:%s\n", szUserName, szPassWord);
			}
		}
		else
		{
			_SetAT(3000, 2, NULL, 0, NULL, 0, "AT^PPPCFG=\"%s\",\"%s\"", szUserName, szPassWord);
			PRTMSG(MSG_DBG, "set ppp dial user name3:%s,password:%s\n", szUserName, szPassWord);
		}
	}
	
	pidPppd = fork();
	switch(pidPppd)
	{
	case -1:
		PRTMSG(MSG_ERR,"failed to fork a new process\n");
		return ERR_THREAD;
		
	case 0:
		iRet = execl("/etc/ppp/ppp-on", "ppp-on",szUsbSerialPathName, PPP_DIAL_NUM, szUserName, szPassWord, NULL);
		if (-1 == iRet) 
		{
			PRTMSG(MSG_ERR,"failed to execl a new program\n");
			return ERR_THREAD;		
		}
		break;
		
	default:
		{
			PRTMSG(MSG_NOR,"fork DialNet process succ\n");

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
			g_objLightCtrl2.SetLightSta(0x07);	// 开始拨号通知
#endif
			sleep(5);
		}
		break;
	}

	return 0;
}


int CPhoneEvdo::_PppSigHandler()
{
	m_bPhoneNetReady = true;
}


void CPhoneEvdo::_PhoneCutNet()
{	
	m_bPhoneNetReady = false;
	PRTMSG(MSG_NOR,"Cut Net...\n");
	system("killall pppd");	//kill pppd拨号进程，可能重复操作
	sleep(10);
}

//-----------------------------------------------------------------------------------------------------------------
// 接收千里眼中心的短信需要进行6转7(SMS)
// sptr: 源字符串指针
// dptr: 目的字符串指针
// len: 源字符串长度
// 返回: 目的字符串长度
int CPhoneEvdo::convert6to7(char *sptr, char *dptr, int len)
{
	static byte mask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    byte i, j, k, stemp, dtemp;
    int rlen;
    
    rlen  = 0;
    i     = 0;
    dtemp = 0;
    j = 7;

    for (;;) {
        if (j == 0) {
            j = 7;
            *dptr++ = dtemp;
            rlen++;
            dtemp = 0;
            if (len == 0) break;
        }
        if (i == 0) {
            if (len == 0) {
                rlen = 0;
                break;
            }
            
            char data = *sptr++;
            stemp = 0xff;
			for(k=0; k< sizeof(MapTable); k++) {
				if(data == MapTable[k])		{ stemp = k ; break; }
			}
			
            if (stemp == 0xff) {
                rlen = 0;
                break;
            } else {
                i = 6;
                len--;
            }
        }
        if(stemp & mask[i-1])		dtemp |= mask[j-1];
        i--;
        j--;
    }
    return rlen;
}


//-----------------------------------------------------------------------------------------------------------------
//接收流媒体中心的短信需要进行6转8(SMS)
// strOldBuf: 源字符串指针
// strNewBuf: 目的字符串指针
// iOldLen: 源字符串长度
// 返回: 目的字符串长度
int CPhoneEvdo::convert6to8(const char* strOldBuf,char* strNewBuf,const int iOldLen)
{
	int		i=0;	//待转化的字符计数器
	int		k=0;	//已转化的字符下标
	int		l=8;	//要需要的比特数
	int		iNewLen;
	byte	strOldBuf1[500];

	if((DWORD)iOldLen>500)	return 0;

	// 查表转换
	memcpy( strOldBuf1, strOldBuf, iOldLen);
	for( i=0; i<iOldLen; i++ )
	{
		for( l=0; l<64; l++ )
		{
			if( strOldBuf1[i]==MapTable[l] )
			{
				strOldBuf1[i] = l;
				break;
			}
		}
	}
		
	l=8;k=0;
	for(i=0;i<iOldLen;i++)
	{
		strNewBuf[i]=0;
		strOldBuf1[i]=((unsigned char)strOldBuf1[i] & 0x3f) << 2;
	}
	for(i=0;i<iOldLen;i++)
	{
		if(l<6)
		{
			strNewBuf[k]=(unsigned char)strNewBuf[k] | ((unsigned char)strOldBuf1[i] >> (8-l));
			k++;
			strNewBuf[k]=(unsigned char)strOldBuf1[i] << l;
			l=8-(6-l);
		}else if(l==6)
		{
			strNewBuf[k]=(unsigned char)strNewBuf[k] | ((unsigned char)strOldBuf1[i] >> (8-l));
			k++;
			l=8;
		}else
		{
			strNewBuf[k]=(unsigned char)strNewBuf[k] | (unsigned char)strOldBuf1[i];
			l -= 6;
		}
	}
	iNewLen=k;
	
	return iNewLen;	
}

//-----------------------------------------------------------------------------------------------------------------
// 得到校验和(查表)
// buf: 字符串指针
// len: 字符串长度
// 返回: 校验和
byte CPhoneEvdo::get_crc1(const byte *buf, const int len) 
{   
	int	j = 0;
	byte sum = 0xff;

	for( int i=len; i>0; i-- ) {
		sum = CrcTable[sum ^ buf[j++]];
	}
	return (0xff-sum);
}


//--------------------------------------------------------------------------------------------------------------------------
// 得到校验和(直接取累加和)
// buf: 字符串指针
// len: 字符串长度
// 返回: 校验和
byte CPhoneEvdo::get_crc2(const byte *buf, const int len)
{   
	byte bytRet;
	bytRet = get_crc_sms( buf, len );
	return bytRet;
}

//--------------------------------------------------------------------------------------------------------------------------
// 检查校验和(查表)
// buf: 待校验字符串指针
// len: 待校验字符串长度
// 返回: 校验和是否正确
bool CPhoneEvdo::check_crc1(const byte *buf, const int len) 
{   
	byte sum = get_crc1( buf+1, len-1 );
	if( buf[0]==sum )
		return true;
	else 
		return false;
}


//--------------------------------------------------------------------------------------------------------------------------
// 检查校验和(直接取累加和)
// buf: 待校验字符串指针
// len: 待校验字符串长度
// 返回: 校验和是否正确
bool CPhoneEvdo::check_crc2(const byte *buf, const int len)
{   
	byte sum = get_crc2( buf, len-1 );
	if( buf[len-1]==sum )		
		return true;
	else						
		return false;
}

// 此函数在生产检测版本中会被调用到，用于在断电时判断手机模块是否正常
bool CPhoneEvdo::IsPhoneValid()
{
	// AT测试
	if( EVDO_ATANS_OK != _SetAT(500, 5, 0, 100, NULL, 0, NULL, 0, "AT") )
		return false;
	else
		return true;
}


