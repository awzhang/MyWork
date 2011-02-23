#include "ComuStdAfx.h"

CInnerDataMng g_objDiaoduOrHandWorkMng;
CInnerDataMng g_objPhoneRecvDataMng;
CInnerDataMng g_objIoStaRcvMng;

CTimerMng g_objTimerMng;

CIoSta g_objIoSta;

CGpsSrc g_objGpsSrc;

CComAdjust g_objComAdjust;

CHandset g_hst;
CHstApp g_hstapp;

CDiaodu g_objDiaodu;

#if VEHICLE_TYPE == VEHICLE_M
CLightCtrl g_objLightCtrl;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
CLightCtrl2 g_objLightCtrl2;
#endif

CPhoneTD g_objPhoneTd;
CPhoneEvdo g_objPhoneEvdo;
CPhoneWcdma g_objPhoneWcdma;
CPhoneGsm g_objPhoneGsm;
CComVir	  g_objComVir;

unsigned short	g_usOilAD = 0;
int		g_iTerminalType = 0;
int		g_simcard = 1;
DWORD	g_last_heartbeat;
volatile bool	g_bProgExit = false;

bool	g_SysUpdate = false;
bool	g_AppUpdate = false;
bool	g_ExeUpdate = false;
bool	g_ImageUpdate = false;
bool	g_diskfind = false;	
bool	g_DataOutPut = false;

void RecTimeAndCnt(char *v_szDateTime, int *v_iCount, int *v_iSecond, int v_iInterval)
{
	int i;
	int iCount = 0;
	int iCurTime;
	struct tm p_objCurTime; 

	G_GetLocalTime(&p_objCurTime);
	
	if(v_iSecond)
	{
		for(i = 0; i < 100; i++)
		{
			if(v_iSecond[i] == 0)
			{
				v_iSecond[i] = iCurTime;
				break;
			}
		}
	
		for(i = 0; i < 100; i++)
		{
			if(iCurTime <= v_iSecond[i] + v_iInterval)
				iCount += 1;
			else
				v_iSecond[i] = 0;
		}
	}
	
	if(v_iCount)
		*v_iCount = iCount;
	
	if(v_szDateTime)
		sprintf(v_szDateTime, "%02d-%02d/%02d:%02d:%02d", p_objCurTime.tm_mon+1, p_objCurTime.tm_mday, p_objCurTime.tm_hour, p_objCurTime.tm_min, p_objCurTime.tm_sec);
}
