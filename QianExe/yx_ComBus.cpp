#include "yx_QianStdAfx.h"

#if USE_COMBUS == 1

#undef MSG_HEAD 
#define MSG_HEAD	("QianExe-ComBus")

void *G_ThreadComBusRecv(void *arg)
{
	g_objComBus.P_ThreadComBusRecv();
}

void *G_ThreadComBusSend(void *arg)
{
	g_objComBus.P_ThreadComBusSend();
}

//////////////////////////////////////////////////////////////////////////
CComBus::CComBus()
{
	m_iComPort = 0;
	m_bComConnect = false;
}

CComBus::~CComBus()
{

}

int CComBus::Init()
{
	// 打开串口
	if( !ComOpen() )
	{
		PRTMSG(MSG_ERR, "Open Com failed!\n");
		return ERR_COM;
	}

	// 创建收发线程
	if( pthread_create(&m_pthreadRecv, NULL, G_ThreadComBusRecv, NULL) != 0
		|| pthread_create(&m_pthreadSend, NULL, G_ThreadComBusSend, NULL) != 0
		)
	{
		ComClose();
		PRTMSG(MSG_ERR, "Create Thread failed!\n");
		return ERR_THREAD;
	}

	PRTMSG(MSG_DBG, "Create ComBus thread succ!\n");

	return 0;
}

int CComBus::Release()
{
	ComClose();

	return 0;
}

bool CComBus::ComOpen()
{
	int iResult;
	struct termios options;

	m_iComPort = open("/dev/ttyAMA0", O_RDWR );

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

	PRTMSG(MSG_DBG,"ComBus open Succ!\n"); 

	return true;
}

bool CComBus::ComClose()
{
	if(m_iComPort != -1)
	{
		close(m_iComPort);
		m_iComPort = -1;

		return true;
	}

	return false;
}

int CComBus::ComWrite(char *v_szBuf, int v_iLen)
{
	int iWrited = 0;
	
	iWrited = write(m_iComPort, v_szBuf, v_iLen);

//	PRTMSG(MSG_DBG, "ComBus send one frame!");
//	PrintString(v_szBuf, v_iLen);
	
	return iWrited;
}

void CComBus::P_ThreadComBusRecv()
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
//			PRTMSG(MSG_DBG, "ComBus recv data!\n");
//			PrintString(szReadBuf, iReadLen);
			AnalyseComFrame(szReadBuf, iReadLen);
		}
	}
}

void CComBus::P_ThreadComBusSend()
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
		if( !m_objComBusReadMng.PopData(bytLvl, dwBufSize, dwRecvLen, szRecvBuf, dwPushTm))
		{
			if( dwRecvLen <= dwBufSize )
				DealComBusFrame(szRecvBuf, (int)dwRecvLen);
		}
		
		// 再向串口发送数据
		if( !m_objComBusWorkMng.PopData(bytLvl, dwBufSize, dwRecvLen, szRecvBuf, dwPushTm))
		{
			szSendBuf[1] = 2;	// 协议版本，固定取2
			szSendBuf[2] = 3;	// 外设编号，固定取3
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

void CComBus::AnalyseComFrame(char *v_szBuf, int v_iLen)
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
					iRet = m_objComBusReadMng.PushData(LV1, (DWORD)(m_iComFrameLen-4), m_szComFrameBuf+4, dwPacketNum);
					if( 0 != iRet )
					{
						PRTMSG(MSG_ERR, "Push one frame failed:%d\n",iRet);
						memset(m_szComFrameBuf, 0, m_iComFrameLen);
						m_iComFrameLen = 0;
					}					
				}
				else
				{
					PRTMSG(MSG_ERR, "ComBus frame check crc err!\n");
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

int CComBus::TranData(char *v_szBuf, int v_iLen)
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

int CComBus::DetranData(char *v_szBuf, int v_iLen)
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

void CComBus::DealComBusFrame(char *v_szBuf, int v_iLen)
{
	char szSendBuf[100] = {0};
	int  iSendLen = 0;
	DWORD dwPacketNum = 0;

	switch( v_szBuf[0] )
	{
	case 0x01:	//串口盒注册请求
		{
			szSendBuf[iSendLen++] = 0x81;		// 注册应答
			m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
			m_bComConnect = true;

			PRTMSG(MSG_DBG, "Recv ComBus connect!\n");

			// 注册后，进行各串口参数配置
			_SetComPara();		
		}
		break;

	case 0x02:	//串口盒心跳
		{
			if(m_bComConnect == false)
			{
				PRTMSG(MSG_NOR, "Recv ComBus link, but is not connect, quit!\n");
				break;
			}
			//PRTMSG(MSG_DBG, "Recv ComBus link!\n");

			szSendBuf[iSendLen++] = 0x82;		// 心跳应答
			m_bComConnect = true;
			m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
		}
		break;

	case 0x03:	//串口盒重新连接应答
		{
			// 重新连接后，再次进行各串口参数配置
			_SetComPara();	
		}
		break;

	case 0x43:	//串口参数设置的应答
		{
			if( v_szBuf[1] == 0x00 )	//若配置失败，则再次配置
				_SetComPara();
			else
				PRTMSG(MSG_DBG, "ComBus set para succ!\n");
		}
		break;
		
	case 0x70:	//物理串口2（计价器）
		{
			// 应答
			szSendBuf[iSendLen++] = 0xf0;
			m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
#if USE_METERTYPE == 1			
			g_objMeter.DealComBuf(v_szBuf+1, v_iLen-1);
#endif
		}
		break;
		
	case 0x72:	//物理串口3（调度屏）
		{
			// 应答
			szSendBuf[iSendLen++] = 0xf2;
			m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
			
			v_szBuf[0] = 0x00;		// 0x00 表示使用扩展盒，从调度屏收到的数据
			DataPush(v_szBuf, v_iLen, DEV_QIAN, DEV_DIAODU, LV2);
		}
		break;
		
	case 0x74:
		break;
	
	case 0x76:	//模拟串口2（LED顶灯）
		{
			// 应答
			szSendBuf[iSendLen++] = 0xf7;
			m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);

#if USE_LEDTYPE == 1
			g_objLedBohai.DealComData_V1_2(v_szBuf+1, v_iLen-1);
#endif

#if USE_LEDTYPE == 2
			if( v_iLen - 1 == 1)
				g_objLedChuangLi.m_objReadMng.PushData((BYTE)LV1, 1, (char*)v_szBuf+1, dwPacketNum);
#endif
		}
		break;

	case 0x78:
	case 0x7a:
	case 0x71://扩展盒物理串口2收到数据应答	
	case 0x73://扩展盒物理串口3收到数据应答
	case 0x75://扩展盒模拟串口1收到数据应答	
	case 0x79://扩展盒模拟串口3收到数据应答
	default:
		break;
	}
}

void CComBus::_SetComPara()
{
	char szSendBuf[100] = {0};
	int iSendLen = 0;
	DWORD dwPacketNum;

	szSendBuf[iSendLen++] = 0xC3;	// 命令
	szSendBuf[iSendLen++] = 0x1f;	// 设置物理串口3，模拟串口2，模拟串口1

	szSendBuf[iSendLen++] = 0x02;	// 物理串口2波特率, 2400
	szSendBuf[iSendLen++] = 0x01;	// 物理串口2数据位，8位
	szSendBuf[iSendLen++] = 0x01;	// 物理串口2停止位，1位
	szSendBuf[iSendLen++] = 0x01;	// 物理串口2校验位，无校验
	
	szSendBuf[iSendLen++] = 0x04;	// 物理串口3波特率, 9600
	szSendBuf[iSendLen++] = 0x01;	// 物理串口3数据位，8位
	szSendBuf[iSendLen++] = 0x01;	// 物理串口3停止位，1位
	szSendBuf[iSendLen++] = 0x01;	// 物理串口3校验位，无校验

	szSendBuf[iSendLen++] = 0x02;	// 模拟串口1波特率, 2400
	szSendBuf[iSendLen++] = 0x01;	// 模拟串口1数据位，8位
	szSendBuf[iSendLen++] = 0x01;	// 模拟串口1停止位，1位
	szSendBuf[iSendLen++] = 0x01;	// 模拟串口1校验位，无校验

	szSendBuf[iSendLen++] = 0x04;	// 模拟串口2波特率, 9600
	szSendBuf[iSendLen++] = 0x01;	// 模拟串口2数据位，8位
	szSendBuf[iSendLen++] = 0x01;	// 模拟串口2停止位，1位
	szSendBuf[iSendLen++] = 0x01;	// 模拟串口2校验位，无校验
	
	szSendBuf[iSendLen++] = 0x02;	// 模拟串口3波特率, 2400
	szSendBuf[iSendLen++] = 0x01;	// 模拟串口3数据位，8位
	szSendBuf[iSendLen++] = 0x01;	// 模拟串口3停止位，1位
	szSendBuf[iSendLen++] = 0x01;	// 模拟串口3校验位，无校验

	m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
}

//--------------------------------------------------------------------------------------------------------------------------
// 检查校验和(直接取累加和)
// buf: 数据缓冲区指针
// len: 数据的长度
// 返回: 校验是否正确
bool CComBus::check_crc(const byte *buf, const int len)
{   
	byte check_sum = get_crc( buf+1, len-1 );
	return (buf[0] == check_sum);
}

//--------------------------------------------------------------------------------------------------------------------------
// 得到校验和的反码
byte CComBus::get_crc(const byte *buf, const int len)
{   
	byte check_sum = 0;
	for(int i=0; i<len; i++) {
		check_sum += *(buf+i);
	}
	check_sum ^= 0xff;
	
    return check_sum;
}

#endif

