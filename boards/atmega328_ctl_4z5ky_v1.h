/* $Id$ */
/* board-specific CPU DIP28/TQFP32 attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
/* ���������� 4Z5KY � �������������� http://ur5yfv.ucoz.ua/forum/28-19-2 */
//
#ifndef ATMEGA328_CTL_4Z5KY_V1_H_INCLUDED
#define ATMEGA328_CTL_4Z5KY_V1_H_INCLUDED 1

#if F_CPU != 16000000
	#error Set F_CPU right value in project file
#endif
	/* ������ ������������ - ����� ������������������ */

#if 1
	/* ������ ��������� ���� - � ��������������� "����" */
	#define FQMODEL_TRX8M		1	// ������ ������ �� 8 ���, 6 ���, 5.5 ��� � ������

	// ����� ����������� ��������
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	//#define DIRECT_125M0_X1		1
	#define DIRECT_80M0_X1		1
	//#define DIRECT_50M0_X1		1
	//#define DIRECT_75M0_X1		1
	//#define DIRECT_66M0_X1		1
	//#define DIRECT_48M0_X1		1
	//#define DIRECT_44M0_X1		1

	#if 0
		// ����� ��
		#define IF3DF (9100000L - 8862500L)
		#define IF3_CUSTOM_CW_CENTER	(8864000L + IF3DF)
		#define IF3_CUSTOM_SSB_LOWER	(8862500L + IF3DF)
		#define IF3_CUSTOM_SSB_UPPER	(8865200L + IF3DF)
		#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

		#undef IF3_MODEL
		#define IF3_MODEL	IF3_TYPE_CUSTOM
	#else
		// ����� ��
		//#define IF3_MODEL IF3_TYPE_9000
		//#define IF3_MODEL IF3_TYPE_8000
		#define IF3_MODEL IF3_TYPE_8868
		//#define IF3_MODEL IF3_TYPE_5000
		//#define IF3_MODEL IF3_TYPE_6000
		//#define IF3_MODEL IF3_TYPE_8192
		//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
		//#define IF3_MODEL IF3_TYPE_5500
		//#define IF3_MODEL IF3_TYPE_9045 
		//#define IF3_MODEL IF3_TYPE_10700
	#endif

#elif 1
	/* ������ ��������� ���� - � ��������������� "����" */
	// ����� ��
	#define FQMODEL_TRX500K		1	// ������ (� �����������) �� 500 ���
	//#define FQMODEL_TRX455K		1	// ������ (� �����������) �� 455 ���
	// ����� ����������� ��������
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	//#define BANDSELSTYLERE_LOCONV15M_NLB	1	/* Down-conversion with working band 1.6..11 MHz */
	
	#define DIRECT_80M0_X1		1
	//#define DIRECT_50M0_X1		1
	//#define DIRECT_48M0_X1		1
	//#define DIRECT_44M0_X1		1
#else
	/* ������ �������������� */
	#define FQMODEL_DCTRX		1	// ������ ��������������
	// ����� ����������� ��������
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	#define DIRECT_80M0_X1		1
	//#define DIRECT_75M0_X1		1
	//#define DIRECT_50M0_X1		1
	//#define DIRECT_44M0_X1		1
	//#define DIRECT_48M0_X1		1
#endif
	
	// --- �������� ��������, ������������� ��� ������ ����
	#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */

	#define CTLREGMODE_4Z5KY_V1	1		/* ����������� ������� 16 ��� */
	//#define	CTLREGMODE8_UR5YFV	1	/* 8 ��� */


	#define WITHONEATTONEAMP	1	/* ������ ���� ��������� ����������� � ��� */
	#define WITHAGCMODENONE		1	/* �������� ��� �� ��������� */

	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	#define BOARD_DETECTOR_MUTE 	(0x02)
	#define BOARD_DETECTOR_SSB 	(0x00)
	#define BOARD_DETECTOR_AM 	(0x01)
	#define BOARD_DETECTOR_FM 	(0x03)
	#define BOARD_DETECTOR_TUNE 0x00	/* ������������ ����� ��� ������ TUNE (CWZ �� ��������) */

	#define BOARD_AGCCODE_OFF	0

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define BOARD_FILTER_0P5		0x01	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		0x00	/* 3.1 or 2.75 kHz filter */

	//#define DSTYLE_UR3LMZMOD	1	// ������������ - ������������ ��������� ������ � ����������� UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// ������������ �������������� �����

	// +++ ������ �������� ������������ ������ �� ����������
	
	#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
	//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
	#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
	#define WITHELKEY	1
	//#define WITHKBDENCODER 1	// ����������� ������� ��������
	#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
	#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ����� ������ */
	#define KEYBOARD_USE_ADC6	1	/* ����� ������ �� ������ ����� ADCx */
	#define KEYBOARD_6BTN	1	/* ������ ��� 4Z5KY */
	// --- ������ �������� ������������ ������ �� ����������

	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2, �������� ������ � LCDMODE_HARD_SPI */
	#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
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
	//#define LCDMODE_UC1608	1		/* ��������� 240*128 � ������������ UC1608.- ����������� */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7735	1	/* ��������� 160*128 � ������������ Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7565S	1	/* ��������� WO12864C2-TFH# 128*64 � ������������ Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������


	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	//#define WITHRFSG	1	/* �������� ���������� �� ������-�����������. */
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	//#define WITHPBT		1	/* ������������ PBT */
	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHVOX		1	/* ������������ VOX */
	//#define WITHSWRMTR	1	/* ���������� ��� */
	#define WITHPWRMTR	1	/* ��������� �������� �������� ��� */
	//#define WITHPWRLIN	1	/* ��������� �������� �������� ���������� ���������� � �� �������� */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHPOTWPM		1	/* ������������ ����������� �������� �������� � ��������� �������������� */
	//#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */
	#define WITHNOTCHONOFF		1	/* notch on/off */

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	//#define WITHDUALFLTR	1	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */
	#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */
	//#define WITHBCBANDS	1		/* � ������� ���������� ������������ ����������� ��������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHLO1LEVELADJ		1	/* �������� ���������� ������� (����������) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	//#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
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
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9851
	#define DDS1_TYPE DDS_TYPE_AD9834
	#define DDS1_AD9834_COMPARATOR_ON	1
	//#define DDS1_TYPE DDS_TYPE_AD9851
	//#define PLL2_TYPE PLL_TYPE_CMX992
	//#define DDS2_TYPE DDS_TYPE_AD9834
	//#define DDS3_TYPE DDS_TYPE_AD9834

	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	//#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */
	//#define DDS3_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS3 */

	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetctl1 SPI_CSEL1 	/* control register */
	#define targetlcd SPI_CSEL2 	/* SPI LCD */

	//#define WITHMODESETSMART 1	/* � ����������� �� ������� �������� �������, ������������ ������ � WITHFIXEDBFO */
	//#define WITHMODESETFULL 1
	//#define WITHMODESETMIXONLY 1	// Use only product detector
	#define WITHMODESETFULLNFM 1
	//#define WITHWFM	1			/* ������������ WFM */
	/* ��� ��������� � ������ ������������ ������� */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	/* ��� ��������� � ������ ������������ ������� ��� �������� */
	#define IF3_FMASKTX	(IF3_FMASK_3P1)
	/* �������, ��� ������� ����� ������� HAVE */
	#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)


// ���������� ������ ��� ����������.
enum 
{ 
	SMETERIX = 5,	// S-meter
	PWRI = 5,		// ��������� �������� �����������
	KI0 = 4	// ����������
};

#define KI_COUNT 1	// ���������� ������������ ��� ���������� ������ ���
#define KI_LIST	KI0,	// �������������� ��� ������� �������������


#endif /* ATMEGA328_CTL_4Z5KY_V1_H_INCLUDED */
