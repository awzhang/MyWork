// MsgQueue.h: interface for the CMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGQUEUE_H__2CDEED82_7DC8_4C9C_A7A8_FB1E3DE1BEC2__INCLUDED_)
#define AFX_MSGQUEUE_H__2CDEED82_7DC8_4C9C_A7A8_FB1E3DE1BEC2__INCLUDED_

#include "../GlobalShare/ComuServExport.h"
#include "Queue.h"

class CMsgQueue  
{
public:
	int Init( int v_iKeySem, int v_iKeyMsg, int v_iKeyShm1, unsigned long v_ulQueSize1,
		int v_iKeyShm2, unsigned long v_ulQueSize2, int v_iKeyShm3, unsigned long v_ulQueSize3 );
	int DataPush( void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb, BYTE v_bytLvl );
	int DataPop( void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPopSymb, BYTE *v_p_bytLvl );
	int DataDel( void* v_pCmpDataFunc );
	int DataWaitPop();
	int DataSkipWait();

	CMsgQueue();
	virtual ~CMsgQueue();

private:
	int _SemCreate( int v_iKeySem );
	int _SemP();
	int _SemV();
	int _MsgCreate( int v_iKeyMsg );

private:
	CQueue m_objQue1;
	CQueue m_objQue2;
	CQueue m_objQue3;

	int	m_iSemID;
	int m_iMsgID;
	
	bool m_bInit;
};

#endif // !defined(AFX_MSGQUEUE_H__2CDEED82_7DC8_4C9C_A7A8_FB1E3DE1BEC2__INCLUDED_)
