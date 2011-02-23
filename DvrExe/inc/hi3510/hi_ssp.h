/*
 * extdrv/include/hi_ssp.h for Linux .
 *
 * History: 
 *      2006-4-11 create this file
 */ 
 
#ifndef __HI_SSP_H__
#define __HI_SSP_H__

    

	extern void hi_ssp_enable(void);
	extern void hi_ssp_disable(void);
	extern int hi_ssp_set_frameform(unsigned char,unsigned char,unsigned char,unsigned char);
	extern int hi_ssp_set_serialclock(unsigned char,unsigned char);
	extern void hi_ssp_set_inturrupt(unsigned char);
	extern void hi_ssp_interrupt_clear(void);
	extern void hi_ssp_dmac_enable(void);
	extern void hi_ssp_dmac_disable(void);
	extern unsigned int hi_ssp_busystate_check(void);
	extern int hi_ssp_readdata(void);
	extern void hi_ssp_writedata(unsigned short);
#ifdef CONFIG_SSP_DMA
	extern int hi_ssp_dmac_init(void *,void *);
	extern int hi_ssp_dmac_transfer(unsigned int,unsigned int,unsigned int);
	extern void hi_ssp_dmac_exit(void);
#endif	
#endif 

