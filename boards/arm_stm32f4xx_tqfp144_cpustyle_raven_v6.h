/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

// ��������� � DSP ���������� "��������-2" �� ���������� 
// STM32F746ZGT6 ������� NAU8822L � FPGA EP4CE22E22I7N
// � �������� DUAL WATCH
// Rmainunit_v4bm.pcb

#ifndef ARM_STM32F4XX_TQFP144_CPUSTYLE_RAVEN_V6_H_INCLUDED
#define ARM_STM32F4XX_TQFP144_CPUSTYLE_RAVEN_V6_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0:
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA9/PA10 pins

#define WITHSPI16BIT	1		/* �������� ������������� 16-�� ������ ���� ��� ������ �� SPI */
#define WITHSPIHW 		1	/* ������������� ����������� ����������� SPI */
#define WITHSPIHWDMA 	1	/* ������������� DMA ��� ������ �� SPI */
//#define WITHSPISW 	1	/* ������������� ������������ ���������� SPI. ������ ������� ��� ������ - ��������� ����� ���������� ��-�� ��������� � I2C */

//#define WITHTWIHW 	1	/* ������������� ����������� ����������� TWI (I2C) */
#define WITHTWISW 	1	/* ������������� ������������ ����������� TWI (I2C) */

#define WITHI2SHW	1	/* ������������� I2S2 & I2S3 - ����������	*/
#define WITHSAI1HW	1	/* ������������� SAI1 - FPGA ��� IF codec	*/
//#define WITHSAI2HW	1	/* ������������� SAI2 - FPGA ��� IF codec	*/

#define WITHCPUDACHW	1	/* ������������� ����������� � ��������� DAC */
#define WITHCPUADCHW 	1	/* ������������� ����������� � ��������� ADC */

#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER � 4-bit bus width */

#if 1

	#define WITHUSBHW	1	/* ������������ ���������� � ��������� ��������� USB */
	#define WITHUSBHWVBUSSENSE	1	/* ������������ ���������������� ����� VBUS_SENSE */
	//#define WITHUSBHWHIGHSPEED	1	/* ������������ ���������� � ��������� ��������� USB HS */
	//#define WITHUSBHWHIGHSPEEDDESC	1	/* ��������� ����������� ����������� ��� ��� HIGH SPEED */
	#define WITHUSBHW_DEVICE	USB_OTG_FS	/* �� ���� ���������� �������������� ���������������� DEVUCE	*/

	#define WITHUSBUAC		1	/* ������������ ����������� �������� ����� �� USB ���������� */
	#define WITHUSBUAC3		1	/* ����������� ��� ������ �������� ����� */
	//#define WITHUABUACOUTAUDIO48MONO	1	/* ��� ���������� ������� ������� � endpoints */

	#define WITHUSBCDC		1	/* ACM ������������ ����������� ���������������� ���� �� USB ���������� */
	//#define WITHUSBCDCEEM	1	/* EEM ������������ Ethernet Emulation Model �� USB ���������� */
	//#define WITHUSBCDCECM	1	/* ECM ������������ Ethernet Control Model �� USB ���������� */
	//#define WITHUSBHID		1	/* HID ������������ Human Interface Device �� USB ���������� */

	#define WITHUART2HW	1	/* ������������ ������������ ���������� ����������������� ����� #2 */

	#define WITHCAT_CDC		1	/* ������������ ����������� ���������������� ���� �� USB ���������� */
	#define WITHMODEM_CDC	1
	#define WITHDEBUG_USART2	1
	#define WITHNMEA_USART2		1	/* ���� ����������� GPS/GLONASS */

#else

	#define WITHUART1HW	1	/* ������������ ������������ ���������� ����������������� ����� #1 */
	#define WITHUART2HW	1	/* ������������ ������������ ���������� ����������������� ����� #2 */

	#define WITHCAT_USART1		1
	#define WITHDEBUG_USART1	1
	#define WITHMODEM_USART1	1
	#define WITHNMEA_USART2		1	/* ���� ����������� GPS/GLONASS */

	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pioa_altfn2((1U << 9) | (1U << 10), AF_USART1); /* PA9: TX DATA line (2 MHz), PA10: RX data line */ \
			arm_hardware_pioa_updown((1U << 10), 0);	/* PA10: pull-up RX data */ \
		} while (0)

#endif

#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_piof_outputs2m(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_piof_outputs2m(LS020_RST, LS020_RST); \
	} while (0)

#if LCDMODE_SPI_NA
	// ��� ����������� ������� ������ RS

	#define LS020_RS_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 3)			// PF3

#elif LCDMODE_SPI_RN
	// ��� ����������� ������� ������ RESET

	#define LS020_RESET_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RST			(1u << 2)			// PF2 D6 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	// ��� ����������� ������� RESET � RS

	#define LS020_RS_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 3)			// PF3

	#define LS020_RESET_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RST			(1u << 2)			// PF2 D6 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	// ������ ����������� ��� ���������� WH2002 ��� ������������ HD44780.
	#define LCD_DATA_INPUT			(GPIOF->IDR)

	// E (enable) bit
	#define LCD_STROBE_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_STROBE_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)

	// RS & WE bits
	#define LCD_RS_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_RS_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LCD_WE_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_WE_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LCD_STROBE_BIT			(1u << 6)	// PF6

	#define WRITEE_BIT				(1u << 5)	// PF5
	
	#define ADDRES_BIT				(1u << 4)	// PF4

	#define LCD_DATAS_BITS			((1u << 3) | (1u << 2) | (1u << 1) | (1u << 0))	// PF3..PF0
	#define LCD_DATAS_BIT_LOW		0		// ����� ��� ������ ������� � ����� ��������� � �����

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* �������� ������ � ���� LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { /* ������ ������ (�� ���������) */ \
			const portholder_t t = (portholder_t) (v) << LCD_DATAS_BIT_LOW; \
			GPIOF->BSRR = BSRR_S(t & LCD_DATAS_BITS) | BSRR_C(~ t & LCD_DATAS_BITS); \
			__DSB(); \
		} while (0)


	/* ������������� ����������� ������� ���������� ��� ���������� HD44780 - ������ ����� ������� */
	#define LCD_CONTROL_INITIALIZE() \
		do { \
			arm_hardware_piof_outputs2m(LCD_STROBE_BIT | WRITEE_BIT | ADDRES_BIT, 0); \
		} while (0)
	/* ������������� ����������� ������� ���������� ��� ���������� HD44780 - WE=0 */
	#define LCD_CONTROL_INITIALIZE_WEEZERO() \
		do { \
			arm_hardware_piof_outputs2m(LCD_STROBE_BIT | WRITEE_BIT_ZERO | ADDRES_BIT, 0); \
		} while (0)
	/* ������������� ����������� ������� ���������� ��� ���������� HD44780 - WE ����������� - ������ � ���������� ������� */
	#define LCD_CONTROL_INITIALIZE_WEENONE() \
		do { \
			arm_hardware_piof_outputs2m(LCD_STROBE_BIT | ADDRES_BIT, 0); \
		} while (0)

	#define LCD_DATA_INITIALIZE_READ() \
		do { \
			arm_hardware_piof_inputs(LCD_DATAS_BITS);	/* ����������� ���� �� ������ � ������� */ \
		} while (0)

	#define LCD_DATA_INITIALIZE_WRITE(v) \
		do { \
			arm_hardware_piof_outputs2m(LCD_DATAS_BITS, (v) << LCD_DATAS_BIT_LOW);	/* ������� ������ ����� */ \
		} while (0)

#endif

#if WITHENCODER
	// ������ ����������� ��������
	// Encoder inputs: PF8 - PHASE A, PF7 = PHASE B
	// ����������� ����� (����� ���������� �������������).

	#define ENCODER_INPUT_PORT			(GPIOF->IDR) 
	#define ENCODER_BITS ((1u << 8) | (1u << 7))		// PF8 & PF7
	#define ENCODER_SHIFT 7

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_piof_inputs(ENCODER_BITS); \
			arm_hardware_piof_updown(ENCODER_BITS, 0); \
			arm_hardware_piof_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY); \
		} while (0)

#endif

#if WITHI2SHW
	// ���������������� I2S2 � I2S3
	#define I2S2HW_INITIALIZE() do { \
		enum { \
			I2S2_WS = (1U << 12),		/* PB12	*/ \
			I2S2_CK = (1U << 13),		/* PB13	*/ \
			I2S2_SD = (1U << 15),		/* PB15	- �������� */ \
			\
			I2S3_WS = (1U << 15),		/* PA15	*/ \
			I2S3_CK = (1U << 3),		/* PB3	*/ \
			I2S3_SD = (1U << 5),		/* PB5	- ���� */ \
			\
		}; \
		arm_hardware_piob_altfn2(I2S2_WS, AF_SPI2); \
		arm_hardware_piob_altfn2(I2S2_CK, AF_SPI2); \
		arm_hardware_piob_altfn2(I2S2_SD, AF_SPI2); \
		arm_hardware_pioa_altfn2(I2S3_WS, AF_SPI3); \
		arm_hardware_piob_altfn2(I2S3_CK, AF_SPI3); \
		arm_hardware_piob_altfn2(I2S3_SD, AF_SPI3); \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI1HW
	#define SAI1HW_INITIALIZE()	do { \
		/*arm_hardware_pioe_altfn20(1U << 2, AF_SAI); */	/* PE2 - SAI1_MCK_A - 12.288 MHz	*/ \
		arm_hardware_pioe_altfn2(1U << 4, AF_SAI);			/* PE4 - SAI1_FS_A	- 48 kHz	*/ \
		arm_hardware_pioe_altfn20(1U << 5, AF_SAI);			/* PE5 - SAI1_SCK_A	*/ \
		arm_hardware_pioe_altfn2(1U << 6, AF_SAI);			/* PE6 - SAI1_SD_A	(i2s data to codec)	*/ \
		arm_hardware_pioe_altfn2(1U << 3, AF_SAI);			/* PE3 - SAI1_SD_B	(i2s data from codec)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI2HW
	/* 
	��������� ���� SAI2 ���������������� ��� SLAVE � �������������� �� SAI1,
	�� ������� �������� ��������� ������ SAI2_SD_A 
	*/
	#define SAI2HW_INITIALIZE()	do { \
		/* arm_hardware_pioe_altfn20(1U << 0, AF_SAI2); */	/* PE0 - SAI2_MCK_A - 12.288 MHz	*/ \
		/* arm_hardware_piod_altfn2(1U << 12, AF_SAI2); */	/* PD12 - SAI2_FS_A	- 48 kHz	*/ \
		/* arm_hardware_piod_altfn20(1U << 13, AF_SAI2); */	/* PD13 - SAI2_SCK_A	*/ \
		/* arm_hardware_piod_altfn2(1U << 11, AF_SAI2); */		/* PD11 - SAI2_SD_A	(i2s data to codec)	*/ \
		arm_hardware_pioe_altfn2(1U << 11, AF_SAI2);	/* PE11 - SAI2_SD_B	(i2s data from codec)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

/* ������������� ����� � ARM ������������ */

#if WITHUART1HW && (WITHCAT || WITHNMEA)
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) // was PINA 
	#define FROMCAT_BIT_RTS				(1u << 11)	/* ������ RTS �� FT232RL	*/

	/* ����������� �� ����� RS-232, ������ PPS �� GPS/GLONASS/GALILEO ������ */
	#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR) // was PINA 
	#define FROMCAT_BIT_DTR				(1u << 12)	/* ������ DTR �� FT232RL	*/

	/* ����������� �� ����� RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(FROMCAT_BIT_DTR); \
			arm_hardware_pioa_updown(FROMCAT_BIT_DTR, 0); \
		} while (0)

	/* ������� �� �������� �� ����� RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(FROMCAT_BIT_RTS); \
			arm_hardware_pioa_updown(FROMCAT_BIT_RTS, 0); \
		} while (0)

	/* ������ PPS �� GPS/GLONASS/GALILEO ������ */
	#define NMEA_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(FROMCAT_BIT_DTR); \
			arm_hardware_pioa_onchangeinterrupt(FROMCAT_BIT_DTR, FROMCAT_BIT_DTR, FROMCAT_BIT_DTR, ARM_SYSTEM_PRIORITY); \
		} while (0)

#endif

#if (WITHCAT && WITHCAT_CDC)

	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) // was PINA 
	//#define FROMCAT_BIT_RTS				(1u << 11)	/* ������ RTS �� FT232RL	*/

	/* ����������� �� ����� RS-232, ������ PPS �� GPS/GLONASS/GALILEO ������ */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR) // was PINA 
	//#define FROMCAT_BIT_DTR				(1u << 12)	/* ������ DTR �� FT232RL	*/

	/* ����������� �� ������������ CDC ����� */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* ������� �� �������� �� ������������ CDC �����*/
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if WITHSDHCHW
	#if WITHSDHCHW4BIT
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1U << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1U << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1U << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_altfn50(1U << 9, AF_SDIO);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_altfn50(1U << 10, AF_SDIO);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_altfn50(1U << 11, AF_SDIO);	/* PC11 - SDIO_D3	*/ \
		} while (0)
		/* ��������� ��������� �� SD ����� - ����� ��� ���������� power cycle �� ��������� ��������� ������� ����� ������� ����������. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1U << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1U << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1U << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_inputs(1U << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_inputs(1U << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_inputs(1U << 11);	/* PC11 - SDIO_D3	*/ \
			arm_hardware_piod_updown(0, 1U << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1U << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1U << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_updown(0, 1U << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_updown(0, 1U << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_updown(0, 1U << 11);	/* PC11 - SDIO_D3	*/ \
		} while (0)
	#else /* WITHSDHCHW4BIT */
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1U << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1U << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1U << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
		} while (0)
		/* ��������� ��������� �� SD ����� - ����� ��� ���������� power cycle �� ��������� ��������� ������� ����� ������� ����������. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1U << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1U << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1U << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_piod_updown(0, 1U << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1U << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1U << 8);	/* PC8 - SDIO_D0	*/ \
		} while (0)
	#endif /* WITHSDHCHW4BIT */

	#define HARDWARE_SDIO_WP_BIT	(1U << 9)	/* PF9 - SDIO_WP */
	#define HARDWARE_SDIO_CD_BIT	(1U << 10)	/* PF10 - SDIO_SENSE */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piof_inputs(HARDWARE_SDIO_WP_BIT); /* PF9 - SDIO_WP */ \
			arm_hardware_piof_updown(HARDWARE_SDIO_WP_BIT, 0); \
			arm_hardware_piof_inputs(HARDWARE_SDIO_CD_BIT); /* PF10 - SDIO_SENSE */ \
			arm_hardware_piof_updown(HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)

	#define HARDWARE_SDIOSENSE_CD() ((GPIOF->IDR & HARDWARE_SDIO_CD_BIT) == 0)	/* �������� ��������� ������� CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() ((GPIOF->IDR & HARDWARE_SDIO_WP_BIT) != 0)	/* �������� ��������� ������� CARD WRITE PROTECT */

	/* ���� ������� SD CARD ����������� ����� � ���������� */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		/* arm_hardware_pio3_outputs(HARDWARE_SDIO_POWER_BIT, HARDWARE_SDIO_POWER_BIT); */ /* ������� ��������� */ \
		} while (0)

#endif /* WITHSDHCHW */

#if WITHTX

	// txpath outputs not used
	////#define TXPATH_TARGET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	////#define TXPATH_TARGET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	// 
	#define TXGFV_RX		(1u << 4)
	#define TXGFV_TRANS		0			// ������� ����� �������� ����� � ��������
	#define TXGFV_TX_SSB	(1u << 0)
	#define TXGFV_TX_CW		(1u << 1)
	#define TXGFV_TX_AM		(1u << 2)
	#define TXGFV_TX_NFM	(1u << 3)

	#define TXPATH_INITIALIZE() \
		do { \
		} while (0)

	// PTT input - PD10
	#define PTT_TARGET_PIN				(GPIOD->IDR)
	#define PTT_BIT_PTT					(1U << 10)		// PD10

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(PTT_BIT_PTT); \
			arm_hardware_piod_updown(PTT_BIT_PTT, 0); \
		} while (0)

#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_TARGET_PIN			(GPIOD->IDR)
	#define ELKEY_BIT_LEFT				(1U << 8)		// PD8
	#define ELKEY_BIT_RIGHT				(1U << 9)		// PD9

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piod_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
//#define SPI_IOUPDATE_BIT		(1U << 15)	// * PA15

// ����� ����������� ��� ������ ��� �������� �����������
#define SPI_ALLCS_PORT_S(v)	do { GPIOG->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_ALLCS_PORT_C(v)	do { GPIOG->BSRR = BSRR_C(v); __DSB(); } while (0)

#define SPI_CSEL_PG15	(1U << 15)	// PG15 ext1
#define SPI_CSEL_PG8	(1U << 8)	// PG8 nvmem FM25L16B
#define SPI_CSEL_PG7	(1U << 7)	// PG7 board control registers chain
#define SPI_CSEL_PG6	(1U << 6)	// PG6 on-board codec1 NAU8822L
//#define SPI_CSEL_PG5	(1U << 5)	// PG5 FPGA CS2 - used as overflov signal from ADC
#define SPI_CSEL_PG1	(1U << 1)	// PG0 FPGA control registers CS
#define SPI_CSEL_PG0	(1U << 0)	// PG1 FPGA NCO1 registers CS

// ����� ������ ���� ����������� ��� ���� ������������ CS � ����������.
#define SPI_ALLCS_BITS ( \
	SPI_CSEL_PG15	| 	/* PG15 ext1 */ \
	SPI_CSEL_PG8	| 	/* PG8 nvmem FM25L16B */ \
	SPI_CSEL_PG7	| 	/* PG7 board control registers chain */ \
	SPI_CSEL_PG6	| 	/* PG6 on-board codec1 NAU8822L */ \
	/*SPI_CSEL_PG5	| */	/* PG5 FPGA CS2 */ \
	SPI_CSEL_PG1	| 	/* PG0 FPGA control registers CS */ \
	SPI_CSEL_PG0	| 	/* PG1 FPGA NCO1 registers CS */ \
	0)

#define SPI_ALLCS_BITSNEG 0		// ������, �������� ��� "1"

//#define SPI_NAEN_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
//#define SPI_NAEN_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)

//#define SPI_NAEN_BIT (1u << 7)		// * PE7 used

/* ������������� ���� ������ ������������ ��������� */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_piog_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
	} while (0)


// MOSI & SCK port
#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
#define	SPI_SCLK_BIT			(1U << 5)	// * PA5 ���, ����� ������� ���� ������������� SPI

#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
#define	SPI_MOSI_BIT			(1U << 7)	// * PA7 ���, ����� ������� ���� ����� (��� ���� � ������ ���������������� SPI).

#define SPI_TARGET_MISO_PIN		(GPIOB->IDR)		// was PINA 
#define	SPI_MISO_BIT			(1U << 4)	// * PB4 ���, ����� ������� ���� ���� � SPI.

#define SPIIO_INITIALIZE() do { \
		arm_hardware_pioa_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
		arm_hardware_piob_inputs(SPI_MISO_BIT); \
	} while (0)


#define HARDWARE_SPI_CONNECT() do { \
		arm_hardware_piob_altfn50(SPI_MISO_BIT, AF_SPI1); /* � ���� ����������� � ����� � ������ ������������� �� ALT FN */ \
		arm_hardware_pioa_altfn50(SPI_MOSI_BIT | SPI_SCLK_BIT, AF_SPI1); /* � ���� ����������� � ����� � ������ ������������� �� ALT FN */ \
	} while (0)
#define HARDWARE_SPI_DISCONNECT() do { \
		arm_hardware_pioa_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
		arm_hardware_piob_inputs(SPI_MISO_BIT); \
	} while (0)
#define HARDWARE_SPI_CONNECT_MOSI() do { \
		arm_hardware_pioa_altfn50(SPI_MOSI_BIT, AF_SPI1);	/* PIO disable for MOSI bit (SD CARD read support) */ \
	} while (0)
#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
		arm_hardware_pioa_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
	} while (0)

#define HARDWARE_USART2_INITIALIZE() do { \
		arm_hardware_piod_altfn2((1U << 5) | (1U << 6), AF_USART2); /* PD5: TX DATA line (2 MHz), PD6: RX data line */ \
		arm_hardware_piod_updown((1U << 6), 0);	/* PD6: pull-up RX data */ \
	} while (0)

#define HARDWARE_SIDETONE_INITIALIZE() do { \
	} while (0)

#if KEYBOARD_USE_ADC
	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)
#else
	#define HARDWARE_KBD_INITIALIZE() do { \
		arm_hardware_pioa_inputs(KBD_MASK); \
		} while (0)
#endif

#if 1 // WITHTWISW
	#define TARGET_TWI_TWCK_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWD_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWCK		(1u << 6)		// PB6 SCL
	#define TARGET_TWI_TWD		(1u << 7)		// PB7 SDA

	// ������������� ����� ������ �����-������ ��� ����������� ���������� I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_piob_opendrain(TARGET_TWI_TWCK | TARGET_TWI_TWD, TARGET_TWI_TWCK | TARGET_TWI_TWD); \
		} while (0) 
	// ������������� ����� ������ �����-������ ��� ���������� ���������� I2C
	// ������������� ������� � ������������� ����������
		#define	TWIHARD_INITIALIZE() do { \
			arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C1);	/* AF=4 */ \
		} while (0) 


#endif // WITHTWISW

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define FPGA_NCONFIG_BIT		(1UL << 13)	/* PE13 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(GPIOE->IDR)
	#define FPGA_CONF_DONE_BIT		(1UL << 14)	/* PE14 bit conneced to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		(GPIOE->IDR)
	#define FPGA_NSTATUS_BIT		(1UL << 15)	/* PE15 bit conneced to NSTATUS pin ALTERA FPGA */

	/* ������������� ������� GPIO ���������� ��� ��������� ��������� � ����������� ��������� FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pioe_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pioe_inputs(FPGA_CONF_DONE_BIT | FPGA_NSTATUS_BIT); \
		} while (0)

	/* �� ��������� ��������� ���������, ��������������������� �� FPGA (����� � user mode). */
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(400), 1)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// ���� ������� � ������� ������������� FIR ������� � FPGA
	#define TARGET_FPGA_FIR_CLK_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR_CLK_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR_CLK_BIT (1U << 7)	/* PE7 - fir CLK */

	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (1U << 8)	/* PE8 - fir1 WE */

	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (1U << 9)	/* PE9 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR_CLK_BIT, TARGET_FPGA_FIR_CLK_BIT); \
		} while (0)
#endif /* WITHDSPEXTFIR */

/* ��������� ��������� ������������ ��� */
#define TARGET_FPGA_OVF_PIN		(GPIOG->IDR)
#define TARGET_FPGA_OVF_BIT		(1u << 5)	// PG5
#define TARGET_FPGA_OVF_GET		((TARGET_FPGA_OVF_PIN & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
#define TARGET_FPGA_OVF_INITIALIZE() do { \
			arm_hardware_piog_inputs(TARGET_FPGA_OVF_BIT); \
		} while (0)

#if WITHCPUDACHW
	/* �������� ������ ������ */
	#define HARDWARE_DAC_INITIALIZE() do { \
			DAC1->CR = DAC_CR_EN1; /* DAC1 enable */ \
		} while (0)
	#define HARDWARE_DAC_ALC(v) do { /* ����� 12-������� �������� �� ��� - ����� 1 */ \
			DAC1->DHR12R1 = (v); /* DAC1 set value */ \
		} while (0)
#endif /* WITHCPUDACHW */

#if WITHCPUADCHW
	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pioa_analoginput(((ainmask) >> 0) & 0xff);	/* ADC12_IN0..ADC12_IN7 */ \
			arm_hardware_piob_analoginput(((ainmask) >> 8) & 0x03);	/* ADC12_IN8..ADC12_IN0 */ \
			arm_hardware_pioc_analoginput(((ainmask) >> 10) & 0x3f);	/* ADC12_IN10..ADC12_IN15 */ \
		} while (0)
#endif /* WITHCPUADCHW */

// ���������� ������ ��� ����������.
enum 
{ 
#if WITHREFSENSOR
	VREFIX = 17,		// Reference voltage
#endif /* WITHREFSENSOR */
#if WITHTEMPSENSOR
	TEMPIX = 16,
#endif /* WITHTEMPSENSOR */
#if WITHVOLTLEVEL 
	VOLTSOURCE = 8,		// PB0 ������� ����� �������� ����������, ��� ���
#endif /* WITHVOLTLEVEL */

#if WITHPOTGAIN
	POTIFGAIN = 2,		// PA2 IF GAIN
	POTAFGAIN = 3,		// PA3 AF GAIN
#endif /* WITHPOTGAIN */

#if WITHPOTWPM
	POTWPM = 6,			// PA6 ������������ ���������� ��������� �������� � ���������
#endif /* WITHPOTWPM */
#if WITHPOTPOWER
	POTPOWER = 6,			// ����������� ��������
#endif /* WITHPOTPOWER */

	ALCINIX = 9,		// PB1 ALC IN

#if WITHCURRLEVEL
	PASENSEIX = 1,		// PA1 PA current sense - ACS712-05 chip
#endif /* WITHCURRLEVEL */

#if WITHSWRMTR
	PWRI = 14,			// PC4
	FWD = 14, REF = 15,	// PC5	SWR-meter
#endif /* WITHSWRMTR */
	KI0 = 10, KI1 = 11, KI2 = 12, KI3 = 13, KI4 = 0	// ����������
};

#define KI_COUNT 5	// ���������� ������������ ��� ���������� ������ ���
#define KI_LIST	KI4, KI3, KI2, KI1, KI0,	// �������������� ��� ������� �������������

#if WITHUSBHW
	/**USB_OTG_FS GPIO Configuration    
	PA9     ------> USB_OTG_FS_VBUS
	PA11     ------> USB_OTG_FS_DM
	PA12     ------> USB_OTG_FS_DP 
	*/
	#define	USBD_FS_INITIALIZE() do { \
		arm_hardware_pioa_altfn50((1U << 11) | (1U << 12), AF_OTGFS);			/* PA11, PA12 - USB_OTG_FS	*/ \
		arm_hardware_pioa_inputs(1U << 9);		/* PA9 - USB_OTG_FS_VBUS */ \
		arm_hardware_pioa_updownoff((1U << 9) | (1U << 11) | (1U << 12)); \
		} while (0)

	/**USB_OTG_HS GPIO Configuration    
	PB13     ------> USB_OTG_HS_VBUS
	PB14     ------> USB_OTG_HS_DM
	PB15     ------> USB_OTG_HS_DP 
	*/
	#define	USBD_HS_FS_INITIALIZE() do { \
		arm_hardware_piob_altfn50((1U << 14) | (1U << 15), AF_OTGHS_FS);			/* PB14, PB15 - USB_OTG_HS	*/ \
		arm_hardware_piob_inputs(1U << 13);		/* PB13 - USB_OTG_HS_VBUS */ \
		arm_hardware_piob_updownoff((1U << 13) | (1U << 14) | (1U << 15)); \
		} while (0)

	#define	USBD_HS_ULPI_INITIALIZE() do { \
		} while (0)
#endif /* WITHUSBHW */

	/* ����������������, ������� ������ �������� � ���� ��� ������������� */
	#define	HARDWARE_INITIALIZE() do { \
		HARDWARE_SIDETONE_INITIALIZE(); \
		HARDWARE_KBD_INITIALIZE(); \
		HARDWARE_DAC_INITIALIZE(); \
		} while (0)

#endif /* ARM_STM32F4XX_TQFP144_CPUSTYLE_RAVEN_V6_H_INCLUDED */
