/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ���������� � ����������� STM32F401RB
//

#ifndef ARM_STM32FXXX_TQFP64_CTLSTYLE_V1K_H_INCLUDED
#define ARM_STM32FXXX_TQFP64_CTLSTYLE_V1K_H_INCLUDED 1

	#define CTLSTYLE_SW2011ALL	1

	#if 0
		#define WITHAUTOTUNER	1	/* ���� ������� ���������� */
		#define SHORTSET7 1
		//#define FULLSET7 1
	#endif

	#define WITHUSEPLL		1	/* ������� PLL	*/
	//#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	#if 1
		// ��� ������� �������� ���������� ����������
		#define WITHCPUXTAL 8000000UL	/* �� ���������� ���������� ����� 8.000 ��� */
		#define REF1_DIV 4			// ref freq = 2.000 MHz

		#if 1
			// normal operation frequency
			#define REF1_MUL 168	// N ���������� � PLL1
			//#define PLL1_P 4	// �������� ����� SYSTEM CLOCK MUX � PLL1
			//#define PLL1_Q 7	// �������� �� ����� 48 ���
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_2WS
		#endif

	#else
		// ������������ �� ����������� RC ���������� 16 ���
		#define REF1_DIV 8			// ref freq = 2.000 MHz

		#if 1
			// normal operation frequency
			#define REF1_MUL 84	// N ���������� � PLL1
			//#define PLL1_P 4	// �������� ����� SYSTEM CLOCK MUX � PLL1
			//#define PLL1_Q 7	// �������� �� ����� 48 ���
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_2WS
		#endif

	#endif

	/* ������ ������������ - ����� ������������������ */
	#if 1

		#define FQMODEL_45_IF6000_UHF144	1
		//#define FQMODEL_45_IF8868_UHF144	1
		// ���������������� �������� ������������.
		#define BANDSELSTYLERE_SW20XX_UHF 1
		//#define BANDSELSTYLERE_RX3QSP 1	/* ����������� ������� ����������� �������� �� ������� RX3QSP */
		//#define BANDSELSTYLERE_R3PAV 1        /* 8 ����������� ��� ��� 12� � 10� ���������� ��� ����������� 74HCT238 ftp://shodtech.net/SW2012/%D0%A4%D0%9D%D0%A7/ */

	#else
		// ��� ������� ������������� ����� ����������� � ����������� � ����� ���������������.
		#define FQMODEL_TRX8M		1	// ������ ������ �� 8 ���, 6 ���, 5.5 ��� � ������
		#define DIRECT_27M0_X1		1	/* Board hardware configuration */
		//#define DIRECT_25M0_X1		1	/* Board hardware configuration */
		//#define DIRECT_50M0_X8		1	/* Board hardware configuration */
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
		#define IF3_MODEL IF3_TYPE_8868
	#endif

	#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
	#define CTLREGSTYLE_SW2016VHF	1

	#define DSTYLE_UR3LMZMOD	1
	//#define	FONTSTYLE_ITALIC	1	//
	// --- �������� ��������, ������������� ��� ������ ����

	// +++ �������� ��������, ������������� ��� ���� �� ATMega
	// --- �������� ��������, ������������� ��� ���� �� ATMega

	// +++ ������ �������� ������������ ������ �� ����������
	//#define KEYBSTYLE_SW2013RDX	1	// 8-�� ��������� ����������
	#define KEYBSTYLE_SW2016RDX	1	// 8-�� ��������� ����������
	//#define KEYBSTYLE_SW2013RDX_LTIYUR	1		// 12-�� ��������� ���������� LTIYUR (check KI_COUNT) 
	//#define KEYBSTYLE_SW2013RDX_UT7LC	1	// 12-�� ��������� ���������� UT7LC (check KI_COUNT) 
	// --- ������ �������� ������������ ������ �� ����������

	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_HARD_SPI	1	/* LCD over SPI line */
	//#define LCDMODE_LTDC	1		/* Use STM32F4xxx with LCD-TFT Controller (LTDC) */
	//#define LCDMODE_LTDC_L8	1	/* ������������ 8 ��� �� ������� ������������� ������. ����� - 16 ���. */
	//#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2, �������� ������ � LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
	//#define LCDMODE_WH1604	1	/* ��� ������������ ���������� 16*4 */
	//#define LCDMODE_WH2004	1	/* ��� ������������ ���������� 20*4 */
	//#define LCDMODE_RDX0077	1	/* ��������� 128*64 � ������������ UC1601.  */
	//#define LCDMODE_RDX0154	1	/* ��������� 132*64 � ������������ UC1601. */
	//#define LCDMODE_RDX0120	1	/* ��������� 64*32 � ������������ UC1601.  */
	#define LCDMODE_RDT065	1	/* ��������� 132*64 � ������������ UC1601, XMIRROR & TOPDOWN */
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
	//#define LCDMODE_L2F50_TOPDOWN	1	/* ��������� ����������� � ������ LCDMODE_L2F50 */
	//#define LCDMODE_S1D13781_NHWACCEL 1	/* �������������� ���������� ������������ EPSON S1D13781 ��� ������ ������� */
	//#define LCDMODE_S1D13781	1	/* ���������� 480*272 � ������������ Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - ����������� ����������� */
	//#define LCDMODE_ILI9225	1	/* ��������� 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 � ������������ ILI9225� */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - ����������� ����������� (��� ������� ����� �� ������) */
	//#define LCDMODE_UC1608	1		/* ��������� 240*128 � ������������ UC1608.- ����������� */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7735	1	/* ��������� 160*128 � ������������ Sitronix ST7735 - TFT ������ 160 * 128 HY-1.8-SPI */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7565S	1	/* ��������� WO12864C2-TFH# 128*64 � ������������ Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	//#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T � ������������ ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI8961	1	/* 320 * 240, RGB, ILI8961 */
	//#define LCDMODE_ILI8961_TOPDOWN	1
	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	//#define ENCRES_DEFAULT ENCRES_128
	#define ENCRES_DEFAULT ENCRES_64
	//#define ENCRES_DEFAULT ENCRES_24
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	#define ENCODER_REVERSE	1	/* ������ � ������� ������� */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */

	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHPBT		1	/* ������������ PBT */
	#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	#define WITHSWRMTR	1	/* ���������� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� �������� ��� */
	#define WITHPWRLIN	1	/* ��������� �������� �������� ���������� ���������� � �� �������� */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	#define WITHSHOWSWRPWR 1	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
	#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */
	#define WITHSUBTONES	1	/* ����������� ������������ ������� ��� �������� NFM */
	//#define WITHPOTWPM		1	/* for UY5UM ������������ ����������� �������� �������� � ��������� �������������� */
	//#define WITHANTSELECT	1	// for UY5UM ���������� ������������� ������

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */
	#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������

	//#define WITHWFM	1			/* ������������ WFM */
	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define DEFPREAMPSTATE 	0	/* ��� �� ��������� ������� (1) ��� �������� (0) */

	//#define HARDWARE_IGNORENONVRAM	1		// ������� �� ������ ��� ��� �������� NVRAM
	/* ��� �� ������ �������� � ������ ������������ � ����������� */
	#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// ��� �� ��� ������������� FM25040A - 5 �����, 512 ����
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM
	//#define NVRAM_TYPE NVRAM_TYPE_BKPSRAM	// ������� ������ � ���������� ��������
	//#define HARDWARE_IGNORENONVRAM	1		// ������� �� ������ ��� ��� �������� NVRAM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16		// demo board with atxmega128a4u - 25LC160A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */

	/* Board hardware configuration */
	//#define PLL1_TYPE PLL_TYPE_SI5351A
	//#define PLL1_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	//#define PLL2_TYPE PLL_TYPE_SI5351A
	//#define PLL2_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9852
	//#define DDS1_TYPE DDS_TYPE_AD9851
	//#define DDS2_TYPE DDS_TYPE_AD9834
	//#define DDS2_TYPE DDS_TYPE_AD9851
	//#define PLL2_TYPE PLL_TYPE_CMX992
	//#define DDS3_TYPE DDS_TYPE_AD9834
	//#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx/STM32F7xx internal RTC peripherial */
	#define WITHSI5351AREPLACE 1

	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define PLL1_TYPE PLL_TYPE_LM7001
	//#define targetpll1 SPI_CSEL1

	//#define WITHSI5351AREPLACE 1

	//#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */

	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */
	#define DDS3_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS3 */

	///#define DDS1_TYPE DDS_TYPE_AD9834
	///#define targetdds1 SPI_CSEL1 	/* DDS1 - LO1 output - �������� AD9834 (Vasiliy, Ufa). */
	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	//#define targetdds3 SPI_CSEL1 	/* DDS2 - LO2 output - RU6NFK with FQMODEL_10M7_500K */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	#define targetext1 SPI_CSEL2 	/* external devices control */
	#define targetctl1 SPI_CSEL3 	/* control register */
	#define targetnvram SPI_CSEL4  	/* serial nvram */
	#define targetlcd targetext1
	#define targetuc1608 targetext1

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
#if WITHTEMPSENSOR
	TEMPIX = 16,		// ADC1->CR2 |= ADC_CR2_TSVREFE;	// ��� ������
#endif
#if WITHVOLTLEVEL 
	VOLTSOURCE = 4,		// ������� ����� �������� ����������, ��� ���
#endif /* WITHVOLTLEVEL */
#if WITHBARS
	SMETERIX = 0,	// S-meter
#endif /* WITHBARS */
#if WITHVOX
	VOXIX = 2, AVOXIX = 1,	// VOX
#endif /* WITHVOX */
#if WITHPOTPBT
	POTPBT = 2,
#endif /* WITHPOTPBT */
#if WITHPOTIFSHIFT
	POTIFSHIFT = 1,
#endif /* WITHPOTIFSHIFT */
#if WITHSWRMTR
	PWRI = 4,
	FWD = 4, REF = 3,	// SWR-meter
#endif /* WITHSWRMTR */
	//KI0 = 5, KI1 = 6, KI2 = 7	// ����������
	KI0 = 6, KI1 = 7	// ����������
};

#define VOLTLEVEL_UPPER		43	// 4.3 kOhm - ������� �������� �������� ������� ����������
#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - ������ ��������

//#define KI_COUNT 3	// ���������� ������������ ��� ���������� ������ ���
//#define KI_LIST	KI2, KI1, KI0,	// �������������� ��� ������� �������������
#define KI_COUNT 2	// ���������� ������������ ��� ���������� ������ ���
#define KI_LIST	KI1, KI0,	// �������������� ��� ������� �������������

#endif /* ARM_STM32FXXX_TQFP64_CTLSTYLE_V1K_H_INCLUDED */
