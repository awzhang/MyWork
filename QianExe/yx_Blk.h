#ifndef	_YX_BLK_H_
#define _YX_BLK_H_

void G_TmBlkSave(void *arg, int len);
void G_TmBlkSndRes(void *arg, int len);
void G_TmBlkRcvQuery(void *arg, int len);

class CBlk
{
public:
	CBlk();
	virtual ~CBlk();
	
	void Init();	
	void BlkSaveStart();
	void BlkSaveStop();	
	void SaveCurBlkGps();
	int  GetLatestBlkGps( tagGPSData& v_objGps );
	void DelOlderBlk( const tagGPSData& v_objGps );
	void DelLaterBlk( const tagGPSData& v_objGps );
	int  Deal0e20( char* v_szData, DWORD v_dwDataLen );
	int  Deal0e21( char* v_szData, DWORD v_dwDataLen );
	int  Deal1012( char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	
	void P_TmBlkSave();
	void P_TmBlkSndRes();
	void P_TmBlkRcvQuery();
	
private:	
	int  _GetCurBlackSaveGps( tagBlackSaveGps &v_objBlackSaveGps );
	void _GpsToBlackSaveGps( const tagGPSData &v_objGps, tagBlackSaveGps &v_objBlackSaveGps );
	void _BlackSaveGpsToGps( const tagBlackSaveGps &v_objBlkSaveGps, tagGPSData &v_objGps );
	void _BlackSaveGpsToBlackGps( tagBlackSaveGps &v_objBlackSaveGps, tag0e60BlackGps &v_objBlackGps );
	int  _SaveBlackGps( const tagBlackSaveGps &v_objBlackSaveGps );
	bool _GetPeriodFromBlkFileName( char* v_szBlkFileName, BYTE& v_bytYear, BYTE& v_bytMon, BYTE& v_bytDayNo );	
	
private:
	tagBlackQuerySta m_objBlackQuerySta;
	tagBlackSndSta	m_objBlackSndSta;
	
	int		m_i0e60Len;
	tag0e60	m_obj0e60; // 缓存要发送的黑匣子数据帧
	
	char	m_szBlackPath[MAX_PATH];
	char	m_szBlackFile[MAX_PATH];
	
	FILE*	m_pfBlack;
	int		m_iBlkFile;
	
	tagBlackSaveGps m_aryBlackGps[ 100 ];
	DWORD	m_dwLstGpsSaveTm;
	WORD	m_wBlkGpsCt;
};

#endif

