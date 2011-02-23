// SpecCtrl.h: interface for the CSpecCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPECCTRL_H__A8D394D3_7977_4199_ACE9_C000FEA3ECE5__INCLUDED_)
#define AFX_SPECCTRL_H__A8D394D3_7977_4199_ACE9_C000FEA3ECE5__INCLUDED_

// #if _MSC_VER > 1000
// #pragma once
// #endif // _MSC_VER > 1000

class CSpecCtrl  
{
public:
	CSpecCtrl();
	virtual ~CSpecCtrl();

	void Init();
	void Release();
	int Deal0601( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen );
	void AccChg(bool v_bOn );
	void DisconnOilElec();

private:
	pthread_t m_hThdCtrl;
	volatile bool m_bThdCtrlExit;

// 	HANDLE m_hThdCtrl;
// 	HANDLE m_hEvtThdCtrlExit;

};

#endif // !defined(AFX_SPECCTRL_H__A8D394D3_7977_4199_ACE9_C000FEA3ECE5__INCLUDED_)
