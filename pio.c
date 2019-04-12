// $Id$
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */
#include "pio.h"

#include <string.h>
#include <math.h>

#if CPUSTYLE_STM32F
// ������� ������� ���� � ����� � ������� � ����������� � 4 ���� �������.
static portholder_t
power4(uint_fast8_t v)
{
/*
	portholder_t r = 0;

	r |= (v & (1U << 0)) ? (1UL << 0) : 0;
	r |= (v & (1U << 1)) ? (1UL << 4) : 0;
	r |= (v & (1U << 2)) ? (1UL << 8) : 0;
	r |= (v & (1U << 3)) ? (1UL << 12) : 0;
	r |= (v & (1U << 4)) ? (1UL << 16) : 0;
	r |= (v & (1U << 5)) ? (1UL << 20) : 0;
	r |= (v & (1U << 6)) ? (1UL << 24) : 0;
	r |= (v & (1U << 7)) ? (1UL << 28) : 0;

	return r;
*/
	static const FLASHMEM uint_fast32_t tablepow4 [256] =
	{
	 0x00000000UL,  0x00000001UL,  0x00000010UL,  0x00000011UL,  0x00000100UL,  0x00000101UL,  0x00000110UL,  0x00000111UL,
	 0x00001000UL,  0x00001001UL,  0x00001010UL,  0x00001011UL,  0x00001100UL,  0x00001101UL,  0x00001110UL,  0x00001111UL,
	 0x00010000UL,  0x00010001UL,  0x00010010UL,  0x00010011UL,  0x00010100UL,  0x00010101UL,  0x00010110UL,  0x00010111UL,
	 0x00011000UL,  0x00011001UL,  0x00011010UL,  0x00011011UL,  0x00011100UL,  0x00011101UL,  0x00011110UL,  0x00011111UL,
	 0x00100000UL,  0x00100001UL,  0x00100010UL,  0x00100011UL,  0x00100100UL,  0x00100101UL,  0x00100110UL,  0x00100111UL,
	 0x00101000UL,  0x00101001UL,  0x00101010UL,  0x00101011UL,  0x00101100UL,  0x00101101UL,  0x00101110UL,  0x00101111UL,
	 0x00110000UL,  0x00110001UL,  0x00110010UL,  0x00110011UL,  0x00110100UL,  0x00110101UL,  0x00110110UL,  0x00110111UL,
	 0x00111000UL,  0x00111001UL,  0x00111010UL,  0x00111011UL,  0x00111100UL,  0x00111101UL,  0x00111110UL,  0x00111111UL,
	 0x01000000UL,  0x01000001UL,  0x01000010UL,  0x01000011UL,  0x01000100UL,  0x01000101UL,  0x01000110UL,  0x01000111UL,
	 0x01001000UL,  0x01001001UL,  0x01001010UL,  0x01001011UL,  0x01001100UL,  0x01001101UL,  0x01001110UL,  0x01001111UL,
	 0x01010000UL,  0x01010001UL,  0x01010010UL,  0x01010011UL,  0x01010100UL,  0x01010101UL,  0x01010110UL,  0x01010111UL,
	 0x01011000UL,  0x01011001UL,  0x01011010UL,  0x01011011UL,  0x01011100UL,  0x01011101UL,  0x01011110UL,  0x01011111UL,
	 0x01100000UL,  0x01100001UL,  0x01100010UL,  0x01100011UL,  0x01100100UL,  0x01100101UL,  0x01100110UL,  0x01100111UL,
	 0x01101000UL,  0x01101001UL,  0x01101010UL,  0x01101011UL,  0x01101100UL,  0x01101101UL,  0x01101110UL,  0x01101111UL,
	 0x01110000UL,  0x01110001UL,  0x01110010UL,  0x01110011UL,  0x01110100UL,  0x01110101UL,  0x01110110UL,  0x01110111UL,
	 0x01111000UL,  0x01111001UL,  0x01111010UL,  0x01111011UL,  0x01111100UL,  0x01111101UL,  0x01111110UL,  0x01111111UL,
	 0x10000000UL,  0x10000001UL,  0x10000010UL,  0x10000011UL,  0x10000100UL,  0x10000101UL,  0x10000110UL,  0x10000111UL,
	 0x10001000UL,  0x10001001UL,  0x10001010UL,  0x10001011UL,  0x10001100UL,  0x10001101UL,  0x10001110UL,  0x10001111UL,
	 0x10010000UL,  0x10010001UL,  0x10010010UL,  0x10010011UL,  0x10010100UL,  0x10010101UL,  0x10010110UL,  0x10010111UL,
	 0x10011000UL,  0x10011001UL,  0x10011010UL,  0x10011011UL,  0x10011100UL,  0x10011101UL,  0x10011110UL,  0x10011111UL,
	 0x10100000UL,  0x10100001UL,  0x10100010UL,  0x10100011UL,  0x10100100UL,  0x10100101UL,  0x10100110UL,  0x10100111UL,
	 0x10101000UL,  0x10101001UL,  0x10101010UL,  0x10101011UL,  0x10101100UL,  0x10101101UL,  0x10101110UL,  0x10101111UL,
	 0x10110000UL,  0x10110001UL,  0x10110010UL,  0x10110011UL,  0x10110100UL,  0x10110101UL,  0x10110110UL,  0x10110111UL,
	 0x10111000UL,  0x10111001UL,  0x10111010UL,  0x10111011UL,  0x10111100UL,  0x10111101UL,  0x10111110UL,  0x10111111UL,
	 0x11000000UL,  0x11000001UL,  0x11000010UL,  0x11000011UL,  0x11000100UL,  0x11000101UL,  0x11000110UL,  0x11000111UL,
	 0x11001000UL,  0x11001001UL,  0x11001010UL,  0x11001011UL,  0x11001100UL,  0x11001101UL,  0x11001110UL,  0x11001111UL,
	 0x11010000UL,  0x11010001UL,  0x11010010UL,  0x11010011UL,  0x11010100UL,  0x11010101UL,  0x11010110UL,  0x11010111UL,
	 0x11011000UL,  0x11011001UL,  0x11011010UL,  0x11011011UL,  0x11011100UL,  0x11011101UL,  0x11011110UL,  0x11011111UL,
	 0x11100000UL,  0x11100001UL,  0x11100010UL,  0x11100011UL,  0x11100100UL,  0x11100101UL,  0x11100110UL,  0x11100111UL,
	 0x11101000UL,  0x11101001UL,  0x11101010UL,  0x11101011UL,  0x11101100UL,  0x11101101UL,  0x11101110UL,  0x11101111UL,
	 0x11110000UL,  0x11110001UL,  0x11110010UL,  0x11110011UL,  0x11110100UL,  0x11110101UL,  0x11110110UL,  0x11110111UL,
	 0x11111000UL,  0x11111001UL,  0x11111010UL,  0x11111011UL,  0x11111100UL,  0x11111101UL,  0x11111110UL,  0x11111111UL,
	};

	return tablepow4 [v & 0xff];
}
#endif /* CPUSTYLE_STM32F */

#if CPUSTYLE_STM32F
// ������� ������� ���� � ����� � ������� � ����������� � 2 ���� �������.
static portholder_t
power2(uint_fast16_t v)
{
	portholder_t r = 0;

	r |= (v & (1U << 0)) ? (1UL << 0) : 0;
	r |= (v & (1U << 1)) ? (1UL << 2) : 0;
	r |= (v & (1U << 2)) ? (1UL << 4) : 0;
	r |= (v & (1U << 3)) ? (1UL << 6) : 0;
	r |= (v & (1U << 4)) ? (1UL << 8) : 0;
	r |= (v & (1U << 5)) ? (1UL << 10) : 0;
	r |= (v & (1U << 6)) ? (1UL << 12) : 0;
	r |= (v & (1U << 7)) ? (1UL << 14) : 0;
	r |= (v & (1U << 8)) ? (1UL << 16) : 0;
	r |= (v & (1U << 9)) ? (1UL << 18) : 0;
	r |= (v & (1U << 10)) ? (1UL << 20) : 0;
	r |= (v & (1U << 11)) ? (1UL << 22) : 0;
	r |= (v & (1U << 12)) ? (1UL << 24) : 0;
	r |= (v & (1U << 13)) ? (1UL << 26) : 0;
	r |= (v & (1U << 14)) ? (1UL << 28) : 0;
	r |= (v & (1U << 15)) ? (1UL << 30) : 0;

	return r;
}
#endif /* CPUSTYLE_STM32F */

#if CPUSTYLE_R7S721

	// Port Register (Pn)
	//GPIO.P7 &= ~ (1U << 1);

	// Port Register (Pn)
	//GPIO.P7 |= (1U << 1);

#define r7s721_pio_inputs(n, ipins1) do { \
		const uint16_t ipins2 = (ipins1); \
		GPIO.PIPC ## n &= ~ (ipins2);	/* Port IP Control Register: 0 - direction control from PMn, 1 - from alternative function */ \
		GPIO.PMC ## n &= ~ (ipins2);	/* Port Mode Control Register: 0 - port, 1 - alternative */ \
		GPIO.PM ## n |= (ipins2);	/* Port Mode Register (PMn): 0 - output, 1 - input */ \
		GPIO.PIBC ## n |= (ipins2);	/* Port Input Buffer Control Register (PIBCn): 0 - hiZ, 1 - input */ \
	} while (0)

#define r7s721_pio_outputs(n, opins1, initialstate1) do { \
		const uint16_t initialstate2 = (initialstate1); \
		const uint16_t opins2 = (opins1); \
		GPIO.PIPC ## n &= ~ (opins2);	/* Port IP Control Register: 0 - direction control from PMn, 1 - from alternative function */ \
		GPIO.PSR ## n = ((opins2) * 0x10000UL) | ((initialstate2) & (opins2)); \
		GPIO.PMC ## n &= ~ (opins2);	/* Port Mode Control Register: 0 - port, 1 - alternative */ \
		GPIO.PM ## n &= ~ (opins2);	/* Port Mode Register (PMn): 0 - output, 1 - input */ \
		GPIO.PIBC ## n &= ~ (opins2);	/* Port Input Buffer Control Register (PIBCn): 0 - hiZ, 1 - input */ \
	} while (0)

#define r7s721_pio_alternative(n, iopins1, alt1) do { \
		const uint16_t iopins2 = (iopins1); \
		const uint16_t alt2 = (alt1); \
		const int pfcae = ((alt2) & 0x04) != 0; \
		const int pfce = ((alt2) & 0x02) != 0; \
		const int pfc = ((alt2) & 0x01) != 0; \
		GPIO.PM ## n |= (iopins2);	/* Port Mode Register (PMn): 0 - output, 1 - input */ \
		GPIO.PMC ## n |= (iopins2);	/* Port Mode Control Register: 0 - port, 1 - alternative */ \
		GPIO.PFCAE ## n = (GPIO.PFCAE ## n & ~ (iopins2)) | ((iopins2) * pfcae); /* Port Function Control Additional Expansion Register (PFCAEn) */ \
		GPIO.PFCE ## n = (GPIO.PFCE ## n & ~ (iopins2)) | ((iopins2) * pfce); /* Port Function Control Expansion Register (PFCEn) */ \
		GPIO.PFC ## n = (GPIO.PFC ## n & ~ (iopins2)) | ((iopins2) * pfc); ; /* Port Function Control Register (PFCn) */ \
		GPIO.PIPC ## n |= (iopins2);	/* Port IP Control Register: 0 - direction control from PMn, 1 - from alternative function */ \
	} while (0)


// R7S721 interrupts

static void r7s721_pio_onchangeinterrupt(
	int irqbase,	/* irq id ��� �������� ���� ���������� ����� */
	int width, 
	unsigned long ipins, 
	int edge,	/* rising edge sensetive or level sensetive */
	uint32_t priority, 
	void (* vector)(void)
	)
{
	int bitpos;
	for (bitpos = 0; bitpos < width; ++ bitpos)
	{
		unsigned long mask = 1uL << bitpos;
		if ((ipins & mask) == 0)
			continue;
		const uint16_t int_id = irqbase + bitpos;
		r7s721_intc_registintfunc(int_id, vector);	/* ==== Register interrupt handler ==== */
		GIC_SetPriority(int_id, priority);
		//r7s721_intc_setconfiguration(int_id, edge ? INTC_EDGE_TRIGGER : INTC_LEVEL_SENSITIVE);
		GIC_SetConfiguration(int_id, edge ? 0x02 : 0x00);
		GIC_EnableIRQ(int_id);		/* ==== Validate interrupt ==== */
	}
}

// inputs
void arm_hardware_pio1_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(1, ipins);
}

void arm_hardware_pio2_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(2, ipins);
}

void arm_hardware_pio3_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(3, ipins);
}

void arm_hardware_pio4_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(4, ipins);
}

void arm_hardware_pio5_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(5, ipins);
}

void arm_hardware_pio6_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(6, ipins);
}

void arm_hardware_pio7_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(7, ipins);
}

void arm_hardware_pio8_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(8, ipins);
}

void arm_hardware_pio9_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(9, ipins);
}

#if CPUSTYLE_R7S721001	// RZ/A1H

void arm_hardware_pio10_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(10, ipins);
}

void arm_hardware_pio11_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(11, ipins);
}
#endif /* CPUSTYLE_R7S721001 */

// outputs
void arm_hardware_pio1_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(1, opins, initialstate);
}

void arm_hardware_pio2_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(2, opins, initialstate);
}

void arm_hardware_pio3_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(3, opins, initialstate);
}

void arm_hardware_pio4_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(4, opins, initialstate);
}

void arm_hardware_pio5_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(5, opins, initialstate);
}

void arm_hardware_pio6_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(6, opins, initialstate);
}

void arm_hardware_pio7_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(7, opins, initialstate);
}

void arm_hardware_pio8_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(8, opins, initialstate);
}

void arm_hardware_pio9_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(9, opins, initialstate);
}

#if CPUSTYLE_R7S721001	// RZ/A1H

void arm_hardware_pio10_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(10, opins, initialstate);
}

void arm_hardware_pio11_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(11, opins, initialstate);
}
#endif /* CPUSTYLE_R7S721001 */

// alternative
void arm_hardware_pio1_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(1, iopins, alt);
}

void arm_hardware_pio2_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(2, iopins, alt);
}

void arm_hardware_pio3_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(3, iopins, alt);
}

void arm_hardware_pio4_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(4, iopins, alt);
}

void arm_hardware_pio5_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(5, iopins, alt);
}

void arm_hardware_pio6_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(6, iopins, alt);
}

void arm_hardware_pio7_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(7, iopins, alt);
}

void arm_hardware_pio8_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(8, iopins, alt);
}

void arm_hardware_pio9_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(9, iopins, alt);
}

#if CPUSTYLE_R7S721001	// RZ/A1H

void arm_hardware_pio10_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(10, iopins, alt);
}

void arm_hardware_pio11_alternative(unsigned long iopins, unsigned alt)
{
	r7s721_pio_alternative(11, iopins, alt);
}
#endif /* CPUSTYLE_R7S721001 */

// pin change interrupts
void arm_hardware_piojp0_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT0_IRQn, 2, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT0_IRQn, 2, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio0_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT2_IRQn, 4, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT2_IRQn, 6, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio1_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT6_IRQn, 16, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT8_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio2_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT22_IRQn, 10, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT24_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio3_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT32_IRQn, 16, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT40_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio4_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT48_IRQn, 8, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT56_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio5_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT56_IRQn, 16, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT72_IRQn, 11, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio6_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT72_IRQn, 16, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT83_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio7_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT88_IRQn, 12, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT99_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio8_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT100_IRQn, 16, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT115_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio9_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT116_IRQn, 6, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT131_IRQn, 8, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

#if CPUSTYLE_R7S721001	// RZ/A1H

void arm_hardware_pio10_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
	r7s721_pio_onchangeinterrupt(TINT139_IRQn, 16, ipins, edge, priority, vector);
}

void arm_hardware_pio11_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
	r7s721_pio_onchangeinterrupt(TINT155_IRQn, 16, ipins, edge, priority, vector);
}

#endif /* CPUSTYLE_R7S721001 */

static void (* r7s721_IRQn_user [8])(void);

static void r7s721_IRQn_IRQHandler(void)
{
	const uint_fast8_t irqrr = INTC.IRQRR;
	if ((irqrr & (1U << 0)) != 0)
	{
		enum { irq = 0 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 1)) != 0)
	{
		enum { irq = 1 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 2)) != 0)
	{
		enum { irq = 2 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 3)) != 0)
	{
		enum { irq = 3 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 4)) != 0)
	{
		enum { irq = 4 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 5)) != 0)
	{
		enum { irq = 5 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 6)) != 0)
	{
		enum { irq = 6 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 7)) != 0)
	{
		enum { irq = 7 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		(* r7s721_IRQn_user [irq])();
	}
}

// IRQ0..IRQ7
/*
	edge values
	00: Interrupt request is detected on low level of IRQn input
	01: Interrupt request is detected on falling edge of IRQn input
	10: Interrupt request is detected on rising edge of IRQn input
	11: Interrupt request is detected on both edges of IRQn input
*/

void arm_hardware_irqn_interrupt(unsigned long irq, int edge, uint32_t priority, void (* vector)(void))
{
	r7s721_IRQn_user [irq] = vector;

	INTC.ICR1 = (INTC.ICR1 & ~ (0x03uL << (irq * 2))) |
		edge * (1uL << (irq * 2)) |
		0;
	{
		const uint16_t int_id = IRQ0_IRQn + irq;
		r7s721_intc_registintfunc(int_id, r7s721_IRQn_IRQHandler);	/* ==== Register interrupt handler ==== */
		GIC_SetPriority(int_id, priority);
		//r7s721_intc_setconfiguration(int_id, INTC_LEVEL_SENSITIVE);
		GIC_SetConfiguration(int_id, 0x00);	// level sensitice
		GIC_EnableIRQ(int_id);		/* ==== Validate interrupt ==== */
	}
}

#elif CPUSTYLE_ARM

	/* ��������� ���������� ��������� �����  � GPIO STM32F4X */
	#define arm_stm32f4xx_hardware_pio_setstate(gpio, opins, initialstate) \
	  do { \
		const portholder_t op = (opins); \
		const portholder_t is = (initialstate); \
		(gpio)->BSRR = \
			BSRR_S((is) & (op)) | /* set bits */ \
			BSRR_C(~ (is) & (op)) | /* reset bits */ \
			0; \
		} while (0)

	#if CPUSTYLE_STM32F1XX


	/* ��������� ����� � �������� AFIO_MAPR. �������������� ����� ������������ JTAG */
	void cpu_stm32f1xx_setmapr(
		unsigned long bits
		)
	{
		RCC->APB2ENR |=  RCC_APB2ENR_AFIOEN;     //�������� ������������ �������������� �������
		__DSB();
		AFIO->MAPR = (AFIO->MAPR & ~ AFIO_MAPR_SWJ_CFG) | AFIO_MAPR_SWJ_CFG_JTAGDISABLE | bits;
	}

	#define arm_stm32f10x_hardware_pio_prog(gpio, iomask, cnf, mode) \
	  do { \
		const portholder_t lo = power4((iomask) >> 0);	\
		const portholder_t hi = power4((iomask) >> 8);	\
		(gpio)->CRL = ((gpio)->CRL & ~ ((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) * lo)) | (((GPIO_CRL_MODE0_0 * (mode)) | (GPIO_CRL_CNF0_0 * (cnf))) * lo);	\
		(gpio)->CRH = ((gpio)->CRH & ~ ((GPIO_CRH_MODE8 | GPIO_CRH_CNF8) * hi)) | (((GPIO_CRH_MODE8_0 * (mode)) | (GPIO_CRH_CNF8_0 * (cnf))) * hi);	\
	  } while (0)

	// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
	#define arm_stm32f10x_hardware_pio_pupdr(gpio, up, down) \
	  do { \
		(gpio)->BSRR = \
			BSRR_S(up) |	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
			BSRR_C(down);	/* ��� ��������� pull-down ��� CNFy[1:0] = 10 */	\
	  } while (0)
		// ���������� ���������� �������� �� ����� (��� ��� ���������������� �� ���� ������ �������� ��������
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
	#define arm_stm32f10x_hardware_pio_pupoff(gpio, ipins) \
	  do { \
		/* const portholder_t ipins3 = power2(ipins); */	\
		/* (gpio)->PUPDR = ((gpio)->PUPDR & ~ (ipins3 * GPIO_PUPDR_PUPDR0)) | ipins3 * (0) * GPIO_PUPDR_PUPDR0_0; */ \
	  } while (0)

	/* ���������� ���������� �� ��������� ��������� ��������� ����� ������� */
	static void 
	arm_stm32f10x_hardware_pio_onchangeinterrupt(portholder_t ipins, 
			portholder_t raise, portholder_t fall,
			portholder_t portcode, uint32_t priority)
	{
		//const portholder_t portcode = AFIO_EXTICR1_EXTI0_PB;	// PORT B
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //�������� ������������ �������������� �������
		__DSB();

	#if 1
		{
			const portholder_t bitpos0 = power4((ipins >> 0) & 0x0f);
			AFIO->EXTICR [0] = (AFIO->EXTICR [0] & ~ (AFIO_EXTICR1_EXTI0 * bitpos0)) | (portcode * bitpos0);
		}
		{
			const portholder_t bitpos1 = power4((ipins >> 4) & 0x0f);
			AFIO->EXTICR [1] = (AFIO->EXTICR [1] & ~ (AFIO_EXTICR2_EXTI4 * bitpos1)) | (portcode * bitpos1);
		}
		{
			const portholder_t bitpos2 = power4((ipins >> 8) & 0x0f);
			AFIO->EXTICR [2] = (AFIO->EXTICR [2] & ~ (AFIO_EXTICR3_EXTI8 * bitpos2)) | (portcode * bitpos2);
		}
		{
			const portholder_t bitpos3 = power4((ipins >> 12) & 0x0f);
			AFIO->EXTICR [3] = (AFIO->EXTICR [3] & ~ (AFIO_EXTICR4_EXTI12 * bitpos3)) | (portcode * bitpos3);
		}
	#else
		uint_fast8_t i;
		for (i = 0; i < 16; ++ i)
		{
			const portholder_t pinmask = (portholder_t) 1UL << i;
			if ((ipins & pinmask) == 0)
				continue;	// ��� ���� �� �������

			const div_t d = div(i, 4);
			const portholder_t bitpos = (portholder_t) 1 << (d.rem * 4);
			const portholder_t bitmask = AFIO_EXTICR1_EXTI0 * bitpos;
			const portholder_t bitvalue = portcode * bitpos;
			AFIO->EXTICR [d.quot] = (AFIO->EXTICR [d.quot] & ~ bitmask) | bitvalue;
		}
	#endif

		EXTI->RTSR = (EXTI->RTSR & ~ ipins) | (ipins & raise);		// ���������� �� ����������
		EXTI->FTSR = (EXTI->FTSR & ~ ipins) | (ipins & fall);		// ���������� �� �����
		EXTI->IMR |= ipins;		// ��������� ����������

		if ((ipins & EXTI_IMR_MR0) != 0)
		{
			NVIC_SetPriority(EXTI0_IRQn, priority);
			NVIC_EnableIRQ(EXTI0_IRQn);		// enable EXTI0_IRQHandler();
		}
		if ((ipins & EXTI_IMR_MR1) != 0)
		{
			NVIC_SetPriority(EXTI1_IRQn, priority);
			NVIC_EnableIRQ(EXTI1_IRQn);		// enable EXTI1_IRQHandler();
		}
		if ((ipins & EXTI_IMR_MR2) != 0)
		{
			NVIC_SetPriority(EXTI2_IRQn, priority);
			NVIC_EnableIRQ(EXTI2_IRQn);		// enable EXTI2_IRQHandler();
		}
		if ((ipins & EXTI_IMR_MR3) != 0)
		{
			NVIC_SetPriority(EXTI3_IRQn, priority);
			NVIC_EnableIRQ(EXTI3_IRQn);		// enable EXTI3_IRQHandler();
		}
		if ((ipins & EXTI_IMR_MR4) != 0)
		{
			NVIC_SetPriority(EXTI4_IRQn, priority);
			NVIC_EnableIRQ(EXTI4_IRQn);		// enable EXTI4_IRQHandler();
		}
		if ((ipins & (EXTI_IMR_MR9 | EXTI_IMR_MR8 | EXTI_IMR_MR7 | EXTI_IMR_MR6 | EXTI_IMR_MR5)) != 0)
		{
			NVIC_SetPriority(EXTI9_5_IRQn, priority);
			NVIC_EnableIRQ(EXTI9_5_IRQn);	// enable EXTI9_5_IRQHandler();
		}
		if ((ipins & (EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR14 | EXTI_IMR_MR13 | EXTI_IMR_MR12 | EXTI_IMR_MR11 | EXTI_IMR_MR10)) != 0)
		{
			NVIC_SetPriority(EXTI15_10_IRQn, priority);
			NVIC_EnableIRQ(EXTI15_10_IRQn);	// enable EXTI15_10_IRQHandler();
		}

	}

	/* ���������������� ����� � ��������� ���������� GPIO, ��������� � iomask, � ������������ CNF � ����� MODE */
	#elif \
		CPUSTYLE_STM32F30X || \
		CPUSTYLE_STM32F4XX || \
		CPUSTYLE_STM32F0XX || \
		CPUSTYLE_STM32L0XX || \
		CPUSTYLE_STM32F7XX || \
		CPUSTYLE_STM32H7XX || \
		0

		/* ���������������� ����� � ��������� ���������� GPIO, ��������� � iomask, � ������������ MODER SPEED PUPDR TYPER */
		// modev: 0:input, 1:output, 2:alternative function mode, 3:analog mode
		// speed: 0:low speed, 1:maximum speed, 2:fast speed, 3:high speed
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		// type: 0: Output push-pull, 1: output open-drain,
		#define arm_stm32f30x_hardware_pio_prog(gpio, iomask0, moder, speed, pupdr, typer) \
		  do { \
			const portholder_t iomask = (iomask0);	\
			const portholder_t mask3 = power2(iomask);	\
			(gpio)->MODER = ((gpio)->MODER & ~ (mask3 * GPIO_MODER_MODER0)) | mask3 * (moder) * GPIO_MODER_MODER0_0; \
			(gpio)->OSPEEDR = ((gpio)->OSPEEDR & ~ (mask3 * GPIO_OSPEEDER_OSPEEDR0)) | mask3 * (speed) * GPIO_OSPEEDER_OSPEEDR0_0; \
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ (mask3 * GPIO_PUPDR_PUPDR0)) | mask3 * (pupdr) * GPIO_PUPDR_PUPDR0_0; \
			(gpio)->OTYPER = ((gpio)->OTYPER & ~ ((iomask) * GPIO_OTYPER_OT_0)) | (iomask) * (typer); \
		  } while (0)
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		#define arm_stm32f30x_hardware_pio_pupdr(gpio, up, down) \
		  do { \
			const portholder_t up3 = power2(up); \
			const portholder_t down3 = power2(down); \
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ ((up3 | down3) * GPIO_PUPDR_PUPDR0)) | \
				up3 * (1) * GPIO_PUPDR_PUPDR0_0 | \
				down3 * (2) * GPIO_PUPDR_PUPDR0_0 | \
				0; \
		  } while (0)
		// ���������� ���������� �������� �� ����� (��� ��� ���������������� �� ���� � ������ ���������� ������ �������� ��������
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		#define arm_stm32f30x_hardware_pio_pupoff(gpio, ipins) \
		  do { \
			const portholder_t ipins3 = power2(ipins);	\
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ (ipins3 * GPIO_PUPDR_PUPDR0)) | ipins3 * (0) * GPIO_PUPDR_PUPDR0_0; \
		  } while (0)


		/*
		* @param  GPIO_AF: selects the pin to be used as Alternate function.    
		*   This parameter can be one of the following value:  
		*     @arg GPIO_AF_0:  JTCK-SWCLK, JTDI, JTDO/TRACESW0, JTMS-SWDAT, MCO, NJTRST,   
		*                      TRACED, TRACECK.  
		*     @arg GPIO_AF_1:  OUT, TIM2, TIM15, TIM16, TIM17.  
		*     @arg GPIO_AF_2:  COMP1_OUT, TIM1, TIM2, TIM3, TIM4, TIM8, TIM15.  
		*     @arg GPIO_AF_3:  COMP7_OUT, TIM8, TIM15, Touch.  
		*     @arg GPIO_AF_4:  I2C1, I2C2, TIM1, TIM8, TIM16, TIM17.  
		*     @arg GPIO_AF_5:  IR_OUT, I2S2, I2S3, SPI1, SPI2, TIM8, USART4, USART5  
		*     @arg GPIO_AF_6:  IR_OUT, I2S2, I2S3, SPI2, SPI3, TIM1, TIM8  
		*     @arg GPIO_AF_7:  AOP2_OUT, CAN, COMP3_OUT, COMP5_OUT, COMP6_OUT, USART1,   
		*                      USART2, USART3.  
		*     @arg GPIO_AF_8:  COMP1_OUT, COMP2_OUT, COMP3_OUT, COMP4_OUT, COMP5_OUT,   
		*                      COMP6_OUT.  
		*     @arg GPIO_AF_9:  AOP4_OUT, CAN, TIM1, TIM8, TIM15.  
		*     @arg GPIO_AF_10: AOP1_OUT, AOP3_OUT, TIM2, TIM3, TIM4, TIM8, TIM17.   
		*     @arg GPIO_AF_11: TIM1, TIM8.  
		*     @arg GPIO_AF_12: TIM1.  
		*     @arg GPIO_AF_14: USBDM, USBDP.  
		*     @arg GPIO_AF_15: OUT.             	
		*/
		#define arm_stm32f30x_hardware_pio_altfn(gpio, opins, afn) \
			{ \
				const portholder_t op = (opins); \
				const portholder_t lo = power4((op) >> 0); \
				const portholder_t hi = power4((op) >> 8); \
				(gpio)->AFR [0] = ((gpio)->AFR [0] & ~ (lo * 0x0f)) | (lo * (afn)); \
				(gpio)->AFR [1] = ((gpio)->AFR [1] & ~ (hi * 0x0f)) | (hi * (afn)); \
			} while (0)
		/* ���������� ���������� �� ��������� ��������� ��������� ����� ������� */
		static void 
		arm_stm32f30x_hardware_pio_onchangeinterrupt(portholder_t ipins, 
				portholder_t raise, portholder_t fall,
				portholder_t portcode, uint32_t priority)
		{
			#if CPUSTYLE_STM32H7XX
				RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;     // �������� ������������ �������������� �������
				__DSB();
			#else /* CPUSTYLE_STM32H7XX */
				RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;     // �������� ������������ �������������� �������
				__DSB();
			#endif /* CPUSTYLE_STM32H7XX */
			//const portholder_t portcode = AFIO_EXTICR1_EXTI0_PB;	// PORT B
			//RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     // �������� ������������ �������������� �������

		#if CPUSTYLE_STM32H7XX

			#if 1
				{
					const portholder_t bitpos0 = power4((ipins >> 0) & 0x0f);
					SYSCFG->EXTICR [0] = (SYSCFG->EXTICR [0] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos0)) | (portcode * bitpos0);
				}
				{
					const portholder_t bitpos1 = power4((ipins >> 4) & 0x0f);
					SYSCFG->EXTICR [1] = (SYSCFG->EXTICR [1] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos1)) | (portcode * bitpos1);
				}
				{
					const portholder_t bitpos2 = power4((ipins >> 8) & 0x0f);
					SYSCFG->EXTICR [2] = (SYSCFG->EXTICR [2] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos2)) | (portcode * bitpos2);
				}
				{
					const portholder_t bitpos3 = power4((ipins >> 12) & 0x0f);
					SYSCFG->EXTICR [3] = (SYSCFG->EXTICR [3] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos3)) | (portcode * bitpos3);
				}
			#else
				uint_fast8_t i;
				for (i = 0; i < 16; ++ i)
				{
					const portholder_t pinmask = (portholder_t) 1 << i;
					if ((ipins & pinmask) == 0)
						continue;	// ��� ���� �� �������

					const div_t d = div(i, 4);
					const portholder_t bitpos = (portholder_t) 1 << (d.rem * 4);
					const portholder_t bitmask = SYSCFG_EXTICR1_EXTI0 * bitpos;
					const portholder_t bitvalue = portcode * bitpos;
					AFIO->EXTICR [d.quot] = (SYSCFG->EXTICR [d.quot] & ~ bitmask) | bitvalue;
				}
			#endif

			EXTI->RTSR1 = (EXTI->RTSR1 & ~ ipins) | (ipins & raise);		// ���������� �� ����������
			EXTI->FTSR1 = (EXTI->FTSR1 & ~ ipins) | (ipins & fall);		// ���������� �� �����

			EXTI_D1->IMR1 |= ipins;		// ��������� ����������

		#else /* CPUSTYLE_STM32H7XX */

			#if 1
				{
					const portholder_t bitpos0 = power4((ipins >> 0) & 0x0f);
					SYSCFG->EXTICR [0] = (SYSCFG->EXTICR [0] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos0)) | (portcode * bitpos0);
				}
				{
					const portholder_t bitpos1 = power4((ipins >> 4) & 0x0f);
					SYSCFG->EXTICR [1] = (SYSCFG->EXTICR [1] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos1)) | (portcode * bitpos1);
				}
				{
					const portholder_t bitpos2 = power4((ipins >> 8) & 0x0f);
					SYSCFG->EXTICR [2] = (SYSCFG->EXTICR [2] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos2)) | (portcode * bitpos2);
				}
				{
					const portholder_t bitpos3 = power4((ipins >> 12) & 0x0f);
					SYSCFG->EXTICR [3] = (SYSCFG->EXTICR [3] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos3)) | (portcode * bitpos3);
				}
			#else
				uint_fast8_t i;
				for (i = 0; i < 16; ++ i)
				{
					const portholder_t pinmask = (portholder_t) 1 << i;
					if ((ipins & pinmask) == 0)
						continue;	// ��� ���� �� �������

					const div_t d = div(i, 4);
					const portholder_t bitpos = (portholder_t) 1 << (d.rem * 4);
					const portholder_t bitmask = SYSCFG_EXTICR1_EXTI0 * bitpos;
					const portholder_t bitvalue = portcode * bitpos;
					AFIO->EXTICR [d.quot] = (SYSCFG->EXTICR [d.quot] & ~ bitmask) | bitvalue;
				}
			#endif

			EXTI->RTSR = (EXTI->RTSR & ~ ipins) | (ipins & raise);		// ���������� �� ����������
			EXTI->FTSR = (EXTI->FTSR & ~ ipins) | (ipins & fall);		// ���������� �� �����

			EXTI->IMR |= ipins;		// ��������� ����������

		#endif /* CPUSTYLE_STM32H7XX */

		#if CPUSTYLE_STM32L0XX

			if ((ipins & (EXTI_IMR_IM1 | EXTI_IMR_IM0)) != 0)
			{
				NVIC_SetPriority(EXTI0_1_IRQn, priority);
				NVIC_EnableIRQ(EXTI0_1_IRQn);		// enable EXTI0_1_IRQHandler();
			}
			else if ((ipins & (EXTI_IMR_IM2 | EXTI_IMR_IM3)) != 0)
			{
				NVIC_SetPriority(EXTI2_3_IRQn, priority);
				NVIC_EnableIRQ(EXTI2_3_IRQn);		// enable EXTI2_3_IRQHandler();
			}
			else if ((ipins & (EXTI_IMR_IM4 | EXTI_IMR_IM5 | EXTI_IMR_IM6 | EXTI_IMR_IM7 | EXTI_IMR_IM8 | 
							EXTI_IMR_IM9 | EXTI_IMR_IM10 | EXTI_IMR_IM11 | EXTI_IMR_IM12 | EXTI_IMR_IM13 | 
							EXTI_IMR_IM14 | EXTI_IMR_IM15)) != 0)
			
				NVIC_SetPriority(EXTI4_15_IRQn, priority);
				NVIC_EnableIRQ(EXTI4_15_IRQn);		// enable EXTI4_15_IRQHandler();
			}

		#elif CPUSTYLE_STM32F0XX

			if ((ipins & (EXTI_IMR_MR1 | EXTI_IMR_MR0)) != 0)
			{
				NVIC_SetPriority(EXTI0_1_IRQn, priority);
				NVIC_EnableIRQ(EXTI0_1_IRQn);		// enable EXTI0_1_IRQHandler();
			}
			else if ((ipins & (EXTI_IMR_MR2 | EXTI_IMR_MR3)) != 0)
			{
				NVIC_SetPriority(EXTI2_3_IRQn, priority);
				NVIC_EnableIRQ(EXTI2_3_IRQn);		// enable EXTI2_3_IRQHandler();
			}
			else if ((ipins & (EXTI_MRR_MR4 | EXTI_MRR_MR5 | EXTI_MRR_MR6 | EXTI_MRR_MR7 | EXTI_MRR_MR8 | 
							EXTI_MRR_MR9 | EXTI_MRR_MR10 | EXTI_MRR_MR11 | EXTI_MRR_MR12 | EXTI_MRR_MR13 | 
							EXTI_MRR_MR14 | EXTI_MRR_MR15)) != 0)
			{
				NVIC_SetPriority(EXTI4_15_IRQn, priority);
				NVIC_EnableIRQ(EXTI4_15_IRQn);		// enable EXTI4_15_IRQHandler();
			}

		#elif CPUSTYLE_STM32F7XX

			if ((ipins & (EXTI_IMR_MR0)) != 0)
			{
				NVIC_SetPriority(EXTI0_IRQn, priority);
				NVIC_EnableIRQ(EXTI0_IRQn);		// enable EXTI0_IRQHandler();
			}
			if ((ipins & (EXTI_IMR_MR1)) != 0)
			{
				NVIC_SetPriority(EXTI1_IRQn, priority);
				NVIC_EnableIRQ(EXTI1_IRQn);		// enable EXTI1_IRQHandler();
			}
			if ((ipins & (EXTI_IMR_MR2)) != 0)
			{
				NVIC_SetPriority(EXTI2_IRQn, priority);
				NVIC_EnableIRQ(EXTI2_IRQn);		// enable EXTI2_IRQHandler();
			}
			if ((ipins & (EXTI_IMR_MR3)) != 0)
			{
				NVIC_SetPriority(EXTI3_IRQn, priority);
				NVIC_EnableIRQ(EXTI3_IRQn);		// enable EXTI3_IRQHandler();
			}
			if ((ipins & (EXTI_IMR_MR4)) != 0)
			{
				NVIC_SetPriority(EXTI4_IRQn, priority);
				NVIC_EnableIRQ(EXTI4_IRQn);		// enable EXTI4_IRQHandler();
			}
			if ((ipins & (EXTI_IMR_MR9 | EXTI_IMR_MR8 | EXTI_IMR_MR7 | EXTI_IMR_MR6 | EXTI_IMR_MR5)) != 0)
			{
				NVIC_SetPriority(EXTI9_5_IRQn, priority);
				NVIC_EnableIRQ(EXTI9_5_IRQn);	// enable EXTI9_5_IRQHandler();
			}
			if ((ipins & (EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR14 | EXTI_IMR_MR13 | EXTI_IMR_MR12 | EXTI_IMR_MR11 | EXTI_IMR_MR10)) != 0)
			{
				NVIC_SetPriority(EXTI15_10_IRQn, priority);
				NVIC_EnableIRQ(EXTI15_10_IRQn);	// enable EXTI15_10_IRQHandler();
			}
			else
			{
				//NVIC_SetPriority(EXTI4_15_IRQn, priority);
				//NVIC_EnableIRQ(EXTI4_15_IRQn);		// enable EXTI4_15_IRQHandler();
			}

		#elif CPUSTYLE_STM32H7XX

			if ((ipins & EXTI_IMR1_IM0) != 0)
			{
				NVIC_SetPriority(EXTI0_IRQn, priority);
				NVIC_EnableIRQ(EXTI0_IRQn);		// enable EXTI0_IRQHandler();
			}
			if ((ipins & EXTI_IMR1_IM1) != 0)
			{
				NVIC_SetPriority(EXTI1_IRQn, priority);
				NVIC_EnableIRQ(EXTI1_IRQn);		// enable EXTI1_IRQHandler();
			}
			if ((ipins & EXTI_IMR1_IM2) != 0)
			{
				NVIC_SetPriority(EXTI2_IRQn, priority);
				NVIC_EnableIRQ(EXTI2_IRQn);		// enable EXTI2_IRQHandler();
			}
			if ((ipins & EXTI_IMR1_IM3) != 0)
			{
				NVIC_SetPriority(EXTI3_IRQn, priority);
				NVIC_EnableIRQ(EXTI3_IRQn);		// enable EXTI3_IRQHandler();
			}
			if ((ipins & EXTI_IMR1_IM4) != 0)
			{
				NVIC_SetPriority(EXTI4_IRQn, priority);
				NVIC_EnableIRQ(EXTI4_IRQn);		// enable EXTI4_IRQHandler();
			}
			if ((ipins & (EXTI_IMR1_IM9 | EXTI_IMR1_IM8 | EXTI_IMR1_IM7 | EXTI_IMR1_IM6 | EXTI_IMR1_IM5)) != 0)
			{
				NVIC_SetPriority(EXTI9_5_IRQn, priority);
				NVIC_EnableIRQ(EXTI9_5_IRQn);	// enable EXTI9_5_IRQHandler();
			}
			if ((ipins & (EXTI_IMR1_IM15 | EXTI_IMR1_IM14 | EXTI_IMR1_IM13 | EXTI_IMR1_IM12 | EXTI_IMR1_IM11 | EXTI_IMR1_IM10)) != 0)
			{
				NVIC_SetPriority(EXTI15_10_IRQn, priority);
				NVIC_EnableIRQ(EXTI15_10_IRQn);	// enable EXTI15_10_IRQHandler();
			}

		#else /* CPUSTYLE_STM32F0XX */
			if ((ipins & EXTI_IMR_MR0) != 0)
			{
				NVIC_SetPriority(EXTI0_IRQn, priority);
				NVIC_EnableIRQ(EXTI0_IRQn);		// enable EXTI0_IRQHandler();
			}
			if ((ipins & EXTI_IMR_MR1) != 0)
			{
				NVIC_SetPriority(EXTI1_IRQn, priority);
				NVIC_EnableIRQ(EXTI1_IRQn);		// enable EXTI1_IRQHandler();
			}
			if ((ipins & EXTI_IMR_MR2) != 0)
			{
				#if CPUSTYLE_STM32F4XX
					NVIC_SetPriority(EXTI2_IRQn, priority);
					NVIC_EnableIRQ(EXTI2_IRQn);		// enable EXTI2_IRQHandler();
				#else
					NVIC_SetPriority(EXTI2_TS_IRQn, priority);
					NVIC_EnableIRQ(EXTI2_TS_IRQn);		// enable EXTI2_IRQHandler();
				#endif
			}
			if ((ipins & EXTI_IMR_MR3) != 0)
			{
				NVIC_SetPriority(EXTI3_IRQn, priority);
				NVIC_EnableIRQ(EXTI3_IRQn);		// enable EXTI3_IRQHandler();
			}
			if ((ipins & EXTI_IMR_MR4) != 0)
			{
				NVIC_SetPriority(EXTI4_IRQn, priority);
				NVIC_EnableIRQ(EXTI4_IRQn);		// enable EXTI4_IRQHandler();
			}
			if ((ipins & (EXTI_IMR_MR9 | EXTI_IMR_MR8 | EXTI_IMR_MR7 | EXTI_IMR_MR6 | EXTI_IMR_MR5)) != 0)
			{
				NVIC_SetPriority(EXTI9_5_IRQn, priority);
				NVIC_EnableIRQ(EXTI9_5_IRQn);	// enable EXTI9_5_IRQHandler();
			}
			if ((ipins & (EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR13 | EXTI_IMR_MR12 | EXTI_IMR_MR11 | EXTI_IMR_MR10)) != 0)
			{
				NVIC_SetPriority(EXTI15_10_IRQn, priority);
				NVIC_EnableIRQ(EXTI15_10_IRQn);	// enable EXTI15_10_IRQHandler();
			}
		#endif /* CPUSTYLE_STM32F0XX */

		}

	#else
		//#error Undefined CPUSTYLE_XXX
	#endif /* CPUSTYLE_STM32F30X */


/* ���������������� ������� �� ����, ��� ������������� � ��������� */
void 
arm_hardware_pioa_inputs(unsigned long ipins)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_PIOA; // ��������� ������������� ����� �����

	AT91C_BASE_PIOA->PIO_PER = ipins;				// initial disconnect - PIO enable
	AT91C_BASE_PIOA->PIO_ODR = ipins;		// this pin not output.
	AT91C_BASE_PIOA->PIO_PPUER = ipins;	// enable pull-up resistors
	//AT91C_BASE_PIOA->PIO_IFER = ipins;	// glitch filter enable
#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOA);	// ��������� ������������� ����� �����

    PIOA->PIO_PER = ipins;	// initial disconnect - PIO enable
    PIOA->PIO_ODR = ipins; // ��� ������ ��  ����
	PIOA->PIO_PUER = ipins;


#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	GPIOA->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOA, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

void 
arm_hardware_piob_inputs(unsigned long ipins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOB);	// ��������� ������������� ����� �����

    PIOA->PIO_PER = ipins;	// initial disconnect - PIO enable
    PIOB->PIO_ODR = ipins; // ��� ������ ��  ����
	PIOB->PIO_PUER = ipins;

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	__DSB();
	GPIOB->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOB, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#endif
}

void 
arm_hardware_pioc_inputs(unsigned long ipins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOC);	// ��������� ������������� ����� �����

    PIOA->PIO_PER = ipins;	// initial disconnect - PIO enable
    PIOC->PIO_ODR = ipins; // ��� ������ ��  ����
	PIOC->PIO_PUER = ipins;

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	__DSB();
	GPIOC->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOC, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#endif
}

void 
arm_hardware_piod_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	__DSB();
	GPIOD->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOD, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#endif
}

void 
arm_hardware_pioe_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	__DSB();
	GPIOE->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOE, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#endif
}


#if defined (GPIOF)
void 
arm_hardware_piof_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPFEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	__DSB();
	GPIOF->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOF, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOFEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#endif
}
#endif /* defined (GPIOF) */

#if defined (GPIOG)
void 
arm_hardware_piog_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	__DSB();
	GPIOG->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOG, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOGEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#endif
}
#endif /* defined (GPIOG) */


void 
arm_hardware_pioa_analoginput(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	GPIOA->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOA, ipins, 0, 0);	/* ���������� CNF=0 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

void 
arm_hardware_piob_analoginput(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	__DSB();
	GPIOB->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOB, ipins, 0, 0);	/* ���������� CNF=0 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

void 
arm_hardware_pioc_analoginput(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	__DSB();
	GPIOC->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOC, ipins, 0, 0);	/* ���������� CNF=0 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������������� ������� �� �����, ��� ������������� � ��������� */
/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_pioa_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_PIOA; // ��������� ������������� ����� �����

	AT91C_BASE_PIOA->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	AT91C_BASE_PIOA->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

	AT91C_BASE_PIOA->PIO_OER = opins;	// ���������� ������
	AT91C_BASE_PIOA->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOA);

 	PIOA->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	PIOA->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

	PIOA->PIO_OER = opins; // ��� ������ �� ����� - ������, ��� ������ � ������������� ������������ �� ���������.
	PIOA->PIO_PUDR = opins;
	PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX
#endif
}
/* ���������������� ������� �� �����, ��� ������������� � ��������� */
/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_pioa_outputs10m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_PIOA; // ��������� ������������� ����� �����

	AT91C_BASE_PIOA->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	AT91C_BASE_PIOA->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

	AT91C_BASE_PIOA->PIO_OER = opins;	// ���������� ������
	AT91C_BASE_PIOA->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOA);

 	PIOA->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	PIOA->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

    PIOA->PIO_OER = opins; // ��� ������ �� ����� - ������, ��� ������ � ������������� ������������ �� ���������.
	PIOA->PIO_PUDR = opins;
	PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ ������� mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX
#endif
}
/* ���������������� ������� �� �����, ��� ������������� � ��������� */
/* ��������� ������ - �����, ��� ����������� �������� (�� STM32) 50 ���	*/
void 
arm_hardware_pioa_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_PIOA; // ��������� ������������� ����� �����

	AT91C_BASE_PIOA->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	AT91C_BASE_PIOA->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

	AT91C_BASE_PIOA->PIO_OER = opins;	// ���������� ������
	AT91C_BASE_PIOA->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOA);

 	PIOA->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	PIOA->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

    PIOA->PIO_OER = opins; // ��� ������ �� ����� - ������, ��� ������ � ������������� ������������ �� ���������.
	PIOA->PIO_PUDR = opins;
	PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ ������� mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 0, 3);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */


#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_piob_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S
#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOB);

 	PIOB->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	PIOB->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

    PIOB->PIO_OER = opins; // ��� ������ �� ����� - ������, ��� ������ � ������������� ������������ �� ���������.
	PIOB->PIO_PUDR = opins;
	PIOB->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ ������� mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX
#endif
}
/* ���������������� ������� �� �����, ��� ������������� � ��������� */
/* ��������� ������ - �����, ��� ����������� �������� (�� STM32) 50 ���	*/
void 
arm_hardware_piob_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S
#elif CPUSTYLE_AT91SAM9XE

	AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_PIOB; // ��������� ������������� ����� �����

	AT91C_BASE_PIOB->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	AT91C_BASE_PIOB->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

	AT91C_BASE_PIOB->PIO_OER = opins;	// ���������� ������
	AT91C_BASE_PIOB->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOB->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOB);

 	PIOB->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	PIOB->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

    PIOB->PIO_OER = opins; // ��� ������ �� ����� - ������, ��� ������ � ������������� ������������ �� ���������.
	PIOB->PIO_PUDR = opins;
	PIOB->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ ������� mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 0, 3);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* ���������������� ������� �� �����, ��� ������������� � ��������� */
/* ��������� ������ - �����, ��� ����������� �������� (�� STM32) 50 ���	*/
void 
arm_hardware_pioc_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S
#elif CPUSTYLE_AT91SAM9XE

	AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_PIOC; // ��������� ������������� ����� �����

	AT91C_BASE_PIOC->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	AT91C_BASE_PIOC->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

	AT91C_BASE_PIOC->PIO_OER = opins;	// ���������� ������
	AT91C_BASE_PIOC->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOC->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOC);

 	PIOC->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	PIOC->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

    PIOC->PIO_OER = opins; // ��� ������ �� ����� - ������, ��� ������ � ������������� ������������ �� ���������.
	PIOC->PIO_PUDR = opins;
	PIOC->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ ������� mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 0, 3);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX
#endif
}


/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_pioc_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOC);

 	PIOC->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	PIOC->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

    PIOC->PIO_OER = opins; // ��� ������ �� ����� - ������, ��� ������ � ������������� ������������ �� ���������.
	PIOC->PIO_PUDR = opins;
	PIOC->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� �����: Max. output speed 10 MHz */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ��������� ������ - �����, � ������������ �������� 2 ���	*/
void 
arm_hardware_pioa_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ��������� ������ - �����, � ������������ �������� 2 ���	*/
void 
arm_hardware_piob_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ��������� ������ - �����, � ������������ �������� 2 ���	*/
void 
arm_hardware_pioc_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ��������� ������ - �����, � ������������ �������� 2 ���	*/
void 
arm_hardware_piod_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_piod_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}
/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_pioe_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	//GPIOE->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ��������� ������ - �����, � ������������ �������� 2 ���	*/
void 
arm_hardware_pioe_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#if defined(GPIOF)

/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_piof_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPFEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	//GPIOF->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ��������� ������ - �����, � ������������ �������� 2 ���	*/
void 
arm_hardware_piof_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPFEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOFEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined(GPIOF) */

#if defined(GPIOG)

/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_piog_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */


#endif
}

/* ��������� ������ - �����, � ������������ �������� 2 ���	*/
void 
arm_hardware_piog_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}
/* ��������� ������ - �����, ��� ����������� ��������	*/
void 
arm_hardware_piog_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined(GPIOG) */

#if defined(GPIOH)

/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_pioh_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPHEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOHEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */


#endif
}

/* ��������� ������ - �����, � ������������ �������� 2 ���	*/
void 
arm_hardware_pioh_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPHEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOHEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined(GPIOH) */


#if defined(GPIOI)

/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_pioi_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPIEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOI, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOIEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */


#endif
}

/* ��������� ������ - �����, � ������������ �������� 2 ���	*/
void 
arm_hardware_pioi_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPIEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOI, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOIEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined(GPIOI) */


#if defined(GPIOK)

/* ��������� ������ - �����, � ������������ �������� (�� STM32) 10 ���	*/
void 
arm_hardware_piok_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPKEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOK, opins, 0, 1);	/* ���������� CNF=0 � MODE=1 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOKEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */


#endif
}

/* ��������� ������ - �����, � ������������ �������� 2 ���	*/
void 
arm_hardware_piok_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPKEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOK, opins, 0, 2);	/* ���������� CNF=0 � MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOKEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined(GPIOK) */


/* ���������������� ������� �� �����, ��� ������������� � ��������� - �������� ���� */
void 
arm_hardware_pioa_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_PIOA; // ��������� ������������� ����� �����

	AT91C_BASE_PIOA->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	AT91C_BASE_PIOA->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

	AT91C_BASE_PIOA->PIO_OER = opins;	// ���������� ������
	AT91C_BASE_PIOA->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOA->PIO_MDER = opins;	// 5 volt (open drain) output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_PIOA);

 	PIOA->PIO_SODR = (opins & initialstate);	// ��������� ������� � �������� ������
	PIOA->PIO_CODR = (opins & ~ initialstate);	// ��������� ����� � �������� ������

    PIOA->PIO_OER = opins; // ��� ������ �� ����� - ������, ��� ������ � ������������� ������������ �� ���������.
	PIOA->PIO_PUER = opins;
	PIOA->PIO_MDER = opins;	// 5 volt (open drain) output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

void 
arm_hardware_piob_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}

void 
arm_hardware_pioc_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}

void 
arm_hardware_piod_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}

void 
arm_hardware_pioe_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPEEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}

#if CPUSTYLE_STM32F && defined (GPIOF)
void 
arm_hardware_piof_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPFEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOFEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}
#endif /* CPUSTYLE_STM32F && defined (GPIOF) */

#if CPUSTYLE_STM32F && defined (GPIOG)
void 
arm_hardware_piog_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->IOPENR |= RCC_IOPENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}
#endif /* CPUSTYLE_STM32F && defined (GPIOG) */

#if CPUSTYLE_STM32F && defined (GPIOH)
void 
arm_hardware_pioh_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPHEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOHEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ���������� ��������� �����
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}
#endif /* CPUSTYLE_STM32F && defined (GPIOH) */

// ������ ������������ � periph A (Atmel specific)
void arm_hardware_pioa_peripha(unsigned long pins)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PIOA->PIO_ASR = pins;                // assign pins to periph A
	AT91C_BASE_PIOA->PIO_PDR = pins;				// outputs are peripherial

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
    // setup PIO pins for SPI bus
    //PIOA->PIO_ASR = WORKMASK;                 // assign pins to SPI interface
     // enable uart pins on PIO
	PIOA->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOA->PIO_ABCDSR [0] &= ~ (pins); // assigns the I/O lines to peripheral A function
	PIOA->PIO_ABCDSR [1] &= ~ (pins); // assigns the I/O lines to peripheral A function
	PIOA->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}
// ������ ������������ � periph B (Atmel specific)
void arm_hardware_pioa_periphb(unsigned long pins)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PIOA->PIO_BSR = pins;                // assign pins to periph B
	AT91C_BASE_PIOA->PIO_PDR = pins;				// outputs are peripherial

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOA->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOA->PIO_ABCDSR [0] |= (pins); // assigns the I/O lines to peripheral B function
	PIOA->PIO_ABCDSR [1] &= ~ (pins); // assigns the I/O lines to peripheral B function
	PIOA->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}

// ������ ������������ � periph A (Atmel specific)
void arm_hardware_piob_peripha(unsigned long pins)
{
#if CPUSTYLE_AT91SAM7S

	//AT91C_BASE_PIOB->PIO_ASR = pins;                // assign pins to periph A
	//AT91C_BASE_PIOB->PIO_PDR = pins;				// outputs are peripherial

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
    // setup PIO pins for SPI bus
    //PIOA->PIO_ASR = WORKMASK;                 // assign pins to SPI interface
     // enable uart pins on PIO
	PIOB->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOB->PIO_ABCDSR [0] &= ~ (pins); // assigns the I/O lines to peripheral A function
	PIOB->PIO_ABCDSR [1] &= ~ (pins); // assigns the I/O lines to peripheral A function
	PIOB->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}
// ������ ������������ � periph B (Atmel specific)
void arm_hardware_piob_periphb(unsigned long pins)
{
#if CPUSTYLE_AT91SAM7S

	//AT91C_BASE_PIOA->PIO_BSR = pins;                // assign pins to periph B
	//AT91C_BASE_PIOA->PIO_PDR = pins;				// outputs are peripherial

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOB->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOB->PIO_ABCDSR [0] |= (pins); // assigns the I/O lines to peripheral B function
	PIOB->PIO_ABCDSR [1] &= ~ (pins); // assigns the I/O lines to peripheral B function
	PIOB->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

// ������ ������������ � periph C (Atmel specific)
void arm_hardware_pioa_periphc(unsigned long pins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOA->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOA->PIO_ABCDSR [0] &= ~ (pins); // assigns the I/O lines to peripheral C function
	PIOA->PIO_ABCDSR [1] |= (pins); // assigns the I/O lines to peripheral C function
	PIOA->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}

// ������ ������������ � periph D (Atmel specific)
void arm_hardware_pioa_periphd(unsigned long pins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOA->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOA->PIO_ABCDSR [0] |= (pins); // assigns the I/O lines to peripheral D function
	PIOA->PIO_ABCDSR [1] |= (pins); // assigns the I/O lines to peripheral D function
	PIOA->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}

// ������ ������������ � periph C (Atmel specific)
void arm_hardware_piob_periphc(unsigned long pins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOB->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOB->PIO_ABCDSR [0] &= ~ (pins); // assigns the I/O lines to peripheral C function
	PIOB->PIO_ABCDSR [1] |= (pins); // assigns the I/O lines to peripheral C function
	PIOB->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}

// ������ ������������ � periph D (Atmel specific)
void arm_hardware_piob_periphd(unsigned long pins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOB->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOB->PIO_ABCDSR [0] |= (pins); // assigns the I/O lines to peripheral D function
	PIOB->PIO_ABCDSR [1] |= (pins); // assigns the I/O lines to peripheral D function
	PIOB->PIO_PDR = pins;	// PIO Disable Register - enable peripherial on this pin

#endif
}


#endif /* CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S */

// ������ ������������ � PIO (Atmel specific)
void arm_hardware_pioa_only(unsigned long pins)		
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PIOA->PIO_PER = pins;				// initial disconnect - PIO enable

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOA->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.

#endif
}

// ������ ������������ � PIO (Atmel specific)
void arm_hardware_piob_only(unsigned long pins)		
{
#if CPUSTYLE_AT91SAM7S

	//AT91C_BASE_PIOB->PIO_PER = pins;				// initial disconnect - PIO enable

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOB->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_pioa_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_piob_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ ������� speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF) 
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ ������� speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_pioc_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_piod_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_piod_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 2, 3);	/* ���������� CNF=2, MODE=3 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_pioa_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_pioa_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_piob_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_piob_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_pioc_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_pioc_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_piod_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#if defined (GPIOE)

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_pioe_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_pioe_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_pioe_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined (GPIOE) */


#if defined (GPIOF)

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_piof_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_piof_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_piof_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined (GPIOF) */

#if defined (GPIOG)

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_piog_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_piog_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_piog_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined (GPIOG) */

#if defined (GPIOH)


void 
arm_hardware_pioh_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	__DSB();
	GPIOE->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOH, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_pioh_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOH, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOH, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOH, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_pioh_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOH, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOH, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOH, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_pioh_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOH, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOH, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOH, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined (GPIOH) */

#if defined (GPIOI)

void 
arm_hardware_pioi_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	__DSB();
	GPIOE->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOI, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_pioi_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOI, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOI, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOI, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOI, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_pioi_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOI, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOI, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOI, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOI, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_pioi_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOI, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOI, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOI, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOI, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined (GPIOI) */

#if defined (GPIOJ)

void 
arm_hardware_pioj_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPJEN;	/* I/O port J clock enable */
	__DSB();
	GPIOE->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOJ, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_pioj_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPJEN;	/* I/O port J clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOJ, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOJ, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOJ, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOJ, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_pioj_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPJEN;	/* I/O port J clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOJ, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOJ, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOJ, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOJ, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_pioj_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPJEN;	/* I/O port J clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOJ, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOJ, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOJ, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOJEN;	/* I/O port J clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOJ, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined (GPIOJ) */

#if defined (GPIOK)

void 
arm_hardware_piok_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	__DSB();
	GPIOE->BSRR = BSRR_S(ipins);	/* ��� ��������� pull-up ��� CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOK, ipins, 2, 0);	/* ���������� CNF=2 � MODE=0 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, push-pull */
void 
arm_hardware_piok_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOK, opins, 2, 2);	/* ���������� CNF=2, MODE=2 (2MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOK, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOK, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOK, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 20 ���, push-pull */
void 
arm_hardware_piok_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOK, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOK, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOK, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOK, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 50 ���, push-pull */
void 
arm_hardware_piok_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOK, opins, 2, 3);	/* ���������� CNF=2, MODE=3 (20 MHz) ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOK, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOK, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOK, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#endif
}

#endif /* defined (GPIOK) */


/* ���������� � ���������, 2 ���, open-drain */
void arm_hardware_pioa_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 3, 2);	/* ���������� CNF=3, MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOA, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOA, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}
/* ���������� � ���������, 2 ���, open-drain */
void arm_hardware_piob_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 3, 2);	/* ���������� CNF=3, MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOB, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOB, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, open-drain */
void arm_hardware_pioc_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 3, 2);	/* ���������� CNF=3, MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOC, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOC, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, open-drain */
void arm_hardware_piod_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 3, 2);	/* ���������� CNF=3, MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIODEN;	/* I/O port D clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOD, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOD, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#endif
}

/* ���������� � ���������, 2 ���, open-drain */
void arm_hardware_pioe_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 3, 2);	/* ���������� CNF=3, MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOE, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOE, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */
#endif
}

#if defined (GPIOF)

/* ���������� � ���������, 2 ���, open-drain */
void arm_hardware_piof_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX && defined(RCC_APB2ENR_IOPFEN)

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 3, 2);	/* ���������� CNF=3, MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	__DSB();

	arm_stm32f30x_hardware_pio_altfn(GPIOF, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOF, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */
#endif
}
#endif /* defined (GPIOF) */

#if defined (GPIOG)
/* ���������� � ���������, 2 ���, open-drain */
void arm_hardware_piog_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	__DSB();
	// ��������� ������ �������
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 3, 2);	/* ���������� CNF=3, MODE=2 ��� ��������� ����� */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	__DSB();
	arm_stm32f30x_hardware_pio_altfn(GPIOG, opins, af);
	// ��������� ������ �������
	arm_stm32f30x_hardware_pio_prog(GPIOG, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	// �� ���� ����������� ����� GPIOG ���

#endif
}

#endif /* defined (GPIOG) */

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_pioa_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOA, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOA, up, down);
#endif
}

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_piob_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOB, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOB, up, down);
#endif
}

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_pioc_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOC, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOC, up, down);
#endif
}

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_piod_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOD, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOD, up, down);
#endif
}

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_pioe_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOE, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOE, up, down);
#endif
}

#if defined (GPIOF)

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_piof_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOF, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOF, up, down);
#endif
}

#endif /* defined (GPIOF) */

#if defined (GPIOG)

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_piog_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOG, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOG, up, down);
#endif
}

#endif /* defined (GPIOG) */

#if defined (GPIOH)

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_pioh_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOH, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOH, up, down);
#endif
}

#endif /* defined (GPIOH) */

#if defined (GPIOI)

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_pioi_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOI, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOI, up, down);
#endif
}

#endif /* defined (GPIOI) */

#if defined (GPIOJ)

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_pioj_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOJ, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOJ, up, down);
#endif
}

#endif /* defined (GPIOJ) */

#if defined (GPIOK)

/* ��������� ������������� ���������� � ������� (up) ��� � ����� (down). */
void 
arm_hardware_piok_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupdr(GPIOK, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupdr(GPIOK, up, down);
#endif
}

#endif /* defined (GPIOK) */

/* ���������� ������������� ����������. */
void 
arm_hardware_pioa_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOA, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOA, ipins);
#endif
}

/* ���������� ������������� ����������. */
void 
arm_hardware_piob_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOB, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOB, ipins);
#endif
}

/* ���������� ������������� ����������. */
void 
arm_hardware_pioc_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOC, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOC, ipins);
#endif
}

/* ���������� ������������� ����������. */
void 
arm_hardware_piod_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOD, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOD, ipins);
#endif
}

/* ���������� ������������� ����������. */
void 
arm_hardware_pioe_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOE, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOE, ipins);
#endif
}

#if defined (GPIOF)

/* ���������� ������������� ����������. */
void 
arm_hardware_piof_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOF, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOF, ipins);
#endif
}

#endif /* defined (GPIOF) */

#if defined (GPIOG)

/* ���������� ������������� ����������. */
void 
arm_hardware_piog_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOG, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOG, ipins);
#endif
}

#endif /* defined (GPIOG) */


#if defined (GPIOH)

/* ���������� ������������� ����������. */
void 
arm_hardware_pioh_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOH, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOH, ipins);
#endif
}

#endif /* defined (GPIOH) */

#if defined (GPIOI)

/* ���������� ������������� ����������. */
void 
arm_hardware_pioi_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOI, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOI, ipins);
#endif
}

#endif /* defined (GPIOI) */

#if defined (GPIOJ)

/* ���������� ������������� ����������. */
void 
arm_hardware_pioj_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOJ, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOJ, ipins);
#endif
}

#endif /* defined (GPIOJ) */

#if defined (GPIOK)

/* ���������� ������������� ����������. */
void 
arm_hardware_piok_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOK, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOK, ipins);
#endif
}

#endif /* defined (GPIOK) */


/* ��������� ���������� �� ��������� ��������� ��������� ����� ����� */
// ��� ������� �� ������ ���������������� ������� (�� ���� ��� �� �����),
// ������ ���������� ����������. ��������� ������ ���������� �� �������� ������� ������������� � ���������.
void 
arm_hardware_pioa_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PIOA->PIO_IFER = (ipins);	// glitch filter enable
	(void) AT91C_BASE_PIOA->PIO_ISR; // consume interrupt request
	AT91C_BASE_PIOA->PIO_IER = (ipins);	// interrupt on change pin enable

	// programming interrupts from PIOA
	AT91C_BASE_AIC->AIC_IDCR = (1UL << AT91C_ID_PIOA);
	AT91C_BASE_AIC->AIC_SVR [AT91C_ID_PIOA] = (AT91_REG) AT91F_PIOA_IRQHandler;
	AT91C_BASE_AIC->AIC_SMR [AT91C_ID_PIOA] = 
		(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL) |
		(AT91C_AIC_PRIOR & priority);
	AT91C_BASE_AIC->AIC_ICCR = (1UL << AT91C_ID_PIOA);		// clear pending interrupt
	AT91C_BASE_AIC->AIC_IECR = (1UL << AT91C_ID_PIOA);	// enable inerrupt

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOA->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOA->PIO_ISR; // consume interrupt request
	PIOA->PIO_IER = (ipins);	// interrupt on change pin enable

	NVIC_SetPriority(PIOA_IRQn, priority);
	NVIC_EnableIRQ(PIOA_IRQn);		// enable PIOA_Handler();


#elif CPUSTYLE_STM32F1XX

	arm_stm32f10x_hardware_pio_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PA, priority);	// PORT A

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	arm_stm32f30x_hardware_pio_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PA, priority);	// PORT A

#elif CPUSTYLE_STM32H7XX

	arm_stm32f30x_hardware_pio_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PA, priority);	// PORT A

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* ��������� ���������� �� ��������� ��������� ��������� ����� ����� */
// ��� ������� �� ������ ���������������� ������� (�� ���� ��� �� �����),
// ������ ���������� ����������. ��������� ������ ���������� �� �������� ������� ������������� � ���������.
void 
arm_hardware_piob_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOB->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOB->PIO_ISR; // consume interrupt request
	PIOB->PIO_IER = (ipins);	// interrupt on change pin enable

	NVIC_SetPriority(PIOB_IRQn, priority);
	NVIC_EnableIRQ(PIOB_IRQn);		// enable PIOB_Handler();


#elif CPUSTYLE_STM32F1XX

	arm_stm32f10x_hardware_pio_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PB, priority);	// PORT B

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	arm_stm32f30x_hardware_pio_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PB, priority);	// PORT B

#endif
}


/* ��������� ���������� �� ��������� ��������� ��������� ����� ����� */
// ��� ������� �� ������ ���������������� ������� (�� ���� ��� �� �����),
// ������ ���������� ����������. ��������� ������ ���������� �� �������� ������� ������������� � ���������.
void 
arm_hardware_pioc_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOC->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOC->PIO_ISR; // consume interrupt request
	PIOC->PIO_IER = (ipins);	// interrupt on change pin enable

	NVIC_SetPriority(PIOC_IRQn, priority);
	NVIC_EnableIRQ(PIOC_IRQn);		// enable PIOC_Handler();


#elif CPUSTYLE_STM32F1XX

	arm_stm32f10x_hardware_pio_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PC, priority);	// PORT C

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	arm_stm32f30x_hardware_pio_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PC, priority);	// PORT C

#endif
}

#if defined (GPIOD) || defined (PIOD)

// ��� ������� �� ������ ���������������� ������� (�� ���� ��� �� �����),
// ������ ���������� ����������. ��������� ������ ���������� �� �������� ������� ������������� � ���������.
void 
arm_hardware_piod_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOD->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOD->PIO_ISR; // consume interrupt request
	PIOD->PIO_IER = (ipins);	// interrupt on change pin enable

	NVIC_SetPriority(PIOD_IRQn, priority);
	NVIC_EnableIRQ(PIOD_IRQn);		// enable PIOE_Handler();


#elif CPUSTYLE_STM32F1XX

	arm_stm32f10x_hardware_pio_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PD, priority);	// PORT D

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	arm_stm32f30x_hardware_pio_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PD, priority);	// PORT D

#endif
}
#endif /* defined (GPIOD) || defined (PIOD) */

#if defined (GPIOE) || defined (PIOE)

// ��� ������� �� ������ ���������������� ������� (�� ���� ��� �� �����),
// ������ ���������� ����������. ��������� ������ ���������� �� �������� ������� ������������� � ���������.
void 
arm_hardware_pioe_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOE->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOE->PIO_ISR; // consume interrupt request
	PIOE->PIO_IER = (ipins);	// interrupt on change pin enable

	NVIC_SetPriority(PIOE_IRQn, priority);
	NVIC_EnableIRQ(PIOE_IRQn);		// enable PIOE_Handler();


#elif CPUSTYLE_STM32F1XX

	arm_stm32f10x_hardware_pio_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PE, priority);	// PORT E

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	arm_stm32f30x_hardware_pio_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PE, priority);	// PORT E

#endif
}
#endif /* defined (GPIOE) || defined (PIOE) */

#if defined (GPIOF) || defined (PIOF)

// ��� ������� �� ������ ���������������� ������� (�� ���� ��� �� �����),
// ������ ���������� ����������. ��������� ������ ���������� �� �������� ������� ������������� � ���������.
void 
arm_hardware_piof_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOF->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOF->PIO_ISR; // consume interrupt request
	PIOF->PIO_IER = (ipins);	// interrupt on change pin enable

	NVIC_SetPriority(PIOF_IRQn, priority);
	NVIC_EnableIRQ(PIOF_IRQn);		// enable PIOF_Handler();


#elif CPUSTYLE_STM32F1XX

	arm_stm32f10x_hardware_pio_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PF, priority);	// PORT F

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	arm_stm32f30x_hardware_pio_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PF, priority);	// PORT F

#endif
}
#endif /* defined (GPIOF) || defined (PIOF) */

#if defined (GPIOG)
// ��� ������� �� ������ ���������������� ������� (�� ���� ��� �� �����),
// ������ ���������� ����������. ��������� ������ ���������� �� �������� ������� ������������� � ���������.
void 
arm_hardware_piog_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOG->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOG->PIO_ISR; // consume interrupt request
	PIOG->PIO_IER = (ipins);	// interrupt on change pin enable

	NVIC_SetPriority(PIOG_IRQn, priority);
	NVIC_EnableIRQ(PIOG_IRQn);		// enable PIOG_Handler();


#elif CPUSTYLE_STM32F1XX

	arm_stm32f10x_hardware_pio_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PG, priority);	// PORT G

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	arm_stm32f30x_hardware_pio_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PG, priority);	// PORT G

#endif
}

#endif /* defined (GPIOG) */

#if defined (GPIOH)
// ��� ������� �� ������ ���������������� ������� (�� ���� ��� �� �����),
// ������ ���������� ����������. ��������� ������ ���������� �� �������� ������� ������������� � ���������.
void 
arm_hardware_pioh_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority)
{
#if CPUSTYLE_STM32F1XX

	arm_stm32f10x_hardware_pio_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PH, priority);	// PORT H

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	arm_stm32f30x_hardware_pio_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PH, priority);	// PORT H

#endif
}

#endif /* defined (GPIOH) */

#elif CPUSTYPE_TMS320F2833X

/* ��������� ���������� ��������� �����  � GPIOA TMS320F2833X */
#define tms320f2833x_hardware_pioa_setstate(opins, initialstate) \
  do { \
	GPASET = ((initialstate) & (opins)); \
	GPACLEAR = (~ (initialstate) & (opins)); \
	} while (0)
/* ��������� ���������� ��������� �����  � GPIOB TMS320F2833X */
#define tms320f2833x_hardware_piob_setstate(opins, initialstate) \
  do { \
	GPBSET = ((initialstate) & (opins)); \
	GPBCLEAR = (~ (initialstate) & (opins)); \
	} while (0)
/* ��������� ���������� ��������� �����  � GPIOC TMS320F2833X */
#define tms320f2833x_hardware_pioc_setstate(opins, initialstate) \
  do { \
	GPCSET = ((initialstate) & (opins)); \
	GPCCLEAR = (~ (initialstate) & (opins)); \
	} while (0)
/* ���������������� ����� � ��������� ���������� GPIOA, ��������� � iomask, mux, direction, pullupdisable */
#define tms320f2833x_hardware_pioa_prog(iomask, mux, direction, pullupdisable, qual) \
  do { \
	const portholder_t mask3lo = power2(iomask >> 0);	\
	const portholder_t mask3hi = power2(iomask >> 16);	\
	GPAMUX1 = (GPAMUX1 & ~ (mask3lo * 0x03UL)) | (mask3lo * (mux)); /* GPIOxx as IO use */ \
	GPAMUX2 = (GPAMUX2 & ~ (mask3hi * 0x03UL)) | (mask3hi * (mux)); /* GPIOxx as IO use */ \
	GPAQSEL1 = (GPAQSEL1 & ~ (mask3lo * 0x03UL)) | (mask3lo * (qual)); /* GPIOxx as IO use */ \
	GPAQSEL2 = (GPAQSEL2 & ~ (mask3hi * 0x03UL)) | (mask3hi * (qual)); /* GPIOxx as IO use */ \
	GPADIR = (GPADIR | (iomask * (direction)));	/* set direction to out */ \
	GPAPUD = (GPAPUD | (iomask * (pullupdisable))); /* disable the pull up */ \
  } while (0)
/* ���������������� ����� � ��������� ���������� GPIOB, ��������� � iomask, mux, direction, pullupdisable */
#define tms320f2833x_hardware_piob_prog(iomask, mux, direction, pullupdisable, qual) \
  do { \
	const portholder_t mask3lo = power2(iomask >> 0);	\
	const portholder_t mask3hi = power2(iomask >> 16);	\
	GPBMUX1 = (GPBMUX1 & ~ (mask3lo * 0x03UL)) | (mask3lo * (mux)); /* GPIOxx as IO use */ \
	GPBMUX2 = (GPBMUX2 & ~ (mask3hi * 0x03UL)) | (mask3hi * (mux)); /* GPIOxx as IO use */ \
	GPBQSEL1 = (GPBQSEL1 & ~ (mask3lo * 0x03UL)) | (mask3lo * (qual)); /* GPIOxx as IO use */ \
	GPBQSEL2 = (GPBQSEL2 & ~ (mask3hi * 0x03UL)) | (mask3hi * (qual)); /* GPIOxx as IO use */ \
	GPBDIR = (GPBDIR | (iomask * (direction)));	/* set direction to out */ \
	GPBPUD = (GPBPUD | (iomask * (pullupdisable))); /* disable the pull up */ \
  } while (0)
/* ���������������� ����� � ��������� ���������� GPIOC, ��������� � iomask, direction, pullupdisable */
#define tms320f2833x_hardware_pioc_prog(iomask, mux, direction, pullupdisable) \
  do { \
	const portholder_t mask3lo = power2(iomask >> 0);	\
	const portholder_t mask3hi = power2(iomask >> 16);	\
	GPCMUX1 = (GPCMUX1 & ~ (mask3lo * 0x03UL)) | (mask3lo * (mux)); /* GPIOxx as IO use */ \
	GPCMUX2 = (GPCMUX2 & ~ (mask3hi * 0x03UL)) | (mask3hi * (mux)); /* GPIOxx as IO use */ \
	GPCDIR = (GPCDIR | (iomask * (direction)));	/* set direction to out */ \
	GPCPUD = (GPCPUD | (iomask * (pullupdisable))); /* disable the pull up */ \
  } while (0)


/* ���������������� ������� �� �����, ��� ������������� � ��������� */
void
tms320_hardware_pioa_outputs(unsigned long opins, unsigned long initialstate)
{
	// ��������� ���������� ��������� �����
	tms320f2833x_hardware_pioa_setstate(opins, initialstate);
	// ��������� ������ �������
	tms320f2833x_hardware_pioa_prog(opins, 0, 1, 1, 0);	/* �� �����, ��� pull-up */
}
/* ���������������� ������� �� �����, ��� ������������� � ��������� */
void
tms320_hardware_piob_outputs(unsigned long opins, unsigned long initialstate)
{
	// ��������� ���������� ��������� �����
	tms320f2833x_hardware_piob_setstate(opins, initialstate);
	// ��������� ������ �������
	tms320f2833x_hardware_piob_prog(opins, 0, 1, 1, 0);	/* �� �����, ��� pull-up */
}
/* ���������������� ������� �� �����, ��� ������������� � ��������� */
void
tms320_hardware_pioc_outputs(unsigned long opins, unsigned long initialstate)
{
	// ��������� ���������� ��������� �����
	tms320f2833x_hardware_pioc_setstate(opins, initialstate);
	// ��������� ������ �������
	tms320f2833x_hardware_pioc_prog(opins, 0, 1, 1);	/* �� �����, ��� pull-up */
}
/* ���������������� ������� �� �����, ������������� � ��������� */
void
tms320_hardware_pioa_periph(unsigned long opins, uint_fast8_t mux)
{
	// ��������� ������ �������
	tms320f2833x_hardware_pioa_prog(opins, mux, 0, 0, 0);
}

/* ���������������� ������� �� �����, ������������� � ��������� */
void
tms320_hardware_piob_periph(unsigned long opins, uint_fast8_t mux)
{
	// ��������� ������ �������
	tms320f2833x_hardware_piob_prog(opins, mux, 0, 0, 0);
}
/* ���������������� ������� �� �����, ������������� � ��������� */
void
tms320_hardware_pioc_periph(unsigned long opins, uint_fast8_t mux)
{
	// ��������� ������ �������
	tms320f2833x_hardware_pioc_prog(opins, mux, 0, 0);
}

/* ���������������� ������� �� ����, ��� ������������� � ��������� */
void
tms320_hardware_pioa_inputs(unsigned long ipins)
{
	// ��������� ������ �������
	tms320f2833x_hardware_pioa_prog(ipins, 0, 0, 0, 0);	/* �� ����, � pull-up */
}
/* ���������������� ������� �� ����, ��� ������������� � ��������� */
void
tms320_hardware_piob_inputs(unsigned long ipins)
{
	// ��������� ������ �������
	tms320f2833x_hardware_piob_prog(ipins, 0, 0, 0, 0);	/* �� ����, � pull-up */
}
/* ���������������� ������� �� ����, ��� ������������� � ��������� */
void
tms320_hardware_pio�_inputs(unsigned long ipins)
{
	// ��������� ������ �������
	tms320f2833x_hardware_pioc_prog(ipins, 0, 0, 0);	/* �� ����, � pull-up */
}

#endif /* CPUSTYLE_ARM */
