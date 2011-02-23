#ifndef _YX_COMU_H_
#define _YX_COMU_H_

void G_TmChkDiaodu(void *arg, int len);

class CComu  
{
public:
	CComu();
	virtual ~CComu();
		
	void DealComuMsg(char *v_szbuf, DWORD v_dwlen, BYTE  v_bytSrcSymb);
	DWORD GetComuSta();
	void P_TmChkDiaodu();

private:
	int _DealComu0b( char* v_szData, DWORD v_dwDataLen );
	int _DealComu35( char* v_szData, DWORD v_dwDataLen );
	int _DealComu37( char* v_szData, DWORD v_dwDataLen );
	int _DealComu41( char* v_szData, DWORD v_dwDataLen );
	int _DealComu61( char* v_szData, DWORD v_dwDataLen );
	int _DealComu70( char* v_szData, DWORD v_dwDataLen );
	int _DealComu7a( char* v_szData, DWORD v_dwDataLen );
	int _DealComu7b( char* v_szData, DWORD v_dwDataLen );
	int _DealComu82( char* v_szData, DWORD v_dwDataLen );
	int _DealComu84( char* v_szData, DWORD v_dwDataLen );
	int _DealComu86( char* v_szData, DWORD v_dwDataLen );
	int _DealComuE0( char* v_szData, DWORD v_dwDataLen );
	int _DealComuE2();
	int _DealComuEE( char* v_szData, DWORD v_dwDataLen );
	int _DealComuF0( char* v_szData, DWORD v_dwDataLen );
	int _DealComuF2( char* v_szData, DWORD v_dwDataLen );
	int _DealComuF3( char* v_szData, DWORD v_dwDataLen );	
		
private:	
	DWORD m_dwComuSta;
};

#endif

