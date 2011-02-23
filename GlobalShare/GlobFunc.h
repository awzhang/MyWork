#if !defined(GLOBFUNC_H_DFNWX46AW35345DFG3466YUTF7978)
#define GLOBFUNC_H_DFNWX46AW35345DFG3466YUTF7978

#define _REENTRANT

#include "ComuServExport.h"
#include "mtd-user.h"
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#define UDISK_UP_PATH	("UDisk/part1")
#define LOCAL_SAVE		("Flash/part3")
#define BOOT_PART			("/dev/mtd/0")
#define KERN_PART			("/dev/mtd/1")
#define ROOT_PART			("/dev/mtd/2")
#define UPD_PART			("/dev/mtd/3")
#define BLK_PART			("/dev/mtd/4")
#define APP_PART			("/dev/mtd/5")
#define CFG_PART			("/dev/mtd/6")

#define CHOOSE_LC6311_PASS		"4312896311"
#define CHOOSE_MC703OLD_PASS  "4312897030"
#define CHOOSE_MC703NEW_PASS  "4312897031"
#define CHOOSE_SIM5218_PASS 	"4312895218"

typedef struct yx_linux_handle
{
	bool bValid;
	char szDirPath[1024];
	char szFileName[1024];
}LINUX_HANDLE;

typedef struct yx_linux_find_data
{
	char szFileName[1024];
	struct stat objFileStat;
}LINUX_FIND_DATA;

void ListPhoneInfo();

int DelErrLog();
void RenewMemInfo(BYTE v_arg0, BYTE v_arg1=0x00, BYTE v_arg2=0x00, BYTE v_arg3=0x00);
void G_RecordDebugInfo(char *fmt, ...);

void G_ResetSystem();
int  G_GetTime(tm *v_Time);
void G_GetLocalTime(tm *v_UtcTime);
void G_GetUTCTime(tm *v_LocalTime);
bool ThreadExit(void *Event);
int WaitForEvent(bool (*v_EventFunc)(void *Event), void *v_Event, uint v_uiWaitTick);
void PrintString(char *v_szbuf, int v_ilen);

int SYSTEM(char *v_szFormat, ...);
int ChangMtdRW(char * v_szmtd);
int ChangMtdRO(char * v_szmtd);
int EraseMtd(char * v_szmtd, char *v_szfindfile, unsigned int v_uisize);
int WriteMtd(char * v_szmtd, char *v_szfindfile, unsigned int v_uisize);

long long yx_atoi64(char *v_szbuf);
char *SPRINTF(char *v_szDes, char *v_szFormat, ...);
char *STRCAT(char *v_szDes, int v_iSrcNum, ...);
char *STRCPY(char *v_szDes, int v_iSrcNum, ...);
int STRCMP(char *v_szDes, char *v_szSrc);
LINUX_HANDLE FindFirstFile(char *v_szPath, char *v_szName, LINUX_FIND_DATA *v_objData, bool v_bRecur);
bool FindNextFile(LINUX_HANDLE *v_objHandle, LINUX_FIND_DATA *v_objData, bool v_bRecur);
int FileCopy(char *pSourFilePath, char *pTarFilePath);
int DirCopy(char *pSourDirPath, char *pTarDirPath);

int G_AppUpdate(char *v_szSrc, char *v_szDes, int v_iFlag);
int G_SysUpdate(char *v_szSrc, char *v_szDes, int v_iFlag);
bool AppIntact(char *v_szFileName);
bool G_ExeUpdate(char *v_szFileName, int v_iFlag);
bool G_ImageUpdate(char *v_szFileName, int v_iFlag);

long G_Lonlat_4CharToL( char* v_szLonlat );
double G_Lonlat_4CharToD( char* v_szLonlat );
void G_Lonlat_DTo4Char( double v_dLonlat, char* v_szLonLat );

const	double		PI						= 3.141592654;
const	double		EQU_LENPERDEGREE		= 1852 * 60; // 赤道处经纬度每度的长度 (单位:米)

DWORD Code8To6( char *v_szDesBuf, char *v_szSrcBuf, DWORD v_dwSrcLen, DWORD v_dwDesSize );
DWORD Code7To6( char *v_szDesBuf, char *v_szSrcBuf, DWORD v_dwSrcLen, DWORD v_dwDesSize );
int Code8To7(const char* szOldBuf, char* szNewBuf, const int iOldLen, const int iNewSize);
int Code6To8(const char* szOldBuf, char* szNewBuf, const int iOldLen, const int iNewSize);
int Code7To8(const char* szOldBuf, char* szNewBuf, const int iOldLen, const int iNewSize);

double G_CalGeoDis(const double x1, const double y1, const double x2, const double y2);
double G_CalGeoDis2(const double x1, const double y1, const double x2, const double y2);
double G_CalDeltAngle( double v_dAng1, double v_dAng2 );
bool JugPtInRect( long v_lLon, long v_lLat, long v_lLonMin, long v_lLatMin, long v_lLonMax, long v_lLatMax );

int Code8ToCodeX(BYTE dest[], const BYTE source[], const int sourceLen, const int x);
int CodeXToCode8(BYTE dest[], const BYTE source[], const int sourceLen, const int x);

int count_days(int y, int m, int d);

byte get_crc_gprs(const byte *buf, const int len);
byte get_crc_sms(const byte *buf, const int len);

#endif
