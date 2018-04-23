/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"


#include "display.h"
#include "formats.h"

// �������� �������� ��� ����������� �� �������

static const char FLASHMEM 
	strFlashWide [] = "WID",
	strFlashNarrow [] = "NAR",

	strFlash0p3 [] = "0.3",
	strFlash0p5 [] = "0.5",
	strFlash1p0 [] = "1.0",
	strFlash1p5 [] = "1.5",
	strFlash1p8 [] = "1.8",
	strFlash2p0 [] = "2.0",
	strFlash2p1 [] = "2.1",
	strFlash2p4 [] = "2.4",
	strFlash2p7 [] = "2.7",
	strFlash3p0 [] = "3.0",
	strFlash3p1 [] = "3.1",
	strFlash6p0 [] = "6.0",
	strFlash7p8 [] = "7.8",
	strFlash8p0 [] = "8.0",
	strFlash9p0 [] = "9.0",
	strFlash10p0 [] = "10.",
	strFlash18p0 [] = "18.",
	strFlash17p0 [] = "17.",
	strFlash15p0 [] = "15.",
	strFlash120p0 [] = "120";


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_128)
// 
#define IF3LOWER 126600ul
#define IF3CWCENTER 128000ul
#define IF3UPPER 129400ul
#define IF3MIDDLE ((IF3LOWER + IF3UPPER) / 2)

#define LO4FREQ	128000ul		/* ������� BFO ��� ������ ������ ��� ��������� DDS */

#define IF3FREQBASE (IF3CWCENTER - IF3OFFS)

#if (LO2_SIDE != LOCODE_INVALID)

	// LO2 ����������� PLL ��� ����� ������ � �������� ����������
	static const lo2param_t lo2param =
	{
		{	LO2_PLL_N, LO2_PLL_N, },	// lo2n: { rx, tx }
		{	LO2_PLL_R, LO2_PLL_R, },	// lo2r: { rx, tx }
		IF3FREQBASE,
	};

#else

	static const lo2param_t lo2param =
	{
		{	0, 0, },	// lo2n: { rx, tx }
		{	1, 1, },	// lo2r: { rx, tx }
		IF3FREQBASE,
	};

#endif


#if (IF3_FMASK & IF3_FMASK_3P1)
filter_t fi_3p1 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_CWZ),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3LOWER - IF3FREQBASE),
	(uint16_t) (IF3UPPER - IF3FREQBASE),
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash3p1,	/* �������� ������� ��� ����������� */
};

	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_3p1_tx =
	{
		0,
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) (IF3LOWER - IF3FREQBASE),
		(uint16_t) (IF3UPPER - IF3FREQBASE),
		{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
		0x01,	/* ���� ������ ���������� */
		0,	/* 0 - ��������� "�����", 1 - "�������" */
		IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
		& lo2param,				/* lo2 parameters set index for this filter */
		strFlash3p1,	/* �������� ������� ��� ����������� */
	};
	#endif /* WITHTX && WITHSAMEBFO == 0 */
#endif


#if (IF3_FMASK & IF3_FMASK_0P5)
filter_t fi_0p5 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,	/* TX modes */
	(1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* ������� ������� ������� */
	(uint16_t) 0,
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_0P5),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash0p5,	/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_1P8)
filter_t fi_1p8 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,	/* TX modes */
	0,	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3LOWER - IF3FREQBASE),
	(uint16_t) (IF3UPPER - IF3FREQBASE),
	{ BOARD_FILTER_1P8, BOARD_FILTER_1P8, },					/* ��� ������ ������� ������� �� ����� ��������� (�������� �����������) */
	0 != (IF3_FHAVE & IF3_FMASK_1P8),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash1p8,	/* �������� ������� ��� ����������� */
};
#endif

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_128) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_200)

#define IF3FREQBASE (200000UL - IF3OFFS)
#define IF3CWCENTER 201000UL
#define LO4FREQ	200000UL		/* ������� BFO ��� ������ ������ ��� ��������� DDS */

#if (LO2_SIDE != LOCODE_INVALID)

	// LO2 ����������� PLL ��� ����� ������ � �������� ����������
	static const lo2param_t lo2param =
	{
		{	LO2_PLL_N, LO2_PLL_N, },	// lo2n
		{	LO2_PLL_R, LO2_PLL_R, },	// lo2r
		IF3FREQBASE,
	};

#else

	static const lo2param_t lo2param =
	{
		{	0, 0, },	// lo2n
		{	1, 1, },	// lo2r
		IF3FREQBASE,
	};

#endif

#if (IF3_FMASK & IF3_FMASK_3P1)
filter_t fi_3p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((200000 + 0) - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) ((200000 + 3700) - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash3p1,		/* �������� ������� ��� ����������� */
};
	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_3p1_tx =
	{
		0,
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) (200000 - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
		(uint16_t) (203700 - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
		{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
		0x01,	/* ���� ������ ���������� */
		0,	/* 0 - ��������� "�����", 1 - "�������" */
		IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
		& lo2param,				/* lo2 parameters set index for this filter */
		strFlash3p1,		/* �������� ������� ��� ����������� */
	};
	#endif
#endif

#if (IF3_FMASK & IF3_FMASK_1P8)
filter_t fi_1p8 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((200000 + 0) - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) ((200000 + 3700) - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_1P8, BOARD_FILTER_1P8, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash1p8,		/* �������� ������� ��� ����������� */
};
#endif


#if (IF3_FMASK & IF3_FMASK_2P7)
filter_t fi_2p7 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((200000 + 0) - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) ((200000 + 2700) - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash2p7,		/* �������� ������� ��� ����������� */
};
	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_2p7_tx =
	{
		0,
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) ((200000 + 0) - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
		(uint16_t) ((200000 + 2700) - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
		{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* ��� ������ ������� ������� �� ����� ��������� */
		0x01,		/* ���� ������ ���������� */
		0,	/* 0 - ��������� "�����", 1 - "�������" */
		IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
		& lo2param,				/* lo2 parameters set index for this filter */
		strFlash2p7,		/* �������� ������� ��� ����������� */
	};
	#endif
#endif

#if (IF3_FMASK & IF3_FMASK_0P5)
filter_t fi_0p5 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_CW),						/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),          /* ������� ������ ������ ����������� */
	(uint16_t) (0),						/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_0P5),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash0p5,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_6P0)

filter_t fi_6p0 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	0,
	(1U << MODE_AM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((200000 + 0) - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) ((200000 + 6000) - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	{ BOARD_FILTER_6P0, BOARD_FILTER_6P0, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_6P0),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash6p0,	/* �������� ������� ��� ����������� */
};

#endif

#endif	/* IF3_TYPE_200 */

#if CTLSTYLE_RA4YBO_V1

/*
1. � ��� ����� ���� ��� _3,1 ��� �� �����,������� �� ��� �������� �����.(��� 00)
2. � ��,�� ��� ������������� ������������ �� 455 ��� � �������� 3,0 ���(��� 10); 6,0���(��� 01) ;10,0���.(��� 11)
�� �������� ,�� �������������, � ������������� ��� ��������� ��� �� 500 ��� ������� 2,35 ��� 3,1 ���, � ��� � ���� �� ����.
�� �������� ,�� �������������,  � ��  ���� ��������� ����� 455 ��� ������� 6 ��� 10���.
*/

#define IF3FREQBASE (500000L - IF3OFFS)

#define IF3CWCENTER455 455000L
#define IF3FREQBASE455 (IF3CWCENTER455 - IF3OFFS)

static const lo2param_t lo2param500 =
{
	// 500 kHz parameters
	{	LO2_PLL_N500, LO2_PLL_N500, },	// lo2n
	{	LO2_PLL_R500, LO2_PLL_R500, },	// lo2r
	IF3FREQBASE,
};

static const lo2param_t lo2param455 =
{
	// 455 kHz parameters
	{	LO2_PLL_N455, LO2_PLL_N455, },	// lo2n
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	IF3FREQBASE455,
};

static const lo2param_t lo2paramWFM =
{
	// 10.7 MHz parameters
	{	0, 0, },	// lo2n
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	WFM_IF1,
};

static const filter_t fi_wfm =
{
	(1U << MODE_WFM),	/* rx: ������ �������� ��� ���� ������� */
	0,					/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_WFM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) 0,		/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	BOARD_FILTER_WFM,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	0,    	    /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2paramWFM,				/* lo2 parameters set index for this filter */
	strFlash120p0,		/* �������� ������� ��� ����������� */
};

filter_t fi_3p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),		/* RX mask */
	0,									/* tx: ������ �������� ��� ���� ������� */
	0, //(1U << MODE_AM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	0x03,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash3p0,	/* �������� ������� ��� ����������� */
};

filter_t fi_6p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	(1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_TUNE),	/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_AM) | (1U << MODE_NFM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	0x01,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash6p0,	/* �������� ������� ��� ����������� */
};

filter_t fi_10p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	0,									/* TX mask: ������ �������� ��� ���� ������� */
	0, //(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	0x02,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash10p0,	/* �������� ������� ��� ����������� */
};

// 500 kHz
filter_t fi_3p1 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (500000L - IF3FREQBASE),
	(uint16_t) (503700L - IF3FREQBASE),
	0x00,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash3p1,	/* �������� ������� ��� ����������� */
};

// 500 kHz
filter_t fi_3p1_tx =
{
	0, //(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* tx: ������ �������� ��� ���� ������� */
	0, //(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (500000L - IF3FREQBASE),
	(uint16_t) (503700L - IF3FREQBASE),
	0x00,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash3p1,	/* �������� ������� ��� ����������� */
};

#endif /* CTLSTYLE_RA4YBO_V1 */

#if CTLSTYLE_RA4YBO_V2

/*
1. � ��� ����� ���� ��� _3,1 ��� �� �����,������� �� ��� �������� �����.(��� 00)
2. � ��,�� ��� ������������� ������������ �� 455 ��� � �������� 3,0 ���(��� 10); 6,0���(��� 01) ;10,0���.(��� 11)
�� �������� ,�� �������������, � ������������� ��� ��������� ��� �� 500 ��� ������� 2,35 ��� 3,1 ���, � ��� � ���� �� ����.
�� �������� ,�� �������������,  � ��  ���� ��������� ����� 455 ��� ������� 6 ��� 10���.
*/

#define IF3FREQBASE (500000L - IF3OFFS)

#define IF3CWCENTER455 455000L
#define IF3FREQBASE455 (IF3CWCENTER455 - IF3OFFS)

static const lo2param_t lo2param500 =
{
	// 500 kHz parameters
	{	LO2_PLL_N500, LO2_PLL_N500, },	// lo2n
	{	LO2_PLL_R500, LO2_PLL_R500, },	// lo2r
	IF3FREQBASE,
};

static const lo2param_t lo2param455 =
{
	// 455 kHz parameters
	{	LO2_PLL_N455, LO2_PLL_N455, },	// lo2n
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	IF3FREQBASE455,
};

static const lo2param_t lo2paramWFM =
{
	// 10.7 MHz parameters
	{	0, 0, },	// lo2n
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	WFM_IF1,
};


static const filter_t fi_wfm =
{
	(1U << MODE_WFM),	/* rx: ������ �������� ��� ���� ������� */
	0,					/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_WFM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) 0,		/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	0x07,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	0,    	    /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2paramWFM,				/* lo2 parameters set index for this filter */
	strFlash120p0,		/* �������� ������� ��� ����������� */
};

filter_t fi_3p0_455 =
{
	(1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM),		/* RX mask */
	0,									/* tx: ������ �������� ��� ���� ������� */
	0, //(1U << MODE_AM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	0x01,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash3p0,	/* �������� ������� ��� ����������� */
};

filter_t fi_6p0_455 =
{
	(1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	(1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_TUNE),	/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_AM) | (1U << MODE_NFM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	0x02,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash6p0,	/* �������� ������� ��� ����������� */
};

filter_t fi_10p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	0,									/* TX mask: ������ �������� ��� ���� ������� */
	0, //(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	0x03,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash10p0,	/* �������� ������� ��� ����������� */
};

// 500 kHz
filter_t fi_3p1 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (500000L - IF3FREQBASE),
	(uint16_t) (503700L - IF3FREQBASE),
	0x00,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash3p1,	/* �������� ������� ��� ����������� */
};

#endif /* CTLSTYLE_RA4YBO_V2 */

#if CTLSTYLE_RA4YBO_V3

/*
1. � ��� ����� ���� ��� _3,1 ��� �� �����,������� �� ��� �������� �����.(��� 00)
2. � ��,�� ��� ������������� ������������ �� 455 ��� � �������� 3,0 ���(��� 10); 6,0���(��� 01) ;10,0���.(��� 11)
�� �������� ,�� �������������, � ������������� ��� ��������� ��� �� 500 ��� ������� 2,35 ��� 3,1 ���, � ��� � ���� �� ����.
�� �������� ,�� �������������,  � ��  ���� ��������� ����� 455 ��� ������� 6 ��� 10���.
*/

#define IF3FREQBASE (500000L - IF3OFFS)

#define IF3CWCENTER455 455000L
#define IF3FREQBASE455 (IF3CWCENTER455 - IF3OFFS)
#define IF3LOWER455 (IF3CWCENTER455-1500)	// (IF3CWCENTER - 1800L)
#define IF3UPPER455 (IF3CWCENTER455+1500)	// (IF3CWCENTER + 1800L)

static const lo2param_t lo2param500 =
{
	// 500 kHz parameters
	{	LO2_PLL_N500, LO2_PLL_N500, },	// lo2n
	{	LO2_PLL_R500, LO2_PLL_R500, },	// lo2r
	IF3FREQBASE,
};

static const lo2param_t lo2param455 =
{
	// 455 kHz parameters
	{	LO2_PLL_N455, LO2_PLL_N455, },	// lo2n
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	IF3FREQBASE455,
};

static const lo2param_t lo2paramWFM =
{
	// 10.7 MHz parameters
	{	LO2_PLL_N455, LO2_PLL_N455, },	// lo2n - �� ����, ����� �� �������� ���������
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	WFM_IF1,
};

static const filter_t fi_wfm =
{
	(1U << MODE_WFM),	/* rx: ������ �������� ��� ���� ������� */
	0,					/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_WFM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) 0,		/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	{ BOARD_FILTER_RX_WFM, BOARD_FILTER_OFF, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	0,    	    /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2paramWFM,				/* lo2 parameters set index for this filter */
	strFlash120p0,		/* �������� ������� ��� ����������� */
};

// mechanical filter emf-500-0.6-S - RX only
filter_t fi_0p5 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,																			/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (500000L - IF3FREQBASE),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	{ BOARD_FILTER500_RX_0P5, BOARD_FILTER_OFF, },					/* ��� ������ ������� ������� �� ����� ��������� */
	1,			/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,			/* lo2 parameters set index for this filter */
	strFlash0p5,		/* �������� ������� ��� ����������� */
};

// mechanical 500 kHz emf-500-3.1-N	- RX/TX 
filter_t fi_3p1 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_CW) | (1U << MODE_SSB) | (1U << MODE_TUNE),
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (496610L - IF3FREQBASE),
	(uint16_t) (500000L - IF3FREQBASE),
	{ BOARD_FILTER500_RX_3P1, BOARD_FILTER500_TX_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash3p1,	/* �������� ������� ��� ����������� */
};

// collins filter - RX only
// 2.4kHz filter
filter_t fi_3p0_455 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),		/* RX mask */
	0,	/* tx: ������ �������� ��� ���� ������� */
	0, //(1U << MODE_AM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3LOWER455 - IF3FREQBASE455),
	(uint16_t) (IF3UPPER455 - IF3FREQBASE455),
	{ BOARD_FILTER455_RX_3P0, BOARD_FILTER_OFF, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash2p4,	/* �������� ������� ��� ����������� */
};

// piezo filter - RX/TX
filter_t fi_10p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	(1U << MODE_AM) | (1U << MODE_NFM),									/* TX mask: ������ �������� ��� ���� ������� */
	0, //(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	{ BOARD_FILTER455_RX_10P0, BOARD_FILTER455_TX_10P0, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash10p0,	/* �������� ������� ��� ����������� */
};

// piezo filter - RX only
filter_t fi_6p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	0,	/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_AM) | (1U << MODE_NFM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	{ BOARD_FILTER455_RX_6P0, BOARD_FILTER_OFF, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash6p0,	/* �������� ������� ��� ����������� */
};

// piezo filter - RX only
filter_t fi_2p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	0,											/* tx: ������ �������� ��� ���� ������� */
	0,	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	{ BOARD_FILTER455_RX_2P1, BOARD_FILTER_OFF, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash2p0,	/* �������� ������� ��� ����������� */
};


#endif /* CTLSTYLE_RA4YBO_V3 */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_500)

#define IF3MIDDLE 500000L
#if BOARD_FILTER_0P5_LSB
	#define IF3CWCENTER 499650L
#else /* BOARD_FILTER_0P5_LSB */
	#define IF3CWCENTER 500000L
#endif /* BOARD_FILTER_0P5_LSB */
#define IF3LOWER 500000L
#define IF3UPPER 503700L
#define IF3FREQBASE (IF3MIDDLE - IF3OFFS)
#define LO4FREQ	IF3LOWER		/* ������� BFO ��� ������ ������ ��� ��������� DDS */

#if CTLSTYLE_RA4YBO

static const lo2param_t lo2param500 =
{
	{	LO2_PLL_N_RX, LO2_PLL_N_TX, },	// lo2n
	{	LO2_PLL_R_RX, LO2_PLL_R_TX, },	// lo2r
	IF3FREQBASE,
};

static const lo2param_t lo2paramWFM =
{
	{	0, 0, },						// lo2n
	{	LO2_PLL_R_RX, LO2_PLL_R_TX, },	// lo2r
	WFM_IF1,
};

static const filter_t fi_wfm =
{
	(1U << MODE_WFM),	/* rx: ������ �������� ��� ���� ������� */
	0,					/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_WFM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) 0,		/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	BOARD_FILTER_WFM,					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	0,    	    /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2paramWFM,	/* lo2 parameters set index for this filter */
	strFlash120p0,		/* �������� ������� ��� ����������� */
};

//������ � ���� ������ ����� �� �����:0,3�_0,6� _1,5� _2,35�_3,1�_7,8�_17,0�

// 500-0.3-s
filter_t fi_0p3 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,		// Set of TX modes
	0, //(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	{ BOARD_FILTER_0P3, BOARD_FILTER_0P3, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash0p3,	/* �������� ������� ��� ����������� */
};
// 500-0.6-s
filter_t fi_0p5 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,		// Set of TX modes
	(1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash0p5,	/* �������� ������� ��� ����������� */
};
// 500-1.0-s
filter_t fi_1p5 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_SSB),
	0,		// Set of TX modes
	0, //(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	{ BOARD_FILTER_1P5, BOARD_FILTER_1P5, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash1p5,	/* �������� ������� ��� ����������� */
};


// 500-2.35-V
filter_t fi_2p4 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,		// Set of TX modes
	0, //(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (500100UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (503050UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_2P4, BOARD_FILTER_2P4, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash2p4,	/* �������� ������� ��� ����������� */
};

// 500-3.1-N
filter_t fi_3p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	// Set of TX modes
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (496400UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (500200UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash3p1,		/* �������� ������� ��� ����������� */
};


// 500-7.8-s
filter_t fi_7p8 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	0,		// Set of TX modes
	(1U << MODE_AM) | (1U << MODE_NFM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) - (7800 / 2)),	/* ������� ������� ����� ������ ����������� */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) + (7800 / 2)),	/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_7P8, BOARD_FILTER_7P8, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash7p8,	/* �������� ������� ��� ����������� */
};
// 500-17.0-s
filter_t fi_17p0 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	0,		// Set of TX modes
	0, //(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) - (17000 / 2)),	/* ������� ������� ����� ������ ����������� */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) + (17000 / 2)),	/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_17P0, BOARD_FILTER_17P0, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash17p0,	/* �������� ������� ��� ����������� */
};

// 500-3.1-v
	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_3p1_tx =
	{
		0, //(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0, //(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
		(uint16_t) (496400UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
		(uint16_t) (500200UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
		{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
		0x01,	/* ���� ������ ���������� */
		0,	/* 0 - ��������� "�����", 1 - "�������" */
		IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
		& lo2param500,	/* lo2 parameters set index for this filter */
		strFlash3p1,		/* �������� ������� ��� ����������� */
	};
	#endif
// 500-17.0-s
filter_t fi_17p0_tx =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),		// Set of TX modes
	0, //(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) - (17000 / 2)),	/* ������� ������� ����� ������ ����������� */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) + (17000 / 2)),	/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_17P0, BOARD_FILTER_17P0, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash17p0,	/* �������� ������� ��� ����������� */
};


#else /* CTLSTYLE_RA4YBO */


static const lo2param_t lo2param =
{
	{	LO2_PLL_N, LO2_PLL_N, },	// lo2n
	{	LO2_PLL_R, LO2_PLL_R, },	// lo2r
	IF3FREQBASE,
};

	// filter codes:
	// 0x06 - fil3 3.1 kHz
	// 0x01 - fil2 2.7 kHz
	// 0x00 - fil0 9.0 kHz
	// 0x02 - fil1 6.0 kHz
	// 0x03 - fil5 0.5 kHz
	// 0x04 - fil2 15 kHz (bypass)
	// 0x05 - unused
	// 0x07 - unused

#if (IF3_FMASK & IF3_FMASK_2P7)
// 500-2.7-n
filter_t fi_2p7 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM), // Set of TX modes
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (496800UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (500300UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_2P7),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash2p7,		/* �������� ������� ��� ����������� */
};

// 500-2.7-n

	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_2p7_tx =
	{
		0,
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) (496800UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
		(uint16_t) (500300UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
		{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* ��� ������ ������� ������� �� ����� ��������� */
		0x01,	/* ���� ������ ���������� */
		0,	/* 0 - ��������� "�����", 1 - "�������" */
		IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
		& lo2param,			/* lo2 parameters set index for this filter */
		strFlash2p7,		/* �������� ������� ��� ����������� */
	};
	#endif
#endif


#if (IF3_FMASK & IF3_FMASK_3P1)

// 500-3-v
filter_t fi_3p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	#if BOARD_FILTER_3P1_LSB
	(uint16_t) (496300UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (500000UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	#else /* BOARD_FILTER_3P1_LSB */
	(uint16_t) (500000UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (503700UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	#endif /* BOARD_FILTER_3P1_LSB */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash3p1,		/* �������� ������� ��� ����������� */
};
	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_3p1_tx =
	{
		0,
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		#if BOARD_FILTER_3P1_LSB
		(uint16_t) (496300UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
		(uint16_t) (500000UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
		#else /* BOARD_FILTER_3P1_LSB */
		(uint16_t) (500000UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
		(uint16_t) (503700UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
		#endif /* BOARD_FILTER_3P1_LSB */
		{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
		0x01,	/* ���� ������ ���������� */
		0,	/* 0 - ��������� "�����", 1 - "�������" */
		IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
		& lo2param,			/* lo2 parameters set index for this filter */
		strFlash3p1,		/* �������� ������� ��� ����������� */
	};
	#endif
#endif


#if (IF3_FMASK & IF3_FMASK_0P3)
// 500-0.5-v
filter_t fi_0p3 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ),
	0,
	(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	{ BOARD_FILTER_0P3, BOARD_FILTER_0P3, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_0P3),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash0p3,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_0P5)
// 500-0.5-v
filter_t fi_0p5 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ),
	0,
	(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* ����������� ������� ������ ����������� */
	(uint16_t) 0,				
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_0P5),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash0p5,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_2P4)
// ������������ 2.7 ��� ������ ������ 6 ���
// 500-2.4-n
filter_t fi_2p4 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (497100UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (500000UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_2P4, BOARD_FILTER_2P4, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_2P4),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash2p4,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_1P8)
filter_t fi_1p8 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	0,					/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (497100UL - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (500000UL - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_1P8, BOARD_FILTER_1P8, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_1P8),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash1p8,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_6P0)
filter_t fi_6p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_DRM) | (1U << MODE_NFM) | (1U << MODE_AM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3MIDDLE - 3500L - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (IF3MIDDLE + 3500L - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_6P0, BOARD_FILTER_6P0, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x00,	/* ��� 500 ��� �� ���� ������ ������ �� ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash6p0,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_8P0)
filter_t fi_8p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_DRM) | (1U << MODE_NFM) | (1U << MODE_AM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3MIDDLE - 4500L - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (IF3MIDDLE + 4500L - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_8P0, BOARD_FILTER_8P0, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_8P0),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash8p0,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_9P0)
filter_t fi_9p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_DRM) | (1U << MODE_NFM) | (1U << MODE_AM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3MIDDLE - 5000L - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (IF3MIDDLE + 5000L - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_9P0, BOARD_FILTER_9P0, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_9P0),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash9p0,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_15P0)
// �� ������ ����� � 45 ��� �������� ���� ��������� ����������� ��� FM ������ (������ � ������ �����).
// ��� 500 ��� ������ ������ ��
filter_t fi_15p0_nfm =
{
	(1U << MODE_NFM),
	0,
	(1U << MODE_NFM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3MIDDLE - 7500L - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (IF3MIDDLE + 7500L - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_15P0_NFM, BOARD_FILTER_15P0_NFM, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_15P0),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash15p0,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_15P0)
filter_t fi_15p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_DRM) | (1U << MODE_NFM) | (1U << MODE_AM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3MIDDLE - 7500L - IF3FREQBASE),          /* ������� ������� ����� ������ ����������� */
	(uint16_t) (IF3MIDDLE + 7500L - IF3FREQBASE),		/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_15P0, BOARD_FILTER_15P0, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_15P0),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash15p0,		/* �������� ������� ��� ����������� */
};
#endif

#endif /* CTLSTYLE_RA4YBO */

#endif/* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_500s) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_455)

	#define IF3_CUSTOM_CW_CENTER	(455000L)
	#define IF3_CUSTOM_SSB_LOWER	(455000L - (3700 / 2))
	#define IF3_CUSTOM_SSB_UPPER	(455000L + (3700 / 2))
	#define IF3_CUSTOM_WIDE_CENTER	(455000L)		/* ����������� ������� ��� ������� �������� � passtrough */
	

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_455) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_215)

	#define IF3_CUSTOM_CW_CENTER	215000L
	#define IF3_CUSTOM_SSB_LOWER	213150L
	#define IF3_CUSTOM_SSB_UPPER	216850L
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_215) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_9045)

	#define IF3_CUSTOM_CW_CENTER	9045000L
	#define IF3_CUSTOM_SSB_LOWER	(IF3_CUSTOM_CW_CENTER - 1500)
	#define IF3_CUSTOM_SSB_UPPER	(IF3_CUSTOM_CW_CENTER + 1500)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_9045) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5000)

	#define IF3_CUSTOM_CW_CENTER	4998404L	//4999404L
	#define IF3_CUSTOM_SSB_LOWER	4997000L	//4999758L
	#define IF3_CUSTOM_SSB_UPPER	5000000L	//5002638L
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5000) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5645)

	#define IF3_CUSTOM_CW_CENTER	5645000L
	#define IF3_CUSTOM_SSB_LOWER	(IF3_CUSTOM_CW_CENTER - 1800)
	#define IF3_CUSTOM_SSB_UPPER	(IF3_CUSTOM_CW_CENTER + 1800)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5645) */


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000)

	#define IF3_CUSTOM_CW_CENTER	5999400L
	#define IF3_CUSTOM_SSB_LOWER	5999300L	// 5999550L
	#define IF3_CUSTOM_SSB_UPPER	6003200L	// 6002650L
	#define IF3_CUSTOM_WIDE_CENTER	6000000L	// 6 kHz filter center freq

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2015)

	#define IF3_CUSTOM_CW_CENTER	5999400L
	#define IF3_CUSTOM_SSB_LOWER	5999300L	// 5999550L
	#define IF3_CUSTOM_SSB_UPPER	6002400L	// 6002650L
	#define IF3_CUSTOM_WIDE_CENTER	6000000L	// 6 kHz filter center freq

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2015) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2016MINI)

	#define IF3_CUSTOM_CW_CENTER	5999400L
	#define IF3_CUSTOM_SSB_LOWER	5999500L
	#define IF3_CUSTOM_SSB_UPPER	6002500L
	#define IF3_CUSTOM_WIDE_CENTER	6000000L	// 6 kHz filter center freq

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2015) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2014NFM)

	#define IF3_CUSTOM_CW_CENTER	5999400L
	#define IF3_CUSTOM_SSB_LOWER	5999300L
	#define IF3_CUSTOM_SSB_UPPER	6002300L
	#define IF3_CUSTOM_WIDE_CENTER	6000000L	// 6 kHz filter center freq

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2014NFM) */


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5500)

	#define IF3_CUSTOM_CW_CENTER	(5500000L-1500)
	#define IF3_CUSTOM_SSB_LOWER	(IF3_CUSTOM_CW_CENTER - 1500)
	#define IF3_CUSTOM_SSB_UPPER	(IF3_CUSTOM_CW_CENTER + 1500)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5500) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5250)

	#define IF3_CUSTOM_CW_CENTER	(5250000L-1500)
	#define IF3_CUSTOM_SSB_LOWER	(IF3_CUSTOM_CW_CENTER - 1500)
	#define IF3_CUSTOM_SSB_UPPER	(IF3_CUSTOM_CW_CENTER + 1500)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5250) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_9000)

	#define IF3_CUSTOM_CW_CENTER	9000000L
	#define IF3_CUSTOM_SSB_LOWER	8998500L // (IF3_CUSTOM_CW_CENTER - 1500)
	#define IF3_CUSTOM_SSB_UPPER	9001500L // (IF3_CUSTOM_CW_CENTER + 1500)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_9000) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8192)

	#define IF3_CUSTOM_CW_CENTER	8191000L
	#define IF3_CUSTOM_SSB_LOWER	8190350L //  8190350 - info from 4Z5KY
	#define IF3_CUSTOM_SSB_UPPER	8193850L
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8192) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8215)
// ��������� ������� ��  ���������� FT-747
	#define IF3_CUSTOM_CW_CENTER	8215000L
	#define IF3_CUSTOM_SSB_LOWER	(IF3_CUSTOM_CW_CENTER - 1500L)
	#define IF3_CUSTOM_SSB_UPPER	(IF3_CUSTOM_CW_CENTER + 1500L)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8215) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8868)
	// ����� 36.131.00   LSB-8865850 USB-8868700

	#define IF3DF (0)
	//#define IF3DF (8862000L - 8865850L)
	//#define IF3DF (8191000L - 8865850L)
	//#define IF3DF (8975000L - 8865850L)
	//#define IF3DF (8822000L - 8865850L)	// Version for RU3VG

	#define IF3_CUSTOM_CW_CENTER	(8866500L + IF3DF)
	#define IF3_CUSTOM_SSB_LOWER	(8865850L + IF3DF)
	#define IF3_CUSTOM_SSB_UPPER	(8868700L + IF3DF)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8868) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8000)
	// hansgo_1969@mail.ru: ����� ������� SSB 7997,184 - 7999,930. CW - 7997,557 - 7998,126.

	#define IF3_CUSTOM_CW_CENTER	(7997840L)
	#define IF3_CUSTOM_SSB_LOWER	(7997184L)
	#define IF3_CUSTOM_SSB_UPPER	(7999930L)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8000) */


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_10000)

	#define IF3_CUSTOM_CW_CENTER	(9998500L)
	#define IF3_CUSTOM_SSB_LOWER	(9996700L)
	#define IF3_CUSTOM_SSB_UPPER	(10000200L)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_10000) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_10700)

	#define IF3_CUSTOM_CW_CENTER	(10700000L)
	#define IF3_CUSTOM_SSB_LOWER	(10700000L - (3100/2))
	#define IF3_CUSTOM_SSB_UPPER	(10700000L + (3100/2))
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_10700) */


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_CUSTOM)

#if ! defined (IF3_CUSTOM_WIDE_CENTER) || ! defined (IF3_CUSTOM_CW_CENTER) || ! defined (IF3_CUSTOM_SSB_LOWER) || ! defined (IF3_CUSTOM_SSB_UPPER)
	#error Please define IF3_CUSTOM_WIDE_CENTER, IF3_CUSTOM_CW_CENTER, IF3_CUSTOM_SSB_LOWER and IF3_CUSTOM_SSB_UPPER
#endif

#define IF3WIDECENTER IF3_CUSTOM_WIDE_CENTER
#define IF3CWCENTER IF3_CUSTOM_CW_CENTER
#define IF3LOWER IF3_CUSTOM_SSB_LOWER	// (IF3CWCENTER - 1800L)
#define IF3UPPER IF3_CUSTOM_SSB_UPPER	// (IF3CWCENTER + 1800L)

#define LO4FREQ	IF3LOWER		/* ������� BFO ��� ������ ������ ��� ��������� DDS */

#define IF3FREQBASE (IF3CWCENTER - IF3OFFS)
#define IF3CWCENTER455 (45000000 - 455000)
#define IF3FREQBASE455 (IF3CWCENTER455 - IF3OFFS)



#if (LO2_SIDE != LOCODE_INVALID)

	// LO2 ����������� PLL ��� ����� ������ � �������� ����������
	static const lo2param_t lo2param =
	{
		{	LO2_PLL_N, LO2_PLL_N, },	// lo2n: { rx, tx }
		{	LO2_PLL_R, LO2_PLL_R, },	// lo2r: { rx, tx }
		IF3FREQBASE,
	};

	#if WITHDEDICATEDNFM

	static const lo2param_t lo2param455 =
	{
		// 455 kHz parameters
		{	455000, 455000, },	// lo2n: { rx, tx }
		{	REFERENCE_FREQ, REFERENCE_FREQ, },	// lo2r: { rx, tx }
		IF3FREQBASE455,
	};
	#endif

#else

	static const lo2param_t lo2param =
	{
		{	0, 0, },	// lo2n
		{	1, 1, },	// lo2r
		IF3FREQBASE,
	};

	static const lo2param_t lo2param455 =
	{
		{	0, 0, },	// lo2n
		{	1, 1, },	// lo2r
		IF3FREQBASE455,
	};


#endif

#if (IF3_FMASK & IF3_FMASK_0P5)
filter_t fi_0p5 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,																			/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* ������� ������� ������� */
	(uint16_t) 0,
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_0P5),	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash0p5,	/* �������� ������� ��� ����������� */
};
#endif /* (IF3_FMASK & IF3_FMASK_0P5) */

#if (IF3_FMASK & IF3_FMASK_1P8)
filter_t fi_1p8 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,																			/* tx: ������ �������� ��� ���� ������� */
	0,	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3LOWER - IF3FREQBASE),
	(uint16_t) (IF3UPPER - IF3FREQBASE),
	{ BOARD_FILTER_1P8, BOARD_FILTER_1P8, },					/* ��� ������ ������� ������� �� ����� ��������� (�������� �����������) */
	0 != (IF3_FHAVE & IF3_FMASK_1P8),	/* ���� ������ ���������� */
	1,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash1p8,	/* �������� ������� ��� ����������� */
};
#endif /* (IF3_FMASK & IF3_FMASK_1P8) */

#if (IF3_FMASK & IF3_FMASK_2P7)
filter_t fi_2p7 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_CWZ),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3LOWER - IF3FREQBASE),
	(uint16_t) (IF3UPPER - IF3FREQBASE),
	{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_2P7),	/* ���� ������ ���������� */
	1,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash2p7,	/* �������� ������� ��� ����������� */
};
#endif /* (IF3_FMASK & IF3_FMASK_2P7) */

#if (IF3_FMASK & IF3_FMASK_3P1)
filter_t fi_3p1 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* rx: ������ �������� ��� ���� ������� */
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_CWZ),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) (IF3LOWER - IF3FREQBASE),
	(uint16_t) (IF3UPPER - IF3FREQBASE),
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* ���� ������ ���������� */
	1,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash3p1,	/* �������� ������� ��� ����������� */
};
#endif /* (IF3_FMASK & IF3_FMASK_2P7) */

#if (IF3_FMASK & IF3_FMASK_6P0)

// ceramic filter
filter_t fi_6p0 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM)  | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,						/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_AM) | (1U << MODE_NFM)  | (1U << MODE_DRM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) - (6000/2)),	/* ������� ������� ����� ������ ����������� */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) + (6000/2)),	/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_6P0, BOARD_FILTER_6P0, },				/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_6P0),	/* ���� ������ ���������� */
	1,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash6p0,	/* �������� ������� ��� ����������� */
};
#endif


#if (IF3_FMASK & IF3_FMASK_8P0)

// passtrough filter
filter_t fi_8p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_DRM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,						/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_DRM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) - (8000/2)),	/* ������� ������� ����� ������ ����������� */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) + (8000/2)),	/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_8P0, BOARD_FILTER_8P0, },				/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_8P0),	/* ���� ������ ���������� */
	1,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash8p0,	/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_9P0)
// ceramic filter
filter_t fi_9p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM)  | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,						/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_AM) | (1U << MODE_NFM)  | (1U << MODE_DRM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) - (9000/2)),	/* ������� ������� ����� ������ ����������� */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) + (9000/2)),	/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_9P0, BOARD_FILTER_9P0, },				/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_6P0),	/* ���� ������ ���������� */
	1,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash9p0,	/* �������� ������� ��� ����������� */
};
#endif


#if (IF3_FMASK & IF3_FMASK_15P0)
// passtrough filter
filter_t fi_15p0 =
{
	0, //(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,						/* tx: ������ �������� ��� ���� ������� */
	0, //(1U << MODE_DRM),	/* ������ "�� ���������" ��� ���� ������� */
	(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) - (15000/2)),	/* ������� ������� ����� ������ ����������� */
	(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) + (15000/2)),	/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_15P0, BOARD_FILTER_15P0, },				/* ��� ������ ������� ������� �� ����� ��������� */
	0 != (IF3_FHAVE & IF3_FMASK_15P0),	/* ���� ������ ���������� */
	1,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param455,		/* lo2 parameters set index for this filter */
	strFlash15p0,	/* �������� ������� ��� ����������� */
};

	#if WITHDEDICATEDNFM /* ��� NFM ��������� ����� �� ����� �������� */ 

	// passtrough filter
	filter_t fi_15p0_nfm =
	{
		(1U << MODE_NFM),		/* rx: ������ �������� ��� ���� ������� */
		0,						/* tx: ������ �������� ��� ���� ������� */
		(1U << MODE_NFM),	/* ������ "�� ���������" ��� ���� ������� */
		(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) - (15000/2)),	/* ������� ������� ����� ������ ����������� */
		(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) + (15000/2)),	/* ������� �������� ����� ������ ����������� */
		{ BOARD_FILTER_15P0_NFM, BOARD_FILTER_15P0_NFM, },				/* ��� ������ ������� ������� �� ����� ��������� */
		0 != (IF3_FHAVE & IF3_FMASK_15P0),	/* ���� ������ ���������� */
		1,	/* 0 - ��������� "�����", 1 - "�������" */
		IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
		& lo2param455,		/* lo2 parameters set index for this filter */
		strFlash15p0,	/* �������� ������� ��� ����������� */
	};
	// passtrough filter
		#if (IF3_FMASKTX & IF3_FMASK_15P0)
		filter_t fi_15p0_tx_nfm =
		{
			0,											/* rx: ������ �������� ��� ���� ������� */
			(1U << MODE_NFM),							/* tx: ������ �������� ��� ���� ������� */
			(1U << MODE_NFM),	/* ������ "�� ���������" ��� ���� ������� */
			(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) - (15000/2)),	/* ������� ������� ����� ������ ����������� */
			(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) + (15000/2)),	/* ������� �������� ����� ������ ����������� */
			{ BOARD_FILTER_15P0_NFM, BOARD_FILTER_15P0_NFM, },				/* ��� ������ ������� ������� �� ����� ��������� */
			0 != (IF3_FHAVE & IF3_FMASK_15P0),	/* ���� ������ ���������� */
			1,	/* 0 - ��������� "�����", 1 - "�������" */
			IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
			& lo2param455,		/* lo2 parameters set index for this filter */
			strFlash15p0,	/* �������� ������� ��� ����������� */
		};
		#endif /* (IF3_FMASKTX & IF3_FMASK_15P0) */
	#endif /* WITHDEDICATEDNFM */
#endif


#if WITHTX && WITHSAMEBFO == 0
	#if (IF3_FMASKTX & IF3_FMASK_2P7)
	filter_t fi_2p7_tx =
	{
		0,											/* rx: ������ �������� ��� ���� ������� */
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) (IF3LOWER - IF3FREQBASE),
		(uint16_t) (IF3UPPER - IF3FREQBASE),
		{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* ��� ������ ������� ������� �� ����� ��������� */
		0x01,	/* ���� ������ ���������� */
		1,	/* 0 - ��������� "�����", 1 - "�������" */
		IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
		& lo2param,		/* lo2 parameters set index for this filter */
		strFlash2p7,	/* �������� ������� ��� ����������� */
	};
	#endif /* (IF3_FMASK & IF3_FMASK_2P7) */
	#if (IF3_FMASKTX & IF3_FMASK_3P1)
	filter_t fi_3p1_tx =
	{
		0,											/* rx: ������ �������� ��� ���� ������� */
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) (IF3LOWER - IF3FREQBASE),
		(uint16_t) (IF3UPPER - IF3FREQBASE),
		{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
		0x01,	/* ���� ������ ���������� */
		1,	/* 0 - ��������� "�����", 1 - "�������" */
		IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
		& lo2param,		/* lo2 parameters set index for this filter */
		strFlash3p1,	/* �������� ������� ��� ����������� */
	};
	#endif /* (IF3_FMASK & IF3_FMASK_3P1) */
#endif /* WITHTX && WITHSAMEBFO == 0 */

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_CUSTOM) */


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX)

#define LO4FREQ 0
#define IF3FREQBASE (LO4FREQ - IF3OFFS)

#if (LO2_SIDE != LOCODE_INVALID)

	// LO2 ����������� PLL ��� ����� ������ � �������� ����������
	static const lo2param_t lo2param =
	{
		{	LO2_PLL_N, LO2_PLL_N, },	// lo2n
		{	LO2_PLL_R, LO2_PLL_R, },	// lo2r
		IF3FREQBASE,
	};

#else

	static const lo2param_t lo2param =
	{
		{	0, 0, },	// lo2n
		{	1, 1, },	// lo2r
		IF3FREQBASE,
	};

#endif

#if (IF3_FMASK & IF3_FMASK_3P1)
filter_t fi_3p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,															/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	0,					/* ������� ������� ����� ������ ����������� */
	0,					/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
	1,	/* ���� ������ ���������� */
	1,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash3p1,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_2P1)
filter_t fi_2p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,																			/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	0,					/* ������� ������� ����� ������ ����������� */
	0,					/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_2P1, BOARD_FILTER_2P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
	1,	/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash2p1,		/* �������� ������� ��� ����������� */
};
#endif

#if (IF3_FMASK & IF3_FMASK_0P5)
filter_t fi_0p5 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ)  | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,																			/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	0,/* ������� ������� ����� ������ ����������� */
	0,	/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* ��� ������ ������� ������� �� ����� ��������� */
	1,			/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash0p5,		/* �������� ������� ��� ����������� */
};
#endif

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_BYPASS)

#define LO4FREQ 0
#define IF3FREQBASE (LO4FREQ - IF3OFFS)

#if (LO2_SIDE != LOCODE_INVALID)

	// LO2 ����������� PLL ��� ����� ������ � �������� ����������
	static const lo2param_t lo2param =
	{
		{	LO2_PLL_N, LO2_PLL_N, },	// lo2n
		{	LO2_PLL_R, LO2_PLL_R, },	// lo2r
		IF3FREQBASE,
	};

#else

	static const lo2param_t lo2param =
	{
		{	0, 0, },	// lo2n
		{	1, 1, },	// lo2r
		IF3FREQBASE,
	};

#endif

#if (IF3_FMASK & IF3_FMASK_3P1)
filter_t fi_3p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_DRM) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM),		/* TX mask */
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* ������ "�� ���������" ��� ���� ������� */
	IF3OFFS,					/* ������� ������� ����� ������ ����������� */
	IF3OFFS,					/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
	1,	/* ���� ������ ���������� */
	1,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlashWide,		/* �������� ������� ��� ����������� */
};

filter_t fi_3p1_tx =
{
	0,											/* rx: ������ �������� ��� ���� ������� */
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	IF3OFFS,					/* ������� ������� ����� ������ ����������� */
	IF3OFFS,					/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* ��� ������ ������� ������� �� ����� ��������� */
	0x01,	/* ���� ������ ���������� */
	1,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash3p1,	/* �������� ������� ��� ����������� */
};

#endif

#if (IF3_FMASK & IF3_FMASK_0P5)
filter_t fi_0p5 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_RTTY) | (1U << MODE_DIGI),						/* TX mask */
	(1U << MODE_CW),	/* ������ "�� ���������" ��� ���� ������� */
	IF3OFFS,					/* ������� ������� ����� ������ ����������� */
	IF3OFFS,					/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* ��� ������ ������� ������� �� ����� ��������� */
	1,			/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlashNarrow,		/* �������� ������� ��� ����������� */
};
#endif

#if WITHWFM
static const filter_t fi_wfm =
{
	(1U << MODE_WFM),	/* rx: ������ �������� ��� ���� ������� */
	0,					/* tx: ������ �������� ��� ���� ������� */
	(1U << MODE_WFM),	/* ������ "�� ���������" ��� ���� ������� */
	IF3OFFS,					/* ������� ������� ����� ������ ����������� */
	IF3OFFS,					/* ������� �������� ����� ������ ����������� */
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* ��� ������ ������� ������� �� ����� ��������� */
	1,			/* ���� ������ ���������� */
	0,	/* 0 - ��������� "�����", 1 - "�������" */
	IF3CEOFFS,        /*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash120p0,		/* �������� ������� ��� ����������� */
};
#endif /* WITHWFM */

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_BYPASS) */

/* ������ ��������, ��������� � ��������� */
#if (IF3_FMASK == 0)
	#error IF3_FMASK error
#endif
static const filter_t * const FLASHMEM gfa [] =
{
#if CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2
		& fi_3p0_455,
		& fi_3p1,
		& fi_6p0_455,
		& fi_10p0_455,
#elif CTLSTYLE_RA4YBO_V3
		& fi_0p5,
		& fi_3p1,
		& fi_3p0_455,
		& fi_2p0_455,
		& fi_6p0_455,
		& fi_10p0_455, // tx
#else /* CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2*/
#if (IF3_FMASK & IF3_FMASK_0P3)
		& fi_0p3,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_0P5)
		& fi_0p5,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_1P0)
		& fi_1p0,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_1P5)
		& fi_1p5,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_1P8)
		& fi_1p8,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_2P1)
		& fi_2p1,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_2P4)
		& fi_2p4,
	#if WITHTX && WITHSAMEBFO == 0
		& fi_2p4_tx,
	#endif /*  */
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_2P7)
		& fi_2p7,
	#if WITHTX && WITHSAMEBFO == 0
		& fi_2p7_tx,
	#endif /*  */
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_3P1)
		& fi_3p1,
	#if WITHTX && WITHSAMEBFO == 0
		& fi_3p1_tx,
	#endif /*  */
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_6P0)
		& fi_6p0,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_7P8)
		& fi_7p8,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_8P0)
		& fi_8p0,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_9P0)
		& fi_9p0,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_17P0)
		& fi_17p0,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_15P0)
		& fi_15p0,
		& fi_15p0_nfm,
	#if (IF3_FMASKTX & IF3_FMASK_15P0)
		& fi_15p0_tx_nfm,
	#endif /* (IF3_FMASKTX & IF3_FMASK_15P0) */
#endif /*  */
#endif /* CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 */
#if WITHWFM
		& fi_wfm,
#endif /* WITHWFM */
};

#if WITHTX
#if (IF3_FMASK & IF3_FMASKTX) == 0
	#error IF3_FMASKTX error
#endif

#endif /* WITHTX */


#define GFA_SIZE (sizeof gfa / sizeof gfa [0])	/* ���������� �������� � ������� */


#if WITHFIXEDBFO 	/* ��������������� ������� ������ �� ���� ��������� ������� ������� ���������� */
	uint_fast16_t lo4offset = (uint_fast16_t) (LO4FREQ - IF3FREQBASE);			/* ������� (��� ����) �������� ���������� */
#elif WITHDUALFLTR	/* ��������������� ������� ������ �� ���� ������������ �������� */
	uint_fast16_t lo4offset = (uint_fast16_t) (LO4FREQ - IF3FREQBASE);			/* ������� (��� ����) �������� ���������� */
#elif WITHDUALBFO	/* ��������������� ������� ������ �� ���� ������������ BFO */
	uint_fast16_t lo4offsets [2] = 
	{
		(uint_fast16_t) (IF3LOWER - IF3FREQBASE),			/* ������� (��� ����) �������� ���������� USB */
		(uint_fast16_t) (IF3UPPER - IF3FREQBASE),			/* ������� (��� ����) �������� ���������� LSB */
	};
#endif

#if WITHDUALFLTR || WITHDUALBFO || WITHFIXEDBFO

/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */

int_fast32_t 
getlo4base(void) /* = IF3FREQBASE */
{
	return IF3FREQBASE;
}

#endif /* WITHDUALFLTR || WITHDUALBFO || WITHFIXEDBFO */

int_fast32_t 
getcefreqshiftbase(void) /* = (int_fast32_t) 0 - IF3CEOFFS */
{
	return (int_fast32_t) 0 - IF3CEOFFS;
}


/* = IF3FREQBASE (��� �������) */
int_fast32_t getlo4baseflt(
	const filter_t * workfilter 
	)
{
	return workfilter->lo2set->lo4base;	// IF3FREQBASE, ��������
}


static
int_fast32_t
getfilteredge(
	const filter_t * workfilter,
	const uint_fast8_t mix4lsb	/* ���������� ��������������� ���� ������� */
	)
{
	const int_fast32_t base = getlo4baseflt(workfilter) + workfilter->ceoffset - IF3CEOFFS;
	if (mix4lsb == 0 || workfilter->high == 0)	/* workfilter->narrow ����� "������ �� ������" */
		return base + workfilter->low_or_center;
	else
		return base + workfilter->high;
}

int_fast32_t
getif3filtercenter(
	const filter_t * workfilter
	)
{
//#if (IF3_MODEL == IF3_TYPE_DCRX) || (IF3_MODEL == IF3_TYPE_BYPASS)
//	return 0;
//#else
	const int_fast32_t base = getlo4baseflt(workfilter) + workfilter->ceoffset - IF3CEOFFS;
	if (workfilter->high == 0)
		return base + workfilter->low_or_center;
	else
		return base + (((unsigned long) workfilter->low_or_center + workfilter->high) / 2);	/* center freq of filter */
//#endif
}

/* ������� "������� �� �4". ��� ��������� � ����� - �������� �������, ��� SSB - ������� ����������� ������� */
/* � ������ dc �� ���������� */
int_fast32_t 
getif3byedge(
	const filter_t * workfilter,
	uint_fast8_t mode,			/* ��� ��������� ������ ������ */
	uint_fast8_t mix4lsb,		/* ��������� ��������� ��� ��������� ������� ������ */
	uint_fast8_t tx,			/* ��� ������ �������� - ������� CW - �������� ������� �� ���������. */
	uint_fast8_t cwpitch10
	)
{
	const int_fast32_t cwpitchsigned = (int) cwpitch10 * (mix4lsb ? - CWPITCHSCALE : CWPITCHSCALE);	/* ��� ��������� */
	const int_fast32_t centerfreq = getif3filtercenter(workfilter);
	/* ��� ��������, � ������� narrow=1 ��� ���
		������� � ���������� "�� �������" - ������ �� ����������� �������
	*/
	const uint_fast8_t narrow = workfilter->high == 0;
	switch (mode)
	{
	/* ������� "������� �� �3". ��� ��������� - �������� ������� */
	case MODE_CW: 
		if (narrow || tx)
		{
			/* ��� ������ �������� - ������� CW - �������� ������� �� ���������. */
			return centerfreq;	
		}
		else
		{
			/* ������� ssb ������ */
			/* ����� ������� �� ������ �������� � ���� ����� ������. */
			return getfilteredge(workfilter, mix4lsb) + cwpitchsigned;	/* ���������� �������, ������� ������ �������� CW ������ � ������ ����������� */
		}

	/* ������� "������� �� �3". ��� SSB - ������� ����������� ������� */
	case MODE_DIGI:
	case MODE_SSB: 
		if (narrow)
		{
			/* ������ "�� ������" - � USB/LSB ����� ������� �� ������ ����������... �� ��� ��... */
			/* � ������ ������� ������������� ���� �� ��������. */
			return centerfreq - cwpitchsigned;
		}
		else
		{
			/* ������� ssb ������ */
			/* ����� ������� �� ������ �������� � ���� ����� ������. */
			return getfilteredge(workfilter, mix4lsb);
		}
	}
	return centerfreq;
}

uint_fast8_t
getgfasize(void)
{
	return GFA_SIZE;
}

/* ������� �������� � ����� ���������� ������ ������ */
/* ��������� ������� ��� ���������� ������ ������
 */
const filter_t * 
NOINLINEAT
getrxfilter(
	uint_fast8_t submode,
	uint_fast8_t ix		/* ������� ����� �������  */
	)
{
	if (GFA_SIZE == 0)
	{
		void wrong_GFA_SIZE(void);
		debug_printf_P(PSTR("getfilter: wrong configuration GFA_SIZE=%d\n"), GFA_SIZE);
		wrong_GFA_SIZE();
		return gfa [0];
	}
	return gfa [ix];
}

/* ������� �������� � ����� ���������� ������ ������ */
/* ��������� ������� ��� ���������� ������ ������
 */
const filter_t * 
NOINLINEAT
gettxfilter(
	uint_fast8_t submode,
	uint_fast8_t ix		/* ������� ����� �������  */
	)
{
#if CTLREGSTYLE_SW2014NFM || CTLREGSTYLE_SW2016VHF
	if (submode == SUBMODE_NFM)
	{
		return & fi_15p0_tx_nfm;
	}
#endif
#if WITHTX && ! (WITHSAMEBFO || FQMODEL_DCTRX)
	if (GFA_SIZE == 0)
	{
		void wrong_GFA_SIZE(void);
		debug_printf_P(PSTR("getfilter: wrong configuration GFA_SIZE=%d\n"), GFA_SIZE);
		wrong_GFA_SIZE();
		return gfa [0];
	}
#endif
	return gfa [ix];
}



/* ��������� ������ ������� "�� ���������" ��� ���������� ������ */
/* ����� ���� ������� ��� ��� ����� ��� � ��� �������� */
uint_fast8_t 
//NOINLINEAT
getdefflt(
	uint_fast8_t mode,
	uint_fast8_t ix		/* ������� ����� ������� - ������������ ���� �� ����� ���������� */
	)
{
	const uint_fast8_t ix0 = ix;	// ��������� ������ - ��������� ���� �� �������� � ����
	const MODEMASK_T mask = (MODEMASK_T) 1 << mode;
	for (;;)
	{
		const filter_t * const pf = gfa [ix];
		if ((pf->defmodes & mask) != 0 && pf->present != 0)
			break;
		ix = calc_next(ix, 0, GFA_SIZE - 1);
		if (ix == ix0)
			break;
	}
	return ix;
}

/* ��������� ������ �������, ����������� ��� ����� ��� ���������� ������ - � ��������� �� ���������, ���� �� ����� ����������. */
uint_fast8_t 
getsuitablerx(
	uint_fast8_t mode,
	uint_fast8_t ix		/* ������� ����� ������� - ������������ ���� �� ����� ���������� */
	)
{
	const uint_fast8_t ix0 = ix;	// ��������� ������ - ��������� ���� �� �������� � ����
	const MODEMASK_T mask = (MODEMASK_T) 1 << mode;

	// ���� ����� ��������� c ����������� ������ � ������ ������
	for (;;)
	{
		const filter_t * const pf = gfa [ix];
		if ((pf->suitmodesrx & mask) != 0 && pf->present != 0)
			return ix;	// ����� ����������
		ix = calc_next(ix, 0, GFA_SIZE - 1);
		if (ix == ix0)
			break;
	}
	return ix;
}

/* ��������� ������ �������, ����������� ��� �������� ��� ���������� ������ - � ��������� �� ���������, ���� �� ����� ����������. */
uint_fast8_t 
getsuitabletx(
	uint_fast8_t mode,
	uint_fast8_t ix		/* ������� ����� ������� - ������������ ���� �� ����� ���������� */
	)
{
	const uint_fast8_t ix0 = ix;	// ��������� ������ - ��������� ���� �� �������� � ����
	const MODEMASK_T mask = (MODEMASK_T) 1 << mode;

	// ���� ����� ��������� ��� ���������� ������ � ������ ������
	for (;;)
	{
		const filter_t * const pf = gfa [ix];
		if ((pf->suitmodestx & mask) != 0 && (pf->suitmodesrx & mask) == 0 && pf->present != 0)
			return ix;	// ����� ����������
		ix = calc_next(ix, 0, GFA_SIZE - 1);
		if (ix == ix0)
			break;
	}

	// ���� �� ����� ����� �������� "������ ��� ��������", ���� ��� ������
	for (;;)
	{
		const filter_t * const pf = gfa [ix];
		if ((pf->suitmodestx & mask) != 0 && pf->present != 0)
			return ix;	// ����� ����������
		ix = calc_next(ix, 0, GFA_SIZE - 1);
		if (ix == ix0)
			break;
	}
	return ix;
}

const phase_t * getplo2n(
	const filter_t * workfilter,
	uint_fast8_t tx			/* ������� ������ � ������ �������� */
	)
{
	return & workfilter->lo2set->lo2n [tx];
}

const phase_t * getplo2r(
	const filter_t * workfilter,
	uint_fast8_t tx			/* ������� ������ � ������ �������� */
	)
{
	return & workfilter->lo2set->lo2r [tx];
}

#if WITHCAT

/* ��������� ������ �������, ���������������� ������ ������ */
uint_fast8_t 
findfilter(
	uint_fast8_t mode,
	uint_fast8_t ix,		// ������� ��������� ������
	unsigned width)			// ������ ������� � ������
{
#if (IF3_FMASK & IF3_FMASK_3P1) == IF3_FMASK
	const filter_t * const pf = & fi_3p1;
#elif (IF3_FMASK & IF3_FMASK_2P7) == IF3_FMASK
	const filter_t * const pf = & fi_2p7;
#elif (IF3_FMASK & IF3_FMASK_3P1)
	const filter_t * const pf = (width > 500) ? & fi_3p1 : & fi_0p5;
#elif (IF3_FMASK & IF3_FMASK_2P7)
	const filter_t * const pf = (width > 500) ? & fi_2p7 : & fi_0p5;
#elif (IF3_FMASK & IF3_FMASK_6P0)
	const filter_t * const pf = & fi_6p0;	// ��������, ����� ���������� ������������ ��� ���
#elif (IF3_FMASK & IF3_FMASK_8P0)
	const filter_t * const pf = & fi_8p0;	// ��������, ����� ���������� ������������ ��� ���
#elif (IF3_FMASK & IF3_FMASK_15P0)
	const filter_t * const pf = & fi_15p0;	// ��������, ����� ���������� ������������ ��� ���
#else
	#error Bad IF3_FMASK defintion
#endif /*  */
	uint_fast8_t i;
	if (pf->present == 0)
		return ix;
	for (i = 0; i < GFA_SIZE; ++ i)
	{
		if (gfa [i] == pf)
			return i;
	}
	return ix;
}

#endif /* WITHCAT */
