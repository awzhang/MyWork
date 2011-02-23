#ifndef _YX_QIANSTDAFX_H_
#define _YX_QIANSTDAFX_H_

#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h> 
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h> 
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <signal.h>
#include <termios.h>
#include <semaphore.h>
#include <linux/fs.h>
#include  <linux/rtc.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h> 
#include <sys/stat.h> 
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include "../GlobalShare/ComuServExport.h"
#include "../GlobalShare/GlobFunc.h"
#include "../GlobalShare/md5.h"
#include "../GlobalShare/Md5Decrypt.h"
#include "../GlobalShare/InnerDataMng.h"
#include "yx_QianDefine.h"
#include "yx_Queue.h"
#include "yx_LinkLst.h"
#include "LN_CommonFunc.h"
#include "LN_DataMng.h"

#define SOCKSNDSMSDATA TcpSendSmsData

extern int g_MsgID;
extern volatile bool g_bProgExit;

extern CInnerDataMng g_objRecvDvrDataMng;
extern CInnerDataMng g_objRecvSmsDataMng;
extern CInnerDataMng g_objRecvComuDataMng;
extern CInnerDataMng g_objRecvIODataMng;
extern CInnerDataMng g_objTcpSendMng;

#include "../GlobalShare/TimerMng.h"
extern	CTimerMng g_objTimerMng;

#include "yx_FlashPart4Mng.h"
extern CFlashPart4Mng g_objFlashPart4Mng;

#include "yx_CarSta.h"
extern	CCarSta g_objCarSta;

#include "yx_IO.h"
extern	CQianIO	g_objQianIO;

#include "yx_Comu.h"
extern	CComu g_objComu;

#include "yx_Cfg.h"
extern	CConfig g_objCfg;

#include "yx_Sock.h"
extern	CSock g_objSock;

#include "yx_Sms.h"
extern	CSms g_objSms;

#include "yx_MonAlert.h"
extern	CMonAlert g_objMonAlert;

#include "yx_Help.h"
extern	CHelp g_objHelp;

#include "yx_Diaodu.h"
extern	CDiaodu g_objDiaodu;

#include "yx_SpecCtrl.h"
extern CSpecCtrl g_objSpecCtrl;

#include "yx_Photo.h"
extern CPhoto g_objPhoto;

#include "yx_DriverCtrlKaiTian.h"
extern CKTDriverCtrl	g_objKTDriverCtrl;

#include "yx_DeviceCtrlKaiTian.h"
extern CKTDeviceCtrl	g_objKTDeviceCtrl;

#include "yx_IrdModKaiTian.h"
extern CKTIrdMod	g_objKTIrdMod;

#if USE_LIAONING_SANQI == 1
#include "LN_Network.h"
extern CLN_Network g_objNetWork;

#include "LN_Transport.h"
extern CLN_Transprot g_objTransport;

#include "LN_Application.h"
extern CLN_Application g_objApplication;

#include "LN_Login.h"
extern CLN_Login g_objLogin;

#include "LN_Config.h"
extern CLN_Config g_objLNConfig;

#include "LN_Report.h"
extern CLN_Report g_objReport;

#include "LN_Update.h"
extern CLNDownLoad g_objLNDownLoad;

#include "LN_Photo.h"
extern CLNPhoto g_objLNPhoto;
#endif

#if USE_OIL == 3
#include "yx_Oil.h"
extern COil	g_objOil;
#endif

#if USE_COMBUS == 1
#include "yx_ComBus.h"
extern CComBus g_objComBus;
#endif

#if USE_COMBUS == 2
#include "yx_Com150TR.h"
extern CCom150TR g_objCom150TR;
#endif

#if USE_REALPOS == 1
#include "yx_RealPos.h"
extern CRealPos	g_objRealPos;
#endif

#if USE_DRIVERCTRL == 1
#include "yx_DriverCtrl.h"
extern CDriverCtrl		g_objDriverCtrl;
#endif

#if USE_JIJIA == 1
#include "yx_Jijia.h"
extern CJijia g_objJijia;
#endif

#if USE_BLK == 1
#include "yx_Blk.h"
extern	CBlk g_objBlk;
#endif

#if USE_AUTORPT == 1
#include "yx_AutoRpt.h"
extern	CAutoRpt g_objAutoRpt;
#endif

#if USE_LEDTYPE == 1
#include "yx_LedBoHai.h"
extern	CLedBoHai g_objLedBohai;
#endif

#if USE_LEDTYPE == 2
#include "yx_LedChuangLi.h"
extern	CLedChuangLi g_objLedChuangLi;
#endif

#if USE_LEDTYPE == 3
#include "yx_LedKaiTian.h"
extern	CKTLed g_objKTLed;
#endif

#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 || USE_DRVRECRPT == 11
#include "yx_DrvRecRpt.h"
extern	CDrvRecRpt g_objDrvRecRpt;
#endif

#if USE_DRVREC == 1
#include "yx_DriveRecord.h"
extern	CDriveRecord g_objDriveRecord;
#endif

#if USE_METERTYPE == 1
#include "yx_MeterMod.h"
extern	CMeterMod g_objMeter;
#endif

#if USE_METERTYPE == 2
#include "yx_MeterModKaiTian.h"
extern	CKTMeterMod g_objKTMeter;
#endif

#if USE_ACDENT == 1 || USE_ACDENT == 2
#include "yx_Acdent.h"
extern CAcdent		g_objAcdent;
#endif

#if USE_TELLIMIT == 1
#include "yx_PhoneBook.h"
extern CPhoneBook	g_objPhoneBook;
#endif

#if USE_AUTORPTSTATION == 1
#include "yx_AutoRptStationNew.h"
extern CAutoRptStationNew g_objAuoRptStationNew;
#include "yx_DownLineFile.h"
extern CDownLineFile g_objDownLine;
#endif

#endif

