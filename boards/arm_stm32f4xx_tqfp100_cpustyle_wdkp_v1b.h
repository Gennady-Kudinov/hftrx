/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

// ����� DSP ��������� "�����-2" �� ���������� STM32F429 � �������� TLV320AIC23B � CS4272

#ifndef ARM_STM32F4XX_TQFP100_CPUSTYLE_WDKP_V1B_H_INCLUDED
#define ARM_STM32F4XX_TQFP100_CPUSTYLE_WDKP_V1B_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0:
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA9/PA10 pins

#define WITHSPI16BIT	1		/* �������� ������������� 16-�� ������ ���� ��� ������ �� SPI */
#define WITHSPIHW 		1	/* ������������� ����������� ����������� SPI */
#define WITHSPIHWDMA 	1	/* ������������� DMA ��� ������ �� SPI */
#define WITHSPISW 	1	/* ������������� ������������ ���������� SPI. ������ ������� ��� ������ - ��������� ����� ���������� ��-�� ��������� � I2C */

#define WITHTWIHW 	1	/* ������������� ����������� ����������� TWI (I2C) */
//#define WITHTWISW 	1	/* ������������� ������������ ����������� TWI (I2C) */

#define WITHI2SHW	1	/* ������������� I2S - ����������	*/
#define WITHSAI1HW	1	/* ������������� SAI1 - FPGA ��� IF codec	*/

#define WITHCPUDACHW	1	/* ������������� DAC */
#define WITHCPUADCHW 	1	/* ������������� ADC */

#define WITHUART1HW	1	/* ������������ ������������ ���������� ����������������� ����� #1 */
//#define WITHUART2HW	1	/* ������������ ������������ ���������� ����������������� ����� #2 */

//#define WITHCAT_CDC		1	/* ������������ ����������� ���������������� ���� �� USB ���������� */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1	1

//#define WITHUSBHW	1	/* ������������ ���������� � ��������� ��������� USB */
//#define WITHUSBHWVBUSSENSE	1	/* ������������ ���������������� ����� VBUS_SENSE */
//#define WITHUSBHWHIGHSPEED	1	/* ������������ ���������� � ��������� ��������� USB HS */
//#define WITHUSBHWHIGHSPEEDDESC	1	/* ��������� ����������� ����������� ��� ��� HIGH SPEED */
//#define WITHUSBHW_DEVICE	USB_OTG_FS	/* �� ���� ���������� �������������� ���������������� DEVUCE	*/

//#define BSRR_S(v) ((v) * GPIO_BSRR_BS_0)	/* �������������� �������� ��� ��������� ���� � �������� */
//#define BSRR_C(v) ((v) * GPIO_BSRR_BR_0)	/* �������������� �������� ��� ������ ���� � �������� */

#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(LS020_RST, LS020_RST); \
	} while (0)


#if LCDMODE_SPI_NA || LCDMODE_SPI_RA
	// ��� ����������� ������� ������ RS
	// ��� ����������� ������� RESET � RS

	#define LS020_RS_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			0//(1u << 9)			// PC9

#endif

#if LCDMODE_UC1608
	// LCDMODE_SPI_RA already defined in paramdepend.h

	#define UC1608_CSP_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define UC1608_CSP_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define UC1608_CSP			0//(1u << 13)			// PC13
	#define SPI_CSEL255			255				// �� ����� ���������� �������� ������������� ��������

	#define UC1608_CSP_INITIALIZE() do { \
			arm_hardware_piod_outputs(UC1608_CSP, 0); \
		} while (0)
#endif


#if WITHENCODER
	// ������ ����������� ��������
	// Encoder inputs: PC15 - PHASE A, PC14 = PHASE B
	// ����������� ����� (����� ���������� �������������).

	#define ENCODER_INPUT_PORT			(GPIOC->IDR) 
	#define ENCODER_BITS ((1u << 15) | (1u << 14))
	#define ENCODER_SHIFT 14

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_pioc_inputs(ENCODER_BITS); \
			arm_hardware_pioc_updown(ENCODER_BITS, 0); \
			arm_hardware_pioc_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY); \
		} while (0)

#endif

#if WITHI2SHW
	#define I2S2HW_INITIALIZE() \
		do { \
			enum { \
				I2S2_MCK = (1U << 6),		/* PC6	*/ \
				I2S2ext_SD = (1U << 14),	/* PB14	- ���� */ \
				I2S2_WS = (1U << 12),		/* PB12	*/ \
				I2S2_CK = (1U << 13),		/* PB13	*/ \
				I2S2_SD = (1U << 15)		/* PB15	*/ \
			}; \
			arm_hardware_piob_altfn2(I2S2_WS, AF_SPI2); \
			arm_hardware_piob_altfn2(I2S2_CK, AF_SPI2); \
			arm_hardware_piob_altfn2(I2S2_SD, AF_SPI2); \
			arm_hardware_pioc_altfn20(I2S2_MCK, AF_SPI2); \
			arm_hardware_piob_altfn2(I2S2ext_SD, AF_SPI2ext); \
		} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI1HW
	#define SAI1HW_INITIALIZE() \
		do { \
			arm_hardware_pioe_altfn20(1U << 2, AF_SAI);		/* PE2 - SAI1_MCK_A - 12.288 MHz	*/ \
			arm_hardware_pioe_altfn20(1U << 3, AF_SAI);		/* PE3 - SAI1_SD_B	(i2s data from codec)	*/ \
			arm_hardware_pioe_altfn2(1U << 4, AF_SAI);		/* PE4 - SAI1_FS_A	- 48 kHz	*/ \
			arm_hardware_pioe_altfn20(1U << 5, AF_SAI);		/* PE5 - SAI1_SCK_A	*/ \
			arm_hardware_pioe_altfn20(1U << 6, AF_SAI);		/* PE6 - SAI1_SD_A	(i2s data to codec)	*/ \
		/* #if CODEC_TYPE_CS4272_STANDALONE	*/ \
			arm_hardware_pioe_updown(0, 1U << 6);	/* pull-down �� ������ ������ �� ������: ����� RESET ���� � stand-alone mode */ \
		/* #endif */ \
		} while (0)
#endif /* WITHSAI1HW */

/* ������������� ����� � ARM ������������ */

#if WITHCAT || WITHNMEA
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) // was PINA 
	#define FROMCAT_BIT_RTS				(1u << 11)	/* ������ RTS �� FT232RL	*/

	/* ����������� �� ����� RS-232, ������ PPS �� GPS/GLONASS/GALILEO ������ */
	#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR) // was PINA 
	#define FROMCAT_BIT_DTR				(1u << 12)	/* ������ DTR �� FT232RL	*/
#endif

#if WITHTX

	// txpath outputs
	#define TXPATH_TARGET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TXPATH_TARGET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	#if 1
		// ���������� ������������ - ������� TXPATH_ENABLE_SSB (PC7) � TXPATH_ENABLE_CW (PC8) - ������� ��� ���� �� ������.
		#define TXPATH_BIT_ENABLE_SSB		(1u << 9)		// PD9
		#define TXPATH_BIT_ENABLE_CW		(1u << 10)		// PD10

		// �������������� ����������� �����
		#define TXGFV_RX		0
		#define TXGFV_TRANS		0			// ������� ����� �������� ����� � ��������
		#define TXGFV_TX_SSB	TXPATH_BIT_ENABLE_SSB
		#define TXGFV_TX_CW		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_AM		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_NFM	TXPATH_BIT_ENABLE_CW
		// ���������� ������������ - ������� TXPATH_ENABLE (PA11) � TXPATH_ENABLE_CW (PA10) - ������� ��� ���� �� ������.
		// ��������� ���������� ��������� �������
		#define TXPATH_INITIALIZE() \
			do { \
				arm_hardware_piod_opendrain(TXPATH_BITS_ENABLE, TXPATH_BITS_ENABLE); \
			} while (0)
	#else
		// ���������� ������������ - ������������ ������ ���������� ������
		#define TXPATH_BIT_GATE (1u << 9)	// PD9 - �������� ������ �� ���������� - ���������� ������������.
		//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// ������ tx2 - ���������� ������������. ��� ����� �������
		#define TXPATH_BIT_GATE_RX 0	// ������ tx2 - ���������� ������������. ��� ����� �� �������

		// �������������� ����������� �����
		#define TXGFV_RX		TXPATH_BIT_GATE_RX
		#define TXGFV_TRANS		0			// ������� ����� �������� ����� � ��������
		#define TXGFV_TX_SSB	TXPATH_BIT_GATE
		#define TXGFV_TX_CW		TXPATH_BIT_GATE
		// ���������� ������������ - ������ TXPATH_BIT_GATE
		// ��������� ���������� ��������� �������
		#define TXPATH_INITIALIZE() \
			do { \
				arm_hardware_piod_outputs2m(TXPATH_BIT_GATE, 0); \
			} while (0)
	#endif

	// PTT input - PC9
	#define PTT_TARGET_PIN				(GPIOC->IDR)
	#define PTT_BIT_PTT					(1U << 9)		// PC9

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_pioc_inputs(PTT_BIT_PTT); \
			arm_hardware_pioc_updown(PTT_BIT_PTT, 0); \
		} while (0)

#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_TARGET_PIN			(GPIOC->IDR)
	#define ELKEY_BIT_LEFT				(1U << 7)		// PC7
	#define ELKEY_BIT_RIGHT				(1U << 8)		// PC8

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_pioc_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_pioc_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */


// IOUPDATE = P�13
#define SPI_IOUPDATE_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_IOUPDATE_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_IOUPDATE_BIT		(1U << 13)	// PC13 - ������ ��� �����, �����������, 3.3 ������

/* ������������� ������� IOUPDATE �� DDS */
/* ��������� ��������� = 1 */
#define SPI_IOUPDATE_INITIALIZE() \
	do { \
		arm_hardware_pioc_outputs2m(SPI_IOUPDATE_BIT, SPI_IOUPDATE_BIT); \
	} while (0)

#define TARGET_CS4272_RESET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
#define TARGET_CS4272_RESET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
#define TARGET_CS4272_RESET_BIT		(1U << 2)	// PD2

#define CS4272_RESET_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(TARGET_CS4272_RESET_BIT, TARGET_CS4272_RESET_BIT); \
	} while (0)

// ����� ����������� ��� ������ ��� �������� �����������
#define SPI_ALLCS_PORT_S(v)	do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_ALLCS_PORT_C(v)	do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)

//	#define targetdds1 SPI_CSEL0 	/* DDS1 */
//	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
//	#define targetdds3 SPI_CSEL2 	/* DDS3 - PBT output */
//	//#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. ��� ������� DDS ������� ���������� - ������ DDS */
//	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
//	#define targetext1 SPI_CSEL4 	/* external devices control */
//#ifndef SPI_IOUPDATE_BIT 
//	#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
//#endif
//	#define targetctl1 SPI_CSEL6 	/* control register */
//	#define targetnvram SPI_CSEL7  	/* serial nvram */

#define SPI_CSEL7	(1U << 0)	// PD0 on-board nvmem FM25L16B
#define SPI_CSEL2	(1U << 1)	// PD1 on-board codec2 CS4272
#define SPI_CSEL3	(1U << 3)	// PD3 on-board codec1 TLV320AIC23B
#define SPI_CSEL0	(1U << 4)	// PD4 ext
#define SPI_CSEL1	(1U << 5)	// PD5 ext
#define SPI_CSEL4	(1U << 6)	// PD6 ext
#define SPI_CSEL5	(1U << 7)	// PD7 ext
#define SPI_CSEL6	(1U << 8)	// PD8 ext
// ����� ������ ���� ����������� ��� ���� ������������ CS  ����������.
#define SPI_ALLCS_BITS (SPI_CSEL0 | SPI_CSEL1 | SPI_CSEL2 | SPI_CSEL3 | SPI_CSEL4 | SPI_CSEL5 | SPI_CSEL6 | SPI_CSEL7)

/* ������������� ���� ������ ������������ ��������� */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(SPI_ALLCS_BITS, SPI_ALLCS_BITS); \
	} while (0)

// MOSI & SCK port
#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)

#define	SPI_SCLK_BIT			(1U << 3)	// PB3 ���, ����� ������� ���� ������������� SPI
#define	SPI_MOSI_BIT			(1U << 5)	// PB5 ���, ����� ������� ���� ����� (��� ���� � ������ ���������������� SPI).

#define SPI_TARGET_MISO_PIN		(GPIOB->IDR)		// was PINA 
#define	SPI_MISO_BIT			(1U << 4)	// PB4 ���, ����� ������� ���� ���� � SPI.

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
			arm_hardware_piob_inputs(SPI_MISO_BIT); \
		} while (0)

#if WITHSPISLAVE
	// SPI SLAVE �� dspboard_v1
	// SPI control pins
	// SPI3 hardware used.

	// MOSI & SCK port
	// STM32F303: SPI1_NSS can be placed on PA4 or PA15

	//#define SPI_TARGET_PIN			GPIOB->IDR // was PINA 
	#define	SPI_SLAVE_SCLK_BIT			(1U << 10)	// PC10 ���, ����� ������� ���� ������������� SPI
	#define	SPI_SLAVE_MISO_BIT			(1U << 11)	// PC11 ���, ����� ������� ���� ���� � SPI.
	#define	SPI_SLAVE_MOSI_BIT			(1U << 12)	// PC12 ���, ����� ������� ���� ����� (��� ���� � ������ ���������������� SPI).

	#define SPI_SLAVE_NSS				(1U << 15)	// PA15 used

	// � ���� ����������� � ����� � ������ ������������� �� ALT FN
	// ����� � ������� master �� ������������ - �� ����������

	#define HARDWARE_SPISLAVE_INITIALIZE() do { \
		arm_hardware_pioc_altfn50(SPI_SLAVE_MOSI_BIT | SPI_SLAVE_SCLK_BIT /*| SPI_SLAVE_MISO_BIT*/, AF_SPI3); \
		arm_hardware_pioa_altfn50(SPI_SLAVE_NSS, AF_SPI3);	/* PA15 */ \
		arm_hardware_pioa_onchangeinterrupt(SPI_SLAVE_NSS, SPI_SLAVE_NSS, 0, ARM_SYSTEM_PRIORITY); \
	} while (0)
#endif /* WITHSPISLAVE */


////#define SIDETONE_TARGET_BIT		(1u << 8)	// (PB8, base mapping) output TIM4_CH3 

#if 1 // WITHTWISW
	#define TARGET_TWI_TWCK_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWD_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWCK		(1u << 6)		// * PB6
	#define TARGET_TWI_TWD		(1u << 7)		// * PB6

	// ������������� ����� ������ �����-������ ��� ����������� ���������� I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_piob_opendrain(TARGET_TWI_TWCK | TARGET_TWI_TWD, TARGET_TWI_TWCK | TARGET_TWI_TWD); \
		} while (0) 
	// ������������� ����� ������ �����-������ ��� ���������� ���������� I2C
	// ������������� ������� � ������������� ����������
	#if CPUSTYLE_STM32F1XX

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, 255);	/* AF=4 */ \
			} while (0) 

	#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C1);	/* AF=4 */ \
			} while (0) 

	#endif

#endif // WITHTWISW


#if WITHCPUDACHW
	/* �������� ������ ������ */
	#define HARDWARE_DAC_INITIALIZE() do { \
			DAC1->CR = DAC_CR_EN2 | DAC_CR_EN1; /* DAC2 and DAC1 enable */ \
		} while (0)
	#define HARDWARE_DAC_AGC(v) do { \
			DAC1->DHR12R1 = (v);	/* DAC1 set value */ \
		} while (0)
	#define HARDWARE_DAC_ALC(v) do { \
			DAC1->DHR12R2 = (v); /* DAC2 set value */ \
		} while (0)
#endif /* WITHCPUDACHW */

#if WITHCPUADCHW
	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pioa_analoginput(((ainmask) >> 0) & 0xff);	/* ADC12_IN0..ADC12_IN7 */ \
			arm_hardware_piob_analoginput(((ainmask) >> 8) & 0x03);	/* ADC12_IN8..ADC12_IN0 */ \
			arm_hardware_pioc_analoginput(((ainmask) >> 10) & 0x3f);	/* ADC12_IN10..ADC12_IN15 */ \
		} while (0)
#endif /* WITHCPUADCHW */

#if WITHUART1HW
	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pioa_altfn2((1U << 9) | (1U << 10), AF_USART1); /* PA9: TX DATA line (2 MHz), PA10: RX data line */ \
			arm_hardware_pioa_updown((1U << 10), 0);	/* PA10: pull-up RX data */ \
		} while (0)
#endif /* WITHUART1HW */


	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT, AF_SPI1); /* � ���� ����������� � ����� � ������ ������������� �� ALT FN */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_piob_outputs(SPI_SCLK_BIT | SPI_MOSI_BIT, SPI_SCLK_BIT | SPI_MOSI_BIT); /* connect back to GPIO */ \
			arm_hardware_piob_inputs(SPI_MISO_BIT); /* connect back to GPIO */ \
		} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT, AF_SPI1);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)
	#define HARDWARE_SIDETONE_INITIALIZE() do { \
		arm_hardware_piob_altfn2(SIDETONE_TARGET_BIT, AF_TIM4);	/* output TIM4_CH3 (PB8, base mapping) */ \
		} while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioa_inputs(KBD_MASK); \
			} while (0)
	#endif

#endif /* ARM_STM32F4XX_TQFP100_CPUSTYLE_WDKP_V1B_H_INCLUDED */
