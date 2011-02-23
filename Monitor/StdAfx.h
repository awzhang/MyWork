#ifndef _STDATX_H_
#define _STDATX_H_

#include 	<time.h>
#include 	<stdio.h>
#include	<fcntl.h> 
#include	<errno.h>
#include  <netdb.h>
#include 	<stdlib.h>
#include	<string.h>
#include  <signal.h>
#include  <stdarg.h>
#include  <assert.h>
#include	<unistd.h>
#include	<dirent.h>
#include 	<getopt.h>
#include	<termios.h>
#include  <pthread.h>
#include  <arpa/inet.h>
#include  <netinet/in.h>
#include  <asm/page.h>  
#include  <sys/uio.h>
#include 	<sys/vfs.h>
#include  <sys/ipc.h> 
#include  <sys/shm.h>
#include  <sys/sem.h> 
#include	<sys/stat.h>
#include  <sys/mman.h>
#include  <sys/poll.h>
#include  <sys/time.h>
#include  <sys/wait.h>
#include 	<sys/mount.h>
#include	<sys/types.h>
#include  <sys/ioctl.h>
#include  <sys/select.h>
#include  <sys/socket.h>
#include  <linux/fs.h>
#include  <linux/fb.h>
#include  <linux/rtc.h>
#include  <linux/ioctl.h>
#include  <linux/types.h>

#include "../GlobalShare/ComuServExport.h"
#include "../GlobalShare/GlobFunc.h"
#include "Dog.h"

#define MSG_NOR		1
#define MSG_DBG		1
#define MSG_ERR		1
#define MSG_HEAD	("")

#define PRTMSG(enb, format, ...) \
if( enb ) \
{ \
	printf( "%s: ", MSG_HEAD ); \
	printf( format, ##__VA_ARGS__ ); \
}

#define ErrLog  ("/mnt/Flash/part3/ErrLog")
#define InitLog ("part:5 error:0 update:0")

extern CDog g_objDog;

int SYSTEM(char *v_szFormat, ...);
bool AllAppExist(char v_szPart);
bool G_ExeUpdate(char *v_szFileName, int v_iFlag);

int CreateLog();
int DeleteLog();
int ReadLog(char &v_szPart, char &v_szError, char &v_szUpdate);
int WriteLog(char &v_szPart, char &v_szError, char &v_szUpdate);

#endif








