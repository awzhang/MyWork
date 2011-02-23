#ifndef _YX_ENCODE_H_
#define _YX_ENCODE_H_ 

#define PACKINT 1800 //每个视频文件时长(秒)

int MPP_Init();
int MPP_Exit();

int StartVenc();
int StopVenc();
int StartVi();
int StopVi();
int StartVencChn();
int StopVencChn();

int StartAenc();
int StopAenc();
int StartAi();
int StopAi();
int StartAencChn();
int StopAencChn();

int StartVPrev();
int StopVPrev();
int SwitchVPrev(int v_iViChn, int v_iVoChn, int v_iFlag);
int StartAPrev();
int StopAPrev();

void *AVRec(void *arg);
int StartAVRec();
int StopAVRec();

void *AVUpl(void *arg);
int StartAVUpl();
int StopAVUpl();

int AVTest(int v_iChn);

int CtrlVideoView(BYTE v_bytVvChn);

#endif
