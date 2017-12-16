/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
/* ��������� 176*132 Sharp LS020B8UD06 � ������������ LR38826 */


#ifndef LS020_H_INCLUDED
#define LS020_H_INCLUDED

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */

#if LCDMODE_LS020

#define DISPLAYMODES_FPS 5	/* ���������� ���������� ����������� ������� ������ */
#define DISPLAY_FPS	10	/* ���������� ��������� ������� �� ������� */
#define DISPLAYSWR_FPS 5	/* ���������� ���������� SWR �� ������� */

// 132 x 176 pixel
/*
	Active Display Area 31.284 x 41.712 mm� (W x H)
	Blackmatrix area 34,2 x 44,9 mm� (W x H)
	Outline Dimensions panel 35.9 x 52,0 mm� (W x H)
	Design Viewing Direction 6 o�clock display
	Controller: 12 o�clock of the display module
*/

typedef uint_fast8_t COLOR_T;
typedef uint8_t PACKEDCOLOR_T;


// RRRGGGBB
#define TFTRGB(red, green, blue) \
	(  (unsigned char) \
		(	\
			(((red) >> 0) & 0xe0)  | \
			(((green) >> 3) & 0x1c) | \
			(((blue) >> 6) & 0x03) \
		) \
	)


#endif /* LCDMODE_LS020 */

#endif /* LS020_H_INCLUDED */

