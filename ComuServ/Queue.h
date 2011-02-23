// Queue.h: interface for the CQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUE_H__FBE57415_4D97_4F0F_A285_5DAD75936AE5__INCLUDED_)
#define AFX_QUEUE_H__FBE57415_4D97_4F0F_A285_5DAD75936AE5__INCLUDED_


// 本模块内部不做互斥,互斥处理应由上层调用模块负责

class CQueue
{
public:
	CQueue();
	virtual ~CQueue();

	int DataPush( void* v_pPushData, DWORD v_dwPushLen, DWORD v_dwPushSymb );
	int DataPop( void* v_pPopData, DWORD v_dwPopSize, DWORD *v_p_dwPopLen, DWORD *v_p_dwPushSymb );
	int DataDel( void* v_pCmpDataFunc );
	int Init( int v_kShm, unsigned long v_ulQueueSize );
	//void Release();

private:

private:
	unsigned long m_ulMemSize;
	int m_iShmID;
	char* m_pShareMem;
};



#endif // !defined(AFX_QUEUE_H__FBE57415_4D97_4F0F_A285_5DAD75936AE5__INCLUDED_)
