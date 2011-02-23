/*
 * extdrv/include/hi_i2c.h for Linux .
 *
 * History: 
 *      10-April-2006 create this file
 *
 *
 */

#ifndef __HI_I2C_H__
#define __HI_I2C_H__


extern unsigned char hi_i2c_read(unsigned char, unsigned char);
extern int hi_i2c_write(unsigned char,unsigned char,unsigned char);
extern int hi_i2c_writespecial(unsigned char,unsigned char,unsigned char,unsigned char);
extern unsigned char hi_i2c_readspecial(unsigned char, unsigned char,unsigned char);



#endif 
