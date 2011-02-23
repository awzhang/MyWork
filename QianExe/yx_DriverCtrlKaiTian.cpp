#include "yx_QianStdAfx.h"

#undef MSG_HEAD 
#define MSG_HEAD	("QianExe-DrvCtrlKaiTian")

void *G_ThreadSendPic(void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	g_objKTDriverCtrl.P_ThreadSendPic();
	
	pthread_exit(0);
}

void G_TmReSendPicProc(void *arg, int arg_len)
{
	g_objKTDriverCtrl.P_TmReSendPicProc();
}

void G_TmResPicProc(void *arg, int arg_len)
{
	g_objKTDriverCtrl.P_TmResPicProc();
}

CKTDriverCtrl::CKTDriverCtrl()
{
	m_bSendPicWork = false;
  m_bSendPicExit = false;
  m_pthreadSendPic = 0;
	m_iResPicTime = 0;
}

CKTDriverCtrl::~CKTDriverCtrl()
{
	ClearSendThd();
}

void CKTDriverCtrl::ClearSendThd()
{
	if( m_pthreadSendPic )
	{
		PRTMSG(MSG_NOR, "exit sendpic thd!\n");
		
		m_bSendPicExit = true;
		usleep(100000);
		
		if( WaitForEvent(ThreadExit, &m_pthreadSendPic, 3000) )
		{
			pthread_cancel(m_pthreadSendPic);
			usleep(100000);
		}

		m_pthreadSendPic = 0;
	}
	
	m_bSendPicWork = false;
	m_bSendPicExit = false;
}

void CKTDriverCtrl::Init()
{
	
}

void CKTDriverCtrl::Release()
{
	
}

//传输电子服务证信息
int CKTDriverCtrl::Deal4004( char* v_szData, DWORD v_dwDataLen )
{
	if(v_dwDataLen<sizeof(frm4004)) 
		return ERR_PAR;
	
	char temp[200] = {0};
	temp[0]=0x01;
	temp[1]=0xD4;
	memcpy( temp+2, v_szData, v_dwDataLen );
	DataPush(temp, v_dwDataLen+2, DEV_QIAN, DEV_DIAODU, LV2);
	
	PRTMSG(MSG_NOR, "Snd DrivInfo!\n");

	frm4004 frm;
	memcpy(&frm,v_szData,v_dwDataLen);
	return 0;
}

//开始传输司机照片请求
int CKTDriverCtrl::Deal4005( char* v_szData, DWORD v_dwDataLen )
{
	if(v_dwDataLen<sizeof(frm4005)) 
		return ERR_PAR;

	frm4005 frm;
	memcpy(&frm,v_szData,v_dwDataLen);
	frm4045 res;
	memcpy(res.driverID,frm.driverID,sizeof(res.driverID));

	//m_objPicRcvInfo.rcvSta=0x00;//zzg debug
	if(m_objPicRcvInfo.rcvSta!=0)
	{
		PRTMSG(MSG_NOR, "Pic Sta:%d,Return\n",m_objPicRcvInfo.rcvSta);
		res.resType=m_objPicRcvInfo.rcvSta;
		goto RES4005;
	}
	else
	{
		res.resType=0x00;
		//清空接收状态
		memset(&m_objPicRcvInfo,0,sizeof(m_objPicRcvInfo));	
		memcpy(m_objPicRcvInfo.driverID,frm.driverID,sizeof(m_objPicRcvInfo.driverID));
		m_objPicRcvInfo.totalPack=ntohs(frm.totalPack);
		m_objPicRcvInfo.winSize=frm.winSize;
		m_objPicRcvInfo.winNum=m_objPicRcvInfo.totalPack/ m_objPicRcvInfo.winSize + 
			(m_objPicRcvInfo.totalPack % m_objPicRcvInfo.winSize ? 1 : 0) ; // 总窗口数;
		m_objPicRcvInfo.lastWinSize=(m_objPicRcvInfo.totalPack % m_objPicRcvInfo.winSize ? (m_objPicRcvInfo.totalPack % m_objPicRcvInfo.winSize) : m_objPicRcvInfo.winSize);
		m_objPicRcvInfo.rcvSta=0x02;
		
		//删除旧照片
		FILE *fp = NULL;
		char szpath[100] = {0};
		sprintf( szpath, "%s", DRIVERPICPATH );
		fp=fopen(szpath, "rb+");
		if(fp!=NULL)
		{
			unlink(szpath);
		}
		fp=fopen(szpath, "wb+");
		if(fp!=NULL)
		{
			fclose(fp);
		}
		
		PRTMSG(MSG_NOR, "Begin Rcv Pic totalPack:%d,winNum:%d,winSize:%d,lastWinSize:%d\n",m_objPicRcvInfo.totalPack,
										m_objPicRcvInfo.winNum,m_objPicRcvInfo.winSize,m_objPicRcvInfo.lastWinSize);
	}
RES4005:
	char buf[200]={0};
	int iRet=0;
	int iBufLen = 0;
	iRet = g_objSms.MakeSmsFrame( (char*)&res, sizeof(res), 0x40, 0x45, buf, sizeof(buf), iBufLen );
	if( !iRet ) iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, 0 );

	return iRet;
}

//接收司机照片
int CKTDriverCtrl::Deal4006( char* v_szData, DWORD v_dwDataLen )
{
	//if(v_dwDataLen<sizeof(frm4006)) return ERR_PAR;
	frm4006 * pFrm=(frm4006 *)v_szData;
	WORD packNum=ntohs(pFrm->packNum);
	WORD packSize=ntohs(pFrm->packSize);
	WORD curWin=packNum/m_objPicRcvInfo.winSize;//当前窗口
	
	if(curWin!=m_objPicRcvInfo.curWin)
	{
		PRTMSG(MSG_ERR, "Rcv Win Err!:cuWin:%d,rcvWin:%d\n",m_objPicRcvInfo.curWin,curWin);
		return 0;
	}
	//if(packNum!=m_objPicRcvInfo.curPack)

	//写入文件
	FILE *fp = NULL;
	char szpath[100] = { 0 };
	sprintf( szpath, "%s", DRIVERPICPATH );
	fp=fopen(szpath, "rb+");
	if(fp==NULL) 
	{
		PRTMSG(MSG_ERR, "write file err!\n");
	}
	fseek(fp,packNum*RCVPICPACKSIZE,0);
	fwrite(pFrm->data,packSize,1,fp);
	if(fp!=NULL)
	{
		fclose(fp);
	}

	PRTMSG(MSG_NOR, "Rcv Pic Pack:%d,Win:%d,Size:%d,Flag:%d\n",packNum,curWin,packSize,pFrm->flag);
	byte index1=packNum%m_objPicRcvInfo.winSize/8;
	if(index1==0) 
		index1=1;
	else 
		index1=0;
	byte index2=packNum%8;
	byte value=0x01;
	m_objPicRcvInfo.winSta[index1] |=(value<<index2);
	m_iResPicTime=0;
	if(pFrm->flag==0x01||(m_objPicRcvInfo.winSta[0]==0xff&&m_objPicRcvInfo.winSta[1]==0xff))
	{
		_Send4046();
	}
	else
	{
		_KillTimer(&g_objTimerMng, RESPIC_TIMER);
		_SetTimer(&g_objTimerMng, RESPIC_TIMER,30*1000,G_TmResPicProc);
	}
	
	//if(memcmp(m_objPicRcvSta.driverID,pFrm->driverID,sizeof(pFrm->driverID)))
   
	return 0;
}

void CKTDriverCtrl::_Send4046()
{
	PRTMSG(MSG_NOR, "Snd 4046\n");
	
	frm4046 res;
	memcpy(res.driverID,m_objPicRcvInfo.driverID,sizeof(res.driverID));
	
	res.curWin=m_objPicRcvInfo.curWin;
	//清除窗口状态
	m_objPicRcvInfo.curWin++;
	memset(m_objPicRcvInfo.winSta,sizeof(m_objPicRcvInfo.winSta),0);
	//传输结束
	if((m_objPicRcvInfo.curWin==m_objPicRcvInfo.winNum))
	{
		m_objPicRcvInfo.rcvSta=0x01;
		bool rcvComplete=true;
		byte sta=0x01;
		if(m_objPicRcvInfo.lastWinSize<16)
		{
			byte winSize=m_objPicRcvInfo.lastWinSize;
			for(byte i=0;i<winSize;i++)
			{
				if(i<8)
				{
					if(!(m_objPicRcvInfo.winSta[1]<<i)&0x01) 
					{
						rcvComplete=false;
						break;
					}
				}
				else
				{
					if(!(m_objPicRcvInfo.winSta[0]<<i)&0x01) 
					{
						rcvComplete=false;
						break;
					}
				}
			}
		}
		
		if(rcvComplete)
		{
			//接收完毕，关闭应答
			_KillTimer(&g_objTimerMng, RESPIC_TIMER);
			m_objPicRcvInfo.winSta[0]=0xff;
			m_objPicRcvInfo.winSta[1]=0xff;
			memset(&m_objPicSndInfo,0,sizeof(m_objPicSndInfo));
			memcpy(m_objPicSndInfo.driverID,m_objPicRcvInfo.driverID,sizeof(m_objPicSndInfo.driverID));
			//向触摸屏发送照片数据
			_ReqSendPic();			
		}		
	}
	
	memcpy(res.winSta,m_objPicRcvInfo.winSta,sizeof(res.winSta));
	memset(m_objPicRcvInfo.winSta,0,sizeof(m_objPicRcvInfo.winSta));
	char buf[200]={0};
	int iRet=0;
	int iBufLen = 0;
	iRet = g_objSms.MakeSmsFrame( (char*)&res, sizeof(res), 0x40, 0x46, buf, sizeof(buf), iBufLen );
	if( !iRet ) iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, 0 );
}

//交接办信息应答
int CKTDriverCtrl::Deal4008( char* v_szData, DWORD v_dwDataLen )
{
	return 0;
}

void CKTDriverCtrl::BeginSendPic()
{
	if( !m_pthreadSendPic ) 
	{
		PRTMSG(MSG_NOR, "begin sendpic thd!\n");
	
		if( pthread_create(&m_pthreadSendPic, NULL, G_ThreadSendPic, NULL) != 0 )
		{
			PRTMSG(MSG_ERR, "Create SendPic Thread failed!\n");
			return;
		}
	}
	
	m_bSendPicWork = true;
}

int CKTDriverCtrl::DealComuD5( char* v_szData, DWORD v_dwDataLen )
{
	PRTMSG(MSG_NOR, "Rcv ComuD5:%x\n",v_szData[0]);
	return 0;
}

int CKTDriverCtrl::DealComuD7( char* v_szData, DWORD v_dwDataLen )
{
	PRTMSG(MSG_NOR, "Rcv ComuD7\n");
	
	if(v_szData[0]==0x01)
	{
		BeginSendPic();
	}
	
	return 0;
}

int CKTDriverCtrl::DealComuD9( char* v_szData, DWORD v_dwDataLen )
{
	WORD curPack=0;
	memcpy(&curPack,v_szData,2);
	PRTMSG(MSG_NOR, "Rcv Check PicPack:%d,%d\n",curPack,v_szData[2]);

	if(curPack==m_objPicSndInfo.curPack&&v_szData[2]==0x01)
	{
		//发送下一包
		if(m_objPicSndInfo.sendSta==1)
		{
			m_objPicSndInfo.curPack++;
			BeginSendPic();
		}
		//发送结束
		else if(m_objPicSndInfo.sendSta==2)
		{
			ClearSendThd();
		}
	}
	//否则重发当前包
	else if(m_objPicSndInfo.sendSta==1)
	{
		BeginSendPic();
	}
	
	return 0;
}

//触摸屏请求电子服务证
int CKTDriverCtrl::DealComuDA( char* v_szData, DWORD v_dwDataLen )
{
	PRTMSG(MSG_NOR, "RcvComuDA\n");

	if(v_dwDataLen!=3)	
		return 0;
	_ReqDriverInfo(v_szData,v_dwDataLen);

// 	frmD4 frm;
// 	sprintf(frm.info.copName,"%s","厦门雅迅");
// 	sprintf(frm.info.driverName,"%s","王老六");
// 	frm.info.driverID[2]=0x02;
// 	frm.info.level=0x05;
//	char temp[256] = {0};
//	temp[0]=0x01;
//	memcpy( temp+1, (char*)&frm, sizeof(frm) );
//	DataPush(temp, sizeof(frm)+1, DEV_QIAN, DEV_DIAODU, LV2);

	return 0;
}

//触摸屏请求交接班
int CKTDriverCtrl::DealComuDB( char* v_szData, DWORD v_dwDataLen )
{
	PRTMSG(MSG_NOR, "RcvComuDB:%x\n",v_szData[0]);
#if USE_LEDTYPE == 3	
	//LED灯显示交班信息
	if(v_szData[0]==0x01)
		g_objKTLed.SetLedShow('B',2,3*60*1000);
	else	
		g_objKTLed.SetLedShow('B',2,0);
#endif
	return 0;
}

//请求驾驶员信息
void CKTDriverCtrl::_ReqDriverInfo(char* v_szDriverID,DWORD v_dwDataLen)
{
	char buf[200]={0};
	int iRet=0;
	int iBufLen = 0;
	iRet = g_objSms.MakeSmsFrame( v_szDriverID, v_dwDataLen, 0x40, 0x44, buf, sizeof(buf), iBufLen );
	if( !iRet ) iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, 0 );
}

bool  CKTDriverCtrl::SendPic()
{
	char szpath[100] = { 0 };
	sprintf( szpath, "%s", DRIVERPICPATH );
	FILE *fp = fopen(szpath, "rb+");
	if(fp==NULL) 
		return false;
	fseek(fp,m_objPicSndInfo.curPack*SNDPICPACKSIZE,0);

	frmD8 frm;
	WORD wDataLen=fread(frm.data,1,SNDPICPACKSIZE,fp);
	if(fp!=NULL) 
		fclose(fp);
	if(wDataLen<=0) 
		return false;
	frm.curPack=m_objPicSndInfo.curPack;
	frm.dataLen=wDataLen;
	
	char temp[256] = {0};
	temp[0]=0x01;
	memcpy( temp+1, (char*)&frm, sizeof(frm) );
	DataPush(temp, sizeof(frm)+1, DEV_QIAN, DEV_DIAODU, LV2);
	
	PRTMSG(MSG_NOR, "Snd PicPack:%d,Len:%d\n",frm.curPack,wDataLen);

	//最后一包退出线程
	if(wDataLen<SNDPICPACKSIZE||m_objPicSndInfo.curPack==m_objPicSndInfo.packNum)
	{
		PRTMSG(MSG_NOR, "Send Last Pack:%d\n",m_objPicSndInfo.packNum);//zzg debug

		m_objPicSndInfo.sendSta=0x02;
		m_objPicRcvInfo.rcvSta=0x00;//可以接收新照片
	}
// 	else
// 	{
// 		_KillTimer(&g_objTimerMng, RESNDPIC_TIMER);
// 		printf("ReSnd PicPack:%d",m_objPicSndInfo.packNum);
// 		_SetTimer(&g_objTimerMng, RESNDPIC_TIMER,5000,G_TmReSendPicProc);
//	}

	return true;
}

//向触摸屏请求发送照片数据
bool CKTDriverCtrl::_ReqSendPic()
{
	WORD wFileSize=_GetPicFileSize();
	if(wFileSize<=0) 
		return false;
	
	frmD6 frm;
	memcpy(frm.driverID, m_objPicSndInfo.driverID, sizeof(m_objPicSndInfo.driverID) );
	m_objPicSndInfo.packNum= WORD( wFileSize/SNDPICPACKSIZE + (wFileSize%SNDPICPACKSIZE ? 1 : 0) ); // 总包数;
	m_objPicSndInfo.curPack=0;
	m_objPicSndInfo.sendSta=0x01;
	
	memcpy(&frm.packNum,&m_objPicSndInfo.packNum,sizeof(m_objPicSndInfo.packNum));
	
	char temp[256] = {0};
	temp[0]=0x01;
	memcpy( temp+1, (char*)&frm, sizeof(frm) );
	DataPush(temp, sizeof(frm)+1, DEV_QIAN, DEV_DIAODU, LV2);
	
	PRTMSG(MSG_NOR, "Req Send Pic,packNum:%d\n",m_objPicSndInfo.packNum);

	return true;
}

void CKTDriverCtrl::P_ThreadSendPic()
{
	while( !g_bProgExit )
	{
		if( m_bSendPicExit )
			break;

		if( m_bSendPicWork )
		{
			m_bSendPicWork = false;
			usleep(350000);
		}
		else
		{
			usleep(600000);
			continue;
		}

		if( SendPic()==false )
			break;
	}
	
	m_bSendPicExit = false;
	m_bSendPicWork = false;
}

WORD CKTDriverCtrl::_GetPicFileSize()
{
	char szpath[100] = {0};
	sprintf( szpath, "%s", DRIVERPICPATH );

	FILE *fp = fopen(szpath, "rb+");
	if(fp==NULL) 
		return false;
	// 计算文件大小
	fseek(fp, 0, SEEK_END);
	WORD wLen=(WORD)ftell(fp);
	fclose(fp);
	return wLen;
}

//交接班请求
void CKTDriverCtrl::SndDrvChgInf(byte* v_aryODriverID,byte v_bytLen)
{
	frmDC frm;
	if(v_aryODriverID==NULL||v_bytLen!=sizeof(frm.driverID)) return;
	memcpy(frm.driverID,v_aryODriverID,sizeof(frm.driverID));
	
	char temp[256] = {0};
	temp[0]=0x01;
	memcpy( temp+1, (char*)&frm, sizeof(frm) );
	DataPush(temp, sizeof(frm)+1, DEV_QIAN, DEV_DIAODU, LV2);
}

void CKTDriverCtrl::PicTest()
{
	static int iTick=0;
	switch(iTick)
	{
		case 0:
			_ReqSendPic();
			//iTick=1;
			break;
		case 1:
			m_objPicSndInfo.curPack++;
			SendPic();
			break;
		default:
			break;
	}
}

void CKTDriverCtrl::DrvInfoChgTest()
{
 	static int tick=0;
// 	byte driverInfoID[3];
// 	driverInfoID[0]=0x00;
// 	driverInfoID[1]=0x00;
// 	if(tick==0)
// 	{
// 		driverInfoID[2]=0x01;
// 		tick=1;
// 	}
// 	else
// 	{
// 		tick=0;
// 		driverInfoID[2]=0x02;
// 	}
// 
//	SndDrvChgInf(driverInfoID,3);
// 	if(tick==0)
// 	{
// 		m_objPicSndInfo.driverID[2]=0x01;
// 		tick=1;
// 
// 	}
// 	else
// 	{
// 		m_objPicSndInfo.driverID[2]=0x02;
// 		tick=0;
// 
// 	}
//	_ReqSendPic();
	frmD4 frm;
	if(tick==0)
	{
		sprintf(frm.info.copName,"%s","成都凯天");
		sprintf(frm.info.driverName,"%s","苍井空");
		frm.info.driverID[2]=0x01;
		frm.info.level=0x05;
		tick=1;
	}
	else
	{
		sprintf(frm.info.copName,"%s","成都凯天");
		sprintf(frm.info.driverName,"%s","松岛枫");
		frm.info.driverID[2]=0x02;
		frm.info.level=0x05;
		tick=0;

	}
	
	char temp[256] = {0};
	temp[0]=0x01;
	memcpy( temp+1, (char*)&frm, sizeof(frm) );
	DataPush(temp, sizeof(frm)+1, DEV_QIAN, DEV_DIAODU, LV2);
}

void CKTDriverCtrl::DriverInfoTest()
{
	static int tick=0;
	byte driverInfoID[3];
	driverInfoID[0]=0x00;
	driverInfoID[1]=0x00;
	if(tick==0)
	{
		driverInfoID[2]=0x01;
		tick=1;
	}
	else
	{
		tick=0;
		driverInfoID[2]=0x02;
	}
	_ReqDriverInfo((char*)driverInfoID,sizeof(driverInfoID));
}

void CKTDriverCtrl::P_TmReSendPicProc()
{
	//BeginSendPic();
	_KillTimer(&g_objTimerMng, RESNDPIC_TIMER);
}

void CKTDriverCtrl::P_TmResPicProc()
{
	if(m_iResPicTime<3)
	{
		_Send4046();
		m_iResPicTime++;
	}
	else 
		_KillTimer(&g_objTimerMng, RESPIC_TIMER);
}
