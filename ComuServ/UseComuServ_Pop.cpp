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
#define MSG_HEAD ("UseComuServ_Pop_1")

int main()
{
	char buf[2048] = { 0 };
	char tmp[30] = { 0 };
	int iVal = 0;
	DWORD dwPushLen = 0;
	DWORD dwPushSymb = 0;
	BYTE bytLvl = 0;

	int iCt = 0;
	while( 0 == DataWaitPop(DEV_UPDATE) )
	{		
		int iResult = 0;
		while( !(iResult = DataPop( buf, sizeof(buf), &dwPushLen, &dwPushSymb, DEV_UPDATE, &bytLvl)) )
		{
			usleep( 100000 );

			memcpy( tmp, buf + dwPushLen - 12, 12 );
			iVal = atoi( tmp );
			PRTMSG( MSG_NOR, "Pop Data: %d\n", iVal );
			if( 100 == iCt ++ ) goto _END;
			
// 			BYTE byt;
// 			memcpy( &byt, buf, sizeof(byt) );
// 			PRTMSG( MSG_NOR, "Pop Data: %d\n", byt );
// 			if( 100 == byt ) break;

		}

// 		if( iResult  )
// 		{
// 			PRTMSG( MSG_ERR, "Pop Data Err:%d\n", iResult );
// 			break;
// 		}

		usleep( 100000 );
	}

_END:
	PRTMSG( MSG_NOR, "exit!\n" );
	exit( 0 );
}
