#include "yx_common.h"

void DealIrdaKey(WORD sys_id_code, BYTE irkey_code, BOOL *v_bPeriodIgnore, SLEEP_EVENT *v_objSleepEvent, WORK_EVENT *v_objWorkEvent, CTRL_EVENT *v_objCtrlEvent)
{
	//printf("sys_id_code=%0x\n", sys_id_code);
	
	switch(sys_id_code)
	{
#if CHK_VER == 1
		case 0x03:
		case 0x87://小遥控
		{
			if( irkey_code==0x30 )//生产检测版本作为省电测试用
			{
				switch(g_enumSysSta)
				{
					case SLEEP:
					{
						v_objSleepEvent->dwStopEvt |= EVENT_TYPE_IRDA;
					}
						break;
					case IDLE:
					{
						v_objSleepEvent->dwStartEvt |= EVENT_TYPE_IRDA;
						g_iTempState=0;
					}
						break;
					case CTRL:
					{
						v_objCtrlEvent->dwStopEvt |= EVENT_TYPE_IRDA;
						v_objSleepEvent->dwStartEvt |= EVENT_TYPE_IRDA;
					}
						break;
					case WORK:
					{
						*v_bPeriodIgnore = TRUE;
						v_objWorkEvent->dwRecStopEvt |= EVENT_TYPE_IRDA;
						v_objWorkEvent->dwUplStopEvt |= EVENT_TYPE_IRDA;
						v_objSleepEvent->dwStartEvt |= EVENT_TYPE_IRDA;
					}
						break;
					default:
						break;
				}
			}
			else if( g_bIrTest==true )
			{
				int iTempState = g_iTempState;
				g_iTempState = 1;
				if( WaitForEvent(IDLESTATE, &g_enumSysSta, 10000)==0 )
				{
					switch(irkey_code)
					{
						case 0x90:
							TTSTest("0");
							//ShowDiaodu("检测到遥控按键 0");
							break;	
						case 0x00:
							TTSTest("1");
							//ShowDiaodu("检测到遥控按键 1");
							break;	
						case 0x80:
							TTSTest("2");
							//ShowDiaodu("检测到遥控按键 2");
							break;
						case 0x40:
							TTSTest("3");
							//ShowDiaodu("检测到遥控按键 3");
							break;
						case 0xc0:
							TTSTest("4");
							//ShowDiaodu("检测到遥控按键 4");
							break;
						case 0x20:
							TTSTest("5");
							//ShowDiaodu("检测到遥控按键 5");
							break;
						case 0xa0:
							TTSTest("6");
							//ShowDiaodu("检测到遥控按键 6");
							break;
						case 0x60:
							TTSTest("7");
							//ShowDiaodu("检测到遥控按键 7");
							break;
						case 0xe0:
							TTSTest("8");
							//ShowDiaodu("检测到遥控按键 8");
							break;
						case 0x10:
							TTSTest("9");
							//ShowDiaodu("检测到遥控按键 9");
							break;
						case 0x70:
							TTSTest("OK");
							//ShowDiaodu("检测到遥控按键 OK");
							break;
						case 0x08:
							TTSTest("上");
							//ShowDiaodu("检测到遥控按键 上");
							break;
						case 0xf0:
							TTSTest("下");
							//ShowDiaodu("检测到遥控按键 下");
							break;
						case 0x88:
							TTSTest("C");
							//ShowDiaodu("检测到遥控按键 C");
							break;
						case 0xb0:
							TTSTest("清除");
							//ShowDiaodu("检测到遥控按键 清除");
							break;
						case 0x28:
							TTSTest("医疗");
							//ShowDiaodu("检测到遥控按键 医疗");
							break;
						case 0xa8:
							TTSTest("纠纷");
							//ShowDiaodu("检测到遥控按键 纠纷");
							break;
						case 0x68:
							TTSTest("交通");
							//ShowDiaodu("检测到遥控按键 交通");
							break;
						case 0x50:
							TTSTest("*");
							//ShowDiaodu("检测到遥控按键 *");
							break;
						case 0xd0:
							TTSTest("#");
							//ShowDiaodu("检测到遥控按键 #");
							break;
						default:
							//ShowDiaodu("检测到遥控按键 ?");
							break;
					}
				}
				g_iTempState = iTempState;
			}
		}
			break;
#endif

#if CHK_VER == 0
		case 0xff://大遥控
		{
			switch(irkey_code)
			{
				case 0xd2:/*启动/关机*/
				{
					switch(g_enumSysSta)
					{
						case SLEEP:
						{
							v_objSleepEvent->dwStopEvt |= EVENT_TYPE_IRDA;
						}
							break;
						case IDLE:
						{
							v_objSleepEvent->dwStartEvt |= EVENT_TYPE_IRDA;
						}
							break;
						case CTRL:
							{
							v_objCtrlEvent->dwStopEvt |= EVENT_TYPE_IRDA;
							v_objSleepEvent->dwStartEvt |= EVENT_TYPE_IRDA;
						}
							break;
						case WORK:
						{
							*v_bPeriodIgnore = TRUE;
							v_objWorkEvent->dwRecStopEvt |= EVENT_TYPE_IRDA;
							v_objWorkEvent->dwUplStopEvt |= EVENT_TYPE_IRDA;
							v_objSleepEvent->dwStartEvt |= EVENT_TYPE_IRDA;
						}
							break;
						default:
							break;
					}
				}
					break;
		
				case 0x30:/*登陆/锁定*/
				{
					switch(g_enumSysSta)
					{
						case IDLE:
						{
							v_objCtrlEvent->dwStartEvt |= EVENT_TYPE_IRDA;
						}
							break;
						case CTRL:
						{
							v_objCtrlEvent->dwStopEvt |= EVENT_TYPE_IRDA;
						}
							break;
						case WORK:
						{
							*v_bPeriodIgnore = TRUE;
							v_objWorkEvent->dwRecStopEvt |= EVENT_TYPE_IRDA;
							v_objWorkEvent->dwUplStopEvt |= EVENT_TYPE_IRDA;
							v_objCtrlEvent->dwStartEvt |= EVENT_TYPE_IRDA;
						}
							break;
						default:
							break;
					}
				}
					break;

				case 0xa0:/*录像*/
				{
					switch(g_enumSysSta)
					{
						case SLEEP:
						{
							v_objSleepEvent->dwStopEvt |= EVENT_TYPE_IRDA;
							v_objWorkEvent->dwRecStartEvt |= EVENT_TYPE_IRDA;
						}
							break;
						case IDLE:
						{
							v_objWorkEvent->dwRecStartEvt |= EVENT_TYPE_IRDA;
						}
							break;
						default:
							break;
					}
				}
					break;
				case 0x80:/*停录*/
				{
					switch(g_enumSysSta)
					{
						case WORK:
						{
							*v_bPeriodIgnore = TRUE;
							v_objWorkEvent->dwRecStopEvt |= EVENT_TYPE_IRDA;
						}
							break;
						default:
							break;
					}
				}
					break;
				case 0x12:/*上*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyDirect(UP);
				}
					break;
				case 0xa2:/*下*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyDirect(DOWN);
				}
					break;
				case 0x22:/*左*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyDirect(LEFT);
				}
					break;
				case 0xe0:/*右*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyDirect(RIGHT);
				}
					break;
				case 0xba:/*ENTER*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyEnter();
				}
					break;
				case 0xb8:/*退出*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyCancel();
				}
					break;
				case 0x50:/*0*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyNumber("0");
				}
					break;
				case 0x38:/*1*/	
				{
					if(g_enumSysSta == CTRL)
						DealKeyNumber("1");
				}
					break;	
				case 0x18:/*2*/	
				{
					if(g_enumSysSta == CTRL)
						DealKeyNumber("2");
				}
					break;
				case 0xa8:/*3*/
				{	
					if(g_enumSysSta == CTRL)
						DealKeyNumber("3");
				}
					break;
				case 0x48:/*4*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyNumber("4");
				}
					break;
				case 0xc8:/*5*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyNumber("5");
				}
					break;
				case 0x28:/*6*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyNumber("6");
				}
					break;
				case 0xd8:/*7*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyNumber("7");
				}
					break;
				case 0x7a:/*8*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyNumber("8");
				}
					break;
				case 0xaa:/*9*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyNumber("9");
				}
					break;
				case 0x60:/*+*/
				{
					if(g_enumSysSta == CTRL)
						DealKeyNumber(".");
				}
					break;
				case 0x90:/*-*/
					break;
				case 0xb0:/*放像*/
				{
					if(g_enumSysSta == CTRL)
					{
						if( g_objPlaybackCtrl.AVPause )
						{
							g_objPlaybackCtrl.AVPause = FALSE;
						}
						else if(g_objPlaybackFold == DP2 || g_objPlaybackFold == DP4 || g_objPlaybackFold == DP8)
						{
							g_objPlaybackFold = DP1;
							g_objPlaybackCtrl.VSlow = TRUE;
						}
						else if(	g_objPlaybackFold == MP1 || g_objPlaybackFold == MP2 
										||g_objPlaybackFold == MP4 || g_objPlaybackFold == MP8 
									  ||g_objPlaybackFold == MM1 || g_objPlaybackFold == MM2 
									  ||g_objPlaybackFold == MM4 || g_objPlaybackFold == MM8 )
						{
							g_objPlaybackFold = DP1;
						}
					}
				}
					break;
				case 0x78:/*暂停/帧放*/
				{
					if(g_enumSysSta == CTRL)
					{
						if(g_objPlaybackCtrl.AVPause == FALSE)
							g_objPlaybackCtrl.AVPause = TRUE;
						else
							g_objPlaybackCtrl.VFrame = TRUE;
					}
				}
					break;
				case 0xc2:/*慢放*/
				{
					if(g_enumSysSta == CTRL && g_objPlaybackCtrl.AVPause == FALSE)
					{
						switch(g_objPlaybackFold)
						{
							case DP1:
								g_objPlaybackFold = DP2;
								break;
							case DP2:
								g_objPlaybackFold = DP4;
								break;
							case DP4:
								g_objPlaybackFold = DP8;
								break;
							case DP8:
								g_objPlaybackFold = DP2;
								break;
							default:
								break;
						}
				
						g_objPlaybackCtrl.VSlow = TRUE;
					}
				}
					break;
				case 0x68:/*快进*/
				{
					if(g_enumSysSta == CTRL && g_objPlaybackCtrl.AVPause == FALSE)
					{
						switch(g_objPlaybackFold)
						{
							case DP1:
								g_objPlaybackFold = MP1;
								break;
							case MP1:
								g_objPlaybackFold = MP2;
								break;
							case MP2:
								g_objPlaybackFold = MP4;
								break;
							case MP4:
								g_objPlaybackFold = MP8;
								break;
							case MP8:
								g_objPlaybackFold = MP1;
								break;
							default:
								break;
						}
					}
				}
					break;
				case 0xe8:/*快退*/
				{
					if(g_enumSysSta == CTRL && g_objPlaybackCtrl.AVPause == FALSE)
					{
						switch(g_objPlaybackFold)
						{
							case DP1:
								g_objPlaybackFold = MM1;
								break;
							case MM1:
								g_objPlaybackFold = MM2;
								break;
							case MM2:
								g_objPlaybackFold = MM4;
								break;
							case MM4:
								g_objPlaybackFold = MM8;
								break;
							case MM8:
								g_objPlaybackFold = MM1;
								break;
							default:
								break;
						}
					}
				}
					break;
				case 0xfa:/*查看*/
					break;
				case 0x0a:/*画面切换*/
					break;
				case 0x3a:/*通道参数*/
					break;
				case 0x32:/*调整*/
					break;
				case 0x92:/*变倍+*/
					break;
				case 0xf2:/*变倍-*/
					break;
				case 0x02:/*光圈+*/
					break;
				case 0xe2:/*光圈-*/
					break;
				case 0x88:/*调焦+*/
					break;				
				case 0x20:/*调焦-*/
					break;
				case 0x42:/*自动*/
					break;
				case 0x08:/*调用*/
					break;
				case 0xb2:/*预置*/
					break;
		 		case 0xd0:/*备用1*/
		 			break;
		 		case 0xf0:/*备用2*/
		 			break;
				case 0xea:/*语言*/
					break;
				case 0x6a:/*网络*/
					break;
				case 0x40:/*设置*/
					break;
				case 0x62:/*F1*/
				{
					if(g_enumSysSta == IDLE)
					{
						IOSET( IOS_TRUMPPOW, IO_TRUMPPOW_ON );//开启功放电源
						IOSET( IOS_EARPHONESEL, IO_EARPHONESEL_OFF );//选择免提通道
						IOSET( IOS_AUDIOSEL, IO_AUDIOSEL_PC );//选择PC音频
						StartAPrev();
					}
				}
					break;
				case 0x52:/*F2*/
				{
					if(g_enumSysSta == IDLE)
					{
						StopAPrev();
						IOSET( IOS_TRUMPPOW, IO_TRUMPPOW_OFF );//关闭功放电源
						IOSET( IOS_EARPHONESEL, IO_EARPHONESEL_ON );//选择耳机通道
						IOSET( IOS_AUDIOSEL, IO_AUDIOSEL_PHONE );//选择手机音频
					}
				}
					break;
				case 0x10:/*F3*/
				{
					if(g_enumSysSta == IDLE || g_enumSysSta == WORK)
					{
						StartVPrev();
					}
				}
					break;
				case 0x58:/*F4*/
				{
					if(g_enumSysSta == IDLE || g_enumSysSta == WORK)
					{
						StopVPrev();
					}
				}
					break;
				default:
					break;
			}
		}
			break;
#endif

		default:
			break;
	}
	
	return;
}

int TtyTest(char *v_szTtyWrite, char *v_szTtyRead)
{
	int iRet = 0;
	int iFttyWrite = -1, iFttyRead = -1;
	struct termios objOptionsWrite, objOptionsRead;
	speed_t IspeedWrite, OspeedWrite, IspeedRead, OspeedRead;
	char szTest[2048] = {0};

#if VEHICLE_TYPE == VEHICLE_M
	if( (iFttyWrite=open(v_szTtyWrite, O_RDWR | O_NDELAY))==-1 || tcflush(iFttyWrite, TCIOFLUSH)==-1 )
	{
		PRTMSG(MSG_ERR, "com open failed!\n");
		return -1;
	}
	
	if( write(iFttyWrite, "123456789\n", 15)<=0  )
	{
		PRTMSG(MSG_ERR, "com write err 1\n");
		perror("");
		return -1;
	}

	sleep(2);
	
	if( read(iFttyWrite, szTest, 2048) <= 0 )
	{
		PRTMSG(MSG_ERR, "com read err!\n");
		perror("");
		return -1;
	}
	if( strstr(szTest, "123456789") == 0 )
	{
		PRTMSG(MSG_ERR, "com test err!\n");
		return -1;
	}
	
	if( close(iFttyWrite) == -1 )
	{
		PRTMSG(MSG_ERR, "com close err!\n");
		perror("");
		return -1;
	}

	return 0;
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
/*	
	if( (iFttyWrite=open(v_szTtyWrite, O_RDWR|O_NDELAY))==-1 || (iFttyRead=open(v_szTtyRead, O_RDWR|O_NDELAY))==-1 )
	{
		PRTMSG(MSG_ERR, "open com failed!\n");
		iRet = -1;
		goto _END;
	}
	
	if( tcgetattr(iFttyWrite, &objOptionsWrite)==-1 || tcgetattr(iFttyRead, &objOptionsRead)==-1 )
	{
		PRTMSG(MSG_ERR, "get attr failed!\n");
		iRet = -1;
		goto _END;
	}
	
	//保存原始波特率
	IspeedWrite = cfgetispeed(&objOptionsWrite);
	OspeedWrite = cfgetospeed(&objOptionsWrite);
	IspeedRead = cfgetispeed(&objOptionsRead);
	OspeedRead = cfgetospeed(&objOptionsRead);
	
	//设置参数
	objOptionsWrite.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	objOptionsWrite.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	objOptionsWrite.c_oflag &= ~OPOST;
	cfsetispeed(&objOptionsWrite, B9600);
	cfsetospeed(&objOptionsWrite, B9600);
	objOptionsRead.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	objOptionsRead.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	objOptionsRead.c_oflag &= ~OPOST;
	cfsetispeed(&objOptionsRead, B9600);
	cfsetospeed(&objOptionsRead, B9600);
	
	if( tcsetattr(iFttyWrite, TCSANOW, &objOptionsWrite)==-1 || tcsetattr(iFttyRead, TCSANOW, &objOptionsRead)==-1 )
	{
		PRTMSG(MSG_ERR, "set attr failed!\n");
		iRet = -1;
		goto _END;
	}
	
	if( write(iFttyWrite, "123456789", 10)<=0 )
	{
		PRTMSG(MSG_ERR, "write com failed!\n");
		iRet = -1;
		goto _END;
	}
	
	sleep(1);
		
	if( read(iFttyRead, szTest, 2048)<=0 || strstr(szTest, "123456789")==0 )
	{
		PRTMSG(MSG_ERR, "read com failed!\n");
		iRet = -1;
		goto _END;
	}

_END:
	cfsetispeed(&objOptionsWrite, IspeedWrite);
	cfsetospeed(&objOptionsWrite, OspeedWrite);
	tcsetattr(iFttyWrite, TCSANOW, &objOptionsWrite);
	cfsetispeed(&objOptionsRead, IspeedRead);
	cfsetospeed(&objOptionsRead, OspeedRead);
	tcsetattr(iFttyRead, TCSANOW, &objOptionsRead);
	
	close(iFttyWrite);
	close(iFttyRead);
	return iRet;
*/
	if( (iFttyWrite=open(v_szTtyWrite, O_RDWR | O_NDELAY))==-1 )
	{
		PRTMSG(MSG_ERR, "open com failed!\n");
		iRet = -1;
		goto _END;
	}
	
	if(tcgetattr(iFttyWrite, &objOptionsWrite)==-1)
	{
		PRTMSG(MSG_ERR, "get com attr failed!\n");
		iRet = -1;
		goto _END;
	}
	
	IspeedWrite = cfgetispeed(&objOptionsWrite);
	OspeedWrite = cfgetospeed(&objOptionsWrite);
	
	objOptionsWrite.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	objOptionsWrite.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	objOptionsWrite.c_oflag &= ~OPOST;
	cfsetispeed(&objOptionsWrite, B9600);
	cfsetospeed(&objOptionsWrite, B9600);
	
	if( tcsetattr(iFttyWrite, TCSANOW, &objOptionsWrite)==-1 )
	{
		PRTMSG(MSG_ERR, "set com attr failed!\n");
		iRet = -1;
		goto _END;
	}
	
	if( write(iFttyWrite, "123456789", 10)<=0 )
	{
		PRTMSG(MSG_ERR, "write com failed!\n");
		iRet = -1;
		goto _END;
	}

	sleep(2);

	if( read(iFttyWrite, szTest, 2048) <= 0 )
	{
		PRTMSG(MSG_ERR, "read com failed!\n");
		perror("");
		iRet = -1;
		goto _END;
	}
	
	if( strstr(szTest, "123456789") == 0 )
	{
		if( 0 == strcmp(v_szTtyWrite, "ttyAMA0") )
			PRTMSG(MSG_DBG, "com recv:%s\n", szTest);
		iRet = -1;
		goto _END;
	}

_END:
	cfsetispeed(&objOptionsWrite, IspeedWrite);
	cfsetospeed(&objOptionsWrite, OspeedWrite);
	tcsetattr(iFttyWrite, TCSANOW, &objOptionsWrite);
	
	close(iFttyWrite);
	return iRet;

#endif
}
