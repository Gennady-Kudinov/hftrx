/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED 1

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */

#include <stdint.h>

enum
{
	KBD_CODE_SPLIT = 0, 
	KBD_CODE_SPLIT_HOLDED,
	KBD_CODE_A_EX_B,		// A/B symbol on key
	KBD_CODE_A_EQ_B,		// A=B sy,bol on key
	KBD_CODE_ATT,
	KBD_CODE_BW,
	KBD_CODE_IFSHIFT,
	KBD_CODE_BAND_UP,
	KBD_CODE_BAND_DOWN,
	KBD_CODE_BKIN,
	KBD_CODE_BKIN_HOLDED,

	KBD_CODE_MENU,
	KBD_CODE_MODE, KBD_CODE_MODEMOD,	// ������������ ������� - ������ ��� ��� �������� �������, ������ - ��� ����������
#if WITHDIRECTBANDS
	KBD_CODE_MODE_3, KBD_CODE_MODEMOD_3,
	KBD_CODE_MODE_2, KBD_CODE_MODEMOD_2,
	KBD_CODE_MODE_1, KBD_CODE_MODEMOD_1,
	KBD_CODE_MODE_0, KBD_CODE_MODEMOD_0,
#endif /* WITHDIRECTBANDS */
	KBD_CODE_LOCK,
	KBD_CODE_USEFAST,
	KBD_CODE_AFNARROW,		// ����������� ��� � ����� ����� ������
	KBD_CODE_TXTUNE,
	KBD_CODE_ATUSTART,
	KBD_CODE_ATUBYPASS,
	KBD_CODE_DISPMODE,		// ����������� ������ ������������ �� ������� ����������

	KBD_CODE_ENTERFREQ,		// ������� � ����� ������� ����� �������
	KBD_CODE_ENTERFREQDONE,	// �����

	KBD_CODE_VOXTOGGLE,	
	KBD_CODE_PWRTOGGLE,
	KBD_CODE_NOTCHTOGGLE,
	KBD_CODE_NOTCHFREQ,
	KBD_CODE_COMPTOGGLE,
	KBD_CODE_ALTTOGGLE,
	KBD_CODE_LDSPTGL,
	KBD_CODE_DATATOGGLE,
	KBD_CODE_DWATCHTOGGLE,
	KBD_CODE_AMBANDPASSUP,	// ���������� ������� ������� ����� �� ������� � ������ ��
	KBD_CODE_AMBANDPASSDOWN,	// ���������� ������� ������� ����� �� ������� � ������ ��

	KBD_CODE_LOCK_HOLDED,
	KBD_CODE_MENU_CWSPEED,
	KBD_CODE_AGC,
	KBD_CODE_PAMP,
	KBD_CODE_GEN_HAM,
	KBD_CODE_ANTENNA,
	KBD_CODE_MOX,
	KBD_CODE_DIGI,

	// ������ ����������� ���������� ������
	KBD_CODE_TUNERTYPE,
	KBD_CODE_CAP_UP,
	KBD_CODE_CAP_DOWN,
	KBD_CODE_IND_UP,
	KBD_CODE_IND_DOWN,

	KBD_CODE_RECORDTOGGLE,		// SD CARD audio recording
	KBD_CODE_RECORD_HOLDED,

	KBD_CODE_VIBROCTL,		// 328
	KBD_CODE_CWSPEEDUP,
	KBD_CODE_CWSPEEDDOWN,
	//
	ENC_CODE_STEP_UP,		// ��������� ������ � �������� - �������������� ��� ���������
	ENC_CODE_STEP_DOWN,		// ��������� ������ � �������� - �������������� ��� ���������
	//
	KBD_CODE_0,
	KBD_CODE_1,
	KBD_CODE_2,
	KBD_CODE_3,
	KBD_CODE_4,
	KBD_CODE_5,
	KBD_CODE_6,
	KBD_CODE_7,
	KBD_CODE_8,
	KBD_CODE_9,
	KBD_CODE_10,
	KBD_CODE_11,
	KBD_CODE_12,
	KBD_CODE_13,
	KBD_CODE_14,
	KBD_CODE_15,
	KBD_CODE_16,
	KBD_CODE_17,
	KBD_CODE_18,
	KBD_CODE_19,
	KBD_CODE_20,
	KBD_CODE_21,
	KBD_CODE_22,
	KBD_CODE_23,

	KBD_CODE_USER1,
	KBD_CODE_USER2,
	KBD_CODE_USER3,
	KBD_CODE_USER4,
	KBD_CODE_USER5,
	//
	KBD_CODE_POWEROFF,
	//
#if WITHDIRECTBANDS
	KBD_CODE_BAND_1M8, KBD_CODE_BAND_3M5, KBD_CODE_BAND_5M3, KBD_CODE_BAND_7M0, 
	KBD_CODE_BAND_10M1, KBD_CODE_BAND_14M0, KBD_CODE_BAND_18M0, 
	KBD_CODE_BAND_21M0, KBD_CODE_BAND_24M9, KBD_CODE_BAND_28M0, KBD_CODE_BAND_50M0,
#endif /* WITHDIRECTBANDS */
	//
	KBD_CODE_MAX		// �� ���� ��� ����������� �������������� �������.
};

// �����, ���������� ����������� ������� �������:
// KIF_NONE - �������� �������, ��� ����������� (������������� � ���������)
// KIF_SLOW - � ��������� ������������
// KIF_FAST - � ������� ������������
// KIF_ERASE - ������� ���� ������� �� ������� ����������� ��� ������� �� �������� NVRAM
// KIF_EXTMENU - ������� ���� ������� �� ������� ��� ���������� ������� � ������������ ������ ������� ����.

enum
{
	KIF_SLOW = 0x01, 
	KIF_FAST = 0x02, 
	KIF_ERASE = 0x04, 
	KIF_EXTMENU = 0x08,
	KIF_SLOW4 = 0x10,
	//
	KIF_USER2 = 0x20,	// rec
	KIF_USER3 = 0x40,	// play

	KIF_NONE = 0x00
};

#define HOLDED_SPEEDUP 5	/* �� ������� ��� ������� �������� ����������� ��������� ��� ���������� ������� */

/* ��������� ����-���� ������� ��� 0 � ������ ����������.
 * ���� ������� ��������, ������������ ����-��� � ����������� ����� 0x80
 */
/* ������������� ���������� ������ � ����������� */
/* ������� ��-���� - �������� ����� ������ ������� */
void  kbd_initialize(void);

uint_fast8_t kbd_is_tready(void);
/* ��������� ����-���� �������, ������� 0 � ������ ����������.
 */
uint_fast8_t kbd_scan(uint_fast8_t * key);
uint_fast8_t kbd_get_ishold(uint_fast8_t key);
uint_fast8_t kbd_getnumpad(uint_fast8_t key);

struct qmkey 
{
	uint_fast8_t flags;
	uint_fast8_t code, holded;
	uint_fast8_t numpad;		/* ��� ������� � ������ ��������� ����� */
};
extern const struct qmkey qmdefs [];
uint_fast8_t getexitkey(void);


#define KEYBOARD_NOKEY UINT8_MAX

uint_fast8_t board_get_pressed_key(void);	// ���� ������ - ���������� KEYBOARD_NOKEY
void board_kbd_initialize(void);

#endif /* KEYBOARD_H_INCLUDED */
