// MsgQueMng.cpp: implementation of the CMsgQueMng class.
//
//////////////////////////////////////////////////////////////////////

#include "MsgQueMng.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgQueMng::CMsgQueMng()
{
	size_t siz1 = sizeof(m_aryMsgQue) / sizeof(m_aryMsgQue[0]);
	size_t siz2 = sizeof(MSGQUE_INFO) / sizeof(MSGQUE_INFO[0]);
	m_sizMsgTotal = siz1 < siz2 ? siz1 : siz2;
	
	for( size_t siz = 0; siz < m_sizMsgTotal; siz ++ )
	{
		m_aryMsgQue[siz].Init( MSGQUE_INFO[siz].m_iKeySem, MSGQUE_INFO[siz].m_iKeyMsg, MSGQUE_INFO[siz].m_iKeyShm1, MSGQUE_INFO[siz].m_dwShm1Size,
			MSGQUE_INFO[siz].m_iKeyShm2, MSGQUE_INFO[siz].m_dwShm2Size, MSGQUE_INFO[siz].m_iKeyShm3, MSGQUE_INFO[siz].m_dwShm3Size );
	}
}

CMsgQueMng::~CMsgQueMng()
{
}

int CMsgQueMng::DataPush( void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb, DWORD v_dwPopSymb, BYTE v_bytLvl )
{
	int iRet = 0;
	bool bFind = false;

	size_t siz = 0;
	for( siz = 0; siz < m_sizMsgTotal; siz ++ )
	{
		if( v_dwPopSymb & MSGQUE_INFO[ siz ].m_dwDevSymb )
		{
			bFind = true;
			iRet = m_aryMsgQue[ siz ].DataPush( v_pPushData, v_dwPushLen, v_dwPushSymb, v_bytLvl );
		}

		if( iRet ) break;
	}

	if( !bFind )
	{
		iRet = ERR_PAR;
	}

	return iRet;
}

int CMsgQueMng::DataPop( void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPushSymb, DWORD v_dwPopSymb, BYTE *v_p_bytLvl )
{
	int iRet = 0;

	size_t siz = 0;
	for( siz = 0; siz < m_sizMsgTotal; siz ++ )
	{
		if( v_dwPopSymb == MSGQUE_INFO[ siz ].m_dwDevSymb )
		{
			iRet = m_aryMsgQue[ siz ].DataPop( v_pPopData, v_dwPopSize, v_p_dwPopLen, v_p_dwPushSymb, v_p_bytLvl );
			break;
		}
	}

	if( siz >= m_sizMsgTotal )
	{
		iRet = ERR_PAR;
	}

	return iRet;
}

int CMsgQueMng::DataDel( DWORD v_dwPopSymb, void* v_pCmpDataFunc )
{
	int iRet = 0;

	size_t siz = 0;
	for( siz = 0; siz < m_sizMsgTotal; siz ++ )
	{
		if( v_dwPopSymb == MSGQUE_INFO[ siz ].m_dwDevSymb )
		{
			iRet = m_aryMsgQue[ siz ].DataDel( v_pCmpDataFunc );
			break;
		}
	}

	if( siz >= m_sizMsgTotal )
	{
		iRet = ERR_PAR;
	}

	return iRet;
}

int CMsgQueMng::DataWaitPop( DWORD v_dwPopSymb )
{
	int iRet = 0;

	size_t siz = 0;
	for( siz = 0; siz < m_sizMsgTotal; siz ++ )
	{
		if( v_dwPopSymb == MSGQUE_INFO[ siz ].m_dwDevSymb )
		{
			iRet = m_aryMsgQue[ siz ].DataWaitPop();
			break;
		}
	}

	if( siz >= m_sizMsgTotal )
	{
		iRet = ERR_PAR;
	}

	return iRet;
}

int CMsgQueMng::DataSkipWait( DWORD v_dwPopSymb )
{
	int iRet = 0;
	DWORD dwSymb = 0;

	size_t siz = 0;
	for( siz = 0; siz < m_sizMsgTotal; siz ++ )
	{
		dwSymb = v_dwPopSymb & MSGQUE_INFO[ siz ].m_dwDevSymb;
		if( dwSymb )
		{
			iRet = m_aryMsgQue[ siz ].DataSkipWait();
		//	break;
		}
	}

	if( siz >= m_sizMsgTotal )
	{
		iRet = ERR_PAR;
	}

	return iRet;
}
