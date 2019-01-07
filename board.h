/* $Id$ */
/* Pin manipulation functioms */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include "synthcalcs.h"

#include <stdint.h>

void board_reset(void);			// ������ ������� reset_n
void board_init_io(void);
//void board_init_io2(void);	// debug
void board_init_chips(void);
void board_init_chips2(void);
uint_fast8_t boad_fpga_adcoverflow(void);	/* ��������� �������� ������������ ��� �������� ������ */
uint_fast8_t boad_mike_adcoverflow(void);	/* ��������� �������� ������������ ��� ������������ ������ */


uint_fast32_t getvcoranges(uint_fast8_t vco, uint_fast8_t top);	/* ������� ��� ��������� ��� */

void prog_dds1_ftw(const ftw_t * value);
void prog_dds1_ftw_sub(const ftw_t * value);
void prog_dds1_ftw_sub3(const ftw_t * value);
void prog_dds1_ftw_sub4(const ftw_t * value);
void prog_dds1_ftw_noioupdate(const ftw_t * value);
void prog_dds2_ftw(const ftw_t * value);
void prog_dds3_ftw(const ftw_t * value);
void prog_rts1_ftw(const ftw_t * value);	// ��������� ����������� ������� ����������� ����������
void prog_xvtr_freq(uint_fast32_t f,uint_fast8_t enable);	// ��������� ������� ����������

typedef uint_fast32_t pllhint_t;

void si570_initialize(void);
void synth_lo1_setreference(uint_fast32_t freq);
pllhint_t si570_get_hint(uint_fast32_t f);
uint_fast16_t si570_get_divider(pllhint_t hint);
void si570_n(pllhint_t hint, const phase_t * value);

//void prog_pll1_r(const phase_t * value);
uint_fast8_t board_pll1_set_n(const phase_t * value, pllhint_t hint, uint_fast8_t stop);	/* ��������� ���������� �������� � ������������ ��� �� ��������� hint (���� ���������) */
pllhint_t board_pll1_get_hint(uint_fast32_t f);		/* �������� �������� �� VCO � ��� VCO �� �������� ������� */
uint_fast16_t board_pll1_get_divider(pllhint_t hint);	/* ��������� �������� �������� �������� �� hint */
void board_pll1_set_vco(pllhint_t hint);		/* ��������� ������������ ��� �� ��������� hint */
void board_pll1_set_vcodivider(pllhint_t hint);	/* ��������� ��������� �������� ��� �� ��������� hint */

extern int_fast32_t si570_get_xtall_base(void); 
extern uint_fast16_t si570_xtall_offset; 
#define OSCSHIFT 15000UL	/* �������� ��������� ����������� ������� �������� ���������� (� ������). */

void prog_pll2_n(const phase_t * value);
void prog_pll2_r(const phase_t * value);
void board_update(void);		/* ������� ��� ������� ��� ������ ������� �������� � �������� */

void board_flt1regchanged(void);
void board_codec1regchanged(void);

void prog_dds1_setlevel(uint_fast8_t percent);

void board_set_lctl1(uint_fast8_t v); // ���������� ������� ��������


void board_set_tx(uint_fast8_t v);	/* ��������� �� �������� */
void board_set_opowerlevel(uint_fast8_t n);	/* ���������� �������� �������� WITHPOWERTRIMMIN..WITHPOWERTRIMMAX */

void board_set_att(uint_fast8_t v);
void board_set_antenna(uint_fast8_t v);
void board_set_agc(uint_fast8_t n);
void board_set_sleep(uint_fast8_t v);	/* ��������� � ����� ������������ ����������� */

void board_set_maxlabdac(uint_fast16_t n);	/* �������� �� ������ ��� ��� ����� ������� �������� ���������� PLL */

void board_setfanflag(uint_fast8_t v);	/* �������� ���������� */
void board_set_mainsubrxmode(uint_fast8_t v);	// �����/������, A - main RX, B - sub RX
void board_set_detector(uint_fast8_t v);
void board_set_nfm(uint_fast8_t v);
void board_set_nfmnbon(uint_fast8_t v);	/* �������� noise blanker �� SW2014FM */
void board_set_filter(uint_fast16_t v);
void board_set_bandf(uint_fast8_t n);	/*  ����������� ������ �������� */
void board_set_bandf2(uint_fast8_t n);	/* ����������� ������ ����������� */
void board_set_bandf3(uint_fast8_t n);	/* ���������� ����� ������ ACC */
void board_set_bcdfreq100k(uint_fast16_t bcdfreq);	/* ��� ������ ��������� - ������� � ������������� 100 ��� */
void board_set_pabias(uint_fast8_t n);	/* ���������� ��� ����� ��������� ������� ����������� */
void board_set_bandfonhpf(uint_fast8_t n);	/* ���������� ��� ������ ������������ �������, � �������� ���������� ��� ����� ��� � SW20xx */
void board_set_bandfonuhf(uint_fast8_t n);	/* ���������� ��� ������������ �������, �� ������� �������� UHF */
void board_set_if4lsb(uint_fast8_t v);	/* ��������� ��� ���������� ������� �������������� */
//void board_set_fltsofter(uint_fast8_t n);/* ��� ���������� ������������ ������ ������� �������� �������� �� ����� */
void board_set_narrow(uint_fast8_t v);	/* ��������� ������ ����� ��������� (��) */
void board_set_notch(uint_fast8_t v);		/* �� ������� �������� - ��������� �� ������������ ������� */
void board_set_notchnarrow(uint_fast8_t v);		/* �� ������� �������� - ��������� �� ������������ ������� � ������ CW */
void board_set_vox(uint_fast8_t v);		/* �� ������� �������� - ��������� VOX (��� ������� ����) */
void board_set_preamp(uint_fast8_t v); /* ��������� ��� */
void board_set_adcfifo(uint_fast8_t v);	/* ��������� ������������ ���� ��� ��� � ������ ������������ ���� */
void board_set_adcoffset(int_fast16_t n); /* �������� ��� ��������� ������� � ��� */
void board_set_xvrtr(uint_fast8_t v);	/* ��������� ������ ���������� */
void board_set_dacstraight(uint_fast8_t v);	/* ��������� ������������ ���� ��� ��� � ������ ������������ ���� */
void board_set_dactest(uint_fast8_t v);	/* ������ ������ ������������� � ��� ����������� ������������ ����� NCO */
void board_set_tx_inh_enable(uint_fast8_t v);	/* ���������� ������� �� ���� tx_inh */
void board_set_tx_bpsk_enable(uint_fast8_t v);	/* ���������� ������� ������������ ��������� � FPGA */
void board_set_mode_wfm(uint_fast8_t v);
void board_set_dither(uint_fast8_t v);	/* ���������� ����������� � LTC2208 */
void board_set_adcrand(uint_fast8_t v);	/* ���������� ����������� � LTC2208 */
void board_set_dacscale(uint_fast8_t n);	/* ������������� ��������� ������� � ��� ����������� - 0..100% */
void board_set_dac1(uint_fast8_t n);	/* ���������� �������� ���������� */
void board_set_bglight(uint_fast8_t n);	/* ��������� ��������� ������� */
void board_set_kblight(uint_fast8_t v);	/* ��������� ��������� ���������� */
void board_set_blfreq(uint_fast32_t n);	/* ��������� �������� ��� ������������ ������� ������� ��������������� ��������� */
void board_set_txcw(uint_fast8_t v);	/* ��������� �������� � ����� ���������� ���������� */
void board_set_txgate(uint_fast8_t v);	/* ���������� �������� � ���������� ��������� */
void board_set_scalelo1(uint_fast8_t n);	/* SW-2011-RDX */
void board_set_tuner_C(uint_fast8_t n);	/* ��������� �������� ������������ � ����������� ���������� */
void board_set_tuner_L(uint_fast8_t n);	/* ��������� �������� ������������� � ����������� ���������� */
void board_set_tuner_type(uint_fast8_t v);	/* ������� �����������/����������� ������������ */
void board_set_tuner_bypass(uint_fast8_t v);	/* ����� ������������ ���������� */
void board_set_autotune(uint_fast8_t v);	/* ��������� � ������ ��������� ������������ ���������� */

void board_set_user1(uint_fast8_t v);
void board_set_user2(uint_fast8_t v);
void board_set_user3(uint_fast8_t v);
void board_set_user4(uint_fast8_t v);
void board_set_user5(uint_fast8_t v);

void board_set_reset_n(uint_fast8_t v);	/* ��������� ������� RESET ���� ����������� */
void board_set_flt_reset_n(uint_fast8_t v);	/* ��������� ������� RESET ������ �������� � FPGA */
void board_set_i2s_enable(uint_fast8_t v);	/* ���������� ��������� �������� ������� ��� I2S � FPGA */

void board_set_stage1level(uint_fast8_t v);		/* ���������� ����� ������� ������� �������� hermes */
void board_set_stage2level(uint_fast8_t v);		/* ���������� ����� ������� ������� �������� hermes */
void board_set_sdcardpoweron(uint_fast8_t n);	/* ��-0: �������� ������� SD CARD */
void board_set_usbflashpoweron(uint_fast8_t n);	/* ��-0: �������� ������� USB FLASH */
void board_set_attvalue(uint_fast8_t v);		/* ���������� �������� ����������� ������-���������� */
void prog_dac1_a_value(uint_fast8_t v);
void prog_dac1_b_value(uint_fast8_t v);
void board_set_affilter(uint_fast8_t v);
void board_set_loudspeaker(uint_fast8_t v);
void board_set_digigainmax(uint_fast8_t v);		/* �������� ������ ����������� ��������� �������� - ������������ �������� */
void board_set_gvad605(uint_fast8_t v);		/* ���������� �� AD605 (���������� ��������� ������ �� */
void board_set_fsadcpower10(int_fast16_t v);		/*	��������, ��������������� full scale �� IF ADC */
uint_fast32_t board_get_fqmeter(void);			/* �������� �������� ���������� ������� */
void board_ctl_set_vco(uint_fast8_t n);	// 0..3, 0..5 - code of VCO
void board_setlo2xtal(uint_fast8_t n);	// ����� ������ ��� ����������������� ���������
void board_codec2_nreset(uint_fast8_t v);	/* ������������ ������� "RESET" ��� codec2. 0 - ����� �����. */
void board_set_modem_speed100(uint_fast32_t v);	// �������� �������� � ��������� 1/100 ���
void board_set_modem_mode(uint_fast8_t v);	// ����������� ���������

void board_lcd_rs(uint_fast8_t v);	// ��������� ������� �� ������� lcd register select - �� ��������� board_update
void board_lcd_reset(uint_fast8_t v);	// ��������� ������� �� ������� lcd reset

/* ����� - ��������� ������� � ������������ � ��������� */
/* ���������� ��� ����������� �����������. */
void board_beep_initialize(void);

void board_sidetone_setfreq(uint_least16_t freq);	/* freq - ������� � ������. ������� - 400 ���� (���������� ������� ������ CAT).*/
void board_keybeep_setfreq(uint_least16_t freq);	/* freq - ������� � ������. */
/* ������� ���������� ������ ��������� ������� - ����� �������������. */
void board_sidetone_enable(uint_fast8_t state);
/* ���������� ��� ����������� �����������. */
void board_keybeep_enable(uint_fast8_t state);
void board_testsound_enable(uint_fast8_t state);
void board_subtone_setfreq(uint_least16_t tonefreq01);	/* tonefreq - ������� � ������� ����� �����. */
void board_subtone_enable(uint_fast8_t state);

/* �������� ������������� FIR ������� � FPGA */
void board_fpga_fir_initialize(void);
void board_reload_fir(uint_fast8_t ifir, const int_fast32_t * const k, unsigned Ntap, unsigned CWidth); /* ������ ����������� ���������� ������� � FPGA (������������) */
/* ���������� ���������� ��������� - bandpass.c */
void bandf_calc_initialize(void);
uint8_t bandf_calc(uint_fast32_t freq);	/* �������� ����� ������������ ������� �� ������� */
uint8_t bandf2_calc(uint_fast32_t freq);	/* �������� ����� ������������ ������� ����������� �� ������� */
uint8_t bandf3_calc(uint_fast32_t freq);	/* �������� ��� ��� ���������� ����� ������ ACC */

void board_rtc_getdate(
	uint_fast16_t * year,
	uint_fast8_t * month,
	uint_fast8_t * dayofmonth
	);
void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	);
void board_rtc_getdatetime(
	uint_fast16_t * year,
	uint_fast8_t * month,	// 01-12
	uint_fast8_t * dayofmonth,
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	);
void board_rtc_setdate(
	uint_fast16_t year,
	uint_fast8_t month,
	uint_fast8_t dayofmonth
	);
void board_rtc_settime(
	uint_fast8_t hour,
	uint_fast8_t minute,
	uint_fast8_t secounds
	);
void board_rtc_setdatetime(
	uint_fast16_t year,
	uint_fast8_t month,
	uint_fast8_t dayofmonth,
	uint_fast8_t hour,
	uint_fast8_t minute,
	uint_fast8_t secounds
	);
uint_fast8_t board_rtc_chip_initialize(void);

#if defined (NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING)

	void nvram_initialize(void);
	void nvram_set_abytes(uint_fast8_t v);
	#ifndef NVRAM_END
		#error NVRAM_END required, may be missing NVRAM_TYPE
	#endif
	#if (NVRAM_END > 255)
		typedef uint_least16_t nvramaddress_t;				/* ����� ������� 8 ���. �������� � NVRAM. ���� MENUNONVRAM - ������ ������ � ������ */
	#else /* (NVRAM_END > 255) */
		typedef uint_least8_t nvramaddress_t;				/* ����� ������� 8 ���. �������� � NVRAM. ���� MENUNONVRAM - ������ ������ � ������ */
	#endif /* (NVRAM_END > 255) */

	void save_i32(nvramaddress_t addr, uint32_t v);	/* ���������� �� ���������� ������� � FRAM ������ 32-������� ����� */
	uint_fast32_t restore_i32(nvramaddress_t addr); /* ������� �� ���������� ������� �� FRAM ������ 32-������� ����� */
	void save_i16(nvramaddress_t addr, uint16_t v); /* ���������� �� ���������� ������� � FRAM ������ 16-������� ����� */
	uint_fast16_t restore_i16(nvramaddress_t addr); /* ������� �� ���������� ������� �� FRAM ������ 16-������� ����� */
	void save_i8(nvramaddress_t addr, uint8_t v); /* ���������� �� ���������� ������� � FRAM ������ ����� */
	uint_fast8_t restore_i8(nvramaddress_t addr); /* ������� �� ���������� ������� �� FRAM ������ ����� */

#else /* defined (NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING) */

#endif /* defined (NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING) */


/* �������� �������� forward & reflected ADC */
adcvalholder_t board_getswrmeter(
	adcvalholder_t * reflected, 	// � ��������� ���
	uint_fast8_t swrcalibr	// 90..110 - ���������
	);	
uint_fast8_t board_getpwrmeter(
	uint_fast8_t * toptrace	// peak hold
	); 

uint_fast16_t 
mcp3208_read(
	spitarget_t target,
	uint_fast8_t diff,
	uint_fast8_t adci,
	uint_fast8_t * valid
	);

void board_adc_initialize(void);
void board_usb_initialize(void);
void board_usb_activate(void);

void usbd_descriptors_initialize(uint_fast8_t deschs);
void hardware_usbd_dma_initialize(void);
void hardware_usbd_dma_enable(void);

uint_fast8_t board_get_adcinputs(void);	/* �������� ���������� ������� ���, ��������������� � ���������� */
uint_fast8_t board_get_adcch(uint_fast8_t index);	/* �������� ����� ��� �� ������� � ������� */

void board_adc_store_data(uint_fast8_t i, adcvalholder_t v);
void board_adc_filtering(void);	// ������� ���������� �� ����������� ���������� ����� ��������� �������� �� ���������� ������ ���

void 
modemchangefreq(
	uint_fast32_t f		// �������, ������� ������������� �� ������� �� modem
	);

void 
modemchangespeed(
	uint_fast32_t speed100		// ��������, ������� ������������� �� ������� �� modem
	);

void 
modemchangemode(
	uint_fast32_t modemmode		// �����, ������� ������������� �� ������� �� modem
	);

int gettxstate(void);


enum 
{
	BOARD_ADCFILTER_DIRECT,		/* ���������� �� ����������� (�������� ��� ���� ������� �� ���������) */
	BOARD_ADCFILTER_AVERAGEPWR,	/* ���������� ��� ���������� �������� - ������ ���� ����� */
	BOARD_ADCFILTER_TRACETOP3S,	/* ������������ ��������� � ���������� ������� 3 ������� */
	BOARD_ADCFILTER_LPF,			/* ���, �������� �������� � ���� ����� � ������������� ������ */
#if WITHPOTFILTERS
	BOARD_ADCFILTER_HISTERESIS2,	/* ���������� � 2 ������ ����������� */
#endif /* WITHPOTFILTERS */
	//
	BOARD_ADCFILTER_TYPECOUNT
};

uint_fast8_t board_getadc_filtered_u8(uint_fast8_t i, uint_fast8_t lower, uint_fast8_t upper);	/* �������� �������� �� ��� � ��������� lower..upper (������� �������) */
uint_fast16_t board_getadc_filtered_u16(uint_fast8_t i, uint_fast16_t lower, uint_fast16_t upper);	/* �������� �������� �� ��� � ��������� lower..upper (������� �������) */
uint_fast8_t board_getadc_smoothed_u8(uint_fast8_t i, uint_fast8_t lower, uint_fast8_t upper);	/* ��� ��������� ���������������� �������� ����� ��� ������������ �������� �� ������ ������ ������������ � ���� �� 1 */
uint_fast8_t board_getadc_unfiltered_u8(uint_fast8_t i, uint_fast8_t lower, uint_fast8_t upper);	/* �������� �������� �� ��� � ��������� lower..upper (������� �������) */
uint_fast16_t board_getadc_unfiltered_u16(uint_fast8_t i, uint_fast16_t lower, uint_fast16_t upper);	/* �������� �������� �� ��� � ��������� lower..upper (������� �������) */
uint_fast32_t board_getadc_unfiltered_u32(uint_fast8_t i, uint_fast32_t lower, uint_fast32_t upper);	/* �������� �������� �� ��� � ��������� 0..255 */
adcvalholder_t board_getadc_filtered_truevalue(uint_fast8_t i);	/* �������� �������� �� ��� */
adcvalholder_t board_getadc_unfiltered_truevalue(uint_fast8_t i);	/* �������� �������� �� ��� */
adcvalholder_t board_getadc_fsval(uint_fast8_t i);	/* �������� ������������ ��������� �������� �� ��� */
//void hardware_set_adc_filter(uint_fast8_t i, uint_fast8_t v);	/* ���������� ������ ���������� ������ (� ������ ������� �� �������� ��� */
//void hardware_set_adc_filterLPF(uint_fast8_t i, uint_fast8_t k);	/* ���������� ������ ���������� LPF � ������� ����� - �������� 1.0..0.0, ���������� �� BOARD_ADCFILTER_LPF_DENOM */
#define BOARD_ADCFILTER_LPF_DENOM	128		/* ��������� ������������� ����� ��� ���������� BOARD_ADCFILTER_LPF */


enum
{
	STTE_OK,		// ����� ���������� � ��������� ���������.
	STTE_BYUSY,		// ��������� state machine ��� ��������� ���� ������� - ��������� ��������� ����.
	//
	STTE_count
};

typedef uint_fast8_t STTE_t;


#endif /* BOARD_H_INCLUDED */
