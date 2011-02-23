#include "ComuStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("ComuExe-Gps:")


int SortSatInfo( const void* v_p1, const void* v_p2 )
{
	return ( *((BYTE*)v_p2 + 1) - *((BYTE*)v_p1 + 1) );
}

unsigned int G_GpsReadThread( void* v_pPar )
{
	if( !v_pPar ) return 1;
	return ((CGpsSrc*)v_pPar)->P_GpsReadThread();
}

void G_GpsSigHandler(int v_signo)
{
	g_objGpsSrc.GpsSigHandler();
}

//设置RTC时间,使用GPS时间校对RTC(格林威治时间格式)
bool CGpsSrc::_SetTimeToRtc(struct tm* v_SetTime)
{
	struct rtc_time objRtcTime;

	memset(&objRtcTime, 0, sizeof(objRtcTime));

	int iDevRtc = open("/dev/misc/rtc", O_NONBLOCK);

	if(-1 == iDevRtc)
	{
		PRTMSG(MSG_ERR,"open rtc dev err\n");
		return false;
	}
		
	objRtcTime.tm_sec = v_SetTime->tm_sec;
	objRtcTime.tm_min = v_SetTime->tm_min;
	objRtcTime.tm_hour = v_SetTime->tm_hour;
	objRtcTime.tm_mday = v_SetTime->tm_mday;
	objRtcTime.tm_mon =	v_SetTime->tm_mon;
	objRtcTime.tm_year = v_SetTime->tm_year+1900;

	if(0 != ioctl(iDevRtc, RTC_SET_TIME, &objRtcTime))
	{
		close(iDevRtc);
		iDevRtc = -1;
		return false;
	}

	PRTMSG(MSG_DBG,"set gps time to rtc\n");

	close(iDevRtc);
	iDevRtc = -1;

	return true;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGpsSrc::CGpsSrc()
{
	m_bNeedGpsOn = false;
}

CGpsSrc::~CGpsSrc()
{

}

bool CGpsSrc::InitGpsAll()
{
	m_bFinishKill = true;//初始状态设为已完成SirfNav进程的关闭
	m_bRtcTimeChecked = false;
	m_bGpsSleep = false;
	//m_bUpdateQuit = false;
	GpsSwitch = false;
	bool bRet = _InitGps();
	if(!bRet)
	{
		g_bProgExit = true;
	}
	return bRet;
}

bool CGpsSrc::ReleaseGpsAll()
{
	_ReleaseGpsSrc();
	_ReleaseGps();
}

bool CGpsSrc::_InitGps()
{
	m_iGpsSta = stInit;

#if VEHICLE_TYPE == VEHICLE_M
	if(system("insmod /dvs/extdrv/yx3510_gps.ko") != 0)
	{
		PRTMSG(MSG_ERR,"GPS模块加载失败！\n");
		return false;
	}
#endif

	if(0 != pthread_mutex_init( &m_hMutexGsv, NULL))
	{
		PRTMSG(MSG_ERR,"初始化互斥量失败！\n");
		return false;
	}

	if(0 != pthread_create(&m_pthreadGpsRead, NULL, (void *(*)(void*))G_GpsReadThread, (void *)&g_objGpsSrc) )
	{
		PRTMSG(MSG_ERR,"创建GPS读线程失败！\n");
		return false;
	}

	return true;
}

bool CGpsSrc::_ReleaseGps()
{
	pthread_mutex_destroy( &m_hMutexGsv );
	return true;
}

bool CGpsSrc::_InitGpsSrc()
{
	int iRet = -1;

	m_bGpsInit = false;	
	m_dwInvalidTime = 10000;
	m_dwGsvGetTm = 0;
	m_iSemid = -1;
	m_iShmid = -1;
	m_pShm = NULL;
	m_bGpsChildClose = false;
	
	iRet = _ForkSiRFNav();

	if(0 == iRet)
	{
		PRTMSG(MSG_ERR,"创建GPS新进程失败！\n");
		return false;
	}
	PRTMSG(MSG_DBG, "fork the SiRF proc succ\n");
	
	usleep(100000);
	
	m_iSemid = _SemInit();
	if(-1 == m_iSemid)
	{
		PRTMSG(MSG_ERR,"创建GPS信号量失败\n");
		return false;
	}
	
	m_iShmid = _ShmInit();////获得共享内存的标识符
	if(-1 == m_iShmid)
	{
		PRTMSG(MSG_NOR,"获得共享内存的标识符失败\n");
		return false;
	}
	
	m_bFinishKill = false;
	m_bGpsInit = true;

	return true;
}

bool CGpsSrc::_ReleaseGpsSrc()
{
	m_bGpsInit = false;
	if( false == _KillSirfReq() )
	{
		PRTMSG(MSG_NOR, "关闭SifvNav进程失败\n");
		g_bProgExit = true;
		return false;
	}
	else
	{
		PRTMSG(MSG_NOR, "关闭SifvNav进程成功\n");
	}

	sleep(1);

	if(-1 != m_iSemid)
	{
		semctl(m_iSemid,0,IPC_RMID);
		m_iSemid = -1;
	}

	if(-1 != m_iShmid)
	{
		//que 是否需要做失败的判断？
		shmctl(m_iShmid, IPC_RMID, NULL);
		m_iShmid = -1;
	}
	m_pShm = NULL;

	return true;
}

//创建GPS数据进程SiRFNavIIIdemo
int CGpsSrc::_ForkSiRFNav()
{
	int iRet;
	m_ChildPid = -1;
	// 	sigset_t newmask;
	// 
	// 	//信号的处理
	// 	if (SIG_ERR == G_Signal(SIGCHLD, G_GpsSigHandler) )
	// 	{
	// 		PRTMSG(MSG_ERR,"failed to set the SIG_CHLD handler function\n");
	// 		perror(" ");
	// 		return 0;
	// 	}
	
	m_pidNum = fork();
	switch(m_pidNum)
	{
	case -1:
		PRTMSG(MSG_ERR,"failed to fork a new process\n");
		perror(" ");
		return 0;
		
	case 0:
		iRet = execl(GPS_PATH,NULL);
		if (-1 == iRet)
		{
			PRTMSG(MSG_ERR,"failed to execl a new program\n");
			perror(" ");
			return 0;		
		}
		break;
		
	default:
		m_ChildPid = m_pidNum;
		sleep(2);//需留足够时间，方便进程被正确的创建
		//PRTMSG(MSG_DBG, "m_ChildPid=%d\n",m_ChildPid);
		break;
	}
//	PRTMSG(MSG_DBG, "m_ChildPid=%d\n",m_ChildPid);
	return 1;
}

void CGpsSrc::GpsSigHandler()
{
	if (true == m_bGpsChildClose)
	{
		PRTMSG(MSG_NOR,"SifrNav进程退出处理！\n");
		g_objGpsSrc._SigChldHandler();
	}
}

int CGpsSrc::_SigChldHandler()
{
	//SiRFNavIIIdemo进程停止的处理
	wait(NULL);

	PRTMSG(MSG_DBG,"sig handler\n");

	m_bFinishKill = true;
}

//SiRfNav进程退出的请求，发送信号给SiRfNav进程，SiRfNav进程会做GPS掉电的操作
bool CGpsSrc::_KillSirfReq(void)
{
	m_bGpsChildClose = true;
	m_bFinishKill = false;
	if (m_ChildPid > 0)
	{
		if (kill(m_ChildPid, SIGUSR1) != 0)
		{
			PRTMSG(MSG_ERR, "send sigusr1 signal failed\n");
			return false;		
		}
		else
		{
			PRTMSG(MSG_NOR, "send a sigusr1 signal succ\n");
			return true;	
		}	
	}
	else
	{
		PRTMSG(MSG_DBG, "KillSirfReq fail,m_ChildPid=%d",m_ChildPid);
	}
}

//信号量
int CGpsSrc::_SemInit()
{	
	int iFlag1,iFlag2,iSemKey;
	int i;
	int iSemid;
	int tmperrno;

	iSemKey=ftok(KEYPATH,'a');

	if (iSemKey == -1)
	{
		PRTMSG(MSG_ERR,"failed to get a key for IPC\n");
		return -1;
	}
	for(i=0; i<10; i++ )
	{
		iSemid=semget(iSemKey,1,0666);//create a semaphore set that only includes one semphore.

		if(-1 == iSemid)
		{
			PRTMSG(MSG_ERR,"semget failed times: %d\n",i+1);
			sleep(1);
			continue;
		}
		else
		{
			return iSemid;
		}
	}

	PRTMSG(MSG_ERR,"gps semget error\n");
	perror(" ");

	return -1;
}

//共享内存
int CGpsSrc::_ShmInit()
{
	int iShmid = -1;
	int i;
	key_t shmKey;
	int flag1, flag2;

	shmKey = ftok(KEYPATH,0);

	if (shmKey == -1)
	{
		PRTMSG(MSG_ERR,"failed to get a key from ftok(name, 0)\n");
		perror(" ");
		return -1;
	}
	for(i = 0; i<5; i++)
	{
		iShmid = shmget(shmKey,SHM_SIZE,0666 );
		if(-1 == iShmid)
		{
			PRTMSG(MSG_ERR,"shmget failed times: %d\n",i+1);
			sleep(1);
			continue;
		}
		else
		{
			return iShmid;
		}
	}

	PRTMSG(MSG_ERR,"gps shmget error\n");
	perror(" ");

	return -1;
}

//获取GPS共享内存数据
void* CGpsSrc::_GetShm(int v_iShmid)
{
	void *shared_men = (void *)0;
	shared_men =(void *)shmat(v_iShmid, NULL, 0);

	if ((void *)-1 == shared_men )
	{
		PRTMSG(MSG_ERR,"failed to map the address to the process memory space\n");
		perror(" ");
		return NULL;
	}
	return shared_men;
}

bool CGpsSrc::_SemaphoreP()	//请求资源
{
	int iRet;
	struct sembuf tagSem_b;

	tagSem_b.sem_num = 0;
	tagSem_b.sem_op = -1;
	tagSem_b.sem_flg = SEM_UNDO ; //IPC_NOWAIT,非阻塞,此处设为阻塞

	iRet = semop(m_iSemid,&tagSem_b,1);
	if (-1 == iRet)
	{
		PRTMSG(MSG_ERR,"failed to get resource!\n");
		return false;
	}
	else
	{
		return true;
	}
}

bool CGpsSrc::_SemaphoreV()	//释放资源
{
	int iRet;
	struct sembuf tagSem_b;

	tagSem_b.sem_num = 0;
	tagSem_b.sem_op = 1;
	tagSem_b.sem_flg = SEM_UNDO;
	iRet = semop(m_iSemid, &tagSem_b, 1);

	if (-1 == iRet)
	{
		PRTMSG(MSG_ERR,"failed to free resource!\n");
		return false;	
	}
	return true;
}

//获取gps原始数据
bool CGpsSrc::_GetGpsOrigData(gps_data_share* v_p_objGpsData)
{
	int i;
	int iRet;

	for(i = 0; i<5; i++)
	{
		iRet = _SemaphoreP();
		if(true != iRet)
		{
			PRTMSG(MSG_ERR,"have not got the sem\n");
			_SemaphoreV();
			sleep(1);
			continue;
		}	
		memcpy(v_p_objGpsData, m_pShm, sizeof(gps_data_share));

		_SemaphoreV();

		//判断数据的有效性，'$'
		if(0 == m_objGpsDataShm.combine_data[0].gga_data[0]) 
		{
			sleep(2);
		}
		else break;
	}

	if(i >= 5)
	{
		return false;
	}
	else return true;
}

bool CGpsSrc::_SetGpsTime( int v_Year, int v_Month, int v_Day, int v_Hour, int v_Minute, int v_Second )
{
//// 设置RTC时间时，用北京时间，不再使用格林威治时间
// 	bool bCarry = false; // 时间计算时的进位标志
// 	
// 	v_Hour += 8;
// 	if( v_Hour >= 24 )
// 	{
// 		v_Hour -= 24;
// 		bCarry = true;
// 	}
// 	
// 	v_Day += (bCarry ? 1 : 0);
// 	int nMaxDay = 31;
// 	if( 2 == v_Month )
// 	{
// 		if( 0 == v_Year % 400 
// 			|| (0 == v_Year % 4 && 0 != v_Year % 100) ) // 闰年
// 		{
// 			nMaxDay = 29;
// 		}
// 		else nMaxDay = 28;
// 	}
// 	else if( v_Month <= 7 )
// 	{
// 		if( 0 == v_Month % 2 ) nMaxDay = 30;
// 	}
// 	else
// 	{
// 		if( 1 == v_Month % 2 ) nMaxDay = 30;
// 	}
// 	if( v_Day > nMaxDay )
// 	{
// 		v_Day -= nMaxDay;
// 		bCarry = true;
// 	}
// 	else
// 	{
// 		bCarry = false;
// 	}
// 	
// 	v_Month += (bCarry ? 1 : 0);
// 	if( v_Month > 12 )
// 	{
// 		v_Month -= 12;
// 		bCarry = true;
// 	}
// 	else
// 	{
// 		bCarry = false;
// 	}
// 	
// 	v_Year += (bCarry ? 1 : 0);
//// 设置RTC时间时，用北京时间，不再使用格林威治时间


	struct tm objSetTime;
	objSetTime.tm_sec = v_Second;
	objSetTime.tm_min = v_Minute;
	objSetTime.tm_hour = v_Hour;
	objSetTime.tm_mday = v_Day;
	objSetTime.tm_mon = v_Month - 1;	//0~11
	objSetTime.tm_year = v_Year - 1900;

	if (false == m_bRtcTimeChecked)
	{
		if( true == _SetTimeToRtc(&objSetTime) )
		{
			m_bRtcTimeChecked = true;//简单使用，可不做互斥
		}
	}
	SetCurTime(&objSetTime);
	
	char sdata = 0;
	DataPush(&sdata, 1, DEV_GPS, DEV_UPDATE, LV1); //通知Update已经对过时

	return true;
}

#if GPS_TYPE == REAL_GPS
unsigned int CGpsSrc::P_GpsReadThread()
{
 	int iRet = 0;
 	GPSDATA gps(0);
 	
 	/// 循环读取GPS共享内存数据
 	char buf[GPS_BUFSIZE];
// 	struct timeval tagCurrTimeVal;//当前系统时间，gettimeofday
// 	struct timezone tagCurrTimeZone;
	unsigned int uiGpsTickChk;

 	char* szGps;
	char szSta[2];

	DWORD dwContinueCount = 0; // 移动/静止 持续次数
	BYTE bytRealMoveType = 0; // 实时状态, 移动 = 1, 静止 = 2.
	BYTE bytSndMoveType = 0; // 发送状态

	static bool sta_bDoneSetTime = false; // 是否已校时
	DWORD dwABegin; // GPS连续有效起始时刻
	DWORD dwVBegin; // GPS连续无效起始时刻
	
	DWORD dwCt = 0;
	unsigned char uszResult;
	static unsigned int uiStaWaitKillTime = GetTickCount();
	static unsigned int uiStaAccValidTime = GetTickCount();

	dwABegin = dwVBegin = GetTickCount();

	szSta[0] = BYTE( 0xf0 );

	char* pgpGga =NULL;	//GGA数据起始地址
	char* pgpRmc =NULL;	//RMC数据起始地址
	char* pgpGsa =NULL;	//GSA数据起始地址
	char* pgpVtg =NULL;	//VTG数据起始地址
	char* pgpGsvBeg =NULL;//GSV数据起始地址
	char* pTemp;
	bool bGpsInvalid = false;
	bool bGpsUnComplete = false;

	while(!g_bProgExit)
 	{
		usleep(500000);

		if (g_bProgExit)
		{
			goto _LOOP_READ_END;
		}

// 		// 升级root前，先把GPS进程关闭
// 		if( m_bUpdateQuit )
// 		{
// 			_ReleaseGpsSrc();
// 			return 0;
// 		}

		if( m_bNeedGpsOn )
		{
			m_bNeedGpsOn = false;
			if( stSleep == m_iGpsSta )
			{
				uiStaAccValidTime = GetTickCount();
				m_iGpsSta = stInit;
			}
		}

		switch(m_iGpsSta)
		{
		case stInit:
			{	
				PRTMSG(MSG_NOR,"GPS STA: stInit\n");

				// 通知QianExe，GPS退出省电状态
				char szSta = 0xf8;	// 0xf1表示退出省电
				DataPush(&szSta, 1, DEV_GPS, DEV_QIAN, LV1);

				if(false == m_bFinishKill)
				{
					if ( (GetTickCount()-uiStaWaitKillTime) > 60*1000 )
					{
						goto _LOOP_READ_END;
					}
					usleep(100000);
					continue;
				}
				
				if( !_InitGpsSrc() ) 
				{
					//g_bProgExit = true;
					G_ResetSystem();
				}

				if(true == m_bGpsInit)
				{
					m_pShm = (gps_data_share *)_GetShm(m_iShmid);//获取GPS共享内存数据
					
					if( NULL == m_pShm)
					{
						PRTMSG(MSG_ERR,"获取GPS共享内存数据失败\n");
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
						g_objDiaodu.show_diaodu("GPS模块异常");
						RenewMemInfo(0x02, 0x00, 0x00, 0x00);
#endif
						m_iGpsSta = stReset;
						break;						
					}
					sleep(1);
					m_iGpsSta = stGetData;
					PRTMSG(MSG_NOR,"stInit succ,intering stGetData!\n");
				}
				else
				{
					m_iGpsSta = stReset;
				}
			}
			break;
		case stReset:
			{
				PRTMSG(MSG_NOR,"GPS STA: stReset\n");
				
				//reset GPS 前先置uiStaWaitKillTime
				uiStaWaitKillTime = GetTickCount();

				if ( false ==_ReleaseGpsSrc() )
				{
					break;
				}

				//获取共享内存que

				m_iGpsSta = stWaitKill;
			}
			break;
			
		case stSleep:
			{	
				//PRTMSG(MSG_NOR,"GPS STA: stSleep\n");
				IOGet((byte)IOS_ACC, &uszResult);
				if( IO_ACC_OFF == uszResult || GpsSwitch)
				{
					sleep(1);
				}
				else
				{
					uiStaAccValidTime = GetTickCount();
					m_bGpsSleep = false;
					m_iGpsSta = stInit;
				}
			}
			break;

		case stWaitKill:
			{
				PRTMSG(MSG_NOR,"GPS STA: stWaitKill\n");
				
				//reset GPS 前先置uiStaWaitKillTime
				if ( (GetTickCount()-uiStaWaitKillTime) > 20*1000 )
				{
					goto _LOOP_READ_END;
				}
				usleep(200000);
				pid_t pid = waitpid(-1,NULL,WNOHANG);
				if (-1 == pid )
				{
					perror("waitpid: ");
				}
				else if ( pid == m_ChildPid )
				{
					m_bFinishKill = true;
					
					if ( true == m_bGpsSleep )
					{
						PRTMSG(MSG_NOR,"Intering stSleep !\n");
						m_iGpsSta = stSleep;

						// 通知QianExe，GPS进入省电状态
						char szSta = 0xf7;	// 0xf0表示进入省电
						DataPush(&szSta, 1, DEV_GPS, DEV_QIAN, LV1);
					}
					else
					{
						sleep(30);//gps的复位操作，复位后等待一段时间再重新初始化
						m_iGpsSta = stInit;
					}
				}
				PRTMSG(MSG_DBG, "waitpid:%d\n",pid);
			}
			break;

		case stGetData:
			{
				iRet = _GetGpsOrigData(&m_objGpsDataShm);
				if (true != iRet)
				{
					m_iGpsSta = stReset;
					pgpGga = pgpRmc = pgpGsa = pgpVtg = pgpGsvBeg =NULL;
					PRTMSG(MSG_ERR, "获取GPS原始数据失败:无法获取共享内存数据!\n");

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
					g_objDiaodu.show_diaodu("GPS模块异常");
					RenewMemInfo(0x02, 0x00, 0x00, 0x00);
#endif
					break;
				}

				pgpGga = (m_objGpsDataShm.combine_data[0]).gga_data;
				pgpRmc = (m_objGpsDataShm.combine_data[0]).rmc_data;
				pgpGsa = (m_objGpsDataShm.combine_data[0]).gsa_data;
				pgpVtg = (m_objGpsDataShm.combine_data[0]).vtg_data;
				pTemp = pgpVtg + DATA_LENGTH;
				pgpGsvBeg = strstr(pTemp, "$GPGSV");

				if(0 == pgpGsvBeg)
				{
					m_iGpsSta = stReset;
					pgpGga = pgpRmc = pgpGsa = pgpVtg = pgpGsvBeg =NULL;
					break;
				}

#if PRINT_GPS_DATA == 1
				usleep(500000);
				printf("\n");
				printf("%s", (m_objGpsDataShm.combine_data[0]).gga_data);
				printf("%s", (m_objGpsDataShm.combine_data[0]).rmc_data);
				printf("%s", (m_objGpsDataShm.combine_data[0]).gsa_data);
				printf("%s", (m_objGpsDataShm.combine_data[0]).vtg_data);
				printf("%s", (m_objGpsDataShm.combine_data[0]).gsv_data);
				printf("\n");
#endif

#if VEHICLE_TYPE == VEHICLE_M
				//使用组件GetTickCount进行时间戳检查
				uiGpsTickChk = GetTickCount();
				if( (uiGpsTickChk - m_objGpsDataShm.n1_time) > 30*1000 )//30s
				{
					PRTMSG(MSG_ERR,"GPS data invalod too long ,reset the gps\n");
					m_objGpsDataShm.n1_time = uiGpsTickChk; // 避免重复复位
					m_iGpsSta = stReset;
					break;
				}
#endif
 				/// 解析前初始化
 				gps.Init();
				bGpsInvalid = false;
				bGpsUnComplete = false;
 				
 				/// 解析RMC格式数据
 				szGps = strstr(pgpRmc,"$GPRMC,");
 				if( szGps )
 				{
 					if( !_DecodeRMCData( szGps, gps ) )
					{
						bGpsUnComplete = true;
						//continue;
					}
 				}
 				else 
				{
					bGpsUnComplete = true;
					//continue;
				}

				if( gps.valid != 'A' ) bGpsInvalid = true;

				// GPS长时间无效检查
				static unsigned int sta_uiGpsBeginInvalid = uiGpsTickChk;
				{
					if( bGpsInvalid || bGpsUnComplete )
					{
						if( uiGpsTickChk - sta_uiGpsBeginInvalid >180000 )
						{
							PRTMSG(MSG_ERR,"GPS data invalod too long ,reset the gps\n");
							sta_uiGpsBeginInvalid = uiGpsTickChk; // 避免重复复位
							m_iGpsSta = stReset;
							break;
						}
						else if( bGpsUnComplete )
						{
							continue;
						}
					}
					else
					{
						sta_uiGpsBeginInvalid = uiGpsTickChk;
					}
				}
		#ifdef GPSGSA_ENB
				/// 解析GSA格式数据
				{
					static DWORD sta_dwNoGsaCt = 0;
					
					szGps = strstr( pgpGsa, "$GPGSA," );
					if( szGps )
					{
						sta_dwNoGsaCt = 0;
						_DecodeGSAData(szGps, gps);
					}
					else continue; 
				}
		#endif

				// 解析GSV格式数据
 				szGps = strstr( pgpGsvBeg, "$GPGSV," );
 				if( szGps )
 				{
					tagSatelliteInfo objSatsInfo;
					if( _DecodeGSVData(szGps, objSatsInfo) ) // 成功解析后
					{
						pthread_mutex_lock( &m_hMutexGsv );
						m_objSatsInfo = objSatsInfo;
						m_dwGsvGetTm = GetTickCount();
						pthread_mutex_unlock( &m_hMutexGsv );
					}
					else
						PRTMSG(MSG_ERR,"Decode GSV Fail !\n");
				}

				// 定位标志若不一致则纠正,以RMC格式的数据为准
				if( 'A' != gps.valid ) gps.valid = 'V';
				if( 'A' == gps.valid && ('2' != gps.m_cFixType && '3' != gps.m_cFixType) )
					gps.m_cFixType = '2';
				else if( 'A' != gps.valid && ('2' == gps.m_cFixType || '3' == gps.m_cFixType) )
					gps.m_cFixType = '1';
 
				
				//通过GPS时间校时设置系统时间
				// 设置时间
				if( !sta_bDoneSetTime )
				{
#if CHK_VER == 1 || GPSVALID_FASTFIXTIME == 1
					if( 'A' == gps.valid )
					{
						// 校时
						if( true == _SetGpsTime( gps.nYear, gps.nMonth, gps.nDay, gps.nHour, gps.nMinute, gps.nSecond ) )
						{
							PRTMSG(MSG_DBG, "Set SysTime: %04d%02d%02d %02d:%02d:%02d\n", gps.nYear, gps.nMonth, gps.nDay, gps.nHour, gps.nMinute, gps.nSecond );
							// 通知QianExe
							BYTE byt = 0xf1;
							DataPush((char*)&byt, 1, DEV_GPS, DEV_QIAN, 2);
							
							sta_bDoneSetTime = true;
						}
					}
#else
					
					DWORD dwCur = GetTickCount();
					static DWORD sta_dwLstChk = dwCur;
					
					if( dwCur - sta_dwLstChk >= 5000 ) // 若2次GPS接收时刻超过5秒
					{
						dwABegin = dwCur; // GPS连续有效的计时重新开始计算
					}
					
					if( 'A' == gps.valid )
					{
						if( dwCur - dwABegin >= TIMEBELIV_GPSAPERIOD ) // 连续有效超过一定时间,则认为GPS时间可信,可以设置为系统时间
						{
							// 校时
							if( true == _SetGpsTime( gps.nYear, gps.nMonth, gps.nDay, gps.nHour, gps.nMinute, gps.nSecond ) )
							{
								PRTMSG(MSG_DBG, "Set SysTime: %04d%02d%02d %02d:%02d:%02d\n", gps.nYear, gps.nMonth, gps.nDay, gps.nHour, gps.nMinute, gps.nSecond );
								
								// 通知QianExe
								BYTE byt = 0xf1;
								DataPush((char*)&byt, 1, DEV_GPS, DEV_QIAN, 2);
								
								sta_bDoneSetTime = true;
							}
						}
						else
						{
							dwVBegin = dwCur; // 确保以后的GPS无效时长都可以重新计时
						}
					}
					else
					{
						if( dwCur - dwVBegin >= TIMEUNBLV_GPSVMINPERIOD ) // 若GPS连续无效超过一定时间
						{
							dwABegin = dwCur; // GPS连续有效的计时重新开始计算
						}
					}
					
					sta_dwLstChk = dwCur; // 更新上次检查时间
#endif
				}

 				/// { 格林威治->北京时区
				// 无效数据也要转换,因为可能时间是正确的 if( 'A' == gps.valid ) // 若数据有效
 				{
 					bool bCarry = false; // 时间计算时的进位标志
 
 					gps.nHour += 8;
 					if( gps.nHour >= 24 )
 					{
 						gps.nHour -= 24;
 						bCarry = true;
 					}
 
 					gps.nDay += (bCarry ? 1 : 0);
 					int nMaxDay = 31;
 					if( 2 == gps.nMonth )
 					{
 						if( 0 == gps.nYear % 400 
 							|| (0 == gps.nYear % 4 && 0 != gps.nYear % 100) ) // 闰年
 						{
 							nMaxDay = 29;
 						}
 						else nMaxDay = 28;
 					}
 					else if( gps.nMonth <= 7 )
 					{
 						if( 0 == gps.nMonth % 2 ) nMaxDay = 30;
 					}
 					else
 					{
 						if( 1 == gps.nMonth % 2 ) nMaxDay = 30;
 					}
 					if( gps.nDay > nMaxDay )
 					{
 						gps.nDay -= nMaxDay;
 						bCarry = true;
 					}
 					else
 					{
 						bCarry = false;
 					}
 
 					gps.nMonth += (bCarry ? 1 : 0);
 					if( gps.nMonth > 12 )
 					{
 						gps.nMonth -= 12;
 						bCarry = true;
 					}
 					else
 					{
 						bCarry = false;
 					}
 
 					gps.nYear += (bCarry ? 1 : 0);
 				}
 				/// } 格林威治->北京时区

		// 		if( sta_bFstValid )
		// 		{
		// 			if( 'A' == gps.valid ) // || (gps.nYear > 2007) || (2007 == gps.nYear && gps.nMonth >= 5) )
		// 			{
		// 				_SetGpsTime( BYTE(gps.nYear - 2000), BYTE(gps.nMonth), BYTE(gps.nDay), BYTE(gps.nHour),
		// 					BYTE(gps.nMinute), BYTE(gps.nSecond) );
		// 				sta_bFstValid = false;
		// 			}
		// 		}

				// 若速度不可信,则设为无效
				if( gps.speed > BELIV_MAXSPEED )
				{
					gps.valid = 'V';
					gps.m_cFixType = '1';
				}
				
				if( 'A' == gps.valid ) // 若GPS数据有效
				{
					if( gps.speed > SPEED_MOVE ) // 若现在是移动的
					{
						if( 1 != bytRealMoveType ) // 若原先不是移动的
						{				
							bytRealMoveType = 1; // 修改为移动标志
							dwContinueCount = 1; // 重置状态持续次数
						}
						else
						{
							++ dwContinueCount; // 递增该状态持续次数
						}

						// 若持续次数达到设定,且上次发送状态不是移动,发送车辆变为移动的通知
						if( SPEED_COUNT == dwContinueCount && 1 != bytSndMoveType )
						{
							szSta[1] = 1;
							PRTMSG(MSG_DBG, "Send car move msg!\n" );
							DataPush(szSta, 2, DEV_GPS, DEV_QIAN, 2);
							bytSndMoveType = 1;
						}
					}
					else if( gps.speed < SPEED_STOP ) // 若现在是静止的
					{
						if( 2 != bytRealMoveType ) // 若原先不是静止的
						{				
							bytRealMoveType = 2; // 修改为静止标志
							dwContinueCount = 1; // 重置状态持续次数
						}
						else
						{
							++ dwContinueCount; // 递增该状态持续次数
						}

						// 若持续次数达到设定,且上次发送状态不是静止,发送车辆变为静止的通知
						if( SPEED_COUNT == dwContinueCount && 2 != bytSndMoveType )
						{
							szSta[1] = 2;
							PRTMSG(MSG_DBG, "Send car silent msg!\n" );
							DataPush(szSta, 2, DEV_GPS, DEV_QIAN, 2);

							bytSndMoveType = 2;
						}
					}
				}
				else
				{
					// dwContinueCount = 0; // GPS数据无效,重置状态持续计数
					// bytRealMoveType = 0; // 清除移动/静止标志
				}

				// ACC无效时,强制让速度等于0
				IOGet((byte)IOS_ACC, &uszResult);
				if( IO_ACC_OFF == uszResult ) 
				{
					gps.speed = 0;
				}
				else
				{
					uiStaAccValidTime = GetTickCount();
				}
				
				//acc无效达20分钟，关闭SiRfNav进程，gps进入省电
				if ( (GetTickCount()-uiStaAccValidTime) > 20*60*1000 || GpsSwitch)
				{
					PRTMSG(MSG_ERR,"acc invalid too long or close Gps, gps sleep, GpsSwitch=%d\n", GpsSwitch);
					m_bGpsSleep = true;
					m_iGpsSta = stReset;
				}

				gps.m_bytMoveType = bytSndMoveType; // 与发送状态同步
				if( gps.speed <= BELIV_MAXSPEED ) // 速度可信的才保留
				{
					SetCurGps( &gps, GPS_REAL );
				}

				if( 0 == dwCt ++ % 3 )
				{
// 					PRTMSG(MSG_DBG,"Gps:%c %u:%u:%u", char(gps.valid), gps.nHour, gps.nMinute, gps.nSecond);
// 					PRTMSG(MSG_DBG, "Lon=%0.6f", gps.longitude );
// 					PRTMSG(MSG_DBG, "Lat=%0.6f", gps.latitude );
				}
 			}
			break;

		default:
			{
				PRTMSG(MSG_ERR,"GPS sta Unknown, Reset gps\n");
				m_iGpsSta = stReset;
			}
		}
	}

_LOOP_READ_END:
	g_bProgExit = true;
	return 0;
}
#elif GPS_TYPE == SIMULATE_GPS
//-----------------------------------------------------------------------------------------------------------------
// 打开模拟跑车的串口
// 返回: 打开成功或者失败
bool CGpsSrc::_mGpsComOpen()
{
	int iResult;
	int i;
	struct termios options;
	const int COMOPEN_TRYMAXTIMES = 5;

	for( i = 0; i < COMOPEN_TRYMAXTIMES; i ++ )
	{
		m_imGpsComPort = open("/dev/ttyAMA2", O_RDWR | O_NONBLOCK);
		if( -1 != m_imGpsComPort )
		{
			PRTMSG(MSG_ERR,"Open mGps com succ!\n");
			break;
		}
		
		PRTMSG(MSG_ERR,"Open Open mGps com fail!\n");
		sleep(1);
	}

	if(tcgetattr(m_imGpsComPort, &options) != 0)
	{
		perror("GetSerialAttr");
		return false;
	}
	
	options.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	options.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	options.c_oflag &= ~OPOST;
	
	cfsetispeed(&options,B9600);	//设置波特率，调度屏设置波特率为9600
	cfsetospeed(&options,B9600);

	if (tcsetattr(m_imGpsComPort,TCSANOW,&options) != 0)   
	{ 
		perror("Set mGps com Attr"); 
		return false;
	}
	
	PRTMSG(MSG_NOR,"mGps com open Succ!\n"); 
	return true;
}

unsigned int CGpsSrc::P_GpsReadThread()
{
 	int iRet = 0;
 	GPSDATA gps(0);
 	
 	/// 循环读取GPS共享内存数据
 	char buf[GPS_BUFSIZE];
// 	struct timeval tagCurrTimeVal;//当前系统时间，gettimeofday
// 	struct timezone tagCurrTimeZone;
	unsigned int uiGpsTickChk;

 	char* szGps;
	char szSta[2];

	DWORD dwContinueCount = 0; // 移动/静止 持续次数
	BYTE bytRealMoveType = 0; // 实时状态, 移动 = 1, 静止 = 2.
	BYTE bytSndMoveType = 0; // 发送状态

	static bool sta_bDoneSetTime = false; // 是否已校时
	DWORD dwABegin; // GPS连续有效起始时刻
	DWORD dwVBegin; // GPS连续无效起始时刻
	
	DWORD dwCt = 0;
	unsigned char uszResult;
	static unsigned int uiStaWaitKillTime = GetTickCount();

	dwABegin = dwVBegin = GetTickCount();

	szSta[0] = BYTE( 0xf0 );

// 	iRet = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
// 	if( 0 != iRet )
// 		PRTMSG(MSG_ERR,"ComRcv设置允许线程接收取消请求失败: %x\n",iRet);
// 	
// 	iRet = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
// 	if( 0 != iRet )
// 		PRTMSG(MSG_ERR,"ComRcv线程接收到取消请求后退出失败: %x\n", iRet);

	char* pgpGga =NULL;	//GGA数据起始地址
	char* pgpRmc =NULL;	//RMC数据起始地址
	char* pgpGsa =NULL;	//GSA数据起始地址
	char* pgpVtg =NULL;	//VTG数据起始地址
	char* pgpGsvBeg =NULL;//GSV数据起始地址
	char* pTemp;

	_mGpsComOpen();

	while(!g_bProgExit)
 	{
		sleep(1);
		
		if (g_bProgExit)
		{
			goto _LOOP_READ_END;
		}
		
		int iLen = read(m_imGpsComPort,buf,sizeof(buf));

		//PRTMSG(MSG_DBG, "mcom Recv %s\n", buf);

		buf[iLen] = 0;
		pgpGga = strstr(buf,"$GPGGA,");
		pgpRmc = strstr(buf,"$GPRMC,");
		pgpGsa = strstr(buf,"$GPGSA,");
		pgpGsvBeg = strstr(buf,"$GPGSV");

		if(NULL == pgpGga || NULL == pgpRmc || NULL == pgpGsa || NULL == pgpGsvBeg )
		{
//			PRTMSG(MSG_ERR, "pgpGga is null!\n");
			pgpGga = pgpRmc = pgpGsa = pgpVtg = pgpGsvBeg =NULL;
			continue;
		}

// 	//testc
// 		sleep(1);
// 		PRTMSG(MSG_DBG,pgpGga);
// 		PRTMSG(MSG_DBG,"gps.valid:%c, gps.speed:%d\n",gps.valid,(int)(gps.speed));
// 
	//testc

 		/// 解析前初始化
 		gps.Init();
 		
 		/// 解析RMC格式数据
 		szGps = strstr(pgpRmc,"$GPRMC,");
 		if( szGps )
 		{
 			if( !_DecodeRMCData( szGps, gps ) ) continue;
 		}
 		else 
		{
			continue;
		}
#ifdef GPSGSA_ENB
		/// 解析GSA格式数据
		{
			static DWORD sta_dwNoGsaCt = 0;
			
			szGps = strstr( pgpGsa, "$GPGSA," );
			if( szGps )
			{
				sta_dwNoGsaCt = 0;
				_DecodeGSAData(szGps, gps);
			}
			else continue; 
		}
#endif

		// 解析GSV格式数据
 		szGps = strstr( pgpGsvBeg, "$GPGSV," );
 		if( szGps )
 		{
			tagSatelliteInfo objSatsInfo;
			if( _DecodeGSVData(szGps, objSatsInfo) ) // 成功解析后
			{
				pthread_mutex_lock( &m_hMutexGsv );
				m_objSatsInfo = objSatsInfo;
				m_dwGsvGetTm = GetTickCount();
				pthread_mutex_unlock( &m_hMutexGsv );
			}
			else
				PRTMSG(MSG_ERR,"Decode GSV Fail !\n");
		}

		// 定位标志若不一致则纠正,以RMC格式的数据为准
		if( 'A' != gps.valid ) gps.valid = 'V';
		if( 'A' == gps.valid && ('2' != gps.m_cFixType && '3' != gps.m_cFixType) )
			gps.m_cFixType = '2';
		else if( 'A' != gps.valid && ('2' == gps.m_cFixType || '3' == gps.m_cFixType) )
			gps.m_cFixType = '1';

		
		//通过GPS时间校时设置系统时间
		// 设置时间
		if( !sta_bDoneSetTime )
		{
#if CHK_VER == 1 || GPSVALID_FASTFIXTIME == 1
			if( 'A' == gps.valid )
			{
				// 校时
				if( true == _SetGpsTime( gps.nYear, gps.nMonth, gps.nDay, gps.nHour, gps.nMinute, gps.nSecond ) )
				{
					PRTMSG(MSG_DBG, "Set SysTime: %04d%02d%02d %02d:%02d:%02d\n", gps.nYear, gps.nMonth, gps.nDay, gps.nHour, gps.nMinute, gps.nSecond );
					// 通知QianExe
					BYTE byt = 0xf1;
					DataPush((char*)&byt, 1, DEV_GPS, DEV_QIAN, 2);
					
					sta_bDoneSetTime = true;
				}
			}
#else
			
			DWORD dwCur = GetTickCount();
			static DWORD sta_dwLstChk = dwCur;
			
			if( dwCur - sta_dwLstChk >= 5000 ) // 若2次GPS接收时刻超过5秒
			{
				dwABegin = dwCur; // GPS连续有效的计时重新开始计算
			}
			
			if( 'A' == gps.valid )
			{
				if( dwCur - dwABegin >= TIMEBELIV_GPSAPERIOD ) // 连续有效超过一定时间,则认为GPS时间可信,可以设置为系统时间
				{
					// 校时
					if( true == _SetGpsTime( gps.nYear, gps.nMonth, gps.nDay, gps.nHour, gps.nMinute, gps.nSecond ) )
					{
						PRTMSG(MSG_DBG, "Set SysTime: %04d%02d%02d %02d:%02d:%02d\n", gps.nYear, gps.nMonth, gps.nDay, gps.nHour, gps.nMinute, gps.nSecond );
						
						// 通知QianExe
						BYTE byt = 0xf1;
						DataPush((char*)&byt, 1, DEV_GPS, DEV_QIAN, 2);
						
						sta_bDoneSetTime = true;
					}
				}
				else
				{
					dwVBegin = dwCur; // 确保以后的GPS无效时长都可以重新计时
				}
			}
			else
			{
				if( dwCur - dwVBegin >= TIMEUNBLV_GPSVMINPERIOD ) // 若GPS连续无效超过一定时间
				{
					dwABegin = dwCur; // GPS连续有效的计时重新开始计算
				}
			}
			
			sta_dwLstChk = dwCur; // 更新上次检查时间
#endif
		}

 		/// { 格林威治->北京时区
		// 无效数据也要转换,因为可能时间是正确的 if( 'A' == gps.valid ) // 若数据有效
 		{
 			bool bCarry = false; // 时间计算时的进位标志

 			gps.nHour += 8;
 			if( gps.nHour >= 24 )
 			{
 				gps.nHour -= 24;
 				bCarry = true;
 			}

 			gps.nDay += (bCarry ? 1 : 0);
 			int nMaxDay = 31;
 			if( 2 == gps.nMonth )
 			{
 				if( 0 == gps.nYear % 400 
 					|| (0 == gps.nYear % 4 && 0 != gps.nYear % 100) ) // 闰年
 				{
 					nMaxDay = 29;
 				}
 				else nMaxDay = 28;
 			}
 			else if( gps.nMonth <= 7 )
 			{
 				if( 0 == gps.nMonth % 2 ) nMaxDay = 30;
 			}
 			else
 			{
 				if( 1 == gps.nMonth % 2 ) nMaxDay = 30;
 			}
 			if( gps.nDay > nMaxDay )
 			{
 				gps.nDay -= nMaxDay;
 				bCarry = true;
 			}
 			else
 			{
 				bCarry = false;
 			}

 			gps.nMonth += (bCarry ? 1 : 0);
 			if( gps.nMonth > 12 )
 			{
 				gps.nMonth -= 12;
 				bCarry = true;
 			}
 			else
 			{
 				bCarry = false;
 			}

 			gps.nYear += (bCarry ? 1 : 0);
 		}
 		/// } 格林威治->北京时区

// 		if( sta_bFstValid )
// 		{
// 			if( 'A' == gps.valid ) // || (gps.nYear > 2007) || (2007 == gps.nYear && gps.nMonth >= 5) )
// 			{
// 				_SetGpsTime( BYTE(gps.nYear - 2000), BYTE(gps.nMonth), BYTE(gps.nDay), BYTE(gps.nHour),
// 					BYTE(gps.nMinute), BYTE(gps.nSecond) );
// 				sta_bFstValid = false;
// 			}
// 		}

		// 若速度不可信,则设为无效
		if( gps.speed > BELIV_MAXSPEED )
		{
			gps.valid = 'V';
			gps.m_cFixType = '1';
		}
		
		if( 'A' == gps.valid ) // 若GPS数据有效
		{
			if( gps.speed > SPEED_MOVE ) // 若现在是移动的
			{
				if( 1 != bytRealMoveType ) // 若原先不是移动的
				{				
					bytRealMoveType = 1; // 修改为移动标志
					dwContinueCount = 1; // 重置状态持续次数
				}
				else
				{
					++ dwContinueCount; // 递增该状态持续次数
				}

				// 若持续次数达到设定,且上次发送状态不是移动,发送车辆变为移动的通知
				if( SPEED_COUNT == dwContinueCount && 1 != bytSndMoveType )
				{
					szSta[1] = 1;
					PRTMSG(MSG_DBG, "Send car move msg!\n" );
					DataPush(szSta, 2, DEV_GPS, DEV_QIAN, 2);
					bytSndMoveType = 1;
				}
			}
			else if( gps.speed < SPEED_STOP ) // 若现在是静止的
			{
				if( 2 != bytRealMoveType ) // 若原先不是静止的
				{				
					bytRealMoveType = 2; // 修改为静止标志
					dwContinueCount = 1; // 重置状态持续次数
				}
				else
				{
					++ dwContinueCount; // 递增该状态持续次数
				}

				// 若持续次数达到设定,且上次发送状态不是静止,发送车辆变为静止的通知
				if( SPEED_COUNT == dwContinueCount && 2 != bytSndMoveType )
				{
					szSta[1] = 2;
					PRTMSG(MSG_DBG, "Send car silent msg!\n" );
					DataPush(szSta, 2, DEV_GPS, DEV_QIAN, 2);

					bytSndMoveType = 2;
				}
			}
		}
		else
		{
			// dwContinueCount = 0; // GPS数据无效,重置状态持续计数
			// bytRealMoveType = 0; // 清除移动/静止标志
		}

		// ACC无效时,强制让速度等于0
		IOGet((byte)IOS_ACC, &uszResult);
		if( IO_ACC_OFF == uszResult ) gps.speed = 0;

		gps.m_bytMoveType = bytSndMoveType; // 与发送状态同步
		if( gps.speed <= BELIV_MAXSPEED ) // 速度可信的才保留
		{
			SetCurGps( &gps, GPS_REAL );
		}

		if( 0 == dwCt ++ % 3 )
		{
// 					PRTMSG(MSG_DBG,"Gps:%c %u:%u:%u", char(gps.valid), gps.nHour, gps.nMinute, gps.nSecond);
// 					PRTMSG(MSG_DBG, "Lon=%0.6f", gps.longitude );
// 					PRTMSG(MSG_DBG, "Lat=%0.6f", gps.latitude );
		}
	}

_LOOP_READ_END:
	g_bProgExit = true;
	return 0;
}
#else
#error No gps type selected!!!
#endif	//#if GPS_TYPE == REAL_GPS


// $GPRMC,hhmmss,status,latitude,N,longitude,E,spd,cog,ddmmyy,mv,mvE,mode*cs<CR><LF>
BOOL CGpsSrc::_DecodeRMCData(const char *const szGps, GPSDATA &gps)
{
	char *szCur = const_cast<char*>(szGps);
	char *pSymbol = NULL;
	double dTemp;
	int iTemp;

	/// 比较帧头
	if( memcmp(szCur, "$GPRMC,", 7) )
	{
		return FALSE;
	}

	/// 检查校验和
	char* pEnd = strchr( szCur, '*' );
	pSymbol = strchr( szCur+1, '$' );
	if( !pEnd || (pEnd > pSymbol && pSymbol) )
		return FALSE;
	else
	{
		BYTE byt = szCur[1];
		for( pSymbol = szCur + 2; pSymbol < pEnd; pSymbol ++ )
			byt ^= *pSymbol;
		char szChk[3];
		szChk[0] = *(pEnd+1);
		szChk[1] = *(pEnd+2);
		szChk[2] = 0;
		if( byt != BYTE(strtoul(szChk, &pSymbol,16)) ) return FALSE;
	}

	// 跳过帧头
	szCur += 7;

	/// 获取时间
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol )
	{
		return FALSE;
	}
	dTemp = atof( szCur );
	gps.nHour = DWORD(dTemp) / 10000;
	if( gps.nHour > 24 || gps.nHour < 0 )
	{
		return FALSE;
	}
	gps.nMinute = DWORD(dTemp) % 10000 / 100;
	if( gps.nMinute > 59 || gps.nMinute < 0 )
	{
		return FALSE;
	}
	gps.nSecond = DWORD(dTemp) % 100;
	if( gps.nSecond > 59 || gps.nSecond < 0 )
	{
		return FALSE;
	}
	szCur = pSymbol + 1;

	/// 获取GPS有效标志
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol || 1 != pSymbol - szCur )
	{
		return FALSE;
	}
	gps.valid = BYTE(szCur[0]);
	szCur = pSymbol + 1;

	/// 获取纬度
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol )
	{
		return FALSE;
	}
	dTemp = atof( szCur );
	dTemp = DWORD(dTemp) / 100 + (dTemp - DWORD(dTemp) / 100 * 100) / 60;
	if( dTemp > 90 || dTemp < 0 )
	{
		return FALSE;
	}
	gps.latitude = dTemp;
	szCur = pSymbol + 1;

	/// 获取纬度标志
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol || pSymbol - szCur > 1)
	{
		return FALSE;
	}
	if( 1 == pSymbol - szCur )
		gps.la_hemi = BYTE(szCur[0]);
	else
		gps.la_hemi = 0;
	szCur = pSymbol + 1;

	/// 获取经度
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol )
	{
		return FALSE;
	}
	dTemp = atof( szCur );
	dTemp = DWORD(dTemp) / 100 + (dTemp - DWORD(dTemp) / 100 * 100) / 60;
	if( dTemp > 180 || dTemp < 0 )
	{
		return FALSE;
	}
	gps.longitude = dTemp;
	szCur = pSymbol + 1;

	/// 获取经度标志
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol || pSymbol - szCur > 1 )
	{
		return FALSE;
	}
	if( 1 == pSymbol - szCur )
		gps.lo_hemi = BYTE(szCur[0]);
	else
		gps.lo_hemi = 0;
	szCur = pSymbol + 1;

	/// 获取速度
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol )
	{
		return FALSE;
	}
	dTemp = atof( szCur ) * 1.852 / 3.6; // 海里/小时->公里/小时->米/秒
	if( dTemp > 250 || dTemp < 0 )
	{
		return FALSE;
	}
	gps.speed = float(dTemp);
	szCur = pSymbol + 1;

	/// 获取方向
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol )
	{
		return FALSE;
	}
	dTemp = atof( szCur );
	if( dTemp > 1000 || dTemp < 0 )
	{
		return FALSE;
	}
	gps.direction = float(dTemp);
	szCur = pSymbol + 1;

	/// 获取日期
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol )
	{
		return FALSE;
	}
	iTemp = atoi( szCur );
	gps.nDay = iTemp / 10000;
	if( gps.nDay > 31 || gps.nDay < 0 )
	{
		return FALSE;
	}
	if( 0 == gps.nDay ) gps.nDay = 1;
	gps.nMonth = iTemp % 10000 / 100;
	if( gps.nMonth > 12 || gps.nMonth < 0 )
	{
		return FALSE;
	}
	if( 0 == gps.nMonth ) gps.nMonth = 1;
	gps.nYear = iTemp % 100 + 2000; // 4位数字年份,如2007
	if( gps.nYear> 2098 || gps.nYear < 2000 )
	{
		return FALSE;
	}
	if( gps.nYear < 2001 ) gps.nYear = 2001;
	szCur = pSymbol + 1;

	// 保险起见,判断数据是否完整,以减少有误码数据的可能性
	pSymbol = strchr( szCur, ',' ); // 判断是否有完整的磁极变量数据
	if( !pSymbol ) return FALSE;
	szCur = pSymbol + 1;
	pSymbol = strchr( szCur, '*' ); // 判断是否有校验码标志
	if( !pSymbol ) return FALSE;
	szCur = pSymbol + 1;

//	szGps = szCur;
	
	return TRUE;
}

// $GPGSA,Smode,FS{,sv},PDOP,HDOP,VDOP*cs<CR><LF>
BOOL CGpsSrc::_DecodeGSAData(const char *const szGps, GPSDATA &gps)
{
	char *szCur = const_cast<char*>(szGps);
	char *pSymbol = NULL, *pEnd = NULL;

	/// 比较帧头
	if( memcmp(szCur, "$GPGSA,", 7) )
	{
		return FALSE;
	}

	/// 检查校验和
	pEnd = strchr( szCur, '*' );
	pSymbol = strchr( szCur+1, '$' );
	if( !pEnd || (pEnd > pSymbol && pSymbol) )
		return FALSE;
	else
	{
		BYTE byt = szCur[1];
		for( pSymbol = szCur + 2; pSymbol < pEnd; pSymbol ++ )
			byt ^= *pSymbol;
		char szChk[3];
		szChk[0] = *(pEnd+1);
		szChk[1] = *(pEnd+2);
		szChk[2] = 0;
		if( byt != BYTE(strtoul(szChk, &pSymbol,16)) ) return FALSE;
	}

	// 跳过帧头
	szCur += 7;

	// 跳过定位模式
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol ) return FALSE;
	szCur = pSymbol + 1;

	// 定位类型
	pSymbol = strchr( szCur, ',' );
	if( !pSymbol ) return FALSE;
	gps.m_cFixType = *(pSymbol - 1);
	if( gps.m_cFixType < 1 || gps.m_cFixType > 3 ) return FALSE;

	
	/// 后面反过来查找

	// VDOP
	for( pSymbol = pEnd - 1; ',' != *pSymbol; pSymbol -- )
		;
	if( pSymbol < szGps ) return FALSE;
	gps.m_bytVDop = BYTE( atof(pSymbol + 1) * 10 );

	// HDOP
	for( --pSymbol; ',' != *pSymbol; pSymbol -- )
		;
	if( pSymbol < szGps ) return FALSE;
	gps.m_bytHDop = BYTE( atof(pSymbol + 1) * 10 );
	
	// PDOP
	for( --pSymbol; ',' != *pSymbol; pSymbol -- )
		;
	if( pSymbol < szGps ) return FALSE;
	gps.m_bytPDop = BYTE( atof(pSymbol + 1) * 10 );

	return TRUE;
}

// $GPGSV,NoMsg,MsgNo,NoSv,{,sv,elv,az,cno}*cs<CR><LF>
BOOL CGpsSrc::_DecodeGSVData(const char *const szGps, SATELLITEINFO &info)
{
	// 注意: 测试时,若本函数中调用了show_msg,将导致不能成功显示

	BOOL blRet = FALSE;
	char *szCur = NULL;
	char *pSymbol = NULL, *pTemp1 = NULL, *pTemp2 = NULL;
	char *pStop;
	BYTE bytFrameTotal = 0, bytFrameNo = 0;
	BYTE bytSatTotal = 0;
	BYTE bytSatCt = 0;
	BYTE bytSatID = 0, bytSatCno = 0;
	BYTE bytTemp = 0;
	bool bFrameEnd = false;

 	{
//		wchar_t wbuf[GPS_BUFSIZE];
// 		if( !strstr(szGps, "$GPGSV,") )
// 
// 			return FALSE;
// 					MultiByteToWideChar(CP_CHS, 0, szGps, strlen(szGps), wbuf, 2048 );
// 					//RETAILMSG(1, ( TEXT("TIC: g_broadcast_timers 0x%x\r\n"),g_broadcast_timers ) ); 
// 					RETAILMSG(1, (wbuf) );
// 					RETAILMSG(1, (L"\n") );
// 
// 		if( !strstr(szGps, "$GPGSV,") )
// 						return FALSE;
 	}
	
	/// 比较帧头
	szCur = const_cast<char*>(szGps);
	if( memcmp(szCur, "$GPGSV,", 7) )
	{
		return FALSE;
	}

	/// 检查校验和
	char* pEnd = strchr( szCur, '*' );
	pSymbol = strchr( szCur+1, '$' );
	if( !pEnd || (pEnd > pSymbol && pSymbol) )
	{
		return FALSE;
	}
	else
	{
		BYTE byt = szCur[1];
		for( pSymbol = szCur + 2; pSymbol < pEnd; pSymbol ++ )
			byt ^= *pSymbol;
		char szChk[3];
		szChk[0] = *(pEnd+1);
		szChk[1] = *(pEnd+2);
		szChk[2] = 0;
		if( byt != BYTE(strtoul(szChk, &pSymbol,16)) ) 
		{
			return FALSE;
		}
	}

	while( szCur )
	{
		if( bytSatCt >= sizeof(info.m_arySatelliteInfo) / sizeof(info.m_arySatelliteInfo[0]) )
			break;
		// 跳过帧头
		szCur += 7;

		// 获取总帧数
		pSymbol = strchr( szCur, ',' );
		if( !pSymbol ) goto _DECODEGSV_END;
		bytTemp = BYTE( strtoul(szCur, &pStop, 10) );
		if( 0 == bytTemp || bytTemp > 8 ) goto _DECODEGSV_END;
		if( bytFrameTotal > 0 && bytFrameTotal != bytTemp ) goto _DECODEGSV_END;
		bytFrameTotal = bytTemp;
		szCur = pSymbol + 1;

		// 获取帧序号
		pSymbol = strchr( szCur, ',' );
		if( !pSymbol ) goto _DECODEGSV_END;
		bytTemp = BYTE( strtoul(szCur, &pStop, 10) );
		if( 0 == bytTemp || bytTemp > bytFrameTotal ) goto _DECODEGSV_END;
		if( 1 != bytTemp - bytFrameNo ) goto _DECODEGSV_END;
		bytFrameNo = bytTemp;
		szCur = pSymbol + 1;

		// 获取卫星数
		pSymbol = strchr( szCur, ',' );
		if( !pSymbol )
		{
			if( !bytSatTotal ) // 若从来未得到卫星数
			{
				/// 判断剩余字段是否全部为空
				pSymbol = strchr( szCur, '*' );
				if( pSymbol )
				{
					bytTemp = BYTE( strtoul(szCur, &pStop, 10) );
					if( 0 == bytTemp )
					{
						bytSatTotal = 0;
						blRet = TRUE;
					}
				}
			}
			goto _DECODEGSV_END;
		}
		bytTemp = BYTE( strtoul(szCur, &pStop, 10) );
		if( bytTemp > 32 ) goto _DECODEGSV_END;
		if( bytSatTotal && bytTemp != bytSatTotal ) goto _DECODEGSV_END;
		bytSatTotal = bytTemp;
		szCur = pSymbol + 1;

		bFrameEnd = false;
		do
		{
			// 获取卫星ID
			pSymbol = strchr( szCur, ',' );
			if( !pSymbol ) goto _DECODEGSV_END;
			bytSatID = BYTE( strtoul(szCur, &pStop, 10) );
			// (发现有ID超过32的卫星,如:42、47、120~138等(李师说的) ) if( bytSatID < 1 || bytSatID > 32 ) return FALSE;
			szCur = pSymbol + 1;

			// 获取信噪比
			pSymbol = strchr( szCur, ',' );
			if( !pSymbol ) goto _DECODEGSV_END;
			szCur = pSymbol + 1;
			pSymbol = strchr( szCur, ',' );
			if( !pSymbol ) goto _DECODEGSV_END;
			szCur = pSymbol + 1;
			pTemp1 = strchr( szCur, ',' );
			pTemp2 = strchr( szCur, '*' );
			
			if( pTemp2 && pTemp1 )
			{
				if( pTemp1 < pTemp2 ) pSymbol = pTemp1;
				if( pTemp1 > pTemp2 ) pSymbol = pTemp2;
			}
			else if( pTemp1 ) pSymbol = pTemp1;
			else if( pTemp2 ) pSymbol = pTemp2;
			else goto _DECODEGSV_END;

			if( '*' == *pSymbol ) bFrameEnd = true;
			bytSatCno = BYTE( strtoul(szCur, &pStop, 10) );
			if( bytSatCno > 99 ) goto _DECODEGSV_END;
// 			if( !strcmp("", szCur) ) bytSatCno = 0xff;
// 			else
// 			{
// 				bytSatCno = BYTE( strtoul(szCur, &pStop, 10) );
// 				if( bytSatCno > 99 )
// 				{
// 					if( bytSatCt > 0 ) goto _DECODEGSV_END;
// 					else return FALSE;
// 				}
// 			}
			szCur = pSymbol + 1;

			//if( 0xff == bytSatCt ) bytSatCt = 0;
			info.m_arySatelliteInfo[ bytSatCt ][ 0 ] = bytSatID;
			info.m_arySatelliteInfo[ bytSatCt ][ 1 ] = bytSatCno;
			bytSatCt ++;
			blRet = TRUE; // 获取一个以上就认为TRUE

			if( bytSatCt >= bytSatTotal ) goto _DECODEGSV_END;
		} while( !bFrameEnd );
		pSymbol = szCur;
		szCur = strstr( pSymbol, "$GPGSV," );
	}

_DECODEGSV_END:
	info.m_bytSatelliteCount = bytSatCt;
	if( bytSatCt > 1 )
		qsort( info.m_arySatelliteInfo, bytSatCt, sizeof(info.m_arySatelliteInfo[0]), SortSatInfo );

	if( 0 == bytSatCt )
		bytSatCt = bytSatCt;

//	szGps = szCur;
	
// 	{
// 		PRTMSG(MSG_DBG, "  共%d颗星\n", bytSatCt );
// 		for( BYTE byt = 0; byt < bytSatCt; byt ++ )
// 		{
// 			PRTMSG(MSG_DBG, "  %03d: %03d\n", info.m_arySatelliteInfo[byt][0], info.m_arySatelliteInfo[byt][1] );
// 		}
// 	}

	return blRet;
}

bool CGpsSrc::GetSatellitesInfo( SATELLITEINFO &v_objSatsInfo )
{
	bool bRet = false;

 	// 读取信息,判断是否可用
	pthread_mutex_lock( &m_hMutexGsv );
	if( 0 == m_dwGsvGetTm || GetTickCount() - m_dwGsvGetTm > 6000 )
		bRet = false;
	else 
	{
		v_objSatsInfo = m_objSatsInfo;
		bRet = true;
	}
 	pthread_mutex_unlock( &m_hMutexGsv );

	return bRet;
}
