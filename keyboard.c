/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */
#include "keyboard.h"
#include "encoder.h"
#include "board.h"

#if WITHKEYBOARD
enum 
{
	KBD_BEEP_LENGTH =			NTICKS(25),	// ������������ ����������� �������

	KBD_STABIL_PRESS =			NTICKS(60),	// ����� ��� ���������� �������
	KBD_STABIL_RELEASE =		NTICKS(60),	// ����� ��� ���������� ����������

	KBD_MAX_PRESS_DELAY_LONG =	NTICKS(600), // RK4CI:1600 ����� ��� ����������� ��������� ������ � ��������� ������������
	KBD_PRESS_REPEAT_SLOW =		NTICKS(400),	// ����� ����� ��������� �� ���������� �����������

	KBD_MAX_PRESS_DELAY_LONG4 =	NTICKS(200), // RK4CI:1600 ����� ��� ����������� ��������� ������ � ��������� ������������
	KBD_PRESS_REPEAT_SLOW4 =	NTICKS(100),	// ����� ����� ��������� �� ���������� �����������

	//KBD_MED_STAGE1 = NTICKS(200),			// �������, �� ������� �������������� ��������� ������ ������� � ������� ������������
	//KBD_MED_STAGE2 = NTICKS(300),
	//KBD_MED_STAGE3 = NTICKS(400),
	//KBD_MED_STAGE4 = NTICKS(500),
	KBD_TIME_SWITCH_QUICK = NTICKS(200), // � ����� ������� ���������� ������� ����������
	KBD_PRESS_REPEAT_QUICK1 =	NTICKS(20),	// ����� ����� ��������� �� �������� �����������
	KBD_PRESS_REPEAT_QUICK2 =	NTICKS(5)	// ����� ����� ��������� �� ����� �������� �����������
};


static uint_fast8_t
getstablev8(volatile uint_fast8_t * p)
{
	uint_fast8_t v1 = * p;
	uint_fast8_t v2;
	do
	{
		v2 = v1;
		v1 = * p;
	} while (v2 != v1);
	return v1;

}

static uint_fast16_t
getstablev16(volatile uint_fast16_t * p)
{
	uint_fast8_t v1 = * p;
	uint_fast8_t v2;
	do
	{
		v2 = v1;
		v1 = * p;
	} while (v2 != v1);
	return v1;

}

// ������� �� 8 ��� - ��� 200 ���� ���������� 600 �� ����� 120 �����.
static volatile uint_fast16_t kbd_press;	/* ����� � ������� ������� */
static uint_fast16_t kbd_release;		/* ����� ����� ���������� - ������ ������ ������� */

static uint_fast8_t kbd_last;	/* ��������� ����-��� (������� ��� ���������� ������) */
static uint_fast8_t kbd_slowcount; /* ���������� ��������������� �������� � ��������� ������������ */

static uint_fast8_t kbd_beep;	/* ����� � ������� ������� */

/* ��������� ����-���� ������� ��� 0 � ������ ����������.
 * ���� ������� ��������, ������������ ����-��� � ����������� ����� 0x80
 * ������������
 */
static uint_fast8_t
kbd_scan_local(uint_fast8_t * key)
{
	const uint_fast8_t chinp = board_get_pressed_key();
	const uint_fast8_t notstab = (kbd_press < (KBD_STABIL_PRESS + 1));

	if (chinp != KEYBOARD_NOKEY)
	{
		if (/*kbd_release != 0 && */ ! notstab && (kbd_last != chinp))		// ������� ��������� � ��������� ����������� �������
		{
			// ����� ������ �������
			kbd_last = chinp;
			kbd_press = 1;
			kbd_release = 0;
			kbd_slowcount = 0;
			//dbg_putchar('t');
			return 0;
		}
		else if (kbd_release != 0 && ! notstab)
		{
			// ��� ���� ��������������������� �������� - �� ������ ������.
			// kbd_last ��� �������� ���������� ��������
			// ������� ������ ��������� �����-�� �������.
			//dbg_putchar('k');
		}
		else if (kbd_press == 0)
		{
			// ����� ������ �������
			kbd_last = chinp;
			kbd_press = 1;
			kbd_release = 0;
			kbd_slowcount = 0;
			//dbg_putchar('l');
			return 0;
		}	
		else if (notstab && (kbd_last != chinp))		// �������� ������������ ���� �������
		{	
			kbd_last = chinp;
			kbd_press = 1;
			kbd_release = 0;
			kbd_slowcount = 0;
			//dbg_putchar('m');
			return 0;
		}

		kbd_release = KBD_STABIL_RELEASE;
		
		const uint_fast8_t flags = qmdefs [kbd_last].flags;
		/* ��������� ����� ������, ��� ������� �������� ��������� ���������� ��� ���������� ��������� */
		if ((flags & KIF_SLOW) != 0)	//(is_slow_repeat(kbd_last))
		{
			// ������� ����� �������� � ��������� ������������
			switch (++ kbd_press)
			{
			case KBD_MAX_PRESS_DELAY_LONG + KBD_PRESS_REPEAT_SLOW:
				kbd_press = KBD_MAX_PRESS_DELAY_LONG;	// ��������� ��� ��� ���� �������.

			case KBD_MAX_PRESS_DELAY_LONG:
				* key = qmdefs [kbd_last].code;	
				kbd_release = 0;
				return 1;
			}
			return 0;
		}
		/* ��������� ����� ������, ��� ������� �������� ��������� ���������� ��� ���������� ��������� */
		else if ((flags & KIF_SLOW4) != 0)	//(is_slow_repeat(kbd_last))
		{
			// ������� ����� �������� � ��������� ������������
			switch (++ kbd_press)
			{
			case KBD_MAX_PRESS_DELAY_LONG4 + KBD_PRESS_REPEAT_SLOW4:
				kbd_press = KBD_MAX_PRESS_DELAY_LONG4;	// ��������� ��� ��� ���� �������.

			case KBD_MAX_PRESS_DELAY_LONG4:
				* key = qmdefs [kbd_last].code;	
				kbd_release = 0;
				return 1;
			}
			return 0;
		}
		/* ��������� ����� ������, ��� ������� �������� ������� ���������� ��� ���������� ��������� */
		else if ((flags & KIF_FAST) != 0)	//(is_fast_repeat(kbd_last))
		{
#if WITHKBDENCODER
			// ������� ����� �������� � ������� ������������
			// ����������� ��������� ������ ���������
			switch (++ kbd_press)
			{
			case KBD_TIME_SWITCH_QUICK:
				if (kbd_slowcount < 20)
				{
					//++ kbd_slowcount;	// ���������������� - ������� �� ����������
					kbd_press = KBD_TIME_SWITCH_QUICK - KBD_PRESS_REPEAT_QUICK1;
				}
				else
					kbd_press = KBD_TIME_SWITCH_QUICK - KBD_PRESS_REPEAT_QUICK2;
				// ������������ ������� � �����������
				encoder_kbdctl(qmdefs [kbd_last].code, 1);
				//dbg_putchar('R');
				//dbg_putchar('0' + kbd_last);
				break;
			default:
				//dbg_putchar('U');
				break;
			}
			return 0;
#endif /* WITHKBDENCODER */
		}
		else
		{
			// ������� ����� �������� � ������� ��������
			if (kbd_press == KBD_MAX_PRESS_DELAY_LONG)
				return 0;	// lond_press symbol already returned
			if (kbd_press < KBD_MAX_PRESS_DELAY_LONG)
			{
				if (++ kbd_press == KBD_MAX_PRESS_DELAY_LONG)
				{	
					* key = qmdefs [kbd_last].holded; // lond_press symbol
					//
					return 1;
				}
			}
		}
		return 0;

	}
	else if (kbd_release != 0) // ��� ������� �������� - ���� �������
	{
		if (notstab)
		{
			kbd_press = 0;		// ������� �������� ������� ����������
			kbd_release = 0;
			//dbg_putchar('J');
			return 0;
		}
		//dbg_putchar('r');
		// keyboard keys released, time is not expire.
		if (-- kbd_release == 0)
		{
			// time is expire
			if ((qmdefs [kbd_last].flags & KIF_FAST) != 0)
			{
#if WITHKBDENCODER
				// ����������� ��������� ������ ���������
				//if (kbd_press < KBD_MED_STAGE1)
				if (kbd_slowcount == 0)
				{
					encoder_kbdctl(qmdefs [kbd_last].code, 0);
					//dbg_putchar('Q');
					//dbg_putchar('0' + kbd_last);
				}
				//else
				//	dbg_putchar('F');

				kbd_press = 0;
				kbd_slowcount = 0;
				return 0;		// ��� ���� ������������ �� �������� �����������
#endif /* WITHKBDENCODER */
			}
			else if (kbd_press < KBD_MAX_PRESS_DELAY_LONG)
			{
				* key = qmdefs [kbd_last].code;	
				kbd_press = 0;
				kbd_slowcount = 0;

				return 1;		// ������������ �� ���������������� ������� �� �������.
			}
			else
			{
				kbd_press = 0;
				kbd_slowcount = 0;
				return 0;		// ��� ���� ������������ �� ����������� ��� �� �������� �������.
			}
		}
		return 0;

	}
	else // ��� ������� ������� � ������ �������� ����� ����
		return 0;
}

static volatile uint_fast8_t kbd_ready;

// ���������� � �������� TICKS_FREQUENCY ����
// ����� ���������� ������� ����� ADC �� ���� ������.
void
kbd_spool(void)
{
	uint_fast8_t code;
	if (kbd_scan_local(& code) != 0)
	{
		kbd_beep = KBD_BEEP_LENGTH;
		board_keybeep_enable(1);	/* ������ ������������ ��������� ������� */

#if ! WITHBBOX
		uint8_t * buff;
		if (takemsgbufferfree_low(& buff) != 0)
		{
			buff [0] = code;
			placesemsgbuffer_low(MSGT_KEYB, buff);
		}
#endif /* ! WITHBBOX */
	}
	else
	{
		if (kbd_beep != 0 && -- kbd_beep == 0)
			board_keybeep_enable(0);
	}
	kbd_ready = 1;
}

uint_fast8_t 
kbd_is_tready(void)
{
#if CPUSTYLE_STM32F30X

	// ���� �� ���� ����������� �� ������� ���
	#warning TODO: remove this
	return 1;

#elif WITHCPUADCHW && KEYBOARD_USE_ADC

	uint_fast8_t f;

	f = getstablev8(& kbd_ready);
	return f;

#else /* WITHCPUADCHW */
	// ������������ ��� ���
	return 1;
#endif /* WITHCPUADCHW */
}

/* ��������, ������ �� ������� c ��������� ������
// KIF_ERASE ��� KIF_EXTMENU
 */
uint_fast8_t kbd_get_ishold(uint_fast8_t flag)
{
#if WITHBBOX
	return 0;
#else /* WITHBBOX */
	uint_fast8_t r;
	uint_fast16_t f = getstablev16(& kbd_press);

	r = (f != 0) && (qmdefs [kbd_last].flags & flag);
	return r;
#endif /* WITHBBOX */
}

/* ������������� ���������� ������ � ����������� */
void kbd_initialize(void)
{
	// todo: ��� ���������� ������ ����� ���� ������.

	////kbd_press = 0;
	////kbd_release = 0;
	////kbd_repeat = 0;
}

#else /* WITHKEYBOARD */

/* ������������� ���������� ������ � ����������� */
void kbd_initialize(void)
{
}

uint_fast8_t 
kbd_is_tready(void)
{
	return 1;
}

uint_fast8_t kbd_get_ishold(uint_fast8_t flag)
{
	return 0;
}

#endif /* WITHKEYBOARD */
