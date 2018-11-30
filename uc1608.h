/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef UC1608_H_INCLUDED
#define UC1608_H_INCLUDED

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */

#if LCDMODE_UC1608

	#define DISPLAYMODES_FPS 5	/* ���������� ���������� ����������� ������� ������ */
	#define DISPLAY_FPS	10	/* ���������� ��������� ������� ������ ��� � ������� */
	#define DISPLAYSWR_FPS 5	/* ���������� ���������� SWR �� ������� */

	// ������� ����������� �� �������������� �� ���� ������� (��������).
	#define TFTRGB(r, g, b)	(0)
	typedef uint_fast8_t COLOR_T;	/* ����� �� �������������� - �������� */
	typedef uint8_t PACKEDCOLOR_T;	/* ����� �� �������������� - �������� */

	#define TFTRGB565 TFTRGB

#endif /* LCDMODE_UC1608 */

#endif /* UC1608_H_INCLUDED */
