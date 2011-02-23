#include "ComuStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("ComuExe-ComVir:")

const unsigned char r_crctable[256] = {		//reversed, 8-bit, poly=0x07 
	0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 
		0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B, 
		0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 
		0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67, 
		0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 
		0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43, 
		0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 
		0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F, 
		0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 
		0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B, 
		0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 
		0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17, 
		0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 
		0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33, 
		0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 
		0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F, 
		0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 
		0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B, 
		0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 
		0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87, 
		0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 
		0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3, 
		0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 
		0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF, 
		0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 
		0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB, 
		0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 
		0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7, 
		0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 
		0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3, 
		0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 
		0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF 
};


void *G_RecvPhoneDataThread(void *arg)
{
	g_objComVir.P_RecvPhoneDataThread();
}

void *G_RecvAtMuxDataThread(void *arg)
{
	g_objComVir.P_RecvAtMuxDataThread();
}

void *G_RecvPppMuxDataThread(void *arg)
{
	g_objComVir.P_RecvPppMuxDataThread();
}


CComVir::CComVir()
{
	m_FdMstAt = -1;	//at通道主设备伪终端句柄
	m_FdMstPpp = -1;	//ppp通道主设备
//	m_FdMstMng = -1;	//管理通道主设备
	
	m_FdSlvAt = -1;	//at通道从设备伪终端句柄
	m_FdSlvPpp = -1;	//ppp通道从设备
	m_FdSlvMng = -1;	//ppp通道从设备
	
	memset(m_szSlvPathAt, 0, sizeof(m_szSlvPathAt));//at通道从设备路径
	memset(m_szSlvPathPpp, 0 ,sizeof(m_szSlvPathPpp));//ppp通道从设备
	memset(m_szSlvPathMng, 0, sizeof(m_szSlvPathMng));//mng通道从设备

	m_FdComPhyPort = -1;
	
	m_bComInMux = false;
	m_bKillRecvThread = false;
	m_gsmtype = PHONE_MODTYPE_BENQ;
	m_bModuleVerHigh = false;

	memset(m_CommBuffer, 0 ,sizeof(m_CommBuffer));
	m_CommBufLen = 0;
	memset(m_ChannelSend, 0 ,sizeof(m_ChannelSend));
}

CComVir::~CComVir()
{
}

//函数功能：	初始化
//线程安全：	否
//调用方式：	ComVir初始化调用
bool CComVir::_InitStatic()
{
#if VEHICLE_TYPE == VEHICLE_M
	IOSet(IOS_HUBPOW, IO_HUBPOW_ON, NULL, 0);			//HUB,正式版本不需控制
#endif

	_GsmPowOn();
	sleep(6);
	// 	_GsmHardReset();
	// 	sleep(40);
	
	// 	PRTMSG(MSG_DBG, "begin\n");//testc
	// 	sleep(10000);//testc
	
	if(0 != pthread_mutex_init( &m_MutexWrite, NULL))
	{
		PRTMSG(MSG_ERR,"初始化互斥量失败！\n");
		return false;
	}
}

//函数功能：	初始化
//线程安全：	否
//调用方式：	ComVir初始化调用
bool CComVir::InitComVir()
{
	m_FdMstAt = _ComMuxOpen((char *)m_szSlvPathAt);	
	sleep(1);	
	m_FdMstPpp = _ComMuxOpen((char *)m_szSlvPathPpp);
//	m_FdMstMng = _ComMuxOpen((char *)m_szSlvPathMng);

//	PRTMSG(MSG_DBG, "m_FdMstAt:%d,m_FdMstPpp:%d,m_FdMstMng:%d\n",m_FdMstAt,m_FdMstPpp,m_FdMstMng);
//	PRTMSG(MSG_DBG, "PathAt:%s,PathPpp:%s,PathMng:%s\n",m_szSlvPathAt,m_szSlvPathPpp,m_szSlvPathMng);

	if (   -1==m_FdMstAt  || 0==strlen(m_szSlvPathAt)
		|| -1==m_FdMstPpp || 0==strlen(m_szSlvPathPpp) 
	//	|| -1==m_FdMstMng || 0==strlen(m_szSlvPathMng)
		)
	{
		PRTMSG(MSG_DBG, "get com mux handle err\n");
		goto _INITMUX_FAIL;
	}

	if(false == _ComPhyOpen())
	{
		PRTMSG(MSG_DBG, "get com phy handle err\n");
		goto _INITMUX_FAIL;
	}
	
	pthread_t pthPhoneComPhyRcv;
	if( 0 != pthread_create(&pthPhoneComPhyRcv, NULL, G_RecvPhoneDataThread, NULL) )
	{
		PRTMSG(MSG_ERR, "create phone comread thread failed: ");
		perror(" ");
		goto _INITMUX_FAIL;
	}
	
	if(false == _InitMux())
	{
		PRTMSG(MSG_DBG, "init mux fail\n");
		goto _INITMUX_FAIL;
	}
	PRTMSG(MSG_DBG, "init mux SUCC\n");
	
	pthread_t pthRcvFromAtMux, pthRcvFromPppMux;
	if( 0 != pthread_create(&pthPhoneComPhyRcv, NULL, G_RecvAtMuxDataThread, NULL)
		|| 0 != pthread_create(&pthPhoneComPhyRcv, NULL, G_RecvPppMuxDataThread, NULL))
	{
		PRTMSG(MSG_ERR, "create phone comread thread failed: ");
		perror(" ");
		goto _INITMUX_FAIL;
	}

	return true;

_INITMUX_FAIL:
	//g_bProgExit = true;
	G_ResetSystem();
	return false;
}

//函数功能：	初始化
//线程安全：	否
//调用方式：	ComVir初始化调用，相关资源需释放后调用
int CComVir::Init()
{
	_InitStatic();
	InitComVir();
}

//函数功能：	ComVir注销
//线程安全：	否
//调用方式：	ComVir释放调用
int CComVir::Release()
{
	pthread_mutex_destroy( &m_MutexWrite );
}

//函数功能：	打开设备文件
//线程安全：	是
//调用方式：	非第一次调用注意相关资源提前释放
//打开手机模块串口
bool CComVir::_ComPhyOpen()
{
	int iResult;
	int i;
	struct termios options;
	const int COMOPEN_TRYMAXTIMES = 5;
	
	for( i = 0; i < COMOPEN_TRYMAXTIMES; i ++ )
	{
		m_FdComPhyPort = open("/dev/ttyAMA2", O_RDWR);
		if( -1 != m_FdComPhyPort )
		{
			break;
		}
		
		PRTMSG(MSG_ERR,"Open Open phone com fail!\n");
		sleep(1);
	}

	if (i>=COMOPEN_TRYMAXTIMES)
	{
		return false;
	}
	
	PRTMSG(MSG_NOR,"set the port attr!\n"); 
	if(tcgetattr(m_FdComPhyPort, &options) != 0)
	{
		perror("GetSerialAttr");
		return false;
	}
	
	options.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	options.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	options.c_oflag &= ~OPOST;

// 	options.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
// 	options.c_oflag &= ~(ONLCR | OCRNL);
// 	options.c_lflag &= ~ (ICANON | ECHO | ECHOE | ISIG);

	
	cfsetispeed(&options,B115200);	//设置波特率
	cfsetospeed(&options,B115200);
	
	if (tcsetattr(m_FdComPhyPort,TCSANOW,&options) != 0)   
	{ 
		perror("Set com Attr"); 
		return false;
	}
	
	PRTMSG(MSG_NOR,"phone com open Succ!\n"); 
	return true;
}

//函数功能：	获取路径
//线程安全：	是
//调用方式：	根据有效的主设备句柄获取
//获取从设备路径
//fdMst：主设备句柄
//return：路径指针
char* CComVir::ptsname(int fdMst)
{
	int numSlvMinor;
	static char pts_name[16];
	if (ioctl(fdMst, TIOCGPTN, &numSlvMinor)<0)
	{
		return NULL;
	}

	sprintf(pts_name, "/dev/pts/%d",numSlvMinor);
//	PRTMSG(MSG_DBG, "pts_name:%s\n",pts_name);
	return pts_name;
}

//函数功能：	属性操作
//线程安全：	是
//调用方式：	
//权限修改
int CComVir::grantpt(int fdm)
{
	char *pts_name;
	pts_name = ptsname(fdm);
	return (chmod(pts_name, S_IRUSR | S_IWUSR | S_IWGRP));
}

//函数功能：	属性操作
//线程安全：	是
//调用方式：	
int CComVir::unlockpt(int fdm)
{
	int lock = 0;
	return (ioctl(fdm, TIOCSPTLCK, &lock));
}

//函数功能：	打开设备文件
//线程安全：	否
//调用方式：	非第一次调用注意相关资源提前释放
int CComVir::_ComMuxOpen(char* v_pSlavePath)
{
	char* pSlavePath = NULL;
	int fdMst = open("/dev/ptmx", O_RDWR | O_NONBLOCK);
	if (-1 == fdMst)
	{
		perror("ComVir open at master handle fail\n");
		return -1;
	}

	struct termios options;
	
	if(tcgetattr(fdMst, &options) != 0)
	{
		perror("ComVir GetSerialAttr");
		return -1;
	}
	
	options.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	options.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	options.c_oflag &= ~OPOST;
	
	if (tcsetattr(fdMst,TCSANOW,&options) != 0)   
	{
		perror("ComVir Set com Attr"); 
		return -1;
	}
	grantpt(fdMst);
	unlockpt(fdMst);

	pSlavePath = ptsname(fdMst);
	if (NULL == pSlavePath)
	{
		perror("get ptsname err: ");
		return -1;
	}
	memcpy(v_pSlavePath, pSlavePath, strlen(pSlavePath) );
	//PRTMSG(MSG_DBG, "SlavePath:%s\n",v_pSlavePath);

	return fdMst;
}

//函数功能：	获取复用通道设备路径
//线程安全：	是
//调用方式：	COMMUX_DEVPATH_AT或COMMUX_DEVPATH_PPP
bool CComVir::GetComMuxPath(DWORD v_pathSymb, char* v_pPath, DWORD v_PathLen)
{
	if ( (v_pathSymb == COMMUX_DEVPATH_AT) && (strlen(m_szSlvPathAt)) )
	{
		memcpy( v_pPath, (char *)m_szSlvPathAt, min(strlen(m_szSlvPathAt), v_PathLen) );
		return true;
	}
	else if ( (v_pathSymb == COMMUX_DEVPATH_PPP) && (strlen(m_szSlvPathPpp)) )
	{
		memcpy( v_pPath, m_szSlvPathPpp, min(strlen(m_szSlvPathPpp), v_PathLen) );
		return true;
	}
	else
	{
		return false;
	}
}

//函数功能：	线程函数
//线程安全：	否
//调用方式：	read阻塞或非阻塞
//接收和处理手机模块串口的数据
int CComVir::P_RecvPhoneDataThread()
{
	char	rcvbuf[8192] = {0};
	int		rcvlen = 0;
	unsigned long i = 0;
	
	static DWORD sta_dwCt = 0;

	while (!m_bKillRecvThread) 
	{		
		if (-1 == m_FdComPhyPort)
		{
			PRTMSG(MSG_DBG, "RecvPhoneDataThread, phyport handle invalid\n");
			sleep(1);
			continue;
		}
		rcvlen = read(m_FdComPhyPort, rcvbuf, sizeof(rcvbuf));
		if(rcvlen > 0)
		{			
			//PRTMSG(MSG_DBG, "rcvbuf:%s",rcvbuf);
			//PrintString(rcvbuf, rcvlen);

			if(m_bComInMux) 
			{
				for(i=0; i<rcvlen && i < sizeof(rcvbuf); i++)	{  _DealChar(rcvbuf[i]); }
			} 
			else 
			{
				for(i=0; i<rcvlen; i++) 
				{
					if(m_CommBufLen<MAX_STR-1)	
					{
						m_CommBuffer[m_CommBufLen++] = rcvbuf[i];//数据保存到缓冲区，mux前的数据
					}
				}
			}
		}	
		usleep(10000);
		
		memset( rcvbuf, 0, sizeof(rcvbuf) );
	}
	PRTMSG(MSG_DBG, "RecvPhoneDataThread end\n");
	return 0;
}

//函数功能：	线程函数
//线程安全：	否
//调用方式：	read阻塞或非阻塞
int CComVir::P_RecvAtMuxDataThread()
{
	char	rcvbuf[2048] = {0};
	int		rcvlen = 0;

	while(!m_bKillRecvThread)
	{

		if (-1 == m_FdMstAt)
		{
			PRTMSG(MSG_DBG, "P_RecvAtMuxDataThread, AtMuxport handle invalid\n");
			sleep(1);
			continue;
		}
		rcvlen = read(m_FdMstAt, rcvbuf, sizeof(rcvbuf));
		if(rcvlen > 0)
		{
			if(m_bComInMux) 
			{
				_WriteFrame(MUXCHANNEL_AT, rcvbuf, rcvlen, UIH);
			}
		}	
		else 
		{
			usleep(50000);
		}
		memset( rcvbuf, 0, sizeof(rcvbuf) );
	}
}

//函数功能：	线程函数
//线程安全：	否
//调用方式：	read阻塞或非阻塞
int CComVir::P_RecvPppMuxDataThread()
{
#define  FRAME_LEN	60

	char	rcvbuf[8192] = {0};
	int		rcvlen = 0;
	int		ilen = 0;

	while(!m_bKillRecvThread)
	{
		if (-1 == m_FdMstPpp)
		{
			PRTMSG(MSG_DBG, "P_RecvPppMuxDataThread, PppMuxport handle invalid\n");
			sleep(1);
			continue;
		}
		rcvlen = read(m_FdMstPpp, rcvbuf, sizeof(rcvbuf));
		if(rcvlen > 0)
		{
			if(m_bComInMux) 
			{
				for(int i=0; i<rcvlen; i+=FRAME_LEN)
				{
					ilen = min(FRAME_LEN, rcvlen-i);
					_WriteFrame(MUXCHANNEL_PPP, rcvbuf+i, ilen, UIH);
					usleep(1000);
				}
			}
		}	
		else 
		{
			usleep(20000);//阻塞读没sleep没什么用
		}
		memset( rcvbuf, 0, sizeof(rcvbuf) );
	}
}

//函数功能：	停止线程
//线程安全：	否
//调用方式：	清除操作时调用
void CComVir::_StopRecvThread()
{
	m_bKillRecvThread = true;
}

//函数功能：	串口写
//线程安全：	是
//调用方式：	
//向串口中写数据
int CComVir::_WriteToSerial(void* szSendBuf, unsigned long dwSendLen)
{
	DWORD 		wrtlen=0;
	pthread_mutex_lock( &m_MutexWrite );
	wrtlen = write(m_FdComPhyPort, szSendBuf, dwSendLen);

 	//PRTMSG(MSG_DBG, "_WriteToSerial: ",szSendBuf,dwSendLen);
 	//PrintString((char*)szSendBuf, dwSendLen);

	pthread_mutex_unlock( &m_MutexWrite );
	return wrtlen;
}

//函数功能：	发送at命令
//线程安全：	否
//调用方式：	初始化mux时调用
bool CComVir::_SendCmd(char* cmd)
{
	int cnt = 10;
	int i = 0;
	bool ret = 0;
	bool blRcv = FALSE;
	while(--cnt>=0) 
	{
		m_CommBufLen = 0;
		memset(m_CommBuffer, 0 ,sizeof(m_CommBuffer));
		//需做互斥
		_WriteToSerial(cmd, strlen(cmd));
		usleep(500000);
		
		// xun:增加等待子循环
		blRcv = false;
		for( i = 0; i < 30; i ++ )
		{
			if( strlen(m_CommBuffer) )
			{			
				blRcv = true;
				if(strstr(m_CommBuffer, "SIMCOM_SIM300D\n"))
				{
					m_gsmtype = SIMCOM;
					PRTMSG(MSG_DBG, "Phone Module Type: SIMCOM\n");
				}
				if(strstr(m_CommBuffer, "M23A"))
				{
					m_gsmtype = BENQM23;
					PRTMSG(MSG_DBG, "Phone Module Type: Benq M23\n");
				}
				if(strstr(m_CommBuffer, "M25"))
				{
					m_gsmtype = BENQM25;
					PRTMSG(MSG_DBG, "Phone Module Type: Benq M25\n");
				}
				if(strstr(m_CommBuffer, "M33"))
				{
					m_gsmtype = BENQM33;
					PRTMSG(MSG_DBG, "Phone Module Type: Benq M33\n");
				}
				
				if(strstr(m_CommBuffer, "Quectel_M10"))
				{
					m_gsmtype = SIMCOM;
					PRTMSG(MSG_DBG, "Phone Module Type: M10. Set to type:SIMCOM\n");
				}
				
				if(strstr(m_CommBuffer, "OK"))					
				{
					ret = true;
				}
			}
			
			if(ret)	
			{
				return ret;
			}
			usleep(100000);
			if( blRcv ) 
			{
				break;
			}
		}
	}
	return ret;
}

//函数功能：	版本查询
//线程安全：	否
//调用方式：	初始化mux时调用
void CComVir::_QueryModuleVer()
{
	char QUERYCMD[] = "AT+CGMR\r\n";
	
	int cnt = 3;
	int i = 0;
	bool blRcv = false;
	while(--cnt>=0) 
	{
		//需做互斥
		m_CommBufLen = 0;
		memset(m_CommBuffer, 0 ,sizeof(m_CommBuffer));
		
		_WriteToSerial(QUERYCMD, strlen(QUERYCMD));
		
		usleep(500000);

		blRcv = false;
		for( i = 0; i < 5; i ++ )
		{
			char buf[MAX_STR] = { 0 };

			memcpy( buf, m_CommBuffer, min(strlen(m_CommBuffer), sizeof(buf) - 2) );
			
			if( strlen(buf) )
			{
				blRcv = true;
				if( strstr(buf, "OK") )
				{
					const char SPEC[] = "SW ver:";
					char* p = strstr(buf, SPEC);
					if( p )
					{
						char szVer[50] = { 0 };
						
						p += strlen(SPEC);
						while( 0x20 == *p ) p++;
						
						while( (*p >= 0x30 && *p <= 0x39) || '.' == *p )
						{
							if( strlen(szVer) + 1 < sizeof(szVer) ) szVer[ strlen(szVer) ] = *p;
							else break;
							p ++;
						}
						if( strlen(szVer) > 0 && strcmp(szVer, "1.50") >= 0 ) 
						{
							m_bModuleVerHigh = true;
						}
						PRTMSG(MSG_DBG, "Module Ver:%s\n", szVer);						
						return;
					}
				}
			}
			
			usleep(100000);
			if( blRcv ) break;
		}
	}
}

//函数功能：	初始化
//线程安全：	否
//调用方式：	初始化ComVir调用
bool CComVir::_InitMux() 
{
	int count = 0;
	char cmd[100] = {0};

	sprintf(cmd, "AT\r\n");
	if(!_SendCmd(cmd))	
	{ 
		PRTMSG(MSG_DBG, "InitMux - Set AT Fail!\n"); 
		return false; 
	}
	PRTMSG(MSG_DBG, "InitMux - Set AT succ\n"); 
	
	sprintf(cmd, "ATE0\r\n");
	if(!_SendCmd(cmd))	
	{ 
		PRTMSG(MSG_DBG, "InitMux - Set ATE0 Fail!\n"); 
		return false; 
	}
	PRTMSG(MSG_DBG, "InitMux - Set ATE0 SUCC\n"); 
	
	sprintf(cmd, "AT+CGMM\r\n");
	if(!_SendCmd(cmd))	
	{ 
		PRTMSG(MSG_DBG, "InitMux - Set CGMM Fail!\n"); 
		return false; 
	}
	PRTMSG(MSG_DBG, "InitMux - Set CGMM SUCC\n"); 
	
	if(SIMCOM==m_gsmtype) 
	{
		PRTMSG(MSG_DBG, "Module is simcom300d!\n");
		sprintf(cmd, "AT+CMUX=0,0,%d\r\n", BANDRATE);
		_WriteToSerial(cmd, strlen(cmd));
	}
	
	if(BENQM23==m_gsmtype || BENQM25 == m_gsmtype || BENQM33 == m_gsmtype)
	{		
		if( BENQM23 == m_gsmtype) 
		{
			_QueryModuleVer();
		}

		sprintf(cmd, "AT$NOSLEEP=1\r\n");
		if(!_SendCmd(cmd))	
		{
			PRTMSG(MSG_DBG, "InitMux - Set NOSLEEP Fail!\n"); 
			return false; 
		}

// 		sprintf(cmd, "ATE0Q0V1\r\n");
// 		if(!_SendCmd(cmd))	
// 		{
// 			PRTMSG(MSG_DBG, "InitMux - Set ATE0Q0V1 Fail!\n"); 
// 			return false; 
// 		}
		
		sprintf(cmd, "AT+CMUX=1,0,%d\r", BANDRATE);
		_WriteToSerial(cmd, strlen(cmd));
	}
	m_bComInMux = true;	//成功标志要在cmux成功后标记，因为后面的SABM协议有需要

	// 一定要休眠2秒以上(非常重要!!!因为AT+CMUX后,手机模块比较繁忙,如果此时马上发送DLC会导致模块死掉)
	sleep(6);	
	
	// 创建DLC，发送SABM帧等
	for( count=0; count<=3; count++) 
	{		
		if( 0 == _WriteFrame(count, NULL, 0, SABM|PF) ) 
		{
			PRTMSG(MSG_DBG, "InitMux - Send SABM Fail!\n");
			return false;
		}
		//sleep(6);
	}
	sleep(6);

	return true;
}

//函数功能：	关闭设备文件
//线程安全：	否
//调用方式：	清除mux时调用
void CComVir::_CloseMux() 
{
	unsigned char close_mux[2] = { C_CLD | CR , 1 };
	
	int i = 0;
	while(0==_WriteFrame(0, (char *)close_mux, 2, UIH | PF)) 
	{
		i++;
		if(i>5)	
		{
			PRTMSG(MSG_DBG,"CloseMux failed!\n");
			break;
		}
		usleep(200000);
	}
	
	//应该等到收到应答帧后再进行链路标志设置
	sleep(1);
	m_bComInMux = false;//
}

//关闭打开的伪终端
bool CComVir::_ComMuxClose()
{

}

//函数功能：	关闭设备文件
//线程安全：	否
//调用方式：	清除操作时调用
//关闭打开的串口
bool CComVir::_ComAllClose()
{
	PRTMSG(MSG_DBG, "Close Phy Com \n");
	
	//停止接收线程
	_StopRecvThread();

	//关闭打开的物理串口
	if(m_FdComPhyPort)
	{
		close(m_FdComPhyPort);
		m_FdComPhyPort = -1;
	}

	//关闭打开的at复用终端
	if(m_FdMstAt)
	{
		close(m_FdMstAt);
		m_FdMstAt = -1;
	}
	
	//关闭打开的ppp复用终端
	if(m_FdMstPpp)
	{
		close(m_FdMstPpp);
		m_FdMstPpp = -1;
	}

	memset(m_szSlvPathAt, 0, sizeof(m_szSlvPathAt));//at通道从设备路径
	memset(m_szSlvPathPpp, 0 ,sizeof(m_szSlvPathPpp));//ppp通道从设备
	memset(m_szSlvPathMng, 0, sizeof(m_szSlvPathMng));//mng通道从设备

	sleep(2);

	return true;
}

//函数功能：	数据处理
//线程安全：	否
//调用方式：	在模块串口接收数据线程调用

//分配从手机模块串口接收的数据到各个通道
//channel：0，控制通道
//channel：1，AT通道
//channel：2，拨号通道
//channel：3，管理通道
int CComVir::_AssignDataToChannels(unsigned char * data, unsigned long data_length, unsigned char channel)
{
	unsigned long RoomLeft = 0;
	
	//写数据的安全性？
	switch(channel) 
	{ 		
		case MUXCHANNEL_AT:
			{
				write(m_FdMstAt, data, data_length);
				//PRTMSG(MSG_DBG, "channel AT write:");
				//PrintString((char*)data, data_length);
			}
			break;

		case MUXCHANNEL_PPP:	
			{
				write(m_FdMstPpp, data, data_length);
// 				PRTMSG(MSG_DBG, "channel ppp write:");
// 				PrintString((char*)data, data_length);
			}
			break;

//		case MUXCHANNEL_MNG:	
//			write(m_FdMstMng, data, data_length);		
			break;

		default:
			return -1;
	}
}

//函数功能：	数据处理
//线程安全：	是
//调用方式：	
unsigned char CComVir::_MakeFcs(const unsigned char *input, int count) 
{
	unsigned char fcs = 0xFF;
	int i;
	for(i = 0; i < count; i++) {
		fcs = r_crctable[fcs^input[i]];
	}
	return (0xFF-fcs);	
}

//函数功能：	写数据帧
//线程安全：	是
//调用方式：	向不同通道写数据时调用
int CComVir::_WriteFrame(int channel, const char *input, int count, unsigned char type)
{
	unsigned char Frame[1024] = {0};
	
	if(PHONE_MODTYPE == PHONE_MODTYPE_SIMCOM)
	{
		int len = 0;
		unsigned char fcs = 0x00;
		int i = 0;
		
		Frame[len++] = 0xF9;
		
		Frame[len++] = (EA|CR) | ((63 & (unsigned char) channel) << 2) ;
		
		Frame[len++] = type;
		
		if(count > 127) 
		{
			Frame[len++] = ((127 & count) << 1);
			Frame[len++] = (32640 & count) >> 7;
		} 
		else 
		{
			Frame[len++] = 1 | (count << 1);
		}
		
		fcs =  _MakeFcs(Frame + 1, len-1);
		
		for(i = 0; i<count; i++)	Frame[len++] = input[i];
		
		Frame[len++] = fcs;
		
		Frame[len++] = 0xF9;
		
		i = _WriteToSerial(Frame, len);
		
		if(i ==len) 
		{
			return 1;
		} 
		else 
		{
			PRTMSG(MSG_DBG, "_WriteFrame Error! writed = %d/%d \n", i, len);
			return 0;
		}
	}
	else if(PHONE_MODTYPE == PHONE_MODTYPE_BENQ)
	{
#define ENCODE_FRAME(Frame, value, len)				\
	switch(value) {														\
				case 0x7d:	Frame[len++]=0x7d;	Frame[len++]=0x5d;	break;		\
				case 0x7e:	Frame[len++]=0x7d;	Frame[len++]=0x5e;	break;		\
				default:	Frame[len++]=value;	break;	}
		
		int len = 0;
		int i = 0;
		unsigned char value = 0x00;
		unsigned char fcs[2] = {0x00, 0x00};
		
		Frame[len++] = 0x7E;		//标志位
		
		value = (EA|CR) | ((63 & (unsigned char) channel) << 2);	//地址位
		fcs[0] = value;
		ENCODE_FRAME(Frame, value, len);	
		
		value = type;		//控制位
		fcs[1] = value;
		ENCODE_FRAME(Frame, value, len);	
		
		for(i=0; i<count; i++) 
		{	//数据
			value = input[i];
			ENCODE_FRAME(Frame, value, len);	
		}
		
		value = _MakeFcs(fcs, 2);	//校验位
		ENCODE_FRAME(Frame, value, len);	
		
		Frame[len++] = 0x7E;	//标志位
		
		i = _WriteToSerial(Frame, len);
		
		if(i == len)	
		{
			return 1;
		}
		else		
		{
			PRTMSG(MSG_DBG, "_WriteFrame Error! writed = %d/%d \n", i, len);
			return 0;
		}
	}
	
	return 0;	
}

//函数功能：	数据处理
//线程安全：	否
//调用方式：	处理模块channel0数据时调用
// Handles commands received from the control channel.
void CComVir::_HandleCommand(GSM0710_Frame * frame)
{
	unsigned char type, signals;
	int length = 0, i, type_length, channel, supported = 1;
	unsigned char *response;
	// struct ussp_operation op;

	if (frame->data_length > 0)
	{
		type = frame->data[0];  // only a byte long types are handled now
		// skip extra bytes
		for(i = 0; (frame->data_length > i && (frame->data[i] & EA) == 0); i++);
		
		i++;
		type_length = i;
		if ((type & CR) == CR)
		{
			// command not ack

			// extract frame length
			while (frame->data_length > i)
			{
				length = (length * 128) + ((frame->data[i] & 254) >> 1);
				if ((frame->data[i] & 1) == 1)	break;
				i++;
			}
			i++;
			
			switch((type & ~CR))
			{
				case C_CLD:
					PRTMSG(MSG_DBG, "The mobile station requested mux-mode termination.\n");
					// terminate = 1;
					// terminateCount = -1;    // don't need to close down channels
					break;
					
				case C_TEST:
					PRTMSG(MSG_DBG, "Test command: ");
					PRTMSG(MSG_DBG, "frame->data = %s  / frame->data_length = %d\n",frame->data + i, frame->data_length - i);
					break;
					
				case C_MSC:
					if (i + 1 < frame->data_length)
					{
						channel = ((frame->data[i] & 252) >> 2);
						i++;
						signals = (frame->data[i]);
						// op.op = USSP_MSC;
						// op.arg = USSP_RTS;
						// op.len = 0;

					//	RETAILMSG(DEBUGMODE, (TEXT("Modem status command on channel %d.\n"), channel));
						if ((signals & S_FC) == S_FC) {
							//PRTMSG(MSG_DBG, "(%d)No frames allowed.\n", channel);
							m_ChannelSend[channel] = false;
						} else {
							// op.arg |= USSP_CTS;
							//PRTMSG(MSG_DBG, "(%d)Frames allowed.\n", channel);
							m_ChannelSend[channel] = true;
						}

						if ((signals & S_RTC) == S_RTC) {
							// op.arg |= USSP_DSR;
							//PRTMSG(MSG_DBG, "RTC\n");	
						}

						if ((signals & S_RTR) == S_RTR) {
							//PRTMSG(MSG_DBG, "RTR\n");
						}	

						if ((signals & S_IC) == S_IC) {
							// op.arg |= USSP_RI;;
							//PRTMSG(MSG_DBG, "Ring\n");
						}

						if ((signals & S_DV) == S_DV) {
							// op.arg |= USSP_DCD;
							//PRTMSG(MSG_DBG, "Data Valid\n");
						}
						// if (channel > 0)
						//     write(ussp_fd[(channel - 1)], &op, sizeof(op));
					}
					else
					{
						PRTMSG(MSG_DBG, "ERROR: Modem status command, but no info. i: %d, len: %d, data-len: %d\n",
								i, length, frame->data_length);
					}
					break;
				default:
					PRTMSG(MSG_DBG, "Unknown command (%d) from the control channel.\n",type);
					response = (unsigned char* )malloc(sizeof(char) * (2 + type_length));
					response[0] = C_NSC;
					// supposes that type length is less than 128
					response[1] = EA & ((127 & type_length) << 1);
					i = 2;
					while (type_length--)
					{
						response[i] = frame->data[(i - 2)];
						i++;
					}
					_WriteFrame(0, (char *)response, i, UIH);
					free(response);
					supported = 0;
					break;
			}

			if (supported)
			{
				// acknowledge the command
				frame->data[0] = frame->data[0] & ~CR;
				_WriteFrame(0, (char *)frame->data, frame->data_length, UIH);
			}
		}
		else
		{
			// received ack for a command
			if (COMMAND_IS(C_NSC, type))
			{
				PRTMSG(MSG_DBG, "The mobile station didn't support the command sent.\n");
			}
			else
			{
				PRTMSG(MSG_DBG, "Command acknowledged by the mobile station.\n");
				if(frame->data[0]==0xc1 && frame->data[1]==0x01)	m_bComInMux = false;
			}
		}
	}
}

//函数功能：	字符处理
//线程安全：	否
//调用方式：	在接收线程调用
void CComVir::_DealChar(unsigned char ch)
{
	static int State = 0;
	static GSM0710_Frame Frame;
	static unsigned char Data[4096] = {0};
	static unsigned char Fcs = 0xFF;

if(SIMCOM==m_gsmtype)
	{
		static int Len = 0;
		static int cntf9 = 0;	//连续收到f9的个数统计(用于SIMCOM模块失步识别)
		unsigned char szf9[4] = { 0xf9, 0xf9, 0xf9, 0xf9 };

		switch(State)
		{	
		case 0:	//标志位
			if(ch==0xf9) 
			{
				Len = 0;
				Frame.data_length = 0;
				State = 1;
			}
			break;

		case 1:	//通道位
			if(ch!=0xf9) 
			{
				cntf9 = 0;
				Fcs = r_crctable[0xFF^ch];
				Frame.channel = ((ch & 252) >> 2);
				if(Frame.channel <= 4)
					State = 2;
				else 
				{
					State = 0;
					PRTMSG(MSG_DBG, "channel num:%d err\n", Frame.channel);
				}
			} 
			else 
			{
				if(++cntf9>=3) 
				{	//如果连续收到4个0xf9,则认为失步,发送同步信号
					PRTMSG(MSG_DBG, "!Rcv %d f9!\n", cntf9+1);
					cntf9 = 0;
					_WriteToSerial(szf9, 4);
				}
			}
			break;

		case 2:	//控制位
			Fcs = r_crctable[Fcs^ch];
			Frame.control = ch;
			State = 3;
			break;

		case 3:	//长度位1
			Frame.data_length = (ch & 254) >> 1;
			Fcs = r_crctable[Fcs^ch];
			if( (ch&1)==0 )
				State = 4;
			else	
			{
				if(Frame.data_length>=180)  
				{ 
					State = 0; 
					PRTMSG(MSG_DBG, "data_length !2 %d err\n", Frame.data_length);
				}
				else if(Frame.data_length==0)	State = 6;
				else State = 5;
			}
			break;

		case 4:	//长度位2
			Frame.data_length += (ch*128);
			Fcs = r_crctable[Fcs^ch];
			if(Frame.data_length>=180)	
			{
				State = 0;
				PRTMSG(MSG_DBG, "data_length !3 %d err\n", Frame.data_length);
			}
			else
				State = 5;
			break;

		case 5:	//数据区
			Data[Len++] = ch;
			if(Len  == Frame.data_length)	State = 6;
			break;

		case 6:	//校验位
			if(r_crctable[Fcs^ch]==0xCF) 
			{
				State = 7;
			} else 
			{
				State = 0;
				PRTMSG(MSG_DBG, "crc err !4\n");
			}
			break;

		case 7:	//结束符
			Frame.data = Data;
			_DealFrame(&Frame);
			State = 0;
			break;

		default:
			//PRTMSG(MSG_DBG, "!0\n");
			State = 0;
			break;	
		}		
	}

if(BENQM23==m_gsmtype || BENQM25==m_gsmtype || BENQM33 == m_gsmtype)
	{
		static int Rev7D = 0;
		int Rev7E = (ch==0x7e) ? (1) : (0);

		if(ch==0x7d)		{ Rev7D = 1; return; }
		if(Rev7D)			{ Rev7D = 0; ch ^= 0x20;  }
		
		switch(State)
		{	
		case 0:	//标志位
			if(Rev7E)	 
			{	
				Frame.data_length = 0;
				State = 1;
			}
			break;

		case 1:	//通道位
			if(!Rev7E) 
			{
				Fcs = r_crctable[0xFF^ch];
				Frame.channel = ((ch & 252) >> 2);
				if(Frame.channel <= 4) {
					State = 2;
				} 
				else
				{
					State = 0;
					//PRTMSG(MSG_DBG, "!5\n");
				}
			}
			break;

		case 2:	//控制位
			if(!Rev7E) 
			{
				Fcs = r_crctable[Fcs^ch];
				Frame.control = ch;
				State = 3;
			} else 
			{
				State = 0;
				//PRTMSG(MSG_DBG, "!6\n");
			}
			break;

		case 3:	//数据区
			if(Rev7E)
			{	
				if(Frame.data_length!=0 && r_crctable[Fcs^Data[--Frame.data_length]]==0xCF)
				{
					Frame.data = Data;
					_DealFrame(&Frame);
				} 
				else
				{
					//PRTMSG(MSG_DBG, "!7\n");
				}
				State = 1;
				Frame.data_length = 0;
			}
			else
			{
				if(Frame.data_length>=sizeof(Data))
				{
					//PRTMSG(MSG_DBG, "!8\n");
					State = 0;
					break;
				}
				Data[Frame.data_length++] = ch;
			}
			break;
				
		default:
			//PRTMSG(MSG_DBG, "!9\n");
			State = 0;
			break;	
		}	
	}

}

//函数功能：	数据处理
//线程安全：	否
//调用方式：	在接收线程调用
void CComVir::_DealFrame(GSM0710_Frame *frame)
{
	if ((FRAME_IS(UI, frame) || FRAME_IS(UIH, frame)))
	{
		if (frame->channel > 0)
		{
			_AssignDataToChannels(frame->data, frame->data_length, frame->channel);				
		}
		else
		{
			// control channel command
			_HandleCommand(frame);
		}
	}
	else		// The other type of frame (except UI or UIH)
	{
		// not an information frame
		switch((frame->control & ~PF))
		{
		case UA:
			//PRTMSG(MSG_DBG, "This  is a UA Frame of channel %d\n",(int)frame->channel);
			if (m_cstatus[frame->channel].opened == 1)
			{
				PRTMSG(MSG_DBG, "Logical channel %d closed.\n",(int)frame->channel);
				m_cstatus[frame->channel].opened = 0;
			}
			else
			{
				m_cstatus[frame->channel].opened = 1;
				if (frame->channel == 0)
				{
				//	#if GSM==MC35
				//		// send version Siemens version test
				//		RETAILMSG(DEBUGMODE, (TEXT("Control channel opened.Must send Siemens version!\n")));
				//		_WriteFrame(0, "\x23\x21\x04TEMUXVERSION2\0\0", 18, UIH);
				//	#endif

				}
				else
				{
					PRTMSG(MSG_DBG, "Logical channel %d opened.\n",(int)frame->channel);
				}
			}
			break;

		case DM:
			if (m_cstatus[frame->channel].opened)
			{
				PRTMSG(MSG_DBG, "DM received, so the channel %d was already closed.\n",(int)frame->channel);
				m_cstatus[frame->channel].opened = 0;
			}
			else
			{
				if (frame->channel == 0)
				{
					PRTMSG(MSG_DBG, "Couldn't open control channel.\n->Terminating.\n");
					//terminate = 1;
					//terminateCount = -1;    // don't need to close channels
				}
				else
				{
					PRTMSG(MSG_DBG, "Logical channel %d couldn't be opened.\n",(int)frame->channel);
				}
			}
			break;

		case DISC:
			if (m_cstatus[frame->channel].opened)
			{
				m_cstatus[frame->channel].opened = 0;
				_WriteFrame( frame->channel, NULL, 0, UA | PF);
				if (frame->channel == 0)
				{
					PRTMSG(MSG_DBG, "Control channel closed.\n");
					//terminate = 1;
					//terminateCount = -1;    // don't need to close channels
				}
				else
				{
					PRTMSG(MSG_DBG, "Logical channel %d closed.\n",(int)frame->channel);
				}
			}
			else
			{
				// channel already closed
				PRTMSG(MSG_DBG, "Received DISC even though channel %d was already closed.\n",(int)frame->channel);
				_WriteFrame(frame->channel, NULL, 0, DM | PF);
			}
			break;
			
		case SABM:
			// channel open request
			if (m_cstatus[frame->channel].opened == 0)
			{
				if (frame->channel == 0) 
				{
					PRTMSG(MSG_DBG, "Control channel opened.\n");
				}
				else 
				{
					PRTMSG(MSG_DBG, "Logical channel %d opened.\n",(int)frame->channel);
				}
			}
			else
			{
				// channel already opened
				PRTMSG(MSG_DBG, "Received SABM even though channel %d was already closed.\n",(int)frame->channel);
			}
			m_cstatus[frame->channel].opened = 1;
			_WriteFrame(frame->channel, NULL, 0, UA | PF);
			break;

		default:
			break;
		}
		
	}
}

//函数功能：	硬件操作
//线程安全：	是
//调用方式：	注意调用前的硬件状态
void CComVir::_GsmPowOn()
{
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET,初始应该为高
	sleep(1);
// 	IOSet(IOS_PHONEPOW, IO_PHONEPOW_ON, NULL, 0);		//POWER
// 	sleep(1);
// 	IOSet(IOS_PHONEPOW, IO_PHONEPOW_OFF, NULL, 0);		//POWER,VBAT 拉至0，建议2-3s
// 	sleep(3);
	IOSet(IOS_PHONEPOW, IO_PHONEPOW_ON, NULL, 0);		//POWER
	sleep(3);											//VBAT 恢复供电后，等待最少13 秒以上
	IOSet(IOS_PHONERST, IO_PHONERST_LO, NULL, 0);		//RESET，在vbat高之后
	usleep(400000);		//about 200ms--500ms
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET
	sleep(1);
	PRTMSG(MSG_DBG,"Gsm power on!\n");
}

//函数功能：	硬件操作
//线程安全：	是
//调用方式：	注意调用前的硬件状态
//一下测试使用
// Gsm硬件复位
void CComVir::_GsmHardReset()
{
	_GsmPowOff();
	sleep(2);
	_GsmPowOn();
}

//函数功能：	硬件操作
//线程安全：	是
//调用方式：	注意调用前的硬件状态
void CComVir::_GsmPowOff()
{
	sleep(1);
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);		//RESET	
	sleep(1);
	IOSet(IOS_PHONEPOW, IO_PHONEPOW_OFF, NULL, 0);		//power	
	sleep(2);
	
	PRTMSG(MSG_DBG,"Gsm power off!\n");
}

//函数功能：	标志清除
//线程安全：	否
//调用方式：	做清除操作时调用
//标志清除操作，在复位操作之后调用
void CComVir::_InitFlagReset()
{
	memset(m_szSlvPathAt, 0, sizeof(m_szSlvPathAt));//at通道从设备路径
	memset(m_szSlvPathPpp, 0 ,sizeof(m_szSlvPathPpp));//ppp通道从设备
	memset(m_szSlvPathMng, 0, sizeof(m_szSlvPathMng));//mng通道从设备

	m_FdComPhyPort = -1;
	
	m_bComInMux = false;
	m_bKillRecvThread = false;
	m_gsmtype = PHONE_MODTYPE_BENQ;
	m_bModuleVerHigh = false;
	
	memset(m_CommBuffer, 0 ,sizeof(m_CommBuffer));
	m_CommBufLen = 0;
	memset(m_ChannelSend, 0 ,sizeof(m_ChannelSend));
}

//函数功能：	释放资源
//线程安全：	否
//调用方式：	PhoneGsm复位时调用
//供PhoneGsm调用使用，释放
void CComVir::ReleaseComVir()
{
	_StopRecvThread();
	sleep(1);
	_CloseMux();
	_ComAllClose();
	sleep(2);
	_InitFlagReset();
// 	_GsmHardReset();
// 	Init();
}


