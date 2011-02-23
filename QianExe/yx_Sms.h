#ifndef _YX_SMS_H_
#define _YX_SMS_H_

class CSms  
{
public:
	CSms();
	virtual ~CSms();

	int MakeSmsFrame( char* v_szSrcData, int v_iSrcLen, BYTE v_bytTranType, BYTE v_bytDataType, char* v_szDesData, int v_iDesSize, int &v_iDesLen, int v_iCvtType = CVT_NONE, bool v_bTcp = true );
	int DealSmsFrame( char* v_szBuf, int v_iBufLen, char* v_szVeTel, size_t v_sizTel, bool v_bFromSM );

private:
	void _AnalyzeSmsFrame( char* v_szSrcHandtel, char* v_szDesHandtel, BYTE v_bytTranType,
		BYTE v_bytDataType, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	bool _Deal3805(char *v_szData, DWORD v_dwDataLen);
};

#endif

