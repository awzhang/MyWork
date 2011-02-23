#ifndef COMUSERVEXPORT_H_FGJIYE382ASD
#define COMUSERVEXPORT_H_FGJIYE382ASD

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stddef.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "typedef.h"

extern int NETWORK_TYPE;
extern int PHONE_MODTYPE;
extern char USB_DEV_INSMOD[128];
extern char USB_DEV_CHECKPATH_AT[128];
extern char USB_DEV_CHECKPATH_PPP[128];
extern char PPP_DIAL_NUM[128];
extern char PPP_USER_NAME[128];
extern char PPP_PASSWORD[128];
extern DWORD COMMUX_DEVPATH_AT;
extern DWORD COMMUX_DEVPATH_PPP;

#define VEHICLE_M		666
#define VEHICLE_M2		777
#define VEHICLE_V8		888
#define VEHICLE_TYPE VEHICLE_V8

// 各版本号,在归档、发布给客户使用等情况后，要注意版本号的升级
#define CHK_VER			0	// 0,发布版本; 1,生产检测版本
#define USE_PROTOCOL	0	// 0,千里眼通用版协议; 1,千里眼研三公开版协议; 2,千里眼研一公开版协议(即公司公开版); 30,两客通用版协议
#define USE_LANGUAGE	0	// 0,简体中文; 11,英文
#define SMALL_VER  1	// 1,最小运行版本,置于文件系统内；0，通常运行版本

#ifdef XUN_DEBUG
#define XUN_ASSERT_VALID( boolval, strval ) { if(!(boolval)) assert(false); }
#else
#define XUN_ASSERT_VALID( boolval, strval ) {}
#endif

#define MSG_NOR		1
#define MSG_DBG		1
#define MSG_ERR		1
#define MSG_HEAD	("")

#define PRTMSG(enb, format, ...) \
if( enb ) \
{ \
	printf( "%s: ", MSG_HEAD ); \
	printf( format, ##__VA_ARGS__ ); \
}

#define PACKED_1 __attribute__( (packed, aligned(1)) )
#define STRUCT_1 struct PACKED_1

#define KEY_SEM_DOG	58588
#define KEY_SHM_DOG	58598

///////// { <1>通用编译开关 (下述开关,都应根据需要来具体控制)

// { 公共开关
#if CHK_VER == 0
#define USE_TELLIMIT	0		// 0,千里眼中心呼入呼出限制; 1,千里眼中心电话本控制; 2,两客中心电话本控制; 3,研一公开版电话本控制
#else
#define USE_TELLIMIT	0
#endif
#define USE_TTS			1		// 1,TTS语音支持; 0,不支持
#define CP_CHS			936
#define USE_SOCKBINDIP	0		// 0,socket不绑定(正式版本使用); 1,绑定GPRS分配的IP(在调试时使用)
#define USE_COMBUS		0		// 0, 不使用串口扩展盒; 1, 使用串口扩展盒;	2, 使用150TR行驶记录仪
#define GPS_TEST		0		// 1, 打开GPS测试功能1; 2,打开GPS测试功能2; 0,关闭
#define USE_JIDIANQI	0		// 1, 常闭继电器; 0, 常开继电器
#define USE_MENCHI		0		// 1, 使用门磁开关; 0, 不使用门磁开关
#define USE_VIDEO_TCP	1		// 1, 视频指令使用独立的套接字，0，不使用独立的套接字
// } 公共开关

// QianExe使用
#define USE_LIAONING_SANQI	0	// 1, 使用辽宁移动三期协议，0, 不使用
#define USE_BLK			1		// 1,支持黑匣子; 0,不支持
#define USE_AUTORPT		0		// 1,支持主动上报(0154); 0,不支持
#define USE_METERTYPE	0		// 1,支持税控,计价器与车台之间用串口通信,包括空重车报告; 2,支持凯天计价器; 0,不支持税控
#define USE_LEDTYPE		0		// 1,使用博海LED屏; 2,使用川力LED; 3,使用凯天LED; 0,不使用LED
#define USE_DRVREC		1		// 1,使用JG2旧的行驶记录仪模块; 0,不使用
//【 若USE_DRVREC为1，则这几条不要启用
#define USE_DRVRECRPT	0		// 1,3745行驶记录上传; 2,3746; 3,3747; 4,3748; 11,3745_2客; 0,不支持	(1和4暂不支持)
#define USE_ACDENT		0		// 1,事故疑点,千里眼中心协议; 2,2客中心协议; 0,不支持
#define USE_DRIVERCTRL	0		// 1,使用司机管理业务; 0,不使用
// 】若USE_DRVREC为1，则这几条不要启用
#define USE_OIL			3		// 1,油耗数据使用IO直接读取; 2,使用油耗检测盒,并使用研一算法; 3,使用150pd02-003油耗检测盒; 0,不支持
#define USE_REALNAV		0		// 1,启用实时指引(目前仅在2客系统中用到); 0,不启用
#define USE_JIJIA		0		// 1,支持计价器模块(包括串口触发和IO触发,但与税控功能无关); 0,不支持
#define USE_REALPOS		1		// 1,支持实时定位模块; 0,不支持
#define USE_DISCONNOILELEC 1	// 1,开启断油断电; 0,不开启
#define USE_HELPAUTODIAL 0		// 1,求助时自动拨打中心的求助电话（需保证求助电话设置有效）; 0,不拨打
//#define DIS_SPD_USEIO			// 打开此开关,行驶记录模块CDrvrecRptDlg中的里程和速度,使用IO(里程脉冲)信号计算
#define UPLOAD_AFTERPHOTOALL 0	// 0,拍照1张即上传1张; 1,全部拍照完毕才开始上传
#define USE_PHOTOUPLD	0		// 0,使用窗口方式上传协议; 1,使用一次性上传完整照片协议
#define USE_PHOTOTHD	1		// 1,拍照处理使用单独子线程; 0,不使用
#define USE_AUTORPTSTATION	1	// 1,启用公交自动报站；0，不启用
#define USE_PLAYSOUNDTYPE	1	// 1,使用TTS语音进行报站，2，使用录音文件进行报站

// 以下开关要小心使用
#define USE_EXCEPTRPT	0		// 1,异常情况发送报告短信; 0,不支持(注意: 发布版本必须是0)


//	ComuExe使用 SIMULATE_GPS:模拟跑车
#define GPS_TYPE REAL_GPS
#define SIMULATE_GPS 1
#define REAL_GPS 2

// ComuExe使用
#define NETWORK_YIDONG	66
#define NETWORK_LIANTONG	88
#define NETWORK_GPRS 0
#define NETWORK_CDMA 1
#define NETWORK_TD	2
#define NETWORK_EVDO 3
#define NETWORK_GSM	4
#define NETWORK_WCDMA 5

// { 手机模块选择 
#define PHONE_MODTYPE_TD_LC6311	10
#define PHONE_MODTYPE_TD_DTM6211 11
#define PHONE_MODTYPE_MC703OLD 12
#define PHONE_MODTYPE_MC703NEW 13
#define PHONE_MODTYPE_EM770W 14
#define PHONE_MODTYPE_BENQ 15
#define PHONE_MODTYPE_SIMCOM 16
#define PHONE_MODTYPE_SIM4222 17
#define PHONE_MODTYPE_SIM5218 18
#define PHONE_MODTYPE_MU203 19

#define NETWORK_GPRSTYPE NETWORK_YIDONG	// NETWORK_LIANTONG
//注意: 如果是CDMA模块要选择模块的类型
#define CDMA_EM200 1
#define CDMA_CM320 2
#define CDMA_TYPE CDMA_EM200
// } 手机模块选择

#define AUTO_PICKUP		0		// 1,表示来电自动接听; 0, 来电不自动接听
#define GPSVALID_FASTFIXTIME 1	// 1,表示一旦GPS有效则立刻校时；0，连续有效约15分钟后才校时
#define USE_IRD			0		// 1,表示使用红外语音,不使用手柄和调度屏(此时应确保启用TTS); 0,不使用红外语音,使用手柄或调度屏(自适应)
#define USE_IRD_KAITIAN		1	// 1,表示使用凯天红外评价，0,不使用凯天红外评价
#define PRINT_GPS_DATA		0	// 1,表示从调试串口打印GPS原始数据，0, 不打印
//#define GPSGSA_ENB			// 是否读取GSA格式数据 (PDOP、HDOP、VDOP)
#define USE_PING		0		// 是否使用PING的机制 1:使用 0:不使用
#if GPS_TEST && !defined(GPSGSA_ENB)
#define GPSGSA_ENB
#endif

// UpdateExe使用
#define WIRELESS_UPDATE_TYPE	0	// 1, 使用车台维护中心进行远程升级， 0，使用流媒体中心进行远程升级
#define WIRELESS_DOWNLOAD_TYPE	1 // 1, 缓存全部数据帧,一次性写Flash	0, 逐帧写Flash

// DvrExe使用
#define DISK_UMOUNT_REBOOT	1	// 1, SD卡未挂载复位， 0，不复位
#define DISK_UCONNECT_INIT	1	// 1, SD卡未识别初始化， 0，不初始化
#define VIDEO_BLACK_TYPE	1	// 1, 视频黑匣子上传使用旧协议，2, 使用新协议

#if VEHICLE_TYPE == VEHICLE_M

	#define SYSTEM_VER		"1.01"

#if SMALL_VER == 0

	#define CFGSPEC	("YXYF3_产品大卖M-X前途无限_2010") // 字节数应小于配置中的存储容量,永不更改

#if CHK_VER == 0
	#define SOFT_VER		"yx003-110mw001-001"
#else
	#define SOFT_VER		"t005-110mw001-001"
#endif

#endif

#if SMALL_VER == 1

	#define SOFT_VER		"yx003-110mw001-bkp"
	#define CFGSPEC	("YXYF3_产品疯卖M-X前途无限_2010") // 字节数应小于配置中的存储容量,永不更改

#endif

#endif



#if VEHICLE_TYPE == VEHICLE_V8

	#define SYSTEM_VER		"1.08"

#if SMALL_VER == 0

	#define CFGSPEC	("YXYF3_产品大卖V8-X前途无限_2010") // 字节数应小于配置中的存储容量,永不更改

#if CHK_VER == 0
	#define SOFT_VER		"yx01b-110v8t001-001"
#else
	#define SOFT_VER		"t008-110v8w001-001"
#endif

#endif

#if SMALL_VER == 1

	#define CFGSPEC	("YXYF3_产品疯卖V8-X前途无限_2010") // 字节数应小于配置中的存储容量,永不更改
	#define SOFT_VER		"yx01b-110v8w001-bkp"

#endif

#endif



#if VEHICLE_TYPE == VEHICLE_M2

	#define SYSTEM_VER		"1.00"

#if SMALL_VER == 0

	#define CFGSPEC	("YXYF3_产品大卖M2-X前途无限_2010") // 字节数应小于配置中的存储容量,永不更改

#if CHK_VER == 0
	#define SOFT_VER		"yx001-110m2w001-001"
#else
	#define SOFT_VER		"t001-110m2w001-001"
#endif

#endif

#if SMALL_VER == 1

	#define CFGSPEC	("YXYF3_产品疯卖M2-X前途无限_2010") // 字节数应小于配置中的存储容量,永不更改
	#define SOFT_VER		"yx001-110m2w001-bkp"

#endif

#endif



// 此字符串将在升级后写到配置区内的m_szSpecode内，以在升级重启恢复出厂配置，但要保留手机号、IP、端口
#define NEED_RUSEMUCFG	("升级后恢复出厂配置")

// 此字符串将在升级后写到重要配置区的m_szReUpdate内，以在升级重启后检查是否需要再次升级
#define NEED_REUPDATE	("程序异常，用备份文件更新")

///////// } <1>通用编译开关

/// { 部分配置初始化定义,仅用于配置区默认配置,且仅用于各配置对象的Init方法中,不同的版本都要注意修改!
/// !!注意是否还有其他要修改的默认配置
#if USE_LIAONING_SANQI == 1
#define APN			"cmcwt"				// APN设置（一般不用改）
#endif
#if USE_LIAONING_SANQI == 0
#define APN			"3gnet"				// APN设置（一般不用改）
#endif

#define CDMA_NAME   "card"
#define CDMA_PASS   "card"

#define QTCPIP		"27.115.0.114"		// 千里眼TCP IP地址
#define QTCPPORT	9020				// 千里眼TCP端口
#if USE_LIAONING_SANQI == 1										//
#define QUDPIP		"10.64.92.80"		// 千里眼UDP IP地址
#define QUDPPORT	11004				// 千里眼UDP端口
#endif
#if USE_LIAONING_SANQI == 0
#define QUDPIP		"27.115.0.114"		// 千里眼UDP IP地址
#define QUDPPORT	3100				// 千里眼UDP端口

#endif

#if USE_LIAONING_SANQI == 1	
#define DTCPIP		"211.137.43.183"	// DVR视频的TCP IP地址
#define DTCPPORT	6200				// DVR视频的TCP端口
#endif
#if USE_LIAONING_SANQI == 0
#define DTCPIP		"27.115.0.114"		// DVR视频的TCP IP地址
#define DTCPPORT	6900				// DVR视频的TCP端口
#endif
#define DUDPIP		"59.61.82.170"		// DVR视频的UDP IP地址
#define DUDPPORT	10101				// DVR视频的UDP端口

#define UTCPIP		"59.61.82.173"		// GPSKING中心的升级服务器的IP
#define UTCPPORT	12345				// GPSKING中心的升级服务器的端口
#define LUDPIP		"59.61.82.173"		// 流媒体(或远程升级中心)UDP IP地址
#define LUDPPORT	18000				// 流媒体(或远程升级中心)UDP端口
			
#define SMSCENTID	""					// 短信中心号,注意"+86"不能丢 // +8613800592500
#define SMSSERVCODE	""					// 特服号
#define CENTID1		0x35		// 中心ID(即区域号)第一字节,注意使用16进制
#define CENTID2		0x00		// 中心ID(即区域号)第二字节,注意使用16进制,0无需转义为7f

#define USERIDENCODE "111111"	// 公开版_研一协议中的GPRS登陆帧中的用户识别码(6字符),每个定制版本都应有自己固定的识别码

#define EXCEP_RPTTEL "15985886953"		// 异常报告短信的目的手机号


#define SOFT_VERSION		"05500030100"
#define COMPANY_ID			"1025"
#define GROUP_BILLING_ID	"9000038873"//"9000098732"
/// } 部分配置初始化定义,仅用于配置区默认配置,且仅用于各配置对象的Init方法中,不同的版本都要注意修改!

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b)) 
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define MAX_PATH	255
#define MSG_MAXLEN		1

#define FLASH_PART4_PATH		("/mnt/Flash/part4/")				// FLASH分区4路径名
#define DRVRECSTA_FILENAME		("/mnt/SDisk/part2/data/DrvRecSta.dat")	// 行驶状态记录文件名
#define DRIVE_RECORD_FILENAME	("/mnt/SDisk/part2/data/DrvJg2")	// 旧的行驶记录仪保存文件
#define BLK_SAVE_PATH			("/mnt/SDisk/part2/data/Blk/")		// 黑匣子文件存放路径
#define ACD_SAVE_PATH			("/mnt/SDisk/part2/data/Acd/")		// 事故疑点文件存放路径
#define HANDLE_FILE_PATH		("/mnt/SDisk/part2/data")			// 手柄配置文件保存目录

#if VEHICLE_TYPE == VEHICLE_V8
#define	PHOTO_SAVE_PATH			("/mnt/SDisk/part2/data/")			// 照片保存路径
#define	VIDEO_SAVE_PATH			("/mnt/SDisk/part1/data/")			// 视频保存路径
#endif
#if VEHICLE_TYPE == VEHICLE_M || VEHICLE_TYPE == VEHICLE_M2
#define	PHOTO_SAVE_PATH			("/mnt/HDisk/part2/data/")			// 照片保存路径
#define	VIDEO_SAVE_PATH			("/mnt/HDisk/part1/data/")			// 视频保存路径
#endif

#define LIU_DOWNLOAD_PATH		("/mnt/Flash/part3/Down/")			// 流媒体下载文件的存放目录
#define RPTSTATION_U_PATH		("/mnt/UDisk/part1/")				// 公交文件在U盘上的路径
#define	RPTSTATION_SAVE_PATH 	("/mnt/SDisk/part2/data/RptSta/")	// 公交文件保存路径

enum QUEELVL // 必须从1开始递增,有且只有16个,越大越优先
{
	LV1 = 1,
	LV2,
	LV3,
	LV4,
	LV5,
	LV6,
	LV7,
	LV8,
	LV9,
	LV10,
	LV11,
	LV12,
	LV13,
	LV14,
	LV15,
	LV16
};

enum ERR_CODE
{
	ERR_SEMGET = 1,
	ERR_SEMCTL,
	ERR_SEMOP,
	
	ERR_SHMGET,
	ERR_SHMAT,
	
	ERR_MSGGET,
	ERR_MSGSND,
	ERR_MSGRCV,
	ERR_MSGSKIPBLOCK,

	ERR_THREAD,
	ERR_READCFGFAILED,

	ERR_MEMLACK,
	ERR_SPACELACK,
	ERR_BUFLACK,
	ERR_PTR,
	ERR_QUEUE_EMPTY,
	ERR_QUEUE_FULL,
	ERR_QUEUE,
	ERR_QUEUE_TOOSMALL,
	ERR_QUEUE_SYMBNOFIND,

	ERR_OPENFAIL,
	ERR_IOCTLFAIL,
	ERR_LSEEKFAIL,
	ERR_WRITEFAIL,
	ERR_READFAIL,

	ERR_SOCKLIB,
	ERR_SOCK,
	ERR_SOCK_TCPFBID,

	ERR_FILEMAP,
	ERR_FILE_FAILED,
	ERR_FILE,

	ERR_COM,
	ERR_IO,
	ERR_PHOTO,
	ERR_CFGBAD,		
	ERR_COMCHK,
	ERR_COMSND,
	ERR_COMSND_UNALL,

	ERR_DLL,
	ERR_PAR,	
	ERR_CFG,
	ERR_DATA_INVALID,
	ERR_BLACKGPS_INVALID,
	ERR_GPS_INVALID,
	ERR_INNERDATA_ERR,
	ERR_STA_ERR,

	ERR_AUTOLSN,
	ERR_AUTOLSN_NOTEL,	

	ERR_CODECVT,

	ERR_LED,
	ERR_LED_QUEE,

	ERR_SMS_DESCODE, // 短信发送目的方手机号不正确

	ERR_TM,

	ERR_SYSCALL,
	ERR_SIG,
};

enum PHONE_TYPE
{
	PHONE_BENQ = 0,
	PHONE_SIMCOM,
	PHONE_HWCDMA,
	PHONE_DTTD
};

#define VALIDYEAR_BEGIN		2009
#define VALIDMONTH_BEGIN	10 // 在VALIDYEAR_BEGIN中的month

#define TCP_RECVSIZE	1024
#define TCP_SENDSIZE	60000 //1024
#define UDP_RECVSIZE	8096
#define UDP_SENDSIZE	8096
#define SOCK_MAXSIZE	60000 //1500
#define COMUBUF_SIZE	2048
#define UDPDATA_SIZE	1200 // UDP数据报中数据内容的最大长度, 也即照片数据帧一个包的大小限定值
// !!注意,该值应小于1500,且小于SOCKSND_MAXBYTES_PERSEC-QIANUPUDPFRAME_BASELEN

#define MAX_UNSIGNED_64		((unsigned long long)(-1))
#define MAX_UNSIGNED_32		((unsigned long)(-1))
#define MAX_UNSIGNED_16		((unsigned short)(-1))
#define MAX_UNSIGNED_8		((unsigned char)(-1))

const double BELIV_MAXSPEED = 100 * 1.852 / 3.6; // 单位:米/秒

const BYTE GPS_REAL		= 0x01;
const BYTE GPS_LSTVALID	= 0x02;

const WORD PHOTOEVT_FOOTALERM		= 0x8000; // 劫警
const WORD PHOTOEVT_OVERTURNALERM	= 0x4000; // 侧翻报警
const WORD PHOTOEVT_BUMPALERM		= 0x2000; // 碰撞报警
const WORD PHOTOEVT_FOOTALERM_BIG	= 0x1000; // 劫警时附加的大照片,但并不立即上传
const WORD PHOTOEVT_TRAFFICHELP		= 0x0800; // 交通求助
const WORD PHOTOEVT_MEDICALHELP		= 0x0400; // 医疗求助
const WORD PHOTOEVT_BOTHERHELP		= 0x0200; // 纠纷求助
const WORD PHOTOEVT_OPENDOOR		= 0x0100; // 开车门
const WORD PHOTOEVT_AREA			= 0x0080; // 进出区域
const WORD PHOTOEVT_TIRE			= 0x0080; // 疲劳驾驶											  //
const WORD PHOTOEVT_ACCON			= 0x0040; // ACC有效
const WORD PHOTOEVT_DRIVEALERM		= 0x0020; // 非法发动车辆报警拍照
const WORD PHOTOEVT_OVERSPEED		= 0x0010; // 超速
const WORD PHOTOEVT_JIJIADOWN		= 0x0008;// 空车变重车
const WORD PHOTOEVT_BELOWSPEED		= 0x0004;// 低速
const WORD PHOTOEVT_TIMER			= 0x0002; // 定时拍照
const WORD PHOTOEVT_CENT			= 0x0001; // 中心抓拍

const WORD PHOTOUPLOAD_FOOTALERM		= PHOTOEVT_FOOTALERM;
const WORD PHOTOUPLOAD_OVERTURNALERM	= PHOTOEVT_OVERTURNALERM;
const WORD PHOTOUPLOAD_BUMPALERM		= PHOTOEVT_BUMPALERM;
const WORD PHOTOUPLOAD_TRAFFICHELP		= PHOTOEVT_TRAFFICHELP;
const WORD PHOTOUPLOAD_MEDICALHELP		= PHOTOEVT_MEDICALHELP;
const WORD PHOTOUPLOAD_BOTHERHELP		= PHOTOEVT_BOTHERHELP;
const WORD PHOTOUPLOAD_OPENDOOR		= PHOTOEVT_OPENDOOR;
const WORD PHOTOUPLOAD_AREA				= PHOTOEVT_AREA;
const WORD PHOTOUPLOAD_TIRE				= PHOTOEVT_TIRE;
const WORD PHOTOUPLOAD_ACCON			= PHOTOEVT_ACCON;
const WORD PHOTOUPLOAD_DRIVEALERM		= PHOTOEVT_DRIVEALERM;
const WORD PHOTOUPLOAD_OVERSPEED		= PHOTOEVT_OVERSPEED;
const WORD PHOTOUPLOAD_JIJIADOWN		= PHOTOEVT_JIJIADOWN;
const WORD PHOTOUPLOAD_BELOWSPEED		= PHOTOEVT_BELOWSPEED;
const WORD PHOTOUPLOAD_TIMER			= PHOTOEVT_TIMER;
const WORD PHOTOUPLOAD_CENT				= PHOTOEVT_CENT;


/// 进程间通信队列处理方代码, 可以是进程或线程 如下定义,二进制为1的bit依次左移 !!!
const DWORD DEV_SOCK	= 0x00000001;
const DWORD DEV_LED		= 0x00000002;
const DWORD DEV_QIAN	= 0X00000004;
const DWORD DEV_DVR		= 0x00000008;
const DWORD DEV_PHONE	= 0x00000010;
const DWORD DEV_DIAODU	= 0x00000020;
const DWORD DEV_UPDATE	= 0x00000040;
const DWORD DEV_IO		= 0x00000080; // 不需要接收队列
const DWORD DEV_GPS		= 0x00000100; // 不需要接收队列


/// 看门狗处理
const DWORD DOG_QIAN	= 0X00000001;
const DWORD DOG_COMU	= 0X00000002;
const DWORD DOG_SOCK	= 0X00000004;
const DWORD DOG_IO		= 0X00000008;
const DWORD DOG_UPGD	= 0X00000010;
const DWORD DOG_DVR		= 0X00000020;
const DWORD DOG_DVR_T1	= 0X00000040;
const DWORD DOG_DVR_T2	= 0X00000080;
const DWORD DOG_DVR_T3	= 0X00000100;


#if VEHICLE_TYPE == VEHICLE_M
// 必须与SYSIO_CFG[]的定义顺序一致!且必须从0开始编号,注意编号取值范围: 0~254 (255已作为函数特殊参数使用)
enum IOSymb
{
	// 输入
	IOS_ACC = 0,
	IOS_ALARM, //劫警信号
	IOS_JIJIA, // 空重载
	IOS_FDOOR, //前门信号
	IOS_BDOOR, //后门信号
	IOS_JIAOSHA,
	IOS_SHOUSHA,
	IOS_PASSENGER1, //载客信号1
	IOS_PASSENGER2, //载客信号2
	IOS_PASSENGER3, //载客信号3
	IOS_HDELELOCK, // 电子锁
	
	
	// 输出
	IOS_HUBPOW,
	IOS_PHONEPOW,
	IOS_PHONERST,
	IOS_HARDPOW, //硬盘电源
	IOS_TRUMPPOW, //功放电源
	IOS_SCHEDULEPOW, //调度屏电源
	IOS_VIDICONPOW, //摄像头/TW2835电源
	IOS_OILELECCTL, //断油电控制
	IOS_APPLEDPOW, //程序工作指示灯
	IOS_TOPLEDPOW, //顶灯电源
	IOS_AUDIOSEL, //手机音频/PC音频选择
	IOS_EARPHONESEL, //耳机/免提选择
	IOS_TW2835RST, //TW2835复位
// 	IOS_GPS,
// 	IOS_SYSLEDPOW, //系统工作指示灯
};

// IO应用意义定义,针对外部IO信号,非CPU管脚信号
enum IO_APP
{
	// 输入
	IO_ACC_ON = 1, // 必须从1开始,因为内部数组用的是unsigned char,节约空间
	IO_ACC_OFF,
	IO_ALARM_ON,
	IO_ALARM_OFF,
	IO_JIJIA_HEAVY,
	IO_JIJIA_LIGHT,
	IO_FDOOR_OPEN,
	IO_FDOOR_CLOSE,
	IO_BDOOR_OPEN,
	IO_BDOOR_CLOSE,
	IO_JIAOSHA_VALID,
	IO_JIAOSHA_INVALID,
	IO_SHOUSHA_VALID,
	IO_SHOUSHA_INVALID,
	IO_PASSENGER1_VALID,
	IO_PASSENGER1_INVALID,
	IO_PASSENGER2_VALID,
	IO_PASSENGER2_INVALID,
	IO_PASSENGER3_VALID,
	IO_PASSENGER3_INVALID,
	IO_HDELELOCK_OPEN,
	IO_HDELELOCK_CLOSE,
	
	
	// 输出
	IO_HUBPOW_ON,
	IO_HUBPOW_OFF,
	IO_PHONEPOW_ON,
	IO_PHONEPOW_OFF,
	IO_PHONERST_HI,
	IO_PHONERST_LO,
	IO_HARDPOW_ON, 
	IO_HARDPOW_OFF, 
	IO_TRUMPPOW_ON,
	IO_TRUMPPOW_OFF,
	IO_SCHEDULEPOW_ON, 
	IO_SCHEDULEPOW_OFF, 
	IO_VIDICONPOW_ON, 
	IO_VIDICONPOW_OFF, 
	IO_OILELECCTL_CUT, 
	IO_OILELECCTL_RESUME, 
	IO_APPLEDPOW_ON, 
	IO_APPLEDPOW_OFF,
	IO_TOPLEDPOW_ON,
	IO_TOPLEDPOW_OFF,
	IO_AUDIOSEL_PHONE, //选择手机音频
	IO_AUDIOSEL_PC, //选择PC音频
	IO_EARPHONESEL_ON,//选择耳机
	IO_EARPHONESEL_OFF,//选择免提
	IO_TW2835RST_HI,
	IO_TW2835RST_LO,
// 	IO_GPSPOW_ON,
// 	IO_GPSPOW_OFF,
// 	IO_SYSLEDPOW_ON, 
// 	IO_SYSLEDPOW_OFF, 
};
#endif

#if VEHICLE_TYPE == VEHICLE_V8
// 必须与SYSIO_CFG[]的定义顺序一致!且必须从0开始编号,注意编号取值范围: 0~254 (255已作为函数特殊参数使用)
enum IOSymb
{
	// 输入
	IOS_ACC = 0,
	IOS_QIANYA,			// 欠压信号
	IOS_ELECSPEAKER,	// 电子喇叭信号
	IOS_FDOOR,			// 前门信号
	IOS_ALARM,			// 劫警信号
	IOS_POWDETM,		// 电源输入检测信号
	IOS_JIJIA,			// 空重载信号
	IOS_CHEDENG,		// 车灯输入信号
	IOS_ZHENLING,		// 振铃信号
//	IOS_SPIDO,			// SD卡检测信号
//	IOS_SPICK,			// SD卡写保护信号
		
	// 输出
	IOS_SYSLEDPOW_RED,	// 工作指示灯（红）
	IOS_SYSLEDPOW_GREEN,// 工作指示灯（绿）
	IOS_SCHEDULEPOW,	// 调度屏电源
	IOS_TRUMPPOW,		// 音频功放电源
	IOS_EARPHONESEL,	// 耳机/免提选择
	IOS_AUDIOSEL,		// 手机音频/PC音频选择
	IOS_PHONERST,		// 手机模块复位
	IOS_PHONEPOW,		// 手机模块电源
	IOS_YUYIN,			// 语音切换开关
	IOS_OILELECCTL,		// 断油电控制
	IOS_VIDICONPOW,		// 摄像头/TW2835电源
	IOS_TW2865RST,		// TW2865复位
	IOS_LCDPOW,			// LCD电源控制
	IOS_USBPOW,			// USB2.0电源
};

// IO应用意义定义,针对外部IO信号,非CPU管脚信号
enum IO_APP
{
	// 输入
	IO_ACC_ON = 1, // 必须从1开始,因为内部数组用的是unsigned char,节约空间
	IO_ACC_OFF,
	IO_QIANYA_VALID,
	IO_QIANYA_INVALID,
	IO_ELECSPEAKER_VALID,
	IO_ELECSPEAKER_INVALID,
	IO_FDOOR_OPEN,
	IO_FDOOR_CLOSE,
	IO_ALARM_ON,
	IO_ALARM_OFF,
	IO_POWDETM_VALID,
	IO_POWDETM_INVALID,
	IO_JIJIA_HEAVY,
	IO_JIJIA_LIGHT,
	IO_CHEDENG_VALID,
	IO_CHEDENG_INVALID,
	IO_ZHENLING_VALID,
	IO_ZHENLING_INVALID,
//	IO_SPIDO_CARD_EXIST,
//	IO_SPIDO_CARD_NOTEXIST,
//	IO_SPICK_CARD_PROTECT,
//	IO_SPICK_CARD_NOTPROTECT,
			
	// 输出
	IO_SYSLEDPOW_RED_ON,
	IO_SYSLEDPOW_RED_OFF,
	IO_SYSLEDPOW_GREEN_ON,
	IO_SYSLEDPOW_GREEN_OFF,
	IO_SCHEDULEPOW_ON, 
	IO_SCHEDULEPOW_OFF, 
	IO_TRUMPPOW_ON,
	IO_TRUMPPOW_OFF,
	IO_EARPHONESEL_ON,	// 选择耳机
	IO_EARPHONESEL_OFF,	// 选择免提
	IO_AUDIOSEL_PHONE,	// 选择手机音频
	IO_AUDIOSEL_PC,		// 选择PC音频
	IO_PHONERST_HI,
	IO_PHONERST_LO,
	IO_PHONEPOW_ON,
	IO_PHONEPOW_OFF,
	IO_YUYIN_ON,
	IO_YUYIN_OFF,
	IO_OILELECCTL_CUT, 
	IO_OILELECCTL_RESUME,
	IO_VIDICONPOW_ON, 
	IO_VIDICONPOW_OFF,
	IO_TW2865RST_HI,
	IO_TW2865RST_LO,
	IO_LCDPOW_ON,
	IO_LCDPOW_OFF,
	IO_USBPOW_ON,
	IO_USBPOW_OFF,
};
#endif

#if VEHICLE_TYPE == VEHICLE_M2
// 必须与SYSIO_CFG[]的定义顺序一致!且必须从0开始编号,注意编号取值范围: 0~254 (255已作为函数特殊参数使用)
enum IOSymb
{
	// 输入
	IOS_ACC = 0,
	IOS_QIANYA,			// 欠压信号
	IOS_FDOOR_ALERT,	// 前门报警
	IOS_FDOOR,			// 前门信号
	IOS_BDOOR_ALERT,	// 后门报警
	IOS_BDOOR,			// 后门信号
	IOS_ALARM,			// 劫警信号
	IOS_JIAOSHA,		// 脚刹信号
	IOS_POWDETM,		// 电源输入检测信号
	IOS_CHEDENG,		// 车灯输入信号
	IOS_ZHENLING,		// 振铃信号
//	IOS_SPIDO,			// SD卡检测信号
//	IOS_SPICK,			// SD卡写保护信号
	
	// 输出
	IOS_SYSLEDPOW_RED,	// 工作指示灯（红）
	IOS_SYSLEDPOW_GREEN,// 工作指示灯（绿）
	IOS_SCHEDULEPOW,	// 调度屏电源
	IOS_TRUMPPOW,		// 音频功放电源
	IOS_EARPHONESEL,	// 耳机/免提选择
	IOS_AUDIOSEL,		// 手机音频/PC音频选择
	IOS_PHONERST,		// 手机模块复位
	IOS_PHONEPOW,		// 手机模块电源
	IOS_YUYIN,			// 语音切换开关
	IOS_JIDIANQI,		// 备用继电器开关
	IOS_OILELECCTL,		// 断油电控制
	IOS_VIDICONPOW,		// 摄像头/TW2835电源
	IOS_TW2865RST,		// TW2865复位
	IOS_LCDPOW,			// LCD电源控制
//	IOS_USBPOW,			// USB2.0电源
	IOS_HUBPOW,			// HUB电源控制
	IOS_HARDPOW,		// 硬盘电源控制
};

// IO应用意义定义,针对外部IO信号,非CPU管脚信号
enum IO_APP
{
	// 输入
	IO_ACC_ON = 1, // 必须从1开始,因为内部数组用的是unsigned char,节约空间
	IO_ACC_OFF,
	IO_QIANYA_VALID,
	IO_QIANYA_INVALID,
	IO_FDOORALERT_VALID,
	IO_FDOORALERT_INVALID,
	IO_FDOOR_OPEN,
	IO_FDOOR_CLOSE,
	IO_BDOORALERT_VALID,
	IO_BDOORALERT_INVALID,
	IO_BDOOR_OPEN,
	IO_BDOOR_CLOSE,
	IO_ALARM_ON,
	IO_ALARM_OFF,
	IO_JIAOSHA_VALID,
	IO_JIAOSHA_INVALID,
	IO_POWDETM_VALID,
	IO_POWDETM_INVALID,
	IO_CHEDENG_VALID,
	IO_CHEDENG_INVALID,
	IO_ZHENLING_VALID,
	IO_ZHENLING_INVALID,
	//	IO_SPIDO_CARD_EXIST,
	//	IO_SPIDO_CARD_NOTEXIST,
	//	IO_SPICK_CARD_PROTECT,
	//	IO_SPICK_CARD_NOTPROTECT,
	
	// 输出
	IO_SYSLEDPOW_RED_ON,
	IO_SYSLEDPOW_RED_OFF,
	IO_SYSLEDPOW_GREEN_ON,
	IO_SYSLEDPOW_GREEN_OFF,
	IO_SCHEDULEPOW_ON, 
	IO_SCHEDULEPOW_OFF, 
	IO_TRUMPPOW_ON,
	IO_TRUMPPOW_OFF,
	IO_EARPHONESEL_ON,	// 选择耳机
	IO_EARPHONESEL_OFF,	// 选择免提
	IO_AUDIOSEL_PHONE,	// 选择手机音频
	IO_AUDIOSEL_PC,		// 选择PC音频
	IO_PHONERST_HI,
	IO_PHONERST_LO,
	IO_PHONEPOW_ON,
	IO_PHONEPOW_OFF,
	IO_YUYIN_ON,
	IO_YUYIN_OFF,
	IO_JIDIANQI_ON,
	IO_JIDIANQI_OFF,
	IO_OILELECCTL_CUT, 
	IO_OILELECCTL_RESUME,
	IO_VIDICONPOW_ON, 
	IO_VIDICONPOW_OFF,
	IO_TW2865RST_HI,
	IO_TW2865RST_LO,
	IO_LCDPOW_ON,
	IO_LCDPOW_OFF,
//	IO_USBPOW_ON,
//	IO_USBPOW_OFF,
	IO_HUBPOW_ON,
	IO_HUBPOW_OFF,
	IO_HARDPOW_ON,
	IO_HARDPOW_OFF,
};
#endif

struct tagIOCfg
{
	unsigned int m_uiPinNo; // 管脚编号
	unsigned int m_uiContPrid; // 中断信号持续时间(使能中断时才有意义),去抖用,ms
	unsigned char m_ucInOut;	// 0,输入; 1,输出
	bool m_bBreak;	// true,使能中断(输入时才有意义); false,禁止中断
	char m_cBreakType; // 中断方式(使能中断时才有意义): 1,电平触发; 2,单边沿触发; 3,双边沿触发
	char m_cBreakPow; // 中断触发电平(在电平触发和单边沿触发时才有意义): 0, 低电平或下降沿触发; 1,高电平或上升沿触发
	unsigned char m_ucPinLowApp; // 管脚低电平的应用意义
	unsigned char m_ucPinHigApp; // 管脚高电平的应用意义
};

#define TCP_MAX_LOG_TIMERS	3	// TCP最大连续登陆失败次数

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 30
#define TCPFRAME_BASELEN	3	// TCP数据帧基础长度,不含头尾7e和用户数据
#define UDPFRAME_BASELEN	3	// UDP数据帧基础长度,不含用户数据
#define QIANDOWNFRAME_BASELEN	36	// 下行的SMS数据帧基础长度,包括校验和
#define QIANUPTCPFRAME_BASELEN		6	// 上行的TCP SMS数据帧基础长度,包括校验和
#define QIANUPUDPFRAME_BASELEN		21	// 上行的UDP SMS数据帧基础长度,包括校验和
#endif

#if USE_PROTOCOL == 2
#define TCPFRAME_BASELEN	4	// TCP数据帧基础长度,不含头尾7e和用户数据
#define UDPFRAME_BASELEN	4	// UDP数据帧基础长度,不含用户数据
#define QIANDOWNFRAME_BASELEN	37	// 下行的SMS数据帧基础长度,包括校验和
#define QIANUPTCPFRAME_BASELEN		22	// 上行的TCP SMS数据帧基础长度,包括校验和
#define QIANUPUDPFRAME_BASELEN		22	// 上行的UDP SMS数据帧基础长度,包括校验和
#endif

STRUCT_1 tagGPRS01
{
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
	BYTE		m_bytParCount;		// 固定为1
	BYTE		m_bytParType_1;		// 固定为0x01
	BYTE		m_bytParLen_1;		// 固定为15
	char		m_szParData[15];	// 固定为手机号,后面不足补空格
	BYTE		m_bytParType_2;
	BYTE		m_bytParLen_2;
	BYTE		m_bytVersion;
	
	tagGPRS01()
	{
		m_bytParCount = 2;
		m_bytParType_1 = 1;
		m_bytParLen_1 = 15;
		m_bytParType_2 = 2;
		m_bytParLen_2 = 1;
		m_bytVersion = 0x31;
	}
#endif
	
#if 2 == USE_PROTOCOL
	//用户识别码（6）	参数个数（1）	参数1类 型（1）	参数1长度（1）	参数1数据（n）	参数2类型（1）	参数2长度（1）	参数2数据（n）	……
	char		m_szUserIdenCode[6];
	BYTE		m_bytParCount;		// 固定为1
	BYTE		m_bytParType_1;		// 固定为0x01
	BYTE		m_bytParLen_1;		// 固定为15
	char		m_szParData[15];	// 固定为手机号,后面不足补空格
	
	tagGPRS01()
	{
		memset( this, 0, sizeof(*this) );
		m_bytParCount = 1;
		m_bytParType_1 = 1;
		m_bytParLen_1 = 15;
	}
#endif
};


#pragma pack(8)

typedef struct tagGPSData
{
	double		latitude;         //纬度0－90
	double		longitude;        //经度0－180
	float		speed;           //速度，米/秒
	float		direction;        //方向，0－360
	float		mag_variation;    //磁变角度，0－180
	int			nYear; // 4个数字表示的值,如2007,而不是7;
	int			nMonth;
	int			nDay;
	int			nHour;
	int			nMinute;
	int			nSecond;
	char		valid;             //定位数据是否有效
	BYTE		la_hemi;          //纬度半球'N':北半球，'S':南半球
    BYTE		lo_hemi;           //经度半球'E':东半球，'W':西半球
	BYTE		mag_direction;    //磁变方向，E:东，W:西
	BYTE		m_bytMoveType; // 1,移动; 2,静止
	BYTE		m_bytPDop; // GPS误差因子×10,下同
	BYTE		m_bytVDop;
	BYTE		m_bytHDop;
	char		m_cFixType; // 定位类型: '1'-未定位; '2'-2D定位; '3'-3D定位
	
	tagGPSData(char c)
	{
		Init();
	}
	tagGPSData()
	{
		Init();
	}
	void Init()
	{
		// 初始化不同,dll的处理可能不同,注意!
		memset( this, 0, sizeof(*this) );
		nYear = VALIDYEAR_BEGIN, nMonth = VALIDMONTH_BEGIN, nDay = 15;
		valid = 'V', m_cFixType = '1';
	}
}GPSDATA;

#pragma pack()


#pragma pack(8)

/// 命名规则:
// (1)属于重要配置的,前缀用"1";属于一般配置的,前缀用"2"
// (2)公共配置,前缀用"P";流媒体配置,前缀用"L";千里眼配置,前缀用"Q"
// (3)一级配置定义,用"Cfg"结尾; 二级配置定义,用"Par"结尾

/// 以下的电话号码,若无特殊说明,不满都是补空格

struct tag1PComuCfg
{
	char		m_szDialCode[33];
	char		m_szSmsCentID[22];	// 短信中心号,不满填0
	char		m_szTel[16];		// 车台手机号,以NULL结尾
	int			m_iRegSimFlow;		// 注册SIM卡流程
	byte		m_PhoneVol;			// 电话音量
//	char		m_szUserIdentCode[6]; // 用户识别码	
	
	char        m_szCdmaUserName[50];
	char        m_szCdmaPassWord[30];

	char		m_szTernimalID[8];	// 登陆维护中心时的移动台ID号

#if USE_LIAONING_SANQI == 1
	bool		m_bRegsiter;			// 是否已向中心自注册
	int			m_iHeartKeepInterval;	// 心跳间隔
#endif

	void Init(const char *v_szCdmaUserName , const char *v_szCdmaPassWord)
	{
		memset( this, 0, sizeof(*this) );
		strncpy( m_szSmsCentID, SMSCENTID, sizeof(m_szSmsCentID) );
		m_szSmsCentID[ sizeof(m_szSmsCentID) - 1 ] = 0;
		strncpy( m_szCdmaUserName , v_szCdmaUserName, sizeof(m_szCdmaUserName) );
		m_szCdmaUserName[sizeof(m_szCdmaUserName) -1] = 0;
		strncpy( m_szCdmaPassWord , v_szCdmaPassWord, sizeof(m_szCdmaPassWord) );
		m_szCdmaPassWord[sizeof(m_szCdmaPassWord) -1] = 0;
		m_iRegSimFlow = 1;
		m_PhoneVol = 3;		//0-5

#if USE_LIAONING_SANQI == 1
		m_bRegsiter = false;
		m_iHeartKeepInterval = 20;//60;
#endif
	}
};

struct tag1LComuCfg
{
	unsigned long m_ulLiuIP; // 流媒体升级中心IP
	unsigned long m_ulLiuIP2; // 维护管理平台中心IP
	unsigned short m_usLiuPort;
	unsigned short m_usLiuPort2;
	char	m_szLiuSmsTel[15];
	char	m_szVeIDKey[20]; // 实际已不用,已单独另外放置
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );
		m_ulLiuIP = inet_addr(LUDPIP); 
		m_usLiuPort = htons(LUDPPORT);
	}
};

struct tag1LMediaCfg
{
	BYTE		m_bytPlayTimes; // 流媒体播放次数
	//unsigned long m_aryDriverID[5]; // 本车注册司机ID
	void Init()
	{
		memset( this, 0, sizeof(*this) );
		m_bytPlayTimes = 1;
	}
};

// 注意：此tag1QIpCfg结构体因关系到恢复出厂配置的关键问题，不得增加或减少成员变量！！！！！！！！！！
struct tag1QIpCfg
{
	unsigned long m_ulQianTcpIP; // (inet_addr之后的,下同)
	unsigned long m_ulVUdpIP; // 视频上传UDP IP
	unsigned long m_ulQianUdpIP;
	unsigned short m_usQianTcpPort; // (htons之后的，下同) 
	unsigned short m_usVUdpPort;
	unsigned short m_usQianUdpPort;
	char		m_szApn[20]; // 以NULL结尾

	void Init( unsigned long v_ulQianTcpIP, unsigned short v_usQianTcpPort,
				unsigned long v_ulVUdpIP, unsigned short v_usVUdpPort,
				unsigned long v_ulQianUdpIP, unsigned short v_usQianUdpPort,
		const char *const v_szApn )
	{
		memset(this, 0, sizeof(*this));
		strncpy( m_szApn, v_szApn, sizeof(m_szApn) - 1 );

		m_ulQianTcpIP = v_ulQianTcpIP, m_usQianTcpPort = v_usQianTcpPort;
		m_ulVUdpIP = v_ulVUdpIP,   m_usVUdpPort = v_usVUdpPort;
		m_ulQianUdpIP = v_ulQianUdpIP, m_usQianUdpPort = v_usQianUdpPort;
	}
};

struct tag1QBiaoding
{
	WORD		m_wCyclePerKm; // 每公里脉冲数
	//HXD 081120 油耗相关的参数。
	BYTE		m_bReverse;
	BYTE	    m_bFiltrate;
	BYTE	    m_bytHiVal;
	BYTE		m_bytLoVal;

	void Init()
	{
		memset( this, 0, sizeof(*this) );
	}
};

//油耗标定
struct tag1QOilBiaodingCfg
{
	unsigned short	   m_aryONOilBiaoding[5][2];
	unsigned short	   m_aryOFFOilBiaoding[5][2];
	
	void Init()
	{
		memset( this, 0, sizeof(*this));
	}
};

// 根据20070914讨论,认为配置区应按照业务进行分块,并且每块大小应是40的整数倍
struct tagImportantCfg
{
	char		m_szSpecCode[40];

	// 公共通信参数
	union
	{
		char		m_szReserved[160];
		tag1PComuCfg m_obj1PComuCfg;
	} m_uni1PComuCfg;

	// 流媒体通信参数
	union
	{
		char		m_szReserved[160];
		tag1LComuCfg m_obj1LComuCfg;
	} m_uni1LComuCfg;

	// 流媒体播放配置
	union
	{
		char		m_szReserved[120];
		tag1LMediaCfg m_obj1LMediaCfg;
	} m_uni1LMediaCfg;

	// 千里眼IP
	union
	{
		char		m_szReserved[200];
		tag1QIpCfg	m_ary1QIpCfg[3];
	} m_uni1QComuCfg;

	// 油耗标定值
	union
	{
		char		m_szReserved[80];
		tag1QOilBiaodingCfg	m_obj1QOilBiaodingCfg;
	} m_uni1QOilBiaodingCfg;

	// IO数据方面的标定值
	union
	{
		char		m_szReserved[80];
		tag1QBiaoding m_obj1QBiaodingCfg;
	} m_uni1QBiaodingCfg;

	char		m_szReUpdate[40];

	void InitDefault()
	{
		memset(this, 0, sizeof(*this));
		strncpy( m_szSpecCode, CFGSPEC, sizeof(m_szSpecCode) - 1 );
		strncpy( m_szReUpdate, NEED_REUPDATE, sizeof(m_szReUpdate) - 1 );

		m_uni1PComuCfg.m_obj1PComuCfg.Init(CDMA_NAME,CDMA_PASS);
		m_uni1LComuCfg.m_obj1LComuCfg.Init();
		m_uni1LMediaCfg.m_obj1LMediaCfg.Init();
		m_uni1QComuCfg.m_ary1QIpCfg[0].Init( inet_addr(QTCPIP), htons(QTCPPORT),
											inet_addr(LUDPIP), htons(LUDPPORT),
											inet_addr(QUDPIP), htons(QUDPPORT),	APN);

		m_uni1QComuCfg.m_ary1QIpCfg[1].Init( inet_addr(DTCPIP), htons(DTCPPORT),
											inet_addr(LUDPIP), htons(LUDPPORT),
											inet_addr(DUDPIP), htons(DUDPPORT),	APN);

		m_uni1QBiaodingCfg.m_obj1QBiaodingCfg.Init();
		m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg.Init();
	}
};

struct tag2LMediaCfg
{
	DWORD		m_dwSysVolume; // 系统音量
	byte        m_bytInvPhoto; //090611 是否要颠倒照片
	void Init()
	{
		memset(this, 0, sizeof(*this));
		m_dwSysVolume = 75;
	}
};

struct tag2QBlkCfg
{
	DWORD			m_dwBlackChkInterv; // 单位:秒

	void Init()
	{
		memset(this, 0, sizeof(*this));
		m_dwBlackChkInterv = 30; // 安全起见
	}
};

struct tag2QRealPosCfg // 432
{
	double	m_dLstLon;
	double	m_dLstLat;
	double	m_dLstSendDis; // 上次发送数据之后累积行驶的距离 (米)

	float	m_fSpeed; // 米/秒, 0表示无效

	DWORD	m_dwLstSendTime; // 上次发送时刻

	long	m_aryArea[16][4]; // 每个范围依次是: 左下经度、左下纬度、右上经度、右上纬度 (都是10的-6次方度)
	DWORD	m_aryAreaContCt[16]; // 满足条件的计数 (元素个数与上面一致)
	BYTE	m_aryAreaProCode[16]; // 各范围的编号 (元素个数与上面一致)
	BYTE	m_bytAreaCount;

	BYTE	m_bytGpsType;
	WORD	m_wSpaceInterv; // 公里,0表示无效
	WORD	m_wTimeInterv; // 分钟,0表示无效

	char	m_szSendToHandtel[15];

	BYTE	m_aryTime[10][3]; // 每个时刻依次是: 序号,时,分
	BYTE	m_bytTimeCount;

	BYTE	m_aryStatus[6];
	// 固定为6个, BYTE	m_bytStaCount; 

	BYTE	m_bytCondStatus; // 0	1	I	T	V	A	S	0
						//	Ｉ：关闭/开启间隔条件，0-关闭，1-开启
						//	Ｔ：关闭/开启时刻条件，0-关闭，1-开启
						//	Ｖ：关闭/开启速度条件，0-关闭，1-开启
						//	Ａ：关闭/开启范围条件，0-关闭，1-开启
						//	Ｓ：关闭/开启状态条件，0-关闭，1-开启

	BYTE	m_bytLstTimeHour; // 上次符合时刻条件的时刻,初始化为无效值——0xff,
	BYTE	m_bytLstTimeMin;

	void Init()
	{
		memset(this, 0, sizeof(*this));
		m_bytLstTimeHour = m_bytLstTimeMin = 0xff;
		m_wTimeInterv = 0, m_wSpaceInterv = 0, m_bytCondStatus = 0x40;
	}
};

// 越界区域设置，其实跟实时定位中的范围条件一样，
// 但为了不和实时定位冲突，此处再增加一个定义，用于“越界报警参数设置业务（业务类型：0FH）”
struct tag2QAreaCfg	
{	
	BYTE	m_bytAreaCount;
	BYTE	m_aryAreaProCode[6];	// 各范围的编号 
	long	m_aryArea[6][4];		// 每个范围依次是: 左下经度、左下纬度、右上经度、右上纬度 (都是10的-6次方度)
	DWORD	m_aryAreaContCt[6];		// 满足条件的计数 (元素个数与上面一致)
	BYTE	m_aryReserve[3];		// 保留3个字节，用于对齐
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );
	}
};

struct tag2QGpsUpPar // 保存在文件中
{
	WORD		m_wMonSpace; // 监控定距距离 （单位：10米）
	WORD		m_wMonTime; // 监控时间 （分钟），0xffff表示永久监控
	WORD		m_wMonInterval; // 监控间隔 （秒），至少为1
	BYTE		m_bytStaType; // 注意: 按照协议里的状态定义
	BYTE		m_bytMonType; // 协议中定义的监控类型

	void Init()
	{
		memset( this, 0, sizeof(*this) );
		m_wMonInterval = 0xffff;
		m_wMonSpace = 0xffff;
	}
};

struct tag2QGprsCfg	// 42
{
	tag2QGpsUpPar	m_aryGpsUpPar[4];
	BYTE		m_bytGprsOnlineType; // 在线方式
	BYTE		m_bytOnlineTip; // 在线方式提示
	BYTE		m_bytGpsUpType; // GPS数据上传标识 1,主动发送0154帧；2,不主动发送
	BYTE		m_bytGpsUpParCount; // GPS数据参数个数
	BYTE		m_bytChannelBkType_1; // 短信备份设置1
		// 0	H1	H2	H3	H4	H5	H6	1
		// H1: 监控, H2: 实时定位, H3: 位置查询, H4: 报警, H5: 求助, H6: 调度
	BYTE		m_bytChannelBkType_2; // 短信备份设置2
		// 0	L1	L2	L3	L4	L5	L6	1
		// L1: 黑匣子, L2: 税控传输

	BYTE		m_bytRptBeginHour;	// 主动上报时间段,若开始时间==结束时间,则表示无时间段限制
	BYTE		m_bytRptBeginMin;
	BYTE		m_bytRptEndHour;
	BYTE		m_bytRptEndMin;

	void Init()
	{
		memset( this, 0, sizeof(*this) );

		// 关闭所有短信备份，短信备份功能已在程序里固定，并注意中心修改短信备份的指令要屏蔽
		m_bytChannelBkType_1 = m_bytChannelBkType_2 = 0;
		
		m_bytGpsUpType = 2;
		for( unsigned int ui = 0; ui < sizeof(m_aryGpsUpPar) / sizeof(m_aryGpsUpPar[0]); ui ++ )
			m_aryGpsUpPar[ui].Init();
	}
};

struct tag2QServCodeCfg
{
	char		m_szAreaCode[2]; // 区域号(中心ID)
	char		m_szQianSmsTel[15]; // 短信特服号(报警特服号),不满填0
	char		m_szTaxHandtel[15]; // 税控特服号
	char		m_szDiaoduHandtel[15]; // 调度特服号
	char		m_szHelpTel[15]; // 求助号码
	
	BYTE		m_bytListenTelCount;
	char		m_aryLsnHandtel[8][15]; // 中心主动发起监听的电话 (协议里定义是最多为5个)

	BYTE		m_bytCentServTelCount;
	char		m_aryCentServTel[5][15]; // 中心服务电话

	BYTE		m_bytUdpAtvTelCount; // UDP激活电话号码
	char		m_aryUdpAtvTel[10][15];

	void Init()
	{
		memset( this, 0, sizeof(*this) );
		m_szAreaCode[0] = CENTID1;
		m_szAreaCode[1] = CENTID2;
		strncpy( m_szQianSmsTel, SMSSERVCODE, sizeof(m_szQianSmsTel) );
		m_szQianSmsTel[ sizeof(m_szQianSmsTel) - 1 ] = 0;
	}
};

struct tag2QListenCfg
{
	BYTE		m_bytListenTelCount;
	char		m_aryLsnHandtel[8][15]; // 中心主动发起监听的电话 (协议里定义是最多为5个)

	void Init()
	{
		memset( this, 0, sizeof(*this) );
	}
};

struct tag2QFbidTelCfg
{
	BYTE		m_bytFbidInTelCount;
	BYTE		m_bytAllowInTelCount;
	BYTE		m_bytFbidOutTelCount;
	BYTE		m_bytAllowOutTelCount;
	BYTE		m_bytFbidType; // 和协议里的定义方式一致
	char		m_aryFbidInHandtel[12][15]; // 禁止呼入的电话 (协议里定义是最多为8个)
	char		m_aryAllowInHandtel[12][15]; // 允许呼入的电话
	char		m_aryFbidOutHandtel[12][15]; // 禁止呼出的电话
	char		m_aryAllowOutHandtel[12][15]; // 允许呼出的电话
		// 高4位,0允许所有呼出,1禁止所有呼出,2允许部分呼出但又禁止某些呼出,3禁止部分呼出,但又允许某些呼出
		// 低4位,0允许所有呼入,1禁止所有呼入,2允许部分呼入但又禁止某些呼入,3禁止部分呼入,但又允许某些呼入

	void Init()
	{
		memset( this, 0, sizeof(*this) );
	}
};

struct tag2QReportPar
{
	char	m_szRID[20];
	bool	m_bEnable;
	byte	m_bytPriority;
	ushort	m_usCnt;
	ushort	m_usInterval;
	
	void Init(char *v_szRID, bool v_bEnable, byte v_bytPriority, ushort v_dwCnt, ushort v_dwInterval)
	{
		strncpy(m_szRID, v_szRID, sizeof(m_szRID)-1);
		m_bEnable = v_bEnable;
		m_bytPriority = v_bytPriority;
		m_usCnt = v_dwCnt;
		m_usInterval = v_dwInterval;
	}
};

struct tag2QReportCfg
{
	tag2QReportPar	m_objFootRptPar;
	tag2QReportPar	m_objDoorRptPar;
	tag2QReportPar	m_objPowOffRptPar;
	tag2QReportPar	m_objAccRptPar;
	tag2QReportPar	m_objOverSpdRptPar;
	tag2QReportPar	m_objBelowSpdPrtPar;
	tag2QReportPar	m_objMilePrtPar;
//	tag2QReportPar  m_objStopTimePrtPar;
	tag2QReportPar  m_objStartEngPrtPar;
	tag2QReportPar  m_objTirePrtPar;
	
	void Init()
	{
		m_objFootRptPar.Init("r001", true, 0x00, 5, 30);
		m_objDoorRptPar.Init("r001", false, 0x00, 1, 20);
		m_objPowOffRptPar.Init("r001", false, 0x00, 1, 20);
		m_objAccRptPar.Init("r001", true, 0x00, 1, 20);
		m_objOverSpdRptPar.Init("r001", false, 0x00, 1, 20);
		m_objBelowSpdPrtPar.Init("r001", false, 0x00, 1, 20);
		m_objMilePrtPar.Init("r001", false, 0x00, 1, 20);
//		m_objStopTimePrtPar.Init("r001", false, 0x00, 1, 20*1000);
		m_objStartEngPrtPar.Init("r001", false, 0x00, 1, 20);
	}
};

#define MAX_AUTORPT_CNT	20

struct tag2QAutoRptCfg
{
	bool	m_bNeedReport[MAX_AUTORPT_CNT];
	char	m_szId[MAX_AUTORPT_CNT][20];
	char	m_szBeginTime[MAX_AUTORPT_CNT][6];
	char	m_szIntervalType[MAX_AUTORPT_CNT];
	DWORD	m_dwLastRptValue[MAX_AUTORPT_CNT];
	DWORD	m_dwIntervalValue[MAX_AUTORPT_CNT];
	char	m_szEndType[MAX_AUTORPT_CNT];
	char	m_szEndTime[MAX_AUTORPT_CNT][6];
	DWORD	m_dwReportCnt[MAX_AUTORPT_CNT];
	DWORD	m_dwMaxCnt[MAX_AUTORPT_CNT];
	
	void Init()
	{
		memset(this, 0, sizeof(*this));
		
		for(int i=0; i<MAX_AUTORPT_CNT; i++)
		{
			m_bNeedReport[i] = false;
		}
	}
};

#define MAX_AREA_COUNT	8	// 最多允许设置10个区域10	
#define MAX_TIME_COUNT	6		// 每个区域最多允许设置6个时间段

struct tag2QAreaRptPar
{
	bool	m_bEnable;							// 是否使能
	char	m_szRID[20];
	ulong	m_ulIndex;
	byte	m_bytPriority;
	ushort  m_usCnt;
	ushort	m_usInterval;
	byte	m_bytAreaAttr;						// 区域属性：01h--进区域；02h--出区域；03h--区域内；04h--区域外。
	bool	m_bTimeEnable[MAX_TIME_COUNT];		// 时间段是否有效（最多允许设置5个时间段）
	char	m_szBeginTime[MAX_TIME_COUNT][3];	// 时间段范围
	char	m_szEndTime[MAX_TIME_COUNT][3];
	long	m_lMaxLat;							// 区域范围（现在只支持矩形区域）
	long	m_lMinLat;
	long	m_lMaxLon;
	long	m_lMinLon;

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct tag2QAreaRptCfg
{
	tag2QAreaRptPar	m_objArea[MAX_AREA_COUNT];
	tag2QAreaRptPar	m_objInArea[MAX_AREA_COUNT];
	tag2QAreaRptPar	m_objOutArea[MAX_AREA_COUNT];
	tag2QAreaRptPar	m_objInRegion[MAX_AREA_COUNT];
	tag2QAreaRptPar	m_objOutRegion[MAX_AREA_COUNT];
	
	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct tag2QTimeSpdRptPar
{
	bool	m_bEnable;							// 是否使能
	char	m_szRID[20];
	ulong	m_ulIndex;
	byte	m_bytspd;
	byte	m_bytPriority;
	ushort	m_usTimeval;
	bool	m_bTimeEnable[MAX_TIME_COUNT];		// 时间段是否有效（最多允许设置5个时间段）
	char	m_szBeginTime[MAX_TIME_COUNT][3];	// 时间段范围
	char	m_szEndTime[MAX_TIME_COUNT][3];

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};
#define MAX_TIMESPD_COUNT 1
struct tag2QTimeSpdRptCfg
{
	tag2QTimeSpdRptPar	m_TimeSpd[MAX_TIMESPD_COUNT];

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct tag2QAreaSpdRptPar
{
	bool	m_bEnable;							// 是否使能
	char	m_szRID[20];
	ulong	m_ulIndex;
	byte	m_bytPriority;
	ushort  m_usCnt;
	ushort	m_usInterval;
	byte	m_bytAreaAttr;						// 区域属性：01h--进区域；02h--出区域；03h--区域内；04h--区域外。
	byte 	m_bytSpeed;
	ushort 	m_usTimeval;											//
	bool	m_bTimeEnable[MAX_TIME_COUNT];		// 时间段是否有效（最多允许设置5个时间段）
	byte	m_szBeginTime[MAX_TIME_COUNT][3];	// 时间段范围
	byte	m_szEndTime[MAX_TIME_COUNT][3];
	long	m_lMaxLat;							// 区域范围（现在只支持矩形区域）
	long	m_lMinLat;
	long	m_lMaxLon;
	long	m_lMinLon;

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct tag2QAreaSpdRptCfg
{
	tag2QAreaSpdRptPar	m_objAreaSpd[MAX_AREA_COUNT];
	tag2QAreaSpdRptPar	m_objInRegionSpd[MAX_AREA_COUNT];
	tag2QAreaSpdRptPar	m_objOutRegionSpd[MAX_AREA_COUNT];
	tag2QAreaSpdRptPar	m_TimeSpd[MAX_TIMESPD_COUNT];
	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct tag2QAreaParkTimeRptPar
{
	bool	m_bEnable;							// 是否使能
	char	m_szRID[20];
	ulong	m_ulIndex;
	byte	m_bytPriority;
	ushort  m_usCnt;
	ushort	m_usInterval;
	byte	m_bytAreaAttr;						// 区域属性：01h--进区域；02h--出区域；03h--区域内；04h--区域外。
	ushort 	m_usParkTime;											//
	bool	m_bTimeEnable[MAX_TIME_COUNT];		// 时间段是否有效（最多允许设置5个时间段）
	char	m_szBeginTime[MAX_TIME_COUNT][3];	// 时间段范围
	char	m_szEndTime[MAX_TIME_COUNT][3];
	long	m_lMaxLat;							// 区域范围（现在只支持矩形区域）
	long	m_lMinLat;
	long	m_lMaxLon;
	long	m_lMinLon;

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct tag2QAreaParkTimeRptCfg
{
	tag2QAreaParkTimeRptPar	m_objAreaParkTime[MAX_AREA_COUNT];
	tag2QAreaParkTimeRptPar	m_objInRegionParkTime[MAX_AREA_COUNT];
	tag2QAreaParkTimeRptPar	m_objOutRegionParkTime[MAX_AREA_COUNT];
	tag2QAreaParkTimeRptPar	m_objParkTime;
	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

#define MAX_POINT_COUNT 100
#define MAX_LINE_COUNT  5
struct tag2QLineRptPar
{
	bool	m_bEnable;							// 是否使能
	char	m_szRID[20];
	ulong	m_ulIndex;
	byte	m_bytPriority;
	ushort  m_usCnt;
	ushort	m_usInterval;
	ushort 	m_usOutval;											//偏离距离单位米
	bool	m_bTimeEnable[MAX_TIME_COUNT];		// 时间段是否有效（最多允许设置5个时间段）
	byte	m_szBeginTime[MAX_TIME_COUNT][3];	// 时间段范围
	byte	m_szEndTime[MAX_TIME_COUNT][3];
	bool	m_bPointEnable[MAX_POINT_COUNT];
	long	m_lLat[MAX_POINT_COUNT];							
	long	m_lLon[MAX_POINT_COUNT];


	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct tag2QLineRptCfg
{
	tag2QLineRptPar	m_objLine[MAX_LINE_COUNT];
	tag2QLineRptPar	m_objTimeLine[MAX_LINE_COUNT];

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct tag2QLNPhotoPar
{

	bool m_bEnable;
	char m_ExtID[20];
//	byte m_EventType;
	BYTE		m_bytSizeType; // 拍照分辨率
	BYTE		m_bytChannel; // 通道字节低四位表示C4C3C2C1，"1"有效，"0"无效
	BYTE		m_bytQualityType; // 拍照质量 (与协议定义相同)
	byte m_CmSta; //字节高两位C7C6：
                   //C7：0：关闭此条件抓拍, 1：开启
                   //C6：0：不需要主动上传 1：需要
	ushort		m_usInterval; // 拍照间隔,s
	byte	m_bytCTakeBeginTime[6]; //抓拍起始
	byte    m_bytCTakeEndTime[6];

	byte	m_bytTTakeBeginTime[6]; //疲劳起始
	byte    m_bytTTakeEndTime[6];

	byte	m_bytTTakeBaseTime; //疲劳抓拍起始
	byte    m_bytTTakeOfTime;


	BYTE		m_bytTime; // 拍照次数
	BYTE		m_bytBright; // 亮度
	BYTE		m_bytContrast; // 对比度
	BYTE		m_bytHue; // 色调
	BYTE		m_bytBlueSaturation; // 蓝饱和
	BYTE		m_bytRedSaturation; // 红饱和
	char		m_szReserve[7];

	void Init( BYTE v_bytTime, BYTE v_bytSizeType, BYTE v_bytQualityType, BYTE v_bytChannel, bool v_EnShot)
	{
		memset( this, 0, sizeof(*this) );
		m_bytBright = m_bytContrast = m_bytHue = m_bytBlueSaturation = m_bytRedSaturation = 8;
		m_bytTime = v_bytTime;
		m_bytSizeType = v_bytSizeType;
		m_bytQualityType = v_bytQualityType;
		m_bytChannel = v_bytChannel;
		m_bEnable = v_EnShot;
	}
};

struct tag2QLNPhotoCfg
{

	WORD		m_wPhotoEvtSymb; // 拍照触发状态字 (见QianExeDef.h中的规定)
	WORD		m_wPhotoUploadSymb; // 拍照主动上传状态字 (见QianExeDef.h中的规定)

	tag2QLNPhotoPar	m_objAlermPhotoPar; // 报警拍照设置
	tag2QLNPhotoPar	m_objAccPhotoPar; // ACC变化拍照
	tag2QLNPhotoPar	m_objDoorPhotoPar; // 开关车门拍照								  //									
	tag2QLNPhotoPar	m_objJijiaPhotoPar; // 空车/重车变化时的拍照设置
	tag2QLNPhotoPar	m_objOverTurnPhotoPar; // 侧翻拍照设置
	tag2QLNPhotoPar	m_objTimerPhotoPar; // 定时拍照
	tag2QLNPhotoPar	m_objTirePhotoPar; // 疲劳驾驶拍照

	void Init()
	{
		memset( this, 0, sizeof(*this));
		m_wPhotoEvtSymb = PHOTOEVT_FOOTALERM | PHOTOEVT_OPENDOOR | PHOTOEVT_TIRE | PHOTOEVT_ACCON | PHOTOEVT_JIJIADOWN | PHOTOEVT_OVERTURNALERM | PHOTOEVT_TIMER;
//		m_wPhotoUploadSymb = PHOTOUPLOAD_FOOTALERM;
		m_objAlermPhotoPar.Init( 1, 3, 1, 0x0f , false); // v_bytTime, v_bytSizeType, v_bytQualityType, v_bytChannel
		m_objJijiaPhotoPar.Init( 1, 2, 2, 0x0f , false);
		m_objTirePhotoPar.Init( 1, 2, 2, 0x0f , false);
		m_objDoorPhotoPar.Init( 1, 2, 2, 0x0f , false);
		m_objTirePhotoPar.Init( 1, 2, 2, 0x0f , false);
		m_objOverTurnPhotoPar.Init( 1, 2, 2, 0x0f, false);
		m_objAccPhotoPar.Init( 1, 2, 2, 0x0f, false);
	}
};

struct tag2QcgvCfg
{
	char m_temiver[11];
	byte m_termiverlen;
	char m_companyid[10];
	byte m_companyidlen;
	char m_groupid[21];
	byte m_groupidlen;
	void Init()
	{
		m_termiverlen = strlen(SOFT_VERSION);
		memcpy(m_temiver, SOFT_VERSION, m_termiverlen);
		m_companyidlen = strlen(COMPANY_ID);
		memcpy(m_companyid, COMPANY_ID, m_companyidlen);
		m_groupidlen = strlen(GROUP_BILLING_ID);
		memcpy(m_groupid, GROUP_BILLING_ID, m_groupidlen);
	}
};

struct tag2QPhotoPar
{
	BYTE		m_bytTime; // 拍照次数
	BYTE		m_bytSizeType; // 拍照分辨率 (与协议定义不同)
							// 1,最小
							// 2,中等
							// 3,最大
	BYTE		m_bytQualityType; // 拍照质量 (与协议定义相同)
							// 0x01：恒定质量等级（高）
							// 0x02：恒定质量等级（中）（默认）
							// 0x03：恒定质量等级（低）
	BYTE		m_bytChannel; // 通道 0x01,通道1; 0x02,通道2; 0x03,通道1和通道2
	BYTE		m_bytInterval; // 拍照间隔,s
	BYTE		m_bytBright; // 亮度
	BYTE		m_bytContrast; // 对比度
	BYTE		m_bytHue; // 色调
	BYTE		m_bytBlueSaturation; // 蓝饱和
	BYTE		m_bytRedSaturation; // 红饱和
	BYTE		m_bytDoorPar; // 仅用于关车门拍照,低四位表示速度限制：0-15（公里/小时）,高四位表示该速度持续时间：0-15（分钟）
	//090611 hxd
	BYTE		m_bytIntervHour; // 拍照间隔，小时（JG2）小时和分钟全0,视为不定时拍照
	BYTE		m_bytIntervMin; // 拍照间隔,分钟(JG2)
	char		m_szReserve[7];

	void Init( BYTE v_bytTime, BYTE v_bytSizeType, BYTE v_bytQualityType, BYTE v_bytChannel )
	{
		memset( this, 0, sizeof(*this) );
		m_bytBright = m_bytContrast = m_bytHue = m_bytBlueSaturation = m_bytRedSaturation = 8;
		m_bytTime = v_bytTime;
		m_bytSizeType = v_bytSizeType;
		m_bytQualityType = v_bytQualityType;
		m_bytChannel = v_bytChannel;
	}
};

struct tag2QAlertCfg
{
	// 报警设置
	BYTE		m_bytAlert_1; // 0	S1	S2	S3	S4	S5	S6	1
		// JG2、JG3、VM1协议：
		// S1：0-关闭欠压报警            1-允许欠压报警
		// S2：0-关闭抢劫报警            1-允许抢劫报警
		// S3；0-关闭震动报警            1-允许震动报警
		// S4：0-关闭非法打开车门报警    1-允许非法打开车门报警
		// S5：0-关闭断路报警            1-允许断路报警
		// S6，保留

		// KJ2协议：
		// S1：0-关闭欠压报警            1-允许欠压报警 (协议上没有,但因为KJ-01测试报告上有此功能,所以补上)
		// S2：0-关闭抢劫报警            1-允许抢劫报警
		// S3；0-关闭碰撞报警            1-允许碰撞报警
		// S4：0-关闭侧翻报警            1-允许侧翻报警
		// S5：0-关闭断路报警            1-允许断路报警 (协议上没有,但因为KJ-01测试报告上有此功能,所以补上)

	BYTE		m_bytAlert_2; // 0	A1	A2	A3	A4	A5	A6	1
		// JG2、JG3、VM1协议：
		// A1：0-关闭越界报警            1-允许越界报警
		// A2：0-关闭超速报警            1-允许超速报警
		// A3：0-关闭停留过长报警        1-允许停留时间过长报警
		// A4：0-关闭时间段检查报警      1-允许时间段检查报警
		// A5：0-关闭低速报警            1-允许低速报警
		// A6, 保留待定

		// KJ2协议：
		// A5：0-关闭非法启动报警        1-允许非法启动报警


	BYTE		m_bytAlermSpd; // 超速报警速度 (海里/小时)
	BYTE		m_bytSpdAlermPrid; // 超速报警的超速持续时间 (秒)

#if USE_LIAONING_SANQI == 1
	BYTE		m_bytOverSpd;		// 超速报警速度 (海里/小时)
	DWORD		m_dwOverPrid;		// 超速报警的超速持续时间 (秒)
	BYTE		m_bytBelowSpd;		// 低速报警速度 (海里/小时）
	DWORD		m_dwBelowPrid;		// 低速报警的低速持续时间（秒）
	//点火报告
	BYTE		m_bytAccSta;
	//里程报告
	ushort		m_usTotalMile;
	//停车报告
//	ushort		m_usParkTime;
	//疲劳驾驶报告
	ushort		m_usDriveTime;
	ushort		m_usRestTime;
	//报告参数
	ushort		m_usInterval;
	ushort		m_usTimes;
	ushort		m_usAlertDelay;
#endif

	void Init()
	{
		// 默认报警全部打开
		m_bytAlert_1 = 0x7f;
		m_bytAlert_2 = 0x7f;

		// 超速报警参数默认设为不报警
		m_bytAlermSpd = 0xff;
		m_bytSpdAlermPrid = 0xff;

#if USE_LIAONING_SANQI == 1
		m_bytOverSpd = 0;
		m_dwOverPrid = 0;
		m_bytBelowSpd = 0;
		m_dwBelowPrid = 0;
		m_bytAccSta = 1;
		m_usTotalMile = 0;
		m_usDriveTime = 0;
		m_usRestTime = 0;
		m_usInterval = 60;
		m_usTimes = 10;
		m_usAlertDelay = 10;
//		m_usParkTime = 0;
#endif
	}
};

// 行驶记录仪相关配置
// (若采样周期为0, 表示不采样; 若发送周期为0,表示不发送; 若窗口大小为0,表示不需要确认不需要可靠传输)
struct tag2QDriveRecCfg
{
	WORD	m_wOnPeriod;	// ACC ON 发送时长 (分钟) (ffff表示永久发送) (千里眼协议时使用)
	WORD	m_wOffPeriod;	// ACC OFF 发送时长 (分钟) (ffff表示永久发送) (千里眼协议时使用)
	WORD	m_wOnSampCycle; // ACC ON 采样周期,秒
	WORD	m_wOffSampCycle; // ACC OFF 采样周期,秒
	BYTE	m_bytSndWinSize; // 发送窗口大小
	BYTE	m_bytOnSndCycle; // ACC ON,发送周期,个
	BYTE	m_bytOffSndCycle; // ACC OFF,发送周期,个

	void Init()
	{
		memset( this, 0, sizeof(*this) );
#if USE_PROTOCOL == 30
		m_wOnPeriod = 0xffff;
		m_wOffPeriod = 0xffff;
		m_wOnSampCycle = 15;
		m_wOffSampCycle = 30;
		m_bytSndWinSize = 5;
		m_bytOnSndCycle = 2;
		m_bytOffSndCycle = 1;
#endif

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
		m_wOnPeriod = 0xffff;
		m_wOffPeriod = 0xffff;
		m_wOnSampCycle = 60;
		m_wOffSampCycle = 1800;
		m_bytSndWinSize = 4;
		m_bytOnSndCycle = 1;
		m_bytOffSndCycle = 1;
#endif
	}
};

// 电话本条目设置
struct tag2QTelBookPar
{
	WORD	m_wLmtPeriod; // 通话时长(分钟),0表示不限时
	BYTE	m_bytType; // 权限类型,同协议定义,即
		// 00H  不允许呼入和呼出
		// 01H  不允许呼入，允许呼出
		// 02H  允许呼入，不许呼出
		// 03H  允许呼入和呼出
	char	m_szTel[15]; // 如果为空,表示该条电话本未启用
	char	m_szName[20];
};

// 电话本配置
struct tag2QCallCfg
{
	BYTE	m_bytSetType; // 设置字
						// 字节定义如下：
						// 0	1	CL	CR	D	D	R	R
						// 
						// CL：1-清除所有电话本设置， 0-保留之前电话本设置
						// DD：呼出电话限制
						// 00：允许所有呼出电话
						// 01：禁止所有呼出电话
						// 10：电话本控制
						// 11：保留
						// RR：呼入电话限制
						// 00：允许所有呼入电话
						// 01：禁止所有呼入电话
						// 10：电话本控制 
						// 11：保留
	tag2QTelBookPar	m_aryTelBook[30];

	void Init()
	{
		memset( this, 0, sizeof(*this) ); // 允许所有呼入呼出
	}
};

struct tag2QCallListCfg
{
	byte	m_bytCtrl;
	int		m_iTelCnt;			// 号码个数
	char	m_szTel[30][15];	// 电话号码
	
	void Init()
	{
		memset( this, 0, sizeof(*this));
		m_bytCtrl = 0x02;		// 默认为允许呼入呼出
	}
	
};


// 中心服务电话号码设置
struct tag2QCentServTelCfg
{
	BYTE	m_bytTelCount;
	char	m_aryCentServTel[5][15];

	void Init() { memset( this, 0, sizeof(*this) ); }
};

// UDP激活电话号码设置
struct tag2QUdpAtvTelCfg
{
	BYTE	m_bytTelCount;
	char	m_aryUdpAtvTel[10][15];

	void Init()
	{
		memset(this, 0, sizeof(*this));
		//strcpy( m_aryUdpAtvTel[0], "05922956773" );
	}
};

// 驾驶员信息 (千里眼系统)
struct tag2QDriverPar_QIAN
{
	WORD	m_wDriverNo; // 编号 0~126, 0表示本信息为空 (实际只用1个字节)
	char	m_szIdentID[7]; // 身份代码,不足补0,也可能全填满
	char	m_szDrivingLic[18]; // 驾驶证号,不足补0,也可能全填满
};

// 驾驶员信息 (2客系统)
struct tag2QDriverPar_KE
{
	WORD	m_wDriverNo; // 编号 0~65534, 0表示本信息为空
	char	m_szIdentID[8]; // 身份代码, 不足后补0；也可能全填满
};

// 驾驶员身份设置
struct tag2QDriverCfg
{
#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
	tag2QDriverPar_QIAN	m_aryDriver[10];
#endif

#if USE_PROTOCOL == 30
	tag2QDriverPar_KE	m_aryDriver[10];
#endif

	void Init() { memset(this, 0, sizeof(*this)); }
};

// 实时指引设置条目 (刚好4字节对齐,别轻易调整)
struct tag2QRealNavPar
{
	long		m_lCentLon;
	long		m_lCentLat;
	WORD		m_wRadius; // 米
	BYTE		m_bytSpeed; // 上限速度,海里/小时,0表示无限制
	BYTE		m_bytDir; // 度,实际度数/3
	BYTE		m_bytType; // 类型
		// 000TD000 (参照协议)
        //T：0--普通类型
        //   1--特殊类型（提示后需检测）（要进行实时指引报警的，现在只用于速度限制）
        //D：0--无需判断行车方向
        //   1--需判断行车方向
	char		m_szTip[51]; // 提示内容,不足填0,且必定以NULL结尾,因为协议里定义的是50字节,该字段正好比协议里多1个字节
};


// 实时指引设置
struct tag2QRealNavCfg
{
	tag2QRealNavPar		m_aryRealNavPar[50];

	void Init() { memset(this, 0, sizeof(*this)); }
};


// 拍照配置
struct tag2QPhotoCfg
{
	WORD		m_wPhotoEvtSymb; // 拍照触发状态字 (见QianExeDef.h中的规定)
	WORD		m_wPhotoUploadSymb; // 拍照主动上传状态字 (见QianExeDef.h中的规定)

	tag2QPhotoPar	m_objAlermPhotoPar; // 报警拍照设置
	tag2QPhotoPar	m_objJijiaPhotoPar; // 空车/重车变化时的拍照设置
	tag2QPhotoPar	m_objHelpPhotoPar; // 求助拍照设置
	tag2QPhotoPar	m_objDoorPhotoPar; // 开关车门拍照
	tag2QPhotoPar	m_objAreaPhotoPar; // 进出区域拍照
	tag2QPhotoPar	m_objTimerPhotoPar; // 定时拍照
	tag2QPhotoPar	m_objAccPhotoPar; // ACC变化拍照
	tag2QPhotoPar	m_objSpeedPhotoPar; // 超速/低速拍照

	void Init()
	{
		memset( this, 0, sizeof(*this));
		m_wPhotoEvtSymb = PHOTOEVT_FOOTALERM | PHOTOEVT_OPENDOOR | PHOTOEVT_AREA | PHOTOEVT_OVERSPEED | PHOTOEVT_BELOWSPEED;
		m_wPhotoUploadSymb = PHOTOUPLOAD_FOOTALERM | PHOTOUPLOAD_OPENDOOR | PHOTOUPLOAD_AREA | PHOTOUPLOAD_OVERSPEED | PHOTOUPLOAD_BELOWSPEED;
		m_objAlermPhotoPar.Init( 1, 2, 2, 0x0f ); // v_bytTime, v_bytSizeType, v_bytQualityType, v_bytChannel
		m_objJijiaPhotoPar.Init( 1, 2, 2, 0x0f );
		m_objHelpPhotoPar.Init( 1, 2, 2, 0x0f );
		m_objDoorPhotoPar.Init( 1, 2, 1, 0x0f );
		m_objAreaPhotoPar.Init( 1, 2, 2, 0x0f );
		m_objTimerPhotoPar.Init( 1, 2, 1, 0x0f );
		m_objAccPhotoPar.Init( 1, 2, 1, 0x0f );
		m_objSpeedPhotoPar.Init( 1, 2, 2, 0x0f );
	}
};

//LED配置 07.9.26 hong
struct tag2QLedCfg 
{
	unsigned short m_usLedParamID;  //LED配置ID
	unsigned short m_usTURNID;		//LED转场信息ID add by kzy 20090819
	unsigned short m_usNTICID;		//LED通知信息ID add by kzy 20090819
	BYTE           m_bytLedPwrWaitTm; //LED上电等待时间
	BYTE           m_bytLedConfig;   //LED功能定制 07.9.12 hong // 0 A1 A2 A3 A4 A5 A6 0
	//A6: 1: 车台移动静止时对LED屏进行亮黑控制 0：不进行控制
	//A5: 1:LED屏通信错误时不禁止下载广告     0：通信错误时禁止下载广告
	//A4：1：启用MD5                          0：不启用MD5
	unsigned short m_usLedBlkWaitTm; //车辆静止后黑屏等待时间，单位：秒,默认为2分钟
	//#ifdef DELAYLEDTIME
	//	unsigned short m_usLedMenuID;   //节目单ID 07.9.30
	//#endif
	char			m_szLedVer[64]; // LED程序版本号
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );
		
		m_usLedParamID = 0;
		m_bytLedPwrWaitTm = 5;
		m_bytLedConfig = 0;
		m_usLedBlkWaitTm = 60;
		m_usNTICID = 0;
		m_usTURNID = 0;
		
		//#ifdef DELAYLEDTIME
		//		m_usLedMenuID = 0;
		//#endif
	}
};


//手柄配置 caoyi 07.11.29
struct tag2QHstCfg{
    char		m_bytcallring;
    char		m_bytsmring;
    bool		m_HandsfreeChannel;
    bool		m_EnableKeyPrompt;
    bool		m_EnableAutoPickup;
	//char		m_bytListenTel[3];	//监听电话的代号  //cyh add:del
	char		m_szListenTel[20];	//cyh add:add
	void Init()
	{
		memset( this, 0, sizeof(*this) );
		
		m_HandsfreeChannel = false;
		m_EnableKeyPrompt = true;
		m_EnableAutoPickup = true;
	}
};

//油耗检测盒配置 wxl 09.05.05
struct tag2QOilCfg
{
	BYTE	m_SetType;
	ushort	m_usRptPeriod;
	BYTE	m_bytPowerCyc; //分时供电周期
	ushort	m_usADMin; //最小AD值设置
	ushort	m_usADMax;
	ushort	m_usACCONAddOilYUZHI;//ACCON加油阈值
	ushort	m_usACCONLeakOilYUZHI;
	ushort	m_usACCOFFAddOilYUZHI;
	ushort	m_usACCOFFLeakOilYUZHI;
	ushort	m_usTimeToCheckOil;
	BYTE	m_bytNumToCheckOil;//24小时加漏油检测次数

// 	void init()
// 	{
// 		memset(this, 0, sizeof(*this));
// 		m_usACCONLeakOilYUZHI = 250;
// 		m_usACCONLeakOilYUZHI = 250;
// 		m_usACCOFFAddOilYUZHI = 150;
// 		m_usACCOFFLeakOilYUZHI = 150;
// 		m_usTimeToCheckOil = 0x001E;
// 		m_bytNumToCheckOil = 5;
// 	}
};

#undef MAX_DRIVER
#define MAX_DRIVER 10

// 行驶配置, 090817 xun move here
struct DrvCfg 
{
	byte inited;		//是否初始化过的标志
	
	ushort factor;		//车辆特征系数(每公里的脉冲数)
	char vin[17];		//车辆VIN号：占用1～17个字节
	char num[12];		//车辆号码：占用1～12个字节
	char type[12];		//车牌分类：占用1～12个字节	
	
	byte total;			//司机个数
	byte max_tire;		//设置的最大疲劳时间(小时)
	byte min_sleep;		//设置的最小休息时间(分钟)
	
	struct Driver 
	{		
		bool valid;			//有效标志位
		byte num;			//司机编号：取值范围0~126
		char ascii[7];		//身份代码(1~7)
		char code[18];		//驾驶证号(1~18)
	} inf[MAX_DRIVER];
	
	byte max_speed;			//上限速度(海里/时)
	byte min_over;			//超速计量时间(秒)
	
	byte  min_speed;		//下限速度(公里/时)
	short min_below;		//低速计量时间(分钟)
	
	byte winsize;		//窗口大小
	byte sta_id;		//状态标识
	ushort uptime[6];	//上报总时长
	ushort cltcyc[6];	//采样周期
	ushort sndcyc[6];	//发送周期
	
	void init() 
	{
		memset(this, 0, sizeof(*this));
		inited = 123; // 初始化标志（比较特殊）
		winsize = 4;
		
		// 090229 xun: 增加默认参数
		uptime[0] = 0xffff; // 移动 (实际处理时是为ACC ON)
		cltcyc[0] = 60;
		sndcyc[0] = 1;
		uptime[1] = 0xffff; // 静止 (实际处理时是为ACC OFF)
		cltcyc[1] = 600;
		sndcyc[1] = 1;
		sta_id = 2; // 090817 xun add
		
		max_speed = 0;
		min_over = 0;
		min_speed = 255;
		min_below = 0;
	}
};

struct tagSpecCtrlCfg
{
	bool			m_bOilCut;

	void Init()
	{
		m_bOilCut = false;
	}
};

struct tag2QInvCarryCfg
{
	byte	m_bytCarryRpt;
	
	void Init()
	{
		m_bytCarryRpt = 0x01;//上报
	}
};

//zzg add
//凯天出租车配置信息
struct tagTaxiCfg
{
	byte m_aryDriverInf[3];//驾驶员身份信息
	byte m_aryBespokeInf[2];//预约电招信息
	byte m_bytJiJiaEnable;//计价器是否开启 0:开启 1:关闭
	void Init()
	{
		memset( this, 0, sizeof(*this) );
		m_aryBespokeInf[0]=0xff;
		m_aryBespokeInf[1]=0xff;
	}
};

struct tag2QRptStationCfg
{
	byte	m_bytRptSta;	// 是否启动公交自动报站
	int		m_iLineNo;		// 启用的线路号
	
	void Init()
	{
		m_iLineNo = 0;
		m_bytRptSta = 1;
	}
};

struct tag2QLineVersionCfg
{
	char strVer[6]; // 单位:秒
	
	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};


//--------------------以下为DvrExe参数--------------------//

//密码
struct tag2DPwdPar
{
	char	AdminPassWord[11];
	char	UserPassWord[11];
	char	m_szReserved[26]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

	  strcpy(AdminPassWord, "4312896654");
	  strcpy(UserPassWord, "4312897765");
	}
};

//设备信息
struct tag2DDevInfoPar
{
	char	DriverID[13]; 
	char	CarNumber[13];
	char	m_szReserved[22]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

	  strcpy(DriverID, "000000000000");
	  strcpy(CarNumber, "000000000000");
	}
};

//网络信息
struct tag2DIpPortPar
{
	char	m_szPhoneMod[20]; //手机模块
	char	m_szTelNum[16]; // 车台手机号,以NULL结尾
	char	m_szApn[20]; // 以NULL结尾
	unsigned long m_ulCommIP; //通信IP
	unsigned long m_ulPhotoIP; //照片IP
	unsigned long m_ulVideoIP; //视频IP
	unsigned long m_ulUpdateIP; //升级IP
	unsigned short m_usCommPort; //通信端口
	unsigned short m_usPhotoPort; //照片端口
	unsigned short m_usVideoPort; //视频端口
	unsigned short m_usUpdatePort; //升级端口

	char	m_szReserved[16]; // 留出预留空间,并使得整个结构体尺寸为8的倍数

	void Init()
	{
		memset( this, 0, sizeof(*this) );

		strcpy(m_szPhoneMod, "LC6311");  	strcpy(m_szTelNum, "");
		strncpy(m_szApn, APN, sizeof(m_szApn)-1);
		m_ulCommIP = inet_addr(QTCPIP);
	  m_ulPhotoIP = inet_addr(QUDPIP);
	  m_ulVideoIP = inet_addr(DUDPIP);
	  m_ulUpdateIP = inet_addr(LUDPIP);
	  m_usCommPort = htons(QTCPPORT);
	  m_usPhotoPort = htons(QUDPPORT);
	  m_usVideoPort = htons(DUDPPORT);
	  m_usUpdatePort = htons(LUDPPORT);
	}
};

//音视频触发类型
struct tag2DTrigPar 
{
	UINT TrigType;
	char m_szReserved[20]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

		TrigType = 1;//TRIG_EVE;
	}
};

//时段触发参数
struct tag2DPerTrigPar
{
	BOOL	PeriodStart[6];
	char	StartTime[6][9];
	char	EndTime[6][9];
	char	m_szReserved[60]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

		int i;
		for(i = 0; i < 6; i++)
		{
			PeriodStart[i] = FALSE;
			strcpy(StartTime[i], "00:00:00");
			strcpy(EndTime[i], "23:59:59");
		}
	}
};

//事件触发参数
struct tag2DEveTrigPar
{
	BOOL	AccStart, MoveStart, OverlayStart, S1Start, S2Start, S3Start, S4Start;
	UINT	AccDelay, MoveDelay, OverlayDelay, S1Delay, S2Delay, S3Delay, S4Delay;
	char	m_szReserved[40]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

		AccStart = TRUE;
		MoveStart = OverlayStart = S1Start = S2Start = S3Start = S4Start = FALSE;
		AccDelay = MoveDelay = OverlayDelay = S1Delay = S2Delay = S3Delay = S4Delay = 20;
	}
};

//视频输入参数
struct tag2DVIPar
{
	UINT	VMode;
	UINT	VNormal;	
	char	m_szReserved[16]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

#if VEHICLE_TYPE == VEHICLE_M
		VMode = 1;//CIF;
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		VMode = 4;//D1;
#endif
		VNormal = 0;//PAL;
	}
};

//视频输出参数
struct tag2DVOPar
{
	UINT  VMode;
	UINT  VNormal;
	char	m_szReserved[16]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

		VMode = 1;//CIF;
		//VMode = 4;//D1;
		VNormal = 0;//PAL;
	}
};

//视频预览参数
struct tag2DVPrevPar
{
	BOOL  VStart[4];
	char	m_szReserved[16]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

		int i;
		for(i = 0; i < 4; i++)
		{
			VStart[i] = TRUE;
		}
	}
};

//音视频编码录像参数
struct tag2DAVRecPar
{
#if VEHICLE_TYPE == VEHICLE_M
	BOOL  AStart[4];
	BOOL  VStart[4];
	UINT 	VMode;
	UINT 	VBitrate[4];
	UINT 	VFramerate[4];
	char  DiskType[10];
	char	m_szReserved[34]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

		strcpy(DiskType, "HDisk");
		VMode = 1;//CIF;
		
		int i;
		for(i = 0; i < 4; i++)
		{
			AStart[i] = TRUE;
			VStart[i] = TRUE;
			VBitrate[i] = 409600;//BITRATE_REC_3;
			VFramerate[i] = 20;
		}
 		AStart[2] = FALSE;
 		VStart[2] = FALSE;
 		AStart[3] = FALSE;
 		VStart[3] = FALSE;
	}
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	BOOL  AStart[4];
	BOOL  VStart[4];
	UINT 	VMode[4];
	UINT 	VBitrate[4];
	UINT 	VFramerate[4];
	char  DiskType[10];
	char	m_szReserved[22]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );
#if VEHICLE_TYPE == VEHICLE_V8
		strcpy(DiskType, "SDisk");
#endif
#if VEHICLE_TYPE == VEHICLE_M2
		strcpy(DiskType, "HDisk");
#endif
		
		int i;
		for(i = 0; i < 4; i++)
		{
			VMode[i] = 1;//CIF;
			AStart[i] = TRUE;
			VStart[i] = TRUE;
			VBitrate[i] = 173000;
			VFramerate[i] = 25;
		}
	}
#endif
};

//音视频编码上传参数
struct tag2DAVUpdPar
{
	BOOL  AStart[4];
	BOOL  VStart[4];
	UINT 	VMode;
	UINT 	VBitrate[4];
	UINT 	VFramerate[4];
	int 	UploadType;
	int 	UploadSecond;
	char	m_szReserved[36]; // 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

		VMode = 1;//CIF;
		UploadType = 0;
		UploadSecond = 300;
		
		int i;
		for(i = 0; i < 4; i++)
		{
			AStart[i] = FALSE;
			VStart[i] = FALSE;
			VBitrate[i] = 16000;//BITRATE_UPL_0;
			VFramerate[i] = 10;
		}
	}
};

//音视频解码回放参数
struct tag2DAVPbPar
{
	BOOL  AStart[4];
	BOOL  VStart[4];
	UINT	DecNum;
	UINT  VFramerate[4];
	char	CreatDate[11];
	char	EncChn[4][2];
	char	AFilePath[4][50];
	char	VFilePath[4][50];
	char	IFilePath[4][50];
	byte	m_bytSysVolume;		// 系统音量
	char	m_szReserved[72];	// 留出预留空间,并使得整个结构体尺寸为8的倍数
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

		int i;
		for(i = 0; i < 4; i++)
		{
			AStart[i] = FALSE;
			VStart[i] = FALSE;
			VFramerate[i] = 0;
			memset(EncChn[i], 0, sizeof(EncChn[i]));
			memset(AFilePath[i], 0, sizeof(AFilePath[i]));
			memset(VFilePath[i], 0, sizeof(VFilePath[i]));
			memset(IFilePath[i], 0, sizeof(IFilePath[i]));
		}
		DecNum = 1;
		memset(CreatDate, 0, sizeof(CreatDate));

		m_bytSysVolume = 15;
	}
};

//DVR配置参数
struct tag2DDvrCfg
{
	tag2DPwdPar	    PasswordPara;
	tag2DDevInfoPar	DeviceInfo;
	tag2DIpPortPar	IpPortPara;
	tag2DTrigPar		TrigPara;
	tag2DPerTrigPar	PeriodTrig;
	tag2DEveTrigPar EventTrig;
	tag2DVIPar			VInput;
	tag2DVOPar 			VOutput;
	tag2DVPrevPar		VPreview;
	tag2DAVRecPar		AVRecord;
	tag2DAVUpdPar		AVUpload;
	tag2DAVPbPar		AVPlayback;
	
	void Init()
	{
		memset( this, 0, sizeof(*this) );

		PasswordPara.Init();
		DeviceInfo.Init();
		IpPortPara.Init();
		TrigPara.Init();
		PeriodTrig.Init();
		EventTrig.Init();
		VInput.Init();
	 	VOutput.Init();
		VPreview.Init();
		AVRecord.Init();
		AVUpload.Init();
		AVPlayback.Init();
	}
};

//--------------------以上为DvrExe参数--------------------//


// 根据20070914讨论,认为配置区应按照业务进行分块,并且每块大小应是80的整数倍
struct tagSecondCfg// 注意结构体内存对齐
{
	char		m_szSpecCode[80];

	// 流媒体设置
	union
	{
		char		m_szReserved[80];
		tag2LMediaCfg m_obj2LMediaCfg;
	} m_uni2LMediaCfg;

	// 黑匣子保存设置
	union
	{
		char		m_szReservered[80];
		tag2QBlkCfg	m_obj2QBlkCfg;
	} m_uni2QBlkCfg;

#if USE_LIAONING_SANQI == 1
	// 辽宁移动三期报告设置
	union
	{
		char		m_szReservered[480];
		tag2QReportCfg	m_obj2QReportCfg;
	}m_uni2QReportCfg;
	
	// 辽宁移动三期主动上报设置
	union
	{
		char		m_szReservered[1200];
		tag2QAutoRptCfg	m_obj2QAutoRptCfg;
	}m_uni2QAutoRptCfg;

	// 辽宁移动三期区域设置
	union
	{
		char		m_szReservered[960];
		tag2QAreaRptCfg	m_obj2QAreaRptCfg;
	}m_uni2QAreaRptCfg;
	// 辽宁移动三期时间段速度异常设置  合并到区域速度异常中
//	union
//	{
//		char 		m_szReservered[80];
//		tag2QTimeSpdRptCfg m_obj2QTimeSpdRptCfg;
//	}m_uni2QTimeSpdRptCfg;

	// 辽宁移动三期区域速度异常设置
	union
	{
		char 		m_szReservered[960];
		tag2QAreaSpdRptCfg m_obj2QAreaSpdRptCfg;
	}m_uni2QAreaSpdRptCfg;

	// 辽宁移动三期区域停车超时
	union
	{
		char 		m_szReservered[480];
		tag2QAreaParkTimeRptCfg m_obj2QAreaParkTimeRptCfg;
	}m_uni2QAreaParkTimeRptCfg;
	 // 辽宁移动三期偏航报警
	union
	{
		char 		m_szReservered[10720];
		tag2QLineRptCfg m_obj2QLineRptCfg;
	}m_uni2QLineRptCfg;
	//辽宁移动三期拍照设置
	union
	{
		char 		m_szReservered[480];
		tag2QLNPhotoCfg m_obj2QLNPhotoCfg;
	}m_uni2QLNPhotoCfg;

	//辽宁移动三期厂商ID 终端版本 集团计费号
	union
	{
		char 		m_szReservered[80];
		tag2QcgvCfg m_obj2QcgvCfg;
	}m_uni2QcgvCfg;
#endif

	// 报警设置
	union
	{
		char		m_szReservered[80];
		tag2QAlertCfg	m_obj2QAlertCfg;
	} m_uni2QAlertCfg;

	// 实时定位条件设置
	union
	{
		char		m_szReservered[560];
		tag2QRealPosCfg	m_obj2QRealPosCfg;
	} m_uni2QRealPosCfg;

	// GPRS参数设置
	union
	{
		char		m_szReservered[160];
		tag2QGprsCfg	m_obj2QGprsCfg;
	} m_uni2QGprsCfg;

	// 中心服务号码设置
	union 
	{
		char		m_szReservered[560];
		tag2QServCodeCfg	m_obj2QServCodeCfg;
	} m_uni2QServCodeCfg;

	// 拍照设置
	union
	{
		char		m_szReservered[480];
		tag2QPhotoCfg	m_obj2QPhotoCfg;
	} m_uni2QPhotoCfg;
	
	// LED配置
	union
	{
		char		m_szReservered[160];
		tag2QLedCfg m_obj2QLedCfg;
	}m_uni2QLedCfg;
	
	//手柄配置
	union
	{
		char		m_szReservered[80];
		tag2QHstCfg	m_obj2QHstCfg;
	}m_uni2QHstCfg;

	// 呼入呼出限制设置
	union
	{
		char		m_szReservered[880];
		tag2QFbidTelCfg m_obj2QFbidTelCfg;
	} m_uni2QFbidTelCfg;
	
	// 电话本设置
	union
	{
		char		m_szReservered[960];
		tag2QCallCfg	m_obj2QCallCfg;
	} m_uni2QCallCfg;

#if USE_LIAONING_SANQI == 1
	union
	{
		char		m_szReservered[480];
		tag2QCallListCfg m_obj2QCallListCfg;
	}m_uni2QCallListCfg;
#endif

	// 行驶记录上报设置
	union
	{
		char		m_szReservered[80];
		tag2QDriveRecCfg	m_obj2QDriveRecCfg;
	} m_uni2QDriveRecCfg;

	// 驾驶员身份设置
	union
	{
		char		m_szReservered[480];
		tag2QDriverCfg		m_obj2QDriverCfg;
	} m_uni2QDriverCfg;

	// 实时指引设置
	union
	{
		char		m_szReservered[3200];
		tag2QRealNavCfg		m_obj2QRealNavCfg;
	} m_uni2QRealNavCfg;
	

	/// { JG2旧的形式记录仪模块配置
	// 电话本 (JG2)
	union
	{
		char		m_szReservered[1600];	//已经使用1245
		
	} m_uniBookCfg;

// 	union
// 	{
// 		char		m_szReservered[160];
// 		tag2QOilCfg	m_obj2QOilCfg;
// 	}m_uni2QOilCfg;
	
	// 行车记录仪 (JG2)
	union
	{
		char		m_szReservered[480];	//已经使用318	
	} m_uniDrvCfg;
	/// } JG2旧的形式记录仪模块配置

	union
	{
		char		m_szReservered[80];
		tagSpecCtrlCfg m_obj2QSpecCtrlCfg;
	} m_uni2QSpecCtrlCfg;

	//载客不打表
	union
	{
		char		m_szReservered[80];
		tag2QInvCarryCfg m_obj2QInvCarryCfg;
	} m_uni2QInvCarryCfg;

	// Dvr设置
	union
	{
		tag2DDvrCfg m_obj2DDvrCfg;
		char		m_szReservered[1760]; //已经使用1530左右
	}m_uni2DDvrCfg;
	
	// 出租车驾驶员身份信息
	union
	{
		char		m_szReservered[80];
		tagTaxiCfg m_obj2QTaxiCfg;
	} m_uni2QTaxiCfg;

	// 越界报警参数设置信息
	union
	{
		char		m_szReservered[80];
		tag2QAreaCfg m_obj2QAreaCfg;
	} m_uni2QAreaCfg;

	// 自动报站
	union
	{
		char		m_szReservered[80];
		tag2QRptStationCfg m_obj2QRptStationCfg;
	} m_uni2QRptStationCfg;

	union
	{
		char		m_szReserved[80];
		tag2QLineVersionCfg m_obj2QLineVersionCfg;
	} m_uni2QLineVersionCfg;

	void InitDefault()
	{
		memset(this, 0, sizeof(*this));
		strncpy( m_szSpecCode, CFGSPEC, sizeof(m_szSpecCode) - 1 );

		m_uni2LMediaCfg.m_obj2LMediaCfg.Init();
		m_uni2QBlkCfg.m_obj2QBlkCfg.Init();		
		m_uni2QAlertCfg.m_obj2QAlertCfg.Init();
		m_uni2QRealPosCfg.m_obj2QRealPosCfg.Init();
		m_uni2QGprsCfg.m_obj2QGprsCfg.Init();
		m_uni2QServCodeCfg.m_obj2QServCodeCfg.Init();
		m_uni2QPhotoCfg.m_obj2QPhotoCfg.Init();
		m_uni2QLedCfg.m_obj2QLedCfg.Init();
		m_uni2QHstCfg.m_obj2QHstCfg.Init();
		m_uni2QFbidTelCfg.m_obj2QFbidTelCfg.Init();
		m_uni2QCallCfg.m_obj2QCallCfg.Init();	
		m_uni2QDriveRecCfg.m_obj2QDriveRecCfg.Init();
		m_uni2QDriverCfg.m_obj2QDriverCfg.Init();
		m_uni2QRealNavCfg.m_obj2QRealNavCfg.Init();
		m_uni2QSpecCtrlCfg.m_obj2QSpecCtrlCfg.Init();
		m_uni2QInvCarryCfg.m_obj2QInvCarryCfg.Init();
		m_uni2QAreaCfg.m_obj2QAreaCfg.Init();
		m_uni2QRptStationCfg.m_obj2QRptStationCfg.Init();
		m_uni2QLineVersionCfg.m_obj2QLineVersionCfg.Init();

#if USE_LIAONING_SANQI == 1
		m_uni2QReportCfg.m_obj2QReportCfg.Init();
		m_uni2QAutoRptCfg.m_obj2QAutoRptCfg.Init();
		m_uni2QAreaRptCfg.m_obj2QAreaRptCfg.Init();
		m_uni2QCallListCfg.m_obj2QCallListCfg.Init();
//		m_uni2QTimeSpdRptCfg.m_obj2QTimeSpdRptCfg.Init();
		m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg.Init();
		m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg.Init();
		m_uni2QLineRptCfg.m_obj2QLineRptCfg.Init();
		m_uni2QcgvCfg.m_obj2QcgvCfg.Init();
		m_uni2QLNPhotoCfg.m_obj2QLNPhotoCfg.Init();
#endif
 		memset(m_uniBookCfg.m_szReservered, 0, sizeof(m_uniBookCfg.m_szReservered));
 		memset(m_uniDrvCfg.m_szReservered, 0, sizeof(m_uniDrvCfg.m_szReservered));

		// 090817 xun add
		DrvCfg objDrvCfg;
		objDrvCfg.init();
		memcpy( m_uniDrvCfg.m_szReservered, &objDrvCfg, sizeof(objDrvCfg) );

		m_uni2DDvrCfg.m_obj2DDvrCfg.Init();
		
		m_uni2QTaxiCfg.m_obj2QTaxiCfg.Init();//zzg add
	}
};

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

int DataPush( void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb, DWORD v_dwPopSymb, BYTE v_bytLvl ); // 此方法可同时指定多个PopSymb,即接收对象
int DataPop( void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPushSymb, DWORD v_dwPopSymb, BYTE *v_p_bytLvl );
int DataDel( DWORD v_dwPopSymb, void* v_pCmpDataFunc );
int DataWaitPop( DWORD v_dwPopSymb );
int DataSkipWait( DWORD v_dwPopSymb );

int GetImpCfg( void *v_pDes, unsigned int v_uiDesSize, unsigned int v_uiGetBegin, unsigned int v_uiGetSize );
int SetImpCfg( void *v_pSrc, unsigned int v_uiSaveBegin, unsigned int v_uiSaveSize );
int GetSecCfg( void *v_pDes, unsigned int v_uiDesSize, unsigned int v_uiGetBegin, unsigned int v_uiGetSize );
int SetSecCfg( void *v_pSrc, unsigned int v_uiSaveBegin, unsigned int v_uiSaveSize );
int ResumeCfg( bool v_bReservTel, bool v_bReservIP );

int IOGet( unsigned char v_ucIOSymb, unsigned char *v_p_ucIOSta );
int IOSet( unsigned char v_ucIOSymb, unsigned char v_ucIOSta,  void* v_pReserv, unsigned int v_uiReservSiz );
int IOCfgGet( unsigned char v_ucIOSymb, void *v_pCfg, DWORD v_dwCfgSize );
int IOIntInit( unsigned char v_ucIOSymb, void* v_pIntFunc ); // 若v_ucIOSymb值为255,则表示设置全部输入IO的中断
//int IOIntRelease();
//int IOIntFuncSet( unsigned char v_ucIOSymb, void* v_pIntFunc ); // 若v_ucIOSymb值为255,则表示设置全部IO的回调函数

int GetCurGps( void* v_pGps, const unsigned int v_uiSize, BYTE v_bytGetType );
int SetCurGps( const void* v_pGps, BYTE v_bytSetType );

bool GetDevID( char* v_szDes, unsigned int v_uiDesSize );
bool GetAppVer( char* v_szDes, unsigned int v_uiDesSize );

int CrtVerFile();
int DelVerFile();
int ReadVerFile(char *v_pszVersion);
void ChkSoftVer(char *v_pszVersion);

int SetCurTime( void* v_pTmSet ); // 传入参数应是tm类型对象的指针 (UTC时间)
unsigned int GetTickCount(); // 返回值: 从1970.1.1凌晨起经历的时间,ms

void DogClr( DWORD v_dwSymb ); // 各进程设置对应的看门狗清除标志
DWORD DogQuery(); // 查询看门狗值(查询后复位所有进程看门狗清除标志)
void DogInit(); // 初始化看门狗值(所有bit置1以便第一次查询)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
