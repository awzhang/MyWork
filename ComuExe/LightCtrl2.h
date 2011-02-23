#ifndef _YX_LIGHTCTRL2_H_
#define _YX_LIGHTCTRL2_H_

enum LIGHTSTA2
{
	Red_KuaiShan,			// ºìµÆ¿ìÉÁ£¨ÁÁ2Ãë£¬Ãğ2Ãë£©
	Red_ManShan,			// ºìµÆÂıÉÁ£¨ÁÁ0.5Ãë£¬Ãğ4.5Ãë£©	
	Green_KuaiShan,			// ÂÌµÆ¿ìÉÁ£¨ÁÁ2Ãë£¬Ãğ2Ãë£©
	Green_ManShan,			// ÂÌµÆÂıÉÁ£¨ÁÁ0.5Ãë£¬Ãğ4.5Ãë£©
	Red_JiShan,				// ºìµÆ¼±ÉÁ£¨ÁÁ0.5Ãë£¬Ãğ0.5Ãë£©
	Green_JiShan			// ÂÌµÆ¼±ÉÁ£¨ÁÁ0.5Ãë£¬Ãğ0.5Ãë£©
};

class CLightCtrl2
{
public:
	CLightCtrl2();
	virtual ~CLightCtrl2();

	int Init();
	int Release();
	void SetLightSta(char v_szSta);
	void P_ThreadLightCtrl();

public:
	CInnerDataMng	m_objRecvCtrlMng;
	CInnerDataMng	m_objMemInfoMng;
	
	char m_szReBootTime[15];
	char m_szSwiLinkTime[15];
	char m_szDiskLinkTime[15];
	char m_szMonLinkTime[15];
	
	char m_szTelOnTime[15];
	int m_iTelOnCnt;
	int m_iTelOnSec[100];
	char m_szTelInitTime[15];
	int m_iTelInitCnt;
	int m_iTelInitSec[100];
	char m_szLowSigTime[15];
	int m_iLowSigCnt;
	int m_iLowSigSec[100];
	char m_szDialNetTime[15];
	int m_iDialNetCnt;
	int m_iDialNetSec[100];
	char m_szTcpConTime[15];
	int m_iTcpConCnt;
	int m_iTcpConSec[100];
	char m_szLogInTime[15];
	int m_iLogInCnt;
	int m_iLogInSec[100];
	
	char m_szDeal3801Time[15];
	int m_iDeal3801Cnt;
	int m_iDeal3801Sec[100];
	char m_szVideoMonTime[15];
	int m_iVideoMonCnt;
	int m_iVideoMonSec[100];
	
	DWORD m_dwDevErrCode;
	DWORD m_dwCfgErrCode;
	DWORD m_dwTelErrCode;
	DWORD m_dwMonErrCode;
	
	BYTE m_bytDvrState;
	BYTE m_bytTcpState;

private:
	byte		m_bytLightSta;
	bool		m_bVideoWork;	// ÊÇ·ñ´¦ÓÚÊÓÆµÂ¼Ïñ×´Ì¬
	bool		m_bNetWork;		// ÊÇ·ñ´¦ÓÚÍøÂçÉÏÏß×´Ì¬
	bool		m_bFoot;		// ÊÇ·ñ´¦ÓÚÇÀ½Ù±¨¾¯×´Ì¬
	bool		m_bDial;		// ÊÇ·ñ´¦ÓÚÍøÂç²¦ºÅ¹ı³ÌÖĞ
	pthread_t	m_pthreadLightCtrl;
};

#endif

