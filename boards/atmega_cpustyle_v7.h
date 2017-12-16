/* $Id$ */
/* board-specific CPU DIP40/TQFP44 attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

// ����� � 5 ���, �� ���� � ���������� (��������� �� ������� ���� �� ATMEGA) - � ����� RA1AGG
// ��� SPI_BIDIRECTIONAL � ��� ��������� ������

#ifndef ATMEGA_CPUSTYLE_V7_H_INCLUDED
#define ATMEGA_CPUSTYLE_V7_H_INCLUDED 1

#define WITHSPISW 	1	/* ������������� ������������ ���������� SPI. */
//#define WITHSPIHW 	1	/* ������������� ����������� ����������� SPI */
//#define WITHTWIHW 	1	/* ������������� ����������� ����������� TWI (I2C) */
//#define WITHTWISW 	1	/* ������������� ������������ ����������� TWI (I2C) */
#define WITHCPUADCHW 	1	/* ������������� ADC */

//#define WITHUART1HW	1	/* ������������ ������������ ���������� ����������������� ����� #1 */
//#define WITHUART2HW	1	/* ������������ ������������ ���������� ����������������� ����� #2 */

//#define WITHCAT_CDC		1	/* ������������ ����������� ���������������� ���� �� USB ���������� */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1

#define SPI_BIDIRECTIONAL 	1	/* ���� ������ �� SPI ���� ����� ��� �� ����� pin, ��� � ����� */

#if LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	// ������ LCD
	#define LCD_DATA_PORT PORTB
	#define LCD_DATA_DIRECTION DDRB
	#define LCD_DATA_INPUT PINB
	// ����� E LCD
	#define LCD_STROBE_PORT PORTB
	#define LCD_STROBE_DIRECTION DDRB
	// ������� RS � WE LCD
	#define LCD_RS_WE_PORT PORTB
	#define LCD_RS_WE_DIRECTION DDRB

	#define LCD_STROBE_BIT (1U << PB3)	// 0x08

	#define LCD_DATAS_BIT_LOW PB4		// ����� ��� ������ ������� � ����� ��������� � �����
	#define LCD_DATAS_BITS (0x0f << LCD_DATAS_BIT_LOW)

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* �������� ������ � ���� LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { \
		LCD_DATA_PORT = \
			(LCD_DATA_PORT & ~ LCD_DATAS_BITS) | \
			(((v) << LCD_DATAS_BIT_LOW) & LCD_DATAS_BITS); \
		} while (0)

	#define WRITEE_BIT (1U << PB2) // 0x04
	#define ADDRES_BIT (1U << PB1)	// 0x02

#endif

#if WITHENCODER
	#define ENCODER_TARGET_PORT PORTD
	#define ENCODER_DIRECTION_PORT DDRD
	#define ENCODER_INPUT_PORT PIND

	#define ENCODER_BITS ((1U << PIND3) | (1U << PIND2))
	#define ENCODER_SHIFT PIND2
#endif /* WITHENCODER */

#if WITHTX
	/* ������������ ���� ���������� ������� ���������� �� ���������� -
	 ���� ���������, �������������� ���� �� ���� �����, ��������� ��� �������� ����������.
		*/

	#define TXPATH_TARGET_PORT PORTD	// ������ ���������� - ���������� ������� ��������� � ������������
	#define TXPATH_TARGET_DDR DDRD		// ������������ �� ����� - ���������� ������� ��������� � ������������

	#define TXPATH_BIT_GATE (1U << PD6)	// �������� ������ �� ���������� - ���������� ������������.
	//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// ������ tx2 - ���������� ������������. ��� ����� �������
	#define TXPATH_BIT_GATE_RX 0	// ������ tx2 - ���������� ������������. ��� ����� �� �������

	// �������������� ����������� �����
	#define TXGFV_RX		TXPATH_BIT_GATE_RX
	#define TXGFV_TRANS		0			// ������� ����� �������� ����� � ��������
	#define TXGFV_TX_SSB	TXPATH_BIT_GATE
	#define TXGFV_TX_CW		TXPATH_BIT_GATE
	#define TXGFV_TX_AM		TXPATH_BIT_GATE
	#define TXGFV_TX_NFM	TXPATH_BIT_GATE

	#if WITHELKEY
		#define ELKEY_TARGET_PORT PORTD		// ����� ���������� - ������� ������������ �����
		#define ELKEY_TARGET_DDR DDRD		// ������������ �� ���� - ������� ������������ �����
		#define ELKEY_TARGET_PIN PIND		// ���� - ������� ������������ �����

		#define ELKEY_BIT_LEFT (1U << PD4)
		#define ELKEY_BIT_RIGHT (1U << PD5)
		// ���������� ���������� �� �������� �� �������� �����
		// ���������������� ��������� ATMega644
		#define PCMSK_PORT	PCMSK3			// PCMSK0 - PA, PCMSK1 - PB, PCMSK2 - PC, PCMSK3 - PD
		#define PCICR_BIT	(1U << PCIE3)	// PCIE0 - PA, PCIE1 - PB, PCIE2 - PC, PCIE3 - PD
		#define PCIVECT PCINT3_vect			// ������ �����������
	#endif

#endif /* WITHTX */



	// ��������� ��������� ���� ������������ ��� �������� ����������
	// ��������� ������� ������������ �� PD7(OC2)
	// PTT ���������� �� PD0 (������ ���� - ���� ������ �� NVRAM. �������� SPI_BIDIRECTIONAL.
	#if SPI_BIDIRECTIONAL

		#define PTT_TARGET_PORT PORTB	
		#define PTT_TARGET_DDR DDRB
		#define PTT_TARGET_PIN PINB

		#define PTT_BIT_PTT (1U << PB0)

	#else
		// ���� �� ����������� SPI_BIDIRECTIONAL - ��� ������������� CAT ������������ ��� ����� RXD (RXD0)
		#if WITHCAT == 0

			#define PTT_TARGET_PORT PORTD	
			#define PTT_TARGET_DDR DDRD
			#define PTT_TARGET_PIN PIND

			#define PTT_BIT_PTT (1U << PD0)

		#endif

	#endif /* SPI_BIDIRECTIONAL */

	//#define SIDETONE_TARGET_PORT	PORTD	/* ���� ����������, �� ������� ��������� ����� ���� ������������ */
	//#define SIDETONE_TARGET_DDR		DDRD
	//#define SIDETONE_TARGET_BIT		(1U << PD7)

#if WITHTWISW
	#if 0
		#if LCDMODE_HD44780
			#error TWI can not use same pins as HD44780
		#endif
		#define TARGET_TWI_PORT PORTB
		#define TARGET_TWI_DDR DDRB
		#define TARGET_TWI_PIN PINB
		//#define TARGET_TWI_TWCK	(1U << PC0)	// ���������� TWCK ����� ��
		//#define TARGET_TWI_TWD	(1U << PC1)	// ���������� TWD ����� ��
		#define TARGET_TWI_TWCK	(1U << PB4)			// �� ���� ����� ���������� � ��� �� �����, ���� ����� �� ������������ ������ ����������
		#define TARGET_TWI_TWD	(1U << PB5)
	#elif 1
		// ������� ��� RA4FIX - TWI �� ������� �����
		#if WITHTX
			#error TWI can not use same pins as CW key
		#endif
		#define TARGET_TWI_PORT PORTD
		#define TARGET_TWI_DDR DDRD
		#define TARGET_TWI_PIN PIND
		#define TARGET_TWI_TWCK	(1U << PD4)
		#define TARGET_TWI_TWD	(1U << PD5)
	#else
		/* No any peripherials in this configuration */
	#endif

//#elif CPUSTYLE_ARM
	//#define TARGET_TWI_PORT_C	AT91C_BASE_PIOA->PIO_CODR
	//#define TARGET_TWI_PORT_S	AT91C_BASE_PIOA->PIO_SODR
	//#define TARGET_TWI_TWCK	AT91C_PA4_TWCK
	//#define TARGET_TWI_TWD	AT91C_PA3_TWD
//#endif
#endif


#if KEYBOARD_USE_ADC == 0
	#define KBD_TARGET_PORT PORTA
	#define KBD_TARGET_DDR DDRA
	#define KBD_TARGET_PIN PINA

	#define KBD_MASK 0xfe	// ��� ������������ ����
#endif

	#define SPI_TARGET_SCLK_PORT	PORTC
	#define SPI_TARGET_SCLK_DDR		DDRC
	#define SPI_TARGET_SCLK_PIN		PINC

	#define SPI_TARGET_MOSI_PORT	PORTC
	#define SPI_TARGET_MOSI_DDR		DDRC
	#define SPI_TARGET_MOSI_PIN		PINC

	#define SPI_IOUPDATE_PORT		PORTC
	#define SPI_IOUPDATE_DDR		DDRC

	#define SPI_IORESET_PORT		PORTC
	#define SPI_IORESET_DDR			DDRC

	#define	SPI_SCLK_BIT	(1U << PC0)	// ���, ����� ������� ���� ������������� SPI
	#define	SPI_MOSI_BIT	(1U << PC1)	// ���, ����� ������� ���� ����� (��� ���� � ������ ���������������� SPI).

	#define SPI_AEN_PORT	PORTC
	#define SPI_AEN_DDR		DDRC
	#define SPI_AEN_BIT		(1U << PC5)
	#define SPI_AEN_PIN		PINC	/* atmegaXX4 only */
	#define SPI_ALLCS_BITS	0		// ��������� ��� �������� ����, ��� ������ � ������ ������� CS (��� �����������) �� ���������

	#define SPI_IORESET_BIT		(1U << PC6)		// ������ ����������. ���������� ����� ���� ������ �� ���.
	#define SPI_IOUPDATE_BIT	(1U << PC7)

	// SPI addresss location
	#define SPI_ADDRESS_PORT		PORTC
	#define SPI_ADDRESS_DDR		DDRC
	//#define SPI_ADDRESS_PIN		PINC

	#define SPI_A0 (1U << PC2)			// ���� ������ ��� ����������� SPI
	#define SPI_A1 (1U << PC3)
	#define SPI_A2 (1U << PC4)


	#if SPI_BIDIRECTIONAL
		#define SPI_TARGET_MOSI_PIN PINC
		//#define SPI_TARGET_MISO_PIN PINC
		//#define SPI_TARGET_MISO_PORT PORTC
		//#define SPI_TARGET_MISO_DDR DDRC
		//#define	SPI_MISO_BIT		(1U << PC1)	// ���, ����� ������� ���� ���� � SPI.
	#else

		#define SPI_TARGET_MISO_PIN PINB
		#define SPI_TARGET_MISO_PORT PORTB
		#define SPI_TARGET_MISO_DDR DDRB
		#define	SPI_MISO_BIT		(1U << PB0)	// ���, ����� ������� ���� ���� � SPI.

	#endif /* SPI_BIDIRECTIONAL */

	// Variations of SPI_ALLCS_INITIALIZE
	#if SPI_ALLCS_BITS != 0
		#define SPI_ALLCS_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(SPI_ALLCS_PORT, SPI_ALLCS_DDR, SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
			} while (0)
	#else /* SPI_ALLCS_BITS != 0 */
		#define SPI_ALLCS_INITIALIZE() do { \
			} while (0)
	#endif /* SPI_ALLCS_BITS != 0 */

	#define HARDWARE_SPI_CONNECT() do { \
		} while (0)

	#define HARDWARE_SPI_DISCONNECT() do { \
			SPCR = 0x00; \
		} while (0)

	#if SPI_BIDIRECTIONAL

		#define SPIIO_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_SCLK_PORT, SPI_TARGET_SCLK_DDR, SPI_SCLK_BIT, SPI_SCLK_BIT); \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_MOSI_PORT, SPI_TARGET_MOSI_DDR, SPI_MOSI_BIT, SPI_MOSI_BIT); \
			} while (0)

	#else /* SPI_BIDIRECTIONAL */

		#define SPIIO_INITIALIZE() do { \
			HARDWARE_INPUT_INITIALIZE(SPI_TARGET_MISO_PORT, SPI_TARGET_MISO_DDR, SPI_MISO_BIT, SPI_MISO_BIT); /* enable pull-up on MISO */ \
			/* PB4(~SS) ������ ���� �������. SPI_NAEN_BIT - ���������� �����������. */ \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_SCLK_PORT, SPI_TARGET_SCLK_DDR, SPI_SCLK_BIT, SPI_SCLK_BIT); \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_MOSI_PORT, SPI_TARGET_MOSI_DDR, SPI_MOSI_BIT, SPI_MOSI_BIT); \
			} while (0)

	#endif /* SPI_BIDIRECTIONAL */


	/* ���������������� �������, ����������� I2C */
	#define TWISOFT_INITIALIZE() do { \
		HARDWARE_INPUT_INITIALIZE(TARGET_TWI_TWCK_PORT, TARGET_TWI_TWCK_DDR, TARGET_TWI_TWCK, TARGET_TWI_TWCK); \
		HARDWARE_INPUT_INITIALIZE(TARGET_TWI_TWD_PORT, TARGET_TWI_TWD_DDR, TARGET_TWI_TWD, TARGET_TWI_TWD); \
		} while (0)

	// ������������� ����� ������ �����-������ ��� ���������� ���������� I2S
	// ������������� ������� � ������������� ����������
	// �� ATMEGA �� ���������
	#define	TWIHARD_INITIALIZE() do { \
		} while (0) 

	#define CS4272_RESET_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(TARGET_CS4272_RESET_PORT, TARGET_CS4272_RESET_DDR, TARGET_CS4272_RESET_BIT, TARGET_CS4272_RESET_BIT); \
		} while (0)

	#define LS020_RS_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(LS020_RS_PORT, LS020_RS_DDR, LS020_RS, LS020_RS); \
		} while(0)

	#define LS020_RESET_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(LS020_RESET_PORT, LS020_RESET_DDR, LS020_RST, LS020_RST); \
		} while(0)

	#define LS020_RESET_PORT_S(b)	do { LS020_RESET_PORT |= (b); } while (0)
	#define LS020_RESET_PORT_C(b)	do { LS020_RESET_PORT &= ~ (b); } while (0)
	#define LS020_RS_PORT_S(b)		do { LS020_RS_PORT |= (b); } while (0)
	#define LS020_RS_PORT_C(b)		do { LS020_RS_PORT &= ~ (b); } while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define HARDWARE_KBD_INITIALIZE() do { \
				KBD_TARGET_PORT |= KBD_MASK;	/* tie up inputs */ \
				KBD_TARGET_DDR &= ~ KBD_MASK;	/* define these bits as inputs - enable pull-up */ \
			} while (0)
	#endif

#if SPI_BIDIRECTIONAL && CPUSTYLE_ATMEGA

	#define SPIIO_MOSI_TO_INPUT() do { \
		SPI_TARGET_MOSI_DDR &= ~ SPI_MOSI_BIT;	/* ����������� ���� �� ������ � ������� */ \
		} while (0)
	#define SPIIO_MOSI_TO_OUTPUT() do { \
		SPI_TARGET_MOSI_DDR |= SPI_MOSI_BIT;	/* ������� ������ ����� */ \
		} while (0)

#endif /* SPI_BIDIRECTIONAL && CPUSTYLE_ATMEGA */


#if CPUSTYLE_ATMEGA328

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
		PTT_TARGET_PORT |= PTT_BIT_PTT;	/* ������� pull-up �� ���� ����. */ \
		PTT_TARGET_DDR &= ~ PTT_BIT_PTT;	/* ��� ����� PTT */ \
	} while (0)

	#define ELKEY_INITIALIZE() do { \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_LEFT;	/* ��� ����� ������ �������� ������������ */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_LEFT; \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_RIGHT;	/* ��� ����� ������� �������� ������������ */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_RIGHT; \
		/* PCMSK_PORT |= (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); */ /* ���������� ���������� �� �������� �� �������� ����� */ \
		/* PCICR |= (PCICR_BIT); */ \
	} while (0)

	#define NMEA_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* ��� ����� DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
	} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* ��� ����� DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
	} while (0)

	// FROMCAT_RTS_INITIALIZE variations
	#if FROMCAT_BIT_RTS != 0
		#define FROMCAT_RTS_INITIALIZE() do { \
			FROMCAT_TARGET_PORT_RTS |= FROMCAT_BIT_RTS;		/* enable pull-up */ \
			FROMCAT_TARGET_DDR_RTS &= ~ FROMCAT_BIT_RTS;	/* ��� ����� RTS */ \
		} while (0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)
	#endif /* FROMCAT_BIT_RTS != 0 */

	#define ENCODER_INITIALIZE() do { \
		EICRA |= 0x05; \
		EIMSK |= 0x03; \
		EIFR |= 0x03; \
		ENCODER_TARGET_PORT |= ENCODER_BITS; /* PULL-UP */  \
		ENCODER_DIRECTION_PORT &= ~ ENCODER_BITS; \
	} while (0)

#elif CPUSTYLE_ATMEGA_XXX4

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
		PTT_TARGET_PORT |= PTT_BIT_PTT;	/* ������� pull-up �� ���� ����. */ \
		PTT_TARGET_DDR &= ~ PTT_BIT_PTT;	/* ��� ����� PTT */ \
	} while (0)

	#define ELKEY_INITIALIZE() do { \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_LEFT;	/* ��� ����� ������ �������� ������������ */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_LEFT; \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_RIGHT;	/* ��� ����� ������� �������� ������������ */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_RIGHT; \
		/* PCMSK_PORT |= (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); */ /* ���������� ���������� �� �������� �� �������� ����� */ \
		/* PCICR |= (PCICR_BIT); */ \
	} while (0)

	#define NMEA_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* ��� ����� DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
	} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* ��� ����� DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
	} while (0)

	// FROMCAT_RTS_INITIALIZE variations
	#if FROMCAT_BIT_RTS != 0
		#define FROMCAT_RTS_INITIALIZE() do { \
			FROMCAT_TARGET_PORT_RTS |= FROMCAT_BIT_RTS;		/* enable pull-up */ \
			FROMCAT_TARGET_DDR_RTS &= ~ FROMCAT_BIT_RTS;	/* ��� ����� RTS */ \
		} while (0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)
	#endif /* FROMCAT_BIT_RTS != 0 */

	#define ENCODER_INITIALIZE() do { \
		EICRA |= 0x05; \
		EIMSK |= 0x03; \
		EIFR |= 0x03; \
		ENCODER_TARGET_PORT |= ENCODER_BITS; /* PULL-UP */  \
		ENCODER_DIRECTION_PORT &= ~ ENCODER_BITS; \
	} while (0)

#elif CPUSTYLE_ATMEGA32

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
		PTT_TARGET_PORT |= PTT_BIT_PTT;	/* ������� pull-up �� ���� ����. */ \
		PTT_TARGET_DDR &= ~ PTT_BIT_PTT;	/* ��� ����� PTT */ \
	} while (0)

	#define ELKEY_INITIALIZE() do { \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_LEFT;	/* ��� ����� ������ �������� ������������ */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_LEFT; \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_RIGHT;	/* ��� ����� ������� �������� ������������ */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_RIGHT; \
	} while (0)

	#define NMEA_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* ��� ����� DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
	} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* ��� ����� DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* ���������� ���������� �� ��������� ��������� DTR */ \
	} while (0)

	// FROMCAT_RTS_INITIALIZE variations
	#if FROMCAT_BIT_RTS != 0
		#define FROMCAT_RTS_INITIALIZE() do { \
			FROMCAT_TARGET_PORT_RTS |= FROMCAT_BIT_RTS;		/* enable pull-up */ \
			FROMCAT_TARGET_DDR_RTS &= ~ FROMCAT_BIT_RTS;	/* ��� ����� RTS */ \
		} while (0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)
	#endif /* FROMCAT_BIT_RTS != 0 */

	#define ENCODER_INITIALIZE() do { \
		GICR |= 0xC0; \
		MCUCR = 0x05; \
		MCUCSR = 0x00; \
		GIFR = 0xC0; \
		ENCODER_TARGET_PORT |= ENCODER_BITS; /* PULL-UP */  \
		ENCODER_DIRECTION_PORT &= ~ ENCODER_BITS; \
	} while (0)

#endif /* CPUSTYLE_ATMEGA_XXX4 */

#if CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA

	// Variations of SPI_ADDRESS_AEN_INITIALIZE
	// Variations of SPI_ADDRESS_NAEN_INITIALIZE
	#if SPI_AEN_BIT != 0

		#define SPI_ADDRESS_AEN_INITIALIZE() do { \
			/* ���������������� �������� ������ SPI */ \
			SPI_ADDRESS_DDR |= SPI_ADDRESS_BITS; \
			/* ���������������� SS SPI */ \
			SPI_AEN_PORT &= ~ (SPI_AEN_BIT); \
			SPI_AEN_DDR |= (SPI_AEN_BIT); \
			} while (0)

		#define SPI_ADDRESS_NAEN_INITIALIZE() do { \
			} while (0)

	#elif SPI_NAEN_BIT != 0

		#define SPI_ADDRESS_NAEN_INITIALIZE() do { \
			/* ���������������� �������� ������ SPI */ \
			SPI_ADDRESS_DDR |= SPI_ADDRESS_BITS; \
			/* ���������������� SS SPI */ \
			SPI_NAEN_PORT &= ~ (SPI_NAEN_BIT); \
			SPI_NAEN_DDR |= (SPI_NAEN_BIT); \
			} while (0)

		#define SPI_ADDRESS_AEN_INITIALIZE() do { \
			} while (0)

	#else /* SPI_AEN_BIT != 0 */

		#define SPI_ADDRESS_NAEN_INITIALIZE() do { \
			} while (0)

		#define SPI_ADDRESS_AEN_INITIALIZE() do { \
			} while (0)

	#endif /* SPI_AEN_BIT != 0 */

	// Variations of SPI_IOUPDATE_INITIALIZE
	#if SPI_IOUPDATE_BIT != 0

		#define SPI_IOUPDATE_INITIALIZE() do { \
			SPI_IOUPDATE_PORT |= SPI_IOUPDATE_BIT;	/* ���������� ��������� - "1" */ \
			SPI_IOUPDATE_DDR |= SPI_IOUPDATE_BIT; \
			} while (0)

	#else /* SPI_IOUPDATE_BIT != 0 */

		#define SPI_IOUPDATE_INITIALIZE() do { \
			} while (0)

	#endif /* SPI_IOUPDATE_BIT != 0 */

	// Variations of SPI_IORESET_INITIALIZE
	#if SPI_IORESET_BIT != 0

		// ������ ����������. ���������� ����� ���� ����� �� ���.
		// � ����� ������ ����� ���� ���.

		#define SPI_IORESET_INITIALIZE() do { \
			SPI_IORESET_PORT &= ~ SPI_IORESET_BIT; \
			SPI_IORESET_DDR |= SPI_IORESET_BIT; \
			} while (0)

	#else /* SPI_IOUPDATE_BIT != 0 */

		#define SPI_IORESET_INITIALIZE() do { \
			} while (0)

	#endif /* SPI_IOUPDATE_BIT != 0 */

#endif /* CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA */

#endif /* ATMEGA_CPUSTYLE_V7_H_INCLUDED */
