/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

// ���������� ���������� RA4YBO V3
// "������������ � ��������" STM32F103C8T6, 2*HMC830, up-conversion to 64455 ��� RA4YBO

#ifndef ARM_STM32FXXX_TQFP48_CPUSTYLE_RA4YBO_V3_INCLUDED
#define ARM_STM32FXXX_TQFP48_CPUSTYLE_RA4YBO_V3_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0: PA5/PA6 pins
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA21/PA22 pins

#define WITHSPI16BIT	1		/* �������� ������������� 16-�� ������ ���� ��� ������ �� SPI */
#define WITHSPIHW 		1	/* ������������� ����������� ����������� SPI */
#define WITHSPIHWDMA 	1	/* ������������� DMA ��� ������ �� SPI */
#define WITHSPISW 	1	/* ������������� ������������ ���������� SPI. ������ ������� ��� ������ - ��������� ����� ���������� ��-�� ��������� � I2C */

#define WITHTWIHW 	1	/* ������������� ����������� ����������� TWI (I2C) */
//#define WITHTWISW 	1	/* ������������� ������������ ����������� TWI (I2C) */
#define WITHCPUADCHW 	1	/* ������������� ADC */

//#define WITHUART1HW	1	/* ������������ ������������ ���������� ����������������� ����� #1 */
//#define WITHUART2HW	1	/* ������������ ������������ ���������� ����������������� ����� #2 */

//#define WITHCAT_CDC		1	/* ������������ ����������� ���������������� ���� �� USB ���������� */
//#define WITHCAT_USART1		1
//#define WITHDEBUG_USART1	1
//#define WITHMODEM_USART1	1
//#define WITHNMEA_USART1		1


#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_pioc_outputs2m(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_pioc_outputs2m(LS020_RST, LS020_RST); \
	} while (0)


#if LCDMODE_SPI_RN
	// ��� ����������� ������� ������ RESET
	#define LS020_RESET_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RST			(1u << 8)			// D6 signal in HD44780 socket

#elif LCDMODE_SPI_NA
	// ��� ����������� ������� ������ RS
	#define LS020_RS_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 9)			// D7 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	// ��� ����������� ������� RESET � RS
	#define LS020_RESET_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RST			(1u << 8)			// D6 signal in HD44780 socket

	#define LS020_RS_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 9)			// D7 signal in HD44780 socket

#endif


#if WITHENCODER

	// ������ ����������� ��������

	//#define ENCODER_DIRECTION_PORT_S			GPIOA->PIO_OER	// was DDRA
	//#define ENCODER_DIRECTION_PORT_C			GPIOA->PIO_ODR	// was DDRA
	//#define KBD_TARGET_DDR			GPIOA->PIO_OSR	// was DDRA
	// Encoder inputs: PB15 - PHASE A, PB14 = PHASE B
	#define ENCODER_INPUT_PORT			(GPIOB->IDR)
	#define ENCODER_BITS ((1u << 15) | (1u << 14))
	#define ENCODER_SHIFT 14

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_piob_inputs(ENCODER_BITS); \
			arm_hardware_piob_updown(ENCODER_BITS, 0); \
			arm_hardware_piob_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY); \
		} while (0)

#endif
/* ������������� ����� � ARM ������������ */

#if WITHTX

	// txpath outputs

	//#define TXPATH_TARGET_DDR_S			GPIOF->PIO_OER	// was DDRA
	//#define TXPATH_TARGET_DDR_C		GPIOD->PIO_ODR	// was DDRA
	#define TXPATH_TARGET_PORT_S(v)		do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TXPATH_TARGET_PORT_C(v)		do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#if 1

		// ���������� ������������ - ������� TXPATH_ENABLE (PB12) � TXPATH_ENABLE_CW (PB13) - ������� ��� ���� �� ������.
		#define TXPATH_BIT_ENABLE_SSB		(1u << 13) 
		#define TXPATH_BIT_ENABLE_CW		(1u << 12)
		#define TXPATH_BITS_ENABLE	(TXPATH_BIT_ENABLE_SSB | TXPATH_BIT_ENABLE_CW)

		// �������������� ����������� �����
		#define TXGFV_RX		0
		#define TXGFV_TRANS		0			// ������� ����� �������� ����� � ��������
		#define TXGFV_TX_SSB	(TXPATH_BIT_ENABLE_SSB)
		#define TXGFV_TX_CW		(TXPATH_BIT_ENABLE_CW | TXPATH_BIT_ENABLE_SSB)
		#define TXGFV_TX_AM		(TXPATH_BIT_ENABLE_SSB)
		#define TXGFV_TX_NFM	(TXPATH_BIT_ENABLE_SSB)

		// ���������� ������������ - ������� TXPATH_ENABLE (PA11) � TXPATH_ENABLE_CW (PA10) - ������� ��� ���� �� ������.
		// ��������� ���������� ��������� �������
		#define TXPATH_INITIALIZE() \
			do { \
				arm_hardware_piob_opendrain(TXPATH_BITS_ENABLE, TXPATH_BITS_ENABLE); \
			} while (0)

	#else
		// ���������� ������������ - ������������ ������ ���������� ������
		#define TXPATH_BIT_GATE (1u << 12)	// �������� ������ �� ���������� - ���������� ������������.
		//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// ������ tx2 - ���������� ������������. ��� ����� �������
		#define TXPATH_BIT_GATE_RX 0	// ������ tx2 - ���������� ������������. ��� ����� �� �������

		// �������������� ����������� �����
		#define TXGFV_RX		TXPATH_BIT_GATE_RX
		#define TXGFV_TRANS		0			// ������� ����� �������� ����� � ��������
		#define TXGFV_TX_SSB	TXPATH_BIT_GATE
		#define TXGFV_TX_CW		TXPATH_BIT_GATE
		#define TXGFV_TX_AM		TXPATH_BIT_GATE
		#define TXGFV_TX_NFM	TXPATH_BIT_GATE
		// ���������� ������������ - ������ TXPATH_BIT_GATE
		// ��������� ���������� ��������� �������
		#define TXPATH_INITIALIZE() \
			do { \
				arm_hardware_piob_outputs2m(TXPATH_BIT_GATE, 0); \
			} while (0)

	#endif

	// PTT input

	#define PTT_TARGET_PIN				(GPIOB->IDR) // was PINA 
	#define PTT_BIT_PTT					(1u << 9)

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_piob_inputs(PTT_BIT_PTT); \
			arm_hardware_piob_updown(PTT_BIT_PTT, 0); \
		} while (0)

#endif /* WITHTX */


#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_TARGET_PIN			(GPIOB->IDR)
	#define ELKEY_BIT_LEFT				(1u << 10)
	#define ELKEY_BIT_RIGHT				(1u << 11)

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_piob_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piob_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif

#if 1
	// ����� ����������� ��� ������ ��� �������� �����������
	#define SPI_ALLCS_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define SPI_ALLCS_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)

	#define SPI_CSEL_PA15	(1u << 15)	/* NVRAM */ 
	#define SPI_CSEL_PA12	(1u << 12)	/* LCD */ 
	#define SPI_CSEL_PA11	(1u << 11)	/* PLL1 - HMC830 */ 
	#define SPI_CSEL_PA10	(1u << 10)	/* PLL2 - HMC830 */ 
	#define SPI_CSEL_PA9	(1u << 9)	/* DDS2 - AD9834 */ 
	#define SPI_CSEL_PA8	(1u << 8)	/* CTL registers chain */ 
	// ����� ������ ���� ����������� ��� ���� ������������ CS  ����������.
	#define SPI_ALLCS_BITS ( \
		SPI_CSEL_PA15 |		/* NVRAM */ \
		SPI_CSEL_PA12 |		/* LCD */ \
		SPI_CSEL_PA11 |		/* PLL1 - HMC830 */ \
		SPI_CSEL_PA10 |		/* PLL2 - HMC830 */ \
		SPI_CSEL_PA9 |		/* DDS2 - AD9834 */ \
		SPI_CSEL_PA8 |		/* CTL registers chain */ \
		0)

	#define SPI_ALLCS_BITSNEG 0		// ������, �������� ��� "1"

#else
	// ���� ������� ���������� �� ���� ������ SPI 

	// ���� ������ ������ ��������� �����������
	#define SPI_ADDRESS_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define SPI_ADDRESS_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	//#define SPI_ADDRESS_DDR_S			(GPIOC->PIO_OER)	// was DDRA
	//#define SPI_ADDRESS_DDR_C			(GPIOC->PIO_ODR)	// was DDRA

	#define SPI_A0 ((1u << 10))			// ���� ������ ��� ����������� SPI
	#define SPI_A1 ((1u << 11))
	#define SPI_A2 ((1u << 12))


	#define SPI_NAEN_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define SPI_NAEN_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
	//#define SPI_NAEN_DDR_S			(GPIOA->PIO_OER)	// was DDRA
	//#define SPI_NAEN_DDR_C			(GPIOA->PIO_ODR)	// was DDRA

	#define SPI_NAEN_BIT (1u << 15)		// PA15 used
	#define SPI_ALLCS_BITS	0		// ��������� ��� �������� ����, ��� ������ � ������ ������� CS (��� �����������) �� ���������
#endif

/* ������������� ���� ������ ������������ ��������� */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_pioa_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
	} while (0)
/* ������������� �������� ����������� ������������ CS */
#define SPI_ADDRESS_AEN_INITIALIZE() \
	do { \
	} while (0)
/* ������������� �������� ����������� ������������ CS */
#define SPI_ADDRESS_NAEN_INITIALIZE() \
	do { \
	} while (0)
/* ������������� ������� IOUPDATE �� DDS */
/* ��������� ��������� = 1 */
#define SPI_IOUPDATE_INITIALIZE() \
	do { \
	} while (0)
/* ������������� ������� IOUPDATE �� DDS */
/* ��������� ��������� = 0 */
#define SPI_IORESET_INITIALIZE() \
	do { \
	} while (0)

// SPI control pins
// SPI1 hardware used.

// MOSI & SCK port
// STM32F303: SPI1_NSS can be placed on PA4 or PA15
#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)

#define	SPI_SCLK_BIT			(1U << 3)	// * PB3 ���, ����� ������� ���� ������������� SPI
#define	SPI_MOSI_BIT			(1U << 5)	// * PB5 ���, ����� ������� ���� ����� (��� ���� � ������ ���������������� SPI).

#define SPI_TARGET_MISO_PIN		(GPIOB->IDR)		// was PINA 
#define	SPI_MISO_BIT			(1U << 4)	// * PB4 ���, ����� ������� ���� ���� � SPI.

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
			arm_hardware_piob_inputs(SPI_MISO_BIT); \
		} while (0)

#define SIDETONE_TARGET_BIT		(1u << 8)	// output TIM4_CH3 (PB8, base mapping)
//#define SIDETONE_TARGET_BIT		GPIO_ODR_ODR12	// output TIM4_CH1 (PD12, alt mapping)

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

	#elif CPUSTYLE_STM32F30X

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, 4 /* AF_I2C1 */);	/* AF=4 */ \
			} while (0) 

	#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C1);	/* AF=4 */ \
			} while (0) 

	#endif
#endif // WITHTWISW

#if WITHCPUADCHW
	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pioa_analoginput(((ainmask) >> 0) & 0xff);	/* ADC12_IN0..ADC12_IN7 */ \
			arm_hardware_piob_analoginput(((ainmask) >> 8) & 0x03);	/* ADC12_IN8..ADC12_IN0 */ \
			arm_hardware_pioc_analoginput(((ainmask) >> 10) & 0x3f);	/* ADC12_IN10..ADC12_IN15 */ \
		} while (0)
#endif /* WITHCPUADCHW */

#if CPUSTYLE_STM32F1XX

	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT | SPI_SCLK_BIT, 255); \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_piob_outputs(SPI_SCLK_BIT | SPI_MOSI_BIT, SPI_SCLK_BIT | SPI_MOSI_BIT); /* connect back to GPIO */ \
			arm_hardware_piob_inputs(SPI_MISO_BIT); /* connect back to GPIO */ \
		} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT, 255);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)
	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pioa_altfn2((1U << 9), 255);	/* TX DATA line (2 MHz) */ \
			arm_hardware_pioa_inputs((1U << 10));		/* RX data line */ \
			arm_hardware_pioa_updown((1U << 10), 0);	/* pull-up RX data */ \
		} while (0)
	#define HARDWARE_SIDETONE_INITIALIZE() do { \
			arm_hardware_piob_altfn2(SIDETONE_TARGET_BIT, 255);	/* output TIM4_CH3 (PB8, base mapping) */ \
		} while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioa_inputs(KBD_MASK); \
			} while (0)
	#endif

#elif CPUSTYLE_STM32F30X

	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT, 5 /*AF_SPI1 */); /* � ���� ����������� � ����� � ������ ������������� �� ALT FN */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_piob_outputs(SPI_SCLK_BIT | SPI_MOSI_BIT, SPI_SCLK_BIT | SPI_MOSI_BIT); /* connect back to GPIO */ \
			arm_hardware_piob_inputs(SPI_MISO_BIT); /* connect back to GPIO */ \
		} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT, 5 /*AF_SPI1 */);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)
	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pioa_altfn2((1U << 9) | (1U << 10), 7 /* AF_USART1 */); /* TX DATA line (2 MHz), RX data line */ \
			arm_hardware_pioa_updown((1U << 10), 0);	/* pull-up RX data */ \
		} while (0)
	#define HARDWARE_SIDETONE_INITIALIZE() do { \
		arm_hardware_piob_altfn2(SIDETONE_TARGET_BIT, 2 /*AF_TIM4 */);/* output TIM4_CH3 (PB8, base mapping) */ \
		} while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioa_inputs(KBD_MASK); \
			} while (0)
	#endif

#endif

#endif /* ARM_STM32FXXX_TQFP48_CPUSTYLE_RA4YBO_V3_INCLUDED */
