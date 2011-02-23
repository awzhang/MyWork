#include "yx_QianStdAfx.h"

#if USE_OIL == 3

#undef MSG_HEAD 
#define MSG_HEAD	("QianExe-Oil  ")

void *G_ThreadOilRecv(void *arg)
{
	g_objOil.P_ThreadRecv();
}

void *G_ThreadOilSend(void *arg)
{
	g_objOil.P_ThreadSend();
}

//////////////////////////////////////////////////////////////////////////

COil::COil()
{
	m_iComPort = 0;
	memset(m_szComFrameBuf, 0, sizeof(m_szComFrameBuf));
	m_iComFrameLen = 0;

	m_bComConnect = false;

	m_objRecvMng.InitCfg(10*1024, 10*1000);
	m_objSendMng.InitCfg(10*1024, 10*1000);

	m_bytRptType = 0;
	m_bytOilSta = 0;
	m_usOilAD = 0;
	m_usPowAD = 0;

	memset(m_usONRealOilData, 0, sizeof(m_usONRealOilData));
	memset(m_usOFFRealOilData, 0, sizeof(m_usOFFRealOilData));
}

COil::~COil()
{

}

int COil::Init()
{
	// 打开串口
	if( !ComOpen() )
	{
		PRTMSG(MSG_ERR, "Open Com failed!\n");
		return ERR_COM;
	}

	pthread_mutex_init(&m_MutexAD, NULL);

	// 创建线程
	pthread_t pThreadRecv, pThreadSend;
	if( pthread_create(&pThreadRecv, NULL, G_ThreadOilRecv, NULL) != 0
		||
		pthread_create(&pThreadSend, NULL, G_ThreadOilSend, NULL) != 0
		)
	{
		PRTMSG(MSG_ERR, "Create thread failed!\n");
		perror("");
		return ERR_THREAD;
	}

	return 0;
}

int COil::Release()
{
	pthread_mutex_destroy(&m_MutexAD);
}

int COil::ComOpen()
{
	int iResult;
	struct termios options;
	
	m_iComPort = open("/dev/ttySIM1", O_RDWR );//| O_NONBLOCK);
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
	
	PRTMSG(MSG_DBG,"COil open Succ!\n"); 
	
	return true;
}

int COil::ComWrite(char *v_szBuf, int v_iLen)
{
	int iWrited = 0;
	
	iWrited = write(m_iComPort, v_szBuf, v_iLen);
	
 	//PRTMSG(MSG_DBG, "COil send data: ");
 	//PrintString(v_szBuf, v_iLen);
	
	return iWrited;
}

void COil::P_ThreadRecv()
{
	char szReadBuf[2048] = {0};
	int  iBufLen = sizeof(szReadBuf);
	char szFrame[2048] = {0};
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
   			//PRTMSG(MSG_DBG, "COil recv data: ");
   			//PrintString(szReadBuf, iReadLen);

			AnalyseComData(szReadBuf, iReadLen);
		}
	}
}

void COil::P_ThreadSend()
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
		if( !m_objRecvMng.PopData(bytLvl, dwBufSize, dwRecvLen, szRecvBuf, dwPushTm))
		{
			if( dwRecvLen <= dwBufSize )
				DealComFrame(szRecvBuf, (int)dwRecvLen);
		}
		
		// 再向串口发送数据
		if( !m_objSendMng.PopData(bytLvl, dwBufSize, dwRecvLen, szRecvBuf, dwPushTm))
		{
			szSendBuf[1] = 2;	// 协议版本，固定取1
			szSendBuf[2] = 4;	// 外设编号，固定取4
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

void COil::AnalyseComData(char *v_szBuf, int v_iLen)
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
					DWORD dwPacketNum = 0;
					m_objRecvMng.PushData(LV2, m_iComFrameLen-4, m_szComFrameBuf+4, dwPacketNum);					
				}
				else
				{
					PRTMSG(MSG_ERR, "Oil frame check crc err!\n");
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

void COil::DealComFrame(char *v_szBuf, int v_iLen)
{
	char szSendBuf[100] = {0};
	int  iSendLen = 0;
	DWORD dwPacketNum = 0;

	switch( v_szBuf[0] )
	{
	case 0x01:		// 连接注册请求
		{
			szSendBuf[iSendLen++] = 0x81;		// 注册应答
			szSendBuf[iSendLen++] = 0x01;		// 支持连接
			m_objSendMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
			
			m_bComConnect = true;
			
			PRTMSG(MSG_DBG, "Recv Oil connect!\n");
		}
		break;

	case 0x02:		// 链路维护请求
		{
			if(m_bComConnect == false)
			{
				PRTMSG(MSG_NOR, "Recv Oil link, but is not connect, quit!\n");
				break;
			}
			
			szSendBuf[iSendLen++] = 0x82;		// 心跳应答
			m_bComConnect = true;
			m_objSendMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);
		}
		break;

	case 0x40:		// 参数设置请求应答
		{
			char szbuf[1024] = {0};
			int  ilen = 0;

			// 向中心应答
			g_objSms.MakeSmsFrame(v_szBuf+1, 1, 0x37, 0x50, szbuf,sizeof(szbuf),ilen,CVT_8TO7);
			g_objSock.SOCKSNDSMSDATA(szbuf,ilen,LV12,DATASYMB_SMSBACKUP);
		}
		break;

	case 0x41:		// 数据获取请求应答
	case 0x43:		// 标定起始请求应答	
	case 0x44:		// 标定结束请求应答
	case 0x45:		// 版本查询请求应答
	case 0x46:		// 状态查询请求应答
		{
			// 保证队列内只有一条数据
			if( m_QueRecv.getcount() )
				m_QueRecv.reset();
			
			m_QueRecv.push((byte*)v_szBuf, v_iLen);
		}
		break;

	case 0x42:		// 主动上报
		{
			szSendBuf[iSendLen++] = 0xC2;		// 主动上报应答
			m_objSendMng.PushData((BYTE)LV1, (DWORD)iSendLen, (char*)szSendBuf, dwPacketNum);

			byte bytRptType = 0;
			byte bytOilSta = 0;
			unsigned short usValidOilAD = 0;
			unsigned short usValidPowAD = 0;

			bytRptType = (byte)v_szBuf[1];
			bytOilSta = (byte)v_szBuf[2];
			usValidOilAD = ((byte)v_szBuf[3])*256 + (byte)v_szBuf[4];
			usValidPowAD = ((byte)v_szBuf[5])*256 + (byte)v_szBuf[6];

			SetOilAndPowAD(bytRptType, bytOilSta, usValidOilAD, usValidPowAD);
		}
		break;

	default:
		break;
	}
}

int COil::DealDiaoDu(char *v_szData, DWORD v_dwDataLen)
{
	if( !v_szData || v_dwDataLen < 1 ) 
		return ERR_PAR;
	
	switch( BYTE(v_szData[0]) )
	{
	case 0x00:
		_DealComuBD00(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0x14:
		_DealComuBD14(v_szData+1,v_dwDataLen-1);
		break;
		
    case 0x12:
		_DealComuBD12(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0x34:
		_DealComuBD34(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0x10:
		_DealComuBDFull(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0xF1:
		_DealComuBDF1(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0xF2:
		_DealComuBDF2(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0xF3:
		_DealComuBDF3(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0xF4:
		_DealComuBDF4(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0xF5:
		_DealComuBDF5(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0x01:
		_DealComuC1(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0x03:
		_DealComuC3(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0x04:
		_DealComuC4(v_szData+1,v_dwDataLen-1);
		break;
		
	case 0x51:
		_DealComu51(v_szData+1,v_dwDataLen-1);
		break;
		
	default:
		return ERR_PAR;
	}

	return 0;
}

int COil::_DealComuBD00(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBD00!\n");

	int iRet = 0;
	char buf[200] = {0};

	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份

	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{
		objOilBDCfg.m_aryONOilBiaoding[0][0] = usOilAD;
		objOilBDCfg.m_aryONOilBiaoding[0][1] = usPowAD;

	    //存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBD00_END;
		}

		buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf[1] = 0x00;
    	DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}

DealComuBD00_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF6;
    DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBD12(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBD12!\n");

	int iRet = 0;
	char buf[200] = {0};

	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份

	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{
		objOilBDCfg.m_aryONOilBiaoding[2][0] = usOilAD;
		objOilBDCfg.m_aryONOilBiaoding[2][1] = usPowAD;

	    //存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBD12_END;
		}

		buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf[1] = 0x12;
    	DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}

DealComuBD12_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF6;
    DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBD14(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "_DealComuBD14!\n");

	int iRet = 0;
	char buf[200] = {0};
	
	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;
	
	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
	
	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{
		objOilBDCfg.m_aryONOilBiaoding[1][0] = usOilAD;
		objOilBDCfg.m_aryONOilBiaoding[1][1] = usPowAD;
		
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBD14_END;
		}
		
		buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf[1] = 0x14;
		DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		
		return 0;
	}
	
DealComuBD14_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF6;
    DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBD34(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBD34!\n");

	int iRet = 0;
	char buf[200] = {0};
	
	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;
	
	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
	
	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{
		objOilBDCfg.m_aryONOilBiaoding[3][0] = usOilAD;
		objOilBDCfg.m_aryONOilBiaoding[3][1] = usPowAD;
		
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBD34_END;
		}
		
		buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf[1] = 0x34;
		DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		
		return 0;
	}
	
DealComuBD34_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF6;
    DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDF1(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBDF1!\n");

	int iRet = 0;
	char buf[200] = {0};
	
	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;
	
	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
	
	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{
		objOilBDCfg.m_aryOFFOilBiaoding[0][0] = usOilAD;
		objOilBDCfg.m_aryOFFOilBiaoding[0][1] = usPowAD;
		
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDF1_END;
		}
		
		buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf[1] = 0xF1;
		DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		
		return 0;
	}
	
DealComuBDF1_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF6;
    DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDF2(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "_DealComuBDF2!\n");

	int iRet = 0;
	char buf[200] = {0};
	
	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;
	
	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
	
	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{
		objOilBDCfg.m_aryOFFOilBiaoding[1][0] = usOilAD;
		objOilBDCfg.m_aryOFFOilBiaoding[1][1] = usPowAD;
		
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDF2_END;
		}
		
		buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf[1] = 0xF2;
		DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		
		return 0;
	}
	
DealComuBDF2_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF6;
    DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDF3(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBDF3!\n");

	int iRet = 0;
	char buf[200] = {0};
	
	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;
	
	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
	
	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{
		objOilBDCfg.m_aryOFFOilBiaoding[2][0] = usOilAD;
		objOilBDCfg.m_aryOFFOilBiaoding[2][1] = usPowAD;
		
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDF3_END;
		}
		
		buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf[1] = 0xF3;
		DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		
		return 0;
	}
	
DealComuBDF3_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF6;
    DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDF4(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBDF4!\n");

	int iRet = 0;
	char buf[200] = {0};
	
	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;
	
	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
	
	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{
		objOilBDCfg.m_aryOFFOilBiaoding[3][0] = usOilAD;
		objOilBDCfg.m_aryOFFOilBiaoding[3][1] = usPowAD;
		
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDF4_END;
		}
		
		buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf[1] = 0xF4;
		DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		
		return 0;
	}
	
DealComuBDF4_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF6;
    DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDF5(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBDF5!\n");

	int iRet = 0;
	char buf[200] = {0};
	
	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;
	
	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
	
	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{
		objOilBDCfg.m_aryOFFOilBiaoding[4][0] = usOilAD;
		objOilBDCfg.m_aryOFFOilBiaoding[4][1] = usPowAD;
		
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDF5_END;
		}
		
		buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf[1] = 0xF5;
		DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		
		return 0;
	}
	
DealComuBDF5_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF6;
    DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDFull(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBDFull!\n");

	int iRet = 0;
	char buf[200] = {0};
	
	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;
	
	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
	
	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{
		objOilBDCfg.m_aryONOilBiaoding[4][0] = usOilAD;
		objOilBDCfg.m_aryONOilBiaoding[4][1] = usPowAD;
		
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDFF_END;
		}
		
		buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf[1] = 0x44;
		DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
		
		return 0;
	}
	
DealComuBDFF_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF6;
    DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuC1(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuC1!\n");

	char buf[200] = {0};
	int  len = 0;

	buf[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xF0;
	
	byte bytOilStatus = 0;
	unsigned short usOilAD = 0;
	unsigned short usPowAD = 0;
	
	if( !QueryOilData(bytOilStatus, usOilAD, usPowAD) )
	{ 		
		buf[2] = 0x01;
		memcpy(buf+3,&usOilAD,2);
		memcpy(buf+5,&usPowAD,2);
		len = 7;    
	}
	else
	{
		buf[2] = 0x02;
		len = 3;    
	}

	DataPush(buf, len, DEV_QIAN, DEV_DIAODU, LV2);
	return 0;
}

int COil::_DealComuC3(char *v_szData, DWORD v_dwDataLen)
{
	char buf[3] = {0};
	buf[0] = 0x01;	// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xC3;	// 标定指示应答 

	if( !QueryBiaoDingStart() )
	{
		buf[2] = 0x01;
	}
	else
	{
		buf[2] = 0x02;
	}

	DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);

    return 0;
}

int COil::_DealComuC4(char *v_szData, DWORD v_dwDataLen)
{
	char buf[3] = {0};
	buf[0] = 0x01;	// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xC4;	// 标定指示应答 
	
	if( !QueryBiaoDingEnd() )
	{
		buf[2] = 0x01;
	}
	else
	{
		buf[2] = 0x02;
	}
	
	DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);
	
    return 0;
}

int COil::_DealComu51(char *v_szData, DWORD v_dwDaraLen)
{
	char buf[1024] = {0};
	char buf1[500] = {0};
	char buf2[500] = {0};
	int iBufLen = 0;
	int iBufLen1 = 0;
	
	tag1QOilBiaodingCfg objOilBDCfg;
	GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg),	offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	
	memcpy(m_usONRealOilData,objOilBDCfg.m_aryONOilBiaoding,sizeof(m_usONRealOilData));
	memcpy(m_usOFFRealOilData,objOilBDCfg.m_aryOFFOilBiaoding,sizeof(m_usOFFRealOilData));
	int len = 0;
    buf1[len++] = BYTE(m_usONRealOilData[0][0]>>8);
	buf1[len++] = BYTE(m_usONRealOilData[0][0]%256);
	buf1[len++] = BYTE(m_usONRealOilData[0][1]>>8);
	buf1[len++] = BYTE(m_usONRealOilData[0][1]%256);
	
    buf1[len++] = BYTE(m_usONRealOilData[1][0]>>8); 
	buf1[len++] = BYTE(m_usONRealOilData[1][0]%256); 
    buf1[len++] = BYTE(m_usONRealOilData[1][1]>>8); 
	buf1[len++] = BYTE(m_usONRealOilData[1][1]%256); 
	
	buf1[len++] = BYTE(m_usONRealOilData[2][0]>>8); 
	buf1[len++] = BYTE(m_usONRealOilData[2][0]%256); 
	buf1[len++] = BYTE(m_usONRealOilData[2][1]>>8); 
	buf1[len++] = BYTE(m_usONRealOilData[2][1]%256); 
	
	buf1[len++] = BYTE(m_usONRealOilData[3][0]>>8); 
	buf1[len++] = BYTE(m_usONRealOilData[3][0]%256); 
	buf1[len++] = BYTE(m_usONRealOilData[3][1]>>8); 
	buf1[len++] = BYTE(m_usONRealOilData[3][1]%256); 
	
	
	buf1[len++] = BYTE(m_usONRealOilData[4][0]>>8); 
	buf1[len++] = BYTE(m_usONRealOilData[4][0]%256); 
	buf1[len++] = BYTE(m_usONRealOilData[4][1]>>8); 
	buf1[len++] = BYTE(m_usONRealOilData[4][1]%256); 
	
	buf1[len++] = BYTE(m_usOFFRealOilData[0][0]>>8);
	buf1[len++] = BYTE(m_usOFFRealOilData[0][0]%256);
	buf1[len++] = BYTE(m_usOFFRealOilData[0][1]>>8);
	buf1[len++] = BYTE(m_usOFFRealOilData[0][1]%256);
	
    buf1[len++] = BYTE(m_usOFFRealOilData[1][0]>>8); 
	buf1[len++] = BYTE(m_usOFFRealOilData[1][0]%256); 
    buf1[len++] = BYTE(m_usOFFRealOilData[1][1]>>8); 
	buf1[len++] = BYTE(m_usOFFRealOilData[1][1]%256); 
	
	buf1[len++] = BYTE(m_usOFFRealOilData[2][0]>>8); 
	buf1[len++] = BYTE(m_usOFFRealOilData[2][0]%256); 
	buf1[len++] = BYTE(m_usOFFRealOilData[2][1]>>8); 
	buf1[len++] = BYTE(m_usOFFRealOilData[2][1]%256); 
	
	buf1[len++] = BYTE(m_usOFFRealOilData[3][0]>>8); 
	buf1[len++] = BYTE(m_usOFFRealOilData[3][0]%256); 
	buf1[len++] = BYTE(m_usOFFRealOilData[3][1]>>8); 
	buf1[len++] = BYTE(m_usOFFRealOilData[3][1]%256); 
	
	
	buf1[len++] = BYTE(m_usOFFRealOilData[4][0]>>8); 
	buf1[len++] = BYTE(m_usOFFRealOilData[4][0]%256); 
	buf1[len++] = BYTE(m_usOFFRealOilData[4][1]>>8); 
	buf1[len++] = BYTE(m_usOFFRealOilData[4][1]%256);
	
	int iRet = g_objSms.MakeSmsFrame((char *)&buf1,len,0x37,0x51,buf,sizeof(buf),iBufLen,CVT_8TO7);
	if(!iRet)
	{
		g_objSock.SOCKSNDSMSDATA(buf,iBufLen,LV12,DATASYMB_SMSBACKUP);
	}
	
	return iRet;	
}

// 设置油量检测盒参数请求
int COil::Deal3710(byte *v_szData, int v_dwDataLen)
{
	char szbuf[1024] = {0};

	szbuf[0] = 0xC0;	// 命令类型：参数设置请求

	szbuf[1] = 0x31;	// 设置类型，默认b0-b5为1

	// 车台主动上报周期较混乱，此处默认设置油耗盒的主动上报周期为40s
	szbuf[2] = 0x00;
	szbuf[3] = 0x28;

	memcpy(szbuf+4, v_szData, v_dwDataLen);

	//发送到油耗盒，在油耗盒给出应答时，再给中心应答
	DWORD dwPacketNum = 0;
	m_objSendMng.PushData(LV2, v_dwDataLen+4, szbuf, dwPacketNum);

	return 0;
}

// 油量检测盒标定数据接收应答
int COil::Deal3711(byte *v_szData, int v_dwDataLen)
{
	char buf[3] = {0};
	buf[0] = 0x01;	// 0x01表示要发往调度屏或者手柄的数据
	buf[1] = 0xC1;	// 标定数据接收应答 
	
	if( 0x01 == v_szData[0] )
	{
		buf[2] = 0x01;
	}
	else
	{
		buf[2] = 0x02;
	}
	
	DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);
	
    return 0;
}

// 查询油量检测盒状态请求
int COil::Deal3712(byte *v_szData, int v_dwDataLen)
{
	char szResType = 0x00;		// 默认为油耗盒已连接，油浮子已连接

	// 油耗盒连接状态
	if( !m_bComConnect )
		szResType |= 0x01;

	// 油浮子连接状态
	if( QueryStatus() )
		szResType |= 0x02;

	// 向中心应答
	char szbuf[1024] = {0};
	int  ilen = 0;
	g_objSms.MakeSmsFrame(&szResType, 1, 0x37, 0x52, szbuf,sizeof(szbuf),ilen,CVT_8TO7);
	g_objSock.SOCKSNDSMSDATA(szbuf,ilen,LV12,DATASYMB_SMSBACKUP);

	return 0;
}

// 查询油量检测盒版本请求
int COil::Deal3713(byte *v_szData, int v_dwDataLen)
{
	char buf[100] = {0};
	int  len = 0;
	
	if( !QueryVersion((char*)buf, sizeof(buf), len) )
	{
		// 向中心应答
		char szbuf[1024] = {0};
		int  ilen = 0;
		g_objSms.MakeSmsFrame(buf, len, 0x37, 0x53, szbuf,sizeof(szbuf),ilen,CVT_8TO7);
		g_objSock.SOCKSNDSMSDATA(szbuf,ilen,LV12,DATASYMB_SMSBACKUP);

		return 0;
	}

	return 1;
}

// 转义，7e->7d+02, 7d->7d+01
int COil::TranData(char *v_szBuf, int v_iLen)
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

// 反转义
int COil::DetranData(char *v_szBuf, int v_iLen)
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

bool COil::bConnect()
{
	return m_bComConnect;
}

// 查询实时油量数据
int COil::QueryOilData(byte &v_bytOilSta, unsigned short &v_usOilAD, unsigned short &v_usPowAD)
{
	char buf[3] = {0xC1, 0x00, 0x00};
	DWORD dwPacketNum = 0;
	m_objSendMng.PushData(LV2, 3, buf,dwPacketNum);

	// 等待应答
	char buf1[100] = {0};
	DWORD dwBeginTm = GetTickCount();
	while( GetTickCount() - dwBeginTm < 3000 )
	{
		usleep(20000);

		if( m_QueRecv.pop((byte*)buf1) && buf1[0] == 0x41 )
		{
			byte bytOilSta = 0;
			unsigned short usValidOilAD = 0;
			unsigned short usValidPowAD = 0;
			
			bytOilSta = (byte)buf1[1];
			usValidOilAD = ((byte)buf1[2])*256 + (byte)buf1[3];
			usValidPowAD = ((byte)buf1[4])*256 + (byte)buf1[5];
			
			SetOilAndPowAD(0x00, bytOilSta, usValidOilAD, usValidPowAD);

			v_bytOilSta = bytOilSta;
			v_usOilAD = usValidOilAD;
			v_usPowAD = usValidPowAD;

			return 0;
		}
	}

	return 1;
}

// 查询版本号
int COil::QueryVersion(char *v_szBuf, int v_iBufLen, int &v_ilen)
{
	char buf = 0xC5;
	DWORD dwPacketNum = 0;
	m_objSendMng.PushData(LV2, 1, &buf,dwPacketNum);
	
	// 等待应答
	char buf1[100] = {0};
	int  len = 0;
	DWORD dwBeginTm = GetTickCount();
	while( GetTickCount() - dwBeginTm < 3000 )
	{
		usleep(20000);
		
		if( (len = m_QueRecv.pop((byte*)buf1)) && buf1[0] == 0x45 )
		{
			if(len > v_iBufLen )
			{
				return 1;
			}
			else
			{
				memcpy(v_szBuf, buf1+1, len-1);
				v_ilen = len;
				return 0;
			}
		}
	}
	
	return 1;
}

// 查询油浮子连接状态
int COil::QueryStatus()
{
	char buf = 0xC6;
	DWORD dwPacketNum = 0;
	m_objSendMng.PushData(LV2, 1, &buf,dwPacketNum);
	
	// 等待应答
	char buf1[100] = {0};
	int  len = 0;
	DWORD dwBeginTm = GetTickCount();
	while( GetTickCount() - dwBeginTm < 3000 )
	{
		usleep(20000);
		
		if( (len = m_QueRecv.pop((byte*)buf1)) && buf1[0] == 0x46 )
		{
			if( buf1[1] == 0x01 )
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
	}
	
	return 1;
}

// 请求标定开始
int COil::QueryBiaoDingStart()
{
	char buf = 0xC3;
	DWORD dwPacketNum = 0;
	m_objSendMng.PushData(LV2, 1, &buf,dwPacketNum);
	
	// 等待应答
	char buf1[100] = {0};
	int  len = 0;
	DWORD dwBeginTm = GetTickCount();
	while( GetTickCount() - dwBeginTm < 3000 )
	{
		usleep(20000);
		
		if( (len = m_QueRecv.pop((byte*)buf1)) && buf1[0] == 0x43 )
			return 0;
	}
	
	return 1;
}

// 请求标定结束
int COil::QueryBiaoDingEnd()
{
	char buf = 0xC4;
	DWORD dwPacketNum = 0;
	m_objSendMng.PushData(LV2, 1, &buf,dwPacketNum);
	
	// 等待应答
	char buf1[100] = {0};
	int  len = 0;
	DWORD dwBeginTm = GetTickCount();
	while( GetTickCount() - dwBeginTm < 3000 )
	{
		usleep(20000);
		
		if( (len = m_QueRecv.pop((byte*)buf1)) && buf1[0] == 0x44 )
			return 0;
	}
	
	return 1;
}

bool COil::SetOilAndPowAD(byte v_bytRptType, byte v_bytOilSta, unsigned short v_usOilAD, unsigned short v_usPowAD)
{
	pthread_mutex_lock(&m_MutexAD);

	m_bytRptType = v_bytRptType;
	m_bytOilSta = v_bytOilSta;
	m_usOilAD = v_usOilAD;
	m_usPowAD = v_usPowAD;

	char buf[3] = {0};
	buf[0]	= 0x03;		// 0x03表示QianExe告知ComuExe的油耗AD值
	memcpy(buf+1, (void*)&m_usOilAD, 2);
	DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);

	pthread_mutex_unlock(&m_MutexAD);

	return true;
}

bool COil::GetOilAndPowAD(byte &v_bytRptType, byte &v_bytOilSta, unsigned short &v_usOilAD, unsigned short &v_usPowAD)
{
	pthread_mutex_lock(&m_MutexAD);

	v_bytRptType = m_bytRptType;
	v_bytOilSta = m_bytOilSta;
	v_usOilAD = m_usOilAD;
	v_usPowAD = m_usPowAD;
	
	pthread_mutex_unlock(&m_MutexAD);

	return true;
}

byte COil::get_crc(const byte *buf, const int len)
{
	byte check_sum = 0;
	for(int i=0; i<len; i++) {
		check_sum += *(buf+i);
	}
    return ~check_sum;
}

bool COil::check_crc(const byte *buf, const int len)
{   
	byte check_sum = get_crc( buf+1, len-1 );
	return (buf[0] == check_sum);
}

#endif


