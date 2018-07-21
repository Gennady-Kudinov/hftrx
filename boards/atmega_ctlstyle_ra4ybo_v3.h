/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ���������� ���������� RA4YBO V3
// "������������ � ��������" STM32F103C8T6, 2*HMC830, up-conversion to 64455 ��� RA4YBO
//

#ifndef ATMEGA_CTLSTYLE_RA4YBO_V3_H_INCLUDED
#define ATMEGA_CTLSTYLE_RA4YBO_V3_H_INCLUDED 1

	#define WITHUSEPLL		1	/* ������� PLL	*/

	// STM32F103C8T6 Medium-density performance line
	#if WITHCPUXTAL
		#define	REFINFREQ WITHCPUXTAL
		#define REF1_DIV 1
		#define REF1_MUL 9	// Up to 16 supported
	#else
		#define	REFINFREQ 8000000UL
		#define REF1_DIV 2
		#define REF1_MUL 8	// Up to 16 supported
	#endif

	/* ������ ������������ - ����� ������������������ */

	// --- �������� ��������, ������������� ��� ������ ����
	//#define CTLREGMODE_RA4YBO_V3	1
	#define CTLREGMODE_RA4YBO_V3A	1

	#define WITHPREAMPATT2_10DB		1	// ���������� ��� � ������������� ������������

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	#define	BOARD_AGCCODE_2		0x02
	#define	BOARD_AGCCODE_3		0x03

	#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_0
	#define	BOARD_AGCCODE_MED	BOARD_AGCCODE_1
	#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_2
	#define	BOARD_AGCCODE_LONG	BOARD_AGCCODE_3
	#define	BOARD_AGCCODE_OFF	0xFF

	#define WITHAGCMODE4STAGES	1	// 4 ��������. ���������� �� ������

	#define	FONTSTYLE_ITALIC	1	//
	#define DSTYLE_UR3LMZMOD	1


	// +++ �������� ��������, ������������� ��� ���� �� ATMega
	// --- �������� ��������, ������������� ��� ���� �� ATMega

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
	#define ENCODER_REVERSE	1	/* ������ � ������� ������� */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	#define WITHVOX		1	/* ������������ VOX */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	#define WITHSHOWSWRPWR 1	/* �� ������� ������������ ������������ SWR-meter � PWR-meter - ������� WITHSWRMTR */
	#define WITHSWRMTR	1	/* ���������� ��� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� ��������  */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHPBT		1	/* ������������ PBT */
	#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	#define WITHMODESETFULLNFM 1
	#define WITHWFM	1			/* ������������ WFM */
	#define WITHCURRLEVEL	1

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */

	#define	WITHAUTOTUNER	1	/* ���� ������� ���������� */
	//#define FULLSET8 1
	#define SHORTSET8 1
	#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������

	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define DEFPREAMPSTATE 	1	/* ��� �� ��������� ������� (1) ��� �������� (0) */
	
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

	#define LO1MODE_DIRECT		1

#if CTLREGMODE_RA4YBO_V3

	#define PLL1_TYPE PLL_TYPE_HMC830
	#define PLL1_FRACTIONAL_LENGTH	24	/* ���������� ��� � ������� ����� �������� */
	#define PLL2_TYPE PLL_TYPE_LMX2306
	//#define PLL2_TYPE PLL_TYPE_HMC830
	//#define PLL2_FRACTIONAL_LENGTH	24	/* ���������� ��� � ������� ����� �������� */

	#define REFERENCE_FREQ	50000000L
	#define	SYNTH_R1		1


#elif CTLREGMODE_RA4YBO_V3A

	#define PLL1_TYPE PLL_TYPE_SI570
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	#define DDS2_TYPE DDS_TYPE_AD9834
	#define PLL2_TYPE PLL_TYPE_LMX2306	/* �������� ��������� ���������� ��� ��������� ��������� ��������� */
	#define REFERENCE_FREQ	50000000UL	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */


#if 1
	// ������� ��������� ����� 25 ���
	// ������ ��� ������ 1-� �� 80.455
	// IF2=500 kHz
	// LO2=79995000
	#define LO2_PLL_N500	3203	// 79,955,067 Hz 
	#define LO2_PLL_R500	2003
	// IF2=455 kHz
	// LO2=80000000
	#define LO2_PLL_N455	3200
	#define LO2_PLL_R455	2000
#else
	// ������� ��������� ����� 50 ���
	// ������ ��� ������ 1-� �� 80.455
	// IF2=500 kHz
	#define LO2_PLL_N500	1593
	#define LO2_PLL_R500	998
	// IF2=455 kHz
	#define LO2_PLL_N455	1600
	#define LO2_PLL_R455	1000
#endif
	/* ���� � ��������� ���������� ADG714 */
	#define ADG714_OFF	0x00u	/* ��� ��������� */
	#define ADG714_S1D1 (0x01u << 0)
	#define ADG714_S2D2 (0x01u << 1)
	#define ADG714_S3D3 (0x01u << 2)
	#define ADG714_S4D4 (0x01u << 3)
	#define ADG714_S5D5 (0x01u << 4)
	#define ADG714_S6D6 (0x01u << 5)
	#define ADG714_S7D7 (0x01u << 6)
	#define ADG714_S8D8 (0x01u << 7)

	#define MAKEFLTCODE(in, out) ((in) * 256 + (out))	// in: ���������� IC1, out: ���������� IC2

	#define BOARD_FILTER_RX_WFM		MAKEFLTCODE(ADG714_OFF, ADG714_OFF)

	#define BOARD_FILTER500_RX_0P5	MAKEFLTCODE(ADG714_S1D1, ADG714_S1D1)		// emf-500-0.6-S

	#define BOARD_FILTER500_RX_3P1	MAKEFLTCODE(ADG714_S3D3, ADG714_S3D3)		// emf-500-3.1-N
	#define BOARD_FILTER500_TX_3P1	MAKEFLTCODE(ADG714_S4D4, ADG714_S4D4)		// emf-500-3.1-N

	#define BOARD_FILTER455_RX_3P0	MAKEFLTCODE(ADG714_S2D2, ADG714_S2D2)		// collins

	#define BOARD_FILTER455_RX_10P0	MAKEFLTCODE(ADG714_S6D6, ADG714_S6D6)		// piezo
	#define BOARD_FILTER455_TX_10P0	MAKEFLTCODE(ADG714_S5D5, ADG714_S5D5)		// piezo

	#define BOARD_FILTER455_RX_6P0	MAKEFLTCODE(ADG714_S7D7, ADG714_S7D7)		// piezo

	#define BOARD_FILTER455_RX_2P1	MAKEFLTCODE(ADG714_S8D8, ADG714_S8D8)		// piezo


	#define BOARD_FILTER_OFF MAKEFLTCODE(ADG714_OFF, ADG714_OFF)

#endif

	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS1_CLK_MUL	1		/* ���������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� ������� ������� ����� ������� � DDS1 */
	#define DDS3_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS3_CLK_MUL	1		/* ���������� ������� ������� ����� ������� � DDS1 */

	//#define DDS1_TYPE DDS_TYPE_AD9834

	//#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	//#define WITHDUALFLTR	1	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */
	#define WITHSAMEBFO	1		/* ������������� ����� �������� BFO ��� ����� � �������� */
	#define DDS2_TYPE DDS_TYPE_AD9834


	#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */

	/* ���������� ������� �� SPI ���� */
	#define targetdds2 SPI_CSEL_PA9 	/* DDS2 - LO3 output */
	#define targetpll1 SPI_CSEL_PA11 	/* PLL1 - HMC830 */ 
	#define targetpll2 SPI_CSEL_PA10 	/* PLL2 - HMC830 */ 
	#define targetctl1 SPI_CSEL_PA8 	/* control register */
	#define targetnvram SPI_CSEL_PA15  	/* serial nvram */
	#define targetlcd SPI_CSEL_PA12 	/* LCD over SPI line devices control */ 
	

	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	#define BOARD_DETECTOR_SSB 0x03
	#define BOARD_DETECTOR_AM 0x02
	#define BOARD_DETECTOR_FM 0x01
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x01	/* ������������ ����� ��� ������ TUNE (CWZ �� ��������) */

	#define BOARD_DETECTOR_MUTE 4

	//#define IF3_FMASK (IF3_FMASK_17P0 | IF3_FMASK_7P8 | IF3_FMASK_3P1 | IF3_FMASK_2P4 | IF3_FMASK_1P5 | IF3_FMASK_0P5 | IF3_FMASK_0P3)
	#define IF3_FMASK (IF3_FMASK_3P1 | IF3_FMASK_0P5 | IF3_FMASK_6P0)
	#define IF3_FHAVE IF3_FMASK
	#define IF3_FMASKTX	(IF3_FMASK_6P0)

	#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
	//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
	#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
	#define WITHELKEY	1
	#define WITHKBDENCODER 1	// ����������� ������� ��������
	#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
	#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */
	#define KEYBOARD_USE_ADC6_V1	1
	//#define KEYBOARD_USE_ADC6	1
	#define WITHDIRECTFREQENER	1	/* ������ ���� ������� � ���������� */

	#define WITHSPKMUTE		1	/* ���������� ����������� �������� */
	#define WITHANTSELECT	1
	// +++ ������ �������� ������������ ������ �� ����������
	#define KEYB_RA4YBO_V3	1	/*  */
	// --- ������ �������� ������������ ������ �� ����������
	// ���������� ������ ��� ����������.
	enum 
	{ 
	#if WITHBARS
		SMETERIX = 0,	// S-meter
	#endif
	#if WITHVOX
		VOXIX = 1, AVOXIX = 2,	// VOX
	#endif
	#if WITHSWRMTR
		FWD = 4, REF = 3,	// SWR-meter
		PWRI = 4, 
	#endif
	#if WITHCURRLEVEL
		PASENSEIX = 5,		// PA1 PA current sense - ACS712-30 chip
	#endif /* WITHCURRLEVEL */
		KI0 = 6, KI1 = 7, KI2 = 8, KI3 = 9	// ����������
	};
	#define KI_COUNT 4	// ���������� ������������ ��� ���������� ������ ���


	#define VOLTLEVEL_UPPER		47	// 4.7 kOhm - ������� �������� �������� ������� ����������
	#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - ������ ��������


	/* ������ �� - 64455, ������ - 500/455 ��� */

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	//#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO1_SIDE_F(freq) (((freq) < 64455000UL) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
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

	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 - "�������" */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 - AD9834 */

	#define	WFM_IF1	10700000ul

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define LO4_POWER2 0

	#define TUNE_BOTTOM 30000L		/* ������ ������� ��������� */
	#define TUNE_TOP 180000000L		/* ������� ������� ��������� */


#endif /* ATMEGA_CTLSTYLE_RA4YBO_V3_H_INCLUDED */
