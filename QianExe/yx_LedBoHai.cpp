#include "yx_QianStdAfx.h"

#if USE_LEDTYPE == 1

#define BOHAILED_PROTOCOLVER 0x21

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-BohiLed      ")

void *G_ThreadLedRead(void* arg)
{
	g_objLedBohai.P_ThreadLedRead();
}

void *G_ThreadLedwork(void* arg)
{
	g_objLedBohai.P_ThreadLedWork();
}

int G_JugSymbData( void* v_pElement, void* v_pSymb, DWORD V_dwSymbLen )
{
// 	PRTMSG( MSG_DBG, "Compare:" );
// 	g_objLedBohai.PrintData( MSG_DBG, (char*)v_pElement, V_dwSymbLen );
// 	g_objLedBohai.PrintData( MSG_DBG, (char*)v_pSymb, V_dwSymbLen );
	return memcmp( v_pElement, v_pSymb, V_dwSymbLen );
}

CLedBoHai::CLedBoHai()
{
	m_objLedMenu.m_usMenuID = 0;
	m_objLedStat.wait_dwnID = 0;
	m_objLedStat.gpsTime = 0;
	m_objUpSta.updating = FALSE;
	m_bytVeSta = 0;
	
	m_md5ok = true;
	m_bAccValid = false;
	m_bAccTurnOn = false;
	m_dwAccToOnTm = GetTickCount();
	m_iAccSta = 0;
	
	// 建立校验表
	m_cnCRC_CCITT = 0x1021;	
	build_table16(m_cnCRC_CCITT);

	m_hComPort = -1;
	m_bEvtExit = false;
	m_bEvtWork = false;
	
	memset( m_szUpgrdReqBuf, 0, sizeof(m_szUpgrdReqBuf) );
	m_iUpgrdReqBufLen = 0;
	m_wUpgrdSndTimes = 0;	
	m_usToSavLedSetID = 0;

	memset(m_szRecvFrameBuf, 0, sizeof(m_szRecvFrameBuf));
	m_iRecvFrameLen = 0;

#if SAVE_LEDDATA == 1
	m_logtype = 0;		//日志类型
#endif
}

CLedBoHai::~CLedBoHai()
{
	
}

int CLedBoHai::Init()
{
	if(m_bytLedCfg & 0x08)
	{
		// 获取车台的ID和密钥
		byte carid[20] = {0};
		ulong car_id;
		cmd5.get_car_id(carid);
		memcpy(&car_id, carid, 4);
		memcpy(m_szCarKey, carid+4, 16);
	}
	
	tag2QLedCfg	obj2QLedCfg;	
	GetSecCfg(&obj2QLedCfg,sizeof(obj2QLedCfg), offsetof(tagSecondCfg, m_uni2QLedCfg.m_obj2QLedCfg), sizeof(obj2QLedCfg) );

	m_objLedMenu.m_LedParam.m_usParamID = obj2QLedCfg.m_usLedParamID;
	m_bytPowerOnWaitTm = 15;//obj2QLedCfg.m_bytLedPwrWaitTm;
	m_bytLedCfg = obj2QLedCfg.m_bytLedConfig;
	m_usBlackWaitTm = obj2QLedCfg.m_usLedBlkWaitTm;
	m_usTURNID = obj2QLedCfg.m_usTURNID;
	m_usTURNID = obj2QLedCfg.m_usNTICID;

	m_usTURNID = 0;	
	m_bytCurSta = stINIT;

#if USE_COMBUS == 0		// 若使用串口扩展盒，则不打开串口
	//打开串口
	if( false == ComOpen())
		return ERR_COM;

	//创建读线程线程
	if( pthread_create(&m_pthreadRead, NULL, G_ThreadLedRead, NULL) != 0 )
	{
		PRTMSG(MSG_ERR, "create Led thread failed!\n");
		return ERR_THREAD;
	}
#endif

	//创建工作线程
	if( pthread_create(&m_pthreadWork, NULL, G_ThreadLedwork, NULL) != 0 )
	{
		PRTMSG(MSG_ERR, "create Led thread failed!\n");
		return ERR_THREAD;
	}
	
	m_bEvtWork = true;
	PRTMSG(MSG_DBG, "Create Led thread succ!\n");

	return 0;
}

int CLedBoHai::Release()
{
	m_bEvtExit = true;

#if USE_COMBUS == 0
	if( m_hComPort != -1)
		close(m_hComPort);
#endif
}

bool CLedBoHai::ComOpen()
{
	struct termios options;

	m_hComPort = open("/dev/ttySIM1", O_RDWR);

	if( m_hComPort < 0 )
	{
		PRTMSG(MSG_ERR, "open Led com failed!\n");
		return false;
	}

	if(tcgetattr(m_hComPort, &options) != 0)
	{
		perror("GetSerialAttr");
		return false;
	}
	
	options.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	options.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	options.c_oflag &= ~OPOST;
	
	cfsetispeed(&options,B9600);	//设置波特率，调度屏设置波特率为9600
	cfsetospeed(&options,B9600);
	
	if (tcsetattr(m_hComPort,TCSANOW,&options) != 0)   
	{ 
		perror("Set com Attr"); 
		return false;
	}

	PRTMSG( MSG_NOR, "LED open com port succ\n" );

	return true;
}

bool CLedBoHai::ComClose()
{
#if USE_COMBUS == 0
	if(m_hComPort != -1)
		close(m_hComPort);
#endif
	return true;
}

void CLedBoHai::LedTurnToWork()
{
	m_bAccValid = true;
	m_dwAccToOnTm = GetTickCount();
	m_bAccTurnOn = true;
	
	m_bEvtWork = true;

	PRTMSG( MSG_NOR, "Led Turn to Work\n" );
}

void CLedBoHai::LedTurnToSleep()
{
	m_bAccValid = false;
	m_dwAccToOnTm = GetTickCount();
	
	m_bEvtWork = false;

	PRTMSG( MSG_NOR, "Led Turn to Sleep\n" );
}

void CLedBoHai::P_ThreadLedRead()
{
	ulong readed = 0;
	char buf[ALABUF] = {0};
	char frm[ALABUF];

	while(1)
	{
		if( true == m_bEvtExit )
			break;
				
		if( false == m_bEvtWork )
		{
			usleep( 600000 );
			continue;
		}

		//PRTMSG( MSG_DBG, "LED reading...\n" );

		readed = read(m_hComPort, buf, sizeof(buf));
		if(readed <= 0)
			continue;

//  		PRTMSG( MSG_DBG, "Rcv LED Data:" );
//  		PrintData( MSG_DBG, buf, readed );

		//保证只有堆栈中只有一条信息 (But why?) xun del
// 		if( !m_objReadComMng.IsEmpty()) // xun modify
// 			m_objReadComMng.Release();

		if( readed > 0 )
 		{
			//PRTMSG( MSG_DBG, "Deal Data From LED\n" );
#if BOHAILED_PROTOCOLVER == 0x12
 			DealComData_V1_2(buf, readed);
#endif

#if BOHAILED_PROTOCOLVER == 0x21
			DealComData_V2_1(buf, readed);
#endif

#if BOHAILED_PROTOCOLVER == 0x22
			DealComData_V2_2(buf, readed);
#endif
 		}

#if SAVE_LEDDATA == 1
		m_logtype = 2;
		char log_buf[ALABUF*3] = {0};
		char temp[3] = {0};

		if (readed > 0 )
		{
			for (ulong l=0; l<readed; l++) 
			{
				sprintf(temp, "%02x ", (BYTE)buf[l]);
				strcat(log_buf, temp);
			}
			
			save_log((char *)log_buf, readed*3);
		}
#endif	
	}
}

void CLedBoHai::P_ThreadLedWork()
{
	sleep(1);
	
	char buf[1024] = { 0 };
	int len;
	WORD_UNION tmp;
	static int excpCnt=0; //异常出现次数
	ushort k = 0;
	
	while(1)
	{
		if( true == m_bEvtExit )
			break;
		
		if( false == m_bEvtWork )
		{
			usleep( 1000000 );
			continue;
		}
		else
		{
			usleep(1000000);
		}
		
		if( m_objLocalCmdQue.pop( (unsigned char*)buf) )
		{
			switch( buf[0] )
			{
			case LED_CMD_LIGHT:
				light();
				break;
				
			case LED_CMD_ALERM:
				{
					light();
				
					usleep(300000);
					unsigned short usPrid = LEDALARM_PRID;

					alarm( (char*)&usPrid, sizeof(usPrid) ); // 报警持续时长参数，高字节在前？
				}
				break;
				
			case LED_CMD_ALERMCANCEL:
				{
					usleep(300000);
					exit_alarm();
				}
				break;
				
			case LED_CMD_INIT:
				{
					m_bytCurSta = stLOG;
				}
				break;
				
			case LED_CMD_CARMOVE:
				if( m_bytLedCfg & 0x02 )
				{
					m_bytVeSta = 2;
					light();
				}
				break;
				
			case LED_CMD_CARSILENT:
				if( m_bytLedCfg & 0x02 )
				{
					// 延时黑屏
					m_bytVeSta = 1;
					m_dwVeStaTick = GetTickCount();
				}
				break;
				
			default:
				break;
			}
		}
		
		switch(m_bytCurSta)
		{
		case stINIT:	//Led初始化状态
			{
				m_objLedStat.comuErr = 0;
				m_iAccSta = 1;		//ACC正常点火
			
				PRTMSG(MSG_NOR, "LED stINIT wait tm %d\n", m_bytPowerOnWaitTm);

				// ACC状态
				byte bytSta = 0;
				IOGet(IOS_ACC, &bytSta);
				if( bytSta == IO_ACC_ON )
				{
					LedTurnToWork();
				}
				else
				{
					LedTurnToSleep();
				}

				if( g_objMonAlert.JugFootAlert() )
				{
					light();

					//通知LED抢劫报警
					const unsigned short ledAlertTm = LEDALARM_PRID;//LED显示抢劫报警时长,单位为秒
				
					buf[0] = (BYTE)ledAlertTm;
					buf[1] = (BYTE)ledAlertTm>>8;					
					alarm(buf,2);
				}

				sleep(m_bytPowerOnWaitTm);

				for(k=0; k<3;k++) // xun, k<30改为k<3,没必要这么久,因为若失败则下面会跳到异常状态
				{
					len = query_state(buf);

					if( true == m_bEvtExit )
						break; 

					if(len)
					{
						break;
					}
					
					sleep(5);
				} 
				
				//查询LED状态并进行管理工作
				{
					//判断状态：LED屏串口通信初始化
					if (!len)
					{
						//LED未工作状态
						m_objLedStat.symb = 0;
						m_bytCurSta = stEXCP; //普通异常
						PRTMSG(MSG_NOR, "Led turn to stEXCP 1!\n");
						
						break;
					}
					else
					{
						//设置状态参数
						m_objLedStat.symb = 1;

						//设置led状态
						int frm_cnt = 0;
						m_objLedStat.lighten = buf[frm_cnt++];
						m_objLedStat.char_Excp = buf[frm_cnt++];
						m_objLedStat.set_Excp = buf[frm_cnt++];

						tmp.bytes.high = buf[frm_cnt++];
						tmp.bytes.low = buf[frm_cnt++];
						m_objLedStat.badPt_Ct = tmp.word;

						m_objLedStat.curLedTime.nYear = buf[frm_cnt++];
						m_objLedStat.curLedTime.nMonth = buf[frm_cnt++];
						m_objLedStat.curLedTime.nDay = buf[frm_cnt++];
						m_objLedStat.curLedTime.nWeekDay = buf[frm_cnt++];
						m_objLedStat.curLedTime.nHour = buf[frm_cnt++];
						m_objLedStat.curLedTime.nMinute = buf[frm_cnt++];
						m_objLedStat.curLedTime.nSecond = buf[frm_cnt++];
#if BOHAILED_PROTOCOLVER == 0x12
						m_objLedStat.mail_Ct = BYTE(buf[frm_cnt++]);
#endif
#if BOHAILED_PROTOCOLVER == 0x21 || BOHAILED_PROTOCOLVER == 0x22
						m_objLedStat.mail_Ct = BYTE(buf[frm_cnt++]) * ushort(256); // 不知道字节序到底怎样，待
						m_objLedStat.mail_Ct += BYTE(buf[frm_cnt++]);
#endif
						m_objLedStat.mode = buf[frm_cnt++];

						tmp.bytes.high = buf[frm_cnt++];
						tmp.bytes.low = buf[frm_cnt++];
						m_objLedStat.curID = tmp.word;

						m_objLedStat.curIerm = buf[frm_cnt++];
						
						//若有字库异常或者设置异常
						if (  m_objLedStat.char_Excp || m_objLedStat.set_Excp)
						{
							//通知中心,返回0x57
							char buf2[100];
							int len2 = 0;

							buf2[len2++] = 1;//应答成功
							memcpy(buf2+len2,buf,len);
							len2+= len;
							SendCenterMsg(0x57,buf2,len2);
						}
					}
				}

				//先取LED节目单及(LED不提供返回屏参数)
				len = list_all( buf );
				if(len)
				{
					WORD_UNION tmp;
					int frm_cnt=0;

#if BOHAILED_PROTOCOLVER == 0x12
					m_objMail.m_bytMailIDCnt = buf[frm_cnt++];
#endif
#if BOHAILED_PROTOCOLVER == 0x21 || BOHAILED_PROTOCOLVER == 0x22
					m_objMail.m_bytMailIDCnt = BYTE(buf[frm_cnt++]) * ushort(256);
					m_objMail.m_bytMailIDCnt += BYTE(buf[frm_cnt++]);
#endif
					for(k=0; k<m_objMail.m_bytMailIDCnt; k++)
					{
						tmp.bytes.high = buf[frm_cnt++];
						tmp.bytes.low = buf[frm_cnt++];
						m_objMail.m_aryMailID[k] = tmp.word;
					}
				}
#ifdef DELAYLEDTIME
				//取中心节目单及屏参数(配置区中存储节目单)
				tmp.word = m_objLedMenu.m_usMenuID;
				buf[0] = tmp.bytes.high;
				buf[1] = tmp.bytes.low;
				SendCenterMsg(0x41,buf,2);
#endif						
				//校对时间
				AdjustTime();

				if (m_bytCurSta != stEXCP) 
				{
					m_dwReNewTick = GetTickCount();	//第一次更新一下节目单时间点
					m_bytCurSta = stWORK;			//转入正常工作状态
					PRTMSG(MSG_NOR, "Led turn to work state\n");
				}
			}
		break;
				
		case stWORK://正常工作状态
			{
				//PRTMSG( MSG_DBG, "LEDWORKSTA: %d, %d, %d\n", int(m_bAccTurnOn), int(m_bAccValid), m_bytVeSta );

				if(m_bAccTurnOn)
				{
					if( GetTickCount() - m_dwAccToOnTm < 30000 )
					{
						break;
					}
					
					tmp.word = m_objLedMenu.m_usMenuID;
					buf[0] = 0;
					buf[1] = 0;
					
					SendCenterMsg(0x41,buf,2);
					m_dwReNewTick = GetTickCount();//第一次更新一下节目单及异常时间点
					
					m_bAccTurnOn = false;
					
					//校对时间！！有的车台熄火,打火过快,导致车台没有收到ACC OFF信号,而延时还没结束,LED电源却关掉,这样造成LED不亮
					AdjustTime();
				}
				
				//ulong l = m_objReadComQue.pop((unsigned char *)buf);
				DWORD dwDataLen = 0;
				byte  bytlvl;
				DWORD dwPushTm = 0;
				if( !m_objReadComMng.PopSymbData(G_JugSymbData, const_cast<void*>((const void*)("PWON")),
					4, bytlvl, (DWORD)(sizeof(buf)), dwDataLen, buf, dwPushTm) )
				{// xun modify from "PopData" to "PopSymbData"
					//通知中心，LED复位
					buf[0] = 7;
					SendCenterMsg(0x57, buf, 1);
					m_iAccSta = 2;		//	ACC正常，但是LED复位
					AdjustTime();		
				}
								
				//车辆静止通知时，延时关屏
				if(m_bytVeSta ==1 && (GetTickCount() - m_dwVeStaTick)/1000 >= m_usBlackWaitTm)
				{
					m_bytVeSta = 0;
					black();
				}
				
				//定时发送节目单请求及校时
				if (GetTickCount() - m_dwReNewTick > RENEWMENUTICK)
				{
					m_dwReNewTick = GetTickCount();
					
					//当ACC有效时才进行校时，无效时由于LED被断电，没有必要校时和请求节目单
					if (m_bAccValid) 
					{
#ifndef LEDTIMEDOWN
						//LED定时发送节目单请求
						tmp.word = m_objLedMenu.m_usMenuID;
						buf[0] = 0;
						buf[1] = 0;
						SendCenterMsg(0x41,buf,2);
#endif 							
						//校对时间！！
						AdjustTime();
					}
				}
				
				if( !DealCenterMsg() )	//与led通信出错，转入异常
				{
					
					if (m_bAccValid)	// ACC有效时才转入异常
					{	
						m_bytCurSta = stEXCP; 
						PRTMSG(MSG_NOR, "Led turn to stEXCP 2!\n");
					}
				}
				
				//加上按时间段请求节目单
#ifdef LEDTIMEDOWN
				tagGPSData gps(0); 
				int iRet = GetCurGps( &gps, sizeof(gps), GPS_REAL );
				
				if ( gps.valid==0x41 )
				{
					if (gps.nHour==DOWNTIME1 ||gps.nHour==DOWNTIME2)
					{
						//LED定时发送节目单请求
						tmp.word = m_objLedMenu.m_usMenuID;
						buf[0] = 0;
						buf[1] = 0;
						SendCenterMsg(0x41,buf,2);
					}
				}
#endif
			}
			break;
				
		case stEXCP://普通异常状态
			{			
				//acc开后出现10次以上则报错,报错标志置位后禁止进行节目下载
				excpCnt++;
				if (excpCnt > 3 && m_bAccValid)	//ACC有效时才向中心报告，避免屏异常后ACC熄火会定时向中心报告异常
				{
					//通知中心，LED错误
					buf[0] = 6;
					SendCenterMsg(0x57, buf, 1);
					excpCnt = 0;
					
					//错误标志位
					m_objLedStat.comuErr = 1;
				}
				sleep(5);
				m_bytCurSta = stINIT;//跳到初始化状态
			}
			break;
				
		case stDOWN://远程升级状态
			{
				//接收中心消息处理
				DealCenterMsg();
				
				if( m_objUpSta.updating ) //如果在升级状态
				{
					if( GetTickCount() - m_dwUpgrdSndTm >= 30000 ) // 间隔超过30s
					{
						if( m_wUpgrdSndTimes >= 5 ) // 请求达到5次
						{
							// 退出升级
							buf[0] = 2;
							SendCenterMsg(0x62, buf, 1); // 升级失败通知
							m_objUpSta.updating = FALSE;
							m_bytCurSta = stINIT; // xun,为何回到初始状态? 是因为升级时是关屏的,升级失败仍然要重新亮屏
						}
						else
						{
							// 重发该包请求
							if( m_iUpgrdReqBufLen <= sizeof(m_szUpgrdReqBuf) )
							{
								SendCenterMsg(0x61, m_szUpgrdReqBuf, m_iUpgrdReqBufLen ); // 重新请求
							}
							m_wUpgrdSndTimes++;
							m_dwUpgrdSndTm = GetTickCount();
						}
					}
				}
			}
			break;
				
		case stLOG://TCP登陆
			{
				//如果在升级状态下,则请求当前包?并进入升级状态
				if (m_objUpSta.updating)
				{
					black();
					
					//请求当前包
					len=0;
					buf[len++] = m_objUpSta.ver_Cnt;
					
					memcpy(buf+len, m_objUpSta.ver, m_objUpSta.ver_Cnt);
					len += m_objUpSta.ver_Cnt;
					
					buf[len++] = m_objUpSta.packet;
					
					SendCenterMsg(0x61, buf, len);
					m_bytCurSta = stDOWN;
					
					break;
				}
				
				//TCP重登陆,需要先请求节目单,校对时间
				buf[0] = 0;
				buf[1] = 0;
				SendCenterMsg(0x41,buf,2);
				
				m_bytCurSta = stWORK;
			}
			break;
			
		default://保留
			break;
		}
	}
}

void CLedBoHai::PrintData( int v_iMsgType, char* v_pData, int v_iDataLen )
{
	if( !v_pData || v_iDataLen < 1 || !v_iMsgType ) return;

	char buf[1024] = { 0 };
	int bufsize = sizeof(buf);
	for( int i = 0; i < v_iDataLen && 3 * i < bufsize; i ++ )
	{
		sprintf( buf + i * 3, "%02x ", BYTE(v_pData[i]) );
	}

	PRTMSG( 1, "%s\n", buf );
}

bool CLedBoHai::DealCenterMsg()
{
	char buf[2048] = { 0 };
	int len=0;
	char buf_snd[2048];
	int len_snd=0;
	int frm_cnt = 1;
	static int msgErr=0; //广告信息写入错误次数 5次为上限
	static tagLedMenuAns ans_LedMenu;//下载节目单应答

	len = m_objCenterCmdQue.pop((unsigned char *)buf);
	if (len)
	{	
		PRTMSG( MSG_DBG, "Rcv Cent LED Data:" );
		PrintData( MSG_DBG, buf, len );

		len--;
		
		switch(buf[0])
		{
		case 0x01:////下载节目单应答
		case 0x11://节目单发送请求
			{
				if (m_objUpSta.updating)
				{
					PRTMSG(MSG_NOR, "Deal 3911, now is updating!\n");
					break;
				}
				if ( !(m_bytLedCfg & 0x04) && m_objLedStat.comuErr)
				{
					PRTMSG(MSG_ERR, "Deal 3911, Led config err or comu err!\n");
					break;
				}

				if (!m_bAccValid)	//ACC无效时不进行下面的流程
				{
					PRTMSG(MSG_NOR, "Deal 3911, Acc invalid!\n");
					break;	
				}
				
				frm_cnt =1;
				m_dwReNewTick = GetTickCount();			
				
				WORD_UNION tmp;
				
				//节目单ID(2)
				tmp.bytes.high = buf[frm_cnt++];
				tmp.bytes.low = buf[frm_cnt++];
				ans_LedMenu.m_usMenuID = tmp.word;
#ifdef DELAYLEDTIME
				if (ans_LedMenu.m_usMenuID == m_objLedMenu.m_usMenuID)
				{
					if (buf[0] == 0x11)
					{
						buf_snd[0] =1;
						SendCenterMsg(0x51,buf_snd,1);//应答：1 成功
					}
					break; 
				}
#endif

				// 屏幕设置ID
				tmp.bytes.high = buf[frm_cnt++];
				tmp.bytes.low = buf[frm_cnt++];
				ans_LedMenu.m_usParamID = tmp.word;

				if (ans_LedMenu.m_usParamID != m_objLedMenu.m_LedParam.m_usParamID)
				{				
					m_usToSavLedSetID = ans_LedMenu.m_usParamID;					
					SendCenterMsg(0x44,NULL,0);
				}
			
#if BOHAILED_PROTOCOLVER == 0x21 || BOHAILED_PROTOCOLVER == 0x22

				//转场信息ID(2)
				tmp.bytes.high = buf[frm_cnt++];
				tmp.bytes.low = buf[frm_cnt++];
				ans_LedMenu.m_usTURNID = tmp.word;
				
				if((ans_LedMenu.m_usTURNID != m_usTURNID) && (ans_LedMenu.m_usTURNID != 0))
				{
					ushort TurnId = htons(ans_LedMenu.m_usTURNID);
					memcpy(buf_snd,(char*)&(TurnId),2);
					SendCenterMsg(0x36,buf_snd,2);
				}

				//通知信息ID(2)
				tmp.bytes.high = buf[frm_cnt++];
				tmp.bytes.low = buf[frm_cnt++];
				ans_LedMenu.m_usNTICID =tmp.word;
				if((ans_LedMenu.m_usNTICID != m_usNTICID) && (ans_LedMenu.m_usNTICID != 0))
				{
					ushort NticId = htons(ans_LedMenu.m_usNTICID);
					memcpy(buf_snd,(char*)&(NticId),2);
					SendCenterMsg(0x38,buf_snd,2);
				}
		
#endif

				// 设置字
				ans_LedMenu.m_bytConfig = buf[frm_cnt++];
				// 个数
				ans_LedMenu.m_bytDownIdCnt = buf[frm_cnt++];

				// 根据设置字处理
				if (ans_LedMenu.m_bytConfig & 0x1)	//清除原先节目单
				{
					m_objLedMenu.Init();
					m_objMail.Init();
					
					//删除led屏内所有信息
					delete_all();
				}

				m_objLedMenu.m_bytConfig = ans_LedMenu.m_bytConfig;

				if(ans_LedMenu.m_bytConfig & 0x4)	//带即时信息
				{
					PRTMSG(MSG_NOR, "Deal 3911, send temp info to Led\n");
					
					//透传
#if BOHAILED_PROTOCOLVER == 0x12
					frm_cnt = 7;
					len_snd = len - 6; // ? xun modify from 5 to 6
#endif
#if BOHAILED_PROTOCOLVER == 0x21 || BOHAILED_PROTOCOLVER == 0x22
					frm_cnt = 11;
					len_snd = len - 10;
#endif
					memcpy(buf_snd, buf+frm_cnt, len_snd);
					
					if (!show(buf_snd,len_snd))	
					{
						PRTMSG(MSG_ERR, "send tmep info err!\n");
						return false;
					}
				}
// 				else if (ans_LedMenu.m_bytConfig&0x2)	//带点播消息
// 				{
// 					frm_cnt = 7;
// 					len_snd =11;
// 					m_objLedMenu.m_LedDispMsg.m_bytLvl = buf[frm_cnt++];
// 					m_objLedMenu.m_LedDispMsg.m_bytExitWay = buf[frm_cnt++];//0只通过退出指令退出，1点播结束时间，2点播播放时间，3点播循环次数。
// 					
// 					DWORD_UNION tmp_dword;
// 					tmp_dword.bytes.b0 = buf[frm_cnt++];
// 					tmp_dword.bytes.b1 = buf[frm_cnt++];
// 					tmp_dword.bytes.b2 = buf[frm_cnt++];
// 					tmp_dword.bytes.b3 = buf[frm_cnt++];
// 					m_objLedMenu.m_LedDispMsg.m_wEndTm = tmp_dword.dword;
// 					
// 					tmp.bytes.high = buf[frm_cnt++];
// 					tmp.bytes.low = buf[frm_cnt++];
// 					m_objLedMenu.m_LedDispMsg.m_usPlayTm = tmp.word;
// 					
// 					m_objLedMenu.m_LedDispMsg.m_bytDISPIDCt = buf[frm_cnt++];
// 					for(int i=0; i<m_objLedMenu.m_LedDispMsg.m_bytDISPIDCt; i++)
// 					{
// 						tmp.bytes.high = buf[frm_cnt++];
// 						tmp.bytes.low = buf[frm_cnt++];
// 						m_objLedMenu.m_LedDispMsg.m_aryDISPID[i] = tmp.word;
// 					}
// 					len_snd+=2 * m_objLedMenu.m_LedDispMsg.m_bytDISPIDCt;
// 					
// 					//拷贝点播信息内容
// 					memcpy(buf_snd, (buf+frm_cnt-len_snd), len_snd);
// 					if (!display(buf_snd,len_snd)) 
// 						return false;
// 				}
				else if (ans_LedMenu.m_bytConfig==0 || ans_LedMenu.m_bytConfig==0x01) 
				{//只带信息ID列表		
					//判断，如果信息ID个数超过125个，则应答错误
					if (ans_LedMenu.m_bytDownIdCnt> MAILCNT_BOHAI && buf[0] == 0x11)
					{
						buf_snd[0] = 2;//失败
						SendCenterMsg(0x51,buf_snd,1);//应答：2失败
						break;
					}
					
#if BOHAILED_PROTOCOLVER == 0x12
					frm_cnt = 7;
#endif

#if BOHAILED_PROTOCOLVER == 0x21 || BOHAILED_PROTOCOLVER == 0x22
					frm_cnt = 11;
#endif
					//信息ID个数(1)下载
					if (ans_LedMenu.m_bytDownIdCnt)
					{
						m_objLedMenu.m_bytMailIDCnt = ans_LedMenu.m_bytDownIdCnt;
						
						for (ushort i=0; i<m_objLedMenu.m_bytMailIDCnt; i++)
						{
							tmp.bytes.high = buf[frm_cnt++];
							tmp.bytes.low = buf[frm_cnt++];
							
							m_objLedMenu.m_aryMailID[i] = tmp.word;
						}
					
						int iSame=0;
						for(ushort k=0; k<m_objMail.m_bytMailIDCnt; k++)
						{
							//遍历LEDMENU中的信息，找不到的则进行删除
							iSame=0;
							for (ushort l=0; l<m_objLedMenu.m_bytMailIDCnt; l++)
							{
								if(m_objMail.m_aryMailID[k] == m_objLedMenu.m_aryMailID[l])
								{
									iSame=1;
									break;
								}
							}

							if (iSame == 0)//在LEDMENU中无法找到
							{
								buf_snd[0] = 1;
								tmp.word = m_objMail.m_aryMailID[k];
								buf_snd[1] = tmp.bytes.high;
								buf_snd[2] = tmp.bytes.low;

								if ( delete_msg(buf_snd,3) )
								{
									m_objMail.m_bytMailIDCnt--;
									
									for (int j=k; j<m_objMail.m_bytMailIDCnt; j++)
									{
										m_objMail.m_aryMailID[j] = m_objMail.m_aryMailID[j+1];
									}
									k--;
								}								
							}
						}

						//获得需下载信息个数
						m_objLedStat.wait_dwnID = 0;
						
						for (ushort k=0; k<m_objLedMenu.m_bytMailIDCnt; k++)
						{
							iSame=0;
							for (int l=0;l<m_objMail.m_bytMailIDCnt;l++)
							{
								if (m_objMail.m_aryMailID[l] == m_objLedMenu.m_aryMailID[k])
								{
									iSame=1;
									break;
								}
							}
							if (iSame == 0)
							{
								m_objLedStat.wait_dwnID++;
							}
						}
						
						SendMailID();
					}
				}
				
				if (buf[0] == 0x11)
				{
					buf_snd[0] =1;
					SendCenterMsg(0x51,buf_snd,1);
				}
				;
			}
			break;

		case 0x02://根据车台请求的信息id下载信息内容的应答
			{
				//当处于升级状态下或者通信错误状态，不插入消息
				if (m_objUpSta.updating)
					break;

				if ( !(m_bytLedCfg & 0x04) && m_objLedStat.comuErr ) 
					break;	

				if (!m_bAccValid)
					break;		//ACC无效时不进行下面的流程
				
				unsigned short dwnMailID;//当前下载的信息ID
				WORD_UNION tmp;
				tmp.bytes.high = buf[2];
				tmp.bytes.low = buf[3];
				dwnMailID = tmp.word;
				
				//当该信息ID已经在LED中存在了，则不下载到LED中			
				for (int i=0; i<m_objLedMenu.m_bytMailIDCnt; i++)
				{
					if (dwnMailID == m_objLedMenu.m_aryMailID[i])
					{	
						//如果在m_objMail中已经有该信息ID，则不操作
						int iSame=0;
						for(int j=0; j<m_objMail.m_bytMailIDCnt; j++)
						{
							if(m_objMail.m_aryMailID[j]==dwnMailID)
							{
								iSame=1;
								break;
							}
						}
						if(iSame == 0)
						{
							if(m_bytLedCfg & 0x08)
							{
								//进行MD5校验
								//先获得MD5值
								memcpy((char*)m_md5, buf+1+len-32, 32);
								len-=32;

								Md5Decrypt demd5;
								demd5.DesDecrypt((char*)m_md5, (char*)m_md5, 32, m_szCarKey, 16);
								
								//获取下载文件实际的md5值
								CMd5 cmd5;
								char md5_new[32] = {0};
								
								if (!cmd5.MD5String(buf+1, len, md5_new))
									cmd5.MD5String(buf+1, len, md5_new);

								//比较两个md5是否相等
								m_md5ok = (0==strncmp(md5_new,(char*)m_md5,32)) ? true : false;
								PRTMSG(MSG_DBG, "m_md5ok = %d\n", m_md5ok);
							}
							
							if (!m_md5ok)//md5校验失败
							{
								msgErr++;
								if (msgErr<5)
								{
									//07.9.12  当出现插入错误时，还是请求，也报错
									if ( m_objLedStat.wait_dwnID )
									{
										SendMailID();
									}
								}
								else
								{
									msgErr = 0;
								}
							
								return true;
							}
							
							if (!insert(buf+1,len)) 
							{
								msgErr++;
								if (msgErr<5) 
								{
									//07.9.12  当出现插入错误时，还是请求，也报错
									if ( m_objLedStat.wait_dwnID )
									{
										SendMailID();
									}
									
									return false;
								}
								else
								{
									msgErr = 0;
								}
							}
							else
							{
								tmp.word = dwnMailID;
								buf_snd[0] = tmp.bytes.high;
								buf_snd[1] = tmp.bytes.low;
								SendCenterMsg(0x50, buf_snd, 2);
							}								
							
							m_objMail.m_aryMailID[m_objMail.m_bytMailIDCnt++] = dwnMailID;
							m_objLedStat.wait_dwnID--;
							
							//收到信息之后，再请求信息
							if ( m_objLedStat.wait_dwnID )
							{
								SendMailID();
							}
							else
							{
								m_objLedMenu.m_usMenuID = ans_LedMenu.m_usMenuID;
							}
#ifdef DELAYLEDTIME
							else
							{
								//节目单中信息已经存储完毕，在此存入节目单ID
								m_objLedMenu.m_usMenuID = ans_LedMenu.m_usMenuID;
								SaveLedConfig(); 
							}
#endif							
							break;
						}
					} 
				}
			}
			break;

		case 0x04://屏幕设置
			{					
				len_snd = set_led(buf+frm_cnt, len);
				if (len_snd) 
				{									
					//上电等待时间需保存到NANDFLASH中
					m_bytPowerOnWaitTm = buf[frm_cnt+28];
					m_objLedMenu.m_LedParam.m_usParamID = m_usToSavLedSetID;

					SaveLedConfig();
					
					buf_snd[0] = 1;
					SendCenterMsg(0x64,buf_snd,1);
				}
				else
				{
					buf_snd[0] = 2;
					SendCenterMsg(0x64,buf_snd,1);
					return false;
				}
			}
			break;
			
#if BOHAILED_PROTOCOLVER == 0x21 || BOHAILED_PROTOCOLVER == 0x22
		case 0x06: // 转场信息
			{
				ushort TURNID = htons(ans_LedMenu.m_usTURNID);
				BYTE byType = 0;
				bool Ret = false; 

				Ret=SndCmdWaitRes( "TURN", buf+1, len, 3, 8000 );

				if(Ret)
				{
					m_usTURNID = ans_LedMenu.m_usTURNID;
					SaveLedConfig();
					byType = 0x01;

					memcpy(buf_snd,(char*)&byType,1);
					memcpy(buf_snd+1,(char*)&TURNID,2);
					SendCenterMsg(0x86,buf_snd,3);
				}
				else
				{
					byType = 0x02;
					memcpy(buf_snd,(char*)&byType,1);
					memcpy(buf_snd+1,(char*)&TURNID,2);
					SendCenterMsg(0x86,buf_snd,3);
				}
			}
			break;
				
		case 0x08: // 通知信息
			{
				ushort NTICID = htons(ans_LedMenu.m_usNTICID);
				BYTE byType = 0;
				bool Ret = false; 

				Ret=SndCmdWaitRes( "NTIC", buf+1, len, 3, 8000 );

				if(Ret)
				{
					m_usNTICID =ans_LedMenu.m_usNTICID;
					SaveLedConfig();
					byType = 0x01;

					memcpy(buf_snd,(char*)&byType,1);
					memcpy(buf_snd+1,(char*)&NTICID,2);
					SendCenterMsg(0x68,buf_snd,3);
				}
				else
				{
					byType = 0x02;
					memcpy(buf_snd,(char*)&byType,1);
					memcpy(buf_snd+1,(char*)&NTICID,2);
					SendCenterMsg(0x68,buf_snd,3);
				}
			}
			break;
#endif

		case 0x12://查询屏幕时间
			{
				if (m_bAccValid)
				{
					len_snd = query_time(buf_snd);
					
					if (len_snd)
					{
						memcpy(&m_objCurtime,buf_snd,len_snd);
						buf_snd[0] = 1;
						memcpy(buf_snd+1,&m_objCurtime,len_snd);
						len_snd++;
						
						SendCenterMsg(0x52,buf_snd,len_snd);
					}
					else
					{
						buf_snd[0] = 2;
						SendCenterMsg(0x52,buf_snd,1);
						return false;
					}
				}
				else 
				{
					buf_snd[0] = 3;
					SendCenterMsg(0x52, buf_snd, 1);
					return false;
				}
			}
			break;

		case 0x13:	//查询版本
			{
				if(m_bAccValid)
				{
					len_snd=query_ver(buf_snd);

					if (len_snd)
					{
						char buf2[50];
						int len2;
						buf2[0] = 1;
						buf2[1] = (BYTE) len_snd;		//版本信息长度
						memcpy(buf2+2,buf_snd,len_snd);	//版本信息内容
						len2 = len_snd+2;

						SendCenterMsg(0x53,buf2,len2);
					}
					else
					{
						buf_snd[0] = 2;
						SendCenterMsg(0x53,buf_snd,1);
						return false;
					}
				} 
				else
				{
					buf_snd[0] = 3;
					SendCenterMsg(0x53, buf_snd, 1);
					return false;
				}
			}
			break;

		case 0x14://删除LED屏内所有信息ID
			{
				if(m_bAccValid)
				{
					if ( delete_all() )
					{
						m_objLedMenu.Init();
						m_objMail.Init();
						buf_snd[0] = 1;
						SendCenterMsg(0x54,buf_snd,1);
					}
					else
					{
						buf_snd[0] = 2;
						SendCenterMsg(0x54,buf_snd,1);
						return false;
					}
				} 
				else {
					buf_snd[0] = 3;
					SendCenterMsg(0x54, buf_snd, 1);
					return false;
				}					
			}
			break;

		case 0x15://查询状态
			{
				if(m_bAccValid)
				{
					len_snd = query_state(buf_snd);
					
					if (len_snd)
					{
						char buf2[100];
						int len2 = 0;
						buf2[len2++] = 1;//应答成功
						memcpy(buf2+len2,buf_snd,len_snd);
						len2+= len_snd;
						SendCenterMsg(0x57,buf2,len2);
					}
					else
					{
						buf_snd[0] = 2;
						SendCenterMsg(0x57,buf_snd,1);
						return false;
					}
				} 
				else
				{
					buf_snd[0] = 3;
					SendCenterMsg(0x57, buf_snd, 1);
					return false;
				}
				
			}
			break;

		case 0x16:	//取消点播/即时消息
			{
				if (buf[frm_cnt]==1)//取消即时
				{
					if(exit_show())
					{
						buf_snd[0] = 1;
						SendCenterMsg(0x56,buf_snd,1);
					}
					else
					{
						buf_snd[0] = 2;
						SendCenterMsg(0x56,buf_snd,1);
						return false;
					}
				}
				else if(buf[frm_cnt]==2)
				{
					if(exit_disp()) // 退出点播
					{
						buf_snd[0] = 4;
						SendCenterMsg(0x56,buf_snd,4);
					}
					else
					{
						buf_snd[0] = 5;
						SendCenterMsg(0x56,buf_snd,5);
						return false;
					}
				}			
			}
			break;

		case 0x17:	//中心取消报警
			{
				if(m_bAccValid)
				{
					if( exit_alarm() )
					{
						buf_snd[0] = 1;
						SendCenterMsg(0x55,buf_snd,1);
					}
					else
					{
						buf_snd[0] = 2;
						SendCenterMsg(0x55,buf_snd,1);
						return false;
					}
				} 
				else
				{
					buf_snd[0] = 3;
					SendCenterMsg(0x55, buf_snd, 1);
					return false;
				}			
			}
			break;

		case 0x18:	//校对时间指令
			{
				if (m_bAccValid) {
					if ( set_time(buf+frm_cnt, len) )
					{
						buf_snd[0] = 1;
						SendCenterMsg(0x58,buf_snd,1);
					}
					else
					{
						buf_snd[0] = 2;
						SendCenterMsg(0x58,buf_snd,1);
						return false;
					}
				} 
				else
				{
					buf_snd[0] = 3;
					SendCenterMsg(0x58, buf_snd, 1);
					return false;
				}
				
			}
			break;

		case 0x19://黑亮屏控制
			{
				if(2 == buf[frm_cnt])
				{
					if(m_bAccValid)
					{
						len_snd = light();
						if (len_snd) 
						{
							buf_snd[0] = 1;
							SendCenterMsg(0x46,buf_snd,1);
						}
						else
						{
							buf_snd[0] = 2;
							SendCenterMsg(0x46,buf_snd,1);
							return false;
						}
					} 
					else
					{
						buf_snd[0] = 3;
						SendCenterMsg(0x46, buf_snd, 1);
						return false;
					}					
				}
				else if(1 == buf[frm_cnt]) 
				{
					if(m_bAccValid)
					{
						len_snd = black();
						if (len_snd) 
						{
							buf_snd[0] = 1;
							SendCenterMsg(0x45,buf_snd,1);
						}
						else
						{
							buf_snd[0] = 2;
							SendCenterMsg(0x45,buf_snd,1);
							return false;
						}
					} 
					else
					{
						buf_snd[0] = 3;
						SendCenterMsg(0x45, buf_snd, 1);
						return false;
					}					
				}				
			}
			break;

		case 0x20://远程程序下载升级请求
			{				
				len_snd=query_ver(buf_snd);
				if (len_snd)
				{
					int iSame = 1;
					for (int i=0;i<VERCNT_BOHAI;i++)
					{
						if (buf[frm_cnt+i] != buf_snd[i])
						{
							iSame = 0;
							break;
						}
					}
					if (iSame)
					{
						buf_snd[0] = 1;//1，已是最新版本，下载结束
						SendCenterMsg(0x60,buf_snd,1);
						break;
					}
				}
				else
				{
					buf_snd[0] = 3;		//无法与LED建立通信 
					SendCenterMsg(0x60,buf_snd,1);
					return false;
				}
				
				//把升级信息放入内存
				m_objUpSta.updating = true;
				
				// 清除请求新升级包状态信息 xun add
				m_dwUpgrdSndTm = GetTickCount(); // xun,更新最后收到升级包的时间
				m_wUpgrdSndTimes = 0;
				memset( m_szUpgrdReqBuf, 0, sizeof(m_szUpgrdReqBuf) );
				m_iUpgrdReqBufLen = 0;
				
				m_objUpSta.ver_Cnt = buf[frm_cnt++];
				memcpy(&m_objUpSta.ver, buf+frm_cnt, m_objUpSta.ver_Cnt);
				frm_cnt+=m_objUpSta.ver_Cnt;
				m_objUpSta.pkt_Cnt = buf[frm_cnt++];
				
				//进入远程下载工作状态
				buf_snd[0] = 0;//0，请求下载成功
				SendCenterMsg(0x60,buf_snd,1);
				
				//请求第一个包
				memcpy(buf_snd, buf+1, VERCNT_BOHAI+1);
				buf_snd[VERCNT_BOHAI+1] = m_objUpSta.packet;
				SendCenterMsg(0x61, buf_snd, VERCNT_BOHAI+2);
				m_bytCurSta = stDOWN;//进入升级状态

				black();
			}
			break;

		case 0x21://远程下载
			{
				//透传，判断返回，4k为单位进行重传
				if ( len!=PKT_BOHAI+1)
				{
					m_objUpSta.updating = false;
					light();
					buf_snd[0] = 2;
					SendCenterMsg(0x62, buf_snd, 1);
					m_bytCurSta = stINIT;
					break;
				}

				if (buf[frm_cnt]==m_objUpSta.packet) 
				{
					len_snd = updd(buf+frm_cnt, len);
					
					if(len_snd == m_objUpSta.packet)
					{
						//len_snd 代表包序号，赋值并递增，请求
						m_objUpSta.packet++;
						if (m_objUpSta.packet>=UPDTCNT_BOHAI) 
						{
							//完成下载，不请求
							m_objUpSta.packet = 0;
							m_objUpSta.updating = false;
							break;
						}
					}
					else
					{
						m_objUpSta.packet -= m_objUpSta.packet%RESEND_BOHAI;
					}

					frm_cnt = 0;
					buf_snd[frm_cnt++] = VERCNT_BOHAI;
					memcpy(buf_snd+frm_cnt, &m_objUpSta.ver, VERCNT_BOHAI);
					frm_cnt+=VERCNT_BOHAI;
					buf_snd[frm_cnt++] = m_objUpSta.packet;

					SendCenterMsg(0x61, buf_snd, frm_cnt);
					
					m_wUpgrdSndTimes = 1;				// 重置请求新升级包次数, xun add
					m_dwUpgrdSndTm = GetTickCount();	// xun,更新最后收到升级包的时间
					
					if( frm_cnt <= sizeof(m_szUpgrdReqBuf) )
					{
						memcpy( m_szUpgrdReqBuf, buf_snd, frm_cnt );
						m_iUpgrdReqBufLen = frm_cnt;
					}
					else
					{
						memset( m_szUpgrdReqBuf, 0, sizeof(m_szUpgrdReqBuf) );
						m_iUpgrdReqBufLen = 0;
					}
				}					
			}
			break;

		case 0x22://查询LED信息
			{
				//先取LED节目单及(LED不提供返回屏参数)
				if (m_bAccValid)
				{
					len_snd = list_all( buf_snd );
				
					if (len_snd) 
					{
						WORD_UNION tmp;
						long frm_cnt=0;
						m_objMail.m_bytMailIDCnt = buf_snd[frm_cnt++];

						for(int k=0; k<m_objMail.m_bytMailIDCnt; k++)
						{
							tmp.bytes.high = buf_snd[frm_cnt++];
							tmp.bytes.low = buf_snd[frm_cnt++];
							m_objMail.m_aryMailID[k] = tmp.word;
						}
						
						buf[0] = 1;
						memcpy(buf+1,buf_snd,len_snd);
						SendCenterMsg(0x47, buf, len_snd+1);
					}
					else
					{
						buf[0] = 2;
						SendCenterMsg(0x47, buf, 1);
						return false;
					}
				}
				else
				{
					buf[0] = 3;
					SendCenterMsg(0x47, buf, 1);
					return false;
				}
				
			}
			break;

		case 0x23://根据信息ID查询屏幕存储信息指令
			{
				if (m_bAccValid)
				{
					WORD_UNION tmp;
					tmp.bytes.high = buf[frm_cnt++];
					tmp.bytes.low = buf[frm_cnt++];

					len_snd = query_id(tmp.word, buf_snd);
				
					if (len_snd)
					{
						frm_cnt = 0;
						buf[frm_cnt++] = 1;
						memcpy(buf+frm_cnt, buf_snd, len_snd);
						frm_cnt+=len_snd;
						SendCenterMsg(0x48, buf, frm_cnt);
					}
					else
					{
						buf[0] = 2;
						SendCenterMsg(0x48, buf, 1);
						return false;
					}
				} 
				else 
				{
					buf[0] = 3;
					SendCenterMsg(0x48, buf, 1);
					return false;
				}
			}
			break;

		case 0x24:	//删除信息指令
			{
				if (delete_msg(buf+frm_cnt,len))
				{
					buf_snd[0] = 1;
					SendCenterMsg(0x49,buf_snd,1);

					//删除后再刷新LED节目单 (LED不提供返回屏参数)
					len_snd = list_all( buf_snd );
					if (len_snd)
					{
						WORD_UNION tmp;
						long frm_cnt=0;
						m_objMail.m_bytMailIDCnt = buf_snd[frm_cnt++];
						
						for(int k=0;k<m_objMail.m_bytMailIDCnt;k++)
						{
							tmp.bytes.high = buf_snd[frm_cnt++];
							tmp.bytes.low = buf_snd[frm_cnt++];
							m_objMail.m_aryMailID[k] = tmp.word;
						}
					}
				}
				else
				{
					buf_snd[0] = 2;
					SendCenterMsg(0x49,buf_snd,1);
					return false;
				}
				
			}
			break;

		case 0x25://配置车台LED功能
			{
				if (buf[frm_cnt]==1)	//启用根据车台运动状态进行黑亮屏控制
				{
					m_bytLedCfg |= 0x02;
					frm_cnt++;
				}
				else
				{
					m_bytLedCfg &= ~0x02;//默认不启用
					frm_cnt++;
				}

				WORD_UNION tmp;
				tmp.bytes.high = buf[frm_cnt++];
				tmp.bytes.low = buf[frm_cnt++];
				m_usBlackWaitTm = tmp.word;

				if (buf[frm_cnt]==1)	//启用屏幕通信错误后禁止下载广告
				{
					m_bytLedCfg &= ~0x04;//默认启用
					frm_cnt++;
				}
				else
				{
					m_bytLedCfg |= 0x04;//置位
					frm_cnt++;
				}

				if (buf[frm_cnt]==1)//启用MD5
				{
					m_bytLedCfg |= 0x08;//置位
					frm_cnt++;
				}
				else
				{
					m_bytLedCfg &= ~0x08;//默认不启用
					frm_cnt++;
				}
				
				if ( SaveLedConfig() ) 
				{
					buf_snd[0] = 1;
					SendCenterMsg(0x65, buf_snd, 1);
				}
				else
				{
					buf_snd[0] = 2;
					SendCenterMsg(0x65, buf_snd, 1);
				}
				
			}
			break;			

#if BOHAILED_PROTOCOLVER == 0x21 || BOHAILED_PROTOCOLVER == 0x22
		case 0x27:	//查询转场信息内容
			{
				char szRes[1500] = { 0 };
				int iResLen = 0;
				BYTE byType = 0;

				SndCmdWaitRes( "QTRN", NULL, 0, 3, 9500, szRes, int(sizeof(szRes)), &iResLen );
			
#if BOHAILED_PROTOCOLVER == 0x21
				if( iResLen >= 5 )
				{
					byType = 0x01;
					memcpy( buf_snd,(char*)&byType,1);
					memcpy( buf_snd+1, szRes + 5, iResLen - 5 );
					SendCenterMsg(0x67,buf_snd,iResLen-5+1);
				}
#endif
#if BOHAILED_PROTOCOLVER == 0x22
				if( iResLen >= 6 )
				{
					byType = 0x01;
					memcpy( buf_snd,(char*)&byType,1);
					memcpy( buf_snd+1, szRes + 6, iResLen - 6 );
					SendCenterMsg(0x67,buf_snd,iResLen-6+1);
				}
#endif
				else
				{
					byType = 0x02;
					memcpy( buf_snd,(char*)&byType,1);
					SendCenterMsg(0x67,buf_snd,1);					
				}
			}
			break;

		case 0x29: // 删除通知
			{
				BYTE byType = 0;
				bool Ret = false; 
				Ret=SndCmdWaitRes( "DTIC ", buf+1, len, 3, 3000 );
				
				if(Ret)
				{
					byType = 0x01;
					memcpy(buf_snd,(char*)&byType,1);
					SendCenterMsg(0x69,buf_snd,1);
				}
				else
				{
					byType = 0x02;
					memcpy(buf_snd,(char*)&byType,1);
					SendCenterMsg(0x69,buf_snd,1);
				}
			}
			break;

		case 0x30: // 查询通知
			{
				char szRes[1500] = { 0 };
				int iResLen = 0;
				BYTE byType = 0;
				SndCmdWaitRes( "QTIC", NULL, 0, 3, 9500, szRes, int(sizeof(szRes)), &iResLen );
			
#if BOHAILED_PROTOCOLVER == 0x21
				if( iResLen >= 5 )
				{
					byType = 0x01;
					memcpy( buf_snd,(char*)&byType,1);
					memcpy( buf_snd+1, szRes + 5, iResLen - 5 );
					SendCenterMsg(0x70,buf_snd,iResLen-5+1);
				}
#endif
#if BOHAILED_PROTOCOLVER == 0x22
				if( iResLen >= 6 )
				{
					byType = 0x01;
					memcpy( buf_snd,(char*)&byType,1);
					memcpy( buf_snd+1, szRes + 6, iResLen - 6 );
					SendCenterMsg(0x70,buf_snd,iResLen-6+1);
				}
#endif
				else
				{
					byType = 0x02;
					memcpy( buf_snd,(char*)&byType,1);
					SendCenterMsg(0x70,buf_snd,1);
				}
			}
			break;
#endif

		case 0x80://强制广告下载
			{
				SendCenterMsg(0x42,buf+1,2);
			}
			break;

		case 0x81://强制屏幕参数下载
			{
				SendCenterMsg(0x44,NULL,0);
			}
			break;

		default:
			break;
			}
	}

	return true;
}

//-----------------------------------------------------------------------------	
// 处理从串口收到的字符串,解析出命令数据并存入队列中
void CLedBoHai::DealComData_V1_2( char *v_szRecv, int v_iRecvLen )
{
	if( !v_szRecv || v_iRecvLen <= 0 ) 
		return;
	
	static int i7ECount = 0;
	
	for( int i = 0; i < v_iRecvLen; i ++ )
	{
		if( 0x7e == v_szRecv[ i ] )
		{
			++ i7ECount;
			
			if( 0 == i7ECount % 2 ) // 若得到一帧
			{
				PRTMSG(MSG_DBG, "Led recv one frame:");
				PrintData(MSG_DBG, m_szRecvFrameBuf, m_iRecvFrameLen);

				_DeTranData( m_szRecvFrameBuf, m_iRecvFrameLen );
				
				if( 0x43 != m_szRecvFrameBuf[0] )
				{
					PRTMSG(MSG_DBG, "Led pack type uncorrect!\n");
					memset((void*)m_szRecvFrameBuf, 0, sizeof(m_szRecvFrameBuf) );
					m_iRecvFrameLen = 0;
					continue;
				}
				
				int iRecvLen = (byte)m_szRecvFrameBuf[1]*256 + (byte)m_szRecvFrameBuf[2];
				
				// 计算校验和
				ushort crc1 = CRC_16((byte*)m_szRecvFrameBuf, (unsigned long)iRecvLen);
				ushort crc2 = (byte)m_szRecvFrameBuf[m_iRecvFrameLen-2]*256
					+ (unsigned char)m_szRecvFrameBuf[m_iRecvFrameLen-1];
				
				if(crc1 != crc2)
				{
					PRTMSG(MSG_ERR, "Led frame crc error\n");
					memset((void*)m_szRecvFrameBuf, 0, sizeof(m_szRecvFrameBuf) );
					m_iRecvFrameLen = 0;
					continue;
				}
				
				// 校验正确，去除包类型、包长度、屏ID、校验码，然后送入队列
				DWORD dwPacketNum = 0;
				if( m_iRecvFrameLen > 7 )
				{
					m_objReadComMng.PushData(LV1, (DWORD)(m_iRecvFrameLen-7), m_szRecvFrameBuf+5, dwPacketNum);
				}
				
				memset((void*)m_szRecvFrameBuf, 0, sizeof(m_szRecvFrameBuf) );
				m_iRecvFrameLen = 0;
			}
		}
		else
		{
			m_szRecvFrameBuf[m_iRecvFrameLen++] = v_szRecv[i];
		}
	}
}


//-----------------------------------------------------------------------------	
// 处理从串口收到的字符串,解析出命令数据并存入队列中
void CLedBoHai::DealComData_V2_1( char *v_szRecv, int v_iRecvLen )
{
	if( !v_szRecv || v_iRecvLen <= 0 ) 
		return;
	
	static int i7ECount = 0;
	
	for( int i = 0; i < v_iRecvLen; i ++ )
	{
		if( 0x7e == v_szRecv[ i ] )
		{
			++ i7ECount;
			
			if( 0 == i7ECount % 2 ) // 若得到一帧
			{
				PRTMSG(MSG_DBG, "Led recv one frame:");
				PrintData(MSG_DBG, m_szRecvFrameBuf, m_iRecvFrameLen);

				_DeTranData( m_szRecvFrameBuf, m_iRecvFrameLen );
				
				if( 0x43 != m_szRecvFrameBuf[0] )
				{
					PRTMSG(MSG_DBG, "Led pack type uncorrect!\n");
					memset((void*)m_szRecvFrameBuf, 0, sizeof(m_szRecvFrameBuf) );
					m_iRecvFrameLen = 0;
					continue;
				}
				
				int iRecvLen = (byte)m_szRecvFrameBuf[1]*256 + (byte)m_szRecvFrameBuf[2];
				
				// 计算校验和
				ushort crc1 = CRC_16((byte*)m_szRecvFrameBuf, (unsigned long)iRecvLen);
				ushort crc2 = (byte)m_szRecvFrameBuf[m_iRecvFrameLen-2]*256
					+ (unsigned char)m_szRecvFrameBuf[m_iRecvFrameLen-1];
				
				if(crc1 != crc2)
				{
					PRTMSG(MSG_ERR, "Led frame crc error\n");
					memset((void*)m_szRecvFrameBuf, 0, sizeof(m_szRecvFrameBuf) );
					m_iRecvFrameLen = 0;
					continue;
				}
				
				// 校验正确，去除包类型、包长度、流水号、[回信包索引(实际没有)]、校验码，然后送入队列
				DWORD dwPacketNum = 0;
				if( m_iRecvFrameLen > 7 )
				{
					m_objReadComMng.PushData(LV1, (DWORD)(m_iRecvFrameLen-7), m_szRecvFrameBuf+5, dwPacketNum);
				}
				
				memset((void*)m_szRecvFrameBuf, 0, sizeof(m_szRecvFrameBuf) );
				m_iRecvFrameLen = 0;
			}
		}
		else
		{
			m_szRecvFrameBuf[m_iRecvFrameLen++] = v_szRecv[i];
		}
	}
}

//-----------------------------------------------------------------------------	
// 处理从串口收到的字符串,解析出命令数据并存入队列中
void CLedBoHai::DealComData_V2_2( char *v_szRecv, int v_iRecvLen )
{
	if( !v_szRecv || v_iRecvLen <= 0 ) 
		return;
	
	static int i7ECount = 0;
	
	for( int i = 0; i < v_iRecvLen; i ++ )
	{
		if( 0x7e == v_szRecv[ i ] )
		{
			++ i7ECount;
			
			if( 0 == i7ECount % 2 ) // 若得到一帧
			{
 				PRTMSG(MSG_DBG, "Led recv one frame:");
				PrintData(MSG_DBG, m_szRecvFrameBuf, m_iRecvFrameLen);

				_DeTranData( m_szRecvFrameBuf, m_iRecvFrameLen );

				if( 0x43 != m_szRecvFrameBuf[0] )
				{
					PRTMSG(MSG_DBG, "Led pack type uncorrect!\n");
					memset((void*)m_szRecvFrameBuf, 0, sizeof(m_szRecvFrameBuf) );
					m_iRecvFrameLen = 0;
					continue;
				}
				
				int iRecvLen = (byte)m_szRecvFrameBuf[1]*256 + (byte)m_szRecvFrameBuf[2];
				
				// 计算校验和
				ushort crc1 = CRC_16((byte*)m_szRecvFrameBuf, (unsigned long)iRecvLen);
				ushort crc2 = (byte)m_szRecvFrameBuf[m_iRecvFrameLen-2]*256
					+ (unsigned char)m_szRecvFrameBuf[m_iRecvFrameLen-1];
				
				if(crc1 != crc2)
				{
					PRTMSG(MSG_ERR, "Led frame crc error\n");
					memset((void*)m_szRecvFrameBuf, 0, sizeof(m_szRecvFrameBuf) );
					m_iRecvFrameLen = 0;
					continue;
				}
				
				// 校验正确，去除包类型、包长度、流水号、[回信包索引(实际没有)]、校验码，然后送入队列
				DWORD dwPacketNum = 0;
				if( m_iRecvFrameLen > 7 )
				{
					m_objReadComMng.PushData(LV1, (DWORD)(m_iRecvFrameLen-7), m_szRecvFrameBuf+5, dwPacketNum);
				}

				memset((void*)m_szRecvFrameBuf, 0, sizeof(m_szRecvFrameBuf) );
				m_iRecvFrameLen = 0;
			}
		}
		else
		{
			m_szRecvFrameBuf[m_iRecvFrameLen++] = v_szRecv[i];
		}
	}
}

//-----------------------------------------------------------------------------
// 发送LED数据帧
// cmd: 4个字节指令 如"INST"
// param: 参数内容
// len: 参数长度
int CLedBoHai::SendComData_V1_2(char* cmd, char* param, int len)
{
	char szOrig[ALABUF] = { 0 };
	char szFrm[ALABUF] = { 0 };
	int i = 0, j = 0, k = 0;
	
	szOrig[i++] = 0x43;				// 包类型
	
	szOrig[i++] = (len+9)/256;		// 包长度
	szOrig[i++] = (len+9)%256;

	szOrig[i++] = 0x00;				// 屏ID
	szOrig[i++] = 0x00;
	
	memcpy(szOrig+i, cmd, 4); 		// 指令代号
	i+=4;
	
	memcpy(szOrig+i, param, len);	// 参数(n)
	i+=len;
	
	ushort crc = CRC_16( (uchar*)szOrig, i );
	
	szOrig[i++] = byte(crc>>8);//校验(2)
	szOrig[i++] = byte(crc);		
	
	// 数据封帧
	j = 0;

#if USE_COMBUS == 1
	szFrm[j++] = 0xf7;		//0xf7表示由扩展盒的模拟串口2发送出去
#endif

	szFrm[j++] = 0x7e;
	for(k=0; k<i; k++) 
	{
		if(0x7e == szOrig[k])
		{
			szFrm[j++] = 0x7d; 
			szFrm[j++] = 0x02; 
		}
		else if(0x7d == szOrig[k])
		{ 
			szFrm[j++] = 0x7d; 
			szFrm[j++] = 0x01; 
		}
		else 
		{ 
			szFrm[j++] = szOrig[k];
		}
	}
	
	szFrm[j++] = 0x7e;
	
	// 将整帧数据发往LED
	ulong writed = 0;
	int ret = 0;
	
//	PRTMSG(MSG_DBG, "Led send one frame: %s\n", cmd);

#if USE_COMBUS == 0	
	ret = write(m_hComPort, szFrm, j);
#endif

#if USE_COMBUS == 1
	DWORD dwPacketNum;
	ret = j;
	g_objComBus.m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)j, szFrm, dwPacketNum);
#endif

	PRTMSG( MSG_DBG, "Send LED Data(%s):", cmd );
	PrintData( MSG_DBG, szFrm, j );

// 	PRTMSG(MSG_DBG, "Led send one frame:");
// 	PrintString(szFrm, j);
	
#if SAVE_LEDDATA == 1	//车台给LED的数据
	m_logtype = 1;
	char log_buf[ALABUF*3] = {0};
	char temp[3] = {0};
	for (int l=0; l<j; l++)
	{
		sprintf(temp, "%02x ", (BYTE)szFrm[l]);
		strcat(log_buf, temp);
	}
	
	save_log((char *)log_buf, j*3);	
#endif
	
	return ret;
}

//-----------------------------------------------------------------------------
// 发送LED数据帧
// cmd: 4个字节指令 如"INST"
// param: 参数内容
// len: 参数长度
int CLedBoHai::SendComData_V2_1(char* cmd, char* param, int len)
{
	char szOrig[ALABUF] = { 0 };
	char szFrm[ALABUF] = { 0 };
	int i = 0, j = 0, k = 0;
	static int iCmdNo = 0;			// 流水号
	
	szOrig[i++] = 0x43;				// 包类型
	
	szOrig[i++] = (len+10)/256;		// 包长度
	szOrig[i++] = (len+10)%256;
	
	szOrig[i++] = iCmdNo/256;		// 流水号
	szOrig[i++] = iCmdNo%256;
	iCmdNo++;
	
	memcpy(szOrig+i, cmd, 4); 		// 指令代号
	i+=4;
	
	memcpy(szOrig+i, param, len);	// 参数(n)
	i+=len;
	
	ushort crc = CRC_16( (uchar*)szOrig, i );
	
	szOrig[i++] = byte(crc>>8);//校验(2)
	szOrig[i++] = byte(crc);		
	
	// 数据封帧
	szFrm[j++] = 0x7e;
	for(k=0; k<i; k++) 
	{
		if(0x7e == szOrig[k])
		{
			szFrm[j++] = 0x7d; 
			szFrm[j++] = 0x02; 
		}
		else if(0x7d == szOrig[k])
		{ 
			szFrm[j++] = 0x7d; 
			szFrm[j++] = 0x01; 
		}
		else 
		{ 
			szFrm[j++] = szOrig[k];
		}
	}
	
	szFrm[j++] = 0x7e;
	
	// 将整帧数据发往LED
	ulong writed = 0;
	int ret = 0;
	
//	PRTMSG(MSG_DBG, "Led send one frame: %s\n", cmd);
	
#if USE_COMBUS == 0	
	ret = write(m_hComPort, szFrm, j);
#endif
	
#if USE_COMBUS == 1
	DWORD dwPacketNum;
	ret = j;
	g_objComBus.m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)j, szFrm, dwPacketNum);
#endif
	
	PRTMSG( MSG_DBG, "Send LED Data:cmd = %s, len = %d:  ", cmd, ret );
	PrintData( MSG_DBG, szFrm, j );
	
	// 	PRTMSG(MSG_DBG, "Led send one frame:");
	//	PrintString(szFrm, j);
	
#if SAVE_LEDDATA == 1	//车台给LED的数据
	m_logtype = 1;
	char log_buf[ALABUF*3] = {0};
	char temp[3] = {0};
	for (int l=0; l<j; l++)
	{
		sprintf(temp, "%02x ", (BYTE)szFrm[l]);
		strcat(log_buf, temp);
	}
	
	save_log((char *)log_buf, j*3);	
#endif
	
	return ret;
}

//-----------------------------------------------------------------------------
// 发送LED数据帧
// cmd: 4个字节指令 如"INST"
// param: 参数内容
// len: 参数长度
int CLedBoHai::SendComData_V2_2(char* cmd, char* param, int len)
{
	char szOrig[ALABUF] = { 0 };
	char szFrm[ALABUF] = { 0 };
	int i = 0, j = 0, k = 0;
	static int iCmdNo = 0;			// 流水号
		
	szOrig[i++] = 0x43;				// 包类型

	szOrig[i++] = (len+10)/256;		// 包长度
	szOrig[i++] = (len+10)%256;

	szOrig[i++] = iCmdNo/256;		// 流水号
	szOrig[i++] = iCmdNo%256;
	iCmdNo++;

	memcpy(szOrig+i, cmd, 4); 		// 指令代号
	i+=4;

	szOrig[i++] = 0x02;				// 通道号

	memcpy(szOrig+i, param, len);	// 参数(n)
	i+=len;

	ushort crc = CRC_16( (uchar*)szOrig, i );
		
	szOrig[i++] = byte(crc>>8);//校验(2)
	szOrig[i++] = byte(crc);		
		
	// 数据封帧
	szFrm[j++] = 0x7e;
	for(k=0; k<i; k++) 
	{
		if(0x7e == szOrig[k])
		{
			szFrm[j++] = 0x7d; 
			szFrm[j++] = 0x02; 
		}
		else if(0x7d == szOrig[k])
		{ 
			szFrm[j++] = 0x7d; 
			szFrm[j++] = 0x01; 
		}
		else 
		{ 
			szFrm[j++] = szOrig[k];
		}
	}

	szFrm[j++] = 0x7e;

	// 将整帧数据发往LED
	ulong writed = 0;
	int ret = 0;

//	PRTMSG(MSG_DBG, "Led send one frame: %s\n", cmd);
	
#if USE_COMBUS == 0	
	ret = write(m_hComPort, szFrm, j);
#endif
	
#if USE_COMBUS == 1
	DWORD dwPacketNum;
	ret = j;
	g_objComBus.m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)j, szFrm, dwPacketNum);
#endif
	
	PRTMSG( MSG_DBG, "Send LED Data(%s):", cmd );
	PrintData( MSG_DBG, szFrm, j );

// 	PRTMSG(MSG_DBG, "Led send one frame:");
//	PrintString(szFrm, j);

#if SAVE_LEDDATA == 1	//车台给LED的数据
		m_logtype = 1;
		char log_buf[ALABUF*3] = {0};
		char temp[3] = {0};
		for (int l=0; l<j; l++)
		{
			sprintf(temp, "%02x ", (BYTE)szFrm[l]);
			strcat(log_buf, temp);
		}

		save_log((char *)log_buf, j*3);	
#endif

	return ret;
}

//----------------------------------------------------------------------------
//获得当前车台时间
//(输入：gps数据结构，gps数据有效：GPS_REAL 0x01只提取有效gps数据，而不提取黑匣子gps数据)
//输出：0：成功，其他：失败
int CLedBoHai::GetCurTime(tagCurTime& v_time)
{
	struct tm pCurrentTime;
	G_GetTime(&pCurrentTime);

	v_time.nYear	= (byte)(pCurrentTime.tm_year-100);
	v_time.nMonth	= (byte)(pCurrentTime.tm_mon+1);
	v_time.nDay		= (byte)pCurrentTime.tm_mday;
	v_time.nWeekDay	= (byte)pCurrentTime.tm_wday;
	v_time.nHour	= (byte)pCurrentTime.tm_hour;
	v_time.nMinute	= (byte)pCurrentTime.tm_min;
	v_time.nSecond	= (byte)pCurrentTime.tm_sec;

//	PRTMSG(MSG_DBG, "%d,%d,%d,%d,%d,%d\n",pCurrentTime->tm_year, pCurrentTime->tm_mon, pCurrentTime->tm_mday, pCurrentTime->tm_hour, pCurrentTime->tm_min,pCurrentTime->tm_sec);

// 	v_time.nYear	= 0x09;
// 	v_time.nMonth	= 0x0c;
// 	v_time.nDay		= 0x09;
// 	v_time.nWeekDay	= 0x04;
// 	v_time.nHour	= 0x13;
// 	v_time.nMinute	= 0x25;
// 	v_time.nSecond	= 0x00;

	v_time.nWeekDay++;

	return 1;
}

//----------------------------------------------------------------------------
//向中心发送数据
bool CLedBoHai::SendCenterMsg(BYTE cmd, char* param, int len)
{
	char szbuf[512];
	int  ilen;

	if( g_objSock.IsOnline() )
	{
		int iResult = g_objSms.MakeSmsFrame(param, len, 0x39, cmd, szbuf, sizeof(szbuf), ilen);
		if( !iResult )
		{
			if( !g_objSock.SOCKSNDSMSDATA( szbuf, ilen, LV9 ) ) return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//遍历信息ID,发送信息ID请求,SendMailID()
void CLedBoHai::SendMailID()
{
	//逐个查询信息ID是否已经在屏上存在，如果存在，则不操作，如果不存在，则发送信息ID请求至中心
	char buf_snd[5];
	WORD_UNION tmp;
	int iSame;

	for (int j=0;j<m_objLedMenu.m_bytMailIDCnt;j++)
	{
		iSame=0;
		for (int i=0;i<m_objMail.m_bytMailIDCnt;i++)
		{
			if (m_objMail.m_aryMailID[i]==m_objLedMenu.m_aryMailID[j])
			{ 
				iSame=1;
				break;
			}
		}
		if (iSame == 0)//led中无此信息ID，则请求下载
		{
			//节目单在LED屏中如果找不到相同的信息ID，请求中心下发
			tmp.word = m_objLedMenu.m_aryMailID[j];
			buf_snd[0] = tmp.bytes.high;
			buf_snd[1] = tmp.bytes.low;
			SendCenterMsg(0x42,buf_snd,2);
				
			PRTMSG(MSG_DBG, "SendMailID请求中心下载信息ID %d\n", m_objLedMenu.m_aryMailID[j]);

			return;
		}
	}	
}

//----------------------------------------------------------------------------
//保存LED配置到配置区中
bool CLedBoHai::SaveLedConfig()
{
	//保存参数设置到FLASH
	tag2QLedCfg obj2QLedCfg;

	GetSecCfg(&obj2QLedCfg, sizeof(obj2QLedCfg), offsetof(tagSecondCfg, m_uni2QLedCfg.m_obj2QLedCfg), sizeof(obj2QLedCfg) );

	obj2QLedCfg.m_usLedParamID = m_objLedMenu.m_LedParam.m_usParamID;
	obj2QLedCfg.m_bytLedPwrWaitTm = m_bytPowerOnWaitTm;
	obj2QLedCfg.m_bytLedConfig = m_bytLedCfg;
	obj2QLedCfg.m_usLedBlkWaitTm = m_usBlackWaitTm;
	obj2QLedCfg.m_usNTICID = m_usNTICID;
	obj2QLedCfg.m_usTURNID = m_usTURNID;

#ifdef DELAYLEDTIME
	obj2QLedCfg.m_usLedMenuID = m_objLedMenu.m_usMenuID;
#endif

	if ( !SetSecCfg(&obj2QLedCfg, offsetof(tagSecondCfg, m_uni2QLedCfg.m_obj2QLedCfg), sizeof(obj2QLedCfg)) ) 
	{
		return true;
	}
	else
	{
		return false;
	}		
}

int CLedBoHai::Deal0x39(BYTE v_bytDataType,char *v_szData, DWORD v_dwDataLen)
{
	char buf[ SOCK_MAXSIZE ];

	if( v_dwDataLen<0 ) 
		return ERR_PAR;

	if( v_dwDataLen + 1 > SOCK_MAXSIZE ) 
		return ERR_SPACELACK;
	
	// 发送LED内容到LED屏
	buf[ 0 ] = v_bytDataType;
	memcpy( buf + 1, v_szData, v_dwDataLen );

	return m_objCenterCmdQue.push((unsigned char *)buf,v_dwDataLen+1);
}

//测试LED
int CLedBoHai::LedTest(char* v_szData, DWORD v_dwDataLen)
{
	char buf[1024] = {0};
	int len = 0;
	Frm47 frm;
	if ( v_dwDataLen < 1 )	return ERR_PAR;
	switch(v_szData[0])
	{
	case 0x01:		//LED校时
		{
			frm.chktype = 0x01;

			tagCurTime curtm;
			GetCurTime(curtm);
			memcpy(buf, &curtm, sizeof(curtm));

			len = sizeof(curtm);

			if (set_time(buf, len))	
			{
				frm.restype = 0x01;
			}
			else
			{
				frm.restype = 0x02;
			}
		}
		break;
	case 0x02:		//发送即时信息
		{
			frm.chktype = 0x02;
			tagLedShowMsg CurShowMsg;
			CurShowMsg.m_bytLvl = 1;
			CurShowMsg.m_bytExitWay = 3;
			CurShowMsg.m_bytCircleCnt = 1;
			CurShowMsg.m_bytShowCt = 2;
			CurShowMsg.m_tagShowPar[0].m_bytType = 0;
			CurShowMsg.m_tagShowPar[1].m_bytType = 0;
			CurShowMsg.m_tagShowPar[0].m_bytColor = 0;
			CurShowMsg.m_tagShowPar[1].m_bytColor = 0;
			CurShowMsg.m_tagShowPar[0].m_bytFont = 0;
			CurShowMsg.m_tagShowPar[1].m_bytFont = 0;
			CurShowMsg.m_tagShowPar[0].m_bytWinX1 = CurShowMsg.m_tagShowPar[1].m_bytWinX1 = 0;
			CurShowMsg.m_tagShowPar[0].m_bytWinY1 = CurShowMsg.m_tagShowPar[1].m_bytWinY1 = 0;
			CurShowMsg.m_tagShowPar[0].m_bytWinX2 = CurShowMsg.m_tagShowPar[1].m_bytWinX2 = 15;
			CurShowMsg.m_tagShowPar[0].m_bytWinY2 = CurShowMsg.m_tagShowPar[1].m_bytWinY2 = 1;
			WORD_UNION tmp;
			const unsigned short LedShowTm = 5;//LED即时信息显示项显示时间,单位为秒
			tmp.bytes.high = (BYTE)LedShowTm;
			tmp.bytes.low = (BYTE)LedShowTm>>8;
			CurShowMsg.m_tagShowPar[0].m_usStayTm = CurShowMsg.m_tagShowPar[1].m_usStayTm = tmp.word;
			CurShowMsg.m_tagShowPar[0].m_bytShowWay = 3;
			CurShowMsg.m_tagShowPar[1].m_bytShowWay = 4;
			CurShowMsg.m_tagShowPar[0].m_bytCircleCnt = CurShowMsg.m_tagShowPar[1].m_bytCircleCnt = 5;
			char buf0[] = "测试即时信息成功";
			char buf1[] = "朋友，地球欢迎你";
			memcpy(CurShowMsg.m_tagShowPar[0].m_bytShowInfo, buf0, strlen(buf0));
			memcpy(CurShowMsg.m_tagShowPar[1].m_bytShowInfo, buf1, strlen(buf1));
			tmp.bytes.high = (BYTE)(strlen(buf0));
			tmp.bytes.low = (BYTE)((strlen(buf0))>>8);
			CurShowMsg.m_tagShowPar[0].m_usShowInfoCt = tmp.word;
			tmp.bytes.high = (BYTE)strlen(buf1);
			tmp.bytes.low = (BYTE)(strlen(buf1)>>8);
			CurShowMsg.m_tagShowPar[1].m_usShowInfoCt = tmp.word;
			len = sizeof(CurShowMsg)-40+strlen(buf0)+strlen(buf1);

			if (show((char*)&CurShowMsg, len))
			{	
				frm.restype = 0x01;	
			}
			else
			{
				frm.restype = 0x02;	
			}
		}
		break;

	case 0x03:		//LED亮屏
		{
			frm.chktype = 0x03;

			if( light() )
			{
				frm.restype = 0x01;	
			}
			else
			{
				frm.restype = 0x02;
			}
		}
		break;

	case 0x04:		//LED黑屏
		{
			frm.chktype = 0x04;
			if (black())
			{
				frm.restype = 0x01;
			}
			else
			{	
				frm.restype = 0x02;
			}
		}
		break;

	default:
		break;
	}

	char szbuf[100] = {0};
	szbuf[0] = 0x01;
	memcpy(szbuf+1, (char*)&frm, sizeof(frm));
	DataPush(szbuf, sizeof(frm)+1, DEV_QIAN, DEV_DIAODU, LV2);

	return 0;
}

//-----------------------------------------------------------------------------	
// 添加信息指令(INST)
// param: 参数指针
// 		信息属性+信息ID+保留一个字节+生命周期起始月+生命周期起始日+
//		生命周期消亡月+生命周期消亡日+信息停留时间+保留一个字节(显示类型)+
//		定时显示设置总数+显示项总数+[定时显示1参数+定时显示2参数+...+定时显示n参数]+
//		[显示项1参数+显示项2参数+...+显示项N参数]信息属性+信息ID+保留一个字节+
//		生命周期起始月+生命周期起始日+生命周期消亡月+生命周期消亡日+信息停留时间+
//		保留一个字节(显示类型)+定时显示设置总数+显示项总数+[定时显示1参数+定时显示2参数+
//		...+定时显示n参数]+[显示项1参数+显示项2参数+...+显示项N参数]
// len: 参数长度
// 返回: true  添加成功	
//		 false 添加失败
bool CLedBoHai::insert(char* param, int len)
{
	return SndCmdWaitRes( "INST", param, len, 3, 5000 );
}
	
//-----------------------------------------------------------------------------	
// 清除所有信息指令(DALL)
// 返回: true  清除成功	 false 清除失败	
bool CLedBoHai::delete_all()
{
	return SndCmdWaitRes( "DALL", NULL, 0, 3, 50000 );	
}
	
//-----------------------------------------------------------------------------	
// 删除信息指令(DELE)
// param: 参数指针
//		删除信息总数+信息ID1+信息 ID2+…+信息IDn
// len: 参数长度
// 返回: true  删除成功	
//		 false 删除失败	
bool CLedBoHai::delete_msg(char* param, int len)
{
	return SndCmdWaitRes( "DELE", param, len, 3, 4200 ); // 2.1/2.2协议中删除信息总数是1个字节吗？待
}
	
//-----------------------------------------------------------------------------	
// 点播信息(DISP)
// param: 参数指针
//		点播信息停留时间+点播信息总数+信息ID1+信息 ID2+…+信息IDn
// len: 参数长度
// 返回: true  点播成功	
//		 false 点播失败	
bool CLedBoHai::display(char* param, int len)
{
	return SndCmdWaitRes( "DISP", param, len, 3, 4000 );	
}
	
//-----------------------------------------------------------------------------	
// 退出点播状态(GOON)
// 返回: true  退出成功	
//		 false 退出失败	
bool CLedBoHai::exit_disp()
{
	return SndCmdWaitRes( "GOON", NULL, 0, 3, 4000 );		
}
	
//-----------------------------------------------------------------------------	
// 查询屏幕已存信息序列(LIST)
// outbuf: 返回的数据指针
//		信息总数量+[信息ID1+信息ID2+...+信息IDn]
// 返回: 数据长度
int CLedBoHai::list_all(char* outbuf)
{
	char szRes[1500] = { 0 };
	int iResLen = 0;
	SndCmdWaitRes( "LIST", NULL, 0, 3, 9500, szRes, int(sizeof(szRes)), &iResLen );

#if BOHAILED_PROTOCOLVER == 0x12 || BOHAILED_PROTOCOLVER == 0x21
	if( iResLen >= 5 )
	{
		
		memcpy( outbuf, szRes + 5, iResLen - 5 );
		return iResLen - 5;
	}
#endif
	
#if BOHAILED_PROTOCOLVER == 0x22
	if( iResLen >= 6 )
	{
		
		memcpy( outbuf, szRes + 6, iResLen - 6 );
		return iResLen - 6;
	}
#endif
	return 0;		
}	
	
//-----------------------------------------------------------------------------	
// 根据信息ID查询屏幕存储信息指令(QIDI)
// id: 信息ID
// outbuf: 返回的数据指针
// 		信息属性+信息ID+保留一个字节+生命周期起始月+生命周期起始日+生命周期消亡月+生命周期消亡日+
//		信息停留时间+保留一个字节(显示类型)+定时显示设置总数+显示项总数+[定时显示1参数+定时显示2参数+...+
//		定时显示n参数]+[显示项1参数+显示项2参数+...+显示项N参数]
// 返回: 数据长度
int CLedBoHai::query_id(ushort id, char* outbuf)
{
	char szSrc[20] = { 0 };
	szSrc[0] = 1;
	memcpy( szSrc + 1, &id, 2 );
	char szRes[1500] = { 0 };
	int iResLen = 0;
	SndCmdWaitRes( "QIDI", szSrc, 3, 2, 4500, szRes, int(sizeof(szRes)), &iResLen );

#if BOHAILED_PROTOCOLVER == 0x12 || BOHAILED_PROTOCOLVER == 0x21
	if( iResLen >= 5 )
	{
		
		memcpy( outbuf, szRes + 5, iResLen - 5 );
		return iResLen - 5;
	}
#endif
	
#if BOHAILED_PROTOCOLVER == 0x22
	if( iResLen >= 6 )
	{
		
		memcpy( outbuf, szRes + 6, iResLen - 6 );
		return iResLen - 6;
	}
#endif
	return 0;	
}
	
//-----------------------------------------------------------------------------	
// 即时信息指令(SHOW)
// param: 参数指针
//		播放级别+信息停留时间+循环次数+显示项总数+ [显示项1参数+显示项2参数+...+显示项N参数]
// len: 参数长度
// 返回: true  显示成功	
//		 false 显示失败	
bool CLedBoHai::show(char* param, int len)
{
	return SndCmdWaitRes( "SHOW", param, len, 3, 4000 );		
}
	
//-----------------------------------------------------------------------------	
// 取消即时播放状态指令(UNIM)
// 返回: true  取消成功
//		 false 取消失败
bool CLedBoHai::exit_show()
{
	return SndCmdWaitRes( "UNIM", NULL, 0, 3, 3000 );
}
	
//-----------------------------------------------------------------------------	
// 查询屏幕时间指令(QRTM)
// outbuf: 返回数据指针
//		年+月+日+星期+时+分+秒	
// 返回: 返回数据长度
int CLedBoHai::query_time(char* outbuf)
{
	char szRes[300] = { 0 };
	int iResLen = 0;
	SndCmdWaitRes( "QRTM", NULL, 0, 1, 3500, szRes, int(sizeof(szRes)), &iResLen );

#if BOHAILED_PROTOCOLVER == 0x12 || BOHAILED_PROTOCOLVER == 0x21
	if( iResLen >= 5 )
	{
		
		memcpy( outbuf, szRes + 5, iResLen - 5 );
		return iResLen - 5;
	}
#endif
	
#if BOHAILED_PROTOCOLVER == 0x22
	if( iResLen >= 6 )
	{
		
		memcpy( outbuf, szRes + 6, iResLen - 6 );
		return iResLen - 6;
	}
#endif
	return 0;
}

//-----------------------------------------------------------------------------	
// 校对时间指令(TIME)
// param: 参数指针
//		年+月+日+星期+时+分+秒
// len: 参数长度	
// 返回: true  校对成功
//		 false 校对失败
//2007.7.29 返回标志: 修改成功 buf[4] = 84,修改!
bool CLedBoHai::set_time(char* param, int len)
{
	return SndCmdWaitRes( "TIME", param, len, 3, 3500 );
}
	
//-----------------------------------------------------------------------------	
// 设置上电等待指令(WAIT)
// sec: 上电等待时间：单字节，单位为秒 0-60S
// 返回: true  设置成功
//		 false 设置失败
bool CLedBoHai::set_wait(BYTE sec)
{
	return SndCmdWaitRes( "WAIT", (char*)&sec, 1, 2, 3000 );
}
	
//-----------------------------------------------------------------------------	
// 报警指令(ALAR)
// param: 参数指针
//		警报内容字节数+警报内容
// len: 参数长度	
// 返回: true  校对成功
//		 false 校对失败
bool CLedBoHai::alarm(char* param, int len)
{
	return SndCmdWaitRes( "ALAR", param, len, 4, 4000 );
}

//-----------------------------------------------------------------------------	
// 取消报警指令(BLAC)
// 返回: true  取消报警成功	
//		 false 取消报警失败	
bool CLedBoHai::exit_alarm()
{
	return SndCmdWaitRes( "UALM", NULL, 0, 4, 4000 );	
}

//-----------------------------------------------------------------------------	
// 查询版本指令(QVER)
// outbuf: 返回数据指针
//		版本信息	
// 返回: 返回数据长度
int CLedBoHai::query_ver(char* outbuf)
{
	char szRes[300] = { 0 };
	int iResLen = 0;
	SndCmdWaitRes( "QVER", NULL, 0, 2, 3200, szRes, int(sizeof(szRes)), &iResLen );

#if BOHAILED_PROTOCOLVER == 0x12 || BOHAILED_PROTOCOLVER == 0x21
	if( iResLen >= 5 )
	{

		memcpy( outbuf, szRes + 5, iResLen - 5 );
		return iResLen - 5;
	}
#endif

#if BOHAILED_PROTOCOLVER == 0x22
	if( iResLen >= 6 )
	{
		
		memcpy( outbuf, szRes + 6, iResLen - 6 );
		return iResLen - 6;
	}
#endif
	return 0;
}
	
//-----------------------------------------------------------------------------	
// 工作状态检测指令(QLED)
// outbuf: 返回数据指针
//		黑/亮屏(byt)+播放状态(byt)+正在播放的信息ID(us)+正在播放的点播信息ID总数(byt)+
//		[正在播放的点播信息ID1，2，3，。。。，n](us)*n	
//07.7.27：  黑/亮屏+字库异常报告+设置异常报告+LED屏坏点数报告+LED时钟日期时间+LED内信息总数+
//播放状态+正在播放的信息ID+正在播放信息的显示项ID
// 返回: 返回数据长度 
//2007.8.3 根据博海LED协议更改，1．返回标志：单字节。0 ，LED工作无异常；3，LED存在影响运行的错误，包括设置异常或字库异常。
int CLedBoHai::query_state(char * outbuf)
{
	char szRes[1500] = { 0 };
	int iResLen = 0;
	SndCmdWaitRes( "QLED", NULL, 0, 3, 5200, szRes, int(sizeof(szRes)), &iResLen );

#if BOHAILED_PROTOCOLVER == 0x12 || BOHAILED_PROTOCOLVER == 0x21
	if( iResLen >= 5 )
	{
		
		memcpy( outbuf, szRes + 5, iResLen - 5 );
		return iResLen - 5;
	}
#endif
	
#if BOHAILED_PROTOCOLVER == 0x22
	if( iResLen >= 6 )
	{
		
		memcpy( outbuf, szRes + 6, iResLen - 6 );
		return iResLen - 6;
	}
#endif
	return 0;
}
	
//-----------------------------------------------------------------------------	
// 亮度控制指令(BRIG) (仅1.2协议有)
// param: 参数指针
//		亮度调节方式+固定亮度值+调节时间1+调节时间2+调节时间3+调节时间4+
//		时间亮度值1+时间亮度值2+时间亮度值3+时间亮度值4+
//		环境亮度值1+环境亮度值2+环境亮度值3+环境亮度值4+环境亮度值5+环境亮度值6+环境亮度值7+环境亮度值8
// len: 参数长度	
// 返回: true  设置成功
//		 false 校对失败
bool CLedBoHai::bright_ctrl(char* param, int len)
{
	return SndCmdWaitRes( "BRIG", param, len, 3, 3300 );
}		
	
//-----------------------------------------------------------------------------	
// 黑屏指令(BLAC)
// 返回: true  黑屏成功	
//		 false 黑屏失败	
bool CLedBoHai::black()
{
	return SndCmdWaitRes( "BLAC", NULL, 0, 3, 2900 );	
}
	
//-----------------------------------------------------------------------------	
// 亮屏指令(LIGH)
// 返回: true  亮屏成功	
//		 false 亮屏失败	
bool CLedBoHai::light()
{
	return SndCmdWaitRes( "LIGH", NULL, 0, 3, 3300 );
}

//-----------------------------------------------------------------------------	
// 复位指令(RSET)
// 返回: true  复位成功	
//		 false 复位失败	
bool CLedBoHai::reset()
{
	return SndCmdWaitRes( "RSET", NULL, 0, 3, 3500 );	// xun modify "REST" to "RSET"
}

//-----------------------------------------------------------------------------	
// 上电报告(PWON)
// 返回: true  LED重新上电成功	
//		 false LED重新上电失败
bool CLedBoHai::pwon()
{
	return SndCmdWaitRes( "PWON", NULL, 0, 3, 3500 );
}

//-----------------------------------------------------------------------------	
// 校对亮屏指令(OPTM)
// param: 参数指针
//		年+月+日+星期+时+分+秒
// len: 参数长度	
// 返回: true  校对成功
//		 false 校对失败
//2007.7.29 返回标志: 修改成功 buf[4] = 84,修改!
bool CLedBoHai::optm(char* param, int len)
{
	return SndCmdWaitRes( "OPTM", param, len, 3, 3500 );
}

//-----------------------------------------------------------------------------
//屏幕设置指令(SETS)
//输入:
	/*
		保留6byte+亮度控制设置+上电等待时间+转场是否启用+保留1byte +默认标语+报警标语+保留55byte
	*/
//输出:true：成功 false：失败
bool CLedBoHai::set_led(char* param, int len)
{
	bool Ret = false;

	Ret=SndCmdWaitRes( "SETS", param, len, 3, 8000 );

	return Ret;
}

//-----------------------------------------------------------------------------	
// 由中心控制的远程升级指令（UPDD）
// param: 包号+包内容
// len: 参数长度	
// 返回: 0-16 包序号
//		 0xff 出错
byte CLedBoHai::updd(char* param, int len)
{
	char szRes[200] = { 0 };
	int iResLen = 0;
	SndCmdWaitRes( "UPDD", param, len, 3, 7500, szRes, int(sizeof(szRes)), &iResLen );
	
#if BOHAILED_PROTOCOLVER == 0x12 || BOHAILED_PROTOCOLVER == 0x21
	if( iResLen >= 5 )
	{
		if( 0 == szRes[4] && iResLen >= 6 )
		{
			return byte(szRes[5]);
		}
		else
		{
			return 0xff;
		}
	}
#endif
#if BOHAILED_PROTOCOLVER == 0x22
	if( iResLen >= 6 )
	{
		if( 0 == szRes[5] && iResLen >= 7 )
		{
			return byte(szRes[6]);
		}
		else
		{
			return 0xff;
		}
	}
#endif
	else
	{
		return 0xff;
	}
}	

bool CLedBoHai::SndCmdWaitRes( char* v_szCmd, char* v_szCmdCont, int v_iCmdContLen, BYTE v_bytSndTimes, DWORD v_dwWaitPridPerTime, char* v_szRes /*=NULL*/, int v_iResSize /*=0*/, int* v_p_iResLen /*= NUL*/ )
{
	// 强制等待参数,以免无限等待
	if( v_dwWaitPridPerTime < 2500 )
		v_dwWaitPridPerTime = 2500;
	if( v_dwWaitPridPerTime > 30000 )
		v_dwWaitPridPerTime = 30000;
	if( v_bytSndTimes > 10 )
		v_bytSndTimes = 10;

	// 确保参数正确
	if( !v_szCmd )
		return false;
	if( !v_szCmdCont )
		v_iCmdContLen = 0; // 否则会非法访问内存
	
	char szRes[1500] = { 0 };

	ulong ulResLen = 0;
	bool bRet = false;
	BYTE bytOtherCmd = 0;
	DWORD dwDataLen = 0;
	byte  bytlvl;
	DWORD dwPushTm = 0;

	for( BYTE byt = 0; byt < v_bytSndTimes; byt ++ )
	{
		if( true == m_bEvtExit )
			return false;

#if BOHAILED_PROTOCOLVER == 0x12
		SendComData_V1_2( v_szCmd, v_szCmdCont, v_iCmdContLen );
		DWORD dwCur = GetTickCount();
		do 
		{
			if( true == m_bEvtExit )
				goto SNDANDWAIT_END;
			
			usleep( 300000 );
			
			memset( szRes, 0, sizeof(szRes) );
			ulResLen = 0;
			if( !m_objReadComMng.PopSymbData(G_JugSymbData, v_szCmd, 4, bytlvl, (DWORD)(sizeof(szRes)), ulResLen, szRes, dwPushTm) )
			{// xun modify from "PopData" to "PopSymbData"
				if( v_szRes && v_iResSize >= int(ulResLen) && v_p_iResLen )
				{
					memcpy( v_szRes, szRes, ulResLen );
					*v_p_iResLen = int(ulResLen);
				}
				bRet = !byte(szRes[4]);
				
				PRTMSG(MSG_DBG, "Led recv one frame: %s, bRet=%d, len=%d\n", v_szCmd, bRet,ulResLen);
				
				goto SNDANDWAIT_END;
			}
		} while( GetTickCount() - dwCur < v_dwWaitPridPerTime );
	
#endif

#if BOHAILED_PROTOCOLVER == 0x21
		SendComData_V2_1( v_szCmd, v_szCmdCont, v_iCmdContLen );
		DWORD dwCur = GetTickCount();
		do 
		{
			if( true == m_bEvtExit )
				goto SNDANDWAIT_END;
			
			usleep( 300000 );
			
			if( !m_objReadComMng.PopSymbData(G_JugSymbData, v_szCmd, 4, bytlvl, (DWORD)(sizeof(szRes)), ulResLen, szRes, dwPushTm) )
			{// xun modify from "PopData" to "PopSymbData"
				if( v_szRes && v_iResSize >= int(ulResLen) && v_p_iResLen )
				{
					memcpy( v_szRes, szRes, ulResLen );
					*v_p_iResLen = int(ulResLen);
				}
				bRet = !byte(szRes[4]);
				
				PRTMSG(MSG_DBG, "Led recv one frame: %s, chn=%d, bRet=%d, len=%d\n",v_szCmd, szRes[4], bRet,ulResLen);
				
				goto SNDANDWAIT_END;
			}
// 			else
// 			{
// 				PRTMSG(MSG_DBG, "PopSymData Fail\n");
// 			}
		} while( GetTickCount() - dwCur < v_dwWaitPridPerTime );
#endif

#if BOHAILED_PROTOCOLVER == 0x22
		SendComData_V2_2( v_szCmd, v_szCmdCont, v_iCmdContLen );
		DWORD dwCur = GetTickCount();
		do 
		{
			if( true == m_bEvtExit )
				goto SNDANDWAIT_END;
			
			usleep( 300000 );
			
			if( !m_objReadComMng.PopSymbData(G_JugSymbData, v_szCmd, 4, bytlvl, (DWORD)(sizeof(szRes)), ulResLen, szRes, dwPushTm) )
			{// xun modify from "PopData" to "PopSymbData"
				if( v_szRes && v_iResSize >= int(ulResLen) && v_p_iResLen )
				{
					memcpy( v_szRes, szRes, ulResLen );
					*v_p_iResLen = int(ulResLen);
				}
				bRet = !byte(szRes[5]);
				
				PRTMSG(MSG_DBG, "Led recv one frame: %s, chn=%d, bRet=%d, len=%d\n",v_szCmd, szRes[4], bRet,ulResLen);
				
				goto SNDANDWAIT_END;
			}
		} while( GetTickCount() - dwCur < v_dwWaitPridPerTime );
#endif
	}

SNDANDWAIT_END:
	return bRet;
}

//取消监控报警
void CLedBoHai::CancelLedAlarm()
{
	int iExit;

	iExit = exit_alarm();

	if (!iExit) 
	{
		exit_alarm();
	}
}

//LED显示报警
void CLedBoHai::EnableLedAlarm()
{
	char buf[2];

	light();

	const unsigned short ledAlertTm = LEDALARM_PRID;//LED显示抢劫报警时长,单位为秒
	
	buf[0] = (BYTE)ledAlertTm;
	buf[1] = (BYTE)ledAlertTm>>8;
	
	usleep(200000);
	alarm(buf,2);
}

void CLedBoHai::AdjustTime()
{
	char buf[ALABUF] = {0};
	int len;
	tagCurTime curtm;
	ulong adjustm ;

	for (int i=0; i<3; i++) 
	{
		GetCurTime(curtm); //取出当前系统时间 

		adjustm =GetTickCount(); 

		do {
			usleep(5000);
			GetCurTime(m_objCurtime); //取出系统时间

			if (GetTickCount() - adjustm > 10*1000) //取时超过10s不再校时 (保险处理)
			{ 
				PRTMSG(MSG_DBG, "adustm:%d\n",adjustm );
				return;
			}
		} while(m_objCurtime.nSecond == curtm.nSecond); //当时间跳变时跳出

		memset( buf,0,sizeof(buf) );
		memcpy(buf,&m_objCurtime,sizeof(m_objCurtime));
		len=sizeof(m_objCurtime);

		if (m_iAccSta == 1) //ACC正常点火
		{ 
			if (!set_time(buf,len))	//如果校时不正确
			{
				PRTMSG( MSG_DBG, "Led Time Fail\n" );
				sleep(1);
				m_bytCurSta = stEXCP; //转入异常工作状态
			} 
			else
			{
				break;
			}
		}
		else if (m_iAccSta == 2) //如果ACC正常，LED屏异常重启
		{
			if (!optm(buf,len)) //如果校时不正确
			{ 
				sleep(1);
				m_bytCurSta = stEXCP; //转入异常工作状态
			}
			else
			{
				m_iAccSta = 1;
				break;
			}
		}
	}
}


// creat a CRC 16 table
void CLedBoHai::build_table16( unsigned short aPoly )
{
	unsigned short i, j;
	unsigned short nData;
	unsigned short nAccum;
	
	for ( i = 0; i < 256; i++ )
	{
		nData = ( unsigned short )( i << 8 );
		nAccum = 0;
		for ( j = 0; j < 8; j++ )
		{
			if ( ( nData ^ nAccum ) & 0x8000 )
				nAccum = ( nAccum << 1 ) ^ aPoly;
			else
				nAccum <<= 1;
			nData <<= 1;
		}
		table_CRC[i] = ( unsigned long )nAccum;
	}
}

//-----------------------------------------------------------------------------	
//caculate the CRC_16
unsigned short CLedBoHai::CRC_16( unsigned char * aData, unsigned long aSize )
{
	unsigned long  i;
	unsigned short nAccum;
	nAccum = 0;
	
	for ( i = 0; i < aSize; i++ )
		nAccum = ( nAccum << 8 ) ^ ( unsigned short )table_CRC[( nAccum >> 8) ^ *aData++];
	return nAccum;
}

/// 7D+1->7D,7D+2->7E,同时去帧头尾7e
void CLedBoHai::_DeTranData(char *v_szBuf, int &v_iBufLen)
{	
	char buf[ 1024 ] = { 0 };
	int iLen = 0;
	for( int i = 0; i < v_iBufLen && iLen < sizeof(buf) - 2; )
	{
		if( 0x7d != v_szBuf[ i ] || i == v_iBufLen - 2 )
		{
			buf[ iLen ++ ] = v_szBuf[ i++ ];
		}
		else
		{
			if( 1 == v_szBuf[ i + 1 ] )
			{
				buf[ iLen ++ ] = 0x7d;
			}
			else if( 2 == v_szBuf[ i + 1 ] )
			{
				buf[ iLen ++ ] = 0x7e;
			}
			else
			{
				buf[ iLen ++ ] = v_szBuf[ i ];
				buf[ iLen ++ ] = v_szBuf[ i + 1 ];
			}
			i += 2;
		}
	}
	
	memcpy( v_szBuf, buf, iLen );
	v_iBufLen = iLen;
}

// void CLedBoHai::_DeTranData(char* v_szBuf, int &v_iBufLen, size_t v_sizBufSize )
// {
// 	char buf[1024] = { 0 };
// 	int iLen = 0;
// 	for( int i = 0; i < v_iBufLen)
// }

#endif
