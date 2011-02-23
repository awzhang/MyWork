#include "yx_common.h"

bool g_bIrTest = false;
volatile bool g_bProgExit = false;
#if CHK_VER == 1
	int g_iTempState = 1;
#else 
	int g_iTempState = 0;
#endif

CTimerMng g_objTimerMng;

char  szATest[4][255] = {0};
char  szVTest[4][255] = {0};
char  szITest[4][255] = {0};

unsigned char 		*g_pMenuPage			=	NULL;
tag2DDvrCfg				g_objMvrCfg;
DEVICE_FD 				g_objDevFd				=	{NULL, NULL, -1, -1, -1, -1};
USB_STATE 				g_objUsbSta 			= {UCON_UMOU_UVAL};
USB_UMOUNT				g_objUsbUmount		=	{FALSE};
SYSTEM_STATE			g_enumSysSta			=	IDLE;
IDLE_START   			g_objIdleStart		= {FALSE};
IDLE_STOP  				g_objIdleStop			= {FALSE};
CTRL_START   			g_objCtrlStart		= {FALSE};
CTRL_STOP  				g_objCtrlStop			= {FALSE};
WORK_START   			g_objWorkStart		= {FALSE};
WORK_STOP  				g_objWorkStop			= {FALSE};
DEVICE_STATE			g_objDevSta 			= {FALSE};
PLAYBACK_FOLD			g_objPlaybackFold	= DP1;
PLAYBACK_CTRL   	g_objPlaybackCtrl = {FALSE};
QianGps						g_objQianGps      = {0, 9, 10, 15, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

AV_UPLOAD_BUF			g_objAVUplBuf = 
{ 
	{0},
	FALSE, {NULL},
	FALSE, {NULL},
};

CInnerDataMng	g_objVideoDataMng[4];
CInnerDataMng	g_objAudioDataMng[4];

AV_RECORD_BUF		g_objAVRecBuf = {{NULL}, {NULL}};

BYTE 						g_bytRecRptBuf[100] 	= {0};
BYTE 						g_bytUplRptBuf[100] 	= {0};

pthread_mutex_t    g_AVListMutex;

CInnerDataMng	  g_objIoMng;
CInnerDataMng	  g_objSockMng;
CInnerDataMng	  g_objQianMng;
CInnerDataMng	  g_objQianTTSMng;
CInnerDataMng	  g_objQianPicMng;
CInnerDataMng	  g_objQianIrdaMng;

CInnerDataMng	  g_objDevErrMng;

CPhoto	g_objPhoto;
CSock	g_objSock;

bool g_bUploadOnlyISlice = false;

TTS_PLAY_BUF g_objTTSPlayBuf = {FALSE, NULL, NULL, NULL, NULL, NULL, 0, TRUE, {0}};

//通过RTC时间设置系统时间(格林威治时间)
bool SetTimeByRtc()
{
	int iDevRtc = open("/dev/misc/rtc", O_NONBLOCK);
	if(-1 ==iDevRtc)
	{
		PRTMSG(MSG_ERR,"open rtc dev err\n");
	}

	struct rtc_time objRtcTime;
	struct tm objSetTime;
	ioctl(iDevRtc, RTC_RD_TIME, &objRtcTime);
	
	objSetTime.tm_year = objRtcTime.tm_year;
	objSetTime.tm_mon = objRtcTime.tm_mon;	//0~11
	objSetTime.tm_mday = objRtcTime.tm_mday;//1~31
	objSetTime.tm_hour = objRtcTime.tm_hour;//0~23
	objSetTime.tm_min = objRtcTime.tm_min;//0~59
	objSetTime.tm_sec = objRtcTime.tm_sec;//0~59

	PRTMSG(MSG_DBG, "year = %d, mon = %d, day = %d, hour = %d, min = %d, sec = %d\n", 
		objSetTime.tm_year, objSetTime.tm_mon, objSetTime.tm_mday, objSetTime.tm_hour, objSetTime.tm_min, objSetTime.tm_sec);
	
	// 若读取RTC时间出错，则全部强制改成2010年6月1号, 同时调整RTC时间
	if( (objSetTime.tm_year<110)
		||(objSetTime.tm_mon<0 || objSetTime.tm_mon>11)
		||(objSetTime.tm_mday<1 || objSetTime.tm_mday>31)
		||(objSetTime.tm_hour<0 || objSetTime.tm_hour>23)
		||(objSetTime.tm_min<0 || objSetTime.tm_min>59)
		||(objSetTime.tm_sec<0 || objSetTime.tm_sec>59))
	{
		objSetTime.tm_sec = 0;
		objSetTime.tm_min = 0;
		objSetTime.tm_hour = 10;
		objSetTime.tm_mday = 1;
		objSetTime.tm_mon = 5;
		objSetTime.tm_year = 110;
		
		objRtcTime.tm_sec = 0;
		objRtcTime.tm_min = 0;
		objRtcTime.tm_hour = 10;
		objRtcTime.tm_mday = 1;
		objRtcTime.tm_mon =	5;	
		objRtcTime.tm_year = 2010;
		ioctl(iDevRtc, RTC_SET_TIME, &objRtcTime);
	}
	
	if(0 != SetCurTime(&objSetTime))
	{
		PRTMSG(MSG_ERR,"set cur time by rtc err \n");
	}
	else
	{
		PRTMSG(MSG_DBG,"set cur time by rtc succ \n");
	}
	
	close(iDevRtc);
	iDevRtc = -1;
}

int GetCurDateTime( int v_iCDateTime[] )
{
	int iCurTime;

	struct tm p_objCurTime;
	iCurTime = G_GetTime(&p_objCurTime);
	
	v_iCDateTime[0] = p_objCurTime.tm_year+1900;
	v_iCDateTime[1] = p_objCurTime.tm_mon+1;
	v_iCDateTime[2] = p_objCurTime.tm_mday;
	v_iCDateTime[3] = p_objCurTime.tm_hour;
	v_iCDateTime[4] = p_objCurTime.tm_min;
	v_iCDateTime[5] = p_objCurTime.tm_sec;	
	
	return iCurTime;
}

int SetSysVolume(bool bSetNew, char szSet)
{
	tag2DDvrCfg objMvrCfg;
	GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));			

	byte bytVolume = objMvrCfg.AVPlayback.m_bytSysVolume;

	if( !bSetNew )
	{
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		ioctl(g_objDevFd.Tw2865,TW2865CMD_AUDIO_VOLUME, &bytVolume);
#endif
	}
	else
	{
		if( szSet == 0x01 )		// 音量加1
		{
			bytVolume++;
			if( bytVolume > 15 )	bytVolume = 15;
		}
		else if( szSet == 0x02)		// 音量减1
		{
			bytVolume--;
			if( bytVolume == 0 )	bytVolume = 1;
		}
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		ioctl(g_objDevFd.Tw2865,TW2865CMD_AUDIO_VOLUME, &bytVolume);
#endif
		objMvrCfg.AVPlayback.m_bytSysVolume = bytVolume;
		SetSecCfg(&objMvrCfg, offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg));

		// 向调度屏提示
		char tmpbuf[] = {0};
		char buf[100] = {0};	
		sprintf(tmpbuf, "系统音量：%d", bytVolume);
		buf[0] = 0x01;
		buf[1] = 0x72;
		buf[2] = 0x01;
		memcpy(buf+3, tmpbuf, strlen(tmpbuf));	
		DataPush(buf, strlen(tmpbuf)+3, DEV_DVR, DEV_DIAODU, LV2);
	}	
}

int MakeUsbFrame(BYTE *InBuf, int InLen, BYTE *OutBuf)
{
	int OutLen = 0;
	
	for(int i = 0; i < InLen; i ++)
	{
		if(0x7d != InBuf[i] && 0x7e != InBuf[i])
		{
			OutBuf[OutLen++] = InBuf[i];
		}
		else
		{
			OutBuf[OutLen++] = 0x7d;
			OutBuf[OutLen++] = InBuf[i] - 0x7d + 1;
		}
	}
	OutBuf[OutLen++] = 0x7e;
	
	return OutLen;
}

BOOL InitBuf(int i, int size, BYTE data, BYTE type)
{
	MEM_BUF *MemBuf;
	
	switch(data)
	{
		case 'A':
		{
			if(type=='R')
			{
				g_objAVRecBuf.ABuf[i] = (BYTE*)malloc(size);
				if(g_objAVRecBuf.ABuf[i]==NULL) goto ERROR;
				return TRUE;
			}
			
			MemBuf = &(g_objAVUplBuf.AMemBuf[i]);
		}
			break;
		case 'V':
		{
			if(type=='R')
			{
				g_objAVRecBuf.VBuf[i] = (BYTE*)malloc(size);
				if(g_objAVRecBuf.VBuf[i]==NULL) goto ERROR;
				return TRUE;
			}
			
			MemBuf = &(g_objAVUplBuf.VMemBuf[i]);
		}
			break;
		case 'T':
		{
			MemBuf = &(g_objTTSPlayBuf.TMemBuf);
		}
			break;
		default:
			break;
	}
	
	MemBuf->Buf = (BYTE*)malloc(size);
	if(MemBuf->Buf==NULL) goto ERROR;
	MemBuf->Head = MemBuf->Read = MemBuf->Write = MemBuf->Buf;
	MemBuf->Tail = MemBuf->Head + size;
	MemBuf->BufSize = size;
	MemBuf->SameCircle = TRUE;
	memset(MemBuf->Buf, 0, MemBuf->BufSize);
	pthread_mutex_init(&MemBuf->Mutex, NULL);
	
	return TRUE;

ERROR:
	PRTMSG(MSG_ERR, "malloc buffer failed!\n");
	return FALSE;
}

DWORD ReadBuf(int i, BYTE *buf, DWORD len, BYTE data)
{
	MEM_BUF *MemBuf;
	
	switch(data)
	{
		case 'A':
			MemBuf = &(g_objAVUplBuf.AMemBuf[i]);
			break;
		case 'V':
			MemBuf = &(g_objAVUplBuf.VMemBuf[i]);
			break;
		case 'T':
			MemBuf = &(g_objTTSPlayBuf.TMemBuf);
			break;
		default:
			break;
	}

	pthread_mutex_lock( &MemBuf->Mutex );

	DWORD readlen = len;

	// 若缓冲区中的数据长度小于所要读取的长度，则读取全部的数据；
	DWORD currentlen = MemBuf->SameCircle ? (MemBuf->Write-MemBuf->Read) : (MemBuf->BufSize - (MemBuf->Read-MemBuf->Write));
	if( currentlen < readlen )
	{
		//pthread_mutex_unlock( &MemBuf->Mutex );
		//return 0;

		readlen = currentlen;
	}
	
	// 若读指针已接近缓冲区尾部，则分两段来读取
	if((MemBuf->Tail - MemBuf->Read) < readlen)
	{
		memcpy(buf, MemBuf->Read, MemBuf->Tail-MemBuf->Read);
		memcpy(buf+(MemBuf->Tail-MemBuf->Read), MemBuf->Head, readlen-(MemBuf->Tail-MemBuf->Read));
		MemBuf->Read = MemBuf->Head + readlen - (MemBuf->Tail-MemBuf->Read);
		MemBuf->SameCircle = TRUE;

		pthread_mutex_unlock(&MemBuf->Mutex);
		return len;
	}
	
	// 其他情况则直接读取
	memcpy(buf, MemBuf->Read, readlen);
	MemBuf->Read += readlen;

	pthread_mutex_unlock(&MemBuf->Mutex);
	return readlen;
}

BOOL WriteBuf(int i, BYTE *buf, DWORD len, BYTE data)
{
	MEM_BUF *MemBuf;
	
	switch(data)
	{
		case 'A':
			MemBuf = &(g_objAVUplBuf.AMemBuf[i]);
			break;
		case 'V':
			MemBuf = &(g_objAVUplBuf.VMemBuf[i]);
			break;
		case 'T':
			MemBuf = &(g_objTTSPlayBuf.TMemBuf);
			break;
		default:
			break;
	}
	
	pthread_mutex_lock( &MemBuf->Mutex );
	
	// 清空缓冲区接口
	if(data == 'T')
	{
	  if(buf == NULL && len == 0)
	  {
	  	MemBuf->Read = MemBuf->Write = MemBuf->Head;
			MemBuf->SameCircle = TRUE;
			memset(MemBuf->Buf, 0, MemBuf->BufSize);
			pthread_mutex_unlock( &MemBuf->Mutex );
			return TRUE;
	  }
	}

	// 若缓冲区所剩的长度小于所要写数据段长度，则清空缓冲区
	DWORD leftlen = MemBuf->SameCircle ? (MemBuf->BufSize-(MemBuf->Write-MemBuf->Read)) : (MemBuf->Read-MemBuf->Write);
	if(leftlen < len)
	{
		MemBuf->Read = MemBuf->Write = MemBuf->Head;
		MemBuf->SameCircle = TRUE;

		if(data == 'V')
		{
			// 同时也清空保存的缓冲区
			for(int j=0; j<32; j++)
			{
				for(int k=0; k<8; k++)
					g_objDataUploadSta[k].m_objDataWin[j].m_ulWinNo = MAX_UNSIGNED_64;
			}
		}
	}
	
	// 若写指针已接近缓冲区尾部，则把所要写的数据分成两段来写
	if((MemBuf->Tail - MemBuf->Write) < len)
	{
		memcpy(MemBuf->Write, buf, MemBuf->Tail-MemBuf->Write);
		memcpy(MemBuf->Head, buf + (MemBuf->Tail-MemBuf->Write), len- (MemBuf->Tail-MemBuf->Write) );
		MemBuf->Write = MemBuf->Head + len - (MemBuf->Tail-MemBuf->Write);
		MemBuf->SameCircle = FALSE;
		
		pthread_mutex_unlock( &MemBuf->Mutex );
		return TRUE;
	}
	
	// 其他情况则将数据直接写入
	memcpy(MemBuf->Write,buf,len);
	MemBuf->Write += len;

	pthread_mutex_unlock(&MemBuf->Mutex);
	return TRUE;
}

int SecondCount( int v_iLastTime, int v_iDelayTime )
{
	int iCurTime = time((time_t*)NULL);

	if(iCurTime>=v_iLastTime+v_iDelayTime)
		return 0;
	else
		return -1;
}

//单屏显示
void ShowDiaodu(char *fmt, ...)
{
	char str[1024] = {0x01, 0x72, 0x01};
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(str+3, fmt, ap);
	va_end(ap);

	DataPush(str, strlen(str), DEV_DVR, DEV_DIAODU, LV2);
}

//多屏显示
void ShowDiaoduForScreens(char *fmt, ...)
{
	char str[1024] = {0x01, 0xE9, 0x00};
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(str+3, fmt, ap);
	va_end(ap);
	
	str[2] = strlen(str+3);

	DataPush(str, strlen(str), DEV_DVR, DEV_DIAODU, LV2);
}

//DvrExe线程每秒清一次看门狗
void ClearThreadDog(int v_iThreadId)
{
	static DWORD dwClearDogTick[3] = {0};
	
  if(GetTickCount()-dwClearDogTick[v_iThreadId] >= 1000)
	{
		dwClearDogTick[v_iThreadId] = GetTickCount();
		
		switch(v_iThreadId)
		{
			case 0:
				DogClr(DOG_DVR_T1);
				break;
			case 1:
				DogClr(DOG_DVR_T2);
				break;
			case 2:
				DogClr(DOG_DVR_T3);
				break;
			default:
				break;
		}
	}
}

void RenewMemInfoHalfMin(int v_iId)
{
	static DWORD dwRenewTick[3] = {0};
	
  if(GetTickCount()-dwRenewTick[v_iId] >= 30000)
	{
		dwRenewTick[v_iId] = GetTickCount();
		
		switch(v_iId)
		{
			case 0:
				RenewMemInfo(0x02, 0x02, 0x00, 0x01);
				break;
			case 1:
				RenewMemInfo(0x02, 0x02, 0x00, 0x02);
				break;
			case 2:
				RenewMemInfo(0x02, 0x02, 0x00, 0x03);
				break;
			default:
				break;
		}
	}
}

bool FileWritable(char *v_szFile, DWORD &v_dwTick)
{
	struct stat objState = {0};
	
  if(GetTickCount() - v_dwTick >= 10*60*1000)
	{
		v_dwTick = GetTickCount();
		
		stat(v_szFile, &objState);
		PRTMSG(MSG_DBG, "File %s size = %d\n", v_szFile, objState.st_size);
		if(objState.st_size == 0)
			return false;
	}
	
	return true;
}

