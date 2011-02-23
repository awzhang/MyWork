// CfgMng.h: interface for the CCfgMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFGMNG_H__163849FF_0879_481F_A962_4B4284650727__INCLUDED_)
#define AFX_CFGMNG_H__163849FF_0879_481F_A962_4B4284650727__INCLUDED_

#include "../GlobalShare/ComuServExport.h"

class CCfgMng
{
public:
	// 内部进行互斥处理
	int GetImpCfg( void *v_pDes, unsigned int v_uiDesSize, unsigned int v_uiGetBegin, unsigned int v_uiGetSize );
	int GetSecCfg( void *v_pDes, unsigned int v_uiDesSize, unsigned int v_uiGetBegin, unsigned int v_uiGetSize );
	int SetImpCfg( void *v_pSrc, unsigned int v_uiSaveBegin, unsigned int v_uiSaveSize );
	int SetSecCfg( void *v_pSrc, unsigned int v_uiSaveBegin, unsigned int v_uiSaveSize );
	int ResumeCfg( bool v_bReservTel, bool v_bReservIP );

	CCfgMng();
	virtual ~CCfgMng();

private:
	// 内部没有进行互斥处理
	int _InitImp();
	int _InitSec();
	int _ResumeSecCfg();
	int _ResumeImpCfg( bool v_bReservTel, bool v_bReservIP );
	//bool _SetApnInReg( const char *const v_szImpCfg );

	// 如下内部读写接口,必须从各配置参数区的首地址开始,并且内部没有进行互斥
	int _CfgGetImp( void* v_szGet, size_t v_sizGet );
	int _CfgSetImp( void* v_szSet, size_t v_sizSetLen );
	int _CfgGetSec( void* v_szGet, size_t v_sizGet );
	int _CfgSetSec( void* v_szSet, size_t v_sizSetLen );

	int _SemCreate();
	int _SemV();
	int _SemP();
	int _ShmCreate();

private:
	int m_iSemID;
	int m_iShmIDImpCfg;
	int m_iShmIDSecCfg;
	char *m_pMemShareImpCfg;
	char *m_pMemShareSecCfg;
	bool m_bInit;
};

#endif // !defined(AFX_CFGMNG_H__163849FF_0879_481F_A962_4B4284650727__INCLUDED_)
