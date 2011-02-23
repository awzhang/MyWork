#include "yx_QianStdAfx.h"

#if USE_LEDTYPE == 2

#undef MSG_HEAD 
#define MSG_HEAD	("QianExe-LedChLi")

void *G_ThreadLedChuangLiRecv(void *arg)
{
	g_objLedChuangLi.P_ThreadRecv();
}

void *G_ThreadLedChuangLiWork(void *arg)
{
	g_objLedChuangLi.P_ThreadWork();
}

//////////////////////////////////////////////////////////////////////////
CLedChuangLi::CLedChuangLi()
{
	m_objReadMng.InitCfg(1000, 3000);
	m_objWorkMng.InitCfg(5000, 10000);

	m_iComPort = 0;
	m_bytInfoNo = 0;
}

CLedChuangLi::~CLedChuangLi()
{

}

int CLedChuangLi::Init()
{
#if USE_COMBUS == 0
	// 打开串口
	if( !ComOpen() )
	{
		PRTMSG(MSG_ERR, "Open Com failed!\n");
		return ERR_COM;
	}

	// 创建接收线程
	if( pthread_create(&m_pthreadRecv, NULL, G_ThreadLedChuangLiRecv, NULL) != 0 )
	{
		ComClose();
		PRTMSG(MSG_ERR, "Create Recv Thread failed!\n");
		return ERR_THREAD;
	}
#endif

	if( pthread_create(&m_pthreadWork, NULL, G_ThreadLedChuangLiWork, NULL) != 0 )
	{
		ComClose();
		PRTMSG(MSG_ERR, "Create Work Thread failed!\n");
		return ERR_THREAD;
	}
	PRTMSG(MSG_DBG, "Create ChuangLi thread succ!\n");

	return 0;
}

int CLedChuangLi::Release()
{
#if USE_COMBUS == 0
	ComClose();
#endif

	return 0;
}

bool CLedChuangLi::ComOpen()
{
	int iResult;
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

bool CLedChuangLi::ComClose()
{
	if(m_iComPort != -1)
	{
		close(m_iComPort);
		m_iComPort = -1;

		return true;
	}

	return false;
}

int CLedChuangLi::ComWrite(char *v_szBuf, int v_iLen)
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

int CLedChuangLi::Deal0x42(BYTE v_bytDataType,char *v_szData, DWORD v_dwDataLen)
{
	switch( v_bytDataType )
	{
	case 0x08:
		Deal4208(v_szData, v_dwDataLen);
		break;

	default:
		break;
	}
}

int CLedChuangLi::Deal4208(char *v_szBuf, int v_iLen)
{
	byte bytInfoType = v_szBuf[0];
	byte bytInfoCnt = v_szBuf[1];
	char szbuf[100] = {0};
	int  ilen = 0, i = 0;
	DWORD dwPacketNum;
	byte bytRes = 2;		// 缺省为失败

	// 即时信息只能有一条
	if( bytInfoType == 1 )
	{
		if( bytInfoCnt != 1 || v_iLen != 19)
		{
			PRTMSG(MSG_ERR, "4208 len err\n");
			goto _DEAL4208_END;
		}

		MakeLedFrame(0x01, 'C', 0x00, v_szBuf+2, 17, szbuf, ilen);
		if( !m_objWorkMng.PushData( (BYTE)LV1, (DWORD)ilen, szbuf, dwPacketNum) )
		{
			bytRes = 1;
		}
	}
	else if( bytInfoType == 2 )
	{
		// 参数长度检查
		if( bytInfoCnt > 20 || v_iLen != 17*bytInfoCnt+2 )
			goto _DEAL4208_END;

		// 中心下发D类信息时，车台先撤销C类信息
		MakeLedFrame(0x02, 'C', 0x00, NULL, 0, szbuf, ilen);
		m_objWorkMng.PushData( (BYTE)LV1, (DWORD)ilen, szbuf, dwPacketNum);

		// 然后再发送D类信息
		for(i=0; i<bytInfoCnt; i++)
		{
			MakeLedFrame(0x01, 'D', m_bytInfoNo, v_szBuf+2+i*17, 17, szbuf, ilen);
			if( !m_objWorkMng.PushData( (BYTE)LV1, (DWORD)ilen, szbuf, dwPacketNum) )
			{
				m_bytInfoNo++;
			}
			else
			{
				break;
			}
		}

		// 全部成功发送到LED屏，则应答正确
		if(i == bytInfoCnt) bytRes = 1;
	}

_DEAL4208_END:
	int iRet = g_objSms.MakeSmsFrame((char*)&bytRes, 1, 0x42, 0x48, szbuf, sizeof(szbuf), ilen);
	if( !iRet ) iRet = g_objSock.SOCKSNDSMSDATA( szbuf, ilen, LV12, 0, NULL );

	return bytRes;	
}

void CLedChuangLi::DealEvent(char v_szCmd, char v_szType, char v_szInfoNo)
{
	char szbuf[100] = {0};
	int  ilen = 0;
	DWORD dwPacketNum;

	MakeLedFrame(v_szCmd, v_szType, v_szInfoNo, NULL, 0, szbuf, ilen);
	m_objWorkMng.PushData( (BYTE)LV1, (DWORD)ilen, szbuf, dwPacketNum);
}

void CLedChuangLi::P_ThreadRecv()
{
	char szRead;
	int  iReadLen = 0;
	int  i = 0;
	
	while(!g_bProgExit) 
	{
		iReadLen = read(m_iComPort, (void*)&szRead, 1);		// LED的返回值只有一个字节
		
		if(iReadLen > 1)
		{
			PRTMSG(MSG_ERR,"Com Read Over Buf!\n");		
			continue;
		}		
		
		if( iReadLen == 1 )
		{
			//PRTMSG(MSG_DBG, "ComBus recv data!\n");
			//PrintString(szReadBuf, iReadLen);
			
			DWORD dwPacketNum;
			m_objReadMng.PushData((BYTE)LV1, 1, &szRead, dwPacketNum);
		}
	}
}

void CLedChuangLi::P_ThreadWork()
{
	byte  bytLvl = 0;
	char  szBuf[1024] = {0};
	DWORD dwLen = 0;
	DWORD dwPushTm;
	DWORD dwBufSize = (DWORD)sizeof(szBuf);
	
	while( !g_bProgExit )
	{
		dwLen = 0;
		memset(szBuf, 0, sizeof(szBuf));
		
		// 向串口发送数据
		if( !m_objWorkMng.PopData(bytLvl, dwBufSize, dwLen, szBuf, dwPushTm))
		{		
 			// 发送数据帧		
 			SendCmdForAns(szBuf, dwLen);
		}

		usleep(100000);
	}
}

bool CLedChuangLi::SendCmdForAns(char *v_szbuf, int v_iLen)
{
	if( v_szbuf == NULL || v_iLen <=0 )
		return false;
	
	bool bRet = false;
	BYTE bytlvl = 0;
	char szResBuf[100] = {0};
	DWORD dwResLen = 0;
	DWORD dwPushTm;

	for(int i=0; i<3; i++)
	{
		ComWrite(v_szbuf, v_iLen);
			
		DWORD dwCur = GetTickCount();
		do 
		{
			if( true == g_bProgExit )
				goto SNDANDWAIT_END;
			
			usleep( 100000 );
			
			if( !m_objReadMng.PopData(bytlvl, (DWORD)(sizeof(szResBuf)), dwResLen, (char*)szResBuf, dwPushTm) )
			{
				if( dwResLen == 1 && szResBuf[0] == 0x55 )
				{
					bRet = true;
					goto SNDANDWAIT_END;	
				}
			}
		} while( GetTickCount() - dwCur < 2*1000 );		// 超时时间2s
	}

SNDANDWAIT_END:
	if( bRet )
	{
		PRTMSG(MSG_DBG, "LedChuangLi recv right answer!\n");
	}
	else
	{
		PRTMSG(MSG_DBG, "LedChuangLi recv wrong answer!\n");
	}

	return bRet;
}

void CLedChuangLi::MakeLedFrame(char v_szCmd, char v_szType, char v_szInfoNo, char *v_szInfoBuf, int v_iInfoLen, 
							   char *v_p_szFrameBuf, int &v_p_iFrameLen)
{
	char szbuf[100] = {0};
	int  ilen = 0;

	szbuf[ilen++] = v_szCmd;
	szbuf[ilen++] = v_szType;
	szbuf[ilen++] = v_szInfoNo;
	
	memcpy(szbuf+ilen, v_szInfoBuf, v_iInfoLen);
	ilen += v_iInfoLen;

	szbuf[ilen++] = (char)get_crc( (byte*)szbuf, ilen );	// 校验和

	// 转义
	ilen = TranData(szbuf, ilen);

#if USE_COMBUS == 0
	memcpy(v_p_szFrameBuf, szbuf, ilen);
	v_p_iFrameLen = ilen;			
#endif

#if USE_COMBUS == 1
	v_p_szFrameBuf[0] = 0xf7;	//0xf7表示由扩展盒的模拟串口2发送出去
	memcpy(v_p_szFrameBuf+1, szbuf, ilen);
	v_p_iFrameLen = ilen+1;
#endif
	
}

int CLedChuangLi::TranData(char *v_szBuf, int v_iLen)
{
	char szNewBuf[1024] = {0};
	int  iNewLen = 0;

	szNewBuf[iNewLen++] = 0x82;

	for( int i=0; i<v_iLen; i++) 
	{
		switch(v_szBuf[i])
		{
		case 0x82:
			szNewBuf[iNewLen++] = 0x81; 
			szNewBuf[iNewLen++] = 0x01;  
			break;

		case 0x83:
			szNewBuf[iNewLen++] = 0x81; 
			szNewBuf[iNewLen++] = 0x02; 
			break;

		case 0x81:
			szNewBuf[iNewLen++] = 0x81; 
			szNewBuf[iNewLen++] = 0x00; 
			break;

		default:
			szNewBuf[iNewLen++]= v_szBuf[i]; 
			break;
		}
	}

	szNewBuf[iNewLen++] = 0x83;

	memcpy(v_szBuf, szNewBuf, iNewLen);
	return iNewLen;
}

//--------------------------------------------------------------------------------------------------------------------------
// 检查校验和(直接取累加和)
// buf: 数据缓冲区指针
// len: 数据的长度
// 返回: 校验是否正确
bool CLedChuangLi::check_crc(const byte *buf, const int len)
{   
	byte check_sum = get_crc( buf+1, len-1 );
	return (buf[0] == check_sum);
}

//--------------------------------------------------------------------------------------------------------------------------
// 得到校验和的反码, 再加1
byte CLedChuangLi::get_crc(const byte *buf, const int len)
{   
	byte check_sum = 0;
	for(int i=0; i<len; i++) {
		check_sum += *(buf+i);
	}
	check_sum ^= 0xff;
	check_sum += 1;
	
    return check_sum;
}

#endif

