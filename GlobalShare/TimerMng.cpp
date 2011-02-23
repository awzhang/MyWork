#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "TimerMng.h"

TIMER_HANDLE g_TimerHdl[MAX_TIMER_CNT] = {-1};

void G_ThreadTimerMng(void *v_pPar)
{
	if( !v_pPar ) 
		return;

	((CTimerMng*)v_pPar)->P_ThreadTimerMng();
}

// 本函数内实现互斥，线程安全
unsigned int _SetTimer( CTimerMng *v_pTimerMng, unsigned int v_uiTmID, unsigned int v_uiInterval, 
			   void	(* v_TimerFunc)(void *arg, int arg_len), void *v_arg, int v_iArgLen )
{
	int iInterval;

	iInterval = v_uiInterval/1000;
	
	if( g_TimerHdl[v_uiTmID] != -1 )
	{
		v_pTimerMng->KillTimer( g_TimerHdl[v_uiTmID] );
		g_TimerHdl[v_uiTmID] = -1;
	}

	g_TimerHdl[v_uiTmID] = v_pTimerMng->SetTimer(iInterval, v_TimerFunc, v_arg, v_iArgLen);

	return g_TimerHdl[v_uiTmID];
}

void _KillTimer( CTimerMng *v_pTimerMng, unsigned int v_uiTmID )
{
	if( g_TimerHdl[v_uiTmID] != -1 )
	{
		v_pTimerMng->KillTimer( g_TimerHdl[v_uiTmID] );
		g_TimerHdl[v_uiTmID] = -1;
	}
}

//////////////////////////////////////////////////////////////////////////
CTimerMng::CTimerMng()
{
	m_pthread_timermng = 0;
	m_bQuit = false;
	
	memset((void*)g_TimerHdl, -1, sizeof(g_TimerHdl));

	memset((void*)&m_objTimer, 0, sizeof(m_objTimer));
}

CTimerMng::~CTimerMng()
{

}

int CTimerMng::Init()
{
	pthread_mutex_init(&m_MutexTimer, NULL);

	// 创建定时器线程
	if( pthread_create(&m_pthread_timermng, NULL, (void *(*)(void*))G_ThreadTimerMng, (void*)this ) != 0 )
		return 1;

	return 0;
}

int CTimerMng::Release()
{
	pthread_mutex_destroy( &m_MutexTimer );
	m_bQuit = true;
}

void CTimerMng::P_ThreadTimerMng()
{
	int i = 0;

	while( !m_bQuit )
	{
		usleep(TIMER_MIN_INTERVAL*1000);

		for(i = 0; i < MAX_TIMER_CNT; i++)
		{
			if( false == m_objTimer.timers[i].m_bEnable )
			{
				continue;
			}

			m_objTimer.timers[i].m_iElapse++;
			if(m_objTimer.timers[i].m_iElapse == m_objTimer.timers[i].m_iInterval)
			{
				m_objTimer.timers[i].m_iElapse = 0;

				if( NULL != m_objTimer.timers[i].TimerFunc )
					m_objTimer.timers[i].TimerFunc(m_objTimer.timers[i].szFuncArg, m_objTimer.timers[i].m_iArgLen);
			}
		}
	}
}

TIMER_HANDLE CTimerMng::SetTimer(int v_iInterval, void (* v_TimerFunc)(void *arg, int arg_len), void *v_arg, int v_iArgLen)
{
	int i;

	pthread_mutex_lock(&m_MutexTimer);
    
    if(v_TimerFunc == NULL || v_iInterval <= 0)
    {
		pthread_mutex_unlock(&m_MutexTimer);
        return (-1);
    } 
    
    for(i = 0; i < MAX_TIMER_CNT; i++)
    {
        if(m_objTimer.timers[i].m_bEnable == true)
        {
            continue;
        }
        
        memset(&m_objTimer.timers[i], 0, sizeof(m_objTimer.timers[i]));

        m_objTimer.timers[i].TimerFunc = v_TimerFunc;

        if(v_arg != NULL)
        {
            if(v_iArgLen > MAX_FUNC_ARG_LEN)
            {
				pthread_mutex_unlock(&m_MutexTimer);
                return (-1);
            }

            memcpy(m_objTimer.timers[i].szFuncArg, v_arg, v_iArgLen);
            m_objTimer.timers[i].m_iArgLen = v_iArgLen;
        }

        m_objTimer.timers[i].m_iInterval = v_iInterval;
        m_objTimer.timers[i].m_iElapse = 0;
        m_objTimer.timers[i].m_bEnable = true;

        break;
    }
    
    if(i >= MAX_TIMER_CNT)
    {
		pthread_mutex_unlock(&m_MutexTimer);
        return (-1);
    }

	pthread_mutex_unlock(&m_MutexTimer);
    return (i);
}

int CTimerMng::KillTimer(TIMER_HANDLE v_iTimer_Handle)
{
	pthread_mutex_lock(&m_MutexTimer);

	if(v_iTimer_Handle < 0 || v_iTimer_Handle >= MAX_TIMER_CNT)
    {
		pthread_mutex_unlock(&m_MutexTimer);
        return (-1);
    }
    
    memset(&m_objTimer.timers[v_iTimer_Handle], 0, sizeof(m_objTimer.timers[v_iTimer_Handle]));
    
	pthread_mutex_unlock(&m_MutexTimer);

    return (0);
}

