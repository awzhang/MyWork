#ifndef _YX_DRIVERECORD_H_
#define _YX_DRIVERECORD_H_

#if USE_DRVREC == 1

#define CAR_RUNNING			0
#define CAR_STOP_RUN		1
#define CAR_RUN_STOP		2
#define CAR_STOP			3
#define UnLog				0 
#define Logined				1
#define Inputing			2
#define MAX_DRIVER			10		//最大司机数
#define MAX_PROMPT			3		//最大司机提醒次数
#define CONTINUE_TIME		3		//行驶记录的最短行驶时间(小时),低于此值不保存
#define VT_LOW_RUN			2		//判断车辆停止的最小速度(海里/时)
#define RUN_MINTIME			5		//判断车辆移动的最小时间(秒)
#define STOP_MINTIME		20		//判断车辆停止的最小时间(秒)
#define SND_TYPE			1		// 1,每帧发送,满一个窗口等待应答   2,积满一个窗口才发送;

// 行驶记录结构
STRUCT_1 RcdDrv {
	
	// 司机编号,取值范围0~126
	byte numdrv;
	
	// 起点时间和经纬度
	byte s_time[5];
	byte s_lon[4];
	byte s_lat[4];
	
	// 终点时间和经纬度
	byte e_time[5];
	byte e_lon[4];
	byte e_lat[4];
	
	// 行驶里程
	ushort space;
	
	void set_start_time()
	{
		struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);
		
		s_time[0]	= (byte)(pCurrentTime.tm_year-100);
		s_time[1]	= (byte)(pCurrentTime.tm_mon+1);
		s_time[2]	= (byte)pCurrentTime.tm_mday;
		s_time[3]	= (byte)pCurrentTime.tm_hour;
		s_time[4]	= (byte)pCurrentTime.tm_min;
	}	
	
	void set_end_time()
	{
		struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);
		
		e_time[0]	= (byte)(pCurrentTime.tm_year-100);
		e_time[1]	= (byte)(pCurrentTime.tm_mon+1);
		e_time[2]	= (byte)pCurrentTime.tm_mday;
		e_time[3]	= (byte)pCurrentTime.tm_hour;
		e_time[4]	= (byte)pCurrentTime.tm_min;
	}	
	
	void set_start_gps()
	{
		tagGPSData gps(0);
		GetCurGps(&gps, sizeof(gps), GPS_LSTVALID);
		
		double db = 60*(gps.latitude-byte(gps.latitude));
		s_lat[0] = byte(gps.latitude);
		s_lat[1] = (byte)db;
		s_lat[2] = byte(long(db*100)%100);
		s_lat[3] = byte(long(db*10000)%100);
		
		db = 60*(gps.longitude-byte(gps.longitude));
		s_lon[0] = byte(gps.longitude);
		s_lon[1] = (byte)db;
		s_lon[2] = byte(long(db*100)%100);
		s_lon[3] = byte(long(db*10000)%100);
	}
	
	void set_end_gps()
	{
		tagGPSData gps(0);
		GetCurGps(&gps, sizeof(gps), GPS_LSTVALID);
		
		double db = 60*(gps.latitude-byte(gps.latitude));
		e_lat[0] = byte(gps.latitude);
		e_lat[1] = (byte)db;
		e_lat[2] = byte(long(db*100)%100);
		e_lat[3] = byte(long(db*10000)%100);
		
		db = 60*(gps.longitude-byte(gps.longitude));
		e_lon[0] = byte(gps.longitude);
		e_lon[1] = (byte)db;
		e_lon[2] = byte(long(db*100)%100);
		e_lon[3] = byte(long(db*10000)%100);
	}
	
};

// 事故疑点记录结构
STRUCT_1 RcdDoub {
	byte stoptype;		//停止类型
	byte numdrv;		//司机编号
	byte times[6];		//记录时间
	struct Gps {
		byte lon[4];
		byte lat[4];
	} pos[20];			//前20个位置信息
	byte vector[100];	//前100个速度
	byte sensor[100];	//前100个开关量信号
	
	void set_data(bool setgps, byte sens)
	{	
		tagGPSData gps(0);
		GetCurGps(&gps, sizeof(gps), GPS_LSTVALID);
		
		//如果上次速度已经为0,同时当前速度也为0,则不再采集
		if(0==vector[0] && 0==gps.speed)	return;
		
		int i = 0;
		if(setgps) {
			for(i=19; i>=1; i--) pos[i] = pos[i-1];
			
			double db = 60*(gps.latitude-byte(gps.latitude));
			pos[0].lat[0] = byte(gps.latitude);
			pos[0].lat[1] = (byte)db;
			pos[0].lat[2] = byte(long(db*100)%100);
			pos[0].lat[3] = byte(long(db*10000)%100);
			db = 60*(gps.longitude-byte(gps.longitude));
			pos[0].lon[0] = byte(gps.longitude);
			pos[0].lon[1] = (byte)db;
			pos[0].lon[2] = byte(long(db*100)%100);
			pos[0].lon[3] = byte(long(db*10000)%100);
		}
		for(i=99; i>=1; i--)	vector[i] = vector[i-1];
		vector[0] = byte( ((gps.speed*3600)/1000)/1.852 );
		for(i=99; i>=1; i--)	sensor[i] = sensor[i-1];
		sensor[0] = sens;	
	}
	
	void set_time()
	{
		struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);
		
		times[0] = (byte)(pCurrentTime.tm_year-100);
		times[1] = (byte)(pCurrentTime.tm_mon+1);
		times[2] = (byte)pCurrentTime.tm_mday;
		times[3] = (byte)pCurrentTime.tm_hour ? (byte)pCurrentTime.tm_hour : 0x7f;
		times[4] = (byte)pCurrentTime.tm_min  ? (byte)pCurrentTime.tm_min  : 0x7f;
		times[5] = (byte)pCurrentTime.tm_sec  ? (byte)pCurrentTime.tm_sec  : 0x7f;
	}
};


// 360小时的行驶里程相关
STRUCT_1 Meter360 {
	double _space;			//当前2个小时内累计里程
	double space[180];		//360内的里程数据
	double get_meter() {
		double ret = _space;
		for(int i=0; i<180; i++)	ret += space[i];
		return ret;
	}
};

// 2个日历天内的行驶里程相关
STRUCT_1 Meter2Day {
	double _space;			//10分钟内的累计里程
	byte date[3][3];		//日期
	double space[3];		//3天的行驶里程数
	double get_meter() {
		return (space[2]+space[1]);
	}
};

// 行驶记录的头文件结构
STRUCT_1 HRcdFile {
	
	ulong ticks;			//定时计数(秒)
	
	int numdrv;				//当前司机编号
	int stacar;				//当前车辆的状态(只有两种状态,移动和静止)
	
	int stacur;				//当前车辆状态(四种状态)
	int stapre;				//上次车辆状态(四种状态)
	int run;				//车辆连续行驶时间(分)
	int stop;				//车辆连续停止时间(分)
	
	int inputflag;			//司机登陆结果(调度屏返回)
	
	RcdDrv rcddrv;			//当前的行驶记录
	RcdDoub rcddoub;		//当前的事故疑点记录
	
	byte speed;				//当前速度(公里/时)
	byte vector[60];		//最近60秒的速度(海里/时)
	byte average[15];		//最近15分钟的平均速度
	
	double lstlon,lstlat;	//最后一个有效的经纬度
	double metercur;		//当次行驶里程数(m)
	double metertotal;		//总里程(m)
	byte metertime[3];		//开始总里程统计时间(年月日)
	
	Meter360 m360;			//360小时里程记录
	Meter2Day m2d;			//2个日历天里程记录
	
	ulong drvcnt;			//行驶记录个数
	ulong drvpw;			//行驶记录写指针
	ulong doubcnt;			//事故疑点记录个数
	ulong doubpw;			//事故疑点记录写指针
	
	byte almtime[20][4];	//行驶时间段
	ushort r_used;			//报警区域当前用了多少块区域
	ushort d_used;			//报警区域当前用了多少个点
	
	byte rp_oil[5];			//标志收到 请求上传行驶数据的应答
	ulong uptime;			//已主动上报的时长
	int st_clt;				//带油耗主动上报采集状态
	
	byte updoub;			//是否需要上报疑点数据的标志, 
	//1:上报最近一个正式停车数据 2:上报所有未上报的停车数据
	int tickdoub;			//事故疑点停车时间计数
	byte rp_doub[2];		//标志收到 实时事故疑点上传应答
	
	
	int st_log;				//司机登陆状态	
	int prompt;				//非法启动已提示次数
	bool alarmed;			//非法启动是否报警过的标志
	
	void init()
	{
		memset(this, 0, sizeof(*this));
		
		numdrv = 0;
		stacar = CAR_STOP;
		
		stacur = CAR_STOP;
		stapre = CAR_STOP;
		run = 0;
		stop = 0;
		
		inputflag = -2;
		
		memset(vector, 0, 60);
		memset(average, 0, 15);	
		
		struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);

		metertime[0] = (byte)(pCurrentTime.tm_year-100);
		metertime[1] = (byte)(pCurrentTime.tm_mon+1);
		metertime[2] = (byte)pCurrentTime.tm_mday;
		
		prompt = 0;
		alarmed = false;
		st_log = UnLog;
	}
	
};

// 报警区域
STRUCT_1 Range {
	ushort areano;			//区域编号
	byte attr;				//区域属性
	byte vector;			//上限速度
	ushort cnt;				//区域的顶点个数
	ushort n_start;			//起始点序号
	ushort n_end;			//终止点序号
};

// 行驶记录的文件结构
STRUCT_1 RcdFile {
	HRcdFile head;
	byte reserved[20*1024-sizeof(HRcdFile)];		//head的保留空间
	
	RcdDrv drv[200];		//行驶记录
	RcdDoub doub[100];		//事故疑点记录
	
	Range range[200];		//报警区域
	byte dot[800][8];		//区域点个数
};

//-------------与中心协议帧------------------
// 疲劳行驶指示
STRUCT_1 frm3647 {
	byte numdrv;		//司机编号
	byte s_time[5];		//起始时间
	byte s_lon[4];		//起点经度
	byte s_lat[4];		//起点纬度
	byte timed;			//已行驶时间(小时)
};

// 查询连续行驶时间记录请求
STRUCT_1 frm3606 {
	byte numdrv;		//司机编号
	byte times;			//连续行驶时间
};

// 查询连续行驶时间记录应答
STRUCT_1 frm3646 {
	byte reply;			//应答类型
	byte index;			//当前帧序号
	byte count;			//记录个数
	RcdDrv rcd;			//记录
};

// 查询最近15分钟内每分钟平均速度应答
STRUCT_1 frm3645 {
	byte reply;			//应答类型
	byte average[15];	//每分钟平均速度(海里/小时)
};

// 查询事故疑点数据应答1(速度和开关量信号)
STRUCT_1 frm36441 {
	byte reply;			//应答类型
	byte index;			//当前帧序号
	byte attr;			//事故疑点数据属性
	byte numdrv;		//司机编码
	byte times[6];		//停车时刻时间
	byte vecsen[50][2];	//速度和开关量信号
};

// 查询事故疑点数据应答2(位置)
STRUCT_1 frm36442 {
	byte reply;			//应答类型
	byte index;			//当前帧序号
	byte attr;			//事故疑点数据属性
	byte numdrv;		//司机编码
	byte times[6];		//停车时刻时间
	byte gps[10][8];	//位置
};

// 总里程查询请求应答
STRUCT_1 frm0941 {
	byte reply;			//应答类型
	byte time[3];		//起始时间
	byte len;			//里程信息长度
	char inf[126];		//里程信息
};

// 里程记录查询
STRUCT_1 frm0905 {
	byte type;			//时间范围类型
	byte s_time[5];		//起始时间
	byte e_time[5];		//终止时间
	byte numdrv;		//司机编码
};

// 里程记录查询应答
STRUCT_1 frm0945 {
	byte reply;			//应答类型
	byte index;			//当前帧序号
	byte count;			//记录个数
	byte numdrv;		//司机编码
	byte bit_g4:1;		//HBIT
	byte bit_g3:1;
	byte bit_g2:1;
	byte bit_g1:1;		
	byte bit_g0:4;		
	byte s_time[5];		//起点时间
	byte s_lon[4];		//起点经度
	byte s_lat[4];		//起点纬度
	byte e_time[5];		//终点时间
	byte e_lon[4];		//终点经度
	byte e_lat[4];		//终点纬度
	byte space[2];		//行驶里程
};

// 设置行驶时间段请求
STRUCT_1 frm3701 {
	byte ctl;			//设置字
	byte cnt;			//设置个数
	byte time[20][5];	//时间段
};

// 设置报警区域请求1
STRUCT_1 frm37021 {
	ushort frmno;		//帧序号
	byte ctl;			//设置字
	ushort areano;		//区域编号
	byte attr;			//区域属性
	byte vector;		//上限速度
	ushort cnt;			//区域点个数
	byte dot[200][8];	//区域点
};

// 设置报警区域请求2
STRUCT_1 frm37022 {
	ushort frmno;		//帧序号
	byte ctl;			//设置字
	byte dot[200][8];	//区域点
};

// 设置报警区域应答
STRUCT_1 frm3742 {
	byte reply;			//应答类型
	ushort frmno;		//出错帧序号
};

// 设置定时上传行驶数据
STRUCT_1 frm103e {
	byte up;			//传输数据标识
	byte sta_id;		//状态标识
	ushort uptime;		//上报总时长
	ushort collect;		//采样周期
	ushort send;		//发送周期
};

STRUCT_1 Upload3747Inf {
	ushort numdrv;		//司机编号
	byte lat[4];		//纬度
	byte lon[4];		//经度
	byte speed;			//速度
	byte dir;			//方向
	byte sensor;		//传感器状态
	byte state[2];		//状态字
	byte sensor2;		//报警传感器状态
	byte ct_load;		//载重传感器个数L
	ushort ad_load[4];	//载重AD值
};

// 请求上传行驶数据(带油耗)
STRUCT_1 frm3747 {
	byte reply;			//应答请求
	byte w_size;		//发送窗口大小
	byte frmno;			//帧序号
	
	ulong meter;		//当前总里程
	byte ad_oil;		//油耗ad
	byte vo_oil;		//油耗电压
	byte st_oil;		//油耗状态字
	ushort tm_spc;		//间隔时间
	byte ct_gps;		//GPS数据个数
	byte date[3];		//日期
	byte time[3];		//时间
	Upload3747Inf inf[4];
	byte ct_avrg;			//1分钟平均速度点个数
	byte avrg[256];			//1分钟内平均速度
	
	int mkfrm() {
		byte tmp = ct_avrg;
		memcpy(&inf[ct_gps], &ct_avrg, tmp+1);
		return ((byte*)&inf[ct_gps] - &reply + tmp + 1);
	}
};

STRUCT_1 Upload3749Inf {
	ushort numdrv;		//司机编号
	byte lat[4];		//纬度
	byte lon[4];		//经度
	byte speed;			//速度
	byte dir;			//方向
	byte sensor;		//传感器状态
	byte state[2];		//状态字
	byte sensor2;		//报警传感器状态
	byte ct_load;		//载重传感器个数L
	ushort ad_load[4];	//载重AD值
	byte m_bytOilSta;	 // 油耗状态字
	ushort m_wValidOilAD; //油耗AD
	ushort m_wValidPowtAD; //电压AD
};

STRUCT_1 frm3749 {
	byte reply;			//应答请求
	byte w_size;		//发送窗口大小
	byte frmno;			//帧序号
	
	ulong meter;		//当前总里程
	ushort tm_spc;		//间隔时间
	byte ct_gps;		//GPS数据个数
	byte date[3];		//日期
	byte time[3];		//时间
	byte m_bytKuoZhanSta1;
	byte m_bytKuoZhanSta2;
	Upload3749Inf inf[4];
	byte ct_avrg;			//1分钟平均速度点个数
	byte avrg[256];			//1分钟内平均速度
	
	int mkfrm() {
		byte tmp = ct_avrg;
		memcpy(&inf[ct_gps], &ct_avrg, tmp+1);
		return ((byte*)&inf[ct_gps] - &reply + tmp + 1);
	}
};

// 实时事故疑点上传
STRUCT_1 frm0157 {
	byte trantype;		//传输类型
	byte e_time[6];		//停车时间
	struct Doub {
		byte lat[4];		//纬度
		byte lon[4];		//经度
		byte speed;			//速度
		byte sensor;		//开关量
	} doub[10];			//事故疑点信息
	byte doubtype;		//事故疑点类型
};


//---------------与调度屏协议帧----------------
// 速度请求应答
STRUCT_1 frm60 {
	byte cmd;
	byte reply;			//应答类型
	byte speed;			//速度(海里/时)
};

// 请求司机登录应答
STRUCT_1 frm63 {
	byte cmd;
	byte reply;			//应答类型
	ushort numdrv;		//司机编号
};

// 请求司机编号应答
STRUCT_1 frm64 {
	byte cmd;
	byte reply;			//应答类型
	ushort numdrv;		//编号
	byte asclen;		//身份代码长度
	char ascii[7];		//身份代码
};

STRUCT_1 tag0f12Area
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

//////////////////////////////////////////////////////////////////////////

class CDriveRecord
{
public:
	CDriveRecord();
	virtual ~CDriveRecord();

	int Init();
	int ReInit();
	int Release();

	void P_ThreadDrvRecWork();
	void P_TmDrvRecTimer();

	bool ChkDriverLog(char *v_pStrDrvNum);
	void hdl_diaodu(byte* data, int len);

	unsigned long GetTotleMeter();

private:
	void init_filecfg();

	// 协议处理函数
	void deal_center();
	void hdl_0117(byte* data, int len);
	void hdl_0127(byte* data, int len);	
	void hdl_0901(byte* data, int len);
	void hdl_0902(byte* data, int len);
	void hdl_0904(byte* data, int len);
	void hdl_0905(byte* data, int len);	
	void hdl_103e(byte* data, int len);	
	void hdl_3601(byte* data, int len);
	void hdl_3602(byte* data, int len);
	void hdl_3603(byte* data, int len);
	void hdl_3604(byte* data, int len);
	void hdl_3605(byte* data, int len);
	void hdl_3606(byte* data, int len);
	void hdl_3611(byte* data, int len);
	void hdl_3613(byte* data, int len);
	void hdl_3618(byte* data, int len);	
	void hdl_3619(byte* data, int len);	
	void hdl_3623(byte* data, int len);	
	void hdl_3701(byte* data, int len);
	void hdl_3702(byte* data, int len);
	void hdl_3707(byte* data, int len);
	void hdl_3709(byte *data, int len);
	void hdl_0f01(byte* data, int len);
	void hdl_0f02(byte* data, int len);	
	void hdl_0f03(byte* data, int len);
	void hdl_0f11(byte* data, int len);
	void hdl_0f12(byte* data, int len);
	
	// 各个定时操作
	void deal_login();
	void deal_state();
	void deal_meter(tagGPSData gps);
	void deal_alarm(tagGPSData gps);
	void deal_area(tagGPSData gps);		// 越界区域（0F 协议类型）
	void deal_updrv(tagGPSData gps);
	void deal_query();
	
	bool chk_doubrcd(RcdDoub *rcd, byte timecnt, byte time[10][10], byte regioncnt, byte region[5][16]);
	bool chk_drvrcd(RcdDrv *rcd, byte numdrv, byte times);
	bool chk_drvrcd2(RcdDrv *rcd,byte type, byte *s_time, byte *e_time, byte numdrv);
	void send_3644(RcdDoub *rcd, byte index, byte reply);
	
#if USE_OIL == 3
	void fill_frm3749(frm3749 &frm, int c_snd, ushort cltcyc, ushort sndcyc, tagGPSData gps);
#else
	void fill_frm3747(frm3747 &frm, int c_snd, ushort cltcyc, ushort sndcyc, tagGPSData gps);
#endif
	
	bool flush_headrcd();
	bool flush_doubrcd();
	bool flush_drvrcd();
	
	void set_car_state(tagGPSData gps);
	bool chk_overspeed();
	bool chk_belowspeed();
	bool chk_overtired(int minutes);
	bool alarm_overtired(int minutes);
	
	int get_minutes(int year, int month, int day, int hour, int minute, 
		int year2, int month2, int day2, int hour2, int minute2 );
	
	void tcp_send(byte trantype, byte datatype, void *data, int len);
	void req_login();
	void req_unlog();
	int  get_drvindex(int numdrv);
	
	bool cut_line(byte *dot1, byte *dot2, byte *dot0);
	
	ulong get_lstdoub(byte updoub, RcdDoub &rd);
	void set_doubtype(ulong pw);
	
	void set_alarm(ulong type, bool set);
	bool acc_valid();
	bool car_load();
	bool car_move();
	byte get_sensor();
	void show_diaodu(char *v_szbuf);
	void play_tts(char *v_szbuf);

public:		
	Queue tcprcv;			//TCP接收堆栈	
	bool m_bLcdLog;			//是否代开LCD日志
	bool m_bNeedClt; 
	byte m_bytAreaCode;
	bool m_bDrvRecQuit;		// 进行root升级时，要退出事故疑点采集线程

#if USE_LIAONING_SANQI == 1
	ushort TotalMeter;
	ulong  Mile;
#endif
	HRcdFile m_v;			//其他成员变量
private:
	pthread_t m_pthreadWork;

	DrvCfg m_cfg;			//行驶配置

	tag2QAreaCfg m_objAreaCfg;
	
	Queue m_quedoub;
	Queue m_quedrv;
	Queue m_quemeter;
	
	
};

#endif

#endif





