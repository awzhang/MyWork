// MsgQueMng.h: interface for the CMsgQueMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGQUEMNG_H__9CEBB10A_E7E6_4ADE_A172_813430BB2E02__INCLUDED_)
#define AFX_MSGQUEMNG_H__9CEBB10A_E7E6_4ADE_A172_813430BB2E02__INCLUDED_

// #if _MSC_VER > 1000
// #pragma once
// #endif // _MSC_VER > 1000

#include "MsgQueue.h"
#include "ComuServDef.h"

class CMsgQueMng  
{
public:
	CMsgQueMng();
	virtual ~CMsgQueMng();

	int DataPush( void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb, DWORD v_dwPopSymb, BYTE v_bytLvl ); // 此方法可同时指定多个PopSymb
	int DataPop( void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPushSymb, DWORD v_dwPopSymb, BYTE *v_p_bytLvl );
	int DataDel( DWORD v_dwPopSymb, void* v_pCmpDataFunc );
	int DataWaitPop( DWORD v_dwPopSymb );
	int DataSkipWait( DWORD v_dwPopSymb );

private:
	CMsgQueue m_aryMsgQue[ sizeof(MSGQUE_INFO) / sizeof(MSGQUE_INFO[0]) ];
	size_t m_sizMsgTotal;
};

#endif // !defined(AFX_MSGQUEMNG_H__9CEBB10A_E7E6_4ADE_A172_813430BB2E02__INCLUDED_)
