#ifndef _LN_LOGIN_H_
#define _LN_LOGIN_H_

class CLN_Login
{
public:
	CLN_Login();
	virtual ~CLN_Login();

public:
	int Init();

	void Register();
	void RegisterAnswer(char *v_szBuf, DWORD v_dwLen);

	void Login();
	void LoginAnswer(char *v_szBuf, DWORD v_dwLen);

	void HeartKeep();
	void HeartKeepAnswer(char *v_szBuf, DWORD v_dwLen);

	int  GetVer(char *v_szBuf);
	int  GetVen(char *v_szBuf);
	int  GetGroup(char *v_szBuf);

	bool IsOnline();

private:
	DWORD  m_dwLoginSucc;
	DWORD  m_dwLoginTimers;
	DWORD  m_dwRegisterTimers;
	
	int   a;
	int   b;

	DWORD  m_dwSendHeartKeepTimers;

};

#endif

