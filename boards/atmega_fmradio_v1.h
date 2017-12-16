/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ����� �������
//
// ��� ���������� ����������� ������ ���� ��� ����������� .h 

#ifndef ATMEGA_FMRADIO_V1_H_INCLUDED
#define ATMEGA_FMRADIO_V1_H_INCLUDED 1

/*
#if CPU_FREQ != 10000000
	#error Set F_CPU right value in project file
#endif
*/
	/* ������ ������������ - ����� ������������������ */

	#define FQMODEL_FMRADIO		1	// 1-st IF=10.7 MHz
	
	// --- �������� ��������, ������������� ��� ������ ����
	//#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */

	//#define CTLREGMODE16	1		/* ����������� ������� 16 ���	*/
	//#define CTLREGMODE24	1		/* ����������� ������� 24 ����	*/
	#define CTLREGMODE24_V1	1		/* ����������� ������� 24 ���� - "�������" � "�������" */
	#define WITHAGCMODENONE 1
	//#define CTLREGMODE24_IGOR	1	/* ������� ��� ����� - ����������� ������� 24 ���� - "�������" � "�������" */
	//#define CTLREGMODE16_GEN500	1
	#define WITHATT2_6DB		1	// ���������� ������������� ������������ ��� ��������� ���

	//#define DSTYLE_UR3LMZMOD	1	// ������������ - ������������ ��������� ������ � ����������� UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// ������������ �������������� �����

	// +++ ������ �������� ������������ ������ �� ����������
	//#define KEYB_RA1AGO	1	/* ������������ ������ ��� RA1AGO (������� �������� � ���������� ��������) */
	#define KEYB_VERTICAL_REV	1	/* ������������ ������ ��� ���� "�������" � "�������" */
	//#define KEYB_VERTICAL	1	/* ������������ ������ ��� ���� "������" */
	//#define KEYBOARD_USE_ADC6	1	/* ����� ������ �� ������ ����� ADCx */
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
	//#define LCDMODE_S1D13781	1	/* ���������� 480*272 � ������������ Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - ����������� ����������� */
	//#define LCDMODE_ILI9225	1	/* ��������� 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 � ������������ ILI9225� */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - ����������� ����������� (��� ������� ����� �� ������) */
	#define LCDMODE_UC1608	1		/* ��������� 240*128 � ������������ UC1608.- ����������� */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7735	1	/* ��������� 160*128 � ������������ Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7565S	1	/* ��������� WO12864C2-TFH# 128*64 � ������������ Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	//#define WITHRFSG	1	/* �������� ���������� �� ������-�����������. */
	//#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	//#define WITHPBT		1	/* ������������ PBT */
	//#define WITHCAT		1	/* ������������ CAT */
	//#define WITHVOX		1	/* ������������ VOX */
	//#define WITHSWRMTR	1	/* ���������� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� �������� ��� */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHPOTWPM		1	/* ������������ ����������� �������� �������� � ��������� �������������� */
	//#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	//#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	//#define WITHDUALFLTR	1	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */
	//#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */
	#define WITHBCBANDS	1		/* � ������� ���������� ������������ ����������� ��������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHLO1LEVELADJ		1	/* �������� ���������� ������� (����������) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������

	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define DEFPREAMPSTATE 	1	/* ��� �� ��������� ������� (1) ��� �������� (0) */

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
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */

	/* Board hardware configuration */
	#define PLL1_TYPE PLL_TYPE_TSA6057
	//#define PLL1_TYPE PLL_TYPE_ADF4001

	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */
	#define DDS3_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS3 */
	#define HYBRID_PLL1_NEGATIVE 1	/* � ����� ���� ���� ������������� ��������� */

	#define HYBRID_NVFOS 1
	/* ���������� ������� �� SPI ���� */
	//#define targetdds1 SPI_CSEL0 	/* DDS1 */
	//#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	//#define targetdds3 SPI_CSEL2 	/* DDS3 - PBT output */
	#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. ��� ������� DDS ������� ���������� - ������ DDS */
	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */
#ifndef SPI_IOUPDATE_BIT 
	#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
#endif
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/

	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	#define BOARD_DETECTOR_MUTE 	(0x02)
	#define BOARD_DETECTOR_SSB 	(0x00)
	#define BOARD_DETECTOR_AM 	(0x01)
	#define BOARD_DETECTOR_FM 	(0x03)
	#define	BOARD_DETECTOR_WFM 0
	#define BOARD_DETECTOR_TUNE 0x00	/* ������������ ����� ��� ������ TUNE (CWZ �� ��������) */

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define	BOARD_FILTER_WFM	0


	#if FQMODEL_FMRADIO
		#define WITHMODESETFULLNFM 1
		#define IF3_FMASK (IF3_FMASK_120P0)
		#define IF3_FMASKTX IF3_FMASK
		#define IF3_FHAVE IF3_FMASK
	#elif 1
		//#define WITHMODESETSMART 1	/* � ����������� �� ������� �������� �������, ������������ ������ � WITHFIXEDBFO */
		#define WITHMODESETFULL 1
		//#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* ������������ WFM */
		/* ��� ��������� � ������ ������������ ������� */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		/* ��� ��������� � ������ ������������ ������� ��� �������� */
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* �������, ��� ������� ����� ������� HAVE */
		#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	#else
		//#define WITHMODESETSMART 1	/* � ����������� �� ������� �������� �������, ������������ ������ � WITHFIXEDBFO */
		#define WITHMODESETFULL 1
		//#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* ������������ WFM */
		#define IF3_FMASK (IF3_FMASK_2P7 | IF3_FMASK_1P8 | IF3_FMASK_0P5)
		#define IF3_FMASKTX (IF3_FMASK_2P7)
		#define IF3_FHAVE (IF3_FMASK_2P7 | IF3_FMASK_0P5)
	#endif

	//#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
	//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
	//#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
	//#define WITHELKEY	1
	//#define WITHKBDENCODER 1	// ����������� ������� ��������
	#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
	#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */

	// ���������� ������ ��� ����������.
	enum 
	{ 
	#if WITHVOLTLEVEL 
		VOLTSOURCE = 4,		// ������� ����� �������� ����������, ��� ���
	#endif /* WITHVOLTLEVEL */

	#if WITHBARS
		SMETERIX = 0,	// S-meter
	#endif
	#if WITHVOX
		VOXIX = 2, AVOXIX = 1,	// VOX
	#endif
	#if WITHSWRMTR
		FWD = 4, REF = 3,	// SWR-meter
	#endif
		KI0 = 5, KI1 = 6, KI2 = 7	// ����������
	};

	#define KI_COUNT 3	// ���������� ������������ ��� ���������� ������ ���


#endif /* ATMEGA_FMRADIO_V1_H_INCLUDED */
