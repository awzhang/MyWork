// PhoneGsm.cpp: implementation of the CPhoneGsm class.
//
//////////////////////////////////////////////////////////////////////

#include "ComuStdAfx.h"


#undef MSG_HEAD
#define MSG_HEAD ("ComuExe-PhoneGSM:")

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



void G_PppSigHandler(int v_signo)
{
	PRTMSG(MSG_NOR,"收到ppp拨号成功通知！\n");

	// 控制指示灯
#if VEHICLE_TYPE == VEHICLE_M
	g_objLightCtrl.SetLightSta(0x03);	// 慢闪
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	g_objLightCtrl2.SetLightSta(0x01);	// 拨号成功通知
#endif

	g_objPhoneGsm._PppSigHandler();
}

unsigned long G_PhoneHdl( void* v_pPar )
{
	if( v_pPar )
		return ((CPhoneGsm*)v_pPar)->P_PhoneHdl();
	else
		return ERR_THREAD;
}

unsigned long G_OtherHdl( void* v_pPar )
{
	if( v_pPar )
		return ((CPhoneGsm*)v_pPar)->P_OtherHdl();
	else
		return ERR_THREAD;
}

unsigned long G_ComRcv( void* v_pPar )
{
	if( v_pPar )
		return ((CPhoneGsm*)v_pPar)->P_ComRcv();
	else
		return ERR_THREAD;
}

bool G_JugNormalAsc( char v_cToJug )
{
	return (v_cToJug >= '0' && v_cToJug <= '9')
		|| (v_cToJug >= 'a' && v_cToJug <= 'z')
		|| (v_cToJug >= 'A' && v_cToJug <= 'Z');
}

int G_CGSN_Jugres(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// AT+CGSN
// 446019197507595
// OK

	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return ATANS_OTHER;
	}

	//返回没有CGSN字符串
// 	const char STR_RESSYMB[] = "+CGSN:";
// 	if( strstr(v_szAtRes, "ERROR") )
// 	{
// 		return ATANS_ERROR;
// 	}
	else if( strstr(v_szAtRes, "OK") )
	{
		//char* pAns = strstr(v_szAtRes, STR_RESSYMB);
		char* pAns = v_szAtRes;
		if( pAns )
		{
			// 寻找起始判断的位置
			//pAns += strlen( STR_RESSYMB ); // 跳过应答标志
			while( 0x20 == *pAns ) // 跳过之后的空格
			{
				pAns ++;
			}

			// 读取IMEI
			if( *pAns )
			{
				DWORD dwLen = 0;
				while( dwLen + 1 < v_sizResult )
				{
					if( !G_JugNormalAsc(pAns[dwLen]) ) // 直到非普通ASCII字符
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

				return ATANS_OK;
			}
			else
			{
				return ATANS_OTHER;
			}
		}
		else
		{
			return ATANS_OTHER;
		}
	}
	else
	{
		return ATANS_RCVNOTALL;
	}
}


int G_CGMM_Jugres(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	// AT+CGMM
	// M33
	// OK
	char* pResEnd = NULL;
	char* pEnd = NULL;
	if( !v_szAtRes )
	{
		return ATANS_OTHER;
	}
	if( strstr(v_szAtRes, "ERROR") )
	{
		return ATANS_ERROR;
	}
	pResEnd = strstr(v_szAtRes, "OK");
	if(pResEnd)
	{
		DWORD dwLen = min(pResEnd - v_szAtRes, v_sizResult - 1 );
		memcpy( v_pResult, v_szAtRes, dwLen );
		((char*)v_pResult)[ dwLen ] = 0;
		return ATANS_OK;
	}
	else
	{
		return ATANS_ERROR;
	}
	
}

int C_AUTOCAMPON_Jg(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	//在at指令上没查到
	if( !v_szAtRes )
	{
		return ATANS_OTHER;
	}
	long lRes = -1;
	const char STR_RESSYMB[] = "$AUTOCAMPON:";
	
	char *pTarget = strstr(v_szAtRes, STR_RESSYMB);
	char *pEnd = strstr(v_szAtRes, "\r");
	
	if (pTarget && pEnd )
	{
		lRes = atol(pTarget+13);
	}
	
	if (-1 != lRes)
	{
		return ATANS_OK;
	}
	else
	{
		return ATANS_ERROR;
	}
}

int G_CREG_Jugres(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	if( !v_szAtRes )
	{
		return ATANS_OTHER;
	}
	
	if( strstr(v_szAtRes, "ERROR") )
	{
		return ATANS_ERROR;
	}
	const char STR_RESSYMB[] = "+CREG:";
	char *pBegin = strstr(v_szAtRes, STR_RESSYMB);
	if(pBegin)  
	{
		char *pTarget = strstr(pBegin, ",");
		if(pTarget)
		{
			if( strstr(pTarget, "1") || strstr(pTarget, "5") )	
			{
				return ATANS_OK;
			}
			else 
				return ATANS_CMEERR;
		}
	}
	else
		return ATANS_OTHER;
}

int G_CSCA_Get(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// AT+CSCA?
// +CSCA: "+8613800200500",145
// 
// 
// 
// OK

	if( !v_szAtRes )
	{
		return ATANS_OTHER;
	}
	if( strstr(v_szAtRes, "ERROR") )
	{
		return ATANS_ERROR;
	}
	const char STR_RESSYMB[] = "+CSCA:";
	char *pHead = strstr(v_szAtRes, STR_RESSYMB);
	if (pHead)
	{
		pHead = strstr(pHead, "\"");
		if (pHead)
		{
			pHead++;
		}
		else
		{
			return ATANS_OTHER;
		}
		char *pEnd = strstr(pHead, "\"");
		if (pEnd && (pEnd-pHead)<v_sizResult && (pEnd-pHead)<15)
		{
			memcpy(v_pResult, pHead, pEnd-pHead);
			((char*)v_pResult)[ pEnd-pHead ] = 0;
			return ATANS_OK;
		}
		else
		{
			return ATANS_OTHER;
		}
	}
	else
		return ATANS_ERROR;

}

int G_CFUN5_Jugres(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// ^DUSIMU: 1
// OK

	if( !v_szAtRes )
	{
		return ATANS_OTHER;
	}

	const char STR_RESSYMB[] = "^DUSIMU:";
	if( strstr(v_szAtRes, "ERROR") )
	{
		return ATANS_ERROR;
	}
	else if( strstr(v_szAtRes, "OK") || strstr(v_szAtRes, STR_RESSYMB) )
	{
		char* pAns = strstr(v_szAtRes, STR_RESSYMB);
		if( pAns ) // 有具体的应答可以检查一下
		{
			// 寻找起始判断的位置
			pAns += strlen( STR_RESSYMB ); // 跳过应答标志
			while( 0x20 == *pAns ) // 跳过之后的空格
			{
				pAns ++;
			}

			// 读取应答
			if( *pAns )
			{
				DWORD dwLen = 0;
				char szResVal[100] = { 0 };
				while( dwLen + 1 < sizeof(szResVal) )
				{
					if( pAns[dwLen] < '0' || pAns[dwLen] > '9' ) // 直到非数字字符
					{
						break;
					}

					szResVal[dwLen] = pAns[dwLen];
					dwLen ++;
				}
				if( dwLen < sizeof(szResVal) )
				{
					szResVal[dwLen] = 0;
				}

				if( 1 == atoi(szResVal) )
					return ATANS_OK;
				else
					return ATANS_ERROR;
			}
			else
			{
				return ATANS_ERROR;
			}
		}
		else
		{
			return ATANS_OTHER;
		}
	}
	else
	{
		return ATANS_NOANS;
	}
}

int G_CFUN1_Jugres(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// AT+CFUN=1         
// 
// OK  
// 
// +CREG: 2        
// 
// +CIEV: 2,0          
// 
// ^DACTI: 2         
// 
// ^DCALLPI        
// 
// +CIEV: 2,2          
// 
// ^DACTI: 0         
// 
// +CREG: 1         

	if( !v_szAtRes )
	{
		return ATANS_OTHER;
	}

	const char STR_RESSYMB[] = "+CREG:";
	char* pStrChk = v_szAtRes;
	if( strstr(v_szAtRes, "ERROR") )
	{
		return ATANS_ERROR;
	}
	else if( strstr(pStrChk, "OK") )
	{
		// 遍历应答字符串,查找是否有成功应答
		char* pAns = NULL;
		while( pAns = strstr(pStrChk, STR_RESSYMB) )
		{
			// 寻找起始判断位置
			pAns += strlen( STR_RESSYMB ); // 跳过应答标志
			while( 0x20 == *pAns ) // 跳过之后的空格
			{
				pAns ++;
			}

			// 读取应答值
			if( *pAns )
			{
				DWORD dwLen = 0;
				char szRegVal[100] = { 0 };
				while( dwLen + 1 < sizeof(szRegVal) )
				{
					if( pAns[dwLen] < '0' || pAns[dwLen] >'9' ) // 直到非数字字符
					{
						break;
					}

					szRegVal[dwLen] = pAns[dwLen];
					dwLen ++;
				}
				int iRegVal = atoi(szRegVal);
				if( 1 == iRegVal || 5 == iRegVal )
				{
					return ATANS_OK;
				}
			}
			else
			{
				return ATANS_RCVNOTALL;
			}

			// 刷新状态准备继续寻找
			pStrChk = pAns;
		}
		
		return ATANS_RCVNOTALL;
	}
	else
	{
		return ATANS_RCVNOTALL;
	}
}

int G_CSQ_Jugres(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// AT+CSQ
// 
// +CSQ: 11,99
// 
// OK

	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return ATANS_OTHER;
	}

	const char STR_RESSYMB[] = "+CSQ:";
	if( strstr(v_szAtRes, "ERROR") )
	{
		return ATANS_ERROR;
	}
	else if( strstr(v_szAtRes, "OK") )
	{
		char* pAns = strstr(v_szAtRes, STR_RESSYMB);
		if( pAns )
		{
			// 寻找起始判断的位置
			pAns += strlen( STR_RESSYMB ); // 跳过应答标志
			while( 0x20 == *pAns ) // 跳过之后的空格
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
						return ATANS_OTHER;
					}
					else
					{
						strncpy((char*)v_pResult, pAns, sizSingalLen );
						((char*)v_pResult)[ sizSingalLen ] = 0;
						return ATANS_OK;
					}
				}
				else
				{
					return ATANS_OTHER;
				}
			}
			else
			{
				return ATANS_OTHER;
			}
		}
		else
		{
			return ATANS_OTHER;
		}
	}
	else
	{
		return ATANS_RCVNOTALL;
	}
}

int G_CSCA_Query_res(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// AT+CSCA?
// 
// +CSCA: "+8613800592500",145
// 
// OK

	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return ATANS_OTHER;
	}

	const char STR_RESSYMB[] = "+CSCA:";
	if( strstr(v_szAtRes, "ERROR") )
	{
		return ATANS_ERROR;
	}
	else if( strstr(v_szAtRes, "OK") )
	{
		char* pAns = strstr(v_szAtRes, STR_RESSYMB);
		if( pAns )
		{
			pAns += strlen(STR_RESSYMB); // 跳过应答标志

			// 寻找起始位置
			char* pBegin = NULL;
			if ('"' == *pAns)
			{
				pBegin = pAns;
			}
			else
			{
				pBegin = strstr(pAns, "\"");
			}
			if( !pBegin ) return ATANS_OTHER;
			if( pBegin - pAns > 3 ) return ATANS_OTHER;

			// 结束位置
			char* pEnd = strstr(pBegin + 1, "\"");
			if( !pEnd ) return ATANS_OTHER;
			if( pEnd - pBegin > 16 ) return ATANS_OTHER;

			DWORD dwCscaLen = min(DWORD(pEnd - pBegin - 1), v_sizResult - 1);
			memcpy( v_pResult, pBegin + 1, dwCscaLen );
			((char*)v_pResult)[ dwCscaLen ] = 0;
			
			return ATANS_OK;
		}
		else
		{
			return ATANS_OTHER;
		}
	}
	else
	{
		return ATANS_RCVNOTALL;
	}
}

int G_CLIP_ReadTel(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// RING    
// 
// +CLIP: "05922956773",161,"",,"",0  

	int iRet = ATANS_NOANS;
	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return ATANS_OTHER;
	}
	
	const char STR_RESSYMB[] = "+CLIP:";
	char* pClipBegin = NULL;

	if( strstr(v_szAtRes, "RING") )
	{
		if( pClipBegin = strstr(v_szAtRes, STR_RESSYMB) )
		{
			pClipBegin += strlen(STR_RESSYMB); // 跳过应答标志

			// 寻找起始位置
			char* pBegin = strchr(pClipBegin, '\"' );
			if( !pBegin ) return ATANS_OTHER;
			if( pBegin - pClipBegin > 3 ) return ATANS_OTHER;

			// 寻找终止位置
			char* pEnd = strchr( pBegin + 1, '\"' );
			if( !pEnd ) return ATANS_OTHER;
			if( pEnd - pBegin > 20 ) return ATANS_OTHER;

			DWORD dwTelLen = min( DWORD(pEnd - pBegin - 1), v_sizResult - 1 );
			memcpy( v_pResult, pBegin + 1, dwTelLen );
			((char*)v_pResult)[ dwTelLen ] = 0;

			iRet = ATANS_OK;
		}
		else
		{
			iRet = ATANS_RCVNOTALL;
		}

		// 判断是否有DSCI指示 (待)
	}
	else
	{
		iRet = ATANS_NOANS;
	}

	return iRet;
}

int G_CLCC_ReadTel(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// at+clcc       
// +CLCC: 1,1,4,0,0,"13055216111",161
// 
// OK

	if( !v_szAtRes || !v_pResult || !v_sizResult )
	{
		return ATANS_OTHER;
	}
	
	const char STR_RESSYMB[] = "+CLCC:";
	char* pClccBegin = NULL;

	if( strstr(v_szAtRes, "ERROR") )
	{
		return ATANS_ERROR;
	}
	else if( strstr(v_szAtRes, "OK") )
	{
		if( pClccBegin = strstr(v_szAtRes, STR_RESSYMB) )
		{
			pClccBegin += strlen(STR_RESSYMB); // 跳过应答标志

			// 寻找起始位置
			char* pBegin = strchr(pClccBegin, '\"');
			if( !pBegin ) return ATANS_OTHER;
			char* pResEnd = strstr(pClccBegin, "\r");
			if( pResEnd < pBegin ) return ATANS_OTHER;

			// 寻找终止位置
			char* pEnd = strchr( pBegin + 1, '\"' );
			if( !pEnd ) return ATANS_OTHER;
			if( pEnd - pBegin > 20 || pResEnd < pEnd ) return ATANS_OTHER;

			DWORD dwTelLen = min( DWORD(pEnd - pBegin - 1), v_sizResult - 1 );
			memcpy( v_pResult, pBegin + 1, dwTelLen );
			((char*)v_pResult)[ dwTelLen ] = 0;

			return ATANS_OK;
		}
		else
		{
			return ATANS_NOANS;
		}
	}
	else
	{
		return ATANS_RCVNOTALL;
	}
}

int G_JugRemotePick(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// +COLP: "",0,"",0                
// ^DSCI: 1,0,0,0,0,"13611112222",129,,,128,"",0    

// NO CARRIER

// ^DSCI: 1,0,6,0,0,"13611112222",129,,19,128,"",255
// NO ANSWER

// ^DSCI: 1,0,6,0,0,"13611112222",129,,17,128,"",255
// BUSY

	if( strstr(v_szAtRes, "ERROR") )
		return ATANS_ERROR;
	else if( strstr(v_szAtRes, "NO CARRIER") )
		return ATANS_NOCARRIER;
	else if( strstr(v_szAtRes, "NO DIALTONE") )
		return ATANS_NODIALTONE;
	else if( strstr(v_szAtRes, "BUSY") )
		return ATANS_BUSY;
	else if( strstr(v_szAtRes, "NO ANSWER") )
		return ATANS_NOANSWER;
	else if( strstr(v_szAtRes, "COLP") )
		return ATANS_OK;
	else
		return ATANS_NOANS;
}

int G_JugRemoteHangup(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	if( strstr(v_szAtRes, "ERROR") )
		return ATANS_ERROR;
	else if( strstr(v_szAtRes, "NO CARRIER") )
		return ATANS_NOCARRIER;
// 	else if( strstr(v_szAtRes, "NO DIALTONE") )
// 		return ATANS_NODIALTONE;
// 	else if( strstr(v_szAtRes, "BUSY") )
// 		return ATANS_BUSY;
// 	else if( strstr(v_szAtRes, "NO ANSWER") )
// 		return ATANS_NOANSWER;
// 	else if( strstr(v_szAtRes, "COLP") )
// 		return ATANS_OK;
// 	else
// 		return ATANS_NOANS;
	else
		return ATANS_OTHER;
}

int G_CLCC_JugRes(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// +CLCC: 1,0,2,0,0,"1305526111",129                                 
// OK

	const char STR_RESSYMB[] = "+CLCC:";
	if( strstr(v_szAtRes, "OK") && strstr(v_szAtRes, STR_RESSYMB) )
	{
		return ATANS_OK;
	}
	else
	{
		return ATANS_NOCALL;
	}
}

int G_CMGS_JugRes(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
	if( strstr(v_szAtRes, "ERROR") )
		return ATANS_ERROR;
	else if( strstr(v_szAtRes, ">") )
		return ATANS_OK;
	else
		return ATANS_NOANSWER;
}

int G_CPAS_JugRes(char* v_szAtRes, void* v_pResult, size_t v_sizResult )
{
// AT+CPAS
// +CPAS: 3
//
// OK
	
	if( !v_szAtRes )
	{
		return ATANS_OTHER;
	}

	const char STR_RESSYMB[] = "+CPAS:";
	if( strstr(v_szAtRes, "ERROR") )
	{
		return ATANS_ERROR;
	}
	else if( strstr(v_szAtRes, "OK") )
	{
		char* pAns = strstr(v_szAtRes, STR_RESSYMB);
		if( pAns )
		{
			// 寻找起始判断的位置
			pAns += strlen( STR_RESSYMB ); // 跳过应答标志
			while( 0x20 == *pAns ) // 跳过之后的空格
			{
				pAns ++;
			}

			// 读取数值
			int i = atoi(pAns);
			if( 3 == i ) // 在振铃中
			{
				return ATANS_OK;
			}
			else
			{
				return ATANS_OTHER;
			}
		}
		else
		{
			return ATANS_OTHER;
		}
	}
	else
	{
		return ATANS_RCVNOTALL;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPhoneGsm::CPhoneGsm()
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
	m_iPhoneHdlSta = PST_INIT;
	m_bPhoneNetReady = false;
	m_bToClosePhone = false;

	
	m_bM33a = false;
	m_GsmType = PHONE_BENQ;

//	InitPhone();
}

CPhoneGsm::~CPhoneGsm()
{

}

//函数功能：	复位手机模块
//线程安全：	是
//调用方式：
void CPhoneGsm::ResetPhone()
{
	tagPhoneTask objPhoneTask;
	_PhoneTaskSet( objPhoneTask, PHTASK_RESET );
}

//函数功能：	手机模块硬件操作
//线程安全：	是
//调用方式：	
// Gsm硬件复位
void CPhoneGsm::_GsmHardReset()
{
	_GsmPowOff();
	sleep(2);
	_GsmPowOn();
}

//函数功能：	手机模块硬件操作
//线程安全：	是
//调用方式：
//benq上电,根据benq建议控制时序编写
//IOS_PHONERST：对应PWON
//IOS_PHONEPOW：对应VBAT
void CPhoneGsm::_GsmPowOn()
{
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET,初始应该为高
	sleep(1);
	IOSet(IOS_PHONEPOW, IO_PHONEPOW_ON, NULL, 0);		//POWER
	sleep(1);
	IOSet(IOS_PHONEPOW, IO_PHONEPOW_OFF, NULL, 0);		//POWER,VBAT 拉至0，建议2-3s
	sleep(3);
	IOSet(IOS_PHONEPOW, IO_PHONEPOW_ON, NULL, 0);		//POWER
	sleep(16);											//VBAT 恢复供电后，等待最少13 秒以上
	IOSet(IOS_PHONERST, IO_PHONERST_LO, NULL, 0);		//RESET，在vbat高之后
	usleep(200000);		//about 200ms
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET
	sleep(1);
	PRTMSG(MSG_DBG,"Gsm power on!\n");
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	RenewMemInfo(0x02, 0x02, 0x01, 0x00);
#endif
}

//函数功能：	手机模块硬件操作
//线程安全：	是
//调用方式：	
void CPhoneGsm::_GsmPowOff()
{
	sleep(1);
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET	
	sleep(1);
	IOSet(IOS_PHONEPOW, IO_PHONEPOW_OFF, NULL, 0);		//power	
	sleep(2);

	PRTMSG(MSG_DBG,"Gsm power off!\n");
}

//函数功能：	手机模块重新拨号
//线程安全：	是
//调用方式：	
void CPhoneGsm::ReDialNet()
{
	tagPhoneTask objPhoneTask;
	_PhoneTaskSet( objPhoneTask, PHTASK_CUTNET );
}

//函数功能：	获取手机模块网络状态
//线程安全：	是
//调用方式：	
// 返回值: 0,表示不在线; 1,表示已注册网络; 2,表示已拨号上网
BYTE CPhoneGsm::GetPhoneState()
{
	switch( m_iPhoneHdlSta )
	{
	case PST_INIT:
	case PST_RESET:
		return 0;
		break;

	default:
		if( m_bPhoneNetReady ) return 2;
		else return 1;
	}
	return 0;
}

//函数功能：	获取手机模块状态机
//线程安全：	是
//调用方式：	
int CPhoneGsm::GetPhoneHdlSta()
{
	return m_iPhoneHdlSta;
}

//函数功能：	获取信号
//线程安全：	是
//调用方式：	
int CPhoneGsm::GetPhoneSignal()
{
	tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	return objPhoneSta.m_iSignal;
}

//函数功能：	获取IMEI号
//线程安全：	是
//调用方式：	
void CPhoneGsm::GetPhoneIMEI( char* v_szImei, size_t v_sizImei )
{
	if( v_sizImei < 1 ) return;
	tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	memset( v_szImei, 0, sizeof(v_sizImei) );
	memcpy( v_szImei, objPhoneSta.m_szGsmIMEI, min(v_sizImei, strlen(objPhoneSta.m_szGsmIMEI)) );
	v_szImei[ v_sizImei - 1 ] = 0;
}

//函数功能：	获取手机模块音量
//线程安全：	是
//调用方式：	
int CPhoneGsm::GetPhoneVolume()
{
	tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	return objPhoneSta.m_iAudioOutVol;
}

//函数功能：	手机模块处理线程
//线程安全：	否
//调用方式：	处理线程调用
unsigned long CPhoneGsm::P_PhoneHdl()
{
	usleep(200000);
	PRTMSG(MSG_DBG,"**********\n");
	usleep(200000);
	m_iPhoneHdlSta = PST_INIT;
	
	while(!g_bProgExit) 
	{
		// 该函数内部有休眠
		_PhoneHdl();
	}
	
	g_bProgExit = true;
	pthread_exit(NULL);
	
	return 0;
}

//函数功能：	手机模块读串口线程
//线程安全：	否
//调用方式：	读线程调用
//手机模块读线程
unsigned long CPhoneGsm::P_ComRcv()
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
			PRTMSG(MSG_DBG, "PCom Rcv:%s\n", szRcv );
			
			//注意push的时候，不能以字符串结束符做判断，unicode数据中有0x00
			//que lvl 5->1
			//m_objDataMngRcv.PushData( 1, strlen(szRcv), szRcv, dwPktNum, 0, (char*)&dwCurTm, sizeof(dwCurTm) );
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

//函数功能：	手机模块处理线程
//线程安全：	否
//调用方式：
unsigned long CPhoneGsm::P_OtherHdl()
{
	while(!g_bProgExit)
	{
		_OtherHdl();
		usleep(20000);//que 怎么没有sleep？
	}
	g_bProgExit = true;
	
	return 0;
}

/*//ping操作暂未实现
unsigned long CPhoneGsm::P_Ping()
{
	int iResult = 0; // 0,成功; 1,失败; 2,其他异常; 3,被中止

	HANDLE hIcmp = NULL;
	ulong ret = 0;
	ulong ip_serv = 0;
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
		ulong dwRetVal = 0;
		ulong ulOutBufLen = sizeof(IP_ADAPTER_INFO);
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
		ulong dwRetVal = 0;
		ulong ulOutBufLen = sizeof(FIXED_INFO);
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
	ip_serv = (phostent==NULL) ? (0) : (*(ulong*)(*phostent->h_addr_list));
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
	ip_serv = (phostent==NULL) ? (0) : (*(ulong*)(*phostent->h_addr_list));
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
		tagPhoneTask objPhoneTask;
		_PhoneTaskSet( objPhoneTask, PHTASK_CUTNET );
	}

	tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	tagPhoneTask objPhoneTask;
	objPhoneTask.m_bytPingResult = iResult;
	objPhoneTask.m_bytPingSrc = objPhoneSta.m_bytPingSrc;
	_PhoneTaskSet( objPhoneTask, PHTASK_PINGRPT );

	return 0;
}
*/
int CPhoneGsm::InitPhone()
{
	int iResult;
	
	//初始化互斥量
	if((0 != pthread_mutex_init( &m_crisecRcv, NULL)) || (0 != pthread_mutex_init( &m_crisecPhoneTask, NULL)) ||
		(0 != pthread_mutex_init( &m_crisecSta, NULL)) || (0 != pthread_mutex_init( &m_crisecDog, NULL)))
	{
		PRTMSG(MSG_ERR,"初始化互斥量失败！\n");
		goto _INITPHONE_FAIL;
	}
	
// 	IOSet(IOS_HUBPOW, IO_HUBPOW_ON, NULL, 0);			//HUB,正式版本不需控制
// 	
// 	_GsmPowOn();//在ComVir中进行

	iResult = _InitCom();
	if( true != iResult ) 
	{
		goto _INITPHONE_FAIL;
	}
	
	// 创建手机模块串口读取线程
	iResult = pthread_create(&m_hThdComRcv, NULL, (void *(*)(void*))G_ComRcv, (void *)this);
	if(0 !=iResult)
	{
		PRTMSG(MSG_ERR,"创建串口读取线程失败！\n");  
		goto _INITPHONE_FAIL;
	}
	
	// 创建 电话处理 线程
	iResult = pthread_create(&m_hThdPhoneHdl, NULL, (void *(*)(void*))G_PhoneHdl, (void *)this);
	if(0 !=iResult)
	{
		PRTMSG(MSG_ERR,"创建电话处理线程失败！\n");  
		goto _INITPHONE_FAIL;
	}
	
	// 创建 命令处理 线程
	iResult = pthread_create(&m_hThdOtherHdl, NULL, (void *(*)(void*))G_OtherHdl, (void *)this);
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

//函数功能：	手机模块释放资源
//线程安全：	否
//调用方式：	程序退出调用
void CPhoneGsm::ReleasePhone()
{
	//进程退出标志
#if CHK_VER == 0	
	g_bProgExit = true;
#endif
	// { 同步控制手机模块关闭,使得状态起迅速退出非复位状态进入复位状态,并且在复位状态中仍然能良好关闭
	m_bToClosePhone = true;
	
	_PhoneCutNet();	
	_CloseCom();

	g_objComVir.ReleaseComVir();
	
	//销毁互斥量
	pthread_mutex_destroy( &m_crisecRcv );
	pthread_mutex_destroy( &m_crisecPhoneTask );
	pthread_mutex_destroy( &m_crisecSta );
	pthread_mutex_destroy( &m_crisecDog );
	
	_GsmPowOff();//模块安全关机
}

//函数功能：	手机模块串口初始化
//线程安全：	否
//调用方式：	状态机调用，注意相关资源的释放
int CPhoneGsm::_InitCom()
{
	int iResult;
	struct termios options;
	const int COMOPEN_TRYMAXTIMES = 15;
	char szUsbSerialPathName[256] = {0};
	struct dirent **ppUsbSerialNameList;
	int i;
	DWORD pathSymb = COMMUX_DEVPATH_AT;
	if(false == g_objComVir.GetComMuxPath(pathSymb, szUsbSerialPathName, sizeof(szUsbSerialPathName)))
	{
#if CHK_VER == 0
		g_bProgExit = true;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		g_objDiaodu.show_diaodu("手机模块异常");
		RenewMemInfo(0x02, 0x00, 0x00, 0x01);
#endif


		return false;
	}
	for( i = 0; i < COMOPEN_TRYMAXTIMES; i ++ )
	{		
		m_iPhonePort = open(szUsbSerialPathName, O_RDWR);
		if( -1 != m_iPhonePort )
		{
			PRTMSG(MSG_DBG,"PhoneGsm Open %s succ!\n",szUsbSerialPathName);
			break;
		}
		
		PRTMSG(MSG_ERR,"Open %s failed!\n",szUsbSerialPathName);
		sleep(1);
	}
	
	if( i >= COMOPEN_TRYMAXTIMES )
	{
#if CHK_VER == 0
		PRTMSG(MSG_ERR,"usb serial open fail to max times,reset system!\n");
		g_bProgExit = true;
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

//函数功能：	手机模块串口关闭
//线程安全：	否
//调用方式：	释放时调用
int CPhoneGsm::_CloseCom()
{
	if( -1 != m_iPhonePort )
	{
		close( m_iPhonePort );
		m_iPhonePort = -1;
		PRTMSG(MSG_DBG, "close Phone Port\n");
	}
	
	return 0;
}

int CPhoneGsm::_ComSnd(void* v_pSnd, DWORD v_dwSndLen )
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

 	PRTMSG(MSG_DBG, "PCom Snd: %s\n", v_pSnd);
// 	PrintString( (char *)v_pSnd, iWrited );
	return 0;
}

//函数功能：	手机模块设置at
//线程安全：	否
//调用方式：	状态机线程调用
// SetAT中的返回值处理不要轻易改变
int CPhoneGsm::_SetAT( DWORD v_dwWaitPerTime, BYTE v_bytTryTimes, void* v_pResult, size_t v_sizResult,
	PCHKATRES v_pChkRes, DWORD v_dwSndLen, char* v_szATCmd, ... )
{
	//int euAns = ATANS_OTHER;
	int iResult = 0;
	int iRet = ATANS_NOANS; // 初始化返回值为无应答
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
				iResult = ATANS_OK;
			}
			else if( strstr(pAtResBegin, "CME ERROR") )
			{
				iResult = ATANS_CMEERR;
			}
			else if( strstr(pAtResBegin, "ERROR") )
			{
				iResult = ATANS_ERROR;
			}
			else
			{
				iResult = ATANS_RCVNOTALL;
			}

			// 根据处理结果决定下步动作
			switch( iResult )
			{
			case ATANS_OK:
				iRet = iResult;
				goto _SETAT_END;
				break;

			case ATANS_RCVNOTALL:
			case ATANS_NOANS:
				// 请处理AT应答的函数注意,如果返回的是以上的应答则将继续等待
				break;

			case ATANS_CMEERR:
			case ATANS_ERROR:
			case ATANS_OTHER:
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

	if( ATANS_OK != iRet )
	{
		szAtCmdData[ strlen(szAtCmdData) - 2 ] = 0; // 去除回车换行再打印
		PRTMSG(MSG_ERR, "SETAT_END: Set %s Fail", szAtCmdData );
	}

	return iRet;
}

//函数功能：	手机模块读at
//线程安全：	否
//调用方式：	状态机线程调用
// ReadAT中的返回值处理不要轻易改变
int CPhoneGsm::_ReadAT( DWORD v_dwWait, void* v_pResult, size_t v_sizResult, PCHKATRES v_pChkRes )
{
	if( !v_pChkRes )
	{
		return ATANS_NOANS;
	}

	int iResult = ATANS_NOANS;
	int iRet = ATANS_NOANS; // 初始化返回值为无应答
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
				iResult = ATANS_NOANS;
			}

			// 处理额外收到的其他数据
			_PhoneDataDeal( szAtResTotalData );

			// 根据处理结果决定下步动作
			switch( iResult )
			{
			case ATANS_OK:
				iRet = iResult;
				goto _READAT_END;
				break;

			case ATANS_RCVNOTALL:
			case ATANS_NOANS:
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

//函数功能：	手机模块串口数据处理
//线程安全：	否
//调用方式：	状态机线程调用
// 仅在手机模块状态机线程里使用
int CPhoneGsm::_PhoneDataDeal(char* v_szAtData)
{
	tagPhoneTask objPhotoTask;
	DWORD dwToSetTaskSymb = 0;
	char* pStart = NULL;

	// 判断是否收到SIM卡不存在通知
	// que benq待添加
// 	if( strstr(v_szAtData, "^DUSIMU: 0") )
// 	{
// 		dwToSetTaskSymb |= PHTASK_RESET;
// 
// 		PRTMSG(MSG_ERR, "No SIM Card!");
// 	}

	// 在空闲状态下,判断是否有来电
	if( PST_IDLE == m_iPhoneHdlSta && strstr(v_szAtData, "RING") )
	{
		dwToSetTaskSymb |= PHTASK_RING;
	}

	// 在通话状态下,判断是否对方挂机
	if( (PST_HOLD == m_iPhoneHdlSta || PST_LSNING == m_iPhoneHdlSta) && strstr(v_szAtData, "NO CARRIER") )
	{
		PRTMSG(MSG_DBG, "Find Remote Hangup!" );
		dwToSetTaskSymb |= PHTASK_HANGUP;
	}

	// 根据上面的检查结果添加任务指令
	if( dwToSetTaskSymb )
	{
		_PhoneTaskSet(objPhotoTask, dwToSetTaskSymb);
	}


	// 短信接收处理
	{
		char* pBuf = v_szAtData;
		char* p = NULL;
		char* pTemp = NULL;
		while( p = strstr(pBuf, "+CMT:") )
		{
			PRTMSG(MSG_DBG, "!!!!!CMT Msg" );

			// +CMT:[<alpha>],<length><CR><LF><pdu>

			//+CMT: "",60
			//0891683108501935F5240D91683150256111F100089040129001352328621160F389819009987994FE67610054004A004D00450074006D00610038003400367A7A88AD6211

			p = strstr( p, "\r\n" );
			if( !p )
			{
				//show_msg( "81" );
				break;
			}
			p+=2;

			//Benq模块似乎才需要下面这段
			pTemp = strstr( p, "\r\n" );
			if( pTemp )
			{
				//show_msg( "82" );
				p=pTemp+2;
			}

			SM_PARAM sm;
			int iDeLen = decode_pdu(p, &sm);
			sms_deal(&sm);

			pBuf = p;
		}
	}

	return 0;
}

//函数功能：	手机模块数据处理
//线程安全：	否
//调用方式：	
int CPhoneGsm::_OtherHdl()
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
		PRTMSG(MSG_DBG,"Thread(PhoneHdl) long time block!");

		pthread_mutex_lock( &m_crisecDog );
		m_dwClrDogTm = GetTickCount();
		pthread_mutex_unlock( &m_crisecDog );
		
#if CHK_VER == 0
		g_bProgExit = true;
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
			succ = succ && (PST_HOLD == iPhoneHdlSta);
			if(!succ)
			{
				Frm11 frm;
				frm.reply = 0x02;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				tagPhoneTask objPhoneTask;
				objPhoneTask.m_szDTMF[0] = ch;
				_PhoneTaskSet( objPhoneTask, PHTASK_SNDDTMF );
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
							|| ( strcmp(num, "122") == 0 ) || ( strcmp(num, "10086") == 0 );

			Frm13 frm;
			frm.reply = 0x01;
			if(PST_IDLE != iPhoneHdlSta)
			{
				frm.reply = (PST_INIT == iPhoneHdlSta) ? 0x04 : 0x02; // 没有搜索到网络:手机正忙
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
				tagPhoneTask objPhoneTask;
				memcpy( objPhoneTask.m_szDialTel, pfrm->num, min(sizeof(objPhoneTask) - 1, pfrm->len) );
				_PhoneTaskSet( objPhoneTask, PHTASK_DIAL ); // 此处都认为是用户拨打电话,而非监听电话
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
			bool succ = PST_RCVRING == iPhoneHdlSta;
			//succ = succ && (!m_simlack);

			if(!succ)
			{
				Frm15 frm;
				frm.reply = 0x02;	//失败
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				tagPhoneTask objPhoneTask;
				_PhoneTaskSet( objPhoneTask, PHTASK_PICKUP );
			}
		}
		break;


		//--------------------------------------------------
	case 0x16:	//挂机请求
		{
			bool succ = (iPhoneHdlSta==PST_RCVRING) || (iPhoneHdlSta==PST_DIAL)
				|| (iPhoneHdlSta==PST_WAITPICK)|| (iPhoneHdlSta==PST_HOLD);
				
			if(!succ)
			{
				Frm17 frm;
				frm.reply = 0x02;	//失败
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				PRTMSG(MSG_DBG,"User Hangup!\n");

				tagPhoneTask objPhoneTask;
				_PhoneTaskSet( objPhoneTask, PHTASK_HANGUP );
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
			tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
			int iSignal = objPhoneSta.m_iSignal;

			Frm25 frm;
			if(iSignal>0 && iSignal<=7)			frm.level = 1;
			else if(iSignal>7 && iSignal<=15)		frm.level = 2;
			else if(iSignal>15 && iSignal<=22)	frm.level = 3;
			else if(iSignal>22 && iSignal<=29)	frm.level = 4;
			else if(iSignal>29 && iSignal<99)		frm.level = 5;
			else 									frm.level = 0;
			g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
		}
		break;


		//--------------------------------------------------
	case 0x26:	//免提耳机切换请求
		{
			bool succ = (iPhoneHdlSta==PST_HOLD);

			if(!succ)
			{
				Frm27 frm;
				frm.reply = 0x02;	//失败
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				tagPhoneTask objPhoneTask;
				_PhoneTaskSet(objPhoneTask, PHTASK_CHGAUDIO);
			}
		}
		break;


		//--------------------------------------------------
	case 0x28:	//音量调节请求  调节方向(1)  00H调高 01H调低
		{
			bool succ = (iPhoneHdlSta==PST_HOLD || iPhoneHdlSta==PST_IDLE);

			if(!succ)
			{
				Frm29 frm;
				frm.reply = 0x02;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				tagPhoneTask objPhoneTask;
				Frm28 *pfrm = (Frm28*)cmd;
				objPhoneTask.m_iChgVol = (pfrm->dir==0x00) ? (1) : (-1);	// 调高 : 调低
				_PhoneTaskSet(objPhoneTask, PHTASK_CHGVOL);
			}
		}
		break;

#if USE_IRD == 0
	case 0x2a:	//通话模式设置请求
		{
			bool succ = (iPhoneHdlSta==PST_HOLD);

			if(succ)
			{
				tagPhoneTask objPhoneTask;
				_PhoneTaskSet(objPhoneTask, PHTASK_CHGAUDIO);
			}

// 			tag2QHstCfg obj2QHstCfg;
// 			GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg), 
// 				offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );
// 			m_telmode = obj2QHstCfg.m_HandsfreeChannel;
// 			m_telmodebk = obj2QHstCfg.m_HandsfreeChannel;
// 			if (!m_telmode) {	//耳机
// 				// 设置免提模式
// 				if(!set_audio_mode(1))		{ PRTMSG(MSG_ERR,"set_audio_mode(1) Fail!"); }
// 				else	{	PRTMSG(MSG_DBG,"set_audio_mode(1) Succ!");	}
// 			} else {	//免提
// 				// 设置免提模式
// 				if(!set_audio_mode(0))		{ PRTMSG(MSG_ERR,"set_audio_mode(0) Fail!"); }
// 				else	{	PRTMSG(MSG_DBG,"set_audio_mode(0) Succ!");	}
// 			}
		}
		break;
#endif

		//--------------------------------------------------
	case 0x30:	//发送短消息请求  被叫手机号长度(1)+ 被叫手机号(n)+短消息内容长度(1)+短消息内容(m)
		{
			PRTMSG(MSG_DBG,"User req send sms");

			if( true ) //PST_IDLE == iPhoneHdlSta )
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
				PRTMSG(MSG_DBG,"Not In Idle Sta,Refuse Sending SMS.");

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
			if( PST_IDLE==iPhoneHdlSta)
			{
				PRTMSG(MSG_DBG,"Rcv %s's net wick inform!\n", (0x01==cmd[1]) ? "Liu" : "Qian" );

				tagPhoneTask objPhoneTask;
				if(0x01==cmd[1])	objPhoneTask.m_bytPingSrc |= 0x02;
				else				objPhoneTask.m_bytPingSrc |= 0x01;
//				if(!m_simlack)		m_gprsdail = true;

				_PhoneTaskSet( objPhoneTask, PHTASK_PING );
			}
		}
		break;

	case 0x36: //重置短信中心号
		{
			tagPhoneTask objPhoneTask;
			_PhoneTaskSet(objPhoneTask, PHTASK_SETSMSCENT);
		}
		break;


		//--------------------------------------------------
	case 0x38:	//往中心发送短信请求
		{
			PRTMSG(MSG_DBG,"Other Program req send sms!");
			
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
			succ = (PST_LSNING == iPhoneHdlSta);
			if(succ)
			{
				PRTMSG(MSG_DBG,"Now is listening...\n");

				FrmF2 frm;
				frm.reply = 0x02;	//正在监听中
				DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
				break;
			}

			//系统忙(可能是长时操作) 或正在拨打监听电话中
			succ =	(iPhoneHdlSta==PST_INIT) || (iPhoneHdlSta==PST_RESET) || (iPhoneHdlSta==PST_DIALNET)
					|| (iPhoneHdlSta==PST_HANGUP) || (iPhoneHdlSta==PST_DIALLSN) || (iPhoneHdlSta==PST_WAITLSNPICK);
			if(succ)
			{
				PRTMSG(MSG_DBG,"The line is busy!\n");

				FrmF2 frm;
				frm.reply = 0x06;	//车台忙
				DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
				break;
			}

			//其他状态(包括空闲,拨打,通话,来电等)
			tagPhoneTask objPhoneTask;
			memcpy( objPhoneTask.m_szDialLsnTel, pfrm->num, min(sizeof(objPhoneTask.m_szDialLsnTel) - 1, pfrm->len) );
			if( PST_IDLE == iPhoneHdlSta ) // 空闲状态下
			{
				_PhoneTaskSet( objPhoneTask, PHTASK_DIALSN );
			}
			else // 拨打/通话/来电状态
			{
				_PhoneTaskSet( objPhoneTask, PHTASK_HANGUP | PHTASK_DIALSN ); // 设置2个指令,实际执行时将是先挂机再拨打监听号码
			}
		}
		break;

		//--------------------------------------------------
	case 0xF3:	//进入报警状态(不允许拨打非中心电话并自动挂断非中心来电)
		{
			//que phoneTD未进行此操作？
// 			tagPhoneTask objPhotoTask;
// 			objPhotoTask.m_dwAlertPrid = DWORD(cmd[1])*1000; // 0表示退出报警
// 			_PhoneTaskSet(objPhotoTask, PHTASK_ALERM);
			tagPhoneStaCommon objPhoneStaComm;
			objPhoneStaComm.m_dwAlertPrid = DWORD(cmd[1])*1000;
			if (0 != objPhoneStaComm.m_dwAlertPrid)
			{
				objPhoneStaComm.m_dwAlertBegin = GetTickCount();
				PRTMSG(MSG_DBG,"Enter alarm state(%d s)!",DWORD(cmd[1]));
			}
			else
			{
				objPhoneStaComm.m_dwAlertBegin = 0;
				PRTMSG(MSG_DBG,"Exit alarm state!");
			}
			_PhoneStaCommonSet(objPhoneStaComm, PHTASK_ALERM);
		}
		break;
		
	default:
		break;
	}

	return 0;
}

//函数功能：	手机模块数据处理
//线程安全：	否
//调用方式：	
int CPhoneGsm::_PhoneHdl()
{
	pthread_mutex_lock( &m_crisecDog );
//	PRTMSG(MSG_DBG,"hdl DogTm:%d",m_dwClrDogTm);
	m_dwClrDogTm = GetTickCount();
	//	PRTMSG(MSG_DBG,"hdl tick:%d",m_dwClrDogTm);
	pthread_mutex_unlock( &m_crisecDog );
	DWORD dwSleep = 50000;

	switch( m_iPhoneHdlSta )
	{
	case PST_INIT:
		{
#if VEHICLE_TYPE == VEHICLE_M
			g_objLightCtrl.SetLightSta(0x02);	// 快闪
#endif

			RenewMemInfo(0x02, 0x02, 0x01, 0x05);

			int iResult = _PhoneStaHdl_Init();
			if( !iResult) //成功
			{
				g_simcard = 1;
				m_iPhoneHdlSta = PST_IDLE;

#if CHK_VER == 1
	#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
				g_objLightCtrl2.SetLightSta(0x01);	// 生产检测版本初始化成功即闪绿灯
	#endif
#endif
			}
			else
			{
				g_simcard = 3;
				m_iPhoneHdlSta = PST_RESET;
			}
		}
		break;

		//-----------------------------------------------------------------
	case PST_IDLE:	//电话空闲中...
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_Idle( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case PST_DIALLSN:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_DialLsn( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case PST_WAITLSNPICK:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_WaitLsnPick( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case PST_LSNING:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_Lsning( iNextSta );
			m_iPhoneHdlSta = iNextSta;

			if( PST_LSNING == iNextSta )
			{
				// 检查是否要发送短信
				_PhoneJugSmsSnd();
			}
		}
		break;

	case PST_DIAL:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_Dial( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case PST_WAITPICK:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_WaitPick( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case PST_HOLD:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_Hold( iNextSta );
			m_iPhoneHdlSta = iNextSta;

			if( PST_HOLD == iNextSta ) // 若仍然是通话状态中
			{
				// 检查是否要发送短信
				_PhoneJugSmsSnd();
			}
		}
		break;

	case PST_RCVRING:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_RcvRing( iNextSta );
			if( iNextSta != m_iPhoneHdlSta )
			{
				// 由于Ring任务可能不断产生,所以要多处地方清除
				_PhoneTaskClr( PHTASK_RING );
			}
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case PST_HANGUP:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_Hangup( iNextSta );
			m_iPhoneHdlSta = iNextSta;

			// 由于Ring任务可能不断产生,所以要多处地方清除
			_PhoneTaskClr( PHTASK_RING );
		}
		break;

	case PST_RESET:
		_PhoneStaHdl_Reset();
		m_iPhoneHdlSta = PST_INIT;

		// 由于Ring任务可能不断产生,所以要多处地方清除
		_PhoneTaskClr( PHTASK_RING );
		break;

	case PST_DIALNET:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_DialNet( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case PST_WAITDIALNET:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_WaitDialNet( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	case PST_CUTNET:
		{
			int iNextSta = m_iPhoneHdlSta;
			_PhoneStaHdl_CutNet( iNextSta );
			m_iPhoneHdlSta = iNextSta;
		}
		break;

	default:
		PRTMSG(MSG_ERR, "Phone Sta Unknown! Reset Phone!" );
		m_iPhoneHdlSta = PST_RESET;
	}

	// 判断是否要进入/退出报警状态
	_PhoneJugAlerm();

	// 判断是否要处理"ping结果报告"
	_PhoneJugSmsPingRpt();

	// 判断是否要进入复位状态
	if( _PhoneJugReset() ) m_iPhoneHdlSta = PST_RESET;

	if( dwSleep ) usleep(dwSleep);

	return 0;
}

int CPhoneGsm::_PhoneStaHdl_Init()
{
	PRTMSG(MSG_DBG,"stInit\n");

	char szResult[2048] = { 0 };
	int iResult = 0;
	char cIndicate = 0;
	char szGsmType[32]={0};
	tagPhoneStaCommon objSta;

	// 清除不应有的,或没有处理价值,或已过时的任务指令
	_PhoneTaskClr( PHTASK_CHGAUDIO | PHTASK_CHGVOL | PHTASK_CUTNET | PHTASK_DIAL | PHTASK_DIALSN
		| PHTASK_HANGUP | PHTASK_RING | PHTASK_SNDDTMF | PHTASK_RESET );

	// 状态机中单一状态独有的环境变量值统一清除 (注意不要用memset,要调用Init)
	m_objPhoneStaDial.Init();
	m_objPhoneStaDialLsn.Init();
	m_objPhoneStaHold.Init();
	m_objPhoneStaLsning.Init();
	m_objPhoneStaRing.Init();

	// 通用环境变量中可以清除的进行清除
	_PhoneStaCommonClr( PHSTVAR_SING | PHSTVAR_NET );

	// 选择通话关闭模式
	_PhoneAudioModeChg( AUDIOMODE_CLOSE );


	// 若usbserial串口句柄为-1，说明已关闭,重新打开串口
	if( -1 == m_iPhonePort )
	{
		iResult = _InitCom();
		if( false == iResult ) 
			goto _INITPHONE_FAIL;
	}

	// AT测试
	if( ATANS_OK != _SetAT(500, 5, NULL, 0, NULL, 0, "AT") )
	{
		goto _INITPHONE_FAIL;
	}
	PRTMSG(MSG_DBG, "set at succ\n");

	// 禁止回显
	if( ATANS_OK != _SetAT(500, 5, NULL, 0, NULL, 0, "ATE0") )
	{
		goto _INITPHONE_FAIL;
	}

	
	// 获取GSM模块类型
	if( ATANS_OK != _SetAT(600, 5, szGsmType, sizeof(szGsmType), G_CGMM_Jugres, 0, "AT+CGMM"))
	{
		goto _INITPHONE_FAIL;
	}

	if(strstr(szGsmType, "SIMCOM_SIM300D"))
	{
		m_GsmType = PHONE_SIMCOM;
		PRTMSG(MSG_DBG, "Gsm init simcom \n");
	}
	if(strstr(szGsmType, "M23A"))
	{
		m_GsmType = PHONE_BENQ;
		PRTMSG(MSG_DBG, "Gsm init benq m23a \n");
	}
	if(strstr(szGsmType, "M33AG"))
	{
		m_bM33a = true;
		m_GsmType = PHONE_BENQ;
		PRTMSG(MSG_DBG, "Gsm init benq m33ag \n");
	}

	// 通知千里眼程序手机模块类型 (用于照片传输流控的设置,因为每个模块的传输带宽不一样)
	{
		FrmF3 frmf3;	
		frmf3.reply = byte(m_GsmType);
		DataPush((char*)&frmf3, sizeof(frmf3), DEV_PHONE, DEV_QIAN, 2);
	}

	
	// 查询IMEI号
	{
		static bool sta_bGetIMEI = false;
		if( !sta_bGetIMEI )
		{
			if( ATANS_OK == _SetAT(1000, 5, objSta.m_szGsmIMEI, sizeof(objSta.m_szGsmIMEI), G_CGSN_Jugres, 0, "AT+CGSN") )
			{
				objSta.m_szGsmIMEI[ sizeof(objSta.m_szGsmIMEI) - 1 ] = 0;
				_PhoneStaCommonSet( objSta, PHSTVAR_IMEI );

				sta_bGetIMEI = true;
			}
		}
	}

	//获取模块版本 AT+CGMR
	_SetAT(600, 5, NULL, 0, NULL, 0, "AT+CGMR");


	// 读取注册SIM流程值(只适用于BENQ)
	tag1PComuCfg cfg;
	GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );	
	static int flow = cfg.m_iRegSimFlow;		//12不设置SATC, 34设置SATC, 13自动搜网, 24手动搜网

	
	//============= 以下为设置网络相关的代码 =======================================
	if(PHONE_SIMCOM==m_GsmType) 
	{
		// 设置TE字符集为GSM格式
		if(ATANS_OK != _SetAT(1000, 3, NULL, 0, NULL, 0, "AT+CSCS=\"GSM\""))
		{
			PRTMSG(MSG_DBG, "set AT+CSCS=GSM fail \n");
			goto _INITPHONE_FAIL;
		}
	}
	
	if(PHONE_BENQ==m_GsmType) 
	{
		// 禁止进入deep睡眠模式
		if(ATANS_OK != _SetAT(1000, 3, NULL, 0, NULL, 0, "AT$NOSLEEP=1"))
		{
			PRTMSG(MSG_DBG, "set AT$NOSLEEP=1 fail\n");
			goto _INITPHONE_FAIL;
		}
		
		// 查询并设置AUTOCAMPON的值，que 待确认
		if(ATANS_OK != _SetAT(1000, 3, NULL, 0, C_AUTOCAMPON_Jg, 0, "AT$AUTOCAMPON?"))
		{
			_SetAT(1000, 3, NULL, 0, NULL, 0, "AT$AUTOCAMPON=0");
		}

		
		// 设置SATC
		if(flow<1 || flow>4)	flow = 1;
		if(3==flow || 4==flow)
		{ 
			_SetAT(1000, 3, NULL, 0, NULL, 0, "AT%SATC=1,7fffffff7f0200fb07");
		}
	}
	
	if( PHONE_BENQ == m_GsmType )
	{
		// 打开模块的大部份功能
		if(ATANS_OK != _SetAT(5500, 3, NULL, 0, NULL, 0, "AT+CFUN=1"))
		{
			PRTMSG(MSG_DBG, "set AT+CFUN=1 fail\n");
			goto _INITPHONE_FAIL;
		}

		// 判断SIM卡是否存在
		if(ATANS_OK != _SetAT(1000, 3, NULL, 0, NULL, 0, "AT+CPIN?"))
		{
			goto _INITPHONE_FAIL;
		}
	}
	else if( PHONE_SIMCOM == m_GsmType )
	{
		// 判断SIM卡是否存在
		if(ATANS_OK != _SetAT(1000, 3, NULL, 0, NULL, 0, "AT+CPIN?"))
		{
			goto _INITPHONE_FAIL;
		}
		// 打开模块的大部份功能
		if(ATANS_OK != _SetAT(3000, 3, NULL, 0, NULL, 0, "AT+CFUN=1"))
		{
			PRTMSG(MSG_DBG, "set AT+CFUN=1 fail\n");
			goto _INITPHONE_FAIL;
		}
	}
	else 
	{
		goto _INITPHONE_FAIL;
	}

	//SIMCOM模块的网络注册
	if(PHONE_SIMCOM==m_GsmType)
	{
		PRTMSG(MSG_DBG, "Search net...\n");
		// Simcom无需设置COPS,因为模块启动时就自动查找网络了,设置了反而会有问题
		//if(!set_at(120, 1, "AT+COPS=0\r\n"))		{ show_msg("Set COPS=0 Fail!"); return 2; }	
	}
	
	//BENQ模块的网络注册
	if(PHONE_BENQ==m_GsmType) 
	{
		PRTMSG(MSG_DBG, "Search net(%d)...\n", flow);
		if(ATANS_OK != _SetAT(30000, 2, NULL, 0, NULL, 0, "AT+COPS=0"))
		{
			PRTMSG(MSG_DBG, "Set COPS=0 Fail!\n");
			
			bool bManul = false;
			
#if NETWORK_GPRSTYPE == NETWORK_YIDONG
			if(ATANS_OK != _SetAT(15000, 2, NULL, 0, NULL, 0, "AT+COPS=4,2,\"46000\""))
			{
				PRTMSG(MSG_DBG, "Set COPS=4,2,46000 Fail!\n");
				
				if(ATANS_OK != _SetAT(15000,2, NULL, 0, NULL, 0, "AT+COPS=4,2,\"46002\""))
				{
					PRTMSG(MSG_DBG, "Set COPS=4,2,46002 Fail!\n");
				}
				else
				{
					bManul = true;
				}
			}
			else
			{
				bManul = true;
			}
#endif
			
#if NETWORK_GPRSTYPE == NETWORK_LIANTONG
			if(ATANS_OK != _SetAT(15000, 2, NULL, 0, NULL, 0, "AT+COPS=4,2,\"46001\""))
			{
				PRTMSG(MSG_DBG, "Set COPS=4,2,46001 Fail!\n");
			}
			else
			{
				bManul = true;
			}
#endif			
			
			if( !bManul ) 
			{
				goto _INITPHONE_FAIL;
			}
		} 
	}

	//_SetAT(1000, 3, NULL, 0, NULL, 0, "AT+COPS?");
	sleep(2);	

	//_SetAT(1000, 3, NULL, 0, NULL, 0, "AT+CREG=1");//+CGREG: <stat>主动上报开关
	int iSignal;
	char szSig[8];
	int cnt;
	cnt = 0;
	while(1)
	{
		if( ATANS_OK == _SetAT(2000, 3, szSig, sizeof(szSig), G_CSQ_Jugres, 0, "AT+CSQ") )
		{
			iSignal = atoi( szSig );
			//PRTMSG(MSG_DBG, "iSignal:%d\n",iSignal);
			if(99!=iSignal && 0!=iSignal && -1!=iSignal)
			{
				break;
			}
		}
		else
		{
			PRTMSG(MSG_ERR, "Get Single Fail!" );
		}
		if (cnt++ > 90)
		{
			PRTMSG(MSG_DBG, "Long time no signal %d\n",iSignal);
			goto _INITPHONE_FAIL;
		}
		sleep(1);
	}

	cnt = 0;
	while(1)
	{
		if(ATANS_OK == _SetAT(2000, 10, NULL, 0, G_CREG_Jugres, 0, "AT+CREG?"))
		{
			PRTMSG(MSG_DBG, "at+creg? succ\n");
			break;
		}
		else
		{
			PRTMSG(MSG_ERR, "Get Single Fail!" );
		}
		if (cnt++ > 90)
		{
			PRTMSG(MSG_DBG, "Long time CREG? fail%d\n");
			goto _INITPHONE_FAIL;
		}
		sleep(1);
	}

// 	if(ATANS_OK != _SetAT(10000, 10, NULL, 0, G_CREG_Jugres, 0, "AT+CREG?"))
// 	{
// 		PRTMSG(MSG_DBG, "Register net fail \n");
// 		g_objDiaodu.show_diaodu(LANG_NETSEARCH_FAIL);
// 		if(PHONE_BENQ==m_GsmType)	
// 		{ 
// 			if(++flow>4)	
// 				flow = 1; 
// 		}
// 	}

	if(PHONE_BENQ==m_GsmType) 
	{
		// 如有注册SIM卡流程有改变,记录该值
		if(flow!=cfg.m_iRegSimFlow) 
		{
			cfg.m_iRegSimFlow = flow;
			SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg));
		}
	}

	//============= 以下为设置声音相关的代码 =======================================
	
	if(PHONE_BENQ==m_GsmType) 
	{
		objSta = _PhoneStaCommonGet(0);
		// Select Audio Path
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$AUPATH=3,1"))
		{
			PRTMSG(MSG_DBG, "set AT$AUPATH=3,1 fail\n");
			goto _INITPHONE_FAIL;
		}

		// Set Audio Gain (消除侧音)
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$AUGAIN=0,0"))
		{
			PRTMSG(MSG_DBG, "set AT$AUGAIN=0,0 fail\n");
			goto _INITPHONE_FAIL;
		}

		// Set Audio Gain (增大音量)
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$AUGAIN=2,9"))
		{
			PRTMSG(MSG_DBG, "set AT$AUGAIN=2,9 fail\n");
			goto _INITPHONE_FAIL;
		}

		// Set Audio Vol
		//AT$AUVOL还是AT$SG=1,9 que 待确认
// 		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$AUVOL=%d\r\n", m_volume))
// 		{
// 			PRTMSG(MSG_DBG, "set AT$AUVOL fail\n");
// 			goto _INITPHONE_FAIL;
// 		}

		// Set Audio Echo
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$AUAEC=566"))
		{
			PRTMSG(MSG_DBG, "set AT$AUAEC=566 fail\n");
			goto _INITPHONE_FAIL;
		}

		// Set Audio Smooth
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$AUSMOOTH=3276"))
		{
			PRTMSG(MSG_DBG, "set AT$AUSMOOTH=3276 fail\n");
			goto _INITPHONE_FAIL;
		}

		// Set Audio Level Max
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$AULEVELMAX=15000"))
		{
			PRTMSG(MSG_DBG, "set AT$AULEVELMAX=15000 fail\n");
			goto _INITPHONE_FAIL;
		}		

		// Set Audio VAD
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$AUVAD=13392"))
		{
			PRTMSG(MSG_DBG, "set AT$AUVAD=13392 fail\n");
			goto _INITPHONE_FAIL;
		}

		// Set Audio ABS
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$AUABS=256"))
		{
			PRTMSG(MSG_DBG, "set AT$AUABS=256 fail\n");
			goto _INITPHONE_FAIL;
		}

		if( m_bM33a )
		{		
			// que 测试,未证实
			//_SetAT(600, 5, NULL, 0, NULL, 0, "AT$SQC=\"AGCU\",\"1 0001 00a0 13f0 7e7e 7fb5 59fa 7eb8 7333 7f5c 7333 0000\"");
			// que 测试,未证实
			// set_at( 5, 3, "AT$SQC=\"AGCU\",\"1 0000 00a0 13f0 7e7e 7fb5 59fa 7eb8 7333 7f5c 7333 0000\"\r\n");
			if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$SP=0"))
			{
				PRTMSG(MSG_DBG, "set AT$SP=0 fail\n");
				goto _INITPHONE_FAIL;
			}
			if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$SG=3,0"))//消側音
			{
				PRTMSG(MSG_DBG, "set AT$SG=3,0 fail\n");
				goto _INITPHONE_FAIL;
			}
			if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$SG=1,9"))//增大音量	
			{
				PRTMSG(MSG_DBG, "set AT$SG=1,9 fail\n");
				goto _INITPHONE_FAIL;
			}
			if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT$SG=2,%d",objSta.m_iAudioOutVol))
			{
				PRTMSG(MSG_DBG, "set AT$SG=2,%d fail\n",objSta.m_iAudioOutVol);
				goto _INITPHONE_FAIL;
			}
		}
	}

	if(PHONE_SIMCOM==m_GsmType) 
	{
		// 选择通道
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT+CHFA=0"))
		{
			PRTMSG(MSG_DBG, "set AT+CHFA=0 fail\n");
			goto _INITPHONE_FAIL;
		}
	
		// 回音抑制, 这个参数是廖伟与唐磊打电话测试了两周的成果。
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT+ECHO=30000,250,25,0"))
		{
			PRTMSG(MSG_DBG, "set AT+ECHO=30000,250,25,0 fail\n");
			goto _INITPHONE_FAIL;
		}

		// 设置音量
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT+CLVL=%d", objSta.m_iAudioOutVol*10+40))
		{
			PRTMSG(MSG_DBG, "set AT+CLVL fail\n");
			goto _INITPHONE_FAIL;
		}
		
		// 消除侧音
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT+SIDET=0"))
		{
			PRTMSG(MSG_DBG, "set AT+SIDET=0 fail\n");
			goto _INITPHONE_FAIL;
		}
		
		// 设定到来的电话铃声的声音级别
		if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT+CRSL=0"))
		{
			PRTMSG(MSG_DBG, "set AT+CRSL=0 fail\n");
			goto _INITPHONE_FAIL;
		}
	}
	// 设置免提/耳机模式
	//待添加
	//Set_AudioMode();

	//============= 以下为设置电话及短信相关的代码 ==============================

	// 拨号后要等待对方摘机后才返回+COLP和OK
	if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT+COLP=1"))
	{
		PRTMSG(MSG_DBG, "set AT+COLP=1 fail\n");
		goto _INITPHONE_FAIL;
	}

	// 开启来电显示
	if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT+CLIP=1"))
	{
		PRTMSG(MSG_DBG, "set AT+CLIP=1 fail\n");
		goto _INITPHONE_FAIL;
	}

	sleep(2);
	cnt = 0;
	while(1)
	{
		//初始化后，发现csms、CPMS和cnmi指令在很久之后才会成功
		// Set SMS Service
		if(ATANS_OK == _SetAT(3000, 3, NULL, 0, NULL, 0, "AT+CSMS=0"))
		{
			PRTMSG(MSG_DBG, "set AT+CSMS=0 succ\n");
			break;
		}
		else
		{
			PRTMSG(MSG_DBG, "set AT+CSMS=0 fail\n");
		}
		if (cnt++ > 60)
		{
			PRTMSG(MSG_DBG, "Long time AT+CSMS=0 fail%d\n");
			goto _INITPHONE_FAIL;
		}
		sleep(1);
	}

	cnt = 0;
	while(1)
	{
		// 设置短信到达后直接从串口送出来
		//set_at(7, "AT+CNMI=2,2,0,0,0\r\n"); // 第一个参数若为2,使得即使直接接收短信,也可能保存到SIM卡上(主要是在手机模块初始化时就收到大量短信时)
		if(ATANS_OK == _SetAT(3000, 3, NULL, 0, NULL, 0, "AT+CNMI=1,2,0,0,0"))
		{
			PRTMSG(MSG_DBG, "set AT+CNMI=1,2,0,0,0 succ\n");
			break;
		}
		else
		{
			PRTMSG(MSG_DBG, "set AT+CNMI fail\n");
		}
		if (cnt++ > 60)
		{
			PRTMSG(MSG_DBG, "Long time CNMI fail%d\n");
			goto _INITPHONE_FAIL;
		}
		sleep(1);
	}

	cnt = 0;
	while(1)
	{
		// 存储区设置// 第一个存储区要设置在SIM卡上,才能对SIM卡上的短信进行操作
		//set_at(3, "AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n"); // 即使全部设置为ME,但短信收到却没有处理时,仍可能保存到SIM卡上,且此时不能对SIM卡上的短信进行处理
		if(ATANS_OK == _SetAT(1000, 3, NULL, 0, NULL, 0, "AT+CPMS=\"SM\",\"ME\",\"ME\""))
		{
			PRTMSG(MSG_DBG, "set AT+CPMS succ\n");
			break;
		}
		else
		{
			PRTMSG(MSG_DBG, "set AT+CPMS fail\n");
		}
		if (cnt++ > 60)
		{
			PRTMSG(MSG_DBG, "Long time AT+CPMS fail%d\n");
			goto _INITPHONE_FAIL;
		}
		sleep(1);
	}

	// 设置PDU模式
	if(ATANS_OK != _SetAT(600, 5, NULL, 0, NULL, 0, "AT+CMGF=0"))
	{
		PRTMSG(MSG_DBG, "set AT+CMGF=0 fail\n");
		goto _INITPHONE_FAIL;
	}

	// 写短信中心号 (如:AT+CSCA="+8613800592500,145")
	//que 设置部分待添加
	static bool sta_bSetCsca = false;
	if( !sta_bSetCsca )
	{
		sta_bSetCsca = true;

		_SetAT( 3000, 1, NULL, 0, NULL, 0, "AT+CSCA=\"+8613800592500\",145" );
		
		// 先查询SIM卡内的短信中心号
		char szSmsCentID[100] = {0};
		_SetAT(1000, 3, szSmsCentID, sizeof(szSmsCentID), G_CSCA_Get, 0, "AT+CSCA?");
		PRTMSG(MSG_DBG, "SmsCent:%s\n", szSmsCentID );
		
		// 		// 设置短信中心号 (AT+CSCA="+8613800592500")
		// 		tag1PComuCfg cfg;
		// 		memset( &cfg, 0, sizeof(cfg) );
		// 		::GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
		// 		if( strcmp(cfg.m_szSmsCentID, "") )
		// 		{
		// 			if( strcmp(cfg.m_szSmsCentID, szSmsCentID) )
		// 			{
		// 				if(!set_at(5, 2, "AT+CSCA=\"%.21s\",%s\r\n", cfg.m_szSmsCentID,
		// 					'+'==cfg.m_szSmsCentID[0] ? "145" : "129") )
		// 				{ 
		// 					show_msg("Set CSCA(%.21s) Fail!", cfg.m_szSmsCentID);
		// 				}
		// 				else
		// 				{
		// 					show_msg("Set CSCA(%.21s) Succ!", cfg.m_szSmsCentID);
		// 				}
		// 			}
		// 		}
		// 		else
		// 		{
		// 			show_msg("No SmsCent In Cfg!");
		// 		}
	}

// 	// 免提/耳机模式选择
// 	if( 1 == g_iTerminalType )
// 	{	
// 		tag2QHstCfg obj2QHstCfg;
// 		::GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg),
// 			offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );
// 
// 		if (obj2QHstCfg.m_HandsfreeChannel)
// 		{
// 			// 设置耳机模式
// 			_PhoneAudioModeChg(AUDIOMODE_HANDFREE);
// 		}
// 		else
// 		{
// 			// 设置免提模式
// 			_PhoneAudioModeChg(AUDIOMODE_EARPHONE);
// 		}
// 	}
// 	else
// 	{
// 		// 设置免提模式
// 		_PhoneAudioModeChg(AUDIOMODE_HANDFREE);
// 	}

	// 获取信号
	_PhoneSingleGet( NULL );

	//testc
	//_SetAT( 1000, 2, NULL, 0, NULL, 0, "AT+CGDCONT=1,\"IP\",\"CMNET\",,0,0");
	
	PRTMSG(MSG_DBG, "Gsm init succ\n");
	return 0;

_INITPHONE_FAIL:
	return -1;
}

int CPhoneGsm::_PhoneStaHdl_Idle( int& v_iNextSta )
{
//	PRTMSG(MSG_NOR,"stIdle");

	int iRet = 0;
	DWORD dwPktNum;
	tagPhoneTask objPhoneTask;
	tagPhoneStaCommon objPhoneSta;

	// 清除不应有的,或没有处理价值,或已过时的任务指令
	_PhoneTaskClr( PHTASK_CHGAUDIO | PHTASK_CHGVOL | PHTASK_HANGUP | PHTASK_SNDDTMF );

	// 检查是否需要断开网络
	if( _PhoneJugCutNet() )
	{
		v_iNextSta = PST_CUTNET;
		goto _PHONEIDLE_END;
	}

	//检查是否有来电
	objPhoneTask = _PhoneTaskGet(PHTASK_RING);
	if( objPhoneTask.m_dwTaskSymb & PHTASK_RING )
	{
		// 进入ring状态前先初始化其下的环境变量
		m_objPhoneStaRing.Init();

		v_iNextSta = PST_RCVRING;
		goto _PHONEIDLE_END;
	}

	//检查是否有监听电话需要拨打
	objPhoneTask = _PhoneTaskGet(PHTASK_DIALSN);
	if( objPhoneTask.m_dwTaskSymb & PHTASK_DIALSN )
	{
		// 进入下一状态前输入下一状态独有的初始环境变量
		memcpy( m_objPhoneStaDialLsn.m_szDialLsnTel, objPhoneTask.m_szDialLsnTel,
			min(sizeof(m_objPhoneStaDialLsn.m_szDialLsnTel), sizeof(objPhoneTask.m_szDialLsnTel)) );
		m_objPhoneStaDialLsn.m_szDialLsnTel[ sizeof(m_objPhoneStaDialLsn.m_szDialLsnTel) - 1 ] = 0;
		m_objPhoneStaDialLsn.m_dwDialBeginTm = GetTickCount();
		
		v_iNextSta = PST_DIALLSN;
		goto _PHONEIDLE_END;
	}

	//检查是否有拨打电话请求
	objPhoneTask = _PhoneTaskGet(PHTASK_DIAL);
	if( objPhoneTask.m_dwTaskSymb & PHTASK_DIAL )
	{
		// 进入下一状态前输入下一状态独有的初始环境变量
		memcpy( m_objPhoneStaDial.m_szDialTel, objPhoneTask.m_szDialTel,
			min(sizeof(m_objPhoneStaDial.m_szDialTel), sizeof(objPhoneTask.m_szDialTel)) );
		m_objPhoneStaDial.m_szDialTel[ sizeof(m_objPhoneStaDial.m_szDialTel) - 1 ] = 0;
		m_objPhoneStaDial.m_dwDialBeginTm = GetTickCount();
		m_objPhoneStaDial.m_dwHoldLmtPrid = objPhoneTask.m_dwHoldLmtPrid;

		v_iNextSta = PST_DIAL;
		goto _PHONEIDLE_END;
	}

	// 检查是否需要设置短信中心号
	_PhoneJugSmsCentSet();
	
	// 检查是否要发送短信
	_PhoneJugSmsSnd();


	// ping并检查ping的结果
	objPhoneTask = _PhoneTaskGet(PHTASK_PING);
	if( objPhoneTask.m_dwTaskSymb & PHTASK_PING )
	{		
		v_iNextSta = PST_CUTNET;
		goto _PHONEIDLE_END;
	}
	
	//空闲时间隔5秒(进行信号检测等等)
	{
		static DWORD sta_dwLst = GetTickCount();
		if( GetTickCount() - sta_dwLst > 5000 ) 
		{
			sta_dwLst = GetTickCount();

			//定时给调度屏发送空闲指示
			Frm23 frm;
			g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			
			// 			//定时选择GSM免提和PC音频输出并关闭功放
			// 			_PhoneAudioModeChg( AUDIOMODE_CLOSE ); // 选择通话关闭模式
			
			objPhoneSta = _PhoneStaCommonGet(0);
			
			int iSignal = -1;
			_PhoneSingleGet( &iSignal );
			{
				//检查是否长时间弱信号
				static DWORD sta_dwLstNoWeakTm = GetTickCount();
				static DWORD sta_dwContWeakTimes = 0; // 连续检测弱信号次数
				if( iSignal > 0 && iSignal < MIN_GPRS_SIGNAL ) 
				{
					PRTMSG(MSG_DBG,"Low signal(%d)!", iSignal);
					sta_dwContWeakTimes ++;

					if( GetTickCount() - sta_dwLstNoWeakTm >= 180000 && sta_dwContWeakTimes > 1 )
					{ //长时间弱信号，最好复位一下模块(3分钟)
						PRTMSG(MSG_DBG,"Low signal(long time),reset Gsm!");

						sta_dwContWeakTimes = 0;
						sta_dwLstNoWeakTm = GetTickCount();
						v_iNextSta = PST_RESET;

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
					PRTMSG(MSG_DBG,"No signal(%d)!", iSignal);
					sta_dwContNoSingTimes ++;

					if( GetTickCount() - sta_dwLstSingNormTm >= 60000 && sta_dwContNoSingTimes > 1 )
					{ //长时间无信号，最好复位一下模块(1分钟)
						PRTMSG(MSG_DBG,"Low signal(long time),reset Gsm!");

						sta_dwContNoSingTimes = 0;
						sta_dwLstSingNormTm = GetTickCount();
						v_iNextSta = PST_RESET;

						goto _PHONEIDLE_END;
					}
				}
				else
				{
					sta_dwContNoSingTimes = 0;
					sta_dwLstSingNormTm = GetTickCount();
				}
			}
		}

//生产检测修改说明: 检测版本不进行GPRS拨号
#if CHK_VER == 0
		//检查是否需要拨号上网
		objPhoneSta = _PhoneStaCommonGet(0);
		if( !objPhoneSta.m_bNetConnected )
		{
			v_iNextSta = PST_DIALNET;

			goto _PHONEIDLE_END;
		}

// 		if(!m_simlack && !m_gprson && m_signal>MIN_GPRS_SIGNAL && m_signal!=99)	{ 
// 			m_iPhoneHdlSta = stGprsDial;
// 			break; 
// 		} 
#endif
	}

_PHONEIDLE_END:
	return iRet;
}

int CPhoneGsm::_PhoneStaHdl_DialLsn( int& v_iNextSta )
{
	if( 0 == m_objPhoneStaDialLsn.m_szDialLsnTel[0] )
	{
		v_iNextSta = PST_IDLE;
		goto _PHONEDIALLSN_END;
	}

	PRTMSG(MSG_NOR,"stDialListen");
	PRTMSG(MSG_DBG,"Dial listen(%.255s)...", m_objPhoneStaDialLsn.m_szDialLsnTel);

	//设置audio为监听模式
	_PhoneAudioModeChg(AUDIOMODE_LSN);
	
	//设置拨号后要等待对方摘机后才返回+COLP和OK (很容易失效的,所以要重新设置)
	if( ATANS_OK != _SetAT(1000, 3, NULL, 0, NULL, 0, "AT+COLP=1") )
	{
		PRTMSG(MSG_DBG, "set AT+COLP=1 fail\n");
		v_iNextSta = PST_IDLE;
		goto _PHONEDIALLSN_END;
	}
	
	//发送拨打指令
	if( ATANS_OK != _SetAT(4000, 5, NULL, 0, NULL, 0, "ATD%.20s;", m_objPhoneStaDialLsn.m_szDialLsnTel) )
	{
		PRTMSG(MSG_DBG,"Dial fail!");

		// 监听请求应答
		FrmF2 frm;
		frm.reply = 0x06;	// 车台手机模块忙
		DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
		
		v_iNextSta = PST_HANGUP;
		goto _PHONEDIALLSN_END;
	}
	else
	{
		// 此处不填充WAITLSNPICK状态下的环境变量，直接使用DIALLSN状态下的环境变量
		v_iNextSta = PST_WAITLSNPICK;
		goto _PHONEDIALLSN_END;
	}

_PHONEDIALLSN_END:
	return 0;
}

int CPhoneGsm::_PhoneStaHdl_WaitLsnPick( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stWaitLsnPick");

	int iResult = _ReadAT( 200, NULL, 0, G_JugRemotePick );

	// 可以使用AT+CLCC检测呼叫状态 (待)

	switch( iResult )
	{
	case ATANS_OK:
		{
			PRTMSG(MSG_DBG,"Dial succ,enter listen!");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x01;	//成功	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			m_objPhoneStaLsning.m_dwCallChkTm = GetTickCount();
			v_iNextSta = PST_LSNING;
		}
		break;

	case ATANS_ERROR:
		{
			PRTMSG(MSG_DBG,"Phone Err,hang up!");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x03;	//占线或设备忙	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
		break;

	case ATANS_NOCARRIER:
		{
			PRTMSG(MSG_DBG,"No Carrier,hang up!");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x03;	//占线或设备忙	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
		break;

	case ATANS_NODIALTONE:
		{
			PRTMSG(MSG_DBG,"No Dialtone,hang up!");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x03;	//占线或设备忙	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
		break;

	case ATANS_BUSY:
		{
			PRTMSG(MSG_DBG,"Phone busy,hang up!");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x03;	//占线或设备忙	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
		break;

	case ATANS_NOANSWER:
		{
			PRTMSG(MSG_DBG,"Phone No Answer,hang up!");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x04;	// 拨打超时或无应答
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
		break;

	case ATANS_NOANS:
	case ATANS_OTHER:
	default:
		if( GetTickCount() - m_objPhoneStaDialLsn.m_dwDialBeginTm > 90000 )
		{
			PRTMSG(MSG_DBG,"Dial Lsn,but wait pick time up,hang up");

			// 监听请求应答
			FrmF2 frm;
			frm.reply = 0x04;	// 拨打超时或无应答
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
			
			v_iNextSta = PST_HANGUP;
		}
	}

	return 0;
}

int CPhoneGsm::_PhoneStaHdl_Lsning(int& v_iNextSta)
{
	PRTMSG(MSG_NOR,"stListening");

	// 判断是否要主动挂机 (原因是检测到NO CARRIER,即对方挂机)
	tagPhoneTask objPhoneTask = _PhoneTaskGet( PHTASK_HANGUP );
	if( objPhoneTask.m_dwTaskSymb & PHTASK_HANGUP )
	{
		v_iNextSta = PST_HANGUP;

		FrmF2 frm;
		frm.reply = 0x05;	//监听结束	
		DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
		return 0;
	}

	int iResult = _ReadAT( 200, NULL, 0, G_JugRemoteHangup );

	switch( iResult )
	{
	case ATANS_ERROR:
	case ATANS_NOCARRIER:
		{
			PRTMSG(MSG_DBG,"Lsning,but no carrier or err,hang up");

			v_iNextSta = PST_HANGUP;

			FrmF2 frm;
			frm.reply = 0x05;	//监听结束	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);
			return 0;
			
			v_iNextSta = PST_HANGUP;
		}
		break;

	default:
		{
			// 隔一段时间检测是否通话中...
			if( GetTickCount() - m_objPhoneStaLsning.m_dwCallChkTm > 10000 )
			{
				m_objPhoneStaLsning.m_dwCallChkTm = GetTickCount();
				if( ATANS_OK != _SetAT(1000, 3, NULL, 0, G_CLCC_JugRes, 0, "AT+CLCC") )
				{
					PRTMSG(MSG_DBG, "Lsning,but chk call fail,hang up" );
					v_iNextSta = PST_HANGUP;
					if(PHONE_BENQ==m_GsmType)
					{
						_SetAT(1000, 3, NULL, 0, NULL, 0, "AT$AUAEC=566"); //重新设置回音抑制
					}
				}
			}
		}
	}

	return 0;
}

int CPhoneGsm::_PhoneStaHdl_Dial( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stDial\n");

	DWORD dwPktNum;
	// 判断是否在报警状态内
	tagPhoneStaCommon objSta = _PhoneStaCommonGet(0);
	if( GetTickCount() - objSta.m_dwAlertBegin < objSta.m_dwAlertPrid )
	{
		PRTMSG(MSG_DBG,"Alert! Stop dial,hang up!");

		// 拨号请求应答
		Frm13 frm;
		frm.reply = 0x02;	
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);	//手机忙
		
		v_iNextSta = PST_HANGUP;
		return 0;
	}

	if( 0 == m_objPhoneStaDial.m_szDialTel[0] )
	{
		v_iNextSta = PST_IDLE;
		return 0;
	}
		
	// 选择拨号模式
	_PhoneAudioModeChg( AUDIOMODE_DIAL );
	
//  	// 设置拨号后要等待对方摘机后才返回+COLP和OK(很容易失效,所以要重新设置)
// 	if( ATANS_OK != _SetAT(1000, 2, NULL, 0, NULL, 0, "AT+COLP=1") )
// 	{
// 		PRTMSG(MSG_DBG, "Set COLP Fail\n");
// 	}

	//发送拨打指令
	if( ATANS_OK != _SetAT(8000, 1, NULL, 0, NULL, 0, "ATD%.20s;", m_objPhoneStaDial.m_szDialTel) )
	//if( ATANS_OK != _SetAT(2000, 5, NULL, 0, NULL, 0, "ATD15980856182;") )
	{
		PRTMSG(MSG_ERR,"Dial fail!");

		// 拨号请求应答
		Frm13 frm;
		frm.reply = 0x02;	//手机正忙	
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
		
		v_iNextSta = PST_HANGUP;
		goto _PHONEDIAL_END;
	}
	else
	{
		PRTMSG(MSG_DBG,"dial ATD%.20s", m_objPhoneStaDial.m_szDialTel);
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
				if(_PhoneAudioModeChg(AUDIOMODE_HANDFREE))
				{
					PRTMSG(MSG_ERR,"_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_HANDFREE);
				}
			}
			else
			{
				if(_PhoneAudioModeChg(AUDIOMODE_EARPHONE))
				{
					PRTMSG(MSG_ERR,"_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_EARPHONE);
				}
			}
		}
		else
		{
			// 设置免提音频模式
			if(_PhoneAudioModeChg(AUDIOMODE_HANDFREE))
			{
				PRTMSG(MSG_ERR,"_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_HANDFREE);
			}
		}

		// 此处不填充WAITPICK状态下的环境变量，WAITPICK状态直接使用DIAL状态下的环境变量
		v_iNextSta = PST_WAITPICK;
	}

_PHONEDIAL_END:
	return 0;
}

int CPhoneGsm::_PhoneStaHdl_WaitPick( int &v_iNextSta )
{
	PRTMSG(MSG_NOR, "stWaitPick");

	DWORD dwPktNum;
	// 判断是否在报警状态内
	tagPhoneStaCommon objSta = _PhoneStaCommonGet(0);
	if( GetTickCount() - objSta.m_dwAlertBegin < objSta.m_dwAlertPrid )
	{
		PRTMSG(MSG_DBG,"Alert! Stop waiting pick,hang up!");

		// 拨号请求应答
		Frm13 frm;
		frm.reply = 0x02;	
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);	//手机忙
		
		v_iNextSta = PST_HANGUP;
		return 0;
	}

	// 判断是否要主动挂机
	tagPhoneTask objPhoneTask = _PhoneTaskGet( PHTASK_HANGUP );
	if( objPhoneTask.m_dwTaskSymb & PHTASK_HANGUP )
	{
		v_iNextSta = PST_HANGUP;

		return 0;
	}

	char szResult[100] = { 0 };
	int iResult = _ReadAT( 200, szResult, sizeof(szResult), G_JugRemotePick );

	// 可以使用AT+CLCC检测呼叫状态 (待)

	switch( iResult )
	{
	case ATANS_OK:
		{
			PRTMSG(MSG_DBG,"Dial succ,enter hold!");

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
					if( _PhoneAudioModeChg(AUDIOMODE_HANDFREE) )
					{
						PRTMSG(MSG_ERR,"_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_HANDFREE);
					}
				}
				else
				{
					if( _PhoneAudioModeChg(AUDIOMODE_EARPHONE) )
					{
						PRTMSG(MSG_ERR,"_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_EARPHONE);
					}
				}
			}
			else
			{
				// 设置免提音频模式
				if( _PhoneAudioModeChg(AUDIOMODE_HANDFREE) )
				{
					PRTMSG(MSG_ERR,"_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_HANDFREE);
				}
			}

			// 初始化Hold状态的独有的环境变量
			m_objPhoneStaHold.m_dwHoldLmtPrid = m_objPhoneStaDial.m_dwHoldLmtPrid;
			m_objPhoneStaHold.m_dwHoldBeginTm = m_objPhoneStaHold.m_dwCallChkTm = ::GetTickCount();
			v_iNextSta = PST_HOLD;
		}
		break;

	case ATANS_ERROR:
		{
			PRTMSG(MSG_ERR,"Phone Err,hang up!");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x02;	//手机忙	
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
		break;

	case ATANS_NOCARRIER:
		{
			PRTMSG(MSG_ERR,"No Carrier,hang up!");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x04; // 未搜索到网络
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
		break;

	case ATANS_NODIALTONE:
		{
			PRTMSG(MSG_ERR,"No Dialtone,hang up!");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x02;	//手机忙
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
		break;

	case ATANS_BUSY:
		{
			PRTMSG(MSG_ERR,"Phone busy,hang up!");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x02;	//手机忙
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
		break;

	case ATANS_NOANSWER:
		{
			PRTMSG(MSG_ERR,"Phone No Answer,hang up!");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x02;	//手机忙
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
		break;

	case ATANS_NOANS:
	case ATANS_OTHER:
	default:
		if( GetTickCount() - m_objPhoneStaDial.m_dwDialBeginTm > 90000 ) // 拨打超时，应该挂断
		{
			PRTMSG(MSG_ERR,"Dial,but wait pick time up,hang up");

			// 拨号请求应答
			Frm13 frm;
			frm.reply = 0x02;	//手机忙
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_QIAN, 2);

			v_iNextSta = PST_HANGUP;
		}
	}

	return 0;
}

int CPhoneGsm::_PhoneStaHdl_Hold( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stHold");

	int iResult = 0;
	DWORD dwPktNum;
	tagPhoneTask objPhoneTask;
	tagPhoneStaCommon objSta;

	// 判断是否在报警状态内
	objSta = _PhoneStaCommonGet(0);
	if( ::GetTickCount() - objSta.m_dwAlertBegin < objSta.m_dwAlertPrid )
	{
		PRTMSG(MSG_DBG,"Alert! Stop hold,hang up!");
		
		v_iNextSta = PST_HANGUP;
		goto _PHONEHDLHOLD_END;
	}

	// 判断是否要主动挂机
	objPhoneTask = _PhoneTaskGet( PHTASK_HANGUP );
	if( objPhoneTask.m_dwTaskSymb & PHTASK_HANGUP )
	{
		v_iNextSta = PST_HANGUP;

		goto _PHONEHDLHOLD_END;
	}

	// 通话时长限制
	if( m_objPhoneStaHold.m_dwHoldLmtPrid &&
		::GetTickCount() - m_objPhoneStaHold.m_dwHoldBeginTm >= m_objPhoneStaHold.m_dwHoldLmtPrid )
	{
		PRTMSG(MSG_DBG,"Hold time is limit(%u),hang up!", m_objPhoneStaHold.m_dwHoldLmtPrid / 1000 );

		v_iNextSta = PST_HANGUP;
		goto _PHONEHDLHOLD_END;
	}
	
	// 免提耳机切换 (手柄模式下)
	objPhoneTask = _PhoneTaskGet( PHTASK_CHGAUDIO );
	if( objPhoneTask.m_dwTaskSymb & PHTASK_CHGAUDIO )
	{
		objSta = _PhoneStaCommonGet(0);

		if( AUDIOMODE_EARPHONE == objSta.m_bytCurAudioMode )
		{
			_PhoneAudioModeChg( AUDIOMODE_HANDFREE );
		}
		else
		{
			_PhoneAudioModeChg( AUDIOMODE_EARPHONE );
		}

		// 免提/耳机切换应答
		Frm27 frm;
		frm.reply = 0x01;
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
	}
	
	// 发送DTMF
	objPhoneTask = _PhoneTaskGet( PHTASK_SNDDTMF );
	if( objPhoneTask.m_dwTaskSymb & PHTASK_SNDDTMF )
	{
		//发送dtmf指令
		int iResult = _SetAT( strlen(objPhoneTask.m_szDTMF) * 1000, 2, NULL, 0, NULL, 0, "AT+VTS=\"%s\"", objPhoneTask.m_szDTMF );
		
		// DTMF发送请求应答
		Frm11 frm;
		frm.reply = ATANS_OK == iResult ? 0x01 : 0x02;
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
	}

	// 调节音量
	objPhoneTask = _PhoneTaskGet( PHTASK_CHGVOL );
	if( objPhoneTask.m_dwTaskSymb & PHTASK_CHGVOL )
	{
		// 或者调整audio scene ? (待)

		iResult = _PhoneAudioOutVolChg( objPhoneTask.m_iChgVol );

		// 音量调节请求应答
		Frm29 frm;
		frm.reply = (!iResult ? 0x01 : 0x02);
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);	
	}

	iResult = _ReadAT( 200, NULL, 0, G_JugRemoteHangup );

	switch( iResult )
	{
	case ATANS_ERROR:
	case ATANS_NOCARRIER:
		{
			PRTMSG(MSG_DBG,"Holding,but no carrier or err,hang up");
			
			v_iNextSta = PST_HANGUP;
		}
		break;

	default:
		{
			// 隔一段时间检测是否通话中... (检查的可以频繁一点)
			if( GetTickCount() - m_objPhoneStaHold.m_dwCallChkTm > 10000 )
			{
				m_objPhoneStaHold.m_dwCallChkTm = GetTickCount();
				if( ATANS_OK != _SetAT(1000, 3, NULL, 0, G_CLCC_JugRes, 0, "AT+CLCC") )
				{
					PRTMSG(MSG_ERR,"Holding,but check call fail,hang up");
					v_iNextSta = PST_HANGUP;
				}
			}
		}
	}

_PHONEHDLHOLD_END:
	return 0;
}

int CPhoneGsm::_PhoneStaHdl_RcvRing( int& v_iNextSta )
{
	PRTMSG(MSG_NOR,"stRcvRing");

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
	iResult = _ReadAT( dwClipWait, szRingTel, sizeof(szRingTel), G_CLIP_ReadTel );
	if( ATANS_OK != iResult && ATANS_NOANS != iResult && ATANS_RCVNOTALL != iResult )
	{
		// 挂机
		v_iNextSta = PST_HANGUP;
		return 0;
	}

	// 判断是否读取到ring
	if( ATANS_OK == iResult || ATANS_RCVNOTALL == iResult )
	{
		bToJugRingActive = false; // 能取得来电信息则不需要检查振铃状态
	}

	if( 0 == m_objPhoneStaRing.m_szRingTel[0] ) // 若来电号码还未获取到
	{
		// 来电号码处理
		if( 0 == szRingTel[0] ) // 若本次从clip获取来电号码失败
		{
			// 主动提取来电号码
			_SetAT( 1800, 1, szRingTel, sizeof(szRingTel), G_CLCC_ReadTel, 0, "AT+CLCC" );
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
			iResult = _SetAT(1000, 2, NULL, 0, G_CPAS_JugRes, 0, "AT+CPAS");

			if( ATANS_OK != iResult )
			{
				// 挂机
				v_iNextSta = PST_HANGUP;
				return 0;
			}
		}
	}

	PRTMSG(MSG_DBG,"Ring %.15s(%02x, %u)...", szRingTel, type, wLmtPrid);

	tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);

	// 呼入类型：0,允许呼入 1,中心主动监听号码 2,禁止呼入号码 3,中心服务电话号码 4,UDP激活号码
	if(type==1) // 若是中心主动监听
	{
		PRTMSG(MSG_DBG,"Ring(%.25s) listen...", szRingTel);

		if( ATANS_OK == _SetAT(1000, 3, NULL, 0, NULL, 0, "ATA") )
		{
			PRTMSG(MSG_DBG,"Auto pick up, Listening...!");
			if( 0 == _PhoneAudioModeChg(AUDIOMODE_LSN) )
			{
				objPhoneSta.m_bytCurAudioMode = AUDIOMODE_LSN;
				_PhoneStaCommonSet( objPhoneSta, PHSTVAR_AUDIOMODE );
			}
			
			// 下一状态环境变量初始化
			m_objPhoneStaLsning.m_dwCallChkTm = ::GetTickCount();

			v_iNextSta = PST_LSNING;
		}
		else
		{
			v_iNextSta = PST_HANGUP; // 保险起见还是挂机一下较好
		}
	}
	else if(type==4) // UDP激活号码
	{
		PRTMSG(MSG_DBG,"UDP active(%.25s),hang up!", szRingTel);

		Frm35 frm;	//网络恢复通知帧
		frm.reply = 0x01;	//拨号成功
		DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_SOCK, 2);	//通知千里眼(KJ2)UDP重登

		v_iNextSta = PST_HANGUP;
	}
	else if( GetTickCount()-objPhoneSta.m_dwAlertBegin < objPhoneSta.m_dwAlertPrid )
	{ // 判断是否在报警状态内(非监听电话则挂断)
		PRTMSG(MSG_DBG,"In Alarm Period,hang up ring(%.25s)!", szRingTel);
		v_iNextSta = PST_HANGUP;
	}
	else if( 3 == type ) // 中心服务电话号码
	{
		PRTMSG(MSG_DBG,"Ring(%.25s) CentServ Tel..", szRingTel);

		Frm19 frm;
		frm.len = min(strlen(szRingTel), 25);
		memcpy(frm.num, szRingTel, frm.len);
		g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, 2+frm.len, (char*)&frm, dwPktNum);
	}
	else if(type==2) // 禁止呼入号码
	{
		PRTMSG(MSG_DBG,"Ring(%.25s) forbid,hang up!", szRingTel);
		v_iNextSta = PST_HANGUP;
	}
	else
	{
		PRTMSG(MSG_DBG,"Ring(%.25s)...", szRingTel);
	}

	if( PST_RCVRING == v_iNextSta ) // 若前面的判断处理未改变状态
	{
		tagPhoneTask objPhoneTask = _PhoneTaskGet( PHTASK_HANGUP | PHTASK_PICKUP );
		if( objPhoneTask.m_dwTaskSymb & PHTASK_HANGUP ) // 若要主动挂断
		{
			v_iNextSta = PST_HANGUP;
		}
		else if( objPhoneTask.m_dwTaskSymb & PHTASK_PICKUP ) // 若用户要接听
		{
			PRTMSG(MSG_DBG,"User Pick up");

// 			// 选择GSM音频输出并打开功放
// 			g_df.set_soudbox_gsm();
// 			g_df.set_soudbox_openpa(1);

			if( ATANS_OK == _SetAT(1000, 3, NULL, 0, NULL, 0, "ATA") )
			{
				PRTMSG(MSG_DBG,"Pick up Succ, Holding..!");

				if(1==g_iTerminalType) //如果是手柄模式
				{
					tag2QHstCfg obj2QHstCfg;
					GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg),
						offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );

					// 恢复默认配置
					if( obj2QHstCfg.m_HandsfreeChannel )
					{
						if( _PhoneAudioModeChg(AUDIOMODE_HANDFREE) )
						{
							PRTMSG(MSG_ERR,"_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_HANDFREE);
						}
					}
					else
					{
						if( _PhoneAudioModeChg(AUDIOMODE_EARPHONE) )
						{
							PRTMSG(MSG_ERR,"_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_EARPHONE);
						}
					}
				}
				else
				{
					// 设置免提音频模式
					if( _PhoneAudioModeChg(AUDIOMODE_HANDFREE) )
					{
						PRTMSG(MSG_ERR,"_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_HANDFREE);
					}
				}
				
				// 下一状态环境变量初始化
				m_objPhoneStaHold.m_dwHoldBeginTm = m_objPhoneStaHold.m_dwCallChkTm = ::GetTickCount();
				m_objPhoneStaHold.m_dwHoldLmtPrid = m_objPhoneStaRing.m_dwHoldLmtPrid;

				v_iNextSta = PST_HOLD;

				// 摘机请求应答
				Frm15 frm;
				frm.reply = 1;
				g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, sizeof(frm), (char*)&frm, dwPktNum);
			}
			else
			{
				PRTMSG(MSG_ERR,"Pick up Fail, Hang up");
				v_iNextSta = PST_HANGUP; // (为保险起见也还是挂机一下较好)

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


int CPhoneGsm::_PhoneStaHdl_Hangup( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stHangup");
	DWORD dwPktNum;

	// 清除不应有的,或没有处理价值,或已过时的任务指令
	_PhoneTaskClr( PHTASK_CHGAUDIO | PHTASK_CHGVOL | PHTASK_HANGUP | PHTASK_RING | PHTASK_SNDDTMF );

	v_iNextSta = PST_IDLE; // 预置处理后的新状态值

	int iResult = _SetAT(15000, 3, NULL, 0, NULL, 0, "ATH"); // 仅挂断语音
	if( ATANS_OK != iResult )
	{
		v_iNextSta = PST_RESET;
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
	_PhoneAudioModeChg( AUDIOMODE_CLOSE );

// 	if(1==g_iTerminalType) //如果是手柄模式
// 	{
// 		tag2QHstCfg obj2QHstCfg;
// 		::GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg),
// 			offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );
// 
// 		// 恢复默认配置
// 		if( obj2QHstCfg.m_HandsfreeChannel )
// 		{
// 			if(_PhoneAudioModeChg(AUDIOMODE_HANDFREE))
// 			{
// 				show_msg("_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_HANDFREE);
// 			}
// 		}
// 		else
// 		{
// 			if(_PhoneAudioModeChg(AUDIOMODE_EARPHONE))
// 			{
// 				show_msg("_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_EARPHONE);
// 			}
// 		}
// 	}
// 	else
// 	{	
// 		// 设置免提音频模式
// 		if(_PhoneAudioModeChg(AUDIOMODE_HANDFREE))
// 		{
// 			show_msg("_PhoneAudioModeChg(%d) Fail!", AUDIOMODE_HANDFREE);
// 		}
// 	}

	// 通话结束后取消正在进行的ping操作


	return 0;
}

int CPhoneGsm::_PhoneStaHdl_Reset()
{
	PRTMSG(MSG_NOR,"stReseting\n");

	// 选择通话关闭模式
	_PhoneAudioModeChg( AUDIOMODE_CLOSE );

	// 更新状态
	tagPhoneStaCommon objPhoneSta;
	objPhoneSta.m_bNetConnected = false;
	_PhoneStaCommonSet( objPhoneSta, PHSTVAR_NET );

	// 程序关闭的同步控制


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
	_PhoneStaCommonClr( PHSTVAR_SING | PHSTVAR_NET );

	// 关闭GPRS(串口7)
 	_PhoneCutNet();
 	usleep(500000);

	_SetAT( 2000, 2, NULL, 0, NULL, 0, "ATH" );
	
	// 关闭手机模块(防止烧卡)
	_SetAT( 1000, 3, NULL, 0, NULL, 0, "AT+CFUN=0" );
	
	if(PHONE_BENQ==m_GsmType) 
	{
		_SetAT( 1000, 3, NULL, 0, NULL, 0, "AT$POWEROFF" );
	}
	if(PHONE_SIMCOM==m_GsmType)
	{
		_SetAT( 1000, 3, NULL, 0, NULL, 0, "AT+CPOWD=1" );
	}
	
	// Benq建议Power off后休眠5秒钟
	sleep(5);
	
	//关闭串口
	_CloseCom();

	//同步控制手机模块关闭
	if( m_bToClosePhone )
	{
		usleep(5000);
		return 0;
	}

	//GSM模块复位
	g_objComVir.ReleaseComVir();//释放ComVir的资源
	_GsmHardReset();	// 模块的硬件复位
	g_objComVir.InitComVir();//

	PRTMSG(MSG_DBG,"Gsm wait ready...\n");
	sleep(3);

	return 0;
}

int CPhoneGsm::_PhoneStaHdl_DialNet( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stDialNet\n");
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	RenewMemInfo(0x02, 0x02, 0x01, 0x02);
	
	tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	if(objPhoneSta.m_iSignal < 10)
		RenewMemInfo(0x02, 0x02, 0x01, 0x01);
#endif
	
	// 获取APN
	char szApn[100] = { 0 };
	tag1QIpCfg objIpCfg;
	int iResult = GetImpCfg( &objIpCfg, sizeof(objIpCfg),
		offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg[0]),sizeof(objIpCfg) );
	if( !iResult )
	{
		strncpy( szApn, objIpCfg.m_szApn, min(strlen(objIpCfg.m_szApn), sizeof(szApn) - 1) );
	}
	else
	{
		strcpy( szApn, "CMNET" );
	}

	//_SetAT(5000, 5, NULL, 0, G_CREG_Jugres, 0, "AT+CREG?"); //testc

	
	// 以下2条指令，若由系统的拨号处理时自动进行，因为指令太长，WinCE无法处理，所以改为由ComuExe设置
	iResult = _SetAT( 1000, 2, NULL, 0, NULL, 0, "AT+CGDCONT=1,\"IP\",\"%s\",,0,0", szApn );
	//	iResult = _SetAT( 1000, 2, NULL, 0, NULL, 0, "AT+CGDCONT=1,\"IP\",\"cmnet\",,0,0", szApn );
	// 	if (ATANS_OK == iResult)
	// 	{
	// 		PRTMSG(MSG_DBG, "set AT+CGDCONT=1,\"IP\",\"%s\",,0,0 succ\n", szApn);
	// 	}
// 	if( ATANS_OK == iResult )
// 	{
// 		iResult = _SetAT( 1000, 2, NULL, 0, NULL, 0, "AT+CGEQREQ=1,2,128,384,0,0,0,0,\"0E0\",\"0E0\",,0,0" );
// 	}
	
	if( ATANS_OK != iResult )
	{
		static BYTE sta_bytDialNetSetAtFailTimes = 0;
		sta_bytDialNetSetAtFailTimes ++;
		if( sta_bytDialNetSetAtFailTimes >= 2 )
		{
			v_iNextSta = PST_RESET;
			sta_bytDialNetSetAtFailTimes = 0;
		}
		else
		{
			v_iNextSta = PST_CUTNET;
		}
		
		return 0;
	}
	
	
	static BYTE sta_bytDialNetFailTime = 0;
	if( 0 == _PhoneDialNet() ) // 若拨号动作成功
	{
		m_dwDialNetTm = GetTickCount();
		sta_bytDialNetFailTime = 0;
		v_iNextSta = PST_WAITDIALNET;
	}
	else
	{
		sta_bytDialNetFailTime ++;
		if( sta_bytDialNetFailTime >= 2 ) // 若连续2次拨号动作失败
		{
			v_iNextSta = PST_RESET;
		}
		else
		{
			sleep(1);
			v_iNextSta = PST_CUTNET;
		}
	}
	
	return 0;
}

int CPhoneGsm::_PhoneStaHdl_WaitDialNet( int &v_iNextSta )
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
		_PhoneTaskClr( PHTASK_CUTNET );
		
		// 取消当前和将要进行的ping任务
		_PhoneTaskClr( PHTASK_PING );
		/// } 清除只与网络相关的任务指令
		
		// 更新状态
		tagPhoneStaCommon objPhoneSta;
		objPhoneSta.m_bNetConnected = true;
		_PhoneStaCommonSet( objPhoneSta, PHSTVAR_NET );
		
		// 回到空闲状态
		v_iNextSta = PST_IDLE;
	}
	else
	{
		if ((GetTickCount()-m_dwDialNetTm) > 1*60*1000)//等待
		{
			PRTMSG(MSG_NOR, "Dial Net Timeout!\n");
			
			//网络断开通知帧
			Frm35 frm;
			frm.reply = 0x03;
			DataPush((char*)&frm, sizeof(frm), DEV_PHONE, DEV_SOCK, 2);
			
			// 控制指示灯
#if VEHICLE_TYPE == VEHICLE_M
			g_objLightCtrl.SetLightSta(0x03);	// 慢闪
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
			g_objLightCtrl2.SetLightSta(0x02);	// 网络断开通知
#endif
			
			// 更新状态
			tagPhoneStaCommon objPhoneSta;
			objPhoneSta.m_bNetConnected = false;
			_PhoneStaCommonSet( objPhoneSta, PHSTVAR_NET );
			
			v_iNextSta = PST_RESET; // 拨号失败就复位模块
			sleep( 12 ); //等待一会,以便手机模块有时间收取短信(避免APN设错都没有时间接收配置短信)
		}
		else
		{
			usleep(500 * 1000);
		}
	}
	
	return 0;
}

void CPhoneGsm::_PhoneStaHdl_CutNet( int &v_iNextSta )
{
	PRTMSG(MSG_NOR,"stCutNet\n");

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
	tagPhoneStaCommon objPhoneSta;
	objPhoneSta.m_bNetConnected = false;
	_PhoneStaCommonSet( objPhoneSta, PHSTVAR_NET );
	/// } 放在断网操作的前面

	_PhoneCutNet();

	/// { 清除只与网络相关的任务指令 (要放在断网操作的后面,因为断开网络的执行需要一段时间)
	// 取消断开网络任务
	_PhoneTaskClr( PHTASK_CUTNET );

	// 取消当前和将要进行的ping任务
	_PhoneTaskClr( PHTASK_PING );
	/// } 清除只与网络相关的任务指令 (要放在断网操作的后面,因为断开网络的执行需要一段时间)

	if( ATANS_OK == _SetAT(1000, 3, NULL, 0, NULL, 0, "AT") )
	{
		v_iNextSta = PST_IDLE;
	}
	else
	{
		v_iNextSta = PST_RESET;
	}
}

//函数功能：	手机模块任务清除
//线程安全：	是
//调用方式：	调用函数成员变量需做互斥
void CPhoneGsm::_ClearPhoneTask( DWORD v_dwToGetTaskSymb, tagPhoneTask& v_objPhoneTask )
{
	v_objPhoneTask.m_dwTaskSymb &= ~v_dwToGetTaskSymb;

	if( v_dwToGetTaskSymb & PHTASK_DIALSN )
	{
		memset( v_objPhoneTask.m_szDialLsnTel, 0, sizeof(v_objPhoneTask.m_szDialLsnTel) );
	}
	if( v_dwToGetTaskSymb & PHTASK_DIAL )
	{
		memset( v_objPhoneTask.m_szDialTel, 0, sizeof(v_objPhoneTask.m_szDialTel) );
		v_objPhoneTask.m_dwHoldLmtPrid = 0;
	}
	if( v_dwToGetTaskSymb & PHTASK_RING )
	{
		//memset( v_objPhoneTask.m_szRingTel, 0, sizeof(v_objPhoneTask.m_szRingTel) );
	}
	if( v_dwToGetTaskSymb & PHTASK_SETSMSCENT )
	{
	}
	if( v_dwToGetTaskSymb & PHTASK_SNDDTMF )
	{
		memset( v_objPhoneTask.m_szDTMF, 0, sizeof(v_objPhoneTask.m_szDTMF) );
	}
	if( v_dwToGetTaskSymb & PHTASK_HANGUP )
	{
	}
	if( v_dwToGetTaskSymb & PHTASK_PICKUP )
	{
	}
	if( v_dwToGetTaskSymb & PHTASK_CHGAUDIO )
	{
		v_objPhoneTask.m_bytChgAudio = 0;
	}
	if( v_dwToGetTaskSymb & PHTASK_CHGVOL )
	{
		v_objPhoneTask.m_iChgVol = 0;
	}
	if( v_dwToGetTaskSymb & PHTASK_PING )
	{
		v_objPhoneTask.m_bytPingSrc = 0;
	}
	if( v_dwToGetTaskSymb & PHTASK_PINGRPT )
	{
		v_objPhoneTask.m_bytPingResult = 0;
		v_objPhoneTask.m_bytPingRptSrc = 0;
	}
	if( v_dwToGetTaskSymb & PHTASK_CUTNET )
	{
	}
	if( v_dwToGetTaskSymb & PHTASK_ALERM )
	{
		v_objPhoneTask.m_dwAlertPrid = 0;
	}
	if( v_dwToGetTaskSymb & PHTASK_RESET )
	{
	}
}


//函数功能：	手机模块任务标志获取
//线程安全：	是
//调用方式：	
// 在取任务的同时就要清除该任务标志
tagPhoneTask CPhoneGsm::_PhoneTaskGet(DWORD v_dwToGetTaskSymb)
{
	tagPhoneTask objRet;
	
	pthread_mutex_lock( &m_crisecPhoneTask );
	objRet = m_objPhoneTask; // 先填充返回值
	_ClearPhoneTask(v_dwToGetTaskSymb, m_objPhoneTask); // 再清除对应类型
	pthread_mutex_unlock( &m_crisecPhoneTask );

	return objRet;
}

//函数功能：	手机模块任务标志设置
//线程安全：	是
//调用方式：	
void CPhoneGsm::_PhoneTaskSet( const tagPhoneTask& v_objPhoneTask, DWORD v_dwToSetTaskSymb )
{
	pthread_mutex_lock( &m_crisecPhoneTask );

	// 设置命令标志位
	m_objPhoneTask.m_dwTaskSymb |= v_dwToSetTaskSymb;
	
	// 设置具体的命令参数
	if( v_dwToSetTaskSymb & PHTASK_DIALSN )
	{
		memcpy( m_objPhoneTask.m_szDialLsnTel, v_objPhoneTask.m_szDialLsnTel, sizeof(m_objPhoneTask.m_szDialLsnTel) );
	}
	if( v_dwToSetTaskSymb & PHTASK_DIAL )
	{
		memcpy( m_objPhoneTask.m_szDialTel, v_objPhoneTask.m_szDialTel, sizeof(m_objPhoneTask.m_szDialTel) );
		m_objPhoneTask.m_dwHoldLmtPrid = v_objPhoneTask.m_dwHoldLmtPrid;
	}
	if( v_dwToSetTaskSymb & PHTASK_RING )
	{
	}
	if( v_dwToSetTaskSymb & PHTASK_SETSMSCENT )
	{
	}
	if( v_dwToSetTaskSymb & PHTASK_SNDDTMF )
	{
		// 这是收到多个DTMF请求可同时发出的模式
		if( strlen(m_objPhoneTask.m_szDTMF) + 1 < sizeof(m_objPhoneTask.m_szDTMF) )
		{
			strncat( m_objPhoneTask.m_szDTMF, v_objPhoneTask.m_szDTMF,
				sizeof(m_objPhoneTask.m_szDTMF) - strlen(m_objPhoneTask.m_szDTMF) - 1 );
		}
	}
	if( v_dwToSetTaskSymb & PHTASK_HANGUP )
	{
	}
	if( v_dwToSetTaskSymb & PHTASK_PICKUP )
	{
	}
	if( v_dwToSetTaskSymb & PHTASK_CHGAUDIO )
	{
		m_objPhoneTask.m_bytChgAudio = v_objPhoneTask.m_bytChgAudio;
	}
	if( v_dwToSetTaskSymb & PHTASK_CHGVOL )
	{
		m_objPhoneTask.m_iChgVol = v_objPhoneTask.m_iChgVol;
	}
	if( v_dwToSetTaskSymb & PHTASK_PING )
	{
		m_objPhoneTask.m_bytPingSrc = v_objPhoneTask.m_bytPingSrc;
	}
	if( v_dwToSetTaskSymb & PHTASK_PINGRPT )
	{
		m_objPhoneTask.m_bytPingRptSrc = v_objPhoneTask.m_bytPingRptSrc;
		m_objPhoneTask.m_bytPingResult = v_objPhoneTask.m_bytPingResult;
	}
	if( v_dwToSetTaskSymb & PHTASK_CUTNET )
	{
	}
	if( v_dwToSetTaskSymb & PHTASK_ALERM )
	{
		m_objPhoneTask.m_dwAlertPrid = v_objPhoneTask.m_dwAlertPrid;
	}
	if( v_dwToSetTaskSymb & PHTASK_RESET )
	{
	}

	pthread_mutex_unlock( &m_crisecPhoneTask );
}

//函数功能：	手机模块任务标志清除
//线程安全：	是
//调用方式：
void CPhoneGsm::_PhoneTaskClr(DWORD v_dwToGetTaskSymb)
{
	// 事后清除指定的任务状态标志
	pthread_mutex_lock( &m_crisecPhoneTask );
	_ClearPhoneTask(v_dwToGetTaskSymb, m_objPhoneTask);
	pthread_mutex_unlock( &m_crisecPhoneTask );
}

//函数功能：	手机模块状态获取
//线程安全：	是
//调用方式：	
tagPhoneStaCommon CPhoneGsm::_PhoneStaCommonGet( DWORD v_dwToClrStaSymb )
{
	tagPhoneStaCommon objRet;
	pthread_mutex_lock( &m_crisecSta );
	objRet = m_objPhoneStaCommon;
	_ClearPhoneStaCommon( m_objPhoneStaCommon, v_dwToClrStaSymb );
	pthread_mutex_unlock( &m_crisecSta );

	return objRet;
}

//函数功能：	手机模块状态获取
//线程安全：	是
//调用方式：	
void CPhoneGsm::_PhoneStaCommonSet( const tagPhoneStaCommon& v_objPhoneStaCommon, DWORD v_dwToSetStaSymb )
{
	pthread_mutex_lock( &m_crisecSta );

	if( v_dwToSetStaSymb & PHSTVAR_IMEI )
	{
		memcpy( m_objPhoneStaCommon.m_szGsmIMEI, v_objPhoneStaCommon.m_szGsmIMEI, sizeof(m_objPhoneStaCommon.m_szGsmIMEI) );
	}

	if( v_dwToSetStaSymb & PHSTVAR_SING )
	{
		m_objPhoneStaCommon.m_iSignal = v_objPhoneStaCommon.m_iSignal;
	}

	if( v_dwToSetStaSymb & PHSTVAR_ALERT )
	{
		m_objPhoneStaCommon.m_dwAlertBegin = v_objPhoneStaCommon.m_dwAlertBegin;
		m_objPhoneStaCommon.m_dwAlertPrid = v_objPhoneStaCommon.m_dwAlertPrid;
	}

	if( v_dwToSetStaSymb & PHSTVAR_NET )
	{
		m_objPhoneStaCommon.m_bNetConnected = v_objPhoneStaCommon.m_bNetConnected;
	}

	if( v_dwToSetStaSymb & PHSTVAR_PING )
	{
		m_objPhoneStaCommon.m_bytPingSrc |= v_objPhoneStaCommon.m_bytPingSrc;
	}

	if( v_dwToSetStaSymb & PHSTVAR_AUDIOMODE )
	{
		m_objPhoneStaCommon.m_bytCurAudioMode = v_objPhoneStaCommon.m_bytCurAudioMode;
	}

	if( v_dwToSetStaSymb & PHSTVAR_AUDIOOUTLVL )
	{
		m_objPhoneStaCommon.m_iAudioOutVol = v_objPhoneStaCommon.m_iAudioOutVol;
	}

	if( v_dwToSetStaSymb & PHSTVAR_NETTECH )
	{
		m_objPhoneStaCommon.m_iNetTech = v_objPhoneStaCommon.m_iNetTech;
	}

	pthread_mutex_unlock( &m_crisecSta );
}

//函数功能：	手机模块状态清除
//线程安全：	是
//调用方式：	
void CPhoneGsm::_PhoneStaCommonClr(DWORD v_dwToClrStaSymb)
{
	pthread_mutex_lock( &m_crisecSta );

	_ClearPhoneStaCommon( m_objPhoneStaCommon, v_dwToClrStaSymb );

	pthread_mutex_unlock( &m_crisecSta );
}

//函数功能：	手机模块状态清除
//线程安全：	是
//调用方式：	调用方成员变量做互斥
void CPhoneGsm::_ClearPhoneStaCommon(tagPhoneStaCommon& v_objPhoneStaCommon, DWORD v_dwToClrStaSymb)
{
	if( v_dwToClrStaSymb & PHSTVAR_IMEI )
	{
		memset( v_objPhoneStaCommon.m_szGsmIMEI, 0, sizeof(v_objPhoneStaCommon.m_szGsmIMEI) );
	}

	if( v_dwToClrStaSymb & PHSTVAR_SING )
	{
		v_objPhoneStaCommon.m_iSignal = 0;
	}

	if( v_dwToClrStaSymb & PHSTVAR_ALERT )
	{
		v_objPhoneStaCommon.m_dwAlertBegin = 0;
		v_objPhoneStaCommon.m_dwAlertPrid = 0;
	}

	if( v_dwToClrStaSymb & PHSTVAR_NET )
	{
		v_objPhoneStaCommon.m_bNetConnected = false;
	}

	if( v_dwToClrStaSymb & PHSTVAR_PING )
	{
		v_objPhoneStaCommon.m_bytPingSrc = 0;
	}

	if( v_dwToClrStaSymb & PHSTVAR_AUDIOMODE )
	{
		v_objPhoneStaCommon.m_bytCurAudioMode = 0;
	}

	if( v_dwToClrStaSymb & PHSTVAR_AUDIOOUTLVL )
	{
		v_objPhoneStaCommon.m_iAudioOutVol = 4;
	}

	if( v_dwToClrStaSymb & PHSTVAR_NETTECH )
	{
		v_objPhoneStaCommon.m_iNetTech = -1;
	}
}

//函数功能：	手机模块短信
//线程安全：	否
//调用方式：	状态机线程调用
void CPhoneGsm::_PhoneJugSmsSnd()
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

		bytSndTimes = bytSymb >> 4;
		bytSrcType = bytSymb & 0x0f;

		memset( &objSmPar, 0, sizeof(objSmPar) );
		memcpy( objSmPar.tpa, szSndTel, min(sizeof(objSmPar.tpa), sizeof(szSndTel)) );
		memcpy( objSmPar.ud, szSndData, min(sizeof(objSmPar.ud), dwLen) );
		iResult = _PhoneSmsSnd( objSmPar );
		if( ATANS_OK != iResult )
		{
			//show_msg( "Send SM fail" );

			bytSndTimes ++;
			if( bytSndTimes < 2 ) // 发送失败次数未达2次,存回待重发
			{
				bytSymb = bytSndTimes << 4 | bytSrcType;
				m_objDataMngSmsSnd.SaveBackData( bytLvl, dwLen, szSndData, bytSymb, szSndTel );
			}
			else
			{
				PRTMSG(MSG_ERR,"Sms Snd Fail 2 times, Discard!");

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
			//show_msg( "Send SM Succ" );

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
	}
}

//函数功能：	手机模块短信中心号
//线程安全：	否
//调用方式：	状态机线程调用
void CPhoneGsm::_PhoneJugSmsCentSet()
{
	tagPhoneTask objPhoneTask = _PhoneTaskGet( PHTASK_SETSMSCENT );
	if( objPhoneTask.m_dwTaskSymb & PHTASK_SETSMSCENT )
	{
		tag1PComuCfg cfg;
		GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );

		if(strcmp(cfg.m_szSmsCentID, "")!=0)
		{
			// 先查询SIM卡内的短信中心号
			char szSmsCentID[100] = {0};
			_SetAT(5000, 2, szSmsCentID, sizeof(szSmsCentID), G_CSCA_Query_res, 0, "AT+CSCA?");

			if( strcmp(cfg.m_szSmsCentID, szSmsCentID) )
			{
				_SetAT(6000, 2, NULL, 0, NULL, 0, "AT+CSCA=\"%.21s\"", cfg.m_szSmsCentID);
			}
			else
			{
				PRTMSG(MSG_DBG,"SmsCent in cfg is equal to in SimCard !");
			}
		}
		else
		{
			PRTMSG(MSG_DBG,"SmsCent in cfg is empty!");
		}
	}
}

void CPhoneGsm::_PhoneJugSmsPingRpt()
{
	tagPhoneTask objPhoneTask = _PhoneTaskGet( PHTASK_PINGRPT );
	if( objPhoneTask.m_dwTaskSymb & PHTASK_PINGRPT )
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

//函数功能：	手机模块任务
//线程安全：	是
//调用方式：	
void CPhoneGsm::_PhoneJugAlerm()
{
	tagPhoneTask objPhoneTask = _PhoneTaskGet(PHTASK_ALERM);
	if( objPhoneTask.m_dwTaskSymb & PHTASK_ALERM )
	{
		tagPhoneStaCommon objPhoneSta;
		objPhoneSta.m_dwAlertBegin = ::GetTickCount() - 1;
		objPhoneSta.m_dwAlertPrid = objPhoneTask.m_dwAlertPrid;
	}
}

//函数功能：	手机模块任务
//线程安全：	是
//调用方式：	
BOOL CPhoneGsm::_PhoneJugReset()
{
	tagPhoneTask objPhoneTask = _PhoneTaskGet(PHTASK_RESET);
	return m_bToClosePhone ? TRUE: (objPhoneTask.m_dwTaskSymb & PHTASK_RESET ? TRUE : FALSE);
}

//函数功能：	手机模块任务
//线程安全：	是
//调用方式：	
BOOL CPhoneGsm::_PhoneJugCutNet()
{
	tagPhoneTask objPhoneTask = _PhoneTaskGet(PHTASK_CUTNET);
	return objPhoneTask.m_dwTaskSymb & PHTASK_CUTNET ? TRUE : FALSE;
}

//函数功能：	手机模块信号
//线程安全：	否
//调用方式：	状态机线程调用
int CPhoneGsm::_PhoneSingleGet( int *v_p_iSignal )
{
	char szResult[80] = { 0 };
	int iSignal = -1;
	tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	int iRet = _SetAT(2000, 3, szResult, sizeof(szResult), G_CSQ_Jugres, 0, "AT+CSQ");

	if( ATANS_OK == iRet )
	{
		iSignal = atoi( szResult );
		objPhoneSta.m_iSignal = iSignal;
		_PhoneStaCommonSet(objPhoneSta, PHSTVAR_SING);	
	}
	else
	{
		PRTMSG(MSG_ERR, "Get Single Fail!" );
	}

	if( v_p_iSignal ) 
		*v_p_iSignal = iSignal;

	//PRTMSG(MSG_DBG,"Signal:%d\n", iSignal);

	FrmF3 frmf3;	
	frmf3.reply = iSignal < 100 ? PHONE_BENQ : PHONE_DTTD;
	DataPush((char*)&frmf3, sizeof(frmf3), DEV_PHONE, DEV_QIAN, 2);
	
	return iRet;
}

//函数功能：	手机模块短信
//线程安全：	否
//调用方式：	状态机线程调用
int CPhoneGsm::_PhoneSmsSnd( SM_PARAM& v_objSmPar )
{
	int iRet = 0;
	char pdu[512] = {0};

	int len1 = encode_pdu(&v_objSmPar, pdu);	//根据PDU参数，编码PDU串
	if( len1 + 5 < sizeof(pdu) )
	{
		strcpy( pdu + len1, "\x1a");				//以Ctrl-Z结束
	}
	
	byte len2 = 0;
	string_to_bytes(pdu, &len2, 2);		//取PDU串中的SMSC信息长度
	len2++;								//加上长度字节本身

	// 命令中的长度，不包括SMSC信息长度，以数据字节计
	int iSmsLen = len1/2 - len2;
	if( iSmsLen <= 0 )
	{
		return -1;
	}
	iRet = _SetAT( 20000, 1, NULL, 0, G_CMGS_JugRes, 0, "AT+CMGS=%d", iSmsLen );

// 	PRTMSG(MSG_DBG, "pdu data:");
// 	PrintString(pdu, strlen(pdu));
	if( ATANS_OK == iRet )
	{
		iRet = _SetAT( 8000, 1, NULL, 0, NULL, DWORD(strlen(pdu)), pdu );
		// PRTMSG(MSG_DBG, "pdu data222:");
		// PrintString(pdu, strlen(pdu));
	}

	if( ATANS_OK == iRet )
	{
		PRTMSG(MSG_NOR,"Send SM succ!");
	}
	else
	{
		PRTMSG(MSG_ERR,"Send SM fail!");
	}

	return iRet;
}

//函数功能：	手机模块手机号码类型判断
//线程安全：	是
//调用方式：	
#if 0 == USE_TELLIMIT
int CPhoneGsm::JugTelType( char* v_szTel, unsigned char v_ucTelLen, unsigned char* v_p_cTelType )
{
	//::OutputDebugStringW( L"0\n" );

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

//	{
//		wchar_t buf[100];
//		wsprintf( buf, L"LsnTelCount: %d\n", objSecCfg.m_bytListenTelCount );
//		::OutputDebugStringW( buf );
//
//		for( BYTE byt = 0; byt < objSecCfg.m_bytListenTelCount; byt ++ )
//		{
//			memcpy( szCompTel, objSecCfg.m_aryLsnHandtel[ byt ], 15 );
//			pos = strchr( szCompTel, 0x20 );
//			if( pos ) *pos = 0;
//
//			trace( szCompTel );
//		}
//	}

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
//		::OutputDebugStringW( L"3\n" );

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


//函数功能：	手机模块手机号码类型判断
//线程安全：	是
//调用方式：	
#if 2 == USE_TELLIMIT
int CPhoneGsm::JugTelType2( char* v_szTel, unsigned char v_ucTelLen, unsigned char* v_p_cTelType,
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


//函数功能：	手机模块手机号码类型判断
//线程安全：	是
//调用方式：	
#if 1==USE_TELLIMIT
//--------------------------------------------------------------------------------------------------------------------------
// 根据电话本判断电话类型
int CPhoneGsm::JugTelType3(char* v_szTel, unsigned char v_ucTelLen, unsigned char* v_p_cTelType,
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
// 	::GetSecCfg( &cfg2, sizeof(cfg2), offset, sizeof(cfg2) );

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


//函数功能：	手机模块手机号码类型判断
//线程安全：	是
//调用方式：	
//--------------------------------------------------------------------------------------------------------------------------
// 判断电话号码类型
// callout: true 呼出电话, false 呼入电话
// tel: 电话号码
// len: 电话号码长度
// holdtime: 返回的通话时长
// 返回: 电话号码类型 A1 A2 A3 A4 B1 B2 B3 B4
// A1 A2 A3 A4——呼出类型：0,允许呼出; 1,车台主动监听号码; 2,禁止呼出号码; 3,中心服务电话号码;
// B1 B2 B3 B4——呼入类型：0,允许呼入; 1,中心主动监听号码; 2,禁止呼入号码; 3,中心服务电话号码; 4,UDP激活号码
byte CPhoneGsm::tel_type(bool callout, char *tel, byte len, ushort *holdtime)
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


//函数功能：	手机模块信号
//线程安全：	否
//调用方式：	状态机线程调用
// 设置音频模式 
// mode: 音频模式	0 免提
//					1 耳机
//					2 监听	
// 返回: 设置是否成功
int CPhoneGsm::_PhoneAudioModeChg(int v_iChgToMode)
{
	DWORD dwIoSetSymb = DEV_PHONE;
	int iResult = -1;

	tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	
	switch( v_iChgToMode )
	{
	case AUDIOMODE_HANDFREE: //免提模式
		{
			// vol 增益
			if(ATANS_OK!=_SetAT( 600, 3, NULL, 0, NULL, 0, "AT$AUVOL=%d", objPhoneSta.m_iAudioOutVol))
			{
				PRTMSG(MSG_DBG, "set AT$AUVOL Fail\n");
				return -1;
			}
			//mic 增益	
			if(ATANS_OK!=_SetAT( 600, 3, NULL, 0, NULL, 0, "AT$AUGAIN=1,8" ))
			{
				PRTMSG(MSG_DBG, "set AT$AUGAIN Fail\n");
				return -1;
			}

			if( m_bM33a )
			{
				if(ATANS_OK!=_SetAT( 600, 3, NULL, 0, NULL, 0, "AT$SP=0" ))
				{
					PRTMSG(MSG_DBG, "set AT SP Fail\n");
				}
				if(ATANS_OK!=_SetAT( 600, 3, NULL, 0, NULL, 0, "AT$SG=3,0" ))//消側音
				{
					PRTMSG(MSG_DBG, "set AT SP Fail\n");
				}
				if(ATANS_OK!=_SetAT( 600, 3, NULL, 0, NULL, 0, "AT$SG=1,9" ))//增大音量
				{
					PRTMSG(MSG_DBG, "set AT SP Fail\n");
				}
				if(ATANS_OK!=_SetAT( 600, 3, NULL, 0, NULL, 0, "AT$SG=2,%d", objPhoneSta.m_iAudioOutVol))
				{
					PRTMSG(MSG_DBG, "set AT SP Fail\n");
				}
				if(ATANS_OK!=_SetAT( 600, 3, NULL, 0, NULL, 0, "AT$SQC=\"AGCU\",\"1 0001 00a0 13f0 7e7e 7fb5 59fa 7eb8 7333 7f5c 7333 0000\"" ))
				{
					PRTMSG(MSG_DBG, "set AT SP Fail\n");
				}
			}

			iResult = _PhoneAudioOutVolChg(0);
			if( iResult ) return -1;

			PRTMSG(MSG_DBG,"Audio Mode switch to handfree!");
			
			IOSet(IOS_EARPHONESEL, IO_EARPHONESEL_OFF, &dwIoSetSymb, sizeof(dwIoSetSymb));
			IOSet(IOS_AUDIOSEL, IO_AUDIOSEL_PHONE, NULL, 0);
			IOSet(IOS_TRUMPPOW, IO_TRUMPPOW_ON, &dwIoSetSymb, sizeof(dwIoSetSymb));

// 硬件上不支持对手机模块自身的双通道切换,而是通过手机模块的外围电路通道切换实现的
// 			int iResult = _SetAT( 5000, 3, NULL, 0, NULL, 0, "AT^DAUDO=2,5" );
// 			if( ATANS_OK != iResult )
// 			{
// 				return -1;
// 			}
// 
// 			iResult = _SetAT( 5000, 3, NULL, 0, NULL, 0, "AT^DAUDI=1,1" );
// 			if( ATANS_OK != iResult )
// 			{
// 				return -1;
// 			}

// 		if(!_SetAT(5, 1, 0, "AT$AUVOL=%d\r\n", m_volume))		return false;	// vol 增益
// 		if(!_SetAT(5, 1, 0, "AT$AUGAIN=1,8\r\n"))		return false;	//mic 增益	
		}
		break;

	case AUDIOMODE_EARPHONE: //耳机模式
		{
			{	//耳机模式
				if(ATANS_OK!=_SetAT( 5000, 3, NULL, 0, NULL, 0, "AT$AUVOL=%d", objPhoneSta.m_iAudioOutVol))// vol 增益
				{
					PRTMSG(MSG_DBG, "set AT$AUVOL fail\n");
				}
				if(ATANS_OK!=_SetAT( 5000, 3, NULL, 0, NULL, 0, "AT$AUGAIN=1,5" ))//mic 增益
				{
					PRTMSG(MSG_DBG, "set AT$AUVOL fail\n");
				}		
				if( m_bM33a )
				{
					if(ATANS_OK!=_SetAT( 5000, 3, NULL, 0, NULL, 0, "AT$SG=2,%d", objPhoneSta.m_iAudioOutVol))//mic 增益
					{
						PRTMSG(MSG_DBG, "set AT$SG fail\n");
					}	
				}				
			} 

			iResult = _PhoneAudioOutVolChg(0);
			if( iResult ) return -1;

			PRTMSG(MSG_DBG,"Audio Mode switch to earphone!\n");

			IOSet(IOS_EARPHONESEL, IO_EARPHONESEL_ON, &dwIoSetSymb, sizeof(dwIoSetSymb));
			IOSet(IOS_AUDIOSEL, IO_AUDIOSEL_PHONE, NULL, 0);
			IOSet(IOS_TRUMPPOW, IO_TRUMPPOW_ON, &dwIoSetSymb, sizeof(dwIoSetSymb));

// 硬件上不支持对手机模块自身的双通道切换,而是通过手机模块的外围电路通道切换实现的
// 			int iResult = _SetAT( 5000, 3, NULL, 0, NULL, 0, "AT^DAUDO=3,5" );
// 			if( ATANS_OK != iResult )
// 			{
// 				return -1;
// 			}
// 
// 			iResult = _SetAT( 5000, 3, NULL, 0, NULL, 0, "AT^DAUDI=2,1" );
// 			if( ATANS_OK != iResult )
// 			{
// 				return -1;
// 			}

// 			if(!_SetAT(5, 1, 0, "AT$AUVOL=%d\r\n", m_volume))		return false;	// vol 增益
// 			if(!_SetAT(5, 1, 0, "AT$AUGAIN=1,5\r\n"))		return false;	//mic 增益	
		}
		break;

	case AUDIOMODE_LSN: //监听模式
		{
			PRTMSG(MSG_DBG,"Audio Mode switch to lsn!\n");

			IOSet(IOS_TRUMPPOW, IO_TRUMPPOW_OFF, &dwIoSetSymb, sizeof(dwIoSetSymb));
			IOSet(IOS_AUDIOSEL, IO_AUDIOSEL_PHONE, NULL, 0);
			IOSet(IOS_EARPHONESEL, IO_EARPHONESEL_OFF, &dwIoSetSymb, sizeof(dwIoSetSymb));

// 硬件上不支持对手机模块自身的双通道切换,而是通过手机模块的外围电路通道切换实现的
// 			int iResult = _SetAT( 5000, 3, NULL, 0, NULL, 0, "AT^DAUDO=2,5" );
// 			if( ATANS_OK != iResult )
// 			{
// 				return -1;
// 			}
// 
// 			iResult = _SetAT( 5000, 3, NULL, 0, NULL, 0, "AT^DAUDI=1,2" );
// 			if( ATANS_OK != iResult )
// 			{
// 				return -1;
// 			}

// 		if(!_SetAT(5, 1, 0, "AT$AUVOL=0\r\n"))			return false;	// vol 增益
// 		if(!_SetAT(5, 1, 0, "AT$AUGAIN=1,9\r\n"))		return false;	// mic 增益
		}
		break;

	case AUDIOMODE_DIAL: // 拨号模式(非监听拨号)
		{
			PRTMSG(MSG_DBG,"Audio Mode switch to dial!\n");

			// (耳机还是免提并不指定)

			IOSet(IOS_AUDIOSEL, IO_AUDIOSEL_PHONE, NULL, 0);
			IOSet(IOS_TRUMPPOW, IO_TRUMPPOW_ON, &dwIoSetSymb, sizeof(dwIoSetSymb));
		}
		break;

	case AUDIOMODE_CLOSE: // 通话关闭模式
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

	tagPhoneStaCommon objSta = _PhoneStaCommonGet(0);
	objSta.m_bytCurAudioMode = v_iChgToMode;
	_PhoneStaCommonSet( objSta, PHSTVAR_AUDIOMODE);

	return 0;
}

//函数功能：	手机模块信号
//线程安全：	否
//调用方式：	状态机线程调用
int CPhoneGsm::_PhoneAudioOutVolChg(int v_iChgDegree)
{
	tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
	
	objPhoneSta.m_iAudioOutVol += v_iChgDegree;
	if( objPhoneSta.m_iAudioOutVol < 1 ) objPhoneSta.m_iAudioOutVol = 1;
	if( objPhoneSta.m_iAudioOutVol > 5 ) objPhoneSta.m_iAudioOutVol = 5;

	int iResult = _SetAT( 2500, 2, NULL, 0, NULL, 0, "AT$AUVOL=%d", objPhoneSta.m_iAudioOutVol );

	if( ATANS_OK == iResult )
	{
		_PhoneStaCommonSet( objPhoneSta, PHSTVAR_AUDIOOUTLVL );
		return 0;
	}
	else
	{
		return -1;
	}
}


//函数功能：	手机模块短信
//线程安全：	是
//调用方式：	
// 发送短信(将待发送的短信压入发送队列)
// to: 目的手机号字符串指针
// tolen: 目的手机号字符串长度
// msg: 待发短信内容指针
// msglen: 待发短信内容长度
// 返回: 1 成功
//		 2 失败
//		 3 短信内容太长
int CPhoneGsm::_PushSndSms(char *to, byte tolen, char *msg, byte msglen, BYTE v_bytSrcType)
{
	// 判断读入的字符串长度
//	if(m_simlack)								{ return 2; }
	if(tolen==0 || tolen>15 || msglen==0)		{ return 2;	}
	if(msglen>160)								{ return 3; }
	
// 	// 判断是否是UNICODE码
// 	bool unicode = false;
// 	for(int i=0; i<msglen; i++)
// 	{
// 		if((uchar)msg[i] > 0x80)	  { unicode=true;  break; }
// 	}
// 
// 	// 如果是UNICODE,检查是否超过70个字符
// 	if(unicode)
// 	{
// 		ushort wstr[180] = { 0 };
// 		int dstlen = ::MultiByteToWideChar(CP_CHS, 0, msg, msglen, wstr, sizeof(wstr) / sizeof(wstr[0]) );
// 		if(dstlen>70)	return 3;	//如果unicode字符超过70个，则返回
// 	}

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
	int iResult = m_objDataMngSmsSnd.PushData(5, msglen, msg, dwPktNum, v_bytSrcType, tel, strlen(tel) );

	return iResult ? 2 : 1;

// 	uchar tomsg[16+161] = {0};
// 	memcpy(tomsg, tel, 16);
// 	memcpy(tomsg+16, msg, min(msglen,161));
// 	if(m_stack.push(tomsg, 16+161))	return 1;
// 	else							return 2;
}

//函数功能：	手机模块短信
//线程安全：	是
//调用方式：	
// 处理短信的函数
// psm: 短信的结构指针
bool CPhoneGsm::sms_deal(SM_PARAM *psm)
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
			PRTMSG(MSG_NOR,"Rcv Liu SM!");
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
			PRTMSG(MSG_NOR,"Rcv Qian SM!!");
			buf[0] = 0x37;	//数据类型
			DataPush(buf, buflen+1, DEV_PHONE, DEV_QIAN, 2);	//发往千里眼
			return true;
		}
	}

	// 检查是否是普通配置短信(通过手机直接配置的短信)
	UserSM *pusm = (UserSM*)psm->ud;
#if USE_LIAONING_SANQI == 0
	if( pusm->add1=='+' && pusm->add2=='+' && pusm->add3=='+' && pusm->add4=='+' && pusm->add5=='+' && pusm->add6=='+' && pusm->add7=='+'
		&& pusm->add8 =='+' && pusm->add9=='+' && pusm->add10=='+' && pusm->add11=='+' ) 
#endif
#if USE_LIAONING_SANQI == 1
	if( pusm->add1=='+' && pusm->add2=='+' && pusm->add3=='+' && pusm->add4=='+' && pusm->add5=='+' && pusm->add6=='+' && pusm->add7=='+'
		&& pusm->add8 =='+' && pusm->add9=='+' && pusm->add10=='+' && pusm->add11=='+' && NULL != strchr(pusm->apn, ';'))	
#endif
	{
		PRTMSG(MSG_DBG, "Rcv Cfg SM!\n");

		tag1PComuCfg cfg;
		GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
		tag1QIpCfg cfg2[2];
		GetImpCfg( &cfg2, sizeof(cfg2), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg2) );
		tag1LComuCfg cfg3;
		GetImpCfg( &cfg3, sizeof(cfg3), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg3) );

#if USE_LIAONING_SANQI == 1
		tag2QcgvCfg cgvcfg;
		GetSecCfg(&cgvcfg, sizeof(cgvcfg), offsetof(tagSecondCfg, m_uni2QcgvCfg.m_obj2QcgvCfg), sizeof(cgvcfg));
#endif
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
		
#if USE_LIAONING_SANQI == 0			
		// 填充APN
		memset(cfg2[0].m_szApn, 0, sizeof(cfg2[0].m_szApn) );
		memcpy(cfg2[0].m_szApn, pusm->apn, strlen(pusm->apn));
		
#endif				
#if USE_LIAONING_SANQI == 1		
		byte apnl = strchr(pusm->apn, ';') - pusm->apn;
		if( apnl <= sizeof(cfg2[0].m_szApn) )
		{
			// 填充APN
			memset(cfg2[0].m_szApn, 0, sizeof(cfg2[0].m_szApn) );
			memcpy(cfg2[0].m_szApn, pusm->apn, apnl);
		}
	
		memset(cgvcfg.m_groupid, 0, sizeof(cgvcfg.m_groupid));
		memcpy(cgvcfg.m_groupid, pusm->apn + apnl + 1, strlen(pusm->apn + apnl + 1));
		cgvcfg.m_groupidlen = strlen(pusm->apn + apnl + 1);
#endif					
		// 保存配置
		bool succ = true;
		if(SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) ))		succ = false;
		if(SetImpCfg(&cfg2, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg2) ))		succ = false;
		if(SetImpCfg(&cfg3, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg3) ))		succ = false;
#if USE_LIAONING_SANQI == 1	
		if( SetSecCfg(&cgvcfg, offsetof(tagSecondCfg, m_uni2QcgvCfg.m_obj2QcgvCfg), sizeof(cgvcfg) )) succ = false;
#endif			
		
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

#if USE_LIAONING_SANQI == 1	
		sprintf(msg+strlen(msg), " GID:%s", cgvcfg.m_groupid);
#endif
		_PushSndSms( psm->tpa, strlen(psm->tpa), msg, min(strlen(msg), 160), 0 );

		// 添加GSM信号、GPRS在线情况、ACC状态、GPS定位情况
		memset( msg, 0, sizeof(msg) );
		tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
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
		tagPhoneStaCommon objPhoneSta = _PhoneStaCommonGet(0);
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
	PRTMSG(MSG_DBG,"User's SM!");
	char frm[1024] = {0};
	int p = 0;
	DWORD dwPktNum;
	frm[p] = 0x33;					p++;		//接收短消息指示
	frm[p] = len1;					p++;		//主叫手机号码长度
	memcpy(frm+p, psm->tpa, len1);	p+=len1;	//主叫手机号码
	memcpy(frm+p, psm->scts, 12);	p+=12;		//接收时间
	frm[p] = len2;					p++;		//短消息内容长度
	memcpy(frm+p, psm->ud, len2);	p+=len2;	//短消息内容
	g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, p, frm, dwPktNum);	//发往调度屏

	return true;
}


//函数功能：	手机模块拨号上网
//线程安全：	否
//调用方式：	状态机线程调用
// GPRS拨号函数
// 返回: 拨号是否成功
int CPhoneGsm::_PhoneDialNet()		
{
	if (SIG_ERR == G_Signal(SIGUSR1, G_PppSigHandler) )
	{
		PRTMSG(MSG_ERR,"failed to set the SIGUSR1 handler function for ppp dial\n");
		return -1;
	}

	if (0 != _ForkPppd() )
	{
		return ERR_THREAD;
	}

	//	que 此处需加ppp拨号完成情况的判断,但拨号时间太长，会不会占用PST_DIALNET状态太久，影响idle状态的检测
	return 0;
}

//函数功能：	手机模块拨号上网
//线程安全：	否
//调用方式：	注意调用前资源的释放
//创建拨号进程pppd
int CPhoneGsm::_ForkPppd()
{
	int iRet;
	pid_t pidPppd;
	char szUsbSerialPathName[256] = {0};
	DWORD pathSymb;

	pathSymb = COMMUX_DEVPATH_PPP;
	if(false == g_objComVir.GetComMuxPath(pathSymb, szUsbSerialPathName, sizeof(szUsbSerialPathName)))
	{
#if CHK_VER == 0
		g_bProgExit = true;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		g_objDiaodu.show_diaodu("手机模块异常");
		RenewMemInfo(0x02, 0x00, 0x00, 0x01);
#endif


		return ERR_COM;
	}

	PRTMSG(MSG_DBG, "ppp use mux path:%s",szUsbSerialPathName);

	pidPppd = fork();
	switch(pidPppd)
	{
	case -1:
		PRTMSG(MSG_ERR,"failed to fork a new process\n");
		return ERR_THREAD;
		
	case 0:
		iRet = execl("/etc/ppp/ppp-on", "ppp-on",szUsbSerialPathName, PPP_DIAL_NUM, PPP_USER_NAME, PPP_PASSWORD, NULL);		
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

//函数功能：	手机模块拨号上网信号
//线程安全：	否
//调用方式：	在信号处理函数调用
int CPhoneGsm::_PppSigHandler()
{
	m_bPhoneNetReady = true;
}

//函数功能：	手机模块断开网络
//线程安全：	否
//调用方式：	状态机线程中调用
void CPhoneGsm::_PhoneCutNet()
{	
	m_bPhoneNetReady = false;
	PRTMSG(MSG_NOR,"Cut Net...\n");
	system("killall pppd");	//kill pppd拨号进程，可能重复操作
	pid_t pid = waitpid(-1,NULL,0); // WNOHANG
	if (pid>0)
	{
		PRTMSG(MSG_NOR, "pppd waitpid:%d\n",pid);
	}
	sleep(10);
}

//函数功能：	数据转换
//线程安全：	是
//调用方式：	
//-----------------------------------------------------------------------------------------------------------------
// 接收千里眼中心的短信需要进行6转7(SMS)
// sptr: 源字符串指针
// dptr: 目的字符串指针
// len: 源字符串长度
// 返回: 目的字符串长度
int CPhoneGsm::convert6to7(char *sptr, char *dptr, int len)
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

//函数功能：	数据转换
//线程安全：	是
//调用方式：	
//-----------------------------------------------------------------------------------------------------------------
//接收流媒体中心的短信需要进行6转8(SMS)
// strOldBuf: 源字符串指针
// strNewBuf: 目的字符串指针
// iOldLen: 源字符串长度
// 返回: 目的字符串长度
int CPhoneGsm::convert6to8(const char* strOldBuf,char* strNewBuf,const int iOldLen)
{
	int		i=0;	//待转化的字符计数器
	int		k=0;	//已转化的字符下标
	int		l=8;	//要需要的比特数
	int		iNewLen;
	byte	strOldBuf1[500];

	if((ulong)iOldLen>500)	return 0;

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

//函数功能：	crc校验
//线程安全：	是
//调用方式：	
// 得到校验和(查表)
// buf: 字符串指针
// len: 字符串长度
// 返回: 校验和
byte CPhoneGsm::get_crc1(const byte *buf, const int len) 
{   
	int	j = 0;
	byte sum = 0xff;

	for( int i=len; i>0; i-- ) {
		sum = CrcTable[sum ^ buf[j++]];
	}
	return (0xff-sum);
}

//函数功能：	crc校验
//线程安全：	是
//调用方式：	
// 得到校验和(直接取累加和)
// buf: 字符串指针
// len: 字符串长度
// 返回: 校验和
byte CPhoneGsm::get_crc2(const byte *buf, const int len)
{   
	return get_crc_sms( buf, len );
}

//函数功能：	crc校验
//线程安全：	是
//调用方式：	
// 检查校验和(查表)
// buf: 待校验字符串指针
// len: 待校验字符串长度
// 返回: 校验和是否正确
bool CPhoneGsm::check_crc1(const byte *buf, const int len) 
{   
	byte sum = get_crc1( buf+1, len-1 );
	if( buf[0]==sum )
		return true;
	else 
		return false;
}

//函数功能：	crc校验
//线程安全：	是
//调用方式：	
// 检查校验和(直接取累加和)
// buf: 待校验字符串指针
// len: 待校验字符串长度
// 返回: 校验和是否正确
bool CPhoneGsm::check_crc2(const byte *buf, const int len)
{   
	byte sum = get_crc2( buf, len-1 );
	if( buf[len-1]==sum )		
		return true;
	else						
		return false;
}

// 此函数在生产检测版本中会被调用到，用于在断电时判断手机模块是否正常
bool CPhoneGsm::IsPhoneValid()
{
	// AT测试
	if( ATANS_OK != _SetAT(500, 5, 0, 100, NULL, 0, NULL, 0, "AT") )
		return false;
	else
		return true;
}

// bool CPhoneGsm::Set_AudioMode()
// {
// 	bool bRet = false;
// #if  USE_IRD ==0
// 	//满足手柄的接听需求
// 	if(1==g_iTerminalType)
// 	{	
// 		tag2QHstCfg obj2QHstCfg;
// 		GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg),
// 			offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );
// 		m_telmode = obj2QHstCfg.m_HandsfreeChannel;
// 		m_telmodebk = obj2QHstCfg.m_HandsfreeChannel;
// 		if (!m_telmode)
// 		{	//耳机
// 			// 设置耳机模式
// 			bRet = set_audio_mode(1);
// 			if(!bRet)		
// 			{ 
// 				PRTMSG(MSG_ERR, "Set_AudioMode(1) Fail!\n"); 
// 			}
// 		}
// 		else
// 		{	//免提
// 			// 设置免提模式
// 			bRet = set_audio_mode(0);
// 			if(!bRet)		
// 			{
// 				PRTMSG(MSG_ERR, "Set_AudioMode(0) Fail!\n");
// 			}
// 		}
// 	}
// 	else
// 	{
// 		bRet = set_audio_mode(0);
// 		if(!bRet)	
// 		{
// 			PRTMSG(MSG_ERR, "Set_AudioMode(2)(0) Fail!\n");
// 		}	
// 	}
// #else
// 	bRet = set_audio_mode(0);
// 	if(!bRet)	
// 	{
// 		PRTMSG(MSG_ERR, "Set_AudioMode(2)(0) Fail!\n");
// 	}	
// #endif
// 	return bRet;
// }


