/* $Id$ */
/* board-specific CPU DIP40/TQFP44 attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef ATMEGA_CPU_UA3DKC_H_INCLUDED
#define ATMEGA_CPU_UA3DKC_H_INCLUDED 1

#define WITHSPIHW 	1	/* ������������� ����������� ����������� SPI */
//#define WITHSPISW 	1	/* ������������� ������������ ���������� SPI. */
#define WITHTWIHW 	1	/* ������������� ����������� ����������� TWI (I2C) */
//#define WITHTWISW 	1	/* ������������� ������������ ����������� TWI (I2C) */
#define WITHCPUADCHW 	1	/* ������������� ADC */

#define WITHUART1HW	1	/* ������������ ������������ ���������� ����������������� ����� #1 */
//#define WITHUART2HW	1	/* ������������ ������������ ���������� ����������������� ����� #2 */

//#define WITHCAT_CDC		1	/* ������������ ����������� ���������������� ���� �� USB ���������� */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1

// ����� ������� ���������� �� Si570 � AD9834 http://www.cqham.ru/forum/showthread.php?23945-�������-����������-��-Si570-�-AD9834

#if LCDMODE_SPI_RN
	#define LS020_RESET_PORT PORTC
	#define LS020_RESET_DDR DDRC
	//#define LS020_RS	(1U << PC7)			// DIP40 Pin 29 - D7 signal in HD44780 socket
	#define LS020_RST	(1U << PC6)			// DIP40 Pin 28 - D6 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	#define LS020_RESET_PORT PORTC
	#define LS020_RESET_DDR DDRC
	//#define LS020_RST	(1U << 2) //(1U << PC6)			// DIP40 Pin 28 - D6 signal in HD44780 socket
	#define LS020_RST	(1U << PC6)			// DIP40 Pin 28 - D6 signal in HD44780 socket

	#define LS020_RS_PORT PORTC
	#define LS020_RS_DDR DDRC
	//#define LS020_RS	(1u << 3) // (1U << PC7)			// DIP40 Pin 29 - D7 signal in HD44780 socket
	#define LS020_RS	(1U << PC7)			// DIP40 Pin 29 - D7 signal in HD44780 socket

#elif LCDMODE_SPI_NA
	#define LS020_RS_PORT PORTC
	#define LS020_RS_DDR DDRC
	//#define LS020_RS	(1u << 3) // (1U << PC7)			// DIP40 Pin 29 - D7 signal in HD44780 socket
	#define LS020_RS	(1U << PC7)			// DIP40 Pin 29 - D7 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	//#undef WITHTWIHW
	#undef WITHTWISW

	// ������ LCD
	#define LCD_DATA_PORT PORTC
	#define LCD_DATA_DIRECTION DDRC
	#define LCD_DATA_INPUT PINC
	#define LCD_DATAS_BIT_LOW PC4		// ����� ��� ������ ������� � ����� ��������� � �����
	#define LCD_DATAS_BITS (0x0f << LCD_DATAS_BIT_LOW)

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* �������� ������ � ���� LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { \
		LCD_DATA_PORT = \
			(LCD_DATA_PORT & ~ LCD_DATAS_BITS) | \
			(((v) << LCD_DATAS_BIT_LOW) & LCD_DATAS_BITS); \
		} while (0)

	// ����� E LCD
	#define LCD_STROBE_PORT 	PORTC
	#define LCD_STROBE_DIRECTION	 	DDRC
	#define LCD_STROBE_BIT 	(1U << PC3)

	// ����� �������� � ����� ������/������
	#define LCD_RS_WE_PORT PORTB
	#define LCD_RS_WE_DIRECTION DDRB

	#define WRITEE_BIT 		(1U << PB1)
	#define ADDRES_BIT 		(1U << PB0)
#endif


#if LCDMODE_UC1608
	#define UC1608_CSP_PORT PORTC
	#define UC1608_CSP_DDR DDRC
	#define UC1608_CSP 	(1U << PC3)			// DIP40 Pin 23 - E signal on HD44780 socket - positive chipselect
	#define SPI_CSEL255	255					// �� ����� ���������� �������� ������������� ��������

#endif


	#define ENCODER_TARGET_PORT PORTD
	#define ENCODER_DIRECTION_PORT DDRD
	#define ENCODER_INPUT_PORT PIND

	#define ENCODER_BITS ((1U << PIND3) | (1U << PIND2))
	#define ENCODER_SHIFT PIND2

#if WITHTX

	#if WITHELKEY
		#define ELKEY_TARGET_PORT PORTA		// ����� ���������� - ������� ������������ �����
		#define ELKEY_TARGET_DDR DDRA		// ������������ �� ���� - ������� ������������ �����
		#define ELKEY_TARGET_PIN PINA		// ���� - ������� ������������ �����

		#define ELKEY_BIT_LEFT (1U << PA1)
		#define ELKEY_BIT_RIGHT (1U << PA2)

		// ���������� ���������� �� �������� �� �������� �����
		// ���������������� ��������� ATMega644
		#define PCMSK_PORT	PCMSK0			// PCMSK0 - PA, PCMSK1 - PB, PCMSK2 - PC, PCMSK3 - PD
		#define PCICR_BIT	(1U << PCIE0)	// PCIE0 - PA, PCIE1 - PB, PCIE2 - PC, PCIE3 - PD
		#define PCIVECT PCINT0_vect			// ������ �����������
	#endif

	#define PTT_TARGET_PORT PORTC	
	#define PTT_TARGET_DDR DDRC
	#define PTT_TARGET_PIN PINC

	#define PTT_BIT_PTT (1U << PC2)
	/* ������������ ���� ���������� ������� ���������� �� ���������� -
	 ���� ���������, �������������� ���� �� ���� �����, ��������� ��� �������� ����������.
		*/

	#define TXPATH_TARGET_PORT PORTD	// ������ ���������� - ���������� ������� ��������� � ������������
	#define TXPATH_TARGET_DDR DDRD		// ������������ �� ����� - ���������� ������� �������� � ������������

	#if 1
		// ���������� ������������ - ������� TXPATH_ENABLE (PA11) � TXPATH_ENABLE_CW (PA10) - ������� ��� ���� �� ������.
		#define TXPATH_BIT_ENABLE_SSB		(1U << PD5)	// DIP40 pin 19
		#define TXPATH_BIT_ENABLE_CW	(1U << PD6)	// DIP40 pin 20
		#define TXPATH_BITS_ENABLE	(TXPATH_BIT_ENABLE_SSB | TXPATH_BIT_ENABLE_CW)

		// �������������� ����������� �����
		#define TXGFV_RX		0
		#define TXGFV_TRANS		0			// ������� ����� �������� ����� � ��������
		#define TXGFV_TX_SSB	TXPATH_BIT_ENABLE_SSB
		#define TXGFV_TX_CW		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_AM		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_NFM	TXPATH_BIT_ENABLE_CW
	#else
		// ���������� ������������ - ������������ ������ ���������� ������
		#define TXPATH_BIT_GATE (1U << PD6)	// �������� ������ �� ���������� - ���������� ������������.
		//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// ������ tx2 - ���������� ������������. ��� ����� �������
		#define TXPATH_BIT_GATE_RX 0	// ������ tx2 - ���������� ������������. ��� ����� �� �������

		// �������������� ����������� �����
		#define TXGFV_RX		TXPATH_BIT_GATE_RX // TXPATH_BIT_GATE ��� �������� � ������ ��������� �� ���� � ��������
		#define TXGFV_TRANS		0			// ������� ����� �������� ����� � ��������
		#define TXGFV_TX_SSB	TXPATH_BIT_GATE
		#define TXGFV_TX_CW		TXPATH_BIT_GATE
		#define TXGFV_TX_AM		TXPATH_BIT_GATE
		#define TXGFV_TX_NFM	TXPATH_BIT_GATE
	#endif

#endif /* WITHTX */

// ��������� ��������� ���� ������������ ��� �������� ����������
// ��������� ������� ������������ �� PD7(OC2)

#define SIDETONE_TARGET_PORT	PORTD	/* ���� ����������, �� ������� ��������� ����� ���� ������������ */
#define SIDETONE_TARGET_DDR		DDRD
#define SIDETONE_TARGET_BIT		(1U << PD7)	// DIP40 pin 21


#if WITHCAT || WITHNMEA
	// ������� ������������ �� �������� � ����������� ����� CAT ���������.
	#define FROMCAT_TARGET_PORT_RTS		PORTD
	#define FROMCAT_TARGET_PIN_RTS		PIND
	#define FROMCAT_TARGET_DDR_RTS		DDRD
	#define FROMCAT_BIT_RTS			(1U << PD4)	/* DIP40 pin 18 ������ RTS �� FT232RL	*/
	/* ����������� �� ����� RS-232, ������ PPS �� GPS/GLONASS/GALILEO ������ */
	#define FROMCAT_TARGET_PORT_DTR		PORTB
	#define FROMCAT_TARGET_PIN_DTR		PINB
	#define FROMCAT_TARGET_DDR_DTR		DDRB
	#define FROMCAT_BIT_DTR			(1U << PB3)	/* DIP40 pin 04 ������ DTR �� FT232RL	*/

	// ���������� ���������� �� ��������� ��������� �� ����� ����������� �� CAT
	// � ���������������� ��������� ATMega644
	#define DTRPCMSK_PORT	PCMSK1			// PCMSK0 - PA, PCMSK1 - PB, PCMSK2 - PC, PCMSK3 - PD
	#define DTRPCICR_BIT	(1U << PCIE1)	// PCIE0 - PA, PCIE1 - PB, PCIE2 - PC, PCIE3 - PD
	#define DTRPCIVECT PCINT1_vect			// ������ �����������
#endif

	// � ���� ������ ����� ��� ���������� IORESET, IOUPDATE ��� � ������ ����������� CS ���������� SPI
	#define SPI_TARGET_SCLK_PORT PORTB
	#define SPI_TARGET_SCLK_DDR DDRB
	#define SPI_TARGET_SCLK_PIN PINB

	#define SPI_TARGET_MOSI_PORT PORTB
	#define SPI_TARGET_MOSI_DDR DDRB
	#define SPI_TARGET_MOSI_PIN PINB

	#define	SPI_SCLK_BIT (1U << PB7)	// DIP40 pin 08: ���, ����� ������� ���� ������������� SPI
	#define	SPI_MOSI_BIT (1U << PB5)	// DIP40 pin 06: ���, ����� ������� ���� ����� ������ SPI.

	#define SPI_TARGET_MISO_PIN PINB
	#define SPI_TARGET_MISO_PORT PORTB
	#define SPI_TARGET_MISO_DDR DDRB

	#define	SPI_MISO_BIT (1U << PB6)	// DIP40 pin 07: ���, ����� ������� ���� ���� � SPI.

#if 0
	#define SPI_ADDRESS_PORT PORTB
	#define SPI_ADDRESS_DDR DDRB
	//#define SPI_ALLCS_PIN PINC

	// ����� ������� (PB0, PB1) ������ ��� ���������� ��������
	#define SPI_CSEL7 (1U << PB2)		// FRAM
	#define SPI_CSEL6 (1U << PB4)		// CTL REGISTER	

		// � ������ ����� ������ ���� �������� ��� ������ �����������.
	#define SPI_ALLCS_BITS (SPI_CSEL7 | SPI_CSEL6)
	#define SPI_ALLCS_BITSNEG 0		// ������, �������� ��� "1"

#else
	#define SPI_NAEN_PORT PORTB
	#define SPI_NAEN_DDR DDRB
	#define SPI_NAEN_PIN PINB

	#define SPI_NAEN_BIT (1U << PB4)	// DIP40 pin 05: 0 - ���������� ������ CS
	#define SPI_ALLCS_BITS	0		// ��������� ��� �������� ����, ��� ������ � ������ ������� CS (��� �����������) �� ���������
	#define SPI_ALLCS_BITSNEG 0		// ������, �������� ��� "1"

	#define SPI_ADDRESS_PORT PORTB
	#define SPI_ADDRESS_DDR DDRB
	//#define SPI_ADDRESS_PIN PINB

	#define SPI_A0 (1U << PB0)			// DIP40 pin 01: ���� ������ ��� ����������� SPI
	#define SPI_A1 (1U << PB1)			// DIP40 pin 02
	#define SPI_A2 (1U << PB2)			// DIP40 pin 03
#endif

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

	// Separated MOSI and MISO signals, only supported if WITHSPIHW used
	#define SPIIO_INITIALIZE() do { \
			HARDWARE_INPUT_INITIALIZE(SPI_TARGET_MISO_PORT, SPI_TARGET_MISO_DDR, SPI_MISO_BIT, SPI_MISO_BIT); /* enable pull-up on MISO */ \
			/* PB4(~SS) ������ ���� �������. SPI_NAEN_BIT - ���������� �����������. */ \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_SCLK_PORT, SPI_TARGET_SCLK_DDR, SPI_SCLK_BIT, SPI_SCLK_BIT); \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_MOSI_PORT, SPI_TARGET_MOSI_DDR, SPI_MOSI_BIT, SPI_MOSI_BIT); \
		} while (0)

	#define TARGET_TWI_TWCK_PORT PORTC
	#define TARGET_TWI_TWCK_DDR DDRC
	#define TARGET_TWI_TWCK_PIN PINC
	#define TARGET_TWI_TWCK	(1U << PC0)	// DIP40 pin 22

	#define TARGET_TWI_TWD_PORT PORTC
	#define TARGET_TWI_TWD_DDR DDRC
	#define TARGET_TWI_TWD_PIN PINC
	#define TARGET_TWI_TWD	(1U << PC1)	// DIP40 pin 23

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

#endif /* ATMEGA_CPU_UA3DKC_H_INCLUDED */
