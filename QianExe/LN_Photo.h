#ifndef _LNPHOTO_H_
#define _LNPHOTO_H_

#define MAX_TIMESEG_COUNT 1

struct PhotoTimeSeg
{
	byte bBegin[6];
	byte bEnd[6];
};
class CLNPhoto
{
public:
	CLNPhoto();
	virtual ~CLNPhoto();

	int Init();
	int Release();
	void P_ThreadSendPhoto();
	void P_TmPhotoTimer();
	void AddPhoto( WORD v_wPhotoType );
	int Deal02_0002(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal02_0003(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);

	int Deal04_0001(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal04_0002(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal42_0004(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);

	void SendPhotoRequest( BYTE v_bytSizeType, BYTE v_bytQualityType, BYTE v_bytBright,
						   BYTE v_bytContrast, BYTE v_bytHue, BYTE v_bytBlueSaturation, BYTE v_bytRedSaturation,
						   BYTE v_bytPhotoTotal, BYTE v_bytChannel, BYTE v_bytInterval, WORD v_wPhotoType );
	void DealOnePhotoEnd();
	void P_AddUploadPhoto( tagPhotoInfo* v_aryPhoto, BYTE v_bytPhotoTotal );

	void IN_GetChannelSizeQualityFromPhotoName( const char *const v_szPhotoName, BYTE &v_bytChannel, BYTE &v_bytSize, BYTE &v_bytQuality );
	void IN_GetLonLatFromPhotoName( const char *const v_szPhotoName, long& v_lLon, long &v_lLat, BYTE &v_bytValid );
	void IN_GetIndexFromPhotoName( const char *const v_szPhotoName, WORD &v_wIndex );
	void IN_GetTypeFromPhotoName( const char *const v_szPhotoName, WORD &v_wPhotoType );
	void IN_GetDateTimeFromPhotoName( const char *const v_szPhotoName, DWORD &v_dwDate, DWORD &v_dwTime );
	void IN_MakePhotoDirName( DWORD v_dwDate, char *v_strDirName );
	void IN_PutFileName(char *v_szBuf, DWORD v_dwDate, DWORD v_dwTime);

private:
	void _UploadPhotos();
	int  _UploadOnePhoto( WORD v_wPhotoIndex );
	int _UploadOnePhotoPack( WORD v_wPhotoIndex, WORD v_wBeginFrameNo, bool v_bNeedResp);
	void _ClearUploadPhotoStaForNewPhoto();
	WORD _UploadTypeToPhotoType( WORD v_wUploadType );
	bool FindFile(char *v_FileTime);
	int  _WaitForAnswer(DWORD v_dwWaitTm, char v_szDataType, char v_szResult, char *v_bytRecvBuf, DWORD v_dwBufLen, DWORD *v_dwRecvLen);
	int FillUpload( WORD v_wPhotoIndex);

	byte    m_bytSta;			// 状态机
public:
	tagPhotoSta m_objPhotoSta;
	tagLNUploadPhotoSta m_objUploadPhotoSta;

	tagPhotoInfo m_aryUpPhotoInfo[ UPLOADPHOTO_MAXCOUNT ]; // 所有要上传照片的信息数组 (只使用照片索引)
	BYTE    m_bytAllPhotoTotal;	// 所有要上传照片的总数
	BYTE    m_bytAllPhotoCt; // 所有已上传照片的计数

	CInnerDataMng m_objDataMngShotDone;
	CInnerDataMng m_objRecvMsg;
	char m_bytMd5[16];
	byte    m_bytBlockSta[16];// 块状态
	char m_szPhotoPath[255];
	pthread_t m_pThreadUp;
	bool    m_bThreadExit;		// 线程退出标志
	pthread_mutex_t m_mutex_photo;
	CMd5 cmd5;
};

/// 年(2)+月(2)+日(2)
inline void CLNPhoto::IN_MakePhotoDirName( DWORD v_dwDate, char* v_szDirName )
{
	if( v_dwDate >= BELIV_DATE )
	{
		sprintf(v_szDirName,"%s%d/", m_szPhotoPath, v_dwDate );
	} else
	{
		sprintf(v_szDirName,"%s%d/", m_szPhotoPath, UNBLV_DATE );
	}
}


// (命名规则: 索引号(4)_日期时间(12)日期时间类型(1)_照片类型(4)_经纬度及有效标志(18)_分辨率(1).jpg )
inline void CLNPhoto::IN_GetDateTimeFromPhotoName( const char *const v_szPhotoName, DWORD &v_dwDate, DWORD &v_dwTime )
{
	char* pStart = strrchr( v_szPhotoName, '/' );
	if( !pStart ) pStart = (char*)v_szPhotoName;
	else pStart++;

	char buf[ 16 ] = { 0};
	char* pPos;

	strncpy( buf, pStart + 5, 6 );
	buf[ 6 ] = 0;
	v_dwDate = strtoul( buf, &pPos, 10 );

	strncpy( buf, pStart + 11, 6 );
	buf[ 6 ] = 0;
	v_dwTime = strtoul( buf, &pPos, 10 );
}

inline void CLNPhoto::IN_GetIndexFromPhotoName( const char *const v_szPhotoName, WORD &v_wIndex )
{
	char* pStart = strrchr( v_szPhotoName, '/' );
	if( !pStart ) pStart = (char*)v_szPhotoName;
	else pStart++;

	char buf[ 16 ] = { 0};
	char* pPos;
	strncpy( buf, pStart, 4 );
	buf[ 4 ] = 0;
	v_wIndex = WORD( strtoul( buf, &pPos, 16 ) );
}

inline void CLNPhoto::IN_GetTypeFromPhotoName( const char *const v_szPhotoName, WORD &v_wPhotoType )
{
	char* pStart = strrchr( v_szPhotoName, '/' );
	if( !pStart ) pStart = (char*)v_szPhotoName;
	else pStart++;

	char buf[ 16 ] = { 0};
	char* pPos;
	strncpy( buf, pStart + 19, 4 );
	buf[ 4 ] = 0;
	v_wPhotoType = WORD( strtoul( buf, &pPos, 16 ) );

}

inline void CLNPhoto::IN_GetLonLatFromPhotoName( const char *const v_szPhotoName, long& v_lLon, long &v_lLat, BYTE &v_bytValid )
{
	char* pStart = strrchr( v_szPhotoName, '/' );
	if( !pStart ) pStart = (char*)v_szPhotoName;
	else pStart++;

	char buf[ 16 ] = { 0};
	char* pPos;

	strncpy( buf, pStart + 24, 9 );
	buf[ 9 ] = 0;
	v_lLon = strtol( buf, &pPos, 10 );

	strncpy( buf, pStart + 33, 8 );
	buf[ 8 ] = 0;
	v_lLat = strtol( buf, &pPos, 10 );

	v_bytValid = ((char*) (pStart + 41))[ 0 ]; // 对于经纬度有效标志可以这样简单
}

inline void CLNPhoto::IN_GetChannelSizeQualityFromPhotoName( const char *const v_szPhotoName, BYTE& v_bytChannel, BYTE &v_bytSize, BYTE &v_bytQuality )
{
	char* pStart = strrchr( v_szPhotoName, '/' );
	if( !pStart ) pStart = (char*)v_szPhotoName;
	else pStart++;

	char buf[ 16 ] = { 0};
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
inline void CLNPhoto::IN_PutFileName(char *v_szBuf, DWORD v_dwDate, DWORD v_dwTime)
{
	v_szBuf[0] = BYTE(v_dwDate / 10000);
	v_szBuf[1] = BYTE((v_dwDate % 10000) / 100);
	v_szBuf[2] = BYTE(v_dwDate % 100);
	v_szBuf[3] = BYTE(v_dwTime / 10000);
	v_szBuf[4] = BYTE((v_dwTime % 10000) / 100);
	v_szBuf[5] = BYTE(v_dwTime % 100);
}
#endif


