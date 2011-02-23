#ifndef _LN_REPORT_H_
#define _LN_REPORT_H_

class CLN_Report
{
public:
	CLN_Report();
	virtual ~CLN_Report();

public:
	int  Init();
	int  Release();

	int  SetAlert(bool v_bEnable, DWORD v_dwAlertType, DWORD SubPar);
	void SendOneAlertReport(short v_shAlermType, char *v_szRID, byte v_bytPriority);
	void P_TmAlertReport();

	int  BeginPositionReport(char *v_szId, char *v_szBeginTime, char v_szIntervalType, DWORD v_dwIntervalValue, char v_szEndType, DWORD v_dwMaxCnt, char *v_szEndTime);
	void P_TmAutoReport();
	void SendOnePositionReport(char v_szReportType);

	int  BeginSpeedCheck();
	int  EndSpeedCheck();
	void P_TmSpeedCheck();

	int  BeginAreaCheck();
	int  EndAreaCheck();
	void P_TmAreaCheck();
	bool AreaEnCheck(tag2QAreaRptCfg *objAreaRptCfg);

	int  BeginStartengCheck();
	int  EndStartengCheck();
	void P_TmStartengCheck();
	
	int BeginSpeedRptCheck();
	int EndSpeedRptCheck();
	void P_TmSpeedRptCheck();
	bool SpeedEnCheck(tag2QAreaSpdRptCfg *objAreaSpdRptCfg);

	int BeginParkTimeRptCheck();
	int EndParkTimeRptCheck();
	void P_TmParkTimeRptCheck();
	bool ParkTimeEnCheck(tag2QAreaParkTimeRptCfg *objAreaParkTimeRptCfg);

	int BeginOutLineRptCheck();
	int EndOutLineRptCheck();
	void P_TmOutLineRptCheck();
	bool OutLineEnCheck(tag2QLineRptCfg *objLineRptCfg);

	int BeginTireRptCheck();
	int EndTireRptCheck();
	void P_TmTireRptCheck();
	
	int BeginMileRptCheck();
	int EndMileRptCheck();
	void P_TmMileRptCheck();

	bool LocalTypeToReportType(DWORD v_dwLocalType, short *v_shReportType);
	bool ReportTyepToLocalType(short v_shReportType, DWORD &v_dwLocalType);	
	byte	m_accsta;
	DWORD   m_dwAreaParkTime[MAX_AREA_COUNT], m_dwInRegPatkTime[MAX_AREA_COUNT], m_dwOutRegPatkTime[MAX_AREA_COUNT], m_dwParkTime;
private:
	DWORD	m_dwReportFlag;
	char	m_szRID[32][20];
	byte	m_bytPriority[32];
	ushort	m_usInterval[32];
	int		m_iReportTimers[32];
	int		m_iMaxTimers[32];
	DWORD	m_dwLastReportTm[32];
	DWORD   m_dwDriverTime;
	DWORD   m_dwRestTime;
	pthread_mutex_t		m_mutex_alert;
	byte	m_bytSpdOverValue;
	tag2QAlertCfg		m_objAlertCfg;
	tag2QAutoRptCfg		m_objAutoCfg;

};

#endif

