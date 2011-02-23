#include "yx_QianStdAfx.h"

#if USE_COMBUS == 2

#undef MSG_HEAD 
#define MSG_HEAD	("QianExe-Com150TR")

void *G_ThreadCom150TRRecv(void *arg)
{
	g_objCom150TR.P_ThreadCom150TRRecv();
}

void *G_ThreadCom150TRSend(void *arg)
{
	g_objCom150TR.P_ThreadCom150TRSend();
}

//////////////////////////////////////////////////////////////////////////
CCom150TR::CCom150TR()
{
	m_iComPort = 0;
	m_bComConnect = false;

	m_bytIOSignal = 0;
	m_dwCycleCount = 0;
	pthread_mutex_init(&m_mutexIOSignal, NULL);

	memset(m_szComFrameBuf, 0, sizeof(m_szComFrameBuf));
	m_iComFrameLen = 0;
}

CCom150TR::~CCom150TR()
{
	pthread_mutex_destroy(&m_mutexIOSignal);
}

int CCom150TR::Init()
{
	// 打开串口
	if( !ComOpen() )
	{
		PRTMSG(MSG_ERR, "Open Com failed!\n");
		return ERR_COM;
	}

	// 创建收发线程
	if( pthread_create(&m_pthreadRecv, NULL, G_ThreadCom150TRRecv, NULL) != 0
		|| pthread_create(&m_pthreadSend, NULL, G_ThreadCom150TRSend, NULL) != 0
		)
	{
		ComClose();
		PRTMSG(MSG_ERR, "Create Thread failed!\n");
		return ERR_THREAD;
	}

	PRTMSG(MSG_DBG, "Create Com150TR thread succ!\n");

	return 0;
}

int CCom150TR::Release()
{
	ComClose();

	return 0;
}

bool CCom150TR::ComOpen()
{
	int iResult;
	struct termios options;

	m_iComPort = open("/dev/ttyAMA0", O_RDWR );//| O_NONBLOCK);
	if( -1 == m_iComPort )
	{
		PRTMSG(MSG_ERR, "Open AMA0 failed!\n");
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

	PRTMSG(MSG_DBG,"Com150TR open Succ!\n"); 

	return true;
}

bool CCom150TR::ComClose()
{
	if(m_iComPort != -1)
	{
		close(m_iComPort);
		m_iComPort = -1;

		return true;
	}

	return false;
}

int CCom150TR::ComWrite(char *v_szBuf, int v_iLen)
{
	int iWrited = 0;
	
	iWrited = write(m_iComPort, v_szBuf, v_iLen);

//   	PRTMSG(MSG_DBG, "Com150TR send one frame: ");
//   	PrintString(v_szBuf, v_iLen);
	
	return iWrited;
}

void CCom150TR::P_ThreadCom150TRRecv()
{
	char szReadBuf[1024] = {0};
	int  iBufLen = sizeof(szReadBuf);
	int  iReadLen = 0;
	int  i = 0;
	
	while(!g_bProgExit) 
	{
		iReadLen = read(m_iComPort, szReadBuf, iBufLen);
		
		if(iReadLen > iBufLen)
		{
			PRTMSG(MSG_ERR,"Com Read Over Buf!\n");
			memset(szReadBuf, 0, sizeof(szReadBuf));
			iReadLen = 0;
			
			continue;
		}		
		
		if( iReadLen > 0 )
		{
//   			PRTMSG(MSG_DBG, "Com150TR recv data: ");
//   			PrintString(szReadBuf, iReadLen);

			AnalyseComFrame(szReadBuf, iReadLen);
		}
	}
}

void CCom150TR::P_ThreadCom150TRSend()
{
	byte  bytLvl = 0;
	char  szRecvBuf[1024] = {0};
	char  szSendBuf[1024] = {0};
	DWORD dwRecvLen = 0;
	int   iSendLen = 0;
	DWORD dwPushTm;
	DWORD dwBufSize = (DWORD)sizeof(szRecvBuf);
	
	while( !g_bProgExit )
	{
		dwRecvLen = 0;
		memset(szRecvBuf, 0, sizeof(szRecvBuf));
		
		// 先处理串口接收到的数据
		if( !m_objCom150TRReadMng.PopData(bytLvl, dwBufSize, dwRecvLen, szRecvBuf, dwPushTm))
		{
			if( dwRecvLen <= dwBufSize )
				DealCom150TRFrame(szRecvBuf, (int)dwRecvLen);
		}
		
		// 再向串口发送数据
		if( !m_objCom150TRWorkMng.PopData(bytLvl, dwBufSize, dwRecvLen, szRecvBuf, dwPushTm))
		{
			szSendBuf[1] = 1;	// 协议版本，固定取1
			szSendBuf[2] = 8;	// 外设编号，固定取8
			szSendBuf[3] = 0;	// 外设序号，固定取0
			memcpy(szSendBuf+4, szRecvBuf, dwRecvLen);
			szSendBuf[0] = get_crc((byte*)szSendBuf+1, 3+dwRecvLen);
			iSendLen = 4+dwRecvLen;

			// 转义
			iSendLen = TranData(szSendBuf, iSendLen);
		
 			// 发送数据帧		
 			ComWrite(szSendBuf, iSendLen);
		}

		usleep(50000);
	}
}

void CCom150TR::AnalyseComFrame(char *v_szBuf, int v_iLen)
{
	int iRet = 0;
	if( !v_szBuf || v_iLen <= 0 ) 
		return ;
	
	static int i7ECount = 0;
	
	for( int i = 0; i < v_iLen; i ++ )
	{
		if( 0x7e == v_szBuf[ i ] )
		{
			++ i7ECount;
			
			if( 0 == i7ECount % 2 ) // 若得到一帧
			{		
				// 反转义
				m_iComFrameLen = DetranData(m_szComFrameBuf, m_iComFrameLen);

				// 计算校验和，校验正确，送入接收队列
				if( check_crc((byte*)m_szComFrameBuf, m_iComFrameLen) )
				{				
					// 推入接收队列时去掉校验码、协议版本、外设编号，外设序号
					DWORD dwPacketNum = 0;
					iRet = m_objCom150TRReadMng.PushData(LV1, (DWORD)(m_iComFrameLen-4), m_szComFrameBuf+4, dwPacketNum);
					if( 0 != iRet )
					{
						PRTMSG(MSG_ERR, "Push one frame failed:%d\n",iRet);
						memset(m_szComFrameBuf, 0, m_iComFrameLen);
						m_iComFrameLen = 0;
					}
// 					else
// 					{
// 						PRTMSG(MSG_DBG, "m_objCom150TRReadMng push one frame:");
// 						PrintString(m_szComFrameBuf+4, m_iComFrameLen-4);
// 					}
				}
				else
				{
					PRTMSG(MSG_ERR, "Com150TR frame check crc err!\n");
					memset(m_szComFrameBuf, 0, m_iComFrameLen);
					m_iComFrameLen = 0;
				}
				
				//无论校验正确与否，都将缓冲区清0
				memset(m_szComFrameBuf, 0, m_iComFrameLen);
				m_iComFrameLen = 0;
			}
		}
		else
		{
			m_szComFrameBuf[m_iComFrameLen++] = v_szBuf[i];
		}
	}
}

int CCom150TR::TranData(char *v_szBuf, int v_iLen)
{
	char szNewBuf[1024] = {0};
	int  iNewLen = 0;

	szNewBuf[iNewLen++] = 0x7e;

	for( int i=0; i<v_iLen; i++) 
	{
		switch(v_szBuf[i])
		{
		case 0x7e:
			szNewBuf[iNewLen++] = 0x7d; 
			szNewBuf[iNewLen++] = 0x02;  
			break;

		case 0x7d:
			szNewBuf[iNewLen++] = 0x7d; 
			szNewBuf[iNewLen++] = 0x01; 
			break;

		default:
			szNewBuf[iNewLen++]= v_szBuf[i]; 
			break;
		}
	}

	szNewBuf[iNewLen++] = 0x7e;

	memcpy(v_szBuf, szNewBuf, iNewLen);
	return iNewLen;
}

int CCom150TR::DetranData(char *v_szBuf, int v_iLen)
{
	char szbuf[1024];
	int  iNewLen = 0;
	
	for( int i = 0; i < v_iLen ; )
	{
		if( 0x7d != v_szBuf[ i ] || i == v_iLen - 1 )
		{
			szbuf[ iNewLen ++ ] = v_szBuf[ i++ ];
		}
		else
		{
			if( 1 == v_szBuf[ i + 1 ] )
			{
				szbuf[ iNewLen ++ ] = 0x7d;
			}
			else if( 2 == v_szBuf[ i + 1 ] )
			{
				szbuf[ iNewLen ++ ] = 0x7e;
			}
			else
			{
				szbuf[ iNewLen ++ ] = v_szBuf[ i ];
				szbuf[ iNewLen ++ ] = v_szBuf[ i + 1 ];
			}
			i += 2;
		}
	}
	
	memcpy( v_szBuf, szbuf, iNewLen );
	return iNewLen;
}

void CCom150TR::DealCom150TRFrame(char *v_szBuf, int v_iLen)
{
	char szSendBuf[100] = {0};
	int  iSendLen = 0;
	DWORD dwPacketNum = 0;

	switch( v_szBuf[0] )
	{
	case 0x01:	//串口盒注册请求
		{
			szSendBuf[iSendLen++] = 0x81;		// 注册应答
			szSendBuf[iSendLen++] = 0x01;		// 支持连接
			m_objCom150TRWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
			m_bComConnect = true;

			PRTMSG(MSG_DBG, "Recv Com150TR connect!\n");

			// 注册后，进行各串口参数配置
			_SetComPara();		
		}
		break;

	case 0x02:	//串口盒心跳
		{
			if(m_bComConnect == false)
			{
				PRTMSG(MSG_NOR, "Recv Com150TR link, but is not connect, quit!\n");
				break;
			}
			//PRTMSG(MSG_DBG, "Recv Com150TR link!\n");

			szSendBuf[iSendLen++] = 0x82;		// 心跳应答
			m_bComConnect = true;
			m_objCom150TRWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
		}
		break;

	case 0x03:	//串口盒重新连接应答
		{
			// 重新连接后，再次进行各串口参数配置
			_SetComPara();	
		}
		break;

	case 0x11:	//串口参数设置的应答
		{
			if( v_szBuf[1] == 0x01 )	
			{
				PRTMSG(MSG_DBG, "Com150TR set para succ!\n");

				// 串口上电
				szSendBuf[iSendLen++] = 0xB3;
				szSendBuf[iSendLen++] = 0x01;
				szSendBuf[iSendLen++] = 0x01;
				m_objCom150TRWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
			}
			else	//若配置失败，则再次配置
			{
				_SetComPara();
			}
		}
		break;
		
	case 0x21:	// 来自扩展串口的数据
		{
			switch( v_szBuf[1] )
			{
			case 0x01:	// 调度屏或手柄使用扩展串口2
				{
					// 给扩展盒应答
					szSendBuf[iSendLen++] = 0xA1;
					m_objCom150TRWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
					
					v_szBuf[1] = 0x00;		// 0x00 表示使用扩展盒，从调度屏收到的数据
					DataPush(v_szBuf+1, v_iLen-1, DEV_QIAN, DEV_DIAODU, LV2);
				}
				break;
			}
		}
		break;

	case 0x30:	// 信号状态值
		{
			// 给扩展盒应答
			szSendBuf[iSendLen++] = 0xB0;
			m_objCom150TRWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);

			// 更新信号状态值
			pthread_mutex_lock(&m_mutexIOSignal);
			m_bytIOSignal = v_szBuf[2];

			int iPackCnt = v_szBuf[3];
			for(int i=0; i<iPackCnt; i++)
			{
				m_dwCycleCount += 256*v_szBuf[4+i*2] + v_szBuf[4+i*2+1];
			}
			pthread_mutex_unlock(&m_mutexIOSignal);
		}
		break;

	case 0x31:	// 信号状态变化通知
		{
			// 给扩展盒应答
			szSendBuf[iSendLen++] = 0xB1;
			m_objCom150TRWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);

			//	表示改变的信号ID和改变状态
			//	S ID6 ID5 ID4 ID3 ID2 ID1 ID0
			//	ID0~ID6 --表示信号ID，从0开始编码最大为127;
			//	S --表示信号改变状态, 1表示从无效变为有效,0表示从有效变为无效;
			//	ID6~ID0的值定义:
			//	00H --前车门
			//	01H --右灯
			//	02H --左灯
			//	03H --手刹
			//	04H --脚刹
			//	05H --前大灯
			//	06H --后大灯
			//	07H --后车门

			// 更新信号状态值
			pthread_mutex_lock(&m_mutexIOSignal);
			bool bValid = v_szBuf[1] & 0x80;
			byte bytIOID = v_szBuf[1] | 0x7f;
			switch( bytIOID )
			{
			case 0x01:
				if( bValid )	m_bytIOSignal |= 0x01;
				else			m_bytIOSignal &= ~(0x01);
				break;

			case 0x02:
				if( bValid )	m_bytIOSignal |= 0x02;
				else			m_bytIOSignal &= ~(0x02);
				break;

			case 0x03:
				if( bValid )	m_bytIOSignal |= 0x04;
				else			m_bytIOSignal &= ~(0x04);
				break;

			case 0x04:
				if( bValid )	m_bytIOSignal |= 0x10;
				else			m_bytIOSignal &= ~(0x10);
				break;

			case 0x05:
				if( bValid )	m_bytIOSignal |= 0x20;
				else			m_bytIOSignal &= ~(0x20);
				break;

			case 0x06:
				if( bValid )	m_bytIOSignal |= 0x40;
				else			m_bytIOSignal &= ~(0x40);
				break;

			case 0x07:
				if( bValid )	m_bytIOSignal |= 0x80;
				else			m_bytIOSignal &= ~(0x80);
				break;

			default:
				break;
			}
			
			pthread_mutex_unlock(&m_mutexIOSignal);
		}
		break;

	case 0x50:	// U盘状态通知
		{
			if( v_szBuf[1] == 0x00 )
			{
				PRTMSG(MSG_NOR, "150TR Udisk insert\n");
			}
			else
			{
				PRTMSG(MSG_NOR, "150TR Udisk Out\n");
			}
		}

	case 0x20:	// 送往扩展串口的数据接收指示应答
	case 0x32:	// 信号滤波参数设置请求应答
	case 0x40:	// 启动数据导出请求应答
	case 0x41:	// 导出数据接收指示应答
		break;
		
	default:
		break;
	}
}

void CCom150TR::_SetComPara()
{
	char szSendBuf[100] = {0};
	int iSendLen = 0;
	DWORD dwPacketNum;

	szSendBuf[iSendLen++] = 0x91;	// 命令

	szSendBuf[iSendLen++] = 0x01;	// 扩展串口ID

	szSendBuf[iSendLen++] = 0x00;	// 波特率, 9600
	szSendBuf[iSendLen++] = 0x00;
	szSendBuf[iSendLen++] = 0x25;
	szSendBuf[iSendLen++] = 0x80;

	szSendBuf[iSendLen++] = 0x08;	// 数据位8位

	szSendBuf[iSendLen++] = 0x00;	// 1位停止位

	szSendBuf[iSendLen++] = 0x00;	// 无校验

	szSendBuf[iSendLen++] = 0x00;	// 通信模式：收发

	szSendBuf[iSendLen++] = 0x02;	// 转发长度门槛（512）
	szSendBuf[iSendLen++] = 0x00;

	szSendBuf[iSendLen++] = 0x00;	// 转发时间门槛 200ms
	szSendBuf[iSendLen++] = 0xC8;

	m_objCom150TRWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
}

byte CCom150TR::GetIOSignal()
{
	byte bytIOSignal;

	pthread_mutex_lock(&m_mutexIOSignal);
	bytIOSignal = m_bytIOSignal;
	pthread_mutex_unlock(&m_mutexIOSignal);

	return bytIOSignal;
}

DWORD CCom150TR::GetCycleCount()
{
	DWORD dwCycleCount;

	pthread_mutex_lock(&m_mutexIOSignal);
	dwCycleCount = m_dwCycleCount;
	pthread_mutex_unlock(&m_mutexIOSignal);
	
	return dwCycleCount;
}

//--------------------------------------------------------------------------------------------------------------------------
// 检查校验和(直接取累加和)
// buf: 数据缓冲区指针
// len: 数据的长度
// 返回: 校验是否正确
bool CCom150TR::check_crc(const byte *buf, const int len)
{   
	byte check_sum = get_crc( buf+1, len-1 );
	return (buf[0] == check_sum);
}

//--------------------------------------------------------------------------------------------------------------------------
// 得到校验和的反码
byte CCom150TR::get_crc(const byte *buf, const int len)
{   
	byte check_sum = 0;
	for(int i=0; i<len; i++) {
		check_sum += *(buf+i);
	}
	check_sum ^= 0xff;
	
    return check_sum;
}

#endif

