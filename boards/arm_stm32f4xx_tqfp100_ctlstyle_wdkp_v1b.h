/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ����� �������-2 (� �������)
//

#ifndef ARM_STM32F4XX_TQFP100_CTLSTYLE_WDKP_V1B_H_INCLUDED
#define ARM_STM32F4XX_TQFP100_CTLSTYLE_WDKP_V1B_H_INCLUDED 1

	#define WITHSAICLOCKFROMI2S 1	/* ���� SAI1 ����������� �� PLL I2S */
	//#define WITHI2SCLOCKFROMPIN 1	// �������� ������� �� SPI2 (I2S) �������� � �������� ����������, � ��������� �������� ����� MCK ������ ����������
	//#define WITHSAICLOCKFROMPIN 1	// �������� ������� �� SAI1 �������� � �������� ����������, � ��������� �������� ����� MCK ������ ����������

	#define WITHUSEPLL		1	/* ������� PLL	*/
	#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	#if 0
		// ��� ������� �������� ���������� ����������
		#define WITHCPUXTAL 16000000uL	/* �� ���������� ���������� ����� 16.000 ��� */
		#define REF1_DIV 8			// ref freq = 2.0000 MHz

		#if CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 216		// 2*192.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#elif CPUSTYLE_STM32F4XX
			// normal operation frequency
			#define REF1_MUL 180		// 2*180.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// �������� ��� ������ � ������� 5 WS for 168 MHz at 3.3 volt
		#endif

	#elif 1
		// ��� ������� �������� ���������� ����������
		#define WITHCPUXTAL 18432000uL	/* �� ���������� ���������� ����� 18.432 ��� */
		#define REF1_DIV 18			// ref freq = 1.024 MHz

		#if CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 421		// 2*xxx.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#elif CPUSTYLE_STM32F4XX
			// normal operation frequency
			#define REF1_MUL 351		// 2*179.712 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// �������� ��� ������ � ������� 5 WS for 168 MHz at 3.3 volt
		#endif

	#else
		// ������������ �� ����������� RC ���������� 16 ���
		#define REF1_DIV 8			// ref freq = 2.000 MHz

		#if CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 216		// 2*192.000 MHz (192 <= PLLN <= 432) - overclocking, but USB 48 MHz clock generation
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#elif CPUSTYLE_STM32F4XX
			// normal operation frequency
			#define REF1_MUL 180		// 2*180.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// �������� ��� ������ � ������� 5 WS for 168 MHz at 3.3 volt
		#endif
	#endif

	#if WITHI2SCLOCKFROMPIN
		#define FPGADECIMATION 2560
		#define FPGADIVIDERATIO 5
		#define EXTI2S_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO)
		#define EXTSAI_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO)

		#define ARMI2SMCLK	(REFERENCE_FREQ * DDS1_CLK_MUL / (FPGADECIMATION / 256))
		#define ARMSAIMCLK	(REFERENCE_FREQ * DDS1_CLK_MUL / (FPGADECIMATION / 256))
	#else /* WITHI2SCLOCKFROMPIN */
		#define PLLI2SN_MUL 336		// 344.064 (192 <= PLLI2SN <= 432)
		#define SAIREF1_MUL 240		// 245.76 / 1.024 = 240 (49 <= PLLSAIN <= 432)
		// ������� ����������� �����������
		#define ARMI2SMCLK	(12288000UL)
		#define ARMSAIMCLK	(12288000UL)
	#endif /* WITHI2SCLOCKFROMPIN */

	/* ������ ������������ - ����� ������������������ */

#if 1
	/* ������ ��������� ���� - � ��������������� "������" */
	//#define FQMODEL_45_IF8868_UHF430	1	// SW2011
	//#define FQMODEL_73050		1	// 1-st IF=73.050, 2-nd IF=0.455 MHz
	//#define FQMODEL_73050_IF0P5		1	// 1-st IF=73.050, 2-nd IF=0.5 MHz
	#define FQMODEL_80455		1	// 1-st IF=80.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455		1	// 1-st IF=64.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455_IF0P5		1	// 1-st IF=64.455, 2-nd IF=0.5 MHz
	//#define FQMODEL_70455		1	// 1-st IF=70.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	//#define FQMODEL_60700_IF05	1	// 60.7 -> 10.7 -> 0.5
	//#define FQMODEL_60725_IF05	1	// 60.725 -> 10.725 -> 0.5
	//#define FQMODEL_60700_IF02	1	// 60.7 -> 10.7 -> 0.2
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */
	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
	#define WITHIF4DSP	1	// "�����"
	#define WITHDSPLOCALFIR 1			/* ���������� ��������� �������������� ����������� */

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
	#define FQMODEL_45_IF455	1	// 1-st IF=45MHz, 2-nd IF=455 kHz
	//#define FQMODEL_45_IF128	1	// 1-st IF=45MHz, 2-nd IF=128 kHz
	#define BANDSELSTYLERE_UPCONV36M	1	/* Up-conversion with working band 0.1 MHz..36 MHz */
	//#define BANDSELSTYLERE_UPCONV32M	1	/* Up-conversion with working band 0.1 MHz..32 MHz */
	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */

#elif 0
	/* ������ ��������� ���� - � ��������������� "����" */
	#define FQMODEL_TRX500K		1	// ������ (� �����������) �� 500 ���
	//#define FQMODEL_TRX455K		1	// ������ (� ������������) �� 455 ���
	//#define WITHCLASSICSIDE	1	/* ������� ������� ���������� ������� �� ������� ������� */
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV15M	1	/* Down-conversion with working band .030..15 MHz */
	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
#elif 0
	/* ������ ��������� ���� - � ������ ���������������  */
	#define FQMODEL_DCTRX		1	// ������ ��������������
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	//#define DIRECT_100M0_X4		1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	//#define WITHIF4DSP	1	// "�����"
	//#define DEFAULT_DSP_IF	12000
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
	//#define IF3_MODEL IF3_TYPE_5500
	//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
	//#define IF3_MODEL IF3_TYPE_8000
	//#define IF3_MODEL IF3_TYPE_8215
	#define IF3_MODEL IF3_TYPE_8868
	//#define IF3_MODEL IF3_TYPE_9000
	//#define IF3_MODEL IF3_TYPE_9045
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	//#define DIRECT_80M0_X5		1	/* Board hardware configuration */
	//#define DIRECT_DDS_ATTINY2313 1
	//#define HYBRID_DDS_ATTINY2313 1
	//#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
#elif 0
	/* ����������� ������, �������� � �� ��������. */
	//#define FQMODEL_GEN500	1	// CML evaluation board with CMX992 chip, 1-st IF = 45 MHz
	//#define FQMODEL_RFSG_SI570	1	// ������-��������� �� Si570 "B" grade
	//#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
#endif	
	// --- �������� ��������, ������������� ��� ������ ����

	//#define REQUEST_FOR_RN4NAB 1	/* ����������� ������� ����������� �������� � blue bg */

	#define CTLREGSTYLE_WDKP 1

	#define WITHAGCMODENONE		1	/* �������� ��� �� ��������� */
	#define WITHPREAMPATT2_6DB		1	// ���������� ��� � ������������� ������������ � ����������� 0 - 6 - 12 - 18 dB */
	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	#define BOARD_DETECTOR_MUTE 0
	#define BOARD_DETECTOR_SSB 	1
	#define BOARD_DETECTOR_AM 	2
	#define BOARD_DETECTOR_FM 	3

	#define DSTYLE_UR3LMZMOD	1	// ������������ ��������� ������ � ����������� UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// ������������ �������������� �����

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
	//#define LCDMODE_LTDC	1		/* Use STM32F4xxx with LCD-TFT Controller (LTDC), also need LCDMODE_ILI9341 */
	//#define LCDMODE_LTDC_L8	1	/* ������������ 8 ��� �� ������� ������������� ������. ����� - 16 ���. */
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
	#define LCDMODE_LS020 	1	/* ��������� 176*132 Sharp LS020B8UD06 � ������������ LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - ����������� ����������� */
	//#define LCDMODE_LPH88		1	/* ��������� 176*132 LPH8836-2 � ������������ Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - ����������� ����������� */
	//#define LCDMODE_ILI9163	1	/* ��������� LPH9157-2 176*132 � ������������ ILITEK ILI9163 - ����� ������� ������, � ������ �����. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - ����������� ����������� (��� ������� ������, ����� ������� �����) */
	//#define LCDMODE_L2F50	1	/* ��������� 176*132 � ������������ Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* ��������� ���������� � � ������ LCDMODE_L2F50 */
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
	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	//#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	/* Board hardware configuration */
	#define	CODEC1_TYPE	CODEC_TYPE_TLV320AIC23B
	#define CODEC2_TYPE CODEC_TYPE_CS4272
	#define WITHSAI1HWTXRXMASTER	1		// SAI1 work in MASTER mode

	//#define CODEC_TYPE_CS4272_USE_SPI	1		// codecboard v2.0, v3.0
	#define CODEC_TYPE_CS4272_STANDALONE	1		// codecboard v3.0
	#define CODEC_TYPE_TLV320AIC23B_USE_SPI	1	// codecboard v2.0

	#if defined (STM32F446xx) || CPUSTYLE_STM32F7XX
		// ���������������� I2S2 � I2S3
		// �� ��������� ����������� I2S �� ����� �������� � full duplex mode,
		// ��� ����� ����������� I2S2 ��� ���������� � I2S3 ��� ������� � �������� ������������
		#define WITHI2SHWRXSLAVE	1		// ������� ����� I2S (��������) ����������� � SLAVE MODE
	#else /* defined (STM32F446xx) */
		#define WITHI2SFULLDUPLEXHW 1
	#endif /* defined (STM32F446xx) */

	//#define WITHI2S_FORMATI2S_PHILIPS 1	// �������� ������������� ��� �������� ������ � �����, ������������ � ��������� � ���������
	#define WITHSAI1_FRAMEBITS	64	// ������ ������ ������
	#define WITHSAICLOCKFROMI2S	1	/* ���� SAI1 ����������� �� PLL I2S */
	//#define	WITHI2SCLOCKFROMPIN 1	// �������� ������� �� SPI2 (I2S) �������� � �������� ����������, � ��������� �������� ����� MCK ������ ����������
	//#define	WITHSAICLOCKFROMPIN 1	// �������� ������� �� SAI1 �������� � �������� ����������, � ��������� �������� ����� MCK ������ ����������
	#define WITHSPISLAVE 	1	/* ������������� ����������� � ������ SPI SLAVE */
	#define WITHNESTEDINTERRUPTS	1	/* ������������ ��� ������� real-time �����. */
	#define WITHINTEGRATEDDSP		1	/* � ��������� �������� ������������� � ������ DSP �����. */

	#define WITHIFDACWIDTH	24		// 1 ��� ���� � 23 ��� ��������
	#define WITHIFADCWIDTH	24		// 1 ��� ���� � 23 ��� ��������
	#define WITHAFADCWIDTH	16		// 1 ��� ���� � 15 ��� ��������
	#define WITHAFDACWIDTH	16		// 1 ��� ���� � 15 ��� ��������
	#define WITHDACOUTDSPAGC		1	/* ��� ����������� ��� ����� ��� �� ���������� �����. */
	//#define WITHEXTERNALDDSP		1	/* ������� ���������� ������� DSP ������. */
	//#define WITHLOOPBACKTEST	1
	//#define WITHMODEM 1	// ������������ �����/�������� BPSK31
	//#define WITHFREEDV	1	/* ��������� ������ FreeDV - http://freedv.org/ */ 

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	//#define WITHRFSG	1	/* �������� ���������� �� ������-�����������. */
	//#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHAUTOTUNER	1	/* ���� ������� ���������� */
	//#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	//#define WITHPBT		1	/* ������������ PBT (���� LO3 ����) */
	//#define WITHCAT		1	/* ������������ CAT */
	//#define WITHNMEA		1	/* ������������ NMEA parser */
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
	//#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */

	//#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	//#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	//#define WITHDUALBFO		1	/* ��������� ������� �� ���� ������������ ������� BFO ������� �������� */
	//#define WITHDUALFLTR	1	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */


	//#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */
	//#define WITHBCBANDS	1		/* � ������� ���������� ������������ ����������� ��������� */
	//#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHLO1LEVELADJ		1	/* �������� ���������� ������� (����������) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	//#define WITHTEMPSENSOR	1	/* ����������� ������ � ������� ����������� */
	//#define WITHDIRECTBANDS 1	/* ������ ������� � ���������� �� �������� �� ���������� */
	//#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
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
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16		// demo board with atxmega128a4u
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	#define HARDWARE_IGNORENONVRAM	1		// ������� �� ������ ��� ��� �������� NVRAM

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */

	/* Board hardware configuration */
	#define PLL1_TYPE PLL_TYPE_NONE

	///#define DDS1_TYPE DDS_TYPE_AD9834
	///#define targetdds1 SPI_CSEL1 	/* DDS1 - LO1 output - �������� AD9834 (Vasiliy, Ufa). */
	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	//#define targetdds3 SPI_CSEL2 	/* DDS3 - PBT output */
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

	//#define targetcodec2 SPI_CSEL2 	/* PD1 CS4272 */
	#define targetcodec1 SPI_CSEL3 	/* PD3 TLV320AIC23B */

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define BOARD_FILTER_0P5		0	/* 0.5 or 0.3 kHz filter */
	//#define BOARD_FILTER_1P8		0	/* 1.8 kHz filter - �� ����� ������ ������ */
	//#define BOARD_FILTER_2P7		0	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_3P1		0	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_6P0		0	/* 6.0 kHz filter */

	//#define WITHMODESETSMART 1	/* � ����������� �� ������� �������� �������, ������������ ������ � WITHFIXEDBFO */
	#define WITHMODESETFULL 1
	//#define WITHMODESETFULLNFM 1
	//#define WITHWFM	1			/* ������������ WFM */
	/* ��� ��������� � ������ ������������ ������� */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1 | IF3_FMASK_6P0)
	/* ��� ��������� � ������ ������������ ������� ��� �������� */
	#define IF3_FMASKTX	(IF3_FMASK_3P1)
	/* �������, ��� ������� ����� ������� HAVE */
	#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)

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
#define KI_LIST	KI2, KI1, KI0,	// �������������� ��� ������� �������������

#endif /* ARM_STM32F4XX_TQFP100_CTLSTYLE_WDKP_V1B_H_INCLUDED */
