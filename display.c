/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ��������� ��� LS020 ������� �������, livas60@mail.ru
//

#include "hardware.h"
#include "board.h"
#include "spifuncs.h"	// hardware_spi_master_send_frame

#include "display.h"
#include "formats.h"
#include <string.h>

#if LCDMODE_LTDC && ! defined (SDRAM_BANK_ADDR)
	// ����� ������
	RAMNOINIT_D1 ALIGNX_BEGIN FRAMEBUFF_T framebuff0 ALIGNX_END;	//L8 (8-bit Luminance or CLUT)
#endif /* LCDMODE_LTDC */

#define DMA2D_AMTCR_DT_VALUE 2

#if LCDMODE_LTDC

	#if LCDMODE_LTDC_L24
		#define DMA2D_FGPFCCR_CM_VALUE	(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
		#define DMA2D_OPFCCR_CM_VALUE	(1 * DMA2D_FGPFCCR_CM_0)	/* 001: RGB888 */
	#elif LCDMODE_LTDC_L8
		#define DMA2D_FGPFCCR_CM_VALUE	(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */
		//#define DMA2D_OPFCCR_CM_VALUE	(x * DMA2D_FGPFCCR_CM_0)	/* not supported */
	#else /* LCDMODE_LTDC_L8 */
		#define DMA2D_FGPFCCR_CM_VALUE	(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
		#define DMA2D_OPFCCR_CM_VALUE	(2 * DMA2D_FGPFCCR_CM_0)	/* 010: RGB565 */
	#endif /* LCDMODE_LTDC_L8 */

#endif /* LCDMODE_LTDC */

/* ���������� ������������� ������� ������ ������ */
void 
display_fillrect(
	PACKEDCOLOR_T * buffer,
	uint_fast16_t dx,	// ������� ������
	uint_fast16_t dy,
	uint_fast16_t col,	// ������� ���� � ������,
	uint_fast16_t row,
	uint_fast16_t w,	// ������ ����
	uint_fast16_t h,
	COLOR_T color
	)
{
#if LCDMODE_LTDC
#if defined (DMA2D)

	// just writes the color defined in the DMA2D_OCOLR register 
	// to the area located at the address pointed by the DMA2D_OMAR 
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * dx * dy);

	/* ������� ����� */
	DMA2D->OMAR = (uintptr_t) & buffer [row * dx + col];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((dx - w) << DMA2D_OOR_LO_Pos) |
		0;

	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(h << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;

	DMA2D->OCOLR = 
		color |
		0;

	DMA2D->OPFCCR = (DMA2D->OPFCCR & ~ (DMA2D_OPFCCR_CM)) |
		(2 * DMA2D_FGPFCCR_CM_VALUE) |	/* framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		1 * DMA2D_AMTCR_EN |
		0;

	/* ��������� �������� */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ������� ���������� �������� */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;


#else /* defined (DMA2D)*/

	const unsigned t = dx - w;
	buffer += (dx * row) + col;
	while (h --)
	{
		PACKEDCOLOR_T * startmem = buffer;

		unsigned n = w;
		while (n --)
			* buffer ++ = color;
		buffer += t;
		arm_hardware_flush((uintptr_t) startmem, sizeof (* startmem) * w);
	}

#endif /* defined (DMA2D) */
#endif /* LCDMODE_LTDC */
}

/* ���������� ������������� ������� ������ ������ */
void 
dma2d_fillrect2_RGB565(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	// ������� ������
	uint_fast16_t dy,
	uint_fast16_t col,	// ������� ���� � ������,
	uint_fast16_t row,
	uint_fast16_t w,	// ������ ����
	uint_fast16_t h,
	COLOR565_T color
	)
{
#if LCDMODE_LTDC
#if defined (DMA2D)

	// just writes the color defined in the DMA2D_OCOLR register 
	// to the area located at the address pointed by the DMA2D_OMAR 
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

	/* ������� ����� */
	DMA2D->OMAR = (uintptr_t) & buffer [row * dx + col];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((dx - w) << DMA2D_OOR_LO_Pos) |
		0;

	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(h << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;

	DMA2D->OCOLR = 
		color |
		0;

	DMA2D->OPFCCR = (DMA2D->OPFCCR & ~ (DMA2D_OPFCCR_CM)) |
		(2 * DMA2D_FGPFCCR_CM_0) |	/* 010: RGB565 Color mode - framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		1 * DMA2D_AMTCR_EN |
		0;

	/* ��������� �������� */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ������� ���������� �������� */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

#else /* defined (DMA2D) */

	const unsigned t = dx - w;
	buffer += (dx * row) + col;
	while (h --)
	{
		PACKEDCOLOR565_T * startmem = buffer;

		unsigned n = w;
		while (n --)
			* buffer ++ = color;
		buffer += t;
		arm_hardware_flush((uintptr_t) startmem, sizeof (* startmem) * w);
	}



#endif /* defined (DMA2D) */
#endif /* LCDMODE_LTDC */
}


/* ���������� �������������� ������ ������ */
static void 
dma2d_fillrect(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	COLOR565_T color
	)
{
	dma2d_fillrect2_RGB565(buffer, dx, dy, 0, 0, dx, dy, color);
}

#if LCDMODE_COLORED
static COLOR_T bgcolor;
#endif /* LCDMODE_COLORED */

void 
display_setbgcolor(COLOR_T c)
{
#if LCDMODE_COLORED
	bgcolor = c;
#endif /* LCDMODE_COLORED */
}

COLOR_T 
display_getbgcolor(void)
{
#if LCDMODE_COLORED
	return bgcolor;
#else /* LCDMODE_COLORED */
	return COLOR_BLACK;
#endif /* LCDMODE_COLORED */
}



// ������������ ��� ������ �� ����������� ���������,
void
display_string(const char * s, uint_fast8_t lowhalf)
{
	char c;

	display_wrdata_begin();
	while((c = * s ++) != '\0') 
		display_put_char_small(c, lowhalf);
	display_wrdata_end();
}

// ������������ ��� ������ �� ����������� ���������,
void
display_string_P(const FLASHMEM  char * s, uint_fast8_t lowhalf)
{
	char c;

	display_wrdata_begin();
	while((c = * s ++) != '\0') 
		display_put_char_small(c, lowhalf);
	display_wrdata_end();
}

// ������������ ��� ������ �� ����������� ���������,
// ����� ��������� �����
void
display_string2(const char * s, uint_fast8_t lowhalf)
{
	char c;

	display_wrdata2_begin();
	while((c = * s ++) != '\0') 
		display_put_char_small2(c, lowhalf);
	display_wrdata2_end();
}



// ������������ ��� ������ �� ����������� ���������,
// ����� ��������� �����
void
display_string2_P(const FLASHMEM  char * s, uint_fast8_t lowhalf)
{
	char c;

	display_wrdata2_begin();
	while((c = * s ++) != '\0') 
		display_put_char_small2(c, lowhalf);
	display_wrdata2_end();
}


// ������ ������ �� ��� � ��������� ����� ������.
void 
//NOINLINEAT
display_at(uint_fast8_t x, uint_fast8_t y, const char * s)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{

		display_gotoxy(x, y + lowhalf);
		display_string(s, lowhalf);

	} while (lowhalf --);
}

// ������ ������ �� ��� � ��������� ����� ������.
void 
//NOINLINEAT
display_at_P(uint_fast8_t x, uint_fast8_t y, const FLASHMEM char * s)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{

		display_gotoxy(x, y + lowhalf);
		display_string_P(s, lowhalf);

	} while (lowhalf --);
}

/* ������ �� ������� ����������� ����� � ��������� dx * dy ����� */
void display_showbuffer(
	const GX_t * buffer,
	unsigned dx,	// �������
	unsigned dy,	// �������
	uint_fast8_t col,	// �����
	uint_fast8_t row	// �����
	)
{
#if LCDMODE_S1D13781

	s1d13781_showbuffer(buffer, dx, dy, col, row);

#else /* LCDMODE_S1D13781 */

	#if WITHSPIHWDMA && (LCDMODE_UC1608 | 0)
		// �� LCDMODE_S1D13781 ������-�� DMA ������� ����������
		arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * MGSIZE(dx, dy));	// ���������� ������
	#endif

	uint_fast8_t lowhalf = (dy) / 8 - 1;
	if (lowhalf == 0)
		return;
	do
	{
		uint_fast8_t pos;
		const GX_t * const p = buffer + lowhalf * dx;	// ������ ������ �������������� ������ � ������
		//debug_printf_P(PSTR("display_showbuffer: col=%d, row=%d, lowhalf=%d\n"), col, row, lowhalf);
		display_plotfrom(GRID2X(col), GRID2Y(row) + lowhalf * 8);		// ������ � ������ ������ ������
		// ������ �������������� ������
		display_wrdatabar_begin();
	#if WITHSPIHWDMA && (0)
		// �� LCDMODE_S1D13781 ������-�� DMA ������� ����������
		// �� LCDMODE_UC1608 ������ ��������� ������ ������� � ������ ������ (STM32F746xx)
		hardware_spi_master_send_frame(p, dx);
	#else
		for (pos = 0; pos < dx; ++ pos)
			display_barcolumn(p [pos]);	// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
	#endif
		display_wrdatabar_end();
	} while (lowhalf --);

#endif /* LCDMODE_S1D13781 */
}

#if LCDMODE_S1D13781

	// ������� ��� �����
	static const uint_fast16_t mapcolumn [16] =
	{
		0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, // ���� ��� ����������� � ������������
		0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000,
	};

#elif LCDMODE_UC1608 || LCDMODE_UC1601

	/* ������� ���� ������������� ������� �������� ����������� */
	// ������ ��� ���� � ������
	static const uint_fast8_t mapcolumn [8] =
	{
		0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, // ���� ��� ����������� � ������������
	};
#else /* LCDMODE_UC1608 || LCDMODE_UC1601 */

	/* ������� ���� ������������� ������� �������� ����������� */
	// ������ ��� ���� � ������
	static const uint_fast8_t mapcolumn [8] =
	{
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, // ���� ��� ����������� � ������������
	};

#endif /* LCDMODE_UC1608 || LCDMODE_UC1601 */

// ��������� ������������� ������
void display_colorbuffer_fill(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	COLOR565_T color
	)
{
#if defined (DMA2D) && LCDMODE_LTDC

	dma2d_fillrect(buffer, dx, dy, color);

#else /* defined (DMA2D) && LCDMODE_LTDC */

	uint_fast32_t len = (uint_fast32_t) dx * dy;
	if (sizeof (* buffer) == 1)
	{
		memset(buffer, color, len);
	}
	else if ((len & 0x07) == 0)
	{
		len /= 8;
		while (len --)
		{
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
		}
	}
	else if ((len & 0x03) == 0)
	{
		len /= 4;
		while (len --)
		{
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
		}
	}
	else
	{
		while (len --)
			* buffer ++ = color;
	}

#endif /* defined (DMA2D) && LCDMODE_LTDC && ! LCDMODE_LTDC_L8 */
}

// ��������� ������� �����.
void display_colorbuffer_set(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// �������������� ���������� ������� (0..dx-1) ����� �������
	uint_fast16_t row,	// ������������ ���������� ������� (0..dy-1) ������ ����
	COLOR565_T color
	)
{
	ASSERT(col < dx);
	ASSERT(row < dy);
#if LCDMODE_HORFILL
	// ������ ������� ����������� ���������� �������������� ���������� �������
	buffer [dx * row + col] = color;
#else /* LCDMODE_HORFILL */
	// ������ ������� ����������� ���������� ������������ ���������� �������
	buffer [dy * col + row] = color;
#endif /* LCDMODE_HORFILL */
}

// ��������� ������� �����.
void display_colorbuffer_xor(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// �������������� ���������� ������� (0..dx-1) ����� �������
	uint_fast16_t row,	// ������������ ���������� ������� (0..dy-1) ������ ����
	COLOR565_T color
	)
{
	ASSERT(col < dx);
	ASSERT(row < dy);
#if LCDMODE_HORFILL
	// ������ ������� ����������� ���������� �������������� ���������� �������
	buffer [dx * row + col] ^= color;
#else /* LCDMODE_HORFILL */
	// ������ ������� ����������� ���������� ������������ ���������� �������
	buffer [dy * col + row] ^= color;
#endif /* LCDMODE_HORFILL */
}

#if LCDMODE_LTDC_PIP16
// ���������� ������ ����� ��� ������� ��� PIP
void display_colorbuffer_pip(
	const PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy
	)
{
	arm_hardware_flush((uintptr_t) buffer, (uint_fast32_t) dx * dy * sizeof * buffer);
	arm_hardware_ltdc_pip_set((uintptr_t) buffer);
}

#else /* LCDMODE_LTDC_PIP16 */

// ������ ����� �� �������
void display_colorbuffer_show(
	const PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// �������������� ���������� ������ �������� ���� �� ������ (0..dx-1) ����� �������
	uint_fast16_t row	// ������������ ���������� ������ �������� ���� �� ������ (0..dy-1) ������ ����
	)
{
#if defined (DMA2D) && LCDMODE_LTDC

	arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

#if LCDMODE_HORFILL

	/* �������� ����� */
	DMA2D->FGMAR = (uintptr_t) buffer;
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		(0 << DMA2D_FGOR_LO_Pos) |
		0;
	/* ������� ����� */
	DMA2D->OMAR = (uintptr_t) & framebuff [row] [col];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((DIM_X - dx) << DMA2D_OOR_LO_Pos) |
		0;
	/* ������ ������������� ������ */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(dy << DMA2D_NLR_NL_Pos) |
		(dx << DMA2D_NLR_PL_Pos) |
		0;
	/* ������ ������� */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM)) |
		DMA2D_FGPFCCR_CM_VALUE |	/* Color mode - framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		1 * DMA2D_AMTCR_EN |
		0;

	/* ��������� �������� */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ������� ���������� �������� */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

#else /* LCDMODE_HORFILL */

#endif /* LCDMODE_HORFILL */

#else /* defined (DMA2D) && LCDMODE_LTDC */
	#if LCDMODE_COLORED
		display_plotfrom(col, row);
		display_plotstart(dy);
		display_plot(buffer, dx, dy);
		display_plotstop();
	#endif
#endif /* defined (DMA2D) && LCDMODE_LTDC */
}
#endif /* LCDMODE_LTDC_PIP16 */

// �������� �����
void display_pixelbuffer(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// �������������� ���������� ������� (0..dx-1) ����� �������
	uint_fast16_t row	// ������������ ���������� ������� (0..dy-1) ������ ����
	)
{
#if LCDMODE_S1D13781

	//row = (dy - 1) - row;		// ����� �����������
	GX_t * const rowstart = buffer + row * ((dx + 15) / 16);	// ������ ������ ������ ������ � ������
	GX_t * const p = rowstart + col / 16;	
	//* p |= mapcolumn [col % 16];	// ���������� �����
	* p &= ~ mapcolumn [col % 16];	// �������� �����
	//* p ^= mapcolumn [col % 16];	// ������������� �����

#else /* LCDMODE_S1D13781 */

	//row = (dy - 1) - row;		// ����� �����������
	GX_t * const p = buffer + (row / 8) * dx + col;	// ������ ������ �������������� ������ � ������
	//* p |= mapcolumn [row % 8];	// ���������� �����
	* p &= ~ mapcolumn [row % 8];	// �������� �����
	//* p ^= mapcolumn [row % 8];	// ������������� �����

#endif /* LCDMODE_S1D13781 */
}

/* ����������� ��� � ������ � ������ */
void display_pixelbuffer_xor(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// �������������� ���������� ������� (0..dx-1) ����� �������
	uint_fast16_t row	// ������������ ���������� ������� (0..dy-1) ������ ����
	)
{
#if LCDMODE_S1D13781
	//row = (dy - 1) - row;		// ����� �����������
	GX_t * const rowstart = buffer + row * ((dx + 15) / 16);	// ������ ������ ������ ������ � ������
	GX_t * const p = rowstart + col / 16;	
	//* p |= mapcolumn [col % 16];	// ���������� �����
	//* p &= ~ mapcolumn [col % 16];	// �������� �����
	* p ^= mapcolumn [col % 16];	// ������������� �����

#else /* LCDMODE_S1D13781 */

	//row = (dy - 1) - row;		// ����� �����������
	GX_t * const p = buffer + (row / 8) * dx + col;	// ������ ������ �������������� ������ � ������
	//* p |= mapcolumn [row % 8];	// ���������� �����
	//* p &= ~ mapcolumn [row % 8];	// �������� �����
	* p ^= mapcolumn [row % 8];	// ������������� �����

#endif /* LCDMODE_S1D13781 */
}

#if LCDMODE_LTDC

static void 
bitblt_fill(
	uint_fast16_t x, uint_fast16_t y, 	// ���������� � ��������
	uint_fast16_t w, uint_fast16_t h, 	// ������� � ��������
	COLOR_T color)
{

#if defined (DMA2D) && LCDMODE_LTDC && ! LCDMODE_LTDC_L8

	dma2d_fillrect2_RGB565(& framebuff [0] [0], DIM_X, DIM_Y, x, y, w, h, color);

#else /* defined (DMA2D) && LCDMODE_LTDC && ! LCDMODE_LTDC_L8 */

	display_fillrect(& framebuff [0] [0], DIM_X, DIM_Y, x, y, w, h, color);

#endif /* defined (DMA2D) && LCDMODE_LTDC && ! LCDMODE_LTDC_L8 */
}

void display_solidbar(uint_fast16_t x, uint_fast16_t y, uint_fast16_t x2, uint_fast16_t y2, COLOR_T color)
{
	if (x2 < x)
	{
		uint_fast16_t t = x;
		x = x2, x2 = t;
	}
	if (y2 < y)
	{
		uint_fast16_t t = y;
		y = y2, y2 = t;
	}
	bitblt_fill(x, y, x2 - x, y2 - y, color);
}

#endif /* LCDMODE_LTDC */


static int local_randomgr( int num )
{

	static unsigned long rand_val = 123456UL;

	if (rand_val & 0x80000000UL)
		rand_val = (rand_val << 1);
	else	rand_val = (rand_val << 1) ^0x201051UL;

	return (rand_val % num);

}


/*                                                                      */
/*      RANDOMBARS: Display local_randomgr bars                                 */
/*                                                                      */

void display_barstest(void)
{
	unsigned n = 20000;
	for (;n --;)
	{                    /* Until user enters a key...   */
		int r = local_randomgr(255);
		int g = local_randomgr(255);
		int b = local_randomgr(255);

		const COLOR_T color = TFTRGB(r, g, b);

		int x = local_randomgr(DIM_X - 1);
		int y = local_randomgr(DIM_Y - 1);
		int x2 = local_randomgr(DIM_X - 1);
		int y2 = local_randomgr(DIM_Y - 1);

		display_solidbar(x, y, x2, y2, color);
		//_delay_ms(10);
	}

	//getch();             /* Pause for user's response    */
}

/*
 * ��������� ������ ��� ����������� ������ � ��������������� (����� ����������������� ������)
 * ��������� �������.
 */
/* ���������� ��� ����������� �����������. */
void display_hardware_initialize(void)
{
	debug_printf_P(PSTR("display_hardware_initialize start\n"));

#if LCDMODE_LTDC 
	#if LCDMODE_LTDCSDRAMBUFF
		arm_hardware_sdram_initialize();
	#endif /* LCDMODE_LTDCSDRAMBUFF */
	// STM32F4xxx with LCD-TFT Controller (LTDC)
	arm_hardware_ltdc_initialize();
#endif /* LCDMODE_LTDC */

#if LCDMODE_HARD_SPI
#elif LCDMODE_HARD_I2C
#elif LCDMODE_LTDC
#else
	#if LCDMODE_HD44780 && (LCDMODE_SPI == 0)
		hd44780_io_initialize();
	#else /* LCDMODE_HD44780 && (LCDMODE_SPI == 0) */
		DISPLAY_BUS_INITIALIZE();	// see LCD_CONTROL_INITIALIZE, LCD_DATA_INITIALIZE_WRITE
	#endif /* LCDMODE_HD44780 && (LCDMODE_SPI == 0) */
#endif
	debug_printf_P(PSTR("display_hardware_initialize done\n"));
}

#if LCDMODE_LTDC

#if DSTYLE_G_X320_Y240
	// � ��������������� ����������� �������� "�� ����������"
	// ��� �������� 320 * 240
	#include "./fonts/ILI9341_font_small.c"
	#include "./fonts/ILI9341_font_half.c"
	#include "./fonts/ILI9341_font_big.c"

	#define	ls020_smallfont	ILI9341_smallfont
	#define	ls020_halffont	ILI9341_halffont
	#define	ls020_bigfont	ILI9341_bigfont

#elif DSTYLE_G_X480_Y272
	// � ��������������� ����������� �������� "�� �����������"
	#include "./fonts/S1D13781_font_small_LTDC.c"
	#include "./fonts/S1D13781_font_half_LTDC.c"
	#include "./fonts/S1D13781_font_big_LTDC.c"

#elif DSTYLE_G_X800_Y480
	// � ��������������� ����������� �������� "�� �����������"
	#include "./fonts/S1D13781_font_small_LTDC.c"
	#include "./fonts/S1D13781_font_half_LTDC.c"
	#include "./fonts/S1D13781_font_big_LTDC.c"

#else /*  */
	// � ��������������� ����������� �������� "�� ����������"
	//#error Undefined display layout

	#include "./fonts/ls020_font_small.c"
	#include "./fonts/ls020_font_half.c"
	#include "./fonts/ls020_font_big.c"
#endif /* DSTYLE_G_X320_Y240 */


#if ! LCDMODE_LTDC_L24
#include "./byte2crun.h"
#endif /* ! LCDMODE_LTDC_L24 */


/* ������� � ������, ���� ����� ���������� ��������� ������� */
static PACKEDCOLOR_T ltdc_fg, ltdc_bg;

#if ! LCDMODE_LTDC_L24
static const FLASHMEM PACKEDCOLOR_T (* byte2run) [256][8] = & byte2run_COLOR_WHITE_COLOR_BLACK;
#endif /* ! LCDMODE_LTDC_L24 */

static unsigned ltdc_first, ltdc_second;	// � ��������
static unsigned ltdc_h;						// ������ ������� (������) � ��������
static unsigned ltdc_secondoffs;			// � ��������

void display_setcolors(COLOR_T fg, COLOR_T bg)
{

#if ! LCDMODE_LTDC_L24
	ltdc_fg = fg;
	ltdc_bg = bg;
#else /* ! LCDMODE_LTDC_L24 */

	ltdc_fg.r = fg >> 16;
	ltdc_fg.g = fg >> 8;
	ltdc_fg.b = fg >> 0;
	ltdc_bg.r = bg >> 16;
	ltdc_bg.g = bg >> 8;
	ltdc_bg.b = bg >> 0;

#endif /* ! LCDMODE_LTDC_L24 */

#if ! LCDMODE_LTDC_L24

	if (fg == COLOR_WHITE && bg == COLOR_DARKGREEN)
		byte2run = & byte2run_COLOR_WHITE_COLOR_DARKGREEN;
	else if (fg == COLOR_YELLOW && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_YELLOW_COLOR_BLACK;
	else if (fg == COLOR_WHITE && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_WHITE_COLOR_BLACK;
	else if (fg == COLOR_BLACK && bg == COLOR_GREEN)
		byte2run = & byte2run_COLOR_BLACK_COLOR_GREEN;
	else if (fg == COLOR_BLACK && bg == COLOR_RED)
		byte2run = & byte2run_COLOR_BLACK_COLOR_RED;
	else if (fg == COLOR_GREEN && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_GREEN_COLOR_BLACK;
	else if (fg == COLOR_RED && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_RED_COLOR_BLACK;
	else
		byte2run = & byte2run_COLOR_WHITE_COLOR_BLACK;

#endif /* ! LCDMODE_LTDC_L24 */

}

void display_setcolors3(COLOR_T fg, COLOR_T bg, COLOR_T fgbg)
{
	display_setcolors(fg, bg);
}

// ������ ���� ������� �������
static void 
ltdc_pix1color(
	uint_fast8_t cgcol,		// �������� � �������� ������������ ���������, ������������ display_gotoxy
	uint_fast8_t cgrow,
	PACKEDCOLOR_T color
	)
{
	PACKEDCOLOR_T * const p = & framebuff [ltdc_first + cgrow] [ltdc_second + ltdc_secondoffs + cgcol];
	// ��������� ������� �� �����������
	//debug_printf_P(PSTR("framebuff=%p, ltdc_first=%d, cgrow=%d, ltdc_second=%d, ltdc_secondoffs=%d, cgcol=%d\n"), framebuff, ltdc_first, cgrow, ltdc_second, ltdc_secondoffs, cgcol);
	* p = color;
	arm_hardware_flush((uintptr_t) p, sizeof * p);
}


// ������ ���� ������� ������� (���/������)
static void 
ltdc_pixel(
	uint_fast8_t cgcol,		// �������� � �������� ������������ ���������, ������������ display_gotoxy
	uint_fast8_t cgrow,
	uint_fast8_t v			// 0 - ���� background, ����� - foreground
	)
{
	ltdc_pix1color(cgcol, cgrow, v ? ltdc_fg : ltdc_bg);
}


// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
static void 
ltdc_vertical_pixN(
	uint_fast8_t v,		// pattern
	uint_fast8_t w		// number of lower bits used in pattern
	)
{

#if LCDMODE_LTDC_L24 || LCDMODE_HORFILL

	// TODO: ��� ��������� ���� ��� ������ ���, ��������� ������ ����� ���.
	ltdc_pixel(0, 0, v & 0x01);
	ltdc_pixel(0, 1, v & 0x02);
	ltdc_pixel(0, 2, v & 0x04);
	ltdc_pixel(0, 3, v & 0x08);
	ltdc_pixel(0, 4, v & 0x10);
	ltdc_pixel(0, 5, v & 0x20);
	ltdc_pixel(0, 6, v & 0x40);
	ltdc_pixel(0, 7, v & 0x80);

	++ ltdc_secondoffs;

#else /* LCDMODE_LTDC_L24 */
	// ��������� ������� �� �����������
	// TODO: ��� ��������� ���� ��� ������ ���, ��������� ������ ����� ���.
	const FLASHMEM PACKEDCOLOR_T * const pcl = (* byte2run) [v];
	memcpy(& framebuff [ltdc_first] [ltdc_second + ltdc_secondoffs], pcl, sizeof (* pcl) * w);
	arm_hardware_flush((uintptr_t) & framebuff [ltdc_first] [ltdc_second + ltdc_secondoffs], sizeof (PACKEDCOLOR_T) * w);
	if ((ltdc_secondoffs += 8) >= ltdc_h)
	{
		ltdc_secondoffs -= ltdc_h;
		++ ltdc_first;
	}
#endif /* LCDMODE_LTDC_L24 */
}


static uint_fast8_t
//NOINLINEAT
bigfont_decode(uint_fast8_t c)
{
	// '#' - ����� ������
	if (c == ' ' || c == '#')
		return 11;
	if (c == '_')
		return 10;		// ������ - ������� ������������� �������
	if (c == '.')
		return 12;		// �����
	return c - '0';		// ��������� - ����� 0..9
}


static uint_fast8_t
smallfont_decode(uint_fast8_t c)
{
	return c - ' ';
}

#if LCDMODE_HORFILL
// ��� ������ ����� �������������� ������� � ����������� ������������� ������

static void
ltdc_horizontal_pixels(
	uint_fast8_t cgrow,
	const FLASHMEM uint8_t * raster,
	uint_fast8_t width	// number of bits (start from LSB first byte in raster)
	)
{
	uint_fast8_t col;
	uint_fast8_t w = width;
	PACKEDCOLOR_T * const t = & framebuff [ltdc_first + cgrow] [ltdc_second];

	for (col = 0; w >= 8; col += 8, w -= 8)
	{
		const FLASHMEM PACKEDCOLOR_T * const pcl = (* byte2run) [* raster ++];
		memcpy(t + col, pcl, sizeof (* t) * 8);
	}
	if (w != 0)
	{
		const FLASHMEM PACKEDCOLOR_T * const pcl = (* byte2run) [* raster ++];
		memcpy(t + col, pcl, sizeof (* t) * w);
	}
	arm_hardware_flush((uintptr_t) t, sizeof (* t) * width);
}

// ����� ���� ������� ������ ������ display_wrdata_begin() � 	display_wrdata_end();
static void ltdc_horizontal_put_char_small(char cc)
{
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		ltdc_horizontal_pixels(cgrow, S1D13781_smallfont_LTDC [c] [cgrow], width);
	}
	ltdc_second += width;
}

// ����� ���� ������� ������ ������ display_wrdatabig_begin() � display_wrdatabig_end();
static void ltdc_horizontal_put_char_big(char cc)
{
	const uint_fast8_t width = ((cc == '.' || cc == '#') ? BIGCHARW_NARROW  : BIGCHARW);	// ������������ ������� � ��������
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < BIGCHARH; ++ cgrow)
	{
		ltdc_horizontal_pixels(cgrow, S1D13781_bigfont_LTDC [c] [cgrow], width);
	}
	ltdc_second += width;
}

// ����� ���� ������� ������ ������ display_wrdatabig_begin() � display_wrdatabig_end();
static void ltdc_horizontal_put_char_half(char cc)
{
	const uint_fast8_t width = HALFCHARW;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < HALFCHARH; ++ cgrow)
	{
		ltdc_horizontal_pixels(cgrow, S1D13781_halffont_LTDC [c] [cgrow], width);
	}
	ltdc_second += width;
}

#else /* LCDMODE_HORFILL */

// ����� ���� ������� ������ ������ display_wrdata_begin() � 	display_wrdata_end();
static void ltdc_vertical_put_char_small(char cc)
{
	uint_fast8_t i = 0;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_smallfont [0] / sizeof ls020_smallfont [0] [0]) };
	const FLASHMEM uint8_t * const p = & ls020_smallfont [c] [0];
	
	for (; i < NBYTES; ++ i)
		ltdc_vertical_pixN(p [i], 8);	// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
}

// ����� ���� ������� ������ ������ display_wrdatabig_begin() � display_wrdatabig_end();
static void ltdc_vertical_put_char_big(char cc)
{
	// '#' - ����� ������
	enum { NBV = (BIGCHARH / 8) }; // ������� ������ � ����� ���������
	uint_fast8_t i = NBV * ((cc == '.' || cc == '#') ? 12 : 0);	// ��������� ������� ���������������, ������ ��������.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_bigfont [0] / sizeof ls020_bigfont [0] [0]) };
	const FLASHMEM uint8_t * const p = & ls020_bigfont [c] [0];
	
	for (; i < NBYTES; ++ i)
		ltdc_vertical_pixN(p [i], 8);	// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
}

// ����� ���� ������� ������ ������ display_wrdatabig_begin() � display_wrdatabig_end();
static void ltdc_vertical_put_char_half(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_halffont [0] / sizeof ls020_halffont [0] [0]) };
	const FLASHMEM uint8_t * const p = & ls020_halffont [c] [0];
	
	for (; i < NBYTES; ++ i)
		ltdc_vertical_pixN(p [i], 8);	// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
}

#endif /* LCDMODE_HORFILL */



/* ����������� ����������� ���� � ����������� ��� �������� */
void
display_scroll_down(
	uint_fast16_t x0,	// ����� ������� ���� ����
	uint_fast16_t y0,	// ����� ������� ���� ����
	uint_fast16_t w, 	// �� 65535 �������� - ������ ����
	uint_fast16_t h, 	// �� 65535 �������� - ������ ����
	uint_fast16_t n,	// ���������� ����� ���������
	int_fast16_t hshift	// ���������� ��������� ��� ������ ����� (������������� �����) ��� ������ (�������������).
	)
{
#if defined (DMA2D) && LCDMODE_LTDC

#if LCDMODE_HORFILL

	/* TODO: � DMA2D ��� ������� ���������� ������������ ���������, ������ ������ ��� �������� ��� �� ���� ������ (����������) */
	/* �������� ����� */
	DMA2D->FGMAR = (uintptr_t) & framebuff [y0 + 0] [x0];
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((DIM_X - w) << DMA2D_FGOR_LO_Pos) |
		0;
	/* ������� ����� */
	DMA2D->OMAR = (uintptr_t) & framebuff [y0 + n] [x0];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((DIM_X - w) << DMA2D_OOR_LO_Pos) |
		0;
	/* ������ ������������� ������ */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		((h - n) << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;
	/* ������ ������� */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM)) |
		DMA2D_FGPFCCR_CM_VALUE |	/* Color mode - framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		1 * DMA2D_AMTCR_EN |
		0;

	/* ��������� �������� */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ������� ���������� �������� */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

#else /* LCDMODE_HORFILL */
#endif /* LCDMODE_HORFILL */

#endif /* defined (DMA2D) && LCDMODE_LTDC */
}

uint_fast8_t
display_getreadystate(void)
{
#if defined (DMA2D) && LCDMODE_LTDC
	return (DMA2D->CR & DMA2D_CR_START) == 0;
#else /*defined (DMA2D) && LCDMODE_LTDC */
	return 1;
#endif /* defined (DMA2D) && LCDMODE_LTDC */
}

/* ����������� ����������� ���� � ����������� ��� �������� */
void
display_scroll_up(
	uint_fast16_t x0,	// ����� ������� ���� ����
	uint_fast16_t y0,	// ����� ������� ���� ����
	uint_fast16_t w, 	// �� 65535 �������� - ������ ����
	uint_fast16_t h, 	// �� 65535 �������� - ������ ����
	uint_fast16_t n,	// ���������� ����� ���������
	int_fast16_t hshift	// ���������� ��������� ��� ������ ����� (������������� �����) ��� ������ (�������������).
	)
{
#if defined (DMA2D) && LCDMODE_LTDC
#if LCDMODE_HORFILL

	/* �������� ����� */
	DMA2D->FGMAR = (uintptr_t) & framebuff [y0 + n] [x0];
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((DIM_X - w) << DMA2D_FGOR_LO_Pos) |
		0;
	/* ������� ����� */
	DMA2D->OMAR = (uintptr_t) & framebuff [y0 + 0] [x0];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((DIM_X - w) << DMA2D_OOR_LO_Pos) |
		0;
	/* ������ ������������� ������ */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		((h - n) << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;
	/* ������ ������� */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM)) |
		DMA2D_FGPFCCR_CM_VALUE |	/* Color mode - framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		1 * DMA2D_AMTCR_EN |
		0;

	/* ��������� �������� */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ������� ���������� �������� */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

#else /* LCDMODE_HORFILL */
#endif /* LCDMODE_HORFILL */
#endif /* defined (DMA2D) && LCDMODE_LTDC */
}


/* �������������� ������� �������� ������� - ����������� � ��������������� �� ������ */
void display_clear(void)
{
	const COLOR_T bg = display_getbgcolor();

#if defined (DMA2D) && LCDMODE_LTDC && ! LCDMODE_LTDC_L8

	dma2d_fillrect(& framebuff [0] [0], DIM_X, DIM_Y, bg);

#elif LCDMODE_LTDC_L8

	memset(framebuff, bg, DIM_FIRST * DIM_SECOND);
	//memset(framebuff, COLOR_WHITE, DIM_FIRST * DIM_SECOND);	// debug version
	arm_hardware_flush((uintptr_t) framebuff, sizeof framebuff);

#elif LCDMODE_LTDC_L24

	unsigned i, j;
	// fill
	PACKEDCOLOR_T c;
	c.r = bg >> 16;
	c.g = bg >> 8;
	c.b = bg >> 0;
	for (i = 0; i < DIM_FIRST; ++ i)
	{
		for (j = 0; j < DIM_SECOND; ++ j)
			framebuff [i][j] = c;
	}

	arm_hardware_flush((uintptr_t) framebuff, sizeof framebuff);

#else /* LCDMODE_LTDC_L8 */

	unsigned i, j;
	// fill
	for (i = 0; i < DIM_FIRST; ++ i)
	{
		for (j = 0; j < DIM_SECOND; ++ j)
			framebuff [i][j] = bg;
	}

	arm_hardware_flush((uintptr_t) framebuff, sizeof framebuff);

#endif /* LCDMODE_LTDC_L8 */
}

void display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
#if LCDMODE_HORFILL
	ltdc_second = GRID2X(x);
	ltdc_first = GRID2Y(y);
#else /* LCDMODE_HORFILL */
	ltdc_first = GRID2X(x);
	ltdc_second = GRID2Y(y);
#endif /* LCDMODE_HORFILL */

	//debug_printf_P(PSTR("display_gotoxy: CHAR_H=%d, CHAR_W=%d, x=%d, y=%d, ltdc_first=%d, ltdc_second=%d\n"), CHAR_H, CHAR_W, x, y, ltdc_first, ltdc_second);
	ASSERT(ltdc_first < DIM_FIRST);
	ASSERT(ltdc_second < DIM_SECOND);
}

// ���������� � ��������
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
#if LCDMODE_ILI8961 || LCDMODE_LQ043T3DX02K
	ltdc_second = x;
	ltdc_first = y;
#else
	ltdc_first = x;
	ltdc_second = y;
#endif
	//debug_printf_P(PSTR("display_gotoxy: CHAR_H=%d, CHAR_W=%d, x=%d, y=%d, ltdc_first=%d, ltdc_second=%d\n"), CHAR_H, CHAR_W, x, y, ltdc_first, ltdc_second);
	ASSERT(ltdc_first < DIM_FIRST);
	ASSERT(ltdc_second < DIM_SECOND);
}

void display_plotstart(
	uint_fast16_t height	// ������ ���� � ��������
	)
{

}

void display_plot(
	const PACKEDCOLOR_T * buffer, 
	uint_fast16_t dx,	// ������� ���� � ��������
	uint_fast16_t dy
	)
{
#if LCDMODE_HORFILL
	const size_t len = dx * sizeof * buffer;
	while (dy --)
	{
		PACKEDCOLOR_T * const p = & framebuff [ltdc_first] [ltdc_second];
		memcpy(p, buffer, len);
		arm_hardware_flush((uintptr_t) p, len);
		buffer += dx;

		++ ltdc_first;
	}
#else /* LCDMODE_HORFILL */
	const size_t len = dy * sizeof * buffer;
	while (dx --)
	{
		PACKEDCOLOR_T * const p = & framebuff [ltdc_first] [ltdc_second];
		memcpy(p, buffer, len);
		arm_hardware_flush((uintptr_t) p, len);
		buffer += dy;

		++ ltdc_first;
	}
#endif /* LCDMODE_HORFILL */
}

void display_plotstop(void)
{

}

// ����� ��������� �����
void display_wrdata2_begin(void)
{
	ltdc_secondoffs = 0;
	ltdc_h = SMALLCHARH;
}

void display_wrdata2_end(void)
{
}

void display_put_char_small2(uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	ltdc_horizontal_put_char_small(c);
#else /* LCDMODE_HORFILL */
	ltdc_vertical_put_char_small(c);
#endif /* LCDMODE_HORFILL */
}

// ������ ����������
void display_wrdatabar_begin(void)
{
	ltdc_secondoffs = 0;
	ltdc_h = 8;
}

// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
void display_barcolumn(uint_fast8_t pattern)
{
	ltdc_vertical_pixN(pattern, 8);	// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
}

void display_wrdatabar_end(void)
{
}

// ������� � ������� ����� (�������)
void display_wrdatabig_begin(void)
{
	ltdc_secondoffs = 0;
	ltdc_h = BIGCHARH;
}

void display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	ltdc_horizontal_put_char_big(c);
#else /* LCDMODE_HORFILL */
	ltdc_vertical_put_char_big(c);
#endif /* LCDMODE_HORFILL */
}

void display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	ltdc_horizontal_put_char_half(c);
#else /* LCDMODE_HORFILL */
	ltdc_vertical_put_char_half(c);
#endif /* LCDMODE_HORFILL */
}

void display_wrdatabig_end(void)
{
}

// ������� �����
void display_wrdata_begin(void)
{
	ltdc_secondoffs = 0;
	ltdc_h = SMALLCHARH;
}

void display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	ltdc_horizontal_put_char_small(c);
#else /* LCDMODE_HORFILL */
	ltdc_vertical_put_char_small(c);
#endif /* LCDMODE_HORFILL */
}

void display_wrdata_end(void)
{
}

#if LCDMODE_LQ043T3DX02K || LCDMODE_AT070TN90 || LCDMODE_AT070TNA2

// ��������

/* ���������� ����� ������� - ����� ������������� */
/* ���������� ��� ����������� �����������. */
void
display_reset(void)
{
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(1); // Delay 1ms
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(10); // Delay 10ms
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(50); // Delay 50 ms
}
/* ���������� ��� ����������� �����������. */
void display_initialize(void)
{
}

void display_set_contrast(uint_fast8_t v)
{
}

/* ��������� ������������ ������� */
void display_discharge(void)
{
}

#endif /* LCDMODE_LQ043T3DX02K */
#endif /* LCDMODE_LTDC */
