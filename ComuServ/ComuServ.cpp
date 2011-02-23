#include "../GlobalShare/ComuServExport.h"
#include "ComuServDef.h"

#include "CfgMng.h"
#include "IO.h"
#include "Gps.h"
#include "MsgQueMng.h"
#include "TimeMng.h"
#include "Dog.h"

//CComuServ g_objComuServ;

// CMsgQueue g_objMsgQueDiaodu( int(KEY_SEM_DIAODU), int(KEY_MSG_DIAODU), int(KEY_SHM_DIAODU_1), QUEUESIZE_QIAN );
// CMsgQueue g_objMsgQueLed( int(KEY_SEM_LED), int(KEY_MSG_LED), int(KEY_SHM_LED_1), QUEUESIZE_LED );
// CMsgQueue g_objMsgQuePhone( int(KEY_SEM_PHONE), int(KEY_MSG_PHONE), int(KEY_SHM_PHONE_1), QUEUESIZE_PHONE );
// CMsgQueue g_objMsgQueQian( int(KEY_SEM_QIAN), int(KEY_MSG_QIAN), int(KEY_SHM_QIAN_1), QUEUESIZE_QIAN );
// CMsgQueue g_objMsgQueLiu( int(KEY_SEM_LIU), int(KEY_MSG_LIU), int(KEY_SHM_LIU_1), QUEUESIZE_LIU );
// CMsgQueue g_objMsgQueDvr( int(KEY_SEM_DVR), int(KEY_MSG_DVR), int(KEY_SHM_DVR_1), QUEUESIZE_DVR );
// CMsgQueue g_objMsgQueSockQian( int(KEY_SEM_SOCK_QIAN), int(KEY_MSG_SOCK_QIAN), int(KEY_SHM_SOCKQIAN_1), QUEUESIZE_SOCK_QIAN );
// CMsgQueue g_objMsgQueSockLiu( int(KEY_SEM_SOCK_LIU), int(KEY_MSG_SOCK_LIU), int(KEY_SHM_SOCKLIU_1), QUEUESIZE_SOCK_LIU );
// CMsgQueue g_objMsgQueSockDvr( int(KEY_SEM_SOCK_DVR), int(KEY_MSG_SOCK_DVR), int(KEY_SHM_SOCKDVR_1), QUEUESIZE_SOCK_DVR );
//CMsgQueue g_objMsgQueIO( int(KEY_SEM_QUEUEIO), int(KEY_MSG_QUEUEIO), int(KEY_SHM_QUEUEIO_1), QUEUESIZE_IO );

int PHONE_MODTYPE = 0;
int NETWORK_TYPE = 0;
char USB_DEV_INSMOD[128] = {0};
char USB_DEV_CHECKPATH_AT[128] = {0};
char USB_DEV_CHECKPATH_PPP[128] = {0};
char PPP_DIAL_NUM[128] = {0};
char PPP_USER_NAME[128] = {0};
char PPP_PASSWORD[128] = {0};
DWORD COMMUX_DEVPATH_AT = 0;
DWORD COMMUX_DEVPATH_PPP = 0;

CTimeMng g_objTmMng; // 放在最前

CMsgQueMng g_objMsgQueMng;
 
CCfgMng g_objCfgMng;
 
CIO g_objIO;

CGps g_objGps;

CDog g_objDog;

void DogClr( DWORD v_dwSymb )
{
	return g_objDog.DogClr(v_dwSymb);
}

DWORD DogQuery()
{
	return g_objDog.DogQuery();
}

DWORD DogQueryOnly()
{
	return g_objDog.DogQueryOnly();
}

void DogInit()
{
	return g_objDog.DogInit();
}

int DataPush( void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb, DWORD v_dwPopSymb, BYTE v_bytLvl )
{
	return g_objMsgQueMng.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_dwPopSymb, v_bytLvl );

// 	int iRet = 0;
// 	int iResult = 0;
//
// 	if( v_dwPopSymb & DEV_DIAODU )
// 	{
// 		iResult = g_objMsgQueDiaodu.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
// 		if( !iRet ) iRet = iResult;
// 	}
// 
// 	if( v_dwPopSymb & DEV_LED )
// 	{
// 		g_objMsgQueLed.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
// 		if( !iRet ) iRet = iResult;
// 	}
// 
// 	if( v_dwPopSymb & DEV_PHONE )
// 	{
// 		g_objMsgQuePhone.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
// 		if( !iRet ) iRet = iResult;
// 	}
// 
// 	if( v_dwPopSymb & DEV_QIAN )
// 	{
// 		g_objMsgQueQian.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
// 		if( !iRet ) iRet = iResult;
// 	}
// 
// 	if( v_dwPopSymb & DEV_UPDATE )
// 	{
// 		g_objMsgQueLiu.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
// 		if( !iRet ) iRet = iResult;
// 	}
// 
// 	if( v_dwPopSymb & DEV_DVR )
// 	{
// 		g_objMsgQueDvr.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
// 		if( !iRet ) iRet = iResult;
// 	}
// 
// 	if( v_dwPopSymb & DEV_SOCK_QIAN )
// 	{
// 		g_objMsgQueSockQian.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
// 		if( !iRet ) iRet = iResult;
// 	}
// 
// 	if( v_dwPopSymb & DEV_SOCK_LIU )
// 	{
// 		g_objMsgQueSockLiu.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
// 		if( !iRet ) iRet = iResult;
// 	}
// 
// 	if( v_dwPopSymb & DEV_SOCK_DVR )
// 	{
// 		g_objMsgQueSockDvr.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
// 		if( !iRet ) iRet = iResult;
// 	}

// 	if( v_dwPopSymb & DEV_IO )
// 	{
// 		g_objMsgQueIO.DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
// 		if( !iRet ) iRet = iResult;
// 	}
//
//	return iRet;
}

int DataPop( void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPushSymb, DWORD v_dwPopSymb, BYTE *v_p_bytLvl )
{
	return g_objMsgQueMng.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_dwPopSymb, v_p_bytLvl );

// 	switch( v_dwPopSymb )
// 	{
// 	case DEV_DIAODU:
// 		return g_objMsgQueDiaodu.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_p_bytLvl );
// 		break;
// 		
// 	case DEV_LED:
// 		return g_objMsgQueLed.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_p_bytLvl );
// 		break;
// 		
// 	case DEV_PHONE:
// 		return g_objMsgQuePhone.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_p_bytLvl );
// 		break;
// 		
// 	case DEV_QIAN:
// 		return g_objMsgQueQian.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_p_bytLvl );
// 		break;
// 		
// 	case DEV_UPDATE:
// 		return g_objMsgQueLiu.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_p_bytLvl );
// 		break;
// 
// 	case DEV_DVR:
// 		return g_objMsgQueDvr.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_p_bytLvl );
// 		break;
// 
// 	case DEV_SOCK_QIAN:
// 		return g_objMsgQueSockQian.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_p_bytLvl );
// 		break;
// 
// 	case DEV_SOCK_LIU:
// 		return g_objMsgQueSockLiu.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_p_bytLvl );
// 		break;
// 
// 	case DEV_SOCK_DVR:
// 		return g_objMsgQueSockDvr.DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_p_bytLvl );
// 		break;
// 		
// 	default:
// 		return ERR_PAR;
// 	}
}

int DataDel( DWORD v_dwPopSymb, void* v_pCmpDataFunc )
{
	return g_objMsgQueMng.DataDel( v_dwPopSymb, v_pCmpDataFunc );

// 	switch( v_dwPopSymb )
// 	{
// 	case DEV_DIAODU:
// 		return g_objMsgQueDiaodu.DataDel( v_pCmpDataFunc );
// 		break;
// 		
// 	case DEV_LED:
// 		return g_objMsgQueLed.DataDel( v_pCmpDataFunc );
// 		break;
// 		
// 	case DEV_PHONE:
// 		return g_objMsgQuePhone.DataDel( v_pCmpDataFunc );
// 		break;
// 		
// 	case DEV_QIAN:
// 		return g_objMsgQueQian.DataDel( v_pCmpDataFunc );
// 		break;
// 		
// 	case DEV_UPDATE:
// 		return g_objMsgQueLiu.DataDel( v_pCmpDataFunc );
// 		break;
// 
// 	case DEV_DVR:
// 		return g_objMsgQueDvr.DataDel( v_pCmpDataFunc );
// 		break;
// 
// 	case DEV_SOCK_QIAN:
// 		return g_objMsgQueSockQian.DataDel( v_pCmpDataFunc );
// 		break;
// 
// 	case DEV_SOCK_LIU:
// 		return g_objMsgQueSockLiu.DataDel( v_pCmpDataFunc );
// 		break;
// 
// 	case DEV_SOCK_DVR:
// 		return g_objMsgQueSockDvr.DataDel( v_pCmpDataFunc );
// 		break;
// 		
// 	default:
// 		return ERR_PAR;
// 	}
}

int DataWaitPop( DWORD v_dwPopSymb )
{
	return g_objMsgQueMng.DataWaitPop( v_dwPopSymb );

// 	switch( v_dwPopSymb )
// 	{
// 	case DEV_DIAODU:
// 		return g_objMsgQueDiaodu.DataWaitPop();
// 		break;
// 		
// 	case DEV_LED:
// 		return g_objMsgQueLed.DataWaitPop();
// 		break;
// 		
// 	case DEV_PHONE:
// 		return g_objMsgQuePhone.DataWaitPop();
// 		break;
// 		
// 	case DEV_QIAN:
// 		return g_objMsgQueQian.DataWaitPop();
// 		break;
// 		
// 	case DEV_UPDATE:
// 		return g_objMsgQueLiu.DataWaitPop();
// 		break;
// 		
// 	default:
// 		return ERR_PAR;
// 	}
}

int DataSkipWait( DWORD v_dwPopSymb )
{
	return g_objMsgQueMng.DataSkipWait( v_dwPopSymb );

// 	switch( v_dwPopSymb )
// 	{
// 	case DEV_DIAODU:
// 		return g_objMsgQueDiaodu.DataSkipWait();
// 		break;
// 		
// 	case DEV_LED:
// 		return g_objMsgQueLed.DataSkipWait();
// 		break;
// 		
// 	case DEV_PHONE:
// 		return g_objMsgQuePhone.DataSkipWait();
// 		break;
// 		
// 	case DEV_QIAN:
// 		return g_objMsgQueQian.DataSkipWait();
// 		break;
// 		
// 	case DEV_UPDATE:
// 		return g_objMsgQueLiu.DataSkipWait();
// 		break;
// 		
// 	default:
// 		return ERR_PAR;
// 	}
}


int GetImpCfg( void *v_pDes, unsigned int v_uiDesSize, unsigned int v_uiGetBegin, unsigned int v_uiGetSize )
{
	return g_objCfgMng.GetImpCfg( v_pDes, v_uiDesSize, v_uiGetBegin, v_uiGetSize );
}

int SetImpCfg( void *v_pSrc, unsigned int v_uiSaveBegin, unsigned int v_uiSaveSize )
{
	return g_objCfgMng.SetImpCfg( v_pSrc, v_uiSaveBegin, v_uiSaveSize );
}

int GetSecCfg( void *v_pDes, unsigned int v_uiDesSize, unsigned int v_uiGetBegin, unsigned int v_uiGetSize )
{
	return g_objCfgMng.GetSecCfg( v_pDes, v_uiDesSize, v_uiGetBegin, v_uiGetSize );
}

int SetSecCfg( void *v_pSrc, unsigned int v_uiSaveBegin, unsigned int v_uiSaveSize )
{
	return g_objCfgMng.SetSecCfg( v_pSrc, v_uiSaveBegin, v_uiSaveSize );
}

int ResumeCfg( bool v_bReservTel, bool v_bReservIP )
{
	return g_objCfgMng.ResumeCfg( v_bReservTel, v_bReservIP );
}


int IOGet( unsigned char v_ucIOSymb, unsigned char *v_p_ucIOSta )
{
	return g_objIO.IOGet( v_ucIOSymb, v_p_ucIOSta );
}

int IOSet( unsigned char v_ucIOSymb, unsigned char v_ucIOSta,  void* v_pReserv, unsigned int v_uiReservSiz )
{
	return g_objIO.IOSet( v_ucIOSymb, v_ucIOSta, v_pReserv, v_uiReservSiz );
}

int IOCfgGet( unsigned char v_ucIOSymb, void *v_pCfg, DWORD v_dwCfgSize )
{
	return g_objIO.IOCfgGet( v_ucIOSymb, v_pCfg, v_dwCfgSize );
}

int IOIntInit( unsigned char v_ucIOSymb, void* v_pIntFunc )
{
	return g_objIO.IOIntInit( v_ucIOSymb, v_pIntFunc );
}

// int IOIntFuncSet( unsigned char v_ucIOSymb, void* v_pIntFunc )
// {
// 	return g_objIO.IOIntFuncSet( v_ucIOSymb, v_pIntFunc );
// }


int GetCurGps( void* v_pGps, const unsigned int v_uiSize, BYTE v_bytGetType )
{
	return g_objGps.GetCurGps( v_pGps, v_uiSize, v_bytGetType );
}

int SetCurGps( const void* v_pGps, BYTE v_bytSetType )
{
	return g_objGps.SetCurGps( v_pGps, v_bytSetType );
}


int CrtVerFile()
{
 	int i, j;
 	FILE *pfVersion = NULL;
 	
 	for(i = 0; i < 3; i++)
 	{
 		if(access("/root/Version", F_OK) != 0)
 		{
 			if(!(pfVersion = fopen("/root/Version", "w+")))
 			{
 				PRTMSG(MSG_ERR, "Create Version File Fail\n");
 				sleep(1);
 				continue;
 			}
 			else
 			{
 				for(j = 0; j < 3; j++)
 				{
 					if(sizeof(SYSTEM_VER) != fwrite(SYSTEM_VER, 1, sizeof(SYSTEM_VER), pfVersion))
 					{
 						PRTMSG(MSG_ERR, "Write Version File Fail\n");
 						fseek(pfVersion, 0, SEEK_SET);
 						sleep(1);
 						continue;
 					}
 
 					fclose(pfVersion);
 					return 0;
 				}
 				
 				fclose(pfVersion);
 				unlink("/root/Version");
 				sleep(1);
 				continue;
 			}
 		}
 		
 		return 0;
 	}
 
 	return -1;
}

int DelVerFile()
{
// 	int i;
// 
// 	for(i = 0; i < 3; i++)
// 	{
// 		if(!access("/root/Version", F_OK))
// 		{
// 			if(unlink("/root/Version"))
// 			{
// 				PRTMSG(MSG_ERR, "Delete Version File Fail\n");
// 				sleep(1);
// 				continue;
// 			}
// 			
// 			return 0;
// 		}
// 		
// 		return 0;
// 	}
// 
 	return -1;
}

int ReadVerFile(char *v_pszVersion)
{
	int i, j;
	FILE *pfVersion = NULL;
	
	for(i = 0; i < 3; i++)
	{
		if(access("/root/Version", F_OK) == 0)
		{
			if(!(pfVersion = fopen("/root/Version", "r+")))
			{
				PRTMSG(MSG_ERR, "Open Version File Fail\n");
				sleep(1);
				continue;
			}
			else
			{
				for(j = 0; j < 3; j++)
				{
					if(sizeof(SYSTEM_VER) != fread(v_pszVersion, 1, sizeof(SYSTEM_VER), pfVersion))
					{
						PRTMSG(MSG_ERR, "Read Version File Fail\n");
						fseek(pfVersion, 0, SEEK_SET);
						sleep(1);
						continue;
					}

					fclose(pfVersion);
					return 0;
				}
				
				fclose(pfVersion);
				sleep(1);
				continue;
			}
		}
		
		return -1;
	}

	return -1;
}

void ChkSoftVer(char *v_pszVersion)
{
	char *p = NULL;

	strcpy(v_pszVersion, SOFT_VER);
	
#if VEHICLE_TYPE == VEHICLE_M
	p = strstr(v_pszVersion, "110m");
	if(p != NULL)
	{
		if(NETWORK_TYPE == NETWORK_TD)
			memcpy(p, "110mt", 5);
		else if(NETWORK_TYPE == NETWORK_EVDO)
			memcpy(p, "110mk", 5);
		else if(NETWORK_TYPE == NETWORK_WCDMA)
			memcpy(p, "110mw", 5);
	}
#endif
#if VEHICLE_TYPE == VEHICLE_V8
	p = strstr(v_pszVersion, "110v8");
	if(p != NULL)
	{
		if(NETWORK_TYPE == NETWORK_TD)
			memcpy(p, "110v8t", 6);
		else if(NETWORK_TYPE == NETWORK_EVDO)
			memcpy(p, "110v8k", 6);
		else if(NETWORK_TYPE == NETWORK_WCDMA)
			memcpy(p, "110v8w", 6);
	}
#endif
#if VEHICLE_TYPE == VEHICLE_M2
	p = strstr(v_pszVersion, "110m2");
	if(p != NULL)
	{
		if(NETWORK_TYPE == NETWORK_TD)
			memcpy(p, "110m2t", 6);
		else if(NETWORK_TYPE == NETWORK_EVDO)
			memcpy(p, "110m2k", 6);
		else if(NETWORK_TYPE == NETWORK_WCDMA)
			memcpy(p, "110m2w", 6);
	}
#endif
}

bool GetDevID( char* v_szDes, unsigned int v_uiDesSize )
{
	if( v_uiDesSize > 0 )
	{
		*v_szDes = 0;
		return true;
	}
	else
	{
		return false;
	}
}

bool GetAppVer( char* v_szDes, unsigned int v_uiDesSize )
{
	char szSoftVer[64] = {0};
	ChkSoftVer(szSoftVer);

	char szSysVer[64] = {0};
	ReadVerFile(szSysVer);
	
	char szVer[64] = {0};
	sprintf(szVer, "%s-%s", szSoftVer, szSysVer);

	if( v_uiDesSize >= strlen(szVer) )
	{
		memcpy( v_szDes, szVer, strlen(szVer) );
		return true;
	}
	else
	{
		return false;
	}
}

int SetCurTime( void* v_pTmSet ) // 传入参数应是tm类型对象的指针
{
	return g_objTmMng.SetCurTime( v_pTmSet );	
}

unsigned int GetTickCount()
{
	return g_objTmMng.GetTickCount();
}
