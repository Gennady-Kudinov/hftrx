/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с DSP обработкой "Аист" на процессоре ZYNQ7020
// rmainunit_sv9b.pcb myir7020-766, TFT 7", USB, NAU88C22, LTC2208

#ifndef ARM_X7C7XX_BGAXXX_CPUSTYLE_STORCH_SV9_H_INCLUDED
#define ARM_X7C7XX_BGAXXX_CPUSTYLE_STORCH_SV9_H_INCLUDED 1

//#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
//#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
//#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */
//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	- у STM32MP1 его нет */
//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

#if WITHINTEGRATEDDSP
	//#define WITHI2S2HW	1	/* Использование I2S - аудиокодек на I2S2 и I2S2_alt или I2S2 и I2S3	*/
	//#define WITHSAI1HW	1	/* Использование SAI1 - FPGA или IF codec	*/
	//#define WITHSAI2HW	1	/* Использование SAI2 - FPGA или IF codec	*/
	//#define WITHSAI3HW	1	/* Использование SAI3 - FPGA скоростной канал записи спктра	*/
#endif /* WITHINTEGRATEDDSP */

//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */
//#define WITHETHHW 1	/* Hardware Ethernet controller */

//#define WITHNANDHW	1		/* Hardware NAND CONTROLLER - PrimeCell Static Memory Controller (PL353) ARM r2p1 */
//#define WITHNANDSW	1		/* Software (bit-bang) NAND flash control */

#define USERFIRSTSBLOCK 0
#define WITHPS7BOARD_ANTMINER 1
//#define WITHPS7BOARD_MYC_Y7Z020 1

#define GPIO_IOTYPE_500	GPIO_IOTYPE_LVCMOS33
#define GPIO_IOTYPE_501	GPIO_IOTYPE_LVCMOS18

#if WITHDEBUG
	#define WITHUART2HW	1	/*	Используется периферийный контроллер последовательного порта UART1 */
	#define WITHUARTFIFO	1	/* испольование FIFO */
#endif /* WITHDEBUG */

//#define WITHCAT_USART1		1
#define WITHDEBUG_USART2	1
#define WITHNMEA_USART2		1	/* порт подключения GPS/GLONASS */


#if WITHISBOOTLOADER

	#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
	//#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
	//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 2-м проводам */
	//#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 4-м проводам */

	//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
	//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

	#define WITHSDRAMHW	1		/* В процессоре есть внешняя память */
	//#define WITHSDRAM_PMC1	1	/* power management chip */

	//#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	//#define WITHEHCIHW	1	/* USB_EHCI controller */
	//#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */

	//#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USBH_HS_DP & USBH_HS_DM
	//#define WITHUSBDEV_DMAENABLE 1

	/* For H7 exist: Legacy defines */
	//#define USB_OTG_HS                   USB1_OTG_HS
	//#define USB_OTG_FS                   USB2_OTG_FS

	//#define WITHUSBHW_HOST		USB_OTG_HS
	#define WITHUSBHOST_HIGHSPEEDULPI	1
	//#define WITHUSBHOST_DMAENABLE 1


	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	//#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
	//#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
	//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */

	//#define WITHUSBCDCACM		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	//#define WITHUSBCDCACM_N	1	/* количество виртуальных последовательных портов */
    //#define WITHUSBCDCACMINTSHARING 1    /* Использование общей notification endpoint на всех CDC ACM устрйоствах */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */
	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
	#define WITHMOVEDFU 1	// Переместить интерфейс DFU в область меньших номеров. Утилита dfu-util 0.9 не работает с DFU на интерфейсе с индексом 10
	#define WITHUSBWCID	1

	//#define WITHLWIP 1
	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
	//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */

#else /* WITHISBOOTLOADER */

	#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
	//#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
	//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 2-м проводам */
	//#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 4-м проводам */

	//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
	//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

	//#define WITHMDMAHW		1	/* Использование MDMA для формирования изображений */
	//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
	//#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

	#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	//#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */

	//#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	//#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
	//#define WITHUSBDEV_DMAENABLE 1

#if 0
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
	#define WITHEHCIHW	1	/* USB_EHCI controller */
	#define WITHUSBHW_EHCI		EHCI0
	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port, 1 - 2nd PHY port.
	#define WITHUSBHOST_HIGHSPEEDULPI	1
	//#define WITHUSBHOST_DMAENABLE 1	// not need for EHCI
#endif

	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	#if 0//WITHINTEGRATEDDSP

		#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
		#if WITHRTS96 || WITHRTS192
			#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
		#endif /* WITHRTS96 || WITHRTS192 */
		//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */
	#endif /* WITHINTEGRATEDDSP */

	//#define WITHUSBCDCACM		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	//#define WITHUSBCDCACM_N	2	/* количество виртуальных последовательных портов */
    //#define WITHUSBCDCACMINTSHARING 1    /* Использование общей notification endpoint на всех CDC ACM устрйоствах */
	#if WITHLWIP
		#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
		//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
		//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */
	#endif /* WITHLWIP */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */

//	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
//	#define WITHMOVEDFU 1	// Переместить интерфейс DFU в область меньших номеров. Утилита dfu-util 0.9 не работает с DFU на интерфейсе с индексом 10
//	#define WITHUSBWCID	1

#endif /* WITHISBOOTLOADER */


#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

	void gt911_interrupt_handler(void);
	#define BOARD_GT911_INT_PIN 	64
	#define BOARD_GT911_RESET_PIN 	65

	#define BOARD_GT911_RESET_SET(v) do { if (v) xc7z_writepin(BOARD_GT911_RESET_PIN, 1); else xc7z_writepin(BOARD_GT911_RESET_PIN, 0);  } while (0)

	#define BOARD_GT911_INT_SET(v) do { if (v) xc7z_writepin(BOARD_GT911_INT_PIN, 1); else xc7z_writepin(BOARD_GT911_INT_PIN, 0); } while (0)

	#define BOARD_GT911_RESET_INITIO_1() do { xc7z_gpio_output(BOARD_GT911_RESET_PIN); xc7z_gpio_output(BOARD_GT911_INT_PIN); } while (0)

	#define BOARD_GT911_RESET_INITIO_2() do { xc7z_gpio_input(BOARD_GT911_INT_PIN); } while (0)

	#define BOARD_GT911_INT_CONNECT() do { \
	} while (0)

#endif

#if WITHCPUTEMPERATURE
	#define GET_CPU_TEMPERATURE() (xc7z_get_cpu_temperature())
#endif /* WITHCPUTEMPERATURE */

//#define PREAMP_MIO	63

#define LS020_RS_INITIALIZE() \
	do { \
		/*arm_hardware_piod_outputs2m(LS020_RS, LS020_RS); *//* PD3 */ \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		/*arm_hardware_piod_outputs2m(LS020_RESET, LS020_RESET); *//* PD4 */ \
	} while (0)

#define LS020_RS_SET(v) do { \
		if ((v) != 0) LS020_RS_PORT_S(LS020_RS); \
		else  LS020_RS_PORT_C(LS020_RS); \
	} while (0)

#define LS020_RESET_SET(v) do { \
		if ((v) != 0) LS020_RESET_PORT_S(LS020_RESET); \
		else  LS020_RESET_PORT_C(LS020_RESET); \
	} while (0)

#if LCDMODE_SPI_NA
	// эти контроллеры требуют только RS

	#define LS020_RS_PORT_S(v)		do { /* GPIOD->BSRR = BSRR_S(v); */__DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { /* GPIOD->BSRR = BSRR_C(v); */__DSB(); } while (0)
	#define LS020_RS			(1u << 3)			// PD3 signal

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v)		do { /* GPIOD->BSRR = BSRR_S(v); */__DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { /* GPIOD->BSRR = BSRR_C(v); */__DSB(); } while (0)
	#define LS020_RESET			(1u << 4)			// PD4 signal

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS
	// LCDMODE_UC1608

	#define LS020_RS_PORT_S(v)		do { /* GPIOD->BSRR = BSRR_S(v);  */__DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { /* GPIOD->BSRR = BSRR_C(v);  */__DSB(); } while (0)
	#define LS020_RS			(1u << 3)			// PD3 signal

	#define LS020_RESET_PORT_S(v)		do { /* GPIOD->BSRR = BSRR_S(v);  */__DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { /* GPIOD->BSRR = BSRR_C(v);  */__DSB(); } while (0)
	#define LS020_RESET			(1u << 2)			// PD4 signal

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	#error Unsupported LCDMODE_HD44780

#endif

#if WITHENCODER

	// Выводы подключения енкодера #1

	// Выводы подключения енкодера #2
	#define ENCODER2_BITA		55
	#define ENCODER2_BITB		54

	#define ENCODER_INITIALIZE() \
		do { \
		} while (0)
#endif

#if WITHI2S2HW
	// Инициализируются I2S2 в дуплексном режиме.
	#define I2S2HW_INITIALIZE() do { \
		SPI2->CFG2 |= SPI_CFG2_IOSWP; \
		arm_hardware_piob_altfn2(1uL << 12,	AF_SPI2); /* PB12 I2S2_WS	*/ \
		arm_hardware_piob_updown(0, 1uL << 12); \
		arm_hardware_piob_altfn2(1uL << 13,	AF_SPI2); /* PB13 I2S2_CK	*/ \
		arm_hardware_piob_updown(0, 1uL << 13); \
		arm_hardware_piob_altfn2(1uL << 15,	AF_SPI2); /* PB15 I2S2_SDO - передача */ \
		arm_hardware_piob_updown(0, 1uL << 15); \
		arm_hardware_piob_altfn2(1uL << 14,	AF_SPI2); /* PB14 I2S2_SDI, - приём от кодека */ \
		arm_hardware_piob_updown(0, 1uL << 14); \
	} while (0)
#endif /* WITHI2S2HW */

	// для предотвращения треска от оставшегося инициализированным кодека
	#define I2S2HW_POOLDOWN() do { \
		arm_hardware_piob_inputs(1uL << 12); /* PB12 I2S2_WS	*/ \
		arm_hardware_piob_updown(0, 1uL << 12); \
		arm_hardware_piob_inputs(1uL << 13); /* PB13 I2S2_CK	*/ \
		arm_hardware_piob_updown(0, 1uL << 13); \
		arm_hardware_piob_inputs(1uL << 15); /* PB15 I2S2_SDO - передача */ \
		arm_hardware_piob_updown(0, 1uL << 15); \
		arm_hardware_piob_inputs(1uL << 14); /* PB14 I2S2_SDI, - приём от кодека */ \
		arm_hardware_piob_updown(0, 1uL << 14); \
	} while (0)

#if WITHSAI1HW
	/*
	 *
	 */
	#define SAI1HW_INITIALIZE()	do { \
		/*arm_hardware_pioe_altfn20(1uL << 2, AF_SAI); */	/* PE2 - SAI1_MCK_A - 12.288 MHz	*/ \
		arm_hardware_pioe_altfn2(1uL << 4,	AF_SAI);			/* PE4 - SAI1_FS_A	- 48 kHz	*/ \
		arm_hardware_pioe_altfn20(1uL << 5,	AF_SAI);			/* PE5 - SAI1_SCK_A	*/ \
		arm_hardware_pioe_altfn2(1uL << 6,	AF_SAI);			/* PE6 - SAI1_SD_A	(i2s data to codec)	*/ \
		arm_hardware_pioe_altfn2(1uL << 3,	AF_SAI);			/* PE3 - SAI1_SD_B	(i2s data from codec)	*/ \
		arm_hardware_pioe_updown(1uL << 3, 0); \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI2HW
	/* 
	Поскольку блок SAI2 инициализируется как SLAVE с синхронизацией от SAI1,
	из внешних сигналов требуется только SAI2_SD_A
	*/
	#define SAI2HW_INITIALIZE()	do { \
		arm_hardware_pioe_altfn2(1uL << 11, AF_SAI2);	/* PE11 - SAI2_SD_B	(i2s data from FPGA)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI3HW
	/*
	*/
	#define SAI3HW_INITIALIZE()	do { \
		arm_hardware_piod_altfn50(1uL << 12, AF_SAI2); 		/* PD12 - SAI2_FS_A	- WS from FPGA	*/ \
		arm_hardware_piod_altfn50(1uL << 13, AF_SAI2); 		/* PD13 - SAI2_SCK_A	*/ \
		arm_hardware_pioe_altfn50(1uL << 11, AF_SAI2);		/* PE11 - SAI2_SD_B	(i2s data from FPGA)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_USART2)
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) 
	//#define FROMCAT_BIT_RTS				(1u << 11)	/* PA11 сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR)
	//#define FROMCAT_BIT_DTR				(1u << 12)	/* PA12 сигнал DTR от FT232RL	*/

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_USART2) */

#if (WITHCAT && WITHCAT_CDC)

	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) // was PINA 
	//#define FROMCAT_BIT_RTS				(1u << 11)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR) // was PINA 
	//#define FROMCAT_BIT_DTR				(1u << 12)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от виртуального CDC порта */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от виртуального CDC порта*/
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if WITHSDHCHW

	// J11
	// SD0 signals
	//	PS_MIO40_CD_CLK
	//	PS_MIO41_CD_CMD
	//	PS_MIO42_CD_D0
	//	PS_MIO43_CD_D1
	//	PS_MIO44_CD_D2
	//	PS_MIO45_CD_D3
	//	PS_MIO46_CD_SW
	//	PS_MIO50_CD_WP

	// return: 0 - no disk
	#define HARDWARE_SDIOSENSE_CD() 1//((SD0->PRESENT_STATE & (1uL << 18)) != 0) /* получить состояние датчика CARD PRESENT */
	// return: ! 0 - write protect
	#define HARDWARE_SDIOSENSE_WP() 0//((SD0->PRESENT_STATE & (1uL << 19)) == 0) /* получить состояние датчика CARD WRITE PROTECT */

	#define HARDWARE_SDIO_HANGOFF() do { \
		} while (0)
	// SD0 signals
	#define HARDWARE_SDIO_D0_MIO	10	//	eMMC_D0		E9	PS_MIO10_500
	#define HARDWARE_SDIO_CMD_MIO	11	//	eMMC_CMD	C6	PS_MIO11_500
	#define HARDWARE_SDIO_CLK_MIO	12	//	eMMC_CLK	D9	PS_MIO12_500
	#define HARDWARE_SDIO_D1_MIO	13	//	eMMC_D1		E8	PS_MIO13_500
	#define HARDWARE_SDIO_D2_MIO	14	//	eMMC_D2		C5	PS_MIO14_500
	#define HARDWARE_SDIO_D3_MIO	15	//	eMMC_D3		C8	PS_MIO15_500

	//EMIT_MASKWRITE(0XF8000830, 0x003F003FU ,0x00380037U),	// SD0_WP_CD_SEL
	#define HARDWARE_SDIO_INITIALIZE() do { \
		const portholder_t miopin_ctl = 0x00000680uL; \
		const portholder_t miopin_dat = 0x00001680uL; \
		SCLR->SD0_WP_CD_SEL = \
				0 * (46uL << 16) |	/* 46 SDIO 0 CD Select */ \
				0 * (50uL << 0) |	/* 50 SDIO 0 WP Select */ \
				0; \
		MIO_SET_MODE(HARDWARE_SDIO_D0_MIO, miopin_dat);		/*  eMMC_D0	 */ \
		MIO_SET_MODE(HARDWARE_SDIO_D1_MIO, miopin_dat);		/*  eMMC_D1	 */ \
		MIO_SET_MODE(HARDWARE_SDIO_D2_MIO, miopin_dat);		/*  eMMC_D2	 */ \
		MIO_SET_MODE(HARDWARE_SDIO_D3_MIO, miopin_dat);		/*  eMMC_D3	 */ \
		MIO_SET_MODE(HARDWARE_SDIO_CMD_MIO, miopin_ctl);	/*  eMMC_CMD */ \
		MIO_SET_MODE(HARDWARE_SDIO_CLK_MIO, miopin_ctl);	/*  eMMC_CLK */ \
	} while (0)
	#define HARDWARE_SDIOSENSE_INITIALIZE() do { \
	} while (0)
	#define HARDWARE_SDIOPOWER_INITIALIZE() do { \
	} while (0)

#endif /* WITHSDHCHW */

#if WITHSDHCHW && 0
	#if WITHSDHCHW4BIT
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1uL << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1uL << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1uL << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_altfn50(1uL << 9, AF_SDIO);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_altfn50(1uL << 10, AF_SDIO);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_altfn50(1uL << 11, AF_SDIO);	/* PC11 - SDIO_D3	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1uL << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_inputs(1uL << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_inputs(1uL << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_inputs(1uL << 11);	/* PC11 - SDIO_D3	*/ \
			arm_hardware_piod_updown(0, 1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1uL << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_updown(0, 1uL << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_updown(0, 1uL << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_updown(0, 1uL << 11);	/* PC11 - SDIO_D3	*/ \
		} while (0)
	#else /* WITHSDHCHW4BIT */
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1uL << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1uL << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1uL << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1uL << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_piod_updown(0, 1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1uL << 8);	/* PC8 - SDIO_D0	*/ \
		} while (0)
	#endif /* WITHSDHCHW4BIT */

	#define HARDWARE_SDIO_WP_BIT	(1U << 8)	/* PG8 - SDIO_WP */
	#define HARDWARE_SDIO_CD_BIT	(1U << 7)	/* PG7 - SDIO_SENSE */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piog_inputs(HARDWARE_SDIO_WP_BIT); /* PD1 - SDIO_WP */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_WP_BIT, 0); \
			arm_hardware_piog_inputs(HARDWARE_SDIO_CD_BIT); /* PD0 - SDIO_SENSE */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)

	#define HARDWARE_SDIOSENSE_CD() ((GPIOG->IDR & HARDWARE_SDIO_CD_BIT) == 0)	/* получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() ((GPIOG->IDR & HARDWARE_SDIO_WP_BIT) != 0)	/* получить состояние датчика CARD WRITE PROTECT */

	#define HARDWARE_SDIOPOWER_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define HARDWARE_SDIOPOWER_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define HARDWARE_SDIOPOWER_BIT (1u << 7)	/* PC7 */
	/* если питание SD CARD управляется прямо с процессора */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		arm_hardware_pioc_outputs2m(HARDWARE_SDIOPOWER_BIT, HARDWARE_SDIOPOWER_BIT); /* питание выключено */ \
		} while (0)
	/* parameter on not zero for powering SD CARD */
	#define HARDWARE_SDIOPOWER_SET(on)	do { \
		if ((on) != 0) \
			HARDWARE_SDIOPOWER_C(HARDWARE_SDIOPOWER_BIT); \
		else \
			HARDWARE_SDIOPOWER_S(HARDWARE_SDIOPOWER_BIT); \
	} while (0)

#endif /* WITHSDHCHW */

#if WITHTX

	// txpath outputs not used
	////#define TXPATH_TARGET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	////#define TXPATH_TARGET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	// 
	#define TXGFV_RX		(1u << 4)
	#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
	#define TXGFV_TX_SSB	(1u << 0)
	#define TXGFV_TX_CW		(1u << 1)
	#define TXGFV_TX_AM		(1u << 2)
	#define TXGFV_TX_NFM	(1u << 3)

	#define TXPATH_INITIALIZE() \
		do { \
		} while (0)


	// +++
	// TXDISABLE input - PD10
	#define TXDISABLE_TARGET_PIN				1//(GPIOD->IDR)
	#define TXDISABLE_BIT_TXDISABLE				0//(1U << 10)		// PD10 - TX INHIBIT
	// получить бит запрета передачи (от усилителя мощности)
	#define HARDWARE_GET_TXDISABLE() (0) //((TXDISABLE_TARGET_PIN & TXDISABLE_BIT_TXDISABLE) != 0)
	#define TXDISABLE_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(TXDISABLE_BIT_TXDISABLE); \
			arm_hardware_piod_updown(0, TXDISABLE_BIT_TXDISABLE); \
		} while (0)
	// ---

//	ELKEY_LEFT	B32	G17	IO_L16P_T2_35
//	ELKEY_RIGHT	B33	G18	IO_L16N_T2_35
//	PTT	B22	K19	IO_L10P_T1_AD11P_35
//	PTT2	B23	J19	IO_L10N_T1_AD11N_35
//	TUNE_REQ	B24	G19	IO_L18P_T2_AD13P_35
//	TX_INH	B25	G20	IO_L18N_T2_AD13N_35
//	ENCODER_A	B28	F20	IO_L15N_T2_DQS_AD12N_35
//	ENCODER_B	B27	F19	IO_L15P_T2_DQS_AD12P_35
//	ENC2_A	B30	H17	IO_L13N_T2_MRCC_35
//	ENC2_B	B29	H16	IO_L13P_T2_MRCC_35
//	ENC2_BUTTON	A5	M15	IO_B35_LN23
//	TS_INT	A2	V15	IO_L10P_T1_34
//	USER_BOOT	A13	U15	IO_B34_LN11	Input, pull-up need
//	ACTIVITY_LED	D7	F16	IO_L6P_T0_35	LED anode
//
//	NMEA_RESET	A10	T15	IO_B34_LN5
//	PPS_IN	A12	U15	IO_B34_LP11

	// +++
	// PTT input - PD10
	// PTT2 input - PD9
	#define PTT_TARGET_PIN				1//(GPIOD->IDR)
	#define PTT_BIT_PTT					1//(1uL << 10)		// PD10 - PTT
	#define PTT2_TARGET_PIN				1//(GPIOD->IDR)
	#define PTT2_BIT_PTT				1//(1uL << 9)		// PD9 - PTT2
	// получить бит запроса оператором перехода на пердачу
	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0 || (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(PTT_BIT_PTT); \
			arm_hardware_piod_updown(PTT_BIT_PTT, 0); \
			arm_hardware_piod_inputs(PTT2_BIT_PTT); \
			arm_hardware_piod_updown(PTT2_BIT_PTT, 0); \
		} while (0)
	// ---

	#define HARDWARE_GET_TUNE() (0)
	#define TUNE_INITIALIZE() \
		do { } while (0)

#else /* WITHTX */

	#define TXDISABLE_INITIALIZE() \
		do { \
		} while (0)
	#define PTT_INITIALIZE() \
		do { \
		} while (0)
	#define TUNE_INITIALIZE() \
		do { \
		} while (0)
#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_BIT_LEFT				(1uL << 14)		// PD14
	#define ELKEY_BIT_RIGHT				(1uL << 15)		// PD15

	#define ELKEY_TARGET_PIN			(GPIOD->IDR)

	#define HARDWARE_GET_ELKEY_LEFT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_LEFT) == 0)
	#define HARDWARE_GET_ELKEY_RIGHT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_RIGHT) == 0)


	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piod_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
//#define SPI_IOUPDATE_BIT		(1uL << 15)	// * PA15

// All trough EMIO
#define TARGET_CTL1_CS_EMIO 	62	//	CTL1_CS		D10	E19	IO_L5N_T0_AD9N_3
#define TARGET_RTC_CS_EMIO 		62	//	RTC_CS		A20	R17	IO_B34_LN19
#define TARGET_CODEC1_CS_EMIO 	62	//	CODEC_CS	B14	J18	IO_L14P_T2_AD4P_SRCC_35
#define TARGET_ADC1_CS_EMIO 	62	//	ADC1_CS		A40	W19	IO_B34_LN22
#define TARGET_ADC2_CS_EMIO 	62	//	ADC2_CS		A39	W18	IO_B34_LP22
#define TARGET_NVRAM_CS_EMIO 	62	//	NVRAM_CS	B35	G15	IO_L19N_T3_VREF_35
#define TARGET_EXT1_CS_EMIO 	62	//	EXT1_CS		D9	E18	IO_L5P_T0_AD9P_35
#define TARGET_EXT2_CS_EMIO 	62	//	EXT2_CS		D8	F17	IO_L6N_T0_VREF_35

#define targetctl1		TARGET_CTL1_CS_EMIO	// control registers
#define targetfpga1		TARGET_CTL1_CS_EMIO	// FPGA control registers (STUB)
#define targetnvram		TARGET_NVRAM_CS_EMIO	// nvram FM25L256
#define targetcodec1	TARGET_CODEC1_CS_EMIO	// nvram NAU88C22

#if WITHSPIHW || WITHSPISW

	#define WITHSPICSEMIO	1	/* специфицеская конфигурация - управление сигналами CS SPI периферии выполняется через EMIO */

	#define TARGET_NVRAM_EMIO	62	// nvram FM25L256


	#define SPI_CS_SET(target)	do { \
		xc7z_writepin((target), 0); \
	} while (0)

	#define SPI_ALLCS_DISABLE() do { \
		xc7z_writepin(TARGET_CTL1_CS_EMIO, 1);		\
		xc7z_writepin(TARGET_RTC_CS_EMIO, 1);		\
		xc7z_writepin(TARGET_CODEC1_CS_EMIO, 1);		\
		xc7z_writepin(TARGET_ADC1_CS_EMIO, 1);		\
		xc7z_writepin(TARGET_ADC2_CS_EMIO, 1);		\
		xc7z_writepin(TARGET_NVRAM_CS_EMIO, 1);		\
		xc7z_writepin(TARGET_EXT1_CS_EMIO, 1);		\
		xc7z_writepin(TARGET_EXT2_CS_EMIO, 1);		\
	} while(0)

	/* инициализация линий выбора периферийных микросхем */
	#define SPI_ALLCS_INITIALIZE() do { \
	} while (0)

	#define	SPI_IOTYPE 	GPIO_IOTYPE_501

	//	SPI_MOSI	C38	B15	PS_MIO45_501
	//	SPI_MISO	C36	C17	PS_MIO41_501
	//	SPI_SCLK	C39	D14	PS_MIO40_501
	// MOSI & SCK port
	#define	SPI_SCLK_MIO 	40
	#define	SPI_MOSI_MIO 	45
	#define	SPI_MISO_MIO 	41

	#define SPI_SCLK_C()	do { gpio_writepin(SPI_SCLK_MIO, 0); __DSB(); } while (0)
	#define SPI_SCLK_S()	do { gpio_writepin(SPI_SCLK_MIO, 1); __DSB(); } while (0)

	#define SPI_MOSI_C()	do { gpio_writepin(SPI_MOSI_MIO, 0); __DSB(); } while (0)
	#define SPI_MOSI_S()	do { gpio_writepin(SPI_MOSI_MIO, 1); __DSB(); } while (0)

	#define SPI_TARGET_MISO_PIN		(gpio_readpin(SPI_MISO_MIO))

	#define SPIIO_INITIALIZE() do { \
		enum { IOTYPE = SPI_IOTYPE }; \
		const portholder_t pinmode_input = MIO_PIN_VALUE(1, 0, IOTYPE, 1, 0, 0, 0, 0, 1); \
		const portholder_t pinmode_output = MIO_PIN_VALUE(1, 0, IOTYPE, 1, 0, 0, 0, 0, 0); \
		gpio_output2(SPI_SCLK_MIO, 1, pinmode_output); \
		gpio_output2(SPI_MOSI_MIO, 1, pinmode_output); \
		gpio_input2(SPI_MISO_MIO, pinmode_input); \
	} while (0)

	#define HARDWARE_SPI_CONNECT() do { \
	} while (0)

	#define HARDWARE_SPI_DISCONNECT() do { \
		enum { IOTYPE = SPI_IOTYPE }; \
		const portholder_t pinmode_input = MIO_PIN_VALUE(1, 0, IOTYPE, 1, 0, 0, 0, 0, 1); \
		const portholder_t pinmode_output = MIO_PIN_VALUE(1, 0, IOTYPE, 1, 0, 0, 0, 0, 0); \
		gpio_output2(SPI_SCLK_MIO, 1, pinmode_output); \
		gpio_output2(SPI_MOSI_MIO, 1, pinmode_output); \
		gpio_input2(SPI_MISO_MIO, pinmode_input); \
	} while (0)

	#define HARDWARE_SPI_CONNECT_MOSI() do { \
	} while (0)

	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
		enum { IOTYPE = SPI_IOTYPE }; \
		gpio_input2(SPI_MOSI_MIO, MIO_PIN_VALUE(1, 0, IOTYPE, 1, 0, 0, 0, 0, 1)); \
	} while (0)

#endif /* WITHSPIHW || WITHSPISW */

#if WITHUART2HW

	// WITHUART2HW
	#define TARGET_UART1_TX_MIO	48	//	USART_TX	C44	B12	PS_MIO48_501	UART1
	#define TARGET_UART1_RX_MIO	49	//	USART_RX	C45	C12	PS_MIO49_501

	//MIO_PIN_VALUE(disablercvr, pullup, io_type, speed, l3_sel, l2_sel, l1_sel, l0_sel, tri_enable)
	#define HARDWARE_UART2_INITIALIZE() do { \
		enum { IOTYPE = GPIO_IOTYPE_501 }; /* LVCMOS18 */ \
		const portholder_t pinmode_uart_tx = MIO_PIN_VALUE(1, 1, IOTYPE, 0, 0x07, 0, 0, 0, 0); \
		const portholder_t pinmode_uart_rx = MIO_PIN_VALUE(1, 1, IOTYPE, 0, 0x07, 0, 0, 0, 1); \
		gpio_peripherial(TARGET_UART1_TX_MIO, pinmode_uart_tx);  /*  MIO_PIN_48 UART1_TXD */ \
		gpio_peripherial(TARGET_UART1_RX_MIO, pinmode_uart_rx);  /*  MIO_PIN_49 UART1_RXD */ \
	} while (0)

#endif /* WITHUART2HW */

#if WITHKEYBOARD

	//	ENC2_BUTTON	A5	M15	IO_B35_LN23
	#define TARGET_ENC2BTN_BIT_MIO 		56

#if WITHENCODER2
	#define TARGET_ENC2BTN_GET (xc7z_readpin(TARGET_ENC2BTN_BIT_MIO) == 0)
#endif /* WITHENCODER2 */

#if WITHPWBUTTON
	// P5_3 - ~CPU_POWER_SW signal
#define TARGET_POWERBTN_BIT 0//(1U << 8)	// PAxx - ~CPU_POWER_SW signal
	#define TARGET_POWERBTN_GET	0//(((GPIOx->IDR) & TARGET_POWERBTN_BIT) == 0)
#endif /* WITHPWBUTTON */

	#define HARDWARE_KBD_INITIALIZE() do { \
		xc7z_gpio_input(TARGET_ENC2BTN_BIT_MIO); \
	} while (0)

#else /* WITHKEYBOARD */

	#define HARDWARE_KBD_INITIALIZE() do { \
	} while (0)

#endif /* WITHKEYBOARD */

#if WITHTWISW

	#define TARGET_TWI_IOTYPE 		GPIO_IOTYPE_501
	#define TARGET_TWI_TWCK_MIO		42	//	I2C_SCL	C42	E12	PS_MIO42_501	Open Drain
	#define TARGET_TWI_TWD_MIO		43	//	I2C_SDA	C43	A9	PS_MIO43_501	Open Drain

	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	// todo: check tri_enable field
	// MIO_PIN_VALUE(disablercvr, pullup, io_type, speed, l3_sel, l2_sel, l1_sel, l0_sel, tri_enable)
	#define	TWIHARD_INITIALIZE() do { \
		enum { IOTYPE = TARGET_TWI_IOTYPE }; \
		const portholder_t pinmode = MIO_PIN_VALUE(1, 1, IOTYPE, 0, 0x02, 0, 0, 0, 1); \
		gpio_peripherial(TARGET_TWI_TWD_MIO, pinmode);	/*  PS_MIO43_501 SDA */ \
		gpio_peripherial(TARGET_TWI_TWCK_MIO, pinmode);	/*  PS_MIO42_501 SCL */ \
		i2chw_initialize(); \
	} while (0)

	#define TWISOFT_INITIALIZE() do { \
		enum { IOTYPE = TARGET_TWI_IOTYPE }; \
		const portholder_t pinmode =  MIO_PIN_VALUE(1, 1, IOTYPE, 0, 0, 0, 0, 0, 0); \
		gpio_opendrain2(TARGET_TWI_TWD_MIO, 0, pinmode);		/*  PS_MIO43_501 SDA */ \
		gpio_opendrain2(TARGET_TWI_TWCK_MIO, 0, pinmode);		/*  PS_MIO42_501 SCL */ \
	} while(0)

	#define SET_TWCK() do { gpio_drive(TARGET_TWI_TWCK_MIO, 0); hardware_spi_io_delay(); } while (0)
	#define CLR_TWCK() do { gpio_drive(TARGET_TWI_TWCK_MIO, 1); hardware_spi_io_delay(); } while (0)

	#define SET_TWD() do { gpio_drive(TARGET_TWI_TWD_MIO, 0); hardware_spi_io_delay(); } while (0)
	#define CLR_TWD() do { gpio_drive(TARGET_TWI_TWD_MIO, 1); hardware_spi_io_delay(); } while (0)

	#define GET_TWCK() (gpio_readpin(TARGET_TWI_TWCK_MIO))
	#define GET_TWD() (gpio_readpin(TARGET_TWI_TWD_MIO))

#endif // WITHTWISW

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define FPGA_NCONFIG_BIT		(1UL << 11)	/* PC11 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(GPIOC->IDR)
	#define FPGA_CONF_DONE_BIT		(1UL << 10)	/* PC10 bit conneced to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		(GPIOC->IDR)
	#define FPGA_NSTATUS_BIT		(1UL << 9)	/* PC9 bit conneced to NSTATUS pin ALTERA FPGA */

	#define FPGA_INIT_DONE_INPUT	(GPIOC->IDR)
	#define FPGA_INIT_DONE_BIT		(1UL << 12)	/* PC12 bit conneced to INIT_DONE pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
		arm_hardware_pioc_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
		arm_hardware_pioc_inputs(FPGA_NSTATUS_BIT); \
		arm_hardware_pioc_inputs(FPGA_CONF_DONE_BIT); \
		arm_hardware_pioc_inputs(FPGA_INIT_DONE_BIT); \
	} while (0)

	/* Проверяем, проинициализировалась ли FPGA (вошла в user mode). */
	/*
		After the option bit to enable INIT_DONE is programmed into the device (during the first
		frame of configuration data), the INIT_DONE pin goes low.
		When initialization is complete, the INIT_DONE pin is released and pulled high. 
		This low-to-high transition signals that the device has entered user mode.
	*/
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(100), (FPGA_INIT_DONE_INPUT & FPGA_INIT_DONE_BIT) != 0)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Биты доступа к массиву коэффициентов FIR фильтра в FPGA

	// FPGA PIN_23
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (1uL << 13)	/* PC13 - fir CS ~FPGA_FIR_CLK */

	// FPGA PIN_8
	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (1uL << 1)	/* PD1 - fir1 WE */

	// FPGA PIN_7
	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (1uL << 0)	/* PD0 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
		arm_hardware_piod_outputs2m(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
		arm_hardware_piod_outputs2m(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
		arm_hardware_pioc_outputs2m(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
	} while (0)
#endif /* WITHDSPEXTFIR */

#if 0
	/* получение состояния переполнения АЦП */
	#define TARGET_FPGA_OVF_INPUT		(GPIOC->IDR)
	#define TARGET_FPGA_OVF_BIT			(1u << 8)	// PC8
	#define TARGET_FPGA_OVF_GET			((TARGET_FPGA_OVF_INPUT & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
	#define TARGET_FPGA_OVF_INITIALIZE() do { \
		arm_hardware_pioc_inputs(TARGET_FPGA_OVF_BIT); \
	} while (0)
#endif

#if WITHCPUDACHW
	/* включить нужные каналы */
	#define HARDWARE_DAC_INITIALIZE() do { \
		DAC1->CR = DAC_CR_EN1; /* DAC1 enable */ \
	} while (0)
	#define HARDWARE_DAC_ALC(v) do { /* вывод 12-битного значения на ЦАП - канал 1 */ \
		DAC1->DHR12R1 = (v); /* DAC1 set value */ \
	} while (0)

#else /* WITHCPUDACHW */
	#define HARDWARE_DAC_INITIALIZE() do { \
	} while (0)

#endif /* WITHCPUDACHW */

#if WITHCPUADCHW
	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
		arm_hardware_pioa_analoginput(((ainmask) >> 0) & 0xff);	/* ADC12_IN0..ADC12_IN7 */ \
		arm_hardware_piob_analoginput(((ainmask) >> 8) & 0x03);	/* ADC12_IN8..ADC12_IN0 */ \
		arm_hardware_pioc_analoginput(((ainmask) >> 10) & 0x3f);	/* ADC12_IN10..ADC12_IN15 */ \
	} while (0)
#endif /* WITHCPUADCHW */

	// direct connection:
	#define USB_RESET_MIO	46	//	USB_RESET	C37	D16		PS_MIO46_501

	// ULPI:
	#define USB_DATA0_MIO	32	//	USB_DATA0	C27	A14		PS_MIO32_501
	#define USB_DATA1_MIO	33	//	USB_DATA1	C28	D15		PS_MIO33_501
	#define USB_DATA2_MIO	34	//	USB_DATA2	C30	A12		PS_MIO34_501
	#define USB_DATA3_MIO	35	//	USB_DATA3	C34	F12		PS_MIO35_501
	#define USB_DATA4_MIO	28	//	USB_DATA4	C25	C16		PS_MIO28_501
	#define USB_DATA5_MIO	37	//	USB_DATA5	C35	A10		PS_MIO37_501
	#define USB_DATA6_MIO	38	//	USB_DATA6	C31	E13		PS_MIO38_501
	#define USB_DATA7_MIO	39	//	USB_DATA7	C23	C18		PS_MIO39_501
	#define USB_DIR_MIO		29	//	USB_DIR		C29	C13		PS_MIO29_501
	#define USB_STP_MIO		30	//	USB_STP		C26	C15		PS_MIO30_501
	#define USB_NXT_MIO		31	//	USB_NXT		C24	E16		PS_MIO31_501
	#define USB_CLK_MIO		36	//	USB_CLK		C32	A11		PS_MIO36_501

#if WITHUSBHW

	#define	USBD_FS_INITIALIZE() do { \
		/*arm_hardware_piod_outputs(TARGET_USBFS_VBUSON_BIT, TARGET_USBFS_VBUSON_BIT); */ /* PD2 */ \
	} while (0)

	#define TARGET_USBFS_VBUSON_SET(on)	do { \
		ulpi_chip_vbuson(!! on); \
	} while (0)

	/**USB_OTG_HS GPIO Configuration
	PB13     ------> USB_OTG_HS_VBUS
	PB14     ------> USB_OTG_HS_DM
	PB15     ------> USB_OTG_HS_DP
	*/
	#define	USBD_HS_FS_INITIALIZE() do { \
		/*arm_hardware_pioa_altfn50((1uL << 11) | (1uL << 12), AF_OTGFS);	*/		/* PA10, PA11, PA12 - USB_OTG_FS	*/ \
		/* arm_hardware_pioa_inputs(1uL << 9);	*/	/* PA9 - USB_OTG_FS_VBUS */ \
	} while (0)

	#define	USBD_HS_ULPI_INITIALIZE() do { \
	} while (0)

	// MIO_PIN_VALUE(disablercvr, pullup, io_type, speed, l3_sel, l2_sel, l1_sel, l0_sel, tri_enable)
	#define USB_ULPI_INITIALIZE() do { \
		enum { IOTYPE = GPIO_IOTYPE_501 }; /* LVCMOS18 */ \
		enum { L3_SEL = 0x00, L2_SEL = 0x00, L1_SEL = 0x01, L0_SEL = 0x00 }; \
		const portholder_t pinmode_ulpi_data = MIO_PIN_VALUE(1, 0, IOTYPE, 0, L3_SEL, L2_SEL, L1_SEL, L0_SEL, 0); \
		const portholder_t pinmode_ulpi_input = MIO_PIN_VALUE(1, 0, IOTYPE, 0, L3_SEL, L2_SEL, L1_SEL, L0_SEL, 1); \
		const portholder_t pinmode_ulpi_output = MIO_PIN_VALUE(1, 0, IOTYPE, 0, L3_SEL, L2_SEL, L1_SEL, L0_SEL, 0); \
		/* ULPI chip */ \
		gpio_peripherial(USB_DATA0_MIO, pinmode_ulpi_data); \
		gpio_peripherial(USB_DATA1_MIO, pinmode_ulpi_data); \
		gpio_peripherial(USB_DATA2_MIO, pinmode_ulpi_data); \
		gpio_peripherial(USB_DATA3_MIO, pinmode_ulpi_data); \
		gpio_peripherial(USB_DATA4_MIO, pinmode_ulpi_data); \
		gpio_peripherial(USB_DATA5_MIO, pinmode_ulpi_data); \
		gpio_peripherial(USB_DATA6_MIO, pinmode_ulpi_data); \
		gpio_peripherial(USB_DATA7_MIO, pinmode_ulpi_data); \
		gpio_peripherial(USB_DIR_MIO, pinmode_ulpi_input); \
		gpio_peripherial(USB_NXT_MIO, pinmode_ulpi_input); \
		gpio_peripherial(USB_CLK_MIO, pinmode_ulpi_input); \
		gpio_peripherial(USB_STP_MIO, pinmode_ulpi_output); \
		/* RESET */ \
		gpio_output2(USB_RESET_MIO, 1, MIO_PIN_VALUE(1, 0, IOTYPE, 0, 0, 0, 0, 0, 0)); /* USB_RESET	C37	D16		PS_MIO46_501 */ \
		local_delay_ms(10); \
		gpio_writepin(USB_RESET_MIO, 0); /* USB_RESET = 0 */ \
		local_delay_ms(10); \
		gpio_writepin(USB_RESET_MIO, 1); /* USB_RESET = 1 */ \
		local_delay_ms(10); \
	} while (0)

#else /* WITHUSBHW */


	// MIO_PIN_VALUE(disablercvr, pullup, io_type, speed, l3_sel, l2_sel, l1_sel, l0_sel, tri_enable)
	#define USB_ULPI_INITIALIZE() do { \
		enum { IOTYPE = GPIO_IOTYPE_501 }; /* LVCMOS18 */ \
		/* RESET */ \
		gpio_output2(USB_RESET_MIO, 0, MIO_PIN_VALUE(1, 0, IOTYPE, 0, 0, 0, 0, 0, 0)); /* USB_RESET	C37	D16		PS_MIO46_501 */ \
		local_delay_ms(10); \
	} while (0)


#endif /* WITHUSBHW */

#if WITHDCDCFREQCTL
	// ST ST1S10 Synchronizable switching frequency from 400 kHz up to 1.2 MHz
	#define WITHHWDCDCFREQMIN 400000L
	#define WITHHWDCDCFREQMAX 1200000L

	// PB9 - DC-DC synchro output
	// TIM17_CH1 AF1
	// TIM4_CH4	AF2	AF_TIM4
	#define	HARDWARE_DCDC_INITIALIZE() do { \
		arm_hardware_piob_altfn2((1U << 9), AF_TIM17); /* PB9 - TIM17_CH1 */ \
		hardware_dcdcfreq_tim17_ch1_initialize(); \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		hardware_dcdcfreq_tim17_ch1_setdiv(f); \
	} while (0)
#else /* WITHDCDCFREQCTL */
	#define	HARDWARE_DCDC_INITIALIZE() do { \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		(void) (f); \
	} while (0)
#endif /* WITHDCDCFREQCTL */

	#if LCDMODE_LQ043T3DX02K
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#elif LCDMODE_AT070TN90 || LCDMODE_AT070TNA2
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#else
		/* Заглушка для работы без дисплея */
		#define WITHLCDBACKLIGHTMIN	0
		#define WITHLCDBACKLIGHTMAX	2	// Верхний предел регулировки (показываемый на дисплее)
	#endif

#if 0
	#define TARGET_BL_ENABLE_MIO	28

	#define	HARDWARE_BL_INITIALIZE() do { \
		xc7z_gpio_output(TARGET_BL_ENABLE_MIO); \
		} while (0)

	/* установка яркости и включение/выключение преобразователя подсветки */

	#define HARDWARE_BL_SET(en, level) do { \
		xc7z_writepin(TARGET_BL_ENABLE_MIO, en); \
	} while (0)
#endif

#if WITHLTDCHW && 0
	enum
	{
		GPIO_AF_LTDC14 = 14,  /* LCD-TFT Alternate Function mapping */
		GPIO_AF_LTDC9 = 9,  /* LCD-TFT Alternate Function mapping */
		//GPIO_AF_LTDC3 = 3  /* LCD-TFT Alternate Function mapping */
	};
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_INITIALIZE(demode) do { \
		const uint32_t MODEmask = (1U << 3); /* PD3 - MODEmask */ \
		const uint32_t DEmask = (1U << 13); /* PE13 - DE */ \
		const uint32_t HSmask = (1U << 6); /* PC6 - HSYNC */ \
		const uint32_t VSmask = (1U << 4); 	/* PA4 - VSYNC */ \
		/* Bit clock */ \
		arm_hardware_piog_altfn50((1U << 7), GPIO_AF_LTDC14);		/* CLK PG7 */ \
		/* Control */ \
		arm_hardware_piod_outputs(MODEmask, (demode != 0) * MODEmask);	/* PD3 MODEmask=state */ \
		/* Synchronisation signals in SYNC mode */ \
		arm_hardware_pioe_outputs((demode == 0) * DEmask, 0);	/* DE=0 (DISP, pin 31) */ \
		arm_hardware_pioa_altfn50((demode == 0) * VSmask, GPIO_AF_LTDC14);	/* VSYNC */ \
		arm_hardware_pioc_altfn50((demode == 0) * HSmask, GPIO_AF_LTDC14);	/* HSYNC */ \
		/* Synchronisation signals in DE mode*/ \
		arm_hardware_pioe_altfn50((demode != 0) * DEmask, GPIO_AF_LTDC14);	/* DE */ \
		arm_hardware_pioa_outputs((demode != 0) * VSmask, VSmask);	/* VSYNC */ \
		arm_hardware_pioc_outputs((demode != 0) * HSmask, HSmask);	/* HSYNC */ \
		/* RED */ \
		arm_hardware_piob_altfn50((1U << 0), GPIO_AF_LTDC14);		/* PB0 R3 */ \
		arm_hardware_pioa_altfn50((1U << 11), GPIO_AF_LTDC14);		/* PA11 R4 */ \
		arm_hardware_pioa_altfn50((1U << 9), GPIO_AF_LTDC14);		/* PA9 R5 */ \
		arm_hardware_pioa_altfn50((1U << 8), GPIO_AF_LTDC14);		/* PA8 R6 */ \
		arm_hardware_piog_altfn50((1U << 6), GPIO_AF_LTDC14);		/* PG6 R7 */ \
		/* GREEN */ \
		arm_hardware_pioa_altfn50((1U << 6), GPIO_AF_LTDC14);		/* PA6 G2 */ \
		arm_hardware_piog_altfn50((1U << 10), GPIO_AF_LTDC9);		/* PG10 G3 */ \
		arm_hardware_piob_altfn50((1U << 10), GPIO_AF_LTDC14);		/* PB10 G4 */ \
		arm_hardware_piof_altfn50((1U << 11), GPIO_AF_LTDC14);		/* PF11 G5 */ \
		arm_hardware_pioc_altfn50((1U << 7), GPIO_AF_LTDC14);		/* PC7 G6 */ \
		arm_hardware_piog_altfn50((1U << 8), GPIO_AF_LTDC14);		/* PG8 G7 */ \
		/* BLUE */ \
		arm_hardware_piog_altfn50((1U << 11), GPIO_AF_LTDC14);		/* PG11 B3 */ \
		arm_hardware_piog_altfn50((1U << 12), GPIO_AF_LTDC9);		/* PG12 B4 */ \
		arm_hardware_pioa_altfn50((1U << 3), GPIO_AF_LTDC14);		/* PA3 B5 */ \
		arm_hardware_piob_altfn50((1U << 8), GPIO_AF_LTDC14);		/* PB8 B6 */ \
		arm_hardware_piod_altfn50((1U << 8), GPIO_AF_LTDC14);		/* PD8 B7 */ \
	} while (0)

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(state) do { \
		const uint32_t VSmask = (1U << 4); 	/* PA4 - VSYNC */ \
		const uint32_t DEmask = (1U << 13); /* PE13 */ \
		/* while ((GPIOA->IDR & VSmask) != 0) ; */ /* схема синхронизации стоит на плате дисплея. дождаться 0 */ \
		/* while ((GPIOA->IDR & VSmask) == 0) ; */ /* дождаться 1 */ \
		arm_hardware_pioe_outputs(DEmask, ((state) != 0) * DEmask); /* DE=DISP, pin 31 - можно менять только при VSYNC=1 */ \
	} while (0)
#endif /* WITHLTDCHW */

	#if WITHSDRAMHW
		// Bootloader parameters
		#define BOOTLOADER_RAMAREA SDRAM_BASE	/* адрес ОЗУ, куда перемещать application */
		#define BOOTLOADER_RAMSIZE SDRAM_APERTURE_SIZE	// 255M
		#define BOOTLOADER_RAMPAGESIZE	(16 * 1024uL * 1024)	// при загрузке на исполнение используется размер страницы в 1 мегабайт
		#define USBD_DFU_RAM_XFER_SIZE 4096
	#endif /* WITHSDRAMHW */

	#define BOOTLOADER_FLASHSIZE (16 * 1024uL * 1024uL)	// 16M FLASH CHIP
	#define BOOTLOADER_SELFBASE QSPI_LINEAR_BASE	/* адрес где лежит во FLASH образ application */
	#define BOOTLOADER_SELFSIZE (1024uL * 512)	// 512k

	#define BOOTLOADER_APPBASE (BOOTLOADER_SELFBASE + BOOTLOADER_SELFSIZE)	/* адрес где лежит во FLASH образ application */
	#define BOOTLOADER_APPSIZE (chipsizeDATAFLASH() - BOOTLOADER_SELFSIZE)	// 2048 - 128

	//#define BOOTLOADER_PAGESIZE (1024uL * 64)	// W25Q32FV with 64 KB pages

	#define USBD_DFU_FLASH_XFER_SIZE 256	// match to (Q)SPI FLASH MEMORY page size
	#define USBD_DFU_FLASHNAME "W25Q128JV"
	//	6.3.4 Quad-SPI Boot
	//	...
	//	2. In cases of Quad-SPI boot, if the image is authenticated, then the boot image should be placed
	//	at a 32K offset other than 0x0 (the image should not be placed starting at 0x0 offset in
	//	Quad-SPI).
	#define BOOTLOADER_FSBL_OFFSET	0x000000

	/* Выводы соединения с QSPI BOOT NOR FLASH */

	#if WIHSPIDFSW || WIHSPIDFHW

		// Table 6-9: Quad-SPI Boot MIO Register Settings
		// QSPI_CS0 	MIO1
		// QSPI_IO[0:3] MIO2..MIO5
		// QSPI_SCLK0 	MIO6
		#define SPDIF_NCS_MIO 	1	// MIO1	QSPI_CS0
		#define SPDIF_MOSI_MIO 	2	// MIO2	QSPI_IO0
		#define SPDIF_MISO_MIO 	3	// MIO3	QSPI_IO1
		#define SPDIF_D2_MIO 	4	// MIO4	QSPI_IO2
		#define SPDIF_D3_MIO 	5	// MIO5	QSPI_IO3
		#define SPDIF_SCLK_MIO 	6	// MIO6	QSPI_SCLK0

		/* Отсоединить процессор от BOOT ROM - для возможности работы внешнего программатора. */
		#define SPIDF_HANGOFF() do { \
			const portholder_t pinmode_input = MIO_PIN_VALUE(1, 0, GPIO_IOTYPE_500, 1, 0, 0, 0, 0, 1); \
			gpio_input2(SPDIF_NCS_MIO, pinmode_input);	/*  */ \
			gpio_input2(SPDIF_SCLK_MIO, pinmode_input);	/*  */ \
			gpio_input2(SPDIF_MOSI_MIO, pinmode_input);	/*  */ \
			gpio_input2(SPDIF_MISO_MIO, pinmode_input);	/*  */ \
			gpio_input2(SPDIF_D2_MIO, pinmode_input);	/*  */ \
			gpio_input2(SPDIF_D3_MIO, pinmode_input);	/*  */ \
		} while (0)

		#if WIHSPIDFHW
			#define SPIDF_HARDINITIALIZE() do { \
				arm_hardware_piof_altfn50(SPDIF_D2_BIT, AF_QUADSPI_AF9);  	/* PF7 D2 QUADSPI_BK1_IO2 */ \
				arm_hardware_piof_altfn50(SPDIF_D3_BIT, AF_QUADSPI_AF9);  	/* PF6 D3 QUADSPI_BK1_IO3 */ \
				/*arm_hardware_piof_outputs(SPDIF_D2_BIT, SPDIF_D2_BIT); */ /* PF7 D2 tie-up */ \
				/*arm_hardware_piof_outputs(SPDIF_D3_BIT, SPDIF_D3_BIT); */ /* PF6 D3 tie-up */ \
				arm_hardware_piof_altfn50(SPDIF_SCLK_BIT, AF_QUADSPI_AF9); /* PF10 SCLK */ \
				arm_hardware_piof_altfn50(SPDIF_MOSI_BIT, AF_QUADSPI_AF10); /* PF8 MOSI */ \
				arm_hardware_piof_altfn50(SPDIF_MISO_BIT, AF_QUADSPI_AF10); /* PF9 MISO */ \
				arm_hardware_piob_altfn50(SPDIF_NCS_BIT, AF_QUADSPI_AF10); /* PB6 CS */ \
			} while (0)

		#else /* WIHSPIDFHW */

			#define SPIDF_MISO() (gpio_readpin(SPDIF_MISO_MIO))
			#define SPIDF_MOSI(v) do { if (v) gpio_writepin(SPDIF_MOSI_MIO, 1); else gpio_writepin(SPDIF_MOSI_MIO, 0); } while (0)
			#define SPIDF_SCLK(v) do { if (v) gpio_writepin(SPDIF_SCLK_MIO, 1); else gpio_writepin(SPDIF_SCLK_MIO, 0); } while (0)
			#define SPIDF_SOFTINITIALIZE() do { \
				const portholder_t pinmode_input = MIO_PIN_VALUE(1, 0, GPIO_IOTYPE_500, 1, 0, 0, 0, 0, 1); \
				const portholder_t pinmode_output = MIO_PIN_VALUE(1, 0, GPIO_IOTYPE_500, 1, 0, 0, 0, 0, 0); \
				gpio_output2(SPDIF_NCS_MIO, 1, pinmode_output);  \
				gpio_output2(SPDIF_SCLK_MIO, 1, pinmode_output);  \
				gpio_output2(SPDIF_MOSI_MIO, 1, pinmode_output);	\
				gpio_input2(SPDIF_MISO_MIO, pinmode_input);	\
				gpio_output2(SPDIF_D2_MIO, 1, pinmode_output);  \
				gpio_output2(SPDIF_D3_MIO, 1, pinmode_output);  \
			} while (0)
			#define SPIDF_SELECT() do { \
				gpio_writepin(SPDIF_NCS_MIO, 0);  \
				__DSB(); \
			} while (0)
			#define SPIDF_UNSELECT() do { \
				gpio_writepin(SPDIF_NCS_MIO, 1);  \
				__DSB(); \
			} while (0)

		#endif /* WIHSPIDFHW */

	#endif /* WIHSPIDFSW || WIHSPIDFHW */

#if (WITHNANDHW || WITHNANDSW)
	// NAND flash data bus
	#define HARDWARE_NAND_D7_MIO 12		// D7: PS_MIO12
	#define HARDWARE_NAND_D6_MIO 11		// D6: PS_MIO11
	#define HARDWARE_NAND_D5_MIO 10		// D5: PS_MIO10
	#define HARDWARE_NAND_D4_MIO 9		// D4: PS_MIO9
	#define HARDWARE_NAND_D3_MIO 13		// D3: PS_MIO13
	#define HARDWARE_NAND_D2_MIO 4		// D2: PS_MIO4
	#define HARDWARE_NAND_D1_MIO 6		// D1: PS_MIO6
	#define HARDWARE_NAND_D0_MIO 5		// D0: PS_MIO5

	// NAND flash Control bits:
	#define HARDWARE_NAND_RBC_MIO 14	// R/B#: PS_MIO14 Ready/Busy#
	#define HARDWARE_NAND_ALE_MIO 2		// ALE: PS_MIO2
	#define HARDWARE_NAND_CLE_MIO 7		// CLE: PS_MIO7
	#define HARDWARE_NAND_WEB_MIO 3		// WE#: PS_MIO3
	//#define HARDWARE_NAND_WPB_MIO 1		// WP#: PS_MIO1 ???
	#define HARDWARE_NAND_REB_MIO 8		// RE#: PS_MIO8
	#define HARDWARE_NAND_CSB_MIO 0		// CS#: PS_MIO0

	#define HARDWARE_NAND_INITIALIZE() do { \
		const portholder_t pinmode_input = MIO_PIN_VALUE(1, 0, GPIO_IOTYPE_500, 1, 0, 0, 0, 0, 0); \
		gpio_input2(HARDWARE_NAND_RBC_MIO, pinmode_input); /* Ready/Busy# */ \
		gpio_input2(HARDWARE_NAND_D7_MIO, pinmode_input); \
		gpio_input2(HARDWARE_NAND_D6_MIO, pinmode_input); \
		gpio_input2(HARDWARE_NAND_D5_MIO, pinmode_input); \
		gpio_input2(HARDWARE_NAND_D4_MIO, pinmode_input); \
		gpio_input2(HARDWARE_NAND_D3_MIO, pinmode_input); \
		gpio_input2(HARDWARE_NAND_D2_MIO, pinmode_input); \
		gpio_input2(HARDWARE_NAND_D1_MIO, pinmode_input); \
		gpio_input2(HARDWARE_NAND_D0_MIO, pinmode_input); \
		xc7z_gpio_output(HARDWARE_NAND_CSB_MIO); \
		xc7z_writepin(HARDWARE_NAND_CSB_MIO, 1); \
		xc7z_gpio_output(HARDWARE_NAND_ALE_MIO); \
		xc7z_writepin(HARDWARE_NAND_ALE_MIO, 0); \
		xc7z_gpio_output(HARDWARE_NAND_CLE_MIO); \
		xc7z_writepin(HARDWARE_NAND_CLE_MIO, 0); \
		xc7z_gpio_output(HARDWARE_NAND_WEB_MIO); \
		xc7z_writepin(HARDWARE_NAND_WEB_MIO, 1); \
		/*xc7z_gpio_output(HARDWARE_NAND_WPB_MIO); *//* Write protect */ \
		/*xc7z_writepin(HARDWARE_NAND_WPB_MIO, 0); */\
		xc7z_gpio_output(HARDWARE_NAND_REB_MIO); \
		xc7z_writepin(HARDWARE_NAND_REB_MIO, 1); \
	} while (0)

#endif /* (WITHNANDHW || WITHNANDSW) */

#if 1

	#define ZYNQBOARD_LED_RED 8 /* Running indicator - PS_MIO8_500  */

	#define BOARD_BLINK_INITIALIZE() do { \
			const portholder_t pinmode_output = MIO_PIN_VALUE(1, 0, GPIO_IOTYPE_500, 1, 0, 0, 0, 0, 0); \
			gpio_output2(ZYNQBOARD_LED_RED, 1, pinmode_output); \
		} while (0)
	#define BOARD_BLINK_SETSTATE(state) do { \
		if (state) \
		{ \
			gpio_writepin(ZYNQBOARD_LED_RED, 1); \
		} else { \
			gpio_writepin(ZYNQBOARD_LED_RED, 0); \
		} \
	} while (0)

#endif

	/* запрос на вход в режим загрузчика */
	#define BOARD_USERBOOT_BIT	0//(1uL << 1)	/* PB1: ~USER_BOOT */
	#define BOARD_IS_USERBOOT() 0//(((GPIOB->IDR) & BOARD_USERBOOT_BIT) == 0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
		arm_hardware_piob_inputs(BOARD_USERBOOT_BIT); /* set as input with pull-up */ \
	} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
			/*I2S2HW_POOLDOWN(); */\
			BOARD_BLINK_INITIALIZE(); \
			/*HARDWARE_KBD_INITIALIZE(); */\
			/*HARDWARE_DAC_INITIALIZE(); */\
			/*HARDWARE_BL_INITIALIZE(); */ \
			/*HARDWARE_DCDC_INITIALIZE(); */\
			/*TXDISABLE_INITIALIZE(); */\
			/*TUNE_INITIALIZE(); */\
			/*BOARD_USERBOOT_INITIALIZE(); */ \
			/*USBD_FS_INITIALIZE(); */\
			USB_ULPI_INITIALIZE(); \
		} while (0)

#endif /* ARM_X7C7XX_BGAXXX_CPUSTYLE_STORCH_SV9_H_INCLUDED */
