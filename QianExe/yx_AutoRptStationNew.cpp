#include "yx_QianStdAfx.h"

#if USE_AUTORPTSTATION == 1

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-AutoRptStation  ")

//数组分别对应F1，F2，F3，......的语音文件
char strVFile[9][12] =	{"F100","F200","F300","F400","F500","F600","","",""};

void *G_ThreadAutoRptStation(void *arg)
{
	g_objAuoRptStationNew.P_ThreadAutoRptStation();
}

void G_TmSetCheCi(void *arg, int len)
{
	g_objAuoRptStationNew.P_TmSetCheCi();
}

void G_InLinesTip()
{
	g_objAuoRptStationNew.Show_diaodu("导入语音文件中，请勿拔U盘勿断电");
}

//////////////////////////////////////////////////////////////////////////
CAutoRptStationNew::CAutoRptStationNew()
{
	m_ucStaCnt = 0;
	m_bytCheCiTmp = 0x0f;
	m_bytCheCi = 0x0f;		//表示其他。
	m_bytEnbRptSta = 1;		//开机即在营运状态.防止中途车台复位不报站了啦
	m_dwInStaTm = 0;		// 进站时间
	m_dwOutStaTm = 0;		// 出站时间
	m_usCurZhiTm = 0;
	m_usCurPaoTm = 0;
	m_ucCurLimitSpd  =  100;//公里/小时。默认设置为50，不能设置太小啊，不然会狂超速报警	
	m_ucLstUpDown = 0xff;

	m_objRcvDataMng.InitCfg(10*1024, 30000);
}

CAutoRptStationNew::~CAutoRptStationNew()
{
	
}

int CAutoRptStationNew::init()
{
	// 创建线程
	pthread_t pThreadAutoRptStation;
	if( pthread_create(&pThreadAutoRptStation, NULL, G_ThreadAutoRptStation, NULL) !=0 )
	{
		PRTMSG(MSG_ERR, "Create thread failed!\n");
		perror("");

		return ERR_THREAD;
	}

	return 0;
}

int CAutoRptStationNew::Release()
{

}

void CAutoRptStationNew::P_ThreadAutoRptStation()
{
	tagGPSData gps(0);
	char iResult = 0;	
	char szTip[300] = { 0 };
	int iLen = 0, i = 0, j = 0;	
	bool bRet = true;
	char szRcv[500] = { 0 };
	DWORD dwRcvLen = 0;
	DWORD dwPushTm = 0;
	BYTE bytLvl = 0;
	bool bLstRcvCmd = false;
	const DWORD STATION_MAXSIZE = sizeof(m_objStations.m_aryStation) / sizeof(m_objStations.m_aryStation[0]);	
	double aryStaDis[STATION_MAXSIZE] = { 0 };			//用于保存与各个站点之间的距离
	float max_direction1 = 0.0, min_direction1 = 0.0;	//进入报站半径的GPS方向范围
	float max_direction2 = 0.0, min_direction2 = 0.0;
	BYTE bytareaNo = 0; //报站后，只计算与相邻的几个站点的距离
	char buf[200] = { 0 };
	char buf2[200] = { 0 };

	szTip[0] = 6;

	//是否启动公交自动报站功能
	tag2QRptStationCfg objRptStationcfg;
	GetSecCfg( &objRptStationcfg, sizeof(objRptStationcfg), offsetof(tagSecondCfg, m_uni2QRptStationCfg.m_obj2QRptStationCfg), sizeof(objRptStationcfg) );
	m_bytEnbRptSta = objRptStationcfg.m_bytRptSta;

	if (m_bytEnbRptSta)
	{
		ShowAvTxt("营运","");
	}
	else 
	{
		ShowAvTxt("非营运","");
	}

	LoadStations( m_objStations, true, objRptStationcfg.m_iLineNo );

// 	for(int k=0; k<3; k++)
// 	{
// 		printf("m_objStations.m_aryStation[%d].m_iCurNo = %d\n", k, m_objStations.m_aryStation[k].m_iCurNo);
// 		printf("m_objStations.m_aryStation[%d].m_iUniId = %d\n", k, m_objStations.m_aryStation[k].m_iUniId);
// 		printf("m_objStations.m_aryStation[%d].m_bIsPlayed = %d\n", k, m_objStations.m_aryStation[k].m_bIsPlayed);	
// 		printf("m_objStations.m_aryStation[%d].m_lLat = %ld\n", k, m_objStations.m_aryStation[k].m_lLat);
// 		printf("m_objStations.m_aryStation[%d].m_lLon = %ld\n", k, m_objStations.m_aryStation[k].m_lLon);
// 		printf("m_objStations.m_aryStation[%d].m_iRad = %d\n", k, m_objStations.m_aryStation[k].m_iRad);
// 		printf("m_objStations.m_aryStation[%d].m_fdirect = %f\n", k, m_objStations.m_aryStation[k].m_fdirect);
// 		printf("m_objStations.m_aryStation[%d].m_bytStaAttr = %d\n", k, m_objStations.m_aryStation[k].m_bytStaAttr);
// 		printf("m_objStations.m_aryStation[%d].m_usZhiTm = %d\n", k, m_objStations.m_aryStation[k].m_usZhiTm);
// 		printf("m_objStations.m_aryStation[%d].m_usPaoTm = %d\n", k, m_objStations.m_aryStation[k].m_usPaoTm);
// 		printf("m_objStations.m_aryStation[%d].m_bytrecordfile = %d\n", k, m_objStations.m_aryStation[k].m_bytrecordfile);
// 		printf("m_objStations.m_aryStation[%d].m_szrecord:%s\n", k, m_objStations.m_aryStation[k].m_szrecord);
// 		printf("m_objStations.m_aryStation[%d].m_szName:%s\n", k, m_objStations.m_aryStation[k].m_szName);
// 		printf("m_objStations.m_aryStation[%d].m_szPlay:%s\n", k, m_objStations.m_aryStation[k].m_szPlay);
// 	}

	bytareaNo = m_objStations.m_iStaTotal/2;	//程序启动时，计算与所有站点之间的距离
	m_objRptSta.m_iPlayNo = 0;

	// 增加对重复报站的过滤
	int iLastStationNo = 0;
	int tempStationNo = 0;  // 增加对滞站越战站点名的判断

	ShowToAV(m_objRptSta.m_iPlayNo);

	while( !g_bProgExit )
	{
		if( m_objStations.m_iStaTotal >= STATION_MAXSIZE )
		{
			PRTMSG(MSG_ERR, "Auto Rpt Station Err 100\n" );
			continue;
		}			
		
		bLstRcvCmd = false;
		
		while( !m_objRcvDataMng.PopData(bytLvl, sizeof(szRcv), dwRcvLen, szRcv, dwPushTm) )
		{
			bLstRcvCmd = true;

			if( dwRcvLen < 1 ) goto _POPRCVDATA_NEXT;

			switch( BYTE(szRcv[0]) )
			{		
			case 0x01:
				{
					switch(BYTE(szRcv[1]))
					{
					case 0x01:
					case 0x02:
					case 0x03:
					case 0x04:
					case 0x05:
					case 0x06:
						{
							// (待) 播放特定语音
						}
						break;

					case 0x07:
						{
							SetWorkSta();		//实际可能乱按，不给用了？		
						}
						break;
						
					case 0x08:
						{
							
						}
						break;

					case 0x09: // 回场/出场切换
						{
							static int sta_iChan = 0;
							if (sta_iChan++%2)
							{
								ShowAvTxt("回场-下班","");
								m_bytCheCiTmp = 2; 
							}
							else
							{
								ShowAvTxt("出场-上班","");
								m_bytCheCiTmp = 1;
							}	
							m_bytEnbRptSta = true; // 为什么这里开启报站？应该回场关闭，出场开启？

							_SetTimer(&g_objTimerMng, SET_CHECI_TIMER, 5000,G_TmSetCheCi);	
						}
						break;
					}
				}
				break;

			case 0x02://向前跳过一站并语音提醒 ,左箭头
				{
					PRTMSG( MSG_NOR, "Front station!\n");

					if((m_objRptSta.m_iPlayNo >= 0) && (m_objRptSta.m_iPlayNo < m_objStations.m_iStaTotal))
					{
						if(0==m_objRptSta.m_iPlayNo)
						{
							m_objRptSta.m_iPlayNo = (m_objStations.m_iStaTotal-1);
						}
						else
						{
							m_objRptSta.m_iPlayNo -= 1;
						}
					}

					ShowToAV(m_objRptSta.m_iPlayNo);
				}
				break;

			case 0x03://向后跳回一战，右箭头
				{	
					if(m_bytEnbRptSta) // 若在营运状态
					{
						if((m_objRptSta.m_iPlayNo >= 0) && (m_objRptSta.m_iPlayNo < m_objStations.m_iStaTotal))
						{
							if(m_objRptSta.m_iPlayNo == (m_objStations.m_iStaTotal-1))
							{
								m_objRptSta.m_iPlayNo = 0;
							}
							else
							{
								m_objRptSta.m_iPlayNo += 1;
							}
						}
						ShowToAV(m_objRptSta.m_iPlayNo);
					}
					else		// 若在非营运状态
					{
						static int sOutWork = 0; //非营运各状态切换
						int offset = 2;

//						if (0 == sOutWork) 
//						{
//							m_bytCheCiTmp = 1; //出场
//							ShowAvTxt("出场-上班","");
//						}
//						if (1 == sOutWork) 
//						{
//							m_bytCheCiTmp = 2; //回场
//							ShowAvTxt("回场-下班","");
//						}

						if (2-offset == sOutWork) 
						{
							m_bytCheCiTmp = 3;  //加油
							ShowAvTxt("非营运-加油","");
						}
						if (3-offset == sOutWork) 
						{
							m_bytCheCiTmp = 4; //保养
							ShowAvTxt("非营运-保养","");
						}
						if (4-offset == sOutWork) 
						{
							m_bytCheCiTmp = 5; //修车
							ShowAvTxt("非营运-修车","");
						}
						if (5-offset == sOutWork) 						
						{
							m_bytCheCiTmp = 6; //包车
							ShowAvTxt("非营运-包车","");
						}
						if (6-offset == sOutWork) 
						{
							m_bytCheCiTmp = 7; //放空
							ShowAvTxt("非营运-放空","");
						}
						if (7-offset == sOutWork) 
						{
							m_bytCheCiTmp = 0x0f; //其他
							ShowAvTxt("非营运-其他","");
						}

						if(++sOutWork>7-offset)sOutWork=0;
						
						_SetTimer(&g_objTimerMng, SET_CHECI_TIMER, 5000,G_TmSetCheCi);						
					}
				}
				break;

			case 0x04: // 上下行切换  C键
				{
					if((m_objRptSta.m_iPlayNo >= 0) && (m_objRptSta.m_iPlayNo < m_objStations.m_iStaTotal))
					{
						static int iturn = 0;//作为上下行切换

						if (iturn++%2 == 0)
						{
							m_objRptSta.m_iPlayNo = 0;//上行出站
						}
						else
						{
							int k;
							for(k=0;k<m_objStations.m_iStaTotal;k++)
							{
								if(!memcmp("下行",m_objStations.m_aryStation[k].m_szName,4))
									break;									
							}
							m_objRptSta.m_iPlayNo = k;//下行出站
						}				
					}

					ShowToAV(m_objRptSta.m_iPlayNo);
				}
				break;

			case 0x07: //重播刚才的到站/出站提示，0键
				{
					if(!m_bytEnbRptSta)  // 若非营运态
					{
						Show_diaodu("非营运状态,恕不报站!");
						break;
					}

					if((m_objRptSta.m_iPlayNo >= 0) && (m_objRptSta.m_iPlayNo < m_objStations.m_iStaTotal))
					{
						m_objRptSta.m_bPlaySound = true;
					}
				}
				break;	

			case 0x08:	//到站，#键
				{

				}
				break;	
				
			case 0x09:	//出站，*键
				{

				}
				break;	

			case 0x0b:
				{

				}
				break;

			case 0x0a: // 开关摄像头
				{

				}
				break;

			//配置线路号
			case 0x21: 
				{
					/////////////////////////////////////////////////////////////
					char frm[20] = {0};	
					frm[0] = (char)0x01;	// 0x01表示要发送到调度屏或手柄
					frm[1] = (char)0x8f;
					frm[2] = (char)0x81;
					frm[3] = (char)0x01;
					DataPush((char*)&frm, strlen(frm), DEV_QIAN, DEV_DIAODU, 2 );

					memset(frm,0,sizeof(frm));				
					memcpy(frm,szRcv+1,dwRcvLen - 1);

					ulong iStaNum = atol(frm+1);
					LoadStations(m_objStations,0,iStaNum); // 加载新线路
				}
				break;

			case 0x10: 
				{
					char frm[100] = {0};
					DWORD dwBegin = GetTickCount();
					Show_diaodu( "U盘检测中..." );
					
					FILE* fp = NULL;
					do
					{
						fp = fopen( SPRINTF(buf, "%s/LineIno.wav", RPTSTATION_U_PATH), "rb" );
						if( fp ) break;

						if( !g_bProgExit )
						{
							sleep(1);
							Show_diaodu( "U盘检测中..." );
						}
						else
						{
							break;
						}
					} while( GetTickCount() - dwBegin < 10000 );

					if( !fp )
					{
						Show_diaodu( "线路文件导入失败" );
						goto IMPORT_WAV;
					}
					else
					{
						fclose( fp );
					}
					
					frm[0] = char(0x01);
					frm[1] = char(0x8f);
					frm[2] = char(0x90);
					frm[3] = 3;
					DataPush( frm, 4, DEV_QIAN, DEV_DIAODU, LV2 ); // 正在导入

					if( access(RPTSTATION_SAVE_PATH, F_OK) != 0 )
					{
						if( 0 == mkdir(RPTSTATION_SAVE_PATH, 777) )
						{
							chmod(RPTSTATION_SAVE_PATH, S_IRWXU|S_IRWXG|S_IRWXO);
						}
						else
						{
							Show_diaodu("创建线路文件目录失败");
							break;
						}
					}

					int iResult = FileCopy( SPRINTF(buf, "%sLineIno.wav", RPTSTATION_U_PATH), SPRINTF(buf2, "%sLineIno.wav", RPTSTATION_SAVE_PATH) );
					frm[3] = iResult > 0 ? 1 : 2;
					DataPush( frm, 4, DEV_QIAN, DEV_DIAODU, 2 ); // 导入成功/失败

					if( iResult > 0 )
					{
						Show_diaodu( "线路文件导入成功" );
						sleep(1);

						tag2QRptStationCfg cfg;
						memset(&cfg,0,sizeof(cfg));
						GetSecCfg( &cfg, sizeof(cfg), offsetof(tagSecondCfg, m_uni2QRptStationCfg.m_obj2QRptStationCfg), sizeof(cfg) );
						LoadStations( m_objStations, true, cfg.m_iLineNo );

						// 重置报告状态
						bytareaNo = m_objStations.m_iStaTotal/2;
						m_objRptSta.m_iPlayNo = 0; 
					}
					else
					{
						Show_diaodu( "线路文件导入失败" );
					}
					sleep(1);

IMPORT_WAV:

#if USE_PLAYSOUNDTYPE == 2		//若使用录音报站，则此时也导入录音文件
					Show_diaodu("开始导入语音文件，请别拔U盘");

					iResult = DirCopyFilter( RPTSTATION_U_PATH, RPTSTATION_SAVE_PATH, ".wav", "LineIno.wav", G_InLinesTip );
					
					if( iResult == 1)
						Show_diaodu( "语音文件导入成功" );
					else
						Show_diaodu( "语音文件导入失败" );
#endif
					;
				}
				break;

			case 0x99: // 更换线路
				{
					int  iSelNo ;
					BYTE bytSelLen = BYTE( szRcv[1] );
					char szSelNo[16] = {0};
					memcpy(szSelNo, szRcv+2, bytSelLen);
					iSelNo = atoi(szSelNo);
					
					LoadStations( m_objStations, true, iSelNo );

					bytareaNo = m_objStations.m_iStaTotal/2;
					m_objRptSta.m_iPlayNo = 0;
				}
				break;

			case 0xf2://中心下发的公交线路文件升级
				{
					BOOL blResult = FALSE;

					char szFileName[MAX_PATH] = {0};
					sprintf(szFileName, "/mnt/Flash/part3/Down/Line.dat");
					
					FILE *fp = NULL;
					fp = fopen(szFileName, "rb");
					if( !fp )
					{
						PRTMSG( MSG_ERR, "Can't find %s\n", szFileName );
						break;
					}
					fclose( fp );

					if( access(RPTSTATION_SAVE_PATH, F_OK) != 0 )
					{
						if( 0 == mkdir(RPTSTATION_SAVE_PATH, 777) )
						{
							chmod(RPTSTATION_SAVE_PATH, S_IRWXU|S_IRWXG|S_IRWXO);
						}
						else
						{
							Show_diaodu("创建线路文件目录失败");
							break;
						}
					}

					int iResult = 0;
					SPRINTF(buf, "%s", szFileName);
					SPRINTF(buf2, "%s%s", RPTSTATION_SAVE_PATH, "LineIno.wav");

					// 删除原有线路文件
					if( access(buf2, F_OK) == 0 )
					{
						unlink(buf2);
					}

#if USE_PLAYSOUNDTYPE == 1
					iResult = FileCopy( buf, buf2 );
#endif
#if USE_PLAYSOUNDTYPE == 2
					iResult = DecompressLineFile( buf );
#endif
					if( iResult )
					{
						Show_diaodu("公交线路文件下载成功");
						PRTMSG(MSG_DBG, "公交线路文件下载成功\n");
					}
					else
					{
						Show_diaodu("公交线路文件下载失败");
						PRTMSG(MSG_DBG, "公交线路文件下载失败\n");
					}
					
					if( iResult > 0 )
					{					
						tag2QRptStationCfg cfg;
						memset(&cfg,0,sizeof(cfg));
						GetSecCfg( &cfg, sizeof(cfg), offsetof(tagSecondCfg, m_uni2QRptStationCfg.m_obj2QRptStationCfg), sizeof(cfg) );

						LoadStations( m_objStations, true, cfg.m_iLineNo );

						bytareaNo = m_objStations.m_iStaTotal/2;
						m_objRptSta.m_iPlayNo = 0;
					}
				}
				break;
			default:
				break;
			}

_POPRCVDATA_NEXT:
			memset( szRcv, 0, sizeof(szRcv) );
			dwRcvLen = 0;
			break; // 一定要退出循环
		}

		if( bLstRcvCmd )
			usleep(100000);
		else			
			usleep(1000000);		

		if (!m_bytEnbRptSta) 
		{
			continue;
		}
	
		GetCurGps(&gps, sizeof(gps), GPS_REAL);

		//若5分钟内都没有报站，则重新开始计算与所有站点之间的距离
		static ulong t1 = 0;
		{
			if(0==t1)	t1 = GetTickCount();
			if((GetTickCount()-t1)>ulong(5*60*1000)) 
			{
				bytareaNo = m_objStations.m_iStaTotal/2;
				m_ucLstUpDown = 0xff;
				t1 = 0;
			}
		}

		char resbuf[1024] = {0};
		int  iBufLen;
		int  iRet;

		//计算与各个站点之间的距离，保存在数组aryStaDis中；
		//为节省计算量，只计算与相邻的bytareaNo个站点的距离
		CalculateDis(gps.latitude,gps.longitude,aryStaDis,m_objRptSta.m_iPlayNo,int(bytareaNo));

		byte bytAccSta = 0;
		IOGet(IOS_ACC, &bytAccSta);

		if('A' == gps.valid && ( bytAccSta == IO_ACC_ON ) )
		{
			for(j = m_objRptSta.m_iPlayNo - int(bytareaNo); j < m_objRptSta.m_iPlayNo + int(bytareaNo); j++ ) // 这样计算，当站点总数为奇数时可能会漏一站？
			{
				if( j<0 )
					i = j + m_objStations.m_iStaTotal;
				else if( j>= m_objStations.m_iStaTotal)
					i = j - m_objStations.m_iStaTotal;
				else 
					i = j;

				// 计算方向角度差绝对值
				float fDeltDir = gps.direction;
				if( fDeltDir < m_objStations.m_aryStation[i].m_fdirect ) fDeltDir += 360;
				fDeltDir -= m_objStations.m_aryStation[i].m_fdirect;
				if( fDeltDir > 180 ) fDeltDir = 360 - fDeltDir;

				//判断是否从该方向进入该站点半径，且该站点未报站
				if( aryStaDis[i] <= m_objStations.m_aryStation[i].m_iRad	//若进入了该点的半径之内
					&& 
					fDeltDir <= 70 // 且方向大致符合
					&&
					m_objStations.m_aryStation[i].m_bIsPlayed == false	//且本站未报站过
					) 
				{
					// 增加对同一个站点进站出站重复报的过滤
					if( m_objStations.m_aryStation[i].m_iCurNo != iLastStationNo)
					{
						m_objRptSta.m_iPlayNo = i;

						if( 
							// 对于中间站, 判断当前站是否与上次的站在同一个方向上，防止停在站点附近由于GPS漂移报反方向的站
							(
								gps.speed<1.8 && 0xff != m_ucLstUpDown && 3 == GetStaAttr(m_objRptSta.m_iPlayNo) 
								&& m_ucLstUpDown != GetUpDownSta(m_objRptSta.m_iPlayNo) 
							)
							||
							(
								//对于起点站要速度大于一定的数值才报，防止车停在终点站乱报
								(1 == GetStaAttr(m_objRptSta.m_iPlayNo) && gps.speed<1.2 )
							)

						  ) // 判断什么时候是上传数据却不报站 (可能是漂移)，之后仍然存在报此站的可能
						{
							static int sta_iLastSendNo = 0;

							m_objRptSta.m_bPlaySound = false;

							//hxd 为了中心统计掉头报警，虽然语音不报站，但是数据要上传到中心
							if (sta_iLastSendNo != m_objStations.m_aryStation[i].m_iCurNo) 
							{
								SendInOutData(m_objRptSta.m_iPlayNo,2);
							}
							sta_iLastSendNo = m_objStations.m_aryStation[i].m_iCurNo;
						}
						else
						{						
							iLastStationNo = m_objStations.m_aryStation[i].m_iCurNo;
							m_objRptSta.m_bPlaySound = true;
							m_objStations.m_aryStation[i].m_bIsPlayed = true;			
						}
					}
					else
					{
						PRTMSG( MSG_DBG, "sta have play\n" );
						m_objRptSta.m_bPlaySound = false;
					}
						
					if(!m_objRptSta.m_bPlaySound)
						continue;

					//调度屏提示到站
					ShowToAV(m_objRptSta.m_iPlayNo);
					t1 = GetTickCount();
					bytareaNo = m_objStations.m_iStaTotal/2;
					PRTMSG( MSG_NOR, "Report station %d\n", i );

					// 将其他站点标记为“未报站”
					for(int j=0; j < m_objStations.m_iStaTotal; j++ )
					{
						if(j != i)	m_objStations.m_aryStation[j].m_bIsPlayed = false;
					}

					//获取当前路段的超速速度
					m_ucCurLimitSpd = GetStaLimitSpd(m_objRptSta.m_iPlayNo);

					//发送到离站数据
					tag4247 obj4247;
					time_t lCurrentTime;
					struct tm *pCurrentTime;
					time(&lCurrentTime);
					pCurrentTime = localtime(&lCurrentTime);
					obj4247.usLineNo =  m_objStations.m_iLineNo;
					obj4247.usStaNo  =  m_objStations.m_aryStation[i].m_iUniId;
					if(  !GetInOutSta(m_objRptSta.m_iPlayNo) )
					{
						obj4247.bytIsInOut = 0x00;//进站
					}
					else
					{
						obj4247.bytIsInOut = 0x01;//出站
					}
					obj4247.usYear   =  (WORD)(pCurrentTime->tm_year + 1900);
					obj4247.bytMon   =  (char)pCurrentTime->tm_mon + 1;
					obj4247.bytDay   =  (char)pCurrentTime->tm_mday;
					obj4247.bytHour  =  (char)pCurrentTime->tm_hour;
					obj4247.bytMin   =  (char)pCurrentTime->tm_min;
					obj4247.bytSec   =  (char)pCurrentTime->tm_sec;
					obj4247.dwMeterTotle  =  0;	//暂不填

					iRet = g_objSms.MakeSmsFrame((char*)&obj4247, sizeof(obj4247), 0x42, 0x47, resbuf, sizeof(resbuf), iBufLen, CVT_8TO7);
					if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);

					if ( !GetInOutSta(m_objRptSta.m_iPlayNo) ) // 如果是进站点
					{// 滞站抛站判断预处理
						m_dwInStaTm = GetTickCount();
						m_usCurZhiTm = m_objStations.m_aryStation[i].m_usZhiTm;
						m_usCurPaoTm = m_objStations.m_aryStation[i].m_usPaoTm;
						tempStationNo = GetUniStaId(i);  // 增加对报滞站越站站点的判断
					}
					else
					{
						tag4246 obj4246;
						int dwSpan;  
						m_dwOutStaTm = GetTickCount();
						dwSpan =(int) (m_dwOutStaTm - m_dwInStaTm)/1000 ;
						obj4246.usLineNo =  WORD(m_objStations.m_iLineNo);
						obj4246.usStaNo  =  WORD(m_objStations.m_aryStation[i].m_iCurNo);
						obj4246.bytYear  =  (WORD)(pCurrentTime->tm_year + 1900);
						obj4246.bytMon   =  (char)pCurrentTime->tm_mon + 1;
						obj4246.bytDay   =  (char)pCurrentTime->tm_mday;
						obj4246.bytHour  =  (char)pCurrentTime->tm_hour;
						obj4246.bytMin   =  (char)pCurrentTime->tm_min;
						obj4246.bytSec   =  (char)pCurrentTime->tm_sec;

						if(dwSpan >= 15360)	
						{
							dwSpan = 15360;
						}

						obj4246.bytTmspanMin = (char)(dwSpan/60);
						obj4246.bytTmspanSec = (char)(dwSpan%60);

						if ( (dwSpan > m_usCurZhiTm) && (tempStationNo == GetUniStaId(i))) // 同一站点才报4246
						{
							obj4246.bytAlemType = 0; 

							iRet = g_objSms.MakeSmsFrame((char*)&obj4246, sizeof(obj4246), 0x42, 0x46, resbuf, sizeof(resbuf), iBufLen, CVT_8TO7);
							if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);
						}

						if ( (dwSpan < m_usCurPaoTm) && (tempStationNo == GetUniStaId(i))) 	 // 同一站点才报4246				
						{
							obj4246.bytAlemType = 1;

							iRet = g_objSms.MakeSmsFrame((char*)&obj4246, sizeof(obj4246), 0x42, 0x46, resbuf, sizeof(resbuf), iBufLen, CVT_8TO7);
							if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);
						}
					}
					break;
				}
			}
		}

		bRet = true;
		
		//是否要进行语音播报
		if( m_objRptSta.m_bPlaySound )
		{
			m_ucLstUpDown = GetUpDownSta(m_objRptSta.m_iPlayNo);

#if USE_PLAYSOUNDTYPE == 1	//使用TTS语音报站
			char szTip[300] = { 0 };
			szTip[0] = 1;
			szTip[1] = 1; // 表示立即播报

			iLen = strlen(m_objStations.m_aryStation[m_objRptSta.m_iPlayNo].m_szPlay);
			memcpy(szTip+2, m_objStations.m_aryStation[m_objRptSta.m_iPlayNo].m_szPlay, iLen );
			iLen += 2;			
			DataPush( szTip, iLen, DEV_QIAN, DEV_DVR, LV2 );
#endif

#if USE_PLAYSOUNDTYPE == 2	//使用录音文件报站 (待)
#endif
			m_objRptSta.m_bPlaySound = false;
		}
	}

}


/// 调度屏上切换状态的处理
void CAutoRptStationNew::P_TmSetCheCi()
{
	_KillTimer(&g_objTimerMng, SET_CHECI_TIMER);

	char buf[500] = { 0 };
	int iRet;
	int iSendLen;
	char sendBuf[512]={0};

	strcpy( buf, "发送营运状态切换请求失败!");

	if( !g_objSock.IsOnline() )
	{
		goto _SETCHECI_END;
	}

	m_ucStaCnt++;

	buf[0] = m_ucStaCnt;
	
	//类型: 非营运 0x01 营运 0x02
	if (m_bytEnbRptSta)
	{
		buf[1] = 2;//当前是非营运状态，那么切换到营运状态.
	}
	else
	{
		buf[1] = 1;
	}
	buf[2] = m_bytCheCiTmp;
	
	iRet = g_objSms.MakeSmsFrame(buf, 3 ,0x42,0x60,sendBuf,sizeof(sendBuf),iSendLen);
	if( !iRet ) 
	{
		g_objSock.SOCKSNDSMSDATA( sendBuf, iSendLen, LV12,0);
	}
	else 
	{
		goto _SETCHECI_END;
	}

	strcpy( buf, "发送营运状态切换请求成功!" );

_SETCHECI_END:
	Show_diaodu( buf );
}

bool CAutoRptStationNew::LoadStations( tagStationsNew &v_objStations, bool v_bReBegin, int v_iSelNo )
{
	bool iRet = false;
	static WORD sta_wSelNo = 0xffff;
	char buf[200] = { 0 };
	tagStationsNew objStationsRead;

	if( v_bReBegin )	sta_wSelNo = 0xffff;

	memset( &m_objLinesHead, 0, sizeof(m_objLinesHead) );

	FILE* fp = fopen( SPRINTF(buf, "%sLineIno.wav", RPTSTATION_SAVE_PATH), "rb" );
	if( fp )
	{
		WORD w = 0;
		WORD wNextNo;
		WORD wCurNo = 0;

		if( 1 != fread( &m_objLinesHead, sizeof(m_objLinesHead), 1, fp ) )
		{
			Show_diaodu("读取线路文件头错误");
			goto LOADLINE_END;
		}
		if( m_objLinesHead.m_wLineTotal < 1 )
		{
			Show_diaodu("线路总数为0");
			goto LOADLINE_END;
		}

		wNextNo = sta_wSelNo + 1;
		if( wNextNo >= m_objLinesHead.m_wLineTotal )
			wNextNo = 0;
		
		// 路线遍历
		for( w = 0; w < m_objLinesHead.m_wLineTotal; w ++ )
		{
			wCurNo = wNextNo;

			if( fseek(fp, sizeof(m_objLinesHead) + wNextNo * sizeof(objStationsRead), SEEK_SET) )
			{
				PRTMSG( MSG_NOR, "fseek %d line err!!!\n", wCurNo );
				Show_diaodu("找不到报站线路");
				goto LOADLINE_END;
			}

			if( 1 != fread( (void*)&objStationsRead, sizeof(objStationsRead), 1, fp ) )
			{
				PRTMSG( MSG_NOR, "Loadsta read Line err\n");
				Show_diaodu("读取线路错误");
				goto LOADLINE_END;
			}

			//如果传入的参数等于0那么就选择第一个使能的那个，否则要按输入的参数进行选择的.
			if( v_iSelNo > 0 )
			{
				if( v_iSelNo == objStationsRead.m_iLineNo )
				{
					char buf[20] = { 0 };
					buf[0] = 1;
					buf[1] = 0;
					buf[2] = char( v_iSelNo );
					char frm[100] = { 0 };
					int iFrmLen = 0;
					int iResult = g_objSms.MakeSmsFrame( buf, 3, 0x39, 0x43, frm, sizeof(frm), iFrmLen, CVT_8TO7 );
					if( !iResult )
					{
						iResult = g_objSock.SOCKSNDSMSDATA( frm, iFrmLen, LV9 );
					}

					break;
				}
			}
			else
			{
				if( objStationsRead.m_bytLineEnb )
					break;
			}

			wNextNo ++;
			if( wNextNo >= m_objLinesHead.m_wLineTotal )	wNextNo = 0;
		}

		if( w < m_objLinesHead.m_wLineTotal )
		{
			// 站点遍历
			for ( int i=0; i<objStationsRead.m_iStaTotal; i++ ) 
			{
				if(objStationsRead.m_aryStation[i].m_bytrecordfile > (sizeof(objStationsRead.m_aryStation[i].m_szrecord) / sizeof(objStationsRead.m_aryStation[i].m_szrecord[0])) )
				{
					Show_diaodu("线路文件有误,语音文件太多.");
					goto LOADLINE_END;
				}
			}
			v_objStations = objStationsRead;
			sta_wSelNo = wCurNo;

			PRTMSG(MSG_NOR, "启用自动报站线路:%d路\n",objStationsRead.m_iLineNo );

			char str[100] = {0};
			sprintf(str, "启用线路--%d", objStationsRead.m_iLineNo );
			Show_diaodu( str );

			//把当前使用的线路存储到配置区里面
			tag2QRptStationCfg cfg;
			GetSecCfg( &cfg, sizeof(cfg), offsetof(tagSecondCfg, m_uni2QRptStationCfg.m_obj2QRptStationCfg), sizeof(cfg) );
			cfg.m_iLineNo = objStationsRead.m_iLineNo;
			SetSecCfg(&cfg, offsetof(tagSecondCfg, m_uni2QRptStationCfg.m_obj2QRptStationCfg), sizeof(cfg) );

            iRet = true;
		}
		else
		{
			PRTMSG(MSG_NOR, "加载线路失败,请重新选择线路\n");
			Show_diaodu("加载线路失败，请重新选择线路");
			iRet = false; 
		}
	}

	
LOADLINE_END:
	if(fp)
	{
		fclose( fp );
	}
	else
	{
		PRTMSG(MSG_NOR, "Open Line file failed!\n");
	}

	return iRet;
}

//------------------------------------------------------------------------------
// 在调度屏上显示提示信息
// fmt,..: 格式化输入
void CAutoRptStationNew::Show_diaodu(char *v_szbuf)
{
	char str[1024] = {0x01, 0x72, 0x01};
	memcpy(str+3, v_szbuf, strlen(v_szbuf));
	DataPush(str, strlen(v_szbuf)+3, DEV_QIAN, DEV_DIAODU, LV2);
}


bool CAutoRptStationNew::CalculateDis(double latitude, double longitude, double *aryStaDis, int curNo, int iAreaNum)
{
	int i=0,j=0;
	double dDis = 0;
	if( iAreaNum < 1 ) return false;

	for( i=curNo - iAreaNum; i <= curNo + iAreaNum; i++)
	{// 照这么循环，总数为偶数时，会多算一个

		if( i<0 ) 
			j = i + m_objStations.m_iStaTotal;
		else if( i>= m_objStations.m_iStaTotal)
			j = i - m_objStations.m_iStaTotal;
		else 
			j = i;

		if( j >= 0 && j < m_objStations.m_iStaTotal )
		{
			dDis = G_CalGeoDis2( longitude, latitude, m_objStations.m_aryStation[j].m_lLon / LONLAT_DO_TO_L, m_objStations.m_aryStation[j].m_lLat / LONLAT_DO_TO_L );
			aryStaDis[j] = dDis;
		}
	}
	return true;
}

// 解压打包的公交线路文件（包括声音文件）
int CAutoRptStationNew::DecompressLineFile(char *filename)
{
	FILE *fp = NULL;
	FILE *fp_write = NULL;
	fp = fopen(filename, "rb+");
	if(NULL != fp)
	{
		mkdir( RPTSTATION_SAVE_PATH, 0777 );

		int ifilecount = 0, ifilesize = 0;
		char szFilename[15];
		char szwritefilename[100];
		char *data_buf;

		fread(&ifilecount,4,1,fp);	//读取文件个数

		for( int i=0; i < ifilecount; i++)
		{
			fread(szFilename,15,1,fp);	//读取文件名
			fread(&ifilesize,4,1,fp);	//读取文件大小

			data_buf = (char*)malloc(ifilesize);	
			fread(data_buf,ifilesize,1,fp);	//读取文件数据

			sprintf( szwritefilename, "%s%s", RPTSTATION_SAVE_PATH, szFilename );
			fp_write = fopen(szwritefilename,"wb+"); //打开（新建）文件	

			if( fp_write )
			{
				fwrite(data_buf,ifilesize,1,fp_write);	//写入文件
				fclose(fp_write);
				fp_write = NULL;
			}
		}
		fclose(fp);
		return 1;
	}
	return 0;
}

// 显示2行信息（上行、下行）
bool CAutoRptStationNew::ShowAvTxt( char* v_szUp,char *v_szDown)
{
	tag8ff1 frm;
	int iUpLen = 0,iDownLen = 0;

	if (!v_szUp||!v_szDown)
	{
		return false;
	}

	iUpLen = byte(strlen(v_szUp));
	iDownLen = byte(strlen(v_szDown));

	if ((iUpLen+iDownLen)>sizeof(frm.m_szContent) - 4 )
	{
		return false;
	}

	frm.m_szContent[0] = iUpLen;
	memcpy(frm.m_szContent+1,v_szUp,iUpLen);
	frm.m_szContent[iUpLen+1] = iDownLen;
	memcpy(frm.m_szContent+iUpLen+2,v_szDown,iDownLen);

	char buf[100] = {0};
	buf[0] = 0x01;	// 0x01表示发送到调度屏
	memcpy(buf+1, (void*)&frm, iUpLen+iDownLen+4);
	DataPush( (char*)&frm, iUpLen+iDownLen+4+1, DEV_QIAN, DEV_DIAODU, LV2 );

	return true;
}

int CAutoRptStationNew::DealComu8e(char* v_szBuf, DWORD v_dwBufLen )
{
	DWORD dwPacketNum = 0;
	return m_objRcvDataMng.PushData( LV8, v_dwBufLen, v_szBuf, dwPacketNum );
}

//删除线路文件跟所有的语音文件
// BOOL CAutoRptStationNew::DeleteDirectory(const CString szDir)
// {
// 	CString szFindDir=szDir;
// 	if(szFindDir.Right(1)!="\\"){szFindDir+="\\"; }
// 	szFindDir+="*.*";
// 
// 	WIN32_FIND_DATA fd;
// 	HANDLE hFind;
// 	hFind=FindFirstFile(szFindDir,&fd);
// 	if(hFind!=INVALID_HANDLE_VALUE)
// 	{
// 		do{
// 			if(fd.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY)
// 			{ 
// 				DeleteDirectory(szDir+"\\"+fd.cFileName); //it must be directory,get into
// 			}else{ 
// 				if(DeleteFile(szDir+"\\"+fd.cFileName)==FALSE)
// 				{
// 					g_objDiaodu.show_diaodu("删除线路文件失败");
// 					return FALSE;
// 				}//it is file ,delete it
// 			}
// 		}while(FindNextFile(hFind,&fd));
// 	}
// 	//if you donot close the handle,the next step of Removing Directory would failed
// 	CloseHandle(hFind);
// 	//the root directory must be empty ,so remove it
// 	if(RemoveDirectory(szDir)==FALSE)
// 	{	
// 		g_objDiaodu.show_diaodu("删除线路目录失败");
// 		return FALSE;
// 	}
// 	return TRUE;
// }


// 显示报站状态
bool CAutoRptStationNew::ShowToAV( DWORD PlayNo )
{
	tag8ff1 frm;
	char	*pSymbol = NULL;
	char	name[20] ={0};
	char	*pName = NULL;
	int		iNLen = 0;
	char	szTemp[80] = { 0 };
	BYTE	bytLen = 2;

	pSymbol = strchr( m_objStations.m_aryStation[PlayNo].m_szName, '-' );

	if(pSymbol==NULL)
	{
		return false;
	}

	pSymbol +=1;
	pName = pSymbol;
	pSymbol = strchr( pName, '-' );
	if(pSymbol==NULL)
	{
		return false;
	}

	iNLen = pSymbol - pName;
	memcpy(name,pName,iNLen);

	if( !memcmp("进站",pSymbol+1,4) )
	{
		sprintf(szTemp,"到达:%s",name);
		frm.m_szContent[0] = BYTE(strlen(szTemp));
		strcpy(frm.m_szContent+1,szTemp);
		bytLen += 1 + BYTE( strlen(szTemp) );
		pSymbol = strchr( m_objStations.m_aryStation[PlayNo-1].m_szName, '-' );
		if(pSymbol==NULL)
		{
			return false;
		}
		pSymbol +=1;
		pName = pSymbol;
		pSymbol = strchr( pName, '-' );
		if(pSymbol==NULL)
		{
			return false;
		}
		iNLen = pSymbol - pName;
		memset(name,0,sizeof(name));
		memcpy(name,pName,iNLen);
		sprintf(szTemp,"上站:%s",name);
		frm.m_szContent[ strlen(frm.m_szContent) ] = BYTE( strlen(szTemp) );
		strcat( frm.m_szContent, szTemp );
		bytLen += 1 + BYTE( strlen(szTemp) );
	}
	else
	{
		sprintf(szTemp,"离开:%s",name);
		frm.m_szContent[0] = BYTE(strlen(szTemp));
		strcpy(frm.m_szContent+1,szTemp);
		bytLen += 1 + BYTE( strlen(szTemp) );
		pSymbol = strchr( m_objStations.m_aryStation[PlayNo+1].m_szName, '-' );
		if(pSymbol==NULL)
		{
			return false;
		}
		pSymbol +=1;
		pName = pSymbol;
		pSymbol = strchr( pName, '-' );
		if(pSymbol==NULL)
		{
			return false;
		}
		iNLen = pSymbol - pName;
		memset(name,0,sizeof(name));
		memcpy(name,pName,iNLen);
		sprintf(szTemp,"下站:%s",name);
		frm.m_szContent[ strlen(frm.m_szContent) ] = BYTE( strlen(szTemp) );
		strcat( frm.m_szContent, szTemp );
		bytLen += 1 + BYTE( strlen(szTemp) );
	}

	char buf[100] = {0};
	buf[0] = 0x01;
	memcpy(buf+1, (void*)&frm, bytLen);
	DataPush( buf, bytLen+1, DEV_QIAN, DEV_DIAODU, 2 );

	return true;
}

void CAutoRptStationNew::SendInOutData(int v_iSta, byte v_bytSym)
{
	//发送到离站数据
	int iRet;
	int iBufLen;
	byte dataType;
	char resbuf[2048];
	tag4247 obj4247;
	
	dataType = 0x47;
	if (1 == v_bytSym) 	dataType = 0x47;
	if (2 == v_bytSym)  dataType = 0x74;

	struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);

	obj4247.usLineNo =  m_objStations.m_iLineNo;
	obj4247.usStaNo  =  m_objStations.m_aryStation[v_iSta].m_iUniId;

	if(  !GetInOutSta(v_iSta) )
	{
		obj4247.bytIsInOut = 0x00;//进站
	}
	else
	{
		obj4247.bytIsInOut = 0x01;//出站
	}

	obj4247.usYear   =  (WORD)(pCurrentTime.tm_year);
	obj4247.bytMon   =  (char)pCurrentTime.tm_mon + 1;
	obj4247.bytDay   =  (char)pCurrentTime.tm_mday;
	obj4247.bytHour  =  (char)pCurrentTime.tm_hour;
	obj4247.bytMin   =  (char)pCurrentTime.tm_min;
	obj4247.bytSec   =  (char)pCurrentTime.tm_sec;
	obj4247.dwMeterTotle  =  0; // 暂不填充

	iRet = g_objSms.MakeSmsFrame((char*)&obj4247, sizeof(obj4247), 0x42, dataType, resbuf, sizeof(resbuf), iBufLen, CVT_8TO7);
	if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);	
	
}

 // 停住播放语音 （待）
// void CAutoRptStationNew::StopPlayCurSta()
// {
// 	PlaySound( NULL, NULL, 0 );	
// 	byte cmd[10] = {0};
// 	cmd[0]=0x17;
// 	PushDataToPhone(SRC_QIAN,3,1,(char*)cmd);
// }

ushort CAutoRptStationNew::GetLineId()
{
	return m_objStations.m_iLineNo;
}

BYTE CAutoRptStationNew::GetCheCi()
{
	return m_bytCheCi;
}

byte CAutoRptStationNew::GetCurLimitSpd()
{
	return m_ucCurLimitSpd;
}

bool CAutoRptStationNew::CenterLoadSta(ushort v_usLine)
{
	v_usLine = htons(v_usLine);
	PRTMSG( MSG_NOR, "CenterLoadSta %d\n",v_usLine);

	return LoadStations( m_objStations, true, v_usLine );
}

BYTE CAutoRptStationNew::GetUpDownSta2()
{
	if (0x00 == m_ucLstUpDown) return 0x02;
	if (0x01 == m_ucLstUpDown) return 0x01;
	if (0xff == m_ucLstUpDown) return 0x00;
	return 0;
}

void CAutoRptStationNew::SetWorkSta()
{
	static int sWork = 0; //营运非营运切换
	if ( !(sWork++%2) )
	{
		m_bytEnbRptSta = false;
		ShowAvTxt("非营运","其他");
	}
	else
	{
		m_bytEnbRptSta = true;
		ShowAvTxt("营运","");
	}

	//刚刚切换过来，车次类型未知，弄成其他就可以了.
	m_bytCheCiTmp = 0x0f ;//其他
	_SetTimer(&g_objTimerMng, SET_CHECI_TIMER, 5000,G_TmSetCheCi);	
}

int CAutoRptStationNew::GetUniStaId(int v_iCurId)
{
	return m_objStations.m_aryStation[v_iCurId].m_iUniId;
}

byte CAutoRptStationNew::GetAutoRptSta()
{
	return m_bytEnbRptSta;
}

void CAutoRptStationNew::SetAutoRptSta(byte m_bytSet)
{
	tag2QRptStationCfg cfg;
	GetSecCfg( &cfg, sizeof(cfg), offsetof(tagSecondCfg, m_uni2QRptStationCfg.m_obj2QRptStationCfg), sizeof(cfg) );
	cfg.m_bytRptSta = m_bytSet;
	SetSecCfg( &cfg, offsetof(tagSecondCfg, m_uni2QRptStationCfg.m_obj2QRptStationCfg), sizeof(cfg) );

	PRTMSG( MSG_NOR, "m_bytEnbRptSta=%d",m_bytSet);
}
	
//中心启动某条线路
void CAutoRptStationNew::hdl_4216(byte* buf, int len)
{	
	tag4216 *obj4216;
	obj4216 = (tag4216*)(buf);

	tag4256 obj4256;
	obj4256.m_usLindId = obj4216->m_usLindId;

	if(CenterLoadSta(obj4216->m_usLindId))
	{
		obj4256.m_bytRes = 1;
	}
	else
	{
		obj4256.m_bytRes = 0;
	}

	int iBufLen, iRet;
	char resbuf[1024] ={0};	

	iRet = g_objSms.MakeSmsFrame((char*)&obj4256, sizeof(obj4256), 0x42, 0x56, resbuf, sizeof(resbuf), iBufLen);
	if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);
}
	
//中心启动报站功能，让车台是否开启主动报站功能
void CAutoRptStationNew::hdl_4217(byte* buf, int len)	
{
	if (!buf)
		return;

	m_bytEnbRptSta = buf[0]-1;
	m_bytCheCi = buf[1];
	SetAutoRptSta(m_bytEnbRptSta);

	if(m_bytEnbRptSta)
	{
		ShowAvTxt("营运","");
		if(1== m_bytCheCi)	ShowAvTxt("出场-上班","");
		if(2== m_bytCheCi)	ShowAvTxt("回场-下班","");
	}
	else
	{
		if(1== m_bytCheCi)	ShowAvTxt("出场-上班","");
		if(2== m_bytCheCi)	ShowAvTxt("回场-下班","");
		if(3== m_bytCheCi)	ShowAvTxt("非营运-加油","");
		if(4== m_bytCheCi)	ShowAvTxt("非营运-保养","");
		if(5== m_bytCheCi)	ShowAvTxt("非营运-修车","");
		if(6== m_bytCheCi)	ShowAvTxt("非营运-包车","");
		if(7== m_bytCheCi)	ShowAvTxt("非营运-放空","");
		if(0x0f == m_bytCheCi)ShowAvTxt("非营运-其他","");
	}

	char res = 1;
	int iBufLen, iRet;
	char resbuf[1024] ={0};

	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS( objGps, true );
	Snd4254(objGps);

	iRet = g_objSms.MakeSmsFrame(&res, sizeof(res), 0x42, 0x57, resbuf, sizeof(resbuf), iBufLen);
	if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);
}

//应答结果(1) + 序号（1）+类型（1） ＋ [ 非营运类型（1）]
void CAutoRptStationNew::hdl_4220(byte* buf, int len)	
{
	if (!buf) return;

	if (!buf[0])
	{
		Show_diaodu("中心拒绝你的运营状态切换请求");
	}

	if (m_ucStaCnt != buf[1]) 
	{
		Show_diaodu("非本次运营状态切换请求");
		return;
	}

	if (2 == buf[2]) 
		m_bytEnbRptSta = true; // 开启报站

	if (1 == buf[2])
	{
		m_bytCheCi = buf[3];
		m_bytEnbRptSta = false;
	}

	if(m_bytEnbRptSta)
	{
		ShowAvTxt("营运","");
		if(1== m_bytCheCi)	ShowAvTxt("出场-上班","");
		if(2== m_bytCheCi)	ShowAvTxt("回场-下班","");
	}
	else
	{
		if(1== m_bytCheCi)	ShowAvTxt("出场-上班","");
		if(2== m_bytCheCi)	ShowAvTxt("回场-下班","");
		if(3== m_bytCheCi)	ShowAvTxt("非营运-加油","");
		if(4== m_bytCheCi)	ShowAvTxt("非营运-保养","");
		if(5== m_bytCheCi)	ShowAvTxt("非营运-修车","");
		if(6== m_bytCheCi)	ShowAvTxt("非营运-包车","");
		if(7== m_bytCheCi)	ShowAvTxt("非营运-放空","");
		if(0x0f == m_bytCheCi)ShowAvTxt("非营运-其他","");
	}

	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS( objGps, true );
	Snd4254(objGps);
}

//中心设置一键拨打多个电话的号码。
void CAutoRptStationNew::hdl_4219(byte* buf, int len)	
{
// 	if (!buf)return;
// 
// 	PRTMSG( MSG_DBG, "hdl_4219 cnt=%d,dataLen=%d\n",buf[0],len);
// 
// 	char telTmp[20] = {0};
// 	int TelCnt = buf[0];
// 	char res = 1;
// 	int iBufLen, iRet;
// 	char resbuf[1024] ={0};
// 
// 	if (len<TelCnt*20+1)
// 	{
// 		PRTMSG( MSG_ERR, "hdl_4219 data len err\n" );
// 		return;
// 	}
// 
// 	tag2QMDailCfg cfg;
// 	::GetSecCfg( &cfg, sizeof(cfg),offsetof(tagSecondCfg, m_uni2QMDailCfg.m_obj2QMDailCfg), sizeof(cfg) );
// 
// 	if (TelCnt>sizeof(cfg.m_szMultiDailTel)/sizeof(cfg.m_szMultiDailTel[0])){
// 		if (g_pView) g_pView->DbgShowStr("hdl_4219 Num Cnt err.");return;
// 	}
// 
// 	memset(&cfg,0,sizeof(cfg));
// 	for(int i = 0;i<TelCnt; i++)
// 	{
// 		memset(telTmp,0,sizeof(telTmp));
// 		memcpy(telTmp, buf+1+i*20, sizeof(telTmp));
// 		char *pos = strchr(telTmp,0x20);
// 		*pos = 0;
// 		if (g_pView) g_pView->DbgShowStr("hdl_4219 telTmp=%s",telTmp);
// 		memcpy(cfg.m_szMultiDailTel[i],telTmp,min(sizeof(cfg.m_szMultiDailTel[0]),sizeof(telTmp)) );
// 	}
// 
// 	if(::SetSecCfg( &cfg,offsetof(tagSecondCfg, m_uni2QMDailCfg.m_obj2QMDailCfg), sizeof(cfg) )) {
// 		res = 2;
// 	}
// 
// 	iRet = g_objSms.MakeSmsFrame(&res, sizeof(res), 0x42, 0x59, resbuf, sizeof(resbuf), iBufLen,NULL);
// 	if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);
}

//中心查询车台线路情况
void CAutoRptStationNew::hdl_4218()	
{
	int iBufLen, iRet;
	char resbuf[1024] ={0};
	int i;
	tagStationsNew objStaRead;
	byte res[512];

	//成功标志(1)+线路版本号(16) + 当前启用线路ID(2) + 线路条数(1)+（具体线路ID）（2*N）
	res[0] = 0x01;//成功

	for(i=0;i<sizeof(m_objLinesHead.ver);i++)
	{
		res[i+1]=0x20;
	}

	memcpy(res+1,m_objLinesHead.ver,strlen(m_objLinesHead.ver));

	res[17] = GetLineId()/256;
	res[18] = GetLineId()%256;
	res[19] = (byte)m_objLinesHead.m_wLineTotal;

	char buf[200] = { 0 };
	FILE* fp = fopen( SPRINTF(buf, "%sLineIno.wav", RPTSTATION_SAVE_PATH), "rb" );
	if (!fp)
	{	
		PRTMSG( MSG_ERR, "hdl_4218 fopen err\n");
		goto LOADLINE_END;
	}

	for( i = 0; i < m_objLinesHead.m_wLineTotal; i ++ )
	{
		if( fseek(fp, sizeof(m_objLinesHead) + i * sizeof(objStaRead), SEEK_SET) )
		{
			PRTMSG( MSG_ERR, "fseek %d line err!!!\n",i);
			goto LOADLINE_END;
		}

		if( 1 != fread( &objStaRead, sizeof(objStaRead), 1, fp ) )
		{
			PRTMSG( MSG_ERR, "Loadsta read Line err!!!\n");
			goto LOADLINE_END;
		}
		res[20+2*i]   = (ushort)(objStaRead.m_iLineNo)/256;
		res[20+2*i+1] = (ushort)(objStaRead.m_iLineNo)%256;
	}	

	if(fp)
		fclose( fp );
	
	memset(resbuf,0,sizeof(resbuf));
	iRet = g_objSms.MakeSmsFrame((char*)res, 20+2*m_objLinesHead.m_wLineTotal, 0x42, 0x58, resbuf, sizeof(resbuf), iBufLen);
	if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);	

	return;

LOADLINE_END:
	if(fp)
		fclose( fp );
	
	memset(res,0,sizeof(res));
	res[0] = 0x7f;
	iRet = g_objSms.MakeSmsFrame((char*)res, 1, 0x42, 0x58, resbuf, sizeof(resbuf), iBufLen);
	if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);	

	return;
}

int CAutoRptStationNew::Snd4254( const tagGPSData &v_objGps )
{
	tag4254 objSnd;
	tagQianGps objQianGps;
	char buf[ 500 ] = { 0 };
	int iBufLen = 0;
	int iRet = 0;
	BYTE bytTransType;  //业务类型
	BYTE bytDataType;   //数据类型
	
	// 遍历所有报警状态,生成报警数据
	g_objMonAlert.FillAlertSymb( g_objMonAlert.GetAlertSta(), &objSnd.m_bytAlertHi, &objSnd.m_bytAlertLo );
	
	objSnd.m_bytMonCentCount = 1; // 中心个数暂填1
	g_objMonAlert.GpsToQianGps( v_objGps, objQianGps );
	memcpy( &(objSnd.m_objQianGps), &objQianGps, sizeof(objSnd.m_objQianGps) );
	
	objSnd.m_usLineId  = htons(GetLineId());//如果线路还没有加载成功的话那么就是0.
	//状态字：A7 A6 A5 A4 A3 A2 A1 A0
	//A1A0: 0x00未知 0x01 下行 0x02 上行
	//A2: 是否在营运状态
	objSnd.m_bytBusSta  =  GetUpDownSta2();
	objSnd.m_bytBusSta |= (GetAutoRptSta() << 2);//状态字
	
	//车次类型(1):0x01: 出场  0x02: 回场 0x03: 加油 0x04 保养,0x05: 修车  0x06: 包车 0x07: 放空 0x0f 其他
	objSnd.m_bytCheCi = GetCheCi();	//车次类型
	
	objSnd.m_dwMeter = htonl(g_objDriveRecord.GetTotleMeter()); //总里程
	
	//objSnd.m_bytSatInUse = v_objGps.m_bytSateInUseCnt;
	objSnd.m_bytSatInUse = 4;

	objSnd.m_bytHDOP     = v_objGps.m_bytHDop;
	
	//objSnd.m_ulDriverId  = htonl(g_objDriveRecord.GetDrvNum());
	objSnd.m_ulDriverId = 0;
	
	PRTMSG(MSG_DBG, "S%02d:%02d:%02d,Sta=%02X,HL=%02X%02X,BUS=%02x/%02x,gps=%02x%02x\n",  
		objQianGps.m_bytHour+8,objQianGps.m_bytMin,objQianGps.m_bytSec,
		objQianGps.m_bytMix, objSnd.m_bytAlertHi, objSnd.m_bytAlertLo,
		objSnd.m_bytBusSta,  objSnd.m_bytCheCi,
		objSnd.m_bytSatInUse,objSnd.m_bytHDOP);
	
	bytTransType = 0x42;
	bytDataType  = 0x54;
	
	iRet = g_objSms.MakeSmsFrame((char*)&objSnd, sizeof(objSnd), bytTransType, bytDataType, buf, sizeof(buf), iBufLen);
	if( iRet ) return iRet;

	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, DATASYMB_0154 );
	if( iRet ) return iRet;
}

void CAutoRptStationNew::ShowLineVer()
{
	int i;
	tagStationsNew objStaRead;

	char buf[200] = { 0 };
	FILE* fp = fopen( SPRINTF(buf, "%sLineIno.wav", RPTSTATION_SAVE_PATH), "rb" );
	
	if (!fp)
	{
		Show_diaodu("线路文件不存在");
		return;
	}

	char szVer[100] = {0};
	char szVar[16]  = {0};
	sprintf(szVer,"版本号:%s,条数(%d):",m_objLinesHead.ver,m_objLinesHead.m_wLineTotal);

	for( i = 0; i < m_objLinesHead.m_wLineTotal; i ++ )
	{
		if( fseek(fp, sizeof(m_objLinesHead) + i * sizeof(objStaRead), SEEK_SET) )
		{
			PRTMSG(MSG_ERR, "fseek %d line err!\n",i);
			Show_diaodu("找不到线路");
			goto LOADLINE_END;
		}

		if( 1 != fread( &objStaRead, sizeof(objStaRead), 1, fp ) )
		{
			PRTMSG(MSG_ERR, "Loadsta read Line err!\n");
			Show_diaodu("读取线路错误");
			goto LOADLINE_END;
		}

		memset(szVar,0,sizeof(szVar));
		sprintf(szVar,"%d ",objStaRead.m_iLineNo);
		strcat(szVer,szVar);
	}

	Show_diaodu(szVer);

LOADLINE_END:
	if(fp)fclose( fp );
}

void CAutoRptStationNew::ShowLineAV()
{
	tag2QRptStationCfg cfg;
	GetSecCfg( &cfg, sizeof(cfg), offsetof(tagSecondCfg, m_uni2QRptStationCfg.m_obj2QRptStationCfg), sizeof(cfg) );
	
	char str[100] = {0};
	sprintf(str,"线路:%d(Cfg)/%ld", cfg.m_iLineNo, m_objStations.m_iLineNo);
	Show_diaodu(str);	
}


byte CAutoRptStationNew::GetUpDownSta(int v_iCurId)
{
	if(!memcmp("下行",m_objStations.m_aryStation[v_iCurId].m_szName,4))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//获取进出站的状态, 0: 进站， 1：出站
bool CAutoRptStationNew::GetInOutSta(int v_iCurId)
{
	if(strstr(m_objStations.m_aryStation[v_iCurId].m_szName,"进站"))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

byte CAutoRptStationNew::GetStaLimitSpd(int v_iCurId)
{
	return m_objStations.m_aryStation[v_iCurId].m_bytSpeed;
}

//获取站点的属性:始发站/终点站/中间站/拐弯/出库/入库
byte CAutoRptStationNew::GetStaAttr(int v_iCurId)
{
	return m_objStations.m_aryStation[v_iCurId].m_bytStaAttr;
}

#endif


