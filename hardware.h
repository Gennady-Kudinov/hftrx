/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#ifndef HARDWARE_H_INCLUDED
#define HARDWARE_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>
//#include <assert.h>

#if \
	defined (__AVR_ATmega32__) || defined (__AVR_ATmega32A__) || \
	defined (__AVR_ATmega16__) || defined (__AVR_ATmega16A__) || \
	0

	#define CPUSTYLE_ATMEGA	1		/* ����������� ���������� */
	#define CPUSTYLE_ATMEGA32	1	/* ����� ������ � ������������ ������� - DIP40 ��� TQFP48 */

	#ifndef F_CPU
	#define F_CPU 8000000ul
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* ������ ��� ������ ������������� ���������� ������ ���������� */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(32)))  */
	#define ALIGNX_END /* nothing */

#elif \
	defined (__AVR_ATmega8__)  || \
	defined (__AVR_ATmega8A__)  || \
	0

	#define CPUSTYLE_ATMEGA	1		/* ����������� ���������� */

	#ifndef F_CPU
	#define F_CPU 8000000ul
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* ������ ��� ������ ������������� ���������� ������ ���������� */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(32)))  */
	#define ALIGNX_END /* nothing */

#elif \
	defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega328A__) || \
	defined (__AVR_ATmega168P__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega168PA__) || defined (__AVR_ATmega168A__) || \
	defined (__AVR_ATmega48P__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega48PA__) || defined (__AVR_ATmega48A__) || \
	0

	#define CPUSTYLE_ATMEGA	1		/* ����������� ���������� */
	#define CPUSTYLE_ATMEGA328	1
	#define CPUSTYLE_ATMEGA_XXX4 1

	#ifndef F_CPU
	#define F_CPU 8000000ul
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* ������ ��� ������ ������������� ���������� ������ ���������� */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(32)))  */
	#define ALIGNX_END /* nothing */

#elif \
	defined (__AVR_ATmega324P__) || defined (__AVR_ATmega324A__) || defined (__AVR_ATmega644__) || \
	defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__) || \
	defined (__AVR_ATmega1284P__) || \
	0

	#define CPUSTYLE_ATMEGA	1		/* ����������� ���������� */
	#define CPUSTYLE_ATMEGA32	1	/* ����� ������ � ������������ ������� - DIP40 ��� TQFP48 */
	#define CPUSTYLE_ATMEGA_XXX4 1	// Atmega324, Atmega644 and so on

	#ifndef F_CPU
		#define F_CPU 10000000ul
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* ������ ��� ������ ������������� ���������� ������ ���������� */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(32)))  */
	#define ALIGNX_END /* nothing */

#elif \
	defined (__AVR_ATmega64__) || defined (__AVR_ATmega64A__) || \
	defined (__AVR_ATmega128__) || defined (__AVR_ATmega128A__) || \
	0

	#define CPUSTYLE_ATMEGA	1		/* ����������� ���������� */
	#define CPUSTYLE_ATMEGA128	1	/* ����� ������ � ������������ ������� - TQFP64 */

	#ifndef F_CPU
	#define F_CPU 8000000ul
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* ������ ��� ������ ������������� ���������� ������ ���������� */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(32)))  */
	#define ALIGNX_END /* nothing */

#elif \
	defined (__AVR_ATxmega32A4__) || \
	defined (__AVR_ATxmega32A4U__) || \
	defined (__AVR_ATxmega128A4U__) || \
	0

	#define CPUSTYLE_ATXMEGA	1
	#define CPUSTYLE_ATXMEGAXXXA4	1
	
	#ifndef F_CPU
	#define F_CPU 32000000uL
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* ������ ��� ������ ������������� ���������� ������ ���������� */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(32)))  */
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_STM32H7XX
	// STM32H743ZIT6 processors, up to 400 MHz

	#define CPUSTYLE_ARM	1		/* ����������� ���������� ARM */
	#define CPUSTYLE_STM32F		1		/* ����������� ���������� STM32F */
	#define CPUSTYLE_ARM_CM7	1		/* ����������� ���������� CORTEX M7 */

	#include "armcpu/stm32h7xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define ARM_MATH_CM7 1
	#include "../../DSP/Include/arm_math.h"
	#include "../../DSP/Include/arm_const_structs.h"

#elif CPUSTYLE_STM32F7XX
	// STM32F745ZGT6 processors, up to 216 MHz 

	#define CPUSTYLE_ARM	1		/* ����������� ���������� ARM */
	#define CPUSTYLE_STM32F		1		/* ����������� ���������� STM32F */
	#define CPUSTYLE_ARM_CM7	1		/* ����������� ���������� CORTEX M7 */

	#include "armcpu/stm32f7xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define ARM_MATH_CM7 1
	#include "../../DSP/Include/arm_math.h"
	#include "../../DSP/Include/arm_const_structs.h"

#elif CPUSTYLE_STM32F4XX

	#define CPUSTYLE_ARM	1		/* ����������� ���������� ARM */
	#define CPUSTYLE_STM32F		1		/* ����������� ���������� STM32F */
	#define CPUSTYLE_ARM_CM4	1		/* ����������� ���������� CORTEX M4 */

	#include "armcpu/stm32f4xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define ARM_MATH_CM4 1
	#include "../../DSP/Include/arm_math.h"
	#include "../../DSP/Include/arm_const_structs.h"

#elif CPUSTYLE_STM32F30X

	#define CPUSTYLE_ARM	1		/* ����������� ���������� ARM */
	#define CPUSTYLE_STM32F		1		/* ����������� ���������� STM32F */
	#define CPUSTYLE_ARM_CM4	1		/* ����������� ���������� CORTEX M3 */

	// STM32F303VC processors
	#include "armcpu/stm32f30x.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define ARM_MATH_CM4 1
	#include "../../DSP/Include/arm_math.h"
	#include "../../DSP/Include/arm_const_structs.h"

#elif CPUSTYLE_STM32F1XX

	#define CPUSTYLE_ARM	1		/* ����������� ���������� ARM */
	#define CPUSTYLE_STM32F		1		/* ����������� ���������� STM32F */
	#define CPUSTYLE_ARM_CM3	1		/* ����������� ���������� CORTEX M3 */

	#include "armcpu/stm32f1xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define ARM_MATH_CM3 1
	#include "../../DSP/Include/arm_math.h"
	#include "../../DSP/Include/arm_const_structs.h"

#elif CPUSTYLE_STM32F0XX

	#define CPUSTYLE_ARM	1		/* ����������� ���������� ARM */
	#define CPUSTYLE_STM32F		1		/* ����������� ���������� STM32F */
	#define CPUSTYLE_ARM_CM0	1		/* ����������� ���������� CORTEX M0 */
	
	#include "armcpu/stm32f0xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define ARM_MATH_CM0 1
	//#define ARM_MATH_CM0PLUS 1
	#include "../../DSP/Include/arm_math.h"
	#include "../../DSP/Include/arm_const_structs.h"

#elif CPUSTYLE_STM32L0XX

	#define CPUSTYLE_ARM	1		/* ����������� ���������� ARM */
	#define CPUSTYLE_STM32F		1		/* ����������� ���������� STM32F */
	#define CPUSTYLE_ARM_CM0	1		/* ����������� ���������� CORTEX M0 */
	#include "armcpu/stm32l0xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define ARM_MATH_CM0 1
	//#define ARM_MATH_CM0PLUS 1
	#include "../../DSP/Include/arm_math.h"
	#include "../../DSP/Include/arm_const_structs.h"

#elif CPUSTYLE_ATSAM3S

	#define CPUSTYLE_ARM	1		/* ����������� ���������� ARM */
	#define CPUSTYLE_ARM_CM3	1		/* ����������� ���������� CORTEX M3 */

	#define __SAM3S4C__ 1		// replace to __SAM3S4B__
	#include "sam3s.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define ARM_MATH_CM3 1
	#include "../../DSP/Include/arm_math.h"
	#include "../../DSP/Include/arm_const_structs.h"

#elif CPUSTYLE_ATSAM4S

	#define CPUSTYLE_ARM	1		/* ����������� ���������� ARM */
	#define CPUSTYLE_ARM_CM4	1		/* ����������� ���������� CORTEX M3 */

	#define __SAM4SA16C__ 1
	#include "sam4s.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define ARM_MATH_CM4 1
	#include "../../DSP/Include/arm_math.h"
	#include "../../DSP/Include/arm_const_structs.h"

#elif CPUSTYLE_MK20DX	// Teensy 3.1 - Freescale Semiconductor - KINETIS MK20DX256VLH7

	#define CPUSTYLE_ARM	1		/* ����������� ���������� ARM */
	#define CPUSTYLE_ARM_CM4	1		/* ����������� ���������� CORTEX M3 */

	#include "armcpu/MK20D5.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define ARM_MATH_CM4 1
	#include "../../DSP/Include/arm_math.h"
	#include "../../DSP/Include/arm_const_structs.h"

#elif CPUSTYLE_AT91SAM7S

	#define CPUSTYLE_ARM		1		/* ����������� ���������� ARM */
	#define	CPUSTYLE_ARM7TDMI	1
	#include "armcpu/at91sam7s64.h"

	#define __ASM            __asm           /*!< asm keyword for GNU Compiler          */
	#define __INLINE         inline          /*!< inline keyword for GNU Compiler       */
	#define __STATIC_INLINE  static inline

	#include "tdmi7_gcc.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_AT91SAM9XE

	#define CPUSTYLE_ARM		1		/* ����������� ���������� ARM */
	#define	CPUSTYLE_ARM7TDMI	1
	#include "armcpu/at91sam9xe512.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	#define __ASM            __asm           /*!< asm keyword for GNU Compiler          */
	#define __INLINE         inline          /*!< inline keyword for GNU Compiler       */
	#define __STATIC_INLINE  static inline

	#include "tdmi7_gcc.h"

#elif CPUSTYLE_R7S721

	// Renesas CPU
	#define CPUSTYLE_ARM		1		/* ����������� ���������� ARM */
	#define	CPUSTYLE_ARM_CA9	1

	#include "armcpu/Renesas_RZ_A1.h"

	#include "armcpu/iodefine.h"
	#include "rza_io_regrw.h"

	//#include "hardware_r7s721.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32))) 
	#define ALIGNX_END /* nothing */

	//#include "../../DSP/Include/arm_math.h"
	//#include "../../DSP/Include/arm_const_structs.h"

#elif \
	defined (__TMS320C28X__) || \
	0

	#define CPUSTYPE_TMS320F2833X 1
	#include "tms320f2833x.h"

	//#define CPU_FREQ 150000000uL
	#define CPU_FREQ 100000000uL
	//#define CPU_FREQ 5000000uL		// ������� ��� ����������w�� PLL - xtal / 4

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(32)))  */
	#define ALIGNX_END /* nothing */

#else 

	#error Not handled processor definition (GCC argument -m=XXXXX)
	#error In Makefile not defined CPUSTYLE_XXX

#endif

/* �� �������� ����� �������� �������� �������� ����, ��������� � ��. */
#define MASK2LSB(mask)  ((mask) & (0 - (mask)))

/* ��������������� ��� ������� ���������� "�����" ��� �������� ������� ��������. */
#define NTICKS(t_mS) ((uint_fast16_t) (((uint_fast32_t) (t_mS) * TICKS_FREQUENCY + 500) / 1000))
#define NTICKSADC(t_uS) ((uint_fast16_t) (((uint_fast32_t) (t_uS) * ADC_FREQ + 5) / 1000000))
#define NTICKSADC01(t_uS01) ((uint_fast16_t) (((uint_fast32_t) (t_uS01) * ADC_FREQ + 5) / 10000000))


void arm_cpu_initialize(void);	// ������� ���������� �� start-up �� ����������� ���� "�������" ������� � �� ������������� ����������
void cpu_initialize(void);
void cpu_initdone(void);	// ������ init ������ �� �����
uint_fast32_t cpu_getdebugticks(void);	// ��������� �� ����������� �������� ��������� ���������������� ����

uint_fast32_t arm_hardware_stm32f7xx_pllq_initialize(void);	// ��������� ����� PLLQ �� 48 ���

void hardware_timer_initialize(uint_fast32_t ticksfreq);

void spool_encinterrupt(void);	/* ���������� �� ��������� ������� �� ������ �� ��������� */
void spool_encinterrupt2(void);	/* ���������� �� ��������� ������� �� ������ �� ��������� #2 */
void hardware_encoder_initialize(void);
uint_fast8_t hardware_get_encoder_bits(void);/* ��������� ���� A - � ���� � ����� 2, ���� B - � ���� � ����� 1 */
uint_fast8_t hardware_get_encoder2_bits(void);/* ��������� ���� A - � ���� � ����� 2, ���� B - � ���� � ����� 1 */

void hardware_adc_initialize(void);

#if CPUSTYLE_ARM

	/* ��� ��� �������� ��������� "����� ���������� �� SPI ����" */
	/* ��� ����� ���� ������� ����� � ����� �����-������ */
	typedef uint_fast32_t spitarget_t;		
	/* ��� ��� �������� ������, ��������� �� ����� �����-������ ��� ��� ������������ �������� */
	typedef uint_fast32_t portholder_t;		

	#define FLASHMEM //__flash
	#define FLASHMEMINIT	__attribute__((section(".init"))) /* �� ��������� ������� ������ - �������� ����� ����������� � FPGA */
	#define RAMNOINIT_D1	__attribute__((section(".noinit"))) /* ���������� � ������ SRAM_D1 */
	#define NOINLINEAT // __attribute__((noinline))

	#if CPUSTYLE_R7S721
		#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))  
		#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))  
	#elif (CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX)
		#define RAMFUNC_NONILINE __attribute__((__section__(".ramfunc")))  // �������� ���������� noinline �������� 2-3 �������� �������������� __attribute__((__section__(".ramfunc"), noinline))  
		#define RAMFUNC			 __attribute__((__section__(".ramfunc")))  
	#elif CPUSTYLE_STM32F4XX
		#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))  
		#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))  
	#else
		#define RAMFUNC_NONILINE __attribute__((__section__(".ramfunc")))  // __attribute__((__section__(".ramfunc"), noinline))  
		#define RAMFUNC			 __attribute__((__section__(".ramfunc")))  
	#endif

	#define ATTRWEAK __attribute__ ((weak))
	// Use __attribute__ ((weak, alias("Default_Handler")))

	//#define PSTR(s) (s)
	#define PSTR(s) (__extension__({static const char __c[] FLASHMEM = (s); &__c[0];}))

	void RAMFUNC local_delay_us(int timeUS);
	void local_delay_ms(int timeMS);

#elif CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA
	// ATMega, ATXMega target

	/* ��� ��� �������� ��������� "����� ���������� �� SPI ����" */
	/* ��� ����� ���� ������� ����� � ����� �����-������ */
	typedef uint_fast8_t spitarget_t;
	/* ��� ��� �������� ������, ��������� �� ����� �����-������ ��� ��� ������������ �������� */
	typedef uint_fast8_t portholder_t;		

	#define local_delay_us(t) do { _delay_us(t); } while (0)
	#define local_delay_ms(t) do { _delay_ms(t); } while (0)
 
	#define FLASHMEM __flash
	#define FLASHMEMINIT	__flash	/* �� ��������� ������� ������ - �������� ����� ����������� � FPGA */

	#if (FLASHEND > 0x7FFF)	
		// ��� ����� ��������� ������ FLASH
		#define NOINLINEAT // __attribute__((noinline))
	#else
		#define NOINLINEAT __attribute__((noinline))	// On small FLASH ATMega CPUs
	#endif
	#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))  
	#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))  
	#define RAMNOINIT_D1
	#define ATTRWEAK __attribute__ ((weak))

#elif CPUSTYPE_TMS320F2833X
	/* ��� ��� �������� ��������� "����� ���������� �� SPI ����" */
	/* ��� ����� ���� ������� ����� � ����� �����-������ */
	typedef uint_fast32_t spitarget_t;
	/* ��� ��� �������� ������, ��������� �� ����� �����-������ ��� ��� ������������ �������� */
	typedef uint_fast32_t portholder_t;		

	#define enableIRQ() do { asm(" NOP"); } while (0)
	#define disableIRQ() do { asm(" NOP"); } while (0)

	#define global_enableIRQ() do { asm(" NOP"); } while (0)
	#define global_disableIRQ() do { asm(" NOP"); } while (0)

	#define FLASHMEM //__flash
	#define FLASHMEMINIT	/* �� ��������� ������� ������ - �������� ����� ����������� � FPGA */

	// ��� ����� ��������� ������ FLASH
	#define NOINLINEAT // __attribute__((noinline))
	#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))  
	#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))  
	#define RAMNOINIT_D1
	#define ATTRWEAK __attribute__ ((weak))

#else
	#error Undefined CPUSTYLE_xxxx
#endif

// Sample usage:
///* ��� ��������� � ������ ������������ ������� */
//#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)

// ���������� � �������� TICKS_FREQUENCY (��������, 200 ��) � ������������ ������������.
void enc_spool(void);
void enc2_spool(void);
void kbd_spool(void);
void display_spool(void);	// ������ ������� �� ���������� ���������� ������� ��� �������� ���������
void board_usb_spool(void);


void seq_spool_ticks(void);	/* ��������� ����-�������� - ��������� ���������*/
//void key_spool_inputs(void);	// ����� ��������� ������������ ����� � ����������� ����� �������
void spool_secound(void);		// ���������� ��� � ������� �� ���������� ����������.

void spool_nmeapps(void);	// ���������� ���������� ��� ������� ���������� �������� PPS

// �������� ��������� � �������� ��� ���������������� ������� 
uint_fast8_t
hardware_calc_sound_params(
	uint_least16_t tonefreq,	/* tonefreq - ������� � ������� ����� �����. ������� - 400 ���� (���������� ������� ������ CAT). */
	unsigned * pvalue);
// ���������� ��������� � �������� ��� ���������������� ������� 
// called from interrupt or with disabled interrupts
void hardware_sounds_setfreq(
	uint_fast8_t prei,
	unsigned value
	);
// ��������� ����
void hardware_sounds_disable(void);
/* ���������� ��� ����������� �����������. */
void hardware_beep_initialize(void);


/* ���������� SPI. ��� ��� ��������� ������������ ���������� �� ����� �������� � 8-�������� �������
   �� ����, � ����� ������� ������������ �������� ���������� - ���������� SPI ��� ���� �����������.
   
   ��� ��� ������������ � ������ ����� SPI ������������ ����� ����������� CS, ��� ������ ���������� �� �������
   ������ SPI � "0" ������� SCLK � ���������� ���������.
   */

enum
{
	SPIC_MODE0,
	SPIC_MODE1,
	SPIC_MODE2, 
	SPIC_MODE3, 
	//
	SPIC_MODES_COUNT
} spi_modes_t;

// +++ dsp
// ��������� � �� ������
void hardware_audiocodec_enable(void);		// ��������� � �� ������
void hardware_audiocodec_initialize(void);	// ��������� � �� ������

// ��������� � �� ������ ��� FPGA
void hardware_fpgacodec_enable(void);		// ��������� � �� ������
void hardware_fpgacodec_initialize(void);	// ��������� � �� ������
void hardware_fpgaspectrum_enable(void);	// ��������� � ��������� ������ � �������
void hardware_fpgaspectrum_initialize(void);	// ��������� � ��������� ������ � �������

void hardware_dac_initialize(void);		/* ������������� DAC �� STM32F4xx */
void hardware_dac_ch1_setvalue(uint_fast16_t v);	// ����� 12-������� �������� �� ��� - ����� 1
void hardware_dac_ch2_setvalue(uint_fast16_t v);	// ����� 12-������� �������� �� ��� - ����� 2


void hardware_spi_io_delay(void);

void cat2_parsechar(uint_fast8_t c);				/* ���������� �� ����������� ���������� */
void cat2_rxoverflow(void);							/* ���������� �� ����������� ���������� */
void cat2_disconnect(void);							/* ���������� �� ����������� ���������� ��������� ������ ����� ��� ������ �� USB CDC */
void cat2_sendchar(void * ctx);							/* ���������� �� ����������� ���������� */

// ������� ������������ ������ �������� �� �����������
void cat3_parsechar(uint_fast8_t c);				/* ���������� �� ����������� ���������� */
void cat3_rxoverflow(void);							/* ���������� �� ����������� ���������� */
void cat3_disconnect(void);							/* ���������� �� ����������� ���������� */
void cat3_sendchar(void * ctx);							/* ���������� �� ����������� ���������� */

void modem_parsechar(uint_fast8_t c);				/* ���������� �� ����������� ���������� */
void modem_rxoverflow(void);						/* ���������� �� ����������� ���������� */
void modem_disconnect(void);						/* ���������� �� ����������� ���������� */
void modem_sendchar(void * ctx);							/* ���������� �� ����������� ���������� */

void nmea_parsechar(uint_fast8_t c);				/* ���������� �� ����������� ���������� */
void nmea_rxoverflow(void);							/* ���������� �� ����������� ���������� */
void nmea_sendchar(void * ctx);							/* ���������� �� ����������� ���������� */

void hardware_uart1_initialize(void);
void hardware_uart1_set_speed(uint_fast32_t baudrate);
void hardware_uart1_tx(void * ctx, uint_fast8_t c);	/* �������� ������� ����� ���������� � ���������� ����������� */
void hardware_uart1_enabletx(uint_fast8_t state);	/* ���������� �� ����������� ���������� */
void hardware_uart1_enablerx(uint_fast8_t state);	/* ���������� �� ����������� ���������� */
uint_fast8_t hardware_usart1_putchar(uint_fast8_t c);/* �������� ������� ���� ����� ���� */
uint_fast8_t hardware_usart1_getchar(char * cp); /* ���� �������, ���� ����� ���� */

void hardware_uart2_initialize(void);
void hardware_uart2_set_speed(uint_fast32_t baudrate);
void hardware_uart2_tx(void * ctx, uint_fast8_t c);	/* �������� ������� ����� ���������� � ���������� ����������� */
void hardware_uart2_enabletx(uint_fast8_t state);	/* ���������� �� ����������� ���������� */
void hardware_uart2_enablerx(uint_fast8_t state);	/* ���������� �� ����������� ���������� */
uint_fast8_t hardware_usart2_putchar(uint_fast8_t c);/* �������� ������� ���� ����� ���� */
uint_fast8_t hardware_usart2_getchar(char * cp); /* ���� �������, ���� ����� ���� */

void usbd_cdc_tx(void * ctx, uint_fast8_t c);			/* �������� ������� ����� ���������� � ���������� ����������� - ���������� �� HARDWARE_CDC_ONTXCHAR */
void usbd_cdc_enabletx(uint_fast8_t state);	/* ���������� �� ����������� ���������� */
void usbd_cdc_enablerx(uint_fast8_t state);	/* ���������� �� ����������� ���������� */

/* ���������� ������ ����� USB CDC */
void debugusb_initialize(void);				/* ���������� �� user-mode ��������� ��� ����������� �����������. */
uint_fast8_t debugusb_putchar(uint_fast8_t c);/* �������� ������� ���� ����� ���� */
uint_fast8_t debugusb_getchar(char * cp); /* ���� �������, ���� ����� ���� */
void debugusb_parsechar(uint_fast8_t c);	/* ���������� �� ����������� ���������� */
void debugusb_sendchar(void * ctx);			/* ���������� �� ����������� ���������� */

void modem_spool(void);		/* ���������� �� ����������� ���������� ���������� */
void buffers_spool(void);	/* ���������� �� ����������� ���������� ���������� */

// spool-based functions for debug
int dbg_puts_impl_P(const FLASHMEM char * s);
int dbg_puts_impl(const char * s);
int dbg_putchar(int c);
int dbg_getchar(char * r);
//#define DEBUGSPEED 256000uL
#define DEBUGSPEED 57600uL

/* TWI (I2C) interface */
#define I2C_RETRIES 3	/* ���������� �������� */
void i2c_initialize(void);
void i2c_start(uint_fast8_t address);

void i2c_read(uint_fast8_t * pdata, uint_fast8_t acknak);

#define I2C_READ_ACK 0  // i2c_read parameter
#define I2C_READ_ACK_1 1  // i2c_read parameter
#define I2C_READ_NACK 2		// ack_type - last parameterr in read block
#define I2C_READ_ACK_NACK 3		// ������ ������� � ������������� ����� �� I2C
void i2c_write(uint_fast8_t data);
void i2c_write_withrestart(uint_fast8_t data);	// ������, ����� ���� restart
void i2c_waitsend(void);	// ������� ����� ���������� i2c_write()
void i2c_stop(void);

void hardware_twi_master_configure(void);

uint32_t hardware_get_random(void);

void arm_hardware_ltdc_initialize(void);	// STM32F4xxx with LCD-TFT Controller (LTDC)
void arm_hardware_sdram_initialize(void);
void arm_hardware_ltdc_set_pip(uintptr_t addr);	// set PIP framebuffer address

uint_fast8_t usbd_cdc_getrts(void);
uint_fast8_t usbd_cdc_getdtr(void);

RAMFUNC_NONILINE void AT91F_PIOA_IRQHandler(void);

void SPI2_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

void DMA1_Stream0_IRQHandler(void);		// ch0: SPI3: audio codec (rx) - on stm32F446/all
void DMA1_Stream3_IRQHandler(void);		// ch3: I2S2_EXT - audio codec (rx) on STM32F429
void DMA1_Stream4_IRQHandler(void);		// ch0: I2S2 (tx) - audio codec

void DMA2_Stream1_IRQHandler(void);		// ch0: SAI1_A (tx)
void DMA2_Stream5_IRQHandler(void);		// ch0: SAI1_B (rx)
void DMA2_Stream4_IRQHandler(void);		// ch3: SAI2_B (tx)
void DMA2_Stream7_IRQHandler(void);		// ch0: SAI2_B (rx)
void DMA2_Stream6_IRQHandler(void);		// ch4: SDIO
void SDIO_IRQHandler(void);

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);


void cpu_stm32f1xx_setmapr(unsigned long bits);

void hardware_tim21_initialize(void);

void hardware_sdhost_initialize(void);
void hardware_sdhost_setspeed(unsigned long ticksfreq);
void hardware_sdhost_setbuswidth(uint_fast8_t use4bit);

void lowtests(void);
void midtests(void);
void hightests(void);
void looptests(void);	// ������������ ���������� � ������� �����

void arm_hardware_invalidate(uintptr_t base, size_t size);	// ������ � ��� ������ ����� ������ �� DMA
void arm_hardware_flush(uintptr_t base, size_t size);	// ������ ��� ������ ����� ������������ �� DMA ����-��
void arm_hardware_flush_invalidate(uintptr_t base, size_t size);	// ������ ��� ������ ����� ������������ �� DMA ����-��. ����� ���������� �� ���������
void arm_hardware_flush_all(void);

void r7s721_sdhi0_dma_handler(void);
void r7s721_intc_registintfunc(uint_fast16_t int_id, void (* func)(void));

#if CPUSTYLE_R7S721
	//  Renesas parameters
	#define HARDWARE_USBD_PIPE_ISOC_OUT	1	// ISOC OUT ����������� �� ���������� � TRX - D0FIFOB0
	#define HARDWARE_USBD_PIPE_ISOC_IN	2	// ISOC IN ����������� � ��������� �� TRX - D0FIFOB1
	#define HARDWARE_USBD_PIPE_CDC_OUT	3	// CDC OUT ������ ���-����� �� ���������� � TRX
	#define HARDWARE_USBD_PIPE_CDC_IN	4	// CDC IN ������ ���-����� � ��������� �� TRX
	#define HARDWARE_USBD_PIPE_CDC_OUTb	14	// CDC OUT - ��� �������� ������
	#define HARDWARE_USBD_PIPE_CDC_INb	15	// CDC IN - ��� �������� ������
	#define HARDWARE_USBD_PIPE_CDC_INT	6	//
	#define HARDWARE_USBD_PIPE_CDC_INTb	7	//
#endif /* CPUSTYLE_R7S721 */

#define CATPCOUNTSIZE (13)
#define MSGBUFFERSIZE8 (9 + CATPCOUNTSIZE)

enum messagetypes
{
	MSGT_EMPTY,
	MSGT_1SEC,
	MSGT_KEYB,	// 1 byte - key code
	MSGT_CAT,	// 12 bytes as parameter
	//
	MSGT_count
};

uint_fast8_t takemsgready_user(uint8_t * * dest);	// ������ � ���������� �� ������������ ���������� �����������
void releasemsgbuffer_user(uint8_t * dest);	// ������������ ������������� ������ ���������
size_t takemsgbufferfree_low(uint8_t * * dest);	// ����� ��� ������������ ���������
void placesemsgbuffer_low(uint_fast8_t type, uint8_t * dest);	// ��������� ��������� � ������� � ���������� 

#include "product.h"
#include "taildefs.h"

#endif // HARDWARE_H_INCLUDED
