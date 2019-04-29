/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef HD44780_H_INCLUDED
#define HD44780_H_INCLUDED

#include "hardware.h"

#if LCDMODE_HD44780

#define CHARS2GRID(columns) ((columns) * 1)		/* ��������� ���������� �������� � ������ ����� �������� ���������� */
#define ROWS2GRID(rows) ((rows) * 1)		/* ��������� ���������� �������� � ������ ����� �������� ���������� */

#if 0

	#define LCDMODE_SMETER3	1	/* �� ������ ���������� ��� �������� ������ */
	#define GRID2X(cellsx) ((cellsx) * 3)	/* ������� ����� ����� �������� � ����� ������� �� ����������� */

#else

	#define LCDMODE_SMETER2	1	/* �� ������ ���������� ��� �������� ������ */
	#define GRID2X(cellsx) ((cellsx) * 2)	/* ������� ����� ����� �������� � ����� ������� �� ����������� */

#endif


#define GRID2Y(cellsy) ((cellsy) * 1)	/* ������� ����� ����� �������� � ����� ������� �� ��������� */

#define DISPLAYMODES_FPS 5	/* ���������� ���������� ����������� ������� ������ */
#define DISPLAY_FPS	10	/* ���������� ��������� ������� �� ������� */
#define DISPLAYSWR_FPS 5	/* ���������� ���������� SWR �� ������� */

// ������� ����������� �� �������������� �� ���� ������� (��������).
#define TFTRGB(r, g, b)	(0)
typedef uint_fast8_t COLOR_T;	/* ����� �� �������������� - �������� */
typedef uint8_t PACKEDCOLOR_T;


// ��������� ���� �������� ��� ����������� �������������
#define PATTERN_BAR_FULL 4	/* � ������ ���� �������� ����� ���� 3 - � ����� ����� ��� ��� ���� ������������ */
#define PATTERN_BAR_HALF 0
#define PATTERN_BAR_EMPTYHALF 0 // ���� ��� �� ������������ - ��� ������������� � ����������� ����������� �����������
#define PATTERN_BAR_EMPTYFULL 0	// ���� ��� �� ������������ - ��� ������������� � ����������� ����������� �����������
#define PATTERN_SPACE	0x20	/* ������� ����� �� SWR � PWR ������ ���� �������� */

void hd44780_io_initialize(void);	/* ��������� ������������ ���������� ������ � ��������, ���������� ��� ����������� �����������. */


#endif /* LCDMODE_HD44780 */

#endif /* HD44780_H_INCLUDED */
