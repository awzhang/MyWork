#include "yx_QianStdAfx.h"

#if USE_LIAONING_SANQI == 1

	#undef MSG_HEAD
	#define MSG_HEAD	("QianExe-LN_Config")

CLN_Config::CLN_Config()
{

}

CLN_Config::~CLN_Config()
{

}

bool CLN_Config::RemoveRpt(ushort Action, char *Rid, char *sz_Buf, DWORD *v_dwLen)
{
	bool bret = false;
	DWORD dwBufLen = 1;
	int i;
	sz_Buf[0] = 0;
	tag2QReportCfg objReportCfg;
	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg)); 

	tag2QAreaRptCfg     objAreaRptCfg;  
	GetSecCfg((void*)&objAreaRptCfg, sizeof(objAreaRptCfg), offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));

	tag2QAreaSpdRptCfg  objAreaSpdRptCfg;
	GetSecCfg((void*)&objAreaSpdRptCfg, sizeof(objAreaSpdRptCfg), offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg)); 

	tag2QAreaParkTimeRptCfg objAreaParkTimeRptCfg;
	GetSecCfg((void*)&objAreaParkTimeRptCfg, sizeof(objAreaParkTimeRptCfg), offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg));

	tag2QLineRptCfg     objLineRptCfg;
	GetSecCfg((void*)&objLineRptCfg, sizeof(objLineRptCfg), offsetof(tagSecondCfg, m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg)); 
	switch( Action )
	{
	case 0x0001:
		{
			objReportCfg.m_objFootRptPar.m_bEnable = false;
			sz_Buf[0]++;
			sz_Buf[dwBufLen++] = strlen(objReportCfg.m_objFootRptPar.m_szRID);  
			memcpy(sz_Buf + dwBufLen, objReportCfg.m_objFootRptPar.m_szRID, sz_Buf[dwBufLen - 1]);
			dwBufLen += sz_Buf[dwBufLen - 1];
			*v_dwLen = dwBufLen;
			SetSecCfg((void *)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
			bret = true;
		}
		break;
	case 0x0002:
		{
			objReportCfg.m_objMilePrtPar.m_bEnable = false;
			sz_Buf[0]++;
			sz_Buf[dwBufLen++] = strlen(objReportCfg.m_objMilePrtPar.m_szRID);  
			memcpy(sz_Buf + dwBufLen, objReportCfg.m_objMilePrtPar.m_szRID, sz_Buf[dwBufLen - 1]);
			dwBufLen += sz_Buf[dwBufLen - 1];
			*v_dwLen = dwBufLen;
			SetSecCfg((void *)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
			bret = true;
		}
		break;
	case 0x0003:
		{
			objReportCfg.m_objDoorRptPar.m_bEnable = false;
			sz_Buf[0]++;
			sz_Buf[dwBufLen++] = strlen(objReportCfg.m_objDoorRptPar.m_szRID);  
			memcpy(sz_Buf + dwBufLen, objReportCfg.m_objDoorRptPar.m_szRID, sz_Buf[dwBufLen - 1]);
			dwBufLen += sz_Buf[dwBufLen - 1];
			*v_dwLen = dwBufLen;
			SetSecCfg((void *)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
			bret = true;
		}
		break;
	case 0x0005:
		{
			objAreaParkTimeRptCfg.m_objParkTime.m_bEnable = false;
			sz_Buf[0]++;
			sz_Buf[dwBufLen++] = strlen(objAreaParkTimeRptCfg.m_objParkTime.m_szRID);   
			memcpy(sz_Buf + dwBufLen, objAreaParkTimeRptCfg.m_objParkTime.m_szRID, sz_Buf[dwBufLen - 1]);
			dwBufLen += sz_Buf[dwBufLen - 1];
			*v_dwLen = dwBufLen;
			SetSecCfg((void *)&objAreaParkTimeRptCfg, offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg));
			bret = true;
		}
		break;
	case 0x0006:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaRptCfg.m_objArea[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaRptCfg.m_objArea[i].m_bEnable = false;
					objAreaRptCfg.m_objArea[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaRptCfg.m_objArea[i].m_szRID);    
					memcpy(sz_Buf + dwBufLen, objAreaRptCfg.m_objArea[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaRptCfg, offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0007:    
		{
			objReportCfg.m_objBelowSpdRptPar.m_bEnable = false;
			objReportCfg.m_objOverSpdRptPar.m_bEnable = false;
			objReportCfg.m_objCrossSpdRptPar.m_bEnable = false;
			sz_Buf[0]++;
			sz_Buf[dwBufLen++] = strlen(objReportCfg.m_objBelowSpdRptPar.m_szRID);  
			memcpy(sz_Buf + dwBufLen, objReportCfg.m_objBelowSpdRptPar.m_szRID, sz_Buf[dwBufLen - 1]);
			dwBufLen += sz_Buf[dwBufLen - 1];
			*v_dwLen = dwBufLen;
			SetSecCfg((void *)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
			bret = true;
		}
		break;
	case 0x0008:
		{
			for( i = 0; i < MAX_TIMESPD_COUNT; i++ )
			{
				if(0 == strncmp(objAreaSpdRptCfg.m_TimeSpd[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaSpdRptCfg.m_TimeSpd[i].m_bEnable = false;
					objAreaSpdRptCfg.m_TimeSpd[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaSpdRptCfg.m_TimeSpd[i].m_szRID); 
					memcpy(sz_Buf + dwBufLen, objAreaSpdRptCfg.m_TimeSpd[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaSpdRptCfg, offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg));
					bret = true;
				}
			} 
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_TIMESPD_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0009:
		{
			for( i = 0; i < MAX_LINE_COUNT; i++ )
			{
				if(0 == strncmp(objLineRptCfg.m_objLine[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objLineRptCfg.m_objLine[i].m_bEnable = false;
					objLineRptCfg.m_objLine[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objLineRptCfg.m_objLine[i].m_szRID);    
					memcpy(sz_Buf + dwBufLen, objLineRptCfg.m_objLine[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objLineRptCfg, offsetof(tagSecondCfg, m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg));
					bret = true;
				}
			} 
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_LINE_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}

		}
		break;
	case 0x000e:
		{
			objReportCfg.m_objPowOffRptPar.m_bEnable = false;
			sz_Buf[0]++;
			sz_Buf[dwBufLen++] = strlen(objReportCfg.m_objPowOffRptPar.m_szRID);    
			memcpy(sz_Buf + dwBufLen, objReportCfg.m_objPowOffRptPar.m_szRID, sz_Buf[dwBufLen - 1]);
			dwBufLen += sz_Buf[dwBufLen - 1];
			*v_dwLen = dwBufLen;
			SetSecCfg((void *)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
			bret = true;
		}
		break;
	case 0x000f:
		{
			objReportCfg.m_objStartEngPrtPar.m_bEnable = false;
			sz_Buf[0]++;
			sz_Buf[dwBufLen++] = strlen(objReportCfg.m_objStartEngPrtPar.m_szRID);  
			memcpy(sz_Buf + dwBufLen, objReportCfg.m_objStartEngPrtPar.m_szRID, sz_Buf[dwBufLen - 1]);
			dwBufLen += sz_Buf[dwBufLen - 1];
			*v_dwLen = dwBufLen;
			SetSecCfg((void *)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
			bret = true;
		}
		break;
	case 0x0014:
		{
			objReportCfg.m_objTirePrtPar.m_bEnable = false;
			sz_Buf[0]++;
			sz_Buf[dwBufLen++] = strlen(objReportCfg.m_objTirePrtPar.m_szRID);  
			memcpy(sz_Buf + dwBufLen, objReportCfg.m_objTirePrtPar.m_szRID, sz_Buf[dwBufLen - 1]);
			dwBufLen += sz_Buf[dwBufLen - 1];
			*v_dwLen = dwBufLen;
			SetSecCfg((void *)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
			bret = true;
		}
		break;
	case 0x0016:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaSpdRptCfg.m_objAreaSpd[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaSpdRptCfg.m_objAreaSpd[i].m_bEnable = false;
					objAreaSpdRptCfg.m_objAreaSpd[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaSpdRptCfg.m_objAreaSpd[i].m_szRID);  
					memcpy(sz_Buf + dwBufLen, objAreaSpdRptCfg.m_objAreaSpd[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaSpdRptCfg, offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0018:
		{
			for( i = 0; i < MAX_LINE_COUNT; i++ )
			{
				if(0 == strncmp(objLineRptCfg.m_objTimeLine[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objLineRptCfg.m_objTimeLine[i].m_bEnable = false;
					objLineRptCfg.m_objTimeLine[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objLineRptCfg.m_objTimeLine[i].m_szRID);    
					memcpy(sz_Buf + dwBufLen, objLineRptCfg.m_objTimeLine[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objLineRptCfg, offsetof(tagSecondCfg, m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_LINE_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0019:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_bEnable = false;
					objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_szRID);    
					memcpy(sz_Buf + dwBufLen, objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaParkTimeRptCfg, offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0022:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaRptCfg.m_objInArea[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaRptCfg.m_objInArea[i].m_bEnable = false;
					objAreaRptCfg.m_objInArea[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaRptCfg.m_objInArea[i].m_szRID);  
					memcpy(sz_Buf + dwBufLen, objAreaRptCfg.m_objInArea[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaRptCfg, offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0023:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaRptCfg.m_objOutArea[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaRptCfg.m_objOutArea[i].m_bEnable = false;
					objAreaRptCfg.m_objOutArea[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaRptCfg.m_objOutArea[i].m_szRID); 
					memcpy(sz_Buf + dwBufLen, objAreaRptCfg.m_objOutArea[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaRptCfg, offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0024:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaRptCfg.m_objInRegion[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaRptCfg.m_objInRegion[i].m_bEnable = false;
					objAreaRptCfg.m_objInRegion[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaRptCfg.m_objInRegion[i].m_szRID);    
					memcpy(sz_Buf + dwBufLen, objAreaRptCfg.m_objInRegion[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaRptCfg, offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0025:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaRptCfg.m_objOutRegion[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaRptCfg.m_objOutRegion[i].m_bEnable = false;
					objAreaRptCfg.m_objOutRegion[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaRptCfg.m_objOutRegion[i].m_szRID);   
					memcpy(sz_Buf + dwBufLen, objAreaRptCfg.m_objOutRegion[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaRptCfg, offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0026:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaSpdRptCfg.m_objInRegionSpd[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaSpdRptCfg.m_objInRegionSpd[i].m_bEnable = false;
					objAreaSpdRptCfg.m_objInRegionSpd[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaSpdRptCfg.m_objInRegionSpd[i].m_szRID);  
					memcpy(sz_Buf + dwBufLen, objAreaSpdRptCfg.m_objInRegionSpd[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaSpdRptCfg, offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0027:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaSpdRptCfg.m_objOutRegionSpd[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaSpdRptCfg.m_objOutRegionSpd[i].m_bEnable = false;
					objAreaSpdRptCfg.m_objOutRegionSpd[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaSpdRptCfg.m_objOutRegionSpd[i].m_szRID); 
					memcpy(sz_Buf + dwBufLen, objAreaSpdRptCfg.m_objOutRegionSpd[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaSpdRptCfg, offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg));
					bret = true;
				}
			} 
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0028:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_bEnable = false;
					objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_szRID);    
					memcpy(sz_Buf + dwBufLen, objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaParkTimeRptCfg, offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0x0029:
		{
			for( i = 0; i < MAX_AREA_COUNT; i++ )
			{
				if(0 == strncmp(objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_szRID, Rid, strlen(Rid)) || 0 == strncmp(Rid, "*", strlen(Rid)))
				{
					objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_bEnable = false;
					objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_ulIndex = 0;
					sz_Buf[0]++;
					sz_Buf[dwBufLen++] = strlen(objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_szRID);   
					memcpy(sz_Buf + dwBufLen, objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_szRID, sz_Buf[dwBufLen - 1]);
					dwBufLen += sz_Buf[dwBufLen - 1];
					*v_dwLen = dwBufLen;
					SetSecCfg((void *)&objAreaParkTimeRptCfg, offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg));
					bret = true;
				}
			}
			if(0 == strncmp(Rid, "*", strlen(Rid)))
			{
				sz_Buf[0] = MAX_AREA_COUNT;
				sz_Buf[1] = 0x01;
				sz_Buf[2] = '*';
				*v_dwLen = 3;   
			}
		}
		break;
	case 0xffff:
		{
			sz_Buf[0] = 23;
			sz_Buf[1] = 0x01;
			sz_Buf[2] = '*';
			*v_dwLen = 3;
			memset((char *)&objReportCfg, 0, sizeof(objReportCfg));
			memset((char *)&objAreaRptCfg, 0, sizeof(objAreaRptCfg));
			memset((char *)&objAreaParkTimeRptCfg, 0, sizeof(objAreaParkTimeRptCfg));
			memset((char *)&objAreaSpdRptCfg, 0, sizeof(objAreaSpdRptCfg));
			memset((char *)&objLineRptCfg, 0, sizeof(objLineRptCfg));
			SetSecCfg((void *)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
			SetSecCfg((void *)&objAreaRptCfg, offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));
			SetSecCfg((void *)&objAreaParkTimeRptCfg, offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg));
			SetSecCfg((void *)&objAreaSpdRptCfg, offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg));
			SetSecCfg((void *)&objLineRptCfg, offsetof(tagSecondCfg, m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg));
			bret = true;
		}
		break;
	default:
		{
			bret = false;
		}
		break;
	}
	return bret;

}

// 终端初始化
int CLN_Config::Deal01_0001(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	// Attr1	K_len1	Key1	V_len1	Value1
	// 00		01		"t"		00 06	09 01 02 08 14 00

	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	int     iRet = 0;
//	tagReply01_0001 objReply;

	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
	char *fail = "fail";
	char *succ = "null";
//	memcpy(objReply.m_objAKVTime.m_szValue, v_szBuf + 5, 6);

	// 参数检查
	if( v_szBuf[3] != 't' || v_dwLen != 11 )
	{
		PRTMSG(MSG_ERR, "Deal01_0001 len err!\n");
		iRet = ERR_PAR;
		goto _DEAL01_0001_END;
	}

	// 根据指令中的时间，设置RTC时间
	{
		struct rtc_time objRtcTime;
		int iDevRtc = open("/dev/misc/rtc", O_NONBLOCK);
		if( iDevRtc == -1 )
		{
			iRet = ERR_OPENFAIL;
			goto _DEAL01_0001_END;
		}

		ioctl(iDevRtc, RTC_RD_TIME, &objRtcTime);
		objRtcTime.tm_year = v_szBuf[5] + 2000;
		objRtcTime.tm_mon  = v_szBuf[6] - 1;
		objRtcTime.tm_mday = v_szBuf[7];
		objRtcTime.tm_hour = v_szBuf[8];
		objRtcTime.tm_min  = v_szBuf[9];
		objRtcTime.tm_sec  = v_szBuf[10];
		ioctl(iDevRtc, RTC_SET_TIME, &objRtcTime);

		close(iDevRtc);
	}

	// 再设置系统时间
	{
		struct tm objSetTime;

		objSetTime.tm_sec  = v_szBuf[10];
		objSetTime.tm_min  = v_szBuf[9];
		objSetTime.tm_hour = v_szBuf[8];
		objSetTime.tm_mday = v_szBuf[7];
		objSetTime.tm_mon  = v_szBuf[6] - 1;
		objSetTime.tm_year = v_szBuf[5] + 100;

		if( 0 != SetCurTime(&objSetTime) )
		{
			iRet = ERR_CFG;
			goto _DEAL01_0001_END;
		}
	}

	// 再恢复出厂配置（保留手机号）
	if( 0 != ResumeCfg(true, false) )
	{
		iRet = ERR_CFG;
		goto _DEAL01_0001_END;
	}

	_DEAL01_0001_END:
//	if ( iRet )		// 失败（若成功则有缺省值）
//	{
//		objReply.m_bytAck = 0;
//		memcpy(objReply.m_objAKVError.m_szValue, "fail", 4);
//	}
	if( iRet )	   // 失败
	{
		szTempBuf[dwTempLen++] = 0x00;
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "t", (byte *)(v_szBuf + 5), 6, &dwTempLen);
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "m", (byte *)fail, 4, &dwTempLen);
//		objReply.m_bytAck = 0;
//		objReply.m_objAKVs.m_szValue = 0;
//		memcpy(objReply.m_objAKVError.m_szValue, "fail", 4);
	} else
	{
		szTempBuf[dwTempLen++] = 0x01;
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "t", (byte *)(v_szBuf + 5), 6, &dwTempLen);
//		PutAKV_char(szTempBuf + dwTempLen, 0x00, "m", (byte *)succ, 4, &dwTempLen);
	}
	// 发送应答
	if( !g_objApplication.MakeAppFrame(0x01, 0x0001, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	if( !iRet )
	{
		// 间隔5秒后，连接新的IP地址和端口
		sleep(5);

		char buf = 0x01;	// QianExe请求SockServExe重新连接套接字
		DataPush((void*)&buf, 1, DEV_QIAN, DEV_SOCK, LV3);
	}

	return iRet;
}

// 点名
int CLN_Config::Deal01_0002(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	// 先应答
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	tagReply01_0002 objReply;

	if( !g_objApplication.MakeAppFrame(0x01, 0x0002, (char*)&objReply, sizeof(objReply), szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	// 再发送一条位置上报信息
	g_objReport.SendOnePositionReport(0x01);

	return 0;
}

// 油路开关
int CLN_Config::Deal01_0004(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	// Attr1	K_len1	Key1	V_len1	Value1
	// 00		01		"s"		00 01	01

	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
//	tagReply01_0004 objReply;
	int     iret = 0;
	char    buf[2] = {0};
	char *fail = "fail";
	char *succ = "null";
	byte oilsta;
	// 参数长度检查
	if( v_szBuf[2] != 's' || v_dwLen != 6 )
	{
		PRTMSG(MSG_ERR, "Deal01_0004 len err!\n");
		iret = ERR_PAR;
		goto _DEAL01_0004_END;
	}

	// 模拟0601帧
	oilsta = v_szBuf[5];
	if( oilsta == 0)	   // 断开油路
		buf[1] = 0x40;
	else						// 闭合油路
		buf[1] = 0x00;

	iret = g_objSpecCtrl.Deal0601(NULL, buf, 2);

	_DEAL01_0004_END:
	if( iret )	   // 失败（若成功则有缺省值）
	{
		szTempBuf[dwTempLen++] = 0x00;
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "s", &oilsta, 1, &dwTempLen);
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "m", (byte *)fail, 4, &dwTempLen);
//		objReply.m_bytAck = 0;
//		objReply.m_objAKVs.m_szValue = 0;
//		memcpy(objReply.m_objAKVError.m_szValue, "fail", 4);
	} else
	{
		szTempBuf[dwTempLen++] = 0x01;
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "s", &oilsta, 1, &dwTempLen);
//		PutAKV_char(szTempBuf + dwTempLen, 0x00, "m", (byte *)succ, 4, &dwTempLen);
	}

	// 发送应答
	if( !g_objApplication.MakeAppFrame(0x01, 0x0004, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	return iret;
}

// 监听
int CLN_Config::Deal01_0006(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	tagReply01_0004 objReply;		// 0004和0006应答结构相同
	int     iret = 0;
	char    szLsnTel[15] = {0};

	// 参数检查（中心实际下发指令格式与协议中描述的不一致，以中心实际下发为准）
	// Attr		K_len	key			V_len
	// 12		03		6e 6f 4c	00 20
	// 00 01 31 00 0b 30 35 39 32 32 39 35 36 37 37 39 —— 第一个电话号码的AKV
	// 00 01 32 00 0b 31 35 39 38 30 38 31 31 32 39 35 —— 第二个电话号码的AKV
	if( v_szBuf[0] != 0x12 && v_szBuf[2] != 0x6e && v_szBuf[3] != 0x6f && v_szBuf[4] != 0x4c )
	{
		PRTMSG(MSG_ERR, "Deal01_0006 para err!\n");
		iret = ERR_PAR;
		goto _DEAL01_0006_END;
	}

	// 简化一下，只提取第一个电话号码，模拟0723协议帧
	szLsnTel[0] = v_szBuf[11];
	memcpy(szLsnTel + 1, v_szBuf + 12, v_szBuf[11]);

	iret = g_objMonAlert.Deal0723(NULL, szLsnTel, v_szBuf[11] + 1);

	_DEAL01_0006_END:
	if( iret )	   // 失败（若成功则有缺省值）
	{
		objReply.m_bytAck = 0;
		objReply.m_objAKVs.m_szValue = 0;
		memcpy(objReply.m_objAKVError.m_szValue, "fail", 4);
	}

	// 发送应答
	if( !g_objApplication.MakeAppFrame(0x01, 0x0006, (char*)&objReply, sizeof(objReply), szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	return iret;
}
//远程开关GPS信号
int CLN_Config::Deal01_0007(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	//00  01 73  00 01      01开启
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	int     iret = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;

	char buf[2] = {0};
	if( 5 > v_dwLen )
	{
		iret = ERR_PAR;
		goto _DEAL01_0007_END;
	}
	if( !AKV_Jugre(v_szBuf, akv, v_dwLen ))
	{
		iret = ERR_PAR;
		goto _DEAL01_0007_END;
	}
	if( 's' != akv->Key[0] || akv->Vlen != 1 || akv->Value[0] > 1)
	{
		iret = ERR_PAR;
		goto _DEAL01_0007_END;
	}

	buf[0] = 0x05; //强制使GPS进入睡眠
	if(0x01 == akv->Value[0])
	{//开启
		buf[1] = 0x01;
	} else
	{//关闭
		buf[1] = 0x00;
	}
	DataPush( buf, 2, DEV_QIAN, DEV_DIAODU, LV1 );
	_DEAL01_0007_END:
	if( iret )
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else
	{
		szTempBuf[dwTempLen++] = 0x01;
	}
	PutAKV_char(szTempBuf + dwTempLen, 0x00, "s", (byte *)akv->Value, 1, &dwTempLen);

	if( !g_objApplication.MakeAppFrame(0x01, 0x0007, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

}

// 里程设置
int CLN_Config::Deal09_0001(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	char *DEBUGMSG = "Deal09_0001";
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	int     iret = 0, i = 0;
	ulong mile = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;
	bool    bNeedConfirm = false;
	if( 11  != v_dwLen )
	{
		iret = ERR_PAR;
		goto _DEAL09_0001_END;
	}
// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;
	if( !AKV_Jugre(v_szBuf+i, akv, v_dwLen ))
	{
		iret = ERR_PAR;
		goto _DEAL09_0001_END;
	}
	if( 0 != strcmp(akv->Key, "mi"))
	{
		iret = ERR_PAR;
		goto _DEAL09_0001_END;
	}
	Get_ulong(akv, &mile, DEBUGMSG);
	PRTMSG(MSG_DBG, "Set metertotal %d km\n", mile);
	g_objDriveRecord.m_v.metertotal = mile * 1000;
	g_objDriveRecord.TotalMeter = (ushort)(g_objDriveRecord.m_v.metertotal/1000);
	g_objDriveRecord.Mile = (ulong)(g_objDriveRecord.m_v.metertotal/1000);
	_DEAL09_0001_END:
	if( iret )
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else
	{
		szTempBuf[dwTempLen++] = 0x01;
	}
	if(bNeedConfirm)
	{
		PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "mi", mile, &dwTempLen);
	}
	if( !g_objApplication.MakeAppFrame(0x09, 0x0001, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}
}

// 本机电话设置
int CLN_Config::Deal09_0008(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	// Need_Confirm		Attr1	K_len1	Key1	V_len1	Value1
	// 01				00		03		"noM"	00 0B	"13700000000"

	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	int     iret = 0;

	char    szTempBuf[256] = {0};
	memcpy(szTempBuf, v_szBuf, v_dwLen);

	// 参数检查
	if( v_dwLen < 8 || v_szBuf[3] != 'n' || v_szBuf[4] != 'o' || v_szBuf[5] != 'M' )
	{
		PRTMSG(MSG_ERR, "Deal09_0008 len err!\n");
		iret = ERR_PAR;
	}

	// 设置后将会使用新的手机号，因此先给中心应答

	if( iret )	   // 失败
	{
		szTempBuf[0] = 0;
	} else
	{
		szTempBuf[1] = 1;
	}

	if( !g_objApplication.MakeAppFrame(0x09, 0x0008, szTempBuf, (v_szBuf[0] == 1 ? v_dwLen : 1), szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	// 应答后再设置新的手机号
	sleep(3);
	if( !iret )
	{
		tag1PComuCfg cfg;
		GetImpCfg( &cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
		memset(cfg.m_szTel, 0, sizeof(cfg.m_szTel) );
		memcpy(cfg.m_szTel, v_szBuf + 8, min(15, v_szBuf[7]) );

		if( strlen(cfg.m_szTel) != 0)
		{	
			cfg.m_bRegsiter = false;
			SetImpCfg(&cfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(cfg) );
		}

		char buf[2] = {0x35, 0x01};
		DataPush( buf, 2, DEV_DIAODU, DEV_SOCK, 2);

		char buf1[2] = {0x01, 0x03};
		DataPush(buf1, 2, DEV_DIAODU, DEV_UPDATE, 2);

		char szbuf[2];
		szbuf[0] = 0x01;	// 0x01 表示非中心协议帧
		szbuf[1] = 0x01;	// 0x01 表示TCP套接字连接成功
		DataPush((void*)szbuf, 2, DEV_SOCK, DEV_QIAN, LV3); 

	}

	return 0;
}

// 呼叫限制设置
int CLN_Config::Deal09_0009(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	// Need_Confirm	Attr1	K_len1	Key1	V_len1	Value1
	// 01			00		01		"p"		00 01	02

	// 控制状态：UBYTE。
	//	00h--禁止所有呼入，呼出；
	//	01h--只能与设置的号码通话；
	//	02h--取消所有限制，可以任意通话；
	//	03h--取消呼入限制，可以被呼叫；
	//	04h--取消呼出限制，可以呼出；

	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	int     iret = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;

	char    szKey[20] = {0};
	int     iVlen = 0;
	tag2QCallListCfg objCallListCfg;
	objCallListCfg.Init();

	// AKV解析
	for(int i = 1; i < v_dwLen;)
	{
		if( v_szBuf[i] == 0x10 || v_szBuf[i] == 0x11 || v_szBuf[i] == 0x12 )   // 若是复合AKV，则执行下一循环
		{
			i = i + 1 + v_szBuf[i+1] + 1;	// 跳过Attr、K_len、Key字段

			iVlen = 256 * v_szBuf[i] + v_szBuf[i+1];	// V_len

			if( (i + iVlen + 2) > v_dwLen )		   // 检查长度
			{
				iret = ERR_PAR;
				PRTMSG(MSG_NOR, "Deal09_0009 len err 1!\n");
				goto _DEAL09_0009_END;
			}

			i += 2;		// 再跳过V_len字段
			continue;
		} else if(v_szBuf[i] == 0x00)	  // 若普通AKV
		{
			i += 1;		//跳过Attr

			memset(szKey, 0, sizeof(szKey));
			memcpy(szKey, v_szBuf + i + 1, v_szBuf[i]);		// Key

			i += 1 + v_szBuf[i];	// 跳过Key

			iVlen = 256 * v_szBuf[i] + v_szBuf[i+1];	// V_len

			if( (i + iVlen + 2) > v_dwLen )		   // 检查长度
			{
				iret = ERR_PAR;
				PRTMSG(MSG_NOR, "Deal09_0009 len err 2!\n");
				goto _DEAL09_0009_END;
			}

			i += 2;

			if( 0 == strcmp(szKey, "p") )
			{
				objCallListCfg.m_bytCtrl = v_szBuf[i];
				i += 1;
			} else
			{
				memcpy(objCallListCfg.m_szTel[objCallListCfg.m_iTelCnt], v_szBuf + i, iVlen);
				objCallListCfg.m_iTelCnt++;
				i += iVlen;
			}
		} else
		{
			iret = ERR_PAR;
			PRTMSG(MSG_NOR, "Deal09_0009 err 3!\n");
			goto _DEAL09_0009_END;
		}
	}

	_DEAL09_0009_END:
	// 给中心应答
	if( iret )	   // 失败
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else
	{
		// 写入配置
		SetImpCfg((void*)&objCallListCfg, offsetof(tagSecondCfg, m_uni2QCallListCfg), sizeof(objCallListCfg));

		PutAKV_char(szTempBuf + dwTempLen, 0x01, "p", &objCallListCfg.m_bytCtrl, 1, &dwTempLen);

		PRTMSG(MSG_DBG, "objCallListCfg.m_bytCtrl = %02x\n", objCallListCfg.m_bytCtrl);
		for(int j = 0; j < objCallListCfg.m_iTelCnt; j++)
		{
			PRTMSG(MSG_DBG, "tel: %s\n", objCallListCfg.m_szTel[j]);
		}
	}

	if( !g_objApplication.MakeAppFrame(0x09, 0x0009, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}
}

// 连接参数设置
int CLN_Config::Deal09_0010(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	// Need_Confirm		Attr1	K_len1	Key1	V_len1	Value1
	// 01				00		04		"ipv4"	00 04	0A 40 5C 50

	// Attr2	K_len2	Key2	V_len2	Value2	Attr3	K_len3	Key3	V_len3	Value3
	// 00		04		"port"	00 02	2A F9	00		03		"apn"	00 05	"CMCWT"

	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	int     iret = 0;

	char    szTempBuf[256] = {0};

	memcpy(szTempBuf, v_szBuf, v_dwLen);

	// 参数检查
	if( v_dwLen != 35
		|| v_szBuf[3] != 'i' || v_szBuf[4] != 'p' || v_szBuf[5] != 'v' || v_szBuf[6] != '4'
		|| v_szBuf[15] != 'p' || v_szBuf[16] != 'o' || v_szBuf[17] != 'r' || v_szBuf[18] != 't'
		|| v_szBuf[25] != 'a' || v_szBuf[26] != 'p' || v_szBuf[27] != 'n'
	  )
	{
		PRTMSG(MSG_ERR, "Deal09_0008 len err!\n");
		iret = ERR_PAR;
	}

	// 设置后将会使用新的IP和端口，因此先给中心应答

	if( iret )	   // 失败
	{
		szTempBuf[0] = 0;
	} else
	{
		szTempBuf[1] = 1;
	}

	if( !g_objApplication.MakeAppFrame(0x09, 0x0008, szTempBuf, (v_szBuf[0] == 1 ? v_dwLen : 1), szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	// 应答后再设置
	sleep(5);
	if( !iret )
	{
		char szIP[16] = {0};
		char szPort[5] = {0};
		char szApn[20] = {0};

		// IP
		for(int i = 0; i < 4; i++)
		{
			G_iToStr((int)v_szBuf[i+9], szIP + 4*i, 3);
		}
		szIP[3] = szIP[7] = szIP[11] = 0x20;

		// 端口
		int iPort = v_szBuf[21] * 256 + v_szBuf[22];
		G_iToStr(iPort, szPort, 5);

		// APN
		memcpy(szApn, v_szBuf + 30, v_szBuf[29]);

		tag1QIpCfg cfg;
		GetImpCfg( (void*)&cfg, sizeof(cfg), offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );

		cfg.m_ulQianUdpIP = inet_addr(szIP);
		cfg.m_usQianUdpPort = htons( (ushort)atol(szPort) );
		memset(cfg.m_szApn, 0, sizeof(cfg.m_szApn) );
		memcpy(cfg.m_szApn, szApn, min(19, strlen(szApn)) );

		if( inet_addr(szIP) != 0 && atol(szPort) != 0 && strlen(szApn) != 0 )
		{
			SetImpCfg((void*)&cfg, offsetof(tagImportantCfg, m_uni1QComuCfg.m_ary1QIpCfg), sizeof(cfg) );

			char buf[2] = {0x35, 0x01};
			DataPush( buf, 2, DEV_DIAODU, DEV_SOCK, 2);
		}
	}

	return iret;
}

// 心跳间隔设置
int CLN_Config::Deal09_0011(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	// Need_Confirm	Attr1	K_len1	Key1	V_len1	Value1
	// 01			00		01		"i"		00 01	3C

	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	int     iret = 0;
	tag1PComuCfg obj1PComuCfg;

	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;

	szTempBuf[0] = 1;		// 默认为成功应答
	memcpy(szTempBuf + 1, v_szBuf, v_dwLen);

	// 参数检查
	if( v_dwLen != 7 || v_szBuf[3] != 'i' )
	{
		PRTMSG(MSG_ERR, "Deal09_0010 len err!\n");
		iret = ERR_PAR;
		goto _DEAL09_0011_END;
	}

	// 设置心跳时间
	GetImpCfg( (void*)&obj1PComuCfg, sizeof(obj1PComuCfg), offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) );
	obj1PComuCfg.m_iHeartKeepInterval = (int)v_szBuf[6];
	if( 0 != SetImpCfg( (void*)&obj1PComuCfg, offsetof(tagImportantCfg, m_uni1PComuCfg.m_obj1PComuCfg), sizeof(obj1PComuCfg) ) )
	{
		iret = ERR_WRITEFAIL;
		goto _DEAL09_0011_END;
	}

	_DEAL09_0011_END:
	if( iret )		   // 失败（若成功则有缺省值）
		szTempBuf[0] = 0;

	if( !g_objApplication.MakeAppFrame(0x09, 0x0011, szTempBuf, (v_szBuf[0] == 1 ? v_dwLen : 1), szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	return iret;
}

// 报告参数设置
int CLN_Config::Deal09_0012(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
	int     iret = 0;
	int     i = 0;
	bool    bNeedConfirm = false;
	char *akvp;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;

	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_ERR, "Deal09_0012 err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL09_0012_END;
	}
	tag2QAlertCfg objAlertCfg;
	GetSecCfg((void*)&objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));

	// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;

	if( i < v_dwLen )
	{
		int loop, msglen;
		msglen = v_dwLen - i;
		for( loop = 0; loop < 3; loop++ )
		{
			akvp = v_szBuf + i;
			if( i >= v_dwLen )
			{
				break;
			}
			if(!AKV_Jugre(akvp, akv, msglen))
			{
				PRTMSG(MSG_DBG, "0x09 0012 AKV err!!!!!\n");
				iret = ERR_PAR;
				goto _DEAL09_0012_END;
			}
			i += akv->AKVLen;
			msglen -= akv->AKVLen;
			if( 0 == strcmp(akv->Key, "i"))
			{
				if( 2 == akv->Vlen )
				{
					objAlertCfg.m_usInterval = (byte)akv->Value[0] * 256 + (byte)akv->Value[1];
					PRTMSG(MSG_DBG, "Deal09_0012 Interval %d \n", objAlertCfg.m_usInterval);
				}
			}
			if( 0 == strcmp(akv->Key, "l"))
			{
				if( 2 == akv->Vlen )
				{
					objAlertCfg.m_usAlertDelay = (byte)akv->Value[0] * 256 + (byte)akv->Value[1];
					PRTMSG(MSG_DBG, "Deal09_0012 usDelay %d \n", objAlertCfg.m_usAlertDelay);
				}
			}
			if( 0 == strcmp(akv->Key, "t"))
			{
				if( 2 == akv->Vlen )
				{
					objAlertCfg.m_usTimes = (byte)akv->Value[0] * 256 + (byte)akv->Value[1];
					objAlertCfg.m_usTimes = objAlertCfg.m_usTimes ? objAlertCfg.m_usTimes : 65000;
					PRTMSG(MSG_DBG, "Deal09_0012 Times %d \n", objAlertCfg.m_usTimes);
				}
			}
		}
	}

	SetSecCfg((void*)&objAlertCfg, offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));
	FlushCntAndInterval(objAlertCfg.m_usTimes, objAlertCfg.m_usInterval);
	_DEAL09_0012_END:
	if( iret )
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else
	{
		szTempBuf[dwTempLen++] = 0x01;
	}
	if( bNeedConfirm )
	{
		// AKV: i

		PutAKV_ushort(szTempBuf + dwTempLen, 0x00, "i", objAlertCfg.m_usInterval, &dwTempLen);

		// AKV: t
		PutAKV_ushort(szTempBuf + dwTempLen, 0x00, "t", objAlertCfg.m_usTimes, &dwTempLen);

		// AKV: l
		PutAKV_ushort(szTempBuf + dwTempLen, 0x00, "l", objAlertCfg.m_usAlertDelay, &dwTempLen);

	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x09, 0x0012, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}
}

void CLN_Config::FlushCntAndInterval(ushort v_usTimes, ushort v_usInterval)
{
	int i;
	tag2QReportCfg objReportCfg;
	tag2QAreaRptCfg     objAreaRptCfg;  
	tag2QAreaSpdRptCfg  objAreaSpdRptCfg;
	tag2QAreaParkTimeRptCfg objAreaParkTimeRptCfg;
	tag2QLineRptCfg     objLineRptCfg;
	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg)); 
	GetSecCfg((void*)&objAreaRptCfg, sizeof(objAreaRptCfg), offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg)); 
	GetSecCfg((void*)&objAreaSpdRptCfg, sizeof(objAreaSpdRptCfg), offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg)); 
	GetSecCfg((void*)&objAreaParkTimeRptCfg, sizeof(objAreaParkTimeRptCfg), offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg)); 
	GetSecCfg((void*)&objLineRptCfg, sizeof(objLineRptCfg), offsetof(tagSecondCfg, m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg)); 
	objReportCfg.m_objAccRptPar.m_usCnt = v_usTimes;
	objReportCfg.m_objAccRptPar.m_usInterval = v_usInterval;
	objReportCfg.m_objBelowSpdRptPar.m_usCnt = v_usTimes;
	objReportCfg.m_objBelowSpdRptPar.m_usInterval = v_usInterval;
	objReportCfg.m_objCrossSpdRptPar.m_usCnt = v_usTimes;
	objReportCfg.m_objCrossSpdRptPar.m_usInterval = v_usInterval;
	objReportCfg.m_objDoorRptPar.m_usCnt = v_usTimes;
	objReportCfg.m_objDoorRptPar.m_usInterval = v_usInterval;
	objReportCfg.m_objFootRptPar.m_usCnt = v_usTimes;
	objReportCfg.m_objFootRptPar.m_usInterval = v_usInterval;
	objReportCfg.m_objMilePrtPar.m_usCnt = v_usTimes;
	objReportCfg.m_objMilePrtPar.m_usInterval = v_usInterval;
	objReportCfg.m_objOverSpdRptPar.m_usCnt = v_usTimes;
	objReportCfg.m_objOverSpdRptPar.m_usInterval = v_usInterval;
	objReportCfg.m_objPowOffRptPar.m_usCnt = v_usTimes;
	objReportCfg.m_objPowOffRptPar.m_usInterval = v_usInterval;
	objReportCfg.m_objStartEngPrtPar.m_usCnt = v_usTimes;
	objReportCfg.m_objStartEngPrtPar.m_usInterval = v_usInterval;
	objReportCfg.m_objTirePrtPar.m_usCnt = v_usTimes;
	objReportCfg.m_objTirePrtPar.m_usInterval = v_usInterval;
	for(i = 0; i < MAX_AREA_COUNT; i++)
	{
		objAreaRptCfg.m_objArea[i].m_usCnt = v_usTimes;
		objAreaRptCfg.m_objArea[i].m_usInterval = v_usInterval;
		objAreaRptCfg.m_objInArea[i].m_usCnt = v_usTimes;
		objAreaRptCfg.m_objInArea[i].m_usInterval = v_usInterval;
		objAreaRptCfg.m_objInRegion[i].m_usCnt = v_usTimes;
		objAreaRptCfg.m_objInRegion[i].m_usInterval = v_usInterval;
		objAreaRptCfg.m_objOutArea[i].m_usCnt = v_usTimes;
		objAreaRptCfg.m_objOutArea[i].m_usInterval = v_usInterval;
		objAreaRptCfg.m_objOutRegion[i].m_usCnt = v_usTimes;
		objAreaRptCfg.m_objOutRegion[i].m_usInterval = v_usInterval;
	}
	for(i = 0; i < MAX_AREA_COUNT; i++)
	{
		objAreaSpdRptCfg.m_objAreaSpd[i].m_usCnt = v_usTimes;
		objAreaSpdRptCfg.m_objAreaSpd[i].m_usInterval = v_usInterval;
		objAreaSpdRptCfg.m_objInRegionSpd[i].m_usCnt = v_usTimes;
		objAreaSpdRptCfg.m_objInRegionSpd[i].m_usInterval = v_usInterval;
		objAreaSpdRptCfg.m_objOutRegionSpd[i].m_usCnt = v_usTimes;
		objAreaSpdRptCfg.m_objOutRegionSpd[i].m_usInterval = v_usInterval;
	}
	for(i = 0; i < MAX_TIMESPD_COUNT; i++)
	{
		objAreaSpdRptCfg.m_TimeSpd[i].m_usCnt = v_usTimes;
		objAreaSpdRptCfg.m_TimeSpd[i].m_usInterval = v_usInterval;
	}
	for(i = 0; i < MAX_AREA_COUNT; i++)
	{
		objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_usCnt = v_usTimes;
		objAreaParkTimeRptCfg.m_objAreaParkTime[i].m_usInterval = v_usInterval;
		objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_usCnt = v_usTimes;
		objAreaParkTimeRptCfg.m_objInRegionParkTime[i].m_usInterval = v_usInterval;
		objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_usCnt = v_usTimes;
		objAreaParkTimeRptCfg.m_objOutRegionParkTime[i].m_usInterval = v_usInterval;
	}
	objAreaParkTimeRptCfg.m_objParkTime.m_usCnt = v_usTimes;
	objAreaParkTimeRptCfg.m_objParkTime.m_usInterval = v_usInterval;

	for(i = 0; i < MAX_LINE_COUNT; i++)
	{
		objLineRptCfg.m_objLine[i].m_usCnt = v_usTimes;
		objLineRptCfg.m_objLine[i].m_usInterval = v_usInterval;
		objLineRptCfg.m_objTimeLine[i].m_usCnt = v_usTimes;
		objLineRptCfg.m_objTimeLine[i].m_usInterval = v_usInterval;
	}

	SetSecCfg((void*)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg)); 
	SetSecCfg((void*)&objAreaRptCfg, offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg)); 
	SetSecCfg((void*)&objAreaSpdRptCfg, offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg)); 
	SetSecCfg((void*)&objAreaParkTimeRptCfg, offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg)); 
	SetSecCfg((void*)&objLineRptCfg, offsetof(tagSecondCfg, m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg)); 
}

// 主动上报规则设置: 对规则做简化处理，回传次数、时间间隔均不做处理，由传输层做统一重传
int CLN_Config::Deal0D_0000(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	// I_len Id								Need_Confirm  AreMove  Cnt		Interval
	// 0a	 32 63 64 65 33 33 38 31 66 62	00			  00	   00 03	00 0a

	// BMAP								TMAP				 EMAP
	// 00 01 36 00 06 0a 08 0d 09 37 1d 00 01 37 00 02 00 3c 00 01 31 00 04 00 00 00 1e

	char    szReplyBuf[1024] = {0};
	DWORD   dwReplyLen = 0;
	char    szTempBuf[1024] = {0};
	ushort   dwTempLen = 0;

	int     i = 0;
	int     iret = 0;
	bool    bNeedConfirm = false;

	char    szId[20] = {0};
	char    szBeginTime[6] = {0};
	char    szIntervalType = 0;
	DWORD   dwIntervalValue = 0;
	char    szEndType = 0;
	DWORD   dwReportCnt = 0;
	char    szEndTime[6] = {0};

	// 参数检查
	if( v_dwLen < v_szBuf[0] + 7 )
	{
		PRTMSG(MSG_ERR, "Deal0D_0000 err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL0D_0000_END;
	}

	// Id字段
	memcpy(szId, v_szBuf + i, min(sizeof(szId), v_szBuf[0]));
	i += v_szBuf[0] + 1;

	// 返回是否需携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i++;

	// 跳过AreMove, Cnt, Interval字段
	i += 5;

	// BMAP字段，若不是"Time"类型，则不处理
	i += 2;
	if( v_szBuf[i] != '6' )
	{
		PRTMSG(MSG_ERR, "Deal0D_0000 err 2, v_szbuf[i] = %02x\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEAL0D_0000_END;
	} else
	{
		i += 3;
		memcpy(szBeginTime, v_szBuf + i, 6);
		i += 6;
	}

	// 参数再检查
	if( v_dwLen < i )
	{
		PRTMSG(MSG_ERR, "Deal0D_0000 err 3,v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL0D_0000_END;
	}

	// TMAP字段，时间间隔或距离间隔，其他的不处理
	i += 2;
	if( v_szBuf[i] != '7' && v_szBuf[i] != '9' )
	{
		PRTMSG(MSG_ERR, "Deal0D_0000 err 4, v_szbuf[i] = %02x\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEAL0D_0000_END;
	} else
	{
		if( v_szBuf[i] == '7' )
		{
			szIntervalType = v_szBuf[i];
			i += 3;
			dwIntervalValue = v_szBuf[i] * 256 + v_szBuf[i+1];
			i += 2;
		} else
		{
			szIntervalType = v_szBuf[i];
			i += 3;
			dwIntervalValue = v_szBuf[i] * 256 * 256 * 256 + v_szBuf[i+1] * 256 * 256 + v_szBuf[i+2] * 256 + v_szBuf[i+3];
			i += 4;
		}
	}

	// 参数再检查
	if( v_dwLen < i )
	{
		PRTMSG(MSG_ERR, "Deal0D_0000 err 5, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL0D_0000_END;
	}

	// EMAP字段，次数或者时间，其他的不处理
	i += 2;
	if( v_szBuf[i] != '1' && v_szBuf[i] != '6' )
	{
		PRTMSG(MSG_ERR, "Deal0D_0000 err 6, v_szbuf[i] = %02x\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEAL0D_0000_END;
	} else
	{
		if( v_szBuf[i] == '1' )
		{
			szEndType = v_szBuf[i];	// 表示停止类型为次数条件
			i += 3;
			dwReportCnt = v_szBuf[i] * 256 * 256 * 256 + v_szBuf[i+1] * 256 * 256 + v_szBuf[i+2] * 256 + v_szBuf[i+3];
			i += 4;
		} else
		{
			szEndType = v_szBuf[i];	// 表示停止类型为时间条件
			i += 3;
			memcpy(szEndTime, v_szBuf + i, 6);
			i += 6;
		}
	}

	// 参数再检查
	if( v_dwLen < i )
	{
		PRTMSG(MSG_ERR, "Deal0D_0000 err 7, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL0D_0000_END;
	}

	// 若结束条件是时间条件，则判断时间是否合理
	if( szEndType == '6' )
	{
		// 先获取系统当前时间
		char szCurTime[6] = {0};
		time_t lCurrentTime;
		struct tm *pCurrentTime;
		time(&lCurrentTime);
		pCurrentTime = localtime(&lCurrentTime);

		// UTC时间转成北京时间
		struct tm pTemp;
		memcpy((void*)&pTemp, (void*)pCurrentTime, sizeof(pTemp));
		G_GetLocalTime(&pTemp);
		pCurrentTime = &pTemp;

		szCurTime[0] = (char)(pCurrentTime->tm_year - 100);
		szCurTime[1] = (char)(pCurrentTime->tm_mon + 1);
		szCurTime[2] = (char)pCurrentTime->tm_mday;
		szCurTime[3] = (char)pCurrentTime->tm_hour;
		szCurTime[4] = (char)pCurrentTime->tm_min;
		szCurTime[5] = (char)pCurrentTime->tm_sec;

		if( strcmp(szBeginTime, szEndTime) > 0 // 若结束时间早于起始时间
			||
			strcmp(szCurTime, szEndTime) > 0	// 或结束时间早于当前时间
		  )
		{
			PRTMSG(MSG_ERR, "Deal0D_0000 err 7!\n");
			iret = ERR_PAR;
			goto _DEAL0D_0000_END;
		}
	}

	_DEAL0D_0000_END:
	if( iret )	   // 失败
	{
		memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
		dwTempLen += (DWORD)v_szBuf[0] + 1;
		szTempBuf[dwTempLen++] = 0x00;
	} else				// 成功
	{
		memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
		dwTempLen += (DWORD)v_szBuf[0] + 1;
		szTempBuf[dwTempLen++] = 0x01;

		if( bNeedConfirm ) // 若需携带确认参数
		{
			// AKV: ar
			szTempBuf[dwTempLen++] = 0x00;	// Attr
			szTempBuf[dwTempLen++] = 0x02;	// K_len
			szTempBuf[dwTempLen++] = 'a';	// Key
			szTempBuf[dwTempLen++] = 'r';
			szTempBuf[dwTempLen++] = 0x00;	// V_len
			szTempBuf[dwTempLen++] = 0x01;
			szTempBuf[dwTempLen++] = 0x01;	// Value —— 删除

			// AKV: ts
			szTempBuf[dwTempLen++] = 0x00;	// Attr
			szTempBuf[dwTempLen++] = 0x02;	// K_len
			szTempBuf[dwTempLen++] = 't';	// Key
			szTempBuf[dwTempLen++] = 's';
			szTempBuf[dwTempLen++] = 0x00;	// V_len
			szTempBuf[dwTempLen++] = 0x01;
			szTempBuf[dwTempLen++] = 0x03;	// Value

			// AKV: i
			szTempBuf[dwTempLen++] = 0x00;	// Attr
			szTempBuf[dwTempLen++] = 0x01;	// K_len
			szTempBuf[dwTempLen++] = 'i';	// Key
			szTempBuf[dwTempLen++] = 0x00;	// V_len
			szTempBuf[dwTempLen++] = 0x01;
			szTempBuf[dwTempLen++] = 0x0a;	// Value

			// AKV: bM
			szTempBuf[dwTempLen++] = 0x10;	// Attr
			szTempBuf[dwTempLen++] = 0x02;	// K_len
			szTempBuf[dwTempLen++] = 'b';	// Key
			szTempBuf[dwTempLen++] = 'M';
			szTempBuf[dwTempLen++] = 0x00;	// V_len
			szTempBuf[dwTempLen++] = 0x0b;
			szTempBuf[dwTempLen++] = 0x00;	// Attr
			szTempBuf[dwTempLen++] = 0x01;	// K_len
			szTempBuf[dwTempLen++] = '6';	// Key
			szTempBuf[dwTempLen++] = 0x00;	// V_len
			szTempBuf[dwTempLen++] = 0x06;
			memcpy(szTempBuf + dwTempLen, szBeginTime, 6);	// Value
			dwTempLen += 6;

			// AKV: tM
			szTempBuf[dwTempLen++] = 0x10;	// Attr
			szTempBuf[dwTempLen++] = 0x02;	// K_len
			szTempBuf[dwTempLen++] = 't';	// Key
			szTempBuf[dwTempLen++] = 'M';
			szTempBuf[dwTempLen++] = 0x00;	// V_len
			szTempBuf[dwTempLen++] = 0x09;
			szTempBuf[dwTempLen++] = 0x00;	// Attr
			szTempBuf[dwTempLen++] = 0x01;	// K_len
			szTempBuf[dwTempLen++] = szIntervalType;	// Key
			szTempBuf[dwTempLen++] = 0x00;	// V_len
			szTempBuf[dwTempLen++] = 0x04;
			char buf[4] = {0};
			G_LocalToNet((char*)&dwIntervalValue, buf, 4);
			memcpy(szTempBuf + dwTempLen, buf, 4);	// Value
			dwTempLen += 4;

			// AKV: eM
			szTempBuf[dwTempLen++] = 0x10;	// Attr
			szTempBuf[dwTempLen++] = 0x02;	// K_len
			szTempBuf[dwTempLen++] = 'e';	// Key
			szTempBuf[dwTempLen++] = 'M';
			szTempBuf[dwTempLen++] = 0x00;	// V_len
			if( szEndType == '1' )
			{
				szTempBuf[dwTempLen++] = 0x0b;
				szTempBuf[dwTempLen++] = 0x00;	// Attr
				szTempBuf[dwTempLen++] = 0x01;	// K_len
				szTempBuf[dwTempLen++] = '1';	// Key
				szTempBuf[dwTempLen++] = 0x00;	// V_len
				szTempBuf[dwTempLen++] = 0x04;
				char buf[4] = {0};
				G_LocalToNet((char*)&dwReportCnt, buf, 4);
				memcpy(szTempBuf + dwTempLen, buf, 4);	// Value
				dwTempLen += 4;
			} else
			{
				szTempBuf[dwTempLen++] = 0x0b;
				szTempBuf[dwTempLen++] = 0x00;	// Attr
				szTempBuf[dwTempLen++] = 0x01;	// K_len
				szTempBuf[dwTempLen++] = '6';	// Key
				szTempBuf[dwTempLen++] = 0x00;	// V_len
				szTempBuf[dwTempLen++] = 0x06;
				memcpy(szTempBuf + dwTempLen, szEndTime, 6);	// Value
				dwTempLen += 6;
			}
		}
	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x0D, 0x0000, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	// 若是成功应答，则开始主动上报
	if( !iret )
	{
		g_objReport.BeginPositionReport(szId, szBeginTime, szIntervalType, dwIntervalValue, szEndType, dwReportCnt, szEndTime);
	}
}

// 主动上报规则移除
int CLN_Config::Deal0E_0000(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;

	memcpy(szTempBuf, v_szBuf, v_dwLen);
	dwTempLen = v_dwLen;

	szTempBuf[dwTempLen++] = 0x01;	//成功应答

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x0E, 0x0000, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}
}

// 紧急报告设置
int CLN_Config::Deal11_0001(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
	int     iret = 0;
	int     i = 0;
	bool    bNeedConfirm = false;
	byte    on_now = 0x01;
	char *akvp;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;

	tag2QReportPar objFootRptPar;
//	tag2QLNPhotoCfg objLNPhotoCfg;
//	objLNPhotoCfg.Init();
	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_ERR, "Deal11_0001 err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL11_0001_END;
	}
	//
	// RID
	memcpy(objFootRptPar.m_szRID, v_szBuf + 1, v_szBuf[0]);
	i += v_szBuf[0] + 1;
	//
	// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;
	//
	// 	// On_now
	on_now = v_szBuf[i];
	if( on_now != 0x01 )
	{
		PRTMSG(MSG_ERR, "Deal11_0001 err 2, On_now=%d\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEAL11_0001_END;
	}
	i += 1;
	objFootRptPar.m_bEnable = true;
	//
	// 	// Priority
	objFootRptPar.m_bytPriority = (byte)v_szBuf[i];
	i += 1;
	// 	cnt
	objFootRptPar.m_usCnt = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	//interval
	objFootRptPar.m_usInterval = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;

	tag2QReportCfg objReportCfg;

	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	memcpy(&objReportCfg.m_objFootRptPar, &objFootRptPar, sizeof(objFootRptPar));
	SetSecCfg((void*)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));

//	GetSecCfg( &objLNPhotoCfg, sizeof(objLNPhotoCfg), offsetof(tagSecondCfg, m_uni2QLNPhotoCfg.m_obj2QLNPhotoCfg), sizeof(objLNPhotoCfg) );
//	objLNPhotoCfg.m_wPhotoUploadSymb |= PHOTOUPLOAD_FOOTALERM;	
//	SetSecCfg( &objLNPhotoCfg, offsetof(tagSecondCfg, m_uni2QLNPhotoCfg.m_obj2QLNPhotoCfg), sizeof(objLNPhotoCfg) );
	_DEAL11_0001_END:
	memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
	dwTempLen += (DWORD)v_szBuf[0] + 1;
	if( iret )
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else
	{
		szTempBuf[dwTempLen++] = 0x01;
	}

	if( bNeedConfirm ) // 若需携带确认参数
	{
		// AKV: oN
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "oN", &on_now, 1, &dwTempLen);
		// AKV: p
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "p", &objFootRptPar.m_bytPriority, 1, &dwTempLen);
		// AKV: ts
		PutAKV_ushort(szTempBuf + dwTempLen, 0x00, "ts", objFootRptPar.m_usCnt, &dwTempLen);
		// AKV: i
		PutAKV_ushort(szTempBuf + dwTempLen, 0x00, "i", objFootRptPar.m_usInterval, &dwTempLen);
	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x11, 0x0001, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}
}

// 里程报告设置
int CLN_Config::Deal11_0002(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
	int     iret = 0;
	int     i = 0;
	bool    bNeedConfirm = false;
	char    on_now = 0x01;
	char *akvp;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;

	ushort mile = 0;
	tag2QReportPar objMileRptPar;

	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_ERR, "Deal11_0002 err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL11_0002_END;
	}
	//
	// RID
	memcpy(objMileRptPar.m_szRID, v_szBuf + 1, v_szBuf[0]);
	i += v_szBuf[0] + 1;
	//
	// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;
	//
	// 	// On_now
	on_now = v_szBuf[i];
	if( on_now != 0x01 )
	{
		PRTMSG(MSG_ERR, "Deal11_0002 err 2, On_now=%d\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEAL11_0002_END;
	}
	i += 1;
	objMileRptPar.m_bEnable = true;
	//
	// 	// Priority
	objMileRptPar.m_bytPriority = (byte)v_szBuf[i];
	i += 1;
	// 	cnt
	objMileRptPar.m_usCnt = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	//interval
	objMileRptPar.m_usInterval = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	if( i < v_dwLen )
	{
		akvp = v_szBuf + i;
		if(!AKV_Jugre(akvp, akv, v_dwLen - i))
		{
			PRTMSG(MSG_DBG, "0x11 0002 AKV err!!!!!\n");
			iret = ERR_PAR;
			goto _DEAL11_0002_END;
		}
		if( 0 == strcmp(akv->Key, "me"))
		{
			if( 2 == akv->Vlen )
			{
				mile = (byte)akv->Value[0] * 256 + (byte)akv->Value[1];
				PRTMSG(MSG_DBG, "Rcv mile %d km!\n", mile);
			}
		}
	}
	tag2QReportCfg objReportCfg;
	tag2QAlertCfg objAlertCfg;
	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	GetSecCfg((void*)&objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));
	memcpy(&objReportCfg.m_objMilePrtPar, &objMileRptPar, sizeof(objMileRptPar));
	objAlertCfg.m_usTotalMile = mile;
	SetSecCfg((void*)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	SetSecCfg((void*)&objAlertCfg, offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));

	_DEAL11_0002_END:
	memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
	dwTempLen += (DWORD)v_szBuf[0] + 1;
	if( iret )
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else
	{
		szTempBuf[dwTempLen++] = 0x01;
	}

	if( bNeedConfirm ) // 若需携带确认参数
	{
		// AKV: me
		PutAKV_ushort(szTempBuf + dwTempLen, 0x00, "me", mile, &dwTempLen);
	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x11, 0x0002, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}
	if( on_now )
	{
		g_objReport.BeginMileRptCheck();
	}
}

// 非法开车门报告设置
int CLN_Config::Deal11_0003(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{

}

// 超时停车报告设置
int CLN_Config::Deal11_0005(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealAreaPark(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x05, 0x0005);
}

// 区域报告设置
int CLN_Config::Deal11_0006(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealAreaRpt(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x06, 0x0006);
}

// 速度异常报告设置
int CLN_Config::Deal11_0007(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	// R_len	RID								Need_confirm	On_now	Priority	Cnt		Interval
	// 0a		33 64 30 31 63 36 64 63 35 36	01				01		04			00 00	00 3c

	// t					s					l
	// 00 01 74 00 01 01	00 01 73 00 01 64	00 01 6c 00 02 00 03
	char *DEBUGMSG = "Deal11_0007";
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	int     iret = 0;
	bool    bNeedConfirm = false;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;

	int     i = 0, limit;
	char    szId[20] = {0};
	ushort   usCnt = 0;
	ushort   usInterval = 0;
	byte    bytPriority = 0;
	byte    bytSpdType = 0;
	byte    bytSpeed = 0;
	byte    on_now;
	ushort   dwSpeedTime = 0;

	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;
	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_ERR, "Deal11_0007 err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL11_0007_END;
	}

	// RID
	memcpy(szId, v_szBuf + 1, v_szBuf[0]);
	i += v_szBuf[0] + 1;

	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;

	// On_now
	on_now = v_szBuf[i];
	if( on_now != 0x01 )
	{
		PRTMSG(MSG_ERR, "Deal11_0007 err 2, On_now=%d\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEAL11_0007_END;
	}
	i += 1;

	// Priority
	bytPriority = (byte)v_szBuf[i];
	i += 1;

	// Cnt
	usCnt = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i+1];
	i += 2;

	// Interval
	usInterval = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i+1];
	i += 2;
	DWORD fvlen;
	fvlen = v_dwLen - i;
	// 	// 进行AKV解析
	for(limit = 0; limit < 3; limit++ )
	{
		if( i >= v_dwLen )
		{
			break;
		}
		if(!AKV_Jugre(&v_szBuf[i], akv, fvlen))
		{
			PRTMSG(MSG_DBG, "Deal11_0007 AKV 1 err!!!!!\n");
			iret = ERR_PAR;
			goto _DEAL11_0007_END;
		}
		i += akv->AKVLen;
		switch( Key_Jugres(akv->Key) )
		{
		case 6:
			{
				if( !Get_char(akv, &bytSpdType, DEBUGMSG ))
				{

					iret = ERR_PAR;
					goto _DEAL11_0007_END;
				}
				if( bytSpdType != 1 && bytSpdType != 2 && bytSpdType != 3)
				{
					PRTMSG(MSG_ERR, "Deal11_0007 err 3, bytSpdTyep = %c\n", bytSpdType);
					iret = ERR_PAR;
					goto _DEAL11_0007_END;
				}
				PRTMSG(MSG_ERR, "Deal11_0007 bytSpdTyep = %d\n", bytSpdType);
			}
			break;
		case 24:
			{
				if( !Get_char(akv, &bytSpeed, DEBUGMSG ))
				{

					iret = ERR_PAR;
					goto _DEAL11_0007_END;
				}

				PRTMSG(MSG_ERR, "Deal11_0007 speed = %d\n", bytSpeed);
			}
			break;
		case 25:
			{
				if( !Get_ushort(akv, &dwSpeedTime, DEBUGMSG ))
				{

					iret = ERR_PAR;
					goto _DEAL11_0007_END;
				}

				PRTMSG(MSG_ERR, "Deal11_0007 SpeedTime = %d\n", dwSpeedTime);
			}
			break;
		default:
			{
				PRTMSG(MSG_ERR, "Deal11_0007 AKV ERR!!!");
				iret = ERR_PAR;
				goto _DEAL11_0007_END;
			}
			break;
		}
	}


	tag2QReportCfg  objReportCfg;
	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg) );

	tag2QAlertCfg   objAlertCfg;
	GetSecCfg((void*)&objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));

	// 若是超速设置
	if( bytSpdType == 0x01 )
	{
		objAlertCfg.m_bytOverSpd = bytSpeed;
		objAlertCfg.m_dwOverPrid = dwSpeedTime;

		memcpy(objReportCfg.m_objOverSpdRptPar.m_szRID, szId, v_szBuf[0]);
		objReportCfg.m_objOverSpdRptPar.m_bytPriority = bytPriority;
		objReportCfg.m_objOverSpdRptPar.m_usCnt = usCnt;
		objReportCfg.m_objOverSpdRptPar.m_usInterval = usInterval;
		objReportCfg.m_objOverSpdRptPar.m_bEnable = true;
	}
	// 若是低速设置
	else if( bytSpdType == 0x02 )
	{
		objAlertCfg.m_bytBelowSpd = bytSpeed;
		objAlertCfg.m_dwBelowPrid = dwSpeedTime;

		memcpy(objReportCfg.m_objBelowSpdRptPar.m_szRID, szId, v_szBuf[0]);
		objReportCfg.m_objBelowSpdRptPar.m_bytPriority = bytPriority;
		objReportCfg.m_objBelowSpdRptPar.m_usCnt = usCnt;
		objReportCfg.m_objBelowSpdRptPar.m_usInterval = usInterval;
		objReportCfg.m_objBelowSpdRptPar.m_bEnable = true;
	} 
	// 穿越设置值
	else if( bytSpdType == 0x03 )
	{
		objAlertCfg.m_bytCrossSpd = bytSpeed;
		objAlertCfg.m_dwCrossPrid = dwSpeedTime;

		memcpy(objReportCfg.m_objCrossSpdRptPar.m_szRID, szId, v_szBuf[0]);
		objReportCfg.m_objCrossSpdRptPar.m_bytPriority = bytPriority;
		objReportCfg.m_objCrossSpdRptPar.m_usCnt = usCnt;
		objReportCfg.m_objCrossSpdRptPar.m_usInterval = usInterval;
		objReportCfg.m_objCrossSpdRptPar.m_bEnable = true;
	}

	SetSecCfg((void*)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	SetSecCfg((void*)&objAlertCfg, offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));

	_DEAL11_0007_END:
	if( iret )	// 失败
	{
		memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
		dwTempLen += (DWORD)v_szBuf[0] + 1;
		szTempBuf[dwTempLen++] = 0x00;
	} else				 // 成功
	{
		memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
		dwTempLen += (DWORD)v_szBuf[0] + 1;
		szTempBuf[dwTempLen++] = 0x01;

		if( bNeedConfirm ) // 若需携带确认参数
		{
			// AKV: s
			PutAKV_char(szTempBuf + dwTempLen, 0x00, "s", &bytSpeed, 1, &dwTempLen);
		}
	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x11, 0x0007, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	// 若是成功应答，则开始进行超速或低速检查
	if( on_now )
	{
		g_objReport.BeginSpeedCheck();
	}
}


// 时间段速度异常报告设置
int CLN_Config::Deal11_0008(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealRegionSpd(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x0008, 0x0008);
}
//偏航报告设置请求
int CLN_Config::Deal11_0009(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealLineRpt(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x0009);
}
//主电源断电报告
int CLN_Config::Deal11_000E(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
	int     iret = 0;
	int     i = 0;
	bool    bNeedConfirm = false;
	char    on_now = 0x01;
	char *akvp;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;

	tag2QReportPar objPowerOffRptPar;

	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_ERR, "Deal11_000e err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL11_000E_END;
	}
	//
	// RID
	memcpy(objPowerOffRptPar.m_szRID, v_szBuf + 1, v_szBuf[0]);
	i += v_szBuf[0] + 1;
	//
	// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i    +=    1;
	//
	// 	// On_now
	on_now = v_szBuf[i];
	if( on_now != 0x01 )
	{
		PRTMSG(MSG_ERR, "Deal11_000e err 2, On_now=%d\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEAL11_000E_END;
	}
	i += 1;
	objPowerOffRptPar.m_bEnable = true;
	//
	// 	// Priority
	objPowerOffRptPar.m_bytPriority = (byte)v_szBuf[i];
	i += 1;
	// 	cnt
	objPowerOffRptPar.m_usCnt = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	//interval
	objPowerOffRptPar.m_usInterval    =    (   byte)v_szBuf[   i]    *    256    +    (   byte)v_szBuf[   i    +    1];
	i += 2;

	tag2QReportCfg objReportCfg;

	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	memcpy(&objReportCfg.m_objPowOffRptPar, &objPowerOffRptPar, sizeof(objPowerOffRptPar));
	SetSecCfg((void*)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));

	_DEAL11_000E_END:
	memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
	dwTempLen += (DWORD)v_szBuf[0] + 1;
	if( iret )
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else
	{
		szTempBuf[dwTempLen++] = 0x01;
	}

	if( bNeedConfirm ) // 若需携带确认参数
	{



	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x11, 0x000e, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

}

//点火报告设置
int CLN_Config::Deal11_000F(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
	int     iret = 0;
	int     i = 0;
	bool    bNeedConfirm = false;
	char    on_now = 0x01;
	char *akvp;
	byte accsta = 0;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;

	tag2QReportPar objStartEngRptPar;

	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_ERR, "Deal11_000F err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL11_000F_END;
	}
	//
	// RID
	memcpy(objStartEngRptPar.m_szRID, v_szBuf + 1, v_szBuf[0]);
	i += v_szBuf[0] + 1;
	//
	// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;
	//
	// 	// On_now
	on_now = v_szBuf[i];
	if( on_now != 0x01 )
	{
		PRTMSG(MSG_ERR, "Deal11_000F err 2, On_now=%d\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEAL11_000F_END;
	}
	i += 1;
	objStartEngRptPar.m_bEnable = true;
	//
	// 	// Priority
	objStartEngRptPar.m_bytPriority = (byte)v_szBuf[i];
	i += 1;
	// 	cnt
	objStartEngRptPar.m_usCnt = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	//interval
	objStartEngRptPar.m_usInterval = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;

	if( i < v_dwLen )
	{
		akvp = v_szBuf + i;
		if(!AKV_Jugre(akvp, akv, v_dwLen - i))
		{
			PRTMSG(MSG_DBG, "0x11 000f AKV err!!!!!\n");
			iret = ERR_PAR;
			goto _DEAL11_000F_END;
		}
		if( 0 == strcmp(akv->Key, "f"))
		{
			if( 1 == akv->Vlen )
			{
				accsta = (byte)akv->Value[0];
			}
		}
	}

	tag2QReportCfg objReportCfg;
	tag2QAlertCfg objAlertCfg;
	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	GetSecCfg((void*)&objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));
	objAlertCfg.m_bytAccSta = accsta;
	memcpy(&objReportCfg.m_objStartEngPrtPar, &objStartEngRptPar, sizeof(objStartEngRptPar));

	SetSecCfg((void*)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	SetSecCfg((void*)&objAlertCfg, offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));
	_DEAL11_000F_END:
	memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
	dwTempLen += (ushort)v_szBuf[0] + 1;
	if( iret )
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else
	{
		szTempBuf[dwTempLen++] = 0x01;
	}

	if( bNeedConfirm ) // 若需携带确认参数
	{
		// AKV: f
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "f", &accsta, 1, &dwTempLen);
	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x11, 0x000f, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}
	if( on_now )
	{
		g_objReport.BeginStartengCheck();
	}
}
//疲劳报告设置请求
int CLN_Config::Deal11_0014(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
	int     iret = 0;
	int     i = 0;
	bool    bNeedConfirm = false;
	char    on_now = 0x01;
	char *akvp;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;

	ushort drivetime = 0, resttime = 0;
	tag2QReportPar objTireRptPar;

	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_ERR, "Deal11_0014 err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEAL11_0014_END;
	}
	//
	// RID
	memcpy(objTireRptPar.m_szRID, v_szBuf + 1, v_szBuf[0]);
	i += v_szBuf[0] + 1;
	//
	// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;
	//
	// 	// On_now
	on_now = v_szBuf[i];
	if( on_now != 0x01 )
	{
		PRTMSG(MSG_ERR, "Deal11_00014 err 2, On_now=%d\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEAL11_0014_END;
	}
	i += 1;
	objTireRptPar.m_bEnable = true;
	//
	// 	// Priority
	objTireRptPar.m_bytPriority = (byte)v_szBuf[i];
	i += 1;
	// 	cnt
	objTireRptPar.m_usCnt = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	//interval
	objTireRptPar.m_usInterval = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;

	if( i < v_dwLen )
	{
		int loop, msglen;
		msglen = v_dwLen - i;
		for( loop = 0; loop < 2; loop++ )
		{
			akvp = v_szBuf + i;
			if( i >= v_dwLen )
			{
				break;
			}
			if(!AKV_Jugre(akvp, akv, msglen))
			{
				PRTMSG(MSG_DBG, "0x11 0014 AKV err!!!!!\n");
				iret = ERR_PAR;
				goto _DEAL11_0014_END;
			}
			i += akv->AKVLen;
			msglen -= akv->AKVLen;
			if( 0 == strcmp(akv->Key, "dT"))
			{
				if( 2 == akv->Vlen )
				{
					drivetime = (byte)akv->Value[0] * 256 + (byte)akv->Value[1];
				}
			}
			if( 0 == strcmp(akv->Key, "rT"))
			{
				if( 2 == akv->Vlen )
				{
					resttime = (byte)akv->Value[0] * 256 + (byte)akv->Value[1];
				}
			}
		}
	}

	tag2QReportCfg objReportCfg;
	tag2QAlertCfg objAlertCfg;
	GetSecCfg((void*)&objReportCfg, sizeof(objReportCfg), offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	GetSecCfg((void*)&objAlertCfg, sizeof(objAlertCfg), offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));
	objAlertCfg.m_usDriveTime = drivetime;
	objAlertCfg.m_usRestTime = resttime;
	memcpy(&objReportCfg.m_objTirePrtPar, &objTireRptPar, sizeof(objTireRptPar));

	SetSecCfg((void*)&objReportCfg, offsetof(tagSecondCfg, m_uni2QReportCfg.m_obj2QReportCfg), sizeof(objReportCfg));
	SetSecCfg((void*)&objAlertCfg, offsetof(tagSecondCfg, m_uni2QAlertCfg.m_obj2QAlertCfg), sizeof(objAlertCfg));
	_DEAL11_0014_END:
	memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
	dwTempLen += (DWORD)v_szBuf[0] + 1;
	if( iret )
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else
	{
		szTempBuf[dwTempLen++] = 0x01;
	}

	if( bNeedConfirm ) // 若需携带确认参数
	//
	{
		// AKV: dT

		PutAKV_ushort(szTempBuf + dwTempLen, 0x00, "dT", drivetime, &dwTempLen);

		// AKV: rT
		PutAKV_ushort(szTempBuf + dwTempLen, 0x00, "rT", resttime, &dwTempLen);

	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x11, 0x0014, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}
	if( on_now )
	{
		g_objReport.BeginTireRptCheck();
	}
}
// 分时段区域超速报告设置请求
int CLN_Config::Deal11_0016(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealRegionSpd(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x16, 0x0016);
}

//分时段偏航报告设置请求
int CLN_Config::Deal11_0018(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealLineRpt(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x0018);
}
//区域超时停车报告设置
int CLN_Config::Deal11_0019(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealAreaPark(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x19, 0x0019);
}

//进区域报告设置
int CLN_Config::Deal11_0022(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealAreaRpt(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x01, 0x0022);
}
//出区域报告设置
int CLN_Config::Deal11_0023(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealAreaRpt(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x02, 0x0023);
}
//区域内报告设置
int CLN_Config::Deal11_0024(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealAreaRpt(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x03, 0x0024);
}
//区域外报告设置
int CLN_Config::Deal11_0025(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealAreaRpt(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x04, 0x0025);
}
//区域内超速报告
int CLN_Config::Deal11_0026(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealRegionSpd(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x03, 0x0026);
}
//区域外超速报告
int CLN_Config::Deal11_0027(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealRegionSpd(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x04, 0x0027);
}
//区域内超时停车报告设置
int CLN_Config::Deal11_0028(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealAreaPark(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x03, 0x0028);
}
//区域外超时停车报告设置
int CLN_Config::Deal11_0029(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	return DealAreaPark(v_szBuf, v_dwLen, v_dwCenterSEQ, 0x04, 0x0029);
}

// 报告触发条件移除
int CLN_Config::Deal12(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	//  R_ACTION  R_len  RID

	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	char    szRidTemp[256] = {0};
	DWORD   dwRidLen = 0;
	char    szTempBuf[512] = {0};
	ushort   dwTempLen = 0;
	int iret = 0;
	ushort usActionType = 0;
	DWORD dwLocalType = 0;
	char  szbuf[2] = {0};
	char Rid[20] = {0};
	if( v_szBuf[2] > 20 || v_szBuf[2] + 2 > v_dwLen )
	{
		PRTMSG(MSG_DBG, "Deal12 MSG len:%d err!\n", v_dwLen);
		iret = ERR_PAR;
		goto _DEAL12_END;
	}
	memcpy(Rid, v_szBuf + 3, v_szBuf[2]);
	usActionType = (byte)v_szBuf[0] * 256 + (byte)v_szBuf[1];
	if( usActionType != 0xffff)
	{
		if(!g_objReport.ReportTyepToLocalType(usActionType, dwLocalType))
		{
			PRTMSG(MSG_DBG, "Deal12 action:%d err!\n", usActionType);
			iret = ERR_PAR;
			goto _DEAL12_END;
		}
	} else
	{
		dwLocalType = 0xffffffff;
	}
	if(!RemoveRpt(usActionType, Rid, szRidTemp, &dwRidLen))
	{
		PRTMSG(MSG_DBG, "Remove %d action err!\n", usActionType);
		iret = ERR_PAR;
		goto _DEAL12_END;
	}
	// 撤销报告
	g_objReport.SetAlert(false, dwLocalType, 0);
	_DEAL12_END:
	// 给中心应答
//	memcpy(szTempBuf, v_szBuf, 2);
//	dwTempLen += 2;
	if( !iret )
	{
		szTempBuf[dwTempLen++] = 0x01;	  // 成功
	} else
	{
		szTempBuf[dwTempLen++] = 0x00;	  // 
	}
	memcpy(szTempBuf + dwTempLen, szRidTemp, dwRidLen);
	dwTempLen += dwRidLen;

	if( !g_objApplication.MakeAppFrame(0x12, usActionType, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}
}

// 报告取消
int CLN_Config::Deal15(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ)
{
	//  R_ACTION  R_len  RID

	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	char    szTempBuf[256] = {0};
	ushort   dwTempLen = 0;
	int iret = 0;
	ushort usActionType = 0;
	DWORD dwLocalType = 0;
	char  szbuf[2] = {0};
	char Rid[20] = {0};
	if( v_szBuf[2] > 20 || v_szBuf[2] + 2 > v_dwLen )
	{
		PRTMSG(MSG_DBG, "Deal15 MSG len:%d err!\n", v_dwLen);
		iret = ERR_PAR;
		goto _DEAL15_END;
	}
	memcpy(Rid, v_szBuf + 3, v_szBuf[2]);
	usActionType = (byte)v_szBuf[0] * 256 + (byte)v_szBuf[1];
	if( usActionType != 0xffff )
	{
		if(!g_objReport.ReportTyepToLocalType(usActionType, dwLocalType))
		{
			PRTMSG(MSG_DBG, "Deal15 action:%d err!\n", usActionType);
			iret = ERR_PAR;
			goto _DEAL15_END;
		}
	} else
	{
		dwLocalType = 0xffffffff;
	}

	// 撤销报告
	g_objReport.SetAlert(false, dwLocalType, 0);
	_DEAL15_END:
	// 给中心应答
//	memcpy(szTempBuf, v_szBuf, 2);
//	dwTempLen += 2;
	if( !iret )
	{
		szTempBuf[dwTempLen++] = 0x01;	  // 成功
	} else
	{
		szTempBuf[dwTempLen++] = 0x00;	  // 
	}
	memcpy(szTempBuf + dwTempLen, v_szBuf + 2, v_szBuf[2] + 1);
	dwTempLen += v_szBuf[2] + 1;

	if( !g_objApplication.MakeAppFrame(0x15, usActionType, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}
}

// 区域报告
int CLN_Config::DealAreaRpt(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ, char AreaAttr, ushort Action)
{
	// R_len	RID								Need_confirm	On_now	Priority	Cnt		Interval
	// 0a		61 37 35 37 30 61 63 65 32 61	01				01		04			00 00	00 3c

	//	11 01 54 00 15 10 01 31 00 10 00 01 65 00 03 17 2d 00 00 01 62 00 03 00 00 00 00 02 69 78 00 04 00 00 01 46 00 02 72 54 00 01 02 00 02 69 4f 00 01 02 11 02 72 53 00 36 10 01 31 00 16 00 03 6c 6f 6e 00 04 19 5b 30 08 00 03 6c 61 74 00 04 05 41 d5 20 10 01 32 00 16 00 03 6c 6f 6e 00 04 19 5d 86 b4 00 03 6c 61 74 00 04 05 3f 1a 69

	// 	11 —— Attr
	// 	01 —— K_len
	// 	54 —— Key: 'T'
	// 	00 15 —— V_len
	//
	// 	10 —— Attr
	// 	01 —— K_len
	// 	31 —— Key: '1'
	// 	00 10 —— V_len
	//
	// 	00 —— Attr
	// 	01 —— K_len
	// 	65 —— Key: 'e'
	// 	00 03 —— V_len
	// 	17 2d 00 —— Value
	//
	// 	00 —— Attr
	// 	01 —— K_len
	// 	62 —— Key: 'b'
	// 	00 03 —— K_len
	// 	00 00 00
	//
	// 	00 —— Attr
	// 	02 —— K_len
	// 	69 78 —— Key: "ix"
	// 	00 04 —— V_len
	// 	00 00 01 46 —— Value
	//
	// 	00 —— Attr
	// 	02 —— K_len
	// 	72 54 —— Key: "rT"
	// 	00 01 —— V_len
	// 	02 —— Value：矩形区域
	//
	// 	00 —— Attr
	// 	02 —— K_len
	// 	69 4f —— Key："iO"
	// 	00 01 —— V_len
	// 	02 —— Value：出区域
	//
	// 	11 —— Attr
	// 	02 —— K_len
	// 	72 53 —— Key："rS"
	// 	00 36 —— V_len
	//
	// 	10 —— Attr	// 以下是左上角坐标点
	// 	01 —— K_len
	// 	31 —— Key
	// 	00 16 —— V_len
	//
	// 	00 —— Attr
	// 	03 —— K_len
	// 	6c 6f 6e —— Key：lon
	// 	00 04 —— V_len
	// 	19 5b 30 08 —— Value
	//
	// 	00 —— Attr
	// 	03 —— K_len
	// 	6c 61 74 —— key：lat
	// 	00 04 —— V_len
	// 	05 41 d5 20 —— Value
	//
	// 	10 —— Attr	// 以下是右下角坐标点
	// 	01 —— K_len
	// 	32 —— Key
	// 	00 16 —— V_len
	//
	// 	00 —— Attr
	// 	03 —— K_len
	// 	6c 6f 6e —— Key
	// 	00 04 —— V_len
	// 	19 5d 86 b4 —— Value
	//
	// 	00 —— Attr
	// 	03 —— K_len
	// 	6c 61 74 ——
	// 	00 04 —— V_len
	// 	05 3f 1a 69 —— Value
	char *DEBUGMSG = "DealAreaRpt";
	char    szReplyBuf[256] = {0};
	DWORD   dwReplyLen = 0;
	ulong index = 0, areacount = MAX_AREA_COUNT;
	int     iret = 0, limit;
	int     i = 0, a = 0;
	bool    bNeedConfirm = false;
	char    szTempBuf[256] = {0};
	ushort  dwTempLen = 0;
	char *akvp;
	byte regiontype = 0, on_now;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;
	tag2QAreaRptPar objAreaRptPar;
	objAreaRptPar.m_bytAreaAttr = AreaAttr;
	//
	// 	char	szkey[20] = {0};
	//
	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_ERR, "DealAreaRpt err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEALAREARPT_END;
	}
	//
	// RID
	memcpy(objAreaRptPar.m_szRID, v_szBuf + 1, v_szBuf[0]);
	i += v_szBuf[0] + 1;
	//
	// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;
	//
	// 	// On_now
	on_now = v_szBuf[i];
	if( on_now != 0x01 )
	{
		PRTMSG(MSG_ERR, "DealAreaRpt err 2, On_now=%d\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEALAREARPT_END;
	}
	i += 1;
	//
	// 	// Priority
	objAreaRptPar.m_bytPriority = (byte)v_szBuf[i];
	i += 1;
	//
	// 	// Cnt, Interval字段
	objAreaRptPar.m_usCnt = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	objAreaRptPar.m_usInterval = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	DWORD fvlen;
	fvlen = v_dwLen - i;
	tag2QAreaRptCfg objAreaRptCfg;
	GetSecCfg((void*)&objAreaRptCfg, sizeof(objAreaRptCfg), offsetof(tagSecondCfg, \
																	 m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));
	// 	// 进行AKV解析
	for(limit = 0; limit < 5; limit++ )
	{
		if( i >= v_dwLen )
		{
			break;
		}
		if(!AKV_Jugre(&v_szBuf[i], akv, fvlen))
		{
			PRTMSG(MSG_DBG, "DealAreaRpt AKV 1 err!!!!!\n");
			iret = ERR_PAR;
			goto _DEALAREARPT_END;
		}
		i += akv->AKVLen;
		switch( Key_Jugres(akv->Key) )
		{
		case 1:	//索引号
			//
			{
				if( !Get_ulong(akv, &index, DEBUGMSG) )
				{
					iret = ERR_PAR;
					goto _DEALAREARPT_END;
				}
				objAreaRptPar.m_ulIndex = index;
				PRTMSG(MSG_DBG, "DealAreaRpt index: %d\n", index);
			}
			break;
		case 2:	//区域状态 01h--进区域 02h--出区域 03h--区域内 04h--区域外
			//
			{
				if( !Get_char(akv, &objAreaRptPar.m_bytAreaAttr, DEBUGMSG))
				{
					iret = ERR_PAR;
					goto _DEALAREARPT_END;
				}
				if( objAreaRptPar.m_bytAreaAttr > 0x04 )
				{
					PRTMSG(MSG_DBG, "Areaattr over 4!!value= %d!!!!!\n", objAreaRptPar.m_bytAreaAttr);
					iret = ERR_PAR;
					goto _DEALAREARPT_END;
				}
				PRTMSG(MSG_DBG, "DealAreaRpt Areaattr: %d\n", objAreaRptPar.m_bytAreaAttr);
			}
			break;
		case 3:	//区域类型01h--圆形区域 02h--矩形区域 03h--多边形区域
			//
			{
				if( !Get_char(akv, &regiontype, DEBUGMSG) )
				{
					PRTMSG(MSG_DBG, "Get region type err!!\n");
					iret = ERR_PAR;
					goto _DEALAREARPT_END;
				}
				if( 0x02 != regiontype )
				{
					PRTMSG(MSG_DBG, "It can support rectangle only!!\n");
					iret = ERR_PAR;
					goto _DEALAREARPT_END;
				}
				PRTMSG(MSG_DBG, "regiontype: %d\n", regiontype);
			}
			break;
		case 4:	//区域
			{
				AKV_rS_t akv_rs;
				if( !AKV_rS(akv, &akv_rs, regiontype, DEBUGMSG ))
				{
					PRTMSG(MSG_DBG, "Get rS err!!\n");
					iret = ERR_PAR;
					goto _DEALAREARPT_END;
				}
				objAreaRptPar.m_lMaxLat = akv_rs.m_lMaxLat;
				objAreaRptPar.m_lMinLon = akv_rs.m_lMinLon;
				objAreaRptPar.m_lMinLat = akv_rs.m_lMinLat;
				objAreaRptPar.m_lMaxLon = akv_rs.m_lMaxLon;
				PRTMSG(MSG_DBG, "Left lat :%u\n", objAreaRptPar.m_lMaxLat);
				PRTMSG(MSG_DBG, "Left lon :%u\n", objAreaRptPar.m_lMinLon);
				PRTMSG(MSG_DBG, "Right lat :%u\n", objAreaRptPar.m_lMinLat);
				PRTMSG(MSG_DBG, "Right lon :%u\n", objAreaRptPar.m_lMaxLon);
			}
			break;
		case 5:	//时间段集合
			{
				AKV_T_t akv_t;
				int result, cnt;
				result = AKV_T(akv, &akv_t, DEBUGMSG );
				if( !result || result > MAX_TIME_COUNT)
				{
					PRTMSG(MSG_DBG, "Get T err!!\n");
					iret = ERR_PAR;
					goto _DEALAREARPT_END;
				}
				for( cnt = 0; cnt < result; cnt++ )
				{
					objAreaRptPar.m_bTimeEnable[cnt] = akv_t.m_bTimeEnable[cnt];
					memcpy(objAreaRptPar.m_szBeginTime[cnt], akv_t.m_szBeginTime[cnt], 3);
					PRTMSG(MSG_DBG, "Btime%d b:%d:%d:%d\n", cnt + 1, objAreaRptPar.m_szBeginTime[cnt][0], objAreaRptPar.m_szBeginTime[cnt][1], objAreaRptPar.m_szBeginTime[cnt][2]);
					memcpy(objAreaRptPar.m_szEndTime[cnt], akv_t.m_szEndTime[cnt], 3);
					PRTMSG(MSG_DBG, "Etime%d e:%d:%d:%d\n", cnt + 1, objAreaRptPar.m_szEndTime[cnt][0], objAreaRptPar.m_szEndTime[cnt][1], objAreaRptPar.m_szEndTime[cnt][2]);
				}
			}
			break;
		default:
			{
				PRTMSG(MSG_DBG, "AKV ERR!!!!!");
				iret = ERR_PAR;
				goto _DEALAREARPT_END;
			}
			break;
		}
	}
	//设置成功
//	PRTMSG(MSG_DBG, "Set succ!!!!!");
	objAreaRptPar.m_bEnable = true;
	if( 0x0022 == Action ) //进区域
	{
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaRptCfg.m_objInArea[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}
		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaRptCfg.m_objInArea[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount = areacount % MAX_AREA_COUNT;
		}
		memcpy(&objAreaRptCfg.m_objInArea[areacount], &objAreaRptPar, sizeof(objAreaRptPar));
	} else if( 0x0023 == Action )
	{//出区域
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaRptCfg.m_objOutArea[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}
		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaRptCfg.m_objOutArea[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount = areacount % MAX_AREA_COUNT;
		}
		memcpy(&objAreaRptCfg.m_objOutArea[areacount], &objAreaRptPar, sizeof(objAreaRptPar));
	} else if( 0x0024 == Action ) //区域内
	{
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaRptCfg.m_objInRegion[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}
		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaRptCfg.m_objInRegion[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount = areacount % MAX_AREA_COUNT;
		}
		memcpy(&objAreaRptCfg.m_objInRegion[areacount], &objAreaRptPar, sizeof(objAreaRptPar));
	} else if( 0x0025 == Action )
	{//区域外
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaRptCfg.m_objOutRegion[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}
		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaRptCfg.m_objOutRegion[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount = areacount % MAX_AREA_COUNT;
		}
		memcpy(&objAreaRptCfg.m_objOutRegion[areacount], &objAreaRptPar, sizeof(objAreaRptPar));
	} else
	{//0006
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaRptCfg.m_objArea[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}
		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaRptCfg.m_objArea[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount = areacount % MAX_AREA_COUNT;
		}
		memcpy(&objAreaRptCfg.m_objArea[areacount], &objAreaRptPar, sizeof(objAreaRptPar));
	}

	SetSecCfg((void*)&objAreaRptCfg, offsetof(tagSecondCfg, m_uni2QAreaRptCfg.m_obj2QAreaRptCfg), sizeof(objAreaRptCfg));
	_DEALAREARPT_END:
	memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
	dwTempLen += (DWORD)v_szBuf[0] + 1;
	if( iret )	// 失败
	{

		szTempBuf[dwTempLen++] = 0x00;
	} else				// 成功
	{
		szTempBuf[dwTempLen++] = 0x01;
	}
	if( bNeedConfirm ) // 若需携带确认参数
	{
		// AKV: ix
		PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "ix", index, &dwTempLen);

		if( 0x0006 == Action )
		{
			// AKV: iO
			PutAKV_char(szTempBuf + dwTempLen, 0x00, "iO", &objAreaRptPar.m_bytAreaAttr, 1, &dwTempLen);
		}

		// AKV: rT
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "rT", &regiontype, 1, &dwTempLen);

		// AKV: rS
		szTempBuf[dwTempLen++] = 0x11; // Attr
		szTempBuf[dwTempLen++] = 0x02; // K_len
		szTempBuf[dwTempLen++] = 'r';	 // Key
		szTempBuf[dwTempLen++] = 'S';	 // Key								//
		szTempBuf[dwTempLen++] = 0x00; // V_len
		szTempBuf[dwTempLen++] = 0x36;

		// AKV: 1
		szTempBuf[dwTempLen++] = 0x10; // Attr
		szTempBuf[dwTempLen++] = 0x01; // K_len
		szTempBuf[dwTempLen++] = '1';	 // Key
		szTempBuf[dwTempLen++] = 0x00; // V_len
		szTempBuf[dwTempLen++] = 0x16;
		//AKV:lat
		PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lat", objAreaRptPar.m_lMaxLat, &dwTempLen);

		//AKV:lon
		PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lon", objAreaRptPar.m_lMinLon, &dwTempLen);

		// AKV: 2
		szTempBuf[dwTempLen++] = 0x10; // Attr
		szTempBuf[dwTempLen++] = 0x01; // K_len
		szTempBuf[dwTempLen++] = '2';	 // Key
		szTempBuf[dwTempLen++] = 0x00; // V_len
		szTempBuf[dwTempLen++] = 0x16;
		//AKV:lat
		PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lat", objAreaRptPar.m_lMinLat, &dwTempLen);

		//AKV:lon
		PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lon", objAreaRptPar.m_lMaxLon, &dwTempLen);

	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x11, Action, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	// 若是成功应答，则开始进行区域检查
	if( on_now )
	{
		g_objReport.BeginAreaCheck();
	}
}

//区域超时停车报告
int CLN_Config::DealAreaPark(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ, char AreaAttr, ushort Action)
{
	char *DEBUGMSG = "DealAreaPark";
	char    szReplyBuf[1024] = {0};
	DWORD   dwReplyLen = 0;
	ulong index = 0, areacount = MAX_AREA_COUNT;
	int     iret = 0, limit;
	int     i = 0, a = 0;
	bool    bNeedConfirm = false;
	char    szTempBuf[1024] = {0};
	ushort  dwTempLen = 0;
	char *akvp;
	byte regiontype = 0, on_now = 0;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;
	tag2QAreaParkTimeRptPar objAreaParkTimeRptPar;
	objAreaParkTimeRptPar.m_bytAreaAttr = AreaAttr;
	//
	// 	char	szkey[20] = {0};
	//
	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_DBG, "DealAreaPark err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEALAREAPARK_END;
	}
	//
	// RID
	memcpy(objAreaParkTimeRptPar.m_szRID, v_szBuf + 1, v_szBuf[0]);
	i += v_szBuf[0] + 1;
	//
	// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;
	//
	// 	// On_now
	on_now = v_szBuf[i];
	if( on_now != 0x01 )
	{
		PRTMSG(MSG_DBG, "DealAreaPark err 2, On_now=%d\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEALAREAPARK_END;
	}
	i += 1;
	//
	// 	// Priority
	objAreaParkTimeRptPar.m_bytPriority = (byte)v_szBuf[i];
	i += 1;
	//
	// 	// 跳过Cnt, Interval字段
	objAreaParkTimeRptPar.m_usCnt = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	objAreaParkTimeRptPar.m_usInterval = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	DWORD fvlen;
	fvlen = v_dwLen - i;
	tag2QAreaParkTimeRptCfg objAreaParkTimeRptCfg;
	GetSecCfg((void*)&objAreaParkTimeRptCfg, sizeof(objAreaParkTimeRptCfg), offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg));
	// 	// 进行AKV解析
	for(limit = 0; limit < 6; limit++)
	{
		if( i >= v_dwLen )
		{

			break;
		}
		if(!AKV_Jugre(&v_szBuf[i], akv, fvlen))
		{
			PRTMSG(MSG_DBG, "AKV err!!!!!\n");
			iret = ERR_PAR;
			goto _DEALAREAPARK_END;
		}
		i += akv->AKVLen;
		switch( Key_Jugres(akv->Key) )
		{
		case 1:	//索引号
			{
				if( !Get_ulong(akv, &index, DEBUGMSG) )
				{
					iret = ERR_PAR;
					goto _DEALAREAPARK_END;
				}
				objAreaParkTimeRptPar.m_ulIndex = index;
				PRTMSG(MSG_DBG, "index: %d\n", index);
			}
			break;
		case 2:	//区域状态 01h--进区域 02h--出区域 03h--区域内 04h--区域外
			{
				if( !Get_char(akv, &objAreaParkTimeRptPar.m_bytAreaAttr, DEBUGMSG))
				{
					PRTMSG(MSG_DBG, "Areaattr err!!value= %d!!!!!\n", objAreaParkTimeRptPar.m_bytAreaAttr);
					iret = ERR_PAR;
					goto _DEALAREAPARK_END;
				}
				if( objAreaParkTimeRptPar.m_bytAreaAttr > 0x04 )
				{
					PRTMSG(MSG_DBG, "Areaattr over 4!!value= %d!!!!!\n", objAreaParkTimeRptPar.m_bytAreaAttr);
					iret = ERR_PAR;
					goto _DEALAREAPARK_END;
				}
				PRTMSG(MSG_DBG, "Areaattr: %d\n", objAreaParkTimeRptPar.m_bytAreaAttr);
			}
			break;
		case 3:	//区域类型01h--圆形区域 02h--矩形区域 03h--多边形区域
			{
				if( !Get_char(akv, &regiontype, DEBUGMSG) )
				{
					PRTMSG(MSG_DBG, "Get region type err!!\n");
					iret = ERR_PAR;
					goto _DEALAREAPARK_END;
				}
				if( 0x02 != regiontype )
				{
					PRTMSG(MSG_DBG, "It can support rectangle only!!\n");
					iret = ERR_PAR;
					goto _DEALAREAPARK_END;
				}
				PRTMSG(MSG_DBG, "regiontype: %d\n", regiontype);
			}
			break;
		case 4:	//区域
			{
				AKV_rS_t akv_rs;
				if( !AKV_rS(akv, &akv_rs, regiontype, DEBUGMSG ))
				{
					PRTMSG(MSG_DBG, "Get rS err!!\n");
					iret = ERR_PAR;
					goto _DEALAREAPARK_END;
				}
				objAreaParkTimeRptPar.m_lMaxLat = akv_rs.m_lMaxLat;
				objAreaParkTimeRptPar.m_lMinLon = akv_rs.m_lMinLon;
				objAreaParkTimeRptPar.m_lMinLat = akv_rs.m_lMinLat;
				objAreaParkTimeRptPar.m_lMaxLon = akv_rs.m_lMaxLon;
				PRTMSG(MSG_DBG, "Left lat :%u\n", objAreaParkTimeRptPar.m_lMaxLat);
				PRTMSG(MSG_DBG, "Left lon :%u\n", objAreaParkTimeRptPar.m_lMinLon);
				PRTMSG(MSG_DBG, "Right lat :%u\n", objAreaParkTimeRptPar.m_lMinLat);
				PRTMSG(MSG_DBG, "Right lon :%u\n", objAreaParkTimeRptPar.m_lMaxLon);
			}
			break;
		case 5:	//时间段集合
			{
				AKV_T_t akv_t;
				int result, cnt;
				result = AKV_T(akv, &akv_t, DEBUGMSG );
				if( !result || result > MAX_TIME_COUNT)
				{
					PRTMSG(MSG_DBG, "Get T err!!\n");
					iret = ERR_PAR;
					goto _DEALAREAPARK_END;
				}
				for( cnt = 0; cnt < result; cnt++ )
				{
					objAreaParkTimeRptPar.m_bTimeEnable[cnt] = akv_t.m_bTimeEnable[cnt];
					memcpy(objAreaParkTimeRptPar.m_szBeginTime[cnt], akv_t.m_szBeginTime[cnt], 3);
					PRTMSG(MSG_DBG, "Btime%d b:%d:%d:%d\n", cnt + 1, objAreaParkTimeRptPar.m_szBeginTime[cnt][0], objAreaParkTimeRptPar.m_szBeginTime[cnt][1], objAreaParkTimeRptPar.m_szBeginTime[cnt][2]);
					memcpy(objAreaParkTimeRptPar.m_szEndTime[cnt], akv_t.m_szEndTime[cnt], 3);
					PRTMSG(MSG_DBG, "Etime%d e:%d:%d:%d\n", cnt + 1, objAreaParkTimeRptPar.m_szEndTime[cnt][0], objAreaParkTimeRptPar.m_szEndTime[cnt][1], objAreaParkTimeRptPar.m_szEndTime[cnt][2]);
				}
			}
			break;
		case 26: //时间
			{
				if( !Get_ushort(akv, &objAreaParkTimeRptPar.m_usParkTime, DEBUGMSG) )
				{
					PRTMSG(MSG_DBG, "timeval par err!!\n");
					iret = ERR_PAR;
					goto _DEALAREAPARK_END;
				}
				PRTMSG(MSG_DBG, "parktime: %d\n", objAreaParkTimeRptPar.m_usParkTime);
			}
			break;
		default:
			{
				PRTMSG(MSG_DBG, " AKV  ERR!!!!!");
				iret = ERR_PAR;
				goto _DEALAREAPARK_END;
			}
			break;
		}
	}
	//设置成功
	objAreaParkTimeRptPar.m_bEnable = true;
	if( 0x0028 == Action )
	{
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaParkTimeRptCfg.m_objInRegionParkTime[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}
		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaParkTimeRptCfg.m_objInRegionParkTime[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount = areacount % MAX_AREA_COUNT;
		}
		memcpy(&objAreaParkTimeRptCfg.m_objInRegionParkTime[areacount], &objAreaParkTimeRptPar, sizeof(objAreaParkTimeRptPar));
		g_objReport.m_dwInRegPatkTime[areacount] = GetTickCount();
	} else if( 0x0029 == Action  )
	{
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaParkTimeRptCfg.m_objOutRegionParkTime[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}
		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaParkTimeRptCfg.m_objOutRegionParkTime[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount = areacount % MAX_AREA_COUNT;
		}
		memcpy(&objAreaParkTimeRptCfg.m_objOutRegionParkTime[areacount], &objAreaParkTimeRptPar, sizeof(objAreaParkTimeRptPar));
		g_objReport.m_dwOutRegPatkTime[areacount] = GetTickCount();
	} else if( 0x0019 == Action  )
	{
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaParkTimeRptCfg.m_objAreaParkTime[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}

		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaParkTimeRptCfg.m_objAreaParkTime[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount = areacount % MAX_AREA_COUNT;
		}
		memcpy(&objAreaParkTimeRptCfg.m_objAreaParkTime[areacount], &objAreaParkTimeRptPar, sizeof(objAreaParkTimeRptPar));
		g_objReport.m_dwAreaParkTime[areacount] = GetTickCount();
	} else if( 0x0005 == Action  )
	{
		memcpy(&objAreaParkTimeRptCfg.m_objParkTime, &objAreaParkTimeRptPar, sizeof(objAreaParkTimeRptPar));
		g_objReport.m_dwParkTime = GetTickCount();

	}
	SetSecCfg((void*)&objAreaParkTimeRptCfg, offsetof(tagSecondCfg, m_uni2QAreaParkTimeRptCfg.m_obj2QAreaParkTimeRptCfg), sizeof(objAreaParkTimeRptCfg));

	_DEALAREAPARK_END:
	memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
	dwTempLen += (DWORD)v_szBuf[0] + 1;
	if( iret )	// 失败
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else				// 成功
	{
		szTempBuf[dwTempLen++] = 0x01;
	}
	if( bNeedConfirm ) // 若需携带确认参数
	{
		if( 0x0005 != Action )
		{
			// AKV: ix
			PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "ix", index, &dwTempLen);
			if( 0x0019 == Action )
			{
				// AKV: iO
				PutAKV_char(szTempBuf + dwTempLen, 0x00, "iO", &objAreaParkTimeRptPar.m_bytAreaAttr, 1, &dwTempLen);
			}

			// AKV: rT
			PutAKV_char(szTempBuf + dwTempLen, 0x00, "rT", &regiontype, 1, &dwTempLen);

			// AKV: rS
			szTempBuf[dwTempLen++] = 0x11; // Attr
			szTempBuf[dwTempLen++] = 0x02; // K_len
			szTempBuf[dwTempLen++] = 'r';	 // Key
			szTempBuf[dwTempLen++] = 'S';	 // Key								//
			szTempBuf[dwTempLen++] = 0x00; // V_len
			szTempBuf[dwTempLen++] = 0x36;

			// AKV: 1
			szTempBuf[dwTempLen++] = 0x10; // Attr
			szTempBuf[dwTempLen++] = 0x01; // K_len
			szTempBuf[dwTempLen++] = '1';	 // Key
			szTempBuf[dwTempLen++] = 0x00; // V_len
			szTempBuf[dwTempLen++] = 0x16;
			//AKV:lat
			PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lat", objAreaParkTimeRptPar.m_lMaxLat, &dwTempLen);


			//AKV:lon
			PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lon", objAreaParkTimeRptPar.m_lMinLon, &dwTempLen);

			// AKV: 2
			szTempBuf[dwTempLen++] = 0x10; // Attr
			szTempBuf[dwTempLen++] = 0x01; // K_len
			szTempBuf[dwTempLen++] = '2';	 // Key
			szTempBuf[dwTempLen++] = 0x00; // V_len
			szTempBuf[dwTempLen++] = 0x16;
			//AKV:lat
			PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lat", objAreaParkTimeRptPar.m_lMinLat, &dwTempLen);
			//AKV:lon
			PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lon", objAreaParkTimeRptPar.m_lMaxLon, &dwTempLen);
			// AKV: pTO
		}
		PutAKV_ushort(szTempBuf + dwTempLen, 0x00, "pTO", objAreaParkTimeRptPar.m_usParkTime, &dwTempLen);
	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x11, Action, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	// 若是成功应答，则开始进行区域停车超时检查
	if( on_now )
	{
		g_objReport.BeginParkTimeRptCheck();
	}
}

//区域超速报告
int CLN_Config::DealRegionSpd(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ, char AreaAttr, ushort Action)
{
	char *DEBUGMSG = "DealRegionSpd";
	char    szReplyBuf[1024] = {0};
	DWORD   dwReplyLen = 0;
	ulong index = 0, areacount = MAX_AREA_COUNT;
	int     iret = 0, limit;
	int     i = 0, a = 0;
	bool    bNeedConfirm = false;
	bool    bindex = false, bT = false;
	char    szTempBuf[1024] = {0};
	ushort  dwTempLen = 0;
	char *akvp;
	byte regiontype = 0, on_now = 0;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;
	tag2QAreaSpdRptPar objAreaSpdRptPar;
	objAreaSpdRptPar.m_bytAreaAttr = AreaAttr;
	//
	// 	char	szkey[20] = {0};
	//
	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_ERR, "DealRegionSpd err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEALREGIONSPD_END;
	}
	//
	// RID
	memcpy(objAreaSpdRptPar.m_szRID, v_szBuf + 1, v_szBuf[0]);
	i += v_szBuf[0] + 1;
	//
	// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;
	//
	// 	// On_now
	on_now = v_szBuf[i];
	if( on_now != 0x01 )
	{
		PRTMSG(MSG_ERR, "DealRegionSpd err 2, On_now=%d\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEALREGIONSPD_END;
	}
	i += 1;
	//
	// 	// Priority
	objAreaSpdRptPar.m_bytPriority = (byte)v_szBuf[i];
	i += 1;
	//
	// 	// Cnt, Interval字段
	objAreaSpdRptPar.m_usCnt = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	objAreaSpdRptPar.m_usInterval = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	DWORD fvlen;
	fvlen = v_dwLen - i;
	tag2QAreaSpdRptCfg objAreaSpdRptCfg;
	GetSecCfg((void*)&objAreaSpdRptCfg, sizeof(objAreaSpdRptCfg), offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg));
	// 	// 进行AKV解析
	for(limit = 0; limit < 7; limit++)
	{
		if( i >= v_dwLen )
		{

			break;
		}
		if(!AKV_Jugre(&v_szBuf[i], akv, fvlen))
		{
			PRTMSG(MSG_DBG, "DealRegionSpd AKV err!!!!!\n");
			iret = ERR_PAR;
			goto _DEALREGIONSPD_END;
		}
		i += akv->AKVLen;
		switch( Key_Jugres(akv->Key) )
		{
		case 1:	//索引号
			{
				if( !Get_ulong(akv, &index, DEBUGMSG) )
				{
					iret = ERR_PAR;
					goto _DEALREGIONSPD_END;
				}
				objAreaSpdRptPar.m_ulIndex = index;
				bindex = true;
				PRTMSG(MSG_DBG, "DealRegionSpd index: %d\n", index);
			}
			break;
		case 2:	//区域状态 01h--进区域 02h--出区域 03h--区域内 04h--区域外
			{
				if( !Get_char(akv, &objAreaSpdRptPar.m_bytAreaAttr, DEBUGMSG))
				{
					iret = ERR_PAR;
					goto _DEALREGIONSPD_END;
				}
				if( objAreaSpdRptPar.m_bytAreaAttr > 0x04 )
				{
					PRTMSG(MSG_DBG, "Areaattr over 4!!value= %d!!!!!\n", objAreaSpdRptPar.m_bytAreaAttr);
					iret = ERR_PAR;
					goto _DEALREGIONSPD_END;
				}
				PRTMSG(MSG_DBG, "Areaattr: %d\n", objAreaSpdRptPar.m_bytAreaAttr);
			}
			break;
		case 3:	//区域类型01h--圆形区域 02h--矩形区域 03h--多边形区域
			{
				if( !Get_char(akv, &regiontype, DEBUGMSG) )
				{
					PRTMSG(MSG_DBG, "Get region type err!!\n");
					iret = ERR_PAR;
					goto _DEALREGIONSPD_END;
				}
				if( 0x02 != regiontype )
				{
					PRTMSG(MSG_DBG, "It can support rectangle only!!\n");
					iret = ERR_PAR;
					goto _DEALREGIONSPD_END;
				}
				PRTMSG(MSG_DBG, "regiontype: %d\n", regiontype);
			}
			break;
		case 4:	//区域
			{

				AKV_rS_t akv_rs;
				if( !AKV_rS(akv, &akv_rs, regiontype, DEBUGMSG ))
				{
					PRTMSG(MSG_DBG, "Get rS err!!\n");
					iret = ERR_PAR;
					goto _DEALREGIONSPD_END;
				}
				objAreaSpdRptPar.m_lMaxLat = akv_rs.m_lMaxLat;
				objAreaSpdRptPar.m_lMinLon = akv_rs.m_lMinLon;
				objAreaSpdRptPar.m_lMinLat = akv_rs.m_lMinLat;
				objAreaSpdRptPar.m_lMaxLon = akv_rs.m_lMaxLon;
				PRTMSG(MSG_DBG, "Left lat :%u\n", objAreaSpdRptPar.m_lMaxLat);
				PRTMSG(MSG_DBG, "Left lon :%u\n", objAreaSpdRptPar.m_lMinLon);
				PRTMSG(MSG_DBG, "Right lat :%u\n", objAreaSpdRptPar.m_lMinLat);
				PRTMSG(MSG_DBG, "Right lon :%u\n", objAreaSpdRptPar.m_lMaxLon);
			}
			break;
		case 5:	//时间段集合
			{
				AKV_T_t akv_t;
				int result, cnt;
				result = AKV_T(akv, &akv_t, DEBUGMSG );
				if( !result || result > MAX_TIME_COUNT)
				{
					PRTMSG(MSG_DBG, "Get T err!!\n");
					iret = ERR_PAR;
					goto _DEALREGIONSPD_END;
				}
				for( cnt = 0; cnt < result; cnt++ )
				{
					objAreaSpdRptPar.m_bTimeEnable[cnt] = akv_t.m_bTimeEnable[cnt];
					memcpy(objAreaSpdRptPar.m_szBeginTime[cnt], akv_t.m_szBeginTime[cnt], 3);
					PRTMSG(MSG_DBG, "Btime%d b:%d:%d:%d\n", cnt + 1, objAreaSpdRptPar.m_szBeginTime[cnt][0], objAreaSpdRptPar.m_szBeginTime[cnt][1], objAreaSpdRptPar.m_szBeginTime[cnt][2]);
					memcpy(objAreaSpdRptPar.m_szEndTime[cnt], akv_t.m_szEndTime[cnt], 3);
					PRTMSG(MSG_DBG, "Etime%d e:%d:%d:%d\n", cnt + 1, objAreaSpdRptPar.m_szEndTime[cnt][0], objAreaSpdRptPar.m_szEndTime[cnt][1], objAreaSpdRptPar.m_szEndTime[cnt][2]);
				}
				bT = true;
			}
			break;
		case 24: //速度
			{
				if( !Get_char(akv, &objAreaSpdRptPar.m_bytSpeed, DEBUGMSG) )
				{
					iret = ERR_PAR;
					goto _DEALREGIONSPD_END;
				}
				PRTMSG(MSG_DBG, "DealRegionSpd speed: %d\n", objAreaSpdRptPar.m_bytSpeed);
			}
			break;
		case 25: //产生报警的时间间隔
			{
				if( !Get_ushort(akv, &objAreaSpdRptPar.m_usTimeval, DEBUGMSG) )
				{
					PRTMSG(MSG_DBG, "timeval par err!!\n");
					iret = ERR_PAR;
					goto _DEALREGIONSPD_END;
				}
				PRTMSG(MSG_DBG, "DealRegionSpd report per:%d s\n", objAreaSpdRptPar.m_usTimeval);
			}
			break;
		default:
			{
				PRTMSG(MSG_DBG, "DealRegionSpd AKV ERR!!!!!");
				iret = ERR_PAR;
				goto _DEALREGIONSPD_END;
			}
			break;
		}
	}
	//设置成功
	objAreaSpdRptPar.m_bEnable = true;
	if( 0x0026 == Action )
	{
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaSpdRptCfg.m_objInRegionSpd[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}
		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaSpdRptCfg.m_objInRegionSpd[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount %= MAX_AREA_COUNT;
		}

		memcpy(&objAreaSpdRptCfg.m_objInRegionSpd[areacount], &objAreaSpdRptPar, sizeof(objAreaSpdRptPar));
	} else if( 0x0027 == Action )
	{
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaSpdRptCfg.m_objOutRegionSpd[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}
		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaSpdRptCfg.m_objOutRegionSpd[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount %= MAX_AREA_COUNT;
		}

		memcpy(&objAreaSpdRptCfg.m_objOutRegionSpd[areacount], &objAreaSpdRptPar, sizeof(objAreaSpdRptPar));
	} else if(0x0016 == Action)
	{
		for( a = 0; a < MAX_AREA_COUNT; a++ )
		{
			if(index == objAreaSpdRptCfg.m_objAreaSpd[a].m_ulIndex)
			{
				areacount = a;
				break;
			}
		}
		if( areacount == MAX_AREA_COUNT )
		{
			for(a = 0 ; a < MAX_AREA_COUNT; a++)
			{
				if(false == objAreaSpdRptCfg.m_objAreaSpd[a].m_bEnable)
				{
					areacount = a;
					break;
				}
			}
			areacount %= MAX_AREA_COUNT;
		}

		memcpy(&objAreaSpdRptCfg.m_objAreaSpd[areacount], &objAreaSpdRptPar, sizeof(objAreaSpdRptPar));
	} else if( 0x0008 == Action )
	{
		memcpy(&objAreaSpdRptCfg.m_TimeSpd[0], &objAreaSpdRptPar, sizeof(objAreaSpdRptPar));
	}

	SetSecCfg((void*)&objAreaSpdRptCfg, offsetof(tagSecondCfg, m_uni2QAreaSpdRptCfg.m_obj2QAreaSpdRptCfg), sizeof(objAreaSpdRptCfg));
	_DEALREGIONSPD_END:
	memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
	dwTempLen += (byte)v_szBuf[0] + 1;
	if( iret )	// 失败
	{
		szTempBuf[dwTempLen++] = 0x00;
	} else				// 成功
	{
		szTempBuf[dwTempLen++] = 0x01;
	}
	if( bNeedConfirm ) // 若需携带确认参数
	{
		if(bT)
		{
			// AKV: T
			szTempBuf[dwTempLen++] = 0x00; // Attr
			szTempBuf[dwTempLen++] = 0x01; // K_len
			szTempBuf[dwTempLen++] = 'T';	 // Key							//
			szTempBuf[dwTempLen++] = 0x00; // V_len
			ushort  vlenp = dwTempLen++;
			for(a = 0 ; a < MAX_TIME_COUNT; a++)
			{
				if(true == objAreaSpdRptPar.m_bTimeEnable[a])
				{
					szTempBuf[dwTempLen++] = 0x10; // Attr
					szTempBuf[dwTempLen++] = 0x01; // K_len
					szTempBuf[dwTempLen++] = a + 0x31; // Key							//
					szTempBuf[dwTempLen++] = 0x00; // V_len
					szTempBuf[dwTempLen++] = 0x10;

					PutAKV_char(szTempBuf + dwTempLen, 0x00, "b", objAreaSpdRptPar.m_szBeginTime[a], 3, &dwTempLen);
					PutAKV_char(szTempBuf + dwTempLen, 0x00, "e", objAreaSpdRptPar.m_szEndTime[a], 3, &dwTempLen);
					szTempBuf[vlenp] += 21;
				}
			}
		}
		// AKV: s
		PutAKV_char(szTempBuf + dwTempLen, 0x00, "s", &objAreaSpdRptPar.m_bytSpeed, 1, &dwTempLen);
		// AKV: ix
		if(bindex)
		{
			PutAKV_ulong(szTempBuf + dwTempLen, 0x00 , "ix", index, &dwTempLen);
		}

		if( 0x0008 != Action )
		{
			// AKV: rT
			PutAKV_char(szTempBuf + dwTempLen, 0x00, "rT", &regiontype, 1, &dwTempLen);
		}

		if( 0x0016 == Action )
		{
			// AKV: iO
			PutAKV_char(szTempBuf + dwTempLen, 0x00, "iO", &objAreaSpdRptPar.m_bytAreaAttr, 1, &dwTempLen);
		}
		if( 0x0008 != Action )
		{
			// AKV: rS
			szTempBuf[dwTempLen++] = 0x11; // Attr
			szTempBuf[dwTempLen++] = 0x02; // K_len
			szTempBuf[dwTempLen++] = 'r';	 // Key
			szTempBuf[dwTempLen++] = 'S';	 // Key								//
			szTempBuf[dwTempLen++] = 0x00; // V_len
			szTempBuf[dwTempLen++] = 0x36;

			// AKV: 1
			szTempBuf[dwTempLen++] = 0x10; // Attr
			szTempBuf[dwTempLen++] = 0x01; // K_len
			szTempBuf[dwTempLen++] = '1';	 // Key
			szTempBuf[dwTempLen++] = 0x00; // V_len
			szTempBuf[dwTempLen++] = 0x16;
			//AKV:lat
			PutAKV_ulong(szTempBuf + dwTempLen, 0x00 , "lat", objAreaSpdRptPar.m_lMaxLat, &dwTempLen);
			//AKV:lon
			PutAKV_ulong(szTempBuf + dwTempLen, 0x00 , "lon", objAreaSpdRptPar.m_lMinLon, &dwTempLen);

			// AKV: 2
			szTempBuf[dwTempLen++] = 0x10; // Attr
			szTempBuf[dwTempLen++] = 0x01; // K_len
			szTempBuf[dwTempLen++] = '2';	 // Key
			szTempBuf[dwTempLen++] = 0x00; // V_len
			szTempBuf[dwTempLen++] = 0x16;
			//AKV:lat
			PutAKV_ulong(szTempBuf + dwTempLen, 0x00 , "lat", objAreaSpdRptPar.m_lMinLat, &dwTempLen);
			//AKV:lon
			PutAKV_ulong(szTempBuf + dwTempLen, 0x00 , "lon", objAreaSpdRptPar.m_lMaxLon, &dwTempLen);
		}
	}

	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x11, Action, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	// 若是成功应答，则开始进行区域检查
	if( on_now )
	{
		g_objReport.BeginSpeedCheck();
	}

}

int CLN_Config::DealLineRpt(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ, ushort Action)
{
	//	11 00 09    0A 61 63 62 64 38 37 61 66 39 63   01   01   02     00 06   00 1E
	// 12 03 72 50 4C 00 D8
	// 10 01 31 00 16 00 03 6C 6F 6E 00 04 19 59 2E C4 00 03 6C 61 74 00 04 05 48 02 20 第一点
	// 10 01 32 00 16 00 03 6C 6F 6E 00 04 19 58 8A 82 00 03 6C 61 74 00 04 05 48 59 DD 第二点
	// 10 01 33 00 16 00 03 6C 6F 6E 00 04 19 58 CB BD 00 03 第三点
	char *DEBUGMSG = "DealLineRpt";
	char    szReplyBuf[8096] = {0};
	DWORD   dwReplyLen = 0, timelen, regionLen;
	ushort  vlenp, len = 0;
	ulong index = 0, linecount = MAX_LINE_COUNT;
	int     iret = 0, limit;
	int     i = 0, a = 0;
	bool    bNeedConfirm = false;
	char    szTempBuf[8096] = {0};
	ushort  dwTempLen = 0;
	char *akvp, regiontype = 0;
	byte on_now;
	AKV_t AKVTemp, *akv;
	akv = &AKVTemp;
	tag2QLineRptPar objLineRptPar;
	//
	// 	char	szkey[20] = {0};
	//
	// 参数检查
	if( v_dwLen < v_szBuf[0] + 8 )
	{
		PRTMSG(MSG_ERR, "DealLineRpt err 1, v_dwLen=%d, i=%d\n", v_dwLen, i);
		iret = ERR_PAR;
		goto _DEALLINERPT_END;
	}
	//
	// RID
	memcpy(objLineRptPar.m_szRID, v_szBuf + 1, v_szBuf[0]);
	i += v_szBuf[0] + 1;
	//
	// 	// 应答是否需要携带确认参数
	bNeedConfirm = (v_szBuf[i] == 1 ? true : false);
	i += 1;
	//
	// 	// On_now
	on_now = v_szBuf[i];
	if( on_now != 0x01 )
	{
		PRTMSG(MSG_ERR, "DealLineRpt err 2, On_now=%d\n", v_szBuf[i]);
		iret = ERR_PAR;
		goto _DEALLINERPT_END;
	}
	i += 1;
	//
	// 	// Priority
	objLineRptPar.m_bytPriority = (byte)v_szBuf[i];
	i += 1;
	//
	// Cnt, Interval字段
	objLineRptPar.m_usCnt = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	objLineRptPar.m_usInterval = (byte)v_szBuf[i] * 256 + (byte)v_szBuf[i + 1];
	i += 2;
	DWORD fvlen;
	fvlen = v_dwLen - i;
	tag2QLineRptCfg objLineRptCfg;
	GetSecCfg((void*)&objLineRptCfg, sizeof(objLineRptCfg), offsetof(tagSecondCfg, \
																	 m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg));
	// 	// 进行AKV解析
	for(limit = 0; limit < 4; limit++)
	{
		if( i >= v_dwLen )
		{

			break;
		}
		if(!AKV_Jugre(&v_szBuf[i], akv, fvlen))
		{
			PRTMSG(MSG_DBG, "DealLineRpt AKV 1 err!!!!!\n");
			iret = ERR_PAR;
			goto _DEALLINERPT_END;
		}
		i += akv->AKVLen;
		switch( Key_Jugres(akv->Key) )
		{
		case 1:	//索引号
			{
				if( !Get_ulong(akv, &index, DEBUGMSG) )
				{
					iret = ERR_PAR;
					goto _DEALLINERPT_END;
				}
				objLineRptPar.m_ulIndex = index;
				PRTMSG(MSG_DBG, "DealLineRpt index: %d\n", index);
			}
			break;
		case 27: //偏离距离
			{
				if( !Get_ushort(akv, &objLineRptPar.m_usOutval, DEBUGMSG) )
				{
					PRTMSG(MSG_DBG, "outval par err!!\n");
					iret = ERR_PAR;
					goto _DEALLINERPT_END;
				}
				PRTMSG(MSG_DBG, "DealLineRpt outval: %d\n", objLineRptPar.m_usOutval);
			}
			break;
		case 28: //线路
			{
				AKV_rPT_t akv_rpt;
				int result, cnt;
				result = AKV_rPT(akv, &akv_rpt, DEBUGMSG);
				if( !result)
				{
					iret = ERR_PAR;
					goto _DEALLINERPT_END;

				}
				for( cnt = 0; cnt < result; cnt++ )
				{
					objLineRptPar.m_bPointEnable[cnt] = akv_rpt.m_bPointEnable[cnt];
					objLineRptPar.m_lLat[cnt] = akv_rpt.m_lLat[cnt];
					PRTMSG(MSG_DBG, "%d Point lat :%u\n", cnt, objLineRptPar.m_lLat[cnt]);
					objLineRptPar.m_lLon[cnt] = akv_rpt.m_lLon[cnt];
					PRTMSG(MSG_DBG, "%d Point lon :%u\n", cnt, objLineRptPar.m_lLon[cnt]);
				}
			}
			break;
		case 5:	//时间段集合
			{
				AKV_T_t akv_t;
				int result, cnt;
				result = AKV_T(akv, &akv_t, DEBUGMSG );
				if( !result || result > MAX_TIME_COUNT)
				{
					PRTMSG(MSG_DBG, "Get T err!!\n");
					iret = ERR_PAR;
					goto _DEALLINERPT_END;
				}
				for( cnt = 0; cnt < result; cnt++ )
				{
					objLineRptPar.m_bTimeEnable[cnt] = akv_t.m_bTimeEnable[cnt];
					memcpy(objLineRptPar.m_szBeginTime[cnt], akv_t.m_szBeginTime[cnt], 3);
					PRTMSG(MSG_DBG, "Btime%d b:%d:%d:%d\n", cnt + 1, objLineRptPar.m_szBeginTime[cnt][0], objLineRptPar.m_szBeginTime[cnt][1], objLineRptPar.m_szBeginTime[cnt][2]);
					memcpy(objLineRptPar.m_szEndTime[cnt], akv_t.m_szEndTime[cnt], 3);
					PRTMSG(MSG_DBG, "Etime%d e:%d:%d:%d\n", cnt + 1, objLineRptPar.m_szEndTime[cnt][0], objLineRptPar.m_szEndTime[cnt][1], objLineRptPar.m_szEndTime[cnt][2]);
				}
			}
			break;
		default:
			{
				PRTMSG(MSG_DBG, "AKV ERR!!!!!");
				iret = ERR_PAR;
				goto _DEALLINERPT_END;
			}
			break;
		}
	}

	//设置成功
	objLineRptPar.m_bEnable = true;
	if( 0x0009 == Action ) //
	{
		for( a = 0; a < MAX_LINE_COUNT; a++ )
		{
			if(index == objLineRptCfg.m_objLine[a].m_ulIndex)
			{
				linecount = a;
				break;
			}
		}
		if( linecount == MAX_LINE_COUNT )
		{
			for(a = 0 ; a < MAX_LINE_COUNT; a++)
			{
				if(false == objLineRptCfg.m_objLine[a].m_bEnable)
				{
					linecount = a;
					break;
				}
			}
			linecount = linecount % MAX_LINE_COUNT;
		}
		memcpy(&objLineRptCfg.m_objLine[linecount], &objLineRptPar, sizeof(objLineRptPar));
	} else if( 0x0018 == Action ) //
	{
		for( a = 0; a < MAX_LINE_COUNT; a++ )
		{
			if(index == objLineRptCfg.m_objTimeLine[a].m_ulIndex)
			{
				linecount = a;
				break;
			}
		}
		if( linecount == MAX_LINE_COUNT )
		{
			for(a = 0 ; a < MAX_LINE_COUNT; a++)
			{
				if(false == objLineRptCfg.m_objTimeLine[a].m_bEnable)
				{
					linecount = a;
					break;
				}
			}
			linecount = linecount % MAX_LINE_COUNT;
		}
		memcpy(&objLineRptCfg.m_objTimeLine[linecount], &objLineRptPar, sizeof(objLineRptPar));
	} else
	{
		PRTMSG(MSG_DBG, "DealAreaRpt Action msg %d err!!!!\n", Action);
		iret = ERR_PAR;
		goto _DEALLINERPT_END;
	}
	SetSecCfg((void*)&objLineRptCfg, offsetof(tagSecondCfg, m_uni2QLineRptCfg.m_obj2QLineRptCfg), sizeof(objLineRptCfg));
	_DEALLINERPT_END:
	memcpy(szTempBuf + dwTempLen, v_szBuf, v_szBuf[0] + 1);
	dwTempLen += (DWORD)v_szBuf[0] + 1;
	if( iret )	// 失败
	{

		szTempBuf[dwTempLen++] = 0x00;
	} else				// 成功
	{
		szTempBuf[dwTempLen++] = 0x01;
	}
	if( bNeedConfirm ) // 若需携带确认参数
	{
		if( 0x0018 == Action )
		{
			// AKV: rPL
			szTempBuf[dwTempLen++] = 0x12; // Attr
			szTempBuf[dwTempLen++] = 0x03; // K_len
			szTempBuf[dwTempLen++] = 'r';	 // Key
			szTempBuf[dwTempLen++] = 'P';
			szTempBuf[dwTempLen++] = 'L';

			vlenp = dwTempLen;
			dwTempLen += 2;
			len = dwTempLen;
			for(a = 0 ; a < MAX_POINT_COUNT; a++)
			{
				if(true == objLineRptCfg.m_objTimeLine[linecount].m_bPointEnable[a])
				{
					szTempBuf[dwTempLen++] = 0x10; // Attr
					if( a <= 8 )
					{
						szTempBuf[dwTempLen++] = 0x01; // K_len
						szTempBuf[dwTempLen++] = a + 0x31; // Key							//
					} else
					{
						szTempBuf[dwTempLen++] = 0x02; // K_len
						szTempBuf[dwTempLen++] = (a + 1) / 10 + 0x30;	// Key
						szTempBuf[dwTempLen++] = (a + 1) % 10 + 0x30;	// Key
					}
					szTempBuf[dwTempLen++] = 0x00; // V_len
					szTempBuf[dwTempLen++] = 0x16;

					//AKV:lat
					PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lat", objLineRptCfg.m_objTimeLine[linecount].m_lLat[a], &dwTempLen);

					//AKV:lon
					PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lon", objLineRptCfg.m_objTimeLine[linecount].m_lLon[a], &dwTempLen);
				}
			}
			szTempBuf[vlenp++] = htons(dwTempLen - len) & 0xff;
			szTempBuf[vlenp++] = (htons(dwTempLen - len) >> 8) & 0xff;

			// AKV: T
			szTempBuf[dwTempLen++] = 0x00; // Attr
			szTempBuf[dwTempLen++] = 0x01; // K_len
			szTempBuf[dwTempLen++] = 'T';	 // Key							//
			szTempBuf[dwTempLen++] = 0x00; // V_len
			vlenp = dwTempLen++;
			for(a = 0 ; a < MAX_TIME_COUNT; a++)
			{
				if(true == objLineRptCfg.m_objTimeLine[linecount].m_bTimeEnable[a])
				{
					szTempBuf[dwTempLen++] = 0x10; // Attr
					szTempBuf[dwTempLen++] = 0x01; // K_len
					szTempBuf[dwTempLen++] = a + 0x31; // Key							//
					szTempBuf[dwTempLen++] = 0x00; // V_len
					szTempBuf[dwTempLen++] = 0x10;

					PutAKV_char(szTempBuf + dwTempLen, 0x00, "b", objLineRptCfg.m_objTimeLine[linecount].m_szBeginTime[a], 3, &dwTempLen);

					PutAKV_char(szTempBuf + dwTempLen, 0x00, "e", objLineRptCfg.m_objTimeLine[linecount].m_szEndTime[a], 3, &dwTempLen);
					szTempBuf[vlenp] += 21;
				}
			}
		} else
		{
			// AKV: rPL
			szTempBuf[dwTempLen++] = 0x12; // Attr
			szTempBuf[dwTempLen++] = 0x03; // K_len
			szTempBuf[dwTempLen++] = 'r';	 // Key
			szTempBuf[dwTempLen++] = 'P';
			szTempBuf[dwTempLen++] = 'L';

			vlenp = dwTempLen;
			dwTempLen += 2;
			len = dwTempLen;
			for(a = 0 ; a < MAX_POINT_COUNT; a++)
			{
				if(true == objLineRptCfg.m_objLine[linecount].m_bPointEnable[a])
				{
					szTempBuf[dwTempLen++] = 0x10; // Attr
					if( a <= 8 )
					{
						szTempBuf[dwTempLen++] = 0x01; // K_len
						szTempBuf[dwTempLen++] = a + 0x31; // Key							//
					} else
					{
						szTempBuf[dwTempLen++] = 0x02; // K_len
						szTempBuf[dwTempLen++] = (a + 1) / 10 + 0x30;	// Key
						szTempBuf[dwTempLen++] = (a + 1) % 10 + 0x30;	// Key
					}
					szTempBuf[dwTempLen++] = 0x00; // V_len
					szTempBuf[dwTempLen++] = 0x16;

					//AKV:lat
					PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lat", objLineRptCfg.m_objLine[linecount].m_lLat[a], &dwTempLen);

					//AKV:lon
					PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "lon", objLineRptCfg.m_objLine[linecount].m_lLon[a], &dwTempLen);
				}
			}
			szTempBuf[vlenp++] = htons(dwTempLen - len) & 0xff;
			szTempBuf[vlenp++] = (htons(dwTempLen - len) >> 8) & 0xff;
		}
		// AKV: ix
		PutAKV_ulong(szTempBuf + dwTempLen, 0x00, "ix", index, &dwTempLen);
		// AKV: yD
		PutAKV_ushort(szTempBuf + dwTempLen, 0x00, "yD", objLineRptPar.m_usOutval, &dwTempLen);
	}
	// 给中心应答
	if( !g_objApplication.MakeAppFrame(0x11, Action, szTempBuf, dwTempLen, szReplyBuf, sizeof(szReplyBuf), dwReplyLen) )
	{
		g_objApplication.SendAppFrame(MODE_A, 0x80, false, v_dwCenterSEQ, szReplyBuf, dwReplyLen);
	}

	// 
	if( on_now )
	{
		g_objReport.BeginOutLineRptCheck();
	}
}

#endif





































