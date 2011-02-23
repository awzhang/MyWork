#ifndef _YX_HELP_H_
#define _YX_HELP_H_

void G_TmAutoDialHelpTel(void *arg, int ien);

class CHelp  
{
public:
	CHelp();
	virtual ~CHelp();
	
	int Deal0301( char* v_szData, DWORD v_dwDataLen );
	int Deal0e01( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen );
	int Deal100b( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	int DealComu80( char* v_szData, DWORD v_dwDataLen );

	void P_TmAutoDialHelpTel();
};

#endif



