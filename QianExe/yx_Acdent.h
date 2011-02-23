#if !defined(AFX_ACDENTDLG_H__DB1EE569_52E0_4436_AF74_488F3657BF9F__INCLUDED_)
#define AFX_ACDENTDLG_H__DB1EE569_52E0_4436_AF74_488F3657BF9F__INCLUDED_

#if USE_ACDENT == 1 || USE_ACDENT == 2

// #if _MSC_VER > 1000
// #pragma once
// #endif // _MSC_VER > 1000
// // AcdentDlg.h : header file

#include "../GlobalShare/InnerDataMng.h"

class CAcdent
{
public:
	int Deal3604(char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen );
	int Init();
	void Release();

	void P_LoopChkAcdent();
	void P_AcdentSave();

protected:
	void P_TmUploadAcdent();
	
// 	friend void G_ChkAcdent( void* v_pVoid );
// 	friend void G_AcdentSave( void* v_pVoid );
	friend void G_TmUploadAcdent(void *arg, int len);

	
private:
	int _Snd3644Frames( tag3644* v_ary3644Frame );

	
private:
	// 线程相关
	volatile bool m_bThdToExit;
	pthread_t m_hThdAcdent;
	pthread_t m_hThdSave;
	sem_t m_semSave;
	
	//HANDLE m_hEvtEndAcdent;

	// 事故疑点处理数据
	CInnerDataMng m_objAcdentDataMng;
	tagAcdentQueryPeriod m_aryAcdQueryPeriod[10];
	BYTE m_bytAcdQueryPeridCount;
	tagAcdentQueryArea m_aryAcdQueryArea[5];
	BYTE m_bytAcdQueryAreaCount;
	tagAcdentQuerySta m_objAcdQuerySta;

	char m_szAcdPath[MAX_PATH];


// Construction
public:
	CAcdent();   // standard constructor
	~CAcdent();
};


#endif

#endif // !defined(AFX_ACDENTDLG_H__DB1EE569_52E0_4436_AF74_488F3657BF9F__INCLUDED_)
