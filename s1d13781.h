/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ��������� ����������� TFT ������ Epson S1D13781
//

#ifndef S1D13781_H_INCLUDED
#define S1D13781_H_INCLUDED

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */

#if LCDMODE_S1D13781 || LCDMODE_LQ043T3DX02K

#define DISPLAYMODES_FPS 5	/* ���������� ���������� ����������� ������� ������ */
#define DISPLAY_FPS	20	/* ���������� ��������� ������� ������ ��� � ������� */
#define DISPLAYSWR_FPS 10	/* ���������� ���������� SWR � �������� �� ������� */

#if LCDMODE_S1D13781

	// ������� ����� ���������� - ��� �������� ������ �������� �� ����.
	//#define DIM_X               480
	//#define DIM_Y              272
	#define S1D_DISPLAY_WIDTH               480
	#define S1D_DISPLAY_HEIGHT              272

	#define S1D_DISPLAY_FRAME_RATE          0
	#define S1D_DISPLAY_PCLK                9000000L
	#define S1D_PHYSICAL_REG_ADDR           0x00060800uL
	#define S1D_PHYSICAL_VMEM_ADDR          0x00000000uL
	#define S1D_PHYSICAL_REG_SIZE           593L
	#define S1D_PHYSICAL_VMEM_SIZE          393216L
	#define S1D_PALETTE_SIZE                256

	#if 0

		// 24 bpp
		#define S1D_DISPLAY_BPP                 24
		#define S1D_DISPLAY_SCANLINE_BYTES      (S1D_DISPLAY_WIDTH * 3)
		#define S1D_PHYSICAL_VMEM_REQUIRED      391680L		// ������ ��� ����� PIP

	#elif 1

		// 16 bpp
		#define S1D_DISPLAY_BPP                 16
		#define S1D_DISPLAY_SCANLINE_BYTES      (S1D_DISPLAY_WIDTH * 2)
		#define S1D_PHYSICAL_VMEM_REQUIRED      261120L		// ������ ��� ����� PIP

	#else

		// 8 bpp
		#define S1D_DISPLAY_BPP                 8
		#define S1D_DISPLAY_SCANLINE_BYTES      (S1D_DISPLAY_WIDTH * 1)
		#define S1D_PHYSICAL_VMEM_REQUIRED      130560L		// ������ ��� ����� PIP

	#endif

	#define FREEMEMSTART S1D_PHYSICAL_VMEM_REQUIRED
	//#define FREEMEMSTART (S1D_PHYSICAL_VMEM_REQUIRED * 2) // ������������ � PIP
	#define PIPMEMSTART (S1D_PHYSICAL_VMEM_REQUIRED * 1) // ������������ � PIP
	// 480 x 272 pixel

#endif

#if (S1D_DISPLAY_BPP == 8) || LCDMODE_LTDC_L8


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


#elif S1D_DISPLAY_BPP == 16 || ! LCDMODE_LTDC_L8

	//#define LCDMODE_RGB565 1
	typedef uint_fast16_t COLOR_T;
	typedef uint16_t PACKEDCOLOR_T;

	// RRRR.RGGG.GGGB.BBBB
	#define TFTRGB(red, green, blue) \
		(  (uint_fast16_t) \
			(	\
				(((uint_fast16_t) (red) << 8) &   0xf800)  | \
				(((uint_fast16_t) (green) << 3) & 0x07e0) | \
				(((uint_fast16_t) (blue) >> 3) &  0x001f) \
			) \
		)

	#define TFTRGB565 TFTRGB

#elif S1D_DISPLAY_BPP == 24

	typedef uint_fast32_t COLOR_T;
	typedef uint32_t PACKEDCOLOR_T;

	// RRRRRRR.GGGGGGGG.BBBBBBBB
	#define TFTRGB(red, green, blue) \
		(  (unsigned long) \
			(	\
				(((unsigned long) (red) << 16) &   0xFF0000ul)  | \
				(((unsigned long) (green) << 8) & 0xFF00ul) | \
				(((unsigned long) (blue) >> 0) &  0xFFul) \
			) \
		)

#endif

// val = 0..DIM_Y
void drawfftbar(int x, int val, int maxval);

#endif /* LCDMODE_S1D13781 */
#endif /* S1D13781_H_INCLUDED */

