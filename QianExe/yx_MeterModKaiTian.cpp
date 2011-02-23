#include "yx_QianStdAfx.h"

#if USE_METERTYPE == 2

#undef MSG_HEAD
#define MSG_HEAD  ("QianExe-MeterModKaiTian  ")

void *G_ThreadKTMeterModRecv(void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	g_objKTMeter.P_ThreadRecv();
	
	pthread_exit(0);
}

void *G_ThreadKTMeterModWork(void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	g_objKTMeter.P_ThreadWork();
	
	pthread_exit(0);
}

void G_TmDownProc(void *arg, int arg_len)
{
	g_objKTMeter.P_TmDownProc();
}

void G_TmSetDeviceProc(void *arg, int arg_len)
{
	g_objKTMeter.P_TmSetDeviceProc();
}

CKTMeterMod::CKTMeterMod()
{
	m_iComPort = -1;
	m_pthreadRecv = 0;
	m_pthreadWork = 0;
	
	m_DownCt = 0;
  m_SendCt  = 0;
	m_SendSta=TAXRECV;
	m_DataType=0;
	m_DataLen=0;
	must_up=false;
	last_pack=false;
	
	m_bEscRecv = false;
	m_bEscWork = false;
	
	pthread_mutex_init(&m_mutexTaxiDataSta, NULL);
	pthread_mutex_init(&m_mutexMeterDataSta, NULL);
}
	
CKTMeterMod:: ~CKTMeterMod()
{
	
}

int CKTMeterMod::Init()
{
#if USE_COMBUS == 0
	// 打开串口
	if( !_ComOpen() )
	{
		PRTMSG(MSG_ERR, "Open Com failed!\n");
		return ERR_COM;
	}
#endif

	m_wMeterSta=0;
	link=false;
	_CheckTaxiFile();//检查是否有未发送完的计价器文件
	if( false==_InitTaxiData() ) 
		return -1;//初始化和检查刷卡数据
	if( false==_InitMeterData() ) 
		return -1;//初始化和检查运营数据

	memset(m_DriverInf,0,sizeof(m_DriverInf));
	m_objUp2068.Init();
		
	tagTaxiCfg objTaxiCfg;
	memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
	GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg), offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	memcpy(m_DriverInf,objTaxiCfg.m_aryDriverInf,sizeof(m_DriverInf));
	if(objTaxiCfg.m_bytJiJiaEnable==0x01)//初始化计价器状态
		m_bJijiaEnable=false;
	else 
		m_bJijiaEnable=true;
	//taxfile.renewflag=true;//zzg debug
		
#if USE_COMBUS == 0		// 使用串口扩展盒时，不创建读线程
	if( pthread_create(&m_pthreadRecv, NULL, G_ThreadKTMeterModRecv, NULL) != 0 )
	{
		Release();
		PRTMSG(MSG_ERR, "Create Recv Thread failed!\n");
		return ERR_THREAD;
	}
#endif

	if( pthread_create(&m_pthreadWork, NULL, G_ThreadKTMeterModWork, NULL) != 0 )
	{
		Release();
		PRTMSG(MSG_ERR, "Create Work Thread failed\n");
		return ERR_THREAD;
	}
	
	return 0;
}

int CKTMeterMod::Release()
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

	_ComClose();
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
	
	pthread_mutex_destroy(&m_mutexTaxiDataSta);
	pthread_mutex_destroy(&m_mutexMeterDataSta);
	
	return 0;
}

void CKTMeterMod::_CheckTaxiFile()
{
	bool resul=false;
	char szpath[100] = {0};
	sprintf( szpath, "%s", TAXIFILEPATH );
	FILE *fp = fopen(szpath, "rb+");
	if(fp!=NULL)
	{
		TaxiFileHead head;
		fread(&head,sizeof(head),1,fp);		
// 		{   //zzg debug
// 			head.curNum=1;
// 			//head.endLen=16;
// 			fseek(fp,0,0);
// 			fwrite(&head,sizeof(head),1,fp); 
//		}
		taxfile.id=head.fileID;
		taxfile.no=head.totalCnt;
		taxfile.endlen=head.endLen;
		m_CurNo=head.curNum;
		if(head.curNum<head.totalCnt)
		{
			taxfile.flag=false;
			_SetTimer(&g_objTimerMng, DOWN_TIMER,10000,G_TmDownProc);
		}
		else 
		{
			taxfile.flag=true;
		}
	}
	if(NULL!=fp) fclose(fp);
}

bool CKTMeterMod::_InitTaxiData()
{
	bool resul=false;
	char szpath[100] = {0};
	sprintf( szpath, "%s", TAXIDATAPATH );
	FILE *fp = fopen(szpath, "rb+");
	if(fp!=NULL)
	{
		//检查是否有未发送完的刷卡数据
		DataHead head;
		fread(&head,sizeof(head),1,fp);
// 	    head.curIndex=94;
//		head.sendCnt=1;
//   	fwrite(&head,sizeof(head),1,fp);
		
		if(head.curIndex>=TOTALDATACNT||head.sendCnt>TOTALDATACNT)
		{//zzg mod 091201
			head.curIndex=0;
			head.sendCnt=0;
			fseek(fp,0,0);
			fwrite(&head,sizeof(head),1,fp);
		}
		else if(head.sendCnt>0)
		{
			printf("Hav Taxi Data:%d",head.sendCnt);
			must_up=true;
		}
		fclose(fp);
		return true;
	}
  fp=fopen(szpath, "wb+");
	if(NULL==fp)	return false;
	DataHead head;
//  head.curIndex=90;
//	head.sendCnt=1;
	if(1==fwrite(&head,sizeof(head),1,fp))
	{
		resul=true;
	}
	if(NULL!=fp) fclose(fp);
	return resul;
}

bool CKTMeterMod::_InitMeterData()
{
	bool resul=false;
	char szpath[100] = {0};
	sprintf( szpath, "%s", METERDATAPATH );
	FILE *fp = fopen(szpath, "rb+");
	if(fp!=NULL)
	{
		//检查是否有未发送完的数据
		DataHead head;
		fread(&head,sizeof(head),1,fp);
// 	    head.curIndex=94;
//		head.sendCnt=1;
//   	fwrite(&head,sizeof(head),1,fp);	
		if(head.curIndex>=TOTALDATACNT||head.sendCnt>TOTALDATACNT)
		{//zzg mod 091201
			head.curIndex=0;
			head.sendCnt=0;
			fseek(fp,0,0);
			fwrite(&head,sizeof(head),1,fp);
		}
		else if(head.sendCnt>0)
		{
			printf("Hav Meter Data:%d",head.sendCnt);
			must_up=true;
		}
		fclose(fp);
		return true;
	}
  fp=fopen(szpath, "wb+");
	if(NULL==fp)	return false;
	DataHead head;
//  head.curIndex=90;
//	head.sendCnt=1;
	if(1==fwrite(&head,sizeof(head),1,fp))
	{
		resul=true;
	}
	if(NULL!=fp) fclose(fp);
	return resul;
}

//-----------------------------------------------------------------------------------------------------------------
// 打开调度屏的串口
// 返回: 打开成功或者失败
bool CKTMeterMod::_ComOpen()
{
	int i;
	struct termios options;
	const int COMOPEN_TRYMAXTIMES = 5;

	for( i = 0; i < COMOPEN_TRYMAXTIMES; i ++ )
	{
		// 打开计价器串口
		m_iComPort = open("/dev/ttySIM0", O_RDWR);
		if( -1 == m_iComPort )
		{	
			PRTMSG(MSG_ERR, "Open serial error!");
			perror(" ");
			sleep(1);
			continue;
		} 
		else 
		{
			if(tcgetattr(m_iComPort, &options) != 0)
			{
				perror("GetSerialAttr");
				break;
			}

			options.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
			options.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
			options.c_oflag &= ~OPOST;
			
			cfsetispeed(&options,B19200);	//设置波特率，调度屏设置波特率为9600
			cfsetospeed(&options,B19200);
			
			tcsetattr(m_iComPort,TCSANOW,&options);
				
			usleep(500000);
			return true;
		}
	}
	
	PRTMSG(MSG_ERR,"打开计价器串口失败!\n");
	return false;
}

bool CKTMeterMod::_ComClose()
{
	if(m_iComPort != -1)
	{
		close(m_iComPort);
		m_iComPort = -1;
		
		return true;
	}
	
	return false;
}
//====================================================================================
// 处理从串口收到的每个字符,解析出命令数据并处理
// ch: 待处理的字节 	
int CKTMeterMod::_DealComChar(byte * chs,int dataLen)
{
	static byte frm[ALABUF] = {0};
	static int len = 0;
	int delCnt=1;
	byte type=0xff;
	//s_count=1;
	if (len == 0) 
	{ 
		if (chs[0] == 0xAA) 
		{
			frm[0]=0xAA;
			len++;
		}
	} 
	else 
	{
		if ( len == 1 ) 
		{
			type=chs[0];	
		}
		else type=frm[1];
		
		switch( type )
		{    
			case 0x00:	
			case 0x01:
			case 0x02:
			case 0x05:	
			case 0x06:	
			case 0x07:
			case 0x09:
			case 0x0c:
			case 0x0d:
				frm[len++]=chs[0];
				if ( len > 3 ) 
				{
					_DealCom(frm,len);
					len = 0;
				} 
				break;
			case 0x03:	
				frm[len++]=chs[0];
				if ( len > 5 )
				{
					_DealCom(frm,len);
					len = 0;
				}
				break;
			case 0x04://一卡通刷卡数据
				//if ( s_msgLen%50 == 0 )ClearWatchdog(); 
				//frm[len++]=ch; 
				if ( len+dataLen > m_DataLen+2 ) 
				{
					memcpy(frm+len,chs,dataLen);
					len+=dataLen;
					_DealCom(frm,len);
					len = 0;
					delCnt=dataLen;
				} 
				break;	
			case 0x0b://单笔运营数据
				if ( len+dataLen>19+2)
				{
					memcpy(frm+len,chs,dataLen);
					len+=dataLen;
					_DealCom(frm,len);
					len = 0;
					delCnt=dataLen;
				}
				break;
			case 0x08:  
				frm[len++]=chs[0];
				if ( len > 4 ) 
				{
					_DealCom(frm,len);
					len = 0;
				} 
				break;
			case 0x0a:
				frm[len++]=chs[0];
				if ( len > 6 ) 
				{
					_DealCom(frm,len);
					len = 0;
				} 
				break;
			case 0xAA://防止接连两个0xaa的出现使状态机异常
				len=0;
				frm[len++]=0xaa;
				break;
		  default  : //防止错误数据
			  len=0;
			  break;
	   }
	}
  return delCnt;
}
//====================================================================================
// 处理计价器串口数据
void CKTMeterMod::_DealCom(byte* frm, int len)
{
	// 对帧进行校验
	byte crc = 0;
	for(int i=0; i<len-1; i++)
		crc += frm[i];
	if(crc!=frm[len-1])		
	{ 
		PRTMSG(MSG_DBG, " Crc Err:", (char*)&crc, 1);
		byte crcErr=0;
		_ComSend(0xff,&crcErr,1);
		return; 
	}

	switch(frm[1])
	{
		case 0x00:  
			hdl_recv0x00();
     	break;
    case 0x01:  
    	hdl_recv0x01();
      break;
    case 0x02:  
    	hdl_recv0x02();
      break;
    case 0x03:	
    	hdl_recv0x03(frm+2);
      break;
    case 0x04:	
    	hdl_recv0x04(frm+2, len-3);
      break;
    case 0x05:  
    	hdl_recv0x05();
      break;
    case 0x06:	
    	hdl_recv0x06(frm+2);
      break;
    case 0x07:	
    	hdl_recv0x07();
      break;
    case 0x08:	
    	hdl_recv0x08(frm+2);
      break;
    case 0x09:  
    	hdl_recv0x09(frm+2);
      break;
		case 0x0a:	
			hdl_recv0x0a(frm+2, len-3);
			break;
		case 0x0b:	
			hdl_recv0x0b(frm+2, len-3);
			break;
		case 0x0c:	
			hdl_recv0x0c(frm+2);
			break;
		case 0x0d:	
			hdl_recv0x0d(frm+2);
			break;
   	default:	
   		break;
	}
}
/****************************************************************
*   函数名:    hdl_recv0x00
*   功能描述:  同步车台
*　 参数:      无
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv0x00(void)
{
	byte send[3];
    
  send[0] = SERIALNUMBER >> 8;           // 车台编号
  send[1] = SERIALNUMBER & 0x00FF;
  send[2] = 0x00;                        // 可以通信
  _ComSend(0x00,send,sizeof(send));
	if(link==false)
	{
		link=true;
		g_objKTDeviceCtrl.SetDeviceSta(KT_METER,true);
		PRTMSG(MSG_NOR, "Meter Connect!\n");
	}
	_KillTimer(&g_objTimerMng, SETDEVICE_TIMER);
	_SetTimer(&g_objTimerMng, SETDEVICE_TIMER,30*1000,G_TmSetDeviceProc);
}

/****************************************************************
*   函数名:    hdl_recv0x01
*   功能描述:  查询事件
*　 参数:      无
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv0x01(void)
{ 
	byte taxflag;
  taxflag = 0x00;
  if (taxfile.flag) 
		taxflag |= 0x02;       // 下栽文件
  if (taxfile.renewflag) 
		taxflag |=0x01;				 // 补采数据
	if(!m_bJijiaEnable)			// 关闭计价器
		taxflag |=0x04;
  _ComSend(0x01,&taxflag,1);  
}

/****************************************************************
*   函数名:    hdl_recv0x02
*   功能描述:  结束通讯
*　 参数:      无
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv0x02(void)
{
	byte resData=0x00;
	_ComSend(0x02,&resData,1);
}

/****************************************************************
*   函数名:    hdl_recv0x03
*   功能描述:  传输刷卡数据命令
*　 参数:      命令数据
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv0x03(byte *data)
{
	WORD_UNION len;
  m_DataType = *data++;
	byte resType=0x00;
  len.bytes.high = *data++;
  len.bytes.low = *data;
  if(taxfile.renewflag && m_DataType==0x01 && len.word<=0x348) 
	{    
		// 删除补采标志位
   	taxfile.renewflag = false;
    //StorePubPara(TXFILEDOWN_); 
  }  
  
  if (len.word >0x348) 
	{   
		// 数据大于840字节只接受840
		m_DataLen=0x348;
		resType=0x01; 
  }
	else
	{	
		m_DataLen=len.word;
		last_pack=true;
	}

	_ComSend(0x03,&resType,1);
}

/****************************************************************
*   函数名:    hdl_recv0x04
*   功能描述:  刷卡数据存储
*　 参数:      INT8U *data: 运营数据
*              INT8U len  : 运营数据长度
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv0x04(byte *data, WORD len)
{
	PRTMSG(MSG_NOR, "Deal 0x04 Succ:", (char*)data, 3);

	if (m_SendSta==TAXRECV) 
	{
    if(_FlushTaxiDataRcd(data,len))
		{
			byte resType=0;	
			_ComSend(0x04, &resType, 1);
			m_SendSta=TAXOK;
			if(last_pack)
			{
				//zzg test
				//must_up=true;//--zzg debug
				last_pack=false;
			}
		}
  }
}
/****************************************************************
*   函数名:    hdl_recv0x05
*   功能描述:  查询传输状态
*　 参数:      无
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv0x05()
{
	byte sndState;
  if (m_SendSta==TAXOK) 
	{
    sndState = 0x03;// 成功
  } 
	else
	{
    if ( !g_objSock.IsOnline() ) 
      sndState = 0x01;           // 无网络
		else 
			sndState = 0x00;        // 正在处理中
  }   
  
  _ComSend(0x05, &sndState, 1);
}

/****************************************************************
*   函数名:    hdl_recv0x06
*   功能描述:  删除下载文件
*　 参数:      INT8U *data: 删除类型
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv0x06(byte *data)//代修改
{   
	byte resType=0;
	_ComSend(0x06,&resType,1);
	if (*data == 0x02 || *data == 0x04 || *data == 0x06)
	{   // 删除下载文件
		PRTMSG(MSG_NOR, "Rcv Meter 0x06: Del TaxiFile!");
		char szpath[100] = {0};
		sprintf( szpath, "%s", TAXIFILEPATH );
		FILE *fp = fopen(szpath, "rb");
		if(fp!=NULL)
		{
			fclose(fp);
			unlink(szpath);
		}
    taxfile.flag = false;
		m_CurNo=0;
	}
}

/****************************************************************
*   函数名:    hdl_recv0x07
*   功能描述:  传送文件命令
*　 参数:      无
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv0x07(void)
{
	byte senddatabuf[2];
  WORD_UNION totallenght; 

  totallenght.word = 0x0040*(taxfile.no-1) + taxfile.endlen;
  senddatabuf[1] = totallenght.bytes.low;   
  senddatabuf[0] = totallenght.bytes.high;
	_ComSend(0x07, senddatabuf, sizeof(senddatabuf));
}

/****************************************************************
*   函数名:    hdl_recv0x08
*   功能描述:  传送文件
*　 参数:      INT8U *data: 第几包
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv0x08(byte *data)
{
	WORD_UNION curno;
  byte sendlen;
	int offset=0;
	byte buffer[64+3]={0};
  curno.bytes.high =data[0];
  curno.bytes.low = data[1];
	if (curno.word > taxfile.no)
	{     
	  taxfile.flag = false;
    return;
  } 
	char szpath[100] = { 0 };
	sprintf( szpath, "%s", TAXIFILEPATH );
	FILE *fp = fopen(szpath, "rb+");
	if(NULL==fp)
	{
		return;
	}
  if (curno.word == taxfile.no) // 最后一包
	{    
    sendlen = taxfile.endlen;
    PRTMSG(MSG_NOR, "Snd Last TaxFilePck:%d,%d",curno.word,sendlen);
  } 
	else 
	{
    sendlen = 64;
  } 
	offset=sizeof(TaxiFileHead)+(curno.word-1)*64;
	fseek(fp,offset,0);
	if(fread(buffer+3,sendlen,1,fp))
	{
		buffer[0]=sendlen;
		buffer[1]=data[0];
		buffer[2]=data[1];
		_ComSend(0x08,buffer,3+sendlen);
		PRTMSG(MSG_NOR, "Snd TaxFilePck:%d,%d",curno.word,sendlen);
	}

	if(NULL!=fp) fclose(fp);
}

/****************************************************************
*   函数名:    hdl_recv0x09
*   功能描述:  删除传输状态标志位命令
*　 参数:      无
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv0x09(byte *data)
{
	byte resType=0;
	_ComSend(0x09,&resType,1); 
  if (*data == 0) 
  {                  
    m_SendSta = TAXRECV; // 删除传输成功的标志
  }   
}

//开关机传输司机身份信息
void CKTMeterMod::hdl_recv0x0a(byte *data, WORD Len)
{
	byte resType=0;
	_ComSend(0x0a,&resType,1); 
	byte bytJijiaSta=0;
	byte aryDriverInfo[3]={0};
	bytJijiaSta=data[0];
	memcpy(aryDriverInfo,data+1,sizeof(aryDriverInfo));
	PRTMSG(MSG_NOR, "cur driver:%x%x%x",aryDriverInfo[0],aryDriverInfo[1],aryDriverInfo[2]);
	if(memcmp(aryDriverInfo,m_DriverInf,sizeof(aryDriverInfo)))//交接班
	{
		frm4048 frm;
		PRTMSG(MSG_NOR, "driver change:%x%x%x-->%x%x%x",m_DriverInf[0],m_DriverInf[1],m_DriverInf[2],aryDriverInfo[0],aryDriverInfo[1],aryDriverInfo[2]);
		//LED灯取消显示交班信息
		g_objKTLed.SetLedShow('B',2,0);
		//更新配置区交接班信息
		memcpy(frm.oldDriver,m_DriverInf,sizeof(frm.oldDriver));
		memcpy(frm.curDriverInfo,aryDriverInfo,sizeof(frm.oldDriver));
		memcpy(m_DriverInf,aryDriverInfo,sizeof(aryDriverInfo));

		tagTaxiCfg objTaxiCfg;
		memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
		GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg),
		offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
		memcpy(objTaxiCfg.m_aryDriverInf,m_DriverInf,sizeof(m_DriverInf));
		SetSecCfg(&objTaxiCfg,offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
		//发送交班信息到触摸屏
		g_objKTDriverCtrl.SndDrvChgInf(m_DriverInf,3);
		//发送中心交班信息
		tagGPSData objGps(0);
		g_objMonAlert.GetCurGPS( objGps, true );
		tagQianGps objQianGps;
		g_objMonAlert.GpsToQianGps( objGps, objQianGps );
		frm.data[0]=objQianGps.m_bytYear;
		frm.data[1]=objQianGps.m_bytMon;
		frm.data[2]=objQianGps.m_bytDay;
		frm.data[3]=objQianGps.m_bytHour;
		frm.data[4]=objQianGps.m_bytMin;
		char buf[200]={0};
		int iRet=0;
		int iBufLen = 0;
		iRet = g_objSms.MakeSmsFrame( (char*)&frm, sizeof(frm), 0x40, 0x48, buf, sizeof(buf), iBufLen );
		if( !iRet ) 
		{
			iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, 0 );
		}
	}
}
//传输当笔运营数据
void CKTMeterMod::hdl_recv0x0b(byte *data, WORD Len)
{
	byte resType=0;
	//填充位置信息
	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS( objGps, true );
	tagQianGps objQianGps;
	g_objMonAlert.GpsToQianGps( objGps, objQianGps );
	memcpy(m_objUp2068.gpsInf.endTime,(byte*)&objQianGps+4,sizeof(m_objUp2068.gpsInf.endTime));
	memcpy(m_objUp2068.gpsInf.endLat,(byte*)&objQianGps+8,sizeof(m_objUp2068.gpsInf.endLat));
	memcpy(m_objUp2068.gpsInf.endLon,(byte*)&objQianGps+12,sizeof(m_objUp2068.gpsInf.endLon));
	//填充运营数据
	memcpy(&(m_objUp2068.meterData),data,sizeof(m_objUp2068.meterData));
	//对计价器上传的日期信息进行处理
	byte month=m_objUp2068.meterData.day;
  m_objUp2068.meterData.day=m_objUp2068.meterData.month;
	m_objUp2068.meterData.month=month & 0x3f;
	m_wMeterSta |=METERDATA_DATA;
	if(METERDATA_COMPLETE==m_wMeterSta)
	{
		_FlushMeterDataRcd((byte *)&m_objUp2068,sizeof(m_objUp2068));
		m_wMeterSta=0;
		must_up=true;//--zzg debug
	}
	_ComSend(0x0b,&resType,1); 
}
//查询传输当笔营运数据
void CKTMeterMod::hdl_recv0x0c(byte *data)
{
	byte resType=0;
	_ComSend(0x0c,&resType,1); 
}
//查询载客人数
void CKTMeterMod::hdl_recv0x0d(byte *data)//未收到过该条指令
{
	byte resType=4;
	_ComSend(0x0d,&resType,1); 
}
//====================================================================================
// 往计价器串口发送数据
bool CKTMeterMod::_ComSend(byte cmd, byte* data, int len)
{	
	if(len<0 && len>500)	return false;
	if(NULL==data)	len = 0;
	static int rcv00Cnt=0;
	
	byte buf[512] = {0};
	int i = 0, j = 0;
	byte crc = 0;		
	
	// 封帧
#if USE_COMBUS == 1	
	buf[i++] = 0xf0;	// 0xf0表示由扩展盒的物理串口2发送
#endif
	buf[i++] = 0xbb;	// 帧头	
	buf[i++] = cmd;	
	memcpy(buf+i, data, len);
	i += len;
  for(j=0; j<i; j++)	
 	crc += buf[j];	// 计算校验码
	buf[i++] = crc;		
	
#if USE_COMBUS == 0
		// 将整帧数据发往串口
		ulong writed = 0;
		writed = write(m_iComPort, buf, i);
		return (writed == i) ? true : false;
#endif

#if USE_COMBUS == 1
		DWORD dwPacketNum;
		g_objComBus.m_objComBusWorkMng.PushData((BYTE)LV1, (DWORD)i, (char*)buf, dwPacketNum);
		return true;
#endif
}
//====================================================================================
// 处理运营数据的状态机
void CKTMeterMod::_Work()
{
	static int stawork = 0;
	static DWORD upCheckTick=GetTickCount();
// 	byte data[]={0x00,0x00,0xaa};
//	_ComSend(0xAA,data,sizeof(data));

	switch(stawork)
	{
		case 0:	// 空闲
		{
			if(must_up) 
			{ 
				must_up = false; 
				if(true==_GetTaxiDataRcd())
					stawork = 1;
				if(true==_GetMeterDataRcd())
					stawork = 1;
				upCheckTick=GetTickCount(); 
				break; 
			}
			
			if(GetTickCount()-upCheckTick>30*1000)//30秒检查一次队列
			{
				upCheckTick=GetTickCount();
				//检查刷卡数据队列
				if(que_taxi.getcount()>0&&g_objSock.IsOnline())
				{
					stawork = 1;
					break;
				}
				
				//检查运营数据队列
				if(que_meter.getcount()>0&&g_objSock.IsOnline())
				{
					stawork = 1;
					break;
				}
			}
			
			// 检查TCP接收队列
			static byte tmp[2048] = {0};
			int len = tcprcv.pop(tmp);
			if( len>0 && len<2048 )		
				hdl_cent(tmp[0], tmp+1, len-1);
		}
			break;
			
		case 1:	// 上传
		{
			//发送刷卡数据
			_SendCashData();
			bool bUp=false;
			
			if(0==que_taxi.getcount())
			{
				if(_GetTaxiDataRcd()) bUp=true;
			}
				
			//发送运营数据
			_SendMeterData();
			if(0==que_meter.getcount())
			{
				if(_GetMeterDataRcd()) 
					bUp=true;
			}
			
			if(!bUp)	
				stawork=0;
			upCheckTick=GetTickCount();
		}
			break;
			
		default:
			stawork = 0;
			break;
	}
}
//====================================================================================
// 往中心发送TCP数据
void CKTMeterMod::_TcpSend(byte trantype, byte datatype, byte *data, int len ,int cvtType)
{
	int ln2 = 0;
	char buf2[2048] = {0};
	int iRet = g_objSms.MakeSmsFrame((char*)data, len, trantype, datatype, buf2, sizeof(buf2),ln2,cvtType);
	if( !iRet ) 
	{
		g_objSock.SOCKSNDSMSDATA( buf2, ln2, LV9 );
	}
}	
//====================================================================================
// 处理中心的税控协议
byte CKTMeterMod::hdl_cent(byte type, byte *data, int len)
{
	byte ret = 0;
	
	if(len<=0 && len>2048)	return ret;
	if(NULL==data)			return ret;
	
// 将用户数据进行7->8的转换
//	byte tmp[2048] = {0};
//	CodeXToCode8(tmp, data ,len, 7);
	
	switch(type) 
	{
		case 0x05:
			hdl_recv2005();
			break;
		case 0x06:
			hdl_recv2006();
			break;
		case 0x65:
			hdl_recv2065(data,len);
			break;
		case 0x66:
			hdl_recv2066(data);
			break;
		case 0x67:
			if (data[0] == 0xFF)
			{
				ret=0xFF;
			}
			break;
		case 0x28:
			{
				if(data[0]==0x01)
				{
					ret=0xFF;
				}
			}	
		default:
			break;
	}
	
	return ret;
}

//禁用计价器
void CKTMeterMod::hdl_recv2005()
{
	byte bytRes=0x00;
	tagTaxiCfg objTaxiCfg;
	memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
	GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg), offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	m_bJijiaEnable=false;
	objTaxiCfg.m_bytJiJiaEnable=0x01;
	SetSecCfg(&objTaxiCfg,offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	bytRes=0x01;
	_TcpSend(0x20,0x47,&bytRes,1);
}

//启用计价器
void CKTMeterMod::hdl_recv2006()
{
	byte bytRes=0x00;
	tagTaxiCfg objTaxiCfg;
	memset(&objTaxiCfg,0,sizeof(objTaxiCfg));
	GetSecCfg(&objTaxiCfg,sizeof(objTaxiCfg), offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	m_bJijiaEnable=true;
	objTaxiCfg.m_bytJiJiaEnable=0x00;
	SetSecCfg(&objTaxiCfg,offsetof(tagSecondCfg, m_uni2QTaxiCfg.m_obj2QTaxiCfg), sizeof(objTaxiCfg));
	bytRes=0x01;
	_TcpSend(0x20,0x48,&bytRes,1);
}
/**************************************************************
*   函数名:    HdlRecv2066
*   功能描述:  接收补采请求接口
*　 参数:      INT8U *Data: 补采请求标志
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv2066(byte *Data)
{
	PRTMSG(MSG_NOR, "Rcv 2066\n");
	byte bytRes=0x00;
  if (*Data == 0x00) 
	{ 
    if (taxfile.renewflag != true)
		{
      taxfile.renewflag = true;
    }
		bytRes=0xff;
		_TcpSend(0x20,0x26,&bytRes,1); 
	}
}
/****************************************************************
*   函数名:    HdlRecv2065
*   功能描述:  接收中心下载接口
*　 参数:      INT8U *Data: 中心下载数据
*              INT16U Len : 中心下载数据长度
*   返回值:    无
***************************************************************/
void CKTMeterMod::hdl_recv2065(byte *Data, WORD Len)
{
  _KillTimer(&g_objTimerMng, DOWN_TIMER); 
  PRTMSG(MSG_NOR, "Rcv 2065\n");
    
	frm2025 frm={0};
	frm.resType=0xff;
  if ( Len>4)
	{
		if(Data[1]==0&&Data[2]==0&&Data[3]==0&&Data[4]==0)//收到00结束	
      return;
  } 
	if(taxfile.flag)
	{
		PRTMSG(MSG_NOR, "Have Taxi File\n");
	}
  if (!taxfile.flag&&Len!=0 && Len>6)
	{
		_StorageFileData(Data, Len);
	}
  frm.id=taxfile.id;
	frm.tolNum[0]=(taxfile.no>>8)&0x00ff;
	frm.tolNum[1]=taxfile.no&0x00ff;
	frm.curNum[0]=(m_CurNo>>8)&0x00ff;
	frm.curNum[1]=m_CurNo&0x00ff;
	_TcpSend(0x20,0x25,(byte*)&frm,sizeof(frm));
	_SetTimer(&g_objTimerMng, DOWN_TIMER,DOWNPERIOD,G_TmDownProc);
}

void CKTMeterMod::P_ThreadRecv()
{
	sleep(2);
	ulong readed = 0;
	char buf[ALABUF] = {0};

	while( !g_bProgExit )
	{
		if( m_bEscRecv )
			break;
			
		readed = 0;
		memset(buf, 0, sizeof(buf));
		readed = read( m_iComPort, buf, ALABUF);
		if(readed>0)
		{
// 			if( readed>0 )
// 			{
//				PRTMSG(MSG_DBG, "Rcv From Meter:%s\n", (char*)buf);//zzg debug
//			}

			for(ulong i=0; i<readed;)
			{
				i+=_DealComChar((byte*)buf+i,readed-i);
			}
		}
	}
	
	m_bEscRecv = false;
		
	return;
}
	
void CKTMeterMod::P_ThreadWork()
{
	sleep(10);

	while( !g_bProgExit )
	{
		if( m_bEscWork )
			break;
			
		_Work();
		usleep(50000);
	}
	
	m_bEscWork = false;

	return;
}

void CKTMeterMod::P_PlayTxt(char *fmt)
{
#if USE_TTS == 1
	char str[1024] = {0};
	str[0] = 0x01;		// 0x01表示TTS数据
	str[1] = 0x00;		// 0x01表示立即播放本数据
	strcpy(str+2, fmt);
	DataPush(str, (DWORD)(2+strlen(fmt)), DEV_QIAN, DEV_DVR, LV2);
#endif
}

void CKTMeterMod::P_TmDownProc()
{
	//StartTmr(s_downtmr, DOWNPERIOD);
  if (taxfile.no == 0) 
	{
    _KillTimer(&g_objTimerMng, DOWN_TIMER);
    return;
  }
  if (m_DownCt < MAX_RESEND) 
		m_DownCt++;
  else 
	{
    //StartTmr(s_downtmr, MINUTE, 30);    // 延迟多久重传待修改
    m_DownCt = 0;
		_KillTimer(&g_objTimerMng, DOWN_TIMER);
    return;
  } 
	hdl_recv2065(0, 0);
}

void CKTMeterMod::P_TmSetDeviceProc()
{
	DWORD dwVeSta=g_objCarSta.GetVeSta();
	if(dwVeSta & VESTA_HEAVY)
	{
		PRTMSG(MSG_NOR, "SetMeterSta But Car Is HEAVY\n");
	}
	else //空车
	{
		link=false;
		g_objKTDeviceCtrl.SetDeviceSta(KT_METER,false);
		PRTMSG(MSG_NOR, "Meter Disconnect\n");
		_KillTimer(&g_objTimerMng, SETDEVICE_TIMER);
	}
}
/****************************************************************
*   函数名:    _StorageFileData
*   功能描述:  存储下载数据
*　 参数:      INT8U *Data: 数据指针
*              INT16U Len : 数据长度
*   返回值:    状态
***************************************************************/
bool CKTMeterMod::_StorageFileData(byte *Data, WORD Len)
{   
  //WORD sectorno, offsetno;
  WORD_UNION fileno,filecurno;
  fileno.bytes.high = Data[1];
  fileno.bytes.low = Data[2];
  filecurno.bytes.high = Data[3];
  filecurno.bytes.low = Data[4];
  PRTMSG(MSG_NOR, "fileno:%d,filecur:%d,m_curno:%d\n",fileno.word,filecurno.word,m_CurNo);
  if(fileno.word==0||filecurno.word!=m_CurNo) 
  	return false;
	char szpath[100] = { 0 };
	sprintf( szpath, "%s", TAXIFILEPATH );
	FILE *fp =NULL;
	TaxiFileHead head;
  if ( filecurno.word == 0 ) 
	{  
		// 第一包
		fp=fopen(szpath, "rb+");
		if(fp!=NULL)
		{
			fclose(fp);
			unlink(szpath);
		}
		fp=fopen(szpath, "wb+");
		if(NULL==fp)	
			return false;
	
	  m_CurNo = 0;	    
	  taxfile.id = Data[0];
    taxfile.no = fileno.word;
		head.fileID=taxfile.id;
		head.totalCnt=taxfile.no;
		head.curNum=0;
		PRTMSG(MSG_NOR, "Begin Rcv TaxiFile,ID:%d,Tol:%d\n",taxfile.id,taxfile.no);
		fwrite(&head,sizeof(head),1,fp);
  }
	else
	{
		fp = fopen(szpath, "rb+");
		if(fp==NULL) return false;
		fseek(fp,0,0);
		fread(&head,sizeof(head),1,fp);
	}
	PRTMSG(MSG_NOR, "Rcv TaxiFile:%d\n",filecurno.word);
	if(head.fileID!=taxfile.id||head.totalCnt!=taxfile.no||head.curNum!=filecurno.word)
	{ 	
		if(NULL!=fp) fclose(fp); 
		return false;
	}
  if (filecurno.word == (taxfile.no-1))
	{   
		// 最后一包
		PRTMSG(MSG_NOR, "Rcv TaxiFile Complete\n");
		taxfile.flag = true;
    taxfile.endlen = Data[6];
		head.endLen=Data[6];
		
	  //StorePubPara(TXFILEDOWN_);
  } 
	int offset=sizeof(head)+head.curNum*TAXIFILEPACKSIZE;
  fseek(fp,offset,0);
	fwrite(Data+7,Len-7,1,fp);
	m_CurNo=filecurno.word+1;
	head.curNum++;
	fseek(fp,0,0);
	fwrite(&head,sizeof(head),1,fp);
	if(NULL!=fp) fclose(fp);
	return true;
}
//====================================================================================
bool CKTMeterMod::_GetTaxiDataRcd()
{
	pthread_mutex_lock(&m_mutexTaxiDataSta);
	bool result=false;
	DataHead head;
	int cnt=0;
	char szpath[100] = {0};
	sprintf( szpath, "%s", TAXIDATAPATH );
	FILE *fp =fopen(szpath, "rb+");
	if(NULL==fp)	goto LEAVECS;
	fread(&head,sizeof(head),1,fp);
	if(head.sendCnt>0&&head.curIndex>=0)
	{
		int curIndex=head.curIndex;
		while(cnt<head.sendCnt)
		{
			fseek(fp,sizeof(head)+curIndex*sizeof(Up2027),0);
			Up2027 taxiData;
			if(!fread(&taxiData,sizeof(taxiData),1,fp)) 
			{//若读取失败初始化文件状态--zzg mod 091201
				head.curIndex=0;
				head.sendCnt=0;
				fseek(fp,0,0);
				fwrite(&head,sizeof(head),1,fp);
				break;
			}
			if(false==que_taxi.push((byte*)&taxiData, sizeof(taxiData)-sizeof(taxiData.data)+taxiData.len)) break;
			curIndex++;
			cnt++;
		}
		if(cnt>0)result=true;//zzg mod 091201
	}
	PRTMSG(MSG_NOR, "get taxi--cur:%d; sendCnt:%d\n",head.curIndex,head.sendCnt);//zzg debug
LEAVECS:
	if(NULL!=fp) fclose(fp);
	pthread_mutex_unlock(&m_mutexTaxiDataSta);
	return result;
}

bool CKTMeterMod::_GetMeterDataRcd()
{
	pthread_mutex_lock(&m_mutexMeterDataSta);
	bool result=false;
	DataHead head;
	int cnt=0;
	char szpath[100] = {0};
	sprintf(szpath, "%s", METERDATAPATH);
	FILE *fp =fopen(szpath, "rb+");
	if(NULL==fp)	goto LEAVECS;
	fread(&head,sizeof(head),1,fp);
	if(head.sendCnt>0&&head.curIndex>=0)
	{
		int curIndex=head.curIndex;
		while(cnt<head.sendCnt)
		{
			fseek(fp,sizeof(head)+curIndex*sizeof(Up2068),0);
			Up2068 up2028;
			if(!fread(&up2028,sizeof(up2028),1,fp)) 
			{//若读取失败初始化文件状态--zzg mod 091201
				head.curIndex=0;
				head.sendCnt=0;
				fseek(fp,0,0);
				fwrite(&head,sizeof(head),1,fp);
				break;
			}
			if(false==que_meter.push((byte*)&up2028, sizeof(up2028))) break;
			curIndex++;
			cnt++;
		}
		if(cnt>0)result=true;//zzg mod 091201
	}
	PRTMSG(MSG_NOR, "get meter--cur:%d; sendCnt:%d\n",head.curIndex,head.sendCnt);//zzg debug
LEAVECS:
	if(NULL!=fp) fclose(fp);
	pthread_mutex_unlock(&m_mutexMeterDataSta);
	return result;
}

//修改文件头发送记录，0为刷卡数据，1为运营数据
bool CKTMeterMod::_FlushDataHead(byte type,WORD Cnt)
{
	if(0==type)
		pthread_mutex_lock(&m_mutexTaxiDataSta);
	else
		pthread_mutex_lock(&m_mutexMeterDataSta);

	bool result=false;
	DataHead head;
	char szpath[100] = {0};
	if(0==type) 
		sprintf( szpath, "%s", TAXIDATAPATH );
	else 
		sprintf( szpath, "%s", METERDATAPATH );

	FILE *fp=fopen(szpath, "rb+");
	if(NULL==fp)	
		goto LEAVECS;
	
	fread(&head,sizeof(head),1,fp);
	if(head.sendCnt-Cnt>0)
	{
		head.sendCnt-=Cnt;
		head.curIndex=(head.curIndex+Cnt)%TOTALDATACNT;//zzg mod 091201
	}
	else
	{
		head.sendCnt=0;
		head.curIndex=0;
	}
	fseek(fp,0,0);
	fwrite(&head,sizeof(head),1,fp);
	
	if(0==type)
	{
		PRTMSG(MSG_DBG, "taxi data--curIndex:%d, sendCnt:%d\n",head.curIndex,head.sendCnt);
	}
	else
	{
		PRTMSG(MSG_DBG, "meter data--curIndex:%d, sendCnt:%d\n",head.curIndex,head.sendCnt);
	}
	result=true;
LEAVECS:
	if(NULL!=fp) 
		fclose(fp);
	if(0==type)
		pthread_mutex_unlock(&m_mutexTaxiDataSta);
	else
		pthread_mutex_unlock(&m_mutexMeterDataSta);
	return result;
}

bool CKTMeterMod::_FlushTaxiDataRcd(byte *Data,WORD Len)
{
	pthread_mutex_lock(&m_mutexTaxiDataSta);
	bool result=false;
	int dataSize=UPLOAD_CNT*TAXIDATASIZE;
	int lastData=Len;
	DataHead head;
	int rcdIndex=0;
	int offset=0;
	int rcdCnt=0;
	char szpath[100] = {0};
	sprintf( szpath, "%s", TAXIDATAPATH );
	FILE *fp =fopen(szpath, "rb+");
	if(NULL==fp)	goto LEAVECS;
	fread(&head,sizeof(head),1,fp);
	rcdIndex=(head.curIndex+head.sendCnt)%TOTALDATACNT;
	fseek(fp,sizeof(head)+rcdIndex*sizeof(Up2027),0);
	while(lastData>0||offset>Len)
	{
		if(rcdIndex>=TOTALDATACNT)
		{	
			rcdIndex=0;//从文件头开始存储 
		}
		
		if(rcdIndex==head.curIndex&&head.sendCnt>0) 
		{ 
			break;//超过文件容量上限
		}
		
		Up2027 taxidata;
		taxidata.type=m_DataType;
		if(lastData-dataSize>=0)
		{
			taxidata.len=dataSize;
		}
		else
		{
			taxidata.len=lastData;
		}
		taxidata.cnt=taxidata.len/TAXIDATASIZE;
		memcpy(taxidata.data,Data+offset,taxidata.len);
		offset+=taxidata.len;
		fwrite(&taxidata,sizeof(taxidata),1,fp);
		lastData-=dataSize;
		rcdCnt++;
		rcdIndex++;
	}
	if(lastData<=0) result=true;
	head.sendCnt+=rcdCnt;
	fseek(fp,0,0);
	fwrite(&head,sizeof(head),1,fp);
	PRTMSG(MSG_NOR, "cur:%d; sendCnt:%d\n",head.curIndex,head.sendCnt);//zzg debug
LEAVECS:
	if(NULL!=fp) fclose(fp);
	pthread_mutex_unlock(&m_mutexTaxiDataSta);
	return result;
}

bool CKTMeterMod::_FlushMeterDataRcd(byte *Data,WORD Len)
{
	pthread_mutex_lock(&m_mutexMeterDataSta);
	PRTMSG(MSG_NOR, "FlushMeterDataRcd\n");
	bool result=false;
	DataHead head;
	int rcdIndex=0;
	int offset=0;
	int rcdCnt=0;
	char szpath[100] = {0};
	sprintf( szpath, "%s", METERDATAPATH );
	FILE *fp =fopen(szpath, "rb+");
	if(NULL==fp)	goto LEAVECS;
	fread(&head,sizeof(head),1,fp);
	rcdIndex=(head.curIndex+head.sendCnt)%TOTALDATACNT;
	fseek(fp,sizeof(head)+rcdIndex*sizeof(MeterData),0);

	if(rcdIndex>=TOTALDATACNT){	rcdIndex=0;}//从文件头开始存储 
	if(rcdIndex==head.curIndex&&head.sendCnt>0) //超过文件容量上限
	{ 
		PRTMSG(MSG_NOR, "meterdata file full\n");
		goto LEAVECS;
	}
	fwrite(Data,Len,1,fp);
	rcdCnt++;
	head.sendCnt+=rcdCnt;
	fseek(fp,0,0);
	fwrite(&head,sizeof(head),1,fp);
	result=true;
	PRTMSG(MSG_NOR, "meter--cur:%d; sendCnt:%d\n",head.curIndex,head.sendCnt);//zzg debug
LEAVECS:
	if(NULL!=fp) fclose(fp);
	pthread_mutex_unlock(&m_mutexMeterDataSta);
	return result;
}

// 检测并等待中心应答
bool CKTMeterMod::_ChkAck(byte datatype)
{
	byte tmp[2048] = {0};
	ulong len = 0;
	
	int left = 400;		// 最多等待40秒钟
	while(left>0) 
	{
		len = tcprcv.pop(tmp);
		if(len>0) 
		{
			byte ret = hdl_cent(tmp[0], tmp+1, len-1);
			if(tmp[0]==datatype && ret==0xff) 	break;
		}
		left--;
		usleep(100000);
	}
	
	PRTMSG(MSG_NOR, "_ChkAck %s\n", (left>0) ? "Succ!" : "Fail!");
	return (left>0) ? true : false;
}	

void CKTMeterMod::_SendMeterData()
{
	PRTMSG(MSG_NOR, "Start to upload Meter data\n");
	byte buf[2048] = {0};
	ulong len = 0;
	int mSndCnt=0;
	PRTMSG(MSG_NOR, "que_meter.getcount:%d\n",que_meter.getcount());
	while( que_meter.getcount()>0&&(len = que_meter.pop(buf)) ) 
	{
		_TcpSend(0x20, 0x68, buf, len);
		if(!_ChkAck(0x28)) 
		{
			que_meter.push(buf, len);
			break;
		}
		
		mSndCnt++;
		if(mSndCnt>=10)//每10次后就修改记录，防止未传完车台重启而丢失
		{
			if(true==_FlushDataHead(1,mSndCnt)) mSndCnt=0;
		}
	}
	if(mSndCnt>0)_FlushDataHead(1,mSndCnt);
}

void CKTMeterMod::_SendCashData()
{
	PRTMSG(MSG_NOR, "Start to upload cash data\n");
	byte buf[2048] = {0};
	ulong len = 0;
	int tSndCnt=0;
	PRTMSG(MSG_NOR, "que_taxi.getcount:%d\n",que_taxi.getcount());
	while( que_taxi.getcount()>0&&(len = que_taxi.pop(buf)) ) 
	{
		_TcpSend(0x20, 0x27, buf, len);
		if(!_ChkAck(0x67)) 
		{
			que_taxi.push(buf, len);
			break;
		}
		
		tSndCnt++;
		if(tSndCnt>=10)//每10次后就修改记录，防止未传完车台重启而丢失
		{
			if(true==_FlushDataHead(0,tSndCnt)) tSndCnt=0;
		}
	}
	if(tSndCnt>0)_FlushDataHead(0,tSndCnt);	
}

void CKTMeterMod::Test()
{
	byte data[]={0x00,0x00,0xaa};
	_ComSend(0xAA,data,sizeof(data));
	PRTMSG(MSG_NOR, "com send\n");
}

void CKTMeterMod::BeginMeterRcd()
{
	//清空运营信息
	m_objUp2068.Init();
	m_wMeterSta=0;
	//获取GPS数据
	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS( objGps, true );
	tagQianGps objQianGps;
	g_objMonAlert.GpsToQianGps( objGps, objQianGps );
	//填充位置信息
	memcpy(m_objUp2068.gpsInf.date,(byte*)&objQianGps+1,sizeof(m_objUp2068.gpsInf.date));
	memcpy(m_objUp2068.gpsInf.beginTime,(byte*)&objQianGps+4,sizeof(m_objUp2068.gpsInf.beginTime));
	memcpy(m_objUp2068.gpsInf.beginLat,(byte*)&objQianGps+8,sizeof(m_objUp2068.gpsInf.beginLat));
	memcpy(m_objUp2068.gpsInf.beginLon,(byte*)&objQianGps+12,sizeof(m_objUp2068.gpsInf.beginLon));

	P_PlayTxt("请输入载客人数");

	//发送载客人数请求至触摸屏
}

void CKTMeterMod::SetSerValue(int v_iValue)
{
	PRTMSG(MSG_NOR, "Set SerValue:%d\n",v_iValue);
	if(v_iValue<2||v_iValue>5) v_iValue=3;
	m_objUp2068.serValue=(byte)v_iValue;
	m_wMeterSta|=METERDATA_SERVALUE;
	if(METERDATA_COMPLETE==m_wMeterSta)
	{
		_FlushMeterDataRcd((byte *)&m_objUp2068,sizeof(m_objUp2068));
		m_wMeterSta=0;
		must_up=true;
	}
	//给屏发送应答
	char buf[3];
	buf[0]=0x01;
	buf[1]=0x4b;
	buf[2]=0x01;
	DataPush(buf, 3, DEV_QIAN, DEV_DIAODU, LV2);
}

//载客人数应答
int CKTMeterMod::DealComu48( char* v_szData, DWORD v_dwDataLen )
{
	PRTMSG(MSG_NOR, "Rcv Comu48:%d\n",v_szData[0]);
	if(v_szData[0]<1||v_szData[0]>4) m_objUp2068.cnt=1;
	m_objUp2068.cnt=v_szData[0];
	return 0;
}

//服务评价应答
int CKTMeterMod::DealComu4A( char* v_szData, DWORD v_dwDataLen )
{
	PRTMSG(MSG_NOR, "Rcv Comu4A:%d\n",v_szData[0]);
	if(v_szData[0]<2||v_szData[0]>5) SetSerValue(3);
	SetSerValue(v_szData[0]);

	P_PlayTxt("感谢您的评价!");
	return 0;
}

#endif
