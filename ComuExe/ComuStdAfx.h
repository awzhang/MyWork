#ifndef _COMUSTDATX_H_
#define _COMUSTDATX_H_

#define _REENTRANT

#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> 
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <signal.h>
#include <termios.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/wait.h>

#include "../GlobalShare/ComuServExport.h"
#include "../GlobalShare/GlobFunc.h"
#include "../GlobalShare/InnerDataMng.h"
#include "./iconv.h"
#include "ComuExeDef.h"
#include "StackLst.h"
#include "Sms.h"
#include "Rtc.h"

extern CInnerDataMng g_objDiaoduOrHandWorkMng;
extern CInnerDataMng g_objPhoneRecvDataMng;
extern CInnerDataMng g_objIoStaRcvMng;

#include "../GlobalShare/TimerMng.h"
extern	CTimerMng g_objTimerMng;

#include "IoSta.h"
extern CIoSta g_objIoSta;

#include "GpsSrc.h"
extern CGpsSrc g_objGpsSrc;

#include "PhoneTD.h"
extern CPhoneTD g_objPhoneTd;

#include "PhoneEvdo.h"
extern CPhoneEvdo g_objPhoneEvdo;

#include "PhoneWcdma.h"
extern CPhoneWcdma g_objPhoneWcdma;

#include "PhoneGsm.h"
extern CPhoneGsm g_objPhoneGsm;
#include "ComVir.h"
extern CComVir	 g_objComVir;

#include "ComAdjust.h"
extern CComAdjust g_objComAdjust;

#include "Handset.h"
extern CHandset	g_hst;

#include "HandApp.h"
extern CHstApp g_hstapp;

#include "DiaoDu.h"
extern CDiaodu g_objDiaodu;

#if VEHICLE_TYPE == VEHICLE_M
#include "LightCtrl.h"
extern CLightCtrl g_objLightCtrl;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
#include "LightCtrl2.h"
extern CLightCtrl2 g_objLightCtrl2;
#endif

extern unsigned short	g_usOilAD;				// 油耗AD值
extern int		g_iTerminalType;		// 0 未识别；1，手柄；2，调度屏
extern int		g_simcard;
extern DWORD	g_last_heartbeat;
extern volatile bool		g_bProgExit;			// 进程退出

extern bool		g_SysUpdate;			// 系统升级标志
extern bool		g_AppUpdate;			// 应用软件升级标志
extern bool		g_ExeUpdate;
extern bool		g_ImageUpdate;
extern bool		g_diskfind;				// 查找U盘操作
extern bool		g_DataOutPut;			// 数据导出标志

extern void RecTimeAndCnt(char *v_szDateTime, int *v_iCount = NULL, int *v_iSecond = NULL, int v_iInterval = 0);

#endif	//_COMUSTDATX_H_








