#ifndef _YX_IRDA_H_
#define _YX_IRDA_H_ 

typedef union 
{
  UINT irda_recv_code;
  struct 
  {
  	BYTE  irkey_reverse_code;
    BYTE  irkey_code;
    WORD 	sys_id_code;
  }irkey;
}IRKEY_INFO_T;

void DealIrdaKey(WORD sys_id_code, BYTE irkey_code, BOOL *v_bPeriodIgnore, SLEEP_EVENT *v_objSleepEvent, WORK_EVENT *v_objWorkEvent, CTRL_EVENT *v_objCtrlEvent);

int TtyTest(char *v_szTtyWrite, char *v_szTtyRead=NULL);

#endif
