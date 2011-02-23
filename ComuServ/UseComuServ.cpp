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

int main()
{
// 	scanf( "%c", &c );
// 
// 		
// 		switch( c )
// 		{
// 		case 'e':
// 			printf( "Test End\n" );
// 			exit( 0 );
// 			break;
// 			
// 		case 's':
// 			{
// 				char buf[2048] = { 0 };
// 				char tmp[30] = { 0 };
// 				static int sta_iCt = 1;
// 				sprintf( tmp, "%07d", sta_iCt );
// 				memcpy( buf + sizeof(buf) - strlen(tmp), tmp, strlen(tmp) );
// 				DataPush( buf, sizeof(buf), DEV_DIAODU, DEV_PHONE, 2 );
// 				//pfDataPush( (void*)&sta_iCt, sizeof(sta_iCt) );
// 				PRTMSG( MSG_NOR, "Push Data: %d\n", sta_iCt );
// 				sta_iCt ++;
// 			}
// 			break;
// 			
// 		case 'r':
// 			{
// 				int iResult = DataWaitPop(DEV_PHONE);
// 				if( !iResult )
// 				{
// 					char buf[2048] = { 0 };
// 					char tmp[30] = { 0 };
// 					int iVal = 0;
// 					unsigned long ulLen = 0;
// 					DWORD dwPushSymb;
// 					BYTE bytLvl = 0;
// 					DataPop( buf, sizeof(buf), &ulLen, &dwPushSymb, DEV_PHONE, &bytLvl );
// 					memcpy( tmp, buf + sizeof(buf) - 7, 7 );
// 					iVal = atoi( tmp );
// 					PRTMSG( MSG_NOR, "Pop Data: %d\n", iVal );
// 				}
// 				else if( ERR_MSGSKIPBLOCK == iResult )
// 				{
// 					PRTMSG( MSG_ERR, "Msg Skip Block\n" );
// 				}
// 			}
// 			break;
// 			
// 		case 'b':
// 			{
// 				DataSkipBlock();
// 			}
// 			break;
// 			
// 		default:
// 			;
// 		}
// 		
// 		usleep(100000);
	
	exit( 0 );
}
