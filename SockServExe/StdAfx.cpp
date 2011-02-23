#include "StdAfx.h"

CTimerMng g_objTimerMng;

volatile bool   g_bProgExit = false;
pthread_mutex_t	g_mutexFlowControl;
pthread_mutex_t g_mutexTimer;
double g_dwSockSndTotalRate = 0;

CUdpSocket g_objDvrUdp(DEV_DVR);
CUdpSocket g_objUpdateUdp(DEV_UPDATE);
CUdpSocket g_objQianUdp(DEV_QIAN);

CTcpSock g_objQianTcp(DEV_QIAN);

#if USE_VIDEO_TCP == 1
CTcpSock g_objDvrTcp(DEV_DVR);
#endif

#if WIRELESS_UPDATE_TYPE == 1
CTcpSock g_objUpdateTcp(DEV_UPDATE);
#endif

int DelSymbData(void* v_pData, void* v_pSymb, DWORD v_dwSymbSize)
{
	BYTE* pData = (BYTE*)v_pData;
	BYTE* pChn = (BYTE*)v_pSymb;
	
	if(*(pData+16) == *pChn)
		return 0;
	else
		return -1;
}
