#ifndef _YX_MENU_H_
#define _YX_MENU_H_ 

#define PAGE_LINE			24							/*页面行数*/
#define PAGE_ROW			60							/*页面列数*/
#define PAGE_LEVEL		0								/*页面层次*/
#define DLG_LEVEL 	1								/*对话框层次*/
#define DLG_INFO  		0 							/*提示对话框*/
#define DLG_SUCC  		1 							/*成功对话框*/ 
#define DLG_ERR 			2  							/*失败对话框*/

#define LANG_PAGE_00 ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                  密码:                                     \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_01 ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_02 ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_03 ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                          文件查询                          \n\
                                                            \n\
                                                            \n\
      通道选择:               查询日期:                     \n\
                                                            \n\
      开始时间:               结束时间:                     \n\
                                                            \n\
      存储磁盘:                                             \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_04 ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                          录像搜索                          \n\
                                                            \n\
                                                            \n\
      通道选择:               搜索日期:                     \n\
                                                            \n\
      开始时间:               结束时间:                     \n\
                                                            \n\
      存储磁盘:                                             \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_05 ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                      磁盘:                                 \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
          警告: 此操作将删除选定磁盘上的所有数据            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_06 ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                警告: 恢复出厂设置将使用户                  \n\
                      所有的设置变成默认值                  \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                警告: 升级完成后将重启设备                  \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                警告: 升级完成后将重启设备                  \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_07 ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
          软件版本号:                                       \n\
                                                            \n\
                                                            \n\
            磁盘      格  式    总容量    剩余空间          \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_08 ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                  日期:                                     \n\
                                                            \n\
                  时间:                                     \n\
                                                            \n\
                  司机编号:                                 \n\
                                                            \n\
                  车牌号码:                                 \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_09 ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_0a ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                  当前密码:                                 \n\
                                                            \n\
                  新密码:                                   \n\
                                                            \n\
                  确定新密码:                               \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_0b ("\
                                                            \n\
                          查 询 结 果                       \n\
                                                            \n\
    通道  创建日期    开始时间   结束时间  帧率  格式  大小 \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                        页码:               \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_0c ("\
                                                            \n\
                          搜索结果                          \n\
                                                            \n\
          通道  创建日期    开始时间    结束时间            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                        页码:               \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_0d ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                  触发模式:                                 \n\
                                                            \n\
                                                            \n\
                  存储磁盘:                                 \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_0e ("\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
        视频制式:                                           \n\
                                                            \n\
                                                            \n\
        通道  分辨率    画质    帧率    录像    预览        \n\
                                                            \n\
        CHN1                                                \n\
                                                            \n\
        CHN2                                                \n\
                                                            \n\
        CHN3                                                \n\
                                                            \n\
        CHN4                                                \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_0f ("\
                                                            \n\
                                                            \n\
                                                            \n\
                        时    段            使能            \n\
                                                            \n\
            1             ----                              \n\
                                                            \n\
            2             ----                              \n\
                                                            \n\
            3             ----                              \n\
                                                            \n\
            4             ----                              \n\
                                                            \n\
            5             ----                              \n\
                                                            \n\
            6             ----                              \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_10 ("\
                                                            \n\
                                                            \n\
                  事件      延迟      使能                  \n\
                                                            \n\
                                                            \n\
                  点火                                      \n\
                                                            \n\
                  运动                                      \n\
                                                            \n\
                  遮挡                                      \n\
                                                            \n\
                  保留                                      \n\
                                                            \n\
                  保留                                      \n\
                                                            \n\
                  保留                                      \n\
                                                            \n\
                  保留                                      \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n\
                                                            \n")
#define LANG_PAGE_11 ("\
                                                            \n\
                通信模块:                                   \n\
                                                            \n\
                本机号码:                                   \n\
                                                            \n\
                接入点:                                     \n\
                                                            \n\
                通信IP:                                     \n\
                                                            \n\
                通信端口:                                   \n\
                                                            \n\
                照片IP:                                     \n\
                                                            \n\
                照片端口:                                   \n\
                                                            \n\
                视频IP:                                     \n\
                                                            \n\
                视频端口:                                   \n\
                                                            \n\
                升级IP:                                     \n\
                                                            \n\
                升级端口:                                   \n\
                                                            \n\
                                                            \n")

enum
{
#if VEHICLE_TYPE == VEHICLE_M
	WHITE  = 0xFFFFFF,
	BLACK	 = 0x0D0D0D,
	RED		 = 0xFF0000,
	BLUE 	 = 0x0000FF,
	YELLOW = 0xFFFF00,
	ORCHID = 0xDB70DB, //淡紫色
	PURPLE = 0x9932CD, //紫色
	MODENA = 0x871F78  //深紫色
#endif
#if VEHICLE_TYPE == VEHICLE_V8 || VEHICLE_TYPE == VEHICLE_M2
	WHITE  = 0xFFFFFFFF,
	BLACK	 = 0x000000FF,
	RED		 = 0x0000FFFF,
	BLUE 	 = 0xFF0000FF,
	YELLOW = 0x00FFFFFF,
	ORCHID = 0xD670DAFF, //淡紫色
	PURPLE = 0xF020A0FF, //紫色
	MODENA = 0xFA3399FF  //深紫色
#endif
};

typedef struct hiMENU_DIALOG
{
	char Content[50];									/*对话框内容*/
	WORD Location[2];									/*对话框位置*/
	uint CharColor;										/*对话框字体颜色*/
	uint BackColor;										/*对话框背景颜色*/
}MENU_DIALOG;

typedef struct hiMENU_ITEM
{
	char	Content[64];									/*选项内容*/
	BYTE 	Location[2];									/*选项位置*/
	uint 	CharColor;										/*选项字体颜色*/
	uint 	BackColor;										/*选项背景颜色*/
	BOOL	Fix;													/*选项是否选定*/
	BOOL	Exist;												/*选项是否存在*/
	
	BYTE  Pid;													/*当前页面编号*/
	BYTE  Iid;													/*当前选项编号*/
	BYTE 	Uid;											    /*向上键对应的选项编号*/
	BYTE 	Did;										      /*向下键对应的选项编号*/
	BYTE 	Lid;										      /*向左键对应的选项编号*/
	BYTE 	Rid;										      /*向右键对应的选项编号*/
	BYTE 	Eid;										      /*确定键对应的页面编号*/
	BYTE 	Cid;									        /*退出键对应的页面编号*/
	
	MENU_DIALOG InfoDialog;							/*选项运行时的提示对话框*/
	MENU_DIALOG SuccDialog;							/*选项运行时的成功对话框*/
	MENU_DIALOG ErrDialog;							/*选项运行时的失败对话框*/
	
	OSD_REGION Region;							    /*选项对应区域*/
}MENU_ITEM;

typedef struct hiMENU_PAGE
{
	char 	Content[1500];						/*页面内容*/
	WORD 	Location[2];							/*页面位置*/
	uint 	CharColor;								/*页面字体颜色*/
	uint 	BackColor;								/*页面背景颜色*/
	BYTE  Pid;											/*页面编号*/
	WORD	Num;							        /*页面中的选项个数*/
	MENU_ITEM *MenuItem;						/*页面中的选项指针*/
	
	OSD_REGION Region;							/*页面对应区域*/
}MENU_PAGE;

typedef struct yxEN_MODE_FRAME
{
	int CIFFrame;
	int HD1Frame;
	int D1Frame;
}EN_MODE_FRAME;

struct Frm35 
{
	byte type;
	byte reply;		//1:网络恢复(拨号成功)  2:网络恢复(Ping成功)   3:网络断开了
	Frm35() 
	{ 
		type = 0x35; 
	}
};

int DealKeyLogin();
int DealKeyLock();
int DealKeyDirect(int v_iKeyStyle);
int DealKeyNumber(char*);
int DealKeyEnter();
int DealKeyCancel();

#endif
