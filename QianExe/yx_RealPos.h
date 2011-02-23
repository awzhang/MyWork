// RealPos.h: interface for the CRealPos class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REALPOS_H__490AA131_7BDA_4722_885E_F3CC3B7009FB__INCLUDED_)
#define AFX_REALPOS_H__490AA131_7BDA_4722_885E_F3CC3B7009FB__INCLUDED_

// #include "../共用文件/ComuServExport.h"

// #if _MSC_VER > 1000
// #pragma once
// #endif // _MSC_VER > 1000

#define DegreeToNum(p)		( ((p)[0]*60 + (p)[1]) * 100 + (p)[2] )

#if USE_REALPOS == 1

class CRealPos  
{
public:
	CRealPos();
	virtual ~CRealPos();

	void Init();
	void Release();

	tag2QRealPosCfg GetRealPosCond();
	void SetRealPosCond( const tag2QRealPosCfg& v_objRealPosCond );

	void BeginRealPosChk();
	void EndRealPosChk();

	int Deal1007( char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen, bool v_bFromSM );

protected:
	void P_TmRealPosChk();

	friend void G_TmRealPosChk(void *arg, int len);
	
private:
// 	bool _CheckSanhuan(double lon,double lat);
// 	bool _CutLine(byte *dot1, byte *dot2, double lon,double lat);

private:
	tag2QRealPosCfg m_objPosCond;
	int m_iSanHuanCt;
};

#endif

#endif // !defined(AFX_REALPOS_H__490AA131_7BDA_4722_885E_F3CC3B7009FB__INCLUDED_)
