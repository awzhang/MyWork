#ifndef _LN_TRANSPORT_H_
#define _LN_TRANSPORT_H_

#define SEQ_ARY_SIZE 100
#define MAX_LONGCMD_PACK 10
class CLN_Transprot
{
public:
	CLN_Transprot();
	virtual ~CLN_Transprot();

public:
	int Init();
	int Release();

	int MakeTransportFrame(byte v_bytControl, char v_szUseLoaclSEQ, char *v_szCenterSEQ, char *v_szOptHeadBuf, DWORD v_dwOptHeadLen, char *v_szSrcBuf, DWORD v_dwSrcLen, char *v_szDesBuf, DWORD &v_dwDesLen);

	void P_ThreadSend();
	void P_ThreadRecv();
	void P_ThreadReUpload();
	void P_ThreadRecvLongCmd();
	int Deal1B_0001(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal1B_0002(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal_21(char *v_szBuf, DWORD v_dwLen);
	int Deal_22(char *v_szBuf, DWORD v_dwLen);
	int Deal_23(char *v_szBuf, DWORD v_dwLen);
	int Deal_24(char *v_szBuf, DWORD v_dwLen);

	int Deal_30(char *v_szBuf, DWORD v_dwLen);
	int Deal_31(char *v_szBuf, DWORD v_dwLen);
	int Deal_32(char *v_szBuf, DWORD v_dwLen);
	int Deal_33(char *v_szBuf, DWORD v_dwLen);
	int Deal_34(char *v_szBuf, DWORD v_dwLen);

	int Deal_41(char *szSEQ, char *v_szBuf, DWORD v_dwLen);
	int Deal_42(char *szSEQ, char *v_szBuf, DWORD v_dwLen);
public:
	CInnerDataMng	m_objSendDataMng;
	CInnerDataMng	m_objRecvDataMng;
	CInnerDataMng	m_objRecvLCmdDataMng;
	CLN_DataMng		m_objReUploadMng;

	DWORD m_dwSEQ;
	byte m_bytSta;
	bool m_bRcvLongCmd;
	bool m_bLongCmdPackSta[MAX_LONGCMD_PACK];
	DWORD m_dwPackLen[MAX_LONGCMD_PACK];
	char m_bytLongCmdBuf[MAX_LONGCMD_PACK][1024];
	char m_bytCRC[2];
	ushort m_usRcvLen;
	byte m_bytPackTotal;
	byte m_bytCurPackNum;
// 	pthread_mutex_t m_MutexSEQ;
// 	int		m_aryiWaitCheckSEQ[SEQ_ARY_SIZE];
// 	bool	m_arybValidSEQ[SEQ_ARY_SIZE];
// 	int		m_iSEQCnt;
};

#endif


