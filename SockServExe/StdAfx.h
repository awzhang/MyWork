#ifndef _STDAFX_H_
#define _STDAFX_H_

#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h> 
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h> 
#include <dirent.h>
#include <string.h>
#include <signal.h> 

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h> 
#include <sys/stat.h> 


typedef struct linger LINGER;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

#include "../GlobalShare/ComuServExport.h"
#include "../GlobalShare/GlobFunc.h"
#include "../GlobalShare/InnerDataMng.h"

#include "TcpSock.h"
#include "UdpSock.h"

#include "../GlobalShare/TimerMng.h"
extern	CTimerMng g_objTimerMng;

enum SOCKTIMER
{
	QIANTCP_REINIT_TIMER = 1,
	LIUUDP_REINIT_TIMER,
	DVRUDP_REINIT_TIMER,
	PHOTOUDP_LOG_TIMER,
	TEST_TIMER
};

const DWORD TCP_RECONNECT_INTERVAL = 20000;		//TCP重新连接间隔
const DWORD TCP_MAX_CONNECT_TIMERS = 3;			//TCP最大重连次数

#define	LIU_UDP_LOCAL_PORT	3031
#define DVR_UDP_LOCAL_PROT	3032
#define PHO_UDP_LOCAL_PROT	3033

#define	SOCKSND_MAXRATE		1024*40		//(字节/秒)

const UINT SPEEDCTRL_INTERVAL	= 100000; // 套接字发送流量控制定时器(微妙)

extern volatile bool g_bProgExit;
extern  pthread_mutex_t	g_mutexFlowControl;
extern  pthread_mutex_t g_mutexTimer;
extern  double g_dwSockSndTotalRate;

extern	CUdpSocket g_objDvrUdp;
extern	CUdpSocket g_objUpdateUdp;
extern	CUdpSocket g_objQianUdp;
extern	CTcpSock g_objQianTcp;

#if USE_VIDEO_TCP == 1
extern  CTcpSock g_objDvrTcp;
#endif

#if WIRELESS_UPDATE_TYPE == 1
extern	CTcpSock g_objUpdateTcp;
#endif

int DelSymbData(void* v_pData, void* v_pSymb, DWORD v_dwSymbSize);

#endif


