#ifndef _YX_LED_H_
#define _YX_LED_H_

void *G_ThreadLedRead(void* arg);
void *G_ThreadLedwork(void* arg);

class CLedBoHai
{
public:
	CLedBoHai();
	~CLedBoHai();

	int  Init();
	int  Release();
	bool ComOpen();
	bool ComClose();

	void P_ThreadLedRead();
	void P_ThreadLedWork();	
	int  Deal0x39(BYTE v_bytDataType,char *v_szData, DWORD v_dwDataLen);
	bool DealCenterMsg();
	bool SendCenterMsg(BYTE cmd, char* param, int len);

	void LedTurnToWork();
	void LedTurnToSleep();

	void DealComData_V1_2(char *v_szRecv, int v_iRecvLen );
	void DealComData_V2_1(char *v_szRecv, int v_iRecvLen );
	void DealComData_V2_2(char *v_szRecv, int v_iRecvLen );
	int  SendComData_V1_2(char* cmd, char* param, int len);
	int  SendComData_V2_1(char* cmd, char* param, int len);
	int  SendComData_V2_2(char* cmd, char* param, int len);
	bool SndCmdWaitRes( char* m_szCmd, char* v_szCmdCont, int v_iCmdContLen,  BYTE v_bytSndTimes, DWORD v_dwWaitPridPerTime, char* v_szRes = NULL, int v_iResSize = 0, int* v_p_iResLen = NULL );

	int  GetCurTime(tagCurTime& v_time);
	void AdjustTime();
	bool SaveLedConfig();	
	void SendMailID();
	void EnableLedAlarm();
	void CancelLedAlarm();
	
	bool insert(char* param, int len);
	bool delete_all();
	bool delete_msg(char* param, int len);
	bool display(char* param, int len);
	bool exit_disp();
	int  list_all(char* outbuf);
	int  query_id(ushort id, char* outbuf);
	bool show(char* param, int len);
	bool exit_show();
	int  query_time(char* outbuf);
	bool set_time(char* param, int len);
	bool set_wait(BYTE sec);
	bool alarm(char* param, int len);
	bool exit_alarm();
	int  query_ver(char* outbuf);
	int  query_state(char * outbuf);
	bool bright_ctrl(char* param, int len);
	bool black();
	bool light();
	bool reset();
	bool set_led(char* param, int len);
	byte updd(char* param, int len);
	bool pwon();
	bool optm(char* param, int len);	
	
	int LedTest( char* v_szData, DWORD v_dwDataLen );
	void PrintData( int v_iMsgType, char* v_pData, int v_iDataLen );

	void _DeTranData(char *v_szBuf, int &v_iBufLen);

#if SAVE_LEDDATA == 1
	void save_log(char *v_szData, int len);
#endif
	
	void build_table16( unsigned short aPoly );
	unsigned short CRC_16( unsigned char * aData, unsigned long aSize );
	
public:
	Queue		m_objCenterCmdQue;		// 中心发送的命令
	Queue		m_objLocalCmdQue;		// 车台发送的命令
//	Queue		m_objReadComQue;		// 串口返回的数据
	CInnerDataMng	m_objReadComMng;

	BYTE		m_bytVeSta;			// 车辆状态：0初始化；1静止通知；2移动通知
	DWORD		m_dwVeStaTick;		// 车辆状态时刻
	BYTE		m_bytLedCfg;		// LED配置

private:
	HANDLE		m_hComPort;			// 串口句柄
	pthread_t	m_pthreadRead;		//LED读取线程
	pthread_t	m_pthreadWork;		//LED工作线程

	bool		m_bEvtExit;
	bool		m_bEvtWork;

	char		m_szRecvFrameBuf[256];
	int			m_iRecvFrameLen;

	BYTE		m_bytCurSta;		// 当前工作状态

	tagLedMenu	m_objLedMenu;		//led节目单
	tagQLEDStat m_objLedStat;		//led工作状态
	tagCurTime	m_objCurtime;		//GPS时间
	tagMail		m_objMail;			//LED中信箱序列	
	tagLedUpdtStat	m_objUpSta;		//升级状态	
	
	bool		m_bAccValid;
	bool		m_bAccTurnOn;		// 表示ACC点火的动作， 不表示ACC的状态
	int			m_iAccSta;			// ACC的状态
	DWORD		m_dwAccToOnTm;

	char		m_szUpgrdReqBuf[100];
	int			m_iUpgrdReqBufLen;
	DWORD		m_dwReNewTick;			//更新节目单时刻
	DWORD		m_dwUpgrdSndTm;			// 最后一个请求新升级包发出的时刻
	WORD		m_wUpgrdSndTimes;		// 请求新升级包的次数
	BYTE		m_bytPowerOnWaitTm;		// LED上电等待时间 
	ushort		m_usBlackWaitTm;		// 黑屏等待时间

	ushort		m_usToSavLedSetID;
	ushort		m_usTURNID;				// 转场信息ID
	ushort		m_usNTICID;				// 通知信息ID

	bool		m_md5ok;				// 校验结果
	char		m_szCarKey[16];			// 车台密钥
	uchar		m_md5[32];				// 加密过的md5值
	CMd5		cmd5;					// 获取下载文件实际的md5值	

	ushort		m_cnCRC_CCITT;
	ulong		table_CRC[256];

#if SAVE_LEDDATA == 1
	int			m_logtype;		//日志类型,1:车台发给LED;2:LED发给车台;3:车台发给前置机;4:前置机发给车台
#endif
};


#endif	// #ifndef _YX_LED_H_



