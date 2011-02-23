#include "yx_common.h"

/*创建区域,参数为区域层次、区域在屏幕中的起始行、列,以及区域的行、列数*/
int CreateRegion(OSD_REGION *Region, uint TopLevel, WORD LineStart, WORD RowStart, WORD Line, WORD Row, int Fb)
{
	int iRet;

#if VEHICLE_TYPE == VEHICLE_M
	OSD_REGION_S objRegionAttr = 
	{
		{
			RowStart*ROWWIDTH, //显示区域起点,左上角的X坐标
			LineStart*LINEHIGH, //显示区域起点,左上角的Y坐标
			Row*ROWWIDTH, //显示区域宽
			Line*LINEHIGH//显示区域高
		}, 
		100, //透明度,0~100,0:完全透明,100:完全不透明
		0x00000000, //背景色,采用RGB格式,取值0x00000000~0x00ffffff(0~24位有效)
		TopLevel//区域显示层次,取值0~255,值小的在下层
	};
	
	iRet = HI_OSD_CreateRegion(OSD_END, 0, &objRegionAttr, &(Region->Handle));
	if(iRet) goto ERROR;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	HIFB_POINT_S objPoint = {RowStart*ROWWIDTH, LineStart*LINEHIGH};
  HIFB_ALPHA_S objAlpha = {HI_TRUE, HI_FALSE, 0xff, 0xff, 0xff, 0};
	fb_var_screeninfo objVInfo = {Row*ROWWIDTH, Line*LINEHIGH, Row*ROWWIDTH, Line*LINEHIGH, 0, 0, 32, 0, {16, 8, 0}, {8, 8, 0}, {0, 8, 0}, {24, 8, 0}, 0, FB_ACTIVATE_NOW, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
  char szDev[32] = {0};
  sprintf(szDev, "/dev/fb/%d", Fb);
  
	Region->Handle = iRet = open(szDev, O_RDWR);
	if(iRet==-1) goto ERROR;
	
  iRet = ioctl(Region->Handle, FBIOPUT_SCREEN_ORIGIN_HIFB, &objPoint);
  if(iRet) goto ERROR;

	iRet = ioctl(Region->Handle, FBIOPUT_ALPHA_HIFB, &objAlpha);
	if(iRet) goto ERROR;
  
  iRet = ioctl(Region->Handle, FBIOPUT_VSCREENINFO, &objVInfo);
  if(iRet) goto ERROR;

  Region->MappedMem = (unsigned char*)mmap(NULL, 1658880, PROT_READ|PROT_WRITE, MAP_SHARED, Region->Handle, 0);
  if(Region->MappedMem==MAP_FAILED) goto ERROR;

  memset(Region->MappedMem, 0, 1658880);
#endif

  return 0;

ERROR:
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	close(Region->Handle);
#endif
	PRTMSG(MSG_ERR, "create osd region failed:0x%08x\n", iRet);
  return iRet;
}

int DeleteRegion(OSD_REGION Region)
{
	int iRet;

#if VEHICLE_TYPE == VEHICLE_M
	iRet = HI_OSD_DeleteRegion(Region.Handle);
	if(iRet) goto ERROR;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	if(Region.Handle)
	{
		iRet = munmap(Region.MappedMem, 1658880);
		if(iRet) goto ERROR;
	
	  iRet = close(Region.Handle);
	  if(iRet) goto ERROR;
	}
#endif
  
  return 0;
  
ERROR:
	PRTMSG(MSG_ERR, "delete osd region failed:%08x\n", iRet);
  return iRet;
}

int ShowRegion(OSD_REGION Region)
{
	int iRet;
	
#if VEHICLE_TYPE == VEHICLE_M
	iRet = HI_OSD_ShowRegion(Region.Handle);
	if(iRet) goto ERROR;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	HI_BOOL bShow = HI_TRUE;
	iRet = ioctl(Region.Handle, FBIOPUT_SHOW_HIFB, &bShow);
	if(iRet < 0) goto ERROR;
#endif

  return 0;
  
ERROR:
	PRTMSG(MSG_ERR, "show osd region failed:0x%08x\n", iRet);
  return iRet;
}

int HideRegion(OSD_REGION Region)
{
	int iRet;
	
#if VEHICLE_TYPE == VEHICLE_M
	iRet = HI_OSD_HideRegion(Region.Handle);
	if(iRet) goto ERROR;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	HI_BOOL bShow = HI_FALSE;
	iRet = ioctl(Region.Handle, FBIOPUT_SHOW_HIFB, &bShow);
	if(iRet < 0) goto ERROR;
#endif

  return 0;
  
ERROR:
	PRTMSG(MSG_ERR, "hide osd region failed:0x%08x\n", iRet);
  return iRet;
}

/*创建图片, 参数为图片的行、列数*/
int CreateBitmap(OSD_BITMAP *Bitmap, WORD Line, WORD Row)
{
#if VEHICLE_TYPE == VEHICLE_M
	Bitmap->enPixelFormat = PIXEL_FORMAT_RGB_888;
#endif
	Bitmap->u32Width = Row*ROWWIDTH;
	Bitmap->u32Height = Line*LINEHIGH;
	Bitmap->pData = g_pMenuPage;
#if VEHICLE_TYPE == VEHICLE_M
	memset(Bitmap->pData, 0, SCRWIDTH*SCRHIGH*3);
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	memset(Bitmap->pData, 0, SCRWIDTH*SCRHIGH*4);
#endif
/*	Bitmap->pData = (void *)malloc(Bitmap->u32Width * Bitmap->u32Height * 3);  

	if(NULL == Bitmap->pData)
	{
	  PRTMSG(MSG_ERR, "create bitmap failed\n");
		return -1;
	}
	
	memset(Bitmap->pData, 0, Bitmap->u32Width * Bitmap->u32Height * 3);*/
  return 0;
}

/*将图片填充到区域中, 参数为图片在区域中的起始行、列*/
int ShowBitmap(OSD_REGION *Region, OSD_BITMAP *Bitmap, WORD LineStart, WORD RowStart)
{
	int	iRet;

#if VEHICLE_TYPE == VEHICLE_M
	iRet = HI_OSD_PutBitmapToRegion(Region->Handle, Bitmap, RowStart*ROWWIDTH, LineStart*LINEHIGH);
	if(iRet) goto ERROR;
#endif

#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	int i,j,k;
	fb_var_screeninfo objVInfo;
	
	iRet = ioctl(Region->Handle, FBIOGET_VSCREENINFO, &objVInfo);
  if(iRet) goto ERROR;

	for(i=0; i<Bitmap->u32Height; i++)
	{
		for(j=0; j<Bitmap->u32Width; j++)
		{
			for(k=0; k<4; k++)
			{
				Region->MappedMem[((LineStart*LINEHIGH+i)*objVInfo.xres_virtual + (RowStart*ROWWIDTH+j))*4 + k] = Bitmap->pData[(i*Bitmap->u32Width+j)*4 + k];
			}
		}
	}
#endif

/*  if(Bitmap->pData)
  {
		free(Bitmap->pData);
		Bitmap->pData = NULL;
	}*/
	return 0;
	
ERROR:
	PRTMSG(MSG_ERR, "show bitmap failed:0x%08x\n", iRet);
  return iRet;
}

/*在图片上写字,参数依次字符内容、字体颜色、背景颜色、字符在图片中的起始行、列*/
int DrawCharacter(OSD_BITMAP *Bitmap, char *ChEn, uint CharColor, uint BackColor, WORD LineStart, WORD RowStart, BOOL Flag)
{
	int x, y;/*字符在字符串中的位置*/
	int i, j, k;/*像素在点阵中的位置*/
	int ChConfine, EnConfine;
	
	int sec, pot;/*字符在字库中的位置*/
	BYTE  Hzkmat[HZKHIGH][(HZKWIDTH%8)?(HZKWIDTH/8+1):HZKWIDTH/8];
	BYTE  Ascmat[ASCHIGH][(ASCWIDTH%8)?(ASCWIDTH/8+1):ASCWIDTH/8];
	char *pdata = (char *)Bitmap->pData;
	for(y = 0; y < (int)(Bitmap->u32Height/LINEHIGH); y++)
	{
		if(*ChEn == '\0')	
			break;
			
		x = 0;	
		while(*ChEn != '\n')
    {
    	if(Flag == TRUE && *ChEn == ' ')//忽略空格
    	{
    		ChEn ++; 
				x ++;
    	}
			else if(*ChEn & 0x80)/*绘制汉字*/      
			{
				sec = *ChEn - 0xa0;     
				pot = *(ChEn + 1) - 0xa0; 
				fseek(g_objDevFd.HZK,(long)(94 * (sec - 1) + (pot - 1)) * (HZKHIGH * ((HZKWIDTH%8)?(HZKWIDTH/8+1):HZKWIDTH/8)),SEEK_SET);
				fread(Hzkmat,HZKHIGH * ((HZKWIDTH%8)?(HZKWIDTH/8+1):HZKWIDTH/8),1,g_objDevFd.HZK);
				for(i = 0; i < HZKHIGH; i++)
				{
					for(j = 0; j < ((HZKWIDTH%8)?(HZKWIDTH/8+1):HZKWIDTH/8); j++)
					{
						if(j == HZKWIDTH/8)
							ChConfine = HZKWIDTH%8;
						else
							ChConfine = 8;
							
						for(k = 0; k < ChConfine; k++)
						{
							if(Hzkmat[i][j] & (0x80 >> k)) 
							{
#if VEHICLE_TYPE == VEHICLE_M
								/*设置字体颜色, 高8位为R值, 中8位为G值,低8位为B值*/
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3] 			= ((CharColor & 0xff0000) >> 16);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3 + 1] 	= ((CharColor & 0x00ff00) >> 8);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3 + 2] 	=  (CharColor & 0x0000ff);
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
								/*设置字体颜色, 高8位为A值, 次高8位为R值, 中8位为G值,低8位为B值*/
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4] 			= ((CharColor & 0xff000000) >> 24);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 1] 	= ((CharColor & 0x00ff0000) >> 16);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 2] 	= ((CharColor & 0x0000ff00) >> 8);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 3] 	=  (CharColor & 0x000000ff);
#endif
							}
							else
							{
#if VEHICLE_TYPE == VEHICLE_M
								/*设置背景颜色, 高8位为R值, 中8位为G值,低8位为B值*/
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3] 			= ((BackColor & 0xff0000) >> 16);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3 + 1] 	= ((BackColor & 0x00ff00) >> 8);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3 + 2] 	=  (BackColor & 0x0000ff); 
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
								/*设置字体颜色, 高8位为A值, 次高8位为R值, 中8位为G值,低8位为B值*/
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4] 			= ((BackColor & 0xff000000) >> 24);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 1] 	= ((BackColor & 0x00ff0000) >> 16);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 2] 	= ((BackColor & 0x0000ff00) >> 8);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 3] 	=  (BackColor & 0x000000ff);
#endif
							}
						}
					}
				}
				ChEn += 2; 
				x += 2;
			}
			else/*绘制字母及符号*/    
			{
				fseek(g_objDevFd.ASC,(long)(*ChEn - 32) * (ASCHIGH * ((ASCWIDTH%8)?(ASCWIDTH/8+1):ASCWIDTH/8)),SEEK_SET);
				fread(Ascmat,ASCHIGH * ((ASCWIDTH%8)?(ASCWIDTH/8+1):ASCWIDTH/8),1,g_objDevFd.ASC);
				for(i = 0; i < ASCHIGH; i++)
				{
					for(j = 0; j < ((ASCWIDTH%8)?(ASCWIDTH/8+1):ASCWIDTH/8); j++)		
					{		
						if(j == ASCWIDTH/8)
							EnConfine = ASCWIDTH%8;
						else
							EnConfine = 8;
								
						for(k = 0; k < EnConfine; k++)
						{
							if(Ascmat[i][j] & (0x80 >> k)) 
							{
#if VEHICLE_TYPE == VEHICLE_M
								/*设置字体颜色, 高8位为R值, 中8位为G值,低8位为B值*/
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3] 			= ((CharColor & 0xff0000) >> 16);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3 + 1]		= ((CharColor & 0x00ff00) >> 8);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3 + 2]		=  (CharColor & 0x0000ff);                 	
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
								/*设置字体颜色, 高8位为A值, 次高8位为R值, 中8位为G值,低8位为B值*/
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4] 			= ((CharColor & 0xff000000) >> 24);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 1] 	= ((CharColor & 0x00ff0000) >> 16);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 2]		= ((CharColor & 0x0000ff00) >> 8);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 3]		=  (CharColor & 0x000000ff);
#endif
							}
							else
							{
#if VEHICLE_TYPE == VEHICLE_M
								/*设置背景颜色, 高8位为R值, 中8位为G值,低8位为B值*/
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3] 			= ((BackColor & 0xff0000) >> 16);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3 + 1]		= ((BackColor & 0x00ff00) >> 8);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*3 + 2] 	=  (BackColor & 0x0000ff);            	
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
								/*设置字体颜色, 高8位为A值, 次高8位为R值, 中8位为G值,低8位为B值*/
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4] 			= ((BackColor & 0xff000000) >> 24);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 1] 	= ((BackColor & 0x00ff0000) >> 16);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 2]		= ((BackColor & 0x0000ff00) >> 8);
								pdata[((Bitmap->u32Width*i + 8*j + k) + (Bitmap->u32Width*LINEHIGH*(y + LineStart) + ROWWIDTH*(x + RowStart)))*4 + 3]		=  (BackColor & 0x000000ff);
#endif
							}
						}
					}
				}
				ChEn ++; 
				x ++;
			}
    }

    ChEn ++;
  }

	return 0;
}

/*在图片上画线,参数依次为直线颜色、水平线的始末横坐标或垂直线的始末纵坐标、水平线的纵坐标或垂直线的横坐标、直线类型*/
int DrawBeeline(OSD_BITMAP *Bitmap, uint BeelineColor, WORD BeelineStart, WORD BeelineEnd, WORD Location, BYTE Style)
{
	int x, y;
	char *pdata = (char *)Bitmap->pData;
	
	switch(Style)
	{
		case HORIZONTAL:/*绘制水平线*/
			if((BeelineEnd>720) || (Location>576))
			{
				PRTMSG(MSG_ERR, "the beeline go beyond\n");
				return -1;
			}
			
			y = Location;
			for(x = BeelineStart; x <= BeelineEnd; x ++)
			{
#if VEHICLE_TYPE == VEHICLE_M
				pdata[(Bitmap->u32Width*y + x)*3] 		= ((BeelineColor & 0xff0000) >> 16);
				pdata[(Bitmap->u32Width*y + x)*3 + 1] = ((BeelineColor & 0x00ff00) >> 8);
				pdata[(Bitmap->u32Width*y + x)*3 + 2] =  (BeelineColor & 0x0000ff);
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
				pdata[(Bitmap->u32Width*y + x)*4] 		= ((BeelineColor & 0xff000000) >> 24);
				pdata[(Bitmap->u32Width*y + x)*4 + 1] = ((BeelineColor & 0x00ff0000) >> 16);
				pdata[(Bitmap->u32Width*y + x)*4 + 2] = ((BeelineColor & 0x0000ff00) >> 8);
				pdata[(Bitmap->u32Width*y + x)*4 + 3] =  (BeelineColor & 0x000000ff);
#endif
			}
			break;
		
		case VERTICAL:/*绘制垂直线*/
			if((BeelineEnd>576) || (Location>720))
			{
				PRTMSG(MSG_ERR, "the beeline go beyond\n");
				return -1;
			}
			
			x = Location;
			for(y = BeelineStart; y <= BeelineEnd; y ++)
			{
#if VEHICLE_TYPE == VEHICLE_M
				pdata[(Bitmap->u32Width*y + x)*3] 		= ((BeelineColor & 0xff0000) >> 16);
				pdata[(Bitmap->u32Width*y + x)*3 + 1] = ((BeelineColor & 0x00ff00) >> 8);
				pdata[(Bitmap->u32Width*y + x)*3 + 2] =  (BeelineColor & 0x0000ff);
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
				pdata[(Bitmap->u32Width*y + x)*4] 		= ((BeelineColor & 0xff000000) >> 24);
				pdata[(Bitmap->u32Width*y + x)*4 + 1] = ((BeelineColor & 0x00ff0000) >> 16);
				pdata[(Bitmap->u32Width*y + x)*4 + 2] = ((BeelineColor & 0x0000ff00) >> 8);
				pdata[(Bitmap->u32Width*y + x)*4 + 3] =  (BeelineColor & 0x000000ff);
#endif
			}
			break;
			
		default:
			break;
	}

	return 0;
}

/*计算字符串行数*/
int CountCharLine(char *ChEn)
{
	int TotalLine = 0;
	
	while(*ChEn != '\0')
  {
		if(*ChEn & 0x80)       /*中文字符*/
		{
			ChEn += 2; 
		}
		else if(*ChEn == '\n') 
		{
			ChEn ++; 
			TotalLine ++;
		}
		else									/*西文字符*/    
		{
			ChEn ++; 
		}
  }
  
  return TotalLine;
}

/*计算字符串列数*/
int CountCharRow(char *ChEn)
{
	int TempRow = 0;
	int TotalRow = 0;

	while(*ChEn != '\0')
  {
		if(*ChEn & 0x80)       /*中文字符*/
		{
			ChEn += 2;
			TempRow += 2;
		}
		else if(*ChEn == '\n') 
		{
			ChEn ++; 
			TempRow = 0;
		}
		else									/*西文字符*/    
		{
			ChEn ++; 
			TempRow ++;
		}
		
		if(TempRow > TotalRow)
		{
			TotalRow = TempRow;
		}
  }
  
  return TotalRow;
}
