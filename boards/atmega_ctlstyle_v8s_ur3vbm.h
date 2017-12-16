/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

// �������� ���������� "������� ���������� �� AD9835"
// ����� ������������ ur5zvu@yandex.ru - RDX0077, Si570

#ifndef ATMEGA_CTLSTYLE_V8S_UR3VBM_INCLUDED
#define ATMEGA_CTLSTYLE_V8S_UR3VBM_INCLUDED 1

	#if F_CPU != 8000000
		#error Set F_CPU right value in project file
	#endif

	/* ������ ������������ - ����� ������������������ */

	//#define ELKEY328 		1	/* ����������� ����	*/
#if 0
	/* ������ ��������� ���� - � ��������������� "����" */
	//#define DIRECT_75M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_66M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_66M66_X1		1	/* Board hardware configuration */
	//#define DIRECT_48M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_50M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_80M0_X1		1	/* Board hardware configuration */
	#define DIRECT_27M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_26M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_60M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_125M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_XXXX_X1	1
	//#define REFERENCE_FREQ	24000000uL	/* ��� Si5351 */

	#define FQMODEL_TRX8M		1	// ������ ������ �� 8 ���, 6 ���, 5.5 ��� � ������

	//#define BANDSELSTYLERE_LOCONV32M	1
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	// ����� ��
	//#define IF3_MODEL IF3_TYPE_9000
	//#define IF3_MODEL IF3_TYPE_8000
	#define IF3_MODEL IF3_TYPE_8868
	//#define IF3_MODEL IF3_TYPE_6000
	//#define IF3_MODEL IF3_TYPE_8192
	//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
	//#define IF3_MODEL IF3_TYPE_5500
	//#define IF3_MODEL IF3_TYPE_9045 
	//#define IF3_MODEL IF3_TYPE_10700
#elif 1
	/* ������ ��������� ���� - � ��������������� "����" */
	//#define DIRECT_75M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_66M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_48M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_50M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_60M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_125M0_X1		1	/* Board hardware configuration */
	#define DIRECT_27M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_26M0_X1		1	/* Board hardware configuration */

	#define FQMODEL_TRX500K		1	// ������ (� �����������) �� 500 ���
	//#define BANDSELSTYLERE_LOCONV15M	1
	#define BANDSELSTYLERE_LOCONV15M_NLB	1	/* Down-conversion with working band 1.6..15 MHz */
#elif 1
	/* ������ �������������� */
	//#define DIRECT_50M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_125M0_X1		1	/* Board hardware configuration */
	#define DIRECT_27M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_26M0_X1		1	/* Board hardware configuration */
	#define FQMODEL_DCTRX		1	// ������ ��������������
	//#define BANDSELSTYLERE_LOCONV32M	1
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
#endif	
	// --- �������� ��������, ������������� ��� ������ ����
	#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */

	// --- �������� ��������, ������������� ��� ������ ����

	#define DSTYLE_UR3LMZMOD	1
	//#define	FONTSTYLE_ITALIC	1	//

	#define CTLREGMODE16_UR3VBM	1		/* ����������� ������� 16 ��� - ��� SIMPLE ����������� �� AD8932/AD8935/AD8933	*/
	//#define CTLREGMODE8_UR5YFV	1	/* 8 ��� */
	//#define CTLREGMODE16_RN3ZOB	1		/* ����������� ������� 16 ��� - ��� SIMPLE ����������� �� AD8932/AD8935/AD8933	*/
	//
	//#define CTLREGSTYLE_NOCTLREG	1	/* ����������� ����������� ������� - ��� ������������. */

	// +++ �������� ��������, ������������� ��� ���� �� ATMega
	// --- �������� ��������, ������������� ��� ���� �� ATMega

	// +++ ������ �������� ������������ ������ �� ����������
	#define KEYB_VERTICAL_UR3VBM	1	/* ������������ ������ ��� ���� "�������" � "�������" */

	#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
	//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
	//#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
	//#define WITHELKEY	1
	//#define WITHKBDENCODER 1	// ����������� ������� ��������
	#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
	#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */
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
	#define LCDMODE_ILI9163	1	/* ��������� LPH9157-2 176*132 � ������������ ILITEK ILI9163 - ����� ������� ������, � ������ �����. */
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
	//#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T � ������������ ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	//#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	#define ENCRES_DEFAULT ENCRES_24
	//#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */



	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	//#define WITHRFSG	1	/* �������� ���������� �� ������-�����������. */
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	//#define WITHPBT		1	/* ������������ PBT */
	//#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	//#define WITHVOX		1	/* ������������ VOX */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	#define WITHSHOWSWRPWR 1	/* �� ������� ������������ ������������ SWR-meter � PWR-meter - ������� WITHSWRMTR */
	#define WITHSWRMTR	1	/* ���������� ��� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� ��������  */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	//#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */
	//#define WITHPOTWPM		1	/* ������������ ����������� �������� �������� � �������� �������������� */
	#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������

	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define DEFPREAMPSTATE 	1	/* ��� �� ��������� ������� (1) ��� �������� (0) */
	
	//#define REQUEST_FOR_RN4NAB 1	/* ����������� ������� ����������� �������� */

	//#define LSBONLO4DEFAULT	1	/* ������� ��� �� ������ ������� */
	//#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	//#define WITHDUALFLTR	1	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */
	#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */
	//#define WITHBCBANDS	1		/* � ������� ���������� ������������ ����������� ��������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHDIRECTBANDS 1	/* ������ ������� � ���������� �� �������� �� ���������� */
	#define WITHFLATMENU	1
	/* ��� �� ������ �������� � ������ ������������ � ����������� */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// ��� �� ��� ������������� FM25040A - 5 �����, 512 ����
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */

	//#define WITHLO1LEVELADJ		1	/* �������� ���������� ������� (����������) LO1 */
	/* Board hardware configuration */
	//#define PLL1_TYPE PLL_TYPE_SI5351A
	//#define PLL1_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	//#define PLL2_TYPE PLL_TYPE_SI5351A
	//#define PLL2_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9835	/* AD9832, AD9835 */
	//#define PLL1_TYPE PLL_TYPE_LMX1601
	//#define DDS1_TYPE DDS_TYPE_AD9851	/* AD9851, AD9850 */
	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define DDS1_TYPE DDS_TYPE_AD9834
	#define WITHSI5351AREPLACE 1

	//#define DDS1_CLK_DIV	7		/* �������� ������� ������� ����� ������� � DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */

	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetnvram SPI_CSEL1  	/* serial nvram */
	#define targetlcd SPI_CSEL4 	/* LCD over SPI line devices control */ 
	#define targetctl1 SPI_CSEL6 	/* control register */
	//#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */

	//#define BOARD_AGCCODE_OFF 0
	//#define WITHMODESETSMART 1	/* � ����������� �� ������� �������� �������, ������������ ������ � WITHFIXEDBFO */
	#if 0 //WITHFIXEDBFO || WITHDUALBFO
		#define	WITHMODESETMIXONLY	1	// USB/LSB, CW/CWR - ��� FIXEDBFO
		/* ��� ��������� � ������ ������������ ������� */
		#define IF3_FMASK	(IF3_FMASK_3P1)
		#define IF3_FHAVE 	(IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#elif 1//FQMODEL_DCTRX
		#define WITHMODESETMIXONLY 1	// USB/LSB, CW/CWR
		/* ��� ��������� � ������ ������������ ������� */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FHAVE 	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#else
		#error Config error
		/* ��� ��������� � ������ ������������ ������� */
		//#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)
		//#define IF3_FHAVE 	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		//#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#endif


// ���������� ������ ��� ����������.
enum 
{ 
#if WITHVOX
	VOXIX = 2, AVOXIX = 1,	// VOX
#endif
#if WITHBARS
	SMETERIX = 0,	// S-meter
	#if WITHSWRMTR
		PWRI = 4,	// SWR-meter
		FWD = 4, REF = 3,	// SWR-meter
	#endif
	#if WITHPWRMTR
		PWRI = 4,	// SWR-meter
	#endif
#endif
#if WITHPOTWPM
	POTWPM = 3,		// ������������ ���������� ��������� �������� � ���������
#endif
	KI0 = 6, KI1 = 7	// ����������
};
#define KI_COUNT 2	// ���������� ������������ ��� ���������� �������� ������ ���


#endif /* ATMEGA_CTLSTYLE_V8S_UR3VBM_INCLUDED */
