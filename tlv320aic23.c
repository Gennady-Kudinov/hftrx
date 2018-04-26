/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"
#include "board.h"

#include "spifuncs.h"

#include "./formats.h"

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_TLV320AIC23B)
//
// ���������� ������� TLV320AIC23B (WM8731)
//
// � slave mode ������ ����� �������� ����������� ������ �� ��������� ������ ������� BCLK.
// ���� �� ������������, ������� ��� ������ ��� �� �������� ���������� ������ �� ���������� ������ BCLK.
//
#include "audio.h"
#include "tlv320aic23.h"

// Clock period, SCLK no less then 80 nS (�� ���� 12.5 ���)
#define TLV320AIC23_SPIMODE		SPIC_MODE3	// Linux initialize in mode 0

// ������� ������������� ���������������� ������� ��������� � SPI
#define WITHSPIEXT16 (WITHSPIHW && WITHSPI16BIT)

// The TLV320AIC23B is a write only device and responds only if R/W is 0.
#define TLV320AIC23_ADDRESS_W	0x34	// I2C address: 0x34 or 0x36	- depend on adress pin state

static void tlv320aic23_setreg(
	uint_fast8_t regv,			/* 7 bit value */
	uint_fast16_t datav			/* 9 bit value */
	)
{
	const uint_fast16_t fulldata = regv * 512 + (datav & 0x1ff);

#if CODEC_TYPE_TLV320AIC23B_USE_SPI
	// ����� ����������� �� SPI
	const spitarget_t target = targetcodec1;	/* addressing to chip */

	#if WITHSPIEXT16

		hardware_spi_connect_b16(SPIC_SPEEDFAST, TLV320AIC23_SPIMODE);
		prog_select(target);	/* start sending data to target chip */
		hardware_spi_b16_p1(fulldata);
		hardware_spi_complete_b16();
		prog_unselect(target);	/* done sending data to target chip */
		hardware_spi_disconnect();

	#else /* WITHSPIEXT16 */

		spi_select(target, TLV320AIC23_SPIMODE);
		spi_progval8_p1(target, fulldata >> 8);		// LSB=b8 of datav
		spi_progval8_p2(target, fulldata >> 0);
		spi_complete(target);
		spi_unselect(target);

	#endif /* WITHSPIEXT16 */

#else /* CODEC_TYPE_TLV320AIC23B_USE_SPI */

	// ����� ����������� �� I2C
	i2c_start(TLV320AIC23_ADDRESS_W);
	i2c_write(fulldata >> 8);
	i2c_write(fulldata >> 0);
	i2c_waitsend();
	i2c_stop();

#endif /* CODEC_TYPE_TLV320AIC23B_USE_SPI */
}

static void tlv320aic23_initialize_slave_fullduplex(void)
{
	tlv320aic23_setreg(TLV320AIC23_RESET, 0x00);	// RESET

	tlv320aic23_setreg(TLV320AIC23_PWR,			
		TLV320AIC23_CLK_OFF |	// ��������� ����� �������� ������� (����� 02) - ����� ������������ ��� ������������ - �� ��������������������� ���������� ������ ������������
		TLV320AIC23_OSC_OFF |	// ��������� ��������� ���������
		//TLV320AIC23_OUT_OFF |	// ��������� ��������� ���������
		//TLV320AIC23_ADC_OFF |	// ��������� ���
		//TLV320AIC23_LINE_OFF |	// ��������� Line input
		0
		);	

	tlv320aic23_setreg(TLV320AIC23_DIGT_FMT, 
		0 * TLV320AIC23_MS_MASTER | /* operate in slave mode */
		TLV320AIC23_IWL_16 |
#if WITHI2S_FORMATI2S_PHILIPS
		TLV320AIC23_FOR_I2S |
#else /* WITHI2S_FORMATI2S_PHILIPS */
		TLV320AIC23_FOR_LJUST |
#endif /* WITHI2S_FORMATI2S_PHILIPS */
		0
		);

	// ��-�� ����������� SPI/I2S ����������� �� STM32 �������� 
	// ������������ ������ ����� � MCLK=256*Fs
	// ���������� �������� �� MCLK �� ������� ������������� - /8
	// �� MCLK ������� 12.288 ���
	// See TI SLWS106H table 3.3.2.2 Normal-Mode Sampling Rates

#if CODEC_TYPE_TLV320AIC23B_USE_8KS

	// BCLK = 0.256 MHz (stereo, 16 bit/ch)
	tlv320aic23_setreg(TLV320AIC23_SRATE,			// Sample Rate Control
		3 * (1U << TLV320AIC23_SR_SHIFT) |	// BOSR = 0, SR[3:0] bits = 3: 12.288 / 256 / 6 = 8 kHz DAC and ADC sampling rate
		0 * (1U << TLV320AIC23_BOSR_SHIFT) |
		0	
		);	

#else /* CODEC_TYPE_TLV320AIC23B_USE_8KS */

	// BCLK = 1.536 MHz (stereo, 16 bit/ch)
	tlv320aic23_setreg(TLV320AIC23_SRATE,			// Sample Rate Control
		0 * (1U << TLV320AIC23_SR_SHIFT) |	// BOSR = 0, SR[3:0] bits = 0: 12.288 / 256 = 48 kHz DAC and ADC sampling rate
		0 * (1U << TLV320AIC23_BOSR_SHIFT) |
		0	
		);	

#endif /* CODEC_TYPE_TLV320AIC23B_USE_8KS */

#if 1 //WITHDEBUG
	// ������� ��� ����, ����� ��� ������� � ��������� ����� ��� �������� �� ������ �����.
	// �������� ���� � ���������, �������� ���
	// mic input
	tlv320aic23_setreg(TLV320AIC23_ANLG, 
		TLV320AIC23_DAC_SELECTED |
		//TLV320AIC23_MICM_MUTED |
		1 * TLV320AIC23_MICB_20DB |			// 1 - ��������� ������������� ���������
		TLV320AIC23_INSEL_MIC |			// ��������� � ��������� � �� � line in
		0
		);

	tlv320aic23_setreg(TLV320AIC23_DIGT, 
		0 * TLV320AIC23_ADCHP_ONFF |			/* ��� ����� ��� - ������� ���� �������� ��������� */
		0
		);
#endif /* WITHDEBUG */

	tlv320aic23_setreg(TLV320AIC23_ACTIVE, 
		TLV320AIC23_ACT_ON |		// Digital Interface Activation
		0
		);
}

/* ��������� ��������� �� �������� */
static void tlv320aic23_setvolume(uint_fast16_t gain, uint_fast8_t mute, uint_fast8_t mutespk)
{
	uint_fast8_t level = (gain - BOARD_AFGAIN_MIN) * (TLV320AIC23_OUT_VOL_MAX - TLV320AIC23_OUT_VOL_MIN) / (BOARD_AFGAIN_MAX - BOARD_AFGAIN_MIN) + TLV320AIC23_OUT_VOL_MIN;
	(void) mutespk;	// ���������� ���������� �� �������� ������ � ���� ������ ���
	tlv320aic23_setreg(TLV320AIC23_LCHNVOL, 
		(mute == 0) * (level & TLV320AIC23_OUT_VOL_MASK) |
		TLV320AIC23_LRS_ENABLED |	/* ����� � ������ ������������ */
		0 * TLV320AIC23_LZC_ON |	/* ������������� � ��������� ����� "0" */
		0
		);
}

/* ����� LINE IN ��� ��������� ��� ��� ������ ��������� */
static void tlv320aic23_lineinput(uint_fast8_t v, uint_fast8_t mikebust20db, uint_fast16_t mikegain, uint_fast16_t linegain)
{
	//debug_printf_P(PSTR("tlv320aic23_lineinput: glob_mik1level=%d\n"), mikegain);
	(void) mikegain;	// ���������� ���������� ��������� � ���� ������ ���
	if (v != 0)
	{
		uint_fast8_t level = (linegain - WITHLINEINGAINMIN) * (TLV320AIC23_IN_VOL_MAX - TLV320AIC23_IN_VOL_MIN) / (WITHLINEINGAINMAX - WITHLINEINGAINMIN) + TLV320AIC23_IN_VOL_MIN;
		tlv320aic23_setreg(TLV320AIC23_LINVOL, 
			TLV320AIC23_LRS_ENABLED |	/* ����� � ������ ������������ */
			(level & TLV320AIC23_IN_VOL_MASK) |
			0
			);
		// ���� � line input
		// line input
		tlv320aic23_setreg(TLV320AIC23_ANLG, 
			TLV320AIC23_DAC_SELECTED |
			TLV320AIC23_MICM_MUTED |
			//TLV320AIC23_MICB_20DB |
			//TLV320AIC23_INSEL_MIC |			// ��������� � ��������� � �� � line in
			0
			);
	}
	else
	{
		// ���� � ���������
		// mic input
		tlv320aic23_setreg(TLV320AIC23_ANLG, 
			TLV320AIC23_DAC_SELECTED |
			//TLV320AIC23_MICM_MUTED |
			(mikebust20db != 0) * TLV320AIC23_MICB_20DB |			// 1 - ��������� ������������� ���������
			TLV320AIC23_INSEL_MIC |			// ��������� � ��������� � �� � line in
			0
			);
	}
	tlv320aic23_setreg(TLV320AIC23_DIGT, 
		0 * TLV320AIC23_ADCHP_ONFF |			/* ��� ����� ��� - ������� ���� �������� ��������� */
		0
		);
}


/* ��������� ��������� ����� � ��������� (���, ����������, ...) */
static void tlv320aic23_setprocparams(
	uint_fast8_t procenable,		/* ��������� ��������� */
	const uint_fast8_t * gains		/* ������ � ����������� */
	)
{

}


const codec1if_t *
board_getaudiocodecif(void)
{

	static const char codecname [] = "TLV320AIC23";

	/* ��������� ���������� ������� */
	static const codec1if_t ifc =
	{
		tlv320aic23_initialize_slave_fullduplex,
		tlv320aic23_setvolume,	/* ��������� ��������� �� �������� */
		tlv320aic23_lineinput,	/* ����� LINE IN ��� ��������� ��� ��� ������ ��������� */
		tlv320aic23_setprocparams,	/* ��������� ��������� ����� � ��������� (���, ����������, ...) */
		codecname				/* �������� ������ (������ ��������� ������� � ���������) */
	};

	return & ifc;
}

#endif /* (CODEC1_TYPE == CODEC_TYPE_TLV320AIC23B) */
