/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>	// format specifiers for printing

uint_fast8_t local_snprintf_P( char *buffer, uint_fast8_t count, const FLASHMEM char *format, ... );
// ���������� ������
void debug_printf_P(const FLASHMEM char *format, ... );

