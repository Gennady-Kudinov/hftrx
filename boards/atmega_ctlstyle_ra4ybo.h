/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ���������� ���������� RA4YBO
//

#ifndef ATMEGA_CTLSTYLE_RA4YBO_H_INCLUDED
#define ATMEGA_CTLSTYLE_RA4YBO_H_INCLUDED 1

#if F_CPU != 10000000
	#error Set F_CPU right value in project file
#endif
#if ! defined(__AVR_ATmega1284P__)
	//#error Set CPU TYPE to ATMEGA1284P in project file
#endif

	/* ������ ������������ - ����� ������������������ */

	// --- �������� ��������, ������������� ��� ������ ����
	#define CTLREGMODE_RA4YBO	1		/* ����������� ������� 16 ���	*/

	#define WITHPREAMPATT2_10DB		1	// ���������� ��� � ������������� ������������

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	#define	BOARD_AGCCODE_2		0x02
	#define	BOARD_AGCCODE_3		0x03

	#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_0
	#define	BOARD_AGCCODE_MED	BOARD_AGCCODE_1
	#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_2
	#define	BOARD_AGCCODE_LONG	BOARD_AGCCODE_3
	#define WITHAGCMODE4STAGES	1	// 4 ��������. ���������� �� ������

	#define	FONTSTYLE_ITALIC	1	//


	// +++ �������� ��������, ������������� ��� ���� �� ATMega
	// --- �������� ��������, ������������� ��� ���� �� ATMega

	// +++ ������ �������� ������������ ������ �� ����������
	#define KEYB_RA4YBO	1	/*  */
	//#define KEYB_RA1AGG	1	/* ������������ ������ ��� �������, ���������� RA1AGG - ��� ������ �� ������ ������� ������ ���� �� ������ � ���� ���. */
	//#define KEYB_UA1CEI	1	/* ������������ ������ ��� UA1CEI */
	//#define KEYB_VERTICAL_REV	1	/* ������������ ������ ��� ���� "�������" � "�������" */
	//#define KEYBOARD_USE_ADC6	1	/* ����� ������ �� ������ ����� ADCx	*/
	// --- ������ �������� ������������ ������ �� ����������

	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2, �������� ������ � LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
	//#define LCDMODE_WH2004	1	/* ��� ������������ ���������� 20*4 */
	//#define LCDMODE_RDX0077	1	/* ��������� 128*64 � ������������ UC1601.  */
	//#define LCDMODE_RDX0154	1	/* ��������� 132*64 � ������������ UC1601. */
	//#define LCDMODE_RDX0120	1	/* ��������� 64*32 � ������������ UC1601.  */
	//#define LCDMODE_UC1601S_XMIRROR	1	/* ��������� 132*64 � ������������ UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - ����������� ����������� */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - ��� ����������� ��������������� */
	//#define LCDMODE_TIC154	1	/* ��������� 133*65 � ������������ PCF8535 */
	//#define LCDMODE_TIC218	1	/* ��������� 133*65 � ������������ PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* ���������� PCF8535 - ����������� ����������� */
	//#define LCDMODE_LS020 	1	/* ��������� 176*132 Sharp LS020B8UD06 � ������������ LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - ����������� ����������� */
	//#define LCDMODE_LPH88		1	/* ��������� 176*132 LPH8836-2 � ������������ Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - ����������� ����������� */
	//#define LCDMODE_ILI9163	1	/* ��������� LPH9157-2 176*132 � ������������ ILITEK ILI9163 - ����� ������� ������, � ������ �����. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - ����������� ����������� (��� ������� ������, ����� ������� �����) */
	//#define LCDMODE_L2F50	1	/* ��������� 176*132 � ������������ Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* ��������� ���������� � � ������ LCDMODE_L2F50 */
	#define LCDMODE_S1D13781	1	/* ���������� 480*272 � ������������ Epson S1D13781 */
	#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - ����������� ����������� */
	//#define LCDMODE_ILI9225	1	/* ��������� 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 � ������������ ILI9225� */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - ����������� ����������� (��� ������� ����� �� ������) */
	//#define LCDMODE_UC1608	1		/* ��������� 240*128 � ������������ UC1608.- ����������� */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7735	1	/* ��������� 160*128 � ������������ Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7565S	1	/* ��������� WO12864C2-TFH# 128*64 � ������������ Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	//#define ENCRES_DEFAULT ENCRES_24
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	#define WITHVOX		1	/* ������������ VOX */
	#define WITHSWRMTR	1	/* ���������� ��� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� ��������  */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHPBT		1	/* ������������ PBT */
	#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	#define WITHMODESETFULLNFM 1
	#define WITHWFM	1			/* ������������ WFM */


	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */
	#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������
	#define WITHSPKMUTE		1	/* ���������� ����������� �������� */

	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define DEFPREAMPSTATE 	1	/* ��� �� ��������� ������� (1) ��� �������� (0) */
	
	//#define REQUEST_FOR_RN4NAB 1	/* ����������� ������� ����������� �������� */

	/* ��� �� ������ �������� � ������ ������������ � ����������� */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// ��� �� ��� ������������� FM25040A - 5 �����, 512 ����
	#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section

	//#define FTW_RESOLUTION 28	/* ����������� FTW ���������� DDS */
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */
	//#define FTW_RESOLUTION 31	/* ����������� FTW ���������� DDS - ATINY2313 */

	//#define WITHLO1LEVELADJ		1	/* �������� ���������� ������� (����������) LO1 */
	/* Board hardware configuration */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9835
	//#define DDS1_TYPE DDS_TYPE_AD9851
	#define PLL1_TYPE PLL_TYPE_SI570
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_ATTINY2313

	//#define DDS1_TYPE DDS_TYPE_AD9834

	//#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	//#define WITHDUALFLTR	1	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */
	//#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	#define DDS2_TYPE DDS_TYPE_AD9834
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define PLL2_TYPE PLL_TYPE_ADF4001	/* �������� ��������� ���������� ��� ��������� ��������� ��������� */
	#define PLL2_TYPE PLL_TYPE_LMX2306	/* �������� ��������� ���������� ��� ��������� ��������� ��������� */

	//#define DDS1_CLK_DIV	7		/* �������� ������� ������� ����� ������� � DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */

	#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */

	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	//#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. ��� ������� DDS ������� ���������� - ������ DDS */
	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetpll2 SPI_CSEL2 	/* RA4YBO: LMX2306 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */ 
	//#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/
	

	#define BOARD_AGCCODE_OFF	0

	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	#define BOARD_DETECTOR_SSB 0x03
	#define BOARD_DETECTOR_AM 0x02
	#define BOARD_DETECTOR_FM 0x01
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x03	/* ������������ ����� ��� ������ TUNE (CWZ �� ��������) */

	#define BOARD_DETECTOR_MUTE BOARD_DETECTOR_WFM

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define	BOARD_FILTER_0P3	0x04
	#define	BOARD_FILTER_0P5	0x05
	#define	BOARD_FILTER_1P5	0x06
	#define	BOARD_FILTER_2P4	0x03
	#define	BOARD_FILTER_3P1	0x00
	#define	BOARD_FILTER_7P8	0x02
	#define	BOARD_FILTER_17P0	0x01
	#define	BOARD_FILTER_WFM	0x07

	#define IF3_FMASK (IF3_FMASK_17P0 | IF3_FMASK_7P8 | IF3_FMASK_3P1 | IF3_FMASK_2P4 | IF3_FMASK_1P5 | IF3_FMASK_0P5 | IF3_FMASK_0P3)
	#define IF3_FHAVE IF3_FMASK
	#define IF3_FMASKTX	(IF3_FMASK_3P1)

	#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
	//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
	#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
	#define WITHELKEY	1
	#define WITHKBDENCODER 1	// ����������� ������� ��������
	#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
	#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */

	// ���������� ������ ��� ����������.
	enum 
	{ 
	#if WITHVOX
		VOXIX = 2, AVOXIX = 1,	// VOX
	#endif
		SMETERIX = 0,	// S-meter
		PWRI = 4, 
		FWD = 4, REF = 3,	// SWR-meter
		KI0 = 5, KI1 = 6, KI2 = 7	// ����������
	};
	#define KI_COUNT 3	// ���������� ������������ ��� ���������� ������ ���
	#define KI_LIST	KI2, KI1, KI0,	// �������������� ��� ������� �������������

	/* ������ �� - 73050/70000 ���, ������ - 500 ��� */

	/* �������� ��������� �������������� ������� � ������ */
	//#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO1_SIDE_F(freq) (((freq) < 70000000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#if WITHIF4DSP
		// ����������� ��� DSP �� ��������� ��
		#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#else /* WITHIF4DSP */
		// ������� ����������� ��� ��������� �����
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
	#endif /* WITHIF4DSP */

	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	12000000UL	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 - "�������" */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 - AD9834 */

	#define IF3_MODEL IF3_TYPE_500
	#define	WFM_IF1	10700000ul

	#define LO2_FREQSTEP	50000UL				/* ������� ��������� 50 ��� */

	#define LO2_RX (72550000ul)
	#define LO2_TX (69500000ul)

	#define LO2_PLL_R_RX	(REFERENCE_FREQ / LO2_FREQSTEP)		/* �������� �������� ������� �� ������� ���������. */	
	#define LO2_PLL_R_TX	(REFERENCE_FREQ / LO2_FREQSTEP)		/* �������� �������� ������� �� ������� ���������. */	
	#define LO2_PLL_N_RX	(LO2_RX / LO2_FREQSTEP)
	#define LO2_PLL_N_TX	(LO2_TX / LO2_FREQSTEP)

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define LO4_POWER2 0

	#define TUNE_BOTTOM 30000L		/* ������ ������� ��������� */
	#define TUNE_TOP 160000000L		/* ������� ������� ��������� */


#endif /* ATMEGA_CTLSTYLE_RA4YBO_H_INCLUDED */
