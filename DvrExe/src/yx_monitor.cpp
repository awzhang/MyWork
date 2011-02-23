#include "yx_common.h"

static BYTE g_bytViChn = 0;
static BYTE g_bytVoChn = 0;

bool g_bDeal380A = false;
int g_iDeal380A = 0;
char g_szDeal380A[1024] = {0};

int IDLE_DELAY = 20; //分钟 
int CTRL_DELAY = 20; //分钟 

tagPhotoInfo g_objPhotoInfo;

char szCurTime[9]="00:00:00";
char szIdleEndTime[9]="00:00:00";
char szRecEndTime[9]="00:00:00";
char szUplEndTime[9]="00:00:00";
char szPicEndTime[9]="00:00:00";
char szTtsEndTime[9]="00:00:00";
char szBlkEndTime[9]="00:00:00";

bool IDLESTATE(void *Event)
{
	if( *((BYTE*)Event)==IDLE )
		return true;
	else
		return false;
}

bool CTRLSTATE(void *Event)
{
	if( *((BYTE*)Event)==CTRL )
		return true;
	else
		return false;
}

bool SLEEPSTATE(void *Event)
{
	if( *((BYTE*)Event)==SLEEP )
		return true;
	else
		return false;
}

void FlushSysState( OSD_REGION v_objRegion[], BOOL v_bRegionOn[], int v_iFlag )
{
#if VEHICLE_TYPE == VEHICLE_M

#if NO_INFO == 0
	int i;
	int iRegionNum;
	int	iLocation[4][2];
	char szContent[50];
	OSD_BITMAP objBitmap;
	
	if( v_iFlag == 0 )
	{
		for(i = 0; i < 4; i++)
		{
			if( v_bRegionOn[i] )
			{
				sprintf(szContent, "%d", i+1);
				if( g_objIdleStart.VPreview[i] )
					strcat(szContent, " 预览");
				else
					strcat(szContent, "     ");
				if( g_objWorkStart.VRecord[i] )
					strcat(szContent, " 录像");
				else
					strcat(szContent, "     ");
				if( g_objWorkStart.VUpload[i] )
					strcat(szContent, " 监控");
				else
					strcat(szContent, "     ");
				strcat(szContent, "\n");
						
				CreateBitmap(&objBitmap, 1, CountCharRow(szContent));							
				DrawCharacter(&objBitmap, szContent, WHITE, 0x000000, 0, 0, FALSE);	
				ShowBitmap(&(v_objRegion[i]), &objBitmap, 0, 0);
			}
		}
	}
	else if( v_iFlag == 1 )
	{
		switch( g_objMvrCfg.VInput.VMode )
		{
			case CIF:
				iLocation[0][0] = 2;
				iLocation[0][1] = 2;
				iLocation[1][0] = 2;
				iLocation[1][1] = 42;
				iLocation[2][0] = 13;
				iLocation[2][1] = 2;
				iLocation[3][0] = 13;
				iLocation[3][1] = 42;
				iRegionNum = 4;
				break;
			case HD1:
				iLocation[0][0] = 2;
				iLocation[0][1] = 2;
				iLocation[1][0] = 13;
				iLocation[1][1] = 2;
				iRegionNum = 2;
				break;
			case D1:
				iLocation[0][0] = 2;
				iLocation[0][1] = 2;
				iRegionNum = 1;
				break;
			default:
				break;
		}
		
		for(i = 0; i < iRegionNum; i++)
		{
			if( !v_bRegionOn[i] )
			{
				sprintf(szContent, "%d", i+1);
				if( g_objIdleStart.VPreview[i] )
					strcat(szContent, " 预览");
				else
					strcat(szContent, "     ");
				if( g_objWorkStart.VRecord[i] )
					strcat(szContent, " 录像");
				else
					strcat(szContent, "     ");
				if( g_objWorkStart.VUpload[i] )
					strcat(szContent, " 监控");
				else
					strcat(szContent, "     ");
				strcat(szContent, "\n");
						
				CreateRegion(&(v_objRegion[i]), 0, iLocation[i][0], iLocation[i][1], 1, CountCharRow(szContent));
				CreateBitmap(&objBitmap, 1, CountCharRow(szContent));							
				DrawCharacter(&objBitmap, szContent, WHITE, 0x000000, 0, 0, FALSE);	
				ShowBitmap(&(v_objRegion[i]), &objBitmap, 0, 0);
				v_bRegionOn[i] = TRUE;
			}
		}
	}
	else if( v_iFlag == -1 )
	{
		for(i = 0; i < 4; i++)
		{
			if( v_bRegionOn[i] )
			{
				DeleteRegion(v_objRegion[i]);
				v_bRegionOn[i] = FALSE;
			}
		}
	}
#endif
	
#endif
}

void FlushShowTime( OSD_REGION *v_objRegion, int v_iCDateTime[], BOOL *v_bRegionOn, int v_iFlag )
{
#if VEHICLE_TYPE == VEHICLE_M

#if NO_INFO == 0
	char szContent[50] = {0};
	OSD_BITMAP objBitmap;
	
	if( v_iFlag == 0 )// 刷新时间域
	{
		if( *v_bRegionOn )
		{
			sprintf(szContent, "%04d-%02d-%02d  %02d:%02d:%02d\n", v_iCDateTime[0], v_iCDateTime[1], v_iCDateTime[2], v_iCDateTime[3], v_iCDateTime[4], v_iCDateTime[5]);
//			CreateBitmap(&objBitmap, 1, 20);							
//			DrawCharacter(&objBitmap, szContent, WHITE, 0x000000, 0, 0, FALSE);	
//			ShowBitmap( v_objRegion, &objBitmap, 0, 0 );
			CreateBitmap(&objBitmap, 1, 20);							
//			DrawCharacter(&objBitmap, szContent, 0x7c00, 0x03e0, 0, 0, FALSE);	
			ShowBitmap( v_objRegion, &objBitmap, 0, 0 );
		}
	}
	else if( v_iFlag == 1 )// 创建时间域
	{
		if( !(*v_bRegionOn) )
		{
			sprintf(szContent, "%04d-%02d-%02d  %02d:%02d:%02d\n", v_iCDateTime[0], v_iCDateTime[1], v_iCDateTime[2], v_iCDateTime[3], v_iCDateTime[4], v_iCDateTime[5]);
//			CreateRegion( v_objRegion, 0, 1, 20, 1, 20 );
//			CreateBitmap( &objBitmap, 1, 20 );
//			DrawCharacter(&objBitmap, szContent, WHITE, 0x000000, 0, 0, FALSE);
//			ShowBitmap( v_objRegion, &objBitmap, 0, 0 );
			CreateRegion( v_objRegion, 0, 10, 20, 1, 20 );
			CreateBitmap( &objBitmap, 1, 20 );
//			DrawCharacter(&objBitmap, szContent, 0x7c00, 0x03e0, 0, 0, FALSE);
			ShowRegion(*v_objRegion);
			ShowBitmap( v_objRegion, &objBitmap, 0, 0 );

			*v_bRegionOn = TRUE;
		}
	}
	else if( v_iFlag == -1 )// 删除时间域
	{
		if( *v_bRegionOn )
		{
			DeleteRegion(*v_objRegion);
			*v_bRegionOn = FALSE;
		}
	}
#endif

#endif
}

void RenewRecRptBuf(WORK_EVENT *v_objWorkEvent, byte *v_bytRecRptBuf)
{
	*(v_bytRecRptBuf+9) = 0x00;
	*(v_bytRecRptBuf+10) = 0x00;

	if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_IRDA )    *(v_bytRecRptBuf+9) |= 0x10; //遥控指令
	if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_CENT )    *(v_bytRecRptBuf+9) |= 0x40; //中心指令
	if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_PERIOD )  *(v_bytRecRptBuf+9) |= 0x20; //时段信号
	if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_ACCON )   *(v_bytRecRptBuf+9) |= 0x80; //Acc信号
	if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_MOVE )    *(v_bytRecRptBuf+9) |= 0x08; //运动信号
	if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_OVERLAY ) *(v_bytRecRptBuf+9) |= 0x04; //遮挡信号
	if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_ALERT )   *(v_bytRecRptBuf+9) |= 0x02; //S1信号
	if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_ALERT2 )  *(v_bytRecRptBuf+9) |= 0x01; //S2信号
	if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_ALERT3 )  *(v_bytRecRptBuf+10) |= 0x80; //S3信号
	if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_ALERT4 )  *(v_bytRecRptBuf+10) |= 0x40; //S4信号
}

void RenewIdleTime( int v_iCurDateTime[], char *v_szIdleEndTime, int v_bFlag )
{
	if( v_bFlag == 0 )
	{
		sprintf( v_szIdleEndTime, "%02d:%02d:%02d", (v_iCurDateTime[3]+(v_iCurDateTime[4]+IDLE_DELAY)/60), ((v_iCurDateTime[4]+IDLE_DELAY)%60), v_iCurDateTime[5] );
	}
	else if( v_bFlag == -1 )
	{
		strcpy(v_szIdleEndTime, "00:00:00");
	}
}

void RenewWorkTime( int v_iWorkStyle, int v_iCurDateTime[], char *v_szEndTime, WORK_EVENT *v_objWorkEvent, int v_bFlag )
{
	char szEventEndTime[9] = {0};
	
	if( v_bFlag == 0 )
	{
		switch(v_iWorkStyle)
		{
			case WORK_REC:
			{
				//遥控指令
				if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_IRDA )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", (v_iCurDateTime[3]+(v_iCurDateTime[4]+30)/60), ((v_iCurDateTime[4]+30)%60), v_iCurDateTime[5] );//遥控默认录像30分钟
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_IRDA;
				}
				
				//中心指令
				if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_CENT )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", 
									(3600*v_iCurDateTime[3] + 60*v_iCurDateTime[4] + v_iCurDateTime[5] + g_objMvrCfg.AVUpload.UploadSecond)/3600, 
									((3600*v_iCurDateTime[3] + 60*v_iCurDateTime[4] + v_iCurDateTime[5] + g_objMvrCfg.AVUpload.UploadSecond)%3600)/60, 
									(3600*v_iCurDateTime[3] + 60*v_iCurDateTime[4] + v_iCurDateTime[5] + g_objMvrCfg.AVUpload.UploadSecond)%60 );
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_CENT;
				}
				
				//时段信号(延时1秒，防止超时)
				if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_PERIOD )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", 
									(3600*v_iCurDateTime[3] + 60*v_iCurDateTime[4] + v_iCurDateTime[5] + 1)/3600, 
									((3600*v_iCurDateTime[3] + 60*v_iCurDateTime[4] + v_iCurDateTime[5] + 1)%3600)/60, 
									(3600*v_iCurDateTime[3] + 60*v_iCurDateTime[4] + v_iCurDateTime[5] + 1)%60 );
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_PERIOD;
				}
				
				//Acc信号
				if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_ACCON )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", 
									(v_iCurDateTime[3] + (v_iCurDateTime[4]+g_objMvrCfg.EventTrig.AccDelay)/60), 
									((v_iCurDateTime[4] + g_objMvrCfg.EventTrig.AccDelay) % 60), 
									v_iCurDateTime[5] );
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ACCON;
				}
				
				//运动信号
				if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_MOVE )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", 
									(v_iCurDateTime[3] + (v_iCurDateTime[4] + g_objMvrCfg.EventTrig.MoveDelay) / 60), 
									((v_iCurDateTime[4] + g_objMvrCfg.EventTrig.MoveDelay) % 60), 
									v_iCurDateTime[5] );
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_MOVE;
				}
				
				//遮挡信号
				if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_OVERLAY )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", 
									(v_iCurDateTime[3] + (v_iCurDateTime[4] + g_objMvrCfg.EventTrig.OverlayDelay)/60), 
									((v_iCurDateTime[4] + g_objMvrCfg.EventTrig.OverlayDelay) % 60), 
									v_iCurDateTime[5] );
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_OVERLAY;
				}
				
				//S1信号
				if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_ALERT )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", 
									(v_iCurDateTime[3] + (v_iCurDateTime[4] + g_objMvrCfg.EventTrig.S1Delay) / 60), 
									((v_iCurDateTime[4] + g_objMvrCfg.EventTrig.S1Delay) % 60), 
									v_iCurDateTime[5] );
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT;
				}
				
				//S2信号
				if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_ALERT2 )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", 
									(v_iCurDateTime[3] + (v_iCurDateTime[4] + g_objMvrCfg.EventTrig.S2Delay) / 60), 
									((v_iCurDateTime[4] + g_objMvrCfg.EventTrig.S2Delay) % 60), 
									v_iCurDateTime[5] );
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT2;
				}
				
				//S3信号
				if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_ALERT3 )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", 
									(v_iCurDateTime[3] + (v_iCurDateTime[4] + g_objMvrCfg.EventTrig.S3Delay) / 60), 
									((v_iCurDateTime[4] + g_objMvrCfg.EventTrig.S3Delay) % 60), 
									v_iCurDateTime[5] );
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT3;
				}
				
				//S4信号
				if( v_objWorkEvent->dwRecStartEvt & EVENT_TYPE_ALERT4 )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", 
									(v_iCurDateTime[3] + (v_iCurDateTime[4] + g_objMvrCfg.EventTrig.S4Delay) / 60), 
									((v_iCurDateTime[4] + g_objMvrCfg.EventTrig.S4Delay) % 60), 
									v_iCurDateTime[5] );
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT4;
				}
			}
				break;
			case WORK_UPL:
			{
				if( v_objWorkEvent->dwUplStartEvt & EVENT_TYPE_CENT )
				{
					sprintf( szEventEndTime, "%02d:%02d:%02d", 
						(3600*v_iCurDateTime[3]+60*v_iCurDateTime[4]+v_iCurDateTime[5]+g_objMvrCfg.AVUpload.UploadSecond)/3600, 
						((3600*v_iCurDateTime[3]+60*v_iCurDateTime[4]+v_iCurDateTime[5]+g_objMvrCfg.AVUpload.UploadSecond)%3600)/60, 
						(3600*v_iCurDateTime[3]+60*v_iCurDateTime[4]+v_iCurDateTime[5]+g_objMvrCfg.AVUpload.UploadSecond)%60 );
					if(strcmp(v_szEndTime, szEventEndTime) < 0) strcpy(v_szEndTime, szEventEndTime);
					v_objWorkEvent->dwUplStartEvt &= ~EVENT_TYPE_CENT;
				}
			}
				break;
			case WORK_PIC:
			{
				if( v_objWorkEvent->dwPicStartEvt & EVENT_TYPE_CENT )
				{
					v_objWorkEvent->dwPicStartEvt &= ~EVENT_TYPE_CENT;
				}
			}
				break;
			case WORK_TTS:
			{
				if( v_objWorkEvent->dwTtsStartEvt & EVENT_TYPE_CENT )
				{
					v_objWorkEvent->dwTtsStartEvt &= ~EVENT_TYPE_CENT;
				}
			}
				break;
			case WORK_BLK:
			{
				if( v_objWorkEvent->dwBlkStartEvt & EVENT_TYPE_CENT )
				{
					v_objWorkEvent->dwBlkStartEvt &= ~EVENT_TYPE_CENT;
				}
			}
				break;
			default:
				break;
		}
	}
	else if( v_bFlag == -1 )
	{
		strcpy(v_szEndTime, "00:00:00");
		
		switch(v_iWorkStyle)
		{
			case WORK_REC:
			{
				v_objWorkEvent->dwRecStopEvt = 0;
			}
				break;
			case WORK_UPL:
			{
				v_objWorkEvent->dwUplStopEvt = 0;
			}
				break;
			case WORK_PIC:
			{
				v_objWorkEvent->dwPicStopEvt = 0;
			}
				break;
			default:
				break;
		}
	}
}

void DetectEvent( BOOL *v_bPeriodIgnore, SLEEP_EVENT *v_pSleepEvent, WORK_EVENT *v_pWorkEvent, CTRL_EVENT *v_pCtrlEvent )
{
	int i;
	uint uiStatus;
	int iResult;
	char szBuf[2048] = {0};
	BYTE  bytLvl;
	DWORD dwLen;
	DWORD dwPushTm = 0;
	
	BYTE ucIOState;

	IRKEY_INFO_T 	objIrda = {0};
	
	//检测时段触发事件
	if(   (g_objMvrCfg.PeriodTrig.PeriodStart[0] && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.StartTime[0]) > 0 && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.EndTime[0]) < 0) 
			||(g_objMvrCfg.PeriodTrig.PeriodStart[1] && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.StartTime[1]) > 0 && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.EndTime[1]) < 0) 
			||(g_objMvrCfg.PeriodTrig.PeriodStart[2] && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.StartTime[2]) > 0 && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.EndTime[2]) < 0) 
			||(g_objMvrCfg.PeriodTrig.PeriodStart[3] && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.StartTime[3]) > 0 && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.EndTime[3]) < 0) 
			||(g_objMvrCfg.PeriodTrig.PeriodStart[4] && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.StartTime[4]) > 0 && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.EndTime[4]) < 0) 
			||(g_objMvrCfg.PeriodTrig.PeriodStart[5] && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.StartTime[5]) > 0 && strcmp(szCurTime, g_objMvrCfg.PeriodTrig.EndTime[5]) < 0) )
	{
		switch(g_enumSysSta)
		{
			case SLEEP:
			{
				v_pSleepEvent->dwStopEvt |= EVENT_TYPE_PERIOD;
			}
				break;
			case IDLE:
			case WORK:
			{
				v_pWorkEvent->dwRecStartEvt |= EVENT_TYPE_PERIOD;
			}
				break;
			default:
				break;
		}
	}
	
	//检测Acc触发事件
	IOGet( IOS_ACC, &ucIOState );
	if( ucIOState == IO_ACC_ON )
	{
		if( g_objMvrCfg.EventTrig.AccStart )
		{
			switch(g_enumSysSta)
			{
				case SLEEP:
				{
					v_pSleepEvent->dwStopEvt |= EVENT_TYPE_ACCON;
				}
				case IDLE:
				case WORK:
				{
					v_pWorkEvent->dwRecStartEvt |= EVENT_TYPE_ACCON;
				}
					break;
				default:
					break;
			}
		}
	}

	//检测移动、遮挡触发事件
#if VEHICLE_TYPE == VEHICLE_M
	ioctl(g_objDevFd.Tw2835, TW2835_IRQ_STATUS_READ, &uiStatus);
	for(i = 0; i < 4; i++)
	{
		if(((uiStatus >> 24) & (0x08 >> i)) > 0)//检测摄像头状态
			g_objDevSta.Camera[i] = FALSE;
		else
			g_objDevSta.Camera[i] = TRUE;
	}
	if(((uiStatus >> 16) & 0x0f) > 0)//检测到移动信号
	{
		if( g_objMvrCfg.EventTrig.MoveStart )
		{
			switch(g_enumSysSta)
			{
				case SLEEP:
				{
					v_pSleepEvent->dwStopEvt |= EVENT_TYPE_MOVE;
				}
				case IDLE:
				case WORK:
				{
					v_pWorkEvent->dwRecStartEvt |= EVENT_TYPE_MOVE;
				}
					break;
				default:
					break;
			}
		}
	}
	if(((uiStatus >> 8) & 0x0f) > 0)//检测到遮挡信号
	{
		if(g_objMvrCfg.EventTrig.OverlayStart)
		{
			switch(g_enumSysSta)
			{
				case SLEEP:
				{
					v_pSleepEvent->dwStopEvt |= EVENT_TYPE_OVERLAY;
				}
				case IDLE:
				case WORK:
				{
					v_pWorkEvent->dwRecStartEvt |= EVENT_TYPE_OVERLAY;
				}
					break;
				default:
					break;
			}
		}
	}
#endif
	
	//检测报警触发事件
	iResult = g_objIoMng.PopData(bytLvl, sizeof(szBuf), dwLen, szBuf, dwPushTm);
	if( !iResult && dwLen == 1 )
	{
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		IOGet( IOS_POWDETM, &ucIOState );
#endif

		if(szBuf[0] == IO_ALARM_ON)
		{
			if( g_objMvrCfg.EventTrig.S1Start )
			{
				switch(g_enumSysSta)
				{
					case SLEEP:
					{
						v_pSleepEvent->dwStopEvt |= EVENT_TYPE_ALERT;
					}
					case IDLE:
					case WORK:
					{
						v_pWorkEvent->dwRecStartEvt |= EVENT_TYPE_ALERT;
					}
						break;
					default:
						break;
				}
			}
		}
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		else if((szBuf[0] == IO_POWDETM_INVALID) || (szBuf[0] == IO_QIANYA_VALID && ucIOState == IO_POWDETM_INVALID))
		{
			switch(g_enumSysSta)
			{
				case IDLE:
				{
					v_pSleepEvent->dwStartEvt |= EVENT_TYPE_POWOFF;
				}
					break;
				case CTRL:
				{
					v_pCtrlEvent->dwStopEvt |= EVENT_TYPE_POWOFF;
					v_pSleepEvent->dwStartEvt |= EVENT_TYPE_POWOFF;
				}
					break;
				case WORK:
				{
					*v_bPeriodIgnore = TRUE;
					v_pWorkEvent->dwRecStopEvt |= EVENT_TYPE_POWOFF;
					v_pWorkEvent->dwUplStopEvt |= EVENT_TYPE_POWOFF;
					v_pSleepEvent->dwStartEvt |= EVENT_TYPE_POWOFF;
				}
					break;
				default:
					break;
			}
		}
#endif
	}
	
	//检测中心指令触发事件
	iResult = g_objQianMng.PopData( bytLvl, sizeof(szBuf), dwLen, szBuf, dwPushTm, NULL, NULL, 0 );
	if( !iResult && dwLen <= sizeof(szBuf) )
	{
		DealQianSmsData(szBuf, (int)dwLen, v_pSleepEvent, v_pWorkEvent, v_pCtrlEvent);
	}
	
	//检测遥控指令触发事件
// 	iResult = g_objQianIrdaMng.PopData( bytLvl, sizeof(szBuf), dwLen, szBuf, dwPushTm, NULL, NULL, 0 );
// 	if( !iResult && dwLen == 2 )
// 	{
// 		DealIrdaKey((BYTE)szBuf[0], (BYTE)szBuf[1], v_bPeriodIgnore, v_pSleepEvent, v_pWorkEvent, v_pCtrlEvent);
// 	}
	iResult = read(g_objDevFd.Irda, &objIrda, sizeof(IRKEY_INFO_T));
	if(iResult > 0)
	{
		DealIrdaKey(objIrda.irkey.sys_id_code, objIrda.irkey.irkey_code, v_bPeriodIgnore, v_pSleepEvent, v_pWorkEvent, v_pCtrlEvent);
	}
	
	//检测拍照触发事件
	iResult = g_objQianPicMng.PopData( bytLvl, sizeof(szBuf), dwLen, szBuf, dwPushTm, NULL, NULL, 0 );
	if( !iResult && dwLen>0 && dwLen <= sizeof(szBuf) )
	{
		DealPicEvent((tagPhotoInfo*)szBuf, v_pSleepEvent, v_pWorkEvent, v_pCtrlEvent);
	}
	
	//检测TTS触发事件
	iResult = g_objQianTTSMng.PopData( bytLvl, sizeof(szBuf), dwLen, szBuf, dwPushTm, NULL, NULL, 0 );
	if( !iResult && dwLen>0 && dwLen<=sizeof(szBuf) )
	{
		PRTMSG(MSG_DBG, "recv tts data:%s, dwLen = %d\n", szBuf, dwLen);

		g_objWorkStop.TTS = FALSE;
		
		//防止语音合成时间过长而来不及清看门狗
		ClearThreadDog(0);
		MakeTTS(szBuf); // 合成TTS语音
		ClearThreadDog(0);
		
		switch(g_enumSysSta)
		{
			case SLEEP:
			{
				v_pSleepEvent->dwStopEvt |= EVENT_TYPE_CENT;
			}
			case IDLE:
			case WORK:
			{
				v_pWorkEvent->dwTtsStartEvt |= EVENT_TYPE_CENT;
			}
				break;
			default:
				break;
		}
	}
	
	//检测Tw2865异常触发事件
	iResult = g_objDevErrMng.PopData(bytLvl, sizeof(szBuf), dwLen, szBuf, dwPushTm);
	if( !iResult && dwLen == 1 )
	{
		if(szBuf[0] == 0x01)
		{
			switch(g_enumSysSta)
			{
				case IDLE:
				{
					v_pWorkEvent->dwRecStartEvt = 0;
					v_pWorkEvent->dwUplStartEvt = 0;
					v_pWorkEvent->dwPicStartEvt = 0;
					v_pWorkEvent->dwTtsStartEvt = 0;
					v_pWorkEvent->dwBlkStartEvt = 0;

					v_pSleepEvent->dwStartEvt |= EVENT_TYPE_ERROR;
				}
					break;
				case CTRL:
				{
					v_pCtrlEvent->dwStopEvt |= EVENT_TYPE_ERROR;
					v_pSleepEvent->dwStartEvt |= EVENT_TYPE_ERROR;
				}
					break;
				case WORK:
				{
					v_pWorkEvent->dwRecStartEvt = 0;
					v_pWorkEvent->dwUplStartEvt = 0;
					v_pWorkEvent->dwPicStartEvt = 0;
					v_pWorkEvent->dwTtsStartEvt = 0;
					v_pWorkEvent->dwBlkStartEvt = 0;

					*v_bPeriodIgnore = TRUE;
					v_pWorkEvent->dwRecStopEvt |= EVENT_TYPE_ERROR;
					v_pWorkEvent->dwUplStopEvt |= EVENT_TYPE_ERROR;
					v_pWorkEvent->dwPicStopEvt |= EVENT_TYPE_ERROR;
					v_pWorkEvent->dwTtsStopEvt |= EVENT_TYPE_ERROR;
					v_pWorkEvent->dwBlkStopEvt |= EVENT_TYPE_ERROR;
					v_pSleepEvent->dwStartEvt |= EVENT_TYPE_ERROR;
				}
					break;
				default:
					break;
			}
		}
	}
	
	//检测磁盘异常触发事件
	if((strcmp("SDisk", g_objMvrCfg.AVRecord.DiskType) == 0 && (g_objUsbSta.SDisk[0] == UCON_UMOU_UVAL || g_objUsbSta.SDisk[0] == CON_UMOU_UVAL || g_objUsbSta.SDisk[1] == UCON_UMOU_UVAL || g_objUsbSta.SDisk[1] == CON_UMOU_UVAL)) 
	 ||(strcmp("HDisk", g_objMvrCfg.AVRecord.DiskType) == 0 && (g_objUsbSta.HDisk[0] == UCON_UMOU_UVAL || g_objUsbSta.HDisk[0] == CON_UMOU_UVAL || g_objUsbSta.HDisk[1] == UCON_UMOU_UVAL || g_objUsbSta.HDisk[1] == CON_UMOU_UVAL)))
	{
		switch(g_enumSysSta)
		{
			case WORK:
			{
				*v_bPeriodIgnore = TRUE;//屏蔽时段触发
				v_pWorkEvent->dwRecStopEvt |= EVENT_TYPE_ERROR;
				v_pWorkEvent->dwPicStopEvt |= EVENT_TYPE_ERROR;
			}
				break;
			default:
				break;
		}
	}
	
	/*检测到超时, 触发停止录像事件*/
	if( (g_objWorkStart.VRecord[0] || g_objWorkStart.VRecord[1] || g_objWorkStart.VRecord[2] || g_objWorkStart.VRecord[3]) 
		&&(strcmp(szCurTime, szRecEndTime)>0 || strcmp(szCurTime, "23:59:59")==0) )
	{
		if( g_iTempState!=1 )//避免摄像头检测后超时
		{
			PRTMSG(MSG_DBG, "timeout stop record, szCurTime:%s, szRecEndTime:%s\n", szCurTime, szRecEndTime);
			v_pWorkEvent->dwRecStopEvt |= EVENT_TYPE_TIMEOUT;
		}
	}
	
	/*检测到超时, 触发停止监控事件*/
	if(	(g_objWorkStart.VUpload[0] || g_objWorkStart.VUpload[1] || g_objWorkStart.VUpload[2] || g_objWorkStart.VUpload[3] ) 
		&&(strcmp(szCurTime, szUplEndTime) > 0))// || strcmp(szCurTime, "23:59:59") == 0))
	{
		PRTMSG(MSG_DBG, "timeout stop upload, szCurTime:%s, szUplEndTime:%s\n", szCurTime, szUplEndTime);
		v_pWorkEvent->dwUplStopEvt |= EVENT_TYPE_TIMEOUT;
	}
}

void SelectEvent( BOOL *v_bPeriodIgnore, SLEEP_EVENT *v_pSleepEvent, WORK_EVENT *v_pWorkEvent, CTRL_EVENT *v_pCtrlEvent )
{
	if((strcmp("SDisk", g_objMvrCfg.AVRecord.DiskType) == 0 && (g_objUsbSta.SDisk[0] == UCON_UMOU_UVAL || g_objUsbSta.SDisk[0] == CON_UMOU_UVAL || g_objUsbSta.SDisk[1] == UCON_UMOU_UVAL || g_objUsbSta.SDisk[1] == CON_UMOU_UVAL)) 
	 ||(strcmp("HDisk", g_objMvrCfg.AVRecord.DiskType) == 0 && (g_objUsbSta.HDisk[0] == UCON_UMOU_UVAL || g_objUsbSta.HDisk[0] == CON_UMOU_UVAL || g_objUsbSta.HDisk[1] == UCON_UMOU_UVAL || g_objUsbSta.HDisk[1] == CON_UMOU_UVAL)))
	{
		switch(g_enumSysSta)
		{
			case IDLE:
			case WORK:
			{
				v_pWorkEvent->dwRecStartEvt = 0;
				v_pWorkEvent->dwPicStartEvt = 0;
				g_objPhoto.m_objDataMngToShot.Release();
			}
				break;
			default:
				break;
		}
	}
	
	switch( g_objMvrCfg.TrigPara.TrigType )
	{
		case TRIG_PER:/*时段触发*/
		{
			v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_ACCON;
			v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_MOVE;
			v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_OVERLAY;
			v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_ALERT;
			v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_ALERT2;
			v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_ALERT3;
			v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_ALERT4;
								
			if( *v_bPeriodIgnore ) 
				v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_PERIOD;
			v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ACCON;
			v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_MOVE;
			v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_OVERLAY;
			v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT;
			v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT2;
			v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT3;
			v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT4;
		}
			break;
			
		case TRIG_EVE:/*事件触发*/
		{
			v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_PERIOD;	
			v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_PERIOD;
		}
			break;
			
		case TRIG_PER_AND_EVE:/*时段和事件触发*/
		{
			if( !(v_pSleepEvent->dwStopEvt & EVENT_TYPE_PERIOD) )
			{
				v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_ACCON;
				v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_MOVE;
				v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_OVERLAY;
				v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_ALERT;
				v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_ALERT2;
				v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_ALERT3;
				v_pSleepEvent->dwStopEvt &= ~EVENT_TYPE_ALERT4;
			}
								
			if( !(v_pWorkEvent->dwRecStartEvt & EVENT_TYPE_PERIOD) )
			{
				v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ACCON;
				v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_MOVE;
				v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_OVERLAY;
				v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT;
				v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT2;
				v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT3;
				v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_ALERT4;
			}
		}
			break;
			
		case TRIG_PER_OR_EVE:/*时段或事件触发*/
		{
			if( *v_bPeriodIgnore ) 
				v_pWorkEvent->dwRecStartEvt &= ~EVENT_TYPE_PERIOD;
		}
			break;
			
		default:
			break;
	}
}

void Init( int v_iSysState, SLEEP_EVENT *v_objSleepEvent, CTRL_EVENT *v_objCtrlEvent, WORK_EVENT *v_objWorkEvent )
{
	switch(v_iSysState)
	{
		case SLEEP:
		{
			StopVPrev();
			StopVo();
			StopVdec();
			StopAPrev();
			StopAo();
			StopAdec();
			StopVi();
			StopVencChn();
			StopVenc();
			StopAi();
			StopAencChn();
			StopAenc();
			MPP_Exit();
			
			//UmountDisk(SDISK, 3000);
			
#if VEHICLE_TYPE == VEHICLE_M
			IOSET( IOS_VIDICONPOW, IO_VIDICONPOW_OFF );//关闭摄像头电源	
			IOSET( IOS_HUBPOW, IO_HUBPOW_ON );//开启HUB/SD电源
			//IOSET( IOS_HARDPOW, IO_HARDPOW_OFF );//关闭硬盘电源
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
			IOSET( IOS_VIDICONPOW, IO_VIDICONPOW_OFF );//关闭摄像头电源	
#if VEHICLE_TYPE == VEHICLE_V8
			IOSET(IOS_USBPOW, IO_USBPOW_OFF);
#endif
#if VEHICLE_TYPE == VEHICLE_M2
			//IOSET(IOS_HUBPOW, IO_HUBPOW_OFF);
			IOSET(IOS_HARDPOW, IO_HARDPOW_OFF);
#endif
			IOSET(IOS_LCDPOW, IO_LCDPOW_OFF);
			IOSET(IOS_TRUMPPOW, IO_TRUMPPOW_OFF);
			IOSET(IOS_SCHEDULEPOW, IO_SCHEDULEPOW_OFF);
			sleep(2);
			close(g_objDevFd.Tw2865);
			RMMOD(KO_YX3511_TW2865);
			RMMOD(KO_HI3511_USB);
#endif

			v_objSleepEvent->dwStartEvt = 0;
			v_objCtrlEvent->dwStartEvt = 0;
			v_objWorkEvent->dwRecStartEvt = 0;
			v_objWorkEvent->dwUplStartEvt = 0;
			v_objWorkEvent->dwPicStartEvt = 0;
			v_objWorkEvent->dwTtsStartEvt = 0;
			v_objWorkEvent->dwBlkStartEvt = 0;

			//SetCpuSlow();/*cpu进入slow模式*/
			SetClkDisable();/*关闭时钟*/	
		}
			break;
		case IDLE:
		{
			MPP_Exit();
			MPP_Init();
	
			StartAenc();
			StartAi();
			StartAencChn();
 			StartVenc();
 			StartVi();
 			StartVencChn();
			StartAdec();
			StartAo();
 			StartVdec();
 			StartVo();

 			StartVPrev();
		}
			break;
		case CTRL:
		{
			IOSET( IOS_TRUMPPOW, IO_TRUMPPOW_ON );//开启功放电源
			IOSET( IOS_EARPHONESEL, IO_EARPHONESEL_OFF );//选择免提通道
			IOSET( IOS_AUDIOSEL, IO_AUDIOSEL_PC);//选择PC音频
								
			StopVPrev();
			StopVo();
			StartVo();	
			DealKeyLogin();
								
			v_objSleepEvent->dwStartEvt = 0;
			v_objCtrlEvent->dwStartEvt = 0;
			v_objWorkEvent->dwRecStartEvt = 0;
			v_objWorkEvent->dwUplStartEvt = 0;
			v_objWorkEvent->dwPicStartEvt = 0;
			v_objWorkEvent->dwTtsStartEvt = 0;
			v_objWorkEvent->dwBlkStartEvt = 0;
		}
			break;
		case WORK:
		{
			v_objSleepEvent->dwStartEvt = 0;
			v_objCtrlEvent->dwStartEvt = 0;
		}
			break;
		default:
			break;
	}
}

void Release( int v_iSysState, SLEEP_EVENT *v_objSleepEvent, CTRL_EVENT *v_objCtrlEvent, WORK_EVENT *v_objWorkEvent )
{
	switch(v_iSysState)
	{
		case SLEEP:
		{
//			SetCpuNormal();/*cpu进入normal模式*/
			SetClkEnable();/*启动时钟*/
			
#if VEHICLE_TYPE == VEHICLE_M
			IOSET(IOS_VIDICONPOW, IO_VIDICONPOW_ON, 100);//开启摄像头电源
			IOSET(IOS_TW2835RST, IO_TW2835RST_LO, 100);//复位TW2835
			IOSET(IOS_TW2835RST, IO_TW2835RST_HI, 100);
			IOSET(IOS_HUBPOW, IO_HUBPOW_ON);//开启HUB/SD电源
			IOSET(IOS_HARDPOW, IO_HARDPOW_ON);//开启硬盘电源
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
			IOSET(IOS_VIDICONPOW, IO_VIDICONPOW_ON, 100);//开启摄像头电源
			IOSET(IOS_TW2865RST, IO_TW2865RST_LO, 100);//复位TW2865
			IOSET(IOS_TW2865RST, IO_TW2865RST_HI, 100);
#if VEHICLE_TYPE == VEHICLE_V8
			IOSET(IOS_USBPOW, IO_USBPOW_ON, 100);
#endif
#if VEHICLE_TYPE == VEHICLE_M2
			IOSET(IOS_HUBPOW, IO_HUBPOW_ON, 100);
			IOSET(IOS_HARDPOW, IO_HARDPOW_ON, 100);
#endif
			IOSET(IOS_LCDPOW, IO_LCDPOW_ON);
			IOSET(IOS_SCHEDULEPOW, IO_SCHEDULEPOW_ON);
			sleep(2);
			INSMOD(KO_HI3511_USB);
			INSMOD(KO_YX3511_TW2865, 100);
			g_objDevFd.Tw2865 = open("/dev/misc/tw2865adev", O_RDWR);
#endif

			//MountDisk(SDISK);
			
			MPP_Init();
			StartAenc();
			StartAi();
			StartAencChn();
			StartVenc();
			StartVi();
			StartVencChn();
			StartAdec();
			StartAo();
			StartVdec();
			StartVo();	
			StartVPrev();
			
			/*清空所有省电状态检测事件*/
			v_objSleepEvent->dwStopEvt	= 0;
		}
			break;
		case IDLE:
		{
			StopVPrev();
			StopVo();
			StopVdec();
			StopAPrev();
			StopAo();
			StopAdec();
			StopVi();
			StopVencChn();
			StopVenc();
			StopAi();
			StopAencChn();
			StopAenc();
						
			UmountDisk(SDISK);
			UmountDisk(HDISK);
			UmountDisk(UDISK);
		}
			break;
		case CTRL:
		{
			DealKeyLock();
						
			StopAVPlay();
			StopVo();
			StopVdec();
			StopVi();
			StopVencChn();
			StopVenc();
			sleep(1);
			StartVenc();
			StartVencChn();
			StartVi();
			StartVdec();
			StartVo();	
		  StartVPrev();
								
			IOSET(IOS_TRUMPPOW, IO_TRUMPPOW_OFF);//关闭功放电源
			IOSET(IOS_EARPHONESEL, IO_EARPHONESEL_ON);//选择耳机通道
			IOSET(IOS_AUDIOSEL, IO_AUDIOSEL_PHONE);//选择手机音频
								
			/*清空所有操作状态检测事件*/
			v_objCtrlEvent->dwStopEvt = 0;
		}
			break;
		case WORK:
		{
			/*清空所有工作状态检测事件*/
			v_objWorkEvent->dwRecStopEvt = 0;
			v_objWorkEvent->dwUplStopEvt = 0;
			v_objWorkEvent->dwPicStopEvt = 0;
			v_objWorkEvent->dwTtsStopEvt = 0;
			v_objWorkEvent->dwBlkStartEvt = 0;
		}
			break;
		default:
			break;
	}
}

void *G_CmdReceive(void *arg)
{
	int   iResult;
	char  szBuf[2048] = {0};
	BYTE  bytLvl;
	DWORD dwPopLen;
	DWORD dwPushSymb;
	DWORD dwPacketNum = 0;
	
	int iTempState;
	char buf[256] = {0};
	buf[0] = 0x01;
	buf[1] = 0x72;
	buf[2] = 0x01;

	iResult = DataWaitPop(DEV_DVR);
	while( !iResult )
	{
		if( g_bProgExit )  
		{ 
			break; 
		} 
		
		memset(szBuf, 0, sizeof(szBuf));
		
		while( !DataPop( szBuf, sizeof(szBuf), &dwPopLen, &dwPushSymb, DEV_DVR, &bytLvl ) )
		{
			switch(dwPushSymb)
			{
				case DEV_IO:
					{
						switch(szBuf[0])
						{
							case IO_ALARM_ON:
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
							case IO_QIANYA_VALID:
							case IO_POWDETM_INVALID:
#endif
							{
								g_objIoMng.PushData( 1, 1, szBuf, dwPacketNum);
							}
								break;
							default:
								break;
						}
					}
					break;
					
				case DEV_SOCK:
					{
						g_objSockMng.PushData( bytLvl, dwPopLen, szBuf, dwPacketNum);
					}
					break;
					
				case DEV_QIAN:
					{
						switch( szBuf[0] )
						{
							case 0x01:		// TTS播放数据
							{
								if( szBuf[1] == 0x01 )	// 立即终止当前TTS播放
								{
									StopTTS();
									g_objQianTTSMng.Release();
								}

								g_objQianTTSMng.PushData( bytLvl, dwPopLen-2, szBuf+2, dwPacketNum );
							}
								break;
							case 0x02:		// 千里眼中心指令
							{
								g_objQianMng.PushData( bytLvl, dwPopLen-1, szBuf+1, dwPacketNum );
							}
								break;
							case 0x03:		// 红外遥控按键数据
							{
								g_objQianIrdaMng.PushData( bytLvl, dwPopLen-1, szBuf+1, dwPacketNum );
							}	
								break;
							case 0x04:		// 拍照事件类型
							{
								g_objQianPicMng.PushData( bytLvl, dwPopLen-1, szBuf+1, dwPacketNum );
							}	
								break;
							default:
								break;
						}
					}
					break;

				case DEV_DIAODU:
					{
						buf[3] = '\0';

						switch( szBuf[0] )
						{
							case 0x01:	// 系统音量调节
							{
								SetSysVolume( true, szBuf[1] );
							}
								break;

							case 0x02:  // 串口、SD卡、硬盘、U盘检测
							{
#if VEHICLE_TYPE == VEHICLE_M
								ShowDiaodu("SD卡:%s;硬盘:%s;U盘:%s", DiskTest("SDisk")==0 ? "有效" : "无效", DiskTest("HDisk")==0 ? "有效" : "无效", DiskTest("UDisk")==0 ? "有效" : "无效");
#endif
#if VEHICLE_TYPE == VEHICLE_V8
								ShowDiaodu("串口1:%s;串口2:%s;SD卡:%s;U盘:%s", 
														TtyTest("/dev/ttyAMA0")==0 ? "有效" : "无效", TtyTest("/dev/ttySIM1")==0 ? "有效" : "无效",
														DiskTest("SDisk")==0 ? "有效" : "无效", DiskTest("UDisk")==0 ? "有效" : "无效");
#endif
#if VEHICLE_TYPE == VEHICLE_M2
								ShowDiaodu("串口1:%s;串口2:%s;SD卡:%s;U盘:%s;硬盘:%s", 
									TtyTest("/dev/ttyAMA0")==0 ? "有效" : "无效", TtyTest("/dev/ttySIM1")==0 ? "有效" : "无效",
									DiskTest("SDisk")==0 ? "有效" : "无效", DiskTest("UDisk")==0 ? "有效" : "无效", DiskTest("HDisk")==0 ? "有效" : "无效");
#endif
							}
								break;

							case 0x03://系统升级、应用升级时保持空闲状态
							{
								iTempState = g_iTempState;
								g_iTempState = 1;
							}
								break;

							case 0x04: // PC音频检测
							{
								iTempState = g_iTempState;
								g_iTempState = 1;
								if( WaitForEvent(IDLESTATE, &g_enumSysSta, 10000)==0 )
								{
									PRTMSG(MSG_NOR, "Start TTS Test\n");
									TTSTest("这是TTS语音测试");
								}
								g_iTempState = iTempState;
							}
								break;

							case 0x05: // 录音录像检测
							{
								iTempState = g_iTempState;
								g_iTempState = 1;
								if( WaitForEvent(IDLESTATE, &g_enumSysSta, 10000)==0 )
								{
									PRTMSG(MSG_NOR, "Start AV Test\n");
									AVTest((int)szBuf[1]);
								}
								g_iTempState = iTempState;
							}
								break;

							case 0x06://格式化SD卡、硬盘
							{
								sleep(1);
								iTempState = g_iTempState;

								if(strcmp("SDisk", g_objMvrCfg.AVRecord.DiskType) == 0)
								{
									g_iTempState = 2;
									sleep(5);
									ShowDiaodu("格式化SD卡%s", (WaitForEvent(IDLESTATE, &g_enumSysSta, 10000)==0 && DiskFormat("SDisk")==0) ? "成功" : "失败");
								}
								else if(strcmp("HDisk", g_objMvrCfg.AVRecord.DiskType) == 0)
								{
									g_iTempState = 3;
									sleep(5);
									ShowDiaodu("格式化硬盘%s", (WaitForEvent(IDLESTATE, &g_enumSysSta, 10000)==0 && DiskFormat("HDisk")==0) ? "成功" : "失败");
								}

								g_iTempState = iTempState;
							}
								break;

							case 0x07:		//请求检测SD卡、硬盘状态
							{
								struct statfs objDiskBuf;
								double dDiskUsage;
								char szTempBuf[100] = {0};
								
								if(strcmp("SDisk", g_objMvrCfg.AVRecord.DiskType) == 0)
								{
									strcat(buf, "SD卡：");

									for(int i=0; i<2; i++)
									{
										if( MOUNT(&g_objUsbSta.SDisk[i]) )
										{
											memset(&objDiskBuf, 0, sizeof(objDiskBuf));
											statfs(SPRINTF(szTempBuf, "/mnt/SDisk/part%d", i+1), &objDiskBuf);
											dDiskUsage = 1 - objDiskBuf.f_bavail/(double)objDiskBuf.f_blocks;
												
											memset(szTempBuf, 0, sizeof(szTempBuf));
											sprintf(szTempBuf, "分区%d使用率%.3f%% ", i+1, 100*dDiskUsage);
											strcat(buf, szTempBuf);	
										}
										else
										{
											memset(szTempBuf, 0, sizeof(szTempBuf));
											sprintf(szTempBuf, "分区%d未挂载 ", i+1);
											strcat(buf, szTempBuf);		
										}
									}
								}
								else if(strcmp("HDisk", g_objMvrCfg.AVRecord.DiskType) == 0)
								{
									strcat(buf, "硬盘：");

									for(int i=0; i<2; i++)
									{
										if( MOUNT(&g_objUsbSta.HDisk[i]) )
										{
											memset(&objDiskBuf, 0, sizeof(objDiskBuf));
											statfs(SPRINTF(szTempBuf, "/mnt/HDisk/part%d", i+1), &objDiskBuf);
											dDiskUsage = 1 - objDiskBuf.f_bavail/(double)objDiskBuf.f_blocks;
												
											memset(szTempBuf, 0, sizeof(szTempBuf));
											sprintf(szTempBuf, "分区%d使用率%.3f%% ", i+1, 100*dDiskUsage);
											strcat(buf, szTempBuf);
										}
										else
										{
											memset(szTempBuf, 0, sizeof(szTempBuf));
											sprintf(szTempBuf, "分区%d未挂载 ", i+1);
											strcat(buf, szTempBuf);
										}
									}
								}
								
								DataPush(buf, strlen(buf), DEV_DVR, DEV_DIAODU, LV2);									
							}
								break;

							case 0x08://退出空闲状态
							{
								g_iTempState = iTempState;
							}
								break;
								
							case 0x09://开始红外检测
							{
								g_bIrTest = true;
							}
								break;
							
							case 0x0a://停止红外检测
							{
								g_bIrTest = false;
							}
								break;
								
							case 0x0b://ComuExe通知已恢复出厂设置
							{
								iTempState = g_iTempState;
								g_iTempState = 1;
								if( WaitForEvent(IDLESTATE, &g_enumSysSta, 20000)==0 )
								{
									GetSecCfg(&g_objMvrCfg, sizeof(g_objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(g_objMvrCfg));
									StopVPrev();
									StopVo();
									StopVdec();
									StopVi();
									StopVencChn();
									StopVenc();
									sleep(1);
									StartVenc();
									StartVencChn();
									StartVi();
									StartVdec();
									StartVo();	
								  StartVPrev();
								}
								g_iTempState = iTempState;
							}
								break;
							case 0x0c://ComuExe通知手机号、APN、IP、端口设置
							{
								iTempState = g_iTempState;
								g_iTempState = 1;
								if( WaitForEvent(IDLESTATE, &g_enumSysSta, 20000)==0 )
									GetSecCfg(&g_objMvrCfg, sizeof(g_objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(g_objMvrCfg));
								g_iTempState = iTempState;
								
								//音视频上传线程重新读取手机号
								GetImpCfg( &g_objComuCfg, sizeof(g_objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(g_objComuCfg) );
							}
								break;
							case 0x0d://切换预览通道
							{
								tag2DVOPar objVOutput;
								
								SwitchVPrev(g_bytViChn, g_bytVoChn, -1);
								StopVPrev();
						
								g_bytViChn = szBuf[1];
								if(g_bytViChn == 0x0a)	// 是否请求全通道预览
								{
									g_bytVoChn = 0x0a;
									
									objVOutput.VMode = CIF;
									SwitchVo(objVOutput);
									StartVPrev();
								}
								else
								{
									g_bytVoChn = 0x00;
									
									objVOutput.VMode = D1;
									SwitchVo(objVOutput);
									SwitchVPrev(g_bytViChn, g_bytVoChn, 1);
								}
							}
								break;
							case 0x0e:	//新增视频预览切换接口
							{
								CtrlVideoView(szBuf[1]);
							}
								break;

							case 0x99:	// 测试用
								{
									// 3809查询时间段请求: 通道号（1）+ 起始日期（3）
									//char buf[4] = {0x01, 0x0b, 0x01, 0x1a};
									//char buf = 0x01;
									//Deal3809(&buf, 1);
									
// 									// 380a黑匣子上传请求：通道号（1）+ 事件类型（1）+ 起始日期（3）+ 起始时间（3）+ 结束日期（3） + 结束时间（3）
// 									char buf[] = {0x04, 0x00, 0x0b, 0x01, 0x1a, 0x07, 0x0a, 0x00, 0x0b, 0x01, 0x1a, 0x07, 0x0b, 0x00};
// 									Deal380A(buf, sizeof(buf));
								}
// 
// 							case 0x9a:	// 测试用
// 								{
// 									// D1应答：文件类型（1）＋ 文件名（40）＋ 时间序号（6）＋ 应答类型（1）
// 									char buf[] = {0x03, 0x30, 0x5f, 0x32, 0x30, 0x31, 0x31, 0x2d, 0x30, 0x31, 0x2d, 0x32, 0x36, 0x5f, 0x30, 0x37, 0x2d, 0x31, 0x30, 0x2d, 0x30, 0x30, 0x5f, 0x30, 0x37, 0x2d, 0x31, 0x31, 0x2d, 0x30, 0x30, 0x2e, 0x69, 0x6e, 0x64, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0b, 0x01, 0x1a, 0x0d, 0x33, 0x01};
// 									Deal38D1(buf, sizeof(buf));
// 								}
// 								break;
// 
// 							case 0x9b:	// 测试用
// 								{
// 									// D3应答：文件类型（1）＋ 文件名（40）＋ 应答类型（1）+ 【请求重传的包个数(8) + 包序号（8）* M】
// 									char buf[] = {0x03, 0x30, 0x5f, 0x32, 0x30, 0x31, 0x31, 0x2d, 0x30, 0x31, 0x2d, 0x32, 0x36, 0x5f, 0x30, 0x37, 0x2d, 0x31, 0x30, 0x2d, 0x30, 0x30, 0x5f, 0x30, 0x37, 0x2d, 0x31, 0x31, 0x2d, 0x30, 0x30, 0x2e, 0x69, 0x6e, 0x64, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01};
// 									Deal38D3(buf, sizeof(buf));
// 								}
// 								break;
								
							default:
								break;
						}
					}
					break;

				default:
					break;
			}
		}
		
		iResult = DataWaitPop(DEV_DVR);
	}

	// 接收线程退出，则整个程序退出
	g_bProgExit = true;
}

void *G_SysSwitch(void *arg)
{
	BOOL bTimeRegionOn = FALSE;
	BOOL bChnRegionOn[4] = { FALSE };
	OSD_REGION objTimeRegion;
	OSD_REGION objChnRegion[4];

	BOOL bPeriodIgnore = FALSE;
	
	SLEEP_EVENT objSleepEvent = {FALSE};
	CTRL_EVENT objCtrlEvent  = {FALSE};
	WORK_EVENT objWorkEvent  = {FALSE};

	int iCurDateTime[6];  
	int lLastSec, lCurSec;

	bool bNewIdle = true;		// 是否重新进入空闲状态	
	DWORD dwIdleBeginTm = GetTickCount();	// 系统进入空闲状态的开始时刻

	/*开机进入空闲状态*/
	Init( IDLE, NULL, NULL, NULL );
 	
 	g_objPhoto.Init();

	lLastSec = lCurSec = GetCurDateTime( iCurDateTime );
	
	FlushShowTime( &objTimeRegion, iCurDateTime, &bTimeRegionOn, 1 );
	FlushSysState( objChnRegion, bChnRegionOn, 1 );
	
	RenewIdleTime( iCurDateTime, szIdleEndTime, 0 );
	
	sprintf( szCurTime, "%02d:%02d:%02d", iCurDateTime[3], iCurDateTime[4], iCurDateTime[5] );
	
	while(1)
	{
		RenewMemInfoHalfMin(0);
		ClearThreadDog(0);
		
		DetectEvent( &bPeriodIgnore, &objSleepEvent, &objWorkEvent, &objCtrlEvent );//检测触发事件
		SelectEvent( &bPeriodIgnore, &objSleepEvent, &objWorkEvent, &objCtrlEvent );//根据触发类型选择触发事件
/*----------------------------------------------------------------------------------------------------------------*/
		//视频黑匣子查询时必须首先停止录像使其写入文件列表
		if(g_bDeal380A)
		{
			StopAVRec();
			sleep(3);
			Deal380A(g_szDeal380A, g_iDeal380A);
		}
		
		//根据触发事件转换系统状态
		switch( g_enumSysSta )
		{
			case SLEEP:/*省电状态事件检测*/
				{
					/*检测到停止省电状态事件, 即从省电状态进入空闲状态*/
					if(	g_iTempState || g_bProgExit || objSleepEvent.dwStopEvt )
					{
						//按启动键重启设备
						if( objSleepEvent.dwStopEvt & EVENT_TYPE_IRDA )
						{
							G_ResetSystem();
						}
						
						Release( SLEEP, &objSleepEvent, NULL, NULL );
						RenewIdleTime( iCurDateTime, szIdleEndTime, 0 );/*记录空闲状态结束时间*/

						FlushShowTime( &objTimeRegion, iCurDateTime, &bTimeRegionOn, 1 );
						FlushSysState( objChnRegion, bChnRegionOn, 1 );
						
						g_enumSysSta = IDLE;
						PRTMSG(MSG_NOR, "System In Idle\n");
						
						RenewMemInfo(0x02, 0x01, 0x00, 0x01);

						// 通知QianExe，系统进入非省电状态
						char buf = 0x04;
						DataPush(&buf, 1, DEV_DVR, DEV_QIAN, LV1);
					}
				}
				break;
			case IDLE:/*空闲状态事件检测*/
				{
					if( g_iTempState==1 )
					{
						bNewIdle = true;
						break;
					}
					else if( g_iTempState==2 )
					{
						ClearThreadDog(0);
						
						bNewIdle = true;
						ShowDiaodu("正在格式化SD卡,请稍侯...");
						sleep(3);
						break;
					}
					else if( g_iTempState==3 )
					{
						ClearThreadDog(0);
						
						bNewIdle = true;
						ShowDiaodu("正在格式化硬盘,请稍侯...");
						sleep(3);
						break;
					}

					if( true == bNewIdle )
					{
						bNewIdle = false;
						dwIdleBeginTm = GetTickCount();
					}
					if( GetTickCount() - dwIdleBeginTm > IDLE_DELAY*60*1000 )
					{
						//还原为默认的20分钟省电时间
						IDLE_DELAY = 20;
							
						BYTE ucIOState;
						IOGet( IOS_ACC, &ucIOState );
						if( ucIOState==IO_ACC_OFF )
						{
							//ACC无效时进入省电状态
							PRTMSG(MSG_NOR, "timeout start sleep\n");
							objSleepEvent.dwStartEvt |= EVENT_TYPE_TIMEOUT;
						}
						else
						{
							//ACC有效时重新计时
							bNewIdle = true;
						}
					}
					
					/*检测信号退出程序*/
					if( g_bProgExit )
					{
						FlushShowTime( &objTimeRegion, iCurDateTime, &bTimeRegionOn, -1 );
						FlushSysState( objChnRegion, bChnRegionOn, -1 );
						
						Release( IDLE, NULL, NULL, NULL );
						
						sleep(2);
						return NULL;
					}
					/*检测到启动省电状态事件, 即从空闲状态进入省电状态*/
					else if( objSleepEvent.dwStartEvt )
					{
						FlushShowTime( &objTimeRegion, iCurDateTime, &bTimeRegionOn, -1 );
						FlushSysState( objChnRegion, bChnRegionOn, -1 );
						
						Init( SLEEP, &objSleepEvent, &objCtrlEvent, &objWorkEvent );
						RenewIdleTime( iCurDateTime, szIdleEndTime, -1 );/*清空空闲状态结束时间*/
						
						g_enumSysSta = SLEEP;	
						bNewIdle = true;
						PRTMSG(MSG_NOR, "System In Sleep\n");
						
						RenewMemInfo(0x02, 0x01, 0x00, 0x00);

						// 通知QianExe，系统进入省电状态
						char buf = 0x03;
						DataPush(&buf, 1, DEV_DVR, DEV_QIAN, LV1);
					}
					/*检测到启动操作状态事件, 即从空闲状态进入操作状态*/
					else if( objCtrlEvent.dwStartEvt )
					{
						FlushShowTime( &objTimeRegion, iCurDateTime, &bTimeRegionOn, -1 );
						FlushSysState( objChnRegion, bChnRegionOn, -1 );
						
						Init( CTRL, &objSleepEvent, &objCtrlEvent, &objWorkEvent );
						RenewIdleTime( iCurDateTime, szIdleEndTime, -1 );/*清空空闲状态结束时间*/

						g_enumSysSta = CTRL;
						bNewIdle = true;
						PRTMSG(MSG_NOR, "System In Ctrl\n");
						
						RenewMemInfo(0x02, 0x01, 0x00, 0x03);
					}
					/*检测到启动工作状态事件, 即从空闲状态进入工作状态*/
					else if( objWorkEvent.dwRecStartEvt ||objWorkEvent.dwUplStartEvt ||objWorkEvent.dwPicStartEvt ||objWorkEvent.dwTtsStartEvt ||objWorkEvent.dwBlkStartEvt )
					{
						Init( WORK, &objSleepEvent, &objCtrlEvent, &objWorkEvent );
						RenewIdleTime( iCurDateTime, szIdleEndTime, -1 );/*清空空闲状态结束时间*/
							
						g_enumSysSta = WORK;
						bNewIdle = true;
						PRTMSG(MSG_NOR, "System In Work\n");
						
						RenewMemInfo(0x02, 0x01, 0x00, 0x02);
					}
				}
				break;
			case CTRL:/*操作状态事件检测*/
				{
					/*检测到停止操作状态事件, 即从操作状态进入空闲状态*/
					if(g_iTempState || g_bProgExit || objCtrlEvent.dwStopEvt)
					{
						Release( CTRL, NULL, &objCtrlEvent, NULL );
						RenewIdleTime( iCurDateTime, szIdleEndTime, 0 );/*记录空闲状态结束时间*/
						bPeriodIgnore = FALSE;/*解除时段触发屏蔽*/
						
						FlushShowTime( &objTimeRegion, iCurDateTime, &bTimeRegionOn, 1 );
						FlushSysState( objChnRegion, bChnRegionOn, 1 );
						
						g_enumSysSta = IDLE;
						PRTMSG(MSG_NOR, "System In Idle\n");
						
						RenewMemInfo(0x02, 0x01, 0x00, 0x01);
					}
				}
				break;
			case WORK:/*工作状态事件检测*/
				{
					/*检测到启动录像事件, 即启动录像*/
					if( objWorkEvent.dwRecStartEvt )
					{
						if( StartAVRec() == 0 )
						{
							FlushSysState( objChnRegion, bChnRegionOn, 0 );
							RenewRecRptBuf( &objWorkEvent, g_bytRecRptBuf );
							bPeriodIgnore = FALSE;/*解除屏蔽时段触发的启动工作状态事件*/
						}
						GetCurDateTime( iCurDateTime );
					  RenewWorkTime( WORK_REC, iCurDateTime, szRecEndTime, &objWorkEvent, 0 );
					}
					
					/*检测到停止录像事件, 即停止录像*/
					if( g_iTempState || g_bProgExit || objWorkEvent.dwRecStopEvt )
					{
						//如果是超时退出录像时立即进入省电状态
						if(objWorkEvent.dwRecStopEvt & EVENT_TYPE_TIMEOUT)
							IDLE_DELAY = 0;
						
						/*记录停录触发条件*/
						if((objWorkEvent.dwRecStopEvt & EVENT_TYPE_ERROR)||(objWorkEvent.dwRecStopEvt & EVENT_TYPE_POWOFF))
							g_bytRecRptBuf[13] = 0x05;
						else if(objWorkEvent.dwRecStopEvt & EVENT_TYPE_IRDA)
							g_bytRecRptBuf[13] = 0x04;
						else if(objWorkEvent.dwRecStopEvt & EVENT_TYPE_CENT)
							g_bytRecRptBuf[13] = 0x03;
						else if(objWorkEvent.dwRecStopEvt & EVENT_TYPE_TIMEOUT)
							g_bytRecRptBuf[13] = 0x02;

						if( StopAVRec() == 0 )
						{
							FlushSysState( objChnRegion, bChnRegionOn, 0 );
							sleep(1);// 避免跨天时出错
						}
						RenewWorkTime( WORK_REC, iCurDateTime, szRecEndTime, &objWorkEvent, -1 );
					}
/*----------------------------------------------------------------------------------------------------------------*/
					/*检测到启动监控事件, 即启动监控*/
					if( !g_bDeal380A && objWorkEvent.dwUplStartEvt )
					{
						RenewMemInfo(0x02, 0x02, 0x02, 0x01);
							
//						FlushShowTime( &objTimeRegion, iCurDateTime, &bTimeRegionOn, -1 );
// 						FlushSysState( objChnRegion, bChnRegionOn, -1 );

//						StopVPrev();	
						if( StartAVUpl() == 0 )
							FlushSysState( objChnRegion, bChnRegionOn, 0 );
						RenewWorkTime( WORK_UPL, iCurDateTime, szUplEndTime, &objWorkEvent, 0 );
					}
					
					/*检测到停止监控事件, 即停止监控*/
					if( g_bDeal380A || g_iTempState || g_bProgExit || objWorkEvent.dwUplStopEvt )
					{
						if(objWorkEvent.dwUplStopEvt & EVENT_TYPE_CENT)
							g_bytUplRptBuf[1] = 0x42;
						else 
							g_bytUplRptBuf[1] = 0x44;
						
						if( StopAVUpl() == 0 )
							FlushSysState( objChnRegion, bChnRegionOn, 0 );

						RenewWorkTime( WORK_UPL, iCurDateTime, szUplEndTime, &objWorkEvent, -1 );
//						StartVPrev();

//						FlushShowTime( &objTimeRegion, iCurDateTime, &bTimeRegionOn, 1 );
//						FlushSysState( objChnRegion, bChnRegionOn, 1 );
					}
/*----------------------------------------------------------------------------------------------------------------*/
					/*检测到启动拍照事件, 即启动拍照*/
					if( objWorkEvent.dwPicStartEvt )
					{
						if( StartPicture() == 0 )
							FlushSysState( objChnRegion, bChnRegionOn, 0 );

						RenewWorkTime( WORK_PIC, iCurDateTime, szPicEndTime, &objWorkEvent, 0 );
					}

					/*检测到停止拍照事件, 即停止拍照*/
					if(g_iTempState || g_bProgExit || objWorkEvent.dwPicStopEvt)
					{
						if( StopPicture() == 0 )
							FlushSysState( objChnRegion, bChnRegionOn, 0 );
						RenewWorkTime( WORK_PIC, iCurDateTime, szPicEndTime, &objWorkEvent, -1 );
					}
/*----------------------------------------------------------------------------------------------------------------*/
					/*检测到启动TTS事件, 即启动TTS*/
					if(objWorkEvent.dwTtsStartEvt)
					{
						if( StartTTS() == 0 )
							FlushSysState( objChnRegion, bChnRegionOn, 0 );
						RenewWorkTime( WORK_TTS, iCurDateTime, szTtsEndTime, &objWorkEvent, 0 );
					}

					/*检测到停止TTS事件, 即停止TTS*/
					if(g_iTempState || g_bProgExit || objWorkEvent.dwTtsStopEvt)
					{
						if( StopTTS() == 0 )
							FlushSysState( objChnRegion, bChnRegionOn, 0 );
						RenewWorkTime( WORK_TTS, iCurDateTime, szTtsEndTime, &objWorkEvent, -1 );
					}
/*----------------------------------------------------------------------------------------------------------------*/
					/*检测到启动黑匣子事件, 即启动黑匣子*/
					if(objWorkEvent.dwBlkStartEvt)
					{
						if( StartBlack() == 0 )
							FlushSysState( objChnRegion, bChnRegionOn, 0 );
						RenewWorkTime( WORK_BLK, iCurDateTime, szBlkEndTime, &objWorkEvent, 0 );
					}

					/*检测到停止黑匣子事件, 即停止黑匣子*/
					if(g_iTempState || g_bProgExit || objWorkEvent.dwBlkStopEvt)
					{
						if( StopBlack() == 0 )
							FlushSysState( objChnRegion, bChnRegionOn, 0 );
						RenewWorkTime( WORK_BLK, iCurDateTime, szBlkEndTime, &objWorkEvent, -1 );
					}
/*----------------------------------------------------------------------------------------------------------------*/
					/*从工作状态进入空闲状态*/
					if( !g_objWorkStart.ARecord[0] && !g_objWorkStart.ARecord[1] && !g_objWorkStart.ARecord[2] && !g_objWorkStart.ARecord[3] 
						&&!g_objWorkStart.VRecord[0] && !g_objWorkStart.VRecord[1] && !g_objWorkStart.VRecord[2] && !g_objWorkStart.VRecord[3] 
						&&!g_objWorkStart.AUpload[0] && !g_objWorkStart.AUpload[1] && !g_objWorkStart.AUpload[2] && !g_objWorkStart.AUpload[3] 
						&&!g_objWorkStart.VUpload[0] && !g_objWorkStart.VUpload[1] && !g_objWorkStart.VUpload[2] && !g_objWorkStart.VUpload[3]
						&&!g_objWorkStart.Picture
						&&!g_objWorkStart.TTS
						&&!g_objWorkStart.Black )
					{
						Release( WORK, NULL, NULL, &objWorkEvent );
						GetCurDateTime( iCurDateTime );
						RenewIdleTime( iCurDateTime, szIdleEndTime, 0 );/*记录空闲状态结束时间*/
						
						/*清空工作状态结束时间*/
						RenewWorkTime( WORK_UPL, iCurDateTime, szUplEndTime, &objWorkEvent, -1 );
						RenewWorkTime( WORK_REC, iCurDateTime, szRecEndTime, &objWorkEvent, -1 );
						RenewWorkTime( WORK_PIC, iCurDateTime, szPicEndTime, &objWorkEvent, -1 );
						RenewWorkTime( WORK_TTS, iCurDateTime, szTtsEndTime, &objWorkEvent, -1 );
						RenewWorkTime( WORK_BLK, iCurDateTime, szBlkEndTime, &objWorkEvent, -1 );
						
						FlushSysState( objChnRegion, bChnRegionOn, 0 );
						
						g_enumSysSta = IDLE;
						PRTMSG(MSG_NOR, "System In Idle\n");
						
						RenewMemInfo(0x02, 0x01, 0x00, 0x01);
					}
				}
				break;
			default:
				break;
		}
		
		if(g_bDeal380A)
		{
			g_bDeal380A = false;
		}
/*----------------------------------------------------------------------------------------------------------------*/
		msleep(50);
		
		lLastSec = lCurSec;
		lCurSec = GetCurDateTime( iCurDateTime );
		
		//刷新显示时间
		if( lLastSec < lCurSec )
			FlushShowTime( &objTimeRegion, iCurDateTime, &bTimeRegionOn, 0 );

		sprintf( szCurTime, "%02d:%02d:%02d", iCurDateTime[3], iCurDateTime[4], iCurDateTime[5] );
	}
}
