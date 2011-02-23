// TimeMng.h: interface for the CTimeMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEMNG_H__8767A9CD_893C_4D65_ACE2_F575DB2F5F11__INCLUDED_)
#define AFX_TIMEMNG_H__8767A9CD_893C_4D65_ACE2_F575DB2F5F11__INCLUDED_

class CTimeMng  
{
public:
	CTimeMng();
	virtual ~CTimeMng();

	int SetCurTime( void* v_pTmSet );
	unsigned int GetTickCount();

private:
	int _SemCreate();
	int _SemP();
	int _SemV();
	int _ShmCreate();

private:
	int	m_iSemID;
	int	m_iShmID;
	char *m_pMemShare;
};

#endif // !defined(AFX_TIMEMNG_H__8767A9CD_893C_4D65_ACE2_F575DB2F5F11__INCLUDED_)
