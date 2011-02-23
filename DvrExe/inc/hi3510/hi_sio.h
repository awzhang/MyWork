/* File name :hi_sio.h                         
 *                         
 * Create date: 2006-04-26                               
 *                                    
 */            
#ifndef __hi_SIO_H__
#define __hi_SIO_H__

	
/*get cmd*/	
#define GET_SIO_SAMPLERATE  _IOR('S', 1,int)
#define GET_SIO_TX_BITWIDTH _IOR('S', 2,int)
#define GET_SIO_RX_BITWIDTH _IOR('S', 3,int)	
#define GET_SIO_I2SMODE     _IOR('S', 4,int)
#define SET_SIO_SAMPLERATE  _IOW('S', 5,int)
#define SET_SIO_TX_BITWIDTH _IOW('S', 6,int)
#define SET_SIO_RX_BITWIDTH _IOW('S', 7,int)
#define SET_SIO_I2SMODE     _IOW('S', 8,int)	
#define I2S_RX_ENABLE       _IOW('S', 9,int)
#define I2S_RX_DISABLE      _IOW('S', 10,int)
#define I2S_TX_ENABLE       _IOW('S', 11,int)
#define I2S_TX_DISABLE      _IOW('S', 12,int)	
#define SET_PCM_CT          _IOW('S', 13,int)
#define GET_PCM_CT          _IOW('S', 14,int)
#define SIO_1_INIT          _IOW('S', 17,int)

#endif /* End of #ifdef __HI_INC_SIO_H__ */


