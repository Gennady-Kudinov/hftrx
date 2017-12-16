/* $Id$ */
/* SW2013_V3	*/
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

// �������� ��� ���������� SW2013_V3, SW2013_V5 � ����������� RDX0154 � ������� FM

#ifndef ATMEGA_CTLSTYLE_V9FM_H_INCLUDED
#define ATMEGA_CTLSTYLE_V9FM_H_INCLUDED 1
	
	#define CTLSTYLE_SW2011ALL	1

	#if F_CPU != 8000000
		#error Set F_CPU right value in project file
	#endif

	//#define WITHAUTOTUNER	1	/* ���� ������� ���������� */
	//#define SHORTSET7 1
	//#define FULLSET7 1

	/* ������ ������������ - ����� ������������������ */
#if 1
	#define FQMODEL_45_IF6000_UHF144	1
	//#define FQMODEL_45_IF8868_UHF144	1
#else
	#define FQMODEL_TRX8M		1	// ������ ������ �� 8 ���, 6 ���, 5.5 ��� � ������
	//#define IF3_MODEL IF3_TYPE_8868
	#define IF3_MODEL IF3_TYPE_10700
	#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
#endif

	#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
	//#define CTLREGSTYLE_SW2013RDX	1
	#define CTLREGSTYLE_SW2014NFM	1	// ������ ���������� �������� ����� ����� �����/�������� NFM
	//#define CTLREGSTYLE_SW2013RDX_UY5UM	1	/* � ������������ �������� R-2R �� �������� ������ ������������ ������� */
	//#define CTLREGSTYLE_SW2013RDX_LTIYUR	1	// SW2013SF � ����������� RDX154

	#define DSTYLE_UR3LMZMOD	1
	//#define	FONTSTYLE_ITALIC	1	//
	// --- �������� ��������, ������������� ��� ������ ����

	// +++ �������� ��������, ������������� ��� ���� �� ATMega
	// --- �������� ��������, ������������� ��� ���� �� ATMega

	// +++ ������ �������� ������������ ������ �� ����������
	#define KEYBSTYLE_SW2013RDX	1	// 8-�� ��������� ����������
	//#define KEYBSTYLE_SW2013RDX_LTIYUR	1		// 12-�� ��������� ���������� LTIYUR (check KI_COUNT) 
	//#define KEYBSTYLE_SW2013RDX_UT7LC	1	// 12-�� ��������� ���������� UT7LC (check KI_COUNT) 
	// --- ������ �������� ������������ ������ �� ����������

	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2, �������� ������ � LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
	//#define LCDMODE_WH2004	1	/* ��� ������������ ���������� 20*4 */
	//#define LCDMODE_RDX0077	1	/* ��������� 128*64 � ������������ UC1601.  */
	#define LCDMODE_RDX0154	1	/* ��������� 132*64 � ������������ UC1601.  */
	//#define LCDMODE_UC1601S_XMIRROR	1	/* ��������� 132*64 � ������������ UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - ����������� ����������� */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - ��� ����������� ��������������� */
	//#define LCDMODE_RDX0120	1	/* ��������� 64*32 � ������������ UC1601.  */
	//#define LCDMODE_TIC154	1	/* ��������� 133*65 � ������������ PCF8535 */
	//#define LCDMODE_TIC218	1	/* ��������� 133*65 � ������������ PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* ���������� PCF8535 - ����������� ����������� */
	//#define LCDMODE_LS020 	1	/* ��������� 176*132 Sharp LS020B8UD06 � ������������ LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - ����������� ����������� */
	//#define LCDMODE_LPH88		1	/* ��������� 176*132 LPH8836-2 � ������������ Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - ����������� ����������� */
	//#define LCDMODE_S1D13781	1	/* ���������� 480*272 � ������������ Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - ����������� ����������� */
	//#define LCDMODE_ILI9225	1	/* ��������� 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 � ������������ ILI9225� */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - ����������� ����������� (��� ������� ����� �� ������) */
	//#define LCDMODE_UC1608	1		/* ��������� 240*128 � ������������ UC1608.- ����������� */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7735	1	/* ��������� 160*128 � ������������ Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7565S	1	/* ��������� WO12864C2-TFH# 128*64 � ������������ Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI9163	1	/* ��������� LPH9157-2 176*132 � ������������ ILITEK ILI9163 - ����� ������� ������, � ������ �����. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - ����������� ����������� (��� ������� ������, ����� ������� �����) */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	//#define LCDMODE_ILI9320_TOPDOWN	1	/* LCDMODE_ILI9320 - ����������� ����������� (��� �������� � ������� ������) */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	#define ENCODER_REVERSE	1	/* ������ � ������� ������� */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHPBT		1	/* ������������ PBT */
	#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	#define WITHSWRMTR	1	/* ���������� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� �������� ��� */
	#define WITHPWRLIN	1	/* ��������� �������� �������� ���������� ���������� � �� �������� */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	#define WITHSHOWSWRPWR 1	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
	#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHPOTWPM		1	/* ������������ ����������� �������� �������� � ��������� �������������� */
	#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */

	//#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������

	//#define WITHWFM	1			/* ������������ WFM */
	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define DEFPREAMPSTATE 	0	/* ��� �� ��������� ������� (1) ��� �������� (0) */

	// ���������������� �������� ������������.
	#define BANDSELSTYLERE_SW20XX_UHF 1
	//#define BANDSELSTYLERE_RX3QSP 1	/* ����������� ������� ����������� �������� �� ������� RX3QSP */
	//#define BANDSELSTYLERE_R3PAV 1        /* 8 ����������� ��� ��� 12� � 10� ���������� ��� ����������� 74HCT238 ftp://shodtech.net/SW2012/%D0%A4%D0%9D%D0%A7/ */


	/* ��� �� ������ �������� � ������ ������������ � ����������� */
	#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// ��� �� ��� ������������� FM25040A - 5 �����, 512 ����
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section

	/* ���������� ����������� SW2011 */
	//#define WITHLO1LEVELADJ		1	/* �������� ���������� ������� (����������) LO1 */

	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */

	/* Board hardware configuration */
	#define DDS1_TYPE DDS_TYPE_AD9951
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_ATTINY2313	1
	//#define DDS1_TYPE DDS_TYPE_AD9834	1
	//#define DDS2_TYPE DDS_TYPE_AD9835
	#define DDS2_TYPE DDS_TYPE_AD9834
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define PLL2_TYPE PLL_TYPE_ADF4001	/* �������� ��������� ���������� ��� ��������� ��������� ��������� */

	//#define DDS1_CLK_DIV	7		/* �������� ������� ������� ����� ������� � DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */

	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0	/* DDS1 - LO1 generator */
	#define targetdds2 SPI_CSEL1	/* DDS2 - LO3 generator */
	//#define targetpll1 SPI_CSEL2	/* ADF4001 after DDS1 - divide by r1 and scale to n1. ��� ������� DDS ������� ���������� - ������ DDS */
	#define targetlcd SPI_CSEL2 	/* LCD over SPI line devices control */
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/
	//#define targetpll2 SPI_CSEL3	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetctl1 SPI_CSEL4	/* control register */
	#define targetnvram SPI_CSEL5 	/* serial nvram */

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define BOARD_FILTER_0P5		BOARD_FILTERCODE_1	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_2P7		BOARD_FILTERCODE_0	/* 3.1 or 2.75 kHz filter */

	#define BOARD_FILTER_15P0 BOARD_FILTER_2P7
	#define BOARD_FILTER_15P0_NFM BOARD_FILTER_2P7

	#define WITHMODESETMIXONLY3NFM 1	/* � ������� ������ ������ ������������� ��������� ����������� NFM */
	#define WITHDEDICATEDNFM 1		/* ��� NFM ��������� ����� �� ����� �������� */

	#define IF3_FMASK (IF3_FMASK_2P7 | IF3_FMASK_0P5 | IF3_FMASK_15P0)
	#define IF3_FMASKTX (IF3_FMASK_2P7 | IF3_FMASK_15P0)
	#define IF3_FHAVE (IF3_FMASK_2P7 | IF3_FMASK_0P5 | IF3_FMASK_15P0)

#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
#define WITHELKEY	1
//#define WITHKBDENCODER 1	// ����������� ������� ��������
#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */


// ���������� ������ ��� ����������.
enum 
{ 
#if WITHBARS
	SMETERIX = 1,	// S-meter
	#if WITHPWRMTR
		PWRI = 0,		// ��������� �������� �����������
	#endif
	#if WITHSWRMTR
		PWRI = 0,		// ��������� �������� �����������
		FWD = 4, REF = 3,	// SWR-meter
	#endif
#endif /* WITHBARS */

#if WITHVOLTLEVEL 
	VOLTSOURCE = 2, // ������� ����� �������� ����������, ��� ���
#endif /* WITHVOLTLEVEL */


#if WITHPOTWPM
	POTWPM = 5,		// ������������ ���������� ��������� �������� � ���������
#endif
	 //KI0 = 5, KI1 = 6, KI2 = 7	// ���������� KEYBSTYLE_SW2013RDX_UT7LC, KEYBSTYLE_SW2013RDX_LTIYUR
	 KI0 = 6, KI1 = 7	// ����������
};

//#define KI_COUNT 3	// ���������� ������������ ��� ���������� ������ ���
#define KI_COUNT 2	// ���������� ������������ ��� ���������� ������ ���

#define VOLTLEVEL_UPPER		43	// 4.3 kOhm - ������� �������� �������� ������� ����������
#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - ������ ��������

#endif /* ATMEGA_CTLSTYLE_V9FM_H_INCLUDED */
