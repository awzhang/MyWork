#include "yx_common.h"

int GetRtcTime(rtc_time *v_pRtcTime)
{
	int	iRet = 0;
	int iFrtc;
	
	if((iFrtc = open("/dev/misc/rtc", O_NONBLOCK)) == -1 || ioctl(iFrtc, RTC_RD_TIME, v_pRtcTime) != 0)
	{
		PRTMSG(MSG_ERR, "get rtc time failed\n");
		iRet = -1;
	}
	
	close(iFrtc);
	return iRet;
}

int SetRtcTime(rtc_time *v_pRtcTime)
{
	int	iRet = 0;
	int iFrtc;
	
	if((iFrtc = open("/dev/misc/rtc", O_NONBLOCK)) == -1 || ioctl(iFrtc, RTC_SET_TIME, v_pRtcTime) != 0)
	{
		PRTMSG(MSG_ERR, "set rtc time failed\n");
		iRet = -1;
	}	
	
	close(iFrtc);
	return iRet;
}

int SetSysTime(rtc_time *v_pSysTime)
{
	int	iRet = 0;
	char 	szCommand[255];
	
	sprintf(szCommand, "date -s %02d%02d%02d%02d%04d.%02d", v_pSysTime->tm_mon+1, v_pSysTime->tm_mday, v_pSysTime->tm_hour, v_pSysTime->tm_min, v_pSysTime->tm_year, v_pSysTime->tm_sec);
	if( system(szCommand) )
	{
		PRTMSG(MSG_ERR, "set sys time failed\n");
		iRet = -1;
	}
	
	return iRet;
}

int SetCpuNormal()
{
#if VEHICLE_TYPE == VEHICLE_M
	int	iRet = 0;
	int iFperspm;
	uint tmpdata = 1;

	if((iFperspm=open("/dev/misc/perspm", O_RDWR))==-1 || ioctl(iFperspm, PERSPMIOC_SETSYSSTATUS, &tmpdata)!=0)
	{
		PRTMSG(MSG_ERR, "set cpu normal failed\n");
		iRet = -1;
	}	
	
	close(iFperspm);
	return iRet;
#endif
}

int SetCpuSlow()
{
#if VEHICLE_TYPE == VEHICLE_M
	int	iRet = 0;
	int iFperspm;
	uint tmpdata = 0;

	if((iFperspm=open("/dev/misc/perspm", O_RDWR))==-1 || ioctl(iFperspm, PERSPMIOC_SETSYSSTATUS, &tmpdata)!=0)
	{
		PRTMSG(MSG_ERR, "set cpu slow failed\n");
		iRet = -1;
	}	
	
	close(iFperspm);
	return iRet;
#endif
}

int SetClkEnable()
{
	int	iRet = 0;
	int iFperspm;
#if VEHICLE_TYPE == VEHICLE_M
	uint tmpdata = 0x00000fff;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	uint tmpdata = 0xffffffff;
#endif

	if((iFperspm=open("/dev/misc/perspm", O_RDWR))==-1 || ioctl(iFperspm, PERSPMIOC_SETCLKENABLE, &tmpdata)!=0)
	{
		PRTMSG(MSG_ERR, "set clock enable failed\n");
		iRet = -1;
	}	
	
	close(iFperspm);
	return iRet;
}

int SetClkDisable()
{
	int	iRet = 0;
	int iFperspm;
#if VEHICLE_TYPE == VEHICLE_M
	uint tmpdata = 0xffffffef;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	uint tmpdata = 0xff93fb9f;
#endif

	if((iFperspm=open("/dev/misc/perspm", O_RDWR))==-1 || ioctl(iFperspm, PERSPMIOC_SETCLKDISABLE, &tmpdata)!=0)
	{
		PRTMSG(MSG_ERR, "set clock disable failed\n");
		iRet = -1;
	}	
	
	close(iFperspm);
	return iRet;
}

int IOSET(BYTE v_bytIOSymb, BYTE v_bytIOSta, BYTE v_bytDelay)
{
	int iRet;
	DWORD dwDevSymb = DEV_DVR;
	
	switch(v_bytIOSymb)
	{
		case IOS_TRUMPPOW:
	  case IOS_EARPHONESEL:
			iRet = IOSet( v_bytIOSymb, v_bytIOSta, &dwDevSymb, sizeof(DWORD) );
			break;
		
	  case IOS_PHONERST:
		case IOS_PHONEPOW:
		case IOS_AUDIOSEL:
		case IOS_SCHEDULEPOW:
		case IOS_OILELECCTL:
		case IOS_VIDICONPOW:
#if VEHICLE_TYPE == VEHICLE_M
		case IOS_HUBPOW:
		case IOS_HARDPOW:
		case IOS_APPLEDPOW:
		case IOS_TOPLEDPOW:
		case IOS_TW2835RST:
#endif
#if VEHICLE_TYPE == VEHICLE_V8
		case IOS_YUYIN:
		case IOS_LCDPOW:
		case IOS_USBPOW:
		case IOS_TW2865RST:
		case IOS_SYSLEDPOW_RED:
		case IOS_SYSLEDPOW_GREEN:
#endif
#if VEHICLE_TYPE == VEHICLE_M2
		case IOS_YUYIN:
		case IOS_LCDPOW:
		case IOS_HUBPOW:
		case IOS_HARDPOW:
		case IOS_TW2865RST:
		case IOS_SYSLEDPOW_RED:
		case IOS_SYSLEDPOW_GREEN:
#endif
	  	iRet = IOSet( v_bytIOSymb, v_bytIOSta, NULL, 0 );
			break;

	  default:
	  	return -1;
	}

	if(v_bytDelay)
	{
		msleep(v_bytDelay);
	}
	
	return iRet;
}

int INSMOD(BYTE v_bytKOSymb, BYTE v_bytDelay)
{
	int iRet;
	
	switch(v_bytKOSymb)
	{
		case KO_LINUX_FB:
			iRet = system("insmod /dvs/extdrv/linux_fb.ko");
			break;
		case KO_LINUX_USBSERIAL:
			if (PHONE_MODTYPE == PHONE_MODTYPE_MC703NEW)
			{
				iRet = system("insmod /dvs/extdrv/linux_usbserial.ko vendor=0x12d1 product=0x1413");
				break;
			}
			else if (PHONE_MODTYPE == PHONE_MODTYPE_MC703OLD)
			{
				iRet = system("insmod /dvs/extdrv/linux_usbserial.ko vendor=0x12d1 product=0x1001");
				break;
			}
			else if (PHONE_MODTYPE == PHONE_MODTYPE_TD_LC6311)
			{
				iRet = system("insmod /dvs/extdrv/linux_usbserial.ko vendor=0x1ab7 product=0x6000");
				break;
			}

		case KO_LINUX_MMCCORE:
			iRet = system("insmod /dvs/extdrv/linux_mmccore.ko");
			break;
		case KO_LINUX_MMCBLOCK:
			iRet = system("insmod /dvs/extdrv/linux_mmcblock.ko");
			break;
			
		case KO_HI3510_VS:
	  	iRet = system("insmod /dvs/extdrv/hi3510_vs.ko firmware=/dvs/extdrv/hi3510_v264.bin sio1type=2 sio2type=4");
	  	break;
		case KO_HI3510_EXT3:
			iRet = system("insmod /dvs/extdrv/hi3510_ext3.ko");
			break;
		case KO_HI3510_I2C:
	  	iRet = system("insmod /dvs/extdrv/hi3510_i2c.ko");
	  	break;
		case KO_HI3510_SIO:
	  	iRet = system("insmod /dvs/extdrv/hi3510_sio.ko");
	  	break;
		case KO_HI3510_SSP:
	  	iRet = system("insmod /dvs/extdrv/hi3510_ssp.ko");
	  	break;
		case KO_HI3510_GPIO:
	  	iRet = system("insmod /dvs/extdrv/hi3510_gpio.ko");
	  	break;
		
		case KO_YX3510_GPS:
			iRet = system("insmod /dvs/extdrv/yx3510_gps.ko");
			break;
		case KO_YX3510_IRDA:
			iRet = system("insmod /dvs/extdrv/yx3510_irda.ko irda_group=0 irda_offset=2 irda_intno=6");
			break;
		case KO_YX3510_GPIO:
			iRet = system("insmod /dvs/extdrv/yx3510_gpio.ko");
			break;
		case KO_YX3510_TW2835:
	  	iRet = system("insmod /dvs/extdrv/yx3510_tw2835.ko");
	  	break;
		case KO_YX3510_PERSPM:
	  	iRet = system("insmod /dvs/extdrv/yx3510_perspm.ko");
	  	break;
		case KO_YX3510_PT7C4311:
			iRet = system("insmod /dvs/extdrv/yx3510_pt7c4311.ko");
			break;
		case KO_YX3510_SIM_UART:
			iRet = system("insmod /dvs/extdrv/yx3510_sim_uart.ko sim_num=2 sim_base_baud=9600 sim_base_prescale=2");
			break;
			
		case KO_HI3511_AI:
	  	iRet = system("insmod /dvs/extdrv/hi3511_ai.ko");
	  	break;
		case KO_HI3511_AO:
			iRet = system("insmod /dvs/extdrv/hi3511_ao.ko");
			break;
		case KO_HI3511_BASE:
			iRet = system("insmod /dvs/extdrv/hi3511_base.ko");
			break;
		case KO_HI3511_CHNL:
			iRet = system("insmod /dvs/extdrv/hi3511_chnl.ko");
			break;
		case KO_HI3511_DMAC:
			iRet = system("insmod /dvs/extdrv/hi3511_dmac.ko");
			break;
		case KO_HI3511_DSU:
			iRet = system("insmod /dvs/extdrv/hi3511_dsu.ko");
			break;
		case KO_HI3511_FB:
			iRet = system("insmod /dvs/extdrv/hi3511_fb.ko video=\"hifb:vram0_size:1658880,vram1_size:1658880\"");
			break;
		case KO_HI3511_GROUP:
			iRet = system("insmod /dvs/extdrv/hi3511_group.ko");
			break;
		case KO_HI3511_H264D:
			iRet = system("insmod /dvs/extdrv/hi3511_h264d.ko");
			break;
		case KO_HI3511_H264E:
			iRet = system("insmod /dvs/extdrv/hi3511_h264e.ko");
			break;
		case KO_HI3511_JPEGD:
			iRet = system("insmod /dvs/extdrv/hi3511_jpegd.ko");
			break;
		case KO_HI3511_JPEGE:
			iRet = system("insmod /dvs/extdrv/hi3511_jpege.ko");
			break;
		case KO_HI3511_MD:
			iRet = system("insmod /dvs/extdrv/hi3511_md.ko");
			break;
		case KO_HI3511_MMZ:
			iRet = system("insmod /dvs/extdrv/hi3511_mmz.ko mmz=ddr,0,0xE8000000,128M");
			break;
		case KO_HI3511_MPEG4E:
			iRet = system("insmod /dvs/extdrv/hi3511_mpeg4e.ko");
			break;
		case KO_HI3511_SD:
			iRet = system("insmod /dvs/extdrv/hi3511_sd.ko");
			break;
		case KO_HI3511_SIO:
			iRet = system("insmod /dvs/extdrv/hi3511_sio.ko");
			break;
		case KO_HI3511_SYS:
			iRet = system("insmod /dvs/extdrv/hi3511_sys.ko logbuflen=32");
			break;
		case KO_HI3511_TDE:
			iRet = system("insmod /dvs/extdrv/hi3511_tde.ko");
			break;
		case KO_HI3511_USB:
			iRet = system("insmod /dvs/extdrv/hi3511_usb.ko");
			break;
		case KO_HI3511_VDEC:
			iRet = system("insmod /dvs/extdrv/hi3511_vdec.ko");
			break;
		case KO_HI3511_VENC:
			iRet = system("insmod /dvs/extdrv/hi3511_venc.ko");
			break;
		case KO_HI3511_VIU:
			iRet = system("insmod /dvs/extdrv/hi3511_viu.ko");
			break;
		case KO_HI3511_VOU:
			iRet = system("insmod /dvs/extdrv/hi3511_vou.ko");
			break;
		case KO_HI3511_VPP:
			iRet = system("insmod /dvs/extdrv/hi3511_vpp.ko");
			break;
		case KO_HI3511_MCI:
			iRet = system("insmod /dvs/extdrv/hi3511_mci.ko");
			break;
		
		case KO_YX3511_GPS:
			iRet = system("insmod /dvs/extdrv/yx3511_gps.ko");
			break;
		case KO_YX3511_IRDA:
			iRet = system("insmod /dvs/extdrv/yx3511_irda.ko");
			break;
		case KO_YX3511_GPIO:
			iRet = system("insmod /dvs/extdrv/yx3511_gpio.ko");
			break;
		case KO_YX3511_TW2865:
			iRet = system("insmod /dvs/extdrv/yx3511_tw2865.ko");
			break;
		case KO_YX3511_PERSPM:
			iRet = system("insmod /dvs/extdrv/yx3511_perspm.ko");
			break;
		
	  default:
	  	return -1;
	}
	
	if(v_bytDelay)
	{
		msleep(v_bytDelay);
	}
	
	return iRet;
}

int RMMOD(BYTE v_bytKOSymb, BYTE v_bytDelay)
{
	int iRet;
	
	switch(v_bytKOSymb)
	{
		case KO_LINUX_FB:
			iRet = system("rmmod linux_fb.ko");
			break;
		case KO_LINUX_USBSERIAL:
			iRet = system("rmmod linux_usbserial.ko");
			break;

		case KO_LINUX_MMCCORE:
			iRet = system("rmmod linux_mmccore.ko");
			break;
		case KO_LINUX_MMCBLOCK:
			iRet = system("rmmod linux_mmcblock.ko");
			break;
			
		case KO_HI3510_VS:
	  	iRet = system("rmmod hi3510_vs.ko");
	  	break;
		case KO_HI3510_EXT3:
			iRet = system("rmmod hi3510_ext3.ko");
			break;
		case KO_HI3510_I2C:
	  	iRet = system("rmmod hi3510_i2c.ko");
	  	break;
		case KO_HI3510_SIO:
	  	iRet = system("rmmod hi3510_sio.ko");
	  	break;
		case KO_HI3510_SSP:
	  	iRet = system("rmmod hi3510_ssp.ko");
	  	break;
		case KO_HI3510_GPIO:
	  	iRet = system("rmmod hi3510_gpio.ko");
	  	break;
		
		case KO_YX3510_GPS:
			iRet = system("rmmod yx3510_gps.ko");
			break;
		case KO_YX3510_IRDA:
			iRet = system("rmmod yx3510_irda.ko");
			break;
		case KO_YX3510_GPIO:
			iRet = system("rmmod yx3510_gpio.ko");
			break;
		case KO_YX3510_TW2835:
	  	iRet = system("rmmod yx3510_tw2835.ko");
	  	break;
		case KO_YX3510_PERSPM:
	  	iRet = system("rmmod yx3510_perspm.ko");
	  	break;
		case KO_YX3510_PT7C4311:
			iRet = system("rmmod yx3510_pt7c4311.ko");
			break;
		case KO_YX3510_SIM_UART:
			iRet = system("rmmod yx3510_sim_uart.ko");
			break;
			
		case KO_HI3511_AI:
	  	iRet = system("rmmod hi3511_ai.ko");
	  	break;
		case KO_HI3511_AO:
			iRet = system("rmmod hi3511_ao.ko");
			break;
		case KO_HI3511_BASE:
			iRet = system("rmmod hi3511_base.ko");
			break;
		case KO_HI3511_CHNL:
			iRet = system("rmmod hi3511_chnl.ko");
			break;
		case KO_HI3511_DMAC:
			iRet = system("rmmod hi3511_dmac.ko");
			break;
		case KO_HI3511_DSU:
			iRet = system("rmmod hi3511_dsu.ko");
			break;
		case KO_HI3511_FB:
			iRet = system("rmmod hi3511_fb.ko");
			break;
		case KO_HI3511_GROUP:
			iRet = system("rmmod hi3511_group.ko");
			break;
		case KO_HI3511_H264D:
			iRet = system("rmmod hi3511_h264d.ko");
			break;
		case KO_HI3511_H264E:
			iRet = system("rmmod hi3511_h264e.ko");
			break;
		case KO_HI3511_JPEGD:
			iRet = system("rmmod hi3511_jpegd.ko");
			break;
		case KO_HI3511_JPEGE:
			iRet = system("rmmod hi3511_jpege.ko");
			break;
		case KO_HI3511_MD:
			iRet = system("rmmod hi3511_md.ko");
			break;
		case KO_HI3511_MMZ:
			iRet = system("rmmod hi3511_mmz.ko");
			break;
		case KO_HI3511_MPEG4E:
			iRet = system("rmmod hi3511_mpeg4e.ko");
			break;
		case KO_HI3511_SD:
			iRet = system("rmmod hi3511_sd.ko");
			break;
		case KO_HI3511_SIO:
			iRet = system("rmmod hi3511_sio.ko");
			break;
		case KO_HI3511_SYS:
			iRet = system("rmmod hi3511_sys.ko");
			break;
		case KO_HI3511_TDE:
			iRet = system("rmmod hi3511_tde.ko");
			break;
		case KO_HI3511_USB:
			iRet = system("rmmod hi3511_usb.ko");
			break;
		case KO_HI3511_VDEC:
			iRet = system("rmmod hi3511_vdec.ko");
			break;
		case KO_HI3511_VENC:
			iRet = system("rmmod hi3511_venc.ko");
			break;
		case KO_HI3511_VIU:
			iRet = system("rmmod hi3511_viu.ko");
			break;
		case KO_HI3511_VOU:
			iRet = system("rmmod hi3511_vou.ko");
			break;
		case KO_HI3511_VPP:
			iRet = system("rmmod hi3511_vpp.ko");
			break;
		case KO_HI3511_MCI:
			iRet = system("rmmod hi3511_mci.ko");
			break;
		
		case KO_YX3511_GPS:
			iRet = system("rmmod yx3511_gps.ko");
			break;
		case KO_YX3511_IRDA:
			iRet = system("rmmod yx3511_irda.ko");
			break;
		case KO_YX3511_GPIO:
			iRet = system("rmmod yx3511_gpio.ko");
			break;
		case KO_YX3511_TW2865:
			iRet = system("rmmod yx3511_tw2865.ko");
			break;
		case KO_YX3511_PERSPM:
			iRet = system("rmmod yx3511_perspm.ko");
			break;
		
	  default:
	  	return -1;
	}
	
	if(v_bytDelay)
	{
		msleep(v_bytDelay);
	}
	
	return iRet;
}

bool ExeUpdate()
{
	if( WaitForEvent(MOUNT, &g_objUsbSta.UDisk[0], 15000)==0 )
	{
		char const szUpdateFile[32] = "exe.bin";
		char diskPathUpfile[128] = {0};	//升级路径+升级文件
		
		sprintf(diskPathUpfile, "/mnt/%s/%s", UDISK_UP_PATH, szUpdateFile);
	
		// 将升级文件exe.bin复制到part3的下载目录下，以作为升级备份
		if( AppIntact(diskPathUpfile) == false || 0 != system("cp -f /mnt/UDisk/part1/exe.bin /mnt/Flash/part3/Down/exe.bin") )
		{
			PRTMSG(MSG_DBG, "cp exe.bin to part3 fail!\n");
			return false;
		}
		
#if VEHICLE_TYPE == VEHICLE_M
		if( G_ExeUpdate(diskPathUpfile, 1) == true )
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		if( G_ExeUpdate(diskPathUpfile, 0) == true )
#endif
		{
			PRTMSG(MSG_DBG, "soft update succ\n");
			
			DelErrLog();	// 升级成功后程序从分区5启动
		
			return true;
		}
	}
	
	return false;
}

bool ImageUpdate()
{
	if( WaitForEvent(MOUNT, &g_objUsbSta.UDisk[0], 15000)==0 )
	{
		char const szUpdateFile[32] = "image.bin";
		char diskPathUpfile[128] = {0};	//升级路径+升级文件
		
		sprintf(diskPathUpfile, "/mnt/%s/%s", UDISK_UP_PATH, szUpdateFile);
		
#if VEHICLE_TYPE == VEHICLE_M
		if( G_ImageUpdate(diskPathUpfile, 1)==true )
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		if( G_ImageUpdate(diskPathUpfile, 0)==true )
#endif
		{
			PRTMSG(MSG_DBG, "%s update succ\n", szUpdateFile);
			
			DelErrLog();	// 升级成功后程序从分区5启动
			DelVerFile();	// 升级成功后修改系统版本号
		
			return true;
		}
	}
	
	return false;
}

bool AppUpdate()
{
	if( WaitForEvent(MOUNT, &g_objUsbSta.UDisk[0], 15000)==0 )
	{
		int iUpdateFileCnt = 0;
		int i;
		char const szUpdateFile[][32] = {"QianExe", "ComuExe", "SockServExe", "IOExe", "UpdateExe", "DvrExe", "libComuServ.so"};
		char diskPathUpfile[128] = {0};	//升级路径+升级文件
		char flashPathUpfile[128] = {0};
		
		for(i=0; i<(sizeof(szUpdateFile)/sizeof(szUpdateFile[0])); i++)
		{
			sprintf(diskPathUpfile, "/mnt/%s/%s", UDISK_UP_PATH, szUpdateFile[i]);
			sprintf(flashPathUpfile, "/mnt/Flash/part5/%s", szUpdateFile[i]);
			
			if( access(diskPathUpfile, F_OK)==0 )
			{
				iUpdateFileCnt++;
				
#if VEHICLE_TYPE == VEHICLE_M
				if( G_AppUpdate(diskPathUpfile, flashPathUpfile, 1)!=0 )
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
				if( G_AppUpdate(diskPathUpfile, flashPathUpfile, 0)!=0 )
#endif
				{
					PRTMSG(MSG_DBG, "%s update fail\n", szUpdateFile[i]);
					return false;
				}
			}
		}
		
		if( iUpdateFileCnt!=0 )
		{
			PRTMSG(MSG_DBG, "soft update succ\n");
			
			DelErrLog();	// 升级成功后程序从分区5启动
		
			return true;
		}
	}
	
	return false;
}

bool SysUpdate()
{
	if( WaitForEvent(MOUNT, &g_objUsbSta.UDisk[0], 15000)==0 )
	{
		int iUpdateFileCnt = 0;
		int i;
		char szMtdPath[][32] = {BOOT_PART, KERN_PART, ROOT_PART, UPD_PART, APP_PART};
		char const szUpdateFile[][32] = {	"boot.bin", "kernel.bin", "root.bin", "update.bin", "app.bin"};
		char diskPathUpfile[128] = {0};	//升级路径+升级文件
		
		for(i=0; i<(sizeof(szUpdateFile)/sizeof(szUpdateFile[0])); i++)
		{
			sprintf(diskPathUpfile, "/mnt/%s/%s", UDISK_UP_PATH, szUpdateFile[i]);
			
			if( access(diskPathUpfile, F_OK)==0 )
			{
				iUpdateFileCnt++;
				
#if VEHICLE_TYPE == VEHICLE_M
				if( G_SysUpdate(diskPathUpfile, szMtdPath[i], 1)!=0 )
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
				if( G_SysUpdate(diskPathUpfile, szMtdPath[i], 0)!=0 )
#endif
				{
					PRTMSG(MSG_DBG, "%s update fail\n", szUpdateFile[i]); 
					return false;
				}
				
				if(i == 0 || i == 1 || i == 2)
					DelVerFile();	// 升级成功后修改系统版本号
				else if(i == 3 || i == 4)
					DelErrLog();	// 升级成功后程序从分区5启动
			}
		}
		
		if( iUpdateFileCnt!=0 )
		{
			PRTMSG(MSG_DBG, "system update succ\n"); 
			return true;
		}
	}
	
	return false;
}
