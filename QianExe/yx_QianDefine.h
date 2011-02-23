#ifndef _YX_QIANDEFINE_H_
#define _YX_QIANDEFINE_H_

enum TIMERSNEW
{
	// CMonAlert
	MON_TIMER = 1,
	AUTO_REQLSN_TIMER,
	ALERT_SILENT_TIMER,
	ALERT_INVALID_TIMER,
	ALERT_TEST_TIMER,
	ALERT_INITQUERY_TIMER,

	// CRealPos
	POS_TIMER,
		
	// CBlk
	BLACK_TIMER,
	BLACK_RCVQUERY_TIMER,
	BLACK_SNDRES_TIMER,
		
	// CAutoRpt
	AUTO_RPT_TIMER,
		
	// CHelp
	AUTODIALHELPTEL_TIMER,
		
	// CComu
	COPYDATA_TIMER,
	CHKDIAODU_TIMER,

	// CSock
	TCP_LOG_TIMER,
	UDP_LOG_TIMER,
	TCP_RECONN_TIMER,
	SHOW_MSG_TIMER,
		
	//CIO
	ACCCHK_TIMER,

	// CDrvRecRpt
	DRVRECWIN_REUPLOAD_TIMER,
	DRVREC_STOPUPLOAD_TIMER,

	// CDriverCtrl
	DRIVERLOG_TIMER,
	ACCOFF_DRVRRELOG_TIMER,
	FSTDRIVERLOG_TIMER,
	
	// CDriveRecord
	DRIVERECORD_TIMER,

	// CAcdent
	ACDUPLOAD_TIMER,
	
	//KTMeterMod
	DOWN_TIMER,
	SETDEVICE_TIMER,
	
	//KTLED
	CLEARFRONT_TIMER,
	CLEARBACK_TIMER,
	SETLEDSTA_TIMER,
	
	//KTDriverCtrl
	RESNDPIC_TIMER,
	RESPIC_TIMER,
	
	//CDiaodu
	SETBESPOKE_TIMER,
	BESPOKENOT_TIMER,

	// CPhoto
	TIMERPHOTO_TIMER,
	UPLOAD_PHOTOWIN_TIMER,
	UPLOAD_PHOTOCHK_TIMER,

	// COil
	AUTO_RPTOIL_TIMER,

	// CAutoRptStation
	SET_CHECI_TIMER,

	// CDownLineFile
	SEND4353_TIMER,

	// CLN_Login
	REGISTER_TIMER,
	LOGIN_TIMER,
	HEART_KEEP_TIMER,
	
	// CLN_Report
	ALERT_REPORT_TIMER,
	AUTO_REPORT_TIMER,
	SPEED_CHECK_TIMER,
	AREA_CHECK_TIMER,
	STARTENG_CHECK_TIMER,
	SPEEDRPT_CHECK_TIMER,
	PARKTIME_CHECK_TIMER,
	OUTLINE_CHECK_TIMER,
	TIRE_CHECK_TIMER,
	MILE_CHECK_TIMER
};

enum ALERM_TYPE_0149
{
	ALERM_0149_1 = 1,	//01	怠速报警
	ALERM_0149_2,		//02	碰撞报警
	ALERM_0149_3,		//03	侧翻报警
	ALERM_0149_4,		//04	拖车报警
	ALERM_0149_5,		//05	多边形电子围栏报警
	ALERM_0149_6,		//06	偏航报警
	ALERM_0149_7,		//07	分段限速报警
	ALERM_0149_8,		//08	停靠点报警
	ALERM_0149_9,		//09	停车超时报警
	ALERM_0149_10,		//10	震动报警(苏州天泽侧翻报警)
	ALERM_0149_11,		//11	非法开车门报警
	ALERM_0149_12,		//12	规定时间段外行驶报警
	ALERM_0149_13,		//13	超速报警
	ALERM_0149_14,		//14	越界报警
	ALERM_0149_15,		//15	低压报警
	ALERM_0149_16,		//16	断路报警
	ALERM_0149_17,		//17	抢劫报警
	ALERM_0149_18,		//18	测试报警
	ALERM_0149_19,		//19	GPS天线异常报警
	ALERM_0149_20,		//20	车台与仪表通信异常报警
	ALERM_0149_21,		//21	电子锁1被打开
	ALERM_0149_22,		//22	电子锁2被打开
	ALERM_0149_23,		//23	电子锁1盖子被打开  也是断线
	ALERM_0149_24,		//24	电子锁2盖子被打开  也是断线
	ALERM_0149_25,		//25	非法启动报警
};

enum DEV_STATE
{
	KT_METER=0x0001,
	KT_LED=0x02,
	KT_TOUCH=0x04,
	KT_PHOTO=0x08,
};

enum SMS_CODECVT
{
	CVT_NONE = 1,
	CVT_8TO6,
	CVT_8TO7
};

enum
{
	LED_CMD_INIT = 66,
	LED_CMD_LIGHT,
	LED_CMD_ALERM,
	LED_CMD_ALERMCANCEL,
	LED_CMD_CARMOVE,
	LED_CMD_CARSILENT
};

enum LEDSTATE
{
	stINIT = 0,
	stLOG,
	stWORK,
	stEXCP,
	stDOWN
};

enum ENU_RPTJUGRESULT	// 主动上报使能判断结果
{
	RPT_YES = 188,
	RPT_CLOSE,		// 上报被关闭
	RPT_OUTPRID,	// 不在上报时间段内
};

enum TRANS_MODE
{
	MODE_A = 1,
	MODE_B,
	MODE_C,
	MODE_D,
	MODE_E
};
const	double		LONLAT_DO_TO_L = 1000000.0;

#define INVALID_NUM_32	0xffffffff
#define INVALID_NUM_16	0xffff
#define INVALID_NUM_8	0Xff

#define LEDALARM_PRID			60	// LED显示报警的时长,秒

// socket发送队列数据类型
const BYTE DATASYMB_SMSBACKUP = 0x01; // 允许使用短信备份
const BYTE DATASYMB_USESMS = 0x02; // 强制使用短信发送
const BYTE DATASYMB_415052 = 0x08;
const BYTE DATASYMB_0154 = 0x10;
const BYTE DATASYMB_OE60 = 0x20;
const BYTE DATASYMB_DONETCPFRAME = 0x80;

// sms 数据类型
const BYTE DATASYMB_RCVFROMSM = 61; // 从短信读到的SMS封装的数据
const BYTE DATASYMB_RCVFROMSOCK = 62; // 从sokcet读到的SMS封装数据

// Benq模块
const double BENQ_SOCKSND_DELAYMULTI = 11.2 / 6.0; // 套接字发送时底层驱动的耗时倍数
const DWORD BENQ_SOCKSND_MAXBYTES_PERSEC = 900; // 每秒允许发送的最大字节数 (用于流量控制) (要考虑底层驱动发送时还要增加字节)
// SIMCOM300D模块
const double SIM300D_SOCKSND_DELAYMULTI = 7.5 / 6.0; // 套接字发送时底层驱动的延迟倍数
const DWORD SIM300D_SOCKSND_MAXBYTES_PERSEC = 1800; // 每秒允许发送的最大字节数 (用于流量控制,) (要考虑底层驱动发送时还要增加字节)

#define TCPSNDFAIL_MAXTIME	2 // TCP发送失败多少次后(但Socket未出错),UDP让TCP先发
#define UDPSNDBYTES_SNDTCP 200 // UDP连续发送多少字节后,优先发送TCP数据

// #define PHOTODISK	ResFlh3
// #define BLKDISK		ResFlh2

//#define DELPHOTO_MAXCOUNT	100

//#define ONEPHOTODIR_DAYS 4 // 每个月内,每多少天保存一个照片目录

//#define PHOTO_MAXCOUNT 10 // 每次拍照时的最大拍照次数 (大一点保险)
#define PHOTOQUEE_MAXCOUNT 40 // 拍照处理队列的大小
#define UPLOADPHOTO_MAXCOUNT 50 // 照片上传的缓冲区
#define UPLOADPHOTO_WINFRAMETOTAL	16 // 上传照片时,多少包为一个窗口
#define REUPLOADPHOTO_TIME 4 // 重传照片信息的最大次数

const double BLKSAVE_MINSPEED = 0.5; // 米/秒

const double MOVE_MINSPEED_GPS = 0.5; // 认为是移动的最小速度,米/秒 (GPS方式)
const double MOVE_MINSPEED_IO = 0.25; // 认为是移动的最小速度,米/秒 (IO方式)

const double MIMIAO_TO_HAIHOUR = 3.6 / 1.852;

const UINT AUTO_REQLSN_LONGINTERVAL		= 30000; // 主动拨打报警监听电话的长间隔 (毫秒) (因为底层判断占线或者被手机挂断等情况需要比较久的时间)
const UINT AUTO_REQLSN_SHORTINTERVAL	= 5000; // 主动拨打报警监听电话的短间隔 (毫秒) (手机模块忙时的短尝试间隔)


const DWORD QUEUE_RCVSMS_SIZE = 10000;
const DWORD QUEUE_TCPSND_SIZE = 200000;
const DWORD QUEUE_UDPSND_SIZE = 200000;
const DWORD QUEUE_ACDENT_SIZE = 10000;
const DWORD QUEUE_WAITSNDDRVREC_SIZE = 500000;

const DWORD QUEUE_RCVSMS_PERIOD = 60000; // ms
const DWORD QUEUE_TCPSND_PERIOD = 180000;
const DWORD QUEUE_UDPSND_PERIOD = 180000;
const DWORD QUEUE_ACDENT_PERIOD	= 600000;
const DWORD QUEUE_WAITSNDDRVREC_PERIOD = 0xfffffff8;

const DWORD	QUEUE_COMRCV_SIZE		= 10000;
const DWORD	QUEUE_COMRCVALID_PERIOD	= 60000;

const UINT MON_MININTERVAL	= 1000; // 用于定距监控时 (毫秒)
const UINT ALERT_SNDDATA_INTERVAL	= 30000;	// 默认报警发送监控数据间隔 (毫秒)
const UINT ALERT_SNDDATA_PERIOD		= 180000;	// 默认报警发送监控数据时长 (毫秒)
const UINT ALERT_SILENT_PERIOD		= 60000 * 600; // 报警处于静默状态的时长 (毫秒)
const UINT ALERT_INVALID_PERIOD		= ALERT_SNDDATA_PERIOD + 5000; // 报警后多少时间报警信号清除 (毫秒)
const UINT ALERT_TEST_PERIOD		= 70000; // 报警测试等待时长 (毫秒)
const UINT POS_INTERVAL				= 1000; // 用于实时定位 (毫秒)
const UINT BLACK_MININTERVAL		= 30; // 黑匣子最小保存间隔 (秒 特别注意!!!!)
const UINT BLACK_SAVEINTERVAL		= 300000; // 黑匣子数据实际写入间隔 (毫秒)
const UINT BLACKQUERY_MAXINTERVAL	= 60000; // 黑匣子查询帧下传最大允许间隔 (毫秒)
const UINT BLACK_SNDINTERVAL		= 1000; // 定时上传黑匣子数据的间隔 (毫秒)
//const UINT FOOTALERTVALID_INTERVAL	= 1000;
//const UINT POWALERTVALID_INTERVAL	= 1000;
const UINT FOOTALERTVALID_PERIOD	= 1800; // 按住抢劫报警开关多长时间以上发出报警 (毫秒)
const UINT POWALERTVALID_PERIOD		= 2000; // 收到断电信号后多长时间以上发出报警 (毫秒)
const UINT ACCCHK_INTERVAL			= 1200000;
const UINT COPYDATA_INTERVAL		= 3000;

const UINT FIRSTPHOTO_INTERVAL		= 2000; // 第一张照片的延时 (一般的摄像头可以少一点,xun,070828)
const UINT PHOTO_INTERVAL			= 500; // 拍照间隔 (毫秒)
const UINT UPLOAD_PHOTOWIN_INTERVAL	= 10000; // 照片每窗口数据重发索引定时间隔 (毫秒) (普通间隔)
const UINT CENTPHOTORES_PERIOD		= 3000; // 一个窗口发送后等待中心应答的时间 (毫秒)
const UINT JIJIAPHOTO_MININTERVAL	= 2500; // 计价器触发拍照,至少间隔多少时间 (毫秒)
const UINT DOORPHOTO_INTERVAL		= 1000; // 定时检查是否要进行关车门拍照的间隔 (ms)

const UINT ONEPHOTO_MAXPERIOD		= 18000; // 拍每张照片的最大时长 (毫秒)
const UINT ONEPHOTOUPLOAD_MAXPERIOD	= 10000 + UPLOAD_PHOTOWIN_INTERVAL;
const UINT ALERTFSTPHOTO_DELAYPERIOD= 2500; // 报警后延时多少时间拍照,以保证第一个报警数据能发送出去 (毫秒)

const UINT ALERTREQLSN_DEALYPERIOD	= 5000; // 报警后多少时间开始启动主动监听流程 (毫秒)

const DWORD BELIV_DATE	= 80801; // 此后的日期认为是有效的,用于保存照片的目录名
const DWORD UNBLV_DATE	= 10101; // 若日期无效,统一视为该值,用于保存照片的目录名

const DWORD CENTLOCKON_DOORMSGPERIOD = 10000; // 锁总控后，会收到多长时间的车门开关信号，毫秒

//const DWORD ALERT_FOOT_TM_SHAKE	= 1505;	// 从触发报警开始,报警触发抖动时间，毫秒
//const DWORD ALERT_TM_ACTIVE		= 182000; // 从触发报警开始,默认的报警活动时间,毫秒 (加上一个误差延时值)
//const DWORD ALERT_TM_SILENT		= 30 * 60 * 1000 + ALERT_TM_ACTIVE; // 从触发报警开始,默认的报警静默时间

const WORD	PHOTOINDEX_MAX		= 65520; // 照片最大索引号 (从1开始的，应小于下面的无效索引号)
const WORD	PHOTOINDEX_INVALID	= 65535; // 照片的无效索引号

const BYTE	PHOTO_QUALITYHIGH	= 55;
const BYTE	PHOTO_QUALITYMID	= 40;
const BYTE	PHOTO_QUALITYLOW	= 20;

const BYTE ALERT_STOP_PHONE_PRD = 255 ;//单位：s；0-255

//const BYTE	PHOTO_UPLOAD_TIME	= 40; // 每张照片的估计上传耗时(秒)

#define AUTO_REQLSN_MAXTIMES	3 // 最大主动监听次数
#if USE_LIAONING_SANQI == 0	
#define POS_CONTCOUNT 20 // 实时定位条件(速度、范围条件)连续多少个GPS数据满足后进行处理

#endif
#if USE_LIAONING_SANQI == 1						 //
#define POS_CONTCOUNT 10 // 实时定位条件(速度、范围条件)连续多少个GPS数据满足后进行处理
#endif
#define SPD_CONTCOUNT 5

#define TIMER_MAXPERIOD	300 // 每次定时处理所耗费的最大允许时间(毫秒),超过则下次定时器再处理。例如：黑匣子上传、事故疑点上传等

// 报警状态类型
#define	ALERT_TYPE_FOOT		    0x00000001		// 抢劫报警
#define ALERT_TYPE_POWOFF	    0x00000002		// 主电断电
#define ALERT_TYPE_POWBROWNOUT	0x00000004	// 主电欠压
#define ALERT_TYPE_FRONTDOOR	0x00000008		// 非法开前车门
#define ALERT_TYPE_OVERSPEED	0x00000010		// 超速报警
#define ALERT_TYPE_DRIVE	    0x00000020		// 非法启动车辆
#define ALERT_TYPE_TIME		    0x00000040		// 时间段外行驶
#define ALERT_TYPE_RANGE	    0x00000080		// 越界
#define ALERT_TYPE_RSPEED	    0x00000100		// 区域超速
#define ALERT_TYPE_OVERTURN     0x00000200		// 侧翻
#define ALERT_TYPE_BUMP		    0x00000400		// 碰撞
#define ALERT_TYPE_BACKDOOR		0x00000800		// 非法开后车门
#define ALERT_TYPE_BELOWSPEED	0x00001000		// 低速报警
												//
//辽宁车务通
#define ALERT_TYPE_STARTENG		0x00002000		//点火报警
#define ALERT_TYPE_MILE			0x00004000		//里程报警
#define ALERT_TYPE_PARKTIMEOUT	0x00008000		//超时停车
#define ALERT_TYPE_AREA			0x00010000		//区域报警
#define ALERT_TYPE_TIMESPD		0x00020000		//时间段速度异常
#define ALERT_TYPE_OUTLINE		0x00040000		//偏航报警
#define ALERT_TYPE_TIRE			0x00080000		//疲劳报警
#define ALERT_TYPE_TIMERESPD	0x00100000		//分时段区域超速报警
#define ALERT_TYPE_TIMEOUTLINE	0x00200000		//分时段偏航报警
#define ALERT_TYPE_REPATIMEOUT  0x00400000		//区域停车超时报警
												//
#define ALERT_TYPE_INAREA		0x00800000		//进区域报警
#define ALERT_TYPE_OUTAREA		0x01000000		//出区域报警
												//
#define ALERT_TYPE_INREGION		0x02000000		//区域内报警
#define ALERT_TYPE_OUTREGION	0x04000000		//区域外报警
												//
#define ALERT_TYPE_INRESPD		0x08000000		//区域内超速
#define ALERT_TYPE_OUTRESPD		0x10000000		//区域外超速												
#define ALERT_TYPE_INREPATOUT	0x20000000		//区域内超时停车
#define ALERT_TYPE_OUTREPATOUT	0x40000000		//区域外超时停车


#define ALERT_TYPE_TEST			0x80000000		// 报警测试

// IO模块中的设备状态类型 (仅用于一次性的IO信号,或者事后无法查询的IO信号或设备控制指令)
#define DEVSTA_LOCKENABLE	0x00000001 // 中控锁锁上/没锁
#define DEVSTA_ACCVALID		0x00000002 // ACC有效/无效
#define DEVSTA_GPSON		0x00000004 // GPS上电/关电
#define DEVSTA_JIJIADOWN	0x00000008 // 计价器翻下/翻上 (仅在计程线使用时)
#define DEVSTA_SYSTEMSLEEP	0x00000010	// 系统省电

// Comu模块中的状态类型
#define COMUSTA_DIAODUCONN	0x00000001 // 调度屏是否通信正常/异常

// 车辆行驶状态 (仅用于没有单独的模块能提供准确查询的状态)
#define VESTA_MOVE		0x00000001 // 移动/静止
#define VESTA_HEAVY		0x00000002 // 重车/空车

#define DISCONNOILELEC_PERIOD 30000 // 断油断电动作持续时长 (毫秒)
#define DISCONNOILELEC_DELAY_PERIOD 10000 // 提示断油断电后多久执行 (毫秒)

// 照片索引文件名
#define CAMINDEXNAME	"Index.dat"

#define BLKREADUNIT		128 // 从黑匣子文件中每次读取数据的最大个数 (最好使得读取的字节数是簇大小的倍数,一个黑匣子数据16字节)
#define BLKSNDUNIT		5 // 每次发送黑匣子数据的最大个数 (考虑到短信的字节数限制,5是上限了)

#define DSK2_ONEFILE_DAYS	8 // 每个月内,每多少天保存一个黑匣子文件或事故疑点文件 (第2分区中)

const DWORD DISK2_RESERVE	 = 512000;//DISK2_CUSIZE * 500;
const DWORD DISK3_RESERVE	 = 5120000;//DISK3_CUSIZE * 2500;

// 黑匣子最大文件大小(一个黑匣子数据16Bytes,30秒保存一次,8天一个文件): 16 * 2 * 60 * 24 * 8 = 368,640
const DWORD BLK_MAXSIZE		= 16 * 20 * 60 * 24 * DSK2_ONEFILE_DAYS; // 8天一个文件=3,686,400
const DWORD PHOTO_MAXSIZE	= 204800;
const DWORD ACD_MAXSIZE = 376 * 1000 * DSK2_ONEFILE_DAYS; // 8天一个文件: 3,008,000

const BYTE DATASYMB_DIAODUTOQIAN = 0x01;
const BYTE DATASYMB_PHONETOQIAN = 0x02;
const BYTE DATASYMB_GPSTOQIAN = 0x04;


//-----------------------------------------------------------------------
//LED相关
#define MAILCNT_BOHAI		252					//博海信箱总数
#define RENEWMENUTICK		30*60*1000			//30*60*1000 //更新节目单定时
#define EXP1TICKMAX			20*1000				//普通异常恢复定时,秒
#define DOWNWAITTICK		10*60*1000			//重传下载定时
#define UPDDERRTICK			30*60*1000			//升级错误定时
#define ERRTICK				10*60*1000			//LED通信错误

#define PKT_BOHAI			512					//博海升级数据包大小
#define RESEND_BOHAI		4*1024/PKT_BOHAI	//升级重传包数
#define UPDTCNT_BOHAI		64*1024/PKT_BOHAI	//博海升级程序总包数
#define VERCNT_BOHAI		8					//博海版本长度

#define ALABUF				1500				//收发缓冲区

#define DOWNTIME1			01					//车台主动请求节目单时间段1
#define DOWNTIME2			02					//车台主动请求节目单时间段2

#define LOG_MAX				8000				//LED相关日志的最大条数				

//------------------------------------------------------------------------
//LED编译开关
//#define SAVE_LEDDATA							//是否记录LED过程数据
//#define LEDTIMEDOWN							//是否具有定时间段请求节目单功能
//#define DELAYLEDTIME							//是否将ACC OFF时的节目单存入配置区

enum MSG_TYPE
{ 
	MSG_SAVRECSTA = 1,
	MSG_DRVECDTOSND,
	MSG_TCPTOSEND,
	MSG_PROGQUIT,
	MSG_IOEVENT,
	MSG_PHOTODONE,
};

typedef struct tagMsg
{
	long int m_lMsgType;
	unsigned char m_uszMsgBuf[MSG_MAXLEN];
	
	tagMsg() { memset( this ,0, sizeof(*this) ); }
}QIANMSG;

#pragma pack(1)	// !!!!!特别注意内存1字节对齐带来的相关问题, 主要是可能不能直接访问结构体内的成员!!!!!

/// { 套接字层直接处理

// 车台->中心


// 中心->车台

/// } 套接字层直接处理


/// { 业务层处理
// GPS数据类型(1)+参考日期(3)+GPS数据(n)+GPS数据校验码(1)
// 标准GPS数据: Time(3)+Hbit(1)+Latitude(4)+Longitude(4)+Vector(1)+Direction(1)
STRUCT_1 tagQianGps // 已对齐
{
	// 日期是北京时间,时刻是格林威治时间
	BYTE		m_bytGpsType;
	BYTE		m_bytYear; // 2位数字表示的值
	BYTE		m_bytMon;
	BYTE		m_bytDay;
	BYTE		m_bytHour;
	BYTE		m_bytMin;
	BYTE		m_bytSec;
	BYTE		m_bytMix;	// 01AS0LGV: 
							// A：GPS数据有效位，0-有效；1-无效
							// L：车台负载状态，0－空车；1－重车
							// S：省电模式，0－非省电模式；1－省电模式
							// G：经度字段中度字节的最高位
							// V：速度字段最高位
	BYTE		m_bytLatDu;
	BYTE		m_bytLatFenzen;
	BYTE		m_bytLatFenxiao1;
	BYTE		m_bytLatFenxiao2;
	BYTE		m_bytLonDu;
	BYTE		m_bytLonFenzen;
	BYTE		m_bytLonFenxiao1;
	BYTE		m_bytLonFenxiao2;
	BYTE		m_bytSpeed; // 海里/小时
	BYTE		m_bytDir;
	BYTE		m_bytChk;

	tagQianGps()
	{
		memset( this, 0, sizeof(*this) );
	}
};

// 黑匣子GPS: 日期(3)+时间(3)+Hbit(1)+纬度(4)+经度(4)+速度(1)
STRUCT_1 tag0e60BlackGps // 已对齐,16 Bytes
{
	// 日期和时刻均是北京时间
	BYTE		m_bytYear; // 2位数字表示的值
	BYTE		m_bytMon;
	BYTE		m_bytDay;
	BYTE		m_bytHour;
	BYTE		m_bytMin;
	BYTE		m_bytSec;
	BYTE		m_bytMix;
	BYTE		m_bytLatDu;
	BYTE		m_bytLatFenzen;
	BYTE		m_bytLatFenxiao1;
	BYTE		m_bytLatFenxiao2;
	BYTE		m_bytLonDu;
	BYTE		m_bytLonFenzen;
	BYTE		m_bytLonFenxiao1;
	BYTE		m_bytLonFenxiao2;
	BYTE		m_bytSpeed; // 海里/小时
#if 2 == USE_PROTOCOL
	BYTE		m_bytIOSta; // 暂不使用
	BYTE		m_bytHeavyIO; // 暂不使用
#endif

	tag0e60BlackGps() { memset( this, 0, sizeof(*this) ); }
};

STRUCT_1 tag1007Interv
{
	BYTE		m_bytGpsType;
	BYTE		m_bytHourInterv;
	BYTE		m_bytMinInterv;
	BYTE		m_bytHunKilo;
	BYTE		m_bytKilo;
};

STRUCT_1 tag1007Time
{
	BYTE		m_bytNo;
	BYTE		m_bytHour;
	BYTE		m_bytMin;
};

STRUCT_1 tag1007Area
{
	BYTE		m_bytAreaProCode;
	BYTE		m_bytLonMix;
	BYTE		m_bytLonWidFenZhen;
	BYTE		m_bytLonWidFenxiao1;
	BYTE		m_bytLonWidFenxiao2;
	BYTE		m_bytLatMix;
	BYTE		m_bytLatHeiFenZhen;
	BYTE		m_bytLatHeiFenxiao1;
	BYTE		m_bytLatHeiFenxiao2;
	BYTE		m_bytLonDu;
	BYTE		m_bytLonFenzen;
	BYTE		m_bytLonFenxiao1;
	BYTE		m_bytLonFenxiao2;
	BYTE		m_bytLatDu;
	BYTE		m_bytLatFenzen;
	BYTE		m_bytLatFenxiao1;
	BYTE		m_bytLatFenxiao2;
};

// 对应的协议：状态标识（1）＋监控业务类型（1）＋ 监控时间（2）＋ 监控周期（2）＋ 定距距离（2）
STRUCT_1 tag103bMonPar
{
	BYTE	m_bytStaType;
	BYTE	m_bytMonType;
	BYTE	m_bytMonTimeHour;
	BYTE	m_bytMonTimeMin;
	BYTE	m_bytMonIntervMin;
	BYTE	m_bytMonIntervSec;
	BYTE	m_bytMonSpaceKilo;
	BYTE	m_bytMonSpaceTenmeter;
};

// 行驶记录 (内存中使用,同时可作为文件部分内容的格式,从司机编号至HDOP的定义正好又可作为协议中的部分内容)
// 该结构体内最好都使用单字节的类型,因为协议相关的数据定义要求1字节对齐
// 该结构体的定义勿轻易改变,因为对程序中在拷贝数据时计算拷贝位置和拷贝大小时有影响
STRUCT_1 tagSampleData_3745_KJ2
{
	BYTE	m_bytYear;
	BYTE	m_bytMon;
	BYTE	m_bytDay;
	BYTE	m_bytHour;
	BYTE	m_bytMin;
	BYTE	m_bytSec;
	char	m_szDriverNo[2]; // 司机编号,htons( WORD )
	char	m_szLat[4];
	char	m_szLon[4];
	BYTE	m_bytSpeed; // 即时速度,海里/小时
	BYTE	m_bytDir;
	BYTE	m_bytIO;
	BYTE	m_bytSta1;
	BYTE	m_bytSta2;
	BYTE	m_bytPDop; // GPS误差因子×10,下同
	BYTE	m_bytVDop;
	BYTE	m_bytHDop;
};

STRUCT_1 tagSampleData_3746
{
	BYTE	m_bytYear;
	BYTE	m_bytMon;
	BYTE	m_bytDay;
	BYTE	m_bytHour;
	BYTE	m_bytMin;
	BYTE	m_bytSec;
	char	m_szDriverNo[2]; // 司机编号,htons( WORD )
	char	m_szLat[4];
	char	m_szLon[4];
	BYTE	m_bytSpeed; // 即时速度,海里/小时
	BYTE	m_bytDir;
	BYTE	m_bytIO;
	BYTE	m_bytSta1;
	BYTE	m_bytSta2;
	BYTE	m_bytAlertIO;
	BYTE	m_bytHeightCount; // 载重传感器个数,暂且=0
	WORD	m_aryHeightIO[4]; // 载重传感器数组,有限个数,时用时,因为内存对齐要求,必须使用memcpy,不要用=赋值!!!
};

typedef struct tagSampleData_3746 tagSampleData_3747;

STRUCT_1 tag4144PhotoIndex
{
	BYTE	m_bytEvtType;
	WORD	m_wIndex;
	char	m_szDateTime[6];
};

/// 调度数据
STRUCT_1 tag0501Data {
	BYTE	m_bytLen;			//数据长度
	BYTE	m_bytCheck;			//校验和
	BYTE	m_szRouter[4];		//路由信息
	BYTE	m_bytDataType;		//数据类型 02为车辆指示
	char	m_szData[200];		//用户数据
	tag0501Data()	{ memset(this, 0, sizeof(*this)); }
};


STRUCT_1 tag0101
{
	BYTE	m_bytMonType;
	BYTE	m_bytTimeHour;
	BYTE	m_bytTimeMin;
	BYTE	m_bytIntervMin;
	BYTE	m_bytIntervSec;
	BYTE	m_bytSpaceKilo;
	BYTE	m_bytSpaceTenmeter;
#if 2 == USE_PROTOCOL
	BYTE	m_bytUpGpsHund;
	BYTE	m_bytUpGpsCount;
#endif
};

STRUCT_1 tag0103
{
#if QIANTYPE_USE == QIANTYPE_JG2 || QIANTYPE_USE == QIANTYPE_JG3 || QIANTYPE_USE == QIANTYPE_VM1 || QIANTYPE_USE == QIANTYPE_V7
	char	m_szHandtel[15];
#endif
	tag0101	m_objO101;
};

// struct tag0104
// {
// 	BYTE	m_bytAnsType;
// };

STRUCT_1 tag0105
{
	BYTE	m_bytCmdType;
	tag0101	m_obj0101;
};

STRUCT_1 tag0141
{
	BYTE	m_bytAnsType;
	tagQianGps	m_objQianGps;
};

// struct tag0142
// {
// 	BYTE	m_bytAnsType;
// };
// 
// struct tag0143
// {
// 	BYTE	m_bytAnsType;
// };
// 
// struct tag0144
// {
// 	BYTE	m_bytEndType;
// };

STRUCT_1 tag0145
{
	BYTE	m_bytAlertHi;
	BYTE	m_bytAlertLo;
	BYTE	m_bytMonCentCount;
	tagQianGps	m_objQianGps;
#if 2 == USE_PROTOCOL
	char		m_szAlertAreaCode[2];		
#endif

	tag0145()
	{
		memset( this, 0, sizeof(*this) );
#if 2 == USE_PROTOCOL
		m_szAlertAreaCode[0] = m_szAlertAreaCode[1] = 0x7f; // 默认区域编号为全0
#endif
	}
};
typedef struct tag0145 tag0154;

STRUCT_1 tag0149
{
	BYTE	m_bytAlertHi;
	BYTE	m_bytAlertLo;
	BYTE	m_bytMonCentCount;
	tagQianGps	m_objQianGps;

	tag0149()
	{
		memset( this, 0, sizeof(*this) );
	}
};

STRUCT_1 tag0151
{
	BYTE	m_bytAnsType;
	tagQianGps	m_objQianGps;
//	char	m_szStatus[2];

	tag0151()
	{
		memset(this, 0, sizeof(*this));
		//m_szStatus[0] = m_szStatus[1] = 1;
	}
};

// 区域查车请求
STRUCT_1 tag0136
{
	BYTE	m_bytQueryIndex;		//查询索引号
	BYTE	m_szMonId[8];			//监控终端ID
	BYTE	m_bytZoneCount;			//区域个数
	
	struct tagZone {				
		BYTE	hbit1;
		BYTE	lat1[4];
		BYTE	lon1[4];
		BYTE	hbit2;
		BYTE	lat2[4];
		BYTE	lon2[4];
	} m_Zone[256];					//区域

};

#if 2 == USE_PROTOCOL
STRUCT_1 tag0156
{
	BYTE	m_bytAlertHi;
	BYTE	m_bytAlertLo;
	BYTE	m_bytMonCentCount;
	tagQianGps	m_objQianGps;
	BYTE	m_bytDriverNo;
	char	m_szAlermAreaCode[2];

	tag0156()
	{
		memset( this, 0, sizeof(*this) );
		m_bytDriverNo = 0x7f;
		m_szAlermAreaCode[0] = m_szAlermAreaCode[1] = 0x7f;
	}
};
#endif

// 区域查车请求应答
STRUCT_1 tag0176
{
	BYTE	m_bytQueryIndex;		//查询索引号
	BYTE	m_szMonId[8];			//监控终端ID
	BYTE	m_bytZoneCount;			//区域个数
	
	BYTE	m_szCarStat[2];			//车辆状态
	tagQianGps m_objQianGps;		//标准GPS
};

STRUCT_1 tag0241
{
	BYTE	m_bytConType;
	BYTE	m_bytConNo;
	BYTE	m_bytAlertHi;
	BYTE	m_bytAlertLo;
	tagQianGps m_objQianGps;

	tag0241() { memset(this, 0, sizeof(*this)); }
};

STRUCT_1 tag0341
{
	BYTE	m_bytHelpType;
	tagQianGps m_objQianGps;
};

STRUCT_1 tag0e20 // 最大容量定义，不能按通常方式使用
{
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	char	m_szCentGroup[10];
#endif
	BYTE	m_bytMaxFrameCount;
	BYTE	m_bytPeriodCount; // 该字段前(包括本字段)可通常方式使用
	BYTE	m_szPeriod[ 10 * 5 ];
	BYTE	m_bytAreaCount;
	BYTE	m_szArea[ 16 * 20 ];
	BYTE	m_bytReqFrameTotal;
	BYTE	m_bytReqFrameNo; // 从1开始编号

	tag0e20() { memset(this, 0, sizeof(*this)); }
};

typedef struct tag0e20 tag0e10;

STRUCT_1 tag0e21
{
	char	m_szCentGroup[10];
};

struct tag0e60 // 最大容量定义，不能按通常方式使用
{
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
	char	m_szCentGroup[10];
#endif
	BYTE	m_bytResType;
	BYTE	m_bytResFrameNo; // 也从1开始编号 (但是协议没有说)
	BYTE	m_bytGpsCount; // 该字段前(包括本字段)可通常方式使用
	tag0e60BlackGps	m_aryBlackGps[BLKSNDUNIT];
	BYTE	m_bytFitAreaCount;
	BYTE	m_bytFitTotalHi;
	BYTE	m_bytFitTotalLo;

	tag0e60() { memset(this, 0, sizeof(*this)); }
};

typedef struct tag0e60 tag0e50;

STRUCT_1 tag0e61
{
	char	m_szCentGroup[10];
	BYTE	m_bytResType;
};

STRUCT_1 tag1001
{
	char	m_szSmsCentID[22];	// 短信中心号,不满填空格
};

STRUCT_1 tag1002
{
	char	m_szSmsTel[15];
	char	m_szTaxHandtel[15];
	char	m_szDiaoduHandtel[15];
};

STRUCT_1 tag1003 // 最大尺寸定义
{
	BYTE	m_bytLsnTelCount;
	char	m_aryLsnHandtel[5][15];
};

STRUCT_1 tag1007
{
	char	m_szHandTel[15];
	BYTE	m_aryControl[2];
	BYTE	m_bytCondCount;
	char	m_szCond[ 17 * 30 ];
};

STRUCT_1 tag1008 // 最大尺寸定义
{
	BYTE	m_bytSetCtrl;
	BYTE	m_bytTelCount;
	char	m_szTelInfo[16*16];
};

STRUCT_1 tag1012
{
	BYTE	m_bytMin;
	BYTE	m_bytSec;
};

// struct tag1042
// {
// 	BYTE	m_bytAnsType;
// };

STRUCT_1 tag103e
{
	BYTE	m_bytSndWinSize; // 窗口大小
#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4
	BYTE	m_bytOnSndHour; // 发送时长 (h)
	BYTE	m_bytOnSndMin; // 发送时长 (m)
#endif
	BYTE	m_bytOnSampCycleMin; // ACC ON 采样周期,分钟
	BYTE	m_bytOnSampCycleSec; // ACC ON 采样周期,秒
	BYTE	m_bytOnSndCycle; // ACC ON,发送周期,个
#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4
	BYTE	m_bytOffSndHour; // 发送时长 (h)
	BYTE	m_bytOffSndMin; // 发送时长 (m)
#endif
	BYTE	m_bytOffSampCycleMin; // ACC OFF 采样周期,分钟
	BYTE	m_bytOffSampCycleSec; // ACC OFF 采样周期,秒
	BYTE	m_bytOffSndCycle; // ACC OFF,发送周期,个

	tag103e() { memset( this, 0, sizeof(*this) );}
};

STRUCT_1 tag103f
{
	char	m_szSmsCentID[22];	// 短信中心号,不满填空格
	char	m_szUserIdentCode[6]; // 用户识别码	
};

STRUCT_1 tag105a
{
	BYTE	m_bytSta_1;
	BYTE	m_bytSta_2;
#if 2 == USE_PROTOCOL || 30 == USE_PROTOCOL
	BYTE	m_bytIO; // 传感器状态
#endif
	tagQianGps objQianGps;

	tag105a() { memset(this, 0, sizeof(*this)); }
};

STRUCT_1 tag107c
{
	BYTE	m_bytIPNo;
	BYTE	m_bytResType;
};

STRUCT_1 tag3341
{
	BYTE	m_bytAnsType;
	BYTE	m_bytFrameNo;
};

STRUCT_1 tag3644
{
	BYTE	m_bytRestype;
	BYTE	m_bytFrameNo;
	BYTE	m_bytType;
#if USE_ACDENT == 1
	BYTE	m_bytDriverNo;
#endif
#if USE_ACDENT == 2
	char	m_szDriverNo[2]; // htons( WORD )
#endif
	char	m_szStopTime[6];
	union
	{
		BYTE	m_arySpeedIO[50][2]; // (1字节speed + 1字节IO) * 50
		BYTE	m_aryLonlat[10][8]; // (4字节Lon + 4字节Lat) * 10
	} m_uniAcdentData;
};

STRUCT_1 tag3743
{
	BYTE	m_bytResType;
	BYTE	m_bytAreaNo;
};

STRUCT_1 tag3744
{
	BYTE	m_bytAreaNo;
	tagQianGps m_objQianGps;
};

STRUCT_1 tag4152
{
	BYTE	m_bytSymb;
	char	m_szIndex[2];
	char	m_szFrameTotal[2];
	char	m_szFrameNo[2];
	BYTE	m_bytReSndTime;
};

// 电话记录
STRUCT_1 Book {
	byte idx;		//索引号
	char tel[15];	//电话号码
	char name[20];	//姓名
	byte limit;		//权限设置
	byte tmhold[2];	//通话时长
};	

// 设置电话号码本请求
STRUCT_1 frm3301 {
	byte frmno;			//帧索引
	byte cnt;			//设置个数
	Book book[30];		//电话记录		
};

// 设置电话号码本应答
STRUCT_1 frm3341 {
	byte reply;			//应答类型
	byte frmno;			//帧索引
};

// 设置中心服务电话号码
STRUCT_1 frm3302 {
	byte cnt;			//个数
	char tel[5][15];	//电话号码
};

// 设置中心服务电话号码应答
STRUCT_1 frm3342 {
	byte reply;			//应答类型
};

// 电话号码配置
STRUCT_1 BookCfg {
	Book book[30];		//电话记录	
	char tel[5][15];	//电话号码
};

STRUCT_1 tag4216
{
	WORD m_usLindId;
	
	tag4216(){memset(this,0,sizeof(*this));}
};

STRUCT_1 tag4256
{
	WORD m_usLindId;
	byte m_bytRes;
	
	tag4256(){memset(this,0,sizeof(*this));}
};

STRUCT_1  tag4246 // 滞站抛站数据
{
	WORD	usLineNo;
	WORD	usStaNo;
	WORD	bytYear;
	byte	bytMon;
	byte	bytDay;
	byte	bytHour;
	byte	bytMin;
	byte	bytSec;
	byte	bytAlemType;
	//		DWORD	usTmspan;
	byte    bytTmspanMin;
	byte    bytTmspanSec;
	
	tag4246()
	{
		memset(this,0,sizeof(*this) );
	}
}; 

STRUCT_1  tag4247 // 到离站数据
{
	WORD	usLineNo;
	WORD	usStaNo;
	WORD	usYear;
	byte	bytMon;
	byte	bytDay;
	byte	bytHour;
	byte	bytMin;
	byte	bytSec;
	byte	bytIsInOut;
	DWORD	dwMeterTotle;
	
	tag4247()
	{
		memset(this,0,sizeof(*this) );
	}
}; 

STRUCT_1  tag4274 // 到离站数据
{
	WORD	usLineNo;
	WORD	usStaNo;
	WORD	usYear;
	byte	bytMon;
	byte	bytDay;
	byte	bytHour;
	byte	bytMin;
	byte	bytSec;
	byte	bytIsInOut;
	DWORD	dwMeterTotle;
	
	tag4274()
	{
		memset(this,0,sizeof(*this) );
	}
}; 

STRUCT_1  tag4248 // 客流数据
{
	WORD	usLineNo;
	WORD	usStaNo;
	byte	bytYear;
	byte	bytMon;
	byte	bytDay;
	byte	bytHour;
	byte	bytMin;
	byte	bytSec;
	ushort  usFup;
	ushort  usFdw;
	ushort  usBup;
	ushort  usBdw;
	
	tag4248()
	{
		memset(this,0,sizeof(*this) );
	}
}; 

STRUCT_1 tag8ff1
{
	BYTE	m_bytTranType;
	BYTE	m_bytDataType;
	char	m_szContent[200];
	
	tag8ff1()
	{
		memset( this, 0, sizeof(*this));
		m_bytTranType = 0x8f, m_bytDataType = 0xf1;
	}
};

STRUCT_1 tag4254
{
	BYTE	m_bytAlertHi;
	BYTE	m_bytAlertLo;
	BYTE	m_bytMonCentCount;
	tagQianGps	m_objQianGps;
	ushort  m_usLineId;  //	当前线路号.
	// A7 A6 A5 A4 A3 A2 A1 A0
	//A1A0:01:下行 10 上行 00 不确定, A3: 有没有开启主动上报
	BYTE    m_bytBusSta; //上行下行
	//车次类型(1):
	//0x01: 出场  0x02: 回场 0x03: 加油 0x04 保养 0x05: 修车  0x06: 包车 0x07: 放空 0x08 其他
	BYTE    m_bytReserve;   //司机编号
	ulong   m_dwMeter;    // 总里程
	BYTE    m_bytCheCi;
	//有新的字段放在最后
	byte    m_bytSatInUse;//在使用的卫星数
	byte    m_bytHDOP;
	ulong   m_ulDriverId; //司机编号
	
	tag4254()
	{
		memset( this, 0, sizeof(*this) );
	}
};

STRUCT_1  tag4310
{
	char  strVersion[6];
	
	tag4310(){memset(this,0,sizeof(*this));}
}; 

STRUCT_1  tag4312
{
	char  strVersion[6];
	DWORD dwPackCnt;
	byte  usWinsize;
	char  m_md5[32];
	
	tag4312(){memset(this,0,sizeof(*this));}
}; 

STRUCT_1  tag4313
{
	byte  bNeedAns;
	DWORD ulPackIndex;
	WORD  usPackSize;//除了最后一包，其他的包大小都是1K.
	byte  aryLineDat[1024];
	
	tag4313(){memset(this,0,sizeof(*this));}
}; 

STRUCT_1  tag4350
{
	char  strVersion[6];
	byte  bSucc;
	
	tag4350(){memset(this,0,sizeof(*this));}
}; 

STRUCT_1  tag4352
{
	char  strVersion[6];
	byte  ucFlag;
	
	tag4352(){memset(this,0,sizeof(*this));}
}; 

STRUCT_1  tag4353
{
	char  strVersion[6];
	DWORD ulWinIndex;
	DWORD ulWinStatus;
	
	tag4353(){memset(this,0,sizeof(*this));}
	};

/// } 业务层处理

#pragma pack()


#pragma pack(4)

struct tagMonStatus
{
	double	m_dLstLon; // 上次检查时的经纬度
	double	m_dLstLat;

	double	m_dLstSendDis; // 上次发送监控数据之后累积行驶的距离 (米)
	
	DWORD	m_dwLstSendTime; // 上次发送监控数据的时刻 (毫秒)

	DWORD	m_dwChkInterv; // 检查间隔 (毫秒), 监控定时器的时间定时——如果有定距更新,使用默认值,否则,使用该值

	DWORD	m_dwMonTime; // 剩余监控时间 (毫秒, 0xffffffff表示永久监控)
	DWORD	m_dwLstChkTime; // 上次进行判断的时刻 (毫秒)

	DWORD	m_dwSpace; // 定距监控间隔 (米)
	DWORD	m_dwMonInterv; // 定时监控间隔 (毫秒)

	DWORD	m_dwGpsCount; // 剩余条数 (0xffffffff,表示不限条数)
	char	m_szMonHandTel[15]; // 监控数据要发往的手机号 (走短信通道时有效) (暂时不用)
	bool	m_bInterv; // 是否定时
	bool	m_bSpace; // 是否定距
	bool	m_bAlertAuto; // 是否因为报警

	tagMonStatus()
	{
		Init();
	}

	void Init()
	{
		memset(this, 0, sizeof(*this));
		memset( m_szMonHandTel, 0x20, sizeof(m_szMonHandTel) );
		m_dwChkInterv = m_dwMonInterv = 0xffffffff; // 安全起见啊
		m_dwMonTime = 0;
		m_dwGpsCount = 0;
	}
};

// 与协议中的格式相同
struct tagBlackQueryPeriod
{
	BYTE	m_bytFromYear;
	BYTE	m_bytFromMon;
	BYTE	m_bytFromDay;
	BYTE	m_bytFromHour;
	BYTE	m_bytFromMin;
	BYTE	m_bytToYear;
	BYTE	m_bytToMon;
	BYTE	m_bytToDay;
	BYTE	m_bytToHour;
	BYTE	m_bytToMin;
};

struct tagBlackQueryArea
{
	long	m_lLonMin;
	long	m_lLatMin;
	long	m_lLonMax;
	long	m_lLatMax;
};

struct tagBlackQuerySta
{
	tagBlackQueryArea m_aryArea[ 20 ];
	tagBlackQueryPeriod	m_aryPeriod[ 20 ];

	DWORD	m_dwLstQueryTime; // 上次接收查询帧的时间 (毫秒)

	BYTE	m_bytPeriodCount; // 已保存的时间段条件个数
	BYTE	m_bytAreaCount; // 已保存的区域条件个数

	char	m_szCentGroup[10]; // 中心分组号
	BYTE	m_bytUploadFrameTotal; // 最大上传帧数
	BYTE	m_bytUploadFrameCt; // 上传帧数计数

	BYTE	m_bytReqFrameTotal; // 请求帧总数
	BYTE	m_bytReqFrameCount; // 当前已收到的请求帧计数
	char	m_aryReqFrameSymb[ 40 ]; // 接收到的帧是否重复的标志 (元素个数=上面2个数组元素总数)

	tagBlackQuerySta() { memset(this, 0, sizeof(*this)); }
};

struct tagBlackSndSta
{
	DWORD	m_dwGpsTotal; // 黑匣子数据点总数
	DWORD	m_dwLstFileDoneCt; // 上次发送时处理的黑匣子文件,已经完成的黑匣子点个数 (从文件内第一个满足条件的点开始计算)
	WORD	m_wFileTotal; // 黑匣子文件总数
	WORD	m_wDoneFileCount; // 已处理的黑匣子文件的计数
	char	m_szBlackFile[60][20]; // 黑匣子文件名,以NULL结尾
	BYTE	m_bytSndFrameCount; // 已上发的黑匣子帧计数

	tagBlackSndSta() { memset( this, 0, sizeof(*this) ); }
};

struct tagBlackSaveGps
{
	// 日期和时刻均是北京时间
	long		m_lLon;
	long		m_lLat;
	BYTE		m_bytHour; // *** { 特殊排列,注意对齐,目的是令 "年月日时"组成一个DWORD数进行比较***
	BYTE		m_bytDay;
	BYTE		m_bytMon;
	BYTE		m_bytYear; // 2位数字表示的值 *** } 特殊排列,注意对齐,目的是令 "年月日时"组成一个DWORD数以方便比较***
	BYTE		m_bytSec;
	BYTE		m_bytMin;
	BYTE		m_bytSpeed; // 海里/小时
	BYTE		m_bytMix;
#if 2 == USE_PROTOCOL
	BYTE		m_bytIOSta; // 暂时不使用
	BYTE		m_bytHeavyIO; // 暂时不使用
#endif

	tagBlackSaveGps() { memset( this, 0, sizeof(*this) ); }
};

// struct tagPosCond
// {
// 	double	m_dLstLon;
// 	double	m_dLstLat;
// 	double	m_dLstSendDis; // 米
// 
// 	float	m_fSpeed; // 米/秒, 0表示无效
// 
// 	DWORD	m_dwLstSendTime; // 上次发送时刻
// 
// 	long	m_aryArea[16][4]; // 每个范围依次是: 左下经度、左下纬度、右上经度、右上纬度 (都是10的-6次方度)
// 	DWORD	m_aryAreaContCt[16]; // 满足条件的计数 (元素个数与上面一致)
// 	BYTE	m_aryAreaProCode[16]; // 各范围的编号 (元素个数与上面一致)
// 	BYTE	m_bytAreaCount;
// 
// 	BYTE	m_bytGpsType;
// 	WORD	m_wSpaceInterv; // 公里,0表示无效
// 	WORD	m_wTimeInterv; // 分钟,0表示无效
// 
// 	BYTE	m_aryTime[10][3]; // 每个时刻依次是: 序号,时,分
// 	BYTE	m_bytTimeCount;
// 
// 	BYTE	m_aryStatus[6];
// 	// 固定为6个, BYTE	m_bytStaCount; 
// 
// 	BYTE	m_bytLstTimeHour; // 上次符合时刻条件的时刻,初始化为无效值——0xff,
// 	BYTE	m_bytLstTimeMin;
// 
// 	BYTE	m_bytCondStatus; // 0	1	I	T	V	A	S	0
// 						//	Ｉ：关闭/开启间隔条件，0-关闭，1-开启
// 						//	Ｔ：关闭/开启时刻条件，0-关闭，1-开启
// 						//	Ｖ：关闭/开启速度条件，0-关闭，1-开启
// 						//	Ａ：关闭/开启范围条件，0-关闭，1-开启
// 						//	Ｓ：关闭/开启状态条件，0-关闭，1-开启
// 
// 	char	m_szSendToHandtel[15];
// 
// 	tagPosCond() { Init(); }
// 	void Init()
// 	{
// 		memset(this, 0, sizeof(*this));
// 		m_bytLstTimeHour = m_bytLstTimeMin = 0xff;
// 		m_wTimeInterv = 0, m_wSpaceInterv = 0, m_bytCondStatus = 0x40;
// 	}
// };

// struct tagIPCfg
// {
// 	unsigned long m_ulTcpIP; // (inet_addr之后的,下同)
// 	unsigned long m_ulVUdpIP;
// 	unsigned long m_ulBUdpIP;
// 	unsigned short m_usTcpPort; // (htons之后的，下同) 
// 	unsigned short m_usVUdpPort;
// 	unsigned short m_usBUdpPort;
// 	char		m_szApn[20];
// 	BYTE		m_bytValid; // 0,表示无效; 非0,表示有效
// 
// 	tagIPCfg() { memset(this, 0, sizeof(*this)); }
// };

// struct tagGPSUpPar // 保存在文件中
// {
// 	WORD		m_wMonSpace; // 监控定距距离 （单位：10米）
// 	WORD		m_wMonTime; // 监控时间 （分钟），0xffff表示永久监控
// 	WORD		m_wMonInterval; // 监控间隔 （秒），至少为1
// 	BYTE		m_bytStaType; // 注意: 按照协议里的状态定义
// 	BYTE		m_bytMonType; // 协议中定义的监控类型
// };

struct tagPhotoInfo
{
	DWORD		m_dwPhotoDate; // 照片日期，如果是0，表示照片未知 (在拍照后上传时,可以快速找到该照片所在目录)
	WORD		m_wPhotoIndex; // 照片索引,从1开始编号
	WORD		m_wPhotoType;
	BYTE		m_bytSizeType; // 分辨率 (与协议定义的不同)
	BYTE		m_bytQualityType; // 质量类型 (与协议定义相同)
	BYTE		m_bytBright; // 亮度
	BYTE		m_bytContrast; // 对比度
	BYTE		m_bytHue; // 色调
	BYTE		m_bytBlueSaturation; // 蓝饱和
	BYTE		m_bytRedSaturation; // 红饱和
	BYTE		m_bytChannel;		// 拍照通道 (add by dkm)
	BYTE		m_bytInterval; // 下次拍照间隔,s
	BYTE		m_bytDoorPar; // 仅用于关车门拍照,低四位表示速度限制：0-15（公里/小时）,高四位表示该速度持续时间：0-15（分钟）
	char		m_szDateTime[6];
	bool		m_bUploaded; // 是否已经上传（包括仅上传索引）

	tagPhotoInfo() { memset(this, 0, sizeof(*this)); }
};

struct tagPhotoSta // 拍照状态信息
{
	tagPhotoInfo m_aryPhotoInfo[ PHOTOQUEE_MAXCOUNT ];
	
	BYTE	m_bytPhotoTotal; // 当前拍照的需要照片总数
	BYTE	m_bytPhotoCt; // 当前拍照的已拍照片计数
	
	tagPhotoSta() { memset(this, 0, sizeof(*this)); }
};

struct tagUploadPhotoSta
{
	tagPhotoInfo m_aryUpPhotoInfo[ UPLOADPHOTO_MAXCOUNT ]; // 所有要上传照片的信息数组 (只使用照片索引)
	BYTE	m_bytAllPhotoTotal; // 所有要上传照片的总数
	BYTE	m_bytAllPhotoCt; // 所有已上传照片的计数

	DWORD	m_dwPhotoSize; // 当前正上传照片的文件大小
	WORD	m_wPhotoFrameTotal; // 当前正上传照片的总包数
	//WORD	m_wPhotoIndex; // 当前正上传照片的照片索引,从1开始编号
	WORD	m_wPhotoWinCt; // 当前正上传照片的已完成的窗口计数 (不再请求重发的)
	char    m_szPhotoPath[ MAX_PATH ]; // 当前正上传照片的全路径名,以NULL结尾
	char	m_szDateTime[6]; // 当前正上传照片的日期时间
	char	m_szLonLat[8]; //当前正上传照片的经纬度
	WORD	m_wPhotoType; // 当前正上传照片的事件类型
	WORD	m_wLstSuccUpPhotoIndex; // 最后一个上传成功的照片索引
	// 当前上传照片的帧数据缓存 (保存未确认窗口内包数据,或者)
	WORD	m_aryPhotoFrameNo[ UPLOADPHOTO_WINFRAMETOTAL ]; // 各包序号,从0开始编号,0不转义, 若=0xffff,说明该位置缓存数据无效
	WORD	m_aryPhotoFrameLen[ UPLOADPHOTO_WINFRAMETOTAL ]; // 各缓存包长度,一定<=UDPDATA_SIZE
	char	m_aryPhotoFrameBuf[ UPLOADPHOTO_WINFRAMETOTAL ][ UDPDATA_SIZE ]; // 各包内容

	BYTE	m_bytPhotoSizeType; // 当前正上传照片的分辨率类型 (与协议定义相同)
	BYTE	m_bytPhotoQualityType; // 当前正上传照片的质量类型 (与协议定义相同)
	BYTE	m_bytPhotoChannel; // 当前正上传照片的摄像头通道号 (与协议定义相同)
	bool	m_bPhotoBeginUpload; // 当前正上传照片是否真的启动过上传

	bool	m_bUploadBreak; // 传输是否中断了 (即指上传超时)

	tag4152 m_obj4152;
	BYTE	m_byt4152Snd; // 0,表示本窗口未发送了4152; 非0,表示本窗口已发送4152
	
	tagUploadPhotoSta() { Init(); }
	void Init()
	{
		memset( this, 0, sizeof(*this) );
		memset( m_aryPhotoFrameNo, 0xff, sizeof(m_aryPhotoFrameNo) );
	}
};

struct tagLNUploadPhotoSta
{
	WORD   m_wPhotoIndex;
	WORD   m_CurPackNo;
	DWORD   m_dwPhotoSize; // 当前正上传照片的文件大小
	WORD    m_wPhotoFrameTotal;	// 当前正上传照片的总包数
	WORD    m_wPhotoWinCt; // 当前正上传照片的已完成的窗口计数 (不再请求重发的)
	char    m_szPhotoPath[ MAX_PATH ]; // 当前正上传照片的全路径名,以NULL结尾
	char    m_szDateTime[6]; // 当前正上传照片的日期时间
	char    m_bytMd5[16];
	byte    m_bytPhotoChannel;
	WORD    m_wPhotoType; // 当前正上传照片的事件类型
	// 当前上传照片的帧数据缓存 (保存未确认窗口内包数据,或者)
	WORD    m_aryPhotoFrameNo[ UPLOADPHOTO_MAXCOUNT ];	// 各包序号,从0开始编号,0不转义, 若=0xffff,说明该位置缓存数据无效
	WORD    m_aryPhotoFrameLen[ UPLOADPHOTO_MAXCOUNT ]; // 各缓存包长度,一定<=UDPDATA_SIZE
	char    m_aryPhotoFrameBuf[ UPLOADPHOTO_MAXCOUNT ][ UDPDATA_SIZE ]; // 各包内容

	bool    m_bPhotoBeginUpload; // 当前正上传照片是否真的启动过上传

	bool    m_bUploadBreak;	// 传输是否中断了 (即指上传超时)

	tagLNUploadPhotoSta() { Init();}
	void Init()
	{
		memset( this, 0, sizeof(*this) );
		memset( m_aryPhotoFrameNo, 0xff, sizeof(m_aryPhotoFrameNo) );
	}
};

// 每秒记录的事故疑点数据
struct tagAcdentSec
{
	char	m_szLon[4]; // 按协议经纬度定义,并且Lon和Lat要连在一起,便于发送时生成数据帧
	char	m_szLat[4];
	BYTE	m_arySpeed[5]; // 海里/小时
	BYTE	m_aryIOSta[5]; // IO状态

	tagAcdentSec()
	{
		memset( this, 0, sizeof(*this) );
	}
};

// 事故疑点记录 (文件内也使用)
struct tagAcdentRec
{
	long			m_lStopLon;
	long			m_lStopLat;
	BYTE			m_bytStopHour; // { 这样的顺序,可以使得年月日时为一个4字节整数,便于比较,并且考虑了内存对齐
	BYTE			m_bytStopDay;
	BYTE			m_bytStopMon;
	BYTE			m_bytStopYear; // }
	WORD			m_wDriverNo; // 司机编号,0~ffff,0表示空白用户,ffff表示异常用户
	BYTE			m_bytStopMin;
	BYTE			m_bytStopSec;
	tagAcdentSec	m_aryAcdentSec[20]; // 停车前20s的数据

	tagAcdentRec()
	{
		memset( this, 0, sizeof(*this) );
	}
};

/// 事故疑点查询时间段条件 (内存中使用)
struct tagAcdentQueryPeriod
{
	BYTE			m_bytBeginHour; // { 这样的顺序,可以使得年月日时为一个4字节整数,便于比较,并且考虑了内存对齐
	BYTE			m_bytBeginDay;
	BYTE			m_bytBeginMon;
	BYTE			m_bytBeginYear; // }
	BYTE			m_bytEndHour; // { 这样的顺序,可以使得年月日时为一个4字节整数,便于比较,并且考虑了内存对齐
	BYTE			m_bytEndDay;
	BYTE			m_bytEndMon;
	BYTE			m_bytEndYear; // }
	WORD			m_wBeginMin;
	WORD			m_wEndMin;

	tagAcdentQueryPeriod() { memset(this, 0, sizeof(*this)); }
};

/// 事故疑点查询范围条件 (内存中使用)
struct tagAcdentQueryArea
{
	long	m_lLonMin;
	long	m_lLatMin;
	long	m_lLonMax;
	long	m_lLatMax;
};

struct tagAcdentQuerySta
{
	DWORD	m_dwLstFileDoneCt; // 上次发送时处理的文件,已经完成的记录个数 (从文件的起始记录开始计算)
	WORD	m_wFileTotal; // 文件总数
	WORD	m_wDoneFileCount; // 已处理的文件的计数
	char	m_aryAcdentFile[60][20]; // 事故疑点文件列表,各以NULL结尾
	BYTE	m_bytFindAcdCt; // 找到的符合条件的事故疑点记录的计数
	bool	m_bFindAcd; // 是否曾经找到过满足条件的数据
};

struct tagDrvSampleDatas
{
	double	m_dDrvDis; // 根据不同的协议: (1)距上次发送记录时行驶的路程; (2)累积行驶的总里程.  单位(米)
	union
	{
 		tagSampleData_3745_KJ2 m_arySampleData_3745_KJ2[100]; // 缓存的行驶记录集
		tagSampleData_3746 m_arySampleData_3746[100]; // 缓存的行驶记录集
		tagSampleData_3747 m_arySampleData_3747[100];
	} m_uniSampleDatas;
 	BYTE	m_bytSampleDataCount; // 缓存的行驶记录个数
	BYTE	m_bytAvgSpeedCount; // 缓存的平均速度个数
	BYTE	m_aryAvgSpeed[300]; // 缓存的平均速度集,数量不能超过上面成员所能表达的范围,海里/小时
	BYTE	m_bytOilAD;
	BYTE	m_bytPowAD;
	BYTE	m_bytOilSta; // D7～D0，表示油耗采样时的状态
				// 千里眼协议: 油耗值的状态 1 D7～D0，表示油耗采样时的状态 (以下各种状态只有一位有效)
				// D0：0正常行驶，1车辆点火
				// D1：0表示正常，1 GPS定位且速度大于2海里/小时
				// D2：0表示定位，1不定位
				// D3：0表示正常，1 GPS定位且速度小于2海里/小时
				// D4：0表示正常，1停车（ACC无效）或ACC有效仪表盘却没有上电
				// D5：0表示正常，1油耗盒没接
				// D6：0表示正常，1油耗值不在设置范围内（即值无效）

				// 2客协议
				// 0x01: 点火 (事件触发)
				// 0x02: 已通电，已定位，速度>15
				// 0x04: 已通电，未定位
				// 0x08: 已通电，已定位，速度<15
				// 0x10: 未通电

	tagDrvSampleDatas() { memset( this, 0, sizeof(this) ); }
};

struct tagDrvRecSampleCfg
{
	WORD	m_wSampleCycle; // 采样周期,s
	BYTE	m_bytSndCycle; // 发送周期,次
	BYTE	m_bytSndWinSize; // 发送窗口大小

	tagDrvRecSampleCfg() { memset( this, 0, sizeof(this) ); }
};

struct tagDrvRecSta
{
	double	m_dTotalLichen; // 总里程,米
	long	m_lLon;	// 最后1个有效的经纬度,用于里程统计
	long	m_lLat;
	WORD	m_wCurDriverNo; // 最后的司机编号
	bool	m_bBeforeReset; // 是否复位前保存的
	BYTE	m_bytChkSum; // 校验和 (累加和取反) (要放在最后)
	BYTE	m_bytOilAD; // 最后的油耗AD
	BYTE	m_bytOilSta; // 最后的油耗状态
	BYTE	m_bytPowAD; // 最后的电源AD

	tagDrvRecSta() { memset( this, 0, sizeof(*this) ); }
};

#pragma pack()

typedef union {
	ushort     word;
	struct {
		BYTE  low;
		BYTE  high;
	} bytes;
}WORD_UNION;

typedef union{
	DWORD dword;
	struct  
	{
		BYTE b0;
		BYTE b1;
		BYTE b2;
		BYTE b3;
	}bytes;
}DWORD_UNION;

#pragma pack(push,1)
struct tagCurTime 
{
	BYTE		nYear;				// 4个数字表示的值,如2007,而不是7;
	BYTE		nMonth;
	BYTE		nDay;
	BYTE        nWeekDay;			//星期
	BYTE		nHour;
	BYTE		nMinute;
	BYTE		nSecond;
};

//QLED数据段
struct tagQLEDStat
{
	BYTE		symb;				//返回标志 0未工作状态 1工作状态
	BYTE		lighten;			//亮黑屏 0黑屏状态，1亮屏状态
	BYTE		char_Excp;			//字库异常0，正常；1，异常，需重新配置字库
	BYTE		set_Excp;			//设置异常0，正常；1，异常，需重新设置
	ushort		badPt_Ct;			//坏点数 双字节
	tagCurTime	curLedTime;			//LED时间
	BYTE		mail_Ct;			//LED信息总数
	BYTE		mode;				//0 转场信息，1正常播放，2点播播放，3播放即时信息，4报警
	ushort		curID;				//正在播放的信息ID 若为0，说明屏幕内没有信息，播放默认标语
	BYTE		curIerm;			//正在播放信息的显示项ID
	
	//需向中心请求的信息ID个数(1)及信息ID(2)*n
	BYTE		wait_dwnID;			//等待下载的信息个数
	ushort		ary_dwnID[MAILCNT_BOHAI];
	
	//LED时间是否为GPS时间
	BYTE		gpsTime;			//1:是 0:否
	bool		comuErr;			//与LED屏通信出错标志位 1：出错 0：正常
	//正在播放的即时信息项
	tagQLEDStat(){Init();}
	void Init()
	{
		memset(this,0,sizeof(*this));
	}
};

struct tagLedUpdtStat 
{
	BYTE		ver_Cnt;			//版本长度
	char		ver[16];			//版本
	BYTE		pkt_Cnt;			//总包数
	bool		updating;			//正在下载
	BYTE		packet;				//正在请求的包
};


//LED参数
struct tagLedParam 
{
	ushort		m_usParamID;		//参数ID：0--65535
	BYTE		m_bytLightenSt;		//亮黑屏控制：1：亮屏 2：黑屏
	BYTE		m_bytBrigType;		//亮度调节方式 : 0固定亮度调节、1按时间调节和 2按环境调节
	BYTE		m_bytStaticVl;		//固定亮度值: 1~10级别
	WORD		m_aryTimeSeg[4];	//按时间调节: 共可设4个时间段 HH:MM 精确到分（ HH * 60 + MM ） 
	BYTE		m_aryTimeSegVl[4];	//按时间调节 对应值
	BYTE		m_aryEnvVl[8];		//按环境调节 8个
	
	tagLedParam()
	{
		Init();
	}

	void Init()
	{
		memset(this,0,sizeof(*this));
	}

	void Clear()
	{
		memset(this+sizeof(unsigned short),0,sizeof(*this)-sizeof(unsigned short));//参数ID不初始化
	}
};

//点播信息
struct tagLedDISPMsg 
{
	BYTE		m_bytLvl;			//播放级别：单字节，0一般，1紧急
	BYTE		m_bytExitWay;		//退出方式：0只通过退出指令退出，1点播结束时间，2点播播放时间，3点播循环次数
	DWORD		m_wEndTm;			//结束时间：HH:MM 精确到分（ HH * 60 + MM ）//四字节，单位秒。以秒为单位。如 08:30:05(hh:mm:ss) 应换算成:(8*60+30)*60+5=30605(0x0000778D)
	ushort		m_usPlayTm;			//播放时间  单位秒
	BYTE		m_bytCircleCnt;		//循环次数
	BYTE		m_bytDISPIDCt;      //点播信息总数
	ushort		m_aryDISPID[MAILCNT_BOHAI];  //点播信息ID表

	tagLedDISPMsg()
	{
		Init();
	}

	void Init()
	{
		memset(this,0,sizeof(*this));
	}
};

struct tagLedShowItemPar				//显示项参数结构  
{
	BYTE		m_bytType;				//类型：0文字，3为图片，其他保留
	BYTE		m_bytColor;				//颜色：0红色，1绿色，2黄色，3横纹，4循环，5随机，6反红，7反绿，8反黄
	BYTE		m_bytFont;				//字体：内码方式。0-宋体，16；1-黑体，16；，2-楷体_GB2312,16；3-幼圆，16
	BYTE		m_bytWinX1;				//窗口X1：左上角x坐标，1单位 = 8像素点
	BYTE		m_bytWinY1;				//窗口Y1：左上角y坐标，1单位 = 8像素点
	BYTE		m_bytWinX2;				//窗口X2：右下角x坐标，1单位 = 8像素点
	BYTE		m_bytWinY2;				//窗口Y2：右下角y坐标，1单位 = 8像素点
	ushort		m_usStayTm;				//双字节，每显示项在LED屏上的停留时间
	BYTE		m_bytShowWay;			//显示方式：0-立即      1-左滚动    2-上滚动    3-左移进    4-右移进 
	//			5-上移进    6-下移进    7-左右对进  8-上下对进  9-左飞字    
	//			10-右飞字   11-左滚字   12-右滚字   13-左上滚字 14-右上滚字 
	//			15-左下滚字 16-右下滚字 17-亮灭闪烁 18-反白闪烁 19-亮灭流动 
	//			20-反白流动 21-左拉帘   22-右拉帘   23-左右对拉 24-方块帘   
	//			25-左百叶   26-右百叶   27-左拖尾   28-右拖尾   29-上拖尾   
	//			30-下拖尾   31-上瀑布   32-下瀑布   33-上落雪   34-下落雪   
	//			35-闪烁打字 36-反白打字 37-扩展打字 38-霹雳     39-波浪     
	//			40-弹跳流动 41-霓虹流动
	BYTE		m_bytShowType;			//显示样式，未开放，置为0
	BYTE		m_bytRunSpeed;			//滚动速度：当显示项的显示方式为1（左滚动）或2（上滚动）时，显示项的滚动速度
	BYTE		m_bytUpInter;			//上滚行间距：当显示项的显示方式为2（上滚动），上滚行间距
	//		BYTE		m_bytLeftInter;			//左滚间隔：当显示项的显示方式为1（左滚动），每屏间字符间隔。屏幕上每8个像数点为1
	BYTE		m_bytLeftStayWay;		//左滚停留方式：单字节。0每屏停留；1最后一屏停留
	BYTE		m_bytCircleCnt;			//循环次数
	BYTE		m_bytReservered[2];		//保留两个字节
	ushort		m_usShowInfoCt;			//显示项内容字节数：双字节
	BYTE		m_bytShowInfo[20];		//显示项内容

	tagLedShowItemPar()
	{
		memset(this, 0, sizeof(*this));	
	}
};

struct tagLedShowMsg				//即时信息结构 
{
	BYTE		m_bytLvl;				//播放级别
	BYTE		m_bytExitWay;			//退出方式：0只通过退出指令退出，1点播结束时间，2点播播放时间，3点播循环次数
	DWORD		m_wEndTm;				//结束时间：四字节，单位秒。以秒为单位。如 08:30:05(hh:mm:ss) 应换算成:(8*60+30)*60+5=30605(0x0000778D)
	ushort		m_usPlayTm;				//播放时间：双字节，单位秒
	BYTE		m_bytCircleCnt;			//循环次数
	BYTE		m_bytReservered[4];		//保留字节
	BYTE		m_bytShowCt;			//显示项总数
	tagLedShowItemPar	m_tagShowPar[2];//显示项结构参数
	
	tagLedShowMsg()	
	{
		memset(this, 0, sizeof(*this));
	}
};

//LED节目单
struct tagLedMenu 
{
	ushort		m_usMenuID;			//节目单ID 0--65535
	tagLedParam m_LedParam;        //LED参数设置
	BYTE		m_bytConfig;       /*
									设置字（1）
									二进制0ABCDEFG，
									G－0：保留原先节目单
									1：清除原先节目单
									F－0：无点播，取消点播
									1：带点播信息
									E－0：无即时播放信息，或取消即时播放
									1：带即时播放信息
								   */
	BYTE		m_bytMailIDCnt;      //节目单中信息ID个数，与LED中信箱ID对比即可获得所需下载的信息ID，并做出请求
	ushort		m_aryMailID[MAILCNT_BOHAI];  //信箱ID集合 博海125 
	tagLedDISPMsg m_LedDispMsg;     //点播信息	
	tagLedMenu(){Init();}
								   
	void Init()
	{
		memset(this,0,sizeof(*this));
	}
								   
	//处理网络字节序
	void hton()
	{
		for (int i=0;i<m_bytMailIDCnt;i++)
		{
			m_aryMailID[i] = htons(m_aryMailID[i]);
		}
	}
};

struct tagMail 
{
	BYTE		m_bytMailIDCnt;			//信箱已用个数！！
	ushort		m_aryMailID[MAILCNT_BOHAI];  //信箱ID集合 博海125
	
	tagMail()
	{
		Init();
	}

	void Init()
	{
		memset(this,0,sizeof(*this));
	}

	//处理网络字节序
	void hton()
	{
		for (int i=0;i<m_bytMailIDCnt;i++)
		{
			m_aryMailID[i] = htons(m_aryMailID[i]);
		}
	}
};

//下载节目单应答格式
struct tagLedMenuAns 
{
	ushort		m_usMenuID;			//节目单ID 0--65535
	ushort		m_usParamID;		//参数ID：0--65535
	ushort      m_usTURNID;			//转场信息ID
	ushort      m_usNTICID;			//通知信息ID
	BYTE		m_bytConfig;       /*
								   设置字（1）
								   二进制0ABCDEFG，
								   G－0：保留原先节目单
								   1：清除原先节目单
								   F－0：无点播，取消点播
								   1：带点播信息
								   E－0：无即时播放信息，或取消即时播放
								   1：带即时播放信息
									*/
	BYTE		m_bytDownIdCnt;		//下发ID个数
									
	tagLedMenuAns()
	{
		Init();
	}
	
	void Init()
	{
		memset(this,0,sizeof(*this));
	}	
};

struct Frm47 
{
	byte	protype;	//协议类型
	char	chktype;	//检测类型
	char	restype;	//应答类型
	Frm47()		{ protype = 0x47; }
};

#pragma pack(pop)

#if SAVE_LEDDATA == 1
//存储日志的结构
struct LogHead {
	int cnt;		//日志总数
	int write;		//写日志指针
};
#endif

enum DOWNSTA{
	Idlesse = 1,
	DispTask,
	ReqAttr,
	WaitData,	
	RcvData,
	ChkBuf,	
	ChkMd5,	
	TranResult,	
	SoftUpdate,
	SendData,	
	ReportChk,
	UpdataRes,	
	CutComu,
	EndRcv
};

enum FILE_TYPE
{
	SOFT,
	ROOT,
	KERN,
	BOOT,
	LINE
};
#define FILE_TYPE_COUNT 5
//-----------------------------------------------------------------------
// 下载文件索引列表
struct __attribute__(packed,aligned (1)) FileAttr {
	bool	m_bExist;		// 文件是否存在
	ulong	m_ulID;			// 文件ID
	ulong	m_ulFileSize;	// 文件大小
	byte	m_bytMd5[32];	// MD5值	
	ulong	m_ulDownSize;	// 已下载
	ushort	m_ulPackLen;	// 使用的数据包大小
};

struct __attribute__(packed,aligned (1)) FileList {
	FileAttr	m_objFile[FILE_TYPE_COUNT];		//文件属性
	ulong		m_ulVer;						//文件列表版本

	FileList() { memset(this, 0, sizeof(*this));}
};
#endif

