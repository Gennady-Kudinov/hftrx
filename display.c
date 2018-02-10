/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ��������� ��� LS020 ������� �������, livas60@mail.ru
//

#include "hardware.h"

#include "display.h"
#include "formats.h"
#include <string.h>

#if LCDMODE_LTDC
	// ����� ������
	FRAMEBUFF_T framebuff0;	//L8 (8-bit Luminance or CLUT)
#endif /* LCDMODE_LTDC */

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

	display_showbufferXXX(buffer, dx, dy, col, row);

#else /* LCDMODE_S1D13781 */

	uint_fast8_t lowhalf = (dy + 7) / 8 - 1;
	do
	{
		uint_fast8_t pos;
		const unsigned char * const p = buffer + lowhalf * dx;	// ������ ������ �������������� ������ � ������
			
		display_gotoxy(col, row + lowhalf);		// ������ � ������ ������ ������
		// ������ �������������� ������
		display_wrdatabar_begin();
		for (pos = 0; pos < dx; ++ pos)
			display_barcolumn(p [pos]);	// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
		display_wrdatabar_end();
	} while (lowhalf --);

#endif /* LCDMODE_S1D13781 */
}

/* ��������� ����� � ������ ����� */
void display_pixelbuffer(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// �������������� ���������� ������� (0..dx-1) ����� �������
	uint_fast16_t row	// ������������ ���������� ������� (0..dy-1) ������ ����
	)
{
#if LCDMODE_S1D13781

	static const uint_fast16_t map [16] =
	{
		0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, // ���� ��� ����������� � ������������
		0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000,
	};
	//row = (dy - 1) - row;		// ����� �����������
	GX_t * const rowstart = buffer + row * ((dx + 15) / 16);	// ������ ������ ������ ������ � ������
	GX_t * const p = rowstart + col / 16;	
	//* p |= map [col % 16];	// ���������� �����
	* p &= ~ map [col % 16];	// �������� �����
	//* p ^= map [col % 16];	// ������������� �����

#else /* LCDMODE_S1D13781 */

	static const uint_fast8_t map [8] =
	{
		0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, // ���� ��� ����������� � ������������
	};
	//row = (dy - 1) - row;		// ����� �����������
	uint8_t * const p = buffer + (row / 8) * dx + col;	// ������ ������ �������������� ������ � ������
	//* p |= map [row % 8];	// ���������� �����
	* p &= ~ map [row % 8];	// �������� �����
	//* p ^= map [row % 8];	// ������������� �����

#endif /* LCDMODE_S1D13781 */
}

/* ��������� ����� � ������ ����� */
void display_pixelbuffer_xor(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// �������������� ���������� ������� (0..dx-1) ����� �������
	uint_fast16_t row	// ������������ ���������� ������� (0..dy-1) ������ ����
	)
{
#if LCDMODE_S1D13781

	static const uint_fast16_t map [16] =
	{
		0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, // ���� ��� ����������� � ������������
		0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000,
	};
	//row = (dy - 1) - row;		// ����� �����������
	GX_t * const rowstart = buffer + row * ((dx + 15) / 16);	// ������ ������ ������ ������ � ������
	GX_t * const p = rowstart + col / 16;	
	//* p |= map [col % 16];	// ���������� �����
	//* p &= ~ map [col % 16];	// �������� �����
	* p ^= map [col % 16];	// ������������� �����

#else /* LCDMODE_S1D13781 */

	static const uint_fast8_t map [8] =
	{
		0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, // ���� ��� ����������� � ������������
	};
	//row = (dy - 1) - row;		// ����� �����������
	uint8_t * const p = buffer + (row / 8) * dx + col;	// ������ ������ �������������� ������ � ������
	//* p |= map [row % 8];	// ���������� �����
	//* p &= ~ map [row % 8];	// �������� �����
	* p ^= map [row % 8];	// ������������� �����

#endif /* LCDMODE_S1D13781 */
}


/*
 * ��������� ������ ��� ����������� ������ � ��������������� (����� ����������������� ������)
 * ��������� �������.
 */
/* ���������� ��� ����������� �����������. */
void display_hardware_initialize(void)
{

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
	#include "./fonts/S1D13781_font_small.c"
	#include "./fonts/S1D13781_font_half.c"
	#include "./fonts/S1D13781_font_big.c"

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

static unsigned ltdc_first, ltdc_second;
static unsigned ltdc_h;
static unsigned ltdc_secondoffs;

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

	if (fg == COLOR_YELLOW && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_YELLOW_COLOR_BLACK;
	else if (fg == COLOR_BLACK && bg == COLOR_RED)
		byte2run = & byte2run_COLOR_BLACK_COLOR_RED;
	else if (fg == COLOR_BLACK && bg == COLOR_GREEN)
		byte2run = & byte2run_COLOR_BLACK_COLOR_GREEN;
	else if (fg == COLOR_RED && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_RED_COLOR_BLACK;
	else if (fg == COLOR_GREEN && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_GREEN_COLOR_BLACK;
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
ltdc_horizontal_pix1color(
	uint_fast8_t cgcol,
	uint_fast8_t cgrow,
	PACKEDCOLOR_T color
	)
{
	// ��������� ������� �� �����������
	framebuff [ltdc_first + cgrow] [ltdc_second + cgcol] = color;
}


// ������ ���� ������� ������� (���/������)
static void 
ltdc_horizontal_pix1(
	uint_fast8_t cgcol,
	uint_fast8_t cgrow,
	uint_fast8_t v
	)
{
	ltdc_horizontal_pix1color(cgcol, cgrow, v ? ltdc_fg : ltdc_bg);
}


// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
static void 
ltdc_vertical_pix8(
	uint_fast8_t v
	)
{
	const FLASHMEM PACKEDCOLOR_T * const pcl = (* byte2run) [v];

#if LCDMODE_LTDC_L24 || LCDMODE_LQ043T3DX02K

	ltdc_horizontal_pix1color(0, 0, pcl [0]);
	ltdc_horizontal_pix1color(0, 1, pcl [1]);
	ltdc_horizontal_pix1color(0, 2, pcl [2]);
	ltdc_horizontal_pix1color(0, 3, pcl [3]);
	ltdc_horizontal_pix1color(0, 4, pcl [4]);
	ltdc_horizontal_pix1color(0, 5, pcl [5]);
	ltdc_horizontal_pix1color(0, 6, pcl [6]);
	ltdc_horizontal_pix1color(0, 7, pcl [7]);

	++ ltdc_second;

#else /* LCDMODE_LTDC_L24 */
	// ��������� ������� �� �����������
	memcpy(& framebuff [ltdc_first] [ltdc_second + ltdc_secondoffs], pcl, sizeof (* byte2run) [v]);
	if ((ltdc_secondoffs += 8) >= ltdc_h)
	{
		ltdc_secondoffs -= ltdc_h;
		++ ltdc_first;
	}
#endif /* LCDMODE_LTDC_L24 */
}

// ������ ������ ������� ��������
static void 
//NOINLINEAT
ltdc_horizontal_pix8(
	uint_fast8_t cgcol,
	uint_fast8_t cgrow,
	uint_fast8_t v
	)
{
	// ��������� ������� �� �����������
	memcpy(& framebuff [ltdc_first + cgrow] [ltdc_second + cgcol], (* byte2run) [v], sizeof (* byte2run) [v]);
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
ascii_decode(uint_fast8_t c)
{
	return c - ' ';
}

#if LCDMODE_LQ043T3DX02K

// ����� ���� ������� ������ ������ display_wrdata_begin() � 	display_wrdata_end();
static void ltdc_horizontal_put_char_small(char cc)
{
	const uint_fast8_t c = ascii_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		uint_fast8_t cgcol;
		for (cgcol = 0; cgcol < 8 * (sizeof S1D13781_smallfont [0][0] / sizeof S1D13781_smallfont [0][0][0]); cgcol += 8)
		{
			const FLASHMEM uint8_t * p = & S1D13781_smallfont [c][cgrow] [cgcol / 8];
			ltdc_horizontal_pix8(cgcol, cgrow, * p);	// ��������� ���������, ���������
		}
	}
	ltdc_second += SMALLCHARW;
}

// NARROWCHARSTARTCOLUMN - ��������� ������� � ������� ��������������� (�������� ������� � ���)

// ����� ���� ������� ������ ������ display_wrdatabig_begin() � display_wrdatabig_end();
static void ltdc_horizontal_put_char_big(char cc)
{
	enum { NHALFS = (sizeof S1D13781_bigfont [0] / sizeof S1D13781_bigfont [0][0]) };
	const uint_fast8_t startcol = ((cc == '.' || cc == '#') ? NARROWCHARSTARTCOLUMN : 0);	// ��������� ������� ���������������, ������ ��������.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	uint_fast8_t lowhalf;
	for (lowhalf = 0; lowhalf < NHALFS; ++ lowhalf)
	{
		uint_fast8_t cgrow;
		for (cgrow = 0; cgrow < 8 && (lowhalf * 8 + cgrow) < BIGCHARH; ++ cgrow)
		{
			uint_fast8_t i = startcol;
			enum { NCOLS = (sizeof S1D13781_bigfont [0][0] / sizeof S1D13781_bigfont [0][0][0]) };
			const FLASHMEM uint8_t * p = & S1D13781_bigfont [c][lowhalf][0];

			for (; i < NCOLS; ++ i)
			{
				const uint_fast8_t v = p [i] & (1U << cgrow);
				ltdc_horizontal_pix1(i - startcol, cgrow + lowhalf * 8, v);
			}
		}
	}
	ltdc_second += BIGCHARW - startcol;
}

// ����� ���� ������� ������ ������ display_wrdatabig_begin() � display_wrdatabig_end();
static void ltdc_horizontal_put_char_half(char cc)
{
	enum { NHALFS = (sizeof S1D13781_halffont [0] / sizeof S1D13781_halffont [0][0]) };
	const uint_fast8_t startcol = 0;	// ��������� ������� ���������������, ������ ��������.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	uint_fast8_t lowhalf;
	for (lowhalf = 0; lowhalf < NHALFS; ++ lowhalf)
	{
		uint_fast8_t cgrow;
		for (cgrow = 0; cgrow < 8 && (lowhalf * 8 + cgrow) < HALFCHARH; ++ cgrow)
		{
			uint_fast8_t i = startcol;
			enum { NCOLS = (sizeof S1D13781_halffont [0][0] / sizeof S1D13781_halffont [0][0][0]) };
			const FLASHMEM uint8_t * p = & S1D13781_halffont [c][lowhalf][0];

			for (; i < NCOLS; ++ i)
			{
				const uint_fast8_t v = p [i] & (1U << cgrow);
				ltdc_horizontal_pix1(i - startcol, cgrow + lowhalf * 8, v);
			}
		}
	}
	ltdc_second += HALFCHARW - startcol;
}

#else /* LCDMODE_LQ043T3DX02K */

// ����� ���� ������� ������ ������ display_wrdata_begin() � 	display_wrdata_end();
static void ltdc_vertical_put_char_small(char cc)
{
	uint_fast8_t i = 0;
	const uint_fast8_t c = ascii_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_smallfont [0] / sizeof ls020_smallfont [0][0]) };
	const FLASHMEM uint8_t * p = & ls020_smallfont [c][0];
	
	for (; i < NBYTES; ++ i)
		ltdc_vertical_pix8(p [i]);	// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
}

// ����� ���� ������� ������ ������ display_wrdatabig_begin() � display_wrdatabig_end();
static void ltdc_vertical_put_char_big(char cc)
{
	// '#' - ����� ������
	enum { NBV = (BIGCHARH / 8) }; // ������� ������ � ����� ���������
	uint_fast8_t i = NBV * ((cc == '.' || cc == '#') ? 12 : 0);	// ��������� ������� ���������������, ������ ��������.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_bigfont [0] / sizeof ls020_bigfont [0][0]) };
	const FLASHMEM uint8_t * p = & ls020_bigfont [c][0];
	
	for (; i < NBYTES; ++ i)
		ltdc_vertical_pix8(p [i]);	// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
}

// ����� ���� ������� ������ ������ display_wrdatabig_begin() � display_wrdatabig_end();
static void ltdc_vertical_put_char_half(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_halffont [0] / sizeof ls020_halffont [0][0]) };
	const FLASHMEM uint8_t * p = & ls020_halffont [c][0];
	
	for (; i < NBYTES; ++ i)
		ltdc_vertical_pix8(p [i]);	// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
}

#endif /* LCDMODE_LQ043T3DX02K */


/* �������������� ������� �������� ������� - ����������� � ��������������� �� ������ */
void display_clear(void)
{
	const COLOR_T bg = display_getbgcolor();

#if LCDMODE_LTDC_L8

	memset(framebuff, bg, DIM_FIRST * DIM_SECOND);
	//memset(framebuff, COLOR_WHITE, DIM_FIRST * DIM_SECOND);	// debug version

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

#else /* LCDMODE_LTDC_L8 */

	unsigned i, j;
	// fill
	for (i = 0; i < DIM_FIRST; ++ i)
	{
		for (j = 0; j < DIM_SECOND; ++ j)
			framebuff [i][j] = bg;
	}

#endif /* LCDMODE_LTDC_L8 */
}

void display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
#if LCDMODE_ILI8961 || LCDMODE_LQ043T3DX02K
	ltdc_second = x * CHAR_W;
	ltdc_first = y * CHAR_H;
#else
	ltdc_first = x * CHAR_W;
	ltdc_second = y * CHAR_H;
#endif
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
#if LCDMODE_LQ043T3DX02K
	ltdc_horizontal_put_char_small(c);
#else /* LCDMODE_LQ043T3DX02K */
	ltdc_vertical_put_char_small(c);
#endif /* LCDMODE_LQ043T3DX02K */
}

// ������ ����������
void display_wrdatabar_begin(void)
{
	ltdc_secondoffs = 0;
	ltdc_h = CHAR_H;
}

// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
void display_barcolumn(uint_fast8_t pattern)
{
	ltdc_vertical_pix8(pattern);	// ������ ������ ������� ��������, ������� ��� - ����� ������� � ������
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
#if LCDMODE_LQ043T3DX02K
	ltdc_horizontal_put_char_big(c);
#else /* LCDMODE_LQ043T3DX02K */
	ltdc_vertical_put_char_big(c);
#endif /* LCDMODE_LQ043T3DX02K */
}

void display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_LQ043T3DX02K
	ltdc_horizontal_put_char_half(c);
#else /* LCDMODE_LQ043T3DX02K */
	ltdc_vertical_put_char_half(c);
#endif /* LCDMODE_LQ043T3DX02K */
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
#if LCDMODE_LQ043T3DX02K
	ltdc_horizontal_put_char_small(c);
#else /* LCDMODE_LQ043T3DX02K */
	ltdc_vertical_put_char_small(c);
#endif /* LCDMODE_LQ043T3DX02K */
}

void display_wrdata_end(void)
{
}

#if LCDMODE_LQ043T3DX02K

// ��������

/* ���������� ����� ������� - ����� ������������� */
/* ���������� ��� ����������� �����������. */
void
display_reset(void)
{
}
/* ���������� ��� ����������� �����������. */
void display_initialize(void)
{
}

void display_set_contrast(uint_fast8_t v)
{
}
#endif /* LCDMODE_LQ043T3DX02K */
#endif /* LCDMODE_LTDC */
