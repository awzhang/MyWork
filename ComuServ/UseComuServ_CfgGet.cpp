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
#define MSG_HEAD ("UseComuServ_CfgGet")

int main()
{
	unsigned char ucIOSta = 0;
	int iVal = 0;
	int iResult = 0;
	tag2QServCodeCfg cfg;
	tag1LComuCfg cfg2;
	tag2QLedCfg objLedCfg;
	char buf[100] = { 0 };
	//PRTMSG( MSG_NOR, "Run!\n" );
	while(1)
	{
		//PRTMSG( MSG_DBG, "Loop!\n" );

		memset( buf, 0, sizeof(buf) );

// 		iResult = GetSecCfg( &cfg, sizeof(cfg), offsetof(tagSecondCfg, m_uni2QServCodeCfg.m_obj2QServCodeCfg), sizeof(cfg) );
// 		memcpy( buf, cfg.m_szTaxHandtel, sizeof(cfg.m_szTaxHandtel) );
//  		PRTMSG( MSG_NOR, "Get seccfg: %s, result: %d\n", buf, iResult );

		iResult = GetSecCfg( &objLedCfg, sizeof(objLedCfg), offsetof(tagSecondCfg, m_uni2QLedCfg.m_obj2QLedCfg), sizeof(objLedCfg) );
 		PRTMSG( MSG_NOR, "Get seccfg result: %d, m_bytLedPwrWaitTm = %d, m_usLedBlkWaitTm = %d\n", iResult, objLedCfg.m_bytLedPwrWaitTm, objLedCfg.m_usLedBlkWaitTm );
		
// 		iResult = GetImpCfg( &cfg2, sizeof(cfg2), offsetof(tagImportantCfg, m_uni1LComuCfg.m_obj1LComuCfg), sizeof(cfg2) );
// 		memcpy( buf, cfg2.m_szVeIDKey, sizeof(cfg2.m_szVeIDKey) );
// 		//iResult = GetImpCfg( buf, sizeof(buf), 200, sizeof(buf) );
// 		PRTMSG( MSG_NOR, "Get impcfg: %s, result: %d\n", buf, iResult );

		iVal ++;
		if( 50 == iVal ) break;

		usleep( 1000000 );
	}

	PRTMSG( MSG_NOR, "exit!\n" );
	exit( 0 );
}
