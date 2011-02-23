#ifndef _YX_AUTORPTSTATION_H_
#define _YX_AUTORPTSTATION_H_

#define PACKED_8 __attribute__( (packed, aligned(8)) )
#define _STRUCT_8 struct PACKED_8

#define PACKED_2 __attribute__( (packed, aligned(2)) )
#define _STRUCT_2 struct PACKED_2

struct tagReportStateNew
{
	bool	m_bPlaySound;		// 是否要进行语音播报
	int  	m_iPlayNo;			// 进行语音播报的站点编
	
	tagReportStateNew() 
	{
		memset(this, 0, sizeof(*this)); 
	}
};

struct tagOneStationNew	// 站点参数 
{
	int		m_iCurNo;			// 本站点编号
	int     m_iUniId;			// 站点统一编号
	bool	m_bIsPlayed;		// 本站是否已经报过
	long	m_lLat;				// 站点纬度
	long	m_lLon;				// 站点经度
	int		m_iRad;				// 本站点的半径
	float	m_fdirect;			// 本站的GPS方向
	BYTE    m_bytStaAttr;		// 站点属性: 0x01：始发站（主站） 0x02：终点站 （副站） 0x03：中途站 0x04：拐弯
	BYTE            m_bytSpeed;        
	unsigned short  m_usZhiTm;
	unsigned short  m_usPaoTm;	
	BYTE 	m_bytrecordfile;	// 所要播放的车内录音文件的个数
	char	m_szrecord[5][11];	// 所要播放的车内录音文件的名称
	
	char	m_szPlay[200];		// 站点播报内容	
	char	m_szName[30];		// 站名 (不足填0,且最后1个字节必须为0)
};

struct tagStationsNew
{
	tagOneStationNew m_aryStation[400];		// 所有站点
	int m_iStaTotal;						// 站点总数 (包含上下行总数)
	int  m_iLineNo;		     				// 线路编号,与后面的编号名无直接关系
	BYTE m_bytLineEnb;						// 线路是否使能：0--不使能，1：使能
	char m_szLineNoName[100];				// 线路名,例如"厦大 29 软件园生产基地"等 (不足填0,且最后1个字节必须为0)
	
	tagStationsNew() {	memset(this, 0, sizeof(*this));	}
};

struct tagLineHeadNew
{
	WORD	m_wLineTotal; // 线路总数
	WORD	m_wLineSelNo; // 启用线路编号
	char ver[16];
	tagLineHeadNew() {	memset(this, 0, sizeof(*this));	}
};

class CAutoRptStationNew
{
public:
	CAutoRptStationNew();
	~CAutoRptStationNew();

	int init();
	int Release();	

	void P_ThreadAutoRptStation();	
	void P_TmSetCheCi();

	bool LoadStations( tagStationsNew &v_objStations, bool v_bReBegin, int v_bytSelNo );
	
	void SendInOutData(int v_iSta, byte v_bytSym);
	void StopPlayCurSta();
	
	int DealComu8e( char* v_szBuf, DWORD v_dwBufLen );
	int DecompressLineFile(char *filename);
	bool ShowAvTxt( char* v_szUp,char *v_szDown);
	void Show_diaodu(char *v_szbuf);
	ushort GetLineId();
	//BOOL DeleteDirectory(const CString szDir);
	BYTE GetCheCi();
	byte GetCurLimitSpd();
	bool ShowToAV( DWORD PlayNo );
	bool CenterLoadSta(ushort v_usLine);
	BYTE GetUpDownSta2();
	void SetWorkSta();
	
	int GetUniStaId(int v_iCurId);
	byte GetAutoRptSta();
	void SetAutoRptSta(byte m_bytSet);
	//中心启动某条线路
	void hdl_4216(byte* buf, int len);
	//中心启动报站功能，让车台是否开启主动报站功能
	void hdl_4217(byte* buf, int len);
	
	//应答结果(1) + 序号（1）+类型（1） ＋ [ 非营运类型（1）]
	void hdl_4220(byte* buf, int len);
	//中心设置一键拨打多个电话的号码。
	void hdl_4219(byte* buf, int len);

	int  Snd4254( const tagGPSData &v_objGps );
	
	//中心查询车台线路情况
	void hdl_4218();
	void ShowLineVer();
	void ShowLineAV();
	
private:
	byte m_ucStaCnt;
	byte  m_bytEnbRptSta;		//营运-非营运状态
	byte  m_bytCheCi;			//车次类型
	byte  m_bytCheCiTmp;		//车次类型
	DWORD m_dwInStaTm;			// 进站时间
	DWORD m_dwOutStaTm;			// 出站时间
	WORD  m_usCurZhiTm;
	WORD  m_usCurPaoTm;
	byte  m_ucLstUpDown;
	byte  m_ucCurLimitSpd;

	CInnerDataMng m_objRcvDataMng;
	
	tagStationsNew m_objStations;
	tagReportStateNew m_objRptSta;		//当前报站状态
	tagLineHeadNew m_objLinesHead;
	
	bool CalculateDis(double latitude,	double longitude, double *dwStaDis, int curNo, int bytareaNo);
	// 	void Playsound();//add by kzy 20091019
	
	byte GetUpDownSta(int v_iCurId);
	
	//获取进出站的状态, 0: 进站， 1：出站
	bool GetInOutSta(int v_iCurId);
	
	byte GetStaLimitSpd(int v_iCurId);

	//获取站点的属性:始发站/终点站/中间站/拐弯/出库/入库
	byte GetStaAttr(int v_iCurId);
};

#endif

