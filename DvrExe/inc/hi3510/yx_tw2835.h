 /* extdrv/include/TW2835.h
 *
 *
 * Copyright (c) 2006 Hisilicon Co., Ltd.
 *
 * History:
 *     10-Apr-2006 Start of Hi3510 TW2835 support
 *     2006-05-23  add record path 2 cif mod
 *     2006-08-12  adjust the order of the record path mod define
 */

#ifndef _TW2835_H
#define _TW2835_H

/* TW2835 i2c slaver address micro-definition. */
#define TW2835_I2C_ADDR 0x44

/*define the norm of input*/
#define VIDEO_NORM_NTSC         1
#define VIDEO_NORM_PAL          0

/************* io control parameter******************/
#define TW2835_READ_REG         0x01
#define TW2835_SET_REG          0x02

#define TW2835_SET_BRIGHT       0x05
#define TW2835_SET_CONTRAST     0x06
#define TW2835_SET_HUE          0x07
#define TW2835_SET_SAT          0x08
#define TW2835_GET_P_N          0x09

#define TW2835_SETNORM          0x10 /*16*/

#define TW2835_SET_1D1          0x11 /*17*/
#define TW2835_SET_1CIF         0x12 /*18*/
#define TW2835_SET_2HALF_D1     0x13 /*19*/
#define TW2835_SET_2CIF         0x14 /*20*/
#define TW2835_SET_4D1          0x15 /*21*/
#define TW2835_SET_4CIF         0x16 /*22*/
#define TW2835_SET_4QCIF        0x17 /*23*/

#define TW2835_SET_Y_SWITCH     0x19 /*25*/

#define TW2835_SET_X_D1         0x20 /*32*/
#define TW2835_SET_X_QUAD       0x21 /*33*/

#define TW2835_SET_VBI_OFFSET_V 0x30 /*48*/
#define TW2835_GET_VBI_OFFSET_V 0x31 /*49*/
#define TW2835_GET_VBI_OFFSET_H 0x32 /*50*/

#define TW2835_GETNORM          0x33 /*51*/
/* 音频功能 */
#define TW2835_SET_PB_SAMPLE_RATE     0x34
#define TW2835_SET_RM_SAMPLE_RATE     0x35
#define TW2835_AUDIO_RATIO                       0x36
#define TW2835_AOUT_CHANNEL_SET          0x37
#define TW2835_INIT                                        0x38

/* 视频回放控制 */
//#define TW2835_DISPLAY_4CH_PB			0x39
//#define TW2835_DISPLAY_4CH_LIVE   0x3A
//#define TW2835_DISPLAY_1CH_D1_PB			0x3B  //CHANNELx (0~3) full screen playback

#define TW2835_AD_DA_POW				0x3C
#define TW2835_GPIO_SET					0x3D
#define TW2835_GPIO_WRITE				0x3E
#define TW2835_GPIO_READ				0x3F
#define TW2835_GPIO_EN					0x40

#define TW2835_DAC_POW_OFF			0x41
#define TW2835_DAC_POW_ON				0x42

#define TW2835_TEST                      		0x43
#define TW2835_IRQ_EN                       0x44
#define TW2835_IRQ_DIS                      0x45
#define TW2835_IRQ_POL_SET                  0x46
#define TW2835_IRQ_STATUS_READ              0x47
#define TW2835_AOUT_MIXED				0x48
#define TW2835_AOUT_PB				0x49
#define TW2835_AOUT_MUTED				0x4a

/*process of bit*/
#define SET_BIT(x,y)        ((x) |= (y))
#define CLEAR_BIT(x,y)      ((x) &= ~(y))
#define CHKBIT_SET(x,y)     (((x)&(y)) == (y))
#define CHKBIT_CLR(x,y)     (((x)&(y)) == 0)

extern unsigned int TW2835_write_byte(int, int, int);
extern unsigned char TW2835_read_byte(int, int);


#endif
