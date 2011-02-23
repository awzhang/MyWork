#ifndef _YX_LN_UPDATE_H_
#define _YX_LN_UPDATE_H_

void *G_ThreadLNDownLoad(void *arg);

#define MAX_FRAME_LEN		1400

class CLNDownLoad
{
public:
	CLNDownLoad();
	~CLNDownLoad();

	int Init();
	int Release();
	void ClearPar();
	void P_ThreadDownLoad();
	bool _DeleteFile(uchar *md5);
	bool _RenewFileLst();
private:
	int  _WaitForAnswer(DWORD v_dwWaitTm, char v_szDataType, char v_szResult);

public:
	bool	m_bNetFined;		// 网络是否畅通
	bool	m_bNeedUpdate;	
	DWORD	m_dwFileSize;		// 当前正在下载的文件大小
	DWORD	m_dwDownSize;		// 已下载大小
								//
	byte	m_bytMd5[16];		// 当前正在下载的加密过的md5值

	ushort	m_ulPackageLen;
	byte	m_bytFileType;
	FileList	m_objFileList;	// 文件下载列表
	CInnerDataMng	m_objRecvMsg;				// 接收队列
private:
	pthread_t m_pThreadDown;
	bool	m_bThreadExit;		// 线程退出标志
	byte	m_bytSta;			// 状态机

	bool	m_bReturnIdl;		// 是否返回空闲状态


//	CInnerDataMng	m_objSendMsg;				// 发送队列
	byte	m_bytRecvBuf[MAX_FRAME_LEN + 10];	// 当前接收帧数据
	DWORD	m_dwRecvLen;						// 当前接收帧长度
	byte	m_bytSendBuf[MAX_FRAME_LEN + 10];	// 当前发送帧数据
	DWORD	m_dwSendLen;						// 当前发送帧长度


	
	byte	m_bytFlstNo;		// 当前下载文件在文件列表中的位置
								
	
	bool	m_bMd5ok;			// 校验结果
	
	// 当前下载
	byte	*m_pDownBuf;			// 指针
	byte	m_bytBlockSta[31];// 块状态
	ushort	m_usPackageNo;
	DWORD	m_dwCurpackLen;
	ushort 	m_usEndPackageNo;
	
	// 其他
	uint	m_timeout;			//接收节目数据超时时间标志

};

#endif

