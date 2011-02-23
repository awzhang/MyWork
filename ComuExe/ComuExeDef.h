#ifndef _COMUEXEDEF_H_
#define _COMUEXEDEF_H_

#define IOSTA_ACC_ON				0x00000001
#define IOSTA_ALARM_ON				0x00000002
#define IOSTA_JIJIA_HEAVY			0x00000004
#define IOSTA_FDOOR_OPEN			0x00000008
#define IOSTA_BDOOR_OPEN			0x00000010
#define IOSTA_JIAOSHA_VALID			0x00000020
#define IOSTA_SHOUSHA_VALID			0x00000040
#define IOSTA_PASSENGER1_VALID		0x00000080
#define IOSTA_PASSENGER2_VALID		0x00000100
#define IOSTA_PASSENGER3_VALID		0x00000200

#if USE_LANGUAGE == 0
#define LANG_ACC_NEEDVALID ("ACC无效!请将ACC先置为有效!")
#define LANG_IO_CHKMODE_OPEN ("传感器实时检测已打开! 5分钟后关闭!")
#define LANG_IO_CHKMODE_CLOSE ("传感器实时检测模式已关闭!")
#define LANG_GPS_NOSIGNAL ("暂时获取不到GPS信号!请稍侯...")
#define LANG_GPS_CHKMODE_OPEN ("GPS实时检测已打开! 30秒后关闭!")
#define LANG_GPS_CHKMODE_CLOSE ("GPS实时检测模式已关闭!")
#define LANG_CHKING_U ("正在检测U盘...")
#define LANG_CHKU_SUCC ("U盘检测成功!")
#define LANG_CHKU_FAIL ("U盘检测失败,换U盘或重启车台!")
#define LANG_NK_UPGRADING ("内核升级中,请保持供电正常并耐心等候…")
#define LANG_NK_UPGRAD_SUCC ("内核升级成功!系统即将重启...")
#define LANG_NK_UPGRAD_FILE_LENERR ("内核升级,文件长度错误!")
#define LANG_NK_UPGRAD_FILE_NOFIND ("内核升级,文件没有找到!")
#define LANG_NK_UPGRAD_FILE_OPENFAIL ("内核升级,文件打开错误!")
#define LANG_NK_UPGRAD_FILE_READFAIL ("内核升级,文件读取错误!")
#define LANG_NK_UPGRAD_TOCREADFAIL ("内核升级,TOC读取失败!")
#define LANG_NK_UPGRAD_TOCWRITEFAIL ("内核升级,TOC写入失败!")
#define LANG_SFT_UPGRAD ("应用软件升级")
#define LANG_SFT_UPGRADING ("应用软件升级中...")
#define LANG_DATAOUTPUT ("正在将数据导入U盘...")
#define LANG_SYS_TORESET ("系统即将重启...")
#define LANG_SUCC ("成功")
#define LANG_FAIL ("失败")
#define LANG_ALA1_TEST ("计价串口测试")
#define LANG_COM6_TEST ("备用串口测试")
#define LANG_TESTOILCUT_COMPLETE ("断油断电 测试完毕!")
#define LANG_LOGGED ("已登陆")
#define LANG_NOLOG ("未登陆")
#define LANG_SIGNAL ("信号")
#define LANG_ONLINE ("在线")
#define LANG_OFFLINE ("掉线")
#define LANG_VALID ("有效")
#define LANG_INVALID ("无效")
#define LANG_CONNECT ("连接")
#define LANG_DISCONNECT ("断开")
#define LANG_NORMAL ("正常")
#define LANG_ALERM ("报警")
#define LANG_HEAVY ("重车")
#define LANG_EMPTY ("空车")
#define LANG_RESUMECFG ("恢复出厂设置")
#define LANG_SMSCENT ("短信中心号")
#define LANG_SPECCODE ("特服号")
#define LANG_IO_METER ("计价器信号")
#define LANG_METER_CONNECTION ("计价器连接")
#define LANG_IO_POWDET ("电源")
#define LANG_IO_FOOTALERM ("劫警")
#define LANG_IO_BROWNOUT ("电压")
#define LANG_IO_OIL ("油耗")
#define LANG_IO_OILPOW ("电压")
#define LANG_IO_PULSE ("里程脉冲")
#define LANG_IO_SPEED ("速度")

#define LANG_IO_FDOOR ("前车门")
#define LANG_IO_BDOOR ("后车门")
#define LANG_IO_LOAD1 ("载客位置1")
#define LANG_IO_LOAD2 ("载客位置2")
#define LANG_IO_LOAD3 ("载客位置3")

#define LANG_IO_QIANYA ("欠压信号")
#define LANG_IO_ELECSPEAKER ("电子喇叭信号")
#define LANG_IO_FDOORALERT ("前门报警信号")
#define LANG_IO_BDOORALERT ("后门报警信号")
#define LANG_IO_JIAOSHA ("脚刹信号")
#define LANG_IO_POWDETM ("电源检测信号")
#define LANG_IO_CHEDENG ("车灯信号")
#define LANG_IO_ZHENLING ("振铃信号")

#endif

#if USE_LANGUAGE == 11
#define LANG_ACC_NEEDVALID ("ACC Invalid! Make ACC Valid First")
#define LANG_IO_CHKMODE_OPEN ("IO Check Mode Opened! 5 Minutes Later Closes")
#define LANG_IO_CHKMODE_CLOSE ("IO Check Mode Closed!")
#define LANG_GPS_NOSIGNAL ("No GPS Signal, Please Waiting...")
#define LANG_GPS_CHKMODE_OPEN ("GPS Check Mode Opened! 30 Secs Later Closes!")
#define LANG_GPS_CHKMODE_CLOSE ("GPS Check Mode Closed!")
#define LANG_CHKING_U ("U Disk Searching...")
#define LANG_CHKU_SUCC ("U Disk Search Success!")
#define LANG_CHKU_FAIL ("U Disk Search Failure!")
#define LANG_NK_UPGRADING ("Maintain the power supply, Sys Upgrading...")
#define LANG_NK_UPGRAD_SUCC ("Sys Upgrade Succ, Resetting...")
#define LANG_NK_UPGRAD_FILE_LENERR ("Sys Upgrade, File Len Err!")
#define LANG_NK_UPGRAD_FILE_NOFIND ("Sys Upgrade, File Find Failure!")
#define LANG_NK_UPGRAD_FILE_OPENFAIL ("Sys Upgrade, File Open Failure!")
#define LANG_NK_UPGRAD_FILE_READFAIL ("Sys Upgrade, File Read Failure!")
#define LANG_NK_UPGRAD_TOCREADFAIL ("Sys Upgrade,Toc Read Fail!")
#define LANG_NK_UPGRAD_TOCWRITEFAIL ("Sys Upgrade,Toc Write Fail!")
#define LANG_SFT_UPGRAD ("App Soft Upgrade")
#define LANG_SFT_UPGRADING ("App Upgrading...")
#define LANG_SYS_TORESET ("Sys Resetting...")
#define LANG_DATAOUTPUT ("Data Output...")
#define LANG_SUCC ("Succ")
#define LANG_FAIL ("Fail")
#define LANG_ALA1_TEST ("ALA1 Port Test")
#define LANG_COM6_TEST ("COM6 Port Test")
#define LANG_TESTOILCUT_COMPLETE ("Oil Cut Test Complete!")
#define LANG_LOGGED ("Logged in")
#define LANG_NOLOG ("Logged out")
#define LANG_SIGNAL ("Signal")
#define LANG_ONLINE ("Online")
#define LANG_OFFLINE ("Offline")
#define LANG_VALID ("Valid")
#define LANG_INVALID ("Invalid")
#define LANG_CONNECT ("Connected")
#define LANG_DISCONNECT ("Disconnected")
#define LANG_NORMAL ("Normal")
#define LANG_ALERM ("Alerm")
#define LANG_RESUMECFG ("Resume Initial Config")
#define LANG_SMSCENT ("SmsCent")
#define LANG_SPECCODE ("SpecCode")
#define LANG_IO_METER ("Meter")
#define LANG_METER_CONNECTION ("Meter Connect")
#define LANG_IO_POWDET ("Pow Connect")
#define LANG_IO_FOOTALERM ("Rob Alerm")
#define LANG_IO_BROWNOUT ("Pow Voltage")
#define LANG_HEAVY ("Heavy")
#define LANG_EMPTY ("Empty")
#define LANG_IO_OIL ("Oil")
#define LANG_IO_PULSE ("Meter Pulse")
#define LANG_IO_SPEED ("Speed")

#define LANG_IO_FDOOR ("Front Door")
#define LANG_IO_BDOOR ("Back Door")
#define LANG_IO_LOAD1 ("Load Passenger Seat 1")
#define LANG_IO_LOAD2 ("Load Passenger Seat 2")
#define LANG_IO_LOAD3 ("Load Passenger Seat 3")
#endif



#if USE_LANGUAGE == 0
#define LANG_IOCHK_LIGHT ("检测到 车灯 信号变化")
#define LANG_IOCHK_SPEAKER ("检测到 电子喇叭 信号变化")
#define LANG_IOCHK_CENTLOCK_OFF ("检测到 中控锁关 信号变化")
#define LANG_IOCHK_CENTLOCK_ON ("检测到 中控锁开 信号变化")
#define LANG_IOCHK_ALERM_LED ("检测到 报警指示灯 信号变化")
#define LANG_IOCHK_ALERMFOOT ("检测到 抢劫报警 信号变化")
#define LANG_IOCHK_ACCON ("检测到 ACC有效 信号变化")
#define LANG_IOCHK_ACCOFF ("检测到 ACC无效 信号变化")
#define LANG_IOCHK_METERON ("检测到 计程有效 信号变化")
#define LANG_IOCHK_METEROFF ("检测到 计程无效 信号变化")
#define LANG_IOCHK_ALERM_POWDETON ("检测到 断电报警 信号变化")
#define LANG_IOCHK_ALERM_POWDETOFF ("检测到 电平正常 信号变化")
#define LANG_IOCHK_DOOROPEN ("检测到 前车门打开 信号变化")
#define LANG_IOCHK_DOORCLOSE ("检测到 前车门关闭 信号变化")
#define LANG_IOCHK_DCDHIGH ("检测到 DCD高 信号变化")
#define LANG_IOCHK_DCDLOW ("检测到 DCD低 信号变化")
#define LANG_IOCHK_RINGHIGH ("检测到 RING高 信号变化")
#define LANG_IOCHK_RINGLOW ("检测到 RING低 信号变化")
#define LANG_IOCHK_ALERMFOOT_CANCEL ("检测到 抢劫报警取消 信号变化")
#define LANG_IOCHK_BROWNOUT ("检测到 欠压 信号变化")
#define LANG_IOCHK_OVERTURN ("检测到 侧翻 信号变化")
#define LANG_IOCHK_OVERTURN_CANCEL ("检测到 侧翻取消 信号变化")
#define LANG_IOCHK_HANDBRAKE ("检测到 手刹 信号变化")
#define LANG_IOCHK_HANDBRAKE_CANCEL ("检测到 手刹取消 信号变化")
#define LANG_IOCHK_COLLISIONG ("检测到 碰撞 信号变化")
#define LANG_IOCHK_COLLISIONG_CANCEL ("检测到 碰撞取消 信号变化")
#define LANG_IOCHK_FOOTBRAKE ("检测到 脚刹 信号变化")
#define LANG_IOCHK_FOOTBRAKE_CANCEL ("检测到 脚刹取消 信号变化")
#define LANG_IOCHK_BACKDOOROPEN ("检测到 后车门打开 信号变化")
#define LANG_IOCHK_BACKDOORCLOSE ("检测到 后车门关闭 信号变化")
#define LANG_IOCHK_LEFTLIGHT_ON ("检测到 左转向灯亮 信号变化")
#define LANG_IOCHK_LEFTLIGHT_OFF ("检测到 左转向灯灭 信号变化")
#define LANG_IOCHK_RIGHTLIGHT_ON ("检测到 右转向灯亮 信号变化")
#define LANG_IOCHK_RIGHTLIGHT_OFF ("检测到 右转向灯灭 信号变化")
#define LANG_IOCHK_FRONTLIGHT_ON ("检测到 前大灯亮 信号变化")
#define LANG_IOCHK_FRONTLIGHT_OFF ("检测到 前大灯灭 信号变化")
#define LANG_IOCHK_JIJIA_VALID ("检测到 计价器有效 信号有效")
#define LANG_IOCHK_JIJIA_INVALID ("检测到 计价器无效 信号有效")
#define LANG_IOCHK_PASSENGER1_VALID ("检测到 载客1 有效 信号变化")
#define LANG_IOCHK_PASSENGER1_INVALID ("检测到 载客1 无效 信号变化")
#define LANG_IOCHK_PASSENGER2_VALID ("检测到 载客2 有效 信号变化")
#define LANG_IOCHK_PASSENGER2_INVALID ("检测到 载客2 无效 信号变化")
#define LANG_IOCHK_PASSENGER3_VALID ("检测到 载客3 有效 信号有效")
#define LANG_IOCHK_PASSENGER3_INVALID ("检测到 载客3 无效 信号有效")
#define LANG_IOCHK_QIANYA_VALID ("检测到 欠压 有效 信号变化")
#define LANG_IOCHK_QIANYA_INVALID ("检测到 欠压 无效 信号变化")
#define LANG_IOCHK_ELECSPEAKER_VALID ("检测到 电子喇叭 有效 信号变化")
#define LANG_IOCHK_ELECSPEAKER_INVALID ("检测到 电子喇叭 无效 信号变化")
#define LANG_IOCHK_POWDETM_VALID ("检测到 电源检测 有效 信号变化")
#define LANG_IOCHK_POWDETM_INVALID ("检测到 电源检测 无效 信号变化")
#define LANG_IOCHK_CHEDENG_VALID ("检测到 车灯 有效 信号变化")
#define LANG_IOCHK_CHEDENG_INVALID ("检测到 车灯 无效 信号变化")
#define LANG_IOCHK_ZHENLING_VALID ("检测到 振铃 有效 信号变化")
#define LANG_IOCHK_ZHENLING_INVALID ("检测到 振铃 无效 信号变化")
#define LANG_IOCHK_FDOORALERT_VALID ("检测到 前门报警 有效 信号变化")
#define LANG_IOCHK_FDOORALERT_INVALID ("检测到 前门报警 无效 信号变化")
#define LANG_IOCHK_BDOORALERT_VALID ("检测到 后门报警 有效 信号变化")
#define LANG_IOCHK_BDOORALERT_INVALID ("检测到 后门报警 无效 信号变化")
#endif	//USE_LANGUAGE == 0

#if USE_LANGUAGE == 11
#define LANG_IOCHK_LIGHT ("IO: Light On")
#define LANG_IOCHK_SPEAKER ("IO: Electronic Speaker")
#define LANG_IOCHK_CENTLOCK_OFF ("IO: Central Locks Off")
#define LANG_IOCHK_CENTLOCK_ON ("IO: Central Locks On")
#define LANG_IOCHK_ALERM_LED ("IO: Alerm Led")
#define LANG_IOCHK_ALERMFOOT ("IO: Foot Alerm")
#define LANG_IOCHK_ACCON ("IO: ACC On")
#define LANG_IOCHK_ACCOFF ("IO: ACC Off")
#define LANG_IOCHK_METERON ("IO: Meter On")
#define LANG_IOCHK_METEROFF ("IO: Meter Off")
#define LANG_IOCHK_ALERM_POWDETON ("IO: Power Failure")
#define LANG_IOCHK_ALERM_POWDETOFF ("IO: Power Resume")
#define LANG_IOCHK_DOOROPEN ("IO: Front Door Open")
#define LANG_IOCHK_DOORCLOSE ("IO: Front Door Close")
#define LANG_IOCHK_DCDHIGH ("IO: DCD High")
#define LANG_IOCHK_DCDLOW ("IO: DCD Low")
#define LANG_IOCHK_RINGHIGH ("IO: RING High")
#define LANG_IOCHK_RINGLOW ("IO: RING Low")
#define LANG_IOCHK_ALERMFOOT_CANCEL ("IO: Foot Alerm Cancel")
#define LANG_IOCHK_BROWNOUT ("IO: Power Brownout")
#define LANG_IOCHK_OVERTURN ("IO: Overturn")
#define LANG_IOCHK_OVERTURN_CANCEL ("IO: Overturn Cancel")
#define LANG_IOCHK_HANDBRAKE ("IO: Hand Brake")
#define LANG_IOCHK_HANDBRAKE_CANCEL ("IO: Hand Brake Cancel")
#define LANG_IOCHK_COLLISIONG ("IO: Collision")
#define LANG_IOCHK_COLLISIONG_CANCEL ("IO: Collision Cancel")
#define LANG_IOCHK_FOOTBRAKE ("IO: Foot Brake")
#define LANG_IOCHK_FOOTBRAKE_CANCEL ("IO: Foot Brake Cancel")
#define LANG_IOCHK_BACKDOOROPEN ("IO: Back Door Open")
#define LANG_IOCHK_BACKDOORCLOSE ("IO: Back Door Close")
#define LANG_IOCHK_LEFTLIGHT_ON ("IO: Left Light On")
#define LANG_IOCHK_LEFTLIGHT_OFF ("IO: Left Light Off")
#define LANG_IOCHK_RIGHTLIGHT_ON ("IO: Right Light On")
#define LANG_IOCHK_RIGHTLIGHT_OFF ("IO: Right Light Off")
#define LANG_IOCHK_FRONTLIGHT_ON ("IO: Front Light On")
#define LANG_IOCHK_FRONTLIGHT_OFF ("IO: Front Light Off")
#define LANG_IOCHK_JIJIA_VALID ("IO: jijia valid")
#define LANG_IOCHK_JIJIA_INVALID ("IO: jijia invalid")
#define LANG_IOCHK_PASSENGER1_VALID ("IO: Passager load 1 valid")
#define LANG_IOCHK_PASSENGER1_INVALID ("IO: Passager load 1 invalid")
#define LANG_IOCHK_PASSENGER2_VALID ("IO: Passager load 2 valid")
#define LANG_IOCHK_PASSENGER2_INVALID ("IO: Passager load 2 invalid")
#define LANG_IOCHK_PASSENGER3_VALID ("IO: Passager load 3 valid")
#define LANG_IOCHK_PASSENGER3_INVALID ("IO: Passager load 3 invalid")
#endif	//USE_LANGUAGE == 11

#define BENQ		0
#define SIMCOM		1
#define DTTD		2
#define EVDO		3

#define DATA_UNIT_BUFF		1			//GPS数据
#define GPS_BUFSIZE			1024
#define MIN_GPRS_SIGNAL		5			// 能拨GPRS的最小信号值

#define MAX_DEAL_BUF_SIZE	10240

#define KEYPATH				("/mnt/Flash/part3/for_key")  //一个路径名
#define SHM_SIZE			4096
#define GPS_PATH			("/SiRFNavIIIdemo")

#define SPEED_STOP		0.5 // 低于该速度认为静止 (米/秒)
#define SPEED_MOVE		1.5 // 高于该速度认为是移动 (米/秒) (SPEED_MOVE 应大于 SPEED_STOP)
#define SPEED_COUNT		10 // 确认静止/移动,需要连续多少次判断

enum ENUGPSSTA
{
	stInit = 1,
	stReset,
	stGetData,
	stWaitKill,
	stSleep
};

enum TIMERS
{
	// CHandset
	HST_CONVERT_TIMER = 1,
	
	// CHandApp
	DRV_LOG_TIMER
};


const DWORD TIMEBELIV_GPSAPERIOD = 15 * 60000; // 单位:毫秒, GPS有效连续多久后,时间可信
const DWORD TIMEUNBLV_GPSVMINPERIOD = 30000; // 单位:毫秒, GPS无效连续多久后,时间不可信

//comu进程内部数据队列，phone-diaodu
const BYTE INNERSYMB_PHONETODIAO = 0x01;
const BYTE INNERSYMB_PHONETOQIAN = 0x02;
const BYTE INNERSYMB_PHONETOLIU = 0x03;

#pragma pack(4)

typedef struct tagComPar
{
	UINT	uiPort;			//串口号
	DWORD	dwBaudrate;		//波特率
	DWORD	dwByteSize;	//字节位数
	DWORD	dwParity;		//奇偶标志
	DWORD	dwStopBits;	//停止位长标志
	DWORD	dwReadIntervalTimeout;
	DWORD	dwReadTotalTimeoutMultiplier;
	DWORD	dwReadTotalTimeoutConstant;
}COMPAR;

typedef struct tagSatelliteInfo
{
	BYTE	m_bytSatelliteCount;
	BYTE	m_arySatelliteInfo[32][2]; // 卫星ID(1~32) + 信噪比(0~99)
	tagSatelliteInfo() { memset(this, 0, sizeof(*this)); }
}SATELLITEINFO;


// 卫星信息
// typedef struct tagSatelliteInfo
// {
// 	BYTE	m_bytSatelliteCount;
// 	BYTE	m_arySatelliteInfo[32][2]; // 卫星ID(1~32) + 信噪比(0~99)
// 	tagSatelliteInfo() { memset(this, 0, sizeof(*this)); }
// }SATELLITEINFO;

typedef struct itemmenu{
	INT8U       Attrib;					//cyh add: 菜单的属性
	char        ContentStr[20];
	void        (*EntryProc)(void);		//cyh add：进入菜单前，首先执行的函数。 
	INT8U       NumChild;				//cyh add 有几个菜单项
	itemmenu	*ChildItem;				//cyh 指向各个菜单项的数组的指针
} ITEMMENU_STRUCT;						//cyh add:保存菜单的相关数据

typedef struct menuwin_st{
	INT8U   WinAttrib;
	void    (*Handler_KEY_ACK)(void);
	void    (*Handler_KEY_NAK)(void);
	void    (*Handler_KEY_FCN)(void);
	void    (*Handler_KEY_SEL)(void);
	INT16U  (*StoreProc)(INT8U *);				//cyh add:将当前的菜单结构体数据ItemMenu保存到所指地址，这样当返回到上层菜单时，原来的数据才不会丢失
	INT16U  (*MaxRecordProc)(void);				//cyh add: 获取当前的最大子菜单数
	INT16U  (*InitRecordProc)(void);			//cyh add: 加载当前菜单的输入模式????还是初始化首先在界面上显示的菜单????
	INT16U  (*ReadRecordProc)(INT16S, INT8U *);	//cyh add:读取当前菜单下的内容(标题?)，并返回内容所占的字节数. 第一个参数表示菜单的索引号，第二个参数表示保存内容的指针，返回 读取到的记录 所占的字节数
} MENUWIN_STRUCT;								//cyh add: 保存菜单相关数据的功能函数

typedef struct prompt_st{
	INT8U   Mode;
	int   LiveTime;					/* UNIT: SECOND */
	INT8U  *TextPtr;
	INT16U  TextLen;
	void (*ExitProc)(void);
	void (*ExitKeyProc)(void);		//cyh add定时时间到或者按下OK键时 的处理函数
} PROMPT_STRUCT;

typedef struct editor_st
{
	INT8U   Mode;
	INT16U  MinLen;
	INT16U  MaxLen;
	INT16U  LabelLen;
	char   *LabelPtr;
	INT16U  InitLen;
	INT8U  *InitPtr;
	INT8U  *StorePtr;
	void    (*ExitProc)(void);
	void    (*AckProc)(void);
	INT16U  (*CurLenProc)(INT16U len, INT8U *ptr);
	INT16U  (*MinLenProc)(INT16U len, INT8U *ptr);
	INT16U  (*MaxLenProc)(INT16U len, INT8U *ptr);
	bool	(*FilterProc)(INT8U  *ch);
} EDITOR_STRUCT;

typedef struct winstackentry_st{
	void    (*InitProc)(void);
	void    (*DestroyProc)(void);
	void    (*SuspendProc)(void);
	void    (*ResumeProc)(void);
	void    (*HdlKeyProc)(void);
} WINSTACKENTRY_STRUCT;

typedef struct
{
	char        pinyin[7];
	const char *PY_mb;
}PY_INDEX;


typedef struct key_st{
	INT8U   attrib;
	INT8U   key;
} KEY_STRUCT;

//GPS协议数据
#define DATA_LENGTH 150
#define GSV_DATA_LEN 512
struct data_unit
{
	char gga_data[ DATA_LENGTH];//for gga format.
	char rmc_data[ DATA_LENGTH];//for rmc format.
	char gsa_data[ DATA_LENGTH];//for gsa format.
	char vtg_data[ DATA_LENGTH];//for vtg format.
	
	char gsv_data[GSV_DATA_LEN];//for gsv format.
};//the  basic data uint.

//GPS协议数据
struct gps_data_share
{
	int flag;//this flag is used to show that whether the share_data is initialized or not.
	data_unit combine_data[DATA_UNIT_BUFF];//data_unit buffer space used to store data of different format. 
	struct timeval time;
#if VEHICLE_TYPE == VEHICLE_M
	unsigned n1_time;//GetTickTime时间戳
#endif
	struct timezone time_zone;
	
	gps_data_share() 
	{ 
		memset(this, 0, sizeof(*this));
	}
};//the gps data controller


#pragma pack()

#pragma pack(1)

STRUCT_1 Frm10 {
	byte type;
	char dtmf;
	Frm10() { type = 0x10; }
};

STRUCT_1 Frm11 {
	byte type;
	char reply;
	Frm11() { type = 0x11; }
};

STRUCT_1 Frm12 {
	byte type;
	byte len;
	char num[256];
	Frm12() { memset(this, 0, sizeof(*this));  type = 0x12; }
};

STRUCT_1 Frm13 {
	byte type;
	char reply;
	Frm13() { type = 0x13; }
};

STRUCT_1 Frm14 {
	byte type;
	Frm14() { type = 0x14; }
};

STRUCT_1 Frm15 {
	byte type;
	char reply;
	Frm15() { type = 0x15; }
};

STRUCT_1 Frm16 {
	byte type;
	Frm16() { type = 0x16; }
};

STRUCT_1 Frm17 {
	byte type;
	char reply;
	Frm17() { type = 0x17; }
};

STRUCT_1 Frm18 {
	byte type;
	Frm18() { type = 0x18; }
};

STRUCT_1 Frm19 {
	byte type;
	byte len;
	char num[256];
	Frm19() { memset(this, 0, sizeof(*this));  type = 0x19; }
};

STRUCT_1 Frm20 {
	byte type;
	char dtmf;
	Frm20() { type = 0x20; }
};

STRUCT_1 Frm21 {
	byte type;
	Frm21() { type = 0x21; }
};

STRUCT_1 Frm22 {
	byte type;
	Frm22() { type = 0x22; }
};

STRUCT_1 Frm23 {
	byte type;
	Frm23() { type = 0x23; }
};

STRUCT_1 Frm24 {
	byte type;
	Frm24() { type = 0x24; }
};

STRUCT_1 Frm25 {
	byte type;
	byte level;
	Frm25() { type = 0x25; }
};

STRUCT_1 Frm26 {
	byte type;
	Frm26() { type = 0x26; }
};

STRUCT_1 Frm27 {
	byte type;
	byte reply;
	Frm27() { type = 0x27; }
};

STRUCT_1 Frm28 {
	byte type;
	byte dir;
	Frm28() { type = 0x28; }
};

STRUCT_1 Frm29 {
	byte type;
	byte reply;
	Frm29() { type = 0x29; }
};

STRUCT_1 Frm31 {
	byte type;
	byte reply;
	Frm31() { type = 0x31; }
};

STRUCT_1 Frm32 {
	byte type;
	byte reply;
	Frm32() { type = 0x32; }
};

// 网络状态通知 
STRUCT_1 Frm35 {
	byte type;
	byte reply;		//1:网络恢复(拨号成功)  2:网络恢复(Ping成功)   3:网络断开了	   4:通知sock检查网络
	Frm35() { type = 0x35; }
};

STRUCT_1 Frm38 {
	byte type;
	byte source;
	char tel[16];
	char msg[160];
	Frm38() { memset(this, 0, sizeof(*this));  type = 0x38; }
};

STRUCT_1 Frm39 {
	byte type;
	byte reply;
	Frm39() { type = 0x39; }
};

STRUCT_1 FrmF1 {
	byte type;
	byte len;
	char num[256];
	FrmF1() { memset(this, 0, sizeof(*this));  type = 0xF1; }
};

STRUCT_1 FrmF2 {
	byte type;
	byte reply;
	FrmF2() { type = 0xF2; reply = 0x00; }
};

STRUCT_1 FrmF3 {
	byte type;
	byte reply;
	FrmF3() { type = 0xF3; reply = 0; }
};

STRUCT_1 FrmFF {	
	byte type;
	byte iSignal;
	FrmFF() { type = 0xff; iSignal = 0;}
};

STRUCT_1 DataFrame {
		int data_len;
		byte data[1024];
	};

STRUCT_1 GpsInfo {
	byte valid;		//字节定义为：01A00000   A：GPS数据有效位，0-有效；1-无效
	byte lat[4];		//纬度
	byte lon[4];		//经度
	byte speed;		//速度
	byte dir;		//方向
};

STRUCT_1 Frm04 {
	byte type;
	GpsInfo gps;
	Frm04() { type = 0x04; }
};


STRUCT_1 Frm08 {
	byte type;
	
	byte hour;
	byte minute;
	byte second;
	
	byte year;
	byte month;
	byte day;
	Frm08() { type = 0x08; }
};

STRUCT_1 Frm0A {
	byte type;
	byte full;
	Frm0A() { type = 0x0a; }
};

STRUCT_1 Frm60 {
	byte type;
	byte reply;
	byte speed;
	Frm60() { type = 0x60; }
};

STRUCT_1 Frm64 {
	byte type;
	byte reply;
	ushort num;
	byte id_len;
	byte id[8];
	Frm64() { type = 0x64; }
};

STRUCT_1 FrmA2 {
	byte type;
	byte reply;
	FrmA2() { type = 0xA2; }
};

STRUCT_1 FrmA4 {
	byte type;
	byte reply;
	FrmA4() { type = 0xA4; }
};

STRUCT_1 FrmA6 {
	byte type;
	byte flag;
	byte reply;
	FrmA6() { type = 0xA6; }
};

STRUCT_1 FrmA8 {
	byte type;
	byte flag;
	byte reply;
	FrmA8() { type = 0xA8; }
};

STRUCT_1 FrmAA {
	byte type;
	byte reply;
	FrmAA() { type = 0xAA; }
};

// STRUCT_1 FrmAE {
// 	byte type;
// 	byte reply;
// 	byte info[100];
// 	FrmAE() { type = 0xAE; }
// };
STRUCT_1 FrmAE {
	byte type;
	byte reply;
	FrmAE() { type = 0xAE; }
};


STRUCT_1 FrmCC {
	byte type;
	char ver[100];
	FrmCC() { memset(this, 0, sizeof(*this)); type = 0xCC; }
};

STRUCT_1 FrmE4 {
	byte type;
	DWORD id_car;
	char key_car[16];
	DWORD ip;
	ushort port;
	DWORD ip2;
	ushort port2;
	FrmE4() { type = 0xE4; }
};

STRUCT_1 FrmE5 {
	byte type;
	byte reply;
	DWORD id;
	FrmE5() { type = 0xE5; }
};

STRUCT_1 FrmE7 {
	byte type;
	byte reply;
	DWORD id_car;
	char key_car[16];
	DWORD ip;
	ushort port;
	DWORD ip2;
	ushort port2;
	FrmE7() { type = 0xE7; }
};

STRUCT_1 FrmEB {
	byte type;
	byte reply;
	FrmEB() { type = 0xEB; }
};

STRUCT_1 FrmED {
	byte type;
	byte reply;
	FrmED() { type = 0xED; }
};	

STRUCT_1 FrmEE {
	byte type;
	byte flag;
	FrmEE() { type = 0xEE; }
};

STRUCT_1 FrmOil{
	byte type;
	byte flag;
};

STRUCT_1 FrmC1{
	byte type;
	byte flag;
	FrmC1(){ type = 0xC1; }
};

STRUCT_1 UserSM {
	char mytel[11];		//手机号码
	
	char add1;
	char ip1[12];		//千里眼TCP IP
	
	char add2;
	char port1[5];		//千里眼TCP Port
	
	char add3;
	char ip2[12];		//千里眼UDP IP
	
	char add4;
	char port2[5];		//千里眼UDP Port
	
	char add5;
	char ip3[12];		//视频上传UDP IP
	
	char add6;
	char port3[5];		//视频上传UDP Port
	
	char add7;
	char ip4[12];		//流媒体UDP IP
	
	char add8;
	char port4[5];		//流媒体UDP Port
	
	char add9;
	char ip5[12];		//管理平台TCP IP
	
	char add10;
	char port5[5];		//管理平台TCP Port
	
	char add11;
#if USE_LIAONING_SANQI == 0
		char apn[20];		//APN
#endif
#if USE_LIAONING_SANQI == 1
		char apn[41]; //含集团计费号以 ；（分号）做为分隔符
#endif
};

STRUCT_1 UserSM2 {
	char pass[10];		// 密码
	char cmd[6];		// 命令
};

//--------用于呼叫限制------------------------
// 电话记录
STRUCT_1 Book {
	byte idx;		//索引号
	char tel[15];	//电话号码
	char name[20];	//姓名
	byte limit;		//权限设置
	byte tmhold[2];	//通话时长
};	

// 电话号码配置
STRUCT_1 BookCfg {
	Book book[30];		//电话记录	
	char tel[5][15];	//电话号码
};
//-----------------------------------------------

#pragma pack()

void (*G_Signal(int signo, void(* func)(int v_signo)))(int);

#endif	//#define _COMUEXEDEF_H_
