#ifndef _SMS_H_
#define _SMS_H_

//----------------------------------------------------------------------------------------------------------------
// 用户信息编码方式
#define SMS_7BIT		0	//ASCI
#define SMS_8BIT		4
#define SMS_UCS2		8

//----------------------------------------------------------------------------------------------------------------
// 短消息参数结构，编码/解码共用
// 其中，字符串以0结尾
struct SM_PARAM {
	char sca[16];			// 短消息服务中心号码(SMSC地址)
	char tpa[16];			// 目标号码或回复号码(TP-DA或TP-RA)
	char pid;				// 用户信息协议标识(TP-PID)
	char dcs;				// 用户信息编码方式(TP-DCS)
	char scts[16];			// 服务时间戳字符串(TP_SCTS), 接收时用到
	char ud[161];			// 原始用户信息(编码前或解码后的TP-UD) 最后必须以0结尾，所以要161
	char type;				// 短消息类型
	char index;				// 短消息序号，在读取时用到
	size_t udl;				// evdo增加短消息长度

	SM_PARAM() { memset(this, 0, sizeof(*this)); }
};

//////////////////////////////////////////////////////////////////////////
static int bytes_to_string(const uchar *psrc, char *pdst, int srclen);
int string_to_bytes(const char *psrc, uchar *pdst, int srclen);
static int encode_7bit(const char *psrc, uchar *pdst, int srclen);
static int decode_7bit(const uchar *psrc, char *pdst, int srclen);
static int encode_8bit(const char *psrc, uchar *pdst, int srclen);
static int decode_8bit(const uchar *psrc, char *pdst, int srclen);
static size_t encode_ucs2(const char *psrc, uchar *pdst, size_t srclen);
static size_t decode_ucs2(const uchar *psrc, char *pdst, size_t srclen);
static int invert_numbers(const char *psrc, char *pdst, int srclen);
static int serialize_numbers(const char *psrc, char *pdst, int srclen);
bool wcdma_encode_pdu(SM_PARAM *v_pSM,char *v_pBuf,int &v_pLen);
bool wcdma_decode_pdu(const char *psrc, SM_PARAM *pdst);
bool evdo_encode_pdu(SM_PARAM *v_pSM,char *v_pBuf,int &v_pLen);
bool evdo_decode_pdu(const char *psrc, SM_PARAM *pdst);
int decode_pdu(const char *psrc, SM_PARAM *pdst);
int encode_pdu(SM_PARAM *psrc, char *pdst);

#endif	//#define _SMS_H_
