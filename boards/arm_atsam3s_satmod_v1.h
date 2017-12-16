/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

// ������� "��������". up-conversion,  AD9951 + ADF4001, AD9834

#ifndef ARM_ATSAM3S_CPUSTYLE_V8A_H_INCLUDED
#define ARM_ATSAM3S_CPUSTYLE_V8A_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// USART0: PA5/PA6 pins - WITHUART1HW
#define HARDWARE_ARM_USEUSART1 1		// USART1: PA21/PA22 pins - WITHUART2HW
//#define HARDWARE_ARM_USEUART0 1		// UART0: PA9/PA10 pins - WITHUART1HW
//#define HARDWARE_ARM_USEUART1 1		// UART1: PB2/PB3 pins - WITHUART2HW

//#define WITHUART1HW	1	/* ������������ ������������ ���������� ����������������� ����� #1 */
#define WITHUART2HW	1	/* ������������ ������������ ���������� ����������������� ����� #2 */

//#define WITHUSBHW	1	/* ������������ ���������� � ��������� ��������� USB */
//#define WITHUSBHWVBUSSENSE	1	/* ������������ ���������������� ����� VBUS_SENSE */
//#define WITHUSBHWHIGHSPEED	1	/* ������������ ���������� � ��������� ��������� USB HS */
//#define WITHUSBHWHIGHSPEEDDESC	1	/* ��������� ����������� ����������� ��� ��� HIGH SPEED */

//#define WITHCAT_CDC		1	/* ������������ ����������� ���������������� ���� �� USB ���������� */
#define WITHCAT_USART2		1
#define WITHDEBUG_USART2	1
#define WITHMODEM_USART2	1
#define WITHNMEA_USART2		1

#define WITHSPI16BIT	1		/* �������� ������������� 16-�� ������ ���� ��� ������ �� SPI */
#define WITHSPIHW 		1	/* ������������� ����������� ����������� SPI */
#define WITHSPIHWDMA 	1	/* ������������� DMA ��� ������ �� SPI */
//#define WITHSPISW 	1	/* ������������� ������������ ���������� SPI. */

#define WITHTWIHW 	1	/* ������������� ����������� ����������� TWI (I2C) */
//#define WITHTWISW 	1	/* ������������� ������������ ����������� TWI (I2C) */
#define WITHCPUADCHW 	1	/* ������������� ADC */

//#define WITHCAT_CDC		1	/* ������������ ����������� ���������������� ���� �� USB ���������� */
#define WITHCAT_USART2		1
#define WITHDEBUG_USART2	1
#define WITHMODEM_USART2	1
#define WITHNMEA_USART2		1

#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_pioa_outputs(LS020_RS, LS020_RS); \
		arm_hardware_pioa_only(LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_pioa_outputs(LS020_RST, LS020_RST); \
		arm_hardware_pioa_only(LS020_RST); \
	} while (0)

#if LCDMODE_ILI9320 || LCDMODE_S1D13781

	#define LS020_RESET_PORT_S(v)		do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { PIOA->PIO_CODR = (v); __DSB(); } while (0)
	#define LS020_RST				PIO_PA28		// D6 signal in HD44760 socket

#elif LCDMODE_UC1608

	// ��� ����������� ������� RESET � RS, � ��� �� ������������� ��������� (����������� �� ��� �� ������ ��� � EE ��44780
	#define UC1608_CSP_PORT_S(v)		do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
	#define UC1608_CSP_PORT_C(v)		do { PIOA->PIO_CODR = (v); __DSB(); } while (0)
	#define UC1608_CSP 			PIO_PA30		// E signal on HD44780 socket - positive chipselect (same as LCD_STROBE_BIT)
	#define SPI_CSEL255			255				// �� ����� ���������� �������� ������������� ��������

	#define LS020_RESET_PORT_S(v)		do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { PIOA->PIO_CODR = (v); __DSB(); } while (0)
	#define LS020_RST			PIO_PA28			// D6 signal in HD44760 socket

	#define LS020_RS_PORT_S(v)		do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { PIOA->PIO_CODR = (v); __DSB(); } while (0)
	#define LS020_RS			PIO_PA29			// D7 signal in HD44760 socket

	#define UC1608_CSP_INITIALIZE() do { \
			arm_hardware_pioa_outputs(UC1608_CSP, 0); \
		} while (0)

#elif LCDMODE_LS020 || LCDMODE_LPH88 || LCDMODE_S1D13781 || LCDMODE_ILI9225 || LCDMODE_ST7735 || LCDMODE_ST7565S || LCDMODE_ILI9163 || LCDMODE_ILI9163

	// ��� ����������� ������� RESET � RS
	#define LS020_RESET_PORT_S(v)		do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { PIOA->PIO_CODR = (v); __DSB(); } while (0)
	#define LS020_RST			PIO_PA28			// D6 signal in HD44760 socket

	#define LS020_RS_PORT_S(v)		do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { PIOA->PIO_CODR = (v); __DSB(); } while (0)
	#define LS020_RS			PIO_PA29			// D7 signal in HD44760 socket

#elif LCDMODE_UC1601 || LCDMODE_PCF8535

	//#define WITHTWIHW 	1	/* ������������� ����������� ����������� TWI (I2C) */

#endif

#undef WITHELKEY

#if WITHENCODER
	// ������ ����������� ��������

	//#define ENCODER_TARGET_PORT_S			PIOA->PIO_SODR	// was PORTA, now - port for set bits
	//#define ENCODER_TARGET_PORT_C			PIOA->PIO_CODR	// was PORTA, now - port for clear bits
	//#define ENCODER_TARGET_PORT			PIOA->PIO_ODSR

	#define ENCODER_DIRECTION_PORT_S			PIOA->PIO_OER	// was DDRA
	#define ENCODER_DIRECTION_PORT_C			PIOA->PIO_ODR	// was DDRA
	//#define KBD_TARGET_DDR			PIOA->PIO_OSR	// was DDRA

	#define ENCODER_INPUT_PORT			PIOA->PIO_PDSR // was PINA 

	#define ENCODER_BITS (PIO_PA26 | PIO_PA25)
	#define ENCODER_SHIFT 25

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(ENCODER_BITS); \
			arm_hardware_pioa_updown(ENCODER_BITS, 0); \
			arm_hardware_pioa_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_SYSTEM_PRIORITY); \
		} while (0)

#endif /* WITHENCODER */
/* ������������� ����� � ARM ������������ */

#define HARDWARE_SIDETONE_INITIALIZE() do { \
	} while (0)
#define HARDWARE_SIDETONE_CONNECT() do { \
	} while (0)
#define HARDWARE_SIDETONE_DISCONNECT() do { \
	} while (0)


// SPI control pins

#define SPI_TARGET_MOSI_PORT_S(v)	do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v)	do { PIOA->PIO_CODR = (v); __DSB(); } while (0)
#define SPI_TARGET_MISO_PORT_S(v)	do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
#define SPI_TARGET_MISO_PORT_C(v)	do { PIOA->PIO_CODR = (v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_C(v)	do { PIOA->PIO_CODR = (v); __DSB(); } while (0)

#define SPI_TARGET_MOSI_DDR_S			(PIOA->PIO_OER)
#define SPI_TARGET_MOSI_DDR_C			(PIOA->PIO_ODR)
#define SPI_TARGET_MISO_DDR_S			(PIOA->PIO_OER)
#define SPI_TARGET_MISO_DDR_C			(PIOA->PIO_ODR)
#define SPI_TARGET_SCLK_DDR_S			(PIOA->PIO_OER)
#define SPI_TARGET_SCLK_DDR_C			(PIOA->PIO_ODR)

// ����� ����������� ��� ������ ��� �������� �����������
#define SPI_ALLCS_PORT_S(v)	do { PIOA->PIO_SODR = (v); } while (0)
#define SPI_ALLCS_PORT_C(v)	do { PIOA->PIO_CODR = (v); } while (0)

#define SPI_CSEL0	(PIO_PA11A_NPCS0)	/* NPCS0 - SD card */
#define SPI_CSEL1	(PIO_PA9B_NPCS1)	/* SPI FRAM/EEPROM */
#define SPI_CSEL2	(PIO_PA30B_NPCS2)	/* SPI TFT PANEL CONTROLLER */
//#define SPI_CSEL3	(AT91C_PIO_PA4)	/* LED */ 
//#define SPI_CSEL4	(AT91C_PIO_PA31)	/* BUTTON S3 */ 
//#define SPI_CSEL5 (AT91C_PIO_PA16)	/*  */
#define SPI_CSEL6 (0)	/* control register */
//#define SPI_CSEL7 (AT91C_PIO_PA18) 	/*  */

#define targetdataflash PIO_PA15	/* ������������ SPI DATA FLASH */

#define TARGET_RESET	PIO_PA10

// ����� ������ ���� ����������� ��� ���� ������������ CS  ����������.
#define SPI_ALLCS_BITS (SPI_CSEL0 | SPI_CSEL1 | SPI_CSEL2 | SPI_CSEL6 | targetdataflash)


/* ������������� �������� ������ ������������ ��������� */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_pioa_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS); \
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



#define	SPI_MISO_BIT			PIO_PA12A_MISO	// ���, ����� ������� ���� ���� � SPI.
#define	SPI_MOSI_BIT			PIO_PA13A_MOSI	// ���, ����� ������� ���� ����� (��� ���� � ������ ���������������� SPI).
#define	SPI_SCLK_BIT			PIO_PA14A_SPCK	// ���, ����� ������� ���� ������������� SPI

#define SPI_TARGET_MISO_PIN		PIOA->PIO_PDSR // was PINA 

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_pioa_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
			arm_hardware_pioa_inputs(SPI_MISO_BIT); \
			arm_hardware_pioa_only(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT); /* disconnect from peripherials */ \
		} while (0)

#if WITHSPIHW

	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_pioa_opendrain(TARGET_RESET, 0); /* ����� ������� � RESET */ \
			arm_hardware_pioa_peripha(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT);	/* PIO disable */ \
		} while (0)

	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_pioa_opendrain(TARGET_RESET, 0); /* ����� ������� � RESET */ \
			arm_hardware_pioa_only(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT);	/* PIO enable */ \
		} while (0)

	/* ��������� SPI ������������� - ���������� �� ���������������� ���������� */
	#define HARDWARE_SPI_HANGOFF() do { \
			arm_hardware_pioa_only(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT);	/* PIO enable */ \
			arm_hardware_pioa_inputs(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT | targetdataflash);	/* PIO enable */ \
			arm_hardware_pioa_opendrain(TARGET_RESET, TARGET_RESET); /* ��������� RESET */ \
		} while (0)

	/* ��������� SPI ������������� - ����������� � ���������������� ���������� */
	#define HARDWARE_SPI_HANGON() do { \
			arm_hardware_pioa_opendrain(TARGET_RESET, 0); /* ����� ������� � RESET */ \
			arm_hardware_pioa_only(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT);	/* PIO enable */ \
			arm_hardware_pioa_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT | targetdataflash); \
		} while (0)

	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_pioa_peripha(SPI_MOSI_BIT);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_pioa_only(SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)

#endif /* WITHSPIHW */

#define SIDETONE_TARGET_PORT_S	PIOA->PIO_SODR	/* ���� ����������, �� ������� ��������� ����� ���� ������������ */
#define SIDETONE_TARGET_PORT_C	PIOA->PIO_CODR	/* ���� ����������, �� ������� ��������� ����� ���� ������������ */
#define SIDETONE_TARGET_DDR_S	PIOA->PIO_OER
#define SIDETONE_TARGET_DDR_C	PIOA->PIO_ODR
#define SIDETONE_TARGET_BIT		PIO_PA15B_TIOA1

#if 1 // WITHTWISW
//#if CPUSTYLE_ATMEGA
	//#define TARGET_TWI_PORT PORTC
	//#define TARGET_TWI_DDR DDRC
	//#define TARGET_TWI_TWCK	(1U << PC0)
	//#define TARGET_TWI_TWD	(1U << PC1)
//#elif CPUSTYLE_ARM
	#define TARGET_TWI_TWCK_PORT_C(v)	do { PIOA->PIO_CODR = (v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v)	do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_C(v)	do { PIOA->PIO_CODR = (v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v)	do { PIOA->PIO_SODR = (v); __DSB(); } while (0)
	#define TARGET_TWI_PIN		(PIOA->PIO_PDSR)
	#define TARGET_TWI_TWCK		PIO_PA4A_TWCK0
	#define TARGET_TWI_TWD		PIO_PA3A_TWD0
//#endif

	// ������������� ����� ������ �����-������ ��� ����������� ���������� I2C
	#define	TWISOFT_INITIALIZE() do { \
			enum { WORKMASK = TARGET_TWI_TWCK | TARGET_TWI_TWD };		/* ������� �����, ���������� ����� ������� �������� */ \
			arm_hardware_pioa_opendrain(WORKMASK, WORKMASK);	/* The user must not program TWD and TWCK as open-drain. It is already done by the hardware. */ \
			arm_hardware_pioa_only(WORKMASK); /* disconnect from peripherials */ \
		} while (0) 

	// ������������� ����� ������ �����-������ ��� ���������� ���������� I2C
	// ������������� ������� � ������������� ����������
	#define	TWIHARD_INITIALIZE() do { \
			enum { WORKMASK = TARGET_TWI_TWCK | TARGET_TWI_TWD };		/* ������� �����, ���������� ����� ������� �������� */ \
			arm_hardware_pioa_peripha(WORKMASK); \
		} while (0) 
#endif

#if HARDWARE_ARM_USEUSART0
	#define HARDWARE_USART1_INITIALIZE() do { \
		arm_hardware_pioa_inputs(PIO_PA5A_RXD0); \
		arm_hardware_pioa_outputs(PIO_PA6A_TXD0, PIO_PA6A_TXD0); \
		arm_hardware_pioa_peripha(PIO_PA5A_RXD0 | PIO_PA6A_TXD0); \
		} while (0)
#elif HARDWARE_ARM_USEUSART1
	#define HARDWARE_USART2_INITIALIZE() do { \
		arm_hardware_pioa_inputs(PIO_PA21A_RXD1); \
		arm_hardware_pioa_outputs(PIO_PA22A_TXD1, PIO_PA22A_TXD1); \
		arm_hardware_pioa_peripha(PIO_PA21A_RXD1 | PIO_PA22A_TXD1); \
		} while (0)
#elif HARDWARE_ARM_USEUART0
	#define HARDWARE_USART1_INITIALIZE() do { \
		arm_hardware_pioa_inputs(PIO_PA9A_URXD0); \
		arm_hardware_pioa_outputs(PIO_PA10A_UTXD0, PIO_PA10A_UTXD0); \
		arm_hardware_pioa_peripha(PIO_PA9A_URXD0 | PIO_PA10A_UTXD0); \
		} while (0)
#elif HARDWARE_ARM_USEUART1
	#define HARDWARE_USART2_INITIALIZE() do { \
		arm_hardware_piob_inputs(PIO_PB2A_URXD1); \
		arm_hardware_piob_outputs(PIO_PB3A_UTXD1, PIO_PB3A_UTXD1); \
		arm_hardware_piob_peripha(PIO_PB2A_URXD1 | PIO_PB3A_UTXD1); \
		} while (0)
#endif

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioa_inputs(KBD_MASK); \
			} while (0)
	#endif

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
		} while (0)

	#define HARDWARE_SDIOSENSE_CD() ( 1 )	/* �������� ��������� ������� CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() ( 0 )	/* �������� ��������� ������� CARD WRITE PROTECT */

	/* ���� ������� SD CARD ����������� ����� � ���������� */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		/* arm_hardware_pio3_outputs(HARDWARE_SDIO_POWER_BIT, HARDWARE_SDIO_POWER_BIT); */ /* ������� ��������� */ \
		} while (0)

#endif /* ARM_ATSAM3S_CPUSTYLE_V8A_H_INCLUDED */
