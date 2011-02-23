#ifndef _YX_PHOTO_H_
#define _YX_PHOTO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPhoto  
{
public:
	CPhoto();
	virtual ~CPhoto();

	void IN_GetChannelSizeQualityFromPhotoName( const char *const v_szPhotoName, BYTE &v_bytChannel, BYTE &v_bytSize, BYTE &v_bytQuality );
	void IN_GetLonLatFromPhotoName( const char *const v_szPhotoName, long& v_lLon, long &v_lLat, BYTE &v_bytValid );
	void IN_GetIndexFromPhotoName( const char *const v_szPhotoName, WORD &v_wIndex );
	void IN_GetTypeFromPhotoName( const char *const v_szPhotoName, WORD &v_wPhotoType );
	void IN_GetDateTimeFromPhotoName( const char *const v_szPhotoName, DWORD &v_dwDate, DWORD &v_dwTime );
	void IN_MakePhotoDirName( DWORD v_dwDate, char *v_strDirName );

	int Init();
	int Release();

	int ReadIndex();

	int Shot( WORD v_wPhotoType, DWORD v_dwDate, DWORD v_dwTime, BYTE v_bytTimeSymb, char* v_szLonLat, 
		        BYTE v_bytChannel, BYTE v_bytSize, BYTE v_bytQuality, BYTE v_bytBright, BYTE v_bytContrast, // add v_bytChannel (modify by dkm)
		        BYTE v_bytHue, BYTE v_bytBlueSaturation, BYTE v_bytRedSaturation, WORD &v_wPhotoIndex );

	unsigned long P_ThdShot();

private:
	int _DealOnePhoto( BYTE &v_bytFailType, tagPhotoInfo& v_objPhotoToShot );
	void _InitPhoto( BYTE v_bytSizeType, BYTE v_bytQualityType, BYTE v_bytBright,
		               BYTE v_bytContrast, BYTE v_bytHue, BYTE v_bytBlueSaturation, BYTE v_bytRedSaturation,
		               BYTE v_bytPhotoTotal, BYTE v_bytChannel, BYTE v_bytInterval, WORD v_wPhotoType );	// add v_bytChannel (modify by dkm)

	void _MakeDateTimeLonLat( DWORD &v_dwDate, DWORD &v_dwTime, BYTE &v_bytTimeSymb, char* v_szLonLat, size_t v_sizLonLat );
	
public:
	pthread_t m_hThdShot;
	CInnerDataMng m_objDataMngToShot;

	bool m_bEvtExit;
	bool m_bEvtPause;
	bool m_bInitSucc;
	
	tagPhotoSta m_objPhotoSta;
	char m_strCamPath[1024];
	char m_strJpgDir[1024];
	WORD m_wCurPhotoIndex;
};


/// 年(2)+月(2)+日(2)
inline void CPhoto::IN_MakePhotoDirName( DWORD v_dwDate, char *v_strDirName )
{
	// strcpy(v_strDirName, m_strCamPath);

	//sprintf(v_strDirName, "%s%04d_%d/", m_strCamPath, v_dwDate / 100,
	//	v_dwDate % 100 % ONEPHOTODIR_DAYS ? v_dwDate % 100 / ONEPHOTODIR_DAYS + 1 : v_dwDate % 100 / ONEPHOTODIR_DAYS );
	
	if( v_dwDate >= BELIV_DATE )
		sprintf(v_strDirName, "%s%d/", m_strCamPath, v_dwDate);
	else
		sprintf(v_strDirName, "%s%d/", m_strCamPath, UNBLV_DATE);
}


// (命名规则: 索引号(4)_日期时间(12)日期时间类型(1)_照片类型(4)_经纬度及有效标志(18)_分辨率(1).jpg )

inline void CPhoto::IN_GetDateTimeFromPhotoName( const char *const v_szPhotoName, DWORD &v_dwDate, DWORD &v_dwTime )
{
	char* pStart = strrchr( v_szPhotoName, '/' );
	if( !pStart ) pStart = (char*)v_szPhotoName;
	else pStart++;

	char buf[ 16 ] = { 0 };
	char* pPos;

	strncpy( buf, pStart + 5, 6 );
	buf[ 6 ] = 0;
	v_dwDate = strtoul( buf, &pPos, 10 );

	strncpy( buf, pStart + 11, 6 );
	buf[ 6 ] = 0;
	v_dwTime = strtoul( buf, &pPos, 10 );
}

inline void CPhoto::IN_GetIndexFromPhotoName( const char *const v_szPhotoName, WORD &v_wIndex )
{
	char* pStart = strrchr( v_szPhotoName, '/' );
	if( !pStart ) pStart = (char*)v_szPhotoName;
	else pStart++;

	char buf[ 16 ] = { 0 };
	char* pPos;
	strncpy( buf, pStart, 4 );
	buf[ 4 ] = 0;
	v_wIndex = WORD( strtoul( buf, &pPos, 16 ) );
}

inline void CPhoto::IN_GetTypeFromPhotoName( const char *const v_szPhotoName, WORD &v_wPhotoType )
{
	char* pStart = strrchr( v_szPhotoName, '/' );
	if( !pStart ) pStart = (char*)v_szPhotoName;
	else pStart++;

	char buf[ 16 ] = { 0 };
	char* pPos;
	strncpy( buf, pStart + 19, 4 );
	buf[ 4 ] = 0;
	v_wPhotoType = WORD( strtoul( buf, &pPos, 16 ) );
}

inline void CPhoto::IN_GetLonLatFromPhotoName( const char *const v_szPhotoName, long& v_lLon, long &v_lLat, BYTE &v_bytValid )
{
	char* pStart = strrchr( v_szPhotoName, '/' );
	if( !pStart ) pStart = (char*)v_szPhotoName;
	else pStart++;

	char buf[ 16 ] = { 0 };
	char* pPos;

	strncpy( buf, pStart + 24, 9 );
	buf[ 9 ] = 0;
	v_lLon = strtol( buf, &pPos, 10 );

	strncpy( buf, pStart + 33, 8 );
	buf[ 8 ] = 0;
	v_lLat = strtol( buf, &pPos, 10 );

	v_bytValid = ((char*) (pStart + 41))[ 0 ]; // 对于经纬度有效标志可以这样简单
}

inline void CPhoto::IN_GetChannelSizeQualityFromPhotoName( const char *const v_szPhotoName, BYTE& v_bytChannel, BYTE &v_bytSize, BYTE &v_bytQuality )
{
	char* pStart = strrchr( v_szPhotoName, '/' );
	if( !pStart ) pStart = (char*)v_szPhotoName;
	else pStart++;

	char buf[ 16 ] = { 0 };
	char* pPos;

	strncpy( buf, pStart + 42, 1 );
	buf[ 1 ] = 0;
	v_bytChannel = BYTE( strtoul( buf, &pPos, 16 ) );

	strncpy( buf, pStart + 43, 1 );
	buf[ 1 ] = 0;
	v_bytSize = BYTE( strtoul( buf, &pPos, 10 ) );

	strncpy( buf, pStart + 44, 1 );
	buf[ 1 ] = 0;
	v_bytQuality = BYTE( strtoul( buf, &pPos, 10 ) );
}

void DealPicEvent(tagPhotoInfo *v_objPhotoToShot, SLEEP_EVENT *v_objSleepEvent, WORK_EVENT *v_objWorkEvent, CTRL_EVENT *v_objCtrlEvent);
int StartPicture();
int StopPicture();

#endif 
