#ifndef _LN_COMMONFUNC_H_
#define _LN_COMMONFUNC_H_

struct AKV_t 
{
	byte Attr;
	byte KeyLen;
	char Key[20];
	ushort Vlen;
	char *Value;
	DWORD AKVLen;
	AKV_t()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct AKV_T_t
{
	bool	m_bTimeEnable[MAX_TIME_COUNT];		// 时间段是否有效（最多允许设置5个时间段）
	char	m_szBeginTime[MAX_TIME_COUNT][3];	// 时间段范围
	char	m_szEndTime[MAX_TIME_COUNT][3];
	AKV_T_t()
	{
		memset(this, 0, sizeof(*this));
	}
};
int AKV_T(AKV_t *v_akv, AKV_T_t *akv_T, char *DebugMsg);
	
struct AKV_rS_t	
{
	long	m_lMaxLat;							// 区域范围（现在只支持矩形区域）
	long	m_lMinLat;
	long	m_lMaxLon;
	long	m_lMinLon;
	AKV_rS_t()
	{
		memset(this, 0, sizeof(*this));
	}
};

int AKV_rS(AKV_t *v_akv, AKV_rS_t *akv_rS, byte regiontype, char *DebugMsg);

struct AKV_rPT_t
{
	bool	m_bPointEnable[MAX_POINT_COUNT];
	long	m_lLat[MAX_POINT_COUNT];							
	long	m_lLon[MAX_POINT_COUNT];
	AKV_rPT_t()
	{
		memset(this, 0, sizeof(*this));
	}
};

int AKV_rPT(AKV_t *v_akv, AKV_rPT_t *akv_rPT, char *DebugMsg);

struct AKV_ibe_t
{
	ushort		m_usInterval; // 拍照间隔
	byte	m_bytCTakeBeginTime[6]; //抓拍起始
	byte    m_bytCTakeEndTime[6];
	AKV_ibe_t()
	{
		memset(this, 0, sizeof(*this));
	}
};

int AKV_ibe(AKV_t *v_akv, AKV_ibe_t *akv_ibe, char *DebugMsg);

struct AKV_bo_t
{
	byte	m_bytTTakeBeginTime[6]; //抓拍起始
	byte    m_bytTTakeEndTime[6];

	byte	m_bytTTakeBaseTime; //疲劳抓拍起始
	byte    m_bytTTakeOfTime;
	AKV_bo_t()
	{
		memset(this, 0, sizeof(*this));
	}
};

int AKV_bo(AKV_t *v_akv, AKV_bo_t *akv_bo, char *DebugMsg);

int Get_ulong(AKV_t *v_akv, ulong *Value, char *DebugMsg);
int Get_ushort(AKV_t *v_akv, ushort *Value, char *DebugMsg);
int Get_char(AKV_t *v_akv, byte *Value, char *DebugMsg);

int G_LocalToNet(char* v_szSrcBuf, char *v_szDesBuf, int v_iLen);
int G_iToStr(int v_iValue, char *v_szBuf, int v_iBufLen);

int G_GetPoint(char *v_szBuf);
DWORD G_GetSecCnt();
int G_GetDEV_ID(char *v_szBuf);
short CaculateCRC(byte *msg, int len);
bool CheckCRC(byte *buf, int len);
int AKV_Jugre(char *v_szkey, AKV_t *akv, DWORD v_akvlen);
int Key_Jugres(char *v_szkey);

ushort PutAKV_char(char *v_DesBuf, char akvattr, char *Key, byte *v_bValue, ushort v_usbLen, ushort *dwTotalLen);
ushort PutAKV_ushort(char *v_DesBuf, char akvattr, char *Key, ushort v_usValue, ushort *dwTotalLen);
ushort PutAKV_ulong(char *v_DesBuf, char akvattr, char *Key, ulong v_ulValue, ushort *dwTotalLen);

bool JugPtInShape(long longitude, long latitude, long MinLon, long MinLat, long MaxLon, long MaxLat, byte AreaAttr, byte *AttrRes);

bool OutLineLen(double lon1, double lat1, double lon2, double lat2, double curlon, double curlat, double *len);
#endif


