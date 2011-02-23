#include "yx_QianStdAfx.h"

void G_TmPhotoTimer(void *arg, int len)
{
	g_objPhoto.P_TmPhotoTimer();
}

void G_TmUploadPhotoWin(void *arg, int len)
{
	g_objPhoto.P_TmUploadPhotoWin();
}

void G_TmUploadPhotoChk(void *arg, int len)
{
	g_objPhoto.P_TmUploadPhotoChk();
}

int SortWord( const void* v_pElement1, const void* v_pElement2 )
{
	return int( *(WORD*)v_pElement1 ) - int( *(WORD*)v_pElement2 );
}

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-Photo")

//////////////////////////////////////////////////////////////////////////

CPhoto::CPhoto()
{
	memset(m_szPhotoPath, 0, sizeof(m_szPhotoPath));
	sprintf(m_szPhotoPath, "%s", PHOTO_SAVE_PATH);

	m_objDataMngShotDone.InitCfg( 20000, 300000 );
}

CPhoto::~CPhoto()
{

}

int CPhoto::Init()
{
	P_TmPhotoTimer();
}

int CPhoto::Release()
{

}

void CPhoto::P_TmPhotoTimer()
{
	tag2QPhotoCfg objPhotoCfg;

	objPhotoCfg.Init();
	GetSecCfg( &objPhotoCfg, sizeof(objPhotoCfg), offsetof(tagSecondCfg, m_uni2QPhotoCfg.m_obj2QPhotoCfg), sizeof(objPhotoCfg) );

	if( objPhotoCfg.m_wPhotoEvtSymb & PHOTOEVT_TIMER )
	{
		AddPhoto( PHOTOEVT_TIMER );
		
		UINT uiInterval = 1000 * (objPhotoCfg.m_objTimerPhotoPar.m_bytIntervHour * 3600 + objPhotoCfg.m_objTimerPhotoPar.m_bytIntervMin * 60 );
	
		//if( uiInterval < 600000 )
		//	uiInterval = 600000;  // 最小间隔改成10分钟，防止大量拍照

		_SetTimer(&g_objTimerMng, TIMERPHOTO_TIMER, uiInterval, G_TmPhotoTimer);
	}
}

void CPhoto::P_TmUploadPhotoWin()
{
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	int iResult = 0;
	WORD wFrameNo;

	static WORD sta_wReChkUdpQueTime = 0;
	static bool sta_bUdpQueEmpty = true;
	bool bUploadPause = false;
	
	// 停止重传判断
	if( m_objUploadPhotoSta.m_bytAllPhotoCt >= m_objUploadPhotoSta.m_bytAllPhotoTotal
		|| m_objUploadPhotoSta.m_bytAllPhotoCt >= UPLOADPHOTO_MAXCOUNT )
	{
		PRTMSG(MSG_DBG, "All Photo Upload Succ!\n" );
		goto STOP_UPLOADPHOTO;
	}

	// 若当前上传照片的索引为0,中止本照片的重传处理 (保险起见)
	if( 0 == m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex )
	{
		++ m_objUploadPhotoSta.m_bytAllPhotoCt;
		PRTMSG(MSG_DBG, "The Index of the photo to be uploaded is 0, Stop Upload Photo!\n" );
		goto STOP_UPLOADPHOTO;
	}

	// 递增重发次数,超过最大次数后停止上传
// 	if( ++ m_objUploadPhotoSta.m_obj4152.m_bytReSndTime > REUPLOADPHOTO_TIME )
// 	{
// 		PRTMSG(MSG_DBG, "Reupload this Photo Win To Max Times!\n" );
// 
// 		// 通知底层检查GPRS在线状态
// 		// g_objSock.LetComuChkGPRS();
// 
// 		bUploadPause = true;
// 		goto STOP_UPLOADPHOTO;
// 	}

	++ m_objUploadPhotoSta.m_obj4152.m_bytReSndTime;

	// 发送重传帧
	iResult = g_objSms.MakeSmsFrame( (char*)&m_objUploadPhotoSta.m_obj4152, sizeof(m_objUploadPhotoSta.m_obj4152),
		0x41, 0x52, buf, sizeof(buf), iBufLen );
	if( !iResult )
	{
		iResult = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV8, DATASYMB_415052 ); // 优先级要和4150一样
		if( !iResult ) m_objUploadPhotoSta.m_byt4152Snd = 1;
	}

	wFrameNo = m_objUploadPhotoSta.m_obj4152.m_szFrameNo[0] * 0x100 + m_objUploadPhotoSta.m_obj4152.m_szFrameNo[1];
	PRTMSG(MSG_DBG, "Reuploading photo win %d--last packet No %d, the %d times\n", m_objUploadPhotoSta.m_wPhotoWinCt, wFrameNo, m_objUploadPhotoSta.m_obj4152.m_bytReSndTime );

	// 重新定时
	_SetTimer(&g_objTimerMng, UPLOAD_PHOTOWIN_TIMER, UPLOAD_PHOTOWIN_INTERVAL, G_TmUploadPhotoWin );
	return;

STOP_UPLOADPHOTO:
	// 中止重传 (既可以续传本幅照片,又可以跳到下个照片上传)
	_KillTimer(&g_objTimerMng, UPLOAD_PHOTOWIN_TIMER );

	if( bUploadPause )
	{
		m_objUploadPhotoSta.m_bUploadBreak = true; // 标记为上传中断,也即超时中止

		g_objSock.TcpLog(); // 重新TCP登陆以触发中心的断点续传 (询问过余建成)
	}

	PRTMSG(MSG_DBG, "Stop Upload Photo, because Reupload this Win To Max Times, or Failure happened\n" );
}

void CPhoto::P_TmUploadPhotoChk()
{
	// 递增上传照片计数
	++ m_objUploadPhotoSta.m_bytAllPhotoCt;
	
	// 判断是否全部照片上传结束
	if( m_objUploadPhotoSta.m_bytAllPhotoCt >= m_objUploadPhotoSta.m_bytAllPhotoTotal )
	{
		_StopUploadPhoto();
		m_objUploadPhotoSta.m_bytAllPhotoCt = m_objUploadPhotoSta.m_bytAllPhotoTotal = 0; // 清除上传照片计数和总数
	}
	else
	{
		// 继续上传下一照片
		PRTMSG(MSG_DBG, "Leave %d Photos To Upload,Next Photo Index:%d\n",
			m_objUploadPhotoSta.m_bytAllPhotoTotal - m_objUploadPhotoSta.m_bytAllPhotoCt,
			m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex );
		
		_KillTimer(&g_objTimerMng, UPLOAD_PHOTOCHK_TIMER );
		_UploadOnePhoto( m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex );
	}
}

int CPhoto::Deal4100( char* v_szData, DWORD v_dwDataLen )
{
	PRTMSG(MSG_NOR, "Rcv Cent 4100 Res\n" );
	
	WORD wResPhotoIndex = 0;
	wResPhotoIndex = BYTE(v_szData[0]) * WORD(0x100) + BYTE( v_szData[1] ); // 照片索引
	
	// 检查应答照片和当前上传的照片是否相同
	if( wResPhotoIndex != m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex )
	{
		PRTMSG(MSG_NOR, "Deal 4110, But the photo in res isn't the Uploading photo,Exit!", wResPhotoIndex );
		return ERR_PAR;
	}
	
	// 记录最新上传成功的照片索引
	m_objUploadPhotoSta.m_wLstSuccUpPhotoIndex =
		m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex;
	
	// 递增上传照片计数
	++ m_objUploadPhotoSta.m_bytAllPhotoCt;
	
	// 判断是否全部照片上传结束
	if( m_objUploadPhotoSta.m_bytAllPhotoCt >= m_objUploadPhotoSta.m_bytAllPhotoTotal )
	{
		_StopUploadPhoto();
		m_objUploadPhotoSta.m_bytAllPhotoCt = m_objUploadPhotoSta.m_bytAllPhotoTotal = 0; // 清除上传照片计数和总数
	}
	else
	{
		// 继续上传下一照片
		PRTMSG(MSG_NOR, "Leave %d Photos To Upload,Next Photo Index:%d\n",
			m_objUploadPhotoSta.m_bytAllPhotoTotal - m_objUploadPhotoSta.m_bytAllPhotoCt,
			m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex );
		
		_KillTimer(&g_objTimerMng, UPLOAD_PHOTOCHK_TIMER );
		_UploadOnePhoto( m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex );
	}
	
	return 0;
}

/// 中心照片抓拍请求
// 类型个数M(1)+【事件类型ID(1)+参数个数K(1)【+参数类型(1)+参数长度N(1)+参数(N)】× K】× M
int CPhoto::Deal4101( char* v_szData, DWORD v_dwDataLen )
{
	int iRet = 0;
	int iBufLen;
	char buf[ SOCK_MAXSIZE ];
	WORD wPhotoIndex = 0;
	BYTE bytChannel = 1;
	BYTE bytInterval = 0x03;
	BYTE bytPhotoTotal = 0;
	BYTE bytSizeType = 2;
	BYTE bytQualityType = 2;
	BYTE bytBright = 8;
	BYTE bytContrast = 8;
	BYTE bytHue = 8;
	BYTE bytBlueSaturation = 8;
	BYTE bytRedSaturation = 8;

	BYTE bytResType = 1;
	BYTE bytParTotal = 0, bytParCt = 0;
	BYTE bytParType = 0, bytParLen = 0;
	BYTE byt = 0;

	/// 参数尺寸检查
	if( v_dwDataLen < 1 )
	{
		bytResType = 2;
		iRet = ERR_PAR;
		goto DEAL4101_END;
	}

	/// 参数个数
	bytParTotal = BYTE( v_szData[0] );
	if( bytParTotal < 1 )
	{
		bytResType = 2;
		iRet = ERR_PAR;
		goto DEAL4101_END;
	}

	/// 轮询参数
	byt = 1;
	for( bytParCt = 0; bytParCt < bytParTotal; bytParCt ++ )
	{
		// 参数尺寸检查
		if( DWORD(byt + 2) > v_dwDataLen )
		{
			bytResType = 2;
			iRet = ERR_PAR;
			goto DEAL4101_END;
		}

		// 参数类型和长度
		bytParType = BYTE( v_szData[ byt ] );
		bytParLen = BYTE( v_szData[ byt + 1 ] );

		// 参数尺寸再次检查
		if( DWORD(byt + 2 + bytParLen) > v_dwDataLen )
		{
			bytResType = 2;
			iRet = ERR_PAR;
			goto DEAL4101_END;
		}

		switch( bytParType )
		{
		case 1: // 分辨率
				// 0x01：352×288
				// 0x02：176×144
				// 0x03：720×576
			{
				bytSizeType = BYTE( v_szData[ byt + 2 ] ); // 协议值
				
				// 转为内部使用值
				switch( bytSizeType )
				{
					case 1:
						bytSizeType = 2;
						break;
					case 2:
						bytSizeType = 1;
						break;
					default:
						;
				}
			}
			break;

		case 2:	// 照片通道号
			{
				bytChannel = BYTE( v_szData[ byt + 2 ] ) & 0x0f;
				if( 0 == bytChannel ) bytChannel = 1; // 通道号检查
			}
			break;

		case 3: // 图片质量
				// 0x01：恒定质量等级（高）
				// 0x02：恒定质量等级（中）
				// 0x03：恒定质量等级（低）
			{
				bytQualityType = BYTE( v_szData[ byt + 2 ] );
				if( bytQualityType < 1 ) bytQualityType = 1;
				if( bytQualityType > 3 ) bytQualityType = 3;
			}
			break;

		case 4: // 单次抓拍张数
			{
				bytPhotoTotal = BYTE( v_szData[ byt + 2 ] );
				if( bytPhotoTotal < 1 || bytPhotoTotal > 10 ) // (重要)
				{
					bytResType = 2;
					iRet = ERR_PAR;
					goto DEAL4101_END;
				}
			}
			break;

		case 5: // 拍照间隔
			bytInterval = BYTE( v_szData[byt + 2] );
			if( bytInterval > 0x3c ) bytInterval = 0x3c;
			else if( bytInterval < 0x03 ) bytInterval = 0x03;
			break;

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
		case 6: // 图像增强配置
			{
				//亮度（1）＋对比度（1）＋色调（1）＋蓝色饱和度（1）＋红色饱和度（1）
				bytBright = BYTE( v_szData[byt + 2] );
				bytContrast = BYTE( v_szData[byt + 3] );
				bytHue = BYTE( v_szData[byt + 4] );
				bytBlueSaturation = BYTE( v_szData[byt + 5] );
				bytRedSaturation = BYTE( v_szData[byt + 6] );

				if( bytBright > 0x0f ) bytBright = 0x0f;
				else if( bytBright < 1 ) bytBright = 1;
				if( bytContrast > 0x0f ) bytContrast = 0x0f;
				else if( bytContrast < 1 ) bytContrast = 1;
				if( bytHue > 0x0f ) bytHue = 0x0f;
				else if( bytHue < 1 ) bytHue = 1;
				if( bytBlueSaturation > 0x0f ) bytBlueSaturation = 0x0f;
				else if( bytBlueSaturation < 1 ) bytBlueSaturation = 1;
				if( bytRedSaturation > 0x0f ) bytRedSaturation = 0x0f;
				else if( bytRedSaturation < 1 ) bytRedSaturation = 1;
			}
			break;
#endif

		default:
			;
		}

		byt += 2 + bytParLen; // 准备下次循环
	}

	PRTMSG(MSG_NOR, "Rcv Cent Photo Req!\n");

	// 上次的拍照过程、或上传过程可能会出错，
	// 因此若有新的拍照请求，则将拍照状态、上传状态，重传定时全部清零，避免本次再出错
	_KillTimer(&g_objTimerMng, UPLOAD_PHOTOWIN_TIMER );
	memset((void*)&m_objPhotoSta, 0, sizeof(m_objPhotoSta));
	memset((void*)&m_objUploadPhotoSta, 0, sizeof(m_objUploadPhotoSta));

	if( m_objPhotoSta.m_bytPhotoCt < m_objPhotoSta.m_bytPhotoTotal )
	{ /// 若上次的拍照任务没有结束

		// 发送"车台正忙"应答
		bytResType = 0x05;
		iRet = 0;
	}
	else
	{ /// 若不在照相状态

		SendPhotoRequest( bytSizeType, bytQualityType, bytBright, bytContrast, bytHue, bytBlueSaturation,
			bytRedSaturation, bytPhotoTotal, bytChannel, bytInterval, PHOTOEVT_CENT );

		iRet = 0;
	}

DEAL4101_END:
	int iRet2 = g_objSms.MakeSmsFrame((char*)&bytResType, 1, 0x41, 0x41, buf, sizeof(buf), iBufLen);
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9 );

	return iRet;
}

// 设置抓拍参数请求
// 类型个数M（1）+【事件类型ID（1）+参数个数K（1）【+参数类型（1）+参数长度N（1）+参数（N）】× K】×M
int CPhoto::Deal4102(char* v_szData, DWORD v_dwDataLen )
{
	tag2QPhotoCfg objPhotoCfg, objPhotoCfgBkp;
	tag2QPhotoPar* pPhotoPar = NULL;
	int iRet = 0;
	int iBufLen = 0;
	DWORD dw = 0;
	WORD wPhotoType = 0, wPhotoUpload = 0;
	char buf[SOCK_MAXSIZE];
	BYTE bytResType = 2;
	BYTE bytTypeTotal = 0;
	BYTE bytEvtType = 0;
	BYTE bytParTotal = 0;
	BYTE bytParType = 0;
	BYTE bytParLen = 0;
	BYTE bytEvt = 0, bytPar = 0;

	// 输入参数长度检查
	if( v_dwDataLen < 1 )
	{
		iRet = ERR_PAR;
		goto DEAL4102_FAILED;
	}

	// 事件类型个数检查
	bytTypeTotal = v_szData[ 0 ];
	if( 0 == bytTypeTotal )
	{
		iRet = ERR_PAR;
		goto DEAL4102_FAILED;
	}

	// 获取原始配置
	iRet = GetSecCfg( &objPhotoCfg, sizeof(objPhotoCfg), offsetof(tagSecondCfg, m_uni2QPhotoCfg.m_obj2QPhotoCfg), sizeof(objPhotoCfg) );
	if( iRet ) goto DEAL4102_FAILED;
	objPhotoCfgBkp = objPhotoCfg; // 先备份

	// 遍历事件类型个数进行设置
	dw = 1;
	for( bytEvt = 0; bytEvt < bytTypeTotal; bytEvt ++ )
	{
		if( dw + 2 > v_dwDataLen )
		{
			iRet = ERR_PAR;
			goto DEAL4102_FAILED;
		}
		// 参数个数
		bytParTotal = v_szData[ dw + 1 ];

		// 事件类型
		bytEvtType = v_szData[ dw ];

		// 初始化起始位置为参数起始位置
		dw += 2;

		// 根据当前事件类型准备设置
		switch( bytEvtType )
		{
#if USE_PROTOCOL == 30
		case 0x10: // 报警类
#endif

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
		case 0x01: // 报警类
#endif
			wPhotoType = PHOTOEVT_FOOTALERM;// | PHOTOEVT_OVERTURNALERM | PHOTOEVT_BUMPALERM;
			wPhotoUpload = PHOTOUPLOAD_FOOTALERM;// | PHOTOUPLOAD_OVERTURNALERM | PHOTOUPLOAD_BUMPALERM;
			pPhotoPar = &objPhotoCfg.m_objAlermPhotoPar;
			break;

#if USE_PROTOCOL == 30
		case 0x30: // 开车门
#endif

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
		case 0x03: 
#endif
			wPhotoType = PHOTOEVT_OPENDOOR;
			wPhotoUpload = PHOTOUPLOAD_OPENDOOR;
			pPhotoPar = &objPhotoCfg.m_objDoorPhotoPar;
			break;
		
#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
		case 0x02: // 通电 (通电视为ACC点火)
#endif

#if USE_PROTOCOL == 30
		case 0x60: // ACC
#endif
			wPhotoType = PHOTOEVT_ACCON;
			wPhotoUpload = PHOTOUPLOAD_ACCON;
			pPhotoPar = &objPhotoCfg.m_objAccPhotoPar;
			break;


#if USE_PROTOCOL == 30
		case 0x50: // 定时拍照
#endif

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
		case 0x06: // 
#endif
			wPhotoType = PHOTOEVT_TIMER;
			wPhotoUpload = PHOTOUPLOAD_TIMER;
			pPhotoPar = &objPhotoCfg.m_objTimerPhotoPar;
			break;

#if USE_JIJIA == 1
		case 0x04: // 计价器
			wPhotoType = PHOTOEVT_JIJIADOWN;
			wPhotoUpload = PHOTOUPLOAD_JIJIADOWN;
			pPhotoPar = &objPhotoCfg.m_objJijiaPhotoPar;
			break;
#endif

#if USE_PROTOCOL == 30
		case 0x20: // 求助类
			wPhotoType = PHOTOEVT_TRAFFICHELP | PHOTOEVT_MEDICALHELP | PHOTOEVT_BOTHERHELP;
			wPhotoUpload = PHOTOUPLOAD_TRAFFICHELP | PHOTOUPLOAD_MEDICALHELP | PHOTOUPLOAD_BOTHERHELP;
			pPhotoPar = &objPhotoCfg.m_objHelpPhotoPar;
			break;

//		case 0x30: // 开关车门
//			wPhotoType = PHOTOEVT_OPENDOOR;
//			wPhotoUpload = PHOTOUPLOAD_OPENDOOR;
//			pPhotoPar = &objPhotoCfg.m_objDoorPhotoPar;
//			break;

		case 0x40: // 进出区域
			wPhotoType = PHOTOEVT_AREA;
			wPhotoUpload = PHOTOUPLOAD_AREA;
			pPhotoPar = &objPhotoCfg.m_objAreaPhotoPar;
			break;

//		case 0x50: // 定时拍照
//			wPhotoType = PHOTOEVT_TIMER;
//			wPhotoUpload = PHOTOUPLOAD_TIMER;
//			pPhotoPar = &objPhotoCfg.m_objTimerPhotoPar;
//			break;

//		case 0x00: // 通电 (通电视为ACC点火)
//		case 0x60: // ACC
//			wPhotoType = PHOTOEVT_ACCON;
//			wPhotoUpload = PHOTOUPLOAD_ACCON;
//			pPhotoPar = &objPhotoCfg.m_objAccPhotoPar;
//			break;
#endif

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
		case 0x07:// 进出区域
			wPhotoType = PHOTOEVT_AREA;
			wPhotoUpload = PHOTOUPLOAD_AREA;
			pPhotoPar = &objPhotoCfg.m_objAreaPhotoPar;
			break;
		case 0x08:// 超速
			wPhotoType = PHOTOEVT_OVERSPEED;
			wPhotoUpload = PHOTOUPLOAD_OVERSPEED;
			pPhotoPar = &objPhotoCfg.m_objSpeedPhotoPar;
			break;
		case 0x09:// 低速
			wPhotoType = PHOTOEVT_BELOWSPEED;
			wPhotoUpload = PHOTOUPLOAD_BELOWSPEED;
			pPhotoPar = &objPhotoCfg.m_objSpeedPhotoPar;
			break;
#endif

		default:
			iRet = ERR_PAR;
			goto DEAL4102_FAILED;
		}

		// 遍历该事件下的各个参数进行设置
		for( bytPar = 0; bytPar < bytParTotal; bytPar ++ )
		{
			if( dw >= v_dwDataLen )
			{
				iRet = ERR_PAR;
				goto DEAL4102_FAILED;
			}
			
			switch( v_szData[ dw ] ) // 判断参数类型
			{
			case 0x01: // 分辨率
				if( dw + 3 > v_dwDataLen )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}

				// 分辨率参数检查
				if( v_szData[ dw + 2 ] > 3 || v_szData[ dw + 2 ] < 1 ) v_szData[ dw + 2 ] = 1;

				// 记录分辨率
				if( pPhotoPar )
				{
					pPhotoPar->m_bytSizeType = BYTE( v_szData[ dw + 2 ] );
					
					// 分辨率转为内部使用值
					switch( pPhotoPar->m_bytSizeType )
					{
						case 1:
							pPhotoPar->m_bytSizeType = 2;
							break;
						case 2:
							pPhotoPar->m_bytSizeType = 1;
							break;
						default:
							;
					}
				}

				// 准备下一循环
				dw += 3;
				break;

			case 0x02: // 通道号
				if( dw + 3 > v_dwDataLen )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}

				// 记录通道号
				if( pPhotoPar )
				{
					pPhotoPar->m_bytChannel = BYTE( v_szData[dw + 2] ) & 0x0f;
					if( 0 == pPhotoPar->m_bytChannel )
						pPhotoPar->m_bytChannel = 1; // 通道号检查
				}

				// 准备下一循环
				dw += 3;
				break;

			case 0x03: // 照片质量
				if( dw + 3 > v_dwDataLen )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}

				// 质量参数检查
				if( v_szData[ dw + 2 ] > 3 ) v_szData[ dw + 2 ] = 3;
				if( v_szData[ dw + 2 ] < 1 ) v_szData[ dw + 2 ] = 1;

				// 记录质量参数
				if( pPhotoPar )
					pPhotoPar->m_bytQualityType = BYTE( v_szData[ dw + 2 ] );

				// 准备下一循环
				dw += 3;

				break; 
				
			case 0x04: // 单次抓拍张数
				if( dw + 3 > v_dwDataLen )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}

				// 抓拍张数检查
				if( v_szData[ dw + 2 ] > 10 ) v_szData[ dw + 2 ] = 10;
				if( v_szData[ dw + 2 ] < 1 ) v_szData[ dw + 2 ] = 1;

				// 记录抓拍张数
				if( pPhotoPar )
					pPhotoPar->m_bytTime = BYTE( v_szData[ dw + 2 ] );

				// 准备下一循环
				dw += 3;

				break;

#if USE_PROTOCOL == 30
			case 0x05: // 拍照间隔
				if( dw + 3 > v_dwDataLen )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}

				if( BYTE(v_szData[dw + 2]) > 0x3c )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}

				if( pPhotoPar ) pPhotoPar->m_bytInterval = BYTE(v_szData[dw + 2]);
				
				// 准备下一循环
				dw += 3;

				break;
#endif

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
			case 0x05: // 命令字
#endif
#if USE_PROTOCOL == 30
			case 0x07: // 命令字
#endif
				if( dw + 3 > v_dwDataLen )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}

				// 记录事件开关状态
				if( 0x80 & v_szData[ dw + 2 ] )
					objPhotoCfg.m_wPhotoEvtSymb |= wPhotoType;
				else
					objPhotoCfg.m_wPhotoEvtSymb &= ~wPhotoType;

				// 记录主动上报状态
				if( 0x40 & v_szData[ dw + 2 ] )
					objPhotoCfg.m_wPhotoUploadSymb |= wPhotoUpload;
				else
					objPhotoCfg.m_wPhotoUploadSymb &= ~wPhotoUpload;

				// 准备下一循环
				dw += 3;

				break;

#if USE_PROTOCOL == 30
			case 0x06:
				if( 0x30 == bytEvtType ) // 仅对开关车门类型有效
				{
					if( pPhotoPar )
						pPhotoPar->m_bytDoorPar = v_szData[ dw + 2 ];
				}
				// 准备下一循环
				dw += 3;
				break;
#endif

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
			case 0x06: // 增强配置
				if( dw + 7 > v_dwDataLen )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}

				// 亮度（1）＋对比度（1）＋色调（1）＋蓝色饱和度（1）＋红色饱和度（1）
				if( pPhotoPar )
				{
					pPhotoPar->m_bytBright = BYTE( v_szData[dw + 2] );
					if( pPhotoPar->m_bytBright > 0x0f ) pPhotoPar->m_bytBright = 0x0f;
					else if( pPhotoPar->m_bytBright < 1 ) pPhotoPar->m_bytBright = 1;
					pPhotoPar->m_bytContrast = BYTE( v_szData[dw + 3] );
					if( pPhotoPar->m_bytContrast > 0x0f ) pPhotoPar->m_bytContrast = 0x0f;
					else if( pPhotoPar->m_bytContrast < 1 ) pPhotoPar->m_bytContrast = 1;
					pPhotoPar->m_bytHue = BYTE( v_szData[dw + 4] );
					if( pPhotoPar->m_bytHue > 0x0f ) pPhotoPar->m_bytHue = 0x0f;
					else if( pPhotoPar->m_bytHue < 1 ) pPhotoPar->m_bytHue = 1;
					pPhotoPar->m_bytBlueSaturation = BYTE( v_szData[dw + 5] );
					if( pPhotoPar->m_bytBlueSaturation > 0x0f ) pPhotoPar->m_bytBlueSaturation = 0x0f;
					else if( pPhotoPar->m_bytBlueSaturation < 1 ) pPhotoPar->m_bytBlueSaturation = 1;
					pPhotoPar->m_bytRedSaturation = BYTE( v_szData[dw + 6] );
					if( pPhotoPar->m_bytRedSaturation > 0x0f ) pPhotoPar->m_bytRedSaturation = 0x0f;
					else if( pPhotoPar->m_bytRedSaturation < 1 ) pPhotoPar->m_bytRedSaturation = 1;
				}

				// 准备下一循环
				dw += 7;
				break;

           //xun,081014   // 定时拍照，按照许宁修改添加 by tl 081218
			case 0x07: // 定时间隔
				if( dw + 4 > v_dwDataLen )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}
				// 小时（1）+ 分钟（1）
				// 小时取值:0~23,分钟取值:0~59,如果是0,转换为7f (xun,询问洪志新,081014)
				if( 0x7f == v_szData[dw + 2] )
					v_szData[dw + 2] = 0;
				if( 0x7f == v_szData[dw + 3] )
					v_szData[dw + 3] = 0;

				if( pPhotoPar )
				{
					pPhotoPar->m_bytIntervHour = v_szData[dw + 2] % 24;
					pPhotoPar->m_bytIntervMin = v_szData[dw + 3] % 60;
					// 检查设置值,确保不为0
					if( 0 == pPhotoPar->m_bytIntervHour && 0 == pPhotoPar->m_bytIntervMin )
					{
	//					pPhotoPar->m_bytIntervMin = 1;
    //    这样可能会有隐患，因为如果参数错误的话，1分钟上一个照片，可能会产生大量的流量。。。。
						pPhotoPar->m_bytIntervMin = 10;
					}
				}			
				// 准备下一循环
				dw += 4;
				break;

#endif

			default:
				if( dw + 2 > v_dwDataLen )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}

				bytParLen = v_szData[ dw + 1 ];
				if( dw + 2 + bytParLen > v_dwDataLen )
				{
					iRet = ERR_PAR;
					goto DEAL4102_FAILED;
				}

				// 准备下一循环
				dw += 2 + bytParLen;
			}
		}
	}

	// 组帧,发送
	bytResType = 1;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x41, 0x42, buf, sizeof(buf), iBufLen );
	if( !iRet )
	{
		// 保存新设置
		if( iRet = SetSecCfg(&objPhotoCfg, offsetof(tagSecondCfg, m_uni2QPhotoCfg.m_obj2QPhotoCfg),
			sizeof(objPhotoCfg)) )
		{
			goto DEAL4102_FAILED;
		}
		else
		{
			if( objPhotoCfg.m_wPhotoEvtSymb & PHOTOEVT_TIMER )
			{
				P_TmPhotoTimer(); // 立刻定时拍照,函数内将开启定时器
			}
			else
			{
				_KillTimer(&g_objTimerMng, TIMERPHOTO_TIMER );
			}
		}

		// 发送应答
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12 );
		if( iRet )
		{
			SetSecCfg( &objPhotoCfgBkp, offsetof(tagSecondCfg, m_uni2QPhotoCfg.m_obj2QPhotoCfg), sizeof(objPhotoCfgBkp) ); // 恢复原设置
			goto DEAL4102_FAILED;
		}
	}
	else 
		goto DEAL4102_FAILED;

	return 0;

DEAL4102_FAILED:
	bytResType = 3;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x41, 0x42, buf, sizeof(buf), iBufLen );
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12 );

	return iRet;
}

/// 通过中心报表查询图片黑匣子请求 (应答成功优先)
// 查询个数（1）+【索引号1（2） + …… + 索引号n（2）】
int CPhoto::Deal4103( char* v_szData, DWORD v_dwDataLen )
{
	tagPhotoInfo aryPhotoInfo[ UPLOADPHOTO_MAXCOUNT ];
	int iRet = 0;
	int iBufLen;
	char buf[ SOCK_MAXSIZE ];
	WORD *aryIndex = NULL;
	WORD wPhotoIndex = 0;
	BYTE bytQueryTotal = 0;
	BYTE bytResType = 5;
	BYTE byt = 0;
	bool bInitUpload = true;

	if( m_objPhotoSta.m_bytPhotoCt < m_objPhotoSta.m_bytPhotoTotal // 若拍照未结束
		|| // 或
		(
			m_objUploadPhotoSta.m_bytAllPhotoCt < m_objUploadPhotoSta.m_bytAllPhotoTotal // 照片上传未结束
			&&
			!m_objUploadPhotoSta.m_bUploadBreak // 且上传未超时中止
		)
	  )
	{
		bytResType = 0x03; // 车台正忙
		iRet = 0;
		goto DEAL4103_FAILED;
	}

	// 参数检查
	if( v_dwDataLen < 1 )
	{
		bytResType = 4; // 未知原因
		iRet = ERR_PAR;
		goto DEAL4103_FAILED;
	}

	// 索引总数
	bytQueryTotal = BYTE( v_szData[0] );
	if( 0 == bytQueryTotal || bytQueryTotal > 20 // 若超过最大索引个数
		|| bytQueryTotal > UPLOADPHOTO_MAXCOUNT ) // 或超过内存缓存尺寸
	{
		bytResType = 2; // 查询索引个数过大
		iRet = ERR_PAR;
		goto DEAL4103_FAILED;
	}

	// 参数再检查
	if( 1 + DWORD(bytQueryTotal * 2) > v_dwDataLen )
	{
		bytResType = 4; // 未知原因
		iRet = ERR_PAR;
		goto DEAL4103_FAILED;
	}

	// 提取要上传的照片索引
	memset( aryPhotoInfo, 0, sizeof(aryPhotoInfo) );
	for( byt = 0; byt < bytQueryTotal; byt ++ )
	{
		// 提取照片索引
		if( 0x7f == v_szData[ 1 + 2 * byt ] )
			v_szData[ 1 + 2 * byt ] = 0;
		if( 0x7f == v_szData[ 1 + 2 * byt + 1 ])
			v_szData[ 1 + 2 * byt + 1 ] = 0;

		memcpy( &wPhotoIndex, v_szData + 1 + 2 * byt, 2 );
		
		if( byt < sizeof(aryPhotoInfo) / sizeof(aryPhotoInfo[0]) ) // 保险判断
			aryPhotoInfo[ byt ].m_wPhotoIndex = htons( wPhotoIndex ); // 从网络字节序恢复
		else
			break;
	}
	bytQueryTotal = byt; // 保险修正

	// 上传应答
	bytResType = 1; // 成功
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x41, 0x43, buf, sizeof(buf), iBufLen );
	if( !iRet )
	{
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12 );
		if( !iRet )
		{
			// 增加到要上传照片的队列中
			P_AddUploadPhoto( aryPhotoInfo, bytQueryTotal );
		}
		else
		{
			bytResType = 4; // 未知原因
			goto DEAL4103_FAILED;
		}
	}
	else
	{
		bytResType = 4; // 未知原因
		goto DEAL4103_FAILED;
	}

	return 0;

DEAL4103_FAILED:
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x41, 0x43, buf, sizeof(buf), iBufLen );
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9 );

	return iRet;
}

/// 通过事件和时间段查询图片黑匣子请求 (应答成功优先)
// 查询时间段（12）+ 索引个数（1）+ 事件条件个数(1) +【事件类型1（1）+ … + 事件类型n（1）】
int CPhoto::Deal4104( char* v_szData, DWORD v_dwDataLen )
{
	int iRet = 0, n;
	int iBufLen;
	char buf[ SOCK_MAXSIZE ];
	char sz4144[ 300 ] = { 0 };
	char szPhotoDir[255];
	struct dirent **namelist;
	tag4144PhotoIndex* aryPhotoIndex = NULL;
	DWORD dwBeginDate, dwBeginTime, dwEndDate, dwEndTime, dwCurDate;
	DWORD dwPhotoDate, wPhotoTime;
	WORD wPhotoIndex, wPhotoType, wReqPhotoType;
	BYTE bytFindCount = 0;
	BYTE bytEvtTotal = 0, bytIdxTotal = 0;
	BYTE bytResType = 4;
	BYTE byt = 0;
	BYTE bytYear, bytMon, bytDay;
	
	// 现在是否正在拍照
	if( m_objPhotoSta.m_bytPhotoCt < m_objPhotoSta.m_bytPhotoTotal )
	{
		iRet = 0;
		bytResType = 3; // 车台忙
		goto DEAL4104_END;
	}

	// 输入参数长度检查
	if( v_dwDataLen < 14 )
	{
		iRet = ERR_PAR;
		bytResType = 4; // 未知原因
		goto DEAL4104_END;
	}

	// 时间段转义
	if( 0x7f == v_szData[0] ) v_szData[0] = 0;
	if( 0x7f == v_szData[3] ) v_szData[3] = 0;
	if( 0x7f == v_szData[4] ) v_szData[4] = 0;
	if( 0x7f == v_szData[5] ) v_szData[5] = 0;
	if( 0x7f == v_szData[6] ) v_szData[6] = 0;
	if( 0x7f == v_szData[9] ) v_szData[9] = 0;
	if( 0x7f == v_szData[10] ) v_szData[10] = 0;
	if( 0x7f == v_szData[11] ) v_szData[11] = 0;

	// 时间段参数检验
	if( v_szData[1] < 1 || v_szData[1] > 12 || v_szData[2] < 1 || v_szData[2] > 31 // 起始月日
		|| v_szData[3] > 23 || v_szData[4] > 59 || v_szData[5] > 59 // 起始时分秒
		|| v_szData[7] < 1 || v_szData[7] > 12 || v_szData[8] < 1 || v_szData[8] > 31 // 终止月日
		|| v_szData[9] > 23 || v_szData[10] > 59 || v_szData[11] > 59 ) // 终止时分秒
	{
		iRet = ERR_PAR;
		bytResType = 4; // 未知原因
		goto DEAL4104_END;
	}

	// 生成时间段数字参数
	dwBeginDate = BYTE(v_szData[0]) * DWORD(10000) + BYTE(v_szData[1]) * DWORD(100) + BYTE(v_szData[2]);
	dwBeginTime = BYTE(v_szData[3]) * DWORD(10000) + BYTE(v_szData[4]) * DWORD(100) + BYTE(v_szData[5]);
	dwEndDate = BYTE(v_szData[6]) * DWORD(10000) + BYTE(v_szData[7]) * DWORD(100) + BYTE(v_szData[8]);
	dwEndTime = BYTE(v_szData[9]) * DWORD(10000) + BYTE(v_szData[10]) * DWORD(100) + BYTE(v_szData[11]);

	// 时间段参数合法性检验
	if( dwEndDate < dwBeginDate || (dwEndDate == dwBeginDate && dwEndTime < dwBeginTime) )
	{
		iRet = ERR_PAR;
		bytResType = 4; // 未知原因
		goto DEAL4104_END;
	}

	// 请求的图像索引个数
	bytIdxTotal = BYTE( v_szData[12] );
	if( 0x7f == bytIdxTotal ) bytIdxTotal = 0;

	// 图像索引个数检查
	if( 0 == bytIdxTotal || bytIdxTotal > 20 )
	{
		iRet = ERR_PAR;
		bytResType = 4; // 未知原因
		goto DEAL4104_END;
	}

	// 分配将要保存的索引数组
	aryPhotoIndex = new tag4144PhotoIndex[ bytIdxTotal ];
	if( !aryPhotoIndex )
	{
		iRet = ERR_MEMLACK;
		bytResType = 4; // 未知原因
		goto DEAL4104_END;
	}

	// 初始化找到的索引个数
	bytFindCount = 0;

	// 事件类型个数
	bytEvtTotal = BYTE( v_szData[13] );
	if( 0x7f == bytEvtTotal ) bytEvtTotal = 0; // 反转义
	if( bytEvtTotal > 10 ) // 拍照类型最多10个
	{
		iRet = ERR_PAR;
		bytResType = 4;
		goto DEAL4104_END;
	}

	// 事件类型->照片类型
	if( v_dwDataLen < WORD(14 + bytEvtTotal) )
	{
		iRet = ERR_MEMLACK;
		bytResType = 4;
		goto DEAL4104_END;
	}
	if( bytEvtTotal > 0 )
	{
		wReqPhotoType = 0;
		for( byt = 0; byt < bytEvtTotal; byt ++ )
		{
			if( 0x7f == v_szData[14 + byt] ) v_szData[14 + byt] = 0;
			wReqPhotoType |= _EvtTypeToPhotoType( BYTE(v_szData[14 + byt]) );

#if USE_PROTOCOL == 30
			// KJ2车台,协议里中心抓拍和通电的拍照类型一样,而现在把通电拍照等同于ACC点火拍照.
			if( 0 == v_szData[14 + byt] ) wReqPhotoType |= PHOTOEVT_ACCON;
#endif
		}
	}
	else
	{
		wReqPhotoType = 0xffff;
	}

	// 根据起止时间段,搜寻照片
	bytFindCount = 0; // 初始化找到的照片计数

	for( dwCurDate = dwBeginDate; dwCurDate <= dwEndDate; )
	{
		IN_MakePhotoDirName( dwCurDate, szPhotoDir );

		n = scandir(szPhotoDir, &namelist, 0, alphasort);
		if (n < 0)
			perror("scandir");
		else 
		{
			while(n--) 
			{
				if( 0 == strcmp(".", namelist[n]->d_name) || 0 == strcmp("..", namelist[n]->d_name) )
					continue;

				// 从文件名获得照片日期时间
				IN_GetDateTimeFromPhotoName( namelist[n]->d_name, dwPhotoDate, wPhotoTime );
				
				// 早于起始时间则跳过
				if( dwPhotoDate < dwBeginDate || (dwPhotoDate == dwBeginDate && wPhotoTime < dwBeginTime) )
					continue;
				
				// 迟于终止时间,跳过
				if( dwPhotoDate > dwEndDate || (dwPhotoDate == dwEndDate && wPhotoTime > dwEndTime) )
					continue;
				
				// 从文件名获得照片类型
				IN_GetTypeFromPhotoName( namelist[n]->d_name, wPhotoType );
				
				// 不属于需要的类型,跳过
				if( bytEvtTotal > 0 )
				{
					if( 0 == (wReqPhotoType & wPhotoType) ) continue;
				}
				
				// 从文件名获得照片索引
				IN_GetIndexFromPhotoName( namelist[n]->d_name, wPhotoIndex );
				
				// 加入到找到的索引中
				aryPhotoIndex[ bytFindCount ].m_bytEvtType = _PhotoTypeToEvtType( wPhotoType );
				wPhotoIndex = htons( wPhotoIndex );
				memcpy( &aryPhotoIndex[ bytFindCount ].m_wIndex, &wPhotoIndex, sizeof(aryPhotoIndex[ 0 ].m_wIndex) );
				aryPhotoIndex[ bytFindCount ].m_szDateTime[0] = char( dwPhotoDate / 10000 );
				aryPhotoIndex[ bytFindCount ].m_szDateTime[1] = char( dwPhotoDate % 10000 / 100 );
				aryPhotoIndex[ bytFindCount ].m_szDateTime[2] = char( dwPhotoDate % 100 );
				aryPhotoIndex[ bytFindCount ].m_szDateTime[3] = char( wPhotoTime / 10000 );
				aryPhotoIndex[ bytFindCount ].m_szDateTime[4] = char( wPhotoTime % 10000 / 100 );
				aryPhotoIndex[ bytFindCount ].m_szDateTime[5] = char( wPhotoTime % 100 );
				
				// 若已找到足够的图片,跳出
				if( ++ bytFindCount >= bytIdxTotal )
					goto _FINDPHOTO_END;
			}
		}

		// 生成下次日期
		bytYear = BYTE(dwCurDate / 10000), bytMon = BYTE(dwCurDate % 10000 / 100), bytDay = BYTE(dwCurDate % 100);
		bytDay ++;
		switch( bytMon )
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if( bytDay > 31 )
			{
				bytDay = 1;
				bytMon ++;
			}
			break;

		case 4:
		case 6:
		case 9:
		case 11:
			if( bytDay > 30 )
			{
				bytDay = 1;
				bytMon ++;
			}
			break;

		case 2:
			if( 0 == bytYear % 4 )
			{
				if( bytDay > 29 )
				{
					bytDay = 1;
					bytMon ++;
				}
			}
			else
			{
				if( bytDay > 28 )
				{
					bytDay = 1;
					bytMon ++;
				}
			}
			break;

		default:
			iRet = ERR_PAR;
			bytResType = 4;
			goto DEAL4104_END;
		}

		if( bytMon > 12 )
		{
			bytMon = 1;
			bytYear ++;
		}

		dwCurDate = bytYear * 10000 + bytMon * 100 + bytDay;
	}

_FINDPHOTO_END:
	if( 0 == bytFindCount )
	{
		iRet = 0;
		bytResType = 2;
		goto DEAL4104_END;
	}

	if( 2 + bytFindCount * 9 > sizeof(sz4144) )
	{
		iRet = ERR_SPACELACK;
		bytResType = 4;
		goto DEAL4104_END;
	}

	bytResType = 1;
	iRet = 0;

DEAL4104_END:
	if( 1 == bytResType )
	{
		// 应答类型（1）【+索引个数（1）+【事件类型（1）+索引号（2）+起始时间（6）】* N】
		sz4144[ 0 ] = 1;
		sz4144[ 1 ] = char( bytFindCount );
		memcpy( sz4144 + 2, aryPhotoIndex, bytFindCount * sizeof(aryPhotoIndex[0]) );
		iRet = g_objSms.MakeSmsFrame(sz4144, 2 + bytFindCount * sizeof(aryPhotoIndex[0]), 0x41, 0x44, buf, sizeof(buf), iBufLen);
		if( !iRet ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9 );
	}
	else
	{
		int iRet2 = g_objSms.MakeSmsFrame((char*)&bytResType, 1, 0x41, 0x44, buf, sizeof(buf), iBufLen);
		if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9 );
	}

	free(namelist);
	if( aryPhotoIndex ) delete[] aryPhotoIndex;

	return iRet;
}

/// 照片传输帧应答
// 照片索引(2) + 包序号(2) + 应答类型（1）【+ 重传包个数（1）+ [包序号1（2） +包序号2（2）+ …… +包序号n（2）] 】
int CPhoto::Deal4110(char* v_szData, DWORD v_dwDataLen, bool v_bFrom4112 /*=false*/ )
{
	PRTMSG(MSG_NOR, " Rcv Cent 4110 Res\n" );

	WORD wResPhotoIndex = 0;
	WORD wResFrameNo = 0;

	if( v_dwDataLen < 5 ) return ERR_PAR;
	
	if( 0x7f == v_szData[0] ) v_szData[0] = 0;
	if( 0x7f == v_szData[1] ) v_szData[1] = 0;
	if( 0x7f == v_szData[2] ) v_szData[2] = 0;
	if( 0x7f == v_szData[3] ) v_szData[3] = 0;

	wResPhotoIndex = BYTE(v_szData[0]) * WORD(0x100) + BYTE( v_szData[1] ); // 照片索引
	wResFrameNo = BYTE(v_szData[2]) * WORD(0x100) + BYTE( v_szData[3] ); // 包序号

	// 检查当前状态是否正确
	if( m_objUploadPhotoSta.m_bytAllPhotoCt >= m_objUploadPhotoSta.m_bytAllPhotoTotal
		||
		m_objUploadPhotoSta.m_bytAllPhotoCt >= sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo) / sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) )
	{
		m_objUploadPhotoSta.m_bytAllPhotoCt = m_objUploadPhotoSta.m_bytAllPhotoTotal = 0;
		_ClearUploadPhotoStaForNewPhoto();
		PRTMSG(MSG_ERR, "Deal 4110, But No Photo Is Uploading, Exit!\n" );
		return ERR_PAR;
	}

	// 检查应答照片和当前上传的照片是否相同
	if( wResPhotoIndex != m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex )
	{
		PRTMSG(MSG_NOR, "Deal 4110, But the photo in res isn't the Uploading photo,Exit!\n", wResPhotoIndex );
		return ERR_PAR;
	}

	if( wResFrameNo / UPLOADPHOTO_WINFRAMETOTAL != m_objUploadPhotoSta.m_wPhotoWinCt ) // 若应答窗口和状态记录中的不同
	{
		PRTMSG(MSG_NOR, "Deal 4110, But the win(last packet %d) isn't the Uploading win,Exit!\n", wResFrameNo );
		return ERR_PAR;
	}

	int iRet = 0;

	if( v_bFrom4112 )
	{
		if( !m_objUploadPhotoSta.m_byt4152Snd ) return 0;
	}

	if( 1 == v_szData[4] ) // 该窗口成功确认
	{
		PRTMSG(MSG_NOR, "The No. %d Win Uploaded Succ!\n", m_objUploadPhotoSta.m_wPhotoWinCt );

		// 停止重传定时
		_KillTimer(&g_objTimerMng, UPLOAD_PHOTOWIN_TIMER );

		// 递增完成窗口计数
		++ m_objUploadPhotoSta.m_wPhotoWinCt;

		// 为后面的新窗口上传做清除工作
		_ClearUploadPhotoStaForNewWin();

		if( DWORD(m_objUploadPhotoSta.m_wPhotoWinCt) * UPLOADPHOTO_WINFRAMETOTAL
			>= DWORD(m_objUploadPhotoSta.m_wPhotoFrameTotal) ) // 若本窗口是最后一个窗口
		{
			// 记录最新上传成功的照片索引
			m_objUploadPhotoSta.m_wLstSuccUpPhotoIndex =
				m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex;

			// 递增上传照片计数
			++ m_objUploadPhotoSta.m_bytAllPhotoCt;
			
			// 判断是否全部照片上传结束
			if( m_objUploadPhotoSta.m_bytAllPhotoCt >= m_objUploadPhotoSta.m_bytAllPhotoTotal )
			{
				_StopUploadPhoto();
				m_objUploadPhotoSta.m_bytAllPhotoCt = m_objUploadPhotoSta.m_bytAllPhotoTotal = 0; // 清除上传照片计数和总数
			}
			else
			{
				// 继续上传下一照片
				PRTMSG(MSG_NOR, "Leave %d Photos To Upload,Next Photo Index:%d\n",
					m_objUploadPhotoSta.m_bytAllPhotoTotal - m_objUploadPhotoSta.m_bytAllPhotoCt,
					m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex );
				
				_ClearUploadPhotoStaForNewPhoto(); // 为新照片上传清除上传照片状态
				_UploadOnePhotoWin( m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex,
					false, 0, 0, NULL );
			}
		}
		else
		{
			// 上传下一窗口
			PRTMSG(MSG_NOR, "Continue Upload Next Win\n" );
			_ClearUploadPhotoStaForNewWin();
			_UploadOnePhotoWin( m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex,
				false, m_objUploadPhotoSta.m_wPhotoWinCt * UPLOADPHOTO_WINFRAMETOTAL, 0, NULL );
		}
	}
	else // 即若该窗口有未成功的包
	{
// 		if( 0 == m_objUploadPhotoSta.m_bytAllPhotoTotal )
// 		{
// 			PRTMSG(MSG_NOR, "中心请求重传,但车台未发现有要上传的照片,不处理!" );
// 			return ERR_PAR;
// 		}

		if( v_dwDataLen < 7 ) return ERR_PAR;

		if( 0x7f == v_szData[5] ) v_szData[5] = 0;
		BYTE bytFrameTotal = BYTE( v_szData[5] ); // 重传包个数

		if( 0 == bytFrameTotal ) return ERR_PAR;

		if( WORD(bytFrameTotal) != m_objUploadPhotoSta.m_wPhotoFrameTotal ) // 若不是全部重传
		{
			if( 0 == bytFrameTotal ||
				(bytFrameTotal > UPLOADPHOTO_WINFRAMETOTAL && WORD(bytFrameTotal) != m_objUploadPhotoSta.m_wPhotoFrameTotal ) )
				return ERR_PAR;
			if( 6 + 2 * DWORD(bytFrameTotal) != v_dwDataLen ) return ERR_PAR;

			DWORD dw = 6;
			BYTE byt = 0;
			WORD aryFrameNo[ UPLOADPHOTO_WINFRAMETOTAL ];
			for( byt = 0; byt < bytFrameTotal; byt ++ )
			{
				if( 0x7f == v_szData[ dw ] ) v_szData[ dw ] = 0;
				if( 0x7f == v_szData[ dw + 1 ] ) v_szData[ dw + 1 ] = 0;

				aryFrameNo[ byt ] = BYTE(v_szData[ dw ]) * WORD(0x100) + BYTE(v_szData[ dw + 1 ]);
				dw += 2;
			}

			// 包序号排序
			if( bytFrameTotal > 1 ) 
				qsort( aryFrameNo, bytFrameTotal, sizeof(aryFrameNo[0]), SortWord );

			// 检查包序号合法性
			if( aryFrameNo[0] / UPLOADPHOTO_WINFRAMETOTAL != aryFrameNo[ byt - 1] / UPLOADPHOTO_WINFRAMETOTAL // 若传入的包序号不属于同一窗口
				|| aryFrameNo[0] / UPLOADPHOTO_WINFRAMETOTAL != m_objUploadPhotoSta.m_wPhotoWinCt ) // 或传入的包序号与状态记录中不属于同一窗口
			{
				PRTMSG(MSG_NOR, "Deal 4110, fail_1\n" );
				return ERR_PAR;
			}
			
			// 停止重传定时
			_KillTimer(&g_objTimerMng, UPLOAD_PHOTOWIN_TIMER );

			// 重传需要的包
			_UploadOnePhotoWin( m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex,
				true, 0, bytFrameTotal, aryFrameNo );
		}
		else
		{
			// 停止重传定时
			_KillTimer(&g_objTimerMng, UPLOAD_PHOTOWIN_TIMER );
			
			PRTMSG(MSG_NOR, "Cent Req Upload All Packets of the Photo\n" );

			// 重传第一个窗口的包
			_UploadOnePhotoWin( m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex,
				false, 0, 0, NULL );
		}
	}
	
	return iRet;
}

/// 照片索引(2) + 包序号（2）＋应答类型（1）【+ 重传包个数（1）+ [包序号1（2） +包序号2（2）+ …… +包序号n（2）] 】
int CPhoto::Deal4112( char* v_szData, DWORD v_dwDataLen )
{
	PRTMSG(MSG_NOR, "Rcv Cent 4112 res!\n" );

	return Deal4110( v_szData, v_dwDataLen, true );
}

// 请求断点续传图片: 图像索引（2）+ 重传包个数（1）+[包序号1（2） +包序号2（2）+ …… +包序号n（2）]
// 可能是网络断了重连后,也可能是系统复位后,所以不能再以内存中的状态来判断请求是否有效
int CPhoto::Deal4113(char* v_szData, DWORD v_dwDataLen )
{
	PRTMSG(MSG_NOR, "Rcv Cent 4113 res!\n" );

	int iRet = 0;
	WORD wResPhotoIndex;

	// 参数长度检查
	if( v_dwDataLen < 3 ) return ERR_PAR;
	
	if( 0x7f == v_szData[0] ) v_szData[0] = 0;
	if( 0x7f == v_szData[1] ) v_szData[1] = 0;
	wResPhotoIndex = BYTE(v_szData[0]) * WORD(0x100) + BYTE( v_szData[1] ); // 照片索引

	// 若该照片不是当前待判断是否上传成功的照片
	if( m_objUploadPhotoSta.m_bytAllPhotoCt < m_objUploadPhotoSta.m_bytAllPhotoTotal
		&& m_objUploadPhotoSta.m_bytAllPhotoCt < UPLOADPHOTO_MAXCOUNT
		&& (wResPhotoIndex != m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex)
		)
	{
		// 要继续判断是否还有照片要上传

		// 停止重传定时
		_KillTimer(&g_objTimerMng, UPLOAD_PHOTOWIN_TIMER );

		// 记录最新上传成功的照片索引
		m_objUploadPhotoSta.m_wLstSuccUpPhotoIndex =
			m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex;
		
		// 递增上传照片计数
		++ m_objUploadPhotoSta.m_bytAllPhotoCt;
		
		// 判断是否全部照片上传结束
		if( m_objUploadPhotoSta.m_bytAllPhotoCt >= m_objUploadPhotoSta.m_bytAllPhotoTotal )
		{
			_StopUploadPhoto();
			m_objUploadPhotoSta.m_bytAllPhotoCt = m_objUploadPhotoSta.m_bytAllPhotoTotal = 0; // 清除上传照片计数和总数
		}
		else
		{
			// 继续上传下一照片
			PRTMSG(MSG_NOR, "Leave %d Photos To Upload,Next Photo Index:%d\n",
				m_objUploadPhotoSta.m_bytAllPhotoTotal - m_objUploadPhotoSta.m_bytAllPhotoCt,
				m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex );
			
			_ClearUploadPhotoStaForNewPhoto(); // 为新照片上传清除上传照片状态
			_UploadOnePhotoWin( m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex,
				false, 0, 0, NULL );
		}

		return 0;
	}

	{
		// 重传包个数
		BYTE bytFrameTotal = BYTE( v_szData[2] );
		if( 0 == bytFrameTotal || bytFrameTotal > UPLOADPHOTO_WINFRAMETOTAL ) return ERR_PAR;

		// 参数长度检查
		if( 3 + 2 * DWORD(bytFrameTotal) != v_dwDataLen ) return ERR_PAR;

		// 获取包序号
		DWORD dw = 3;
		BYTE byt = 0;
		WORD aryFrameNo[ UPLOADPHOTO_WINFRAMETOTAL ];
		for( byt = 0; byt < bytFrameTotal; byt ++ )
		{
			if( 0x7f == v_szData[ dw ] ) v_szData[ dw ] = 0;
			if( 0x7f == v_szData[ dw + 1 ] ) v_szData[ dw + 1 ] = 0;

			aryFrameNo[ byt ] = BYTE(v_szData[ dw ]) * WORD(0x100) + BYTE(v_szData[ dw + 1 ]);
			dw += 2;
		}

		// 包序号排序
		if( bytFrameTotal >= 2 ) 
			qsort( aryFrameNo, bytFrameTotal, sizeof(aryFrameNo[0]), SortWord );

		// 检查包序号合法性
		if( aryFrameNo[0] / UPLOADPHOTO_WINFRAMETOTAL != aryFrameNo[ byt - 1 ] / UPLOADPHOTO_WINFRAMETOTAL ) // 若传入的包序号不属于同一窗口
			return ERR_PAR;

		// 若上传曾经中断过且未曾恢复
		if( m_objUploadPhotoSta.m_bUploadBreak )
		{
			_ClearUploadPhotoStaForNewPhoto();
			m_objUploadPhotoSta.m_bytAllPhotoCt ++; // 跳过该照片上传
			if( m_objUploadPhotoSta.m_bytAllPhotoCt >= m_objUploadPhotoSta.m_bytAllPhotoTotal )
				m_objUploadPhotoSta.m_bytAllPhotoCt = m_objUploadPhotoSta.m_bytAllPhotoTotal = 0; // 清除上传照片计数和总数
		}
		
		/// 更新上传照片状态
		if( m_objUploadPhotoSta.m_bytAllPhotoCt < m_objUploadPhotoSta.m_bytAllPhotoTotal ) // 若此时已有要上传的照片
		{
			// 缓存原数组
			tagPhotoInfo aryTempIndex[ UPLOADPHOTO_MAXCOUNT ];
			memcpy( aryTempIndex, m_objUploadPhotoSta.m_aryUpPhotoInfo,
			 UPLOADPHOTO_MAXCOUNT * sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) );
			 
			// 重传照片信息发在数组第一个位置
			memset( &(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]), 0, sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) );
			m_objUploadPhotoSta.m_aryUpPhotoInfo[ 0 ].m_wPhotoIndex = wResPhotoIndex;
			
			// 复制数组中其他未完成的照片
			for( byt = 1;
				byt < sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo) / sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0])
				&& m_objUploadPhotoSta.m_bytAllPhotoCt < m_objUploadPhotoSta.m_bytAllPhotoTotal;
				 m_objUploadPhotoSta.m_bytAllPhotoCt ++ )
			{
				if( wResPhotoIndex != aryTempIndex[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex )
				{
					m_objUploadPhotoSta.m_aryUpPhotoInfo[ byt ] = aryTempIndex[ m_objUploadPhotoSta.m_bytAllPhotoCt ];
					byt ++;
				}
			}
			
			// 重设总数和已发计数
			m_objUploadPhotoSta.m_bytAllPhotoTotal = byt;
			m_objUploadPhotoSta.m_bytAllPhotoCt = 0;
		}
		else
		{
			memset( &(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]), 0, sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) );
			m_objUploadPhotoSta.m_aryUpPhotoInfo[ 0 ].m_wPhotoIndex = wResPhotoIndex;
			m_objUploadPhotoSta.m_bytAllPhotoTotal = 1;
			m_objUploadPhotoSta.m_bytAllPhotoCt = 0;
		}
		
		// 重传需要的包
		_ClearUploadPhotoStaForNewPhoto();
		_UploadOnePhotoWin( wResPhotoIndex, true, 0, bytFrameTotal, aryFrameNo );
	}
	
	return iRet;
}

void CPhoto::AddPhoto( WORD v_wPhotoType )
{
	// 读取配置
	tag2QPhotoCfg objPhotoCfg;
	objPhotoCfg.Init();
	GetSecCfg( &objPhotoCfg, sizeof(objPhotoCfg), offsetof(tagSecondCfg, m_uni2QPhotoCfg.m_obj2QPhotoCfg), sizeof(objPhotoCfg) );

	if( !(objPhotoCfg.m_wPhotoEvtSymb & v_wPhotoType) )
		return;

	tag2QPhotoPar objPhotoPar;
	if( v_wPhotoType & PHOTOEVT_FOOTALERM || v_wPhotoType & PHOTOEVT_FOOTALERM_BIG )
	{
		objPhotoPar = objPhotoCfg.m_objAlermPhotoPar;
	}
#if USE_PROTOCOL == 30
	else if( v_wPhotoType & (PHOTOEVT_OVERTURNALERM | PHOTOEVT_BUMPALERM | PHOTOEVT_DRIVEALERM) )
	{
		objPhotoPar = objPhotoCfg.m_objAlermPhotoPar;
	}
	else if( v_wPhotoType & PHOTOEVT_OPENDOOR )
	{
		objPhotoPar = objPhotoCfg.m_objDoorPhotoPar;
	}
#endif
#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
	else if( v_wPhotoType & PHOTOEVT_JIJIADOWN )
	{
		objPhotoPar = objPhotoCfg.m_objJijiaPhotoPar;
	}
	else if( v_wPhotoType & PHOTOEVT_TIMER )
	{
		objPhotoPar = objPhotoCfg.m_objTimerPhotoPar;
	}
	else if( v_wPhotoType & PHOTOEVT_ACCON )
	{
		objPhotoPar = objPhotoCfg.m_objAccPhotoPar;
	}
	else if( v_wPhotoType & PHOTOEVT_OPENDOOR )
	{
		objPhotoPar = objPhotoCfg.m_objDoorPhotoPar;
	}
	else if( v_wPhotoType & PHOTOEVT_AREA )
	{
		objPhotoPar = objPhotoCfg.m_objAreaPhotoPar;
	}
	else if( v_wPhotoType & PHOTOEVT_OVERSPEED || v_wPhotoType & PHOTOEVT_BELOWSPEED )
	{
		objPhotoPar = objPhotoCfg.m_objSpeedPhotoPar;
	}
#endif
#if USE_PROTOCOL == 30
	else if( v_wPhotoType & (PHOTOEVT_TRAFFICHELP | PHOTOEVT_MEDICALHELP | PHOTOEVT_BOTHERHELP) )
	{
		objPhotoPar = objPhotoCfg.m_objHelpPhotoPar;
	}
	else if( v_wPhotoType & PHOTOEVT_AREA )
	{
		objPhotoPar = objPhotoCfg.m_objAreaPhotoPar;
	}
	else if( v_wPhotoType & PHOTOEVT_OPENDOOR )
	{
		objPhotoPar = objPhotoCfg.m_objDoorPhotoPar;
	}
	else if( v_wPhotoType & PHOTOEVT_TIMER )
	{
		objPhotoPar = objPhotoCfg.m_objTimerPhotoPar;
	}
	else if( v_wPhotoType & PHOTOEVT_ACCON )
	{
		objPhotoPar = objPhotoCfg.m_objAccPhotoPar;
	}
#endif
	else return;

	BYTE bytPhotoTime = objPhotoPar.m_bytTime;
	BYTE bytPhotoSizeType = objPhotoPar.m_bytSizeType;
	BYTE bytPhotoQualityType = objPhotoPar.m_bytQualityType;
	BYTE bytBright = objPhotoPar.m_bytBright;
	BYTE bytContrast = objPhotoPar.m_bytContrast;
	BYTE bytHue = objPhotoPar.m_bytHue;
	BYTE bytBlueSaturation = objPhotoPar.m_bytBlueSaturation;
	BYTE bytRedSaturation = objPhotoPar.m_bytRedSaturation;
	BYTE bytChannel = objPhotoPar.m_bytChannel;
	BYTE bytInterval = objPhotoPar.m_bytInterval;
	if( 0 == bytPhotoTime ) bytPhotoTime = 1;
	else if( bytPhotoTime > PHOTOQUEE_MAXCOUNT ) bytPhotoTime = PHOTOQUEE_MAXCOUNT;

	// 若是报警拍照的不上传的中等尺寸照片，特殊处理
	if( v_wPhotoType & PHOTOEVT_FOOTALERM_BIG )
	{
		bytPhotoTime = 1;
		if( bytPhotoSizeType < 2 ) bytPhotoSizeType = 2;
		if( bytPhotoQualityType > 2 || 0 == bytPhotoQualityType ) bytPhotoQualityType = 2;
		bytChannel |= 3;
	}

	// 更新拍照的张数
	if( m_objPhotoSta.m_bytPhotoTotal < PHOTOQUEE_MAXCOUNT )
	{
		// 修正本次增加拍照的张数
		bytPhotoTime = min(bytPhotoTime, PHOTOQUEE_MAXCOUNT - m_objPhotoSta.m_bytPhotoTotal);
		
		// 更新总张数
		m_objPhotoSta.m_bytPhotoTotal += bytPhotoTime;
	}
	else
	{
		return;
	}

	SendPhotoRequest(bytPhotoSizeType, bytPhotoQualityType, bytBright, bytContrast,bytHue,
		bytBlueSaturation, bytRedSaturation, bytPhotoTime, bytChannel, bytInterval,v_wPhotoType);
}

void CPhoto::SendPhotoRequest( BYTE v_bytSizeType, BYTE v_bytQualityType, BYTE v_bytBright,
							 BYTE v_bytContrast, BYTE v_bytHue, BYTE v_bytBlueSaturation, BYTE v_bytRedSaturation, 
							 BYTE v_bytPhotoTotal, BYTE v_bytChannel, BYTE v_bytInterval, WORD v_wPhotoType )
{
	memset( &m_objPhotoSta, 0, sizeof(m_objPhotoSta) );
	
	size_t sizTotal = sizeof(m_objPhotoSta.m_aryPhotoInfo) / sizeof(m_objPhotoSta.m_aryPhotoInfo[0]);
	
	if( size_t(v_bytPhotoTotal) >= sizTotal )
		v_bytPhotoTotal = BYTE(sizTotal);

	int iChannelNum = 0;
	// 待拍照摄像头个数计算
	if( v_bytChannel & 0x01 ) 
		iChannelNum ++;
	if( v_bytChannel & 0x02 ) 
		iChannelNum ++;
	if( v_bytChannel & 0x04 ) 
		iChannelNum ++;
	if( v_bytChannel & 0x08 ) 
		iChannelNum ++;
	
	m_objPhotoSta.m_bytPhotoTotal = v_bytPhotoTotal * iChannelNum;
	
	tagPhotoInfo objToShot;
	DWORD dwPktNum = 0;
	char buf[100] = {0};
	buf[0] = 0x04;
	//static int k = 0;
	for( BYTE byt = 0; byt < v_bytPhotoTotal; byt ++ )
	{
		objToShot.m_bytSizeType = v_bytSizeType;
		objToShot.m_bytQualityType = v_bytQualityType;
		objToShot.m_bytBright = v_bytBright;
		objToShot.m_bytContrast = v_bytContrast;
		objToShot.m_bytHue = v_bytHue;
		objToShot.m_bytBlueSaturation = v_bytBlueSaturation;
		objToShot.m_bytRedSaturation = v_bytRedSaturation;
		objToShot.m_wPhotoType = v_wPhotoType;
		objToShot.m_bytInterval = v_bytInterval;
		for( int i=0; i<4; i++ )
		{
			objToShot.m_bytChannel = v_bytChannel & (0x01<<i) ;
			if( objToShot.m_bytChannel != 0)
			{
				memcpy(buf+1, (void*)&objToShot, sizeof(objToShot));			
				DataPush(buf, sizeof(objToShot)+1, DEV_QIAN, DEV_DVR, LV2);
			//	PRTMSG(MSG_DBG, "Qian push %d frame!\n", ++k);
			}
		}
	}
}

void CPhoto::DealOnePhotoEnd()
{	
	tagPhotoInfo objPhotoInfo;
	DWORD dwDataLen = 0;
	DWORD dwPushTm = 0;
	BYTE bytLvl = 0;
	
	while( !m_objDataMngShotDone.PopData(bytLvl, sizeof(objPhotoInfo), dwDataLen, (char*)&objPhotoInfo, dwPushTm) )
	{
		if( m_objPhotoSta.m_bytPhotoCt < sizeof(m_objPhotoSta.m_aryPhotoInfo) / sizeof(m_objPhotoSta.m_aryPhotoInfo[0]) )
		{
			m_objPhotoSta.m_aryPhotoInfo[m_objPhotoSta.m_bytPhotoCt] = objPhotoInfo;
			m_objPhotoSta.m_bytPhotoCt ++;
		}
	}
#if UPLOAD_AFTERPHOTOALL == 0
	_UploadPhotos();
#endif


}

void CPhoto::_UploadPhotos()
{
	// 保险处理
	if( m_objPhotoSta.m_bytPhotoCt > sizeof(m_objPhotoSta.m_aryPhotoInfo) / sizeof(m_objPhotoSta.m_aryPhotoInfo[0]) )
		m_objPhotoSta.m_bytPhotoCt = sizeof(m_objPhotoSta.m_aryPhotoInfo) / sizeof(m_objPhotoSta.m_aryPhotoInfo[0]);
	
	// 读取配置
	tag2QPhotoCfg objPhotoCfg;
	objPhotoCfg.Init();
	GetSecCfg( &objPhotoCfg, sizeof(objPhotoCfg), offsetof(tagSecondCfg, m_uni2QPhotoCfg.m_obj2QPhotoCfg), sizeof(objPhotoCfg) );
	WORD wUploadPhotoType = _UploadTypeToPhotoType( objPhotoCfg.m_wPhotoUploadSymb ); // 配置里要上传的照片类型
	
	// 逐照片确定是上传照片，还是上传索引
	tagPhotoInfo aryPhoto[PHOTOQUEE_MAXCOUNT];
	tag4144PhotoIndex aryIndex[PHOTOQUEE_MAXCOUNT] = { 0 };
	BYTE bytPhotoCt = 0, bytIndexCt = 0;
	bool bCurPhotoSnd = false;
	for( BYTE byt = 0; byt < m_objPhotoSta.m_bytPhotoCt; byt ++ )
	{
		if( m_objPhotoSta.m_aryPhotoInfo[ byt ].m_bUploaded ) // 若已经处理过上传
			continue;
			
		m_objPhotoSta.m_aryPhotoInfo[ byt ].m_bUploaded = true; // 标记本张照片已经处理过上传
		
		if(  m_objPhotoSta.m_aryPhotoInfo[ byt ].m_wPhotoType & PHOTOEVT_CENT // 若属于中心抓拍
			|| ( !(PHOTOEVT_FOOTALERM_BIG & m_objPhotoSta.m_aryPhotoInfo[byt].m_wPhotoType)
			 &&(wUploadPhotoType & m_objPhotoSta.m_aryPhotoInfo[byt].m_wPhotoType)) // 或属于要主动上传的照片类型 (报警附加的大照片除外))
			)
		{
			bCurPhotoSnd = true;
		}
		else
		{
			bCurPhotoSnd = false;
		}
		
		if( bCurPhotoSnd && bytPhotoCt < PHOTOQUEE_MAXCOUNT )
		{
			aryPhoto[ bytPhotoCt ] = m_objPhotoSta.m_aryPhotoInfo[ byt ];
			++ bytPhotoCt;
		}
		else if( bytIndexCt < PHOTOQUEE_MAXCOUNT )
		{
			aryIndex[ bytIndexCt ].m_bytEvtType
				= _PhotoTypeToEvtType( m_objPhotoSta.m_aryPhotoInfo[ byt ].m_wPhotoType );
			memcpy( aryIndex[ bytIndexCt ].m_szDateTime, m_objPhotoSta.m_aryPhotoInfo[ byt ].m_szDateTime, 6 );
			WORD wIndex = m_objPhotoSta.m_aryPhotoInfo[ byt ].m_wPhotoIndex;
			wIndex = htons( wIndex );
			memcpy( &aryIndex[ bytIndexCt ].m_wIndex, &wIndex, 2 );
			
			++ bytIndexCt;
		}
	}
	
	// 上传索引
	if( bytIndexCt > 0 && bytIndexCt <= PHOTOQUEE_MAXCOUNT )
	{
		char sz4145[ 300 ] = { 0 };
		sz4145[0] = bytIndexCt;
		memcpy( sz4145 + 1, aryIndex, bytIndexCt * sizeof(aryIndex[0]) );
		
		char buf[ 200 ] = { 0 };
		int iBufLen = 0;
		
		int iResult = g_objSms.MakeSmsFrame( sz4145, 1 + bytIndexCt * sizeof(aryIndex[0]),
			0x41, 0x45, buf, sizeof(buf), iBufLen );
		if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV8 );
	}
	
	// 上传照片
	if( bytPhotoCt > 0 )
	{
		P_AddUploadPhoto( aryPhoto, bytPhotoCt );
	}
}

void CPhoto::P_AddUploadPhoto( tagPhotoInfo* v_aryPhoto, BYTE v_bytPhotoTotal )
{
	if( m_objUploadPhotoSta.m_bytAllPhotoCt < m_objUploadPhotoSta.m_bytAllPhotoTotal ) // 若上次上传还未结束
	{
#if USE_PHOTOUPLD == 0
		// 若上传曾经中断过且未曾恢复
		if( m_objUploadPhotoSta.m_bUploadBreak )
		{
			PRTMSG(MSG_DBG, "Last Photo Upload Break\n" );
			_ClearUploadPhotoStaForNewPhoto();

			// 缓存此时还未发送的照片索引
			tagPhotoInfo aryTempIndex[ UPLOADPHOTO_MAXCOUNT ];
			BYTE bytOldTotal = m_objUploadPhotoSta.m_bytAllPhotoTotal - m_objUploadPhotoSta.m_bytAllPhotoCt; // 还未发送的张数
			memcpy( aryTempIndex, m_objUploadPhotoSta.m_aryUpPhotoInfo + m_objUploadPhotoSta.m_bytAllPhotoCt,
				bytOldTotal * sizeof(aryTempIndex[0]) );

			// 新照片放在前面
			if( v_bytPhotoTotal > UPLOADPHOTO_MAXCOUNT ) v_bytPhotoTotal = UPLOADPHOTO_MAXCOUNT; // 保险处理
			memcpy( m_objUploadPhotoSta.m_aryUpPhotoInfo, v_aryPhoto,
				v_bytPhotoTotal * sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) );

			// 旧照片放在后面
			bytOldTotal = v_bytPhotoTotal + bytOldTotal <= UPLOADPHOTO_MAXCOUNT
				? bytOldTotal : UPLOADPHOTO_MAXCOUNT - v_bytPhotoTotal;
			memcpy( m_objUploadPhotoSta.m_aryUpPhotoInfo + v_bytPhotoTotal, aryTempIndex,
 				bytOldTotal * sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) );

			// 现在总共要发送照片的张数
			m_objUploadPhotoSta.m_bytAllPhotoTotal = v_bytPhotoTotal + bytOldTotal;
		}
		else
		{
			// 缓存此时还未发送完成的照片索引
			tagPhotoInfo aryTempIndex[ UPLOADPHOTO_MAXCOUNT ];
			BYTE bytOldTotal = m_objUploadPhotoSta.m_bytAllPhotoTotal - m_objUploadPhotoSta.m_bytAllPhotoCt; // 还未发送的张数
			memcpy( aryTempIndex, m_objUploadPhotoSta.m_aryUpPhotoInfo + m_objUploadPhotoSta.m_bytAllPhotoCt,
				bytOldTotal * sizeof(aryTempIndex[0]) );

			// 未发送的旧照片移至最前
			if( bytOldTotal > UPLOADPHOTO_MAXCOUNT ) bytOldTotal = UPLOADPHOTO_MAXCOUNT; // 保险处理
			memcpy( m_objUploadPhotoSta.m_aryUpPhotoInfo, aryTempIndex,
				bytOldTotal * sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) );

			// 新照片放在最后
			v_bytPhotoTotal = bytOldTotal + v_bytPhotoTotal <= UPLOADPHOTO_MAXCOUNT
				? v_bytPhotoTotal : UPLOADPHOTO_MAXCOUNT - bytOldTotal;
			memcpy( m_objUploadPhotoSta.m_aryUpPhotoInfo + bytOldTotal, v_aryPhoto,
				v_bytPhotoTotal * sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) );

			// 现在总共要发送照片的张数
			m_objUploadPhotoSta.m_bytAllPhotoTotal = bytOldTotal + v_bytPhotoTotal;
		}
								
		// 已发送照片计数清0
		m_objUploadPhotoSta.m_bytAllPhotoCt = 0;
				
		// 保险起见,要再启动重传照片定时 (重要, 否则,可能中心的成功应答车台没有收到,<车台手机模块复位了>,导致下次拍照时照片不能续传)
		if( m_objUploadPhotoSta.m_bPhotoBeginUpload && m_objUploadPhotoSta.m_byt4152Snd )
		{
			m_objUploadPhotoSta.m_obj4152.m_bytReSndTime = 0;
			_SetTimer(&g_objTimerMng, UPLOAD_PHOTOWIN_TIMER, UPLOAD_PHOTOWIN_INTERVAL, G_TmUploadPhotoWin);
			
			PRTMSG(MSG_DBG, "New Upload, but last photo is uploading, Set resend timer\n"); 
		}
		else if( !m_objUploadPhotoSta.m_bPhotoBeginUpload )
		{
			// 发送第一张照片第一个窗口
			_ClearUploadPhotoStaForNewPhoto();
			_UploadOnePhotoWin( m_objUploadPhotoSta.m_aryUpPhotoInfo[0].m_wPhotoIndex, false, 0, 0, NULL );
			
			PRTMSG(MSG_DBG, "New Upload, but last photo hasn't uploaded, upload last photo first\n"); 
		}
#endif

#if USE_PHOTOUPLD == 1
		// 缓存此时还未发送的照片索引
		tagPhotoInfo aryTempIndex[ UPLOADPHOTO_MAXCOUNT ];
		BYTE bytOldTotal = m_objUploadPhotoSta.m_bytAllPhotoTotal - m_objUploadPhotoSta.m_bytAllPhotoCt; // 还未发送的张数
		memcpy( aryTempIndex, m_objUploadPhotoSta.m_aryUpPhotoInfo + m_objUploadPhotoSta.m_bytAllPhotoCt,
			bytOldTotal * sizeof(aryTempIndex[0]) );

		// 未发送的旧照片移至最前
		if( bytOldTotal > UPLOADPHOTO_MAXCOUNT ) bytOldTotal = UPLOADPHOTO_MAXCOUNT; // 保险处理
		memcpy( m_objUploadPhotoSta.m_aryUpPhotoInfo, aryTempIndex,
			bytOldTotal * sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) );

		// 新照片放在最后
		v_bytPhotoTotal = bytOldTotal + v_bytPhotoTotal <= UPLOADPHOTO_MAXCOUNT
			? v_bytPhotoTotal : UPLOADPHOTO_MAXCOUNT - bytOldTotal;
		memcpy( m_objUploadPhotoSta.m_aryUpPhotoInfo + bytOldTotal, v_aryPhoto,
			v_bytPhotoTotal * sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) );

		// 现在总共要发送照片的张数
		m_objUploadPhotoSta.m_bytAllPhotoTotal = bytOldTotal + v_bytPhotoTotal;
#endif
	}
	else
	{
		// 清除旧状态,初始化新状态
		m_objUploadPhotoSta.Init();
		//m_objUploadPhotoSta.m_wPhotoIndex = m_objUploadPhotoSta.m_aryPhotoInfo[0].m_wPhotoIndex;
		
		// 复制新照片索引
		if( v_bytPhotoTotal > UPLOADPHOTO_MAXCOUNT ) v_bytPhotoTotal = UPLOADPHOTO_MAXCOUNT; // 保险起见
		memcpy( m_objUploadPhotoSta.m_aryUpPhotoInfo, v_aryPhoto,
			v_bytPhotoTotal * sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo[0]) );
		
		// 现在总共要发送照片的张数
		m_objUploadPhotoSta.m_bytAllPhotoTotal = v_bytPhotoTotal;
		
		// 已发送照片计数清0
		m_objUploadPhotoSta.m_bytAllPhotoCt = 0;
		
		printf("\n");
		PRTMSG(MSG_DBG, "New Upload,Total %d,First Index:%d\n",
			m_objUploadPhotoSta.m_bytAllPhotoTotal, m_objUploadPhotoSta.m_aryUpPhotoInfo[0].m_wPhotoIndex );

#if USE_PHOTOUPLD == 0		
		// 发送第一张照片第一个窗口
		_ClearUploadPhotoStaForNewPhoto();
		_UploadOnePhotoWin( m_objUploadPhotoSta.m_aryUpPhotoInfo[0].m_wPhotoIndex, false, 0, 0, NULL );
#endif
#if USE_PHOTOUPLD == 1
		_KillTimer(&g_objTimerMng, UPLOAD_PHOTOCHK_TIMER );
		_UploadOnePhoto( m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex );
#endif
	}
}

/// 上传一幅照片: 发满一个窗口,或者发完一幅照片函数退出,若中间有失败,也退出,并且停止上传照片
/// 输入参数中的包序号,是未转义的
int CPhoto::_UploadOnePhotoWin( WORD v_wPhotoIndex, bool v_bSpcyFrame, WORD v_wBeginFrameNo,
							   WORD v_wSpcyFrameTotal, WORD* v_arySpcyFrame )
{
	PRTMSG(MSG_DBG, "Upload One Win of the photo\n" );

	int iRet = 0;
	int iBufLen = 0;
	int iBigFrameLen = 0;
	long lPos = 0;
	char buf[ SOCK_MAXSIZE ];
	char szSendBuf[ SOCK_MAXSIZE ];
	int  iSendLen = 0;
	char szFrame[ UDPDATA_SIZE ]; // 照片包内容
	char szBigFrame[ UDPDATA_SIZE + 200 ]; // 照片帧
	DWORD dwFrameLen = 0;
	DWORD dwSndBytes = 0;
	FILE* fpPhoto = NULL;
	LINUX_HANDLE hPhotoFnd = {0};
	LINUX_HANDLE hDirFnd = {0};
	LINUX_FIND_DATA fdPhoto;
	LINUX_FIND_DATA fdDir;
	char szPhotoName[255] = {0};
	char szPhotoPath[255] = {0};
	char szPhotoIndex[ 32 ] = { 0 };
	DWORD dwPhotoDate = 0;
	WORD wCurFrameNo = 0, wFrameCt = 0, wFrameTotal = 0;
	bool bFillFrame = false; // 循环中是否填充了一帧的标志
	bool bWinEnd = false; // 是否发满一个窗口的标志
	BYTE byt = 0;
	BYTE bytSndFrameCt = 0;
	bool bReqLiuPause = false;
	char szTempBuf1[255] = {0};
	char szTempBuf2[255] = {0};

	// 参数检查
	if( 0 == v_wPhotoIndex )
	{
		PRTMSG(MSG_ERR, "9\n" );
		iRet = ERR_PAR;
		goto _UPLOADONEPHOTOWIN_FAIL;
	}
	if( v_bSpcyFrame )
	{
		if( !v_wSpcyFrameTotal || !v_arySpcyFrame )
		{
			PRTMSG(MSG_ERR, "10\n" );
			iRet = ERR_PAR;
			goto _UPLOADONEPHOTOWIN_FAIL;
		}
		if( v_wSpcyFrameTotal > UPLOADPHOTO_WINFRAMETOTAL )
		{
			PRTMSG(MSG_ERR, "11\n" );
			iRet = ERR_PAR;
			goto _UPLOADONEPHOTOWIN_FAIL;
		}

		if( v_wSpcyFrameTotal >= 2 )
		{
			// 排序输入包序号数组,因为后面对于若发现某个包是照片的最后一包,则发送结束,所以,要按照序号升序排列
			qsort( v_arySpcyFrame, v_wSpcyFrameTotal, sizeof(v_arySpcyFrame[0]), SortWord );

			// 参数再检查,判断输入的包序号是否同属于一个窗口,注意!
			if( v_arySpcyFrame[ 0 ] / UPLOADPHOTO_WINFRAMETOTAL
				!= v_arySpcyFrame[ v_wSpcyFrameTotal - 1 ] / UPLOADPHOTO_WINFRAMETOTAL )
			{
				PRTMSG(MSG_ERR, "12\n" );
				iRet = ERR_PAR;
				goto _UPLOADONEPHOTOWIN_FAIL;
			}
		}
	}

	//上传前清除上传中断标志
	m_objUploadPhotoSta.m_bUploadBreak = false;

	// 若要发送的照片和状态记录不一致,则清除状态记录的数据,以便之后记录新状态 (重要)
	if( v_wPhotoIndex != m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex )
		_ClearUploadPhotoStaForNewPhoto();

	// 获取照片所在的日期文件夹
	dwPhotoDate = m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_dwPhotoDate;

	// 若要发送的包和状态中的记录不在同一窗口,则部分清除,以便传输新窗口
	if( v_bSpcyFrame )
	{
		if( v_arySpcyFrame[0] / UPLOADPHOTO_WINFRAMETOTAL != m_objUploadPhotoSta.m_wPhotoWinCt )
		{
			_ClearUploadPhotoStaForNewWin(); // 部分清除状态
			m_objUploadPhotoSta.m_wPhotoWinCt = v_arySpcyFrame[0] / UPLOADPHOTO_WINFRAMETOTAL; // 重设已完成窗口计数 (确保状态正确)
		}
	}
	else
	{
		if( v_wBeginFrameNo / UPLOADPHOTO_WINFRAMETOTAL != m_objUploadPhotoSta.m_wPhotoWinCt )
		{
			_ClearUploadPhotoStaForNewWin(); // 部分清除状态
			m_objUploadPhotoSta.m_wPhotoWinCt = v_wBeginFrameNo / UPLOADPHOTO_WINFRAMETOTAL; // 重设已完成窗口计数 (确保状态正确)
		}
	}

	/// 准备照片目录名
	memcpy(szPhotoPath, m_szPhotoPath, sizeof(m_szPhotoPath));
	sprintf( szPhotoIndex, "%04x", v_wPhotoIndex );

	wFrameCt = 0;
	do
	{
		/// 确定当前要发送的包序号 (包序号从0开始编号)
		if( v_bSpcyFrame ) // 若有指定帧 (应该是窗口重发)
		{
			wCurFrameNo = v_arySpcyFrame[ wFrameCt ];
		}
		else if( v_wPhotoIndex == m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex )
			// 若要发送的照片和状态记录中的相符 (应该是要发本照片的新窗口)
		{
			wCurFrameNo = v_wBeginFrameNo + wFrameCt;
		}
		else
		{
			// 只可能第一次循环时进入
			wCurFrameNo = 0;
		}
		if( INVALID_NUM_16 == wCurFrameNo )
		{
			PRTMSG(MSG_ERR, "13\n" );
			iRet = ERR_PAR;
			goto _UPLOADONEPHOTOWIN_FAIL;
		}

		/// 填充当前要发送的包内容
		bFillFrame = false;
		dwFrameLen = 0;
		if( v_wPhotoIndex == m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_wPhotoIndex )
			// 若状态中的照片和要发送的照片相同
		{
			// 遍历查找是否已有缓存的帧
			for( byt = 0; byt < UPLOADPHOTO_WINFRAMETOTAL; byt ++ )
			{
				if( wCurFrameNo == m_objUploadPhotoSta.m_aryPhotoFrameNo[ byt ] ) // 若该缓存包序号与当前需要的包相同
				{
					// 安全检测包长度是否合法
					if( 0 != m_objUploadPhotoSta.m_aryPhotoFrameLen[ byt ]
						&& DWORD(m_objUploadPhotoSta.m_aryPhotoFrameLen[ byt ]) > sizeof(szFrame) )
					{
						// 若该包长度不合法,清除该包缓存
						m_objUploadPhotoSta.m_aryPhotoFrameNo[ byt ] = 0xffff;
						break;
					}

					memcpy( szFrame, m_objUploadPhotoSta.m_aryPhotoFrameBuf[ byt ],
						m_objUploadPhotoSta.m_aryPhotoFrameLen[ byt ] );
					dwFrameLen = m_objUploadPhotoSta.m_aryPhotoFrameLen[ byt ];
					bFillFrame = true;
					
					break;
				}
			}
		}
		
		if( !bFillFrame ) // 若没有填充(可能是状态中的照片和当前要发送照片不同,也可能是状态中不含要发送包的缓存数据),则从照片文件中读取
		{
			// 若没有打开过,寻找并打开
			// 先尝试打开状态记录中的照片文件名
			// (如果记录中的照片和当前照片不同,记录中的文件名已被清除了,则打开操作将失败,没关系,继续执行)
			if( !fpPhoto )
			{
				memcpy(szPhotoName, m_objUploadPhotoSta.m_szPhotoPath, MAX_PATH);
				fpPhoto = fopen( szPhotoName, "rb" );
			}

			// 若打不开,且若照片日期目录名有效，则从照片目录直接打开
			if( !fpPhoto && dwPhotoDate >= BELIV_DATE )
			{
				char szDir[30];
				sprintf( szDir, "%d/", dwPhotoDate );
				
				hPhotoFnd = FindFirstFile( SPRINTF(szTempBuf1, "%s%s", szPhotoPath, szDir), SPRINTF(szTempBuf2, "%s%s", szPhotoIndex, "_*.jpg"), &fdPhoto, false );
				if( hPhotoFnd.bValid )
				{
					if( !(S_IFDIR & fdPhoto.objFileStat.st_mode) )
					{
						sprintf(szPhotoName, "%s%s%s", szPhotoPath, szDir, fdPhoto.szFileName);// 记录当前照片路径
						fpPhoto = fopen( szPhotoName, "rb" );
					}
					memset(&hPhotoFnd, 0, sizeof(LINUX_HANDLE));
				}
			}

			if( !fpPhoto ) // 若还是打不开, 寻找该索引照片文件
			{				
				/// 遍历照片目录
				hDirFnd = FindFirstFile( szPhotoPath, "*", &fdDir, false );
				if( hDirFnd.bValid )
				{
					do
					{
						// 不是子目录,跳过
						if( !(S_IFDIR & fdDir.objFileStat.st_mode) ) 
							continue;
						
						// 不是有效的子目录,跳过
						if( !strcmp(".", fdDir.szFileName) || !strcmp("..", fdDir.szFileName) )
							continue;
						
						// 遍历照片文件
						hPhotoFnd = FindFirstFile( SPRINTF(szTempBuf1, "%s%s%s", szPhotoPath, fdDir.szFileName, "/"), SPRINTF(szTempBuf2, "%s%s", szPhotoIndex, "_*.jpg"), &fdPhoto, false );
						if( hPhotoFnd.bValid )
						{
							do
							{
								if( S_IFDIR & fdPhoto.objFileStat.st_mode )
									continue;
								
								// 记录当前照片路径
								sprintf(szPhotoName, "%s%s%s%s", szPhotoPath, fdDir.szFileName, "/", fdPhoto.szFileName);
							
								fpPhoto = fopen(szPhotoName, "rb" );	
							} while( !fpPhoto && FindNextFile(&hPhotoFnd, &fdPhoto, false) );
							
							memset(&hPhotoFnd, 0, sizeof(LINUX_HANDLE));
						}
					} while( !fpPhoto && FindNextFile(&hDirFnd, &fdDir, false) );
				
					memset(&hDirFnd, 0, sizeof(LINUX_HANDLE));
				}
			}

			if( !fpPhoto ) // 若没有找到
			{
				PRTMSG( MSG_DBG, "Find Photo File Fail in Disk,Index: %04x\n", szPhotoIndex );
				iRet = ERR_FILE_FAILED;
				goto _UPLOADONEPHOTOWIN_FAIL;
			}
			else if( strcmp(szPhotoName, m_objUploadPhotoSta.m_szPhotoPath) != 0  )
				// 否则若状态记录中的照片和当前照片不同,则要提取当前照片信息
			{
				// 计算文件大小
				if( fseek(fpPhoto, 0, SEEK_END) )
				{
					PRTMSG( MSG_DBG, "2\n" );
					iRet = ERR_FILE_FAILED;
					goto _UPLOADONEPHOTOWIN_FAIL;
				}
				long lEnd = ftell( fpPhoto );
				if( lEnd <= 0 )
				{
					PRTMSG( MSG_DBG, "14\n" );
					iRet = ERR_FILE_FAILED;
					goto _UPLOADONEPHOTOWIN_FAIL;
				}
				DWORD dwFileSize = DWORD( lEnd );
				if( 0 == dwFileSize )
				{
					PRTMSG( MSG_DBG, "3\n" );
					iRet = ERR_FILE_FAILED;
					goto _UPLOADONEPHOTOWIN_FAIL;
				}

				// 从文件名提取照片相关信息
				DWORD dwDate, dwTime;
				long lLon, lLat;
				WORD wPhotoIndex, wPhotoType;
				BYTE bytGpsValid, bytSizeType, bytQualityType, bytChannel;
				IN_GetIndexFromPhotoName( szPhotoName, wPhotoIndex );
				IN_GetDateTimeFromPhotoName( szPhotoName, dwDate, dwTime );
				IN_GetTypeFromPhotoName( szPhotoName, wPhotoType );
				IN_GetLonLatFromPhotoName( szPhotoName, lLon, lLat, bytGpsValid );
				IN_GetChannelSizeQualityFromPhotoName( szPhotoName, bytChannel, bytSizeType, bytQualityType );

				// 分辨率类型还原为协议定义方式
				switch( bytSizeType )
				{
				case 1:
					bytSizeType = 2;
					break;
				case 2:
					bytSizeType = 1;
					break;
				case 3:
					//bytSizeType = 1; // 老大说让大照片用中等尺寸显示可能更清楚
					bytSizeType = 3; // 还是保留大尺寸显示，免得引起误会
					break;
				default:
					bytSizeType = 2;
				}
				
				// 安全起见判断
				if( wPhotoIndex != v_wPhotoIndex )
				{
					PRTMSG( MSG_DBG, "4\n" );
					iRet = ERR_FILE_FAILED;
					goto _UPLOADONEPHOTOWIN_FAIL;
				}
				
				// 记录新状态 (可能重复写入相同的,不要紧,且一般不应出现此种情况)
				m_objUploadPhotoSta.m_dwPhotoSize = dwFileSize; // 照片尺寸
				m_objUploadPhotoSta.m_bytPhotoSizeType = bytSizeType; // 分辨率类型
				m_objUploadPhotoSta.m_bytPhotoQualityType = bytQualityType; // 质量类型
				m_objUploadPhotoSta.m_wPhotoFrameTotal = WORD( m_objUploadPhotoSta.m_dwPhotoSize
					/ UDPDATA_SIZE + (m_objUploadPhotoSta.m_dwPhotoSize % UDPDATA_SIZE ? 1 : 0) ); // 总包数
				_ConvertLonLatFromLongToStr( lLon / LONLAT_DO_TO_L, lLat / LONLAT_DO_TO_L,
					m_objUploadPhotoSta.m_szLonLat, sizeof(m_objUploadPhotoSta.m_szLonLat) ); // 经纬度
				m_objUploadPhotoSta.m_wPhotoType = wPhotoType; // 照片类型
				m_objUploadPhotoSta.m_bytPhotoChannel = bytChannel; // 通道号
				m_objUploadPhotoSta.m_szDateTime[ 0 ] = BYTE(dwDate / 10000); // 日期时间
				m_objUploadPhotoSta.m_szDateTime[ 1 ] = BYTE(dwDate % 10000 / 100);
				m_objUploadPhotoSta.m_szDateTime[ 2 ] = BYTE(dwDate % 100);
				m_objUploadPhotoSta.m_szDateTime[ 3 ] = BYTE(dwTime / 10000);
				m_objUploadPhotoSta.m_szDateTime[ 4 ] = BYTE(dwTime % 10000 / 100);
				m_objUploadPhotoSta.m_szDateTime[ 5 ] = BYTE(dwTime % 100);
				
				// 照片全路径名
				strncpy( m_objUploadPhotoSta.m_szPhotoPath, szPhotoName,
					sizeof(m_objUploadPhotoSta.m_szPhotoPath) / sizeof(m_objUploadPhotoSta.m_szPhotoPath[0]) );
				m_objUploadPhotoSta.m_szPhotoPath[ sizeof(m_objUploadPhotoSta.m_szPhotoPath) / sizeof(m_objUploadPhotoSta.m_szPhotoPath[0]) - 1 ] = 0;
				
				PRTMSG( MSG_DBG, "Cur Photo File: %s\n", szPhotoName );
				PRTMSG( MSG_DBG, "Photo File Size:%d, Packets Total: %d\n", 
				                 m_objUploadPhotoSta.m_dwPhotoSize, m_objUploadPhotoSta.m_wPhotoFrameTotal );
			}
			

			//////// 直接从照片文件读取照片数据 /////////

			// 跳转到读取处
			if( fseek(fpPhoto, wCurFrameNo * UDPDATA_SIZE, SEEK_SET) )
			{
				PRTMSG( MSG_DBG, "5\n" );
				iRet = ERR_FILE_FAILED;
				goto _UPLOADONEPHOTOWIN_FAIL;
			}
			
			// 判断现在可以读取多少数据 (也由此得知该照片该窗口是否上传完毕)
			lPos = ftell( fpPhoto );
			if( lPos < 0 || lPos > long(m_objUploadPhotoSta.m_dwPhotoSize) )
			{
				PRTMSG( MSG_DBG, "6\n" );
				iRet = ERR_FILE_FAILED;
				goto _UPLOADONEPHOTOWIN_FAIL;
			}
			dwFrameLen = DWORD(m_objUploadPhotoSta.m_dwPhotoSize - lPos);
			if( dwFrameLen <= UDPDATA_SIZE )
				bWinEnd = true; // 标志照片上传完毕,同时该窗口也结束
			else
				dwFrameLen = UDPDATA_SIZE;
			
			// 读取
			if( 1 != fread(szFrame, dwFrameLen, 1, fpPhoto) )
			{
				PRTMSG( MSG_DBG, "7: Pos = %d, Read bytes = %d, Total = %d\n", lPos, dwFrameLen, m_objUploadPhotoSta.m_dwPhotoSize );
				
				iRet = ERR_FILE_FAILED;
				goto _UPLOADONEPHOTOWIN_FAIL;
			}
			bFillFrame = true;
			
			// 缓存该数据包
			memcpy( m_objUploadPhotoSta.m_aryPhotoFrameBuf[ wFrameCt ], szFrame, dwFrameLen );
			m_objUploadPhotoSta.m_aryPhotoFrameLen[ wFrameCt ] = WORD( dwFrameLen );
			m_objUploadPhotoSta.m_aryPhotoFrameNo[ wFrameCt ] = wCurFrameNo;
		}

		// 若没有成功填充该包,退出
		if( !bFillFrame )
		{
			PRTMSG(MSG_DBG, "16\n" );
			goto _UPLOADONEPHOTOWIN_FAIL;
		}

		// 递增已发送包计数,并判断是否发满该窗口
		if( wCurFrameNo + 1 >= m_objUploadPhotoSta.m_wPhotoFrameTotal ) bWinEnd = true;
		wFrameCt ++;
		if( v_wSpcyFrameTotal > 0 && wFrameCt >= v_wSpcyFrameTotal ) bWinEnd = true;
		else if( wFrameCt >= UPLOADPHOTO_WINFRAMETOTAL ) bWinEnd = true;

		// 生成数据包 标志位(1) + 当前图像索引号（2）+ 总包数（2） + 当前包序号（2）
		// + 【开始时间（6）+ 起始经纬度（8）+分辨率（1）+图像质量等级（1）+事件类型（1）+ 通道号（1）】+数据长度(2) + 数据内容(n)
		szBigFrame[ 0 ] = 0x21 | (bWinEnd ? 0x40 : 0) | (m_objUploadPhotoSta.m_wPhotoType & ~PHOTOEVT_CENT ? 0x80 : 0);
		szBigFrame[ 1 ] = BYTE(v_wPhotoIndex / 0x100);
		szBigFrame[ 2 ] = BYTE(v_wPhotoIndex % 0x100);
		szBigFrame[ 3 ] = m_objUploadPhotoSta.m_wPhotoFrameTotal / 0x100;
		szBigFrame[ 4 ] = m_objUploadPhotoSta.m_wPhotoFrameTotal % 0x100;
		szBigFrame[ 5 ] = wCurFrameNo / 0x100;
		szBigFrame[ 6 ] = wCurFrameNo % 0x100;
		if( 0 == wCurFrameNo ) // 若是照片的第一包数据
		{
			memcpy( szBigFrame + 7, m_objUploadPhotoSta.m_szDateTime, 6 );
			memcpy( szBigFrame + 13, m_objUploadPhotoSta.m_szLonLat, 8 );
			szBigFrame[ 21 ] = m_objUploadPhotoSta.m_bytPhotoSizeType;
			szBigFrame[ 22 ] = m_objUploadPhotoSta.m_bytPhotoQualityType;
			szBigFrame[ 23 ] = _PhotoTypeToEvtType( m_objUploadPhotoSta.m_wPhotoType );
			szBigFrame[ 24 ] = 1 == m_objUploadPhotoSta.m_bytPhotoChannel || 2 == m_objUploadPhotoSta.m_bytPhotoChannel 
				|| 4 == m_objUploadPhotoSta.m_bytPhotoChannel || 8 == m_objUploadPhotoSta.m_bytPhotoChannel 
				? m_objUploadPhotoSta.m_bytPhotoChannel : 1;
			iBigFrameLen = 25;
		}
		else
		{
			iBigFrameLen = 7;
		}
		szBigFrame[ iBigFrameLen ++ ] = char( dwFrameLen / 0x100 );
		szBigFrame[ iBigFrameLen ++ ] = char( dwFrameLen % 0x100 );
		if( DWORD(iBigFrameLen) + dwFrameLen > sizeof(szBigFrame) )
		{
			PRTMSG(MSG_DBG, "17\n" );
			iRet = ERR_MEMLACK;
			goto _UPLOADONEPHOTOWIN_FAIL;
		}
		memcpy( szBigFrame + iBigFrameLen, szFrame, dwFrameLen );
		iBigFrameLen += int( dwFrameLen );

		// 发送数据包，推到SockServExe发送
		iRet = g_objSms.MakeSmsFrame( szBigFrame, iBigFrameLen, 0x41, 0x50, buf, sizeof(buf), iBufLen, CVT_NONE, false );
		
		szSendBuf[0] = 0x04;
		iRet = g_objSock.MakeUdpFrame(buf, iBufLen, 0x02, 0x20, szSendBuf+1, sizeof(szSendBuf)-1, iSendLen);
		if( !iRet )
		{
			iRet = DataPush(szSendBuf, iSendLen+1, DEV_QIAN, DEV_SOCK, LV2);
		}

		if( !iRet )
		{
			dwSndBytes += DWORD(iBufLen);
			++ bytSndFrameCt;
			m_objUploadPhotoSta.m_bPhotoBeginUpload = true;
		}

		// 若是最后本窗口的最后一个包，缓存信息,重置重发次数,开启重传定时器
		if( bWinEnd )
		{
			memcpy( &m_objUploadPhotoSta.m_obj4152, szBigFrame, sizeof(m_objUploadPhotoSta.m_obj4152) );
			m_objUploadPhotoSta.m_obj4152.m_bytReSndTime = 0;

			DWORD dwInterval = 5000;
			_KillTimer(&g_objTimerMng, UPLOAD_PHOTOWIN_TIMER );
			_SetTimer(&g_objTimerMng, UPLOAD_PHOTOWIN_TIMER, dwInterval, G_TmUploadPhotoWin ); // 重发定时
		}
	} while( !bWinEnd ); // 若发送窗口未满则继续循环

	// 都发送成功
	iRet = 0;

	PRTMSG(MSG_DBG, "This Win of the Photo Pushed to Send!\n" );

	goto _UPLOADONEPHOTOWIN_END;

_UPLOADONEPHOTOWIN_FAIL:
	_StopUploadPhoto();
	PRTMSG(MSG_ERR, "Upload this Win of the photo Fail, err code:%d\n", iRet );

_UPLOADONEPHOTOWIN_END:
	if( fpPhoto ) 
		fclose( fpPhoto );

	PRTMSG(MSG_NOR, "Sended this win, Packets Num: %d\n", bytSndFrameCt );

	return iRet;
}

int CPhoto::_UploadOnePhoto( WORD v_wPhotoIndex )
{
	PRTMSG(MSG_NOR, "Upload One photo\n" );

	char szTempBuf1[1024];
	char szTempBuf2[1024];

	int iRet = 0;
	FILE* fpPhoto = NULL;
	LINUX_HANDLE hPhotoFnd = {0};
	LINUX_HANDLE hDirFnd = {0};
	LINUX_FIND_DATA fdPhoto;
	LINUX_FIND_DATA fdDir;
	char szPhotoPath[255] = {0};
	char szPhotoName[255] = {0};
	DWORD dwPhotoDate = 0;
	DWORD dwFileSize = 0;
	DWORD dwReadLen = 0;
	int iBufLen = 0;
	char szPhotoIndex[ 32 ] = { 0 };
	char szPhoto[ TCP_SENDSIZE ] = { 0 };
	char buf[ SOCK_MAXSIZE ] = { 0 };

	// 获取照片所在的日期文件夹
	dwPhotoDate = m_objUploadPhotoSta.m_aryUpPhotoInfo[ m_objUploadPhotoSta.m_bytAllPhotoCt ].m_dwPhotoDate;

	/// 准备照片目录名
	memcpy(szPhotoPath, m_szPhotoPath, sizeof(m_szPhotoPath));
	sprintf( szPhotoIndex, "%04x", v_wPhotoIndex );

	// 若打不开,且若照片日期目录名有效，则从照片目录直接打开
	if( !fpPhoto && dwPhotoDate >= BELIV_DATE )
	{
		char szDir[30];
		sprintf( szDir, "%d/", dwPhotoDate );
 
		hPhotoFnd = FindFirstFile( SPRINTF(szTempBuf1, "%s%s", szPhotoPath, szDir), SPRINTF(szTempBuf2, "%s%s", szPhotoIndex, "_*.jpg"), &fdPhoto, false );
		if( hPhotoFnd.bValid )
		{
			if( !(S_IFDIR & fdPhoto.objFileStat.st_mode) )
			{
				// 记录当前照片路径
				sprintf(szPhotoName, "%s%s%s", szPhotoPath, szDir, fdPhoto.szFileName);
				fpPhoto = fopen( szPhotoName, "rb" );
			}
			memset(&hPhotoFnd, 0, sizeof(LINUX_HANDLE));
		}
	}

	if( !fpPhoto ) // 若还是打不开, 寻找该索引照片文件
	{				
		/// 遍历照片目录
		hDirFnd = FindFirstFile( szPhotoPath, "*", &fdDir, false );
		if( hDirFnd.bValid )
		{
			do
			{
				// 不是子目录,跳过
				if( !(S_IFDIR & fdDir.objFileStat.st_mode) ) continue;
				
				// 不是有效的子目录,跳过
				if( !strcmp(".", fdDir.szFileName) || !strcmp("..", fdDir.szFileName) ) continue;
				
				// 遍历照片文件
				hPhotoFnd = FindFirstFile( SPRINTF(szTempBuf1, "%s%s%s", szPhotoPath, fdDir.szFileName, "/"), SPRINTF(szTempBuf2, "%s%s", szPhotoIndex, "_*.jpg"), &fdPhoto, false );
				if( hPhotoFnd.bValid )
				{
					do
					{
						if( S_IFDIR & fdPhoto.objFileStat.st_mode ) continue;
						
						sprintf(szPhotoName, "%s%s%s%s", szPhotoPath, fdDir.szFileName, "/", fdPhoto.szFileName);// 记录当前照片路径
						fpPhoto = fopen(szPhotoName, "rb" );
						
					} while( !fpPhoto && FindNextFile(&hPhotoFnd, &fdPhoto, false) );
				
					memset(&hPhotoFnd, 0, sizeof(LINUX_HANDLE));
				}
			} while( !fpPhoto && FindNextFile(&hDirFnd, &fdDir, false) );
		
			memset(&hDirFnd, 0, sizeof(LINUX_HANDLE));
		}
	}

	if( !fpPhoto ) // 若没有找到
	{
		PRTMSG( MSG_DBG, "Find Photo File Fail in Disk,Index: %04x\n", szPhotoIndex );
		iRet = ERR_FILE_FAILED;
		goto _UPLOADONEPHOTO_FAIL;
	}
	// 	else if( strcmp(strPhotoName, m_objUploadPhotoSta.m_wszPhotoPath) != 0 )
	// 		// 否则若状态记录中的照片和当前照片不同,则要提取当前照片信息
	{
		// 计算文件大小
		if( fseek(fpPhoto, 0, SEEK_END) )
		{
			PRTMSG( MSG_DBG, "2\n" );
			iRet = ERR_FILE_FAILED;
			goto _UPLOADONEPHOTO_FAIL;
		}
		long lEnd = ftell( fpPhoto );
		if( lEnd <= 0 )
		{
			PRTMSG( MSG_DBG, "14\n" );
			iRet = ERR_FILE_FAILED;
			goto _UPLOADONEPHOTO_FAIL;
		}
		dwFileSize = DWORD( lEnd );
		if( 0 == dwFileSize )
		{
			PRTMSG( MSG_DBG, "3\n" );
			iRet = ERR_FILE_FAILED;
			goto _UPLOADONEPHOTO_FAIL;
		}
		
		// 从文件名提取照片相关信息
		DWORD dwDate, dwTime;
		long lLon, lLat;
		WORD wPhotoIndex, wPhotoType;
		BYTE bytGpsValid, bytSizeType, bytQualityType, bytChannel;
		IN_GetIndexFromPhotoName( szPhotoName, wPhotoIndex );
		IN_GetDateTimeFromPhotoName( szPhotoName, dwDate, dwTime );
		IN_GetTypeFromPhotoName( szPhotoName, wPhotoType );
		IN_GetLonLatFromPhotoName( szPhotoName, lLon, lLat, bytGpsValid );
		IN_GetChannelSizeQualityFromPhotoName( szPhotoName, bytChannel, bytSizeType, bytQualityType );
		
		// 分辨率类型还原为协议定义方式
		switch( bytSizeType )
		{
		case 1:
			bytSizeType = 2;
			break;
		case 2:
			bytSizeType = 1;
			break;
		case 3:
			//bytSizeType = 1; // 老大说让大照片用中等尺寸显示可能更清楚
			bytSizeType = 3; // 还是保留大尺寸显示，免得引起误会
			break;
		default:
			bytSizeType = 2;
		}
		
		// 安全起见判断
		if( wPhotoIndex != v_wPhotoIndex )
		{
			PRTMSG( MSG_DBG, "4\n" );
			iRet = ERR_FILE_FAILED;
			goto _UPLOADONEPHOTO_FAIL;
		}
		
		// 记录新状态 (可能重复写入相同的,不要紧,且一般不应出现此种情况)
		m_objUploadPhotoSta.m_dwPhotoSize = dwFileSize; // 照片尺寸
		m_objUploadPhotoSta.m_bytPhotoSizeType = bytSizeType; // 分辨率类型
		m_objUploadPhotoSta.m_bytPhotoQualityType = bytQualityType; // 质量类型
		m_objUploadPhotoSta.m_wPhotoFrameTotal = WORD( m_objUploadPhotoSta.m_dwPhotoSize
			/ UDPDATA_SIZE + (m_objUploadPhotoSta.m_dwPhotoSize % UDPDATA_SIZE ? 1 : 0) ); // 总包数
		_ConvertLonLatFromLongToStr( lLon / LONLAT_DO_TO_L, lLat / LONLAT_DO_TO_L,
			m_objUploadPhotoSta.m_szLonLat, sizeof(m_objUploadPhotoSta.m_szLonLat) ); // 经纬度
		m_objUploadPhotoSta.m_wPhotoType = wPhotoType; // 照片类型
		m_objUploadPhotoSta.m_bytPhotoChannel = bytChannel; // 通道号
		m_objUploadPhotoSta.m_szDateTime[ 0 ] = BYTE(dwDate / 10000); // 日期时间
		m_objUploadPhotoSta.m_szDateTime[ 1 ] = BYTE(dwDate % 10000 / 100);
		m_objUploadPhotoSta.m_szDateTime[ 2 ] = BYTE(dwDate % 100);
		m_objUploadPhotoSta.m_szDateTime[ 3 ] = BYTE(dwTime / 10000);
		m_objUploadPhotoSta.m_szDateTime[ 4 ] = BYTE(dwTime % 10000 / 100);
		m_objUploadPhotoSta.m_szDateTime[ 5 ] = BYTE(dwTime % 100);

		PRTMSG( MSG_DBG, "Cur Photo File: %s\n", szPhotoName );
		PRTMSG( MSG_DBG, "Photo File Size:%d, Packets Total: %d\n", 
			m_objUploadPhotoSta.m_dwPhotoSize, m_objUploadPhotoSta.m_wPhotoFrameTotal );
	}

	// 时间（6）+ 纬度（4）+ 经度（4）+分辨率（1）+图像质量等级（1）+事件类型（1）+ 通道号（1）+索引号（2）+数据长度（4）+数据（n）
#define PHOTOHEAD_LEN	24

	// 跳转到起始位置
	if( fseek(fpPhoto, 0, SEEK_SET) )
	{
		PRTMSG( MSG_DBG, "5\n" );
		iRet = ERR_FILE_FAILED;
		goto _UPLOADONEPHOTO_FAIL;
	}
	
	// 读取
	if( dwFileSize + PHOTOHEAD_LEN > sizeof(szPhoto) )
	{
		iRet = ERR_BUFLACK;
		PRTMSG( MSG_DBG, "6\n" );
		goto _UPLOADONEPHOTO_FAIL;
	}
	dwReadLen = fread(szPhoto + PHOTOHEAD_LEN, dwFileSize, 1, fpPhoto);
	if( 1 != dwReadLen )
	{
		PRTMSG( MSG_DBG, "7: Read bytes != FileSize(%d)\n", dwFileSize );
		
		iRet = ERR_FILE_FAILED;
		goto _UPLOADONEPHOTO_FAIL;
	}
	
	memcpy( szPhoto, m_objUploadPhotoSta.m_szDateTime, 6 );
	memcpy( szPhoto + 6, m_objUploadPhotoSta.m_szLonLat + 4, 4 );
	memcpy( szPhoto + 10, m_objUploadPhotoSta.m_szLonLat, 4 );
	szPhoto[ 14 ] = m_objUploadPhotoSta.m_bytPhotoSizeType;
	szPhoto[ 15 ] = m_objUploadPhotoSta.m_bytPhotoQualityType;
	szPhoto[ 16 ] = _PhotoTypeToEvtType( m_objUploadPhotoSta.m_wPhotoType );
	szPhoto[ 17 ] = 1 == m_objUploadPhotoSta.m_bytPhotoChannel || 2 == m_objUploadPhotoSta.m_bytPhotoChannel 
		|| 4 == m_objUploadPhotoSta.m_bytPhotoChannel || 8 == m_objUploadPhotoSta.m_bytPhotoChannel 
		? m_objUploadPhotoSta.m_bytPhotoChannel : 1;
	szPhoto[ 18 ] = BYTE(v_wPhotoIndex / 0x100);
	szPhoto[ 19 ] = BYTE(v_wPhotoIndex % 0x100);
	szPhoto[ 20 ] = char( dwFileSize / 0x1000000 );
	szPhoto[ 21 ] = char( dwFileSize % 0x1000000 / 0x10000 );
	szPhoto[ 22 ] = char( dwFileSize % 0x10000 / 0x100 );
	szPhoto[ 23 ] = char( dwFileSize % 0x100 );

	// 发送数据包
	iRet = g_objSms.MakeSmsFrame( szPhoto, PHOTOHEAD_LEN + dwFileSize, 0x41, 0x40, buf, sizeof(buf), iBufLen, CVT_NONE, true );
	if( !iRet )
	{
		iRet = g_objSock.TcpSendSmsData( buf, iBufLen, LV8, DATASYMB_415052 );
	}

	iRet = 0;

	PRTMSG(MSG_NOR, "This Photo Pushed to Send!\n" );

	_SetTimer( &g_objTimerMng, UPLOAD_PHOTOCHK_TIMER, 60000, G_TmUploadPhotoChk ); // 重发定时

	goto _UPLOADONEPHOTO_END;

_UPLOADONEPHOTO_FAIL:
	_StopUploadPhoto();
	PRTMSG(MSG_NOR, "Upload this photo Fail, err code:%d", iRet );
	
_UPLOADONEPHOTO_END:
	if( fpPhoto ) fclose( fpPhoto );
	return iRet;
}

void CPhoto::_StopUploadPhoto()
{
	// 清除上传照片信息
	memset( m_objUploadPhotoSta.m_aryUpPhotoInfo, 0, sizeof(m_objUploadPhotoSta.m_aryUpPhotoInfo) );
	m_objUploadPhotoSta.m_bytAllPhotoTotal = 0;

	// 若全部照片已拍完，则清除拍照信息
	if( m_objPhotoSta.m_bytPhotoCt == m_objPhotoSta.m_bytPhotoTotal )
		memset((void*)&m_objPhotoSta, 0, sizeof(m_objPhotoSta));
	
	// 为今后上传新照片清除部分状态信息
	_ClearUploadPhotoStaForNewPhoto();
	
	// 停止重传定时
	_KillTimer( &g_objTimerMng, UPLOAD_PHOTOWIN_TIMER );
	
	// 停止照片上传应答检查定时
	_KillTimer( &g_objTimerMng, UPLOAD_PHOTOCHK_TIMER );
	
	PRTMSG( MSG_DBG, "Stop Upload Photo!\n" );
}

/// 为发新的窗口或新的照片,对状态信息做清除工作
void CPhoto::_ClearUploadPhotoStaForNewWin()
{
	// 清除窗口重发缓存
	memset( &m_objUploadPhotoSta.m_obj4152, 0, sizeof(m_objUploadPhotoSta.m_obj4152) );
	
	// 清除状态记录中的缓存包信息,使得以后重发时不会误发不正确的包
	memset( m_objUploadPhotoSta.m_aryPhotoFrameNo, 0xff, sizeof(m_objUploadPhotoSta.m_aryPhotoFrameNo) );
	
	// 清除重发标志
	m_objUploadPhotoSta.m_byt4152Snd = 0;
	
	// 清除重发次数
	m_objUploadPhotoSta.m_obj4152.m_bytReSndTime = 0;
	
	m_objUploadPhotoSta.m_bUploadBreak = false;
}

void CPhoto::_ClearUploadPhotoStaForNewPhoto()
{
	m_objUploadPhotoSta.m_dwPhotoSize = 0; // 当前正上传照片的文件大小
	m_objUploadPhotoSta.m_wPhotoFrameTotal = 0; // 当前正上传照片的总包数
	m_objUploadPhotoSta.m_wPhotoWinCt = 0; // 当前正上传照片的已完成的窗口计数
	
	m_objUploadPhotoSta.m_szPhotoPath[0] = 0; // 当前正上传照片的全路径名
	memset( m_objUploadPhotoSta.m_szDateTime, 0, sizeof(m_objUploadPhotoSta.m_szDateTime) ); // 当前正上传照片的日期时间
	memset( m_objUploadPhotoSta.m_szLonLat, 0, sizeof(m_objUploadPhotoSta.m_szLonLat) ); //当前正上传照片的经纬度
	m_objUploadPhotoSta.m_wPhotoType = 0; // 当前正上传照片的事件类型
	memset( &m_objUploadPhotoSta.m_obj4152, 0, sizeof(m_objUploadPhotoSta.m_obj4152) ); // 清除窗口重发缓存
	m_objUploadPhotoSta.m_byt4152Snd = 0; // 清除重发标志
	m_objUploadPhotoSta.m_obj4152.m_bytReSndTime = 0; // 清除重发次数
	m_objUploadPhotoSta.m_bPhotoBeginUpload = false;
	
	m_objUploadPhotoSta.m_bUploadBreak = false;
	
	// 当前上传照片的帧数据缓存 (保存未确认窗口内包数据,或者)
	memset( m_objUploadPhotoSta.m_aryPhotoFrameNo, 0xff, sizeof(m_objUploadPhotoSta.m_aryPhotoFrameNo) ); // 各包序号,从0开始编号,0不转义, 若=0xffff,说明该位置缓存数据无效
}

bool CPhoto::IsPhotoEnd()
{
	bool bRet = m_objPhotoSta.m_bytPhotoCt >= m_objPhotoSta.m_bytPhotoTotal;
	return bRet;
}

bool CPhoto::IsUploadPhotoEnd()
{
	bool bRet = m_objUploadPhotoSta.m_bytAllPhotoCt >= m_objUploadPhotoSta.m_bytAllPhotoTotal;
	return bRet;
}

bool CPhoto::IsUploadPhotoBreak()
{
	bool bRet = m_objUploadPhotoSta.m_bUploadBreak;
	return bRet;
}

void CPhoto::_ConvertLonLatFromLongToStr( double v_dLon, double v_dLat, char* v_szLonLat, size_t v_sizStr )
{
	if( v_sizStr < 8 ) return;
	
	v_szLonLat[ 0 ] = BYTE( v_dLon );
	double dFen = (v_dLon - BYTE(v_szLonLat[0]) ) * 60;
	v_szLonLat[ 1 ] = BYTE( dFen );
	WORD wFenxiao = WORD( (dFen - BYTE(v_szLonLat[1]) ) * 10000 );
	v_szLonLat[ 2 ] = wFenxiao / 100;
	v_szLonLat[ 3 ] = wFenxiao % 100;
	
	v_szLonLat[ 4 ] = BYTE( v_dLat );
	dFen = (v_dLat - BYTE(v_szLonLat[4]) ) * 60;
	v_szLonLat[ 5 ] = BYTE( dFen );
	wFenxiao = WORD( (dFen - BYTE(v_szLonLat[5]) ) * 10000 );
	v_szLonLat[ 6 ] = wFenxiao / 100;
	v_szLonLat[ 7 ] = wFenxiao % 100;
}

WORD CPhoto::_EvtTypeToPhotoType( BYTE v_bytEvtType )
{
#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
/*
0x01：报警（包括抢劫报警和侧翻报警）
0x02：点火（ACC开）
0x03：开车门
0x04：空车－>重车 / 重车－>空车
*/
	switch( v_bytEvtType )
	{
	case 0x01: // 报警
		return PHOTOEVT_FOOTALERM;

	case 0x02: //hxd 080816
		return PHOTOEVT_ACCON;

	case 0x03://hxd 080816 
		return PHOTOEVT_OPENDOOR;

	case 0x04: // 空车->重车
		return PHOTOEVT_JIJIADOWN;

	case 0x06://hxd 080816
		return PHOTOEVT_TIMER;

	case 0x07: //进出区域
		return PHOTOEVT_AREA;
		
	case 0x08: //超速
		return PHOTOEVT_OVERSPEED;
		
	case 0x09: //低速
		return PHOTOEVT_BELOWSPEED;
		
	case 0x0: // 中心抓拍
		return PHOTOEVT_CENT;
		
	case 0xff: // 全部类型
		return 0xffff;

	default:
		return 0;
	}
#endif

#if USE_PROTOCOL == 30
/*
0x00：中心主动抓拍事件
0x11：抢劫报警
0x12：侧翻报警
0x13：碰撞报警
0x21：交通事故
0x22：医疗求助
0x23：纠纷求助
0x31：关车门
0x41：进出区域
0x51：定时拍照
0x61：ACC点火拍照
*/
	switch( v_bytEvtType )
	{
	case 0x11: // 劫警
		return PHOTOEVT_FOOTALERM;

	case 0x12: // 侧翻报警
		return PHOTOEVT_OVERTURNALERM;

	case 0x13: // 碰撞报警
		return PHOTOEVT_BUMPALERM;

	case 0x21: // 交通求助
		return PHOTOEVT_TRAFFICHELP;

	case 0x22: // 医疗求助
		return PHOTOEVT_MEDICALHELP;

	case 0x23: // 纠纷求助
		return PHOTOEVT_BOTHERHELP;

	case 0x41: // 进出区域
		return PHOTOEVT_AREA;

	case 0x31: // 开车门拍照
		return PHOTOEVT_OPENDOOR;

	case 0x51: // 定时拍照
		return PHOTOEVT_TIMER;

	case 0x61: // ACC点火
		return PHOTOEVT_ACCON;

	case 0: // 中心抓拍
		return PHOTOEVT_CENT;

	case 0xff: // 全部类型
		return 0xffff;

	default:
		return 0;
	}
#endif
}

BYTE CPhoto::_PhotoTypeToEvtType( WORD v_wPhotoType )
{
#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
	if( v_wPhotoType & PHOTOEVT_FOOTALERM )
		return 0x01;

	else if( v_wPhotoType & PHOTOEVT_ACCON )
		return 0x02;

	else if( v_wPhotoType & PHOTOEVT_OPENDOOR )
		return 0x03;

	else if( v_wPhotoType & PHOTOEVT_JIJIADOWN )
		return 0x04;

	else if( v_wPhotoType & PHOTOEVT_TIMER )
		return 0x06;
	
	else if( v_wPhotoType & PHOTOEVT_AREA )
		return 0x07;
		
	else if( v_wPhotoType & PHOTOEVT_OVERSPEED )
		return 0x08;
		
	else if( v_wPhotoType & PHOTOEVT_BELOWSPEED )
		return 0x09;
		
	else if( v_wPhotoType & PHOTOEVT_CENT )
		return 0;
		
	else
		return 0;
#endif

#if USE_PROTOCOL == 30
	if( v_wPhotoType & PHOTOEVT_FOOTALERM )
		return 0x11;

	else if( v_wPhotoType & PHOTOEVT_OVERTURNALERM )
		return 0x12;

	else if( v_wPhotoType & PHOTOEVT_BUMPALERM )
		return 0x13;

	else if( v_wPhotoType & PHOTOEVT_TRAFFICHELP )
		return 0x21;

	else if( v_wPhotoType & PHOTOEVT_MEDICALHELP )
		return 0x22;
	
	else if( v_wPhotoType & PHOTOEVT_BOTHERHELP )
		return 0x23;
	
	else if( v_wPhotoType & PHOTOEVT_AREA )
		return 0x41;

	else if( v_wPhotoType & PHOTOEVT_OPENDOOR )
		return 0x31;
	
	else if( v_wPhotoType & PHOTOEVT_ACCON )
		return 0x61;
	
	else if( v_wPhotoType & PHOTOEVT_TIMER )
		return 0x51;
	
	else if( v_wPhotoType & PHOTOEVT_CENT )
		return 0;
	
	else
		return 0;
#endif
}

/// 根据上传类型,得到对应的照片类型
WORD CPhoto::_UploadTypeToPhotoType( WORD v_wUploadType )
{
	return v_wUploadType; // 注意: 因为照片类型和上传类型的定义完全一致,所以才能这样!
}

