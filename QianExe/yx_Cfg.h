#ifndef _YX_CFG_H_
#define _YX_CFG_H_

class CConfig
{
public:
	CConfig();
	virtual ~CConfig();
	
	void GetVer( char* v_szTel, size_t v_sizTel );
	
	int GetSelfTel(char *v_szBuf, size_t v_sizTel );
	int GetSmsTel( char* v_szTel, size_t v_sizTel );

	int Deal3301( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen );
	int Deal3302( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen );
	int Deal3304( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen );
#if 2 == USE_PROTOCOL
	int Deal103f( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, char* v_szDesHandtel, bool v_bFromSM );
	int Deal103e( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
#endif
	int Deal103c( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	int Deal103b( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	int Deal1011( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	int Deal1008( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	int Deal1003( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	int Deal1002( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	int Deal1001( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, char* v_szDesHandtel, bool v_bFromSM );
	
	int DealComu6d( char* v_szData, DWORD v_dwDataLen );
	
	
private:
	DWORD m_dwBiaodinBeginCycle;	// 标定开始时的脉冲总数
	DWORD m_dwBiaodinEndCycle;		// 标定结束时的脉冲总数
	BYTE m_bytBiaodinSta;			// 0,未进行标定; 1,开始标定; 2,结束标定;
	char	m_szVer[60];
};

#endif


