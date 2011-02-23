#include "yx_QianStdAfx.h"

#if USE_OIL == 3

#undef MSG_HEAD 
#define MSG_HEAD	("QianExe-Oil")

void *G_ThreadOilRead(void *arg)
{
	g_objOil.P_ThreadRead();
}

void *G_ThreadAutoRptOil(void *arg)
{
	g_objOil.P_ThreadAutoRptOil();
}

//////////////////////////////////////////////////////////////////////////

#define ACC_OFFTOON		1	// b1 b0
#define ACC_ONTOOFF		2	// b1 b0
#define SPD_SILENTTOMOVE	4	// b3 b2
#define SPD_MOVETOSILENT	8	// b3 b2
const BYTE OILDATA_GUIYIHUA_POWAD = 204; // 参考底层驱动的归一化值815，精度上等于油耗盒的204（即815 / 4 约等于 204）

COil::COil()
{
	m_iComPort = 0;
	m_usRealOilAD = 0;
	m_usRealPowAD = 0;
	m_usValidOilAD = 0;
	m_usValidPowAD = 0;	
	m_bytOilSta = 0;
	m_bytRptType = 0;
	m_dwCmdSetTm = 0;
	m_bytOilAD = 0;
	m_bytOilSta = 0;

	m_bOilConnect = false;
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

	pthread_mutex_init(&m_mutex_Oil, NULL);
	pthread_mutex_init(&m_mutex_OilData, NULL);
	pthread_mutex_init(&m_mutex_COil, NULL);
	pthread_mutex_init(&m_mutex_cmd,NULL);

	// 创建读线程
	pthread_t pThreadRead, pThreadStatus;
	if( pthread_create(&pThreadRead, NULL, G_ThreadOilRead, NULL) != 0 
		||
		pthread_create(&pThreadStatus, NULL, G_ThreadAutoRptOil, NULL) != 0 )
	{
		PRTMSG(MSG_ERR, "Create thread failed!\n");
		perror("");
		return ERR_THREAD;
	}

	return 0;
}

int COil::Release()
{
	pthread_mutex_destroy(&m_mutex_Oil);
	pthread_mutex_destroy(&m_mutex_OilData);
	pthread_mutex_destroy(&m_mutex_COil);
	pthread_mutex_destroy(&m_mutex_cmd);
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
	
	PRTMSG(MSG_DBG, "COil send data: ");
	PrintString(v_szBuf, v_iLen);
	
	return iWrited;
}

void COil::P_ThreadRead()
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
			PRTMSG(MSG_DBG, "Oil recv data: ");
			PrintString(szReadBuf, iReadLen);		

			AnalyseComData(szReadBuf, iReadLen);
		}
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
					//保证堆栈中只有一条信息
					if (m_QueOilRecv.getcount()) 
						m_QueOilRecv.reset();

					// 推入接收队列时去掉校验码
					m_QueOilRecv.push((byte*)m_szComFrameBuf+1, m_iComFrameLen-1);
					
					// 若收到主动上报数据，则在此处就给予应答
					if(m_szComFrameBuf[4] == 0x42)
					{
						m_usValidOilAD = ((BYTE)m_szComFrameBuf[7])*256 + (BYTE)m_szComFrameBuf[8];
						m_usValidPowAD = ((BYTE)m_szComFrameBuf[9])*256 + (BYTE)m_szComFrameBuf[10];
						m_bytOilSta = m_szComFrameBuf[6];
						m_bytRptType = m_szComFrameBuf[5];
						
						sendfrm(0xC2,NULL,0);//主动上报应答
					}
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

void COil::P_ThreadAutoRptOil()
{
	DWORD dwSleepTm = 1000;
	int   iLinkErrCnt = 0;

	while( !g_bProgExit )
	{
		// 若未注册，则每隔1秒钟检查一次
		if( !m_bOilConnect )
		{
			if( g_objOil.deal_OilRpt() )
			{
				m_bOilConnect = true;
				dwSleepTm = 60000;
			}
			else
			{
				dwSleepTm = 1000;
			}
		}
		// 若已注册，则每隔60秒检查一次
		else
		{
			// 查询链路是否正常
			if( !g_objOil.query_link() )
			{
				iLinkErrCnt++;

				if( iLinkErrCnt > 3) 
				{ 
					m_bOilConnect = false;
					iLinkErrCnt = 0; 

					sendfrm(0x82,NULL,0);	// 请求油耗盒重新连接注册

					dwSleepTm = 1000;
					continue;
				}
			}
			else
			{
				iLinkErrCnt = 0;
			}	
		}

		usleep(dwSleepTm*1000);
	}
}

bool COil::sendfrm(BYTE cmd, char *param, int len)
{
	int i = 0, j = 0, k = 0, z =0;

	memset(buf_sp,0,ALABUF);
	memset(frm_sp,0,ALABUF);

	byte crc = 0;
	buf_sp[i++] = 0;
	buf_sp[i++] = 0x02;		//协议版本（固定）
	buf_sp[i++] = 0x04;	    //外设编码（固定）
	
	buf_sp[i++] = 0x00;    //100115ford,油耗盒新协议
	
	buf_sp[i++] = cmd;		//命令
	
	memcpy(buf_sp+i, param, len);	//参数(n)
	i+=len;
	for(z=1; z<i; z++)  
	{
		crc += buf_sp[z];
	}
	crc = ~crc;//100125ford,油耗盒新协议
	
    buf_sp[0] = crc;
	
	// 数据封帧
	frm_sp[j++] = 0x7e;
	for(k=0; k<i; k++) {
		if(0x7e == buf_sp[k])	
		{ frm_sp[j++] = 0x7d; frm_sp[j++] = 0x02; }//07.8.16 改成7e->7d+02 //原:frm[j++] = 0x01;
		else if(0x7d == buf_sp[k])	
		{ frm_sp[j++] = 0x7d; frm_sp[j++] = 0x01; }//07.8.16 改成7d->7d+01 //原：frm[j++] = 0x00;
		else 						
		{ frm_sp[j++] = buf_sp[k]; }
	}
	frm_sp[j++] = 0x7e;
	
	int iWrited = ComWrite(frm_sp, j);
		
	return (iWrited==j ? true : false);
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
	char buf1[200] = {0};
	char buf2[200] = {0};
	int len_snd = 0;

	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
 
	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);

	if(len_snd)
	{
		objOilBDCfg.m_aryONOilBiaoding[0][0] = ((BYTE)buf2[5])*256 + (BYTE)buf2[6];
		objOilBDCfg.m_aryONOilBiaoding[0][1] = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuC1_END;
		}
		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0x00;
		memcpy(buf1+2,buf2,9);
		DataPush(buf1, 11, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}

DealComuC1_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF6;
	DataPush(buf1, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBD12(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBD12!\n");

	int iRet = 0;
	char buf1[200] = {0};
	char buf2[200] = {0};
	int len_snd = 0;
	
	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份

	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);
	if(len_snd)
	{
	
		objOilBDCfg.m_aryONOilBiaoding[2][0] = ((BYTE)buf2[5])*256 + (BYTE)buf2[6];
		objOilBDCfg.m_aryONOilBiaoding[2][1] = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
	    //存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBD12_END;
		}
		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0x12;
		memcpy(buf1+2,buf2,9);
    	DataPush(buf1, 11, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}

DealComuBD12_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF6;
	DataPush(buf1, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBD14(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "_DealComuBD14!\n");

	int iRet = 0;
	char buf1[200] = {0};
	char buf2[200] = {0};
	int len_snd = 0;
		
	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = ::GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg),
		 offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份

	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);
	if(len_snd)
	{
		//从第三个开始取数
//		m_bytStatusType = buf[3];
//		if(g_pView) g_pView->DbgShowMsg2(L"buf[]:%x,%x,%x,%x,%x,%x,%x,%x",buf[4],buf[5],buf[6],buf[7],buf[8],buf[9],buf[10],buf[11]);
		//03 3b 05 65 03 3c 05 64
	//	m_usRealOilAD = buf[4]*256 + buf[5];
	//	m_usRealPowAD = buf[6]*256 + buf[7];
		objOilBDCfg.m_aryONOilBiaoding[1][0] = ((BYTE)buf2[5])*256 + (BYTE)buf2[6];
		objOilBDCfg.m_aryONOilBiaoding[1][1] = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
///		if(g_pView) g_pView->DbgShowMsg2(L"Biaoding11:%d,%d,%d",m_bytStatusType,m_usRealOilAD,m_usRealPowAD);
//		if(g_pView) g_pView->DbgShowMsg2(L"Biaoding22:%d,%d",m_usValidOilAD,m_usValidPowAD);
//		objOilBDCfg.m_aryONOilBiaoding[1][0] = m_usONRealOilData[1][0] = m_usValidOilAD;
//		objOilBDCfg.m_aryONOilBiaoding[1][1] = m_usONRealOilData[1][1] = m_usValidPowAD;
//		if(g_pView) g_pView->DbgShowMsg2(L"ValidOilAD11:%d",objOilBDCfg.m_aryONOilBiaoding[1][0]);
//		if(g_pView) g_pView->DbgShowMsg2(L"ValidPowAD22:%d",objOilBDCfg.m_aryONOilBiaoding[1][1]);
     //存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBD14_END;
		}
		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0x14;
		memcpy(buf1+2,buf2,9);
    	DataPush(buf1, 11, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}
DealComuBD14_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF6;
	DataPush(buf1, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBD34(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBD34!\n");

	int iRet = 0;
	char buf1[200] = {0};
	char buf2[200] = {0};
	int len_snd = 0;

	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份

	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);
	if(len_snd)
	{
	
		objOilBDCfg.m_aryONOilBiaoding[3][0] = ((BYTE)buf2[5])*256 + (BYTE)buf2[6];
		objOilBDCfg.m_aryONOilBiaoding[3][1] = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBD34_END;
		}
		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0x34;
		memcpy(buf1+2,buf2,9);
    	DataPush(buf1, 11, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}

DealComuBD34_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF6;
	DataPush(buf1, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDF1(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBDF1!\n");

	int iRet = 0;
	char buf1[200] = {0};
	char buf2[200] = {0};
	int len_snd = 0;
		
	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
   
	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);
	if(len_snd)
	{		
		objOilBDCfg.m_aryOFFOilBiaoding[0][0] = ((BYTE)buf2[5])*256 + (BYTE)buf2[6];
		objOilBDCfg.m_aryOFFOilBiaoding[0][1] = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
	
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDF1_END;
		}

		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0xF1;
		memcpy(buf1+2,buf2,9);
    	DataPush(buf1, 11, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}

DealComuBDF1_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF6;
	DataPush(buf1, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDF2(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "_DealComuBDF2!\n");

	int iRet = 0;
	char buf1[200] = {0};
	char buf2[200] = {0};
	int len_snd = 0;
	
	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
  
	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);
	if(len_snd)
	{
		objOilBDCfg.m_aryOFFOilBiaoding[1][0] = ((BYTE)buf2[5])*256 + (BYTE)buf2[6];
		objOilBDCfg.m_aryOFFOilBiaoding[1][1] = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
	
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDF2_END;
		}
	
		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0xF2;
		memcpy(buf1+2,buf2,9);
    	DataPush(buf1, 11, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}

DealComuBDF2_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF6;
	DataPush(buf1, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDF3(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBDF3!\n");

	int iRet = 0;
	char buf1[200] = {0};
	char buf2[200] = {0};
	int len_snd = 0;

	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份

	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);
	if(len_snd)
	{
	
		objOilBDCfg.m_aryOFFOilBiaoding[2][0] = ((BYTE)buf2[5])*256 + (BYTE)buf2[6];
		objOilBDCfg.m_aryOFFOilBiaoding[2][1] = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
	
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDF3_END;
		}

		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0xF3;
		memcpy(buf1+2,buf2,9);
    	DataPush(buf1, 11, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}
	
DealComuBDF3_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF6;
	DataPush(buf1, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDF4(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBDF4!\n");

	int iRet = 0;
	char buf1[200] = {0};
	char buf2[200] = {0};
	int len_snd = 0;

	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
  
	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);
	if(len_snd)
	{
	
		objOilBDCfg.m_aryOFFOilBiaoding[3][0] = ((BYTE)buf2[5])*256 +(BYTE)buf2[6];
		objOilBDCfg.m_aryOFFOilBiaoding[3][1] = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
	
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDF4_END;
		}

		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0xF4;
		memcpy(buf1+2,buf2,9);
    	DataPush(buf1, 11, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}

DealComuBDF4_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF6;
	DataPush(buf1, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDF5(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBDF5!\n");

	int iRet = 0;
	char buf1[200] = {0};
	char buf2[200] = {0};
	int len_snd = 0;
		
	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份
   
	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);
	if(len_snd)
	{
		objOilBDCfg.m_aryOFFOilBiaoding[4][0] = ((BYTE)buf2[5])*256 +(BYTE)buf2[6];
		objOilBDCfg.m_aryOFFOilBiaoding[4][1] = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
	
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDF5_END;
		}
	
		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0xF5;
		memcpy(buf1+2,buf2,9);
    	DataPush(buf1, 11, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}

DealComuBDF5_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF6;
	DataPush(buf1, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuBDFull(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuBDFull!\n");

	int iRet = 0;
	char buf1[200] = {0};
	char buf2[200] = {0};
	int len_snd = 0;

	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	tag1QOilBiaodingCfg objOilBDCfg,objOilBDCfgBkp;
	iRet = GetImpCfg(&objOilBDCfg,sizeof(objOilBDCfg), offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfg));
	objOilBDCfgBkp = objOilBDCfg; //先备份

	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);
	if(len_snd)
	{		
		objOilBDCfg.m_aryONOilBiaoding[4][0] = ((BYTE)buf2[5])*256 + (BYTE)buf2[6];
		objOilBDCfg.m_aryONOilBiaoding[4][1] = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
	
		//存储与处理 存入临界区
		if(iRet = SetImpCfg(&objOilBDCfg,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),	sizeof(objOilBDCfg)))
		{
			goto DealComuBDFull_END;
		}

		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0x44;
		memcpy(buf1+2,buf2,9);
    	DataPush(buf1, 11, DEV_QIAN, DEV_DIAODU, LV2);

		return 0;
	}

DealComuBDFull_END:
	SetImpCfg(&objOilBDCfgBkp,offsetof(tagImportantCfg,m_uni1QOilBiaodingCfg.m_obj1QOilBiaodingCfg),sizeof(objOilBDCfgBkp));
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF6;
	DataPush(buf1, 2, DEV_QIAN, DEV_DIAODU, LV2);
    return 0;
}

int COil::_DealComuC1(char *v_szData, DWORD v_dwDataLen)
{
	PRTMSG(MSG_DBG, "DealComuC1!\n");

	int iRet = 0;
	int len_snd = 0;
	char buf1[200] = {0};
	char buf2[200] ={0};

	DealAccChg();
	buf1[0] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle>>8);
	buf1[1] =BYTE(m_objDrvRecSampleCfg.m_wSampleCycle%256);

	len_snd = g_objOil.OilDataDesire(buf1,buf2,2);
	if(len_snd)
	{
		m_usRealOilAD = ((BYTE)buf2[1])*256 + (BYTE)buf2[2];
		m_usRealPowAD = ((BYTE)buf2[3])*256 + (BYTE)buf2[4];
		m_usValidOilAD = ((BYTE)buf2[5])*256 + (BYTE)buf2[6];
		m_usValidPowAD = ((BYTE)buf2[7])*256 + (BYTE)buf2[8];
 
		buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0xF0;
		buf1[2] = 0x01;
		memcpy(buf1+3,&m_usValidOilAD,2);
		memcpy(buf1+5,&m_usValidPowAD,2);
    	DataPush(buf1, 7, DEV_QIAN, DEV_DIAODU, LV2);
		return 0;
	}

DealComuC1_END:
	buf1[0] = 0x01;		// 0x01表示要发往调度屏或者手柄的数据
	buf1[1] = 0xF0;
	buf1[2] = 0x02;
	DataPush(buf1, 3, DEV_QIAN, DEV_DIAODU, LV2);

    return 0;
}

int COil::_DealComuC3(char *v_szData, DWORD v_dwDataLen)
{
	int len_snd = 0;
	len_snd = g_objOil.biaodDStart();
	if(len_snd)
	{
		buf_sp[0] = 0x01;	// 0x01表示要发往调度屏或者手柄的数据
		buf_sp[1] = 0xC3;	// 标定应答成功指示 
		buf_sp[2] = 0x01;
		DataPush(buf_sp, 3, DEV_QIAN, DEV_DIAODU, LV2);
	}
	else
	{
		buf_sp[0] = 0x01;	// 0x01表示要发往调度屏或者手柄的数据
		buf_sp[1] = 0xC3;	// 标定应答失败指示 
		buf_sp[2] = 0x02;
		DataPush(buf_sp, 3, DEV_QIAN, DEV_DIAODU, LV2);
	}
    return 0;
}

int COil::_DealComuC4(char *v_szData, DWORD v_dwDataLen)
{
	int len_snd = 0;
	len_snd = g_objOil.biaoDEnd();
	if(len_snd)
	{
		buf_sp[0] = 0x01;	// 0x01表示要发往调度屏或者手柄的数据
		buf_sp[1] = 0x4e;
		buf_sp[2] = 0x01;                    
		DataPush(buf_sp, 3, DEV_QIAN, DEV_DIAODU, LV2);
	}
	else
	{
		buf_sp[0] = 0x01;	// 0x01表示要发往调度屏或者手柄的数据
		buf_sp[1] = 0x4e;	//标定应答失败指示 
		buf_sp[2] = 0x02;    
		DataPush(buf_sp, 3, DEV_QIAN, DEV_DIAODU, LV2);
	}
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

int COil::biaodDStart()
{
	byte buf[20] ={0};
	
	for(int i=0;i<3;i++)
	{
		sendfrm(0xC3,NULL,0);

		ulong t1 =GetTickCount();

		while((GetTickCount()-t1)<5000)
		{
			usleep(200000);

			ulong l = m_QueOilRecv.pop(buf);

			if(buf[3] == 0x43)
			{
				return 1;
			}
		}
	}
	return 0;
}

int COil::biaoDEnd()
{
	byte buf[20] ={0};

	for(int i=0;i<3;i++)
	{
		sendfrm(0xC4,NULL,0);

		ulong t1 =GetTickCount();

		while((GetTickCount()-t1)<5000)
		{
			usleep(200000);

			ulong l = m_QueOilRecv.pop(buf);

			if(buf[3] == 0x44)
			{
				return 1;
			}
		}
	}
	return 0;
}

void COil::DealAccChg()
{
	tag2QDriveRecCfg objDrvRecCfg;
	objDrvRecCfg.Init();
	GetSecCfg( &objDrvRecCfg, sizeof(objDrvRecCfg),	offsetof(tagSecondCfg, m_uni2QDriveRecCfg.m_obj2QDriveRecCfg), sizeof(objDrvRecCfg) );
	
	// 修改内存中的采样发送配置
	//	tagDrvRecSampleCfg objDrvSampleCfg;
	if( g_objQianIO.GetDeviceSta() & DEVSTA_ACCVALID)
	{
		m_objDrvRecSampleCfg.m_wSampleCycle = objDrvRecCfg.m_wOnSampCycle;
		m_objDrvRecSampleCfg.m_bytSndCycle = objDrvRecCfg.m_bytOnSndCycle;
	}
	else
	{
		m_objDrvRecSampleCfg.m_wSampleCycle = objDrvRecCfg.m_wOffSampCycle;
		m_objDrvRecSampleCfg.m_bytSndCycle = objDrvRecCfg.m_bytOffSndCycle;
	}
	m_objDrvRecSampleCfg.m_bytSndWinSize = objDrvRecCfg.m_bytSndWinSize;
}

int COil::Deal3710(byte *v_szData, int v_dwDataLen)
{
	char buf[1024] = {0};
	int iRet = 0;
	BYTE len_snd = 0;
	int iBufLen = 0;
	BYTE bytResType;
	char buf1[500] = {0};
    tag2QOilCfg  objOilCfg, objOilCfgBkp,objOil;

	// 获取原始参数
	iRet = GetSecCfg( &objOilCfg, sizeof(objOilCfg), offsetof(tagSecondCfg, m_uni2QOilCfg.m_obj2QOilCfg), sizeof(objOilCfg) );
	if( iRet ) goto DEAL3710_FAILED;

	objOilCfgBkp = objOilCfg; // 先备份

	buf1[0] = 0x31; //设置类型，默认b0-b5为1
 
	DealAccChg();
	buf1[1] = m_objDrvRecSampleCfg.m_wSampleCycle>>8; //重新计算
	buf1[2] = m_objDrvRecSampleCfg.m_wSampleCycle%256;

	memcpy(buf1+3,v_szData,v_dwDataLen);

	objOil.m_SetType = 0x31;
	objOil.m_usRptPeriod = BYTE(m_objDrvRecSampleCfg.m_wSampleCycle);
    objOil.m_bytPowerCyc = BYTE(v_szData[0]); //分时供电周期
	objOil.m_usADMin = ((BYTE)v_szData[1])*256 + (BYTE)v_szData[2];
	objOil.m_usADMax = ((BYTE)v_szData[3])*256 + (BYTE)v_szData[4];
	objOil.m_usACCONAddOilYUZHI = ((BYTE)v_szData[5])*256 + (BYTE)v_szData[6];
	objOil.m_usACCONLeakOilYUZHI = ((BYTE)v_szData[7])*256 + (BYTE)v_szData[8];
	objOil.m_usACCOFFAddOilYUZHI = ((BYTE)v_szData[9])*256 + (BYTE)v_szData[10];
	objOil.m_usACCOFFLeakOilYUZHI = ((BYTE)v_szData[11])*256 + (BYTE)v_szData[12];
    objOil.m_usTimeToCheckOil = ((BYTE)v_szData[13])*256 + (BYTE)v_szData[14];
	objOil.m_bytNumToCheckOil = BYTE(v_szData[15]);

	//向油耗检测盒下发设置
    len_snd = g_objOil.oil_setting(buf1,v_dwDataLen+3);
    if(len_snd)
	{
	    bytResType = 0x01;

		iRet = g_objSms.MakeSmsFrame((char*)&bytResType,1,0x37,0x50,buf,sizeof(buf),iBufLen,CVT_8TO7);
		if(!iRet)
		{
			if(iRet = SetSecCfg(&objOil,offsetof(tagSecondCfg,m_uni2QOilCfg.m_obj2QOilCfg),	sizeof(objOil)))
			{
				goto DEAL3710_FAILED;
			}

			iRet = g_objSock.SOCKSNDSMSDATA(buf,iBufLen,LV12,DATASYMB_SMSBACKUP);
			if(iRet)
			{
				SetSecCfg(&objOilCfgBkp,offsetof(tagSecondCfg,m_uni2QOilCfg.m_obj2QOilCfg),	sizeof(objOilCfgBkp));
				goto DEAL3710_FAILED;
			}				
		}
	}
	else
		goto DEAL3710_FAILED;

	return 0; //不可缺少

DEAL3710_FAILED:
	bytResType = 0x00;
	int iRet2 = g_objSms.MakeSmsFrame((char*)&bytResType,1,0x37,0x50,buf,sizeof(buf),iBufLen,CVT_8TO7);
	if(!iRet2) g_objSock.SOCKSNDSMSDATA(buf,iBufLen,LV12,DATASYMB_SMSBACKUP);

   return iRet;
}

int COil::Deal3711(byte *v_szData, int v_dwDataLen)
{
	char buf1[100] = {0};

	if(0x01 == v_szData[0])
	{
		buf1[0] = 0x01;	// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0xC1;
		buf1[2] = 0x01;
		DataPush(buf1, 3, DEV_QIAN, DEV_DIAODU, LV2);
	}
	else
	{
		buf1[0] = 0x01;	// 0x01表示要发往调度屏或者手柄的数据
		buf1[1] = 0xC1;
		buf1[2] = 0x00;
		DataPush(buf1, 3, DEV_QIAN, DEV_DIAODU, LV2);
	}

	return 0;
}

int COil::Deal3712(byte *v_szData, int v_dwDataLen)
{
	char buf_snd[200] = {0};
	int iBufLen = 0;
	int iRet = 0;
	char buf1[ 500 ];

	buf_snd[0] = 0x00;	// 默认为油耗检测盒未连接、油浮子传感器未连接

	if( m_bOilConnect )		buf_snd[0] |= 0x01;

    byte bytSta = g_objOil.query_state();
	if( 0x01 == bytSta )	buf_snd[0] |= 0x02;
	
	iRet = g_objSms.MakeSmsFrame((char*)&buf_snd[0], 1, 0x37, 0x52, buf1, sizeof(buf1), iBufLen,CVT_8TO7);
	if(!iRet)
	{
		iRet = g_objSock.SOCKSNDSMSDATA( buf1, iBufLen, LV12);//, v_szSrcHandtel );
	}

	return 0;
}

int COil::Deal3713(byte *v_szData, int v_dwDataLen)
{
	char buf_snd[100] = {0};
	char bufup[100] = {0};
	char buf1[200];
	int len_snd = 0;
	int iBufLen = 0;
	int iRet = 0;

	//车载终端应答 
	//油量检测盒程序版本号长度（1） ＋ 油量检测盒程序版本号（N）
    //油量检测盒程序版本号  //例如：程序版本号：GK-150PD02-001

    len_snd = g_objOil.query_ver(buf_snd);

	//接受探询应答02
	if(len_snd)
	{
    	memcpy(bufup,buf_snd,len_snd );
		iRet = g_objSms.MakeSmsFrame((char*)&bufup, len_snd, 0x37, 0x53, buf1, sizeof(buf1), iBufLen,CVT_8TO7);
		if(!iRet)
		{
			iRet = g_objSock.SOCKSNDSMSDATA( buf1, iBufLen, LV12);//, v_szSrcHandtel );
		}
	}
	return 0;
}

int COil::deal_OilRpt()
{
	byte buf[20] = {0};
	char buf1[10] = {0};

	for(int i=0;i<3;i++)
	{
		//usleep(20000);

		ulong l = m_QueOilRecv.pop((unsigned char *)buf);

		if(l>0 && buf[3] == 0x01)
		{
			PRTMSG(MSG_NOR, "Recv Oil Register frame!\n");

			buf1[0] = 0x01;
			sendfrm(0x81,buf1,1);
			return 1;
		}
	}
	
	return 0;
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

int COil::GetOilAndPowAd(WORD *v_wOilAD, WORD *v_wPowAD, BYTE *v_bytOilSta, BYTE *v_bytRptType)
{
	pthread_mutex_lock(&m_mutex_OilData);
	
	if( v_wOilAD )	   *v_wOilAD = m_usValidOilAD;
	if( v_wPowAD )		*v_wPowAD = m_usValidPowAD;
	if( v_bytOilSta )	*v_bytOilSta = m_bytOilSta;
	*v_bytRptType = m_bytRptType;
	
    pthread_mutex_unlock(&m_mutex_OilData);

    return TRUE;
}

void COil::SetOilAndPowAD(WORD v_usOilAD, WORD v_usPowAD, BYTE v_bytOilSta, BYTE v_bytRptType)
{
	pthread_mutex_lock(&m_mutex_OilData);
	
	if( v_usOilAD > 0 ) // 当传进来的油耗AD值非0时,才保存油耗AD值
		m_usValidOilAD = v_usOilAD;
	
	if( v_usPowAD > 0 ) // 当传进来的电源AD值非0时,才保存电源AD值
		m_usValidPowAD = v_usPowAD;
	
    m_bytOilSta = v_bytOilSta;
	m_bytRptType =  v_bytRptType;
	
	pthread_mutex_unlock(&m_mutex_OilData);
}

bool COil::bConnect(bool &v_bOilConnect)
{
	return m_bOilConnect;
}

byte COil::oil_setting(char *param, int len)
{
	byte buf[100] = {0};
	
	for(int i=0;i<3;i++)
	{
		byte ret = sendfrm(0xC0,param,len);

		ulong t1 =GetTickCount();

		while((GetTickCount()-t1)<1000)
		{
			usleep(100000);

			ulong l = m_QueOilRecv.pop(buf);

			if( l>0 && buf[3] == 0x40) //最好加上长度判断
			{
				if(!buf[4])  return 0;
				return 1;
			}
		}
	}

	return 0;
}


int COil::OilDataDesire(char *param, char *outbuf, int len)
{
	byte buf[100] = {0};

	for(int i=0;i<3;i++)
	{
		sendfrm(0xC1,param,len);

		ulong t1 =GetTickCount();

		while((GetTickCount()-t1)<4000)
		{
			usleep(200000);

			ulong l = m_QueOilRecv.pop(buf);

			if(buf[3] == 0x41)
			{
				memcpy(outbuf,buf+4,l-4);
				return 1;
			}
		}
	}
	return 0;
}

int COil::query_ver(char *outbuf)
{
	byte buf[100] = {0};

	for(int i=0;i<3;i++)
	{
		sendfrm(0xc5,NULL,0);

		ulong t1 =GetTickCount();

		while((GetTickCount()-t1)<5000)
		{
			usleep(200000);

			ulong l = m_QueOilRecv.pop(buf);

			if(l>0 && l < 200 && buf[3] == 0x45)
			{
				if( buf[3] != l-4 )
				{
					return 0;
				}

				memcpy( outbuf,buf+3,l-3 );//从版本数据长度开始
				return (l-3);
			}
		}
	}
	return 0;
}

byte COil::query_state()
{
	byte buf[100] = {0};

	for(int i=0;i<3;i++)
	{
		sendfrm(0xc6,NULL,0);

		ulong t1 =GetTickCount();

		while((GetTickCount()-t1)<5000)
		{
			usleep(200000);

			ulong l = m_QueOilRecv.pop(buf);

			if( 4==l && buf[3] == 0x46)
			{
				return buf[l-1];
			}
		}
	}

	return 0x55;
}

bool COil::query_link()
{
	byte buf[100] = {0};

	for(int i=0;i<3;i++)
	{
//		sendfrm(0x82,NULL,0);

		ulong t1 =GetTickCount();

		while((GetTickCount()-t1)<5000)
		{
			usleep(200000);

			ulong l = m_QueOilRecv.pop(buf);

			if( (l>0) && (l < 200) && (buf[3] == 0x42) )//kzy由于油耗盒没有做链路维护，暂时用主动上报代替链路维护
				return true;
		}
	}

	return false;
}

void COil::SetEvt( BYTE v_bytAccEvt, BYTE v_bytSpdEvt, bool v_bInit )
{
	static bool sta_bInitDone = false;

	pthread_mutex_lock(&m_mutex_Oil);

	if( v_bInit && sta_bInitDone )
		goto SETEVT_END;

	if( 1 == v_bytAccEvt )
	{
		m_bytEvt &= 0xfc;
		m_bytEvt |= ACC_OFFTOON;
	}
	else if( 2 == v_bytAccEvt )
	{
		m_bytEvt &= 0xfc;
		m_bytEvt |= ACC_ONTOOFF;
	}

	if( 1 == v_bytSpdEvt )
	{
		m_bytEvt &= 0xf3;
		m_bytEvt |= SPD_SILENTTOMOVE;
	}
	else if( 2 == v_bytSpdEvt )
	{
		m_bytEvt &= 0xf3;
		m_bytEvt |= SPD_MOVETOSILENT;
	}

	sta_bInitDone = true;

	m_dwCmdSetTm = GetTickCount();

SETEVT_END:
	pthread_mutex_unlock(&m_mutex_Oil);
}

BOOL COil::GetOilPowAndSta( BYTE* v_p_bytOilAD, BYTE *v_p_bytPowAD, BYTE* v_p_bytOilSta )
{
	pthread_mutex_lock(&m_mutex_COil);

	if( v_p_bytOilAD ) *v_p_bytOilAD = m_bytOilAD;
	if( v_p_bytPowAD ) *v_p_bytPowAD = m_bytPowAD;
	if( v_p_bytOilSta ) *v_p_bytOilSta = m_bytOilSta;

	pthread_mutex_unlock(&m_mutex_COil);

	return TRUE; // 固定返回TRUE
}

void COil::SetOilPowAndSta( BYTE v_bytOilAD, BYTE v_bytPowAD, BYTE v_bytOilSta )
{
	// 输入参数v_bytOilAD是已经归一化后的值

	BYTE bytOilAD = 0, bytPowAD = 0, bytOilSta = 0;

	pthread_mutex_lock(&m_mutex_COil);

	if( v_bytOilAD > 0 ) // 当传进来的油耗AD值非0时,才保存油耗AD值
		bytOilAD = m_bytOilAD = v_bytOilAD;
	else
		bytOilAD = m_bytOilAD;

	if( v_bytPowAD > 0 ) // 当传进来的电源AD值非0时,才保存电源AD值
		bytPowAD = m_bytPowAD = v_bytPowAD;
	else
		bytPowAD = m_bytPowAD;

	m_bytOilSta = v_bytOilSta;

	pthread_mutex_unlock(&m_mutex_COil);

	// 最后得到的油耗有效AD,通知ComuExe
	char buf[2] = {0};
	buf[0] = 0x03;	// 0x03表示油耗AD通知
	buf[1] = (char)m_bytOilAD;
	DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
	
	g_objCarSta.SavOil( bytOilAD, bytPowAD, bytOilSta ); // 保存在状态模块中，以便存储状态
}

#endif


