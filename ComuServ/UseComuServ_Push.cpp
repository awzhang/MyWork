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
#define MSG_HEAD ("UseComuServ_Push")

int main()
{
	while( 1 )
	{
		char buf[50] = { 0 };
		char tmp[30] = { 0 };
		
		static int sta_iCt = 1;
		sprintf( tmp, "%12d", sta_iCt );
		memcpy( buf + sizeof(buf) - strlen(tmp), tmp, strlen(tmp) );
		int iResult = DataPush( buf, sizeof(buf), DEV_DIAODU, DEV_UPDATE, 2 );
		PRTMSG( MSG_NOR, "Push Data: %d, Result:%d\n", sta_iCt, iResult );
		if( iResult ) break;
		sta_iCt ++;
		if( 100 == sta_iCt ) break;

// 		static BYTE sta_byt = 1;
// 		DataPush( &sta_byt, sizeof(sta_byt), DEV_DIAODU, DEV_UPDATE, 2 );
// 		PRTMSG( MSG_NOR, "Push Data: %d\n", sta_byt );
// 		sta_byt ++;
//		if( 50 == sta_byt ) break;
				
		usleep(100000);
	}
	PRTMSG( MSG_NOR, "exit!\n" );
	exit( 0 );
}
