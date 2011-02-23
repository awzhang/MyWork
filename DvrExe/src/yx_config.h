#ifndef _YX_CONFIG_H_
#define _YX_CONFIG_H_ 

enum
{
	KO_LINUX_FB,
	KO_LINUX_USBSERIAL,
	KO_LINUX_MMCCORE,
	KO_LINUX_MMCBLOCK,

	KO_HI3510_VS,
	KO_HI3510_I2C,
	KO_HI3510_SIO,
	KO_HI3510_SSP,
	KO_HI3510_EXT3,
	KO_HI3510_GPIO,
	
	KO_YX3510_GPS,
	KO_YX3510_IRDA,
	KO_YX3510_GPIO,
	KO_YX3510_TW2835,
	KO_YX3510_PERSPM,
	KO_YX3510_PT7C4311,
	KO_YX3510_SIM_UART,
	
	KO_HI3511_AI,
	KO_HI3511_AO,
	KO_HI3511_BASE,
	KO_HI3511_CHNL,
	KO_HI3511_DMAC,
	KO_HI3511_DSU,
	KO_HI3511_FB,
	KO_HI3511_GROUP,
	KO_HI3511_H264D,
	KO_HI3511_H264E,
	KO_HI3511_JPEGD,
	KO_HI3511_JPEGE,
	KO_HI3511_MD,
	KO_HI3511_MMZ,
	KO_HI3511_MPEG4E,
	KO_HI3511_SD,
	KO_HI3511_SIO,
	KO_HI3511_SYS,
	KO_HI3511_TDE,
	KO_HI3511_USB,
	KO_HI3511_VDEC,
	KO_HI3511_VENC,
	KO_HI3511_VIU,
	KO_HI3511_VOU,
	KO_HI3511_VPP,
	KO_HI3511_MCI,
	
	KO_YX3511_GPS,
	KO_YX3511_IRDA,
	KO_YX3511_GPIO,
	KO_YX3511_TW2865,
	KO_YX3511_PERSPM
};

int GetRtcTime(rtc_time *v_pRtcTime);
int SetRtcTime(rtc_time *v_pRtcTime);
int SetSysTime(rtc_time *v_pSysTime);

int SetCpuNormal();
int SetCpuSlow();
int SetClkEnable();
int SetClkDisable();

int IOSET(BYTE v_bytIOSymb, BYTE v_bytIOSta, BYTE v_bytDelay=0);
int INSMOD(BYTE v_bytKOSymb, BYTE v_bytDelay=0);
int RMMOD(BYTE v_bytKOSymb, BYTE v_bytDelay=0);

bool ExeUpdate();
bool ImageUpdate();
bool AppUpdate();
bool SysUpdate();

#endif
