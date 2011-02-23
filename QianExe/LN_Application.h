#ifndef _LN_APPLICATION_H_
#define _LN_APPLICATION_H_

class CLN_Application
{
public:
	CLN_Application();
	virtual ~CLN_Application();

public:
	int Init();
	int Release();

	int MakeAppFrame(char v_szCmd, short v_shAction, char *v_szSrcBuf, DWORD v_dwSrcLen, char *v_szDesBuf, DWORD v_dwDesSize, DWORD &v_dwDesLen);
	int SendAppFrame(char v_szModeType, char v_szControlByte, bool v_bUseLocalSEQ, DWORD v_dwCenterSEQ, char *v_szData, DWORD v_dwDataLen);

	void P_ThreadRecv();

public:
	CInnerDataMng m_objRecvDataMng; 
};

#endif


