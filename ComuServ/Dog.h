// Dog.h: interface for the CDog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOG_H__01824FC7_E924_4461_A01F_80A64890C007__INCLUDED_)
#define AFX_DOG_H__01824FC7_E924_4461_A01F_80A64890C007__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDog  
{
public:
	CDog();
	virtual ~CDog();

public:
	void DogClr( DWORD v_dwSymb );
	DWORD DogQuery();
	DWORD DogQueryOnly();
	void DogInit();

private:
	int _SemCreate();
	int _SemV();
	int _SemP();
	int _ShmCreate();

private:
	int m_iSemID;
	int m_iShmID;
	char* m_pShareMem;
};

#endif // !defined(AFX_DOG_H__01824FC7_E924_4461_A01F_80A64890C007__INCLUDED_)
