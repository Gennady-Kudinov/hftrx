/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer*
* Copyright (C) 2013-2017 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name : cpg_iodefine.h
* $Rev: 29 $
* $Date:: 2016-12-20 17:40:26 +0900#$
* Description : Definition of I/O Register for RZ/A1L (V2.00l)
******************************************************************************/
#ifndef CPG_IODEFINE_H
#define CPG_IODEFINE_H
/* ->QAC 0639 : Over 127 members (C90) */
/* ->QAC 0857 : Over 1024 #define (C90) */
/* ->MISRA 18.4 : Pack unpack union */ /* ->SEC M1.6.2 */
/* ->SEC M1.10.1 : Not magic number */

#define CPG     (*(struct st_cpg     *)0xFCFE0010uL) /* CPG */


/* Start of channel array defines of CPG */

/* Channel array defines of CPG_FROM_STBCR3_ARRAY */
/*(Sample) value = CPG_FROM_STBCR3_ARRAY[ channel ]->STBCR3; */
#define CPG_FROM_STBCR3_ARRAY_COUNT  (10)
#define CPG_FROM_STBCR3_ARRAY_ADDRESS_LIST \
{   /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */ \
    &CPG_FROM_STBCR3, &CPG_FROM_STBCR4, &CPG_FROM_STBCR5, &CPG_FROM_STBCR6, &CPG_FROM_STBCR7, &CPG_FROM_STBCR8, &CPG_FROM_STBCR9, &CPG_FROM_STBCR10, \
    &CPG_FROM_STBCR11, &CPG_FROM_STBCR12 \
}   /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */ /* { } is for MISRA 19.4 */
#define CPG_FROM_STBCR3 (*(struct st_cpg_from_stbcr3 *)&CPG.STBCR3) /* CPG_FROM_STBCR3 */
#define CPG_FROM_STBCR4 (*(struct st_cpg_from_stbcr3 *)&CPG.STBCR4) /* CPG_FROM_STBCR4 */
#define CPG_FROM_STBCR5 (*(struct st_cpg_from_stbcr3 *)&CPG.STBCR5) /* CPG_FROM_STBCR5 */
#define CPG_FROM_STBCR6 (*(struct st_cpg_from_stbcr3 *)&CPG.STBCR6) /* CPG_FROM_STBCR6 */
#define CPG_FROM_STBCR7 (*(struct st_cpg_from_stbcr3 *)&CPG.STBCR7) /* CPG_FROM_STBCR7 */
#define CPG_FROM_STBCR8 (*(struct st_cpg_from_stbcr3 *)&CPG.STBCR8) /* CPG_FROM_STBCR8 */
#define CPG_FROM_STBCR9 (*(struct st_cpg_from_stbcr3 *)&CPG.STBCR9) /* CPG_FROM_STBCR9 */
#define CPG_FROM_STBCR10 (*(struct st_cpg_from_stbcr3 *)&CPG.STBCR10) /* CPG_FROM_STBCR10 */
#define CPG_FROM_STBCR11 (*(struct st_cpg_from_stbcr3 *)&CPG.STBCR11) /* CPG_FROM_STBCR11 */
#define CPG_FROM_STBCR12 (*(struct st_cpg_from_stbcr3 *)&CPG.STBCR12) /* CPG_FROM_STBCR12 */


/* Channel array defines of CPG_FROM_SYSCR1_ARRAY */
/*(Sample) value = CPG_FROM_SYSCR1_ARRAY[ channel ]->SYSCR1; */
#define CPG_FROM_SYSCR1_ARRAY_COUNT  (3)
#define CPG_FROM_SYSCR1_ARRAY_ADDRESS_LIST \
{   /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */ \
    &CPG_FROM_SYSCR1, &CPG_FROM_SYSCR2, &CPG_FROM_SYSCR3 \
}   /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */ /* { } is for MISRA 19.4 */
#define CPG_FROM_SYSCR1 (*(struct st_cpg_from_syscr1 *)&CPG.SYSCR1) /* CPG_FROM_SYSCR1 */
#define CPG_FROM_SYSCR2 (*(struct st_cpg_from_syscr1 *)&CPG.SYSCR2) /* CPG_FROM_SYSCR2 */
#define CPG_FROM_SYSCR3 (*(struct st_cpg_from_syscr1 *)&CPG.SYSCR3) /* CPG_FROM_SYSCR3 */

/* End of channel array defines of CPG */


#define CPGFRQCR (CPG.FRQCR)
#define CPGCPUSTS (CPG.CPUSTS)
#define CPGSTBCR1 (CPG.STBCR1)
#define CPGSTBCR2 (CPG.STBCR2)
#define CPGSTBREQ1 (CPG.STBREQ1)
#define CPGSTBREQ2 (CPG.STBREQ2)
#define CPGSTBACK1 (CPG.STBACK1)
#define CPGSTBACK2 (CPG.STBACK2)
#define CPGSYSCR1 (CPG.SYSCR1)
#define CPGSYSCR2 (CPG.SYSCR2)
#define CPGSYSCR3 (CPG.SYSCR3)
#define CPGSTBCR3 (CPG.STBCR3)
#define CPGSTBCR4 (CPG.STBCR4)
#define CPGSTBCR5 (CPG.STBCR5)
#define CPGSTBCR6 (CPG.STBCR6)
#define CPGSTBCR7 (CPG.STBCR7)
#define CPGSTBCR8 (CPG.STBCR8)
#define CPGSTBCR9 (CPG.STBCR9)
#define CPGSTBCR10 (CPG.STBCR10)
#define CPGSTBCR11 (CPG.STBCR11)
#define CPGSTBCR12 (CPG.STBCR12)
#define CPGSWRSTCR1 (CPG.SWRSTCR1)
#define CPGRRAMKP (CPG.RRAMKP)
#define CPGDSCTR (CPG.DSCTR)
#define CPGDSSSR (CPG.DSSSR)
#define CPGDSESR (CPG.DSESR)
#define CPGDSFR (CPG.DSFR)
#define CPGXTALCTR (CPG.XTALCTR)


typedef struct st_cpg
{
                                                           /* CPG              */
    volatile uint16_t FRQCR;                                  /*  FRQCR           */
    volatile uint8_t   dummy319[6];                            /*                  */
    volatile uint8_t   CPUSTS;                                 /*  CPUSTS          */
    volatile uint8_t   dummy320[7];                            /*                  */
    volatile uint8_t   STBCR1;                                 /*  STBCR1          */
    volatile uint8_t   dummy321[3];                            /*                  */
    volatile uint8_t   STBCR2;                                 /*  STBCR2          */
    volatile uint8_t   dummy322[11];                           /*                  */
    volatile uint8_t   STBREQ1;                                /*  STBREQ1         */
    volatile uint8_t   dummy323[3];                            /*                  */
    volatile uint8_t   STBREQ2;                                /*  STBREQ2         */
    volatile uint8_t   dummy324[11];                           /*                  */
    volatile uint8_t   STBACK1;                                /*  STBACK1         */
    volatile uint8_t   dummy325[3];                            /*                  */
    volatile uint8_t   STBACK2;                                /*  STBACK2         */
    volatile uint8_t   dummy326[955];                          /*                  */

/* start of struct st_cpg_from_syscr1 */
    volatile uint8_t   SYSCR1;                                 /*  SYSCR1          */
    volatile uint8_t   dummy327[3];                            /*                  */

/* end of struct st_cpg_from_syscr1 */

/* start of struct st_cpg_from_syscr1 */
    volatile uint8_t   SYSCR2;                                 /*  SYSCR2          */
    volatile uint8_t   dummy328[3];                            /*                  */

/* end of struct st_cpg_from_syscr1 */

/* start of struct st_cpg_from_syscr1 */
    volatile uint8_t   SYSCR3;                                 /*  SYSCR3          */
    volatile uint8_t   dummy3290[3];                           /*                  */

/* end of struct st_cpg_from_syscr1 */
    volatile uint8_t   dummy3291[20];                          /*                  */

/* start of struct st_cpg_from_stbcr3 */
    volatile uint8_t   STBCR3;                                 /*  STBCR3          */
    volatile uint8_t   dummy330[3];                            /*                  */

/* end of struct st_cpg_from_stbcr3 */

/* start of struct st_cpg_from_stbcr3 */
    volatile uint8_t   STBCR4;                                 /*  STBCR4          */
    volatile uint8_t   dummy331[3];                            /*                  */

/* end of struct st_cpg_from_stbcr3 */

/* start of struct st_cpg_from_stbcr3 */
    volatile uint8_t   STBCR5;                                 /*  STBCR5          */
    volatile uint8_t   dummy332[3];                            /*                  */

/* end of struct st_cpg_from_stbcr3 */

/* start of struct st_cpg_from_stbcr3 */
    volatile uint8_t   STBCR6;                                 /*  STBCR6          */
    volatile uint8_t   dummy333[3];                            /*                  */

/* end of struct st_cpg_from_stbcr3 */

/* start of struct st_cpg_from_stbcr3 */
    volatile uint8_t   STBCR7;                                 /*  STBCR7          */
    volatile uint8_t   dummy334[3];                            /*                  */

/* end of struct st_cpg_from_stbcr3 */

/* start of struct st_cpg_from_stbcr3 */
    volatile uint8_t   STBCR8;                                 /*  STBCR8          */
    volatile uint8_t   dummy335[3];                            /*                  */

/* end of struct st_cpg_from_stbcr3 */

/* start of struct st_cpg_from_stbcr3 */
    volatile uint8_t   STBCR9;                                 /*  STBCR9          */
    volatile uint8_t   dummy336[3];                            /*                  */

/* end of struct st_cpg_from_stbcr3 */

/* start of struct st_cpg_from_stbcr3 */
    volatile uint8_t   STBCR10;                                /*  STBCR10         */
    volatile uint8_t   dummy337[3];                            /*                  */

/* end of struct st_cpg_from_stbcr3 */

/* start of struct st_cpg_from_stbcr3 */
    volatile uint8_t   STBCR11;                                /*  STBCR11         */
    volatile uint8_t   dummy338[3];                            /*                  */

/* end of struct st_cpg_from_stbcr3 */

/* start of struct st_cpg_from_stbcr3 */
    volatile uint8_t   STBCR12;                                /*  STBCR12         */
    volatile uint8_t   dummy3390[3];                           /*                  */

/* end of struct st_cpg_from_stbcr3 */
    volatile uint8_t   dummy3391[24];                          /*                  */
    volatile uint8_t   SWRSTCR1;                               /*  SWRSTCR1        */
    volatile uint8_t   dummy340[70559];                        /*                  */
    volatile uint8_t   RRAMKP;                                 /*  RRAMKP          */
    volatile uint8_t   dummy341[1];                            /*                  */
    volatile uint8_t   DSCTR;                                  /*  DSCTR           */
    volatile uint8_t   dummy342[1];                            /*                  */
    volatile uint16_t DSSSR;                                  /*  DSSSR           */
    volatile uint16_t DSESR;                                  /*  DSESR           */
    volatile uint16_t DSFR;                                   /*  DSFR            */
    volatile uint8_t   dummy343[6];                            /*                  */
    volatile uint8_t   XTALCTR;                                /*  XTALCTR         */
} r_io_cpg_t;


typedef struct st_cpg_from_syscr1
{
 
    volatile uint8_t   SYSCR1;                                 /*  SYSCR1          */
    volatile uint8_t   dummy1[3];                              /*                  */
} r_io_cpg_from_syscr1_t;


typedef struct st_cpg_from_stbcr3
{
 
    volatile uint8_t   STBCR3;                                 /*  STBCR3          */
    volatile uint8_t   dummy1[3];                              /*                  */
} r_io_cpg_from_stbcr3_t;


/* Channel array defines of CPG (2)*/
#ifdef  DECLARE_CPG_FROM_STBCR3_ARRAY_CHANNELS
volatile struct st_cpg_from_stbcr3*  CPG_FROM_STBCR3_ARRAY[ CPG_FROM_STBCR3_ARRAY_COUNT ] =
    /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */
    CPG_FROM_STBCR3_ARRAY_ADDRESS_LIST;
    /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */
#endif  /* DECLARE_CPG_FROM_STBCR3_ARRAY_CHANNELS */

#ifdef  DECLARE_CPG_FROM_SYSCR1_ARRAY_CHANNELS
volatile struct st_cpg_from_syscr1*  CPG_FROM_SYSCR1_ARRAY[ CPG_FROM_SYSCR1_ARRAY_COUNT ] =
    /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */
    CPG_FROM_SYSCR1_ARRAY_ADDRESS_LIST;
    /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */
#endif  /* DECLARE_CPG_FROM_SYSCR1_ARRAY_CHANNELS */
/* End of channel array defines of CPG (2)*/


/* <-SEC M1.10.1 */
/* <-MISRA 18.4 */ /* <-SEC M1.6.2 */
/* <-QAC 0857 */
/* <-QAC 0639 */
#endif
