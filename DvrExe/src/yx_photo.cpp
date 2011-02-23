#include "yx_common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_JPEG_BUF 0x120000

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2

VENC_CHN_STAT_S objVencStat;
bool PhotoReady(void *Chn)
{
	int iRet = 0;

	memset(&objVencStat, 0, sizeof(objVencStat));
				
	iRet = HI_MPI_VENC_Query(*((int*)Chn), &objVencStat);
	if(iRet) 
	{
		//PRTMSG(MSG_ERR, "query video stream failed: 0x%08x\n", iRet);
		return false;
	}
				
	if( !objVencStat.u32CurPacks )
	{
		//PRTMSG(MSG_ERR, "video stream is empty\n");
		return false;
	}
			
	return true;
}
#endif

void *G_ThdShot( void* v_pVoid )
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);// 允许线程接收取消请求
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);// 接收到取消请求后，线程立即退出
	
	g_objPhoto.P_ThdShot();
	
	pthread_exit(0);
}

CPhoto::CPhoto()
{
	m_bInitSucc = false;
	m_wCurPhotoIndex = 1; // 程序初始化时用配置文件里的记录值赋值

	strcpy(m_strCamPath, PHOTO_SAVE_PATH);

	m_objDataMngToShot.InitCfg( 20000, 1000000 );
	m_hThdShot = 0;
	m_bEvtExit = false;
	m_bEvtPause = false;
}

CPhoto::~CPhoto()
{

}

unsigned long CPhoto::P_ThdShot()
{
	tagPhotoInfo objToShot;
	DWORD dwDataLen = 0;
	DWORD dwPushTm = 0;
	DWORD dwPktNum = 0;
	BYTE bytLvl = 0;
	BYTE bytFailType = 0;
	int iResult = 0;
	DWORD dwSleep = 500;
	DWORD dwLstBegin = 0;
	DWORD dwLstEndTm = 0;
	int	  iret = 0;
	
	g_objWorkStart.Picture = TRUE;
	
	while( WaitForEvent(NULL, &m_bEvtExit, 0) == 1 )
	{
		if( m_objDataMngToShot.IsEmpty() )// 拍照任务结束则退出线程
		{
			PRTMSG(MSG_DBG, "photo quene is empty, exit photo thread!\n");
			break;
		}

		dwSleep = 1000;

		if(	WaitForEvent(NULL, &m_bEvtPause, 0) == 1 ) // 若不需要暂停拍照
		{
			if( (iret = m_objDataMngToShot.PopData(bytLvl, sizeof(objToShot), dwDataLen, (char*)&objToShot, dwPushTm) ) )
			{
				PRTMSG(MSG_DBG, "m_objDataMngToShot PopData = %d\n", iret);
				continue;
			}

			PRTMSG(MSG_DBG, "Shot Photo\n");
			DWORD dwCur = GetTickCount();

#if USE_PROTOCOL == 30 // 2客业务中是可调间隔
			DWORD dwInterval = dwCur - dwLstBegin; // 距上次开始拍照时的时间跨度
			BYTE bytInterval = m_objPhotoSta.m_aryPhotoInfo[ m_objPhotoSta.m_bytPhotoCt ].m_bytInterval;
			if( bytInterval > 0x3c ) 
			{
				bytInterval = 0x3c;
			}
			else if( bytInterval < 0x03 ) 
			{
				bytInterval = 0x03;
			}
				
			if( dwInterval < DWORD(bytInterval) * 1000 )
			{
				dwSleep = DWORD(bytInterval) * 1000 - dwInterval;
			}
			else
			{
				dwSleep = 0;
			}
#endif
				
#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
			dwSleep = 0;
#endif

			if( dwLstEndTm > 0 )
			{
				if( dwCur + dwSleep < dwLstEndTm + PHOTO_INTERVAL )
				{
					dwSleep = dwLstEndTm + PHOTO_INTERVAL - dwCur; // 距离上次拍照结束至少要休息一会
				}
			}

			WaitForEvent(NULL, &m_bEvtExit, dwSleep);
			dwSleep = 0;

			dwLstBegin = GetTickCount();
			if( !_DealOnePhoto(bytFailType, objToShot) )	// 拍照成功
			{
				BYTE bytSend[100] = {0};
				bytSend[0] = 0x02;	
				memcpy(bytSend+1, (char*)&objToShot, sizeof(objToShot));
				DataPush(bytSend, (DWORD)(sizeof(objToShot)+1), DEV_DVR, DEV_QIAN, LV2);
			}
			else		// 拍照失败
			{
				BYTE bytSend[100] = {0};
				bytSend[0] = 0x02;	
				objToShot.m_wPhotoIndex = 0xffff;	// 失败仍然要通知QianExe，不过要将索引号置为最大
				memcpy(bytSend+1, (char*)&objToShot, sizeof(objToShot));
				DataPush(bytSend, (DWORD)(sizeof(objToShot)+1), DEV_DVR, DEV_QIAN, LV2);
			}

			dwLstEndTm = GetTickCount();
		}
		else
		{
			dwSleep = 200;
		}

		msleep(dwSleep);
	}
	
	m_objDataMngToShot.Release();
	g_objWorkStart.Picture = FALSE;

	return 0;
}

int CPhoto::Init()
{
	char szTempBuf[1024];
	
	if( m_bInitSucc ) return 0;

	int iRet = 0;
	FILE* fp = NULL;

	Release();

	// 读取最新的可用照片索引
	fp = fopen( SPRINTF(szTempBuf, "%s%s", m_strCamPath, CAMINDEXNAME), "rb" );
	if( fp )
	{
		fread( &m_wCurPhotoIndex, sizeof(m_wCurPhotoIndex), 1, fp );
		fclose( fp );
		fp = NULL;
	}

	m_bEvtExit = false;
	m_bEvtPause = false;
//	pthread_create(&m_hThdShot, NULL, G_ThdShot, NULL);

	m_bInitSucc = true;

	return 0;

_INIT_FAILED:
	Release();
	return iRet;
}

int CPhoto::Release()
{
	m_bEvtPause = true;
	if( m_hThdShot )
	{
		m_bEvtExit = true;
		msleep(50);
		
		if( WaitForEvent(ThreadExit, &m_hThdShot, 2500) )
		{
			pthread_cancel(m_hThdShot);
			msleep(50);
		}

		m_hThdShot = 0;
	}
	m_bEvtExit = false;

	m_bInitSucc = false;

	return 0;
}

int CPhoto::ReadIndex()
{
	FILE *fp = NULL;
	char szTempBuf[1024];

	// 读取最新的可用照片索引
	fp = fopen( SPRINTF(szTempBuf, "%s%s", m_strCamPath, CAMINDEXNAME), "rb" );
	if( fp )
	{
		fread( &m_wCurPhotoIndex, sizeof(m_wCurPhotoIndex), 1, fp );
		fclose( fp );
		fp = NULL;

		PRTMSG(MSG_NOR, "Read photo index, m_wCurPhotoIndex = %d\n", m_wCurPhotoIndex);
	}
// 	else
// 	{
// 		PRTMSG(MSG_NOR, "open photo index file failed, file: %s\n", szTempBuf);
// 	}
}

/// 输入的时间字符串格式: yymmddhhmmss
int CPhoto::Shot( WORD v_wPhotoType, DWORD v_dwDate, DWORD v_dwTime, BYTE v_bytTimeSymb, char* v_szLonLat, 
	BYTE v_bytChannel, BYTE v_bytSize, BYTE v_bytQuality, BYTE v_bytBright, BYTE v_bytContrast, // add v_bytChannel (modify by dkm)
	BYTE v_bytHue, BYTE v_bytBlueSaturation, BYTE v_bytRedSaturation, WORD &v_wPhotoIndex )
{
	char szTempBuf[1024];
	
	int i;
	int iRet = 0;
	FILE* fpIndex = NULL;
	FILE* fpPic = NULL;
	BYTE bytQuality = 50;
	uint uiPicLen = 0;
	BYTE *pPicBuf = NULL;
	char strJpgName[1024];
	WORD wNextPhotoIndex = 1;

	/// 生成照片存储目录
	IN_MakePhotoDirName( v_dwDate, m_strJpgDir );
	mkdir(m_strJpgDir, 0777);

	/// 更新照片索引并保存在文件上 (必须在保存照片文件之前,否则若失败,可能产生多个照片索引相同)
	wNextPhotoIndex = m_wCurPhotoIndex + 1;
	if( 0x7f == LOBYTE(wNextPhotoIndex) )
		wNextPhotoIndex ++;
	if( 0x7f == HIBYTE(wNextPhotoIndex) )
		wNextPhotoIndex += 0x100;
	if( wNextPhotoIndex > PHOTOINDEX_MAX ) 
		wNextPhotoIndex = 1;
	fpIndex = fopen( SPRINTF(szTempBuf, "%s%s", m_strCamPath, CAMINDEXNAME), "wb" );
	if( fpIndex )
	{
		// 反复写同一个文件,时间长了会不会有坏块导致写入失败? 不会，底层驱动做了磨损平衡 
		if( 1 != fwrite(&wNextPhotoIndex, sizeof(wNextPhotoIndex), 1, fpIndex) )
		{
			PRTMSG(MSG_DBG, "Write Photo Index File Fail!\n");
			iRet = ERR_FILE_FAILED;
			goto SHOT_END;
		}
		else
		{
			PRTMSG(MSG_DBG, "Write Photo Index Succ, wNextPhotoIndex = %d\n", wNextPhotoIndex);
		}
	}
	else
	{
		PRTMSG(MSG_DBG, "Open Photo Index File Fail!\n");
		iRet = ERR_FILE_FAILED;
		goto SHOT_END;
	}
	
	// 通道号检查 (保证高4位为0,且值不为0,重要)
	v_bytChannel &= 0x0f;
	if( v_bytChannel == 0 ) 
		v_bytChannel = 1;

	// 分辨率检查
	if( v_bytSize > 3 || v_bytSize == 0 ) 
		v_bytSize = 1;

	// 质量等级转为数值
	switch( v_bytQuality )
	{
		case 1:
			bytQuality = PHOTO_QUALITYHIGH;
			break;
		case 2:
			bytQuality = PHOTO_QUALITYMID;
			break;
		case 3:
			bytQuality = PHOTO_QUALITYLOW;
			break;
		default:
			bytQuality = PHOTO_QUALITYMID;
	}

	pPicBuf = (BYTE *)malloc(MAX_JPEG_BUF);
	if(NULL == pPicBuf)
	{
		PRTMSG(MSG_DBG, "Malloc Photo Buf Fail!\n");
		iRet = ERR_FILE_FAILED;
		goto SHOT_END;
	}

	for( i=0; i<=3; i++)
	{
		if( v_bytChannel & (0x01<<i) )
		{
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
			VENC_CHN_ATTR_S objChnAttr;
		 	VENC_ATTR_JPEG_S objJpegAttr;
		 	//VENC_CHN_STAT_S objVencStat;
			VENC_STREAM_S  	objVencStream;	
			VENC_PACK_S* pobjData;
			BYTE *pDataBuf = pPicBuf;
			
		 	objChnAttr.enType = PT_JPEG;
			objChnAttr.pValue = (HI_VOID *)&objJpegAttr;
		 	objJpegAttr.bVIField = HI_TRUE;//静态属性,输入图像的帧场标志(1:场;0:帧)
			objJpegAttr.bByFrame = HI_TRUE;//静态属性,获取码流模式(1:按帧获取;0:按包获取)
			objJpegAttr.u32ImageQuality = bytQuality==PHOTO_QUALITYHIGH?1:(bytQuality=PHOTO_QUALITYMID?2:3);//动态属性,抓拍图像质量(0~5,数字越小图像质量越好)
			
		 	switch(v_bytSize)
		 	{
		 		case 1://QCIF
		 			objJpegAttr.u32BufSize = 176*144*2;//静态属性,配置buffer大小(不小于图像宽高乘积的1.5倍)
					objJpegAttr.u32PicWidth = 176;//静态属性,编码图像宽度(80~2048)
					objJpegAttr.u32PicHeight = 144;//静态属性,编码图像高度(64~1536)
					objJpegAttr.u32MCUPerECS = 99;//动态属性,每个ECS中MCU个数,0~MCU总数,建议不小于16个,MCU总数即图像中宏块(16像素*16像素)个数。(本版本只支持u32MCUPerECS=heigh*width/256)
		 			break;
		 		case 2://CIF
		 			objJpegAttr.u32BufSize = 352*288*2;
					objJpegAttr.u32PicWidth = 352;
					objJpegAttr.u32PicHeight = 288;
					objJpegAttr.u32MCUPerECS = 396;
		 			break;
		 		case 3://D1
		 			objJpegAttr.u32BufSize = 720*576*2;
					objJpegAttr.u32PicWidth = 720;
					objJpegAttr.u32PicHeight = 576;
					objJpegAttr.u32MCUPerECS = 1620;
		 			break;
		 		default:
		 			break;
		 	}
			
			iRet = HI_MPI_VENC_CreateChn(i+8, &objChnAttr, HI_NULL);
			if(iRet) 
			{
				PRTMSG(MSG_DBG, "Create Jpeg Channel Fail:0x%08x\n", iRet);
		    goto SHOT_END;
			}
				
			iRet = HI_MPI_VENC_RegisterChn(i+8, i+8);
			if(iRet) 
			{
				PRTMSG(MSG_DBG, "Register Jpeg Channel Fail:0x%08x\n", iRet);
		    goto SHOT_END;
			}
			
			iRet = HI_MPI_VENC_StartRecvPic(i+8);
			if(iRet)
			{
				PRTMSG(MSG_ERR, "Start Jpeg Channel Fail:0x%08x\n", iRet);
				goto SHOT_END;
			}
		  
			int iChn = i+8;
			if( WaitForEvent(PhotoReady, &iChn, 3000) )
			{
				PRTMSG(MSG_ERR, "photo is not ready\n");
				goto SHOT_END;
			}

			memset(&objVencStream, 0, sizeof(objVencStream));
			objVencStream.u32PackCount = objVencStat.u32CurPacks;
			objVencStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S)*objVencStat.u32CurPacks);
			if(objVencStream.pstPack==NULL)
			{
				PRTMSG(MSG_ERR, "malloc stream pack err\n");
				goto SHOT_END;
			}
					
			iRet = HI_MPI_VENC_GetStream(i+8, &objVencStream, HI_IO_BLOCK);
			if(iRet)
			{
				PRTMSG(MSG_DBG, "Get Jpeg Stream Fail:0x%08x\n", iRet);
				free(objVencStream.pstPack);
				goto SHOT_END;
			}

			for( int j=0; j<objVencStream.u32PackCount; j++ )
			{
				pobjData = &(objVencStream.pstPack[j]);
				memcpy(pDataBuf, pobjData->pu8Addr[0], pobjData->u32Len[0]);
				pDataBuf += pobjData->u32Len[0];

				memcpy(pDataBuf, pobjData->pu8Addr[1], pobjData->u32Len[1]);
				pDataBuf += pobjData->u32Len[1];
				uiPicLen += (pobjData->u32Len[0] + pobjData->u32Len[1]);
			}
					
			iRet = HI_MPI_VENC_ReleaseStream(i+8, &objVencStream);            
			if(iRet)
			{
				PRTMSG(MSG_DBG, "Release Jpeg Stream Fail:%08x\n", iRet);
				free(objVencStream.pstPack);
				goto SHOT_END;
			}
		  
			free(objVencStream.pstPack);
		  
			iRet = HI_MPI_VENC_StopRecvPic(i+8);
			iRet = HI_MPI_VENC_UnRegisterChn(i+8);
			iRet = HI_MPI_VENC_DestroyChn(i+8);
#endif
	    
			/// 生成照片文件 (要在生成新索引之后) (通道号要注意,转换为字符串后不能超过1个字节)
			// (命名规则: 索引号(4)_日期时间(12)日期时间类型(1)_照片类型(4)_经纬度及有效标志(18)通道号(1)分辨率(1)质量(1).jpg)
			sprintf(strJpgName, "%s%04x_%06d%06d%c_%04x_%s%x%d%d.jpg", 
								m_strJpgDir, m_wCurPhotoIndex, v_dwDate, v_dwTime, v_bytTimeSymb, v_wPhotoType, v_szLonLat,
								v_bytChannel, v_bytSize <= 9 ? v_bytSize : 0, v_bytQuality <= 9 ? v_bytQuality : 0);

	    fpPic = fopen( strJpgName, "wb+" );
			if( fpPic )
			{
				pPicBuf[1] = 0xD8;

				if( 1 != fwrite(pPicBuf, uiPicLen, 1, fpPic) )
				{
					PRTMSG(MSG_DBG, "Write Photo File Fail!\n");
					iRet = ERR_FILE_FAILED;
					goto SHOT_END;
				}
			}
			else
			{
				PRTMSG(MSG_DBG, "Open Photo File Fail!\n");
				iRet = ERR_FILE_FAILED;
				goto SHOT_END;
			}
		}
	}
	
	v_wPhotoIndex = m_wCurPhotoIndex; // 返回值
	m_wCurPhotoIndex = wNextPhotoIndex; // 内存中同步更新索引

SHOT_END:

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	HI_MPI_VENC_StopRecvPic(i+8);
	HI_MPI_VENC_UnRegisterChn(i+8);
	HI_MPI_VENC_DestroyChn(i+8);
#endif
	
	if( fpPic ) 
	{
		fclose( fpPic );
		fpPic = NULL;
	}
	if( fpIndex ) 
	{
		fclose( fpIndex );
		fpIndex = NULL;
	}

	free (pPicBuf);

	return iRet;
}

/// 执行拍照,如果拍照失败,将有一个协议返回值,与内部返回值不同,该值与中心抓拍协议对应,
int CPhoto::_DealOnePhoto( BYTE &v_bytFailType, tagPhotoInfo& v_objPhotoToShot )
{
	DWORD dwDate = 0, dwTime = 0;
	WORD wPhotoIndex = 0;
	char szLonLat[ 100 ] = { 0 };
	BYTE bytTimeSymb;

	WORD wPhotoType = v_objPhotoToShot.m_wPhotoType;
	BYTE bytChannel = v_objPhotoToShot.m_bytChannel;
	BYTE bytSizeType = v_objPhotoToShot.m_bytSizeType;
	BYTE bytQualityType = v_objPhotoToShot.m_bytQualityType;
	BYTE bytBright = v_objPhotoToShot.m_bytBright;
	BYTE bytContrast = v_objPhotoToShot.m_bytContrast;
	BYTE bytHue = v_objPhotoToShot.m_bytHue;
	BYTE bytBlueSaturation = v_objPhotoToShot.m_bytBlueSaturation;
	BYTE bytRedSaturation = v_objPhotoToShot.m_bytRedSaturation;

	_MakeDateTimeLonLat( dwDate, dwTime, bytTimeSymb, szLonLat, sizeof(szLonLat) / sizeof(szLonLat[0]) );
	
	int iRet = Shot( wPhotoType, dwDate, dwTime, bytTimeSymb, szLonLat, 
		               bytChannel, bytSizeType, bytQualityType, bytBright, bytContrast,
		               bytHue, bytBlueSaturation, bytRedSaturation, wPhotoIndex );

	switch( iRet )
	{
		case 0:
		{
			// 记录照片信息
			v_objPhotoToShot.m_szDateTime[0] = dwDate / 10000 ? char(dwDate / 10000) : 0x7f;
			v_objPhotoToShot.m_szDateTime[1] = char(dwDate % 10000 / 100);
			v_objPhotoToShot.m_szDateTime[2] = char(dwDate % 100);
			v_objPhotoToShot.m_szDateTime[3] = dwTime / 10000 ? char(dwTime / 10000) : 0x7f;
			v_objPhotoToShot.m_szDateTime[4] = dwTime % 10000 / 100 ? char(dwTime % 10000 / 100) : 0x7f;
			v_objPhotoToShot.m_szDateTime[5] = dwTime % 100 ? char(dwTime % 100) : 0x7f;
			v_objPhotoToShot.m_dwPhotoDate = dwDate; // 记录照片日期
			v_objPhotoToShot.m_wPhotoIndex = wPhotoIndex; // 照片索引
			//v_objPhotoToShot.m_dwPhotoEndTm = GetTickCount(); // 记录本张照片结束拍摄的时刻
		}
			break;
		case ERR_PHOTO:
			v_bytFailType = 3;
			break;
		default:
			v_bytFailType = 2;
	}

	return iRet;
}

/// 输出：年(2)月(2)日(2)时(2)分(2)秒(2), 经度(9)纬度(8)有效标志(1)
// 输入参数:v_sizLonLat,表示输出经纬度字符串缓冲区的最大存储字符个数 (是字符数,不是字节数)
void CPhoto::_MakeDateTimeLonLat( DWORD &v_dwDate, DWORD &v_dwTime, BYTE &v_bytTimeSymb, char* v_szLonLat, size_t v_sizLonLat )
{
	if( v_sizLonLat < 19 )
	{
		v_dwDate = v_dwTime = 0;
		
		if( v_sizLonLat > 0 )
		{
			memset( v_szLonLat, 0x0030, v_sizLonLat );
			v_szLonLat[ v_sizLonLat - 1 ] = 0;
		}

		return;
	}

	tagGPSData gps(0);
	int iResult = GetCurGps( &gps, sizeof(gps), GPS_LSTVALID ); // 获取最后一个有效的GPS数据
	if( 'A' == gps.valid || gps.nYear > VALIDYEAR_BEGIN || (VALIDYEAR_BEGIN == gps.nYear && gps.nMonth >= VALIDMONTH_BEGIN) )
	{
		v_dwDate = DWORD( (gps.nYear - 2000) * 10000 + gps.nMonth * 100 + gps.nDay );
		v_dwTime = DWORD( gps.nHour * 10000 + gps.nMinute * 100 + gps.nSecond );
		v_bytTimeSymb = 'G';
	}
	else
	{ 
		int iCurDateTime[6];
		GetCurDateTime( iCurDateTime );
	
		if( iCurDateTime[0] > VALIDYEAR_BEGIN || (VALIDYEAR_BEGIN == iCurDateTime[0] && iCurDateTime[1] >= VALIDMONTH_BEGIN) )
		{
			v_dwDate = DWORD(iCurDateTime[0] - 2000) * 10000 + DWORD(iCurDateTime[1]) * 100 + iCurDateTime[2];
			v_dwTime = DWORD(iCurDateTime[3]) * 10000 + DWORD(iCurDateTime[4]) * 100 + iCurDateTime[5];
			v_bytTimeSymb = 'S';
		}
		else
		{
			// 若系统时间也不正常,则还是使用GPS时间,这样在报警拍照时,MT上看到的GPS时间和照片时间一致
			v_dwDate = DWORD( (gps.nYear - 2000) * 10000 + gps.nMonth * 100 + gps.nDay );
			v_dwTime = DWORD( gps.nHour * 10000 + gps.nMinute * 100 + gps.nSecond );
			v_bytTimeSymb = 'G';
		}
	}

	sprintf( v_szLonLat, "%09d%08d%c", long(gps.longitude * LONLAT_DO_TO_L), long(gps.latitude * LONLAT_DO_TO_L), gps.valid );
}

void DealPicEvent(tagPhotoInfo *v_objPhotoToShot, SLEEP_EVENT *v_objSleepEvent, WORK_EVENT *v_objWorkEvent, CTRL_EVENT *v_objCtrlEvent)
{
	DWORD dwPktNum = 0; 
	//static int i = 0;

	switch(g_enumSysSta)
	{
		case SLEEP:
		{
			v_objSleepEvent->dwStopEvt |= EVENT_TYPE_CENT;
		}
		case IDLE:
		case WORK:
		{
			v_objWorkEvent->dwPicStartEvt |= EVENT_TYPE_CENT;
			g_objPhoto.m_objDataMngToShot.PushData( 1, sizeof(tagPhotoInfo), (char*)v_objPhotoToShot, dwPktNum );
		}
			break;
		default:
			break;
	}
}

int StartPicture()
{
	if( !g_objWorkStart.Picture )
	{
		PRTMSG(MSG_NOR, "Start Photo\n");
		
		int iRet = pthread_create(&g_objPhoto.m_hThdShot, NULL, G_ThdShot, NULL);
		if(iRet) 
		{
			PRTMSG(MSG_ERR, "start Photo failed: %08x\n", iRet);
			return -1;
		}
		
		sleep(1);//延时一秒确保线程成功启动
		return 0;
	}

	return -1;
}

int StopPicture()
{
	if( g_objWorkStart.Picture )
	{
		PRTMSG(MSG_NOR, "Stop Photo\n");
		
		g_objPhoto.m_bEvtExit = TRUE;
		pthread_join(g_objPhoto.m_hThdShot, NULL);
		g_objPhoto.m_hThdShot = 0;
		
		PRTMSG(MSG_NOR, "stop photo succ\n");
		return 0;
	}
	
	return -1;
}



