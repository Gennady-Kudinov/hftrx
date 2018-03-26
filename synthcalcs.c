/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "synthcalcs.h"
#include "board.h"
#include "spifuncs.h"

#include "display.h"
#include "formats.h"

//#define WITHSI5351AREPLACE 1

// LO1MODE_DIRECT
//	- ��������� DDS ��� ������������� ���� � ������� ����� ��� ������� ���� � ������ �����
//
// LO1MODE_HYBRID, 
// - ���� � ���������� ����������, � ������ �� DDS
//
// LO1MODE_FIXSCALE 
// - ���� � ������������� ���������� ����� �� DDS
//
// LO1MODE_MAXLAB, 
// �������� � MAXLAB_DACARRAY - ����������� ���������� ��� �������� ������������� ������� - �������� � �������
// - ���� � ������ �� ���������� ���������� � ������ ������� ���������� �� ���

// ����������� ����������, �� ������������� ����� - ����������������� ��� ���������
//#if defined(DDS1_TYPE) || defined(PLL1_TYPE)
	static const uint_fast8_t dds1refdiv = DDS1_CLK_DIV;
	static const uint_fast8_t dds1refmul = DDS1_CLK_MUL;
	static uint_fast32_t dds1ref; /* �������� ������� �� ����� DDS1 */
//#endif /* defined(DDS1_TYPE) */

#if defined(DDS2_TYPE)
	static const uint_fast8_t dds2refdiv = DDS2_CLK_DIV;
	static const uint_fast8_t dds2refmul = DDS2_CLK_MUL;
	static uint_fast32_t dds2ref; /* �������� ������� �� ����� DDS2 */
#else /* defined(DDS2_TYPE) */
	static const uint_fast8_t dds2refdiv = 1;
	static const uint_fast8_t dds2refmul = 1;
	static uint_fast32_t dds2ref; /* �������� ������� �� ����� DDS2 */
#endif /* defined(DDS2_TYPE) */

#if defined(DDS3_TYPE)
	static const uint_fast8_t dds3refdiv = DDS3_CLK_DIV;
	static const uint_fast8_t dds3refmul = DDS3_CLK_MUL;
	static uint_fast32_t dds3ref; /* �������� ������� �� ����� DDS3 */
#endif /* defined(DDS1_TYPE) */

#if defined(PLL1_TYPE)
	// ����������� ����������, �� ������������� ����� - ����������������� ��� ���������
	const phase_t r1_ph = SYNTH_R1;

#endif /* defined(PLL1_TYPE) */

#if LO1MODE_HYBRID
	static uint_fast64_t ph1_min; // = freq2ftw(ddsosc, cf - df / 2);	/* ���������� ����, ��������������� ����������� ������� �� ������ DDS */
#endif /* LO1MODE_HYBRID */


#if LO1MODE_MAXLAB
	//static uint_fast64_t ref1_min; // ������� �������� (����������� �������)
	static uint_fast64_t ref1_min_scaled; // ������� �������� (����������� �������) * MAXLAB_LINEARDACRANGE
	static uint_fast32_t df_maxlab;			// ������������ �������� ����������� �������� �� ���� ��� ����������
#endif /* LO1MODE_MAXLAB */

#ifdef FTW_RESOLUTION
/* ������� �������������� ��������� ������� � FTW ��� �������� �������� ������� DDS */
/* divider - ��� �������� ������� ������� ����� DDS, ���������� �� �������� �������� ������� �� DDS. */
static ftw_t 
freq2ftw(uint_fast32_t freq, uint_fast16_t divider, uint_fast64_t ddsosc)
{
	const uint_fast64_t d = (uint_fast64_t) divider << FTW_RESOLUTION;
	return (freq * d) / ddsosc;
}
#endif /* FTW_RESOLUTION */

#if WITHLFM && LO1MODE_DIRECT


volatile uint_fast8_t spool_lfm_enable;
volatile uint_fast8_t spool_lfm_flag;

void spool_lfm(void)
{
	if (spool_lfm_enable != 0)
	{
		prog_pulse_ioupdate();
		spool_lfm_flag = 1;
		//return 1;
	}
	//return 0;
}

// ���������� �� ����������� PPS ��� ���������� ������� ������.
void lfm_run(void)
{
	spool_lfm_enable = 1;
}

static void
board_waitextsync(void)
{

	for (;;)
	{
		uint_fast8_t f;
		disableIRQ();
		f = spool_lfm_flag;
		spool_lfm_flag = 0;
		enableIRQ();
		if (f != 0)
			break;	
	}
}


static void 
scanfreq(ftw_t ftw0, ftw_t ftw_last, long int lfm_nsteps)
{
	const ftw_t lfm_dy = ftw_last - ftw0; 
	const ftw_t lfm_SCALE = lfm_nsteps;
	const ftw_t lfm_SCALEDIV2 = lfm_nsteps / 2;
	ftw_t lfm_y1_scaled = ftw0 * lfm_SCALE;	// ��������� ��������
	long int lfm_position = 0;

	// ��������� ����������� �������, ��������������� ������ ������������ ���������
	// ����� ��� ������� �������� (� ������ Tstart+delta), ����������� �� ���������� ������������� � GPS

	prog_dds1_ftw(& ftw0);


	//board_set_ddsext(1);	// ������� ���������� IOUPDATE DDS1
	//board_update();

	spool_lfm_flag = 0;
	//spool_lfm_enable = 1;
	hardware_lfm_setupdatefreq(LFMTICKSFREQ);

	//display_freq(position, ftw0); // 
    do              
    {         
		// �������������� ��������� ��� ���������������� DDS
		const ftw_t lfm_value = ((lfm_y1_scaled += lfm_dy) + lfm_SCALEDIV2) / lfm_SCALE;
		// ����� FTW � ���������� ����������� �������, ��� ������� 
		// ���������� ����������������� ��������.
		board_waitextsync();
		disableIRQ();
		prog_dds1_ftw_noioupdate(& lfm_value);
		enableIRQ();
		//display_freq(position, value); // �� ��� ���� ����� �����
    } while (++ lfm_position < lfm_nsteps);
	// ���� ����������� ������� ��������

	hardware_lfm_setupdatefreq(20);
	spool_lfm_enable = 0;

	//board_set_ddsext(0);	// ���������� ���������� IOUPDATE DDS1
	//board_update();
} 

static long int lfm_speed;		// �������� ����������� ���� / �������
static long int lfm_start;	// ��������� �������
static long int lfm_stop;	// �������� �������
static long int lfm_lo1div;	// �������� ����� ������� �� ���������

void 
testlfm(void)
{
	enum { fi = 0 };	// ����� ������
	// LFMTICKSFREQ - ���������� ���������� ������� �� �������

	//lfm_speed = 100000;		// �������� ����������� ���� / �������
	//lfm_start = 2000000;	// ��������� �������
	//lfm_stop =  9000000;	// �������� �������

	long int nsteps = (uint_fast64_t) (lfm_stop - lfm_start) * LFMTICKSFREQ / lfm_speed;
	//printf("nsteps = %d\n", nsteps);
	scanfreq(freq2ftw(synth_freq2lo1(lfm_start, fi), dds1refdiv * lfm_lo1div, dds1ref), freq2ftw(synth_freq2lo1(lfm_stop, fi), dds1refdiv * lfm_lo1div, dds1ref), nsteps);
}

void synth_lfm_setparams(uint_fast32_t astart, uint_fast32_t astop, uint_fast32_t aspeed, uint_fast8_t od)
{
	lfm_speed = aspeed;
	lfm_start = astart;
	lfm_stop = astop;
	lfm_lo1div = od;
} 

#else

void spool_lfm(void)
{
	//return 0;
}
#endif

#if LO1MODE_DIRECT && ! defined(DDS1_TYPE)

#if defined(PLL1_TYPE)

/* ������������� � ������� ����������� - ������� �� PLL1_FRACTIONAL_LENGTH */

#if ! defined (PLL1_FRACTIONAL_LENGTH)
	#error PLL1_FRACTIONAL_LENGTH must be defibed
#endif /* ! defined (PLL1_FRACTIONAL_LENGTH) */

/* ������� �������������� ��������� ������� � FTW ��� �������� ������� ��������� */
/* divider - ��� �������� ������� ������� ����� DDS, ���������� �� �������� �������� ������� �� DDS. */
static ftw_t 
pll1_freq2fract(uint_fast32_t freq, uint_fast16_t divider, uint_fast32_t pllref)
{
	const uint_fast64_t d = (uint_fast64_t) divider << PLL1_FRACTIONAL_LENGTH;
	return (freq * d) / pllref;
}

/* 	���������� � ������� ��� ������������� �����.
 */
static void synth_integer1_setfreq(
	uint_fast32_t f,			/* �������, ������� ����� �������� �� ������ ������ ���� */
	uint_fast8_t om				/* ���������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	)
{
	const pllhint_t hint = board_pll1_get_hint(f / om);	/* ����� ��������� ������������ ��� � �������� ��� ������ ������� */
	const uint_fast16_t divider = board_pll1_get_divider(hint);	/* �������� �������� �� VCO */
	const ftw_t n1 = pll1_freq2fract(f, divider * r1_ph, dds1ref * om); /* ������� �� PLL1_FRACTIONAL_LENGTH */

	board_pll1_set_vco(hint);
	const uint_fast8_t fchange = board_pll1_set_n(& n1, hint, 1);
	if (fchange != 0)	// ������������� �������������������� - ��������� ������ PLL1
		board_pll1_set_n(& n1, hint, 0);
	board_pll1_set_vcodivider(hint);
}
#else
// stup ��� ������ ���� ���������� LO1MODE_DIRECT, �� �� ����� ����������
static void synth_integer1_setfreq(
	uint_fast32_t f,			/* �������, ������� ����� �������� �� ������ ������ ���� */
	uint_fast8_t om				/* ���������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	)
{
}
#endif

#elif LO1MODE_FIXSCALE

/* 	DDS ������ ������� ��������� ��� �������� ���������, 
 *	�� ��� ���������� �������� � ����. ������� SYNTH_N1
 *	�� DDS ���������� �������� � ����. ������� SYNTH_R1
 */
static void synth_scale1_setfreq(
	uint_fast32_t f,			/* �������, ������� ����� �������� �� ������ ������ ���� */
	uint_fast8_t om				/* ���������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	)
{
	const pllhint_t hint = board_pll1_get_hint(f / om);	/* ����� ��������� ������������ ��� � �������� ��� ������ ������� */
	const uint_fast16_t divider = board_pll1_get_divider(hint);		/* �������� �������� �� VCO */
	const ftw_t n1 = SYNTH_N1;		/* �� ������� ������� ������� ���������� � PLL */
	const ftw_t ph1 = freq2ftw(f, divider, dds1ref * (uint_fast64_t) SYNTH_N1 * om);    /* �������������� ��������� ������� � ���� */

	board_pll1_set_vco(hint);
	const uint_fast8_t fchange = board_pll1_set_n(& n1, hint, 1);
	prog_dds1_ftw(& ph1);
	if (fchange != 0)	// ������������� �������������������� - ��������� ������ PLL1
		board_pll1_set_n(& n1, hint, 0);
	board_pll1_set_vcodivider(hint);
}

#elif LO1MODE_MAXLAB

#if MAXLAB_DACARRAY

static uint_fast16_t dacvalues [MAXLAB_LINEARDACRANGE];

#endif /* MAXLAB_DACARRAY */

static uint_fast16_t ui16max(uint_fast16_t a, uint_fast16_t b)
{
	return a > b ? a : b;
}

static uint_fast16_t ui16min(uint_fast16_t a, uint_fast16_t b)
{
	return a < b ? a : b;
}


static uint_fast16_t
daci2dacval(
	uint_fast16_t i		// DAC value index 0 .. MAXLAB_LINEARDACRANGE-1
	)
{
	// ���������� ������� �������� ��� ������ ���
	// ������������� �� ������������ �������� ������������ ���� ������
	// ���� ������ ������ #1 ������, ��� � ������ #2
	// ������������� �� ����������� �������� ������������ ���� ������
	// ���� ������ ������ #1 ������, ��� � ������ #2

	uint_fast16_t dac_min1 = 100;	// U � ������ ������ #1
	uint_fast16_t dac_max1 = 500;	// U � ����� ������ #1
	uint_fast16_t dac_min2 = 5;	// U � ������ ������ #2
	uint_fast16_t dac_max2 = 1000;	// U � ����� ������ #2

	const uint_fast16_t delta1 = (dac_max1 - dac_min1);
	const uint_fast16_t delta2 = (dac_max2 - dac_min2);

	const uint_fast16_t v1 = (uint_fast32_t) delta1 * i / MAXLAB_LINEARDACRANGE + dac_min1;
	const uint_fast16_t v2 = (uint_fast32_t) delta2 * i / MAXLAB_LINEARDACRANGE + dac_min2;
	if (delta1 > delta2)
		return ui16max(v1, v2);
	else
		return ui16min(v1, v2);
}


static void
synth_maxlabloop1_setfreq(
	uint_fast32_t f,		/* �������, ������� ����� �������� �� ������ ������ ���� */
	uint_fast8_t om		/* ���������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	)
{
	// MAXLAB_LINEARDACRANGE: �� ������������� �������� �� 0 �� 1023

	const pllhint_t hint = board_pll1_get_hint(f / om);	/* ����� ��������� ������������ ��� � �������� ��� ������ ������� */
	// ������ ���������� ��� ��������� �������
	const uint_fast64_t fscaled = f * r1_ph * MAXLAB_LINEARDACRANGE / om;   /* �������������� ��������� ������� � ������������ (��� ��� REF DIVIDER = 1) */
	const uint_fast64_t n1 = fscaled / ref1_min_scaled;	/* �� ������� ������� ������� ���������� � PLL */
	const uint_fast64_t fref_scaled = fscaled / n1;
	const uint_fast16_t dacindex = (fref_scaled - ref1_min_scaled) / df_maxlab;

#if MAXLAB_DACARRAY
	const uint_fast16_t dacvalue = dacvalues [dacindex < MAXLAB_LINEARDACRANGE ? dacindex : (MAXLAB_LINEARDACRANGE - 1)];	// ������ �� �������� ���� ���������� �������� � ��������� �� ������� �������
#else
	const uint_fast16_t dacvalue = daci2dacval(dacindex < MAXLAB_LINEARDACRANGE ? dacindex : (MAXLAB_LINEARDACRANGE - 1));	// ������ �� �������� ���� ���������� �������� � ��������� �� ������� �������
#endif
	// ��������� ����������� ��������
	board_pll1_set_vco(hint);
	const uint_fast8_t fchange = board_pll1_set_n(& n1, hint, 1);
	board_set_maxlabdac(dacvalue);
	if (fchange != 0)	// ������������� �������������������� - ��������� ������ PLL1
		board_pll1_set_n(& n1, hint, 0);
	board_pll1_set_vcodivider(hint);

#if WITHDEBUG
	const long fref2 = fref_scaled * 100 / MAXLAB_LINEARDACRANGE;
	debug_printf_P(PSTR("LM7001: fout=%ld.%03ld n1=%d DACi=%-4u DAC=%-4u (ref=%ld.%05ld)\n"), f / 1000, f % 1000, (int) n1, dacindex, dacvalue, fref2 / 100000, fref2 % 100000); 
#endif /* WITHDEBUG */
}

#elif LO1MODE_DIRECT

static void synth_direct1_setfreq(
	uint_fast8_t pathi,	/* ����� ������ - 0/1: main/sub */
	uint_fast32_t f,	/* �������, ������� ����� �������� �� ������ DDS */
	uint_fast8_t od,	/* �������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	uint_fast8_t om		/* ���������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	)
{
	//debug_printf_P(PSTR("synth_direct1_setfreq: pathi=%u, freq=%lu\n"), pathi, f);
#if defined(DDS1_TYPE)
	const ftw_t ph1 = freq2ftw(f, dds1refdiv * od, dds1ref * (uint_fast64_t) om);    /* �������������� ��������� ������� � ���� */
	switch (pathi)
	{
	case 0:
		prog_dds1_ftw(& ph1);
		break;
	case 1:
		prog_dds1_ftw_sub(& ph1);
		break;
	case 2:
		prog_dds1_ftw_sub3(& ph1);
		break;
	case 3:
		prog_dds1_ftw_sub4(& ph1);
		break;
	}
#endif
}

#elif LO1MODE_HYBRID && defined(DDS1_TYPE)

static void synth_loop1_setfreq(
	uint_fast8_t pathi,		/* ����� ������ - 0/1: main/sub */
	uint_fast32_t f,		/* �������, ������� ����� �������� �� ������ ������ ���� */
	uint_fast8_t om			/* ���������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	)
{
	const pllhint_t hint = board_pll1_get_hint(f / om);
	const uint_fast16_t divider = board_pll1_get_divider(hint);	/* �������� �������� �� VCO */
	const ftw_t t = freq2ftw(f, divider * dds1refdiv * SYNTH_R1, dds1ref * om);   /* �������������� ��������� ������� � ������������ ���� */
	const ftw_t n1 = (t / ph1_min);		/* �� ������� ������� ������� ���������� � PLL */
	const ftw_t ph1a = t / n1;		/* FTW ��� DDS */
	//debug_printf_P(PSTR("synth_loop1_setfreq: f=%ld. n1=%ld, hint=%d, R1=%d\n"), (long) f, (long) n1, (int) hint, (int) SYNTH_R1);

	board_pll1_set_vco(hint);
	const uint_fast8_t fchange = board_pll1_set_n(& n1, hint, 1);
	prog_dds1_ftw(& ph1a);
	if (fchange != 0)	// ������������� �������������������� - ��������� ������ PLL1
		board_pll1_set_n(& n1, hint, 0);
	board_pll1_set_vcodivider(hint);
}
#endif


/* ������� ���������� ��� ������� ���������� - 
   �� ������� ���������� ������� ����� ����������� ����� ������� �� ��������� 
 */
static uint_fast8_t
board_getscalelo1(
	uint_fast32_t f
	)
{
#if CTLSTYLE_SW2016VHF
	// ������������ FQMODEL_45_IF8868_UHF144
	return 1;
#elif \
	FQMODEL_45_IF8868_UHF144 || FQMODEL_45_IF6000_UHF144 || \
	FQMODEL_45_IF8868_UHF430 || FQMODEL_45_IF6000_UHF430
	// � ������ ��������� ���������� ����������� ������� ������� ���������
	if (f >= 250000000UL)
		return 8;
	if (f >= 85000000UL)
		return 2;
	return 1;

#else

	return 1;

#endif /* CTLSTYLE_SW2011 */
}

#if WITHSI5351AREPLACE
	#include "chip/si5351a.h"
#endif /* WITHSI5351AREPLACE */

void synth_lo1_setfrequ(
	uint_fast8_t pathi,		/* ����� ������ - 0/1: main/sub */
	uint_fast32_t f,		/* �������, ������� ����� �������� �� ������ LO1 */
	uint_fast8_t od			/* �������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	)
{
	const uint_fast8_t om = board_getscalelo1(f * od);	/* ������� ���������� ��� ������� ���������� - �� ������� ���������� ������� ����� ����������� ����� ������� �� ��������� */
	board_set_scalelo1(om);

#if WITHSI5351AREPLACE
	si5351aSetFrequencyA(f * od / om);
	return;
#endif /* WITHSI5351AREPLACE */

#if LO1MODE_DIRECT && ! defined(DDS1_TYPE)
	synth_integer1_setfreq(f * od, om);
#elif LO1MODE_DIRECT
	synth_direct1_setfreq(pathi, f, od, om);
#elif LO1MODE_HYBRID
	synth_loop1_setfreq(pathi, f * od, om);
#elif LO1MODE_MAXLAB
	synth_maxlabloop1_setfreq(pathi, f * od, om);
#elif LO1MODE_FIXSCALE
	synth_scale1_setfreq(pathi, f * od, om);
#else
	#error Select proper LO1MODE_XXX define!
#endif
}

/* ��������� ������� ������� ����������, ��������� ��� ������ */
void synth_lo1_setfreq(
	uint_fast8_t pathi,		/* ����� ������ - 0/1: main/sub */
	int_fast32_t f,			/* �������, ������� ����� �������� �� ������ LO1 */
	uint_fast8_t od			/* �������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	)
{
	if (f < 0)
		f = - f;
	synth_lo1_setfrequ(pathi, f, od);
}

void synth_lo4_setfreq(
	uint_fast8_t pathi,		/* ����� ������ - 0/1: main/sub */
	int_fast32_t f,			/* �������, ������� ����� �������� �� ������ DDS */
	uint_fast8_t od,		/* �������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	int_fast8_t enable
	)
{
	if (enable == 0)
		f = 0;
	else if (f < 0)
		f = - f;

#if WITHSI5351AREPLACE
	si5351aSetFrequencyB(f);
	return;
#endif /* WITHSI5351AREPLACE */

#if defined(DDS2_TYPE)
	ftw_t ph = freq2ftw(f, dds2refdiv * od, dds2ref);    /* �������������� ��������� ������� � ���� */
	prog_dds2_ftw(& ph);
#endif
}


// ��������� ������� ������� ���������� � ������ ������������� DDS3 ad9951
void synth_lo2_setfreq(
	uint_fast8_t pathi,		/* ����� ������ - 0/1: main/sub */
	int_fast32_t f,			/* �������, ������� ����� �������� �� ������ DDS */
	uint_fast8_t od			/* �������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	)
{
	if (f < 0)
		f = - f;
#if LO2_DDSGENERATED
	#if defined(DDS3_TYPE)
		const ftw_t ph = freq2ftw(f, dds3refdiv * od, dds3ref);    /* �������������� ��������� ������� � ���� */
		prog_dds3_ftw(& ph);
	#else
		(void) od;
	#endif
#endif /* LO2_DDSGENERATED */
}

// ��������� ������� ����������, ��������������� passband tuning - PBT
void synth_lo3_setfreq(
	uint_fast8_t pathi,		/* ����� ������ - 0/1: main/sub */
	int_fast32_t f,			/* �������, ������� ����� �������� �� ������ DDS */
	uint_fast8_t od			/* �������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	)
{
	if (f < 0)
		f = - f;
#if ! LO2_DDSGENERATED
	#if defined(DDS3_TYPE)
		const ftw_t ph = freq2ftw(f, dds3refdiv * od, dds3ref);    /* �������������� ��������� ������� � ���� */
		prog_dds3_ftw(& ph);
	#else
		(void) od;
	#endif
#endif /* ! LO2_DDSGENERATED */
}


// ��������� ����������� ������� ����������� ����������
void synth_rts1_setfreq(
	uint_fast8_t pathi,		/* ����� ������ - 0/1: main/sub */
	int_fast32_t f	/* �������, ������� ����� �������� � ������ �������� */
	)
{
	if (pathi == 0)
	{
		/* ���� ������ ���� �������� �������� */
		if (f < 0)
			f = - f;
		//debug_printf_P(PSTR("synth_rts1_setfreq: freq=%lu\n"), f);
	#if (WITHRTS192 || WITHRTS96) && WITHDSPEXTDDC
		const ftw_t ph = freq2ftw(f, dds1refdiv, dds1ref);    /* �������������� ��������� ������� � ���� */
		prog_rts1_ftw(& ph);
	#endif /* (WITHRTS192 || WITHRTS96) && WITHDSPEXTDDC */
	}
}

// ��������� ������� ����������
void synth_lo0_setfreq(
	int_fast32_t f,	/* �������, ������� ����� �������� �� ������ DDS */
	uint_fast8_t enable		/* ����� ������ - 0/1: main/sub */
	)
{
	if (f < 0)
		f = - f;
#if CTLSTYLE_OLEG4Z_V1
	prog_xvtr_freq(f, enable);
#endif /* CTLSTYLE_OLEG4Z_V1 */
}

// ��������� ������� ������� ��� ������������ LO1.
void
synth_lo1_setreference(
	uint_fast32_t refclock
	)
{
//#if defined(DDS1_TYPE) || defined(PLL1_TYPE)
	dds1ref =  refclock * dds1refmul;   /* �������� ������� �� ����� DDS1 */
//#endif

#if LO1MODE_HYBRID

	const long freqlow = SYNTHLOWFREQ;	/* ����������� �������� ������� ����������� */
	/* ��������� ����������� - ������������� ���� ��� � ������� �� ��������� ������� ������, ������� � ������� ��������� */
	//r1_ph = SYNTH_R1;			/* �������� ����� DDS - ���������� ������� ����� ��� PLL */
	//freqlow = SYNTHLOWFREQ;	/* ����������� �������� ������� ����������� */


    const int_fast64_t cf =   SYNTH_CFCENTER;   /* ����� ������ ����������� ������� ����� DDS */
    const int_fast64_t df =   (ftw_t) (((ftw_t) cf * cf) / (freqlow * r1_ph));   /* ��������� ������ ����������� ������� ����� DDS */
    ph1_min = freq2ftw(cf - df / 2, dds1refdiv, dds1ref);	/* ����, ��������������� ����������� ������� �� ������ DDS */

#elif LO1MODE_MAXLAB

	// ������ ���������� ��� �������� ������� ������� (����������� ������� ��������������� ���������� ����������).
	// �������� ���� ��� ��� ������������� ��� ���������� ������� �� ����.
	const long freqlow = SYNTHLOWFREQ;	/* ����������� �������� ������� ����������� */

	uint_fast64_t ref1_min = refclock;	 // ������� ��������
	ref1_min_scaled = (uint_fast64_t) refclock * MAXLAB_LINEARDACRANGE;
    df_maxlab = ((uint_fast64_t) ref1_min * ref1_min) / (freqlow * r1_ph);   /* ��������� ������ ����������� ������� ����� DDS */
#if WITHDEBUG
	const long df2 = df_maxlab * 100;
	debug_printf_P(PSTR("LM7001: df=%ld.%05ld\n"), df2 / 100000, df2 % 100000); 
#endif /* WITHDEBUG */

	// ���������� ������� �������� ��� ������ ���
	// ������������� �� ������������ �������� ������������ ���� ������
	// ���� ������ ������ #1 ������, ��� � ������ #2
	// ������������� �� ����������� �������� ������������ ���� ������
	// ���� ������ ������ #1 ������, ��� � ������ #2
#if MAXLAB_DACARRAY

	uint_fast16_t i;
	for (i = 0; i < MAXLAB_LINEARDACRANGE; ++ i)
	{
		dacvalues [i] = daci2dacval(i);
	}

#endif

#elif LO1MODE_FIXSCALE

	// do nothing

#elif LO1MODE_DIRECT

	// do nothing

#elif LO1MODE_DIRECT && ! defined(DDS1_TYPE)

	// do nothing

#endif

}

// ��������� ������� ������� ��� ���� ������������, ����� LO1.
void synth_setreference(
	uint_fast32_t refclock 	/* ������� �������� */
	)
{

//#if defined(DDS2_TYPE)
	dds2ref =  refclock * dds2refmul;   /* �������� ������� �� ����� DDS2 */
//#endif

#if defined(DDS3_TYPE)
	dds3ref =  refclock * dds3refmul;   /* �������� ������� �� ����� DDS3 */
#endif
}

void synthcalc_init(void)
{
#if WITHSI5351AREPLACE
	si5351aInitialize();
#endif /* WITHSI5351AREPLACE */
#if LO1MODE_DIRECT && ! defined(DDS1_TYPE)
#elif LO1MODE_HYBRID
#elif LO1MODE_MAXLAB
#elif LO1MODE_FIXSCALE
#elif LO1MODE_DIRECT
#endif
}
