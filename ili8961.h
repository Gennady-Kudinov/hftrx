/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef ILI8961_H_INCLUDED
#define ILI8961_H_INCLUDED

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */

#if LCDMODE_ILI8961

	#define DISPLAYMODES_FPS 5	/* ���������� ���������� ����������� ������� ������ */
	#define DISPLAY_FPS	10	/* ���������� ��������� ������� ������ ��� � ������� */
	#define DISPLAYSWR_FPS 5	/* ���������� ���������� SWR �� ������� */

	#if LCDMODE_LTDC

		// ����������� COLOR_T, PACKEDCOLOR_T � TFTRGB ������������ �� diaplay.h - ������ ���������� �������� ������ � ����� LCDMODE_LTDC

	#else /* LCDMODE_LTDC */

		#error Use LCDMODE_LTDC

	#endif /* LCDMODE_LTDC */

#endif /* LCDMODE_ILI8961 */

#endif /* ILI8961_H_INCLUDED */

