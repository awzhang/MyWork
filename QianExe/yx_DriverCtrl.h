// DriverCtrl.h: interface for the CDriverCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERCTRL_H__7B48B8CF_6582_436C_A927_D7D6F0631A98__INCLUDED_)
#define AFX_DRIVERCTRL_H__7B48B8CF_6582_436C_A927_D7D6F0631A98__INCLUDED_

#if USE_DRIVERCTRL == 1

// #if _MSC_VER > 1000
// #pragma once
// #endif // _MSC_VER > 1000

class CDriverCtrl  
{
public:
	CDriverCtrl();
	virtual ~CDriverCtrl();
	
	void Init();
	void Release();
	void DriverCtrl(bool v_bAccOn);

	int Deal3602( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen );
	int DealComu63( char* v_szData, DWORD v_dwDataLen );
	int DealComu65( char* v_szData, DWORD v_dwDataLen );
	int DealComu6c( char* v_szData, DWORD v_dwDataLen );

	
protected:
	void P_TmDriverLog();
	void P_TmAccOffDriverReLog();
	void P_TmFstDriverLog();

	friend void G_TmDriverLog(void *arg, int len);
	friend void G_TmAccOffDriverReLog(void *arg, int len);
	friend void G_TmFstDriverLog(void *arg, int len);
};

#endif

#endif // !defined(AFX_DRIVERCTRL_H__7B48B8CF_6582_436C_A927_D7D6F0631A98__INCLUDED_)
