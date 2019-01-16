/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#if WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN
	#define ARMI2SMCLKX(scale)	(DUCDDC_FREQ * (uint_fast64_t) (scale) / FPGADECIMATION)
#else /* WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN */
	#define ARMI2SMCLKX(scale)	(ARMSAIMCLK * (uint_fast64_t) (scale) / 256)
#endif /* WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN */

#define ARMSAIRATE		(ARMSAIMCLK / 256)	// SAI sample rate (FPGA/IF CODEC side)

#if WITHDTMFPROCESSING

	#define ARMI2SRATE			((unsigned long) 8000)	// I2S sample rate audio codec (human side)
	#define ARMI2SRATEX(scale)	((unsigned long) (ARMI2SRATE * (scale)))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATE100		((unsigned long) ARMI2SRATEX(100))

#else /* WITHDTMFPROCESSING */

	#define ARMI2SRATE			((unsigned long) (ARMI2SMCLK / 256))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATEX(scale)	((unsigned long) (ARMI2SMCLKX(scale)))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATE100		((unsigned long) (ARMI2SRATEX(100)))

#endif /* WITHDTMFPROCESSING */

#define MODEMBUFFERSIZE8	1024

#if WITHUSEDUALWATCH
	#define NTRX 2	/* ���������� ������� ���������. */
#else /* WITHUSEDUALWATCH */
	#define NTRX 1	/* ���������� ������� ���������. */
#endif /* WITHUSEDUALWATCH */

/* ���������� ������� ��������������� �������, 
   ����� �� ���� ������� ���������� �� ������ �������, 
   ��� � DMABUFFSIZE32RX 
 */
#if WITHDSPEXTDDC

	#if CPUSTYLE_R7S721

		// buff data layout: I main/I sub/Q main/Q sub
		#define DMABUFSTEP32RX	8		// ������� ������ ������������� ������ ����� � DMA ������

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX1I	1		// RX1, I
		#define DMABUF32RX0Q	4		// RX0, Q
		#define DMABUF32RX1Q	5		// RX1, Q

		#define DMABUFSTEP32TX	8		// ������� ������ ������������� ������ ����� � DMA ������
		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	4		// TX, Q

		#if WITHRTS96
			#define DMABUF32RTS0I	2		// RTS0, I	// previous - oldest
			#define DMABUF32RTS0Q	6		// RTS0, Q	// previous
			#define DMABUF32RTS1I	3		// RTS1, I	// current	- nevest
			#define DMABUF32RTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */

		// Slot S0, S4: Oldest sample (T-3)
		// Slot S1, S5: Old sample (T-2)
		// Slot S2, S6: Old sample (T-1)
		// Slot S3, S7: Newest sample (T-0)
		#define DMABUF32RXWFM0I	0		// WFM OLDEST
		#define DMABUF32RXWFM0Q	4		// WFM
		#define DMABUF32RXWFM1I	1		// WFM
		#define DMABUF32RXWFM1Q	5		// WFM
		#define DMABUF32RXWFM2I	2		// WFM
		#define DMABUF32RXWFM2Q	6		// WFM
		#define DMABUF32RXWFM3I	3		// WFM NEWEST
		#define DMABUF32RXWFM3Q	7		// WFM

		#define DMABUFSTEP16	2		// 2 - ������� ������ ������������� ��� ����� � DMA ������

	#elif CPUSTYLE_STM32F

		// buff data layout: I main/I sub/Q main/Q sub
		#define DMABUFSTEP32RX	8		// ������� ������ ������������� ������ ����� � DMA ������

		#define DMABUF32RX0I	0		// RX0, I
		#define DMABUF32RX1I	1		// RX1, I
		#define DMABUF32RX0Q	4		// RX0, Q
		#define DMABUF32RX1Q	5		// RX1, Q

		#if WITHRTS96
			#define DMABUF32RTS0I	2		// RTS0, I	// previous - oldest
			#define DMABUF32RTS0Q	6		// RTS0, Q	// previous
			#define DMABUF32RTS1I	3		// RTS1, I	// current	- nevest
			#define DMABUF32RTS1Q	7		// RTS1, Q	// current
		#endif /* WITHRTS96 */

		// Slot S0, S4: Oldest sample (T-3)
		// Slot S1, S5: Old sample (T-2)
		// Slot S2, S6: Old sample (T-1)
		// Slot S3, S7: Newest sample (T-0)
		#define DMABUF32RXWFM0I	0		// WFM OLDEST
		#define DMABUF32RXWFM0Q	4		// WFM
		#define DMABUF32RXWFM1I	1		// WFM
		#define DMABUF32RXWFM1Q	5		// WFM
		#define DMABUF32RXWFM2I	2		// WFM
		#define DMABUF32RXWFM2Q	6		// WFM
		#define DMABUF32RXWFM3I	3		// WFM NEWEST
		#define DMABUF32RXWFM3Q	7		// WFM
		
		#define DMABUFSTEP32TX	2		// 2 - ������� ������ ������������� ��� ����� � DMA ������	- I/Q
		#define DMABUF32TXI	0		// TX, I
		#define DMABUF32TXQ	1		// TX, Q

		#define DMABUFSTEP16	2		// 2 - ������� ������ ������������� ��� ����� � DMA ������

	#endif

#else /* WITHDSPEXTDDC */
	// buff data layout: ADC data/unused channel
	#define DMABUF32RX		0		// ADC data index
	#define DMABUFSTEP32RX	(WITHSAI1_FRAMEBITS / 32) //2		// 2 - ������� ������ ������������� ��� ����� � DMA ������
	#define DMABUF32RXI	0		// RX0, I
	#define DMABUF32RXQ	1		// RX0, Q

	#define DMABUFSTEP32TX	2		// 2 - ������� ������ ������������� ��� ����� � DMA ������	- I/Q
	#define DMABUF32TXI	0		// TX, I
	#define DMABUF32TXQ	1		// TX, Q

	#define DMABUFSTEP16	2		// 2 - ������� ������ ������������� ��� ����� � DMA ������

#endif /* WITHDSPEXTDDC */


// ���������� �� ��������� ������� ������ ��� �������� �� USB DMA
#if CPUSTYLE_R7S721
	#define HARDWARE_RTSDMABYTES	4
#else /* CPUSTYLE_R7S721 */
	#define HARDWARE_RTSDMABYTES	1
#endif /* CPUSTYLE_R7S721 */

// ��������� ������� � ������ ���������� ������������
#define DMAHWEPADJUST(sz, granulation) (((sz) + ((granulation) - 1)) / (granulation) * (granulation))

/* ���������. � ������� ����� �������� � ��� �������� 125 ��� �� FPGA ��� ��������� 2560 = 48.828125 kHz sample rate */
//#define MSOUTSAMPLES	49 /* ���������� ������� �� ����������� � UAC OUT */
// ��� ������ - ������ ��� 48000
#define MSOUTSAMPLES	48 /* ���������� ������� �� ����������� � UAC OUT */
#define MSINSAMPLES		(MSOUTSAMPLES + 1) /* ���������� ������� �� ����������� � UAC IN */
// ��������� ������� �� ��������� - � ��� ������ 4 ������
#define DMABUFFSIZE16 (MSINSAMPLES * DMABUFSTEP16)	/* ������ ��� USB ENDPOINT PACKET SIZE � ������ ���������� ���� �������� - ������ ����� */

#define DMABUFCLUSTER	8	// C������ ��������� ���������� - ��� �������������� � �������������� ������������ �����

#define DMABUFFSIZE32RX (DMABUFCLUSTER * DMABUFSTEP32RX)
#define DMABUFFSIZE32TX (DMABUFCLUSTER * DMABUFSTEP32TX * 4)

// ��������� ��� ������ �������� Real Time Spectrum - stereo, 32 bit, 192 kS/S
#define DMABUFSTEP192RTS 8	// 8: ������ �� 32 ���, 6: ������ �� 24 ���
#define DMABUFFSIZE192RTS_AJ (128/*288*/ * DMABUFSTEP192RTS)

#define DMABUFFSIZE192RTS DMAHWEPADJUST(DMABUFFSIZE192RTS_AJ, DMABUFSTEP192RTS * HARDWARE_RTSDMABYTES)

#define DMABUFSTEP96RTS 6	// 6: ������ �� 24 ���
#define DMABUFFSIZE96RTS_AJ ((MSOUTSAMPLES * 2 + 1) * DMABUFSTEP96RTS) //((96 + 4) * DMABUFSTEP96RTS)		// 588 - ������ ���� ������ 4 ������ - ��� ������ DMA � Renesas

#define DMABUFFSIZE96RTS DMAHWEPADJUST(DMABUFFSIZE96RTS_AJ, DMABUFSTEP96RTS * HARDWARE_RTSDMABYTES)

#if WITHRTS96

	// stereo, 24 bit samples
	#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS96		24
	#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96		(DMABUFFSIZE96RTS * sizeof (uint8_t))

	#define HSINTERVAL_RTS96 4	// endpoint descriptor parameters
	#define FSINTERVAL_RTS96 1

#endif /* WITHRTS96 */
#if WITHRTS192

	// �� ������ real-time ������� ������, 32 ���, 192 ��� - 288*2*4 = 2304 �����
	// stereo, 32 bit samples
	#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS192	32
	#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192		(DMABUFFSIZE192RTS * sizeof (int8_t))

	#define HSINTERVAL_RTS192 3	// 500 us
	#define FSINTERVAL_RTS192 1

#endif /* WITHRTS192 */

#define HARDWARE_USBD_AUDIO_IN_CHANNELS_RTS		2	// I/Q ������ ������

// stereo, 16 bit samples
// �� ��������� ������ ���������� ������, 16 ���, 48 ��� - 288 ���� ������ ������ � ��������
#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_AUDIO48	16
#define HARDWARE_USBD_AUDIO_IN_CHANNELS_AUDIO48		2
#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_AUDIO48		(DMABUFFSIZE16 * sizeof (uint16_t))

/*
	For full-/high-speed isochronous endpoints, this value
	must be in the range from 1 to 16. The bInterval value
	is used as the exponent for a 2^(bInterval-1) value; e.g.,
	a bInterval of 4 means a period of 8 (2^(4-1))."

  */
#define HSINTERVAL_AUDIO48 4	// endpoint descriptor parameters - ��� ����������� 1 ��� �������
#define FSINTERVAL_AUDIO48 1

#define HSINTERVAL_8MS 7	// endpoint descriptor parameters - ��� ����������� 10 ms �������
#define FSINTERVAL_8MS 8

#define HSINTERVAL_32MS 9	// endpoint descriptor parameters - ��� ����������� 32 ms �������
#define FSINTERVAL_32MS 32

#define HSINTERVAL_256MS 12	// endpoint descriptor parameters - ��� ����������� 255 ms ������� (interrupt endpoint for CDC)
#define FSINTERVAL_255MS 255


#define HARDWARE_USBD_AUDIO_OUT_SAMPLEBITS	16
#if WITHUABUACOUTAUDIO48MONO
	#define HARDWARE_USBD_AUDIO_OUT_CHANNELS	1
#else /* WITHUABUACOUTAUDIO48MONO */
	#define HARDWARE_USBD_AUDIO_OUT_CHANNELS	2
#endif /* WITHUABUACOUTAUDIO48MONO */

// ������������ ���� ������
#define VIRTUAL_AUDIO_PORT_DATA_SIZE_OUT	( \
	MSOUTSAMPLES * \
	((HARDWARE_USBD_AUDIO_OUT_SAMPLEBITS * HARDWARE_USBD_AUDIO_OUT_CHANNELS + 7) / 8) \
	)

#define HARDWARE_USBD_AUDIO_IN_CHANNELS	2	/* ��� ���� ������� � IN ����������� */

#if WITHINTEGRATEDDSP

	#if (__ARM_FP & 0x08) && 0

		typedef double FLOAT_t;

		#define LOG10F	log10
		//#define LOGF	log
		//#define POWF	pow
		//#define LOG2F	log2
		#define LOGF	local_log
		#define POWF	local_pow
		#define LOG2F	local_log2
		#define SINF	sin
		#define COSF	cos
		#define ATAN2F	atan2
		#define ATANF	atan
		//#define EXPF	exp
		#define EXPF	local_exp
		#define FABSF	fabs
		#define SQRTF	sqrt
		#define FMAXF	fmax
		#define FMINF	fmin
		#define LDEXPF	ldexp
		#define FREXPF	frexp
		
		#if defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMA)
			#define FMAF	fma
		#endif /* defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMA) */
		#define DSP_FLOAT_BITSMANTISSA 54

	#elif (__ARM_FP & 0x04)

		typedef float FLOAT_t;

		#define LOG10F	log10f
		//#define LOGF	logf
		//#define POWF	powf
		//#define LOG2F	log2f
		#define LOGF	local_log
		#define POWF	local_pow
		#define LOG2F	local_log2
		#define SINF	sinf
		#define COSF	cosf
		#define ATAN2F	atan2f
		#define ATANF	atanf
		//#define EXPF	expf
		#define EXPF	local_exp
		#define FABSF	fabsf
		#define SQRTF	sqrtf
		#define FMAXF	fmaxf
		#define FMINF	fminf
		#define LDEXPF	ldexpf
		#define FREXPF	frexpf

		#if defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMAF)
			#define FMAF	fmaf
		#endif /* defined (__ARM_FEATURE_FMA) || defined (FP_FAST_FMAF) */
		#define DSP_FLOAT_BITSMANTISSA 24

	#else

		#error This CPU not support floating point

	#endif


/* from "C Language Algorithms for Digital Signal Processing"
   by Paul M. Embree and Bruce Kimble, Prentice Hall, 1991 */

	
struct Complex
{
	FLOAT_t real;
	FLOAT_t imag;
};

#if CPUSTYLE_R7S721 || CPUSTYLE_STM32H7XX
	#define FFTSizeFiltersM 10
	#define FFTSizeSpectrumM 10
#else
	#define FFTSizeFiltersM 10
	#define FFTSizeSpectrumM 10
#endif


#define FFTSizeFilters (1 << (FFTSizeFiltersM))
#define FFTSizeSpectrum (1 << (FFTSizeSpectrumM))


void FFT_initialize(void);
void FFT(struct Complex *x, int n, int m);
void IFFT(struct Complex *x, int n, int m);

	/* ��� ����������� ������ � ��������� ������������ NEON - vld1_f32 �������� */
	#define IV ivqv [0]
	#define QV ivqv [1]

	typedef struct
	{
		FLOAT_t ivqv [2];
	} FLOAT32P_t;

	typedef struct
	{
		int_fast32_t ivqv [2];
	} INT32P_t;


uint_fast8_t modem_getnextbit(
	uint_fast8_t suspend	// ���������� ������ ��� ���� - �� ��������� ��������� ������������ ���������
	 );
// ��������� ���� �� �����
void
modem_frames_decode(
	uint_fast8_t v
	);
	uint_fast8_t getsampmlemike(INT32P_t * v);			/* �������� ��������� ����������� ����� � ��������� */

	FLOAT_t local_log(FLOAT_t x);
	FLOAT_t local_pow(FLOAT_t x, FLOAT_t y);

#endif /* WITHINTEGRATEDDSP */


// Buffers interface functions
void buffers_initialize(void);

uint_fast8_t processmodem(void);

uintptr_t allocate_dmabuffer32tx(void);
uintptr_t allocate_dmabuffer32rx(void);
void release_dmabuffer32tx(uintptr_t addr);

uintptr_t allocate_dmabuffer16(void);
void release_dmabuffer16(uintptr_t addr);

uintptr_t getfilled_dmabufferx(uint_fast16_t * sizep);	/* �������� ����� ������ �� �����, ������� ����� �������������� ��� �������� ����������� � ��������� �� USB */
uintptr_t getfilled_dmabufferxrts(uint_fast16_t * sizep);	/* �������� ����� ������ �� �����, ������� ����� �������������� ��� �������� ����������� � ��������� �� USB */
void release_dmabufferx(uintptr_t addr);	/* ���������� ����� ������ �� �����, ������� ����� �������������� ��� �������� ����������� � ��������� �� USB */

void refreshDMA_uacin(void); // ����� DMA ��� ����� - ��������� � �������, ����� �������� ������ ����� getfilled_dmabufferx

uintptr_t getfilled_dmabuffer32tx_main(void);
uintptr_t getfilled_dmabuffer32tx_sub(void);
uintptr_t getfilled_dmabuffer16phones(void);

void dsp_extbuffer32rx(const uint32_t * buff);	// RX
void dsp_extbuffer32wfm(const uint32_t * buff);	// RX

void processing_dmabuffer16rx(uintptr_t addr);	// ���������� ����� ����� ��������� AF ADC
void processing_dmabuffer16rxuac(uintptr_t addr);	// ���������� ����� ����� ����� ������ � USB AUDIO
void processing_dmabuffer32rx(uintptr_t addr);
void processing_dmabuffer32rts(uintptr_t addr);
void processing_dmabuffer32wfm(uintptr_t addr);

void savesamplerecord16SD(int_fast16_t ch0, int_fast16_t ch1); /* to SD CARD */
void savesamplerecord16uacin(int_fast16_t ch0, int_fast16_t ch1); /* to USB AUDIO */
unsigned takerecordbuffer(void * * dest);
void releaserecordbuffer(void * dest);

// ������������ ������
size_t takemodemtxbuffer(uint8_t * * dest);	// ������ � ������� ��� �������� ����� �����
size_t takemodemtxbuffer_low(uint8_t * * dest);	// ������ � ������� ��� �������� ����� �����
uint_fast8_t statusmodemtxbuffer(void);		// ���� �� ������ ��� ��������
size_t takemodemrxbuffer(uint8_t * * dest);	// ������ � ���������� ����� ����� �������
size_t takemodembuffer(uint8_t * * dest);	// ������ ��� ���������� �������
size_t takemodembuffer_low(uint8_t * * dest);	// ������ ��� ���������� �������
void savemodemrxbuffer(uint8_t * dest, unsigned size_t);	// ����� ����� � ��������� �������
void savemodemrxbuffer_low(uint8_t * dest, unsigned size_t);	// ����� ����� � ��������� �������
void savemodemtxbuffer(uint8_t * dest, unsigned size_t);	// ����� ����� � ������� ��� ��������
void releasemodembuffer(uint8_t * dest);
void releasemodembuffer_low(uint8_t * dest);

void savesampleout16stereo(int_fast16_t ch0, int_fast16_t ch1);
void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1);
void savesampleout96stereo(int_fast32_t ch0, int_fast32_t ch1);
void savesampleout192stereo(int_fast32_t ch0, int_fast32_t ch1);

uint32_t allocate_dmabuffer192rts(void);

int get_lout16(void); // �������� �������
int get_rout16(void);

void prog_dsplreg(void);
void prog_fltlreg(void);
void prog_codec1reg(void);
void prog_codecreg_update(void);		// ��������� ���������� ��������� ����� ������
void prog_dsplreg_update(void);
void prog_fltlreg_update(void);
void board_dsp1regchanged(void);
void prog_codec1reg(void);


void board_set_trxpath(uint_fast8_t v);	/* �����, � �������� ��������� ��� ����������� ������. ��� ���������� ������������ ������ 0 */
void board_set_mikemute(uint_fast8_t v);	/* ��������� ����������� ��������� */
void board_set_mik1level(uint_fast16_t v);	/* �������� ������������ ��������� */
void board_set_agcrate(uint_fast8_t v);	/* �� n ������� ��������� �������� ������� 1 �� ���������. UINT8_MAX - "�������" ��� */
void board_set_agc_t1(uint_fast8_t v);	/* ���������� ��������� ��� */
void board_set_agc_t2(uint_fast8_t v);	/* ���������� ��������� ��� */
void board_set_agc_t4(uint_fast8_t v);	/* ���������� ��������� ��� */
void board_set_agc_thung(uint_fast8_t v);	/* ���������� ��������� ��� */
void board_set_nfm_sql_lelel(uint_fast8_t v);	/* ������� ���������� �������������� NFM */
void board_set_squelch(uint_fast8_t v);	/* ������� ���������� �������������� */
void board_set_nfm_sql_off(uint_fast8_t v);	/* ���������� �������������� NFM */
void board_set_notch_freq(uint_fast16_t n);	/* ������� NOTCH ������� */
void board_set_notch_width(uint_fast16_t n);	/* ������ NOTCH ������� */
void board_set_notch_on(uint_fast8_t v);	/* ��������� NOTCH ������� */
void board_set_cwedgetime(uint_fast8_t n);	/* ����� ����������/����� ��������� ��������� ��� �������� - � 1 �� */
void board_set_sidetonelevel(uint_fast8_t n);	/* ������� ������� ������������ � ��������� - 0%..100% */
void board_set_subtonelevel(uint_fast8_t n);	/* ������� ������� CTCSS � ��������� - 0%..100% */
void board_set_amdepth(uint_fast8_t n);		/* ������� ��������� � �� - 0..100% */
void board_set_swapiq(uint_fast8_t v);	/* �������� ������� I � Q ������ � ������ RTS96 */
void board_set_swaprts(uint_fast8_t v);	/* ���� ������������ ��������� �� Rafael Micro R820T - ��������� �������� ������� */
void buffers_set_uacinalt(uint_fast8_t v);	/* ����� �������������� ������������ ��� UAC IN interface */
void buffers_set_uacoutalt(uint_fast8_t v);	/* ����� �������������� ������������ ��� UAC OUT interface */
void buffers_set_uacinrtsalt(uint_fast8_t v);	/* ����� �������������� ������������ ��� UAC IN interface */
void board_set_lo6(int_fast32_t f);
void board_set_fullbw6(int_fast16_t f);	/* ��������� ������� ����� �������� �� � ��������� ������ - �������� ������ ������ ����������� */

void board_set_aflowcutrx(int_fast16_t v);		/* ������ ������� ����� ������� �� */
void board_set_afhighcutrx(int_fast16_t v);	/* ������� ������� ����� ������� �� */
void board_set_aflowcuttx(int_fast16_t v);		/* ������ ������� ����� ������� �� */
void board_set_afhighcuttx(int_fast16_t v);	/* ������� ������� ����� ������� �� */

void board_set_afgain(uint_fast16_t v);	// �������� ��� ����������� ������ �� ������ �����-���
void board_set_ifgain(uint_fast16_t v);	// �������� ��� ����������� �������� ��/��
void board_set_dspmode(uint_fast8_t v);	// �������� ��� ��������� ������ ������ �������� A/����������� A
void board_set_lineinput(uint_fast8_t n);	// ��������� line input ������ ���������
void board_set_lineamp(uint_fast16_t v);	// �������� ��� ����������� ������ �� ����� �����-��� ��� ������ � LINE IN
void board_set_txaudio(uint_fast8_t v);	// �������������� ��������� ������� ��� ��������
void board_set_mikebust20db(uint_fast8_t n);	// ��������� ������������� �� ����������
void board_set_afmute(uint_fast8_t n);	// ���������� �����
void board_set_mikeequal(uint_fast8_t n);	// ��������� ��������� ������� � ��������� (�������, ����������, ...)
void board_set_mikeequalparams(const uint_fast8_t * p);	// ���������� 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
void board_set_mikeagc(uint_fast8_t n);		/* ��������� ����������� ��� ����� ����������� */
void board_set_mikeagcgain(uint_fast8_t v);	/* ������������ �������� ��� ��������� */
void board_set_afresponcerx(int_fast8_t v);	/* ��������� ������ ����� � ������ ��������� */
void board_set_afresponcetx(int_fast8_t v);	/* ��������� ������ ����� � ������ ����������� */
void board_set_mikehclip(uint_fast8_t gmikehclip);	/* ������������ */

void board_set_uacplayer(uint_fast8_t v);	/* ����� ������������� ������ ���������� � ��������� ���������� - ���������� ����� */
void board_set_uacmike(uint_fast8_t v);	/* �� ���� ���������� ������� ����������� � USB ����������� �����, � �� � ��������� */

void dsp_initialize(void);

#if WITHINTEGRATEDDSP
	// ���������� ���������� � ������� � ������� ������ ������ 
	// wfarray (�������������� � �������� ������ */
	void dsp_getspectrumrow(
		FLOAT_t * const hbase,
		uint_fast16_t dx	// pixel X width of display window
		);
	// ������������ ������ ������� � �����
	// ���������� �������� �� 0 �� dy ������������
	int dsp_mag2y(FLOAT_t mag, uint_fast16_t dy);	
#endif /* WITHINTEGRATEDDSP */

int_fast32_t dsp_get_ifreq(void);		/* �������� �������� ������� �� ��� ��������� DSP */
int_fast32_t dsp_get_sampleraterx(void);	/* �������� �������� ������� ������� ��������� ������ DSP */
int_fast32_t dsp_get_sampleraterxscaled(uint_fast8_t scale);	/* �������� �������� ������� ������� ��������� ������ DSP */
int_fast32_t dsp_get_sampleratetx(void);	/* �������� �������� ������� ������� �������� ������ DSP */
int_fast32_t dsp_get_samplerate100(void);	/* �������� �������� ������� ������� ��������� ������ DSP */

int_fast32_t dsp_get_samplerateuacin_audio48(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_rts96(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_rts192(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_rts(void);			// RTS samplerate
int_fast32_t dsp_get_samplerateuacout(void);			// UAC OUT samplerate

uint_fast8_t dsp_getsmeter(uint_fast8_t * tracemax, uint_fast8_t lower, uint_fast8_t upper, uint_fast8_t clean);	/* �������� �������� �� ��� s-����� */
uint_fast8_t dsp_getvox(uint_fast8_t fullscale);	/* �������� �������� �� ��������� VOX */
uint_fast8_t dsp_getavox(uint_fast8_t fullscale);	/* �������� �������� �� ��������� Anti-VOX */
uint_fast8_t dsp_getfreqdelta10(int_fast32_t * p, uint_fast8_t pathi);	/* �������� �������� ���������� ������� � ��������� 0.1 ����� */
uint_fast8_t dsp_getmikeadcoverflow(void); /* ��������� �������� ������������ ��� ������������ ������ */

void dsp_speed_diagnostics(void);	/* DSP speed test */
void buffers_diagnostics(void);
void usbd_showstate(void);
void usbd_diagnostics(void);
void dtmftest(void);

void modem_initialze(void);
uint_fast8_t modem_get_ptt(void);

/* ��������� � AF ������ */
typedef struct codec1if_tag
{
	void (* initialize)(void);
	void (* setvolume)(uint_fast16_t gain, uint_fast8_t mute, uint_fast8_t mutespk);	/* ��������� ��������� �� �������� */
	void (* setlineinput)(uint_fast8_t linein, uint_fast8_t mikebust20db, uint_fast16_t mikegain, uint_fast16_t linegain);	/* ����� LINE IN ��� ��������� ��� ��� ������ ��������� */
	void (* setprocparams)(uint_fast8_t procenable, const uint_fast8_t * gains);	/* ��������� ��������� ����� � ��������� (���, ����������, ...) */
	const char * label;									/* �������� ������ (������ ��������� ������� � ���������) */
} codec1if_t;

const codec1if_t * board_getaudiocodecif(void);		// �������� ��������� ���������� ������� � ������� ���������

/* ��������� � IF ������ */
typedef struct codec2if_tag
{
	void (* initialize)(void);
	const char * label;
} codec2if_t;

const codec2if_t * board_getfpgacodecif(void);		// �������� ��������� ���������� ������� ��� ��������� ������ � ������������

/* +++ UAC OUT data save */
void uacout_buffer_initialize(void);
void uacout_buffer_start(void);
void uacout_buffer_stop(void);
void uacout_buffer_save(const uint8_t * buff, uint_fast16_t size);


#define USBALIGN_BEGIN __attribute__ ((aligned (32)))
#define USBALIGN_END /* nothing */

#endif /* AUDIO_H_INCLUDED */
