#include "yx_QianStdAfx.h"

#if USE_LEDTYPE == 3
//#define LOGDDLED

#undef MSG_HEAD 
#define MSG_HEAD	("QianExe-LedKaiTian")

void *G_ThreadKTLedRecv(void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	g_objKTLed.P_ThreadRecv();
	
	pthread_exit(0);
}

void *G_ThreadKTLedWork(void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	g_objKTLed.P_ThreadWork();
	
	pthread_exit(0);
}

void G_TmClearFrontProc(void *arg, int arg_len)
{
	g_objKTLed.P_TmClearFrontProc();
}
void G_TmClearBackProc(void *arg, int arg_len)
{
	g_objKTLed.P_TmClearBackProc();
}
void G_TmSetLedStaProc(void *arg, int arg_len)
{
	g_objKTLed.P_TmSetLedStaProc();
}

CKTLed::CKTLed()
{
	m_objReadMng.InitCfg(1000, 3000);
	m_objWorkMng.InitCfg(5000, 10000);

	m_iComPort = -1;
	m_pthreadRecv = 0;
	m_pthreadWork = 0;
	
	m_bEscRecv = false;
	m_bEscWork = false;
	
	m_cShowType=0;
	m_bytIndex=0xff;
	m_bConnect=false;
	//	LedInit();
}

CKTLed::~CKTLed()
{
	
}

int CKTLed::Init()
{
#if USE_COMBUS == 0
	// 打开串口
	if( !ComOpen() )
	{
		PRTMSG(MSG_ERR, "Open Com failed!\n");
		return ERR_COM;
	}
	
	// 创建接收线程
	if( pthread_create(&m_pthreadRecv, NULL, G_ThreadKTLedRecv, NULL) != 0 )
	{
		Release();
		PRTMSG(MSG_ERR, "Create Recv Thread failed!\n");
		return ERR_THREAD;
	}
#endif
	
	if( pthread_create(&m_pthreadWork, NULL, G_ThreadKTLedWork, NULL) != 0 )
	{
		Release();
		PRTMSG(MSG_ERR, "Create Work Thread failed!\n");
		return ERR_THREAD;
	}
	
	return 0;
}

int CKTLed::Release()
{
#if USE_COMBUS == 0
	if( m_pthreadRecv )
	{
		m_bEscRecv = true;
		usleep(100000);
		
		if( WaitForEvent(ThreadExit, &m_pthreadRecv, 3000) )
		{
			pthread_cancel(m_pthreadRecv);
			usleep(100000);
		}

		m_pthreadRecv = 0;
	}
	m_bEscRecv = false;
		
	ComClose();
#endif

	if( m_pthreadWork )
	{
		m_bEscWork = true;
		usleep(100000);
		
		if( WaitForEvent(ThreadExit, &m_pthreadWork, 3000) )
		{
			pthread_cancel(m_pthreadWork);
			usleep(100000);
		}

		m_pthreadWork = 0;
	}
	m_bEscWork = false;
	
	return 0;
}

bool CKTLed::ComOpen()
{
	struct termios options;
	
	m_iComPort = open("/dev/ttySIM1", O_RDWR );
	
	if( -1 == m_iComPort )
	{
		PRTMSG(MSG_ERR, "Open SIM1 failed!\n");
		perror("");
		return false;
	}
	
	if(tcgetattr(m_iComPort, &options) != 0)
	{
		PRTMSG(MSG_ERR, "GetSerialAttr\n");
		perror("");
		return false;
	}
	
	options.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	options.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	options.c_oflag &= ~OPOST;
	
	cfsetispeed(&options,B9600);	//扩展盒波特率9600
	cfsetospeed(&options,B9600);
	
	if(tcsetattr(m_iComPort, TCSANOW, &options) != 0)   
	{ 
		PRTMSG(MSG_ERR, "Set com Attr\n"); 
		perror("");
		return false;
	}
	
	PRTMSG(MSG_DBG,"ComBus open Succ!\n"); 
	
	return true;
}

bool CKTLed::ComClose()
{
	if(m_iComPort != -1)
	{
		close(m_iComPort);
		m_iComPort = -1;
		
		return true;
	}
	
	return false;
}

int CKTLed::ComWrite(char *v_szBuf, int v_iLen)
{
	PRTMSG(MSG_DBG, "Com send one frame!");
	PrintString(v_szBuf, v_iLen);
	
#if USE_COMBUS == 0
	int iWrited = 0;
	iWrited = write(m_iComPort, v_szBuf, v_iLen);
	return iWrited;
#endif
	
#if USE_COMBUS == 1
	DWORD dwPacketNum;
	g_objComBus.m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)v_iLen, v_szBuf, dwPacketNum);
	return v_iLen;
#endif	
}

void CKTLed::_SendLinkKeep()
{
	DWORD dwPacketNum;
	byte cmd[]={0x03,'E',0x00};
	
	m_objWorkMng.PushData( (BYTE)LV1, (DWORD)sizeof(cmd), (char*)cmd, dwPacketNum);
}

void CKTLed::P_ThreadWork()
{
	static DWORD dwSendLinkTick=0;
	
	while( !g_bProgExit )
	{
		if( m_bEscWork )
			break;
		
		DealLEDStack();
		
		if( dwSendLinkTick==0 || (GetTickCount()-dwSendLinkTick)>10*1000 )//10秒钟发送一次链路维护
		{
			_SendLinkKeep();
			dwSendLinkTick=GetTickCount();
		}

		usleep(50000);
	}
	
	m_bEscWork = false;
}

void CKTLed::P_ThreadRecv()
{
	char szRead[ALABUF] = {0};
	int  iReadLen = 0;
	
	while( !g_bProgExit ) 
	{
		if( m_bEscRecv )
			break;
		
		iReadLen = read(m_iComPort, (void*)szRead, ALABUF);
#ifdef LOGDDLED
		PRTMSG(MSG_DBG, "Read LED:");
		PrintString(szReadBuf, iReadLen);
#endif		

		if( iReadLen == 1 )
		{
			DWORD dwPacketNum;
			m_objReadMng.PushData((BYTE)LV1, 1, szRead, dwPacketNum);
		}		
	}
	
	m_bEscRecv = false;
}

void CKTLed::P_TmClearFrontProc()
{
	_KillTimer(&g_objTimerMng, CLEARFRONT_TIMER);
	
#ifdef LOGDDLED
	PRTMSG(MSG_DBG, "clear showtype:%c\n",m_cShowType);
#endif

	byte cmd[3]={0};
	cmd[0]=0x04;
	cmd[1]=(byte)m_cShowType;
	
	DWORD dwPacketNum;
	m_objWorkMng.PushData((BYTE)LV1, (DWORD)sizeof(cmd), (char*)cmd, dwPacketNum);
}

void CKTLed::P_TmClearBackProc()
{
	_KillTimer(&g_objTimerMng, CLEARBACK_TIMER);
	
	byte cmd[3]={0};
	cmd[0]=0x02;
	cmd[1]=(byte)'C';
	
	DWORD dwPacketNum;
	m_objWorkMng.PushData( (BYTE)LV1, (DWORD)sizeof(cmd), (char*)cmd, dwPacketNum);
}

void CKTLed::P_TmSetLedStaProc()
{
	
}

//-----------------------------------------------------------------------------
//线程中调用，实时处理LED数据堆栈中的指令
void CKTLed::DealLEDStack()
{
	bool bRet = false;
	byte  bytLvl = 0, bytReslvl = 0;
	char  szBuf[2048] = {0}, szResBuf[ALABUF] = {0};
	DWORD dwLen = 0, dwResLen = 0;
	DWORD dwPushTm, dwResPushTm;
	
//	char buf_snd[2048];
//	int len_snd=0;
//	int iFrm_cnt = 1;//剔出buf[0]
//	static int msgErr=0; //广告信息写入错误次数 3次为上限
	
	m_objWorkMng.PopData(bytLvl, (DWORD)(sizeof(szBuf)), dwLen, szBuf, dwPushTm);

	if( dwLen && dwLen < 512 )
	{
#ifdef LOGDDLED
		PRTMSG(MSG_DBG, "pop led stack len:%d\n",dwLen);
#endif

		for( int i=0; i<3; i++ )//失败最多发三次
		{
			Sendfrm( szBuf,(int)dwLen );

			DWORD dwCur = GetTickCount();
			do 
			{
				if( true == g_bProgExit )
					goto SNDANDWAIT_END;
				
				usleep( 200000 );
				
				if( !m_objReadMng.PopData(bytReslvl, (DWORD)(sizeof(szResBuf)), dwResLen, (char*)szResBuf, dwResPushTm) )
				{
					if( 0x55 == szResBuf[0] || 0xaa == szResBuf[0]  )
					{
						if( !m_bConnect )
						{
							m_bConnect = true;						
							g_objKTDeviceCtrl.SetDeviceSta(KT_LED,true);
							PRTMSG(MSG_DBG, "LED Connect!\n");
						}
						
						if( 0x55 == szResBuf[0] )
						{
							bRet = true;
							goto SNDANDWAIT_END;
						}
					}
				}
			} while( GetTickCount() - dwCur < 3*1000 );		//屏的响应时间应该在2秒以内
		}

    if( m_bConnect )
		{
			m_bConnect = false;
			g_objKTDeviceCtrl.SetDeviceSta(KT_LED,false);
			PRTMSG(MSG_DBG, "LED Disconnect!\n");
		}

//如果要给中心关于信息下发的应答，应该在这里做。
	}

SNDANDWAIT_END:
	if( bRet )
	{
		PRTMSG(MSG_DBG, "LedKaiTian recv right answer!\n");
	}
	else
	{
		PRTMSG(MSG_DBG, "LedKaiTian recv wrong answer!\n");
	}
}

//-----------------------------------------------------------------------------
// 发送LED数据帧
// v_pData:格式（未转义）：命令 信息类型  信息号  信息字段
// iLen: 参数长度
bool CKTLed::Sendfrm(char* v_pData,  int v_iLen)
{
#ifdef LOGDDLED
	PRTMSG(MSG_DBG, "send data to LED v_iLen:%d\n",v_iLen);
#endif

	if( v_iLen > 512 || NULL == v_pData ) 
	{
		PRTMSG(MSG_ERR, "data to led err\n");
		return false;
	}

	char szOrig[ALABUF] = { 0 };
	char szFrm[ALABUF] = { 0 };
	int i = 0, j = 0, k = 0;
	byte bytSum = 0;

	//计算校验和
	for( j=0; j<v_iLen; j++)
	{
		bytSum += (byte)v_pData[j];
	}
	
	memcpy( szOrig,v_pData,v_iLen );
	i += v_iLen;
	szOrig[i++] = (~bytSum)+1;//按位取反再加1
	
	// 数据封帧、转义
	j = 0;
	szFrm[j++] = (byte)0x82;
	for(k=0; k<i; k++) 
	{
		if(0x82 == szOrig[k]) { szFrm[j++] = (byte)0x81; szFrm[j++] = (byte)0x01; }//0x82→0x81,0x01
		else if(0x83 == szOrig[k]) { szFrm[j++] = (byte)0x81; szFrm[j++] = (byte)0x02; }//0x83→0x81,0x02
		else if(0x81 == szOrig[k]) { szFrm[j++] = (byte)0x81; szFrm[j++] = (byte)0x00; }//0x81→0x81,0x00
		else { szFrm[j++] = szOrig[k]; }
	}
	szFrm[j++] = (byte)0x83;
		
#ifdef LOGDDLED
	PRTMSG(MSG_DBG, "send LED frm:");
	PrintString(szFrm, j);
#endif

	int ret = ComWrite(szFrm, j);
	return (ret == j) ? true : false;
}

void CKTLed::SetBackText(char v_bytType,byte * v_aryText,byte v_bytCount)
{ 
	if( (v_bytType!='C' && v_bytType!='D') || v_bytCount<=0 || v_aryText==NULL ) return;
	//先进行清除

	byte bytCnt = v_bytCount;
	byte bytCmdLen = 0;
	if( 'C'==v_bytType )
	{	
		bytCnt = 1; 
		bytCmdLen = CTEXTLEN; 
	}
	else if( 'D'==v_bytType )
	{
		P_TmClearBackProc();
		bytCmdLen = DTEXTLEN;
	}

	DWORD dwPacketNum;
	byte cmd[23]={0};
	cmd[0]=0x01;
	cmd[1]=v_bytType;
	for(byte i=0; i<bytCnt; i++)
	{
		cmd[2]=i;
		memcpy(cmd+3,v_aryText+i*bytCmdLen,bytCmdLen);	

		m_objWorkMng.PushData( (BYTE)LV1, (DWORD)bytCmdLen+3, (char*)cmd, dwPacketNum);
	}
	
	//if( 'C'==v_bytType ) _SetTimer(&g_objTimerMng, CLEARBACK_TIMER,CLEARBACK_INTERVAL,G_TmClearBackProc);
}

//带自动取消显示的LED前灯设置功能，v_uiInterval为0时表示马上取消某类显示
void CKTLed::SetLedShow(char v_bytType,byte v_bytIndex,UINT v_uiInterval )
{
#ifdef LOGDDLED
	PRTMSG(MSG_DBG, "showtype:%c,index:%d\n",v_bytType,v_bytIndex);
#endif
	
	byte cmd[3] = {0};
	if( v_uiInterval<=0 )//取消显示
	{
		if(( m_cShowType==v_bytType && m_bytIndex==v_bytIndex ) || v_bytIndex==0xff )
		{
			m_cShowType=0;
			m_bytIndex=0xff;
			cmd[0]=0x04;
			cmd[1]=(byte)v_bytType;
			
#ifdef LOGDDLED
			PRTMSG(MSG_DBG, "clear showtype:%c\n",m_cShowType);
#endif
			DWORD dwPacketNum;
			m_objWorkMng.PushData( (BYTE)LV1, (DWORD)sizeof(cmd), (char*)cmd, dwPacketNum);
		}
	}
	else//设定显示
	{
		m_cShowType=v_bytType;
		m_bytIndex=v_bytIndex;
		cmd[0]=0x03;
		cmd[1]=(byte)v_bytType;
		cmd[2]=v_bytIndex;
		
		if( 'B'==v_bytType )//B类信息定时取消
		{
			_KillTimer(&g_objTimerMng, CLEARFRONT_TIMER);
			_SetTimer(&g_objTimerMng, CLEARFRONT_TIMER,v_uiInterval,G_TmClearFrontProc);
		}
		
		DWORD dwPacketNum;
		m_objWorkMng.PushData( (BYTE)LV1, (DWORD)sizeof(cmd), (char*)cmd, dwPacketNum);
	}
}
/*********************************  以下为LED各功能接口  ***********************************/

//A类信息（只有报警和取消报警）
//入口参数v_bAlarm:true,显示报警；false,取消报警显示
void CKTLed::DingdengClassA( bool v_bAlarm )
{
	byte szClassA[] = { 0x03,(byte)'A',0x00 };

	if(v_bAlarm)
	{
		szClassA[0] = (byte)0x03;//显示报警
		PRTMSG(MSG_DBG, "LED alarm\n");
	}
	else
	{
		szClassA[0] = (byte)0x04;//取消显示报警
		PRTMSG(MSG_DBG, "cancel LED alarm\n");
	}

	DWORD dwPacketNum;
	m_objWorkMng.PushData( (BYTE)LV1, (DWORD)sizeof(szClassA), (char*)szClassA, dwPacketNum);
}

//B类信息
//入口参数v_bDisp:true,显示相应项；false,取消相应项显示
//入口参数v_bytIndex:信息序号  0,预约；1,到达；2,交班；3,电招；4,急招
void CKTLed::DingdengClassB( bool v_bDisp,byte v_bytIndex )
{
	if( v_bytIndex > 4 )
	{
		PRTMSG(MSG_ERR, "LED set classB err\n");
		return;
	}

	byte szClassB[] = { 0x04,(byte)'B',0x00 };

	if(v_bDisp)
	{
		szClassB[0] = (byte)0x03;
		szClassB[2] = v_bytIndex;
	}
	else
	{
		szClassB[0] = (byte)0x04;
		szClassB[2] = (byte)0x00;		
	}

	DWORD dwPacketNum;
	m_objWorkMng.PushData( (BYTE)LV1, (DWORD)sizeof(szClassB), (char*)szClassB, dwPacketNum);
}

//C类信息
//入口参数v_bytCmd:命令：1,发送某类的信息2,删除某类全部信息3,显示某类预置数据4,取消显示某类预置数据
//入口参数v_pContent:信息内容:第一个字节是密钥序号，从0X00~0X7f，后面是要显示的信息。注意:信息应该是已经过DES加密后的(必须16字节)
//注意：严格保证v_pContent的总字节数为17B
void CKTLed::DingdengClassC( byte v_bytCmd,byte* v_pContent /*=NULL*/ )
{
	if( 0==v_bytCmd || v_bytCmd>4 )
	{
		PRTMSG(MSG_ERR, "LED set classC err1\n");
		return;
	}
	
	if( 1==v_bytCmd && NULL==v_pContent )//如果是发送信息，但内容为空的，则报错返回
	{
		PRTMSG(MSG_ERR, "LED set classC err2\n");
		return;
	}

	int iSndLen = 0;

	if( 1 != v_bytCmd )
	{
		byte szClassC[] = { 0x02,(byte)'C',0x00 };
		szClassC[0] = v_bytCmd;
		iSndLen = 3;
		
		DWORD dwPacketNum;
		m_objWorkMng.PushData( (BYTE)LV1, (DWORD)iSndLen, (char*)szClassC, dwPacketNum);
	}
	else
	{
		byte szClassC[30] = {0};
//		unsigned char szContent[17] = { (byte)0x20 };//保险起见，补0x20
//		memcpy( szContent,v_pContent,v_iLen );
		szClassC[iSndLen++] = (byte)0x01;
		szClassC[iSndLen++] = (byte)'C';
		szClassC[iSndLen++] = (byte)0x00;
		memcpy( szClassC+iSndLen,v_pContent,17 );
		iSndLen +=17;
		
		DWORD dwPacketNum;
		m_objWorkMng.PushData( (BYTE)LV1, (DWORD)iSndLen, (char*)szClassC, dwPacketNum);
	}
}

//D类信息
//入口参数v_bytCmd:命令：1,发送某类的信息2,删除某类全部信息3,显示某类预置数据4,取消显示某类预置数据
//入口参数v_pContent:信息内容:第一个字节是密钥序号，从0X00~0X7f，后面是要显示的信息。注意:应该是已经过DES加密后的
//入口参数v_bytIndex:信息序号0x00~0x13
void CKTLed::DingdengClassD( byte v_bytCmd,byte v_bytIndex /*=0*/,byte* v_pContent /*=NULL*/ )
{
	if( 0==v_bytCmd || v_bytCmd>4 || v_bytIndex>0x13 )
	{
		PRTMSG(MSG_ERR, "LED set classD err1\n");
		return;
	}
	
	if( 1==v_bytCmd && NULL==v_pContent )//如果是发送信息，但内容为空的，则报错返回
	{
		PRTMSG(MSG_ERR, "LED set classD err2\n");
		return;
	}

	int iSndLen = 0;

	if( 1!=v_bytCmd )
	{
		byte szClassD[] = { 0x02,(byte)'D',0x00 };
		szClassD[0] = v_bytCmd;
		iSndLen = 3;
		
		DWORD dwPacketNum;
		m_objWorkMng.PushData( (BYTE)LV1, (DWORD)iSndLen, (char*)szClassD, dwPacketNum);
	}
	else
	{
		byte szClassD[30] = {0};
		szClassD[iSndLen++] = (byte)0x01;
		szClassD[iSndLen++] = (byte)'C';
		szClassD[iSndLen++] = (byte)0x00;
		memcpy( szClassD+iSndLen,v_pContent,17 );
		iSndLen +=17;
		
		DWORD dwPacketNum;
		m_objWorkMng.PushData( (BYTE)LV1, (DWORD)iSndLen, (char*)szClassD, dwPacketNum);
	}
}

//E类信息（测试链路用）
void CKTLed::DingdengClassE()
{
#ifdef LOGDDLED
	PRTMSG(MSG_DBG, "LED alarm\n");
#endif

	byte szClassE[] = { 0x03,(byte)'E',0x00 };

	DWORD dwPacketNum;
	m_objWorkMng.PushData( (BYTE)LV1, (DWORD)sizeof(szClassE), (char*)szClassE, dwPacketNum);
}

#endif
