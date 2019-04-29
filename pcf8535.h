/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//



#ifndef PCF8535_H_INCLUDED
#define PCF8535_H_INCLUDED

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */

#if LCDMODE_PCF8535 || LCDMODE_PCF8531

#define DISPLAYMODES_FPS 4	/* ���������� ���������� ����������� ������� ������ */
#define DISPLAY_FPS	4	/* ���������� ��������� ������� �� ������� */
#define DISPLAYSWR_FPS 4	/* ���������� ���������� SWR �� ������� */



// ������� ����������� �� �������������� �� ���� ������� (��������).
#define TFTRGB(r, g, b)	(0)
typedef uint_fast8_t COLOR_T;	/* ����� �� �������������� - �������� */
typedef uint8_t PACKEDCOLOR_T;	/* ����� �� �������������� - �������� */

#endif /* LCDMODE_PCF8535 || LCDMODE_PCF8531 */

#endif /* PCF8535_H_INCLUDED */

