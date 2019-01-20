/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef ATMEGA_CTLSTYLE_TS850_V1_H_INCLUDED
#define ATMEGA_CTLSTYLE_TS850_V1_H_INCLUDED 1

	#define WITHUSEPLL		1	/* ������� PLL	*/
	//#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	/* ������ ������������ - ����� ������������������ */

	#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */


	#define	FONTSTYLE_ITALIC	1	//
	// --- �������� ��������, ������������� ��� ������ ����

	#define CTLREGMODE24_V1	1		/* ����������� ������� - "�������" � "�������" */
	#define WITHATT1PRE1		1	// ���������� ��� � ��������� ������������

	// +++ �������� ��������, ������������� ��� ���� �� ATMega
	// --- �������� ��������, ������������� ��� ���� �� ATMega

	#define KEYB_VERTICAL_REV	1	/* ������������ ������ ��� ���� "�������" � "�������" */
	// +++ ������ �������� ������������ ������ �� ����������
	//#define KEYB_RA1AGG	1	/* ������������ ������ ��� �������, ���������� RA1AGG - ��� ������ �� ������ ������� ������ ���� �� ������ � ���� ���. */
	//#define KEYB_UA1CEI	1	/* ������������ ������ ��� UA1CEI */
	// --- ������ �������� ������������ ������ �� ����������

	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_SPI	1		/* LCD WEHxxxx over SPI line */
	//#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2, �������� ������ � LCDMODE_HARD_SPI */
	#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
	//#define LCDMODE_WH2004	1	/* ��� ������������ ���������� 20*4 */
	//#define LCDMODE_RDX0077	1	/* ��������� 128*64 � ������������ UC1601.  */
	//#define LCDMODE_RDX0154	1	/* ��������� 132*64 � ������������ UC1601. */
	//#define LCDMODE_RDX0120	1	/* ��������� 64*32 � ������������ UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - ����������� ����������� */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - ��� ����������� ��������������� */
	//#define LCDMODE_TIC154	1	/* ��������� 133*65 � ������������ PCF8535 */
	//#define LCDMODE_TIC218	1	/* ��������� 133*65 � ������������ PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* ���������� PCF8535 - ����������� ����������� */
	//#define LCDMODE_LS020 	1	/* ��������� 176*132 Sharp LS020B8UD06 � ������������ LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - ����������� ����������� */
	//#define LCDMODE_LPH88		1	/* ��������� 176*132 LPH8836-2 � ������������ Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - ����������� ����������� */
	//#define LCDMODE_L2F50	1	/* ��������� 176*132 � ������������ Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* ��������� ���������� � � ������ LCDMODE_L2F50 */
	//#define LCDMODE_S1D13781_NHWACCEL 1	/* �������������� ���������� ������������ EPSON S1D13781 ��� ������ ������� */
	//#define LCDMODE_S1D13781	1	/* ���������� 480*272 � ������������ Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - ����������� ����������� */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	//#define LCDMODE_ILI9225	1	/* ��������� 220*176 � ������������ ILI9225� */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - ����������� ����������� (��� ������� ����� �� ������) */
	//#define LCDMODE_UC1608	1		/* ��������� 240*128 � ������������ UC1608.- ����������� */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - ����������� ����������� (��� ������� ������) */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	//#define ENCRES_DEFAULT ENCRES_128
	#define ENCRES_DEFAULT ENCRES_100
	//#define ENCRES_DEFAULT ENCRES_24

	#define WITHNESTEDINTERRUPTS	1	/* ������������ ��� ������� real-time �����. */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHPBT		1	/* ������������ PBT */
	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHVOX		1	/* ������������ VOX */
	#define WITHSWRMTR	1	/* ���������� ��� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� ��������  */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */
	#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������

	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define DEFPREAMPSTATE 	1	/* ��� �� ��������� ������� (1) ��� �������� (0) */
	
	#define WITHAGCMODEONOFF 1	/* ����� ������� ��� ���� ������ ��������-��������� */
	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01
	#define WITHMODESETFULL 1
	/* ��� �� ������ �������� � ������ ������������ � ����������� */
	#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// ��� �� ��� ������������� FM25040A - 5 �����, 512 ����
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section

	/* �����, �� ������� ��� DDS - ��� AD9834 */
	//#define FTW_RESOLUTION 28	/* ����������� FTW ���������� DDS */
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */
	//#define FTW_RESOLUTION 31	/* ����������� FTW ���������� DDS - ATINY2313 */


	/* Board hardware configuration */
	#define DDS1_TYPE DDS_TYPE_AD9951
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define DDS1_TYPE DDS_TYPE_AD9834
	#define DDS2_TYPE DDS_TYPE_AD9834
	#define DDS3_TYPE DDS_TYPE_AD9834
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define PLL2_TYPE PLL_TYPE_ADF4001	/* �������� ��������� ���������� ��� ��������� ��������� ��������� */

	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */

	//#define DDS1_CLK_DIV	7		/* �������� ������� ������� ����� ������� � DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */
	#define DDS3_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS3 */

	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	#define targetdds3 SPI_CSEL2 	/* DDS3 - PBT output */
	//#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. ��� ������� DDS ������� ���������� - ������ DDS */
	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */ 
	#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/

	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	#define BOARD_DETECTOR_MUTE 	(0x02)
	#define BOARD_DETECTOR_SSB 	(0x00)
	#define BOARD_DETECTOR_AM 	(0x01)
	#define BOARD_DETECTOR_FM 	(0x03)
	#define BOARD_DETECTOR_TUNE 0x00	/* ������������ ����� ��� ������ TUNE (CWZ �� ��������) */

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define BOARD_FILTER_0P5		0x00	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_1P8		0x01	/* 1.8 kHz filter - �� ����� ������ ������ */
	#define BOARD_FILTER_2P7		0x02	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_3P1		0x03	/* 3.1 or 2.75 kHz filter */

	/* ��� ��������� � ������ ������������ ������� */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	/* ��� ��������� � ������ ������������ ������� ��� �������� */
	#define IF3_FMASKTX	(IF3_FMASK_3P1)
	/* �������, ��� ������� ����� ������� HAVE */
	#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)

	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */
	#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
	//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
	#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
	#define WITHELKEY	1
	#define WITHKBDENCODER 1	// ����������� ������� ��������
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
	#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */
	// ���������� ������ ��� ����������.
	enum 
	{ 
	#if WITHBARS
		SMETERIX = 0,	// S-meter
	#endif
	#if WITHVOX
		VOXIX = 2, AVOXIX = 1,	// VOX
	#endif
	#if WITHSWRMTR
		FWD = 4, REF = 3,	// SWR-meter
		PWRI = 4,
	#endif
		KI0 = 5, KI1 = 6, KI2 = 7	// ����������
	};

	#define KI_COUNT 3	// ���������� ������������ ��� ���������� ������ ���
	#define KI_LIST	KI2, KI1, KI0,	// �������������� ��� ������� �������������


	/* �������� ��������� �������������� ������� � ������ */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	//#define LO1_SIDE_F(freq) (((freq) < 73050000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

	// LO2 = 64.220 MHz
	// LO4 = 8.357 MHz

	#define DIRECT_50M0_X8	1	/* DDS LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */	

	#define IF3_MODEL IF3_TYPE_455

	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO2_PLL_R	REFERENCE_FREQ	
	#define LO2_PLL_N	64220000UL		

	#define	LO3_POWER2	0		/* 0 - ��� �������� ����� ���������� LO3 ����� ������� �� ��������� */

	#define LO3_PLL_R	REFERENCE_FREQ	
	#define LO3_PLL_N	8357000UL		


	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define LO4_POWER2 0

	#define TUNE_BOTTOM 30000L		/* ������ ������� ��������� */
	#define TUNE_TOP 56000000L		/* ������� ������� ��������� */
	//#define TUNE_TOP 150000000L		/* ������� ������� ��������� */

	#if (MODEL_DIRECT == 0)

		#define SYNTHLOWFREQ 	(73050000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(73050000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=73.050000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1099410      /* Full VCO range 73.080000..129.050000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=73.050000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1152762      /* Full VCO range 73.080000..129.050000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif



#endif /* ATMEGA_CTLSTYLE_TS850_V1_H_INCLUDED */
