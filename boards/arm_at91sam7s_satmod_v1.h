/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef ARM_CPUSTYLE_V9_H_INCLUDED
#define ARM_CPUSTYLE_V9_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0: PA5/PA6 pins - WITHUART1HW
#define HARDWARE_ARM_USEUSART1 1		// US1: PA21/PA22 pins - WITHUART2HW
//#define HARDWARE_ARM_USEUART0 1		// UART0: PA9/PA10 pins - WITHUART1HW
//#define HARDWARE_ARM_USEUART1 1		// UART1: PB2/PB3 pins - WITHUART2HW
//#define HARDWARE_IGNORENONVRAM	1		// ������� �� ������ ��� ��� �������� NVRAM

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

#define WITHTWIHW 	1	/* ������������� ����������� ����������� TWI (I2C) */
//#define WITHTWISW 	1	/* ������������� ������������ ����������� TWI (I2C) */

#define WITHSPI16BIT	1		/* �������� ������������� 16-�� ������ ���� ��� ������ �� SPI */
#define WITHSPIHW 		1	/* ������������� ����������� ����������� SPI */
#define WITHSPIHWDMA 	1	/* ������������� DMA ��� ������ �� SPI */
//#define WITHSPISW 	1	/* ������������� ������������ ���������� SPI (������, ���� ��� ����������). */
#define WITHCPUADCHW 	1	/* ������������� ADC */

//#define WITHCAT_CDC		1	/* ������������ ����������� ���������������� ���� �� USB ���������� */
#define WITHCAT_USART2		1
#define WITHDEBUG_USART2	1
#define WITHMODEM_USART2	1
#define WITHNMEA_USART2		1

#if LCDMODE_ILI9320 || LCDMODE_S1D13781

	#define LS020_RESET_PORT_S(v)		do { AT91C_BASE_PIOA->PIO_SODR = (v); } while (0)
	#define LS020_RESET_PORT_C(v)		do { AT91C_BASE_PIOA->PIO_CODR = (v); } while (0)
	//#define LS020_RS	AT91C_PIO_PA31
	#define LS020_RST	AT91C_PIO_PA30

#elif LCDMODE_LS020

	#define LS020_RS_PORT_S(v)		do { AT91C_BASE_PIOA->PIO_SODR = (v); } while (0)
	#define LS020_RS_PORT_C(v)		do { AT91C_BASE_PIOA->PIO_CODR = (v); } while (0)
	#define LS020_RS	AT91C_PIO_PA31

	#define LS020_RESET_PORT_S(v)		do { AT91C_BASE_PIOA->PIO_SODR = (v); } while (0)
	#define LS020_RESET_PORT_C(v)		do { AT91C_BASE_PIOA->PIO_CODR = (v); } while (0)
	#define LS020_RST	AT91C_PIO_PA30

#elif LCDMODE_HD44780

	// ������ ����������� ��� ���������� WH2002 ��� ������������ (HD44780).
	#define LCD_TARGET_PORT			(AT91C_BASE_PIOA->PIO_ODSR)		// enabled by PIO_OWSR bits can be written was PORTA
	#define LCD_TARGET_PORT_S			(AT91C_BASE_PIOA->PIO_SODR)
	#define LCD_TARGET_PORT_C			(AT91C_BASE_PIOA->PIO_CODR)

	//#define LCD_DIRECTION_PORT			(AT91C_BASE_PIOA->PIO_OSR)	// was DDRA
	#define LCD_DIRECTION_PORT_S		(AT91C_BASE_PIOA->PIO_OER)
	#define LCD_DIRECTION_PORT_C		(AT91C_BASE_PIOA->PIO_ODR)
	#define LCD_INPUT_PORT			(AT91C_BASE_PIOA->PIO_PDSR) // was PINA 

	#define LCD_STROBE_BIT AT91C_PIO_PA23
	#define WRITEE_BIT AT91C_PIO_PA1
	#define ADDRES_BIT AT91C_PIO_PA0
	#define LCD_DATAS_BITS (AT91C_PIO_PA31 | AT91C_PIO_PA30 | AT91C_PIO_PA29 | AT91C_PIO_PA28)
	#define DATA_BIT_LOW 28		// ����� ��� ������ ������� � ����� ��������� � �����

#elif LCDMODE_UC1601 || LCDMODE_PCF8535

	//#define WITHTWIHW	1	/* ������������ ���������� ���������� TWI (I2C) */

#endif

#undef WITHELKEY

#if WITHENCODER
	// ������ ����������� ��������

	//#define ENCODER_TARGET_PORT_S			(AT91C_BASE_PIOA->PIO_SODR)	// was PORTA, now - port for set bits
	//#define ENCODER_TARGET_PORT_C			(AT91C_BASE_PIOA->PIO_CODR)	// was PORTA, now - port for clear bits
	//#define ENCODER_TARGET_PORT			(AT91C_BASE_PIOA->PIO_ODSR)

	#define ENCODER_DIRECTION_PORT_S			(AT91C_BASE_PIOA->PIO_OER)	// was DDRA
	#define ENCODER_DIRECTION_PORT_C			(AT91C_BASE_PIOA->PIO_ODR)	// was DDRA
	//#define KBD_TARGET_DDR			(AT91C_BASE_PIOA->PIO_OSR)	// was DDRA

	#define ENCODER_INPUT_PORT			(AT91C_BASE_PIOA->PIO_PDSR) // was PINA 

	#define ENCODER_BITS (AT91C_PIO_PA9 | AT91C_PIO_PA8)
	#define ENCODER_SHIFT 8

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(ENCODER_BITS); \
			arm_hardware_pioa_updown(ENCODER_BITS, 0); \
			arm_hardware_pioa_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_SYSTEM_PRIORITY); \
		} while (0)

#endif /* WITHENCODER */

/* ������������� ����� � ARM ������������ */

// SPI control pins

#define SPI_TARGET_MOSI_PORT_S(v)	do { AT91C_BASE_PIOA->PIO_SODR = (v); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v)	do { AT91C_BASE_PIOA->PIO_CODR = (v); } while (0)
#define SPI_TARGET_MISO_PORT_S(v)	do { AT91C_BASE_PIOA->PIO_SODR = (v); } while (0)
#define SPI_TARGET_MISO_PORT_C(v)	do { AT91C_BASE_PIOA->PIO_CODR = (v); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { AT91C_BASE_PIOA->PIO_SODR = (v); } while (0)
#define SPI_TARGET_SCLK_PORT_C(v)	do { AT91C_BASE_PIOA->PIO_CODR = (v); } while (0)

#define SPI_TARGET_MOSI_DDR_S			(AT91C_BASE_PIOA->PIO_OER)
#define SPI_TARGET_MOSI_DDR_C			(AT91C_BASE_PIOA->PIO_ODR)
#define SPI_TARGET_MISO_DDR_S			(AT91C_BASE_PIOA->PIO_OER)
#define SPI_TARGET_MISO_DDR_C			(AT91C_BASE_PIOA->PIO_ODR)
#define SPI_TARGET_SCLK_DDR_S			(AT91C_BASE_PIOA->PIO_OER)
#define SPI_TARGET_SCLK_DDR_C			(AT91C_BASE_PIOA->PIO_ODR)

// ����� ����������� ��� ������ ��� �������� �����������
#define SPI_ALLCS_PORT_S(v)	do { AT91C_BASE_PIOA->PIO_SODR = (v); } while (0)
#define SPI_ALLCS_PORT_C(v)	do { AT91C_BASE_PIOA->PIO_CODR = (v); } while (0)

#define SPI_CSEL0	(AT91C_PA11_NPCS0)	/* NPCS0 - SD card */
#define SPI_CSEL1	(AT91C_PA9_NPCS1)	/* SPI FRAM/EEPROM */
#define SPI_CSEL2	(AT91C_PA30_NPCS2)	/* SPI TFT PANEL CONTROLLER */
//#define SPI_CSEL3	(AT91C_PIO_PA4)	/* LED */ 
//#define SPI_CSEL4	(AT91C_PIO_PA31)	/* BUTTON S3 */ 
//#define SPI_CSEL5 (AT91C_PIO_PA16)	/*  */
#define SPI_CSEL6 0 //(AT91C_PIO_PA17)	/* */
//#define SPI_CSEL7 (AT91C_PIO_PA18) 	/*  */
// ����� ������ ���� ����������� ��� ���� ������������ CS  ����������.
#define SPI_ALLCS_BITS (SPI_CSEL0 | SPI_CSEL1 | SPI_CSEL2 | SPI_CSEL6)


/* ������������� ���� ������ ������������ ��������� */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_pioa_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS); \
	} while (0)
/* ������������� �������� ����������� ������������ CS */
#define SPI_ADDRESS_AEN_INITIALIZE() \
	do { \
		/* arm_hardware_pioa_outputs(SPI_AEN_BIT | SPI_ADDRESS_BITS, 0); */ \
	} while (0)
/* ������������� �������� ����������� ������������ CS */
#define SPI_ADDRESS_NAEN_INITIALIZE() \
	do { \
		/* arm_hardware_pioa_outputs(SPI_NAEN_BIT | SPI_ADDRESS_BITS, SPI_NAEN_BIT); */ \
	} while (0)
/* ������������� ������� IOUPDATE �� DDS */
/* ��������� ��������� = 1 */
#define SPI_IOUPDATE_INITIALIZE() \
	do { \
		/* arm_hardware_pioa_outputs(SPI_IOUPDATE_BIT, SPI_IOUPDATE_BIT); */ \
	} while (0)
/* ������������� ������� IOUPDATE �� DDS */
/* ��������� ��������� = 0 */
#define SPI_IORESET_INITIALIZE() \
	do { \
		/* arm_hardware_pioa_outputs(SPI_IORESET_BIT, 0); */ \
	} while (0)



#define	SPI_SCLK_BIT		AT91C_PIO_PA14	// ���, ����� ������� ���� ������������� SPI
#define	SPI_MOSI_BIT	AT91C_PIO_PA13	// ���, ����� ������� ���� ����� (��� ���� � ������ ���������������� SPI).

#define SPI_TARGET_MISO_PIN			(AT91C_BASE_PIOA->PIO_PDSR) // was PINA 
#define	SPI_MISO_BIT	AT91C_PIO_PA12	// ���, ����� ������� ���� ���� � SPI.

#define SPIIO_INITIALIZE() do { \
		arm_hardware_pioa_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
		arm_hardware_pioa_inputs(SPI_MISO_BIT); \
		arm_hardware_pioa_only(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT); /* disconnect from peripherials */ \
	} while (0)

#if WITHSPIHW

	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_pioa_peripha(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT);	/* PIO disable */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_pioa_only(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT);	/* PIO enable */ \
		} while (0)

	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_pioa_peripha(SPI_MOSI_BIT);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_pioa_only(SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)

#endif /* WITHSPIHW */

#define SIDETONE_TARGET_PORT_S	AT91C_BASE_PIOA->PIO_SODR)	/* ���� ����������, �� ������� ��������� ����� ���� ������������ */
#define SIDETONE_TARGET_PORT_C	AT91C_BASE_PIOA->PIO_CODR)	/* ���� ����������, �� ������� ��������� ����� ���� ������������ */
#define SIDETONE_TARGET_DDR_S	(AT91C_BASE_PIOA->PIO_OER)
#define SIDETONE_TARGET_DDR_C	(AT91C_BASE_PIOA->PIO_ODR)
#define SIDETONE_TARGET_BIT		AT91C_PA15_TIOA1

#define HARDWARE_SIDETONE_INITIALIZE() do { \
	} while (0)
#define HARDWARE_SIDETONE_CONNECT() do { \
	} while (0)
#define HARDWARE_SIDETONE_DISCONNECT() do { \
	} while (0)


#if WITHTWIHW || WITHTWISW
//#if CPUSTYLE_ATMEGA
	//#define TARGET_TWI_PORT PORTC
	//#define TARGET_TWI_DDR DDRC
	//#define TARGET_TWI_TWCK	(1U << PC0)
	//#define TARGET_TWI_TWD	(1U << PC1)
//#elif CPUSTYLE_ARM
	#define TARGET_TWI_TWCK_PORT_C(v)	do { AT91C_BASE_PIOA->PIO_CODR = (v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v)	do { AT91C_BASE_PIOA->PIO_SODR = (v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_C(v)	do { AT91C_BASE_PIOA->PIO_CODR = (v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v)	do { AT91C_BASE_PIOA->PIO_SODR = (v); __DSB(); } while (0)
	#define TARGET_TWI_PIN		(AT91C_BASE_PIOA->PIO_PDSR)
	#define TARGET_TWI_TWCK		AT91C_PA4_TWCK
	#define TARGET_TWI_TWD		AT91C_PA3_TWD
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

#endif /* WITHTWIHW || WITHTWISW */

	#if HARDWARE_ARM_USEUSART0
		#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pioa_inputs(AT91C_PA5_RXD0); \
			arm_hardware_pioa_outputs(AT91C_PA6_TXD0, AT91C_PA6_TXD0); \
			arm_hardware_pioa_peripha(AT91C_PA5_RXD0 | AT91C_PA6_TXD0); \
			} while (0)
	#elif HARDWARE_ARM_USEUSART1
		#define HARDWARE_USART2_INITIALIZE() do { \
			arm_hardware_pioa_inputs(AT91C_PA21_RXD1); \
			arm_hardware_pioa_outputs(AT91C_PA22_TXD1, AT91C_PA22_TXD1); \
			arm_hardware_pioa_peripha(AT91C_PA21_RXD1 | AT91C_PA22_TXD1); \
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



#if CPUSTYLE_SAM9XE

	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_piob_peripha(SPI_MOSI_BIT);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_piob_only(SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)

#endif /* CPUSTYLE_SAM9XE */

#endif /* ARM_CPUSTYLE_V9_H_INCLUDED */
