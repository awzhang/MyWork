#ifndef _YX_SYSTEM_H_
#define _YX_SYSTEM_H_

/****************************************************************
    DEFINE THE DATATYPE
****************************************************************/
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                   /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                   /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                  /* Signed   16 bit quantity                           */
typedef signed   short INT16S;                  /* Unsigned 32 bit quantity                           */
typedef unsigned int   INT32U;                  /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                  /* Signed   32 bit quantity                           */
typedef float          FP32;                    /* Single precision floating point                    */
typedef double         FP64;                    /* Double precision floating point                    */


/****************************************************************
    DEFINE SOME SWITCH
****************************************************************/

#ifndef  OFF
#define  OFF                    0
#endif

#ifndef  ON
#define  ON                     1
#endif

#ifndef  false
#define  false                  0
#endif

#ifndef  true
#define  true                   1
#endif

#ifndef  TRUE
#define  TRUE                   1
#endif

#ifndef   FALSE
#define   FALSE                 0
#endif


#ifndef   NULL 
#define   NULL                  ((void*)0)
#endif

#ifndef  PNULL
#define  PNULL                  ((void *)0)
#endif

#ifndef	 CR
#define  CR                      0x0D
#endif

#ifndef  LF
#define  LF                      0x0A
#endif

#ifndef	 CTRL_Z
#define  CTRL_Z                  0x1A
#endif

#ifndef  ESC
#define  ESC                     0x1B
#endif

#ifndef	 _SUCCESS
#define  _SUCCESS                0
#endif

#ifndef	 _FAILURE
#define  _FAILURE                1
#endif

#ifndef  _OVERTIME
#define  _OVERTIME               2
#endif

#ifndef  OPEN
#define  OPEN                    0xAA
#endif

#ifndef  CLOSE
#define  CLOSE                   0x55
#endif

#endif
