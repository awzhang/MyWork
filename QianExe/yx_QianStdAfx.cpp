#include "yx_QianStdAfx.h"

int		g_MsgID;
volatile bool	g_bProgExit = false;			// 程序退出标志

CInnerDataMng g_objRecvDvrDataMng;
CInnerDataMng g_objRecvSmsDataMng;
CInnerDataMng g_objRecvComuDataMng;
CInnerDataMng g_objRecvIODataMng;
CInnerDataMng g_objTcpSendMng;

CTimerMng g_objTimerMng;

CFlashPart4Mng g_objFlashPart4Mng;

CCarSta g_objCarSta;

CQianIO	g_objQianIO;

CComu g_objComu;

CConfig g_objCfg;

CSock g_objSock;

CSms g_objSms;

CMonAlert g_objMonAlert;

CHelp g_objHelp;

CDiaodu g_objDiaodu;

CSpecCtrl g_objSpecCtrl;

CPhoto g_objPhoto;

CKTDriverCtrl g_objKTDriverCtrl;

CKTDeviceCtrl	g_objKTDeviceCtrl;

CKTIrdMod	g_objKTIrdMod;

#if USE_LIAONING_SANQI == 1
CLN_Network g_objNetWork;
CLN_Transprot g_objTransport;
CLN_Application g_objApplication;
CLN_Login g_objLogin;
CLN_Config g_objLNConfig;
CLN_Report g_objReport;
CLNDownLoad g_objLNDownLoad;
CLNPhoto	g_objLNPhoto;
#endif

#if USE_OIL == 3
COil	g_objOil;
#endif

#if USE_COMBUS == 1
CComBus g_objComBus;
#endif

#if USE_COMBUS == 2
CCom150TR g_objCom150TR;
#endif

#if USE_REALPOS == 1
CRealPos	g_objRealPos;
#endif

#if USE_DRIVERCTRL == 1
CDriverCtrl		g_objDriverCtrl;
#endif

#if USE_JIJIA == 1
CJijia g_objJijia;
#endif

#if USE_BLK == 1
CBlk g_objBlk;
#endif

#if USE_AUTORPT == 1
CAutoRpt g_objAutoRpt;
#endif

#if USE_LEDTYPE == 1
CLedBoHai g_objLedBohai;
#endif

#if USE_LEDTYPE == 2
CLedChuangLi g_objLedChuangLi;
#endif

#if USE_LEDTYPE == 3
CKTLed g_objKTLed;
#endif

#if USE_DRVRECRPT == 1 || USE_DRVRECRPT == 2 || USE_DRVRECRPT == 3 || USE_DRVRECRPT == 4 || USE_DRVRECRPT == 11
CDrvRecRpt g_objDrvRecRpt;
#endif

#if USE_DRVREC == 1
CDriveRecord g_objDriveRecord;
#endif

#if USE_METERTYPE == 1
CMeterMod g_objMeter;
Queue que_up;
Queue tcprcv;
#endif

#if USE_METERTYPE == 2
CKTMeterMod g_objKTMeter;
#endif

#if USE_ACDENT == 1 || USE_ACDENT == 2
CAcdent		g_objAcdent;
#endif

#if USE_TELLIMIT == 1
CPhoneBook	g_objPhoneBook;
#endif

#if USE_AUTORPTSTATION == 1
CAutoRptStationNew g_objAuoRptStationNew;
CDownLineFile g_objDownLine;
#endif

