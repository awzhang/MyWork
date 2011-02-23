/*******************************************************************
*
*   文件名:     YX_GPIO_API.c
*   版权:       厦门雅迅网络股份有限公司
*   创建人:     任赋    2009年5月18日
*
*   版本号:     1.0
*   文件描述:   HI平台GPIO应用接口
*
*******************************************************************/
#define _REENTRANT

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "YX_GPIO_API.h"

/*
********************************************************************************
* 定义宏调用
********************************************************************************
*/
#define GPIO_FD     gpiodrv_fd

/*
********************************************************************************
* 定义模块变量
********************************************************************************
*/
static INT32S gpiodrv_fd = -1;

static void YX_GPIO_Handler(int signo, siginfo_t *info, void *context)
{
	void (*intinform)(INT32U, INT32U);
	INT32U tmpdata[3];
	
	tmpdata[0] = info->si_int;
	ioctl(GPIO_FD, GPIOIOC_GETCALLBACK, tmpdata);
	intinform =(void (*)(INT32U, INT32U))tmpdata[0];
	if (intinform != 0) {
	    intinform(tmpdata[1], tmpdata[2]);
	}
}

void YX_GPIO_Init(void)
{
    gpiodrv_fd = open("/dev/misc/gpiodrv", O_RDWR);
    if(-1 == gpiodrv_fd){
        printf("open gpiodrv error!\n");
    }
}

void YX_GPIO_SetDirection(INT32U gpio_id, BOOLEAN directions)
{
    INT32U tmpdata[2];
    
    tmpdata[0] = gpio_id;
    tmpdata[1] = directions & 0x01;
    ioctl(GPIO_FD, GPIOIOC_SETDIRECTION, tmpdata);
}

BOOLEAN YX_GPIO_GetDirection(INT32U gpio_id)
{
    INT32U tmpdata[8];
    
    tmpdata[0] = gpio_id;
    ioctl(GPIO_FD, GPIOIOC_GETPININFO, tmpdata);
    if (tmpdata[0] & 0x01) {
        return true;
    } else {
        return false;
    }
}
    
void YX_GPIO_SetValue(INT32U gpio_id, BOOLEAN b_on)
{
    INT32U tmpdata[2];
    
    tmpdata[0] = gpio_id;
    if (b_on) {
        tmpdata[1] = GPIO_PIN_HIGH;
    } else {
        tmpdata[1] = GPIO_PIN_LOW;
    }
    ioctl(GPIO_FD, GPIOIOC_SETPINVALUE, tmpdata);
}

BOOLEAN YX_GPIO_GetValue( INT32U gpio_id )
{
    INT32U tmpdata[8];
    GPIO_INFO_T *p;
    
    tmpdata[0] = gpio_id;
    ioctl(GPIO_FD, GPIOIOC_GETPININFO, tmpdata);
    p = (GPIO_INFO_T *)tmpdata;
    if (p->data == GPIO_PIN_HIGH) {
        return true;
    } else {
        return false;
    }
}

void YX_GPIO_Release(void)
{
    close(GPIO_FD);
}

void YX_GPIO_INTEnable(INT32U gpio_id)
{
    INT32U tmpdata;
    
    tmpdata = gpio_id;
    ioctl(GPIO_FD, GPIOIOC_ENABLEIRQ, &tmpdata);
}

void YX_GPIO_INTDisable(INT32U gpio_id)
{
    INT32U tmpdata;
    
    tmpdata = gpio_id;
    ioctl(GPIO_FD, GPIOIOC_DISABLEIRQ, &tmpdata);
}
   
void YX_GPIO_INTConfig(INT32U gpio_id, INT32U mode, INT32U edge, INT32U level)
{
    INT32U tmpdata[4];
    
    tmpdata[0] = gpio_id;
    tmpdata[1] = mode & 0x01;
    tmpdata[2] = edge & 0x01;
    tmpdata[3] = level & 0x01;
    ioctl(GPIO_FD, GPIOIOC_SETIRQALL, tmpdata);
}

void YX_GPIO_INTSetMode(INT32U gpio_id, INT32U mode)
{
    INT32U tmpdata[2];
    
    tmpdata[0] = gpio_id;
    tmpdata[1] = mode & 0x01;
    ioctl(GPIO_FD, GPIOIOC_SETTRIGGERMODE, tmpdata);
}

void YX_GPIO_INTSetEdge(INT32U gpio_id, INT32U edge)
{
    INT32U tmpdata[2];
    
    tmpdata[0] = gpio_id;
    tmpdata[1] = edge & 0x01;
    ioctl(GPIO_FD, GPIOIOC_SETTRIGGERIBE, tmpdata);
}

void YX_GPIO_INTSetLevel(INT32U gpio_id, INT32U level)
{
    INT32U tmpdata[2];
    
    tmpdata[0] = gpio_id;
    tmpdata[1] = level & 0x01;
    ioctl(GPIO_FD, GPIOIOC_SETTRIGGERIEV, tmpdata);
}
   
void YX_GPIO_INTSethandler(INT32U gpio_id, void (*inthandler)(INT32U, INT32U))
{
    INT32U tmpdata[3];
    struct sigaction new_sa;
    
    tmpdata[0] = gpio_id;
    tmpdata[1] = getpid();
    tmpdata[2] = (INT32U)inthandler;
    ioctl(GPIO_FD, GPIOIOC_SETCALLBACK, tmpdata);
	new_sa.sa_sigaction = YX_GPIO_Handler;
	sigemptyset(&new_sa.sa_mask);
	new_sa.sa_flags = SA_RESTART | SA_SIGINFO;
	new_sa.sa_restorer = NULL;
	sigaction(SIGUSR1, &new_sa, NULL);
}

