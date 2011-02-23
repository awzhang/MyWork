#ifndef _PHONEEVDO_H_
#define _PHONEEVDO_H_

enum EVDO_ATANS
{
	EVDO_ATANS_OK = 1,
	EVDO_ATANS_ERROR,
	EVDO_ATANS_CMEERR,
	EVDO_ATANS_NOANS,
	EVDO_ATANS_RCVNOTALL,
	// { 仅用于呼入呼出
	EVDO_ATANS_NOANSWER,
	EVDO_ATANS_NOCARRIER,
	EVDO_ATANS_NODIALTONE,
	EVDO_ATANS_BUSY,
	EVDO_ATANS_NOCALL, // 不在通话中
	// } 仅用于呼入呼出
	EVDO_ATANS_OTHER
};

enum EVDO_PHONESTA
{
	EVDO_PST_INIT = 1,
	EVDO_PST_IDLE,
	EVDO_PST_DIALNET,
	EVDO_PST_WAITDIALNET,
	EVDO_PST_CUTNET,
	EVDO_PST_RESET,
	EVDO_PST_DIALLSN,
	EVDO_PST_WAITLSNPICK,
	EVDO_PST_LSNING,
	EVDO_PST_DIAL,
	EVDO_PST_WAITPICK,
	EVDO_PST_HOLD,
	EVDO_PST_HANGUP,
	EVDO_PST_RCVRING
};

// enum EVDO_PINGSTA
// {
// 	EVDO_PINGST_IDLE = 666,
// 	EVDO_PINGST_DOING,
// 	EVDO_PINGST_SUCC,
// 	EVDO_PINGST_FAIL,
// 	EVDO_PINGST_CANCEL
// };

enum EVDO_AUDIOMODE
{
	EVDO_AUDIOMODE_HANDFREE = 1, //免提// 不要太大,有的结构体里只用1个字节保存
	EVDO_AUDIOMODE_EARPHONE,		//耳机
	EVDO_AUDIOMODE_LSN,			//监听
	EVDO_AUDIOMODE_DIAL,			//拨号模式(非监听拨号)
	EVDO_AUDIOMODE_CLOSE			//通话关闭模式
};

// 任务指令定义（可或起来）
const DWORD EVDO_PHTASK_DIALSN		= 0X00000001;	//
const DWORD EVDO_PHTASK_DIAL			= 0X00000002;
const DWORD EVDO_PHTASK_RING			= 0X00000004;
const DWORD EVDO_PHTASK_SETSMSCENT	= 0X00000008;
const DWORD EVDO_PHTASK_SNDDTMF		= 0X00000010;
const DWORD EVDO_PHTASK_PICKUP		= 0X00000020;
const DWORD EVDO_PHTASK_HANGUP		= 0X00000040;
const DWORD EVDO_PHTASK_CHGAUDIO		= 0X00000080;	//免提耳机切换
const DWORD EVDO_PHTASK_CHGVOL		= 0X00000100;	//调节音量
const DWORD EVDO_PHTASK_PING			= 0x00000200;
const DWORD EVDO_PHTASK_PINGRPT		= 0x00000400;
const DWORD EVDO_PHTASK_CUTNET		= 0x00000800;
const DWORD EVDO_PHTASK_ALERM		= 0x00001000;	//报警
const DWORD EVDO_PHTASK_RESET		= 0x00002000;

// 任务数据定义 (多线程访问,需要互斥)
struct tagEvdoPhoneTask
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
	BYTE	m_bytChgAudio; // EVDO_AUDIOMODE_HANDFREE,切换到免提; EVDO_AUDIOMODE_EARPHONE,切换到耳机
	int		m_iChgVol; // 1,增大; -1,减小

//	bool	m_bPing;
	BYTE	m_bytPingSrc; // 1,QianExe; 2,StreamMedia (或的关系 )

	BYTE	m_bytPingRptSrc;
	BYTE	m_bytPingResult; // 0,成功; 1,失败; 2,异常; 3,被取消

//	bool	m_bCutNet;

//	DWORD	m_dwAlertBegin;
	DWORD	m_dwAlertPrid;

	tagEvdoPhoneTask() { memset(this, 0, sizeof(*this)); }
};


/// { 状态机中每一个状态对应的状态值 (仅在状态机线程中使用,无需互斥访问)
struct tagEvdoPhoneStaDial
{
	DWORD	m_dwDialBeginTm;
	DWORD	m_dwHoldLmtPrid;
	char	m_szDialTel[20];

	void Init() { memset(this, 0, sizeof(*this)); }
	tagEvdoPhoneStaDial() { Init(); }
};

struct tagEvdoPhoneStaHold
{
	DWORD	m_dwHoldBeginTm;
	DWORD	m_dwHoldLmtPrid;
	DWORD	m_dwCallChkTm;

	void Init() { memset(this, 0, sizeof(*this)); }
	tagEvdoPhoneStaHold() { Init(); }
};

struct tagEvdoPhoneStaDialLsn
{
	DWORD	m_dwDialBeginTm;
	char	m_szDialLsnTel[20];

	void Init() { memset(this, 0, sizeof(*this)); }
	tagEvdoPhoneStaDialLsn() { Init(); }
};

struct tagEvdoPhoneStaLsning
{
	DWORD	m_dwCallChkTm;

	void Init() { memset(this, 0, sizeof(*this)); }
	tagEvdoPhoneStaLsning() { Init(); }
};

struct tagEvdoPhoneStaRing
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
	tagEvdoPhoneStaRing() { Init(); }
};
/// } 状态机中每一个状态对应的状态值


// 通用状态处理标记 (按位定义)
const DWORD EVDO_PHSTVAR_IMEI		= 0X00000001;
const DWORD EVDO_PHSTVAR_SING		= 0X00000002;
const DWORD EVDO_PHSTVAR_ALERT		= 0X00000004;
const DWORD EVDO_PHSTVAR_NET			= 0X00000008;
const DWORD EVDO_PHSTVAR_PING		= 0X00000010;
const DWORD EVDO_PHSTVAR_AUDIOMODE	= 0X00000020;
const DWORD EVDO_PHSTVAR_AUDIOOUTLVL = 0x00000040;

/// { 状态机中多个状态下需要使用的状态值 (可能多线程访问,需要互斥)
struct tagEvdoPhoneStaCommon
{
	char	m_szGsmIMEI[128];
	int		m_iSignal;

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
	}

	tagEvdoPhoneStaCommon()
	{
		Init();
	}
};
/// } 状态机中多个状态下需要使用的状态值

typedef int ( *PCHKATRES) (char* v_szAtRes, void* v_pResult, size_t v_sizResult );

class CPhoneEvdo  
{
public:
	CPhoneEvdo();
	virtual ~CPhoneEvdo();
	void ResetPhone();
	void ReDialNet();
	BYTE GetPhoneState();
	int GetPhoneHdlSta();
	int GetPhoneSignal();
	void GetPhoneIMEI( char* v_szImei, size_t v_sizImei );
	int GetPhoneVolume();
	int InitPhone();
	void ReleasePhone();
	
	int _PppSigHandler();

	bool IsPhoneValid();

protected:
	unsigned long P_PhoneHdl();
	friend unsigned long G_EVDO_PhoneHdl(void* v_pPar);

	unsigned long P_ComRcv();
	friend unsigned long G_EVDO_ComRcv(void* v_pPar);

	unsigned long P_OtherHdl();
	friend unsigned long G_EVDO_OtherHdl(void* v_pPar);

//	unsigned long P_Ping();
	friend unsigned long G_EVDO_Ping(void* v_pPar);

private:

	void _GsmHardReset();
	void _GsmPowOn();
	void _GsmPowOff();
	int _InitCom();
	int _CloseCom();
	int _OtherHdl();

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
	int _PhoneJugSmsRcv();
	void _PhoneJugSmsCentSet();
	void _PhoneJugSmsPingRpt();
	void _PhoneJugAlerm();
	BOOL _PhoneJugReset();
	BOOL _PhoneJugCutNet();

	int _PhoneAudioModeSet(int v_iChgToMode);
	int _PhoneAudioOutVolSet(int v_iChgDegree);

	int _PhoneSmsSnd( SM_PARAM& v_objSmPar );
	int _PushSndSms(char *to, byte tolen, char *msg, byte msglen, BYTE v_bytSrcType);
	bool sms_deal(SM_PARAM *psm);
	int _QueryRcvSMS();

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

	tagEvdoPhoneTask _PhoneTaskGet( DWORD v_dwToGetTaskSymb );
	void _PhoneTaskSet( const tagEvdoPhoneTask& v_objPhoneTask, DWORD v_dwToSetTaskSymb );
	void _PhoneTaskClr(DWORD v_dwToGetTaskSymb);
	void _ClearPhoneTask( DWORD v_dwToGetTaskSymb, tagEvdoPhoneTask& v_objPhoneTask );

	tagEvdoPhoneStaCommon _PhoneStaCommonGet( DWORD v_dwToClrStaSymb );
	void _PhoneStaCommonSet( const tagEvdoPhoneStaCommon& v_objPhoneStaCommon, DWORD v_dwToSetStaSymb );
	void _PhoneStaCommonClr(DWORD v_dwToClrStaSymb);
	void _ClearPhoneStaCommon(tagEvdoPhoneStaCommon& v_objPhoneStaCommon, DWORD v_dwToClrStaSymb);

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
	CInnerDataMng m_objDataMngSmsRcv;

	tagEvdoPhoneTask m_objPhoneTask;

	// 状态机中单一状态使用的状态值
	tagEvdoPhoneStaDial m_objPhoneStaDial;
	tagEvdoPhoneStaDialLsn m_objPhoneStaDialLsn;
	tagEvdoPhoneStaHold m_objPhoneStaHold;
	tagEvdoPhoneStaLsning m_objPhoneStaLsning;
	tagEvdoPhoneStaRing m_objPhoneStaRing;

	// 多个状态及多线程要使用的状态值
	tagEvdoPhoneStaCommon m_objPhoneStaCommon;

	DWORD m_dwClrDogTm;
	volatile int m_iPhoneHdlSta;
	volatile bool m_bPhoneNetReady;
	volatile bool m_bToClosePhone;
	DWORD m_dwDialNetTm;
};

#endif	//#ifndef _PHONEEVDO_H_
