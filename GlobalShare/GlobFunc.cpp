#include <math.h>
#include <unistd.h>
#include "GlobFunc.h"

#undef MSG_HEAD
#define MSG_HEAD ("GlobFunc")

void ListPhoneInfo()
{
	tag2DDvrCfg objMvrCfg;
	
	if(GetSecCfg(&objMvrCfg, sizeof(objMvrCfg), offsetof(tagSecondCfg, m_uni2DDvrCfg.m_obj2DDvrCfg), sizeof(objMvrCfg)) == 0)
	{
		//printf("objMvrCfg.IpPortPara.m_szPhoneMod:%s\n", objMvrCfg.IpPortPara.m_szPhoneMod);

		if( strcmp(objMvrCfg.IpPortPara.m_szPhoneMod, "MC703NEW")==0 )
		{
			NETWORK_TYPE = NETWORK_EVDO;
			PHONE_MODTYPE = PHONE_MODTYPE_MC703NEW;
			
			strcpy(USB_DEV_INSMOD, "insmod /dvs/extdrv/linux_usbserial.ko vendor=0x12d1 product=0x1413");
#if VEHICLE_TYPE == VEHICLE_M
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.3/1-2.3:1.0");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.3/1-2.3:1.3");
#endif
#if VEHICLE_TYPE == VEHICLE_M2
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.2/1-1.2:1.0");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.2/1-1.2:1.3");
#endif
#if VEHICLE_TYPE == VEHICLE_V8
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.0");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.3");
#endif
			strcpy(PPP_DIAL_NUM, "*99#");
			strcpy(PPP_USER_NAME, "CARD");
			strcpy(PPP_PASSWORD, "CARD");
		}
		else if( strcmp(objMvrCfg.IpPortPara.m_szPhoneMod, "MC703OLD")==0 )
		{
			NETWORK_TYPE = NETWORK_EVDO;
			PHONE_MODTYPE = PHONE_MODTYPE_MC703OLD;
			
			strcpy(USB_DEV_INSMOD, "insmod /dvs/extdrv/linux_usbserial.ko vendor=0x12d1 product=0x1001");
#if VEHICLE_TYPE == VEHICLE_M
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.3/1-2.3:1.2");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.3/1-2.3:1.0");
#endif
#if VEHICLE_TYPE == VEHICLE_M2
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.2/1-1.2:1.2");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.2/1-1.2:1.0");
#endif
#if VEHICLE_TYPE == VEHICLE_V8
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.2");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.0");
#endif
			strcpy(PPP_DIAL_NUM, "*99#");
			strcpy(PPP_USER_NAME, "CARD");
			strcpy(PPP_PASSWORD, "CARD");
		}
		else if( strcmp(objMvrCfg.IpPortPara.m_szPhoneMod, "LC6311")==0 )
		{
			NETWORK_TYPE = NETWORK_TD;
			PHONE_MODTYPE = PHONE_MODTYPE_TD_LC6311;
			
			strcpy(USB_DEV_INSMOD, "insmod /dvs/extdrv/linux_usbserial.ko vendor=0x1ab7 product=0x6000");
#if VEHICLE_TYPE == VEHICLE_M
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.3/1-2.3:1.5");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.3/1-2.3:1.0");
#endif
#if VEHICLE_TYPE == VEHICLE_M2
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.2/1-1.2:1.5");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.2/1-1.2:1.0");
#endif
#if VEHICLE_TYPE == VEHICLE_V8
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.5");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.0");
#endif
			strcpy(PPP_DIAL_NUM, "*98*1#");
			strcpy(PPP_USER_NAME, "");
			strcpy(PPP_PASSWORD, "");
		}
		else if( strcmp(objMvrCfg.IpPortPara.m_szPhoneMod, "SIM5218")==0 )
		{
			NETWORK_TYPE = NETWORK_WCDMA;
			PHONE_MODTYPE = PHONE_MODTYPE_SIM5218;
			
			strcpy(USB_DEV_INSMOD, "insmod /dvs/extdrv/linux_usbserial.ko vendor=0x05c6 product=0x9000");
#if VEHICLE_TYPE == VEHICLE_M
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.3/1-2.3:1.2");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.3/1-2.3:1.3");
#endif
#if VEHICLE_TYPE == VEHICLE_M2
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.2/1-1.2:1.2");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.2/1-1.2:1.3");
#endif
#if VEHICLE_TYPE == VEHICLE_V8
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.2");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.3");
#endif
			strcpy(PPP_DIAL_NUM, "*99#");
			strcpy(PPP_USER_NAME, "");
			strcpy(PPP_PASSWORD, "");
		}
		else if( strcmp(objMvrCfg.IpPortPara.m_szPhoneMod, "SIM4222")==0 )
		{
			NETWORK_TYPE = NETWORK_WCDMA;
			PHONE_MODTYPE = PHONE_MODTYPE_SIM4222;
			
			strcpy(USB_DEV_INSMOD, "insmod /dvs/extdrv/linux_usbserial.ko vendor=0x1ab7 product=0x6000");
			
#if VEHICLE_TYPE == VEHICLE_V8
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.5");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.0");
#endif
			strcpy(PPP_DIAL_NUM, "*98*1#");
			strcpy(PPP_USER_NAME, "");
			strcpy(PPP_PASSWORD, "");
		}
		else if( strcmp(objMvrCfg.IpPortPara.m_szPhoneMod, "EM770W")==0 )
		{
			NETWORK_TYPE = NETWORK_WCDMA;
			PHONE_MODTYPE = PHONE_MODTYPE_EM770W;
			
			strcpy(USB_DEV_INSMOD, "insmod /dvs/extdrv/linux_usbserial.ko vendor=0x12d1 product=0x1001");
			
#if VEHICLE_TYPE == VEHICLE_V8
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.2");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.0");
#endif
			strcpy(PPP_DIAL_NUM, "*99#");
			strcpy(PPP_USER_NAME, "");
			strcpy(PPP_PASSWORD, "");
		}
		else if( strcmp(objMvrCfg.IpPortPara.m_szPhoneMod, "MU203")==0 )
		{
			NETWORK_TYPE = NETWORK_WCDMA;
			PHONE_MODTYPE = PHONE_MODTYPE_MU203;
			
			strcpy(USB_DEV_INSMOD, "insmod /dvs/extdrv/linux_usbserial.ko vendor=0x12d1 product=0x1001");
			
#if VEHICLE_TYPE == VEHICLE_V8
			strcpy(USB_DEV_CHECKPATH_AT, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.2");
			strcpy(USB_DEV_CHECKPATH_PPP, "/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.0");
#endif
			strcpy(PPP_DIAL_NUM, "*99#");
			strcpy(PPP_USER_NAME, "");
			strcpy(PPP_PASSWORD, "");
		}
		else if( strcmp(objMvrCfg.IpPortPara.m_szPhoneMod, "BENQ")==0 )
		{
#if VEHICLE_TYPE == VEHICLE_V8
			NETWORK_TYPE = NETWORK_WCDMA;
			PHONE_MODTYPE = PHONE_MODTYPE_BENQ;
			
			strcpy(USB_DEV_INSMOD, "insmod /dvs/extdrv/linux_usbserial.ko vendor=0x12d1 product=0x1001");
			
			COMMUX_DEVPATH_AT = 0x00000010;
			COMMUX_DEVPATH_PPP = 0x00000011;

			strcpy(PPP_DIAL_NUM, "*99***1#");
			strcpy(PPP_USER_NAME, "cmnet");
			strcpy(PPP_PASSWORD, "cmnet");
#endif
		}
	}
}

int DelErrLog()
{
	for(int n = 0; n < 3; n++)
	{
		if(!access("/mnt/Flash/part3/ErrLog", F_OK))
		{
			if(unlink("/mnt/Flash/part3/ErrLog"))
			{
				sleep(1);
				continue;
			}
				
			return 0;
		}
			
		return 0;
	}

	return -1;
}

void RenewMemInfo(BYTE v_arg0, BYTE v_arg1, BYTE v_arg2, BYTE v_arg3)
{
	BYTE buf[128];
	
	buf[0] = v_arg0;
	buf[1] = v_arg1;
	buf[2] = v_arg2;
	buf[3] = v_arg3;
	
	DataPush(buf, 4, DEV_DIAODU, DEV_DIAODU, LV2);
}

void G_RecordDebugInfo(char *fmt, ...)
{
	char str[1024] = {0};

	struct tm pCurrentTime;
	G_GetTime(&pCurrentTime);

	sprintf(str, "%02d-%02d-%02d %02d:%02d:%02d:  ", pCurrentTime.tm_year-100, pCurrentTime.tm_mon+1, pCurrentTime.tm_mday, pCurrentTime.tm_hour, pCurrentTime.tm_min, pCurrentTime.tm_sec);
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(str+20, fmt, ap);
	va_end(ap);
	
	DataPush(str, strlen(str), DEV_QIAN, DEV_IO, LV2);
}

void G_ResetSystem()
{
	DataSkipWait( DEV_DVR | DEV_DIAODU | DEV_PHONE | DEV_SOCK | DEV_UPDATE | DEV_IO | DEV_QIAN);
}

int ChangMtdRW(char * v_szmtd)
{ 
	int fd;
	if((fd = open(v_szmtd, O_RDONLY)) == -1) 
	{
		PRTMSG(MSG_ERR, "open %s failed\n", v_szmtd);
		return 1;
	} 
	else 
	{
		ioctl(fd, MEMWRITE, 1);
		close(fd);
		return 0;
	}
}

int ChangMtdRO(char * v_szmtd)
{ 
	int fd;
	if((fd = open(v_szmtd, O_RDONLY)) == -1) 
	{
		PRTMSG(MSG_ERR, "open %s failed\n", v_szmtd);
		return 1;
	} 
	else 
	{
		ioctl(fd, MEMWRITE, 0);
		close(fd);
		return 0;
	}
}

int EraseMtd(char * v_szmtd, char *v_szfindfile, unsigned int v_uisize)
{ 
	int  ff1, ff2;
	unsigned int len, eraselen;
	struct mtd_info_user	info;
	erase_info_t	erase;
	
	if((ff1 = open(v_szmtd, O_RDWR)) ==  -1) 
	{
		PRTMSG(MSG_ERR, "open %s failed\n", v_szmtd);
		return 1;
	}
	else 
	{
		ioctl(ff1, MEMGETINFO, &info);
		PRTMSG(MSG_NOR, "info.size=%d\n",info.size);
	}
	
	if(v_uisize)//内存升级
	{
		len = v_uisize;
	}
	else//文件升级
	{
		if((ff2 = open(v_szfindfile, O_RDWR)) == -1) 
		{
			perror("open failed\n");
			close(ff2);
			return 1;
		}
		
		len = lseek(ff2, 0, SEEK_END);
		lseek(ff2, 0, SEEK_SET);
		close(ff2);
	}
	
	//每次都擦除整个分区
	len = info.size;
	
	erase.length = info.erasesize;
	//eraselen = (len/info.erasesize+1)*info.erasesize;	
	eraselen = (len/info.erasesize)*info.erasesize;	
	
	if(len>info.size)
		return 1;
	
	PRTMSG(MSG_NOR, "erase %s start\n", v_szmtd);
	
	for(erase.start = 0; erase.start < eraselen; erase.start += info.erasesize) 
	{
		if(ioctl(ff1, MEMERASE, &erase) != 0) 
		{
			PRTMSG(MSG_ERR, "erase %s failed\n", v_szmtd);
			close(ff1);
			return 1;
		}
	}
	
	PRTMSG(MSG_NOR, "erase %s succ\n", v_szmtd);
	close(ff1);
	return 0;
}

int WriteMtd(char * v_szmtd, char *v_szfindfile, unsigned int v_uisize)
{
	int i;
	int iRet;
	unsigned int len;
	int  pfile, tfile, cnt;
	char readbuf[4096];
//	struct  mtd_info_user info;
	
	if((pfile = open(v_szmtd, O_RDWR)) ==  -1) 
	{
		PRTMSG(MSG_ERR, "open %s failed\n", v_szmtd);
		close(pfile);
		return 1;
	} 
	
	if(v_uisize)//内存升级
	{
		len = v_uisize;
		while(len > 4096) 
		{
			memcpy(readbuf, v_szfindfile, 4096);
			
			for(i=0; i<3; i++)
			{
				iRet = write(pfile, readbuf, 4096);
				
				if( -1==iRet || 3==i )
					return 1;
				else if( 4096!=iRet )
					lseek(pfile, -iRet, SEEK_CUR);
				else
					break;
			}
			
			v_szfindfile += 4096;
			len -= 4096;
		}
		
		memcpy(readbuf, v_szfindfile, len);
		
		for(i=0; i<3; i++)
		{
			iRet = write(pfile, readbuf, len);
				
			if( -1==iRet || 3==i )
				return 1;
			else if( (int)len!=iRet )
				lseek(pfile, -iRet, SEEK_CUR);
			else
				break;
		}
	}
	else//文件升级
	{
		if((tfile = open(v_szfindfile , O_RDWR)) == -1) 
		{
			perror("open failed\n");
			close(tfile);
			return 1;
		}
		
		len=lseek(tfile, 0, SEEK_END);
		lseek(tfile, 0, SEEK_SET);
		while(len > 0) 
		{
			cnt = read(tfile, readbuf, 4096);
			
			for(i=0; i<3; i++)
			{
				iRet = write(pfile, readbuf, cnt);
				
				if( -1==iRet || 3==i )
					return 1;
				else if( cnt!=iRet )
					lseek(pfile, -iRet, SEEK_CUR);
				else
					break;
			}
			
			len -= cnt;
		}
		
		close(tfile);
	}
	
	close(pfile);
	
	return 0;
}

int G_GetTime(tm *v_Time)
{
	time_t lCurrentTime;
	struct tm *pCurrentTime;
	int iSecCount;
	
	iSecCount=time(&lCurrentTime);
	pCurrentTime = localtime(&lCurrentTime); 
	
	// UTC时间转成北京时间
	struct tm pTemp;
	memcpy((void*)&pTemp, (void*)pCurrentTime, sizeof(pTemp));
	G_GetLocalTime(&pTemp);

	memcpy((void*)v_Time, (void*)&pTemp, sizeof(pTemp));

	return iSecCount;
}

//获取本地时间（北京时间）（此函数只是起转换作用，传入UTC时间，传出北京时间）
void G_GetLocalTime(tm *v_UtcTime)
{
	bool bCarry = false; // 时间计算时的进位标志
	
	v_UtcTime->tm_hour += 8;
	if( v_UtcTime->tm_hour >= 24 )
	{
		v_UtcTime->tm_hour -= 24;
		bCarry = true;
	}
	
	v_UtcTime->tm_mday += (bCarry ? 1 : 0);
	int nMaxDay = 31;
	if( 1 == v_UtcTime->tm_mon )
	{
		if( 0 == (v_UtcTime->tm_year + 1900)% 400 
			|| (0 == (v_UtcTime->tm_year + 1900) % 4 && 0 != (v_UtcTime->tm_year + 1900) % 100) ) // 闰年
		{
			nMaxDay = 29;
		}
		else nMaxDay = 28;
	}
	else if( v_UtcTime->tm_mon <= 6 )
	{
		if( 1 == v_UtcTime->tm_mon % 2 ) nMaxDay = 30;
	}
	else
	{
		if( 0 == v_UtcTime->tm_mon % 2 ) nMaxDay = 30;
	}
	if( v_UtcTime->tm_mday > nMaxDay )
	{
		v_UtcTime->tm_mday -= nMaxDay;
		bCarry = true;
	}
	else
	{
		bCarry = false;
	}
	
	v_UtcTime->tm_mon += (bCarry ? 1 : 0);
	if( v_UtcTime->tm_mon > 11 )
	{
		v_UtcTime->tm_mon -= 12;
		bCarry = true;
	}
	else
	{
		bCarry = false;
	}
	
	v_UtcTime->tm_year += (bCarry ? 1 : 0);
}

//获取UTC时间（此函数只是起转换作用，传入北京时间，传出UTC时间）
void G_GetUTCTime(tm *v_LocalTime)
{
	int Mon[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if(v_LocalTime->tm_hour >= 8)
	{
		v_LocalTime->tm_hour -= 8;
		return;
	}
	v_LocalTime->tm_hour += 16;
	if(1 !=v_LocalTime->tm_mday)
	{
		v_LocalTime->tm_mday -= 1;
		return;
	}
	if(0 != v_LocalTime->tm_mon)
	{
		v_LocalTime->tm_mon -= 1;
		if(1 != v_LocalTime->tm_mon)
		{
			v_LocalTime->tm_mday = Mon[v_LocalTime->tm_mon];
			return;
		}
		if( 0 == (v_LocalTime->tm_year + 1900)% 400 
			|| (0 == (v_LocalTime->tm_year + 1900) % 4 && 0 != (v_LocalTime->tm_year + 1900) % 100) ) // 闰年
		{
			v_LocalTime->tm_mday = 29;
		}
		else
		{
			v_LocalTime->tm_mday = 28;
		}
		return;
	}
	v_LocalTime->tm_year -= 1;
	v_LocalTime->tm_mon = 11;
	v_LocalTime->tm_mday = 31;
}


bool ThreadExit(void *Event)
{
	if( pthread_kill(*((pthread_t*)Event), 0)==ESRCH )
		return true;
	else
		return false;
}

int WaitForEvent(bool (*v_EventFunc)(void *Event), void *v_Event, uint v_uiWaitTick)
{
	uint uiCurTick = GetTickCount();
	
	do
	{
		if( v_EventFunc ? (v_EventFunc(v_Event)) : *((bool*)v_Event) )//判断事件是否发生
			return 0;
		else
			usleep(10000);
	}while( GetTickCount()-uiCurTick<v_uiWaitTick );

	return 1;//超时
}

void PrintString(char *v_szbuf, int v_ilen)
{
	int i=0;

	for(i=0; i<v_ilen; i++)
		printf(" %02x", (byte)v_szbuf[i]);
	
	printf("\n");
}

// Linux下找不到atoi64函数
long long yx_atoi64(char *v_szbuf)
{
	int i = 0, iLen = 0, iBitValue = 0;
	long long base = 1;
	long long result = 0;
	
	// 找到第一个非数字
	while( v_szbuf[i] <= 0x39 && v_szbuf[i] >= 0x30 )
	{
		i++;
	}
	iLen = i;
	
	// 各个位数乘10累加
	for(i=iLen; i>0; i--)
	{	
		iBitValue = v_szbuf[i-1] - 0x30;

		result += iBitValue * base;

		base = base *10;
	}
	
	return result;
}

int SYSTEM(char *v_szFormat, ...)
{
	char szCmd[1024] = {0};
	va_list va = NULL;
		
	va_start(va, v_szFormat);
	vsprintf(szCmd, v_szFormat, va);
	va_end(va);
	
	return system(szCmd);
}

char *SPRINTF(char *v_szDes, char *v_szFormat, ... )
{
	if(v_szDes)
	{
		va_list va = NULL;
		
		va_start(va, v_szFormat);
		vsprintf(v_szDes, v_szFormat, va);
		va_end(va);
	}
	
	return v_szDes;
}

char *STRCAT(char *v_szDes, int v_iSrcNum, ...)
{
	if(v_szDes)
	{
		char *szSrc = NULL;
		va_list va = NULL;
		
		va_start(va, v_iSrcNum);
		for(int i=0; i<v_iSrcNum; i++)
		{
			if((szSrc = (char *)va_arg(va, char *)))
				strcat(v_szDes, szSrc);
		}
		va_end(va);
	}
	
	return v_szDes;
}

char *STRCPY(char *v_szDes, int v_iSrcNum, ...)
{
	if(v_szDes)
	{
		char *szSrc = NULL;
		va_list va = NULL;
		
		va_start(va, v_iSrcNum);
		if((szSrc = (char *)va_arg(va, char *)))
			strcpy(v_szDes, szSrc);
		for(int i=1; i<v_iSrcNum; i++)
		{
			if((szSrc = (char *)va_arg(va, char *)))
				strcat(v_szDes, szSrc);
		}
		va_end(va);
	}
	
	return v_szDes;
}

int STRCMP(char *v_szDes, char *v_szSrc)
{
	int i, len;
	uint uiWildcard1=0, uiWildcard2=0;
	uint uiStandard = (0xFFFFFFFF>>(32-strlen(v_szDes)));
	
	//判断条件字符串是否含有通配符
	len = strlen(v_szDes);
	for( i=0; i<len; i++ )
	{
		if( v_szDes[i]=='*' )
			uiWildcard1 |= (0x00000001<<i);
		if( v_szDes[i]=='?' )
			uiWildcard2 |= (0x00000001<<i);
	}
	
	//条件字符串全是'*'时直接返回成功
	if( uiWildcard1==uiStandard )
	{
		return 0;
	}
	
	//条件字符串全是'?'时只比较字符串长度
	if( uiWildcard2==uiStandard )
	{
		if( strlen(v_szDes)==strlen(v_szSrc) )
			return 0;
		else
			return -1;
	}
	
	//条件字符串是'?'和'*'混合时直接返回成功
	if( uiWildcard1 && (uiWildcard1|uiWildcard2)==uiStandard )
	{
		return 0;
	}
	
  //否则根据通配符'*'分割条件字符串进行比较
	int iDesLen;
	char szDes[1024];
	char *pSrc, *pDes, *pDesChild, *pDesDelim;
	
	pSrc = v_szSrc;
	strcpy(szDes, v_szDes);
	pDesDelim = szDes;
	
	while( (pDes=pDesChild=strtok(pDesDelim, "*"))!=NULL )//根据通配符分割条件字符串
	{
		pDesDelim = NULL;
		
		iDesLen = strlen(pDes)+1;
		for(i=0; i<iDesLen; i++)
		{
			if( pDes[i]=='?' || pDes[i]==0 ) //如果遇到通配符'?'或者结束符则进行比较
			{
				pDes[i] = 0;
				if( strlen(pDesChild)==0 || (pSrc=strstr(pSrc, pDesChild))!=NULL )
				{
					pSrc += (strlen(pDesChild)+1); //跳过已找到的条件字符
					pDesChild += (strlen(pDesChild)+1);
				}
				else
					return -1;
			}
		}
	}

	return 0;
}

LINUX_HANDLE FindFirstFile(char *v_szPath, char *v_szName, LINUX_FIND_DATA *v_objData, bool v_bRecur)
{
	int i;
	int iAllFileNum = 0;
	LINUX_HANDLE objHandle = {0};
	char szPath[1024] = {0};
	struct dirent **objList;
	struct stat objStat;
	
	iAllFileNum = scandir(v_szPath, &objList, 0, alphasort);	
	
	for(i=0; i<iAllFileNum; i++)
	{
		STRCPY(szPath, 2, v_szPath, objList[i]->d_name);
	
		if( 0 != stat(szPath, &objStat) )
		{
			PRTMSG(MSG_ERR, "stat err\n");
			perror("");
			continue;
		}
		
		if(S_ISREG(objStat.st_mode) || S_ISCHR(objStat.st_mode) || S_ISBLK(objStat.st_mode))//找到一个文件
		{
			if( STRCMP(v_szName, objList[i]->d_name)==0 )// 满足条件
			{
				objHandle.bValid = true;
				strcpy(objHandle.szDirPath, v_szPath);
				strcpy(objHandle.szFileName, objList[i]->d_name);
				strcpy(v_objData->szFileName, objList[i]->d_name);
				memcpy(&(v_objData->objFileStat), &objStat, sizeof(struct stat));
				break;
			}
			
			strcpy(szPath, v_szPath);
		}
		else if(S_ISDIR(objStat.st_mode))//找到一个目录
		{
			if(v_bRecur)//递归查找
			{
				if(strcmp(objList[i]->d_name, ".") && strcmp(objList[i]->d_name, ".."))//忽略这两个目录
				{
					strcat(szPath, "/");
					objHandle = FindFirstFile(szPath, v_szName, v_objData, v_bRecur);
					if(objHandle.bValid)//子目录下找到目标
						break;
				}
			}
			else
			{
				if( STRCMP(v_szName, objList[i]->d_name)==0 )// 满足条件
				{
					objHandle.bValid = true;
					strcpy(objHandle.szDirPath, v_szPath);
					strcpy(objHandle.szFileName, objList[i]->d_name);
					strcpy(v_objData->szFileName, objList[i]->d_name);
					memcpy(&(v_objData->objFileStat), &objStat, sizeof(struct stat));
					break;
				}
			}
			
			strcpy(szPath, v_szPath);
		}
	}
	
	return objHandle;
}

bool FindNextFile(LINUX_HANDLE *v_objHandle, LINUX_FIND_DATA *v_objData, bool v_bRecur)
{
	int i;
	int iAllFileNum = 0;
	char szPath[1024] = {0};
	struct dirent **objList;
	struct stat objStat;
	
	iAllFileNum = scandir(v_objHandle->szDirPath, &objList, 0, alphasort);	

	for(i=0; i<iAllFileNum; i++)
	{
		STRCPY(szPath, 2, v_objHandle->szDirPath, objList[i]->d_name);
	
		if( 0!=stat(szPath, &objStat) )
		{
			PRTMSG(MSG_ERR, "stat err\n");
			perror("");
			continue;
		}
		
		if(S_ISREG(objStat.st_mode) || S_ISCHR(objStat.st_mode) || S_ISBLK(objStat.st_mode))//找到一个文件
		{
			if( STRCMP(v_objHandle->szFileName, objList[i]->d_name)==0 )// 满足条件
			{
				if( i+1<iAllFileNum )// 找到下一个文件
				{
					strcpy(v_objHandle->szFileName, objList[i+1]->d_name);
					strcpy(v_objData->szFileName, objList[i+1]->d_name);
					memcpy(&(v_objData->objFileStat), &objStat, sizeof(struct stat));

					return true;
				}
			}
			
			strcpy(szPath, v_objHandle->szDirPath);
		}
		else if(S_ISDIR(objStat.st_mode))//找到一个目录
		{
			if(v_bRecur)//递归查找
			{
				if(strcmp(objList[i]->d_name, ".") && strcmp(objList[i]->d_name, ".."))//忽略这两个目录
				{
					if( FindNextFile(v_objHandle, v_objData, v_bRecur) )
					{
						return true;
					}
				}
			}
			else
			{
				if( STRCMP(v_objHandle->szFileName, objList[i]->d_name)==0 )// 满足条件
				{
					if( i+1<iAllFileNum )// 找到下一个文件
					{
						strcpy(v_objHandle->szFileName, objList[i+1]->d_name);
						strcpy(v_objData->szFileName, objList[i+1]->d_name);
						memcpy(&(v_objData->objFileStat), &objStat, sizeof(struct stat));
						
						return true;
					}
				}
			}
			
			strcpy(szPath, v_objHandle->szDirPath);
		}
	}
	
	return false;
}

int FileCopy(char *pSourFilePath, char *pTarFilePath)                                                                   
{
	struct stat objSourFileStat;
	int iSourFile, iTarFile;
	int iBytesRead, iBytesWrite, iBytesCopy;	
	char szBuffer[128*1024], *p_szBuffer;	
	
	iSourFile = open(pSourFilePath, O_RDONLY);
	if(iSourFile == -1)
	{
		PRTMSG(MSG_ERR, "open sour file failed\n");
		return 0;
	}
	
	iTarFile = open(pTarFilePath, O_RDWR|O_CREAT);
	if(iTarFile == -1)
	{
		PRTMSG(MSG_ERR, "open tar file failed\n");
		return 0;
	}
	
	iBytesRead = iBytesWrite = iBytesCopy = 0;
	
	while((iBytesRead = read(iSourFile, &szBuffer, sizeof(szBuffer))) != 0)
	{		
		if((iBytesRead == -1) && (errno != EINTR))
			break;
		else if(iBytesRead > 0)
		{
			p_szBuffer = szBuffer;
			while((iBytesWrite = write(iTarFile, p_szBuffer, iBytesRead)) != 0)
			{
				if((iBytesWrite == -1) && (errno != EINTR))
					break;
				else if(iBytesWrite == iBytesRead)
					break;
				else if(iBytesWrite > 0)
				{
					p_szBuffer += iBytesWrite;
					iBytesRead -= iBytesWrite;
				}
			}
			if(iBytesWrite == -1)
				break;
			iBytesCopy += iBytesRead;            	
		}
	}
	
	fsync(iTarFile);
	
	fstat(iSourFile, &objSourFileStat);
	chown(pTarFilePath, objSourFileStat.st_uid, objSourFileStat.st_gid);
	chmod(pTarFilePath, objSourFileStat.st_mode);
	
	close(iSourFile);
	close(iTarFile);
	
	return iBytesCopy;/*正常拷贝时返回拷贝的字节数*/
}

// int FileCopy(char *pSourFilePath, char *pTarFilePath)                                                                   
// {
// 	struct stat objSourFileStat;
// 	FILE *fpSourFile = NULL, *fpTarFile = NULL;
// 	int iBytesRead, iBytesWrite, iBytesCopy;	
// 	char szBuf[2048];	
// 	
// 	fpSourFile = fopen(pSourFilePath, "rb");
// 	if(fpSourFile == NULL)
// 	{
// 		PRTMSG(MSG_ERR, "open sour file failed\n");
// 		return 0;
// 	}
// 	
// 	fpTarFile = fopen(pTarFilePath, "wb+");
// 	if(fpTarFile == NULL)
// 	{
// 		PRTMSG(MSG_ERR, "open tar file failed\n");
// 		return 0;
// 	}
// 	
// 	iBytesRead = iBytesWrite = iBytesCopy = 0;
// 	
// 	while((iBytesRead = fread(szBuf, 1, 2048, fpSourFile)) != 0)
// 	{		
// 		if((iBytesRead == -1) && (errno != EINTR))
// 			break;
// 		else if(iBytesRead > 0)
// 		{
// 			iBytesWrite = fwrite(szBuf, 1, iBytesRead, fpTarFile);			
// 			iBytesCopy += iBytesRead;            	
// 		}
// 	}
// 	
// 	fflush(fpTarFile);
// 	
// 	fclose(fpSourFile);
// 	fclose(fpTarFile);
// 	
// 	return iBytesCopy;/*正常拷贝时返回拷贝的字节数*/
// }

int DirCopy(char *pSourDirPath, char *pTarDirPath)
{
	int iBytesCopy;
	char szSourDirPath[255], szTarDirPath[255];
	struct stat objSourDirStat;
	struct dirent *p_objNameList;
	DIR *pSourDir, *pTarDir;
	
	if(stat(pSourDirPath, &objSourDirStat) != 0)
	{
		PRTMSG(MSG_ERR, "get the sour dir stat failed\n");
		return 0;
	}
	
	if(access(pTarDirPath, F_OK) != 0)   
	{
		mkdir(pTarDirPath, objSourDirStat.st_mode);
		chown(pTarDirPath, objSourDirStat.st_uid, objSourDirStat.st_gid);
	}
	else
	{
		chmod(pTarDirPath, objSourDirStat.st_mode);
		chown(pTarDirPath, objSourDirStat.st_uid, objSourDirStat.st_gid);
	}
	
	pSourDir = opendir(pSourDirPath);
	pTarDir = opendir(pTarDirPath);
	
	strcpy(szSourDirPath, pSourDirPath);
	strcpy(szTarDirPath, pTarDirPath);
	while((p_objNameList = readdir(pSourDir)) != NULL)
	{
		/*忽略"."及".."目录*/
		if(strcmp(p_objNameList->d_name, ".") != 0 && strcmp(p_objNameList->d_name, "..") != 0)
		{
			strcat(szSourDirPath, "/");
			strcat(szSourDirPath, p_objNameList->d_name);
			strcat(szTarDirPath, "/");
			strcat(szTarDirPath, p_objNameList->d_name);
			stat(szSourDirPath, &objSourDirStat);
			
			if(S_ISLNK(objSourDirStat.st_mode))
			{
				PRTMSG(MSG_ERR, "this is a link file\n");
			}
			else if(S_ISREG(objSourDirStat.st_mode) || S_ISCHR(objSourDirStat.st_mode) || S_ISBLK(objSourDirStat.st_mode))
			{
				if(access(szTarDirPath, F_OK) != 0) 
				{
					iBytesCopy = FileCopy(szSourDirPath, szTarDirPath);
					if(iBytesCopy == -1)
					{
						closedir(pSourDir);
						closedir(pTarDir);
						unlink(szTarDirPath);
						return -1;
					}
				}
				strcpy(szSourDirPath, pSourDirPath);
				strcpy(szTarDirPath, pTarDirPath);
			}
			else if(S_ISDIR(objSourDirStat.st_mode))
			{
				iBytesCopy = DirCopy(szSourDirPath, szTarDirPath);
				if(iBytesCopy == -1)
				{
					closedir(pSourDir);
					closedir(pTarDir);
					return -1;
				}
				strcpy(szSourDirPath, pSourDirPath);
				strcpy(szTarDirPath, pTarDirPath);
			}
		}
	}
	
	closedir(pSourDir);
	closedir(pTarDir);
	return 1;
}

int G_AppUpdate(char *v_szSrc, char *v_szDes, int v_iFlag)
{
	int  iResult = -1;
	unsigned int  iSrcFileSize = 0;
	char *szBuf = NULL;
	char szTempFile[64] = {0};
	FILE *fpSrc = NULL;
	FILE *fpDes = NULL;

	if( access(v_szSrc, F_OK) != 0 )
	{
		PRTMSG(MSG_ERR, "There is no %s\n", v_szSrc);
		goto _CP_END;
	}

	if( v_iFlag==0 )//Memory升级
	{
		// 打开文件
		if( (fpSrc=fopen(v_szSrc, "r+"))==NULL )
		{
			PRTMSG(MSG_ERR, "Open %s fail\n", v_szSrc);
			goto _CP_END;
		}
	
		// 计算文件大小
		fseek(fpSrc, 0, SEEK_END);
		iSrcFileSize = ftell(fpSrc);
	
		// 分配缓冲
		if( (szBuf=(char*)malloc(iSrcFileSize))==NULL )
		{
			PRTMSG(MSG_ERR, "Malloc buf fail\n");
			goto _CP_END;
		}
	
		// 读写文件数据
		fseek(fpSrc, 0, SEEK_SET);
		if( fread(szBuf, 1, iSrcFileSize, fpSrc)!=iSrcFileSize )
		{
			PRTMSG(MSG_ERR, "Read %s fail\n", v_szSrc);
			goto _CP_END;
		}

		if( unlink(v_szDes)==-1 )
		{
			PRTMSG(MSG_ERR, "Delete %s fail\n", v_szDes);
		}
			
		if( (fpDes=fopen(v_szDes, "w+"))==NULL )
		{
			PRTMSG(MSG_ERR, "Open %s fail\n", v_szDes);
			perror("fopen:");
			goto _CP_END;
		}
			
		if( fwrite(szBuf, 1, iSrcFileSize, fpDes)!=iSrcFileSize )
		{
			PRTMSG(MSG_ERR, "Write %s fail\n", v_szDes);
			goto _CP_END;
		}
			
		if( SYSTEM("chmod 777 %s", v_szDes)!=0 )
		{
			PRTMSG(MSG_ERR, "Chmod %s fail\n", v_szDes);
			goto _CP_END;
		}
	}
	else if( v_iFlag==1 )//Flash升级
	{
		sprintf(szTempFile, "/mnt/%s/UpdateFile", LOCAL_SAVE );
		
		if( SYSTEM("cp -f %s %s", v_szSrc, szTempFile)!=0 )
		{
			PRTMSG(MSG_ERR, "Copy %s fail\n", v_szSrc);
			goto _CP_END;
		}
		
		if( unlink(v_szDes)==-1 )
		{
			PRTMSG(MSG_ERR, "Delete %s fail\n", v_szDes);
		}
			
		if( SYSTEM("cp -f %s %s", szTempFile, v_szDes)!=0 )
		{
			PRTMSG(MSG_ERR, "Copy %s fail\n", v_szDes);
			goto _CP_END;
		}
			
		if( SYSTEM("chmod 777 %s", v_szDes)!=0 )
		{
			PRTMSG(MSG_ERR, "Chmod %s fail\n", v_szDes);
			goto _CP_END;
		}
	}
	
	iResult = 0;

_CP_END:
	if(strlen(szTempFile))
	{
		unlink(szTempFile);
	}

	if(szBuf) 
	{	
		free(szBuf);
		szBuf = NULL;
	}	
	
	if(fpSrc)	
	{
		fclose(fpSrc);
		fpSrc = NULL;
	}
	
	if(fpDes)
	{
		fclose(fpDes);
		fpDes = NULL;
	}
		
	if(iResult)
		printf("Update %s failed\n", v_szDes);

	return iResult;
}

int G_SysUpdate(char *v_szSrc, char *v_szDes, int v_iFlag)
{
	int  iResult = -1;
	unsigned int  iSrcFileSize = 0;
	char *szBuf = NULL;
	char szTempFile[64] = {0};
	FILE *fpSrc = NULL;
	FILE *fpDes = NULL;

	if( access(v_szSrc, F_OK) != 0 )
	{
		PRTMSG(MSG_ERR, "There is no %s\n", v_szSrc);
		goto _CP_END;
	}

	if( v_iFlag==0 )//Memory升级
	{
		// 打开文件
		if( (fpSrc=fopen(v_szSrc, "r+"))==NULL )
		{
			PRTMSG(MSG_ERR, "Open %s fail\n", v_szSrc);
			goto _CP_END;
		}
	
		// 计算文件大小
		fseek(fpSrc, 0, SEEK_END);
		iSrcFileSize = ftell(fpSrc);
	
		// 分配缓冲
		if( (szBuf=(char*)malloc(iSrcFileSize))==NULL )
		{
			PRTMSG(MSG_ERR, "Malloc buf fail\n");
			goto _CP_END;
		}
	
		// 读写文件数据
		fseek(fpSrc, 0, SEEK_SET);
		if( fread(szBuf, 1, iSrcFileSize, fpSrc)!=iSrcFileSize )
		{
			PRTMSG(MSG_ERR, "Read %s fail\n", v_szSrc);
			goto _CP_END;
		}
	
		// 若是升级root，则通知QianExe停止所有对Flash的操作，通知GPS进程关闭
		if( strcmp(v_szDes, ROOT_PART)==0 )
		{
			char buf[] = {0xfe} ;
			DataPush(buf, 1, DEV_DIAODU, DEV_QIAN, LV2);

			//g_objGpsSrc.m_bUpdateQuit = true;
		}

		if( ChangMtdRW(v_szDes) )
		{
			PRTMSG(MSG_ERR, "Open %s fail\n", v_szDes);
			goto _CP_END;
		}
			
		if( EraseMtd(v_szDes, szBuf, iSrcFileSize) )
		{
			PRTMSG(MSG_ERR, "Erase %s fail\n", v_szDes);
			goto _CP_END;
		}
			
		if( WriteMtd(v_szDes, szBuf, iSrcFileSize) )
		{
			PRTMSG(MSG_ERR, "Write %s fail\n", v_szDes);
			goto _CP_END;
		}
			
//		if( ChangMtdRO(v_szDes) )
//		{
//			PRTMSG(MSG_ERR, "Close %s fail\n", v_szDes);
//			goto _CP_END;
//		}
	}
	else if( v_iFlag==1 )//Flash升级
	{
		sprintf(szTempFile, "/mnt/%s/UpdateFile", LOCAL_SAVE );
		
		if( SYSTEM("cp -f %s %s", v_szSrc, szTempFile)!=0 )
		{
			PRTMSG(MSG_ERR, "Copy %s fail\n", v_szSrc);
			goto _CP_END;
		}
		
		if( ChangMtdRW(v_szDes) )
		{
			PRTMSG(MSG_ERR, "Open %s fail\n", v_szDes);
			goto _CP_END;
		}

		if( EraseMtd(v_szDes, szTempFile, 0) )
		{
			PRTMSG(MSG_ERR, "Erase %s fail\n", v_szDes);
			goto _CP_END;
		}
			
		if( WriteMtd(v_szDes, szTempFile, 0) )
		{
			PRTMSG(MSG_ERR, "Write %s fail\n", v_szDes);
			goto _CP_END;
		}
			
//		if( ChangMtdRO(v_szDes) )
//		{
//			PRTMSG(MSG_ERR, "Close %s fail\n", v_szDes);
//			goto _CP_END;
//		}
	}
	
	iResult = 0;

_CP_END:
	if(strlen(szTempFile))
	{
		unlink(szTempFile);
	}

	if(szBuf) 
	{	
		free(szBuf);
		szBuf = NULL;
	}	
	
	if(fpSrc)	
	{
		fclose(fpSrc);
		fpSrc = NULL;
	}
	
	if(fpDes)
	{
		fclose(fpDes);
		fpDes = NULL;
	}
		
	if(iResult)
		printf("Update %s failed\n", v_szDes);

	return iResult;
}

bool AppIntact(char *v_szFileName)
{
	int i;
	FILE *fpSrc = NULL;
	
	DWORD dwSrcChkSum = 0, dwChkSum = 0;
	char cSrcFileData;
	int iSrcFileSize = 0;
				
	// 进行解包处理, 压缩包格式：文件个数(1)＋【文件目录(40) + 文件名称(40) + 文件长度(4) + 文件数据(n)】* N + 校验和(4)
	if((fpSrc = fopen(v_szFileName, "rb+")) == NULL)
	{
		PRTMSG(MSG_ERR, "Open Updfile Fail\n");
		goto APP_DAMAGE;
	}

	fseek(fpSrc, 0, SEEK_END);
	iSrcFileSize = ftell(fpSrc);
	fseek(fpSrc, 0, SEEK_SET);
				
	for(i = 0; i < iSrcFileSize - 4; i++)
	{
		if(1 != fread((void*)&cSrcFileData, 1, 1, fpSrc))
		{
			PRTMSG(MSG_ERR, "Read Updfile Data Fail\n");
			goto APP_DAMAGE;
		}
					
		dwChkSum += (BYTE)cSrcFileData;
	}
				
	if(4 != fread((void*)&dwSrcChkSum, 1, 4, fpSrc))
	{
		PRTMSG(MSG_ERR, "Read Updfile ChkSum Fail\n");
		goto APP_DAMAGE;
	}
	
	PRTMSG(MSG_NOR, "ChkSum = %ld, SrcChkSum = %ld\n", dwChkSum, dwSrcChkSum);
	
	if(dwChkSum != dwSrcChkSum)
	{
		PRTMSG(MSG_ERR, "Updfile be changed\n");
		goto APP_DAMAGE;
	}

	if(fpSrc)
	{
		fclose(fpSrc);
		fpSrc = NULL;
	}
	
	return true;

APP_DAMAGE:
	if(fpSrc)
	{
		fclose(fpSrc);
		fpSrc = NULL;
	}
	
	return false;
}

bool G_ExeUpdate(char *v_szFileName, int v_iFlag)
{
	byte bytFileCnt = 0;
	bool bUpdateSucc = false;	// 缺省为升级失败
	
	FILE *fpSrc = NULL;
	FILE *fpDes = NULL;
	size_t iFileSize = 0;
	char *szBuf = NULL;
	char szDesFileToDir[40] = {0};
	char szDesFileToName[40] = {0};
	char szDesFileFromPath[80] = {0};
	char szDesFileToPath[80] = {0};

	if( (fpSrc = fopen(v_szFileName, "rb+"))==NULL )
	{
		PRTMSG(MSG_ERR, "Open updfile failed:");
		perror(" ");
		goto _UPDATE_END;
	}
	
	// 进行解包处理，压缩包格式：文件个数(1)＋【文件目录(40) + 文件名称(40) + 文件长度(4) + 文件数据(n)】* N + 校验和(4)
	if( 1 != fread((void*)&bytFileCnt, 1, 1, fpSrc) )	// 文件个数
		goto _UPDATE_END;
			
	for(int i = 0; i < bytFileCnt; i++)
	{
		if( 40 != fread(szDesFileToDir, 1, 40, fpSrc) || 40 != fread(szDesFileToName, 1, 40, fpSrc) || 4 != fread((void*)&iFileSize, 1, 4, fpSrc) )
			goto _UPDATE_END;
				
		if( NULL == (szBuf = (char*)malloc(iFileSize)) )
		{
			PRTMSG(MSG_ERR, "Malloc buffer failed");
			goto _UPDATE_END;
		}
				
		if( iFileSize != fread(szBuf, 1, iFileSize, fpSrc) )
		{
			PRTMSG(MSG_ERR, "Read updfile failed");
			goto _UPDATE_END;
		}
				
		sprintf(szDesFileFromPath, "/mnt/%s/UpdateFile", LOCAL_SAVE);
		sprintf(szDesFileToPath, "%s%s", szDesFileToDir, szDesFileToName);
				
		if( v_iFlag==0 )//Memory升级
		{
			if( unlink(szDesFileToPath)==-1 )
			{
				PRTMSG(MSG_ERR, "Delete %s fail\n", szDesFileToPath);
			}
			
			if( (fpDes = fopen(szDesFileToPath, "wb+"))==NULL )
			{
				PRTMSG(MSG_ERR, "Open %s failed", szDesFileToPath);
				goto _UPDATE_END;
			}
					
			if( iFileSize != fwrite(szBuf, 1, iFileSize, fpDes) )
			{
				PRTMSG(MSG_ERR, "Write %s failed", szDesFileToPath);
				goto _UPDATE_END;
			}
					
			if(szBuf)
			{
				free(szBuf);
				szBuf = NULL;
			}
					
			if(fpDes)
			{
				fclose(fpDes);
				fpDes = NULL;
			}
		}
		else if( v_iFlag==1 )//Flash升级
		{
			if( (fpDes=fopen(szDesFileFromPath, "wb+"))==NULL )
			{
				PRTMSG(MSG_ERR, "Open %s failed", szDesFileFromPath);
				goto _UPDATE_END;
			}
					
			if( iFileSize != fwrite(szBuf, 1, iFileSize, fpDes) )
			{
				PRTMSG(MSG_ERR, "Write %s failed", szDesFileFromPath);
				goto _UPDATE_END;
			}
					
			if(szBuf)
			{
				free(szBuf);
				szBuf = NULL;
			}
					
			if(fpDes)
			{
				fclose(fpDes);
				fpDes = NULL;
			}
					
			if( unlink(szDesFileToPath)==-1 )
			{
				PRTMSG(MSG_ERR, "Delete %s fail\n", szDesFileToPath);
			}
					
			// 替换原文件
			if( SYSTEM("cp -f %s %s", szDesFileFromPath, szDesFileToPath)!=0 )
			{
				PRTMSG(MSG_ERR, "Copy %s failed", szDesFileToPath);
				goto _UPDATE_END;
			}
		}
				
		// 更改权限
		if( chmod(szDesFileToPath, S_IRWXU|S_IRWXG|S_IRWXO) )
		{
			PRTMSG(MSG_ERR, "Chmod %s failed", szDesFileToPath);
			goto _UPDATE_END;
		}

		if( v_iFlag==1 && unlink(szDesFileFromPath)==-1 )	
		{
			PRTMSG(MSG_ERR, "Delete %s fail\n", szDesFileFromPath);
		}
	}
	
	bUpdateSucc = true;
	
_UPDATE_END:
	if(szBuf)
	{
		free(szBuf);
		szBuf = NULL;
	}
	
	if(fpSrc)
	{
		fclose(fpSrc);
		fpSrc = NULL;
	}
	
	if(fpDes)
	{
		fclose(fpDes);
		fpDes = NULL;
	}
	
	//unlink(szDesFileFromPath);
	
	return bUpdateSucc;
}

// 本函数仅用于升级32M的image.bin文件
bool G_ImageUpdate(char *v_szFileName, int v_iFlag)
{
	bool bUpdateSucc = false;	// 缺省为升级失败
	
	FILE *fpSrc = NULL;
	size_t iFileSize[7] = {1*1024*1024, 2*1024*1024, 13*1024*1024, 5*1024*1024, 3*1024*1024, 7*1024*1024, 1*1024*1024};
	char *szBuf = NULL;
	char szDesFileToPath[80] = {0};

	if( (fpSrc = fopen(v_szFileName, "rb+"))==NULL )
	{
		PRTMSG(MSG_ERR, "Open updfile failed:");
		perror(" ");
		goto _UPDATE_END;
	}

	for(int i = 0; i < 7; i++)
	{
		if( NULL == (szBuf = (char*)malloc(iFileSize[i])) )
		{
			PRTMSG(MSG_ERR, "Malloc buffer failed");
			goto _UPDATE_END;
		}
				
		if( iFileSize[i] != fread(szBuf, 1, iFileSize[i], fpSrc) )
		{
			PRTMSG(MSG_ERR, "Read updfile failed");
			goto _UPDATE_END;
		}

		sprintf(szDesFileToPath, "/dev/mtd/%d", i);
				
		if( ChangMtdRW(szDesFileToPath) )
		{
			PRTMSG(MSG_ERR, "Open %s fail\n", szDesFileToPath);
			goto _UPDATE_END;
		}
				
		if( EraseMtd(szDesFileToPath, szBuf, iFileSize[i]) )
		{
			PRTMSG(MSG_ERR, "Erase %s fail\n", szDesFileToPath);
			goto _UPDATE_END;
		}
				
		if( WriteMtd(szDesFileToPath, szBuf, iFileSize[i]) )
		{
			PRTMSG(MSG_ERR, "Write %s fail\n", szDesFileToPath);
			goto _UPDATE_END;
		}
					
		if(szBuf)
		{
			free(szBuf);
			szBuf = NULL;
		}
	}
	
	bUpdateSucc = true;
	
_UPDATE_END:
	if(szBuf)
	{
		free(szBuf);
		szBuf = NULL;
	}
	
	if(fpSrc)
	{
		fclose(fpSrc);
		fpSrc = NULL;
	}
	
	return bUpdateSucc;
}

long G_Lonlat_4CharToL( char* v_szLonlat )
{
	return long( ( v_szLonlat[0] + (v_szLonlat[1] + v_szLonlat[2] / 100.0 + v_szLonlat[3] / 10000.0) / 60.0 ) * 1000000.0 );
}

double G_Lonlat_4CharToD( char* v_szLonlat )
{
	return v_szLonlat[0] + (v_szLonlat[1] + v_szLonlat[2] / 100.0 + v_szLonlat[3] / 10000.0) / 60.0;
}

// 注意,输入的v_szLonlat,必须是4字节的
void G_Lonlat_DTo4Char( double v_dLonlat, char* v_szLonlat )
{
	v_szLonlat[ 0 ] = BYTE( v_dLonlat );
	double dFen = (v_dLonlat - v_szLonlat[0]) * 60;
	v_szLonlat[ 1 ] = BYTE( dFen );
	v_szLonlat[ 2 ] = BYTE( (dFen - v_szLonlat[1]) * 100 );
	v_szLonlat[ 3 ] = BYTE( WORD((dFen - v_szLonlat[1]) * 10000) % 100 );
}

//------------------------------------------------------------------------------
// 7<->6 转换用的编码映射表
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
char	MapTable_7To6[64]={   
	0x2e,0x2c,0x30,0x31,0x32,0x33,0x34,0x35,					
	0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,					
	0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,						
	0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,						
	0x55,0x56,0x57,0x58,0x59,0x5A,0x61,0x62,						
	0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,						
	0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,						
	0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A 
};
#endif

#if 2 == USE_PROTOCOL
char	MapTable_7To6[64]={ 
	0x30, 0x38, 0x47, 0x4F, 0x57, 0x65, 0x6D, 0x75,
	0x31, 0x39, 0x48, 0x50, 0x58, 0x66, 0x6E, 0x76,
	0x32, 0x41, 0x49, 0x51, 0x59, 0x67, 0x6F, 0x77,
	0x33, 0x42, 0x4A, 0x52, 0x5A, 0x68, 0x70, 0x78,
	0x34, 0x43, 0x4B, 0x53, 0x61, 0x69, 0x71, 0x79,
	0x35, 0x44, 0x4C, 0x54, 0x62, 0x6A, 0x72, 0x7A,
	0x36, 0x45, 0x4D, 0x55, 0x63, 0x6B, 0x73, 0x2C,
	0x37, 0x46, 0x4E, 0x56, 0x64, 0x6C, 0x74, 0x2E
};
#endif
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// 8<->6 转换用的编码映射表
char	MapTable_8To6[64]={   
	0x2e,0x2c,0x30,0x31,0x32,0x33,0x34,0x35,					
	0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,					
	0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,						
	0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,						
	0x55,0x56,0x57,0x58,0x59,0x5A,0x61,0x62,						
	0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,						
	0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,						
	0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A 
};
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// DWORD G_GetCurrentDictory( DWORD nBufferLength, LPTSTR lpBuffer)
// {
// 	if( nBufferLength <= 1 ) return 0;
// 	
// 	DWORD dwLength = ::GetModuleFileName( NULL, lpBuffer, nBufferLength );
// 	if( 0 == dwLength )
// 	{
// 		*lpBuffer = 0;
// 		return 0;
// 	}
// 
// 	TCHAR tc[] = _T("\\");
// 	
// 	TCHAR* pPos = _tcsrchr( lpBuffer, tc[0] );
// 	if( !pPos )
// 	{
// 		*lpBuffer = 0;
// 		return 0;
// 	}
// 
// 	dwLength = pPos - lpBuffer + 1;
// 	if( dwLength >= nBufferLength )
// 	{
// 		*lpBuffer = 0;
// 		return 0;
// 	}
// 	*(pPos+1) = 0;
// 	
// 	return dwLength;
// }

//------------------------------------------------------------------------------
char Encode(char data, char* v_pTable)
{
	if( data < 0x40 ) return v_pTable[data & 0x3f];
	else return 1;
}


//------------------------------------------------------------------------------
DWORD Code7To6( char *v_szDesBuf, char *v_szSrcBuf, DWORD v_dwSrcLen, DWORD v_dwDesSize )
{
	if( v_dwSrcLen * 8 / 6  + 1 > v_dwDesSize ) return 0;

	if ( 0 == v_dwSrcLen ) return 0;
	
		const BYTE mask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

    DWORD  i, j, stemp;
    DWORD rlen;
		char dtemp;
    
    rlen  = 0;
    i     = 0;
    j     = 6;
    dtemp = 0;
    for (;;)
		{
        if (i == 0) 
				{
            if (v_dwSrcLen == 0)
						{
                if (rlen >= v_dwDesSize) 
								{
                    rlen = 0;
                }
								else
								{
                    *v_szDesBuf = Encode(dtemp, MapTable_7To6);
                    rlen++;
                }
                break;
            }

            i = 7;
            stemp = *v_szSrcBuf++;
            v_dwSrcLen--;
        }
		
        if (j == 0)
				{
            if (rlen >= v_dwDesSize) 
						{
                rlen = 0;
                break;
            }

            j = 6;
            *v_szDesBuf++ = Encode(dtemp, MapTable_7To6);
            dtemp   = 0;
            rlen++;
        }

        if (stemp & mask[i - 1]) dtemp |= mask[j - 1];
        i--;
        j--;
    }

    return rlen;
}

//------------------------------------------------------------------------------
DWORD Code8To6( char *v_szDesBuf, char *v_szSrcBuf, DWORD v_dwSrcLen, DWORD v_dwDesSize )
{
	if( v_dwSrcLen * 8 / 6  + 1 > v_dwDesSize ) return 0;

	if ( 0 == v_dwSrcLen ) return 0;
	
	const BYTE mask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

    DWORD  i, j, stemp;
    DWORD rlen;
	char dtemp;
    
    rlen  = 0;
    i     = 0;
    j     = 6;
    dtemp = 0;
    for (;;) {
        if (i == 0) {
            if (v_dwSrcLen == 0) {
                if (rlen >= v_dwDesSize) {
                    rlen = 0;
                } else {
                    *v_szDesBuf = Encode(dtemp, MapTable_8To6);
                    rlen++;
                }
                break;
            }
            i = 8;
            stemp = *v_szSrcBuf++;
            v_dwSrcLen--;
        }
        if (j == 0) {
            if (rlen >= v_dwDesSize) {
                rlen = 0;
                break;
            }
            j = 6;
            *v_szDesBuf++ = Encode(dtemp, MapTable_8To6);
            dtemp   = 0;
            rlen++;
        }
        if (stemp & mask[i - 1]) dtemp |= mask[j - 1];
        i--;
        j--;
    }
    return rlen;
}


//------------------------------------------------------------------------------
int Code8To7( const char* szOldBuf, char* szNewBuf, const int iOldLen, const int iNewSize)
{
	if( iOldLen > 500 || iOldLen < 1 ) return 0;
	if( iNewSize < iOldLen * 8 / 7 + 1 ) return 0;

	const int DESBITWIDTH = 7;
	const char cMask = 0xff >> 8-DESBITWIDTH; // 逻辑右移
	int iDesCt = 0;
	int iSrcCt = 0;
	int iLeave = 0; // 上一次留下几个bit没有使用
	int iCur = 0; // 本次还需要几个bit
	int iRight = 0; // 本次需要的bit,需要原始字节逻辑右移几位
	int iLeft = 0; // 本次的剩余bit,需要原始字节逻辑左移几位
	char cLast = 0; // 上一次留下没有使用的字节 (从最高位的低1位开始填入,不足的后面填0)
	memset( szNewBuf, 0, iNewSize );
	for( iSrcCt = 0; iSrcCt < iOldLen || iLeave > 0; )
	{
		// 先将上次遗留的bits填入
		if( iLeave < 0 || iLeave > DESBITWIDTH ) return 0; // 保险判断
		else if( iLeave > 0 ) szNewBuf[ iDesCt ] = cLast; // 当确实有上次遗留的bits时

		// 加入本次还需要的bits
		iCur = DESBITWIDTH - iLeave; // 本次还需要的bit数
		if( iCur < 0 ) return 0; // 保险起见 (iCur肯定<=7,不用判断)
		else if( iCur > 0 && iSrcCt < iOldLen ) // 当确实需要本次的bit时,且还有本次的bit时
		{
			iRight = 8 - iCur; // 本次需要原始字节逻辑右移的位数
			szNewBuf[ iDesCt ] |= BYTE(szOldBuf[iSrcCt]) >> iRight;
			if( iRight < 0 ) return 0; // 保险起见 (iRight肯定<8,不用判断)
		}
		else iRight = 0;

		// 递增转换后的计数
		++ iDesCt;

		// 本次遗留的bit数
		iLeave = iRight; // 即等于右移的位数

		// 本次遗留的字节 (从最高位的低1位开始填入,不足的后面填0)
		if( iLeave > 0 )
		{
			iLeft = DESBITWIDTH - iLeave; // 需要原始字节左移的位数 (从最高位的低1位开始填入,不足的后面填0)
			if( iLeft < 0 ) return 0; // 保险起见 (iLeft肯定<7,不用判断)
			cLast = BYTE(szOldBuf[iSrcCt]) << iLeft & cMask;
		}

		// 累加已处理完毕的源字串计数
		if( iLeave < DESBITWIDTH ) ++ iSrcCt; // 当前原始字节遗留的位数不足7位时
	}

	return iDesCt;
}


//------------------------------------------------------------------------------
int Code6To8( const char* szOldBuf, char* szNewBuf, const int iOldLen, const int iNewSize )
{
	int		i=0;	//待转化的字符计数器
	int		k=0;	//已转化的字符下标
	int		l=8;	//要需要的比特数
	int		iNewLen = 0;
	byte	strOldBuf1[500];

	if( (ulong)iOldLen>500 || iOldLen <= 0 ) return 0;
	if( iNewSize < iOldLen * 8 / 6 + 1 ) return 0;

	//查表转换
	memcpy( strOldBuf1, szOldBuf, iOldLen);
	for( i=0; i<iOldLen; i++ )
	{
		for( l=0; l<64; l++ )
		{
			if( strOldBuf1[i]==MapTable_8To6[l] )
			{
				strOldBuf1[i] = l;
				break;
			}
		}
	}
		
	l=8;k=0;
	for(i=0;i<iOldLen;i++)
	{
		szNewBuf[i]=0;
		strOldBuf1[i]=((unsigned char)strOldBuf1[i] & 0x3f) << 2;
	}
	for(i=0;i<iOldLen;i++)
	{
		if(l<6)
		{
			szNewBuf[k]=(unsigned char)szNewBuf[k] | ((unsigned char)strOldBuf1[i] >> (8-l));
			k++;
			szNewBuf[k]=(unsigned char)strOldBuf1[i] << l;
			l=8-(6-l);
		}else if(l==6)
		{
			szNewBuf[k]=(unsigned char)szNewBuf[k] | ((unsigned char)strOldBuf1[i] >> (8-l));
			k++;
			l=8;
		}else
		{
			szNewBuf[k]=(unsigned char)szNewBuf[k] | (unsigned char)strOldBuf1[i];
			l -= 6;
		}
	}
	iNewLen=k;
	
	return iNewLen;
	
}

//------------------------------------------------------------------------------
int Code7To8(const char* szOldBuf, char* szNewBuf, const int iOldLen, const int iNewSize)
{
	if( iOldLen > 500 || iOldLen < 1 ) return 0;
	if( iNewSize < iOldLen * 7 / 8 + 1 ) return 0;

	const int SRCBITWIDTH = 7;
	const char cMask = 0xff >> 8-SRCBITWIDTH; // 逻辑右移
	int iDesCt = 0;
	int iSrcCt = 0;
	int iLeave = 0; // 上一次留下几个bit没有使用
	int iCur = 0; // 本次还需要几个bit
	int iCurRight = 0; // 本次需要的bit,需要原始字节逻辑右移几位
	int iCurLeft = 0; // 本次需要的bit,需要原始字节逻辑左移几位
	int iLack = 0; // 本次还需要几个bit
	int iLackRight = 0; // 本次还需要的bit,应由下个源字串逻辑右移几位
	int iLeft = 0; // 留给下一次的bit,需要原始字节逻辑左移几位
	char cLast = 0; // 上一次留下没有填充完整的字节 (已经逻辑左移到最高位)
	memset( szNewBuf, 0, iNewSize );
	

	for( iSrcCt = 0; iSrcCt < iOldLen; )
	{
		// 先填入上次留下的字节
		if( iLeave < 0 || iLeave > SRCBITWIDTH ) return 0; // 保险判断
		else if( iLeave > 0 ) szNewBuf[ iDesCt ] = cLast;

		// 再加入本次需要的bits
		iCur = 8 - iLeave; // 本次还需要的bit数
		if( iCur <= 0 ) return 0; // 保险判断 (iCur肯定<=8,无需判断)
		else if( iCur <= SRCBITWIDTH ) // 若还需要的位数,当前原始字节就能提供
		{
			// 填入本次需要的bits
			iCurRight = SRCBITWIDTH - iCur; // 本次需要的bit,应由原始字节逻辑右移多少位
			if( iCurRight < 0 || iCurRight >= SRCBITWIDTH ) return 0; // 保险判断
			if( iSrcCt < iOldLen ) szNewBuf[ iDesCt ] |= (szOldBuf[ iSrcCt ] & cMask) >> iCurRight;

			// 下次可提供的bits
			iLeave = iCurRight; // 本次遗留的bit数
			if( iLeave < 0 || iLeave >= 8 ) return 0; // 保险判断
			else if( iLeave > 0 )
			{
				iLeft = 8 - iLeave;
				cLast = szOldBuf[ iSrcCt ] << iLeft;
			}
		}
		else // 若还需要的位数,当前原始字节不足以提供 (iCur必定=8,且iLeave必定=0)
		{
			// 先使用当前原始字节填充
			iCurLeft = iCur - SRCBITWIDTH; // 本次需要的bit,应由当前原始字节逻辑左移多少位
			szNewBuf[ iDesCt ] = szOldBuf[ iSrcCt ] << iCurLeft;
			iSrcCt ++;

			// 使用下一原始字节填充
			if( iSrcCt < iOldLen ) // 若还有原始字节时
			{
				// 使用下一原始字节的bits填充
				iLack = 8 - SRCBITWIDTH; // 还需要的几个bit
				if( iLack < 0 || iLack >= SRCBITWIDTH ) return 0; // 保险判断
				iLackRight = SRCBITWIDTH - iLack; // 还需的bit由下一个原始字节右移多少位
				if( iLackRight <= 0 || iLackRight >= SRCBITWIDTH ) return 0; // 保险判断
				szNewBuf[ iDesCt ] |= (szOldBuf[iSrcCt] & cMask) >> iLackRight;

				// 下次可提供的bits
				iLeave = iLackRight; // 本次遗留的bit数
				iLeft = 8 - iLeave; // 本次遗留字节,需要下一个原始字节左移多少位
				cLast = szOldBuf[ iSrcCt ] << iLeft; // 本次遗留字节 (已左移至最高位)
			}
			else
			{
				iLeave = 0;
				cLast = 0;
			}
		}

		// 递增已处理的原始字节计数
		if( iLeave < 8 ) ++ iSrcCt; // 若遗留的位数,不足以生成新的字节

		// 递增转换后的计数
		++ iDesCt;
	}

	return iDesCt;
}

//------------------------------------------------------------------------------
double G_CalGeoDis(const double x1, const double y1, const double x2, const double y2)
{
	double dx,dy,yy,ctg,len=0;

	dx = x1-x2;
	dy = y1-y2;
	yy = (y1<y2 ?y1 : y2) + dy/2;
	if( 0 == dy ) dy = 0.000000001;
	ctg = dx/dy;

	//dl = 6.371004*1000000*2*pi/360;//平均半径算的弧长
	//dl = 6.365755*1000000*2*pi/360;//极地半径算得弧长
	//dl = EQU_LENPERDEGREE; // 赤道每度弧长(m)

	/////近似计算两点距离的公式,误差表见下(117°~118°,24°~25°)
/*
		斜率	误差	MapX计算长度
		|  		-74m 	111194m
		－ 		-68m  	101581m
		45° 	+2663m	150340m
		67.5°	+891m	122163m
		22.5°	+2549m	115627m
*/
	len = EQU_LENPERDEGREE * sqrt( dx * dx + dy * dy )
			* ( cos(yy * PI / 180) + ( 1 - cos( yy * PI / 180) ) * sqrt( 1 / ( 1 + ctg * ctg ) ) );
	//即dis*(cosd+(1-cosd)*sinr);
	return len;
}


//------------------------------------------------------------------------------
// 计算两个经纬度之间的距离
// 返回值单位为米(更准)
double G_CalGeoDis2(const double x1, const double y1, const double x2, const double y2)
{
	double x, y, out;
 //   double PI=3.14159265;
    const double R = 6.371229*1e6;

    x = ( x2 - x1 ) * PI * R * cos( ( ( y1 + y2 )/2 ) * PI/180 )/180;
    y = ( y2 - y1 ) * PI * R/180;
    out = sqrt( x * x + y * y );
    return out;
}


//------------------------------------------------------------------------------
// 计算夹角,输入的角度都是度,且坐标系相同,取值范围都是[0,360)
double G_CalDeltAngle( double v_dAng1, double v_dAng2 )
{
	if( v_dAng1 >= 360 ) v_dAng1 -= 360;
	if( v_dAng2 >= 360 ) v_dAng2 -= 360;

	double dDelt = v_dAng1 - v_dAng2;
	if( dDelt < 0 ) dDelt = -dDelt;
	if( dDelt > 180 ) dDelt = 360 - dDelt;
	return dDelt;
}


//------------------------------------------------------------------------------
bool JugPtInRect( long v_lLon, long v_lLat, long v_lLonMin, long v_lLatMin, long v_lLonMax, long v_lLatMax )
{
	if( v_lLon > v_lLonMax ) return false;
	if( v_lLon < v_lLonMin ) return false;
	if( v_lLat > v_lLatMax ) return false;
	if( v_lLat < v_lLatMin ) return false;

	return true;
}


// ------------------------------------------------------------------------------
// 8转x函数
int Code8ToCodeX(BYTE dest[], const BYTE source[], const int sourceLen, const int x)
{
	const int             step = x;
	
	int                   destLen = 0;
//	int                   stepCount = 0; 
	int                   stepExtra = 0;
	
	BYTE                  headValue = 0;
	
	float temp = float(sourceLen*8.0/step);
	float temp2 = float((int)temp);
	
	destLen = temp>temp2 ? (int)temp+1 : (int)temp;
    
	int sourceNo = 0;
	int destNo = 0;
	
	while(true)
	{
		// check if the convertion is End or Wrong
		if( (sourceNo == sourceLen) && (destNo == destLen) )
			break;
		else if( (sourceNo >= sourceLen) || (destNo >= destLen) )
			return -1;
		
		// calculate dest[destNo]
		sourceNo = ( (temp=float(destNo*step/8.0)) > (int)temp ) ? ((int)(temp+1)) : ((int)temp);
		stepExtra = (stepExtra + (8-step))%8;
		if( (destNo==destLen-1) || (stepExtra==0) )
			dest[destNo] = headValue>>(8-step);
		else
			dest[destNo] = (headValue>>(8-step)) | (source[sourceNo]>>stepExtra);
		
		++destNo;
		headValue = source[sourceNo]<<(8-stepExtra);
	}
	
	return destLen;
}

// ------------------------------------------------------------------------------
// x转8函数
int CodeXToCode8(BYTE dest[], const BYTE source[], const int sourceLen, const int x)
{
	const int          step = x;
	int                destLen;
	
	int                destNo;
	int                sourceNo;
	
	BYTE               tail;
	int                tailLen;
	
	destLen = sourceLen*step/8;
	destNo = 0;
	sourceNo = 0;
	tail = 0;
	
	for(sourceNo = 0; sourceNo < sourceLen; ++sourceNo)
	{
		tailLen = ((8-step)*(sourceNo+1))%8;
		if(tailLen==0)
			continue;
		
		tail = source[sourceNo+1]>>(step-tailLen);
		dest[destNo] = source[sourceNo]<<tailLen | tail;
		++destNo;
	}
	
	return destLen;
}

//------------------------------------------------------------------------------
// 在调试串口上打印缓冲区数据
// void prtbuf(char *head, unsigned char *buf, int len)
// {
// 	char tmp[1024] = {0};
// 	strcat(tmp, head);
// 	strcat(tmp, ": ");
// 
// 	for(int i=0; i<len; i++ ) {
// 		int ln = strlen(tmp);
// 		if(ln>1000)		break;
// 		sprintf(tmp+ln, "%02x ", byte(buf[i]));
// 	}
// 	strcat(tmp, "\r\n");
// 
// 	unsigned short wstr[1024] = {0};
// 	::MultiByteToWideChar(CP_ACP, 0, tmp, strlen(tmp), wstr, 1024);
// 	::OutputDebugStringW(wstr);	
// }


//------------------------------------------------------------------------------
// 计算两个日期的间隔;
// 使用方法: 间隔天数 = count_days(2006,10,1) -  count_days(2006,9,1) ;	
int count_days(int y, int m, int d)
{
	y += m/12 + 100 * (y<50);
	m = m%12;
	if ( m==0 ) {
		m = 12;
		y--;
	}
	return 365*y + (m-1)*30 + d + y/4 + (m>8? (m+1)/2 : m/2) - (m>2? 2 : y%4==0);
}


//------------------------------------------------------------------------------
// 计算校验和(应用于GPRS封装)
byte get_crc_gprs(const byte *buf, const int len)
{
// 通用版协议,公开版协议_研三,	buf从优先级开始
// 校验		优先级	类型	用户数据
// 1 Byte	1 Byte	1 Byte	n Byte

// 公开版协议_研一,	buf从版本号开始
// 版本号（1）	校验(1)	优先级(1)	类型(1)	数据(n)

	byte sum = 0;
	for(int i=0; i<len; i++)
	{
 #if 2 == USE_PROTOCOL
 		if( 1 == i ) continue; // 跳过校验和位置
 #endif
		sum += byte(buf[i]);
#if 0 == USE_PROTOCOL || 2 == USE_PROTOCOL || 30 == USE_PROTOCOL
		if(sum < byte(buf[i])) sum++;
#endif
	}
#if 1 == USE_PROTOCOL
	sum = ~sum;
#endif
	return sum;
}


//------------------------------------------------------------------------------
// 计算校验和(应用于SMS封装)
byte get_crc_sms(const byte *buf, const int len)
{
	byte sum = 0;
	for(int i=0; i<len; i++)
	{
		sum += buf[i];
#if 0 == USE_PROTOCOL || 2 == USE_PROTOCOL
		if(sum < buf[i] )	sum++;
#endif
	}
#if 1 == USE_PROTOCOL
	sum = ~sum;
#endif
	sum &= 0x7f;
	if( 0 == sum ) sum = 0x7f;
	return sum;
}

