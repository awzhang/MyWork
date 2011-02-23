#ifndef _YX_DRVRECRPT_H_
#define _YX_DRVRECRPT_H_

#define DRVREC_SNDTYPE		2	// 1,积满一个窗口才发送; 2,每帧发送,满一个窗口等待应答

void *G_ThreadDrvRecSample(void *arg);
void  G_TmReRptDrvRecWin(void *arg, int len);
void  G_TmStopUploadDrvrec(void *arg, int len);

class CDrvRecRpt
{
public:
	CDrvRecRpt();
	~CDrvRecRpt();

	int Init();
	int Release();

	int Deal103e(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen );
	int Deal3705(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen );

	void P_ThreadDrvRecSample();
	void P_TmReRptDrvRecWin();
	void P_TmStopUploadDrvrec();

	void DrvRecToSnd(bool v_bNeedSndWin);
	void RecStaSav();
	void DealAccChg( BOOL v_blAccOn );

private:
	void _BeginDrvRecSample( BOOL v_blAccOn );
	void _StopDrvRecSample();
	void _GetDrvRecSampleCfg( tagDrvRecSampleCfg &v_objDrvRecSampleCfg );
	void _SetDrvRecSampleCfg( const tagDrvRecSampleCfg &v_objDrvRecSampleCfg  );
	
#if DRVREC_SNDTYPE == 1
	bool _IsCurSndDrvRecWinEmpty();
	void _FillCurSndDrvRecWin();
#endif
	void _ClearCurSndDrvRecWin();
	int  _MakeOneDrvrecFrame( const tagDrvSampleDatas &v_objDrvSampleDatas, const WORD v_wSampleCycle,
		char *const v_szBuf, const size_t v_sizBuf, DWORD &v_dwLen );
	void _SndDrvrecWin(bool v_bReSnd);

private:
	// 行驶记录处理线程相关
	pthread_t m_pthreadDrvSample;
	bool m_bEndDrvSample;
	bool m_bStopDrvSample;
	bool m_bForceRptDrv;
	pthread_mutex_t m_MutexDrvRec;
	
	// 行驶记录处理数据
	tagDrvRecSampleCfg m_objDrvRecSampleCfg;	// 需要互斥访问
	CInnerDataMng m_objWaitSndDrvRecMng;		// 需要互斥访问,对象自身提供了互斥访问的方法
	DWORD m_aryCurSndDrvRecFrameInfo[32][2];	// 当前发送窗口的各帧信息数组: 第1个元素是帧长度; 第2个元素是帧缓冲区指针
	DWORD m_dwDrvCurWinFstSndTime;				// 当前窗口首次发送时的时刻
	bool m_bDrvCurWinChkGprs;					// 当前窗口重传超时请求过底层检查GPRS
	BYTE m_bytCurSndDrvRecFrameNum;				// 当前窗口的帧数
	bool m_bCurSndNeedAns;						// 当前发送窗口是否要可靠传输
};


#endif // #ifndef _YX_DRVRECRPT_H_

