// Jijia.cpp: implementation of the CJijia class.
//
//////////////////////////////////////////////////////////////////////

#include "yx_QianStdAfx.h"
#include "yx_Jijia.h"

#if USE_JIJIA == 1

// #ifdef _DEBUG
// #undef THIS_FILE
// static char THIS_FILE[]=__FILE__;
// #define new DEBUG_NEW
// #endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJijia::CJijia()
{

}

CJijia::~CJijia()
{

}


int CJijia::BeginJijia( bool v_bJijiaDown )
{
	if( v_bJijiaDown )
	{
#if USE_AUTORPT == 1
		//g_objAutoRpt.InitAutoRpt();
		g_objAutoRpt.BeginAutoRpt(4);
#endif
		char szBuf[2] = {0};
		szBuf[0] = 0x04;
		szBuf[1] = 0x0b;
		DataPush(szBuf, 2, DEV_QIAN, DEV_DVR, LV2);
	}
	else
	{
#if USE_AUTORPT == 1
		//g_objAutoRpt.InitAutoRpt();
		g_objAutoRpt.BeginAutoRpt(3);
#endif
		char szBuf[2] = {0};
		szBuf[0] = 0x04;
		szBuf[1] = 0x0c;
		DataPush(szBuf, 2, DEV_QIAN, DEV_DVR, LV2);
	}
	
	return 0;
}

#endif
