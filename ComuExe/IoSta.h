#ifndef _IOSTA_H_
#define _IOSTA_H_


class CIoSta
{
public:
	CIoSta();
	~CIoSta();
	
	int DealIOMsg(char *v_szbuf, DWORD v_dwlen, BYTE v_bytSrcSymb);

	DWORD GetIoSta();
	
private:
	
	int _Init();
	int _Release();
	void _SetIoSta( bool v_bSet, DWORD v_dwVal );
	
public:

	
private:
	pthread_mutex_t m_MutexIoSta;
	
	DWORD m_dwIoSta;		// 仅用于一次性的IO信号,或者事后无法查询的IO信号或设备控制指令
};

#endif	//#define _IOSTA_H_

