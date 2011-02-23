#ifndef _YX_DISK_H_
#define _YX_DISK_H_ 

#if VEHICLE_TYPE == VEHICLE_M
#define BUS_SDISK ("/sys/bus/usb/drivers/usb-storage/1-2.1:1.0")
#define BUS_HDISK ("/sys/bus/usb/drivers/usb-storage/1-1:1.0")
#define BUS_UDISK ("/sys/bus/usb/drivers/usb-storage/1-2.2:1.0")
#define DEV_SDISK ("/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.1/1-2.1:1.0")
#define DEV_HDISK ("/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1:1.0")
#define DEV_UDISK ("/sys/devices/platform/hisilicon-ohci.0/usb1/1-2/1-2.2/1-2.2:1.0")
#endif

#if VEHICLE_TYPE == VEHICLE_M2
#define BUS_SDISK ("/sys/bus/usb/drivers/usb-storage/1-1.1:1.0")
#define BUS_HDISK ("/sys/bus/usb/drivers/usb-storage/1-1.3:1.0")
#define BUS_UDISK ("/sys/bus/usb/drivers/usb-storage/2-1:1.0")
#define DEV_SDISK ("/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.1/1-1.1:1.0")
#define DEV_HDISK ("/sys/devices/platform/hisilicon-ohci.0/usb1/1-1/1-1.3/1-1.3:1.0")
#define DEV_UDISK ("/sys/devices/platform/hiusb-hcd.0/usb2/2-1/2-1:1.0")
#endif

#if VEHICLE_TYPE == VEHICLE_V8
#define BUS_UDISK ("/sys/bus/usb/drivers/usb-storage/2-1:1.0")
#define DEV_UDISK ("/sys/devices/platform/hiusb-hcd.0/usb2/2-1/2-1:1.0")
#endif

//磁盘使用率上下限
#define MAX_INODE_USAGE_SDISK 0.90
#define MIN_INODE_USAGE_SDISK 0.80
#define MAX_INODE_USAGE_HDISK 0.90
#define MIN_INODE_USAGE_HDISK 0.80
#define MAX_INODE_USAGE_UDISK 0.90
#define MIN_INODE_USAGE_UDISK 0.80

#define MAX_SPACE_USAGE_SDISK 0.90
#define MIN_SPACE_USAGE_SDISK 0.80
#define MAX_SPACE_USAGE_HDISK 0.90
#define MIN_SPACE_USAGE_HDISK 0.80
#define MAX_SPACE_USAGE_UDISK 0.90
#define MIN_SPACE_USAGE_UDISK 0.80

//磁盘检测间隔(秒)
#define VALID_DECCNT_SDISK 6 
#define VALID_DECCNT_HDISK 6
#define VALID_DECCNT_UDISK 6

#define SPACE_DECCNT_SDISK 60 
#define SPACE_DECCNT_HDISK 60
#define SPACE_DECCNT_UDISK 60

#define INODE_DECCNT_SDISK 60*60
#define INODE_DECCNT_HDISK 60*60
#define INODE_DECCNT_UDISK 60*60

//磁盘分区数
#define PART_NUM_SDISK 2
#define PART_NUM_HDISK 2
#define PART_NUM_UDISK 1

//磁盘未挂载复位时间(秒)
#define UMOUNT_REBOOT_SDISK 1200 
#define UMOUNT_REBOOT_HDISK 1200 
#define UMOUNT_REBOOT_UDISK 1200 

//磁盘未识别复位时间(秒)
#define UCONNECT_INIT_SDISK 60 
#define UCONNECT_INIT_HDISK 60 
#define UCONNECT_INIT_UDISK 60 

bool UCONNECT(void *Event);
bool UMOUNT(void *Event);
bool MOUNT(void *Event);
bool UVALID(void *Event);
bool VALID(void *Event);
void UmountDisk(int v_iDiskStyle, unsigned int v_uiDelay=0);
void MountDisk(int v_iDiskStyle, unsigned int v_uiDelay=0);
bool CtrlDiskPart( char *v_szDiskPath, int v_iDiskPart, int v_iDiskSize, char *v_szFileSystem, int v_iInode, int v_iFlag );

int GetDiskSystem( int v_iDiskStyle, int v_iDiskPart, char *v_szFileSystem );
void GetDiskSize( int v_iDiskStyle, int v_iDiskPart, char *v_szTotalSize, char *v_szFreeSize, char *v_szAvailSize );
void GetDiskSpace( int v_iDiskStyle, int v_iDiskPart, long long *v_lTotalSize, long long *v_lFreeSize, long long *v_lAvailSize );
int GetFileNum(char *v_szPath, char *v_szName, bool v_bRecur);
void FlushAVList(char *v_szAVListFile, FILE_INDEX *v_pAVFileIndex, char *v_szDelFileTime, int v_iFlag);
int ClearDisk(char *v_szDiskStyle, char *v_szDiskPart);
int FileSearch(FILE_INDEX **v_ppAVFileIndex, FILE_SEARCH v_objFileSearch);
void *G_DiskManage(void *arg);

int DiskFormat(char *v_szDiskStyle);
int InitSd();
int ResetUsb();
int DiskTest(char *v_szDiskStyle);

#endif
