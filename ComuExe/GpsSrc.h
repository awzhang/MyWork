#ifndef _GPS_H_
#define _GPS_H_

unsigned int G_GpsReadThread( void* v_pPar );

class CGpsSrc  
{
public:
	CGpsSrc();
	virtual ~CGpsSrc();

	bool GetSatellitesInfo( SATELLITEINFO &v_objSatsInfo );
	unsigned int P_GpsReadThread();
	bool InitGpsAll();
	bool ReleaseGpsAll();
	void GpsSigHandler();
	bool GpsSwitch;	
private:
	bool _ReleaseGps();
	bool _InitGps();
	bool _InitGpsSrc();
	bool _ReleaseGpsSrc();
	int  _SemInit();
	bool _SemaphoreP();
	bool _SemaphoreV();
	int  _ShmInit();
	int  _ForkSiRFNav();
	int  _SigChldHandler();

	void *_GetShm(int v_iShmid);
	BOOL _DecodeGSVData(const char *const szGps, SATELLITEINFO &info);
	BOOL _DecodeGSAData(const char *const szGps, GPSDATA &gps);
	BOOL _DecodeRMCData(const char *const szGps, GPSDATA &gps);
	bool _KillSirfReq();
	bool _GetGpsOrigData(gps_data_share* v_p_objGpsData);
	bool _SetTimeToRtc(struct tm* v_SetTime);
	bool _SetGpsTime( int v_Year, int v_Month, int v_Day, int v_Hour, int v_Minute, int v_Second );
	
public:
	//bool m_bUpdateQuit;		//升级root前，关闭GPS进程
	bool m_bNeedGpsOn;

private:
	SATELLITEINFO m_objSatsInfo;
	gps_data_share m_objGpsDataShm;

#if GPS_TYPE == SIMULATE_GPS
	int m_imGpsComPort;
	bool _mGpsComOpen();
#endif

	pthread_t	m_pthreadGpsRead;
	pthread_mutex_t m_hMutexGsv;
	struct timeval m_objRecentTime;
	
	DWORD	m_dwInvalidTime;
	DWORD	m_dwGsvGetTm;
	int		m_iSemid;
	int		m_iShmid;
	void	*m_pShm;
	
	bool	m_bGpsInit;//未进行互斥操作
	volatile int m_iGpsSta;

	pid_t m_pidNum;
	pid_t m_ChildPid;
	volatile bool m_bGpsChildClose;
	volatile bool m_bFinishKill;
	volatile bool m_bGpsSleep;
	bool m_bRtcTimeChecked;//RTC校时标志
};

#endif // #ifndef _GPS_H_

