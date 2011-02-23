#ifndef _YX_DOWNLOAD_H_
#define _YX_DOWNLOAD_H_

void *G_ThreadDownLoad(void *arg);

#define MAX_FRAME_LEN		1400

class CDownLoad
{
public:
	CDownLoad();
	~CDownLoad();

	int Init();
	int Release();

	void P_ThreadDownLoad();
	void P_TmRetran();
	void AnalyseRecvData(char *v_szRecvBuf, DWORD v_dwRecvLen);
	void SendSockData();

private:
	void _SendOneFrame(char v_szTranType, char v_szDataType, char *v_szFrameBuf, int v_iFrameLen, bool v_bNeedRetran, DWORD v_dwRetranTm);
	int  _WaitForAnswer(DWORD v_dwWaitTm, char v_szTranType, char v_szDataType);

	bool _DeleteFile(DWORD v_dwID);
	bool _RenewFileLst();
	int  _ConvertBlockSta(byte *v_bytSta_in, byte *v_bytSta_out, ushort v_usCurBlockSize);

	byte _GetCrc(const byte *v_bytCarId, const int v_iLen);
	void _GetCarId(ulong &v_ulCarId);
	bool _SetTimeToRtc(struct tm* v_SetTime);

public:
	bool	m_bNetFined;		// 网络是否畅通
	bool	m_bLoginNow;		// 是否立即登陆
	bool 	m_bRtcTimeChecked;     //RTC校时标志
	bool  m_bRtcNeedChk;		//是否需要对时true:需要 false:不需要


#if WIRELESS_UPDATE_TYPE == 1
	bool	m_bNeedUpdate;		// 使用维护中心进行远程升级，是否要升级的标志
#endif

private:
	pthread_t m_pThreadDown;
	bool	m_bThreadExit;		// 线程退出标志
	byte	m_bytSta;			// 状态机
	ulong	m_ulCarId;			// 车辆ID

	bool	m_bReturnIdl;		// 是否返回空闲状态

	CInnerDataMng	m_objRecvMsg;				// 接收队列
	CInnerDataMng	m_objSendMsg;				// 发送队列
	byte	m_bytRecvBuf[MAX_FRAME_LEN + 10];	// 当前接收帧数据
	DWORD	m_dwRecvLen;						// 当前接收帧长度
	byte	m_bytSendBuf[MAX_FRAME_LEN + 10];	// 当前发送帧数据
	DWORD	m_dwSendLen;						// 当前发送帧长度

	DWORD	m_dwLastLoginTm;	// 上次登陆时刻

	FileList	m_objFileList;	// 文件下载列表
	DWORD	m_dwSoftId;
	DWORD	m_dwLineId;

	DWORD	m_dwFileId;			// 当前正在下载的文件ID
	DWORD	m_dwFileSize;		// 当前正在下载的文件大小
	DWORD	m_dwDownSize;		// 已下载大小
	DWORD	m_dwDownFrameCt;	// 已下载帧数
	byte	m_bytMd5[32];		// 当前正在下载的加密过的md5值
	byte	m_bytFlstNo;		// 当前下载文件在文件列表中的位置
	byte	m_bytFileType;
	bool	m_bMd5ok;			// 校验结果
	
	// 当前下载
	byte	*m_pAllDownBuf;			// 文件缓存指针
	byte	*m_pDownBuf;			// 指针
	ushort	m_usBlockSize;		// 块大小
	ushort	m_usFrameSize;		// 帧大小
	byte	m_bytBlockSta[1024];// 块状态
	ushort	m_usBlockNo;		// 块序号
	
	// 其他
	bool	m_sended;			//是否发送请求的标志
	DWORD	m_timeout;			//接收节目数据超时时间标志
	int		m_informed;			//发送网络故障的次数
};

#endif
