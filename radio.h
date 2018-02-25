/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// �����������, ������������� ��� ���������, ����������� � ����������.
//


#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

typedef uint_least64_t ftw_t;	/* ���, ���������� �� �������� ��� �������� ������������� ����������� ���������� */
typedef uint_least64_t phase_t;

#define CWPITCHSCALE 10

#if WITHBBOX && defined (WITHBBOXFREQ)
	#define DEFAULTDIALFREQ	WITHBBOXFREQ
#elif ! defined (DEFAULTDIALFREQ)
	#define DEFAULTDIALFREQ	7012000L
#endif

#define	WITHMAXRXTXDELAY	100
#define	WITHMAXTXRXDELAY	100

#define	WITHNOTCHFREQMIN	300
#define WITHNOTCHFREQMAX	5000

enum
{
	BOARD_WTYPE_BLACKMAN_HARRIS,
	BOARD_WTYPE_BLACKMAN_HARRIS_MOD,
	BOARD_WTYPE_BLACKMAN_HARRIS_3TERM,
	BOARD_WTYPE_BLACKMAN_HARRIS_3TERM_MOD,
	BOARD_WTYPE_BLACKMAN_HARRIS_7TERM,
	BOARD_WTYPE_BLACKMAN_NUTTALL,
	BOARD_WTYPE_HAMMING,
	BOARD_WTYPE_HANN,
	BOARD_WTYPE_RECTANGULAR,
	//
	BOARD_WTYPE_count
};



#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX) 
	#if WITHIFSHIFT
		#error Can not be defined WITHIFSHIFT together with FQMODEL_DCTRX
	#endif
	#if WITHPBT
		#error Can not be defined WITHPBT together with FQMODEL_DCTRX
	#endif
	#if WITHDUALBFO
		#error Can not be defined WITHDUALBFO together with FQMODEL_DCTRX
	#endif
	#if WITHFIXEDBFO
		#error Can not be defined WITHFIXEDBFO together with FQMODEL_DCTRX
	#endif
	#if WITHDUALFLTR
		#error Can not be defined WITHDUALFLTR together with FQMODEL_DCTRX
	#endif
#endif

#if ELKEY328
	#define CWWPMMIN	12 //328 10
	#define CWWPMMAX	30 //328 60
#else
	#define CWWPMMIN	4	// � ts-590s �� 4-�, � �� �� 10 ��� � ��������� kenwood
	#define CWWPMMAX	60
#endif

#if WITHPOTGAIN
	#define	BOARD_RFGAIN_MIN	0		/* ��� ���������� ��������� �� ������ */
	#define	BOARD_RFGAIN_MAX	255		/* ��� ���������� ��������� �� ������ */
#else /* WITHPOTGAIN */
	#define	BOARD_RFGAIN_MIN	0		/* ��� ���������� ��������� �� ������ */
	#define	BOARD_RFGAIN_MAX	100		/* ��� ���������� ��������� �� ������ */
	#define	BOARD_AFGAIN_MIN	0		/* ��� ���������� ��������� �� ������ */
	#define	BOARD_AFGAIN_MAX	100		/* ��� ���������� ��������� �� ������ */
#endif /* WITHPOTGAIN */

#define	WITHLINEINGAINMIN	0		/* ��� ���������� ��������� ����� � ����� */
#define	WITHLINEINGAINMAX	100		/* ��� ���������� ��������� ����� � ����� */
#define	WITHMIKEINGAINMIN	0		/* ��� ���������� ��������� ����� � ��������� */
#define	WITHMIKEINGAINMAX	100		/* ��� ���������� ��������� ����� � ��������� */

//#define	WITHFILTSOFTMIN		0		/* ��� ���������� ������������ ������ ������� �������� �������� �� ����� */
//#define	WITHFILTSOFTMAX		100		/* ��� ���������� ������������ ������ ������� �������� �������� �� ����� */

#if defined (DAC1_TYPE)
	#define WITHDAC1VALMIN	0
	#define WITHDAC1VALMAX	255
#endif /* defined (DAC1_TYPE) */

#if WITHIF4DSP

	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01

	// ��������� �� ������� � ������ ��
	#define WITHAMLOW10MIN	(50 / 10)		// ����������� ������� ���������������� ����� � �������� ����
	#define WITHAMLOW10MAX	(700 / 10)		// ����������� ������� ���������������� ����� � �������� ����
	#define WITHAMHIGH100MAX	(6000 / 100)	// ������������ ������� ���������������� ����� � �������� ����
	#define WITHAMHIGH100MIN	(800 / 100)		// ����������� ������� ���������������� ����� � ������ ����
	#define WITHAMHIGH100MAX	(6000 / 100)	// ������������ ������� ���������������� ����� � ������ ����
	#define WITHAMHIGH100DEF	(4400 / 100)	// ��������� �������� � ������ ����

	// ��������� �� ������� � ������ SSB
	#define WITHSSBLOW10MIN	(50 / 10)		// ����������� ������� ���������������� ����� � �������� ����
	#define WITHSSBLOW10MAX	(700 / 10)		// ����������� ������� ���������������� ����� � �������� ����
	#define WITHSSBHIGH100MAX	(6000 / 100)	// ������������ ������� ���������������� ����� � �������� ����
	#define WITHSSBHIGH100MIN	(800 / 100)		// ����������� ������� ���������������� ����� � ������ ����
	#define WITHSSBHIGH100MAX	(6000 / 100)	// ������������ ������� ���������������� ����� � ������ ����
	#define WITHSSBHIGH100DEF	(4400 / 100)	// ��������� �������� � ������ ����


	#define	WITHNOTCHFREQ	1	/* NOTCH ������ � ��������������� ����� ���� ��� �������������� �������� */
	#define WITHSUBTONES	1	/* ����������� ������������ ������� ��� �������� NFM */
	#define WITHSAM			1	/* synchronous AM demodulation */
	#define WITHIFSHIFT		1	/* ������������ IF SHIFT */

#endif /* WITHIF4DSP */


#if LO3_SIDE == LOCODE_INVALID
	//#error PBT can be used only with triple conversion schemes
#endif

#ifndef DDS2_CLK_DIV
	#define DDS2_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS2 */
#endif

#define WITHPOTFILTERS (WITHPOTWPM || WITHPOTGAIN || WITHPOTPBT || WITHPOTIFSHIFT || WITHPOTPOWER || CTLSTYLE_RA4YBO_V3)


extern uint_fast8_t glob_agc;
extern uint_fast8_t glob_opowerlevel;
extern uint_fast8_t	glob_loudspeaker_off;

enum
{
	DSPCTL_OFFSET_MODEA,
	DSPCTL_OFFSET_MODEB,
	DSPCTL_OFFSET_AFGAIN_HI,
	DSPCTL_OFFSET_AFGAIN_LO,
	DSPCTL_OFFSET_AFMUTE,
	DSPCTL_OFFSET_RFGAIN_HI,
	DSPCTL_OFFSET_RFGAIN_LO,
	DSPCTL_OFFSET_AGCOFF,
	DSPCTL_OFFSET_AGC_T1,
	DSPCTL_OFFSET_AGC_T2,
	DSPCTL_OFFSET_AGC_T4,
	DSPCTL_OFFSET_AGC_THUNG,
	DSPCTL_OFFSET_AGCRATE,

	DSPCTL_OFFSET_MICLEVEL_HI,
	DSPCTL_OFFSET_MICLEVEL_LO,
	DSPCTL_OFFSET_NFMSQLOFF,
	DSPCTL_OFFSET_NFMSQLLEVEL,

	DSPCTL_OFFSET_CWEDGETIME,
	DSPCTL_OFFSET_SIDETONELVL,			// ������� ������������/������� ������

	DSPCTL_OFFSET_LOWCUTRX_HI,		// ������ ������� ����� ����� �������
	DSPCTL_OFFSET_LOWCUTRX_LO,		// ������ ������� ����� ����� �������
	DSPCTL_OFFSET_HIGHCUTRX_HI,		// ������� ������� ����� ����� �������
	DSPCTL_OFFSET_HIGHCUTRX_LO,		// ������� ������� ����� ����� �������

	DSPCTL_OFFSET_LOWCUTTX_HI,		// ������ ������� ����� ����� �������
	DSPCTL_OFFSET_LOWCUTTX_LO,		// ������ ������� ����� ����� �������
	DSPCTL_OFFSET_HIGHCUTTX_HI,		// ������� ������� ����� ����� �������
	DSPCTL_OFFSET_HIGHCUTTX_LO,		// ������� ������� ����� ����� �������

	DSPCTL_OFFSET_NOTCH_ON,
	DSPCTL_OFFSET_NOTCH_WIDTH_HI,
	DSPCTL_OFFSET_NOTCH_WIDTH_LO,
	DSPCTL_OFFSET_NOTCH_FREQ_HI,
	DSPCTL_OFFSET_NOTCH_FREQ_LO,
	DSPCTL_OFFSET_LO6_FREQ_HI,
	DSPCTL_OFFSET_LO6_FREQ_LO,
	DSPCTL_OFFSET_FULLBW6_HI,
	DSPCTL_OFFSET_FULLBW6_LO,
	DSPCTL_OFFSET_CWPITCH_HI,
	DSPCTL_OFFSET_CWPITCH_LO,
	DSPCTL_OFFSET_DIGIGAINMAX,
	//DSPCTL_OFFSET_FLTSOFTER,
	DSPCTL_OFFSET_AMDEPTH,
	DSPCTL_OFFSET_MIKEAGC,
	DSPCTL_OFFSET_MIKEAGCSCALE,

	// ��� ��� ������ ��������� - ������ ������
	DSPCTL_BUFSIZE
};

enum
{
	DSPCTL_MODE_IDLE = 0,		/* �����, � ������� �� ����������� ������� ���������. */

	DSPCTL_MODE_RX_BASE,

	DSPCTL_MODE_RX_BPSK,
	DSPCTL_MODE_RX_FREEDV,
	DSPCTL_MODE_RX_WIDE, 	/* ���� � �������� ������ ���������� �������� � ������� ����� ��� */
	DSPCTL_MODE_RX_NARROW, 	/* ���� � �������� ������ ��������� ����������� ������� � ������ ����������� */
	DSPCTL_MODE_RX_AM,
	DSPCTL_MODE_RX_SAM,
	DSPCTL_MODE_RX_NFM,
	DSPCTL_MODE_RX_DSB,		/* ���� ���� ������� ����� (����� ��������� �� ������� ������� */
	DSPCTL_MODE_RX_DRM,		/* ��������� ������ ������������� �� ����� �� ������� 12 ��� */
	DSPCTL_MODE_RX_ISB,		/* ����� ���� ����������� ������� ����� */
	DSPCTL_MODE_RX_WFM,		/* ����� WFM (����������� � FPGA, ������ ��� WITHUSEDUALWATCH)	*/

	DSPCTL_MODE_RX_TOP,

	DSPCTL_MODE_TX_BASE,

	DSPCTL_MODE_TX_SSB,
	DSPCTL_MODE_TX_CW,
	DSPCTL_MODE_TX_AM,
	DSPCTL_MODE_TX_NFM,
	DSPCTL_MODE_TX_ISB,		/* �������� ���� ����������� ������� ����� */
	DSPCTL_MODE_TX_BPSK,
	DSPCTL_MODE_TX_FREEDV,

	DSPCTL_MODE_TX_TOP,
	//
	DSPCTL_MODE_COUNT
};


uint_fast8_t board_getsmeter(uint_fast8_t * tracemax, uint_fast8_t minval, uint_fast8_t maxval, uint_fast8_t clean);	/* �������� �������� �� ��� s-����� */

uint_fast8_t board_getvox(void);	/* �������� �������� �� ��������� VOX */
uint_fast8_t board_getavox(void);	/* �������� �������� �� ��������� Anti-VOX */


// filters.c interface

/* ������������ ���� ��������� ������� ����������� ����������.
 * ����� - ��� ���������� ������ + �������� + ������ ��������� + ������� ������
 */

enum { 
#if WITHMODESETSMART
	SUBMODE_SSBSMART,
	SUBMODE_CWSMART, SUBMODE_CWZSMART, 
	SUBMODE_DIGISMART,
#else
	SUBMODE_USB, SUBMODE_LSB, 
	SUBMODE_CW, SUBMODE_CWR, SUBMODE_CWZ, 
	SUBMODE_AM,
	#if WITHSAM
		SUBMODE_SAM,	/* synchronous AM demodulation */
	#endif /* WITHSAM */
	SUBMODE_DRM,
	SUBMODE_DGL, SUBMODE_DGU,
	#if WITHMODESETFULLNFM || WITHMODESETMIXONLY3NFM || WITHMODESETFULLNFMWFM
		SUBMODE_NFM,
	#endif /* WITHMODESETFULLNFM || WITHMODESETMIXONLY3NFM || WITHMODESETFULLNFMWFM*/
	#if WITHWFM || WITHMODESETFULLNFMWFM
		SUBMODE_WFM,
	#endif /* WITHWFM || WITHMODESETFULLNFMWFM */
	#if WITHMODEM
		SUBMODE_BPSK,
	#endif /* WITHMODEM */
	SUBMODE_ISB,
	#if WITHFREEDV
		SUBMODE_FREEDV,
	#endif /* WITHFREEDV */
#endif
	//
	SUBMODE_COUNT
};

/* ��������� ������� ������ */
enum {
	MODE_CW = 0,/* cw & cwr */
	MODE_SSB,	/* usb or lsb */
	MODE_AM,	/* am */
#if WITHSAM
	MODE_SAM,	/* synchronous AM demodulation */
#endif /* WITHSAM */
	MODE_NFM,	/* fm */
	MODE_DRM,	/* ���� DRM */
	MODE_CWZ,	/* cwz - ���������� �� ������� �������. �� �������� ������������ ��� TUNE */
	MODE_TUNE = MODE_CWZ,	/* cwz - ���������� �� ������� �������. �� �������� ������������ ��� TUNE */
#if WITHWFM || WITHMODESETFULLNFMWFM
	MODE_WFM,	/* WFM mode */
#endif /* WITHWFM || WITHMODESETFULLNFMWFM */
	MODE_DIGI,	/* dgu & dgl */
	MODE_RTTY,	/* rtty */
#if WITHMODEM
	MODE_MODEM,
#endif /* WITHMODEM */
	MODE_ISB,	/* usb & lsb */
#if WITHFREEDV
	MODE_FREEDV,
#endif /* WITHFREEDV */
	//
	MODE_COUNT	/* ������ ������� ������������ ����� ��� ������� ����� � ������ ���������� ������� ������ ��� �������� */
};

//#if WITHMODEM || WITHWFM || WITHIF4DSP || CPUSTYLE_ARM
	typedef uint_fast16_t MODEMASK_T;	/* ���, ����������� ��� �������� MODE_COUNT ����� */
//#else /* WITHMODEM || WITHWFM */
//	typedef uint_fast8_t MODEMASK_T;	/* ���, ����������� ��� �������� MODE_COUNT ����� */
//#endif /* WITHMODEM || WITHWFM || WITHIF4DSP || CPUSTYLE_ARM */


typedef struct lo2param_tag
{
	uint_least64_t lo2n [2];			/* N �������� ��� ���� (RX � TX) */
	uint_least64_t lo2r [2];			/* R �������� ��� ���� (RX � TX) */
	int_fast32_t lo4base;				/* IF3FREQBASE, �������� */
} lo2param_t;

/* �������� ������� */
/* ���� ������ ������������ ������ � ������ �����, ���� suitmodestx = 0 */
/* ���� ������ ������������ ������ � ������ ��������, ���� suitmodesrx = 0 */
/* � ������� code ����������� ���� �������������� ���������� ������� � ������� ������ � �������� */
typedef struct filter_tag
{
	MODEMASK_T suitmodesrx;	/* suitable for: modefamily bits */
	MODEMASK_T suitmodestx;	/* suitable for: modefamily bits */
	MODEMASK_T defmodes;	/* default for: modefamily bits */
	uint_fast16_t low_or_center;		/* frequency of low edge (for ssb detector) -  Hz - offset from ::if3base  */
	uint_fast16_t high;		/*  high edge frequency (for ssb detector) - Hz - offset from ::if3base, 0 for narrow filters */
	uint_fast16_t code [2];	/* code of filter for RX/TX board control */
	uint_fast8_t present;	/* this filter installed */
	uint_fast8_t widefilter;	/* 0 - ��������� "�����", 1 - "�������" */
	uint_fast16_t ceoffset;		/*  ��������� �� IF3CEOFFS ����� ����������� �������: IF3CEOFFS - 0 ���� */
	const lo2param_t * lo2set;
	const FLASHMEM char * labelf3;	/* name of filter - 3 chars width */
} filter_t;

#define IF3OFFS 15000	/* �������� ����������� ������� ����� ����� ���� - ��������� �������� ������ ���������� � uint_fast16_t */
#define IF3CEOFFS 5000	/* �������� ����������� ������� ������ ����� ���� - ��������� �������� ������ ���������� � uint_fast16_t */

// ������������ � ���� �������
extern filter_t fi_2p4;
extern filter_t fi_2p4_tx;
extern filter_t fi_2p7;
extern filter_t fi_2p7_tx;
extern filter_t fi_0p3;
extern filter_t fi_0p5;
extern filter_t fi_1p8;
extern filter_t fi_3p1;
extern filter_t fi_3p1_tx;
extern filter_t fi_6p0;
extern filter_t fi_7p8;
extern filter_t fi_8p0;
extern filter_t fi_9p0;
extern filter_t fi_15p0;
extern filter_t fi_15p0_tx_nfm;
extern filter_t fi_17p0;

extern filter_t fi_2p0_455;
extern filter_t fi_10p0_455;
extern filter_t fi_3p0_455;
extern filter_t fi_6p0_455;

extern uint_fast16_t lo4offset;			/* ������� (��� ����) �������� ���������� */
extern uint_fast16_t lo4offsets [2]; 	/* ������� (��� ����) �������� ���������� ��� ������ ������� */

int_fast32_t getlo4base(void); /* = IF3FREQBASE */
int_fast32_t getcefreqshiftbase(void); /* = (int_fast32_t) 0 - IF3CEOFFS */

int_fast32_t 
getif3byedge(
	const filter_t * workfilter,
	uint_fast8_t mode,			/* ��� ��������� ������ ������ */
	uint_fast8_t mix4lsb,		/* ��������� ��������� ��� ��������� ������� ������ */
	uint_fast8_t tx,			/* ��� ������ �������� - ������� CW - �������� ������� �� ���������. */
	uint_fast8_t gcwpitch10
	);

int_fast32_t
getif3filtercenter(
	const filter_t * workfilter
	);

/* = IF3FREQBASE (��� �������) */
int_fast32_t getlo4baseflt(
	const filter_t * workfilter 
	);

uint_fast8_t calc_next(uint_fast8_t v, uint_fast8_t low, uint_fast8_t high);

uint_fast8_t getgfasize(void);

const filter_t * 
NOINLINEAT
getrxfilter(
	uint_fast8_t submode,
	uint_fast8_t ix		/* ������� ����� �������  */
	);

const filter_t * 
NOINLINEAT
gettxfilter(
	uint_fast8_t submode,
	uint_fast8_t ix		/* ������� ����� �������  */
	);

uint_fast8_t 
//NOINLINEAT
getdefflt(
	uint_fast8_t mode,
	uint_fast8_t ix		/* ������� ����� ������� - ������������ ���� �� ����� ���������� */
	);
uint_fast8_t 
//NOINLINEAT
getsuitablerx(
	uint_fast8_t mode,
	uint_fast8_t ix		/* ������� ����� ������� - ������������ ���� �� ����� ���������� */
	);
uint_fast8_t 
//NOINLINEAT
getsuitabletx(
	uint_fast8_t mode,
	uint_fast8_t ix		/* ������� ����� ������� - ������������ ���� �� ����� ���������� */
	);

uint_fast8_t 
findfilter(
	uint_fast8_t mode,
	uint_fast8_t ix,		// ������� ��������� ������
	unsigned width);			// ������ ������� � ������

const phase_t * getplo2n(
	const filter_t * workfilter,
	uint_fast8_t tx			/* ������� ������ � ������ �������� */
	);

const phase_t * getplo2r(
	const filter_t * workfilter,
	uint_fast8_t tx			/* ������� ������ � ������ �������� */
	);


enum
{
	BOARD_TXAUDIO_MIKE,	// "MIKE ",
	BOARD_TXAUDIO_LINE,	// "LINE ",
#if WITHUSBUAC
	BOARD_TXAUDIO_USB,	// "USB AUDIO",
#endif /* WITHUSBUAC */
	BOARD_TXAUDIO_2TONE,	// "2TONE",
	BOARD_TXAUDIO_NOISE,	// "NOISE",
	BOARD_TXAUDIO_1TONE,	// "1TONE",
	BOARD_TXAUDIO_MUTE,		// no signal to tx
	//
	BOARD_TXAUDIO_count
};

enum
{
	BOARD_RXMAINSUB_A_A,	// � ��� ��� ������� A
	BOARD_RXMAINSUB_A_B,	// ����� - A, ������ - B
	BOARD_RXMAINSUB_B_A,
	BOARD_RXMAINSUB_B_B,
	//
	BOARD_RXMAINSUB_count
};
void spool_lfm(void);	// ������� ��-0, ���� ������� ����� LFM
void lfm_run(void);
void hardware_lfm_setupdatefreq(unsigned ticksfreq);
void hardware_lfm_timer_initialize(void);

/* ���������� ������ �������� ������������ ����-�������� */
void vox_initialize(void);
void vox_enable(uint_fast8_t voxstate, uint_fast8_t vox_delay_tens);	/* ���������� (��-0) ��� ���������� (0) ������ vox. */
void vox_set_levels(uint_fast8_t level, uint_fast8_t alevel);	/* ���������� ������� ������������ vox � anti-vox */
void vox_probe(uint_fast8_t vlevel, uint_fast8_t alevel);		/* ���������� ��� �������� ����������������� ������� ������� � anti-vox */

void seq_set_bkin_enable(uint_fast8_t bkinstate, uint_fast8_t bkin_delay_tens);	/* ���������� (��-0) ��� ���������� (0) ������ ��������� BREAK-IN. */
void seq_set_cw_enable(uint_fast8_t state);	/* ���������� (��-0) ��� ���������� (0) ������ qsk. �������� ������ CW */
void seq_set_rgbeep(uint_fast8_t state);	/* ���������� (��-0) ��� ���������� (0) ������������ roger beep */

// ��� ���������� ������� ���� ������ ����������� ����� ��� ��������� ���������:
//
// ��������� �����
// ��������� �������� � ����� �� �������� � � �������� �� ����
// ��������� �������� SSB
// ��������� �������� CW (� ��� �� AM, NFM)

enum 
{
	TXGFI_RX = 0,	// ������ ��������� ��� ������ �����
	TXGFI_TRANSIENT,	// ������ ��������� ��� ������ �������� � ����� �� �������� � � �������� �� ����
	TXGFI_TX_KEYUP,		// ����� �������� ��� �������� �����
	TXGFI_TX_KEYDOWN,	// ����� �������� � ������� ������
	//
	TXGFI_SZIE	// ���������� ���������
};

// �������������� ����������� �����
//#define TXGFV_RX		0
//#define TXGFV_TRANS		0			// ������� ����� �������� ����� � ��������
//#define TXGFV_TX_SSB	TXPATH_BIT_ENABLE_SSB
//#define TXGFV_TX_CW		TXPATH_BIT_ENABLE_CW

//	portholder_t txgfva [TXGFI_SZIE];	// ���������� ������� ��� ������ �������
//	uint_fast8_t sdtnva [TXGFI_SZIE];	// �������� ��������� ������������ ��� ������ �������

//	{ TXGFV_RX, TXGFV_TRANS, TXGFV_TRANS, TXGFV_TX_CW }, // ��� CW
//	{ 0, 0, 0, 1 },	// ��� CW

//	{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB } // ��� SSB
//	{ 0, 0, 0, 0 },	// ��� SSB

void seq_set_txgate_P(const portholder_t FLASHMEM * txgfp, const uint_fast8_t FLASHMEM * sdtnp);	/* ��� �������� ���������� � ������ ������ ������ �� ���������� */

void hardware_cw_diagnostics_noirq(
	uint_fast8_t c1,
	uint_fast8_t c2,
	uint_fast8_t c3);
void hardware_cw_diagnostics(
	uint_fast8_t c1,
	uint_fast8_t c2,
	uint_fast8_t c3);


/* �� ������� ����� �������� �����:
 ������������ ����:
	- ����������� ������� - KEY
	- ��������� ������� - NC
	- �������� ������� - GND

 ���������� (�� ����������� � ���� ���������):
	- ����������� ������� - DOT (�����)
	- ��������� ������� - DASH (����)
	- �������� ������� - GND
*/
enum
{
	ELKEY_PADDLE_DIT = 0x01,
	ELKEY_PADDLE_DASH = 0x02,
};

#define ELKEY_DISCRETE 20		/* ������������ ����� � ���� � ��������� �� 1/20 �� ������������ ����� */

uint_fast8_t cat_get_keydown(void);	// �������� ������ ����� �� ����� ����������, ���������� �� ����������� �����������


/* ����������� ���� */
void seq_initialize(void);
void elkey_initialize(void);
void board_set_wpm(uint_fast8_t wpm); /* ��������� ���� - ���������� ��������  */
void elkey_set_format(uint_fast8_t dashratio, uint_fast8_t spaceratio); /* ��������� ���� - ���������� ���������� ���� � ����� (� �������� ���������). */
void elkey_set_mode(uint_fast8_t mode, uint_fast8_t reverse);	/* ����� ������������ ����� - 0 - asf, 1 - paddle, 2 - keyer */
void elkey_set_slope(uint_fast8_t slope);	// �������� ���������� ������������ ����� � ����� - �������� �����������
uint_fast8_t elkey_get_output(void);
uint_fast8_t elkey_getnextcw(void);	// ��������� ������� ��� �������� (������ ������� �������)

/* ��������� ���� - ���������� �������� ��� �������� �� �������� � �������. */
void seq_set_rxtxdelay(
	uint_fast8_t rxtxdelay, 
	uint_fast8_t txrxdelay,
	uint_fast8_t pretxdelay
	);
// ������ �� user-mode ����� ��������� �� ������� �� �������� ��� tune.
void seq_txrequest(uint_fast8_t tune, uint_fast8_t ptt);

/* ��������� ���������� �� ���������� ������ �� ������������ �� �������� � �������� ��������� */
uint_fast8_t seq_get_txstate(void);
/* ������������� �� user-mode ��������� � ���, ��� ����� ������ ����-�������� ������������ */
void seq_ask_txstate(
	uint_fast8_t tx);	/* 0 - ��������� ��������� � ��������� �����, ����� - � ��������� �������� */

void seq_purge(void);	/* ������� ����������� ������� �� ����� �������. ���������� �� user-mode ��������� */

uint_fast8_t hardware_getshutdown(void);	/* ���������� ������ �� ���������� - �� ����������� ������� */

void hardware_elkey_timer_initialize(void);
void hardware_elkey_set_speed(uint_fast32_t ticksfreq);
void hardware_elkey_ports_initialize(void); // ������������� ������ ������������ �����, ����� CAT_DTR
void hardware_ptt_port_initialize(void);	// ������������� ����� PTT, ����� CAT_RTS

uint_fast8_t hardware_elkey_getpaddle(uint_fast8_t reverse);
uint_fast8_t hardware_get_ptt(void);

void hardware_txpath_initialize(void);
void hardware_txpath_set(portholder_t txpathstate);
void dsp_txpath_set(portholder_t txpathstate);

/* ��������� ����-�������� - � �� ��������� � �� 1/ELKEY_DISCRETE ����� */
void elkey_spool_dots(void);	/* ����������� ���� - ���������� � �������� 1/ELKEY_DISCRETE �� ������������ �����. */
/* ���� ����������� �������������� ������� � ������ */
#define LOCODE_UPPER	0	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
#define LOCODE_LOWER	1	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
#define LOCODE_TARGETED	2	/* ���� ��������� ����������� ��� ��������� ��������� �������. ������ ���� ����� ���� �����. */
#define LOCODE_INVALID	3	/* ����� ���������� (� ��������) ���. */

/* ������ �� - 10.7 MHz */
#if FQMODEL_FMRADIO

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */

	#define WITHWFM 1
	#define LO1MODE_DIRECT	1
	#define	PLL1_FRACTIONAL_LENGTH	0	/* ������������� ���������� */

	#define REFERENCE_FREQ		4000000UL	/* ������� ������� */
	#define SYNTH_R1			160				/* ��� ����������� (������� ���������) - 25 ��� */
	#define TSA6057_PLL1_RCODE	TSA6057_R25_160	/* �������� ���������������� TSA6057 */
	#define	DDS1_CLK_MUL	1

	#define	TUNE_BOTTOM	87000000L
	#define	TUNE_TOP	108500000L

	#define	WFM_IF1	10700000ul

	#define IF3_MODEL IF3_TYPE_455

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */

	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define LO4_POWER2	0		/* ���� 0 - ��������� � ������ �������� ���������� ����� ���������� ���. */

	#define SYNTHLOWFREQ 	(WFM_IF1 + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
	#define SYNTHTOPFREQ 	(WFM_IF1 + TUNE_TOP)		/* Highest frequency of 1-st LO */

	#if (HYBRID_NVFOS == 1)
		#define FREQ_DENOM 1000000
		#define FREQ_SCALE 1500000	
	#else
		#error HYBRID_NVFOS not defined
	#endif

#endif /* FQMODEL_FMRADIO */

/* ������ �� - 73050 ���, ������ - 455 ��� ��� 500 ��� */
#if FQMODEL_73050 || FQMODEL_73050_IF0P5

	#if FQMODEL_73050_IF0P5

		#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO1_SIDE_F(freq) (((freq) < 73050000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
		#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#if WITHIF4DSP
			// ����������� ��� DSP �� ��������� ��
			#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
			#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#else /* WITHIF4DSP */
			// ������� ����������� ��� ��������� �����
			#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
			//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
		#endif /* WITHIF4DSP */

		#if MODEL_DIRECT
			#define DIRECT_72M550_X5	1	/* DDS LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */	
		#else
			#define HYBRID_72M550_10M7	1	/* Hybrid LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */
		#endif

		#define IF3_MODEL IF3_TYPE_500

		//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

		#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */
		#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
		#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

	#elif FQMODEL_73050

		#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO1_SIDE_F(freq) (((freq) < 73050000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
		#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#if WITHIF4DSP
			// ����������� ��� DSP �� ��������� ��
			#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
			#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#else /* WITHIF4DSP */
			// ������� ����������� ��� ��������� �����
			#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
			//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
		#endif /* WITHIF4DSP */

		#if MODEL_DIRECT
			#define DIRECT_72M595_X5	1	/* DDS LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */	
		#else
			#define HYBRID_72M595_10M7	1	/* Hybrid LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */
		#endif

		#define IF3_MODEL IF3_TYPE_455

		#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
		#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

		//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

		#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */
	#else
		#error Config error
	#endif

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 56000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV56M_45M	/* ������ �� 45 ��� */
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 45000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV56M_36M	/* ������ �� 36 ��� */
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 36000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define SYNTHLOWFREQ 	(73050000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(73050000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=73.050000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1099410      /* Full VCO range 73.080000..129.050000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=73.050000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1152762      /* Full VCO range 73.080000..129.050000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif


#endif /* FQMODEL_73050 */

/* ������ �� - 70455 ���, ������ - 455 ��� */
#if FQMODEL_70455

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	//#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO1_SIDE_F(freq) (((freq) < 70455000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#if WITHIF4DSP
		// ����������� ��� DSP �� ��������� ��
		#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#else /* WITHIF4DSP */
		// ������� ����������� ��� ��������� �����
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
	#endif /* WITHIF4DSP */

	#if MODEL_DIRECT
		#define DIRECT_70M000_X5	1	/* DDS LO1, LO2 = 70000 kHz - for 70455 kHz filter from FT-1000 */	
	#else
		#define HYBRID_70M000_10M7	1	/* Hybrid LO1, LO2 = 70000 kHz - for 70455 kHz filter from FT-1000 */
	#endif

	#define IF3_MODEL IF3_TYPE_455

	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 56000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV56M_45M	/* ������ �� 45 ��� */
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 45000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV56M_36M	/* ������ �� 36 ��� */
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 36000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define SYNTHLOWFREQ 	(70455000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(70455000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=70.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1102317      /* Full VCO range 70.485000..126.455000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=70.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1157336      /* Full VCO range 70.485000..126.455000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined

		#endif
	#endif


#endif /* FQMODEL_70455 */

/* RU6BK: 1-st if=10.7 MHz, 2-nd if=500k, fixed BFO */
#if FQMODEL_10M7_500K
	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	//#define WITHCLASSICSIDE	1	/* ������� ������� ���������� ������� �� ������� ������� */
	//#define LO1_SIDE_F(freq) (((freq) < 13950000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
	#define LO2_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	//#define LO2_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	//#define LO4_SIDE	LOCODE_TARGETED	/* ��� WITHFIXEDBFO ������������ �������� �� ���� */
	#if WITHIF4DSP
		// ����������� ��� DSP �� ��������� ��
		#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#else /* WITHIF4DSP */
		// ������� ����������� ��� ��������� �����
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
	#endif /* WITHIF4DSP */
	#define WITHFIXEDBFO	1	/* ��������� ������� �� ���� 2-�� ��������� 10.2 MHz / 10.9 MHz */

	#define IF3_MODEL IF3_TYPE_500

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
	#define LO1_POWER2	0		/* 0 - ��������� �������� - ��� �������� */
	#define LO4_POWER2 0
	#define LO2_DDSGENERATED	1			// dds3 used as LO2 output

	#if (LO2_SIDE == LOCODE_LOWER)
		#define LO2_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
		#define LO2_PLL_N	10200000uL		/* for 10.2 MHz LO2 */
	#elif (LO2_SIDE == LOCODE_UPPER)
		#define LO2_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
		#define LO2_PLL_N	10700000uL		/* for 10.2 MHz LO2 */
	#endif

	#if BANDSELSTYLERE_LOCONV32M_NLB
		#define TUNE_BOTTOM 1500000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV32M
		#define TUNE_BOTTOM 100000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLE_LADVABEST
		#define TUNE_BOTTOM 1600000L			/* ������ ������� ��������� */
		#define TUNE_TOP 56000000L
	#else
		#error Strange BANDSELSTYLExxx
	#endif

#endif /* FQMODEL_10M7_500K */

/* ������ �� - 80455 ���, ������ - 455 ��� */
#if FQMODEL_80455

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE_F(freq) (((freq) < 80455000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
	//#define LO1_SIDE_F(freq) (((freq) < 80455000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#if WITHIF4DSP
		// ����������� ��� DSP �� ��������� ��
		#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#else /* WITHIF4DSP */
		// ������� ����������� ��� ��������� �����
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
	#endif /* WITHIF4DSP */

	#if MODEL_DIRECT
		#define DIRECT_80M0_X5	1	/* DDS LO1, LO2 = 80000 kHz - for 80455 kHz filter */	
	#else
		#define HYBRID_80M0_10M7	1	/* Hybrid LO1, LO2 = 80000 kHz - for 80455 kHz filter */
	#endif

	#define IF3_MODEL IF3_TYPE_455

	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 56000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV56M_45M	/* ������ �� 45 ��� */
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 45000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV56M_36M	/* ������ �� 36 ��� */
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 36000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define SYNTHLOWFREQ 	(80455000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(80455000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=80.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1091974      /* Full VCO range 80.485000..136.455000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=80.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1141086      /* Full VCO range 80.485000..136.455000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif


#endif /* FQMODEL_80455 */

/* DUC/DDC. ��������� ��������� � DSP ���������� */
#if FQMODEL_FPGA
	#define DUCDDC_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL)		/* �������, ������� ����������� NCO � DUC/DDC */
	/* �������� ��������� �������������� ������� � ������ */

	#define R820T_IFFREQ 4063000uL
	#define R820T_REFERENCE (REFERENCE_FREQ / 4)	// 122.88 / 4 = 30.720 MHz
	//#define R820T_LOSTEP (R820T_REFERENCE / 16)	// 1.92 MHz
	#define R820T_LOSTEP (R820T_REFERENCE / 32)	// 920 kHz

	#if defined (XVTR1_TYPE)
		#define LO0_SIDE_F(freq) ((freq) >= 50000000uL ? LOCODE_UPPER : LOCODE_INVALID)	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO1_SIDE	LOCODE_LOWER	/* rf->baseband: ��� �������������� �� ���� ���������� ��� �������� ������� */
	#else
		#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO1_SIDE_F(freq) ((((freq) % DUCDDC_FREQ) >= (DUCDDC_FREQ / 2)) ? LOCODE_UPPER : LOCODE_LOWER)	/* rf->baseband: ������� ������� �� ������� ������� */
	#endif
	//#define LO1_SIDE	LOCODE_LOWER	/* rf->baseband: ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define LO4_POWER2 0
	#define IF3_MODEL IF3_TYPE_BYPASS

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 56000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV56M_45M	/* ������ �� 45 ��� */
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 45000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV56M_36M	/* ������ �� 36 ��� */
		#define TUNE_BOTTOM 30000L		/* 30 kHz ������ ������� ��������� */
		#define TUNE_TOP 36000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 32000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 36000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLE_OLEG4Z
		#define TUNE_BOTTOM 20000L		/* 20 kHz ������ ������� ��������� */
		#define TUNE_TOP 1700000000L	/* 1.7 GHz ������� ������� ��������� */
	#elif BANDSELSTYLERE_UHF_137M_174M
		#define TUNE_BOTTOM 137000000L		/* ������ ������� ��������� */
		#define TUNE_TOP 174000000L			/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

#endif /* FQMODEL_FPGA */

#if FQMODEL_64455 || FQMODEL_64455_IF0P5

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE_F(freq) (((freq) < 64455000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#if WITHIF4DSP
		// ����������� ��� DSP �� ��������� ��
		#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#else /* WITHIF4DSP */
		// ������� ����������� ��� ��������� �����
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
	#endif /* WITHIF4DSP */

	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	/* ����� ������������������ */
	#if FQMODEL_64455_IF0P5

		#if MODEL_DIRECT
			#define DIRECT_63M9_X6	1	// DDS1 clock = 383.730 MHz
		#elif HYBRID_13M
			#define HYBRID_63M9_13M0	1
		#else
			#define HYBRID_63M9_10M7	1
		#endif

		#define IF3_MODEL IF3_TYPE_500

	#elif FQMODEL_64455
		#if MODEL_DIRECT
			#define DIRECT_64M0_X6	1	// DDS1 clock = 384 MHz
		#elif HYBRID_13M
			#define HYBRID_64M0_13M0	1
		#else
			#define HYBRID_64M0_10M7	1
			//#define HYBRID_12M8_10M7	1
		#endif

		#define IF3_MODEL IF3_TYPE_455

	#else
		#error TODO: missing data for configuration
	#endif


	//#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* ������ ������� ��������� */
		#define TUNE_TOP 56000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define SYNTHLOWFREQ 	(64455000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(64455000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=64.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1109756      /* Full VCO range 64.485000..120.455000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=64.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1169072      /* Full VCO range 64.485000..120.455000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif

	#endif

#endif // MODEL_64455

/* ������ �� - 70200 ���, ������ - 200 ��� (������ �� �������� EKD) */
#if FQMODEL_70200

	/* �������� ��������� �������������� ������� � ������ */
	//#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE_F(freq) (((freq) < 70200000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#if WITHIF4DSP
		// ����������� ��� DSP �� ��������� ��
		#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#else /* WITHIF4DSP */
		// ������� ����������� ��� ��������� �����
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
	#endif /* WITHIF4DSP */


	#define IF3_MODEL IF3_TYPE_200

	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 100000L		/* ������ ������� ��������� */
		#define TUNE_TOP 30000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if MODEL_DIRECT

		#define DIRECT_70M000_X5	1	/* DDS LO1, LO2 = 70000 kHz - for 70200 kHz filter from EKD */	

	#else /* MODEL_DIRECT */

		#define HYBRID_70M000_10M7	1	/* Hybrid LO1, LO2 = 70000 kHz - for 70200 kHz filter from EKD */

		#define SYNTHLOWFREQ 	(70200000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(70200000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=70.200000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1102612      /* Full VCO range 70.230000..126.200000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=70.200000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1157801      /* Full VCO range 70.230000..126.200000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif

	#endif /* MODEL_DIRECT */


#endif /* FQMODEL_70200 */

/* ������ �� - 60.7 ���, ������ - 10.7 ���, ������ - 500 ��� */
/* ������ �� - 60.7 ���, ������ - 10.7 ���, ������ - 200 ��� */
#if FQMODEL_60700_IF05 || FQMODEL_60700_IF02 || FQMODEL_60700_IF0455

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	//#define LO3_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_UPPER 	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	#if WITHIF4DSP
		// ����������� ��� DSP �� ��������� ��
		#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#else /* WITHIF4DSP */
		// ������� ����������� ��� ��������� �����
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
	#endif /* WITHIF4DSP */
	
	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

	#define	LO3_POWER2	0		/* 0 - ��� �������� ����� ���������� LO3 ����� ������� �� ��������� */
	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */


	#if FQMODEL_60700_IF05
		// ������ �� - 500 ���
		#if LO3_SIDE == LOCODE_UPPER
			#define LO3_PLL_R	500		/* ������� ��� ���������� ������ ���������� */
			#define LO3_PLL_N	112		/* LO3 = 11.2 MHz */
		#else /* LO3_UPPER */
			#define LO3_PLL_R	500		/* ������� ��� ���������� ������ ���������� */
			#define LO3_PLL_N	102		/* LO3 = 10.2 MHz */
		#endif /* LO3_UPPER */

		#define IF3_MODEL IF3_TYPE_500
	#elif FQMODEL_60700_IF02
		// ������ �� - 200 ���
		#if LO3_SIDE == LOCODE_UPPER
			#define LO3_PLL_R	500		/* ������� ��� ���������� ������ ���������� */
			#define LO3_PLL_N	109		/* LO3 = 10.9 MHz */
		#else /* LO3_UPPER */
			#define LO3_PLL_R	500		/* ������� ��� ���������� ������ ���������� */
			#define LO3_PLL_N	105		/* LO3 = 10.5 MHz */
		#endif /* LO3_UPPER */

		#define IF3_MODEL IF3_TYPE_200
	#elif FQMODEL_60700_IF0455
		// ������ �� - 455 ���
		#if LO3_SIDE == LOCODE_UPPER
			#define LO3_PLL_R	REFERENCE_FREQ		/* ������� ��� ���������� ������ ���������� */
			#define LO3_PLL_N	11155000			/* LO3 = 11.155 MHz */
		#else /* LO3_UPPER */
			#define LO3_PLL_R	REFERENCE_FREQ		/* ������� ��� ���������� ������ ���������� */
			#define LO3_PLL_N	10245000			/* LO3 = 10.245 MHz */
		#endif /* LO3_UPPER */

		#define IF3_MODEL IF3_TYPE_455
	#endif

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* ������ ������� ��������� */
		#define TUNE_TOP 56000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 32000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 36000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define HYBRID_50M0_10M7	1	/*  */	

		#define SYNTHLOWFREQ 	(60700000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(60700000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (TUNE_BOTTOM == 30000L) && (TUNE_TOP == 56000000L)	// BANDSELSTYLERE_UPCONV56M
			#if (HYBRID_NVFOS == 6)
				/* 6 VCOs, IF=60.700000, tune: 0.030000..56.000000 */
				#define FREQ_SCALE 1115007      /* Full VCO range 60.730000..116.700000 */
				#define FREQ_DENOM 1000000      /* 6 VCOs */
			#elif (HYBRID_NVFOS == 4)
				/* 4 VCOs, IF=60.700000, tune: 0.030000..56.000000 */
				#define FREQ_SCALE 1177380      /* Full VCO range 60.730000..116.700000 */
				#define FREQ_DENOM 1000000      /* 4 VCOs */			
			#else
				#error HYBRID_NVFOS not defined
			#endif
		#elif (TUNE_BOTTOM == 100000L) && (TUNE_TOP == 32000000L)	// BANDSELSTYLERE_UPCONV32M
			#if (HYBRID_NVFOS == 6)
				/* 6 VCOs, IF=60.700000, tune: 0.100000..32.000000 */
				#define FREQ_SCALE 1072826      /* Full VCO range 60.800000..92.700000 */
				#define FREQ_DENOM 1000000      /* 6 VCOs */
			#elif (HYBRID_NVFOS == 4)
				/* 4 VCOs, IF=60.700000, tune: 0.100000..32.000000 */
				#define FREQ_SCALE 1111204      /* Full VCO range 60.800000..92.700000 */
				#define FREQ_DENOM 1000000      /* 4 VCOs */
			#else
				#error HYBRID_NVFOS not defined
			#endif
		#else
			#error Wrong BANDSELSTYLERE_xxx used
		#endif
	#else
	
		#define DIRECT_50M0_X8	1	/* DDS LO1, LO2 = 50000 kHz - for 60700 kHz filter */	

	#endif

#endif /* FQMODEL_60700_IF05 || FQMODEL_60700_IF02 */

/* ������ �� - 45 ���, ������ - 13.0 ���, ������ - 500 ��� */
#if FQMODEL_45000_13M_IF0P5

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_LOWER	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	#if WITHIF4DSP
		// ����������� ��� DSP �� ��������� ��
		#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#else /* WITHIF4DSP */
		// ������� ����������� ��� ��������� �����
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
	#endif /* WITHIF4DSP */
	
	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

	#define	LO3_POWER2	0		/* 0 - ��� �������� ����� ���������� LO3 ����� ������� �� ��������� */
	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	#define DIRECT_32M0_X12	1	/* DDS LO1, LO2 = 32000 kHz */	

	#if LO3_SIDE == LOCODE_UPPER
		#define LO3_PLL_R	320		/* ������� ��� ���������� ������ ���������� */
		#define LO3_PLL_N	135		/* LO3 = 13.5 MHz */
	#else /* LO3_UPPER */
		#define LO3_PLL_R	320		/* ������� ��� ���������� ������ ���������� */
		#define LO3_PLL_N	125		/* LO3 = 12.5 MHz */
	#endif /* LO3_UPPER */

	#define IF3_MODEL IF3_TYPE_500

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 32000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 36000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

#endif /* FQMODEL_45000_13M_IF0P5 */

/* ������ �� - 36000 ���, ������ - 500 ��� */
#if FQMODEL_36000_IF500

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	//#define LO1_SIDE_F(freq) (((freq) < 70200000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
#if WITHIF4DSP
	// ����������� ��� DSP �� ��������� ��
	#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
#else /* WITHIF4DSP */
	// ������� ����������� ��� ��������� �����
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
#endif /* WITHIF4DSP */

	#define IF3_MODEL IF3_TYPE_500

	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* ������ ������� ��������� */
		#define TUNE_TOP 32000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if MODEL_DIRECT
		#define DIRECT_35M5_X11	1	/* DDS LO1, LO2 = 36000 kHz - for 70200 kHz filter from EKD */	
	#else
		#define HYBRID_35M5_10M7	1	/* Hybrid LO1, LO2 = 36000 kHz - for 70200 kHz filter from EKD */

		#define SYNTHLOWFREQ 	(36000000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(36000000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			//#define FREQ_DENOM 1000000
			//#define FREQ_SCALE 1102612	// 32 Mhz over 6 VFO
		#elif (HYBRID_NVFOS == 4)
			//#define FREQ_DENOM 1000000
			//#define FREQ_SCALE 1157802	// 32 MHz over 4 VFO
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif

#endif /* FQMODEL_36000_IF500 */

/* ������-��������� �� Si570 "B" grade */
#if FQMODEL_RFSG_SI570

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

	#define LO1MODE_DIRECT		1
	#define IF3_MODEL IF3_TYPE_DCRX

	#define	DDS1_CLK_MUL	1	// ��������
	#define	DDS2_CLK_MUL	1	// ��������
	#define	DDS3_CLK_MUL	1	// ��������
	#define	REFERENCE_FREQ 100000000

	#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
	//#define LO1_POWER2	0		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */
	#define LO1_POWER2	0		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */

	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define TUNE_BOTTOM 4008264L		/* ������ ������� ��������� Si570 */
	#define TUNE_TOP 810000000L		/* ������� ������� ��������� Si570  */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

#endif /* FQMODEL_RFSG_SI570 */

/* ������ �� - 60.725 ���, ������ - 10.725 ���, ������ - 500 ��� */
#if FQMODEL_60725_IF05

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	//#define LO3_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_UPPER 	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	
	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

	#define	LO3_POWER2	0		/* 0 - ��� �������� ����� ���������� LO3 ����� ������� �� ��������� */
	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	#define DIRECT_50M0_X8	1	/* DDS LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */	

	#if LO3_SIDE == LOCODE_UPPER
		#define LO3_PLL_R	(50000/25)		/* ������� ��� ���������� ������ ���������� */
		#define LO3_PLL_N	(11225/25)		/* LO3 = 11.225 MHz */
	#else /* LO3_UPPER */
		#define LO3_PLL_R	(50000/25)		/* ������� ��� ���������� ������ ���������� */
		#define LO3_PLL_N	(10225/25)		/* LO3 = 10.225 MHz */
	#endif /* LO3_UPPER */

	#define IF3_MODEL IF3_TYPE_500

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* ������ ������� ��������� */
		#define TUNE_TOP 56000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLExxx
	#endif /* BANDSELSTYLERE_UPCONV56M */

#endif /* FQMODEL_60725_IF05 */

/* ������ �� - 45000 ���, ������ - 455 ��� ��� 500 ��� ��� 128 ���*/
#if FQMODEL_45_IF455 || FQMODEL_45_IF0P5 || FQMODEL_45_IF128

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */

	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define	LO4_POWER2	2		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	#if 0
		#define DIRECT_25M0_X6	1
		#define IF3_MODEL IF3_TYPE_DCRX
		#define LO2_PLL_R	REFERENCE_FREQ		
		#define LO2_PLL_N	45000000ul		

	#elif FQMODEL_45_IF128

		#if MODEL_DIRECT
			#define DIRECT_50M0_X8_LO4AD9951	1			/* DDS LO1, LO2 = 44500 kHz - for 45000 kHz filter from Kenwood */	
		#else
			#define HYBRID_44M5_10M7	1	/* Hybrid LO1, LO2 = 44500 kHz - for 45000 kHz filter */
		#endif
		//#define FIXED100_44M5		1
		//#define FIXED100_50M0		1
		#define IF3_MODEL IF3_TYPE_128

		#define LO2_DDSGENERATED	1			// dds3 used as LO2 output
		#define LO2_PLL_R	REFERENCE_FREQ		/* ������� ������ ����� ������������ ��� LO2 */
		#define LO2_PLL_N	44872000ul		/* ������� ������ ����� ������������ ��� LO2  */

	#elif FQMODEL_45_IF0P5
		#if MODEL_DIRECT
			#define DIRECT_44M5_X8	1			/* DDS LO1, LO2 = 44500 kHz - for 45000 kHz filter */	
		#else
			#define HYBRID_44M5_10M7	1	/* Hybrid LO1, LO2 = 44500 kHz - for 45000 kHz filter */
		#endif
		//#define FIXED100_44M5		1
		//#define FIXED100_50M0		1
		#define IF3_MODEL IF3_TYPE_500

		#define LO2_PLL_R	REFERENCE_FREQ		/* ������� ������ ����� ������������ ��� LO2 */
		#define LO2_PLL_N	44500000ul		/* ������� ������ ����� ������������ ��� LO2  */
		//#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
		//#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */
	#elif FQMODEL_45_IF455
		#if MODEL_DIRECT
			#define DIRECT_44M545_X8		1
		#else
			#define HYBRID_44M545_10M7	1	/* Hybrid LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */
		#endif
		//#define FIXED100_44M545		1
		//#define FIXED100_50M0		1

		#define IF3_MODEL IF3_TYPE_455

		//#define LO2_PLL_R	REFERENCE_FREQ		/* ������� ������ ����� ������������ ��� LO2 */
		//#define LO2_PLL_N	44545000ul		/* ������� ������ ����� ������������ ��� LO2  */
		#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
		#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */
	#else
		#error Strange configuration...
	#endif

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 32000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 36000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if MODEL_DIRECT
	#else

		#define SYNTHLOWFREQ 	(45000000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(45000000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1157649      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#elif (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1102515      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif


#endif /* MODEL_45_IF455 || MODEL_45_IF0P5 */

/* Nikolai: ������ �� - 45000 ���, ������ - 10700 ��� */
#if FQMODEL_45_IF10700_IF200

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_UPPER	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

	#define DIRECT_34M3_X11	1	/*  */

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */

	#define LO2_PLL_R	343		/* For 0.1 MHz phase detector freq */
	#define LO2_PLL_N	343		/* for 34.3 MHz LO2 */
	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO3_PLL_R	343		/* For 0.1 MHz phase detector freq */
	#define LO3_PLL_N	109		/* for 10.9 MHz LO3 */
	#define	LO3_POWER2	0		/* 0 - ��� �������� ����� ���������� LO3 ����� ������� �� ��������� */
	//#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	#define IF3_MODEL IF3_TYPE_200

	#define WFM_IF1 (10700000UL)	/* 10.7 MHz */

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 32000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 36000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if MODEL_DIRECT
	#else

		#define SYNTHLOWFREQ 	(45000000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(45000000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1157649      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#elif (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1102515      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif

#endif /* FQMODEL_45_IF10700_IF200 */

/* RA6LPO TRX: ������ �� - 45000 ���, ������ - 8868 ���, ������ - 200 ��� */
#if FQMODEL_45_IF8868_IF200

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_LOWER	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */

	//#define DIRECT_36M13_X11	1	/*  */
	#define DIRECT_50M0_X8	1	/*  */
	#define LO2_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
	#define LO2_PLL_N	36130000		/* for 34.3 MHz LO2 */
	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */
	//#define	LO2_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define LO3_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
	#define LO3_PLL_N	8668000		/* for 10.9 MHz LO3 */
	#define	LO3_POWER2	0		/* 0 - ��� �������� ����� ���������� LO3 ����� ������� �� ��������� */
	#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	#define IF3_MODEL IF3_TYPE_200

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 32000000L		/* ������� ������� ��������� */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz ������ ������� ��������� */
		#define TUNE_TOP 36000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if MODEL_DIRECT
	#else

		#define SYNTHLOWFREQ 	(45000000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(45000000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1157649      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#elif (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1102515      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif

#endif /* FQMODEL_45_IF8868_IF200 */

#if FQMODEL_TRX5M_UA1FA		// ������ ������ ��, ������ �� 500 ���

	#if ! defined (IF3_MODEL)
		#error Please define IF3_MODEL as IF3_TYPE_XXXX
		//#define IF3_MODEL IF3_TYPE_500
	#endif
	// ������ DDS ������������ ��� ���������
	#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
	//#define LO1_POWER2	1		/* 8 - ��� ������ ������� (4 - � ��������� ���, 2 - ������������� ������� �� ������ ������������ ��������) */
	#define LO1_POWER2	0		/* 0 - ��� �������� ����� ���������� LO1 ����� ������� �� ��������� */
	//#define LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO3_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
	#define LO3_PLL_N	5000000UL		/* for 5.000 MHz LO3 */
	#define	LO3_POWER2	0		/* 0 - ��� �������� ����� ���������� LO3 ����� ������� �� ��������� */
	#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#if WITHLO4FREQPOWER2
		#define LO4FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ���������� ���������� ��������� ����� ���� */
		#define	LO4_POWER2	1		/* 1 - �������� �� 2 ����� ���������� LO4 ����� ������� �� ��������� */
	#else
		#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */
	#endif

	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#if WITHFIXEDBFO
		/* �������� ��������� �������������� ������� � ������ */
		#define LO1_SIDE	LOCODE_TARGETED	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
		#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		/* LO4_SIDE ������������� � getsidelo4() � ����� ���� LOCODE_UPPER ��� LOCODE_LOWER */
	#elif WITHCLASSICSIDE
		//#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
		#define LO1_SIDE_F(freq) (((freq) < 13950000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
		#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	#else
		// WITHDUALBFO ��� ������������ ������� � AD9834 � BFO 
		/* �������� ��������� �������������� ������� � ������ */
		#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
		#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	#endif

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define TUNE_BOTTOM 1600000L		/* ������ ������� ��������� */
	#define TUNE_TOP 32000000L		/* ������� ������� ��������� */

#endif	/* FQMODEL_TRX5M_UA1FA */

#if FQMODEL_TRX8M2CONV	// ������ ������ ��, ����� ��� �� 200/455/500 ���

	// �����  ���� ������� �������� ��������� �� �������������� ����������

	#if ! defined (IF3_MODEL)
		#error Please define IF3_MODEL as IF3_TYPE_XXXX
	#endif /* ! defined (IF3_MODEL) */

	// ������ DDS ������������ ��� ���������
	#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
	//#define LO1_POWER2	1		/* 8 - ��� ������ ������� (4 - � ��������� ���, 2 - ������������� ������� �� ������ ������������ ��������) */
	#define LO1_POWER2	0		/* 0 - ��� �������� ����� ���������� LO1 ����� ������� �� ��������� */
	//#define LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO3_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
	#define LO3_PLL_N	8900000		/* for 8.9 MHz LO3 */
	#define	LO3_POWER2	0		/* 0 - ��� �������� ����� ���������� LO3 ����� ������� �� ��������� */
	#define	LO3_FREQADJ	1	/* ���������� ������� ���������� ����� ����. */

	#if WITHLO4FREQPOWER2
		#define LO4FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ���������� ���������� ��������� ����� ���� */
		#define	LO4_POWER2	1		/* 1 - �������� �� 2 ����� ���������� LO4 ����� ������� �� ��������� */
	#else
		#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */
	#endif

	// WITHDUALBFO ��� ������������ ������� � AD9834 � BFO 
	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

	#if MODEL_DIRECT
		
	#elif MODEL_HYBRID
		// ��������� ��� ����������� ������ ������
		#define HYBRID_OCTAVE_80_160	1	/* ����� � 4 ��� */
	#else
		#error VFO model not specified
	#endif
	
	#define TUNE_BOTTOM 1500000L
	#define TUNE_TOP 32000000L

#endif // MODEL_TRX8M

#if FQMODEL_TRX8M	// ������ ������ �� 8 ���, 6 ���, 5.5 ��� � ������

	// �����  ���� ������� �������� ��������� �� �������������� ����������

	#if ! defined (IF3_MODEL)
		#error Please define IF3_MODEL as IF3_TYPE_XXXX
		//#define IF3_MODEL IF3_TYPE_10000
		//#define IF3_MODEL IF3_TYPE_8000
		//#define IF3_MODEL IF3_TYPE_5000
		//#define IF3_MODEL IF3_TYPE_6000
		//#define IF3_MODEL IF3_TYPE_9000
		//#define IF3_MODEL IF3_TYPE_8868
		//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
		//#define IF3_MODEL IF3_TYPE_5500
		//#define IF3_MODEL IF3_TYPE_9045
		////#define IF3_MODEL IF3_TYPE_500 ������������ FQMODEL_TRX500K
		//#define IF3_MODEL IF3_TYPE_455
	#endif /* ! defined (IF3_MODEL) */

	// ������ DDS ������������ ��� ���������
	#ifndef LO1FDIV_ADJ
		#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
		//#define LO1_POWER2	1		/* 8 - ��� ������ ������� (4 - � ��������� ���, 2 - ������������� ������� �� ������ ������������ ��������) */
		#define LO1_POWER2	0		/* 0 - ��� �������� ����� ���������� LO1 ����� ������� �� ��������� */
	
	#endif	/* LO1FDIV_ADJ */

	//#define LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#if WITHLO4FREQPOWER2
		#define LO4FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ���������� ���������� ��������� ����� ���� */
		#define	LO4_POWER2	1		/* 1 - �������� �� 2 ����� ���������� LO4 ����� ������� �� ��������� */
	#else
		#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */
	#endif

	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#if WITHFIXEDBFO
		/* �������� ��������� �������������� ������� � ������ */
		#define LO1_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		/* LO4_SIDE ������������� � getsidelo4() � ����� ���� LOCODE_UPPER ��� LOCODE_LOWER */
	#elif WITHCLASSICSIDE
		//#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
		#define LO1_SIDE_F(freq) (((freq) < 13950000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
		#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	#else
		// WITHDUALBFO ��� ������������ ������� � AD9834 � BFO 
		/* �������� ��������� �������������� ������� � ������ */
		#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
		#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	#endif

	#if MODEL_DIRECT
		
	#elif MODEL_HYBRID
		// ��������� ��� ����������� ������ ������
		#define HYBRID_OCTAVE_80_160	1	/* ����� � 4 ��� */
	#else
		#error VFO model not specified
	#endif
	
	#if BANDSELSTYLERE_LOCONV32M_NLB
		#define TUNE_BOTTOM 1500000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV32M
		#define TUNE_BOTTOM 100000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLE_LADVABEST
		#define TUNE_BOTTOM 1600000L			/* ������ ������� ��������� */
		#define TUNE_TOP 56000000L
	#else
		#error Strange BANDSELSTYLExxx
	#endif


#endif // MODEL_TRX8M


// ������ �� 500 ���
#if FQMODEL_TRX500K || FQMODEL_TRX455K

	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#if WITHFIXEDBFO
		/* �������� ��������� �������������� ������� � ������ */
		#define LO1_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		/* LO4_SIDE ������������� � getsidelo4() � ����� ���� LOCODE_UPPER ��� LOCODE_LOWER */
	#else
		/* �������� ��������� �������������� ������� � ������ */
		#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
		#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	#endif
	// ����� ���� ������� �������� ��������� �� �������������� ����������

	#if FQMODEL_TRX500K
		#define IF3_MODEL IF3_TYPE_500
	#elif FQMODEL_TRX455K
		#define IF3_MODEL IF3_TYPE_455
	#else
		#erros Strange FQMODEL_XXX value
	#endif

	// ������ DDS ������������ ��� ���������
	#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
	//#define LO1_POWER2	0		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */
	#define LO1_POWER2	0		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */


	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */
	#define LO2_PLL_R	1		/* LO2 = 0 MHz */
	#define LO2_PLL_N	0		/* LO2 = 0 MHz */

	/* Board hardware configuration */
	#if MODEL_DIRECT

	#else
		// ��������� ��� ����������� ������ ������
		#define HYBRID_OCTAVE_80_160	1	/* ����� � 4 ��� */
	#endif

	#if BANDSELSTYLERE_LOCONV32M_NLB
		#define TUNE_BOTTOM 1500000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV32M
		#define TUNE_BOTTOM 100000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV15M_NLB
		#define TUNE_BOTTOM 1600000L	// 1.6 ���
		#define TUNE_TOP 15000000L		// � 500 ��� ������ ������������� �� 10.999999
	#elif BANDSELSTYLERE_LOCONV15M
		#define TUNE_BOTTOM 100000L		// 100 ���
		#define TUNE_TOP 15000000L		// � 500 ��� ������ ������������� �� 10.999999
	#else
		#error Strange BANDSELSTYLExxx
	#endif

#endif // FQMODEL_TRX500K


// �����������
#if FQMODEL_UW3DI

	// ����������� LO1 � LO2 ��� ������� ����������� �������� �������.
	// � ������ ��������� LO1 ��� ������ ��������� � ������� ����������

	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#if WITHFIXEDBFO
		/* �������� ��������� �������������� ������� � ������ */
		#define LO1_SIDE_F(freq) (((freq) < 60000000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
		//#define LO1_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO2_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		/* LO4_SIDE ������������� � getsidelo4() � ����� ���� LOCODE_UPPER ��� LOCODE_LOWER */
	#else
		#error Use WITHFIXEDBFO with FQMODEL_UW3DI
		/* �������� ��������� �������������� ������� � ������ */
		//#define LO1_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		//#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		//#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		//#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	#endif
	// ����� ���� ������� �������� ��������� �� �������������� ����������

	#define IF3_MODEL IF3_TYPE_500

	//#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
	//#define LO1_POWER2	0		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */
	#define LO1_POWER2	0		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */


	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	#define HINTLO2_GRANULARITY (5L * 100 * 1000)	/* ������������ �������� ��� � ����� 500 ��� */
	#define IF2FREQBOTOM	6000000L		/* ����������� ������� ��������� ����������������� ��������� */
	#define IF2FREQTOP		6500000L		/* ������������ ������� ��������� ����������������� ��������� */

	/* Board hardware configuration */
	#if MODEL_DIRECT

	#elif MODEL_MAXLAB

		#define SYNTHLOWFREQ 5480000L	/* ����������� ������� ��� */
		#define DDS1_CLK_MUL 1

	#elif MODEL_HYBRID
		#define SYNTHLOWFREQ 5480000L	/* ����������� ������� ��� */
		#define SYNTHTOPFREQ 6020000L	/* ����������� ������� ��� */
		#define HYBRID_NVFOS 1
	#else
		// ��������� ��� ����������� ������ ������
		#define HYBRID_OCTAVE_80_160	1	/* ����� � 4 ��� */
	#endif

	#define TUNE_BOTTOM 1500000L
	#define TUNE_TOP 30000000L

#endif // FQMODEL_UW3DI


// ������ ��������������
#if FQMODEL_DCTRX

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#if WITHIF4DSP
		/* �������� ��������� �������������� ������� � ������ */
		#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO4_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#else
		/* �������� ��������� �������������� ������� � ������ */
		#define LO1_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
		#define LO2_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	#endif

	#if 1

		#define IF3_MODEL IF3_TYPE_DCRX

		// ������ DDS ������������ ��� ���������
		#if 1
			#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
			#define LO1_POWER2	2		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */
		#else
			//#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
			#define LO1_POWER2	0		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */
		#endif


	// -- parameters for 128 MHz..256 MHz
	#elif 0

		#define IF3_MODEL IF3_TYPE_DCRX
		//#define IF3_MODEL IF3_TYPE_8000
		//#define IF3_MODEL IF3_TYPE_8868

		/* ����� � ��������� ������������ ��� ������� ������� �������������� */
		#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
		#define LO1_POWER2	3		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */

		#define LO1MODE_HYBRID	1
		#define HYBRID_OCTAVE_128_256	1
	#endif // structure versions

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L	// 30 kHz
		#define TUNE_TOP 56000000L	// 56 MHz
	#elif BANDSELSTYLERE_LOCONV32M_NLB
		#define TUNE_BOTTOM 1600000L	// 1.6 ���
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV32M
		#define TUNE_BOTTOM 100000L		// 100 ���
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV15M_NLB
		#define TUNE_BOTTOM 1600000L	// 1.6 ���
		#define TUNE_TOP 11000000L		// � 500 ��� ������ ������������� �� 10.999999
	#elif BANDSELSTYLERE_LOCONV15M
		#define TUNE_BOTTOM 100000L		// 100 ���
		#define TUNE_TOP 11000000L		// � 500 ��� ������ ������������� �� 10.999999
	#else
		#error Strange BANDSELSTYLExxx
	#endif

	/* +++ ��������� ����������� � ������������� ����. ��������� ������� DDS */
	//#define LO1MODE_FIXSCALE	1	
	//#define SYNTH_N1 256
	//#define SYNTH_R1 1
	/* --- ��������� ����������� � ������������� ����. ��������� ������� DDS */

#endif // MODEL_DCTRX

#if FQMODEL_23M_IF500

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

	#define DIRECT_22M5_X17	1	/* R-143 */
	#define IF3_MODEL IF3_TYPE_500

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 30000L		/* ������ ������� ��������� */
		#define TUNE_TOP 32000000L		/* ������� ������� ��������� */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /*  */

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */
	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */
	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	//#define WFM_IF1 (10700000UL)	/* 10.7 MHz */
#endif /* FQMODEL_23M_IF500 */


/* For UT1MB */
#if FQMODEL_45M_46615120_IF8868

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

	#define DIRECT_46615120_X8	1	/* SW2011 TRX scheme V.1 */
	#define IF3_MODEL IF3_TYPE_8868

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO2_PLL_R	46615120UL		/* ����� 1 ��� */
	#define LO2_PLL_N	36132000UL		/* ��� ������������ ��� LO2  */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	#define WFM_IF1 (10700000UL)	/* 10.7 MHz */

	#define TUNE_BOTTOM 100000L		/* ������ ������� ��������� */
	#define TUNE_TOP 36000000L		/* ������� ������� ��������� */

	#if MODEL_DIRECT
	#else

		#define SYNTHLOWFREQ 	(45000000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(45000000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1157649      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#elif (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1102515      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif

#endif /* FQMODEL_45M_46615120_IF8868 */



/* SW2011 TRX: ������ �� - 45000 ���, ������ - 8868 ��� */
#if \
	FQMODEL_45_IF8868 || \
	FQMODEL_45_IF8868_UHF144 || FQMODEL_45_IF8868_UHF430 || \
	FQMODEL_45_IF6000 || \
	FQMODEL_45_IF6000_UHF144 || FQMODEL_45_IF6000_UHF430 || \
	FQMODEL_40_IF8000 || FQMODEL_40_IF8000_UHF144 || \
	FQMODEL_45_IF5000 || FQMODEL_45_IF5000_UHF144 || \
	FQMODEL_45_IF8000_UHF433_REF53 || \
	0

	#if FQMODEL_45_IF8000_UHF433_REF53
		#define DIRECT_53M00_X7	1	/* ladvabest@gmail.com */
		#define IF3_MODEL IF3_TYPE_8000
	#elif FQMODEL_45_IF6000 || FQMODEL_45_IF6000_UHF144 || FQMODEL_45_IF6000_UHF430
		#if CTLSTYLE_SW2014FM
			#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000_SW2014NFM
		#elif CTLSTYLE_SW2015
			#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000_SW2015	/* ������ ���� �� ������� ������� ���� */
		#elif CTLSTYLE_SW2016
			#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000_SW2015	/* ������ ���� �� ������� ������� ���� */
		#elif CTLSTYLE_SW2016VHF
			#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000_SW2015	/* ������ ���� �� ������� ������� ���� */
		#elif CTLSTYLE_SW2016MINI
			#define DIRECT_39M00_X10_EXACT	1	/* SW2016 MINI TRX */
			#define IF3_MODEL IF3_TYPE_6000_SW2016MINI
		#else /* CTLSTYLE_SW2014FM */
			#define DIRECT_39M00_X10	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000
		#endif /* CTLSTYLE_SW2014FM */
	#elif FQMODEL_45_IF8868 || FQMODEL_45_IF8868_UHF144 || FQMODEL_45_IF8868_UHF430
		#define DIRECT_36M13_X11	1	/* SW2011 TRX scheme V.1 */
		#define IF3_MODEL IF3_TYPE_8868
	#elif FQMODEL_40_IF8000 || FQMODEL_40_IF8000_UHF144
		#define DIRECT_40M0_X10	1
		#define IF3_MODEL IF3_TYPE_8000
	#elif FQMODEL_45_IF5000 || FQMODEL_45_IF5000_UHF144
		#define DIRECT_40M0_X10	1
		#define IF3_MODEL IF3_TYPE_5000
	#endif

	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */

	#if FQMODEL_45_IF8000_UHF433_REF53

		/* �������� ��������� �������������� ������� � ������ */
		#define LO2_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#if WITHIF4DSP
			// ����������� ��� DSP �� ��������� ��
			#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
			#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#else /* WITHIF4DSP */
			// ������� ����������� ��� ��������� �����
			#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
			//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
		#endif /* WITHIF4DSP */

		//#define TUNE_BOTTOM 30000L			/* ������ ������� ��������� */
		#define TUNE_BOTTOM 1600000L			/* ������ ������� ��������� */
		#define TUNE_TOP 490000000L		/* ������� ������� ��������� */

	#elif FQMODEL_45_IF8868_UHF144 || FQMODEL_45_IF6000_UHF144 || FQMODEL_40_IF8000_UHF144 || FQMODEL_45_IF5000_UHF144

		/* �������� ��������� �������������� ������� � ������ */
		#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#if WITHIF4DSP
			// ����������� ��� DSP �� ��������� ��
			#define LO4_SIDE	LOCODE_LOWER	/* �������� �������� �� ���������� ������ ��� ����� ��� FQMODEL_80455. ��� �������������� �� ���� ���������� ��� �������� ������� */
			#define LO5_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#else /* WITHIF4DSP */
			// ������� ����������� ��� ��������� �����
			#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
			//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: ��� ������������� ���������� - �� �� ��� �������� ������� */
		#endif /* WITHIF4DSP */

		#define TUNE_BOTTOM 30000L			/* ������ ������� ��������� */
		//#define TUNE_BOTTOM 1600000L			/* ������ ������� ��������� */
		#define TUNE_TOP 170000000L		/* ������� ������� ��������� */

	#elif FQMODEL_45_IF8868_UHF430 || FQMODEL_45_IF6000_UHF430	// UR7GP request

		/* �������� ��������� �������������� ������� � ������ */
		#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

		#define TUNE_BOTTOM 30000L			/* ������ ������� ��������� */
		#define TUNE_TOP 470000000L		/* ������� ������� ��������� */

	#else				// ������ HF 

		/* �������� ��������� �������������� ������� � ������ */
		#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
		#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

		#define TUNE_BOTTOM 30000L			/* ������ ������� ��������� */
		#define TUNE_TOP 32000000L		/* ������� ������� ��������� */

	#endif

	#if CTLSTYLE_SW2016VHF
		#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#else /* CTLSTYLE_SW2016VHF */
		#define LO1_SIDE_F(freq) (((freq) < 80000000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* ������� ������� �� ������� ������� */
	#endif /* CTLSTYLE_SW2016VHF */


	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */
	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */
	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */
	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	#define WFM_IF1 (10700000UL)	/* 10.7 MHz */

#endif /* FQMODEL_45_IF8868 */

/* ������ �� - 58.1125 ���, ������ - 8868 ��� - ��� gena, DL6RF */
#if FQMODEL_58M11_IF8868

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_UPPER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

	#define DIRECT_49M247_X8	1		/* SW2011 TRX from DL6RF */
	#define IF3_MODEL IF3_TYPE_8868

	#define TUNE_BOTTOM 30000L			/* ������ ������� ��������� */
	#define TUNE_TOP 32000000L		/* ������� ������� ��������� */

	#define LO1_POWER2	0		/* ���� 0 - ��������� � ������ ������� ���������� ����� ���������� ���. */

	#define	LO2_POWER2	0		/* 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define LO2_PLL_R	1		/* ������� ������ ����� ������������ ��� LO2 */
	#define LO2_PLL_N	1		/* ������� ������ ����� ������������ ��� LO2  */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	#define WFM_IF1 (10700000UL)	/* 10.7 MHz */

#endif /* FQMODEL_45_IF8868 */

// CML evaluation board with CMX992 chip.
// ������ �� 45 ���
// ������ �� 10.7 ���
#if FQMODEL_GEN500

	/* �������� ��������� �������������� ������� � ������ */
	#define LO0_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO1_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
	#define LO2_SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define LO3_SIDE	LOCODE_INVALID	/* ��� ������������� ���������� - �� �� ��� �������� ������� */
	#define LO4_SIDE	LOCODE_LOWER	/* ���� ��������� ����������� ��� ��������� ��������� ������� */

	#define IF3_MODEL IF3_TYPE_DCRX

	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	12800000L	/* LO2 = 63955 kHz - for 64455 kHz filter from ICOM with 500 kHz IF2 */
	//#define REFERENCE_FREQ	19200000L	/* LO2 = 63955 kHz - for 64455 kHz filter from ICOM with 500 kHz IF2 */
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */

	//#define	LO2_PLL_N	(450 * 4)	// 45 * 4 MHz
	#define	LO2_PLL_N	(107 * 4)	// 10.7 * 5 MHz
	#define LO2_PLL_R	128		// 100 kHz ref freq
	//#define LO2_PLL_R	192		// 100 kHz ref freq

	// ������ DDS ������������ ��� ���������
	//#define LO1FDIV_ADJ	1		/* ���� ���������� - ���������� �������� � ������ ������� ���������� ��������� ����� ���� */
	//#define LO1_POWER2	0		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */
	#define LO1_POWER2	0		/* 8 - ��� ������ ������� (4 - � ���������, 2 - ������������� ������� �� ������ ������������ ��������) */

	#define	LO2_POWER2	2		/* 2 - /4, 0 - ��� �������� ����� ���������� LO2 ����� ������� �� ��������� */

	#define	LO4_POWER2	0		/* 0 - ��� �������� ����� ���������� LO4 ����� ������� �� ��������� */

	/* Board hardware configuration */
	//#define HYBRID_500_600	1
	//#define DIRECT_100M0_X4		1	/* Board hardware configuration */
	//#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	//#define DIRECT_40M0_X10		1	/* Board hardware configuration */
	//#define DIRECT_36M13_X11	1	/* SW2011 TRX scheme */
	//#define DIRECT_72M595_X5	1	/* ��� �������� � ��������� � ����� �������� �� ������ */

	// ��������� ��� ����������� ������ ������
	//#define HYBRID_OCTAVE_80_160	1	/* ����� � 4 ��� */


	#define TUNE_BOTTOM 470000000L
	#define TUNE_TOP 500000000L

	#define SYNTHLOWFREQ 470000000L
	#define FREQ_SCALE 120
	#define FREQ_DENOM 100
	#define HYBRID_NVFOS 6

#endif /* FQMODEL_GEN500 */

// � ����������� �� ��������� ����� - ��������� �������������� ����� �����������

#if DIRECT_63M9_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	63955000L	/* LO2 = 63955 kHz - for 64455 kHz filter from ICOM with 500 kHz IF2 */
	#define DDS1_CLK_MUL	6 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_63M9_X6 */

#if DIRECT_63M9_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	63955000L	/* LO2 = 63955 kHz - for 64455 kHz filter from ICOM with 500 kHz IF2 */
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_63M9_X1 */

#if DIRECT_64M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	64000000L	/* LO2 = 64000 kHz - for 64455 kHz filter from ICOM with 455 kHz IF2 */
	#define DDS1_CLK_MUL	6 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_64M0_X6 */

#if DIRECT_64M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	64000000L	/* LO2 = 64000 kHz - for 64455 kHz filter from ICOM with 455 kHz IF2 */
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_64M0_X1 */

#if DIRECT_44M545_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	44545000L	/* LO2 = 45 MHz 1-st IF with 455 kHz IF2 */
	#define DDS1_CLK_MUL	8 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_44M545_X8 */

#if DIRECT_44M5_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	44500000L	/* LO2 = for 45000 kHz filter with 500 kHz IF2 */
	#define DDS1_CLK_MUL	8 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_44M5_X8 */

#if DIRECT_25M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	25000000ul	
	#define DDS1_CLK_MUL	6 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_25M0_X6 */

#if DIRECT_72M550_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	72550000L	/* LO2 = 72550 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define DDS1_CLK_MUL	5 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_72M550_X5 */

#if DIRECT_40M0_X10
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	40000000ul
	#define DDS1_CLK_MUL	10 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1 		/* ���������� � DDS3 */
#endif	/* DIRECT_40M0_X10 */

#if DIRECT_100M0_X4
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	100000000ul
	#define DDS1_CLK_MUL	4 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1 		/* ���������� � DDS3 */
#endif	/* DIRECT_100M0_X4 */

#if DIRECT_40M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	40000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1 		/* ���������� � DDS3 */
#endif	/* DIRECT_40M0_X1 */

#if DIRECT_25M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	25000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1 		/* ���������� � DDS3 */
#endif	/* DIRECT_25M0_X1 */

#if DIRECT_26M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	26000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS2 */
#endif	/* DIRECT_26M0_X1 */

#if DIRECT_27M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	27000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS2 */
#endif	/* DIRECT_27M0_X1 */

#if DIRECT_100M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	100000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_100M0_X1 */

#if DIRECT_125M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	125000000uL
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_125M0_X1 */

#if DIRECT_122M88_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	122880000uL
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_122M88_X1 */

#if DIRECT_12M288_X10
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	12288000uL
	#define DDS1_CLK_MUL	10 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	10		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	10		/* ���������� � DDS3 */
#endif	/* DIRECT_12M288_X10 */

#if DIRECT_XXXX_X1
	#define LO1MODE_DIRECT	1
	//#define REFERENCE_FREQ	122880000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_XXXX_X1 */

#if DIRECT_160M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	160000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_160M0_X1 */

#if DIRECT_16M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	16000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_16M0_X1 */

#if DIRECT_48M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	48000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_48M0_X1 */

#if FIXSCALE_48M0_X1_DIV256
	#define LO1MODE_FIXSCALE	1
	#define SYNTH_N1	256
	#define SYNTH_R1	1
	#define REFERENCE_FREQ	48000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
	#define HYBRID_NVFOS	0
#endif	/* FIXSCALE_48M0_X1_DIV256 */

#if DIRECT_44M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	44000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_44M0_X1 */

#if DIRECT_50M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	50000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_50M0_X1 */

#if DIRECT_75M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	75000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_75M0_X1 */

#if DIRECT_80M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	80000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS2 */
#endif	/* DIRECT_80M0_X1 */

#if DIRECT_60M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	60000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS2 */
#endif	/* DIRECT_60M0_X1 */

#if DIRECT_48M0_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	48000000ul
	#define DDS1_CLK_MUL	8 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_48M0_X8 */

#if DIRECT_80M0_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	80000000ul
	#define DDS1_CLK_MUL	5 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_80M0_X5 */

#if DIRECT_32M0_X12
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	32000000ul
	#define DDS1_CLK_MUL	12 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_32M0_X12 */

#if DIRECT_35M5_X11
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	35500000ul
	#define DDS1_CLK_MUL	11 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_35M5_X11 */

#if DIRECT_50M0_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	50000000ul
	#define DDS1_CLK_MUL	8 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_50M0_X8 */

#if DIRECT_60M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	60000000ul
	#define DDS1_CLK_MUL	6 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_50M0_X8 */

#if DIRECT_50M0_X8_LO4AD9951
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	50000000ul
	#define DDS1_CLK_MUL	8 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	8		/* ���������� � DDS3 */
#endif	/* DIRECT_50M0_X8_LO4AD9951 */

#if DIRECT_63M8976_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	63897600ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1 		/* ���������� � DDS3 */
#endif	/* DIRECT_50M0_X8 */

#if DIRECT_63M8976_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	63897600ul
	#define DDS1_CLK_MUL	5 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1 		/* ���������� � DDS3 */
#endif	/* DIRECT_63M8976_X5 */

#if DIRECT_30M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	30000000ul
	#define DDS1_CLK_MUL	6 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1 		/* ���������� � DDS3 */
#endif	/* DIRECT_30M0_X6 */

#if DIRECT_32M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	32000000ul
	#define DDS1_CLK_MUL	6 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1 		/* ���������� � DDS3 */
#endif	/* DIRECT_32M0_X6 */

#if DIRECT_66M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	66000000ul
	#define DDS1_CLK_MUL	6 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_66M0_X6 */

#if DIRECT_66M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	66000000ul
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_66M0_X1 */

/* SW2011 TRX from DL6RF, 2-nd if = 8865 kHz */
#if DIRECT_49M247_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	49247000UL	/* LO2 = 49.247 MHz */
	#define DDS1_CLK_MUL	8 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_49M247_X8 */

#if DIRECT_34M3_X11
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	34300000ul
	#define DDS1_CLK_MUL	11 //5 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_34M3_X11 */
/* SW2011 TRX scheme V1 */

#if DIRECT_46615120_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	46615120ul
	#define DDS1_CLK_MUL	8 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_46615120_X8 */

#if DIRECT_36M13_X11
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	36131000ul
	#define DDS1_CLK_MUL	11 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_36M13_X11 */

/* Board hardware configuration for Si5351a */
#if DIRECT_39M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	39000000uL
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_39M0_X1 */

/* R-143  */
#if DIRECT_22M5_X17
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	22500000ul
	#define DDS1_CLK_MUL	17 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif /* DIRECT_22M5_X17 */

/* SW2011 TRX scheme V2 */
#if DIRECT_39M00_X10
	#define LO1MODE_DIRECT	1
	#ifdef ATMEGA_CTLSTYLE_V9SF_US2IT_H_INCLUDED
		#define REFERENCE_FREQ	39000000L
	#else
		#define REFERENCE_FREQ	39002000L	//38994000L	//38997000ul
	#endif
	#define DDS1_CLK_MUL	10 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_39M00_X10 */

/* ladvabest@gmail.com */
#if DIRECT_53M00_X7
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	53000000L
	#define DDS1_CLK_MUL	7 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_53M00_X7 */

/* CTLREGSTYLE_SW2014NFM TRX - ����������� ������� ��������� ������ ������ */
#if DIRECT_39M00_X10_EXACT
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	39000000L
	#define DDS1_CLK_MUL	10 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_39M00_X10_EXACT */

/* SW2011 TRX scheme V2 */
#if DIRECT_40M00_X10
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	40000000ul
	#define DDS1_CLK_MUL	10 //5 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_36M2_10M7 */

#if DIRECT_DDS_ATTINY2313
	//#define REFERENCE_FREQ	18432000UL	// 18.432 kHz - ��, ��� ��������� � �������
	#define REFERENCE_FREQ	20000000L	// 18.432 kHz - ��, ��� ��������� � �������
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS1_CLK_DIV	9		/* �������� ������� ������� ����� ������� � DDS1 (7=NCO, 9=DDS) */

#endif /* DIRECT_DDS_ATTINY2313 */

#if HYBRID_64M0_13M0
	/* � �������� ������������� ������� �� DDS ������������ ����������� 13.0 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	64000000ul
	#define SYNTH_CFCENTER	13000000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1 25	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_64M0_13M0 */

#if HYBRID_64M0_10M7
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	64000000L	/* At 10.6666 MHz is a center of mirroring */
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1 21	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_64M0_10M7 */

#if HYBRID_63M9_10M7
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	63955000L	/* 10659166,66 is a center of mrroring */
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1 23	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_63M9_10M7 */

#if HYBRID_80M0_10M7
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	80000000L	/*  */
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1 15	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_80M0_10M7 */

#if HYBRID_50M0_10M7
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	50000000L	/*  */
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1 21	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_50M0_10M7 */

#if HYBRID_63M9_13M0
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	63955000L
	#define SYNTH_CFCENTER	13000000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1 25	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define LO2_PLL_R	10		/* For 6.4 MHz phase detector freq */
	#define LO2_PLL_N	10		/* for 64 MHz LO2 */
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_64M0_13M0 */

#if HYBRID_500_600
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	100000000L	/*  */
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1 11	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define LO2_PLL_R	1000		/* For 0.1 MHz phase detector freq */
	#define LO2_PLL_N	1800		/* for 180 MHz LO2 */
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_500_600 */

#if HYBRID_12M8_10M7
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	12800000ul
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1 21	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define LO2_PLL_R	4		/* For 3.2 MHz phase detector freq */
	#define LO2_PLL_N	20		/* for 64 MHz LO2 */
	#define DDS1_CLK_MUL	4		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_12M8_10M7 */

#if DIRECT_72M595_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	72595000L	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define DDS1_CLK_MUL	5 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_72M595_X5 */

#if DIRECT_72M595_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	72595000L	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define DDS1_CLK_MUL	1 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_72M595_X5 */

// FQMODEL_73050_XXX should be defined
#if HYBRID_72M595_10M7
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	72595000L	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1	15	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_36M2_10M7 */

// FQMODEL_70455 jr FQMODEL_70200 use this definitions
#if DIRECT_70M000_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	70000000L	/* LO2 = 70000 kHz - for 70455 kHz filter from Kenwood with 455 kHz IF2 */
	#define DDS1_CLK_MUL	5 		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* DIRECT_70M000_X5 */

// FQMODEL_70455 should be defined
#if HYBRID_70M000_10M7
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	70000000L	/* LO2 = 70000 kHz - for 70455 kHz filter from Kenwood with 455 kHz IF2 */
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1	15	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_70M000_10M7 */

// MODEL_73050 should be defined
#if HYBRID_72M550_10M7
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	72550000L	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1		15	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif	/* HYBRID_72M550_10M7 */

#if MAXLAB_7M2
	#define LO1MODE_MAXLAB	1	
	#define REFERENCE_FREQ	7200000L	// 7.2 ��� ��������� �������� - ����������� ������� �� ������ �������� ����������
	//#define REFERENCE_FREQ	8000000L	// 7.2 ��� ��������� �������� - ����������� ������� �� ������ �������� ����������
	// ��������� ���������������� PLL � ������� ���������
	#define SYNTH_R1 7200
	#define LM7001_PLL1_RCODE	LM7001_RCODE_7200

	#if 0 // CPUSTYLE_ARM
		#define MAXLAB_DACARRAY 1	/* ����������� ���������� ��� �������� ������������� ������� - �������� � ������� */
		#define MAXLAB_LINEARDACRANGE 128	// �� ������������� �������� �� ��� �� 0 �� MAXLAB_LINEARDACRANGE - 1
	#else
		//#define MAXLAB_DACARRAY 1	/* ����������� ���������� ��� �������� ������������� ������� - �������� � ������� */
		#define MAXLAB_LINEARDACRANGE 4096	// �� ������������� �������� �� ��� �� 0 �� MAXLAB_LINEARDACRANGE - 1
	#endif
#endif	/* MAXLAB_7M2 */

#if FIXED100_44M5
	#define LO1MODE_FIXSCALE 1
	#define REFERENCE_FREQ	44500000L
	#define SYNTH_N1		100		/* �������� ����� VFO - ���������� ������� ��� ��� PLL */
	#define SYNTH_R1		1
	#define DDS1_CLK_MUL	1
	#define DDS2_CLK_MUL	1
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif

#if FIXED100_44M545
	#define LO1MODE_FIXSCALE 1
	#define REFERENCE_FREQ	45545000L
	#define SYNTH_N1		100		/* �������� ����� VFO - ���������� ������� ��� ��� PLL */
	#define SYNTH_R1		1
	#define DDS1_CLK_MUL	1
	#define DDS2_CLK_MUL	1
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif

#if FIXED100_50M0
	#define LO1MODE_FIXSCALE 1
	#define REFERENCE_FREQ	50000000L
	#define SYNTH_N1		100		/* �������� ����� VFO - ���������� ������� ��� ��� PLL */
	#define SYNTH_R1		1
	#define DDS1_CLK_MUL	1
	#define DDS2_CLK_MUL	1
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif

#if HYBRID_44M5_10M7
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	12800000L
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1		25	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define DDS1_CLK_MUL	1
	#define DDS2_CLK_MUL	1
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif // HYBRID_44M5_10M7 */

#if HYBRID_44M545_10M7
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	44545000UL
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	#define SYNTH_R1		25	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define DDS1_CLK_MUL	1
	#define DDS2_CLK_MUL	1
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */
#endif // HYBRID_44M545_10M7 */

#if HYBRID_OCTAVE_128_256
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	// ++ hybrid synthesizer parameters for 128 MHz..256 MHz
	#define SYNTHLOWFREQ 	128000000L	/* Lowest frequency of 1-st LO */
	#define SYNTHTOPFREQ 	256000000L	/* Highest frequency of 1-st LO */
	#define FREQ_DENOM 1000000L
	#define FREQ_SCALE 1189207L
	#define HYBRID_NVFOS 4
	#define SYNTH_R1 13	/* divider afrer DSS1 unit */
	// -- parameters for 128 MHz..256 MHz

	// ++ hybrid synthesizer parameters for 0.1..75 MHz with IF=80.455
	//#define LO1MODE_HYBRID	1
	//#define SYNTHLOWFREQ 	80555000	/* Lowest frequency of 1-st LO */
	//#define SYNTHTOPFREQ 	155455000	/* Highest frequency of 1-st LO */
	//#define FREQ_DENOM 1000000
	//#define FREQ_SCALE 1178632L
	//#define SYNTH_R1 15	/* divider afrer DSS1 unit */
	// -- parameters for 0.1..75 MHz with IF=80.455

	#define REFERENCE_FREQ	12800000ul
	#define SYNTH_CFCENTER	10700000L	/* ����� ������ ����������� ������� ����� DDS */
	//#define REFERENCE_FREQ	32000000ul
	//#define SYNTH_CFCENTER	10760000L	/* ����� ������ ����������� ������� ����� DDS */
	#define LO2_PLL_N	0		/* LO2 = 0 MHz */

	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	//#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	//#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */

#endif

#if HYBRID_OCTAVE_80_160
	/* � �������� ������������� ������� �� DDS ������������ ����������� 10.7 MHz */
	// ++ hybrid synthesizer parameters for 80 MHz..160 MHz
	#define LO1MODE_HYBRID	1
	#define SYNTHLOWFREQ 	80000000L	/* Lowest frequency of 1-st LO */
	#define SYNTHTOPFREQ 	160000000L	/* Highest frequency of 1-st LO */
	#define FREQ_DENOM 1000000L
	#define FREQ_SCALE 1189207L
	#define HYBRID_NVFOS 4		
	#define SYNTH_R1 15	/* divider afrer DSS1 unit */

	#define REFERENCE_FREQ	64000000ul
	#define SYNTH_CFCENTER	10760000L	/* ����� ������ ����������� ������� ����� DDS */
	//#define REFERENCE_FREQ	32000000ul
	//#define SYNTH_CFCENTER	10760000L	/* ����� ������ ����������� ������� ����� DDS */
	#define LO2_PLL_N	0		/* LO2 = 0 MHz */

	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS2_CLK_MUL	1		/* ���������� � DDS2 */
	#define DDS3_CLK_MUL	1		/* ���������� � DDS3 */

#endif

#if HYBRID_DDS_ATTINY2313
	#define LO1MODE_HYBRID	1
	/* � �������� ������������� ������� �� DDS ������������ ����������� 455 kHz */
	#define SYNTH_CFCENTER	455000L	/* ����� ������ ����������� ������� ����� DDS */
	//#define SYNTH_R1		25	/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	#define SYNTH_R1 72
	#define LM7001_PLL1_RCODE	LM7001_RCODE_72
	#define FTW_RESOLUTION 32
	//#define REFERENCE_FREQ	18432000L	// 18.432 kHz - ��, ��� ��������� � �������
	#define REFERENCE_FREQ	20000000L	// 18.432 kHz - ��, ��� ��������� � �������
	#define DDS1_CLK_MUL	1		/* ���������� � DDS1 */
	#define DDS1_CLK_DIV	9		/* �������� ������� ������� ����� ������� � DDS1 (���������� ������ �� ���� ������� ��������� �������� ���������) */

#endif /* HYBRID_DDS_ATTINY2313 */

#endif /* RADIO_H_INCLUDED */



