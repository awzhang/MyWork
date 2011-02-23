#ifndef _YX_AUTORPT_H_
#define _YX_AUTORPT_H_

void G_TmAutoRpt(void *arg, int len);

class CAutoRpt  
{
public:
	CAutoRpt();
	virtual ~CAutoRpt();
	
	void Init();
// 	void Release();
	void BeginAutoRpt( DWORD v_dwNewSta );
	int  EndAutoRpt();

	void P_TmAutoRpt();
	
private:
	int _Snd0154( const tagGPSData &v_objGps );
	ENU_RPTJUGRESULT _JugCanRpt( const tagGPSData& v_objGps );
	
private:
	tagMonStatus m_objAutoRptSta;
	
};


#endif





