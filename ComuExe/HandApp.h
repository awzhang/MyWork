#ifndef _INC_CHSTAPP_H
#define _INC_CHSTAPP_H

/****手柄数据存储分配**HXD 090504 *******/ 

#define TB_REC_OFFSET    0 
#define TALK_DAIL_OFFSET 2000
#define TALK_RECV_OFFSET 2600
#define TALK_MISS_OFFSET 3200
#define ATM_REC_OFFSET   4000
#define SM_REC_OFFSET    9000
#define DIANZAO_REC_OFFSET    14000

/**************MainWin**********************************************************/
#define SIZE_TELEDITBUF                         20

/**************VirPhone**********************************************************/
#define SIZE_DTMFBUF							30
//#define SIZE_DPLYBUF							30

#define VIR_FREE								0
#define VIR_CALLING								1
#define VIR_TALKING								2
#define VIR_RINGING								3

#define CHA_HANDSET								2
#define CHA_HANDSFREE							4

/****************CallMan********************************************************/
#define TALK_DIAL								0
#define TALK_RECV								1
#define TALK_MISS								2

#define MAX_TALKREC								10

#define TALKREC_SIZE_TEL						20
#define TALKREC_SIZE_DATE						10
#define TALKREC_SIZE_TIME						10

/****************TelBook********************************************************/
#define MAX_TBREC							    50 //	50  //hxd 090424

#define TBREC_SIZE_NAME							10
#define TBREC_SIZE_TEL							20

#define SIZE_DPLYBUF                            50

#define OP_ADD                                  0
#define OP_MODIFY                               1
#define OP_FIND                                 2

/********************ShortMsg****************************************************/
#define MAX_SMREC								20

#define SMREC_SIZE_TIME							12
#define SMREC_SIZE_TEL							16
#define SMREC_SIZE_DATA							160 //161 hxd 090425 按测试部的结果修改

/********************Succor****************************************************/
#define SUCCOR_MEDICAL                          0
#define SUCCOR_TROUBLE                          1
#define SUCCOR_INFORMATION                      2

/********************SetGPRS****************************************************/
#define SIZE_RECVBUF							30
#define TYPE_TCP_PORT1							0
#define TYPE_UDP_PORT1							1
#define TYPE_TCP_IP1							3
#define TYPE_UDP_IP1							4
#define TYPE_TCP_PORT2							5
#define TYPE_UDP_PORT2							6
#define TYPE_TCP_IP2							7
#define TYPE_UDP_IP2							8
#define TYPE_APN								9
#define TYPE_TEL								10
#define TYPE_LISTENTEL							11
#define TYPE_CDMANAME							12
#define TYPE_CDMAPASS							13

/********************SetCGV****************************************************/
#define TYPE_COMPANYID							0
#define TYPE_TERMIVER							1
#define TYPE_GROUPID							2

/********************SysSet****************************************************/
//对比度
#define MAX_STEP								9
#define INIT_STEP								4
#define VALUE_STEP								3
//振铃音
#define OP_CALLRING                             0

/*****************SaveDataType**************************************************/
#define	TYPE_TB									0
#define TYPE_TK									1
#define TYPE_SM									2									

/******************AtmpMsg******************************************************/
#define MAX_ATMPREC								20

#define ATMPREC_SIZE_DATE						10
#define ATMPREC_SIZE_TIME						10
#define ATMPREC_SIZE_DATA						180

/*******************PhoneCallMsg*****************************************************/
#define MAX_PCREC								MAX_ATMPREC
#define PC_SIZE_DATE							ATMPREC_SIZE_DATE
#define PC_SIZE_TIME							ATMPREC_SIZE_TIME
#define PC_SIZE_DATA							ATMPREC_SIZE_DATA

#define CON_COE									(3600*1000)	//转换系数

typedef	void   (*RETINFORMER)(INT8U tellen, INT8U *tel, INT8U namelen, INT8U *name);
	   
struct Frm053d {			//电召抢答信息
	char	tel[15];		//乘客联系方式
	char	car_id[15];		//车牌号
	char	content[50];	//简短说明
	Frm053d()	{ memset(this, 0, sizeof(*this)); }
};

class CHstApp
{
public: 	
	CHstApp();
	~CHstApp();

	int Init();
	int Release();

	void P_ThreadHandleWork();

	void P_TmDrvRec();
	void P_TmIsOnLine();
	
private:
	enum {
		PowerOn = 0,		//上电
		Idle,				//空闲
		PowerOff,			//手柄断电时释放手柄模块资源
	}m_hststate;

	pthread_t		m_pthreadHandleWork;

	static ulong	m_hsttm;	//手柄校时时间
	static bool		m_IsRefuseDial;	//是否拒绝拨打电话
	int				m_DialCount;	//拨打电话成功次数
	static bool		m_IsReturnMainWin;		//是否回主界面

	void Init_Menu();

private:
	static tag2QHstCfg		m_HstCfg;
	static INT8U			m_hangupstate;		//挂机状态应答
	INT8U					m_tempbuf[100];
	static bool				m_hstmode;
	static INT8U			m_helptype;			//求助类型
	PROMPT_STRUCT			m_dianzhaoprompt;	//电召提示信息
//	static bool				m_flagdz;			//判断应答是电召抢答还是电召结果报告
	static ulong			m_diatm;			//电话接通起点时刻
	static EDITOR_STRUCT	EditorEntry_MainWin;  //cyh  编辑界面的处理函数
	char					m_diasucctm[20];	//拨号成功时刻
	char					m_recvtm[20];		//来电时刻或未接电话未接时刻
	ulong					m_talktm;			//电话接通时刻
	ulong					m_calltm;			//通话时长
	char					m_strcalltm[10];
//	bool					m_IsmissTel;		//是否是未接来电
//	bool					m_Iscallsucc;		//拨打电话时对方是否接通
	static INT8U			m_state_tel;		//未接还是拨号对方未接
	static INT8U			m_dianzhaostate;	//电召状态
	ulong					m_dianzhaotm;		//收到电召信息时刻
	ulong					m_qiangdatm;		//抢答时刻
	ulong					m_qiangdasucctm;	//抢答成功时刻
	static bool				m_IsPDkey;			//是否有按键
	Frm053d					m_frm053d;
	static int				m_testalarmsta;		//报警测试状态
	static int				m_msgsta;			//是调度信息还是电召信息
	static int				m_pickupsta;		//自动/手动接听状态
	static int				m_gpsdect;			//检测GPS信号
	static int				m_gprssta;			//设置的GPRS类型 千里眼/流媒体

	typedef struct {
		INT16U     index;
		void (* entryproc)(void);
	} FUNCENTRY_STRUCT;
	static FUNCENTRY_STRUCT Handler_Key_entry[7];  //cyh  主界面下，处理功能按键的函数。主界面下，就处理这7个按键，其余不处理。
	static bool FindProcEntry(INT16U index, FUNCENTRY_STRUCT *funcentry, INT16U num);
	
/*************MainWin***********************************************************/
	static KEY_STRUCT		m_curkey;
	static bool				m_KBIsLocked, m_WaitKEY_Star;
	static INT8U			m_TelEditBuf[SIZE_TELEDITBUF];
	ITEMMENU_STRUCT			MENU_MAIN_CHILD[11];  //cyh add：主界面下，按下SET后激活的9个菜单
	static ITEMMENU_STRUCT	MENU_MAIN;  //cyh add  主界面下，按下SET后激活的菜单，保存链接到9个子菜单的入口指针
	static WINSTACKENTRY_STRUCT WinStackEntry_MainWin;

/*************VirPhone***********************************************************/
	typedef struct {
		INT8U       attrib;
		INT8U       channel;
		INT8U       tellen;
		INT8U       tel[TALKREC_SIZE_TEL];
	} ENTRYTEL_STRUCT;
	static INT8U			m_status_phone;
	static INT8U			m_dtmfbuf[SIZE_DTMFBUF], m_dtmflen, m_dtmfpos;
	static INT8U			m_dplybuf_phone[SIZE_DPLYBUF], m_dplyptr_phone[SIZE_DPLYBUF], m_dplylen_phone;
	static ENTRYTEL_STRUCT	EntryTel;

	static PROMPT_STRUCT	PromptEntry_Phone;
	static WINSTACKENTRY_STRUCT WinStackEntry_Dia;			//拨号窗口  //cyh 拨号界面的窗口函数
	static WINSTACKENTRY_STRUCT WinStackEntry_DialSuccor;	//求助拨号窗口
	static WINSTACKENTRY_STRUCT WinStackEntry_Ring;			//来电窗口
	
/*************CallMan***********************************************************/
	//sizeof(TALKREC_STRUCT)=29
	typedef struct {
		INT8U       tellen;
		char		tel[TALKREC_SIZE_TEL];
		char		talktime[10];
		char		date[TALKREC_SIZE_DATE];
		char		time[TALKREC_SIZE_TIME];
	} TALKREC_STRUCT;		//通话记录
	
	typedef struct {
		INT8U   NumTel;
		INT8U   NumSM;
	} HSTMISS_STRUCT;

	static HSTMISS_STRUCT	HstMiss;
	static MENUWIN_STRUCT	SelMenuWinEntry;		
	ITEMMENU_STRUCT			MENU_ERASE_DIAL;		//删除已拨电话
	ITEMMENU_STRUCT			MENU_ERASE_MISS;		//删除未接电话
	ITEMMENU_STRUCT			MENU_ERASE_RECV;		//删除已接电话
	ITEMMENU_STRUCT			MENU_ERASE_ALL;			//删除所有通话记录
	ITEMMENU_STRUCT			MENU_ERASE_CHILD[4];
	ITEMMENU_STRUCT			MENU_MISS;				//未接来电
	ITEMMENU_STRUCT			MENU_DIAL;				//已拨电话
	ITEMMENU_STRUCT			MENU_RECV;				//已接电话
	ITEMMENU_STRUCT			MENU_ERASE;				//删除
	ITEMMENU_STRUCT			MENU_CALLMAN_CHILD[4];
	ITEMMENU_STRUCT			MENU_CALLMAN;			//通话记录
	static INT8U			m_talktype;
	static TALKREC_STRUCT	CurRecord;
	//直接按下OK键后出来的通话记录菜单
	ITEMMENU_STRUCT			MENU_OPREC_DIAL;		//呼出
	ITEMMENU_STRUCT			MENU_OPREC_READ;		//通话时间
	ITEMMENU_STRUCT			MENU_OPREC_ERASE;		//删除
	ITEMMENU_STRUCT			MENU_OPREC_DISPLAY;		//显示通话时间
	ITEMMENU_STRUCT			MENU_OPREC_SAVE;		//存储
	ITEMMENU_STRUCT			MENU_OPREC_CHILD_CM[5];
	static ITEMMENU_STRUCT	MENU_OPREC_CM;
	static PROMPT_STRUCT	PromptEntry_CM;
	
	TALKREC_STRUCT			m_calllog;			//通话记录
	
/*************TelBook***********************************************************/
	ITEMMENU_STRUCT			MENU_ADD;				//增加
	ITEMMENU_STRUCT			MENU_FIND;				//查找
	ITEMMENU_STRUCT			MENU_LIST;				//浏览
	ITEMMENU_STRUCT			MENU_DelALL;			//删除所有
	ITEMMENU_STRUCT			MENU_Browse;			//存储状况

	ITEMMENU_STRUCT			MENU_TELBOOK_CHILD[5];
	ITEMMENU_STRUCT			MENU_TELBOOK;			//电话本
	//每条电话记录对应的子菜单
	ITEMMENU_STRUCT			MENU_DIAL_TB;			//呼出
	ITEMMENU_STRUCT			MENU_DEL_TB;			//删除
	ITEMMENU_STRUCT			MENU_MODIFY_TB;			//修改
	ITEMMENU_STRUCT			MENU_OPREC_CHILD_TB[3];
	static ITEMMENU_STRUCT	MENU_OPREC_TB;

	//sizeof(TBREC_STRUCT)=32
	typedef struct {
		INT8U       namelen;
		INT8U       name[TBREC_SIZE_NAME];
		INT8U       tellen;
		INT8U       tel[TBREC_SIZE_TEL];
	} TBREC_STRUCT;
	static TBREC_STRUCT		currecord, readrecord;
	static INT16S			m_selrecord_tb;
	static INT8U			m_op_type_tb;  //cyh add: 对电话本的操作方式。增加，修改，查找
	static char				m_dplybuf[SIZE_DPLYBUF];
	static MENUWIN_STRUCT	ListTBEntry;
	static RETINFORMER	RetInformer;		

	static BOOL             m_bDrvHaveLog;//hxd 090428

/*************SaveData***********************************************************/
	struct SAVEDATAINDEX_STRUCT
	{
		INT8U	num_TB;             // 电话本个数
		INT8U	num_TK_dia;        // 已拨电话
		INT8U	num_TK_rec;        // 已接电话
		INT8U	num_TK_mis;         // 未接电话
		INT8U	num_Atmp;			 //调度信息
		INT8U	num_SM;             // 短信个数
		INT8U	num_PC;				//电召个数
		INT8U	flag_TB[MAX_TBREC];	//该条电话记录的位置是否被使用的标记,1:已占用,0:未占用
		INT8U	flag_TK_dia[MAX_TALKREC];
		INT8U	flag_TK_rec[MAX_TALKREC];
		INT8U	flag_TK_mis[MAX_TALKREC];
		INT8U	flag_Atmp[MAX_ATMPREC];
		INT8U	flag_SM[MAX_SMREC];
		INT8U	flag_PC[MAX_PCREC];	//电召个数
		SAVEDATAINDEX_STRUCT() { init(); }
		void init()
		{
			memset(this, 0, sizeof(*this));
		}
	};

/**************ShortMsg**********************************************************/
	ITEMMENU_STRUCT			MENU_READSM;			//收到的信息
	ITEMMENU_STRUCT			MENU_WRITESM;			//写信息
	ITEMMENU_STRUCT			MENU_DELALL;			//删除所有信息
	ITEMMENU_STRUCT			MENU_SHORTMSG_CHILD[3];
	ITEMMENU_STRUCT			MENU_SHORTMSG;			//短消息

	ITEMMENU_STRUCT			MENU_WRITEBACK;			//回复
	ITEMMENU_STRUCT			MENU_TRANSMIT;			//转发
	ITEMMENU_STRUCT			MENU_ERASE_SM;			//删除
	ITEMMENU_STRUCT			MENU_SAVE_SM;			//保存
	ITEMMENU_STRUCT			MENU_DIAL_SM;			//拨号
	ITEMMENU_STRUCT			MENU_OPSM_CHILD[5];		
	static ITEMMENU_STRUCT	MENU_OPSM;				
	static MENUWIN_STRUCT	MENU_LISTSM;			//列出短消息

	//sizeof(SMREC_STRUCT)=190
	typedef struct {
		INT8U		unread;
		INT8U       tellen;
		INT8U       tel[SMREC_SIZE_TEL];
		INT8U       time[SMREC_SIZE_TIME];
		INT8U       datalen;
		INT8U       data[SMREC_SIZE_DATA];
	} SMREC_STRUCT;
	typedef struct  
	{
		byte		cmd;
		int			len0;		//电话号码长度
		char		tel[16];	//电话号码
		int			len1;		//短信内容长度
		char		info[160];	//短信内容
	} SMS_STRUCT;
	static INT16S			m_selrecord_sm;
	static SMREC_STRUCT		CurRecord_SM, EditRecord;
	SMREC_STRUCT			m_smrec;
	static INT8U			m_storebuf[300];
	static EDITOR_STRUCT	TelEditorEntry;

/*************SetGPRS***********************************************************/
	ITEMMENU_STRUCT			MENU_SETGPRS;			//设置GPRS参数
	static INT8U			m_recvbuf[SIZE_RECVBUF];
	static EDITOR_STRUCT	EditorEntry;
	typedef struct {
		INT8U ip;
		INT8U port;
		INT8U apn;
		INT8U tel;
		INT8U listentel;
		INT8U cdmaname;
		INT8U cdmapass;
	}SETGPRS_STRUCT;	//设置GPRS数据结构
	static SETGPRS_STRUCT	set_gprs;
	ITEMMENU_STRUCT			MENU_TCPPORT1;			//设置千里眼TCP端口1
	ITEMMENU_STRUCT			MENU_UDPPORT1;			//设置千里眼UDP端口1
	ITEMMENU_STRUCT			MENU_TCPIP1;			//设置千里眼TCPIP1
	ITEMMENU_STRUCT			MENU_UDPIP1;			//设置千里眼UDPIP1
	ITEMMENU_STRUCT			MENU_TCPPORT2;			//设置千里眼TCP端口2
	ITEMMENU_STRUCT			MENU_UDPPORT2;			//设置千里眼UDP端口2
	ITEMMENU_STRUCT			MENU_TCPIP2;			//设置千里眼TCPIP2
	ITEMMENU_STRUCT			MENU_UDPIP2;			//设置千里眼UDPIP2
	ITEMMENU_STRUCT			MENU_APN;				//设置APN
	ITEMMENU_STRUCT			MENU_TEL;				//设置本级手机号
	ITEMMENU_STRUCT			MENU_LISTENTEL;			//设置监听电话代号
	ITEMMENU_STRUCT			MENU_ACTIVATE;			//激活GPRS
	ITEMMENU_STRUCT			MENU_CDMANAME;          //设置CDMA用户名
	ITEMMENU_STRUCT			MENU_CDMAPASS;			//设置CDMA密码
//	ITEMMENU_STRUCT			MENU_SET_CHILD[9];	    // hxd 090713	
//add lzy 101026
	ITEMMENU_STRUCT			MENU_TERMIVER;
	ITEMMENU_STRUCT			MENU_SETCOMP;
	ITEMMENU_STRUCT			MENU_SETGROUPID;

//	
#if USE_LIAONING_SANQI == 1													//
	ITEMMENU_STRUCT			MENU_SET_CHILD[16];	    	
#else
	ITEMMENU_STRUCT			MENU_SET_CHILD[13];		// hxd 090713
#endif																
	static ITEMMENU_STRUCT  MENU_SET;				//设置GPRS参数
	ITEMMENU_STRUCT			MENU_LIP;				//设置流媒体IP
	ITEMMENU_STRUCT			MENU_LPORT;				//设置流媒体端口
	ITEMMENU_STRUCT			MENU_SET_LCHILD[2];
	static ITEMMENU_STRUCT	MENU_SETL;				//设置流媒体参数				

/****************SysSet********************************************************/
	ITEMMENU_STRUCT			MENU_VERSION;			//版本说明
	ITEMMENU_STRUCT			MENU_SYSSET_CHILD[6];
	ITEMMENU_STRUCT			MENU_SYSSET;			//系统设置
	//对比度调节	
	static INT8U			m_curvalue;				//对比度当前值
	static WINSTACKENTRY_STRUCT WinStackEntry_AD;	//调节对比度入口
	ITEMMENU_STRUCT			MENU_ADCONST;			//调节对比度
	//选择振铃
	static INT8U			m_optype;				//当前振铃音
	ITEMMENU_STRUCT			MENU_CALLRING;			//来电铃声
	ITEMMENU_STRUCT			MENU_SMRING;			//短信铃声
	ITEMMENU_STRUCT			MENU_SELRING_CHILD[2];
	ITEMMENU_STRUCT			MENU_SELRING;			//设置振铃音
	static MENUWIN_STRUCT	MenuWinEntry ;
	//设置按键音
	ITEMMENU_STRUCT			MENU_SETKEYPT;			//设置按键音
	ITEMMENU_STRUCT			MENU_OPEN;				//打开按键音
	ITEMMENU_STRUCT			MENU_CLOSE;				//关闭按键音
	ITEMMENU_STRUCT			MENU_SELOPTION_CHILD[2];
	static ITEMMENU_STRUCT	MENU_SELOPTION;
	//设置接听方式
	ITEMMENU_STRUCT			MENU_SETPICK;			//设置接听方式
	ITEMMENU_STRUCT			MENU_AUTO;				//自动接听
	ITEMMENU_STRUCT			MENU_MANUAL;			//手动接听
	ITEMMENU_STRUCT			MENU_SELPICK_CHILD[2];
	static ITEMMENU_STRUCT  MENU_SELPICK;
	//设置缺省模式
	ITEMMENU_STRUCT			MENU_SETCHA;			//设置缺省模式
	ITEMMENU_STRUCT			MENU_AUTO_CHA;			//免提模式
	ITEMMENU_STRUCT			MENU_MANUAL_CHA;		//手柄模式
	ITEMMENU_STRUCT			MENU_SELCHA_CHILD[2];
	static ITEMMENU_STRUCT  MENU_SELCHA;
		
/*************ParaChk***********************************************************/
	ITEMMENU_STRUCT			MENU_CENTERNO;			//查询中心特服号
	ITEMMENU_STRUCT			MENU_GPRSSTATUS;		//查询GPRS状态
	ITEMMENU_STRUCT			MENU_GPSSTATUS;			//查询GPS状态
	ITEMMENU_STRUCT			MENU_PARACHK_CHILD[3];
	ITEMMENU_STRUCT			MENU_PARACHK;			//查询参数状态
	static PROMPT_STRUCT	ParaPrompt;				//查询参数提示信息

/*****************************************************************************/
//wxl
    ITEMMENU_STRUCT			MENU_DATAREQUEST;       //油量检测盒油量数据请求
    ITEMMENU_STRUCT			MENU_BIAODINGSTART;		//油量检测盒标定起始指示
	ITEMMENU_STRUCT			MENU_GRADUATIONBD;      // ACCON 刻度标定
	ITEMMENU_STRUCT			MENU_GRADUATIONBDOFF;      //ACCOFF 刻度标定
	ITEMMENU_STRUCT			MENU_ZEROBD;			// 0刻度标定
	ITEMMENU_STRUCT			MENU_QUERTERBD;			//ACCON 1/4刻度标定
	ITEMMENU_STRUCT			MENU_HALFBD;			// 1/2刻度标定
	ITEMMENU_STRUCT			MENU_THREEFOURBD;		// 3/4刻度标定
	ITEMMENU_STRUCT			MENU_FULLBD;			// 满刻度标定
	ITEMMENU_STRUCT			MENU_ZEROBDOFF;			// ACCOFF 0刻度标定
	ITEMMENU_STRUCT			MENU_QUERTERBDOFF;			// 1/4刻度标定
	ITEMMENU_STRUCT			MENU_HALFBDOFF;			// 1/2刻度标定
	ITEMMENU_STRUCT			MENU_THREEFOURBDOFF;		// 3/4刻度标定
	ITEMMENU_STRUCT			MENU_FULLBDOFF;			// 满刻度标定
	ITEMMENU_STRUCT			MENU_BIAODINGEND;		//标定完成指示
	ITEMMENU_STRUCT			MENU_BIAODING;			//刻度标定
	ITEMMENU_STRUCT			MENU_BIAODINGTOCENTER;	//对中心发送油量检测盒标定数据请求
    ITEMMENU_STRUCT			MENU_OILCHK_CHILD[5];
	ITEMMENU_STRUCT			MENU_GRADUATIONBD_CHILD[5];
	ITEMMENU_STRUCT			MENU_GRADUATIONBDOFF_CHILD[5];
	ITEMMENU_STRUCT			MENU_BIAODING_CHILD[2];  
	ITEMMENU_STRUCT			MENU_OILCHK;            //油量检测盒查询
    static PROMPT_STRUCT    OilPrompt;				//油量检测盒查询提示信息
	
//wxl

/*****************************************************************************/
//hhq
    ITEMMENU_STRUCT			MENU_CHN1VIEW;       //预览1通道
    ITEMMENU_STRUCT			MENU_CHN2VIEW;		 //预览2通道
	ITEMMENU_STRUCT			MENU_CHN3VIEW;       //预览3通道
	ITEMMENU_STRUCT			MENU_CHN4VIEW;       //预览4通道
	ITEMMENU_STRUCT			MENU_CHNAVIEW;       //预览全通道
	ITEMMENU_STRUCT			MENU_VIDEOVIEW_CHILD[5];
	ITEMMENU_STRUCT			MENU_VIDEOVIEW;		 //预览通道
//hhq

/***************AtmpMsg*********************************************************/
	typedef struct {
		char		date[ATMPREC_SIZE_DATE];
		char		time[ATMPREC_SIZE_TIME];
		INT8U       datalen;
		char		data[ATMPREC_SIZE_DATA];
	} ATMPREC_STRUCT;		//调度信息结构
	static ATMPREC_STRUCT	CurRec_Atmp;			//当前调度信息提示
	static ATMPREC_STRUCT   m_tagDiaoduMsg;//cyh add:add  防止调度信息与电召信息相互覆盖。如果有电召时，又来了一条调度信息就麻烦了。。。
	PROMPT_STRUCT			AtmpPrompt;
	ATMPREC_STRUCT			m_atmprec;
	ITEMMENU_STRUCT			MENU_ATMPMSG;			//调度信息
	static MENUWIN_STRUCT	MENU_LISTATMP;			//读调度信息列表
	static MENUWIN_STRUCT	MENU_READCONTENT;		//读调度信息窗口

/***************PhoneCallMsg*********************************************************/
//	typedef struct  {
//		char		date[PC_SIZE_DATE];
//		char		time[PC_SIZE_TIME];
//		INT8U       datalen;
//		char		data[PC_SIZE_DATA];
//	} PHONECALL_STRUCT;		//电召信息结构
	static ATMPREC_STRUCT	CurRec_PC;
//	PROMPT_STRUCT			PcPrompt;
//	ATMPREC_STRUCT			m_pcrec;
	ITEMMENU_STRUCT			MENU_PCMSG;				//电召信息
	static MENUWIN_STRUCT	MENU_LISTPC;			//读电召信息列表
	static MENUWIN_STRUCT	MENU_READCONTENT_PC;	//读电召信息窗口

/***************ProjectMenu*********************************************************/
	static ITEMMENU_STRUCT	MENU_PROJECT;
	ITEMMENU_STRUCT			MENU_PROJECTACCESS;	
	ITEMMENU_STRUCT			MENU_PROJECT_CHILD[14];
	ITEMMENU_STRUCT			MENU_UPDATESYS;			//U盘系统升级
	ITEMMENU_STRUCT			MENU_RESUMESET;			//恢复出厂设置
	ITEMMENU_STRUCT			MENU_FORMATSDISK;		//格式化SD卡
	ITEMMENU_STRUCT			MENU_UPDATEAPP;			//U盘应用升级
	ITEMMENU_STRUCT			MENU_TESTCUTOIL;		//测试断油断电
	ITEMMENU_STRUCT			MENU_INPUTVIDEO;		//视频文件导入导出
	ITEMMENU_STRUCT			MENU_UDATAOUTPUT;		//U盘数据导出
	ITEMMENU_STRUCT			MENU_TESTPCSOUND;		//PC音频测试
	ITEMMENU_STRUCT			MENU_TESTCAM;			//摄像头1检测
	ITEMMENU_STRUCT			MENU_CHKSENSORSTA;		//查询传感器状态
	ITEMMENU_STRUCT			MENU_TESTALARM;			//报警测试
	ITEMMENU_STRUCT			MENU_CHKLED;			//LED屏检测
	ITEMMENU_STRUCT			MENU_CHKGPS;			//检测GPS信号
	ITEMMENU_STRUCT			MENU_CHKLED_CHILD[4];	
	ITEMMENU_STRUCT			MENU_TESTCAM_CHILD[4];
	ITEMMENU_STRUCT			MENU_CHKLED_TIME;		//给LED校时
	ITEMMENU_STRUCT			MENU_CHKLED_SHOW;		//给LED发即使信息
	ITEMMENU_STRUCT			MENU_CHKLED_BLAC;		//发送黑屏指令给LED
	ITEMMENU_STRUCT			MENU_CHKLED_LIGH;		//发送亮屏指令给LED
	ITEMMENU_STRUCT			MENU_TESTCAM1;			//摄像头2检测
	ITEMMENU_STRUCT			MENU_TESTCAM2;			//摄像头2检测
	ITEMMENU_STRUCT			MENU_TESTCAM3;			//摄像头3检测
	ITEMMENU_STRUCT			MENU_TESTCAM4;			//摄像头4检测
	ITEMMENU_STRUCT			MENU_LOGINLIU;
	static PROMPT_STRUCT	ProjectPrompt;

/***********DiaoZhao*************************************************************/
	static void HdlDianZhao();
	static void HdlQiangDa();

/************TestAlarm************************************************************/
	static void HdlTestAlarm();

/*************MainWin***********************************************************/
	static void DisplayScreen(void);
	static void SuspendProc_MainWin(void);
	static void ResumeProc_MainWin(void);
	static void InitProc_MainWin(void);
	static void HdlKeyProc_MainWin(void);
	static void Handler_KEY_ACK(void);
	static void Handler_KEY_FCN(void);
	static void Handler_KEY_UP(void);
	static void Handler_KEY_DOWN(void);
	static void Handler_KEY_SUCCOR(void);
	static void ExitTelEditor(void);
	static void Handler_KEY_DIGITAL(void);
	static void    ActivateMainWin(void);
	void    InitMainWin(void);

/***************VirPhone*********************************************************/
	static bool Display_DTMF(void);
	static void AddDtmfChar(INT8U ch);
	static void DelDtmfChar(void);
	static void ModifyDtmfChar(INT8U ch);
	static bool HaveDtmfChar(void);
	static void InitDtmfBuf(void);
	static void GetDisplayPara(void);
	static void Display_COMMON(INT8U textlen, char *textptr, INT8U tellen, INT8U *tel);
	static void Display_CALLING(void);
	static void Display_CALLING_INIT(void);
	static void Display_TALKING(void);
	static void Display_RINGING(void);
	static void Display_VOLUME(void);
	static void Display_CHANNEL(void);
	static void Status_Phone(void);
	static void ExitVirPhone(char *str, INT8U time);
	static void InitProc_Dial(void);
	static void InitProc_Ring(void);
	static void DestroyProc_Phone(void);
	static void Handler_CALLING(INT8U phonestatus);
	static void Handler_TALKING(INT8U phonestatus);
	static void Handler_RINGING(INT8U phonestatus);
	static void HdlEventProc(INT8U phonestatus);
	static void HdlKeyProc_Phone(void);
	static void    ActivateVirPhone_Dial(bool findsk, INT8U tellen, INT8U *tel);
	void    ActivateVirPhone_DialSuccor(INT8U tellen, INT8U *tel);
//	void    ActivateVirPhone_Ring(void);
//	void    HangupVirPhone(void);
//	void    ResetVirPhone(void);
//	void    InitVirPhone(void);
//	void    InformVirPhone(INT8U phonestatus);
//	bool	VirPhoneIsActivated(void);

/*************CallMan***********************************************************/
	static void OPREC_DIAL(void);
	static void OPREC_READ(void);
	static void ACK_OP_ERASE(void);
	static void OPREC_ERASE(void);
	static void OPREC_DISPLAY(void);
	static void OPREC_SAVE(void);
	static void Handler_KEY_ACK_CallMan(void);
	static INT16U MaxRecordProc_CallMan(void);
	static INT16U ReadRecordProc_CallMan(INT16S numrec, INT8U *ptr);
	static void SelTalkProc(void);
	static void ACK_DelTalkProc(void);
	static void DelTalkProc(void);
	static void ListMissTel(void);
	static void ExitPromptReadMissTel(void);
	static void    ActivateDialTalkRec(void);
	static bool PromptReadMissTel(void);
	//hxd 090428
	static void InputDrvLog(void);
	static void ACK_InputDrvNum(void);//许宁的司机登陆模块的处理函数
	static void ACK_InputDrvNum_AD(void);//阿杜的司机登陆模块的处理函数
	static void ACK_ExitDrvPro(void);

/*************TelBook***********************************************************/
	static void AddRec(void);
	static void ACK_AddRec(void);
	static void ModifyRec(void);
	static void ACK_ModifyRec(void);
	static void ExitPhoneEditor(void);
	static void ActivatePhoneEditor(void);
	static void ExitNameEditor(void);
	static void ActivateNameEditor(void);
	static void ActivateNameEditor_Md(void);
	static void AddTB(void);
	static void ACK_DelProc(void);
	static void DelProc(void);
	static void ModifyProc(void);
	static void DialProc(void);
	static void Handler_KEY_ACK_TelBook(void);
	static void Handler_KEY_ACK_RET(void);
	static INT16U MaxRecordProc_TelBook(void);
	static INT16U InitRecordProc_TelBook(void);
	static INT16U ReadRecordProc_TelBook(INT16S numrec, INT8U *ptr);
	static void ListTB_Init(INT16U initrecord);
	static void FindTB(void);
	static void ACK_DelAllTB(void);
	static void DelAllTB(void);
	static void BrowseMemory(void);
	static void    ListTB(void);
	static bool	ListTB_RET(void Informer(INT8U tellen, INT8U *tel, INT8U namelen, INT8U *name));
	static void    SaveToTB(INT8U *tel, INT8U tellen);

/*************SaveData***********************************************************/
	static bool AddTBRec(TBREC_STRUCT *recptr);
	void AddToTB(TBREC_STRUCT *recptr);
	static bool ModifyTBRec (INT16U numrec, TBREC_STRUCT *recptr);
	static void DelTBRec(INT16U numrec);
	static INT16S GetTBRecByName_FULL(INT8U len, INT8U *name);
	static INT16U GetTBNameByTel(INT8U *ptr, INT8U len, INT8U *tel);
	static INT16U GetTBRec(INT16U numrec, TBREC_STRUCT *recptr);
	static INT16U GetTBItem(void);
	static void DelDB_TB(void);
	static INT16U GetTBBlankItem(void);
	static void DelTalkRecord(INT8U talktype, INT16U numrec);
	static INT16U GetTalkRecord(INT8U talktype, INT16U numrec, TALKREC_STRUCT *recptr);
	static INT16U GetNumTalkRecord(INT8U talktype);
	static void DelAllTalkRecord(INT8U talktype);
	void SaveTKToFile(INT8U talktype, TALKREC_STRUCT *recptr);
	static bool SaveHstConfig();
	//ShortMsg
	static void AddSMRec(SMREC_STRUCT *smrec);
	static void DelSMRec(INT16U numrec);
	static INT16U GetSMRec(INT16U numrec, SMREC_STRUCT *ptr);
	static void DelAllSMRec(void);

/****************ShortMsg********************************************************/
	static void ACK_EraseSM(void);
	static void EraseSM(void);
	static void TransmitSM(void);
	static void WriteBackSM(void);
	static void SaveTel(void);
	static void DialTel_SM(void);
	static void Handler_KEY_ACK_CONTENT(void);
	static INT16U MaxRecordProc_CONTENT(void);
	static INT16U ReadRecordProc_CONTENT(INT16S numrec, INT8U *ptr);
	static void Handler_KEY_ACK_SM(void);
	static INT16U MaxRecordProc_SM(void);
	static INT16U ReadRecordProc_SM(INT16S numrec, INT8U *ptr);
	static void ReadSM(void);
	static void SendInformer(INT8U result);
	static void GetTelFromTB(INT8U tellen, INT8U *tel, INT8U namelen, INT8U *name);
	static void ExitTelEditor_SM(void);
	static void AckSMEditor(void);
	static INT16U CurLenProc(INT16U len, INT8U *ptr);
	static INT16U MaxLenProc(INT16U len, INT8U *ptr);
	static void WriteSM(void);
	static void ReadSMSTel(void);
	static void ACK_DelAll(void);
	static void DelAll(void);
	static void    ActivateReadShorMsgMenu(void);
	static void CreateSMEditor(void);
	static void CreateTelEditor(void);
	static INT8U HaveUnReadSMRec(void);
	static void ClearSMRecUnReadFlag(INT16U numrec);
	static INT16U GetSMRecItem(void);
	static INT8U SMRecUnRead(INT16U numrec);
	static INT8U GetSMRecTel(INT16U numrec, INT8U *ptr);
	void SendSMToCustomer();

/****************Succor********************************************************/
	static void PromptResult(INT8U type, INT8U result);
	static void Informer_Medical(INT8U result);
	static void Informer_Trouble(INT8U result);
	static void Informer_Information(INT8U result);
	static void StartSuccor(INT8U type);

/****************SetGPRS********************************************************/
	static void Set_GPRS_PARA(INT8U type, INT8U len, INT8U *ptr);
	
	static void SetTCPIP1(void);		
	static void SetUDPIP1(void);
	static void SetTCPPort1(void);	
	static void SetUDPPort1(void);
	static void ACK_SetTCPIP1(void);
	static void ACK_SetUDPIP1(void);
	static void ACK_SetTCPPort1(void);
	static void ACK_SetUDPPort1(void);

	static void SetTCPIP2(void);		
	static void SetUDPIP2(void);
	static void SetTCPPort2(void);	
	static void SetUDPPort2(void);
	static void ACK_SetTCPIP2(void);
	static void ACK_SetUDPIP2(void);
	static void ACK_SetTCPPort2(void);
	static void ACK_SetUDPPort2(void);

	static bool CheckIPValid(INT8U len);

	static bool FilterProc(INT8U *ch);
	static void ACK_SetAPN(void);
	static void SetAPN(void);
	static void ACK_SetTel(void);
	static void SetTel(void);
	static void ACK_SetListenTel(void);
	static void SetListenTel(void);
	static void SetCdmaName(void);
	static void SetCdmaPass(void);
	static void ACK_SetCdmaName(void);
	static void ACK_SetCdmaPass(void);
	static void Activate(void);
	static void ACK_InputPassword(void);
	static void InputPassword(void);
	static void SetLiuIP();
	static void SetLiuPort();

/****************SysSet********************************************************/
	static void ReadVersion(void);
	//对比度调节
	static void DisplayProc_AD(void);
	static void TurnUp(void);
	static void TurnDown(void);
	static void HdlKeyProc_AD(void);
	static void ActivateADConst(void);
	void    InitADConst(void);
	static void    AdjustContrast(void);
	//振铃音
	static void Handler_KEY_ACK_Ring(void);
	static void Handler_KEY_NAK_Ring(void);
	static void Handler_KEY_SEL_Ring(void);
	static INT16U MaxRecordProc_Ring(void);
	static INT16U InitRecordProc_Ring(void);
	static INT16U ReadRecordProc_Ring(INT16S numrec, INT8U *ptr);
	static void SelRing(void);
	//设置按键音
	static INT16U InitRecordProc_KeyPT(void);
	static void StoreProc_KeyPT(void);
	static void ActivateSelOptionMenu(void);
	//设置接听方式
	static INT16U InitRecordProc_Pick(void);
	static void StoreProc_Pick(void);
	static void ActivateSelOptionMenu_Pick(void);
	//设置缺省模式
	static INT16U InitRecordProc_Cha(void);
	static void StoreProc_Cha(void);
	static void ActivateSelOptionMenu_Cha(void);

/****************ParaChk********************************************************/
	static void GetCenterNo(void);
	static void GetGPRSStatus(void);
	static void GetGPSStatus(void);

/*****************AtmpMsg*******************************************************/
	static void AddATMPRec(ATMPREC_STRUCT *atmprec);
	static INT16U GetATMPRec(INT16U numrec, ATMPREC_STRUCT *ptr);
	static INT16U GetATMPRecItem(void);
	static INT16U MaxRecordProc_CONTENT_Atmp(void);
	static INT16U ReadRecordProc_CONTENT_Atmp(INT16S numrec, INT8U *ptr);
	static void Handler_KEY_ACK_Atmp(void);
	static INT16U MaxRecordProc_Atmp(void);
	static INT16U ReadRecordProc_Atmp(INT16S numrec, INT8U *ptr);
	static void ReadATMP(void);

/*****************PhoneCallMsg*******************************************************/
//	static void AddPCRec(ATMPREC_STRUCT *atmprec);
//	static INT16U GetPCRec(INT16U numrec, ATMPREC_STRUCT *ptr);
//	static INT16U GetPCRecItem(void);
	static INT16U MaxRecordProc_CONTENT_PC(void);
	static INT16U ReadRecordProc_CONTENT_PC(INT16S numrec, INT8U *ptr);
	static void Handler_KEY_ACK_PC(void);
	static INT16U MaxRecordProc_PC(void);
	static INT16U ReadRecordProc_PC(INT16S numrec, INT8U *ptr);
	static void ReadPC(void);

/*****************Oil*******************************************************/
	static void SetQuerterBDOFF();
	static void SetHalfBDOFF();
	static void SetThreeFourBDOFF();
	static void SetFullBDOFF();
	static void SetZeroBDOFF();
	static void SetFullBD();
	static void SetThreeFourBD();
	static void SetHalfBD();
	static void SetQuerterBD();
	static void SetZeroBD();
	static void SndBDData();
	static void BiaoDEnd();
	static void BiaoDStart();
	static void SndOilDataDesire();
	
/*****************VideoView*******************************************************/
	static void SwhChn1View();
	static void SwhChn2View();
	static void SwhChn3View();
	static void SwhChn4View();
	static void SwhChnAllView();

/*****************ProjectMenu*******************************************************/
	static void UpdateApp();
	static void ResumeSet();
	static void FormatSDisk();
	static void UpdateSys();
	static void TestOilCut();
	static void InputVideo();
	static void OutputUData();
	static void PCSoundTest();
	static void TestCAM1();
	static void TestCAM2();
	static void TestCAM3();
	static void TestCAM4();
	static void ChkSensorState();
	static void TestAlarm();
	static void ChkLED();
	static void ChkGPS();

	
	static void Settemiver();
	static void ACK_Settemiver();
	static void Setcompanyid();
	static void ACK_Setcompanyid();
	static void Setgroupid();
	static void ACK_Setgroupid();
	static void Set_CGV_PARA(INT8U type, INT8U len, INT8U *ptr);
	static void Loginliu();

	static void InputPasswordToProject();
	static void ACK_InputPasswordProject();

public:	//AuxFunc
	static void DelTBRecByName(char *v_pName);
	static int CheckNameExist(byte v_byLen,char *v_pName);
	void    ActivateVirPhone_Ring(void);

	static INT8U HexToChar(INT8U sbyte);
	static INT8U DecToAscii(INT8U *dptr, INT16U data, INT8U reflen);
	static INT8U FormatYear(INT8U *ptr, INT8U year, INT8U month, INT8U day);
	static INT8U FormatTime(INT8U *ptr, INT8U hour, INT8U minute, INT8U second);
	static bool SearchGBCode(INT8U *ptr, INT16U len);
	static INT8U ASCIIToGsmCode(INT8U incode);
	static INT16U TestGBLen(INT8U *ptr, INT16U len);
};


#endif

