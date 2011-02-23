#ifndef _YX_LIGHTCTRL_H_
#define _YX_LIGHTCTRL_H_

enum LIGHTSTA
{
	ChangLiang = 1,		// ≥£¡¡
	KuaiShan,			// øÏ…¡£®¡¡0.5√Î£¨√0.5√Î£©
	ManShan				// ¬˝…¡£®¡¡2√Î£¨√2√Î£©	
};

class CLightCtrl
{
public:
	CLightCtrl();
	virtual ~CLightCtrl();

	int Init();
	int Release();
	void SetLightSta(char v_szSta);
	void P_ThreadLightCtrl();

public:
	CInnerDataMng	m_objRecvCtrlMng;

private:
	byte		m_bytLightSta;
	pthread_t	m_pthreadLightCtrl;
};

#endif

