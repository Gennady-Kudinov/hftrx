/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

/* ����� STM32F4DISCOVERY � ����������� STM32F407VGT6	*/

#ifndef PION_DSP_CTLSTYLE_V1_H_INCLUDED
#define PION_DSP_CTLSTYLE_V1_H_INCLUDED 1

	#define WITHSAICLOCKFROMI2S	1	/* ���� SAI1 ����������� �� PLL I2S */
	//#define	WITHI2SCLOCKFROMPIN 1	// �������� ������� �� SPI2 (I2S) �������� � �������� ����������, � ��������� �������� ����� MCK ������ ����������
	//#define	WITHSAICLOCKFROMPIN 1	// �������� ������� �� SAI1 �������� � �������� ����������, � ��������� �������� ����� MCK ������ ����������

	#define WITHUSEPLL		1	/* ������� PLL	*/
	//#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	#if 0
		// ��� ������� �������� ���������� ����������
		#define WITHCPUXTAL 8000000UL	/* �� ���������� ���������� ����� 8.000 ��� */
		#define REF1_DIV 4			// ref freq = 2.000 MHz

		#if 0
			// overclocking
			#define REF1_MUL 192		// 2*192.000 MHz (192 <= PLLN <= 432) - overclocking, but USB 48 MHz clock generation
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_6WS	// overvlocking
		#else
			// normal operation frequency
			//#define REF1_MUL 180		// 2*180.000 MHz (192 <= PLLN <= 432)
			#define REF1_MUL 168	// for USB
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// �������� ��� ������ � ������� 5 WS for 168 MHz at 3.3 volt
		#endif

	#else
		// ������������ �� ����������� RC ���������� 16 ���
		#define REF1_DIV 8			// ref freq = 2.000 MHz

		#if 0
			// overclocking
			#define REF1_MUL 192		// 2*192.000 MHz (192 <= PLLN <= 432) - overclocking, but USB 48 MHz clock generation
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_6WS	// overvlocking
		#else
			// normal operation frequency
			//#define REF1_MUL 180		// 2*180.000 MHz (192 <= PLLN <= 432)
			#define REF1_MUL 168	// for USB
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// �������� ��� ������ � ������� 5 WS for 168 MHz at 3.3 volt
		#endif
	#endif

	#if WITHI2SCLOCKFROMPIN
		#define FPGADECIMATION 2560
		#define FPGADIVIDERATIO 5
		#define I2SCLOCKINFREQ (REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO)

		#define ARMI2SMCLK	(REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO / 2)
		#define ARMSAIMCLK	(REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO / 2)
	#else /* WITHI2SCLOCKFROMPIN */
		#define PLLI2SN_MUL 84		// 172032000UL
		#define SAIREF1_MUL 72		// 288 MHz
		// ������� ����������� �����������
		#define ARMI2SMCLK	(12288000UL)
		#define ARMSAIMCLK	(12288000UL)
	#endif /* WITHI2SCLOCKFROMPIN */

	/* ������ ������������ - ����� ������������������ */

#if 0
	/* ������ ��������� ���� - � ��������������� "������" */
	//#define FQMODEL_45_IF8868_UHF430	1	// SW2011
	//#define FQMODEL_73050		1	// 1-st IF=73.050, 2-nd IF=0.455 MHz
	//#define FQMODEL_73050_IF0P5		1	// 1-st IF=73.050, 2-nd IF=0.5 MHz
	//#define FQMODEL_80455		1	// 1-st IF=80.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455		1	// 1-st IF=64.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455_IF0P5		1	// 1-st IF=64.455, 2-nd IF=0.5 MHz
	//#define FQMODEL_70455		1	// 1-st IF=70.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	#define FQMODEL_60700_IF05	1	// 60.7 -> 10.7 -> 0.5
	//#define FQMODEL_60725_IF05	1	// 60.725 -> 10.725 -> 0.5
	//#define FQMODEL_60700_IF02	1	// 60.7 -> 10.7 -> 0.2
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */
	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */

#elif 0
	/* ������ ��������� ���� - � ��������������� "������" */
	//#define FQMODEL_45_IF8868	1	// SW2011
	//#define FQMODEL_45_IF0P5	1	// 1-st IF=45MHz, 2-nd IF=500 kHz
	//#define FQMODEL_45_IF455	1	// 1-st IF=45MHz, 2-nd IF=455 kHz
	//#define FQMODEL_45_IF10700_IF200		1	// 1st IF=45.0 MHz, 2nd IF=10.7 MHz, 3rd IF=0.2 MHz
	//#define FQMODEL_45_IF8868_IF200	1	// RA6LPO version
	//#define FQMODEL_58M11_IF8868	1	// for gena, DL6RF
	//#define FQMODEL_45_IF6000	1	// up to 32 MHz
	//#define FQMODEL_45_IF8868_UHF144	1	// SW2011
	//#define FQMODEL_45_IF8868	1	// SW2011
	//#define FQMODEL_45_IF0P5	1	// 1-st IF=45MHz, 2-nd IF=500 kHz
	//#define FQMODEL_45_IF455	1	// 1-st IF=45MHz, 2-nd IF=455 kHz
	//#define FQMODEL_45_IF128	1	// 1-st IF=45MHz, 2-nd IF=128 kHz
	//#define BANDSELSTYLERE_UPCONV36M	1	/* Up-conversion with working band 0.1 MHz..36 MHz */
	#define BANDSELSTYLERE_UPCONV32M	1	/* Up-conversion with working band 0.1 MHz..32 MHz */
	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */

#elif 0
	/* ������ ��������� ���� - � ��������������� "����" */
	#define FQMODEL_TRX500K		1	// ������ (� �����������) �� 500 ���
	//#define FQMODEL_TRX455K		1	// ������ (� ������������) �� 455 ���
	//#define WITHCLASSICSIDE	1	/* ������� ������� ���������� ������� �� ������� ������� */
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV15M	1	/* Down-conversion with working band .030..15 MHz */
	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
#elif 1
	/* ������ ��������� ���� - � ������ ���������������  */
	#define FQMODEL_DCTRX		1	// ������ ��������������
	//#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	#define DIRECT_100M0_X4		1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	#define WITHIF4DSP	1	// "�����"
	//#define DEFAULT_DSP_IF	12000
	//#define WITHEXTERNALDDSP		1	/* ������� ���������� ������� DSP ������. */
	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
#elif 0
	/* ������ ��������� ���� - � ��������������� "����" ���������� MAXLAB */
	#define FQMODEL_UW3DI		1	// ������ (� �����������) �� 500 ���
	//#define DIRECT_100M0_X4		1	/* Board hardware configuration */
	#define HYBRID_DDS_ATTINY2313 1
	//#define MAXLAB_7M2			1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	//#define MODEL_MAXLAB	1	/* PLL � ������ �������� ���������� ��������� ����� ��� */
	//#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
	#define MODEL_HYBRID 1	/* ��������� ���� ��������� */
#elif 1
	/* ������ ��������� ���� - � ��������������� "����" */
	#define FQMODEL_TRX8M		1	// ������ ������ �� 8 ���, 6 ���, 5.5 ��� � ������
	//#define WITHCLASSICSIDE	1	/* ������� ������� ���������� ������� �� ������� ������� */
	// ����� ��
	//#define IF3_MODEL IF3_TYPE_9000
	//#define IF3_MODEL IF3_TYPE_8000
	#define IF3_MODEL IF3_TYPE_8868
	//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
	//#define IF3_MODEL IF3_TYPE_5500
	//#define IF3_MODEL IF3_TYPE_9045
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	//#define DIRECT_80M0_X5		1	/* Board hardware configuration */
	//#define DIRECT_DDS_ATTINY2313 1
	//#define HYBRID_DDS_ATTINY2313 1
	#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	//#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
#elif 0
	/* ����������� ������, �������� � �� ��������. */
	//#define FQMODEL_GEN500	1	// CML evaluation board with CMX992 chip, 1-st IF = 45 MHz
	//#define FQMODEL_RFSG_SI570	1	// ������-��������� �� Si570 "B" grade
	//#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
#endif	
	// --- �������� ��������, ������������� ��� ������ ����


	#define CTLREGSTYLE_NOCTLREG	1		/* ����������� ������� �� SPI ����������� */
	#define WITHAGCMODENONE		1	/* �������� ��� �� ��������� */
	#define WITHPREAMPATT2_6DB		1	// ���������� ��� � ������������� ������������ � ����������� 0 - 6 - 12 - 18 dB */
	#define BOARD_AGCCODE_OFF 0x01
	//#define DSTYLE_UR3LMZMOD	1	// ������������ - ������������ ��������� ������ � ����������� UR3LMZ
	//#define	FONTSTYLE_ITALIC	1	// ������������ �������������� �����

	// +++ ������ �������� ������������ ������ �� ����������
	//#define KEYB_RA1AGO	1	/* ������������ ������ ��� RA1AGO (������� �������� � ���������� ��������) */
	#define KEYB_VERTICAL_REV	1	/* ������������ ������ ��� ���� "�������" � "�������" */
	//#define KEYB_VERTICAL_REV_TOPDOWN	1	/* ������������ ������ ��� ����¨������ ���� "�������" � "�������" */
	//#define KEYB_VERTICAL	1	/* ������������ ������ ��� ���� "������" */
	//#define KEYB_V8S_DK1VS	1	/* ������������ ��� ����������� DK1VS */
	//#define KEYB_VERTICAL_REV_RU6BK	1	/* ������������ ������ ��� ���� "�������" � "�������" */
	//#define KEYBOARD_USE_ADC6	1	/* ����� ������ �� ������ ����� ADCx */
	//#define KEYB_M0SERG	1	/* ������������ ������ ��� Serge Moisseyev */
	// --- ������ �������� ������������ ������ �� ����������

	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_HARD_SPI	1	/* LCD over SPI line */
	//#define LCDMODE_LTDC	1		/* Use STM32F4xxx with LCD-TFT Controller (LTDC) */
	//#define LCDMODE_LTDC_L8	1	/* ������������ 8 ��� �� ������� ������������� ������. ����� - 16 ���. */
	//#define LCDMODE_LTDC_L24	1	/* 32-bit �� ������� � ������ (3 �����) */
	//#define LCDMODE_LTDCSDRAMBUFF	1	/* ������������ ������� �������� SDRAM ��� �������� framebuffer */
	//#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2, �������� ������ � LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
	//#define LCDMODE_WH1604	1	/* ��� ������������ ���������� 16*4 */
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
	//#define LCDMODE_ST7735	1	/* ��������� 160*128 � ������������ Sitronix ST7735 - TFT ������ 160 * 128 HY-1.8-SPI */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7565S	1	/* ��������� WO12864C2-TFH# 128*64 � ������������ Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T � ������������ ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI8961	1	/* 320 * 240 HHT270C-8961-6A6, RGB, ILI8961, use LCDMODE_LTDC_L24 and LCDMODE_LTDCSDRAMBUFF */
	//#define LCDMODE_ILI8961_TOPDOWN	1
	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	//#define WITHDIRECTFREQENER	1
	#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
	//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
	#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
	#define WITHELKEY	1
	//#define WITHKBDENCODER 1	// ����������� ������� ��������
	//#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
	//#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */
	#define WITHENCODER	1	/* ��� ��������� ������� ������� ������� */
	#define WITHNESTEDINTERRUPTS	1	/* ������������ ��� ������� real-time �����. */
	#define WITHINTEGRATEDDSP		1	/* � ��������� �������� ������������� � ������ DSP �����. */
	#define WITHIF4DSP	1	// "�����"
	//#define WITHEXTERNALDDSP		1	/* ������� ���������� ������� DSP ������. */
	//#define WITHLOOPBACKTEST	1
	//#define DEFAULT_DSP_IF	12000
	//#define WITHDSPEXTDDC 1			/* ���������� ���������� ������� ����������� */
	//#define WITHDSPEXTFIR 1			/* ���������� ��������� �������������� ������� ����������� */
	#define WITHDSPLOCALFIR 1			/* ���������� ��������� �������������� ����������� */
	#define WITHRTS96 1		/* ������ ��������� ������������ �������� ��������� �� USB */

	#define WITHIFDACWIDTH	32		// 1 ��� ���� � 31 ��� ��������
	#define WITHIFADCWIDTH	32		// 1 ��� ���� � 31 ��� ��������
	#define WITHAFADCWIDTH	16		// 1 ��� ���� � 15 ��� ��������
	#define WITHAFDACWIDTH	16		// 1 ��� ���� � 15 ��� ��������

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	//#define WITHRFSG	1	/* �������� ���������� �� ������-�����������. */
	//#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	//#define WITHPBT		1	/* ������������ PBT (���� LO3 ����) */
	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	//#define WITHBEACON	1	/* ������������ ����� ����� */
	//#define WITHVOX		1	/* ������������ VOX */
	//#define WITHSHOWSWRPWR 1	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
	//#define WITHSWRMTR	1	/* ���������� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� �������� ��� */
	//#define WITHPWRLIN	1	/* ��������� �������� �������� ���������� ���������� � �� �������� */
	//#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHPOTWPM		1	/* ������������ ����������� �������� �������� � ��������� �������������� */
	#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	//#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	//#define WITHDUALFLTR	1	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */
	#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */
	#define WITHBCBANDS	1		/* � ������� ���������� ������������ ����������� ��������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHLO1LEVELADJ		1	/* �������� ���������� ������� (����������) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	//#define WITHTEMPSENSOR	1	/* ����������� ������ � ������� ����������� */
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
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	#define NVRAM_TYPE NVRAM_TYPE_BKPSRAM	// ������� ������ � ���������� ��������
	//#define HARDWARE_IGNORENONVRAM	1		// ������� �� ������ ��� ��� �������� NVRAM

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */

	/* Board hardware configuration */
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9851
	//#define DDS2_TYPE DDS_TYPE_AD9834
	//#define DDS2_TYPE DDS_TYPE_AD9851
	//#define PLL2_TYPE PLL_TYPE_CMX992
	//#define DDS3_TYPE DDS_TYPE_AD9834
	#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx/STM32F7xx internal RTC peripherial */
	//#define WITHRTCLSI 1	/* ������ LSI (��� ������) */

	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define PLL1_TYPE PLL_TYPE_LM7001
	//#define targetpll1 SPI_CSEL1

	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */
	#define DDS3_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS3 */

	///#define DDS1_TYPE DDS_TYPE_AD9834
	///#define targetdds1 SPI_CSEL1 	/* DDS1 - LO1 output - �������� AD9834 (Vasiliy, Ufa). */

	//#define TSC1_TYPE TSC_TYPE_STMPE811	/* touch screen controller */

	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	#define targetdds3 SPI_CSEL2 	/* DDS3 - PBT output */
	//#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. ��� ������� DDS ������� ���������� - ������ DDS */
	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */
#ifndef SPI_IOUPDATE_BIT 
	#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
#endif
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/
	#define targetdsp1 targetext1 	/* external devices control */

	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	//#define BOARD_DETECTOR_MUTE 	(0x02)
	//#define BOARD_DETECTOR_SSB 	(0x00)
	//#define BOARD_DETECTOR_AM 	(0x01)
	//#define BOARD_DETECTOR_FM 	(0x03)
	//#define BOARD_DETECTOR_TUNE 0x00	/* ������������ ����� ��� ������ TUNE (CWZ �� ��������) */

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define BOARD_FILTER_0P5		0x00	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_1P8		0x01	/* 1.8 kHz filter - �� ����� ������ ������ */
	#define BOARD_FILTER_2P7		0x03	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_3P1		0x03	/* 3.1 or 2.75 kHz filter */

	//#define BOARD_AGCCODE_OFF 0
	#if 1
		// ������� � ������� 3.1 ���
		//#define WITHMODESETSMART 1	/* � ����������� �� ������� �������� �������, ������������ ������ � WITHFIXEDBFO */
		//#define WITHMODESETFULL 1
		#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* ������������ WFM */
		/* ��� ��������� � ������ ������������ ������� */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		/* ��� ��������� � ������ ������������ ������� ��� �������� */
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* �������, ��� ������� ����� ������� HAVE */
		#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	#elif 1
		// ������� � ������� 2.7 ���
		//#define WITHMODESETSMART 1	/* � ����������� �� ������� �������� �������, ������������ ������ � WITHFIXEDBFO */
		#define WITHMODESETFULL 1
		//#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* ������������ WFM */
		/* ��� ��������� � ������ ������������ ������� */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_2P7)
		/* ��� ��������� � ������ ������������ ������� ��� �������� */
		#define IF3_FMASKTX	(IF3_FMASK_2P7)
		/* �������, ��� ������� ����� ������� HAVE */
		#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_2P7)
	#else
		//#define WITHMODESETSMART 1	/* � ����������� �� ������� �������� �������, ������������ ������ � WITHFIXEDBFO */
		#define WITHMODESETFULL 1
		//#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* ������������ WFM */
		#define IF3_FMASK (IF3_FMASK_2P7 | IF3_FMASK_1P8 | IF3_FMASK_0P5)
		#define IF3_FMASKTX (IF3_FMASK_2P7)
		#define IF3_FHAVE (IF3_FMASK_2P7 | IF3_FMASK_0P5)
	#endif

#if 0
// ���������� ������ ��� ����������.
enum 
{ 
#if WITHTEMPSENSOR
	TEMPIX = 16,		// ADC1->CR2 |= ADC_CR2_TSVREFE;	// ��� ������
#endif
#if WITHVOLTLEVEL 
	VOLTSOURCE = 0,		// ������� ����� �������� ����������, ��� ���
#endif /* WITHVOLTLEVEL */

#if WITHBARS
	SMETERIX = 0,	// S-meter
#endif
#if WITHVOX
	VOXIX = 0, AVOXIX = 0,	// VOX
#endif
#if WITHSWRMTR
	PWRI = 0,
	FWD = 0, REF = 0,	// SWR-meter
#endif
	KI0 = 0, KI1 = 0, KI2 = 0	// PF3, PF4, PF5 ����������
};

#define KI_COUNT 3	// ���������� ������������ ��� ���������� ������ ���
#define KI_LIST	KI2, KI1, KI0,	// �������������� ��� ������� �������������

#endif

#endif /* PION_DSP_CTLSTYLE_V1_H_INCLUDED */
