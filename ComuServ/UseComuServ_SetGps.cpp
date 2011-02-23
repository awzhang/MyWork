#include "../GlobalShare/ComuServExport.h"
#include "../GlobalShare/GlobFunc.h"
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>

// int DataPush( void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb, DWORD v_dwPopSymb, BYTE v_bytLvl ); // 此方法可同时指定多个PopSymb,即接收对象
// int DataPop( void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPushSymb, DWORD v_dwPopSymb, BYTE *v_p_bytLvl );
// int DataDel( DWORD v_dwPopSymb, void* v_pCmpDataFunc );
// int DataWaitPop( BYTE v_bytPopSymb );
// int DataSkipWait( BYTE v_bytPopSymb );
// 
// int GetImpCfg( void *v_pDes, unsigned int v_uiDesSize, unsigned int v_uiGetBegin, unsigned int v_uiGetSize );
// int SetImpCfg( void *v_pSrc, unsigned int v_uiSaveBegin, unsigned int v_uiSaveSize );
// int GetSecCfg( void *v_pDes, unsigned int v_uiDesSize, unsigned int v_uiGetBegin, unsigned int v_uiGetSize );
// int SetSecCfg( void *v_pSrc, unsigned int v_uiSaveBegin, unsigned int v_uiSaveSize );
// int ResumeQianCfg( bool v_bReservTel );
// 
// int IOGet( unsigned char v_ucIOSymb, char *v_p_cIOSta );
// int IOSet( unsigned char v_ucIOSymb, char v_cIOSta,  void* v_pReserv, unsigned int v_uiReservSiz );
// int IOCfgGet( unsigned char v_ucIOSymb, void *v_pCfg, DWORD v_dwCfgSize );
// int IOIntFuncSet( unsigned char v_ucIOSymb, void* v_pIntFunc );
// 
// int GetCurGps( void* v_pGps, const unsigned int v_uiSize, BYTE v_bytGetType );
// int SetCurGps( const void* v_pGps, BYTE v_bytSetType );

#undef MSG_HEAD
#define MSG_HEAD ("UseComuServ_SetGps")

int main()
{
	char buf[2048] = { 0 };
	DWORD dwPushLen = 0;
	DWORD dwPushSymb = 0;
	BYTE bytLvl = 0;
	tm t;
	tagGPSData gps;
	int i = 0;
	do
	{
		GetLocalTime( &t );
		gps.valid = 'A';
		gps.nYear = t.tm_year + 1900;
		gps.nMonth = t.tm_mon + 1;
		gps.nDay = t.tm_yday + 1;
		gps.nHour = t.tm_hour;
		gps.nMinute = t.tm_min;
		gps.nSecond = t.tm_sec;

		PRTMSG( MSG_NOR, "Set Gps: %4d-%2d-%2d %2d:%2d:%2d\n",
			gps.nYear, gps.nMonth, gps.nDay, gps.nHour, gps.nMinute, gps.nSecond );

		i++;
		if( i > 100 ) break;
		usleep( 100000 );
	} while( !SetCurGps(&gps, GPS_REAL) );

	PRTMSG( MSG_NOR, "exit!\n" );

	exit( 0 );
}
