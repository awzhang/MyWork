#include "yx_QianStdAfx.h"
#include "yx_AutoRpt.h"

#if USE_AUTORPT == 1

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-AutoRpt  ")

void G_TmAutoRpt(void *arg, int len)
{
	g_objAutoRpt.P_TmAutoRpt();
}

CAutoRpt::CAutoRpt()
{

}

CAutoRpt::~CAutoRpt()
{

}

void CAutoRpt::Init()
{
	// 判断是否要启动主动上报
//	PRTMSG(MSG_NOR, "Init AutoRpt\n" );

	DWORD dwVeSta = g_objCarSta.GetVeSta(); // 所以要求在CIO类对象初始化之后调用

	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS( objGps, false );
	ENU_RPTJUGRESULT enuResult = _JugCanRpt( objGps );
	if( RPT_YES != enuResult )
	{
		//PRTMSG(MSG_NOR, "can not Rpt!\n");
		EndAutoRpt();
		return;
	}

#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	// 移动/静止上报
	if( dwVeSta & VESTA_MOVE )
		BeginAutoRpt( 1 ); // 移动
	else // 将GPS无效状态也视为静止状态上报	if ( 2 == gps.m_bytMoveType )
		BeginAutoRpt( 2 ); // 静止

	// 空车/重车上报
	
	if( dwVeSta & VESTA_HEAVY )
		BeginAutoRpt(4); // 重车
	else
		BeginAutoRpt(3); // 空车
#endif

#if 2 == USE_PROTOCOL
	if( dwVeSta & VESTA_MOVE )
	{
		if( dwVeSta & VESTA_HEAVY )
			BeginAutoRpt( 1 ); // 移动重车
		else
			BeginAutoRpt( 3 ); // 移动空车
	}
	else
	{
		if( dwVeSta & VESTA_HEAVY )
			BeginAutoRpt( 4 ); // 静止重车
		else
			BeginAutoRpt( 2 ); // 静止空车
	}
#endif
}

void CAutoRpt::BeginAutoRpt( DWORD v_dwNewSta )
{
	// 若正在监控，不主动上报
	if( g_objMonAlert.IsMonAlert() )
	{
		PRTMSG(MSG_DBG, "Now is Monitoring, AutoRpt quit\n");
		return;
	}
	// 读取配置
	tag2QGprsCfg objGprsCfg;
	objGprsCfg.Init();
	GetSecCfg(&objGprsCfg, sizeof(objGprsCfg), offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg));

	tagMonStatus objAutoRptSta;
	tag2QGpsUpPar objCfgPar;
	BYTE bytMonType;

	// 若不要主动上报
	if( 1 != objGprsCfg.m_bytGpsUpType )
	{
		EndAutoRpt();
		return;
	}

	objAutoRptSta.m_dwMonInterv = 0xffffffff;
	bool bModfySta = false; // 是否更新了参数
	for( BYTE byt = 0; byt < objGprsCfg.m_bytGpsUpParCount; byt ++ )
	{
		objCfgPar = objGprsCfg.m_aryGpsUpPar[ byt ];

		// 跳过非该新状态的主动上传参数
		if( v_dwNewSta != objCfgPar.m_bytStaType )
		{
			PRTMSG(MSG_DBG, "this AutoRpt type is ignore!\n");
			continue;
		}

		// 定时定距上报类型
		bytMonType = 0x18 & objCfgPar.m_bytMonType;

		// 定时上报参数
		if( 0 == bytMonType || 0x10 == bytMonType ) // 若有定时上报
		{
			objAutoRptSta.m_bInterv = true;
			if( DWORD(objCfgPar.m_wMonInterval) * 1000 < objAutoRptSta.m_dwMonInterv )
			{
				objAutoRptSta.m_dwMonInterv = DWORD(objCfgPar.m_wMonInterval) * 1000;
				if( objAutoRptSta.m_dwMonInterv < 1000 ) objAutoRptSta.m_dwMonInterv = 1000;
			}
		}
		else
		{
			objAutoRptSta.m_bInterv = false;
		}

		// 定距上报参数
		if( 0x08 == bytMonType || 0x10 == bytMonType ) // 若有定距上报
		{
			objAutoRptSta.m_bSpace = true;
			objAutoRptSta.m_dwSpace = DWORD(objCfgPar.m_wMonSpace) * 10;
			if( objAutoRptSta.m_dwSpace < 10 ) objAutoRptSta.m_dwSpace = 10;
		}
		else
		{
			objAutoRptSta.m_bSpace = false;
		}

		// 上报时间
		if( INVALID_NUM_16 == objCfgPar.m_wMonTime )
			objAutoRptSta.m_dwMonTime = INVALID_NUM_32;
		else
			objAutoRptSta.m_dwMonTime = DWORD(objCfgPar.m_wMonTime) * 60000;

		bModfySta = true;
		break;
	}

	// 上报状态初始化
	if( bModfySta ) // 若更新了主动上报参数
	{
		EndAutoRpt(); // 在最前面调用

		tagGPSData objGps(0);
		g_objMonAlert.GetCurGPS( objGps, true );

		// 其他上报参数设置
		objAutoRptSta.m_dLstLon = objGps.longitude;
		objAutoRptSta.m_dLstLat = objGps.latitude;
		objAutoRptSta.m_dLstSendDis = 0;
		objAutoRptSta.m_dwLstChkTime = objAutoRptSta.m_dwLstSendTime = GetTickCount();
		if( objAutoRptSta.m_bSpace )
			objAutoRptSta.m_dwChkInterv = 1000;
		else
			objAutoRptSta.m_dwChkInterv = objAutoRptSta.m_dwMonInterv;

		// 更新内存中的设置
		m_objAutoRptSta = objAutoRptSta;

		// 打开/重设主动上报定时
		if( objAutoRptSta.m_bInterv || objAutoRptSta.m_bSpace )
		{
			PRTMSG(MSG_NOR, "AutoRpt Begin, Type is %d\n", v_dwNewSta );
			_Snd0154( objGps ); // 第一帧立刻发送
			_SetTimer(&g_objTimerMng,  AUTO_RPT_TIMER, m_objAutoRptSta.m_dwChkInterv, G_TmAutoRpt );
		}
	}
}

int CAutoRpt::EndAutoRpt()
{
	m_objAutoRptSta.m_bInterv = false;
	m_objAutoRptSta.m_bSpace = false;
	_KillTimer(&g_objTimerMng, AUTO_RPT_TIMER );
	
	//PRTMSG(MSG_NOR, "AutoRpt Stop\n" );
	return 0;
}

void CAutoRpt::P_TmAutoRpt()
{
	/// 获得基本数据和信息
	DWORD dwCur = GetTickCount();
	DWORD dwIntervChk = dwCur - m_objAutoRptSta.m_dwLstChkTime; // 距上次定时检查的时间
	tagGPSData objGps(0);
	bool bNeedSend = false;
	int iResult = 0;
	
	// 递减剩余监控时间
	if( INVALID_NUM_32 != m_objAutoRptSta.m_dwMonTime )
	{
		if( m_objAutoRptSta.m_dwMonTime >= dwIntervChk )
			m_objAutoRptSta.m_dwMonTime -= dwIntervChk;
		else
			m_objAutoRptSta.m_dwMonTime = 0;
	}
	
	// 修改上次检查时间,以便后续的监控时间到判断
	m_objAutoRptSta.m_dwLstChkTime = dwCur;
	
	// 获取当前GPS信息
	iResult = g_objMonAlert.GetCurGPS( objGps, true );
	if( iResult ) goto AUTORPTTIMER_END;
	
	/// 判断是否要发送主动上报数据
	if( m_objAutoRptSta.m_bInterv )
	{
		DWORD dwIntervSend = dwCur - m_objAutoRptSta.m_dwLstSendTime; // 距上次发送监控数据的时间
		if( dwIntervSend >= m_objAutoRptSta.m_dwMonInterv ) // 若监控间隔到
		{
			bNeedSend = true;
		}
	}
	if( m_objAutoRptSta.m_bSpace && !bNeedSend )
	{
		m_objAutoRptSta.m_dLstSendDis += G_CalGeoDis2(m_objAutoRptSta.m_dLstLon, m_objAutoRptSta.m_dLstLat,
			objGps.longitude, objGps.latitude);
		m_objAutoRptSta.m_dLstLon = objGps.longitude;
		m_objAutoRptSta.m_dLstLat = objGps.latitude;
		
		if( m_objAutoRptSta.m_dLstSendDis >= double( m_objAutoRptSta.m_dwSpace ) )
		{
			bNeedSend = true;
			m_objAutoRptSta.m_dLstSendDis = 0;
		}
	}
	
	/// 发送主动上报数据
	if( bNeedSend )
	{
		if( !_Snd0154(objGps) )
		{
			// 只要发送了监控数据就更新 (重要)
			m_objAutoRptSta.m_dwLstSendTime = dwCur; // 使得重新判断定时
			m_objAutoRptSta.m_dLstSendDis = 0; // 使得重新判断定距
			m_objAutoRptSta.m_dLstLon = objGps.longitude;
			m_objAutoRptSta.m_dLstLat = objGps.latitude;
		}
	}
	
AUTORPTTIMER_END:
	// 车台终止主动上报判断
	ENU_RPTJUGRESULT enuResult = _JugCanRpt( objGps );
	if( 0 == m_objAutoRptSta.m_dwMonTime || RPT_YES != enuResult )
	{
		EndAutoRpt();
	}
	else
	{
		static BYTE sta_bytCt = 0;
		if( 0 == sta_bytCt ++ % 10 ) // 每10次修正1次定时
		{
			if( m_objAutoRptSta.m_dwChkInterv < 1000 ) m_objAutoRptSta.m_dwChkInterv = 1000; // 安全起见
			_SetTimer(&g_objTimerMng, AUTO_RPT_TIMER, m_objAutoRptSta.m_dwChkInterv, G_TmAutoRpt );
		}
	}
}

int CAutoRpt::_Snd0154( const tagGPSData &v_objGps )
{
//#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	tag0154 objSnd;
//#endif
//#if 2 == USE_PROTOCOL
//	tag0156 objSnd;
//#endif
	tagQianGps objQianGps;
	char buf[ 500 ] = { 0 };
	int iBufLen = 0;
	int iRet = 0;
	BYTE bytDataType;

	// 遍历所有报警状态,生成报警数据
//	g_objMonAlert.FillAlertSymb( g_objMonAlert.GetAlertSta(), objSnd.m_bytAlertHi, objSnd.m_bytAlertLo );
	g_objMonAlert.FillAlertSymb( g_objMonAlert.GetAlertSta(), &(objSnd.m_bytAlertHi), &(objSnd.m_bytAlertLo) );

	objSnd.m_bytMonCentCount = 1; // 中心个数暂填1
	g_objMonAlert.GpsToQianGps( v_objGps, objQianGps );
	memcpy( &(objSnd.m_objQianGps), &objQianGps, sizeof(objSnd.m_objQianGps) );

//#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	bytDataType = 0x54;
//#endif

//#if 2 == USE_PROTOCOL
//	bytDataType = 0x56;		// 司机编号和报警区域赋值 (待)
//#endif

	iRet = g_objSms.MakeSmsFrame((char*)&objSnd, sizeof(objSnd), 0x01, bytDataType, buf, sizeof(buf), iBufLen);
	if( iRet ) 
		return iRet;

	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, DATASYMB_0154 );
	if( iRet ) 
		return iRet;

	return iRet;
}

// 判断是否可以上报
ENU_RPTJUGRESULT CAutoRpt::_JugCanRpt( const tagGPSData& v_objGps )
{
	int iResult = 0;
	tag2QGprsCfg objGprsCfg;
	objGprsCfg.Init();
	
	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	// 上报标志判断
	if( 1 != objGprsCfg.m_bytGpsUpType )
	{
		return RPT_CLOSE;
	}

	// 上报时间段判断
	BYTE bytCurHour = 0, bytCurMin = 0;
	if( objGprsCfg.m_bytRptBeginHour != objGprsCfg.m_bytRptEndHour
		|| objGprsCfg.m_bytRptBeginMin != objGprsCfg.m_bytRptEndMin )
	{
		if( 'A' == v_objGps.valid )
		{
			bytCurHour = BYTE( v_objGps.nHour );
			bytCurMin = BYTE( v_objGps.nMinute );
		}
		else
		{
			struct tm pCurrentTime;
			G_GetTime(&pCurrentTime);

			bytCurHour = BYTE( pCurrentTime.tm_hour );
			bytCurMin = BYTE( pCurrentTime.tm_min );
		}

		if( bytCurHour > objGprsCfg.m_bytRptEndHour
			||
			bytCurHour < objGprsCfg.m_bytRptBeginHour
			)
		{
			return RPT_OUTPRID;
		}
		else if(
			(bytCurHour == objGprsCfg.m_bytRptBeginHour && bytCurMin < objGprsCfg.m_bytRptBeginMin)
			||
			(bytCurHour == objGprsCfg.m_bytRptEndHour && bytCurMin > objGprsCfg.m_bytRptEndMin)
			)
		{
			return RPT_OUTPRID;
		}
	}

	return RPT_YES;
}

#endif





