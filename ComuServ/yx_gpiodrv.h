/*******************************************************************
*
*   文件名:     yx_gpiodrv.h
*   版权:       厦门雅迅网络股份有限公司
*   创建人:     任赋    2009年5月11日
*
*   版本号:     1.0
*   文件描述:   HI平台GPIO驱动模块头文件
*
************************修改记录1************************************
*
*	修改者:		纪晓军	2009年11月18日
*	版本号：	1.1
*	
*	修改描述：	增加在Hi3511平台下gpio复用寄存器的读写
**********************************************************************/

#ifndef _LINUX_YX_GPIODRV_H_
#define _LINUX_YX_GPIODRV_H_

#include <linux/ioctl.h>
#include <linux/types.h>
//#include <asm/arch/platform.h>
//#include <asm/arch/hardware.h>

// 定义引脚输入输出方向
#define GPIO_INPUT                      0
#define GPIO_OUTPUT                     1
// 定义引脚高低电平
#define GPIO_PIN_LOW                    0
#define GPIO_PIN_HIGH                   1
// 定义引脚触发模式
#define INT_TRIGGER_EDGE                0               
#define INT_TRIGGER_LEVEL               1
// 定义引脚触发边沿类型
#define DOUBLE_EDGE_TRIGGER             1
#define SINGLE_EDGE_TRIGGER             0
// 定义引脚触发类型
#define FALL_EDGE_TRIGGER               0
#define RAISE_EDGE_TRIGGER              1


/*定义用来获取GPIO引脚的配置信息*/
typedef struct {
	__u32 direction;        // 引脚方向
	__u32 data;             // 引脚电平
	__u32 irqenable;        // 中断使能
	__u32 irq_is;           // 中断触发模式
	__u32 irq_ibe;          // 中断边沿模式 
	__u32 irq_iev;          // 中断电平模式 
// 下面为调试而设
	__u32 irq_mis;	      // 当前中断状态			          
} GPIO_INFO_T;

#define	GPIODRV_IOCTL_BASE	'G'

#define GPIOIOC_SETDIRECTION    _IOW(GPIODRV_IOCTL_BASE, 0, int)
#define GPIOIOC_SETPINVALUE     _IOW(GPIODRV_IOCTL_BASE, 1, int)
#define GPIOIOC_SETTRIGGERMODE  _IOW(GPIODRV_IOCTL_BASE, 2, int)
#define GPIOIOC_SETTRIGGERIBE   _IOW(GPIODRV_IOCTL_BASE, 3, int)
#define GPIOIOC_SETTRIGGERIEV   _IOW(GPIODRV_IOCTL_BASE, 4, int)
#define GPIOIOC_SETIRQALL       _IOW(GPIODRV_IOCTL_BASE, 5, int)
#define GPIOIOC_SETCALLBACK     _IOW(GPIODRV_IOCTL_BASE, 6, int)
#define GPIOIOC_ENABLEIRQ       _IO(GPIODRV_IOCTL_BASE,  7)
#define GPIOIOC_DISABLEIRQ      _IO(GPIODRV_IOCTL_BASE,  8)
#define GPIOIOC_GETPININFO      _IOR(GPIODRV_IOCTL_BASE, 9, GPIO_INFO_T)
#define GPIOIOC_GETCALLBACK     _IOR(GPIODRV_IOCTL_BASE, 10, int)

#define YX_GPIO_GET_SC_PERCTRL1   (*(volatile unsigned long *)(IO_ADDRESS(REG_BASE_SCTL)+0x20))
#define YX_GPIO_SET_SC_PERCTRL1(v)  do{ *(volatile unsigned long *)(IO_ADDRESS(REG_BASE_SCTL)+0x20)=v;} while(0)
#define YX_GPIO_SC_PERCTRL1(g,b)      YX_GPIO_SET_SC_PERCTRL1 ((YX_GPIO_GET_SC_PERCTRL1 & (~(1<<(g)))) | ((b)<<(g)))
#endif

