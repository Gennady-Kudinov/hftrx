/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ����� UA3DKC ������ (ATMega32 @ 8 MHz, FM25040, Si570+AD9834+WH2002) - ref 40 MHz
// ����� UA3DKC ������ (ATMega644PA @ 8 MHz, AT25160B, Si570+AD9834+WH2002) - ref 40 MHz
//

#ifndef ATMEGA_CTL_UA3DKC_H_INCLUDED
#define ATMEGA_CTL_UA3DKC_H_INCLUDED 1

#if F_CPU != 8000000
	//#error Set F_CPU right value in project file
#endif
	/* ������ ������������ - ����� ������������������ */

	/* ������ ��������� ���� - � ��������������� "����" */
	#define FQMODEL_TRX8M		1	// ������ ������ �� 8 ���, 6 ���, 5.5 ��� � ������
	//#define FQMODEL_TRX500K		1	// ������ (� �����������) �� 500 ���
	//#define FQMODEL_DCTRX		1	// ������ ��������������
	#define BANDSELSTYLERE_LOCONV32M	1
	//#define BANDSELSTYLERE_LOCONV15M	1

	#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
	#define LO1MODE_DIRECT	1

	#if 0
		// UR3IUK
		#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
		#define DIRECT_40M0_X1		1
		#define IF3_MODEL IF3_TYPE_8868
		#define CTLREGMODE24_UA3DKC	1		/* ����������� ������� 24 ��� - ��� ����������� UA3DKC	*/
	#elif 1
		// UA3DKC
		//#define DIRECT_40M0_X1		1	// � ���������� � ����������� 20*2
		//#define DIRECT_48M0_X1		1	// � ���������� � ����������� 20*2
		#define DIRECT_64M0_X1		1	// � ���������� � ����������� 20*2
		//#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2  */
		#define LCDMODE_UC1608	1		/* ��������� 240*128 � ������������ UC1608.- ����������� */
		//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - ����������� ����������� (��� ������� ������) */
		#define IF3_MODEL IF3_TYPE_8868
		#define CTLREGMODE24_UA3DKC	1		/* ����������� ������� 24 ��� - ��� ����������� UA3DKC	*/
	#else
		// UA3DKC
		#define DIRECT_66M0_X1	1	// � ���������� � ����������� 16*2
		#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
		#define IF3_MODEL IF3_TYPE_9000
		#define CTLREGMODE24_UA3DKC	1		/* ����������� ������� 24 ��� - ��� ����������� UA3DKC	*/
	#endif
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1 		/* ���������� � DDS3 */

	#define	FONTSTYLE_ITALIC	1	//

	// --- �������� ��������, ������������� ��� ������ ����
	#if 1
		#define KEYB_V8S_HORISONTAL_UA3DKC	1	/* ������������ ��� ����������� DK1VS/UA3DKC */
		#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */
	#else
		#define KEYB_UA3DKC	1	/* ������������ ��� ����������� KEYB_UA3DKC */
		#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */
		#define KEYBOARD_USE_ADC6	1	/* ����� ������ �� ������ ����� ADCx */
		#define KEYBOARD_USE_ADC6_V1	1	/* ����� ������ �� ������ ����� ADCx */
	#endif

	#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
	//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
	#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
	#define WITHELKEY	1
	//#define WITHKBDENCODER 1	// ����������� ������� ��������
	#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	//#define ENCRES_DEFAULT ENCRES_24
	#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	#define WITHAUTOTUNER	1	/* ���� ������� ���������� */
	//#define FULLSET7 1
	//#define SHORTSET7 1
	//#define FULLSET8 1
	#define SHORTSET8 1

	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	//#define WITHVOX		1	/* ������������ VOX */
	#define WITHSWRMTR	1	/* ���������� ��� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� ��������  */
	#define WITHSHOWSWRPWR 1	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
	#define WITHBARS	1	/* ����������� S-����� � SWR-����� */
	//#define WITHPWRLIN	1	/* ��������� �������� �������� ���������� ���������� � �� �������� */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHPBT		1	/* ������������ PBT */
	//#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	//#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	//#define WITHDUALBFO		1	/* ��������� ������� �� ���� ������������ ������� BFO ������� �������� */
	//#define WITHDUALFLTR	1	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */
	//#define WITHSAMEBFO	1	/* ������������� ����� �������� BFO ��� ����� � �������� */
	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */
	//#define WITHBCBANDS	1		/* � ������� ���������� ������������ ����������� ��������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHDIRECTBANDS 1	/* ������ ������� � ���������� �� �������� �� ���������� */
	#define WITHSPKMUTE		1	/* ���������� ����������� �������� */

	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	//#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */
	#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
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
	#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section

	//#define FTW_RESOLUTION 28	/* ����������� FTW ���������� DDS */
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */
	//#define FTW_RESOLUTION 31	/* ����������� FTW ���������� DDS - ATINY2313 */

	//#define WITHLO1LEVELADJ		1	/* �������� ���������� ������� (����������) LO1 */
	/* Board hardware configuration */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9835
	//#define DDS1_TYPE DDS_TYPE_AD9851
	#define PLL1_TYPE PLL_TYPE_SI570
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define DDS1_TYPE DDS_TYPE_AD9834
	#define DDS2_TYPE DDS_TYPE_AD9834
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define PLL2_TYPE PLL_TYPE_ADF4001	/* �������� ��������� ���������� ��� ��������� ��������� ��������� */
	//#define PLL2_TYPE PLL_TYPE_LMX2306	/* �������� ��������� ���������� ��� ��������� ��������� ��������� */

	//#define DDS1_CLK_DIV	7		/* �������� ������� ������� ����� ������� � DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */

	/* ���������� ������� �� SPI ���� */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. ��� ������� DDS ������� ���������� - ������ DDS */
	#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */ 
	//#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/


	#define BOARD_FILTER_0P5		0x01	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_1P8		0x02	/* 1.8 kHz filter - �� ����� ������ ������ */
	#define BOARD_FILTER_2P7		0x14	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_3P1		0x28	/* 3.1 or 2.75 kHz filter */

	#define WITHMODESETMIXONLY3 1
	/* ��� ��������� � ������ ������������ ������� */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)
	#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)
	#define IF3_FMASKTX	(IF3_FMASK_3P1 | IF3_FMASK_2P7)

	
// ���������� ������ ��� ����������.
enum 
{ 
#if WITHVOX
	//VOXIX = 2, AVOXIX = 1,	// VOX - ������, ������ ������������� ��� ����� �����
#endif
	SMETERIX = 0,	// S-meter
	PWRI = 4, 
	FWD = 4, REF = 3,	// SWR-meter
	KI0 = 5, KI1 = 6, KI2 = 7	// ����������
};
#define KI_COUNT 3	// ���������� ������������ ��� ���������� ������ ���
#define KI_LIST	KI2, KI1, KI0,	// �������������� ��� ������� �������������


#endif /* ATMEGA_CTL_UA3DKC_H_INCLUDED */
