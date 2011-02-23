#include "yx_QianStdAfx.h"

#if USE_LIAONING_SANQI == 1

#undef MSG_HEAD
#define MSG_HEAD	("QianExe-LN_ComFunc")

int G_LocalToNet(char* v_szSrcBuf, char *v_szDesBuf, int v_iLen)
{
	if( v_szDesBuf == NULL || v_szDesBuf == NULL || v_iLen <=0 )
		return ERR_PAR;

	for(int i=0; i<v_iLen; i++)
	{
		v_szDesBuf[i] = v_szSrcBuf[v_iLen-i-1];
	}

	return 0;
}


int G_GetPoint(char *v_szBuf)
{
	char    szBuf[1024] = {0};
	DWORD   dwLen = 0;

	tagGPSData objGps(0);
	g_objMonAlert.GetCurGPS( objGps, true );

	tagQianGps objQianGps;
	g_objMonAlert.GpsToQianGps(objGps, objQianGps);

	szBuf[dwLen++] = 0x10;	// Attr
	szBuf[dwLen++] = 0x02;	// K_len
	szBuf[dwLen++] = 'p';	// Key: pI 位置信息
	szBuf[dwLen++] = 'I';
	szBuf[dwLen++] = 0x00;	// V-len
	szBuf[dwLen++] = 0x25;

	// 定位类型
	szBuf[dwLen++] = 0x00;	// Attr
	szBuf[dwLen++] = 0x01;	// K-len
	szBuf[dwLen++] = 't';	// Key: t
	szBuf[dwLen++] = 0x00;	// V-len
	szBuf[dwLen++] = 0x01;  
	szBuf[dwLen] = 0;
	byte bytSta;
	IOGet(IOS_ACC, &bytSta);    
	if( IO_ACC_OFF == bytSta )	szBuf[dwLen] |= 0x40;	// 省电位
	if( 'A' == objGps.valid)	szBuf[dwLen] |= 0x10;	// GPS有效位
	dwLen += 1;
	// 北纬、东经、偏移加密位均为0

	// 纬度
	szBuf[dwLen++] = 0x00;	// Attr
	szBuf[dwLen++] = 0x03;	// K-len
	szBuf[dwLen++] = 'l';	// Key: lat
	szBuf[dwLen++] = 'a';
	szBuf[dwLen++] = 't';
	szBuf[dwLen++] = 0x00;	// V-len
	szBuf[dwLen++] = 0x04;  
	DWORD dwLat = (DWORD)(objGps.latitude * 60 * 60 * 1000);	// 度 －> 毫秒
	G_LocalToNet((char*)&dwLat, szBuf+dwLen, 4);
	dwLen += 4;

	// 经度
	szBuf[dwLen++] = 0x00;	// Attr
	szBuf[dwLen++] = 0x03;	// K-len
	szBuf[dwLen++] = 'l';	// Key: lon
	szBuf[dwLen++] = 'o';
	szBuf[dwLen++] = 'n';
	szBuf[dwLen++] = 0x00;	// V-len
	szBuf[dwLen++] = 0x04;
	DWORD dwLon = (DWORD)(objGps.longitude * 60 * 60 * 1000);	// 度 －> 毫秒
	G_LocalToNet((char*)&dwLon, szBuf+dwLen, 4);
	dwLen += 4;

	// 海拔
	szBuf[dwLen++] = 0x00;	// Attr
	szBuf[dwLen++] = 0x03;	// K-len
	szBuf[dwLen++] = 'a';	// Key: lon
	szBuf[dwLen++] = 'l';
	szBuf[dwLen++] = 't';
	szBuf[dwLen++] = 0x00;	// V-len
	szBuf[dwLen++] = 0x02;  
	szBuf[dwLen++] = 0x00;	// 海拔置0
	szBuf[dwLen++] = 0x00;

	// 速度
	szBuf[dwLen++] = 0x00;	// Attr4
	szBuf[dwLen++] = 0x01;	// K_len4
	szBuf[dwLen++] = 's';	// Key4: s 速度
	szBuf[dwLen++] = 0x00;	// V_len4
	szBuf[dwLen++] = 0x01;
	szBuf[dwLen++] = (byte)(objGps.speed * 3.6);

	// 方向
	szBuf[dwLen++] = 0x00;	// Attr5
	szBuf[dwLen++] = 0x01;	// K_len5
	szBuf[dwLen++] = 'd';	// Key5: s 方向
	szBuf[dwLen++] = 0x00;	// V_len5
	szBuf[dwLen++] = 0x02;
	szBuf[dwLen++] = ((byte)(objGps.direction))/256;
	szBuf[dwLen++] = ((byte)(objGps.direction))%256;

	// 里程
	szBuf[dwLen++] = 0x00;	// Attr6
	szBuf[dwLen++] = 0x02;	// K_len6
	szBuf[dwLen++] = 'm';	// Key6: me 里程
	szBuf[dwLen++] = 'e';
	szBuf[dwLen++] = 0x00;	// V_len6
	szBuf[dwLen++] = 0x04;
	szBuf[dwLen++] = (byte)(htonl(g_objDriveRecord.Mile) & 0xff);	// 里程先暂时全部置为0
	szBuf[dwLen++] = (byte)((htonl(g_objDriveRecord.Mile) >> 8) & 0xff);
	szBuf[dwLen++] = (byte)((htonl(g_objDriveRecord.Mile) >> 16) & 0xff);
	szBuf[dwLen++] = (byte)((htonl(g_objDriveRecord.Mile) >> 24) & 0xff);

	memcpy(v_szBuf, szBuf, dwLen);

	return dwLen;
}

int G_GetDEV_ID(char *v_szBuf)
{
	tag1PComuCfg objComuCfg;
	GetImpCfg( &objComuCfg, sizeof(objComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(objComuCfg) ); 

	char tmp = 0;

	v_szBuf[0] = 0xff;
	v_szBuf[1] = 0xff;

	v_szBuf[2] = 0xf0;
	tmp = objComuCfg.m_szTel[0] - 0x30;
	v_szBuf[2] += tmp;

	tmp = (objComuCfg.m_szTel[1] - 0x30) << 4;
	v_szBuf[3] = tmp;
	tmp = objComuCfg.m_szTel[2] - 0x30;
	v_szBuf[3] += tmp;
	tmp = (objComuCfg.m_szTel[3] - 0x30) << 4;
	v_szBuf[4] = tmp;
	tmp = objComuCfg.m_szTel[4] - 0x30;
	v_szBuf[4] += tmp;
	tmp = (objComuCfg.m_szTel[5] - 0x30) << 4;
	v_szBuf[5] = tmp;
	tmp = objComuCfg.m_szTel[6] - 0x30;
	v_szBuf[5] += tmp;
	tmp = (objComuCfg.m_szTel[7] - 0x30) << 4;
	v_szBuf[6] = tmp;
	tmp = objComuCfg.m_szTel[8] - 0x30;
	v_szBuf[6] += tmp;
	tmp = (objComuCfg.m_szTel[9] - 0x30) << 4;
	v_szBuf[7] = tmp;
	tmp = objComuCfg.m_szTel[10] - 0x30;
	v_szBuf[7] += tmp;

	return 8;
}

// 获取从2000年1月1日0点0分0秒（格林威治时间）起至今的秒数
DWORD G_GetSecCnt()
{
	// 从1970年1月1日至2000年1月1日的秒数
	DWORD dwSecInterval = (30*365+7)*24*60*60;  

	// 当前秒数减去dwSecInterval，即是从2000年1月1日起至今的秒数
	DWORD dwSecCnts =  (DWORD)( time((time_t*)NULL) - dwSecInterval);

	// 因为车台是以北京时间作为UTC时间，所以要再减去8小时（现在是以格林威治时间，所以不用减了）
	// dwSecCnts -= 8*60*60;

	return dwSecCnts;
}

short CaculateCRC(byte *msg, int len)
{
	short crc = (short) 0xFFFF; 
	int i, j;
	unsigned char c15, bit;

	for(i = 0; i < len; i++)
	{
		for(j = 0; j < 8; j++)
		{
			c15 = ((crc >> 15 & 1) == 1);

			bit = ((msg[i] >> (7 - j) & 1) == 1);

			crc <<= 1;

			if(c15 ^ bit)
				crc ^= 0x1021;
		}
	}
	return crc;
}

bool CheckCRC(byte *buf, int len)
{
	short crc1 = CaculateCRC(buf+2, len-2);

	short crc2 = 0;
	byte tmpbuf[2] = {0};

	tmpbuf[0] = buf[1];
	tmpbuf[1] = buf[0];

	memcpy((void*)&crc2, tmpbuf, 2);

	if( crc1 == crc2 )
		return true;
	else
		return false;
}

// 整型数转成字符串，例如 v_iValue = 202, 则 v_szBuf 为 0x32 0x30 0x32 
int G_iToStr(int v_iValue, char *v_szBuf, int v_iBufLen)
{
	if( v_szBuf == NULL )
		return ERR_PAR;

	int  i=0, j=0, iTmp = 0;
	char buf[20] = {0};

	iTmp = v_iValue;

	for(i=0;;)
	{
		buf[i] = iTmp % 10;
		iTmp = (iTmp-buf[i])/10;

		buf[i] += 0x30;
		i++;

		if(iTmp == 0)
			break;
	}

	if( i > v_iBufLen )
		return ERR_BUFLACK;

	for(j=i-1; j>=0; j--)
	{
		v_szBuf[j] = buf[i-j-1];
	}

	return 0;
}

//分析AKV数据
int AKV_Jugre(char *v_szkey, AKV_t *akv, DWORD v_akvlen)
{
	unsigned long i = 0;
	akv->Attr = v_szkey[i++];
	if( (akv->Attr & 0xf0) > 0x10 ||  (akv->Attr & 0xf) > 0x2 || v_akvlen < 4)
	{
		return 0;
	}
	akv->KeyLen = v_szkey[i++];
	if( akv->KeyLen >= 20 )
	{
		return 0;
	}
	memcpy(akv->Key, v_szkey + i, akv->KeyLen);
	akv->Key[akv->KeyLen] = '\0';
	i += 0 == akv->KeyLen? 1 : akv->KeyLen;	//key长度可能为0
	akv->Vlen = (v_szkey[i] << 8) + (byte)v_szkey[i + 1];
	i += 2;
	akv->Value = v_szkey + i;
	akv->AKVLen = akv->KeyLen + akv->Vlen + 4;
	if(akv->AKVLen > v_akvlen)
	{
		return 0;
	}
	return 1;
}

//Key类型判断
int Key_Jugres(char *v_szkey)
{
	if( 0 == strcmp(v_szkey, "ix") )
	{
		return 1; //索引号
	} else if( 0 == strcmp(v_szkey, "iO") )
	{
		return 2; //区域状态 01h--进区域 02h--出区域 03h--区域内 04h--区域外
	} else if( 0 == strcmp(v_szkey, "rT") )
	{
		return 3; //区域类型01h--圆形区域 02h--矩形区域 03h--多边形区域
	} else if( 0 == strcmp(v_szkey, "rS") )
	{
		return 4; //区域
	} else if( 0 == strcmp(v_szkey, "T") )
	{
		return 5; //时间段集合
	} else if( 0 == strcmp(v_szkey, "t") )
	{
		return 6; //定位类型
	} else if( 0 == strcmp(v_szkey, "lat") )
	{
		return 7; //纬度 单位毫秒
	} else if( 0 == strcmp(v_szkey, "lon") )
	{
		return 8; //经度
	} else if( 0 == strcmp(v_szkey, "alt") )
	{
		return 9; //海拔 单位米
	} else if( 0 == strcmp(v_szkey, "1") )
	{
		return 10;
	} else if( 0 == strcmp(v_szkey, "2") )
	{
		return 11;
	} else if( 0 == strcmp(v_szkey, "3") )
	{
		return 12;
	} else if( 0 == strcmp(v_szkey, "4") )
	{
		return 13;
	} else if( 0 == strcmp(v_szkey, "5") )
	{
		return 14;
	} else if( 0 == strcmp(v_szkey, "6") )
	{
		return 15;
	} else if( 0 == strcmp(v_szkey, "b") )
	{
		return 16;
	} else if( 0 == strcmp(v_szkey, "e") )
	{
		return 17;
	} else if( 0 == strcmp(v_szkey, "iF"))
	{
		return 18;
	} else if( 0 == strcmp(v_szkey, "fT"))
	{
		return 19;
	} else if( 0 == strcmp(v_szkey, "ver") )
	{
		return 20;
	} else if( 0 == strcmp(v_szkey, "md5"))
	{
		return 21;
	} else if( 0 == strcmp(v_szkey, "tT"))
	{
		return 22;
	} else if( 0 == strcmp(v_szkey, "pL"))
	{
		return 23;
	} else if( 0 == strcmp(v_szkey, "s"))
	{
		return 24;
	} else if( 0 == strcmp(v_szkey, "l"))
	{
		return 25;
	} else if( 0 == strcmp(v_szkey, "pTO"))
	{
		return 26;
	} else if( 0 == strcmp(v_szkey, "yD"))
	{
		return 27;
	} else if( 0 == strcmp(v_szkey, "rPL"))
	{
		return 28;
	}else if( 0 == strcmp(v_szkey, "te"))
	{
		return 29;
	}else if( 0 == strcmp(v_szkey, "rt"))
	{
		return 30;
	}else if( 0 == strcmp(v_szkey, "ch"))
	{
		return 31;
	}else if( 0 == strcmp(v_szkey, "qa"))
	{
		return 32;
	}else if( 0 == strcmp(v_szkey, "cm"))
	{
		return 33;
	}else if( 0 == strcmp(v_szkey, "ibe"))
	{
		return 34;
	}else if( 0 == strcmp(v_szkey, "bo"))
	{
		return 35;
	}else if( 0 == strcmp(v_szkey, "i"))
	{
		return 36;
	}else if( 0 == strcmp(v_szkey, "m"))
	{
		return 37;
	}else if( 0 == strcmp(v_szkey, "of"))
	{
		return 38;
	}else if( 0 == strcmp(v_szkey, "max"))
	{
		return 39;
	}else if( 0 == strcmp(v_szkey, "qT"))
	{
		return 40;
	}else if( 0 == strcmp(v_szkey, "f"))
	{
		return 41;
	}else if( 0 == strcmp(v_szkey, "bN"))
	{
		return 42;
	}

	return -1;
}
int AKV_T(AKV_t *v_akv, AKV_T_t *akv_T, char *DebugMsg)
{
	char *akvp;
	AKV_t akvtemp;
	AKV_t *akv = &akvtemp;
	DWORD scope = 0, timelen, numtime, result = 0;
	DWORD limit1, limit2;
	if( 0 == v_akv->Vlen )
	{
		PRTMSG(MSG_DBG, "%s AKV T 1 err!!!!!\n", DebugMsg);
		result = 0;
		goto _AKVT_END;
	}
	timelen = v_akv->Vlen;
	akvp = v_akv->Value;
	for(limit1 = 0; limit1 < MAX_TIME_COUNT; limit1++)
	{
		if( scope >= timelen )
		{
			break;
		}
		if(!AKV_Jugre(akvp, akv, timelen))
		{
			PRTMSG(MSG_DBG, "%s AKV_T 2 err!!!!!\n", DebugMsg);
			result = 0;
			goto _AKVT_END;
		}
		scope += akv->AKVLen;
		switch( Key_Jugres(akv->Key) )
		{
		case 10: //第一个时间段
			numtime = 0;
			goto FILLTIME;
		case 11:  //第二个时间段
			numtime = 1;
			goto FILLTIME;
		case 12:  //第三个时间段
			numtime = 2;
			goto FILLTIME;
		case 13:  //第四个时间段
			numtime = 3;
			goto FILLTIME;
		case 14:  //第五个时间段
			numtime = 4;
			goto FILLTIME;
		case 15:  //第六个时间段
			numtime = 5;
			FILLTIME:
			{
				DWORD point = 0;
				DWORD akvlen = akv->Vlen;
				akvp = akv->Value;
				result++;
				for(limit2 = 0; limit2 < 2; limit2++)
				{
					if( point >= akvlen )
					{
						break;
					}
					if(!AKV_Jugre(akvp, akv, akvlen))
					{
						PRTMSG(MSG_DBG, "%s AKV_T 3 err!!!!!\n", DebugMsg);
						result = 0;
						goto _AKVT_END;
					}
					point += akv->AKVLen;
					akvp += akv->AKVLen;
					switch( Key_Jugres(akv->Key) )
					{
					case 16: //开始
						{
							int a = 0;                                        
							if( akv->Vlen != 3 )
							{
								PRTMSG(MSG_DBG, "%s Btime%d b err!!!!!\n", DebugMsg, numtime + 1);
								result = 0;
								goto _AKVT_END;
							}
							while( akv->Vlen-- )
							{
								akv_T->m_szBeginTime[numtime][a] = akv->Value[a];
								a++;
							}
							PRTMSG(MSG_DBG, "%s Btime%d b:%d:%d:%d\n", DebugMsg, numtime + 1, akv_T->m_szBeginTime[numtime][0], akv_T->m_szBeginTime[numtime][1], akv_T->m_szBeginTime[numtime][2]);
						}
						break;
					case 17: //结束
						{
							int a = 0;
							if( akv->Vlen != 3 )
							{
								PRTMSG(MSG_DBG, "%s Btime%d e err!!!!!\n", DebugMsg, numtime + 1);
								result = 0;
								goto _AKVT_END;
							}
							while( akv->Vlen-- )
							{
								akv_T->m_szEndTime[numtime][a] = akv->Value[a];
								a++;
							}
							akv_T->m_bTimeEnable[numtime] = true;
							PRTMSG(MSG_DBG, "%s Btime%d e:%d:%d:%d\n", DebugMsg, numtime + 1, akv_T->m_szEndTime[numtime][0], akv_T->m_szEndTime[numtime][1], akv_T->m_szEndTime[numtime][2]);
						}
						break;
					default:
						{
							PRTMSG(MSG_DBG, "%s %d Btime err!!!!\n", DebugMsg, numtime);
							result = 0;
							goto _AKVT_END;
						}
						break;
					}
				}
			}
			break;

		default:
			{
				PRTMSG(MSG_DBG, "%s T msg err!!!!\n", DebugMsg);
				result = 0;
				goto _AKVT_END;
			}
			break;
		}
	}
	_AKVT_END:
	return result;
}

int AKV_rS(AKV_t *v_akv, AKV_rS_t *akv_rS, byte regiontype, char *DebugMsg)
{
	char *akvp;
	AKV_t akvtemp;
	AKV_t *akv = &akvtemp;
	DWORD limit1, limit2;
	DWORD scope = 0, regionLen, result = 1;
	if( 0 == v_akv->Vlen || 0x02 != regiontype)
	{
		PRTMSG(MSG_DBG, "%s AKV_rS 1 err!!!!!\n", DebugMsg);
		result = 0;
		goto _AKVrS_END;
	}
	regionLen = v_akv->Vlen;
	akvp = v_akv->Value;
	for(limit1 = 0; limit1 < 2; limit1++)
	{
		if( scope >= regionLen )
		{
			break;
		}
		if(!AKV_Jugre(akvp, akv, regionLen))
		{
			PRTMSG(MSG_DBG, "%s AKV_rS 2 err!!!!!\n", DebugMsg);
			result = 0;
			goto _AKVrS_END;
		}
		scope += akv->AKVLen;
		switch( Key_Jugres(akv->Key) )
		{
		case 10: //左上角
			{
				DWORD Leftlen = 0;
				DWORD Leftakvlen = akv->Vlen;
				akvp = akv->Value;
				for(limit2 = 0; limit2 < 4; limit2++)
				{
					if( Leftlen >= Leftakvlen )
					{
						break;
					}
					if(!AKV_Jugre(akvp, akv, Leftakvlen))
					{
						PRTMSG(MSG_DBG, "%s AKV_rS 3 err!!!!!\n", DebugMsg);
						result = 0;
						goto _AKVrS_END;
					}
					Leftlen += akv->AKVLen;
					akvp += akv->AKVLen;
					switch( Key_Jugres(akv->Key) )
					{
					case 6:	//定位类型
						{

						}
						break;
					case 7:	//纬度
						{
							int a = 0;
							if( akv->Vlen != 4 )
							{
								PRTMSG(MSG_DBG, "%s Left lat err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVrS_END;
							}
							akv_rS->m_lMaxLat = akv->Value[a];
							while( --akv->Vlen )
							{
								akv_rS->m_lMaxLat = (akv_rS->m_lMaxLat << 8) + (byte)akv->Value[++a];
							}
//							PRTMSG(MSG_DBG, "%s Left lat :%u\n", DebugMsg, akv_rS->m_lMaxLat);
						}
						break;
					case 8:	//经度
						{
							int a = 0;
							if( akv->Vlen != 4 )
							{
								PRTMSG(MSG_DBG, "%s Left lon err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVrS_END;
							}
							akv_rS->m_lMinLon = akv->Value[a];
							while( --akv->Vlen )
							{
								akv_rS->m_lMinLon = (akv_rS->m_lMinLon << 8) + (byte)akv->Value[++a];
							}
//							PRTMSG(MSG_DBG, "%s Left lon :%u\n",DebugMsg, akv_rS->m_lMinLon);
						}
						break;
					case 9:	//高度
						{

						}
						break;
					default:
						{
							PRTMSG(MSG_DBG, "%s Left point type err!!!!\n", DebugMsg);
							result = 0;
							goto _AKVrS_END;
						}
						break;
					}
				}
			}
			break;
		case 11:  //右下角
			{
				DWORD rightlen = 0;
				DWORD rightakvlen = akv->Vlen;
				akvp = akv->Value;
				for(limit2 = 0; limit2 < 4; limit2++ )
				{
					if( rightlen >= rightakvlen )
					{
						break;
					}
					if(!AKV_Jugre(akvp, akv, rightakvlen))
					{
						PRTMSG(MSG_DBG, "%s AKV_rS 4 err!!!!!\n", DebugMsg);
						result = 0;
						goto _AKVrS_END;
					}
					rightlen += akv->AKVLen;
					akvp += akv->AKVLen;
					switch( Key_Jugres(akv->Key) )
					{
					case 6:	//定位类型
						{

						}
						break;
					case 7:	//纬度
						{
							int a = 0;
							if( akv->Vlen != 4 )
							{
								PRTMSG(MSG_DBG, "%s Right lat err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVrS_END;
							}
							akv_rS->m_lMinLat = akv->Value[a];
							while( --akv->Vlen )
							{
								akv_rS->m_lMinLat = (akv_rS->m_lMinLat << 8) + (byte)akv->Value[++a];
							}
//							PRTMSG(MSG_DBG, "%s Right lat :%u\n", DebugMsg, akv_rS->m_lMinLat);
						}
						break;
					case 8:	//经度
						{
							int a = 0;
							if( akv->Vlen != 4 )
							{
								PRTMSG(MSG_DBG, "%s Right lon err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVrS_END;
							}
							akv_rS->m_lMaxLon = akv->Value[a];
							while( --akv->Vlen )
							{
								akv_rS->m_lMaxLon = (akv_rS->m_lMaxLon << 8) + (byte)akv->Value[++a];
							}
//							PRTMSG(MSG_DBG, "%s Right lon :%u\n", DebugMsg, akv_rS->m_lMaxLon);
						}
						break;

					case 9:	//高度
						{

						}
						break;
					default:
						{
							PRTMSG(MSG_DBG, "%s Right point type err!!!!\n", DebugMsg);
							result = 0;
							goto _AKVrS_END;
						}
						break;
					}
				}

			}
			break;

		default:
			{
				PRTMSG(MSG_DBG, "%s rS msg err!!!!\n", DebugMsg);
				result = 0;
				goto _AKVrS_END;
			}
			break;
		}
	}

_AKVrS_END:
	return result;
}

int AKV_rPT(AKV_t *v_akv, AKV_rPT_t *akv_rPT, char *DebugMsg)
{
	char *akvp;
	AKV_t akvtemp;
	AKV_t *akv = &akvtemp;
	DWORD limit1, limit2, point;
	DWORD scope = 0, regionLen, result = 0;
	if( 0 == v_akv->Vlen)
	{
		PRTMSG(MSG_DBG, "%s AKV rPL 1 err!!!!!\n", DebugMsg);
		result = 0;
		goto _AKVRPT_END;
	}
	regionLen = v_akv->Vlen;
	akvp = v_akv->Value;
	for(limit1 = 0; limit1 < MAX_POINT_COUNT; limit1++)
	{
		if( scope >= regionLen )
		{
			break;
		}
		if(!AKV_Jugre(akvp, akv, regionLen))
		{
			PRTMSG(MSG_DBG, "%s AKV rPL 2 err!!!!!\n", DebugMsg);
			result = 0;
			goto _AKVRPT_END;
		}
		scope += akv->AKVLen;
		point = akv->Key[0] - 0x31;
		if( 2 == akv->KeyLen )
		{
			point = (akv->Key[0] - 0x30) * 10 + akv->Key[1] - 0x31;
		}
		result++;
		{
			DWORD Leftlen = 0;
			DWORD Leftakvlen = akv->Vlen;
			akvp = akv->Value;
			for(limit2 = 0; limit2 < 4; limit2++)
			{
				if( Leftlen >= Leftakvlen )
				{
					break;
				}
				if(!AKV_Jugre(akvp, akv, Leftakvlen))
				{
					PRTMSG(MSG_DBG, "%s AKV rPL 3 err!!!!!\n", DebugMsg);
					result = 0;
					goto _AKVRPT_END;
				}
				Leftlen += akv->AKVLen;
				akvp += akv->AKVLen;
				switch( Key_Jugres(akv->Key) )
				{
				case 6:	//定位类型
					{


					}
					break;
				case 7:	//纬度
					{
						int a = 0;
						if( akv->Vlen != 4 )
						{
							PRTMSG(MSG_DBG, "%s lat err!!!!!\n", DebugMsg);
							result = 0;
							goto _AKVRPT_END;
						}
						akv_rPT->m_lLat[point] = akv->Value[a];
						while( --akv->Vlen )
						{
							akv_rPT->m_lLat[point] = (akv_rPT->m_lLat[point] << 8) + (byte)akv->Value[++a];
						}
//							 PRTMSG(MSG_DBG, "DealLineRpt lat :%u\n", objLineRptPar.m_lLat[point]);
					}
					break;
				case 8:	//经度
					{
						int a = 0;
						if( akv->Vlen != 4 )
						{
							PRTMSG(MSG_DBG, "%s lon err!!!!!\n", DebugMsg);
							result = 0;
							goto _AKVRPT_END;
						}
						akv_rPT->m_lLon[point] = akv->Value[a];
						while( --akv->Vlen )
						{
							akv_rPT->m_lLon[point] = (akv_rPT->m_lLon[point] << 8) + (byte)akv->Value[++a];
						}
						akv_rPT->m_bPointEnable[point] = true;
//							PRTMSG(MSG_DBG, "DealLineRpt lon :%u\n", objLineRptPar.m_lLon[point]);
					}
					break;
				case 9:	//高度
					{

					}
					break;  
				default:
					{
						PRTMSG(MSG_DBG, "%s point type err!!!!\n", DebugMsg);
						result = 0;
						goto _AKVRPT_END;
					}
					break;
				}
			}
		}
	}
_AKVRPT_END:
	return result;

}

int AKV_bo(AKV_t *v_akv, AKV_bo_t *akv_bo, char *DebugMsg)
{
	char *akvp;
	AKV_t akvtemp;
	AKV_t *akv = &akvtemp;
	DWORD result = 1;
	DWORD limit;
	DWORD point = 0;
	DWORD akvlen = v_akv->Vlen;
	akvp = v_akv->Value;
	for(limit = 0; limit < 4; limit++)
		{
					if( point >= akvlen )
					{
						break;
					}
					if(!AKV_Jugre(akvp, akv, akvlen))
					{
						PRTMSG(MSG_DBG, "%s AKV_bo 1 err!!!!!\n", DebugMsg);
						result = 0;
						goto _AKVbo_END;
					}
					point += akv->AKVLen;
					akvp += akv->AKVLen;
					switch( Key_Jugres(akv->Key) )
					{
						case 37: //m
						{                                    
							if( !Get_char(akv, &akv_bo->m_bytTTakeBaseTime, DebugMsg) )
							{
								PRTMSG(MSG_DBG, "%s m err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVbo_END;
							}
						}
						break;
						case 38: //of
						{                                    
							if( !Get_char(akv, &akv_bo->m_bytTTakeOfTime, DebugMsg) )
							{
								PRTMSG(MSG_DBG, "%s of err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVbo_END;
							}
						}
						break;
						case 16: //开始
						{
							int a = 0;                                        
							if( akv->Vlen != 6 )
							{
								PRTMSG(MSG_DBG, "%s Btime b err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVbo_END;
							}
							while( akv->Vlen-- )
							{
								akv_bo->m_bytTTakeBeginTime[a] = akv->Value[a];
								a++;
							}
						}
						break;
					case 17: //结束
						{
							int a = 0;
							if( akv->Vlen != 6 )
							{
								PRTMSG(MSG_DBG, "%s Btime e err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVbo_END;
							}
							while( akv->Vlen-- )
							{
								akv_bo->m_bytTTakeEndTime[a] = akv->Value[a];
								a++;
							}
						}
						break;
					default:
						{
							PRTMSG(MSG_DBG, "%s bo err!!!!\n", DebugMsg);
							result = 0;
							goto _AKVbo_END;
						}
						break;
					}

		}
_AKVbo_END:
	return result;
}

int AKV_ibe(AKV_t *v_akv, AKV_ibe_t *akv_ibe, char *DebugMsg)
{
	char *akvp;
	AKV_t akvtemp;
	AKV_t *akv = &akvtemp;
	DWORD result = 1;
	DWORD limit;
	DWORD point = 0;
	DWORD akvlen = v_akv->Vlen;
	akvp = v_akv->Value;
	for(limit = 0; limit < 3; limit++)
		{
					if( point >= akvlen )
					{
						break;
					}
					if(!AKV_Jugre(akvp, akv, akvlen))
					{
						PRTMSG(MSG_DBG, "%s AKV_ibe 1 err!!!!!\n", DebugMsg);
						result = 0;
						goto _AKVibe_END;
					}
					point += akv->AKVLen;
					akvp += akv->AKVLen;
					switch( Key_Jugres(akv->Key) )
					{
						case 36: //i
						{                                    
							if( !Get_ushort(akv, &akv_ibe->m_usInterval, DebugMsg) )
							{
								PRTMSG(MSG_DBG, "%s i err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVibe_END;
							}
						}
						break;

					case 16: //开始
						{
							int a = 0;                                        
							if( akv->Vlen != 6 )
							{
								PRTMSG(MSG_DBG, "%s Btime b err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVibe_END;
							}
							while( akv->Vlen-- )
							{
								akv_ibe->m_bytCTakeBeginTime[a] = akv->Value[a];
								a++;
							}
						}
						break;
					case 17: //结束
						{
							int a = 0;
							if( akv->Vlen != 6 )
							{
								PRTMSG(MSG_DBG, "%s Btime e err!!!!!\n", DebugMsg);
								result = 0;
								goto _AKVibe_END;
							}
							while( akv->Vlen-- )
							{
								akv_ibe->m_bytCTakeEndTime[a] = akv->Value[a];
								a++;
							}
						}
						break;
					default:
						{
							PRTMSG(MSG_DBG, "ibe err!!!!\n", DebugMsg);
							result = 0;
							goto _AKVibe_END;
						}
						break;
					}
		}
_AKVibe_END:
	return result;
}


int Get_ulong(AKV_t *v_akv, ulong *Value, char *DebugMsg)
{
	ulong ulv;
	int result = 0;
	int a = 0;
	if(v_akv->Vlen != 4)
	{
		PRTMSG(MSG_DBG, "%s get ulong value err! Vlen=%d\n", DebugMsg, v_akv->Vlen);
		goto _GETULONG_END;
	}
	ulv = v_akv->Value[a];
	while( --v_akv->Vlen )
	{
		ulv = (ulv << 8) + (byte)v_akv->Value[++a];
	}             
	*Value = ulv;
	result = 1;
//	PRTMSG(MSG_DBG, "%s Value: %d\n", DebugMsg, ulv);
	_GETULONG_END:
	return result;
}

int Get_ushort(AKV_t *v_akv, ushort *Value, char *DebugMsg)
{
	int result = 0;
	int a = 0;
	if(v_akv->Vlen != 2)
	{
		PRTMSG(MSG_DBG, "%s get ushort value err! Vlen=%d\n", DebugMsg, v_akv->Vlen);
		goto _GETUSHORT_END;
	}
	*Value = v_akv->Value[0] * 256 + v_akv->Value[1];     
	result = 1;
//	PRTMSG(MSG_DBG, "%s Value: %d\n", DebugMsg, *Value);
_GETUSHORT_END:
	return result;
}

int Get_char(AKV_t *v_akv, byte *Value, char *DebugMsg)
{
	int result = 0;
	if(v_akv->Vlen != 1)
	{
		PRTMSG(MSG_DBG, "%s get char value err! Vlen=%d\n", DebugMsg, v_akv->Vlen);
		goto _GETCHAR_END;
	}
	*Value = v_akv->Value[0];
	result = 1;
//	PRTMSG(MSG_DBG, "%s Value: %d\n", DebugMsg, *Value);
_GETCHAR_END:
	return result;
}

ushort PutAKV_ulong(char *v_DesBuf, char akvattr, char *Key, ulong v_ulValue, ushort *dwTotalLen)
{
	ushort i = 0;
	v_DesBuf[i++] = akvattr;
	v_DesBuf[i++] = strlen(Key);
	memcpy(v_DesBuf + i, Key, v_DesBuf[1]);
	i += v_DesBuf[1];
	v_DesBuf[i++] = 0x00;
	v_DesBuf[i++] = 0x04;
	v_DesBuf[i++] = (byte)(htonl(v_ulValue) & 0xff);
	v_DesBuf[i++] = (byte)((htonl(v_ulValue) >> 8) & 0xff);
	v_DesBuf[i++] = (byte)((htonl(v_ulValue) >> 16) & 0xff);
	v_DesBuf[i++] = (byte)((htonl(v_ulValue) >> 24) & 0xff);
	*dwTotalLen += i;
	return i;
}

ushort PutAKV_ushort(char *v_DesBuf, char akvattr, char *Key, ushort v_usValue, ushort *dwTotalLen)
{
	ushort i = 0;
	v_DesBuf[i++] = akvattr;
	v_DesBuf[i++] = strlen(Key);
	memcpy(v_DesBuf + i, Key, v_DesBuf[1]);
	i += v_DesBuf[1];
	v_DesBuf[i++] = 0x00;
	v_DesBuf[i++] = 0x02;
	v_DesBuf[i++] = (byte)(htons(v_usValue) & 0xff);
	v_DesBuf[i++] = (byte)((htons(v_usValue) >> 8) & 0xff);
	*dwTotalLen += i;
	return i;
}

ushort PutAKV_char(char *v_DesBuf, char akvattr, char *Key, byte *v_bValue, ushort v_usbLen, ushort *dwTotalLen)
{
	ushort i = 0;
	ushort cnt = 0;
	v_DesBuf[i++] = akvattr;
	v_DesBuf[i++] = strlen(Key);
	memcpy(v_DesBuf + i, Key, v_DesBuf[1]);
	i += v_DesBuf[1];
	v_DesBuf[i++] = (byte)(htons(v_usbLen) & 0xff);
	v_DesBuf[i++] = (byte)((htons(v_usbLen) >> 8) & 0xff);
	for(cnt = 0; cnt < v_usbLen; cnt++)
	{
		v_DesBuf[i++] = v_bValue[cnt];
	}
	*dwTotalLen += i;
	return i;
}

bool JugPtInShape(long longitude, long latitude, long MinLon, long MinLat, long MaxLon, long MaxLat, byte AreaAttr, byte *AttrRes)
{
	bool bret;
	switch(AreaAttr)
	{
		case 0x01:
			{
				bret = false;
				*AttrRes = 0x01;
			}
			break;
		case 0x02:
			{
				bret = JugPtInRect(longitude, latitude, MinLon, MinLat, MaxLon, MaxLat); 
				*AttrRes = 0x02;
			}
			break;
		case 0x03:
			{
				bret = false;
				*AttrRes = 0x03;
			}
			break;
		default:
			{
				bret = false;
				*AttrRes = 0xff;
			}
			break;	
	}
	return bret;
}
//偏离航线距离  
bool OutLineLen(double lon1, double lat1, double lon2, double lat2, double curlon, double curlat, double *len)
{
	double a, b, c, p;
	bool bret = true;
	if(lon1 < lon2)
	{
		if(lon1 > curlon || curlon > lon2)
		{
			bret = false;
			return bret;
		}
	}
	else if(lon1 > lon2)
	{
		if(lon1 < curlon || curlon < lon2)
		{
			bret = false;
			return bret;
		}
	}
	else
	{//lon1 == lon2
		if(lat1 == lat2)
		{
			bret = false;
			return bret;
		}
		else if(lat1 < lat2)
		{
			if(curlat < lat1 || curlat > lat2)
			{
				bret = false;
				return bret;
			}
		}
		else
		{
			if(curlat > lat1 || curlat < lat2)
			{
				bret = false;
				return bret;
			}
		}
	}
	a = G_CalGeoDis2(lon1, lat1, lon2, lat2);
	b = G_CalGeoDis2(curlon, curlat, lon2, lat2);
	c = G_CalGeoDis2(lon1, lat1, curlon, curlat);
	if(a <= 0 || b < 0 || c < 0 || c > a)
	{//无效值
		bret = false;
	}
	else
	{	
		if(a == (b + c))
		{
			*len = 0;
		}	
		else
		{
			p = (a + b + c) / 2;
			*len = 2 * sqrt(p*(p - a)*(p - b)*(p - c)) / a;
		}
	}
	return bret;
}

#endif
