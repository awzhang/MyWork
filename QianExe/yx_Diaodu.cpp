#include "yx_QianStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-Diaodu   ")

void G_TmSetBespokeProc(void *arg, int arg_len)
{
	g_objDiaodu.P_TmSetBespokeProc();
}
void G_TmBespokeNotProc(void *arg, int arg_len)
{
	g_objDiaodu.P_TmBespokeNotProc();
}

CDiaodu::CDiaodu()
{

}

CDiaodu::~CDiaodu()
{

}

/// 中心调度指示
int CDiaodu::Deal0501( char* v_szData, DWORD v_dwDataLen )
{
	int iResult; 
	char buf[ 500 ] = { 0 };

	if( 0 == v_dwDataLen ) 
		return ERR_PAR;

	if( v_dwDataLen + 1 > sizeof(buf) )
		return ERR_SPACELACK;
	
	// 发送调度内容到调度屏
	buf[ 0 ] = 0x01;
	buf[ 1 ] = BYTE(0x70);
	memcpy( buf + 2, v_szData, v_dwDataLen );
	iResult = DataPush(buf, WORD(v_dwDataLen + 2), DEV_QIAN, DEV_DIAODU, LV2);

#if USE_TTS == 1 && USE_IRD == 0
	memset(buf, 0, sizeof(buf));
	
	int ilen = Code6To8(v_szData, buf, v_dwDataLen, sizeof(buf));

	//调度帧格式如下
	//长度		校验码	路由信息	数据类型	用户数据
	//1 Byte	1 Byte	4 Byte		1 Byte		0~100 Byte

	char  szbuf[1024] = {0};
	DWORD dwlen = 0;
	
	szbuf[dwlen++] = 0x01;	// 0x01表示TTS数据
	szbuf[dwlen++] = 0x00;	// 0x01表示立即播放本数据

	if( buf[6] == 0x02 || buf[6] == 0x08 )
	{
		memcpy(szbuf+dwlen, "收到调度信息：", strlen("收到调度信息："));
		dwlen += strlen("收到调度信息：");
		memcpy(szbuf+dwlen, buf+10, ilen-10);
		dwlen += (ilen - 10);
	}
	else if( buf[6] == 0x00 || buf[6] == 0x04 )
	{
		memcpy(szbuf+dwlen, "收到广播信息：", strlen("收到广播信息："));
		dwlen += strlen("收到广播信息：");
		memcpy(szbuf+dwlen, buf+9, ilen-9);
		dwlen += (ilen - 9);
	}	

	iResult = DataPush(szbuf, dwlen, DEV_QIAN, DEV_DVR, LV2);
#endif

	return iResult;
}

/// 电召指令(简短电召指令)
// 协议:电召范围(16)(左下角位置8+右上角位置8) 位置(纬度4+经度4)
int CDiaodu::Deal053c( char* v_szData, DWORD v_dwDataLen )
{
	int iRet = 0;

	PRTMSG(MSG_DBG, "dianzhao:\n");
	PrintString(v_szData, v_dwDataLen);

	// 若现在是重车，不处理电召
	DWORD dwVeSta = g_objCarSta.GetVeSta();
	if( dwVeSta & VESTA_HEAVY )
	{
		PRTMSG(MSG_DBG, "now is heavy, donot deal dianzhao!\n");
		return 0;
	}

	// 判断输入参数长度
	if( v_dwDataLen < 16 ) 
	{
		PRTMSG(MSG_DBG, "053c len err!\n");
		return ERR_PAR;
	}

	// 获取当前位置
	tagGPSData objGps(0);
	iRet = g_objMonAlert.GetCurGPS( objGps, false );
	if( iRet ) return iRet;
	if( 'A' != objGps.valid )
	{
		PRTMSG(MSG_DBG, "gps is invalid, donot deal dianzhao\n" );
		return ERR_GPS_INVALID;
	}
	// 经纬度数据转换
	for( DWORD dw = 0; dw < 16; dw ++ )
	{
		if( 0x7f == v_szData[dw] )
			v_szData[dw] = 0;
	}

	// 判断是否在召车范围内
	long lMinLon = 0, lMinLat = 0, lMaxLon = 0, lMaxLat = 0;
	lMinLat = long( ( BYTE(v_szData[0]) + ( BYTE(v_szData[1]) + BYTE(v_szData[2]) / 100.0 + BYTE(v_szData[3]) / 10000.0 ) / 60.0 ) * LONLAT_DO_TO_L );
	lMinLon = long( ( BYTE(v_szData[4]) + ( BYTE(v_szData[5]) + BYTE(v_szData[6]) / 100.0 + BYTE(v_szData[7]) / 10000.0 ) / 60.0 ) * LONLAT_DO_TO_L );
	lMaxLat = long( ( BYTE(v_szData[8]) + ( BYTE(v_szData[9]) + BYTE(v_szData[10]) / 100.0 + BYTE(v_szData[11]) / 10000.0 ) / 60.0 ) * LONLAT_DO_TO_L );
	lMaxLon = long( ( BYTE(v_szData[12]) + ( BYTE(v_szData[13]) + BYTE(v_szData[14]) / 100.0 + BYTE(v_szData[15]) / 10000.0 ) / 60.0 ) * LONLAT_DO_TO_L );
	if( !JugPtInRect( long(objGps.longitude * LONLAT_DO_TO_L), long(objGps.latitude * LONLAT_DO_TO_L),
		lMinLon, lMinLat, lMaxLon, lMaxLat ) )
	{
		PRTMSG(MSG_DBG, "Not in the dianzhao area! \n");
		PRTMSG(MSG_DBG, "(%d,%d)\n", long(objGps.longitude * LONLAT_DO_TO_L), long(objGps.latitude * LONLAT_DO_TO_L) );
		PRTMSG(MSG_DBG, "(%d,%d)\n", lMinLon, lMinLat );
		PRTMSG(MSG_DBG, "(%d,%d)\n", lMaxLon, lMaxLat );
		return 0;
	}

	// 发送到调度屏 (无数据内容)
	char buf[ 100 ] = { 0 };
	buf[ 0 ] = 0x01;
	buf[ 1 ] = BYTE(0x74);

	iRet = DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);;

	PRTMSG(MSG_NOR, "Rcv From Cent: Dianzao,Send to Diaodu\n" );

	return iRet;
}

/// 抢答确认(详细电召指令)
// 乘客电话(15)+车牌号(15)+描述信息(50) 
int CDiaodu::Deal053d( char* v_szData, DWORD v_dwDataLen )
{
	int iRet = 0;
	char szSelfTel[15] = { 0x20 };
	char buf[ 200 ] = { 0 };

	if( v_dwDataLen < 80 ) return ERR_PAR;

	// 发送到调度屏 (乘客电话(15)+车牌号(15)+描述信息(50)+本机手机号（15）)
	buf[0] = 0x01;
	buf[1] = BYTE(0x75);
	memcpy( buf + 2, v_szData, 80 );
	iRet = g_objCfg.GetSelfTel( szSelfTel, sizeof(szSelfTel) );
	if( iRet ) return iRet;
	memcpy( buf + 2 + 80, szSelfTel, 15 ); // 增加本机手机号

	iRet = DataPush(buf, WORD(2 + v_dwDataLen), DEV_QIAN, DEV_DIAODU, LV2);

	if( !iRet )
	{
		// 成功发送到调度屏后,还要发应答给中心
		int iBufLen = 0;
		iRet = g_objSms.MakeSmsFrame( NULL, 0, 0x05, 0x7d, buf, sizeof(buf), iBufLen );

		if( !iRet ) iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, 0 );
	}

#if USE_LEDTYPE == 2
	g_objLedChuangLi.DealEvent(0x03, 'B', 3);
#endif

	PRTMSG(MSG_NOR, "Rcv from Cent:Qiangda queren,send to Diaodu\n" );

	return iRet;
}

void CDiaodu::Init()
{
	tagTaxiCfg objTaxiCfg;
	memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
	GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg),
	offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	if(objTaxiCfg.m_aryBespokeInf[0]!=0xff&&objTaxiCfg.m_aryBespokeInf[1]!=0xff)
	{
		printf("Begin Check Bespoke:%d:%d",objTaxiCfg.m_aryBespokeInf[0],objTaxiCfg.m_aryBespokeInf[1]);
		if(objTaxiCfg.m_aryBespokeInf[0]==0xee&&objTaxiCfg.m_aryBespokeInf[1]==0xee)
		{
			
			//顶灯显示预约状态
			_KillTimer(&g_objTimerMng, SETBESPOKE_TIMER);
#if USE_LEDTYPE == 3
			g_objKTLed.SetLedShow('B',0,30*60*1000);
#endif
			//10分钟进行一次提醒
			P_TmBespokeNotProc();
			_SetTimer(&g_objTimerMng, BESPOKENOT_TIMER,10*60*1000,G_TmBespokeNotProc);

		}
		else if(objTaxiCfg.m_aryBespokeInf[0]<=24&&objTaxiCfg.m_aryBespokeInf[1]<=60&&
			objTaxiCfg.m_aryBespokeInf[0]>0&&objTaxiCfg.m_aryBespokeInf[1]>0)
		{
				//一分种检查一次预约电招
			_SetTimer(&g_objTimerMng, SETBESPOKE_TIMER,60000,G_TmSetBespokeProc);
		}
		else 
		{
			objTaxiCfg.m_aryBespokeInf[0]=0xff;
			objTaxiCfg.m_aryBespokeInf[1]=0xff;
			SetSecCfg(&objTaxiCfg,offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
		}
	}
}

void CDiaodu::LedTest()
{
	byte ledTest[]={0x39,0x42 ,0x30 ,0x42 ,0x32 ,0x44 ,0x45 ,0x41 ,0x43 ,0x44 ,0x30 ,0x39
		,0x33 ,0x34 ,0x33 ,0x42 ,0x33 };
	
	byte ledTest2[]={0x39 ,0x24 ,0x4d ,0x50 ,0xe9 ,0x51 ,0xf4 ,0x9d ,0xde ,0x67 ,0x3f ,0x4e ,0xba 
		,0xee ,0x84 ,0x18 ,0x61 };
	printf("rcv led Test ");

#if USE_LEDTYPE == 3
	g_objKTLed.SetBackText('D',ledTest2,1);
#endif
}

void CDiaodu::DzTest()
{
	printf("diaodu test");
	static int iTick=0;
	switch(iTick)
	{
		case 0:
			{
				frm0510 frm;
				frm.type=0x01;
				frm.preTime[0]=0x13;
				frm.preTime[1]=0x01;
				
				sprintf(frm.fromAdd,"%s","厦门雅迅网络");
				sprintf(frm.endAdd,"%s","股份有限公司");

				char buf1[ 200 ] = { 0 };
				buf1[0]=0x01;
				buf1[1]=0x74;
				memcpy( buf1 + 2, &frm, sizeof(frm) );
				DataPush(buf1, (DWORD)(sizeof(frm)+2), DEV_QIAN, DEV_DIAODU, LV2);

				char buf2[100] = { 0 };
				buf2[0] = 6;
				if(0x01==frm.type)
				{
					strcat( buf2, "请注意,有预约信息请抢答" );
					DataPush(buf2, (DWORD)(strlen(buf2) + 1), DEV_QIAN, DEV_LED, LV2);
				}
				else if(0x00==frm.type)
				{
					strcat( buf2, "请注意,有电招信息请抢答" );
					DataPush(buf2, (DWORD)(strlen(buf2) + 1), DEV_QIAN, DEV_LED, LV2);
				}

			}
			iTick++;
			break;
		case 1:
			{
				frm0511 frm;
				frm.type=0x01;
				frm.preTime[0]=0x13;
				frm.preTime[1]=0x01;
				frm.earlyTime=0x01;
			
				sprintf(frm.tel,"%s","12345678901");
				sprintf(frm.name,"%s","王老六");
				sprintf(frm.fromAdd,"%s","成都凯天");
				sprintf(frm.endAdd,"%s","通信实业");
				char buf1[ 200 ] = { 0 };
				buf1[0]=0x01;
				buf1[1]=0x75;
				memcpy( buf1 + 2, &frm, sizeof(frm) );
				DataPush(buf1, (DWORD)(sizeof(frm)+2), DEV_QIAN, DEV_DIAODU, LV2);

				switch(frm.type)
				{
					case 0x00://普通电召
						{
#if USE_LEDTYPE == 3
							g_objKTLed.SetLedShow('B',3,30*60*1000);
#endif
							char buf[100] = { 0 };
							buf[0] = 6;
							strcat( buf, "抢答成功,请注意电招信息" );
							DataPush(buf, (DWORD)(strlen(buf) + 1), DEV_QIAN, DEV_LED, LV2);
						}
						break;
					case 0x01://预约电召
						if(frm.earlyTime>0)
						{
							UINT interval=frm.earlyTime*60*1000;
							_SetTimer(&g_objTimerMng, SETBESPOKE_TIMER,interval,G_TmSetBespokeProc);
							char buf[100] = { 0 };
							buf[0] = 6;
							strcat( buf, "抢答成功,请注意电招信息" );
							DataPush(buf, (DWORD)(strlen(buf) + 1), DEV_QIAN, DEV_LED, LV2);
						}	
						break;
					case 0x02://急召
						{
#if USE_LEDTYPE == 3
							g_objKTLed.SetLedShow('B',4,30*60*1000);
#endif
							char buf[100] = { 0 };
							buf[0] = 6;
							strcat( buf, "请注意,你车已被急招,请务必前往指定地点接送客人" );
							DataPush(buf, (DWORD)(strlen(buf) + 1), DEV_QIAN, DEV_LED, LV2);
						}
						break;
					default:
						break;
				}

			}
			iTick++;
			break;
		default:
			iTick=0;
			break;
	}
	


}
//凯天一键拨号设置
int CDiaodu::DealComu3C( char* v_szData, DWORD v_dwDataLen )
{
	printf("Rcv Comu3c");
	Frm12 frm;
	if(v_szData[0]==0x04)//报警电话
	{
		frm.len=3;
		sprintf(frm.num,"%s","110");
	}
	else
	{
		frm.len=11;
		//frm.len=5;
		sprintf(frm.num,"%s","02885120180");
		//sprintf(frm.num,"%s","10086");
	}	
	DataPush((char*)(&frm), (DWORD)(sizeof(frm)), DEV_QIAN, DEV_PHONE, LV2);
	return 0;

}
//凯天电招确认
int CDiaodu::DealComu7B( char* v_szData, DWORD v_dwDataLen )
{
	int iRet = 0;
	int iBufLen = 0;
	char buf[ 200 ] = { 0 };
	bool bUpResult=true;
	byte bytCurHour=0;
	byte bytCurMin=0;		
	tagGPSData objGps(0);
	if( 0 == v_dwDataLen || v_dwDataLen < 1 )
	{
		iRet = ERR_PAR;
		goto DEALCOMU7B_END;
	}
	printf("RcvComu7B");
	//检查是否到了预约时间，若还没到预约时间则不让应答
	tagTaxiCfg objTaxiCfg;
	memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
	GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg),
	offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	if((objTaxiCfg.m_aryBespokeInf[0]!=0xff&&objTaxiCfg.m_aryBespokeInf[1]!=0xff)&&
		(objTaxiCfg.m_aryBespokeInf[0]!=0xee&&objTaxiCfg.m_aryBespokeInf[1]!=0xee))

	{
		
		//获取GPS数据
		g_objMonAlert.GetCurGPS( objGps, true );
		bytCurHour=(byte)objGps.nHour;
		bytCurMin=(byte)objGps.nMinute;
		if(bytCurHour<objTaxiCfg.m_aryBespokeInf[0])
			bUpResult=false;
		else if(bytCurHour==objTaxiCfg.m_aryBespokeInf[0])
		{
			if(bytCurMin<objTaxiCfg.m_aryBespokeInf[1])
				bUpResult=false;

		}
		if(!bUpResult)
		{
			char buf[100] = { 0 };
			buf[0] = 6;
			strcat( buf, "未到预约时间,不能确认" );
			DataPush(buf, (DWORD)(strlen(buf) + 1), DEV_QIAN, DEV_LED, LV2);
			return 0;
		}
		
	}
	
	/// 发送电召结果报告应答到中心
	// 与中心协议: 电召结果(1)  电召结果：0x00：失败； 0x01：成功；
	iRet = g_objSms.MakeSmsFrame( v_szData, v_dwDataLen, 0x05, 0x7e, buf, sizeof(buf), iBufLen, CVT_8TO6 );
	if( iRet )
	{
		goto DEALCOMU7B_END;
	}

	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9);
	if( iRet )
	{
		goto DEALCOMU7B_END;
	}

	PRTMSG(MSG_NOR, "Rcv from diaodu:Dianzao result,Send to Cent\n");

DEALCOMU7B_END:
	//凯天电召处理结果
	if(v_szData[0]==0x01) DzCheck(true);
	else DzCheck(false);
	return iRet;

}
//凯天触摸屏车辆指示应答
int CDiaodu::DealComu73( char* v_szData, DWORD v_dwDataLen )
{
	printf("RcvComu73:%d",v_szData[0]);
	int iRet = 0;
	int iBufLen = 0;
	BYTE bytResType = 0;
	char buf[ 300 ] = { 0 };
	char res[100]={0};
	int  resLen=0;
	
	if(v_szData[0]==0x01)
	{
		memcpy(m_objDiaoduInfo.m_szData,"接受",5);

	}
	else memcpy(m_objDiaoduInfo.m_szData,"拒绝",5);
	m_objDiaoduInfo.m_bytLen=6+2+5;
	m_objDiaoduInfo.m_bytDataLen=5;
	m_objDiaoduInfo.m_bytDataType=0x03;
	resLen=7+2+m_objDiaoduInfo.m_bytDataLen;

	memcpy(res,&m_objDiaoduInfo,resLen);
	byte crc=0;
	for(int j=2; j<resLen; j++)	
 		crc += buf[j];	// 计算校验码
	buf[1] = crc;		

	iRet = g_objSms.MakeSmsFrame(res, resLen, 0x05, 0x41, buf, sizeof(buf), iBufLen,CVT_8TO6 );
	if( !iRet )
	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9);
	return iRet;

}
//预约电召--zzg add
int CDiaodu::Deal0510( char* v_szData, DWORD v_dwDataLen )
{
	if(v_dwDataLen<sizeof(frm0510)) return ERR_PAR;

	tagTaxiCfg objTaxiCfg;
	memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
	GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg),
	offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	if(objTaxiCfg.m_aryBespokeInf[0]!=0xff&&objTaxiCfg.m_aryBespokeInf[1]!=0xff)//有预约
	{
		printf("Rcv 0510,But Hav Bespoke");
		return ERR_PAR;
	}
	printf("Rcv 0510");
	memcpy(&m_objDzInfo,v_szData,sizeof(m_objDzInfo));
    //_Send0550();

	char buf1[ 200 ] = { 0 };
	buf1[0]=0x01;
	buf1[1]=0x74;
	memcpy( buf1 + 2, v_szData, v_dwDataLen );
	DataPush(buf1, (DWORD)(v_dwDataLen+2), DEV_QIAN, DEV_DIAODU, LV2);

	char buf2[100] = { 0 };
	buf2[0] = 6;
	if(0x01==m_objDzInfo.type)
	{
		strcat( buf2, "请注意,有预约信息请抢答" );
		DataPush(buf2, (DWORD)(strlen(buf2) + 1), DEV_QIAN, DEV_LED, LV2);
	}
	else if(0x00==m_objDzInfo.type)
	{
		strcat( buf2, "请注意,有电招信息请抢答" );
		DataPush(buf2, (DWORD)(strlen(buf2) + 1), DEV_QIAN, DEV_LED, LV2);
	}
}

//确认抢答
void CDiaodu::Send0550()
{
	printf("Snd 0550");

	char buf2[100] = { 0 };
	buf2[0] = 6;
	strcat( buf2, "您已确认抢答,请等待抢答结果" );
	DataPush(buf2, (DWORD)(strlen(buf2) + 1), DEV_QIAN, DEV_LED, LV2);

	char buf[200]={0};
	int iRet=0;
	int iBufLen = 0;
	iRet = g_objSms.MakeSmsFrame( NULL, 0, 0x05, 0x50, buf, sizeof(buf), iBufLen );
	if( !iRet ) iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, 0 );
}
//点召结果应答
void CDiaodu::DzCheck(bool v_bResult)
{
	//关闭预约电召提醒
	_KillTimer(&g_objTimerMng, SETBESPOKE_TIMER);
	_KillTimer(&g_objTimerMng, BESPOKENOT_TIMER);

#if USE_LEDTYPE == 3
	//关闭B类信息
	g_objKTLed.SetLedShow('B',0xff,0);
#endif

	//语音提示
	char buf[100] = { 0 };
	buf[0] = 6;
	if(v_bResult)
	strcat( buf, "成功接到客人,电招结束" );
	else strcat( buf, "未接到客人,电招结束" );
	DataPush(buf, (DWORD)(strlen(buf) + 1), DEV_QIAN, DEV_LED, LV2);

	tagTaxiCfg objTaxiCfg;
	memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
	GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg),
	offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	objTaxiCfg.m_aryBespokeInf[0]=0xff;
	objTaxiCfg.m_aryBespokeInf[1]=0xff;
	SetSecCfg(&objTaxiCfg,offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));

}
//电召确认--zzg add
int CDiaodu::Deal0511( char* v_szData, DWORD v_dwDataLen )
{
	if(v_dwDataLen<sizeof(frm0511)) return ERR_PAR;
	
	tagTaxiCfg objTaxiCfg;
	memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
	GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg),
	offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	if(objTaxiCfg.m_aryBespokeInf[0]!=0xff&&objTaxiCfg.m_aryBespokeInf[1]!=0xff)//有预约
	{
		printf("Rcv 0511,But Hav Bespoke");
		return ERR_PAR;
	}
		

	printf("Rcv 0511");
	char temp[ 200 ] = { 0 };
	temp[0]=0x01;
	temp[1]=0x75;
	memcpy( temp + 2, v_szData, v_dwDataLen );
	DataPush(temp, (DWORD)(v_dwDataLen+2), DEV_QIAN, DEV_DIAODU, LV2);

	char buf[200]={0};
	int iRet=0;
	frm0511 frm;
	memcpy(&frm,v_szData,sizeof(frm));

	switch(frm.type)
	{
		case 0x00://普通电召
			{
#if USE_LEDTYPE == 3
				g_objKTLed.SetLedShow('B',3,30*60*1000);
#endif

				char buf[100] = { 0 };
				buf[0] = 6;
				strcat( buf, "抢答成功,请注意电招信息" );
				DataPush(buf, (DWORD)(strlen(buf) + 1), DEV_QIAN, DEV_LED, LV2);
				//取消预约记录
// 				tagTaxiCfg objTaxiCfg;
// 				memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
// 				GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg),
// 				offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
// 				objTaxiCfg.m_aryBespokeInf[0]=0xff;
// 				objTaxiCfg.m_aryBespokeInf[1]=0xff;
// 
// 				SetSecCfg(&objTaxiCfg,offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
			}
			break;
		case 0x01://预约电召
			if(frm.earlyTime>0)
			{
				//UINT interval=frm.earlyTime*60*1000;
				//_SetTimer(&g_objTimerMng, SETBESPOKE_TIMER,interval,G_TmSetBespokeProc);

				char buf[100] = { 0 };
				buf[0] = 6;
				strcat( buf, "抢答成功,请注意电招信息" );
				DataPush(buf, (DWORD)(strlen(buf) + 1), DEV_QIAN, DEV_LED, LV2);
				byte earlyTime=frm.earlyTime;
				byte bespokeTime[2]={0};
				memcpy(bespokeTime,frm.preTime,sizeof(bespokeTime));
				bespokeTime[0]-=earlyTime/60;
				if(earlyTime%60>bespokeTime[1])
				{
					bespokeTime[0]--;
					bespokeTime[1]=60-(earlyTime%60-bespokeTime[1]);
				}
				else bespokeTime[1]-=earlyTime%60;
				printf("Save Bespoke Time:%d:%d",bespokeTime[0],bespokeTime[1]);

				tagTaxiCfg objTaxiCfg;
				memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
				GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg),
				offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
				memcpy(objTaxiCfg.m_aryBespokeInf,bespokeTime,sizeof(objTaxiCfg.m_aryBespokeInf));
				SetSecCfg(&objTaxiCfg,offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
				//一分种检查一次预约电招
				_KillTimer(&g_objTimerMng, SETBESPOKE_TIMER);
				_KillTimer(&g_objTimerMng, BESPOKENOT_TIMER);
				_SetTimer(&g_objTimerMng, SETBESPOKE_TIMER,60*1000,G_TmSetBespokeProc);
			}	
			break;
		case 0x02://急召
			{
#if USE_LEDTYPE == 3
				g_objKTLed.SetLedShow('B',4,30*60*1000);
#endif
				char buf[100] = { 0 };
				buf[0] = 6;
				strcat( buf, "请注意,你车已被急招,请务必前往指定地点接送客人" );
				DataPush(buf, (DWORD)(strlen(buf) + 1), DEV_QIAN, DEV_LED, LV2);
				//取消预约记录
// 				tagTaxiCfg objTaxiCfg;
//				memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
// 				GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg),
// 				offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
// 				objTaxiCfg.m_aryBespokeInf[0]=0xff;
// 				objTaxiCfg.m_aryBespokeInf[1]=0xff;
// 				SetSecCfg(&objTaxiCfg,offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
			}
			break;
		default:
			break;
	}
	int iBufLen = 0;
	iRet = g_objSms.MakeSmsFrame( NULL, 0, 0x05, 0x51, buf, sizeof(buf), iBufLen );
	if( !iRet ) iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, 0 );
	return iRet;

}

//LED顶灯自定义信息设置(C、D 类)
int CDiaodu::Deal4208( char* v_szData, DWORD v_dwDataLen )
{
	if(v_dwDataLen<19 || (v_szData[1]*17)>v_dwDataLen-2) return ERR_PAR;
	char buf[200]={0};
	int iRet=0;
	char bResult=0x01;
	switch(v_szData[0])
	{
		case 0x01://即时信息
			{
				printf("rcv led c class text ");
#if USE_LEDTYPE == 3
				g_objKTLed.SetBackText('C',(byte*)(v_szData+2),1);
#endif
			}
			break;

		case 0x02://滚动信息
			{
				printf("rcv led d class text ");
#if USE_LEDTYPE == 3
				g_objKTLed.SetBackText('D',(byte*)(v_szData+2),v_szData[1]);
#endif
			}
			break;

		default:
			bResult=0x02;
			break;
	}

	int iBufLen = 0;
	iRet = g_objSms.MakeSmsFrame( &bResult, 1, 0x42, 0x48, buf, sizeof(buf), iBufLen );
	if( !iRet ) iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, 0 );
	return iRet;
}

void CDiaodu::P_TmSetBespokeProc()
{
	//_KillTimer(&g_objTimerMng, SETBESPOKE_TIMER);
	
	bool bSetBespoke=false;
	tagTaxiCfg objTaxiCfg;
	memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
	GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg),
	offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));

	
	byte bytCurHour=0;
	byte bytCurMin=0;
	//获取GPS数据
	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS( objGps, true );
	bytCurHour=(byte)objGps.nHour;
	bytCurMin=(byte)objGps.nMinute;
	//tagQianGps objQianGps;
	//g_objMonAlert.GpsToQianGps( objGps, objQianGps );
	printf("Bespoke:%d,%d;CurTim:%d,%d",objTaxiCfg.m_aryBespokeInf[0],
		objTaxiCfg.m_aryBespokeInf[1],bytCurHour,bytCurMin);
	if(objTaxiCfg.m_aryBespokeInf[0]>=24)
	{
		_KillTimer(&g_objTimerMng, SETBESPOKE_TIMER);
		objTaxiCfg.m_aryBespokeInf[0]=0xff;
		objTaxiCfg.m_aryBespokeInf[1]=0xff;
		SetSecCfg(&objTaxiCfg,offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	}
	if(bytCurHour>=objTaxiCfg.m_aryBespokeInf[0])
	{
		if(bytCurHour==objTaxiCfg.m_aryBespokeInf[0])
		{
			if(bytCurMin>=objTaxiCfg.m_aryBespokeInf[1])	bSetBespoke=true;
		}
		else bSetBespoke=true;
	}
   if(bSetBespoke)
   {
		objTaxiCfg.m_aryBespokeInf[0]=0xee;//已进入预约状态
		objTaxiCfg.m_aryBespokeInf[1]=0xee;
		SetSecCfg(&objTaxiCfg,offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
		//顶灯显示预约状态
		_KillTimer(&g_objTimerMng, SETBESPOKE_TIMER);

#if USE_LEDTYPE == 3
		g_objKTLed.SetLedShow('B',0,30*60*1000);
#endif
		//10分钟进行一次提醒
		P_TmBespokeNotProc();
		_SetTimer(&g_objTimerMng, BESPOKENOT_TIMER,10*60*1000,G_TmBespokeNotProc);
   }
	

}
void CDiaodu::P_TmBespokeNotProc()
{
	//_KillTimer(&g_objTimerMng, BESPOKENOT_TIMER);
	char buf[100] = { 0 };
	buf[0] = 6;
	strcat( buf, "你车已被预约，请按时前往接送客人" );
	DataPush(buf, (DWORD)(strlen(buf) + 1), DEV_QIAN, DEV_LED, LV2);

#if USE_LEDTYPE == 3	
	//LED顶灯刷新预约显示
	g_objKTLed.SetLedShow('B',0,30*60*1000);
#endif

}

