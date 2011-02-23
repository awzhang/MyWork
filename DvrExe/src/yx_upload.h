#ifndef _YX_UPLOAD_H_
#define _YX_UPLOAD_H_

#define		UPLOAD_WIN_SIZE  8	// 窗口大小

typedef struct tagDataWinDow
{
	HI_U64	m_ulWinNo;								// 本窗口的窗口号
	bool	m_bISlice[UPLOAD_WIN_SIZE];				// 是否I帧的标志
	int		m_iBufLen[UPLOAD_WIN_SIZE];				// 各数据包的长度
	BYTE	m_uszDataBuf[UPLOAD_WIN_SIZE][1500];	// 数据
}DataWinDow;

typedef struct tagDataUploadSta
{
	int		m_iCurWinNo;				// 当前所要填充的窗口
	DataWinDow	m_objDataWin[32];			// 数据
}DataUploadSta;

extern DataUploadSta	g_objDataUploadSta[8];
extern tag1PComuCfg g_objComuCfg;

void *G_DataUpload(void*);
int	Deal3835(char *v_databuf, int v_datalen);
int	Deal3836(char *v_databuf, int v_datalen);

#endif

