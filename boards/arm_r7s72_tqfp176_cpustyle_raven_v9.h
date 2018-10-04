/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

// ��������� � DSP ���������� "Storch" �� ���������� Renesas R7S721020VCFP � ������� NAU8822L � FPGA EP4CE22E22I7N
// Rmainunit_v7bm.pcb

#ifndef ARM_R7S72_TQFP176_CPUSTYLE_RAVEN_V9_H_INCLUDED
#define ARM_R7S72_TQFP176_CPUSTYLE_RAVEN_V9_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0:
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA9/PA10 pins

#define WITHSPI16BIT	1		/* �������� ������������� 16-�� ������ ���� ��� ������ �� SPI */
#define WITHSPIHW 		1	/* ������������� ����������� ����������� SPI */
#define WITHSPIHWDMA 	1	/* ������������� DMA ��� ������ �� SPI */
#define WITHSPISW 	1	/* ������������� ������������ ���������� SPI. */

//#define WITHTWIHW 	1	/* ������������� ����������� ����������� TWI (I2C) */
//#define WITHTWISW 	1	/* ������������� ������������ ����������� TWI (I2C) */

#define WITHI2SHW	1	/* ������������� SSIF0 I2S 2*16 bit - ����� ����� */
#define WITHSAI1HW	1	/* ������������� SSIF1 I2S 8*32 bit - FPGA IF codec */
//#define WITHSAI2HW	1	/* ������������� SSIF2 I2S 2*32 (2*24) bit - FPGA ��������	*/

//#define WITHCPUDACHW	1	/* ������������� DAC - � renesas R7S72 ���� */
#define WITHCPUADCHW 	1	/* ������������� ADC */

#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER � 4-bit bus width */

#define WITHUSBHW	1	/* ������������ ���������� � ��������� ��������� USB */
#define WITHUSBHWVBUSSENSE	1	/* ������������ ���������������� ����� VBUS_SENSE */
#define WITHUSBHWHIGHSPEED	1	/* ������������ ���������� � ��������� ��������� USB HS */
#define WITHUSBHWHIGHSPEEDDESC	1	/* ��������� ����������� ����������� ��� ��� HIGH SPEED */
#define WITHUSBHW_DEVICE	(& USB200)	/* �� ���� ���������� �������������� ���������������� DEVUCE	*/

#define WITHUART1HW	1	/* ������������ ������������ ���������� ����������������� ����� #1 */
//#define WITHUART2HW	1	/* ������������ ������������ ���������� ����������������� ����� #2 */

//#define WITHCAT_USART1	1
#define WITHCAT_CDC			1	/* ������������ ����������� ���������������� ���� �� USB ���������� */
#define WITHMODEM_CDC		1
#define WITHDEBUG_USART1	1
//#define WITHMODEM_USART1	1
#define WITHNMEA_USART2		1	/* ���� ����������� GPS/GLONASS */

#define WITHUSBUAC		1	/* ������������ ����������� �������� ����� �� USB ���������� */

#define WITHUSBCDC		1	/* ACM ������������ ����������� ���������������� ���� �� USB ���������� */
//#define WITHUSBCDCEEM	1	/* EEM ������������ Ethernet Emulation Model �� USB ���������� */
//#define WITHUSBCDCECM	1	/* ECM ������������ Ethernet Control Model �� USB ���������� */
//#define WITHUSBHID		1	/* HID ������������ Human Interface Device �� USB ���������� */

// �������-������� ���������/������ ����� � ��������� �����
//
//  ������ �������������:
//		R7S721_TARGET_PORT_S(6, 0x04);	/* P6_2=1 */
//		R7S721_TARGET_PORT_C(6, 0x04);	/* P6_2=0 */

#define R7S721_TARGET_PORT_S(p, v) do { GPIO.PSR ## p = ((v) * 0x10000UL) | (v); __DSB(); } while (0)
#define R7S721_TARGET_PORT_C(p, v) do { GPIO.PSR ## p = ((v) * 0x10000UL) | (0); __DSB(); } while (0)

#define R7S721_INPUT_PORT(p) (GPIO.PPR ## p)

#define WRITEE_BIT				(1u << 12)	// RD/~WR  P3_12 - ������ ���� � "0" - ��� ��� ������ - ��� ���������� �������� �� ���� ������ LCD

/* � ������ ������ ����� ������ � ����� - ��� ������ ���������������� ������ "������" WRITEE_BIT ����� ���� � "1". 
*/
#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_pio3_outputs(WRITEE_BIT, 1 * WRITEE_BIT); /* � ����� ������ ����� �������� "0" */ \
		arm_hardware_pio3_outputs(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_pio3_outputs(WRITEE_BIT, 1 * WRITEE_BIT); /* � ����� ������ ����� �������� "0" */ \
		arm_hardware_pio3_outputs(LS020_RST, LS020_RST); \
	} while (0)

#if LCDMODE_SPI_NA
	// ��� ����������� ������� ������ RS

	#define LS020_RS_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RS_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RS			(1u << 11)			// P3_11 D7 signal in HD44780 socket

#elif LCDMODE_SPI_RN
	// ��� ����������� ������� ������ RESET

	#define LS020_RESET_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RESET_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RST			(1u << 10)			// * P3_10 D6 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	// ��� ����������� ������� RESET � RS

	#define LS020_RS_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RS_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RS			(1u << 11)			// P3_11 D7 signal in HD44780 socket

	#define LS020_RESET_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RESET_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RST			(1u << 10)			// P3_10 D6 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	#error Unsupported configuration 
	// ������ ����������� ��� ���������� WH2002 ��� ������������ HD44780.
	#define LCD_DATA_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)		// P3_11,,P3_8
	#define LCD_DATA_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)		// P3_11,,P3_8

	#define LCD_DATA_INPUT			(R7S721_INPUT_PORT(3))

	// E (enable) bit
	#define LCD_STROBE_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LCD_STROBE_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)

	// RS & WE bits
	#define LCD_RS_WE_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LCD_RS_WE_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)

	// RS bit
	#define LCD_RS_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LCD_RS_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)

	// W bit
	#define LCD_WE_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LCD_WE_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)

	#define LCD_STROBE_BIT			(1u << 14)	// E: P3_14

	#define WRITEE_BIT				(1u << 12)	// RD/~WR  P3_12
	//#define WRITEE_BIT_ZERO				(1u << 12)	// RD/~WR  P3_12
	
	#define ADDRES_BIT				(1u << 13)	// * P3_13

	#define LCD_DATAS_BITS			((1u << 11) | (1u << 10) | (1u << 9) | (1u << 8))	// P3_11,,P3_8
	#define LCD_DATAS_BIT_LOW		8		// ����� ��� ������ ������� � ����� ��������� � �����

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* �������� ������ � ���� LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { /* ������ ������ (�� ���������) */ \
			const portholder_t t = (portholder_t) (v) << LCD_DATAS_BIT_LOW; \
			LCD_DATA_PORT_S(LCD_DATAS_BITS & t); \
			LCD_DATA_PORT_C(LCD_DATAS_BITS & ~ t); \
		} while (0)

	/* ������������� ����������� ������� ���������� ��� ���������� HD44780 - ������ ����� ������� */
	#define LCD_CONTROL_INITIALIZE() \
		do { \
			arm_hardware_pio3_outputs(LCD_STROBE_BIT | WRITEE_BIT | ADDRES_BIT, 0); \
		} while (0)
	/* ������������� ����������� ������� ���������� ��� ���������� HD44780 - WE=0 */
	#define LCD_CONTROL_INITIALIZE_WEEZERO() \
		do { \
			arm_hardware_pio3_outputs(LCD_STROBE_BIT | WRITEE_BIT_ZERO | ADDRES_BIT, 0); \
		} while (0)
	/* ������������� ����������� ������� ���������� ��� ���������� HD44780 - WE ����������� - ������ � ���������� ������� */
	#define LCD_CONTROL_INITIALIZE_WEENONE() \
		do { \
			arm_hardware_pio3_outputs(LCD_STROBE_BIT | ADDRES_BIT, 0); \
		} while (0)

	#define LCD_DATA_INITIALIZE_READ() \
		do { \
			arm_hardware_pio3_inputs(LCD_DATAS_BITS);	/* ����������� ���� �� ������ � ������� */ \
		} while (0)

	#define LCD_DATA_INITIALIZE_WRITE(v) \
		do { \
			arm_hardware_pio3_outputs(LCD_DATAS_BITS, (v) << LCD_DATAS_BIT_LOW);	/* ������� ������ ����� */ \
		} while (0)

#endif

#if 0 && LCDMODE_UC1608
	#define UC1608_CSP_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
	#define UC1608_CSP_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)
	#define UC1608_CSP			(1u << 15)			// * PE15
	#define SPI_CSEL255			255				// �� ����� ���������� �������� ������������� ��������

	#define UC1608_CSP_INITIALIZE() do { \
			arm_hardware_pio1_outputs(UC1608_CSP, 0); \
		} while (0)
#endif


#if WITHENCODER
	// ������ ����������� ��������
	// Encoder inputs: P2_7 - PHASE A, P2_6 = PHASE B
	// Encoder inputs: P2_9 - PHASE A, P2_8 = PHASE B

	// P2_6 ������ ������������ ��� ������ �� ������� �������� ����� ELKEY_BIT_RIGHT

	// P2_7 == IRQ2 (Function 4)
	// P2_9 == IRQ1 (Function 4)

	// ���������:
	// IRQ0:			P6_7,	P1_8, P1_4, 
	// IRQ1:			P6_8,	P1_9, P1_5,									P2_9
	// IRQ2:			P6_9,	P1_10, P1_6,								P2_7
	// IRQ3:	P5_8,	P6_10,	P1_11, P1_7,	P7_3, P7_8
	// IRQ4:	P5_9,			P1_12, P1_0,	P7_2,				P0_0
	// IRQ5:	P5_10,			P1_13. P1_1,	P7_7, P7_9,					P2_2
	// IRQ6:	P5_11,			P1_14, P1_2,	P7_6, 		P8_6,			P2_1
	// IRQ7:					P1_15, P1_3,				P8_7,	P0_2,	P2_0,	

	// ����������� ����� (����� ���������� �������������).

	#define ENCODER_INPUT_PORT			(R7S721_INPUT_PORT(2)) 
	#define ENCODER_BITA (1u << 9)	// P2_9
	#define ENCODER_BITB (1u << 7)	// P2_7
	#define ENCODER_BITS (ENCODER_BITA | ENCODER_BITB)
	//#define ENCODER_SHIFT 6	// ���������� ����� �������� �������� ��� ���� �� ������

	//#define ENCODER2_INPUT_PORT			(R7S721_INPUT_PORT(2)) 
	//#define ENCODER2_BITS ((1u << 9) | (1u << 8))		// P2_9, P2_8
	//#define ENCODER2_SHIFT 8

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_pio2_alternative(ENCODER_BITS, R7S721_PIOALT_4); \
			arm_hardware_irqn_interrupt(1, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt); /* IRQ1, both edges */ \
			arm_hardware_irqn_interrupt(2, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt); /* IRQ2, both edges */ \
		} while (0)

#endif /* WITHENCODER */

#define R7S721_USE_AUDIO_CLK 0	// CKS 1: AUDIO_CLK input 0: AUDIO_X1 input

#if WITHI2SHW
	#define HARDWARE_SSIF0_INITIALIZE() do { \
		arm_hardware_pio6_alternative(1U << 8, R7S721_PIOALT_3); /* P6_8 SSISCK0 */ \
		arm_hardware_pio6_alternative(1U << 9, R7S721_PIOALT_3); /* P6_9 SSIWS0 */ \
		arm_hardware_pio6_alternative(1U << 10, R7S721_PIOALT_3); /* P6_10 SSITxD0 */ \
		arm_hardware_pio6_alternative(1U << 11, R7S721_PIOALT_3); /* P6_11 SSIRxD0 */ \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI1HW
	#define HARDWARE_SSIF1_INITIALIZE() do { \
		arm_hardware_pio5_alternative(1U << 4, R7S721_PIOALT_3); /* P5_4 SSISCK1 */ \
		arm_hardware_pio5_alternative(1U << 5, R7S721_PIOALT_3); /* P5_5 SSIWS1 */ \
		arm_hardware_pio5_alternative(1U << 6, R7S721_PIOALT_3); /* P5_6 SSITxD1 */ \
		arm_hardware_pio5_alternative(1U << 7, R7S721_PIOALT_3); /* P5_7 SSIRxD1 */ \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI2HW
	#define HARDWARE_SSIF2_INITIALIZE() do { \
		arm_hardware_pio5_alternative(1U << 12, R7S721_PIOALT_2); /* P5_12 SSISCK2 */ \
		arm_hardware_pio5_alternative(1U << 13, R7S721_PIOALT_2); /* P5_13 SSIWS2 */ \
		arm_hardware_pio5_alternative(1U << 14, R7S721_PIOALT_2); /* P5_14 SSIDATA2 */ \
	} while (0)
#endif /* WITHSAI2HW */

#if WITHSDHCHW

	#define HARDWARE_SDIO_WP_BIT	(1U << 6)	/* P3_6 SD_WP_0 */
	#define HARDWARE_SDIO_CD_BIT	(1U << 7)	/* P3_7 SD_CD_0 */

	#define HARDWARE_SDIO_INITIALIZE() do { \
		arm_hardware_pio3_alternative(1U << 0, R7S721_PIOALT_2); /* P3_0 SD_D2_0 */ \
		arm_hardware_pio3_alternative(1U << 1, R7S721_PIOALT_2); /* P3_1 SD_D3_0 */ \
		arm_hardware_pio3_alternative(1U << 2, R7S721_PIOALT_2); /* P3_2 SD_CMD_0 */ \
		arm_hardware_pio3_alternative(1U << 3, R7S721_PIOALT_2); /* P3_3 SD_CLK_0 */ \
		arm_hardware_pio3_alternative(1U << 4, R7S721_PIOALT_2); /* P3_4 SD_D0_0 */ \
		arm_hardware_pio3_alternative(1U << 5, R7S721_PIOALT_2); /* P3_5 SD_D1_0 */ \
		/* arm_hardware_pio3_alternative(HARDWARE_SDIO_WP_BIT, R7S721_PIOALT_2); */ /* P3_6 SD_WP_0 */ \
		/* arm_hardware_pio3_alternative(HARDWARE_SDIO_CD_BIT, R7S721_PIOALT_2); */ /* P3_7 SD_CD_0 */ \
	} while (0)

	/* ��������� ��������� �� SD ����� - ����� ��� ���������� power cycle �� ��������� ��������� ������� ����� ������� ����������. */
	#define HARDWARE_SDIO_HANGOFF()	do { \
		arm_hardware_pio3_inputs(1U << 0); /* P3_0 SD_D2_0 */ \
		arm_hardware_pio3_inputs(1U << 1); /* P3_1 SD_D3_0 */ \
		arm_hardware_pio3_inputs(1U << 2); /* P3_2 SD_CMD_0 */ \
		arm_hardware_pio3_inputs(1U << 3); /* P3_3 SD_CLK_0 */ \
		arm_hardware_pio3_inputs(1U << 4); /* P3_4 SD_D0_0 */ \
		arm_hardware_pio3_inputs(1U << 5); /* P3_5 SD_D1_0 */ \
		/* arm_hardware_pio3_alternative(HARDWARE_SDIO_WP_BIT, R7S721_PIOALT_2); */ /* P3_6 SD_WP_0 */ \
		/* arm_hardware_pio3_alternative(HARDWARE_SDIO_CD_BIT, R7S721_PIOALT_2); */ /* P3_7 SD_CD_0 */ \
	} while (0)

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
		arm_hardware_pio3_inputs(HARDWARE_SDIO_WP_BIT); /* P3_6 SD_WP_0 */ \
		arm_hardware_pio3_inputs(HARDWARE_SDIO_CD_BIT); /* P3_7 SD_CD_0 */ \
	} while (0)

	#define HARDWARE_SDIOSENSE_CD() ((R7S721_INPUT_PORT(3) & HARDWARE_SDIO_CD_BIT) == 0)	/* �������� ��������� ������� CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() ((R7S721_INPUT_PORT(3) & HARDWARE_SDIO_WP_BIT) != 0)	/* �������� ��������� ������� CARD WRITE PROTECT */

	/* ���� ������� SD CARD ����������� ����� � ���������� */
	#define HARDWARE_SDIO_POWER_BIT (1uL << 15)	// P3_15
	#define HARDWARE_SDIO_POWER_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define HARDWARE_SDIO_POWER_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)

	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		arm_hardware_pio3_outputs(HARDWARE_SDIO_POWER_BIT, HARDWARE_SDIO_POWER_BIT); /* ������� ��������� */ \
	} while (0)

	/* parameter on not zero for powering SD CARD */
	#define HARDWARE_SDIOPOWER_SET(on)	do { \
		if ((on) != 0) \
			HARDWARE_SDIO_POWER_C(HARDWARE_SDIO_POWER_BIT); \
		else \
			HARDWARE_SDIO_POWER_S(HARDWARE_SDIO_POWER_BIT); \
	} while (0)

#endif /* WITHSDHCHW */
/* ������������� ����� � ARM ������������ */

#if (WITHCAT && WITHCAT_USART1)
	// CAT data lites
	// RXD at P6_14, TXD at P6_15

	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(R7S721_INPUT_PORT(2)) // P2_0
	#define FROMCAT_BIT_RTS				(1u << 0)	/* ������ RTS �� FT232RL	*/

	/* ����������� �� ����� RS-232, ������ PPS �� GPS/GLONASS/GALILEO ������ */
	#define FROMCAT_TARGET_PIN_DTR		(R7S721_INPUT_PORT(2)) // P2_1
	#define FROMCAT_BIT_DTR				(1u << 1)	/* ������ DTR �� FT232RL	*/

	/* ����������� �� ����� RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_DTR); \
		} while (0)

	/* ������� �� �������� �� ����� RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_RTS); \
		} while (0)

#endif /* (WITHCAT && WITHCAT_USART1) */

#if (WITHCAT && WITHCAT_CDC)
	// CAT data lites
	// RXD at P6_14, TXD at P6_15

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(R7S721_INPUT_PORT(2)) // P2_0
	//#define FROMCAT_BIT_RTS				(1u << 0)	/* ������ RTS �� FT232RL	*/

	/* ����������� �� ����� RS-232, ������ PPS �� GPS/GLONASS/GALILEO ������ */
	//#define FROMCAT_TARGET_PIN_DTR		(R7S721_INPUT_PORT(2)) // P2_1
	//#define FROMCAT_BIT_DTR				(1u << 1)	/* ������ DTR �� FT232RL	*/

	/* ����������� �� ����� RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* ������� �� �������� �� ����� RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if (WITHNMEA && WITHNMEA_USART1)
	// CAT data lites
	// RXD at P6_14, TXD at P6_15

	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(R7S721_INPUT_PORT(2)) // P2_0
	#define FROMCAT_BIT_RTS				(1u << 0)	/* ������ RTS �� FT232RL	*/

	/* ����������� �� ����� RS-232, ������ PPS �� GPS/GLONASS/GALILEO ������ */
	#define FROMCAT_TARGET_PIN_DTR		(R7S721_INPUT_PORT(2)) // P2_1
	#define FROMCAT_BIT_DTR				(1u << 1)	/* ������ DTR �� FT232RL	*/

	/* ����������� �� ����� RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_DTR); \
		} while (0)

	/* ������� �� �������� �� ����� RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_RTS); \
		} while (0)

	/* ������ PPS �� GPS/GLONASS/GALILEO ������ */
	#define NMEA_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_DTR); \
			arm_hardware_pio2_onchangeinterrupt(FROMCAT_BIT_DTR, FROMCAT_BIT_DTR, ARM_SYSTEM_PRIORITY, r7s721_nmea_interrupt); \
		} while (0)

#endif /* (WITHNMEA && WITHNMEA_USART1) */

#if WITHUART1HW
	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pio6_alternative(1U << 14, R7S721_PIOALT_5);	/* P6_14: RXD0: RX DATA line */ \
			arm_hardware_pio6_alternative(1U << 15, R7S721_PIOALT_5);	/* P6_15: TXD0: TX DATA line */ \
		} while (0)
#endif /* WITHUART1HW */

#if WITHTX

	// txpath outputs not used
	////#define TXPATH_TARGET_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
	////#define TXPATH_TARGET_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)
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

	// PTT input - P6_12
	#define PTT_TARGET_PIN				(R7S721_INPUT_PORT(6))
	#define PTT_BIT_PTT					(1U << 12)		// P6_12

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_pio6_inputs(PTT_BIT_PTT); \
		} while (0)

#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_TARGET_LEFT_PIN		(R7S721_INPUT_PORT(6))
	#define ELKEY_TARGET_RIGHT_PIN		(R7S721_INPUT_PORT(2))
	#define ELKEY_BIT_LEFT				(1U << 13)	// P6_13
	#define ELKEY_BIT_RIGHT				(1U << 6)	// P2_6 (��������� � �������� ��������������� ������ 37)

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_pio6_inputs(ELKEY_BIT_LEFT); \
			arm_hardware_pio2_inputs(ELKEY_BIT_RIGHT); \
		} while (0)

#endif /* WITHELKEY */

#if WITHUSBHW
	#define USB_INITIALIZE() \
		do { \
		} while (0)

#endif /* WITHUSBHW */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_S(v) do {	R7S721_TARGET_PORT_S(1, v); } while (0)
//#define SPI_IOUPDATE_PORT_C(v) do {	R7S721_TARGET_PORT_C(1, v); } while (0)
//#define SPI_IOUPDATE_BIT		(1U << 15)	// * PA15

/* ���������� ������� �� SPI ���� */
#define targetnvram		(1U << 0)	// P7_0  nvmem FM25L16B
#define targetctl1		(1U << 1)	// P7_1  board control registers chain
#define targetcodec1	(1U << 2)	// P7_2  on-board codec1 NAU8822L
#define targetfpgacs4_u	(1U << 3)	// P7_3  FPGA control registers CS4
#define targetfpgacs3_u	(1U << 4)	// P7_4  FPGA control registers CS3
#define targetfpgacs2_u	(1U << 5)	// P7_5  FPGA control registers CS2
#define targetfpga1		(1U << 6)	// P7_6  FPGA control registers CS1
#define targetdds1_u	(1U << 10)	// P7_10 FPGA NCO registers CS
#define targetext1		(1U << 11)	// P7_11 external spi device (LCD)

#define targetuc1608	targetext1 

// ����� ������ ���� ����������� ��� ���� ������������ CS � ����������.
#define SPI_ALLCS_BITS ( \
	targetnvram |	/* P7_0  nvmem FM25L16B */ \
	targetctl1 |	/* P7_1  board control registers chain */ \
	targetcodec1 |	/* P7_2  on-board codec1 NAU8822L */ \
	targetfpgacs4_u |	/* P7_3  FPGA control registers CS4 */ \
	targetfpgacs3_u |	/* P7_4  FPGA control registers CS3 */ \
	targetfpgacs2_u |	/* P7_5  FPGA control registers CS2 */ \
	targetfpga1 |	/* P7_6  FPGA control registers CS1 */ \
	targetdds1_u |	/* P7_10 FPGA NCO registers CS */ \
	targetext1 |	/* P7_11 external spi device (LCD) */ \
	0)

#define SPI_ALLCS_BITSNEG 0		// ������, �������� ��� "1"

#if 0

	#define SPI_ADDRESS_PORT_S(v) do {	R7S721_TARGET_PORT_S(xx, v); } while (0)
	#define SPI_ADDRESS_PORT_C(v) do {	R7S721_TARGET_PORT_C(xx, v); } while (0)

	// ���� ������ ��� ����������� SPI
	#define SPI_A0 ((1u << xx))			// * PE13 
	#define SPI_A1 ((1u << xx))			// * PE14 
	#define SPI_A2 ((1u << xx))			// * PE15 

	#define SPI_NAEN_PORT_S(v) do {	R7S721_TARGET_PORT_S(xx, v); } while (0)
	#define SPI_NAEN_PORT_C(v) do {	R7S721_TARGET_PORT_C(xx, v); } while (0)

	#define SPI_NAEN_BIT (1u << xx)		// * PE7 used

#endif


// ����� ����������� ��� ������ ��� �������� �����������
#define SPI_ALLCS_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
#define SPI_ALLCS_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)

/* ������������� ���� ������ ������������ ��������� */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_pio7_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
	} while (0)

// reset ������
////#define TARGET_CS4272_RESET_PORT_S(v)		do { R7S721_TARGET_PORT_S(7, v); } while (0)
////#define TARGET_CS4272_RESET_PORT_C(v)		do { R7S721_TARGET_PORT_C(7, v); } while (0)
////#define TARGET_CS4272_RESET_BIT		(1U << 2)	// PD2

// MOSI & SCK port
#define SPI_TARGET_SCLK_PORT_S(v) do {	R7S721_TARGET_PORT_S(6, v); } while (0)
#define SPI_TARGET_SCLK_PORT_C(v) do {	R7S721_TARGET_PORT_C(6, v); } while (0)
#define	SPI_SCLK_BIT			(1U << 0)	// * P6_0 ���, ����� ������� ���� ������������� SPI

#define SPI_TARGET_MOSI_PORT_S(v) do {	R7S721_TARGET_PORT_S(6, v); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v) do {	R7S721_TARGET_PORT_C(6, v); } while (0)
#define	SPI_MOSI_BIT			(1U << 2)	// * P6_2 ���, ����� ������� ���� ����� (��� ���� � ������ ���������������� SPI).

#define SPI_TARGET_MISO_PIN		(R7S721_INPUT_PORT(6))		// was PINA 
#define	SPI_MISO_BIT			(1U << 3)	// * P6_3 ���, ����� ������� ���� ���� � SPI.

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_pio6_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
			arm_hardware_pio6_inputs(SPI_MISO_BIT); \
		} while (0)

#if WITHSPIHW

	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_pio6_alternative(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT, R7S721_PIOALT_3);	/* PIO disable */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_pio6_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT);	/* PIO enable */ \
		} while (0)

	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_pio6_alternative(SPI_MOSI_BIT, R7S721_PIOALT_3);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_pio6_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)

#endif /* WITHSPIHW */

#if 1 // WITHTWISW
	// �������� ���� P1 by design
	// P1_6 - SCL3
	// P1_7 - SDA3
	#define TARGET_TWI_TWCK_PORT_C(v) do {	R7S721_TARGET_PORT_S(1, v); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do {	R7S721_TARGET_PORT_C(1, v); } while (0)
	#define TARGET_TWI_TWCK_PIN		(R7S721_INPUT_PORT(1))
	#define TARGET_TWI_TWCK		(1u << 6)		// P1_6 - SCL3

	#define TARGET_TWI_TWD_PORT_C(v) do {	R7S721_TARGET_PORT_S(1, v); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do {	R7S721_TARGET_PORT_C(1, v); } while (0)
	#define TARGET_TWI_TWD_PIN		(R7S721_INPUT_PORT(1))
	#define TARGET_TWI_TWD		(1u << 7)		// P1_7 - SDA3

	// ������������� ����� ������ �����-������ ��� ����������� ���������� I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_pio1_outputs(TARGET_TWI_TWCK | TARGET_TWI_TWD, TARGET_TWI_TWCK | TARGET_TWI_TWD); \
		} while (0) 
	// ������������� ����� ������ �����-������ ��� ���������� ���������� I2C
	// ������������� ������� � ������������� ����������
	#define	TWIHARD_INITIALIZE() do { \
			arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C1);	/* AF=4 */ \
		} while (0) 


#endif

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { R7S721_TARGET_PORT_S(6, v); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { R7S721_TARGET_PORT_C(6, v); } while (0)

	#define FPGA_NCONFIG_BIT		(1UL << 5)	/* P6_5 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(R7S721_INPUT_PORT(6))
	#define FPGA_NSTATUS_INPUT		(R7S721_INPUT_PORT(6))

	#define FPGA_CONF_DONE_BIT		(1UL << 4)	/* P6_4 bit conneced to CONF_DONE pin ALTERA FPGA */
	#define FPGA_NSTATUS_BIT		(1UL << 6)	/* P6_6 bit conneced to NSTATUS pin ALTERA FPGA */

	/* ������������� ������� GPIO ���������� ��� ��������� ��������� � ����������� ��������� FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pio6_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pio6_inputs(FPGA_CONF_DONE_BIT | FPGA_NSTATUS_BIT); \
		} while (0)

	/* �� ��������� ��������� ���������, ��������������������� �� FPGA (����� � user mode). */
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(400), 1)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// ���� ������� � ������� ������������� FIR ������� � FPGA
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { R7S721_TARGET_PORT_C(5, v); } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { R7S721_TARGET_PORT_S(5, v); } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (1U << 15)	/* P5_15 - fir clock */

	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { R7S721_TARGET_PORT_C(7, v); } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { R7S721_TARGET_PORT_S(7, v); } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (1U << 8)	/* P7_8 - fir1 WE */

	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { R7S721_TARGET_PORT_C(7, v); } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { R7S721_TARGET_PORT_S(7, v); } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (1U << 9)	/* P7_9 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
				arm_hardware_pio7_outputs(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
				arm_hardware_pio7_outputs(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
				arm_hardware_pio5_outputs(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
			} while (0)
#endif /* WITHDSPEXTFIR */

	/* ��������� ��������� ������������ ��� */
	#define TARGET_FPGA_OVF_BIT		(1u << 1)	// P6_1
	#define TARGET_FPGA_OVF_GET		((R7S721_INPUT_PORT(6) & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
	#define TARGET_FPGA_OVF_INITIALIZE() do { \
				arm_hardware_pio6_inputs(TARGET_FPGA_OVF_BIT); \
			} while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define KBD_TARGET_PIN (R7S721_INPUT_PORT(1))

		#define KBD_MASK 0 //(0x01 | 0x02)	// ��� ������������ ����
		#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pio1_inputs(KBD_MASK); \
			} while (0)
	#endif

	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pio1_alternative((ainmask) << 8, R7S721_PIOALT_1);	/* P1_8..P1_15 - AN0..AN7 inputs */ \
		} while (0)

	#define HARDWARE_DAC_INITIALIZE() do { \
		} while (0)

	#define HARDWARE_SIDETONE_INITIALIZE() do { \
		} while (0)

	/* ����������������, ������� ������ �������� � ���� ��� ������������� */
	#define	HARDWARE_INITIALIZE() do { \
		HARDWARE_SIDETONE_INITIALIZE(); \
		HARDWARE_KBD_INITIALIZE(); \
		HARDWARE_DAC_INITIALIZE(); \
		} while (0)

#endif /* ARM_R7S72_TQFP176_CPUSTYLE_RAVEN_V9_H_INCLUDED */
