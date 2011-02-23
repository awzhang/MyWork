#ifndef _YX_PHOTO_H_
#define _YX_PHOTO_H_

class CPhoto
{
public:
	CPhoto();
	virtual ~CPhoto();

	int Init();
	int Release();

	void P_TmPhotoTimer();
	void P_TmUploadPhotoWin();
	void P_TmUploadPhotoChk();

	int Deal4100(char* v_szData, DWORD v_dwDataLen);
	int Deal4101(char* v_szData, DWORD v_dwDataLen);
	int Deal4102(char* v_szData, DWORD v_dwDataLen);
	int Deal4103(char* v_szData, DWORD v_dwDataLen);
	int Deal4104(char* v_szData, DWORD v_dwDataLen);
	int Deal4110(char* v_szData, DWORD v_dwDataLen, bool v_bFrom4112 =false);
	int Deal4112(char* v_szData, DWORD v_dwDataLen);
	int Deal4113(char* v_szData, DWORD v_dwDataLen);

	void AddPhoto( WORD v_wPhotoType );
	void SendPhotoRequest( BYTE v_bytSizeType, BYTE v_bytQualityType, BYTE v_bytBright,
		BYTE v_bytContrast, BYTE v_bytHue, BYTE v_bytBlueSaturation, BYTE v_bytRedSaturation,
		BYTE v_bytPhotoTotal, BYTE v_bytChannel, BYTE v_bytInterval, WORD v_wPhotoType );
	void DealOnePhotoEnd();
	void P_AddUploadPhoto( tagPhotoInfo* v_aryPhoto, BYTE v_bytPhotoTotal );

	bool IsPhotoEnd();	
	bool IsUploadPhotoEnd();	
	bool IsUploadPhotoBreak();

	void IN_GetChannelSizeQualityFromPhotoName( const char *const v_szPhotoName, BYTE &v_bytChannel, BYTE &v_bytSize, BYTE &v_bytQuality );
	void IN_GetLonLatFromPhotoName( const char *const v_szPhotoName, long& v_lLon, long &v_lLat, BYTE &v_bytValid );
	void IN_GetIndexFromPhotoName( const char *const v_szPhotoName, WORD &v_wIndex );
	void IN_GetTypeFromPhotoName( const char *const v_szPhotoName, WORD &v_wPhotoType );
	void IN_GetDateTimeFromPhotoName( const char *const v_szPhotoName, DWORD &v_dwDate, DWORD &v_dwTime );
	void IN_MakePhotoDirName( DWORD v_dwDate, char *v_strDirName );

private:
	void _UploadPhotos();
	int  _UploadOnePhoto( WORD v_wPhotoIndex );
	int  _UploadOnePhotoWin( WORD v_wPhotoIndex, bool v_bSpcyFrame, WORD v_wBeginFrameNo, WORD v_wSpcyFrameTotal, WORD* v_arySpcyFrame );
	void _StopUploadPhoto();

	void _ClearUploadPhotoStaForNewPhoto();
	void _ClearUploadPhotoStaForNewWin();
	void _ConvertLonLatFromLongToStr( double v_dLon, double v_dLat, char* v_szLonLat, size_t v_sizStr );
	WORD _EvtTypeToPhotoType( BYTE v_bytEvtType );
	BYTE _PhotoTypeToEvtType( WORD v_wPhotoType );
	WORD _UploadTypeToPhotoType( WORD v_wUploadType );

public:
	tagPhotoSta m_objPhotoSta;
	tagUploadPhotoSta m_objUploadPhotoSta;

	CInnerDataMng m_objDataMngShotDone;

	char m_szPhotoPath[255];
};

/// 年(2)+月(2)+日(2)
inline void CPhoto::IN_MakePhotoDirName( DWORD v_dwDate, char* v_szDirName )
{
	if( v_dwDate >= BELIV_DATE )
	{
		sprintf(v_szDirName,"%s%d/", m_szPhotoPath, v_dwDate );
	}
	else
	{
		sprintf(v_szDirName,"%s%d/", m_szPhotoPath, UNBLV_DATE );
	}
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

#endif

