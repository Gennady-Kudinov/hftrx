/* $Id$ */
// ATMega: DU VENT SI570+AD9834 (66MHz), LS020, ATMega32 - ��� �������� ����������� ������� - atmega_cpustyle_v1a.h, atmega_ctlstyle_v1a.h - ������ �� sw2011
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef ATMEGA_CTLSTYLE_V1A_H_INCLUDED
#define ATMEGA_CTLSTYLE_V1A_H_INCLUDED 1

	#define FQMODEL_TRX8M	1	// ������ �� 8 ���
	//#define WITHCLASSICSIDE	1	/* ������� ������� ���������� ������� �� ������� ������� */
	//#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	// ����� ��
	//#define IF3_MODEL IF3_TYPE_9000
	#define IF3_MODEL IF3_TYPE_8868
	//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
	//#define IF3_MODEL IF3_TYPE_5500
	//#define IF3_MODEL IF3_TYPE_9045
	#define DIRECT_66M0_X1		1	/* Board hardware configuration */
	
	#define DSTYLE_UR3LMZMOD	1	// ������������ - ������������ ��������� ������ � ����������� UR3LMZ
	//#define	FONTSTYLE_ITALIC	1	//
	// --- �������� ��������, ������������� ��� ������ ����
	#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */

	#define CTLREGMODE16	1		/* ����������� ������� 16 ��� - ��� SIMPLE ����������� �� AD8935	*/
	//#define CTLREGSTYLE_SW2011	1	// ����������� �������� SW2011RDX
	//#define KEYB_VERTICAL_REV	1	/* ������������ ������ ��� ���� "�������" � "�������" */
	#define KEYB_V8S_DK1VS	1	/* ������������ ��� ����������� DK1VS */
	#define WITHAGCMODEONOFF 1	/* ����� ������� ��� ���� ������ ��������-��������� */
	#define	BOARD_AGCCODE_ON	BOARD_AGCCODE_0
	#define	BOARD_AGCCODE_OFF	BOARD_AGCCODE_1


	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2, �������� ������ � LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
	//#define LCDMODE_WH2004	1	/* ��� ������������ ���������� 20*4 */
	//#define LCDMODE_RDX0077	1	/* ��������� 128*64 � ������������ UC1601.  */
	//#define LCDMODE_RDX0154	1	/* ��������� 132*64 � ������������ UC1601. */
	//#define LCDMODE_RDX0120	1	/* ��������� 64*32 � ������������ UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - ����������� ����������� */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - ��� ����������� ��������������� */
	//#define LCDMODE_TIC154	1	/* ��������� 133*65 � ������������ PCF8535 */
	//#define LCDMODE_TIC218	1	/* ��������� 133*65 � ������������ PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* ���������� PCF8535 - ����������� ����������� */
	#define LCDMODE_LS020 	1	/* ��������� 176*132 Sharp LS020B8UD06 � ������������ LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - ����������� ����������� */
	//#define LCDMODE_LPH88		1	/* ��������� 176*132 LPH8836-2 � ������������ Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - ����������� ����������� */
	//#define LCDMODE_S1D13781	1	/* ���������� 480*272 � ������������ Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - ����������� ����������� */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	//#define LCDMODE_ILI9225	1	/* ��������� 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 � ������������ ILI9225� */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - ����������� ����������� (��� ������� ����� �� ������) */
	//#define LCDMODE_ILI9163	1	/* ��������� LPH9157-2 176*132 � ������������ ILITEK ILI9163 - ����� ������� ������, � ������ �����. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - ����������� ����������� (��� ������� ������, ����� ������� �����) */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHPBT		1	/* ������������ PBT */
	#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	//#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	#define WITHVOX		1	/* ������������ VOX */
	#define WITHSHOWSWRPWR 1	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
	#define WITHSWRMTR	1	/* ���������� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� �������� ��� */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHPOTWPM		1	/* ������������ ����������� �������� �������� � ��������� �������������� */
	#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */
	//#define WITHDIRECTBANDS 1	/* ������ ������� � ���������� �� �������� �� ���������� */

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������

	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define DEFPREAMPSTATE 	1	/* ��� �� ��������� ������� (1) ��� �������� (0) */

	/* ��� �� ������ �������� � ������ ������������ � ����������� */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// ��� �� ��� ������������� FM25040A - 5 �����, 512 ����
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */

	/* Board hardware configuration */
	#define PLL1_TYPE PLL_TYPE_SI570
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9834
	
	//#define PLL1_TYPE PLL_TYPE_ADF4001

	//#define PLL1_TYPE PLL_TYPE_ADF4360
	//#define PLL2_TYPE PLL_TYPE_CMX992

	#define DDS2_TYPE DDS_TYPE_AD9834

	//#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	//#define WITHDUALFLTR	1	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */
	//#define WITHSAMEBFO	1		/* ������������� ����� �������� BFO ��� ����� � �������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */
	#define WITHBCBANDS	1		/* � ������� ���������� ������������ ����������� ��������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */

	//#define DDS3_TYPE DDS_TYPE_AD9834

	//#define DDS1_CLK_DIV	7		/* �������� ������� ������� ����� ������� � DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */

	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0	/* DDS1 - LO1 generator */
	#define targetdds2 SPI_CSEL1	/* DDS2 - LO3 generator */
	//#define targetpll1 SPI_CSEL2	/* ADF4001 after DDS1 - divide by r1 and scale to n1. ��� ������� DDS ������� ���������� - ������ DDS */
	#define targetlcd SPI_CSEL2	/* ��������� ���������� LS020 */
	//#define targetpll2 SPI_CSEL3	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetctl1 SPI_CSEL4	/* control register */
	#define targetnvram SPI_CSEL5 	/* serial nvram */

	#define WITHMODESETFULL 1
	//#define WITHMODESETSMART 1	/* � ����������� �� ������� �������� �������, ������������ ������ � WITHFIXEDBFO */

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define BOARD_FILTER_0P5		0x00	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_2P7		0x01	/* 3.1 or 2.75 kHz filter */

	/* ��� ��������� � ������ ������������ ������� */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_2P7)
	#define IF3_FMASKTX	(IF3_FMASK_2P7)
	#define IF3_FHAVE 	(IF3_FMASK_2P7 | IF3_FMASK_0P5)

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
	#if WITHBARS
		SMETERIX = 0,	// S-meter
	#endif
	#if WITHVOX
		VOXIX = 2, AVOXIX = 1,	// VOX
	#endif
	#if WITHSWRMTR
		PWRI = 4, 
		FWD = 4, REF = 3,	// SWR-meter
	#endif
		KI0 = 5, KI1 = 6, KI2 = 7	// ����������
	};

	#define KI_COUNT 3	// ���������� ������������ ��� ���������� ������ ���

#endif /* ATMEGA_CTLSTYLE_V1A_H_INCLUDED */
