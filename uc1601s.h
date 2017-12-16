/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef UC1601S_H_INCLUDED
#define UC1601S_H_INCLUDED

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */

#if LCDMODE_UC1601

#define DISPLAYMODES_FPS 5	/* ���������� ���������� ����������� ������� ������ */
#define DISPLAY_FPS	10	/* ���������� ��������� ������� �� ������� */
#define DISPLAYSWR_FPS 10	/* ���������� ���������� SWR �� ������� */

// ������� ����������� �� �������������� �� ���� ������� (��������).
#define TFTRGB(r, g, b)	(0)
typedef uint_fast8_t COLOR_T;	/* ����� �� �������������� - �������� */
typedef uint8_t PACKEDCOLOR_T;	/* ����� �� �������������� - �������� */

#endif /* LCDMODE_UC1601 */

#endif /* UC1601S_H_INCLUDED */
