/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */
#include "board.h"
#include "audio.h"
#include "spifuncs.h"
#include "formats.h"	// for debug prints
#include "display.h"	// for debug prints

#include "tlv320aic23.h"	// ��������� ���������� ��������� ������
#include "nau8822.h"
#include "wm8994.h"

#include <limits.h>
#include <string.h>
#include <math.h>
#include <assert.h>

//#define WITHLIMITEDAGCATTACK 1
#define DUALFILTERSPROCESSING 1	// ������� �� ��� ������ � ������� ������� - ������� � ���������������� ����
#define WITHDOUBLEFIRCOEFS 1

#if WITHUSEDUALWATCH && WITHDSPLOCALFIR
	#error Can not support WITHUSEDUALWATCH && WITHDSPLOCALFIR together
#endif /* WITHUSEDUALWATCH && WITHDSPLOCALFIR */

#if 0
	#ifdef __ARM_FP
		#warning Avaliable __ARM_FP
		#if __ARM_FP & 0x02
			#warning Avaliable __ARM_FP - half
		#endif
		#if __ARM_FP & 0x04
			#warning Avaliable __ARM_FP - single
		#endif
		#if __ARM_FP & 0x08
			#warning Avaliable __ARM_FP - double
		#endif
		#ifdef __ARM_FEATURE_FMA
			#warning Avaliable __ARM_FEATURE_FMA
		#endif
	#endif
	#ifdef __ARM_FP_FAST
		#warning __ARM_FP_FAST
	#endif
	#ifdef __ARM_FEATURE_UNALIGNED
		#warning __ARM_FEATURE_UNALIGNED
	#endif
	#ifdef __ARM_FEATURE_DSP
		#warning Avaliable__ARM_FEATURE_DSP
	#endif
	#ifdef __ARM_FP_FAST
		#warning Avaliable __ARM_FP_FAST
	#endif
	#ifdef __ARM_NEON
		#warning Avaliable __ARM_NEON
	#endif
	#ifdef __ARM_NEON_FP
		#warning Avaliable __ARM_NEON_FP
	#endif
	#ifdef FP_FAST_FMA
		#warning Avaliable FP_FAST_FMA
	#endif
	#ifdef FP_FAST_FMAF
		#warning Avaliable FP_FAST_FMAF
	#endif
	#ifdef __ARM_ACLE
		#warning Avaliable __ARM_ACLE
	#endif
#endif

#ifdef __ARM_ACLE
	#include <arm_acle.h>
#endif /* __ARM_ACLE */

#if __ARM_NEON
	#include <arm_neon.h>
#endif /* __ARM_NEON */

// ARMSAIRATE - sample rate IF ������ � ������
#define NSAITICKS(t_ms) ((uint_fast16_t) (((uint_fast32_t) (t_ms) * ARMSAIRATE + 500) / 1000))

///////////////////////////////////////
//

static uint_fast8_t		glob_trxpath = 0;			/* �����, � �������� ��������� ��� ����������� ������. ��� ���������� ������������ ������ 0 */
static uint_fast16_t 	glob_ifgain = BOARD_IFGAIN_MIN;
static uint_fast8_t 	glob_dspmodes [2] = { DSPCTL_MODE_IDLE, DSPCTL_MODE_IDLE, };

static uint_fast8_t		glob_agcrate [2] = { 20, 20 }; //10	// 10 �� ��������� �������� �� 1 �� ���������
static uint_fast8_t 	glob_agc_t1 [2] = { 95, 95 };
static uint_fast8_t 	glob_agc_t2 [2] = { 2, 2 };
static uint_fast8_t 	glob_agc_thung [2] = { 3, 3 };	// 0.3 S
static uint_fast8_t 	glob_agc_t4 [2] = { 120, 120 };

static int_fast16_t 	glob_aflowcutrx [2] = { 300, 300 } ;		// ������� ��������������� ����� ������ ����������� (� 10 �� ���������)
static int_fast16_t 	glob_afhighcutrx [2] = { 3400, 3400 };	// ������� ���������������� ����� ������ ����������� (� 100 �� ���������)

static int_fast16_t 	glob_aflowcuttx = 300 ;		// ������� ��������������� ����� ������ ����������� (� 10 �� ���������)
static int_fast16_t 	glob_afhighcuttx = 3400;	// ������� ���������������� ����� ������ ����������� (� 100 �� ���������)

static int_fast16_t		glob_fullbw6 [2] = { 1000, 1000 };		/* ������� ����� �������� �� � ��������� ������ */
static int_fast32_t		glob_lo6 [2] = { 0, 0 };
//static uint_fast8_t		glob_fltsofter [2] = { WITHFILTSOFTMIN, WITHFILTSOFTMIN }; /* WITHFILTSOFTMIN..WITHFILTSOFTMAX ��� ���������� ������������ ������ ������� �������� �������� �� ����� */

static uint_fast8_t 	glob_nfm_sql_lelel = 127;
static uint_fast8_t 	glob_nfm_sql_off = 0;
static uint_fast8_t 	glob_squelch;

static uint_fast8_t 	glob_swapiq = 0;	// �������� ������� I � Q ������ � ������ RTS96

// codec-related parameters
static uint_fast16_t 	glob_afgain;
static uint_fast8_t 	glob_afmute;	/* ��������� ���� � ��������� � ��������� */
static uint_fast8_t 	glob_lineinput;	/* ������������ line input ������ ��������� */
static uint_fast8_t 	glob_mikebust20db;	/* ��������� ��������� 20 �� �� ���������� */
static uint_fast8_t		glob_mikeagc = 1;	/* ��������� ����������� ��� ����� ����������� */
static uint_fast8_t		glob_mikeagcscale = 100;	/* �� ����� ����� (� ���������) �� ������ ��������� ��������� ��� ��������� */
static uint_fast8_t		glob_mikeagcgain = 40;	/* ������ �������� � ��� */
static uint_fast8_t		glob_mikehclip;			/* �������� ������������ ���������	*/
#if defined(CODEC1_TYPE)
static uint_fast8_t 	glob_mikeequal;	// ��������� ��������� ������� � ��������� (�������, ����������, ...)
static uint_fast8_t		glob_codec1_gains [HARDWARE_CODEC1_NPROCPARAMS]; // = { -2, -1, -3, +6, +9 };	// ��������� �����������
#endif /* defined(CODEC1_TYPE) */

static uint_fast16_t 	glob_lineamp = WITHLINEINGAINMAX;
static uint_fast16_t	glob_mik1level = WITHMIKEINGAINMAX;
static uint_fast8_t 	glob_txaudio = BOARD_TXAUDIO_MIKE;	// ��� SSB/AM/FM �������� � �������� ����������
static uint_fast8_t		glob_mainsubrxmode = BOARD_RXMAINSUB_A_A;	// �����/������, A - main RX, B - sub RX

static uint_fast16_t 	glob_notch_freq = 1000;	/* ������� NOTCH ������� */
static uint_fast16_t	glob_notch_width = 500;	/* ������ NOTCH ������� */
static uint_fast8_t 	glob_notch_on;		/* ��������� NOTCH ������� */

static uint_fast8_t 	glob_cwedgetime = 4;		/* CW Rise Time (in 1 ms discrete) */
static uint_fast8_t 	glob_sidetonelevel = 10;	/* ������� ������� ������������ � ��������� - 0%..100% */
static uint_fast8_t 	glob_subtonelevel = 0;	/* ������� ������� CTCSS � ��������� - 0%..100% */
static uint_fast8_t 	glob_amdepth = 30;		/* ������� ��������� � �� - 0..100% */
static uint_fast8_t		glob_dacscale = 100;	/* �� ����� ����� (� ���������) �� ������ ��������� ������������ ��� ����������� */

static uint_fast8_t 	glob_digigainmax = 96;
static uint_fast8_t		glob_gvad605 = UINT8_MAX;	/* ���������� �� AD605 (���������� ��������� ������ �� */

static int_fast16_t		glob_fsadcpower10 = 0;	// ��������, ��������������� full scale �� IF ADC � ��������� 0.1 �����

static uint_fast8_t		glob_modem_mode;		// ����������� ���������
static uint_fast32_t	glob_modem_speed100 = 3125;	// �������� �������� � ��������� 1/100 ���

static int_fast8_t		glob_afresponcerx;	// ��������� ������ ����� � ������ ��������� - �� Samplerate/2 ��� ���������� �� ������� ������� 
static int_fast8_t		glob_afresponcetx;	// ��������� ������ ����� � ������ ����������� - �� Samplerate/2 ��� ���������� �� ������� ������� 

static int_fast8_t		glob_swaprts;		// ���������� ������� ������ ������������������

#if WITHINTEGRATEDDSP

#define NPROF 2	/* ���������� �������� ���������� DSP ��������. */

// ����������� ��� ����� �� ����������� ��������������
#if WITHDEBUG && 0

	// stm32f746, no dualwatch:
	//	dtcount=0, dtmax=0, dtlast=0, dtcount2=41807716, dtmax2=1244, dtlast2=739, dtcount3=41806755, dtmax3=1446, dtlast3=916
	// R7S721xxx, Neon, dualwatch:
	//	dtcount=0, dtmax=0, dtlast=0, dtcount2=15890107, dtmax2=1119, dtlast2=590, dtcount3=31778668, dtmax3=1169, dtlast3=723
	// R7S721xxx, no Neon, dualwatch:
	// dtcount=0, dtmax=0, dtlast=0, dtcount2=184728, dtmax2=1461, dtlast2=911, dtcount3=367872, dtmax3=1169, dtlast3=713

	static volatile uint_fast32_t dtmax = 0, dtlast = 0, dtcount = 0;
	static volatile uint_fast32_t dtmax2 = 0, dtlast2 = 0, dtcount2 = 0;
	static volatile uint_fast32_t dtmax3 = 0, dtlast3 = 0, dtcount3 = 0;
	static uint_fast32_t perft = 0;
	static uint_fast32_t perft2 = 0;
	static uint_fast32_t perft3 = 0;

	static void debug_cleardtmax(void)
	{
		dtmax = 0;
		dtmax2 = 0;
		dtmax3 = 0;
	}

	#define BEGIN_STAMP() do { \
			perft = cpu_getdebugticks(); \
		} while (0);

	#define END_STAMP() do { \
			const uint_fast32_t t2 = cpu_getdebugticks(); \
			if (perft < t2) \
			{ \
				const uint_fast32_t vdt = t2 - perft; \
				dtlast = vdt; /* ������� �������� ������������ */ \
				if (vdt > dtmax) \
					dtmax = vdt; /* ������������ �������� ������������ */ \
				++ dtcount; \
			} \
		} while (0);

	#define BEGIN_STAMP2() do { \
			perft2 = cpu_getdebugticks(); \
		} while (0);

	#define END_STAMP2() do { \
			const uint_fast32_t t2 = cpu_getdebugticks(); \
			if (perft2 < t2) \
			{ \
				const uint_fast32_t vdt = t2 - perft2; \
				dtlast2 = vdt; /* ������� �������� ������������ */ \
				if (vdt > dtmax2) \
					dtmax2 = vdt; /* ������������ �������� ������������ */ \
				++ dtcount2; \
			} \
		} while (0);

	#define BEGIN_STAMP3() do { \
			perft3 = cpu_getdebugticks(); \
		} while (0);

	#define END_STAMP3() do { \
			const uint_fast32_t t2 = cpu_getdebugticks(); \
			if (perft3 < t2) \
			{ \
				const uint_fast32_t vdt = t2 - perft3; \
				dtlast3 = vdt; /* ������� �������� ������������ */ \
				if (vdt > dtmax3) \
					dtmax3 = vdt; /* ������������ �������� ������������ */ \
				++ dtcount3; \
			} \
		} while (0);


	//static uint32_t dd [4];
	/* DSP speed test */
	void dsp_speed_diagnostics(void)
	{
		//debug_printf_P(PSTR("data=%08lX,%08lX,%08lX,%08lX\n"), dd [0], dd [1], dd [2], dd [3]);
		debug_printf_P(PSTR("dtcount=%" PRIuFAST32 ", dtmax=%" PRIuFAST32 ", dtlast=%" PRIuFAST32 ", "), dtcount, dtmax, dtlast);
		debug_printf_P(PSTR("dtcount2=%" PRIuFAST32 ", dtmax2=%" PRIuFAST32 ", dtlast2=%" PRIuFAST32 ", "), dtcount2, dtmax2, dtlast2);
		debug_printf_P(PSTR("dtcount3=%" PRIuFAST32 ", dtmax3=%" PRIuFAST32 ", dtlast3=%" PRIuFAST32 "\n"), dtcount3, dtmax3, dtlast3);
	}

#else /* WITHDEBUG */

	#define BEGIN_STAMP() do { \
		} while (0);

	#define END_STAMP() do { \
		} while (0);

	#define BEGIN_STAMP2() do { \
		} while (0);

	#define END_STAMP2() do { \
		} while (0);

	#define BEGIN_STAMP3() do { \
		} while (0);

	#define END_STAMP3() do { \
		} while (0);

	static void debug_cleardtmax(void)
	{
	}

	void dsp_speed_diagnostics(void)
	{
	}

#endif /* WITHDEBUG */



// ����������� ��������� ���������� ���������� ������� - �������� �������� Ntap.
// ����� ����, ��� ������� ���������� � �������������� ��������� �������������, ��������� ��������� 2 �������� Ntap

#define NtapValidate(n)	((unsigned) (n) / 8 * 8 + 1)
#define NtapCoeffs(n)	((unsigned) (n) / 2 + 1)


#if CPUSTYLE_R7S721
	#define FFTSizeFiltersM 10
	#define FFTSizeSpectrumM 10
#else
	#define FFTSizeFiltersM 10
	#define FFTSizeSpectrumM 10
#endif


#if WITHDSPEXTFIR || WITHDSPEXTDDC


	// ��������� �������� � ������ ������������� FPGA � �������� �� ������������ �������
	#define Ntap_trxi_IQ		1535	// ������ � FPGA
	#define HARDWARE_COEFWIDTH	23		// ����������� �������������. format is S0.22

	// ������ ��� ������������ ������� �������� � ����������� � FPGA (�������������).
	// ��������� ��� �������� � FPGA
	static int_fast32_t FIRCoef_trxi_IQ [NtapCoeffs(Ntap_trxi_IQ)];
	#if WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08)
		static double FIRCwndL_trxi_IQ [NtapCoeffs(Ntap_trxi_IQ)];			// �������������� �������� ������� ����
	#else
		static FLOAT_t FIRCwnd_trxi_IQ [NtapCoeffs(Ntap_trxi_IQ)];			// �������������� �������� ������� ����
	#endif

#endif /* WITHDSPEXTFIR || WITHDSPEXTDDC */

#if WITHDSPEXTFIR


	#if CPUSTYLE_R7S721 && ! WITHUSEDUALWATCH
		// ��� WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(241)
		#define Ntap_tx_MIKE	NtapValidate(481)
	#elif CPUSTYLE_R7S721
		// ���� ����� WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(241)
		#define Ntap_tx_MIKE	NtapValidate(481)
	#elif (defined (STM32F767xx) || defined (STM32F769xx)) && ! WITHUSEDUALWATCH
		// ��� WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(221)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif (defined (STM32F767xx) || defined (STM32F769xx))
		// ���� ����� WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(129)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif CPUSTYLE_STM32H7XX && ! WITHUSEDUALWATCH
		// ��� WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(221)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif CPUSTYLE_STM32H7XX
		// ���� ����� WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(129)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif CPUSTYLE_STM32F7XX && ! WITHUSEDUALWATCH
		// ��� WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(221)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif CPUSTYLE_STM32F7XX
		// ���� ����� WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(129)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif CPUSTYLE_STM32F4XX && ! WITHUSEDUALWATCH
		// ��� WITHUSEDUALWATCH (������)
		#define Ntap_rx_AUDIO	NtapValidate(129)
		#define Ntap_tx_MIKE	NtapValidate(129)
	#else
		#error Not suitable CPUSTYLE_xxx and WITHUSEDUALWATCH combination
	#endif

#endif /* WITHDSPEXTFIR */

#if WITHDSPLOCALFIR	
	/* ���������� ��������� �������������� ����������� */
	#if CPUSTYLE_R7S721
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_rx_AUDIO	NtapValidate(105)	// single samples, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
	#elif CPUSTYLE_STM32F7XX
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_rx_AUDIO	NtapValidate(105)	// single samples, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
	#else
		#define Ntap_rx_SSB_IQ	NtapValidate(181)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(181)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_rx_AUDIO	NtapValidate(105)	// single samples, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
	#endif

	// ������ ��� ������������ ������� �������� (floating point).
	static FLOAT_t FIRCoef_rx_SSB_IQ [NPROF] [NtapCoeffs(Ntap_rx_SSB_IQ)] = { { 0 }, { 0 } };
	static FLOAT_t FIRCwnd_rx_SSB_IQ [NtapCoeffs(Ntap_rx_SSB_IQ)];			// �������������� �������� ������� ����

	// ������ ��� ������������ ������� ����������� (floating point)
	static FLOAT_t FIRCoef_tx_SSB_IQ [NPROF] [NtapCoeffs(Ntap_tx_SSB_IQ)] = { { 0 }, { 0 } };
	static FLOAT_t FIRCwnd_tx_SSB_IQ [NtapCoeffs(Ntap_tx_SSB_IQ)];			// �������������� �������� ������� ����

#endif /* WITHDSPLOCALFIR */

// ������ ��� �����������
// �������������� ��� ��������������
// ������ ���������� ��� ���� ����������
static FLOAT_t FIRCoef_rx_AUDIO [NPROF] [2 /* ��� ����������� ����� ������ */] [NtapCoeffs(Ntap_rx_AUDIO)] = { { { 0 }, { 0 } }, { { 0 }, { 0 } } };
static FLOAT_t FIRCwnd_rx_AUDIO [NtapCoeffs(Ntap_rx_AUDIO)];			// �������������� �������� ������� ����

// ������ ��� ����������� (floating point)
// �������������� ��� ��������������
static FLOAT_t FIRCoef_tx_MIKE [NPROF] [NtapCoeffs(Ntap_tx_MIKE)] = { { 0 }, { 0 } };
static FLOAT_t FIRCwnd_tx_MIKE [NtapCoeffs(Ntap_tx_MIKE)];			// �������������� �������� ������� ����

/* ��������� ������������ ������ � ���������� ����������� ������������ �������. */
/* ��������� ����������� ����������� ������ IF � AF �������. */
#if (WITHIFDACWIDTH > WITHAFADCWIDTH)
	#define TXINSCALE		(1 << (WITHIFDACWIDTH - WITHAFADCWIDTH))		// ������������� ������� � ����������� ��� ��������� ������� � ��������� ���
	#define TXOUTDENOM		1
#elif (WITHIFDACWIDTH == WITHAFADCWIDTH)
	#define TXINSCALE		1				// ������������� ������� � ����������� ��� ��������� ������� � ��������� ���
	#define TXOUTDENOM		1
#else
	#error Strange WITHIFDACWIDTH & WITHAFADCWIDTH relations
#endif

/* ��������� ����������� ����������� ������ IF � AF �������. */
#if (WITHIFADCWIDTH > WITHAFDACWIDTH)
	#define RXINSCALE		1
	#define RXOUTDENOM		(1 << (WITHIFADCWIDTH - WITHAFDACWIDTH))				// ������������� ������� � ����������� ��� ��������� ������� � ��������� ���
#elif (WITHIFADCWIDTH == WITHAFDACWIDTH)
	#define RXINSCALE		1
	#define RXOUTDENOM		1				// ������������� ������� � ����������� ��� ��������� ������� � ��������� ���
#else
	#error Strange WITHIFADCWIDTH & WITHAFDACWIDTH relations
#endif

static FLOAT_t txlevelfence = INT_MAX;
static FLOAT_t txlevelfenceHALF = INT_MAX / 2;

static int_fast32_t txlevelfenceSSB_INTEGER = INT_MAX - 1;
static FLOAT_t txlevelfenceSSB = INT_MAX / 2;

static FLOAT_t txlevelfenceNFM = INT_MAX / 2;
static FLOAT_t txlevelfenceBPSK = INT_MAX / 2;
static FLOAT_t txlevelfenceCW = INT_MAX / 2;

static FLOAT_t rxlevelfence = INT_MAX;

static FLOAT_t mikefence = INT_MAX;
static FLOAT_t phonefence = INT_MAX;	// ����������� ������������ �� �������� �������
static FLOAT_t rxoutdenom = 1 / (FLOAT_t) RXOUTDENOM;

static volatile FLOAT_t nbfence;
static volatile FLOAT_t nfmoutscale;	// ��������������� (INT32_MAX + 1) � phonefence

static uint_fast8_t gwprof = 0;	// work profile - ������ ���������������� ����������, ����������� ��� ������ */

static uint_fast8_t globDSPMode [NPROF] [2] = { { DSPCTL_MODE_IDLE, DSPCTL_MODE_IDLE }, { DSPCTL_MODE_IDLE, DSPCTL_MODE_IDLE } };

/* ��������� �� ���������� */
static volatile FLOAT_t amshapesignalHALF;
static volatile FLOAT_t amcarrierHALF;
static volatile FLOAT_t scaleDAC = 1;

static FLOAT_t shapeSidetoneStep(void);		// 0..1
static FLOAT_t shapeCWEnvelopStep(void);	// 0..1
static uint_fast8_t getTxShapeNotComplete(void);

static uint_fast8_t getRxGate(void);	/* ���������� ������ ������ � ������ ����� */

//#include "sinetable.h"
//#include "sinetable_15.h"
#include "sinetable_14.h"
//#include "sinetable_13.h"
//#include "sinetable_12.h"
typedef uint32_t ncoftw_t;
typedef int32_t ncoftwi_t;
#define NCOFTWBITS 32	// ���������� ����� � ncoftw_t
#define ASH (NCOFTWBITS - TABLELOG2)	// 22 = 32 - log2(number of items in sintable)
#define FTW2ANGLEI(ftw)	((uint32_t) (ftw) >> ASH)
#define FTW2ANGLEQ(ftw)	((uint32_t) ((ftw) + 0x40000000L) >> ASH)	// ������� ��������� �� �������� �������
#define FTWROUND(ftw) ((uint32_t) (ftw))
#define FTWAF001(freq) (((int_fast64_t) (freq) << NCOFTWBITS) / ARMI2SRATE100)
#define FTWAF(freq) (((int_fast64_t) (freq) << NCOFTWBITS) / ARMI2SRATE)
static FLOAT_t omega2ftw_k1; // = POWF(2, NCOFTWBITS);
#define OMEGA2FTWI(angle) ((ncoftwi_t) ((FLOAT_t) (angle) * omega2ftw_k1 / (FLOAT_t) M_TWOPI))	// angle in radians -pi..+pi to signed version of ftw_t

static RAMFUNC FLOAT_t peekvalf(uint32_t a)
{
	const ncoftw_t mask = (1UL << (TABLELOG2 - 2)) - 1;
	const ncoftw_t quoter = 1UL << (TABLELOG2 - 2);
	const ncoftw_t index = a & mask;
	switch (a >> (TABLELOG2 - 2))	 // �������� ����� ��������� �� ������� � �������
	{
	case 0: return sintable4f_fs [index];
	case 1: return sintable4f_fs [quoter - index];
	case 2: return - sintable4f_fs [index];
	case 3: return - sintable4f_fs [quoter - index];
	}
	return 0;
}

#if 1//WITHLOOPBACKTEST || WITHSUSBSPKONLY || WITHUSBHEADSET

static RAMFUNC int peekvali16(uint32_t a)
{
	const ncoftw_t mask = (1UL << (TABLELOG2 - 2)) - 1;
	const ncoftw_t quoter = 1UL << (TABLELOG2 - 2);
	const ncoftw_t index = a & mask;
	switch (a >> (TABLELOG2 - 2))	 // �������� ����� ��������� �� ������� � �������
	{
	case 0: return sintable4i32_fs [index] >> 16;
	case 1: return sintable4i32_fs [quoter - index] >> 16;
	case 2: return - sintable4i32_fs [index] >> 16;
	case 3: return - sintable4i32_fs [quoter - index] >> 16;
	}
	return 0;
}

static RAMFUNC int peekvali24(uint32_t a)
{
	const ncoftw_t mask = (1UL << (TABLELOG2 - 2)) - 1;
	const ncoftw_t quoter = 1UL << (TABLELOG2 - 2);
	const ncoftw_t index = a & mask;
	switch (a >> (TABLELOG2 - 2))	 // �������� ����� ��������� �� ������� � �������
	{
	case 0: return sintable4i32_fs [index] >> 8;
	case 1: return sintable4i32_fs [quoter - index] >> 8;
	case 2: return - sintable4i32_fs [index] >> 8;
	case 3: return - sintable4i32_fs [quoter - index] >> 8;
	}
	return 0;
}

static RAMFUNC int32_t peekvali32(uint32_t a)
{
	const ncoftw_t mask = (1UL << (TABLELOG2 - 2)) - 1;
	const ncoftw_t quoter = 1UL << (TABLELOG2 - 2);
	const ncoftw_t index = a & mask;
	switch (a >> (TABLELOG2 - 2))	 // �������� ����� ��������� �� ������� � �������
	{
	case 0: return sintable4i32_fs [index];
	case 1: return sintable4i32_fs [quoter - index];
	case 2: return - sintable4i32_fs [index];
	case 3: return - sintable4i32_fs [quoter - index];
	}
	return 0;
}

#endif /* WITHLOOPBACKTEST */

//////////////////////////////////////////
#if 1//WITHLOOPBACKTEST || WITHSUSBSPKONLY || WITHUSBHEADSET

static ncoftw_t anglestep_lout = FTWAF(700), anglestep_rout = FTWAF(500);
static ncoftw_t angle_lout, angle_rout;

static ncoftw_t anglestep_lout2 = FTWAF(5600), anglestep_rout2 = FTWAF(6300);
static ncoftw_t angle_lout2, angle_rout2;

int get_rout16(void)
{
	// ������������ �������� ��� ROUT
	const int v = peekvali16(FTW2ANGLEI(angle_rout));
	angle_rout = FTWROUND(angle_rout + anglestep_rout);
	return v;
}

int get_lout16(void)
{
	// ������������ �������� ��� LOUT
	const int v = peekvali16(FTW2ANGLEI(angle_lout));
	angle_lout = FTWROUND(angle_lout + anglestep_lout);
	return v;
}

static int get_rout24(void)
{
	// ������������ �������� ��� ROUT
	const int v = peekvali24(FTW2ANGLEI(angle_rout2));
	angle_rout2 = FTWROUND(angle_rout2 + anglestep_rout2);
	return v;
}

static int get_lout24(void)
{
	// ������������ �������� ��� LOUT
	const int v = peekvali24(FTW2ANGLEI(angle_lout2));
	angle_lout2 = FTWROUND(angle_lout2 + anglestep_lout2);
	return v;
}

#endif /* WITHLOOPBACKTEST */

//////////////////////////////////////////
static ncoftw_t anglestep_sidetone = 0; //FTWAF(700);
static ncoftw_t angle_sidetone = 0;

static RAMFUNC FLOAT_t get_float_sidetone(void)
{
	const FLOAT_t v = peekvalf(FTW2ANGLEI(angle_sidetone));
	angle_sidetone = FTWROUND(angle_sidetone + anglestep_sidetone);
	return v;
}

//////////////////////////////////////////

static ncoftw_t anglestep_subtone = 0; //FTWAF(700);
static ncoftw_t angle_subtone = 0;

static RAMFUNC FLOAT_t get_float_subtone(void)
{
	const FLOAT_t v = peekvalf(FTW2ANGLEI(angle_subtone));
	angle_subtone = FTWROUND(angle_subtone + anglestep_subtone);
	return v;
}

//////////////////////////////////////////

static ncoftw_t anglestep_toneout = FTWAF(700);
static ncoftw_t angle_toneout;

static RAMFUNC FLOAT_t get_singletonefloat(void)
{
	// ������������ �������� ��� LOUT
	const FLOAT_t v = peekvalf(FTW2ANGLEI(angle_toneout));
	angle_toneout = FTWROUND(angle_toneout + anglestep_toneout);
	return v;
}

// ������������� ��������� ��� ���������
// ��������� ������

// American (AT&T)
// dial tone 350 and 440 Hz
//static ncoftw_t anglestep_af1 = FTWAF(440);
//static ncoftw_t anglestep_af2 = FTWAF(350);

// Dual tone signal generator for SSB TX IMD3 tests - 1200 Hz spacing
// Document: Procedure Manual 2011 with page breaks.pdf, page 16
static ncoftw_t anglestep_af1 = FTWAF(700);
static ncoftw_t anglestep_af2 = FTWAF(1900);

static ncoftw_t angle_af1;
static ncoftw_t angle_af2;

// ������������� ��������� ��� ���������
static RAMFUNC FLOAT_t get_dualtonefloat(void)
{
	// ������������ �������� �������
	const FLOAT_t v1 = peekvalf(FTW2ANGLEI(angle_af1));
	const FLOAT_t v2 = peekvalf(FTW2ANGLEI(angle_af2));
	angle_af1 = FTWROUND(angle_af1 + anglestep_af1);
	angle_af2 = FTWROUND(angle_af2 + anglestep_af2);
	return (v1 + v2) / 2;
}


//////////////////////////////////////////
// ��������� ������������ �������� ��� ������ �������
/*
static ncoftw_t anglestep_aflosim = FTWAF(700);
static ncoftw_t angle_aflosim = 0;
static INT32P_t get_int32_aflosim(void)
{
	INT32P_t v;
	v.IV = peekvali32(FTW2ANGLEI(angle_aflosim));
	v.QV = peekvali32(FTW2ANGLEQ(angle_aflosim));
	angle_aflosim = FTWROUND(angle_aflosim + anglestep_aflosim);
	return v;
}
*/

static ncoftw_t anglestep_aflo [NTRX] = { 0, };
static ncoftw_t angle_aflo [NTRX] = { 0, };
const ncoftw_t gnfmdeviationftw = FTWAF(2500);	// 2.5 kHz (-2.5..+2.5) deviation

// ���������� �������
static void nco_setlo_ftw(ncoftw_t ftw, uint_fast8_t pathi)
{
	if (ftw == 0)
	{
		/* ��� ����������� ������������ ��������� ����� ������������ ������������ ������������ � FPA */
		//angle_aflo [pathi] = (ncoftw_t) 1 << (NCOFTWBITS - 2);	// 1/2 Pi
		/* ��� ����������� 0.7 �� ������������ ��������� ����� ������������ ������������ ������������ � FPA */
		angle_aflo [pathi] = 0;	// 0 Pi
	}
	anglestep_aflo [pathi] = ftw;
}

// ��������� ������������ �������� ��� ������ ������� �� ��������� (� ������)
// Returned is a full scale value
static RAMFUNC FLOAT32P_t get_float_aflo_delta(long int deltaftw, uint_fast8_t pathi)
{
	FLOAT32P_t v;
	v.IV = peekvalf(FTW2ANGLEI(angle_aflo [pathi]));
	v.QV = peekvalf(FTW2ANGLEQ(angle_aflo [pathi]));
	angle_aflo [pathi] = FTWROUND(angle_aflo [pathi] + anglestep_aflo [pathi] + deltaftw);
	return v;
}

#if 0
// ��������� ������������ �������� ��� ������ ������� �� ��������� (� ������)
// Returned is a full scale value
static RAMFUNC INT32P_t get_int32_aflo_delta(long int deltaftw, uint_fast8_t pathi)
{
	INT32P_t v;
	v.IV = peekvali32(FTW2ANGLEI(angle_aflo [pathi]));
	v.QV = peekvali32(FTW2ANGLEQ(angle_aflo [pathi]));
	angle_aflo [pathi] = FTWROUND(angle_aflo [pathi] + anglestep_aflo [pathi] + deltaftw);
	return v;
}
#endif

//////////////////////////////////////////

#if ! WITHDSPEXTDDC

static const ncoftw_t anglestep_iflo = (1U << (NCOFTWBITS - 2));	// ���������� ������� � 1/4 sample rate
static ncoftw_t angle_iflo = 0;


// ��������� ������������ �������� ��� ������ �������
static FLOAT32P_t get_float4_iflo(void)
{
	static const FLOAT_t sintable4f [4] = { + 0, + 1, + 0, - 1 };
	FLOAT32P_t v;
	v.IV = (sintable4f [FTW2ANGLEI(angle_iflo) >> (TABLELOG2 - 2)]);
	v.QV = (sintable4f [FTW2ANGLEQ(angle_iflo) >> (TABLELOG2 - 2)]);
	angle_iflo = FTWROUND(angle_iflo + anglestep_iflo);
	return v;
}

#endif /* ! WITHDSPEXTDDC */

//////////////////////////////////////////

#if 1

// ������� ��� ������ ����������� ����������

#define FP_OPER 1		/* FPU op code error			*/
#define FP_ZDIV 2		/* FPU divide by zero			*/
#define FP_FTOI 3		/* FPU float to integer conv error	*/
#define FP_OFLO 4		/* FPU overflow				*/
#define FP_UFLO 5		/* FPU underflow			*/
#define FP_UDEF 6		/* FPU undefined variable (-0)		*/
#define FP_BIGI 7		/* Atof input too large			*/
#define FP_BADC 8		/* Bad character in atof input string	*/
#define FP_NESQ 9		/* Square root of negative number	*/
#define FP_LEXP	10		/* Exp argument too large		*/
#define FP_SEXP 11		/* Exp argument too small		*/
#define FP_NLOG 12		/* Log argument zero or negative	*/
#define FP_TANE 13		/* Argument of tan too large		*/
#define FP_TRIG 14		/* Argument of sin/cos too large	*/
#define FP_ATAN 15		/* Atan2 arguments both zero		*/
#define FP_COTE 16		/* Argument of cotan too small		*/
#define FP_ARSC 17		/* Bad argument for asin/acos		*/
#define FP_SINH 18		/* Argument of sinh too large		*/
#define FP_COSH 19		/* Argument of cosh too large		*/
#define FP_POWN 20		/* Negative argument in pow		*/
#define FP_POWO 21		/* Result of pow overflows		*/
#define FP_POWU 22		/* Result of pow underflows		*/

#define LOG_HUGE ((FLOAT_t) 0.880296919311130543E+02)	/* log of HUGE		*/
#define LOG_TINY ((FLOAT_t) -0.887228391116729997E+02)	/* log of TINY		*/

#define LOGBE2 ((FLOAT_t) 0.69314718055994530942)	/* log of 2 to base e		*/
#define LOGB2E ((FLOAT_t) 1.44269504088896341)	/* log of e to base 2		*/

#define HUGE ((FLOAT_t) 0.170141183460469230E+39)   /* largest no = 2**+127     */


static void CMEMSGF(int erno, FLOAT_t * datap)
{
	* datap = 0;
}

FLOAT_t local_log(FLOAT_t x)
{
	static const FLOAT_t C1 = ((FLOAT_t) 0.693359375);		/* C1 + C2 should represent log 2 to	*/
	static const FLOAT_t C2 = ((FLOAT_t) -2.12194440054690583e-4);	/* more than machine precision	*/

	static const FLOAT_t a0 = -0.641249434237455811e+2;
	static const FLOAT_t a1 =  0.163839435630215342e+2;
	static const FLOAT_t a2 = -0.789561128874912573e+0;
	static const FLOAT_t b0 = -0.769499321084948798e+3;
	static const FLOAT_t b1 =  0.312032220919245328e+3;
	static const FLOAT_t b2 = -0.356679777390346462e+2;

    FLOAT_t a, b, f, r, w, z;
    int n;

    if (x <= (FLOAT_t)0.0) 
	{
		CMEMSGF(FP_NLOG, &x);
		if (x == (FLOAT_t)0.0)
			return(-HUGE);
		else
			x = -x;
	}

    f = FREXPF(x, &n);
    if (f > M_SQRT1_2) 
	{
		a = (f - (FLOAT_t)0.5) - (FLOAT_t)0.5;
		b = f * (FLOAT_t)0.5 + (FLOAT_t)0.5;
    }
    else 
	{
		n--;
		a = f - (FLOAT_t)0.5;
		b = a * (FLOAT_t)0.5 + (FLOAT_t)0.5;
    }
    z = a / b;
    w = z * z;
    a = (a2 * w + a1) * w + a0;
    b = ((w + b2) * w + b1) * w + b0;
    r = z + z * w * a / b;
    z = n;
    return ((z * C2 + r) + z * C1);
}

// from ftp://ftp.update.uu.se/pub/pdp11/rt/cmath/pow.c

#define MAXEXP 2031		/* (MAX_EXP * 16) - 1			*/
#define MINEXP (-2047)		/* (MIN_EXP * 16) - 1			*/

FLOAT_t local_pow(FLOAT_t x, FLOAT_t y)
{

	static const FLOAT_t a1[] = {
		1.0,
		0.95760328069857365,
		0.91700404320467123,
		0.87812608018664974,
		0.84089641525371454,
		0.80524516597462716,
		0.77110541270397041,
		0.73841307296974966,
		0.70710678118654752,
		0.67712777346844637,
		0.64841977732550483,
		0.62092890603674203,
		0.59460355750136054,
		0.56939431737834583,
		0.54525386633262883,
		0.52213689121370692,
		0.50000000000000000
	};
	static const FLOAT_t a2[] = {
		 0.24114209503420288E-17,
		 0.92291566937243079E-18,
		-0.15241915231122319E-17,
		-0.35421849765286817E-17,
		-0.31286215245415074E-17,
		-0.44654376565694490E-17,
		 0.29306999570789681E-17,
		 0.11260851040933474E-17
	};

	static const FLOAT_t p1 = 0.833333333333332114e-1;
	static const FLOAT_t p2 = 0.125000000005037992e-1;
	static const FLOAT_t p3 = 0.223214212859242590e-2;
	static const FLOAT_t p4 = 0.434457756721631196e-3;
	static const FLOAT_t q1 = 0.693147180559945296e0;
	static const FLOAT_t q2 = 0.240226506959095371e0;
	static const FLOAT_t q3 = 0.555041086640855953e-1;
	static const FLOAT_t q4 = 0.961812905951724170e-2;
	static const FLOAT_t q5 = 0.133335413135857847e-2;
	static const FLOAT_t q6 = 0.154002904409897646e-3;
	static const FLOAT_t q7 = 0.149288526805956082e-4;
	static const FLOAT_t k = 0.442695040888963407;

    FLOAT_t g, r, u1, u2, v, w, w1, w2, y1, y2, z;
    int iw1, m, p;

    if (y == 0)
		return 1;
    if (x <= 0) 
	{
		if (x == 0) 
		{
			if (y > 0)
				return(x);
			CMEMSGF(FP_POWO, &y);
			return(HUGE);
		}
		else 
		{
			CMEMSGF(FP_POWN, &x);
			x = -x;
		}
    }
    g = FREXPF(x, &m);
    p = 0;
    if (g <= a1[8])
	p = 8;
    if (g <= a1[p + 4])
	p += 4;
    if (g <= a1[p + 2])
	p += 2;
    p++;
    z = ((g - a1[p]) - a2[p / 2]) / (g + a1[p]);
    z += z;
    v = z * z;
    r = (((p4 * v + p3) * v + p2) * v + p1) * v * z;
    r += k * r;
    u2 = (r + z * k) + z;
    u1 = (FLOAT_t)0.0625 * (FLOAT_t)(16 * m - p);
    y1 = (FLOAT_t)0.0625 * (FLOAT_t)((int)(16 * y));
    y2 = y - y1;
    w = u2 * y + u1 * y2;
    w1 = (FLOAT_t)0.0625 * (FLOAT_t)((int)(16 * w));
    w2 = w - w1;
    w = w1 + u1 * y1;
    w1 = (FLOAT_t)0.0625 * (FLOAT_t)((int)(16 * w));
    w2 += (w - w1);
    w = (FLOAT_t)0.0625 * (FLOAT_t)((int)(16 * w2));
    iw1 = 16 * (w1 + w);
    w2 -= w;
    while (w2 > 0) 
	{
		iw1++;
		w2 -= (FLOAT_t) 0.0625;
    }
    if (iw1 > MAXEXP) 
	{
		CMEMSGF(FP_POWO, &y);
		return(HUGE);
    }
    if (iw1 < MINEXP) 
	{
		CMEMSGF(FP_POWU, &y);
		return(0);
    }
    m = iw1 / 16;
    if (iw1 >= 0)
	m++;	
    p = 16 * m - iw1;
    z = ((((((q7*w2 + q6)*w2 + q5)*w2 + q4)*w2 + q3)*w2 + q2)*w2 + q1)*w2;
    z = a1[p] + a1[p] * z;
    return LDEXPF(z, m);

}

// from: ftp://ftp.update.uu.se/pub/pdp11/rt/cmath/exp.c


FLOAT_t local_exp(FLOAT_t x)
{
	static const FLOAT_t EPS2 = ((FLOAT_t) 6.93889401e-18);	/* exp(eps) = 1.0 to m/c precision	*/
	static const FLOAT_t C1 = ((FLOAT_t) 0.693359375);		/* C1 + C2 should represent log 2 to	*/

	static const FLOAT_t C2 = ((FLOAT_t) -2.12194440054690583e-4);	/* more than machine precision	*/
	static const FLOAT_t p0 = 0.249999999999999993e0;
	static const FLOAT_t p1 = 0.694360001511792852e-2;
	static const FLOAT_t p2 = 0.165203300268279130e-4;
	static const FLOAT_t q0 = 0.5;
	static const FLOAT_t q1 = 0.555538666969001188e-1;
	static const FLOAT_t q2 = 0.495862884905441294e-3;

    FLOAT_t g, gp, q, xn, z;
    int n;

/* check for exponent too large, if so print argument and return large no */
    if (x >= LOG_HUGE){
		CMEMSGF(FP_LEXP, & x);
		return(HUGE);
    }
/* check for exponent underflow, if so call error system (set at warning
   level) and return zero */
    if (x < LOG_TINY){
		CMEMSGF(FP_SEXP, & x);
		return (0);
    }
    if (x < EPS2 && x > - EPS2)
		return 1;
    z = x * LOGB2E;
    if (z < 0)
		n = z - (FLOAT_t)0.5;
    else
		n = z + (FLOAT_t)0.5;
    xn = n;
    g = (x - xn * C1) - xn * C2;
    z = g * g;
    gp = ((p2 * z + p1) * z + p0) * g;
    q = (q2 * z + q1) * z + q0;
    return(LDEXPF((FLOAT_t) 0.5 + gp / (q - gp), ++n));
}

#endif

//////////////////////////////////////////

// ������������� ��������� ���������� ���������� � "�����" � ���������.
/*
static FLOAT_t ratio2db(FLOAT_t ratio)
{
	return LOG10F(ratio) * 20;
}
*/
// ������������� ��������� ���������� � ��������� � "�����" ��������� ����������.

static FLOAT_t db2ratio(FLOAT_t valueDBb)
{
	return POWF(10, valueDBb / 20);
}

static struct Complex Sig [FFTSizeFilters];

/* ��������� �������� �������� ��� */
static FLOAT_t getmaxresponce(void)
{
	FLOAT_t r = (FLOAT_t) 1 / 16384;
	int i;
	for (i = 0; i < FFTSizeFilters / 2; ++ i)
	{
		r = FMAXF(r, SQRTF(Sig [i].real * Sig [i].real + Sig [i].imag * Sig [i].imag));
	}
	return r;
}
//====================================================
//  calculate impulse response of FIR filter
//====================================================

// ��������� ��� �� ������������� �������������� FIR

static void imp_response(const FLOAT_t *dCoeff, int iCoefNum) 
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	int i;

	//---------------------------
	// copy coefficients to Sig
	//---------------------------
	Sig [iHalfLen].real = dCoeff [iHalfLen];
	Sig [iHalfLen].imag = 0;
	for (i = 1; i <= iHalfLen; ++ i) 
	{
		const FLOAT_t k = dCoeff [iHalfLen - i];
		Sig [iHalfLen - i].real = k;
		Sig [iHalfLen + i].real = k;
		Sig [iHalfLen - i].imag = 0;
		Sig [iHalfLen + i].imag = 0;
	} 	

	//---------------------------
	// append zeros
	//---------------------------
	for (i = iCoefNum; i < FFTSizeFilters; ++ i) {
		Sig [i].real = 0;
		Sig [i].imag = 0;
	} 	

	//---------------------------
	// Do FFT
	//---------------------------

	FFT(Sig, FFTSizeFilters, FFTSizeFiltersM); 
}

//====================================================
// construct FIR coefficients from frequency response
//====================================================
static void reconstruct(void)
{

	// ����� ��������� Sig - ������ � �������� FFTSizeFilters / 2
	// �������������� �� responce � ����� ������������� ��� ������������� FIR

#if 1
#else
	int i;
	// ����� ��������� Sig - ������ � �������� FFTSizeFilters / 2
	// +++ black magic
	for (i = 0; i < FFTSizeFilters / 2; i++)
	{
		Sig [FFTSizeFilters - 1 - i].imag = - Sig [i + 1].imag;
		Sig [FFTSizeFilters - 1 - i].real = Sig [i + 1].real;
	}
	Sig [FFTSizeFilters / 2].imag = Sig [FFTSizeFilters / 2 + 1].imag;
	Sig [FFTSizeFilters / 2].real = Sig [FFTSizeFilters / 2 + 1].real;
	// --- black magic
#endif
	IFFT(Sig, FFTSizeFilters, FFTSizeFiltersM); 

}

static void sigtocoeffs(FLOAT_t *dCoeff, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int i;
	//---------------------------
	// Magnitude in dB
	//---------------------------
	for (i = 0; i < j; ++ i) {
		dCoeff [i] = Sig [i].real;
	}

}

static void scalecoeffs(FLOAT_t *dCoeff, int iCoefNum, FLOAT_t scale)
{
	const int j = NtapCoeffs(iCoefNum);
	int i;
	//---------------------------
	// Magnitude in dB
	//---------------------------
	for (i = 0; i < j; ++ i) {
		dCoeff [i] *= scale;
	}

}


static void fir_design_applaywindow(FLOAT_t *dCoeff, const FLOAT_t *dWindow, int iCoefNum);
static void fir_design_applaywindowL(double *dCoeff, const double *dWindow, int iCoefNum);

// slope: ��������� ������ ����� - �� Samplerate/2 ��� ���������� �� ������� ������� 
// scale: ����� ������� ��������� ���
static void correctspectrumcomplex(int_fast8_t targetdb)
{
#if 1
	const FLOAT_t slope = db2ratio(targetdb);
	// ����� ��������� Sig - ������ � �������� FFTSizeFilters / 2
	FLOAT_t scale = 1;
	const FLOAT_t step = POWF(slope, (FLOAT_t) 1 / (FFTSizeFilters / 2 - 1));
	const int n = FFTSizeFilters / 2;
	int i;
	for (i = 1; i < n; ++ i, scale *= step)
	{
		Sig [i].real *= scale;
		Sig [i].imag *= scale;
		Sig [FFTSizeFilters - i].real = Sig [i].real;
		Sig [FFTSizeFilters - i].imag = - Sig [i].imag;
	}
	/* ������������ ����������� ������� ������� */
	Sig [FFTSizeFilters / 2].real *= scale;
	Sig [FFTSizeFilters / 2].imag *= scale;
	/* ������? */
	//Sig [0].real = 0;
	//Sig [0].imag = 0;
#else

	// https://ru.wikipedia.org/wiki/%D0%A6%D0%B2%D0%B5%D1%82%D0%B0_%D1%88%D1%83%D0%BC%D0%B0
	// ����������� (�������, "����������") ���
	// ������� ���� ������ �� 6 ������� �� ������
	const FLOAT_t ratio = db2ratio(targetdb);
	const FLOAT_t slope = LOG10F(ratio) / (FLOAT_t) M_LOG2E;
	const int n = FFTSizeFilters / 2;
	//y = exp(log(100) - log(x)), x: 1..100
	const FLOAT_t delta = 1 / EXPF(slope * LOGF((FLOAT_t) 1 / n));
	int i;
	for (i = 1; i < n; ++ i)
	{
		const FLOAT_t scale = EXPF(slope * LOGF((FLOAT_t) i / n)) * delta;
		Sig [i].real *= scale;
		Sig [i].imag *= scale;
		Sig [FFTSizeFilters - i].real = Sig [i].real;
		Sig [FFTSizeFilters - i].imag = - Sig [i].imag;
	}
	/* ������������ ����������� ������� ������� */
	Sig [FFTSizeFilters / 2].real *= ratio;
	Sig [FFTSizeFilters / 2].imag *= ratio;
#endif
}

// ������������ ������� ��� ��������� ������ ���������
static void fir_design_adjust_rx(FLOAT_t *dCoeff, const FLOAT_t *dWindow, int iCoefNum, uint_fast8_t usewindow)
{
	if (glob_afresponcerx != 0)
	{
		imp_response(dCoeff, iCoefNum);	//  calculate impulse response of FIR filter
		correctspectrumcomplex(glob_afresponcerx);
		reconstruct();					// construct FIR coefficients from frequency response
		sigtocoeffs(dCoeff, iCoefNum);
	}

	if (usewindow != 0)
		fir_design_applaywindow(dCoeff, dWindow, iCoefNum);

	imp_response(dCoeff, iCoefNum);	//  calculate impulse response of FIR filter
	const FLOAT_t resp = getmaxresponce();
	scalecoeffs(dCoeff, iCoefNum, 1 / resp);	// ������������� �. �������� � ��������� �������� (1)
}

// ������������ ������� ��� ��������� ������ �����������
static void fir_design_adjust_tx(FLOAT_t *dCoeff, const FLOAT_t *dWindow, int iCoefNum)
{
	if (glob_afresponcetx != 0)
	{
		imp_response(dCoeff, iCoefNum);	//  calculate impulse response of FIR filter
		correctspectrumcomplex(glob_afresponcetx);
		reconstruct();	// construct FIR coefficients from frequency response
		sigtocoeffs(dCoeff, iCoefNum);
	}
	fir_design_applaywindow(dCoeff, dWindow, iCoefNum);
	imp_response(dCoeff, iCoefNum);	//  calculate impulse response of FIR filter
	const FLOAT_t resp = getmaxresponce();
	scalecoeffs(dCoeff, iCoefNum, 1 / resp);	// ������������� �. �������� � ��������� �������� (1)
}

// ������ ������������ ��� ������ � ���������� ������� ������� ���.
// ���������� �� ������ ����� � ��� - ������� ARMSAIRATE � ������.
// ��������: ���������� ������� ���� � ��������
// ���������: 1 - ���������, 0 - �������

static FLOAT_t MAKETAUIF(FLOAT_t t)
{
	if (t == 0)
		return 1;

	const FLOAT_t samplerate = ARMSAIRATE;	// 48 kHz
	const FLOAT_t step = POWF((FLOAT_t) M_SQRT1_2, 1 / (t * samplerate));
	//const FLOAT_t step = EXPF(- 1 / (t * samplerate));

	return 1 - step;
}

// ��������: ���������� ������� ���� � ��������
// ���������: 1 - ���������, 0 - �������
static FLOAT_t MAKETAUAF(FLOAT_t t)
{
	if (t == 0)
		return 1;

	const FLOAT_t samplerate = ARMI2SRATE;	// 48 kHz or 12 kHz
	const FLOAT_t step = POWF((FLOAT_t) M_SQRT1_2, 1 / (t * samplerate));
	//const FLOAT_t step = EXPF(- 1 / (t * samplerate));

	return 1 - step;
}

// ���������: 1 - ���������
static FLOAT_t MAKETAUAF0(void)
{
	return 1;
}

// ������������� �����.
// v1 - current value, v2 - charger value
// R2 by series with capacitor
// chargespeed 0: �������, 1: ���������
// ������� ��������� ������� ������������� ����� ������������� ��������.

static void charge2(volatile FLOAT_t * vcap, FLOAT_t vinput, FLOAT_t chargespeed)
{
	* vcap += (vinput - * vcap) * chargespeed;
}

typedef struct agcstate
{
	FLOAT_t  agcfastcap;	// ������� ����� �����������
	FLOAT_t  agcslowcap;	// ������� ����� �����������
	unsigned agchangticks;				// ������� ������� ���� ��������� agcslowcap ����������.
} agcstate_t;

typedef struct agcparams
{
	uint_fast8_t agcoff;	// ������� ���������� ���

	// ��������� ��������� ���

	// ���������� ������� ���� ��� ��� ������� �� ���������� ������ (������� ���).
	FLOAT_t dischargespeedfast;	//0.02f;	// 1 - ���������, 0 - �������
	FLOAT_t	chargespeedfast;

	// ���������� ������� ��������� ������� ��� -  ����� ������ ������ ���� ���� �� �������, ��� � ������ ���� ������� ���
	FLOAT_t chargespeedslow;		//0.05f;	// 1 - ���������, 0 - �������
	FLOAT_t dischargespeedslow;	// 1 - ���������, 0 - �������
	unsigned hungticks;				// ������� ������� ���� ��������� agcslowcap ����������.

	// ����������� ��������� ���

	FLOAT_t gainlimit;				// ������������ �������� � ����� �� ����������, ���������� ��� ���
	FLOAT_t	mininput;
	FLOAT_t levelfence;				// ������������� �������� �� ������ ���
	FLOAT_t agcfactor;				// �������� ��� ���������� "����������" ���
} agcparams_t;


/////////////
// agc +++

static FLOAT_t agc_calcagcfactor(uint_fast8_t rate)
{
	return - (1 - 1 / (FLOAT_t) rate);
}

// ��������� ���������  ���������� ��� ��������

static void agc_parameters_initialize(volatile agcparams_t * agcp)
{
	agcp->agcoff = 0;

	agcp->dischargespeedfast = MAKETAUIF((FLOAT_t) 0.095);
#if WITHLIMITEDAGCATTACK
	agcp->chargespeedfast = MAKETAUIF((FLOAT_t) 0.001);	// 1 mS
#else /* WITHLIMITEDAGCATTACK */
	agcp->chargespeedfast = MAKETAUAF0();
#endif /* WITHLIMITEDAGCATTACK */

	agcp->chargespeedslow = MAKETAUIF((FLOAT_t) 0.095);
	agcp->dischargespeedslow = MAKETAUIF((FLOAT_t) 0.2);	

	agcp->hungticks = NSAITICKS(300);			// 0.3 secounds

	agcp->gainlimit = db2ratio(60);
	agcp->mininput = 16;
	agcp->levelfence = rxlevelfence;
	agcp->agcfactor = agc_calcagcfactor(10);

	//debug_printf_P(PSTR("agc_parameters_initialize: dischargespeedfast=%f, chargespeedfast=%f\n"), agcp->dischargespeedfast, agcp->chargespeedfast);
}

// ��������� ���������� ��� ��������

static void agc_parameters_update(volatile agcparams_t * const agcp, FLOAT_t gainlimit, uint_fast8_t pathi)
{
	const uint_fast8_t flatgain = glob_agcrate [pathi] == UINT8_MAX;

	agcp->agcoff = (glob_agc == BOARD_AGCCODE_OFF);

	agcp->dischargespeedfast = MAKETAUIF((int) glob_agc_t4 [pathi] * (FLOAT_t) 0.001);	// � ������������

	agcp->chargespeedslow = MAKETAUIF((int) glob_agc_t1 [pathi] * (FLOAT_t) 0.001);	// � ������������
	agcp->dischargespeedslow = MAKETAUIF((int) glob_agc_t2 [pathi] * (FLOAT_t) 0.1);	// � ������ ���������� (0.1 �������)
	agcp->hungticks = NSAITICKS(glob_agc_thung [pathi] * 100);			// � ������ ���������� (0.1 �������)

	agcp->gainlimit = gainlimit;
	agcp->agcfactor = flatgain ? (FLOAT_t) -1 : agc_calcagcfactor(glob_agcrate [pathi]);

	//debug_printf_P(PSTR("agc_parameters_update: dischargespeedfast=%f, chargespeedfast=%f\n"), agcp->dischargespeedfast, agcp->chargespeedfast);
}

// ��������� ���������� S-����� ��������

static void agc_smeter_parameters_update(volatile agcparams_t * const agcp, FLOAT_t gainlimit, uint_fast8_t pathi)
{
	const uint_fast8_t flatgain = glob_agcrate [pathi] == UINT8_MAX;

	agcp->agcoff = (glob_agc == BOARD_AGCCODE_OFF);

	agcp->dischargespeedfast = MAKETAUIF((int) glob_agc_t4 [pathi] * (FLOAT_t) 0.001);	// � ������������

	agcp->chargespeedslow = MAKETAUIF((FLOAT_t) 0.015);	// 15 mS
	agcp->dischargespeedslow = MAKETAUIF((FLOAT_t) 0.3);	// 300 mS
	agcp->hungticks = NSAITICKS(800);			// � ������ ���������� (0.8 �������)

	agcp->gainlimit = gainlimit;
	agcp->agcfactor = flatgain ? (FLOAT_t) -1 : agc_calcagcfactor(glob_agcrate [pathi]);

	//debug_printf_P(PSTR("agc_parameters_update: dischargespeedfast=%f, chargespeedfast=%f\n"), agcp->dischargespeedfast, agcp->chargespeedfast);
}

// ��������� ��������� ���������� ��� ������������ ������ �����������

static void comp_parameters_initialize(volatile agcparams_t * agcp)
{
	agcp->agcoff = 0;

	agcp->dischargespeedfast = MAKETAUIF((FLOAT_t) 0.100);
	agcp->chargespeedfast = MAKETAUAF0();

	agcp->chargespeedslow = MAKETAUIF((FLOAT_t) 0.200);
	agcp->dischargespeedslow = MAKETAUIF((FLOAT_t) 0.200);

	agcp->hungticks = NSAITICKS(300);			// 0.3 secounds

	agcp->gainlimit = db2ratio(60);
	agcp->mininput = 1;
	agcp->levelfence = txlevelfence * (int) glob_mikeagcscale / 100;
	agcp->agcfactor = (FLOAT_t) - 1;
}

// ��������� ���������� ��� �����������

static void comp_parameters_update(volatile agcparams_t * const agcp, FLOAT_t gainlimit)
{
	agcp->agcoff = glob_mikeagc == 0;

	agcp->gainlimit = gainlimit;
	agcp->levelfence = txlevelfence * (int) glob_mikeagcscale / 100;
}

// �������� ��� - ������������ �������� �������� ��������������� ������� 
// �� ����� ����������� ���������� �� ������������/����������

static void
performagc(const volatile agcparams_t * agcp, volatile agcstate_t * st, FLOAT_t sample)
{
	// ������� ���� ���
	if (st->agcfastcap < sample)
	{
		// ���������� � ������������ � ���������� agcp->chargespeedfast
		charge2(& st->agcfastcap, sample, agcp->chargespeedfast);
	}
	else
	{
		// ������ �� ��������� agcp->dischargespeedfast
		charge2(& st->agcfastcap, sample, agcp->dischargespeedfast);
	}

	// ��������� ���� ���
	// hang time processing
	if (st->agcslowcap < st->agcfastcap)	// ��������� ����� slow ����
	{
		// ����������
		charge2(& st->agcslowcap, st->agcfastcap, agcp->chargespeedslow);
		st->agchangticks = agcp->hungticks;
	}
	else if (st->agcslowcap > st->agcfastcap)	// ��������� ����� slow ����
	{
		if (st->agchangticks == 0)
		{
			// �����������
			charge2(& st->agcslowcap, st->agcfastcap, agcp->dischargespeedslow);
		}
		else
		{
			// ���� ��������� hang time
			st->agchangticks -= 1;
		}
	}
	else
	{
		// �� �������� ��������
		st->agchangticks = agcp->hungticks;
	}
}

static FLOAT_t performagcresultslow(const volatile agcstate_t * st)
{
	return FMAXF(st->agcfastcap, st->agcslowcap);	// ������� ����� ���
}

static FLOAT_t performagcresultfast(const volatile agcstate_t * st)
{
	return st->agcfastcap;
}


///////////////////////////

static FLOAT_t mikeinlevel;
static FLOAT_t VOXDISCHARGE;
static FLOAT_t VOXCHARGE = 0;

// ���������� �������� 0..255
uint_fast8_t dsp_getvox(void)
{
	return mikeinlevel / (1 << (WITHAFADCWIDTH - 8 - 1));	// �������������� q15 � 0..255
}

// ���������� �������� 0..255
uint_fast8_t dsp_getavox(void)
{
	return 0;
}

/* ��������� �������� ������������ ��� ������������ ������ */
uint_fast8_t dsp_getmikeadcoverflow(void)
{
	return 0;
}

static void voxmeter_initialize(void)
{
	VOXCHARGE = MAKETAUAF0();	// ������� �������� �� �������� ������ 0
	VOXDISCHARGE = MAKETAUAF((FLOAT_t) 0.02);	// ������� �������� �� �������� ������� 0.02 �������
}

// �������������� NFM

static FLOAT_t NBTAU;
static FLOAT_t holdmaxv3;
static void testholdmax3(FLOAT_t v)
{
	FLOAT_t x = holdmaxv3;
	FLOAT_t t = FABSF(v);
	x = (x < t) ? t : x * NBTAU;
	holdmaxv3 = x;
}
long getholdmax3(void)
{
	return holdmaxv3;
}


/////////////////////////////
// ������������ ����������� � ���������
//

#if 0
/* ������, ������������ ��������������� MATLAB ������� ����������.
 * Tnx to RX9CIM
 */
static FLOAT_t iir_filter(
	FLOAT_t new_sample, 
	FLOAT_t (* aWHistory) [2], 
	const FLOAT_t (* aNUM) [3], const FLOAT_t (* aDEN) [3]
	)
{
	int i;
	for (i = 0; i < MWSPT_NSEC; i ++)
	{
		FLOAT_t buf = new_sample;
		int j;
		for (j = 1; j < DL_lpf_1550 [i]; j ++)
		{
			buf -= aDEN[i][j] * aWHistory [i][j - 1];
		}
		buf /= aDEN[i][0];
		new_sample = buf * aNUM [i][0];
		for (j = 1; j < NL_lpf_1550 [i]; j ++)
		{  
			new_sample += aNUM[i][j] * aWHistory [i] [j - 1];
		}
		aWHistory [i] [1] = aWHistory [i] [0];
		aWHistory [i] [0] = buf;
	}
	return new_sample;
}
#endif

// �������� �������� ������� �� ������� 0
// ������ ������������� ��� ������������� �������
static FLOAT_t testgain_float_DC(const FLOAT_t * dCoeff, int iCoefNum)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	int iCnt;
	FLOAT_t dSum = 0;
	for (iCnt = 0; iCnt < iHalfLen; iCnt ++)
	{
		dSum += dCoeff [iCnt] * 2;	// ��������� �� 2 - ��� ��� ��������� �������� ������������� (������������ ������)
	}
	dSum += dCoeff [iHalfLen];		// �������������� �������� ��� ������� � float ����������
	return FABSF(dSum);		// �������� ������ �������� �� ���������� ������������.
}

// �������� �������� ������� �� ������� 0
// ������ ������������� ��� ������������� �������
static double testgain_float_DCL(const double * dCoeff, int iCoefNum)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	int iCnt;
	double dSum = 0;
	for (iCnt = 0; iCnt < iHalfLen; iCnt ++)
	{
		dSum += dCoeff [iCnt] * 2;	// ��������� �� 2 - ��� ��� ��������� �������� ������������� (������������ ������)
	}
	dSum += dCoeff [iHalfLen];		// �������������� �������� ��� ������� � float ����������
	return fabs(dSum);		// �������� ������ �������� �� ���������� ������������.
}

#if 0
	double getWindow(int i, int n) {
	    if (n == 1)
		return 1;
	    double x = 2*pi*i/(n-1);
	    double n2 = n/2; // int
	    switch (windowChooser.getSelectedIndex()) {
	    case 0: return 1; // rect
	    case 1: return .54 - .46*Math.cos(x); // hamming
	    case 2: return .5  -  .5*Math.cos(x); // hann
	    case 3: return .42 -  .5*Math.cos(x) + .08*Math.cos(2*x); // blackman
	    case 4: // kaiser 
				{
				double kaiserAlphaPi = kaiserBar.getValue()*pi/120.;
				double q = (2*i/(double) n)-1;
				return bessi0(kaiserAlphaPi*Math.sqrt(1-q*q));
				}
	    case 5: return (i < n2) ? i/n2 : 2-i/n2; // bartlett
	    case 6: { double xt = (i-n2)/n2; return 1-xt*xt; } // welch
	    }
	    return 0;
	}
#endif

	/*
	double getWindow(int i, int n) {
	    if (n == 1)
		return 1;
	    double x = 2*pi*i/(n-1);
	    double n2 = n/2; // int
	    switch (windowChooser.getSelectedIndex()) {
	    case 0: return 1; // rect
	    case 1: return .54 - .46*Math.cos(x); // hamming
	    case 2: return .5  -  .5*Math.cos(x); // hann
	    case 3: return .42 -  .5*Math.cos(x) + .08*Math.cos(2*x); // blackman
	    case 4: // kaiser 
				{
		double kaiserAlphaPi = kaiserBar.getValue()*pi/120.;
		double q = (2*i/(double) n)-1;
		return bessi0(kaiserAlphaPi*Math.sqrt(1-q*q));
	    }
	    case 5: return (i < n2) ? i/n2 : 2-i/n2; // bartlett
	    case 6: { double xt = (i-n2)/n2; return 1-xt*xt; } // welch
	    }
	    return 0;
	}
	*/

// Calculate window function (blackman-harris, hamming, rectangular)
static FLOAT_t fir_design_window(int iCnt, int iCoefNum)
{
	const int n = iCoefNum - 1;
	const FLOAT_t a = (FLOAT_t) M_TWOPI * iCnt / n;
	const FLOAT_t a2 = 2 * (FLOAT_t) M_TWOPI * iCnt / n;	// ��� ��������� �������� ��������� ���������� �� �������
	const FLOAT_t a3 = 3 * (FLOAT_t) M_TWOPI * iCnt / n;	// ��� ��������� �������� ��������� ���������� �� �������
	const FLOAT_t a4 = 4 * (FLOAT_t) M_TWOPI * iCnt / n;	// ��� ��������� �������� ��������� ���������� �� �������
	const FLOAT_t a5 = 5 * (FLOAT_t) M_TWOPI * iCnt / n;	// ��� ��������� �������� ��������� ���������� �� �������
	const FLOAT_t a6 = 6 * (FLOAT_t) M_TWOPI * iCnt / n;	// ��� ��������� �������� ��������� ���������� �� �������
	
	switch (BOARD_WTYPE_BLACKMAN_HARRIS_MOD)
	{
	default:
	case BOARD_WTYPE_BLACKMAN_HARRIS:
		// Blackman-Harris
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.35875 
				- (FLOAT_t) 0.48829 * COSF(a) 
				+ (FLOAT_t) 0.14128 * COSF(a2) 
				- (FLOAT_t) 0.01168 * COSF(a3)
				);
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_MOD:
		// Modified Blackman-Harris (classic)
		// http://nsg.sourceforge.net/doc/windows/winfuns_code.php
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.35872
				- (FLOAT_t) 0.48832 * COSF(a) 
				+ (FLOAT_t) 0.14128 * COSF(a2) 
				- (FLOAT_t) 0.01168 * COSF(a3)
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_3TERM:
		// Blackman-Harris 3-term
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.42323
				- (FLOAT_t) 0.49755 * COSF(a) 
				+ (FLOAT_t) 0.07922 * COSF(a2) 
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_3TERM_MOD:
		// Blackman-Harris 3-term
		// in https://www.dsprelated.com/freebooks/sasp/Three_Term_Blackman_Harris_Window.html
		// 0.4243801 0.4973406 0.0782793
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.4243801
				- (FLOAT_t) 0.4973406 * COSF(a) 
				+ (FLOAT_t) 0.0782793 * COSF(a2) 
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_7TERM:
		// Blackman-Harris 7-term
		// �� float � ��������� ��������� �� ����� ������ ���������� � BOARD_WTYPE_BLACKMAN_HARRIS
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.27105140069342
				- (FLOAT_t) 0.43329793923448 * COSF(a) 
				+ (FLOAT_t) 0.21812299954311 * COSF(a2) 
				- (FLOAT_t) 0.06592544638803 * COSF(a3) 
				+ (FLOAT_t) 0.01081174209837 * COSF(a4) 
				- (FLOAT_t) 0.00077658482522 * COSF(a5) 
				+ (FLOAT_t) 0.00001388721735 * COSF(a6)
			);
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_NUTTALL:
		// Blackman-Nuttall
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.3635819
				- (FLOAT_t) 0.4891775 * COSF(a) 
				+ (FLOAT_t) 0.1365995 * COSF(a2) 
				- (FLOAT_t) 0.0106411 * COSF(a3)
				);	
			return w;
		}
	case BOARD_WTYPE_HAMMING:
		// Hamming
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.54
				- (FLOAT_t) 0.46 * COSF(a)
				);	
			return w;
		}
	case BOARD_WTYPE_HANN:
		// Hann
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.5
				- (FLOAT_t) 0.5 * COSF(a)
				);	
			return w;
		}
	case BOARD_WTYPE_RECTANGULAR:
		// rectangular
		{
			const FLOAT_t w = 1;
			return w;
		}
	}
}


// Calculate window function (blackman-harris, hamming, rectangular)
static double fir_design_windowL(int iCnt, int iCoefNum)
{
	const int n = iCoefNum - 1;
	const double a = (double) M_TWOPI * iCnt / n;
	const double a2 = 2 * (double) M_TWOPI * iCnt / n;	// ��� ��������� �������� ��������� ���������� �� �������
	const double a3 = 3 * (double) M_TWOPI * iCnt / n;	// ��� ��������� �������� ��������� ���������� �� �������
	const double a4 = 4 * (double) M_TWOPI * iCnt / n;	// ��� ��������� �������� ��������� ���������� �� �������
	const double a5 = 5 * (double) M_TWOPI * iCnt / n;	// ��� ��������� �������� ��������� ���������� �� �������
	const double a6 = 6 * (double) M_TWOPI * iCnt / n;	// ��� ��������� �������� ��������� ���������� �� �������
	
	switch (BOARD_WTYPE_BLACKMAN_HARRIS_MOD)
	{
	default:
	case BOARD_WTYPE_BLACKMAN_HARRIS:
		// Blackman-Harris
		{
			const double w = (
				+ (double) 0.35875
				- (double) 0.48829 * cos(a) 
				+ (double) 0.14128 * cos(a2) 
				- (double) 0.01168 * cos(a3)
			);	
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_MOD:
		// Modified Blackman-Harris (classic)
		// http://nsg.sourceforge.net/doc/windows/winfuns_code.php
		{
			const double w = (
				+ (double) 0.35872
				- (double) 0.48832 * cos(a) 
				+ (double) 0.14128 * cos(a2) 
				- (double) 0.01168 * cos(a3)
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_3TERM:
		// Blackman-Harris 3-term
		{
			const double w = (
				+ (double) 0.42323
				- (double) 0.49755 * cos(a) 
				+ (double) 0.07922 * cos(a2) 
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_3TERM_MOD:
		// Blackman-Harris 3-term
		// in https://www.dsprelated.com/freebooks/sasp/Three_Term_Blackman_Harris_Window.html
		// 0.4243801 0.4973406 0.0782793
		{
			const double w = (
				+ (double) 0.4243801
				- (double) 0.4973406 * cos(a) 
				+ (double) 0.0782793 * cos(a2) 
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_7TERM:
		// Blackman-Harris 7-term
		// �� float � ��������� ��������� �� ����� ������ ���������� � BOARD_WTYPE_BLACKMAN_HARRIS
		{
			const double w = (
				+ (double) 0.27105140069342
				- (double) 0.43329793923448 * cos(a) 
				+ (double) 0.21812299954311 * cos(a2) 
				- (double) 0.06592544638803 * cos(a3) 
				+ (double) 0.01081174209837 * cos(a4) 
				- (double) 0.00077658482522 * cos(a5) 
				+ (double) 0.00001388721735 * cos(a6)
			);
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_NUTTALL:
		// Blackman-Nuttall
		{
			const double w = (
				+ (double) 0.3635819
				- (double) 0.4891775 * cos(a) 
				+ (double) 0.1365995 * cos(a2) 
				- (double) 0.0106411 * cos(a3)
				);	
			return w;
		}
	case BOARD_WTYPE_HAMMING:
		// Hamming
		{
			const double w = (
				+ (double) 0.54
				- (double) 0.46 * cos(a)
				);	
			return w;
		}
	case BOARD_WTYPE_HANN:
		// Hann
		{
			const double w = (
				+ (double) 0.5
				- (double) 0.5 * cos(a)
				);	
			return w;
		}
	case BOARD_WTYPE_RECTANGULAR:
		// rectangular
		{
			const double w = 1;
			return w;
		}
	}
}


////////////////////////////////////////////////

// ������ ������� ������ ������
// ������ ������� ��� ��������� ������� �������
static void fir_design_lowpass(FLOAT_t *dCoeff, int iCoefNum, FLOAT_t m_fCutHigh)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	//float dCoeff [iHalfLen + 1];	/* Use GCC extension */
	int iCnt;

	/*------------------*/
	/*  Lowpass filter  */
	/*------------------*/

	dCoeff [iHalfLen] = m_fCutHigh;
	for (iCnt = 1; iCnt <= iHalfLen; iCnt ++)
	{
		const FLOAT_t a = (FLOAT_t) M_PI * iCnt;
		const FLOAT_t k = SINF(m_fCutHigh * a) / a;
		dCoeff [iHalfLen - iCnt] = k;
	}
}

// ������ ������� ������ ������
// ������ ������� ��� ��������� ������� �������
static void fir_design_lowpassL(double *dCoeff, int iCoefNum, double m_fCutHigh)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	//float dCoeff [iHalfLen + 1];	/* Use GCC extension */
	int iCnt;

	/*------------------*/
	/*  Lowpass filter  */
	/*------------------*/

	dCoeff [iHalfLen] = m_fCutHigh;
	for (iCnt = 1; iCnt <= iHalfLen; iCnt ++)
	{
		const double a = (double) M_PI * iCnt;
		const double k = sin(m_fCutHigh * a) / a;
		dCoeff [iHalfLen - iCnt] = k;
	}
}

// ������ ������� - ������
// ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow < CutHigh))
// ������ ������� ��� ��������� ������� �������
static void fir_design_bandstop(FLOAT_t * dCoeff, int iCoefNum, FLOAT_t fCutLow, FLOAT_t fCutHigh)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	int iCnt;

	/*------------------*/
	/* Bandstop filter  */
	/*------------------*/
	//if ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow>CutHigh)) {

	dCoeff [iHalfLen] = fCutLow - fCutHigh;   
	for (iCnt = 1; iCnt <= iHalfLen; iCnt ++) 
	{ 
		const FLOAT_t a = (FLOAT_t) M_PI_2 * iCnt;	// M_PI_2 - constant pi/2
		const FLOAT_t k = SINF((fCutLow - fCutHigh) * a) * COSF((fCutHigh + fCutLow) * a) / a;  
		dCoeff [iHalfLen - iCnt] = k;
	}   

	dCoeff [iHalfLen] += 1;   
}

// ������ ������� ��� ��������� ������� �������
static void fir_design_passtrough(FLOAT_t * dCoeff, int iCoefNum, FLOAT_t dGain)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	int iCnt;

	for (iCnt = 1; iCnt <= iHalfLen; iCnt ++)
	{
		dCoeff [iHalfLen - iCnt] = 0;
	}
	dCoeff [iHalfLen] = dGain;
}

// ��� ��������� ������� �������
static void fir_design_bandpass(FLOAT_t * dCoeff, int iCoefNum, FLOAT_t fCutLow, FLOAT_t fCutHigh)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	//float dCoeff [iHalfLen + 1];	/* Use GCC extension */
	//const float dGain = 1;		// ��������� �������� �������
	int iCnt;

	dCoeff [iHalfLen] = fCutHigh - fCutLow;
	for (iCnt = 1; iCnt <= iHalfLen; iCnt ++)
	{
		const FLOAT_t a = (FLOAT_t) M_PI_2 * iCnt;	// M_PI_2 - constant pi/2
		const FLOAT_t k = SINF((fCutHigh - fCutLow) * a) *
										 COSF((fCutHigh + fCutLow) * a) / a;
		dCoeff [iHalfLen - iCnt] = k;
	}
}

// ��������� ������� �������
static void fir_design_applaywindow(FLOAT_t *dCoeff, const FLOAT_t *dWindow, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dCoeff [iCnt] *= dWindow [iCnt];
	}
}

// ��������� ������� �������
static void fir_design_applaywindowL(double *dCoeff, const double *dWindow, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dCoeff [iCnt] *= dWindow [iCnt];
	}
}

// ���������� ������ � ������� ��������
static void fir_design_windowbuff(FLOAT_t *dWindow, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dWindow [iCnt] = fir_design_window(iCnt, iCoefNum);
	}
}

// ���������� ������ � ������� ��������
static void fir_design_windowbuffL(double *dWindow, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dWindow [iCnt] = fir_design_windowL(iCnt, iCoefNum);
	}
}

// ��������������� ��� ������������� �������
static void fir_design_scale(FLOAT_t * dCoeff, int iCoefNum, FLOAT_t dScale)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	if (dScale == 1)
		return;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dCoeff [iCnt] *= dScale;
	}
}

// ��������������� ��� ������������� �������
static void fir_design_scaleL(double * dCoeff, int iCoefNum, double dScale)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	if (dScale == 1)
		return;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dCoeff [iCnt] *= dScale;
	}
}

#if 0
/* ��������� ������������ ������� ������� */
static int getCoefNumLtdValidated(int iCoefNum)
{
	enum { WITHFILTSOFTDENOM = 10, WITHFILTSOFTSCALE = 8 };	// ���������� ������������� ����������� �� 0.2 �� ��������� ��������
	const int iCoefNumLtd = iCoefNum * WITHFILTSOFTSCALE * (glob_fltsofter - WITHFILTSOFTMIN) / (WITHFILTSOFTDENOM * (WITHFILTSOFTMAX - WITHFILTSOFTMIN));
	return NtapValidate(iCoefNum - iCoefNumLtd);
}
#endif

/* ������ ���������� - ������� ��� ������� ��������� �������� */
#define GETNORMFREQ(freq)	((freq) * 2 / (FLOAT_t) ARMSAIRATE)
#define GETNORMFREQAUDIO(freq)	((freq) * 2 / (FLOAT_t) ARMI2SRATE)
#define GETNORMFREQAUDIOL(freq)	((freq) * 2 / (double) ARMI2SRATE)

static FLOAT_t fir_design_normfreq(int freq)
{
	return GETNORMFREQAUDIO(freq);
}

static double fir_design_normfreqL(int freq)
{
	return GETNORMFREQAUDIOL(freq);
}

static void fir_design_lowpass_freq(FLOAT_t * dCoeff, int iCoefNum, int iCutHigh)
{
	fir_design_lowpass(dCoeff, iCoefNum, fir_design_normfreq(iCutHigh));
}

// � ����������� ��������� ������ � ������������� ��������, � ���������� ����
static void fir_design_lowpass_freq_scaled(FLOAT_t * dCoeff, const FLOAT_t * dWindow, int iCoefNum, int iCutHigh, FLOAT_t dGain)
{
	fir_design_lowpass(dCoeff, iCoefNum, fir_design_normfreq(iCutHigh));
	fir_design_applaywindow(dCoeff, dWindow, iCoefNum);
	fir_design_scale(dCoeff, iCoefNum, dGain / testgain_float_DC(dCoeff, iCoefNum));
}

// � ����������� ��������� ������ � ������������� ��������, � ���������� ����
static void fir_design_lowpass_freq_scaledL(double * dCoeff, const double * dWindow, int iCoefNum, int iCutHigh, double dGain)
{
	fir_design_lowpassL(dCoeff, iCoefNum, fir_design_normfreqL(iCutHigh));
	fir_design_applaywindowL(dCoeff, dWindow, iCoefNum);
	fir_design_scaleL(dCoeff, iCoefNum, dGain / testgain_float_DCL(dCoeff, iCoefNum));
}

// ������ ������������� ��� ��������������� �������
static void fir_design_bandpass_freq(FLOAT_t * dCoeff, int iCoefNum, int iCutLow, int iCutHigh)
{
	fir_design_bandpass(dCoeff, iCoefNum, fir_design_normfreq(iCutLow), fir_design_normfreq(iCutHigh));
}

#if WITHDSPEXTFIR

// �������������� � �����
static void fir_design_copy_integers(int_fast32_t * lCoeff, const FLOAT_t * dCoeff, int iCoefNum)
{
	const FLOAT_t scaleout = POWF(2, HARDWARE_COEFWIDTH - 1);
	int iCnt;
	const int j = NtapCoeffs(iCoefNum);
	// �������� ���������.
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		lCoeff [iCnt] = dCoeff [iCnt] * scaleout;
	}
}

static void fir_design_integer_lowpass_scaled(int_fast32_t *lCoeff, const FLOAT_t *dWindow, int iCoefNum, int iCutHigh, FLOAT_t dGain)
{
	FLOAT_t dCoeff [NtapCoeffs(iCoefNum)];	/* Use GCC extension */
	fir_design_lowpass_freq_scaled(dCoeff, dWindow, iCoefNum, iCutHigh, dGain);	// � ����������� ��������� ������ � ������������� ��������, � ���������� ����
	fir_design_copy_integers(lCoeff, dCoeff, iCoefNum);
}

// �������������� � �����
static void fir_design_copy_integersL(int_fast32_t * lCoeff, const double * dCoeff, int iCoefNum)
{
	const double scaleout = pow(2, HARDWARE_COEFWIDTH - 1);
	int iCnt;
	const int j = NtapCoeffs(iCoefNum);
	// �������� ���������.
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		lCoeff [iCnt] = dCoeff [iCnt] * scaleout;
	}
}

static void fir_design_integer_lowpass_scaledL(int_fast32_t *lCoeff, const double *dWindow, int iCoefNum, int iCutHigh, double dGain)
{
	double dCoeff [NtapCoeffs(iCoefNum)];	/* Use GCC extension */
	fir_design_lowpass_freq_scaledL(dCoeff, dWindow, iCoefNum, iCutHigh, dGain);	// � ����������� ��������� ������ � ������������� ��������, � ���������� ����
	fir_design_copy_integersL(lCoeff, dCoeff, iCoefNum);
}

static void fir_design_integers_passtrough(int_fast32_t *lCoeff, int iCoefNum, FLOAT_t dGain)
{
	FLOAT_t dCoeff [NtapCoeffs(iCoefNum)];	/* Use GCC extension */
	fir_design_passtrough(dCoeff, iCoefNum, dGain);
	fir_design_copy_integers(lCoeff, dCoeff, iCoefNum);
}

#endif /* WITHDSPEXTFIR */

#if 0
// debug function
static void writecoefs(const int_fast32_t * lCoeff, int iCoefNum)
{
	debug_printf_P(PSTR("# iCoefNum = %i\n"), iCoefNum);
	const int iHalfLen = (iCoefNum - 1) / 2;
	int i;
	for (i = 0; i <= iHalfLen; ++ i)
	{
		debug_printf_P(PSTR("%ld\n"), lCoeff [i]);
	}
	i -= 1;
	for (; -- i >= 0; )
	{
		debug_printf_P(PSTR("%ld\n"), lCoeff [i]);
	}
	debug_printf_P(PSTR("# end\n"));
}
#endif

/*-----------------------------------*/

// float32 * float32 -> float32
static RAMFUNC FLOAT32P_t scalepair(FLOAT32P_t a, FLOAT_t b)
{
	a.IV *= b;
	a.QV *= b;
	return a;
}

#if 0

// int32 * int32 -> int32
static RAMFUNC FLOAT32P_t scalepair_int32(INT32P_t a, int_fast32_t b)
{
	FLOAT32P_t v;
	const int_fast64_t bi = b;
	v.IV = (a.IV * bi) >> 31;
	v.QV = (a.QV * bi) >> 31;
	return v;
}
#endif

#if WITHDSPLOCALFIR


#if WITHDSPEXTDDC
// ������ ������������ ������� ��������
// ������������ � ������ �������� DDCV
static RAMFUNC_NONILINE FLOAT32P_t filter_firp_rx_SSB_IQ(FLOAT32P_t NewSample)
{
	const FLOAT_t * const k = FIRCoef_rx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_rx_SSB_IQ, NtapHalf = Ntap / 2 };
	// ����� � ������������ ���������� �������
	static FLOAT32P_t x [Ntap * 2] = { { { 0, 0 }, }, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	// fir_head -  ������ �������������� ����� ������
	// fir_head + Ntap -  ������� �� ������ ��������������� �����
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    x [fir_head] = x [fir_head + Ntap] = NewSample;

	uint_fast16_t bh = fir_head + NtapHalf;			// ������ �������������� ����� ������
	uint_fast16_t bt = bh;	// ������� �� ������ ��������������� ������
    // Calculate the new output
	uint_fast16_t n = NtapHalf;
	// ������� � �������� ������
	FLOAT32P_t v = scalepair(x [bh], k [n]);            // sample at middle of buffer
	do
	{	
		{
			const FLOAT_t kv = k [-- n];
			v.IV += kv * (x [-- bh].IV + x [++ bt].IV);
			v.QV += kv * (x [bh].QV + x [bt].QV);
		}
		{
			const FLOAT_t kv = k [-- n];
			v.IV += kv * (x [-- bh].IV + x [++ bt].IV);
			v.QV += kv * (x [bh].QV + x [bt].QV);
		}
	}
	while (n != 0);

    return v;
}
// ������ ������������ ������� �����������
// ������������ � ������ �������� DUC
static RAMFUNC_NONILINE FLOAT32P_t filter_firp_tx_SSB_IQ(FLOAT32P_t NewSample)
{
	const FLOAT_t * const k = FIRCoef_tx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_tx_SSB_IQ, NtapHalf = Ntap / 2 };
	// ����� � ������������ ���������� �������
	static FLOAT32P_t x [Ntap * 2] = { { { 0, 0 }, }, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	// fir_head -  ������ �������������� ����� ������
	// fir_head + Ntap -  ������� �� ������ ��������������� �����
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    x [fir_head] = x [fir_head + Ntap] = NewSample;

	uint_fast16_t bh = fir_head + NtapHalf;			// ������ �������������� ����� ������
	uint_fast16_t bt = bh;	// ������� �� ������ ��������������� ������
    // Calculate the new output
	uint_fast16_t n = NtapHalf;
	// ������� � �������� ������
	FLOAT32P_t v = scalepair(x [bh], k [n]);            // sample at middle of buffer
	do
	{	
		{
			const FLOAT_t kv = k [-- n];
			v.IV += kv * (x [-- bh].IV + x [++ bt].IV);
			v.QV += kv * (x [bh].QV + x [bt].QV);
		}
		{
			const FLOAT_t kv = k [-- n];
			v.IV += kv * (x [-- bh].IV + x [++ bt].IV);
			v.QV += kv * (x [bh].QV + x [bt].QV);
		}
	}
	while (n != 0);

    return v;
}

#else  /* WITHDSPEXTDDC */
// �������
// ������ (floating point) ��� ������������ ��������. ����������� ��� multiplier 0 1 0 -1.
// Speed tests: DT=145 (129), ntaps=481

static RAMFUNC_NONILINE FLOAT32P_t filter_fir4_rx_SSB_IQ(FLOAT32P_t NewSample, uint_fast8_t useI) 
{
	const FLOAT_t * const k = FIRCoef_rx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_rx_SSB_IQ, NtapHalf = Ntap / 2 };
	FLOAT32P_t rv;

	// ����� � ������������ ���������� �������
	static FLOAT32P_t x [Ntap * 2] = { { { 0, 0, }, }, }; // input samples (force CCM allocation)
	static uint_fast16_t fir_head = 0;		// ������� ������ � ����� � ��������� ���

	// shift the old samples
	// fir_head -  ������ �������������� ����� ������
	// fir_head + Ntap -  ������� �� ������ ��������������� �����
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
	const uint_fast16_t fir_tail = fir_head + Ntap;
	// Calculate the new output

	uint_fast16_t n;
	if (useI != 0)
	{
		// ���� ������� == 0 - ��� � ��� ����� ���� ����������
		x [fir_head].IV = NewSample.IV;
		x [fir_tail].IV = NewSample.IV;
		//
		// ������������ I
		//
		// ������� � �������� ������
		rv.IV = k [NtapHalf] * x [fir_head + NtapHalf].IV;
		uint_fast16_t bh = fir_head;	// ������ �������������� ����� ������
		uint_fast16_t bt = fir_tail;	// ������� �� ������ ��������������� ������
		// NewSample.QV ����� ����� 0 - ����� �� �����������
		n = 0;
		do
		{	
			// ��������� I
			// 1_23 * 1_31 = 1_54. ��� ����� ��������� �� FIRCoef.
			rv.IV += k [n ++] * (x [bh ++].IV + x [-- bt].IV);
			// ���������� Q
			++ n;
			++ bh;
			-- bt;
		}
		while (n < NtapHalf);
		//
		// ������������ Q
		//
		rv.QV = 0;
		bh = fir_head;			// ������ �������������� ����� ������
		bt = fir_tail;			// ������� �� ������ ��������������� ������
		n = 0;
		do
		{	
			// ���������� I
			++ n;
			++ bh;
			-- bt;
			// ��������� Q
			// 1_23 * 1_31 = 1_54. ��� ����� ��������� �� FIRCoef.
			rv.QV += k [n ++] * (x [bh ++].QV + x [-- bt].QV);
		}
		while (n < NtapHalf);
	}
	else
	{
		// ���� ������� == 0 - ��� � ��� ����� ���� ����������
		x [fir_head].QV = NewSample.QV;
		x [fir_tail].QV = NewSample.QV;
		//
		// ������������ Q
		//
		// ������� � �������� ������
		rv.QV = k [NtapHalf] * x [fir_head + NtapHalf].QV;
		uint_fast16_t bh = fir_head;	// ������ �������������� ����� ������
		uint_fast16_t bt = fir_tail;	// ������� �� ������ ��������������� ������
		// NewSample.IV ����� ����� 0 - ����� �� �����������
		n = 0;
		do
		{	
			// ��������� Q
			// 1_23 * 1_31 = 1_54. ��� ����� ��������� �� FIRCoef.
			rv.QV += k [n ++] * (x [bh ++].QV + x [-- bt].QV);
			// ���������� I
			++ n;
			++ bh;
			-- bt;
		}
		while (n < NtapHalf);
		//
		// ������������ I
		//
		rv.IV = 0;
		bh = fir_head;			// ������ �������������� ����� ������
		bt = fir_tail;			// ������� �� ������ ��������������� ������
		n = 0;
		do
		{	
			// ���������� Q
			++ n;
			++ bh;
			-- bt;
			// ��������� I
			// 1_23 * 1_31 = 1_54. ��� ����� ��������� �� FIRCoef.
			rv.IV += k [n ++] * (x [bh ++].IV + x [-- bt].IV);
		}
		while (n < NtapHalf);
	}
    return rv;
}

// ������ ������������ ������� �����������. ����������� ��� multiplier 0 1 0 -1.
// ���������� ���������� �� ���, � � ����������� �� useI
static RAMFUNC_NONILINE FLOAT32P_t filter_fir4_tx_SSB_IQ(FLOAT32P_t NewSample, uint_fast8_t useI) 
{
	const FLOAT_t * const k = FIRCoef_tx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_tx_SSB_IQ, NtapHalf = Ntap / 2 };
	// ����� � ������������ ���������� �������
	static FLOAT32P_t x [Ntap * 2] = { { { 0, 0, }, }, }; // input samples (force CCM allocation)
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	// fir_head -  ������ �������������� ����� ������
	// fir_head + Ntap -  ������� �� ������ ��������������� �����
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);

	// ���� ������� == 0 - ��� � ��� ����� ���� ����������
	uint_fast16_t bh = fir_head;			// ������ �������������� ����� ������
	uint_fast16_t bt = fir_head + Ntap;	// ������� �� ������ ��������������� ������
    // Calculate the new output
    x [bh] = NewSample;
    x [bt] = NewSample;
	uint_fast16_t n = 0;
	FLOAT32P_t v = { { 0, 0 }, };
	if (useI != 0)
	{
		// �� ������ ���������� ������ I
		do
		{	
			v.IV += k [n ++] * (x [bh ++].IV + x [-- bt].IV);
			v.IV += k [n ++] * (x [bh ++].IV + x [-- bt].IV);
		}
		while (n < NtapHalf);
		// ������� � �������� ������
		v.IV += k [NtapHalf] * x [bh].IV;
	}
	else
	{
		// �� ������ ���������� ������ Q
		do
		{	
			v.QV += k [n ++] * (x [bh ++].QV + x [-- bt].QV);
			v.QV += k [n ++] * (x [bh ++].QV + x [-- bt].QV);
		}
		while (n < NtapHalf);
		// ������� � �������� ������
		v.QV += k [NtapHalf] * x [bh].QV;
	}

    return v;
}
#endif /* WITHDSPEXTDDC */

#endif /* WITHDSPLOCALFIR */

/* ���������� ��������� � ������������ FIR ������� */
static RAMFUNC FLOAT_t filter_fir_compute(const FLOAT_t * const pk0, const FLOAT_t * xbh, uint_fast16_t n)
{
	const FLOAT_t * xbt = xbh;		// ������� ������ �� ������
    // Calculate the new output
	// ������� � �������� ������
	FLOAT_t v = pk0 [-- n] * * -- xbh;             // sample at middle of buffer
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	float32x4_t v4 = vdupq_n_f32(0);
#endif /* __ARM_NEON_FP */
	do
	{	
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
		// ������� �������� �� ����� ��������
		xbh -= 4;
		const float32x4_t vh = vld1q_f32(xbh);
		// �������� �������� �� ������ �������� (��������� ���������� ������� �� ���������������)
		const float32x4_t vt = vld1q_f32(xbt);
		xbt += 4;
		/* ��������� ����� � ������ */
		const float32x4_t va = vaddq_f32(
			vrev64q_f32(vcombine_f32(vget_high_f32(vt), vget_low_f32(vt))), // ������ ������� �� ���������������
			vh);
		/* ������������ */
		n -= 4;
		const float32x4_t vk = vld1q_f32(& pk0 [n]);
		/* ��������� � ����������� */
		v4 = vmlaq_f32(v4, va, vk);
#elif defined (FMAF)
		v = FMAF(pk0 [-- n], * -- xbh + * xbt ++, v);	/* ���������� ���������� ��� ������ �������� ��-�� ���������� ����� ��������� */
#else /* defined (FMAF) */
		v += pk0 [-- n] * (* -- xbh + * xbt ++);
#endif /* defined (FMAF) */
	}
	while (n != 0);
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	v += vgetq_lane_f32(v4, 0);
	v += vgetq_lane_f32(v4, 1);
	v += vgetq_lane_f32(v4, 2);
	v += vgetq_lane_f32(v4, 3);
#endif /* __ARM_NEON_FP */

	return v;
}

#if WITHUSEDUALWATCH && DUALFILTERSPROCESSING

/* ���������� ��������� � ������������ FIR ������� */
// ���������� ��� �������� ���������� ��������
static RAMFUNC_NONILINE FLOAT32P_t filter_fir_compute_Pair(const FLOAT_t * const pk0, const FLOAT32P_t * xbh, uint_fast16_t n)
{
	const FLOAT32P_t * xbt = xbh;		// ������� ������ �� ������
    // Calculate the new output
	// ������� � �������� ������
	const FLOAT32P_t t1 = * -- xbh;
	const FLOAT_t k = pk0 [-- n];
	FLOAT32P_t v = { { k * t1.IV, k * t1.QV } };             // sample at middle of buffer
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	float32x4_t v4 = vdupq_n_f32(0);	// lane 0, 2, IV values, lane 1, 3: QV values
#endif /* __ARM_NEON_FP */
	do
	{	
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
		// ������� �������� �� ����� ��������
		xbh -= 2;
		const float32x4_t vh = vcombine_f32(vld1_f32(xbh [0].ivqv), vld1_f32(xbh [1].ivqv));	// lane 0, 2, IV values, lane 1, 3: QV values
		// ������� �������� �� ������ �������� (� �������� ������� - �������� � ��� �� �������, ��� � ������������)
		const float32x4_t vt = vcombine_f32(vld1_f32(xbt [1].ivqv), vld1_f32(xbt [0].ivqv));	// lane 0, 2, IV values, lane 1, 3: QV values
		xbt += 2;
		/* ��������� ����� � ������ */
		const float32x4_t va = vaddq_f32(
			vt,
			vh);
		/* ������������ */
		n -= 2;
		const float32x4_t vk = vcombine_f32(vdup_n_f32(pk0 [n + 0]), vdup_n_f32(pk0 [n + 1]));	// lane 0, 2: IV values, lane 1, 3: QV values
		/* ��������� � ����������� */
		v4 = vmlaq_f32(v4, va, vk);
#elif defined (FMAF)
		const FLOAT32P_t t1 = * -- xbh;
		const FLOAT32P_t t2 = * xbt ++;
		const FLOAT_t k = pk0 [-- n];
		v.IV = FMAF(k, t1.IV + t2.IV, v.IV);	/* ���������� ���������� ��� ������ �������� ��-�� ���������� ����� ��������� */
		v.QV = FMAF(k, t1.QV + t2.QV, v.QV);	/* ���������� ���������� ��� ������ �������� ��-�� ���������� ����� ��������� */
#else /* defined (FMAF) */
		const FLOAT32P_t t1 = * -- xbh;
		const FLOAT32P_t t2 = * xbt ++;
		const FLOAT_t k = pk0 [-- n];
		v.IV += k * (t1.IV + t2.IV);
		v.QV += k * (t1.QV + t2.QV);
#endif /* defined (FMAF) */
	}
	while (n != 0);
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	v.IV += vgetq_lane_f32(v4, 0);	// IV values
	v.IV += vgetq_lane_f32(v4, 2);	// IV values
	v.QV += vgetq_lane_f32(v4, 1);	// QV values
	v.QV += vgetq_lane_f32(v4, 3);	// QV values
#endif /* __ARM_NEON_FP */

	return v;
}


/* ���������� ��������� � ������������ FIR ������� */
// ���������� ��� �������� ������� ���������
static RAMFUNC_NONILINE FLOAT32P_t filter_fir_compute_Pair2(const FLOAT_t * const pk0, const FLOAT_t * const pk1, const FLOAT32P_t * xbh, uint_fast16_t n)
{
	const FLOAT32P_t * xbt = xbh;		// ������� ������ �� ������
    // Calculate the new output
	// ������� � �������� ������
	const FLOAT32P_t t1 = * -- xbh;
	const FLOAT_t k0 = pk0 [-- n];
	const FLOAT_t k1 = pk1 [n];
	FLOAT32P_t v = { { k0 * t1.IV, k1 * t1.QV } };             // sample at middle of buffer
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	float32x4_t v4 = vdupq_n_f32(0);	// lane 0, 2, IV values, lane 1, 3: QV values
#endif /* __ARM_NEON_FP */
	do
	{	
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
		// ������� �������� �� ����� ��������
		xbh -= 2;
		const float32x4_t vh = vcombine_f32(vld1_f32(xbh [0].ivqv), vld1_f32(xbh [1].ivqv));	// lane 0, 2, IV values, lane 1, 3: QV values
		// ������� �������� �� ������ �������� (� �������� ������� - �������� � ��� �� �������, ��� � ������������)
		const float32x4_t vt = vcombine_f32(vld1_f32(xbt [1].ivqv), vld1_f32(xbt [0].ivqv));	// lane 0, 2, IV values, lane 1, 3: QV values
		xbt += 2;
		/* ��������� ����� � ������ */
		const float32x4_t va = vaddq_f32(
			vt,
			vh);
		/* ������������ */
		n -= 2;
		const float32_t vks [4] = { pk0 [n + 0], pk1 [n + 0], pk0 [n + 1], pk1 [n + 1] };
		//const float32x4_t vk = vld1q_f32(vls);
		/* ��������� � ����������� */
		v4 = vmlaq_f32(v4, va, vld1q_f32(vks));
#elif defined (FMAF)
		const FLOAT32P_t t1 = * -- xbh;
		const FLOAT32P_t t2 = * xbt ++;
		const FLOAT_t k0 = pk0 [-- n];
		const FLOAT_t k1 = pk1 [n];
		v.IV = FMAF(k0, t1.IV + t2.IV, v.IV);	/* ���������� ���������� ��� ������ �������� ��-�� ���������� ����� ��������� */
		v.QV = FMAF(k1, t1.QV + t2.QV, v.QV);	/* ���������� ���������� ��� ������ �������� ��-�� ���������� ����� ��������� */
#else /* defined (FMAF) */
		const FLOAT32P_t t1 = * -- xbh;
		const FLOAT32P_t t2 = * xbt ++;
		const FLOAT_t k0 = pk0 [-- n];
		const FLOAT_t k1 = pk1 [n];
		v.IV += k0 * (t1.IV + t2.IV);
		v.QV += k1 * (t1.QV + t2.QV);
#endif /* defined (FMAF) */
	}
	while (n != 0);
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	v.IV += vgetq_lane_f32(v4, 0);	// IV values
	v.IV += vgetq_lane_f32(v4, 2);	// IV values
	v.QV += vgetq_lane_f32(v4, 1);	// QV values
	v.QV += vgetq_lane_f32(v4, 3);	// QV values
#endif /* __ARM_NEON_FP */

	return v;
}

#endif /* DUALFILTERSPROCESSING */

// ������ ��������� �����������
static RAMFUNC_NONILINE FLOAT_t filter_fir_tx_MIKE(FLOAT_t NewSample, uint_fast8_t bypass) 
{
	enum { Ntap = Ntap_tx_MIKE, NtapHalf = Ntap / 2 };
	// ����� � ������������ ���������� �������
	static FLOAT_t xshift [Ntap * 2] = { 0, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return bypass ? xshift [fir_head + NtapHalf] : filter_fir_compute(FIRCoef_tx_MIKE [gwprof], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}

// �������� ������ ��������.
static RAMFUNC_NONILINE FLOAT_t filter_fir_rx_AUDIO_A(FLOAT_t NewSample) 
{
	enum { Ntap = Ntap_rx_AUDIO, NtapHalf = Ntap / 2 };
	// ����� � ������������ ���������� �������
	static FLOAT_t xshift [Ntap * 2] = { 0, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return filter_fir_compute(FIRCoef_rx_AUDIO [gwprof] [0], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}

#if WITHUSEDUALWATCH

// �������� ������ ��������.
static RAMFUNC_NONILINE FLOAT_t filter_fir_rx_AUDIO_B(FLOAT_t NewSample) 
{
#if ! DUALFILTERSPROCESSING
	return NewSample;
#endif /* ! DUALFILTERSPROCESSING */

	enum { Ntap = Ntap_rx_AUDIO, NtapHalf = Ntap / 2 };
	// ����� � ������������ ���������� �������
	static FLOAT_t xshift [Ntap * 2] = { 0, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return filter_fir_compute(FIRCoef_rx_AUDIO [gwprof] [1], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}

#if DUALFILTERSPROCESSING
// �������� ������ ��������.
// ���������� ��� �������� ���������� ��������
static RAMFUNC_NONILINE FLOAT32P_t filter_fir_rx_AUDIO_Pair(FLOAT32P_t NewSample) 
{
	enum { Ntap = Ntap_rx_AUDIO, NtapHalf = Ntap / 2 };
	// ����� � ������������ ���������� �������
	static FLOAT32P_t xshift [Ntap * 2] = { 0, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return filter_fir_compute_Pair(FIRCoef_rx_AUDIO [gwprof] [0], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}

// �������� ������ ��������.
// ���������� ��� �������� ������� ���������
static RAMFUNC_NONILINE FLOAT32P_t filter_fir_rx_AUDIO_Pair2(FLOAT32P_t NewSample) 
{
	enum { Ntap = Ntap_rx_AUDIO, NtapHalf = Ntap / 2 };
	// ����� � ������������ ���������� �������
	static FLOAT32P_t xshift [Ntap * 2] = { 0, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return filter_fir_compute_Pair2(FIRCoef_rx_AUDIO [gwprof] [0], FIRCoef_rx_AUDIO [gwprof] [1], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}
#endif /* DUALFILTERSPROCESSING */

#endif /* WITHUSEDUALWATCH */


static RAMFUNC uint_fast8_t isdspmoderx(uint_fast8_t dspmode)
{
	return dspmode > DSPCTL_MODE_RX_BASE && dspmode < DSPCTL_MODE_RX_TOP;
}

static RAMFUNC uint_fast8_t isdspmodetx(uint_fast8_t dspmode)
{
	return dspmode > DSPCTL_MODE_TX_BASE && dspmode < DSPCTL_MODE_TX_TOP;
}

static int inside(int low, int val, int high)
{
	if (low > val)
		return low;
	if (high < val)
		return high;
	return val;
}

static int_fast16_t audio_validatebw6(int_fast16_t n)
{
	const int_fast16_t bw6limit = ARMSAIRATE - 2;
	return (n == INT16_MAX || n < bw6limit) ? n : INT16_MAX;
}

// ��������� ���������� ������ ��������
static void audio_setup_wiver(const uint_fast8_t spf, const uint_fast8_t pathi)
{
	const uint_fast8_t dspmode = glob_dspmodes [pathi];
	const uint_fast16_t fullbw6 = audio_validatebw6(glob_fullbw6 [pathi]);
#if WITHDSPEXTDDC
	#if WITHDSPLOCALFIR
		const FLOAT_t rxfiltergain = 1;
		const FLOAT_t txfiltergain = 1;
	#endif /* WITHDSPLOCALFIR */
#else /* WITHDSPEXTDDC */
	const FLOAT_t rxfiltergain = 2;	// ��� IQ ������� ����� ��� - ��� ����������� 0.5 �������� ��-�� ������������� ����� ���.
	const FLOAT_t txfiltergain = 2;	// ��� IQ ������� ����� ��� - ��� ����������� 0.5 �������� ��-�� ������������� ����� ���.
#endif /* WITHDSPEXTDDC */

	const int cutfreq = fullbw6 / 2;
	//debug_printf_P(PSTR("audio_setup_wiver: fullbw6=%u\n"), (unsigned) fullbw6);

	if (fullbw6 == INT16_MAX)
	{
		//debug_printf_P(PSTR("audio_setup_wiver: construct bypass glob_fullbw6=%u\n"), (unsigned) glob_fullbw6);
	#if WITHDSPLOCALFIR
		if (isdspmoderx(dspmode))
			fir_design_passtrough(FIRCoef_rx_SSB_IQ [spf], Ntap_rx_SSB_IQ, rxfiltergain);
		else if (isdspmodetx(dspmode))
			fir_design_passtrough(FIRCoef_tx_SSB_IQ [spf], Ntap_tx_SSB_IQ, txfiltergain);
	#else /* WITHDSPLOCALFIR */
		(void) dspmode;
		fir_design_integers_passtrough(FIRCoef_trxi_IQ, Ntap_trxi_IQ, 1);
	#endif /* WITHDSPLOCALFIR */
	}
	else
	{
		//debug_printf_P(PSTR("audio_setup_wiver: construct filter glob_fullbw6=%u\n"), (unsigned) glob_fullbw6);
	#if WITHDSPLOCALFIR
		if (isdspmoderx(dspmode))
			fir_design_lowpass_freq_scaled(FIRCoef_rx_SSB_IQ [spf], FIRCwnd_rx_SSB_IQ, Ntap_rx_SSB_IQ, cutfreq, rxfiltergain);	// � ����������� ��������� ������ � ������������� ��������, � ���������� ����
		else if (isdspmodetx(dspmode))
			fir_design_lowpass_freq_scaled(FIRCoef_tx_SSB_IQ [spf], FIRCwnd_tx_SSB_IQ, Ntap_tx_SSB_IQ, cutfreq, txfiltergain);	// � ����������� ��������� ������ � ������������� ��������, � ���������� ����
	#else /* WITHDSPLOCALFIR */
		(void) dspmode;
		#if WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08)
			fir_design_integer_lowpass_scaledL(FIRCoef_trxi_IQ, FIRCwndL_trxi_IQ, Ntap_trxi_IQ, cutfreq, 1);
		#else
			fir_design_integer_lowpass_scaled(FIRCoef_trxi_IQ, FIRCwnd_trxi_IQ, Ntap_trxi_IQ, cutfreq, 1);
		#endif
	#endif /* WITHDSPLOCALFIR */
	}

#if WITHDSPEXTFIR && WITHDSPLOCALFIR
	// ���� ���� � ������� � ���������� ������ - ������� ���������� � ����� passtrough - ��� ������������
	fir_design_integers_passtrough(FIRCoef_trxi_IQ, Ntap_trxi_IQ, 1);
#endif /* WITHDSPEXTDDC && WITHDSPLOCALFIR */

#if WITHDSPEXTDDC && WITHDSPEXTFIR
	// �������� ������������� ������� � FPGA
	//writecoefs(FIRCoef_trxi_IQ, Ntap_trxi_IQ);	/* ������ ������������� ������� */
	board_fpga_fir_send(pathi, FIRCoef_trxi_IQ, Ntap_trxi_IQ, HARDWARE_COEFWIDTH);		/* ��������� ������ ������������� � FPGA */
	boart_tgl_firprofile(pathi);
#endif /* WITHDSPEXTDDC && WITHDSPEXTFIR */
}

// ��������� ���������� ������ ��������
static void audio_setup_rx(const uint_fast8_t spf, const uint_fast8_t pathi)
{
	const int cutfreqlow = glob_aflowcutrx [pathi];
	const int cutfreqhigh = glob_afhighcutrx [pathi];
	FLOAT_t * const dCoeff = FIRCoef_rx_AUDIO [spf] [pathi];
	const FLOAT_t * const dWindow = FIRCwnd_rx_AUDIO;
	enum { iCoefNum = Ntap_rx_AUDIO };

	switch (glob_dspmodes [0])
	{
	case DSPCTL_MODE_RX_DSB:
		// � ���� ������ ������ �� ������������
		//fir_design_passtrough(dCoeff, iCoefNum, 1);
		// ���
		fir_design_lowpass_freq(dCoeff, iCoefNum, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1);	// ������������ ������� ���
		break;

	case DSPCTL_MODE_RX_SAM:
		// ���
		//fir_design_lowpass_freq(dCoeff, iCoefNum, cutfreqhigh);
		fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1);	// ������������ ������� ���
		break;

	case DSPCTL_MODE_RX_WFM:
	case DSPCTL_MODE_RX_AM:
	case DSPCTL_MODE_RX_WIDE:
		// audio
		if (glob_notch_on != 0)
		{
			// ������� SSB �������
			//const int fssbL = cutfreqlow;
			//const int fssbH = cutfreqhigh;

			// ������� NOTCH �������
			const int fNotch = inside(cutfreqlow + glob_notch_width / 2, glob_notch_freq, cutfreqhigh - glob_notch_width / 2);
			const int fcutL = fNotch - glob_notch_width / 2;
			const int fcutH = fNotch + glob_notch_width / 2;
			if (0)
			{
				// ������������� Notch
				fir_design_bandstop(dCoeff, iCoefNum, fir_design_normfreq(fcutL), fir_design_normfreq(fcutH));
				fir_design_scale(dCoeff, iCoefNum, 1 / testgain_float_DC(dCoeff, iCoefNum));	// ��������������� ��� ��������������� �������
				fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 0);	// ������������ ������� ���, ��� ���������� ������� �������
			}
			else
			{
				FLOAT_t dC2 [NtapCoeffs(iCoefNum)];
				int i;
				// ������������� ��� ����������������� ��������� �������
				fir_design_bandpass_freq(dC2, iCoefNum, cutfreqlow, fcutL);	// �������������� ������ �����������
				fir_design_bandpass_freq(dCoeff, iCoefNum, fcutH, cutfreqhigh);	// ��������������� ������ �����������
				// ������������ �������������
				for (i = 0; i < NtapCoeffs(iCoefNum); ++ i)
					dCoeff [i] += dC2 [i];
				fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 0);	// ������������ ������� ���, ��� ���������� ������� �������
			}
		}
		else
		{
			fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
			fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1);	// ������������ ������� ���
		}
		break;

	case DSPCTL_MODE_RX_NARROW:
	case DSPCTL_MODE_RX_FREEDV:
		// audio
		fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1);	// ������������ ������� ���
		break;

	case DSPCTL_MODE_RX_DRM:
		// audio
		// � ���� ������ ������ �� ������������
		fir_design_passtrough(dCoeff, iCoefNum, 1);		// ������ ����� �� ������ �� ��������
		break;


	case DSPCTL_MODE_RX_NFM:
		// audio
		fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1);	// ������������ ������� ���
		NBTAU = 1 - MAKETAUIF((FLOAT_t) 0.1);
		break;

	// � ������ ��������
	default:
		break;
	}
}


// ��������� ���������� ������ �����������
static void audio_setup_mike(const uint_fast8_t spf)
{
	FLOAT_t * const dCoeff = FIRCoef_tx_MIKE [spf];
	const FLOAT_t * const dWindow = FIRCwnd_tx_MIKE;
	enum { iCoefNum = Ntap_tx_MIKE };

	switch (glob_dspmodes [0])
	{
	case DSPCTL_MODE_TX_BPSK:
		// 15-�� �������� ��� ����� ����������� - �� "�������������" �������� ��������� � ������ � QEX July/Aug 1999 7 - x9907003.pdf
		fir_design_lowpass_freq(dCoeff, iCoefNum, 15);
		fir_design_scale(dCoeff, iCoefNum, 1 / testgain_float_DC(dCoeff, iCoefNum));	// ��������������� ��� ��������������� �������
		fir_design_applaywindow(dCoeff, dWindow, iCoefNum);
		break;

	// ��������� �������
	case DSPCTL_MODE_TX_NFM:
	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_AM:
	case DSPCTL_MODE_TX_FREEDV:
		fir_design_bandpass_freq(dCoeff, iCoefNum, glob_aflowcuttx, glob_afhighcuttx);
		fir_design_adjust_tx(dCoeff, dWindow, iCoefNum);	// ���������� ����������� � ���������
		break;

	// � ������ ������ ��� � ������� �������� ��� ��������� - ������ �� ������
	default:
		fir_design_passtrough(dCoeff, iCoefNum, 1);
		break;
	}
}

#if WITHCPUDACHW

	//#define HARDWARE_DACBITS 12	/* ��� �������� � 12-������� ���������� */

	// AGC dac constans
	enum
	{	
		dacwidth = HARDWARE_DACBITS,
		dacFScode = (1U << dacwidth) - 1,
		dacrefvoltage = DACVREF_CPU * 100,		// 3.3 volt - DAC reference voltage
	};

	enum
	{	
	#if (CTLSTYLE_RAVENDSP_V1 || CTLSTYLE_DSPV1A)

		dacXagchighvotage = 2950,	// 2.9 volt - AD605 VGN max
		dacXagclowvoltage = 100,		// 0.1 volt - AD605 VGN min

		dac_agc_highcode = dacFScode * dacXagchighvotage / dacrefvoltage,
		dac_agc_lowcode = dacFScode * dacXagclowvoltage / dacrefvoltage,
		dac_agc_coderange = dac_agc_highcode - dac_agc_lowcode,

		// ��������
		//dacXagchighvotage = 3300,	// 0.1..1.25 volt - AD9744 REFERENCE INPUT range (after 18k/10k chain).
		//dacXagclowvoltage = 280,

		dac_dacfs_highcode = dacFScode * dacXagchighvotage / dacrefvoltage,
		dac_dacfs_lowcode = dacFScode * dacXagclowvoltage / dacrefvoltage,
		dac_dacfs_coderange = dac_dacfs_highcode - dac_dacfs_lowcode

	#elif \
		CTLSTYLE_RAVENDSP_V3 || \
		CTLSTYLE_RAVENDSP_V4 || \
		CTLSTYLE_RAVENDSP_V5 || \
		CTLSTYLE_RAVENDSP_V6 || \
		CTLSTYLE_RAVENDSP_V7 || \
		CTLSTYLE_RAVENDSP_V8 || \
		CTLSTYLE_STORCH_V2 || \
		CTLSTYLE_STORCH_V3 || \
		CTLSTYLE_STORCH_V4 || \
		CTLSTYLE_STORCH_V5 || \
		CTLSTYLE_STORCH_V6 || \
		CTLSTYLE_STORCH_V7 || \
		CTLSTYLE_OLEG4Z_V1 || \
		0

		dacXagchighvotage = 3300,	// 0.1..1.25 volt - AD9744 REFERENCE INPUT range (after 18k/10k chain).
		dacXagclowvoltage = 280,

		dac_dacfs_highcode = dacFScode * dacXagchighvotage / dacrefvoltage,
		dac_dacfs_lowcode = dacFScode * dacXagclowvoltage / dacrefvoltage,
		dac_dacfs_coderange = dac_dacfs_highcode - dac_dacfs_lowcode

	#endif

	};
#endif /* WITHCPUDACHW */

#if 0 && WITHDACOUTDSPAGC

static void setagcattenuation(long code, uint_fast8_t tx)	// � ����� ���
{
	if (tx != 0)
		HARDWARE_DAC_AGC(0);
	else
		HARDWARE_DAC_AGC(dac_agc_highcode - ((code > dac_agc_coderange) ? dac_agc_coderange : code));
}

static void setlevelindicator(long code)	// � ����� ���
{
	hardware_dac_ch2_setvalue((dacFScode - 1) > code ? (dacFScode - 1) : code);
}


#endif /* WITHDACOUTDSPAGC */

// ���������� ������� ����� ������ ��
// ���������� �� ���������������� ���������, �� ����� ���� ������� � �� ������������� DSP - ���������� �� updateboard.
static void audio_update(const uint_fast8_t spf, uint_fast8_t pathi)
{
	globDSPMode  [spf] [pathi] = glob_dspmodes [pathi];
	//debug_printf_P(PSTR("nco_setlo_ftw: freq=%d\n"), glob_lo6A);
	const ncoftw_t lo6_ftw = FTWAF(- glob_lo6 [pathi]);
	nco_setlo_ftw(lo6_ftw, pathi);	// ������� #0

#if WITHLOOPBACKTEST
	fir_design_bandpass_freq(FIRCoef_tx_MIKE [spf], Ntap_tx_MIKE, glob_aflowcuttx, glob_afhighcuttx);
	fir_design_applaywindow(FIRCoef_tx_MIKE [spf], FIRCwnd_tx_MIKE, Ntap_tx_MIKE);
#endif /* WITHLOOPBACKTEST */


	// ������ ������ �������� ������ � ������ ����� (������������� ������� ������).
	audio_setup_wiver(spf, pathi);	/* ��������� ���������� ��� � ������ ��������� ������� �������� ������ */

	audio_setup_rx(spf, pathi);

	debug_cleardtmax();		// ����� ������������� �������� � ����� ������������������ DSP

#if 0
	debug_printf_P(PSTR("audio_update[pathi=%d]: dsp_mode=%d, bw6=%d, lo6=%d, rx=%d..%d, tx=%d..%d\n"), 
		(int) pathi, 
		(int) glob_dspmodes [pathi], 
		(int) glob_fullbw6 [pathi], 
		(int) glob_lo6 [pathi], 
		glob_aflowcutrx [pathi], glob_afhighcutrx [pathi], 
		glob_aflowcuttx, glob_afhighcuttx
		);
#endif
}

#if WITHMODEM

// RX
// ---------

static uint_fast32_t g_TxBitFreqFTW = 0;


// ���������� ��-0 ������ 32 �� (31.25 ��) - ���������� � �������� ARMI2SRATE
static int bpsk31_phase_tick(void)
{
	static uint_fast32_t bps31_tx_bitrateNCO;
	auto uint_fast32_t old = bps31_tx_bitrateNCO;
	return ((bps31_tx_bitrateNCO += g_TxBitFreqFTW) < old);
}

// return 0/1 for 0/PI
static 
uint_fast8_t 
pbsk_get_phase(
	uint_fast8_t suspend	// ���������� ������ ��� ���� - �� ��������� ��������� ������������ ���������
	)
{
	static uint_fast8_t phase;

	if (bpsk31_phase_tick() && modem_getnextbit(suspend))
	{
		phase = ! phase;
	}
	return phase;
}

static volatile uint_fast8_t	glob_modemmode;		// ����������� ��������� (bpsk/qpsk)
static volatile uint_fast32_t	glob_modemspeed100;	// �������� �������� � ��������� 1/100 ���

static uint16_t m_RxBitPhase;
static uint16_t g_RxBitFreqFTW;

// ����������� BPSK
static void demod_bpsk2_symbol(int_fast32_t i, int_fast32_t q, int level)
{
	static int_fast32_t oldi, oldq;
	const int_fast64_t dot = (int_fast64_t) oldi * i + (int_fast64_t) oldq * q;
	const int_fast8_t bitv = ! (dot > 0);
	modem_frames_decode(bitv);

	oldi = i;
	oldq = q;
}




static void demod_bpsk(int_fast32_t RxSin, int_fast32_t RxCos)
{

	enum 
	{
		BITFILTERLENGTH = 16,	// �� �������
	};

	typedef long SAMPLEHOLDER_T;
		//saved values for bit synchronization filter taps
	static SAMPLEHOLDER_T m_RxAmpFil [BITFILTERLENGTH];

	{	
		const SAMPLEHOLDER_T ISum = RxSin;
		const SAMPLEHOLDER_T QSum = RxCos;

		int	level = 30;
		long ampl = (ISum >> 16) * (ISum >> 16) + (QSum >> 16) * (QSum >> 16);

		// �������� �� ��������� 2
		while (ampl > 0)
		{
			level += 1;
			ampl >>= 1;
		}

		//Select 1 of 16
		const int BitPhaseInt = m_RxBitPhase >> 12;
		ASSERT(BitPhaseInt < BITFILTERLENGTH);
		m_RxAmpFil [BitPhaseInt] = (SAMPLEHOLDER_T)level;

		int i;
		ampl = 0;
		for (i = 0; i < BITFILTERLENGTH/2; i += 1)
		{
			ampl += m_RxAmpFil[i] - m_RxAmpFil [i + (BITFILTERLENGTH/2)];
		}

		//The correction is the amplitude times a synchronization gain, which is empirical.
		const long BitPhaseCorrection = (long) (ampl * 4);

		long NextSymPhase;	//long enough to contain the 17th bit when 16 bit adds overflow
		NextSymPhase = (long)m_RxBitPhase + (long) g_RxBitFreqFTW - BitPhaseCorrection;
		m_RxBitPhase = (unsigned short)NextSymPhase;

		if (NextSymPhase > 0xFFFF)
		{
			//We're at the centre of the bit:  31.25 Hz
			demod_bpsk2_symbol(ISum, QSum, level);
		}
	}
}

/////////////////////////////
// ������������ ������� ������������
// ��������� � �������� ARMI2SRATE ����
// iq - ����������, ���������� �� �����������

static void modem_demod_iq(FLOAT32P_t iq)
{
	switch (glob_modemmode)
	{
	case 0:
		// BPSK
		demod_bpsk(iq.IV, iq.QV);
		break;
	case 1:
		// QPSK
		//demod_qpsk(iq.IV, iq.QV);
		break;
	}
}
// ������������ ������� ����������
// ��������� � �������� ARMI2SRATE ����
// ������ ��� ���������������� �������
static int modem_get_tx_b(
	uint_fast8_t suspend	// ���������� ������ ��� ���� - �� ��������� ��������� ������������ ���������
	)
{

	return pbsk_get_phase(suspend);	
}
// ������������ ������� ����������
// ��������� � �������� ARMI2SRATE ����
// ������ ��� ��������������� �������
static FLOAT32P_t modem_get_tx_iq(
	uint_fast8_t suspend	// ���������� ������ ��� ���� - �� ��������� ��������� ������������ ���������
	)
{
	switch (glob_modemmode)
	{
	case 0:
		// BPSK
		{
			const int_fast32_t ph = (1 - pbsk_get_phase(suspend) * 2);
			const FLOAT32P_t iq = { { ph, ph } };
			return iq;
		}
	case 1:
		// QPSK
		{
			const FLOAT32P_t iq = { { 1, 0 } };
			return iq;
		}
	default:
		{
			const FLOAT32P_t iq = { { 1, 0 } };
			return iq;
		}

	}
}


static void modem_set_tx_speed(uint_fast32_t speed100)
{
	// TX
	g_TxBitFreqFTW = ((uint_fast64_t) speed100 << 32) / (ARMI2SRATE100);
}

static void modem_set_rx_speed(uint_fast32_t speed100)
{
	// RX
	g_RxBitFreqFTW = (((uint_fast64_t) speed100 << 16) / (ARMI2SRATE100));
}


/* ���������� ��������, �������� � ��������� 1/100 ��� */
static void 
modem_set_speed(uint_fast32_t speed100)
{
	if (glob_modemspeed100 != speed100)
	{
		glob_modemspeed100 = speed100;

		modem_set_tx_speed(speed100);
		modem_set_rx_speed(speed100);
	}
}

/* ���������� ��������� ��� ������ */
void modem_set_mode(uint_fast8_t modemmode)
{
	if (glob_modemmode != modemmode)
	{
		glob_modemmode = modemmode;	
	}
}
#endif /* WITHMODEM */

static void modem_update(void)
{
#if WITHMODEM
	modem_set_speed(glob_modem_speed100);
	modem_set_mode(glob_modem_mode);

	debug_printf_P(PSTR("modem_update: modem_speed100=%d.%02d, modem_mode=%d\n"), (int) (glob_modem_speed100 / 100), (int) (glob_modem_speed100 % 100), (int) glob_modem_mode);
#endif /* WITHMODEM */
}
///////////////////////

static FLOAT_t agclogof10 = 1;
	
static void agc_state_initialize(volatile agcstate_t * st, const volatile agcparams_t * agcp)
{
	const FLOAT_t f0 = agcp->levelfence;
	const FLOAT_t m0 = agcp->mininput;
	const FLOAT_t siglevel = 0;
	const FLOAT_t level = FMINF(FMAXF(m0, siglevel), f0);	// �������� � ��������� �� 1 �� f
	const FLOAT_t ratio = (agcp->agcoff ? m0 : level) / f0;

	const FLOAT_t streingth_log = LOGF(ratio);
	const FLOAT_t caplevel = streingth_log;

	st->agcfastcap = caplevel;
	st->agcslowcap = caplevel;
}

// ��� ������ ������� performagc ��������� siglevel, ������� �������� �������� 
// ������������� ������� ������� �������. ����� ���� �������������
static RAMFUNC FLOAT_t agccalcstrength(const volatile agcparams_t * const agcp, FLOAT_t siglevel)
{
	const FLOAT_t f0 = agcp->levelfence;
	const FLOAT_t m0 = agcp->mininput;
	const FLOAT_t level = FMINF(FMAXF(m0, siglevel), f0);	// �������� � ��������� �� 1 �� levelfence
	const FLOAT_t ratio = (agcp->agcoff ? m0 : level) / f0;

	const FLOAT_t streingth_log = LOGF(ratio);
	return streingth_log;
}

// �� ��������������� � ������������ � ��������� ���������� ����������� ���������
// ���� ������� �������� ��������� �������� (� ����� ��������� ����������).
static RAMFUNC FLOAT_t agccalcgain(const volatile agcparams_t * const agcp, FLOAT_t streingth)
{
	const FLOAT_t gain0 = POWF((FLOAT_t) M_E, streingth * agcp->agcfactor);
	// ���������� "����������" ���
	// ���������� ������� �� glob_agcrate ������� ������ ��������������� 
	// ���������� ������ �������� �� 1 ��������
	const FLOAT_t gain = FMINF(agcp->gainlimit, gain0);
	return gain;
}

// �� ��������������� � ������������ � ��������� ���������� ����������� ���������
// ���� ������� �������� ������������� ������� (�����������������)
static RAMFUNC FLOAT_t agccalcstrengthlog10(const volatile agcparams_t * const agcp, FLOAT_t streingth)
{
	return streingth / agclogof10;	// ��� ���������������
}

static RAMFUNC FLOAT_t agc_getsiglevel(
	FLOAT32P_t sampleiq
	)
{
	const FLOAT_t sample1 = SQRTF((FLOAT_t) sampleiq.IV * sampleiq.IV + (FLOAT_t) sampleiq.QV * sampleiq.QV);
	const FLOAT_t sample2 = FMAXF(FABSF(sampleiq.IV), FABSF(sampleiq.QV));	// ������������ ��� �������, ��� ��� ���� - ��������� �������������� ������������.
	const FLOAT_t sample = FMAXF(sample1, sample2);

	//return sample2;
	return sample;
}
//
// ���������� ������� ������� ���
// ������������� ������� ��� ����, ����� ��������� ��� ���������� � �������� CCM ������
// ���������� ����������� �������� ������������ � ��������������� �������� �������������.

static volatile agcstate_t rxsmeterstate [NTRX] = { { 0, } };	// �� ������ �������
static volatile agcstate_t rxagcstate [NTRX] = { { 0, } };	// �� ������ �������
static volatile agcstate_t txagcstate = { 0, };

static volatile agcparams_t rxsmeterparams = { 0, };
static volatile agcparams_t rxagcparams [NPROF] [NTRX] = { { { 0, } } };
static volatile agcparams_t txagcparams [NPROF] = { { 0, } };
static volatile uint_fast8_t gwagcprofrx = 0;	// work profile - ������ ���������������� ����������, ����������� ��� ������ */
static volatile uint_fast8_t gwagcproftx = 0;	// work profile - ������ ���������������� ����������, ����������� ��� ������ */
//	
static void agc_initialize(void)
{
	// ��������� ���������� ��� ��������
	gwagcprofrx = 0;
	gwagcproftx = 0;

	agclogof10 = LOGF(10);

	uint_fast8_t profile;
	for (profile = 0; profile < NPROF; ++ profile)
	{
		uint_fast8_t pathi;
		for (pathi = 0; pathi < NTRX; ++ pathi)
		{

			agc_parameters_initialize(& rxagcparams [profile] [pathi]);
			agc_state_initialize(& rxagcstate [pathi], & rxagcparams [profile] [pathi]);
			// s-meter
			agc_parameters_initialize(& rxsmeterparams);
			agc_state_initialize(& rxsmeterstate [pathi], & rxsmeterparams);
		}

		// �������� ������ � flatgain=1
		comp_parameters_initialize(& txagcparams [profile]);
		agc_state_initialize(& txagcstate, & txagcparams [profile]);
	}

#if WITHDSPEXTDDC

#elif WITHDACOUTDSPAGC
	setagcattenuation(0, 0);	// � ����� ��� ���������� ��������
	setlevelindicator(0);
#endif /* WITHDSPEXTDDC */
}

static RAMFUNC FLOAT32P_t
agc_delaysignal(
	FLOAT32P_t v, 
	uint_fast8_t pathi
	)
{
#if WITHLIMITEDAGCATTACK

	enum { DLY = NSAITICKS(5) };	// ������ ������������� �� ��������� ���������� mS

	static FLOAT32P_t data [NTRX] [DLY] = { { { 0, 0 }, } };
	static unsigned pos [NTRX] = { 0, };

	unsigned * const pp = & pos [pathi];
	unsigned posv = * pp;
	FLOAT32P_t * const pdata = & data [pathi] [posv];

	const FLOAT32P_t r = * pdata;	// �������� ����������� ������
	* pdata = v;					// ���������� ����� �����������
	* pp = (posv == 0) ? (DLY - 1) : (posv - 1);	// ����������� ���������� ���������� �������

	return r;

#else /* WITHLIMITEDAGCATTACK */

	return v;

#endif /* WITHLIMITEDAGCATTACK */
}

// ��� ����� ��� floaing-point ������
// ��������� ����������� ������ �������
static RAMFUNC FLOAT_t agc_measure_float(
	const uint_fast8_t dspmode, 
	FLOAT32P_t sampleiq, 
	uint_fast8_t pathi
	)
{
	BEGIN_STAMP3();

	const FLOAT_t siglevel0 = agc_getsiglevel(sampleiq);

	const volatile agcparams_t * const agcp = & rxagcparams [gwagcprofrx] [pathi];
	volatile agcstate_t * const st = & rxagcstate [pathi];
	//BEGIN_STAMP();
	const FLOAT_t strength = agccalcstrength(agcp, siglevel0);	// ��������� ���������������� �������� ������ �������
	//END_STAMP();

	// ����� S-�����
	performagc(& rxsmeterparams, & rxsmeterstate [pathi], strength);	// ���������� ������ �������

	//BEGIN_STAMP();
	performagc(agcp, st, strength);	// ���������� ������ �������
	//END_STAMP();

	END_STAMP3();
	return performagcresultslow(st);
}

// ��� ����� ��� floaing-point ������
// ��������� ��������
static RAMFUNC FLOAT_t agc_getgain_float(
	FLOAT_t fltstrengthslow,
	uint_fast8_t pathi
	)
{
	const volatile agcparams_t * const agcp = & rxagcparams [gwagcprofrx] [pathi];

	//BEGIN_STAMP();
	const FLOAT_t gain = agccalcgain(agcp, fltstrengthslow);
	//END_STAMP();

	return gain;
}

static FLOAT_t manualsquelch [NTRX] = { 0, };

static RAMFUNC int agc_squelchopen(
	FLOAT_t fltstrengthslow,
	uint_fast8_t pathi
	)
{
	return fltstrengthslow > manualsquelch [pathi];
}
// ������� ��� S-����� - ��������� ����������� ��������� ������ ������� �� FS
/* ���������� �� user-mode ��������� */
static void agc_reset(
	uint_fast8_t pathi
	)
{
	volatile agcparams_t * const agcp = & rxsmeterparams;
	volatile agcstate_t * const st = & rxsmeterstate [pathi];
	FLOAT_t m0 = agcp->mininput;
	FLOAT_t m1;
	global_disableIRQ();
	st->agcfastcap = m0;
	st->agcslowcap = m0;
	global_enableIRQ();
#if ! CTLSTYLE_V1D		// �� ����� STM32F429I-DISCO � ����������� STM32F429ZIT6 - �� ��� ������ ���
	for (;;)
	{
		local_delay_ms(1);
		global_disableIRQ();
		const FLOAT_t v = performagcresultslow(st);
		global_enableIRQ();
		if (v != m0)
		{
			m1 = v;
			break;
		}
	}
	for (;;)
	{
		local_delay_ms(1);
		global_disableIRQ();
		const FLOAT_t v = performagcresultslow(st);
		global_enableIRQ();
		if (v != m1)
			break;
	}
#endif
}

// ������� ��� S-����� - ��������� ����������� ��������� ������ ������� �� FS
/* ���������� �� user-mode ��������� */
static FLOAT_t agc_forvard_getstreigthlog10(
	FLOAT_t * tracemax,
	uint_fast8_t pathi
	)
{
	volatile agcparams_t * const agcp = & rxsmeterparams;
	volatile const agcstate_t * const st = & rxsmeterstate [pathi];

	const FLOAT_t fltstrengthfast = performagcresultfast(st);	// ���������� ������ �������
	const FLOAT_t fltstrengthslow = performagcresultslow(st);	// ���������� ������ �������
	* tracemax = agccalcstrengthlog10(agcp, fltstrengthslow);
	return agccalcstrengthlog10(agcp, fltstrengthfast);
}

/* �������� �������� ������ ������� � ���������, �������� �� upper */
/* -73.01dBm == 50 uV rms == S9 */
/* ���������� �� user-mode ��������� */
uint_fast8_t 
dsp_getsmeter(uint_fast8_t * tracemax, uint_fast8_t lower, uint_fast8_t upper, uint_fast8_t clean)
{
	const uint_fast8_t pathi = 0;	// �����, ����������� ��� ������ s-�����
	//if (clean != 0)
	//	agc_reset(pathi);
	FLOAT_t tmaxf;
	int level = upper + (int) (agc_forvard_getstreigthlog10(& tmaxf, pathi) * 200 + (glob_fsadcpower10 + 5)) / 10;	// �������������� � �������� � ��������� �� ������ ������� �����
	int tmax = upper + (int) (tmaxf * 200 + (glob_fsadcpower10 + 5)) / 10;

	if (tmax > (int) upper)
		tmax = upper;
	else if (tmax < (int) lower)
		tmax = lower;

	if (level > (int) upper)
		level = upper;
	else if (level < (int) lower)
		level = lower;

	* tracemax = tmax;
	return level;
}

static FLOAT_t mickecliplevelp [NPROF] = { + INT_MAX, + INT_MAX };
static FLOAT_t mickeclipleveln [NPROF] = { - INT_MAX, - INT_MAX };
static FLOAT_t mickeclipscale [NPROF] = { 1, 1 };

// ��� � ���������� ���������
static RAMFUNC FLOAT_t txmikeagc(FLOAT_t vi)
{
	volatile agcparams_t * const agcp = & txagcparams [gwagcproftx];
	if (agcp->agcoff == 0)
	{
		const FLOAT_t siglevel0 = FABSF(vi);
		volatile agcstate_t * const st = & txagcstate;

		performagc(agcp, st, agccalcstrength(agcp, siglevel0));	// ���������� ������ �������
		const FLOAT_t gain = agccalcgain(agcp, performagcresultslow(st));
		vi *= gain;
	}

	{
		// ������������
		const FLOAT_t levelp = mickecliplevelp [gwagcproftx];
		const FLOAT_t leveln = mickeclipleveln [gwagcproftx];
		if (vi > levelp)	
			vi = levelp;
		else if (vi < leveln)
			vi = leveln;
		vi *= mickeclipscale [gwagcproftx];
	}

	return vi;
}


void debug_showcompstate(void)
{
	const volatile agcstate_t * const st = & txagcstate;
	char s [128];

	snprintf(s, sizeof s / sizeof s [0], 
		"agcfastcap=%f, agchangticks=%u, agcslowcap=%f\n", 
		st->agcfastcap, st->agchangticks, st->agcslowcap);
	dbg_puts_impl(s);
}

// agc ---

/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: High Pass
Filter model: Chebyshev
Filter order: 4
Sampling Frequency: 48 KHz
Cut Frequency: 7.500000 KHz
Pass band Ripple: 1.000000 dB
Coefficents Quantization: float

Z domain Zeros
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000

Z domain Poles
z = 0.639035 + j -0.302403
z = 0.639035 + j 0.302403
z = 0.501849 + j -0.734622
z = 0.501849 + j 0.734622
***************************************************************/
static RAMFUNC float iir_nfmnbbpf(FLOAT_t NewSample) {
	enum { NCoef = 4 };
   static const FLOAT_t ACoef[NCoef+1] = {
        0.48404079901937469000,
        -1.93616319607749880000,
        2.90424479411624810000,
        -1.93616319607749880000,
        0.48404079901937469000
    };

    static const FLOAT_t BCoef[NCoef+1] = {
        1.00000000000000000000,
        -2.28176832599058790000,
        2.57413221290498310000,
        -1.51328228347904200000,
        0.39561310580923453000
    };

	static uint_fast8_t iir_stage = 0;
    static FLOAT_t y[2 * (NCoef+1)] = { 0 }; //output samples
    static FLOAT_t x[2 * (NCoef+1)] = { 0 }; //input samples
    int n;

	// shift the old samples
	iir_stage = (iir_stage == 0) ? (NCoef) : (iir_stage - 1);
    //shift the old samples
    //for(n=NCoef; n>0; n--) {
    //   x[n] = x[n-1];
    //   y[n] = y[n-1];
    //}

    //Calculate the new output
    x [iir_stage] = x [iir_stage + (NCoef+1)] = NewSample;
    y [iir_stage] = y [iir_stage + (NCoef+1)] = ACoef[0] * NewSample;

    for (n = 1; n <= NCoef; n ++)
        y [iir_stage + 0] += ACoef [n] * x [iir_stage + n] - BCoef [n] * y [iir_stage + n];
    
    return y [iir_stage + 0];
// original
    //shift the old samples
    for(n=NCoef; n>0; n--) {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    //Calculate the new output
    x[0] = NewSample;
    y[0] = ACoef[0] * x[0];
    for(n=1; n<=NCoef; n++)
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
    
    return y[0];


}

/* �������� 0..1 */

static FLOAT_t sidetonevolume = 0; //(glob_sidetonelevel / (FLOAT_t) 100);
static FLOAT_t mainvolumerx = 1; //1 - sidetonevolume;
static FLOAT_t subtonevolume = 0; //(glob_subtonelevel / (FLOAT_t) 100);
static FLOAT_t mainvolumetx = 1; //1 - subtonevolume;

// ����� �������� ������� � ���������, ���������� ��� ������
static RAMFUNC FLOAT_t injectsidetone(FLOAT_t v, FLOAT_t sdtn)
{

	return v * mainvolumerx + sdtn * sidetonevolume;
}

// ����� �������� ������� � ���������, ���������� ��� ������
static RAMFUNC FLOAT_t injectsubtone(FLOAT_t v, FLOAT_t ctcss)
{

	return v * mainvolumetx + ctcss * subtonevolume;
}

// ��������� ��������� ������ ����
static unsigned long local_random(unsigned long num)
{

	static unsigned long rand_val = 123456UL;

	if (rand_val & 0x80000000UL)
		rand_val = (rand_val << 1);
	else	
		rand_val = (rand_val << 1) ^ 0x201051UL;

	return (rand_val % num);
}

// return audio sample in range [- txlevelfence.. + txlevelfence]
static RAMFUNC FLOAT_t preparevi(
	int vi0,
	uint_fast8_t dspmode,
	FLOAT_t ctcss	// ������, audio sample in range [- txlevelfence.. + txlevelfence]
	)
{
	FLOAT_t vi0f = vi0;

	switch (dspmode)
	{
	case DSPCTL_MODE_TX_BPSK:
		return txlevelfenceBPSK;	// ���������� ������������ � ������������ �������

	case DSPCTL_MODE_TX_CW:
		return txlevelfenceHALF;	// ���������� ������������ � ������������ �������

	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_AM:
	case DSPCTL_MODE_TX_NFM:
	case DSPCTL_MODE_TX_FREEDV:
		// �������� ����������� � txlevelfenceSSB
		switch (glob_txaudio)
		{
		case BOARD_TXAUDIO_MIKE:
		case BOARD_TXAUDIO_LINE:
			// �������� - ��������
			// ������������� �������� ������������.
			// see glob_mik1level (0..100)
			return injectsubtone(txmikeagc(vi0f * txlevelfenceSSB / mikefence), ctcss); //* TXINSCALE; // �������� ������� - ��������

#if WITHUSBUAC
		case BOARD_TXAUDIO_USB:
#endif /* WITHUSBUAC */
		default:
			// �������� - LINE IN ��� USB
			// see glob_mik1level (0..100)
			return injectsubtone(vi0f * txlevelfenceSSB / mikefence, ctcss); //* TXINSCALE; // �������� ������� - ��������

		case BOARD_TXAUDIO_NOISE:
			// �������� - ���
			//vf = filter_fir_tx_MIKE((local_random(2UL * IFDACMAXVAL) - IFDACMAXVAL), 0);	// ���
			// return audio sample in range [- txlevelfence.. + txlevelfence]
			return injectsubtone((int) (local_random(2 * txlevelfenceSSB_INTEGER - 1) - txlevelfenceSSB_INTEGER), ctcss);	// ���

		case BOARD_TXAUDIO_2TONE:
			// �������� - ����������� ������
			// return audio sample in range [- txlevelfence.. + txlevelfence]
			return injectsubtone(get_dualtonefloat() * txlevelfenceSSB, ctcss);		// �������� ������� - ������������� ��������� ��� ���������

		case BOARD_TXAUDIO_1TONE:
			// �������� - �������������� ������
			// return audio sample in range [- txlevelfence.. + txlevelfence]
			return injectsubtone(get_singletonefloat() * txlevelfenceSSB, ctcss);

		case BOARD_TXAUDIO_MUTE:
			return injectsubtone(0, ctcss);
		}
	}
	// � ������ ����� ��� bypass ������ �� ������.
	return 0;
}

/* �������� I/Q ���� ��� �������� � up-converter */
static RAMFUNC FLOAT32P_t baseband_modulator(
	FLOAT_t vi,
	uint_fast8_t dspmode,
	FLOAT_t shape
	)
{
	const uint_fast8_t pathi = 0;	// �����, ����������� ��� ��������
	switch (dspmode)
	{
	default:
		{
			const FLOAT32P_t vfb = { { 0, 0 } };
			// � ������ ����� ������ �� ������.
			return vfb;
		}

#if WITHMODEM
	case DSPCTL_MODE_TX_BPSK:
		{
			const FLOAT32P_t vfb = scalepair(modem_get_tx_iq(getTxShapeNotComplete()), txlevelfenceBPSK * shape);
	#if WITHMODEMIQLOOPBACK
			modem_demod_iq(vfb);	// debug loopback
	#endif /* WITHMODEMIQLOOPBACK */
			return vfb;
		}
#endif /* WITHMODEM */

	case DSPCTL_MODE_TX_CW:
		{
			// vi - audio sample in range [- txlevelfence.. + txlevelfence]
			//const FLOAT32P_t vfb = scalepair_int32(get_int32_aflo_delta(0, pathi), vi * shape);
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(0, pathi), txlevelfenceCW * shape);
			return vfb;
		}

	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_FREEDV:
		{
#if WITHTXDACFULL
			shape *= (FLOAT_t) M_SQRT2;
#endif /* WITHTXDACFULL */
			// vi - audio sample in range [- txlevelfence.. + txlevelfence]
			//const FLOAT32P_t vfb = scalepair_int32(get_int32_aflo_delta(0, pathi), vi * shape);
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(0, pathi), vi * shape);
			return vfb;
		}
	
	case DSPCTL_MODE_TX_AM:
		{
			// vi - audio sample in range [- txlevelfenceSSB.. + txlevelfenceSSB]
			// input range: of vi: (- IFDACMAXVAL) .. (+ IFDACMAXVAL)
			const FLOAT_t peak = amcarrierHALF + vi * amshapesignalHALF;
			//const FLOAT32P_t vfb = scalepair_int32(get_int32_aflo_delta(0, pathi), peak * shape);
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(0, pathi), peak * shape);
			return vfb;
		}

	case DSPCTL_MODE_TX_NFM:
		{
			// vi - audio sample in range [- txlevelfence.. + txlevelfence]
			const long int deltaftw = (long) gnfmdeviationftw * vi / txlevelfenceSSB;	// ����������� ������������ ��������� �����
			//const FLOAT32P_t vfb = scalepair_int32(get_int32_aflo_delta(deltaftw, pathi), txlevelfenceHALF * shape);
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(deltaftw, pathi), txlevelfenceNFM * shape);
			return vfb;
		}
	}
}

// � ������ ������ LEFT JUSTIFIED

static RAMFUNC int_fast32_t iq2tx(int_fast32_t v)
{
	return v << (32 - WITHIFDACWIDTH);
}


#if WITHDSPEXTDDC


// ���������� - ������������ ������ ������ (���� I/Q).
// �������������� 16 ������ (WITHAFADCWIDTH) �����
// ������������ � ������ �������� DUC
static RAMFUNC void processafadcsampleiq(
	INT32P_t vi0,	// ������� � ��������� (� vi)
	uint_fast8_t dspmode,
	FLOAT_t shape,	// 0..1 - ���������
	FLOAT_t ctcss	// ������, audio sample in range [- txlevelfence.. + txlevelfence]
	)
{
	// vi - audio sample in range [- txlevelfence.. + txlevelfence]
	FLOAT_t vi = preparevi(vi0.IV, dspmode, ctcss);	// vi ���������� � ����������� ��������� ���
	if (isdspmodetx(dspmode))
	{
#if WITHMODEM
		if (dspmode == DSPCTL_MODE_TX_BPSK)
		{
			// ���������������� �����. �� ������ baseband_modulator � � �� modem_get_tx_iq ���� �� �������.
			const int txb = modem_get_tx_b(getTxShapeNotComplete());
	#if WITHMODEMIQLOOPBACK
			const int_fast32_t ph = (1 - txb * 2);
			const FLOAT32P_t iq = { { ph, ph } };
			modem_demod_iq(iq);	// debug loopback
	#endif /* WITHMODEMIQLOOPBACK */
			const int vv = txb ? 0 : - 1;	// txiq[63] ��������� ��������� ������� ����� ���
			savesampleout32stereo(vv, vv);
			return;
		}
#endif /* WITHMODEM */

		
		vi = filter_fir_tx_MIKE(vi, 0);
#if WITHDSPLOCALFIR
		const FLOAT32P_t vfb = filter_firp_tx_SSB_IQ(baseband_modulator(vi, dspmode, shape));
#else /* WITHDSPLOCALFIR */
		const FLOAT32P_t vfb = baseband_modulator(vi, dspmode, shape);
#endif /* WITHDSPLOCALFIR */
		savesampleout32stereo(iq2tx(vfb.IV), iq2tx(vfb.QV));	// ������ � ����� � ����������� I/Q ��������.
	}
	else
	{
		filter_fir_tx_MIKE(vi, 1);		// ������ �� �����������, ������ ����������� ����� � ����� ��������
		savesampleout32stereo(0, 0);
	}
}

#else /* WITHDSPEXTDDC */


// ��������
// �������������� 16 ������ (WITHAFADCWIDTH) �����
static RAMFUNC void processafadcsample(
	INT32P_t vi0,	// ������� � ��������� (� vi)
	uint_fast8_t dspmode,
	FLOAT_t shape,	// 0..1 - ���������
	FLOAT_t ctcss	// ������, audio sample in range [- txlevelfence.. + txlevelfence]
	)
{
#if WITHDSPLOCALFIR == 0
	#error WITHDSPLOCALFIR should be defined
#endif /* WITHDSPLOCALFIR == 0 */

	FLOAT_t vi = preparevi(vi0.IV, dspmode, ctcss);	// vi ���������� � ����������� ��������� ���
	// vi - audio sample in range [- txlevelfence.. + txlevelfence]
	if (isdspmodetx(dspmode))
	{
		vi = filter_fir_tx_MIKE(vi, 0);
		const FLOAT32P_t vfb = baseband_modulator(vi, dspmode, shape);
		// �����, ���� ������������ ������� vfb.IV � vfb.QV, 
		// ���������� Digital Up Conversion
		const FLOAT32P_t v_if = get_float4_iflo();	// ������� 12 ��� - 1/4 ������� ������� ��� - ����� ��������������� ������ ����������.
		const FLOAT32P_t e1 = filter_fir4_tx_SSB_IQ(vfb, v_if.IV != 0);		// 1.85 kHz - ������ ����� �������� 2.0
		const FLOAT_t r = (e1.QV * v_if.QV + e1.IV * v_if.IV);	// ��������� �� �������� ������� ("+" - ��� ��������).
		// ��������� � �� - ������������� ADC/DAC
		savesampleout32stereo(iq2tx(r * shape), 0);	// ����� �������� 24 ���� left justified � 32-� ������ �����.
	}
	else
	{
		filter_fir_tx_MIKE(vi, 1);		// ������ �� �����������, ������ ����������� ����� � ����� ��������
		savesampleout32stereo(0, 0);
	}
}

#endif /* WITHDSPEXTDDC */

//
// Todo: Use FLT_EPSILON (DBL_EPSILON)
// See also: https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
#if 0

// http://www.vcl.salk.edu/~micah/FM/FM.html micah@salk.edu
// To make the code faster you can change the ATAN2F to the following:

// Fast approximate arctan2 code posted by Jim Shima
static RAMFUNC FLOAT_t arctan2(FLOAT_t y, FLOAT_t x)
{
	FLOAT_t angle;
	//const float abs_y = y >= 0 ? y : - y;
	const FLOAT_t abs_y = FABSF(y);
	if (x >= 0)
	{
		const FLOAT_t r = (x - abs_y) / (x + abs_y + 1e-40f); // add 1e-40 to prevent 0/0 condition
		angle = (FLOAT_t) 0.1963 * r * r * r - (FLOAT_t) 0.9817 * r + (FLOAT_t) M_PI_4;
	} else {
		const float r = (x + abs_y) / (abs_y - x);
		angle = (FLOAT_t) 0.1963 * r * r * r - (FLOAT_t) 0.9817 * r + 3 * (FLOAT_t) M_PI_4;
	}
	if (y < 0) 
		return - angle;     // negate if in quad III or IV
	else 
		return angle;
}
#endif



//////////////////////////

// ����������� FM
static RAMFUNC ncoftwi_t demodulator_FM(
	FLOAT32P_t vp1,
	const uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
	// �����, ���� ������������ ������� vp1.IV � vp1.QV, �������� �����������
	//
	// tnx Vladimir Vassilevsky
	// http://www.dsprelated.com/showmessage/71491/2.php
	//
	static ncoftwi_t prev_fi [NTRX] = { 0, };

	if (vp1.IV == 0 && vp1.QV == 0)
		vp1.QV = 1;
	const ncoftwi_t fi = OMEGA2FTWI(ATAN2F(vp1.QV, vp1.IV));	//  returns a value in the range �pi to pi radians, using the signs of both parameters to determine the quadrant of the return value.

	const ncoftwi_t d_fi = (ncoftwi_t) (fi - prev_fi [pathi]);
	prev_fi [pathi] = fi;

	return d_fi;
}


enum { AMDSTAGES = 7, AMDOUT_IDX = (3 * AMDSTAGES) };


struct amd
{
	//int run;
	//int buff_size;					// buffer size
	//FLOAT_t *in_buff;					// pointer to input buffer
	//FLOAT_t *out_buff;				// pointer to output buffer
	//int mode;							// demodulation mode
	//FLOAT_t sample_rate;				// sample rate
	FLOAT_t dc;							// dc component in demodulated output
	ncoftwi_t omegai_min;					// pll - minimum lock check parameter
	ncoftwi_t omegai_max;					// pll - maximum lock check parameter
	ncoftwi_t phsi;						// pll - phase accumulator
	ncoftwi_t omegai;						// pll - locked pll frequency
	ncoftwi_t fil_outi;					// pll - filter output
	int64_t g1i, g2i;					// pll - filter gain parameters

	FLOAT_t mtauR;						// carrier removal multiplier
	FLOAT_t onem_mtauR;					// 1.0 - carrier_removal_multiplier
	FLOAT_t mtauI;						// carrier insertion multiplier
	FLOAT_t onem_mtauI;					// 1.0 - carrier_insertion_multiplier

	FLOAT_t a [3 * AMDSTAGES + 3];		// Filter a variables
	FLOAT_t b [3 * AMDSTAGES + 3];		// Filter b variables
	FLOAT_t c [3 * AMDSTAGES + 3];		// Filter c variables
	FLOAT_t d [3 * AMDSTAGES + 3];		// Filter d variables
	FLOAT_t c0 [AMDSTAGES];				// Filter coefficients - path 0
	FLOAT_t c1 [AMDSTAGES];				// Filter coefficients - path 1
	FLOAT_t dsI;						// delayed sample, I path
	FLOAT_t dsQ;						// delayed sample, Q path
	FLOAT_t dc_insert;					// dc component to insert in output
	int sbmode;						// sideband mode
	//int levelfade;					// Fade Leveler switch
};

static struct amd amds [NTRX] = { { 0 }, };	// force CCM allocation

/* �������� ���������� �� ������ ��������� � ������ SAM */
/* �������� �������� ���������� ������� � ��������� 0.1 ����� */
uint_fast8_t hamradio_get_samdelta10(int_fast32_t * p, uint_fast8_t pathi)
{
	const uint_fast32_t sample_rate10 = ARMSAIRATE * 10;

	* p = ((int_fast64_t) amds [pathi].omegai * sample_rate10) >> 32;
	return glob_dspmodes [pathi] == DSPCTL_MODE_RX_SAM;
}

static volatile int32_t saved_delta_fi [NTRX] = { 0, };	// force CCM allocation

/* �������� �������� ���������� ������� � ��������� 0.1 ����� */
uint_fast8_t dsp_getfreqdelta10(int_fast32_t * p, uint_fast8_t pathi)
{
	const uint_fast32_t sample_rate10 = ARMSAIRATE * 10;

	* p = ((int_fast64_t) saved_delta_fi [pathi] * sample_rate10) >> 32;
	return glob_dspmodes [pathi] == DSPCTL_MODE_RX_NFM;
}

static void init_amd(struct amd * a)
{
	a->phsi = 0;
	a->fil_outi = 0;
	a->omegai = 0;

	//fade leveler
	a->dc = 0;
	a->dc_insert = 0;

	//sideband separation
	a->c0 [0] = (FLOAT_t) -0.328201924180698;
	a->c0 [1] = (FLOAT_t) -0.744171491539427;
    a->c0 [2] = (FLOAT_t) -0.923022915444215;
    a->c0 [3] = (FLOAT_t) -0.978490468768238;
    a->c0 [4] = (FLOAT_t) -0.994128272402075;
    a->c0 [5] = (FLOAT_t) -0.998458978159551;
    a->c0 [6] = (FLOAT_t) -0.999790306259206;
   
    a->c1 [0] = (FLOAT_t) -0.0991227952747244;
    a->c1 [1] = (FLOAT_t) -0.565619728761389;
    a->c1 [2] = (FLOAT_t) -0.857467122550052;
    a->c1 [3] = (FLOAT_t) -0.959123933111275;
    a->c1 [4] = (FLOAT_t) -0.988739372718090;
    a->c1 [5] = (FLOAT_t) -0.996959189310611;
    a->c1 [6] = (FLOAT_t) -0.999282492800792;
}


static void 
create_amd(
	struct amd * a,
	//int run,
	//int mode,
	//int levelfade,
	int sbmode,
	//int sample_rate,
	int fmin,
	int fmax,
	FLOAT_t zeta,		// pll - damping factor; as coded, must be <=1.0
	FLOAT_t omegaN,		// pll - natural frequency
	FLOAT_t tauR,		// carrier removal time constant
	FLOAT_t tauI		// carrier insertion time constant
	)
{
	FLOAT_t g1, g2;						// pll - filter gain parameters
	FLOAT_t knorm = POWF(2, NCOFTWBITS);
	const FLOAT_t sample_rate = (FLOAT_t) ARMSAIRATE;
	//a->run = run;
	//a->mode = mode;
	//a->levelfade = levelfade;
	a->sbmode = sbmode;

	a->omegai_min = FTWAF(fmin);
	a->omegai_max = FTWAF(fmax);
	g1 = 1 - EXPF(- 2 * omegaN * zeta / sample_rate);
	g2 = - g1 + 2 * (1 - EXPF(- omegaN * zeta / sample_rate) * COSF(omegaN / sample_rate * SQRTF(1 - zeta * zeta)));

	a->g1i = g1 * knorm;	// 2^32
	a->g2i = g2 * knorm;	// 2^32

	// carrier removal
	a->mtauR = EXPF(- 1 / (sample_rate * tauR));
	a->onem_mtauR = 1 - a->mtauR;
	a->mtauI = EXPF(- 1 / (sample_rate * tauI));
	a->onem_mtauI = 1 - a->mtauI;

	init_amd(a);
}

#if 0
static void 
flush_amd(struct amd * a)
{
	a->dc = 0;
	a->dc_insert = 0;
}
#endif

// ����������� SAM
static RAMFUNC FLOAT_t 
demodulator_SAM(
	FLOAT32P_t vp1,
	const uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
	// taken from Warren Pratt´s WDSP, 2016
	// http://svn.tapr.org/repos_sdr_hpsdr/trunk/W5WC/PowerSDR_HPSDR_mRX_PS/Source/wdsp/amd.c

	FLOAT_t audio;	// �������� ����� (���������������� ��������).
	FLOAT32P_t vco;
	FLOAT_t corr [2];
	ncoftwi_t deti;
	ncoftwi_t del_outi;
	FLOAT_t ai, bi, aq, bq;
	FLOAT_t ai_ps, bi_ps, aq_ps, bq_ps;

	struct amd * const a = & amds [pathi];

	vco.QV = peekvalf(FTW2ANGLEI(a->phsi));	// COSF(a->phs);
	vco.IV = peekvalf(FTW2ANGLEQ(a->phsi));	// SINF(a->phs);

	ai = vp1.IV * vco.IV;
	bi = vp1.IV * vco.QV;
	aq = vp1.QV * vco.IV;
	bq = vp1.QV * vco.QV;

	if (a->sbmode != 0)
	{
		int j;

		a->a [0] = a->dsI;
		a->b [0] = bi;
		a->c [0] = a->dsQ;
		a->d [0] = aq;
		a->dsI = ai;
		a->dsQ = bq;

		for (j = 0; j < AMDSTAGES; ++ j)
		{
			const int k = 3 * j;
			a->a [k + 3] = a->c0 [j] * (a->a [k] - a->a [k + 5]) + a->a [k + 2];
			a->b [k + 3] = a->c1 [j] * (a->b [k] - a->b [k + 5]) + a->b [k + 2];
			a->c [k + 3] = a->c0 [j] * (a->c [k] - a->c [k + 5]) + a->c [k + 2];
			a->d [k + 3] = a->c1 [j] * (a->d [k] - a->d [k + 5]) + a->d [k + 2];
		}
		ai_ps = a->a [AMDOUT_IDX];
		bi_ps = a->b [AMDOUT_IDX];
		bq_ps = a->c [AMDOUT_IDX];
		aq_ps = a->d [AMDOUT_IDX];

		for (j = AMDOUT_IDX + 2; j > 0; j--)
		{
			a->a [j] = a->a [j - 1];
			a->b [j] = a->b [j - 1];
			a->c [j] = a->c [j - 1];
			a->d [j] = a->d [j - 1];
		}
	}

	corr [0] = + ai + bq;
	corr [1] = - bi + aq;

	switch (a->sbmode)
	{
	default:
	case 0:	//both sidebands
		{
			audio = corr [0];
			break;
		}
	case 1:	//LSB
		{
			audio = (ai_ps - bi_ps) + (aq_ps + bq_ps);
			break;
		}
	case 2:	//USB
		{
			audio = (ai_ps + bi_ps) - (aq_ps - bq_ps);
			break;
		}
	}

	if (0/*a->levelfade*/)
	{
		a->dc = a->mtauR * a->dc + a->onem_mtauR * audio;
		a->dc_insert = a->mtauI * a->dc_insert + a->onem_mtauI * corr[0];
		audio += a->dc_insert - a->dc;
	}

	if ((corr [0] == 0) && (corr [1] == 0)) 
		corr [0] = 1;

	deti = OMEGA2FTWI(ATAN2F(corr [1], corr [0]));	// - M_PI .. + M_PI
	a->omegai += (a->g2i * deti) >> 32;
	if (a->omegai < a->omegai_min) 
		a->omegai = a->omegai_min;
	else if (a->omegai > a->omegai_max) 
		a->omegai = a->omegai_max;

	del_outi = a->fil_outi;
	a->fil_outi = (int32_t) ((a->g1i * deti) >> 32) + a->omegai;
	a->phsi += del_outi;	/* "�������" �� ������ M_TWOPI ������������� �������������� ������������� ������������� */

	return audio;
}


// ��Ȩ�
// �������������� floating point ����������
// ��� ������������� ����������� ���
// ������������ ����� - ����� ���������, ��������������� ��� ��������� � �������� WITHAFDACWIDTH
static RAMFUNC_NONILINE FLOAT_t baseband_demodulator(
	FLOAT32P_t vp0f,					// ������������ �������� �������
	const uint_fast8_t dspmode, 
	const uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
#if DUALFILTERSPROCESSING
	enum { DUALRXFLT = 1 };
#else /* DUALFILTERSPROCESSING */
	enum { DUALRXFLT = 0 };
#endif /* DUALFILTERSPROCESSING */

	FLOAT_t r;
	switch (dspmode)
	{
	default:
		// ����� ��������
		{
			r = 0;	
		}
		break;

	case DSPCTL_MODE_RX_DSB:
	case DSPCTL_MODE_RX_WIDE:
	case DSPCTL_MODE_RX_NARROW:
	case DSPCTL_MODE_RX_DRM:
	case DSPCTL_MODE_RX_FREEDV:
		{
			// use floating point
			const FLOAT_t fltstrengthslow = agc_measure_float(dspmode, vp0f, pathi);
			const FLOAT_t gain = agc_getgain_float(fltstrengthslow, pathi);
			const FLOAT32P_t vp1 = scalepair(agc_delaysignal(vp0f, pathi), gain);
			const FLOAT32P_t af = get_float_aflo_delta(0, pathi);	// ������� ������� ��������� �������
			r = (vp1.QV * af.QV + vp1.IV * af.IV); // ��������� �� �������� ������� ("+" - ��� ��������).
			r = r * rxoutdenom;
			r *= agc_squelchopen(fltstrengthslow, pathi);
		}
		break;

#if WITHMODEM
	case DSPCTL_MODE_RX_BPSK:
		if (pathi == 0)
		{
			/*const FLOAT_t fltstrengthslow = */ agc_measure_float(dspmode, vp0f, pathi);
			//const FLOAT_t gain = agc_getgain_float(fltstrengthslow, pathi);
			//INT32P_t vp0i32;
			//saved_delta_fi [pathi] = demodulator_FM(vp0f, pathi);	// ����������� ��������� - ��������� ����������� ���
			modem_demod_iq(vp0f);
		}
		r = 0;
		break;
#endif /* WITHMODEM */

	case DSPCTL_MODE_RX_NFM:
		if (/*DUALRXFLT || */pathi == 0)
		{
			// ����������� NBFM
			const FLOAT_t fltstrengthslow = agc_measure_float(dspmode, vp0f, pathi);
			//const FLOAT_t gain = agc_getgain_float(fltstrengthslow, pathi);
			saved_delta_fi [pathi] = demodulator_FM(vp0f, pathi);	// ����������� ��������� - ��������� ����������� ���
			//const int fdelta10 = ((int64_t) saved_delta_fi [pathi] * ARMSAIRATE * 10) >> 32;	// ��������� ������� � 0.1 ���� ��������
			// �������� ��� �������������
			// 0.707 == M_SQRT1_2
			const FLOAT_t sample = saved_delta_fi [pathi]; //(FLOAT_t) M_SQRT1_2;
			r = sample * nfmoutscale; //* rxoutdenom;	// ��������������� � ����������� �����-������ 1_31 -> 1_15
			if (glob_nfm_sql_off == 0)
			{
				// "�������"
				testholdmax3(iir_nfmnbbpf(sample));
				const int nbopen = getholdmax3() < nbfence;
				r = (nbopen != 0) ? r : (r / 16);
			}
			r *= agc_squelchopen(fltstrengthslow, pathi);
		}
		else
			r = 0;
		break;

	case DSPCTL_MODE_RX_AM:
		if (DUALRXFLT || pathi == 0)
		{
			/* AM demodulation */
			// �����, ���� ������������ ������� vp1.IV � vp1.QV, �������� �����������
			const FLOAT_t fltstrengthslow = agc_measure_float(dspmode, vp0f, pathi);
			const FLOAT_t gain = agc_getgain_float(fltstrengthslow, pathi);
			const FLOAT32P_t vp1 = scalepair(agc_delaysignal(vp0f, pathi), gain);
			// ����������� ��
			const FLOAT_t sample = SQRTF(vp1.IV * vp1.IV + vp1.QV * vp1.QV);// * (FLOAT_t) 0.5; //M_SQRT1_2;
			//saved_delta_fi [pathi] = demodulator_FM(vp2, pathi);	// ����������� ��������� - ��������� ����������� ���
			r = sample * rxoutdenom;
			r *= agc_squelchopen(fltstrengthslow, pathi);
		}
		else
			r = 0;
		break;

	case DSPCTL_MODE_RX_WFM:
		r = 0;
		break;

	case DSPCTL_MODE_RX_SAM:
		if (/*DUALRXFLT || */pathi == 0)
		{
			/* synchronous AM demodulation */
			// �����, ���� ������������ ������� vp1.IV � vp1.QV, �������� �����������
			const FLOAT_t fltstrengthslow = agc_measure_float(dspmode, vp0f, pathi);
			const FLOAT_t gain = agc_getgain_float(fltstrengthslow, pathi);
			const FLOAT32P_t vp1 = scalepair(agc_delaysignal(vp0f, pathi), gain);
			//const FLOAT_t sample = SQRTF(vp1.IV * vp1.IV + vp1.QV * vp1.QV) * (FLOAT_t) 0.5; //M_SQRT1_2;
			//saved_delta_fi [pathi] = demodulator_FM(vp2, pathi);	// ����������� ��������� - ��������� ����������� ���
			// ����������� S��
			const FLOAT_t sample = demodulator_SAM(vp1, pathi);
			r = sample * rxoutdenom;
			r *= agc_squelchopen(fltstrengthslow, pathi);
		}
		else
			r = 0;
		break;
	}
	return r;
}

// ���������� ��������� 24/32 ������� ����� (left justified) �� 32 ���
static RAMFUNC int_fast32_t q23lj_to_q31(uint32_t v)
{
	return ((int32_t) v >> (32 - WITHIFADCWIDTH));	// � ������ ������� left justified 24 ���� � 32-� ������ �����
}

#if WITHDSPEXTDDC

// ��Ȩ�
// �������������� 32-� ������ ����������
// ������������ ����� - ����� ���������
static FLOAT32P_t processifadcsampleIQ_ISB(
	const int32_t iv0, const int32_t qv0		// ������������ �������� �������
	)
{
	enum { pathi = 0 };				// 0/1: main_RX/sub_RX
	FLOAT32P_t rv = { 0 };

	return rv;
}

// ��Ȩ�
// �������������� 32-� ������ ����������
// ������������ ����� - ����� ���������
static RAMFUNC FLOAT_t processifadcsampleIQ(
	const uint32_t iv0, const uint32_t qv0,		// ������������ �������� �������
	const uint_fast8_t dspmode, 
	const uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
	if (isdspmoderx(dspmode))
	{
		FLOAT32P_t vp0 = { { q23lj_to_q31(iv0), q23lj_to_q31(qv0) } };
#if WITHDSPLOCALFIR
		////BEGIN_STAMP();
		vp0 = filter_firp_rx_SSB_IQ(vp0);
		////END_STAMP();
#endif /* WITHDSPLOCALFIR */
		return baseband_demodulator(vp0, dspmode, pathi);
	}
	else
	{
		return 0;
	}
}

#else /* WITHDSPEXTDDC */

// ��Ȩ�
// �������������� 24-� ������ �����.
// ������������ ����� - ����� ���������
//
static RAMFUNC FLOAT_t processifadcsamplei(uint32_t v1, uint_fast8_t dspmode)
{
	const uint_fast8_t pathi = 0;

	if (isdspmoderx(dspmode))
	{
		// down-converter � 12 ��� �� zero IF
		const FLOAT32P_t if_lo = get_float4_iflo();
		BEGIN_STAMP();
		const FLOAT32P_t vp0 = filter_fir4_rx_SSB_IQ(scalepair(if_lo, q23lj_to_q31(v1) * RXINSCALE), if_lo.IV != 0); // ������� 12 ��� - 1/4 ������� ������� ��� - ����� ��������������� ������ ����������.
		END_STAMP();
		return baseband_demodulator(vp0, dspmode, pathi);
	}
	else
	{
		return 0;
	}
}


#endif /* WITHDSPEXTDDC */

#if WITHLOOPBACKTEST

// ����������� ������ ��������� ������ ����������� ������ �����
static INT32P_t loopbacktestaudio(INT32P_t vi0, uint_fast8_t dspmode, FLOAT_t shape)
{
	INT32P_t vi;
	vi = vi0;		// ������������� ������������ �������
	//
	// ����� ��������, ��� ������������ ��� �����
	BEGIN_STAMP2();
	//vi.IV = txmikeagc(vi.IV * txlevelfence / mikefence) / TXINSCALE;
	//vi.IV = filter_fir_rx_AUDIO_A(vi.IV);		// ������������� ������������ ������� ����� ������ ��������
	//vi.IV = filter_fir_tx_MIKE(vi.IV, 0);		// ������������� ������������ ������� ����� ������ �����������
	END_STAMP2();
	//vi.IV = 0;	// ��������� ����� �����
	//vi.QV = 0;	// ��������� ������ �����
	//vi.IV = get_dualtone16();	// ����������� ������

#if WITHSECTOGGLE

	// ��������� ��������������� �������
	{
		FLOAT_t v = (get_lout16() + get_rout16()) * shape / 2;
		vi.IV = v;
		vi.QV = v;
	}

#elif 0

	// ����������������� ���������
	vi.QV = vi.IV;	// � ������ ����� �������� ����� ����� (��������)

#else /* WITHSECTOGGLE */

	// ��������� ���� ����� ��� ������ �������
	vi.IV = get_lout16();		// ��� 700 Hz
	vi.QV = get_rout16();		// ��� 500 Hz
	//vi.QV = vi.IV;	// � ������ ����� �������� ����� ����� (��������)

#endif /* WITHSECTOGGLE */

	//float f0 = ssb_iir(vi); // / 4; // * (0x7fff / (float) 0x16000);
	//const float f = filter_fir_tx_MIKE(vi0, 0); //ssb_iir2(vi) * (0x7ff0 / (float) 0x7fff);
	//const float f = vi;
	//const float denom = 0.99999f;

	//float f0 = ssb_iir(vi); // / 4; // * (0x7fff / (float) 0x16000);
	//const float f = filter_fir_tx_MIKE(vi, 0); //ssb_iir2(vi) * (0x7ff0 / (float) 0x7fff);
	//const float f = vi;
	//const float denom = 0.99999f;
	//float f1 = FABSF(vi);

	//float f2 = FABSF(f);
	//vmax = (f2 > vmax) ? f2 : vmax * denom;

	return vi;
}

#endif /* WITHLOOPBACKTEST */

/* �������� ��������� ����������� ����� � ��������� ��� USB AUDIO ������. 16-bit samples */
static RAMFUNC INT32P_t getsampmlemike2(void)			
{
	INT32P_t v;
	if (getsampmlemike(& v) == 0)
	{
		v.IV = 0;
		v.QV = 0;
	}
	// VOX detector � ��������� ����
	// ��������� ������ VOX
	const FLOAT_t vi0f = FMAXF(FABSF(v.IV), FABSF(v.QV));
	charge2(& mikeinlevel, vi0f, (mikeinlevel < vi0f) ? VOXCHARGE : VOXDISCHARGE);

	return v;
}

#if WITHDTMFPROCESSING

// === ���������� ��������� ����� ������
// *** ������ �������������� "������" TDM-�������
// ***  (�� 2 ������ �� ����� � ��������)


#define DTMF_STEPS              205 // ����� ����� �������������� ������
                                    //  ��� DTMF-��������
                                    //  (����� ������ ������ ������ ������ ��������� ���
                                    //      ������� DTMF-����������)
#define DTMF_FREQ               8   // ����� DTMF-������
#define DTMF_EMPTY              (-1)  // ��� ���������� �������� �����


static int16_t inp_samples [DTMF_STEPS];
static volatile int dtmfbi = 0;

static FLOAT_t powIQ(FLOAT_t i, FLOAT_t q)
{
	return SQRTF(i * i + q * q);
}


/******************************************************************************

        Function Name    : int16_t Dtmf_receive (DTMF_buffer_info *)

        Description      : ������� �������� DTMF-������

        Parameters       : ��������� �� ����� �������� �������� ������
                            ������������� �����

        Returns          : ��� ������������ �����

*******************************************************************************/
int16_t Dtmf_receive (const int16_t * samples)
{
	// *** ������� ������������� S � C ��� ������ ������ DTMF (697-1633 ��)
	static const FLOAT_t S_coeff [DTMF_FREQ] = 
	{
		0.520488f / 1, 0.568561f / 1, 0.620326f / 1, 0.673593f / 1,
		0.813151f / 1, 0.867070f / 1, 0.916816f / 1, 0.958745f / 1,
	};
	static const FLOAT_t C_coeff [DTMF_FREQ] = 
	{
		0.853868f / 1, 0.822640f / 1, 0.784343f / 1, 0.739102f / 1,
		0.582052f / 1, 0.498185f / 1, 0.399309f / 1, 0.284266f / 1,
	};

	// *** ������� ��������� ���� �������� DTMF-�����
	static const int16_t Dtmf_code [16] = 
		{0x01,0x04,0x07,0x0B,0x02,0x05,0x08,0x0A,
		0x03,0x06,0x09,0x0C,0x0D,0x0E,0x0F,0x00};

	FLOAT_t         SinA [DTMF_FREQ]; // = { 0.0 };            // ����������� �������� ������
	FLOAT_t         CosA [DTMF_FREQ]; // = { 0.0 };            //  � ��������
	FLOAT_t         furie_image[DTMF_FREQ][2]; // = { { 0.0, 0.0 } };  // ������������ � ������ ����� ������
	FLOAT_t         powS [DTMF_FREQ];            // ����������� �������� ������
	int     i,j;


    // *** Initialisation section ***
    // --- �������������� ������ �������
    for (i = 0; i < DTMF_FREQ; ++i)
    {
        furie_image[i][0] = 0.0f;    // ������������ �����
        furie_image[i][1] = 0.0f;    // ������ �����
        SinA[i] = 0.0f;               // �������� ������
        CosA[i] = 0.7f;               // �������� ��������
//        CosA[i] = 1.0f;               // �������� ��������
    }
    // *** ��������� ���������� ***
    // ������������ ���������� ������������ � ������ ������������ �����-������
    //  �� ���� 8 ��������
    for ( i = 0; i < DTMF_STEPS; ++i )
    {   
		// ��� ������� ���� ��������������
		const float sample = samples [i];

        for ( j = 0; j < DTMF_FREQ; ++j )
        {   // ��� ������ DTMF-�������
            FLOAT_t Cosv = CosA [j];
            FLOAT_t Sinv = SinA [j];
			FLOAT_t sin_tmp = Sinv;

            furie_image[j][0] += sample * Cosv;   // Re
            furie_image[j][1] += sample * Sinv;   // Im
            SinA [j] = Sinv * C_coeff [j] + Cosv * S_coeff[j];
            CosA [j] = Cosv * C_coeff [j] - sin_tmp * S_coeff[j];
        }
    }

    // ��������� �������� ��������� ����� ����������� ������
    for ( j = 0; j < DTMF_FREQ; ++j )
    {   // ���������� ��� �������� ����� ������ Sin
        powS [j] = powIQ(furie_image[j][0], furie_image[j][1]);
    }

    // ������ 4 �������� powS [j] - ���� ������� 4-� ������,
    // ������ 4 �������� powS [j] - ���� ������� 4-� ������,
    // ���������� ��� ����������� ������ � ������������ ����� ������ Cos []
	 int     il, ih;
	FLOAT_t tcos0;
	FLOAT_t tcos1;

	tcos0 = 0; //powS [0];
	il = 0;
	for ( j = 0; j < DTMF_FREQ/2; ++j )   // ���������� ���������� ��� ������� ������
        if ( powS[j] > tcos0 )
        {   tcos0 = powS[j];
            il = j;                 // ���������� ������ ������� ������������� ����
        }

	tcos1 = 0; //powS [DTMF_FREQ/2];
	ih = 0; //DTMF_FREQ/2;
    for ( j = DTMF_FREQ/2; j < DTMF_FREQ; ++j )     // ���������� ���������� ��� ������� ������
        if ( powS[j] > tcos1 )
        {   tcos1 = powS[j];
            ih = j;                 // ���������� ������ ������� ������������� ����
        }
	const FLOAT_t fence = 4;		// ��������� ���������
    // ��������� ������������� ��������� ���������� ����� �� �����, ��� � 16 ���
    for ( i = 0; i < DTMF_FREQ / 2; ++ i )
    {   if ( i == il )  
            continue;
        if ( tcos0 / powS[i] < fence )  // ���� �� ������� ������ �� ������ ������������
                                        //  ��� ������� � 16 ���
            return  DTMF_EMPTY;         // ������� ��������� ������ �� ���������
    }

    for ( i = DTMF_FREQ / 2; i < DTMF_FREQ; ++ i )
    {   if ( i == ih )
            continue;
        if ( tcos1 / powS [i] < fence )  // ���� �� ������� ������ �� ������ ������������
                                        //  ��� ������� � 16 ���
            return  DTMF_EMPTY;         // ������� ��������� ������ �� ���������
    }

	if (tcos0 == 0 || tcos1 == 0)
		return DTMF_EMPTY;

    // ���������� � ���������� �������� �����
    return (Dtmf_code [il | (ih & 3) << 2]);
}

void dtmftest(void)
{
	//debug_printf_P(PSTR("."));
	if (dtmfbi >= DTMF_STEPS)
	{
		long t = 0;
		static const char digits [16] =
		{
			'D', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'0', '*', '#', 'A', 'B', 'C',
		};
		const int digit = Dtmf_receive(inp_samples);
		/*
		debug_printf_P("spectrum:[%8d %8d %8d %8d %8d %8d %8d %8d] ",
				(int) (powS [0] * 10),
				(int) (powS [1] * 10),
				(int) (powS [2] * 10),
				(int) (powS [3] * 10),
				(int) (powS [4] * 10),
				(int) (powS [5] * 10),
				(int) (powS [6] * 10),
				(int) (powS [7] * 10)
				);
		*/
		//t += nsaiirqsrx - tstart;
		if (digit != DTMF_EMPTY)
		{
			if (digit >= (sizeof digits / sizeof digits [0]))
				debug_printf_P(PSTR("dtmf: 0x%02x\n"), digit);
			else
				debug_printf_P(PSTR("dtmf: %c\n"), digit < 16 ? digits [digit] : 'Z');
		}
		else
		{
			//debug_printf_P(PSTR("dtmf: none\n"));
		}
		dtmfbi = 0;
	}
	else
	{
		//debug_printf_P(PSTR(":"));
	}
}

#endif /* WITHDTMFPROCESSING */

// ������ � USB UAC
static RAMFUNC void recordsampleUAC(int left, int right)
{
#if WITHUSBUAC && ! WITHRTSNOAUDIO
	savesamplerecord16uacin(left, right);	// ������ ����������������� ������� ��� ������� ������ � USB 
#endif /* WITHUSBUAC && ! WITHRTSNOAUDIO */
}

// ������ �� SD CARD
static RAMFUNC void recordsampleSD(int left, int right)
{
#if WITHUSEAUDIOREC
	savesamplerecord16SD(left, right);	// ������ ����������������� ������� ��� ������� ������ �� SD CARD
#endif /* WITHUSEAUDIOREC */
}

static RAMFUNC uint_fast8_t isneedfiltering(uint_fast8_t dspmode)
{
	switch (dspmode)
	{
	case DSPCTL_MODE_RX_DSB:
	case DSPCTL_MODE_RX_WIDE:
	case DSPCTL_MODE_RX_NARROW:
	case DSPCTL_MODE_RX_FREEDV:
	case DSPCTL_MODE_RX_NFM:
	case DSPCTL_MODE_RX_AM:
	case DSPCTL_MODE_RX_SAM:
	case DSPCTL_MODE_RX_ISB:
	case DSPCTL_MODE_RX_WFM:
		return 1;

#if WITHUSBUAC
	case DSPCTL_MODE_RX_DRM:	// � ���� ������ �� �������� � ��������
		return 0;
#else /* WITHUSBUAC */
	case DSPCTL_MODE_RX_DRM:	// � ���� ������ �� �������� � ��������
		return 0;
#endif /* WITHUSBUAC */

	default:
		// in transmit modes
		return 0;
	}
}

static RAMFUNC uint_fast8_t isneedmute(uint_fast8_t dspmode)
{
	switch (dspmode)
	{
	case DSPCTL_MODE_RX_WIDE:
	case DSPCTL_MODE_RX_NARROW:
	case DSPCTL_MODE_RX_FREEDV:
	case DSPCTL_MODE_RX_NFM:
	case DSPCTL_MODE_RX_AM:
	case DSPCTL_MODE_RX_SAM:
	case DSPCTL_MODE_RX_ISB:
	case DSPCTL_MODE_RX_DSB:
	case DSPCTL_MODE_RX_WFM:
		return 0;

	case DSPCTL_MODE_RX_BPSK:
		return 1;

#if WITHUSBUAC
	case DSPCTL_MODE_RX_DRM:	// � ���� ������ �� �������� � ��������
		return 1;
#else /* WITHUSBUAC */
	case DSPCTL_MODE_RX_DRM:	// � ���� ������ �������� � ��������
		return 0;
#endif /* WITHUSBUAC */

	default:
		// in transmit modes
		return 1;
	}
}

static RAMFUNC FLOAT_t filterRxAudio_rxA(FLOAT_t v, uint_fast8_t dspmode)
{
	return isneedmute(dspmode) ? 0 : (isneedfiltering(dspmode) ? filter_fir_rx_AUDIO_A(v) : v);
}

#if WITHUSEDUALWATCH

static RAMFUNC FLOAT_t filterRxAudio_rxB(FLOAT_t v, uint_fast8_t dspmode)
{
	return isneedmute(dspmode) ? 0 : (isneedfiltering(dspmode) ? filter_fir_rx_AUDIO_B(v) : v);
}

// ������� ������ - �� ����� ��������� ���� ����������� ���������
static RAMFUNC FLOAT32P_t filterRxAudio_Pair2(FLOAT32P_t v, uint_fast8_t dspmodeA, uint_fast8_t dspmodeB)
{
	const uint_fast8_t m0 = isneedmute(dspmodeA);
	const uint_fast8_t f0 = isneedfiltering(dspmodeA);
	const uint_fast8_t m1 = isneedmute(dspmodeB);
	const uint_fast8_t f1 = isneedfiltering(dspmodeB);
	if (f0 && f1 && ! m0 && ! m1)
	{
		return filter_fir_rx_AUDIO_Pair2(v);	// ���������� ��� �������� ������� ���������
	}
	else
	{
		FLOAT32P_t vFiltered;
		vFiltered.IV = m0 ? 0 : (f0 ? filter_fir_rx_AUDIO_A(v.IV) : v.IV);
		vFiltered.QV = m1 ? 0 : (f1 ? filter_fir_rx_AUDIO_B(v.QV) : v.QV);
		return vFiltered;
	}
}

// ������� ���������� - ����� ��������� ���� ����������� ���������
static RAMFUNC FLOAT32P_t filterRxAudio_Pair(FLOAT32P_t v, uint_fast8_t dspmode)
{
	if (isneedmute(dspmode))
	{
		const FLOAT32P_t vFiltered = { { 0, 0 } };
		return vFiltered;
	}
	else if (isneedfiltering(dspmode))
		return filter_fir_rx_AUDIO_Pair(v);	// ���������� ��� �������� ���������� ��������
	else
		return v;
}

#endif /* WITHUSEDUALWATCH */

#if (WITHRTS96 || WITHRTS192) && ! WITHTRANSPARENTIQ

// ��������� ��������� � ��������

enum { NTap256 = FFTSizeSpectrum };

static volatile uint_fast8_t rendering;

static struct Complex x256 [NTap256 * 2] = { { 0, 0 }, };
static uint_fast16_t fft_head = 0;

// ������������ ����������� �������
void savesamplespectrum96stereo(FLOAT_t iv, FLOAT_t qv)
{
	if (rendering == 0)
	{
		const struct Complex NewSample = { iv, qv };

		// shift the old samples
		// fft_head -  ������ �������������� ����� ������
		// fft_head + NTap256 -  ������� �� ������ ��������������� �����
		fft_head = (fft_head == 0) ? (NTap256 - 1) : (fft_head - 1);
		x256 [fft_head] = x256 [fft_head + NTap256] = NewSample;

	}
}

//static uint_fast8_t	glob_waterfalrange = 64;
static FLOAT_t wnd256 [FFTSizeSpectrum];
static const FLOAT_t waterfalrange = 64;
static FLOAT_t toplogdb; // = LOG10F((FLOAT_t) INT32_MAX / waterfalrange); 

static void buildsigwnd(FLOAT_t *w, int n /* FFTSize */)
{
	int i;
	for (i = 0; i < n; i++)
	{
		w [i] = fir_design_window(i, n);
	}
}

static void adjustwmwp(struct Complex *w, int n /* FFTSize */)
{
	int i;
	for (i = 0; i < n; i++)
	{
		const FLOAT_t r = wnd256 [i]; //fir_design_window(i, n);
		w [i].real *= r;
		w [i].imag *= r;
	}
}

static FLOAT_t getmag2(const struct Complex * Sig)
{
	const FLOAT_t mag = SQRTF(Sig->real * Sig->real + Sig->imag * Sig->imag);
	return mag;
}

// �������������� ������� ������� FFT
// � �������������� ���������� ���� ����������� �������
static int mapfft2raster(
	int i,	/* FFT buffer index */
	int dx	/* Raster position */
	)
{
	const int xx = (i * (dx - 1)) / (FFTSizeSpectrum - 1);	// ����������� �������� ��������� �������� � �������
	const int x = (xx + dx / 2) % dx;	// ����������� ����������� �������
	return x;
}

int dsp_mag2y(FLOAT_t mag, uint_fast16_t dy)
{
	if (mag < waterfalrange)
		return 0;
	const FLOAT_t v = mag / waterfalrange;
	int val = LOG10F(v) * ((int_fast16_t) dy - 1) / toplogdb;

	if (val >= dy) 
		val = dy - 1;
	else if (val < 0) 
		val = 0;
	return val;
}

// ���������� ���������� � ������� � ������� ������ ������ 
// wfarray (�������������� � �������� ������ */
void dsp_getspectrumrow(
	FLOAT_t * const hbase,
	uint_fast16_t dx	// pixel X width of display window
	)
{
	uint_fast16_t i;
	struct Complex * const Sig = & x256 [fft_head];	// ������ ������� ������� ����������� �����
	rendering = 1;
	adjustwmwp(Sig, FFTSizeSpectrum);
	IFFT(Sig, FFTSizeSpectrum, FFTSizeSpectrumM);

	// ������� ������ ������ ������� �����������
	for (i = 0; i < dx; ++ i)
	{
		FLOAT_t * const p1 = & hbase [i];
		* p1 = 0;
	}

	// ����������� � ����� ������� �����������
	for (i = 0; i < FFTSizeSpectrum; ++ i)
	{
		const int x = mapfft2raster(i, dx);
		const FLOAT_t v = getmag2(& Sig [i]);
		FLOAT_t * const p1 = & hbase [x];
		* p1 = FMAXF(* p1, v);
	}
	rendering = 0;

}

static void
dsp_rasterinitialize(void)
{
	buildsigwnd(wnd256, FFTSizeSpectrum);
	toplogdb = LOG10F((FLOAT_t) INT32_MAX / waterfalrange);
}

#else /* (WITHRTS96 || WITHRTS192) && ! WITHTRANSPARENTIQ */

void dsp_getspectrumrow(
	FLOAT_t * const hbase,
	uint_fast16_t dx	// pixel X width of display window
	)
{
	uint_fast16_t x;
	for (x = 0; x < dx; ++ x)
	{
		hbase [x] = 0;
	}
}

int dsp_mag2y(FLOAT_t mag, uint_fast16_t dy)
{
	return 0;
}

#endif /* (WITHRTS96 || WITHRTS192) && ! WITHTRANSPARENTIQ */

#if WITHDSPEXTDDC
// ������������� ������ � �������, ������������ � ����� ������
static void RAMFUNC 
saverts96(const uint32_t * buff)
{
#if WITHRTS96 && ! WITHTRANSPARENTIQ
#if WITHUSBHW && WITHUSBUAC
	// ���� ������������ ��������� �� Rafael Micro R820T - ��������� �������� �������
	if (glob_swapiq != glob_swaprts)
	{
		savesampleout96stereo(
			(int_fast32_t) buff [DMABUF32RTS0Q],	// previous
			(int_fast32_t) buff [DMABUF32RTS0I]
			);	
		savesampleout96stereo(
			(int_fast32_t) buff [DMABUF32RTS1Q],	// current
			(int_fast32_t) buff [DMABUF32RTS1I]
			);	
	}
	else
	{
		savesampleout96stereo(
			(int_fast32_t) buff [DMABUF32RTS0I],	// previous
			(int_fast32_t) buff [DMABUF32RTS0Q]
			);	
		savesampleout96stereo(
			(int_fast32_t) buff [DMABUF32RTS1I],	// current
			(int_fast32_t) buff [DMABUF32RTS1Q]
			);	
	}
#endif /* WITHUSBHW && WITHUSBUAC */

	// ������������ ����������� �������
	// ���� ������������ ��������� �� Rafael Micro R820T - ��������� �������� �������
	if (glob_swaprts != 0)
	{
		savesamplespectrum96stereo(
			(int_fast32_t) buff [DMABUF32RTS0Q],	// previous
			(int_fast32_t) buff [DMABUF32RTS0I]
			);	
		savesamplespectrum96stereo(
			(int_fast32_t) buff [DMABUF32RTS1Q],	// current
			(int_fast32_t) buff [DMABUF32RTS1I]
			);	
	}
	else
	{
		savesamplespectrum96stereo(
			(int_fast32_t) buff [DMABUF32RTS0I],	// previous
			(int_fast32_t) buff [DMABUF32RTS0Q]
			);	
		savesamplespectrum96stereo(
			(int_fast32_t) buff [DMABUF32RTS1I],	// current
			(int_fast32_t) buff [DMABUF32RTS1Q]
			);	
	}

#endif /* WITHRTS96 && ! WITHTRANSPARENTIQ */
}

#endif /* WITHDSPEXTDDC */

// Taken from https://stackoverflow.com/questions/11930594/calculate-atan2-without-std-functions-or-c99

// Approximates atan(x) normalized to the [-1,1] range
// with a maximum error of 0.1620 degrees.

static float normalized_atan( float x )
{
    static const uint32_t sign_mask = 0x80000000;
    static const float b = 0.596227f;

    // Extract the sign bit
    uint32_t ux_s  = sign_mask & * (uint32_t *) & x;

    // Calculate the arctangent in the first quadrant
    float bx_a = fabsf( b * x );
    float num = bx_a + x * x;
    float atan_1q = num / ( 1.f + bx_a + num );

    // Restore the sign bit
    uint32_t atan_2q = ux_s | * (uint32_t *) & atan_1q;
    return * (float *) & atan_2q;
}

// Approximates atan2(y, x) normalized to the [0,4] range
// with a maximum error of 0.1620 degrees

static float normalized_atan2( float y, float x )
{
    static const uint32_t sign_mask = 0x80000000;
    static const float b = (float) 0.596227;

    // Extract the sign bits
    uint32_t ux_s  = sign_mask & * (uint32_t *) & x;
    uint32_t uy_s  = sign_mask & * (uint32_t *) & y;

    // Determine the quadrant offset
    float q = (float)( ( ~ux_s & uy_s ) >> 29 | ux_s >> 30 ); 

    // Calculate the arctangent in the first quadrant
    float bxy_a = fabsf( b * x * y );
    float num = bxy_a + y * y;
    float atan_1q =  num / ( x * x + bxy_a + num );

    // Translate it to the proper quadrant
    uint32_t uatan_2q = (ux_s ^ uy_s) | * (uint32_t *) & atan_1q;
    return q + * (float *) & uatan_2q;
}

static FLOAT_t
demod_WFM(
	int32_t i,
	int32_t q
	)
{
	if (i == 0 && q == 0)
		i = 1;

	static int prev_fi = 0;

	const int fi = ((int16_t) (0x4000 * normalized_atan2(q, i)));	//  returns a value in the range �pi to pi radians, using the signs of both parameters to determine the quadrant of the return value.

	const int d_fi = (int16_t) (fi - prev_fi);
	prev_fi = fi;

	return d_fi;
}

static FLOAT_t
getmonitx(
	uint_fast8_t dspmode, 
	FLOAT_t sdtn, 
	FLOAT_t mike
	)
{
	switch (dspmode)
	{
	default:
		return sdtn;

	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_FREEDV:
	case DSPCTL_MODE_TX_AM:
	case DSPCTL_MODE_TX_NFM:
		return mike;
	}
}
// ��������� ����������� �� DMA ������ � ��������� ��� ������������ (��� ������������� ����).
// ���������� �� ARM_REALTIME_PRIORITY ������.
void RAMFUNC dsp_extbuffer32wfm(const uint32_t * buff)
{
	ASSERT(buff != NULL);
	ASSERT(gwprof < NPROF);
	const uint_fast8_t dspmodeA = globDSPMode [gwprof] [0];
	const uint_fast8_t tx = isdspmodetx(dspmodeA);
	unsigned i;
	const FLOAT_t sdtn = 0;

	//memcpy(dd, buff, sizeof dd);

	for (i = 0; i < DMABUFFSIZE32RX; i += DMABUFSTEP32RX)
	{
		if (dspmodeA == DSPCTL_MODE_RX_WFM)
		{
			FLOAT_t a0 = demod_WFM(buff [i + DMABUF32RXWFM0I], buff [i + DMABUF32RXWFM0Q]);
			FLOAT_t a1 = demod_WFM(buff [i + DMABUF32RXWFM1I], buff [i + DMABUF32RXWFM1Q]);
			FLOAT_t a2 = demod_WFM(buff [i + DMABUF32RXWFM2I], buff [i + DMABUF32RXWFM2Q]);
			FLOAT_t a3 = demod_WFM(buff [i + DMABUF32RXWFM3I], buff [i + DMABUF32RXWFM3Q]);
			const FLOAT_t left = (a0 + a1 + a2 + a3) / 4;

			/* ����� WFM (����������� � FPGA, ������ ��� WITHUSEDUALWATCH)	*/
			//const FLOAT_t right = (int_fast32_t) buff [i + DMABUF32RX1Q] * rxgate;		// ��������� 24-� ������ ����� �� 32 ���
			BEGIN_STAMP2();
			const FLOAT_t leftFiltered = filterRxAudio_rxA(left, dspmodeA);
			//const FLOAT_t leftFiltered = filterRxAudio_rxA(get_lout16(), dspmodeA);	// TODO: debug
			END_STAMP2();
			recordsampleSD(leftFiltered, leftFiltered);	// ������ ����������������� ������� ��� ������� ������
			recordsampleUAC(leftFiltered, leftFiltered);	// ������ � UAC ����������������� ������� ��� ������� ������
			savesampleout16stereo(injectsidetone(leftFiltered, sdtn), injectsidetone(leftFiltered, sdtn));
		}
	}
}

// ��������� ����������� �� DMA ������ � ��������� ��� ������������ (��� ������������� ����).
// ���������� �� ARM_REALTIME_PRIORITY ������.
void RAMFUNC dsp_extbuffer32rx(const uint32_t * buff)
{
	ASSERT(buff != NULL);
	ASSERT(gwprof < NPROF);
	const uint_fast8_t dspmodeA = globDSPMode [gwprof] [0];
	const uint_fast8_t tx = isdspmodetx(dspmodeA);
#if WITHUSEDUALWATCH
	const uint_fast8_t dspmodeB = tx ? DSPCTL_MODE_IDLE : globDSPMode [gwprof] [1];
#endif /* WITHUSEDUALWATCH */
	unsigned i;
	const int rxgate = getRxGate();


	for (i = 0; i < DMABUFFSIZE32RX; i += DMABUFSTEP32RX)
	{
	#if ! WITHTRANSPARENTIQ
		const FLOAT_t ctcss = get_float_subtone() * txlevelfence;
		const FLOAT_t sdtn = get_float_sidetone() * phonefence * shapeSidetoneStep();	// ����� �������� ������� � ���������, ���������� ��� ������
		const INT32P_t vi = getsampmlemike2();	// � ��������� (��� 0, ���� ��� �� ����������) */
		const FLOAT_t monitx = getmonitx(dspmodeA, sdtn, vi.IV);
		const FLOAT_t shape = shapeCWEnvelopStep() * scaleDAC;	// 0..1
	#endif /* ! WITHTRANSPARENTIQ */

#if WITHSUSBSPKONLY
		// ������������ � ������ USB SPEAKER

		if (isdspmodetx(dspmodeA))
		{
			//const INT32P_t dual = loopbacktestaudio(vi, dspmodeA, shape);
			INT32P_t dual;
			dual.IV = get_lout16();		// ��� 700 Hz
			dual.QV = get_rout16();		// ��� 500 Hz
			savesampleout32stereo(iq2tx(dual.IV), iq2tx(dual.QV));	// ����� �������� 24 ���� left justified � 32-� ������ �����.
			//savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));
			recordsampleUAC(dual.IV, dual.QV);	// ������ � UAC ����������������� ������� ��� ������� ������
		}
		else if (isdspmoderx(dspmodeA))
		{
			//const INT32P_t dual = loopbacktestaudio(vi, dspmodeA, shape);
			const INT32P_t dual = vi;
			savesampleout32stereo(iq2tx(dual.IV), iq2tx(dual.QV));	// ����� �������� 24 ���� left justified � 32-� ������ �����.
			//savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));
			recordsampleUAC(get_lout16(), get_rout16());	// ������ � UAC ����������������� ������� ��� ������� ������
		}
		else
		{
			savesampleout32stereo(iq2tx(0), iq2tx(0));	// ����� �������� 24 ���� left justified � 32-� ������ �����.
		}

#elif WITHTRANSPARENTIQ
		/* ��������� ������ ������������ ��������������� ����� ����� USB � FPGA */
	
		savesampleout96stereo(
			(int_fast32_t) buff [i + DMABUF32RX0I],
			(int_fast32_t) buff [i + DMABUF32RX0Q]
			);

		static INT32P_t vi;
		static uint_fast8_t outupsamplecnt;
		if ((outupsamplecnt ++ & 0x01) == 0)	// � ������� ����������� ���� 96 ��� ������
		{
			vi = getsampmlemike2();	// � ��������� (��� 0, ���� ��� �� ����������) */
		}
		savesampleout32stereo(vi.IV << 16, vi.QV << 16);

#elif WITHDTMFPROCESSING
		// ������������ ������������� DTMF

		INT32P_t dual;
		//dual.IV = vi.IV; //get_lout16();
		dual.IV = get_lout16();
		dual.QV = 0;
		processafadcsampleiq(dual, dspmodeA, shape, ctcss);	// ���������� - ������������ ������ ������ (���� I/Q).
		// ������������ ������������� DTMF
		if (dtmfbi < DTMF_STEPS)
		{
			inp_samples [dtmfbi] = vi.IV;
			++ dtmfbi;
		}
		savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));

#elif WITHDSPEXTDDC
	// ������ ����������� � ������� DDC

	saverts96(buff + i);	// ������������� ������ � �������, ������������ � ����� ������

	#if WITHLOOPBACKTEST

		const INT32P_t dual = loopbacktestaudio(vi, dspmodeA, shape);
		processafadcsampleiq(dual, dspmodeA, shape, ctcss);	// ���������� - ������������ ������ ������ (���� I/Q).
		//
		// ������������ ���������� � ������������ �����
		recordsampleSD(dual.IV, dual.QV);	// ������ ����������������� ������� ��� ������� ������
		recordsampleUAC(dual.IV, dual.QV);	// ������ � UAC ����������������� ������� ��� ������� ������
		savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));
		//savesampleout16stereo(dual.IV, dual.QV);

	#elif WITHUSBHEADSET
		/* ��������� �������� USB ���������� ��� ��������� - ����� ������������ */

		recordsampleUAC(get_lout16(), get_rout16());	// ������ � UAC ����������������� ������� ��� ������� ������
		savesampleout16stereo(vi.IV, vi.QV);	/* � line output ��������� ����� - ������� ��������� */
		//savesampleout16stereo(injectsidetone(vi.IV, sdtn), injectsidetone(vi.QV, sdtn));
		savesampleout32stereo(iq2tx(0), iq2tx(0));

	#elif WITHUSEDUALWATCH

		processafadcsampleiq(vi, dspmodeA, shape, ctcss);	// ���������� - ������������ ������ ������ (���� I/Q).
		//
		// ������������� �������

		if (dspmodeA == DSPCTL_MODE_RX_ISB)
		{
			/* ����� ����������� ������� ����� */
			// ��������� ������ � ������ ��������
			const FLOAT32P_t pair = processifadcsampleIQ_ISB(
				(int_fast32_t) buff [i + DMABUF32RX0I] * rxgate,	// ��������� 24-� ������ ����� �� 32 ���
				(int_fast32_t) buff [i + DMABUF32RX0Q] * rxgate	// ��������� 24-� ������ ����� �� 32 ���
				);	
			BEGIN_STAMP2();
			const FLOAT32P_t filtered = filterRxAudio_Pair(pair, dspmodeA);
			END_STAMP2();
			recordsampleSD(tx ? monitx : filtered.IV, tx ? monitx : filtered.QV);	// ������ ����������������� ������� ��� ������� ������
			recordsampleUAC(tx ? monitx : filtered.IV, tx ? monitx : filtered.QV);	// ������ � UAC ����������������� ������� ��� ������� ������
			savesampleout16stereo(filtered.IV, filtered.QV);	/* � line output ��������� ����� - ������� ��������� */
		}
		else
		{
			// buff data layout: I main/I sub/Q main/Q sub
			const FLOAT_t rxA = processifadcsampleIQ(
				(int_fast32_t) buff [i + DMABUF32RX0I] * rxgate,	// ��������� 24-� ������ ����� �� 32 ���
				(int_fast32_t) buff [i + DMABUF32RX0Q] * rxgate,	// ��������� 24-� ������ ����� �� 32 ���
				dspmodeA,
				0	// MAIN RX
				);

			const FLOAT_t rxB = processifadcsampleIQ(
				(int_fast32_t) buff [i + DMABUF32RX1I] * rxgate,	// ��������� 24-� ������ ����� �� 32 ���
				(int_fast32_t) buff [i + DMABUF32RX1Q] * rxgate,	// ��������� 24-� ������ ����� �� 32 ���
				dspmodeB,
				1	// SUB RX
				);	


			switch (glob_mainsubrxmode)
			{
			default:
			case BOARD_RXMAINSUB_A_A:
				// left:A/right:A
				{
					BEGIN_STAMP2();
					const FLOAT_t filtered = filterRxAudio_rxA(rxA, dspmodeA);
					END_STAMP2();

					recordsampleSD(tx ? monitx : filtered, tx ? monitx : filtered);	// ������ ����������������� ������� ��� ������� ������
					recordsampleUAC(tx ? monitx : filtered, tx ? monitx : filtered);	// ������ � UAC ����������������� ������� ��� ������� ������
					savesampleout16stereo(injectsidetone(filtered, sdtn), injectsidetone(filtered, sdtn));
				}
				break;

			case BOARD_RXMAINSUB_A_B:
				// left:A/right:B
				if /*(0)*/(dspmodeA == dspmodeB)
				{
					// ������� ���������� - ����� ��������� ���� ����������� ���������
					const FLOAT32P_t pair = { { rxA, rxB } };
					BEGIN_STAMP2();
					const FLOAT32P_t filtered2 = filterRxAudio_Pair(pair, dspmodeA);
					END_STAMP2();

					recordsampleSD(tx ? monitx : filtered2.IV, tx ? monitx : filtered2.QV);	// ������ ����������������� ������� ��� ������� ������
					recordsampleUAC(tx ? monitx : filtered2.IV, tx ? monitx : filtered2.QV);	// ������ � UAC ����������������� ������� ��� ������� ������
					savesampleout16stereo(injectsidetone(filtered2.IV, sdtn), injectsidetone(filtered2.QV, sdtn));
				}
				else
				{
					const FLOAT32P_t pair = { { rxA, rxB } };
					BEGIN_STAMP2();
					const FLOAT32P_t filtered2 = filterRxAudio_Pair2(pair, dspmodeA, dspmodeB);
					BEGIN_STAMP2();

					recordsampleSD(tx ? monitx : filtered2.IV, tx ? monitx : filtered2.QV);	// ������ ����������������� ������� ��� ������� ������
					recordsampleUAC(tx ? monitx : filtered2.IV, tx ? monitx : filtered2.QV);	// ������ � UAC ����������������� ������� ��� ������� ������
					savesampleout16stereo(injectsidetone(filtered2.IV, sdtn), injectsidetone(filtered2.QV, sdtn));
				}
				break;

			case BOARD_RXMAINSUB_TWO:
				// � ��� ����������� ��������� ����� ������� ����������.	
				if /*(0)*/(dspmodeA == dspmodeB)
				{
					// ������� ���������� - ����� ��������� ���� ����������� ���������
					const FLOAT32P_t pair = { { rxA, rxB } };
					BEGIN_STAMP2();
					const FLOAT32P_t filtered2 = filterRxAudio_Pair(pair, dspmodeA);
					const FLOAT_t filtered = (filtered2.IV + filtered2.QV) / 2;
					END_STAMP2();

					recordsampleSD(tx ? monitx : filtered, tx ? monitx : filtered);	// ������ ����������������� ������� ��� ������� ������
					recordsampleUAC(tx ? monitx : filtered, tx ? monitx : filtered);	// ������ � UAC ����������������� ������� ��� ������� ������
					savesampleout16stereo(injectsidetone(filtered, sdtn), injectsidetone(filtered, sdtn));
				}
				else
				{
					const FLOAT32P_t pair = { { rxA, rxB } };
					BEGIN_STAMP2();
					const FLOAT32P_t filtered2 = filterRxAudio_Pair2(pair, dspmodeA, dspmodeB);
					const FLOAT_t filtered = (filtered2.IV + filtered2.QV) / 2;
					END_STAMP2();

					recordsampleSD(tx ? monitx : filtered, tx ? monitx : filtered);	// ������ ����������������� ������� ��� ������� ������
					recordsampleUAC(tx ? monitx : filtered, tx ? monitx : filtered);	// ������ � UAC ����������������� ������� ��� ������� ������
					savesampleout16stereo(injectsidetone(filtered, sdtn), injectsidetone(filtered, sdtn));
				}
				break;

			case BOARD_RXMAINSUB_B_A:
				// left:B/right:A
				if /*(0)*/(dspmodeA == dspmodeB)
				{
					// ������� ���������� - ����� ��������� ���� ����������� ���������
					const FLOAT32P_t pair = { { rxA, rxB } };
					BEGIN_STAMP2();
					const FLOAT32P_t filtered2 = filterRxAudio_Pair(pair, dspmodeA);
					END_STAMP2();

					recordsampleSD(tx ? monitx : filtered2.QV, tx ? monitx : filtered2.IV);	// ������ ����������������� ������� ��� ������� ������
					recordsampleUAC(tx ? monitx : filtered2.QV, tx ? monitx : filtered2.IV);	// ������ � UAC ����������������� ������� ��� ������� ������
					savesampleout16stereo(injectsidetone(filtered2.QV, sdtn), injectsidetone(filtered2.IV, sdtn));
				}
				else
				{
					const FLOAT32P_t pair = { { rxA, rxB } };
					BEGIN_STAMP2();
					const FLOAT32P_t filtered2 = filterRxAudio_Pair2(pair, dspmodeA, dspmodeB);
					END_STAMP2();

					recordsampleSD(tx ? monitx : filtered2.QV, tx ? monitx : filtered2.IV);	// ������ ����������������� ������� ��� ������� ������
					recordsampleUAC(tx ? monitx : filtered2.QV, tx ? monitx : filtered2.IV);	// ������ � UAC ����������������� ������� ��� ������� ������
					savesampleout16stereo(injectsidetone(filtered2.QV, sdtn), injectsidetone(filtered2.IV, sdtn));
				}
				break;

			case BOARD_RXMAINSUB_B_B:
				// left:B/right:B
				{
					BEGIN_STAMP2();
					const FLOAT_t filtered = filterRxAudio_rxB(rxB, dspmodeB);	// todo: ���� ������ � ��������
					END_STAMP2();

					recordsampleSD(tx ? monitx : filtered, tx ? monitx : filtered);	// ������ ����������������� ������� ��� ������� ������
					recordsampleUAC(tx ? monitx : filtered, tx ? monitx : filtered);	// ������ � UAC ����������������� ������� ��� ������� ������
					savesampleout16stereo(injectsidetone(filtered, sdtn), injectsidetone(filtered, sdtn));
				}
				break;
			}
		}

	#else /* WITHUSEDUALWATCH */

		processafadcsampleiq(vi, dspmodeA, shape, ctcss);	// ���������� - ������������ ������ ������ (���� I/Q).
		// ������������� �������

		if (dspmodeA == DSPCTL_MODE_RX_WFM)
		{
#if ! WITHSAI2HW
			BEGIN_STAMP2();
			const FLOAT_t leftFiltered = 0;
			END_STAMP2();
			recordsampleSD(leftFiltered, leftFiltered);	// ������ ����������������� ������� ��� ������� ������
			recordsampleUAC(leftFiltered, leftFiltered);	// ������ � UAC ����������������� ������� ��� ������� ������
			savesampleout16stereo(injectsidetone(leftFiltered, sdtn), injectsidetone(leftFiltered, sdtn));
#endif /* ! WITHSAI2HW */
		}
		else if (dspmodeA == DSPCTL_MODE_RX_ISB)
		{
			/* ����� ����������� ������� ����� */
			// ��������� ������ � ������ ��������
			const FLOAT32P_t rv = processifadcsampleIQ_ISB(
				(int_fast32_t) buff [i + DMABUF32RX0I] * rxgate,	// ��������� 24-� ������ ����� �� 32 ���
				(int_fast32_t) buff [i + DMABUF32RX0Q] * rxgate	// ��������� 24-� ������ ����� �� 32 ���
				);	
			recordsampleSD(tx ? monitx : rv.IV, tx ? monitx : rv.QV);	// ������ ����������������� ������� ��� ������� ������
			recordsampleUAC(tx ? monitx : rv.IV, tx ? monitx : rv.QV);	// ������ � UAC ����������������� ������� ��� ������� ������
			savesampleout16stereo(rv.IV, rv.QV);	/* � line output ��������� ����� - ������� ��������� */
		}
		else
		{
			// ��������� ������ � ������ ��������
			const FLOAT_t left = processifadcsampleIQ(
				(int_fast32_t) buff [i + DMABUF32RX0I] * rxgate,	// ��������� 24-� ������ ����� �� 32 ���
				(int_fast32_t) buff [i + DMABUF32RX0Q] * rxgate,	// ��������� 24-� ������ ����� �� 32 ���
				dspmodeA,
				0		// MAIN RX
				);	

			BEGIN_STAMP2();
			const FLOAT_t leftFiltered = filterRxAudio_rxA(left, dspmodeA);
			END_STAMP2();
			recordsampleSD(tx ? monitx : leftFiltered, tx ? monitx : leftFiltered);	// ������ ����������������� ������� ��� ������� ������
			recordsampleUAC(tx ? monitx : leftFiltered, tx ? monitx : leftFiltered);	// ������ � UAC ����������������� ������� ��� ������� ������
			savesampleout16stereo(injectsidetone(leftFiltered, sdtn), injectsidetone(leftFiltered, sdtn));
		}

	#endif /*  DMABUFSTEP32 == 4 */

#else /* WITHDSPEXTDDC */
	// ������ ����������� ��� �������� DDC

	#if WITHLOOPBACKTEST

		const INT32P_t dual = loopbacktestaudio(vi, dspmodeA, shape);
		processafadcsample(dual, dspmodeA, shape, ctcss);	// ��������� ������ ������ � ���������
		//
		// ������������ ���������� � ������������ �����
		recordsampleSD(dual.IV, dual.QV);	// ������ ����������������� ������� ��� ������� ������
		recordsampleUAC(dual.IV, dual.QV);	// ������ � UAC ����������������� ������� ��� ������� ������
		savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));

	#else /* WITHLOOPBACKTEST */

		processafadcsample(vi, dspmodeA, shape, ctcss);	// ���������� - ������������� ��������� � AF ADC ������

		const FLOAT_t left = processifadcsamplei(buff [i + DMABUF32RX] * rxgate, dspmodeA);	// ��������� 24-� ������ ����� �� 32 ���
		BEGIN_STAMP2();
		const FLOAT_t leftFiltered = filterRxAudio_rxA(left, dspmodeA);
		END_STAMP2();
		recordsampleSD(tx ? monitx : leftFiltered, tx ? monitx : leftFiltered);	// ������ ����������������� ������� ��� ������� ������
		recordsampleUAC(tx ? monitx : leftFiltered, tx ? monitx : leftFiltered);	// ������ � UAC ����������������� ������� ��� ������� ������
		savesampleout16stereo(injectsidetone(leftFiltered, sdtn), injectsidetone(leftFiltered, sdtn));

	#endif /* WITHLOOPBACKTEST */
#endif /* WITHDSPEXTDDC */
	}
}

//////////////////////////////////////////
// glob_cwedgetime - ������������ ����������/����� ��������� CW (� ������� ������������) � �������� ����������

static volatile unsigned enveloplen0 = NSAITICKS(5) + 1;	/* ���������� ����� ����. */
static unsigned shapeSidetonePos = 0;
static volatile uint_fast8_t shapeSidetoneInpit = 0;

static unsigned shapeCWEnvelopPos = 0;
static volatile uint_fast8_t cwgateflag = 0;
static volatile uint_fast8_t rxgateflag = 0;

// 0..1
static RAMFUNC FLOAT_t peakshapef(unsigned shapePos)	/* shapePos: �� 0 �� enveloplen0 ������������. */
{
	const ftw_t halfcircle = (ftw_t) 1U << (NCOFTWBITS - 1);
	const FLOAT_t v = (1 - peekvalf(FTW2ANGLEQ((uint_fast64_t) shapePos * halfcircle / enveloplen0))) * (FLOAT_t) 0.5;	// v = - cos(angle)
	return v;
}

// ������������ ��������� ��� ������������
// 0..1
static RAMFUNC FLOAT_t shapeSidetoneStep(void)
{
	const unsigned enveloplen = enveloplen0;
	/* ��� ����������� ������������ ����������/����� �� ���� ������� ������� �� �������������� */
	if (shapeSidetoneInpit >= enveloplen)
		shapeSidetoneInpit = enveloplen;

	const FLOAT_t v = peakshapef(shapeSidetonePos);
	
	if (shapeSidetoneInpit != 0)
		shapeSidetonePos = shapeSidetonePos >= enveloplen ? enveloplen : (shapeSidetonePos + 1);
	else
		shapeSidetonePos = shapeSidetonePos == 0 ? 0 : (shapeSidetonePos - 1);
	return v;
}

// ������������ ��������� ��� ��������
// 0..1
static RAMFUNC FLOAT_t shapeCWEnvelopStep(void)
{
	const unsigned enveloplen = enveloplen0;
	/* ��� ����������� ������������ ����������/����� �� ���� ������� ������� �� �������������� */
	if (shapeCWEnvelopPos >= enveloplen)
		shapeCWEnvelopPos = enveloplen;
	const FLOAT_t v = peakshapef(shapeCWEnvelopPos);

	if (cwgateflag != 0)
		shapeCWEnvelopPos = shapeCWEnvelopPos >= enveloplen ? enveloplen : (shapeCWEnvelopPos + 1);
	else
		shapeCWEnvelopPos = shapeCWEnvelopPos == 0 ? 0 : (shapeCWEnvelopPos - 1);
	return v;
}

// ������� �������� ����, ��� ���������� ������ ��� ���� - �� ��������� ��������� ������������ ���������
static RAMFUNC uint_fast8_t getTxShapeNotComplete(void)
{
	const unsigned enveloplen = enveloplen0;
	/* ��� ����������� ������������ ����������/����� �� ���� ������� ������� �� �������������� */
	if (shapeCWEnvelopPos >= enveloplen)
		shapeCWEnvelopPos = enveloplen;
	return shapeCWEnvelopPos != enveloplen;
}

/* ���������� ������ ������ � ������ ����� */
static uint_fast8_t getRxGate(void)
{
#if TXGFV_RX != 0
	return rxgateflag;
#else
	return ! cwgateflag;
#endif
}


// ����������� � ������������ �� ��������
void dsp_txpath_set(portholder_t txpathstate)
{
#if WITHINTEGRATEDDSP
	#if WITHTX
		cwgateflag = (txpathstate & (TXGFV_TX_CW | TXGFV_TX_SSB | TXGFV_TX_AM | TXGFV_TX_NFM)) != 0;
		#if TXGFV_RX != 0
			rxgateflag = (txpathstate & (TXGFV_RX)) != 0;
		#endif /* TXGFV_RX != 0 */
	#else /* WITHTX */
		rxgateflag = 1;
	#endif /* WITHTX */
#endif /* WITHINTEGRATEDDSP */
}

//////////////////////////////////////////

/* ����� ��������� ������ ����������� ������ - ���������� ���������������� �������. */
void hardware_sounds_disable(void)
{
	//anglestep_sidetone = 0;
	shapeSidetoneInpit = 0;
}

#if SIDETONE_TARGET_BIT != 0
	#error SIDETONE_TARGET_BIT != 0
#endif
// called from interrupt or with disabled interrupts
// ������ �������� ��������� ��������� �������
void hardware_sounds_setfreq(
	uint_fast8_t prei,		// returned value from hardware_calc_sound_params
	unsigned value
	)
{
	anglestep_sidetone = value;
	shapeSidetoneInpit = 1;
}

// return code: prescaler
uint_fast8_t
hardware_calc_sound_params(
	uint_least16_t tonefreq,	/* tonefreq - ������� � ������� ����� �����. ������� - 400 ���� (���������� ������� ������ CAT). */
	unsigned * pvalue)
{
	* pvalue = FTWAF001(tonefreq * 10u);	// � ����� ����� �����
	return 0;
}

/* �������� �������� ������� �� ��� ��������� DSP */
int_fast32_t
dsp_get_ifreq(void)
{
#if WITHDSPEXTDDC
	return 0;
#else /* WITHDSPEXTDDC */
	return ARMSAIRATE / 4;
#endif /* WITHDSPEXTDDC */
}

/* �������� �������� ������� ������� ��������� ������ DSP */
int_fast32_t
dsp_get_sampleraterx(void)
{
	return ARMI2SRATE;
}

/* �������� �������� ������� ������� ��������� ������ DSP, ����������� � scale ��� */
int_fast32_t
dsp_get_sampleraterxscaled(uint_fast8_t scale)
{
	return ARMI2SRATEX(scale);
}

/* �������� �������� ������� ������� �������� ������ DSP */
int_fast32_t
dsp_get_sampleratetx(void)
{
	return ARMI2SRATE;
}

/* �������� �������� ������� ������� ��������� ������ DSP */
int_fast32_t
dsp_get_samplerate100(void)
{
	return ARMI2SRATE100;
}

// �������� ���������� � DSP ������
// ���������� ���������� ��������� (����� ��������).
static void 
rxparam_update(uint_fast8_t profile, uint_fast8_t pathi)
{
	// ��������� ��� ��������
	{
		const int gainmax = glob_digigainmax;	// ������� ������ ����������� ��������
		const int gainmin = 0;	// ������ ������ ����������� ��������
		const int gaindb = ((gainmax - gainmin) * (int) (glob_ifgain - BOARD_IFGAIN_MIN) / (int) (BOARD_IFGAIN_MAX - BOARD_IFGAIN_MIN)) + gainmin;	// -20..+100 dB
		const FLOAT_t manualrfgain = db2ratio(gaindb);
		
		agc_parameters_update(& rxagcparams [profile] [pathi], manualrfgain, pathi);	// ������� #0,#1
	}

	// ��������� S-����� ��������
	{
		const int gainmax = glob_digigainmax;	// ������� ������ ����������� ��������
		const int gainmin = 0;	// ������ ������ ����������� ��������
		const int gaindb = ((gainmax - gainmin) * (int) (glob_ifgain - BOARD_IFGAIN_MIN) / (int) (BOARD_IFGAIN_MAX - BOARD_IFGAIN_MIN)) + gainmin;	// -20..+100 dB
		const FLOAT_t manualrfgain = db2ratio(gaindb);
		
		agc_smeter_parameters_update(& rxsmeterparams, manualrfgain, 0);	// ��� � ��������� #0
	}


	// ��������� SAM ��������
	{
		const int pll = 4000;
		const int zeta_help = 65;
		const FLOAT_t zeta = (FLOAT_t) zeta_help / (FLOAT_t) 100; // PLL step response: smaller, slower response 1.0 - 0.1
		const FLOAT_t omegaN = 200; // PLL bandwidth 50.0 - 1000.0
		const FLOAT_t tauR = (FLOAT_t) 0.02; // original 0.02;
		const FLOAT_t tauI = (FLOAT_t) 1.4; // original 1.4;  
		create_amd(& amds [pathi], 0, - pll, + pll, zeta, omegaN, tauR, tauI);
	}

	// �������
	{
		const volatile agcparams_t * const agcp = & rxsmeterparams;

		const FLOAT_t upper = agccalcstrength(agcp, agcp->levelfence);
		const FLOAT_t lower = agccalcstrength(agcp, agcp->mininput);
		manualsquelch [pathi] = (int) glob_squelch * (upper - lower) / 255 + lower;
	}

	// ������� NFM
	nbfence = POWF(2, WITHIFADCWIDTH - 8) * (int) glob_nfm_sql_lelel;	// glob_nfm_sql_lelel: 0..255

	// ������� ������� ������������
	sidetonevolume = (glob_sidetonelevel / (FLOAT_t) 100);
	mainvolumerx = 1 - sidetonevolume;

}
// �������� ���������� � DSP ������
// ���������� ���������� ����������� (����� ��������).
static void 
txparam_update(uint_fast8_t profile)
{

	// ��������� ��� ���������
	comp_parameters_update(& txagcparams [profile], (int) glob_mikeagcgain);

	{
		// ��������� ������������
		const FLOAT_t FS = txagcparams [profile].levelfence;
		const FLOAT_t grade = 1 - (glob_mikehclip / (FLOAT_t) 100);
		mickeclipscale [profile] = 1 / grade;
		mickecliplevelp [profile] = FS * grade;
		mickeclipleveln [profile] = - FS * grade;
	}

	{
		// AM parameters
		const FLOAT_t amshapesignal = (FLOAT_t) (int) glob_amdepth / (100 + (int) glob_amdepth);
		amshapesignalHALF = amshapesignal / 2;
		amcarrierHALF = txlevelfenceHALF - txlevelfenceHALF * amshapesignal;
	}

	scaleDAC = (FLOAT_t) (int) glob_dacscale / 100;

	subtonevolume = (glob_subtonelevel / (FLOAT_t) 100);
	mainvolumetx = 1 - subtonevolume;

#if WITHCPUDACHW && WITHPOWERTRIM
	// ALC
	// ����������� ���������� �� REFERENCE INPUT TXDAC AD9744 
	HARDWARE_DAC_ALC((glob_opowerlevel - WITHPOWERTRIMMIN) * dac_dacfs_coderange / (WITHPOWERTRIMMAX - WITHPOWERTRIMMIN) + dac_dacfs_lowcode);
#endif /* WITHCPUDACHW && WITHPOWERTRIM */
}

// �������� ���������� � DSP ������
static void 
trxparam_update(void)
{
	// CW & sidetone edge
	enveloplen0 = NSAITICKS(glob_cwedgetime) + 1;		/* ���������� �������, �� ������� �������� ��������� */

}

/* ���������� ��� ����������� �����������. */
void dsp_initialize(void)
{
	FFT_initialize();
#if (WITHRTS96 || WITHRTS192) && ! WITHTRANSPARENTIQ
	dsp_rasterinitialize();
#endif /* WITHRTS96 && ! WITHTRANSPARENTIQ */

	fir_design_windowbuff(FIRCwnd_tx_MIKE, Ntap_tx_MIKE);
	fir_design_windowbuff(FIRCwnd_rx_AUDIO, Ntap_rx_AUDIO);

#if WITHDSPEXTFIR
	#if WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08)
		fir_design_windowbuffL(FIRCwndL_trxi_IQ, Ntap_trxi_IQ);
	#else
		fir_design_windowbuff(FIRCwnd_trxi_IQ, Ntap_trxi_IQ);
	#endif
#endif /* WITHDSPEXTFIR */

#if WITHDSPLOCALFIR	
	fir_design_windowbuff(FIRCwnd_rx_SSB_IQ, Ntap_rx_SSB_IQ);
	fir_design_windowbuff(FIRCwnd_tx_SSB_IQ, Ntap_tx_SSB_IQ);
#endif /* WITHDSPLOCALFIR */

	omega2ftw_k1 = POWF(2, NCOFTWBITS);

	// 0.707 == M_SQRT1_2
	/* http://gregstoll.dyndns.org/~gregstoll/floattohex/ use for tests */

	// ����������� ����������� ������
	#if WITHIFDACWIDTH > DSP_FLOAT_BITSMANTISSA
		const int_fast32_t dacFS = 0x7ffff000L >> (32 - WITHIFDACWIDTH);	/* 0x7ffff800L ��� ��� float ����� �������� 24 ���� � �������� (23 ����� � ���� - ������� - ��������������� ������ �������) */
	#else /* WITHIFDACWIDTH > DSP_FLOAT_BITSMANTISSA */
		const int_fast32_t dacFS = (((uint_fast64_t) 1 << (WITHIFDACWIDTH - 1)) - 1);
	#endif /* WITHIFDACWIDTH > DSP_FLOAT_BITSMANTISSA */

	txlevelfence = dacFS * db2ratio(- (FLOAT_t) 1.75) * (FLOAT_t) M_SQRT1_2;	// �������������� �� ���������� ��������� ������������ DUC ��� ��������
	txlevelfenceSSB_INTEGER = txlevelfence;	// ��� ��������� ����
	txlevelfenceHALF = txlevelfence / 2;	// ��� ������� � lo6=0 - � ������� ��� ���������� ��������� �������

	txlevelfenceSSB = txlevelfence;
#if WITHTXDACFULL
	txlevelfenceBPSK = txlevelfence;
	txlevelfenceNFM = txlevelfence;
	txlevelfenceCW = txlevelfence;
#else /* WITHTXDACFULL */
	txlevelfenceBPSK = txlevelfence / (FLOAT_t) 1.5;
	txlevelfenceNFM = txlevelfence / 2;
	txlevelfenceCW = txlevelfence / 2;
#endif /* WITHTXDACFULL */

	// ����������� �������� ������
	#if WITHIFADCWIDTH > DSP_FLOAT_BITSMANTISSA
		const int_fast32_t adcFS = (0x7ffff000L >> (32 - WITHIFADCWIDTH));	/* 0x7ffff800L ��� ��� float ����� �������� 24 ���� � �������� (23 ����� � ���� - ������� - ��������������� ������ �������) */
	#else /* WITHIFADCWIDTH > DSP_FLOAT_BITSMANTISSA */
		const int_fast32_t adcFS = (((uint_fast64_t) 1 << (WITHIFADCWIDTH - 1)) - 1);
	#endif /* WITHIFADCWIDTH > DSP_FLOAT_BITSMANTISSA */

	rxlevelfence = adcFS * db2ratio(- 1);
	// ����������� ������������ � ��������� �������
	mikefence = POWF(2, WITHAFADCWIDTH - 1) - 1;

	// ����������� ������������ �� �������� �������
	phonefence = (POWF(2, WITHAFDACWIDTH - 1) - 1)  * db2ratio(- 1);
	// ��������������� (INT32_MAX + 1) � phonefence
	nfmoutscale = POWF(2, - (int) (NCOFTWBITS - 1)) * phonefence;

	agc_initialize();
	voxmeter_initialize();
	trxparam_update();
	const uint_fast8_t rprofile = ! gwagcprofrx;	// ������ �������, ������� ������ �������

	uint_fast8_t pathi;
	for (pathi = 0; pathi < NTRX; ++ pathi)
		rxparam_update(rprofile, pathi);
	gwagcprofrx = rprofile;

	const uint_fast8_t tprofile = ! gwagcproftx;	// ������ �������, ������� ������ �������
	txparam_update(tprofile);
	gwagcproftx = tprofile;

	const uint_fast8_t spf = ! gwprof;	// ������ ������� ��� ���������� ���������� DSP

	audio_setup_mike(spf);
	for (pathi = 0; pathi < NTRX; ++ pathi)
		audio_update(spf, pathi);
	gwprof = spf;

	modem_update();

	debug_printf_P(PSTR("dsp_initialize: ARMI2SRATE=%lu\n"), (unsigned long) ARMI2SRATE);
	debug_printf_P(PSTR("dsp_initialize: ARMI2SRATE100=%lu.%02lu\n"), (unsigned long) (ARMI2SRATE100 / 100), (unsigned long) (ARMI2SRATE100 % 100));

#if 0
	{
		float32_t vs [4] = { 1, 2, 3, 4 };
		float32x4_t v1 = vld1q_f32(vs);
		float32x4_t v2 = vrev64q_f32(v1);
		// output: neontest: 2 1 4 3
		debug_printf_P(PSTR("neontest: %d %d %d %d\n"), (int) vgetq_lane_f32(v2, 0), (int) vgetq_lane_f32(v2, 1), (int) vgetq_lane_f32(v2, 2), (int) vgetq_lane_f32(v2, 3));
	}
#endif
}

void 
prog_dsplreg(void)
{
	const uint_fast8_t pathn = isdspmodetx(glob_dspmodes [0]) ? 1 : NTRX;	// ��� �������� ������ ����� � ������� 0
	trxparam_update();
	const uint_fast8_t rprofile = ! gwagcprofrx;	// ������ �������, ������� ������ �������
	uint_fast8_t pathi;
	for (pathi = 0; pathi < pathn; ++ pathi)
		rxparam_update(rprofile, pathi);
	gwagcprofrx = rprofile;

	const uint_fast8_t tprofile = ! gwagcproftx;	// ������ �������, ������� ������ �������
	txparam_update(tprofile);
	gwagcproftx = tprofile;

#if (CTLSTYLE_RAVENDSP_V1 || CTLSTYLE_DSPV1A)
	if (isdspmoderx(glob_dspmodes [0]))
	{
		// �������� ������ �� (AD605)
		HARDWARE_DAC_AGC((glob_gvad605 * dac_agc_coderange) / UINT8_MAX + dac_agc_lowcode);
	}
	else
	{
		// ��� �������� �������
		HARDWARE_DAC_AGC(0);
	}
#endif /* (CTLSTYLE_RAVENDSP_V1 || CTLSTYLE_DSPV1A) */

}

void 
prog_fltlreg(void)
{
	const uint_fast8_t pathn = isdspmodetx(glob_dspmodes [0]) ? 1 : NTRX;	// ��� �������� ������ ����� � ������� 0
	// ���������� ������������� ����������
	const uint_fast8_t spf = ! gwprof;	// ������ ������� ��� ���������� ���������� DSP

	audio_setup_mike(spf);
	uint_fast8_t pathi;
	for (pathi = 0; pathi < pathn; ++ pathi)
		audio_update(spf, pathi);

	gwprof = spf;

	modem_update();
}

#elif WITHEXTERNALDDSP /* ������� ���������� ������� DSP ������. */


#define DSPREG_SPIMODE	SPIC_MODE3	// DSP module ��� ���������� ������ "�����"

void 
prog_dsplreg(void)
{
	const spitarget_t target = targetdsp1;
	uint_fast8_t buff [DSPCTL_BUFSIZE]; // = { 0 };
	uint_fast8_t i;

	buff [DSPCTL_OFFSET_MODEA] = glob_dspmodes [0];
	buff [DSPCTL_OFFSET_MODEB] = glob_dspmodes [1];
	buff [DSPCTL_OFFSET_AFGAIN_HI] = glob_afgain >> 8;
	buff [DSPCTL_OFFSET_AFGAIN_LO] = glob_afgain;
	buff [DSPCTL_OFFSET_IFGAIN_HI] = glob_ifgain >> 8;
	buff [DSPCTL_OFFSET_IFGAIN_LO] = glob_ifgain;
	buff [DSPCTL_OFFSET_AFMUTE] = glob_afmute;	/* ��������� ���� � ��������� � ��������� */
	buff [DSPCTL_OFFSET_AGCOFF] = (glob_agc == BOARD_AGCCODE_OFF);
	buff [DSPCTL_OFFSET_MICLEVEL_HI] = glob_mik1level >> 8;
	buff [DSPCTL_OFFSET_MICLEVEL_LO] = glob_mik1level;
	buff [DSPCTL_OFFSET_AGC_T1] = glob_agc_t1;
	buff [DSPCTL_OFFSET_AGC_T2] = glob_agc_t2;
	buff [DSPCTL_OFFSET_AGC_T4] = glob_agc_t4;
	buff [DSPCTL_OFFSET_AGC_THUNG] = glob_agc_thung;
	buff [DSPCTL_OFFSET_AGCRATE] = glob_agcrate; // may be UINT8_MAX

	buff [DSPCTL_OFFSET_NFMSQLLEVEL] = glob_nfm_sql_lelel;
	buff [DSPCTL_OFFSET_NFMSQLOFF] = glob_nfm_sql_off;
	buff [DSPCTL_OFFSET_CWEDGETIME] = glob_cwedgetime;
	buff [DSPCTL_OFFSET_SIDETONELVL] = glob_sidetonelevel;

	buff [DSPCTL_OFFSET_NOTCH_ON] = glob_notch_on;
	buff [DSPCTL_OFFSET_NOTCH_WIDTH_HI] = glob_notch_width >> 8;
	buff [DSPCTL_OFFSET_NOTCH_WIDTH_LO] = glob_notch_width >> 0;
	buff [DSPCTL_OFFSET_NOTCH_FREQ_HI] = glob_notch_freq >> 8;
	buff [DSPCTL_OFFSET_NOTCH_FREQ_LO] = glob_notch_freq >> 0;
	buff [DSPCTL_OFFSET_LO6_FREQ_HI] = glob_lo6A >> 8;
	buff [DSPCTL_OFFSET_LO6_FREQ_LO] = glob_lo6A >> 0;
	buff [DSPCTL_OFFSET_FULLBW6_HI] = glob_fullbw6 >> 8;
	buff [DSPCTL_OFFSET_FULLBW6_LO] = glob_fullbw6 >> 0;

	buff [DSPCTL_OFFSET_LOWCUTRX_HI] = glob_aflowcutrx >> 8;
	buff [DSPCTL_OFFSET_LOWCUTRX_LO] = glob_aflowcutrx >> 0;
	buff [DSPCTL_OFFSET_HIGHCUTRX_HI] = glob_afhighcutrx >> 8;
	buff [DSPCTL_OFFSET_HIGHCUTRX_LO] = glob_afhighcutrx >> 0;

	buff [DSPCTL_OFFSET_LOWCUTTX_HI] = glob_aflowcuttx >> 8;
	buff [DSPCTL_OFFSET_LOWCUTTX_LO] = glob_aflowcuttx >> 0;
	buff [DSPCTL_OFFSET_HIGHCUTTX_HI] = glob_afhighcuttx >> 8;
	buff [DSPCTL_OFFSET_HIGHCUTTX_LO] = glob_afhighcuttx >> 0;

	buff [DSPCTL_OFFSET_DIGIGAINMAX] = glob_digigainmax;

	//buff [DSPCTL_OFFSET_FLTSOFTER] = glob_fltsofter;
	buff [DSPCTL_OFFSET_AMDEPTH] = glob_amdepth;
	buff [DSPCTL_OFFSET_MIKEAGC] = glob_mikeagc;
	buff [DSPCTL_OFFSET_MIKEAGCSCALE] = glob_mikeagcscale;

	spi_select(target, DSPREG_SPIMODE);
	prog_spi_send_frame(target, buff, sizeof buff / sizeof buff [0]);
	spi_unselect(target);
}

/* 1/4 FS (12 kHz) ��� 0 ��� DSP */
int_fast32_t
dsp_get_ifreq(void)
{
	return DEFAULT_DSP_IF;		/* ��� ������� �� �������� DSP ����� ������ �� ����� �����������. */
}

void 
prog_fltlreg(void)
{
}

#else

void 
prog_dsplreg(void)
{
}

void 
prog_fltlreg(void)
{
}

#endif /* WITHINTEGRATEDDSP */


void 
prog_codec1reg(void)
{
#if defined(CODEC1_TYPE)
	const codec1if_t * const ifc1 = board_getaudiocodecif();

	// also use glob_mik1level
	ifc1->setvolume(glob_afgain, glob_afmute, glob_loudspeaker_off);
	ifc1->setlineinput(glob_lineinput, glob_mikebust20db, glob_mik1level, glob_lineamp);
	ifc1->setprocparams(glob_mikeequal, glob_codec1_gains);	/* ��������� ��������� ����� � ��������� (���, ����������, ...) */

#endif /* defined(CODEC1_TYPE) */
}


static uint_fast8_t flag_dsp1reg;	/* ������� ����������� ������� ��������. ��������� ����� � �������� */
static uint_fast8_t flag_flt1reg;	/* ������� ����������� ������� ��������. ��������� ����� � �������� */
static uint_fast8_t flag_codec1reg;	/* ������� ����������� ������� ��������. ��������� ����� � �������� */

/* ������ ����� ��������������� �� ����������� ���������� � ������ WITHSPISLAVE */
void prog_dsplreg_update(void)
{	
	uint_fast8_t f;
#if WITHSPISLAVE
	disableIRQ();
	f = flag_dsp1reg;
	flag_dsp1reg = 0;
	enableIRQ();
#else /* WITHSPISLAVE */
	f = flag_dsp1reg;
	flag_dsp1reg = 0;
#endif /* WITHSPISLAVE */
	if (f != 0)
	{
		prog_dsplreg();
	}
}
/* ������ ����� ��������������� �� ����������� ���������� � ������ WITHSPISLAVE */
void prog_fltlreg_update(void)
{	
	uint_fast8_t f;
#if WITHSPISLAVE
	disableIRQ();
	f = flag_flt1reg;
	flag_flt1reg = 0;
	enableIRQ();
#else /* WITHSPISLAVE */
	f = flag_flt1reg;
	flag_flt1reg = 0;
#endif /* WITHSPISLAVE */
	if (f != 0)
	{
		prog_fltlreg();
	}
}

/* ������ ����� ��������������� �� ����������� ���������� � ������ WITHSPISLAVE */
void prog_codecreg_update(void)		// ��������� ���������� ��������� ����� ������
{
	uint_fast8_t f;
#if WITHSPISLAVE
	disableIRQ();
	f = flag_codec1reg;
	flag_codec1reg = 0;
	enableIRQ();
#else /* WITHSPISLAVE */
	f = flag_codec1reg;
	flag_codec1reg = 0;
#endif /* WITHSPISLAVE */
	if (f != 0)
	{
		prog_codec1reg();
	}
}

/* ������� ����� ���������� �� ����������� ���������� � ������ WITHSPISLAVE */
/* ��������� ������� �� ���������� �������� ���������� */
void
board_dsp1regchanged(void)
{
	flag_dsp1reg = 1; 
}
/* ������� ����� ���������� �� ����������� ���������� � ������ WITHSPISLAVE */
/* ��������� ������� �� ���������� �������� ���������� */
void
board_flt1regchanged(void)
{
#if WITHEXTERNALDDSP /* ������� ���������� ������� DSP ������. */
	// � ���� ������ ���������� � ������ �������
	flag_dsp1reg = 1; 
#else /* WITHEXTERNALDDSP */
	flag_flt1reg = 1; 
#endif /* WITHEXTERNALDDSP */
}
/* ������� ����� ���������� �� ����������� ���������� � ������ WITHSPISLAVE */
/* ��������� ������� �� ���������� �������� ���������� */
void
board_codec1regchanged(void)
{
	flag_codec1reg = 1; 
}

////////////////////////////////


/* �����, � �������� ��������� ��� ����������� ������. ��� ���������� ������������ ������ 0 */
void board_set_trxpath(uint_fast8_t v)
{
	glob_trxpath = v;
}


/*	��������, ��������������� full scale �� IF ADC */
void
board_set_fsadcpower10(int_fast16_t v)
{
	if (glob_fsadcpower10 != v)
	{
		glob_fsadcpower10 = v;
		board_dsp1regchanged();
	}
}

void
board_set_ifgain(uint_fast16_t v)
{
	if (glob_ifgain != v)
	{
		glob_ifgain = v;
		board_dsp1regchanged();
	}
}

void
board_set_agcrate(uint_fast8_t n)	/* �� n ������� ��������� �������� ������� 1 �� ���������. UINT8_MAX - "�������" ��� */
{
	if (glob_agcrate [glob_trxpath] != n)
	{
		glob_agcrate [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

void
board_set_agc_t1(uint_fast8_t n)	/* ���������� ��������� ��� */
{
	if (glob_agc_t1 [glob_trxpath] != n)
	{
		glob_agc_t1 [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

void
board_set_agc_t2(uint_fast8_t n)	/* ���������� ��������� ��� */
{
	if (glob_agc_t2 [glob_trxpath] != n)
	{
		glob_agc_t2 [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

void
board_set_agc_t4(uint_fast8_t n)	/* ���������� ��������� ��� */
{
	if (glob_agc_t4 [glob_trxpath] != n)
	{
		glob_agc_t4 [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

void
board_set_agc_thung(uint_fast8_t n)	/* ���������� ��������� ��� HUNG TIME */
{
	if (glob_agc_thung [glob_trxpath] != n)
	{
		glob_agc_thung [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

void 
board_set_nfm_sql_lelel(uint_fast8_t n)	/* ������� ���������� �������������� NFM */
{
	if (glob_nfm_sql_lelel != n)
	{
		glob_nfm_sql_lelel = n;
		board_dsp1regchanged();
	}
}

void 
board_set_squelch(uint_fast8_t n)	/* ������� ���������� �������������� */
{
	if (glob_squelch != n)
	{
		glob_squelch = n;
		board_dsp1regchanged();
	}
}

void 
board_set_nfm_sql_off(uint_fast8_t v)	/* ���������� �������������� NFM */
{
	const uint_fast8_t n = v != 0;
	if (glob_nfm_sql_off != n)
	{
		glob_nfm_sql_off = n;
		board_dsp1regchanged();
	}
}

void 
board_set_swapiq(uint_fast8_t v)	/* �������� ������� I � Q ������ � ������ RTS96 */
{
	const uint_fast8_t n = v != 0;
	if (glob_swapiq != n)
	{
		glob_swapiq = n;
		board_dsp1regchanged();
	}
}

void 
board_set_swaprts(uint_fast8_t v)	/* ���� ������������ ��������� �� Rafael Micro R820T - ��������� �������� ������� */
{
	const uint_fast8_t n = v != 0;
	if (glob_swaprts != n)
	{
		glob_swaprts = n;
		board_dsp1regchanged();
	}
}

void 
board_set_notch_freq(uint_fast16_t n)	/* ������� NOTCH ������� */
{
	if (glob_notch_freq != n)
	{
		glob_notch_freq = n;
		board_flt1regchanged();		// ��������� ���� ������� ������������ � audio_update();
	}
}

void 
board_set_notch_width(uint_fast16_t n)	/* ������ NOTCH ������� */
{
	if (glob_notch_width != n)
	{
		glob_notch_width = n;
		board_flt1regchanged();		// ��������� ���� ������� ������������ � audio_update();
	}
}

void 
board_set_notch_on(uint_fast8_t v)	/* ��������� NOTCH ������� */
{
	const uint_fast8_t n = v != 0;
	if (glob_notch_on != n)
	{
		glob_notch_on = n;
		board_flt1regchanged();		// ��������� ���� ������� ������������ � audio_update();
	}
}

void 
board_set_sidetonelevel(uint_fast8_t n)	/* ������� ������� ������������ � ��������� - 0%..100% */
{
	if (glob_sidetonelevel != n)
	{
		glob_sidetonelevel = n;
		board_dsp1regchanged();
	}
}

void 
board_set_subtonelevel(uint_fast8_t n)	/* ������� ������� CTCSS � ��������� - 0%..100% */
{
	if (glob_subtonelevel != n)
	{
		glob_subtonelevel = n;
		board_dsp1regchanged();
	}
}

void 
board_set_cwedgetime(uint_fast8_t n)	/* ����� ����������/����� ��������� ��������� ��� �������� - � 1 �� */
{
	if (glob_cwedgetime != n)
	{
		glob_cwedgetime = n;
		board_dsp1regchanged();
	}
}

void 
board_set_amdepth(uint_fast8_t n)	/* ������� ��������� � �� - 0..100% */
{
	if (glob_amdepth != n)
	{
		glob_amdepth = n;
		board_dsp1regchanged();
	}
}

void 
board_set_dacscale(uint_fast8_t n)	/* ������������� ��������� ������� � ��� ����������� - 0..100% */
{
	if (glob_dacscale != n)
	{
		glob_dacscale = n;
		board_dsp1regchanged();
	}
}

void 
board_set_mik1level(uint_fast16_t n)	/* �������� ������������ ��������� */
{
	if (glob_mik1level != n)
	{
		glob_mik1level = n;
		board_codec1regchanged();
	}
}

// �������� ��� ����������� ������ �� ������ �����-���
void
board_set_afgain(uint_fast16_t v)
{
	if (glob_afgain != v)
	{
		glob_afgain = v;
		board_codec1regchanged();
	}
}

// �������������� ��������� ������� ��� ��������
void
board_set_txaudio(uint_fast8_t v)
{
	if (glob_txaudio != v)
	{
		glob_txaudio = v;
		board_dsp1regchanged();
	}
}

// �����/������, A - main RX, B - sub RX
void board_set_mainsubrxmode(uint_fast8_t v)
{
	if (glob_mainsubrxmode != v)
	{
		glob_mainsubrxmode = v;
		board_dsp1regchanged();
	}
}

// �������� ��� ����������� ������ �� ����� �����-��� ��� ������ � LINE IN
void
board_set_lineamp(uint_fast16_t v)
{
	if (glob_lineamp != v)
	{
		glob_lineamp = v;
		board_codec1regchanged();
	}
}


// ��������� line input ������ ���������
void
board_set_lineinput(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_lineinput != v)
	{
		glob_lineinput = v;
		board_codec1regchanged();
	}
}

// ��������� ������������� �� ����������
void
board_set_mikebust20db(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_mikebust20db != v)
	{
		glob_mikebust20db = v;
		board_codec1regchanged();
	}
}

/* ��������� ����������� ��� ����� ����������� */
void
board_set_mikeagc(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_mikeagc != v)
	{
		glob_mikeagc = v;
		board_dsp1regchanged();
	}
}

/* �� ����� ����� (� ���������) �� ������ ��������� ��������� ��� ��������� */
void
board_set_mikeagcscale(uint_fast8_t v)
{
	if (glob_mikeagcscale != v)
	{
		glob_mikeagcscale = v;
		board_dsp1regchanged();
	}
}

/* ������������ �������� ��� ��������� */
void
board_set_mikeagcgain(uint_fast8_t v)
{
	if (glob_mikeagcgain != v)
	{
		glob_mikeagcgain = v;
		board_dsp1regchanged();
	}
}

/* ������������ � ������ ��������� */
void
board_set_mikehclip(uint_fast8_t v)
{
	if (glob_mikehclip != v)
	{
		glob_mikehclip = v;
		board_dsp1regchanged();
	}
}

/* ��������� ������ ����� - �� Samplerate/2 ��� ������ �� ������� �������  */
void
board_set_afresponcerx(int_fast8_t v)
{
	if (glob_afresponcerx != v)
	{
		glob_afresponcerx = v;
		board_flt1regchanged();
	}
}
/* ��������� ������ ����� - �� Samplerate/2 ��� ������ �� ������� �������  */
void
board_set_afresponcetx(int_fast8_t v)
{
	if (glob_afresponcetx != v)
	{
		glob_afresponcetx = v;
		board_flt1regchanged();
	}
}

#if defined(CODEC1_TYPE) && WITHAFCODEC1HAVEPROC
// ��������� ��������� ������� � ��������� (�������, ����������, ...)
void
board_set_mikeequal(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_mikeequal != v)
	{
		glob_mikeequal = v;
		board_codec1regchanged();
	}
}

// ���������� 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
void board_set_mikeequalparams(const uint_fast8_t * p)
{
	// 
	if (memcmp(glob_codec1_gains, p, sizeof glob_codec1_gains) != 0)
	{
		memcpy(glob_codec1_gains, p, sizeof glob_codec1_gains);
		board_codec1regchanged();
	}

}

#endif /* defined(CODEC1_TYPE) && WITHAFCODEC1HAVEPROC */

/* ��������� ���� � ��������� � ��������� */
void
board_set_afmute(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_afmute != v)
	{
		glob_afmute = v;
		board_codec1regchanged();
	}
}

// �������� ��� ��������� ������ ������ �������� A/����������� A
void
board_set_dspmode(uint_fast8_t v)
{
	if (glob_dspmodes [glob_trxpath] != v)
	{
		glob_dspmodes [glob_trxpath] = v;
		board_flt1regchanged();		// ��������� ���� ������� ������������ � audio_update();
	}
}

void board_set_lo6(int_fast32_t f)
{
	if (glob_lo6 [glob_trxpath] != f)
	{
		glob_lo6 [glob_trxpath] = f;
		board_flt1regchanged();
	}
}

/* ��������� ������� ����� �������� �� � ��������� ������ - �������� ������ ������ ����������� */
void board_set_fullbw6(int_fast16_t n)
{
	if (glob_fullbw6 [glob_trxpath] != n)
	{
		glob_fullbw6 [glob_trxpath] = n;
		board_flt1regchanged();	// ��������� ���� ������� ������������ � audio_update();
	}
}

#if 0
/* ��� ���������� ������������ ������ ������� �������� �������� �� ����� */
/* WITHFILTSOFTMIN..WITHFILTSOFTMAX */
void board_set_fltsofter(uint_fast8_t n)
{
	if (glob_fltsofter [glob_trxpath] != n)
	{
		glob_fltsofter [glob_trxpath] = n;
		board_flt1regchanged();	// ��������� ���� ������� ������������ � audio_update();
	}
}
#endif

void 
board_set_aflowcutrx(int_fast16_t n)	/* ������ ������� ����� ������� �� �� ������ */
{
	if (glob_aflowcutrx [glob_trxpath] != n)
	{
		glob_aflowcutrx [glob_trxpath] = n;
		board_flt1regchanged();	// ��������� ���� ������� ������������ � audio_update();
	}
}

void 
board_set_afhighcutrx(int_fast16_t n)	/* ������� ������� ����� ������� �� �� ������ */
{
	if (glob_afhighcutrx [glob_trxpath] != n)
	{
		glob_afhighcutrx [glob_trxpath] = n;
		board_flt1regchanged();	// ��������� ���� ������� ������������ � audio_update();
	}
}

void 
board_set_aflowcuttx(int_fast16_t n)	/* ������ ������� ����� ������� �� �� �������� */
{
	if (glob_aflowcuttx != n)
	{
		glob_aflowcuttx = n;
		board_flt1regchanged();	// ��������� ���� ������� ������������ � audio_update();
	}
}

void 
board_set_afhighcuttx(int_fast16_t n)	/* ������� ������� ����� ������� �� �� �������� */
{
	if (glob_afhighcuttx != n)
	{
		glob_afhighcuttx = n;
		board_flt1regchanged();	// ��������� ���� ������� ������������ � audio_update();
	}
}

/* �������� ������ ����������� ��������� �������� - ������������ �������� */
void
board_set_digigainmax(uint_fast8_t v)
{
	if (glob_digigainmax != v)
	{
		glob_digigainmax = v;
		board_dsp1regchanged();
	}
}

/* ���������� �� AD605 (���������� ��������� ������ �� */
void
board_set_gvad605(uint_fast8_t v)
{
	if (glob_gvad605 != v)
	{
		glob_gvad605 = v;
		board_dsp1regchanged();
	}
}

// �������� �������� � ��������� 1/100 ���
void
board_set_modem_speed100(uint_fast32_t v)
{
	if (glob_modem_speed100 != v)
	{
		glob_modem_speed100 = v;
		board_flt1regchanged();
	}
}

// ����������� ���������
void
board_set_modem_mode(uint_fast8_t v)
{
	if (glob_modem_mode != v)
	{
		glob_modem_mode = v;
		board_flt1regchanged();
	}
}

#if WITHSPISLAVE

// ���������� �� ���������� ��� ��������� ��������� ����� ������
void hardware_spi_slave_callback(uint8_t * buff, uint_fast8_t len)
{
	if (len == DSPCTL_BUFSIZE)
	{
		board_set_dspmodeA(buff [DSPCTL_OFFSET_MODEA]);
		board_set_dspmodeB(buff [DSPCTL_OFFSET_MODEB]);
		board_set_agc(buff [DSPCTL_OFFSET_AGCOFF] ? BOARD_AGCCODE_OFF : BOARD_AGCCODE_ON);
#if ! WITHPOTIFGAIN
		board_set_ifgain(buff [DSPCTL_OFFSET_IFGAIN_HI] * 256 + buff [DSPCTL_OFFSET_IFGAIN_LO]);
#endif /* ! WITHPOTIFGAIN */
#if ! WITHPOTAFGAIN
		board_set_afgain(buff [DSPCTL_OFFSET_AFGAIN_HI] * 256 + buff [DSPCTL_OFFSET_AFGAIN_LO]);
#endif /* ! WITHPOTAFGAIN */
		board_set_afmute(buff [DSPCTL_OFFSET_AFMUTE]);
		board_set_agc_t1(buff [DSPCTL_OFFSET_AGC_T1]);
		board_set_agc_t2(buff [DSPCTL_OFFSET_AGC_T2]);
		board_set_agc_t4(buff [DSPCTL_OFFSET_AGC_T4]);
		board_set_agc_thung(buff [DSPCTL_OFFSET_AGC_THUNG]);
		board_set_agcrate(buff [DSPCTL_OFFSET_AGCRATE]);	// �� n ������� ��������� �������� ������� 1 �� ���������. UINT8_MAX - "�������" ���
	
		board_set_mik1level(buff [DSPCTL_OFFSET_MICLEVEL_HI] * 256 + buff [DSPCTL_OFFSET_MICLEVEL_LO]);
		board_set_nfm_sql_lelel(buff [DSPCTL_OFFSET_NFMSQLLEVEL]);
		board_set_nfm_sql_off(buff [DSPCTL_OFFSET_NFMSQLOFF]);

		board_set_afhighcutrx(buff [DSPCTL_OFFSET_HIGHCUTRX_HI] * 256 + buff [DSPCTL_OFFSET_HIGHCUTRX_LO]);
		board_set_aflowcutrx(buff [DSPCTL_OFFSET_LOWCUTRX_HI] * 256 + buff [DSPCTL_OFFSET_LOWCUTRX_LO]);

		board_set_afhighcuttx(buff [DSPCTL_OFFSET_HIGHCUTTX_HI] * 256 + buff [DSPCTL_OFFSET_HIGHCUTTX_LO]);
		board_set_aflowcuttx(buff [DSPCTL_OFFSET_LOWCUTTX_HI] * 256 + buff [DSPCTL_OFFSET_LOWCUTTX_LO]);

		board_set_cwedgetime(buff [DSPCTL_OFFSET_CWEDGETIME]);
		board_set_sidetonelevel(buff [DSPCTL_OFFSET_SIDETONELVL]);

		board_set_notch_on(buff [DSPCTL_OFFSET_NOTCH_ON]);
		board_set_notch_width(buff [DSPCTL_OFFSET_NOTCH_WIDTH_HI] * 256 + buff [DSPCTL_OFFSET_NOTCH_WIDTH_LO]);
		board_set_notch_freq(buff [DSPCTL_OFFSET_NOTCH_FREQ_HI] * 256 + buff [DSPCTL_OFFSET_NOTCH_FREQ_LO]);
		board_set_lo6(buff [DSPCTL_OFFSET_LO6_FREQ_HI] * 256 + buff [DSPCTL_OFFSET_LO6_FREQ_LO]);
		board_set_fullbw6(buff [DSPCTL_OFFSET_FULLBW6_HI] * 256 + buff [DSPCTL_OFFSET_FULLBW6_LO]);
		board_set_digigainmax(buff [DSPCTL_OFFSET_DIGIGAINMAX]);
		//board_set_fltsofter(buff [DSPCTL_OFFSET_FLTSOFTER]);
		board_set_amdepth(buff [DSPCTL_OFFSET_AMDEPTH]);
		board_set_mikeagc(buff [DSPCTL_OFFSET_MIKEAGC]);
		board_set_mikeagcscale(buff [DSPCTL_OFFSET_MIKEAGCSCALE]);
	}
}

#endif /* WITHSPISLAVE */
