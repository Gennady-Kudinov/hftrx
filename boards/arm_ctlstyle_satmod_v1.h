/* $Id$ */
/* satmod9_v0 ARM board */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef ARM_CTLSTYLE_V9_H_INCLUDED
#define ARM_CTLSTYLE_V9_H_INCLUDED 1
	/* ������ ������������ - ����� ������������������ */
	#define DIRECT_50M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_125M0_X1		1	/* Board hardware configuration */
	#define FQMODEL_TRX8M		1	// ������ ������ �� 8 ���, 6 ���, 5.5 ��� � ������
	//#define FQMODEL_DCTRX		1	// ������ ��������������
	#define WITHNESTEDINTERRUPTS	1	/* ������������ ��� ������� real-time �����. */

	//#define BANDSELSTYLERE_NOTHING 1
	#define BANDSELSTYLERE_LOCONV32M_NLB 1
	// ����� ��
	//#define IF3_MODEL IF3_TYPE_9000
	#define IF3_MODEL IF3_TYPE_8868
	#define	MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */

	#define DEFPREAMPSTATE 	1	/* ��� �� ��������� ������� (1) ��� �������� (0) */

	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	//#define BOARD_DETECTOR_MUTE 0x02
	//#define BOARD_DETECTOR_SSB 0x00
	//#define BOARD_DETECTOR_AM 0x01
	//#define BOARD_DETECTOR_FM 0x03

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define BOARD_FILTER_0P5		0x00	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_1P8		0x01	/* 1.8 kHz filter - �� ����� ������ ������ */
	#define BOARD_FILTER_2P7		0x02	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_3P1		0x03	/* 3.1 or 2.75 kHz filter */

	#define WITHMODESETMIXONLY 1	/* CW/CWR/USB/LSB */

	#define ENCRES_DEFAULT ENCRES_128
	/* ��� ��������� � ������ ������������ ������� */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	#define IF3_FHAVE 	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	#define IF3_FMASKTX	(IF3_FMASK_3P1)

	// +++ �������� ��������, ������������� ��� ���� �� ATMega
	// --- �������� ��������, ������������� ��� ���� �� ATMega

	// +++ ������ �������� ������������ ������ �� ����������
	//#define KEYB_RA1AGG	1	/* ������������ ������ ��� �������, ���������� RA1AGG - ��� ������ �� ������ ������� ������ ���� �� ������ � ���� ���. */
	//#define KEYB_UA1CEI	1	/* ������������ ������ ��� UA1CEI */
	// --- ������ �������� ������������ ������ �� ����������

	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_S1D13781	1	/* ���������� 480*272 � ������������ Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - ����������� ����������� */
	#define LCDMODE_UC1608	1		/* ��������� 240*128 � ������������ UC1608.- ����������� */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - ����������� ����������� (��� ������� ������) */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������


	#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */
	#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	//#define WITHSLEEPTIMER	1	/* ��������� ��������� � ����� ����� �� ���������� ���������� ������� */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������

	#define CTLREGSTYLE_NOCTLREG 1
	#define WITHAGCMODENONE		1	/* �������� ��� �� ��������� */
	#define BOARD_AGCCODE_OFF	0	// stub
	#define WITHPREAMPATT2_6DB		1	// ���������� ��� � ������������� ������������ � ����������� 0 - 6 - 12 - 18 dB */
	#define WITHUSESDCARD 1			// ��������� ��������� SD CARD
	//#define WITHUSEAUDIOREC	1	// ������ ����� �� SD CARD

	/* ��� �� ������ �������� � ������ ������������ � ����������� */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// ��� �� ��� ������������� FM25040A - 5 �����, 512 ����
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_ATMEGA

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	#define NVRAM_TYPE NVRAM_TYPE_NOTHING	// ��� NVRAM
	#define HARDWARE_IGNORENONVRAM	1		// ������� �� ������ ��� ��� �������� NVRAM

	// End of NVRAM definitions section
	#define PLL1_TYPE PLL_TYPE_NONE		// ���� �������, ����� �� �����������. For zak user

/* ���������� ������� �� SPI ���� */
#define targetsdcard	SPI_CSEL0 	/* SD CARD */
#define targetnvram		SPI_CSEL1  	/* serial nvram */
#define targetlcd	SPI_CSEL2 	/* TFT over SPI line devices control */
#define targetctl1		SPI_CSEL6 	/* control register */
#define targetuc1608 0

#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 15�. */
// ���������� ������ ��� ����������.
enum 
{ 
	KI0 = 5, KI1 = 6, KI2 = 7, // ����������
};
#define	KI_COUNT 3
#define KI_LIST	KI2, KI1, KI0,	// �������������� ��� ������� �������������

#endif /* ARM_CTLSTYLE_V9_H_INCLUDED */
