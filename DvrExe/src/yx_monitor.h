#ifndef _YX_MONITOR_H_
#define _YX_MONITOR_H_ 

extern bool g_bDeal380A;
extern int g_iDeal380A;
extern char g_szDeal380A[1024];

bool IDLESTATE(void *Event);
bool CTRLSTATE(void *Event);
bool SLEEPSTATE(void *Event);

void *G_CmdReceive(void*);
void *G_SysSwitch(void*);

#endif
