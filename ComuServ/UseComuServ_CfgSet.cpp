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
#define MSG_HEAD ("UseComuServ_CfgSet")

int main()
{
	//unsigned char ucIOSta = 0;
	int iVal = 0;
	tag2QServCodeCfg cfg;
	//tag1LComuCfg cfg2;
	int iCode = 1111111111;
	int iRet = 0;
	char buf[100] = { 0 };
	while(1)
	{
		memset( buf, 0, sizeof(buf) );
		sprintf( buf, "%d", iCode ++ );
		
		memcpy( cfg.m_szTaxHandtel, buf, sizeof(cfg.m_szTaxHandtel) );
		iRet = SetSecCfg( &cfg, offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(cfg) );
		PRTMSG( MSG_NOR, "Set seccfg: %s, result: %d\n", buf, iRet );

// 		memcpy( cfg2.m_szVeIDKey, buf, sizeof(cfg2.m_szVeIDKey) );
// 		iRet = SetImpCfg( &cfg2, offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) );
// 		//iRet = SetImpCfg( buf, 200, sizeof(buf) );
// 		PRTMSG( MSG_NOR, "Set impcfg: %s, result: %d\n", buf, iRet );
		

		iVal ++;
		if( 20 == iVal ) break;

		usleep( 1000000 );
	}

	PRTMSG( MSG_NOR, "exit!\n" );
	exit( 0 );
}
