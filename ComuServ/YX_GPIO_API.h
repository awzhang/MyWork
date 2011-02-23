/*******************************************************************
*
*   文件名:     YX_GPIO_API.h
*   版权:       厦门雅迅网络股份有限公司
*   创建人:     任赋    2009年5月18日
*
*   版本号:     1.0
*   文件描述:   HI平台GPIO应用接口
*
*******************************************************************/
#ifndef _LINUX_YX_GPIO_API_H_
#define _LINUX_YX_GPIO_API_H_

#include "yx_gpiodrv.h"
#include "yx_system.h"

void YX_GPIO_Init(void);
void YX_GPIO_SetDirection(INT32U gpio_id, BOOLEAN directions);
BOOLEAN YX_GPIO_GetDirection(INT32U gpio_id);  
void YX_GPIO_SetValue(INT32U gpio_id, BOOLEAN b_on);
BOOLEAN YX_GPIO_GetValue( INT32U gpio_id );
void YX_GPIO_Release(void);
void YX_GPIO_INTEnable(INT32U gpio_id);
void YX_GPIO_INTDisable(INT32U gpio_id);  
void YX_GPIO_INTConfig(INT32U gpio_id, INT32U mode, INT32U edge, INT32U level);
void YX_GPIO_INTSetMode(INT32U gpio_id, INT32U mode);
void YX_GPIO_INTSetEdge(INT32U gpio_id, INT32U edge);
void YX_GPIO_INTSetLevel(INT32U gpio_id, INT32U level);
void YX_GPIO_INTSethandler(INT32U gpio_id, void (*inthandler)(INT32U, INT32U ));
#endif
