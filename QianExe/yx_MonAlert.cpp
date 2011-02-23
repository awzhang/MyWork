#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-MonAlert ")

void G_TmMon(void *arg, int len)
{
	g_objMonAlert.P_TmMon();
}

void G_TmAlertInvalid(void *arg, int len)
{
	g_objMonAlert.P_TmAlertInvalid();
}

void G_TmAlertSilent(void *arg, int len)
{
	g_objMonAlert.P_TmAlertSilent();
}

void G_TmAutoReqLsn(void *arg, int len)
{
	g_objMonAlert.P_TmAutoReqLsn();
}

void G_TmAlertInitQuery(void *arg, int len)
{
	g_objMonAlert.P_TmAlertInitQuery();
}

void G_TmAlertTest(void *arg, int len)
{
	g_objMonAlert.P_TmAlertTest();
}

//////////////////////////////////////////////////////////////////////////
CMonAlert::CMonAlert()
{
	m_dwAlertSymb = 0;
	
	m_dwAlertFootTm = 0;
	m_dwAlertPowerOffTm = 0;
	m_dwAlertBrownoutTm = 0;
	m_dwAlertFrontDoorTm = 0;
	m_dwAlertBackDoorTm = 0;
	m_dwAlertOverTurnTm = 0;
	m_dwAlertBumpTm = 0;
	
	m_bytAutoReqLsnCount = 0;
	m_bytAutoReqLsnContBusyCt = 0;
	memset( m_szAutoReqLsnTel, 0, sizeof(m_szAutoReqLsnTel) );
	
	m_dSpdAlermMinSpd = 9999999;
	m_bytSpdAlermPrid = 255;
	m_bytSpdAlermSta = 0; // 必须初始化为false
}

CMonAlert::~CMonAlert()
{

}

void CMonAlert::P_TmMon()
{
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	int iResult = 0;
	
	/// 获得基本数据和信息
	DWORD dwCur = GetTickCount();
	DWORD dwIntervChk = dwCur - m_objMonStatus.m_dwLstChkTime; // 距上次定时检查的时间
	tag2QGprsCfg objGprsCfg;
	tagGPSData objGps(0);
	bool bNeedSend = false;

	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg) ,sizeof(objGprsCfg) );
	
	// 递减剩余监控时间
	if( INVALID_NUM_32 != m_objMonStatus.m_dwMonTime )
	{
		if( m_objMonStatus.m_dwMonTime >= dwIntervChk )
			m_objMonStatus.m_dwMonTime -= dwIntervChk;
		else
			m_objMonStatus.m_dwMonTime = 0;
	}
	
	// 修改上次检查时间,以便后续的监控时间到判断
	m_objMonStatus.m_dwLstChkTime = dwCur;
	
	// 获取当前GPS信息
	iResult = GetCurGPS( objGps, true );
	if( iResult ) goto MONTIMER_END;
	
	/// 判断是否要发送监控数据
	if( m_objMonStatus.m_bInterv )
	{
		DWORD dwIntervSend = dwCur - m_objMonStatus.m_dwLstSendTime; // 距上次发送监控数据的时间
		if( dwIntervSend >= m_objMonStatus.m_dwMonInterv ) // 若监控间隔到
		{
			bNeedSend = true;
		}
	}
	if( m_objMonStatus.m_bSpace && !bNeedSend )
	{
		m_objMonStatus.m_dLstSendDis += G_CalGeoDis2(m_objMonStatus.m_dLstLon, m_objMonStatus.m_dLstLat,
			objGps.longitude, objGps.latitude);
		m_objMonStatus.m_dLstLon = objGps.longitude;
		m_objMonStatus.m_dLstLat = objGps.latitude;
		
		if( m_objMonStatus.m_dLstSendDis >= double( m_objMonStatus.m_dwSpace ) )
		{
			bNeedSend = true;
			m_objMonStatus.m_dLstSendDis = 0;
		}
	}
	
	/// 发送监控数据
	if( bNeedSend )
	{
		_Snd0145( objGps, objGprsCfg );
		
		// 只要发送了监控数据就更新 (重要)
		m_objMonStatus.m_dwLstSendTime = dwCur; // 使得重新判断定时
		m_objMonStatus.m_dLstSendDis = 0; // 使得重新判断定距
		m_objMonStatus.m_dLstLon = objGps.longitude;
		m_objMonStatus.m_dLstLat = objGps.latitude;
		
		// 递减GPS数据个数
		if( m_objMonStatus.m_dwGpsCount > 0 ) -- m_objMonStatus.m_dwGpsCount;
	}
	
MONTIMER_END:
	// 车台终止监控判断
	if( 0 == m_objMonStatus.m_dwMonTime || 0 == m_objMonStatus.m_dwGpsCount ) // 若监控时间到,或者上传GPS个数到限
	{
		BYTE bytEndType = 0 == m_objMonStatus.m_dwGpsCount ? 0x05 : 0x01;
		
		iResult = g_objSms.MakeSmsFrame((char*)&bytEndType, sizeof(bytEndType), 0x01, 0x44, buf, sizeof(buf), iBufLen);
		if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV11, // 优先级应和监控数据一样
			(objGprsCfg.m_bytChannelBkType_1 & 0x40) ? DATASYMB_SMSBACKUP : 0 );
				
		_EndMon(); // 要放在后面，因为函数内会清除监控状态，放在前面导致后面根据状态填写的“终止监控原因”不正确
#if USE_LEDTYPE == 3
		//清空LED顶灯报警信息
		g_objKTLed.SetLedShow('A',0xff,0);
#endif
	}
	else
	{
		// 约每10次,重新设置一次定时器
		static BYTE sta_bytCt = 0;
		if( 0 == sta_bytCt ++ % 10 )
		{
			if( m_objMonStatus.m_dwChkInterv < 1000 ) m_objMonStatus.m_dwChkInterv = 1000; // 安全起见
			_SetTimer(&g_objTimerMng, MON_TIMER, m_objMonStatus.m_dwChkInterv, G_TmMon );
		}
	}
}

void CMonAlert::P_TmAlertSilent()
{
	_KillTimer(&g_objTimerMng, ALERT_SILENT_TIMER );

	char cResult = 0;

	// 判断是否还存在断电报警
	m_dwAlertSymb &= ~ALERT_TYPE_POWOFF;
	//if( g_objIO.QueryOneIOState(IOCTL_GPT_POWDET_STATE, cResult) )
	{
		if( 1 == cResult )
		{
			m_dwAlertSymb |= ALERT_TYPE_POWOFF;
		}
	}

	if( ALERT_TYPE_POWOFF & m_dwAlertSymb )
		SetAlertPowOff( true );
	else if( ALERT_TYPE_POWBROWNOUT & m_dwAlertSymb )
		SetAlertPowBrownout( true );
}

void CMonAlert::P_TmAlertInvalid()
{
	_ClrAlertSta();
}

void CMonAlert::P_TmAutoReqLsn()
{
	_KillTimer(&g_objTimerMng, AUTO_REQLSN_TIMER );
	
	// EVDO和TD模块使用USB口通信，可以同时打电话和拨号
	if (NETWORK_TYPE == NETWORK_EVDO || NETWORK_TYPE == NETWORK_TD || NETWORK_TYPE == NETWORK_WCDMA)
	{
		if( 0 != _AutoReqLsn() )
		{
			if( m_bytAutoReqLsnCount < AUTO_REQLSN_MAXTIMES ) // 若请求监听次数未到限定次数
				_SetTimer(&g_objTimerMng, AUTO_REQLSN_TIMER, AUTO_REQLSN_SHORTINTERVAL, G_TmAutoReqLsn );
		}
	}
	else if (NETWORK_TYPE == NETWORK_GSM)
	{
		if( g_objPhoto.IsPhotoEnd() // 且拍照结束
			&& // 且满足如下条件
				( 
				g_objPhoto.IsUploadPhotoEnd() // 照片上传结束
				||
				g_objPhoto.IsUploadPhotoBreak() // 或上传照片因故中断
				||
				!g_objSock.IsOnline() // 或GPRS未连接未登陆
				)
			)
		{	
			if( 0 != _AutoReqLsn() )
			{
				if( m_bytAutoReqLsnCount < AUTO_REQLSN_MAXTIMES ) // 若请求监听次数未到限定次数
					_SetTimer(&g_objTimerMng, AUTO_REQLSN_TIMER, AUTO_REQLSN_SHORTINTERVAL, G_TmAutoReqLsn );
			}
		}
		else
			_SetTimer(&g_objTimerMng, AUTO_REQLSN_TIMER, 3000, G_TmAutoReqLsn );
	}
}

void CMonAlert::P_TmAlertTest()
{

}

void CMonAlert::P_TmAlertInitQuery()
{
	_KillTimer(&g_objTimerMng, ALERT_INITQUERY_TIMER );
}

void CMonAlert::P_TmAlertSpd()
{
}

int CMonAlert::_BeginMon( double v_dLon, double v_dLat )
{
	// GPS模块上电
 	g_objQianIO.StopAccChk();

	char buf = 0x04;
	DataPush(&buf, 1, DEV_QIAN, DEV_DIAODU, LV2);

	// 关闭主动上报
#if USE_AUTORPT == 1
	g_objAutoRpt.EndAutoRpt();
#endif

	// 更新监控条件,并设置监控初始状态
	m_objMonStatus.m_dwLstChkTime = GetTickCount();
	m_objMonStatus.m_dwLstSendTime = m_objMonStatus.m_dwLstChkTime;
	if( 0 == v_dLon || 0 == v_dLat )
	{
		tagGPSData objGps(0);
		GetCurGPS( objGps, true );
		m_objMonStatus.m_dLstLon = objGps.longitude;
		m_objMonStatus.m_dLstLat = objGps.latitude;
	}
	else
	{
		m_objMonStatus.m_dLstLon = v_dLon;
		m_objMonStatus.m_dLstLat = v_dLat;
	}
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	m_objMonStatus.m_dwGpsCount = 0xffffffff;
#endif	

	// 重启监控定时
	if( m_objMonStatus.m_bSpace ) m_objMonStatus.m_dwChkInterv = MON_MININTERVAL; // 有定距时,则采用默认监控间隔
	else m_objMonStatus.m_dwChkInterv = m_objMonStatus.m_dwMonInterv;
	unsigned int uiTmId = _SetTimer(&g_objTimerMng, MON_TIMER, m_objMonStatus.m_dwChkInterv, G_TmMon );

	PRTMSG(MSG_NOR, "Mon Start, TmId = %d\n", uiTmId );

	return 0;
}

/// 不仅仅是终止监控，清除报警的相关工作也要在这里进行
int CMonAlert::_EndMon()
{
	// 启动关闭GPS电源检查
	g_objQianIO.StartAccChk( 2000 );

	// 关闭监控定时
	m_objMonStatus.Init();
	_KillTimer(&g_objTimerMng, MON_TIMER );
	
	// 取消报警时中止通话
	{
		char buf[2];
		buf[0] = (char)0xf3;
		buf[1] = 0;
		DataPush(buf, 2, DEV_QIAN, DEV_PHONE, LV2);
	}

	// 清除报警状态
	_ClrAlertSta();

	PRTMSG(MSG_NOR, "Mon Stop!\n" );

#if USE_AUTORPT == 1
	// 重新启动主动上报
	g_objAutoRpt.Init();
#endif

	return 0;
}

int CMonAlert::_Snd0145( const tagGPSData &v_objGps, const tag2QGprsCfg &v_objGprsCfg, bool v_bFbidAllowSms /*=false*/ )
{
	int iBufLen = 0;
	int iRet = 0;
	char buf[ SOCK_MAXSIZE ];
	tagQianGps objQianGps;
	tag0145 data;
	DWORD dwAlertSta = 0;

	PRTMSG(MSG_DBG, "_Send0145 Alerm data.\n");
	
	// 遍历所有报警状态,生成报警数据
	dwAlertSta = g_objMonAlert.GetAlertSta();
	//FillAlertSymb( dwAlertSta, data.m_bytAlertHi, data.m_bytAlertLo );
	FillAlertSymb(dwAlertSta, &(data.m_bytAlertHi), &(data.m_bytAlertLo));

	// 中心个数暂填1
	data.m_bytMonCentCount = 1;

	// gps数据
	GpsToQianGps( v_objGps, objQianGps );
	memcpy( &(data.m_objQianGps), &objQianGps, sizeof(data.m_objQianGps) );
	
#if 2 == USE_PROTOCOL
	// 报警区域赋值
	data.m_szAlertAreaCode[0] = 0;
	data.m_szAlertAreaCode[1] = (char)g_objDriveRecord.m_bytAreaCode;
#endif

	// SMS封装
	iRet = g_objSms.MakeSmsFrame((char*)&data, sizeof(data), 0x01, 0x45, buf, sizeof(buf), iBufLen);
	if( iRet ) return iRet;

	// 发送
	BYTE bytLvl = LV8, bytSndSymb = 0;
	if( !dwAlertSta )
	{
		bytLvl = LV11;
		bytSndSymb = 0;
	}
	else
	{
		bytLvl = LV13;
		bytSndSymb = DATASYMB_SMSBACKUP;
	}
	
	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, bytLvl, bytSndSymb );

	return iRet;
}

int CMonAlert::_Snd0149( const tagGPSData &v_objGps, const tag2QGprsCfg &v_objGprsCfg, bool v_bFbidAllowSms /*=false*/ )
{
	int iBufLen = 0;
	int iRet = 0;
	char buf[ SOCK_MAXSIZE ];
	tagQianGps objQianGps;
	tag0149 data;
	DWORD dwAlertSta = 0;

	PRTMSG(MSG_DBG, "_Send0149 Alerm data.\n");
	
	// 遍历所有报警状态,生成报警数据
	dwAlertSta = g_objMonAlert.GetAlertSta();
	FillAlertSymbFor0149(dwAlertSta, &(data.m_bytAlertHi), &(data.m_bytAlertLo));

	// 中心个数暂填1
	data.m_bytMonCentCount = 1;

	// gps数据
	GpsToQianGps( v_objGps, objQianGps );
	memcpy( &(data.m_objQianGps), &objQianGps, sizeof(data.m_objQianGps) );
	
	// SMS封装
	iRet = g_objSms.MakeSmsFrame((char*)&data, sizeof(data), 0x01, 0x49, buf, sizeof(buf), iBufLen);
	if( iRet ) return iRet;

	// 发送
	BYTE bytLvl = LV8, bytSndSymb = 0;
	if( !dwAlertSta )
	{
		bytLvl = LV11;
		bytSndSymb = 0;
	}
	else
	{
		bytLvl = LV13;
		bytSndSymb = DATASYMB_SMSBACKUP;
	}
	
	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, bytLvl, bytSndSymb );

	return iRet;
}

int CMonAlert::_SendOne0149Alert(const tagGPSData &v_objGps, byte v_bytAlertType)
{
	int iBufLen = 0;
	int iRet = 0;
	char buf[ SOCK_MAXSIZE ];
	tagQianGps objQianGps;

	char szTempBuf[SOCK_MAXSIZE] = {0};
	int  iTempLen = 0;

	// 中心个数
	szTempBuf[iTempLen++] = 1;
	
	// gps数据
	GpsToQianGps( v_objGps, objQianGps );
	memcpy( szTempBuf+iTempLen, &objQianGps, sizeof(objQianGps) );
	iTempLen += sizeof(objQianGps);

	// 版本号
	szTempBuf[iTempLen++] = 0x01;
	
	// 扩展参数个数
	szTempBuf[iTempLen++] = 0x01;

	// 参数类型：报警类
	szTempBuf[iTempLen++] = 0x02;

	// 参数长度
	szTempBuf[iTempLen++] = 0x01;

	// 参数数据
	szTempBuf[iTempLen++] = v_bytAlertType;
	
	// SMS封装
	iRet = g_objSms.MakeSmsFrame((char*)szTempBuf, iTempLen, 0x01, 0x49, buf, sizeof(buf), iBufLen);
	if( iRet ) return iRet;
	
	// 发送
	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV13, 0 );
	return iRet;
}

void CMonAlert::Init()
{
	_SetTimer(&g_objTimerMng, ALERT_INITQUERY_TIMER, 5000, G_TmAlertInitQuery );
}

// 仅适合于主线程中调用
// 若要在子线程中获取GPS，请直接调用dll接口——::GetCurGps
int CMonAlert::GetCurGPS( tagGPSData& v_objGps, bool v_bGetLstValid )
{
	bool bNeedSetBackGps = false;
	int iRet = GetCurGps( &v_objGps, sizeof(v_objGps), v_bGetLstValid ? GPS_LSTVALID : GPS_REAL );

#if USE_BLK == 1
	// 第1次GPS有效时的保险处理：删除有效日期之后的黑匣子数据
	static bool sta_bDelLaterBlk = false;
	if( !sta_bDelLaterBlk && 'A' == v_objGps.valid )
	{
		g_objBlk.DelLaterBlk( v_objGps );
		sta_bDelLaterBlk = true;
	}

	// 每600次有效时的检测：当日期变化时，删除1年之前的黑匣子数据
	if( 'A' == v_objGps.valid  )
	{
		static DWORD sta_dwGpsValidTime = 0;
		++ sta_dwGpsValidTime;

		if( 599 == sta_dwGpsValidTime % 600 ) // 每600次
		{
			static byte bytDay = 0;

			struct tm pCurrentTime;
			G_GetTime(&pCurrentTime);

			if( bytDay != pCurrentTime.tm_mday ) // 若日期变化
			{
				g_objBlk.DelOlderBlk( v_objGps );
				bytDay = pCurrentTime.tm_mday;
			}
		}
	}
	
	static bool sta_bGetBlk = false;
	if( !sta_bGetBlk && v_bGetLstValid && (0 == v_objGps.longitude || 0 == v_objGps.latitude) )
	{
		// 若获取的GPS数据完全无效(全0),且允许使用上次有效数据代替,则读取最后一个黑匣子数据
		tagGPSData objGps(0);
		iRet = g_objBlk.GetLatestBlkGps( objGps );
		if( !iRet )
		{
			v_objGps = objGps;
			bNeedSetBackGps = true;
		}
		
		sta_bGetBlk = true; // 无论是否读到黑匣子数据，都只读1次
	}

#endif

#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 || USE_DRVRECRPT == 11|| USE_DRVRECRPT == 0
	static bool sta_bGetDrvRec = false;
	if( !sta_bGetDrvRec && v_bGetLstValid && (0 == v_objGps.longitude || 0 == v_objGps.latitude) )
	{
		double dLichen;
		long lLon = 0, lLat = 0;
		g_objCarSta.GetLstValidLonlatAndLichen( lLon, lLat, dLichen );
		v_objGps.longitude = double(lLon) / LONLAT_DO_TO_L;
		v_objGps.latitude = double(lLat) / LONLAT_DO_TO_L;
		v_objGps.speed = 0;
		v_objGps.direction = 0;
		v_objGps.valid = 'V';
		
		bNeedSetBackGps = true;
		
		sta_bGetDrvRec = true;
	}
#endif

	if( bNeedSetBackGps )
	{
		SetCurGps( &v_objGps, GPS_REAL | GPS_LSTVALID );
		GetCurGps( &v_objGps, sizeof(v_objGps), GPS_LSTVALID );
	}

	// ACC无效而且GPS无效时，速度强制为0
	if( 'A' !=v_objGps.valid )
	{
		v_objGps.speed = 0;
	}

	return 0; // 无论是否失败，都返回成功，因为有的地方如果读GPS失败，将发送失败应答，似乎不好，比如监控应答
}

int CMonAlert::GetCurQianGps( tagQianGps &v_objQianGps, bool v_bGetLstValid )
{
	tagGPSData gps(0);
	int iResult = GetCurGPS( gps, v_bGetLstValid );
	if( iResult )
	{
		memset( (void*)&v_objQianGps, 0, sizeof(v_objQianGps) );
		v_objQianGps.m_bytMix = 0x60;
		return iResult;
	}

	GpsToQianGps( gps, v_objQianGps );
	return 0; // 无论是否失败，都返回成功，因为有的地方如果读GPS失败，将发送失败应答，似乎不好
}

void CMonAlert::GpsToQianGps( const tagGPSData &v_objGps, tagQianGps &v_objQianGps )
{
	v_objQianGps.m_bytGpsType = 0x42; // 标准GPS数据

// 千里眼通用版,公开版_研三
// 01ASCLGV
// A：GPS数据有效位，0-有效；1-无效
// L：车台负载状态，0－空车；1－重车
// S：省电模式，0－非省电模式；1－省电模式
// C: ACC状态， 0－ACC OFF， 1 － ACC ON
// G：经度字段中度字节的最高位
// V：速度字段最高位


// 千里眼公开版_研一
// 字节定义为：01A0TTGV
// A：GPS数据有效位，0-有效；1-无效
// G：经度字段中度字节的最高位
// V：速度字段最高位
// TT：空重车及停业状态 01-空车  10-重车 11-待定

	v_objQianGps.m_bytMix = 0x40;

	// 空车/重车
	DWORD dwVeSta = g_objCarSta.GetVeSta();
	if( dwVeSta & VESTA_HEAVY )
	{
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
		v_objQianGps.m_bytMix |= 0x04;
#endif
#if 2 == USE_PROTOCOL
		v_objQianGps.m_bytMix &= 0xf3;
		v_objQianGps.m_bytMix |= 0x08;
#endif
	}
	else
	{
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
		v_objQianGps.m_bytMix &= 0xfb;
#endif
#if 2 == USE_PROTOCOL
		v_objQianGps.m_bytMix &= 0xf3;
		v_objQianGps.m_bytMix |= 0x04;
#endif
	}
		
	// 省电模式
	if( g_objQianIO.GetDeviceSta() & DEVSTA_SYSTEMSLEEP )
		v_objQianGps.m_bytMix |= 0x10;
	else
		v_objQianGps.m_bytMix &= 0xef;

	// ACC状态
	byte bytSta = 0;
	IOGet(IOS_ACC, &bytSta);
	if( bytSta == IO_ACC_ON )
		v_objQianGps.m_bytMix |= 0x08;	
	else
		v_objQianGps.m_bytMix &= 0xf7;


	// GPS是否有效
	if( 'A' == v_objGps.valid ) v_objQianGps.m_bytMix &= 0xdf;
	else v_objQianGps.m_bytMix |= 0x20;

	v_objQianGps.m_bytDay = BYTE(v_objGps.nDay);
	v_objQianGps.m_bytMon = BYTE(v_objGps.nMonth);
	v_objQianGps.m_bytYear = BYTE(v_objGps.nYear % 1000);
	v_objQianGps.m_bytSec = BYTE(v_objGps.nSecond);
	v_objQianGps.m_bytMin = BYTE(v_objGps.nMinute);
	v_objQianGps.m_bytHour = BYTE(v_objGps.nHour);
	v_objQianGps.m_bytDir = BYTE(v_objGps.direction / 3 + 1);

	// 北京时间->格林威治 (仅仅是小时改变)
	if( v_objQianGps.m_bytHour < 8 ) 
		v_objQianGps.m_bytHour += 24;

	v_objQianGps.m_bytHour -= 8;

	// 原始纬度
	v_objQianGps.m_bytLatDu = BYTE(v_objGps.latitude);
	double dLatFen = ( v_objGps.latitude - v_objQianGps.m_bytLatDu ) * 60;
	v_objQianGps.m_bytLatFenzen = BYTE(dLatFen);
	WORD wLatFenxiao = WORD( (dLatFen - v_objQianGps.m_bytLatFenzen) * 10000 );
	v_objQianGps.m_bytLatFenxiao1 = wLatFenxiao / 100;
	v_objQianGps.m_bytLatFenxiao2 = wLatFenxiao % 100;

	// 原始经度
	v_objQianGps.m_bytLonDu = BYTE(v_objGps.longitude);
	double dLonFen = (v_objGps.longitude - v_objQianGps.m_bytLonDu) * 60;
	v_objQianGps.m_bytLonFenzen = BYTE(dLonFen);
	WORD wLonFenxiao = WORD( (dLonFen - v_objQianGps.m_bytLonFenzen) * 10000 );
	v_objQianGps.m_bytLonFenxiao1 = wLonFenxiao / 100;
	v_objQianGps.m_bytLonFenxiao2 = wLonFenxiao % 100;

	// 原始速度
	double dSpeed = v_objGps.speed * 3.6 / 1.852; // 转换为海里/小时
	v_objQianGps.m_bytSpeed = BYTE( dSpeed );

	// 校验和
	{
		BYTE bytSum = 0;
		char szTemp[64];
		int i;

		// 纬度
		int iTemp = int( v_objQianGps.m_bytLatDu );
		sprintf( szTemp, "%02d", iTemp );
		for( i = 0; i < int(strlen(szTemp)); i++ ) bytSum += BYTE( szTemp[i] );

		// 纬度整数分
		iTemp = int( v_objQianGps.m_bytLatFenzen );
		sprintf( szTemp, "%02d.", iTemp );
		for( i = 0; i < int(strlen(szTemp)); i++ ) bytSum += BYTE( szTemp[i] );

		// 纬度小数分_1
		iTemp = int( v_objQianGps.m_bytLatFenxiao1 );
		sprintf( szTemp, "%02d", iTemp );
		for( i = 0; i < int(strlen(szTemp)); i++ ) bytSum += BYTE( szTemp[i] );

		// 纬度小数分_2
		iTemp = int( v_objQianGps.m_bytLatFenxiao2 );
		sprintf( szTemp, "%02d", iTemp );
		for( i = 0; i < int(strlen(szTemp)); i++ ) bytSum += BYTE( szTemp[i] );

		// 经度
		iTemp = int( v_objQianGps.m_bytLonDu );
		sprintf( szTemp, "%03d", iTemp );
		for( i = 0; i < int(strlen(szTemp)); i++ ) bytSum += BYTE( szTemp[i] );

		// 经度整数分
		iTemp = int( v_objQianGps.m_bytLonFenzen );
		sprintf( szTemp, "%02d.", iTemp );
		for( i = 0; i < int(strlen(szTemp)); i++ ) bytSum += BYTE( szTemp[i] );

		// 经度小数分_1
		iTemp = int( v_objQianGps.m_bytLonFenxiao1 );
		sprintf( szTemp, "%02d", iTemp );
		for( i = 0; i < int(strlen(szTemp)); i++ ) bytSum += BYTE( szTemp[i] );

		// 经度小数分_2
		iTemp = int( v_objQianGps.m_bytLonFenxiao2 );
		sprintf( szTemp, "%02d", iTemp );
		for( i = 0; i < int(strlen(szTemp)); i++ ) bytSum += BYTE( szTemp[i] );

		// 速度
		iTemp = int( v_objQianGps.m_bytSpeed );
		sprintf( szTemp, "%03d.0", iTemp );
		for( i = 0; i < int(strlen(szTemp)); i++ ) bytSum += BYTE( szTemp[i] );

		bytSum &= 0x7f;
		if( 0 == bytSum ) bytSum = 0x7f;
		v_objQianGps.m_bytChk = bytSum;
	}

	// 转义
	if( 0 == v_objQianGps.m_bytLatDu ) v_objQianGps.m_bytLatDu = 0x7f;
	if( 0 == v_objQianGps.m_bytLatFenzen ) v_objQianGps.m_bytLatFenzen = 0x7f;
	if( 0 == v_objQianGps.m_bytLatFenxiao1 ) v_objQianGps.m_bytLatFenxiao1 = 0x7f;
	if( 0 == v_objQianGps.m_bytLatFenxiao2 ) v_objQianGps.m_bytLatFenxiao2 = 0x7f;
	if( 0 == v_objQianGps.m_bytLonFenzen ) v_objQianGps.m_bytLonFenzen = 0x7f;
	if( 0 == v_objQianGps.m_bytLonFenxiao1 ) v_objQianGps.m_bytLonFenxiao1 = 0x7f;
	if( 0 == v_objQianGps.m_bytLonFenxiao2 ) v_objQianGps.m_bytLonFenxiao2 = 0x7f;
	if( 0 == v_objQianGps.m_bytLonDu ) v_objQianGps.m_bytLonDu = 0xff;
	else if( 0x80 == v_objQianGps.m_bytLonDu ) v_objQianGps.m_bytLonDu = 0xfe;
	if( v_objQianGps.m_bytLonDu & 0x80 ) v_objQianGps.m_bytMix |= 0x02;
	else v_objQianGps.m_bytMix &= 0xfd;
	v_objQianGps.m_bytLonDu &= 0x7f;
	if( 0 == v_objQianGps.m_bytSpeed ) v_objQianGps.m_bytSpeed = 0xff;
	else if( 0x80 == v_objQianGps.m_bytSpeed ) v_objQianGps.m_bytSpeed = 0xfe;
	if( v_objQianGps.m_bytSpeed & 0x80 ) v_objQianGps.m_bytMix |= 0x01;
	else v_objQianGps.m_bytMix &= 0xfe;
	v_objQianGps.m_bytSpeed &= 0x7f;
}

//（需补充）
int CMonAlert::DealPhoneAutoReqLsnRes( BYTE v_bytResType )
{
}

/// 执行报警操作 (业务处理优先) (过滤了重复报警,只有新报警才会调用本过程)
int CMonAlert::_DealAlert( bool v_bReqLsn, bool v_bSilentTimer )
{
	int iRet = 0;
	
	if (NETWORK_TYPE != NETWORK_CDMA)
	{
		// 屏蔽CDMA车台的主动监听功能，因为会导致一直发送报警短信
		// 启动主动监听
		if( v_bReqLsn )
		{
			_BeginAutoReqLsn( ALERTREQLSN_DEALYPERIOD ); // 不立刻启动监听,以尽量保证第一个报警数据能发送出去
		}
	}

	// 打开报警静默定时
	if( v_bSilentTimer )
		_SetTimer(&g_objTimerMng, ALERT_SILENT_TIMER, ALERT_SILENT_PERIOD, G_TmAlertSilent );
		
	// 打开清除报警状态的定时
	_KillTimer(&g_objTimerMng, ALERT_INVALID_TIMER );
	_SetTimer(&g_objTimerMng, ALERT_INVALID_TIMER, ALERT_INVALID_PERIOD, G_TmAlertInvalid );
	
	// 修改定时监控参数
	if( !m_objMonStatus.m_bInterv || m_objMonStatus.m_dwMonInterv > ALERT_SNDDATA_INTERVAL )
		m_objMonStatus.m_dwMonInterv = ALERT_SNDDATA_INTERVAL;
	m_objMonStatus.m_bInterv = true;
	if( ALERT_SNDDATA_PERIOD > m_objMonStatus.m_dwMonTime + 2000 )
	{
		m_objMonStatus.m_dwMonTime = ALERT_SNDDATA_PERIOD;
		m_objMonStatus.m_bAlertAuto = true; // 标记为报警主动激发的监控
	}

	// 立刻发送第一个报警数据
	tagGPSData objGps(0);
	tag2QGprsCfg objGprsCfg;
	GetCurGPS( objGps, true );
	objGprsCfg.Init();
 	::GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),
 		offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );
	_Snd0145( objGps, objGprsCfg, true );

	// 打开(重设)监控定时
	_BeginMon( 0, 0 );

	return iRet;
}

void CMonAlert::_ClrAlertSta()
{
	// 关闭 "清除报警状态"定时
//	m_dwAlertSymb = 0;
	m_dwAlertSymb &= 0x00000808;//前后车门只有在关车门时才清除报警状态
	m_objMonStatus.m_bAlertAuto = false;
	_KillTimer(&g_objTimerMng, ALERT_INVALID_TIMER );

	// 关闭主动监听定时
	_KillTimer(&g_objTimerMng, AUTO_REQLSN_TIMER );

#if USE_LEDTYPE == 1	
	char cCmd = LED_CMD_ALERMCANCEL;
	g_objLedBohai.m_objLocalCmdQue.push( (unsigned char*)&cCmd, 1 );
#endif

#if USE_LEDTYPE == 2
	g_objLedChuangLi.DealEvent(0x04, 'A', 0);
#endif

	// 指示灯慢闪
	char szbuf[10] = {0};
	szbuf[0] = 0x02;	// 0x02表示控制指示灯
	szbuf[1] = 0x06;	// 撤销抢劫报警通知
	DataPush(szbuf, 2, DEV_QIAN, DEV_DIAODU, LV2);
}

/// 打开/关闭抢劫报警
int CMonAlert::SetAlertFoot( const bool v_bSet )
{
	int iRet = 0;
	DWORD dwCur = GetTickCount();

	tag2QAlertCfg objAlertCfg;
	objAlertCfg.Init();
 	GetSecCfg( &objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );

	if( v_bSet && !(objAlertCfg.m_bytAlert_1 & 0x20) )
	{
		m_dwAlertSymb &= ~ALERT_TYPE_FOOT;
		PRTMSG(MSG_NOR, "Deal FootAlerm, but this func is Disabled! Exit!\n" );
		return 0;
	}

	// 注意初始化值
	BYTE bytAlertDealType = 0; // 1,要新处理报警; 2,要关闭所有报警处理

	// (注意处理时的顺序)
	if( v_bSet ) // 若是设置报警
	{
		if( !(ALERT_TYPE_FOOT & m_dwAlertSymb) ) // 若之前没有该报警状态
		{
			bytAlertDealType = 1; // 要新处理该报警
			
#if USE_LEDTYPE == 3
			//LED 顶灯显示报警
			g_objKTLed.SetLedShow('A',0,1);
#endif
		}
		else // 若已经有该报警状态
		{
			if( dwCur - m_dwAlertFootTm > ALERT_SNDDATA_PERIOD ) // 若超过报警活动时间
			{
				bytAlertDealType = 1; // 要新处理该报警
			}
		}

		m_dwAlertSymb |= ALERT_TYPE_FOOT; // 设置该报警状态
	}

	// 新报警处理
	if( 1 == bytAlertDealType ) // 若要新处理报警
	{
		PRTMSG(MSG_NOR, "Start FootAlerm\n" );
		
		char buf[16] = { 0 };

		//增加报警时中止通话
		buf[0] = (char) 0xf3;
		buf[1] = (char) ALERT_STOP_PHONE_PRD;
		DataPush(buf, 2, DEV_QIAN, DEV_PHONE, LV2);
		PRTMSG(MSG_NOR, "Deal Foot Alerm,Req Comu handoff phone,and Fbid Phone in %d s\n", ALERT_STOP_PHONE_PRD );

#if USE_LEDTYPE == 1
		char cCmd = LED_CMD_ALERM;
		g_objLedBohai.m_objLocalCmdQue.push( (unsigned char*)&cCmd, 1 );
#endif

#if USE_LEDTYPE == 2
		g_objLedChuangLi.DealEvent(0x03, 'A', 0);
#endif

		// 统一报警处理
		iRet = _DealAlert( true, false ); // (与老大商定,抢劫报警之后,不检查抢劫报警电平)
		
		// 更新报警初始时刻
		m_dwAlertFootTm = dwCur;

		// 抢劫报警时视频主动上传
		char szbuf[100];
// 		int  iBufLen = 0;
// 		byte bytType = 0x00;		// 抢劫报警时的视频主动上传类型为0x00 
// 		int iResult = g_objSms.MakeSmsFrame((char*)&bytType, 1, 0x38, 0x46, szbuf, sizeof(szbuf), iBufLen);
// 		if( !iResult ) g_objSock.SOCKSNDSMSDATA( szbuf, iBufLen, LV12, 0 );

		// 指示灯快闪
		szbuf[0] = 0x02;	// 0x02表示控制指示灯
		szbuf[1] = 0x05;	// 绿灯快闪（160VC), 或 抢劫报警通知（V6-60)
		DataPush(szbuf, 2, DEV_QIAN, DEV_DIAODU, LV2);

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
 		g_objPhoto.AddPhoto( PHOTOEVT_FOOTALERM );
#endif
	}

	return 0;
}

/// 打开/关闭断电报警
int CMonAlert::SetAlertPowOff( const bool v_bSet )
{
	int iRet = 0;
	DWORD dwCur = GetTickCount();

	tag2QAlertCfg objAlertCfg;
 	GetSecCfg( &objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );

	if( v_bSet && !(objAlertCfg.m_bytAlert_1 & 0x04) )
	{
		m_dwAlertSymb &= ~ALERT_TYPE_POWOFF;
		PRTMSG(MSG_NOR, "Deal Powoff Alerm,but this func is Disabled! Exit!\n" );
		return 0;
	}

	// 注意初始化值
	BYTE bytAlertDealType = 0; // 1,要新处理报警; 2,要关闭所有报警处理

	// (注意处理时的顺序)
	if( v_bSet ) // 若是设置报警
	{
		if( !(ALERT_TYPE_POWOFF & m_dwAlertSymb) ) // 若之前没有该报警状态
		{
			bytAlertDealType = 1; // 要新处理该报警
		}
		else // 若已经有该报警状态
		{
			if( dwCur - m_dwAlertPowerOffTm > ALERT_SNDDATA_PERIOD ) // 若超过报警活动时间
			{
				bytAlertDealType = 1; // 要新处理该报警
			}
		}

		m_dwAlertSymb |= ALERT_TYPE_POWOFF; // 设置该报警状态
	}
	else // 若是清除报警
	{
		PRTMSG(MSG_NOR, "Stop Powoff Alerm!\n" );
		
		if( m_dwAlertSymb & ALERT_TYPE_POWOFF )
		{
			m_dwAlertSymb &= ~ALERT_TYPE_POWOFF; // 清除该报警状态
			
			// 不用关闭监控定时,因为可能报警关了,监控还存在
			// 不用打开主动上报定时,因为监控停止时会打开主动上报定时

			if( !m_dwAlertSymb ) // 若清除后没有任何报警
				bytAlertDealType = 2; // 要关闭报警处理
		}
	}

	// 新报警处理
	if( 1 == bytAlertDealType ) // 若要新处理报警
	{	
		PRTMSG(MSG_NOR, "Start Powoff Alerm!\n" );

		// 统一报警处理
		iRet = _DealAlert( false, true );

		// 更新报警初始时刻
		m_dwAlertPowerOffTm = dwCur;
	}
	else if( 2 == bytAlertDealType ) // 若要关闭所有报警
	{		
		if( m_objMonStatus.m_bAlertAuto )
		{
			_EndMon(); // 若是报警激发的,关闭监控

			char buf[ SOCK_MAXSIZE ];
			int iBufLen = 0;
			BYTE bytEndType = 0x04; // 因为报警信号终止
			int iResult = g_objSms.MakeSmsFrame((char*)&bytEndType, 1, 0x01, 0x44, buf, sizeof(buf), iBufLen);
			if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );
		}
	}

	return 0;
}

int CMonAlert::SetAlertPowBrownout( const bool v_bSet )
{
	int iRet = 0;
	DWORD dwCur = GetTickCount();

	tag2QAlertCfg objAlertCfg;
 	GetSecCfg( &objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );

	if( v_bSet && !(objAlertCfg.m_bytAlert_1 & 0x40) )
	{
		m_dwAlertSymb &= ~ALERT_TYPE_POWBROWNOUT;
		PRTMSG(MSG_NOR, "Deal Pow Brownout Alerm, but this func is Disabled! Exit!\n" );
		return 0;
	}

	// 注意初始化值
	BYTE bytAlertDealType = 0; // 1,要新处理报警; 2,要关闭所有报警处理

	// (注意处理时的顺序)
	if( v_bSet ) // 若是设置报警
	{
		if( !(ALERT_TYPE_POWBROWNOUT & m_dwAlertSymb) ) // 若之前没有该报警状态
		{
			bytAlertDealType = 1; // 要新处理该报警
		}
		else // 若已经有该报警状态
		{
			if( dwCur - m_dwAlertPowerOffTm > ALERT_SNDDATA_PERIOD ) // 若超过报警活动时间
			{
				bytAlertDealType = 1; // 要新处理该报警
			}
		}

		m_dwAlertSymb |= ALERT_TYPE_POWBROWNOUT; // 设置该报警状态
	}
	else // 若是清除报警
	{
		PRTMSG(MSG_NOR, "Stop Pow Brownout Alerm!\n" );
		
		if( m_dwAlertSymb & ALERT_TYPE_POWBROWNOUT )
		{
			m_dwAlertSymb &= ~ALERT_TYPE_POWBROWNOUT; // 清除该报警状态
			
			// 不用关闭监控定时,因为可能报警关了,监控还存在
			// 不用打开主动上报定时,因为监控停止时会打开主动上报定时

			if( !m_dwAlertSymb ) // 若清除后没有任何报警
				bytAlertDealType = 2; // 要关闭报警处理
		}
	}

	// 新报警处理
	if( 1 == bytAlertDealType ) // 若要新处理报警
	{	
		PRTMSG(MSG_NOR, "Start Pow Brownout Alerm!\n" );

		// 统一报警处理
		iRet = _DealAlert( false, true );

		// 更新报警初始时刻
		m_dwAlertPowerOffTm = dwCur;
	}
	else if( 2 == bytAlertDealType ) // 若要关闭所有报警
	{		
		// 到时候检查发现报警信号不存在时自然会关闭 _KillTimer(&g_objTimerMng, ALERT_SILENT_TIMER ); // 关闭静默报警定时器

		if( m_objMonStatus.m_bAlertAuto )
		{
			_EndMon(); // 若是报警激发的,关闭监控

			char buf[ SOCK_MAXSIZE ];
			int iBufLen = 0;
			BYTE bytEndType = 0x04; // 因为报警信号终止
			int iResult = g_objSms.MakeSmsFrame((char*)&bytEndType, 1, 0x01, 0x44, buf, sizeof(buf), iBufLen);
			if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );
		}
	}

	return 0;
}

/// 打开/关闭非法开前车门报警
int CMonAlert::SetAlertFrontDoor( const bool v_bSet )
{
	int iRet = 0;
	DWORD dwCur = GetTickCount();

	tag2QAlertCfg objAlertCfg;
 	GetSecCfg( &objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );
	
	tag2QGprsCfg objGprsCfg;
 	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	// 注意初始化值
	BYTE bytAlertDealType = 0; // 1,要新处理报警; 2,要关闭所有报警处理

	// (注意处理时的顺序)
	if( v_bSet ) // 若是设置报警
	{
		if( v_bSet && !(objAlertCfg.m_bytAlert_1 & 0x08) )
		{
			m_dwAlertSymb &= ~ALERT_TYPE_FRONTDOOR;
			PRTMSG(MSG_NOR, "Deal FrontDooropen Alerm, but this func is Disabled, Exit!\n" );
			return 0;
		}

		if( !(ALERT_TYPE_FRONTDOOR & m_dwAlertSymb) ) // 若之前没有该报警状态
		{
			bytAlertDealType = 1; // 要新处理该报警
		}
		else // 若已经有该报警状态
		{
			if( dwCur - m_dwAlertFrontDoorTm > ALERT_SNDDATA_PERIOD ) // 若超过报警活动时间
			{
				bytAlertDealType = 1; // 要新处理该报警
			}
		}

		m_dwAlertSymb |= ALERT_TYPE_FRONTDOOR; // 设置该报警状态
	}
	else // 若是清除报警
	{
		PRTMSG(MSG_NOR, "Stop FrontDooropen Alerm!\n" );
		
		if( m_dwAlertSymb & ALERT_TYPE_FRONTDOOR )
		{
			m_dwAlertSymb &= ~ALERT_TYPE_FRONTDOOR; // 清除该报警状态
			
			// 不用关闭监控定时,因为可能报警关了,监控还存在
			// 不用打开主动上报定时,因为监控停止时会打开主动上报定时

			if( !m_dwAlertSymb ) // 若清除后没有任何报警
				bytAlertDealType = 2; // 要关闭报警处理
		}
	}

	PRTMSG(MSG_NOR, "bytAlertDealType = %d\n", bytAlertDealType);
	
	// 新报警处理
	if( 1 == bytAlertDealType ) // 若要新处理报警
	{
		PRTMSG(MSG_NOR, "Start FrontDooropen Alerm!\n" );

		// 统一报警处理
		iRet = _DealAlert( false, false );

		// 更新报警初始时刻
		m_dwAlertFrontDoorTm = dwCur;
		
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
 		g_objPhoto.AddPhoto( PHOTOEVT_OPENDOOR );
#endif
	}
	else if( 2 == bytAlertDealType ) // 若要关闭所有报警
	{
		if( m_objMonStatus.m_bAlertAuto )
		{
			_EndMon(); // 若是报警激发的,关闭监控

			char buf[ SOCK_MAXSIZE ];
			int iBufLen = 0;
			BYTE bytEndType = 0x04; // 因为报警信号终止
			int iResult = g_objSms.MakeSmsFrame((char*)&bytEndType, 1, 0x01, 0x44, buf, sizeof(buf), iBufLen);
			if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );
		}
	}

	return 0;
}

/// 打开/关闭非法开后车门报警
int CMonAlert::SetAlertBackDoor( const bool v_bSet )
{
	int iRet = 0;
	DWORD dwCur = GetTickCount();

	tag2QAlertCfg objAlertCfg;
 	GetSecCfg( &objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );
	
	tag2QGprsCfg objGprsCfg;
 	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	// 注意初始化值
	BYTE bytAlertDealType = 0; // 1,要新处理报警; 2,要关闭所有报警处理

	// (注意处理时的顺序)
	if( v_bSet ) // 若是设置报警
	{
		if( v_bSet && !(objAlertCfg.m_bytAlert_1 & 0x08) )
		{
			m_dwAlertSymb &= ~ALERT_TYPE_BACKDOOR;
			PRTMSG(MSG_NOR, "Deal BackDooropen Alerm, but this func is Disabled, Exit!\n" );
			return 0;
		}

		if( !(ALERT_TYPE_BACKDOOR & m_dwAlertSymb) ) // 若之前没有该报警状态
		{
			bytAlertDealType = 1; // 要新处理该报警
		}
		else // 若已经有该报警状态
		{
			if( dwCur - m_dwAlertBackDoorTm > ALERT_SNDDATA_PERIOD ) // 若超过报警活动时间
			{
				bytAlertDealType = 1; // 要新处理该报警
			}
		}

		m_dwAlertSymb |= ALERT_TYPE_BACKDOOR; // 设置该报警状态
	}
	else // 若是清除报警
	{
		PRTMSG(MSG_NOR, "Stop BackDooropen Alerm!\n" );
		
		if( m_dwAlertSymb & ALERT_TYPE_BACKDOOR )
		{
			m_dwAlertSymb &= ~ALERT_TYPE_BACKDOOR; // 清除该报警状态
			
			// 不用关闭监控定时,因为可能报警关了,监控还存在
			// 不用打开主动上报定时,因为监控停止时会打开主动上报定时

			if( !m_dwAlertSymb ) // 若清除后没有任何报警
				bytAlertDealType = 2; // 要关闭报警处理
		}
	}

	// 新报警处理
	if( 1 == bytAlertDealType ) // 若要新处理报警
	{
		PRTMSG(MSG_NOR, "Start BackDooropen Alerm!\n" );

		// 统一报警处理
		iRet = _DealAlert( false, false );

		// 更新报警初始时刻
		m_dwAlertBackDoorTm = dwCur;
		
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
 		g_objPhoto.AddPhoto( PHOTOEVT_OPENDOOR );
#endif
	}
	else if( 2 == bytAlertDealType ) // 若要关闭所有报警
	{
		if( m_objMonStatus.m_bAlertAuto )
		{
			_EndMon(); // 若是报警激发的,关闭监控

			char buf[ SOCK_MAXSIZE ];
			int iBufLen = 0;
			BYTE bytEndType = 0x04; // 因为报警信号终止
			int iResult = g_objSms.MakeSmsFrame((char*)&bytEndType, 1, 0x01, 0x44, buf, sizeof(buf), iBufLen);
			if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );
		}
	}

	return 0;
}

int CMonAlert::SetAlertOverTurn( const bool v_bSet )
{
	int iRet = 0;
	DWORD dwCur = GetTickCount();

	tag2QAlertCfg objAlertCfg;
 	GetSecCfg( &objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );

	if( v_bSet && !(objAlertCfg.m_bytAlert_1 & 0x08) )
	{
		m_dwAlertSymb &= ~ALERT_TYPE_OVERTURN;
		PRTMSG(MSG_NOR, "Deal OverTurn Alerm, but this func is Disabled! Exit!\n" );
		return 0;
	}

	// 注意初始化值
	BYTE bytAlertDealType = 0; // 1,要新处理报警; 2,要关闭所有报警处理

	// (注意处理时的顺序)
	if( v_bSet ) // 若是设置报警
	{
		if( !(ALERT_TYPE_OVERTURN & m_dwAlertSymb) ) // 若之前没有该报警状态
		{
			bytAlertDealType = 1; // 要新处理该报警
		}
		else // 若已经有该报警状态
		{
			if( dwCur - m_dwAlertOverTurnTm > ALERT_SNDDATA_PERIOD ) // 若超过报警活动时间
			{
				bytAlertDealType = 1; // 要新处理该报警
			}
		}

		m_dwAlertSymb |= ALERT_TYPE_OVERTURN; // 设置该报警状态
	}

	// 新报警处理
	if( 1 == bytAlertDealType ) // 若要新处理报警
	{
		PRTMSG(MSG_NOR, "Start OverTurn Alerm\n" );

		// 统一报警处理
		iRet = _DealAlert( false, false ); // 侧翻报警之后,虽然定期会有报警电平出现,但主动查询时不一定能查到,故不需要检查
		
		// 更新报警初始时刻
		m_dwAlertOverTurnTm = dwCur;
		
		// 侧翻报警拍照（去除）
		char szBuf[2] = {0};
		szBuf[0] = 0x04;
		szBuf[1] = 0x02;
		DataPush(szBuf, 2, DEV_QIAN, DEV_DVR, LV2);
	}

	return 0;
}

int CMonAlert::SetAlertBump( const bool v_bSet )
{
	int iRet = 0;
	DWORD dwCur = GetTickCount();

	tag2QAlertCfg objAlertCfg;
 	::GetSecCfg( &objAlertCfg, sizeof(objAlertCfg),
 		offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );

	if( v_bSet && !(objAlertCfg.m_bytAlert_1 & 0x10) )
	{
		m_dwAlertSymb &= ~ALERT_TYPE_BUMP;
		PRTMSG(MSG_NOR, "Deal Bump Alerm, but this func is Disabled! Exit!\n" );
		return 0;
	}

	// 注意初始化值
	BYTE bytAlertDealType = 0; // 1,要新处理报警; 2,要关闭所有报警处理

	// (注意处理时的顺序)
	if( v_bSet ) // 若是设置报警
	{
		if( !(ALERT_TYPE_BUMP & m_dwAlertSymb) ) // 若之前没有该报警状态
		{
			bytAlertDealType = 1; // 要新处理该报警
		}
		else // 若已经有该报警状态
		{
			if( dwCur - m_dwAlertBumpTm > ALERT_SNDDATA_PERIOD ) // 若超过报警活动时间
			{
				bytAlertDealType = 1; // 要新处理该报警
			}
		}

		m_dwAlertSymb |= ALERT_TYPE_BUMP; // 设置该报警状态
	}

	// 新报警处理
	if( 1 == bytAlertDealType ) // 若要新处理报警
	{
		PRTMSG(MSG_NOR, "Start Bump Alerm\n" );

		// 统一报警处理
		iRet = _DealAlert( false, false ); // 碰撞报警之后,不需要定时检查报警电平
		
		// 更新报警初始时刻
		m_dwAlertBumpTm = dwCur;
		
		// 侧翻报警拍照（去除）
		char szBuf[2] = {0};
		szBuf[0] = 0x04;
		szBuf[1] = 0x03;
		DataPush(szBuf, 2, DEV_QIAN, DEV_DVR, LV2);
	}

	return 0;
}

int CMonAlert::SetAlertDrive( const bool v_bSet )
{
	int iRet = 0;

	tag2QAlertCfg objAlertCfg;
 	GetSecCfg( &objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );

	if( v_bSet && !(objAlertCfg.m_bytAlert_2 & 0x04) )
	{
		m_dwAlertSymb &= ~ALERT_TYPE_DRIVE;
		PRTMSG(MSG_NOR, "Deal Illegal Drive Alerm, but this func is Disabled! Exit!\n" );
		return 0;
	}

	if( v_bSet ) // 若要开始报警
	{
		PRTMSG(MSG_NOR, "Start Illegal Drive Alerm!\n" );

		m_dwAlertSymb |= ALERT_TYPE_DRIVE;

		// 发送一条非法启动报警
		tagGPSData objGps(0);
		tag2QGprsCfg objGprsCfg;
		GetCurGPS( objGps, true );
		objGprsCfg.Init();
		::GetSecCfg( &objGprsCfg, sizeof(objGprsCfg), offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );
		_SendOne0149Alert(objGps, ALERM_0149_25);
		
		m_dwAlertSymb &= ~ALERT_TYPE_DRIVE;	// 报警之后，将标志位清除

// 		// 统一报警处理
// 		iRet = _DealAlert( false, false );
// 		
// 		// 非法发动车辆报警拍照
// 		char szBuf[2] = {0};
// 		szBuf[0] = 0x04;
// 		szBuf[1] = 0x0a;
// 		DataPush(szBuf, 2, DEV_QIAN, DEV_DVR, LV2);
	}
// 	else
// 	{
// 		if( m_dwAlertSymb & ALERT_TYPE_DRIVE )
// 		{
// 			m_dwAlertSymb &= ~ALERT_TYPE_DRIVE;
// 
// 			if( 0 == m_dwAlertSymb && m_objMonStatus.m_bAlertAuto )
// 			{
// 				_EndMon(); // 若是报警激发的,关闭监控
// 
// 				char buf[ 200 ] = { 0 };
// 				int iBufLen = 0;
// 				BYTE bytEndType = 0x04; // 因为报警信号终止
// 				int iResult = g_objSms.MakeSmsFrame((char*)&bytEndType, 1, 0x01, 0x44, buf, sizeof(buf), iBufLen);
// 				if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );
// 			}
// 		}
// 	}

	return 0;
}

int CMonAlert::SetAlertOverSpd( const bool v_bSet )
{
	int iRet = 0;

	tag2QAlertCfg objAlertCfg;
 	::GetSecCfg( &objAlertCfg, sizeof(objAlertCfg),
 		offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );

	if( v_bSet && !(objAlertCfg.m_bytAlert_2 & 0x20) )
	{
		m_dwAlertSymb &= ~ALERT_TYPE_OVERSPEED;
		PRTMSG(MSG_NOR, "Deal OverSpeed Alerm, but this func is Disabled! Exit!\n" );
		return 0;
	}

	if( v_bSet ) // 若要开始报警
	{
		PRTMSG(MSG_NOR, "Start OverSpeed Alerm!\n" );

		m_dwAlertSymb |= ALERT_TYPE_OVERSPEED;

		// 统一报警处理（超速报警不统一处理，因为只报一次）
		//iRet = _DealAlert( false, false );

		// 发送一条超速报警
		tagGPSData objGps(0);
		tag2QGprsCfg objGprsCfg;
		GetCurGPS( objGps, true );
		objGprsCfg.Init();
		::GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),
			offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );
		_Snd0145( objGps, objGprsCfg, true );

		m_dwAlertSymb &= ~ALERT_TYPE_OVERSPEED;	// 超速报警之后，将标志位清除
		
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
 		g_objPhoto.AddPhoto( PHOTOEVT_OVERSPEED );
#endif
	}
// 	else
// 	{
// 		if( m_dwAlertSymb & ALERT_TYPE_OVERSPEED )
// 		{
// 			m_dwAlertSymb &= ~ALERT_TYPE_OVERSPEED;
// 
// 			if( 0 == m_dwAlertSymb && m_objMonStatus.m_bAlertAuto )
// 			{
// 				_EndMon(); // 若是报警激发的,关闭监控
// 
// 				char buf[ 200 ] = { 0 };
// 				int iBufLen = 0;
// 				BYTE bytEndType = 0x04; // 因为报警信号终止
// 				int iResult = g_objSms.MakeSmsFrame((char*)&bytEndType, 1, 0x01, 0x44, buf, sizeof(buf), iBufLen);
// 				if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );
// 			}
// 		}
// 	}

	return 0;
}

int CMonAlert::SetAlertBelowSpd( const bool v_bSet )
{
	int iRet = 0;

	tag2QAlertCfg objAlertCfg;
 	::GetSecCfg( &objAlertCfg, sizeof(objAlertCfg),
 		offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );

	if( v_bSet && !(objAlertCfg.m_bytAlert_2 & 0x04) )
	{
		m_dwAlertSymb &= ~ALERT_TYPE_BELOWSPEED;
		PRTMSG(MSG_NOR, "Deal BelowSpeed Alerm, but this func is Disabled! Exit!\n" );
		return 0;
	}

	if( v_bSet ) // 若要开始报警
	{
		PRTMSG(MSG_NOR, "Start BelowSpeed Alerm!\n" );

		m_dwAlertSymb |= ALERT_TYPE_BELOWSPEED;

		// 统一报警处理（低速报警不统一处理，因为只报一次）
		//iRet = _DealAlert( false, false );

		// 发送一条低速报警
		tagGPSData objGps(0);
		tag2QGprsCfg objGprsCfg;
		GetCurGPS( objGps, true );
		objGprsCfg.Init();
		::GetSecCfg( &objGprsCfg, sizeof(objGprsCfg), offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );
		_SendOne0149Alert(objGps, ALERM_0149_1);

		m_dwAlertSymb &= ~ALERT_TYPE_BELOWSPEED;	// 超速报警之后，将标志位清除
		
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
 		g_objPhoto.AddPhoto( PHOTOEVT_BELOWSPEED );
#endif
	}
// 	else
// 	{
// 		if( m_dwAlertSymb & ALERT_TYPE_BELOWSPEED )
// 		{
// 			m_dwAlertSymb &= ~ALERT_TYPE_BELOWSPEED;
// 
// 			if( 0 == m_dwAlertSymb && m_objMonStatus.m_bAlertAuto )
// 			{
// 				_EndMon(); // 若是报警激发的,关闭监控
// 
// 				char buf[ 200 ] = { 0 };
// 				int iBufLen = 0;
// 				BYTE bytEndType = 0x04; // 因为报警信号终止
// 				int iResult = g_objSms.MakeSmsFrame((char*)&bytEndType, 1, 0x01, 0x44, buf, sizeof(buf), iBufLen);
// 				if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );
// 			}
// 		}
// 	}

	return 0;
}

int CMonAlert::SetAlertArea( const bool v_bSet )
{
	int iRet = 0;
	
	tag2QAlertCfg objAlertCfg;
	::GetSecCfg( &objAlertCfg, sizeof(objAlertCfg),
		offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );
	
	if( v_bSet && !(objAlertCfg.m_bytAlert_2 & 0x40) )
	{
		m_dwAlertSymb &= ~ALERT_TYPE_RANGE;
		PRTMSG(MSG_NOR, "Deal Area Alerm, but this func is Disabled! Exit!\n" );
		return 0;
	}
	
	if( v_bSet ) // 若要开始报警
	{
		PRTMSG(MSG_NOR, "Start Area Alerm!\n" );
		
		m_dwAlertSymb |= ALERT_TYPE_RANGE;
		
		// 发送一条越界报警
		tagGPSData objGps(0);
		tag2QGprsCfg objGprsCfg;
		GetCurGPS( objGps, true );
		objGprsCfg.Init();
		::GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),
			offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );
		_Snd0145( objGps, objGprsCfg, true );
		
		m_dwAlertSymb &= ~ALERT_TYPE_RANGE;	// 报警之后，将标志位清除

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
 		g_objPhoto.AddPhoto( PHOTOEVT_AREA );
#endif
	}
	
	return 0;
}

/// 现在用于JG2
int CMonAlert::SetAlertOther(DWORD v_dwType, const bool v_bSet)
{
	if(v_bSet)
	{
		m_dwAlertSymb |= v_dwType;	// 设置该报警状态
		_DealAlert( false, false );
	}
	else
	{
		if( m_dwAlertSymb & v_dwType )
		{
			m_dwAlertSymb &= ~v_dwType;

			if( 0 == m_dwAlertSymb )  // 若清除后没有任何报警
			{
				if( m_objMonStatus.m_bAlertAuto )
				{
					_EndMon(); // 若是报警激发的,关闭监控

					char buf[ 200 ] = { 0 };
					int iBufLen = 0;
					BYTE bytEndType = 0x04; // 因为报警信号终止
					int iResult = g_objSms.MakeSmsFrame((char*)&bytEndType, 1, 0x01, 0x44, buf, sizeof(buf), iBufLen);
					if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );
				}
			}
		}
	}

	return 1;
}

int CMonAlert::SetAlertTest()
{
	_KillTimer(&g_objTimerMng, ALERT_TEST_TIMER );
	_SetTimer(&g_objTimerMng, ALERT_TEST_TIMER, ALERT_TEST_PERIOD, G_TmAlertTest );
	m_dwAlertSymb |= ALERT_TYPE_TEST;
	return 0;
}

int CMonAlert::_BeginAutoReqLsn( DWORD v_dwBeginInterval, char* v_szLsnTel/*=NULL*/ )
{
	// 设置监听号码
	memset( m_szAutoReqLsnTel, 0, sizeof(m_szAutoReqLsnTel) );
	if( !v_szLsnTel )
	{
		tag2QServCodeCfg objLsnCfg;
		objLsnCfg.Init();
 		GetSecCfg( &objLsnCfg, sizeof(objLsnCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objLsnCfg) );

		strncpy( m_szAutoReqLsnTel, (char*)&(objLsnCfg.m_aryLsnHandtel[0]), sizeof(m_szAutoReqLsnTel) - 1 );
	}
	else
	{
		strncpy( m_szAutoReqLsnTel, v_szLsnTel, sizeof(m_szAutoReqLsnTel) - 1 );
	}
	m_szAutoReqLsnTel[ 14 ] = 0;
	char* pBlank = strchr( m_szAutoReqLsnTel, 0x20 );
	if( pBlank ) *pBlank = 0;

	// 清除监听状态并启动监听
	m_bytAutoReqLsnCount = 0;
	m_bytAutoReqLsnContBusyCt = 0;
	_KillTimer(&g_objTimerMng, AUTO_REQLSN_TIMER );
	_SetTimer(&g_objTimerMng, AUTO_REQLSN_TIMER, v_dwBeginInterval, G_TmAutoReqLsn ); // 多少时间之后启动监听

	return 0;
}

int CMonAlert::_AutoReqLsn()
{
	// 保险起见
	m_szAutoReqLsnTel[ sizeof(m_szAutoReqLsnTel) - 1 ] = 0;
	WORD wTelLen = WORD( strlen(m_szAutoReqLsnTel) );
	if( wTelLen > 20 || wTelLen < 3 ) return ERR_AUTOLSN_NOTEL;
	
	char buf[ 100 ] = { 0 };
	buf[ 0 ] = BYTE(0xf1);
	buf[ 1 ] = BYTE( wTelLen );
	memcpy( buf + 2, m_szAutoReqLsnTel, wTelLen );

	if( DataPush(buf, 2+wTelLen, DEV_QIAN, DEV_PHONE, LV2) ) 
		return ERR_AUTOLSN;

	return 0;
}

bool CMonAlert::JugFootAlert()
{
	return m_dwAlertSymb & ALERT_TYPE_FOOT ? true : false;
}

DWORD CMonAlert::GetAlertSta()
{
	return m_dwAlertSymb;
}

/*
void CMonAlert::FillAlertSymb( const DWORD v_dwAlertSta, BYTE &v_byt1, BYTE &v_byt2 )
{
// 千里眼通用版,公开版_研三
// 高字节：
// 0	A1	A2	E1	E2	E3	V	1
// 低字节：
// 0	C	K	S1	S2	S3	T	1
// A1，A2：外接报警器状态
// 00：正常
// 01：震动报警
// 10：非法打开车门报警
// 11：保留待定
// E1，E2，E3：越界报警器状态（本组报警状态适合于危险车辆）
// 000：正常
// 001：在规定的时间段以外行驶
// 010：超速行驶
// 011：停留时间过长
// 100：越界行驶
// 其它保留待定
// V：欠压报警
// 0：正常       1：欠压
// C：断路报警
//     0：正常       1：断路
// K：抢劫报警
// 0：正常       1：抢劫
// T：测试标志位
// 0：正常       1：测试
// S1，S2，S3：保留待定

// 千里眼公开版_研一
// 高字节：
// 0	A1	A2	E1	E2	E3	V	1
// 低字节：
// 0	C	K	S1	S2	S3	T	1
// A1，A2：外接报警器状态
// 00：正常
// 01：震动报警
// 10：非法打开车门报警
// 11：保留待定
// E1：越界行驶标志
// 0：正常
// 1：越界行驶
// E2：超速行驶标志
// 0：正常
// 1：超速行驶
// E3：在规定时间段外行驶标志
// 0：正常
// 1：在规定时间段外行驶
// S1：非法发动车辆标志
// 0：正常
// 1：非法发动车辆
// V：欠压报警
// 0：正常       1：欠压
// C：断路报警
//     0：正常       1：断路
// K：抢劫报警
// 0：正常       1：抢劫
// T：测试标志位
// 0：正常       1：测试
// S2，S3：保留待定


	v_byt1 = 1, v_byt2 = 1;

	if( ALERT_TYPE_POWBROWNOUT & v_dwAlertSta ) // 欠压报警
		v_byt1 |= 0x02;
	if( ALERT_TYPE_TIME & v_dwAlertSta ) // 时间段外行驶报警
		v_byt1 |= 0x04;
	if( ALERT_TYPE_RSPEED & v_dwAlertSta ) // 区域超速报警(越界并超速)
	{
		v_byt1 |= 0x08;
		v_byt1 |= 0x10;
	}
	if( ALERT_TYPE_RANGE & v_dwAlertSta ) // 越界报警
		v_byt1 |= 0x10;
	if( ALERT_TYPE_OVERTURN & v_dwAlertSta ) // 侧翻报警
		v_byt1 |= 0x20;

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
	if( ALERT_TYPE_FRONTDOOR & v_dwAlertSta ) // 非法开车门报警
		v_byt1 |= 0x40;
#endif

#if USE_PROTOCOL == 30
	if( ALERT_TYPE_BUMP & v_dwAlertSta ) // 碰撞报警
		v_byt1 |= 0x40;
#endif

	if( ALERT_TYPE_DRIVE & v_dwAlertSta ) // 非法启动车辆
		v_byt2 |= 0x10;
	if( ALERT_TYPE_OVERSPEED & v_dwAlertSta ) // 超速报警
		v_byt2 |= 0x08;
	if( ALERT_TYPE_FOOT & v_dwAlertSta ) // 抢劫报警
		v_byt2 |= 0x20;
	if( ALERT_TYPE_POWOFF & v_dwAlertSta ) // 断电报警
		v_byt2 |= 0x40;
	if( ALERT_TYPE_TEST & v_dwAlertSta ) // 报警测试
		v_byt2 |= 0x02;
}
*/

void CMonAlert::FillAlertSymb( const DWORD v_dwAlertSta, BYTE *v_byt1, BYTE *v_byt2 )
{
// 千里眼通用版,公开版_研三
// 高字节：
// 0	A1	A2	E1	E2	E3	V	1
// 低字节：
// 0	C	K	S1	S2	S3	T	1
// A1，A2：外接报警器状态
// 00：正常
// 01：震动报警
// 10：非法打开车门报警
// 11：保留待定
// E1，E2，E3：越界报警器状态（本组报警状态适合于危险车辆）
// 000：正常
// 001：在规定的时间段以外行驶
// 010：超速行驶
// 011：停留时间过长
// 100：越界行驶
// 其它保留待定
// V：欠压报警
// 0：正常       1：欠压
// C：断路报警
//     0：正常       1：断路
// K：抢劫报警
// 0：正常       1：抢劫
// T：测试标志位
// 0：正常       1：测试
// S1，S2，S3：保留待定

// 千里眼公开版_研一
// 高字节：
// 0	A1	A2	E1	E2	E3	V	1
// 低字节：
// 0	C	K	S1	S2	S3	T	1
// A1，A2：外接报警器状态
// 00：正常
// 01：震动报警
// 10：非法打开车门报警
// 11：保留待定
// E1：越界行驶标志
// 0：正常
// 1：越界行驶
// E2：超速行驶标志
// 0：正常
// 1：超速行驶
// E3：在规定时间段外行驶标志
// 0：正常
// 1：在规定时间段外行驶
// S1：非法发动车辆标志
// 0：正常
// 1：非法发动车辆
// V：欠压报警
// 0：正常       1：欠压
// C：断路报警
//     0：正常       1：断路
// K：抢劫报警
// 0：正常       1：抢劫
// T：测试标志位
// 0：正常       1：测试
// S2，S3：保留待定


	*v_byt1 = 1, *v_byt2 = 1;

	if( ALERT_TYPE_POWBROWNOUT & v_dwAlertSta ) // 欠压报警
		*v_byt1 |= 0x02;
	if( ALERT_TYPE_TIME & v_dwAlertSta ) // 时间段外行驶报警
		*v_byt1 |= 0x04;
	if( ALERT_TYPE_RSPEED & v_dwAlertSta ) // 区域超速报警(越界并超速)
	{
		*v_byt1 |= 0x08;
		*v_byt1 |= 0x10;
	}
	if( ALERT_TYPE_RANGE & v_dwAlertSta ) // 越界报警
		*v_byt1 |= 0x10;
	if( ALERT_TYPE_OVERTURN & v_dwAlertSta ) // 侧翻报警
		*v_byt1 |= 0x20;

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
	if( (ALERT_TYPE_FRONTDOOR & v_dwAlertSta) || (ALERT_TYPE_BACKDOOR & v_dwAlertSta) ) // 非法开车门报警
		*v_byt1 |= 0x40;
#endif

#if USE_PROTOCOL == 30
	if( ALERT_TYPE_BUMP & v_dwAlertSta ) // 碰撞报警
		*v_byt1 |= 0x40;
#endif

	if( ALERT_TYPE_DRIVE & v_dwAlertSta ) // 非法启动车辆
		*v_byt2 |= 0x10;
	if( ALERT_TYPE_OVERSPEED & v_dwAlertSta ) // 超速报警
		*v_byt1 |= 0x08;
	if( ALERT_TYPE_FOOT & v_dwAlertSta ) // 抢劫报警
		*v_byt2 |= 0x20;
	if( ALERT_TYPE_POWOFF & v_dwAlertSta ) // 断电报警
		*v_byt2 |= 0x40;
	if( ALERT_TYPE_TEST & v_dwAlertSta ) // 报警测试
		*v_byt2 |= 0x02;
}

void CMonAlert::FillAlertSymbFor0149( const DWORD v_dwAlertSta, BYTE *v_byt1, BYTE *v_byt2 )
{
// 千里眼通用版,公开版_研三
// 高字节：
// A15 A14	A13	A12	A11	A10	A9 A8
// 低字节：
// A7  A6  A5	A4	A3	A2	A1 A0
// A0，A1：
// 00：正常
// 01：碰撞报警
// 10：侧翻报警
// A2：
// 1：低速报警
// 0：正常


	*v_byt1 = 0, *v_byt2 = 0;

	if( ALERT_TYPE_BUMP & v_dwAlertSta ) // 碰撞报警
		*v_byt2 |= 0x02;
	if( ALERT_TYPE_OVERTURN & v_dwAlertSta ) // 侧翻报警
		*v_byt2 |= 0x01;
	if( ALERT_TYPE_BELOWSPEED & v_dwAlertSta ) // 低速报警
		*v_byt2 |= 0x04;
}

bool CMonAlert::IsMonAlert()
{
	return (m_objMonStatus.m_bInterv || m_objMonStatus.m_bSpace) && m_objMonStatus.m_dwMonTime >= 1000;
}

/// 监控请求 （应答成功优先）
int CMonAlert::Deal0101(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen, bool v_bFromSM, bool v_bSendRes/*=true*/)
{
// 通用版,公开版_研三
// 监控业务类型（1）+ 监控时间（2）+ 监控周期（2）+ 定距距离（2）

// 公开版_研一
// 监控业务类型（1）+ 监控时间（2）+ 监控周期（2）+ 定距距离（2）+ 上传GPS数据总条数(2)

	PRTMSG(MSG_NOR, "Rcv Cent Mon Req\n" );
	
	tag0141 res;
	tagMonStatus objMonStatus;
	tagGPSData objGps(0);
	int iRet = 0;
	int iBufLen = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytAnsType = 0x01;

	if( v_dwDataLen < sizeof(tag0101) )
	{
		PRTMSG(MSG_ERR, "Deal0101 error 1, v_dwDatalen = %d, sizeof(tag0101) = %d\n", v_dwDataLen, sizeof(tag0101));
		XUN_ASSERT_VALID( false, "" );
		return ERR_DATA_INVALID;
	}
	
	tag0101 req;
	memcpy( &req, v_szData, sizeof(req) );

	BYTE bytTemp = req.m_bytMonType & 0x18;

	// 解析参数，设置监控条件
	objMonStatus = m_objMonStatus; // 先用原参数初始化
	if( 0 == bytTemp || 0x10 == bytTemp ) // 有定时
	{
		if( 0x7f == req.m_bytIntervMin ) req.m_bytIntervMin = 0;
		if( 0x7f == req.m_bytIntervSec ) req.m_bytIntervSec = 0;

		if( req.m_bytIntervMin > 126 )
		{
			PRTMSG(MSG_ERR, "Deal0101 error 2\n");
			iRet = ERR_DATA_INVALID;
			goto DEAL0101_FAIL;
		}
		if( req.m_bytIntervSec > 59 )
		{
			PRTMSG(MSG_ERR, "Deal0101 error 3\n");
			iRet = ERR_DATA_INVALID;
			goto DEAL0101_FAIL;
		}

		objMonStatus.m_bInterv = true;
		objMonStatus.m_dwMonInterv = (req.m_bytIntervMin * 60 + req.m_bytIntervSec) * 1000 ;
		if( objMonStatus.m_dwMonInterv < 1000 ) objMonStatus.m_dwMonInterv = 1000; // 保险起见
	}
	else
	{
		objMonStatus.m_bInterv = false;
	}
	if( 0x08 == bytTemp || 0x10 == bytTemp ) // 有定距
	{
		if( 0x7f == req.m_bytSpaceKilo ) req.m_bytSpaceKilo = 0;
		if( 0x7f == req.m_bytSpaceTenmeter ) req.m_bytSpaceTenmeter = 0;

		if( req.m_bytSpaceKilo > 126 )
		{
			PRTMSG(MSG_ERR, "Deal0101 error 4\n");
			iRet = ERR_DATA_INVALID;
			goto DEAL0101_FAIL;
		}
		if( req.m_bytSpaceTenmeter > 99 )
		{
			PRTMSG(MSG_ERR, "Deal0101 error 5\n");
			iRet = ERR_DATA_INVALID;
			goto DEAL0101_FAIL;
		}

		objMonStatus.m_bSpace = true;
		objMonStatus.m_dwSpace = req.m_bytSpaceKilo * 1000 + req.m_bytSpaceTenmeter * 10;
		if( objMonStatus.m_dwSpace < 10 ) objMonStatus.m_dwSpace = 10;
	}
	else
	{
		objMonStatus.m_bSpace = false;
	}
	if( 0x18 == bytTemp ) // 这个判断重要,防止不正确的监控参数,导致不停的异常监控处理
	{
		PRTMSG(MSG_ERR, "Deal0101 error 6\n");
		iRet = ERR_DATA_INVALID;
		goto DEAL0101_FAIL; // 无效的更新方式
	}

	// 监控时间
	if( 0x7f == req.m_bytTimeHour ) req.m_bytTimeHour = 0;
	if( 0x7f == req.m_bytTimeMin ) req.m_bytTimeMin = 0;
	if( 0 == req.m_bytTimeHour && 0 == req.m_bytTimeMin )
	{
		objMonStatus.m_dwMonTime = INVALID_NUM_32; // (注意哦)
		m_objMonStatus.m_bAlertAuto = false;
	}
	else
	{
		DWORD dwMonTime = (req.m_bytTimeHour * DWORD(60) + req.m_bytTimeMin) * 60 * 1000;
		if( dwMonTime > objMonStatus.m_dwMonTime ) // 若新监控时间超过了原有的监控时间(可能是报警时间)
			m_objMonStatus.m_bAlertAuto = false;
		objMonStatus.m_dwMonTime = dwMonTime;
	}

#if 2 == USE_PROTOCOL
	// GPS数据个数
	if( 0x7f == req.m_bytUpGpsHund ) req.m_bytUpGpsHund = 0;
	if( 0x7f == req.m_bytUpGpsCount ) req.m_bytUpGpsCount = 0;
	if( 0 == req.m_bytUpGpsHund && 0 == req.m_bytUpGpsCount )
		objMonStatus.m_dwGpsCount = 0xffffffff; // (注意哦)
	else
	{
		if( req.m_bytUpGpsHund > 126 || req.m_bytUpGpsCount > 99 )
		{
			PRTMSG(MSG_ERR, "Deal0101 error 7\n");
			iRet = ERR_DATA_INVALID;
			goto DEAL0101_FAIL;
		}
		objMonStatus.m_dwGpsCount = req.m_bytUpGpsHund * 100 + req.m_bytUpGpsCount;
	}
#endif

#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	objMonStatus.m_dwGpsCount = 0xffffffff;
#endif

	// 目的手机号
	if( 0x20 != v_szSrcHandtel[0] && 0 != v_szSrcHandtel[0] && '0' != v_szSrcHandtel[0] )
		memcpy( objMonStatus.m_szMonHandTel, v_szSrcHandtel, sizeof(objMonStatus.m_szMonHandTel) );

	// 发送监控成功应答
	if( v_bSendRes )
	{
		tagQianGps objQianGps;

		res.m_bytAnsType = 0x01;
		iRet = GetCurGPS( objGps, true );
		if( iRet ) 
			goto DEAL0101_FAIL;

		GpsToQianGps( objGps, objQianGps );
		memcpy( &(res.m_objQianGps), &objQianGps, sizeof(res.m_objQianGps) );

		iRet = g_objSms.MakeSmsFrame( (char*)&res, sizeof(res), 0x01, 0x41, buf, sizeof(buf), iBufLen );		
		if( iRet ) 
			goto DEAL0101_FAIL;

		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV13, v_bFromSM ? DATASYMB_USESMS : 0, v_szSrcHandtel );		
		if( iRet )
			goto DEAL0101_FAIL;
	}

	m_objMonStatus = objMonStatus; // 监控参数更新到内存中的常驻状态
	_BeginMon( objGps.longitude, objGps.latitude );

	return 0;

DEAL0101_FAIL:
	// 发送监控失败应答
	if( v_bSendRes )
	{
		bytAnsType = 0x7f;
		int iRet2 = g_objSms.MakeSmsFrame((char*)&bytAnsType, 1, 0x01, 0x41, buf, sizeof(buf), iBufLen);
		if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_szSrcHandtel );
	}

	return iRet;
}

/// 撤销监控 (撤销优先)
int CMonAlert::Deal0102(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen, bool v_bFromSM)
{
	PRTMSG(MSG_NOR, "Rcv Cent Stop Mon Req\n" );
	
	int iRet = 0;
	int iBufLen = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytResType = 1;
	iRet = g_objSms.MakeSmsFrame((char*)&bytResType, 1, 0x01, 0x42, buf, sizeof(buf), iBufLen);
	if( !iRet ) 
	{
		g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV11, v_bFromSM ? DATASYMB_USESMS : 0, v_szSrcHandtel );
	}
	
	_EndMon();
	
	PRTMSG(MSG_NOR, "Stop Mon Succ\n" );
	
	return iRet;
}

/// 修改监控参数 （业务处理优先）
int CMonAlert::Deal0103(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_NOR, "Rcv Cent Modify Mon Par Req\n" );
	
	if( sizeof(tag0103) != v_dwDataLen ) return ERR_DATA_INVALID;

	tag2QGprsCfg objGprsCfg;
 	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	tag0103 req;
	memcpy( &req, v_szData, sizeof(req) );

	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	BYTE bytResType;

	int iRet = Deal0101( v_szSrcHandtel, (char*)&(req.m_objO101), sizeof(req.m_objO101), false );
	if( !iRet )
	{
		bytResType = 0x01;
		iRet = g_objSms.MakeSmsFrame((char*)&bytResType, 1, 0x01, 0x43, buf, sizeof(buf), iBufLen);
		if( !iRet ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, (objGprsCfg.m_bytChannelBkType_1 & 0x40) ? DATASYMB_SMSBACKUP : 0 );
	}
	else goto DEAL0103_FAILED;
		
	PRTMSG(MSG_NOR, "Modify mon par Succ\n" );

	return 0;

DEAL0103_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame((char*)&bytResType, 1, 0x01, 0x43, buf, sizeof(buf), iBufLen);
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12,
		(objGprsCfg.m_bytChannelBkType_1 & 0x40) ? DATASYMB_SMSBACKUP : 0 );
	return iRet;
}

/// 报警处理指示 （业务处理优先）
int CMonAlert::Deal0105(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen)
{
	int iRet = 0;
	int iBufLen = 0;
	char buf[ SOCK_MAXSIZE ];

	bool bNeedSendRes = false; // 注意初始化的值是false
	bool bDealSucc = false; // 注意初始化的值是false

	tag2QGprsCfg objGprsCfg;
 	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	if( v_dwDataLen < 1 ) return ERR_DATA_INVALID;

	switch( BYTE(*v_szData) )
	{
	case 0x01: // 默认处理
		{
			PRTMSG(MSG_NOR, "Rcv Cent Alerm Res: Default\n" );

			if( m_dwAlertSymb & ALERT_TYPE_TEST )
			{
				m_dwAlertSymb &= ~ALERT_TYPE_TEST; // 清除报警测试标记
				_KillTimer(&g_objTimerMng, ALERT_TEST_TIMER );

				// 发送测试成功通知到调度屏
				char buf[3] = { 0x01, 0x43, 0x01 };
				DataPush(buf, sizeof(buf), DEV_QIAN, DEV_DIAODU, LV2);

				_EndMon(); // 关闭监控
			}
			
			bDealSucc = true;
		}
		break;

	case 0x02: // 撤销报警
		{
			PRTMSG(MSG_NOR, "Rcv Cent Alerm Res: Stop Alerm\n" );
			
			bNeedSendRes = true;

			if( m_objMonStatus.m_bAlertAuto ) // 若是报警激发的,关闭监控
				_EndMon();
			else // 否则,清除报警标志 (监控仍继续)
				_ClrAlertSta();
			bDealSucc = true;
		}
		break;

	case 0x03: // 修改监控参数
		{
			PRTMSG(MSG_NOR, "Rcv Cent Alerm Res: Modify Mon Par\n" );
			
			bNeedSendRes = true;

			if( sizeof(tag0105) != v_dwDataLen )
				break;

			tag0105 req;
			memcpy( &req, v_szData, sizeof(req) );
			iRet = Deal0101( v_szSrcHandtel, (char*)&(req.m_obj0101), sizeof(req.m_obj0101), false );
			if( !iRet ) bDealSucc = true;
		}
		break;
		
	default:
		;
	}

	// 收到报警处理指示,则关闭静默报警定时
	_KillTimer(&g_objTimerMng, ALERT_SILENT_TIMER );

	if( bDealSucc )
	{
		BYTE bytAnsType = 0x01;

		iRet = g_objSms.MakeSmsFrame( (char*)&bytAnsType, 1, 0x01, 0x52, buf, sizeof(buf), iBufLen );
		if( !iRet ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, DATASYMB_SMSBACKUP );
	}
	else goto DEAL0105_FAILED;
		
	PRTMSG(MSG_NOR, "Deal Cent Alerm Res Succ\n" );

	return 0;

DEAL0105_FAILED:
	BYTE bytAnsType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytAnsType, 1, 0x01, 0x52, buf, sizeof(buf), iBufLen );
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, DATASYMB_SMSBACKUP );

	return iRet;
}

/// 位置查询请求 (应答成功优先)
int CMonAlert::Deal0111( char* v_szSrcHandtel, bool v_bFromSM )
{
	tagQianGps objQianGps;
	tag0151 res;
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	int iRet = 0;

	tag2QGprsCfg objGprsCfg;
	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg), offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	iRet = GetCurQianGps( objQianGps, true );
	if( !iRet )
	{
		res.m_bytAnsType = 0x01;
		memcpy( &(res.m_objQianGps), &objQianGps, sizeof(res.m_objQianGps) );
		iRet = g_objSms.MakeSmsFrame((char*)&res, sizeof(res), 0x01, 0x51, buf, sizeof(buf), iBufLen);
		if( !iRet )
		{
			iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12,
 				(objGprsCfg.m_bytChannelBkType_1 & 0x10) ? DATASYMB_SMSBACKUP : 0 );
			iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_szSrcHandtel );
			if( iRet ) 
				goto DEAL0111_FAILED;

			// GPS模块上电 (其实只有以后再位置查询,才可能收到有效GPS)
 			//g_objQianIO.PowerOnGps( true );
 			//g_objQianIO.StartAccChk( 300000 );
		}
		else goto DEAL0111_FAILED;
	}
	else goto DEAL0111_FAILED;

	return 0;

DEAL0111_FAILED:
	BYTE bytAnsType = 0x7f;

	int iRet2 = g_objSms.MakeSmsFrame((char*)&bytAnsType, 1, 0x01, 0x51, buf, sizeof(buf), iBufLen);
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12,
		(objGprsCfg.m_bytChannelBkType_1 & 0x10) ? DATASYMB_SMSBACKUP : 0 );
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_szSrcHandtel );
	
	return iRet;
}


// 指令监听请求 公开版_研一
int CMonAlert::Deal0720( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen )
{
// 电话号码串（15）

	int iRet = 0;
	char szLsnTel[20] = { 0 };
	char buf[300] = { 0 };
	int iBufLen = 0;
	char* pStop = NULL;
	BYTE bytResType = 1;

	if( v_dwDataLen < 15 )
	{
		iRet = ERR_PAR;
		goto DEAL0720_FAILED;
	}

	strncpy( szLsnTel, v_szData, 15 );
	szLsnTel[ sizeof(szLsnTel) - 1 ] = 0;
	pStop = strchr( szLsnTel, 0x20 );
	if( pStop ) *pStop = 0;
	iRet = _BeginAutoReqLsn( 3000, szLsnTel );
	if( iRet ) goto DEAL0720_FAILED;

	bytResType = 1;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x07, 0x60, buf, sizeof(buf), iBufLen );
	if( !iRet )
	{
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV11, DATASYMB_SMSBACKUP, v_szSrcHandtel );
	}

	return 0;

DEAL0720_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x07, 0x60, buf, sizeof(buf), iBufLen );
	if( !iRet2 ) iRet2 = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV11, DATASYMB_SMSBACKUP );

	return iRet;
}

// 指令监听请求 千里眼通用版,公开版_研三
int CMonAlert::Deal0723( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen )
{
// 电话号码长度（1）+ 电话号码（N≤15）

	char szLsnTel[20] = { 0 };
	char buf[300];
	int iBufLen = 0;
	int iRet = 0;
	BYTE bytResType = 1;
	BYTE bytTelLen = 0;

	if( v_dwDataLen < 2 )
	{
		iRet = ERR_PAR;
		goto DEAL0723_FAILED;
	}

	bytTelLen = v_szData[0];
	if( bytTelLen > 15 || bytTelLen < 1 || bytTelLen > sizeof(szLsnTel) )
	{
		iRet = ERR_PAR;
		goto DEAL0723_FAILED;
	}

	if( v_dwDataLen < DWORD(1 + bytTelLen) )
	{
		iRet = ERR_PAR;
		goto DEAL0723_FAILED;
	}

	strncpy( szLsnTel, v_szData + 1, bytTelLen );
	szLsnTel[ sizeof(szLsnTel) - 1 ] = 0;
	iRet = _BeginAutoReqLsn( 3000, szLsnTel );
	if( iRet ) goto DEAL0723_FAILED;

	bytResType = 1;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x07, 0x63, buf, sizeof(buf), iBufLen );
	if( !iRet )
	{
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV11, DATASYMB_SMSBACKUP, v_szSrcHandtel );
	}

	return 0;

DEAL0723_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x07, 0x63, buf, sizeof(buf), iBufLen );
	if( !iRet2 ) iRet2 = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV11, DATASYMB_SMSBACKUP );

	return iRet;
}


/// 报警设置 （应答成功优先）
int CMonAlert::Deal1009( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
	tag2QAlertCfg obj2QAlertCfg, obj2QAlertCfgBkp;
	int iRet = 0;
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	BYTE bytResType = 0;

	// 数据参数检查
	if( 2 != v_dwDataLen )
	{
		iRet = ERR_PAR;
		goto DEAL1009_FAILED;
	}

	// 读取->修改
	iRet = GetSecCfg( &obj2QAlertCfg, sizeof(obj2QAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(obj2QAlertCfg) );
	if( iRet ) goto DEAL1009_FAILED;
	obj2QAlertCfgBkp = obj2QAlertCfg; // 先备份
	obj2QAlertCfg.m_bytAlert_1 = BYTE(v_szData[0] & 0x7f);
	obj2QAlertCfg.m_bytAlert_2 = BYTE(v_szData[1] & 0x7f);

	// 写回和发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x49, buf, sizeof(buf), iBufLen); 
	if( !iRet )
	{
		if( iRet = SetSecCfg(&obj2QAlertCfg, offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(obj2QAlertCfg)) )
		{
			goto DEAL1009_FAILED;
		}
		
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL ); // 发送
		if( iRet ) // 若发送失败
		{
			SetSecCfg( &obj2QAlertCfgBkp, offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(obj2QAlertCfgBkp) ); // 恢复原设置
			goto DEAL1009_FAILED;
		}
	}
	else goto DEAL1009_FAILED;

	return 0;

DEAL1009_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x49, buf, sizeof(buf), iBufLen); 
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );

	return iRet;
}

/// 状态查询
int CMonAlert::Deal101a(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen )
{
// 千里眼通用版,公开版_研三
// 状态字（2）+ 标准GPS数据
// 填充状态字
// ——VM1、JG2、JG3协议定义
// Bit  15~8: 0	1	T1	T2	T3	T4	T5	0
// Bit  7~0:  0	1	D1	D2	D3	D4	0	0
// T1-报警号码设置位，      1：设置；0：未设置
// T2-调度号码设置位，      1：设置；0：未设置
// T3-税控号码设置位，      1：设置；0：未设置
// T4-短信中心号码设置位，  1：设置；0：未设置
// T5-抢劫报警设置位，      1：允许；0：禁止
// T6-GPS模块状态			1：正常  0：异常
// D1-与调度终端通讯状态位，1：正常；0：异常
// D2-税控时间设置位，      1：正常；0：异常
// D3-传呼号码设置位，      1：正常；0：异常
// D4-黑匣子采集时间设置位，1：正常；0：异常

// KJ2协议通用版
// 状态字（2）+传感状态（1）+ GPS数据
// 状态字
// Bit  15~8: 0	1 T1 T2	1 T4 T5	T6
// Bit  7~0:  0	1  1  1	1  1 T7	 1
// T1-报警号码设置位，      1：设置；0：未设置
// T2－调度号码设置位，      1：设置；0：未设置
// T4-短信中心号码设置位，  1：设置；0：未设置
// T5-抢劫报警设置位，      1：允许；0：禁止
// T6-GPS模块状态位，      1：正常；0：异常 (是关电吗?)
// T7-调度屏状态位，        1：正常；0：异常
// 其它保留


// 千里眼公开版_研一
// 状态字（2）+传感状态（1）+ GPS数据
// 状态字:
// Bit  15~8: 0	1 T1  1	1 T4 T5	T6
// Bit  7~0:  0	1  1  1	1  1 T7	 1
// T1-报警号码设置位，      1：设置；0：未设置
// T4-短信中心号码设置位，  1：设置；0：未设置
// T5-抢劫报警设置位，      1：允许；0：禁止
// T6-GPS模块状态位，      1：正常；0：异常
// T7-调度屏状态位，        1：正常；0：异常


	int iRet = 0;
	int iBufLen = 0;
	tag105a res;
	tagQianGps objQianGps;
	char buf[ 500 ];

	memset( &res, 0, sizeof(res) );
	res.m_bytSta_1 = 0x40; // 注意,这样填是因为各版本的最高2位都是01
	res.m_bytSta_2 = 0x40; // 注意,这样填是因为各版本的最高2位都是01

	{
		tag2QServCodeCfg objServCodeCfg;
		GetSecCfg( &objServCodeCfg, sizeof(objServCodeCfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(objServCodeCfg) );
		
		if( objServCodeCfg.m_szQianSmsTel[0] && 0x20 != objServCodeCfg.m_szQianSmsTel[0] )
			res.m_bytSta_1 |= 0x20; // 报警号码(特服号)设置
	
		if( objServCodeCfg.m_szDiaoduHandtel[0] && 0x20 != objServCodeCfg.m_szDiaoduHandtel[0] )
			res.m_bytSta_1 |= 0x10; // 调度号码设置

#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
		if( objServCodeCfg.m_szTaxHandtel[0] && 0x20 != objServCodeCfg.m_szTaxHandtel[0] ) res.m_bytSta_1 |= 0x08; // 税控号码设置
#endif
	}
	{
		tag1PComuCfg objComuCfg;
		GetImpCfg( &objComuCfg, sizeof(objComuCfg),	offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) );
		
		if( objComuCfg.m_szSmsCentID[0] && 0x20 != objComuCfg.m_szSmsCentID[0] )
			res.m_bytSta_1 |= 0x04; // 短信中心号设置
	}

	{
		tag2QAlertCfg objAlertCfg;
		GetSecCfg( &objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg) );
	
		if( objAlertCfg.m_bytAlert_1 & 0x20 ) 
			res.m_bytSta_1 |= 0x02; // 抢劫报警设置位
	}

	{
		if( g_objQianIO.GetDeviceSta() & DEVSTA_GPSON ) res.m_bytSta_1 |= 0x01; // GPS模块状态
	}

	{
		DWORD dwComuSta;
		dwComuSta = g_objComu.GetComuSta();
#if 2 == USE_PROTOCOL || 30 == USE_PROTOCOL
		if( dwComuSta & COMUSTA_DIAODUCONN ) res.m_bytSta_2 |= 0x02; // 调度屏状态
#endif
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
		if( dwComuSta & COMUSTA_DIAODUCONN ) res.m_bytSta_2 |= 0x20; // 调度屏状态
#endif

#if USE_BLK == 1 && (0 == USE_PROTOCOL || 1 == USE_PROTOCOL)
		res.m_bytSta_2 |= 0x04; // 黑匣子采集间隔固定,只要启动黑匣子功能
#endif
	}
	
#if 30 == USE_PROTOCOL
	// 传感器状态
	// 0	FB	HB	LL	RL	FD	BD	A
	// A -ACC信号状态,			1：ON； 0：OFF
	// BD-后车门状态，			1：有效；0：无效
	// FD－前车门状态，          1：有效；0：无效
	// RL－右转向灯，            1：有效；0：无效
	// LL－左转向灯，            1：有效；0：无效
	// HB－手刹，                1：有效；0：无效
	// FB－脚刹，                1：有效；0：无效
	{
		DWORD dwIoSta = 0;
		if( g_objQianIO.IN_QueryIOSta(dwIoSta) )
		{
			if( !(dwIoSta & 0x00000001) ) res.m_bytIO |= 0x01;
			if( !(dwIoSta & 0x00000004) ) res.m_bytIO |= 0x02;
			if( !(dwIoSta & 0x00000008) ) res.m_bytIO |= 0x04;
			if( !(dwIoSta & 0x00000010) ) res.m_bytIO |= 0x08;
			if( !(dwIoSta & 0x00000020) ) res.m_bytIO |= 0x10;
			if( !(dwIoSta & 0x00000040) ) res.m_bytIO |= 0x20;
			if( !(dwIoSta & 0x00000080) ) res.m_bytIO |= 0x40;
		}
	}
#endif

#if 2 == USE_PROTOCOL
	// 传感器状态
	// Bit   7       ->             0
	// 0	1	0	0	0	0	D	A
	// A-ACC信号状态,			1：ON； 0：OFF
	// D-车门状态，				1：关闭；0：打开

	{
		DWORD dwIoSta = 0;
		if( g_objQianIO.IN_QueryIOSta(dwIoSta) )
		{
			if( !(dwIoSta & 0x00000001) ) res.m_bytIO |= 0x01;
			if( dwIoSta & 0x00000008 ) res.m_bytIO |= 0x02;
		}
	}
#endif

	// 标准GPS数据
	iRet = GetCurQianGps( res.objQianGps, true );
	if( iRet ) goto DEAL101A_FAILED;
	else memcpy( &res.objQianGps, &objQianGps, sizeof(res.objQianGps) );

	iRet = g_objSms.MakeSmsFrame( (char*)&res, sizeof(res), 0x10, 0x5a, buf, sizeof(buf), iBufLen);
	if( !iRet )
	{
		// 发送应答
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, DATASYMB_SMSBACKUP );
		if( iRet ) goto DEAL101A_FAILED;
	}
	else goto DEAL101A_FAILED;

	return 0;

DEAL101A_FAILED:
	return iRet;
}

// 超速报警设置
int CMonAlert::Deal103d( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
// 公开版协议_研一
// 报警速度	1	单位：海里/小时, 取值范围：0～7EH，0转义成7FH
// 连续超速时间	1	单位：秒
	
	int iRet = 0;

#if 2 == USE_PROTOCOL

	DrvCfg objDrvCfg;

	BYTE bytResType = 0;
	BYTE bytAlermSpd = 0;
	BYTE bytAlermPrid = 0;
	char buf[100] = { 0 };
	int iBufLen = 0;

	if( v_dwDataLen < 2 )
	{
		iRet = ERR_PAR;
		goto DEAL103D_FAILED;
	}

	bytAlermSpd = BYTE( v_szData[0] );
	if( bytAlermSpd > 0x7f )
	{
		iRet = ERR_PAR;
		goto DEAL103D_FAILED;
	}
	if( 0x7f == bytAlermSpd ) bytAlermSpd = 0;

	bytAlermPrid = BYTE( v_szData[1] );

	iRet = GetSecCfg( &objDrvCfg, sizeof(objDrvCfg), offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered), sizeof(objDrvCfg) );
	if( iRet ) goto DEAL103D_FAILED;

	objDrvCfg.max_speed = bytAlermSpd;
	objDrvCfg.min_over	= bytAlermPrid;

	iRet = SetSecCfg( &objDrvCfg, offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered), sizeof(objDrvCfg) );
	if( iRet ) goto DEAL103D_FAILED;

	bytResType = 1;
	goto DEAL103D_END;

DEAL103D_FAILED:
	bytResType = 0x7f;

DEAL103D_END:
	if( !g_objSms.MakeSmsFrame((char*)&bytResType, sizeof(bytResType), 0x10, 0x7d, buf, sizeof(buf), iBufLen) )
	{
		g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );
	}

#endif

	return iRet;
}


///区域查车
// 查询索引号（1）+ 监控终端ID（8）＋区域个数（1）+【区域（18）】×N
int CMonAlert::Deal0136( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen )
{
	tag0176 res;
	tag0136 *p = (tag0136*)v_szData;
	char buf[500] = {0};
	int iBufLen = 0;
	int i, r1, r2, r3, r4;
	bool in_zone = false;
	int iRet = GetCurQianGps( res.m_objQianGps, true );
	if( iRet ) goto DEAL0136_FAILED;

	// 判断当前经纬度是否在所给的区域内
	for(i=0; i<p->m_bytZoneCount; i++) 
	{
		r1 = memcmp(p->m_Zone[i].lat1, &res.m_objQianGps.m_bytLatDu, 4);
		r2 = memcmp(p->m_Zone[i].lat2, &res.m_objQianGps.m_bytLatDu, 4);

		// 将经度最高位放回度字节中，方便判断
		if(p->m_Zone[i].hbit1 & 0x02)	p->m_Zone[i].lon1[0] |= 0x80;
		if(p->m_Zone[i].hbit2 & 0x02)	p->m_Zone[i].lon2[0] |= 0x80;
		if(res.m_objQianGps.m_bytMix & 0x02)	res.m_objQianGps.m_bytLonDu |= 0x80; 

		r3 = memcmp(p->m_Zone[i].lon1, &res.m_objQianGps.m_bytLonDu, 4);
		r4 = memcmp(p->m_Zone[i].lon2, &res.m_objQianGps.m_bytLonDu, 4);

		if( r1<=0 && r2>=0 && r3<=0 && r4>=0 )	{ in_zone = true; break; }
	}
	
	if(in_zone) 
	{
		memcpy(&res.m_bytQueryIndex, &p->m_bytQueryIndex, 10);
		res.m_objQianGps.m_bytLonDu &= ~0x80;	//清空经度的度字节最高位

		iRet = g_objSms.MakeSmsFrame( (char*)&res, sizeof(res), 0x01, 0x76, buf, sizeof(buf), iBufLen);
		if( !iRet ) {
			// 发送应答
			iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, DATASYMB_SMSBACKUP );
			if( iRet ) goto DEAL0136_FAILED;
		} else goto DEAL0136_FAILED;
	} 

	return 0;

DEAL0136_FAILED:
	return iRet;
}


