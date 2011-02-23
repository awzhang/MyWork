#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-Comu     ")

void G_TmChkDiaodu(void *arg, int len)
{
	g_objComu.P_TmChkDiaodu();
}

CComu::CComu()
{
	m_dwComuSta = 0;
}

CComu::~CComu()
{

}

// 调度终端查询登陆中心状态
int CComu::_DealComu0b( char* v_szData, DWORD v_dwDataLen )
{
	char buf[3];
	buf[0] = 0x01;
	buf[1] = BYTE( 0x0c );

#if USE_LIAONING_SANQI == 0
	buf[2] = g_objSock.IsOnline() ? BYTE(1) : BYTE(2);
#endif
#if USE_LIAONING_SANQI == 1
	buf[2] = g_objSock.m_bDVRTcpLoged && g_objLogin.IsOnline() ? BYTE(1) : BYTE(2);
#endif
	DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);

	m_dwComuSta |= COMUSTA_DIAODUCONN;
	_SetTimer(&g_objTimerMng, CHKDIAODU_TIMER, 30000, G_TmChkDiaodu );
	
	return 0;
}

/// CFG_GPRS网络恢复通知（待讨论）
int CComu::_DealComu35( char* v_szData, DWORD v_dwDataLen )
{
	return 0;
}

// 千里眼短信接收通知
int CComu::_DealComu37( char* v_szData, DWORD v_dwDataLen )
{
	PRTMSG(MSG_NOR, "Rcv ShortMsg Data From Comu\n" );

	DWORD dwPacketNum = 0;
	int iRet = g_objRecvSmsDataMng.PushData( LV10, v_dwDataLen, v_szData, dwPacketNum, DATASYMB_RCVFROMSM );

	if( iRet )	
	{
		PRTMSG(MSG_ERR, "Push ShortMsg Data To Queue Failed, because %d\n", iRet );
	}
	
	return iRet;
}

int CComu::_DealComu41( char* v_szData, DWORD v_dwDataLen )
{
	int iRet = 0;
	if( v_dwDataLen < 1 ) return ERR_PAR;

	switch( BYTE(v_szData[0]) )
	{
	case 1: // 报警测试
		{
			iRet = g_objMonAlert.SetAlertTest();
			
			if( !iRet )
			{
				char buf[] = { 0x01, 0x42, 0x01 };
				DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);
			}
		}
		break;

	default:
		;
	}

	return iRet;
}

// 速度查询请求
int CComu::_DealComu61( char* v_szData, DWORD v_dwDataLen )
{
	double dSpeed = 0; // 米/秒
	BYTE bytAnsType = 0; // 应答类型
	bool bGetSpd = false;

#ifdef DIS_SPD_USEIO
	DWORD dwCyclePerSec = 0;
	WORD wCyclePerKm = 0;
	GetImpCfg( &wCyclePerKm, sizeof(wCyclePerKm), offsetof(tagImportantCfg, m_uni1QBiaodingCfg.m_obj1QBiaodingCfg.m_wCyclePerKm), sizeof(wCyclePerKm) );
	
	if( wCyclePerKm > 50 && g_objQianIO.IN_QueryIOSpeed(dwCyclePerSec) )
	{
		dSpeed = dwCyclePerSec * (1000.0 / wCyclePerKm); // 注意保证wCyclePerKm大于0,以防止除0异常
		bytAnsType = 1;
		bGetSpd = true;
	}
#endif

	if( !bGetSpd )
	{
		tagGPSData objGps(0);
		objGps.Init();
		g_objMonAlert.GetCurGPS( objGps, FALSE );

		dSpeed = objGps.speed;
		if( 'A' == objGps.valid ) bytAnsType = 1;
		else bytAnsType = 2;
	}

	char buf[16] = {0};
	buf[0] = 0x01;
	buf[1] = 0x60;
	buf[2] = bytAnsType;
	buf[3] = BYTE( dSpeed * MIMIAO_TO_HAIHOUR );
	DataPush(buf, 4, DEV_QIAN, DEV_DIAODU, LV2);

	return 0;
}

/// 调度终端发送调度内容请求
int CComu::_DealComu70( char* v_szData, DWORD v_dwDataLen )
{
	int iRet = 0;
	int iBufLen = 0;
	BYTE bytResType = 0;
	char buf[ 300 ] = { 0 };

	tag2QGprsCfg objGprsCfg;
	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	if( 0 == v_dwDataLen || sizeof(buf) < v_dwDataLen )
	{
		bytResType = 2;
		goto DEALCOMU70_END;
	}
	
	iRet = g_objSms.MakeSmsFrame( v_szData, v_dwDataLen, 0x05, 0x41, buf, sizeof(buf), iBufLen );
	if( iRet )
	{
		bytResType = 2;
		goto DEALCOMU70_END;
	}

	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9,
		(objGprsCfg.m_bytChannelBkType_1 & 0x02) ? DATASYMB_SMSBACKUP : 0 );
	if( iRet )
	{
		bytResType = 2;
		goto DEALCOMU70_END;
	}

	bytResType = 1;

DEALCOMU70_END:
	buf[ 0 ] = 0x01;
	buf[ 1 ] = BYTE( 0x71 );
	buf[ 2 ] = bytResType;
	DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);

	return iRet;
}

/// 抢答信息(来自调度屏)
// 与调度屏协议: 无数据内容
int CComu::_DealComu7a( char* v_szData, DWORD v_dwDataLen )
{
	int iRet = 0;
	int iBufLen = 0;
	BYTE bytResType = 0;
	char buf[ 200 ] = { 0 };

	tag2QGprsCfg objGprsCfg;
	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );
	
	/// 发送电召应答(即抢答信息)到中心
	// 与中心协议: 无数据内容
	iRet = g_objSms.MakeSmsFrame( NULL, 0, 0x05, 0x7c, buf, sizeof(buf), iBufLen, CVT_8TO6 );
	if( iRet )
	{
		bytResType = 0;
		goto DEALCOMU7A_END;
	}
	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9,
		(objGprsCfg.m_bytChannelBkType_1 & 0x02) ? DATASYMB_SMSBACKUP : 0 );
	if( iRet )
	{
		bytResType = 0;
		goto DEALCOMU7A_END;
	}

	PRTMSG(MSG_NOR, "Rcv From Diaodu: Dianzao qiangda, Send To Cent\n" );

	bytResType = 1;

DEALCOMU7A_END:

	// 发送确认应答到调度屏
	buf[ 0 ] = 0x01;
 	buf[ 1 ] = BYTE( 0x76 );
	buf[ 2 ] = bytResType;
	DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);

	return iRet;
}

/// 电召结果报告(来自调度屏)
// 与调度屏协议: 电召结果(1) 电召结果：0x00：失败； 0x01：成功；
int CComu::_DealComu7b( char* v_szData, DWORD v_dwDataLen )
{	
	int iRet = 0;
	int iBufLen = 0;
	BYTE bytResType = 0;
	char buf[ 200 ] = { 0 };

	tag2QGprsCfg objGprsCfg;
	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	if( 0 == v_dwDataLen || v_dwDataLen < 1 )
	{
		iRet = ERR_PAR;
		goto DEALCOMU7B_END;
	}

	/// 发送电召结果报告应答到中心
	// 与中心协议: 电召结果(1)  电召结果：0x00：失败； 0x01：成功；
	iRet = g_objSms.MakeSmsFrame( v_szData, v_dwDataLen, 0x05, 0x7e, buf, sizeof(buf), iBufLen, CVT_8TO6 );
	if( iRet )
	{
		goto DEALCOMU7B_END;
	}

	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9,
		(objGprsCfg.m_bytChannelBkType_1 & 0x02) ? DATASYMB_SMSBACKUP : 0 );
	if( iRet )
	{
		goto DEALCOMU7B_END;
	}

	PRTMSG(MSG_NOR, "Rcv from diaodu:Dianzao result,Send to Cent\n" );

	bytResType = 1;

DEALCOMU7B_END:
	// 发送确认应答到调度屏
	buf[ 0 ] = 0x01;
	buf[ 1 ] = BYTE( 0x76 );
	buf[ 2 ] = bytResType;
	DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);

		
#if USE_LEDTYPE == 2
		g_objLedChuangLi.DealEvent(0x04, 'B', 3);	// 不论电召成功与否，都去掉LED屏的“电召”显示
#endif

	return iRet;
}


// 调度请求车台发送紧急求助:	求助类型（1）
// 求助类型：01000TTT
// TTT：求助类型
// 000-医疗求助
// 001-交通求助
// 010-纠纷
// 其它无效
//  
// int CComu::_DealComu80( char* v_szData, DWORD v_dwDataLen )
// {
// 	return g_objHelp.DealComu80(v_szData, v_dwDataLen);
// }

// 调度收到车台转发的中心求助响应后，给车台应答: 应答类型（1）01H-正确,02H-错误
int CComu::_DealComu82( char* v_szData, DWORD v_dwDataLen )
{
	return 0;
}

// 调度请求车台发送取消求助	求助类型（1）
int CComu::_DealComu84( char* v_szData, DWORD v_dwDataLen )
{
	return 0;
}

// 调度收到车台转发的中心取消求助响应后，给车台应答: 应答类型（1）01H-正确,02H-错误
int CComu::_DealComu86( char* v_szData, DWORD v_dwDataLen )
{
	return 0;
}

/// 摄像头检测请求（去除）
int CComu::_DealComuE0( char* v_szData, DWORD v_dwDataLen )
{
	return 0;
}

/// 数据导出请求（去除）
int CComu::_DealComuE2()
{
	return 0;
}

/// 程序窗口切换请求（去除）
int CComu::_DealComuEE( char* v_szData, DWORD v_dwDataLen )
{
	switch(v_szData[0])
	{
	case 0xdd:
		g_objCarSta.ResumeDrvSta();
		break;

	default:
		break;
	}
	return 0;
}

/// 车台状态变化通知
int CComu::_DealComuF0( char* v_szData, DWORD v_dwDataLen )
{
	if( 1 != v_dwDataLen ) return ERR_PAR;
	static bool sta_bFst = true;

	DWORD dwVeSta = g_objCarSta.GetVeSta();
	switch( BYTE(v_szData[0]) ) 
	{
	case 0x01: // 报告移动
		{
			if( sta_bFst || !(VESTA_MOVE & dwVeSta) )
			{
				PRTMSG(MSG_NOR, "Rcv Vehicle Move Msg\n" );

				dwVeSta |= VESTA_MOVE; // 先——更新新状态
				g_objCarSta.SetVeSta( dwVeSta );
				
#if USE_AUTORPT == 1
				//g_objAutoRpt.InitAutoRpt(); // 再——重新初始化主动上报
				g_objAutoRpt.BeginAutoRpt(1);
#endif

				sta_bFst = false;

#if USE_LEDTYPE == 1
				//070903 hong 车辆运动状态变化，车辆移动，LED亮屏
				if (g_objLedBohai.m_bytLedCfg & 0x02)
				{
					g_objLedBohai.m_bytVeSta = 2;
					g_objLedBohai.light();
					PRTMSG(MSG_NOR, "Light on LED\n");
				}
#endif
			}
		}
		break;
		
	case 0x02: // 报告静止
		{
			if( sta_bFst || (VESTA_MOVE & dwVeSta) )
			{
				PRTMSG(MSG_NOR, "Rcv Vehicle Stop Msg\n" );

				dwVeSta &= ~VESTA_MOVE; // 先——更新新状态
				g_objCarSta.SetVeSta( dwVeSta );
				
#if USE_AUTORPT == 1
				//g_objAutoRpt.InitAutoRpt(); // 再——重新初始化主动上报
				g_objAutoRpt.BeginAutoRpt(2);
#endif

				sta_bFst = false;

#if USE_LEDTYPE == 1
				//070903 hong 车辆运动状态变化，车辆静止，LED延时黑屏
				if (g_objLedBohai.m_bytLedCfg&0x02)
				{
					g_objLedBohai.m_bytVeSta = 1;
					g_objLedBohai.m_dwVeStaTick = GetTickCount();
					PRTMSG(MSG_NOR, "Black LED when LED is on stall\n");
				}
#endif
			}
		}
		break;
		
	default:
		;
	}

	return 0;
}

/// 拨打监听电话应答
int CComu::_DealComuF2( char* v_szData, DWORD v_dwDataLen )
{
	if( v_dwDataLen < 1 ) return ERR_PAR;

	return g_objMonAlert.DealPhoneAutoReqLsnRes( BYTE(v_szData[0]) );
}

//（去除）
int CComu::_DealComuF3( char* v_szData, DWORD v_dwDataLen )
{
	return 0;
}

void CComu::P_TmChkDiaodu()
{
	m_dwComuSta &= ~COMUSTA_DIAODUCONN;
}

DWORD CComu::GetComuSta()
{
	return m_dwComuSta;
}

void CComu::DealComuMsg(char *v_szbuf, DWORD v_dwlen, BYTE  v_bytSrcSymb)
{
	switch( v_bytSrcSymb )
	{
	case DATASYMB_DIAODUTOQIAN:
		switch( BYTE(v_szbuf[0]) )
		{
		case 0x0b: // 查询登陆中心状态
			_DealComu0b( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x35: // GPRS网络状态通知 (修改配置后的通知)
			_DealComu35( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x41: // 报警测试和修车
			_DealComu41( v_szbuf + 1, v_dwlen - 1 );
			break;

#if USE_DRIVERCTRL == 1
		case 0x63: // 请求司机登录应答
			g_objDriverCtrl.DealComu63( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x65: // 请求司机编号
			g_objDriverCtrl.DealComu65( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x6C: // 司机登出指示应答
			g_objDriverCtrl.DealComu6c( v_szbuf + 1, v_dwlen - 1 );
			break;
#endif
			
			
#if USE_DRVREC == 1	// JG2行车记录仪调度屏协议
		case 0x61: // 请求速度
		case 0x63: // 请求司机登录应答
		case 0x65: // 请求司机编号
		case 0x67: // 请求载重传感器数据
		case 0x69: // 请求传感器状态
		case 0x6B: // 请求文本信息显示应答
		case 0x6C: // 司机登出指示应答
		case 0x6D: // 车辆特征系数指令
		case 0x6F: // 总里程相关指令
			g_objDriveRecord.hdl_diaodu((byte*)v_szbuf, v_dwlen);
			break;
#endif

#if USE_OIL == 3
		case 0xC1:
		case 0xC3:
		case 0xC4:
		case 0x51:
			g_objOil.DealDiaoDu(v_szbuf + 1, v_dwlen - 1);
			break;
#endif
			
#if USE_DRVREC == 0
		case 0x61: // 请求速度
			_DealComu61( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x6D: // 车辆特征系数指令
			g_objCfg.DealComu6d( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x67: // 请求载重传感器数据
		case 0x69: // 请求传感器状态
		case 0x6B: // 请求文本信息显示应答
			break;
#endif
			
		case 0x70: // 请求发送调度数据
			_DealComu70( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x7a: // 抢答信息(来自调度屏)
			_DealComu7a( v_szbuf + 1, v_dwlen - 1 );
			//凯天新电召功能
			//g_objDiaodu.Send0550();
			break;
			
		case 0x7b: // 电召结果报告(来自调度屏)
			_DealComu7b( v_szbuf + 1, v_dwlen - 1 );
			//g_objDiaodu.DealComu7B(v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x80: // 请求发送求助
			g_objHelp.DealComu80( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x82: // 收到中心求助响应的应答
			_DealComu82( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x84: // 请求发送撤销求助
			_DealComu84( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x86: // 收到中心撤销求助的应答
			_DealComu86( v_szbuf + 1, v_dwlen - 1 );
			break;

#if USE_AUTORPTSTATION == 1
		case 0x8e: // 报站器业务
			g_objAuoRptStationNew.DealComu8e( v_szbuf + 1, v_dwlen - 1 );
			break;
#endif
			
		case 0xe0: // 摄像头调试指示
			_DealComuE0( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0xe2: // 数据导出请求
			_DealComuE2();
			break;
			
		case 0xee: // 切换控制等
			_DealComuEE( v_szbuf + 1, v_dwlen - 1);
			break;

		//凯天触摸屏协议
		case 0x3c:
			g_objDiaodu.DealComu3C(v_szbuf + 1, v_dwlen - 1);
			break;

#if USE_METERTYPE == 2
		case 0x48:
			g_objKTMeter.DealComu48(v_szbuf + 1, v_dwlen - 1);
			break;
		case 0x4a:
			g_objKTMeter.DealComu4A(v_szbuf + 1, v_dwlen - 1);
			break;
#endif

		case 0x73://调度信息车辆指示应答
			g_objDiaodu.DealComu73(v_szbuf + 1, v_dwlen - 1);
			break;
		case 0xe9://设备检测--应需获得ComuExe中的GPS和GPRS状态，故传过来时转为E9
			g_objKTDeviceCtrl.DealComuAD(v_szbuf, v_dwlen);
			break;
		case 0xd5://电子服务证更新请求应答
			g_objKTDriverCtrl.DealComuD5(v_szbuf + 1, v_dwlen - 1);
			break;
		case 0xd7://请求照片传输的应答
			g_objKTDriverCtrl.DealComuD7(v_szbuf + 1, v_dwlen - 1);
			break;
		case 0xd9://照片数据传输应答
			g_objKTDriverCtrl.DealComuD9(v_szbuf + 1, v_dwlen - 1);
			break;
		case 0xda://请求电子服务证
			g_objKTDriverCtrl.DealComuDA(v_szbuf + 1, v_dwlen - 1);
			break;
		case 0xdb://请求交接班
			g_objKTDriverCtrl.DealComuDB(v_szbuf + 1, v_dwlen - 1);
				break;

		case 0xfe:	//ComuExe通知要升级root，退出所有相关Flash操作
			{
				_KillTimer(&g_objTimerMng, BLACK_TIMER);
				_KillTimer(&g_objTimerMng, DRIVERECORD_TIMER);
				g_objDriveRecord.m_bDrvRecQuit = true;
			}
			break;

#if USE_COMBUS == 1
		case 0xff:	// 使用串口扩展盒，向调度屏或手柄发送数据时，必须由CComBus转发
			{
				DWORD dwPacketNum;
				g_objComBus.m_objComBusWorkMng.PushData((BYTE)LV1, v_dwlen-1, v_szbuf+1, dwPacketNum);
			}
			break;
#endif

#if USE_COMBUS == 2
		case 0xff:	// 使用150TR行驶记录仪，向调度屏或手柄发送数据时，必须由CCom150TR转发
			{
				DWORD dwPacketNum;
				g_objCom150TR.m_objCom150TRWorkMng.PushData((BYTE)LV1, v_dwlen-1, v_szbuf+1, dwPacketNum);
			}
			break;
#endif

		default:
			break;
		}
	break;
		
	case DATASYMB_PHONETOQIAN:
		switch( BYTE(v_szbuf[0]) )
		{				
		case 0x35: // GPRS网络状态通知
			_DealComu35( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0x37: // 接收短信通知
			_DealComu37( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0xf2: // 拨打监听电话应答
			_DealComuF2( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		case 0xf3: // 手机模块类型通知
			_DealComuF3( v_szbuf + 1, v_dwlen - 1 );
			break;
			
		default:
			break;
		}
		break;
		
	case DATASYMB_GPSTOQIAN:
		switch( BYTE(v_szbuf[0]) )
		{
		case 0xf0: // 车辆状态通知
			_DealComuF0( v_szbuf + 1, v_dwlen - 1 );
			break;

		case 0xf7:	// GPS进入省电通知
			PRTMSG(MSG_DBG, "Gps into sleep!\n");
			g_objQianIO.SetDevSta( false, DEVSTA_GPSON );
			break;

		case 0xf8:	// GPS退出省电通知
			PRTMSG(MSG_DBG, "Gps out sleep!\n");
			g_objQianIO.SetDevSta( true, DEVSTA_GPSON );
			break;
				
		default:
			break;
		}
		break;
			
	default:
			break;
	}
}


