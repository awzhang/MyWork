#ifndef _LN_DATAMNG_H_
#define _LN_DATAMNG_H_

class CLN_DataMng
{
public:
	CLN_DataMng();
	virtual ~CLN_DataMng();

public:
	int Init();
	int Release();

	bool  IsCurNodeValid();
	bool  IsNeedDelCurNode();
	int   UpdateNode();
	DWORD GetCurNodeSEQ();
	int   GetCurNodeData(char *v_szBuf, DWORD v_dwBufSize, DWORD v_dwTm);
	int   GetCurNodeTm();

	int InsertOneNodeToTail(DWORD v_dwSEQ, char *v_szDataBuf, DWORD v_dwDataLen, int v_iNeedReUploadTimers, int v_dwLastUploadTm);
	int DeleteSpecialNode(DWORD v_dwSEQ);
	int DeleteCurNode();
	int GotoNextNode();
	int GotoHeadNode();

private:
	struct tagOneNode
	{
		struct tagOneNode *m_next;			// 下一节点指针
		struct tagOneNode *m_prev;			// 上一节点指针
		DWORD	m_dwSEQ;					// SEQ流水号
		char	*m_szDataBuf;				// 数据指针
		DWORD	m_dwDataLen;				// 数据长度
		int		m_iNeedReUploadTimers;		// 若需要重传，则重传的最大次数
		int		m_iTimers;					// 已传输次数
		DWORD	m_dwLastUploadTm;			// 上次传输时刻
	};

	tagOneNode	*m_HeadNode;
	tagOneNode	*m_TailNode;
	tagOneNode	*m_CurNode;
	DWORD		m_dwNodeCnt;
	
	pthread_mutex_t m_mutex_node;
};

#endif


