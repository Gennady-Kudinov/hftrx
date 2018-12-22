/*
 * SpeechProc.h
 *
 *  Created on: 10 ����. 2018 �.
 *      Author: Admin
 */

#ifndef SPEECHPROC_H_
#define SPEECHPROC_H_
#include <arm_math.h>
#include <stdlib.h>
#include <math.h>
#include "adc_dac_norm_data.h"
#include "dsp_var.h"
#include "filter_design.h"
#include "usb_sound.h"
#define uint uint32_t
#define SYMDETSMOOTH 0.99f
#define BAND1_MIN_FREQ 0
#define BAND1_MAX_FREQ 110
#define BAND2_MIN_FREQ 110
#define BAND2_MAX_FREQ 300
#define BAND3_MIN_FREQ 300
#define BAND3_MAX_FREQ 800
#define BAND4_MIN_FREQ 800
#define BAND4_MAX_FREQ 2000
#define BAND5_MIN_FREQ 2000
#define BAND5_MAX_FREQ 3500
#define BAND6_MIN_FREQ 3500
#define BAND6_MAX_FREQ 5000
#define LIMITTER_LOOK_AHEAD (100)//10ms
#define MAX_24bit_FLOAT 8388607.0f
#define LIMITTER_UP_THRESHOLD -1// � ��
#define N_POLOS 6
#define LIM_LAH_DELAY 120
#define PH_STAGES_MAX 31
#define COMP_SMOOTH_FACTOR 0.999f
#define N_FILTER (N_POLOS+1)//����� ���������� ��������, 6 - �� ���������� � 7-� - �� �������� ����������
#define REVERBERATOR_MAX_DELAY 100//100//ms
#define REVERBERATOR_DELAY_MAX_LEN (REVERBERATOR_MAX_DELAY*SAMPLE_RATE/1000)
#define DC_OFFSET_COEFF 126.0f //63 - 10Hz, 126-20Hz, 190-30Hz, 250 - 40Hz
#define SYMMETRY_CNT (SAMPLE_RATE/10)
extern const float WindowCoefs_FFT[];
extern const float WindowCoefs_2048[];

typedef struct
{
COMPLEX filter[7][FFT_FILTER_SIZE/2];
}filter_bank_t;

typedef struct
{
float R;
float y_old;
float x_old;
}
dc_reject_t;

typedef struct
{
float fc;
float g;
uint nstages;
float x[2][PH_STAGES_MAX];
float y[2][PH_STAGES_MAX];
float a[2];
float b[2];
}
phase_rotator_t;
typedef struct
{
float alpha;
float neg_ave;
float pos_ave;
float symmetry;
uint cnt;
}
symdet_t;
typedef struct
{
float F0;
float Df;
float Fs;
float be;
float a[3];
float b[3];
float c0;
float w[3]; // filter state vector and coefficients
} NotchFilter;
typedef struct
{
	int8_t MAG[FFT_FILTER_SIZE];

	uint input_type;//0- ��������, 1- �������� ����

	uint notch_en;
		uint notch_frequency;//0...1000�� � ����� 1��
		uint notch_width;//5...300 � ����� 5��

	uint ng_en;
	int ng_threshold_dB;//0...-100
	uint ng_trel;//��� � �����������
	float ng_ratio;//��� � �����������
	float ng_tatt;//��� � �����������

	uint ph_rot_en;//���/���� ������� �������
	 uint ph_rot_freq;//������� �������� ��������
	 uint ph_rot_stage;//���������� �������� �������� ��������
	 float symmetry;//���������� ���������, ������ ������
	 int pre_gain;//-40...+40��

	 uint mb_comp_en;//���/���� �������������� �����������
	 int mb_comp_threshold_dB[N_POLOS];
	 float mb_comp_tatt[N_POLOS];
	 uint mb_comp_trel[N_POLOS];
	 float mb_comp_ratio[N_POLOS];
	 int mb_comp_env[N_POLOS];//����������� ��������� � ��

	 uint eq_en;
	 int eq_gain[N_POLOS];//-15...+15��

	 uint soft_knee_comp_en;
	 int soft_knee_comp_threshold_dB;
	 float soft_knee_comp_tatt;
	 uint soft_knee_comp_trel;
	 float soft_knee_comp_ratio;
	 float soft_knee;
	 int soft_knee_comp_env;

	 int post_gain;//-40...+40��

	 uint reverb_en;//���/���� �������������
	 uint reverb_delay;//� �������������, �� �� ����� 100
	 float reverb_echo_gain;//0.0...1.0

	 float limitter_tatt;
	 uint limitter_trel;
	 int limitter_level;

	 uint out_filter_en;
	 uint filter_bank_fmin[N_FILTER];//0-5 - ��� ����������, 6-� - ��� �������� ����������, ��� 5��
	 uint filter_bank_fmax[N_FILTER];
/*
 * filter_bank_fmin[0] - �� ������������
 * filter_bank_fmax[0] - ��� 1� ������ ����������� ������� �������, ��� ���
 *
 * filter_bank_fmin[1] - ��� 2� ������ ����������� ������ �������, ��� ��������� ������
 * filter_bank_fmax[1] - ��� 2� ������ ����������� ������� �������, ��� ��������� ������
 *
 * filter_bank_fmin[2] - ��� 3� ������ ����������� ������ �������, ��� ��������� ������
 * filter_bank_fmax[2] - ��� 3� ������ ����������� ������� �������, ��� ��������� ������
 *
 * filter_bank_fmin[3] - ��� 4� ������ ����������� ������ �������, ��� ��������� ������
 * filter_bank_fmax[3] - ��� 4� ������ ����������� ������� �������, ��� ��������� ������
 *
 * filter_bank_fmin[4] - ��� 5� ������ ����������� ������ �������, ��� ��������� ������
 * filter_bank_fmax[4] - ��� 5� ������ ����������� ������� �������, ��� ��������� ������
 *
 * filter_bank_fmin[5] - ��� 6� ������ ����������� ������ �������, ��� ���
 * filter_bank_fmax[5] - �� ������������
 *
 * filter_bank_fmin[6] - ��� ��������� ������� ������ �������, ��� ��������� ������
 * filter_bank_fmax[6] - ��� ��������� ������� ������� �������, ��� ��������� ������
 *
 * */
	// int8_t FilterSpectrum[N_FILTER][FFT_FILTER_SIZE/2];

	 uint mb_filter_sharpness;//������� �������� ����������� - �� 64 �� 512
	 uint out_filter_sharpness;//������� �������� ����������� - �� 64 �� 512
}SpProcOpt_t;

typedef struct
{
float max_level_dB;
float  slope;//����������, � ��
uint     sr;
float threshold;
int threshold_dB;//��, ������� -90, ����� ������������ max_level_dB, �������������
float gain;
float gain_dB;
float env;
float env_dB;
float ratio;
uint trel;
float alpha_rel;
float beta_rel;
float tatt;//����� �����, � ��,
float alpha_att;
float beta_att;
float ave_out;
float ave_gain;
int ave_gain_dB;
}
compparam_t;

typedef struct
{
float max_level_dB;
float  slope;//����������, � ��
uint     sr;
float threshold;
int threshold_dB;//��, ������� -90, ����� ������������ max_level_dB, �������������
float gain;
float gain_dB;
float env;
float env_dB;
float ratio;
uint trel;
float alpha_rel;
float beta_rel;
float tatt;//����� �����, � ��,
float alpha_att;
float beta_att;
float ave_out;
float ave_gain;
int ave_gain_dB;

float knee;
float knee_width_dB;
float knee_low_bound_dB;
float knee_up_bound_dB;


float knee_width;
float knee_low_bound;
float knee_up_bound;
/*int lah_ptr;
int lah_mask;
float lah_delay[DSP_SAMPLE_RATE/LIMITTER_LOOK_AHEAD];*/
}
soft_kneecompp_t;

typedef struct
{
float a[3];
float b[3];
float w[3];
//float fs;// sample rate
float Df;//bandwidth
float G;
//float GdB;//��������� �������� , � ��
float GB;
float be;
float c0;
float f0;//central frequency
}
eqparam_t;


typedef struct
{
float max_level_dB;
float  slope;//����������, � ��
uint     sr;
float threshold;
int threshold_dB;//��, ������� -90, ����� ������������ max_level_dB, �������������
float gain;
float gain_dB;
float env;
float env_dB;
float ratio;
uint trel;
float alpha_rel;
float beta_rel;
float tatt;//����� �����, � ��,
float alpha_att;
float beta_att;
float ave_out;
float ave_gain;
int ave_gain_dB;
int lah_ptr;
int lah_mask;
float lah_delay[LIM_LAH_DELAY];
}
limitparam_t;
extern void SpeechProcessor (int* in,//���� �� ������
					int* out//����� ������
		) ;

#define ENC1_REV 0
#define ENC2_REV 1

#endif /* SPEECHPROC_H_ */
