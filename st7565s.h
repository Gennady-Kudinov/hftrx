/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//


#ifndef ST7565S_H_INCLUDED
#define ST7565S_H_INCLUDED

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */

#if LCDMODE_ST7565S || LCDMODE_PTE1206

#define DISPLAYMODES_FPS 5	/* ���������� ���������� ����������� ������� ������ */
#define DISPLAY_FPS	10	/* ���������� ��������� ������� ������ ��� � ������� */
#define DISPLAYSWR_FPS 5	/* ���������� ���������� SWR �� ������� */

// ������� ����������� �� �������������� �� ���� ������� (��������).
#define TFTRGB(r, g, b)	(0)
typedef uint_fast8_t COLOR_T;	/* ����� �� �������������� - �������� */
typedef uint8_t PACKEDCOLOR_T;	/* ����� �� �������������� - �������� */

#endif /* LCDMODE_ST7565S || LCDMODE_PTE1206 */

#endif /* ST7565S_H_INCLUDED */

