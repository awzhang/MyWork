#ifndef _INC_CHANDSET_H
#define	_INC_CHANDSET_H

#define SIZE_KEY                                6
#define SIZE_INIT                               8
#define SIZE_PYINDEX                            100

#define MAX_SEL                                 5

#define ARROW_LEFT                              0x81
#define ARROW_RIGHT                             0x82

#define SEL_PINYIN                              0
#define SEL_GBCODE                              1
/*****************DEFINE CONFIG PARAMETERS(editor)***********************/
#define MAX_LINE                            50
#define SIZE_BUF                            256

#define EDT_REVERSE                         0x01	//反显编辑模式
#define EDT_PASSWORD                        0x02	//密码编辑模式
#define EDT_DIGITAL                         0x04	//数字编辑模式
#define EDT_LOCKUPPER                       0x08
#define EDT_CHINESE                         0x10
#define EDT_CURSOR                          0x40	//光标显示编辑模式
#define EDT_PULS                            0x80	

/********************DEFINE Extend Keyboard Mode(ExtKB)*******************/
#define EXTKB_CHINESE                       0
#define EXTKB_ASCII							1

/********************DEFINE FMENUWIN PARA(FMenuWin)***********************/
#define MAX_RECORD                          2
#define SIZE_RECORD                         256

/********************DEFINE ITEMMENU ATTRIB(ItemMenu)*********************/
#define ITEM_LMENU                          0x01
#define ITEM_FMENU                          0x02
#define ITEM_FCN                            0x03

/**************Input**********************************************************/
#define INPUT_CHINESE                       0
#define INPUT_UPPER                         1
#define INPUT_LOWER                         2
#define INPUT_DIGITAL                       3

/********************DEFINE KEYBOARD(keyboard)********************************/
#define ATTR_REHIT                          0x80
#define ATTR_FCN                            0x01
#define ATTR_CHAR                           0x02
#define ATTR_CONVERT                        0x04	

/*
********************************************************************************
*                  DEFINE FUNCTION KEY
********************************************************************************
*/
#define KEY_ACK                             0x80
#define KEY_NAK                             0x81
#define KEY_SND                             KEY_ACK
#define KEY_END                             KEY_NAK
#define KEY_CLR                             KEY_NAK
#define KEY_UP                              0x82
#define KEY_DOWN                            0x83
#define PTT_ON                              0x84
#define PTT_OFF                             0x85
#define PTT_HOLD                            0x86
#define KEY_PWR_ON                          0x87
#define KEY_PWR_OFF                         0x88
#define KEY_FCN                             0x89
#define KEY_SHORTCUT                        0x8a
#define KEY_NOOPERATE                       0x8b
#define KEY_LOOSEN                          0x8c
#define KEY_INEXIST                         0x8d
#define KEY_TOGGLECASE                      0x8e
#define KEY_CLRALL                          0x8f
#define KEY_MEDICAL                         0x91
#define KEY_TROUBLE                         0x92
#define KEY_INFORM                          0x93
#define KEY_VOICE                           0x94
#define KEY_NUMTOGGLE                       0x95
#define KEY_PAGEUP                          0x96
#define KEY_PAGEDOWN                        0x97
#define KEY_SPACE                           0x98
#define KEY_NULL                            0xff

#define KEY_STAR_                           'a'
#define KEY_JING_                           'b'
#define KEY_ACK_                            'c'
#define KEY_NAK_                            'd'
#define KEY_SND_                            'e'
#define KEY_UP_                             'f'
#define KEY_DOWN_                           'g'
#define PTT_ON_                             'h'
#define PTT_OFF_                            'i'
#define PTT_HOLD_                           'j'
#define KEY_NOOPERATE_                      'k'                 /* VIRTUAL KEY */
#define KEY_END_                            'l'
#define KEY_PWR_ON_                         'm'
#define KEY_SHORTCUT_                       'n'
#define KEY_PROMPT_                         'o'                 /* VIRTUAL KEY */
#define KEY_REHIT_                          'p'
#define KEY_PWR_OFF_                        'q'
#define KEY_FCN_                            'r'
#define KEY_LOOSEN_                         's'
#define KEY_SET_                            't'
#define KEY_MEDICAL_                        'u'
#define KEY_TROUBLE_                        'v'
#define KEY_INFORM_                         'w'
#define KEY_VOICE_                          'x'

/********************DEFINE PROMPT MODE(Prompt)***************************/
#define PROMPT_NOKEY                        0x01
#define PROMPT_ACKINFORM                    0x02
#define PROMPT_NOT_RETURN                   0x04  //cyh add:这种模式的提示框不会自动退出，除非按下ACK或者Cancel键

/********************DEFINE CONFIG PARAMETERS(Screen)*********************/
#define NUMLINE                             2
#define LINESIZE                            12
#define SCREENSIZE                          (NUMLINE * LINESIZE)

/********************DEFINE CONFIG PARAMETERS(TextDply)******************/
#define MAXLINE                             300

/********************DEFINE WINDOW ATTRIB(Window)************************/
#define WIN_TEXT                            0x01
#define WIN_LMENU                           0x02
#define WIN_FMENU                           0x03

/********************DEFINE CONFIG PARAMETERS(WinStack)******************/
#define MAX_STACK                           20
#define SIZE_ENV                            200

extern ulong	g_hsttm;

class CHandset
{
public: 
	CHandset();
	virtual ~CHandset();

	int Init();
	int Release();

	void P_ThreadHandAnalyse();

private:
	typedef struct {
		char   signal;
		char   talk;
		char   sm;
		char   arrow;
		char   voltage;
		char   key;
		char   music;
		char   clock;
		void Init()
		{
			memset(this, 0, sizeof(*this));
		}
	}Indicator;

	pthread_t	m_pthreadHandlWork;
	Indicator m_indicator;
	void InitMeun();

	static	WINSTACKENTRY_STRUCT WinStackEntry_Prompt;  //提示界面的窗口函数

public:
	CInnerDataMng m_objHandleReadMng;

	static	ITEMMENU_STRUCT ItemMenu_cur;  //cyh add:记录当前的菜单信息

	typedef struct editorbuf_st{
		INT8U   Input;				//cyh add 输入模式的索引 0:Chinese; 1:Upper 2:Lower 3:Digital  search InitRecordProc() for detail
		INT16U  EditPos;
		INT16U  CursorPos;
		INT16U  CurLen;				//cyh add 当前缓冲区保存的字符，包括标题
		INT8U   Buffer[SIZE_BUF];
	} EDITORBUF_STRUCT;				//用来存放编辑数据,包括标题数据

	static ITEMMENU_STRUCT	MENU_CHINESE;
	static ITEMMENU_STRUCT	MENU_UPPER;
	static ITEMMENU_STRUCT	MENU_LOWER;
	static ITEMMENU_STRUCT	MENU_DIGITAL;
	static ITEMMENU_STRUCT	MENU_SELINPUT_CHILD[4];
	static ITEMMENU_STRUCT	MENU_SELINPUT;

	static EDITOR_STRUCT    Editor;
	static EDITORBUF_STRUCT EditorBuf;
	static INT16U           LinePos[MAX_LINE], CurLine, PreLine;
	static INT16U           MaxLine;
	static KEY_STRUCT		m_curkey_editor;
	static WINSTACKENTRY_STRUCT WinStackEntry_Editor;  //cyh  编辑界面的窗口函数结构体

/***************WinStack**********************************************************/
	typedef struct {
		void    (*InitProc)(void);
		void    (*DestroyProc)(void);
		void    (*SuspendProc)(void);
		void    (*ResumeProc)(void);
		void    (*HdlKeyProc)(void);
		INT8U   enviroment[SIZE_ENV];  //cyh add:保留当前窗口的相关数据，这样可以返回到原来的现实环境
		//cyh add:参见函数SuspendProc_MenuWin, environment保存的数据应该是: 2B+ItemMenu(菜单内容)+2B+MenuWin(功能函数)+2B+1B(待保存的菜单属性)+FMENUWIN_STRUCT
	} STACKITEM_STRUCT;

//	typedef struct {
//		void    (*InitProc)(void);
//		void    (*DestroyProc)(void);
//		void    (*SuspendProc)(void);
//		void    (*ResumeProc)(void);
//		void    (*HdlKeyProc)(void);
//	} WINSTACKENTRY_STRUCT;

	static STACKITEM_STRUCT winstack[MAX_STACK];
	static INT8S			curstack;
	
/***************ExtKB**********************************************************/
	typedef struct extkb{
		INT8U       Mode;
		INT8U       SelPos;
		void        (*Informer)(INT16U ch);
		extkb()		{	memset(this, 0, sizeof(*this));		}
	} ExtKB;

	static ExtKB			m_extkb;
	static KEY_STRUCT		m_curkey_extkb;

/***************FMenuWin**********************************************************/
	typedef struct {
		INT16U  currecord;
		INT16U  maxrecord;
		INT16U  (*readproc)(INT16S num, INT8U *ptr);  //cyh add:num:待读取的索引号,ptr:将读取数据写到ptr地址
	} FMENUWIN_STRUCT;		//每屏显示多个子菜单

	typedef struct {
		INT16U  len;
		INT8U   data[SIZE_RECORD];
	} RECORD_STRUCT;

	static RECORD_STRUCT	record;
	static FMENUWIN_STRUCT	fmenuwin;

/***************Window**********************************************************/
	typedef struct {
		INT8U   attrib;
		INT8U  *textptr;
		INT16U  textlen;
		INT16U  maxrecord;
		INT16U  initrecord;
		INT16U (*readproc)(INT16S, INT8U *);  ////cyh add:读取当前菜单下的内容(标题?)，并返回内容所占的字节数. 第一个参数表示菜单的索引号，第二个参数表示保存内容的指针，返回 读取到的记录 所占的字节数
	} WINDOWENTRY_STRUCT;
	static INT8U			winattrib;


	static bool				numlocked, uppercase;			//???
	static INT8U			recvkey, prekey, hitnum;
//	static StackLst			key_stack;
	static KEY_STRUCT		m_curkey_kb;

/***************LMenuWin**********************************************************/
	typedef struct {
		INT16U  maxrecord;
		INT16U  startpos;
		INT16U  indicator;  //cyh add:?? 哪一行需要显示背景阴影
		INT16U  (*readproc)(INT16S num, INT8U *ptr);
	} LMENUWIN_STRUCT;		//每屏显示一个子菜单
	static LMENUWIN_STRUCT	lmenuwin;
	static RECORD_STRUCT	record_LMenu[MAX_RECORD];

/***************MenuWin**********************************************************/
//	typedef struct {
//		INT8U   WinAttrib;
//		void    (*Handler_KEY_ACK)(void);
//		void    (*Handler_KEY_NAK)(void);
//		void    (*Handler_KEY_FCN)(void);
//		void    (*Handler_KEY_SEL)(void);
//		INT16U  (*StoreProc)(INT8U *);
//		INT16U  (*MaxRecordProc)(void);
//		INT16U  (*InitRecordProc)(void);
//		INT16U  (*ReadRecordProc)(INT16S, INT8U *);
//	} MENUWIN_STRUCT;

	static MENUWIN_STRUCT		MenuWin_cur;  //cyh add: 记录当前菜单的功能处理函数等等
	static WINDOWENTRY_STRUCT	WinEntry;
	static KEY_STRUCT			m_curkey_menuwin;

/***************Prompt**********************************************************/
//	typedef struct {
//		INT8U   Mode;
//		INT8U   LiveTime;               /* UNIT: SECOND */
//		INT8U  *TextPtr;
//		INT16U  TextLen;
//		void (*ExitProc)(void);
//		void (*ExitKeyProc)(void);
//	} PROMPT_STRUCT;
	//即时信息
	static PROMPT_STRUCT		Prompt, PromptEntry;
	static WINDOWENTRY_STRUCT	WinEntry_Prompt;
	static KEY_STRUCT			m_curkey_prompt;

/***************Screen**********************************************************/
	typedef struct 
	{
		INT8U       l;  //cyh add 当前屏幕指针
		INT8U       cursor;  //cyh add :当前屏幕指针
		INT8U       data[SCREENSIZE];  //保存当前显示在屏幕上的数据
	} SCREEN_STRUCT;
	static SCREEN_STRUCT		screen;

/**************hxd 090518 输入法相关********************************************/
	typedef struct 
	{
		INT8U   len;
		INT8U   key[SIZE_KEY];
	} keyword_struct;

	static keyword_struct keyword;

	typedef	struct 
	{
		INT8U   len;
		INT8U   init[SIZE_INIT];
	} initchar_struct;

	static initchar_struct initchar; // 怎么初始化?
	static KEY_STRUCT		m_curkey_py;

//	typedef struct
//	{
//		char        pinyin[7];
//		const char *PY_mb;
//	}PY_INDEX;

	typedef struct 
	{
		INT8U   status;
		INT8U   tag0;                           /* for pinyin */
		INT8U   tag1;                           /* for gbcode */
		INT16U  indexlen;
		void    (*informer)(INT16U gbcode);
		const PY_INDEX *py_index[SIZE_PYINDEX];
	} SCB_STRUCT;

	static SCB_STRUCT SCB; 
	static void UpdateWin_SelChn(void);
	static void Handler_KEY_CHR(void);
	
	void P_TmHstConvert();

	static void memcpy_rom(INT8U  *dptr,char const  *sptr,INT8U len)
	{
		while (len-- > 0){
			*dptr++=*sptr++;
		}
	}
	//static INT16U strlen_rom(char const _generic *sptr)
	static INT16U strlen_rom(char const  *sptr)
	{
		INT16U len=0;
    
		while(*sptr++) len++;
		return len;
	}

	static BOOLEAN PushKey(INT8U ch)
	{
		if (keyword.len < sizeof(keyword.key)) {
			keyword.key[keyword.len++] = ch;
			return TRUE;
		} else {
			return FALSE;
		}
	}

	static BOOLEAN PopKey(void)
	{
		if (keyword.len > 0) {
			keyword.len--;
			return TRUE;
		} else {
			return FALSE;
		}
	}

	static void ExitSelChn(INT16U gbcode)
	{
		PopWinStack();
		if (SCB.informer != 0) SCB.informer(gbcode);
	}
	static WINSTACKENTRY_STRUCT WinStackEntry_SelChn; //选择汉字的窗体。
	static void ResumeProc_SelChn(void);
	static void SuspendProc_SelChn(void);
	static void DestroyProc_SelChn(void);
	static void HdlKeyProc_SelChn(void);
	static void InitProc_SelChn(void);
	static INT16U SerachChineseCode(INT8U indexlen, INT8U *index, const PY_INDEX **result);
	static void   ActivateSelChn(INT16U initlen, INT8U *initptr, INT8U initkey, void (*informer)(INT16U gbcode));

/***************TextDply**********************************************************/
	static INT8U  m_textptr[300];	
	static INT16U linepos[MAXLINE], maxline, curline, textlen;  //cyh add: maxline:显示当前内容所要占用的行数  curline:当前行,0第一行;  linepos:记录当前行从m_textprt的哪一个字节开始显示 ;textlen:待显示字符的字节数

/***************TextWin**********************************************************/
	typedef struct {
		INT8U   *textptr;
		INT16U  textlen;
	} TEXTWIN_STRUCT;	//文本窗口
	static TEXTWIN_STRUCT textwin;

//	static HANDLE			m_port;					//串口句柄
//	static DataFrame		HDFrame;

	static ulong comm_write(char *data, int len);

	bool check_crc(const byte *buf, const int len);
	byte get_crc(const byte *buf, const int len);

	static bool		SendHstFrame(char *cmd, char *data, int len);
	static bool		GetKeyMsg(KEY_STRUCT *key);

/***************editor*******************************************************/
	static void		ExitEditor(void);
	void			CreateEditor(EDITOR_STRUCT *entry);
	void			ResetEditor(EDITOR_STRUCT *entry);
	INT16U			GetEditorData(INT8U *ptr);
	static void		SelInputMode(void);
	static void		Sel_Chinese(void);
	static void		Sel_Upper(void);
	static void		Sel_Lower(void);
	static void		Sel_Digital(void);
	static INT16U	InitRecordProc(void);
	static bool		ActivateSelInputMode(void);
	static INT16U	GetMaxLen(void);
	static INT16U	GetMinLen(void);
	static INT16U	GetCurLen(void);
	static void		CheckEditorValid(void);
	static void		SetLinePara(void);
	static INT16U	GetInitCursorPos(void);
	static void		UpdateWin_Editor(bool CanShowCursor);
	static void		AddEditPos(void);
	static void		DecEditPos(INT8U DecStep);
	static void		MovUpEditPos(void);
	static void		MovDownEditPos(void);
	static bool		AddChar(INT16U ch);
	static bool		ModifyChar(INT8U ch);
	static void		InitProc_Editor(void);
	static void		DestroyProc_Editor(void);
	static void		SuspendProc_Editor(void);
	static void		ResumeProc_Editor(void);
	static void		HdlKeyProc_Editor(void);
	static void		Inform_SelChn(INT16U gbcode);
	static void		Inform_ExtKB(INT16U ch);
	static void		Handler_KEY_CHAR(void);
	static void		Handler_KEY_CONVERT(void);
	static void		Handler_KEY_REHIT(void);

/***************ExtKB**********************************************************/
	static void		ActivateExtKB(INT8U Mode, void (*Informer)(INT16U ch));
	static void		ExitExtKB(INT16U ch);
	static void		UpdateWin_ExtKB(void);
	static void		InitProc_ExtKB(void);
	static void		ResumeProc_ExtKB(void);
	static void		HdlKeyProc_ExtKB(void);

/***************FMenuWin*******************************************************/
	static void		ReadRecord(void);
	static void		CreateFMenuWin(INT16U initrecord, INT16U maxrecord, INT16U (*readproc)(INT16S num, INT8U *ptr));
	static INT16U	StoreFMenuWin(INT8U *storeptr);
	static void		ReasumeFMenuWin(INT8U *storeptr);
	static void		PageUpFMenuWin(void);
	static void		PageDownFMenuWin(void);
	static INT16S	GetFMenuCurRecord(void);

/***************HstCtl*********************************************************/
	void			TurnonHst(void);
	void			TurnoffHst(void);
	void			TurnonBKLight(void);
	void			TurnoffBKLight(void);
	static void		CancelRing(void);
	void			RequestRing(char data, int len);
	static void		CtlKeyPrompt(char data, int len);
	void			InitHstIndicator(void);
	void			TurnonSMIndicator(void);
	void			TurnoffSMIndicator(void);
	void			TurnonArrowIndicator(void);
	void			TurnoffArrowIndicator(void);
	void			CtlSignalIndicator(INT8U level);
	void			CtlVoltageIndicator(INT8U level);
	void			TurnonTalkIndicator(void);
	void			TurnoffTalkIndicator(void);
	void			CtlHstContrast(INT8U level);
	void			ChkHst(void);
	
/*******************ItemMenu***************************************************/
	static void		GetCurItemMenu(void);
	static INT16U	StoreProc_ItemMenu(INT8U *StorePtr);
	static INT16U	MaxRecordProc_ItemMenu(void);
	static INT16U	ReadRecordProc_ItemMenu(INT16S num, INT8U *ptr);
	static void		Handler_KEY_ACK_ItemMenu(void);
	static void		ActivateItemMenu(ITEMMENU_STRUCT *ItemMenuEntry);
	static void		ActivateItemMenu_Init(ITEMMENU_STRUCT *ItemMenuEntry, INT16U (*InitRecordProc)(void));	//??

/*******************KeyBoard***************************************************/
	void			handle_keyboard();
	static void		SendCurKeyMsg(void);
	static bool		GetDEFKey(INT8U inkey);
	static void		HdlRehitKey(void);
	static void		ParseKeyBoard(char v_szData);
	static bool		GetConvertKey(void);
	static void		ToggleCase(void);
	static void		ToggleUpperCase(void);
	static void		ToggleLowerCase(void);
	static void		UnlockNum(void);
	static void		LockNum(void);
	static void		ResetConvertKey(void);
	void			InitKeyBoard(void);

/*******************LMenuWin***************************************************/
	static void		ReadRecord_Up(void);
	static void		ReadRecord_Down(void);
	static void		ReadRecord_Init(void);
	static void		UpdateWin_LMenuWin(void);
	static void		CreateLMenuWin(INT16U initrecord, INT16U maxrecord, INT16U (*readproc)(INT16S num, INT8U *ptr));
	static INT16U	StoreLMenuWin(INT8U *storeptr);
	static void		ResumeLMenuWin(INT8U *storeptr);
	static void		PageUpLMenuWin(void);
	static void		PageDownLMenuWin(void);
	static INT16S	GetLMenuCurRecord(void);

/*******************MenuWin****************************************************/
	static void		CreateMenuWin(INT16U initrecord, INT16U maxrecord);
	static void		InitProc_MenuWin(void);
	static void		SuspendProc_MenuWin(void);
	static void		ResumeProc_MenuWin(void);
	static void		HdlKeyProc_MenuWin(void);
	static void		PopMenuWin(void);	//??
	void			RefreshMenuWin(void);
	static void		ActivateMenuWin(MENUWIN_STRUCT *MenuWinEntry);
	static INT8U	*GetMenuWinEnviroment(void);
	static INT16S	GetMenuWinRecordNum(void);

/*******************Prompt*******************************************************/
	static void		ExitPrompt(void);
	static void		InitProc_Prompt(void);
	static void		DestroyProc_Prompt(void);
	static void		SuspendProc_Prompt(void);
	static void		ResumeProc_Prompt(void);
	static void		HdlKeyProc_Prompt(void);
	static void		ActivatePrompt(PROMPT_STRUCT *PromptEntry);
	void			ActivateStringPrompt(char *textstr);
	static void		ActivateStringPrompt_TIME(char *textstr, INT8U delaytime);
	void			ActivateStringPrompt_TIME_KEY(char *textstr,INT8U delaytime,void(*KeyProc)(void));
	void			ActivateSelPrompt(void *textptr, INT16U textlen, void (*SelProc)(void));
	void			ActivateSelPrompt_TIME(void *textptr, INT16U textlen, void (*SelProc)(void), INT8U delaytime);

/*******************Screen*******************************************************/
	static INT8U	GetScreenSize(void);	//??
	static INT8U	GetScreenLineSize(void);
	static INT8U	GetScreenNumLine(void);	//??
	static void		VTL_ClearScreen(void);
	static void		VTL_ScreenPrint(INT8U ch);
	static void		VTL_ScreenPrintMem(INT8U *ptr, INT8U len);
	static void		VTL_ScreenPrintStr(char *str);
	static void		VTL_ScreenPrintChar(INT8U ch, INT8U len);
	static void		VTL_AbsMovCursor(INT8U pos);
	static void		VTL_RelMovCursor(INT8U pos);
	static void		OverWriteScreen(void);
	static void		ReverseBlock(INT8U startpos, INT8U endpos);
	void			ReverseChar(INT8U pos);
	static void		ReverseLine(INT8U line);
	static void		ShowCursor(INT8U pos);
	static void		HideCursor(void);
	static bool		CheckScreenValid(void);
	static void		FilterChar(INT8U ch);
	static void		ControlCursor(INT8U mode, INT8U pos);
	
/*******************TextDply*****************************************************/
	static void		UpdateDply(void);
	static void		InitTextDply(INT8U *ptr, INT16U len);
	static void		PageHomeText(void);
	static void		PageEndText(void);
	static bool		PageDownText(void);
	static bool		PageUpText(void);
	
/*******************TextWin*****************************************************/
	static void		CreateTextWin(INT8U *textptr, INT16U textlen);
	static INT16U	StoreTextWin(INT8U *storeptr);
	static void		ResumeTextWin(INT8U *storeptr);
	static void		PageUpTextWin(void);
	static void		PageDownTextWin(void);
	
/*******************Window*****************************************************/
	static void		CreateWin(WINDOWENTRY_STRUCT *entry);
	static INT16U	StoreWin(INT8U *storeptr);
	static void		ResumeWin(INT8U *storeptr);
	static void		PageUpWin(void);
	static void		PageDownWin(void);
	static INT16S	GetCurWinRecord(void);

/*******************WinStack**************************************************/
	void			InitWinStack(void);
	static void		PopWinStack(void);
	static void		PushWinStack(WINSTACKENTRY_STRUCT *WinStackEntry);
	void			DestroyWinStack(void);
	static void		ActivateLowestWinStack(void);
	void			WinStackProc(void);
	static INT8U   *GetWinStackEnviroment(void);
	static bool		ReserveLowestStack(void);
	INT8S			GetCurstack(void);

public:
	static	StackLst	keymsg_stack;
	/*****************************AuxFunc*******************************************/
	static bool		NeedRoundLine(INT8U *ptr, INT16U len);
	static INT16U	PlotLinePos(INT8U *textptr, INT16U textlen, INT16U *linepos, INT16U maxline, INT16U linesize);
	static INT16U	InsertEditBuf(INT8U *ptr, INT16U len, INT16U pos, INT16U ch);
	static INT8U	DelEditBuf(INT8U *ptr, INT16U len, INT16U pos);
	static void		WriteWordByPointer(INT8U *ptr, INT16U wdata);
	INT16U			SearchDigitalString(INT8U *ptr, INT16U maxlen, INT8U flagchar, INT8U numflag);
	INT8U			CharToHex(INT8U schar);
	static INT16U	ReadWordByPointer(INT8U *ptr);
};

#endif


