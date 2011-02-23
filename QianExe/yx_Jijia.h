// Jijia.h: interface for the CJijia class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JIJIA_H__2F85B20E_C4F0_4C7A_9823_4F1EAFF22337__INCLUDED_)
#define AFX_JIJIA_H__2F85B20E_C4F0_4C7A_9823_4F1EAFF22337__INCLUDED_

#if USE_JIJIA == 1

// #if _MSC_VER > 1000
// #pragma once
// #endif // _MSC_VER > 1000

class CJijia  
{
public:
	CJijia();
	virtual ~CJijia();

	int BeginJijia( bool v_bJijiaDown );

};

#endif

#endif // !defined(AFX_JIJIA_H__2F85B20E_C4F0_4C7A_9823_4F1EAFF22337__INCLUDED_)

