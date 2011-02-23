#ifndef _YX_OSD_H_
#define _YX_OSD_H_ 

#if VEHICLE_TYPE == VEHICLE_M
typedef OSD_BITMAP_S OSD_BITMAP;
//typedef OSD_REGION_HANDLE OSD_REGION;
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
typedef struct yxOSD_BITMAP_S
{ 
	unsigned int u32Width;           
  unsigned int u32Height;
  unsigned char *pData;
}OSD_BITMAP;
#endif

typedef struct yxOSD_REGION_S
{
	unsigned int Handle;
	unsigned char *MappedMem;
}OSD_REGION;

#define SCRWIDTH  	720 		/*屏幕宽度*/
#define SCRHIGH  		576 		/*屏幕高度*/ 
#define HZKWIDTH 		24  		/*中文字符宽度*/
#define HZKHIGH  		24  		/*中文字符高度*/
#define ASCWIDTH 		12  		/*英文字符宽度*/
#define ASCHIGH  		24  		/*英文字符高度*/
#define LINEHIGH		24			/*行高*/
#define ROWWIDTH		12			/*列宽*/

#define HORIZONTAL	0				/*水平线*/
#define VERTICAL		1				/*垂直线*/	

int CreateRegion(OSD_REGION *Region, uint TopLevel, WORD LineStart, WORD RowStart, WORD Line, WORD Row, int Fb=0);
int DeleteRegion(OSD_REGION Region);
int ShowRegion(OSD_REGION Region);
int HideRegion(OSD_REGION Region);

int CreateBitmap(OSD_BITMAP *Bitmap, WORD Line, WORD Row);
int DrawCharacter(OSD_BITMAP *Bitmap, char *ChEn, uint CharColor, uint BackColor, WORD LineStart, WORD RowStart, BOOL Flag);
int DrawBeeline(OSD_BITMAP *Bitmap, uint BeelineColor, WORD BeelineStart, WORD BeelineEnd, WORD Location, BYTE Style);
int ShowBitmap(OSD_REGION *Region, OSD_BITMAP *Bitmap, WORD LineStart, WORD RowStart);
int CountCharLine(char *ChEn);
int CountCharRow(char *ChEn);

#endif
