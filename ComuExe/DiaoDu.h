#ifndef _DIAODU_H_
#define _DIAODU_H_

#include "ComuStdAfx.h"

//void *G_DiaoduReadThread(void *arg);
void *G_DiaoduWorkThread(void *arg);
void *G_ExeUpdateThread(void *arg);
void *G_ImageUpdateThread(void *arg);
void *G_AppUpdateThread(void *arg);
void *G_SysUpdateThread(void *arg);
void *G_DataOutPutThread(void *arg);
void *G_VideoOutPutThread(void *arg);

class CDiaodu  
{
public:
	CDiaodu();
	virtual ~CDiaodu();

	int  Init();
	int  Release();

// 	void P_DiaoduReadThread();
	void P_DiaoduWorkThread();
	bool P_ExeUpdateThread();
	bool P_ImageUpdateThread();
	bool P_AppUpdateThread();
	bool P_SysUpdateThread();
	bool P_DataOutPutThread();
	bool P_VideoOutPutThread();
	void _IoDetectThread();

	int  DetranData(char *v_szBuf, int v_iLen);
//	void AnalyseComFrame(char *v_szBuf, int v_iLen);
	void DealDiaoduFrame(char *v_szBuf, int v_iLen);

// 	bool ComOpen();
// 	void ComClose();
//	int  ComWrite(char *v_szBuf, int v_iLen);

	bool check_crc(const byte *buf, const int len);
	byte get_crc(const byte *buf, const int len);

	void show_diaodu(char *fmt, ...);

public:
//	int			m_iComPort;		//串口句柄

	pthread_t	m_pthreadDiaoduWork;
//	pthread_t   m_pthreadDiaoduRead;

	CInnerDataMng	m_objDiaoduReadMng;
	
	char		m_szComFrameBuf[1024];
	int			m_iComFrameLen;

	int			m_gpsdect;		//GPS检测标志
	int			m_iodect;		//IO实时检测标志,有多线程访问，需添加
};

#endif	//#ifndef _DUAODU_H_
