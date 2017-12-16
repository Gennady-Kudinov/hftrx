/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef ENCODER_H_INCLUDED
#define ENCODER_H_INCLUDED

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */


#define ENCODER_SLOW_STEPS 48	/* ����� �� ���� ������ ��������� �� ����������� �������� ��������  */
#define ENCODER_MENU_STEPS 24	/* ���������� ��������� �������������� ��������� �� ���� ������ ��������� */


int_least16_t getRotateHiRes(uint_fast8_t * jumpsize, uint_fast8_t hiresdiv);	/* ��������� ����������� �������� ���������� �� ���������. ���������� ������������ */
int_least16_t getRotateHiRes2(uint_fast8_t * jumpsize);	/* ��������� ����������� �������� ���������� �� ���������. ���������� ������������ */
void encoder_clear(void);	/* ���������� ������������ */
int getRotateLoRes(uint_fast8_t hiresdiv); /* ��������� "���������������" ���������� ���������� �� ���������. */
void encoder_initialize(void);
void encoder_pushback(int outsteps, uint_fast8_t hiresdiv);
void encoder_kbdctl(
	uint_fast8_t code, 		// ��� �������
	uint_fast8_t accel		// 0 - ��������� ������� �� �������, ����� ����������
	);

int encoder_get_snapshot(unsigned * speed, const uint_fast8_t derate);
int encoder2_get_snapshot(unsigned * speed, const uint_fast8_t derate);

void encoder_set_resolution(uint_fast8_t resolution, uint_fast8_t dynamic);	// �������� - ������� �� ENCRESSCALE ��������.

#define ENCODER_NORMALIZED_RESOLUTION (1440)	// ����������� ��������� �� ������ � ������� - ��������������� ��������
//#define ENCODER_NORMALIZED_RESOLUTION (144)	// ����������� ��������� �� ������ � ������� - ��������������� ��������

#define ENCRESSCALE 4UL


#define ENC_DYNA_MAX 4

#endif /* ENCODER_H_INCLUDED */

