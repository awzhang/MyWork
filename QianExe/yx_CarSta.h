#ifndef _YX_CARSTA_H_
#define _YX_CARSTA_H_

class CCarSta
{
public:
	CCarSta();
	virtual ~CCarSta();

public:	
	void Init();
	void Release();
	
	void SetVeSta( const DWORD v_dwVeSta );
	DWORD GetVeSta();
	
	void SavDrvRecSta( bool v_bBeforeReset );
	void SavLstValidLonlatAndLichen( long v_lLon, long v_lLat, double v_dLichen );
	void GetLstValidLonlatAndLichen( long& v_lLon, long& v_lLat, double& v_dLichen );
	void SavDriverNo( WORD v_wDriverNo );
	WORD GetDriverNo();
	
	void SavOil( BYTE v_bytOilAD, BYTE v_bytPowAD, BYTE v_bytOilSta );
	void GetOil( BYTE *v_p_bytOilAD, BYTE *v_p_bytPowAD, BYTE *v_p_bytOilSta );
	
	int ResumeDrvSta();
	
private:
	void _ReadDrvRecSta();
	
private:
	DWORD m_dwVeSta;
	
	tagDrvRecSta	m_objDrvRecSta; // ÐÐÊ»×´Ì¬»º´æÐÅÏ¢
	pthread_mutex_t m_VestaMutex;
	
	bool m_bDrvRecStaChg;
};

#endif



