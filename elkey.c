/* $Id$ */
//
// ���������� ������������ ������������ �����
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"
#include "board.h"
#include "spifuncs.h"
#include "formats.h"

#if WITHELKEY
// 
// ���������� ������������ ������������ �����
//

enum {
	//
	ELKEY_STATE_INITIALIZE,	// ������ �� ��������� - ��������� ��������� ����������

	ELKEY_STATE_ACTIVE_DIT,	// ������ ��������� ������� �����
	ELKEY_STATE_ACTIVE_DASH,	// ������ ��������� ������� �����
	ELKEY_STATE_ACTIVE_WITH_PENDING_DIT,	// ��������� ������� ����� � �������� �������� ��������
	ELKEY_STATE_ACTIVE_WITH_PENDING_DASH,	// ��������� ������� ���� � �������� �������� ��������

	ELKEY_STATE_SPACE_WITH_PENDING_DIT,	// ��������� ������� ����� � �������� �������� �� ������ (��� ���� �����).
	ELKEY_STATE_SPACE_WITH_PENDING_DASH,	// ��������� ������� ���� � �������� �������� �� ������ (��� ���� �����).

	ELKEY_STATE_SPACE,	// ������ ������������� ����� ����� �������� �������� �����
	ELKEY_STATE_SPACE2,		// ������ ������������� ����� ����� ��������� �����

#if WITHCAT && WITHCATEXT
	//
	ELKEY_STATE_AUTO_INITIALIZE,	// ������ �� ��������� - ��������� ��������� ����������
	ELKEY_STATE_AUTO_ELEMENT_DIT,
	ELKEY_STATE_AUTO_ELEMENT_DASH,
	ELKEY_STATE_AUTO_SPACE,
	ELKEY_STATE_AUTO_SPACE2,
	ELKEY_STATE_AUTO_NEXT,
#endif /* WITHCAT && WITHCATEXT */

	//
	ELKEY_STATE_MAX		// �������������� �����
};

static const FLASHMEM uint_fast8_t elkeyout [ELKEY_STATE_MAX] =
{
	0, // ELKEY_STATE_INITIALIZE,	// ������ �� ��������� - ��������� ��������� ����������
	1, // ELKEY_STATE_ACTIVE_DIT,	// ������ ��������� ������� �����
	1, // ELKEY_STATE_ACTIVE_DASH,	// ������ ��������� ������� �����
	1, // ELKEY_STATE_ACTIVE_WITH_PENDING_DIT,	// ��������� ������� ����� � �������� �������� ��������
	1, // ELKEY_STATE_ACTIVE_WITH_PENDING_DASH,	// ��������� ������� ���� � �������� �������� ��������
	0, // ELKEY_STATE_SPACE_WITH_PENDING_DIT,	// ��������� ������� ����� � �������� �������� �� ������ (��� ���� �����).
	0, // ELKEY_STATE_SPACE_WITH_PENDING_DASH,	// ��������� ������� ���� � �������� �������� �� ������ (��� ���� �����).
	0, // ELKEY_STATE_SPACE,	// ������ ������������� ����� ����� �������� �������� �����
	0, // ELKEY_STATE_SPACE2,		// ������ ������������� ����� ����� ��������� �����
#if WITHCAT && WITHCATEXT
	//
	0, // ELKEY_STATE_AUTO_INITIALIZE,	// ������ �� ��������� - ��������� ��������� ����������
	1, // ELKEY_STATE_AUTO_ELEMENT_DIT,
	1, // ELKEY_STATE_AUTO_ELEMENT_DASH,
	0, // ELKEY_STATE_AUTO_SPACE,
	0, // ELKEY_STATE_AUTO_SPACE2,
	0, // ELKEY_STATE_AUTO_NEXT,
#endif /* WITHCAT && WITHCATEXT */
};


typedef struct elkey_tag
{
	uint_fast8_t state;
	uint_fast16_t morse;	// ������� ������������ ����
	uint_fast8_t ticks;
	uint_fast8_t maxticks;
	uint_fast8_t ignore_dit;	/* �������� �������������� ���������������� � �������� ������������ ����� ��������� ������������ �������� �����. */
	uint_fast8_t ignore_dash;	/* �������� �������������� ���������������� � �������� ������������ ����� ��������� ������������ �������� �����. */

#if WITHVIBROPLEX
	uint_fast8_t vibroplex_slope /* = 0 */;		// �������� ���������� ������������ ����� � �����
	uint_fast8_t vibroplex_grade;		// ���������� (����� � ������� �����) "�����������"
#endif /* WITHVIBROPLEX */
	uint_fast8_t vibroplex_derate;		// ������������� ����� "�����������" ������������
} elkey_t;


static uint_fast8_t elkey_reverse;
static uint_fast8_t elkey_straight_flags;

static elkey_t elkey0;	// ������ ����
#if WITHCAT && WITHCATEXT
	static elkey_t elkey1;	// CAT (������� KY, KS, ...) ����
#endif /* WITHCAT && WITHCATEXT */

#define DASHFLAG 	0x01
#define DITSFLAG 	0x02
#define ACSFLAG 	0x04

#define elkey_straight_dash() ((elkey_straight_flags & DASHFLAG) != 0)	/* ���� ����������� ������� */
#define elkey_straight_dits() ((elkey_straight_flags & DITSFLAG) != 0)	/* ����� ����������� ������� */
#define elkey_acs_mode() ((elkey_straight_flags & ACSFLAG) != 0)			/* �������������� �������� ����� �� ��������� ����� */


// � ������ ���������� ����������� � ���� � ���� ����� ��� �������� ����� �������.
// ���� ������� �������� �������. ���� - ��������� (��� ������ ������ ��� ���������).
enum { delay_dit = ELKEY_DISCRETE * 1, delay_words = ELKEY_DISCRETE * 6 };	// ����� ���������� ���� - ���� ����������
static uint_fast8_t delay_space;	// 10
static uint_fast8_t delay_dash;	// 30


	
#if WITHVIBROPLEX
	enum { ELKEY_DERATE_QUOTANT = 2 };

	// �������� ���������� ������������ ����� � ����� - �������� �����������
	static void elkeyx_set_slope(elkey_t * const elkey, uint_fast8_t slope)
	{
		disableIRQ();
		if (elkey->vibroplex_slope > slope)
		{
			elkey->vibroplex_grade = 0;
			elkey->vibroplex_derate = 0;
		}
		elkey->vibroplex_slope = slope;
		enableIRQ();
	}

	static void elkey_vibroplex_next(elkey_t * const elkey)
	{
		if (elkey->vibroplex_grade < (ELKEY_DERATE_QUOTANT * 3 * ELKEY_DISCRETE / 10))	// ��������� �� ����� ��� �� 0.3 �� ��������� ������������ �����
		{
			elkey->vibroplex_grade += elkey->vibroplex_slope;
			elkey->vibroplex_derate = elkey->vibroplex_grade / ELKEY_DERATE_QUOTANT;
		}
	}

#else /* WITHVIBROPLEX */

	static void elkey_vibroplex_next(elkey_t * const elkey)
	{
	}

#endif /* WITHVIBROPLEX */

static void elkey_vibroplex_reset(elkey_t * const elkey)
{
#if WITHVIBROPLEX
	elkey->vibroplex_grade = 0;
#endif /* WITHVIBROPLEX */
	elkey->vibroplex_derate = 0;
}


void elkey_set_slope(uint_fast8_t slope)
{
#if WITHVIBROPLEX
	elkeyx_set_slope(& elkey0, slope);
#if WITHCAT && WITHCATEXT
	elkeyx_set_slope(& elkey1, slope);
#endif /* WITHCAT && WITHCATEXT */
#endif /* WITHVIBROPLEX */
}

/* � ���������� ������ �������� ������ ����� ����� CAT */
enum
{
	ELSIZE = 2,
	ELMASK = 0x03,

	E0 = 0 * ELSIZE,
	E1 = 1 * ELSIZE,
	E2 = 2 * ELSIZE,
	E3 = 3 * ELSIZE,
	E4 = 4 * ELSIZE,
	E5 = 5 * ELSIZE,
	E6 = 6 * ELSIZE,
	E7 = 7 * ELSIZE,	// �� 8 ��������� � ����� - 16 ���

	MDIT = 1,
	MDASH = 2,
	MSPACE = 3,
	MEND = 0
};

/*
 ��� ���������� ������� �������������� ������ http://en.wikipedia.org/wiki/Morse_code
 */
static uint_fast16_t
//NOINLINEAT
get_morse(
	uint_fast8_t c		// ������ ��� �������� (������ ������� �������).
	)
{
#if 0
	// use table
	static const FLASHMEM uint16_t codes [] =
	{
	/* 0x20 */
	// Code ' ':
		(MSPACE << E0),
	// Code '!':	/* Exclamation mark */
		(MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDIT << E3) | (MDASH << E4) | (MDASH << E5),
	// Code '"':	/* quotatiom mark */
		(MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4) | (MDIT << E5),
	// Code '#':	/* ts-590s prochars substitutes */* HH prochar */
		(MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4) | (MDIT << E5) | (MDIT << E6) | (MDIT << E7),
	// Code '$':	/* dollar sign */
		(MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4) | (MDIT << E5) | (MDASH << E6),
	// Code '%':	/* ts-590s prochars substitutes */* SN prochar */
		(MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4),
	// Code '&':	/* 'Ampersand, wait */
		(MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4),
	// Code '\'':	/* Apostrophe */
		(MDIT << E0) | (MDASH << E1) | (MDASH << E2) | (MDASH << E3) | (MDASH << E4) | (MDIT << E5),
	// Code '(':	/*  */
		(MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4),
	// Code ')':	/*  */
		(MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4) | (MDASH << E5),
	// Code '*':
		(MSPACE << E0),
	// Code '+':	/* plus */
		(MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4),
	// Code ',':	/* comma */
		(MDASH << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4) | (MDASH << E5),
	// Code '-':	/* haypen, minus */
		(MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4) | (MDASH << E5),
	// Code '.':	/* period */
		(MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4) | (MDASH << E5),
	// Code '/':	/* slash */
		(MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4),

	/* 0x30 */
	// Code '0':/* digits */
		(MDASH << E0) | (MDASH << E1) | (MDASH << E2) | (MDASH << E3) | (MDASH << E4),
	// Code '1':/* digits */
		(MDIT << E0) | (MDASH << E1) | (MDASH << E2) | (MDASH << E3) | (MDASH << E4),
	// Code '2':/* digits */
		(MDIT << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDASH << E4),
	// Code '3':/* digits */
		(MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3) | (MDASH << E4),
	// Code '4':/* digits */
		(MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4),
	// Code '5':/* digits */
		(MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4),
	// Code '6':/* digits */
		(MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4),
	// Code '7':/* digits */
		(MDASH << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4),
	// Code '8':/* digits */
		(MDASH << E0) | (MDASH << E1) | (MDASH << E2) | (MDIT << E3) | (MDIT << E4),
	// Code '9':/* digits */
		(MDASH << E0) | (MDASH << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4),
	// Code ':':	/* colon */
		(MDASH << E0) | (MDASH << E1) | (MDASH << E2) | (MDIT << E3) | (MDIT << E4) | (MDIT << E5),
	// Code ';':	/* semicolon */
		(MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDIT << E3) | (MDASH << E4) | (MDIT << E5),
	// Code '<':	/* ts-590s prochars substitutes */* AS prochar */
		(MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4),
	// Code '=':	/* double dash */
		(MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4),
	// Code '>':	/* ts-590s prochars substitutes */* SK prochar */
		(MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4) | (MDASH << E5),
	// Code '?':	/* question mark */
		(MDIT << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4) | (MDIT << E5),

	/* International Letters */
	// Code '@':	/* 'at' sign */
		(MDIT << E0) | (MDASH << E1) | (MDASH << E2) | (MDIT << E3) | (MDASH << E4) | (MDIT << E5),
	// Code 'A':
		(MDIT << E0) | (MDASH << E1),
	// Code 'B':
		(MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3),
	// Code 'C':
		(MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDIT << E3),
	// Code 'D':
		(MDASH << E0) | (MDIT << E1) | (MDIT << E2),
	// Code 'E':
		(MDIT << E0),
	// Code 'F':
		(MDIT << E0) | (MDIT << E1) | (MDASH << E2) | (MDIT << E3),
	// Code 'G':
		(MDASH << E0) | (MDASH << E1) | (MDIT << E2),
	// Code 'H':
		(MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3),
	// Code 'I':
		(MDIT << E0) | (MDIT << E1),
	// Code 'J':
		(MDIT << E0) | (MDASH << E1) | (MDASH << E2) | (MDASH << E3),
	// Code 'K':
		(MDASH << E0) | (MDIT << E1) | (MDASH << E2),
	// Code 'L':
		(MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3),
	// Code 'M':
		(MDASH << E0) | (MDASH << E1),
	// Code 'N':
		(MDASH << E0) | (MDIT << E1),
	// Code 'O':
		(MDASH << E0) | (MDASH << E1) | (MDASH << E2),
	// Code 'P':
		(MDIT << E0) | (MDASH << E1) | (MDASH << E2) | (MDIT << E3),
	// Code 'Q':
		(MDASH << E0) | (MDASH << E1) | (MDIT << E2) | (MDASH << E3),
	// Code 'R':
		(MDIT << E0) | (MDASH << E1) | (MDIT << E2),
	// Code 'S':
		(MDIT << E0) | (MDIT << E1) | (MDIT << E2),
	// Code 'T':
		(MDASH << E0),
	// Code 'U':
		(MDIT << E0) | (MDIT << E1) | (MDASH << E2),
	// Code 'V':
		(MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3),
	// Code 'W':
		(MDIT << E0) | (MDASH << E1) | (MDASH << E2),
	// Code 'X':
		(MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3),
	// Code 'Y':
		(MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3),
	// Code 'Z':
		(MDASH << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3),
	// Code '[':	/* ts-590s prochars substitutes */* BT prochar */
		(MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4),
	// Code '\\':	/* ts-590s prochars substitutes */* BK prochar */
		(MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4) | (MDIT << E5) | (MDASH << E6),
	// Code ']':	/* ts-590s prochars substitutes */* KN prochar */
		(MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4),
	// Code 0x4E: '_':	/* ts-590s prochars substitutes */* AR prochar */
		//(MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4),
	//// Code 0x4F: '_':	/* underscore */ // conflict with Kenwood AR prochar
		//(MDIT << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4) | (MDASH << E5),

	// 0x60
	};

	if (c == '_')
		/* ts-590s prochars substitutes AR prochar */
		return (MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4);
	if (c < ' ' || (c - ' ') > (sizeof codes / sizeof codes [0]))
		return (MSPACE << E0);

	return codes [(c - ' ')];

#else
	/*
	 ��� ���������� ������� �������������� ������ http://en.wikipedia.org/wiki/Morse_code
	 */
	switch (c)
	{
	/* International Letters */
	case 'A':
		return (MDIT << E0) | (MDASH << E1);
	case 'B':
		return (MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3);
	case 'C':
		return (MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDIT << E3);
	case 'D':
		return (MDASH << E0) | (MDIT << E1) | (MDIT << E2);
	case 'E':
		return (MDIT << E0);
	case 'F':
		return (MDIT << E0) | (MDIT << E1) | (MDASH << E2) | (MDIT << E3);
	case 'G':
		return (MDASH << E0) | (MDASH << E1) | (MDIT << E2);
	case 'H':
		return (MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3);
	case 'I':
		return (MDIT << E0) | (MDIT << E1);
	case 'J':
		return (MDIT << E0) | (MDASH << E1) | (MDASH << E2) | (MDASH << E3);
	case 'K':
		return (MDASH << E0) | (MDIT << E1) | (MDASH << E2);
	case 'L':
		return (MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3);
	case 'M':
		return (MDASH << E0) | (MDASH << E1);
	case 'N':
		return (MDASH << E0) | (MDIT << E1);
	case 'O':
		return (MDASH << E0) | (MDASH << E1) | (MDASH << E2);
	case 'P':
		return (MDIT << E0) | (MDASH << E1) | (MDASH << E2) | (MDIT << E3);
	case 'Q':
		return (MDASH << E0) | (MDASH << E1) | (MDIT << E2) | (MDASH << E3);
	case 'R':
		return (MDIT << E0) | (MDASH << E1) | (MDIT << E2);
	case 'S':
		return (MDIT << E0) | (MDIT << E1) | (MDIT << E2);
	case 'T':
		return (MDASH << E0);
	case 'U':
		return (MDIT << E0) | (MDIT << E1) | (MDASH << E2);
	case 'V':
		return (MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3);
	case 'W':
		return (MDIT << E0) | (MDASH << E1) | (MDASH << E2);
	case 'X':
		return (MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3);
	case 'Y':
		return (MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3);
	case 'Z':
		return (MDASH << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3);


	/* punctuation elements */
	case '.':	/* period */
		return (MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4) | (MDASH << E5);
	case ',':	/* comma */
		return (MDASH << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4) | (MDASH << E5);
	case '?':	/* question mark */
		return (MDIT << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4) | (MDIT << E5);
	case '\'':	/* Apostrophe */
		return (MDIT << E0) | (MDASH << E1) | (MDASH << E2) | (MDASH << E3) | (MDASH << E4) | (MDIT << E5);
	case '!':	/* Exclamation mark */
		return (MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDIT << E3) | (MDASH << E4) | (MDASH << E5);
	case '/':	/* slash */
		return (MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4);

	case '(':	/*  */
		return (MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4);
	case ')':	/*  */
		return (MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4) | (MDASH << E5);
	case '&':	/* 'Ampersand, wait */
		return (MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4);

	case ':':	/* colon */
		return (MDASH << E0) | (MDASH << E1) | (MDASH << E2) | (MDIT << E3) | (MDIT << E4) | (MDIT << E5);
	case ';':	/* semicolon */
		return (MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDIT << E3) | (MDASH << E4) | (MDIT << E5);
	case '=':	/* double dash */
		return (MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4);
	case '+':	/* plus */
		return (MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4);
	case '-':	/* haypen, minus */
		return (MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4) | (MDASH << E5);
	//case '_':	/* underscore */ // conflict with Kenwood AR prochar
	//	return (MDIT << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4) | (MDASH << E5);
	case '"':	/* quotatiom mark */
		return (MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4) | (MDIT << E5);
	case '$':	/* dollar sign */
		return (MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4) | (MDIT << E5) | (MDASH << E6);
	case '@':	/* 'at' sign */
		return (MDIT << E0) | (MDASH << E1) | (MDASH << E2) | (MDIT << E3) | (MDASH << E4) | (MDIT << E5);

	/* digits */
	case '0':
		return (MDASH << E0) | (MDASH << E1) | (MDASH << E2) | (MDASH << E3) | (MDASH << E4);
	case '1':
		return (MDIT << E0) | (MDASH << E1) | (MDASH << E2) | (MDASH << E3) | (MDASH << E4);
	case '2':
		return (MDIT << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDASH << E4);
	case '3':
		return (MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3) | (MDASH << E4);
	case '4':
		return (MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4);
	case '5':
		return (MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4);
	case '6':
		return (MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4);
	case '7':
		return (MDASH << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4);
	case '8':
		return (MDASH << E0) | (MDASH << E1) | (MDASH << E2) | (MDIT << E3) | (MDIT << E4);
	case '9':
		return (MDASH << E0) | (MDASH << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4);
		
	/* ts-590s prochars substitutes */
	case '[':	/* BT prochar */
		return (MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4);
	case '_':	/* AR prochar */
		return (MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4);
	case '<':	/* AS prochar */
		return (MDIT << E0) | (MDASH << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4);
	case '#':	/* HH prochar */
		return (MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDIT << E4) | (MDIT << E5) | (MDIT << E6) | (MDIT << E7);
	case '>':	/* SK prochar */
		return (MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4) | (MDASH << E5);
	case ']':	/* KN prochar */
		return (MDASH << E0) | (MDIT << E1) | (MDASH << E2) | (MDASH << E3) | (MDIT << E4);
	case '\\':	/* BK prochar */
		return (MDASH << E0) | (MDIT << E1) | (MDIT << E2) | (MDIT << E3) | (MDASH << E4) | (MDIT << E5) | (MDASH << E6);
	case '%':	/* SN prochar */
		return (MDIT << E0) | (MDIT << E1) | (MDIT << E2) | (MDASH << E3) | (MDIT << E4);

	default:
	case ' ':
		return (MSPACE << E0);
	}

#endif

}

enum { DOTS20IGNORE = 4 * ELKEY_DISCRETE / 10 };	// ����� ������������� 0.4 �� ������������ �����

// �������� ��������� ������������
// �� ��������� �����
// � �������������� ������
static uint_fast8_t 
//NOINLINEAT
dit_auto(
	elkey_t * const elkey,
	uint_fast8_t pl /* hardware paddle value */
	)
{
	if (elkey->ignore_dit != 0)
		return 0;
	if ((pl & ELKEY_PADDLE_DIT) == 0)
		return 0;
	return (elkey_straight_dits() == 0);
}

// �������� ��������� ������������
// �� ��������� ����
// � �������������� ������
static uint_fast8_t 
//NOINLINEAT
dash_auto(
	elkey_t * const elkey,
	uint_fast8_t pl /* hardware paddle value */
	)
{
	if (elkey->ignore_dash != 0)
		return 0;
	if ((pl & ELKEY_PADDLE_DASH) == 0)
		return 0;
	return (elkey_straight_dash() == 0);
}



// �������� ��������� ������������
// �� ��������� �����
// � ������ ������
static uint_fast8_t 
//NOINLINEAT
dit_manual(
	uint_fast8_t pl /* hardware paddle value */
	)
{
	if ((pl & ELKEY_PADDLE_DIT) == 0)
		return 0;
	return elkey_straight_dits();
}

// �������� ��������� ������������
// �� ��������� ����
// � ������ ������
static uint_fast8_t 
//NOINLINEAT
dash_manual(
	uint_fast8_t pl /* hardware paddle value */
	)
{
	if ((pl & ELKEY_PADDLE_DASH) == 0)
		return 0;
	return elkey_straight_dash();
}


static uint_fast8_t
elkey_manual(void)
{
	const uint_fast8_t pv = hardware_elkey_getpaddle(elkey_reverse);

	return
		dash_manual(pv) || /* ����������� ������ ����������� �������� �������. */
		dit_manual(pv);	/* ����������� ������ ����������� �������� �������. */
}

// ������� � ��������� � ���������� ������. �������� ������� ������.
static void 
NOINLINEAT
setnextstate(
	elkey_t * const elkey,
	uint_fast8_t state,
	uint_fast8_t ticks)
{
	elkey->state = state;
	elkey->maxticks = ticks;
	elkey->ticks = 0;
}

// ������ ������� ����������� ������������ ������������ �����
// ���������� � �������� 1/ELKEY_DISCRETE �� ������������ �����.
static uint_fast8_t 
//NOINLINEAT
getovf(elkey_t * const elkey)
{
	return (elkey->ticks >= elkey->maxticks) || (++ elkey->ticks >= elkey->maxticks);
}

// ���������� �� ����������� ����������. 
// ���������� � �������� 1/ELKEY_DISCRETE �� ������������ �����.
// 

void elkeyx_spool_dots(elkey_t * const elkey, uint_fast8_t paddle)
{
	/* ����� ������������ ����� */
	// �������� ��������, � ������� �������� ������������ ������� ����� ����� ������ �����
	if (elkey->ignore_dit != 0)
		-- elkey->ignore_dit;
	if (elkey->ignore_dash != 0)
		-- elkey->ignore_dash;


	// ��������� ��������� ������������ �����
	/* 
		��������� �������� ����� ��������� ���������� ������� ����������� �����.
		��� ����, ����� �� "���������" ������ ������ ������� ����� ���������������
		������������ �� ��������, ������� ������������ ���������� ���� ����������
		�� ��� ���, ���� ��������� �� ������������ � ����� ��������.
	*/
	const uint_fast8_t ovf = getovf(elkey); // (elkey->ticks >= elkey->maxticks) || (++ elkey->ticks >= elkey->maxticks);

	//dbg_putchar('a' + elkey->state);
	// CW �� ����������� �����
	switch (elkey->state)
	{
	case ELKEY_STATE_INITIALIZE:	// ������ �� ����������
		elkey_vibroplex_reset(elkey);	// ����������� ��������� ���������� ������������ ���������. ��� vibroplex ���������.
		/* �������� ������� ��� �������� */
		if (dash_auto(elkey, paddle))
		{
			setnextstate(elkey, ELKEY_STATE_ACTIVE_DASH, delay_dash);
		}
		else if (dit_auto(elkey, paddle))
		{
			setnextstate(elkey, ELKEY_STATE_ACTIVE_DIT, delay_dit);
			elkey_vibroplex_next(elkey);
		}
		break;	/* end of case ELKEY_STATE_INITIALIZE */

	case ELKEY_STATE_ACTIVE_DIT:	// ������ ���������� ������� �����
		if (ovf)
		{
			// ��������� ����������� - ����� ���������
			setnextstate(elkey, ELKEY_STATE_SPACE, delay_space - elkey->vibroplex_derate);
			elkey->ignore_dit = DOTS20IGNORE;
			elkey_vibroplex_next(elkey);
		}
		else if (dash_auto(elkey, paddle))
		{
			//delay_pending = delay_dash;
			elkey->state = ELKEY_STATE_ACTIVE_WITH_PENDING_DASH;
			elkey_vibroplex_reset(elkey);	// ����������� ��������� ���������� ������������ ���������. ��� vibroplex ���������.
		}
		break;

	case ELKEY_STATE_ACTIVE_DASH:	// ������ ���������� ������� �����
		if (ovf)
		{
			// ��������� ����������� - ����� ���������
			setnextstate(elkey, ELKEY_STATE_SPACE, delay_space);
			elkey->ignore_dash = DOTS20IGNORE;
		}
		else if (dit_auto(elkey, paddle))
		{
			//delay_pending = delay_dit;
			elkey->state = ELKEY_STATE_ACTIVE_WITH_PENDING_DIT;
		}
		break;

	case ELKEY_STATE_ACTIVE_WITH_PENDING_DASH:	// ������ ���������� ������� �����
		if (ovf)
		{
			// ��������� ����������� - ����� ���������
			setnextstate(elkey, ELKEY_STATE_SPACE_WITH_PENDING_DASH, delay_space);
		}
		break;

	case ELKEY_STATE_ACTIVE_WITH_PENDING_DIT:	// ������ ���������� ������� �����
		if (ovf)
		{
			// ��������� ����������� - ����� ���������
			setnextstate(elkey, ELKEY_STATE_SPACE_WITH_PENDING_DIT, delay_space - elkey->vibroplex_derate);
			elkey_vibroplex_next(elkey);
		}
		break;

	case ELKEY_STATE_SPACE:	// ������ ������������� ����� ����� �������� �������
		if (ovf)
		{
			// ��������� ��������� �������� ����� �������� �����.
			// �� ��� ����� ������ �� ������ ������.
			if (elkey_acs_mode())
			{
				// � ������ ASF ������������� ����������� ��� ��� ������������ �����.
				setnextstate(elkey, ELKEY_STATE_SPACE2, delay_dash - delay_space);
				elkey_vibroplex_reset(elkey);	// ����������� ��������� ���������� ������������ ���������. ��� vibroplex ���������.
			}
			else
			{
				// � ������ ������ ������� � ���������� ���������
				setnextstate(elkey, ELKEY_STATE_INITIALIZE, 0);
			}
		}
		else if (dit_auto(elkey, paddle))
		{
			// ��� �� ���������� ��������� �������� ����� ����������
			// ����������� �������� �����, � ��� ����� ������ �� �����������
			//delay_pending = delay_dit;
			elkey->state = ELKEY_STATE_SPACE_WITH_PENDING_DIT;
		}
		else if (dash_auto(elkey, paddle))
		{
			// ��� �� ���������� ��������� �������� ����� ����������
			// ����������� �������� �����, � ��� ����� ������ �� �����������
			//delay_pending = delay_dash;
			elkey->state = ELKEY_STATE_SPACE_WITH_PENDING_DASH;
		}
		break;

	case ELKEY_STATE_SPACE2:	// ������ ������������� ����� ����� �������� ������� ����� � ������ ASF
		if (ovf)
		{
			setnextstate(elkey, ELKEY_STATE_INITIALIZE, 0);
		}
		else if (dash_auto(elkey, paddle))
		{
			elkey->state = ELKEY_STATE_SPACE_WITH_PENDING_DASH;
		}
		else if (dit_auto(elkey, paddle))
		{
			elkey->state = ELKEY_STATE_SPACE_WITH_PENDING_DIT;
		}
		break;

	case ELKEY_STATE_SPACE_WITH_PENDING_DIT:	// ������������ ����� - ���� ������� �� ��������������� �����
		if (ovf)
		{
			// ����� �����������
			setnextstate(elkey, ELKEY_STATE_ACTIVE_DIT, delay_dit - elkey->vibroplex_derate);
			elkey_vibroplex_next(elkey);
		}
		break;

	case ELKEY_STATE_SPACE_WITH_PENDING_DASH:	// ������������ ����� - ���� ������� �� ��������������� �����
		if (ovf)
		{
			// ����� �����������
			setnextstate(elkey, ELKEY_STATE_ACTIVE_DASH, delay_dash);
		}
		break;

#if WITHCAT && WITHCATEXT

	case ELKEY_STATE_AUTO_INITIALIZE:	// ������ �� ����������
		elkey_vibroplex_reset(elkey);	// ����������� ��������� ���������� ������������ ���������. ��� vibroplex ���������.
		{
			const uint_fast8_t ch = elkey_getnextcw();	// ��������� ������� ��� �������� (������ ������� �������)

			if (ch != '\0')
			{
				elkey->morse = get_morse(ch);		// ��������� ������� ����� ������������������ ��������� ������� �����.

				switch (elkey->morse & ELMASK)	// ��������� ������� �������� �����
				{
				case MDASH:
					setnextstate(elkey, ELKEY_STATE_AUTO_ELEMENT_DASH, delay_dash);
					break;
				case MDIT:
					setnextstate(elkey, ELKEY_STATE_AUTO_ELEMENT_DIT, delay_dit - elkey->vibroplex_derate);
					elkey_vibroplex_next(elkey);
					break;
				case MSPACE:	// ����� ������� ���� ����������
					setnextstate(elkey, ELKEY_STATE_AUTO_SPACE2, delay_words - delay_space);	/* �������� delay_space ��� ���� ����� ��������� ���������� ����� */
					break;
				}
			}
		}
		break;


	case ELKEY_STATE_AUTO_ELEMENT_DIT:	// ������ ���������� ������� �����
		if (ovf)
		{
			// ��������� ����������� - ����� ���������
			setnextstate(elkey, ELKEY_STATE_AUTO_SPACE, delay_space - elkey->vibroplex_derate);	/* �������� �� �������� ����� */
			elkey_vibroplex_next(elkey);
		}
		break;

	case ELKEY_STATE_AUTO_ELEMENT_DASH:	// ������ ���������� ������� �����
		if (ovf)
		{
			// ��������� ����������� - ����� ���������
			setnextstate(elkey, ELKEY_STATE_AUTO_SPACE, delay_space);	/* �������� �� �������� ����� */
			elkey_vibroplex_reset(elkey);	// ����������� ��������� ���������� ������������ ���������. ��� vibroplex ���������.
		}
		break;

	case ELKEY_STATE_AUTO_SPACE:	// ������ ������������� ����� ����� �������� �������
		if (ovf)
		{
			// ��������� ����������� - ����� ���������
			// ��������� �������� ����� �������� �����.
			// ��������� �������
			elkey->morse >>= ELSIZE;
			switch (elkey->morse & ELMASK)
			{
			case MDASH:
				setnextstate(elkey, ELKEY_STATE_AUTO_ELEMENT_DASH, delay_dash);
				elkey_vibroplex_reset(elkey);	// ����������� ��������� ���������� ������������ ���������. ��� vibroplex ���������.
				break;
			case MDIT:
				setnextstate(elkey, ELKEY_STATE_AUTO_ELEMENT_DIT, delay_dit - elkey->vibroplex_derate);
				elkey_vibroplex_next(elkey);
				break;
			default:
				// ���������� ����� �� ������
				setnextstate(elkey, ELKEY_STATE_AUTO_SPACE2, delay_dash - delay_space);	// or -1
				elkey_vibroplex_reset(elkey);	// ����������� ��������� ���������� ������������ ���������. ��� vibroplex ���������.
				break;
			}
		}
		break;

	case ELKEY_STATE_AUTO_SPACE2:	// ������ ������������� ����� ����� �������� �������
		if (ovf)
		{
			// � ������ ������ ������� � ���������� ���������
			setnextstate(elkey, ELKEY_STATE_AUTO_INITIALIZE, 0);
		}
		break;

#endif /* WITHCAT && WITHCATEXT */
	}
}

// ���������� � �������� 1/ELKEY_DISCRETE �� ������������ ����� 
void elkey_spool_dots(void)
{
	elkeyx_spool_dots(& elkey0, hardware_elkey_getpaddle(elkey_reverse));
#if WITHCAT	&& WITHCATEXT
	elkeyx_spool_dots(& elkey1, ELKEY_PADDLE_NONE);
#endif /* WITHCAT && WITHCATEXT */
}


static
void elkeyx_initialize(elkey_t * const elkey, uint_fast8_t state)
{

	elkey->state = state;
	elkey->maxticks = 0;
	elkey->ticks = 0;
#if WITHCAT && WITHCATEXT
	elkey->morse = 0;
#endif /* WITHCAT && WITHCATEXT */
#if WITHVIBROPLEX
	elkey->vibroplex_slope = 0;
	elkey->vibroplex_grade = 0;
#endif /* WITHVIBROPLEX */
	elkey->vibroplex_derate = 0;
}
/* ������������� ���������� � ������������ �����. ����������� ��� ����������� �����������. */
void elkey_initialize(void)
{
	hardware_elkey_ports_initialize();
	hardware_elkey_timer_initialize();
	elkeyx_initialize(& elkey0, ELKEY_STATE_INITIALIZE);
#if WITHCAT && WITHCATEXT
	elkeyx_initialize(& elkey1, ELKEY_STATE_AUTO_INITIALIZE);
#endif /* WITHCAT && WITHCATEXT */
}

#endif /* WITHELKEY */

/* ��������� ���� - ���������� �������� � ���������� ���� � ����� (� �������� ���������). */
/* ����������� ����������� (3:1) - 30 */
void elkey_set_format(
	uint_fast8_t dashratio,
	uint_fast8_t spaceratio
	)
{
#if WITHELKEY
	disableIRQ();

	delay_space = (spaceratio * ELKEY_DISCRETE) / 10;
	delay_dash = (dashratio * ELKEY_DISCRETE) / 10;
	//deay_half = (5 * ELKEY_DISCRETE) / 10;	//delay_space * 5 / 10;


	enableIRQ();
#endif /* WITHELKEY */
}


/* ��������� ���� - ���������� ����� ������ */
void 
elkey_set_mode(
	uint_fast8_t mode,	/* ����� ������������ ����� - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key, 4 - vibroplex */
	uint_fast8_t reverse
	)
{
#if WITHELKEY
	disableIRQ();
	elkey_reverse = reverse;
	switch (mode)
	{
	case 0:
		elkey_straight_flags = ACSFLAG; // DASHFLAG DITSFLAG
		break;
	case 1:
		elkey_straight_flags = 0; // DASHFLAG DITSFLAG
		break;
	case 2:
		elkey_straight_flags = DASHFLAG | DITSFLAG;
		break;
	// "bug" key - ����� ��������������, ���� �������
	case 3:
		elkey_straight_flags = DASHFLAG;
		break;
	}
	enableIRQ();
#endif /* WITHELKEY */
}

// ������������ ������� - ��������� ��������� ������ ������������ ����� ("�������") ��� ����������
// ������� ������ ���������� 0 ��� 1 � ����� �� ������ ��������
// ���������� �� ����������� ����������.
uint_fast8_t 
elkey_get_output(void)
{
#if WITHELKEY	
	const uint_fast8_t r = 
		elkeyout [elkey0.state] != 0 || 
		elkey_manual() ||
  #if WITHCAT	
	#if WITHCATEXT
		elkeyout [elkey1.state] != 0 ||
	#endif	/* WITHCATEXT */
		cat_get_keydown() ||
  #endif	/* WITHCAT */
		0;

	return r;
#else /* WITHELKEY */
	return 0;
#endif /* WITHELKEY */
}
