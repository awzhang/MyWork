// Gps.h: interface for the CGps class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPS_H__77528E24_7673_4EF5_8AEB_DE999E027214__INCLUDED_)
#define AFX_GPS_H__77528E24_7673_4EF5_8AEB_DE999E027214__INCLUDED_

#include "../GlobalShare/ComuServExport.h" 

class CGps
{
public:
	CGps();
	virtual ~CGps();

	int GetCurGps( void* v_pGps, const unsigned int v_uiSize, BYTE v_bytGetType );
	int SetCurGps( const void* v_pGps, BYTE v_bytSetType );

private:
	int _SemCreate();
	int _SemV();
	int _SemP();
	int _ShmCreate();
	void _BjTimeToGlwzTime(tm *v_Time, int *v_iTime);	// 北京时间转成格林威治时间

private:
	int m_iSemID;
	int m_iShmID;
	char* m_pShareMem;
};

#endif // !defined(AFX_GPS_H__77528E24_7673_4EF5_8AEB_DE999E027214__INCLUDED_)
