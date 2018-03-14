/* $Id$ */
/* SW2016MINI - � ����������� � ���-������	*/
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

// �������� ��� ���������� SW2013C - mini "Color" - � ������� �������� � ����������� �������� ���������.

#ifndef ATMEGA_CTLSTYLE_V9CN6_H_INCLUDED
#define ATMEGA_CTLSTYLE_V9CN6_H_INCLUDED 1
	
	#if F_CPU != 8000000
		#error Set F_CPU right value in project file
	#endif
	
	#if 0
		#define WITHAUTOTUNER	1	/* ���� ������� ���������� */
		#define SHORTSET7 1
	#endif
	
	/* ������ ������������ - ����� ������������������ */
	#if 0
		#define FQMODEL_45_IF6000	1	// up to 32 MHz
		//#define FQMODEL_45_IF8868	1	// up to 32 MHz
	#else
		// ��� ������� ������������� ����� ����������� � ����������� � ����� ���������������.
		#define FQMODEL_TRX8M		1	// ������ ������ �� 8 ���, 6 ���, 5.5 ��� � ������
		#define DIRECT_20M0_X1		1	/* Board hardware configuration */
		//#define DIRECT_80M0_X5		1	/* Board hardware configuration */
		//#define DIRECT_60M0_X6		1	/* Board hardware configuration */
		//#define DIRECT_125M0_X3		1	/* Board hardware configuration */

		//#define BANDSELSTYLERE_LOCONV32M	1
		#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
		//#define BANDSELSTYLE_LADVABEST	1		/* Down-conversion with working band 1.6..56 MHz */
		// ����� ��
		//#define IF3_MODEL IF3_TYPE_9000
		//#define IF3_MODEL IF3_TYPE_8000
		//#define IF3_MODEL IF3_TYPE_6000
		#define IF3_MODEL IF3_TYPE_10700
	#endif

	#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
	
	#define CTLSTYLE_SW2012_MINI 1
	#define CTLREGSTYLE_SW2012CN	1	// ����������� �������� SW2012MINI c ������� �������� � ������������ �������� ��������
	//#define CTLREGSTYLE_SW2012CN_RN3ZOB 1 // � ������ ����������

	//#define DSTYLE_UR3LMZMOD	1
	//#define	FONTSTYLE_ITALIC	1	//
	// --- �������� ��������, ������������� ��� ������ ����

	// +++ �������� ��������, ������������� ��� ���� �� ATMega
	// --- �������� ��������, ������������� ��� ���� �� ATMega

	// +++ ������ �������� ������������ ������ �� ����������
	//#define KEYB_RA1AGG	1	/* ������������ ������ ��� �������, ���������� RA1AGG - ��� ������ �� ������ ������� ������ ���� �� ������ � ���� ���. */
	//#define KEYB_UA1CEI	1	/* ������������ ������ ��� UA1CEI */
	#define KEYBSTYLE_RA4YBO_AM0	1
	// --- ������ �������� ������������ ������ �� ����������

	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2, �������� ������ � LCDMODE_HARD_SPI */
	#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
	//#define LCDMODE_WH2004	1	/* ��� ������������ ���������� 20*4 */
	//#define LCDMODE_RDX0077	1	/* ��������� 128*64 � ������������ UC1601.  */
	//#define LCDMODE_RDX0154	1	/* ��������� 132*64 � ������������ UC1601.  */
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
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	//#define ENCRES_DEFAULT ENCRES_128
	#define ENCRES_DEFAULT ENCRES_24
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	#define ENCODER_REVERSE	1	/* ������ � ������� ������� */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHPBT		1	/* ������������ PBT */
	//#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	//#define WITHCAT		1	/* ������������ CAT */
	//#define WITHVOX		1	/* ������������ VOX */
	#if 1
		#define WITHPWRMTR	1	/* ��������� �������� �������� ��� */
		#define WITHPWRLIN	1	/* ��������� �������� �������� ���������� ���������� � �� �������� */
	#else
		#define WITHSWRMTR	1	/* ���������� ��� */
		#define WITHPWRLIN	1	/* ��������� �������� �������� ���������� ���������� � �� �������� */
		#define WITHSHOWSWRPWR 1	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
	#endif
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHPOTWPM		1	/* ������������ ����������� �������� �������� � ��������� �������������� */
	//#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	//#define WITHBCBANDS	1		/* � ������� ���������� ������������ ����������� ��������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */

	//#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������

	//#define WITHWFM	1			/* ������������ WFM */
	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define DEFPREAMPSTATE 	0	/* ��� �� ��������� ������� (1) ��� �������� (0) */

	// ���������������� �������� ������������.
	#define BANDSELSTYLERE_SW20XX 1
	//#define BANDSELSTYLERE_RX3QSP 1	/* ����������� ������� ����������� �������� �� ������� RX3QSP */
	//#define BANDSELSTYLERE_R3PAV 1        /* 8 ����������� ��� ��� 12� � 10� ���������� ��� ����������� 74HCT238 ftp://shodtech.net/SW2012/%D0%A4%D0%9D%D0%A7/ */

	/* ��� �� ������ �������� � ������ ������������ � ����������� */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// ��� �� ��� ������������� FM25040A - 5 �����, 512 ����
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

	// ��� �����������
	#if 1
		// ������� ������ - �� �� Si5351A
		#define WITHSI5351AREPLACE 1
	#else
		// ������ ������
		#define DDS1_TYPE DDS_TYPE_AD9951
		//#define PLL1_TYPE PLL_TYPE_SI570
		//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
		//#define DDS2_TYPE DDS_TYPE_AD9835
		#define DDS2_TYPE DDS_TYPE_AD9834
	#endif

	/* Board hardware configuration */

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
	#define BOARD_FILTER_1P8		BOARD_FILTERCODE_1	/* 1.8 kHz filter - ���������� ������ ������� 0.5 */
	#define BOARD_FILTER_2P7		BOARD_FILTERCODE_0	/* 3.1 or 2.75 kHz filter */
	//#define BOARD_FILTER_WFM		2	/* 3.1 or 2.75 kHz filter */

	#define WITHMODESETMIXONLY3 1
	//#define WITHMODESETMIXONLY3AM 1

	#define IF3_FMASK (IF3_FMASK_2P7 | /*IF3_FMASK_1P8 | */IF3_FMASK_0P5)
	#define IF3_FMASKTX (IF3_FMASK_2P7)
	#define IF3_FHAVE (IF3_FMASK_2P7 | IF3_FMASK_0P5)

#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
//#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
//#define WITHELKEY	1
//#define WITHKBDENCODER 1	// ����������� ������� ��������
#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */
#define KEYBOARD_USE_ADC6_V1	1	/* ����� ������ �� ������ ����� ADCx */

// ���������� ������ ��� ����������.
enum 
{ 
	//VOLTSOURCE = 2, // ������� ����� �������� ����������, ��� ���
#if WITHBARS
	SMETERIX = 3,	// S-meter
	PWRI = 5,		// ��������� �������� �����������
	//FWD = 4, REF = 3,	// SWR-meter
#endif /* WITHBARS */
	KI0 = 6, // ����������
};

#define KI_COUNT 1	// ���������� ������������ ��� ���������� ������ ���

#define VOLTLEVEL_UPPER		43	// 4.3 kOhm - ������� �������� �������� ������� ����������
#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - ������ ��������


#endif /* ATMEGA_CTLSTYLE_V9CN6_H_INCLUDED */
