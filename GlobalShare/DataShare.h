#ifndef _YX_DATASHARE_H_
#define _YX_DATASHARE_H_


class CDataShare  
{
public:
	CDataShare();
	virtual ~CDataShare();
	
	unsigned short GetValidOilAD();
	void SetValidOilAD( unsigned short v_usValidOilAD );
	
private:
	HANDLE m_hMutexShare;
};

#endif

