#ifndef _YX_UPDATESTDAFX_H_
#define _YX_UPDATESTDAFX_H_

#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h> 
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h> 
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <signal.h> 

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h> 
#include <sys/stat.h> 
#include <sys/msg.h>

#include "../GlobalShare/ComuServExport.h"
#include "../GlobalShare/md5.h"
#include "../GlobalShare/GlobFunc.h"
#include "../GlobalShare/InnerDataMng.h"
#include "../ComuServ/mtd-abi.h"
#include "../ComuServ/mtd-user.h"
#include "../ComuExe/Rtc.h"
#include "yx_UpdateDefine.h"

extern int g_iMsgId;

#include "../GlobalShare/TimerMng.h"
extern CTimerMng g_objTimerMng;

#include "yx_DownLoad.h"
extern CDownLoad g_objDownLoad;

#include "yx_Sock.h"
extern CSock g_objUpdateSock;

//bool G_SoftUpdate(byte v_bytFlstNo, char *v_szFileName, int v_iFlag);

#endif

