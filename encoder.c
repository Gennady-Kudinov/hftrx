/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#include "encoder.h"
#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */
#include "keyboard.h"	/* ������� ������ ������������ ����� */
//#include "display.h"	/* test */
#include "formats.h"	/* test */

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>


/* ����������� ���������� �� ��������� */

#if WITHHARDINTERLOCK
static volatile uint32_t position1;		/* ���������� �� ��������� - �������� ����� */
static volatile uint32_t position2;		/* ���������� �� ��������� - �������� ����� */
#else /* WITHHARDINTERLOCK */
static volatile int position1;		/* ���������� �� ��������� - �������� ����� */
static volatile int position2;		/* ���������� �� ��������� - �������� ����� */
#endif /* WITHHARDINTERLOCK */
#if WITHKBDENCODER
static int position_kbd;	/* ���������� �� ���������� - �������� ����� */
#endif /* WITHKBDENCODER */

// dimensions are:
// old_bits new_bits
static const int_fast8_t graydecoder [4][4] =
{
	{
		+0,		/* 00 -> 00 stopped				*/
		-1,		/* 00 -> 01 rotate left			*/
		+1,		/* 00 -> 10 rotate right		*/
		+0,		/* 00 -> 11 invalid combination */		
	},
	{
		+1,		/* 01 -> 00 rotate right		*/
		+0,		/* 01 -> 01 stopped				*/
		+0,		/* 01 -> 10 invalid combination */
		-1,		/* 01 -> 11 rotate left			*/
	},
	{
		-1,		/* 10 -> 00 rotate left			*/
		+0,		/* 10 -> 01 invalid combination */
		+0,		/* 10 -> 10 stopped				*/
		+1,		/* 10 -> 11 rotate right		*/
	},
	{
		+0,		/* 11 -> 00 invalid combination */
		+1,		/* 11 -> 01 rotate right		*/
		-1,		/* 11 -> 10 rotate left			*/
		+0,		/* 11 -> 11 stopped				*/
	},
};

static uint_fast8_t old_val;

void spool_encinterrupt(void)
{
	const uint_fast8_t new_val = hardware_get_encoder_bits();	/* ��������� ���� A - � ���� � ����� 2, ���� B - � ���� � ����� 1 */

#if ENCODER_REVERSE
	position1 -= graydecoder [old_val][new_val];
#else
	position1 += graydecoder [old_val][new_val];
#endif
	old_val = new_val;
}

static uint_fast8_t old_val2;

void spool_encinterrupt2(void)
{
	const uint_fast8_t new_val = hardware_get_encoder2_bits();	/* ��������� ���� A - � ���� � ����� 2, ���� B - � ���� � ����� 1 */

#if ENCODER2_REVERSE
	position2 -= graydecoder [old_val2][new_val];
#else
	position2 += graydecoder [old_val2][new_val];
#endif
	old_val2 = new_val;
}

static int safegetposition1(void)
{
#if WITHHARDINTERLOCK
	uint32_t r;
	do
		r = __LDREXW(& position1);
	while (__STREXW(0, & position1));
	return (int32_t) r;
#else /* WITHHARDINTERLOCK */
	global_disableIRQ();
	int r = position1;
	position1 = 0;
	global_enableIRQ();
	return r;
#endif /* WITHHARDINTERLOCK */
}

static int safegetposition2(void)
{
#if WITHHARDINTERLOCK
	uint32_t r;
	do
		r = __LDREXW(& position2);
	while (__STREXW(0, & position2));
	return (int32_t) r;
#else /* WITHHARDINTERLOCK */
	global_disableIRQ();
	int r = position2;
	position2 = 0;
	global_enableIRQ();
	return r;
#endif /* WITHHARDINTERLOCK */
}


static int safegetposition_kbd(void)
{
#if WITHKBDENCODER
	int r = position_kbd;
	position_kbd = 0;
	return r;
#else
	return 0;
#endif
}

#if WITHKBDENCODER

// ���������� �� ����������� ���������� ���������� - ����������.
void encoder_kbdctl(
	uint_fast8_t code, 		// ��� �������
	uint_fast8_t accel		// 0 - ��������� ������� �� �������, ����� ����������
	)
{
	int_fast8_t d = code == ENC_CODE_STEP_UP ? 1 : -1;
	position_kbd += d * (accel ? 5 : 1);

}

#endif


/* ���������� ���������� �� ��������� - �������� ����� */
static int rotate1;
static int rotate_kbd;
static int backup_rotate;

/* ���������� ���������� �� ��������� #2 - �������� ����� */
static int rotate2;
static int backup_rotate2;

#define HISTLEN 4		// ���-��� ������ ���� ���, � ������� ���� ������������ ��� ��������
#define TICKSMAX NTICKS(125)

static unsigned enchist [HISTLEN];	
static uint_fast8_t tichist;	// ������ ����������� ����� �� 0 �� TICKSMAX ������������

static uint_fast8_t enchistindex;	

/* �������� ������������ ��� ���������� - ����������� ��� ������� ���������� */

static unsigned encoder_actual_resolution = 128 * 4; //(encoder_resolution * 4 * ENCRESSCALE)	// Number of increments/decrements per revolution
static uint_fast8_t encoder_dynamic = 1;

//#define ENCODER_ACTUAL_RESOLUTION (encoder_resolution * 4 * ENCRESSCALE)	// Number of increments/decrements per revolution
//static uint_fast8_t encoder_resolution;

void encoder_set_resolution(uint_fast8_t v, uint_fast8_t encdynamic)
{
	//encoder_resolution = v;	/* ������������ ����������� ����� */
	encoder_actual_resolution = v * 4 * ENCRESSCALE;	/* ������������ ����������� ����� */
	encoder_dynamic = encdynamic;
}

// ���������� � �������� TICKS_FREQUENCY (��������, 200 ��) � ������������ ������������.
// ������ ������� �������� �������� ��������� (���������� ������ ��� ������� ��� ����������).
void	
enc_spool(void)
{
	const int p1 = safegetposition1();	// �������� #1
	const int p1kbd = safegetposition_kbd();
	rotate1 += p1;		/* ���� ���������� ��������� (��� ������� �������) */
#if WITHKBDENCODER
	rotate_kbd += p1kbd;		/* ���� ���������� ��������� (��� ������� �������) */
#endif

	/* ����������� ������ ��� ������� �������� �������� ��������� */
	/* ��� ������� �������� ������������ ����������� �������� - ���������� ��������� ��������, ����� ��� ��� 
	   ������������ ��������� ������������ ������ ����������� - �������� ����� � ����������� �������. ��� ���� ��� 
	   ��������������� ����������� ��������� �� ������ ����������.
	*/
	enchist [enchistindex] += abs(p1) + abs(p1kbd);
	if (++ tichist >= TICKSMAX)	// ���������� ������� - ��������� "��������� �������" ���������� �������� ���������
	{	
		tichist  = 0;
		enchistindex = (enchistindex + 1) % HISTLEN;
		enchist [enchistindex] = 0;		// ��������� ������ ���������� ����� ��������� ����� ��������������.
	}

	// �������� #2
	const int p2 = safegetposition2();
	rotate2 += p2;		/* ���� ���������� ��������� (��� ������� �������) */
}

/* ��������� ������ �� ���������� */

void encoder_clear(void)
{
	backup_rotate = 0;
	backup_rotate2 = 0;
	disableIRQ();
	rotate1 = 0;
	rotate_kbd = 0;

	// HISTLEN == 4
	enchist [0] = enchist [1] = enchist [2] = enchist [3] = 0; 
	tichist = 0;

	enableIRQ();
}

/* ��������� ���������� ����� � �������� ��������. */
int 
encoder_get_snapshot(
	unsigned * speed, 
	const uint_fast8_t derate)
{
	int hrotate;
	
	unsigned s;				// ���������� ����� �� ����� ���������
	unsigned tdelta;	// ����� ���������

	disableIRQ();

	// ��������� ����������� �������� �� ������������
	// 1. ���������� ����� �� ����� ���������
	// HISTLEN == 4
	s =
		enchist [0] + enchist [1] + enchist [2] + enchist [3]; // ���������� ����� ��������� �� ����� ����������
	// 2. ����� ���������
	tdelta = tichist + TICKSMAX * (HISTLEN - 1); // �� ���� ��������� ������, ����� ��������, ���������� ����� ������������.

	hrotate = rotate1 + rotate_kbd * derate;	/* ������ � ���� �� ������ - ��������� ����� */
	rotate1 = 0;
	rotate_kbd = 0;


	enableIRQ();

	// ������ ��������. ��������� - (1 / ENCODER_NORMALIZED_RESOLUTION) ����� ������� �� �������
	// ���� ��������� ENCODER_NORMALIZED_RESOLUTION ��� ���������� ���� ������ � �������
	// ((s * TICKS_FREQUENCY) / t) - ��������� � ����������� "��������� � �������".
	* speed = ((s * (unsigned long) TICKS_FREQUENCY * ENCODER_NORMALIZED_RESOLUTION) / (tdelta * (unsigned long) encoder_actual_resolution));
	
	/* ���������� ���������� ��������� � ����������� �� ��������� � ���� */
	const div_t h = div(hrotate + backup_rotate, derate);

	backup_rotate = h.rem;

	return h.quot;
}

/* ��������� ���������� ����� � �������� ��������. */
int 
encoder2_get_snapshot(
	unsigned * speed, 
	const uint_fast8_t derate)
{
	int hrotate;

	disableIRQ();
	hrotate = rotate2;
	rotate2 = 0;
	enableIRQ();

	/* ���������� ���������� ��������� � ����������� �� ��������� � ���� */
	const div_t h = div(hrotate + backup_rotate2, derate);
	backup_rotate2 = h.rem;
	* speed = 0;
	return h.quot;
}

/* ��������� "���������������" ���������� ���������� �� ���������.
 * �� ��� �������� ����� ������� �� scale, �������� � ����������
 */
int getRotateLoRes(uint_fast8_t hiresdiv)
{
	unsigned speed;
	return encoder_get_snapshot(& speed, encoder_actual_resolution * hiresdiv / ENCODER_MENU_STEPS);
}


// ���������� ��������� ��������� �� CAT
void encoder_pushback(int outsteps, uint_fast8_t hiresdiv)
{
	backup_rotate += (outsteps * (int) hiresdiv);
}
/* ��������� ������������ �������� ���������� �� ���������.
		���������� ������������ */
int_least16_t 
getRotateHiRes(
	uint_fast8_t * jumpsize,	/* jumpsize - �� ������� ��� ������������� �������� ����������� */
	uint_fast8_t hiresdiv
	)
{

	typedef struct accel_tag
	{
		unsigned speed;
		uint_fast8_t muliplier;
	} accel;

	static const accel velotable [] =
	{
#if REQUEST_BA
		// ��� �.�.
		{	ENCODER_NORMALIZED_RESOLUTION * 22U / 10U,	100U	},	// 2.2 ������� � ������� 100 �����
		{	ENCODER_NORMALIZED_RESOLUTION * 17U / 10U,	50U	},	// 1.7 ������� � ������� 50 �����
		{	ENCODER_NORMALIZED_RESOLUTION * 15U / 10U,	10U	},	// 1.5 ������� � ������� 10 �����
		{	ENCODER_NORMALIZED_RESOLUTION * 8U / 10U, 5U	},	// 0.8 ������� � ������� 5 �����
		{	ENCODER_NORMALIZED_RESOLUTION * 5U / 10U, 2U },	// 0.5 ������� � ������� - �������� ����
		{	ENCODER_NORMALIZED_RESOLUTION * 3U / 10U, 1U },	// 0.3 ������� � ������� - ���������� ���
#elif 1
		{	ENCODER_NORMALIZED_RESOLUTION * 45U / 10U,	200U },	// 4.5 ������� � �������
		{	ENCODER_NORMALIZED_RESOLUTION * 25U / 10U,	20U },	// 2.5 ������� � �������
		{	ENCODER_NORMALIZED_RESOLUTION * 16U / 10U,	5U },	// 1.6 ������� � �������
		{	ENCODER_NORMALIZED_RESOLUTION * 8U / 10U, 2U },	// 0.8 ������� � ������� - �������� ����
#else
		{	ENCODER_NORMALIZED_RESOLUTION * 22U / 10U,	50U },	// 2.2 ������� � �������
		{	ENCODER_NORMALIZED_RESOLUTION * 15U / 10U, 10U	},	// 1.5 ������ � �������
		{	ENCODER_NORMALIZED_RESOLUTION * 8U / 10U, 2U },	// 0.8 ������� � ������� - �������� ����
		//{	ENCODER_NORMALIZED_RESOLUTION * 2U / 10U, 1U },	// 0.2 ������� � ������� - ���������� ���
#endif
	};


	unsigned speed;
	int nrotate = encoder_get_snapshot(& speed, hiresdiv);

	if (encoder_dynamic != 0)
	{
		uint_fast8_t i;

		for (i = 0; i < (sizeof velotable / sizeof velotable [0]); ++ i)
		{
			const unsigned vtspeed = velotable [i].speed;
			//if (vtspeed == 0)
			//	break;			/* ������ �� �� ����� ����������� ������� */
			if (speed >= vtspeed)
			{
				* jumpsize = velotable [i].muliplier;
				return nrotate;
			}
		}
	}

	* jumpsize = 1;
#if REQUEST_BA

	/* ���������� ���������� ��������� ��� ��������� �������, ��� � ������� �������� ������ 48 */
	const div_t h = div(nrotate, encoder_actual_resolution / ENCODER_SLOW_STEPS);

	backup_rotate += h.rem * hiresdiv;

	return h.quot;
#else
	return nrotate;
#endif
}

/* ��������� ������������ �������� ���������� �� ���������.
		���������� ������������ */
int_least16_t 
getRotateHiRes2(
	uint_fast8_t * jumpsize	/* jumpsize - �� ������� ��� ������������� �������� ����������� */
	)
{
	#if defined (BOARD_ENCODER2_DIVIDE)
		const unsigned loresdiv = BOARD_ENCODER2_DIVIDE;
	#else /* defined (BOARD_ENCODER2_DIVIDE) */
		const unsigned loresdiv = 2;	/* �������� ��� ��������� PEC16-4220F-n0024 (� ���������") */
	#endif /* defined (BOARD_ENCODER2_DIVIDE) */

	unsigned speed;
	int nrotate = encoder2_get_snapshot(& speed, loresdiv);

	* jumpsize = 1;
	return nrotate;
}

/* ���������� ��� ����������� ����������� */
void encoder_initialize(void)
{
	//rotate = backup_rotate = 0;
	//enchistindex = 0;
	//tichist [enchistindex] = 0;
	//enchist [enchistindex] = 0;


	old_val = hardware_get_encoder_bits();	/* ��������� ���� A - � ���� � ����� 2, ���� B - � ���� � ����� 1 */
	old_val2 = hardware_get_encoder2_bits();	/* ��������� ���� A - � ���� � ����� 2, ���� B - � ���� � ����� 1 */
}
