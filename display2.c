/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "audio.h"

#include "display.h"
#include "formats.h"

#include <string.h>
#include <math.h>

// todo: switch off -Wunused-function

#if WITHDIRECTFREQENER
struct editfreq
{
	uint_fast32_t freq;
	uint_fast8_t blinkpos;		// ������� (������� 10) �������������� �������
	uint_fast8_t blinkstate;	// � ����� �������������� ������� ������������ ������������� (0 - ������)
};
#endif /* WITHDIRECTFREQENER */

// ������������ ������ ������� ��� ������������ �����������
// ������� ��� ��������
static void dsp_latchwaterfall(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	);
static void display2_waterfallbg(
	uint_fast8_t x0, 
	uint_fast8_t y0, 
	void * pv
	);

static void display2_spectrum(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	);

static void display2_waterfall(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	);

// ����������� ����� S-����� � ������ �����������
static void display2_legend(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	);

//#define WIDEFREQ (TUNE_TOP > 100000000L)

static const FLASHMEM int32_t vals10 [] =
{
	1000000000UL,
	100000000UL,
	10000000UL,
	1000000UL,
	100000UL,
	10000UL,
	1000UL,
	100UL,
	10UL,
	1UL,
};

// ����������� ���� � ���� "������� ����" - ��������� �������� ������� ��������� ��������.
static void 
NOINLINEAT
display_value_big(
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// �������, ��� ������ ������ ��������
	uint_fast8_t blinkstate,	// 0 - ������, 1 - ������
	uint_fast8_t withhalf,		// 0 - ������ ������� �����
	uint_fast8_t lowhalf		// lower half
	)
{
	//const uint_fast8_t comma2 = comma + 3;		// comma position (from right, inside width)
	const uint_fast8_t j = (sizeof vals10 /sizeof vals10 [0]) - rj;
	uint_fast8_t i = (j - width);
	uint_fast8_t z = 1;	// only zeroes
	uint_fast8_t half = 0;	// ���������� ����� ������ ������� - ��������� �������

	display_wrdatabig_begin();
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);		// ���������� ������� �������� ������� �� �����������

		// ����������� �������� ��������
		if (comma2 == g)
		{
			display_put_char_big((z == 0) ? '.' : '#', lowhalf);	// '#' - ����� ������. ����� ������ �����
		}
		else if (comma == g)
		{
			z = 0;
			half = withhalf;
			display_put_char_big('.', lowhalf);
		}

		if (blinkpos == g)
		{
			const uint_fast8_t bc = blinkstate ? '_' : ' ';
			// ��� ������� �������������� �������. ������ �� �� �������� ��� ����
			z = 0;
			if (half)
				display_put_char_half(bc, lowhalf);

			else
				display_put_char_big(bc, lowhalf);
		}
		else if (z == 1 && (i + 1) < j && res.quot == 0)
			display_put_char_big(' ', lowhalf);	// supress zero
		else
		{
			z = 0;
			if (half)
				display_put_char_half('0' + res.quot, lowhalf);

			else
				display_put_char_big('0' + res.quot, lowhalf);
		}
		freq = res.rem;
	}
	display_wrdatabig_end();
}


static void
NOINLINEAT
display_value_small(
	int_fast32_t freq,
	uint_fast8_t width,	// full width (if >= 128 - display with sign)
	uint_fast8_t comma,		// comma position (from right, inside width)
	uint_fast8_t comma2,
	uint_fast8_t rj,		// right truncated
	uint_fast8_t lowhalf
	)
{
	const uint_fast8_t wsign = (width & WSIGNFLAG) != 0;
	const uint_fast8_t wminus = (width & WMINUSFLAG) != 0;
	const uint_fast8_t j = (sizeof vals10 /sizeof vals10 [0]) - rj;
	uint_fast8_t i = j - (width & WWIDTHFLAG);	// ����� ����� �� �������
	uint_fast8_t z = 1;	// only zeroes

	display_wrdata_begin();
	if (wsign || wminus)
	{
		// ����������� �� ������.
		z = 0;
		if (freq < 0)
		{
			display_put_char_small('-', lowhalf);
			freq = - freq;
		}
		else if (wsign)
			display_put_char_small('+', lowhalf);
		else
			display_put_char_small(' ', lowhalf);
	}
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);
		// ����������� �������� ��������
		if (comma2 == g)
		{
			display_put_char_small((z == 0) ? '.' : ' ', lowhalf);
		}
		else if (comma == g)
		{
			z = 0;
			display_put_char_small('.', lowhalf);
		}

		if (z == 1 && (i + 1) < j && res.quot == 0)
			display_put_char_small(' ', lowhalf);	// supress zero
		else
		{
			z = 0;
			display_put_char_small('0' + res.quot, lowhalf);
		}
		freq = res.rem;
	}
	display_wrdata_end();
}


// ����������� �������. ����� ��� �� ������� �������.
static void display_freqXbig_a(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	display_setcolors3(BIGCOLOR, BGCOLOR, BIGCOLORHALF);
	if (pv != NULL)
	{
#if WITHDIRECTFREQENER
		const struct editfreq * const efp = (const struct editfreq *) pv;


		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_gotoxy(x, y + lowhalf);
			display_value_big(efp->freq, fullwidth, comma, comma + 3, rj, efp->blinkpos, efp->blinkstate, 0, lowhalf);	// ������������ ������� ����� ������
		} while (lowhalf --);
#endif /* WITHDIRECTFREQENER */
	}
	else
	{
		enum { blinkpos = 255, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_a();

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_gotoxy(x, y + lowhalf);
			display_value_big(freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 0, lowhalf);	// ������������ ������� ����� ������
		} while (lowhalf --);
	}
}

// ����������� �������. ����� ��������� �������.
static void display_freqX_a(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	display_setcolors3(BIGCOLOR, BGCOLOR, BIGCOLORHALF);
	if (pv != NULL)
	{
#if WITHDIRECTFREQENER
		const struct editfreq * const efp = (const struct editfreq *) pv;

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_gotoxy(x, y + lowhalf);
			display_value_big(efp->freq, fullwidth, comma, comma + 3, rj, efp->blinkpos, efp->blinkstate, 1, lowhalf);	// ������������ ������� ����� ������
		} while (lowhalf --);
#endif /* WITHDIRECTFREQENER */
	}
	else
	{
		enum { blinkpos = 255, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_a();

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_gotoxy(x, y + lowhalf);
			display_value_big(freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 1, lowhalf);	// ������������ ������� ����� ������
		} while (lowhalf --);
	}
}

// ������� ����������� ��� ����� ����� ����������� � ������� �������� (��� ��������� ���������)
// FREQ B
static void display_freqchr_a(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	display_setcolors3(BIGCOLOR, BGCOLOR, BIGCOLORHALF);
	if (pv != NULL)
	{
#if WITHDIRECTFREQENER
		const struct editfreq * const efp = (const struct editfreq *) pv;

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_gotoxy(x, y + lowhalf);
			display_value_big(efp->freq, fullwidth, comma, 255, rj, efp->blinkpos, efp->blinkstate, 1, lowhalf);	// ������������ ������� ����� ������
		} while (lowhalf --);
#endif /* WITHDIRECTFREQENER */
	}
	else
	{
		enum { blinkpos = 255, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_a();

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_gotoxy(x, y + lowhalf);
			display_value_big(freq, fullwidth, comma, 255, rj, blinkpos, blinkstate, 1, lowhalf);	// ������������ ������� ����� ������
		} while (lowhalf --);
	}
}

// ������� ����������� ��� ����� ����� ����������� � ������� �������� (��� ��������� ���������)
// FREQ B
static void display_freqchr_b(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	display_setcolors(BIGCOLOR, BGCOLOR);
#if 0
	if (pv != NULL)
	{
#if WITHDIRECTFREQENER
		const struct editfreq * const efp = (const struct editfreq *) pv;

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_gotoxy(x, y + lowhalf);
			display_value_big(efp->freq, fullwidth, comma, 255, rj, efp->blinkpos, efp->blinkstate, 1, lowhalf);	// ������������ ������� ����� ������
		} while (lowhalf --);
#endif /* WITHDIRECTFREQENER */
	}
	else
#endif
	{
		enum { blinkpos = 255, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_b();

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_gotoxy(x, y + lowhalf);
			display_value_big(freq, fullwidth, comma, 255, 1, blinkpos, blinkstate, 1, lowhalf);	// ������������ ������� ����� ������
		} while (lowhalf --);
	}
}

static void display_freqX_b(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	const uint_fast32_t freq = hamradio_get_freq_b();

	display_setcolors(FRQCOLOR, BGCOLOR);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_gotoxy(x, y + lowhalf);
		display_value_small(freq, fullwidth, comma, comma + 3, rj, lowhalf);
	} while (lowhalf --);
}

// ���������� ������� ���������� ������� �������
static void display_freqmeter10(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHFQMETER
	char buffer [32];
	local_snprintf_P(buffer, sizeof buffer / sizeof buffer [0], PSTR("%10lu"), board_get_fqmeter());

	display_setcolors(FRQCOLOR, BGCOLOR);
	display_at(x, y, buffer);
#endif /* WITHFQMETER */
}


// ����������� ������ (�� FLASH) � ���������� �� ���������
static void 
NOINLINEAT
display2_text_P(
	uint_fast8_t x, 
	uint_fast8_t y, 
	const FLASHMEM char * const * labels,	// ������ ���������� �� �����
	const COLOR_T * colorsfg,			// ������ ������ face
	const COLOR_T * colorsbg,			// ������ ������ background
	uint_fast8_t state
	)
{
	#if LCDMODE_COLORED
	#else /* LCDMODE_COLORED */
	#endif /* LCDMODE_COLORED */

	display_setcolors(colorsfg [state], colorsbg [state]);
	display_at_P(x, y, labels [state]);
}

// ����������� ������ � ���������� �� ���������
static void 
NOINLINEAT
display2_text(
	uint_fast8_t x, 
	uint_fast8_t y, 
	const char * const * labels,	// ������ ���������� �� �����
	const COLOR_T * colorsfg,			// ������ ������ face
	const COLOR_T * colorsbg,			// ������ ������ background
	uint_fast8_t state
	)
{
	#if LCDMODE_COLORED
	#else /* LCDMODE_COLORED */
	#endif /* LCDMODE_COLORED */

	display_setcolors(colorsfg [state], colorsbg [state]);
	display_at(x, y, labels [state]);
}

// ����������� ������� TX / RX
static void display_txrxstatecompact(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHTX
	const uint_fast8_t tx = hamradio_get_tx();
	display_setcolors(TXRXMODECOLOR, tx ? MODECOLORBG_TX : MODECOLORBG_RX);
	display_at_P(x, y, tx ? PSTR("T") : PSTR(" "));
#endif /* WITHTX */
}

// todo: ������ LCDMODE_COLORED

// ��������� ����������� ��������� �����/��������
static const COLOR_T colorsfg_2alarm [2] = { COLOR_BLACK, COLOR_BLACK, };
static const COLOR_T colorsbg_2alarm [2] = { COLOR_GREEN, COLOR_RED, };

// ��������� ����������� ��������� �� ���� ���������
static const COLOR_T colorsfg_4state [4] = { COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, };
static const COLOR_T colorsbg_4state [4] = { COLOR_GREEN, COLOR_DARKGREEN, COLOR_DARKGREEN, COLOR_DARKGREEN, };

// ��������� ����������� ��������� �� ���� ���������
static const COLOR_T colorsfg_2state [2] = { COLOR_BLACK, COLOR_WHITE, };
static const COLOR_T colorsbg_2state [2] = { COLOR_GREEN, COLOR_DARKGREEN, };

// ��������� ����������� ������� ��� ���������
static const COLOR_T colorsfg_1state [1] = { COLOR_GREEN, };
static const COLOR_T colorsbg_1state [1] = { COLOR_BLACK, };	// ��������������� � ���� ���� �� �������

// ��������� ����������� ������� ��� ���������
static const COLOR_T colorsfg_1gold [1] = { COLOR_YELLOW, };
static const COLOR_T colorsbg_1gold [1] = { COLOR_BLACK, };	// ��������������� � ���� ���� �� �������

// ����������� ������� TX / RX
static void display_txrxstate2(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHTX
	const uint_fast8_t state = hamradio_get_tx();

	static const FLASHMEM char text0 [] = "RX";
	static const FLASHMEM char text1 [] = "TX";
	const FLASHMEM char * const labels [2] = { text0, text1 };
	display2_text_P(x, y, labels, colorsfg_2alarm, colorsbg_2alarm, state);
#endif /* WITHTX */
}

static void display_recstatus(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHUSEAUDIOREC
	unsigned long hamradio_get_recdropped(void);
	int hamradio_get_recdbuffered(void);

	char buffer [12];
	local_snprintf_P(
		buffer,
		sizeof buffer / sizeof buffer [0],
		PSTR("%08lx %2d"), 
		(unsigned long) hamradio_get_recdropped(),
		(int) hamradio_get_recdbuffered()
		);
		
	display_setcolors(MODECOLOR, BGCOLOR);
	display_at(x, y, buffer);

#endif /* WITHUSEAUDIOREC */
}
// ����������� ������ ������ ����� ���������
static void display_rec3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHUSEAUDIOREC

	const uint_fast8_t state = hamradio_get_rec_value();

	static const FLASHMEM char text_pau [] = "PAU";
	static const FLASHMEM char text_rec [] = "REC";
	const FLASHMEM char * const labels [2] = { text_pau, text_rec };
	display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);

#endif /* WITHUSEAUDIOREC */
}

// ����������� ��������� USB HOST
static void display_usb1(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if defined (WITHUSBHW_HOST)
	const uint_fast8_t active = hamradio_get_usbh_active();
	#if LCDMODE_COLORED
		display_setcolors(TXRXMODECOLOR, active ? MODECOLORBG_TX : MODECOLORBG_RX);
		display_at_P(x, y, PSTR("U"));
	#else /* LCDMODE_COLORED */
		display_at_P(x, y, active ? PSTR("U") : PSTR("X"));
	#endif /* LCDMODE_COLORED */
#endif /* defined (WITHUSBHW_HOST) */
}

// ����������� ������ NOCH ON/OFF
static void display_notch5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	static const FLASHMEM char text_nch [] = "NOTCH";
	static const FLASHMEM char text_nul [] = "     ";
	#if LCDMODE_COLORED
		const FLASHMEM char * const labels [2] = { text_nch, text_nch, };
	#else /* LCDMODE_COLORED */
		const FLASHMEM char * const labels [2] = { text_nul, text_nch, };
	#endif /* LCDMODE_COLORED */
	display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}

// ����������� ������ NOCH ON/OFF
static void display_notch3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	static const FLASHMEM char text_nch [] = "NCH";
	static const FLASHMEM char text_nul [] = "   ";
	#if LCDMODE_COLORED
		const FLASHMEM char * const labels [2] = { text_nch, text_nch, };
	#else /* LCDMODE_COLORED */
		const FLASHMEM char * const labels [2] = { text_nul, text_nch, };
	#endif /* LCDMODE_COLORED */
	display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}

// ����������� ������ �������� ����� � USB
static void display_datamode4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHTX
	#if WITHIF4DSP && WITHUSBUAC && WITHDATAMODE
		const uint_fast8_t state = hamradio_get_datamode();
		static const FLASHMEM char text_data [] = "DATA";
		static const FLASHMEM char text_null [] = "    ";
		#if LCDMODE_COLORED
			const FLASHMEM char * const labels [2] = { text_data, text_data, };
		#else /* LCDMODE_COLORED */
			const FLASHMEM char * const labels [2] = { text_null, text_data, };
		#endif /* LCDMODE_COLORED */
		display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);
	#endif /* WITHIF4DSP && WITHUSBUAC && WITHDATAMODE */
#endif /* WITHTX */
}

// ����������� ������ �������� ����� � USB
static void display_datamode3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHTX
	#if WITHIF4DSP && WITHUSBUAC && WITHDATAMODE
		const uint_fast8_t state = hamradio_get_datamode();
		static const FLASHMEM char text_data [] = "DAT";
		static const FLASHMEM char text_null [] = "   ";
		#if LCDMODE_COLORED
			const FLASHMEM char * const labels [2] = { text_data, text_data, };
		#else /* LCDMODE_COLORED */
			const FLASHMEM char * const labels [2] = { text_null, text_data, };
		#endif /* LCDMODE_COLORED */
		display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);
	#endif /* WITHIF4DSP && WITHUSBUAC && WITHDATAMODE */
#endif /* WITHTX */
}

// ����������� ������ �������������
static void display_atu3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHTX
	#if WITHAUTOTUNER
		const uint_fast8_t state = hamradio_get_atuvalue();
		static const FLASHMEM char text_atu [] = "ATU";
		static const FLASHMEM char text_nul [] = "   ";
		#if LCDMODE_COLORED
			const FLASHMEM char * const labels [2] = { text_atu, text_atu, };
		#else /* LCDMODE_COLORED */
			const FLASHMEM char * const labels [2] = { text_nul, text_atu, };
		#endif /* LCDMODE_COLORED */
		display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);
	#endif /* WITHAUTOTUNER */
#endif /* WITHTX */
}


// ����������� ������ �������������
static void display_genham1(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHBCBANDS

	const uint_fast8_t state = hamradio_get_genham_value();

	static const FLASHMEM char text_ham [] = "H";
	static const FLASHMEM char text_gen [] = "G";
	const FLASHMEM char * const labels [2] = { text_ham, text_gen };
	display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);

#endif /* WITHBCBANDS */
}

// ����������� ������ ������ ������
static void display_byp3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHTX
	#if WITHAUTOTUNER
		const uint_fast8_t state = hamradio_get_bypvalue();
		static const FLASHMEM char text_byp [] = "BYP";
		static const FLASHMEM char text_nul [] = "   ";
		#if LCDMODE_COLORED
			const FLASHMEM char * const labels [2] = { text_byp, text_byp, };
		#else /* LCDMODE_COLORED */
			const FLASHMEM char * const labels [2] = { text_nul, text_byp, };
		#endif /* LCDMODE_COLORED */
		display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);
	#endif /* WITHAUTOTUNER */
#endif /* WITHTX */
}

// ����������� ������ VOX
static void display_vox3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHTX
	#if WITHVOX
		const uint_fast8_t state = hamradio_get_voxvalue();
		static const FLASHMEM char text_vox [] = "VOX";
		static const FLASHMEM char text_nul [] = "   ";
		#if LCDMODE_COLORED
			const FLASHMEM char * const labels [2] = { text_vox, text_vox, };
		#else /* LCDMODE_COLORED */
			const FLASHMEM char * const labels [2] = { text_nul, text_vox, };
		#endif /* LCDMODE_COLORED */
		display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);
	#endif /* WITHVOX */
#endif /* WITHTX */
}

// ����������� ������� VOX � TUNE
// ���� VOX �� ������������, ������ TUNE
static void display_voxtune3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHTX

	static const FLASHMEM char text_vox [] = "VOX";
	static const FLASHMEM char text_tun [] = "TUN";
	static const FLASHMEM char text_nul [] = "   ";

#if WITHVOX

	const uint_fast8_t tunev = hamradio_get_tunemodevalue();
	const uint_fast8_t voxv = hamradio_get_voxvalue();

	#if LCDMODE_COLORED
		const FLASHMEM char * const labels [4] = { text_vox, text_vox, text_tun, text_tun, };
	#else /* LCDMODE_COLORED */
		const FLASHMEM char * const labels [4] = { text_nul, text_vox, text_tun, text_tun, };
	#endif /* LCDMODE_COLORED */

	display2_text_P(x, y, labels, colorsfg_4state, colorsbg_4state, tunev * 2 + voxv);

#else /* WITHVOX */

	const uint_fast8_t state = hamradio_get_tunemodevalue();

	#if LCDMODE_COLORED
		const FLASHMEM char * const labels [2] = { text_tun, text_tun, };
	#else /* LCDMODE_COLORED */
		const FLASHMEM char * const labels [2] = { text_nul, text_tun, };
	#endif /* LCDMODE_COLORED */

	display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);

#endif /* WITHVOX */
#endif /* WITHTX */
}

// ����������� ������� VOX � TUNE
// ������� �����
// ���� VOX �� ������������, ������ TUNE
static void display_voxtune4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHTX
#if WITHVOX

	const uint_fast8_t tunev = hamradio_get_tunemodevalue();
	const uint_fast8_t voxv = hamradio_get_voxvalue();
	static const FLASHMEM char text0 [] = "VOX ";
	static const FLASHMEM char text1 [] = "TUNE";
	const FLASHMEM char * const labels [4] = { text0, text0, text1, text1, };
	display2_text_P(x, y, labels, colorsfg_4state, colorsbg_4state, tunev * 2 + voxv);

#else /* WITHVOX */

	const uint_fast8_t state = hamradio_get_tunemodevalue();
	static const FLASHMEM char text [] = "TUNE";
	const FLASHMEM char * const labels [2] = { text, text, };
	display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);

#endif /* WITHVOX */
#endif /* WITHTX */
}

// ����������� ������� VOX � TUNE
// ������������� �����������
// ���� VOX �� ������������, ������ TUNE
static void display_voxtune1(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHTX
#if WITHVOX

	const uint_fast8_t tunev = hamradio_get_tunemodevalue();
	const uint_fast8_t voxv = hamradio_get_voxvalue();
	static const FLASHMEM char textx [] = " ";
	static const FLASHMEM char text0 [] = "V";
	static const FLASHMEM char text1 [] = "U";
	const FLASHMEM char * const labels [4] = { textx, text0, text1, text1, };
	display2_text_P(x, y, labels, colorsfg_4state, colorsbg_4state, tunev * 2 + voxv);

#else /* WITHVOX */

	const uint_fast8_t state = hamradio_get_tunemodevalue();
	static const FLASHMEM char textx [] = " ";
	static const FLASHMEM char text1 [] = "U";
	const FLASHMEM char * const labels [2] = { textx, text1, };
	display2_text_P(x, y, labels, colorsfg_2state, colorsbg_2state, state);

#endif /* WITHVOX */
#endif /* WITHTX */
}


static void display_lockstate4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const uint_fast8_t lockv = hamradio_get_lockvalue();
	const uint_fast8_t fastv = hamradio_get_usefastvalue();

	static const FLASHMEM char text0 [] = "    ";
	static const FLASHMEM char text1 [] = "LOCK";
	static const FLASHMEM char text2 [] = "FAST";
#if LCDMODE_COLORED
	const FLASHMEM char * const labels [4] = { text1, text2, text1, text1, };
#else /* LCDMODE_COLORED */
	const FLASHMEM char * const labels [4] = { text0, text2, text1, text1, };
#endif
	display2_text_P(x, y, labels, colorsfg_4state, colorsbg_4state, lockv * 2 + fastv);
}


static void display_lockstate1(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	display_setcolors(LOCKCOLOR, BGCOLOR);
	display_at_P(x, y, hamradio_get_lockvalue() ? PSTR("*") : PSTR(" "));
}

// ����������� PBT
static void display_pbt(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHPBT
	const int_fast32_t pbt = hamradio_get_pbtvalue();

	//display_setcolors(LOCKCOLOR, BGCOLOR);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_gotoxy(x + 0, y + lowhalf);		
		display_string_P(PSTR("PBT "), lowhalf);
		display_gotoxy(x + 4, y + lowhalf);		
		display_menu_value(pbt, 4 | WSIGNFLAG, 2, 1, lowhalf);
	} while (lowhalf --);
#endif /* WITHPBT */
}

// RX path bandwidth
static void display_rxbw3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_rxbw_value_P(), };
	display2_text_P(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
}


// ������� ��������� DUAL WATCH
static void display_mainsub3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHUSEDUALWATCH
	const char FLASHMEM * const labels [1] = { hamradio_get_mainsubrxmode3_value_P(), };
	display2_text_P(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
#endif /* WITHUSEDUALWATCH */
}


// RX preamplifier
static void display_pre3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_pre_value_P(), };
	display2_text_P(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
}

// ������������ ��� (���� ���������� ��� REDRM_BARS - � ��������� �����������)
static void display_ovf3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHDSPEXTDDC
	//const char FLASHMEM * const labels [1] = { hamradio_get_pre_value_P(), };
	//display2_text_P(x, y, labels, colorsfg_1state, colorsbg_1state, 0);

	if (boad_fpga_adcoverflow() != 0)
	{
		display_setcolors(BGCOLOR, OVFCOLOR);
		display_at_P(x, y, PSTR("OVF"));
	}
	else if (boad_mike_adcoverflow() != 0)
	{
		display_setcolors(BGCOLOR, OVFCOLOR);
		display_at_P(x, y, PSTR("MIK"));
	}
	else
	{
		display_setcolors(BGCOLOR, BGCOLOR);
		display_at_P(x, y, PSTR("   "));
	}
#endif /* WITHDSPEXTDDC */
}

// RX preamplifier ��� ������������ ��� (���� ���������� ��� REDRM_BARS - � ��������� �����������)
static void display_preovf3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	if (boad_fpga_adcoverflow() != 0)
	{
		display_setcolors(OVFCOLOR, BGCOLOR);
		display_at_P(x, y, PSTR("OVF"));
	}
	else if (boad_mike_adcoverflow() != 0)
	{
		display_setcolors(BGCOLOR, OVFCOLOR);
		display_at_P(x, y, PSTR("MIK"));
	}
	else
	{
		display_setcolors(MODECOLOR, BGCOLOR);
		display_at_P(x, y, hamradio_get_pre_value_P());
	}
}

// display antenna
static void display_ant5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHANTSELECT
	const char FLASHMEM * const labels [1] = { hamradio_get_ant5_value_P(), };
	display2_text_P(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
#endif /* WITHANTSELECT */
}

// RX att (or att/pre)
static void display_att4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_att_value_P(), };
	display2_text_P(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
}

// HP/LP
static void display_hplp2(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHPOWERLPHP
	const char FLASHMEM * const labels [1] = { hamradio_get_hplp_value_P(), };
	display2_text_P(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
#endif /* WITHPOWERLPHP */
}

// RX att, ��� �������� ���������� TX
static void display_att_tx3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const uint_fast8_t tx = hamradio_get_tx();
	const FLASHMEM char * text = tx ? PSTR("TX  ") : hamradio_get_att_value_P();

	display_setcolors(MODECOLOR, BGCOLOR);
	display_at_P(x, y, text);
}

// RX agc
static void display_agc3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_agc3_value_P(), };
	display2_text_P(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
}

// RX agc
static void display_agc4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const FLASHMEM char * text = hamradio_get_agc4_value_P();

	display_setcolors(MODECOLOR, BGCOLOR);
	display_at_P(x, y, text);
}

// VFO mode
static void display_vfomode3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const char * const labels [1] = { hamradio_get_vfomode3_value(), };
	display2_text(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
}

// VFO mode
static void display_vfomode5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const char * const labels [1] = { hamradio_get_vfomode5_value(), };
	display2_text(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
}

// VFO mode - ����� �������� (������ �� ����� SPLIT ��� ��������)
static void display_vfomode1(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const char * text = hamradio_get_vfomode3_value();

	display_setcolors(MODECOLOR, BGCOLOR);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_gotoxy(x, y + lowhalf);
		display_wrdata_begin();
		display_put_char_small(text [0], lowhalf);
		display_wrdata_end();
	} while (lowhalf --);
}

// SSB/CW/AM/FM/...
static void display_mode3_a(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_mode_a_value_P(), };
	display2_text_P(x, y, labels, colorsfg_1gold, colorsbg_1gold, 0);
}


// SSB/CW/AM/FM/...
static void display_mode3_b(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_mode_b_value_P(), };
	display2_text_P(x, y, labels, colorsfg_1gold, colorsbg_1gold, 0);
}

// dd.dV - 5 places
static void display_voltlevelV5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHVOLTLEVEL && WITHCPUADCHW
	uint_fast8_t volt = hamradio_get_volt_value();	// ���������� � ������ ���������� �.�. 151 = 15.1 ������

	display_setcolors(colorsfg_1state [0], colorsbg_1state [0]);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_gotoxy(x + CHARS2GRID(0), y + lowhalf);	
		display_value_small(volt, 3, 1, 255, 0, lowhalf);
		display_gotoxy(x + CHARS2GRID(4), y + lowhalf);	
		display_string_P(PSTR("V"), lowhalf);
	} while (lowhalf --);
#endif /* WITHVOLTLEVEL && WITHCPUADCHW */
}

// dd.d - 4 places
static void display_voltlevel4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHVOLTLEVEL && WITHCPUADCHW
	const uint_fast8_t volt = hamradio_get_volt_value();	// ���������� � ������ ���������� �.�. 151 = 15.1 ������

	display_setcolors(colorsfg_1state [0], colorsbg_1state [0]);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_gotoxy(x, y + lowhalf);	
		display_value_small(volt, 3, 1, 255, 0, lowhalf);
	} while (lowhalf --);
#endif /* WITHVOLTLEVEL && WITHCPUADCHW */
}

// +d.ddA - 6 places (with "A")
// amphermeter with "A"
static void display_currlevelA6(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHCURRLEVEL && WITHCPUADCHW
	int_fast16_t drain = hamradio_get_pacurrent_value();	// ��� � �������� ��������� (�� 2.55 ������), ����� ���� �������������

	display_setcolors(colorsfg_1state [0], colorsbg_1state [0]);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_gotoxy(x + CHARS2GRID(0), y + lowhalf);	
		display_value_small(drain, 3 | WMINUSFLAG, 2, 255, 0, lowhalf);
		display_gotoxy(x + CHARS2GRID(5), y + lowhalf);	
		display_string_P(PSTR("A"), lowhalf);
	} while (lowhalf --);
#endif /* WITHCURRLEVEL && WITHCPUADCHW */
}

// d.dd - 5 places (without "A")
static void display_currlevel5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHCURRLEVEL && WITHCPUADCHW
	int_fast16_t drain = hamradio_get_pacurrent_value();	// ��� � �������� ��������� (�� 2.55 ������), ����� ���� �������������

	display_setcolors(colorsfg_1state [0], colorsbg_1state [0]);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_gotoxy(x + CHARS2GRID(0), y + lowhalf);	
		display_value_small(drain, 3 | WMINUSFLAG, 2, 255, 0, lowhalf);
		//display_gotoxy(x + CHARS2GRID(4), y + lowhalf);	
		//display_string_P(PSTR("A"), lowhalf);
	} while (lowhalf --);
#endif /* WITHCURRLEVEL && WITHCPUADCHW */
}
// dd.d - 5 places (without "A")
static void display_currlevel5alt(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHCURRLEVEL && WITHCPUADCHW
	int_fast16_t drain = hamradio_get_pacurrent_value();	// ��� � �������� ��������� (�� 2.55 ������), ����� ���� �������������

	display_setcolors(colorsfg_1state [0], colorsbg_1state [0]);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_gotoxy(x + CHARS2GRID(0), y + lowhalf);	
		display_value_small(drain, 3 | WMINUSFLAG, 1, 255, 1, lowhalf);
		//display_gotoxy(x + CHARS2GRID(4), y + lowhalf);	
		//display_string_P(PSTR("A"), lowhalf);
	} while (lowhalf --);
#endif /* WITHCURRLEVEL && WITHCPUADCHW */
}

// ����������� ������ ������� � dBm
static void display_siglevel7(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHIF4DSP
	uint_fast8_t tracemax;
	uint_fast8_t v = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);

	char buff [32];
	local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("%-+4d" "dBm"), tracemax - UINT8_MAX);
	(void) v;
	const char * const labels [1] = { buff, };
	display2_text(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
#endif /* WITHIF4DSP */
}

// ����������� ������ ������� � ������ ����� S
// S9+60
static void display_siglevel5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHIF4DSP
	uint_fast8_t tracemax;
	uint_fast8_t v = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);

	char buff [32];
	const int s9level = - 73;
	const int s9step = 6;
	const int alevel = tracemax - UINT8_MAX;

	(void) v;
	if (alevel < (s9level - s9step * 9))
	{
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("S0   "));
	}
	else if (alevel < (s9level - s9step * 7))
	{
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("S1   "));
	}
	else if (alevel < (s9level - s9step * 6))
	{
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("S2   "));
	}
	else if (alevel < (s9level - s9step * 5))
	{
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("S3   "));
	}
	else if (alevel < (s9level - s9step * 4))
	{
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("S4   "));
	}
	else if (alevel < (s9level - s9step * 3))
	{
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("S5   "));
	}
	else if (alevel < (s9level - s9step * 2))
	{
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("S6   "));
	}
	else if (alevel < (s9level - s9step * 1))
	{
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("S7   "));
	}
	else if (alevel < (s9level - s9step * 0))
	{
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("S8   "));
	}
	else
	{
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("S9+%02d"), alevel - s9level);
	}
	const char * const labels [1] = { buff, };
	display2_text(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
#endif /* WITHIF4DSP */
}

static void display_freqdelta8(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHINTEGRATEDDSP
	int_fast32_t deltaf;
	const uint_fast8_t f = dsp_getfreqdelta10(& deltaf, 0);		/* �������� �������� ���������� ������� � ��������� 0.1 ����� ��� ��������� A */
	deltaf = - deltaf;	// ������ �� ������� ������������� � ����������
	display_setcolors(colorsfg_1state [0], colorsbg_1state [0]);
	if (f != 0)
	{
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_gotoxy(x, y + lowhalf);	
			display_value_small(deltaf, 6 | WSIGNFLAG, 1, 255, 0, lowhalf);
		} while (lowhalf --);
	}
	else
	{
		display_at_P(x, y, PSTR("        "));
	}
#endif /* WITHINTEGRATEDDSP */
}

/* �������� ���������� �� ������ ��������� � ������ SAM */
static void display_samfreqdelta8(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHINTEGRATEDDSP
	int_fast32_t deltaf;
	const uint_fast8_t f = hamradio_get_samdelta10(& deltaf, 0);		/* �������� �������� ���������� ������� � ��������� 0.1 ����� ��� ��������� A */
	deltaf = - deltaf;	// ������ �� ������� ������������� � ����������
	display_setcolors(colorsfg_1state [0], colorsbg_1state [0]);
	if (f != 0)
	{
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_gotoxy(x, y + lowhalf);	
			display_value_small(deltaf, 6 | WSIGNFLAG, 1, 255, 0, lowhalf);
		} while (lowhalf --);
	}
	else
	{
		display_at_P(x, y, PSTR("        "));
	}
#endif /* WITHINTEGRATEDDSP */
}

// d.d - 3 places
// ������� �������� ������� ������� ����� �� ������� ��/��
static void display_amfmhighcut3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHAMHIGHKBDADJ
	const uint_fast8_t v = hamradio_get_amfm_highcut100_value();	// ������� �������� ������� ������� ����� �� ������� ��/�� (� ������ ����)

	display_setcolors(colorsfg_1state [0], colorsbg_1state [0]);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_gotoxy(x, y + lowhalf);	
		display_value_small(v, 2, 1, 255, 0, lowhalf);
	} while (lowhalf --);
#endif /* WITHAMHIGHKBDADJ */
}

// ������ ������� - ����, ������ � �������
static void display_time8(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if defined (RTC1_TYPE)
	uint_fast8_t hour, minute, secounds;
	char buff [9];

	board_rtc_gettime(& hour, & minute, & secounds);
	local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("%02d:%02d:%02d"), 
		hour, minute, secounds
		);
	
	const char * const labels [1] = { buff, };
	display2_text(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
#endif /* WITHNMEA */
}

// ������ ������� - ������ ���� � ������, ��� ������
static void display_time5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if defined (RTC1_TYPE)
	uint_fast8_t hour, minute, secounds;
	char buff [6];

	board_rtc_gettime(& hour, & minute, & secounds);
	local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("%02d:%02d"), 
		hour, minute
		);

	const char * const labels [1] = { buff, };
	display2_text(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
#endif /* WITHNMEA */
}

// ������ ������� - ������ ���� � ������, ��� ������
static void display_datetime12(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if defined (RTC1_TYPE)
	char buff [13];

	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, secounds;
	static const char months [13] [4] = 
	{
		"JAN",
		"FEB",
		"MAR",
		"APR",
		"MAY",
		"JUN",
		"JUL",
		"AUG",
		"SEP",
		"OCT",
		"NOV",
		"DEC",
	};

	board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);

	local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("%s/%2d %02d:%02d"), 
		months [month - 1],
		day,
		hour, minute
		);

	const char * const labels [1] = { buff, };
	display2_text(x, y, labels, colorsfg_1state, colorsbg_1state, 0);
#endif /* WITHNMEA */
}

struct dzone
{
	uint_fast8_t x; // ����� ������� ����
	uint_fast8_t y;
	void (* redraw)(uint_fast8_t x, uint_fast8_t y, void * pv);	// ������� ����������� ��������
	uint_fast8_t key;		// ��� ����� ����������� ���������������� ���� �������
	uint_fast8_t subset;
};

/* struct dzone subset field values */

#define REDRSUBSET(page)		(1U << (page))	// ������ ������������� ������ ������������� ������ ���������
#define REDRSUBSET_ALL ( \
		REDRSUBSET(0) | \
		REDRSUBSET(1) | \
		REDRSUBSET(2) | \
		REDRSUBSET(3) | \
		REDRSUBSET(4) | \
		REDRSUBSET(5) | \
		0)
#define REDRSUBSET_MENU		REDRSUBSET(7)
#define REDRSUBSET_SLEEP	REDRSUBSET(6)

enum
{
	REDRM_MODE,		// ���� ���������� ��� ��������� ������� ������, LOCK state
	REDRM_FREQ,		// ���������� �������
	REDRM_FREQB,	// ���������� �������
	REDRM_BARS,		// S-meter, SWR-meter, voltmeter
	REDRM_VOLT,		// ��������� (����� ���������� ���������)
	REDRM_MVAL,		// �������� ����
	REDRM_MLBL,		// �������� �������������� ���������
	//
	REDRM_count
};

/* �������� ������������ ��������� �� �������� */

#if DSTYLE_T_X20_Y4

	enum
	{
		BDTH_ALLRX = 14,	// ������ ���� ��� ����������� ������ �� ����������
		BDTH_RIGHTRX = 5,	// ������ ���������� ������
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
		BDTH_ALLSWR = 9,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 4,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

	enum {
		DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		DPAGE1,					// ��������, � ������� ������������ �������� (��� ���) 
		DISPLC_MODCOUNT,
	};
	enum {
		DPAGEEXT = REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1)
	};
	#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
	#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, DPAGEEXT, },	// ������� ��� ���������� ��������
		//{	0, 0,	display_txrxstate2,	REDRM_MODE, DPAGEEXT, },
		{	9, 0,	display_mode3_a,	REDRM_MODE,	DPAGEEXT, },	// SSB/CW/AM/FM/...
		{	12, 0,	display_lockstate1,	REDRM_MODE, DPAGEEXT, },
		{	13, 0,	display_rxbw3,		REDRM_MODE, DPAGEEXT, },
		{	17, 0,	display_vfomode3,	REDRM_MODE, DPAGEEXT, },	// SPLIT

		{	0, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	4, 1,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if defined (RTC1_TYPE)
		{	8, 1,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
	#endif /* defined (RTC1_TYPE) */
	#if WITHUSEDUALWATCH
		{	0, 1,	display_freqchr_b,	REDRM_FREQ, REDRSUBSET(DPAGE1), },	// ������� ��� ���������� ��������
		{	17, 1,	display_mainsub3, REDRM_MODE, DPAGEEXT, },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */

		{	0, 2,	display2_bars,		REDRM_BARS, DPAGEEXT, },	// S-METER, SWR-METER, POWER-METER
		{	17, 2,	display_voxtune3,	REDRM_MODE, DPAGEEXT, },

	#if WITHVOLTLEVEL && WITHCURRLEVEL
		{	0, 3,	display_voltlevel4, REDRM_VOLT, DPAGEEXT, },	// voltmeter with "V"
		{	5, 3,	display_currlevel5, REDRM_VOLT, DPAGEEXT, },	// without "A"
	#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
#if WITHIF4DSP
	#if WITHUSEAUDIOREC
		{	13, 3,	display_rec3,		REDRM_BARS, DPAGEEXT, },	// ����������� ������ ������ ����� ���������
	#endif /* WITHUSEAUDIOREC */
#endif /* WITHIF4DSP */
		{	17, 3,	display_agc3,		REDRM_MODE, DPAGEEXT, },

	#if WITHMENU 
		{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
		{	0, 1,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
		{	4, 1,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
		{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
	#endif /* WITHMENU */
	};

#elif DSTYLE_T_X20_Y2
	/*
		��� ��, ��� ������ ������ ������� 2*20, ��������� ������ � ��� 
		��������� (������ ��������� ��������) - ��� ����� ���������, 
		���� ������������ ������.
		����� ���������, ����� ������� � ����� �� ������� ������������ �������� ���������. 
		���� ���� ������� ��� ����� � �������� ������� ������ �������� - ����� ��� � �����������, 
		� �������. ������ ��� ���������� ���� ����� �������� ������ �������� ���������� 
		����� �� ������, ������� ������ ����������� ��� ������������ ������ �����������. 
		��, ��� ������������� �� MENU (REDRSUBSET(0)/REDRSUBSET(1)) - ���������������� �� ��������� ������.

	*/
	enum
	{
		BDTH_ALLRX = 12,	// ������ ���� ��� ����������� ������ �� ����������
		BDTH_RIGHTRX = 4,	// ������ ���������� ������
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
		BDTH_SPACERX = 1,		/* ���������� �������, ���������� ������ �� ������ S-����� */
	#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
		BDTH_ALLSWR = 6,
		BDTH_SPACESWR = 1,	/* ���������� �������, ���������� ������ �� ������ SWR-����� */
		BDTH_ALLPWR = 5,
		BDTH_SPACEPWR = 0	/* ���������� �������, ���������� ������ �� ������ PWR-����� */
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

	enum {
		DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		DPAGE_SMETER,
		DPAGE_TIME,
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
	#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
	static const FLASHMEM struct dzone dzones [] =
	{
		// ������ 0
		{	0, 0,	display_vfomode3,	REDRM_MODE, REDRSUBSET_ALL, },	// SPLIT
		{	4, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// ������� ��� ���������� ��������
		{	12, 0,	display_lockstate1,	REDRM_MODE, REDRSUBSET_ALL, },
		{	13, 0,	display_mode3_a,	REDRM_MODE,	REDRSUBSET_ALL, },	// SSB/CW/AM/FM/...
		{	17, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET_ALL, },
		// ������ 1 - ���������� ��������
		{	0, 1,	display_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },
		// ������ 1
		{	4, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if WITHDSPEXTDDC
		{	9, 1,	display_preovf3,	REDRM_BARS, REDRSUBSET(DPAGE0), },	// ovf/pre
	#else /* WITHDSPEXTDDC */
		{	9, 1,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// pre
	#endif /* WITHDSPEXTDDC */
		// ������ 1
		{	4, 1,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE_SMETER), },	// S-METER, SWR-METER, POWER-METER
		{	4, 1,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// TIME
		// ������ 1 - ���������� ��������
		{	17, 1,	display_agc3,		REDRM_MODE, REDRSUBSET_ALL, },

		//{	0, 0,	display_txrxstate2,	REDRM_MODE, REDRSUBSET_ALL, },
	#if WITHMENU
		{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
		{	0, 1,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
		{	4, 1,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
		{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
	#endif /* WITHMENU */
	};

#elif DSTYLE_T_X20_Y2_IGOR

	enum
	{
		BDTH_ALLRX = 12,	// ������ ���� ��� ����������� ������ �� ����������
		BDTH_RIGHTRX = 4,	// ������ ���������� ������
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
		BDTH_SPACERX = 1,		/* ���������� �������, ���������� ������ �� ������ S-����� */
	#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
		BDTH_ALLSWR = 6,
		BDTH_SPACESWR = 1,	/* ���������� �������, ���������� ������ �� ������ SWR-����� */
		BDTH_ALLPWR = 5,
		BDTH_SPACEPWR = 0	/* ���������� �������, ���������� ������ �� ������ PWR-����� */
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

	enum {
		DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		DPAGE1 = 0,					// ��������, � ������� ������������ �������� (��� ���) 
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
	#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1), },	// SPLIT
		{	4, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1), },	// ������� ��� ���������� ��������
		{	12, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1), },
		{	4, 1,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		//{	0, 1, display_pbt,		REDRM_BARS, REDRSUBSET(DPAGE1), },	// PBT +00.00
	#if WITHMENU
		{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
		{	0, 1,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
		{	4, 1,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
		{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
	#endif /* WITHMENU */
	};

#elif DSTYLE_T_X16_Y2 && DSTYLE_SIMPLEFREQ

	enum
	{
		BDTH_ALLRX = 12,	// ������ ���� ��� ����������� ������ �� ����������
		BDTH_RIGHTRX = 4,	// ������ ���������� ������
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
		BDTH_ALLSWR = 7,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 4,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	40	// 4.0 - �������� �� ������ �����
	enum {
		DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		//
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
	#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// ������� ��� ���������� ��������
	};

#elif DSTYLE_T_X16_Y2 && CTLSTYLE_RA4YBO_AM0

	enum
	{
		BDTH_ALLRX = 15,	// ������ ���� ��� ����������� ������ �� ����������
		BDTH_RIGHTRX = 5,	// ������ ���������� ������
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
		BDTH_ALLSWR = 7,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 7,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	40	// 4.0 - �������� �� ������ �����
	enum {
		DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		//
		DISPLC_MODCOUNT
	};

	#define DISPLC_WIDTH	6	// ���������� ���� � ����������� �������
	#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },	// ������� ��� ���������� ��������
		{	8, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	13, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	1, 1,	display2_bars_amv0,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
	#if WITHMENU 
		{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
		{	0, 1,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
		{	4, 1,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
	#endif /* WITHMENU */
	};

#elif DSTYLE_T_X16_Y2

	enum
	{
		BDTH_ALLRX = 16,	// ������ ���� ��� ����������� ������ �� ����������
		BDTH_RIGHTRX = 5,	// ������ ���������� ������
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
		BDTH_ALLSWR = 7,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 8,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	40	// 4.0 - �������� �� ������ �����
	enum {
		DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		DPAGE_SMETER,			// �������� � ������������ S-�����, SWR-�����
	#if WITHUSEAUDIOREC
		//DPAGE_SDCARD,
	#endif /* WITHUSEAUDIOREC */
	#if WITHUSEDUALWATCH
		DPAGE_SUBRX,
	#endif /* WITHUSEDUALWATCH */
	#if defined (RTC1_TYPE)
		DPAGE_TIME,
	#endif /* defined (RTC1_TYPE) */
	#if WITHMODEM
		DPAGE_BPSK,
	#endif /* WITHMODEM */
	#if WITHVOLTLEVEL && WITHCURRLEVEL 
		DPAGE_VOLTS,
	#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
		//
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
	#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// ������� ��� ���������� ��������
		{	8, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET_ALL, },
		{	9, 0,	display_mode3_a,	REDRM_MODE,	REDRSUBSET_ALL, },	// SSB/CW/AM/FM/...
		{	13, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET_ALL, },

		{	0, 1,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
		{	4, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// ��� ������� s-metre, ��� �������� ���������� TX
	#if WITHDSPEXTDDC
		{	9, 1,	display_preovf3,	REDRM_BARS, REDRSUBSET(DPAGE0), },	// ovf/pre
	#else /* WITHDSPEXTDDC */
		{	9, 1,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// pre
	#endif /* WITHDSPEXTDDC */
#if WITHIF4DSP
	#if WITHUSEAUDIOREC
		{	13, 1,	display_rec3,		REDRM_BARS, REDRSUBSET(DPAGE0) /*| REDRSUBSET(DPAGE_SMETER)*/, },	// ����������� ������ ������ ����� ���������
	#endif /* WITHUSEAUDIOREC */
#else /* WITHIF4DSP */
		{	13, 1,	display_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0) /*| REDRSUBSET(DPAGE_SMETER)*/, },
#endif /* WITHIF4DSP */

		{	0, 1,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE_SMETER), },	// S-METER, SWR-METER, POWER-METER

	#if WITHVOLTLEVEL && WITHCURRLEVEL
		{	0, 1,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE_VOLTS), },	// voltmeter with "V"
		{	6, 1,	display_currlevelA6, REDRM_VOLT, REDRSUBSET(DPAGE_VOLTS), },	// amphermeter with "A"
	#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#if WITHMODEM
		{	0, 1,	display_freqdelta8, REDRM_BARS, REDRSUBSET(DPAGE_BPSK), },	// ����� �� ������������
	#endif /* WITHMODEM */
	#if WITHUSEDUALWATCH
		{	0, 1,	display_freqchr_b,	REDRM_FREQ, REDRSUBSET(DPAGE_SUBRX), },	// FREQ B
		{	9, 1,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE_SUBRX), },	// SPLIT
		{	13, 1,	display_mainsub3, REDRM_MODE, REDRSUBSET(DPAGE_SUBRX), },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */

	//#if WITHUSEAUDIOREC
	//	{	0, 1,	display_recstatus,	REDRM_BARS, REDRSUBSET(DPAGE_SDCARD), },	// recording debug information
	//	{	13, 1,	display_rec3,		REDRM_BARS, REDRSUBSET(DPAGE_SDCARD), },	// ����������� ������ ������ ����� ���������
	//#endif /* WITHUSEAUDIOREC */

	#if defined (RTC1_TYPE)
		{	0, 1,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// TIME
	#if WITHUSEDUALWATCH
		{	9, 1,	display_mainsub3,	REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */
	#if WITHUSEAUDIOREC
		{	13, 1,	display_rec3,		REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// ����������� ������ ������ ����� ���������
	#endif /* WITHUSEAUDIOREC */
	#endif /* defined (RTC1_TYPE) */


		//{	0, 0,	display_txrxstate2,	REDRM_MODE, REDRSUBSET_ALL, },
		//{	0, 0,	display_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },
	#if WITHMENU
		{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
		{	0, 1,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
		{	4, 1,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
		{	15, 0,	display_lockstate1,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
	#endif /* WITHMENU */
	};

#elif DSTYLE_T_X16_Y4

	enum
	{
		BDTH_ALLRX = 14,	// ������ ���� ��� ����������� ������ �� ����������
		BDTH_RIGHTRX = 5,	// ������ ���������� ������
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
		BDTH_ALLSWR = 9,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 4,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	40	// 4.0 - �������� �� ������ �����

	enum {
		DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
	#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// ������� ��� ���������� ��������
		{	8, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET_ALL, },
		{	9, 0,	display_mode3_a,	REDRM_MODE,	REDRSUBSET_ALL, },	// SSB/CW/AM/FM/...
		{	13, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET_ALL, },

		{	0, 1,	display_vfomode3,	REDRM_MODE, REDRSUBSET_ALL, },	// SPLIT
		{	4, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET_ALL, },		// ��� �������� ���������� TX
	#if WITHDSPEXTDDC
		{	9, 1,	display_preovf3,	REDRM_BARS, REDRSUBSET_ALL, },	// ovf/pre
	#else /* WITHDSPEXTDDC */
		{	9, 1,	display_pre3,		REDRM_MODE, REDRSUBSET_ALL, },	// pre
	#endif /* WITHDSPEXTDDC */

		{	0, 2,	display2_bars,		REDRM_BARS, REDRSUBSET_ALL, },	// S-METER, SWR-METER, POWER-METER

		{	0, 3,	display_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },
	#if defined (RTC1_TYPE)
		{	4, 3,	display_time8,		REDRM_BARS, REDRSUBSET_ALL, },	// TIME
	#endif /* defined (RTC1_TYPE) */
		{	13, 3,	display_agc3,		REDRM_MODE, REDRSUBSET_ALL, },

		//{	0, 0,	display_txrxstate2,	REDRM_MODE, REDRSUBSET_ALL, },
		//{	0, 0,	display_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },

	#if WITHMENU
		{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ��������� ���������
		{	0, 1,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
		{	4, 1,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
		{	15, 0,	display_lockstate1,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
	#endif /* WITHMENU */
	};

#elif DSTYLE_G_X64_Y32
	// RDX0120
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */

	enum
	{
		BDTH_ALLRX = 9,	// ������ ���� ��� ����������� ������ �� ����������
		BDTH_RIGHTRX = 3,	// ������ ���������� ������
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
		BDTH_SPACERX = 0,
		//
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	};
	enum
	{
		PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
		PATTERN_BAR_FULL = 0x7e,	//0x7C,	// ������ ��� - ������
		PATTERN_BAR_HALF = 0x1e,	//0x38,	// ������ ��� - ������
		PATTERN_BAR_EMPTYFULL = 0x42,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x12	//0x00
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

	enum {
		DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
	#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	3, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	7, 0,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

		{	0, 1,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },

		{	9, 2,	display_vfomode1,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT - ����� ��������
		{	0, 2,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER

		{	6, 3,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	9, 3,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	0, 3,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if WITHMENU 
		{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
		{	0, 3,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
		{	0, 2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
		{	9, 3,	display_lockstate1,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
	#endif /* WITHMENU */
	};

#elif DSTYLE_G_X128_Y64
	// RDX0077
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP

		enum
		{
			BDTH_ALLRX = 21,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 11,	// ������ ���������� ������ 
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 11,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890"
		#define SMETERMAP 	  " 1 3 5 7 9 + 20 40 60"	//
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%   | 100%" 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		#else
			#define POWERMAP  " 0 10 20 40 60 80 100"
			#define SWRMAP    "1    |    2    |   3 "	// 
			#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - �������� �� ������ �����
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
		#if 1
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x7e,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x42
		#else
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x24
		#endif
		};

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },	// TX/RX
			{	3, 0,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },		// VOX/TUN
			{	7, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	13, 0,	display_voltlevel4, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter without "V"
			{	18, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },


			{	0,	1,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	18, 2,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	2, 4,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	7, 4,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	18, 4,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...


			{	2, 5,	display_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 5,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	11, 5,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 6,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0, 7,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },// ����������� ��������� ����� S-�����
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 1,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 1,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

		enum
		{
			BDTH_ALLRX = 17,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 6,	// ������ ���������� ������ 
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 10,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 6,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//#define SMETERMAP "1 3 5 7 9 + 20 40 60"
		//#define POWERMAP  "0 10 20 40 60 80 100"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x24
		};

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3,	0,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7,	0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0,	2,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },		// col = 1 for !WIDEFREQ
			{	18, 3,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	18, 5,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0,	5,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	7,	5,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },		// x=8 then !WIDEFREQ
			{	18, 7,	display_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0,	7,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 1,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 1,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

#elif DSTYLE_G_X132_Y64
	// RDX0154
	// �� ����������� 22 ���������� (x=0..21)
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP

		enum
		{
			BDTH_ALLRX = 21,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 11,	// ������ ���������� ������ 
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 11,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "0123456789012345678901"
		#define SMETERMAP 	  " 1 3 5 7 9 + 20 40 60 "	//
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%   | 100% " 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		#else
			#define POWERMAP  " 0 10 20 40 60 80 100 "
			#define SWRMAP    "1    |    2    |   3  "	// 
			#define SWRMAX	(SWRMIN * 32 / 10)	// 3.2 - �������� �� ������ �����
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
		#if 1
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x7e,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x42
		#else
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x24
		#endif
		};

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		// �� ����������� 22 ���������� (x=0..21)
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },	// TX/RX
			{	3,	0,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// VOX/TUN
			{	7,	0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// PRE/ATT/___
			{	11, 0,	display_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },	// LOCK
			{	19, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// 3.1

			{	0,	1,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	19, 2,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/..
			//
			{	2,	4,	display_vfomode5,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	8,	4,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	19, 4,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0,	5,	display_ant5,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// ANTENNA
			{	6,	5,	display_voltlevelV5,REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
			{	12, 5,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// HP/LP
		#if defined (RTC1_TYPE)
			{	15, 5,	display_time5,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
		#endif /* defined (RTC1_TYPE) */
			{	15, 5,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// ATU
			{	19, 5,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// BYP

			{	0,	6,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0,	7,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// ����������� ��������� ����� S-�����
		#if WITHMENU
			{	0,	0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0,	1,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4,	1,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

		enum
		{
			BDTH_ALLRX = 17,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 6,	// ������ ���������� ������
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 8,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 8,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x00	//0x00
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 2,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },	// x=1 then !WIDEFREQ
			{	19, 3,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 5,	display_voltlevel4, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter without "V"
			{	0, 6,	display_currlevelA6, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// amphermeter with "A"
			{	13, 5,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	16, 5,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },	// x=9 then !WIDEFREQ
			{	19, 5,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 6,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	4, 6,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

	#if WITHIF4DSP
		#if WITHUSEAUDIOREC
			{	19, 7,	display_rec3,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// ����������� ������ ������ ����� ���������
		#endif /* WITHUSEAUDIOREC */
	#else /* WITHIF4DSP */
			{	19, 7,	display_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#endif /* WITHIF4DSP */
	#if WITHUSEDUALWATCH
			{	19, 6,	display_mainsub3, REDRM_BARS, REDRSUBSET(DPAGE0), },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */

			{	0, 7,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER

		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 1,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 1,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#endif /* DSTYLE_UR3LMZMOD */

#elif DSTYLE_G_X160_Y128
	// TFT-1.8, LCDMODE_ST7735 for example
	// CHAR_W=8, CHAR_H=8, SMALLCHARH=16
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// ������������ � �������� ������� sw2013 mini - CTLSTYLE_SW2012CN � CTLSTYLE_SW2012CN5

		enum
		{
			BDTH_ALLRX = 20,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 11,	// ������ ���������� ������ 
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 10,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		#define SMETERMAP "1 3 5 7 9 + 20 40 60"	//
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			//                "01234567890123456789"
			#define SWRPWRMAP "1 | 2 | 3 0%  | 100%"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		#else /* WITHSHOWSWRPWR */
			#define POWERMAP  "0 10 20 40 60 80 100"
			#define SWRMAP    "1    |    2    |   3"	// 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		#endif /* WITHSHOWSWRPWR */
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
		#if 1
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		#else
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x24
		#endif
		};

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	17, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 3,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	17, 5,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 8,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	7, 8,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	17, 8,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 11,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	4, 11,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	15, 11,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"

			{	0, 13,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0, 14,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// ����������� ��������� ����� S-�����
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 2,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

		enum
		{
			BDTH_ALLRX = 15,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 5,	// ������ ���������� ������
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 7,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 7,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x24
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3,	0,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7,	0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	17, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0,	4,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	17, 6,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0,	9,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	7,	9,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	17, 9,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0,	12,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	4,	12,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	8,	12,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"

			{	16, 13,	display_agc4,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 14,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 2,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

#elif DSTYLE_G_X176_Y132
	// Siemens S65 LS020, Siemens S65 LPH88
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// SW20XXX - ������ ���� ��������� ����������� � ��� - "�� �����"
		enum
		{
			BDTH_ALLRX = 20,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 11,	// ������ ���������� ������
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 10,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		#define SMETERMAP     "1 3 5 7 9 + 20 40 60"
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			//                "01234567890123456789"
			#define SWRPWRMAP "1 | 2 | 3 0%  | 100%"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		#else /* WITHSHOWSWRPWR */
			#define SWRMAP    "1    |    2    |   3"	// 
			#define POWERMAP  "0 10 20 40 60 80 100"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		#endif /* WITHSHOWSWRPWR */
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			//PATTERN_BAR_FULL = 0x7e,
			//PATTERN_BAR_HALF = 0x7e /* 0x3c */,
			PATTERN_BAR_EMPTYFULL = 0x00,
			PATTERN_BAR_EMPTYHALF = 0x00
		};

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	6, 0,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	14, 0,	display_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0, 2,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
			{	6, 2,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 2,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	17, 3,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0,	5,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	0, 10,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	17, 10,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	5, 10,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	0, 13,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },		// S-METER, SWR-METER, POWER-METER
			{	1, 14,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// ����������� ��������� ����� S-�����
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 2,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#elif DSTYLE_UR3LMZMOD
		// ���������� ��� � ������������� ������������
		// ����������� ���
		// ��� CTLSTYLE_RA4YBO_V1

		enum
		{
			BDTH_ALLRX = 20,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 11,	// ������ ���������� ������
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 10,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		#define SMETERMAP     "1 3 5 7 9 + 20 40 60"
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			//                "01234567890123456789"
			#define SWRPWRMAP "1 | 2 | 3 0%  | 100%"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		#else /* WITHSHOWSWRPWR */
			#define SWRMAP    "1    |    2    |   3"	// 
			#define POWERMAP  "0 10 20 40 60 80 100"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		#endif /* WITHSHOWSWRPWR */
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			//PATTERN_BAR_FULL = 0x7e,
			//PATTERN_BAR_HALF = 0x7e /* 0x3c */,
			PATTERN_BAR_EMPTYFULL = 0x00,
			PATTERN_BAR_EMPTYHALF = 0x00
		};

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },		// 4 chars - 12dB, 18dB
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 2,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
			{	6, 2,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 2,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 2,	display_agc4,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0,	6,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	18, 4,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 10,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	4, 10,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	16, 10,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 13,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },		// S-METER, SWR-METER, POWER-METER
			{	1, 14,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// ����������� ��������� ����� S-�����
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 2,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};


	#else /* DSTYLE_UR3LMZMOD */

		enum
		{
			BDTH_ALLRX = 17,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 6,	// ������ ���������� ������
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 8,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 8,
			BDTH_SPACEPWR = 0,
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX,
		#endif /* WITHSHOWSWRPWR */
			BDCV_ALLRX = ROWS2GRID(1)		// ���������� ������, ���������� ��� S-����, ��������, ���� �����������
		};
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x00	//0x00
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3,	0,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7,	0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0,	2,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
			{	6,	2,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 2,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0,	4,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	19, 4,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0,	9,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	8,	9,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	19, 9,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0,	14,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	18, 14,	display_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 2,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#endif /* DSTYLE_UR3LMZMOD */

#elif DSTYLE_G_X220_Y176
	// ��������� 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 � ������������ ILI9225�
	// x= 27 characters
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP

		enum
		{
			BDTH_ALLRX = 27,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 14,	// ������ ���������� ������
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 16,
			BDTH_SPACEPWR = 1
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		// 27 horisontal places
		//                    "012345678901234567890123456"
			#define SMETERMAP "1  3  5  7  9 + 20  40  60 "
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
			#define SWRPWRMAP "1 | 2 | 3 0%     |    100% "
		#else
			#define POWERMAP  "0  10  20  40  60  80  100"
			#define SWRMAP    "1    -    2    -    3    -"	// 
			#define SWRMAX	(SWRMIN * 36 / 10)	// 3.6 - �������� �� ������ �����
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x81	//0x00
		};

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	6, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	20, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	24, 0,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			
			{	0, 5,	display_freqXbig_a, REDRM_FREQ, REDRSUBSET(DPAGE0), },

			{	0, 11,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	11, 11, display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	24, 11,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 14,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0, 17,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// ����������� ��������� ����� S-�����

			{	0, 20,	display_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	9, 20,	display_voxtune4,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	14, 20,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 20,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	22, 20,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 2,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#elif DSTYLE_UR3LMZMOD
		// KEYBSTYLE_SW2013SF_US2IT
		enum
		{
			BDTH_ALLRX = 27,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 14,	// ������ ���������� ������
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 16,
			BDTH_SPACEPWR = 1
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		// 27 horisontal places
		//                    "012345678901234567890123456"
			#define SMETERMAP "1  3  5  7  9 + 20  40  60 "
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
			#define SWRPWRMAP "1 | 2 | 3 0%     |    100% "
		#else
			#define POWERMAP  "0  10  20  40  60  80  100"
			#define SWRMAP    "1    -    2    -    3    -"	// 
			#define SWRMAX	(SWRMIN * 36 / 10)	// 3.6 - �������� �� ������ �����
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x81	//0x00
		};

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	6, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	20, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	24, 0,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			
			{	0, 5,	display_freqXbig_a, REDRM_FREQ, REDRSUBSET(DPAGE0), },

			{	0, 11,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	11, 11, display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	24, 11,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 14,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0, 17,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// ����������� ��������� ����� S-�����

			{	0, 20,	display_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	9, 20,	display_voxtune4,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	14, 20,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 20,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	22, 20,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 2,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#else /* DSTYLE_UR3LMZMOD */

		enum
		{
			BDTH_ALLRX = 17,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 6,	// ������ ���������� ������
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 8,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 8,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x00	//0x00
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	4, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	15, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	23, 0,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 4,	display_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	19, 10,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0, 10,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	8, 10,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	19, 13,	display_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0, 14,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER

		#if defined (RTC1_TYPE)
			{	0, 20,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
		#endif /* defined (RTC1_TYPE) */
			{	10, 20,	display_voxtune4,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	15, 20,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 20,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	23, 20,	display_voltlevel4, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter without "V"
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 2,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#endif /* DSTYLE_UR3LMZMOD */

#elif DSTYLE_G_X240_Y128
	// WO240128A
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// x=30, y=16

		enum
		{
			BDTH_ALLRX = 30,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_LEFTRX = 15,	// ������ ���������� ������
			BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ������ ���������� ������ 
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 20,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890123456789"
		#define SMETERMAP 	  "S 1  3  5  7  9  +20  +40  +60"	//
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%      50%     100%" 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		#else
			#error Not designed for work DSTYLE_UR3LMZMOD without WITHSHOWSWRPWR
			//#define POWERMAP  " 0 10 20 40 60 80 100"
			//#define SWRMAP    "1    |    2    |   3 "	// 
			//#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - �������� �� ������ �����
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
		/* ---------------------------------- */
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3,	0,	display_ant5,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	9,	0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// attenuator state
			{	21, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	25,	0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	27, 0,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	2,	display_freqXbig_a, REDRM_FREQ, REDRSUBSET(DPAGE0), },	// fullwidth = 8 constantly
		/* ---------------------------------- */
			//{	0,	8,	display_mainsub3, REDRM_MODE, REDRSUBSET(DPAGE0), },	// main/sub RX
			{	4,	8,	display_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPL
			{	12, 8,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	27, 8,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	11,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0,	12,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// ����������� ��������� ����� S-�����
			/* ---------------------------------- */
			{	0,	14,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if WITHNOTCHONOFF || WITHNOTCHFREQ
			// see next {	4,	14,	display_notch5,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ  */
	#if WITHAUTOTUNER
			{	4,	14,	display_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	8,	14,	display_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#endif /* WITHAUTOTUNER  */
		/* ---------------------------------- */
	#if WITHVOLTLEVEL
			{	12,	14,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
	#endif /* WITHVOLTLEVEL  */
	#if defined (RTC1_TYPE)
			//{	18,	14,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
			{	18,	14,	display_datetime12,	REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
	#endif /* defined (RTC1_TYPE) */
		#if WITHNOTCHONOFF || WITHNOTCHFREQ
			{	27, 14,	display_notch3, REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
		/* ---------------------------------- */
	#if WITHMENU
			{	0,	0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0,	2,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4,	2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#if WITHVOLTLEVEL && WITHCURRLEVEL
			{	0,	9,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET_MENU, },	// voltmeter with "V"
			{	6,	9,	display_currlevelA6, REDRM_VOLT, REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#endif /* WITHMENU */
		};

	#elif DSTYLE_UR3LMZMOD
		// x=30, y=16

		enum
		{
			BDCV_ALLRX = 7,		// ���������� �����, ���������� ��� S-����, ��������, ���� �����������
			BDTH_ALLRX = 30,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_LEFTRX = 15,	// ������ ���������� ������
			BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ������ ���������� ������ 
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 20,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890123456789"
		#define SMETERMAP 	  "S 1  3  5  7  9  +20  +40  +60"	//
		//#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%      50%     100%" 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		//#else
		//	#error Not designed for work DSTYLE_UR3LMZMOD without WITHSHOWSWRPWR
			//#define POWERMAP  " 0 10 20 40 60 80 100"
			//#define SWRMAP    "1    |    2    |   3 "	// 
			//#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - �������� �� ������ �����
		//#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		};
		//#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DPAGE1,					// ��������, � ������� ������������ �������� (��� ���) 
			DPAGE2,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		enum
		{
			PG0 = REDRSUBSET(DPAGE0),
			PG1 = REDRSUBSET(DPAGE1),
			PG2 = REDRSUBSET(DPAGE2),
			PGALL = PG0 | PG1 | PG2 | REDRSUBSET_MENU,
			PGLATCH = PGALL,
			PGunused
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
		/* ---------------------------------- */
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display_ant5,		REDRM_MODE, PGALL, },
			{	9,	0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf
			{	14,	0,	display_pre3,		REDRM_MODE, PGALL, },	// pre
			{	18, 0,	display_att4,		REDRM_MODE, PGALL, },	// attenuator state
			{	22,	0,	display_lockstate1, REDRM_MODE, PGALL, },
			{	23, 0,	display_rxbw3,		REDRM_MODE, PGALL, },
			{	27, 0,	display_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	2,	display_freqXbig_a, REDRM_FREQ, PGALL, },	// fullwidth = 8 constantly
		#if ! WITHDSPEXTDDC
			{	27, 2,	display_agc3,		REDRM_MODE, PGALL, },
		#endif /* ! WITHDSPEXTDDC */
			{	27, 4,	display_voxtune3,	REDRM_MODE, PGALL, },
		/* ---------------------------------- */
		#if WITHUSEAUDIOREC
			{	0,	7,	display_rec3,		REDRM_BARS, PGALL, },	// ����������� ������ ������ ����� ���������
		#endif /* WITHUSEAUDIOREC */
			{	4,	7,	display_mainsub3, REDRM_MODE, PGALL, },	// main/sub RX
			{	8,	7,	display_vfomode3,	REDRM_MODE, PGALL, },	// SPL
			{	16, 7,	display_freqX_b,	REDRM_FREQB, PGALL, },
			{	27, 7,	display_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	9,	display2_bars,		REDRM_BARS, PG0, },	// S-METER, SWR-METER, POWER-METER
			{	0,	10,	display2_legend,	REDRM_MODE, PG0, },	// ����������� ��������� ����� S-�����
			/* ---------------------------------- */
			{	0,	9,	dsp_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// ������������ ������ ������� ��� ������������ ����������� ������� ��� ��������
		#if LCDMODE_LTDC_PIP16
			{	0,	9,	display2_waterfallbg,REDRM_MODE,	PG1 | PG2, },
		#endif /* LCDMODE_LTDC_PIP16 */
			{	0,	9,	display2_spectrum,	REDRM_BARS, PG1, },// ����������� ��������� ����� S-�����
			{	0,	9,	display2_waterfall,	REDRM_BARS, PG2, },// ����������� ��������
			/* ---------------------------------- */
		#if defined (RTC1_TYPE)
			{	0,	14,	display_time5,		REDRM_BARS, PG0, },	// TIME
		#endif /* defined (RTC1_TYPE) */
		#if WITHVOLTLEVEL
			{	6,	14,	display_voltlevelV5, REDRM_VOLT, PG0 | REDRSUBSET_MENU, },	// voltmeter with "V"
		#endif /* WITHVOLTLEVEL  */
		#if WITHCURRLEVEL
			{	11, 14,	display_currlevelA6, REDRM_VOLT, PG0 | REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /*  WITHCURRLEVEL */
		#if WITHAMHIGHKBDADJ
			{	6, 14,	display_amfmhighcut3,REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHAMHIGHKBDADJ */
		#if WITHSAM
			{	18, 14,	display_samfreqdelta8, REDRM_BARS, PG0 | REDRSUBSET_MENU, },	/* �������� ���������� �� ������ ��������� � ������ SAM */
		#endif /* WITHSAM */
		#if WITHNOTCHONOFF || WITHNOTCHFREQ
			{	27, 14,	display_notch3, REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
		/* ---------------------------------- */
	#if WITHMENU
			{	0,	9,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0,	11,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4,	11,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			//{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#if WITHVOLTLEVEL && WITHCURRLEVEL
			//{	0,	9,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET_MENU, },	// voltmeter with "V"
			//{	6,	9,	display_currlevelA6, REDRM_VOLT, REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#endif /* WITHMENU */
		};

	#elif DSTYLE_UA1CEIMOD
		// x=30, y=16

		enum
		{
			BDCV_ALLRX = 7,		// ���������� �����, ���������� ��� S-����, ��������, ���� �����������
			BDTH_ALLRX = 30,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_LEFTRX = 15,	// ������ ���������� ������
			BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ������ ���������� ������ 
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 20,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890123456789"
		#define SMETERMAP 	  "S 1  3  5  7  9  +20  +40  +60"	//
		//#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%      50%     100%" 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		//#else
		//	#error Not designed for work DSTYLE_UR3LMZMOD without WITHSHOWSWRPWR
			//#define POWERMAP  " 0 10 20 40 60 80 100"
			//#define SWRMAP    "1    |    2    |   3 "	// 
			//#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - �������� �� ������ �����
		//#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		};
		//#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DPAGE1,					// ��������, � ������� ������������ �������� (��� ���) 
			DPAGE2,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		enum
		{
			PG0 = REDRSUBSET(DPAGE0),
			PG1 = REDRSUBSET(DPAGE1),
			PG2 = REDRSUBSET(DPAGE2),
			PGALL = PG0 | PG1 | PG2 | REDRSUBSET_MENU,
			PGLATCH = PGALL,
			PGunused
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
		/* ---------------------------------- */
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display_ant5,		REDRM_MODE, PGALL, },
			{	9,	0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf
			{	14,	0,	display_pre3,		REDRM_MODE, PGALL, },	// pre
			{	18, 0,	display_att4,		REDRM_MODE, PGALL, },	// attenuator state
			{	22,	0,	display_lockstate1, REDRM_MODE, PGALL, },
			{	23, 0,	display_rxbw3,		REDRM_MODE, PGALL, },
			{	27, 0,	display_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	2,	display_freqXbig_a, REDRM_FREQ, PGALL, },	// fullwidth = 8 constantly
			{	27, 2,	display_agc3,		REDRM_MODE, PGALL, },
			{	27, 4,	display_voxtune3,	REDRM_MODE, PGALL, },
		/* ---------------------------------- */
		#if WITHUSEAUDIOREC
			{	0,	7,	display_rec3,		REDRM_BARS, PGALL, },	// ����������� ������ ������ ����� ���������
		#endif /* WITHUSEAUDIOREC */
			{	4,	7,	display_mainsub3, REDRM_MODE, PGALL, },	// main/sub RX
			{	8,	7,	display_vfomode3,	REDRM_MODE, PGALL, },	// SPL
			{	16, 7,	display_freqX_b,	REDRM_FREQB, PGALL, },
			{	27, 7,	display_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	9,	display2_bars,		REDRM_BARS, PG0, },	// S-METER, SWR-METER, POWER-METER
			{	0,	10,	display2_legend,	REDRM_MODE, PG0, },	// ����������� ��������� ����� S-�����
			/* ---------------------------------- */
			{	0,	9,	dsp_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// ������������ ������ ������� ��� ������������ ����������� ������� ��� ��������
		#if LCDMODE_LTDC_PIP16
			{	0,	9,	display2_waterfallbg,REDRM_MODE,	PG1 | PG2, },
		#endif /* LCDMODE_LTDC_PIP16 */
			{	0,	9,	display2_spectrum,	REDRM_BARS, PG1, },// ����������� ��������� ����� S-�����
			{	0,	9,	display2_waterfall,	REDRM_BARS, PG2, },// ����������� ��������
			/* ---------------------------------- */
		#if defined (RTC1_TYPE)
			{	0,	14,	display_time5,		REDRM_BARS, PG0, },	// TIME
		#endif /* defined (RTC1_TYPE) */
		#if WITHVOLTLEVEL
			{	6,	14,	display_voltlevelV5, REDRM_VOLT, PG0 | REDRSUBSET_MENU, },	// voltmeter with "V"
		#endif /* WITHVOLTLEVEL  */
		#if WITHCURRLEVEL
			{	11, 14,	display_currlevelA6, REDRM_VOLT, PG0 | REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /*  WITHCURRLEVEL */
		#if WITHAMHIGHKBDADJ
			{	6, 14,	display_amfmhighcut3,REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHAMHIGHKBDADJ */
		#if WITHSAM
			{	18, 14,	display_samfreqdelta8, REDRM_BARS, PG0 | REDRSUBSET_MENU, },	/* �������� ���������� �� ������ ��������� � ������ SAM */
		#endif /* WITHSAM */
		#if WITHNOTCHONOFF || WITHNOTCHFREQ
			{	27, 14,	display_notch3, REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
		/* ---------------------------------- */
	#if WITHMENU
			{	0,	9,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0,	11,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4,	11,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			//{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#if WITHVOLTLEVEL && WITHCURRLEVEL
			//{	0,	9,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET_MENU, },	// voltmeter with "V"
			//{	6,	9,	display_currlevelA6, REDRM_VOLT, REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#endif /* WITHMENU */
		};

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */
		// RA1AGO version
		// x=30, y=16

		enum
		{
			BDCV_ALLRX = 7,		// ���������� �����, ���������� ��� S-����, ��������, ���� �����������
			BDTH_ALLRX = 30,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_LEFTRX = 15,	// ������ ���������� ������
			BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ������ ���������� ������ 
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 20,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890123456789"
		#define SMETERMAP 	  "S 1  3  5  7  9  +20  +40  +60"	//
		//#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%      50%     100%" 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - �������� �� ������ �����
		//#else
		//	#error Not designed for work DSTYLE_UR3LMZMOD without WITHSHOWSWRPWR
			//#define POWERMAP  " 0 10 20 40 60 80 100"
			//#define SWRMAP    "1    |    2    |   3 "	// 
			//#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - �������� �� ������ �����
		//#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		};
		//#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		#if WITHDSPEXTDDC
			DPAGE1,					// ��������, � ������� ������������ �������� (��� ���) 
			DPAGE2,					// ��������, � ������� ������������ �������� (��� ���) 
		#endif /* WITHDSPEXTDDC */
			DISPLC_MODCOUNT
		};
		enum
		{
			PG0 = REDRSUBSET(DPAGE0),
		#if WITHDSPEXTDDC
			PG1 = REDRSUBSET(DPAGE1),
			PG2 = REDRSUBSET(DPAGE2),
			PGALL = PG0 | PG1 | PG2 | REDRSUBSET_MENU,
		#else /* WITHDSPEXTDDC */
			PGALL = PG0 | REDRSUBSET_MENU,
		#endif /* WITHDSPEXTDDC */
			PGLATCH = PGALL,
			PGunused
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
		/* ---------------------------------- */
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display_ant5,		REDRM_MODE, PGALL, },
		#if WITHDSPEXTDDC
			{	9,	0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf
			{	14,	0,	display_pre3,		REDRM_MODE, PGALL, },	// pre
			{	18, 0,	display_att4,		REDRM_MODE, PGALL, },	// attenuator state
			{	22,	0,	display_lockstate1, REDRM_MODE, PGALL, },
		#else /* WITHDSPEXTDDC */
			{	9,	0,	display_pre3,		REDRM_MODE, PGALL, },	// pre
			{	13, 0,	display_att4,		REDRM_MODE, PGALL, },	// attenuator state
			{	18,	0,	display_lockstate4, REDRM_MODE, PGALL, },
		#endif /* WITHDSPEXTDDC */
			{	23, 0,	display_rxbw3,		REDRM_MODE, PGALL, },
			{	27, 0,	display_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	2,	display_freqXbig_a, REDRM_FREQ, PGALL, },	// fullwidth = 8 constantly
		#if ! WITHDSPEXTDDC
			{	27, 2,	display_agc3,		REDRM_MODE, PGALL, },
		#endif /* ! WITHDSPEXTDDC */
			{	27, 4,	display_voxtune3,	REDRM_MODE, PGALL, },
		/* ---------------------------------- */
		#if WITHUSEAUDIOREC
			{	0,	7,	display_rec3,		REDRM_BARS, PGALL, },	// ����������� ������ ������ ����� ���������
		#endif /* WITHUSEAUDIOREC */
			{	4,	7,	display_mainsub3, REDRM_MODE, PGALL, },	// main/sub RX
			{	8,	7,	display_vfomode3,	REDRM_MODE, PGALL, },	// SPL
			{	16, 7,	display_freqX_b,	REDRM_FREQB, PGALL, },
			{	27, 7,	display_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	9,	display2_bars,		REDRM_BARS, PG0 | REDRSUBSET_MENU, },	// S-METER, SWR-METER, POWER-METER
			{	0,	10,	display2_legend,	REDRM_MODE, PG0 | REDRSUBSET_MENU, },	// ����������� ��������� ����� S-�����
			/* ---------------------------------- */
		#if WITHDSPEXTDDC

			{	0,	9,	dsp_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// ������������ ������ ������� ��� ������������ ����������� ������� ��� ��������
		#if LCDMODE_LTDC_PIP16
			{	0,	9,	display2_waterfallbg,REDRM_MODE,	PG1 | PG2, },
		#endif /* LCDMODE_LTDC_PIP16 */
			{	0,	9,	display2_spectrum,	REDRM_BARS, PG1, },// ����������� ��������� ����� S-�����
			{	0,	9,	display2_waterfall,	REDRM_BARS, PG2, },// ����������� ��������
		#else /* WITHDSPEXTDDC */
			{	27, 12,	display_atu3,		REDRM_MODE, PGALL, },	// ATU
			{	27, 14,	display_byp3,		REDRM_MODE, PGALL, },	// BYP
		#endif /* WITHDSPEXTDDC */
			/* ---------------------------------- */
		#if defined (RTC1_TYPE)
			{	0,	14,	display_time5,		REDRM_BARS, PG0, },	// TIME
		#endif /* defined (RTC1_TYPE) */
		#if WITHVOLTLEVEL
			{	6,	14,	display_voltlevelV5, REDRM_VOLT, PG0, },	// voltmeter with "V"
		#endif /* WITHVOLTLEVEL  */
		#if WITHCURRLEVEL
			{	11, 14,	display_currlevelA6, REDRM_VOLT, PG0, },	// amphermeter with "A"
		#endif /*  WITHCURRLEVEL */
		#if WITHAMHIGHKBDADJ
			{	6, 14,	display_amfmhighcut3,REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHAMHIGHKBDADJ */
		#if WITHSAM
			{	18, 14,	display_samfreqdelta8, REDRM_BARS, PG0 | REDRSUBSET_MENU, },	/* �������� ���������� �� ������ ��������� � ������ SAM */
		#endif /* WITHSAM */
		#if WITHNOTCHONOFF || WITHNOTCHFREQ
			{	27, 14,	display_notch3, REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
		/* ---------------------------------- */
	#if WITHMENU
			{	0,	12,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0,	14,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4,	14,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			//{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#if WITHVOLTLEVEL && WITHCURRLEVEL
			//{	0,	14,	display_voltlevelV5, REDRM_VOLT, REDRSUBSET_MENU, },	// voltmeter with "V"
			//{	6,	14,	display_currlevelA6, REDRM_VOLT, REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#endif /* WITHMENU */
		};

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

#elif DSTYLE_G_X320_Y240
	// TFT ������ 320 * 240 ADI_3.2_AM-240320D4TOQW-T00H(R)
	// 320*240 SF-TC240T-9370-T � ������������ ILI9341
	// 32*15 ��������� 10*16
	#define CHAR_W	10
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// TFT ������ 320 * 240
		enum
		{
			BDTH_ALLRX = 20,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 11,	// ������ ���������� ������
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 3,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 16,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		#define SMETERMAP "1 3 5 7 9 + 20 40 60"
		#define POWERMAP  "0 10 20 40 60 80 100"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x00,
			PATTERN_BAR_EMPTYHALF = 0x00
		};

		enum {
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 0,	display_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_preovf3,	REDRM_BARS, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 8,	display_freqXbig_a, REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	19, 8,	display_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 18,	display_vfomode5,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	5, 18,	display_freqX_b,	REDRM_FREQB, REDRSUBSET(DPAGE0), },
			{	19, 18,	display_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	1, 24,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		#if defined (RTC1_TYPE)
			{	0, 28,	display_time8,		REDRM_BARS, REDRSUBSET_MENU | REDRSUBSET(DPAGE0), },	// TIME
		#endif /* defined (RTC1_TYPE) */
			{	18, 28,	display_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		#if WITHMENU
			{	0, 0,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 2,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 2,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	16, 0,	display_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// ��������� ���������� ���������
		#endif /* WITHMENU */
		};

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */
		// TFT ������ 320 * 240
		// ��� �����
		enum
		{
			BDCV_ALLRX = 8,		// ���������� �����, ���������� ��� S-����, ��������, ���� �����������
			BDTH_ALLRX = 26,	// ������ ���� ��� ����������� ������ �� ����������
			BDTH_RIGHTRX = 16,	// ������ ���������� ������
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ������ ���������� ������
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
			BDTH_ALLSWR = 8,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 8,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		enum
		{
			PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x00,
			PATTERN_BAR_EMPTYHALF = 0x00
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����

		enum 
		{
			DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		#if WITHIF4DSP
			DPAGE1,					// ��������, � ������� ������������ ������ 
			DPAGE2,					// ��������, � ������� ������������ �������
		#endif /* WITHIF4DSP */
			DISPLC_MODCOUNT
		};
		enum {
			PG0 = REDRSUBSET(DPAGE0),
		#if WITHIF4DSP
			PG1 = REDRSUBSET(DPAGE1),
			PG2 = REDRSUBSET(DPAGE2),
			PGALL = PG0 | PG1 | PG2 | REDRSUBSET_MENU,
		#else /* WITHIF4DSP */
			PGALL = PG0 | REDRSUBSET_MENU,
		#endif /* WITHIF4DSP */
			PGLATCH = PGALL,	// ��������, �� ������� �������� ����������� �������� ��� ��������.
			PGunused
		};
		#define DISPLC_WIDTH	8	// ���������� ���� � ����������� �������
		#define DISPLC_RJ		1	// ���������� ������� ������ ���� � ����������� �������
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display_voxtune3,	REDRM_MODE, PGALL, },
			{	7,	0,	display_att4,		REDRM_MODE, PGALL, },
			{	12, 0,	display_pre3,		REDRM_MODE, PGALL, },
		#if WITHDSPEXTDDC
			{	16, 0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf/pre
		#endif /* WITHDSPEXTDDC */
			{	20, 0,	display_lockstate4, REDRM_MODE, PGALL, },
			{	25, 0,	display_agc3,		REDRM_MODE, PGALL, },
			{	29, 0,	display_rxbw3,		REDRM_MODE, PGALL, },

			{	0,	8,	display_freqXbig_a, REDRM_FREQ, PGALL, },
			{	29, 8,	display_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
			//---
			{	0,	16,	display_vfomode5,	REDRM_MODE, PGALL, },	// SPLIT
			{	6,	16,	display_freqX_b,	REDRM_FREQB, PGALL, },
		#if WITHUSEDUALWATCH
			{	25, 16,	display_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX
		#endif /* WITHUSEDUALWATCH */
			{	29, 16,	display_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
			//---
			{	0,	19,	display2_bars,		REDRM_BARS, PG0, },	// S-METER, SWR-METER, POWER-METER
		#if WITHIF4DSP
			{	0,	19,	dsp_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// ������������ ������ ������� ��� ������������ ����������� ������� ��� ��������
		#if LCDMODE_LTDC_PIP16
			{	0,	19,	display2_waterfallbg,REDRM_MODE,	PG1 | PG2, },
		#endif /* LCDMODE_LTDC_PIP16 */
			{	0,	19,	display2_spectrum,	REDRM_BARS, PG1, },// ����������� �������
			{	0,	19,	display2_waterfall,	REDRM_BARS, PG2, },// ����������� ��������

			{	27, 19,	display_siglevel5,	REDRM_BARS, PGALL, },	// signal level
		#endif /* WITHIF4DSP */
			//---
		#if WITHSAM
			//{	22, 25,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* �������� ���������� �� ������ ��������� � ������ SAM */
		#endif /* WITHSAM */

		#if WITHVOLTLEVEL
			{	0, 28,	display_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
		#endif /* WITHVOLTLEVEL */
		#if WITHCURRLEVEL
			{	6, 28,	display_currlevelA6, REDRM_VOLT, PGALL, },	// amphermeter with "A"
		#endif /* WITHCURRLEVEL */
		#if defined (RTC1_TYPE)
			{	13, 28,	display_time8,		REDRM_BARS, PGALL, },	// TIME
		#endif /* defined (RTC1_TYPE) */
		#if WITHUSEAUDIOREC
			{	25, 28,	display_rec3,		REDRM_BARS, PGALL, },	// ����������� ������ ������ ����� ���������
		#endif /* WITHUSEAUDIOREC */
		#if WITHMENU
			{	0, 19,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
			{	0, 21,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
			{	4, 21,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
			{	9,	24,	display_freqmeter10,	REDRM_VOLT, REDRSUBSET_MENU, },	// ���������� ������� ���������� ������� �������
		#if WITHSAM
			{	9, 26,	display_samfreqdelta8, REDRM_BARS, REDRSUBSET_MENU, },	/* �������� ���������� �� ������ ��������� � ������ SAM */
		#endif /* WITHSAM */
		#endif /* WITHMENU */
		};

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

#elif DSTYLE_G_X480_Y272

	// TFT ������ SONY PSP-1000
	// 272/15 = 18, 480/16=30


	#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
		//					"012345678901234567890123"
		#define SWRPWRMAP	"1   2   3   4  0% | 100%" 
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����
	#else
		//					"012345678901234567890123"
		#define POWERMAP	"0    25    50   75   100"
		#define SWRMAP		"1   |   2  |   3   |   4"	// 
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - �������� �� ������ �����
	#endif
	//						"012345678901234567890123"
	#define SMETERMAP		"1  3  5  7  9 +20 +40 60"
	enum
	{
		BDTH_ALLRX = 24,	// ������ ���� ��� ����������� ������ �� ����������
		BDTH_LEFTRX = 12,	// ������ ���������� ������
		BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ������ ���������� ������
		BDTH_SPACERX = 0,
	#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
		BDTH_ALLSWR = 13,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 9,
		BDTH_SPACEPWR = 0,
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX,
	#endif /* WITHSHOWSWRPWR */
		BDCV_ALLRX = ROWS2GRID(7)		// ���������� ������, ���������� ��� S-����, ��������, ���� �����������
	};

	enum
	{
		PATTERN_SPACE = 0x00,	/* ������� ����� �� SWR � PWR ������ ���� �������� */
		PATTERN_BAR_FULL = 0xFF,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x00	//0x00
	};

	enum 
	{
		DPAGE0,					// ��������, � ������� ������������ �������� (��� ���) 
		DPAGE1,					// ��������, � ������� ������������ ������ 
		DPAGE2,					// ��������, � ������� ������������ �������
		DISPLC_MODCOUNT
	};

	enum
	{
		PG0 = REDRSUBSET(DPAGE0),
		PG1 = REDRSUBSET(DPAGE1),
		PG2 = REDRSUBSET(DPAGE2),
		PGALL = PG0 | PG1 | PG2 | REDRSUBSET_MENU,
		PGLATCH = PGALL,	// ��������, �� ������� �������� ����������� �������� ��� ��������.
		PGunused
	};
	#define DISPLC_WIDTH	9	// ���������� ���� � ����������� �������
	#define DISPLC_RJ		0	// ���������� ������� ������ ���� � ����������� �������
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
		{	3,	0,	display_ant5,		REDRM_MODE, PGALL, },
		{	9,	0,	display_att4,		REDRM_MODE, PGALL, },
		{	14,	0,	display_preovf3,	REDRM_BARS, PGALL, },
		{	18,	0,	display_genham1,	REDRM_BARS, PGALL, },
		//{	17, 0,	display_agc3,		REDRM_MODE, PGALL, },
		{	21, 0,	display_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR

		{	25, 0,	display_notch5,		REDRM_MODE, PGALL, },
		{	26,	3,	display_vfomode3,	REDRM_MODE, PGALL, },	// SPLIT
		{	26, 5,	display_voxtune3,	REDRM_MODE, PGALL, },	// VOX
		{	26, 7,	display_atu3,		REDRM_MODE, PGALL, },
		{	26, 9,	display_byp3,		REDRM_MODE, PGALL, },
		{	26, 11,	display_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX
		{	26, 13,	display_rec3,		REDRM_BARS, PGALL, },	// ����������� ������ ������ ����� ���������
		{	26, 15,	display_lockstate4, REDRM_MODE, PGALL, },	// LOCK
		{	25, 17,	display_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"

		
		{	0,	2,	display_freqX_a,	REDRM_FREQ, PGALL, },	// ������� (������� �����)
		{	21, 3,	display_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	21, 5,	display_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator

		{	9,	7,	display_freqX_b,	REDRM_FREQB, PGALL, },
		{	21, 7,	display_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...

		{	0,	9,	display2_legend,	REDRM_MODE, PG0, },// ����������� ��������� ����� S-�����
		{	0,	10,	display2_bars,		REDRM_BARS, PG0, },	// S-METER, SWR-METER, POWER-METER

		{	0,	9,	dsp_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// ������������ ������ ������� ��� ������������ ����������� ������� ��� ��������
	#if LCDMODE_LTDC_PIP16
		{	0,	9,	display2_waterfallbg,REDRM_MODE,	PG1 | PG2, },
	#endif /* LCDMODE_LTDC_PIP16 */
		{	0,	9,	display2_spectrum,	REDRM_BARS, PG1, },// ����������� �������
		{	0,	9,	display2_waterfall,	REDRM_BARS, PG2, },// ����������� ��������

		{	0,	17,	display_time8,		REDRM_BARS, PGALL, },	// TIME
		{	9,	17,	display_siglevel5,	REDRM_BARS, PGALL, },	// signal level in S points
#if CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V3
		{	19, 17,	display_currlevel5alt, REDRM_VOLT, PGALL, },	// PA drain current dd.d without "A"
#else
		{	19, 17,	display_currlevel5, REDRM_VOLT, PGALL, },	// PA drain current d.dd without "A"
#endif

	#if WITHMENU
		{	0,	9,	display_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// �������� ���������
		{	0,	11,	display_menu_lblc3,	REDRM_MLBL, REDRSUBSET_MENU, },	// ��� �������������� ���������
		{	4,	11,	display_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// �������� �������������� ���������
	#endif /* WITHMENU */
	};

	/* �������� ���������� ���� � ��������� �/��� ���������. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = GRID2X(0);	// ������� �������� ������ ���� � ��������
		p->y = GRID2Y(9);	// ������� �������� ������ ���� � ��������
		p->w = GRID2X(CHARS2GRID(BDTH_ALLRX));	// ������ �� ����������� � ��������
		p->h = GRID2Y(BDCV_ALLRX);				// ������ �� ��������� � ��������
	}
#else
	#error TODO: to be implemented
#endif /* LCDMODE_LS020 */

void
//NOINLINEAT
display_menu_value(
	int_fast32_t value,
	uint_fast8_t width,	// full width (if >= 128 - display with sign)
	uint_fast8_t comma,		// comma position (from right, inside width)
	uint_fast8_t rj,		// right truncated
	uint_fast8_t lowhalf
	)
{
	display_value_small(value, width, comma, 255, rj, lowhalf);
}

//+++ bars
		
static uint_fast8_t display_mapbar(
	uint_fast8_t val, 
	uint_fast8_t bottom, uint_fast8_t top, 
	uint_fast8_t mapleft, 
	uint_fast8_t mapinside, 
	uint_fast8_t mapright
	)
{
	if (val < bottom)
		return mapleft;
	if (val < top)
		return mapinside;
	return mapright;
}
#if WITHBARS

// ���������� ����� � ����������� �������� �� ������
static uint_fast16_t display_getpwrfullwidth(void)
{
	return GRID2X(CHARS2GRID(BDTH_ALLPWR));
}

#if LCDMODE_HD44780
	// �� HD44780 ������������ �������������

#elif LCDMODE_S1D13781 && ! LCDMODE_LTDC


#else /* LCDMODE_HD44780 */

// ������ ���� ������� (��� ������ �������) ��������� "��������" ����� �������
// display_wrdatabar_begin() � display_wrdatabar_end().
//
void 
//NOINLINEAT
display_dispbar(
	uint_fast8_t width,	/* ���������� ���������, ���������� ����������� */
	uint_fast8_t value,		/* ��������, ������� ���� ���������� */
	uint_fast8_t tracevalue,		/* �������� �������, ������� ���� ���������� */
	uint_fast8_t topvalue,	/* ��������, ��������������� ��������� ������������ ���������� */
	uint_fast8_t pattern,	/* DISPLAY_BAR_HALF ��� DISPLAY_BAR_FULL */
	uint_fast8_t patternmax,	/* DISPLAY_BAR_HALF ��� DISPLAY_BAR_FULL - ��� ����������� ������������ �������� */
	uint_fast8_t emptyp			/* ������� ��� ���������� ����� �������� */
	)
{
	//enum { DISPLAY_BAR_LEVELS = 6 };	// ���������� �������� � ����� ����������

	enum { NCOLS = CHAR_W };
	value = value < 0 ? 0 : value;
	const uint_fast16_t wfull = width * NCOLS;
	const uint_fast16_t wpart = (uint_fast32_t) wfull * value / topvalue;
	const uint_fast16_t wmark = (uint_fast32_t) wfull * tracevalue / topvalue;
	uint_fast8_t i = 0;

	for (; i < wpart; ++ i)
	{
		if (i == wmark)
		{
			display_barcolumn(patternmax);
			continue;
		}
#if (DSTYLE_G_X132_Y64 || DSTYLE_G_X128_Y64) && DSTYLE_UR3LMZMOD
		display_barcolumn(pattern);
#elif DSTYLE_G_X64_Y32
		display_barcolumn((i % 6) != 5 ? pattern : emptyp);
#else
		display_barcolumn((i % 2) == 0 ? pattern : PATTERN_SPACE);
#endif
	}

	for (; i < wfull; ++ i)
	{
		if (i == wmark)
		{
			display_barcolumn(patternmax);
			continue;
		}
#if (DSTYLE_G_X132_Y64 || DSTYLE_G_X128_Y64) && DSTYLE_UR3LMZMOD
		display_barcolumn(emptyp);
#elif DSTYLE_G_X64_Y32
		display_barcolumn((i % 6) == 5 ? pattern : emptyp);
#else
		display_barcolumn((i % 2) == 0 ? emptyp : PATTERN_SPACE);
#endif
	}
}
#endif /* LCDMODE_HD44780 */

#endif /* WITHBARS */

// ��������� ��� s-meter
static void
display_bars_address_rx(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t xoffs	// grid
	)
{
	display_gotoxy(x + xoffs, y);
}

// ��������� ��� swr-meter
static void
display_bars_address_swr(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t xoffs	// grid
	)
{
	display_bars_address_rx(x, y, xoffs);
}

// ��������� ��� pwr-meter
static void
display_bars_address_pwr(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t xoffs	// grid
	)
{
#if WITHSHOWSWRPWR	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
	display_bars_address_rx(x, y, xoffs + CHARS2GRID(BDTH_ALLSWR + BDTH_SPACESWR));
#else
	display_bars_address_rx(x, y, xoffs);
#endif
}

// ���������� ��� ������ ����� PWR & SWR
void display_swrmeter(  
	uint_fast8_t x, 
	uint_fast8_t y, 
	adcvalholder_t f,	// forward, 
	adcvalholder_t r,	// reflected (�����������������)
	uint_fast16_t minforward
	)
{
#if WITHBARS
	// SWRMIN - �������� 10 - ������������� SWR = 1.0, �������� = 0.1
	// SWRMAX - ����� ����� ����� � ����� ����� SWR-����� (30 = ��� 3.0)
	const uint_fast16_t fullscale = SWRMAX - SWRMIN;
	uint_fast16_t swr10;		// ������������  ��������
	if (f < minforward)
		swr10 = 0;	// SWR=1
	else if (f <= r)
		swr10 = fullscale;		// SWR is infinite
	else
		swr10 = (f + r) * SWRMIN / (f - r) - SWRMIN;
	// v = 10..40 for swr 1..4
	// swr10 = 0..30 for swr 1..4
	const uint_fast8_t mapleftval = display_mapbar(swr10, 0, fullscale, 0, swr10, fullscale);

	//debug_printf_P(PSTR("swr10=%d, mapleftval=%d, fs=%d\n"), swr10, mapleftval, display_getmaxswrlimb());

	display_bars_address_swr(x, y, CHARS2GRID(0));

	display_setcolors(SWRCOLOR, BGCOLOR);

	display_wrdatabar_begin();
	display_dispbar(BDTH_ALLSWR, mapleftval, fullscale, fullscale, PATTERN_BAR_FULL, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYFULL);
	display_wrdatabar_end();

	if (BDTH_SPACESWR != 0)
	{
		// ��������� ������ ����� �� ���������� ���
		display_bars_address_swr(x, y, CHARS2GRID(BDTH_ALLSWR));
		display_wrdatabar_begin();
		display_dispbar(BDTH_SPACESWR, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}

// ���������� �� display2_bars_amv0 (������ ��� CTLSTYLE_RA4YBO_AM0)
// ���������� ��� ������ ����� PWR & SWR
// ������������ ����� ��� SWR
void display_modulationmeter_amv0(  
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,
	uint_fast8_t fullscale
	)
{
#if WITHBARS
	const uint_fast8_t mapleftval = display_mapbar(value, 0, fullscale, 0, value, fullscale);

	//debug_printf_P(PSTR("swr10=%d, mapleftval=%d, fs=%d\n"), swr10, mapleftval, display_getmaxswrlimb());

	display_setcolors(SWRCOLOR, BGCOLOR);
	display_bars_address_swr(x - 1, y, CHARS2GRID(0));
	display_string_P(PSTR("M"), 0);

	display_bars_address_swr(x, y, CHARS2GRID(0));

	display_setcolors(SWRCOLOR, BGCOLOR);

	display_wrdatabar_begin();
	display_dispbar(BDTH_ALLSWR, mapleftval, fullscale, fullscale, PATTERN_BAR_FULL, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYFULL);
	display_wrdatabar_end();

	if (BDTH_SPACESWR != 0)
	{
		// ��������� ������ ����� �� ���������� ���
		display_bars_address_swr(x, y, CHARS2GRID(BDTH_ALLSWR));
		display_wrdatabar_begin();
		display_dispbar(BDTH_SPACESWR, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}
// ���������� ��� ������ ����� PWR & SWR
// ���������� �� display2_bars_amv0 (������ ��� CTLSTYLE_RA4YBO_AM0)
void display_pwrmeter_amv0(  
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,			// ������� ��������
	uint_fast8_t tracemax,		// max hold ��������
	uint_fast8_t maxpwrcali		// �������� ��� ���������� �� ��� �����
	)
{
#if WITHBARS
	const uint_fast16_t fullscale = display_getpwrfullwidth();	// ���������� ����� � ����������� �������� �� ������
#if WITHPWRLIN
	uint_fast8_t v = (uint_fast32_t) value * fullscale / ((uint_fast32_t) maxpwrcali);
	uint_fast8_t t = (uint_fast32_t) tracemax * fullscale / ((uint_fast32_t) maxpwrcali);
#else /* WITHPWRLIN */
	uint_fast8_t v = (uint_fast32_t) value * value * fullscale / ((uint_fast32_t) maxpwrcali * maxpwrcali);
	uint_fast8_t t = (uint_fast32_t) tracemax * tracemax * fullscale / ((uint_fast32_t) maxpwrcali * maxpwrcali);
#endif /* WITHPWRLIN */
	const uint_fast8_t mapleftval = display_mapbar(v, 0, fullscale, 0, v, fullscale);
	const uint_fast8_t mapleftmax = display_mapbar(t, 0, fullscale, fullscale, t, fullscale); // fullscale - invisible

	display_setcolors(PWRCOLOR, BGCOLOR);
	display_bars_address_pwr(x - 1, y, CHARS2GRID(0));
	display_string_P(PSTR("P"), 0);

	display_bars_address_pwr(x, y, CHARS2GRID(0));

	display_setcolors(PWRCOLOR, BGCOLOR);

	display_wrdatabar_begin();
	display_dispbar(BDTH_ALLPWR, mapleftval, mapleftmax, fullscale, PATTERN_BAR_HALF, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYHALF);
	display_wrdatabar_end();

	if (BDTH_SPACEPWR != 0)
	{
		// ��������� ������ ����� �� ���������� ��������
		display_bars_address_pwr(x, y, CHARS2GRID(BDTH_ALLPWR));
		display_wrdatabar_begin();
		display_dispbar(BDTH_SPACEPWR, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}

void display_smeter_amv0(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,		// ������� ��������
	uint_fast8_t tracemax,	// ����� ������������ ���������
	uint_fast8_t level9,	// s9 level
	uint_fast8_t delta1,	// s9 - s0 delta
	uint_fast8_t delta2)	// s9+50 - s9 delta
{
#if WITHBARS
	tracemax = value > tracemax ? value : tracemax;	// ������ �� ��������������� ��������
	//delta1 = delta1 > level9 ? level9 : delta1;
	
	const uint_fast8_t leftmin = level9 - delta1;
	const uint_fast8_t mapleftval = display_mapbar(value, leftmin, level9, 0, value - leftmin, delta1);
	const uint_fast8_t mapleftmax = display_mapbar(tracemax, leftmin, level9, delta1, tracemax - leftmin, delta1); // delta1 - invisible
	const uint_fast8_t maprightval = display_mapbar(value, level9, level9 + delta2, 0, value - level9, delta2);
	const uint_fast8_t maprightmax = display_mapbar(tracemax, level9, level9 + delta2, delta2, tracemax - level9, delta2); // delta2 - invisible

	display_setcolors(LCOLOR, BGCOLOR);
	display_bars_address_rx(x - 1, y, CHARS2GRID(0));
	display_string_P(PSTR("S"), 0);

	display_bars_address_rx(x, y, CHARS2GRID(0));
	display_setcolors(LCOLOR, BGCOLOR);
	display_wrdatabar_begin();
	display_dispbar(BDTH_LEFTRX, mapleftval, mapleftmax, delta1, PATTERN_BAR_HALF, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYHALF);		//���� 9 ������ ������
	display_wrdatabar_end();
	//
	display_bars_address_rx(x, y, CHARS2GRID(BDTH_LEFTRX));
	display_setcolors(RCOLOR, BGCOLOR);
	display_wrdatabar_begin();
	display_dispbar(BDTH_RIGHTRX, maprightval, maprightmax, delta2, PATTERN_BAR_FULL, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYFULL);		// ���� 9 ������ ������ ���.
	display_wrdatabar_end();

	if (BDTH_SPACERX != 0)
	{
		display_bars_address_pwr(x, y, CHARS2GRID(BDTH_ALLRX));
		display_wrdatabar_begin();
		display_dispbar(BDTH_SPACERX, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}

// ���������� ��� ������ ����� PWR & SWR
void display_pwrmeter(  
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,			// ������� ��������
	uint_fast8_t tracemax,		// max hold ��������
	uint_fast8_t maxpwrcali		// �������� ��� ���������� �� ��� �����
	)
{
#if WITHBARS
	const uint_fast16_t fullscale = display_getpwrfullwidth();	// ���������� ����� � ����������� �������� �� ������
#if WITHPWRLIN
	uint_fast8_t v = (uint_fast32_t) value * fullscale / ((uint_fast32_t) maxpwrcali);
	uint_fast8_t t = (uint_fast32_t) tracemax * fullscale / ((uint_fast32_t) maxpwrcali);
#else /* WITHPWRLIN */
	uint_fast8_t v = (uint_fast32_t) value * value * fullscale / ((uint_fast32_t) maxpwrcali * maxpwrcali);
	uint_fast8_t t = (uint_fast32_t) tracemax * tracemax * fullscale / ((uint_fast32_t) maxpwrcali * maxpwrcali);
#endif /* WITHPWRLIN */
	const uint_fast8_t mapleftval = display_mapbar(v, 0, fullscale, 0, v, fullscale);
	const uint_fast8_t mapleftmax = display_mapbar(t, 0, fullscale, fullscale, t, fullscale); // fullscale - invisible

	display_bars_address_pwr(x, y, CHARS2GRID(0));

	display_setcolors(PWRCOLOR, BGCOLOR);

	display_wrdatabar_begin();
	display_dispbar(BDTH_ALLPWR, mapleftval, mapleftmax, fullscale, PATTERN_BAR_HALF, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYHALF);
	display_wrdatabar_end();

	if (BDTH_SPACEPWR != 0)
	{
		// ��������� ������ ����� �� ���������� ��������
		display_bars_address_pwr(x, y, CHARS2GRID(BDTH_ALLPWR));
		display_wrdatabar_begin();
		display_dispbar(BDTH_SPACEPWR, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}

void display_smeter(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,		// ������� ��������
	uint_fast8_t tracemax,	// ����� ������������ ���������
	uint_fast8_t level9,	// s9 level
	uint_fast8_t delta1,	// s9 - s0 delta
	uint_fast8_t delta2)	// s9+50 - s9 delta
{
#if WITHBARS
	tracemax = value > tracemax ? value : tracemax;	// ������ �� ��������������� ��������
	//delta1 = delta1 > level9 ? level9 : delta1;
	
	const uint_fast8_t leftmin = level9 - delta1;
	const uint_fast8_t mapleftval = display_mapbar(value, leftmin, level9, 0, value - leftmin, delta1);
	const uint_fast8_t mapleftmax = display_mapbar(tracemax, leftmin, level9, delta1, tracemax - leftmin, delta1); // delta1 - invisible
	const uint_fast8_t maprightval = display_mapbar(value, level9, level9 + delta2, 0, value - level9, delta2);
	const uint_fast8_t maprightmax = display_mapbar(tracemax, level9, level9 + delta2, delta2, tracemax - level9, delta2); // delta2 - invisible

	display_bars_address_rx(x, y, CHARS2GRID(0));
	display_setcolors(LCOLOR, BGCOLOR);
	display_wrdatabar_begin();
	display_dispbar(BDTH_LEFTRX, mapleftval, mapleftmax, delta1, PATTERN_BAR_HALF, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYHALF);		//���� 9 ������ ������
	display_wrdatabar_end();
	//
	display_bars_address_rx(x, y, CHARS2GRID(BDTH_LEFTRX));
	display_setcolors(RCOLOR, BGCOLOR);
	display_wrdatabar_begin();
	display_dispbar(BDTH_RIGHTRX, maprightval, maprightmax, delta2, PATTERN_BAR_FULL, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYFULL);		// ���� 9 ������ ������ ���.
	display_wrdatabar_end();

	if (BDTH_SPACERX != 0)
	{
		display_bars_address_pwr(x, y, CHARS2GRID(BDTH_ALLRX));
		display_wrdatabar_begin();
		display_dispbar(BDTH_SPACERX, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}
//--- bars

// ����������� ����� S-����� � ������ �����������
static void display2_legend(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if defined(SMETERMAP)
	display_setcolors(MODECOLOR, BGCOLOR);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_gotoxy(x, y + lowhalf);
		if (hamradio_get_tx())
		{
	#if WITHTX
		#if WITHSWRMTR
			#if WITHSHOWSWRPWR /* �� ������� ������������ ������������ SWR-meter � PWR-meter */
					display_string_P(PSTR(SWRPWRMAP), lowhalf);
			#else
					if (swrmode) 	// ���� TUNE �� ���������� ����� ���
						display_string_P(PSTR(SWRMAP), lowhalf);
					else
						display_string_P(PSTR(POWERMAP), lowhalf);
			#endif
		#elif WITHPWRMTR
					display_string_P(PSTR(POWERMAP), lowhalf);
		#else
			#warning No TX indication
		#endif
	#endif /* WITHTX */
		}
		else
		{
			display_string_P(PSTR(SMETERMAP), lowhalf);
		}

	} while (lowhalf --);
#endif /* defined(SMETERMAP) */
#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */
}

#if WITHINTEGRATEDDSP && (WITHRTS96 || WITHRTS192) && ! LCDMODE_HD44780

enum 
{
	WFDX = GRID2X(CHARS2GRID(BDTH_ALLRX)),	// ������ �� ����������� � ��������
	WFDY = GRID2Y(BDCV_ALLRX)				// ������ �� ��������� � ��������
};


static PACKEDCOLOR_T * getnextpip(void)
{
#if LCDMODE_LTDC_PIP16

	static RAMNOINIT_D1 ALIGNX_BEGIN PACKEDCOLOR_T colorpips [2] [GXSIZE(WFDX, WFDY)] ALIGNX_END;
	static int phase;

	return colorpips [phase = ! phase];

#else /* LCDMODE_LTDC_PIP16 */

	static ALIGNX_BEGIN PACKEDCOLOR_T colorpip0 [GXSIZE(WFDX, WFDY)] ALIGNX_END;

	return colorpip0;
#endif /* LCDMODE_LTDC_PIP16 */
}

enum { AVGLEN = 2 };
enum { MAXHISTLEN = 5 };
enum { SPAVGSIZE = 5 };	// max(AVGLEN, MAXHISTLEN)
static FLOAT_t spavgarray [SPAVGSIZE] [WFDX];	// ������ ��� ����������
static uint_fast8_t spavgrow;				// ������, � ������� ��������� �������� ������

#if 1
	static uint8_t wfarray [WFDY] [WFDX];	// ������ "��������"
	static uint_fast16_t wfrow;				// ������, � ������� ��������� �������� ������
#else
	static uint8_t wfarray [1] [WFDX];	// ������ "��������"
	enum { wfrow = 0 };				// ������, � ������� ��������� �������� ������
#endif
enum { PALETTESIZE = 256 };
static PACKEDCOLOR_T wfpalette [PALETTESIZE];
extern uint_fast8_t wflfence;

#define COLOR_CENTERMAKER	COLOR_RED
#define COLOR_WAERFALLBG	COLOR_WHITE
#define COLOR_WAERFALLFG	COLOR_BLUE
#define COLOR_WAERFALLFENCE	COLOR_YELLOW

// ��� ���� �� ������� Malamute
static void wfpalette_initialize(void)
{
	int type = 0;
	int i;
	int a = 0;

	if (type)
	{
		for (i = 0; i < 42; ++ i)
		{
			wfpalette [a + i] = TFTRGB(0, 0, (int) (powf((float) 0.095 * i, 4)));
		}
		a += i;
		for (i = 0; i < 42; ++ i)
		{
			wfpalette [a + i] = TFTRGB(0, i * 6, 255);
		}
		a += i;
		for (i = 0; i < 42; ++ i)
		{
			wfpalette [a + i] = TFTRGB(0, 255, (int)(((float) 0.39 * (41 - i )) * ((float) 0.39 * (41 - i))) );
		}
		a += i;
		for (i = 0; i < 42; ++ i)
		{
			wfpalette [a + i] = TFTRGB(i * 6, 255, 0);
		}
		a += i;
		for (i = 0; i < 42; ++ i)
		{
			wfpalette [a + i] = TFTRGB(255, (41 - i) * 6, 0);
		}
		a += i;
		for (i = 0; i < 42; ++ i)
		{
			wfpalette [a + i] = TFTRGB(255, 0, i * 6);
		}
		a += i;
		// a = 252
	}
	else
	{

		// a = 0
		for (i = 0; i < 64; ++ i)
		{
			// ��� i = 0..15 ��������� ������� = ����
			wfpalette [a + i] = TFTRGB(0, 0, (int) (powf((float) 0.0625 * i, 4)));	// ��������� ��������� ����� �������� ��������� ������������� ����������!
		}
		a += i;
		// a = 64
		for (i = 0; i < 32; ++ i)
		{
			wfpalette [a + i] = TFTRGB(0, i * 8, 255);
		}
		a += i;
		// a = 96
		for (i = 0; i < 32; ++ i)
		{
			wfpalette [a + i] = TFTRGB(0, 255, 255 - i * 8);
		}
		a += i;
		// a = 128
		for (i = 0; i < 32; ++ i)
		{
			wfpalette [a + i] = TFTRGB(i * 8, 255, 0);
		}
		a += i;
		// a = 160
		for (i = 0; i<64; ++ i)
		{
			wfpalette [a + i] = TFTRGB(255, 255 - i * 4, 0);
		}
		a += i;
		// a = 224
		for (i = 0; i < 32; ++ i)
		{
			wfpalette [a + i] = TFTRGB(255, 0, i * 8);
		}
		a += i;
		// a = 256
	}
}


#define EEAVERAGE 1
#define EEMAXIMUM 0

// ������������ ������ ������� ��� ������������ �����������
// ������� ��� ��������
static void dsp_latchwaterfall(
	uint_fast8_t x0, 
	uint_fast8_t y0, 
	void * pv
	)
{
	uint_fast16_t x;
	(void) x0;
	(void) y0;
	(void) pv;


	// ����������� ���������� ������� ��� �������������
	spavgrow = (spavgrow == 0) ? (SPAVGSIZE - 1) : (spavgrow - 1);
	dsp_getspectrumrow(& spavgarray [spavgrow] [0], WFDX);

#if 1
	wfrow = (wfrow == 0) ? (WFDY - 1) : (wfrow - 1);
#endif

	// ����������� ���������� ������� ��� ��������
	for (x = 0; x < WFDX; ++ x)
	{
#if 0
		// ����������
		FLOAT_t mag = 0;
		uint_fast8_t h;
		for (h = 0; h < AVGLEN; ++ h)
			mag += spavgarray [(spavgrow + h) % SPAVGSIZE] [x];

		// �������� - � ������ �������
		const int val = dsp_mag2y(mag / AVGLEN, PALETTESIZE);
#elif 0
		// ��������
		FLOAT_t mag = 0;
		uint_fast8_t h;
		for (h = 0; h < MAXHISTLEN; ++ h)
			mag = FMAXF(mag, spavgarray [(spavgrow + h) % SPAVGSIZE] [x]);

		// �������� - � ������ �������
		const int val = dsp_mag2y(mag, PALETTESIZE);
#else
		// ��� ���������� ��� ��������
		const int val = dsp_mag2y(spavgarray [spavgrow] [x], PALETTESIZE);
#endif
		// ������ � ����� ��������
		wfarray [wfrow] [x] = val;
	}
}


#if LCDMODE_LTDC_PIP16
// ������������� ������������� �������� ������ 
static void display2_waterfallbg(
	uint_fast8_t x0, 
	uint_fast8_t y0, 
	void * pv
	)
{
	dma2d_fillrect2(& framebuff [0][0], DIM_X, DIM_Y, GRID2X(x0), GRID2Y(y0), WFDX, WFDY, COLOR_KEY);
}

#endif /* LCDMODE_LTDC_PIP16 */

// ����������� ������������������
static void display2_spectrum(
	uint_fast8_t x0, 
	uint_fast8_t y0, 
	void * pv
	)
{
#if LCDMODE_UC1608 || LCDMODE_UC1601 || LCDMODE_S1D13781
	// ������ �� ����������� �������� 
	// ��� �� �������,��� ���� ����������� �������� ���������� ��������.
	static ALIGNX_BEGIN GX_t wfmonoscr [MGSIZE(WFDX, WFDY)] ALIGNX_END;

	if (hamradio_get_tx() == 0)
	{
		uint_fast16_t x;
		uint_fast16_t y;
		// ������������ ������
		// ������ ����������� ������� ������
		memset(wfmonoscr, 0xFF, sizeof wfmonoscr);			// ��������� �������� �������� �����
		for (x = 0; x < WFDY; ++ x)
		{
			display_pixelbuffer(wfmonoscr, WFDX, WFDY, WFDX / 2, x);	// �������� �����
		}

		// ����������� �������
		for (x = 0; x < WFDX; ++ x)
		{
		#if EEAVERAGE
			// ����������
			FLOAT_t mag = 0;
			uint_fast8_t h;
			for (h = 0; h < AVGLEN; ++ h)
				mag += spavgarray [(spavgrow + h) % SPAVGSIZE] [x];

			// �������� - � ������������ ����������
			const int val = dsp_mag2y(mag / AVGLEN, WFDY);
		#elif EEMAXIMUM
			// ��������
			FLOAT_t mag = 0;
			uint_fast8_t h;
			for (h = 0; h < MAXHISTLEN; ++ h)
				mag = FMAXF(mag, spavgarray [(spavgrow + h) % SPAVGSIZE] [x]);

			// �������� - � ������������ ����������
			const int val = dsp_mag2y(mag, WFDY);
		#else
			// ��� ����������
			// �������� - � ������������ ����������
			const int val = dsp_mag2y(spavgarray [spavgrow ] [x], WFDY);
		#endif
			// ������������ �������
			int zv = (WFDY - 1) - val;
			int z;
			for (z = WFDY - 1; z >= zv; -- z)
				display_pixelbuffer_xor(wfmonoscr, WFDX, WFDY, x, z);	// xor �����
		}
	}
	else
	{
		memset(wfmonoscr, 0xFF, sizeof wfmonoscr);			// ��������� �������� �������� �����
	}
	display_setcolors(COLOR_WAERFALLBG, COLOR_WAERFALLFG);
	display_showbuffer(wfmonoscr, WFDX, WFDY, x0, y0);

#else /* */

	PACKEDCOLOR_T * const colorpip = getnextpip();
	// ������ �� ������� ��������, �� �������������� ����������� 
	// ���������� ����������� �� bitmap � ��������������
	if (hamradio_get_tx() == 0)
	{
		uint_fast16_t x;
		uint_fast16_t y;
		// ����������� �������
		for (x = 0; x < WFDX; ++ x)
		{
		#if EEAVERAGE
			// ����������
			FLOAT_t mag = 0;
			uint_fast8_t h;
			for (h = 0; h < AVGLEN; ++ h)
				mag += spavgarray [(spavgrow + h) % SPAVGSIZE] [x];

			// �������� - � ������������ ����������
			const int val = dsp_mag2y(mag / AVGLEN, WFDY);
		#elif EEMAXIMUM
			// ��������
			FLOAT_t mag = 0;
			uint_fast8_t h;
			for (h = 0; h < MAXHISTLEN; ++ h)
				mag = FMAXF(mag, spavgarray [(spavgrow + h) % SPAVGSIZE] [x]);

			// �������� - � ������������ ����������
			const int val = dsp_mag2y(mag, WFDY);
		#else
			// ��� ����������
			// �������� - � ������������ ����������
			const int val = dsp_mag2y(spavgarray [spavgrow ] [x], WFDY);
		#endif
			// ������������ �������
			int zv = (WFDY - 1) - val;
			int z;
			for (z = WFDY - 1; z >= zv; -- z)
				display_colorbuffer_set(colorpip, WFDX, WFDY, x, z, COLOR_WAERFALLFG);	// ����� �������
			// ������������ ���� ������
			for (; z >= 0; -- z)
				display_colorbuffer_set(colorpip, WFDX, WFDY, x, z, COLOR_WAERFALLBG);	// ����� ����
		}
		// ������ ����������� ������� ������
		// xor �����
		for (y = 0; y < WFDY; ++ y)
		{
			display_colorbuffer_xor(colorpip, WFDX, WFDY, WFDX / 2, y, COLOR_CENTERMAKER); 
		}
	}
	else
	{
		display_colorbuffer_fill(colorpip, WFDX, WFDY, COLOR_GRAY);
	}

#if LCDMODE_LTDC_PIP16
	display_colorbuffer_pip(colorpip, WFDX, WFDY);
#else /* LCDMODE_LTDC_PIP16 */
	display_colorbuffer_show(colorpip, WFDX, WFDY, GRID2X(x0), GRID2Y(y0));
#endif /* LCDMODE_LTDC_PIP16 */
#endif
}

// ��������� ����� ����������� ������ �� �������� (� ������ ������������� ����������� scroll �����������).
static void display_wfputrow(uint_fast16_t x, uint_fast16_t y, const uint8_t * p)
{
	enum { dx = WFDX, dy = 1 };
	static ALIGNX_BEGIN PACKEDCOLOR_T b [GXSIZE(dx, dy)] ALIGNX_END;
	uint_fast16_t xp; 
	for (xp = 0; xp < dx; ++ xp)
		display_colorbuffer_set(b, dx, dy, xp, 0, wfpalette [p [xp]]);

	// ������ ����������� ������� ������
	display_colorbuffer_xor(b, dx, dy, dx / 2, 0, COLOR_CENTERMAKER);

	display_colorbuffer_show(b, dx, dy, x, y);
}

// ����������� ��������
static void display2_waterfall(
	uint_fast8_t x0, 
	uint_fast8_t y0, 
	void * pv
	)
{
#if LCDMODE_S1D13781

	#if 1
		// ����� ������� ("�������")
		// �������� ����, ������������ ������ ������� ������
		uint_fast16_t x;
		display_scroll_down(GRID2X(x0), GRID2Y(y0), WFDX, WFDY, 1);
		while (display_getreadystate() == 0)
			;
		x = 0;
		display_wfputrow(GRID2X(x0) + x, GRID2Y(y0) + 0, & wfarray [wfrow] [0]);	// display_plot inside for one row
	#elif 0
		// ����� ������� ("������")
		// �������� �����, ������������ ������ ������ ������
		uint_fast16_t x;
		display_scroll_up(GRID2X(x0), GRID2Y(y0), WFDX, WFDY, 1);
		while (display_getreadystate() == 0)
			;
		x = 0;
		display_wfputrow(GRID2X(x0) + x, GRID2Y(y0) + WFDY - 1, & wfarray [wfrow] [0]);	// display_plot inside for one row
	#else
		// ����� ������� ("�������")
		// ������������ ���� �����
		while (display_getreadystate() == 0)
			;
		uint_fast16_t x, y;
		for (y = 0; y < WFDY; ++ y)
		{
			// ��������� ��������������� �������
			x = 0;
			display_wfputrow(GRID2X(x0) + x, GRID2Y(y0) + y, & wfarray [(wfrow + y) % WFDY] [0]);	// display_plot inside for one row
		}
	#endif

#elif LCDMODE_UC1608 || LCDMODE_UC1601
	// ����� ������� ("�������") �� ����������� ��������

	static ALIGNX_BEGIN GX_t wfmonoscr [MGSIZE(WFDX, WFDY)] ALIGNX_END;
	memset(wfmonoscr, 0xFF, sizeof wfmonoscr);			// ��������� �������� �������� �����

	if (hamradio_get_tx() == 0)
	{

		uint_fast16_t x, y;


		// ������������ ������
		// ������ ����������� ������� ������
		for (y = 0; y < WFDY; ++ y)
		{
			display_pixelbuffer(wfmonoscr, WFDX, WFDY, WFDX / 2, y);	// �������� �����
		}
		// ����� ������� ("�������")
		for (x = 0; x < WFDX; ++ x)
		{
			for (y = 0; y < WFDY; ++ y)
			{
				const uint_fast8_t v = wfarray [(wfrow + y) % WFDY] [x];
				if (v > wflfence)
					display_pixelbuffer(wfmonoscr, WFDX, WFDY, x, y);	// �������� �����
			}
		}
	}

	display_setcolors(COLOR_GRAY, COLOR_BLUE);
	display_showbuffer(wfmonoscr, WFDX, WFDY, x0, y0);

#else /* */
	// ����� ������� ("�������") �� ������� ��������

	PACKEDCOLOR_T * const colorpip = getnextpip();
	if (hamradio_get_tx() == 0)
	{

		uint_fast16_t x, y;


		// ������������ ������
		// ����� ������� ("�������")
		for (x = 0; x < WFDX; ++ x)
		{
			for (y = 0; y < WFDY; ++ y)
			{
				display_colorbuffer_set(colorpip, WFDX, WFDY, x, y, wfpalette [wfarray [(wfrow + y) % WFDY] [x]]);
			}
		}
		// ������ ����������� ������� ������
		for (y = 0; y < WFDY; ++ y)
		{
			display_colorbuffer_xor(colorpip, WFDX, WFDY, WFDX / 2, y, COLOR_RED);
		}
	}
	else
	{
		display_colorbuffer_fill(colorpip, WFDX, WFDY, COLOR_GRAY);
	}

#if LCDMODE_LTDC_PIP16
	display_colorbuffer_pip(colorpip, WFDX, WFDY);
#else /* LCDMODE_LTDC_PIP16 */
	display_colorbuffer_show(colorpip, WFDX, WFDY, GRID2X(x0), GRID2Y(y0));
#endif /* LCDMODE_LTDC_PIP16 */

#endif /* LCDMODE_S1D13781 */
}

#else /* WITHINTEGRATEDDSP && (WITHRTS96 || WITHRTS192) */

static void dsp_latchwaterfall(
	uint_fast8_t x0, 
	uint_fast8_t y0, 
	void * pv
	)
{
}

static void display2_spectrum(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
}

static void display2_waterfall(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
}

#endif /* WITHINTEGRATEDDSP && (WITHRTS96 || WITHRTS192) */

#define STMD 1

#if STMD

// ��������� state machine �����������
static uint_fast8_t reqs [REDRM_count];		// ������� �� �����������
static uint_fast8_t subsets [REDRM_count];	// �������� ������� �� ������ �����������.
static uint_fast8_t walkis [REDRM_count];	// ������ � ������ ���������� ����������� � ������ �������
static uint_fast8_t keyi;					// ������ �� �����������, ������������� ������.

#endif /* STMD */

static uint_fast8_t
getsubset(
	uint_fast8_t menuset,	/* ������ ������ ����������� (0..DISPLC_MODCOUNT - 1) */
	uint_fast8_t extra		/* ��������� � ������ ����������� �������� */
	)
{
	return extra ? REDRSUBSET_MENU : REDRSUBSET(menuset);
}

// ���������� ��������� ���� ��������� �� ���.
static void 
display_walktrough(
	uint_fast8_t key,
	uint_fast8_t subset,
	void * pv
	)
{
	enum { WALKCOUNT = sizeof dzones / sizeof dzones [0] };
	uint_fast8_t i;

	for (i = 0; i < WALKCOUNT; ++ i)
	{
		const FLASHMEM struct dzone * const p = & dzones [i];

		if ((p->key != key) || (p->subset & subset) == 0)
			continue;
		(* p->redraw)(p->x, p->y, pv);
	}
}


// ����� �� ���������� ��������� ���� ��������� ����� state machine.
static void 
display_walktroughsteps(
	uint_fast8_t key,
	uint_fast8_t subset
	)
{
#if STMD
	reqs [key] = 1;
	subsets [key] = subset;
	walkis [key] = 0;
#else /* STMD */

	display_walktrough(key, subset, NULL);

#endif /* STMD */
}

// Interface functions
// ���������� ����� state machine ����������� �������
void display2_bgprocess(void)
{
#if STMD
	enum { WALKCOUNT = sizeof dzones / sizeof dzones [0] };
	const uint_fast8_t keyi0 = keyi;

	for (;;)
	{
		if (reqs [keyi] != 0)
			break;
		keyi = (keyi == (REDRM_count - 1)) ? 0 : (keyi + 1);
		if (keyi == keyi0)
			return;			// �� ����� �� ������ �������
	}

	//return;
	for (; walkis [keyi] < WALKCOUNT; ++ walkis [keyi])
	{
		const FLASHMEM struct dzone * const p = & dzones [walkis [keyi]];

		if ((p->key != keyi) || (p->subset & subsets [keyi]) == 0)
			continue;
		(* p->redraw)(p->x, p->y, NULL);
		walkis [keyi] += 1;
		break;
	}
	if (walkis [keyi] >= WALKCOUNT)
	{
		reqs [keyi] = 0;	// ����� ������ �� ����������� ������� ���� ���������
		keyi = (keyi == (REDRM_count - 1)) ? 0 : (keyi + 1);
	}
#endif /* STMD */
}

// Interface functions
// ����� state machine ����������� ������� � �������� �������
void display2_bgreset(void)
{
	uint_fast8_t i;

	// �������� �������.
	display_clear();	

#if STMD
	// ����� state machine ����������� �������
	for (i = 0; i < REDRM_count; ++ i)
	{
		reqs [i] = 0;
		//walkis [keyi] = 0;
	}
	keyi = 0;
#endif /* STMD */

#if WITHINTEGRATEDDSP && (WITHRTS96 || WITHRTS192) && ! LCDMODE_HD44780
	// ������������� ������� ��������
	wfpalette_initialize();
#endif /* WITHINTEGRATEDDSP && (WITHRTS96 || WITHRTS192) */
}

// Interface functions
void display_mode_subset(
	uint_fast8_t menuset	/* ������ ������ ����������� (0..DISPLC_MODCOUNT - 1) */
	)
{
	display_walktroughsteps(REDRM_MODE, getsubset(menuset, 0));
}

void display_barmeters_subset(
	uint_fast8_t menuset,	/* ������ ������ ����������� (0..3) */
	uint_fast8_t extra		/* ��������� � ������ ����������� �������� */
	)
{
	display_walktroughsteps(REDRM_BARS, getsubset(menuset, extra));
}

void display_dispfreq_ab(
	uint_fast8_t menuset	/* ������ ������ ����������� (0..DISPLC_MODCOUNT - 1) */
	)
{
	display_walktroughsteps(REDRM_FREQ, getsubset(menuset, 0));
	display_walktroughsteps(REDRM_FREQB, getsubset(menuset, 0));
}

void display_dispfreq_a2(
	uint_fast32_t freq,
	uint_fast8_t blinkpos,		// ������� (������� 10) �������������� �������
	uint_fast8_t blinkstate,	// � ����� �������������� ������� ������������ ������������� (0 - ������)
	uint_fast8_t menuset	/* ������ ������ ����������� (0..DISPLC_MODCOUNT - 1) */
	)
{
#if WITHDIRECTFREQENER
	struct editfreq ef;

	ef.freq = freq;
	ef.blinkpos = blinkpos;
	ef.blinkstate = blinkstate;

	display_walktrough(REDRM_FREQ,  getsubset(menuset, 0), & ef);
#else	/* WITHDIRECTFREQENER */
	display_walktroughsteps(REDRM_FREQ,  getsubset(menuset, 0));
#endif /* WITHDIRECTFREQENER */
}

void display_volts(
	uint_fast8_t menuset,	/* ������ ������ ����������� (0..DISPLC_MODCOUNT - 1) */
	uint_fast8_t extra		/* ��������� � ������ ����������� �������� */
	)
{
	display_walktroughsteps(REDRM_VOLT, getsubset(menuset, extra));
}

// ����������� �������� ��������� ��� ������
void display_menuitemlabel(
	void * pv
	)
{
	display_walktrough(REDRM_FREQ, REDRSUBSET_MENU, NULL);
	display_walktrough(REDRM_FREQB, REDRSUBSET_MENU, NULL);
	display_walktrough(REDRM_MODE, REDRSUBSET_MENU, NULL);
	display_walktrough(REDRM_MLBL, REDRSUBSET_MENU, pv);
	display_walktrough(REDRM_MVAL, REDRSUBSET_MENU, pv);
}

// ����������� �������� ���������
void display_menuitemvalue(
	void * pv
	)
{
	display_walktrough(REDRM_MVAL, REDRSUBSET_MENU, pv);
}

// ��������� ����� �������� ����������� (menuset)
uint_fast8_t display_getpagesmax(void)
{
	return DISPLC_MODCOUNT - 1;
}

// �������� ��������� ����������� ������� (��� ������� ������� �����)
uint_fast8_t display_getfreqformat(
	uint_fast8_t * prjv
	)
{
	* prjv = DISPLC_RJ;
	return DISPLC_WIDTH;
}
