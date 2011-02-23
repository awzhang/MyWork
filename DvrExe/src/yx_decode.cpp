#include "yx_common.h"

pthread_t gAVdecPlaybackThread;

FRAME_INDEX gAVFrameIndex;
FILE_BUFFER_S gAudioBuf[4]; 

/*索引文件操作函数*/
int IFile_Init(INDEX_BUFFER_S	*pIndexBuf, char	*fname)
{
	ASSERT_RETURN((NULL != pIndexBuf), -1);
	
	if( access(fname, F_OK) )
		return -1;
	
	pIndexBuf->pFile = fopen(fname, "rb");
	ASSERT_RETURN((NULL != pIndexBuf->pFile), -1);
	
	return 0;
}

int IFile_DeInit(INDEX_BUFFER_S *pIndexBuf)
{
	ASSERT_RETURN((NULL != pIndexBuf), -1);
	ASSERT_RETURN((NULL != pIndexBuf->pFile), -1);
	
	fclose(pIndexBuf->pFile);
	
	return 0;
}

int IFile_Fseek(INDEX_BUFFER_S *pIndexBuf, HI_U64 offset)
{
	ASSERT_RETURN((NULL != pIndexBuf), -1);
	ASSERT_RETURN((NULL != pIndexBuf->pFile), -1);
	
	fseek(pIndexBuf->pFile, offset, SEEK_CUR);
	
	return 0;
}

int IFile_GetFrameIndex(INDEX_BUFFER_S *pIndexBuf, FRAME_INDEX *pAVFrameIndex)
{
	FILE *pFile;
	int readlen;
	
	ASSERT_RETURN((NULL != pIndexBuf), -1);
	ASSERT_RETURN((NULL != pIndexBuf->pFile), -1);
	
	pFile = pIndexBuf->pFile;
	
	if(feof(pFile))
		return -1;
	
	readlen = fread(pAVFrameIndex, sizeof(FRAME_INDEX), 1, pFile);
	ASSERT_RETURN((1 == readlen), -1);
	
	return 0;
}

int IFile_Rewind(INDEX_BUFFER_S *pIndexBuf)
{    
	ASSERT_RETURN((NULL != pIndexBuf), -1);
	ASSERT_RETURN((NULL != pIndexBuf->pFile), -1);
	
	rewind(pIndexBuf->pFile);
	
	return 0;
}

/*音视频文件操作函数*/
int File_Init(FILE_BUFFER_S *pFileBuf, char *fname)
{
	ASSERT_RETURN((NULL != pFileBuf), -1);
	
	if( access(fname, F_OK) )
		return -1;
    
	pFileBuf->u32BufCurPos = 0;
	pFileBuf->u32BufEndPos = 0;
	pFileBuf->u32BufLen    = FILE_BUFFER;
	
	pFileBuf->pFile        = fopen(fname, "rb");
	ASSERT_RETURN((NULL != pFileBuf->pFile), -1);
	
	PRTMSG(MSG_NOR, "open file %s\n", fname);
    
	fseek(pFileBuf->pFile, 0, SEEK_END);
	pFileBuf->u32FileLen = ftell(pFileBuf->pFile);
	
	rewind(pFileBuf->pFile);
	
	return 0;
}

int File_DeInit(FILE_BUFFER_S *pFileBuf)
{
	ASSERT_RETURN((NULL != pFileBuf), -1);
	ASSERT_RETURN((NULL != pFileBuf->pFile), -1);
	
	fclose(pFileBuf->pFile);
	
	return 0;
}

int File_Fseek(FILE_BUFFER_S *pFileBuf, HI_U64 offset)
{
	ASSERT_RETURN((NULL != pFileBuf), -1);
	ASSERT_RETURN((NULL != pFileBuf->pFile), -1);
	
	fseek(pFileBuf->pFile, offset, SEEK_SET);
	
	return 0;
}

int File_Ftell(FILE_BUFFER_S *pFileBuf, HI_U64 offset)
{
	FILE *pFile;
	
	ASSERT_RETURN((NULL != pFileBuf), -1);
	ASSERT_RETURN((NULL != pFileBuf->pFile), -1);
	
	pFile = pFileBuf->pFile;
	
	if(ftell(pFile) >= offset)
		return -1;
	
	return 0;
}

int File_ReadStream(FILE_BUFFER_S *pFileBuf)
{
	int readlen, nFillPos, nFillLen;
	FILE *pFile;
    
	ASSERT_RETURN((NULL != pFileBuf), -1);
	ASSERT_RETURN((NULL != pFileBuf->pFile), -1);
	
	nFillPos = pFileBuf->u32BufEndPos;
	nFillLen = pFileBuf->u32BufLen - pFileBuf->u32BufEndPos;
	pFile    = pFileBuf->pFile;
	
	if(feof(pFile))
		return -1;
	
	if(pFileBuf->u32FileLen > nFillLen + ftell(pFile))
	{
		readlen = fread(&pFileBuf->u8Buf[nFillPos], nFillLen, 1, pFile);
		ASSERT_RETURN((1 == readlen), -1);
		
		readlen = nFillLen;
	}
	else
	{
		readlen = fread(&pFileBuf->u8Buf[nFillPos], 1, nFillLen, pFile);
		ASSERT_RETURN((readlen > 0), -1);
	}
    
	pFileBuf->u32BufEndPos = nFillPos + readlen;
	
	return 0;
}

int File_FindNextVSlice(FILE_BUFFER_S *pFileBuf)
{
	BYTE   *pu8Buf;
	int  nPos, nSliceLen = -1;
	
	ASSERT_RETURN((NULL != pFileBuf), -1);
	
	nPos   = pFileBuf->u32BufCurPos + 4;
	pu8Buf = pFileBuf->u8Buf;
    
	if (nPos + 4 > pFileBuf->u32BufEndPos)
		return -1;
	
	while((pu8Buf[nPos] != 0x00) || (pu8Buf[nPos+1]!= 0x00) || (pu8Buf[nPos+2] != 0x00) || (pu8Buf[nPos+3]!= 0x01))
	{
		nPos ++;
		if (nPos + 4 >= pFileBuf->u32BufEndPos)
			break;
	}
    
	if (nPos + 4 < pFileBuf->u32BufEndPos)
		nSliceLen = nPos - pFileBuf->u32BufCurPos;
	else
		return -1;
    
	return  nSliceLen;
}

int File_FindNextASlice(FILE_BUFFER_S *pFileBuf)
{
	BYTE   *pu8Buf;
	int  nPos, nSliceLen = -1;
	
	ASSERT_RETURN((NULL != pFileBuf), -1);
	
	nPos   = pFileBuf->u32BufCurPos + 4;
	pu8Buf = pFileBuf->u8Buf;
    
	if (nPos + 4 > pFileBuf->u32BufEndPos)
		return -1;
	
#if VEHICLE_TYPE == VEHICLE_M
	while( pu8Buf[nPos]!=0x00 || pu8Buf[nPos+1]!=0x01 || pu8Buf[nPos+2]!=0x0A || pu8Buf[nPos+3]!=0x00 )
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
		while( pu8Buf[nPos]!=0x00 || pu8Buf[nPos+1]!=0x01 || pu8Buf[nPos+2]!=0x3C || pu8Buf[nPos+3]!=0x00 )
#endif
		{
			nPos ++;
			if (nPos + 4 >= pFileBuf->u32BufEndPos)
				break;
		}
		
		if (nPos + 4 < pFileBuf->u32BufEndPos)
			nSliceLen = nPos - pFileBuf->u32BufCurPos;
		else
			return -1;
		
		return  nSliceLen;
}

int File_CopyData2Head(FILE_BUFFER_S *pFileBuf)
{
	BYTE   *pu8Buf;
	int  nCurPos, nLen;
	
	ASSERT_RETURN((NULL != pFileBuf), -1);
	
	pu8Buf  = pFileBuf->u8Buf;
	nCurPos = pFileBuf->u32BufCurPos;
	nLen    = pFileBuf->u32BufEndPos - nCurPos;
	
	if(nLen > 0)
		memmove (pu8Buf, &pu8Buf[nCurPos], nLen);
    
	pFileBuf->u32BufEndPos = nLen;
	pFileBuf->u32BufCurPos = 0;
	
	return 0;
}

int File_GetVSlice(FILE_BUFFER_S *pFileBuf, BYTE **ppAddr, uint *pLen)
{
	int iRet = 0;
	int s32SliceLen;
	ASSERT_RETURN((NULL != pFileBuf), -1);
	
	while(1)
	{
		iRet = 0;
		if(pFileBuf->u32BufEndPos < pFileBuf->u32BufLen)
		{
			iRet = File_ReadStream(pFileBuf);
		}
		
		s32SliceLen = File_FindNextVSlice(pFileBuf);
		if(s32SliceLen <= 0)
		{
			File_CopyData2Head(pFileBuf);
			s32SliceLen = pFileBuf->u32BufEndPos - pFileBuf->u32BufCurPos;
			if((s32SliceLen > 0))
			{
				if(iRet)
				{
					*pLen   = s32SliceLen;
					*ppAddr = pFileBuf->u8Buf;
					pFileBuf->u32BufEndPos = 0;
					break;
				}
			}
			else
			{
				if(iRet)
					return -1;
			}
		}
		else
		{
			*pLen   = s32SliceLen;
			*ppAddr = &(pFileBuf->u8Buf[pFileBuf->u32BufCurPos]);
			pFileBuf->u32BufCurPos += s32SliceLen;
			break;
		}
	}
	
	return 0;
}

int File_GetASlice(FILE_BUFFER_S *pFileBuf, BYTE **ppAddr, uint *pLen)
{
	int iRet;
	int iFlameLen;
	ASSERT_RETURN((NULL != pFileBuf), -1);
	
	while(1)
	{
		iRet = 0;
		if(pFileBuf->u32BufEndPos < pFileBuf->u32BufLen)
		{
			iRet = File_ReadStream(pFileBuf);
		}
		
		iFlameLen = File_FindNextASlice(pFileBuf);
		
		if(iFlameLen <= 0)
		{
			File_CopyData2Head(pFileBuf);
			iFlameLen = pFileBuf->u32BufEndPos - pFileBuf->u32BufCurPos;
			
			if((iFlameLen > 0))
			{
				if(iRet)
				{
					*pLen   = iFlameLen;
					*ppAddr = pFileBuf->u8Buf;
					pFileBuf->u32BufEndPos = 0;
					break;
				}
			}
			else
			{
				if(iRet)
					return -1;
			}
		}
		else
		{
			*pLen   = iFlameLen;
			*ppAddr = &(pFileBuf->u8Buf[pFileBuf->u32BufCurPos]);
			pFileBuf->u32BufCurPos += iFlameLen;
			break;
		}
	}
	
	return 0;
}

int File_Rewind(FILE_BUFFER_S *pFileBuf)
{    
	ASSERT_RETURN((NULL != pFileBuf), -1);
	ASSERT_RETURN((NULL != pFileBuf->pFile), -1);
	rewind(pFileBuf->pFile);
	
	return 0;
}

int StartVdec()
{
	int	iRet;
	
	PRTMSG(MSG_NOR, "Start Video Decode Device\n");
	
#if VEHICLE_TYPE == VEHICLE_M
	iRet = HI_VDEC_Open();
	if(iRet) goto ERROR;
#endif
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "start video decode device failed: %08x\n", iRet);
	return iRet;
}

int StopVdec()
{
	int	iRet;
	
	PRTMSG(MSG_NOR, "Stop Video Decode Device\n");
	
#if VEHICLE_TYPE == VEHICLE_M
	iRet = HI_VDEC_Close();
	if(iRet) goto ERROR;
#endif
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "stop video decode device failed: %08x\n", iRet);
	return iRet;
}

int VoDACfg(uint v_uimode, uint v_uinormal)
{
	int val = 0;
	int iRet;
	
#if VEHICLE_TYPE == VEHICLE_M  
	iRet = ioctl(g_objDevFd.Tw2835, TW2835_DAC_POW_ON, &val);
	if(iRet) goto ERROR;
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	IOSET(IOS_LCDPOW, IO_LCDPOW_ON);//开启LCD屏电源
	iRet = ioctl(g_objDevFd.Tw2865, TW2865CMD_VIDEO_OUT_ON, &val);
	if(iRet) goto ERROR;
#endif
	
	return 0;
	
ERROR:
	return iRet;
}

int VoPubCfg(uint v_uimode, uint v_uinormal)
{
	int iRet;
	
#if VEHICLE_TYPE == VEHICLE_M
	VO_PUBLIC_ATTR_S  objPubAttr;
	
	HI_VO_Init();
	
	iRet = HI_VO_GetPublicAttr(&objPubAttr);
	if(iRet) goto ERROR;
	
	objPubAttr.enable = HI_TRUE;
	objPubAttr.display_mode = VO_MODE_BT656;//TV模式
	objPubAttr.tv_config.enMaster = VIDEO_CONTROL_MODE_MASTER;
	objPubAttr.tv_config.compose_mode = (VIDEO_NORM_E)v_uinormal;
	objPubAttr.tv_config.seriatim_mode = HI_TRUE;
	objPubAttr.bOutputScale = HI_TRUE;//启用缩放
	objPubAttr.background_color = 0x0000FF; //默认背景为蓝色
	objPubAttr.vo_chn_img_num = 4;
	objPubAttr.pip_buf_num = 4;
	
	iRet = HI_VO_SetPublicAttr (&objPubAttr);
	if(iRet) goto ERROR;
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	VO_PUB_ATTR_S objPubAttr;
	
	objPubAttr.stTvConfig.stComposeMode = v_uinormal==PAL ? VIDEO_ENCODING_MODE_PAL : VIDEO_ENCODING_MODE_NTSC;	//显示模式
	objPubAttr.u32BgColor = 0x0000FF; //默认背景为蓝色
	
	iRet = HI_MPI_VO_SetPubAttr(&objPubAttr);
	if(iRet) goto ERROR;
	
	iRet = HI_MPI_VO_Enable();
	if(iRet) goto ERROR;
#endif
	
	return 0;
	
ERROR:
	return iRet;
}

int VoChnCfg(uint v_uimode, uint v_uinormal)
{
	int iRet;
	
#if VEHICLE_TYPE == VEHICLE_M
	VO_CHN_ATTR_S  objChnAttr;
	
	iRet = HI_VO_GetChnAttr(0, &objChnAttr);
	if(iRet) goto ERROR;
	
	objChnAttr.enable = HI_TRUE;
	objChnAttr.zoom_enable = HI_TRUE;
	
	if(v_uinormal == PAL)
	{
		switch (v_uimode)
		{			
			case CIF:
			{
				objChnAttr.rectangle.x = 0;
				objChnAttr.rectangle.y = 0;
				objChnAttr.rectangle.width = 352;
				objChnAttr.rectangle.height = 288; 					
				iRet = HI_VO_SetChnAttr (0, &objChnAttr);
				if(iRet) goto ERROR;
				
				objChnAttr.rectangle.x = 352;
				objChnAttr.rectangle.y = 0;
				objChnAttr.rectangle.width = 352;
				objChnAttr.rectangle.height = 288; 				
				iRet = HI_VO_SetChnAttr (1, &objChnAttr);
				if(iRet) goto ERROR;
				
				objChnAttr.rectangle.x = 0;
				objChnAttr.rectangle.y = 288;
				objChnAttr.rectangle.width = 352;
				objChnAttr.rectangle.height = 288; 					
				iRet = HI_VO_SetChnAttr (2, &objChnAttr);
				if(iRet) goto ERROR;
				
				objChnAttr.rectangle.x = 352;
				objChnAttr.rectangle.y = 288;
				objChnAttr.rectangle.width = 352;
				objChnAttr.rectangle.height = 288; 
				iRet = HI_VO_SetChnAttr (3, &objChnAttr);
				if(iRet) goto ERROR;
			}
			break;
            
			case HD1:
			{
				objChnAttr.rectangle.x = 0;
				objChnAttr.rectangle.y = 0;
				objChnAttr.rectangle.width = 720;
				objChnAttr.rectangle.height = 288; 
				iRet = HI_VO_SetChnAttr (0, &objChnAttr);
				if(iRet) goto ERROR;
				
				objChnAttr.rectangle.x = 0;
				objChnAttr.rectangle.y = 288;
				objChnAttr.rectangle.width = 720;
				objChnAttr.rectangle.height = 288; 
				iRet = HI_VO_SetChnAttr (1, &objChnAttr);
				if(iRet) goto ERROR;
			}	
			break;
			
			case D1:
			{
				objChnAttr.rectangle.x = 0;
				objChnAttr.rectangle.y = 0;
				objChnAttr.rectangle.width = 720;
				objChnAttr.rectangle.height = 576; 
				iRet = HI_VO_SetChnAttr (0, &objChnAttr);
				if(iRet) goto ERROR;
			}
			break;
            
			default: 
				break;
		}
	}
	else
	{
		switch (v_uimode)
		{
			case CIF:
			{
				objChnAttr.rectangle.x = 0;
				objChnAttr.rectangle.y = 0;
				objChnAttr.rectangle.width = 352;
				objChnAttr.rectangle.height = 240; 
				iRet = HI_VO_SetChnAttr (0, &objChnAttr);
				if(iRet) goto ERROR;
				
				objChnAttr.rectangle.x = 352;
				objChnAttr.rectangle.y = 0;
				objChnAttr.rectangle.width = 352;
				objChnAttr.rectangle.height = 240; 
				iRet = HI_VO_SetChnAttr (1, &objChnAttr);
				if(iRet) goto ERROR;
				
				objChnAttr.rectangle.x = 0;
				objChnAttr.rectangle.y = 240;
				objChnAttr.rectangle.width = 352;
				objChnAttr.rectangle.height = 240; 
				iRet = HI_VO_SetChnAttr (2, &objChnAttr);
				if(iRet) goto ERROR;
				
				objChnAttr.rectangle.x = 352;
				objChnAttr.rectangle.y = 240;
				objChnAttr.rectangle.width = 352;
				objChnAttr.rectangle.height = 240; 
				iRet = HI_VO_SetChnAttr (3, &objChnAttr);
				if(iRet) goto ERROR;
			}
				break;
            
			case HD1:
			{
				objChnAttr.rectangle.x = 0;
				objChnAttr.rectangle.y = 0;
				objChnAttr.rectangle.width = 720;
				objChnAttr.rectangle.height = 240; 
				iRet = HI_VO_SetChnAttr (0, &objChnAttr);
				if(iRet) goto ERROR;
				
				objChnAttr.rectangle.x = 0;
				objChnAttr.rectangle.y = 240;
				objChnAttr.rectangle.width = 720;
				objChnAttr.rectangle.height = 240; 
				iRet = HI_VO_SetChnAttr (1, &objChnAttr);
				if(iRet) goto ERROR;
			}
				break;
				
			case D1:
			{
				objChnAttr.rectangle.x = 0;
				objChnAttr.rectangle.y = 0;
				objChnAttr.rectangle.width = 720;
				objChnAttr.rectangle.height = 480; 
				iRet = HI_VO_SetChnAttr (0, &objChnAttr);
				if(iRet) goto ERROR;
			}
				break;
	            
			default: 
				break;
		}        
	}
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	int i;
	VO_CHN_ATTR_S objChnAttr;
	
	objChnAttr.u32Priority = 0;
	objChnAttr.bZoomEnable = HI_TRUE;
	
	if(v_uinormal == PAL)
	{
		switch (v_uimode)
		{
			case CIF:
				objChnAttr.stRect.s32X = 16;
				objChnAttr.stRect.s32Y = 0;
				objChnAttr.stRect.u32Width = 352;
				objChnAttr.stRect.u32Height = 288;
				iRet = HI_MPI_VO_SetChnAttr(0, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(0);
				if(iRet) goto ERROR;
				
				objChnAttr.stRect.s32X = 368;
				objChnAttr.stRect.s32Y = 0;
				objChnAttr.stRect.u32Width = 352;
				objChnAttr.stRect.u32Height = 288;
				iRet = HI_MPI_VO_SetChnAttr(1, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(1);
				if(iRet) goto ERROR;
				
				objChnAttr.stRect.s32X = 16;
				objChnAttr.stRect.s32Y = 288;
				objChnAttr.stRect.u32Width = 352;
				objChnAttr.stRect.u32Height = 288;
				iRet = HI_MPI_VO_SetChnAttr(2, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(2);
				if(iRet) goto ERROR;
				
				objChnAttr.stRect.s32X = 368;
				objChnAttr.stRect.s32Y = 288;
				objChnAttr.stRect.u32Width = 352;
				objChnAttr.stRect.u32Height = 288;
				iRet = HI_MPI_VO_SetChnAttr(3, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(3);
				if(iRet) goto ERROR;
				break;
	            
			case HD1:
				objChnAttr.stRect.s32X = 0;
				objChnAttr.stRect.s32Y = 0;
				objChnAttr.stRect.u32Width = 720;
				objChnAttr.stRect.u32Height = 288;
				iRet = HI_MPI_VO_SetChnAttr(0, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(0);
				if(iRet) goto ERROR;
				
				objChnAttr.stRect.s32X = 0;
				objChnAttr.stRect.s32Y = 288;
				objChnAttr.stRect.u32Width = 720;
				objChnAttr.stRect.u32Height = 288;
				iRet = HI_MPI_VO_SetChnAttr(1, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(1);
				if(iRet) goto ERROR;
				break;
				
			case D1:
				objChnAttr.stRect.s32X = 0;
				objChnAttr.stRect.s32Y = 0;
				objChnAttr.stRect.u32Width = 720;
				objChnAttr.stRect.u32Height = 576;
				iRet = HI_MPI_VO_SetChnAttr(0, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(0);
				if(iRet) goto ERROR;
				break;
	            
			default: 
				break;
		}
	}
	else
	{
		switch (v_uimode)
		{
			case CIF:
				objChnAttr.stRect.s32X = 16;
				objChnAttr.stRect.s32Y = 0;
				objChnAttr.stRect.u32Width = 352;
				objChnAttr.stRect.u32Height = 240;
				iRet = HI_MPI_VO_SetChnAttr(0, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(0);
				if(iRet) goto ERROR;
				
				objChnAttr.stRect.s32X = 368;
				objChnAttr.stRect.s32Y = 0;
				objChnAttr.stRect.u32Width = 352;
				objChnAttr.stRect.u32Height = 240;
				iRet = HI_MPI_VO_SetChnAttr(1, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(1);
				if(iRet) goto ERROR;
				
				objChnAttr.stRect.s32X = 16;
				objChnAttr.stRect.s32Y = 240;
				objChnAttr.stRect.u32Width = 352;
				objChnAttr.stRect.u32Height = 240;
				iRet = HI_MPI_VO_SetChnAttr(2, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(2);
				if(iRet) goto ERROR;
				
				objChnAttr.stRect.s32X = 368;
				objChnAttr.stRect.s32Y = 240;
				objChnAttr.stRect.u32Width = 352;
				objChnAttr.stRect.u32Height = 240;
				iRet = HI_MPI_VO_SetChnAttr(3, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(3);
				if(iRet) goto ERROR;
				break;
	            
			case HD1:
				objChnAttr.stRect.s32X = 0;
				objChnAttr.stRect.s32Y = 0;
				objChnAttr.stRect.u32Width = 720;
				objChnAttr.stRect.u32Height = 240;
				iRet = HI_MPI_VO_SetChnAttr(0, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(0);
				if(iRet) goto ERROR;
				
				objChnAttr.stRect.s32X = 0;
				objChnAttr.stRect.s32Y = 240;
				objChnAttr.stRect.u32Width = 720;
				objChnAttr.stRect.u32Height = 240;
				iRet = HI_MPI_VO_SetChnAttr(1, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(1);
				if(iRet) goto ERROR;
				break;
				
			case D1:
				objChnAttr.stRect.s32X = 0;
				objChnAttr.stRect.s32Y = 0;
				objChnAttr.stRect.u32Width = 720;
				objChnAttr.stRect.u32Height = 480;
				iRet = HI_MPI_VO_SetChnAttr(0, &objChnAttr);
				if(iRet) goto ERROR;
				iRet = HI_MPI_VO_EnableChn(0);
				if(iRet) goto ERROR;
				break;
	            
			default: 
				break;
		}        
	}
#endif
	
	return 0;
	
ERROR:
	return iRet;
}

int StartVo()
{
	PRTMSG(MSG_NOR, "Start Video Output Channel\n");
	
	int iRet;
	
	iRet = VoDACfg(g_objMvrCfg.VOutput.VMode, g_objMvrCfg.VOutput.VNormal);
	if(iRet) goto ERROR;
	
	iRet = VoPubCfg(g_objMvrCfg.VOutput.VMode, g_objMvrCfg.VOutput.VNormal);
	if(iRet) goto ERROR;
	
	iRet = VoChnCfg(g_objMvrCfg.VOutput.VMode, g_objMvrCfg.VOutput.VNormal);
	if(iRet) goto ERROR;
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "start video output channel failed: 0x%08x\n", iRet);
	return iRet;
}

int StopVo()
{
	int i;
	int iRet;
	int val;
	
	PRTMSG(MSG_NOR, "Stop Video Output Channel\n");
	
#if VEHICLE_TYPE == VEHICLE_M
	VO_CHN_ATTR_S objChnAttr;
	VO_PUBLIC_ATTR_S objPubAttr;
	
	for(i=0; i<4; i++)
	{
		iRet = HI_VO_GetChnAttr(i, &objChnAttr);
		if(iRet) goto ERROR;
		
		objChnAttr.enable = HI_FALSE;
		iRet = HI_VO_SetChnAttr(i, &objChnAttr);
		if(iRet) goto ERROR;
	}
	
	iRet = HI_VO_GetPublicAttr(&objPubAttr);
	if(iRet) goto ERROR;
	
	objPubAttr.enable = HI_FALSE;
	iRet = HI_VO_SetPublicAttr(&objPubAttr);
	if(iRet) goto ERROR;
	
	val = 0;
	iRet = ioctl(g_objDevFd.Tw2835, TW2835_DAC_POW_OFF, &val);
	if(iRet) goto ERROR;
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	for(i=0; i<=3; i++)
	{
		iRet = HI_MPI_VO_DisableChn(i);
		if(iRet) goto ERROR;
	}
	
	iRet = HI_MPI_VO_Disable();
	if(iRet) goto ERROR;
	
	BYTE ucIOState;
	IOGet( IOS_POWDETM, &ucIOState );
	if(ucIOState == IO_POWDETM_VALID)
	{
		val = 0;
		iRet = ioctl(g_objDevFd.Tw2865, TW2865CMD_VIDEO_OUT_OFF, &val);
		if(iRet) goto ERROR;
	}
#endif
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "stop video output channel failed: %08x\n", iRet);
	return iRet;
}

int SwitchVo(tag2DVOPar v_objVOutput)
{
	int i;
	int val;
	int iRet;
	
	PRTMSG(MSG_NOR, "Switch Video Output Channel\n");
	
#if VEHICLE_TYPE == VEHICLE_M
	VO_CHN_ATTR_S objChnAttr;
	VO_PUBLIC_ATTR_S objPubAttr;
	
	for(i=0; i<=3; i++)
	{
		iRet = HI_VO_GetChnAttr(i, &objChnAttr);
		if(iRet) goto ERROR;
		
		objChnAttr.enable = HI_FALSE;
		iRet = HI_VO_SetChnAttr(i, &objChnAttr);
		if(iRet) goto ERROR;
	}
	
	iRet = HI_VO_GetPublicAttr(&objPubAttr);
	if(iRet) goto ERROR;
	
	objPubAttr.enable = HI_FALSE;
	iRet = HI_VO_SetPublicAttr(&objPubAttr);
	if(iRet) goto ERROR;
	
	val = 0;
	iRet = ioctl(g_objDevFd.Tw2835, TW2835_DAC_POW_OFF, &val);
	if(iRet) goto ERROR;
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	for(i=0; i<=3; i++)
	{
		iRet = HI_MPI_VO_DisableChn(i);
		if(iRet) goto ERROR;
	}
	
	iRet = HI_MPI_VO_Disable();
	if(iRet) goto ERROR;
	
	val = 0;
	iRet = ioctl(g_objDevFd.Tw2865, TW2865CMD_VIDEO_OUT_OFF, &val);
	if(iRet) goto ERROR;
#endif
	
	iRet = VoDACfg(v_objVOutput.VMode, v_objVOutput.VNormal);
	if(iRet) goto ERROR;
	
	iRet = VoPubCfg(v_objVOutput.VMode, v_objVOutput.VNormal);
	if(iRet) goto ERROR;
	
	iRet = VoChnCfg(v_objVOutput.VMode, v_objVOutput.VNormal);
	if(iRet) goto ERROR;
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "switch video output channel failed: 0x%08x\n", iRet);
	return iRet;
}

int CtrlVideoOutput(tagVideoOutputCfg v_objVideoOutputCfg)
{
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2

	int iRet;
	int i;
	
	VO_PUB_ATTR_S objPubAttr;
	VO_CHN_ATTR_S objChnAttr;
	
	for(i=0; i<=3; i++)
	{
		iRet = HI_MPI_VO_DisableChn(i);
		if(iRet) goto ERROR;
	}
	
	iRet = HI_MPI_VO_Disable();
	if(iRet) goto ERROR;

	objPubAttr.stTvConfig.stComposeMode = v_objVideoOutputCfg.bytVoNormal==PAL ? VIDEO_ENCODING_MODE_PAL : VIDEO_ENCODING_MODE_NTSC;	//显示模式
	objPubAttr.u32BgColor = 0x0000FF; //默认背景为蓝色
	
	iRet = HI_MPI_VO_SetPubAttr(&objPubAttr);
	if(iRet) goto ERROR;
	
	iRet = HI_MPI_VO_Enable();
	if(iRet) goto ERROR;
		
	objChnAttr.u32Priority = 0;
	objChnAttr.bZoomEnable = HI_TRUE;
	
	if(v_objVideoOutputCfg.bytVoNormal == PAL)
	{
		switch(v_objVideoOutputCfg.bytVoMode)
		{
			case CIF:
				{
					for(i = 0; i < 4; i++)
					{
						if(v_objVideoOutputCfg.bytVoChn & (0x01 << i))
						{
							objChnAttr.stRect.s32X = i % 2 ? 360 : 0;
							objChnAttr.stRect.s32Y = i >= 2 ? 288 : 0;
							objChnAttr.stRect.u32Width = 360;
							objChnAttr.stRect.u32Height = 288;
							iRet = HI_MPI_VO_SetChnAttr(i, &objChnAttr);
							if(iRet) goto ERROR;
							iRet = HI_MPI_VO_EnableChn(i);
							if(iRet) goto ERROR;
						}
					}
				}
				break;
			case HD1:
				{
					int iChn = 0;
					
					for(i = 0; i < 4; i++)
					{
						if(v_objVideoOutputCfg.bytVoChn & (0x01 << i))
						{
							iChn++;
							
							objChnAttr.stRect.s32X = iChn % 2 ? 0 : 360;
							objChnAttr.stRect.s32Y = 0;
							objChnAttr.stRect.u32Width = 360;
							objChnAttr.stRect.u32Height = 576;
							iRet = HI_MPI_VO_SetChnAttr(i, &objChnAttr);
							if(iRet) goto ERROR;
							iRet = HI_MPI_VO_EnableChn(i);
							if(iRet) goto ERROR;
						}
					}
				}
				break;
			case D1:
				{
					for(i = 0; i < 4; i++)
					{
						if(v_objVideoOutputCfg.bytVoChn & (0x01 << i))
						{
							objChnAttr.stRect.s32X = 0;
							objChnAttr.stRect.s32Y = 0;
							objChnAttr.stRect.u32Width = 720;
							objChnAttr.stRect.u32Height = 576;
							iRet = HI_MPI_VO_SetChnAttr(i, &objChnAttr);
							if(iRet) goto ERROR;
							iRet = HI_MPI_VO_EnableChn(i);
							if(iRet) goto ERROR;
						}
					}
				}
				break;
			default: 
				break;
		}
	}
	else
	{
		switch(v_objVideoOutputCfg.bytVoMode)
		{
			case CIF:
				{
					for(i = 0; i < 4; i++)
					{
						if(v_objVideoOutputCfg.bytVoChn & (0x01 << i))
						{
							objChnAttr.stRect.s32X = i % 2 ? 360 : 0;
							objChnAttr.stRect.s32Y = i >= 2 ? 240 : 0;
							objChnAttr.stRect.u32Width = 360;
							objChnAttr.stRect.u32Height = 240;
							iRet = HI_MPI_VO_SetChnAttr(i, &objChnAttr);
							if(iRet) goto ERROR;
							iRet = HI_MPI_VO_EnableChn(i);
							if(iRet) goto ERROR;
						}
					}
				}
				break;
			case HD1:
				{
					int iChn = 0;
					
					for(i = 0; i < 4; i++)
					{
						if(v_objVideoOutputCfg.bytVoChn & (0x01 << i))
						{
							iChn++;
							
							objChnAttr.stRect.s32X = iChn % 2 ? 0 : 360;
							objChnAttr.stRect.s32Y = 0;
									
							objChnAttr.stRect.u32Width = 360;
							objChnAttr.stRect.u32Height = 480;
							iRet = HI_MPI_VO_SetChnAttr(i, &objChnAttr);
							if(iRet) goto ERROR;
							iRet = HI_MPI_VO_EnableChn(i);
							if(iRet) goto ERROR;
						}
					}
				}
				break;
			case D1:
				{
					for(i = 0; i < 4; i++)
					{
						if(v_objVideoOutputCfg.bytVoChn & (0x01 << i))
						{
							objChnAttr.stRect.s32X = 0;
							objChnAttr.stRect.s32Y = 0;
							objChnAttr.stRect.u32Width = 720;
							objChnAttr.stRect.u32Height = 480;
							iRet = HI_MPI_VO_SetChnAttr(i, &objChnAttr);
							if(iRet) goto ERROR;
							iRet = HI_MPI_VO_EnableChn(i);
							if(iRet) goto ERROR;
						}
					}
				}
				break;
			default: 
				break;
		}      
	}
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "ctrl video output failed: 0x%08x\n", iRet);
	return iRet;
	
#endif
}

int StartAdec()
{
	int	iRet;
	
	PRTMSG(MSG_NOR, "Start Audio Decode Device\n");
	
#if VEHICLE_TYPE == VEHICLE_M
	iRet = HI_ADEC_Open(); 
	if(iRet) goto ERROR;
#endif
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "start audio decode device failed: %x\n", iRet);
	return iRet;
}

int StopAdec()
{
	int	iRet;
	
	PRTMSG(MSG_NOR, "Stop Audio Decode Device\n");
	
#if VEHICLE_TYPE == VEHICLE_M
	iRet = HI_ADEC_Close();
	if(iRet) goto ERROR;
#endif
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "stop audio decode device failed: %x\n", iRet);
	return iRet;
}

int AoDACfg(AUDIO_SAMPLE_RATE_E v_iAoRate)
{
	int val;
	int iRet;
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	val = 0;     
	iRet = ioctl(g_objDevFd.Tw2865, TW2865CMD_AUDIO_OUT_ON, &val);
	if(iRet) goto ERROR;
#endif
	
	return 0;
	
ERROR:
	return iRet;
}

int AoPubCfg(AUDIO_SAMPLE_RATE_E v_iAoRate)
{
	int iRet;
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	AIO_ATTR_S objPubAttr;
	objPubAttr.enSamplerate = v_iAoRate;//AUDIO_SAMPLE_RATE_32000;
	objPubAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	objPubAttr.enWorkmode = AIO_MODE_I2S_MASTER;    
	objPubAttr.enSoundmode = AUDIO_SOUND_MODE_MOMO;  
	objPubAttr.u32EXFlag = 1;
	objPubAttr.u32FrmNum = 30; 
	objPubAttr.u32PtNumPerFrm = 480;
	
	iRet = HI_MPI_AO_SetPubAttr(0, &objPubAttr);
	if(iRet) goto ERROR;
	
	iRet = HI_MPI_AO_Enable(0);
	if(iRet) goto ERROR;
#endif
	
	return 0;
	
ERROR:
	return iRet;
}

int AoChnCfg(AUDIO_SAMPLE_RATE_E v_iAoRate)
{
	int i;
	int iRet;
	
#if VEHICLE_TYPE == VEHICLE_M
	AUDIO_CH_ATTR_S objChnAttr;
	
	iRet = HI_ADEC_GetAOAttr(0, &objChnAttr); 
	if(iRet) goto ERROR;
	
	objChnAttr.enBitWidth 	= AUDIO_BIT_WIDTH_16;
	objChnAttr.enMode 			= AUDIO_MODE_I2S_SLAVE;
	objChnAttr.enSampleRate = v_iAoRate;//AUDIO_SAMPLE_RATE_8;
	
	iRet = HI_ADEC_SetAOAttr(0, &objChnAttr);
	if(iRet) goto ERROR;
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	for(i=0; i<=3; i++)
	{
		iRet = HI_MPI_AO_EnableChn(0, i);
		if(iRet) goto ERROR;   
	}
#endif
	
	return 0;
	
ERROR:
	return iRet;
}

int StartAo()
{
#if VEHICLE_TYPE == VEHICLE_M
	AUDIO_SAMPLE_RATE_E iAoRate = AUDIO_SAMPLE_RATE_8;
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	AUDIO_SAMPLE_RATE_E iAoRate = AUDIO_SAMPLE_RATE_32000;
#endif

	int	iRet;
	
	PRTMSG(MSG_NOR, "Start Audio Output Channel\n");
	
	iRet = AoDACfg(iAoRate);
	if(iRet) goto ERROR;
	
	iRet = AoPubCfg(iAoRate);
	if(iRet) goto ERROR;
	
	iRet = AoChnCfg(iAoRate);
	if(iRet) goto ERROR;
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "start audio output channel failed: 0x%08x\n", iRet);
	return iRet;
}

int StopAo()
{
	int i;
	int val = 0;
	int	iRet;
	
	PRTMSG(MSG_NOR, "Stop Audio Output Channel\n");
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	for(i=0; i<=3; i++)
	{           
		iRet = HI_MPI_AO_DisableChn(0, i);
		if(iRet) goto ERROR;
	}
	
	iRet = HI_MPI_AO_Disable(0);
	if(iRet) goto ERROR;
	
	BYTE ucIOState;
	IOGet( IOS_POWDETM, &ucIOState );
	if(ucIOState == IO_POWDETM_VALID)
	{
		iRet = ioctl(g_objDevFd.Tw2865, TW2865CMD_AUDIO_OUT_OFF, &val);
		if(iRet) goto ERROR;
	}
#endif
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "stop audio output channel failed: %08x\n", iRet);
	return iRet;
}

int SwitchAo(AUDIO_SAMPLE_RATE_E v_iAoRate)
{
	int i;
	int val = 0;
	int	iRet;
	
	PRTMSG(MSG_NOR, "Switch Audio Output Channel\n");
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	for(i=0; i<=3; i++)
	{           
		iRet = HI_MPI_AO_DisableChn(0, i);
		if(iRet) goto ERROR;
	}
	
	iRet = HI_MPI_AO_Disable(0);
	if(iRet) goto ERROR;
	
	iRet = ioctl(g_objDevFd.Tw2865, TW2865CMD_AUDIO_OUT_OFF, &val);
	if(iRet) goto ERROR;
#endif

	iRet = AoDACfg(v_iAoRate);
	if(iRet) goto ERROR;
	
	iRet = AoPubCfg(v_iAoRate);
	if(iRet) goto ERROR;
	
	iRet = AoChnCfg(v_iAoRate);
	if(iRet) goto ERROR;
	
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "switch audio output channel failed: %08x\n", iRet);
	return iRet;
}

void FlushPbTime( OSD_REGION v_objRegion[], BOOL v_bRegionOn[], char *v_szEncChn, char *v_szCreatDate, int v_iRegionNum, int v_iChnNo, int v_iFlag )
{
#if VEHICLE_TYPE == VEHICLE_M

#if NO_INFO	== 0
	int	iLocation[4][2];
	char szContent[50];
	OSD_BITMAP objBitmap;
	
	if( v_iFlag == 0 )
	{
		sprintf( szContent, "%s %s %s %s\n", 
			v_szEncChn, 
			g_objPlaybackCtrl.VFrame == TRUE ? "帧放   " :
		(g_objPlaybackCtrl.AVPause == TRUE ? "暂停   " :
		(g_objPlaybackFold == DP1 ? "回放   " : 
		(g_objPlaybackFold == DP2 ? "慢放 x2" : 
		(g_objPlaybackFold == DP4 ? "慢放 x4" : 
		(g_objPlaybackFold == DP8 ? "慢放 x8" : 
		(g_objPlaybackFold == MP1 ? "快进 x1" : 
		(g_objPlaybackFold == MP2 ? "快进 x2" : 
		(g_objPlaybackFold == MP4 ? "快进 x4" : 
		(g_objPlaybackFold == MP8 ? "快进 x8" : 
		(g_objPlaybackFold == MM1 ? "快退 x1" : 
		(g_objPlaybackFold == MM2 ? "快退 x2" : 
		(g_objPlaybackFold == MM4 ? "快退 x4" : 
		(g_objPlaybackFold == MM8 ? "快退 x8" :"???? x?"))))))))))))), 
			v_szCreatDate, 
			gAVFrameIndex.CurTime );
		
		if( !v_bRegionOn[v_iChnNo] )
		{
			v_bRegionOn[v_iChnNo] = TRUE;
			
			switch( v_iRegionNum )
			{
			case 1:
				{
					iLocation[0][0] = 22;
					iLocation[0][1] = 15;
				}
				break;
			case 2:
				{
					iLocation[0][0] = 11;
					iLocation[0][1] = 15;
					iLocation[1][0] = 22;
					iLocation[1][1] = 15;
				}
				break;
			case 4:
				{
					iLocation[0][0] = 11;
					iLocation[0][1] = 1;
					iLocation[1][0] = 11;
					iLocation[1][1] = 30;
					iLocation[2][0] = 22;
					iLocation[2][1] = 1;
					iLocation[3][0] = 22;
					iLocation[3][1] = 30;
				}
				break;
			default:
				break;
			}
			
			CreateRegion(&v_objRegion[v_iChnNo], 0, iLocation[v_iChnNo][0], iLocation[v_iChnNo][1], 1, CountCharRow(szContent));
		}
		
		CreateBitmap(&objBitmap, 1, CountCharRow(szContent));							
		DrawCharacter(&objBitmap, szContent, WHITE, BLACK, 0, 0, FALSE);	
		ShowBitmap(&v_objRegion[v_iChnNo], &objBitmap, 0, 0);
	}
	else if( v_iFlag == -1 )
	{
		if( v_bRegionOn[v_iChnNo] )
		{
			DeleteRegion(v_objRegion[v_iChnNo]);
			v_bRegionOn[v_iChnNo] = FALSE;
		}
	}
#endif
	
#endif
}

void FlushOneFrame(int v_iDecChn, FILE_BUFFER_S *v_pVideoBuf)
{
	int iRet;
	BYTE *pVideoAddr;
	uint uiVideoLen;
	
#if VEHICLE_TYPE == VEHICLE_M
	VDEC_STREAM_S objVdecStream;
	
	while(File_Ftell(v_pVideoBuf, gAVFrameIndex.VEndOffset) == 0)
	{
		iRet = File_GetVSlice(v_pVideoBuf, &pVideoAddr, &uiVideoLen);
		if(!iRet)
		{
			objVdecStream.Dataaddr = (ADDR)pVideoAddr;
			objVdecStream.Datalen = uiVideoLen;
			objVdecStream.pts = 0;
			
			while( (iRet=HI_VDEC_SendStream(v_iDecChn, &objVdecStream, HI_TRUE)) )
			{
				if((iRet==HI_ERR_VDEC_NO_MSGBUF)||(iRet==HI_ERR_VDEC_NO_INPUTBUF))/*当消息缓存或输入缓存不足时等待*/
					msleep(1);
				else
					break;
			}
		}
	}
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	VDEC_STREAM_S objVdecStream;
	
	while( (iRet=File_Ftell(v_pVideoBuf, gAVFrameIndex.VEndOffset))==0 )
	{
		iRet = File_GetVSlice(v_pVideoBuf, &pVideoAddr, &uiVideoLen);
		if(!iRet)
		{
			objVdecStream.pu8Addr = pVideoAddr;
			objVdecStream.u32Len = uiVideoLen;
			objVdecStream.u64PTS = 0;
			
			while( (iRet=HI_MPI_VDEC_SendStream(v_iDecChn, &objVdecStream, HI_IO_BLOCK)) )
			{
				if(iRet==HI_ERR_VDEC_BUF_FULL)
					msleep(1);
				else
					break;
			}
		}
	}
#endif
}

void *APlay(void *args)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	int iRet;
	int i = (int)args;
	uint uiAudioLen;
	BYTE *pAudioAddr;
	
#if VEHICLE_TYPE == VEHICLE_M
	AENC_STREAM_S	objAencStream;
	
	while(1) 
	{
		if( g_objCtrlStop.APlayback )
		{
			PRTMSG(MSG_ERR, "audio playback thread exit\n");
			break;
		}
		
		if(g_objPlaybackFold==DP1 && !g_objPlaybackCtrl.AVPause)
		{
			iRet = File_GetASlice(&gAudioBuf[i], &pAudioAddr, &uiAudioLen);
			if(!iRet)
			{
				if(!g_objPlaybackCtrl.APause)
				{
					objAencStream.ps16Addr = (short *)pAudioAddr;
					objAencStream.u32Len = uiAudioLen;
					
					HI_ADEC_SendStream(i, &objAencStream);
				}
			}
			else
			{
				msleep(1);
			}
		}
	}
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	AUDIO_STREAM_S objAencStream;
	AUDIO_FRAME_INFO_S objAencFrame;
	
	sleep(1);

	while(1) 
	{
		if( g_objCtrlStop.APlayback )
		{
			PRTMSG(MSG_ERR, "audio playback thread exit\n");
			break;
		}
		
		if(g_objPlaybackFold==DP1 && !g_objPlaybackCtrl.AVPause)
		{
			iRet = File_GetASlice(&gAudioBuf[i], &pAudioAddr, &uiAudioLen);
			if(!iRet)
			{
				if(!g_objPlaybackCtrl.APause)
				{
					objAencStream.pStream = pAudioAddr;
					objAencStream.u32Len = uiAudioLen;
					objAencStream.u64TimeStamp = 0;
					objAencStream.u32Seq = 0;
					
					iRet = HI_MPI_ADEC_SendStream(i, &objAencStream);//阻塞向解码通道发送码流
					if(!iRet)
					{
						HI_MPI_ADEC_GetData(i, &objAencFrame);//阻塞获取解码后音频帧数据
						HI_MPI_AO_SendFrame(0, i, objAencFrame.pstFrame, HI_IO_BLOCK);
						HI_MPI_ADEC_ReleaseData(i, &objAencFrame); //释放音频帧缓存
					}
				}
			}
			else
			{
				msleep(1);
			}
		}
	}
#endif
	
	pthread_exit(NULL);
	return NULL;
}

void *VPlay(void *args)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	int iRet;
	int i = (int)args;
	
#if VEHICLE_TYPE == VEHICLE_M
	VDEC_FRAMEINFO_S objVencStream;
	
	while(1)
	{
		if( g_objCtrlStop.VPlayback )
		{
			PRTMSG(MSG_ERR, "video playback thread exit\n");
			break;
		}
		
		iRet = HI_VDEC_Receive(i, &objVencStream, NULL, HI_FALSE);//非阻塞获取解码后视频帧数据
		if(!iRet)
		{
			if(objVencStream.value)//判断有无视频帧数据
			{
				while( (iRet=HI_VO_SendData(i, &objVencStream)) )
				{
					if( iRet==HI_ERR_VO_BUF_FULL )//当输出缓存满时等待
					{
						msleep(1);
					}
					else
					{
						HI_VDEC_ReleaseVideoBuf(i, &objVencStream);//释放视频帧缓存
						break;
					}
				}
			}
		}
		else
		{
			msleep(1);
		}
	}
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	VDEC_DATA_S objVencStream;
	
	while(1)
	{
		if( g_objCtrlStop.VPlayback )
		{
			PRTMSG(MSG_ERR, "video playback thread exit\n");
			break;
		}
		
		iRet = HI_MPI_VDEC_GetData(i, &objVencStream, HI_IO_NOBLOCK);//非阻塞获取解码后视频帧数据
		if(!iRet)
		{
			if(objVencStream.stFrameInfo.bValid)//判断有无视频帧数据
			{
				while( (iRet=HI_MPI_VO_SendFrame(i, &objVencStream.stFrameInfo.stVideoFrameInfo)) )
				{
					if( iRet==HI_ERR_VO_BUSY )//当系统忙时等待
						msleep(1);
					else
						break;
				}
			}
			
			HI_MPI_VDEC_ReleaseData(i, &objVencStream);//释放视频帧缓存
		}
		else
		{
			msleep(1);
		}
	}
#endif
	
	pthread_exit(NULL);
	return NULL;
}

void *AVPlay(void *arg)
{
	int i, j;
	int iRet;
	int iFold = 0;
	
	OSD_REGION objChnRegion[4];
	BOOL bRegionOn[4] = {FALSE};
	
	FILE_BUFFER_S  objVideoBuf[4]; 
	INDEX_BUFFER_S objIndexBuf[4];
	
	pthread_t pthAPlayback[4];
	pthread_t pthVPlayback[4];
	
	tag2DAVPbPar objAVPlayback = *((tag2DAVPbPar*)arg);
	
#if VEHICLE_TYPE == VEHICLE_M
	ADEC_CHN lAdecChn[4] = {-1};
	VDEC_CHN lVdecChn[4] = {-1};
	VDEC_ATTR_TYPE_E type = VDEC_ATTR_ALL;
	VDEC_ChannelPara_S config = {VDEC_H264, VDEC_DEFAULT_PICTUREFORMAT, 0};
	
	for(i = 0; i < 4; i ++)
	{
		if(objAVPlayback.AStart[i])
		{
			iRet = HI_ADEC_CreateCH(i, AUDIO_CODEC_FORMAT_G726, HI_TRUE , &lAdecChn[i]);
			if(iRet)
			{
				PRTMSG(MSG_ERR, "create audio playback chn%d failed: %08x\n", i+1, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = HI_ADEC_StartCH(lAdecChn[i], 0); 
			if(iRet) 
			{
				PRTMSG(MSG_ERR, "start audio playback chn%d failed: %08x\n", i+1, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			g_objCtrlStart.APlayback[i] = TRUE;
			
			if(pthread_create(&pthAPlayback[i], NULL, APlay, (void *)lAdecChn[i]))
			{
				PRTMSG(MSG_ERR, "start audio recv thread %d failed: %08x\n", i, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = IFile_Init(&objIndexBuf[i], objAVPlayback.IFilePath[i]);
			if(iRet) 
			{
				PRTMSG(MSG_ERR, "open index file failed: 0x%08x\n", iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = File_Init(&gAudioBuf[i], objAVPlayback.AFilePath[i]);
			if(iRet) 
			{
				PRTMSG(MSG_ERR, "open audio file failed failed: 0x%08x\n", iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
		}
		
		if( objAVPlayback.VStart[i] )
		{
			iRet = HI_VDEC_CreateCH(&lVdecChn[i], type, &config);
			if(iRet)
			{
				PRTMSG(MSG_ERR, "create video playback chn %d failed: %08x\n", i+1, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = HI_VO_SetFrameRate(i, objAVPlayback.VFramerate[i]);
			if(iRet)
			{
				PRTMSG(MSG_ERR, "set video playback chn %d framerate failed: %08x\n", i+1, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			g_objCtrlStart.VPlayback[i] = TRUE;
			
			if(pthread_create(&pthVPlayback[i], NULL, VPlay, (void *)lVdecChn[i]))
			{
				PRTMSG(MSG_ERR, "start video recv thread %d failed\n", i);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = IFile_Init(&objIndexBuf[i], objAVPlayback.IFilePath[i]);
			if(iRet) 
			{
				PRTMSG(MSG_ERR, "open index file failed: 0x%08x\n", iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = File_Init(&objVideoBuf[i], objAVPlayback.VFilePath[i]);
			if(iRet) 
			{
				PRTMSG(MSG_ERR, "open video file failed failed: 0x%08x\n", iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
		}
	}
#endif
	
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	ADEC_ATTR_G726_S objG726Attr;
	ADEC_CHN_ATTR_S objAChnAttr;
	VDEC_ATTR_H264_S objH264Attr;
	VDEC_CHN_ATTR_S objVChnAttr;
	
	for(i = 0; i < 4; i ++)
	{
		if( objAVPlayback.AStart[i] )
		{
			objG726Attr.enG726bps = MEDIA_G726_16K;
			objAChnAttr.enType = PT_G726;
			objAChnAttr.u32BufSize = 8;
			objAChnAttr.enMode = ADEC_MODE_PACK;
			objAChnAttr.pValue = &objG726Attr;
			
			iRet = HI_MPI_ADEC_CreateChn(i, &objAChnAttr);
			if(iRet)
			{
				PRTMSG(MSG_ERR, "create audio playback chn%d failed: 0x%08x\n", i+1, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			g_objCtrlStart.APlayback[i] = TRUE;
			
			if(pthread_create(&pthAPlayback[i], NULL, APlay, (void *)i))
			{
				PRTMSG(MSG_ERR, "start audio recv thread %d failed: 0x%08x\n", i, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = IFile_Init(&objIndexBuf[i], objAVPlayback.IFilePath[i]);
			if(iRet) 
			{
				PRTMSG(MSG_ERR, "open index file failed: 0x%08x\n", iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = File_Init(&gAudioBuf[i], objAVPlayback.AFilePath[i]);
			if(iRet) 
			{
				PRTMSG(MSG_ERR, "open audio file failed failed: 0x%08x\n", iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
		}
		
		if( objAVPlayback.VStart[i] )
		{
			objH264Attr.u32Priority = 0;
			objH264Attr.u32PicHeight = 576;
			objH264Attr.u32PicWidth = 720;
			objH264Attr.u32RefFrameNum = 2;
			objH264Attr.enMode = H264D_MODE_STREAM;
			
			objVChnAttr.enType = PT_H264;
			objVChnAttr.u32BufSize = (objH264Attr.u32PicWidth*objH264Attr.u32PicHeight*2);
			objVChnAttr.pValue = (void*)&objH264Attr;
            
			iRet = HI_MPI_VDEC_CreateChn(i, &objVChnAttr, NULL);
			if(iRet)
			{
				PRTMSG(MSG_ERR, "create video playback chn%d failed: %08x\n", i+1, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = HI_MPI_VDEC_StartRecvStream(i);
			if(iRet)
			{
				PRTMSG(MSG_ERR, "start video playback chn%d framerate failed: %08x\n", i+1, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = HI_MPI_VO_SetFrameRate(i, objAVPlayback.VFramerate[i]);
			if(iRet)
			{
				PRTMSG(MSG_ERR, "set video playback chn%d framerate failed: %08x\n", i+1, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = HI_MPI_VDEC_BindOutput(i, i);
			if(iRet)
			{
				PRTMSG(MSG_ERR, "bind video output chn%d failed: %08x\n", i+1, iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			g_objCtrlStart.VPlayback[i] = TRUE;
			
			if(pthread_create(&pthVPlayback[i], NULL, VPlay, (void *)i))
			{
				PRTMSG(MSG_ERR, "start video recv thread %d failed\n", i);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = IFile_Init(&objIndexBuf[i], objAVPlayback.IFilePath[i]);
			if(iRet) 
			{
				PRTMSG(MSG_ERR, "open index file failed: 0x%08x\n", iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
			
			iRet = File_Init(&objVideoBuf[i], objAVPlayback.VFilePath[i]);
			if(iRet) 
			{
				PRTMSG(MSG_ERR, "open video file failed failed: 0x%08x\n", iRet);
				goto AVDEC_PLAYBACK_STOP;
			}
		}
	}
#endif
	
	if(	!g_objCtrlStart.APlayback[0] && !g_objCtrlStart.APlayback[1] && !g_objCtrlStart.APlayback[2] && !g_objCtrlStart.APlayback[3] 
		&&!g_objCtrlStart.VPlayback[0] && !g_objCtrlStart.VPlayback[1] && !g_objCtrlStart.VPlayback[2] && !g_objCtrlStart.VPlayback[3])
	{
		g_objCtrlStop.AVPlayback = TRUE;
		return NULL;
	}
	
	while(1)
	{
		for(i = 0; i < 4; i++)
		{
			/*音像回放*/
			if( g_objCtrlStart.VPlayback[i] && (!g_objPlaybackCtrl.AVPause||g_objPlaybackCtrl.VFrame) )
			{
				switch(g_objPlaybackFold)
				{
					case DP1:
					case DP2:
					case DP4:
					case DP8:
						{
							/*取出一帧索引*/
							iRet = IFile_GetFrameIndex(&objIndexBuf[i], &gAVFrameIndex);
							if(iRet)
							{
								//								IFile_Rewind(&objIndexBuf[i]);
								//								File_Rewind(&objVideoBuf[i]);
								//								if(g_objCtrlStart.APlayback[i])
								//									File_Rewind(&gAudioBuf[i]);
								goto OUT;
							}
							
							if(gAVFrameIndex.IFrameFlag)
							{
								if(g_objPlaybackCtrl.VSlow)
								{
									switch(g_objPlaybackFold)
									{
	#if VEHICLE_TYPE == VEHICLE_M
									case DP1:
										iRet = HI_VO_SetFrameRate(i, objAVPlayback.VFramerate[i]);
										break;
									case DP2:
										iRet = HI_VO_SetFrameRate(i, objAVPlayback.VFramerate[i]/2==0 ? 1 : (objAVPlayback.VFramerate[i]/2));
										break;
									case DP4:
										iRet = HI_VO_SetFrameRate(i, objAVPlayback.VFramerate[i]/4==0 ? 1 : (objAVPlayback.VFramerate[i]/4));
										break;
									case DP8:
										iRet = HI_VO_SetFrameRate(i, objAVPlayback.VFramerate[i]/8==0 ? 1 : (objAVPlayback.VFramerate[i]/8));
										break;
									default:
										break;
	#endif
	#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
									case DP1:
										iRet = HI_MPI_VO_SetFrameRate(i, objAVPlayback.VFramerate[i]);
										break;
									case DP2:
										iRet = HI_MPI_VO_SetFrameRate(i, objAVPlayback.VFramerate[i]/2==0 ? 1 : (objAVPlayback.VFramerate[i]/2));
										break;
									case DP4:
										iRet = HI_MPI_VO_SetFrameRate(i, objAVPlayback.VFramerate[i]/4==0 ? 1 : (objAVPlayback.VFramerate[i]/4));
										break;
									case DP8:
										iRet = HI_MPI_VO_SetFrameRate(i, objAVPlayback.VFramerate[i]/8==0 ? 1 : (objAVPlayback.VFramerate[i]/8));
										break;
									default:
										break;
	#endif
									}
									
									if(iRet)
									{
										PRTMSG(MSG_ERR, "set video playback chn %d framerate failed: %08x\n", i+1, iRet);
									}
									
									g_objPlaybackCtrl.VSlow = FALSE;
								}
								
								/*显示该帧时间*/
								FlushPbTime( objChnRegion, bRegionOn, objAVPlayback.EncChn[i], objAVPlayback.CreatDate, objAVPlayback.DecNum, i, 0 );
							}
							
							/*播放该帧图像*/
							FlushOneFrame( i, &objVideoBuf[i] );
							g_objPlaybackCtrl.VFrame = FALSE;
						}
						break;
						
					case MP1:
					case MP2:
					case MP4:
					case MP8:
						{
							/*取出一个I帧索引*/
							do
							{
								iRet = IFile_GetFrameIndex(&objIndexBuf[i], &gAVFrameIndex);
								if(iRet)
								{
									//									IFile_Rewind(&objIndexBuf[i]);
									goto OUT;
								}
							}
							while( !gAVFrameIndex.IFrameFlag );
							
							/*视频文件指针移动到相应起始位置*/
							File_Fseek(&objVideoBuf[i], gAVFrameIndex.VStartOffset);
							
							/*显示该帧时间*/
							FlushPbTime( objChnRegion, bRegionOn, objAVPlayback.EncChn[i], objAVPlayback.CreatDate, objAVPlayback.DecNum, i, 0 );
							/*播放该帧图像*/
							FlushOneFrame( i, &objVideoBuf[i] );
							
							/*快进时跳过相应倍速的I帧*/
							switch(g_objPlaybackFold)
							{
								case MP1:
									iFold = 0;
									break;
								case MP2:
									iFold = 1;
									break;
								case MP4:
									iFold = 3;
									break;
								case MP8:
									iFold = 7;
									break;
								default:
									break;
							}
							
							for(j=0; j<iFold; j++)
							{
								do
								{
									iRet = IFile_GetFrameIndex(&objIndexBuf[i], &gAVFrameIndex);
									if(iRet)
									{
										//										IFile_Rewind(&objIndexBuf[i]);
										goto OUT;
									}
								}
								while( !gAVFrameIndex.IFrameFlag );
							}
							
							File_Fseek(&objVideoBuf[i], gAVFrameIndex.VStartOffset);
							if( g_objCtrlStart.APlayback[i] )
								File_Fseek(&gAudioBuf[i], gAVFrameIndex.AStartOffset);
							
							g_objPlaybackCtrl.VFrame = FALSE;
						}
						break;
						
					case MM1:
					case MM2:
					case MM4:
					case MM8:
						{
							/*取出一个I帧索引*/
							do
							{
								if(ftell(objIndexBuf[i].pFile) == 0)
								{
									//									fseek(objIndexBuf->pFile, -sizeof(FRAME_INDEX), SEEK_END);
									goto OUT;
								}
								
								iRet = IFile_GetFrameIndex(&objIndexBuf[i], &gAVFrameIndex);
								if(iRet)
								{
									fseek(objIndexBuf->pFile, -sizeof(FRAME_INDEX), SEEK_END);
									goto OUT;
								}
								
								fseek(objIndexBuf->pFile, -2*sizeof(FRAME_INDEX), SEEK_CUR);
							}
							while( !gAVFrameIndex.IFrameFlag );
							
							/*视频文件指针移动到I帧起始位置*/
							File_Fseek(&objVideoBuf[i], gAVFrameIndex.VStartOffset);
							
							/*显示该帧时间*/
							FlushPbTime( objChnRegion, bRegionOn, objAVPlayback.EncChn[i], objAVPlayback.CreatDate, objAVPlayback.DecNum, i, 0 );
							/*播放该帧图像*/
							FlushOneFrame( i, &objVideoBuf[i] );
							
							/*快退时跳过相应倍速的I帧*/
							switch(g_objPlaybackFold)
							{
								case MM1:
									iFold = 0;
									break;
								case MM2:
									iFold = 1;
									break;
								case MM4:
									iFold = 3;
									break;
								case MM8:
									iFold = 7;
									break;
								default:
									break;
							}
							
							for(j = 0; j < iFold; j++)
							{
								do
								{
									if(ftell(objIndexBuf[i].pFile) == 0)
									{
										//										fseek(objIndexBuf->pFile, -sizeof(FRAME_INDEX), SEEK_END);
										goto OUT;
									}
									
									iRet = IFile_GetFrameIndex(&objIndexBuf[i], &gAVFrameIndex);
									if(iRet)
									{
										fseek(objIndexBuf->pFile, -sizeof(FRAME_INDEX), SEEK_END);
										goto OUT;
									}
									
									fseek(objIndexBuf->pFile, -2*sizeof(FRAME_INDEX), SEEK_CUR);
								}
								while( !gAVFrameIndex.IFrameFlag );
							}
							
							File_Fseek(&objVideoBuf[i], gAVFrameIndex.VStartOffset);
							if( g_objCtrlStart.APlayback[i] )
								File_Fseek(&gAudioBuf[i], gAVFrameIndex.AStartOffset);
							
							g_objPlaybackCtrl.VFrame = FALSE;
						}
						break;
						
					default:
						break;
				}
			}
			
OUT:
			continue;
		}
		
		if(g_objCtrlStop.AVPlayback)
		{
AVDEC_PLAYBACK_STOP:
		
			for(i = 0; i < 4; i++)
			{
				FlushPbTime( objChnRegion, bRegionOn, NULL, NULL, 0, i, -1 );
				
				if(g_objCtrlStart.VPlayback[i])
				{
					g_objCtrlStop.VPlayback = TRUE;
					if( WaitForEvent(ThreadExit, &pthVPlayback[i], 3000) )
					{
						PRTMSG(MSG_ERR, "Cancel Video Playback\n");
						pthread_cancel(pthVPlayback[i]);
						msleep(100);
					}
					g_objCtrlStop.VPlayback = FALSE;
					
					File_DeInit(&objVideoBuf[i]); 
					IFile_DeInit(&objIndexBuf[i]);
					
					g_objCtrlStart.VPlayback[i] = FALSE;
				}
				
				if(g_objCtrlStart.APlayback[i])
				{
					g_objCtrlStop.APlayback = TRUE;
					if( WaitForEvent(ThreadExit, &pthAPlayback[i], 3000) )
					{
						PRTMSG(MSG_ERR, "Cancel Audio Playback\n");
						pthread_cancel(pthAPlayback[i]);
						msleep(100);
					}
					g_objCtrlStop.APlayback = FALSE;
					
					File_DeInit(&gAudioBuf[i]); 
					IFile_DeInit(&objIndexBuf[i]);
					
					g_objCtrlStart.APlayback[i] = FALSE;
				}
				
#if VEHICLE_TYPE == VEHICLE_M
				HI_ADEC_StopCH(lAdecChn[i]); 
				HI_ADEC_DestroyCH(lAdecChn[i], HI_TRUE);
				HI_VDEC_DestroyCH(lVdecChn[i]);
#endif
				
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
				HI_MPI_ADEC_DestroyChn(i);
				HI_MPI_VDEC_StopRecvStream(i);
				HI_MPI_VDEC_UnbindOutput(i);
				HI_MPI_VDEC_DestroyChn(i);
#endif
			}
			
			if(	!g_objCtrlStart.APlayback[0] && !g_objCtrlStart.APlayback[1] && !g_objCtrlStart.APlayback[2] && !g_objCtrlStart.APlayback[3] 
				&&!g_objCtrlStart.VPlayback[0] && !g_objCtrlStart.VPlayback[1] && !g_objCtrlStart.VPlayback[2] && !g_objCtrlStart.VPlayback[3] )
			{
				g_objPlaybackCtrl.AVPause = FALSE;
				g_objPlaybackFold = DP1;
				g_objCtrlStop.AVPlayback = FALSE;
				
				pthread_exit(NULL);
				return NULL;
			}
		}
		
		msleep(10);
	}
}

int StartAVPlay()
{
	int iRet;
	
	if(
		(	!g_objCtrlStart.APlayback[0] && !g_objCtrlStart.APlayback[1] && !g_objCtrlStart.APlayback[2] && !g_objCtrlStart.APlayback[3] 
		&&!g_objCtrlStart.VPlayback[0] && !g_objCtrlStart.VPlayback[1] && !g_objCtrlStart.VPlayback[2] && !g_objCtrlStart.VPlayback[3] )
		&&
		(	g_objMvrCfg.AVPlayback.VStart[0] || g_objMvrCfg.AVPlayback.VStart[1] || g_objMvrCfg.AVPlayback.VStart[2] || g_objMvrCfg.AVPlayback.VStart[3] )
		)
	{
		PRTMSG(MSG_NOR, "Start AV Playback\n");
		
		iRet = pthread_create(&gAVdecPlaybackThread, NULL, AVPlay, (void*)(&g_objMvrCfg.AVPlayback));
		if(iRet) 
		{
			PRTMSG(MSG_ERR, "start av playback failed: 0x%08x\n", iRet);
			return -1;
		}
		
		return 0;
	}
	
	return -1;
}

int StopAVPlay()
{
	if( g_objCtrlStart.APlayback[0] || g_objCtrlStart.APlayback[1] || g_objCtrlStart.APlayback[2] || g_objCtrlStart.APlayback[3] 
		||g_objCtrlStart.VPlayback[0] || g_objCtrlStart.VPlayback[1] || g_objCtrlStart.VPlayback[2] || g_objCtrlStart.VPlayback[3])
	{
		PRTMSG(MSG_NOR, "Stop AV Playback\n");
		
		g_objCtrlStop.AVPlayback = TRUE;
		pthread_join(gAVdecPlaybackThread, NULL);
		
		PRTMSG(MSG_NOR, "stop av playback succ\n");
		return 0;
	}
	
	return -1;
}

int SwitchAVPlay(int v_iAVPlay)
{
	g_objMvrCfg.AVPlayback.DecNum = 1;
	if( v_iAVPlay==0||v_iAVPlay==1 )
		g_objMvrCfg.AVPlayback.AStart[0] = TRUE;
	else
		g_objMvrCfg.AVPlayback.AStart[0] = FALSE;
	g_objMvrCfg.AVPlayback.VStart[0] = TRUE;
	g_objMvrCfg.AVPlayback.VFramerate[0] = g_objMvrCfg.AVRecord.VFramerate[v_iAVPlay];//25;
	strcpy(g_objMvrCfg.AVPlayback.AFilePath[0], szATest[v_iAVPlay]);
	strcpy(g_objMvrCfg.AVPlayback.VFilePath[0], szVTest[v_iAVPlay]);
	strcpy(g_objMvrCfg.AVPlayback.IFilePath[0], szITest[v_iAVPlay]);
}
