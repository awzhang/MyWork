/******************************************************************
*
*   文件名:     yx_iomultplex.c
*   版权:       厦门雅迅网络股份有限公司
*   创建人:     纪晓军    2009年12月2日
*
*   版本号:     1.0
*   文件描述:   HI3511平台IO复用选择模块
*
*******************************************************************/
#include <asm/arch/platform.h>
#include <asm/arch/hardware.h>

#define YX_GPIO_GET_SC_PERCTRL1   (*(volatile unsigned long *)(IO_ADDRESS(REG_BASE_SCTL)+0x20))
#define YX_GPIO_SET_SC_PERCTRL1(v)  do{ *(volatile unsigned long *)(IO_ADDRESS(REG_BASE_SCTL)+0x20)=v;} while(0)
#define YX_GPIO_SC_PERCTRL1(g,b)      YX_GPIO_SET_SC_PERCTRL1 ((YX_GPIO_GET_SC_PERCTRL1 & (~(1<<(g)))) | ((b)<<(g)))

extern void yx_iomul(unsigned int group, unsigned int offset, unsigned int pur);
