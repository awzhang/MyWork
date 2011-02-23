#include "yx_QianStdAfx.h"

#if USE_BLK == 1

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-Blk      ")

int SortStr( const void* v_pElement1, const void* v_pElement2 )
{
	return strcmp( (char*)v_pElement1, (char*)v_pElement2 );
}

int SortBlackPeriod( const void* v_pElement1, const void* v_pElement2 )
{
	tagBlackQueryPeriod *pPeriod1 = (tagBlackQueryPeriod*)v_pElement1;
	tagBlackQueryPeriod *pPeriod2 = (tagBlackQueryPeriod*)v_pElement2;
	
	int iRet = int(pPeriod1->m_bytFromYear) - pPeriod2->m_bytFromYear;
	if( iRet ) return iRet;
	
	iRet = int(pPeriod1->m_bytFromMon) - pPeriod2->m_bytFromMon;
	if( iRet ) return iRet;
	
	iRet = int(pPeriod1->m_bytFromDay) - pPeriod2->m_bytFromDay;
	if( iRet ) return iRet;
	
	iRet = int(pPeriod1->m_bytFromHour) - pPeriod2->m_bytFromHour;
	if( iRet ) return iRet;
	
	return int(pPeriod1->m_bytFromMin) - pPeriod2->m_bytFromMin;
}

void G_TmBlkSave(void *arg, int len)
{
	g_objBlk.P_TmBlkSave();
}

void G_TmBlkSndRes(void *arg, int len)
{
	g_objBlk.P_TmBlkSndRes();
}

void G_TmBlkRcvQuery(void *arg, int len)
{
	g_objBlk.P_TmBlkRcvQuery();
}


CBlk::CBlk()
{
	m_i0e60Len = 0;
	
	memset(m_szBlackPath, 0, sizeof(m_szBlackPath));
	memset(m_szBlackFile, 0, sizeof(m_szBlackFile));
	
	memcpy(m_szBlackPath, BLK_SAVE_PATH, strlen(BLK_SAVE_PATH));

	m_pfBlack = NULL;
	m_iBlkFile = 0;
	
	m_dwLstGpsSaveTm = GetTickCount();
	m_wBlkGpsCt = 0;
}

CBlk::~CBlk()
{
	if( m_pfBlack )
	{
		if( m_wBlkGpsCt > 0 )
		{
			fwrite( m_aryBlackGps, sizeof(m_aryBlackGps[0]) * m_wBlkGpsCt, 1, m_pfBlack );
			m_wBlkGpsCt = 0;
		}
		fclose( m_pfBlack );
		m_pfBlack = NULL;
	}
}

void CBlk::P_TmBlkSave()
{
//	PRTMSG(MSG_DBG, "Blk Save Timer!(100224)\n");

	tagBlackSaveGps objBlackSaveGps;
	int iResult = _GetCurBlackSaveGps( objBlackSaveGps );

	if( !iResult )
	{
		_SaveBlackGps( objBlackSaveGps ); // 此时,已保证黑匣子GPS数据是有效的,且速度>0
	}

	if( ERR_BLACKGPS_INVALID == iResult )
	{
		//PRTMSG(MSG_DBG, "Gps invalid, wait for 5s!\n");
		_SetTimer(&g_objTimerMng, BLACK_TIMER, 5000, G_TmBlkSave ); // 若黑匣子数据无效,则延时5秒后检查
	}
	else
	{
		_SetTimer(&g_objTimerMng, BLACK_TIMER, BLACK_MININTERVAL * 1000, G_TmBlkSave ); // (固定黑匣子保存间隔)
	}
}

void CBlk::P_TmBlkSndRes()
{
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	int iResult = 0;

	tag2QGprsCfg objGprsCfg;
	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	DWORD dwBegin = GetTickCount();
	char szCurFile[ MAX_PATH ];
	FILE* fp = NULL;
	size_t sizResult;
	char* szBlkFileName = NULL;
	tagBlackSaveGps aryBlackSaveGps[ BLKREADUNIT ];
	tag0e60BlackGps aryBlackGps[ BLKSNDUNIT ];
	tagBlackQueryPeriod* pPeriod;
	DWORD dwCt = 0; // 黑匣子点轮循计数
	WORD wNameSize = sizeof(szCurFile) / sizeof(szCurFile[0]);
	BYTE bytYear = 0, bytMon = 0, bytDayNo = 0; // 黑匣子文件名解析得到的年月日信息
	BYTE bytPeriodBeginNo = 0, byt = 0;
	BYTE bytAreaCt = 0; // 本帧内满足范围条件的黑匣子数据计数
	BYTE bytSndCt = 0; // 本帧要发送的黑匣子数据计数
	BYTE bytAdvanceEndSymb = 0; // 0,不是提前结束; 1,提前结束本次定时器处理; 2,提前结束全部查询处理
	bool bFitArea = false, bFitPeriod = false;

	BYTE bytDataType = 0;
	tag0e60 res;

#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL || 30 == USE_PROTOCOL
	const BYTE PKT0E60_BASLEN = 16;
	bytDataType = 0x60;
	memcpy( res.m_szCentGroup, m_objBlackQuerySta.m_szCentGroup, sizeof(res.m_szCentGroup) );
#endif
#if 2 == USE_PROTOCOL
	const BYTE PKT0E60_BASLEN = 6;
	bytDataType = 0x50;
#endif

	// 遍历黑匣子数据文件进行处理
	while( m_objBlackSndSta.m_wDoneFileCount < m_objBlackSndSta.m_wFileTotal )
	{
		// 跳过前面遍历过的文件,定位到本次要处理的文件
		szBlkFileName = (char*)(m_objBlackSndSta.m_szBlackFile[m_objBlackSndSta.m_wDoneFileCount]);

		// 寻找该黑匣子文件所满足的第一个时间段条件
		bytPeriodBeginNo = INVALID_NUM_8;
		if( _GetPeriodFromBlkFileName(szBlkFileName, bytYear, bytMon, bytDayNo) )
		{
			for( byt = 0; byt < m_objBlackQuerySta.m_bytPeriodCount; byt ++ )
			{
				pPeriod = m_objBlackQuerySta.m_aryPeriod + byt;
				if( pPeriod->m_bytFromYear > bytYear || pPeriod->m_bytToYear < bytYear )
				{
					continue;
				}
				if( (pPeriod->m_bytFromYear == bytYear && pPeriod->m_bytFromMon > bytMon)
					||
					(pPeriod->m_bytToYear == bytYear && pPeriod->m_bytToMon < bytMon)
				  )
				{
					continue;
				}
				if(
					(pPeriod->m_bytFromYear == bytYear && pPeriod->m_bytFromMon == bytMon && pPeriod->m_bytFromDay / DSK2_ONEFILE_DAYS + 1 > bytDayNo)
					||
					(pPeriod->m_bytToYear == bytYear && pPeriod->m_bytToMon == bytMon && pPeriod->m_bytToDay / DSK2_ONEFILE_DAYS + 1 < bytDayNo)
				  )
				{
					continue;
				}

				bytPeriodBeginNo = byt;
				break;
			}
		}
		else
		{
			// 无法从文件名解析信息的,将跳过
			PRTMSG(MSG_ERR, "can not deal the file:%s\n", szBlkFileName);
			iResult = ERR_FILE;
			continue;
		}

		// 若有时间段条件,且未找到本文件所满足的时间段条件,跳过
		if( m_objBlackQuerySta.m_bytPeriodCount > 0 && INVALID_NUM_8 == bytPeriodBeginNo )
		{
			goto BLKONEFILELOOP_END;
		}

		// 文件名
		memset(szCurFile, 0, sizeof(szCurFile));
		memcpy(szCurFile, m_szBlackPath, strlen(m_szBlackPath));
		memcpy(szCurFile+strlen(m_szBlackPath), szBlkFileName, strlen(szBlkFileName));

		// 打开文件
		fp = fopen( szCurFile, "rb" );
		if( !fp )
		{
			PRTMSG(MSG_ERR, "Open Blk File To Read Failure: %s\n" , szCurFile);
			iResult = ERR_FILE_FAILED;
			goto BLKONEFILELOOP_END;
		}

		// 跳过已处理的点
		if( fseek(fp, sizeof(tagBlackSaveGps) * m_objBlackSndSta.m_dwLstFileDoneCt, SEEK_SET) )
		{
			iResult = ERR_FILE_FAILED;
			goto BLKONEFILELOOP_END;
		}

		// 分批读取黑匣子数据,提取满足条件的点,组帧发送,并更新发送状态
		bytSndCt = 0; // 初始化待发送的黑匣子点个数 (满一帧了才发送)
		do 
		{
			// 读取一个黑匣子数据块
			sizResult = fread( aryBlackSaveGps, sizeof(aryBlackSaveGps[0]), BLKREADUNIT, fp );
			if( 0 == sizResult )
			{
				iResult = ERR_FILE_FAILED;
				goto BLKONEFILELOOP_END;
			}

			// 逐黑匣子点判断
			bytAreaCt = 0; // 初始化满足区域条件的黑匣子点计数
			for( dwCt = 0; dwCt < sizResult; dwCt ++ )
			{
				// 遍历地域条件判断
				bFitArea = false; // 初始化为不满足地域条件
				for( byt = 0; byt < m_objBlackQuerySta.m_bytAreaCount; byt ++ )
				{
					if( JugPtInRect( aryBlackSaveGps[ dwCt ].m_lLon, aryBlackSaveGps[ dwCt ].m_lLat,
						m_objBlackQuerySta.m_aryArea[ byt ].m_lLonMin, m_objBlackQuerySta.m_aryArea[ byt ].m_lLatMin,
						m_objBlackQuerySta.m_aryArea[ byt ].m_lLonMax, m_objBlackQuerySta.m_aryArea[ byt ].m_lLatMax ) )
					{
						bFitArea = true;
						break;
					}
				}

				if( m_objBlackQuerySta.m_bytAreaCount > 0 && !bFitArea ) continue; // 若有地域条件但不满足,跳过

				// 遍历时间段条件判断 (从前面已得到的第一个满足的时间段条件开始)
				bFitPeriod = false; // 初始化为不满足时间段条件
				for( byt = bytPeriodBeginNo; byt < m_objBlackQuerySta.m_bytPeriodCount; byt ++ )
				{
					pPeriod = m_objBlackQuerySta.m_aryPeriod + byt;

					// 若该点不在年的范围内,跳过
					if( aryBlackSaveGps[ dwCt ].m_bytYear < pPeriod->m_bytFromYear
						|| aryBlackSaveGps[ dwCt ].m_bytYear > pPeriod->m_bytToYear )
						continue;

					if( aryBlackSaveGps[ dwCt ].m_bytYear == pPeriod->m_bytFromYear ) // 若在起始年
					{
						if( aryBlackSaveGps[ dwCt ].m_bytMon < pPeriod->m_bytFromMon ) // 若在起始月之前 跳过
							continue;
						else if( aryBlackSaveGps[ dwCt ].m_bytMon == pPeriod->m_bytFromMon ) // 若在起始月
						{
							if( aryBlackSaveGps[ dwCt ].m_bytDay < pPeriod->m_bytFromDay ) // 若在起始天之前,跳过
								continue;
							else if( aryBlackSaveGps[ dwCt ].m_bytDay == pPeriod->m_bytFromDay ) // 若在起始天
							{
								if( aryBlackSaveGps[ dwCt ].m_bytHour < pPeriod->m_bytFromHour ) // 若在起始时之前,跳过
									continue;
								else if( aryBlackSaveGps[ dwCt ].m_bytHour == pPeriod->m_bytFromHour ) // 若在起始时
								{
									if( aryBlackSaveGps[ dwCt ].m_bytMin < pPeriod->m_bytFromMin ) // 若在起始分之前,跳过
										continue;
								}
							}
						}
					}

					if( aryBlackSaveGps[ dwCt ].m_bytYear == pPeriod->m_bytToYear ) // 若在终止年
					{
						if( aryBlackSaveGps[ dwCt ].m_bytMon > pPeriod->m_bytToMon ) // 若在终止月之后 跳过
							continue;
						else if( aryBlackSaveGps[ dwCt ].m_bytMon == pPeriod->m_bytToMon ) // 若在终止月
						{
							if( aryBlackSaveGps[ dwCt ].m_bytDay > pPeriod->m_bytToDay ) // 若在终止天之后,跳过
								continue;
							else if( aryBlackSaveGps[ dwCt ].m_bytDay == pPeriod->m_bytToDay ) // 若在终止天
							{
								if( aryBlackSaveGps[ dwCt ].m_bytHour > pPeriod->m_bytToHour ) // 若在终止时之后,跳过
									continue;
								else if( aryBlackSaveGps[ dwCt ].m_bytHour == pPeriod->m_bytToHour ) // 若在终止时
								{
									if( aryBlackSaveGps[ dwCt ].m_bytMin > pPeriod->m_bytToMin ) // 若在终止分之后,跳过
										continue;
								}
							}
						}
					}

					bFitPeriod = true;
					break;
				}

				if( m_objBlackQuerySta.m_bytPeriodCount > 0 && !bFitPeriod )
				{
					continue; // 若有时间段条件但不满足,跳过
				}

				_BlackSaveGpsToBlackGps( aryBlackSaveGps[ dwCt ], aryBlackGps[ bytSndCt ++ ] );
			
				if( bFitArea ) bytAreaCt ++; // 若满足区域条件，累加该类个数

				if( bytSndCt >= BLKSNDUNIT ) // 若待上传的黑匣子点满一帧
				{
					if( m_i0e60Len > 0 ) // 若有前一次数据没有上传
					{
						// 先发送前一次数据
						iResult = g_objSms.MakeSmsFrame( (char*)&m_obj0e60, m_i0e60Len, 0x0e, bytDataType, buf, sizeof(buf), iBufLen );
						if( !iResult )
						{
							iResult = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9,
								DATASYMB_OE60 | ((objGprsCfg.m_bytChannelBkType_2 & 0x40) ? DATASYMB_SMSBACKUP : 0) );
							if( !iResult )
							{
								m_objBlackSndSta.m_dwGpsTotal += m_obj0e60.m_bytGpsCount;
								m_objBlackSndSta.m_bytSndFrameCount ++;
							}
						}

						// 前一次数据缓存清空
						memset((void*)&m_obj0e60, 0, sizeof(m_obj0e60) );
						m_i0e60Len = 0;
					}

					// 递增上传帧数计数 (因为本次数据)
					m_objBlackQuerySta.m_bytUploadFrameCt ++;

					// 判断上传的帧数是否已经达到最大值
					if( m_objBlackQuerySta.m_bytUploadFrameCt >= m_objBlackQuerySta.m_bytUploadFrameTotal )
					{
						bytAdvanceEndSymb = 2; // 要提前结束全部黑匣子查询处理
						break; // 跳出本块黑匣子数据处理
					}

					// 生成本次数据
					res.m_bytResFrameNo = m_objBlackSndSta.m_bytSndFrameCount + 1;
					res.m_bytGpsCount = BLKSNDUNIT;
					res.m_bytResType = 0x01; // 都预先置为非结束帧的成功应答
					memcpy( res.m_aryBlackGps, aryBlackGps, sizeof(res.m_aryBlackGps) ); 
					res.m_bytFitAreaCount = bytAreaCt;
					res.m_bytFitTotalHi = 0;
					res.m_bytFitTotalLo = 0;

					// 暂存本次数据(下次发送)
					m_obj0e60 = res;
					m_i0e60Len = PKT0E60_BASLEN + sizeof(tag0e60BlackGps) * BLKSNDUNIT;

					// 重置计数,以便下次使用
					bytSndCt = 0, bytAreaCt = 0;
				}
			}

			// 更新本黑匣子文件中,已完成的黑匣子点计数
			m_objBlackSndSta.m_dwLstFileDoneCt += dwCt;

			// 本次处理若已花费大量时间,则提前结束
			if( GetTickCount() - dwBegin >= TIMER_MAXPERIOD ) // 若本次发送用时已经超过允许最大值
			{
				if(bytAdvanceEndSymb != 2)
				{
					PRTMSG(MSG_DBG, "query over time, quit\n");
					bytAdvanceEndSymb = 1; // 要提前结束本次定时器黑匣子处理
					break; // 先跳出本文件处理
				}
			}
		} while( BLKREADUNIT == sizResult && !bytAdvanceEndSymb );

BLKONEFILELOOP_END:
		if( fp )
		{
			fclose( fp );
			fp = NULL;
		}

		if( bytAdvanceEndSymb ) // 若是提前结束 (但不包括异常导致的提前结束)
			break;
		else
		{
			m_objBlackSndSta.m_wDoneFileCount ++; // 更新已处理的文件记录
			m_objBlackSndSta.m_dwLstFileDoneCt = 0; // 对新文件,重置该文件已处理的点个数
		}
	}

	if( fp ) 
		fclose( fp );

	if( bytSndCt > 0 ) // 若有本次数据
	{
		if( m_i0e60Len > 0 ) // 若有前一次数据还未上传
		{
			// 发送前一次数据
			iResult = g_objSms.MakeSmsFrame( (char*)&m_obj0e60, m_i0e60Len, 0x0e, bytDataType, buf, sizeof(buf), iBufLen );
			if( !iResult )
			{
				iResult = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9,
					DATASYMB_OE60 | ((objGprsCfg.m_bytChannelBkType_2 & 0x40) ? DATASYMB_SMSBACKUP : 0) );
				if( !iResult )
				{
					m_objBlackSndSta.m_dwGpsTotal += m_obj0e60.m_bytGpsCount;
					m_objBlackSndSta.m_bytSndFrameCount ++;
				}
			}

			// 前一次数据缓存清空 (重要)
			memset((void*)&m_obj0e60, 0, sizeof(m_obj0e60) );
			m_i0e60Len = 0;
		}

		// 生成本次数据
		res.m_bytResFrameNo = m_objBlackSndSta.m_bytSndFrameCount + 1;
		res.m_bytGpsCount = bytSndCt;
		res.m_bytResType = 0x01; // 都预先置为非结束帧的成功应答
		memcpy( res.m_aryBlackGps, aryBlackGps, bytSndCt * sizeof(res.m_aryBlackGps[0]) ); 
		res.m_bytFitAreaCount = bytAreaCt;
		res.m_bytFitTotalHi = 0;
		res.m_bytFitTotalLo = 0;

		// 暂存本次数据(下次发送)
		m_obj0e60 = res;
		m_i0e60Len = PKT0E60_BASLEN + sizeof(tag0e60BlackGps) * bytSndCt;

		// 重置计数,以便下次使用
		bytSndCt = 0, bytAreaCt = 0;
	}

	/// **后面都归结到"无本次数据"的处理了** ///

	int iEndType = 0; // 1,中止；2，结束
	if( m_objBlackSndSta.m_wDoneFileCount >= m_objBlackSndSta.m_wFileTotal || 2 == bytAdvanceEndSymb )
	{
		iEndType = 2;
	}
	else if( 1 == bytAdvanceEndSymb )
	{
		iEndType = 1;
	}

	if(  2 == iEndType ) // 若是结束查询
	{
		if( m_i0e60Len > 0 ) // 若有前一次数据 
		{
			// 修改为成功结束帧
			m_obj0e60.m_bytResType = 0x02;
			
			// 填充黑匣子数据总数
			m_objBlackSndSta.m_dwGpsTotal += m_obj0e60.m_bytGpsCount;
			if( m_i0e60Len >= 2 ) // (安全起见)
			{
				*( (BYTE*)(&m_obj0e60) + m_i0e60Len - 2 ) = BYTE( m_objBlackSndSta.m_dwGpsTotal / 0x7e );
				*( (BYTE*)(&m_obj0e60) + m_i0e60Len - 1 ) = BYTE( m_objBlackSndSta.m_dwGpsTotal % 0x7e );
			}
			
			// 发送最后一帧数据
			iResult = g_objSms.MakeSmsFrame( (char*)&m_obj0e60, m_i0e60Len, 0x0e, bytDataType, buf, sizeof(buf), iBufLen );
			if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9,
				DATASYMB_OE60 | ((objGprsCfg.m_bytChannelBkType_2 & 0x40) ? DATASYMB_SMSBACKUP : 0) );
		}
		else
		{
			// 生成失败应答数据
			res.m_bytResType = 0x04; // 没有满足符合条件的帧
			
			// 发送数据
			iResult = g_objSms.MakeSmsFrame( (char*)&res, 11, 0x0e, bytDataType, buf, sizeof(buf), iBufLen );
			if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV9, 
				(objGprsCfg.m_bytChannelBkType_2 & 0x40) ? DATASYMB_SMSBACKUP : 0 );
		}

		_KillTimer(&g_objTimerMng, BLACK_SNDRES_TIMER ); // 关闭定时器
	}
	else
	{
		_KillTimer(&g_objTimerMng, BLACK_SNDRES_TIMER ); // 关闭定时器
		_SetTimer(&g_objTimerMng, BLACK_SNDRES_TIMER, BLACK_SNDINTERVAL, G_TmBlkSndRes ); // 重开定时器
	}
}

void CBlk::P_TmBlkRcvQuery()
{
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;
	int iResult = 0;
	BYTE bytDataType = 0;
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	bytDataType = 0x60;
#endif
#if 2 == USE_PROTOCOL
	bytDataType = 0x50;
#endif

	DWORD dwCur = GetTickCount();

	if( dwCur - m_objBlackQuerySta.m_dwLstQueryTime >= BLACKQUERY_MAXINTERVAL // 若2次查询帧间隔超时
		&& m_objBlackQuerySta.m_bytReqFrameTotal > m_objBlackQuerySta.m_bytReqFrameCount ) // 且查询帧未接受完整
	{
		tag0e60 res;
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
		memcpy( &res.m_szCentGroup, m_objBlackQuerySta.m_szCentGroup, sizeof(res.m_szCentGroup) );
#endif
		res.m_bytResType = 0x03; // 查询帧不完整

		tag2QGprsCfg objGprsCfg;
		GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

		iResult = g_objSms.MakeSmsFrame( (char*)&res, 11, 0x0e, bytDataType, buf, sizeof(buf), iBufLen );
		if( !iResult ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12,
			(objGprsCfg.m_bytChannelBkType_2 & 0x40) ? DATASYMB_SMSBACKUP : 0 );

		_KillTimer(&g_objTimerMng, BLACK_RCVQUERY_TIMER );
	}
	else // 即2次查询帧间隔未超时 (查询帧接收完整的情况这里不可能出现)
	{
		m_objBlackQuerySta.m_dwLstQueryTime = dwCur;
		_SetTimer(&g_objTimerMng, BLACK_RCVQUERY_TIMER, BLACKQUERY_MAXINTERVAL, G_TmBlkRcvQuery );
	}
}

int CBlk::_GetCurBlackSaveGps( tagBlackSaveGps &v_objBlackSaveGps )
{
	static bool sta_bFstValid = true;
	tagGPSData gps(0);
	int iResult = 0;

	// 必须调用监控模块的获取GPS接口，因为该接口有针对黑匣子模块的一些处理
	iResult = g_objMonAlert.GetCurGPS( gps, false );
	if( iResult )
	{
		memset( (void*)&v_objBlackSaveGps, 0, sizeof(v_objBlackSaveGps) );
		return iResult;
	}
	else if( 'A' != gps.valid )			//GPS无效则返回错误
	{
		memset( (void*)&v_objBlackSaveGps, 0, sizeof(v_objBlackSaveGps) );
		return ERR_BLACKGPS_INVALID;
	}
// 	// 首次有效必须保存，但在这之后，若速度为0，则不予保存，节省空间
// 	else if( !sta_bFstValid && gps.speed < BLKSAVE_MINSPEED )	
// 	{
// 		memset( (void*)&v_objBlackSaveGps, 0, sizeof(v_objBlackSaveGps) );
// 		return ERR_BLACKGPS_INVALID;
// 	}

	sta_bFstValid = false;
	_GpsToBlackSaveGps( gps, v_objBlackSaveGps );
	
	return 0;
}

void CBlk::Init()
{
	BlkSaveStart();
	return ;

	unsigned char ucCurIOSta = 0;
	IOGet(IOS_ACC, &ucCurIOSta);
	
	if( ucCurIOSta == IO_ACC_ON )
	{
		BlkSaveStart();
	}
	else
	{
		PRTMSG(MSG_NOR, "Acc invalid, can not save blk!\n");
	}
}

void CBlk::BlkSaveStart()
{
	if( access(m_szBlackPath, F_OK) != 0 )
	{
		struct stat lDirStat;
		if( 0 == mkdir(m_szBlackPath, lDirStat.st_mode) )
		{
			chown(m_szBlackPath, lDirStat.st_uid, lDirStat.st_gid);
		}
		else
		{
			perror("Blk mkdir failed: ");
			return;
		}
	}

	tag2QBlkCfg objBlkCfg;
	objBlkCfg.Init();
	GetSecCfg( &objBlkCfg, sizeof(objBlkCfg),	offsetof(tagSecondCfg, m_uni2QBlkCfg.m_obj2QBlkCfg), sizeof(objBlkCfg) );

	if( objBlkCfg.m_dwBlackChkInterv < BLACK_MININTERVAL )
		objBlkCfg.m_dwBlackChkInterv = BLACK_MININTERVAL;

	_SetTimer(&g_objTimerMng, BLACK_TIMER, BLACK_MININTERVAL * 1000, G_TmBlkSave ); // 固定黑匣子间隔
	P_TmBlkSave();

	PRTMSG(MSG_NOR, "Blk Save Timer Open\n" );
}

void CBlk::BlkSaveStop()
{
	if( NULL != m_pfBlack )
	{
		fflush(m_pfBlack);
		fclose(m_pfBlack);
	}
	_KillTimer(&g_objTimerMng, BLACK_TIMER );

	PRTMSG(MSG_NOR, "Blk Save Timer Close\n" );
}

// 本函数内不能再调用
int CBlk::GetLatestBlkGps( tagGPSData& v_objGps )
{
	int iRet = 0;
	char szFileName[MAX_PATH] = {0};

	if( m_wBlkGpsCt > 0 && m_pfBlack )
	{
		fwrite( m_aryBlackGps, sizeof(m_aryBlackGps[0]) * m_wBlkGpsCt, 1, m_pfBlack );
		fclose( m_pfBlack );
		m_pfBlack = NULL;

		m_dwLstGpsSaveTm = GetTickCount();
		m_wBlkGpsCt = 0;
	}

	struct dirent **namelist = NULL;
	int n = 0;
	
	n = scandir(m_szBlackPath, &namelist, 0, alphasort);
	if (n > 0)
	{
		char szCompareName[10] = {0};
		char szLatestFile[ MAX_PATH ] = { 0 };
		char szSubLatestFile[ MAX_PATH ] = { 0 };
		memcpy(szLatestFile, "0000_0", strlen("0000_0"));

		while(n--)
		{
			if( 0 == strcmp(".", namelist[n]->d_name) || 0 == strcmp("..", namelist[n]->d_name) )
				continue;	
			if( 14 != strlen(namelist[n]->d_name) )
				continue;

			memcpy(szCompareName, namelist[n]->d_name + 4, 6);
			if( strcmp(szLatestFile, szCompareName) < 0 )
			{
				memcpy( szSubLatestFile, szLatestFile, 6 );
				szSubLatestFile[ 6 ] = 0;
				
				memcpy( szLatestFile, namelist[n]->d_name + 4, 6 );
				szLatestFile[ 6 ] = 0;
			}
		} 

		sprintf(szFileName, "%sBlk_%s.dat", m_szBlackPath,szLatestFile);
		FILE* fp = fopen( szFileName, "rb" );
		if( fp )
		{
			tagBlackSaveGps objBlkSaveGps;
			if( fseek(fp, long(sizeof(tagBlackSaveGps)) * -1, SEEK_END) )
			{
				fclose(fp);
				iRet = ERR_FILE;
				goto _GETSUBLATESTFILE;
			}
			if( 1 != fread(&objBlkSaveGps, sizeof(objBlkSaveGps), 1, fp) )
			{
				fclose(fp);
				iRet = ERR_FILE;
				goto _GETSUBLATESTFILE;
			}

			fclose( fp );

			_BlackSaveGpsToGps( objBlkSaveGps, v_objGps );
			v_objGps.valid = 'V';
			v_objGps.direction = 0;
			v_objGps.speed = 0;
			SetCurGps( &v_objGps, GPS_REAL | GPS_LSTVALID );
			GetCurGps( &v_objGps, sizeof(v_objGps), GPS_LSTVALID );
		}
		else
		{
			iRet = ERR_FILE_FAILED;
			goto _GETSUBLATESTFILE;
		}

		goto _GETCURGPS_END;

_GETSUBLATESTFILE: // 可能最新的黑匣子文件大小为0或者读取出错,所以此时要使用次新的文件
		sprintf(szFileName, "%sBlk_%s.dat", m_szBlackPath, szSubLatestFile);
		fp = fopen( szFileName, "rb" );
		if( fp )
		{
			tagBlackSaveGps objBlkSaveGps;
			if( fseek(fp, long(sizeof(tagBlackSaveGps)) * -1, SEEK_END) )
			{
				fclose(fp);
				iRet = ERR_FILE;
				goto _GETCURGPS_END;
			}
			if( 1 != fread(&objBlkSaveGps, sizeof(objBlkSaveGps), 1, fp) )
			{
				fclose(fp);
				iRet = ERR_FILE;
				goto _GETCURGPS_END;
			}

			fclose( fp );

			_BlackSaveGpsToGps( objBlkSaveGps, v_objGps );
			v_objGps.valid = 'V';
			v_objGps.direction = 0;
			v_objGps.speed = 0;
			SetCurGps( &v_objGps, GPS_REAL | GPS_LSTVALID );
			GetCurGps( &v_objGps, sizeof(v_objGps), GPS_LSTVALID );
		}
		else
		{
			iRet = ERR_FILE_FAILED;
		}
	}
	else
	{
		iRet = ERR_FILE_FAILED;
	}

_GETCURGPS_END:
	if(NULL != namelist) 
		free(namelist);
	return iRet;
}

void CBlk::SaveCurBlkGps()
{
	P_TmBlkSave();

	// 缓存的黑匣子数据写入磁盘
	if( m_pfBlack )
	{
		if( m_wBlkGpsCt > 0 )
		{
			fwrite( m_aryBlackGps, sizeof(m_aryBlackGps[0]) * m_wBlkGpsCt, 1, m_pfBlack );
			
			m_dwLstGpsSaveTm = GetTickCount();
 			m_wBlkGpsCt = 0;
		}
		else
		{
			fflush( m_pfBlack );
		}

		fclose( m_pfBlack );
		m_pfBlack = NULL;
	}
}

// 调用时,应保证黑匣子GPS数据是有效的,且速度>0
int CBlk::_SaveBlackGps( const tagBlackSaveGps &v_objBlackSaveGps )
{
	int iRet = 0;
	DWORD dwCur = 0;
	char szFile[MAX_PATH];
	int iDaysNo = v_objBlackSaveGps.m_bytDay / DSK2_ONEFILE_DAYS + 1;
	sprintf( szFile, "%sBlk_%02d%02d_%d.dat", m_szBlackPath, v_objBlackSaveGps.m_bytYear, v_objBlackSaveGps.m_bytMon, iDaysNo );

	// 若当前需要保存新的黑匣子文件,或者原来没有打开黑匣子文件句柄
	// 将当前缓存的黑匣子数据写入文件(如果有的话),然后关闭文件(如果打开的话),并删除旧的黑匣子文件
	if( strcmp(m_szBlackFile, szFile) != 0 || !m_pfBlack )
	{
		if( m_wBlkGpsCt > 0 && m_pfBlack )
		{
			fwrite( m_aryBlackGps, sizeof(m_aryBlackGps[0]) * m_wBlkGpsCt, 1, m_pfBlack );
			m_dwLstGpsSaveTm = GetTickCount();
			m_wBlkGpsCt = 0;
		}
		if( m_pfBlack )
		{
			fclose( m_pfBlack );
			m_pfBlack = NULL;
		}

		//g_objFlashPart4Mng.DelOldFile();
	}

	// 若黑匣子文件句柄无效,则创建当前的黑匣子文件
	if( !m_pfBlack )
	{
		m_pfBlack = fopen( szFile, "ab" );
		if( m_pfBlack )
		{
			memcpy(m_szBlackFile, szFile, sizeof(szFile));
		}
		else
		{
			memset(m_szBlackFile, 0 ,sizeof(m_szBlackFile));
			iRet = ERR_FILE_FAILED;
			perror("open blk file error:");
			goto _SAVEBLKGPS_END;
		}
	}
	
	if( m_wBlkGpsCt < sizeof(m_aryBlackGps) / sizeof(m_aryBlackGps[0]) )
	{
		m_aryBlackGps[ m_wBlkGpsCt ++ ] = v_objBlackSaveGps;
	}
	
	dwCur = GetTickCount();
	if( dwCur - m_dwLstGpsSaveTm < BLACK_SAVEINTERVAL && m_wBlkGpsCt < sizeof(m_aryBlackGps) / sizeof(m_aryBlackGps[0]) )
	{
		iRet = 0;
		goto _SAVEBLKGPS_END;
	}

	if( m_pfBlack ) 
	{
		PRTMSG(MSG_DBG, "Begin black write Tm= %d\n", GetTickCount());
		fwrite( m_aryBlackGps, sizeof(m_aryBlackGps[0]) * m_wBlkGpsCt, 1, m_pfBlack );
		fclose( m_pfBlack );
		PRTMSG(MSG_DBG, "End black write Tm= %d\n", GetTickCount());
		m_pfBlack = NULL;
		
		m_dwLstGpsSaveTm = GetTickCount();
		m_wBlkGpsCt = 0;
	}
	else
	{
		iRet = ERR_FILE_FAILED;
	}

_SAVEBLKGPS_END:
	return iRet;
}

//使用write函数写黑匣子数据，测试用
/*
int CBlk::_SaveBlackGps( const tagBlackSaveGps &v_objBlackSaveGps )
{
	int iRet = 0;
	DWORD dwCur = 0;
	char szFile[MAX_PATH];
	int iDaysNo = v_objBlackSaveGps.m_bytDay / DSK2_ONEFILE_DAYS + 1;
	sprintf( szFile, "%sBlk_%02d%02d_%d.dat", m_szBlackPath, v_objBlackSaveGps.m_bytYear, v_objBlackSaveGps.m_bytMon, iDaysNo );
	
	// 若当前需要保存新的黑匣子文件,或者原来没有打开黑匣子文件句柄
	// 将当前缓存的黑匣子数据写入文件(如果有的话),然后关闭文件(如果打开的话),并删除旧的黑匣子文件
	if( strcmp(m_szBlackFile, szFile) != 0 || !m_iBlkFile )
	{
		if( m_wBlkGpsCt > 0 && m_iBlkFile )
		{
			write(m_iBlkFile, m_aryBlackGps, sizeof(m_aryBlackGps[0]) * m_wBlkGpsCt);
			m_dwLstGpsSaveTm = GetTickCount();
			m_wBlkGpsCt = 0;
		}
		if( m_iBlkFile )
		{
			close( m_iBlkFile );
			m_iBlkFile = 0;
		}
	}
	
	// 若黑匣子文件句柄无效,则创建当前的黑匣子文件
	if( !m_iBlkFile )
	{
		m_iBlkFile = open( szFile, O_RDWR );
		if( m_iBlkFile )
		{
			memcpy(m_szBlackFile, szFile, sizeof(szFile));
		}
		else
		{
			memset(m_szBlackFile, 0 ,sizeof(m_szBlackFile));
			iRet = ERR_FILE_FAILED;
			perror("open blk file error:");
			goto _SAVEBLKGPS_END;
		}
	}
	
	if( m_wBlkGpsCt < sizeof(m_aryBlackGps) / sizeof(m_aryBlackGps[0]) )
	{
		m_aryBlackGps[ m_wBlkGpsCt ++ ] = v_objBlackSaveGps;
	}
	
	dwCur = GetTickCount();
	if( dwCur - m_dwLstGpsSaveTm < BLACK_SAVEINTERVAL && m_wBlkGpsCt < sizeof(m_aryBlackGps) / sizeof(m_aryBlackGps[0]) )
	{
		iRet = 0;
		goto _SAVEBLKGPS_END;
	}
	
	if( m_iBlkFile ) 
	{
		PRTMSG(MSG_DBG, "begin blk write = %d\n", GetTickCount());
		write(m_iBlkFile,  m_aryBlackGps, sizeof(m_aryBlackGps[0]) * m_wBlkGpsCt);
		close( m_iBlkFile );
		PRTMSG(MSG_DBG, "end blk write = %d\n", GetTickCount());
		m_iBlkFile = 0;
		
		m_dwLstGpsSaveTm = GetTickCount();
		m_wBlkGpsCt = 0;
	}
	else
	{
		iRet = ERR_FILE_FAILED;
	}
	
_SAVEBLKGPS_END:
	return iRet;
}
*/

// 删除指定GPS时间之后的黑匣子数据
void CBlk::DelLaterBlk( const tagGPSData& v_objGps )
{
	if( 'A' != v_objGps.valid ) return;
	
	char szNowFile[32];
	char szCommand[100];
	struct dirent **namelist;
    int n;
	char szTempbuf[4];
	
	sprintf( szNowFile, "Blk_%02d%02d_%d.dat", v_objGps.nYear % 1000, v_objGps.nMonth, v_objGps.nDay / DSK2_ONEFILE_DAYS + 1 );
	
	n = scandir(m_szBlackPath, &namelist, 0, alphasort); // 返回值会等于0吗？
    if (n < 0)
        perror("scandir");
	else 
    {
        while(n--) 
        {
			memcpy(szTempbuf, namelist[n]->d_name, 4);

			if( 0 != strcmp("Blk_", szTempbuf) || 0 == strcmp(".", namelist[n]->d_name) || 0 == strcmp("..", namelist[n]->d_name) )
				continue;
			
			if( 0 > strcmp(szNowFile, namelist[n]->d_name) )
			{
				sprintf(szCommand, "rm %s%s", m_szBlackPath, namelist[n]->d_name);
				system(szCommand);
				PRTMSG(MSG_DBG, "Delete file %s%s\n", m_szBlackPath, namelist[n]->d_name);
			}
            free(namelist[n]);
        }
        free(namelist);
    }
}

// 删除指定GPS时间之前1年的黑匣子数据
void CBlk::DelOlderBlk( const tagGPSData& v_objGps )
{
	if( 'A' != v_objGps.valid ) return;
	
	// 转变为1年前的GPS时间
	tagGPSData objGpsJug = v_objGps;
	objGpsJug.nYear --;
	if( objGpsJug.nYear < 2008 )
		objGpsJug.nYear = 2008;
	
	char szCommand[100];
	struct dirent **namelist;
    int n;
	char szTempbuf[4];
	
	char szJugFile[32];
	sprintf( szJugFile, "Blk_%02d%02d_%d.dat", objGpsJug.nYear % 1000, objGpsJug.nMonth, objGpsJug.nDay / DSK2_ONEFILE_DAYS );
	
	n = scandir(m_szBlackPath, &namelist, 0, alphasort);
    if (n < 0)
        perror("scandir");
	else 
    {
        while(n--) 
        {
			memcpy(szTempbuf, namelist[n]->d_name, 4);
			
			if( 0 != strcmp("Blk_", szTempbuf) || 0 == strcmp(".", namelist[n]->d_name) || 0 == strcmp("..", namelist[n]->d_name) )
				continue;
			
			if( 0 < strcmp(szJugFile, namelist[n]->d_name) )
			{
				sprintf(szCommand, "rm %s%s", m_szBlackPath, namelist[n]->d_name);
				system(szCommand);
				PRTMSG(MSG_DBG, "Delete file %s%s\n", m_szBlackPath, namelist[n]->d_name);
			}
            free(namelist[n]);
        }
        free(namelist);
    }
}

void CBlk::_GpsToBlackSaveGps( const tagGPSData &v_objGps, tagBlackSaveGps &v_objBlackSaveGps )
{
	DWORD dwVeSta = g_objCarSta.GetVeSta();

	v_objBlackSaveGps.m_bytDay = BYTE(v_objGps.nDay);
	v_objBlackSaveGps.m_bytMon = BYTE(v_objGps.nMonth);
	v_objBlackSaveGps.m_bytYear = BYTE(v_objGps.nYear % 1000);
	v_objBlackSaveGps.m_bytSec = BYTE(v_objGps.nSecond);
	v_objBlackSaveGps.m_bytMin = BYTE(v_objGps.nMinute);
	v_objBlackSaveGps.m_bytHour = BYTE(v_objGps.nHour);
	
	v_objBlackSaveGps.m_bytMix = 0x40;

	// 空车/重车
	if( dwVeSta & VESTA_HEAVY )
		v_objBlackSaveGps.m_bytMix |= 0x04;
	else
		v_objBlackSaveGps.m_bytMix &= 0xfb;

	// 省电/非省电
 	if( g_objQianIO.GetDeviceSta() & DEVSTA_SYSTEMSLEEP )
 		v_objBlackSaveGps.m_bytMix |= 0x10;
 	else
 		v_objBlackSaveGps.m_bytMix &= 0xef;

	// ACC状态
	if( g_objQianIO.GetDeviceSta() & DEVSTA_ACCVALID )
		v_objBlackSaveGps.m_bytMix |= 0x08;	
	else
		v_objBlackSaveGps.m_bytMix &= 0xf7;

	// GPS是否有效
	if( 'A' == v_objGps.valid ) v_objBlackSaveGps.m_bytMix &= 0xdf;
	else v_objBlackSaveGps.m_bytMix |= 0x20;

	v_objBlackSaveGps.m_lLon = long(v_objGps.longitude * LONLAT_DO_TO_L);
	v_objBlackSaveGps.m_lLat = long(v_objGps.latitude * LONLAT_DO_TO_L);

	double dSpeed = v_objGps.speed * 3.6 / 1.852; // 转换为海里/小时
	v_objBlackSaveGps.m_bytSpeed = BYTE( dSpeed );
}

void CBlk::_BlackSaveGpsToGps( const tagBlackSaveGps &v_objBlkSaveGps, tagGPSData &v_objGps )
{
	v_objGps.nYear = v_objBlkSaveGps.m_bytYear + 2000;
	v_objGps.nMonth = v_objBlkSaveGps.m_bytMon;
	v_objGps.nDay = v_objBlkSaveGps.m_bytDay;
	v_objGps.nHour = v_objBlkSaveGps.m_bytHour;
	v_objGps.nMinute = v_objBlkSaveGps.m_bytMin;
	v_objGps.nSecond = v_objBlkSaveGps.m_bytSec;
	v_objGps.longitude = v_objBlkSaveGps.m_lLon / LONLAT_DO_TO_L;
	v_objGps.latitude = v_objBlkSaveGps.m_lLat / LONLAT_DO_TO_L;
	v_objGps.speed = float( v_objBlkSaveGps.m_bytSpeed * 1.852 / 3.6 );
	v_objGps.direction = 0;
}

void CBlk::_BlackSaveGpsToBlackGps( tagBlackSaveGps &v_objBlackSaveGps, tag0e60BlackGps &v_objBlackGps )
{
	v_objBlackGps.m_bytMix = v_objBlackSaveGps.m_bytMix;
	
	v_objBlackGps.m_bytDay = v_objBlackSaveGps.m_bytDay;
	v_objBlackGps.m_bytMon = v_objBlackSaveGps.m_bytMon;
	v_objBlackGps.m_bytYear = v_objBlackSaveGps.m_bytYear;
	v_objBlackGps.m_bytSec = v_objBlackSaveGps.m_bytSec;
	v_objBlackGps.m_bytMin = v_objBlackSaveGps.m_bytMin;
	v_objBlackGps.m_bytHour = v_objBlackSaveGps.m_bytHour;
	v_objBlackGps.m_bytSpeed = v_objBlackSaveGps.m_bytSpeed;

	// 纬度
	double dLat = v_objBlackSaveGps.m_lLat / LONLAT_DO_TO_L;
	v_objBlackGps.m_bytLatDu = BYTE(dLat);
	double dLatFen = ( dLat - v_objBlackGps.m_bytLatDu ) * 60;
	v_objBlackGps.m_bytLatFenzen = BYTE(dLatFen);
	WORD wLatFenxiao = WORD( (dLatFen - v_objBlackGps.m_bytLatFenzen) * 10000 );
	v_objBlackGps.m_bytLatFenxiao1 = wLatFenxiao / 100;
	v_objBlackGps.m_bytLatFenxiao2 = wLatFenxiao % 100;

	// 经度
	double dLon = v_objBlackSaveGps.m_lLon / LONLAT_DO_TO_L;
	v_objBlackGps.m_bytLonDu = BYTE(dLon);
	double dLonFen = (dLon - v_objBlackGps.m_bytLonDu) * 60;
	v_objBlackGps.m_bytLonFenzen = BYTE(dLonFen);
	WORD wLonFenxiao = WORD( (dLonFen - v_objBlackGps.m_bytLonFenzen) * 10000 );
	v_objBlackGps.m_bytLonFenxiao1 = wLonFenxiao / 100;
	v_objBlackGps.m_bytLonFenxiao2 = wLonFenxiao % 100;

	// 转义
	if( 0 == v_objBlackGps.m_bytLatDu ) v_objBlackGps.m_bytLatDu = 0x7f;
	if( 0 == v_objBlackGps.m_bytLatFenzen ) v_objBlackGps.m_bytLatFenzen = 0x7f;
	if( 0 == v_objBlackGps.m_bytLatFenxiao1 ) v_objBlackGps.m_bytLatFenxiao1 = 0x7f;
	if( 0 == v_objBlackGps.m_bytLatFenxiao2 ) v_objBlackGps.m_bytLatFenxiao2 = 0x7f;
	if( 0 == v_objBlackGps.m_bytLonFenzen ) v_objBlackGps.m_bytLonFenzen = 0x7f;
	if( 0 == v_objBlackGps.m_bytLonFenxiao1 ) v_objBlackGps.m_bytLonFenxiao1 = 0x7f;
	if( 0 == v_objBlackGps.m_bytLonFenxiao2 ) v_objBlackGps.m_bytLonFenxiao2 = 0x7f;
	if( 0 == v_objBlackGps.m_bytLonDu ) v_objBlackGps.m_bytLonDu = 0xff;
	else if( 0x80 == v_objBlackGps.m_bytLonDu ) v_objBlackGps.m_bytLonDu = 0xfe;
	if( v_objBlackGps.m_bytLonDu & 0x80 ) v_objBlackGps.m_bytMix |= 0x02;
	else v_objBlackGps.m_bytMix &= 0xfd;
	v_objBlackGps.m_bytLonDu &= 0x7f;
	if( 0 == v_objBlackGps.m_bytSpeed ) v_objBlackGps.m_bytSpeed = 0xff;
	else if( 0x80 == v_objBlackGps.m_bytSpeed ) v_objBlackGps.m_bytSpeed = 0xfe;
	if( v_objBlackGps.m_bytSpeed & 0x80 ) v_objBlackGps.m_bytMix |= 0x01;
	else v_objBlackGps.m_bytMix &= 0xfe;
	v_objBlackGps.m_bytSpeed &= 0x7f;
}

bool CBlk::_GetPeriodFromBlkFileName( char* v_szBlkFileName, BYTE& v_bytYear, BYTE& v_bytMon, BYTE& v_bytDayNo )
{
	if( 14 != strlen(v_szBlkFileName) ) return false;
	
	char buf[ 20 ] = { 0 };

	memcpy( buf, v_szBlkFileName + 4, 2 );
	buf[ 2 ] = 0;
	v_bytYear = BYTE( atoi( buf ));
	if( 0 == v_bytYear || v_bytYear > 50 ) return false;

	memcpy( buf, v_szBlkFileName + 6, 2 );
	v_bytMon = BYTE( atoi( buf ));
	if( 0 == v_bytMon || v_bytMon > 12 ) return false;

	buf[ 0 ] = v_szBlkFileName[ 9 ];
	buf[ 1 ] = 0;
	v_bytDayNo = BYTE( atoi( buf ));
	if( v_bytDayNo < 1 || v_bytDayNo > 31 / DSK2_ONEFILE_DAYS + 1 ) return false;

	return true;
}

/// 黑匣子查询请求
int CBlk::Deal0e20( char* v_szData, DWORD v_dwDataLen )
{
// 千里眼通用版，公开版_研三
// 监控中心分组号(10)+允许最大传输帧个数（1）+ 查询时间段个数（1）
// + [ 查询时间段(10) ] x N +查询位置范围个数(1)
// + [ 位置范围(16)] x N +查询数据总帧数（1）+当前帧序号（1）

// 千里眼公开版_研一
// 允许最大传输帧个数（1）+ 查询时间段个数（1）+ [ 查询时间段1 + …… + 查询时间段n]

	char buf[ 200 ] = { 0 };
	int iBufLen = 0;
	DWORD dwCt = 0;
	BYTE bytPeriCount = 0, bytAreaCount = 0;
	BYTE bytReqFrameNo = 0;
	BYTE byt = 0;
	int iRet = 0;
	tag0e20 req;
	DWORD dwBaseNo = 0;
	DWORD dwBaseLen = 0;

#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	dwBaseNo = 10;
	dwBaseLen = 15;
#endif
#if 2 == USE_PROTOCOL
	dwBaseNo = 0;
	dwBaseLen = 2;
#endif

	// 参数长度检查
	if( v_dwDataLen > sizeof(tag0e20) || v_dwDataLen < dwBaseLen )
		return ERR_DATA_INVALID; // 若输入数据超过最大可能个数,或者小于最小可能个数
	memcpy( &req, v_szData, v_dwDataLen );

#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	/// 保存监控中心分组号
	if( strncmp(req.m_szCentGroup, m_objBlackQuerySta.m_szCentGroup, sizeof(req.m_szCentGroup)) )
	{
		memset((void*)&m_objBlackQuerySta, 0, sizeof(m_objBlackQuerySta) ); // 若是新的分组，先清除原有的查询状态
		memcpy( m_objBlackQuerySta.m_szCentGroup, req.m_szCentGroup, sizeof(m_objBlackQuerySta.m_szCentGroup) );
	}
	memset((void*)&m_objBlackQuerySta, 0, sizeof(m_objBlackQuerySta) ); // 先清除原有的查询状态
#endif

#if 2 == USE_PROTOCOL
	memset((void*)&m_objBlackQuerySta, 0, sizeof(m_objBlackQuerySta) ); // 先清除原有的查询状态
#endif

	// 提取配置
	tag2QGprsCfg objGprsCfg;
	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	// 关闭黑匣子数据发送定时 (确保收到黑匣子查询帧后,关闭正在进行的黑匣子数据发送)
	_KillTimer(&g_objTimerMng, BLACK_SNDRES_TIMER );

	dwCt = dwBaseNo;

	/// 保存上传总帧数,并清除上传计数
	m_objBlackQuerySta.m_bytUploadFrameTotal = BYTE( v_szData[dwCt] );
	m_objBlackQuerySta.m_bytUploadFrameCt = 0;
	if( m_objBlackQuerySta.m_bytUploadFrameTotal < 1 ) m_objBlackQuerySta.m_bytUploadFrameTotal = 1;
	if( m_objBlackQuerySta.m_bytUploadFrameTotal > 100 ) m_objBlackQuerySta.m_bytUploadFrameTotal = 100;

	/// 检验时间段条件
	bytPeriCount = req.m_bytPeriodCount;
	if( 0x7f == bytPeriCount ) bytPeriCount = 0;
	if( bytPeriCount > 5 )
	{
		iRet = ERR_DATA_INVALID;
		goto DEAL0E20_FAILED;
	}
	dwCt += 2 + bytPeriCount * 10;

	// 时间段条件个数确定后,判断帧是否完整
	if( dwCt > v_dwDataLen )
	{
		iRet = ERR_DATA_INVALID;
		goto DEAL0E20_FAILED;
	}
	
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	/// 检验区域条件
	bytAreaCount = BYTE( v_szData[dwCt] );
	if( 0x7f == bytAreaCount ) bytAreaCount = 0;
	if( bytAreaCount > 20 )
	{
		iRet = ERR_DATA_INVALID;
		goto DEAL0E20_FAILED;
	}
	dwCt += 1 + 16 * bytAreaCount;
	if( dwCt + 2 != v_dwDataLen ) // 区域条件个数确定后,判断帧是否完整
	{
		iRet = ERR_DATA_INVALID;
		goto DEAL0E20_FAILED;
	}

	// 查询帧总帧数
	m_objBlackQuerySta.m_bytReqFrameTotal = BYTE( v_szData[dwCt] );

	// 查询帧序号
	bytReqFrameNo = BYTE(v_szData[dwCt + 1]);
#endif

#if 2 == USE_PROTOCOL
	// 区域条件个数
	bytAreaCount = 0;
	
	// 查询帧总帧数
	m_objBlackQuerySta.m_bytReqFrameTotal = 1;

	// 查询帧序号
	bytReqFrameNo = 1;
#endif

	{
		if( bytReqFrameNo <= sizeof(m_objBlackQuerySta.m_aryReqFrameSymb) && bytReqFrameNo > 0 ) // 保证不会非法访问内存
		{
			if( 0 == m_objBlackQuerySta.m_aryReqFrameSymb[ bytReqFrameNo - 1 ] ) // 若该帧未收到过
			{
				m_objBlackQuerySta.m_bytReqFrameCount ++;
				m_objBlackQuerySta.m_aryReqFrameSymb[ bytReqFrameNo - 1 ] = 1; // 以后将不再重复处理该帧

				/// 增加时间段条件
				if( bytPeriCount > 0 )
				{
					BYTE bytDoneCount = m_objBlackQuerySta.m_bytPeriodCount; // 已保存的时间段个数
					BYTE bytTotal = BYTE(sizeof(m_objBlackQuerySta.m_aryPeriod) / sizeof(m_objBlackQuerySta.m_aryPeriod[0])); // 可保存的时间段总数
					if( bytTotal < bytDoneCount )
					{
						iRet = ERR_INNERDATA_ERR;
						goto DEAL0E20_FAILED;
					}
					BYTE bytCanDealCount = bytTotal - bytDoneCount;
					bytCanDealCount = bytCanDealCount > bytPeriCount ? bytPeriCount : bytCanDealCount; // 本次还可以保存的时间段个数

					if( bytCanDealCount > 0 )
					{
						for( byt = 0; byt < bytCanDealCount; byt ++ )
						{
							memcpy( &(m_objBlackQuerySta.m_aryPeriod[ bytDoneCount + byt ]),
								v_szData + dwBaseNo + 2 + byt * 10, sizeof(m_objBlackQuerySta.m_aryPeriod[0]) );

							// 07.08.01 bug,没有将7f转义为0,造成后面查找当天满足时间段条件数据的错误,modified by xun
							if( 0x7f == m_objBlackQuerySta.m_aryPeriod[ bytDoneCount + byt ].m_bytFromHour )
								m_objBlackQuerySta.m_aryPeriod[ bytDoneCount + byt ].m_bytFromHour = 0;
							if( 0x7f == m_objBlackQuerySta.m_aryPeriod[ bytDoneCount + byt ].m_bytFromMin )
								m_objBlackQuerySta.m_aryPeriod[ bytDoneCount + byt ].m_bytFromMin = 0;
							if( 0x7f == m_objBlackQuerySta.m_aryPeriod[ bytDoneCount + byt ].m_bytToHour )
								m_objBlackQuerySta.m_aryPeriod[ bytDoneCount + byt ].m_bytToHour = 0;
							if( 0x7f == m_objBlackQuerySta.m_aryPeriod[ bytDoneCount + byt ].m_bytToMin )
								m_objBlackQuerySta.m_aryPeriod[ bytDoneCount + byt ].m_bytToMin = 0;
						}
					}

					m_objBlackQuerySta.m_bytPeriodCount += bytCanDealCount; // 更新时间段计数
				}

				/// 增加区域条件
				if( bytAreaCount > 0 )
				{
					BYTE bytDoneCount = m_objBlackQuerySta.m_bytAreaCount; // 已保存的区域个数
					BYTE bytTotal = BYTE(sizeof(m_objBlackQuerySta.m_aryArea) / sizeof(m_objBlackQuerySta.m_aryArea[0])); // 可保存的区域总数
					if( bytTotal < bytDoneCount )
					{
						iRet = ERR_INNERDATA_ERR;
						goto DEAL0E20_FAILED;
					}
					BYTE bytCanDealCount = bytTotal - bytDoneCount;
					bytCanDealCount = bytCanDealCount > bytAreaCount ? bytAreaCount : bytCanDealCount; // 本次还可保存的区域个数

					if( bytCanDealCount > 0 )
					{
						char* szArea;
						for( byt = 0; byt < bytCanDealCount; byt ++ )
						{
							szArea = v_szData + dwBaseNo + 2 + bytPeriCount * 10 + 1 + byt * 16;
							m_objBlackQuerySta.m_aryArea[ bytDoneCount + byt ].m_lLatMin
								= long( ( szArea[0] + ( szArea[1] + (szArea[2]*0.01 + szArea[3]*0.0001) ) / 60.0 ) * LONLAT_DO_TO_L );
							m_objBlackQuerySta.m_aryArea[ bytDoneCount + byt ].m_lLonMin
								= long( ( szArea[4] + ( szArea[5] + (szArea[6]*0.01 + szArea[7]*0.0001) ) / 60.0 ) * LONLAT_DO_TO_L );
							m_objBlackQuerySta.m_aryArea[ bytDoneCount + byt ].m_lLatMax
								= long( ( szArea[8] + ( szArea[9] + (szArea[10]*0.01 + szArea[11]*0.0001) ) / 60.0 ) * LONLAT_DO_TO_L );
							m_objBlackQuerySta.m_aryArea[ bytDoneCount + byt ].m_lLonMax
								= long( ( szArea[12] + ( szArea[13] + (szArea[14]*0.01 + szArea[15]*0.0001) ) / 60.0 ) * LONLAT_DO_TO_L );
						}
					}

					m_objBlackQuerySta.m_bytAreaCount += bytCanDealCount; // 更新区域条件计数
				}
			}
		}
	}

	if( m_objBlackQuerySta.m_bytReqFrameTotal <= m_objBlackQuerySta.m_bytReqFrameCount ) // 若查询帧已接收完毕
	{
		// 关闭接收黑匣子查询条件定时
		_KillTimer(&g_objTimerMng, BLACK_RCVQUERY_TIMER );
		
		// 清除旧的发送状态信息
		memset((void*)&m_objBlackSndSta, 0, sizeof(m_objBlackSndSta) );
		memset((void*)&m_obj0e60, 0, sizeof(m_obj0e60) );
		m_i0e60Len = 0;

		// 提取黑匣子文件名
		struct dirent **namelist;
		int n = 0;
		
		n = scandir(m_szBlackPath, &namelist, 0, alphasort);
		if (n < 0)
		{
			perror("scandir");
			iRet = ERR_FILE;
			goto DEAL0E20_FAILED;
		}
		else 
		{
			WORD wFileCount = 0;
			WORD wFileMaxTotal = sizeof(m_objBlackSndSta.m_szBlackFile) / sizeof(m_objBlackSndSta.m_szBlackFile[0]) ;
			
			while(n--) 
			{
				if( 0 == strcmp(".", namelist[n]->d_name) || 0 == strcmp("..", namelist[n]->d_name) ) continue;	
				
				if( 14 != strlen(namelist[n]->d_name) )
					continue;
				
				if( wFileCount >= wFileMaxTotal )
					break;
				
				memcpy(m_objBlackSndSta.m_szBlackFile[wFileCount], namelist[n]->d_name, strlen(namelist[n]->d_name) );

				wFileCount++;
			}
			free(namelist);
			
			// 记录黑匣子文件个数
			m_objBlackSndSta.m_wFileTotal = wFileCount > wFileMaxTotal ? wFileMaxTotal : wFileCount; 
		}

		// 按照时间排序文件名
		qsort( m_objBlackSndSta.m_szBlackFile, m_objBlackSndSta.m_wFileTotal, 
			sizeof(m_objBlackSndSta.m_szBlackFile[0]), SortStr );

		// 按照起始时间排序时间段条件
		if( m_objBlackQuerySta.m_bytPeriodCount >= 2 )
		{
			qsort( m_objBlackQuerySta.m_aryPeriod, m_objBlackQuerySta.m_bytPeriodCount,
				sizeof(m_objBlackQuerySta.m_aryPeriod[0]), SortBlackPeriod );
		}

		// 当前的黑匣子数据,从缓存保存到磁盘上
		if( m_pfBlack )
		{
			if( m_wBlkGpsCt > 0 )
			{
				fwrite( m_aryBlackGps, sizeof(m_aryBlackGps[0]) * m_wBlkGpsCt, 1, m_pfBlack );
				fclose( m_pfBlack );
				m_pfBlack = NULL;
				
				m_dwLstGpsSaveTm = GetTickCount();
				m_wBlkGpsCt = 0;
			}
			else
				fflush( m_pfBlack );
		}

		// 打开黑匣子数据发送定时
		_SetTimer(&g_objTimerMng, BLACK_SNDRES_TIMER, BLACK_SNDINTERVAL, G_TmBlkSndRes );
	}
	else
	{
		// 重置接收黑匣子查询条件定时
		_SetTimer(&g_objTimerMng, BLACK_RCVQUERY_TIMER, BLACKQUERY_MAXINTERVAL, G_TmBlkRcvQuery );
	}

	return 0;

DEAL0E20_FAILED:
	tag0e60 res;
	BYTE bytDataType = 0;
#if 0 == USE_PROTOCOL || 1 == USE_PROTOCOL
	memcpy( &res.m_szCentGroup, m_objBlackQuerySta.m_szCentGroup, sizeof(res.m_szCentGroup) );
	bytDataType = 0x60;
#endif
#if 2 == USE_PROTOCOL
	bytDataType = 0x50;
#endif
	res.m_bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame((char*)&res, 11, 0x0e, bytDataType, buf, sizeof(buf), iBufLen);
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12,
		(objGprsCfg.m_bytChannelBkType_2 & 0x40) ? DATASYMB_SMSBACKUP : 0 );
	return iRet;
}

/// 业务处理优先
int CBlk::Deal0e21( char* v_szData, DWORD v_dwDataLen )
{
	char buf[ SOCK_MAXSIZE ];
	int iBufLen = 0;

	tag2QGprsCfg objGprsCfg;
	GetSecCfg( &objGprsCfg, sizeof(objGprsCfg),	offsetof(tagSecondCfg, m_uni2QGprsCfg.m_obj2QGprsCfg), sizeof(objGprsCfg) );

	tag0e61 res;
	memcpy( &res.m_szCentGroup, v_szData, sizeof(res.m_szCentGroup) );
	res.m_bytResType = 0x01;
	int iRet = g_objSms.MakeSmsFrame((char*)&res, sizeof(res), 0x0e, 0x61, buf, sizeof(buf), iBufLen);
	if( iRet ) return iRet;
	iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12,
		(objGprsCfg.m_bytChannelBkType_2 & 0x40) ? DATASYMB_SMSBACKUP : 0 );
	if( iRet ) return iRet;

	memset((void*)&m_objBlackQuerySta, 0, sizeof(m_objBlackQuerySta) );
	memset((void*)&m_objBlackSndSta, 0, sizeof(m_objBlackSndSta) );
	memset((void*) &m_obj0e60, 0, sizeof(m_obj0e60) );
	m_i0e60Len = 0;

	_KillTimer(&g_objTimerMng, BLACK_SNDRES_TIMER );
	_KillTimer(&g_objTimerMng, BLACK_RCVQUERY_TIMER );

	g_objTcpSendMng.DelSpecData( DATASYMB_OE60 ); // 删除队列里还未发送的黑匣子数据

	return 0;
}

/// 黑匣子间隔设置 （应答成功优先）
int CBlk::Deal1012( char* v_szSrcHandtel, char *v_szData, DWORD v_dwDataLen, bool v_bFromSM )
{
	tag2QBlkCfg objBlkCfg, objBlkCfgBkp;
	tag1012 req;
	int iRet = 0;
	int iBufLen = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytResType;

	// 参数检查
	if( sizeof(tag1012) != v_dwDataLen )
	{
		iRet = ERR_DATA_INVALID;
		goto DEAL1012_FAILED;
	}

	// 获取原始参数
	iRet = GetSecCfg( &objBlkCfg, sizeof(objBlkCfg), offsetof(tagSecondCfg, m_uni2QBlkCfg.m_obj2QBlkCfg), sizeof(objBlkCfg) );
	if( iRet ) goto DEAL1012_FAILED;
	objBlkCfgBkp = objBlkCfg; // 先备份

	// 解帧,更新设置
	memcpy( &req, v_szData, sizeof(req) );
	if( 0x7f == req.m_bytMin ) req.m_bytMin = 0;
	if( 0x7f == req.m_bytSec ) req.m_bytSec = 0;
	objBlkCfg.m_dwBlackChkInterv = req.m_bytMin * 60 + req.m_bytSec;
	if( objBlkCfg.m_dwBlackChkInterv < BLACK_MININTERVAL )
		objBlkCfg.m_dwBlackChkInterv = BLACK_MININTERVAL;

	// 组帧
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x52, buf, sizeof(buf), iBufLen);
	if( !iRet )
	{
		// 保存新设置
		if( iRet = SetSecCfg(&objBlkCfg, offsetof(tagSecondCfg,m_uni2QBlkCfg.m_obj2QBlkCfg),
			sizeof(objBlkCfg)) )
		{
			goto DEAL1012_FAILED;
		}

		// 发送应答
		iRet = g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );
		if( !iRet )
		{
			SaveCurBlkGps();
		}
		else
		{
			SetSecCfg(&objBlkCfgBkp, offsetof(tagSecondCfg,m_uni2QBlkCfg.m_obj2QBlkCfg), sizeof(objBlkCfgBkp)); // 恢复原设置
			goto DEAL1012_FAILED;
		}
	}
	else goto DEAL1012_FAILED;

	return 0;

DEAL1012_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x10, 0x52, buf, sizeof(buf), iBufLen);
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, v_bFromSM ? DATASYMB_USESMS : 0, v_bFromSM ? v_szSrcHandtel : NULL );

	return iRet;
}

#endif



