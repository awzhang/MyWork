#include "yx_QianStdAfx.h"

#if USE_LIAONING_SANQI == 1

void G_TmAutoReport(void *arg, int len)
{
	g_objReport.P_TmAutoReport();
}

void G_TmAlertReport(void *arg, int len)
{
	g_objReport.P_TmAlertReport();
}

void G_TmSpeedCheck(void *arg, int len)
{
	g_objReport.P_TmSpeedCheck();
}

void G_TmAreaCheck(void *arg, int len)
{
	g_objReport.P_TmAreaCheck();
}

void G_TmSpeedRptCheck(void *arg, int len)
{
	g_objReport.P_TmSpeedRptCheck();
}

void G_TmParkTimeRptCheck(void *arg, int len)
{
	g_objReport.P_TmParkTimeRptCheck();
}

void G_TmOutLineRptCheck(void *arg, int len)
{
	g_objReport.P_TmOutLineRptCheck();
}

void G_TmStartengCheck(void *arg, int len)
{
	g_objReport.P_TmStartengCheck();
}

void G_TmTireRptCheck(void *arg, int len)
{
	g_objReport.P_TmTireRptCheck();
}

void G_TmMileRptCheck(void *arg, int len)
{
	g_objReport.P_TmMileRptCheck();
}
//////////////////////////////////////////////////////////////////////////

	#undef MSG_HEAD
	#define MSG_HEAD	("QianExe-LN_Report")

CLN_Report::CLN_Report()
{
	m_dwReportFlag = 0;
	memset(m_szRID, 0, sizeof(m_szRID));
	memset(m_bytPriority, 0, sizeof(m_bytPriority));
	memset(m_usInterval, 0, sizeof(m_usInterval));
	memset(m_iReportTimers, 0, sizeof(m_iReportTimers));
	memset(m_iMaxTimers, 0, sizeof(m_iMaxTimers));
	memset(m_dwLastReportTm, 0, sizeof(m_dwLastReportTm));
	m_accsta = 0;
//	m_dwParkTime = 0;
//	m_dwAreaParkTime = 0;
//	m_dwInRegPatkTime = 0;
//	m_dwOutRegPatkTime = 0;
	m_dwDriverTime = 0;
	m_dwRestTime = 0;
	m_bytSpdOverValue = 0;
}

CLN_Report::~CLN_Report()
{

}

int CLN_Report::Init()
{
	pthread_mutex_init(&m_mutex_alert, NULL);
	m_dwParkTime = GetTickCount();
	for ( int i = 0; i < MAX_AREA_COUNT; i++ )
	{
		m_dwAreaParkTime[i] = GetTickCount();
		m_dwInRegPatkTime[i] = GetTickCount();
		m_dwOutRegPatkTime[i] = GetTickCount();
	}
	GetSecCfg( (void*)&m_objAutoCfg, sizeof(m_objAutoCfg), offsetof(tagSecondCfg, m_uni2QAutoRptCfg), sizeof(m_objAutoCfg) );

	// 报警检测定时器，程序启动时即开启，不再关闭
	_SetTimer(&g_objTimerMng, ALERT_REPORT_TIMER, 1000, G_TmAlertReport);

	// 主动上报定时器，程序启动时即开启，不再关闭
	_SetTimer(&g_objTimerMng, AUTO_REPORT_TIMER, 1000, G_TmAutoReport);

	// 超速或低速报告
	BeginSpeedCheck();
	// 区域报告
	BeginAreaCheck();
	//点火报告
	BeginStartengCheck();
	//疲劳报告
	BeginTireRptCheck();
	//速度报告
	BeginSpeedRptCheck();
	//停车时间报告
	BeginParkTimeRptCheck();
	//偏航报告
	BeginOutLineRptCheck();
	//里程报告
	BeginMileRptCheck();
}

int CLN_Report::Release()
{
	pthread_mutex_destroy(&m_mutex_alert);
}

// 开启或关闭报警
int CLN_Report::SetAlert(bool v_bEnable, DWORD v_dwAlertType, DWORD SubPar)
{   
	if ( !v_bEnable )		 // 若是要撤销报警
	{
		pthread_mutex_lock(&m_mutex_alert);
		m_dwReportFlag      &= ~v_dwAlertType;
		pthread_mutex_unlock(&m_mutex_alert);

		// 撤销抢劫报警时，取消红灯急闪
		if ( v_dwAlertType == ALERT_TYPE_FOOT )
		{
			char szbuf[2] = {0};
			szbuf[0] = 0x02;
			szbuf[1] = 0x06;	// 撤销抢劫报警通知
			DataPush(szbuf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		}

		return 0;
	}

	// 否则即是开启报警
	tag2QReportCfg objReportCfg;
	tag2QReportPar objReportPar;

	tag2QAreaRptCfg     objAreaRptCfg;  
	tag2QAreaRptPar     objAreaRptPar;

	tag2QAreaSpdRptCfg  objAreaSpdRptCfg;
	tag2QAreaSpdRptPar  objAreaSpdRptPar;

	tag2QAreaParkTimeRptCfg objAreaParkTimeRptCfg;
	tag2QAreaParkTimeRptPar objAreaParkTimeRptPar;

	tag2QLineRptCfg     objLineRptCfg;
	tag2QLineRptPar     objLineRptPar;
	int i=0;		// 报警标志字的第几位
	tag2QAlertCfg objAlertCfg;
	GetSecCfg((void*)&objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));

	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg)); 

	GetSecCfg((void*)&objAreaRptCfg, sizeof(objAreaRptCfg), offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg)); 

	GetSecCfg((void*)&objAreaSpdRptCfg, sizeof(objAreaSpdRptCfg), offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg)); 

	GetSecCfg((void*)&objAreaParkTimeRptCfg, sizeof(objAreaParkTimeRptCfg), offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg)); 

	GetSecCfg((void*)&objLineRptCfg, sizeof(objLineRptCfg), offsetof(tagSecondCfg, m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg)); 

	if ( v_dwAlertType == ALERT_TYPE_FOOT )
	{
		objReportPar = objReportCfg.m_objFootRptPar;
		i = 0;
	} else if ( v_dwAlertType == ALERT_TYPE_FRONTDOOR )
	{
		objReportPar = objReportCfg.m_objDoorRptPar;
		i = 3;
	} else if ( v_dwAlertType == ALERT_TYPE_POWOFF )
	{
		objReportPar = objReportCfg.m_objPowOffRptPar;
		i = 1;
	} else if ( v_dwAlertType == ALERT_TYPE_OVERSPEED )
	{
		objReportPar = objReportCfg.m_objOverSpdRptPar;
		i = 4;
	} else if ( v_dwAlertType == ALERT_TYPE_BELOWSPEED )
	{
		if ( 0 == SubPar )
		{
			objReportPar = objReportCfg.m_objBelowSpdRptPar;
		} else
		{
			objReportPar = objReportCfg.m_objCrossSpdRptPar;
		}
		i = 12;
	} else if ( v_dwAlertType == ALERT_TYPE_STARTENG )
	{
		objReportPar = objReportCfg.m_objStartEngPrtPar;
		i = 13;
	} else if ( v_dwAlertType == ALERT_TYPE_MILE )
	{
		objReportPar = objReportCfg.m_objMilePrtPar;
		i = 14;
	} else if ( v_dwAlertType == ALERT_TYPE_TIRE )
	{
		objReportPar = objReportCfg.m_objTirePrtPar;
		i = 19;
	}

//区域报警
	else if ( v_dwAlertType == ALERT_TYPE_AREA )
	{
		objAreaRptPar = objAreaRptCfg.m_objArea[SubPar];
		i = 16;
	} else if ( v_dwAlertType == ALERT_TYPE_INAREA )
	{
		objAreaRptPar = objAreaRptCfg.m_objInArea[SubPar];
		i = 23;
	} else if ( v_dwAlertType == ALERT_TYPE_OUTAREA )
	{
		objAreaRptPar = objAreaRptCfg.m_objOutArea[SubPar];
		i = 24;
	} else if ( v_dwAlertType == ALERT_TYPE_INREGION )
	{
		objAreaRptPar = objAreaRptCfg.m_objInRegion[SubPar];

		i = 25;
	} else if ( v_dwAlertType == ALERT_TYPE_OUTREGION )
	{
		objAreaRptPar = objAreaRptCfg.m_objOutRegion[SubPar];
		i = 26;
	}

//区域超速
	else if ( v_dwAlertType == ALERT_TYPE_TIMESPD )
	{
		objAreaSpdRptPar = objAreaSpdRptCfg.m_TimeSpd[SubPar];
		i = 17;
	} else if ( v_dwAlertType == ALERT_TYPE_TIMERESPD )
	{
		objAreaSpdRptPar = objAreaSpdRptCfg.m_objAreaSpd[SubPar];
		i = 20;
	} else if ( v_dwAlertType == ALERT_TYPE_INRESPD )
	{
		objAreaSpdRptPar = objAreaSpdRptCfg.m_objInRegionSpd[SubPar];
		i = 27;
	} else if ( v_dwAlertType == ALERT_TYPE_OUTRESPD )
	{
		objAreaSpdRptPar = objAreaSpdRptCfg.m_objOutRegionSpd[SubPar];
		i = 28;
	}

//区域停车超时
	else if ( v_dwAlertType == ALERT_TYPE_PARKTIMEOUT )
	{
		objAreaParkTimeRptPar = objAreaParkTimeRptCfg.m_objParkTime;
		i = 15;
	} else if ( v_dwAlertType == ALERT_TYPE_REPATIMEOUT )
	{
		objAreaParkTimeRptPar = objAreaParkTimeRptCfg.m_objAreaParkTime[SubPar];
		i = 22;
	} else if ( v_dwAlertType == ALERT_TYPE_INREPATOUT )
	{
		objAreaParkTimeRptPar = objAreaParkTimeRptCfg.m_objInRegionParkTime[SubPar];
		i = 29;
	} else if ( v_dwAlertType == ALERT_TYPE_OUTREPATOUT )
	{
		objAreaParkTimeRptPar = objAreaParkTimeRptCfg.m_objOutRegionParkTime[SubPar];
		i = 30;
	}

//偏航
	else if ( v_dwAlertType == ALERT_TYPE_OUTLINE )
	{
		objLineRptPar = objLineRptCfg.m_objLine[SubPar];
		i = 18;
	} else if ( v_dwAlertType == ALERT_TYPE_TIMEOUTLINE )
	{
		objLineRptPar = objLineRptCfg.m_objTimeLine[SubPar];
		i = 21;
	} else
	{
		PRTMSG(MSG_NOR, "Undefined Alerm Type, v_dwAlertType=%08x\n", v_dwAlertType);
		return ERR_PAR;
	}
	switch ( i )
	{
	case 0:
	case 1:
	case 3:
	case 4:
	case 12:
	case 13:
	case 14:
	case 19:
		{
			// 若该报警未开启，则不处理
			if ( true == v_bEnable && false == objReportPar.m_bEnable )
			{
				PRTMSG(MSG_NOR, "Deal Alerm Type:%08x, Bit:%d but is not Enable, Exit Alerm!\n", v_dwAlertType, i);
				return 0;
			}
			pthread_mutex_lock(&m_mutex_alert);
			if ( 0 == (m_dwReportFlag & v_dwAlertType) )
			{
				m_dwReportFlag      |= v_dwAlertType;
				memcpy(m_szRID[i], objReportPar.m_szRID, sizeof(m_szRID[i]));
				m_bytPriority[i]    = objReportPar.m_bytPriority; 
				m_usInterval[i]     = objReportPar.m_usInterval;
				m_dwLastReportTm[i] = GetTickCount() - (DWORD)m_usInterval[i]*1000 + (DWORD)objAlertCfg.m_usAlertDelay*1000;	// 
				m_iReportTimers[i]  = 0;							// 还未上报一次
				m_iMaxTimers[i]     = objReportPar.m_usCnt+1;		// 需上报的总次数
			}

			pthread_mutex_unlock(&m_mutex_alert);
		}
		break;
	case 16:
	case 23:
	case 24:
	case 25:
	case 26:
		{
			// 若该报警未开启，则不处理
			if ( true == v_bEnable && false == objAreaRptPar.m_bEnable )
			{
				PRTMSG(MSG_NOR, "Deal Alerm Type:%08x, Bit:%d SubPar:%d but is not Enable, Exit Alerm!\n", v_dwAlertType, i, SubPar);
				return 0;
			}
			pthread_mutex_lock(&m_mutex_alert);
			if ( 0 == (m_dwReportFlag & v_dwAlertType) )
			{
				m_dwReportFlag      |= v_dwAlertType;
				memcpy(m_szRID[i], objAreaRptPar.m_szRID, sizeof(m_szRID[i]));
				m_bytPriority[i]    = objAreaRptPar.m_bytPriority; 
				m_usInterval[i]     = objAreaRptPar.m_usInterval;
				m_dwLastReportTm[i] = GetTickCount() - (DWORD)m_usInterval[i]*1000 + (DWORD)objAlertCfg.m_usAlertDelay*1000;	// 
				m_iReportTimers[i]  = 0;							// 还未上报一次
				m_iMaxTimers[i]     = objReportPar.m_usCnt+1;		// 需上报的总次数
			}
			pthread_mutex_unlock(&m_mutex_alert);
		}
		break;

	case 17:
	case 20:
	case 27:
	case 28:
		{
			// 若该报警未开启，则不处理
			if ( true == v_bEnable && false == objAreaSpdRptPar.m_bEnable )
			{
				PRTMSG(MSG_NOR, "Deal Alerm Type:%08x, Bit:%d but is not Enable, Exit Alerm!\n", v_dwAlertType, i);
				return 0;
			}
			pthread_mutex_lock(&m_mutex_alert);
			if ( 0 == (m_dwReportFlag & v_dwAlertType) )
			{
				m_dwReportFlag      |= v_dwAlertType;
				memcpy(m_szRID[i], objAreaSpdRptPar.m_szRID, sizeof(m_szRID[i]));
				m_bytPriority[i]    = objAreaSpdRptPar.m_bytPriority; 
				m_usInterval[i]     = objAreaSpdRptPar.m_usInterval;
				m_dwLastReportTm[i] = GetTickCount() - (DWORD)m_usInterval[i]*1000 + (DWORD)objAlertCfg.m_usAlertDelay*1000;	// 
				m_iReportTimers[i]  = 0;							// 还未上报一次
				m_iMaxTimers[i]     = objReportPar.m_usCnt+1;		// 需上报的总次数
			}
			pthread_mutex_unlock(&m_mutex_alert);
		}
		break;
	case 15:
	case 22:
	case 29:
	case 30:
		{
			// 若该报警未开启，则不处理
			if ( true == v_bEnable && false == objAreaParkTimeRptPar.m_bEnable )
			{
				PRTMSG(MSG_NOR, "Deal Alerm Type:%08x, Bit:%d but is not Enable, Exit Alerm!\n", v_dwAlertType, i);
				return 0;
			}
			pthread_mutex_lock(&m_mutex_alert);
			if ( 0 == (m_dwReportFlag & v_dwAlertType) )
			{
				m_dwReportFlag      |= v_dwAlertType;
				memcpy(m_szRID[i], objAreaParkTimeRptPar.m_szRID, sizeof(m_szRID[i]));
				m_bytPriority[i]    = objAreaParkTimeRptPar.m_bytPriority; 
				m_usInterval[i]     = objAreaParkTimeRptPar.m_usInterval;
				m_dwLastReportTm[i] = GetTickCount() - (DWORD)m_usInterval[i]*1000 + (DWORD)objAlertCfg.m_usAlertDelay*1000;	// 
				m_iReportTimers[i]  = 0;							// 还未上报一次
				m_iMaxTimers[i]     = objReportPar.m_usCnt+1;		// 需上报的总次数
			}
			pthread_mutex_unlock(&m_mutex_alert);
		}
		break;

	case 18:
	case 21:
		{
			// 若该报警未开启，则不处理
			if ( true == v_bEnable && false == objLineRptPar.m_bEnable )
			{
				PRTMSG(MSG_NOR, "Deal Alerm Type:%08x, Bit:%d but is not Enable, Exit Alerm!\n", v_dwAlertType, i);
				return 0;
			}
			pthread_mutex_lock(&m_mutex_alert);
//			PRTMSG(MSG_NOR, "m_dwReportFlag:%08x, v_dwAlertType:%08x!\n", m_dwReportFlag, v_dwAlertType);
			if ( 0 == (m_dwReportFlag & v_dwAlertType) )
			{

				m_dwReportFlag      |= v_dwAlertType;
				memcpy(m_szRID[i], objLineRptPar.m_szRID, sizeof(m_szRID[i]));
				m_bytPriority[i]    = objLineRptPar.m_bytPriority; 
				m_usInterval[i]     = objLineRptPar.m_usInterval;
				m_dwLastReportTm[i] = GetTickCount() - (DWORD)m_usInterval[i]*1000 + (DWORD)objAlertCfg.m_usAlertDelay*1000;	// 
				m_iReportTimers[i]  = 0;							// 还未上报一次
				m_iMaxTimers[i]     = objReportPar.m_usCnt+1;		// 需上报的总次数
			}
			pthread_mutex_unlock(&m_mutex_alert);
		}
		break;

	default:
		break;
	}

	// 若是抢劫报警，则红灯急闪
	if ( v_dwAlertType == ALERT_TYPE_FOOT )
	{
		char szbuf[2] = {0};
		szbuf[0] = 0x02;
		szbuf[1] = 0x05;	// 抢劫报警通知
		DataPush(szbuf, 2, DEV_QIAN, DEV_DIAODU, LV2);
	}
	return 0;
}

void CLN_Report::P_TmAlertReport()
{
	int     i = 0;
	DWORD   dwJudgeFlag = 0;

	short   shAlermType = 0;
	char    szRID[20] = {0};
	byte    bytPriority = 0;

	dwJudgeFlag = 0x00000001;

	pthread_mutex_lock(&m_mutex_alert);

	for (i=0; i<32; i++)
	{
		// 若报警状态字的某个bit为1，说明要进行相应的报警
		if ( dwJudgeFlag & m_dwReportFlag )
		{
			// 且距离上次上报时间已超过了上报间隔
//			PRTMSG(MSG_DBG, "CurT:%d, Interval:%d\n", (GetTickCount() - m_dwLastReportTm[i])/1000, m_usInterval[i]);
			if ( GetTickCount() > m_dwLastReportTm[i] && ((GetTickCount() - m_dwLastReportTm[i])/1000) > m_usInterval[i] )
			{
				if ( !LocalTypeToReportType(dwJudgeFlag, &shAlermType) )
					continue;

				memcpy(szRID, m_szRID[i], sizeof(szRID));

				bytPriority = m_bytPriority[i];
				PRTMSG(MSG_DBG, "Report flag:%x\n", m_dwReportFlag);
				// 发送一条上报数据
				SendOneAlertReport(shAlermType, szRID, bytPriority);

				// 更新已上报次数、上次上报时刻
				m_iReportTimers[i]++;
				m_dwLastReportTm[i] = GetTickCount();

				// 若发送次数已满，则清除该报警的标志比特位
				if ( m_iReportTimers[i] >= m_iMaxTimers[i] )
				{
					m_dwReportFlag &= ~dwJudgeFlag;
					// 撤销抢劫报警时，取消红灯急闪
					if ( dwJudgeFlag == ALERT_TYPE_FOOT )
					{
						char szbuf[2] = {0};
						szbuf[0] = 0x02;
						szbuf[1] = 0x06;	// 撤销抢劫报警通知
						DataPush(szbuf, 2, DEV_QIAN, DEV_DIAODU, LV2);
					}
				}
			}
		}

		dwJudgeFlag = dwJudgeFlag << 1;
	}

	pthread_mutex_unlock(&m_mutex_alert);
}

// 发送一条报告上报信息
void CLN_Report::SendOneAlertReport(short v_shAlermType, char *v_szRID, byte v_bytPriority)
{
	char    szSendBuf[1024] = {0};
	DWORD   dwSendLen = 0;
	char    szTempBuf[1024] = {0};
	DWORD   dwTempLen = 0;

	szSendBuf[dwSendLen++] = 0x40;	// 命令类型：异步上报
	szSendBuf[dwSendLen++] = 0x00;	// 动作：报告上报
	szSendBuf[dwSendLen++] = 0x03;
	szSendBuf[dwSendLen++] = 0x00;	// 不需要中心返回异步上报响应

	// AKV: rname
	szSendBuf[dwSendLen++] = 0x00;	// Attr
	szSendBuf[dwSendLen++] = 0x05;	// K_len
	szSendBuf[dwSendLen++] = 'r';	// Key
	szSendBuf[dwSendLen++] = 'n';
	szSendBuf[dwSendLen++] = 'a';
	szSendBuf[dwSendLen++] = 'm';
	szSendBuf[dwSendLen++] = 'e';
	szSendBuf[dwSendLen++] = 0x00;	// V_len
	szSendBuf[dwSendLen++] = 0x02;
	memcpy(szSendBuf+dwSendLen, (void*)&v_shAlermType, 2);	// Value
	dwSendLen += 2;

	// AKV: rid
	szSendBuf[dwSendLen++] = 0x00;	// Attr
	szSendBuf[dwSendLen++] = 0x03;	// K_len
	szSendBuf[dwSendLen++] = 'r';	// Key
	szSendBuf[dwSendLen++] = 'i';
	szSendBuf[dwSendLen++] = 'd';
	szSendBuf[dwSendLen++] = 0x00;	// V_len
	szSendBuf[dwSendLen++] = strlen(v_szRID);
	memcpy(szSendBuf+dwSendLen, v_szRID, strlen(v_szRID));
	dwSendLen += strlen((v_szRID));

	// AKV: p
	szSendBuf[dwSendLen++] = 0x00;	// Attr
	szSendBuf[dwSendLen++] = 0x01;	// K_len
	szSendBuf[dwSendLen++] = 'p';	// Key
	szSendBuf[dwSendLen++] = 0x00;	// V_len
	szSendBuf[dwSendLen++] = 0x01;
	szSendBuf[dwSendLen++] = (char)v_bytPriority;
	if (0x0007 == ntohs(v_shAlermType))
	{
// AKV:RMAP
		szSendBuf[dwSendLen++] = 0x10;	// Attr
		szSendBuf[dwSendLen++] = 0x04;	// K_len
		szSendBuf[dwSendLen++] = 'R';	// Key		
		szSendBuf[dwSendLen++] = 'M';	// Key
		szSendBuf[dwSendLen++] = 'A';	// Key								
		szSendBuf[dwSendLen++] = 'P';	// Key				
		szSendBuf[dwSendLen++] = 0x00;	// V_len
		szSendBuf[dwSendLen++] = 0x06;	// V_len
// 速度
		szSendBuf[dwSendLen++] = 0x00;	// Attr4
		szSendBuf[dwSendLen++] = 0x01;	// K_len4
		szSendBuf[dwSendLen++] = 's';	// Key4: s 速度
		szSendBuf[dwSendLen++] = 0x00;	// V_len4
		szSendBuf[dwSendLen++] = 0x01;
		szSendBuf[dwSendLen++] = m_bytSpdOverValue; 
	}

	// AKV:TSMAP
	szSendBuf[dwSendLen++] = 0x10;	// Attr
	szSendBuf[dwSendLen++] = 0x05;	// K_len
	szSendBuf[dwSendLen++] = 'T';	// Key
	szSendBuf[dwSendLen++] = 'S';	// Key		
	szSendBuf[dwSendLen++] = 'M';	// Key
	szSendBuf[dwSendLen++] = 'A';	// Key								
	szSendBuf[dwSendLen++] = 'P';	// Key				
	szSendBuf[dwSendLen++] = 0x00;	// V_len
	szSendBuf[dwSendLen++] = 0x06;	// V_len
	szSendBuf[dwSendLen++] = 0x00;
	szSendBuf[dwSendLen++] = 0x01;
	szSendBuf[dwSendLen++] = 'a';
	szSendBuf[dwSendLen++] = 0x00;	// V_len
	szSendBuf[dwSendLen++] = 0x01;	// V_len
	szSendBuf[dwSendLen++] = m_accsta;  

	// 位置信息，速度，方向，里程
	dwTempLen = G_GetPoint(szTempBuf);
	memcpy(szSendBuf+dwSendLen, szTempBuf, dwTempLen);
	dwSendLen += dwTempLen;

	// 发送
	g_objApplication.SendAppFrame(MODE_B, 0x90, true, 0, szSendBuf, dwSendLen);

	PRTMSG(MSG_NOR, "Send One Alert Report!\n");
}

// 开始主动位置上报
int CLN_Report::BeginPositionReport(char *v_szId, char *v_szBeginTime, char v_szIntervalType, DWORD v_dwIntervalValue, char v_szEndType, DWORD v_dwMaxCnt, char *v_szEndTime)
{
	for (int i=0; i<MAX_AUTORPT_CNT; i++)
	{
		if ( m_objAutoCfg.m_bNeedReport[i] == false )
		{
			m_objAutoCfg.m_bNeedReport[i] = true;
			memcpy(m_objAutoCfg.m_szId[i], v_szId, min(sizeof(m_objAutoCfg.m_szId[i]), strlen(v_szId)) );
			memcpy(m_objAutoCfg.m_szBeginTime[i], v_szBeginTime, 6);
			m_objAutoCfg.m_szIntervalType[i] = v_szIntervalType;
			m_objAutoCfg.m_dwLastRptValue[i] = 0;
			m_objAutoCfg.m_dwIntervalValue[i] = v_dwIntervalValue*1000;
			m_objAutoCfg.m_szEndType[i] = v_szEndType;
			memcpy(m_objAutoCfg.m_szEndTime[i], v_szEndTime, 6);
			m_objAutoCfg.m_dwReportCnt[i] = 0;
			m_objAutoCfg.m_dwMaxCnt[i] = v_dwMaxCnt;

// 			PRTMSG(MSG_DBG, "One AutoRpt:\n");
// 			printf("%s\n", m_objAutoCfg.m_szId[i]);
// 			printf("BeginTime:");PrintString(m_objAutoCfg.m_szBeginTime[i], 6);
// 			printf("IntervalType: %02x\n", m_objAutoCfg.m_szIntervalType[i]);
// 			printf("IntervalValue: %d\n", m_objAutoCfg.m_dwIntervalValue[i]);
// 			printf("EndType: %02x\n", m_objAutoCfg.m_szEndType[i]);
// 			printf("ReportCnt: %d\n", m_objAutoCfg.m_dwMaxCnt[i]);
// 			printf("EndTime:");PrintString(m_objAutoCfg.m_szEndTime[i], 6);

			PRTMSG(MSG_NOR, "Begin Auto Report!\n");

			break;
		}
	}

	// 保存到配置区
	SetSecCfg((void*)&m_objAutoCfg, offsetof(tagSecondCfg, m_uni2QAutoRptCfg), sizeof(m_objAutoCfg));
}

void CLN_Report::P_TmAutoReport()
{
	tagGPSData objGps(0);
	GetCurGps(&objGps, sizeof(objGps), GPS_REAL);

	// 获取距离间隔
	static DWORD sta_dwLastDis = 0;
	static double sta_dLastLon = 0;
	static double sta_dLastLat = 0;
	sta_dwLastDis += (DWORD)G_CalGeoDis2(sta_dLastLon, sta_dLastLat, objGps.longitude, objGps.latitude);
	sta_dLastLat = objGps.latitude;
	sta_dLastLon = objGps.longitude;

	// 获取系统当前时间
	char szCurTime[6] = {0};
	time_t lCurrentTime;
	struct tm *pCurrentTime;    
	time(&lCurrentTime);
	pCurrentTime = localtime(&lCurrentTime); 

	// UTC时间转成北京时间
	struct tm pTemp;
	memcpy((void*)&pTemp, (void*)pCurrentTime, sizeof(pTemp));
	G_GetLocalTime(&pTemp);
	pCurrentTime = &pTemp;

	szCurTime[0] = (char)(pCurrentTime->tm_year-100);
	szCurTime[1] = (char)(pCurrentTime->tm_mon+1);
	szCurTime[2] = (char)pCurrentTime->tm_mday;
	szCurTime[3] = (char)pCurrentTime->tm_hour;
	szCurTime[4] = (char)pCurrentTime->tm_min;
	szCurTime[5] = (char)pCurrentTime->tm_sec;

	bool bNeedSaveCfg = false;
	bool bNeedReport = false;

	for (int i=0; i<MAX_AUTORPT_CNT; i++)
	{
		bNeedReport = false;

		if ( m_objAutoCfg.m_bNeedReport[i] )
		{
			// 判断起始时间
			if ( strcmp(szCurTime, m_objAutoCfg.m_szBeginTime[i]) < 0 )
			{
				continue;
			}

			// 判断结束时间，或者次数
			if ( (m_objAutoCfg.m_szEndType[i] == '6' && strcmp(szCurTime, m_objAutoCfg.m_szEndTime[i]) > 0)
				 ||
				 (m_objAutoCfg.m_szEndType[i] == '1' && m_objAutoCfg.m_dwReportCnt[i] >= m_objAutoCfg.m_dwMaxCnt[i] )
			   )
			{
				PRTMSG(MSG_NOR, "End Auto Report!\n");
				m_objAutoCfg.m_bNeedReport[i] = false;
				bNeedSaveCfg = true;
				continue;
			}

			// 若是定时上报，则判断时间间隔
			if ( m_objAutoCfg.m_szIntervalType[i] == '7')
			{
				//PRTMSG(MSG_DBG, "%ld, %ld, %ld\n", GetTickCount(),m_objAutoCfg.m_dwLastRptValue[i],m_objAutoCfg.m_dwIntervalValue[i] );
				if ( (GetTickCount() - m_objAutoCfg.m_dwLastRptValue[i]) > m_objAutoCfg.m_dwIntervalValue[i] )
					bNeedReport = true;
			}

			// 若是定距上报，则判断距离间隔
			if ( m_objAutoCfg.m_szIntervalType[i] == '9')
			{
				if ( sta_dwLastDis - m_objAutoCfg.m_dwLastRptValue[i] > m_objAutoCfg.m_dwIntervalValue[i] )
					bNeedReport = true;
			}

			// 若需要上报
			if ( bNeedReport )
			{
				SendOnePositionReport(0x00);

				// 上报之后要更新上报次数，上次上报时刻，上报距离
				m_objAutoCfg.m_dwReportCnt[i]++;
				if ( m_objAutoCfg.m_szIntervalType[i] == '7')
					m_objAutoCfg.m_dwLastRptValue[i] = GetTickCount();
				if ( m_objAutoCfg.m_szIntervalType[i] == '9')
					m_objAutoCfg.m_dwLastRptValue[i] = sta_dwLastDis;
			}
		}
	}

	if ( bNeedSaveCfg )
	{
		SetSecCfg((void*)&m_objAutoCfg, offsetof(tagSecondCfg, m_uni2QAutoRptCfg), sizeof(m_objAutoCfg));
	}
}

// 发送一条位置上报信息
void CLN_Report::SendOnePositionReport(char v_szReportType)
{
	char    szSendBuf[512] = {0};
	DWORD   dwSendLen = 0;

	char    szTempBuf[512] = {0};
	DWORD   dwTempLen = 0;

	szSendBuf[dwSendLen++] = 0x40;	// 命令类型：异步上报
	szSendBuf[dwSendLen++] = 0x00;	// 动作：位置上报
	szSendBuf[dwSendLen++] = 0x01;

	szSendBuf[dwSendLen++] = 0x00;	// 不需要中心返回异步上报响应

	// AKV: 上报类型
	szSendBuf[dwSendLen++] = 0x00;	// Attr
	szSendBuf[dwSendLen++] = 0x03;	// K_len
	szSendBuf[dwSendLen++] = 'T';	// Key
	szSendBuf[dwSendLen++] = 'i';
	szSendBuf[dwSendLen++] = 'd';
	szSendBuf[dwSendLen++] = 0x00;	// V_len
	szSendBuf[dwSendLen++] = 0x04;
	if ( v_szReportType == 0x01 )	 // Value
	{
		memcpy(szSendBuf+dwSendLen, "CALL", 4);
	} else
	{
		memcpy(szSendBuf+dwSendLen, "PRST", 4);
	}
	dwSendLen += 4;

	// 位置信息，速度，方向，里程
	dwTempLen = G_GetPoint(szTempBuf);
	memcpy(szSendBuf+dwSendLen, szTempBuf, dwTempLen);
	dwSendLen += dwTempLen;

	// 发送
	g_objApplication.SendAppFrame(MODE_B, 0x90, true, 0, szSendBuf, dwSendLen);

	PRTMSG(MSG_NOR, "Send One Postion Report!\n");
}

int CLN_Report::BeginSpeedCheck()
{
	int iret;
	tag2QReportCfg objReportCfg;
	iret = GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));

	//iret = GetSecCfg((void*)&m_objAlertCfg, sizeof(m_objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(m_objAlertCfg));
	if ( 0 != iret )
		return ERR_CFG;

	if ( true == objReportCfg.m_objOverSpdRptPar.m_bEnable || true == objReportCfg.m_objBelowSpdRptPar.m_bEnable || true == objReportCfg.m_objCrossSpdRptPar.m_bEnable)
	{
		// 启动速度检查定时器
		_SetTimer(&g_objTimerMng, SPEED_CHECK_TIMER, 1000, G_TmSpeedCheck);
	}
}

int CLN_Report::EndSpeedCheck()
{
	_KillTimer(&g_objTimerMng, SPEED_CHECK_TIMER);
}

void CLN_Report::P_TmSpeedCheck()
{
	static DWORD sta_dwOverCnt = 0;
	static DWORD sta_dwBelowCnt = 0;
	static byte  sta_bytCrossSpd = 0, sta_bytCrossSpdBak = 0;
	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS(objGps, false);

	if ( 'V' == objGps.valid )
		return;
	tag2QReportCfg objReportCfg;
	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	// 先进行超速判断
	if ( true == objReportCfg.m_objOverSpdRptPar.m_bEnable)
	{
		if ( (byte)(3.6*objGps.speed) > m_objAlertCfg.m_bytOverSpd )
			sta_dwOverCnt++;

		if ( sta_dwOverCnt > m_objAlertCfg.m_dwOverPrid )
		{
			SetAlert(true, ALERT_TYPE_OVERSPEED, 0);
			sta_dwOverCnt = 0;
			m_bytSpdOverValue = (byte)(3.6*objGps.speed) - m_objAlertCfg.m_bytOverSpd;
		}
	}

	// 再进行低速判断
	if ( m_objAlertCfg.m_bytBelowSpd != 0 && true == objReportCfg.m_objBelowSpdRptPar.m_bEnable)
	{
		if ( (byte)(3.6*objGps.speed) < m_objAlertCfg.m_bytBelowSpd )
			sta_dwBelowCnt++;

		if ( sta_dwBelowCnt > m_objAlertCfg.m_dwBelowPrid )
		{
			SetAlert(true, ALERT_TYPE_BELOWSPEED, 0);
			sta_dwBelowCnt = 0;
			m_bytSpdOverValue = m_objAlertCfg.m_bytBelowSpd - (byte)(3.6*objGps.speed);
		}
	}
	if ( m_objAlertCfg.m_bytCrossSpd != 0 && true == objReportCfg.m_objCrossSpdRptPar.m_bEnable)
	{
		if ( (byte)(3.6*objGps.speed) < m_objAlertCfg.m_bytCrossSpd )
		{
			sta_bytCrossSpd = 0;
		} else if ( (byte)(3.6*objGps.speed) > m_objAlertCfg.m_bytCrossSpd )
		{
			sta_bytCrossSpd = 1;
		} else
		{
			return;
		}
		if ( sta_bytCrossSpd ^ sta_bytCrossSpdBak )
		{
			SetAlert(true, ALERT_TYPE_BELOWSPEED, 1);
			m_bytSpdOverValue = 0;
		}
		sta_bytCrossSpdBak = sta_bytCrossSpd;
	}
}

bool CLN_Report::AreaEnCheck(tag2QAreaRptCfg *objAreaRptCfg)
{
	int i;
	for ( i = 0; i < MAX_AREA_COUNT; i++ )
	{
		if (true == objAreaRptCfg->m_objArea[i].m_bEnable || true == objAreaRptCfg->m_objInArea[i].m_bEnable || true == objAreaRptCfg->m_objOutArea[i].m_bEnable || true == objAreaRptCfg->m_objInRegion[i].m_bEnable || true == objAreaRptCfg->m_objOutRegion[i].m_bEnable)
		{
			return true;
		}
	}
	return false;
}

int CLN_Report::BeginAreaCheck()
{
	int iret;
	tag2QAreaRptCfg     objAreaRptCfg;
	iret = GetSecCfg((void*)&objAreaRptCfg, sizeof(objAreaRptCfg), offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));
	if ( 0 != iret || false == AreaEnCheck(&objAreaRptCfg) )
		return ERR_CFG;
	PRTMSG(MSG_DBG, "Start area report check!\n");
	_KillTimer(&g_objTimerMng, AREA_CHECK_TIMER);
	_SetTimer(&g_objTimerMng, AREA_CHECK_TIMER, 1000, G_TmAreaCheck);
	return 0;
}

int CLN_Report::EndAreaCheck()
{
	_KillTimer(&g_objTimerMng, AREA_CHECK_TIMER);
}

void CLN_Report::P_TmAreaCheck()
{
	static DWORD sta_dwAreaInCnt[MAX_AREA_COUNT] = {0};
	static DWORD sta_dwAreaOutCnt[MAX_AREA_COUNT] = {0};
	static bool bIsFirstTime[MAX_AREA_COUNT] = {true};
	bool bMeetArea[MAX_AREA_COUNT] = {false};
	bool bMeetTime[MAX_TIME_COUNT] = {false};
	byte bCurPos[MAX_TIME_COUNT] = {0};	 //上一次位于区域内或外
	static byte bCurPosBak[MAX_AREA_COUNT] = {0};

	bool bret;
	int  i=0, j=0;
	byte AttrRes;
	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS(objGps, false);

	if ( 'V' == objGps.valid )
		return;

	// 把当前时间转成字符串格式，以便于比较
	char szCurTime[3] = {0};
	szCurTime[0] = (char)objGps.nHour;
	szCurTime[1] = (char)objGps.nMinute;
	szCurTime[2] = (char)objGps.nSecond;

//	PRTMSG(MSG_DBG, "lat:%f  lon:%f", objGps.latitude, objGps.longitude);
	tag2QAreaRptCfg     objAreaRptCfg;
	GetSecCfg((void*)&objAreaRptCfg, sizeof(objAreaRptCfg), offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));
//区域 报警
	for (i = 0; i < MAX_AREA_COUNT; i++)
	{
		if ( !objAreaRptCfg.m_objArea[i].m_bEnable )
			continue;
		// 先判断是否符合区域范围条件
		bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
							objAreaRptCfg.m_objArea[i].m_lMinLon,
							objAreaRptCfg.m_objArea[i].m_lMinLat, 
							objAreaRptCfg.m_objArea[i].m_lMaxLon,
							objAreaRptCfg.m_objArea[i].m_lMaxLat, 0x02, &AttrRes);
		if ( 0xff != AttrRes)
		{
			if ( bret)
			{ //当前在区域中	
				if ( sta_dwAreaOutCnt[i] > 0 )
				{
					sta_dwAreaOutCnt[i]--;
				}
				sta_dwAreaInCnt[i]++;               
			} else
			{ //当前在区域外
				if ( sta_dwAreaInCnt[i] > 0 )
				{
					sta_dwAreaInCnt[i]--;
				}
				sta_dwAreaOutCnt[i]++;
			}
		} else
		{
			sta_dwAreaInCnt[i] = 0;
			sta_dwAreaOutCnt[i] = 0;
			continue;
		}

		if ( sta_dwAreaInCnt[i] >= POS_CONTCOUNT )
		{
			bCurPos[i] = 1;
			sta_dwAreaInCnt[i] = 0;
			sta_dwAreaOutCnt[i] = 0;
			if ( 0x03 == objAreaRptCfg.m_objArea[i].m_bytAreaAttr)
			{
				bMeetArea[i] = true;
			}
		} else if ( sta_dwAreaOutCnt[i] >= POS_CONTCOUNT )
		{
			bCurPos[i] = 0;
			sta_dwAreaOutCnt[i] = 0;
			sta_dwAreaInCnt[i] = 0;
			if ( 0x04 == objAreaRptCfg.m_objArea[i].m_bytAreaAttr)
			{
				bMeetArea[i] = true;
			}
		} else
		{
			continue;
		}
		if (bIsFirstTime[i])
		{
			bIsFirstTime[i] = false;
			bCurPosBak[i] = bCurPos[i];
		}
		if (0x01 == objAreaRptCfg.m_objArea[i].m_bytAreaAttr)
		{ //进区域
			if ( (bCurPos[i] ^ bCurPosBak[i]) & bCurPos[i] )
			{
				bMeetArea[i] = true;
			}
		} else if (0x02 == objAreaRptCfg.m_objArea[i].m_bytAreaAttr)
		{ //出区域
			if ( (bCurPos[i] ^ bCurPosBak[i]) & bCurPosBak[i] )
			{
				bMeetArea[i] = true;
			}
		}
		bCurPosBak[i] = bCurPos[i];
		// 
		bMeetTime[i] = false;

		for (j=0; j<MAX_TIME_COUNT; j++)
		{
			if ( objAreaRptCfg.m_objArea[i].m_bTimeEnable[j] )
			{
				if ( strncmp(szCurTime, objAreaRptCfg.m_objArea[i].m_szBeginTime[j], 3) >= 0
					 && strncmp(szCurTime, objAreaRptCfg.m_objArea[i].m_szEndTime[j], 3) <= 0 )
				{
					bMeetTime[i] = true;
					break;
				}
			}
		}

		//若区域和时间范围都符合，则上报一条信息
		if ( bMeetArea[i] && bMeetTime[i] )
		{
			PRTMSG(MSG_DBG, "AreaAttr:%d Area alert!!!!\n", objAreaRptCfg.m_objArea[i].m_bytAreaAttr);
			SetAlert(true, ALERT_TYPE_AREA, i);
		}
	}
//进出区域报警
	static DWORD sta_dwInAreaInCnt[MAX_AREA_COUNT] = {0}; 
	static DWORD sta_dwInAreaOutCnt[MAX_AREA_COUNT] = {0}; 
	byte bInAreaPos[MAX_TIME_COUNT] = {0};	 //上一次位于区域内或外
	static byte bInAreaPosBak[MAX_TIME_COUNT] = {0};
	static bool bIsFirstTime_IA[MAX_AREA_COUNT] = {true};
	static DWORD sta_dwOutAreaInCnt[MAX_AREA_COUNT] = {0};
	static DWORD sta_dwOutAreaOutCnt[MAX_AREA_COUNT] = {0};
	byte bOutArePos[MAX_TIME_COUNT] = {0};	 //上一次位于区域内或外
	static bool bIsFirstTime_OA[MAX_AREA_COUNT] = {true};
	static byte bOutArePosBak[MAX_TIME_COUNT] = {0};

	for (i=0; i<MAX_AREA_COUNT; i++)
	{
		if ( objAreaRptCfg.m_objInArea[i].m_bEnable )
		{
			// 先判断是否符合区域范围条件
			bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
								objAreaRptCfg.m_objInArea[i].m_lMinLon,
								objAreaRptCfg.m_objInArea[i].m_lMinLat, 
								objAreaRptCfg.m_objInArea[i].m_lMaxLon,
								objAreaRptCfg.m_objInArea[i].m_lMaxLat, 0x02, &AttrRes);
			if ( 0xff != AttrRes)
			{
				if ( bret)
				{ //当前在区域中	
					if ( sta_dwInAreaOutCnt[i] > 0 )
					{
						sta_dwInAreaOutCnt[i]--;
					}
					sta_dwInAreaInCnt[i]++;               
				} else
				{ //当前在区域外
					if ( sta_dwInAreaInCnt[i] > 0 )
					{
						sta_dwInAreaInCnt[i]--;
					}
					sta_dwInAreaOutCnt[i]++;
				}
			} else
			{
				sta_dwInAreaInCnt[i] = 0;
				sta_dwInAreaOutCnt[i] = 0;
				continue;
			}

			if ( sta_dwInAreaInCnt[i] >= POS_CONTCOUNT )
			{
				bInAreaPos[i] = 1;
				sta_dwInAreaInCnt[i] = 0;
				sta_dwInAreaOutCnt[i] = 0;
			} else if ( sta_dwInAreaOutCnt[i] >= POS_CONTCOUNT )
			{
				bInAreaPos[i] = 0;
				sta_dwInAreaInCnt[i] = 0;
				sta_dwInAreaOutCnt[i] = 0;
			} else
			{
				continue;
			}
			if (bIsFirstTime_IA[i])
			{
				bIsFirstTime_IA[i] = false;
				bInAreaPosBak[i] = bInAreaPos[i];
			}
			if (( bInAreaPos[i] ^ bInAreaPosBak[i]) & bInAreaPos[i])
			{
				PRTMSG(MSG_DBG, "In area alert!!!!\n");
				SetAlert(true, ALERT_TYPE_INAREA, i);
			}
			bInAreaPosBak[i] = bInAreaPos[i];
		}

		if ( !objAreaRptCfg.m_objOutArea[i].m_bEnable )
			continue;
		// 先判断是否符合区域范围条件
		bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
							objAreaRptCfg.m_objOutArea[i].m_lMinLon,
							objAreaRptCfg.m_objOutArea[i].m_lMinLat, 
							objAreaRptCfg.m_objOutArea[i].m_lMaxLon,
							objAreaRptCfg.m_objOutArea[i].m_lMaxLat, 0x02, &AttrRes);
		if ( 0xff != AttrRes)
		{
			if ( !bret)
			{ //当前在区域外	
				if ( sta_dwOutAreaOutCnt[i] > 0 )
				{
					sta_dwOutAreaOutCnt[i]--;
				}
				sta_dwOutAreaInCnt[i]++;              
			} else
			{ //当前在区域内
				if ( sta_dwOutAreaInCnt[i] > 0 )
				{
					sta_dwOutAreaInCnt[i]--;
				}
				sta_dwOutAreaOutCnt[i]++;  
			}
		} else
		{
			sta_dwOutAreaInCnt[i] = 0;
			sta_dwOutAreaOutCnt[i] = 0;
			continue;
		}

		if ( sta_dwOutAreaInCnt[i] >= POS_CONTCOUNT )
		{
			bOutArePos[i] = 1;
			sta_dwOutAreaInCnt[i] = 0;
			sta_dwOutAreaOutCnt[i] = 0;       
		} else if ( sta_dwOutAreaOutCnt[i] >= POS_CONTCOUNT )
		{
			bOutArePos[i] = 0;
			sta_dwOutAreaInCnt[i] = 0;
			sta_dwOutAreaOutCnt[i] = 0;       
		} else
		{
			continue;
		}
		if (bIsFirstTime_OA[i])
		{
			bIsFirstTime_OA[i] = false;
			bOutArePosBak[i] = bOutArePos[i];
		}
		if (( bOutArePos[i] ^ bOutArePosBak[i]) & bOutArePosBak[i])
		{
			PRTMSG(MSG_DBG, "Out area alert!!!!\n");
			SetAlert(true, ALERT_TYPE_OUTAREA, i);
		}
		bOutArePosBak[i] = bOutArePos[i];
	}
//区域内外报警
	static DWORD sta_dwInRegionCnt[MAX_AREA_COUNT] = {0};   

	static DWORD sta_dwOutRegionCnt[MAX_AREA_COUNT] = {0};

	for (i=0; i<MAX_AREA_COUNT; i++)
	{
		if ( objAreaRptCfg.m_objInRegion[i].m_bEnable )
		{
			// 先判断是否符合区域范围条件
			bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
								objAreaRptCfg.m_objInRegion[i].m_lMinLon,
								objAreaRptCfg.m_objInRegion[i].m_lMinLat, 
								objAreaRptCfg.m_objInRegion[i].m_lMaxLon,
								objAreaRptCfg.m_objInRegion[i].m_lMaxLat, 0x02, &AttrRes);
			if ( 0xff != AttrRes)
			{
				if ( bret)
				{ //当前在区域中	
					sta_dwInRegionCnt[i]++;             
				} else
				{ //当前在区域外
					if ( sta_dwInRegionCnt[i] > 0 )
					{
						sta_dwInRegionCnt[i]--;
					}
				}
			} else
			{
				sta_dwInRegionCnt[i] = 0;
			}

			if ( sta_dwInRegionCnt[i] >= POS_CONTCOUNT )
			{
				PRTMSG(MSG_DBG, "In region alert!!!!\n");
				SetAlert(true, ALERT_TYPE_INREGION, i);
				sta_dwInRegionCnt[i] = 0;
			}

		}

		if ( !objAreaRptCfg.m_objOutRegion[i].m_bEnable )
			continue;
		// 先判断是否符合区域范围条件
		bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
							objAreaRptCfg.m_objOutRegion[i].m_lMinLon,
							objAreaRptCfg.m_objOutRegion[i].m_lMinLat, 
							objAreaRptCfg.m_objOutRegion[i].m_lMaxLon,
							objAreaRptCfg.m_objOutRegion[i].m_lMaxLat, 0x02, &AttrRes);
		if ( 0xff != AttrRes)
		{
			if ( !bret)
			{ //当前在区域外	
				sta_dwOutRegionCnt[i]++;                
			} else
			{ //当前在区域内
				if ( sta_dwOutRegionCnt[i] > 0 )
				{
					sta_dwOutRegionCnt[i]--;
				}
			}
		} else
		{
			sta_dwOutRegionCnt[i] = 0;
		}

		if ( sta_dwOutRegionCnt[i] >= POS_CONTCOUNT )
		{
			sta_dwOutRegionCnt[i] = 0;
			PRTMSG(MSG_DBG, "SubPar:%d Out region alert!!!!\n", i);
			SetAlert(true, ALERT_TYPE_OUTREGION, i);        
		}
	}
}

int  CLN_Report::BeginStartengCheck()
{
	int iret;
	tag2QReportCfg objReportCfg;

	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg)); 
	iret = GetSecCfg((void*)&m_objAlertCfg, sizeof(m_objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(m_objAlertCfg));
	if ( 0 != iret || false == objReportCfg.m_objStartEngPrtPar.m_bEnable)
		return ERR_CFG;

	if ( m_objAlertCfg.m_bytAccSta <= 0x01)
	{
		PRTMSG(MSG_DBG, "Start %s ACC alarm check!\n", m_objAlertCfg.m_bytAccSta ? "ON" : "OFF");
		_KillTimer(&g_objTimerMng, STARTENG_CHECK_TIMER);
		_SetTimer(&g_objTimerMng, STARTENG_CHECK_TIMER, 1000, G_TmStartengCheck);
	}

}
int  CLN_Report::EndStartengCheck()
{
	_KillTimer(&g_objTimerMng, STARTENG_CHECK_TIMER);
}
void CLN_Report::P_TmStartengCheck()
{
	static byte accsta = 0xff;
	if ( m_objAlertCfg.m_bytAccSta == m_accsta )
	{
		if ( accsta != m_accsta )
		{
			PRTMSG(MSG_DBG, "ACC %d change alarm!!\n", m_accsta);
			SetAlert(true, ALERT_TYPE_STARTENG, 0);
			accsta = m_accsta;
		}
	} else
	{
		accsta = 0xff;
	}
}

int CLN_Report::BeginSpeedRptCheck()
{
	int iret;
	tag2QAreaSpdRptCfg  objAreaSpdRptCfg;
	iret = GetSecCfg((void*)&objAreaSpdRptCfg, sizeof(objAreaSpdRptCfg), offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg));
	if ( 0 != iret || false == SpeedEnCheck(&objAreaSpdRptCfg))
	{
		return ERR_CFG;
	}
	PRTMSG(MSG_DBG, "Start speed report check!\n");
	_KillTimer(&g_objTimerMng, SPEEDRPT_CHECK_TIMER);
	_SetTimer(&g_objTimerMng, SPEEDRPT_CHECK_TIMER, 1000, G_TmSpeedRptCheck);
}
int CLN_Report::EndSpeedRptCheck()
{
	_KillTimer(&g_objTimerMng, SPEEDRPT_CHECK_TIMER);
}

void CLN_Report::P_TmSpeedRptCheck()
{
	static DWORD sta_dwAreaSpdInCnt[MAX_AREA_COUNT] = {0};
	static DWORD sta_dwAreaSpdOutCnt[MAX_AREA_COUNT] = {0};

	static DWORD sta_dwInReSpdCnt[MAX_AREA_COUNT] = {0};
	static DWORD sta_dwOutReSpdCnt[MAX_AREA_COUNT] = {0};
	static DWORD sta_dwTimeSpdCnt[MAX_TIMESPD_COUNT] = {0};

	bool bMeetAreaSpdArea[MAX_AREA_COUNT] = {false};
	bool bMeetAreaSpdTime[MAX_TIME_COUNT] = {false};

	bool bret;
	byte AttrRes;

	int  i=0, j=0;
	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS(objGps, false);

	if ( 'V' == objGps.valid )
		return;
//	PRTMSG(MSG_DBG, "Current Spd:%d\n", (byte)(3.6*objGps.speed));
	// 把当前时间转成字符串格式，以便于比较
	char szCurTime[3] = {0};
	szCurTime[0] = (char)objGps.nHour;
	szCurTime[1] = (char)objGps.nMinute;
	szCurTime[2] = (char)objGps.nSecond;
	tag2QAreaSpdRptCfg  objAreaSpdRptCfg;
	GetSecCfg((void*)&objAreaSpdRptCfg, sizeof(objAreaSpdRptCfg), offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg));
	//分时段区域超速
	for (i=0; i<MAX_AREA_COUNT; i++)
	{
		if ( objAreaSpdRptCfg.m_objAreaSpd[i].m_bEnable )
		{
			if (objAreaSpdRptCfg.m_objAreaSpd[i].m_bytSpeed + 1 < (byte)(3.6*objGps.speed))
			{
				// 先判断是否符合区域范围条件
				bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
									objAreaSpdRptCfg.m_objAreaSpd[i].m_lMinLon,
									objAreaSpdRptCfg.m_objAreaSpd[i].m_lMinLat, 
									objAreaSpdRptCfg.m_objAreaSpd[i].m_lMaxLon,
									objAreaSpdRptCfg.m_objAreaSpd[i].m_lMaxLat, 0x02, &AttrRes);
				if ( 0xff != AttrRes)
				{
					if ( bret)
					{ //当前在区域中	
						if ( sta_dwAreaSpdOutCnt[i] > 0 )
						{
							sta_dwAreaSpdOutCnt[i]--;
						}
						sta_dwAreaSpdInCnt[i]++;                
					} else
					{ //当前在区域外
						if ( sta_dwAreaSpdInCnt[i] > 0 )
						{
							sta_dwAreaSpdInCnt[i]--;
						}
						sta_dwAreaSpdOutCnt[i]++;
					}
				} else
				{
					sta_dwAreaSpdInCnt[i] = 0;      
					sta_dwAreaSpdOutCnt[i] = 0;
				}

				if ( sta_dwAreaSpdInCnt[i] >= (SPD_CONTCOUNT +  objAreaSpdRptCfg.m_objAreaSpd[i].m_usTimeval))
				{
					sta_dwAreaSpdInCnt[i] = 0;
					if ( 0x03 == objAreaSpdRptCfg.m_objAreaSpd[i].m_bytAreaAttr)
					{
						bMeetAreaSpdArea[i] = true;
					}
				}
				if ( sta_dwAreaSpdOutCnt[i] >= (SPD_CONTCOUNT +  objAreaSpdRptCfg.m_objAreaSpd[i].m_usTimeval) )
				{
					sta_dwAreaSpdOutCnt[i] = 0;
					if ( 0x04 == objAreaSpdRptCfg.m_objAreaSpd[i].m_bytAreaAttr)
					{
						bMeetAreaSpdArea[i] = true;
					}
				}

				// 默认先把时间段符合条件值置为真
				bMeetAreaSpdTime[i] = true;

				// 若有时间段范围条件，则再把条件值设为假
				for (j=0; j < MAX_TIME_COUNT; j++)
				{
					if ( objAreaSpdRptCfg.m_objAreaSpd[i].m_bTimeEnable[j] )
					{
						bMeetAreaSpdTime[i] = false;
						break;
					}
				}

				// 再判断当前时间是否落在时间段范围条件内
				if ( bMeetAreaSpdTime[i] == false )
				{
					for (j=0; j < MAX_TIME_COUNT; j++)
					{
						if ( objAreaSpdRptCfg.m_objAreaSpd[i].m_bTimeEnable[j] )
						{
							if ( strncmp(szCurTime, (char *)objAreaSpdRptCfg.m_objAreaSpd[i].m_szBeginTime[j], 3) >= 0
								 && strncmp(szCurTime, (char *)objAreaSpdRptCfg.m_objAreaSpd[i].m_szEndTime[j], 3) <= 0 )
							{
								bMeetAreaSpdTime[i] = true;
								break;
							}
						}
					}
				}

				//若区域和时间范围都符合，则上报一条信息
				if ( bMeetAreaSpdArea[i] && bMeetAreaSpdTime[i] )
				{
					PRTMSG(MSG_DBG, "Time Region over speed alert!!!!\n");
					SetAlert(true, ALERT_TYPE_TIMERESPD, i);
				}
			} else
			{
				if ( sta_dwAreaSpdInCnt[i] > 0 )
				{
					sta_dwAreaSpdInCnt[i]--;
				}
				if ( sta_dwAreaSpdOutCnt[i] > 0 )
				{
					sta_dwAreaSpdOutCnt[i]--;
				}
			}
		}
	}
//区域超速
	for (i=0; i<MAX_AREA_COUNT; i++)
	{
		if ( objAreaSpdRptCfg.m_objInRegionSpd[i].m_bEnable)
		{
			if (objAreaSpdRptCfg.m_objInRegionSpd[i].m_bytSpeed + 1 < (byte)(3.6*objGps.speed) )
			{
				// 先判断是否符合区域范围条件
				bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
									objAreaSpdRptCfg.m_objInRegionSpd[i].m_lMinLon,
									objAreaSpdRptCfg.m_objInRegionSpd[i].m_lMinLat, 
									objAreaSpdRptCfg.m_objInRegionSpd[i].m_lMaxLon,
									objAreaSpdRptCfg.m_objInRegionSpd[i].m_lMaxLat, 0x02, &AttrRes);
				if ( 0xff != AttrRes)
				{
					if ( bret)
					{ //当前在区域中	
						sta_dwInReSpdCnt[i]++;              
					}
				} else
				{
					if ( sta_dwInReSpdCnt[i] > 0 )
					{
						sta_dwInReSpdCnt[i]--;
					}
				}

			} else
			{
				if ( sta_dwInReSpdCnt[i] > 0 )
				{
					sta_dwInReSpdCnt[i]--;
				}
			}
			if ( sta_dwInReSpdCnt[i] >= (SPD_CONTCOUNT + objAreaSpdRptCfg.m_objInRegionSpd[i].m_usTimeval) )
			{
				sta_dwInReSpdCnt[i] = 0;
				PRTMSG(MSG_DBG, "In Region over speed alert!!!!\n");
				SetAlert(true, ALERT_TYPE_INRESPD, i);
				break;
			}
		}

		if ( objAreaSpdRptCfg.m_objOutRegionSpd[i].m_bEnable)
		{
			if ( objAreaSpdRptCfg.m_objOutRegionSpd[i].m_bytSpeed + 1 < (byte)(3.6*objGps.speed) )
			{
				// 先判断是否符合区域范围条件
				bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
									objAreaSpdRptCfg.m_objOutRegionSpd[i].m_lMinLon,
									objAreaSpdRptCfg.m_objOutRegionSpd[i].m_lMinLat, 
									objAreaSpdRptCfg.m_objOutRegionSpd[i].m_lMaxLon,
									objAreaSpdRptCfg.m_objOutRegionSpd[i].m_lMaxLat, 0x02, &AttrRes);
				if ( 0xff != AttrRes)
				{
					if ( !bret )
					{ //当前在区域外
						sta_dwOutReSpdCnt[i]++;
					}
				} else
				{
					if ( sta_dwOutReSpdCnt[i] > 0 )
					{
						sta_dwOutReSpdCnt[i]--;
					}
				}

				if ( sta_dwOutReSpdCnt[i] >= (SPD_CONTCOUNT + objAreaSpdRptCfg.m_objOutRegionSpd[i].m_usTimeval))
				{
					sta_dwOutReSpdCnt[i] = 0;
					PRTMSG(MSG_DBG, "Out Region over speed alert!!!!\n");
					SetAlert(true, ALERT_TYPE_OUTRESPD, i);
					break;
				}
			} else
			{
				if ( sta_dwOutReSpdCnt[i] > 0 )
				{
					sta_dwOutReSpdCnt[i]--;
				}
			}
		}
	}
//时间段超速
	i = 0;
	if ( objAreaSpdRptCfg.m_TimeSpd[i].m_bEnable)
	{
		if (objAreaSpdRptCfg.m_TimeSpd[i].m_bytSpeed + 1 < (byte)(3.6*objGps.speed) )
		{
			for (j = 0; j < MAX_TIME_COUNT; j++)
			{
				if ( objAreaSpdRptCfg.m_TimeSpd[i].m_bTimeEnable[j] )
				{
					if ( strncmp(szCurTime, (char *)objAreaSpdRptCfg.m_TimeSpd[i].m_szBeginTime[j], 3) >= 0
						 && strncmp(szCurTime, (char *)objAreaSpdRptCfg.m_TimeSpd[i].m_szEndTime[j], 3) <= 0 )
					{
						sta_dwTimeSpdCnt[i]++;                      
						break;
					}
				}
			}
		} else
		{
			if ( sta_dwTimeSpdCnt[i] > 0 )
			{
				sta_dwTimeSpdCnt[i]--;
			}
		}

	}
	if ( sta_dwTimeSpdCnt[i] >= (SPD_CONTCOUNT + objAreaSpdRptCfg.m_TimeSpd[i].m_usTimeval))
	{
		sta_dwTimeSpdCnt[i] = 0;    
		PRTMSG(MSG_DBG, "Time over speed alert!!!!\n");
		SetAlert(true, ALERT_TYPE_TIMESPD, i);
	}
}


bool CLN_Report::SpeedEnCheck(tag2QAreaSpdRptCfg *objAreaSpdRptCfg)
{
	int i;
	for ( i = 0; i < MAX_AREA_COUNT; i++ )
	{
		if (true == objAreaSpdRptCfg->m_objAreaSpd[i].m_bEnable || true == objAreaSpdRptCfg->m_objInRegionSpd[i].m_bEnable || true == objAreaSpdRptCfg->m_objOutRegionSpd[i].m_bEnable)
		{
			return true;
		}
	}
	for ( i = 0; i < MAX_TIMESPD_COUNT; i++ )
	{
		if (true == objAreaSpdRptCfg->m_TimeSpd[i].m_bEnable)
		{
			return true;
		}
	}
	return false;
}

bool CLN_Report::ParkTimeEnCheck(tag2QAreaParkTimeRptCfg *objAreaParkTimeRptCfg)
{
	int i;
	for ( i = 0; i < MAX_AREA_COUNT; i++ )
	{
		if (true == objAreaParkTimeRptCfg->m_objAreaParkTime[i].m_bEnable || true == objAreaParkTimeRptCfg->m_objInRegionParkTime[i].m_bEnable || true == objAreaParkTimeRptCfg->m_objOutRegionParkTime
			[i].m_bEnable)
		{
			return true;
		}
	}
	if ( objAreaParkTimeRptCfg->m_objParkTime.m_bEnable )
	{
		return true;
	}
	return false;
}

int CLN_Report::BeginParkTimeRptCheck()
{
	int iret;
	tag2QAreaParkTimeRptCfg objAreaParkTimeRptCfg;
	iret = GetSecCfg((void*)&objAreaParkTimeRptCfg, sizeof(objAreaParkTimeRptCfg), offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg));
	if ( 0 != iret || false == ParkTimeEnCheck(&objAreaParkTimeRptCfg))
	{
		return ERR_CFG;
	}
	PRTMSG(MSG_DBG, "Start park time check!\n");
	_KillTimer(&g_objTimerMng, PARKTIME_CHECK_TIMER);
	_SetTimer(&g_objTimerMng, PARKTIME_CHECK_TIMER, 2000, G_TmParkTimeRptCheck);

}

int CLN_Report::EndParkTimeRptCheck()
{
	_KillTimer(&g_objTimerMng, PARKTIME_CHECK_TIMER);
}

void CLN_Report::P_TmParkTimeRptCheck()
{   
	static DWORD sta_dwAreaParkInCnt[MAX_AREA_COUNT] = {0};
	static DWORD sta_dwAreaParkOutCnt[MAX_AREA_COUNT] = {0};
	bool bMeetAreaParkTime[MAX_AREA_COUNT] = {false};

	static DWORD sta_dwInReParkCnt[MAX_AREA_COUNT] = {0};
	static DWORD sta_dwOutReParkCnt[MAX_AREA_COUNT] = {0};
	static DWORD SilentCount = 0;
	bool bret;
	byte AttrRes;

	int  i=0;
	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS(objGps, false);

	if ( 'V' == objGps.valid )
		return;

	// 把当前时间转成字符串格式，以便于比较
	char szCurTime[3] = {0};
	szCurTime[0] = (char)objGps.nHour;
	szCurTime[1] = (char)objGps.nMinute;
	szCurTime[2] = (char)objGps.nSecond;

	if ( 1 == objGps.m_bytMoveType )
	{
		SilentCount++;
	} else
	{
		SilentCount = 0;
	}
	if ( SilentCount >= 5 )
	{//10s移动才更新
		SilentCount = 0;
		m_dwParkTime = GetTickCount();
		for (i = 0; i < MAX_AREA_COUNT; i++)
		{
			m_dwAreaParkTime[i] = GetTickCount();
			m_dwInRegPatkTime[i] = GetTickCount();
			m_dwOutRegPatkTime[i] = GetTickCount();
		}
	}
	tag2QAreaParkTimeRptCfg objAreaParkTimeRptCfg;
	GetSecCfg((void*)&objAreaParkTimeRptCfg, sizeof(objAreaParkTimeRptCfg), offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg));
	//区域停车超时
	for (i = 0; i < MAX_AREA_COUNT; i++)
	{
		if ( objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_bEnable )
		{
			// 先判断是否符合区域范围条件
			bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
								objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_lMinLon,
								objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_lMinLat, 
								objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_lMaxLon,
								objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_lMaxLat, 0x02, &AttrRes);
//			PRTMSG(MSG_DBG, "curlon:%d, curlat:%d, letflon:%d, letflat:%d, rightlon:%d, rightlat:%d bret:%d parktime:%d\n", long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_lMinLon, objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_lMaxLat, objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_lMaxLon, objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_lMinLat, bret, (GetTickCount() - m_dwParkTime)/(1000 * 60));
			if ( 0xff != AttrRes && (GetTickCount() - m_dwAreaParkTime[i])/(1000 * 60) >= objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_usParkTime)
			{
				if ( bret)
				{ //当前在区域中	
					if ( sta_dwAreaParkOutCnt[i] > 0 )
					{
						sta_dwAreaParkOutCnt[i]--;
					}
					sta_dwAreaParkInCnt[i]++;               
				} else
				{ //当前在区域外
					if ( sta_dwAreaParkInCnt[i] > 0 )
					{
						sta_dwAreaParkInCnt[i]--;
					}
					sta_dwAreaParkOutCnt[i]++;
				}
			} else
			{
				if ( sta_dwAreaParkInCnt[i] > 0 )
				{
					sta_dwAreaParkInCnt[i]--;
				}
				if ( sta_dwAreaParkOutCnt[i] > 0 )
				{
					sta_dwAreaParkOutCnt[i]--;
				}
			}

			if ( sta_dwAreaParkInCnt[i] >= POS_CONTCOUNT )
			{
				sta_dwAreaParkInCnt[i] = 0;
				if ( 0x03 == objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_bytAreaAttr)
				{
					bMeetAreaParkTime[i] = true;
				}
			}
			if ( sta_dwAreaParkOutCnt[i] >= POS_CONTCOUNT )
			{
				sta_dwAreaParkOutCnt[i] = 0;
				if ( 0x04 == objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_bytAreaAttr)
				{
					bMeetAreaParkTime[i] = true;
				}
			}

			//若区域范围符合，则上报一条信息
			if ( bMeetAreaParkTime[i])
			{
				PRTMSG(MSG_DBG, "Region park over time alert!!!!\n");
				m_dwAreaParkTime[i] = GetTickCount();
				SetAlert(true, ALERT_TYPE_REPATIMEOUT, i);
			}
		}
	}
//区域超时停车
	for (i = 0; i < MAX_AREA_COUNT; i++)
	{
		if ( objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_bEnable)
		{
			// 先判断是否符合区域范围条件
			bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
								objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_lMinLon,
								objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_lMinLat, 
								objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_lMaxLon,
								objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_lMaxLat, 0x02, &AttrRes);
			if ( 0xff != AttrRes && (GetTickCount() - m_dwInRegPatkTime[i])/(1000 * 60) >= objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_usParkTime)
			{
				if ( bret)
				{ //当前在区域中	
					sta_dwInReParkCnt[i]++;             
				}
			} else
			{
				if ( sta_dwInReParkCnt[i] > 0 )
				{
					sta_dwInReParkCnt[i]--;
				}
			}

			if ( sta_dwInReParkCnt[i] >= POS_CONTCOUNT )
			{
				sta_dwInReParkCnt[i] = 0;
				PRTMSG(MSG_DBG, "In Region park over time alert!!!!\n");
				m_dwInRegPatkTime[i] = GetTickCount();

				SetAlert(true, ALERT_TYPE_INREPATOUT, i);
				break;
			}
		}

		if ( objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_bEnable)
		{
			// 先判断是否符合区域范围条件
			bret = JugPtInShape(long(objGps.longitude * 3600 * 1000), long(objGps.latitude * 3600 * 1000), 
								objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_lMinLon,
								objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_lMinLat, 
								objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_lMaxLon,
								objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_lMaxLat, 0x02, &AttrRes);
			if ( 0xff != AttrRes && (GetTickCount() - m_dwOutRegPatkTime[i]) / (1000 * 60) >= objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_usParkTime)
			{
				if ( !bret )
				{ //当前在区域外
					sta_dwOutReParkCnt[i]++;
				}
			} else
			{
				if ( sta_dwOutReParkCnt[i] > 0 )
				{
					sta_dwOutReParkCnt[i]--;
				}
			}

			if ( sta_dwOutReParkCnt[i] >= POS_CONTCOUNT )
			{
				sta_dwOutReParkCnt[i] = 0;
				PRTMSG(MSG_DBG, "Out Region park over time alert!!!!\n");
				m_dwOutRegPatkTime[i] = GetTickCount();
				SetAlert(true, ALERT_TYPE_OUTREPATOUT, i);
				break;
			}
		}
	}

	if ( objAreaParkTimeRptCfg.m_objParkTime.m_bEnable && (GetTickCount() - m_dwParkTime) / (1000 * 60) > objAreaParkTimeRptCfg.m_objParkTime.m_usParkTime)
	{
		PRTMSG(MSG_DBG, "Park over time alert!!!!\n");
		m_dwParkTime = GetTickCount();
		SetAlert(true, ALERT_TYPE_PARKTIMEOUT, 0);
	}

}

int CLN_Report::BeginOutLineRptCheck()
{
	int iret;
	tag2QLineRptCfg     objLineRptCfg;
	iret = GetSecCfg((void*)&objLineRptCfg, sizeof(objLineRptCfg), offsetof(tagSecondCfg, m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg));
	if ( 0 != iret || false == OutLineEnCheck(&objLineRptCfg))
	{
		return ERR_CFG;
	}
	PRTMSG(MSG_DBG, "Start out line check!\n");
	_KillTimer(&g_objTimerMng, OUTLINE_CHECK_TIMER);
	_SetTimer(&g_objTimerMng, OUTLINE_CHECK_TIMER, 2000, G_TmOutLineRptCheck);
}

int CLN_Report::EndOutLineRptCheck()
{
	_KillTimer(&g_objTimerMng, OUTLINE_CHECK_TIMER);
}

void CLN_Report::P_TmOutLineRptCheck()
{
	bool bret, outline = false;
	static DWORD OutLineCnt = 0;
	int  i=0, j=0;
	double len;
	ushort result;
	byte AttrRes;
	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS(objGps, false);
//	PRTMSG(MSG_DBG, "Out line checking!\n");
	if ( 'V' == objGps.valid )
//		PRTMSG(MSG_DBG, "GPS value V!!\n");
		return;
	char szCurTime[3] = {0};
	szCurTime[0] = (char)objGps.nHour;
	szCurTime[1] = (char)objGps.nMinute;
	szCurTime[2] = (char)objGps.nSecond;

	tag2QLineRptCfg     objLineRptCfg;
	GetSecCfg((void*)&objLineRptCfg, sizeof(objLineRptCfg), offsetof(tagSecondCfg, m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg));
//偏航报警

	for (i = 0; i < MAX_LINE_COUNT; i++)
	{
		if ( !objLineRptCfg.m_objLine[i].m_bEnable )
			continue;
		outline = true;
		for ( j = 0; j < MAX_POINT_COUNT - 1; j++ )
		{
			if ( !objLineRptCfg.m_objLine[i].m_bPointEnable[j] || !objLineRptCfg.m_objLine[i].m_bPointEnable[j + 1])
			{
				break;
			}
//			PRTMSG(MSG_DBG, "lon1 = %d!\n", (objLineRptCfg.m_objLine[i].m_lLon[j]));
//			PRTMSG(MSG_DBG, "lat1 = %d!\n", (objLineRptCfg.m_objLine[i].m_lLat[j]));
//			PRTMSG(MSG_DBG, "lon2 = %d!\n", (objLineRptCfg.m_objLine[i].m_lLon[j + 1]));
//			PRTMSG(MSG_DBG, "lat2 = %d!\n", objLineRptCfg.m_objLine[i].m_lLat[j + 1]);
//			PRTMSG(MSG_DBG, "clon = %d!\n", (ulong)(objGps.longitude*3600*1000));
//			PRTMSG(MSG_DBG, "clat = %d!\n", (ulong)(objGps.latitude*3600*1000));
			bret = OutLineLen((double)objLineRptCfg.m_objLine[i].m_lLon[j]/(3600*1000), (double)objLineRptCfg.m_objLine[i].m_lLat[j]/(3600*1000), (double)objLineRptCfg.m_objLine[i].m_lLon[j + 1]/(3600*1000), (double)objLineRptCfg.m_objLine[i].m_lLat[j + 1]/(3600*1000), objGps.longitude, objGps.latitude, &len);             
			if ( !bret )
			{//无效计算点
				continue;
			}
			if ( (ushort)len <= objLineRptCfg.m_objLine[i].m_usOutval )
			{//在某一条航线上
				outline = false;
				goto _OUTLINE_END;                           
			}

		}
	}
	_OUTLINE_END:
	if ( outline )
	{
		OutLineCnt++;
	} else
	{
		if ( OutLineCnt > 0 )
		{
			OutLineCnt--;
		}
	}
	if ( OutLineCnt >= 5)
	{
		OutLineCnt = 0;
		PRTMSG(MSG_DBG, "Out line %dm Alert!!!\n", (ushort)len);
		SetAlert(true, ALERT_TYPE_OUTLINE, 0);
	}
	outline = false;
	bool intime;
	static DWORD TimeLineCnt = 0;
	intime = false;
//分时段偏航报警
	for (i = 0; i < MAX_LINE_COUNT; i++)
	{
		if ( !objLineRptCfg.m_objTimeLine[i].m_bEnable )
			continue;
		for (j = 0; j < MAX_TIME_COUNT; j++)
		{
			if ( objLineRptCfg.m_objTimeLine[i].m_bTimeEnable[j] )
			{
				if ( strncmp(szCurTime, (char *)objLineRptCfg.m_objTimeLine[i].m_szBeginTime[j], 3) >= 0
					 && strncmp(szCurTime, (char *)objLineRptCfg.m_objTimeLine[i].m_szEndTime[j], 3) <= 0 )
				{
					intime = true;                  
					break;
				}
			}
		}
		if ( !intime )
		{
			continue;
		}
		outline = true;
		for ( j = 0; j < MAX_POINT_COUNT - 1; j++ )
		{
			if ( !objLineRptCfg.m_objTimeLine[i].m_bPointEnable[j] || !objLineRptCfg.m_objTimeLine[i].m_bPointEnable[j + 1])
			{
				break;
			}
			bret = OutLineLen((double)objLineRptCfg.m_objTimeLine[i].m_lLon[j]/(3600*1000), (double)objLineRptCfg.m_objTimeLine[i].m_lLat[j]/(3600*1000), (double)objLineRptCfg.m_objTimeLine[i].m_lLon[j + 1]/(3600*1000), (double)objLineRptCfg.m_objTimeLine[i].m_lLat[j + 1]/(3600*1000), objGps.longitude, objGps.latitude, &len); 
			if ( !bret )
			{
				continue;
			}
			if ( (ushort)len <= objLineRptCfg.m_objTimeLine[i].m_usOutval )
			{
				outline = false;
				goto _TIMEOUTLINE_END;                           
			}

		}
	}
	_TIMEOUTLINE_END:
	if ( outline )
	{
		TimeLineCnt++;
	} else
	{
		if ( TimeLineCnt > 0 )
		{
			TimeLineCnt--;
		}
	}
	if ( TimeLineCnt >= 5)
	{
		TimeLineCnt = 0;
		PRTMSG(MSG_DBG, "Time Out line %dm Alert!!!\n", (ushort)len);
		SetAlert(true, ALERT_TYPE_TIMEOUTLINE, 0);
	}

}

bool CLN_Report::OutLineEnCheck(tag2QLineRptCfg *objLineRptCfg)
{
	int i;
	for ( i = 0; i < MAX_LINE_COUNT; i++ )
	{
		if (true == objLineRptCfg->m_objLine[i].m_bEnable || true == objLineRptCfg->m_objTimeLine[i].m_bEnable)
		{
			return true;
		}
	}
	return false;
}

int CLN_Report::BeginTireRptCheck()
{
	int iret;
	tag2QReportCfg objReportCfg;
	GetSecCfg((void *)&m_objAlertCfg, sizeof(m_objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(m_objAlertCfg));

	iret = GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg)); 
	if (  0 != iret || m_objAlertCfg.m_usRestTime == 0 || false == objReportCfg.m_objTirePrtPar.m_bEnable)
	{
		return ERR_CFG;
	}
	PRTMSG(MSG_DBG, "Start tire check!\n");
	_KillTimer(&g_objTimerMng, TIRE_CHECK_TIMER);
	_SetTimer(&g_objTimerMng, TIRE_CHECK_TIMER, 1000, G_TmTireRptCheck);
}

int CLN_Report::EndTireRptCheck()
{
	_KillTimer(&g_objTimerMng, TIRE_CHECK_TIMER);
}

void CLN_Report::P_TmTireRptCheck()
{   
	static DWORD SilentCount = 0, MoveCount = 0;
	static bool run = false;
	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS(objGps, false);
	tag2QReportCfg objReportCfg;
	GetSecCfg((void *)&m_objAlertCfg, sizeof(m_objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(m_objAlertCfg));
	if ( 'V' == objGps.valid)
		return;
	if ( false == objReportCfg.m_objTirePrtPar.m_bEnable )
	{
		m_dwDriverTime = 0;
		m_dwRestTime = 0;
		return;
	}
	if ( 1 == objGps.m_bytMoveType )
	{
		SilentCount = 0;
		MoveCount++;
	} else
	{
		MoveCount = 0;
		SilentCount++;
	}
	if (SilentCount >= 5)
	{
		run = false;
	}
	if ( MoveCount >= 5 )
	{
		run = true;
	}

	if ( run && 1 == m_accsta )
	{//行驶
		m_dwDriverTime++;
		if ( (ushort)(m_dwDriverTime / 3600) >= m_objAlertCfg.m_usDriveTime )
		{
			m_dwDriverTime = 0;
			goto    _TIRE_ALERT;
		}
		if ( m_dwRestTime > 5 )
		{
			//		PRTMSG(MSG_DBG, "Rest %d min this time!\n", (m_dwRestTime / 60));
			if ((m_dwRestTime / 60) < 20 || (ushort)(m_dwRestTime / 60) <  m_objAlertCfg.m_usRestTime)
			{
				m_dwRestTime = 0;
				goto    _TIRE_ALERT;
			}
			m_dwDriverTime = 0;
		}
		m_dwRestTime = 0;
		return;
	} else if ( !run && 0 == m_accsta )
	{
		if ( m_dwDriverTime > 5 )
		{
			//		PRTMSG(MSG_DBG, "Drive %d min this time!\n", (m_dwDriverTime / 60));
			m_dwRestTime++;
		} else
		{
			m_dwDriverTime = 0;
			m_dwRestTime = 0;
			run = false;
		}
		return;
	}
	return;
	_TIRE_ALERT:
//	play_tts("司机朋友您已疲劳行驶,请注意休息!");
	// 向调度屏也提示
//	char tmpbuf[] = {"司机朋友您已疲劳行驶,请注意休息!"};
//	char buf[100] = {0};					
//	buf[0] = 0x01;
//	buf[1] = 0x72;
//	buf[2] = 0x01;
//	memcpy(buf+3, tmpbuf, sizeof(tmpbuf));					
//	DataPush(buf, sizeof(tmpbuf)+3, DEV_QIAN, DEV_DIAODU, LV2);
	PRTMSG(MSG_DBG, "Tire alert!!!!\n");
	SetAlert(true, ALERT_TYPE_TIRE, 0);
}

int CLN_Report::BeginMileRptCheck()
{
	int iret;
	tag2QReportCfg objReportCfg;
	GetSecCfg((void *)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	iret = GetSecCfg((void*)&m_objAlertCfg, sizeof(m_objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(m_objAlertCfg));
	if ( 0 != iret || false == objReportCfg.m_objMilePrtPar.m_bEnable )
	{
		return ERR_CFG;
	}
	_KillTimer(&g_objTimerMng, MILE_CHECK_TIMER);
	PRTMSG(MSG_DBG, "Start Mile check!\n");
	_SetTimer(&g_objTimerMng, MILE_CHECK_TIMER, 1000, G_TmMileRptCheck);
}

int CLN_Report::EndMileRptCheck()
{
	_KillTimer(&g_objTimerMng, MILE_CHECK_TIMER);
}

void CLN_Report::P_TmMileRptCheck()
{
	tag2QReportCfg objReportCfg;
	GetSecCfg((void *)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	if ( m_objAlertCfg.m_usTotalMile < g_objDriveRecord.TotalMeter && objReportCfg.m_objMilePrtPar.m_bEnable )
	{
		PRTMSG(MSG_DBG, "Mileage alert!!!!\n");
		SetAlert(true, ALERT_TYPE_MILE, 0);
	}
}

// 本地类型转换成上报的类型
bool CLN_Report::LocalTypeToReportType(DWORD v_dwLocalType, short *v_shReportType)
{
	char buf[2] = {0};

	if ( v_dwLocalType == ALERT_TYPE_FOOT )
	{
		buf[0] = 0x00; buf[1] = 0x01;
	} else if ( v_dwLocalType == ALERT_TYPE_MILE )
	{
		buf[0] = 0x00; buf[1] = 0x02;
	} else if ( v_dwLocalType == ALERT_TYPE_FRONTDOOR )
	{
		buf[0] = 0x00; buf[1] = 0x03;
	} else if ( v_dwLocalType == ALERT_TYPE_PARKTIMEOUT )
	{
		buf[0] = 0x00; buf[1] = 0x05;
	} else if ( v_dwLocalType == ALERT_TYPE_AREA )
	{
		buf[0] = 0x00; buf[1] = 0x06;
	} else if ( v_dwLocalType == ALERT_TYPE_OVERSPEED )
	{
		buf[0] = 0x00; buf[1] = 0x07;
	} else if ( v_dwLocalType == ALERT_TYPE_BELOWSPEED )
	{
		buf[0] = 0x00; buf[1] = 0x07;
	} else if ( v_dwLocalType == ALERT_TYPE_TIMESPD )
	{
		buf[0] = 0x00; buf[1] = 0x08;
	} else if ( v_dwLocalType == ALERT_TYPE_OUTLINE )
	{
		buf[0] = 0x00; buf[1] = 0x09;
	} else if ( v_dwLocalType == ALERT_TYPE_POWOFF )
	{
		buf[0] = 0x00; buf[1] = 0x0e;
	} else if ( v_dwLocalType == ALERT_TYPE_STARTENG )
	{
		buf[0] = 0x00; buf[1] = 0x0f;
	} else if ( v_dwLocalType == ALERT_TYPE_TIRE )
	{
		buf[0] = 0x00; buf[1] = 0x14;
	} else if ( v_dwLocalType == ALERT_TYPE_TIMERESPD )
	{
		buf[0] = 0x00; buf[1] = 0x16;
	} else if ( v_dwLocalType == ALERT_TYPE_TIMEOUTLINE )
	{
		buf[0] = 0x00; buf[1] = 0x18;
	} else if ( v_dwLocalType == ALERT_TYPE_REPATIMEOUT )
	{
		buf[0] = 0x00; buf[1] = 0x19;
	} else if ( v_dwLocalType == ALERT_TYPE_INAREA )
	{
		buf[0] = 0x00; buf[1] = 0x22;
	} else if ( v_dwLocalType == ALERT_TYPE_OUTAREA )
	{
		buf[0] = 0x00; buf[1] = 0x23;
	} else if ( v_dwLocalType == ALERT_TYPE_INREGION )
	{
		buf[0] = 0x00; buf[1] = 0x24;
	} else if ( v_dwLocalType == ALERT_TYPE_OUTREGION )
	{
		buf[0] = 0x00; buf[1] = 0x25;
	} else if ( v_dwLocalType == ALERT_TYPE_INRESPD )
	{
		buf[0] = 0x00; buf[1] = 0x26;
	} else if ( v_dwLocalType == ALERT_TYPE_OUTRESPD )
	{
		buf[0] = 0x00; buf[1] = 0x27;
	} else if ( v_dwLocalType == ALERT_TYPE_INREPATOUT )
	{
		buf[0] = 0x00; buf[1] = 0x28;
	} else if ( v_dwLocalType == ALERT_TYPE_OUTREPATOUT )
	{
		buf[0] = 0x00; buf[1] = 0x29;
	} else
		return false;   

	memcpy((void*)v_shReportType, buf, 2);
	return true;
}

// 上报类型转成本地类型
bool CLN_Report::ReportTyepToLocalType(short v_shReportType, DWORD &v_dwLocalType)
{
	if ( v_shReportType == 0x0001 )
	{
		v_dwLocalType = ALERT_TYPE_FOOT;
	} else if ( v_shReportType == 0x0002)
	{
		v_dwLocalType = ALERT_TYPE_MILE;
	} else if ( v_shReportType == 0x0003)
	{
		v_dwLocalType = ALERT_TYPE_FRONTDOOR;
	} else if ( v_shReportType == 0x0005)
	{
		v_dwLocalType = ALERT_TYPE_PARKTIMEOUT;
	} else if ( v_shReportType == 0x0006)
	{
		v_dwLocalType = ALERT_TYPE_AREA;
	} else if ( v_shReportType == 0x0007)
	{
		v_dwLocalType = ALERT_TYPE_OVERSPEED | ALERT_TYPE_BELOWSPEED;
	} else if ( v_shReportType == 0x0008)
	{
		v_dwLocalType = ALERT_TYPE_TIMESPD;
	} else if ( v_shReportType == 0x0009)
	{
		v_dwLocalType = ALERT_TYPE_OUTLINE;
	} else if ( v_shReportType == 0x000e)
	{
		v_dwLocalType = ALERT_TYPE_POWOFF;
	} else if ( v_shReportType == 0x000f )
	{
		v_dwLocalType = ALERT_TYPE_STARTENG;
	} else if ( v_shReportType == 0x0014)
	{
		v_dwLocalType = ALERT_TYPE_TIRE;
	} else if ( v_shReportType == 0x0016)
	{
		v_dwLocalType = ALERT_TYPE_TIMERESPD;
	} else if ( v_shReportType == 0x0018)
	{
		v_dwLocalType = ALERT_TYPE_TIMEOUTLINE;
	} else if ( v_shReportType == 0x0019)
	{
		v_dwLocalType = ALERT_TYPE_REPATIMEOUT;
	} else if ( v_shReportType == 0x0022)
	{
		v_dwLocalType = ALERT_TYPE_INAREA;
	} else if ( v_shReportType == 0x0023)
	{
		v_dwLocalType = ALERT_TYPE_OUTAREA;
	} else if ( v_shReportType == 0x0024)
	{
		v_dwLocalType = ALERT_TYPE_INREGION;
	} else if ( v_shReportType == 0x0025)
	{
		v_dwLocalType = ALERT_TYPE_OUTREGION;
	} else if ( v_shReportType == 0x0026)
	{
		v_dwLocalType = ALERT_TYPE_INRESPD;
	} else if ( v_shReportType == 0x0027)
	{
		v_dwLocalType = ALERT_TYPE_OUTRESPD;
	} else if ( v_shReportType == 0x0028)
	{
		v_dwLocalType = ALERT_TYPE_INREPATOUT;
	} else if ( v_shReportType == 0x0029)
	{
		v_dwLocalType = ALERT_TYPE_OUTREPATOUT;
	} else
		return false;

	return true;
}

#endif















