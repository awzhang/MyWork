/*******************************************************************
*
*   文件名:     yx_perspm.h
*   版权:       厦门雅迅网络股份有限公司
*   创建人:     任赋    2009年5月20日
*
*   版本号:     1.0
*   文件描述:   HI平台各外围电源管理模块头文件
*
*******************************************************************/
#ifndef _LINUX_YX_PERSPM_H_
#define _LINUX_YX_PERSPM_H_

#include <linux/ioctl.h>
#include <linux/types.h>

#define	PERSPM_IOCTL_BASE	'P'

#define PERSPMIOC_SETCLKENABLE      _IOW(PERSPM_IOCTL_BASE, 50, int)
#define PERSPMIOC_SETCLKDISABLE     _IOW(PERSPM_IOCTL_BASE, 51, int)
#define PERSPMIOC_GETCLKSTATE       _IOR(PERSPM_IOCTL_BASE, 52, int)

#define PERSPMIOC_GETSYSINFO        _IOR(PERSPM_IOCTL_BASE, 53, int)
#define PERSPMIOC_DDRC_SETDISABLE   _IO(PERSPM_IOCTL_BASE, 54)
#define PERSPMIOC_SETSYSSTATUS      _IOW(PERSPM_IOCTL_BASE, 55, int)
#define PERSPMIOC_SETSYSRESET       _IO(PERSPM_IOCTL_BASE, 56)

#endif
