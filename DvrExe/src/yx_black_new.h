#ifndef _YX_BLACKNEW_H_
#define _YX_BLACKNEW_H_ 

#if VIDEO_BLACK_TYPE == 2

extern int UPLOAD_PACK_SIZE;		// 上传帧中的包数据长度

#define		MAX_UPLOAD_FILE_COUNT		48
#define		UPLOAD_WINUNIT					32
#define		UPLOAD_WINCOUNT					2
#define		UPLOAD_WINSIZE					(UPLOAD_WINUNIT*UPLOAD_WINCOUNT)
//#define		UPLOAD_PACK_SIZE				600		// 上传帧中的包数据长度
#define		UPLOAD_FRAME_SIZE				1100	// 上传帧的总长度
#define		C2_HEAD_SIZE					67		// A2帧的帧头长

typedef struct yx_BLACK_QUERY_PARA
{
	BYTE	m_uszChnNo;				// 通道号
	BYTE	m_uszEventType;		// 事件类型
	char	m_szDate[11];			// 查询日期
	char	m_szStartTime[9];	// 起始时间
	char	m_szEndTime[9];		// 结束时间
}BLACK_QUERY_PARA;

typedef struct tagUpLoadFileSta
{
	int 	m_iFileCount;																		// 待上传的文件个数
	char		m_szTempFileName[MAX_UPLOAD_FILE_COUNT][40];		// 待上传的临时文件名队列（不含路径）
	char		m_szRealFileName[MAX_UPLOAD_FILE_COUNT][80];		// 待上传的临时文件所对应的正式文件名（含路径）
	HI_U64	m_ulStartOffset[MAX_UPLOAD_FILE_COUNT];					// 临时文件在所对应的正式文件中的起始位置
	HI_U64	m_ulEndOffset[MAX_UPLOAD_FILE_COUNT];						// 临时文件在所对应的正式文件中的结束位置
	BYTE		m_uszFileType;																	// 正在上传的文件的类型
	HI_U64	m_ulPackNo;																			// 正在上传的包序号
	HI_U64	m_ulWinNo;																			// 正在上传的窗口序号
	HI_U64	m_ulPackTotal;																	// 正在上传的文件的总包数
	HI_U64	m_ulWinTotal;																		// 正在上传的文件的总窗口数
	int 	m_iBufCount;																		// 当前缓存中的包数
	int	m_iBufLen[UPLOAD_WINSIZE];											// 缓存中的各包的长度（因为包数据长度可能不相同）
	BYTE		m_szPackBuf[UPLOAD_WINSIZE][UPLOAD_FRAME_SIZE]; // 包数据
}UpLoadFileSta;

extern UpLoadFileSta g_objUploadFileSta;

int  Deal3809(char *v_databuf, int v_datalen);
int  Deal380A(char *v_databuf, int v_datalen);
int  Deal38D1(char *v_databuf, int v_datalen);
int  Deal38D3(char *v_databuf, int v_datalen);

int	 QueryBlackData(BLACK_QUERY_PARA v_objQueryPara, char *v_szStartTime, char *v_szEndTime, int  v_iFileCnt);
void BeginUploadFile();
BOOL UpdateUploadSta(BOOL v_bDeleteFirstFile);

void *G_BlackUploadThread(void* arg);
void Deal_ReUpload(void *arg, int len);

void *G_DealSockData(void *arg);
int StartBlack();
int StopBlack();

#endif

#endif

