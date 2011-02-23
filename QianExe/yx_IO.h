#ifndef _YX_IO_H_
#define _YX_IO_H_

void G_TmAccChk(void *arg, int len);

class CQianIO
{
public:
	CQianIO();
	~CQianIO();

	int Init();
	int Release();

	int DealIOMsg(char *v_szbuf, DWORD v_dwlen, BYTE v_bytSrcSymb);

	void P_TmAccChk();

	BOOL PowerOnGps( bool v_bVal );
	void StartAccChk(DWORD v_dwFstInterval);
	void StopAccChk();
	DWORD GetDeviceSta();
	void SetDevSta( bool v_bSet, DWORD v_dwVal );
	void SetLowPowSta( bool v_bVal );
	bool GetLowPowSta();
	void ConnectOilElec();
	void DisconnOilElec();
	
	BOOL IN_QueryIOSta( DWORD &v_dwIoSta );
	BOOL IN_QueryIOSpeed( DWORD &v_dwCyclePerSec );
	BOOL IN_QueryMeterCycle( DWORD &v_dwCycle );
	BOOL IN_QueryOil( BYTE &v_bytOilAD, BYTE &v_bytPowAD );
	
private:
	pthread_mutex_t m_MutexDevSta;
	
	DWORD m_dwMeterComuCycle;	// 里程脉冲累加值	
	DWORD m_dwDeviceSta;		// 仅用于一次性的IO信号,或者事后无法查询的IO信号或设备控制指令
	DWORD m_dwPowAlertTm;
	bool m_bPowLowSta;
	DWORD m_dwTmRcvDCD;			//检测到DCD信号的时间	
	DWORD m_dwAccInvalidTm;
};

// 获取实时的传感器信号 (全部传感器)（需补充)
inline BOOL CQianIO::IN_QueryIOSta( DWORD &v_dwIoSta )
{
	byte bytSta;
	IOGet(IOS_ACC, &bytSta);
	if(bytSta == IO_ACC_ON)
		v_dwIoSta |= 0x00000001;
		
	return TRUE;
}

// 获得即时速度值,脉冲/秒
inline BOOL CQianIO::IN_QueryIOSpeed( DWORD &v_dwCyclePerSec )
{
	return TRUE;
}

// 获取车台启动以来的累计里程,返回值单位:脉冲数
inline BOOL CQianIO::IN_QueryMeterCycle( DWORD &v_dwCycle )
{
	return TRUE;
}

// 获取油耗AD,电源电压AD (含归一化和取值范围转换)
inline BOOL CQianIO::IN_QueryOil( BYTE &v_bytOilAD, BYTE &v_bytPowAD )
{
	return TRUE;
}

#endif

