#ifndef _YX_DECODE_H_
#define _YX_DECODE_H_

#define FILE_BUFFER 204800
typedef struct hiFILE_BUFFER_S
{
	FILE   *pFile;
	uint  u32FileLen;
    
	uint  u32BufEndPos;
	uint  u32BufCurPos;
	uint  u32BufLen;
	BYTE   u8Buf[FILE_BUFFER];
} FILE_BUFFER_S;

typedef struct hiINDEX_BUFFER_S
{
	FILE   *pFile;
} INDEX_BUFFER_S;

struct tagVideoOutputCfg
{
	BYTE bytVoChn;
	BYTE bytVoNormal;
	BYTE bytVoMode;
};

int IFile_Init(INDEX_BUFFER_S *pIndexBuf, char *fname);
int IFile_DeInit(INDEX_BUFFER_S *pIndexBuf);
int IFile_Fseek(INDEX_BUFFER_S *pIndexBuf, HI_U64 offset);
int IFile_GetFrameIndex(INDEX_BUFFER_S *pIndexBuf, FRAME_INDEX *pAVFrameIndex);
int IFile_Rewind(INDEX_BUFFER_S *pIndexBuf);

int File_Init(FILE_BUFFER_S *pFileBuf, char *fname);
int File_DeInit(FILE_BUFFER_S *pFileBuf);
int File_Fseek(FILE_BUFFER_S *pFileBuf, HI_U64 offset);
int File_Ftell(FILE_BUFFER_S *pFileBuf, HI_U64 offset);
int File_ReadStream(FILE_BUFFER_S *pFileBuf);
int File_FindNextVSlice(FILE_BUFFER_S *pFileBuf);
int File_FindNextASlice(FILE_BUFFER_S *pFileBuf);
int File_CopyData2Head(FILE_BUFFER_S *pFileBuf);
int File_GetVSlice(FILE_BUFFER_S *pFileBuf, BYTE **ppAddr, uint *pLen);
int File_GetASlice(FILE_BUFFER_S *pFileBuf, BYTE **ppAddr, uint *pLen);
int File_Rewind(FILE_BUFFER_S *pFileBuf);

int StartVdec();
int StopVdec();
int StartVo();
int StopVo();
int SwitchVo(tag2DVOPar v_objVOutput);

int StartAdec();
int StopAdec();
int StartAo();
int StopAo();
int SwitchAo(AUDIO_SAMPLE_RATE_E v_iAoRate);

void *APlay(void * args);
void *VPlay(void * args);
void *AVPlay(void *arg);
int StartAVPlay();
int StopAVPlay();
int SwitchAVPlay(int v_iAVPlay);

int CtrlVideoOutput(tagVideoOutputCfg v_objVideoOutputCfg);

#endif
