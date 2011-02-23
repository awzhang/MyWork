/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_struct.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/02/13
  Description   : 
  History       :
  1.Date        : 2006/02/13
    Author      : luochuanzao
    Modification: Created file

  2.Date         : 2006/2/13
    Author       : QuYaxin 46153
    Modification : 
******************************************************************************/


#ifndef __HI_STRUCT_H__
#define __HI_STRUCT_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define VENC_MAX_CHN_NUM 8
#define VENC_MAX_STREAM_NUM 2
#define VENC_MAX_CAP 720*576*25*5 /* The max ability of VENC Pix/s */

typedef enum hiAUDIO_CODEC_FORMAT_E
{
    AUDIO_CODEC_FORMAT_G711A   = 1,   /* G.711 A            */
    AUDIO_CODEC_FORMAT_G711Mu  = 2,   /* G.711 Mu           */
    AUDIO_CODEC_FORMAT_ADPCM   = 3,   /* ADPCM              */
    AUDIO_CODEC_FORMAT_G726    = 4,   /* G.726              */
    AUDIO_CODEC_FORMAT_AMR     = 5,   /* AMR encoder format */
    AUDIO_CODEC_FORMAT_AMRDTX  = 6,   /* AMR encoder formant and VAD1 enable */
    AUDIO_CODEC_FORMAT_AAC     = 7,   /* AAC encoder        */
    AUDIO_CODEC_FORMAT_ORG_G711A   = 8,   /* Old G.711 A    */
    AUDIO_CODEC_FORMAT_ORG_G711Mu  = 9,   /* Old G.711 Mu   */
    AUDIO_CODEC_FORMAT_ORG_ADPCM   = 10,  /* Old ADPCM      */
    AUDIO_CODEC_FORMAT_ORG_G726    = 11,  /* Old G.726      */
    AUDIO_CODEC_FORMAT_BUTT
} AUDIO_CODEC_FORMAT_E;

/* Transfer old audio stream to new format */
typedef enum hiAUDIO_STREAM_TRANSFER_E
{
    AUDIO_STREAM_TRANSFER_G711A = 0, /* For G711A */
    AUDIO_STREAM_TRANSFER_G711U = 1, /* For G711U */
    AUDIO_STREAM_TRANSFER_ADPCM = 2, /* For ADPCM */
    AUDIO_STREAM_TRANSFER_G726  = 3, /* For G726  */
    AUDIO_STREAM_TRANSFER_BUTT
} AUDIO_STREAM_TRANSFER_E;

typedef enum hiAUDIO_SAMPLE_RATE_E
{
    AUDIO_SAMPLE_RATE_8     = 0,   /* 8K Sample rate     */
    AUDIO_SAMPLE_RATE_11025 = 1,   /* 11.025K Sample rate*/
    AUDIO_SAMPLE_RATE_16    = 2,   /* 16K Sample rate    */
    AUDIO_SAMPLE_RATE_22050 = 3,   /* 22.050K Sample rate*/
    AUDIO_SAMPLE_RATE_24    = 4,   /* 24K Sample rate    */
    AUDIO_SAMPLE_RATE_32    = 5,   /* 32K Sample rate    */
    AUDIO_SAMPLE_RATE_441   = 6,   /* 44.1K Sample rate  */
    AUDIO_SAMPLE_RATE_48    = 7,   /* 48K Sample rate    */
    AUDIO_SAMPLE_RATE_64    = 8,   /* 64K Sample rate    */
    AUDIO_SAMPLE_RATE_882   = 9,   /* 88.2K Sample rate  */
    AUDIO_SAMPLE_RATE_96    = 10,  /* 96K Sample rate    */
    AUDIO_SAMPLE_RATE_1764  = 11,  /* 176.4K Sample rate */
    AUDIO_SAMPLE_RATE_192   = 12,  /* 192K Sample rate   */
    AUDIO_SAMPLE_RATE_BUTT
} AUDIO_SAMPLE_RATE_E;

typedef enum hiAUDIO_BIT_WIDTH_E
{
    AUDIO_BIT_WIDTH_8   = 0,   /* Bit width is 8 bits   */
    AUDIO_BIT_WIDTH_16  = 1,   /* Bit width is 16 bits  */
    AUDIO_BIT_WIDTH_BUTT
}AUDIO_BIT_WIDTH_E;

typedef enum hiAUDIO_MODE_E
{
    AUDIO_MODE_PCM        = 0,   /* PCM Mode        */
    AUDIO_MODE_I2S_MASTER = 1,   /* I2S Master Mode */
    AUDIO_MODE_I2S_SLAVE  = 2,   /* I2S Slave Mode  */
    AUDIO_MODE_BUTT    
} AUDIO_MODE_E;

/* The package lenght type of Gxx and ADPCM encoder */
typedef enum hiVOICE_PACAKGE_TYPE_E
{
    VOICE_PACAKGE_10MS = 0,     /* Voice encoder package is 10ms */
    VOICE_PACAKGE_20MS = 1,     /* Voice encoder package is 20ms */
    VOICE_PACAKGE_30MS = 2,     /* Voice encoder package is 30ms */
    
    VOICE_PACAKGE_BUTT
} VOICE_PACAKGE_TYPE_E;

/* Voice payload format, Now just support ASF and RTP. */
typedef enum hiVOICE_PAYLOAD_FORMAT_E
{
    VOICE_PAYLOAD_ASF = 0,
    VOICE_PAYLOAD_RTP = 1,
    
    VOICE_PAYLOAD_BUTT
} VOICE_PAYLOAD_FORMAT_E;

/* The package type of amr */
typedef enum hiAMR_PACKAGE_TYPE_E
{
    AMR_PACKAGE_MMS = 0,  /* Using for file saving        */
    AMR_PACKAGE_IF1 = 1,  /* Using for wireless translate */
    AMR_PACKAGE_IF2 = 2,  /* Using for wireless translate */
    
    AMR_PACKAGE_BUTT
} AMR_PACKAGE_TYPE_E;

/* The bit rate type of amr */
typedef enum hiAMR_MODE_E
{
    AMR_MODE_MR475 = 0,     /* AMR bit rate as  4.75k */
    AMR_MODE_MR515,         /* AMR bit rate as  5.15k */        
    AMR_MODE_MR59,          /* AMR bit rate as  5.90k */
    AMR_MODE_MR67,          /* AMR bit rate as  6.70k */
    AMR_MODE_MR74,          /* AMR bit rate as  7.40k */
    AMR_MODE_MR795,         /* AMR bit rate as  7.95k */
    AMR_MODE_MR102,         /* AMR bit rate as 10.20k */
    AMR_MODE_MR122,         /* AMR bit rate as 12.20k */
    AMR_MODE_BUTT
} AMR_MODE_E;

/* The bit rate type of G726 */
typedef enum hiVOICE_BITRATE_E
{
    VOICE_BITRATE_16KBPS = 0,  /* G726 bit rate as 16Kbps */
    VOICE_BITRATE_24KBPS = 1,  /* G726 bit rate as 24Kbps */
    VOICE_BITRATE_32KBPS = 2,  /* G726 bit rate as 32Kbps */
    VOICE_BITRATE_40KBPS = 3,  /* G726 bit rate as 40Kbps */
    VOICE_BITRATE_BUTT
} VOICE_BITRATE_E;

typedef enum hiVIDEO_NORM_E
{
    VIDEO_ENCODING_MODE_PAL=0,
    VIDEO_ENCODING_MODE_NTSC,
    VIDEO_ENCODING_MODE_AUTO,
    VIDEO_ENCODING_MODE_BUTT
} VIDEO_NORM_E;

typedef enum hiVIDEO_CONTROL_MODE_E
{   VIDEO_CONTROL_MODE_SLAVER=0,
    VIDEO_CONTROL_MODE_MASTER,
    VIDEO_CONTROL_MODE_BUTT
} VIDEO_CONTROL_MODE_E;

typedef enum hiPIXEL_FORMAT_E
{   PIXEL_FORMAT_RGB_1BPP=0,
    PIXEL_FORMAT_RGB_2BPP,
    PIXEL_FORMAT_RGB_4BPP,
    PIXEL_FORMAT_RGB_8BPP,
    PIXEL_FORMAT_RGB_444,
    PIXEL_FORMAT_RGB_4444,
    PIXEL_FORMAT_RGB_555,
    PIXEL_FORMAT_RGB_565,
    PIXEL_FORMAT_RGB_1555,
    
    /*  9 reserved */
    PIXEL_FORMAT_RGB_888=0xA,
    PIXEL_FORMAT_RGB_8888,
    PIXEL_FORMAT_RGB_PLANAR_888,
    PIXEL_FORMAT_RGB_BAYER,
    PIXEL_FORMAT_YUV_A422,
    PIXEL_FORMAT_YUV_A444,
    PIXEL_FORMAT_YUV_PLANAR_422,
    PIXEL_FORMAT_YUV_PLANAR_420,
    PIXEL_FORMAT_YUV_PLANAR_444,
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    PIXEL_FORMAT_YUV_SEMIPLANAR_444,
    PIXEL_FORMAT_UYVY_PACKAGE_422,
    PIXEL_FORMAT_YCbCr_PLANAR,

    /* for VOU1.0 Main Layer */
    PIXEL_FORMAT_YUV_0 = 0,         /* yuv422 cosited, no up scaling      */
    PIXEL_FORMAT_YUV_1,             /* yuv422 interspersed, no up scaling */
    PIXEL_FORMAT_YUV_2,             /* yuv422 cosited, 1:2 output         */
    PIXEL_FORMAT_YUV_3,             /* yuv422 interspersed, 1:2 output    */
    PIXEL_FORMAT_YUV_4,             /* yuv420 cosited, no up scaling      */
    PIXEL_FORMAT_YUV_5,             /* yuv420 interspersed, no up scaling */
    PIXEL_FORMAT_YUV_6,             /* yuv420 cosited, 1:2 output         */
    PIXEL_FORMAT_YUV_7,             /* yuv420 interspersed, 1:2 output    */
    PIXEL_FORMAT_YUV_8,             /* yuv444,yuv->rgb                    */
    PIXEL_FORMAT_YUV_9,             /* yuv444, 1:2 output, yuv->rgb       */
    PIXEL_FORMAT_YUV_10,
    PIXEL_FORMAT_YUV_11,
    PIXEL_FORMAT_YUV_12,
    PIXEL_FORMAT_YUV_13,
    PIXEL_FORMAT_YUV_14,
    PIXEL_FORMAT_YUV_15,

    /* for HC */
    PIXEL_FORMAT_2COLOR = 0,
    PIXEL_FORMAT_4COLOR,
    PIXEL_FORMAT_3COLOR,
    PIXEL_FORMAT_RESERVED,

    /* for ovlayer type */
    PIXEL_FORMAT_YUV422 = 0,
    PIXEL_FORMAT_YUV444,
    PIXEL_FORMAT_RGB555,
    PIXEL_FORMAT_RGB888, 
    PIXEL_FORMAT_BUTT   
} PIXEL_FORMAT_E;

typedef struct hiRECT_S
{
    HI_U32   x;
    HI_U32   y;
    HI_U32   width;
    HI_U32   height;
}RECT_S, *PRECT_S;

/* The audio channel attribute */
typedef struct hiAUDIO_CH_ATTR_S
{   
    AUDIO_SAMPLE_RATE_E  enSampleRate;  /* Sample rate      */
    AUDIO_BIT_WIDTH_E    enBitWidth;    /* Sample bit width */
    AUDIO_MODE_E         enMode;        /* Work mode        */
} AUDIO_CH_ATTR_S;

/* The raw data struct for audio */
typedef struct hiAUDIO_RAWDATA_S
{
    AUDIO_BIT_WIDTH_E    enBitDepth;    /* Sample bit width              */
    AUDIO_SAMPLE_RATE_E  enSampleRate;  /* Sample rate                   */
    HI_U64               u64TimeStamp;  /* The time stamp                */
    HI_U32               u32FrameNum;   /* The frame number              */
    HI_S16              *ps16Addr;      /* The buffer address            */
    HI_U32               u32Length;     /* The available data length     */
    HI_U32               u32BufLength;  /* The buffer length can be used */
} AUDIO_RAWDATA_S;

/* AAC quality define */
typedef enum hiAAC_QUALITY_E
{
    AAC_QUALITY_EXCELLENT   = 0, 
    AAC_QUALITY_HIGH        = 1, 
    AAC_QUALITY_MEDIUM      = 2, 
    AAC_QUALITY_LOW         = 3,
    AAC_QUALITY_BUTT
} AAC_QUALITY_E;

/* AAC attribute struct */
typedef struct hiAENC_AAC_CONFIG_S
{
    AUDIO_SAMPLE_RATE_E enSmpRate; /* AAC sample rate                      */   
    AUDIO_BIT_WIDTH_E   enBitWidth;/* AAC bit width                        */
    AAC_QUALITY_E  enQuality;      /* AAC encoder quality                  */
    HI_S32         s32BitRate;     /* encoder bit rate in bits/sec         */
    HI_S32         s32BandWidth;   /* targeted audio bandwidth in Hz (<20K)*/
    HI_S16         s16ChannelsIn;  /* number of channels on input (2)      */
    HI_S16         s16ChannelsOut; /* number of channels on output (2)     */
} AAC_CONFIG_S;

//--------------------------------------------
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif 

