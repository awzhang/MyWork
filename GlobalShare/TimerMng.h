#ifndef _YX_TIMERMNG_H_
#define _YX_TIMERMNG_H_

#define TIMER_MIN_INTERVAL	1000		// ms
#define MAX_TIMER_CNT 60
#define MAX_FUNC_ARG_LEN 100
 
typedef int TIMER_HANDLE;

extern TIMER_HANDLE g_TimerHdl[MAX_TIMER_CNT]; 

struct tagOneTimer
{
	bool	m_bEnable;
	int		m_iInterval;
	int		m_iElapse;
	void	(* TimerFunc)(void *arg, int arg_len);
	char	szFuncArg[MAX_FUNC_ARG_LEN];
	int		m_iArgLen;
};

struct tagTimerPar
{
	tagOneTimer		timers[MAX_TIMER_CNT];
};

class CTimerMng
{
public:
	CTimerMng();
	~CTimerMng();
	
	int Init();
	int Release();
	
	void P_ThreadTimerMng();
	
	TIMER_HANDLE SetTimer(int v_iInterval, void (* v_TimerFunc)(void *arg, int arg_len), void *v_arg, int v_iArgLen);
	int KillTimer(TIMER_HANDLE v_iTimer_Handle);
	
private:
	pthread_t	m_pthread_timermng;
	bool		m_bQuit;
	
	tagTimerPar	m_objTimer;

	pthread_mutex_t m_MutexTimer;			// ¶¨Ê±Æ÷»¥³âÁ¿
};

void G_ThreadTimerMng(void *v_pPar);
unsigned int _SetTimer( CTimerMng *v_pTimerMng, unsigned int v_uiTmID, unsigned int v_uiInterval, 
			   void	(* v_TimerFunc)(void *arg, int arg_len), void *v_arg = NULL, int v_iArgLen = 0);
void _KillTimer( CTimerMng *v_pTimerMng, unsigned int v_uiTmID );


#endif

