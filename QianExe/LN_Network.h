#ifndef _LN_NETWORK_H_
#define _LN_NETWORK_H_

class CLN_Network
{
public:
	CLN_Network();
	virtual ~CLN_Network();

public:
	int Init();
	int Release();

	int DealSocketData(char *v_szBuf, DWORD v_dwLen);
	int SendSocketData(char *v_szBuf, DWORD v_dwLen);
};

#endif

