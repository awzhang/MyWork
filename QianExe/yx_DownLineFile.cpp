#include "yx_QianStdAfx.h"

#if USE_AUTORPTSTATION == 1

#undef MSG_HEAD
#define MSG_HEAD	("QianExe-DownLine")

#define LINE_FILE_PATH  ("/mnt/SDisk/part2/data/200101/LineIno.wav")

void G_TmSend4353(void *arg, int len)
{
	g_objDownLine.P_TmSend4353();
}

//////////////////////////////////////////////////////////////////////////

CDownLineFile::CDownLineFile()
{
	m_dwBufSta = 0;
	m_usWinSize = 0;
	m_dwPackCnt = 0;
	m_dwWinCnt = 0;
	m_dwPackNum = 0;
	m_dwCurWinIdx = 0;
	memset(m_strCurVer,0,sizeof(m_strCurVer));
	memset(m_RevBuf,0,sizeof(m_RevBuf));
	memset(m_AryMd5,0,sizeof(m_AryMd5));
	
	memset(&m_ObjLst4353,0,sizeof(m_ObjLst4353));
	m_iSend4353Cnt = 0;
}

CDownLineFile::~CDownLineFile()
{

}

int CDownLineFile::Deal4312(char *v_szbuf, DWORD v_dwLen)
{
	int iBufLen, iRet;
	char resbuf[1024] ={0};
	tag4312 *obj4312;
	tag4352  obj4352;
	tag2QLineVersionCfg objLineVerCfg;
	GetSecCfg( &objLineVerCfg, sizeof(objLineVerCfg), offsetof(tagSecondCfg, m_uni2QLineVersionCfg.m_obj2QLineVersionCfg), sizeof(objLineVerCfg) );
	
	obj4312 = (tag4312*)(v_szbuf);
	
	m_usWinSize = obj4312->usWinsize;
	m_dwPackNum = obj4312->dwPackCnt;
	memcpy(m_AryMd5,obj4312->m_md5,sizeof(m_AryMd5));	
	memcpy( obj4352.strVersion, obj4312->strVersion , sizeof(obj4312->strVersion));
	
	if (!memcmp(objLineVerCfg.strVer,(char*)obj4312->strVersion,sizeof(objLineVerCfg.strVer))) 
	{
		PRTMSG(MSG_NOR, "Line File has been Downed!\n");
		obj4352.ucFlag = 1;			
	}
	else
	{
		if (!memcmp(m_strCurVer,(char*)v_szbuf,sizeof(m_strCurVer))) 
		{		
			obj4352.ucFlag = 0;
		}
		else
		{
			obj4352.ucFlag = 0;
		}
	}
	
	iRet = g_objSms.MakeSmsFrame((char*)&obj4352, sizeof(obj4352), 0x43, 0x52, resbuf, sizeof(resbuf), iBufLen);
	if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);
	
	if (!obj4352.ucFlag) 
	{
		memcpy( m_strCurVer , obj4312->strVersion , sizeof(obj4312->strVersion));
		m_dwPackCnt = 0;
		m_dwBufSta  = 0;
		
		// 创建线路文件
		FILE *fpbus =NULL;
		char buf[MAX_PATH] = {0};
		fpbus =	fopen(SPRINTF(buf, "%sLineIno.wav", RPTSTATION_SAVE_PATH), "wb+");
		fclose(fpbus);
		fpbus = NULL;
				
		tag4353 obj4353;
		memcpy(obj4353.strVersion,m_strCurVer,sizeof(obj4353.strVersion));
		obj4353.ulWinIndex  = 0;
		obj4353.ulWinStatus = 0;	
		m_dwCurWinIdx       = 0;
		Send4353(obj4353);
	}
}

int CDownLineFile::Deal4313(char *v_szbuf, DWORD v_dwLen)
{
	FILE *fpbus = NULL;
	int iBufLen, iRet;
	char resbuf[1024] ={0};
	tag4353 obj4353;
	tag4313 *obj4313;

	if (m_usWinSize>32) 
		return ERR_FILE;

	_KillTimer(&g_objTimerMng, SEND4353_TIMER);
	
	obj4313 = (tag4313*)(v_szbuf);

	PRTMSG(MSG_DBG, "Deal4313: Idx=%d/%d,Sta=0x%08X,Len=%d\n",obj4313->ulPackIndex,m_dwPackNum,m_dwBufSta,v_dwLen);
	if (m_dwCurWinIdx != obj4313->ulPackIndex/m_usWinSize) 
	{
		PRTMSG(MSG_DBG, "Deal4313 data Win Index err!\n");
		return ERR_PAR;
	}

	memcpy(m_RevBuf+1024*(obj4313->ulPackIndex%m_usWinSize), obj4313->aryLineDat, obj4313->usPackSize);

	if (obj4313->ulPackIndex == m_dwPackNum-1) // 最后一个包
	{
		m_usLastPackSize = obj4313->usPackSize;
	}

	m_dwWinCnt  = obj4313->ulPackIndex/m_usWinSize;
	m_dwBufSta |= (0x01<<(obj4313->ulPackIndex%m_usWinSize));	
	m_dwPackCnt ++;  

	if(obj4313->bNeedAns)
	{
		memcpy(obj4353.strVersion,m_strCurVer,sizeof(obj4353.strVersion));

		if (IsWinRecvOver()) // 该窗口全部接收完毕
		{
			PRTMSG(MSG_DBG, "Recv Win=%d Full\n", m_dwWinCnt);			
			
			char buf[MAX_PATH] = {0};
			fpbus =	fopen(SPRINTF(buf, "%sLineIno.wav", RPTSTATION_SAVE_PATH),"rb+");

			// 将数据写入文件
			if (fpbus) 
			{		
				fseek(fpbus, m_dwWinCnt*32*1024, 0);

				if ( m_dwWinCnt == (m_dwPackNum / m_usWinSize  ) ) 	
				{
					fwrite(m_RevBuf,(m_dwPackNum%m_usWinSize-1) *1024 + m_usLastPackSize ,1,fpbus);
				}
				else
				{
					fwrite(m_RevBuf, m_usWinSize*1024,1,fpbus);
				}

				fclose(fpbus);
				m_dwCurWinIdx++;
			}
			else
			{
				PRTMSG(MSG_NOR, "Deal4313 open Line File err\n");
				return ERR_FILE;
			}

			if(obj4313->ulPackIndex == m_dwPackNum-1) //传输完毕
			{
				//进行MD5校验
				CMd5 cmd5;
				tag4350 obj4350;
				char strMd5_here[32];

				char buf[MAX_PATH] = {0};
				cmd5.MD5File(SPRINTF(buf, "%sLineIno.wav", RPTSTATION_SAVE_PATH),strMd5_here);

				byte m_md5ok = (0==strncmp(strMd5_here,(char*)m_AryMd5,32)) ? true : false;

				memcpy(obj4350.strVersion,m_strCurVer,sizeof(obj4350.strVersion));

				obj4350.bSucc = m_md5ok;

				if (m_md5ok) 
				{				
					tag2QLineVersionCfg objLineVerCfg;
					memcpy(objLineVerCfg.strVer,m_strCurVer,sizeof(objLineVerCfg.strVer));

					if (SetSecCfg(&objLineVerCfg, offsetof(tagSecondCfg, m_uni2QLineVersionCfg.m_obj2QLineVersionCfg), sizeof(objLineVerCfg)) ) 
					{
						obj4350.bSucc = 0;
					}
				}

				iRet = g_objSms.MakeSmsFrame((char*)&obj4350, sizeof(obj4350), 0x43, 0x50, resbuf, sizeof(resbuf), iBufLen);
				if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);
               
				ClrAllFlags();

				g_objAuoRptStationNew.CenterLoadSta(0);
 			}
			else
			{
				obj4353.ulWinIndex  = obj4313->ulPackIndex/m_usWinSize +1;
				obj4353.ulWinStatus = 0;
				Send4353(obj4353);

				//清标志，为接收下一个窗口
				m_dwBufSta = 0;
				memset(m_RevBuf, 0, sizeof(m_RevBuf));
			}
		}
		else
		{
			obj4353.ulWinIndex  = obj4313->ulPackIndex/m_usWinSize;
			obj4353.ulWinStatus = m_dwBufSta;	
			Send4353(obj4353);
		}			
	}
}

int CDownLineFile::Deal4310(char *v_szbuf, DWORD v_dwLen)
{

}

void CDownLineFile::Send4353(tag4353 &obj4353)
{
	int iBufLen, iRet;
	char resbuf[1024] ={0};	

	PRTMSG(MSG_DBG, "Send4353 WinIndex=%d,Sta=0x%08X.",obj4353.ulWinIndex,obj4353.ulWinStatus);

	iRet = g_objSms.MakeSmsFrame((char*)&obj4353, sizeof(obj4353), 0x43, 0x53, resbuf, sizeof(resbuf), iBufLen);
	if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);
	
	memcpy(&m_ObjLst4353,&obj4353,sizeof(m_ObjLst4353));

	_SetTimer(&g_objTimerMng, SEND4353_TIMER, 10000, G_TmSend4353);
	
	m_iSend4353Cnt = 0;	
}

void CDownLineFile::P_TmSend4353()
{
	PRTMSG(MSG_DBG, "TmSnd4353 Cnt=%d,WinIdx=%d,Sta=0x%08X.",m_iSend4353Cnt,m_ObjLst4353.ulWinIndex,m_ObjLst4353.ulWinStatus);

	int iBufLen, iRet;
	char resbuf[1024] ={0};

	iRet = g_objSms.MakeSmsFrame((char*)&m_ObjLst4353, sizeof(m_ObjLst4353), 0x43, 0x53, resbuf, sizeof(resbuf), iBufLen);
	if( !iRet ) g_objSock.SOCKSNDSMSDATA( resbuf, iBufLen, LV12,0);

	if(	++m_iSend4353Cnt >=3 ) 
		_KillTimer(&g_objTimerMng, SEND4353_TIMER);
}

int CDownLineFile::IsWinRecvOver()
{
	DWORD tmp;
	int iLoop;

	if ( m_dwWinCnt == (m_dwPackNum / m_usWinSize  ) ) 
		iLoop = m_dwPackNum%m_usWinSize ;
	else 
		iLoop = m_usWinSize;

	for(int i = 0;i<iLoop; i++)
	{
		tmp |= (0x01<<i);
	}

	return(tmp == m_dwBufSta);
}

void CDownLineFile::ClrAllFlags()
{
	m_dwBufSta = 0;
	m_dwCurWinIdx = 0;
	m_usWinSize = 0;
	m_dwPackCnt = 0;
	m_dwWinCnt  = 0;
	m_dwPackNum = 0;
	memset(m_strCurVer,0,sizeof(m_strCurVer));
	memset(m_RevBuf,   0,sizeof(m_RevBuf));
	memset(m_AryMd5,   0,sizeof(m_AryMd5));
}


#endif

