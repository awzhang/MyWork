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
#define MSG_HEAD ("UseComuServ_SetTime")

int main()
{
	int y,mo,d,h,mi,s;
	char c;

	while(1)
	{
		scanf( "%c", &c );

		switch( c )
		{
		case 'e':
			goto _END;
			break;

		case 'y':
			scanf( "%d", &y );
			break;

		case 'm':
			scanf( "%d", &mo );
			break;

		case 'd':
			scanf( "%d", &d );
			break;

		case 'h':
			scanf( "%d", &h );
			break;

		case 'f':
			scanf( "%d", &mi );
			break;

		case 's':
			scanf( "%d", &s );
			break;

		case 't':
			{
				tm t;
				t.tm_year = y;
				t.tm_mon = mo;
				t.tm_mday = d;
				t.tm_hour = h;
				t.tm_min = mi;
				t.tm_sec = s;

				SetCurTime( &t );
				system( "date -R" );
			}
			break;

		case 'g':
			PRTMSG( MSG_NOR, "Tickcount = %d\n", int(GetTickCount()) );
			break;

		default:
			;
		}
	}

_END:
	PRTMSG( MSG_NOR, "exit!\n" );

	exit( 0 );
}
