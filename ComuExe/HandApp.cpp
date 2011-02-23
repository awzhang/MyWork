#include "ComuStdAfx.h"

#if CHK_VER == 1
	#define ALL_PASS	""				//调度屏通用密码
	#define LIU_PASS	"4312896654"	//流媒体参数配置密码
	#define EXP_PASS	"1122334455"	//调试模式密码(工程菜单输入此密码即进入调试模式)
#else 
	#define ALL_PASS	"4312897765"	//调度屏通用密码
	#define LIU_PASS	"4312896654"	//流媒体参数配置密码
	#define EXP_PASS	"1122334455"	//调试模式密码(工程菜单输入此密码即进入调试模式)
#endif	

#define build_frm2(a, b, c, d)	if(a)	{ sprintf(frm+len, "%s:%s,", b, c); len+=strlen(b)+strlen(c)+2; }  \
								else	{ sprintf(frm+len, "%s:%s,", b, d); len+=strlen(b)+strlen(d)+2; }
#define build_frm(a, b, c)	if(a)	{ strcpy(frm+len, b); len+=strlen(b); }  \
							else	{ strcpy(frm+len, c); len+=strlen(c); }


static INT8U asciitime[] = {"18:00:00"};
static INT8U asciidate[] = {"2004.01.01"};

static INT8U dplytime_phone[] = {"通话结束\n  00:00:00"};

static char dplytime[] = {"共通话:\n  00:00:00\n时间:00:00\n2003.01.01"};

static INT8U GSMCODE[] = { '@', ' ', '$', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x0A,' ', ' ', 0x0D,' ', ' ',
                           '_', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                           ' ', '!', '"', '#', ' ', '%', '&', '\'','(', ')', '*', '+', ',', '-', '.', '/',
                           '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
                           ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
                           'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' ', ' ', ' ', ' ', ' ',
                           '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
                           'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' ', ' ', ' ', ' ', ' '
                         };
static char *CallRing[]                 = {"一般铃声", "笑傲江湖", "我的中国心", 
										   "康定情歌", "外婆的澎湖湾", "北京的金山上"};
static char *SMRing[]                   = {"铃声1", "铃声2", "铃声3",
											   "铃声4", "铃声5"};

	//初始化静态变量
INT8U	CHstApp::m_hangupstate;
bool	CHstApp::m_hstmode;
bool	CHstApp::m_KBIsLocked;
bool	CHstApp::m_WaitKEY_Star;
INT8U	CHstApp::m_TelEditBuf[SIZE_TELEDITBUF];
INT8U	CHstApp::m_status_phone;
INT8U   CHstApp::m_dtmfbuf[SIZE_DTMFBUF];
INT8U	CHstApp::m_dtmflen;
INT8U	CHstApp::m_dtmfpos;
INT8U	CHstApp::m_dplybuf_phone[SIZE_DPLYBUF];
INT8U	CHstApp::m_dplyptr_phone[SIZE_DPLYBUF];
INT8U	CHstApp::m_dplylen_phone;
INT8U	CHstApp::m_talktype;
INT16S	CHstApp::m_selrecord_tb;
INT8U	CHstApp::m_op_type_tb;
char	CHstApp::m_dplybuf[SIZE_DPLYBUF];
INT16S	CHstApp::m_selrecord_sm;
INT8U	CHstApp::m_storebuf[300];
INT8U	CHstApp::m_recvbuf[SIZE_RECVBUF];
INT8U	CHstApp::m_curvalue;
INT8U	CHstApp::m_optype;
ulong	CHstApp::m_hsttm;
INT8U	CHstApp::m_helptype;
//bool	CHstApp::m_flagdz;
bool	CHstApp::m_IsRefuseDial;
ulong	CHstApp::m_diatm;
bool	CHstApp::m_IsReturnMainWin;
INT8U	CHstApp::m_state_tel;
INT8U	CHstApp::m_dianzhaostate;
bool	CHstApp::m_IsPDkey;
int		CHstApp::m_testalarmsta;
int		CHstApp::m_msgsta;
int		CHstApp::m_pickupsta;
int		CHstApp::m_gpsdect;
int		CHstApp::m_gprssta;
//HXD 090428
BOOL    CHstApp::m_bDrvHaveLog;
tag2QHstCfg	CHstApp::m_HstCfg;

KEY_STRUCT CHstApp::m_curkey;
ITEMMENU_STRUCT	CHstApp::MENU_MAIN;
CHstApp::ENTRYTEL_STRUCT CHstApp::EntryTel;
PROMPT_STRUCT	CHstApp::PromptEntry_Phone;
WINSTACKENTRY_STRUCT	CHstApp::WinStackEntry_Dia;
WINSTACKENTRY_STRUCT	CHstApp::WinStackEntry_DialSuccor;
WINSTACKENTRY_STRUCT	CHstApp::WinStackEntry_Ring;
CHstApp::HSTMISS_STRUCT	CHstApp::HstMiss;
MENUWIN_STRUCT	CHstApp::SelMenuWinEntry;
CHstApp::TALKREC_STRUCT	CHstApp::CurRecord;
CHstApp::TBREC_STRUCT	CHstApp::currecord;
CHstApp::TBREC_STRUCT	CHstApp::readrecord;
MENUWIN_STRUCT	CHstApp::ListTBEntry;
CHstApp::SMREC_STRUCT	CHstApp::CurRecord_SM;
CHstApp::SMREC_STRUCT	CHstApp::EditRecord;
EDITOR_STRUCT	CHstApp::TelEditorEntry;
CHstApp::SETGPRS_STRUCT	CHstApp::set_gprs;
WINSTACKENTRY_STRUCT	CHstApp::WinStackEntry_AD;
CHstApp::ATMPREC_STRUCT	CHstApp::CurRec_Atmp;
CHstApp::ATMPREC_STRUCT	CHstApp::CurRec_PC;
CHstApp::ATMPREC_STRUCT  CHstApp::m_tagDiaoduMsg;//cyh add
int g_iNeedPhoneTip = 0;  //cyh add:add 标记是否要显示 通话结束 这个Prompt
EDITOR_STRUCT	CHstApp::EditorEntry;
PROMPT_STRUCT	CHstApp::ParaPrompt;
PROMPT_STRUCT	CHstApp::PromptEntry_CM;

WINSTACKENTRY_STRUCT CHstApp::WinStackEntry_MainWin;
CHstApp::FUNCENTRY_STRUCT CHstApp::Handler_Key_entry[7];

ITEMMENU_STRUCT CHstApp::MENU_SET;
ITEMMENU_STRUCT CHstApp::MENU_SELOPTION;
ITEMMENU_STRUCT	CHstApp::MENU_SELPICK;
ITEMMENU_STRUCT	CHstApp::MENU_SELCHA;
ITEMMENU_STRUCT	CHstApp::MENU_OPSM;
ITEMMENU_STRUCT	CHstApp::MENU_PROJECT;
ITEMMENU_STRUCT CHstApp::MENU_OPREC_CM;
ITEMMENU_STRUCT CHstApp::MENU_OPREC_TB;
ITEMMENU_STRUCT CHstApp::MENU_SETL;
MENUWIN_STRUCT	CHstApp::MenuWinEntry;
MENUWIN_STRUCT	CHstApp::MENU_LISTSM;
MENUWIN_STRUCT	CHstApp::MENU_LISTATMP;
MENUWIN_STRUCT	CHstApp::MENU_READCONTENT;
MENUWIN_STRUCT	CHstApp::MENU_LISTPC;
MENUWIN_STRUCT	CHstApp::MENU_READCONTENT_PC;

PROMPT_STRUCT	CHstApp::ProjectPrompt;
EDITOR_STRUCT	CHstApp::EditorEntry_MainWin;

RETINFORMER CHstApp::RetInformer;

int g_iIndexSelRec = -1;  //cyh add:add 修改电话本时，保存当前选中的记录的索引
int g_iIndexRptRec = -1;  //cyh add:add 修改电话本时，保存和选中记录重名的记录的索引好

void *G_ThreadHandleWork(void *arg)
{
	g_hstapp.P_ThreadHandleWork();
}

void G_TmDrvRec(void *arg, int len)
{
	g_hstapp.P_TmDrvRec();
}

void G_TmIsOnLine(void *arg, int len)
{
	g_hstapp.P_TmIsOnLine();
}

#undef MSG_HEAD
#define MSG_HEAD ("Comu-HandApp")

//////////////////////////////////////////////////////////////////////////

CHstApp::CHstApp()
{
	m_bDrvHaveLog = TRUE;
	m_hangupstate = 0;

	m_KBIsLocked = false;
	m_WaitKEY_Star = false;
	memset(m_TelEditBuf, 0, SIZE_TELEDITBUF);
	m_status_phone = 0;
	memset(m_dtmfbuf, 0, SIZE_DTMFBUF);	//DTMF缓存,如输入的分机号等
	m_dtmflen = 0;
	m_dtmfpos = 0;
	memset(m_dplybuf_phone, 0, SIZE_DPLYBUF);
	memset(m_dplyptr_phone, 0, SIZE_DPLYBUF);
	m_dplylen_phone = 0;
	m_talktype = 0xff;
	m_selrecord_tb = 0;
	m_op_type_tb = 0xff;
	memset(m_dplybuf, 0, SIZE_DPLYBUF);
	m_selrecord_sm = 0;
	memset(m_storebuf, 0, 300);
	m_curvalue = INIT_STEP;			//对比度当前值
	m_optype = 0;
	memset(m_recvbuf, 0 ,SIZE_RECVBUF);
	m_helptype = 0xff;		//求助类型

	m_IsRefuseDial = true;
	m_DialCount = 0;
	m_diatm = 0;
	m_IsReturnMainWin = true;
	memset(&m_curkey, 0, sizeof(m_curkey));
	m_talktm = 0;
	m_calltm = 0;
	memset(m_diasucctm, 0, 20);
	memset(m_recvtm, 0, 20);

	m_state_tel = 0;
	m_talktype = 0xff;  //cyh add
	m_state_tel = 0xff;  //cyh add
	memset(&m_calllog, 0, sizeof(m_calllog));
	memset(m_strcalltm, 0, 10);
	m_dianzhaostate = 0xff;
	m_dianzhaotm = 0;
	m_qiangdatm = 0;
	m_qiangdasucctm = 0;
	m_IsPDkey = false;
	memset(&HstMiss, 0, sizeof(HstMiss));
	m_testalarmsta = 0;
	m_msgsta = 0;
	m_pickupsta = 0;
	m_gpsdect = 0;
	m_gprssta = 0;

	Init_Menu();	
}

CHstApp::~CHstApp()
{

}

int CHstApp::Init()
{
	//取出手柄配置区参数
	tag2QHstCfg obj2QHstCfg;
	
	GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg), offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );
	m_HstCfg.m_bytcallring = obj2QHstCfg.m_bytcallring;
	m_HstCfg.m_bytsmring = obj2QHstCfg.m_bytsmring;
	m_HstCfg.m_HandsfreeChannel = obj2QHstCfg.m_HandsfreeChannel;
	m_HstCfg.m_EnableKeyPrompt = obj2QHstCfg.m_EnableKeyPrompt;
	m_HstCfg.m_EnableAutoPickup = obj2QHstCfg.m_EnableAutoPickup;
	
	memcpy( m_HstCfg.m_szListenTel, obj2QHstCfg.m_szListenTel, strlen(obj2QHstCfg.m_szListenTel) ); //cyh add:add
	m_hstmode = obj2QHstCfg.m_HandsfreeChannel;
	
	if (m_HstCfg.m_EnableKeyPrompt)
	{
		g_hst.CtlKeyPrompt((char)0x80, 1);
	}
	else
	{
		g_hst.CtlKeyPrompt((char)0x00, 1);
	}

	//初始化存储文件
	FILE* fp;
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	fp = fopen(path, "rb");
	
	//如果文件不存在则创建
	if(fp == NULL) 
	{	
		fp = fopen(path, "wb+");

		if (fp != NULL) 
		{
			fwrite(0, 8192*2, 1, fp);		//存储空间大小为11000byte=10.7K
			fclose(fp);
		}
	}
	else
	{
		fclose(fp);
	}
		
	m_hststate = PowerOn;
	m_hsttm = GetTickCount();
		
	if( pthread_create(&m_pthreadHandleWork, NULL, G_ThreadHandleWork, NULL) != 0 )
	{
		PRTMSG(MSG_ERR, "create handle app work failed!\n");
		perror("");
		return ERR_THREAD;
	}
	
	return 0;
}

int CHstApp::Release()
{

}

void CHstApp::P_ThreadHandleWork()
{
	byte  bytLvl = 0;
	char  szRecvBuf[256] = {0};
	DWORD dwRecvLen = 0;
	DWORD dwPushTm;
	DWORD dwBufSize = (DWORD)sizeof(szRecvBuf);

	static int  ring_sta = 0;	//电话模块在收到来电后会不断发送振铃指示应答给手柄模块

	static ulong sta_ulInfoTm = 0;	// 信息更新时刻（指升级提示信息、GPS监测信息等）

	while( !g_bProgExit )
	{
		usleep(20000);

		switch(m_hststate) 
		{
		case PowerOn:
			{
				g_hst.TurnonHst();
				InitMainWin();                          /* init mainwin */
				AdjustContrast();                       /* adjust handset constrast */
				g_hst.DestroyWinStack();

				ActivateMainWin();                      /* activate mainwin */

				DisplayScreen();
				m_hststate = Idle;
			}
			break;

		case Idle:
			{
				if (m_curkey.key == KEY_PWR_ON_) //如果手柄重新上电
				{		
					m_hststate = PowerOff;
				}
			}	
			break;

		case PowerOff:		//当手柄被断电后,再次上电时捕获到'm'则重新启动流程
			{
				sleep(3);

				g_hst.TurnoffHst();
				g_hst.DestroyWinStack();

				// 为何要检测ACC状态，那熄火时，手柄岂不是不能用了？
// 				unsigned char ucAccSta;
// 				IOGet(IOS_ACC, &ucAccSta);
// 				if( ucAccSta != IO_ACC_ON)	
// 				{
// 					//此处要做手柄断电操作，但似乎没必要，省略
// 					break;
// 				}
// 				else
// 				{
// 					//此处要做手柄上电操作，但似乎没必要，省略
// 					m_hststate = PowerOn;  //cyh add: mod  但是似乎有时候在手柄正常工作时会收到"mm"，如果是这样，就不能这样改了，待测试
// 				}

				// 调整一下，不检测ACC状态，手柄直接上电
				m_hststate = PowerOn;
			}
			break;

		default:
			break;
		}

		// 若当前不是使用手柄，则continue
		if (g_iTerminalType != 1)
			continue;

		// 处理应答信息
		if( !g_objDiaoduOrHandWorkMng.PopData(bytLvl, dwBufSize, dwRecvLen, szRecvBuf, dwPushTm))
		{
			switch( (byte)szRecvBuf[0] ) 
			{
				case 0x00: //油量数据应答
				{
					tag1QOilBiaodingCfg cfg;
					char str[100] = {0};
					GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
					sprintf(str, "%sAD:%u,%sAD:%u", LANG_IO_OIL, cfg.m_aryONOilBiaoding[0][0], LANG_IO_OILPOW,cfg.m_aryONOilBiaoding[0][1]);
#endif
					g_hst.ActivateStringPrompt_TIME((char *)str,5);
				}
				break;


				case 0x14:
				{
					tag1QOilBiaodingCfg cfg;
					char str[100] = {0};
					GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
					sprintf(str, "%sAD:%u,%sAD:%u", LANG_IO_OIL, cfg.m_aryONOilBiaoding[1][0], LANG_IO_OILPOW,cfg.m_aryONOilBiaoding[1][1]);
#endif
					g_hst.ActivateStringPrompt_TIME((char *)str,5);
				}
				break;

				case 0x12:
				{
					tag1QOilBiaodingCfg cfg;
					char str[100] = {0};
					GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
					sprintf(str, "%sAD:%u,%sAD:%u", LANG_IO_OIL, cfg.m_aryONOilBiaoding[2][0], LANG_IO_OILPOW,cfg.m_aryONOilBiaoding[2][1]);
#endif
					g_hst.ActivateStringPrompt_TIME((char *)str,5);
				}
				break;

				case 0x34:
				{
					tag1QOilBiaodingCfg cfg;
					char str[100] = {0};
					GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
					sprintf(str, "%sAD:%u,%sAD:%u", LANG_IO_OIL, cfg.m_aryONOilBiaoding[3][0], LANG_IO_OILPOW,cfg.m_aryONOilBiaoding[3][1]);
#endif
					g_hst.ActivateStringPrompt_TIME((char *)str,5);
				}
				break;

			case 0x44:
				{
					tag1QOilBiaodingCfg cfg;
					char str[100] = {0};
					GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
					sprintf(str, "%sAD:%u,%sAD:%u", LANG_IO_OIL, cfg.m_aryONOilBiaoding[4][0],LANG_IO_OILPOW,cfg.m_aryONOilBiaoding[4][1]);
#endif
					g_hst.ActivateStringPrompt_TIME((char *)str,5);			
				}
				break;

			case 0xF0:
				{
					if(0x01 == szRecvBuf[1])
					{
						unsigned short *oilAD = (unsigned short *)&szRecvBuf[2];
						unsigned short *pwAD = (unsigned short *)&szRecvBuf[4];
						tag1QOilBiaodingCfg cfg;
						char str[100] = {0};

						GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
						sprintf(str, "当前值:%u,%u;0油耗AD:%u,0电压AD:%u;1/4油耗AD:%u,1/4电压AD:%u;1/2油耗AD:%u,1/2电压AD:%u;3/4油耗AD:%u,3/4电压AD:%u;满油耗AD:%u,满电压AD:%u",
							*oilAD,*pwAD,
							cfg.m_aryONOilBiaoding[0][0],cfg.m_aryONOilBiaoding[0][1],
							cfg.m_aryONOilBiaoding[1][0],cfg.m_aryONOilBiaoding[1][1],
							cfg.m_aryONOilBiaoding[2][0],cfg.m_aryONOilBiaoding[2][1],
							cfg.m_aryONOilBiaoding[3][0],cfg.m_aryONOilBiaoding[3][1],
							cfg.m_aryONOilBiaoding[4][0],cfg.m_aryONOilBiaoding[4][1]
							);
#endif
						g_hst.ActivateStringPrompt_TIME((char *)str,30);
					}
					else
					{
						g_hst.ActivateStringPrompt_TIME("请求失败!",100);
					}
				}
				break;

			case 0xF1:
				{
					tag1QOilBiaodingCfg cfg;
					char str[100] = {0};
					GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
					sprintf(str, "%sAD:%u,%sAD:%u", LANG_IO_OIL, cfg.m_aryOFFOilBiaoding[0][0],	LANG_IO_OILPOW,cfg.m_aryOFFOilBiaoding[0][1]);
#endif
					g_hst.ActivateStringPrompt_TIME((char *)str,5);
				}
				break;

			case 0xF2:
				{
					tag1QOilBiaodingCfg cfg;
					char str[100] = {0};
					GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
					sprintf(str, "%sAD:%u,%sAD:%u", LANG_IO_OIL, cfg.m_aryOFFOilBiaoding[1][0],	LANG_IO_OILPOW,cfg.m_aryOFFOilBiaoding[1][1]);
#endif
					g_hst.ActivateStringPrompt_TIME((char *)str,5);
				}
				break;
				
			case 0xF3:
				{
					tag1QOilBiaodingCfg cfg;
					char str[100] = {0};
					GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
					sprintf(str, "%sAD:%u,%sAD:%u", LANG_IO_OIL, cfg.m_aryOFFOilBiaoding[2][0],	LANG_IO_OILPOW,cfg.m_aryOFFOilBiaoding[2][1]);
#endif
					g_hst.ActivateStringPrompt_TIME((char *)str,5);
				}
				break;

			case 0xF4:
				{
					tag1QOilBiaodingCfg cfg;
					char str[100] = {0};
					GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
					sprintf(str, "%sAD:%u,%sAD:%u", LANG_IO_OIL, cfg.m_aryOFFOilBiaoding[3][0],	LANG_IO_OILPOW,cfg.m_aryOFFOilBiaoding[3][1]);
#endif
					g_hst.ActivateStringPrompt_TIME((char *)str,5);
				}
				break;

			case 0xF5:
				{
					tag1QOilBiaodingCfg cfg;
					char str[100] = {0};
					GetImpCfg(&cfg,sizeof(cfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(cfg));
#if USE_OIL == 3
					sprintf(str, "%sAD:%u,%sAD:%u", LANG_IO_OIL, cfg.m_aryOFFOilBiaoding[4][0],	LANG_IO_OILPOW,cfg.m_aryOFFOilBiaoding[4][1]);
#endif
					g_hst.ActivateStringPrompt_TIME((char *)str,5);
				}
				break;

			case 0xF6:
				{
					g_hst.ActivateStringPrompt_TIME("标定失败!",3);		
				}
				break;
			
			case 0xC1:
				{
					if( 0x01 == szRecvBuf[1] )
					{
						g_hst.ActivateStringPrompt_TIME("发送成功!",100);
					}
					else
					{
						g_hst.ActivateStringPrompt_TIME("发送失败!",100);
					}
				}
				break;

			case 0xC3:	// 标定开始指示
				{
					char buf[100] ={0};
					if( szRecvBuf[1] == 0x01 )
					{
						g_hst.ActivateStringPrompt_TIME("指示成功!",3);
					}
					else
					{
						g_hst.ActivateStringPrompt_TIME("指示失败!",3);
					}
				}
				break;

			case 0xC4:	//标定完成指示
				{
					char buf[100] ={0};
					if( szRecvBuf[1] == 0x01 )
					{
						g_hst.ActivateStringPrompt_TIME("指示成功!",3);
					}
					else
					{
						g_hst.ActivateStringPrompt_TIME("指示失败!",3);
					}
				}
				break;

			case 0x04:	
				{
					if( 0x40 == szRecvBuf[1] )
					{
						g_hst.TurnonArrowIndicator();
					}
					else
					{
						g_hst.TurnoffArrowIndicator();
					}
				}
				break;

			case 0x0c:
				{
					if (1 == szRecvBuf[1])
					{
						if (m_hststate == Idle) 
							m_hststate = PowerOff;
					}
				}
				break;
				
			case 0x62:
				{
					_SetTimer(&g_objTimerMng, DRV_LOG_TIMER, 3000, G_TmDrvRec);

					m_bDrvHaveLog = FALSE;
					InputDrvLog();
				}
				break;

			case 0x6c: // 090901 xun add,取消司机登陆
				{
					_KillTimer(&g_objTimerMng, DRV_LOG_TIMER);
				}
				break;

			case 0xf8: // 超时了
				{
					_KillTimer(&g_objTimerMng, DRV_LOG_TIMER);

					m_bDrvHaveLog = FALSE;
					g_hst.ActivateLowestWinStack();
				}
				break;

			case 0xf9:
				{
					if (1 == szRecvBuf[1]) 
					{
						PRTMSG(MSG_NOR, "0xf9 TurnonHst\n");
					}
					if (0 == szRecvBuf[1])
					{
						PRTMSG(MSG_NOR, "0xf9 TurnoffHst\n");
					}
				}
				break;
				
			case 0x11:	//DTMF应答
				{
					if (szRecvBuf[1] == 0x01) 
					{	//应答成功
						Display_TALKING();	//显示字符
					}
					else
					{
						PRTMSG(MSG_NOR, "send dtmf err:%d\n",szRecvBuf[1]);
					}
				}
				break;

			case 0x13:	//拨号请求应答
				{
					switch(szRecvBuf[1])
					{
					case 0x01:	//拨号成功
						{
							m_status_phone = VIR_CALLING;

							struct tm pCurrentTime;
							G_GetTime(&pCurrentTime);

							sprintf(m_diasucctm, "%04d.%02d.%02d", pCurrentTime.tm_year+1900, pCurrentTime.tm_mon+1, pCurrentTime.tm_mday);
							sprintf(m_diasucctm+10, "%02d:%02d:%02d", pCurrentTime.tm_hour, pCurrentTime.tm_min, pCurrentTime.tm_sec);
						}
						break;

					case 0x02:	//手机忙
						{
							ExitVirPhone("对不起\n现正忙…", 3);
						}
						break;

					case 0x03:	//SIM卡无效
						ExitVirPhone("  无SIM卡", 3);
						break;

					case 0x04:	//没有搜索到网络
						ExitVirPhone("   无网络", 3);
						break;

					case 0x05:	//被呼叫限制的号码
						ExitVirPhone("对不起\n呼叫被限制", 3);
  						break;

					default:
						break;
					}
					m_talktype = TALK_DIAL;
				}
				break;

			case 0x15:	//摘机应答
				{
					if (szRecvBuf[1] == 0x01) 
					{
						m_status_phone = VIR_TALKING;
						Display_TALKING();
						m_state_tel = 3;
						m_talktm = GetTickCount();
					}
				}
				break;

			case 0x17:	//挂机请求应答
				{
					char dply[50] = {0};
					
					m_hangupstate = 1;
					ring_sta = 0;
					m_pickupsta = 0;
					
					if ((m_hstmode && !m_HstCfg.m_HandsfreeChannel) || (!m_hstmode && m_HstCfg.m_HandsfreeChannel))
					{	
						m_HstCfg.m_HandsfreeChannel = !m_HstCfg.m_HandsfreeChannel;
					}
					
					g_hst.TurnoffTalkIndicator();
					g_hst.CancelRing();
					
					m_diatm = 0;	//挂机后通话起点时刻置为0
					m_IsReturnMainWin = true;
					m_status_phone = VIR_FREE;
					
					if (m_state_tel==0xff)//cyh add:为了避免没有拨打电话出现的通话结束的提示
					{
						PRTMSG(MSG_ERR, "state_tel err:%d\n",m_state_tel);
					}
					
					if (m_state_tel == 2 || m_state_tel == 3) //如果拨打电话成功
					{
						m_calltm = GetTickCount() - m_talktm;
						PRTMSG(MSG_NOR, "Is pick up\n");
					}
					
					if (m_state_tel == 1) //如果是未接来电--响铃不摘机
					{
						m_talktype = TALK_MISS;
						PRTMSG(MSG_NOR, "Is mis tel\n");
					}
					
					sprintf(m_strcalltm, "%02d:%02d:%02d", m_calltm/CON_COE, (m_calltm/1000)/60, (m_calltm/1000)%60);
					PRTMSG(MSG_NOR, "%s\n", m_strcalltm);
					
					if (1==m_state_tel)  //cyh add: 如果是未接来电
					{
						sprintf(dply, "未接来电\n  ");
						ExitVirPhone(dply, 4);
					}
					else
					{
						if (g_iNeedPhoneTip)
						{
							sprintf(dply, "通话结束\n  %02d:%02d:%02d",	m_calltm/CON_COE, (m_calltm/1000)/60, (m_calltm/1000)%60);
							ExitVirPhone(dply, 3);
						}
						else
						{
							sleep(2);
							g_hst.ActivateLowestWinStack();
						}
					}
					
					switch(m_talktype) 
					{
					case TALK_DIAL:		//保存通话记录
						{
							m_calllog.tellen = EntryTel.tellen;
							memcpy(m_calllog.tel, EntryTel.tel, TALKREC_SIZE_TEL);	//存入所拨号码
							memcpy(m_calllog.talktime, m_strcalltm, 10);
							memcpy(m_calllog.date, m_diasucctm, 10);
							memcpy(m_calllog.time, m_diasucctm+10, 10);
						}
						break;
						
					case TALK_RECV:
						{
							m_calllog.tellen = EntryTel.tellen;
							memcpy(m_calllog.tel, EntryTel.tel, TALKREC_SIZE_TEL);	//存入来电号码
							memcpy(m_calllog.talktime, m_strcalltm, 10);
							memcpy(m_calllog.date, m_recvtm, 10);
							memcpy(m_calllog.time, m_recvtm+10, 10);
						}
						break;
						
					case TALK_MISS:
						{
							m_calllog.tellen = EntryTel.tellen;
							memcpy(m_calllog.tel, EntryTel.tel, TALKREC_SIZE_TEL);	//存入未接号码
							memcpy(m_calllog.talktime, m_strcalltm, 10);
							memcpy(m_calllog.date, m_recvtm, 10);
							memcpy(m_calllog.time, m_recvtm+10, 10);
						}
						break;
						
					default:
						break;
					}
					
					m_state_tel = 0;
					memset(m_strcalltm, 0, 10);
					m_calltm = 0;
					SaveTKToFile(m_talktype, &m_calllog);
					
					m_talktype = 0xff;
					m_state_tel = 0xff;
					g_iNeedPhoneTip = 0;
				}
				break;

			case 0x19:	//振铃指示应答
				{
					if (ring_sta == 0) 
					{
						m_tempbuf[0] = szRecvBuf[1];	//电话号码长度
						memcpy(m_tempbuf+1, szRecvBuf+2, m_tempbuf[0]);	//电话号码

						ActivateVirPhone_Ring();
						m_talktype = TALK_RECV;
						m_state_tel = 1;

						struct tm pCurrentTime;
						G_GetTime(&pCurrentTime);
						
						sprintf(m_recvtm, "%04d.%02d.%02d", pCurrentTime.tm_year+1900, pCurrentTime.tm_mon+1, pCurrentTime.tm_mday);
						sprintf(m_recvtm+10, "%02d:%02d:%02d", pCurrentTime.tm_hour, pCurrentTime.tm_min, pCurrentTime.tm_sec);
	
						ring_sta = 1;
					}
				}
				break;

			case 0x21:	//接通指示
				{
					m_status_phone = VIR_TALKING;

					InitDtmfBuf();
					Display_TALKING();
					m_talktm = GetTickCount();
					m_state_tel = 2;

					g_hst.CancelRing();
				}
				break;

			case 0x23:	//空闲指示
				{	
					m_status_phone = VIR_FREE;
					m_hangupstate = 0;	//空闲时挂机状态恢复
				}
				break;

			case 0x25:	//信号强度请求应答
				{
					g_hst.CtlSignalIndicator(szRecvBuf[1]);	//显示手机信号
				}
				break;

			case 0x27:	//免提耳机切换请求应答
				{
					if (szRecvBuf[1] == 0x01)
					{
						Display_CHANNEL();	//显示切换状态
					}
				}
				break;

			case 0x29:	//音量调节请求应答
				{
					if (szRecvBuf[1] == 0x01) 
					{
						Display_VOLUME();	//显示音量标识
					}
				}
				break;

			case 0x31:	//发送短信应答
				{
					if (1 == szRecvBuf[1])
					{
						g_hst.ActivateStringPrompt("短信发送成功!");
					}
					if (2 == szRecvBuf[1])
					{
						g_hst.ActivateStringPrompt("短信发送失败!");
					}
				}
				break;

			case 0x33:	//收到短消息
				{
					m_IsReturnMainWin = false;

					byte byTelLen = 0;
					byte byMsgLen = 0;

		 			m_smrec.unread = 1;
					byTelLen = szRecvBuf[1];
					m_smrec.tellen = byTelLen;
					memcpy(m_smrec.tel,szRecvBuf+2,byTelLen);
					memcpy(m_smrec.time,szRecvBuf+2+byTelLen,12);
					memcpy(&byMsgLen,szRecvBuf+14+byTelLen,1);
					m_smrec.datalen = byMsgLen;
					memcpy(m_smrec.data,szRecvBuf+15+byTelLen,byMsgLen);
					AddSMRec(&m_smrec);
					g_hst.RequestRing(m_HstCfg.m_bytsmring, 1);
					g_hst.TurnonSMIndicator();

					char szSmsTip[] = "收到短信";
					g_hst.ActivateStringPrompt_TIME_KEY(szSmsTip, 3, g_hst.CancelRing);
				}
				
				break;
			case 0x42:	//车辆维护应答
				{
					if (szRecvBuf[1] == 0x01)		//启动报警测试应答
					{
						char buf[] = "报警测试中...";
						g_hst.ActivateStringPrompt_TIME(buf, 5);
					}
				}
				break;

			case 0x43:	//报警测试结果发送请求
				{
					char buf_prompt[50] = {0};

					if (szRecvBuf[1] == 0x01)		//报警测试成功
					{
						PRTMSG(MSG_NOR, "Test Alarm succ!\n");

						memcpy(buf_prompt, "测试成功，报警功能正常\n", 24);
						m_testalarmsta = 1;
					}
					else
					{
						memcpy(buf_prompt, "测试失败，请重新测试", 20);
						m_testalarmsta = 2;
					}

					g_hst.ActivateStringPrompt_TIME_KEY(buf_prompt, 0, g_hst.CancelRing); 
					g_hst.RequestRing(0x01, 1);		//响铃提示
				}
				break;

// 			case 0x47:	//博海LED屏检测应答
// 				{
// 					char buf[100] = {0};
// 					switch(cmd[2])
// 					{
// 					case 0x01:		//LED校时
// 						{
// 							if (cmd[3] == 0x01)			{	memcpy(buf, "校时成功", 8);	}
// 							else if (cmd[3] == 0x02)	{	memcpy(buf, "校时失败", 8);	}
// 						}
// 						break;
// 					case 0x02:		//发送即时信息
// 						{
// 							if (cmd[3] == 0x01)			{	memcpy(buf, "发送即时信息成功", 16);	}
// 							else if (cmd[3] == 0x02)	{	memcpy(buf, "发送即时信息失败", 16);	}
// 						}
// 						break;
// 					case 0x03:		//亮屏
// 						{
// 							if (cmd[3] == 0x01)			{	memcpy(buf, "亮屏成功", 8);	}
// 							else if (cmd[3] == 0x02)	{	memcpy(buf, "亮屏失败", 8);	}
// 						}
// 						break;
// 					case 0x04:		//黑屏
// 						{
// 							if (cmd[3] == 0x01)			{	memcpy(buf, "黑屏成功", 8);	}
// 							else if (cmd[3] == 0x02)	{	memcpy(buf, "黑屏失败", 8);	}
// 						}
// 						break;
// 					default:
// 						break;
// 					}
// 
// 					g_hst.ActivateStringPrompt_TIME_KEY(buf, 0, g_hst.CancelRing); 
// 					g_hst.RequestRing(0x01, 1);		//响铃提示
// 				}
// 				break;

			case 0x70:	//中心下来的调度信息
			case 0x71:
				{
					m_IsReturnMainWin = false;
					char buf_du[1024] = {0};
					
					memset(&m_tagDiaoduMsg,0x00,sizeof(m_tagDiaoduMsg));
					ushort buflen = 0;

					//中心和调度的交互需6转8
					buflen = Code6To8(szRecvBuf+1, buf_du, dwRecvLen-1, sizeof(buf_du) );

					switch(buf_du[6]) 
					{
					case 0x02:		//车辆指示
						{
							memcpy(m_tagDiaoduMsg.data, buf_du+10, buf_du[8]-1);
							m_tagDiaoduMsg.datalen = buf_du[8] - 1;
						}
						break;

					case 0x04:		//信息发布
						{
							memcpy(m_tagDiaoduMsg.data, buf_du+9, buf_du[8]);
							m_tagDiaoduMsg.datalen = buf_du[8];
						}
						break;

					default:
						break;
					}

					m_msgsta = 1;	//表示存储的是调度信息
					AddATMPRec(&m_tagDiaoduMsg); 
					
					AtmpPrompt.Mode        = 0;
					AtmpPrompt.LiveTime    = 30;
					//AtmpPrompt.TextPtr     = (INT8U *)m_atmprec.data;
					AtmpPrompt.TextPtr     = (INT8U *)m_tagDiaoduMsg.data;
					AtmpPrompt.TextLen     = m_tagDiaoduMsg.datalen;//buf_du[8]-1;
					AtmpPrompt.ExitProc    = g_hst.CancelRing;
					AtmpPrompt.ExitKeyProc = 0;
					g_hst.ActivatePrompt(&AtmpPrompt);
					g_hst.RequestRing(0x02, 1);			
				}
				break;

			case 0x72:	//显示提示信息
				{
					char buf[100] = {0};
					g_hst.RequestRing(0x05, 1);		//响铃提示

					switch(szRecvBuf[1]) 
					{
					case 0x01:	//普通提示
					case 0x02:	//警告信息
						{
							memcpy(buf, szRecvBuf+2, dwRecvLen-2);
							g_hst.ActivateStringPrompt_TIME(buf, 2); 
						
							sleep(1);

							g_hst.CancelRing();
						}
						break;
					default:
						break;
					}
					
				}
				break;

			case 0x74:	//电召指令(表示有电召任务)
				{//cyh add:caution  由于电召与调度信息共用一个存储结构，因此如果中心下发电召指令后，马上又下发了调度信息，则会出错					
					m_IsReturnMainWin = false;
					m_dianzhaotm = GetTickCount();

					PRTMSG(MSG_NOR, "Rcv dianzhao MSG\n");
					char dianz[] = "有电召任务,按OK抢答";

					m_dianzhaoprompt.Mode        = 0;
					m_dianzhaoprompt.LiveTime    = 20;
					m_dianzhaoprompt.TextPtr     = (INT8U *)dianz;
					m_dianzhaoprompt.TextLen     = 20;
					m_dianzhaoprompt.ExitProc    = g_hst.CancelRing;
					m_dianzhaoprompt.ExitKeyProc = HdlDianZhao;
					g_hst.ActivatePrompt(&m_dianzhaoprompt);
					g_hst.RequestRing(0x02, 1);		//响铃提示

					m_qiangdatm = GetTickCount();
					m_dianzhaostate = 0;
				}
				break;

			case 0x75:	//抢答确认信息:乘客电话(15)+预留(15)+性别(4)+描述信息(50)
				//该车台抢答到电召指令后,中心下发的电召信息给手柄
				{
					m_IsReturnMainWin = false;
					m_dianzhaostate = 2;	//抢答成功
					m_qiangdasucctm = GetTickCount();

					PRTMSG(MSG_NOR, "Rcv qiangda MSG\n");
					memcpy(&m_frm053d, szRecvBuf+1, dwRecvLen-1);
					
					char *c = strchr(m_frm053d.tel, ' ');
					*c = 0x00;
					m_frm053d.tel[sizeof(m_frm053d.tel)-1] = 0;
					
					m_frm053d.content[sizeof(m_frm053d.content)-1] = 0;
					c = strchr(m_frm053d.content, ' ');

					if(c)
					{
						*c = 0x00;
					}
					
					m_frm053d.car_id[sizeof(m_frm053d.car_id)-1] = 0;
					c = strchr(m_frm053d.car_id, ' ');

					if(c)
					{
						*c = 0x00;
					}

					char buf_dz[100] = {0};
					int len1 = 0;
					memset(buf_dz, 0, 100);
					memcpy(buf_dz, m_frm053d.tel, strlen(m_frm053d.tel));
					len1 += strlen(m_frm053d.tel);
					memcpy(buf_dz+len1, m_frm053d.content, strlen(m_frm053d.content));
					len1+= strlen(m_frm053d.content);
					memcpy(m_atmprec.data, buf_dz, len1);	//保存电召详细信息
					m_atmprec.datalen = len1;

					m_dianzhaoprompt.Mode        = 0;
					m_dianzhaoprompt.LiveTime    = 30;
					m_dianzhaoprompt.TextPtr     = (INT8U *)buf_dz;
					m_dianzhaoprompt.TextLen     = len1;
					m_dianzhaoprompt.ExitProc    = g_hst.CancelRing;
					m_dianzhaoprompt.ExitKeyProc = HdlQiangDa;
					g_hst.ActivatePrompt(&m_dianzhaoprompt);
					g_hst.RequestRing(0x03, 1);	
				}
				break;

			case 0x76:	//确认:对手柄抢答和电召结果的确认
				{
					m_IsReturnMainWin = false;
					m_qiangdatm = GetTickCount();

					char dianzhao[30] = {0};
					m_dianzhaoprompt.Mode        = 0;
					m_dianzhaoprompt.LiveTime    = 30;
					m_dianzhaoprompt.ExitProc    = g_hst.CancelRing;
					m_dianzhaoprompt.ExitKeyProc = 0;

					if (szRecvBuf[1] == 0x01) 
					{
 						if (m_dianzhaostate == 1)
 						{
							PRTMSG(MSG_NOR, "Qiangda...\n");
							memcpy(dianzhao, "抢答中,请稍候...", 20);
 						}
						else if (m_dianzhaostate == 3)
						{
							PRTMSG(MSG_NOR, "Dianzhao succ\n");
							memcpy(dianzhao, "电召成功,按OK键结束", 20);
							m_msgsta = 2;	//表示存储的是电召信息
							AddATMPRec(&m_atmprec);	
						}

						m_dianzhaoprompt.TextPtr     = (INT8U *)dianzhao;
						m_dianzhaoprompt.TextLen     = 20;
						g_hst.ActivatePrompt(&m_dianzhaoprompt);
						g_hst.RequestRing(0x04, 1);		//响铃提示
					}
					else
					{
						if (m_dianzhaostate == 3) 
						{
							PRTMSG(MSG_NOR, "Diaozhao Unsucc\n");
							memcpy(dianzhao, "电召失败,按OK键结束", 20);
							m_dianzhaoprompt.TextLen = 20;
						} 
						else if (m_dianzhaostate == 1)
						{
							PRTMSG(MSG_NOR, "Qiangda Unsucc\n");
							memcpy(dianzhao, "欢迎下次抢答,按OK键结束", 24);
							m_dianzhaoprompt.TextLen = 24;
						}	

						m_dianzhaoprompt.TextPtr = (INT8U *)dianzhao;
						g_hst.ActivatePrompt(&m_dianzhaoprompt);
						g_hst.RequestRing(0x01, 1);		//响铃提示
					}
				}
				break;

			case 0x83:		// 中心的求助应答
				break;

			default:
				break;
			}
		}

		// 处理信息更新
		if(sta_ulInfoTm==0)		sta_ulInfoTm = GetTickCount();
		if( (GetTickCount()-sta_ulInfoTm) > 2500 )  
		{ 
			sta_ulInfoTm = 0;

			if(g_diskfind) 
			{
				g_objDiaodu.show_diaodu(LANG_CHKING_U);
			}
			
			if(g_SysUpdate) 
			{
				g_objDiaodu.show_diaodu(LANG_NK_UPGRADING);
			}
			
			if (g_AppUpdate)
			{
				g_objDiaodu.show_diaodu(LANG_SFT_UPGRADING);
			}

			if(g_DataOutPut)	// 数据导出信息
			{
				g_objDiaodu.show_diaodu(LANG_DATAOUTPUT);
			}

// 			if(m_updateSys)	// 升级提示信息
// 			{
// 				g_hst.RequestRing(0x08, 1);		//响铃提示
// 				char buf_updatasys2[] = "正在升级, 请保证供电正常并耐心等侯…";		
// 				CHandset::CreateTextWin((byte*)buf_updatasys2, strlen(buf_updatasys2));
// 			}
			
			if(m_gpsdect)	// 实时显示GPS显示信息
			{		
				char buf[200] = {0};
				static ulong t2 = 0;

				if(m_gpsdect==1)
				{ 
					m_gpsdect = 2; 
					t2 = GetTickCount(); 
				}
				if( (GetTickCount()-t2) > 30*1000 )
				{
					t2 = 0;
					m_gpsdect = 0;
					memcpy(buf, "GPS实时检测模式已经关闭!", 24);
					g_hst.CancelRing();
				} 
				else 
				{
					tagSatelliteInfo info;
					char str[100] = {0};

					if(g_objGpsSrc.GetSatellitesInfo(info)) 
					{
						int cnt = 0;
						int i=0;
						for(i=0; i<info.m_bytSatelliteCount; i++)
						{
							if(info.m_arySatelliteInfo[i][1]>35)
								cnt++;
						}

						sprintf(str, "NUM:%02d      SNR>35:%d    ", info.m_bytSatelliteCount, cnt);
					
						for(i=0; i<info.m_bytSatelliteCount; i++)
						{
							char tmp[20] = {0};
							sprintf(tmp, "%d/%d ", info.m_arySatelliteInfo[i][0], info.m_arySatelliteInfo[i][1]);
							
							if((strlen(str)+strlen(tmp))>=45)
								break;

							strcat(str, tmp);
						}
						memcpy(buf ,str, strlen(str));
					}
					else
					{
						memcpy(buf, "暂时获取不到GPS信号!稍侯...", 27);
					}
				}
				g_hst.ActivateStringPrompt_TIME(buf, 1);	
			}
		}

		//一分钟校时一次
		if (GetTickCount() - m_hsttm >= 1*1000 && m_IsReturnMainWin)
		{
			DisplayScreen();	
			m_hsttm = GetTickCount();
		}

		// 5秒钟请求一次信号强度和GPS定位状态
		static DWORD sta_dwTm = GetTickCount();
		if( (GetTickCount() - sta_dwTm) > 5000)
		{
			sta_dwTm = GetTickCount();

			char buf[10];
			buf[0] = 0x24;	//请求信号强度
			DWORD dwPacketNum;
			g_objPhoneRecvDataMng.PushData(LV1, 1, buf, dwPacketNum);	

			buf[0] = 0x03;
			g_objDiaodu.m_objDiaoduReadMng.PushData(LV1, 1, buf, dwPacketNum);
		}

		// 一个小时后回到待机界面
		if (GetTickCount() - g_hsttm >= 6*60*1000) 
		{
			g_hst.ActivateLowestWinStack();	
			g_hsttm = GetTickCount();
		}

		// 如果1分钟内没有按抢答键或者收到抢答应答后没有按确认键
		if ((GetTickCount() - m_dianzhaotm > 1*60*1000 && m_dianzhaostate == 0) ||	
			(GetTickCount() - m_qiangdasucctm > 1*60*1000 && m_dianzhaostate == 2) )	
		{
			g_hst.CancelRing();
			g_hst.ActivateLowestWinStack();
			m_dianzhaotm = GetTickCount();
			m_qiangdasucctm = GetTickCount();
			m_dianzhaostate = 0xff;
		}

		//抢答时间超过30s，即30s内没有收到抢答结果
		if (GetTickCount() - m_qiangdatm > 30*1000 && m_dianzhaostate == 1)	
		{
			PRTMSG(MSG_NOR, "Qiangda Unsucc, Dianzhao over\n");

			char qiangda[30] = {0};
			memcpy(qiangda, "抢答失败,按OK键结束电召", 24);
			m_dianzhaoprompt.Mode        = 0;
			m_dianzhaoprompt.LiveTime    = 0;
			m_dianzhaoprompt.ExitProc    = g_hst.CancelRing;
			m_dianzhaoprompt.ExitKeyProc = 0;
			m_dianzhaoprompt.TextPtr     = (INT8U *)qiangda;
			m_dianzhaoprompt.TextLen     = 24;
			g_hst.ActivatePrompt(&m_dianzhaoprompt);
			g_hst.RequestRing(0x04, 1);		//响铃提示
			m_qiangdatm = GetTickCount();
			m_dianzhaostate = 0xff;
		}

		g_hst.WinStackProc();	
	}
}

void CHstApp::Init_Menu()
{
	SelMenuWinEntry.WinAttrib = WIN_FMENU;
	SelMenuWinEntry.Handler_KEY_ACK = Handler_KEY_ACK_CallMan;
	SelMenuWinEntry.Handler_KEY_FCN = 0;
	SelMenuWinEntry.Handler_KEY_NAK = 0;
	SelMenuWinEntry.Handler_KEY_SEL = 0;
	SelMenuWinEntry.InitRecordProc = 0;
	SelMenuWinEntry.MaxRecordProc = MaxRecordProc_CallMan;
	SelMenuWinEntry.ReadRecordProc = ReadRecordProc_CallMan;
	SelMenuWinEntry.StoreProc = 0;

	MENU_ERASE_DIAL.Attrib = ITEM_FCN;
	MENU_ERASE_DIAL.ChildItem = 0;
	memcpy(MENU_ERASE_DIAL.ContentStr, "已拨电话", 8);
	MENU_ERASE_DIAL.EntryProc = DelTalkProc;
	MENU_ERASE_DIAL.NumChild = 0;

	MENU_ERASE_MISS.Attrib = ITEM_FCN;
	MENU_ERASE_MISS.ChildItem = 0;
	memcpy(MENU_ERASE_MISS.ContentStr, "未接电话", 8);
	MENU_ERASE_MISS.EntryProc = DelTalkProc;
	MENU_ERASE_MISS.NumChild = 0;
		
	MENU_ERASE_RECV.Attrib = ITEM_FCN;
	MENU_ERASE_RECV.ChildItem = 0;
	memcpy(MENU_ERASE_RECV.ContentStr, "已接电话", 8);
	MENU_ERASE_RECV.EntryProc = DelTalkProc;
	MENU_ERASE_RECV.NumChild = 0;

	MENU_ERASE_ALL.Attrib = ITEM_FCN;
	MENU_ERASE_ALL.ChildItem = 0;
	memcpy(MENU_ERASE_ALL.ContentStr, "所有电话", 8);
	MENU_ERASE_ALL.EntryProc = DelTalkProc;
	MENU_ERASE_ALL.NumChild = 0;

	MENU_ERASE_CHILD[0] = MENU_ERASE_DIAL;
	MENU_ERASE_CHILD[1] = MENU_ERASE_MISS;
	MENU_ERASE_CHILD[2] = MENU_ERASE_RECV;
	MENU_ERASE_CHILD[3] = MENU_ERASE_ALL;

	MENU_MISS.Attrib = ITEM_FCN;
	MENU_MISS.ChildItem = 0;
	memcpy(MENU_MISS.ContentStr, "未接电话", 8);
	MENU_MISS.EntryProc = SelTalkProc;
	MENU_MISS.NumChild = 0;

	MENU_DIAL.Attrib = ITEM_FCN;
	MENU_DIAL.ChildItem = 0;
	memcpy(MENU_DIAL.ContentStr, "已拨电话", 8);
	MENU_DIAL.EntryProc = SelTalkProc;
	MENU_DIAL.NumChild = 0;

	MENU_RECV.Attrib = ITEM_FCN;
	MENU_RECV.ChildItem = 0;
	memcpy(MENU_RECV.ContentStr, "已接电话", 8);
	MENU_RECV.EntryProc = SelTalkProc;
	MENU_RECV.NumChild = 0;

	MENU_ERASE.Attrib = ITEM_LMENU;
	MENU_ERASE.ChildItem = (ITEMMENU_STRUCT *)MENU_ERASE_CHILD;
	memcpy(MENU_ERASE.ContentStr, "删除", 4);
	MENU_ERASE.EntryProc = 0;
	MENU_ERASE.NumChild = 4;

	MENU_CALLMAN_CHILD[0] = MENU_MISS;
	MENU_CALLMAN_CHILD[1] = MENU_DIAL;
	MENU_CALLMAN_CHILD[2] = MENU_RECV;
	MENU_CALLMAN_CHILD[3] = MENU_ERASE;

	MENU_CALLMAN.Attrib = ITEM_LMENU;
	MENU_CALLMAN.ChildItem = (ITEMMENU_STRUCT *)MENU_CALLMAN_CHILD;
	memcpy(MENU_CALLMAN.ContentStr, "  通话记录", 10);
	MENU_CALLMAN.EntryProc = 0;
	MENU_CALLMAN.NumChild = 4;

	MENU_OPREC_DIAL.Attrib = ITEM_FCN;
	MENU_OPREC_DIAL.ChildItem = 0;
	memcpy(MENU_OPREC_DIAL.ContentStr, "呼出", 4);
	MENU_OPREC_DIAL.EntryProc = OPREC_DIAL;
	MENU_OPREC_DIAL.NumChild = 0;

	MENU_OPREC_READ.Attrib = ITEM_FCN;
	MENU_OPREC_READ.ChildItem = 0;
	memcpy(MENU_OPREC_READ.ContentStr, "通话时间", 8);
	MENU_OPREC_READ.EntryProc = OPREC_READ;
	MENU_OPREC_READ.NumChild = 0;

	MENU_OPREC_ERASE.Attrib = ITEM_FCN;
	MENU_OPREC_ERASE.ChildItem = 0;
	memcpy(MENU_OPREC_ERASE.ContentStr, "删除", 4);
	MENU_OPREC_ERASE.EntryProc = OPREC_ERASE;
	MENU_OPREC_ERASE.NumChild = 0;

	MENU_OPREC_DISPLAY.Attrib = ITEM_FCN;
	MENU_OPREC_DISPLAY.ChildItem = 0;
	memcpy(MENU_OPREC_DISPLAY.ContentStr, "显示号码", 8);
	MENU_OPREC_DISPLAY.EntryProc = OPREC_DISPLAY;
	MENU_OPREC_DISPLAY.NumChild = 0;

	MENU_OPREC_SAVE.Attrib = ITEM_FCN;
	MENU_OPREC_SAVE.ChildItem = 0;
	memcpy(MENU_OPREC_SAVE.ContentStr, "存储", 4);
	MENU_OPREC_SAVE.EntryProc = OPREC_SAVE;
	MENU_OPREC_SAVE.NumChild = 0;

	MENU_OPREC_CHILD_CM[0] = MENU_OPREC_DIAL;
	MENU_OPREC_CHILD_CM[1] = MENU_OPREC_READ;
	MENU_OPREC_CHILD_CM[2] = MENU_OPREC_ERASE;
	MENU_OPREC_CHILD_CM[3] = MENU_OPREC_DISPLAY;
	MENU_OPREC_CHILD_CM[4] = MENU_OPREC_SAVE;

	
	MENU_OPREC_CM.Attrib = ITEM_LMENU;
	MENU_OPREC_CM.ChildItem = (ITEMMENU_STRUCT *)MENU_OPREC_CHILD_CM;
	memset(MENU_OPREC_CM.ContentStr, 0, 20);
	MENU_OPREC_CM.EntryProc = 0;
	MENU_OPREC_CM.NumChild = 5;
	
	MENU_ADD.Attrib = ITEM_FCN;
	MENU_ADD.ChildItem = 0;
	memcpy(MENU_ADD.ContentStr, "增加", 4);
	MENU_ADD.EntryProc = AddTB;
	MENU_ADD.NumChild = 0;

	MENU_FIND.Attrib = ITEM_FCN;
	MENU_FIND.ChildItem = 0;
	memcpy(MENU_FIND.ContentStr, "查找", 4);
	MENU_FIND.EntryProc = FindTB;
	MENU_FIND.NumChild = 0;

	MENU_LIST.Attrib = ITEM_FCN;
	MENU_LIST.ChildItem = 0;
	memcpy(MENU_LIST.ContentStr, "浏览", 4);
	MENU_LIST.EntryProc = ListTB;
	MENU_LIST.NumChild = 0;

	MENU_DelALL.Attrib = ITEM_FCN;
	MENU_DelALL.ChildItem = 0;
	memcpy(MENU_DelALL.ContentStr, "删除所有", 8);
	MENU_DelALL.EntryProc = DelAllTB;
	MENU_DelALL.NumChild = 0;

	MENU_Browse.Attrib = ITEM_FCN;
	MENU_Browse.ChildItem = 0;
	memcpy(MENU_Browse.ContentStr, "存储状况", 8);
	MENU_Browse.EntryProc = BrowseMemory;
	MENU_Browse.NumChild = 0;
		
	MENU_TELBOOK_CHILD[0] = MENU_ADD;
	MENU_TELBOOK_CHILD[1] = MENU_FIND;
	MENU_TELBOOK_CHILD[2] = MENU_LIST;
	MENU_TELBOOK_CHILD[3] = MENU_DelALL;
	MENU_TELBOOK_CHILD[4] = MENU_Browse;
	MENU_TELBOOK.Attrib = ITEM_LMENU;
	MENU_TELBOOK.ChildItem = (ITEMMENU_STRUCT *)MENU_TELBOOK_CHILD;
	memcpy(MENU_TELBOOK.ContentStr, "   电话本", 9);
	MENU_TELBOOK.EntryProc = 0;
	MENU_TELBOOK.NumChild = 5;
	
	MENU_DIAL_TB.Attrib = ITEM_FCN;
	MENU_DIAL_TB.ChildItem = 0;
	memcpy(MENU_DIAL_TB.ContentStr, "呼出", 4);
	MENU_DIAL_TB.EntryProc = DialProc;
	MENU_DIAL_TB.NumChild = 0;

	MENU_DEL_TB.Attrib = ITEM_FCN;
	MENU_DEL_TB.ChildItem = 0;
	memcpy(MENU_DEL_TB.ContentStr, "删除", 4);
	MENU_DEL_TB.EntryProc = DelProc;
	MENU_DEL_TB.NumChild = 0;

	MENU_MODIFY_TB.Attrib = ITEM_FCN;
	MENU_MODIFY_TB.ChildItem = 0;
	memcpy(MENU_MODIFY_TB.ContentStr, "修改", 4);
	MENU_MODIFY_TB.EntryProc = ModifyProc;
	MENU_MODIFY_TB.NumChild = 0;

	MENU_OPREC_CHILD_TB[0] = MENU_DIAL_TB;
	MENU_OPREC_CHILD_TB[1] = MENU_MODIFY_TB;
	MENU_OPREC_CHILD_TB[2] = MENU_DEL_TB;
	MENU_OPREC_TB.Attrib = ITEM_LMENU;
	MENU_OPREC_TB.ChildItem = (ITEMMENU_STRUCT *)MENU_OPREC_CHILD_TB;
	memset(MENU_OPREC_TB.ContentStr, 0, 20);
	MENU_OPREC_TB.EntryProc = 0;
	MENU_OPREC_TB.NumChild = 3;

	MENU_READSM.Attrib = ITEM_FCN;
	MENU_READSM.ChildItem = 0;
	memcpy(MENU_READSM.ContentStr, "收到的信息", 10);
	MENU_READSM.EntryProc = ReadSM;
	MENU_READSM.NumChild = 0;
	
	MENU_WRITESM.Attrib = ITEM_FCN;
	MENU_WRITESM.ChildItem = 0;
	memcpy(MENU_WRITESM.ContentStr, "写信息", 6);
	MENU_WRITESM.EntryProc = WriteSM;
	MENU_WRITESM.NumChild = 0;

	MENU_DELALL.Attrib = ITEM_FCN;
	MENU_DELALL.ChildItem = 0;
	memcpy(MENU_DELALL.ContentStr, "删除所有信息", 12);
	MENU_DELALL.EntryProc = DelAll;
	MENU_DELALL.NumChild = 0;

	MENU_SHORTMSG_CHILD[0] = MENU_WRITESM;
	MENU_SHORTMSG_CHILD[1] = MENU_READSM;
	MENU_SHORTMSG_CHILD[2] = MENU_DELALL;
	MENU_SHORTMSG.Attrib = ITEM_LMENU;
	MENU_SHORTMSG.ChildItem = (ITEMMENU_STRUCT *)MENU_SHORTMSG_CHILD;
	memcpy(MENU_SHORTMSG.ContentStr, "   短消息", 9);
	MENU_SHORTMSG.EntryProc = 0;
	MENU_SHORTMSG.NumChild = 3;

	MENU_WRITEBACK.Attrib = ITEM_FCN;
	MENU_WRITEBACK.ChildItem = 0;
	memcpy(MENU_WRITEBACK.ContentStr, "回复", 4);
	MENU_WRITEBACK.EntryProc = WriteBackSM;
	MENU_WRITEBACK.NumChild = 0;

	MENU_TRANSMIT.Attrib = ITEM_FCN;
	MENU_TRANSMIT.ChildItem = 0;
	memcpy(MENU_TRANSMIT.ContentStr, "转发", 4);
	MENU_TRANSMIT.EntryProc = TransmitSM;
	MENU_TRANSMIT.NumChild = 0;

	MENU_ERASE_SM.Attrib = ITEM_FCN;
	MENU_ERASE_SM.ChildItem = 0;
	memcpy(MENU_ERASE_SM.ContentStr, "删除", 4);
	MENU_ERASE_SM.EntryProc = EraseSM;
	MENU_ERASE_SM.NumChild = 0;

	MENU_SAVE_SM.Attrib = ITEM_FCN;
	MENU_SAVE_SM.ChildItem = 0;
	memcpy(MENU_SAVE_SM.ContentStr, "存储号码", 8);
	MENU_SAVE_SM.EntryProc = SaveTel;
	MENU_SAVE_SM.NumChild = 0;

	MENU_DIAL_SM.Attrib = ITEM_FCN;
	MENU_DIAL_SM.ChildItem = 0;
	memcpy(MENU_DIAL_SM.ContentStr, "呼出", 4);
	MENU_DIAL_SM.EntryProc = DialTel_SM;
	MENU_DIAL_SM.NumChild = 0;

	MENU_OPSM_CHILD[0] = MENU_ERASE_SM;
	MENU_OPSM_CHILD[1] = MENU_TRANSMIT;
	MENU_OPSM_CHILD[2] = MENU_WRITEBACK;
	MENU_OPSM_CHILD[3] = MENU_SAVE_SM;
	MENU_OPSM_CHILD[4] = MENU_DIAL_SM;
	MENU_OPSM.Attrib = ITEM_LMENU;
	MENU_OPSM.ChildItem = (ITEMMENU_STRUCT *)MENU_OPSM_CHILD;
	memset(MENU_OPSM.ContentStr, 0, 20);
	MENU_OPSM.EntryProc = 0;
	MENU_OPSM.NumChild = 5;

	MENU_SETGPRS.Attrib = ITEM_FCN;
	MENU_SETGPRS.ChildItem = 0;
	memcpy(MENU_SETGPRS.ContentStr, "    设置\n  GPRS参数", 20);
	MENU_SETGPRS.EntryProc = InputPassword;
	MENU_SETGPRS.NumChild = 0;

	MENU_VERSION.Attrib = ITEM_FCN;
	MENU_VERSION.ChildItem = 0;
	memcpy(MENU_VERSION.ContentStr, "版本说明", 8);
	MENU_VERSION.EntryProc = ReadVersion;
	MENU_VERSION.NumChild = 0;

	MENU_ADCONST.Attrib = ITEM_FCN;
	MENU_ADCONST.ChildItem = 0;
	memcpy(MENU_ADCONST.ContentStr, "调节对比度", 10);
	MENU_ADCONST.EntryProc = ActivateADConst;
	MENU_ADCONST.NumChild = 0;

	MENU_CALLRING.Attrib = ITEM_FCN;
	MENU_CALLRING.ChildItem = 0;
	memcpy(MENU_CALLRING.ContentStr, "来电铃声", 8);
	MENU_CALLRING.EntryProc = SelRing;
	MENU_CALLRING.NumChild = 0;

	MENU_SMRING.Attrib = ITEM_FCN;
	MENU_SMRING.ChildItem = 0;
	memcpy(MENU_SMRING.ContentStr, "信息铃声", 8);
	MENU_SMRING.EntryProc = SelRing;
	MENU_SMRING.NumChild = 0;

	MENU_SELRING_CHILD[0] = MENU_CALLRING;
	MENU_SELRING_CHILD[1] = MENU_SMRING;
	MENU_SELRING.Attrib = ITEM_LMENU;
	MENU_SELRING.ChildItem = (ITEMMENU_STRUCT *)MENU_SELRING_CHILD;
	memcpy(MENU_SELRING.ContentStr, "选择振铃音", 10);
	MENU_SELRING.EntryProc = 0;
	MENU_SELRING.NumChild = 2;

	MENU_SETKEYPT.Attrib = ITEM_FCN;
	MENU_SETKEYPT.ChildItem = 0;
	memcpy(MENU_SETKEYPT.ContentStr, "设置按键音", 10);
	MENU_SETKEYPT.EntryProc = ActivateSelOptionMenu;
	MENU_SETKEYPT.NumChild = 0;

	MENU_SETPICK.Attrib = ITEM_FCN;
	MENU_SETPICK.ChildItem = 0;
	memcpy(MENU_SETPICK.ContentStr, "设置接听方式", 12);
	MENU_SETPICK.EntryProc = ActivateSelOptionMenu_Pick;
	MENU_SETPICK.NumChild = 0;

	MENU_SETCHA.Attrib = ITEM_FCN;
	MENU_SETCHA.ChildItem = 0;
	memcpy(MENU_SETCHA.ContentStr, "设置缺省模式", 12);
	MENU_SETCHA.EntryProc = ActivateSelOptionMenu_Cha;
	MENU_SETCHA.NumChild = 0;

	MENU_SYSSET_CHILD[0] = MENU_ADCONST;
	MENU_SYSSET_CHILD[1] = MENU_SELRING;
	MENU_SYSSET_CHILD[2] = MENU_SETKEYPT;
	//MENU_SYSSET_CHILD[3] = MENU_SETPICK;	// 不支持设置接听方式
	MENU_SYSSET_CHILD[3] = MENU_SETCHA;
	MENU_SYSSET_CHILD[4] = MENU_VERSION;
	MENU_SYSSET.Attrib = ITEM_LMENU;
	MENU_SYSSET.ChildItem = (ITEMMENU_STRUCT *)MENU_SYSSET_CHILD;
	memcpy(MENU_SYSSET.ContentStr, "  系统设置", 10);
	MENU_SYSSET.EntryProc = 0;
	MENU_SYSSET.NumChild = 5;

	MENU_CENTERNO.Attrib = ITEM_FCN;
	MENU_CENTERNO.ChildItem = 0;
	memcpy(MENU_CENTERNO.ContentStr, "中心特服号", 10);
	MENU_CENTERNO.EntryProc = GetCenterNo;
	MENU_CENTERNO.NumChild = 0;

	MENU_GPRSSTATUS.Attrib = ITEM_FCN;
	MENU_GPRSSTATUS.ChildItem = 0;
	memcpy(MENU_GPRSSTATUS.ContentStr, "GPRS状态", 8);
	MENU_GPRSSTATUS.EntryProc = GetGPRSStatus;
	MENU_GPRSSTATUS.NumChild = 0;

	MENU_GPSSTATUS.Attrib = ITEM_FCN;
	MENU_GPSSTATUS.ChildItem = 0;
	memcpy(MENU_GPSSTATUS.ContentStr, "定位状态", 8);
	MENU_GPSSTATUS.EntryProc = GetGPSStatus;
	MENU_GPSSTATUS.NumChild = 0;

	MENU_PARACHK_CHILD[0] = MENU_CENTERNO;
	MENU_PARACHK_CHILD[1] = MENU_GPRSSTATUS;
	MENU_PARACHK_CHILD[2] = MENU_GPSSTATUS;
	MENU_PARACHK.Attrib = ITEM_LMENU;
	MENU_PARACHK.ChildItem = (ITEMMENU_STRUCT *)MENU_PARACHK_CHILD;
	memcpy(MENU_PARACHK.ContentStr, "参数状态查询", 12);
	MENU_PARACHK.EntryProc = 0;
	MENU_PARACHK.NumChild = 3;

	//----------------------------------wxl
	MENU_DATAREQUEST.Attrib = ITEM_FCN;
	MENU_DATAREQUEST.ChildItem = 0;
	memcpy(MENU_DATAREQUEST.ContentStr, "油量数据请求", 12);
	MENU_DATAREQUEST.EntryProc = SndOilDataDesire;
	MENU_DATAREQUEST.NumChild = 0;

	MENU_BIAODINGSTART.Attrib = ITEM_FCN;
	MENU_BIAODINGSTART.ChildItem = 0;
	memcpy(MENU_BIAODINGSTART.ContentStr, "标定起始指示", 12);
	MENU_BIAODINGSTART.EntryProc = BiaoDStart;
	MENU_BIAODINGSTART.NumChild = 0;

	// ACCON 刻度标定
	MENU_ZEROBD.Attrib = ITEM_FCN;
	MENU_ZEROBD.ChildItem = 0;
	memcpy(MENU_ZEROBD.ContentStr, "0刻度标定", 10);
	MENU_ZEROBD.EntryProc = SetZeroBD;
	MENU_ZEROBD.NumChild = 0;

	MENU_QUERTERBD.Attrib = ITEM_FCN;
	MENU_QUERTERBD.ChildItem = 0;
	memcpy(MENU_QUERTERBD.ContentStr, "1/4刻度标定", 12);
	MENU_QUERTERBD.EntryProc = SetQuerterBD;
	MENU_QUERTERBD.NumChild = 0;

	MENU_HALFBD.Attrib = ITEM_FCN;
	MENU_HALFBD.ChildItem = 0;
	memcpy(MENU_HALFBD.ContentStr, "1/2刻度标定", 12);
	MENU_HALFBD.EntryProc = SetHalfBD;
	MENU_HALFBD.NumChild = 0;

	MENU_THREEFOURBD.Attrib = ITEM_FCN;
	MENU_THREEFOURBD.ChildItem = 0;
	memcpy(MENU_THREEFOURBD.ContentStr, "3/4刻度标定", 12);
	MENU_THREEFOURBD.EntryProc = SetThreeFourBD;
	MENU_THREEFOURBD.NumChild = 0;

	MENU_FULLBD.Attrib = ITEM_FCN;
	MENU_FULLBD.ChildItem = 0;
	memcpy(MENU_FULLBD.ContentStr, "满刻度标定", 12);
	MENU_FULLBD.EntryProc = SetFullBD;
	MENU_FULLBD.NumChild = 0;

	// ACCOFF 刻度标定
	MENU_ZEROBDOFF.Attrib = ITEM_FCN;
	MENU_ZEROBDOFF.ChildItem = 0;
	memcpy(MENU_ZEROBDOFF.ContentStr, "0刻度标定", 10);
	MENU_ZEROBDOFF.EntryProc = SetZeroBDOFF;
	MENU_ZEROBDOFF.NumChild = 0;

	MENU_QUERTERBDOFF.Attrib = ITEM_FCN;
	MENU_QUERTERBDOFF.ChildItem = 0;
	memcpy(MENU_QUERTERBDOFF.ContentStr, "1/4刻度标定", 12);
	MENU_QUERTERBDOFF.EntryProc = SetQuerterBDOFF;
	MENU_QUERTERBDOFF.NumChild = 0;

	MENU_HALFBDOFF.Attrib = ITEM_FCN;
	MENU_HALFBDOFF.ChildItem = 0;
	memcpy(MENU_HALFBDOFF.ContentStr, "1/2刻度标定", 12);
	MENU_HALFBDOFF.EntryProc = SetHalfBDOFF;
	MENU_HALFBDOFF.NumChild = 0;

	MENU_THREEFOURBDOFF.Attrib = ITEM_FCN;
	MENU_THREEFOURBDOFF.ChildItem = 0;
	memcpy(MENU_THREEFOURBDOFF.ContentStr, "3/4刻度标定", 12);
	MENU_THREEFOURBDOFF.EntryProc = SetThreeFourBDOFF;
	MENU_THREEFOURBDOFF.NumChild = 0;

	MENU_FULLBDOFF.Attrib = ITEM_FCN;
	MENU_FULLBDOFF.ChildItem = 0;
	memcpy(MENU_FULLBDOFF.ContentStr, "满刻度标定", 12);
	MENU_FULLBDOFF.EntryProc = SetFullBDOFF;
	MENU_FULLBDOFF.NumChild = 0;

	MENU_GRADUATIONBD_CHILD[0] = MENU_ZEROBD;
	MENU_GRADUATIONBD_CHILD[1] = MENU_QUERTERBD;
	MENU_GRADUATIONBD_CHILD[2] = MENU_HALFBD;
	MENU_GRADUATIONBD_CHILD[3] = MENU_THREEFOURBD;
	MENU_GRADUATIONBD_CHILD[4] = MENU_FULLBD;
	MENU_GRADUATIONBD.Attrib = ITEM_LMENU;
	MENU_GRADUATIONBD.ChildItem = (ITEMMENU_STRUCT *)MENU_GRADUATIONBD_CHILD;
	memcpy(MENU_GRADUATIONBD.ContentStr, " ACCON标定", 16);
	MENU_GRADUATIONBD.EntryProc = 0;
	MENU_GRADUATIONBD.NumChild = 5;

	MENU_GRADUATIONBDOFF_CHILD[0] = MENU_ZEROBDOFF;
	MENU_GRADUATIONBDOFF_CHILD[1] = MENU_QUERTERBDOFF;
	MENU_GRADUATIONBDOFF_CHILD[2] = MENU_HALFBDOFF;
	MENU_GRADUATIONBDOFF_CHILD[3] = MENU_THREEFOURBDOFF;
	MENU_GRADUATIONBDOFF_CHILD[4] = MENU_FULLBDOFF;
	MENU_GRADUATIONBDOFF.Attrib = ITEM_LMENU;
	MENU_GRADUATIONBDOFF.ChildItem = (ITEMMENU_STRUCT *)MENU_GRADUATIONBDOFF_CHILD;
	memcpy(MENU_GRADUATIONBDOFF.ContentStr, " ACCOFF标定", 16);
	MENU_GRADUATIONBDOFF.EntryProc = 0;
	MENU_GRADUATIONBDOFF.NumChild = 5;

	MENU_BIAODINGEND.Attrib = ITEM_FCN;
	MENU_BIAODINGEND.ChildItem = 0;
	memcpy(MENU_BIAODINGEND.ContentStr, "标定完成指示", 12);
	MENU_BIAODINGEND.EntryProc = BiaoDEnd;
	MENU_BIAODINGEND.NumChild = 0;

	MENU_BIAODINGTOCENTER.Attrib = ITEM_FCN;
	MENU_BIAODINGTOCENTER.ChildItem = 0;
	memcpy(MENU_BIAODINGTOCENTER.ContentStr, "发送标定数据",12);
	MENU_BIAODINGTOCENTER.EntryProc = SndBDData;
	MENU_BIAODINGTOCENTER.NumChild = 0;

	MENU_BIAODING_CHILD[0] = MENU_GRADUATIONBD;
	MENU_BIAODING_CHILD[1] = MENU_GRADUATIONBDOFF;
	MENU_BIAODING.Attrib = ITEM_LMENU;
	MENU_BIAODING.ChildItem = (ITEMMENU_STRUCT *)MENU_BIAODING_CHILD;
	memcpy(MENU_BIAODING.ContentStr, " 刻度标定", 10);
	MENU_BIAODING.EntryProc = 0;
	MENU_BIAODING.NumChild = 2;

	MENU_OILCHK_CHILD[0] = MENU_DATAREQUEST;
	MENU_OILCHK_CHILD[1] = MENU_BIAODINGSTART;
	MENU_OILCHK_CHILD[2] = MENU_BIAODING;
//	MENU_OILCHK_CHILD[2] = MENU_GRADUATIONBD; //ACCON刻度标定
//	MENU_OILCHK_CHILD[3] = MENU_GRADUATIONBDOFF; //ACCOFF刻度标定
	MENU_OILCHK_CHILD[3] = MENU_BIAODINGEND;
	MENU_OILCHK_CHILD[4] = MENU_BIAODINGTOCENTER;
	MENU_OILCHK.Attrib = ITEM_LMENU;
	MENU_OILCHK.ChildItem = (ITEMMENU_STRUCT *)MENU_OILCHK_CHILD;
	memcpy(MENU_OILCHK.ContentStr, " 油耗统计 ", 10);
	MENU_OILCHK.EntryProc = 0;
	MENU_OILCHK.NumChild = 5;
	//------------------wxl

	//----------------------------------------hhq
	MENU_CHN1VIEW.Attrib = ITEM_FCN;
	MENU_CHN1VIEW.ChildItem = 0;
	memcpy(MENU_CHN1VIEW.ContentStr, "预览1通道", 9);
	MENU_CHN1VIEW.EntryProc = SwhChn1View;
	MENU_CHN1VIEW.NumChild = 0;

	MENU_CHN2VIEW.Attrib = ITEM_FCN;
	MENU_CHN2VIEW.ChildItem = 0;
	memcpy(MENU_CHN2VIEW.ContentStr, "预览2通道", 9);
	MENU_CHN2VIEW.EntryProc = SwhChn2View;
	MENU_CHN2VIEW.NumChild = 0;

	MENU_CHN3VIEW.Attrib = ITEM_FCN;
	MENU_CHN3VIEW.ChildItem = 0;
	memcpy(MENU_CHN3VIEW.ContentStr, "预览3通道", 9);
	MENU_CHN3VIEW.EntryProc = SwhChn3View;
	MENU_CHN3VIEW.NumChild = 0;

	MENU_CHN4VIEW.Attrib = ITEM_FCN;
	MENU_CHN4VIEW.ChildItem = 0;
	memcpy(MENU_CHN4VIEW.ContentStr, "预览4通道", 9);
	MENU_CHN4VIEW.EntryProc = SwhChn4View;
	MENU_CHN4VIEW.NumChild = 0;

	MENU_CHNAVIEW.Attrib = ITEM_FCN;
	MENU_CHNAVIEW.ChildItem = 0;
	memcpy(MENU_CHNAVIEW.ContentStr, "预览全通道", 9);
	MENU_CHNAVIEW.EntryProc = SwhChnAllView;
	MENU_CHNAVIEW.NumChild = 0;

	MENU_VIDEOVIEW_CHILD[0] = MENU_CHN1VIEW;
	MENU_VIDEOVIEW_CHILD[1] = MENU_CHN2VIEW;
	MENU_VIDEOVIEW_CHILD[2] = MENU_CHN3VIEW;
	MENU_VIDEOVIEW_CHILD[3] = MENU_CHN4VIEW;
	MENU_VIDEOVIEW_CHILD[4] = MENU_CHNAVIEW;
	MENU_VIDEOVIEW.Attrib = ITEM_LMENU;
	MENU_VIDEOVIEW.ChildItem = (ITEMMENU_STRUCT *)MENU_VIDEOVIEW_CHILD;
	memcpy(MENU_VIDEOVIEW.ContentStr, " 视频预览 ", 10);
	MENU_VIDEOVIEW.EntryProc = 0;
	MENU_VIDEOVIEW.NumChild = 5;
	//----------------------------------------hhq

	MENU_ATMPMSG.Attrib = ITEM_FCN;
	MENU_ATMPMSG.ChildItem = 0;
	memcpy(MENU_ATMPMSG.ContentStr, "  调度信息", 10);
	MENU_ATMPMSG.EntryProc = ReadATMP;
	MENU_ATMPMSG.NumChild = 0;

	MENU_PCMSG.Attrib = ITEM_FCN;
	MENU_PCMSG.ChildItem = 0;
	memcpy(MENU_PCMSG.ContentStr, "  电召信息", 10);
	MENU_PCMSG.EntryProc = ReadPC;
	MENU_PCMSG.NumChild = 0;
	
	Handler_Key_entry[0].index = KEY_FCN;
	Handler_Key_entry[0].entryproc = Handler_KEY_FCN;  //cyh add 主界面按下SET键
	Handler_Key_entry[1].index = KEY_ACK;
	Handler_Key_entry[1].entryproc = Handler_KEY_ACK;  //cyh add 主界面按下OK键
	Handler_Key_entry[2].index = KEY_UP;
	Handler_Key_entry[2].entryproc = Handler_KEY_UP;
	Handler_Key_entry[3].index = KEY_DOWN;
	Handler_Key_entry[3].entryproc = Handler_KEY_DOWN;
	Handler_Key_entry[4].index = KEY_MEDICAL;
	Handler_Key_entry[4].entryproc = Handler_KEY_SUCCOR;
	Handler_Key_entry[5].index = KEY_TROUBLE;
	Handler_Key_entry[5].entryproc = Handler_KEY_SUCCOR;
	Handler_Key_entry[6].index = KEY_INFORM;
	Handler_Key_entry[6].entryproc = Handler_KEY_SUCCOR;

	MENU_OPEN.Attrib = ITEM_FCN;
	MENU_OPEN.ChildItem = 0;
	memcpy(MENU_OPEN.ContentStr, "开", 2);
	MENU_OPEN.EntryProc = StoreProc_KeyPT;
	MENU_OPEN.NumChild = 0;
	MENU_CLOSE.Attrib = ITEM_FCN;
	MENU_CLOSE.ChildItem = 0;
	memcpy(MENU_CLOSE.ContentStr, "关", 2);
	MENU_CLOSE.EntryProc = StoreProc_KeyPT;
	MENU_CLOSE.NumChild = 0;

	MENU_SELOPTION_CHILD[0] = MENU_OPEN;
	MENU_SELOPTION_CHILD[1] = MENU_CLOSE;
	MENU_SELOPTION.Attrib = ITEM_LMENU;
	MENU_SELOPTION.ChildItem = (ITEMMENU_STRUCT *)MENU_SELOPTION_CHILD;
	memset(MENU_SELOPTION.ContentStr, 0, 20);
	MENU_SELOPTION.EntryProc = 0;
	MENU_SELOPTION.NumChild = 2;

	MENU_AUTO.Attrib = ITEM_FCN;
	MENU_AUTO.ChildItem = 0;
	memcpy(MENU_AUTO.ContentStr, "自动接听", 8);
	MENU_AUTO.EntryProc = StoreProc_Pick;
	MENU_AUTO.NumChild = 0;
	MENU_MANUAL.Attrib = ITEM_FCN;
	MENU_MANUAL.ChildItem = 0;
	memcpy(MENU_MANUAL.ContentStr, "手动接听", 8);
	MENU_MANUAL.EntryProc = StoreProc_Pick;
	MENU_MANUAL.NumChild = 0;

	MENU_SELPICK_CHILD[0] = MENU_AUTO;
	MENU_SELPICK_CHILD[1] = MENU_MANUAL;
	MENU_SELPICK.Attrib = ITEM_LMENU;
	MENU_SELPICK.ChildItem = (ITEMMENU_STRUCT *)MENU_SELPICK_CHILD;
	memset(MENU_SELPICK.ContentStr, 0, 20);
	MENU_SELPICK.EntryProc = 0;
	MENU_SELPICK.NumChild = 2;

	MENU_AUTO_CHA.Attrib = ITEM_FCN;
	MENU_AUTO_CHA.ChildItem = 0;
	memcpy(MENU_AUTO_CHA.ContentStr, "免提", 4);
	MENU_AUTO_CHA.EntryProc = StoreProc_Cha;
	MENU_AUTO_CHA.NumChild = 0;
	MENU_MANUAL_CHA.Attrib = ITEM_FCN;
	MENU_MANUAL_CHA.ChildItem = 0;
	memcpy(MENU_MANUAL_CHA.ContentStr, "手柄", 4);
	MENU_MANUAL_CHA.EntryProc = StoreProc_Cha;
	MENU_MANUAL_CHA.NumChild = 0;

	MENU_SELCHA_CHILD[0] = MENU_AUTO_CHA;
	MENU_SELCHA_CHILD[1] = MENU_MANUAL_CHA;
	MENU_SELCHA.Attrib = ITEM_LMENU;
	MENU_SELCHA.ChildItem = (ITEMMENU_STRUCT *)MENU_SELCHA_CHILD;
	memset(MENU_SELCHA.ContentStr, 0, 20);
	MENU_SELCHA.EntryProc = 0;
	MENU_SELCHA.NumChild = 2;

	MENU_TCPPORT1.Attrib = ITEM_FCN;
	MENU_TCPPORT1.ChildItem = 0;
	memcpy(MENU_TCPPORT1.ContentStr, "TCP端口号1", 10);
	MENU_TCPPORT1.EntryProc = SetTCPPort1;
	MENU_TCPPORT1.NumChild = 0;

	MENU_TCPPORT2.Attrib = ITEM_FCN;
	MENU_TCPPORT2.ChildItem = 0;
	memcpy(MENU_TCPPORT2.ContentStr, "TCP端口号2", 10);
	MENU_TCPPORT2.EntryProc = SetTCPPort2;
	MENU_TCPPORT2.NumChild = 0;

	MENU_UDPPORT1.Attrib = ITEM_FCN;
	MENU_UDPPORT1.ChildItem = 0;
	memcpy(MENU_UDPPORT1.ContentStr, "UDP端口号1", 10);
	MENU_UDPPORT1.EntryProc = SetUDPPort1;
	MENU_UDPPORT1.NumChild = 0;

	MENU_UDPPORT2.Attrib = ITEM_FCN;
	MENU_UDPPORT2.ChildItem = 0;
	memcpy(MENU_UDPPORT2.ContentStr, "UDP端口号2", 10);
	MENU_UDPPORT2.EntryProc = SetUDPPort2;
	MENU_UDPPORT2.NumChild = 0;

	MENU_TCPIP1.Attrib = ITEM_FCN;
	MENU_TCPIP1.ChildItem = 0;
	memcpy(MENU_TCPIP1.ContentStr, "TCPIP地址1", 10);
	MENU_TCPIP1.EntryProc = SetTCPIP1;
	MENU_TCPIP1.NumChild = 0;

	MENU_TCPIP2.Attrib = ITEM_FCN;
	MENU_TCPIP2.ChildItem = 0;
	memcpy(MENU_TCPIP2.ContentStr, "TCPIP地址2", 10);
	MENU_TCPIP2.EntryProc = SetTCPIP2;
	MENU_TCPIP2.NumChild = 0;
	
	MENU_UDPIP1.Attrib = ITEM_FCN;
	MENU_UDPIP1.ChildItem = 0;
	memcpy(MENU_UDPIP1.ContentStr, "UDPIP地址1", 10);
	MENU_UDPIP1.EntryProc = SetUDPIP1;
	MENU_UDPIP1.NumChild = 0;

	MENU_UDPIP2.Attrib = ITEM_FCN;
	MENU_UDPIP2.ChildItem = 0;
	memcpy(MENU_UDPIP2.ContentStr, "UDPIP地址2", 10);
	MENU_UDPIP2.EntryProc = SetUDPIP2;
	MENU_UDPIP2.NumChild = 0;

	MENU_APN.Attrib = ITEM_FCN;
	MENU_APN.ChildItem = 0;
	memcpy(MENU_APN.ContentStr, "APN", 3);
	MENU_APN.EntryProc = SetAPN;
	MENU_APN.NumChild = 0;

	MENU_TEL.Attrib = ITEM_FCN;
	MENU_TEL.ChildItem = 0;
	memcpy(MENU_TEL.ContentStr, "本机号码", 8);
	MENU_TEL.EntryProc = SetTel;
	MENU_TEL.NumChild = 0;

	MENU_LISTENTEL.Attrib = ITEM_FCN;
	MENU_LISTENTEL.ChildItem = 0;
	memcpy(MENU_LISTENTEL.ContentStr, "监听号码", 8);
	MENU_LISTENTEL.EntryProc = SetListenTel;
	MENU_LISTENTEL.NumChild = 0;

	MENU_ACTIVATE.Attrib = ITEM_FCN;
	MENU_ACTIVATE.ChildItem = 0;
	memcpy(MENU_ACTIVATE.ContentStr, "激活GPRS", 8);
	MENU_ACTIVATE.EntryProc = Activate;
	MENU_ACTIVATE.NumChild = 0;

	MENU_CDMANAME.Attrib = ITEM_FCN;
	MENU_CDMANAME.ChildItem = 0;
	memcpy(MENU_CDMANAME.ContentStr, "CDMA用户名", 10);
	MENU_CDMANAME.EntryProc = SetCdmaName;
	MENU_CDMANAME.NumChild = 0;

	MENU_CDMAPASS.Attrib = ITEM_FCN;
	MENU_CDMAPASS.ChildItem = 0;
	memcpy(MENU_CDMAPASS.ContentStr, "CDMA密码", 8);
	MENU_CDMAPASS.EntryProc = SetCdmaPass;
	MENU_CDMAPASS.NumChild = 0;

//add lzy 101026
	MENU_TERMIVER.Attrib = ITEM_FCN;
	MENU_TERMIVER.ChildItem = 0;
	memcpy(MENU_TERMIVER.ContentStr, "终端版本号", 14);
    MENU_TERMIVER.EntryProc = Settemiver;
	MENU_TERMIVER.NumChild = 0;

	MENU_SETGROUPID.Attrib = ITEM_FCN;
	MENU_SETGROUPID.ChildItem = 0;
	memcpy(MENU_SETGROUPID.ContentStr, "集团计费号", 14);
    MENU_SETGROUPID.EntryProc = Setgroupid;
	MENU_SETGROUPID.NumChild = 0;
	
	MENU_SETCOMP.Attrib = ITEM_FCN;
	MENU_SETCOMP.ChildItem = 0;
	memcpy(MENU_SETCOMP.ContentStr, "厂商ID", 12);
    MENU_SETCOMP.EntryProc = Setcompanyid;
	MENU_SETCOMP.NumChild = 0;	
//

	MENU_SET_CHILD[0] = MENU_TEL;
	MENU_SET_CHILD[1] = MENU_APN;
	MENU_SET_CHILD[2] = MENU_TCPIP1;
	MENU_SET_CHILD[3] = MENU_UDPIP1;
	MENU_SET_CHILD[4] = MENU_TCPPORT1;
	MENU_SET_CHILD[5] = MENU_UDPPORT1;
	MENU_SET_CHILD[6] = MENU_TCPIP2;
	MENU_SET_CHILD[7] = MENU_UDPIP2;
	MENU_SET_CHILD[8] = MENU_TCPPORT2;
	MENU_SET_CHILD[9] = MENU_UDPPORT2;
	MENU_SET_CHILD[10] = MENU_ACTIVATE;
	MENU_SET_CHILD[11] = MENU_CDMANAME;
	MENU_SET_CHILD[12] = MENU_CDMAPASS;
#if USE_LIAONING_SANQI == 1
	MENU_SET_CHILD[13] = MENU_TERMIVER;
	MENU_SET_CHILD[14] = MENU_SETCOMP;
	MENU_SET_CHILD[15] = MENU_SETGROUPID;
#endif
//	MENU_SET_CHILD[9] = MENU_LISTENTEL;
	MENU_SET.Attrib = ITEM_LMENU;
	MENU_SET.ChildItem = (ITEMMENU_STRUCT *)MENU_SET_CHILD;
	memset(MENU_SET.ContentStr, 0, 20);
	MENU_SET.EntryProc = 0;
#if USE_LIAONING_SANQI == 1
	MENU_SET.NumChild = 16;
#else
	MENU_SET.NumChild = 13;
#endif

	MENU_LIP.Attrib = ITEM_FCN;
	MENU_LIP.ChildItem = 0;
	memcpy(MENU_LIP.ContentStr, "流媒体IP地址", 12);
	MENU_LIP.EntryProc = SetLiuIP;
	MENU_LIP.NumChild = 0;

	MENU_LPORT.Attrib = ITEM_FCN;
	MENU_LPORT.ChildItem = 0;
	memcpy(MENU_LPORT.ContentStr, "流媒体端口号", 12);
	MENU_LPORT.EntryProc = SetLiuPort;
	MENU_LPORT.NumChild = 0;
	
	MENU_SET_LCHILD[0] = MENU_LIP;
	MENU_SET_LCHILD[1] = MENU_LPORT;
	MENU_SETL.Attrib = ITEM_LMENU;
	MENU_SETL.ChildItem = (ITEMMENU_STRUCT *)MENU_SET_LCHILD;
	memset(MENU_SETL.ContentStr, 0, 20);
	MENU_SETL.EntryProc = 0;
	MENU_SETL.NumChild = 2;

	MENU_PROJECTACCESS.Attrib = ITEM_FCN;
	MENU_PROJECTACCESS.ChildItem = 0;
	memcpy(MENU_PROJECTACCESS.ContentStr, "  工程菜单", 10);
	MENU_PROJECTACCESS.EntryProc = InputPasswordToProject;
	MENU_PROJECTACCESS.NumChild = 0;

	MENU_UPDATEAPP.Attrib = ITEM_FCN;
	MENU_UPDATEAPP.ChildItem = 0;
	memcpy(MENU_UPDATEAPP.ContentStr, "U盘应用升级", 12);
	MENU_UPDATEAPP.EntryProc = UpdateApp;
	MENU_UPDATEAPP.NumChild = 0;

	MENU_FORMATSDISK.Attrib = ITEM_FCN;
	MENU_FORMATSDISK.ChildItem = 0;
	memcpy(MENU_FORMATSDISK.ContentStr, "格式化SD卡", 12);
	MENU_FORMATSDISK.EntryProc = FormatSDisk;
	MENU_FORMATSDISK.NumChild = 0;
	
	MENU_RESUMESET.Attrib = ITEM_FCN;
	MENU_RESUMESET.ChildItem = 0;
	memcpy(MENU_RESUMESET.ContentStr, "恢复出厂设置", 12);
	MENU_RESUMESET.EntryProc = ResumeSet;
	MENU_RESUMESET.NumChild = 0;

	MENU_UPDATESYS.Attrib = ITEM_FCN;
	MENU_UPDATESYS.ChildItem = 0;
	memcpy(MENU_UPDATESYS.ContentStr, "U盘系统升级", 12);
	MENU_UPDATESYS.EntryProc = UpdateSys;
	MENU_UPDATESYS.NumChild = 0;

	MENU_TESTCUTOIL.Attrib = ITEM_FCN;
	MENU_TESTCUTOIL.ChildItem = 0;
	memcpy(MENU_TESTCUTOIL.ContentStr, "断油断电测试", 12);
	MENU_TESTCUTOIL.EntryProc = TestOilCut;
	MENU_TESTCUTOIL.NumChild = 0;

	MENU_INPUTVIDEO.Attrib = ITEM_FCN;
	MENU_INPUTVIDEO.ChildItem = 0;
	memcpy(MENU_INPUTVIDEO.ContentStr, "视频文件导入", 12);
	MENU_INPUTVIDEO.EntryProc = InputVideo;
	MENU_INPUTVIDEO.NumChild = 0;

	MENU_UDATAOUTPUT.Attrib = ITEM_FCN;
	MENU_UDATAOUTPUT.ChildItem = 0;
	memcpy(MENU_UDATAOUTPUT.ContentStr, "U盘数据导出", 12);
	MENU_UDATAOUTPUT.EntryProc = OutputUData;
	MENU_UDATAOUTPUT.NumChild = 0;

	MENU_TESTPCSOUND.Attrib = ITEM_FCN;
	MENU_TESTPCSOUND.ChildItem = 0;
	memcpy(MENU_TESTPCSOUND.ContentStr, "PC音频测试", 10);
	MENU_TESTPCSOUND.EntryProc = PCSoundTest;
	MENU_TESTPCSOUND.NumChild = 0;

	MENU_CHKSENSORSTA.Attrib = ITEM_FCN;
	MENU_CHKSENSORSTA.ChildItem = 0;
	memcpy(MENU_CHKSENSORSTA.ContentStr, "传感器状态", 10);
	MENU_CHKSENSORSTA.EntryProc = ChkSensorState;
	MENU_CHKSENSORSTA.NumChild = 0;

	MENU_TESTALARM.Attrib = ITEM_FCN;
	MENU_TESTALARM.ChildItem = 0;
	memcpy(MENU_TESTALARM.ContentStr, "报警测试", 8);
	MENU_TESTALARM.EntryProc = TestAlarm;
	MENU_TESTALARM.NumChild = 0;

	MENU_CHKLED_TIME.Attrib = ITEM_FCN;
	MENU_CHKLED_TIME.ChildItem = 0;
	memcpy(MENU_CHKLED_TIME.ContentStr, "LED校时", 8);
	MENU_CHKLED_TIME.EntryProc = ChkLED;
	MENU_CHKLED_TIME.NumChild = 0;

	MENU_CHKLED_SHOW.Attrib = ITEM_FCN;
	MENU_CHKLED_SHOW.ChildItem = 0;
	memcpy(MENU_CHKLED_SHOW.ContentStr, "发送即时信息", 12);
	MENU_CHKLED_SHOW.EntryProc = ChkLED;
	MENU_CHKLED_SHOW.NumChild = 0;

	MENU_CHKLED_LIGH.Attrib = ITEM_FCN;
	MENU_CHKLED_LIGH.ChildItem = 0;
	memcpy(MENU_CHKLED_LIGH.ContentStr, "LED亮屏", 8);
	MENU_CHKLED_LIGH.EntryProc = ChkLED;
	MENU_CHKLED_LIGH.NumChild = 0;

	MENU_CHKLED_BLAC.Attrib = ITEM_FCN;
	MENU_CHKLED_BLAC.ChildItem = 0;
	memcpy(MENU_CHKLED_BLAC.ContentStr, "LED黑屏", 8);
	MENU_CHKLED_BLAC.EntryProc = ChkLED;
	MENU_CHKLED_BLAC.NumChild = 0;

	MENU_CHKLED_CHILD[0] = MENU_CHKLED_TIME;
	MENU_CHKLED_CHILD[1] = MENU_CHKLED_SHOW;
	MENU_CHKLED_CHILD[2] = MENU_CHKLED_LIGH;
	MENU_CHKLED_CHILD[3] = MENU_CHKLED_BLAC;
	MENU_CHKLED.Attrib = ITEM_LMENU;
	MENU_CHKLED.ChildItem = (ITEMMENU_STRUCT *)MENU_CHKLED_CHILD;
	memcpy(MENU_CHKLED.ContentStr, "LED屏检测", 10);
	MENU_CHKLED.EntryProc = 0;
	MENU_CHKLED.NumChild = 4;

	MENU_CHKGPS.Attrib = ITEM_FCN;
	MENU_CHKGPS.ChildItem = 0;
	memcpy(MENU_CHKGPS.ContentStr, "检测GPS信号", 12);
	MENU_CHKGPS.EntryProc = ChkGPS;
	MENU_CHKGPS.NumChild = 0;

	MENU_TESTCAM1.Attrib = ITEM_FCN;
	MENU_TESTCAM1.ChildItem = 0;
	memcpy(MENU_TESTCAM1.ContentStr, "摄像头1检测", 12);
	MENU_TESTCAM1.EntryProc = TestCAM1;
	MENU_TESTCAM1.NumChild = 0;

	MENU_TESTCAM2.Attrib = ITEM_FCN;
	MENU_TESTCAM2.ChildItem = 0;
	memcpy(MENU_TESTCAM2.ContentStr, "摄像头2检测", 12);
	MENU_TESTCAM2.EntryProc = TestCAM2;
	MENU_TESTCAM2.NumChild = 0;

	MENU_TESTCAM3.Attrib = ITEM_FCN;
	MENU_TESTCAM3.ChildItem = 0;
	memcpy(MENU_TESTCAM3.ContentStr, "摄像头3检测", 12);
	MENU_TESTCAM3.EntryProc = TestCAM3;
	MENU_TESTCAM3.NumChild = 0;

	MENU_TESTCAM4.Attrib = ITEM_FCN;
	MENU_TESTCAM4.ChildItem = 0;
	memcpy(MENU_TESTCAM4.ContentStr, "摄像头4检测", 12);
	MENU_TESTCAM4.EntryProc = TestCAM4;
	MENU_TESTCAM4.NumChild = 0;

	MENU_TESTCAM_CHILD[0] = MENU_TESTCAM1;
	MENU_TESTCAM_CHILD[1] = MENU_TESTCAM2;
	MENU_TESTCAM_CHILD[2] = MENU_TESTCAM3;
	MENU_TESTCAM_CHILD[3] = MENU_TESTCAM4;
	MENU_TESTCAM.Attrib = ITEM_LMENU;
	MENU_TESTCAM.ChildItem = (ITEMMENU_STRUCT *)MENU_TESTCAM_CHILD;
	memcpy(MENU_TESTCAM.ContentStr, "摄像头检测", 10);
	MENU_TESTCAM.EntryProc = 0;
	MENU_TESTCAM.NumChild = 4;

	MENU_LOGINLIU.Attrib = ITEM_FCN;
	MENU_LOGINLIU.ChildItem = 0;
	memcpy(MENU_LOGINLIU.ContentStr, "登陆流媒体", 12);
    MENU_LOGINLIU.EntryProc = Loginliu;
	MENU_LOGINLIU.NumChild = 0;
////

	MENU_PROJECT_CHILD[0] = MENU_UPDATEAPP;
	MENU_PROJECT_CHILD[1] = MENU_RESUMESET;
	MENU_PROJECT_CHILD[2] = MENU_UPDATESYS;
	MENU_PROJECT_CHILD[3] = MENU_TESTCUTOIL;
	MENU_PROJECT_CHILD[4] = MENU_INPUTVIDEO;
	MENU_PROJECT_CHILD[5] = MENU_UDATAOUTPUT;
	MENU_PROJECT_CHILD[6] = MENU_TESTPCSOUND;
	MENU_PROJECT_CHILD[7] = MENU_TESTCAM;
	MENU_PROJECT_CHILD[8] = MENU_CHKSENSORSTA;
	MENU_PROJECT_CHILD[9] = MENU_TESTALARM;
	MENU_PROJECT_CHILD[10] = MENU_CHKLED;
	MENU_PROJECT_CHILD[11] = MENU_CHKGPS;
	MENU_PROJECT_CHILD[12] = MENU_FORMATSDISK;
	//add lzy 101026
	MENU_PROJECT_CHILD[13] = MENU_LOGINLIU;

	MENU_PROJECT.Attrib = ITEM_LMENU;
	MENU_PROJECT.ChildItem = (ITEMMENU_STRUCT *)MENU_PROJECT_CHILD;
	memset(MENU_PROJECT.ContentStr, 0, 20);
	MENU_PROJECT.EntryProc = 0;
	MENU_PROJECT.NumChild = 14;

	//主菜单
	MENU_MAIN_CHILD[0] = MENU_TELBOOK;  //cyh add 电话本 对应的菜单
	MENU_MAIN_CHILD[1] = MENU_SHORTMSG;  //cyh add 短消息 对应的菜单
	MENU_MAIN_CHILD[2] = MENU_CALLMAN;  //cyh add 通话记录 对应的菜单
	MENU_MAIN_CHILD[3] = MENU_SETGPRS;  //cyh add 设置GPRS参数 对应的菜单
	MENU_MAIN_CHILD[4] = MENU_SYSSET;  //cyh add 系统设置 对应的菜单
	MENU_MAIN_CHILD[5] = MENU_PARACHK;  //cyh add 参数状态查询 对应的菜单
	MENU_MAIN_CHILD[6] = MENU_ATMPMSG;  //cyh add 调度信息 对应的菜单
	MENU_MAIN_CHILD[7] = MENU_PROJECTACCESS;  //cyh add 工程菜单 对应的菜单
	MENU_MAIN_CHILD[8] = MENU_PCMSG;  //cyh add 电召 对应的菜单
	MENU_MAIN_CHILD[9] = MENU_OILCHK; //wxl add 油耗检测盒对应的菜单
	MENU_MAIN_CHILD[10] = MENU_VIDEOVIEW; //hhq add 视频预览对应的菜单

	MENU_MAIN.Attrib = ITEM_FMENU;
	MENU_MAIN.ChildItem = (ITEMMENU_STRUCT *)MENU_MAIN_CHILD;
	memset(MENU_MAIN.ContentStr, 0, 20);
	MENU_MAIN.EntryProc = 0;
	MENU_MAIN.NumChild = 11;

	MenuWinEntry.WinAttrib = WIN_LMENU;
	MenuWinEntry.Handler_KEY_ACK = Handler_KEY_ACK_Ring;
	MenuWinEntry.Handler_KEY_FCN = 0;
	MenuWinEntry.Handler_KEY_NAK = Handler_KEY_NAK_Ring;
	MenuWinEntry.Handler_KEY_SEL = Handler_KEY_SEL_Ring;
	MenuWinEntry.InitRecordProc = InitRecordProc_Ring;
	MenuWinEntry.MaxRecordProc = MaxRecordProc_Ring;
	MenuWinEntry.ReadRecordProc = ReadRecordProc_Ring;
	MenuWinEntry.StoreProc = 0;                                                                                                                  

	MENU_LISTSM.WinAttrib = WIN_LMENU;
	MENU_LISTSM.Handler_KEY_ACK = Handler_KEY_ACK_SM;
	MENU_LISTSM.Handler_KEY_FCN = 0;
	MENU_LISTSM.Handler_KEY_NAK = 0;
	MENU_LISTSM.Handler_KEY_SEL = 0;
	MENU_LISTSM.InitRecordProc = 0;
	MENU_LISTSM.MaxRecordProc = MaxRecordProc_SM;
	MENU_LISTSM.ReadRecordProc = ReadRecordProc_SM;
	MENU_LISTSM.StoreProc = 0;

	EditorEntry_MainWin.Mode     = EDT_REVERSE | EDT_DIGITAL | EDT_PULS;
    EditorEntry_MainWin.MinLen   = 0;
    EditorEntry_MainWin.MaxLen   = sizeof(m_TelEditBuf);
	EditorEntry_MainWin.LabelLen = 0;
    EditorEntry_MainWin.LabelPtr = 0;
    EditorEntry_MainWin.InitLen = 1;
    EditorEntry_MainWin.InitPtr = (INT8U *)m_TelEditBuf;
	EditorEntry_MainWin.StorePtr = 0;
	EditorEntry_MainWin.ExitProc = ExitTelEditor;
	EditorEntry_MainWin.AckProc  = 0;
	EditorEntry_MainWin.CurLenProc = 0;
	EditorEntry_MainWin.MinLenProc  = 0;
	EditorEntry_MainWin.MaxLenProc = 0;
	EditorEntry_MainWin.FilterProc = 0;
	
	MENU_LISTATMP.WinAttrib = WIN_LMENU;
	MENU_LISTATMP.Handler_KEY_ACK = Handler_KEY_ACK_Atmp;
	MENU_LISTATMP.Handler_KEY_FCN = 0;
	MENU_LISTATMP.Handler_KEY_NAK = 0;
	MENU_LISTATMP.Handler_KEY_SEL = 0;
	MENU_LISTATMP.InitRecordProc = 0;
	MENU_LISTATMP.MaxRecordProc = MaxRecordProc_Atmp;
	MENU_LISTATMP.ReadRecordProc = ReadRecordProc_Atmp;
	MENU_LISTATMP.StoreProc = 0;

	MENU_READCONTENT.WinAttrib = WIN_FMENU;
	MENU_READCONTENT.Handler_KEY_ACK = 0;
	MENU_READCONTENT.Handler_KEY_FCN = 0;
	MENU_READCONTENT.Handler_KEY_NAK = 0;
	MENU_READCONTENT.Handler_KEY_SEL = 0;
	MENU_READCONTENT.InitRecordProc = 0;
	MENU_READCONTENT.MaxRecordProc = MaxRecordProc_CONTENT_Atmp;
	MENU_READCONTENT.ReadRecordProc = ReadRecordProc_CONTENT_Atmp;
	MENU_READCONTENT.StoreProc = 0;

	MENU_LISTPC.WinAttrib = WIN_LMENU;
	MENU_LISTPC.Handler_KEY_ACK = Handler_KEY_ACK_PC;
	MENU_LISTPC.Handler_KEY_FCN = 0;
	MENU_LISTPC.Handler_KEY_NAK = 0;
	MENU_LISTPC.Handler_KEY_SEL = 0;
	MENU_LISTPC.InitRecordProc = 0;
	MENU_LISTPC.MaxRecordProc = MaxRecordProc_PC;
	MENU_LISTPC.ReadRecordProc = ReadRecordProc_PC;
	MENU_LISTPC.StoreProc = 0;

	MENU_READCONTENT_PC.WinAttrib = WIN_FMENU;
	MENU_READCONTENT_PC.Handler_KEY_ACK = 0;
	MENU_READCONTENT_PC.Handler_KEY_FCN = 0;
	MENU_READCONTENT_PC.Handler_KEY_NAK = 0;
	MENU_READCONTENT_PC.Handler_KEY_SEL = 0;
	MENU_READCONTENT_PC.InitRecordProc = 0;
	MENU_READCONTENT_PC.MaxRecordProc = MaxRecordProc_CONTENT_PC;
	MENU_READCONTENT_PC.ReadRecordProc = ReadRecordProc_CONTENT_PC;
	MENU_READCONTENT_PC.StoreProc = 0;
}

bool CHstApp::FindProcEntry(INT16U index, FUNCENTRY_STRUCT *funcentry, INT16U num)
{//cyh  寻找主界面下处理 功能按键的函数
    for (; num > 0; num--, funcentry++)
	{
        if (index == funcentry->index) 
		{
            if (funcentry->entryproc != 0)
			{
                funcentry->entryproc();
                return true;
            }
        }
    }
    return false;
}
/***************DianZhao*********************************************************/
void CHstApp::HdlDianZhao()
{
	g_hst.CancelRing();

	char buf_dz[10];
	buf_dz[0] = 0x7a;
	DataPush(buf_dz, 1, DEV_DIAODU, DEV_QIAN, LV2);	//发送抢答给车台

	m_dianzhaostate = 1;	//发送抢答信息
}

void CHstApp::HdlQiangDa()
{
	char dz[10];
	
	dz[0] = 0x7b;
	dz[1] = 0x01;
	DataPush(dz, 2, DEV_DIAODU, DEV_QIAN, LV2);	//发送电召结果报告
	m_dianzhaostate = 3;	//抢答成功
}

/*************TestAlarm***********************************************************/
void CHstApp::HdlTestAlarm()
{
	char buf[2];
	buf[0] = 0x44;
	if (m_testalarmsta == 1)	//测试成功
	{
		buf[1] = 0x01;
	}
	else if (m_testalarmsta == 2)
	{
		buf[1] = 0x02;
	}

	DataPush(buf, 2, DEV_DIAODU, DEV_QIAN, LV2);
}

/*********************MainWin***************************************************/
void CHstApp::DisplayScreen(void)
{
	//cyh add:add  只有在主界面下，才刷新屏幕
	if (CHandset::curstack!=0)
	{
		return;
	}

	struct tm pCurrentTime;
	G_GetTime(&pCurrentTime);
	
	sprintf((char*)asciidate, "%04d.%02d.%02d", pCurrentTime.tm_year+1900, pCurrentTime.tm_mon+1, pCurrentTime.tm_mday);
	sprintf((char*)asciitime, "%02d:%02d:%02d", pCurrentTime.tm_hour, pCurrentTime.tm_min, pCurrentTime.tm_sec);

    g_hst.VTL_ClearScreen();
    if (3 == g_simcard)
	{
        g_hst.VTL_ScreenPrintStr("  请插SIM卡\n");
    } 
	else
	{
        g_hst.VTL_ScreenPrintChar(' ', 3);
        g_hst.VTL_ScreenPrintMem(asciitime, sizeof(asciitime) - 1 - 3);
        g_hst.VTL_ScreenPrint('\n');
		g_hst.VTL_ScreenPrint(' ');
        g_hst.VTL_ScreenPrintMem(asciidate, sizeof(asciidate) - 1);
    }

    if (m_KBIsLocked) {
        g_hst.VTL_ScreenPrintStr("  按#*开锁");
    } 

    g_hst.OverWriteScreen();
}

void CHstApp::SuspendProc_MainWin(void)
{
//     if (m_HstCfg.m_EnableKeyPrompt) {
//         g_hst.CtlKeyPrompt((char)0x80, 1);
//     } else {
//         g_hst.CtlKeyPrompt((char)0x00, 1);
//     }
}

void CHstApp::ResumeProc_MainWin(void)
{

    if (HaveUnReadSMRec())
        g_hst.TurnonSMIndicator();
    else
        g_hst.TurnoffSMIndicator();

    if (!PromptReadMissTel()) 
	{
        DisplayScreen();
    }

	m_IsReturnMainWin = true;
}

void CHstApp::InitProc_MainWin(void)
{
    ResumeProc_MainWin();
}

void CHstApp::Handler_KEY_ACK(void)
{
    ActivateDialTalkRec();
	m_IsReturnMainWin = false;
}

void CHstApp::Handler_KEY_FCN(void)
{
    g_hst.ActivateItemMenu(&MENU_MAIN);
	m_IsReturnMainWin = false;
}

void CHstApp::Handler_KEY_UP(void)
{
	g_hst.CancelRing();		//取消短信铃声
    ActivateReadShorMsgMenu();
	g_hst.TurnoffSMIndicator();
	m_IsReturnMainWin = false;
}

void CHstApp::Handler_KEY_DOWN(void)
{
    ListTB();
	m_IsReturnMainWin = false;
}

void CHstApp::Handler_KEY_SUCCOR(void)
{
    if (m_curkey.attrib & ATTR_REHIT)
	{
        if (m_curkey.key == KEY_MEDICAL)
		{
// 			char buf[] = {0x02, 0x03, 0x02, 0x02, 0x01};
// 			DataPush(buf, 5, DEV_QIAN, DEV_DVR, LV2);
			StartSuccor(SUCCOR_MEDICAL);
		}
        else if (m_curkey.key == KEY_TROUBLE)
		{
			StartSuccor(SUCCOR_TROUBLE);
		}
        else if (m_curkey.key == KEY_INFORM)
		{
			StartSuccor(SUCCOR_INFORMATION);
		}
    }

	m_IsReturnMainWin = false;
}

void CHstApp::ExitTelEditor(void)
{
    INT8U len;

    if ((len = g_hst.GetEditorData(m_TelEditBuf)) > 0)
	{
		ActivateVirPhone_Dial(true, len, m_TelEditBuf);
	}
}

void CHstApp::Handler_KEY_DIGITAL(void)
{
    memset(m_TelEditBuf, 0, SIZE_TELEDITBUF);
	m_TelEditBuf[0] = m_curkey.key;
    g_hst.CreateEditor(&EditorEntry_MainWin);
	m_IsReturnMainWin = false;
}

//主界面的消息处理函数
void CHstApp::HdlKeyProc_MainWin(void)
{
	if (g_hst.GetKeyMsg(&m_curkey)) 
	{
		if (m_WaitKEY_Star)
		{
			if (m_curkey.key == '*') 
			{
				m_KBIsLocked = !m_KBIsLocked;
				if (m_KBIsLocked)
					g_hst.ActivateStringPrompt("键盘已锁!");
				else
					g_hst.ActivateStringPrompt("键盘锁已开!");
			
				m_WaitKEY_Star = FALSE;
				return;
			} 
			else if (m_curkey.attrib == ATTR_CHAR)
			{
				m_WaitKEY_Star = FALSE;
			}
		}

		if (m_curkey.key == '#')
		{
			m_WaitKEY_Star = TRUE;
			ulong t1 = GetTickCount();		//如果5s内不按'#'键,再按'*'键无法解锁
			if (GetTickCount() - t1 >= 5000) 
			{
				m_WaitKEY_Star = false;
			}
		}

		if (m_KBIsLocked) 
			return;

		// 先判断是否是按下：SET,OK,UP,DOWN,医疗，求助，维修这七个键
		if ( !FindProcEntry(m_curkey.key, Handler_Key_entry, sizeof(Handler_Key_entry)/sizeof(Handler_Key_entry[0])) )
		{
			if( NETWORK_TYPE==NETWORK_GSM )
			{
				if (m_curkey.key==KEY_END && ( g_objPhoneGsm.GetPhoneHdlSta()==PST_DIAL || g_objPhoneGsm.GetPhoneHdlSta()==PST_WAITPICK || g_objPhoneGsm.GetPhoneHdlSta()==PST_HOLD ) )
				{ 
					byte buf[2] = {0};
					buf[0] = 0x16;
					DataPush(buf, 2, DEV_DIAODU, DEV_PHONE, LV2);
				}
			}
			else if( NETWORK_TYPE==NETWORK_TD )
			{
				if (m_curkey.key==KEY_END && ( g_objPhoneTd.GetPhoneHdlSta()==PST_DIAL || g_objPhoneTd.GetPhoneHdlSta()==PST_WAITPICK || g_objPhoneTd.GetPhoneHdlSta()==PST_HOLD ) )
				{ 
					byte buf[2] = {0};
					buf[0] = 0x16;
					DataPush(buf, 2, DEV_DIAODU, DEV_PHONE, LV2);
				}
			}
			else if( NETWORK_TYPE==NETWORK_EVDO )
			{
				if (m_curkey.key==KEY_END && ( g_objPhoneEvdo.GetPhoneHdlSta()==PST_DIAL || g_objPhoneEvdo.GetPhoneHdlSta()==PST_WAITPICK || g_objPhoneEvdo.GetPhoneHdlSta()==PST_HOLD ) )
				{ 
					byte buf[2] = {0};
					buf[0] = 0x16;
					DataPush(buf, 2, DEV_DIAODU, DEV_PHONE, LV2);
				}
			}
			else if( NETWORK_TYPE==NETWORK_WCDMA )
			{
				if (m_curkey.key==KEY_END && ( g_objPhoneWcdma.GetPhoneHdlSta()==PST_DIAL || g_objPhoneWcdma.GetPhoneHdlSta()==PST_WAITPICK || g_objPhoneWcdma.GetPhoneHdlSta()==PST_HOLD ) )
				{ 
					byte buf[2] = {0};
					buf[0] = 0x16;
					DataPush(buf, 2, DEV_DIAODU, DEV_PHONE, LV2);
				}
			}

			if ((m_curkey.key >= '0' && m_curkey.key <= '9') ||	m_curkey.key == '+' || m_curkey.key == '*')
			{
				Handler_KEY_DIGITAL();
			}
		}
	}
}

void CHstApp::ActivateMainWin(void)
{
    g_hst.PushWinStack(&WinStackEntry_MainWin);
}

void CHstApp::InitMainWin(void)
{
    m_KBIsLocked   = false;
    m_WaitKEY_Star = false;
	WinStackEntry_MainWin.InitProc = InitProc_MainWin;
	WinStackEntry_MainWin.DestroyProc = 0;
	WinStackEntry_MainWin.SuspendProc = SuspendProc_MainWin;
	WinStackEntry_MainWin.ResumeProc = ResumeProc_MainWin;
	WinStackEntry_MainWin.HdlKeyProc = HdlKeyProc_MainWin;
}

/************VirPhone************************************************************/
bool CHstApp::Display_DTMF(void)
{
    INT8U screensize, i, pos, dplylen;

    screensize = g_hst.GetScreenSize();
    if (m_dtmflen > screensize)
        dplylen = screensize;
    else
        dplylen = m_dtmflen;
    if (dplylen == 0) return false;

    g_hst.VTL_ClearScreen();     /* clear screen */
    for (i = 1, pos = m_dtmfpos; i <= dplylen; i++) {
        g_hst.VTL_AbsMovCursor(screensize - i);
        if (pos > 0)
            pos--;
        else
            pos = sizeof(m_dtmfbuf) - 1;
        g_hst.VTL_ScreenPrint(m_dtmfbuf[pos]);
    }

    g_hst.OverWriteScreen();	/* overwrite screen */

    return true;
}

void CHstApp::AddDtmfChar(INT8U ch)
{
    m_dtmfbuf[m_dtmfpos++] = ch;

    if (m_dtmfpos >= sizeof(m_dtmfbuf)) 
		m_dtmfpos = 0;
    if (m_dtmflen < sizeof(m_dtmfbuf))
		m_dtmflen++;
}

void CHstApp::DelDtmfChar(void)
{
    if (m_dtmflen > 0)
	{
        if (m_dtmfpos == 0)
            m_dtmfpos = sizeof(m_dtmfbuf) - 1;
        else
            m_dtmfpos--;
       
		m_dtmflen--;
    }
}
void CHstApp::ModifyDtmfChar(INT8U ch)
{
    DelDtmfChar();
    AddDtmfChar(ch);
}

bool CHstApp::HaveDtmfChar(void)
{
    if (m_dtmflen > 0) 
		return true;
    else
		return false;
}

void CHstApp::InitDtmfBuf(void)
{
    m_dtmflen = m_dtmfpos = 0;
	memset(m_dtmfbuf,0x00,sizeof(m_dtmfbuf));//cyh add 20081222
}

void CHstApp::GetDisplayPara(void)
{
    m_dplylen_phone = GetTBNameByTel(m_dplybuf_phone, EntryTel.tellen, EntryTel.tel);
    
	if (m_dplylen_phone > 0)
	{
		memcpy(m_dplyptr_phone, m_dplybuf_phone, m_dplylen_phone);

    } 
	else
	{
        m_dplylen_phone = EntryTel.tellen;
		memcpy(m_dplyptr_phone, EntryTel.tel, EntryTel.tellen);
    }
}

void CHstApp::Display_COMMON(INT8U textlen, char *textptr, INT8U tellen, INT8U *tel)
{
    INT8U screensize;

    screensize = g_hst.GetScreenSize();
    g_hst.VTL_ClearScreen();                                      /* clear screen */
    g_hst.VTL_ScreenPrintMem((INT8U *)textptr, textlen);
   
	if (textlen + tellen < screensize) 
	{
        g_hst.VTL_AbsMovCursor(screensize - tellen);
        g_hst.VTL_ScreenPrintMem(tel, tellen);
    }

    g_hst.OverWriteScreen();	/* overwrite screen */
}

void CHstApp::Display_CALLING(void)
{
    GetDisplayPara();
    Display_COMMON(strlen("拨号…"), "拨号…", m_dplylen_phone, m_dplyptr_phone);
}

void CHstApp::Display_CALLING_INIT(void)
{
    Display_COMMON(strlen("拨号…"), "拨号…", EntryTel.tellen, EntryTel.tel);
}

void CHstApp::Display_TALKING(void)
{
    GetDisplayPara();
    g_hst.TurnonTalkIndicator();
    if (!Display_DTMF())	//如果没有DTMF字符显示就显示电话号码
        Display_COMMON(strlen("接通…"), "接通…", m_dplylen_phone, m_dplyptr_phone);
}

void CHstApp::Display_RINGING(void)
{
    if (EntryTel.tellen == 0)
        Display_COMMON(strlen("未知来电?"), "未知来电?", 0, 0);
    else 
	{
        GetDisplayPara();
        Display_COMMON(strlen("来电?"), "来电?", m_dplylen_phone, m_dplyptr_phone);
    }
}

void CHstApp::Display_VOLUME(void)
{
    INT8U step, linesize;

		if( NETWORK_TYPE==NETWORK_GSM )
		{
			step = g_objPhoneGsm.GetPhoneVolume();
		}
		else if( NETWORK_TYPE==NETWORK_TD )
		{
			step = g_objPhoneTd.GetPhoneVolume();
		}
		else if( NETWORK_TYPE==NETWORK_EVDO )
		{
			step = g_objPhoneEvdo.GetPhoneVolume();
		}
		else if( NETWORK_TYPE==NETWORK_WCDMA )
		{
			step = g_objPhoneWcdma.GetPhoneVolume();
		}

    linesize = g_hst.GetScreenLineSize();

    g_hst.VTL_ClearScreen();
    g_hst.VTL_ScreenPrintStr("音量\n");
    g_hst.VTL_ScreenPrintStr("-----------+");
    g_hst.VTL_AbsMovCursor(linesize + 1);
	g_hst.VTL_ScreenPrintChar(0x83, step*2);  // edit by tl 原来音量显示只有一半，不能显示全部

    g_hst.OverWriteScreen();

	sleep(1);
	Status_Phone();
}

void CHstApp::Display_CHANNEL(void)
{	
    g_hst.VTL_ClearScreen();

    if (!m_HstCfg.m_HandsfreeChannel)
	{	
         g_hst.VTL_ScreenPrintStr("切换到\n手柄模式");
    } 
	else 
	{
         g_hst.VTL_ScreenPrintStr("切换到\n免提模式");
    }

    g_hst.OverWriteScreen();

	usleep(1000);
	Status_Phone();
}

void CHstApp::Status_Phone(void)
{    
	if (m_status_phone == VIR_TALKING) 
	{
        Display_TALKING();
    } 
	else if (m_status_phone == VIR_CALLING)
	{
        Display_CALLING();
    } 
	else if (m_status_phone == VIR_RINGING)
	{
		Display_RINGING();
    }
}

void CHstApp::ExitVirPhone(char *str, INT8U time)
{
    g_hst.ActivateLowestWinStack();	//使界面返回到待机界面

    memset(&PromptEntry_Phone, 0, sizeof(PromptEntry_Phone));
    PromptEntry_Phone.Mode     = PROMPT_NOKEY;
    PromptEntry_Phone.LiveTime = time;

    if (str == 0) 
	{
        PromptEntry_Phone.TextPtr  = dplytime_phone;
        PromptEntry_Phone.TextLen  = sizeof(dplytime_phone) - 1;
    }
	else
	{
        PromptEntry_Phone.TextPtr  = (INT8U *)str;
        PromptEntry_Phone.TextLen  = strlen(str);
    }

    if (time > 0) 
	{
		g_hst.ActivatePrompt(&PromptEntry_Phone);	//激活通话提示窗口
	}

	m_hangupstate = 0;
}

void CHstApp::InitProc_Dial(void) 
{
    INT8U *tel, tellen;

    EntryTel.attrib = TALK_DIAL;
  
    m_status_phone = VIR_FREE;

    tel    = EntryTel.tel;
    tellen = EntryTel.tellen;

    if (tellen > 3 && tel[0] == '+')
	{
        tel    += 3;
        tellen -= 3;
    }

	if (m_hangupstate == 1) 
	{	//挂机成功,通话结束
		ExitVirPhone("  通话结束", 3);
		//hxd 090806 
		byte buf[2] = {0};
		buf[0] = 0x16;
		DataPush(buf, 1, DEV_DIAODU, DEV_PHONE, LV2);	
	} 
	else
	{
	    m_status_phone = VIR_CALLING;
        Display_CALLING_INIT();
    }
}

void CHstApp::InitProc_Ring(void)
{
    EntryTel.attrib = TALK_MISS;

    g_hst.RequestRing((char)m_HstCfg.m_bytcallring, 1);  /* request ring beeper */
    m_status_phone = VIR_RINGING;
   
	Display_RINGING();
}

void CHstApp::DestroyProc_Phone(void)
{
	INT8U buf[1];

	if (m_status_phone != VIR_FREE) 
	{	
		buf[0] = 0x16;	//挂机
		DataPush(buf, 1, DEV_DIAODU, DEV_PHONE, LV2);
	}

    g_hst.TurnoffTalkIndicator();  /* turn off talk indicator */
}

void CHstApp::Handler_CALLING(INT8U phonestatus)
{
	INT8U buf[2];

	// 若正在通话中
    if (phonestatus == PST_HOLD) 
	{	
		g_iNeedPhoneTip = 1;
        m_status_phone = VIR_TALKING;
        InitDtmfBuf();
        Display_TALKING();
    } 
	else if(phonestatus == PST_IDLE	|| (m_IsPDkey && m_curkey.key == KEY_END) )
	{
        if (m_curkey.key == KEY_END)
		{
			ExitVirPhone(0, 10);

			buf[0] = 0x16;
			DataPush(buf, 1, DEV_DIAODU, DEV_PHONE, LV2);
	
			g_iNeedPhoneTip = 0;
        }
    }
}

void CHstApp::Handler_TALKING(INT8U phonestatus)
{
	INT8U buf[2];

	if (m_diatm == 0)
		m_diatm = GetTickCount();

    if (phonestatus == PST_IDLE || (m_IsPDkey && m_curkey.key == KEY_END) )
	{
		//如果按下C键并且有DTMF字符
        if (m_curkey.key == KEY_END && HaveDtmfChar())
		{	
            DelDtmfChar();			//删除DTMF字符
            Display_TALKING();		//显示通话状态
        } 
		else	//如果按下C键没有DTMF字符则挂机 
		{				
            if (m_curkey.key == KEY_END) 
			{
				g_iNeedPhoneTip = 1;
				m_status_phone = VIR_FREE;

				buf[0] = 0x16;	//挂机
				DataPush(buf, 1, DEV_DIAODU, DEV_PHONE, LV2);
			}
        }
    } 
	else if(m_IsPDkey && ((m_curkey.key >= '0' && m_curkey.key <= '9') ||
			m_curkey.key == '*' || m_curkey.key == '#' || m_curkey.key == '+')) 
	{
        if (m_curkey.key == '+') 
			m_curkey.key = '*';
		
		buf[0] = 0x10;
		buf[1] = m_curkey.key;
		DataPush(buf, 2, DEV_DIAODU, DEV_PHONE, LV2);		//发送DTMF请求

        AddDtmfChar(m_curkey.key);

    }
	else if (m_IsPDkey && m_curkey.key == KEY_CLRALL && HaveDtmfChar()) 
	{
        InitDtmfBuf();
        Display_TALKING();
    } 
	else if (m_IsPDkey && m_curkey.key == KEY_UP) 
	{
		buf[0] = 0x28;
		buf[1] = 0x00;
		DataPush(buf, 2, DEV_DIAODU, DEV_PHONE, LV2);
    } 
	else if (m_IsPDkey && m_curkey.key == KEY_DOWN) 
	{
		buf[0] = 0x28;
		buf[1] = 0x01;	//减小音量
		DataPush(buf, 2, DEV_DIAODU, DEV_PHONE, LV2);
    } 
	else if (m_IsPDkey && m_curkey.key == KEY_VOICE) 
	{
		buf[0] = 0x26;	//免提耳机切换
		DataPush(buf, 1, DEV_DIAODU, DEV_PHONE, LV2);

		m_HstCfg.m_HandsfreeChannel = !m_HstCfg.m_HandsfreeChannel;
    }
}

void CHstApp::Handler_RINGING(INT8U phonestatus)
{
	g_iNeedPhoneTip = 0; 

	char buf[1];

	//对方挂机(此时电话模块处于空闲状态)或己方挂机
    if( phonestatus == PST_IDLE || (m_IsPDkey && m_curkey.key == KEY_END) )
	{	
		//来电挂机
        if (m_curkey.key == KEY_END)
		{	
            m_status_phone			= VIR_FREE;
            EntryTel.attrib			= TALK_RECV;

            buf[0] = 0x16;
			DataPush(buf, 1, DEV_DIAODU, DEV_PHONE, LV2);
        }
		else //来电未接
		{						
            HstMiss.NumTel++;
			PRTMSG(MSG_NOR, "Miss Tel ++\n");
        }
		m_state_tel = 1;
    }
	//如果处于拨号,通话或者链路切断状态
	else if(phonestatus == PST_DIAL || phonestatus == PST_WAITPICK || phonestatus == PST_CUTNET || phonestatus == PST_DIALNET)
	{
        m_status_phone = VIR_FREE;
        g_hst.CancelRing();
    }
	//如果接通
	else if (m_IsPDkey && m_curkey.key == KEY_SND)
	{
        EntryTel.attrib			= TALK_RECV;
        g_hst.CancelRing();
	
		buf[0] = 0x14;	//摘机
		DataPush(buf, 1, DEV_DIAODU, DEV_PHONE, LV2);
	
		g_iNeedPhoneTip = 1;
    }
}

void CHstApp::HdlEventProc(INT8U phonestatus)
{
    if (m_status_phone > VIR_RINGING /*|| (!m_activated)*/) 
	{
        g_hst.ActivateLowestWinStack();
        return;
    }

    switch (m_status_phone)
    {
        case VIR_CALLING:
            Handler_CALLING(phonestatus);
            break;
        case VIR_TALKING:
            Handler_TALKING(phonestatus);
            break;
        case VIR_RINGING:
            Handler_RINGING(phonestatus);
            break;
        default:
			{
				//如果是按下“C”键，则挂断电话
				if (m_IsPDkey && KEY_NAK==m_curkey.key)
				{
					byte buf[2] = {0};
					buf[0] = 0x16;
					DataPush(buf, 1, DEV_DIAODU, DEV_PHONE, LV2);
				}
			}
            break;
    }
}

void CHstApp::HdlKeyProc_Phone(void)
{
	m_IsPDkey = g_hst.GetKeyMsg(&m_curkey);
		
	if (m_IsPDkey)
	{
		if( NETWORK_TYPE==NETWORK_GSM )
		{
			HdlEventProc( g_objPhoneGsm.GetPhoneHdlSta() );
		}
		else if( NETWORK_TYPE==NETWORK_TD )
		{
			HdlEventProc( g_objPhoneTd.GetPhoneHdlSta() );
		}
		else if( NETWORK_TYPE==NETWORK_EVDO )
		{
			HdlEventProc( g_objPhoneEvdo.GetPhoneHdlSta() );
		}
		else if( NETWORK_TYPE==NETWORK_WCDMA )
		{
			HdlEventProc( g_objPhoneWcdma.GetPhoneHdlSta() );
		}
	}
}

void CHstApp::ActivateVirPhone_Dial(bool findsk, INT8U tellen, INT8U *tel)
{
	m_IsReturnMainWin = false;
	memset(&EntryTel, 0, sizeof(EntryTel));

	INT8U buf_tel[50] = { 0 };
	buf_tel[0] = 0x12;	//拨号请求

    if (m_HstCfg.m_HandsfreeChannel)
	{
        EntryTel.channel = CHA_HANDSFREE;
    }
	else
	{
        EntryTel.channel = CHA_HANDSET;
    }

	{
		if( NETWORK_TYPE==NETWORK_GSM )
		{
			if( g_objPhoneGsm.GetPhoneHdlSta() != PST_IDLE )
			{
				static char szTip[] = "对不起\n手机忙";
				g_hst.ActivateStringPrompt(szTip);
				return;
			}
		}
		else if( NETWORK_TYPE==NETWORK_TD )
		{
			if( g_objPhoneTd.GetPhoneHdlSta() != PST_IDLE )
			{
				static char szTip[] = "对不起\n手机忙";
				g_hst.ActivateStringPrompt(szTip);
				return;
			}
		}
		else if( NETWORK_TYPE==NETWORK_EVDO )
		{
			if( g_objPhoneEvdo.GetPhoneHdlSta() != PST_IDLE )
			{
				static char szTip[] = "对不起\n手机忙";
				g_hst.ActivateStringPrompt(szTip);
				return;
			}
		}
		else if( NETWORK_TYPE==NETWORK_WCDMA )
		{
			if( g_objPhoneWcdma.GetPhoneHdlSta() != PST_IDLE )
			{
				static char szTip[] = "对不起\n手机忙";
				g_hst.ActivateStringPrompt(szTip);
				return;
			}
		}

		bool ishurrydia =  ( strcmp((char *)tel, "110") == 0 ) ||
							( strcmp((char *)tel, "112") == 0 ) ||
							( strcmp((char *)tel, "119") == 0 ) ||
							( strcmp((char *)tel, "120") == 0 ) ||
							( strcmp((char *)tel, "122") == 0 );

		if (ishurrydia || m_IsRefuseDial ) // 不做号码长度限制判断,因为不考虑拨打集团网小号,xun,081022
		{
			if (tellen > sizeof(EntryTel.tel))
			{
				tellen = sizeof(EntryTel.tel);
			}

			EntryTel.tellen  = tellen;
			memcpy(EntryTel.tel, tel, tellen);

			buf_tel[1] = EntryTel.tellen;
			memcpy(buf_tel+2, EntryTel.tel, EntryTel.tellen);
			
			WinStackEntry_Dia.DestroyProc = DestroyProc_Phone;
			WinStackEntry_Dia.HdlKeyProc = HdlKeyProc_Phone;
			WinStackEntry_Dia.ResumeProc = 0;
			WinStackEntry_Dia.SuspendProc = 0;
			DataPush((char *)buf_tel, (DWORD)(2+buf_tel[1]), DEV_DIAODU, DEV_PHONE, 1);
			
			m_hangupstate = 0;

			WinStackEntry_Dia.InitProc = InitProc_Dial;
			g_hst.PushWinStack(&WinStackEntry_Dia);
		}
		else
		{
			g_hst.ActivateStringPrompt("对不起\n拨号错误");
		}
    }
}

void CHstApp::ActivateVirPhone_DialSuccor(INT8U tellen, INT8U *tel)
{
	m_IsReturnMainWin = false;
	memset(&EntryTel, 0, sizeof(EntryTel));
    EntryTel.channel = CHA_HANDSFREE;

    if (tellen > sizeof(EntryTel.tel))
		tellen = sizeof(EntryTel.tel);

    EntryTel.tellen  = tellen;
    memcpy(EntryTel.tel, tel, tellen);

	WinStackEntry_DialSuccor.DestroyProc = DestroyProc_Phone;
	WinStackEntry_DialSuccor.HdlKeyProc = HdlKeyProc_Phone;
	WinStackEntry_DialSuccor.ResumeProc = 0;
	WinStackEntry_DialSuccor.SuspendProc = 0;
    WinStackEntry_DialSuccor.InitProc = InitProc_Dial;
    g_hst.PushWinStack(&WinStackEntry_DialSuccor);
}

void CHstApp::ActivateVirPhone_Ring(void)
{
	m_IsReturnMainWin = false;
	memset(&EntryTel, 0, sizeof(EntryTel));
	
	//如果是网内电话，隐藏前面的7位号段（待讨论）
	if (strncmp((char *)(m_tempbuf+1), "1509957", 7) == 0) 
	{	
		char buf[10] = {0};
		memcpy(buf, m_tempbuf+8, 4);
		memcpy(EntryTel.tel, buf, 4);
		EntryTel.tellen = 4;
	} 
	else
	{
		EntryTel.tellen = m_tempbuf[0];

		if (EntryTel.tellen > sizeof(EntryTel.tel))
			EntryTel.tellen = sizeof(EntryTel.tel);
	
		memcpy(EntryTel.tel, m_tempbuf+1, EntryTel.tellen);
	}

	WinStackEntry_Ring.DestroyProc = DestroyProc_Phone;
	WinStackEntry_Ring.HdlKeyProc = HdlKeyProc_Phone;
	WinStackEntry_Ring.ResumeProc = 0;
	WinStackEntry_Ring.SuspendProc = 0;
    WinStackEntry_Ring.InitProc = InitProc_Ring;
    g_hst.PushWinStack(&WinStackEntry_Ring);
}

/************CallMan************************************************************/
void CHstApp::OPREC_DIAL(void)	//呼出
{
	ActivateVirPhone_Dial(false, CurRecord.tellen, (INT8U *)CurRecord.tel);
}

void CHstApp::OPREC_READ(void)	//读取通话记录
{
	char dply_tk[100] = {0};
	
	if (m_talktype != TALK_MISS)
	{
		strcat(dply_tk, "共通话:\n  ");
		strcat(dply_tk, CurRecord.talktime);
		strcat(dply_tk, "\n时间:");
	} 
	else
	{
		strcat(dply_tk, "时间:");
	}
	strncat(dply_tk, CurRecord.time, 5);
	strcat(dply_tk, "\n");
	strncat(dply_tk, CurRecord.date, 10);

	g_hst.ActivateStringPrompt_TIME((char *)dply_tk, 0);
}

void CHstApp::ACK_OP_ERASE(void)
{
    INT16U DelRec;

    PRTMSG(MSG_NOR, "ACK_OP_ERASE\n");
    g_hst.PopMenuWin();
    DelRec = g_hst.GetMenuWinRecordNum();
    g_hst.PopMenuWin();
	PRTMSG(MSG_NOR, "DelRec = %d\n", DelRec);

    g_hst.ActivateStringPrompt("删除中…");
    DelTalkRecord(m_talktype, DelRec);
    g_hst.ActivateStringPrompt("已删除");
}

void CHstApp::OPREC_ERASE(void)	//删除通话记录
{
	char str_tk[] = "删除?";
    g_hst.ActivateSelPrompt_TIME(str_tk, 5, ACK_OP_ERASE, 0);
}

void CHstApp::OPREC_DISPLAY(void)	//显示电话号码
{
    INT8U len;
    
    len = CurRecord.tellen;
    if (len > 0) 
	{
        memcpy(m_dplybuf, CurRecord.tel, len);
        m_dplybuf[len] = '\0';
        g_hst.ActivateStringPrompt_TIME((char *)m_dplybuf, 0);
    } 
	else 
	{
        g_hst.ActivateStringPrompt_TIME("(空白)", 0);
    }
}


void CHstApp::OPREC_SAVE(void)	//保存到电话本
{
    SaveToTB((INT8U *)CurRecord.tel, CurRecord.tellen);
}

void CHstApp::Handler_KEY_ACK_CallMan(void)	//进入通话记录相应的菜单后按下OK键
{
	memset(&currecord,0x00,sizeof(currecord));
	memset(&CurRecord,0x00,sizeof(CurRecord));
	memset(&CurRecord_SM,0x00,sizeof(CurRecord_SM));

	//如果没有通话记录
    if (GetTalkRecord(m_talktype, g_hst.GetMenuWinRecordNum(), &CurRecord) == 0) 
        g_hst.PopMenuWin();
	//如果有,则激活该通话记录的菜单项
    else 
        g_hst.ActivateItemMenu(&MENU_OPREC_CM);	
}

INT16U CHstApp::MaxRecordProc_CallMan(void)	//获取通话记录的数量
{
    return GetNumTalkRecord(m_talktype);
}

INT16U CHstApp::ReadRecordProc_CallMan(INT16S numrec, INT8U *ptr) //读通话记录
{
    char screensize, len, templen, dplylen, *dplyptr;
    
    if (numrec == -1)
	{
        memcpy(ptr, "无通话记录", strlen("无通话记录"));
        return strlen("无通话记录");
    }

    if (GetTalkRecord(m_talktype, numrec, &CurRecord) == 0) return 0;

    screensize = g_hst.GetScreenSize();

    if (m_talktype == TALK_MISS) 
	{
        len = strlen("未接");
        memcpy(ptr, "未接", len);
    } 
	else if (m_talktype == TALK_DIAL) 
	{
        len = strlen("已拨");
        memcpy(ptr, "已拨", len);
    }
	else
	{
        len = strlen("已接");
        memcpy(ptr, "已接", len);
    }

    ptr    += len;
    templen = DecToAscii(ptr, numrec + 1, 0);
    len    += templen;
    ptr    += templen;
    *ptr++  = ':';
    len++;
    
    if (CurRecord.tellen == 0)
	{
        dplylen = strlen("未知电话");
        dplyptr = "未知电话";
    } 
	else
	{
        dplylen = GetTBNameByTel((INT8U *)m_dplybuf, CurRecord.tellen, (INT8U *)CurRecord.tel);
	
		if (dplylen == 0) 
		{
            dplylen = CurRecord.tellen;
            dplyptr = CurRecord.tel;
        } 
		else
		{
            dplyptr = m_dplybuf;
        }
		
    }

    if (len + dplylen < screensize)
	{
        templen = screensize - (len + dplylen);  //计算要补充多少个空格才刚好满屏
        memset(ptr, ' ', templen);
        len += templen;
        ptr += templen;
    } 
	else
	{
        *ptr++ = '\n';
        len++;
    }

    memcpy(ptr, dplyptr, dplylen);
    return len + dplylen;
}

void CHstApp::SelTalkProc(void)
{
    INT8U index;
    
    index = g_hst.GetMenuWinRecordNum();

    if (index == 0) 
        m_talktype = TALK_MISS;
    else if (index == 1) 
        m_talktype = TALK_DIAL;
    else 
        m_talktype = TALK_RECV;

    g_hst.ActivateMenuWin(&SelMenuWinEntry);
}

void CHstApp::ACK_DelTalkProc(void)
{
    INT8U index;
    
    index = g_hst.GetMenuWinRecordNum();
    g_hst.PopMenuWin();
    g_hst.ActivateStringPrompt("删除中…");

    if (index == 3)
	{
        DelAllTalkRecord(TALK_DIAL);
		sleep(1);
        DelAllTalkRecord(TALK_MISS);
		sleep(1);
        DelAllTalkRecord(TALK_RECV);
    }
	else if (index == 0)
	{
        DelAllTalkRecord(TALK_DIAL);
    } 
	else if (index == 1) 
	{
        DelAllTalkRecord(TALK_MISS);
    } 
	else if (index == 2) 
	{
        DelAllTalkRecord(TALK_RECV);
    }

    g_hst.ActivateStringPrompt("已删除");
}

void CHstApp::DelTalkProc(void)
{
	char buf[] = {"删除?"};
    g_hst.ActivateSelPrompt_TIME(buf, 5, ACK_DelTalkProc, 5);
}

void CHstApp::ActivateDialTalkRec(void)
{
    m_talktype = TALK_DIAL;
    g_hst.ActivateMenuWin(&SelMenuWinEntry);
}

void CHstApp::ListMissTel(void)
{
    m_talktype = TALK_MISS;
    g_hst.ActivateMenuWin(&SelMenuWinEntry);    
}

void CHstApp::ExitPromptReadMissTel(void)
{
    HstMiss.NumTel = 0;
}

bool CHstApp::PromptReadMissTel(void)
{
    INT8U len;

	PromptEntry_CM.Mode = PROMPT_ACKINFORM;
	PromptEntry_CM.LiveTime = 0;
	PromptEntry_CM.ExitProc = ListMissTel;
	PromptEntry_CM.ExitKeyProc = ExitPromptReadMissTel;
    
    if (HstMiss.NumTel == 0) 
		return false;

    len  = DecToAscii((INT8U *)m_dplybuf, HstMiss.NumTel, 0);
    memcpy(&m_dplybuf[len], "个\n未接电话", strlen("个\n未接电话"));
    len += strlen("个\n未接电话");

    PromptEntry_CM.TextPtr = (INT8U *)m_dplybuf;
    PromptEntry_CM.TextLen = len;
    g_hst.ActivatePrompt(&PromptEntry_CM);

    return true;
}

/*************TelBook***********************************************************/
void CHstApp::AddRec(void)
{
    g_hst.ActivateStringPrompt("存储中…");

    if (AddTBRec(&currecord))
        g_hst.ActivateStringPrompt("已存");
    else
        g_hst.ActivateStringPrompt("存储已满");

	memset(&currecord,0x00,sizeof(currecord));
	memset(&CurRecord,0x00,sizeof(CurRecord));
	memset(&CurRecord_SM,0x00,sizeof(CurRecord_SM));
}

void CHstApp::ACK_AddRec(void)
{
    g_hst.ActivateStringPrompt("存储中…");
    DelTBRec(m_selrecord_tb);
    AddRec();
}

void CHstApp::ModifyRec(void)
{
    g_hst.ActivateStringPrompt("存储中…");

    if (ModifyTBRec(m_selrecord_tb, &currecord)) 
	{
        g_hst.ActivateStringPrompt("已修改");
	}
    else
	{
        g_hst.ActivateStringPrompt("修改失败");
	}

	memset(&currecord,0x00,sizeof(currecord));//cyh add:add
}

void CHstApp::ACK_ModifyRec(void)
{
	if (-1==g_iIndexSelRec)
	{
		g_hst.ActivateStringPrompt("修改失败");
		g_iIndexSelRec = -1;
		g_iIndexRptRec = -1;

		return;
	}

	DelTBRec(g_iIndexSelRec);  //先删除选中的记录，一定要放在前面，因为删除后，原先保存的索引将不再有效了
	DelTBRecByName((char*)currecord.name);
	AddRec();
}

void CHstApp::ExitPhoneEditor(void)
{
    INT8U   len;
    INT16S  rec;

	memset(currecord.tel,0,sizeof(currecord.tel));
    
	if ((currecord.tellen = g_hst.GetEditorData(currecord.tel)) > 0) 
	{
		//如果是修改,先检查有没有重名的
        if (m_op_type_tb == OP_MODIFY)
		{	
			//将编辑名字的窗口先pop掉，等下直接返回到电话本的添加、删除、修改界面
            g_hst.PopMenuWin();  
			
			rec = CheckNameExist(currecord.namelen,(char*)currecord.name);  //cyh add:add
			
			//刚增加完没有退出编辑状态时如果还需要对刚刚编辑的名字再次改动,此时要比较刚刚存入的电话姓名          
			if (rec != -1 && rec != m_selrecord_tb)
			{   
				len = strlen("替换?\n");
                memcpy(m_dplybuf, "替换?\n", len);
                memcpy(m_dplybuf + len, currecord.name, currecord.namelen);
				g_iIndexRptRec = rec;
				g_iIndexSelRec = m_selrecord_tb;
                g_hst.ActivateSelPrompt(m_dplybuf, currecord.namelen + len, ACK_ModifyRec);
            }
			else
			{
                ModifyRec();
            }
        } 
		else 
		{	//如果是增加,则先检查增加的有没有重名的
            m_selrecord_tb = (int)GetTBRecByName_FULL(currecord.namelen, currecord.name);
			
            if (m_selrecord_tb != -1)
			{
                len = strlen("替换?\n");
                memcpy(m_dplybuf, "替换?\n", len);
                memcpy(m_dplybuf + len, currecord.name, currecord.namelen);
                g_hst.ActivateSelPrompt(m_dplybuf, currecord.namelen + len, ACK_AddRec);
            } 
			else
			{
                AddRec();
            }
        }
    }
}

void CHstApp::ActivatePhoneEditor(void)
{
	EDITOR_STRUCT PhoneEditor =  {EDT_DIGITAL | EDT_PULS | EDT_CURSOR, 1, TBREC_SIZE_TEL, 0, "请输号码:", 0, 0, 0, ExitPhoneEditor, 0, 0, 0, 0, 0};

	if (!strlen((char*)currecord.tel) ) 
	{
		memcpy(currecord.tel, (const char *)CurRecord.tel,strlen((char*)CurRecord.tel));		
	}
	if (!strlen((char*)currecord.tel) ) 
	{
		memcpy(currecord.tel, (const char *)CurRecord_SM.tel,strlen((char*)CurRecord_SM.tel));		
	}

    PhoneEditor.InitPtr = currecord.tel;
	PhoneEditor.InitLen = strlen((char*)currecord.tel);
    g_hst.CreateEditor(&PhoneEditor);
}

void CHstApp::ExitNameEditor(void)
{
    INT16S findrec;
    
	memset( currecord.name, 0, sizeof(currecord.name) );
    
	if ((currecord.namelen = g_hst.GetEditorData(currecord.name)) > 0) 
	{
        if (m_op_type_tb != OP_FIND)
		{
			ActivatePhoneEditor();
		}
        else
		{
            if ((findrec = GetTBRecByName_FULL(currecord.namelen, currecord.name)) == -1) //hxd
			{
				findrec = 0;
				PROMPT_STRUCT m_prmpt;

				char strPro[] = "没有相关记录";
				m_prmpt.Mode        = 0;
				m_prmpt.LiveTime    = 2;
				m_prmpt.TextPtr     = (INT8U *)strPro;
				m_prmpt.TextLen     = 12;
				m_prmpt.ExitProc    = 0;
				m_prmpt.ExitKeyProc = 0;

				g_hst.ActivatePrompt(&m_prmpt);

				return;
			}
            ListTB_Init(findrec);
        }
    }
}
 
void CHstApp::ActivateNameEditor(void)
{
	EDITOR_STRUCT NameEditor = {EDT_CURSOR | EDT_CHINESE, 1, TBREC_SIZE_NAME, 0, "姓名：", 0, 0, 0, ExitNameEditor, 0, 0, 0, 0, 0};//cyh add:caution 初始化后是一个指针指向字符串"姓名:"，而不是做字符串拷贝，这样有可能会带来问题
	
	memset(&currecord,0x00,sizeof(currecord));
	
	NameEditor.InitPtr = currecord.name;
	NameEditor.InitLen = strlen((char*)currecord.name);

    g_hst.CreateEditor(&NameEditor);
}

void CHstApp::ActivateNameEditor_Md(void)
{
	EDITOR_STRUCT NameEditor = {EDT_CURSOR | EDT_CHINESE, 1, TBREC_SIZE_NAME, 0, "姓名：", 0, 0, 0, ExitNameEditor, 0, 0, 0, 0, 0};//cyh add:caution 初始化后是一个指针指向字符串"姓名:"，而不是做字符串拷贝，这样有可能会带来问题
	
	NameEditor.MaxLen  = 10;
	NameEditor.InitPtr = currecord.name;
    
	NameEditor.InitLen = min(strlen((char*)currecord.name),TBREC_SIZE_NAME);
    g_hst.CreateEditor(&NameEditor);
}

void CHstApp::AddTB(void)	//增加到电话本
{
    m_op_type_tb = OP_ADD;
    currecord.namelen = 0;
    currecord.tellen  = 0;

	memset(&currecord,0x00,sizeof(currecord));
	memset(&CurRecord,0x00,sizeof(CurRecord));
	memset(&CurRecord_SM,0x00,sizeof(CurRecord_SM));

    ActivateNameEditor();
}

void CHstApp::ACK_DelProc(void)	
{
    g_hst.PopMenuWin();
    g_hst.ActivateStringPrompt("删除中…"); 
    DelTBRec(m_selrecord_tb);
    g_hst.ActivateStringPrompt("已删除");
}

void CHstApp::DelProc(void)
{
	char buf[] = {"删除?"};
    g_hst.ActivateSelPrompt(buf, 0, ACK_DelProc);
}

void CHstApp::ModifyProc(void)
{
    m_op_type_tb = OP_MODIFY;

	memset(&currecord,0,sizeof(currecord));

    if (GetTBRec(m_selrecord_tb, &currecord) == 0) 
        g_hst.PopMenuWin();
    else 
		ActivateNameEditor_Md();
}

void CHstApp::DialProc(void)
{    
	if (GetTBRec(m_selrecord_tb, &currecord))
	{
		ActivateVirPhone_Dial(false, currecord.tellen, currecord.tel);
	}
}


void CHstApp::Handler_KEY_ACK_TelBook(void)
{
    m_selrecord_tb = g_hst.GetMenuWinRecordNum();	//获取选中的菜单项,比如需要编辑或的电话号码名
    g_hst.ActivateItemMenu(&MENU_OPREC_TB);
}

void CHstApp::Handler_KEY_ACK_RET(void)
{
    INT16U num;
    
    num = g_hst.GetMenuWinRecordNum();
    g_hst.PopMenuWin();

    if (GetTBRec(num, &currecord)) 
	{
        if (RetInformer != 0) 
            RetInformer(currecord.tellen, currecord.tel, currecord.namelen, currecord.name);
    } 
	else if (RetInformer != 0)
	{
        RetInformer(0, 0, 0, 0);
    }
}

INT16U CHstApp::MaxRecordProc_TelBook(void)
{
    return GetTBItem();
}

INT16U CHstApp::InitRecordProc_TelBook(void)
{
    return m_selrecord_tb;
}

INT16U CHstApp::ReadRecordProc_TelBook(INT16S numrec, INT8U *ptr)
{
    INT16U linesize;
    
    if (numrec == -1) 
	{
        memcpy(ptr, "无号码", strlen("无号码"));
        return strlen("无号码");
    }

    if (GetTBRec(numrec, &readrecord) == 0) return 0;

    memcpy(ptr, readrecord.name, readrecord.namelen);
    ptr   += readrecord.namelen;
    *ptr++ = '\n';
    
    linesize = g_hst.GetScreenLineSize();

    if (linesize > readrecord.tellen)
		linesize -= readrecord.tellen;
    else
		linesize = 0;

	memcpy(ptr, readrecord.tel, readrecord.tellen);
	ptr+=readrecord.tellen;
    memset(ptr, ' ', linesize);    
	
    return (readrecord.namelen + 1 + linesize + readrecord.tellen);
}

void CHstApp::ListTB_Init(INT16U initrecord)
{
	ListTBEntry.WinAttrib = WIN_FMENU;
	ListTBEntry.Handler_KEY_ACK = Handler_KEY_ACK_TelBook;
	ListTBEntry.Handler_KEY_FCN = 0;
	ListTBEntry.Handler_KEY_NAK = 0;
	ListTBEntry.Handler_KEY_SEL = 0;
	ListTBEntry.StoreProc = 0;
	ListTBEntry.InitRecordProc = InitRecordProc_TelBook;
	ListTBEntry.MaxRecordProc = MaxRecordProc_TelBook;
	ListTBEntry.ReadRecordProc = ReadRecordProc_TelBook;
    m_selrecord_tb = initrecord;
    g_hst.ActivateMenuWin(&ListTBEntry);
}

void CHstApp::ListTB(void)
{
    ListTB_Init(0);
}

bool CHstApp::ListTB_RET(void Informer(INT8U tellen, INT8U *tel, INT8U namelen, INT8U *name))
{
	ListTBEntry.WinAttrib = WIN_FMENU;
	ListTBEntry.Handler_KEY_FCN = 0;
	ListTBEntry.Handler_KEY_NAK = 0;
	ListTBEntry.Handler_KEY_SEL = 0;
	ListTBEntry.StoreProc = 0;
	ListTBEntry.InitRecordProc = InitRecordProc_TelBook;
	ListTBEntry.MaxRecordProc = MaxRecordProc_TelBook;
	ListTBEntry.ReadRecordProc = ReadRecordProc_TelBook;
	
    if (GetTBItem() == 0) 
		return FALSE;

    m_selrecord_tb   = 0;
    RetInformer = Informer;
    ListTBEntry.Handler_KEY_ACK = Handler_KEY_ACK_RET;
    g_hst.ActivateMenuWin(&ListTBEntry);
  
	return TRUE;
}

void CHstApp::FindTB(void)
{
    m_op_type_tb = OP_FIND;
    currecord.namelen = 0;
    ActivateNameEditor();
}

void CHstApp::ACK_DelAllTB(void)
{
    g_hst.ActivateStringPrompt("删除中…");
    DelDB_TB();

    g_hst.ActivateStringPrompt("已删除所有");
}

void CHstApp::DelAllTB(void)
{
	char buf[] = {"删除所有?"};
    g_hst.ActivateSelPrompt(buf, 0, ACK_DelAllTB);
}

void CHstApp::BrowseMemory(void)	//存储状况
{
    INT8U len;
	INT8U tlen;
  
	len   = DecToAscii((INT8U *)&m_dplybuf[0], GetTBBlankItem(), 0);
    tlen  = len;
    len   = strlen("个未用\n已用");
    memcpy(&m_dplybuf[tlen], "个未用\n已用", len);
    tlen += len;

	len   = DecToAscii((INT8U *)&m_dplybuf[tlen], GetTBItem(), 0);
    tlen += len;
    len   = strlen("个");
    memcpy(&m_dplybuf[tlen], "个", len);
    tlen += len;
    m_dplybuf[tlen] = '\0';
    g_hst.ActivateStringPrompt_TIME((char *)m_dplybuf, 3);
}

void CHstApp::SaveToTB(INT8U *tel, INT8U tellen)
{
    if (tellen > sizeof(currecord.tel)) tellen = sizeof(currecord.tel);
    m_op_type_tb           = OP_ADD;
    currecord.namelen = 0;
    currecord.tellen  = tellen;
    memcpy(currecord.tel, tel, tellen);
    ActivateNameEditor();
}

/*************SaveData***********************************************************/
bool CHstApp::AddTBRec(TBREC_STRUCT *recptr)
{
	int i=0;
	char buf[30];
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;
	
	memcpy(buf, recptr->name, 10);
	memcpy(buf+10, recptr->tel, 20);
	
	SAVEDATAINDEX_STRUCT indata;
	int iSize = 0;
	iSize = sizeof(indata);

	fseek(fp, 0, SEEK_SET);
	fread(&indata, sizeof(indata), 1, fp);

	indata.num_TB++;		

	if (indata.num_TB > MAX_TBREC)
	{
		indata.num_TB = MAX_TBREC;
		return false;
	}

	char *pTelBook = NULL;
	pTelBook = new char[30*MAX_TBREC];	

	if (NULL==pTelBook)
	{
		return false;
	}

	memset(pTelBook,0x00,30*MAX_TBREC);

	for (i=0; i<MAX_TBREC; i++) //查询前面已存的空间,如果有位置没有存储(中间有被删除的)
	{	
		if (indata.flag_TB[i] == 0) //如果该位置没有被占用,则可写入
		{		
			fseek(fp, sizeof(indata)+30*i, SEEK_SET);
			fwrite(buf, 30, 1, fp);  
			indata.flag_TB[i] = 1;
			break;	//正确写入后直接退出循环,以免同一个号码多次被写
		}
	}	
	
	byte flagTB[MAX_TBREC] = {0};
	byte byIndex = 0;
	int iCnt = 0;
	size_t uiSize = 0; 	

	iCnt = 0;
	for (i = 0;i<MAX_TBREC;i++)
	{
		if (indata.flag_TB[i]!=0)
		{
			flagTB[iCnt++] = i;  //记录有效电话本的索引
		}
	}

	memset( indata.flag_TB,0x00,sizeof(indata.flag_TB) );  //清除所有电话本有效的标记
	indata.num_TB = iCnt;

	for (i = 0;i<iCnt;i++)
	{
		byIndex = flagTB[i];  //依次取出有效的索引号，移动数据
		indata.flag_TB[i] = 1;		
		fseek(fp, sizeof(indata)+30*byIndex, SEEK_SET);
		fread(pTelBook+30*i,30,1,fp);
	}

	fseek(fp, sizeof(indata), SEEK_SET);//移动到保存电话本处
	fwrite(pTelBook,30,iCnt,fp);

	fseek(fp, 0, SEEK_SET);  //移到文件头
	fwrite(&indata,sizeof(indata),1,fp);
	fclose(fp);
	delete[] pTelBook;
	pTelBook = NULL;

	return true;
}

//现将该条记录删除,然后在添加
bool CHstApp::ModifyTBRec (INT16U numrec, TBREC_STRUCT *recptr)
{
	DelTBRec(numrec);
	return AddTBRec(recptr);
}

void CHstApp::DelTBRec(INT16U numrec)
{
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	
	SAVEDATAINDEX_STRUCT indata;

	byte flagTB[MAX_TBREC] = {0};
	byte byIndex = 0;
	int iCnt = 0;
	size_t uiSize = 0; 	
	int i = 0;
	char aryTelBook[30*MAX_TBREC] = {0};

	FILE *fp = fopen(path, "rb+");		
	if (NULL==fp)
	{
		PRTMSG(MSG_ERR, "Open HandSet file err\n");
		perror("");
		return;
	}

	fread(&indata, sizeof(indata), 1, fp);
	indata.flag_TB[numrec] = 0;	//该位置标记为可再次存入电话号码(相当于删除)
	indata.num_TB--;

	//将文件索引重新写入文件头部
	fseek(fp, 0, SEEK_SET);
	fwrite(&indata, sizeof(indata), 1, fp);
	
	if (indata.num_TB>200)
	{
		goto YX_HandSet_DelTBError;
		return;
	}
	
	
	iCnt = 0;
	for (i = 0;i<MAX_TBREC;i++)
	{
		if (indata.flag_TB[i]!=0)
		{
			flagTB[iCnt++] = i;  //记录有效电话本的索引
		}
	}	

	memset( indata.flag_TB,0x00,sizeof(indata.flag_TB) );  //清除所有电话本有效的标记
	indata.num_TB = iCnt;

	for (i = 0;i<iCnt;i++)
	{
		byIndex = flagTB[i];  //依次取出有效的索引号，移动数据
		indata.flag_TB[i] = 1;		
		fseek(fp, sizeof(indata)+30*byIndex, SEEK_SET);
		fread(aryTelBook+30*i,30,1,fp);
	}

	fseek(fp, sizeof(indata), SEEK_SET);//移动到保存电话本处
	fwrite(aryTelBook,30,iCnt,fp);

	fseek(fp, 0, SEEK_SET);  //移到文件头
	fwrite(&indata,sizeof(indata),1,fp);
	fclose(fp);
	fp=NULL;

	return;

YX_HandSet_DelTBError:
	indata.num_TB = 0;
	memset(indata.flag_TB,0x00,sizeof(indata.flag_TB));
	fseek(fp, 0, SEEK_SET);
	fwrite(&indata, sizeof(indata), 1, fp);
	memset(aryTelBook,0x00,sizeof(aryTelBook));
	fwrite(aryTelBook,1,sizeof(aryTelBook),fp);
	fclose(fp);
	fp = NULL;	
}

INT16S CHstApp::GetTBRecByName_FULL(INT8U len, INT8U *name)
{//cyh add  返回-1表示没有重名

	INT16S ret = -1; 
	char buf[500];
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	for (int i=0; i<indata.num_TB; i++)
	{
		//fseek(fp, sizeof(indata)+i*30, SEEK_SET);
		fseek(fp, sizeof(indata)+i*30, SEEK_SET);
		fread(buf+i*10, 10, 1, fp);	//从文件中读出电话本中的姓名存入buf中

		if (strncmp(buf+i*10, (char *)name, len) == 0 && strlen(buf+i*10)==len)
		{
			ret = i;
			break;
		}
	}	

	fclose(fp);
	return ret;
}

INT16U CHstApp::GetTBNameByTel(INT8U *ptr, INT8U len, INT8U *tel)
{//cyh add 通过电话号码找名字  返回值：如果搜索到泽返回1，否则返回0
	
	INT8U ret = 0;
	int i;

	char buf[MAX_TBREC*20 + 100];
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return ret;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);
	if (indata.num_TB == 0)		return 0;

	for (i=0; i<indata.num_TB; i++) 
	{
		fseek(fp, sizeof(indata)+10+i*30, SEEK_SET);
		fread(buf+i*20, 20, 1, fp);	//从文件中读出电话本中的电话号码存入buf中
		
		if (strncmp(buf+i*20, (char *)tel, len) != 0)
		{
			ret = 0;
		}
		else 
			break;
	}

	//如果找到该号码,将姓名存入ptr
	if ( i<indata.num_TB )
	{	
		char szName[11] = {0};
		fseek(fp, sizeof(indata)+i*30, SEEK_SET);
		fread(szName, 10, 1, fp);

		byte byNameLen = 0;
		byNameLen = strlen(szName);
		if (byNameLen>10)
		{
			byNameLen = 10;
		}

		memcpy(ptr,szName,byNameLen);
		ret = byNameLen;
	}	

	fclose(fp);
	return ret;
}

//获取numrec位置的电话记录
INT16U CHstApp::GetTBRec(INT16U numrec, TBREC_STRUCT *recptr)	
{
	memset(recptr,0x00,sizeof(TBREC_STRUCT));

	INT8U buf[30];
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	if (indata.num_TB == 0)  
		return 0;

	fseek(fp, sizeof(indata)+30*numrec, SEEK_SET);
	fread(buf, 30, 1, fp);

	byte byLen = 0;
	byLen = strlen((char*)buf);

	if (byLen>10)
	{
		byLen = 10;
	}
	recptr->namelen = byLen;

	memcpy(recptr->name, (byte *)buf, byLen);
	
	byLen = strlen((char*)(buf+10));

	if (byLen>20)
	{
		byLen=20;
	}
	recptr->tellen = byLen;
	memcpy(recptr->tel, (byte *)(buf+10), byLen);
	
	fclose(fp);
	return 1;
}

INT16U CHstApp::GetTBItem(void)
{
	INT8U	ret;
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);
	ret = indata.num_TB;

	fclose(fp);

	return ret;

}

void CHstApp::DelDB_TB(void)
{
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");

	if (NULL==fp)
	{
		return ;
	}

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	for (int i=0; i<MAX_TBREC; i++) 
	{
		indata.flag_TB[i] = 0;	//该位置标记为0,可再次存入电话号码(相当于删除)
	}
	
	indata.num_TB = 0;
	//将文件索引重新写入文件头部
	fseek(fp, 0, SEEK_SET);
	fwrite(&indata, sizeof(indata), 1, fp);
	
	char buf[30*MAX_TBREC] = {0};
	fwrite(buf,1,sizeof(buf),fp);
	
	fclose(fp);
}

INT16U CHstApp::GetTBBlankItem(void)
{
	return MAX_TBREC - GetTBItem();
}

void CHstApp::SaveTKToFile(INT8U talktype, TALKREC_STRUCT *recptr)
{
	char buf0[1000] = {0};
	char buf1[1000] = {0};
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");

	SAVEDATAINDEX_STRUCT index;

	if (fp != NULL) 
	{
		fread(&index, sizeof(index), 1, fp);	//读出文件索引

		switch(talktype) 
		{
		case TALK_DIAL:		//已拨,存储空间600Byte,从文件的第2000Byte处开始存储
			{
				index.num_TK_dia++;
				if (index.num_TK_dia >= MAX_TALKREC)
				{
					index.num_TK_dia = MAX_TALKREC;
				}

				fseek(fp, sizeof(index)+TALK_DAIL_OFFSET, SEEK_SET);	//移动指针至已拨电话位置
				fread(buf0, sizeof(TALKREC_STRUCT)*(index.num_TK_dia-1), 1, fp);	//将整个已拨电话记录读入内存
				memcpy(buf1, recptr, sizeof(TALKREC_STRUCT));		//将当前记录在已拨电话位置的头部
				memcpy(buf1+sizeof(TALKREC_STRUCT), buf0, sizeof(TALKREC_STRUCT)*(index.num_TK_dia-1));	//将已存的记录存入
				fseek(fp, sizeof(index)+TALK_DAIL_OFFSET, SEEK_SET);	//移动指针至已拨电话位置
				fwrite(buf1, sizeof(TALKREC_STRUCT)*index.num_TK_dia, 1, fp);
			}
			break;

		case TALK_RECV:
			{
				index.num_TK_rec++;
				if (index.num_TK_rec >= MAX_TALKREC) 
				{
					index.num_TK_rec = MAX_TALKREC;
				}
				fseek(fp, sizeof(index)+TALK_RECV_OFFSET, SEEK_SET);	//移动指针至已接电话位置
				fread(buf0, sizeof(TALKREC_STRUCT)*(index.num_TK_rec-1), 1, fp);	//将整个已接电话记录读入内存
				memcpy(buf1, recptr, sizeof(TALKREC_STRUCT));		//将当前记录在已拨电话位置的头部
				memcpy(buf1+sizeof(TALKREC_STRUCT), buf0, sizeof(TALKREC_STRUCT)*(index.num_TK_rec-1));	//将已存的记录存入
				fseek(fp, sizeof(index)+TALK_RECV_OFFSET, SEEK_SET);	//移动指针至已接电话位置
				fwrite(buf1, sizeof(TALKREC_STRUCT)*index.num_TK_rec, 1, fp);
			}
			break;

		case TALK_MISS:
			{
				index.num_TK_mis++;
				if (index.num_TK_mis >= MAX_TALKREC) 
				{
					index.num_TK_mis = MAX_TALKREC;
				}
				fseek(fp, sizeof(index)+TALK_MISS_OFFSET, SEEK_SET);	//移动指针至已接电话位置
				fread(buf0, sizeof(TALKREC_STRUCT)*(index.num_TK_mis-1), 1, fp);	//将整个已接电话记录读入内存
				memcpy(buf1, recptr, sizeof(TALKREC_STRUCT));		//将当前记录在已拨电话位置的头部
				memcpy(buf1+sizeof(TALKREC_STRUCT), buf0, sizeof(TALKREC_STRUCT)*(index.num_TK_mis-1));	//将已存的记录存入
				fseek(fp, sizeof(index)+TALK_MISS_OFFSET, SEEK_SET);	//移动指针至已接电话位置
				fwrite(buf1, sizeof(TALKREC_STRUCT)*index.num_TK_mis, 1, fp);
			}
			break;

		default:
			break;
		}

		fseek(fp, 0, SEEK_SET);
		fwrite(&index, sizeof(index), 1, fp);
	}
	
	fclose(fp);
}

void CHstApp::DelTalkRecord(INT8U talktype, INT16U numrec)
{
	char buf[1000] = {0};
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");

	if (fp != NULL)
	{
		SAVEDATAINDEX_STRUCT indata;
		fread(&indata, sizeof(indata), 1, fp);
		
		switch(talktype) 
		{
		case TALK_DIAL:
			{
				indata.num_TK_dia--;
				fseek(fp, sizeof(indata)+TALK_DAIL_OFFSET+sizeof(TALKREC_STRUCT)*(numrec+1), SEEK_SET);	//指针移至下一位置
				fread(buf, sizeof(TALKREC_STRUCT)*(indata.num_TK_dia-numrec), 1, fp);		//读入所删记录后面的数据
				fseek(fp, sizeof(indata)+TALK_DAIL_OFFSET+sizeof(TALKREC_STRUCT)*numrec, SEEK_SET);		//移至所删记录位置
				fwrite(buf, sizeof(TALKREC_STRUCT)*(indata.num_TK_dia-numrec), 1, fp);		//将后面的数据写入
			}
			break;

		case TALK_RECV:
			{
				indata.num_TK_rec--;
				fseek(fp, sizeof(indata)+TALK_RECV_OFFSET+sizeof(TALKREC_STRUCT)*(numrec+1), SEEK_SET);	//指针移至下一位置
				fread(buf, sizeof(TALKREC_STRUCT)*(indata.num_TK_rec-numrec), 1, fp);		//读入所删记录后面的数据
				fseek(fp, sizeof(indata)+TALK_RECV_OFFSET+sizeof(TALKREC_STRUCT)*numrec, SEEK_SET);		//移至所删记录位置
				fwrite(buf, sizeof(TALKREC_STRUCT)*(indata.num_TK_rec-numrec), 1, fp);		//将后面的数据写入
			}
			
			break;
		case TALK_MISS:
			{
				indata.num_TK_mis--;
				fseek(fp, sizeof(indata)+TALK_MISS_OFFSET+sizeof(TALKREC_STRUCT)*(numrec+1), SEEK_SET);	//指针移至下一位置
				fread(buf, sizeof(TALKREC_STRUCT)*(indata.num_TK_mis-numrec), 1, fp);		//读入所删记录后面的数据
				fseek(fp, sizeof(indata)+TALK_MISS_OFFSET+sizeof(TALKREC_STRUCT)*numrec, SEEK_SET);		//移至所删记录位置
				fwrite(buf, sizeof(TALKREC_STRUCT)*(indata.num_TK_mis-numrec), 1, fp);		//将后面的数据写入
			}
			break;

		default:
			break;
		}

		//将文件索引重新写入文件头部
		fseek(fp, 0, SEEK_SET);
		fwrite(&indata, sizeof(indata), 1, fp);
	}
	
	fclose(fp);
}

INT16U CHstApp::GetTalkRecord(INT8U talktype, INT16U numrec, TALKREC_STRUCT *recptr)
{
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	switch(talktype)
	{
	case TALK_DIAL:
		{
			if (indata.num_TK_dia == 0) 
				return 0;

			fseek(fp, sizeof(indata)+TALK_DAIL_OFFSET+sizeof(TALKREC_STRUCT)*numrec, SEEK_SET);
			fread(recptr, sizeof(TALKREC_STRUCT), 1, fp);
		}
		break;

	case TALK_RECV:
		{
			if (indata.num_TK_rec == 0)
				return 0;

			fseek(fp, sizeof(indata)+TALK_RECV_OFFSET+sizeof(TALKREC_STRUCT)*numrec, SEEK_SET);
			fread(recptr, sizeof(TALKREC_STRUCT), 1, fp);
		}
		break;

	case TALK_MISS:
		{
			if (indata.num_TK_dia == 0)  return 0;
			fseek(fp, sizeof(indata)+TALK_MISS_OFFSET+sizeof(TALKREC_STRUCT)*numrec, SEEK_SET);
			fread(recptr, sizeof(TALKREC_STRUCT), 1, fp);
		}
		break;

	default:
		break;
	}

	fclose(fp);
	return 1;
}

INT16U CHstApp::GetNumTalkRecord(INT8U talktype)
{
	INT8U ret;
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);
	
	switch(talktype) {
	case TALK_DIAL:
		ret = indata.num_TK_dia;
		break;
	case TALK_RECV:
		ret = indata.num_TK_rec;
		break;
	case TALK_MISS:
		ret = indata.num_TK_mis;
		break;
	default:
		break;
	}
	fclose(fp);

	return ret;
}

void CHstApp::DelAllTalkRecord(INT8U talktype)
{
	char buf[600] = {0};
	INT16U num = GetNumTalkRecord(talktype);
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");

	if (fp != NULL)
	{
		SAVEDATAINDEX_STRUCT indata;
		fread(&indata, sizeof(indata), 1, fp);
		
		switch(talktype) 
		{
		case TALK_DIAL:
			{
				indata.num_TK_dia = 0;
				fseek(fp, sizeof(indata)+TALK_DAIL_OFFSET, SEEK_SET);
				fwrite(buf, num*sizeof(TALKREC_STRUCT), 1, fp);
			}
			break;

		case TALK_RECV:
			{
				indata.num_TK_rec = 0;
				fseek(fp, sizeof(indata)+TALK_RECV_OFFSET, SEEK_SET);
				fwrite(buf, num*sizeof(TALKREC_STRUCT), 1, fp);
			}
			break;

		case TALK_MISS:
			{
				indata.num_TK_mis = 0;
				fseek(fp, sizeof(indata)+TALK_MISS_OFFSET, SEEK_SET);
				fwrite(buf, num*sizeof(TALKREC_STRUCT), 1, fp);
			}
			break;

		default:
			break;
		}

		//将文件索引重新写入文件头部
		fseek(fp, 0, SEEK_SET);
		fwrite(&indata, sizeof(indata), 1, fp);
	}

	fclose(fp);
}

void CHstApp::AddATMPRec(ATMPREC_STRUCT *atmprec)
{
	INT8U str[150] = {0};
	char buf0[5000] = {0};
	char buf1[5000] = {0};

	struct tm pCurrentTime;
	G_GetTime(&pCurrentTime);
	
	sprintf(atmprec->date, "%04d.%02d.%02d", pCurrentTime.tm_year+1900, pCurrentTime.tm_mon+1, pCurrentTime.tm_mday);
	sprintf(atmprec->time, "%02d:%02d:%02d", pCurrentTime.tm_hour, pCurrentTime.tm_min, pCurrentTime.tm_sec);

	memcpy(str, atmprec->date, 10);
	memcpy(str+10, atmprec->time, 10);
	str[20] = atmprec->datalen;
	memcpy(str+21, atmprec->data, strlen(atmprec->data));

	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");

	if (fp != NULL) 
	{
		SAVEDATAINDEX_STRUCT indata;
		fread(&indata, sizeof(indata), 1, fp);

		if (m_msgsta == 1)
		{
			indata.num_Atmp++;
			if (indata.num_Atmp >= MAX_ATMPREC) 
			{
				indata.num_Atmp = MAX_ATMPREC;				
			}

			//将当前记录记在调度区的头部
			memcpy(buf1, atmprec, sizeof(ATMPREC_STRUCT));
			
			//如果有旧记录
			if (indata.num_Atmp>1)
			{
				fseek(fp, sizeof(indata)+ATM_REC_OFFSET, SEEK_SET);	//移动指针至调度位置
				fread(buf0, sizeof(ATMPREC_STRUCT)*(indata.num_Atmp-1), 1, fp);	//将整个调度记录读入内存
				memcpy(buf1+sizeof(ATMPREC_STRUCT), buf0, sizeof(ATMPREC_STRUCT)*(indata.num_Atmp-1));	//将已存的记录存入
			}

			fseek(fp, sizeof(indata)+ATM_REC_OFFSET, SEEK_SET);	//移动指针至调度区起始位置
			fwrite(buf1, sizeof(ATMPREC_STRUCT)*indata.num_Atmp, 1, fp);
		} 
		else if (m_msgsta == 2)
		{
			indata.num_PC++;
			if (indata.num_PC >= MAX_PCREC)
			{
				indata.num_PC = MAX_PCREC;				
			}

			//将当前记录记在调度区的头部
			memcpy(buf1, atmprec, sizeof(ATMPREC_STRUCT));

			if (indata.num_PC > 1)
			{
				fseek(fp, sizeof(indata)+DIANZAO_REC_OFFSET, SEEK_SET);	//移动指针至调度位置
				fread(buf0, sizeof(ATMPREC_STRUCT)*(indata.num_PC-1), 1, fp);	//将已存的电召信息读入内存			
				memcpy(buf1+sizeof(ATMPREC_STRUCT), buf0, sizeof(ATMPREC_STRUCT)*(indata.num_PC-1));	//将已存的记录存入
			}

			fseek(fp, sizeof(indata)+DIANZAO_REC_OFFSET, SEEK_SET);	//移动指针至调度区起始位置
			fwrite(buf1, sizeof(ATMPREC_STRUCT)*indata.num_PC, 1, fp);
		}
		
		fseek(fp, 0, SEEK_SET);
		fwrite(&indata, sizeof(indata), 1, fp);
	}
	
	fclose(fp);
	
}

INT16U CHstApp::GetATMPRec(INT16U numrec, ATMPREC_STRUCT *ptr)
{
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	if (m_msgsta == 1) 
	{
		if (indata.num_Atmp == 0) 
			return 0;

		fseek(fp, sizeof(indata)+ATM_REC_OFFSET+sizeof(ATMPREC_STRUCT)*numrec, SEEK_SET);
	} 
	else if (m_msgsta == 2)
	{
		if (indata.num_PC == 0)
			return 0;

		fseek(fp, sizeof(indata)+DIANZAO_REC_OFFSET+sizeof(ATMPREC_STRUCT)*numrec, SEEK_SET);  //HXD 为什么是14000
	}

	fread(ptr, sizeof(ATMPREC_STRUCT), 1, fp);
	fclose(fp);

	return 1;
}

INT16U CHstApp::GetATMPRecItem(void)
{
	INT8U	ret;
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);
	if (m_msgsta == 1) 
	{
		ret = indata.num_Atmp;
	}
	else if (m_msgsta == 2)
	{
		ret = indata.num_PC;
	}

	fclose(fp);

	return ret;
}

void CHstApp::AddSMRec(SMREC_STRUCT *smrec)
{
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");

	if (NULL==fp)
	{
		return;
	}

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	int i;
	indata.num_SM++;
	if (indata.num_SM >= MAX_SMREC) 
	{
		indata.num_SM = MAX_SMREC;			
		
// 		//如果记录满,则标记所有的位置可以再次被写入
// 		i = 0;
// 		for (int j=0; j<MAX_SMREC; j++)
// 		{
// 			indata.flag_SM[j] = 0;		
// 		}

		// 存储已满， 则覆盖最旧的短信
		indata.flag_SM[MAX_SMREC-1] = 0;
	}

	for (i=0; i<MAX_SMREC; i++) //查询前面已存的空间,如果有位置没有存储(中间有被删除的)
	{	
		if (indata.flag_SM[i] == 0) //如果该位置没有被占用,则可写入
		{
			fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*i, SEEK_SET);
			fwrite(smrec, 200, 1, fp);
			indata.flag_SM[i] = 1;
			break;	//正确写入后直接退出循环,以免同一条记录多次被写
		}
	}

	char szSms[200*MAX_SMREC] = {0};
	byte aryFlagSms[MAX_SMREC] = {0};

	int iSmsCnt = 0;
	for (i=MAX_SMREC -1; i>=0; i-- ) 
	{
		if (0 == indata.flag_SM[i])
		{
			continue;
		}
		
		aryFlagSms[iSmsCnt++] = i;
	}

	if (iSmsCnt>MAX_SMREC)
	{
		iSmsCnt = MAX_SMREC;
	}

	memset(indata.flag_SM,0x00,sizeof(indata.flag_SM));

	// 这样调整会使得短信顺序每次都发生颠倒
// 	for (i=0;i<iSmsCnt;i++)
// 	{
// 		indata.flag_SM[i] = 1;
// 		fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*aryFlagSms[i], SEEK_SET);
// 		fread(szSms+200*i,1,200,fp);
// 	}

	// 改为如下调整方式, 把最后面一条（即最新的那条）调整到第一条，其他的依次后移
	fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*aryFlagSms[0], SEEK_SET);
	fread(szSms,1,200,fp);
	for(i=iSmsCnt-1; i>0; i--)
	{
		fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*aryFlagSms[i], SEEK_SET);
 		fread(szSms+200*(iSmsCnt-i),1,200,fp);
	}
	// 调整顺序后，再重新写入
	fseek(fp, sizeof(indata)+SM_REC_OFFSET, SEEK_SET);
	fwrite(szSms,200,iSmsCnt,fp);//写入短信	

	for (i=0;i<iSmsCnt;i++)
	{
		indata.flag_SM[i] = 1;
	}
	fseek(fp, 0, SEEK_SET);
	fwrite(&indata, sizeof(indata), 1, fp);

	fclose(fp);
}

void CHstApp::DelSMRec(INT16U numrec)
{
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if (NULL==fp)
	{
		return;
	}

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata,sizeof(indata),1,fp);
	indata.flag_SM[numrec] = 0;
	if (indata.num_SM)
	{
		indata.num_SM--;
	}	

	char szSms[200*MAX_SMREC] = {0};
	byte aryFlagSms[MAX_SMREC] = {0};
	int i = 0;
	int iSmsCnt = 0;

	for (i=MAX_SMREC-1;i>=0;i--)
	{
		if (0==indata.flag_SM[i])
		{
			continue;
		}
		
		aryFlagSms[iSmsCnt++] = i;
	}

	if (iSmsCnt>MAX_SMREC)
	{
		iSmsCnt = MAX_SMREC;
	}

	memset(indata.flag_SM,0x00,sizeof(indata.flag_SM));
	for (i=0;i<iSmsCnt;i++)
	{
		indata.flag_SM[i] = 1;
		fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*aryFlagSms[i], SEEK_SET);
		fread(szSms+200*i,1,200,fp);//
	}

	fseek(fp, sizeof(indata)+SM_REC_OFFSET, SEEK_SET);
	fwrite(szSms,200,iSmsCnt,fp);//写入短信	

	fseek(fp,0,SEEK_SET);
	fwrite(&indata,sizeof(indata),1,fp);

	fclose(fp);
	fp=NULL;
}

INT16U CHstApp::GetSMRec(INT16U numrec, SMREC_STRUCT *ptr)
{
	INT8U buf[200] = {0};
	int iSize = sizeof(SMREC_STRUCT);
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);
	SMREC_STRUCT tagSms;
	memset(&tagSms,0x00,sizeof(tagSms));
	int len = 0;
	if (indata.num_SM == 0)  return 0;
	fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*numrec, SEEK_SET);
	fread(&tagSms, 200, 1, fp);
	tagSms.unread=false;		
	ptr->unread = tagSms.unread;				
	ptr->tellen = tagSms.tellen;
	memcpy(ptr->tel,tagSms.tel,tagSms.tellen);
	memcpy(ptr->time,tagSms.time,12);
	ptr->datalen = tagSms.datalen;
	memcpy(ptr->data,tagSms.data,tagSms.datalen);
	
	fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*numrec, SEEK_SET);
	buf[0] = 0;
	fwrite(&buf[0], 1, 1, fp);	//将更新后的值重新写入文件,表示该短信已读

	fclose(fp);

	return 1;
}

void CHstApp::DelAllSMRec(void)
{
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if (NULL==fp)
	{
		PRTMSG(MSG_NOR, "del sms:Open handset file err\n");
		return;
	}

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata,sizeof(indata),1,fp);	
	memset(indata.flag_SM,0x00,sizeof(indata.flag_SM));
	indata.num_SM = 0;	

	char szSms[200*MAX_SMREC] = {0};
	fseek(fp, sizeof(indata)+SM_REC_OFFSET, SEEK_SET);
	fwrite(szSms,1,sizeof(szSms),fp);//写入短信	

	fseek(fp,0,SEEK_SET);
	fwrite(&indata,sizeof(indata),1,fp);

	fclose(fp);
	fp=NULL;
}

bool CHstApp::SaveHstConfig()
{
	tag2QHstCfg obj2QHstCfg;
	obj2QHstCfg.m_bytcallring = m_HstCfg.m_bytcallring;
	obj2QHstCfg.m_bytsmring	= m_HstCfg.m_bytsmring;
	obj2QHstCfg.m_HandsfreeChannel = m_HstCfg.m_HandsfreeChannel;
	obj2QHstCfg.m_EnableKeyPrompt = m_HstCfg.m_EnableKeyPrompt;
	obj2QHstCfg.m_EnableAutoPickup = m_HstCfg.m_EnableAutoPickup;

	memcpy( obj2QHstCfg.m_szListenTel,m_HstCfg.m_szListenTel,strlen(m_HstCfg.m_szListenTel) );

	if ( !SetSecCfg(&obj2QHstCfg, offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg)) ) 
	{
		return true;
	}
	else
	{
		return false;
	}
}

/***************ShortMsg*********************************************************/
void CHstApp::ACK_EraseSM(void)
{
    g_hst.PopMenuWin();
    g_hst.ActivateStringPrompt("删除中…");
    DelSMRec(m_selrecord_sm);
    if (!HaveUnReadSMRec()) g_hst.TurnoffSMIndicator();

	g_hst.PopMenuWin();//cyh add:add  如果没有加这个，会显示刚才的短信
	g_hst.PopMenuWin();//cyh add:add
    g_hst.ActivateStringPrompt("已删除");
}

void CHstApp::EraseSM(void)
{
	char buf[] = {"删除?"};
    g_hst.ActivateSelPrompt(buf, 0, ACK_EraseSM);
}
void CHstApp::TransmitSM(void)
{
    g_hst.PopMenuWin();
    memcpy(&EditRecord, &CurRecord_SM, sizeof(EditRecord));
    EditRecord.tellen = 0;
    CreateSMEditor();
}

void CHstApp::WriteBackSM(void)
{
    g_hst.PopMenuWin();
    memcpy(&EditRecord, &CurRecord_SM, sizeof(EditRecord));
    EditRecord.datalen = 0;
    CreateSMEditor();
}

void CHstApp::SaveTel(void)
{
    SaveToTB(CurRecord_SM.tel, CurRecord_SM.tellen);
}

void CHstApp::DialTel_SM(void)
{
	ActivateVirPhone_Dial(false, CurRecord_SM.tellen, CurRecord_SM.tel);
}

void CHstApp::Handler_KEY_ACK_CONTENT(void)
{
    g_hst.ActivateItemMenu(&MENU_OPSM);
}

INT16U CHstApp::MaxRecordProc_CONTENT(void)
{
    return 3;
}

INT16U CHstApp::ReadRecordProc_CONTENT(INT16S numrec, INT8U *ptr)
{
    INT8U len;

    if (numrec == 0)
	{
        memcpy(ptr, CurRecord_SM.data, CurRecord_SM.datalen);
       
		return CurRecord_SM.datalen;
    } 
	else if (numrec == 1) 
	{
        len  = strlen("发信人:");
        memcpy(ptr, "发信人:", len);
        ptr    += len;

		char szName[16] = {0};
		byte byRet = 0;
		byRet = GetTBNameByTel((byte*)szName,CurRecord_SM.tellen,CurRecord_SM.tel);
		
		if (byRet)
		{//如果电话本有对应的记录，显示名字，否则显示电话号码
			szName[15] = 0;
			byte byNameLen = 0;
			byNameLen = strlen(szName);
			memcpy(ptr,szName,byNameLen);
			len+=byNameLen;
		}
		else
		{
			memcpy(ptr, CurRecord_SM.tel, CurRecord_SM.tellen);
			len    += CurRecord_SM.tellen;
		}

        return len;
    } 
	else if (numrec == 2)
	{
        len     = strlen("时间:");
        memcpy(ptr, "时间:", len);
        ptr    += len;

		char szTime[20] = {0};
		for (int i = 0;i<6;i++)
		{
			memcpy(szTime+3*i,CurRecord_SM.time+2*i,2);
			if (i<2) 
			{
				szTime[3*i+2] = '-';	
			}
			if (i==2) 
			{
				szTime[3*i+2] = ' ';			
			}
			if (i>2) {
				szTime[3*i+2] = ':';			
			}
	
		}
		szTime[17] = 0;//将最后一个.去掉
		memcpy(ptr,szTime,strlen(szTime));
		len    += strlen(szTime);  
        ptr    += strlen(szTime);
        *ptr++  = '\n';
        len    += 1;

        return len;
    }
	else 
	{
        return 0;
    }
}

void CHstApp::Handler_KEY_ACK_SM(void)
{
 	static MENUWIN_STRUCT MENU_READCONTENT =
 	{WIN_FMENU, Handler_KEY_ACK_CONTENT, 0, 0, 0, 0, MaxRecordProc_CONTENT, 0, ReadRecordProc_CONTENT};

	memset(&currecord,0x00,sizeof(currecord));//cyh add:add
	memset(&CurRecord,0x00,sizeof(CurRecord));//cyh add:add
	memset(&CurRecord_SM,0x00,sizeof(CurRecord_SM));//cyh add:add

    m_selrecord_sm = g_hst.GetMenuWinRecordNum();

	//PRTMSG(MSG_DBG, "m_selrecord_sm = %d\n", m_selrecord_sm);

	if (m_selrecord_sm != -1) 
	{
		if (GetSMRec(m_selrecord_sm, &CurRecord_SM)) 
		{
			g_hst.ActivateMenuWin(&MENU_READCONTENT);
			ClearSMRecUnReadFlag(m_selrecord_sm);      /* clear unread flag */
			if (!HaveUnReadSMRec()) g_hst.TurnoffSMIndicator();
		}
	}
}

INT16U CHstApp::MaxRecordProc_SM(void)
{
    return GetSMRecItem();
}

INT16U CHstApp::ReadRecordProc_SM(INT16S numrec, INT8U *ptr)
{
    INT8U len1, len2;

    if (numrec == -1)
	{
        memcpy(ptr, "无信息", strlen("无信息"));
        return strlen("无信息");
    }
    if (SMRecUnRead(numrec))
        *ptr++ = 0x7e;
    else
        *ptr++ = ' ';

    len1 = GetSMRecTel(numrec, (INT8U *)m_dplybuf);
    len2 = GetTBNameByTel(ptr, len1, (INT8U *)m_dplybuf);

	//PRTMSG(MSG_DBG, "len1 = %d, len2 = %d\n", len1, len2);

	//如果在电话本中没有搜索到符合的名字，就返回电话的长度+1，否则返回名字的长度+1
    //如果能在电话本中找到发短消息的人
	if (len2 > 0) 
	{
        return len2 + 1;  //cyh add:这个1代表最开始添加的0x7e或者空格' '
    } 
	else
	{
        memcpy(ptr, m_dplybuf, len1);
        return len1 + 1;
    }
}

void CHstApp::ReadSM(void)
{
    g_hst.ActivateMenuWin(&MENU_LISTSM);
}

void CHstApp::SendInformer(INT8U result)
{
    if (result == 0)
        g_hst.ActivateStringPrompt("发送成功!");
    else
        g_hst.ActivateStringPrompt("发送失败!");
}

void CHstApp::GetTelFromTB(INT8U tellen, INT8U *tel, INT8U namelen, INT8U *name)
{
    namelen = namelen;
    name    = name;
    TelEditorEntry.InitLen = tellen;
    TelEditorEntry.InitPtr = tel;
    g_hst.ResetEditor(&TelEditorEntry);
}

void CHstApp::ExitTelEditor_SM(void)
{
    INT16U  i;

    EditRecord.tellen = g_hst.GetEditorData(EditRecord.tel);

    if (EditRecord.tellen > 0)
	{
        g_hst.ExitEditor();   /* Exit Editor */

        if (!SearchGBCode(EditRecord.data, EditRecord.datalen))
		{
            for (i = 0; i < EditRecord.datalen; i++)
			{
                EditRecord.data[i] = ASCIIToGsmCode(EditRecord.data[i]);
            }
        }
		INT8U buf[200];
		INT8U len = 0;
		buf[0] = 0x30;											len++;
		buf[1] = EditRecord.tellen;								len++;
		memcpy(buf+len, EditRecord.tel, EditRecord.tellen);		len += EditRecord.tellen;
		buf[len] = EditRecord.datalen;							len++;
		memcpy(buf+len, EditRecord.data, EditRecord.datalen);	len += EditRecord.datalen;
		DataPush(buf, len, DEV_DIAODU, DEV_PHONE, LV2);
    }
	else
	{
        if (!ListTB_RET(GetTelFromTB)) 
		{
            g_hst.ExitEditor();
        }
    }
	g_hst.ExitEditor();
	g_hst.ActivateStringPrompt_TIME("短信发送中",9);
}

void CHstApp::CreateTelEditor(void)
{
    memset(&TelEditorEntry, 0, sizeof(TelEditorEntry));
    TelEditorEntry.Mode     = EDT_DIGITAL | EDT_CURSOR | EDT_PULS;
    TelEditorEntry.MinLen   = 0;
    TelEditorEntry.MaxLen   = SMREC_SIZE_TEL;
    TelEditorEntry.LabelLen = strlen("手机号：    ");
    TelEditorEntry.LabelPtr = "手机号：    ";
    TelEditorEntry.InitLen  = EditRecord.tellen;
    TelEditorEntry.InitPtr  = EditRecord.tel;
    TelEditorEntry.AckProc  = ExitTelEditor_SM;
    g_hst.CreateEditor(&TelEditorEntry);
}

void CHstApp::AckSMEditor(void)
{
    EditRecord.datalen = g_hst.GetEditorData(EditRecord.data);
    CreateTelEditor();
}

INT16U CHstApp::CurLenProc(INT16U len, INT8U *ptr)
{
    return TestGBLen(ptr, len);
}

INT16U CHstApp::MaxLenProc(INT16U len, INT8U *ptr)
{
    if (SearchGBCode(ptr, len))
        return 140;
    else
        return SMREC_SIZE_DATA;
}

void CHstApp::CreateSMEditor(void)
{
	EDITOR_STRUCT SMEditorEntry = {EDT_CURSOR | EDT_CHINESE, 0, 0, 0, 0, 0, 0, m_storebuf, 0, AckSMEditor, CurLenProc, 0, MaxLenProc, 0};

    SMEditorEntry.InitLen = EditRecord.datalen;
    SMEditorEntry.InitPtr = EditRecord.data;
    g_hst.CreateEditor(&SMEditorEntry);
}

void CHstApp::WriteSM(void)
{
    EditRecord.tellen  = 0;
    EditRecord.datalen = 0;
    CreateSMEditor();
}

void CHstApp::ACK_DelAll(void)
{
    g_hst.TurnoffSMIndicator();
    g_hst.ActivateStringPrompt("删除中…");
    DelAllSMRec();
    g_hst.ActivateStringPrompt("已删除");
}

void CHstApp::DelAll(void)
{
	char buf[] = {"删除所有?"};
    g_hst.ActivateSelPrompt_TIME(buf, 9, ACK_DelAll, 0);
}

void CHstApp::ActivateReadShorMsgMenu(void)
{
    ReadSM();
}

INT8U CHstApp::HaveUnReadSMRec()
{
	INT8U buf[20];
	INT8U ret;
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	if (indata.num_SM == 0)  return 0;
	for (int i=0; i<indata.num_SM; i++)
	{
		fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*i, SEEK_SET);
		fread(buf+i, 1, 1, fp);
		ret	= buf[i];

		//如果有未读的短消息,即buf[i] = 1,直接退出并返回
		if (buf[i])		break;
	}

	fclose(fp);

	return ret;
}

void CHstApp::ClearSMRecUnReadFlag(INT16U numrec)
{
	INT8U buf[10] = {0};
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");

	if (fp != NULL) 
	{
		SAVEDATAINDEX_STRUCT indata;
		fread(&indata, sizeof(indata), 1, fp);

		fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*numrec, SEEK_SET);

		buf[0] = 0;	//表示该消息已读
		fwrite(&buf[0], 1, 1, fp);
	}
	
	fclose(fp);
}

INT16U CHstApp::GetSMRecItem(void)
{
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	fclose(fp);

	return indata.num_SM;
}

INT8U CHstApp::SMRecUnRead(INT16U numrec)
{
	INT8U buf[1];
	INT8U ret;
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	if (indata.num_SM == 0)  return 0;

	fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*numrec, SEEK_SET);
	fread(&buf[0], 1, 1, fp);
	ret = buf[0];

	fclose(fp);
	return ret;
}

INT8U CHstApp::GetSMRecTel(INT16U numrec, INT8U *ptr)
{
	INT8U buf[2] = {0};
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	if (indata.num_SM == 0)  return 0;

	//PRTMSG(MSG_DBG, "numrec = %d\n", numrec);

	fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*numrec, SEEK_SET);  //cyh add:mod
	fread(buf, 2, 1, fp);	//先读出是否阅读标记和电话号码长度

	//PRTMSG(MSG_DBG, "buf[0] = %d, buf[1] = %d\n", buf[0], buf[1]);

	fseek(fp, sizeof(indata)+SM_REC_OFFSET+200*numrec+2, SEEK_SET);  //cyh add:mod
	fread(ptr, buf[1], 1, fp);	//读出电话号码至ptr
	
	fclose(fp);

	return buf[1];
}

void CHstApp::SendSMToCustomer()
{
	SMS_STRUCT sms;
	memset(sms.info, 0 , sizeof(sms.info));
	sms.len0 = strlen(m_frm053d.tel);
	strcpy(sms.tel, m_frm053d.tel);

	if (strncmp(m_frm053d.tel, "1", 1) != 0)
	{
		char lingtong[16] = {0};
		strcat(lingtong, "106");
		strcat(lingtong, sms.tel);
		memset(sms.tel, 0, 16);
		strcpy(sms.tel, lingtong);
	}
	strcpy(sms.info, "车牌号为");
	strcat(sms.info, m_frm053d.car_id);
	strcat(sms.info, "的司机马上就到，请您耐心等候！");
	sms.len1 = strlen(sms.info);
	sms.cmd = 0x30;

	PRTMSG(MSG_NOR, "%s\n", (char *)&sms)

	DWORD dwPacketNum;
	g_objPhoneRecvDataMng.PushData(LV1, (DWORD)3+sms.len0+sms.len1, (char*)&sms, dwPacketNum);
}

/********************Succor****************************************************/
void CHstApp::PromptResult(INT8U type, INT8U result)
{
    INT8U len;
    char  *str;

    switch (type)
    {
        case SUCCOR_MEDICAL:
            str = "医疗求助";
            break;
        case SUCCOR_TROUBLE:
            str = "故障求助";
            break;
        case SUCCOR_INFORMATION:
            str = "信息求助";
            break;
        default:
            str = 0;
            break;
    }
    len = strlen(str);
    memcpy(m_dplybuf, str, len);
    m_dplybuf[len++] = '\n';

    if (result == 0)
	{
        strcpy(&m_dplybuf[len], "发送成功！");
    } 
	else 
	{
        strcpy(&m_dplybuf[len], "发送失败！");
    }

    g_hst.ActivateStringPrompt(m_dplybuf);
}

void CHstApp::Informer_Medical(INT8U result)
{
    PromptResult(SUCCOR_MEDICAL, result);
}

void CHstApp::Informer_Trouble(INT8U result)
{
    PromptResult(SUCCOR_TROUBLE, result);
}

void CHstApp::Informer_Information(INT8U result)
{
    PromptResult(SUCCOR_INFORMATION, result);
}

void CHstApp::StartSuccor(INT8U type)
{
    INT8U   succor;
	INT8U	buf[2];

    switch (type)
    {
        case SUCCOR_MEDICAL:
            g_hst.ActivateStringPrompt("发送医疗求助成功^_^");
            succor   = 0x40;
			m_helptype = SUCCOR_MEDICAL;
            break;

        case SUCCOR_TROUBLE:
            g_hst.ActivateStringPrompt("发送故障求助成功^_^");
            succor   = 0x41;
			m_helptype = SUCCOR_TROUBLE;
            break;

        case SUCCOR_INFORMATION:
            g_hst.ActivateStringPrompt("发送信息求助成功^_^");
            succor   = 0x42;
			m_helptype = SUCCOR_INFORMATION;
            break;

        default:
            return;
    }

	buf[0] = 0x80;
	buf[1] = succor;
	DataPush(buf, 2, DEV_DIAODU, DEV_QIAN, LV2);	//发送紧急求助请求
}

/******************SetGPRS******************************************************/
void CHstApp::Set_GPRS_PARA(INT8U type, INT8U len, INT8U *ptr)
{
	tag1QIpCfg cfg[2];
	GetImpCfg( cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );

	tag1LComuCfg cfg2;
	GetImpCfg( &cfg2, sizeof(cfg2), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) );

	char str[100] = {0};
	memcpy(str, (char*)ptr, len);

	switch(type)
	{
	case TYPE_CDMANAME:
		{
			tag1PComuCfg obj1PComuCfg;
			GetImpCfg( &obj1PComuCfg, sizeof(obj1PComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg) ,sizeof(obj1PComuCfg) );
			memset(obj1PComuCfg.m_szCdmaUserName,0,sizeof(obj1PComuCfg.m_szCdmaUserName));			
			memcpy(obj1PComuCfg.m_szCdmaUserName, str,strlen(str));
			SetImpCfg(&obj1PComuCfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg),sizeof(obj1PComuCfg));
		}
		break;

	case TYPE_CDMAPASS:
		{
			tag1PComuCfg obj1PComuCfg;
			GetImpCfg( &obj1PComuCfg, sizeof(obj1PComuCfg),offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg) ,sizeof(obj1PComuCfg) );
			memset(obj1PComuCfg.m_szCdmaPassWord,0,sizeof(obj1PComuCfg.m_szCdmaPassWord));				
			memcpy(obj1PComuCfg.m_szCdmaPassWord, str,strlen(str));
			SetImpCfg(&obj1PComuCfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg),sizeof(obj1PComuCfg));
		}
		break;
	case TYPE_TCP_PORT1:
		{
			char* pStop = NULL;
 			unsigned long ulPort = strtoul(str, &pStop, 10);

 			if( ulPort < 1 || ulPort > 65535 )
			{
				g_hst.ActivateStringPrompt("设置失败!");
				break;
			}
			else
			{
				g_hst.ActivateStringPrompt("设置成功!");
			}

			cfg[0].m_usQianTcpPort = htons( (ushort)atol(str) );

			if(atol(str)!=0) 
			{ 
				SetImpCfg(cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );

				Frm35 frm1;		//通知千里眼端口改变了
				frm1.reply = 0x01;
				DataPush((void*)&frm1, (DWORD)sizeof(frm1), DEV_DIAODU, DEV_SOCK, LV2);
				
// 				tag2DDvrCfg objMvrCfg;
// 				GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 				objMvrCfg.IpPortPara.m_usCommPort = htons((ushort)atol(str));
// 				SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
// 				char by = 0x0c;//通知DvrExe 端口改变了
// 				DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
			}
		}
		break;

	case TYPE_TCP_PORT2:
		{
			char* pStop = NULL;
			unsigned long ulPort = strtoul(str, &pStop, 10);
			
			if( ulPort < 1 || ulPort > 65535 )
			{
				g_hst.ActivateStringPrompt("设置失败!");
				break;
			}
			else
			{
				g_hst.ActivateStringPrompt("设置成功!");
			}	
				cfg[1].m_usQianTcpPort = htons( (ushort)atol(str) );
			if(atol(str)!=0)
			{
				SetImpCfg(cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );

				Frm35 frm1;		//通知千里眼端口改变了
				frm1.reply = 0x01;
				DataPush((void*)&frm1, (DWORD)sizeof(frm1), DEV_DIAODU, DEV_SOCK, LV2);
				
// 				tag2DDvrCfg objMvrCfg;
// 				GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 				objMvrCfg.IpPortPara.m_usCommPort = htons((ushort)atol(str));
// 				SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
// 				char by = 0x0c;//通知DvrExe 端口改变了
// 				DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
			}
		}
		break;

	case TYPE_UDP_PORT1:
		{
			char* pStop = NULL;
 			unsigned long ulPort = strtoul(str, &pStop, 10);

 			if( ulPort < 1 || ulPort > 65535 )
			{
				g_hst.ActivateStringPrompt("设置失败!");
				break;
			}
			else
			{
				g_hst.ActivateStringPrompt("设置成功!");
			}

			if (m_gprssta == 12)
			{
				cfg[0].m_usQianUdpPort = htons( (ushort)atol(str) );
			} 
			else if (m_gprssta == 2)
			{
				cfg2.m_usLiuPort = htons( (ushort)atol(str) );
			}	
			
			if(atol(str)!=0)
			{
				if (m_gprssta == 12)
				{
					SetImpCfg(cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );
					
					Frm35 frm1;		//通知千里眼端口改变了
					frm1.reply = 0x01;
					DataPush((void*)&frm1, (DWORD)sizeof(frm1), DEV_DIAODU, DEV_SOCK, LV2);
					
// 					tag2DDvrCfg objMvrCfg;
// 					GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 					objMvrCfg.IpPortPara.m_usPhotoPort = htons((ushort)atol(str));
// 					SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
// 					char by = 0x0c;//通知DvrExe 端口改变了
// 					DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
				} 
				else if (m_gprssta == 2)
				{
					SetImpCfg(&cfg2, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) );
					
					char buf[2] = {0x01, 0x03};		//通知流媒体
 					DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);
 					
//  					tag2DDvrCfg objMvrCfg;
// 					GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 					objMvrCfg.IpPortPara.m_usUpdatePort = htons((ushort)atol(str));
// 					SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
//  					char by = 0x0c;//通知DvrExe 端口改变了
// 					DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
				}
			}
		}
		break;
	case TYPE_UDP_PORT2:
		{
			char* pStop = NULL;
			unsigned long ulPort = strtoul(str, &pStop, 10);
			
			if( ulPort < 1 || ulPort > 65535 )
			{
				g_hst.ActivateStringPrompt("设置失败!");
				break;
			}
			else
			{
				g_hst.ActivateStringPrompt("设置成功!");
			}
			
			if (m_gprssta == 12)
			{
				cfg[0].m_usVUdpPort = htons( (ushort)atol(str) );
				
			} else if(m_gprssta == 2)
			{
				cfg2.m_usLiuPort = htons( (ushort)atol(str) );
			}	
			
			if(atol(str)!=0)
			{
				if (m_gprssta == 12)
				{
					SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );
					
					Frm35 frm1;		//通知千里眼端口改变了
					frm1.reply = 0x01;
					DataPush((void*)&frm1, (DWORD)sizeof(frm1), DEV_DIAODU, DEV_SOCK, LV2);
					
// 					tag2DDvrCfg objMvrCfg;
// 					GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 					objMvrCfg.IpPortPara.m_usPhotoPort = htons((ushort)atol(str));
// 					SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
// 					char by = 0x0c;//通知DvrExe 端口改变了
// 					DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
				} 
				else if (m_gprssta == 2)
				{
					SetImpCfg(&cfg2, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) );
					
					char buf[2] = {0x01, 0x03};		//通知流媒体
					DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);
					
// 					tag2DDvrCfg objMvrCfg;
// 					GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 					objMvrCfg.IpPortPara.m_usUpdatePort = htons((ushort)atol(str));
// 					SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
// 					char by = 0x0c;//通知DvrExe 端口改变了
// 					DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
				}
			}
		}
		break;
	case TYPE_TCP_IP1:
		{
			cfg[0].m_ulQianTcpIP = inet_addr(str);

			if(inet_addr(str)!=0) 
			{ 
				SetImpCfg(cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );

				Frm35 frm1;		//通知千里眼IP改变了
				frm1.reply = 0x01;
				DataPush((void*)&frm1, (DWORD)sizeof(frm1), DEV_DIAODU, DEV_SOCK, LV2);
				
// 				tag2DDvrCfg objMvrCfg;
// 				GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 				objMvrCfg.IpPortPara.m_ulCommIP = inet_addr(str);
// 				SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
// 				char by = 0x0c;//通知DvrExe IP改变了
// 				DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
			}
		}
		break;

	case TYPE_TCP_IP2:
		{
			cfg[1].m_ulQianTcpIP = inet_addr(str);

			if(inet_addr(str)!=0) 
			{ 
				SetImpCfg(cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );
				
				Frm35 frm1;		//通知千里眼IP改变了
				frm1.reply = 0x01;
				DataPush((void*)&frm1, (DWORD)sizeof(frm1), DEV_DIAODU, DEV_SOCK, LV2);
				
// 				tag2DDvrCfg objMvrCfg;
// 				GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 				objMvrCfg.IpPortPara.m_ulCommIP = inet_addr(str);
// 				SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
// 				char by = 0x0c;//通知DvrExe IP改变了
// 				DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
			}
		}
		break;

	case TYPE_UDP_IP1:
		{
			if (m_gprssta == 11) 
			{
				cfg[0].m_ulQianUdpIP = inet_addr(str);
			} 
			else if (m_gprssta == 1)
			{
				cfg2.m_ulLiuIP = inet_addr(str);
			}
			
			if(inet_addr(str)!=0)
			{
				if (m_gprssta == 11)
				{
					SetImpCfg(cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );
					
					Frm35 frm1;		//通知千里眼IP改变了
					frm1.reply = 0x01;
					DataPush((void*)&frm1, (DWORD)sizeof(frm1), DEV_DIAODU, DEV_SOCK, LV2);
					
// 					tag2DDvrCfg objMvrCfg;
// 					GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 					objMvrCfg.IpPortPara.m_ulPhotoIP = inet_addr(str);
// 					SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
// 					char by = 0x0c;//通知DvrExe IP改变了
// 					DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
				} 
				else if (m_gprssta == 1) 
				{
					SetImpCfg(&cfg2, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) );
					
					char buf[2] = {0x01, 0x03};		//通知流媒体
 					DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);
 					
//  					tag2DDvrCfg objMvrCfg;
// 					GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 					objMvrCfg.IpPortPara.m_ulUpdateIP = inet_addr(str);
// 					SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
//  					char by = 0x0c;//通知DvrExe IP改变了
// 					DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
				}				
			}
		}
		break;
	case TYPE_UDP_IP2:
		{
			if (m_gprssta == 11) 
			{
				cfg[0].m_ulVUdpIP = inet_addr(str);
	
			} else if(m_gprssta == 1)
			{
				cfg2.m_ulLiuIP = inet_addr(str);
			}
			
			if(inet_addr(str)!=0)
			{
				if (m_gprssta == 11)
				{
					SetImpCfg(cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );
					
					Frm35 frm1;		//通知千里眼IP改变了
					frm1.reply = 0x01;
					DataPush((void*)&frm1, (DWORD)sizeof(frm1), DEV_DIAODU, DEV_SOCK, LV2);
					
// 					tag2DDvrCfg objMvrCfg;
// 					GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 					objMvrCfg.IpPortPara.m_ulPhotoIP = inet_addr(str);
// 					SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
// 					char by = 0x0c;//通知DvrExe IP改变了
// 					DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
				} 
				else if (m_gprssta == 1) 
				{
					SetImpCfg(&cfg2, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) );
					
					char buf[2] = {0x01, 0x03};		//通知流媒体
					DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);
					
// 					tag2DDvrCfg objMvrCfg;
// 					GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 					objMvrCfg.IpPortPara.m_ulUpdateIP = inet_addr(str);
// 					SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));	
// 					char by = 0x0c;//通知DvrExe IP改变了
// 					DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
				}				
			}
		}
		break;

	case TYPE_APN:
		{
			memset(cfg[0].m_szApn, 0, sizeof(cfg[0].m_szApn) );
			memcpy(cfg[0].m_szApn, str, min(19,strlen(str)) );

			if(strlen(str)!=0) 
			{ 
				SetImpCfg(cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );
			}
			
// 			tag2DDvrCfg objMvrCfg;
// 		  GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 			memset(objMvrCfg.IpPortPara.m_szApn, 0, sizeof(objMvrCfg.IpPortPara.m_szApn) );
// 			memcpy(objMvrCfg.IpPortPara.m_szApn, str, min(19,strlen(str)) );
// 			SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
// 			char by = 0x0c;//通知DvrExe APN改变了
// 			DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
		}
		break;

	case TYPE_TEL:
		{
			tag1PComuCfg cfg_tel;
			GetImpCfg( &cfg_tel, sizeof(cfg_tel), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg_tel) );
			memset(cfg_tel.m_szTel, 0, sizeof(cfg_tel.m_szTel) );
			memcpy(cfg_tel.m_szTel, str, min(15,strlen(str)) );

			if(strlen(str)!=0)

			{
#if USE_LIAONING_SANQI == 1
				cfg_tel.m_bRegsiter = false;
#endif
				SetImpCfg(&cfg_tel, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg_tel) );
			}

			Frm35 frm1;		//通知千里眼手机号码改变了
			frm1.reply = 0x01;
			DataPush((void*)&frm1, (DWORD)sizeof(frm1), DEV_DIAODU, DEV_SOCK, LV2);

			char buf[2] = {0x01, 0x03};		//通知流媒体
 			DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);
 			
 			tag2DDvrCfg objMvrCfg;
		  GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
			memset(objMvrCfg.IpPortPara.m_szTelNum, 0, sizeof(objMvrCfg.IpPortPara.m_szTelNum) );
			memcpy(objMvrCfg.IpPortPara.m_szTelNum, str, min(15,strlen(str)) );
			SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
 			char by = 0x0c;//通知DvrExe 手机号码改变了
			DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
#if USE_LIAONING_SANQI == 1
			char szbuf[2];
			szbuf[0] = 0x01;	// 0x01 表示非中心协议帧
			szbuf[1] = 0x01;	// 0x01 表示TCP套接字连接成功
			DataPush((void*)szbuf, 2, DEV_SOCK, DEV_QIAN, LV3); 
#endif
		}
		break;

	case TYPE_LISTENTEL:		//设置手柄监听电话快捷键,默认为"611"
		{
			tag2QHstCfg obj2QHstCfg;
			GetSecCfg(&obj2QHstCfg,sizeof(obj2QHstCfg), offsetof(tagSecondCfg, m_uni2QHstCfg.m_obj2QHstCfg), sizeof(obj2QHstCfg) );

			memset( obj2QHstCfg.m_szListenTel,0x00,sizeof(obj2QHstCfg.m_szListenTel) );
			memcpy(obj2QHstCfg.m_szListenTel, str, len);
			memcpy(&m_HstCfg,&obj2QHstCfg,sizeof(m_HstCfg));

			SaveHstConfig();
		}
		break;

	default:
		break;
	}
}

void CHstApp::ACK_SetTCPPort1(void)
{
	set_gprs.port= g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();
	
    if (set_gprs.port > 0)
	{
		Set_GPRS_PARA(TYPE_TCP_PORT1 ,set_gprs.port, m_recvbuf);
		memset(m_recvbuf, 0 ,SIZE_RECVBUF);	//设置完成后清空RecvBuf,便于下次输入
    }
}

void CHstApp::SetTCPPort1(void)
{ 
	tag1QIpCfg cfg[2];
	GetImpCfg( cfg, 2*sizeof(tag1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), 2*sizeof(tag1QIpCfg) );

	char str[100] = {0};
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_DIGITAL | EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 5;
    EditorEntry.LabelPtr = "TCP端口号1:     ";
    EditorEntry.AckProc  = ACK_SetTCPPort1;
    //EditorEntry.InitLen = 5;
	sprintf(str, "%d", ntohs(cfg[0].m_usQianTcpPort));

	EditorEntry.InitPtr = (INT8U *)str;
	EditorEntry.InitLen = strlen(str);

    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::ACK_SetTCPPort2(void)
{
	set_gprs.port= g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();
	
    if (set_gprs.port > 0)
	{
		Set_GPRS_PARA(TYPE_TCP_PORT2 ,set_gprs.port, m_recvbuf);
		memset(m_recvbuf, 0 ,SIZE_RECVBUF);	//设置完成后清空RecvBuf,便于下次输入
    }
}

void CHstApp::SetTCPPort2(void)
{ 
	tag1QIpCfg cfg[2];
	GetImpCfg( cfg, 2*sizeof(tag1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), 2*sizeof(tag1QIpCfg) );
	
	char str[100] = {0};
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_DIGITAL | EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 5;
    EditorEntry.LabelPtr = "TCP端口号2:     ";
    EditorEntry.AckProc  = ACK_SetTCPPort2;
    //EditorEntry.InitLen = 5;
	sprintf(str, "%d", ntohs(cfg[1].m_usQianTcpPort));
	
	EditorEntry.InitPtr = (INT8U *)str;
	EditorEntry.InitLen = strlen(str);
	
    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::ACK_SetUDPPort1(void)
{
    set_gprs.port = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (set_gprs.port > 0) 
	{
		Set_GPRS_PARA(TYPE_UDP_PORT1 ,set_gprs.port, m_recvbuf);
		memset(m_recvbuf, 0 ,SIZE_RECVBUF);
    }
}

void CHstApp::SetUDPPort1(void)
{ 
	m_gprssta = 12;

	tag1QIpCfg cfg[2];
	GetImpCfg( cfg, 2*sizeof(tag1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), 2*sizeof(tag1QIpCfg) );

	char str[100] = {0};
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_DIGITAL | EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 5;
    EditorEntry.LabelPtr = "UDP端口号1:     ";
    EditorEntry.AckProc  = ACK_SetUDPPort1;
    //EditorEntry.InitLen = 5;
	sprintf(str, "%d", ntohs(cfg[0].m_usQianUdpPort));

    EditorEntry.InitPtr = (INT8U *)str;
	EditorEntry.InitLen = strlen(str);
  
    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::ACK_SetUDPPort2(void)
{
    set_gprs.port = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();
	
    if (set_gprs.port > 0) 
	{
		Set_GPRS_PARA(TYPE_UDP_PORT2 ,set_gprs.port, m_recvbuf);
		memset(m_recvbuf, 0 ,SIZE_RECVBUF);
    }
}

void CHstApp::SetUDPPort2(void)
{ 
	m_gprssta = 12;
	
	tag1QIpCfg cfg[2];
	GetImpCfg( cfg, 2*sizeof(tag1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), 2*sizeof(tag1QIpCfg) );
	
	char str[100] = {0};
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_DIGITAL | EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 5;
    EditorEntry.LabelPtr = "UDP端口号2:     ";
    EditorEntry.AckProc  = ACK_SetUDPPort2;
    //EditorEntry.InitLen = 5;
	sprintf(str, "%d", ntohs(cfg[0].m_usVUdpPort));
	
    EditorEntry.InitPtr = (INT8U *)str;
	EditorEntry.InitLen = strlen(str);
	
    g_hst.CreateEditor(&EditorEntry);
}

bool CHstApp::CheckIPValid(INT8U len)
{
	INT16U usTmp1,usTmp2,usTmp3,usTmp4;
	usTmp1 = g_hst.SearchDigitalString(m_recvbuf, len, '.',  1);
	if (usTmp1==0xffff || usTmp1>255 )//|| usTmp1 <0 )
		return false;

	usTmp2 = g_hst.SearchDigitalString(m_recvbuf, len, '.',  2);
	if (usTmp2==0xffff || usTmp2>255 )//|| usTmp2 <0 )
		return false;

	usTmp3 = g_hst.SearchDigitalString(m_recvbuf, len, '.',  3);
	if (usTmp3==0xffff || usTmp3>255 )//|| usTmp3 <0 ) 
		return false;

	usTmp4 = g_hst.SearchDigitalString(m_recvbuf, len, '\0', 1);
	if (usTmp4==0xffff || usTmp4>255 )//|| usTmp4 <0 ) 
		return false;

	if (!usTmp1 && !usTmp2 && !usTmp3 && !usTmp4 )  
		return false;

	if (255==usTmp1 && 255==usTmp2 && 255==usTmp3 && 255==usTmp4 )
		return false;

    return true;
}

void CHstApp::ACK_SetTCPIP1(void)
{
    set_gprs.ip = g_hst.GetEditorData(m_recvbuf);

    if (set_gprs.ip > 0) 
	{
        m_recvbuf[set_gprs.ip++] = '\0';
        if (!CheckIPValid(set_gprs.ip))
		{
            g_hst.ActivateStringPrompt("IP地址有误!");
        } 
		else 
		{
            g_hst.ExitEditor();
            g_hst.ActivateStringPrompt("设置成功!");

			Set_GPRS_PARA(TYPE_TCP_IP1, set_gprs.ip, m_recvbuf);
        }
    }
	else 
	{
        g_hst.ExitEditor();
    }

	memset(m_recvbuf, 0 ,SIZE_RECVBUF);
}

void CHstApp::SetTCPIP1(void)
{
	tag1QIpCfg cfg[2];
	GetImpCfg( cfg, 2*sizeof(tag1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), 2*sizeof(tag1QIpCfg) );
	
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode       = EDT_CURSOR | EDT_DIGITAL | EDT_PULS;
    EditorEntry.MinLen     = 0;
	EditorEntry.MaxLen     = 15;
    EditorEntry.LabelPtr   = "TCPIP地址1:  ";
    EditorEntry.AckProc    = ACK_SetTCPIP1;
    EditorEntry.FilterProc = FilterProc;
    EditorEntry.InitLen	= strlen(inet_ntoa(*(in_addr*)&(cfg[0].m_ulQianTcpIP)));
    EditorEntry.InitPtr = (INT8U *)inet_ntoa(*(in_addr*)&(cfg[0].m_ulQianTcpIP));
	
    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::ACK_SetTCPIP2(void)
{
    set_gprs.ip = g_hst.GetEditorData(m_recvbuf);
	
    if (set_gprs.ip > 0) 
	{
        m_recvbuf[set_gprs.ip++] = '\0';
        if (!CheckIPValid(set_gprs.ip))
		{
            g_hst.ActivateStringPrompt("IP地址有误!");
        } 
		else 
		{
            g_hst.ExitEditor();
            g_hst.ActivateStringPrompt("设置成功!");
			
			Set_GPRS_PARA(TYPE_TCP_IP2, set_gprs.ip, m_recvbuf);
        }
    }
	else 
	{
        g_hst.ExitEditor();
    }
	
	memset(m_recvbuf, 0 ,SIZE_RECVBUF);
}

void CHstApp::SetTCPIP2(void)
{
	tag1QIpCfg cfg[2];
	GetImpCfg( cfg, 2*sizeof(tag1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), 2*sizeof(tag1QIpCfg) );
	
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode       = EDT_CURSOR | EDT_DIGITAL | EDT_PULS;
    EditorEntry.MinLen     = 0;
	EditorEntry.MaxLen     = 15;
    EditorEntry.LabelPtr   = "TCPIP地址2:  ";
    EditorEntry.AckProc    = ACK_SetTCPIP2;
    EditorEntry.FilterProc = FilterProc;
    EditorEntry.InitLen	= strlen(inet_ntoa(*(in_addr*)&(cfg[1].m_ulQianTcpIP)));
    EditorEntry.InitPtr = (INT8U *)inet_ntoa(*(in_addr*)&(cfg[1].m_ulQianTcpIP));
	
    g_hst.CreateEditor(&EditorEntry);
}


void CHstApp::ACK_SetUDPIP1(void)
{
    set_gprs.ip = g_hst.GetEditorData(m_recvbuf);
    
	if (set_gprs.ip > 0)
	{
        m_recvbuf[set_gprs.ip++] = '\0';
        if (!CheckIPValid(set_gprs.ip))
		{
            g_hst.ActivateStringPrompt("IP地址有误!");
        }
		else 
		{
            g_hst.ExitEditor();
            g_hst.ActivateStringPrompt("设置成功!");

			Set_GPRS_PARA(TYPE_UDP_IP1, set_gprs.ip, m_recvbuf);
        }
    } 
	else 
	{
        g_hst.ExitEditor();
    }

	memset(m_recvbuf, 0 ,SIZE_RECVBUF);
}

void CHstApp::SetUDPIP1(void)
{
	m_gprssta = 11;

	tag1QIpCfg cfg[2];
	GetImpCfg( cfg, 2*sizeof(tag1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), 2*sizeof(tag1QIpCfg) );
	
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode       = EDT_CURSOR | EDT_DIGITAL | EDT_PULS;
    EditorEntry.MinLen     = 0;
	EditorEntry.MaxLen     = 15;
    EditorEntry.LabelPtr   = "UDPIP地址1:  ";
    EditorEntry.AckProc    = ACK_SetUDPIP1;
    EditorEntry.FilterProc = FilterProc;
    EditorEntry.InitLen = strlen(inet_ntoa(*(in_addr*)&(cfg[0].m_ulQianUdpIP)));
    EditorEntry.InitPtr = (INT8U *)inet_ntoa(*(in_addr*)&(cfg[0].m_ulQianUdpIP));
	
    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::ACK_SetUDPIP2(void)
{
    set_gprs.ip = g_hst.GetEditorData(m_recvbuf);
    
	if (set_gprs.ip > 0)
	{
        m_recvbuf[set_gprs.ip++] = '\0';
        if (!CheckIPValid(set_gprs.ip))
		{
            g_hst.ActivateStringPrompt("IP地址有误!");
        }
		else 
		{
            g_hst.ExitEditor();
            g_hst.ActivateStringPrompt("设置成功!");
			
			Set_GPRS_PARA(TYPE_UDP_IP2, set_gprs.ip, m_recvbuf);
        }
    } 
	else 
	{
        g_hst.ExitEditor();
    }
	
	memset(m_recvbuf, 0 ,SIZE_RECVBUF);
}

void CHstApp::SetUDPIP2(void)
{
	m_gprssta = 11;
	tag1QIpCfg cfg[2];
	GetImpCfg( cfg, 2*sizeof(tag1QIpCfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), 2*sizeof(tag1QIpCfg) );
	
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode       = EDT_CURSOR | EDT_DIGITAL | EDT_PULS;
    EditorEntry.MinLen     = 0;
	EditorEntry.MaxLen     = 15;
    EditorEntry.LabelPtr   = "UDPIP地址2:  ";
    EditorEntry.AckProc    = ACK_SetUDPIP2;
    EditorEntry.FilterProc = FilterProc;
    EditorEntry.InitLen = strlen(inet_ntoa(*(in_addr*)&(cfg[0].m_ulVUdpIP)));
    EditorEntry.InitPtr = (INT8U *)inet_ntoa(*(in_addr*)&(cfg[0].m_ulVUdpIP));
	
    g_hst.CreateEditor(&EditorEntry);
}

bool CHstApp::FilterProc(INT8U *ch)
{
    if (*ch < '0' || *ch > '9')
	{
        *ch = '.';
        return true;
    } 
	else 
	{
        return false;
    }
}


void CHstApp::ACK_SetAPN(void)
{
    set_gprs.apn = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (set_gprs.apn > 0)
	{
        g_hst.ActivateStringPrompt("设置成功!");

		Set_GPRS_PARA(TYPE_APN, set_gprs.apn, m_recvbuf);
		memset(m_recvbuf, 0 ,SIZE_RECVBUF);
    }
}

void CHstApp::SetAPN(void)
{
	tag1QIpCfg cfg;
	GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );

    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = sizeof(cfg.m_szApn)-1;
    EditorEntry.LabelPtr = "APN:        ";
    EditorEntry.AckProc  = ACK_SetAPN;
    EditorEntry.InitLen = strlen(cfg.m_szApn);
    EditorEntry.InitPtr = (INT8U *)cfg.m_szApn;
    
    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::ACK_ExitDrvPro(void)
{
	PRTMSG(MSG_NOR, "ACK_ExitDrvPro Log=%d.\n",m_bDrvHaveLog);

	if (!m_bDrvHaveLog) 
	{
		char buf[ 64 ];
		memset( buf, 0, sizeof(buf) );
		buf[0] = 0x62;

		DWORD dwPacketNum;
		g_objDiaoduOrHandWorkMng.PushData(LV1, 1, buf, dwPacketNum);
	}
	else
	{
		_KillTimer(&g_objTimerMng, DRV_LOG_TIMER);
	}
}

void CHstApp::P_TmIsOnLine()
{
	char buf[] = {0x0b} ;
	DataPush(buf, 1, DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::P_TmDrvRec()
{
	g_hst.RequestRing(0x08, 1);		//响铃提示
}

void CHstApp::InputDrvLog(void)
{
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_PASSWORD | EDT_DIGITAL;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 10;
    EditorEntry.LabelPtr = "请输司机号:    ";
#if USE_DRIVERCTRL == 1
    EditorEntry.AckProc  = ACK_InputDrvNum;
#else
    EditorEntry.AckProc  = ACK_InputDrvNum_AD;
#endif

	EditorEntry.ExitProc = ACK_ExitDrvPro;

    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::ACK_InputDrvNum_AD(void)
{
	int i;
	int len;
	byte ucCurNum;

	DrvCfg m_cfg;
	unsigned int offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
	GetSecCfg(&m_cfg, sizeof(m_cfg), offset, sizeof(m_cfg));
	len = g_hst.GetEditorData(m_recvbuf);

	if (len > 0) 
	{
		for (i = 0;i<MAX_DRIVER;i++) 
		{
			ucCurNum = m_cfg.inf[i].num;
			if (!strncmp((char*)m_recvbuf, m_cfg.inf[i].ascii,min(7,strlen((char*)m_recvbuf)))) 
			{
				char resbuf[16] = {0};
				resbuf[0] = 0x63;
				resbuf[1] = 0x01;
				resbuf[2] = 0;
				resbuf[3] = ucCurNum;

				DataPush(resbuf, 4, DEV_DIAODU, DEV_QIAN, LV2);

				g_hst.ActivateStringPrompt("恭喜啦\n登陆成功");
				
				sleep(1);
				m_bDrvHaveLog = TRUE;
				g_hst.ExitEditor();
				g_hst.ActivateLowestWinStack();

				return;
			}
		}
	}

	if (i == MAX_DRIVER  || 0 == len) 
	{
		char resbuf[16] = {0};
		resbuf[0] = 0x63;
		resbuf[1] = 0x02;
		resbuf[2] = 0;
		resbuf[3] = ucCurNum;
		DataPush(resbuf, 4, DEV_DIAODU, DEV_QIAN, LV2);//要发4个字节，不然可能会错啊
	
		g_hst.ActivateStringPrompt("对不起\n号码有误");	
		sleep(1);

		m_bDrvHaveLog = FALSE;
		g_hst.ExitEditor();
	}
}

void CHstApp::ACK_InputDrvNum(void)
{
	INT8U len;
	int iRet;
	tag2QDriverCfg objDrvrCfg;

	iRet = GetSecCfg( &objDrvrCfg, sizeof(objDrvrCfg),offsetof(tagSecondCfg, m_uni2QDriverCfg.m_obj2QDriverCfg), sizeof(objDrvrCfg) );

    len = g_hst.GetEditorData(m_recvbuf);

	tag2QDriverPar_QIAN* aryDrvrPar = objDrvrCfg.m_aryDriver;

	size_t siz ;
	WORD usCurNum;
	WORD wTemp;
	const size_t DRIVER_MAXCOUNT = sizeof(objDrvrCfg.m_aryDriver) / sizeof(objDrvrCfg.m_aryDriver[0]);

	if (len > 0) 
	{
		for( siz = 0; siz < DRIVER_MAXCOUNT; siz ++ )
		{
			// 检查该记录是否存在有效司机信息
			if( aryDrvrPar[siz].m_wDriverNo < 1 )
				continue;

			usCurNum = htons( aryDrvrPar[siz].m_wDriverNo );

			if( 0 == aryDrvrPar[siz].m_szIdentID[ sizeof(aryDrvrPar[0].m_szIdentID) - 1] )
				wTemp = strlen(aryDrvrPar[siz].m_szIdentID);
			else
				wTemp = sizeof(aryDrvrPar[siz].m_szIdentID);
			if( wTemp > sizeof(aryDrvrPar[siz].m_szIdentID) ) wTemp = sizeof(aryDrvrPar[siz].m_szIdentID);

			if (!strncmp((char*)m_recvbuf, aryDrvrPar[siz].m_szIdentID,wTemp)) 
			{
				char resbuf[16] = {0};
				resbuf[0] = 0x63;
				resbuf[1] = 0x01;
				resbuf[2] = usCurNum/0x100;
				resbuf[3] = usCurNum%0x100;
				DataPush(resbuf, 4, DEV_DIAODU, DEV_QIAN, LV2);

				g_hst.ActivateStringPrompt("恭喜\n登陆成功");
				sleep(1);

				m_bDrvHaveLog = TRUE;
				g_hst.ExitEditor();
				g_hst.ActivateLowestWinStack();

				return;
			}
		}			
	}

	if (siz == DRIVER_MAXCOUNT  || 0 == len) 
	{
		char resbuf[16] = {0};
		resbuf[0] = 0x63;
		resbuf[1] = 0x02;
		resbuf[2] = usCurNum/0x100;
		resbuf[3] = usCurNum%0x100;
		DataPush(resbuf, 4, DEV_DIAODU, DEV_QIAN, LV2);//要发4个字节，不然可能会错啊
	
		g_hst.ActivateStringPrompt("对不起\n号码有误");
		sleep(1);

		m_bDrvHaveLog = FALSE;
		g_hst.ExitEditor();
	}
}

void CHstApp::ACK_SetTel(void)
{
    set_gprs.tel = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (set_gprs.tel > 0)
	{
        g_hst.ActivateStringPrompt("设置成功!");

		Set_GPRS_PARA(TYPE_TEL, set_gprs.tel, m_recvbuf);
		memset(m_recvbuf, 0 ,SIZE_RECVBUF);
    }
}

void CHstApp::SetTel(void)
{
	tag1PComuCfg cfg_tel;
	GetImpCfg( &cfg_tel, sizeof(cfg_tel), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg_tel) );
	
	memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_DIGITAL | EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 15; 
    EditorEntry.LabelPtr = "本机号码:   ";
    EditorEntry.AckProc  = ACK_SetTel;
	EditorEntry.InitPtr = (INT8U *)cfg_tel.m_szTel;	//手机号设置完成后,以后每次显示上次设置的手机号

    EditorEntry.InitLen = strlen(cfg_tel.m_szTel);
    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::ACK_SetListenTel(void)
{
    set_gprs.listentel = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (set_gprs.listentel > 0) 
	{
        g_hst.ActivateStringPrompt("设置成功!");

		Set_GPRS_PARA(TYPE_LISTENTEL, set_gprs.listentel, m_recvbuf);
		memset(m_recvbuf, 0 ,SIZE_RECVBUF);
	}
}

void CHstApp::SetCdmaName(void)
{
	tag1PComuCfg obj1PComuCfg;
	int iRet = GetImpCfg( &obj1PComuCfg, sizeof(obj1PComuCfg),offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );

	PRTMSG(MSG_NOR, "cdma name:%s\n",obj1PComuCfg.m_szCdmaUserName);

	memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_CHINESE | EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 20;  //cyh add:del

    EditorEntry.LabelPtr = "CDMA用户名:   ";
    EditorEntry.AckProc  = ACK_SetCdmaName;
    EditorEntry.InitLen = strlen(obj1PComuCfg.m_szCdmaUserName);	
	EditorEntry.InitPtr = (INT8U *)obj1PComuCfg.m_szCdmaUserName;

    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::ACK_SetCdmaName(void)
{
    set_gprs.cdmaname = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (set_gprs.cdmaname > 0) 
	{
        g_hst.ActivateStringPrompt("设置成功!");

		Set_GPRS_PARA(TYPE_CDMANAME, set_gprs.cdmaname, m_recvbuf);
		memset(m_recvbuf, 0 ,SIZE_RECVBUF);
	}
}

void CHstApp::SetCdmaPass(void)
{
	tag1PComuCfg obj1PComuCfg;
	int iRet = GetImpCfg( &obj1PComuCfg, sizeof(obj1PComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );

	memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_CHINESE | EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 20;
    EditorEntry.LabelPtr = "CDMA密码:   ";
    EditorEntry.AckProc  = ACK_SetCdmaPass;
    EditorEntry.InitLen = strlen(obj1PComuCfg.m_szCdmaPassWord);
	EditorEntry.InitPtr = (INT8U *)obj1PComuCfg.m_szCdmaPassWord;

    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::ACK_SetCdmaPass(void)
{
    set_gprs.cdmapass = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (set_gprs.cdmapass > 0)
	{
        g_hst.ActivateStringPrompt("设置成功!");
		Set_GPRS_PARA(TYPE_CDMAPASS, set_gprs.cdmapass, m_recvbuf);
		memset(m_recvbuf, 0 ,SIZE_RECVBUF);
	}
}

void CHstApp::SetListenTel(void)
{
	memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_DIGITAL | EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 20; 
    EditorEntry.LabelPtr = "监听号码:   ";
    EditorEntry.AckProc  = ACK_SetListenTel;
    EditorEntry.InitLen = 13; 	
	EditorEntry.InitPtr = (INT8U *)m_HstCfg.m_szListenTel;

    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::SetLiuIP()
{
	m_gprssta = 1;
	tag1LComuCfg cfg;
	GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg) );

    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode       = EDT_CURSOR | EDT_DIGITAL | EDT_PULS;
    EditorEntry.MinLen     = 0;
    EditorEntry.MaxLen     = sizeof(inet_ntoa(*(in_addr*)&cfg.m_ulLiuIP))+50;
    EditorEntry.LabelPtr   = "流媒体IP:   ";
    EditorEntry.AckProc    = ACK_SetUDPIP1;
    EditorEntry.FilterProc = FilterProc;
    EditorEntry.InitLen	= strlen(inet_ntoa(*(in_addr*)&cfg.m_ulLiuIP));
    EditorEntry.InitPtr = (INT8U *)inet_ntoa(*(in_addr*)&cfg.m_ulLiuIP);
	
    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::SetLiuPort()
{
	m_gprssta = 2;
	tag1LComuCfg cfg;
	GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg) );

	char str[100] = {0};
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_DIGITAL | EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 6;
    EditorEntry.LabelPtr = "流媒体端口: ";
    EditorEntry.AckProc  = ACK_SetUDPPort1;
    EditorEntry.InitLen = 5;
	sprintf(str, "%d", ntohs(cfg.m_usLiuPort));
    EditorEntry.InitPtr = (INT8U *)str;
  
    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::Activate(void)
{
    bool success;

    success = true;

    if (set_gprs.tel == 0)			success = false;
    if (set_gprs.apn == 0)			success = false;
    if (set_gprs.ip == 0)			success = false;
    if (set_gprs.port == 0)			success = false;
	if (set_gprs.listentel == 0)	success = false;
	
	g_hst.ActivateStringPrompt("激活成功!");
}

void CHstApp::ACK_InputPassword(void)
{
	INT8U len;

    len = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (len > 0)
	{
        if (strcmp((char *)m_recvbuf, (char *)ALL_PASS) == 0)
		{
            g_hst.ActivateItemMenu(&MENU_SET);
        } 
        else if(strcmp((char *)m_recvbuf, (char *)LIU_PASS) == 0) 
		{
			g_hst.ActivateItemMenu(&MENU_SETL);	
        }
		else
		{
            g_hst.ActivateStringPrompt("对不起\n密码有误");
        }
		memset(m_recvbuf, 0 ,SIZE_RECVBUF);
    }
}

void CHstApp::InputPassword(void)
{
    memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_PASSWORD | EDT_DIGITAL;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 10;
    EditorEntry.LabelPtr = "请输密码:    ";
    EditorEntry.AckProc  = ACK_InputPassword;
    g_hst.CreateEditor(&EditorEntry);
}

/****************SysSet********************************************************/
void CHstApp::ReadVersion(void)
{
	char szSoftVer[64] = {0};
	ChkSoftVer(szSoftVer);

	char szSysVer[64] = {0};
	ReadVerFile(szSysVer);

	char szVer[64] = {0};
	sprintf(szVer, "%s-%s", szSoftVer, szSysVer);

	char str[64] = {0};
	sprintf(str, "Ver:%.29s  ", szVer);

    g_hst.ActivateStringPrompt_TIME((char *)str, 0);
}

//对比度调节
void CHstApp::DisplayProc_AD(void)
{
    INT8U linesize;

    AdjustContrast();
    
    linesize = g_hst.GetScreenLineSize();
    g_hst.VTL_ClearScreen();
    g_hst.VTL_ScreenPrintStr("对比度\n ----------+");
    g_hst.VTL_AbsMovCursor(linesize + 2);
    g_hst.VTL_ScreenPrintChar(0x83, m_curvalue);

    g_hst.OverWriteScreen();
}

void CHstApp::TurnUp(void)
{
    if (m_curvalue < MAX_STEP)
		m_curvalue++;

    DisplayProc_AD();
}

void CHstApp::TurnDown(void)
{
    if (m_curvalue > 0) 
		m_curvalue--;

    DisplayProc_AD();
}

void CHstApp::HdlKeyProc_AD(void)
{
	if (g_hst.GetKeyMsg(&m_curkey))
	{
		switch (m_curkey.key)
		{
			case KEY_UP:
				TurnUp();
				break;
			case KEY_DOWN:
				TurnDown();
				break;
			case KEY_ACK:
				{			
					g_hst.PopWinStack();
					g_hst.ActivateStringPrompt("设置成功!");
				}
				break;
			case KEY_NAK:
				{			
					g_hst.PopWinStack();
				}
				break;
			default:
				break;
		}
	}
}

void CHstApp::ActivateADConst(void)
{
	WinStackEntry_AD.InitProc = DisplayProc_AD;
	WinStackEntry_AD.DestroyProc = 0;
	WinStackEntry_AD.SuspendProc = 0;
	WinStackEntry_AD.ResumeProc = DisplayProc_AD;
	WinStackEntry_AD.HdlKeyProc = HdlKeyProc_AD;

    g_hst.PushWinStack(&WinStackEntry_AD);
}

void CHstApp::InitADConst(void)
{
    m_curvalue = INIT_STEP;
}

void CHstApp::AdjustContrast(void)
{
    g_hst.CtlHstContrast(4 + m_curvalue * VALUE_STEP);
}

//振铃音
void CHstApp::Handler_KEY_ACK_Ring(void)
{
	if (m_optype == OP_CALLRING)
	{
        m_HstCfg.m_bytcallring = g_hst.GetMenuWinRecordNum();
    }
	else
	{
        m_HstCfg.m_bytsmring   = g_hst.GetMenuWinRecordNum() + 0x06;
    }

    g_hst.CancelRing();
    g_hst.PopMenuWin();
    g_hst.ActivateStringPrompt("设置成功！");
    SaveHstConfig();
}

void CHstApp::Handler_KEY_NAK_Ring(void)
{
    g_hst.CancelRing();
}

void CHstApp::Handler_KEY_SEL_Ring(void)
{
    INT8U selnum;
    
    if (m_optype == OP_CALLRING)
	{
        selnum = g_hst.GetMenuWinRecordNum();
    } 
	else 
	{
        selnum = g_hst.GetMenuWinRecordNum() + 0x06;
    }

    g_hst.RequestRing(selnum, 1);
}

INT16U CHstApp::MaxRecordProc_Ring(void)
{
    if (m_optype == OP_CALLRING)
	{
        return 6;
    } 
	else 
	{
        return 5;
    }
}

INT16U CHstApp::InitRecordProc_Ring(void)
{
    INT8U initnum;
    
    if (m_optype == OP_CALLRING) 
	{
        initnum = m_HstCfg.m_bytcallring;
        if (initnum >= 6) initnum = 0;
    }
	else 
	{
        initnum = m_HstCfg.m_bytsmring - 6;
        if (initnum >= 5) initnum = 0;
    }
    return initnum;
}

INT16U CHstApp::ReadRecordProc_Ring(INT16S numrec, INT8U *ptr)
{
    if (m_optype == OP_CALLRING)
	{
        if (numrec >= 6) numrec = 0;
        memcpy(ptr, CallRing[numrec], strlen(CallRing[numrec]));
        return strlen(CallRing[numrec]);
    } 
	else
	{
        if (numrec >= 5) numrec = 0;
        memcpy(ptr, SMRing[numrec], strlen(SMRing[numrec]));
        return strlen(SMRing[numrec]);
    }
}

void CHstApp::SelRing(void)
{
    m_optype = g_hst.GetMenuWinRecordNum();
    g_hst.ActivateMenuWin(&MenuWinEntry);
}

//设置按键音
INT16U CHstApp::InitRecordProc_KeyPT(void)
{
    if (m_HstCfg.m_EnableKeyPrompt) {
        return 0;
    } else {
        return 1;
    }
}

void CHstApp::StoreProc_KeyPT(void)
{
    if (g_hst.GetMenuWinRecordNum() == 0) {
        m_HstCfg.m_EnableKeyPrompt = TRUE;
        g_hst.CtlKeyPrompt((char)0x80, 1);
    } else {
        m_HstCfg.m_EnableKeyPrompt = FALSE;
        g_hst.CtlKeyPrompt((char)0x00, 1);
    }
    g_hst.PopMenuWin();
    g_hst.ActivateStringPrompt("设置成功！");
    SaveHstConfig();
}

void CHstApp::ActivateSelOptionMenu(void)
{  	
	g_hst.ActivateItemMenu_Init(&MENU_SELOPTION, InitRecordProc_KeyPT);
}

//设置接听方式
INT16U CHstApp::InitRecordProc_Pick(void)
{
    if (m_HstCfg.m_EnableAutoPickup) {
        return 0;
    } else {
        return 1;
    }
}

void CHstApp::StoreProc_Pick(void)
{
    if (g_hst.GetMenuWinRecordNum() == 0) {
        m_HstCfg.m_EnableAutoPickup = false;		//自动接听
    } else {
        m_HstCfg.m_EnableAutoPickup = true;			//手动接听
    }
    g_hst.PopMenuWin();
    g_hst.ActivateStringPrompt("设置成功！");
    SaveHstConfig();
}

void CHstApp::ActivateSelOptionMenu_Pick(void)
{
    g_hst.ActivateItemMenu_Init(&MENU_SELPICK, 0);
}

//设置缺省模式
INT16U CHstApp::InitRecordProc_Cha(void)
{
    if (m_HstCfg.m_HandsfreeChannel) {
        return 0;
    } else {
        return 1;
    }
}
void CHstApp::StoreProc_Cha(void)
{
	char c = 0x2a;
    if (g_hst.GetMenuWinRecordNum() == 0)
	{
        m_HstCfg.m_HandsfreeChannel = true;
    } 
	else
	{
        m_HstCfg.m_HandsfreeChannel = false;
    }

	m_hstmode = m_HstCfg.m_HandsfreeChannel;

    g_hst.PopMenuWin();
    g_hst.ActivateStringPrompt("设置成功！");
    SaveHstConfig(); //先保存配置然后通知手机模块

	DWORD dwPacketNum;
	g_objPhoneRecvDataMng.PushData(LV1, 1, (char*)&c, dwPacketNum);
}
void CHstApp::ActivateSelOptionMenu_Cha(void)
{
	g_hst.ActivateItemMenu_Init(&MENU_SELCHA, 0);
}

/*****************ParaChk*******************************************************/
void CHstApp::GetCenterNo(void)
{
	tag2QServCodeCfg cfg2;
	GetSecCfg( &cfg2, sizeof(cfg2), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(cfg2) );

	if( 0 == strcmp(cfg2.m_szQianSmsTel, "") ) 
	{
		g_hst.ActivateStringPrompt("特服号无设置");
	}
	else
	{
		ParaPrompt.Mode        = 0;
		ParaPrompt.LiveTime    = 3;
   		ParaPrompt.TextPtr     = (INT8U *)cfg2.m_szQianSmsTel;
   		ParaPrompt.TextLen     = strlen(cfg2.m_szQianSmsTel);
   		ParaPrompt.ExitProc    = 0;
   		ParaPrompt.ExitKeyProc = 0;
   		g_hst.ActivatePrompt(&ParaPrompt);
	}
}

void CHstApp::GetGPRSStatus(void)
{
	if( NETWORK_TYPE==NETWORK_GSM )
	{
		if( 2 == g_objPhoneGsm.GetPhoneState() ) 
			g_hst.ActivateStringPrompt_TIME("在线",3);
		else 
			g_hst.ActivateStringPrompt_TIME("不在线",3);
	}
	else if( NETWORK_TYPE==NETWORK_TD )
	{
		if( 2 == g_objPhoneTd.GetPhoneState() ) 
			g_hst.ActivateStringPrompt_TIME("在线",3);
		else 
			g_hst.ActivateStringPrompt_TIME("不在线",3);
	}
	else if( NETWORK_TYPE==NETWORK_EVDO )
	{
		if( 2 == g_objPhoneEvdo.GetPhoneState() ) 
			g_hst.ActivateStringPrompt_TIME("在线",3);
		else 
			g_hst.ActivateStringPrompt_TIME("不在线",3);
	}
	else if( NETWORK_TYPE==NETWORK_WCDMA )
	{
		if( 2 == g_objPhoneWcdma.GetPhoneState() ) 
			g_hst.ActivateStringPrompt_TIME("在线",3);
		else 
			g_hst.ActivateStringPrompt_TIME("不在线",3);
	}
}

void CHstApp::GetGPSStatus(void)
{
	GPSDATA gps(0);
	GetCurGps( &gps, sizeof(gps), GPS_LSTVALID );

	if (gps.valid=='A')
		g_hst.ActivateStringPrompt_TIME("GPS数据有效",3);
	else 
		g_hst.ActivateStringPrompt_TIME("GPS数据无效",3);
}

void CHstApp::SndOilDataDesire()
{
// 	char buf[] = {0x00, 0x7e, 0x42, 0x20, 0x83, 0x00, 0x00, 0x00, 0x38, 0x01, 
// 		0x31, 0x35, 0x39, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x20, 0x20, 0x20, 0x20, 
// 		0x31, 0x35, 0x39, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x20, 0x20, 0x20, 0x20, 
// 		0x02, 0x03, 0x01, 0x01, 0x01, 0x02, 0x01, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x0f, 0x7e};
// 	DataPush(buf, sizeof(buf), DEV_SOCK, DEV_DVR, LV2);
// 	return;
	
	g_hst.ActivateStringPrompt("数据请求中…");
    FrmOil frm;
	frm.type = 0xC1;
	frm.flag = 0x01;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

//------------------------------------hhq
void CHstApp::SwhChn1View()
{
	g_hst.ActivateStringPrompt("预览1通道…");

	char buf[2] = {0x0d, 0x00};
	DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV2);
}

void CHstApp::SwhChn2View()
{
	g_hst.ActivateStringPrompt("预览2通道…");
    
	char buf[2] = {0x0d, 0x01};
	DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV2);
}

void CHstApp::SwhChn3View()
{
	g_hst.ActivateStringPrompt("预览3通道…");
    
	char buf[2] = {0x0d, 0x02};
	DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV2);
}

void CHstApp::SwhChn4View()
{
	g_hst.ActivateStringPrompt("预览4通道…");
    
	char buf[2] = {0x0d, 0x03};
	DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV2);
}

void CHstApp::SwhChnAllView()
{
	g_hst.ActivateStringPrompt("预览全通道…");
    
	char buf[2] = {0x0d, 0x0a};
	DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV2);
}
//------------------------------------hhq

void CHstApp::BiaoDStart()
{
	g_hst.ActivateStringPrompt("标定起始指示…");
    FrmOil frm;
	frm.type = 0xC3;
	frm.flag = 0x03;
    DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::BiaoDEnd()
{
	g_hst.ActivateStringPrompt("标定完成指示…");
    FrmOil frm;
	frm.type = 0xC4;
	frm.flag = 0x04;
    DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SndBDData()
{
	g_hst.ActivateStringPrompt("发送标定数据…");
    FrmOil frm;
	frm.type = 0x51;
	frm.flag = 0x51;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SetZeroBD()
{
	char buf = 0x99;
	DataPush((void*)&buf, 1, DEV_DIAODU, DEV_DVR, LV2);
	return ;
	
	g_hst.ActivateStringPrompt("0刻度标定…");
    FrmC1 frm;
	frm.flag = 0x00;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SetQuerterBD()
{
	// 	char buf = 0x9a;
	// 	DataPush((void*)&buf, 1, DEV_DIAODU, DEV_DVR, LV2);
	// 	return;
	
	g_hst.ActivateStringPrompt("1/4刻度标定…");
    FrmC1 frm;
	frm.flag = 0x14;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SetHalfBD()
{
	// 	char buf = 0x9b;
	// 	DataPush((void*)&buf, 1, DEV_DIAODU, DEV_DVR, LV2);
	// 	return;
	
	g_hst.ActivateStringPrompt("1/2刻度标定…");
    FrmC1 frm;
	frm.flag = 0x12;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SetThreeFourBD()
{
	g_hst.ActivateStringPrompt("3/4刻度标定…");
    FrmC1 frm;
	frm.flag = 0x34;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SetFullBD()
{
	g_hst.ActivateStringPrompt("满刻度标定…");
    FrmC1 frm;
	frm.flag = 0x10;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SetZeroBDOFF()
{
	g_hst.ActivateStringPrompt("0刻度标定…");
    FrmC1 frm;
	frm.flag = 0xF1;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SetFullBDOFF()
{
	g_hst.ActivateStringPrompt("满刻度标定…");
    FrmC1 frm;
	frm.flag = 0xF5;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SetThreeFourBDOFF()
{
	g_hst.ActivateStringPrompt("3/4刻度标定…");
    FrmC1 frm;
	frm.flag = 0xF4;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SetHalfBDOFF()
{
	g_hst.ActivateStringPrompt("1/2刻度标定…");
    FrmC1 frm;
	frm.flag = 0xF3;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::SetQuerterBDOFF()
{
	g_hst.ActivateStringPrompt("1/4刻度标定…");
    FrmC1 frm;
	frm.flag = 0xF2;
	DataPush((void*)&frm, sizeof(frm), DEV_DIAODU, DEV_QIAN, LV2);
}

/****************AtmpMsg********************************************************/
INT16U CHstApp::MaxRecordProc_CONTENT_Atmp(void)
{
	return 1;
}

INT16U CHstApp::ReadRecordProc_CONTENT_Atmp(INT16S numrec, INT8U *ptr)
{
	numrec = numrec;
	memcpy(ptr, CurRec_Atmp.data, CurRec_Atmp.datalen);
	return CurRec_Atmp.datalen;
}

void CHstApp::Handler_KEY_ACK_Atmp(void)
{
	static INT16S	SelRecord_Atmp;

	SelRecord_Atmp = g_hst.GetMenuWinRecordNum();
	if(SelRecord_Atmp != -1)
	{
		m_msgsta = 1;

		if(GetATMPRec(SelRecord_Atmp, &CurRec_Atmp))
			g_hst.ActivateMenuWin(&MENU_READCONTENT);
    }
}

INT16U CHstApp::MaxRecordProc_Atmp(void)
{
	m_msgsta = 1;
	return GetATMPRecItem();
}

INT16U CHstApp::ReadRecordProc_Atmp(INT16S numrec, INT8U *ptr)
{
	if(numrec == -1)
	{
		memcpy(ptr, "无调度信息", strlen("无调度信息"));
		return strlen("无调度信息");
	}

	m_msgsta = 1;
	GetATMPRec(numrec, &CurRec_Atmp);

	char str_atmp[20];
	memcpy(str_atmp, (char *)CurRec_Atmp.date, 10);
	memcpy(str_atmp+10, (char *)CurRec_Atmp.time, 10);

	memcpy(ptr, str_atmp+5, 5);
	ptr[5]='-';
	memcpy(ptr+6, str_atmp+10, 5);

	return 11;
}

void CHstApp::ReadATMP(void)
{
// 	char buf[] = {0x00, 0x7e, 0x03, 0x6f, 0x08, 0x83, 0x05, 0x31, 0x10, 0x70, 0x38, 0x0a, 0x31, 0x33, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x20, 0x20, 0x20, 0x20, 0x38, 0x38, 0x39, 0x30, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x02, 0x00, 0x0b, 0x01, 0x0c, 0x0f, 0x29, 0x00, 0x0b, 0x01, 0x0c, 0x0f, 0x2a, 0x00, 0x70, 0x7e};
// 	DataPush(buf, sizeof(buf), DEV_SOCK, DEV_QIAN, LV2);

	g_hst.ActivateMenuWin(&MENU_LISTATMP);
}

/***************PhoneCallMsg*********************************************************/
INT16U CHstApp::MaxRecordProc_CONTENT_PC(void)
{
	return 1;
}

INT16U CHstApp::ReadRecordProc_CONTENT_PC(INT16S numrec, INT8U *ptr)
{
	numrec = numrec;
	memcpy(ptr, CurRec_PC.data, CurRec_PC.datalen);
	return CurRec_PC.datalen;
}

void CHstApp::Handler_KEY_ACK_PC(void)
{
	static INT16S	SelRecord_PC;
	SelRecord_PC = g_hst.GetMenuWinRecordNum();
	
	if(SelRecord_PC != -1)
	{
		m_msgsta = 2;
		if(GetATMPRec(SelRecord_PC, &CurRec_PC))	g_hst.ActivateMenuWin(&MENU_READCONTENT_PC);
    }
}

INT16U CHstApp::MaxRecordProc_PC(void)
{
	m_msgsta = 2;
	return GetATMPRecItem();
}

INT16U CHstApp::ReadRecordProc_PC(INT16S numrec, INT8U *ptr)
{
	if(numrec == -1)
	{
		memcpy(ptr, "无电召信息", strlen("无电召信息"));
		return strlen("无电召信息");
	}

	m_msgsta = 2;
	GetATMPRec(numrec, &CurRec_PC);
	char str_pc[20];
	memcpy(str_pc, (char *)CurRec_PC.date, 10);
	memcpy(str_pc+10, (char *)CurRec_PC.time, 10);

	memcpy(ptr, str_pc+5, 5);
	memcpy(ptr+5, str_pc+10, 5);

	return 10;
}

void CHstApp::ReadPC(void)
{
	g_hst.ActivateMenuWin(&MENU_LISTPC);
}

/**************ProjectMenu**********************************************************/
void CHstApp::ACK_InputPasswordProject()
{
	INT8U len;
	static bool sta_bDebug = false;
	char szPhoneMode[100] = {0};

    len = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (len > 0) 
	{
        if (strcmp((char *)m_recvbuf, ALL_PASS) == 0) 
		{
            g_hst.ActivateItemMenu(&MENU_PROJECT);
		}
		else if(strcmp((char *)m_recvbuf, EXP_PASS) == 0) 
		{
			PRTMSG(MSG_DBG, "Debuging...\n");
			g_objDiaodu.show_diaodu("密码错误");
            sta_bDebug = true;
		}
		else if( strcmp((char *)m_recvbuf, "1") == 0 && sta_bDebug )
		{
			PRTMSG(MSG_DBG, "Query SD state\n");
			// 通知DvrExe进行SD卡状态查询，并向调度屏返回提示
			char buf = 0x07;
			DataPush(&buf, 1, DEV_DIAODU, DEV_DVR, LV1);
		}
		else if( strcmp((char *)m_recvbuf, "2") == 0 && sta_bDebug )	// 视频文件导出
		{
			pthread_t thDataOutPut;
			pthread_create(&thDataOutPut, NULL, G_VideoOutPutThread, NULL);
		}
		else if( strcmp((char *)m_recvbuf, "3") == 0 && sta_bDebug )	// 复位手机模块
		{
			PRTMSG(MSG_DBG, "Reset Phone\n");
			if( NETWORK_TYPE==NETWORK_GSM )
				g_objPhoneGsm.ResetPhone();
			else if( NETWORK_TYPE==NETWORK_TD )
				g_objPhoneTd.ResetPhone();
			else if( NETWORK_TYPE==NETWORK_EVDO )
				g_objPhoneEvdo.ResetPhone();
			else if( NETWORK_TYPE==NETWORK_WCDMA )
				g_objPhoneWcdma.ResetPhone();
		}
		else if( strcmp((char *)m_recvbuf, "4") == 0 && sta_bDebug ) // 流媒体立即登陆
		{			
			char buf[2] = {0x01, 0x03};
			DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);			
		} 
		else if( strcmp((char *)m_recvbuf, "5") == 0 && sta_bDebug )
		{
			G_ResetSystem();
		}
		else if(strcmp((char *)m_recvbuf, CHOOSE_LC6311_PASS)==0)
		{
			strcpy(szPhoneMode, "LC6311");
		}
		else if(strcmp((char *)m_recvbuf, CHOOSE_MC703OLD_PASS)==0)
		{
			strcpy(szPhoneMode, "MC703OLD");
		}
		else if(strcmp((char *)m_recvbuf, CHOOSE_MC703NEW_PASS)==0)
		{
			strcpy(szPhoneMode, "MC703NEW");
		}
		else if(strcmp((char *)m_recvbuf, CHOOSE_SIM5218_PASS)==0)
		{
			strcpy(szPhoneMode, "SIM5218");
		}
		
		if( strlen(szPhoneMode)>0 )
		{
			tag2DDvrCfg objMvrCfg;
			GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));
			strcpy(objMvrCfg.IpPortPara.m_szPhoneMod, szPhoneMode);
			
			if( SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg))==0 )
			{
				g_objDiaodu.show_diaodu("变更手机模块成功,设备重启后生效");

			}
		}

		memset(m_recvbuf, 0 ,SIZE_RECVBUF);
	}
}

void CHstApp::InputPasswordToProject()
{
	memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_PASSWORD | EDT_DIGITAL;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 10;
    EditorEntry.LabelPtr = "请输密码:    ";
    EditorEntry.AckProc  = ACK_InputPasswordProject;
    g_hst.CreateEditor(&EditorEntry);
}

void CHstApp::UpdateApp()
{
	pthread_t  pthreadAppUpdate;

	if( access("/mnt/UDisk/part1/exe.bin", F_OK) == 0 )
		pthread_create(&pthreadAppUpdate, NULL, G_ExeUpdateThread, NULL);
	else
		pthread_create(&pthreadAppUpdate, NULL, G_AppUpdateThread, NULL);
}

void CHstApp::FormatSDisk()
{
	char by = 0x06;
	DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);
}

void CHstApp::ResumeSet()
{	
	// 通知QianExe
	BYTE buf[2];
	buf[0] = 0xee;
	buf[1] = 0x0d;
	DataPush(buf, 2, DEV_DIAODU, DEV_QIAN, LV2);

	ProjectPrompt.Mode        = 0;
   	ProjectPrompt.LiveTime    = 3;	
  	ProjectPrompt.ExitProc    = 0;
   	ProjectPrompt.ExitKeyProc = 0;

	if(0==ResumeCfg(false,false)) 
	{	
		//通知DvrExe
		char by = 0x0b;
		DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);		
							
		//恢复成功
		char buf_resumeset0[] = "恢复出厂设置成功!";
		ProjectPrompt.TextPtr     = (INT8U *)buf_resumeset0;
   		ProjectPrompt.TextLen     = strlen(buf_resumeset0);
		g_hst.ActivatePrompt(&ProjectPrompt);
	} 
	else
	{	//恢复失败
		char buf_resumeset1[] = "恢复出厂设置失败!";
		ProjectPrompt.TextPtr     = (INT8U *)buf_resumeset1;
   		ProjectPrompt.TextLen     = strlen(buf_resumeset1);
		g_hst.ActivatePrompt(&ProjectPrompt);
	}
}

void CHstApp::UpdateSys()
{
	pthread_t pthreadSysUpdate;
	if( access("/mnt/UDisk/part1/image.bin", F_OK) == 0 )
		pthread_create(&pthreadSysUpdate, NULL, G_ImageUpdateThread, NULL);
	else
		pthread_create(&pthreadSysUpdate, NULL, G_SysUpdateThread, NULL);
}

void CHstApp::TestOilCut()
{	
	IOSet(IOS_OILELECCTL, IO_OILELECCTL_CUT, NULL, 0);

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	IOSet(IOS_YUYIN, IO_YUYIN_ON, NULL, 0);
#endif

	sleep(1);
	IOSet(IOS_OILELECCTL, IO_OILELECCTL_RESUME, NULL, 0);

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	IOSet(IOS_YUYIN, IO_YUYIN_OFF, NULL, 0);
#endif

	char buf_cutoil[] = "断油断电 测试完毕!";
	ProjectPrompt.Mode        = 0;
   	ProjectPrompt.LiveTime    = 1;	
  	ProjectPrompt.ExitProc    = 0;
   	ProjectPrompt.ExitKeyProc = 0;
	ProjectPrompt.TextPtr     = (INT8U *)buf_cutoil;
   	ProjectPrompt.TextLen     = strlen(buf_cutoil);
	g_hst.ActivatePrompt(&ProjectPrompt);
}

void CHstApp::InputVideo()
{
	//pthread_t thDataOutPut;
	//pthread_create(&thDataOutPut, NULL, G_VideoOutPutThread, NULL);

	char szBuf[2] = {0x8e, 0x10};
	DataPush(szBuf, 2, DEV_DIAODU, DEV_QIAN, LV2);	
}

void CHstApp::OutputUData()
{
	pthread_t thDataOutPut;
	pthread_create(&thDataOutPut, NULL, G_DataOutPutThread, NULL);
}

void CHstApp::PCSoundTest()
{
	char by = 0x04;
	DataPush(&by, 1, DEV_DIAODU, DEV_DVR, LV1);	
}

void CHstApp::TestCAM1()
{
	char buf[2] = {0x05, 0x00};
	DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV1);
}	

void CHstApp::TestCAM2()
{
	char buf[2] = {0x05, 0x01};
	DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV1);
}

void CHstApp::TestCAM3()
{
	char buf[2] = {0x05, 0x02};
	DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV1);
}

void CHstApp::TestCAM4()
{
	char buf[2] = {0x05, 0x03};
	DataPush(buf, 2, DEV_DIAODU, DEV_DVR, LV1);
}


void CHstApp::ChkSensorState()
{
	char frm[1024] = {0};
	int len = 0;
	
	GPSDATA gps(0);
	GetCurGps( &gps, sizeof(gps), GPS_LSTVALID );
	
	unsigned char uszResult;
	IOGet((byte)IOS_ACC, &uszResult );
	build_frm2((IO_ACC_ON == uszResult), "ACC", LANG_VALID, LANG_INVALID);
	
#if VEHICLE_TYPE == VEHICLE_M	
	//空重车/计价器
	IOGet((byte)IOS_JIJIA, &uszResult );
	build_frm2((IO_JIJIA_HEAVY == uszResult),	LANG_IO_METER, LANG_HEAVY, LANG_EMPTY);
	
	//前车门信号
	IOGet((byte)IOS_FDOOR, &uszResult );
	build_frm2((IO_FDOOR_OPEN == uszResult),	LANG_IO_FDOOR, LANG_VALID, LANG_INVALID);
	
	//后车门信号
	IOGet((byte)IOS_BDOOR, &uszResult );
	build_frm2((IO_BDOOR_OPEN == uszResult),	LANG_IO_BDOOR, LANG_VALID, LANG_INVALID);
	
	//载客信号1
	IOGet((byte)IOS_PASSENGER1, &uszResult );
	build_frm2((IO_PASSENGER1_VALID == uszResult),	LANG_IO_LOAD1, LANG_VALID, LANG_INVALID);
	
	//载客信号2
	IOGet((byte)IOS_PASSENGER2, &uszResult );
	build_frm2((IO_PASSENGER2_VALID == uszResult),	LANG_IO_LOAD2, LANG_VALID, LANG_INVALID);
	
	//载客信号3
	IOGet((byte)IOS_PASSENGER3, &uszResult );
	build_frm2((IO_PASSENGER3_VALID == uszResult),	LANG_IO_LOAD3, LANG_VALID, LANG_INVALID);
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8
	//空重车/计价器
	IOGet((byte)IOS_JIJIA, &uszResult );
	build_frm2((IO_JIJIA_HEAVY == uszResult),	LANG_IO_METER, LANG_HEAVY, LANG_EMPTY);
	
	//前车门信号
	IOGet((byte)IOS_FDOOR, &uszResult );
	build_frm2((IO_FDOOR_OPEN == uszResult),	LANG_IO_FDOOR, LANG_VALID, LANG_INVALID);
	
	// 欠压信号
	IOGet((byte)IOS_QIANYA, &uszResult );
	build_frm2((IO_QIANYA_VALID == uszResult), LANG_IO_QIANYA, LANG_VALID, LANG_INVALID);
	
	// 电子喇叭信号
	IOGet((byte)IOS_ELECSPEAKER, &uszResult );
	build_frm2((IO_ELECSPEAKER_VALID == uszResult),	LANG_IO_ELECSPEAKER, LANG_VALID, LANG_INVALID);
	
	// 电源输入检测信号
	IOGet((byte)IOS_POWDETM, &uszResult );
	build_frm2((IO_POWDETM_VALID == uszResult),	LANG_IO_POWDETM, LANG_VALID, LANG_INVALID);
	
	// 车灯输入信号
	IOGet((byte)IOS_CHEDENG, &uszResult );
	build_frm2((IO_CHEDENG_VALID == uszResult),	LANG_IO_CHEDENG, LANG_VALID, LANG_INVALID);
	
	// 振铃信号
	IOGet((byte)IOS_ZHENLING, &uszResult );
	build_frm2((IO_ZHENLING_VALID == uszResult), LANG_IO_ZHENLING, LANG_VALID, LANG_INVALID);
#endif
	
#if VEHICLE_TYPE == VEHICLE_M2
	// 欠压信号
	IOGet((byte)IOS_QIANYA, &uszResult );
	build_frm2((IO_QIANYA_VALID == uszResult), LANG_IO_QIANYA, LANG_VALID, LANG_INVALID);
	
	// 前门报警信号
	IOGet((byte)IOS_FDOOR_ALERT, &uszResult );
	build_frm2((IO_FDOORALERT_VALID == uszResult),	LANG_IO_FDOORALERT, LANG_VALID, LANG_INVALID);
	
	//前车门信号
	IOGet((byte)IOS_FDOOR, &uszResult );
	build_frm2((IO_FDOOR_OPEN == uszResult),	LANG_IO_FDOOR, LANG_VALID, LANG_INVALID);
	
	// 后门报警信号
	IOGet((byte)IOS_BDOOR_ALERT, &uszResult );
	build_frm2((IO_BDOORALERT_VALID == uszResult),	LANG_IO_BDOORALERT, LANG_VALID, LANG_INVALID);
	
	//后车门信号
	IOGet((byte)IOS_BDOOR, &uszResult );
	build_frm2((IO_BDOOR_OPEN == uszResult),	LANG_IO_BDOOR, LANG_VALID, LANG_INVALID);
	
	// 电源输入检测信号
	IOGet((byte)IOS_POWDETM, &uszResult );
	build_frm2((IO_POWDETM_VALID == uszResult),	LANG_IO_POWDETM, LANG_VALID, LANG_INVALID);
	
	// 脚刹信号
	IOGet((byte)IOS_JIAOSHA, &uszResult );
	build_frm2((IO_JIAOSHA_VALID == uszResult),	LANG_IO_JIAOSHA, LANG_VALID, LANG_INVALID);
	
	// 车灯输入信号
	IOGet((byte)IOS_CHEDENG, &uszResult );
	build_frm2((IO_CHEDENG_VALID == uszResult),	LANG_IO_CHEDENG, LANG_VALID, LANG_INVALID);
	
	// 振铃信号
	IOGet((byte)IOS_ZHENLING, &uszResult );
	build_frm2((IO_ZHENLING_VALID == uszResult), LANG_IO_ZHENLING, LANG_VALID, LANG_INVALID);
#endif							
	
	bool meter_link = ( int(GetTickCount()-g_last_heartbeat) <20*1000 ) ? true : false ;
	build_frm2(meter_link, LANG_METER_CONNECTION, LANG_CONNECT,	LANG_DISCONNECT);
	
	
	//电源电平检测，不支持
	
	//报警
	IOGet((byte)IOS_ALARM, &uszResult );
	build_frm2(IO_ALARM_OFF == uszResult, LANG_IO_FOOTALERM, LANG_NORMAL, LANG_ALERM);
	
	//欠压信号，暂不支持

	char str[100] = {0};
#if USE_OIL == 3
	sprintf(str, "%s AD:%u,", LANG_IO_OIL, g_usOilAD);
#endif
	strcpy(frm+len, str); 
	len += strlen(str);

	DWORD speed = 0;
	DWORD tick;
	//tick = G_GetMeterTick(&speed);（需补充）
	sprintf(str, "%s:%u,%s:%u", LANG_IO_PULSE, tick, LANG_IO_SPEED, speed);
	strcpy(frm+len, str); 
	len += strlen(str);
 	
	g_hst.ActivateStringPrompt_TIME(frm, 30);									
}

void CHstApp::TestAlarm()
{
	char buf[2];
	buf[0] = 0x41;
	buf[1] = 0x01;
	DataPush(buf, 2, DEV_DIAODU, DEV_QIAN, LV2);
}

void CHstApp::ChkLED()
{
// 	INT8U index;
// 	Frm46 frm;
// 	index = g_hst.GetMenuWinRecordNum();
// 	if (index == 0)			//LED校时
// 	{
// 		frm.chktype = 0x01;
// 	}
// 	else if (index == 1)	//发送即时信息
// 	{
// 		frm.chktype = 0x02;
// 	}
// 	else if (index == 2)	//LED亮屏
// 	{
// 		frm.chktype = 0x03;
// 	}
// 	else if (index == 3)	//LED黑屏
// 	{
// 		frm.chktype = 0x04;
// 	}
// 
// 	PushDataToQian(SRC_DIAODU, 1, 2, (char*)&frm);
}

void CHstApp::ChkGPS()
{
	char buf[50] = {0};
	unsigned char ucAccSta;
	IOGet(IOS_ACC, &ucAccSta);

	if(ucAccSta==IO_ACC_ON) 
	{	
		m_gpsdect = 1;
#if CHK_VER == 0
		memcpy(buf, "GPS实时检测已打开! 30秒后自动关闭!", 34);
		g_hst.RequestRing(0x04, 1);		//响铃提示
#endif						
	} 
	else {
		memcpy(buf, "ACC无效!请将ACC置为有效!", 24);
	}

	g_hst.ActivateStringPrompt_TIME(buf, 6); 
}

void CHstApp::ACK_Settemiver()
{
	INT8U verlen;
	verlen = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (verlen > 0)
	{
        g_hst.ActivateStringPrompt("设置成功!");

		Set_CGV_PARA(TYPE_TERMIVER, verlen, m_recvbuf);	
    }
	else 
	{
		g_hst.ActivateStringPrompt("设置错误!");
	}
	memset(m_recvbuf, 0 ,SIZE_RECVBUF);
}
void CHstApp::Settemiver()
{
#if USE_LIAONING_SANQI == 1
	tag2QcgvCfg cfg_cgv;
	GetSecCfg( &cfg_cgv, sizeof(cfg_cgv), offsetof(tagSecondCfg, m_uni2QcgvCfg.m_obj2QcgvCfg), sizeof(cfg_cgv) );

	memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_DIGITAL | EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 11; 
    EditorEntry.LabelPtr = "终端版本:   ";
    EditorEntry.AckProc  = ACK_Settemiver;
	EditorEntry.InitPtr = (INT8U *)cfg_cgv.m_temiver;	

    EditorEntry.InitLen = cfg_cgv.m_termiverlen;
    g_hst.CreateEditor(&EditorEntry);
#endif
}

void CHstApp::ACK_Setcompanyid()
{
	INT8U verlen;
	verlen = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (verlen	> 0)
	{
        g_hst.ActivateStringPrompt("设置成功!");

		Set_CGV_PARA(TYPE_COMPANYID, verlen, m_recvbuf);	
    }
	else 
	{
		g_hst.ActivateStringPrompt("设置错误!");
	}
	memset(m_recvbuf, 0 ,SIZE_RECVBUF);
}
void CHstApp::Setcompanyid()
{
#if USE_LIAONING_SANQI == 1
	tag2QcgvCfg cfg_cgv;
	GetSecCfg( &cfg_cgv, sizeof(cfg_cgv), offsetof(tagSecondCfg, m_uni2QcgvCfg.m_obj2QcgvCfg), sizeof(cfg_cgv) );

	memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 10; 
    EditorEntry.LabelPtr = "厂商ID:     ";
    EditorEntry.AckProc  = ACK_Setcompanyid;
	EditorEntry.InitPtr = (INT8U *)cfg_cgv.m_companyid;	

    EditorEntry.InitLen = cfg_cgv.m_companyidlen;
    g_hst.CreateEditor(&EditorEntry);
#endif
}

void CHstApp::ACK_Setgroupid()
{
	INT8U verlen;
	verlen = g_hst.GetEditorData(m_recvbuf);
    g_hst.ExitEditor();

    if (verlen	> 0)
	{
        g_hst.ActivateStringPrompt("设置成功!");

		Set_CGV_PARA(TYPE_GROUPID, verlen, m_recvbuf);	
    }
	else 
	{
		g_hst.ActivateStringPrompt("设置错误!");
	}
	memset(m_recvbuf, 0 ,SIZE_RECVBUF);
}
void CHstApp::Setgroupid()
{
#if USE_LIAONING_SANQI == 1
	tag2QcgvCfg cfg_cgv;
	GetSecCfg( &cfg_cgv, sizeof(cfg_cgv), offsetof(tagSecondCfg, m_uni2QcgvCfg.m_obj2QcgvCfg), sizeof(cfg_cgv) );

	memset(&EditorEntry, 0, sizeof(EditorEntry));
    EditorEntry.Mode     = EDT_CURSOR;
    EditorEntry.MinLen   = 0;
    EditorEntry.MaxLen   = 20; 
    EditorEntry.LabelPtr = "集团计费号: ";
    EditorEntry.AckProc  = ACK_Setgroupid;
	EditorEntry.InitPtr = (INT8U *)cfg_cgv.m_groupid;	

    EditorEntry.InitLen = cfg_cgv.m_groupidlen;
    g_hst.CreateEditor(&EditorEntry);
#endif
}

void CHstApp::Set_CGV_PARA(INT8U type, INT8U len, INT8U *ptr)
{
#if USE_LIAONING_SANQI == 1
	tag2QcgvCfg cfg_cgv;
	GetSecCfg( &cfg_cgv, sizeof(cfg_cgv), offsetof(tagSecondCfg, m_uni2QcgvCfg.m_obj2QcgvCfg), sizeof(cfg_cgv) );
	tag1PComuCfg obj1PComuCfg;
	GetImpCfg( (void*)&obj1PComuCfg, sizeof(obj1PComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
	char str[30] = {0};
	memcpy(str, (char*)ptr, len);
	switch(type)
	{
	case	TYPE_TERMIVER:
		{
			memcpy(cfg_cgv.m_temiver, str, len);
			cfg_cgv.m_termiverlen = len;
		}
		break;
	case 	TYPE_COMPANYID:
		{
			memcpy(cfg_cgv.m_companyid, str, len);
			cfg_cgv.m_companyidlen = len;
			obj1PComuCfg.m_bRegsiter = false;
		}
		break;
	case 	TYPE_GROUPID:
		{
			memcpy(cfg_cgv.m_groupid, str, len);
			cfg_cgv.m_groupidlen = len;
			obj1PComuCfg.m_bRegsiter = false;
		}
		break;
	default:
		break;
	}

	SetSecCfg( &cfg_cgv, offsetof(tagSecondCfg, m_uni2QcgvCfg.m_obj2QcgvCfg), sizeof(cfg_cgv) );
	SetImpCfg( &obj1PComuCfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
	char szbuf[2];
 	szbuf[0] = 0x01;	// 0x01 表示非中心协议帧
 	szbuf[1] = 0x01;	// 0x01 表示TCP套接字连接成功
 	DataPush((void*)szbuf, 2, DEV_SOCK, DEV_QIAN, LV3);	

#endif
}

void CHstApp::Loginliu()
{
	char buf[2] = {0x01, 0x03};	
 	DataPush(buf, 2, DEV_DIAODU, DEV_UPDATE, 2);
}
/**************AuxFunc**********************************************************/
INT8U CHstApp::HexToChar(INT8U sbyte)
{
    sbyte &= 0x0F;
    if (sbyte < 0x0A) return (sbyte + '0');
    else return (sbyte - 0x0A + 'A');
}

/* SAMPLE: 7899 -----> '7','8','9','9' */
INT8U CHstApp::DecToAscii(INT8U *dptr, INT16U data, INT8U reflen)
{
    INT8U i, len, temp;
    INT8U *tempptr;

    len     = 0;
    tempptr = dptr;
    for (;;) {
        *dptr++ = HexToChar(data % 10);
        len++;
        if (data < 10) break;
        else data /= 10;
    }
    if (len != 0) {
        dptr = tempptr;
        for (i = 0; i < (len/2); i++) {
            temp = *(dptr + i);
            *(dptr + i) = *(dptr + (len - 1 - i));
            *(dptr + (len - 1 - i)) = temp;
        }
        if (reflen > len) {
            dptr = tempptr + (len - 1);
            for (i = 1; i <= reflen; i++) {
                if (i <= len) *(dptr + (reflen - len)) = *dptr;
                else *(dptr + (reflen - len)) = '0';
                dptr--;
            }
            len = reflen;
        }
    }
    return len;
}

INT8U CHstApp::FormatYear(INT8U *ptr, INT8U year, INT8U month, INT8U day)
{
    if (year > 99) year = 99;
    if (month > 12 || month == 0) month = 12;
    if (day > 31 || day == 0) day = 1;
//    *ptr++ = '2';
//    *ptr++ = '0';
//    DecToAscii(ptr, year, 2);
//    ptr   += 2;
//    *ptr++ = '.';
//    DecToAscii(ptr, month, 2);
//    ptr   += 2;
//    *ptr++ = '.';
//    DecToAscii(ptr, day, 2);
	sprintf((char *)ptr, "%04d.%02d.%02d", year, month, day);
    return (2 + 2 + 1 + 2 + 1 + 2);
}
INT8U CHstApp::FormatTime(INT8U *ptr, INT8U hour, INT8U minute, INT8U second)
{
    minute += second / 60;
    second %= 60;
    hour   += minute / 60;
    minute %= 60;
    hour   %= 24;

	sprintf((char *)ptr, "%02d:%02d:%02d", hour, minute, second);

//    DecToAscii(ptr, hour, 2);
//    ptr   += 2;
//    *ptr++ = ':';
//    DecToAscii(ptr, minute, 2);
//    ptr   += 2;
//    *ptr++ = ':';
//    DecToAscii(ptr, second, 2);
    return (2 + 1 + 2 + 1 + 2);
}

bool CHstApp::SearchGBCode(INT8U *ptr, INT16U len)
{
    for (; len > 0; len--)
	{
        if (*ptr++ >= 0x80)
			return TRUE;
    }

    return FALSE;
}

INT8U CHstApp::ASCIIToGsmCode(INT8U incode)
{
    INT8U i;
    
    if (incode == ' ') 
		return ' ';

    for (i = 0; i < sizeof(GSMCODE); i++)
	{
        if (incode == GSMCODE[i]) 
			return i;
    }

    return ' ';
}

INT16U CHstApp::TestGBLen(INT8U *ptr, INT16U len)
{
    INT16U rlen;
    
    if (!SearchGBCode(ptr, len))
		return len;

    rlen = 0;

    for (;;) 
	{
        if (len == 0)
			break;

        rlen += 2;
        len--;

        if (len == 0)
			break;

        if (*ptr++ >= 0x80)
		{
            ptr++;
            len--;
        }
    }
    return rlen;
}

int CHstApp::CheckNameExist(byte v_byLen, char *v_pName)
{//查询电话本里是否有相同名字的记录存在
//返回-1表示没有重名，否则返回重名记录的索引

	int ret = -1;  
	char buf[500];
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	FILE *fp = fopen(path, "rb+");
	if(fp==NULL)	return false;

	SAVEDATAINDEX_STRUCT indata;
	fread(&indata, sizeof(indata), 1, fp);

	for (int i=0; i<indata.num_TB; i++)
	{
		if (i==m_selrecord_tb)
		{
			continue;
		}

		fseek(fp, sizeof(indata)+i*30, SEEK_SET); 
		fread(buf+i*10, 10, 1, fp);	//从文件中读出电话本中的姓名存入buf中

		if (strncmp(buf+i*10, (char *)v_pName, v_byLen) == 0 && strlen(buf+i*10)==v_byLen)//cyh add:mod
		{
			ret = i;
			break;
		}
	}	

	fclose(fp);
	return ret;	
}


void CHstApp::DelTBRecByName(char *v_pName)
{//cyh add: 将电话本中 名字 与 指定名字相符合的记录删除
	char path[100] = {0};
	sprintf(path, "%s/HandsetData.dat", HANDLE_FILE_PATH);
	
	SAVEDATAINDEX_STRUCT indata;

	byte flagTB[MAX_TBREC] = {0};
	byte byIndex = 0;
	int iCnt = 0;
	size_t uiSize = 0; 	
	int i = 0;
	char aryTelBook[30*MAX_TBREC] = {0};

	FILE *fp = fopen(path, "rb+");		
	if (NULL==fp)
	{
		PRTMSG(MSG_ERR, "Open HandSet file err\n");
		return;
	}

	fread(&indata, sizeof(indata), 1, fp);

	char buf[30] = {0};
	byte byLen = strlen(v_pName);

	for (i = 0;i<MAX_TBREC;i++)
	{
		fread(buf,1,10,fp);

		if (strncmp(buf,v_pName,byLen)==0 && strlen(buf)==byLen)
		{
			indata.flag_TB[i] = 0;  //后面再统一删除
		}

		fseek(fp,20,SEEK_CUR);//caituon:如果将来电话本的结构体发生变化了，将会出错。没办法啊
	}
	
	iCnt = 0;
	for (i = 0;i<MAX_TBREC;i++)
	{
		if (indata.flag_TB[i]!=0)
		{
			flagTB[iCnt++] = i;  //记录有效电话本的索引
		}
	}

	memset( indata.flag_TB,0x00,sizeof(indata.flag_TB) );  //清除所有电话本有效的标记
	indata.num_TB = iCnt;

	for (i = 0;i<iCnt;i++)
	{
		byIndex = flagTB[i];  //依次取出有效的索引号，移动数据
		indata.flag_TB[i] = 1;		
		fseek(fp, sizeof(indata)+30*byIndex, SEEK_SET);
		fread(aryTelBook+30*i,30,1,fp);
	}

	fseek(fp, sizeof(indata), SEEK_SET);//移动到保存电话本处
	fwrite(aryTelBook,30,iCnt,fp);

	fseek(fp, 0, SEEK_SET);  //移到文件头
	fwrite(&indata,sizeof(indata),1,fp);
	fclose(fp);
	fp=NULL;

	return;
}




