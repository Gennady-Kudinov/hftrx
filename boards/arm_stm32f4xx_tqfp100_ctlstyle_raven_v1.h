/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ��������� � 12-��� DSP ���������� "��������-2" �� ���������� STM32F429 � �������� TLV320AIC23B � CS4272
//

#ifndef ARM_STM32F4XX_TQFP100_CTLSTYLE_RAVEN_V1_H_INCLUDED
#define ARM_STM32F4XX_TQFP100_CTLSTYLE_RAVEN_V1_H_INCLUDED 1

	#define WITHSAICLOCKFROMI2S 1	/* ���� SAI1 ����������� �� PLL I2S */
	//#define WITHI2SCLOCKFROMPIN 1	// �������� ������� �� SPI2 (I2S) �������� � �������� ����������, � ��������� �������� ����� MCK ������ ����������
	//#define WITHSAICLOCKFROMPIN 1	// �������� ������� �� SAI1 �������� � �������� ����������, � ��������� �������� ����� MCK ������ ����������

	#define WITHUSEPLL		1	/* ������� PLL	*/
	#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/
	#define LTDC_DOTCLK	9000000uL	// ������� �������� ��� ������ � ����������� RGB

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
	// --- �������� ��������, ������������� ��� ������ ����

	#define CTLREGMODE_RAVENDSP_V1	1	/* "��������" � DSP */

	#define WITHLCDBACKLIGHT	1	// ������� ���������� ���������� ������� 
	#define WITHLCDBACKLIGHTMIN	0	// ������ ������ ����������� (������������ �� �������)
	#define WITHLCDBACKLIGHTMAX	3	// ������� ������ ����������� (������������ �� �������)
	#define WITHKBDBACKLIGHT	1	// ������� ���������� ���������� ���������� 

	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	#define BOARD_DETECTOR_SSB 	0		// ��������

	/* ���������� ���� ���������� ������������ �������� */
	#define BOARD_FMUX_BYP_RX	0x80	// ������� bypass ������ ��
	#define BOARD_FMUX_BYP_OFF	0x01	// �������� bypass ������ ��
	#define BOARD_FMUX_6P0_RX	0x40	// ������� 6 ��� ������ ��
	#define BOARD_FMUX_6P0_TX	0x02	// �������� 6 ��� ������ ��
	#define BOARD_FMUX_3P1_RX	0x20	// ������� 3.1 ��� ������ ��
	#define BOARD_FMUX_3P1_TX	0x04	// �������� 3.1 ��� ������ ��
	#define BOARD_FMUX_0P5_RX	0x10	// ������� 0.5 ��� ������ ��
	#define BOARD_FMUX_0P5_OFF	0x08	// �������� 0.5 ��� ������ ��
	/* ���� �������� ������ ��, ���������� �� �������������� ADG714 */
	/* �������� ���� �� �������� � ����� ����� */
	#define	BOARD_FILTERCODE_BYPRX	(BOARD_FMUX_BYP_RX | BOARD_FMUX_6P0_TX | BOARD_FMUX_3P1_TX | BOARD_FMUX_0P5_OFF)
	#define	BOARD_FILTERCODE_6P0RX	(BOARD_FMUX_BYP_OFF | BOARD_FMUX_6P0_RX | BOARD_FMUX_3P1_TX | BOARD_FMUX_0P5_OFF)
	#define	BOARD_FILTERCODE_3P1RX	(BOARD_FMUX_BYP_OFF | BOARD_FMUX_6P0_TX | BOARD_FMUX_3P1_RX | BOARD_FMUX_0P5_OFF)
	#define	BOARD_FILTERCODE_0P5RX	(BOARD_FMUX_BYP_OFF | BOARD_FMUX_6P0_TX * 0| BOARD_FMUX_3P1_TX * 1 | BOARD_FMUX_0P5_RX)
	/* �������� ���� �� �������� � ����� �������� */
	#define	BOARD_FILTERCODE_6P0TX	(BOARD_FMUX_BYP_OFF | BOARD_FMUX_6P0_TX * 1 | BOARD_FMUX_3P1_TX * 0 | BOARD_FMUX_0P5_OFF)
	#define	BOARD_FILTERCODE_3P1TX	(BOARD_FMUX_BYP_OFF | BOARD_FMUX_6P0_TX * 0 | BOARD_FMUX_3P1_TX * 1 | BOARD_FMUX_0P5_OFF)

	// +++ �������� ��� ���� � DSP ����������
	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01
	// --- �������� ��� ���� � DSP ����������

	#define WITHPREAMPATT2_6DB		1	// ���������� ��� � ������������� ������������ � ����������� 0 - 6 - 12 - 18 dB */

	#define WITHAGCMODEONOFF	1	// ��� ���/����
	//#define	WITHMIC1LEVEL		1	// ��������� �������� ���������

	//#define DSTYLE_UR3LMZMOD	1	// ������������ ��������� ������ � ����������� UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// ������������ �������������� �����

	// +++ ������ �������� ������������ ������ �� ����������
	// ��� ���� ������ ��� � ����������
	#define KEYB_FPAMEL20_V0A	1	/* 20 ������ �� 5 ����� - ����� rfrontpanel_v0 + LCDMODE_UC1608 � ���������� ������������ � ����� ������������� */
	//#define KEYB_RAVEN20_V5	1		/* 5 ����� ����������: ������������ ������ ��� ������� � DSP ���������� */

	// ��� ����� ������ ��� � ����������
	//#define KEYB_RAVEN24	1	/* ������������ ������ ��� ������� � DSP ���������� */
	//#define KEYB_VERTICAL_REV	1	/* ������������ ������ ��� ���� "�������" � "�������" */
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
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	/* Board hardware configuration */
	#define	CODEC1_TYPE	CODEC_TYPE_TLV320AIC23B
	#define CODEC2_TYPE CODEC_TYPE_CS4272
	#define WITHSAI1HWTXRXMASTER	1		// SAI1 work in MASTER mode

	//#define CODEC_TYPE_CS4272_USE_SPI	1		// codecboard v2.0
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

	#define WITHNESTEDINTERRUPTS	1	/* ������������ ��� ������� real-time �����. */
	#define WITHINTEGRATEDDSP		1	/* � ��������� �������� ������������� � ������ DSP �����. */
	//#define WITHDACOUTDSPAGC		1	/* ��� ����������� ��� ����� ��� �� ���������� �����. */
	//#define WITHDSPEXTDDC 1			/* ���������� ���������� ������� ����������� */

	#define WITHIFDACWIDTH	24		// 1 ��� ���� � 23 ��� ��������
	#define WITHIFADCWIDTH	24		// 1 ��� ���� � 23 ��� ��������
	#define WITHAFADCWIDTH	16		// 1 ��� ���� � 15 ��� ��������
	#define WITHAFDACWIDTH	16		// 1 ��� ���� � 15 ��� ��������
	//#define WITHEXTERNALDDSP		1	/* ������� ���������� ������� DSP ������. */
	//#define WITHLOOPBACKTEST	1
	#define WITHIF4DSP	1	// "�����"
	#define WITHDSPLOCALFIR 1			/* ���������� ��������� �������������� ����������� */
	//#define WITHMODEM 1		// ������������ �����/�������� BPSK31
	//#define WITHFREEDV	1	/* ��������� ������ FreeDV - http://freedv.org/ */ 
	//#define WITHUSESDCARD 1			// ��������� ��������� SD CARD
	//#define WITHUSEAUDIOREC	1	// ������ ����� �� SD CARD

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	//#define WITHRFSG	1	/* �������� ���������� �� ������-�����������. */
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHAUTOTUNER	1	/* ���� ������� ���������� */
	#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	//#define WITHPBT		1	/* ������������ PBT (���� LO3 ����) */
	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	//#define WITHNMEA		1	/* ������������ NMEA parser */
	//#define WITHBEACON	1	/* ������������ ����� ����� */
	#define WITHVOX 1		/* ������������ VOX */
	#define WITHSHOWSWRPWR 1	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
	#define WITHSWRMTR	1	/* ���������� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� �������� ��� */
	//#define WITHPWRLIN	1	/* ��������� �������� �������� ���������� ���������� � �� �������� */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	////#define WITHPOTWPM		1	/* ������������ ����������� �������� �������� � ��������� �������������� */
	#define WITHPOTGAIN		1	/* ���������� �������� �� �������������� */
	#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */
	#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	//#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	//#define WITHDUALBFO		1	/* ��������� ������� �� ���� ������������ ������� BFO ������� �������� */
	//#define WITHDUALFLTR	1	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */


	#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */
	#define WITHBCBANDS	1		/* � ������� ���������� ������������ ����������� ��������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHLO1LEVELADJ		1	/* �������� ���������� ������� (����������) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	#define WITHTEMPSENSOR	1	/* ����������� ������ � ������� ����������� */
	#define WITHDIRECTBANDS 1	/* ������ ������� � ���������� �� �������� �� ���������� */
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
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16		// demo board with atxmega128a4u
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	//#define HARDWARE_IGNORENONVRAM	1		// ������� �� ������ ��� ��� �������� NVRAM

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */

	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
	/* Board hardware configuration */
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	#define DDS1_TYPE DDS_TYPE_AD9951
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	#define DDS2_TYPE DDS_TYPE_AD9834
	//#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx internal RTC peripherial */

	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#if FQMODEL_80455
		#define DDS2_CLK_DIV	2		/* �������� ������� ������� ����� ������� � DDS2 */
	#else
		#define DDS2_CLK_DIV	2		/* �������� ������� ������� ����� ������� � DDS2 */
	#endif
	////#define HYBRID_NVFOS 4				/* ���������� ��� ������� ���������� */
	////#define HYBRID_PLL1_POSITIVE 1		/* � ����� ���� ��� �������������� ��������� */
	//#define HYBRID_PLL1_NEGATIVE 1		/* � ����� ���� ���� ������������� ��������� */

	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. ��� ������� DDS ������� ���������� - ������ DDS */
	#define targetext1 SPI_CSEL4 	/* external devices control */
	#define targetext2 SPI_CSEL5 	/* external devices control */
	#define targetctl1 SPI_CSEL6 	/* control register */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetsdcard targetext2	/* ������ ����� �� SD CARD */

	//#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/
	#define targetuc1608 targetext1	/* LCD with positive chip select signal	*/

	#define targetnvram SPI_CSEL8  	/* serial nvram */
	#define targetcodec1 SPI_CSEL9 	/* PD3 TLV320AIC23B */

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0P5RX	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		BOARD_FILTERCODE_3P1RX	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_6P0		BOARD_FILTERCODE_6P0RX	/* 6.0 kHz filter */
	#define BOARD_FILTER_8P0		BOARD_FILTERCODE_BYPRX	/* 6.0 kHz filter */

	#define WITHMODESETFULLNFM 1
	//#define WITHWFM	1			/* ������������ WFM */
	/* ��� ��������� � ������ ������������ ������� */
	#define IF3_FMASK	(/*IF3_FMASK_0P5 | IF3_FMASK_3P1 | */ IF3_FMASK_6P0 | IF3_FMASK_8P0)
	/* ��� ��������� � ������ ������������ ������� ��� �������� */
	#define IF3_FMASKTX	(/*IF3_FMASK_3P1 | */ IF3_FMASK_6P0)
	/* �������, ��� ������� ����� ������� HAVE */
	#define IF3_FHAVE	(/* IF3_FMASK_0P5 | IF3_FMASK_3P1 | */ IF3_FMASK_6P0 | IF3_FMASK_8P0)

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
#if WITHTEMPSENSOR
	TEMPIX = 16,		// ADC1->CR2 |= ADC_CR2_TSVREFE;	// ��� ������
#endif

#if WITHPOTGAIN
	POTIFGAIN = 10,		// PC0 IF GAIN
	POTAFGAIN = 11,		// PC1 AF GAIN
#endif /* WITHPOTGAIN */

#if WITHPOTWPM
	POTWPM = 12,		// PC2 ������������ ���������� ��������� �������� � ���������
#endif /* WITHPOTWPM */

	POTAUX1 = 13,		// PC3 AUX1
	POTAUX2 = 14,		// PC4 AUX2
	POTAUX3 = 15,		// PC5 AUX3

#if WITHSWRMTR
	PWRI = 9,			// PB1
	FWD = 9, REF = 8,	// PB0	SWR-meter
#endif /* WITHSWRMTR */
	// ��� ����� ������ ��� � ����������
	//KI0 = 0, KI1 = 1, KI2 = 2, KI3 = 3, KI4 = 6, KI5 = 7	// ����������
	// ��� ���� ������ ��� � ����������
	KI0 = 0, KI1 = 1, KI2 = 2, KI3 = 3, KI4 = 6	// ����������
};

#define KI_COUNT 5	// ���������� ������������ ��� ���������� ������ ���
#define KI_LIST	KI4, KI3, KI2, KI1, KI0,	// �������������� ��� ������� �������������
//#define KI_COUNT 6	// ���������� ������������ ��� ���������� ������ ���
//#define KI_LIST	KI5, KI4, KI3, KI2, KI1, KI0,	// �������������� ��� ������� �������������

#endif /* ARM_STM32F4XX_TQFP100_CTLSTYLE_RAVEN_V1_H_INCLUDED */
