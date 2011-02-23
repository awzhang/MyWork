#include "ComuStdAfx.h"

#if SMALL_VER == 1
#define _REENTRANT
#include 	<time.h>
#include 	<stdio.h>
#include	<fcntl.h> 
#include	<errno.h>
#include  <netdb.h>
#include 	<stdlib.h>
#include	<string.h>
#include  <signal.h>
#include  <stdarg.h>
#include  <assert.h>
#include	<unistd.h>
#include	<dirent.h>
#include 	<getopt.h>
#include	<termios.h>
#include  <pthread.h>
#include  <arpa/inet.h>
#include  <netinet/in.h>
#include  <asm/page.h>  
#include  <sys/uio.h>
#include 	<sys/vfs.h>
#include  <sys/ipc.h> 
#include  <sys/shm.h>
#include  <sys/sem.h> 
#include	<sys/stat.h>
#include  <sys/mman.h>
#include  <sys/poll.h>
#include  <sys/time.h>
#include  <sys/wait.h>
//#include <sys/mount.h>
#include <sys/types.h>
#include  <sys/ioctl.h>
#include  <sys/select.h>
#include  <sys/socket.h>
#include  <linux/fs.h>
#include  <linux/fb.h>
#include  <linux/rtc.h>
#include  <linux/ioctl.h>
#include  <linux/types.h>
#endif

#undef MSG_HEAD
#define MSG_HEAD ("ComuExe-Main   ")

int _InitAll()
{
#if VEHICLE_TYPE == VEHICLE_M
	g_objLightCtrl.Init();
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	g_objLightCtrl2.Init();
#endif

#if SMALL_VER == 0
 	g_objGpsSrc.InitGpsAll();
#endif
 
	g_objComAdjust.Init();

	g_hst.Init();

	g_hstapp.Init();

 	g_objDiaodu.Init();
 	
 	sleep(1);
 	system("insmod /dvs/extdrv/hi3511_usb.ko");

	if( NETWORK_TYPE==NETWORK_GSM )
	{
		g_objComVir.Init();
		g_objPhoneGsm.InitPhone();
	}
	else if( NETWORK_TYPE==NETWORK_TD )
	{
		g_objPhoneTd.InitPhone();
	}
	else if( NETWORK_TYPE==NETWORK_EVDO )
	{
		g_objPhoneEvdo.InitPhone();
	}
	else if( NETWORK_TYPE==NETWORK_WCDMA )
	{
		g_objPhoneWcdma.InitPhone();
	}

	return 0;
}

int _ReleaseAll()
{
	if( NETWORK_TYPE==NETWORK_GSM )
	{
		g_objPhoneGsm.ReleasePhone();
		g_objComVir.Release();
	}
	else if( NETWORK_TYPE==NETWORK_TD )
	{
		g_objPhoneTd.ReleasePhone();
	}
	else if( NETWORK_TYPE==NETWORK_EVDO )
	{
		g_objPhoneEvdo.ReleasePhone();
	}
	else if( NETWORK_TYPE==NETWORK_WCDMA )
	{
		g_objPhoneWcdma.ReleasePhone();
	}

#if SMALL_VER == 0
	g_objGpsSrc.ReleaseGpsAll();
#endif

	g_objDiaodu.Release();

	g_hstapp.Release();

	g_hst.Release();

	g_objComAdjust.Release();

#if VEHICLE_TYPE == VEHICLE_M
	g_objLightCtrl.Release();
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	g_objLightCtrl2.Release();
#endif

	return 0;
}

void *G_RecvDiaoduData(void *arg)
{
	//读取通信组件diaodu数据
	char szbuf[MAX_DEAL_BUF_SIZE] = {0};
	DWORD dwLen;
	BYTE  bytLvl;
	DWORD dwPktNum;
	DWORD dwSrcSymb;
	
	int iResult = DataWaitPop(DEV_DIAODU); 
	while( !iResult ) 
	{
		if( g_bProgExit )  
		{ 
			break; 
		} 
		
		//pop进程组件发送到调度的数据
		while( !DataPop(szbuf, MAX_DEAL_BUF_SIZE, &dwLen, &dwSrcSymb, DEV_DIAODU, &bytLvl) )
		{ 
			if( dwLen < 1 ) continue;	//保险处理

			if(DEV_IO == dwSrcSymb )
			{
				continue;
			}

			switch( szbuf[0] )
			{
			case 0x00:		// 使用串口扩展盒，从调度屏/手柄接收到的数据
				{
					g_objComAdjust.AnalyseComFrame(szbuf+1, dwLen-1);
				}
				break;

			case 0x01:		// 要发往调度屏/手柄的数据
				{
					g_objDiaoduOrHandWorkMng.PushData((BYTE)LV1, dwLen-1, (char*)szbuf+1, dwPktNum, 0);
				}
				break;

			case 0x02:		// 指示灯控制
				{
#if VEHICLE_TYPE == VEHICLE_M
					g_objLightCtrl.SetLightSta(szbuf[1]);
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
					if(dwLen > 2)
						g_objLightCtrl2.m_objMemInfoMng.PushData(LV1, dwLen-1, (char*)szbuf+1, dwPktNum);
					else
						g_objLightCtrl2.SetLightSta(szbuf[1]);
#endif
				}
				break;

			case 0x03:		//油耗AD指示
				{
					memcpy((void*)&g_usOilAD, szbuf+1, 2);
				}
				break;

			case 0x04:		// 请求恢复GPS
				{
					g_objGpsSrc.m_bNeedGpsOn = true;
				}
				break;
			case 0x05:		//GPS开关
				{
					g_objGpsSrc.GpsSwitch = szbuf[1] ? false : true;
				}
				break;	
			default:
				break;
			}
		}
		
		iResult = DataWaitPop(DEV_DIAODU); 
	}
	
	// DataWaitPop返回出错，可能以后需要添加一些清除操作
	if(ERR_MSGSKIPBLOCK == iResult)
	{
		
	}
	
	// 若接收线程退出，则整个程序退出
	g_bProgExit = true;
	PRTMSG(MSG_NOR,"G_RecvDiaoduData线程退出\n");
}

void *G_RecvPhoneData(void *arg)
{
	//读取通信组件phone数据
	char szbuf[MAX_DEAL_BUF_SIZE] = {0};
	DWORD dwLen;
	BYTE  bytLvl;
	DWORD dwPktNum;
	DWORD dwSrcSymb;
	
	int iResult = DataWaitPop(DEV_PHONE); 
	while( !iResult ) 
	{		
		if( g_bProgExit )  
		{ 
			break; 
		} 
		
		//pop进程组件发送到手机模块的数据
		while( !DataPop(szbuf, MAX_DEAL_BUF_SIZE, &dwLen, &dwSrcSymb, DEV_PHONE, &bytLvl) )
		{ 
			if( dwLen < 1 ) continue;	//保险处理

			if(DEV_IO == dwSrcSymb )
			{
				g_objIoStaRcvMng.PushData((BYTE)LV1, dwLen, (char*)szbuf, dwPktNum);
				continue;
			}

			//推到手机模块数据接收队列
			if( dwSrcSymb != DEV_IO )
				g_objPhoneRecvDataMng.PushData((BYTE)LV1, dwLen, (char*)szbuf, dwPktNum, 0);
		} 
		
		iResult = DataWaitPop(DEV_PHONE); 
	}
	
	// DataWaitPop返回出错，可能以后需要添加一些清除操作
	if(ERR_MSGSKIPBLOCK == iResult)
	{
		
	}
	
	// 若接收线程退出，则整个程序退出
	g_bProgExit = true;
	PRTMSG(MSG_NOR,"G_RecvPhoneData线程退出\n");
}

void *G_DealIOThread(void* arg)
{
	char  szbuf[100];
	DWORD dwLen;
	BYTE  bytLvl;
	BYTE  bytSrcSymb;
	DWORD dwPushTm;
	
	while( !g_bProgExit )
	{
		usleep(100000);
		
		while( !g_objIoStaRcvMng.PopData(bytLvl, (DWORD)sizeof(szbuf), dwLen, (char*)szbuf, dwPushTm, &bytSrcSymb) )
		{
			if( dwLen != 1 )
				continue;
			
			g_objIoSta.DealIOMsg(szbuf, dwLen, bytSrcSymb);
		}
	}
}

void (* G_Signal(int signo, void(* func)(int v_signo)))(int)
{
	struct sigaction act;//新的信号关联信息
	struct sigaction oact;//老的信号关联信息
	act.sa_handler = func;//新的信号处理函数
	sigemptyset(&act.sa_mask);//初始化屏蔽字
	act.sa_flags = 0;//初始化flags
	if (signo == SIGALRM) 
	{//不会重启动
		//		act.sa_flags |= SA_INERRUPT;
	} 
	else 
	{ //重启动
		act.sa_flags |= SA_RESTART;
	}
	if (sigaction(signo, &act, &oact) < 0)//新老交换
		return(SIG_ERR);
	return NULL;
}

#if SMALL_VER == 1

#if VEHICLE_TYPE == VEHICLE_M
#define BUS_SDISK ("/sys/bus/usb/drivers/usb-storage/1-2.1:1.0")
#define BUS_HDISK ("/sys/bus/usb/drivers/usb-storage/1-1:1.0")
#define BUS_UDISK ("/sys/bus/usb/drivers/usb-storage/1-2.2:1.0")
#define DEV_SDISK ("/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.1/1-2.1:1.0")
#define DEV_HDISK ("/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.0")
#define DEV_UDISK ("/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.2/1-2.2:1.0")
#endif

#if VEHICLE_TYPE == VEHICLE_M2
#define BUS_SDISK ("/sys/bus/usb/drivers/usb-storage/1-1.1:1.0")
#define BUS_HDISK ("/sys/bus/usb/drivers/usb-storage/1-1.3:1.0")
#define BUS_UDISK ("/sys/bus/usb/drivers/usb-storage/2-1:1.0")
#define DEV_SDISK ("/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.1/1-1.1:1.0")
#define DEV_HDISK ("/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.3/1-1.3:1.0")
#define DEV_UDISK ("/sys/devices/platform/hiusb-hcd.0/usb2/2-1/2-1:1.0")
#endif

#if VEHICLE_TYPE == VEHICLE_V8
#define BUS_UDISK ("/sys/bus/usb/drivers/usb-storage/2-1:1.0")
#define DEV_UDISK ("/sys/devices/platform/hiusb-hcd.0/usb2/2-1/2-1:1.0")
#endif

//磁盘使用率上下限
#define MAX_INODE_USAGE_SDISK 0.90
#define MIN_INODE_USAGE_SDISK 0.80
#define MAX_INODE_USAGE_HDISK 0.90
#define MIN_INODE_USAGE_HDISK 0.80
#define MAX_INODE_USAGE_UDISK 0.90
#define MIN_INODE_USAGE_UDISK 0.80

#define MAX_SPACE_USAGE_SDISK 0.90
#define MIN_SPACE_USAGE_SDISK 0.80
#define MAX_SPACE_USAGE_HDISK 0.90
#define MIN_SPACE_USAGE_HDISK 0.80
#define MAX_SPACE_USAGE_UDISK 0.90
#define MIN_SPACE_USAGE_UDISK 0.80

//磁盘检测间隔(秒)
#define VALID_DECCNT_SDISK 6 
#define VALID_DECCNT_HDISK 6
#define VALID_DECCNT_UDISK 6

#define SPACE_DECCNT_SDISK 60 
#define SPACE_DECCNT_HDISK 60
#define SPACE_DECCNT_UDISK 60

#define INODE_DECCNT_SDISK 60*60
#define INODE_DECCNT_HDISK 60*60
#define INODE_DECCNT_UDISK 60*60

//磁盘分区数
#define PART_NUM_SDISK 2
#define PART_NUM_HDISK 2
#define PART_NUM_UDISK 1

//磁盘未挂载复位时间(秒)
#define UMOUNT_REBOOT_SDISK 1200 
#define UMOUNT_REBOOT_HDISK 1200 
#define UMOUNT_REBOOT_UDISK 1200 

//磁盘未识别复位时间(秒)
#define UCONNECT_INIT_SDISK 60 
#define UCONNECT_INIT_HDISK 60 
#define UCONNECT_INIT_UDISK 60 

/*存储器状态*/
enum
{
	UCON_UMOU_UVAL,//未连接、未挂载、无可用空间
		CON_UMOU_UVAL,//已连接、未挂载、无可用空间
		CON_MOU_UVAL,//已连接、已挂载、无可用空间
		CON_MOU_VAL//已连接、已挂载、有可用空间
};


/*USB设备状态*/
typedef struct yx_USB_STATE
{
	BYTE	SDisk[2];
	BYTE	HDisk[2];
	BYTE	UDisk[2];
}USB_STATE;

/*USB设备卸载*/
typedef struct yx_USB_UMOUNT
{
	BOOL	SDisk[2];
	BOOL	HDisk[2];
	BOOL	UDisk[2];
}USB_UMOUNT;

USB_STATE 				g_objUsbSta 			= {UCON_UMOU_UVAL};
USB_UMOUNT				g_objUsbUmount		=	{FALSE};

enum{ SDISK, HDISK, UDISK };
enum{ PART1=0, PART2, PART3 };

bool UCONNECT(void *Event)
{
	if( *((BYTE*)Event)==UCON_UMOU_UVAL )
		return true;
	else
		return false;
}

bool UMOUNT(void *Event)
{
	if( *((BYTE*)Event)==CON_UMOU_UVAL )
		return true;
	else
		return false;
}

bool MOUNT(void *Event)
{
	if( *((BYTE*)Event)==CON_MOU_UVAL || *((BYTE*)Event)==CON_MOU_VAL )
		return true;
	else
		return false;
}

//单屏显示
void ShowDiaodu(char *fmt, ...)
{
	char str[1024] = {0x01, 0x72, 0x01};
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(str+3, fmt, ap);
	va_end(ap);
	
	DataPush(str, strlen(str), DEV_DIAODU, DEV_DIAODU, LV2);
}

int GetFileNum(char *v_szPath, char *v_szName, bool v_bRecur)
{
	int i;
	int iAllFileNum = 0;
	int iFileNum = 0;
	char szPath[1024] = {0};
	struct dirent **objList;
	struct stat objStat;
	
	iAllFileNum = scandir(v_szPath, &objList, 0, alphasort);	
	
	for(i=0; i<iAllFileNum; i++)
	{
//		ClearThreadDog(1);//防止文件太多时计算时间过长而来不及清看门狗
		
		STRCPY(szPath, 2, v_szPath, objList[i]->d_name);
		if( 0!=stat(szPath, &objStat) )
		{
			PRTMSG(MSG_ERR, "stat err\n");
			perror("");
			continue;
		}
		
		if(S_ISREG(objStat.st_mode) || S_ISCHR(objStat.st_mode) || S_ISBLK(objStat.st_mode))//找到一个文件
		{
			if( STRCMP(v_szName, objList[i]->d_name)==0 )// 满足条件
				iFileNum += 1;
			
			strcpy(szPath, v_szPath);
		}
		else if(S_ISDIR(objStat.st_mode))//找到一个目录
		{
			if(strcmp(objList[i]->d_name, ".") && strcmp(objList[i]->d_name, ".."))//忽略这两个目录
			{
				if(v_bRecur)//递归查找
				{
					strcat(szPath, "/");
					iFileNum += GetFileNum(szPath, v_szName, v_bRecur);//子目录下查找目标
				}
				else if( STRCMP(v_szName, objList[i]->d_name)==0 )// 满足条件
					iFileNum += 1;
			}
			
			strcpy(szPath, v_szPath);
		}
	}
	
	return iFileNum;
}

//磁盘分区未挂载时获取到的是Flash的容量
void GetDiskSpace( int v_iDiskStyle, int v_iDiskPart, long long *v_lTotalSpace, long long *v_lFreeSpace, long long *v_lAvailSpace )
{
	char szDiskPart[128] = {0};
	struct statfs buf = {0};
	
//	ClearThreadDog(1);//防止文件太多时计算时间过长而来不及清看门狗
	
	switch( v_iDiskStyle )
	{
		case SDISK:
		{
			if( MOUNT(&g_objUsbSta.SDisk[v_iDiskPart]) )
			{
				statfs(SPRINTF(szDiskPart, "/mnt/SDisk/part%d", v_iDiskPart+1), &buf);
				goto DISK_SIZE;
			}
		}
			break;
		case HDISK:
		{
			if( MOUNT(&g_objUsbSta.HDisk[v_iDiskPart]) )
			{
				statfs(SPRINTF(szDiskPart, "/mnt/HDisk/part%d", v_iDiskPart+1), &buf);
				goto DISK_SIZE;
			}
		}
			break;
		case UDISK:
		{
			if( MOUNT(&g_objUsbSta.UDisk[v_iDiskPart]) )
			{
				statfs(SPRINTF(szDiskPart, "/mnt/UDisk/part%d", v_iDiskPart+1), &buf);
				goto DISK_SIZE;
			}
		}
			break;
		default:
			break;
	}
	
	if(v_lTotalSpace)	
		*v_lTotalSpace = -1;
	if(v_lFreeSpace)	  
		*v_lFreeSpace  = -1;
	if(v_lAvailSpace)	
		*v_lAvailSpace = -1;
	return;
	
DISK_SIZE:
	if(v_lTotalSpace)	
		*v_lTotalSpace = (long long)buf.f_bsize * buf.f_blocks;
	if(v_lFreeSpace)	  
		*v_lFreeSpace  = (long long)buf.f_bsize * buf.f_bfree;
	if(v_lAvailSpace)	
		*v_lAvailSpace = (long long)buf.f_bsize * buf.f_bavail;
}

double GetDiskUsage( int v_iDiskStyle, int v_iDiskPart, char v_cUsageType )
{
	double dDiskUsage;
	int iTotalInode, iUseInode;
	long long lTotalSpace, lAvailSpace;
	
	GetDiskSpace(v_iDiskStyle, v_iDiskPart, &lTotalSpace, NULL, &lAvailSpace);

	//计算磁盘节点使用率
	if( v_cUsageType=='I' )
	{
		switch( v_iDiskStyle )
		{
			case SDISK:
			{
				if( v_iDiskPart==PART1 )
				{
					iTotalInode = lTotalSpace/1048576;
					iUseInode = GetFileNum("/mnt/SDisk/part1/data/", "*", true);
				}
				else if( v_iDiskPart==PART2 )
				{
					iTotalInode = lTotalSpace/4096;
					iUseInode = GetFileNum("/mnt/SDisk/part2/data/", "*", true);
				}
			}
				break;
			case HDISK:
			{
				if( v_iDiskPart==PART1 )
				{
					iTotalInode = lTotalSpace/1048576;
					iUseInode = GetFileNum("/mnt/HDisk/part1/data/", "*", true);
				}
				else if( v_iDiskPart==PART2 )
				{
					iTotalInode = lTotalSpace/4096;
					iUseInode = GetFileNum("/mnt/HDisk/part2/data/", "*", true);
				}
			}
				break;
			case UDISK:
			{
				if( v_iDiskPart==PART1 )
				{
					iTotalInode = lTotalSpace/4096;
					iUseInode = GetFileNum("/mnt/UDisk/part1/data/", "*", true);
				}
				else if( v_iDiskPart==PART2 )
				{
					iTotalInode = lTotalSpace/4096;
					iUseInode = GetFileNum("/mnt/UDisk/part2/data/", "*", true);
				}
			}
				break;
			default:
				break;
		}
		
		dDiskUsage = iUseInode/(double)iTotalInode;
	}
	//计算磁盘容量使用率
	else if( v_cUsageType=='S' )
	{
		dDiskUsage = 1 - lAvailSpace/(double)lTotalSpace;
	}
	
	return dDiskUsage;
}

void* G_ChkUDisk(void* arg)
{
	char szTempBuf[1024];
	
	int iRet;
	int i,j;
	
	DWORD dwSDiskUconnectCnt[PART_NUM_SDISK] = {0}, dwHDiskUconnectCnt[PART_NUM_HDISK] = {0}, dwUDiskUconnectCnt[PART_NUM_UDISK] = {0};
	DWORD dwSDiskUmountCnt[PART_NUM_SDISK] = {0}, dwHDiskUmountCnt[PART_NUM_HDISK] = {0}, dwUDiskUmountCnt[PART_NUM_UDISK] = {0};
	
	DWORD dwSDiskMountCnt[PART_NUM_SDISK] = {0}, dwHDiskMountCnt[PART_NUM_HDISK] = {0}, dwUDiskMountCnt[PART_NUM_UDISK] = {0};
	DWORD dwSDiskDetectCnt[PART_NUM_SDISK] = {0}, dwHDiskDetectCnt[PART_NUM_HDISK] = {0}, dwUDiskDetectCnt[PART_NUM_UDISK] = {0};
	
	BOOL bSDiskExist[PART_NUM_SDISK] = {FALSE}, bHDiskExist[PART_NUM_HDISK] = {FALSE}, bUDiskExist[PART_NUM_UDISK] = {FALSE};
	BOOL bSDiskSpaceValid[PART_NUM_SDISK] = {FALSE}, bHDiskSpaceValid[PART_NUM_HDISK] = {FALSE}, bUDiskSpaceValid[PART_NUM_UDISK] = {FALSE};
	BOOL bSDiskInodeValid[PART_NUM_SDISK] = {FALSE}, bHDiskInodeValid[PART_NUM_HDISK] = {FALSE}, bUDiskInodeValid[PART_NUM_UDISK] = {FALSE};
	BOOL bSDiskMount[PART_NUM_SDISK] = {TRUE}, bHDiskMount[PART_NUM_HDISK] = {TRUE}, bUDiskMount[PART_NUM_UDISK] = {TRUE};
	
	FILE *pSDiskFile[PART_NUM_SDISK] = {NULL}, *pHDiskFile[PART_NUM_HDISK] = {NULL}, *pUDiskFile[PART_NUM_UDISK] = {NULL};
	struct dirent **ppSDiskList, **ppHDiskList, **ppUDiskList;
	struct statfs objSDiskBuf, objHDiskBuf, objUDiskBuf;
	
	double dSDiskUsage[PART_NUM_SDISK] = {0}, dHDiskUsage[PART_NUM_HDISK] = {0}, dUDiskUsage[PART_NUM_UDISK] = {0};
	double dSDiskInodeUsage[PART_NUM_SDISK] = {0}, dSDiskSpaceUsage[PART_NUM_SDISK] = {0};
	double dHDiskInodeUsage[PART_NUM_HDISK] = {0}, dHDiskSpaceUsage[PART_NUM_HDISK] = {0};
	double dUDiskInodeUsage[PART_NUM_UDISK] = {0}, dUDiskSpaceUsage[PART_NUM_UDISK] = {0};

#if VEHICLE_TYPE == VEHICLE_V8
	IOSet(IOS_USBPOW, IO_USBPOW_ON, NULL, 0);
#endif
	
	while(1)
	{
		///----------U盘状态检测----------
		for(i=0; i<PART_NUM_UDISK; i++)
		{
			switch(g_objUsbSta.UDisk[i])
			{
			case UCON_UMOU_UVAL:
				{
					//					ResetUsb();//如果usb otg出错则复位之
					if(access(BUS_UDISK, F_OK) == 0)
					{
						int n = scandir(DEV_UDISK, &ppUDiskList, 0, alphasort);	
						if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/disc", ppUDiskList[10]->d_name), F_OK) == 0)
						{
							bUDiskMount[i] = TRUE;
							g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;
						}
					}
				}
				break;
			case CON_UMOU_UVAL:
				{
					if( g_objUsbUmount.UDisk[i] )
						break;
					
					if(access(SPRINTF(szTempBuf, "/dev/scsi/%s/bus0/target0/lun0/part%d", ppUDiskList[10]->d_name, i+1), F_OK) == 0)
					{	
						if( bUDiskMount[i] )
						{
							if(SYSTEM("mount -t vfat -o rw /dev/scsi/%s/bus0/target0/lun0/part%d /mnt/UDisk/part%d", ppUDiskList[10]->d_name, i+1, i+1) == 0 
								&&access(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/flash", i+1), F_OK) != 0)
							{
								if(access(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), F_OK) == 0)
								{
									chmod(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);// 将目录的访问权限设置为所有人可读、可写、可执行
								}
								else
								{   
									if(mkdir(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), 0777) == 0)   
									{   
										chmod(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data", i+1), S_IRWXU|S_IRWXG|S_IRWXO);
									}
								}
								
								if((pUDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data/disk", i+1), "w+b")) == NULL)
								{
									switch(++dwUDiskDetectCnt[i])
									{
									case 1:
									case 2:
									case 3:
									case 4:
										{
											dwUDiskMountCnt[i] = 0;
											g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
											
											SYSTEM("umount /mnt/UDisk/part%d", i+1);
										}
										break;
									case 5:
										{
											bUDiskMount[i] = FALSE;//当U盘连续5次读写失败时，停止挂载
										}
										break;
									default:
										break;
									}
								}
								else
								{
									fclose(pUDiskFile[i]);
									pUDiskFile[i] = NULL;
									
									dwUDiskMountCnt[i] = 0;
									dwUDiskDetectCnt[i] = 0;
									
									g_objUsbSta.UDisk[i] = CON_MOU_UVAL;
									
									dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
									dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
									
									if( dUDiskInodeUsage[i]>=MAX_INODE_USAGE_UDISK )
									{
										bUDiskInodeValid[i] = FALSE;
									}
									else
									{
										bUDiskInodeValid[i] = TRUE;
									}
									
									if( dUDiskSpaceUsage[i]>=MAX_SPACE_USAGE_UDISK )
									{
										bUDiskSpaceValid[i] = FALSE;
									}
									else
									{
										bUDiskSpaceValid[i] = TRUE;
									}
									
									if( bUDiskInodeValid[i] && bUDiskSpaceValid[i] )
									{
										g_objUsbSta.UDisk[i] = CON_MOU_VAL;
									}
									
									PRTMSG(MSG_NOR, "UDisk part%d mount succ, inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dUDiskInodeUsage[i], 100*dUDiskSpaceUsage[i]);
									
									bool bMount = true;
									for(int c=0; c<PART_NUM_UDISK; c++)
									{
										if( MOUNT(&g_objUsbSta.UDisk[c])==false )
										{
											bMount = false;
											break;
										}
									}
									if( bMount )
										ShowDiaodu("检测到U盘插入");
								}
							}
							else 
							{
								switch( ++dwUDiskMountCnt[i] )
								{
								case 2:
								case 4:
									//										SYSTEM("umount /mnt/UDisk/part%d", i+1);
									//										g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;
									break;
								case 5:
									bUDiskMount[i] = FALSE;//当U盘连续5次挂载失败时，停止挂载
									PRTMSG(MSG_NOR, "UDisk part%d can not dectect\n", i+1);
									break;
								default:
									break;
								}
							}
						}
					}
					else
					{
						dwUDiskMountCnt[i] = 0;
						dwUDiskDetectCnt[i] = 0;
						g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
					}
				}
				break;
				case CON_MOU_UVAL:
					{
						if( g_objUsbUmount.UDisk[i] )
						{
							if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
							}
							
							dwUDiskMountCnt[i] = 0;
							dwUDiskDetectCnt[i] = 0;
							g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;	
							
							break;
						}
						
						++dwUDiskDetectCnt[i];
						
						if((pUDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data/disk", i+1), "r+b")) != NULL)
						{
							fclose(pUDiskFile[i]);
							pUDiskFile[i] = NULL;
							
							//ClearDisk("UDisk", SPRINTF(szTempBuf, "part%d", i+1));
							
							if( !bUDiskInodeValid[i] )
							{
								dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
								if( dUDiskInodeUsage[i]<=MIN_INODE_USAGE_UDISK )
								{
									bUDiskInodeValid[i] = TRUE;
								}
							}
							
							if( !bUDiskSpaceValid[i] )
							{
								dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
								if( dUDiskSpaceUsage[i]<=MIN_SPACE_USAGE_UDISK )
								{
									bUDiskSpaceValid[i] = TRUE;
								}
							}
							
							if( bUDiskInodeValid[i] && bUDiskSpaceValid[i] )
							{
								//							dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
								//							dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
								PRTMSG(MSG_NOR, "UDisk part%d: inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dUDiskInodeUsage[i], 100*dUDiskSpaceUsage[i]);
								
								g_objUsbSta.UDisk[i] = CON_MOU_VAL;
							}
						}
						else
						{
							dwUDiskMountCnt[i] = 0;
							dwUDiskDetectCnt[i] = 0;
							g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;	
							
							if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
							}
							
							bool bUconnect = true;
							for(int c=0; c<PART_NUM_UDISK; c++)
							{
								if( UCONNECT(&g_objUsbSta.UDisk[c])==false )
								{
									bUconnect = false;
									break;
								}
							}
							if( bUconnect )
								ShowDiaodu("检测到U盘拔出");
						}
					}
					break;
				case CON_MOU_VAL:
					{
						if( g_objUsbUmount.UDisk[i] )
						{
							if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
							{
								PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);							
							}
							
							dwUDiskMountCnt[i] = 0;
							dwUDiskDetectCnt[i] = 0;
							g_objUsbSta.UDisk[i] = CON_UMOU_UVAL;	
							
							break;
						}
						
						if(++dwUDiskDetectCnt[i]%VALID_DECCNT_UDISK == 0)
						{
							if((pUDiskFile[i] = fopen(SPRINTF(szTempBuf, "/mnt/UDisk/part%d/data/disk", i+1), "r+b")) != NULL)
							{
								fclose(pUDiskFile[i]);
								pUDiskFile[i] = NULL;
								
								if(dwUDiskDetectCnt[i]%INODE_DECCNT_UDISK == 0)
								{
									dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
									if( dUDiskInodeUsage[i]>=MAX_INODE_USAGE_UDISK )
									{
										bUDiskInodeValid[i] = FALSE;
									}
								}
								
								if(dwUDiskDetectCnt[i]%SPACE_DECCNT_UDISK == 0)
								{
									dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
									if( dUDiskSpaceUsage[i]>=MAX_SPACE_USAGE_UDISK )
									{
										bUDiskSpaceValid[i] = FALSE;
									}
								}
								
								if( !bUDiskInodeValid[i] || !bUDiskSpaceValid[i] )
								{
									//								dUDiskInodeUsage[i] = GetDiskUsage(UDISK, i, 'I');
									//								dUDiskSpaceUsage[i] = GetDiskUsage(UDISK, i, 'S');
									PRTMSG(MSG_NOR, "UDisk part%d: inode usage %.3f%%, space usage %.3f%%\n", i+1, 100*dUDiskInodeUsage[i], 100*dUDiskSpaceUsage[i]);
									
									g_objUsbSta.UDisk[i] = CON_MOU_UVAL;
								}
							}
							else
							{
								if(SYSTEM("umount /mnt/UDisk/part%d", i+1) == 0)
								{
									PRTMSG(MSG_NOR, "UDisk part%d umount succ\n", i+1);
								}
								
								dwUDiskMountCnt[i] = 0;
								dwUDiskDetectCnt[i] = 0;
								g_objUsbSta.UDisk[i] = UCON_UMOU_UVAL;
								
								bool bUconnect = true;
								for(int c=0; c<PART_NUM_UDISK; c++)
								{
									if( UCONNECT(&g_objUsbSta.UDisk[c])==false )
									{
										bUconnect = false;
										break;
									}
								}
								if( bUconnect )
									ShowDiaodu("检测到U盘拔出");
							}
						}
					}
					break;
				default:
					break;
			}
		}

		sleep(1);
	}
}

#endif


int main()
{
	ListPhoneInfo();

	pthread_t pRecvDiaoduData, pRecvPhoneData, pRecvIoData;

#if SMALL_VER == 1
	pthread_t pChkUDisk;
#endif

	bool	bNeedReset = false;
	DWORD	dwBeginTm = 0, dwCurTm = 0;
	int		iRand = 0;	
	byte	bytAccSta;
	struct tm pCurrentTime;

	dwBeginTm = dwCurTm = GetTickCount();
	srand( (int)time(0) );
	iRand = rand() % 60 ;		// 产生一个随机数，用于复位控制

	_InitAll();

	if( pthread_create(&pRecvDiaoduData, NULL, G_RecvDiaoduData, NULL) != 0
		||
		pthread_create(&pRecvPhoneData, NULL, G_RecvPhoneData, NULL) != 0
		||
		pthread_create(&pRecvIoData, NULL, G_DealIOThread, NULL) != 0
#if SMALL_VER == 1
		||
 		pthread_create(&pChkUDisk, NULL, G_ChkUDisk, NULL) != 0
#endif
		)
	{
		PRTMSG(MSG_ERR, "create deal thread failed: ");
		perror(" ");

		_ReleaseAll();

		return ERR_THREAD;
	}

	PRTMSG(MSG_NOR, "The ComuExe begin to run!\n");

	int i = 0;
	while(!g_bProgExit)
	{
		sleep(1);

		// 清看门狗
		if( i++ % 5 == 0 )
		{
			DogClr( DOG_COMU );
		}

		//////////////////////////////////////////////////////////////////////////
		// 定时复位
		dwCurTm = GetTickCount();

		if( dwCurTm - dwBeginTm > 48*3600*1000 )	// 若运行时间已超过48小时，则强制复位
			goto _RESET_SYSTEM;

		if( dwCurTm - dwBeginTm > 24*3600*1000 )	// 若运行时间已超过24小时，根据ACC状态、时间段来判断是否复位
		{
			IOGet(IOS_ACC, &bytAccSta);
			
			// 若ACC有效，则不复位
			if( bytAccSta == IO_ACC_ON )
				continue;

			G_GetTime(&pCurrentTime);

			// 若不在复位时间段内，则不复位（复位时间段定为每天 09:00 ~ 11:00, 随机选取一个时刻复位）
			if( pCurrentTime.tm_hour == 12 && pCurrentTime.tm_min == iRand )
				goto _RESET_SYSTEM;
			else
				continue;
		}
		else
			continue;

_RESET_SYSTEM:
		PRTMSG(MSG_NOR, "Reset Time!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		G_ResetSystem();
		break;

	}

	g_bProgExit = true;
	_ReleaseAll();

	// 等待子线程、以及其他进程退出
	sleep(15);

	PRTMSG(MSG_NOR, "ComuExe is now to quit\n");

	// system("reboot");

	return 0;
}
