#ifndef _DATADEF_H_
#define _DATADEF_H_

#define MIN_GPRS_SIGNAL		5				// 能拨GPRS的最小信号值

#pragma pack(push,1)

	struct Frm10 {
		byte type;
		char dtmf;
		Frm10() { type = 0x10; }
	};

	struct Frm11 {
		byte type;
		char reply;
		Frm11() { type = 0x11; }
	};

	struct Frm12 {
		byte type;
		byte len;
		char num[256];
		Frm12() { memset(this, 0, sizeof(*this));  type = 0x12; }
	};

	struct Frm13 {
		byte type;
		char reply;
		Frm13() { type = 0x13; }
	};

	struct Frm14 {
		byte type;
		Frm14() { type = 0x14; }
	};

	struct Frm15 {
		byte type;
		char reply;
		Frm15() { type = 0x15; }
	};

	struct Frm16 {
		byte type;
		Frm16() { type = 0x16; }
	};

	struct Frm17 {
		byte type;
		char reply;
		Frm17() { type = 0x17; }
	};

	struct Frm18 {
		byte type;
		Frm18() { type = 0x18; }
	};

	struct Frm19 {
		byte type;
		byte len;
		char num[256];
		Frm19() { memset(this, 0, sizeof(*this));  type = 0x19; }
	};
	
	struct Frm20 {
		byte type;
		char dtmf;
		Frm20() { type = 0x20; }
	};

	struct Frm21 {
		byte type;
		Frm21() { type = 0x21; }
	};

	struct Frm22 {
		byte type;
		Frm22() { type = 0x22; }
	};

	struct Frm23 {
		byte type;
		Frm23() { type = 0x23; }
	};

	struct Frm24 {
		byte type;
		Frm24() { type = 0x24; }
	};

	struct Frm25 {
		byte type;
		byte level;
		Frm25() { type = 0x25; }
	};
	
	struct Frm26 {
		byte type;
		Frm26() { type = 0x26; }
	};

	struct Frm27 {
		byte type;
		byte reply;
		Frm27() { type = 0x27; }
	};

	struct Frm28 {
		byte type;
		byte dir;
		Frm28() { type = 0x28; }
	};

	struct Frm29 {
		byte type;
		byte reply;
		Frm29() { type = 0x29; }
	};

	struct Frm31 {
		byte type;
		byte reply;
		Frm31() { type = 0x31; }
	};

	struct Frm32 {
		byte type;
		byte reply;
		Frm32() { type = 0x32; }
	};

	// 网络状态通知 
	struct Frm35 {
		byte type;
		byte reply;		//1:网络恢复(拨号成功)  2:网络恢复(Ping成功)   3:网络断开了
		Frm35() { type = 0x35; }
	};

	struct Frm38 {
		byte type;
		byte source;
		char tel[16];
		char msg[160];
		Frm38() { memset(this, 0, sizeof(*this));  type = 0x38; }
	};

	struct Frm39 {
		byte type;
		byte reply;
		Frm39() { type = 0x39; }
	};

	struct FrmF1 {
		byte type;
		byte len;
		char num[256];
		FrmF1() { memset(this, 0, sizeof(*this));  type = 0xF1; }
	};

	struct FrmF2 {
		byte type;
		byte reply;
		FrmF2() { type = 0xF2; reply = 0x00; }
	};

	struct FrmF3 {
		byte type;
		byte reply;
		FrmF3() { type = 0xF3; reply = 0; }
	};

	struct FrmFF {		// add by zyq 090824
		byte type;
		byte iSignal;
		FrmFF() { type = 0xff; iSignal = 0;}
	};

	struct UserSM {
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

	struct UserSM2 {
		char pass[10];		// 密码
		char cmd[6];		// 命令
	};
	
	//--------用于呼叫限制------------------------
	// 电话记录
	struct Book {
		byte idx;		//索引号
		char tel[15];	//电话号码
		char name[20];	//姓名
		byte limit;		//权限设置
		byte tmhold[2];	//通话时长
	};	

	// 电话号码配置
	struct BookCfg {
		Book book[30];		//电话记录	
		char tel[5][15];	//电话号码
	};
	//-----------------------------------------------

#pragma pack(pop)

	
#if USE_LANGUAGE == 0
#define LANG_IOCHK_LIGHT ("检测到 车灯 信号变化")
#define LANG_IOCHK_SPEAKER ("检测到 电子喇叭 信号变化")
#define LANG_IOCHK_CENTLOCK_OFF ("检测到 中控锁关 信号变化")
#define LANG_IOCHK_CENTLOCK_ON ("检测到 中控锁开 信号变化")
#define LANG_IOCHK_ALERM_LED ("检测到 报警指示灯 信号变化")
#define LANG_IOCHK_ALERMFOOT ("检测到 抢劫报警 信号变化")
#define LANG_IOCHK_ACCON ("检测到 ACC有效 信号变化")
#define LANG_IOCHK_ACCOFF ("检测到 ACC 无效 信号变化")
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
#define LANG_IOCHK_JIJIA_VALID ("检测到 计价器信号有效")
#define LANG_IOCHK_PASSENGER1_VALID ("检测到 载客1 信号变化")
#define LANG_IOCHK_PASSENGER2_VALID ("检测到 载客2 信号变化")
#define LANG_IOCHK_PASSENGER3_VALID ("检测到 载客3 信号有效")
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
#define LANG_IOCHK_PASSENGER1_VALID ("IO: Passager load 1 valid")
#define LANG_IOCHK_PASSENGER2_VALID ("IO: Passager load 2 valid")
#define LANG_IOCHK_PASSENGER3_VALID ("IO: Passager load 3 valid")
#endif	//USE_LANGUAGE == 11


#endif	//#define _DATADEF_H_
