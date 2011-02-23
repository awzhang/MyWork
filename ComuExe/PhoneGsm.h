// PhoneGsm.h: interface for the CPhoneGsm class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _PHONEGSM_H_
#define _PHONEGSM_H_


enum ATANS
{
	ATANS_OK = 1,
	ATANS_ERROR,
	ATANS_CMEERR,
	ATANS_NOANS,
	ATANS_RCVNOTALL,
	// { 仅用于呼入呼出
	ATANS_NOANSWER,
	ATANS_NOCARRIER,
	ATANS_NODIALTONE,
	ATANS_BUSY,
	ATANS_NOCALL, // 不在通话中
	// } 仅用于呼入呼出
	ATANS_OTHER
};

enum PHONESTA
{
	PST_INIT = 1,
	PST_IDLE,
	PST_DIALNET,
	PST_WAITDIALNET,
	PST_CUTNET,
	PST_RESET,
	PST_DIALLSN,
	PST_WAITLSNPICK,
	PST_LSNING,
	PST_DIAL,
	PST_WAITPICK,
	PST_HOLD,
	PST_HANGUP,
	PST_RCVRING
};

// enum PINGSTA
// {
// 	PINGST_IDLE = 666,
// 	PINGST_DOING,
// 	PINGST_SUCC,
// 	PINGST_FAIL,
// 	PINGST_CANCEL
// };

enum AUDIOMODE
{
	AUDIOMODE_HANDFREE = 1, // 不要太大,有的结构体里只用1个字节保存
	AUDIOMODE_EARPHONE,
	AUDIOMODE_LSN,
	AUDIOMODE_DIAL,
	AUDIOMODE_CLOSE
};

// 任务指令定义（可或起来）
const DWORD PHTASK_DIALSN		= 0X00000001;
const DWORD PHTASK_DIAL			= 0X00000002;
const DWORD PHTASK_RING			= 0X00000004;
const DWORD PHTASK_SETSMSCENT	= 0X00000008;
const DWORD PHTASK_SNDDTMF		= 0X00000010;
const DWORD PHTASK_PICKUP		= 0X00000020;
const DWORD PHTASK_HANGUP		= 0X00000040;
const DWORD PHTASK_CHGAUDIO		= 0X00000080;
const DWORD PHTASK_CHGVOL		= 0X00000100;
const DWORD PHTASK_PING			= 0x00000200;
const DWORD PHTASK_PINGRPT		= 0x00000400;
const DWORD PHTASK_CUTNET		= 0x00000800;
const DWORD PHTASK_ALERM		= 0x00001000;
const DWORD PHTASK_RESET		= 0x00002000;

// 任务数据定义 (多线程访问,需要互斥)
struct tagPhoneTask
{
	DWORD	m_dwTaskSymb; // 取值见上面的定义

//	bool	m_bDialLsn;
	char	m_szDialLsnTel[20];

//	bool	m_bDial;
	char	m_szDialTel[20];
	DWORD	m_dwHoldLmtPrid;
	
//	bool	m_bRing;
//	char	m_szRingTel[20];

//	bool	m_bSetSmsCent;
//	char	m_szSmsCent[19];

//	bool	m_bSndDTMF;
	char	m_szDTMF[60];

//	bool	m_bSndSms;
//	bool	m_bHangUp; // 用户操作主动挂机
	BYTE	m_bytChgAudio; // AUDIOMODE_HANDFREE,切换到免提; AUDIOMODE_EARPHONE,切换到耳机
	int		m_iChgVol; // 1,增大; -1,减小

//	bool	m_bPing;
	BYTE	m_bytPingSrc; // 1,QianExe; 2,StreamMedia (或的关系 )

	BYTE	m_bytPingRptSrc;
	BYTE	m_bytPingResult; // 0,成功; 1,失败; 2,异常; 3,被取消

//	bool	m_bCutNet;

	DWORD	m_dwAlertPrid;

	tagPhoneTask() { memset(this, 0, sizeof(*this)); }
};


/// { 状态机中每一个状态对应的状态值 (仅在状态机线程中使用,无需互斥访问)
struct tagPhoneStaDial
{
	DWORD	m_dwDialBeginTm;
	DWORD	m_dwHoldLmtPrid;
	char	m_szDialTel[20];

	void Init() { memset(this, 0, sizeof(*this)); }
	tagPhoneStaDial() { Init(); }
};

struct tagPhoneStaHold
{
	DWORD	m_dwHoldBeginTm;
	DWORD	m_dwHoldLmtPrid;
	DWORD	m_dwCallChkTm;

	void Init() { memset(this, 0, sizeof(*this)); }
	tagPhoneStaHold() { Init(); }
};

struct tagPhoneStaDialLsn
{
	DWORD	m_dwDialBeginTm;
	char	m_szDialLsnTel[20];

	void Init() { memset(this, 0, sizeof(*this)); }
	tagPhoneStaDialLsn() { Init(); }
};

struct tagPhoneStaLsning
{
	DWORD	m_dwCallChkTm;

	void Init() { memset(this, 0, sizeof(*this)); }
	tagPhoneStaLsning() { Init(); }
};

struct tagPhoneStaRing
{
	DWORD	m_dwHoldLmtPrid;
	BYTE	m_bytCallInType; // 呼入类型：0,允许呼入 1,中心主动监听号码 2,禁止呼入号码 3,中心服务电话号码 4,UDP激活号码
	char	m_szRingTel[20];
	DWORD	m_dwLstChkRingTm;

	void Init()
	{
		memset(this, 0, sizeof(*this));
		m_dwLstChkRingTm = GetTickCount();
	}
	tagPhoneStaRing() { Init(); }
};
/// } 状态机中每一个状态对应的状态值


// 通用状态处理标记 (按位定义)
const DWORD PHSTVAR_IMEI		= 0X00000001;
const DWORD PHSTVAR_SING		= 0X00000002;
const DWORD PHSTVAR_ALERT		= 0X00000004;
const DWORD PHSTVAR_NET			= 0X00000008;
const DWORD PHSTVAR_PING		= 0X00000010;
const DWORD PHSTVAR_AUDIOMODE	= 0X00000020;
const DWORD PHSTVAR_AUDIOOUTLVL = 0x00000040;
const DWORD	PHSTVAR_NETTECH		= 0x00000080;

/// { 状态机中多个状态下需要使用的状态值 (可能多线程访问,需要互斥)
struct tagPhoneStaCommon
{
	char	m_szGsmIMEI[256];
	int		m_iSignal;
	int		m_iNetTech; // -1,未知; 0,GSM; 1,GSM compact; 2,TDD

	DWORD	m_dwAlertBegin;
	DWORD	m_dwAlertPrid;

	int		m_iAudioOutVol; // 通话时音量输出增益: 0~5
	BYTE	m_bytCurAudioMode;

	bool	m_bNetConnected;

	BYTE	m_bytPingSrc; // 1,QianExe; 2,StreamMedia (或的关系 )

	void Init()
	{
		memset(this, 0, sizeof(*this));
		m_iAudioOutVol = 2;
		m_iNetTech = -1;
	}

	tagPhoneStaCommon()
	{
		Init();
	}
};
/// } 状态机中多个状态下需要使用的状态值

typedef int (*PCHKATRES) (char* v_szAtRes, void* v_pResult, size_t v_sizResult );

class CPhoneGsm
{
public:
	CPhoneGsm();
	virtual ~CPhoneGsm();
	int InitPhone();
	void ReleasePhone();
	void ResetPhone();
	void ReDialNet();
	BYTE GetPhoneState();
	int GetPhoneHdlSta();
	int GetPhoneSignal();
	void GetPhoneIMEI( char* v_szImei, size_t v_sizImei );
	int GetPhoneVolume();
	int _PppSigHandler();

	bool IsPhoneValid();

protected:
	unsigned long P_PhoneHdl();
	friend unsigned long G_PhoneHdl(void* v_pPar);

	unsigned long P_ComRcv();
	friend unsigned long G_ComRcv(void* v_pPar);

	unsigned long P_OtherHdl();
	friend unsigned long G_OtherHdl(void* v_pPar);

//	unsigned long P_Ping();
	friend unsigned long G_Ping(void* v_pPar);

private:

	int _InitCom();
	int _CloseCom();
	int _OtherHdl();

	void _GsmHardReset();
	void _GsmPowOn();
	void _GsmPowOff();

	// { 仅被状态机线程调用
	int _ComSnd(void* v_pSnd, DWORD v_dwSndLen );
	int _SetAT( DWORD v_dwWaitPerTime, BYTE v_bytTryTimes, void* v_pResult, size_t v_sizResult,
		PCHKATRES v_pChkRes, DWORD v_dwSndLen, char* v_szATCmd, ... );
	int _ReadAT( DWORD v_dwWait, void* v_pResult, size_t v_sizResult, PCHKATRES v_pChkRes );
	int _PhoneDataDeal(char* v_szAtData);
	
	int _PhoneHdl();
	int _PhoneStaHdl_Init();
	int _PhoneStaHdl_Idle( int& v_iNextSta );
	int _PhoneStaHdl_DialLsn( int& v_iNextSta );
	int _PhoneStaHdl_WaitLsnPick( int &v_iNextSta );
	int _PhoneStaHdl_Lsning(int &v_iNextSta );
	int _PhoneStaHdl_Dial( int &v_iNextSta );
	int _PhoneStaHdl_WaitPick( int &v_iNextSta );
	int _PhoneStaHdl_Hold( int &v_iNextSta );
	int _PhoneStaHdl_RcvRing( int& v_iNextSta );
	int _PhoneStaHdl_Hangup( int &v_iNextSta );
	int _PhoneStaHdl_Reset();
	int _PhoneStaHdl_DialNet( int &v_iNextSta );
	int _PhoneStaHdl_WaitDialNet( int &v_iNextSta );
	void _PhoneStaHdl_CutNet( int &v_iNextSta );

	void _PhoneJugSmsSnd();
	void _PhoneJugSmsCentSet();
	void _PhoneJugSmsPingRpt();
	void _PhoneJugAlerm();
	BOOL _PhoneJugReset();
	BOOL _PhoneJugCutNet();

	int _PhoneSingleGet( int *v_p_iSignal );

	int _PhoneAudioModeChg(int v_iChgToMode);
	int _PhoneAudioOutVolChg(int v_iChgDegree);

	int _PhoneSmsSnd( SM_PARAM& v_objSmPar );
	int _PushSndSms(char *to, byte tolen, char *msg, byte msglen, BYTE v_bytSrcType);
	bool sms_deal(SM_PARAM *psm);

	int _PhoneDialNet();
	void _PhoneCutNet();
	int _ForkPppd();
	
	int  convert6to7(char *sptr, char *dptr, int len);
	int  convert6to8(const char* strOldBuf, char* strNewBuf, const int iOldLen);
	bool check_crc1(const byte *buf, const int len);	// 查表
	byte get_crc1(const byte *buf, const int len);		// 查表
	bool check_crc2(const byte *buf, const int len);	// 直接取累加和
	byte get_crc2(const byte *buf, const int len);		// 直接取累加和
	// } 仅被状态机线程调用

	tagPhoneTask _PhoneTaskGet( DWORD v_dwToGetTaskSymb );
	void _PhoneTaskSet( const tagPhoneTask& v_objPhoneTask, DWORD v_dwToSetTaskSymb );
	void _PhoneTaskClr(DWORD v_dwToGetTaskSymb);
	void _ClearPhoneTask( DWORD v_dwToGetTaskSymb, tagPhoneTask& v_objPhoneTask );

	tagPhoneStaCommon _PhoneStaCommonGet( DWORD v_dwToClrStaSymb );
	void _PhoneStaCommonSet( const tagPhoneStaCommon& v_objPhoneStaCommon, DWORD v_dwToSetStaSymb );
	void _PhoneStaCommonClr(DWORD v_dwToClrStaSymb);
	void _ClearPhoneStaCommon(tagPhoneStaCommon& v_objPhoneStaCommon, DWORD v_dwToClrStaSymb);

	byte tel_type(bool callout, char *tel, byte len, ushort *holdtime);
	int JugTelType( char* v_szTel, unsigned char v_ucTelLen, unsigned char* v_p_cTelType );
	int JugTelType2( char* v_szTel, unsigned char v_ucTelLen, unsigned char* v_p_cTelType,
		WORD* v_p_wLmtOutPeriod, WORD* v_p_wLmtInPeriod );
	int JugTelType3(char* v_szTel, unsigned char v_ucTelLen, unsigned char* v_p_cTelType,
		WORD* v_p_wLmtOutPeriod, WORD* v_p_wLmtInPeriod);

private:

	int m_iPhonePort;	//手机模块串口设备文件句柄
	
	pthread_t m_hThdComRcv;	//串口接收线程句柄
	pthread_t m_hThdPhoneHdl;
	pthread_t m_hThdOtherHdl;
//	pthread_t m_hThdPing;

	//多线程访问互斥量
	pthread_mutex_t 	m_crisecRcv;
	pthread_mutex_t 	m_crisecPhoneTask;
	pthread_mutex_t 	m_crisecSta;
	pthread_mutex_t 	m_crisecDog;

	CInnerDataMng m_objDataMngRcv;
	CInnerDataMng m_objDataMngSmsSnd;

	tagPhoneTask m_objPhoneTask;

	// 状态机中单一状态使用的状态值
	tagPhoneStaDial m_objPhoneStaDial;
	tagPhoneStaDialLsn m_objPhoneStaDialLsn;
	tagPhoneStaHold m_objPhoneStaHold;
	tagPhoneStaLsning m_objPhoneStaLsning;
	tagPhoneStaRing m_objPhoneStaRing;

	// 多个状态及多线程要使用的状态值
	tagPhoneStaCommon m_objPhoneStaCommon;

	DWORD m_dwClrDogTm;
	volatile int m_iPhoneHdlSta;
	volatile bool m_bPhoneNetReady;
	volatile bool m_bToClosePhone;
	DWORD m_dwDialNetTm;

	bool m_bM33a;
	int m_GsmType;//gsm模块类型

};

#endif // #ifndef _PHONEGSM_H_
