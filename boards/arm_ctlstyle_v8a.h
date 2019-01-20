/* arm_ctlstyle_v8a.h */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

// ������� "��������". up-conversion,  AD9951 + ADF4001, AD9834

#ifndef ARM_CTLSTYLE_V8A_H_INCLUDED
#define ARM_CTLSTYLE_V8A_H_INCLUDED 1
	/* ������ ������������ - ����� ������������������ */

#if 1
	/* ������ ��������� ���� - � ��������������� "������" */
	//#define FQMODEL_73050		1	// 1-st IF=73.050, 2-nd IF=0.455 MHz
	//#define FQMODEL_73050_IF0P5		1	// 1-st IF=73.050, 2-nd IF=0.5 MHz
	//#define FQMODEL_80455		1	// 1-st IF=80.455, 2-nd IF=0.455 MHz
	#define FQMODEL_64455		1	// 1-st IF=64.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455_IF0P5		1	// 1-st IF=64.455, 2-nd IF=0.5 MHz
	//#define FQMODEL_70455		1	// 1-st IF=70.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */

#else
	/* ������ ��������� ���� - � ��������������� "������" */
	//#define FQMODEL_45_IF8868	1	// SW2011
	//#define FQMODEL_45_IF0P5	1	// 1-st IF=45MHz, 2-nd IF=500 kHz
	#define FQMODEL_45_IF455	1	// 1-st IF=45MHz, 2-nd IF=455 kHz
	#define BANDSELSTYLERE_UPCONV36M	1	/* Up-conversion with working band 0.1 MHz..36 MHz */
	//#define BANDSELSTYLERE_UPCONV32M	1	/* Up-conversion with working band 0.1 MHz..32 MHz */
#endif

	#define	FONTSTYLE_ITALIC	1	//
	// --- �������� ��������, ������������� ��� ������ ����

	//#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */

	#define CTLREGMODE_V8A	1		/* "��������"	*/

	// +++ �������� ��������, ������������� ��� ���� �� ATMega
	// --- �������� ��������, ������������� ��� ���� �� ATMega

	// +++ ������ �������� ������������ ������ �� ����������
	//#define KEYB_RA1AGG	1	/* ������������ ������ ��� �������, ���������� RA1AGG - ��� ������ �� ������ ������� ������ ���� �� ������ � ���� ���. */
	//#define KEYB_RA1AGO	1	/* ������������ ������ ��� RA1AGO (������� �������� � ���������� ��������) */
	//#define KEYB_UA1CEI	1	/* ������������ ������ ��� UA1CEI */
	//#define KEYB_VERTICAL	1	/* ������������ ������ ��� ���� "������" */
	#define KEYB_VERTICAL_REV	1	/* ������������ ������ ��� ���� "�������" � "�������" */
	// --- ������ �������� ������������ ������ �� ����������

	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
	#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2, �������� ������ � LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1604	1	/* ��� ������������ ���������� 16*4 */
	//#define LCDMODE_WH2004	1	/* ��� ������������ ���������� 20*4 */
	//#define LCDMODE_SPI	1		/* LCD WE2002 over SPI line - ��������� � LCDMODE_WHxxxxx */
	//#define LCDMODE_RDX0120	1	/* ��������� 64*32 � ������������ UC1601. */
	//#define LCDMODE_RDX0077	1	/* ��������� 128*64 � ������������ UC1601.  */
	//#define LCDMODE_RDX0154	1	/* ��������� 132*64 � ������������ UC1601. */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - ����������� ����������� */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - ��� ����������� ��������������� */
	//#define LCDMODE_TIC218	1	/* ��������� 133*65 � ������������ PCF8535 (���������� ������ � LCDMODE_UC1601) */
	//#define LCDMODE_TIC154	1	/* ��������� 133*65 � ������������ PCF8535 (���������� ������ � LCDMODE_UC1601 � LCDMODE_TIC218) */
	//#define LCDMODE_LS020 	1	/* ��������� 176*132 Sharp LS020B8UD06 � ������������ LR38826 */
	//#define LCDMODE_LPH88		1	/* ��������� 176*132 LPH8836-2 � ������������ Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - ����������� ����������� */
	//#define LCDMODE_S1D13781_NHWACCEL 1	/* �������������� ���������� ������������ EPSON S1D13781 ��� ������ ������� */
	//#define LCDMODE_S1D13781	1	/* ���������� 480*272 � ������������ Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - ����������� ����������� */
	//#define LCDMODE_ILI9225	1	/* ��������� 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 � ������������ ILI9225� */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - ����������� ����������� (��� ������� ����� �� ������) */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	//#define LCDMODE_ILI9320_TOPDOWN	1	/* LCDMODE_ILI9320 - ����������� ����������� (��� �������� � ������� ������) */
	//#define LCDMODE_UC1608	1		/* ��������� 240*128 � ������������ UC1608.- ����������� */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7735	1	/* ��������� 160*128 � ������������ Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7565S	1	/* ��������� WO12864C2-TFH# 128*64 � ������������ Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI9163	1	/* ��������� LPH9157-2 176*132 � ������������ ILITEK ILI9163 - ����� ������� ������, � ������ �����. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - ����������� ����������� (��� ������� ������, ����� ������� �����) */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */
	#define WITHNESTEDINTERRUPTS	1	/* ������������ ��� ������� real-time �����. */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	//#define WITHLFM		1	/* LFM MODE */
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	#define WITHVOX		1	/* ������������ VOX */

	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	#define WITHSWRMTR	1	/* ���������� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� �������� ��� */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHPOTWPM		1	/* ������������ ����������� �������� �������� � ��������� �������������� */
	//#define WITHTEMPSENSOR	1	/* ����������� ������ � ������� ����������� */
	//#define WITHVOLTSENSOR	1	/* ����������� ������ � ���������� ������� */
	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������


	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define WITHMODESETFULLNFM 1	/* ������������ FM */
	//#define WITHWFM	1			/* ������������ WFM */
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

	//#define FTW_RESOLUTION 28	/* ����������� FTW ���������� DDS */
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */
	//#define FTW_RESOLUTION 31	/* ����������� FTW ���������� DDS - ATINY2313 */


	/* Board hardware configuration */
	#define DDS1_TYPE DDS_TYPE_AD9951
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define DDS1_TYPE DDS_TYPE_AD9834
	#define DDS2_TYPE DDS_TYPE_AD9834
	#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define PLL2_TYPE PLL_TYPE_ADF4001	/* �������� ��������� ���������� ��� ��������� ��������� ��������� */

	//#define DDS1_CLK_DIV	7		/* �������� ������� ������� ����� ������� � DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */

	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. ��� ������� DDS ������� ���������� - ������ DDS */
	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */ 
	#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */

	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	#define BOARD_DETECTOR_MUTE 	(0x02)
	#define BOARD_DETECTOR_SSB 	(0x00)
	#define BOARD_DETECTOR_AM 	(0x01)
	#define BOARD_DETECTOR_FM 	(0x03)
	#define BOARD_DETECTOR_TUNE 0x00	/* ������������ ����� ��� ������ TUNE (CWZ �� ��������) */

	/* ���� �������� ������ ��, ���������� (����� ��������) �� �����������. */
	#define BOARD_FILTER_6P0			(0x01)	/* 6 kHz filter */
	#define BOARD_FILTER_9P0			(0x02)	/* only 1-st IF filter used (6, 8 or 15 khz width) */
	#define BOARD_FILTER_15P0		(0x02)	/* only 1-st IF filter used (6, 8 or 15 khz width) */
	#define BOARD_FILTER_3P1			(0x04)	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_0P5			(0x08)	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_0P3			(0x08)	/* 0.5 or 0.3 kHz filter */

	//#define WITHMODESETSMART 1	/* � ����������� �� ������� �������� �������, ������������ ������ � WITHFIXEDBFO */
	#if FQMODEL_64455_IF0P5 || FQMODEL_73050_IF0P5
		//#define WITHMODESETFULL 1
		#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* ������������ WFM */
		#define IF3_FMASK (IF3_FMASK_9P0 | IF3_FMASK_6P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FHAVE (IF3_FMASK_9P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#elif FQMODEL_64455 || FQMODEL_73050
		//#define WITHMODESETFULL 1
		#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* ������������ WFM */
		#define IF3_FMASK (IF3_FMASK_9P0 | IF3_FMASK_6P0  | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FHAVE (IF3_FMASK_9P0 | IF3_FMASK_6P0  | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#else
		//#define WITHMODESETFULL 1
		#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* ������������ WFM */
		#define IF3_FMASK (IF3_FMASK_15P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FHAVE (IF3_FMASK_15P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#endif

#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
#define WITHELKEY	1
#define WITHKBDENCODER 1	// ����������� ������� ��������
#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */

#if CPUSTYLE_STM32F
		// ���������� ������ ��� ����������.
		enum 
		{ 
		#if WITHTEMPSENSOR
			TEMPIX = 16,		// ADC1->CR2 |= ADC_CR2_TSVREFE;	// ��� ������
		#endif
		#if WITHVOLTSENSOR
			VOLTSIX = 17,		// ADC1->CR2 |= ADC_CR2_TSVREFE;	// ��� ������
		#endif
		#if WITHVOX
			VOXIX = 14, AVOXIX = 14,	// VOX
		#endif
		#if WITHBARS
			SMETERIX = 14,	// S-meter
		#endif
		#if WITHSWRMTR
			PWRI = 14,
			FWD = 14, REF = 14,	// SWR-meter
		#endif
		KI0 = 17, KI1 = 17, KI2 = 17	// ����������
		};
	#define KI_COUNT 3	// ���������� ������������ ��� ���������� ������ ���
	#define KI_LIST	KI2, KI1, KI0,	// �������������� ��� ������� �������������

#elif CPUSTYLE_AT91SAM7S || CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
		// ���������� ������ ��� ����������.
		enum 
		{ 
		#if WITHVOX
			VOXIX = 2, AVOXIX = 3,	// VOX
		#endif
		#if WITHBARS
			SMETERIX = 4,	// S-meter
		#endif
		#if WITHSWRMTR
			PWRI = 0,
			FWD = 0, REF = 1,	// SWR-meter
		#endif
			KI0 = 5, KI1 = 6, KI2 = 7,	// ����������
		};
	#define KI_COUNT 3	// ���������� ������������ ��� ���������� ������ ���
	#define KI_LIST	KI2, KI1, KI0,	// �������������� ��� ������� �������������

#endif


#endif /* ARM_CTLSTYLE_V8A_H_INCLUDED */
