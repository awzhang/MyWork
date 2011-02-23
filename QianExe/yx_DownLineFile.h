#ifndef _YX_DOWNLINEFILE_H_
#define _YX_DOWNLINEFILE_H_

class CDownLineFile
{
public:
	CDownLineFile();
	virtual ~CDownLineFile();

	int Deal4312(char *v_szbuf, DWORD v_dwLen);
	int Deal4313(char *v_szbuf, DWORD v_dwLen);
	int Deal4310(char *v_szbuf, DWORD v_dwLen);
	void Send4353(tag4353 &obj4353);
	void P_TmSend4353();
	int  IsWinRecvOver();
	void ClrAllFlags();

public:
	int m_stDownSta;
	DWORD m_dwBufSta;
	WORD m_usWinSize;
	DWORD m_dwPackCnt;
	DWORD m_dwWinCnt;
	DWORD m_dwPackNum;
	char m_strCurVer[6];
	BYTE m_RevBuf[32*1024];
	char m_AryMd5[32];
	WORD m_usLastPackSize;
	tag4353 m_ObjLst4353;
	int  m_iSend4353Cnt;
	DWORD m_dwCurWinIdx;
};

#endif

