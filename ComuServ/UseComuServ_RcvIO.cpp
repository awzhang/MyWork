#include "../GlobalShare/ComuServExport.h"
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
#define MSG_HEAD ("UseComuServ_RcvIO")

//typedef void (*PIOINTEFUNC)(unsigned int, unsigned int);

void IOIntFunc(unsigned int v_uiPin, unsigned int v_uiLvl )
{
	//PRTMSG( MSG_DBG, "Call IO Int Func: %d, %d\n", v_uiPin, v_uiLvl );
	//IOSet( IOS_APPLEDPOW, v_uiLvl ? IO_APPLEDPOW_ON : IO_APPLEDPOW_OFF, NULL, 0 );
}

int main()
{
	//unsigned char ucIOSta = 0;
	int iVal = 0;
	//tagIOCfg objCfg;
	//IOIntInit( IOS_ACC, (void*)IOIntFunc );

	char buf[100] = {0};
	while(1)
	{
		iVal ++;
		if( 1000 == iVal ) break;

		DataPush( buf, sizeof(buf), DEV_IO, DEV_QIAN, 2 );

		usleep( 1000000 );
	}

	PRTMSG( MSG_NOR, "exit!\n" );
	exit( 0 );
}
