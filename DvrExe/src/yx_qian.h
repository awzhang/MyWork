#ifndef _YX_QIAN_H_
#define _YX_QIAN_H_ 


int Deal3801(char *v_databuf, int v_datalen);
int Deal3802(char *v_databuf, int v_datalen, WORK_EVENT *v_objWorkEvent);
int Deal3803(char *v_databuf, int v_datalen);
int Deal380b(char *v_databuf, int v_datalen);
int Deal380c(char *v_databuf, int v_datalen);
BOOL ResetUploadPara(byte v_bytISlice);

void DealQianSmsData(char *v_szBuf, int v_iLen, SLEEP_EVENT *v_objSleepEvent, WORK_EVENT *v_objWorkEvent, CTRL_EVENT *v_objCtrlEvent);

#endif
