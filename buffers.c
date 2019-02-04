/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */
#include "list.h"
#include "formats.h"	// for debug prints

//#define WITHBUFFERSDEBUG WITHDEBUG

#if WITHINTEGRATEDDSP

#include "audio.h"
#include "usb200.h"
#include "usbch9.h"

#include <string.h>		// for memset

#if defined(WITHRTS96) && defined(WITHRTS192)
	#error Configuration Error: WITHRTS96 and WITHRTS192 can not be used together
#endif /* defined(WITHRTS96) && defined(WITHRTS192) */
enum
{
	BUFFTAG_VOICE16 = 44,
	BUFFTAG_RTS192,
	BUFFTAG_RTS96,
	BUFFTAG_total
};

	/* ���������� �������� ��� ������� ������ �� ������� - ��������� �������� ������ ������� */
	typedef struct listcnt
	{
		LIST_ENTRY item;
		uint_fast8_t Count;	// ���������� �������� � ������
	} LIST_ENTRY2, * PLIST_ENTRY2;

	#define LIST2PRINT(name) do { \
			debug_printf_P(PSTR(# name "[%3d] "), (int) name.Count); \
		} while (0)

	__STATIC_INLINE int
	IsListEmpty2(PLIST_ENTRY2 ListHead)
	{
		return (ListHead)->Count == 0;
		//return IsListEmpty(& (ListHead)->item);
	}

	__STATIC_INLINE void
	InitializeListHead2(LIST_ENTRY2 * ListHead)
	{
		(ListHead)->Count = 0;
		InitializeListHead(& (ListHead)->item);
	}

	__STATIC_INLINE void
	InsertHeadList2(PLIST_ENTRY2 ListHead, PLIST_ENTRY Entry)
	{
		(ListHead)->Count += 1;
		InsertHeadList(& (ListHead)->item, (Entry));
	}

	__STATIC_INLINE PLIST_ENTRY
	RemoveTailList2(PLIST_ENTRY2 ListHead)
	{
		(ListHead)->Count -= 1;
		const PLIST_ENTRY t = RemoveTailList(& (ListHead)->item);	/* ����� ������� �������� RemoveTailList ������ - Microsoft ������ �� ������ ���������� ������. �� �� ������� � �� ��������� �� � ����������� ����� C. */
		return t;
	}

	/* ���������� ������� � "������������". */
	__STATIC_INLINE uint_fast8_t fiforeadyupdate(
		uint_fast8_t ready,		// ������� ��������� ����������
		uint_fast8_t Count,		// ������� ��������� ������ � �������
		uint_fast8_t normal		// ������� ��������� ����������
		)
	{
		return ready ? Count != 0 : Count >= normal;
	}

	static volatile uint_fast8_t uacoutplayer = 0;	/* ����� ������������� ������ ���������� � ��������� ���������� - ���������� ����� */
	static volatile uint_fast8_t uacoutmike = 0;	/* �� ���� ���������� ������� ����������� � USB ����������� �����, � �� � ��������� */
	static volatile uint_fast8_t uacinalt = UACINALT_NONE;		/* ����� �������������� ������������ ��� UAC IN interface */
	static volatile uint_fast8_t uacinrtsalt = UACINRTSALT_NONE;		/* ����� �������������� ������������ ��� RTS UAC IN interface */
	static volatile uint_fast8_t uacoutalt;
	//////////////////////////////////
	// ������� ����������� �����������
	//
	typedef ALIGNX_BEGIN struct voice16
	{
		LIST_ENTRY item;	// layout should be same in voice16_t, voice96rts_t and voice192rts_t
		uint_fast8_t tag;	// layout should be same in voice16_t, voice96rts_t and voice192rts_t
		ALIGNX_BEGIN uint16_t buff [DMABUFFSIZE16] ALIGNX_END;
	} ALIGNX_END voice16_t;

	typedef ALIGNX_BEGIN struct voices32tx
	{
		LIST_ENTRY item;
		ALIGNX_BEGIN uint32_t buff [DMABUFFSIZE32TX] ALIGNX_END;
	} ALIGNX_END voice32tx_t;

	typedef ALIGNX_BEGIN struct voices32rx
	{
		LIST_ENTRY item;
		ALIGNX_BEGIN uint32_t buff [DMABUFFSIZE32RX] ALIGNX_END;
	} ALIGNX_END voice32rx_t;

	static LIST_ENTRY2 voicesfree16;
	static LIST_ENTRY2 voicesmike16;	// ������ � ������������� ������� � ���������/Line in
	static const uint_fast8_t VOICESMIKE16NORMAL = 5;	// ���������� ���������� ������� � �������
	static uint_fast8_t voicesmike16ready = 0;	// ��������� �������� ������������

	static LIST_ENTRY2 voicesphones16;	// ������, ��������������� ��� ������ �� ��������
	
	static LIST_ENTRY2 voicesready32tx;	// ������, ��������������� ��� ������ �� IF DAC
	static LIST_ENTRY2 voicesfree32tx;
	static LIST_ENTRY2 voicesfree32rx;

#if WITHUSBUAC

	#if WITHRTS192

		typedef ALIGNX_BEGIN struct voices192rts
		{
			LIST_ENTRY item;	// layout should be same in voice16_t, voice96rts_t and voice192rts_t
			uint_fast8_t tag;	// layout should be same in voice16_t, voice96rts_t and voice192rts_t
			ALIGNX_BEGIN uint8_t buff [DMABUFFSIZE192RTS] ALIGNX_END;		// ������, 2*24*192 kS/S
		} ALIGNX_END voice192rts_t;

		static LIST_ENTRY2 voicesfree192rts;
		static LIST_ENTRY2 uacin192rts;	// ������ ��� ������ � ���������� USB � ���������� �������, 2*32*192 kS/S
	
	#endif /* WITHRTS192 */

	#if WITHRTS96

		typedef ALIGNX_BEGIN struct voices96rts
		{
			LIST_ENTRY item;	// layout should be same in voice16_t, voice96rts_t and voice192rts_t
			uint_fast8_t tag;	// layout should be same in voice16_t, voice96rts_t and voice192rts_t
			ALIGNX_BEGIN uint8_t buff [DMABUFFSIZE96RTS] ALIGNX_END;		// ������, 2*24*192 kS/S
		} ALIGNX_END voice96rts_t;

		static LIST_ENTRY2 voicesfree96rts;
		static LIST_ENTRY2 uacin96rts;	// ������ ��� ������ � ���������� USB � ��������� �������, 2*32*192 kS/S
	
	#endif /* WITHRTS96 */

	static LIST_ENTRY2 uacin16;		// ������ ��� ������ � ���������� USB � ��������� 2*16*24 kS/S
	static LIST_ENTRY2 uacout16;		// ������ �� USB ��� �������������
	//static const uint_fast8_t VOICESMIKE16NORMAL = VOICESMIKE16NORMAL;	// ���������� ���������� ������� � �������
	static uint_fast8_t uacout16ready = 0;	// ��������� �������� ������������

#endif /* WITHUSBUAC */

#endif /* WITHINTEGRATEDDSP */

#if WITHUSEAUDIOREC

	#define SDCARDBUFFSIZE16 (2048)	// ������ ������ ������ ���� �� ������ ������� �������� �� SD �����

	typedef ALIGNX_BEGIN struct records16
	{
		LIST_ENTRY item;
		ALIGNX_BEGIN int16_t buff [SDCARDBUFFSIZE16] ALIGNX_END;
	} ALIGNX_END records16_t;

	static LIST_ENTRY2 recordsfree16;		// ��������� ������
	static LIST_ENTRY2 recordsready16;	// ������ ��� ������ �� SD CARD

	static volatile unsigned recdropped;
	static volatile unsigned recbuffered;

#endif /* WITHUSEAUDIOREC */

#if WITHMODEM


typedef struct modems8
{
	LIST_ENTRY item;
	size_t length;
	uint8_t buff [MODEMBUFFERSIZE8];
} modems8_t;

static LIST_ENTRY2 modemsfree8;		// ��������� ������
static LIST_ENTRY2 modemsrx8;	// ������ � ���������� ����� ����� �������
//static LIST_ENTRY modemstx8;	// ������ � ������� ��� �������� ����� �����

#endif /* WITHMODEM */

/* C�������� �� ������ ������������ ���������� � user-level ��������. */

typedef struct message
{
	LIST_ENTRY item;
	uint8_t type;
	uint8_t data [MSGBUFFERSIZE8];
} message_t;

static LIST_ENTRY msgsfree8;		// ��������� ������
static LIST_ENTRY msgsready8;		// ����������� - ������� � ���������

#if WITHBUFFERSDEBUG
	static volatile unsigned n1, n1wfm, n2, n3, n4, n5, n6;
	static volatile unsigned e1, e2, e3, e4, e5, e6;
	static volatile unsigned nbadd, nbdel, nbzero;

	static volatile unsigned debugcount_ms10;	// � ��������� 0.1 ms

	static volatile unsigned debugcount_uacout;
	static volatile unsigned debugcount_mikeadc;
	static volatile unsigned debugcount_phonesdac;
	static volatile unsigned debugcount_rtsadc;
	static volatile unsigned debugcount_uacin;
	static volatile unsigned debugcount_rx32adc;
	static volatile unsigned debugcount_rx32wfm;
	static volatile unsigned debugcount_tx32dac;
	
#endif /* WITHBUFFERSDEBUG */

/* ���������� �� ����������� ���������� ���������� */
void buffers_spool(void)
{
#if WITHBUFFERSDEBUG
	debugcount_ms10 += 10000 / TICKS_FREQUENCY;
#endif /* WITHBUFFERSDEBUG */
}

#if WITHBUFFERSDEBUG

static unsigned 
getresetval(volatile unsigned * p)
{
	unsigned v;
	global_disableIRQ();
	v = * p;
	* p = 0;
	global_enableIRQ();
	return v;
}

#endif /* WITHBUFFERSDEBUG */

void buffers_diagnostics(void)
{
#if 1 && WITHDEBUG && WITHINTEGRATEDDSP && WITHBUFFERSDEBUG

	/* Normal processing (with DIGI mode and RTS):
	voicesfree16[  9] voicesmike16[  1] voicesphones16[  1] voicesfree96rts[  5] uacin96rts[  0] uacin16[  0] uacout16[  5]
	voicesfree16[ 10] voicesmike16[  0] voicesphones16[  1] voicesfree96rts[  6] uacin96rts[  0] uacin16[  0] uacout16[  6]
	voicesfree16[  9] voicesmike16[  0] voicesphones16[  1] voicesfree96rts[  6] uacin96rts[  0] uacin16[  0] uacout16[  6]
	*/

	LIST2PRINT(voicesfree16);
	LIST2PRINT(voicesmike16);
	LIST2PRINT(voicesphones16);

	#if WITHUSBUAC
		#if WITHRTS192
			LIST2PRINT(voicesfree192rts);
			LIST2PRINT(uacin192rts);
		#elif WITHRTS96
			LIST2PRINT(voicesfree96rts);
			LIST2PRINT(uacin96rts);
		#endif
		LIST2PRINT(uacin16);
		LIST2PRINT(uacout16);
	#endif /* WITHUSBUAC */

	debug_printf_P(PSTR(" add=%u, del=%u, zero=%u, "), nbadd, nbdel, nbzero);
		
	debug_printf_P(PSTR("\n"));

#endif

#if 1 && WITHDEBUG && WITHINTEGRATEDDSP && WITHBUFFERSDEBUG
	debug_printf_P(PSTR("n1=%u n1wfm=%u n2=%u n3=%u n4=%u n5=%u n6=%u\n"), n1, n1wfm, n2, n3, n4, n5, n6);
	debug_printf_P(PSTR("e1=%u e2=%u e3=%u e4=%u e5=%u e6=%u uacinalt=%d\n"), e1, e2, e3, e4, e5, e6, uacinalt);

	{
		const unsigned ms10 = getresetval(& debugcount_ms10);
		const unsigned uacout = getresetval(& debugcount_uacout);
		const unsigned mikeadc = getresetval(& debugcount_mikeadc);
		const unsigned phonesdac = getresetval(& debugcount_phonesdac);
		const unsigned rtsadc = getresetval(& debugcount_rtsadc);
		const unsigned rx32adc = getresetval(& debugcount_rx32adc);
		const unsigned rx32wfm = getresetval(& debugcount_rx32wfm);
		const unsigned tx32dac = getresetval(& debugcount_tx32dac);
		const unsigned uacin = getresetval(& debugcount_uacin);

		debug_printf_P(PSTR("uacout=%u, uacin=%u, mikeadc=%u, phonesdac=%u, rtsadc=%u rx32adc=%u rx32wfm=%u tx32dac=%u\n"), 
			uacout * 10000 / ms10, 
			uacin * 10000 / ms10, 
			mikeadc * 10000 / ms10, 
			phonesdac * 10000 / ms10, 
			rtsadc * 10000 / ms10, 
			rx32adc * 10000 / ms10, 
			rx32wfm * 10000 / ms10, 
			tx32dac * 10000 / ms10
			);
	}
#endif
}



#if 0 && WITHBUFFERSDEBUG

	typedef struct
	{
		uint8_t lock;
		int line;
		const char * file;
	} LOCK_T;

	static volatile LOCK_T locklist16;
	static volatile LOCK_T locklist16ststem;
	static volatile LOCK_T locklist32;
	static volatile LOCK_T locklist8;

	static void lock_impl(volatile LOCK_T * p, int line, const char * file, const char * variable)
	{
	#if WITHHARDINTERLOCK
		uint8_t r;
		do
			r = __LDREXB(& p->lock);
		while (__STREXB(1, & p->lock));
		if (r != 0)
		{
			debug_printf_P(PSTR("LOCK @%p %s already locked at %d in %s by %d in %s\n"), p, variable, line, file, p->line, p->file);
			for (;;)
				;
		}
		else
		{
			p->file = file;
			p->line = line;
		}
	#endif /* WITHHARDINTERLOCK */
	}

	static void unlock_impl(volatile LOCK_T * p, int line, const char * file, const char * variable)
	{
	#if WITHHARDINTERLOCK
		uint8_t r;
		do
			r = __LDREXB(& p->lock);
		while (__STREXB(0, & p->lock));
		if (r == 0)
		{
			debug_printf_P(PSTR("LOCK @%p %s already unlocked at %d in %s by %d in %s\n"), p, variable, line, file, p->line, p->file);
			for (;;)
				;
		}
		else
		{
			p->file = file;
			p->line = line;
		}
	#endif /* WITHHARDINTERLOCK */
	}

	#define LOCK(p) do { lock_impl((p), __LINE__, __FILE__, # p); } while (0)
	#define UNLOCK(p) do { unlock_impl((p), __LINE__, __FILE__, # p); } while (0)

#else /* WITHBUFFERSDEBUG */

	#define LOCK(p) do {  } while (0)
	#define UNLOCK(p) do {  } while (0)

#endif /* WITHBUFFERSDEBUG */

// ������������� ������� �������
void buffers_initialize(void)
{
	unsigned i;

#if WITHINTEGRATEDDSP

	static voice16_t voicesarray16 [24];

	InitializeListHead2(& voicesphones16);	// ������ ��� ������ �� ���
	InitializeListHead2(& voicesmike16);	// ������ ������������ � ���
	InitializeListHead2(& voicesfree16);	// �������������
	for (i = 0; i < (sizeof voicesarray16 / sizeof voicesarray16 [0]); ++ i)
	{
		voice16_t * const p = & voicesarray16 [i];
		p->tag = BUFFTAG_VOICE16;
		InsertHeadList2(& voicesfree16, & p->item);
	}

#if WITHUSBUAC

	//ASSERT((DMABUFFSIZE16 % HARDWARE_RTSDMABYTES) == 0);
	ASSERT((DMABUFFSIZE192RTS % HARDWARE_RTSDMABYTES) == 0);
	ASSERT((DMABUFFSIZE96RTS % HARDWARE_RTSDMABYTES) == 0);

	#if WITHRTS192

		RAMNOINIT_D1 static voice192rts_t voicesarray192rts [4];

		ASSERT(offsetof(voice16_t, item) == offsetof(voice192rts_t, item));
		ASSERT(offsetof(voice16_t, buff) == offsetof(voice192rts_t, buff));
		ASSERT(offsetof(voice16_t, tag) == offsetof(voice192rts_t, tag));

		InitializeListHead2(& uacin192rts);		// ������ ��� ������ � ����� USB AUDIO - ������
		InitializeListHead2(& voicesfree192rts);	// �������������
		for (i = 0; i < (sizeof voicesarray192rts / sizeof voicesarray192rts [0]); ++ i)
		{
			voice192rts_t * const p = & voicesarray192rts [i];
			p->tag = BUFFTAG_RTS192;
			InsertHeadList2(& voicesfree192rts, & p->item);
		}

	#endif /* WITHRTS192 */

	#if WITHRTS96

		RAMNOINIT_D1 static voice96rts_t voicesarray96rts [4];

		ASSERT(offsetof(voice16_t, item) == offsetof(voice96rts_t, item));
		ASSERT(offsetof(voice16_t, buff) == offsetof(voice96rts_t, buff));
		ASSERT(offsetof(voice16_t, tag) == offsetof(voice96rts_t, tag));

		InitializeListHead2(& uacin96rts);		// ������ ��� ������ � ����� USB AUDIO - ������
		InitializeListHead2(& voicesfree96rts);	// �������������
		for (i = 0; i < (sizeof voicesarray96rts / sizeof voicesarray96rts [0]); ++ i)
		{
			voice96rts_t * const p = & voicesarray96rts [i];
			p->tag = BUFFTAG_RTS96;
			InsertHeadList2(& voicesfree96rts, & p->item);
		}

	#endif /* WITHRTS192 */

	InitializeListHead2(& uacin16);	// ������ ��� ������ � ����� USB AUDIO
	InitializeListHead2(& uacout16);	// ������ �� USB ��� �������������

#endif /* WITHUSBUAC */

	static voice32tx_t voicesarray32tx [6];

	InitializeListHead2(& voicesready32tx);	// ������ ��� ������ �� ���
	InitializeListHead2(& voicesfree32tx);	// �������������
	for (i = 0; i < (sizeof voicesarray32tx / sizeof voicesarray32tx [0]); ++ i)
	{
		voice32tx_t * const p = & voicesarray32tx [i];
		InsertHeadList2(& voicesfree32tx, & p->item);
	}

	static voice32rx_t voicesarray32rx [6];	// ��� WFM ���� 2

	InitializeListHead2(& voicesfree32rx);	// �������������
	for (i = 0; i < (sizeof voicesarray32rx / sizeof voicesarray32rx [0]); ++ i)
	{
		voice32rx_t * const p = & voicesarray32rx [i];
		InsertHeadList2(& voicesfree32rx, & p->item);
	}

#if WITHUSEAUDIOREC

	#if CPUSTYLE_R7S721
		RAMNOINIT_D1 static records16_t recordsarray16 [16];
	#elif defined (STM32F767xx)
		RAMNOINIT_D1 static records16_t recordsarray16 [16];
	#elif defined (STM32F746xx)
		RAMNOINIT_D1 static records16_t recordsarray16 [16];
	#elif defined (STM32F429xx)
		RAMNOINIT_D1 static records16_t recordsarray16 [16];
	#elif defined (STM32H743xx)
		RAMNOINIT_D1 static records16_t recordsarray16 [28];
	#else
		RAMNOINIT_D1 static records16_t recordsarray16 [16];
	#endif

	/* ���������� ������� ��� ������ �� SD CARD */
	InitializeListHead2(& recordsready16);	// ����������� - ������� ��� ������ �� SD CARD
	InitializeListHead2(& recordsfree16);	// �������������
	for (i = 0; i < (sizeof recordsarray16 / sizeof recordsarray16 [0]); ++ i)
	{
		records16_t * const p = & recordsarray16 [i];
		InsertHeadList2(& recordsfree16, & p->item);
	}

#endif /* WITHUSEAUDIOREC */

#if WITHMODEM
	static modems8_t modemsarray8 [8];

	/* ���������� ������� ��� ������ � ������� */
	InitializeListHead2(& modemsrx8);	// ����������� - �������� � ������
	//InitializeListHead2(& modemstx8);	// ����������� - ������� ��� �������� ����� �����
	InitializeListHead2(& modemsfree8);	// �������������
	for (i = 0; i < (sizeof modemsarray8 / sizeof modemsarray8 [0]); ++ i)
	{
		modems8_t * const p = & modemsarray8 [i];
		//InitializeListHead2(& p->item);
		InsertHeadList2(& modemsfree8, & p->item);
	}
#endif /* WITHMODEM */

#endif /* WITHINTEGRATEDDSP */

	/* C�������� �� ������ ������������ ���������� � user-level ��������. */
	static message_t messagesarray8 [12];

	/* ���������� ������� ��� ������ � ������� */
	InitializeListHead(& msgsready8);	// ����������� - ������� � ���������
	InitializeListHead(& msgsfree8);	// �������������
	for (i = 0; i < (sizeof messagesarray8 / sizeof messagesarray8 [0]); ++ i)
	{
		message_t * const p = & messagesarray8 [i];
		//InitializeListHead2(& p->item);
		InsertHeadList(& msgsfree8, & p->item);
	}
}

/* C�������� �� ������ ������������ ���������� � user-level ��������. */

// ������ � ���������� �� ������������ ���������� �����������
uint_fast8_t takemsgready_user(uint8_t * * dest)
{
	ASSERT_IRQL_USER();
	global_disableIRQ();
	LOCK(& locklist8);
	if (! IsListEmpty(& msgsready8))
	{
		PLIST_ENTRY t = RemoveTailList(& msgsready8);
		UNLOCK(& locklist8);
		global_enableIRQ();
		message_t * const p = CONTAINING_RECORD(t, message_t, item);
		* dest = p->data;
		ASSERT(p->type != MSGT_EMPTY);
		return p->type;
	}
	UNLOCK(& locklist8);
	global_enableIRQ();
	return MSGT_EMPTY;
}


// ������������ ������������� ������ ���������
void releasemsgbuffer_user(uint8_t * dest)
{
	ASSERT_IRQL_USER();
	message_t * const p = CONTAINING_RECORD(dest, message_t, data);
	global_disableIRQ();
	LOCK(& locklist8);
	InsertHeadList(& msgsfree8, & p->item);
	UNLOCK(& locklist8);
	global_enableIRQ();
}

// ����� ��� ������������ ���������
size_t takemsgbufferfree_low(uint8_t * * dest)
{
	ASSERT_IRQL_SYSTEM();
	LOCK(& locklist8);
	if (! IsListEmpty(& msgsfree8))
	{
		PLIST_ENTRY t = RemoveTailList(& msgsfree8);
		UNLOCK(& locklist8);
		message_t * const p = CONTAINING_RECORD(t, message_t, item);
		* dest = p->data;
		return (MSGBUFFERSIZE8 * sizeof p->data [0]);
	}
	UNLOCK(& locklist8);
	return 0;
}

// ��������� ��������� � ������� � ���������� 
void placesemsgbuffer_low(uint_fast8_t type, uint8_t * dest)
{
	ASSERT_IRQL_SYSTEM();
	ASSERT(type != MSGT_EMPTY);
	message_t * p = CONTAINING_RECORD(dest, message_t, data);
	p->type = type;
	LOCK(& locklist8);
	InsertHeadList(& msgsready8, & p->item);
	UNLOCK(& locklist8);
}


#if WITHINTEGRATEDDSP


// ��������� ���� �� �������� ����� ����������
static RAMFUNC void buffers_savetophones(voice16_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& voicesphones16, & p->item);
	UNLOCK(& locklist16);
}

// ��������� ���� � ������...
static RAMFUNC void buffers_savetonull(voice16_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& voicesfree16, & p->item);
	UNLOCK(& locklist16);
}

// ��������� ���� �� ���� �����������
static RAMFUNC void buffers_savtomodulators(voice16_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& voicesmike16, & p->item);
	voicesmike16ready = fiforeadyupdate(voicesmike16ready, voicesmike16.Count, VOICESMIKE16NORMAL);
	UNLOCK(& locklist16);
}

// ��������� ���� ����� ��������� �� ���� ���������� ��� ����������
static RAMFUNC void buffers_aftermodulators(voice16_t * p)
{
	// ���� ����� ������������ � ��� �������� ��������������� ��� ���������� � ��� ���������,
	// � �������� ����� ��������� ����� ����������

	if (uacoutplayer && uacoutmike)
		buffers_savetophones(p);
	else
		buffers_savetonull(p);
}

//////////////////////////////////////////
// ������������ ������ ������ AF ADC

// 16 bit, signed
// � ���� ��������, ������������ ������ ��������, vi �������� �������� �� ���������. vq ���������������� ��� ������ ISB (��� ����������� �������)
// ��� ���������� ������ � ������� - ���������� 0
RAMFUNC uint_fast8_t getsampmlemike(INT32P_t * v)
{
	static voice16_t * p = NULL;
	static unsigned pos = 0;	// ������� �� ��������� ����������
	const unsigned CNT = (DMABUFFSIZE16 / DMABUFSTEP16);	// ������������� ����� ������� �� ������� ������

	LOCK(& locklist16);
	if (p == NULL)
	{
		if (voicesmike16ready /*&& ! IsListEmpty2(& voicesmike16) */)
		{
			PLIST_ENTRY t = RemoveTailList2(& voicesmike16);
			voicesmike16ready = fiforeadyupdate(voicesmike16ready, voicesmike16.Count, VOICESMIKE16NORMAL);
			p = CONTAINING_RECORD(t, voice16_t, item);
			UNLOCK(& locklist16);
			pos = 0;
		}
		else
		{
			// ����������� ����� ��� �� ����� ������ �������� - ���������� 0.
			UNLOCK(& locklist16);
			return 0;
		}
	}
	else
	{
		UNLOCK(& locklist16);
	}

	// ������������� ������.
	v->ivqv [0] = (int16_t) p->buff [pos * DMABUFSTEP16 + 0];	// �������� ��� ����� �����
	v->ivqv [1] = (int16_t) p->buff [pos * DMABUFSTEP16 + 1];	// ������ �����

	if (++ pos >= CNT)
	{
		buffers_aftermodulators(p);
		p = NULL;
	}
	return 1;	
}

#if WITHUSBUAC

// ��������� ���� �� ������������� ��������� - USB - ��� ������������ �����������
RAMFUNC static void buffers_savetoresampling(voice16_t * p)
{
	LOCK(& locklist16);
	// ���������� � ������� �������� � USB UAC
	InsertHeadList2(& uacout16, & p->item);
	uacout16ready = fiforeadyupdate(uacout16ready, uacout16.Count, VOICESMIKE16NORMAL);
	if (uacout16.Count > (VOICESMIKE16NORMAL * 2))
	{
		// ��-�� ������ � ����������� ����� �������� �������� ����������� �� ������ ���� �������
		const PLIST_ENTRY t = RemoveTailList2(& uacout16);
		InsertHeadList2(& voicesfree16, t);
		uacout16ready = fiforeadyupdate(uacout16ready, uacout16.Count, VOICESMIKE16NORMAL);

	#if WITHBUFFERSDEBUG
		++ e6;
	#endif /* WITHBUFFERSDEBUG */
	}
	UNLOCK(& locklist16);
}

static uint_fast8_t isaudio48(void)
{
#if WITHUSBHW && WITHUSBUAC
	return uacinalt == UACINALT_AUDIO48;
#else /* WITHUSBHW && WITHUSBUAC */
	return 0;
#endif /* WITHUSBHW && WITHUSBUAC */
}

// UAC IN samplerate
// todo: ������� ���������� ������ ��� ��������� �������� ARMI2SRATE
int_fast32_t dsp_get_samplerateuacin_audio48(void)
{
	return dsp_get_sampleraterx();
}
// UAC IN samplerate
// todo: ������� ���������� ������ ��� ��������� �������� ARMI2SRATE
int_fast32_t dsp_get_samplerateuacin_rts96(void)
{
	return dsp_get_sampleraterxscaled(2);
}

// UAC IN samplerate
// todo: ������� ���������� ������ ��� ��������� �������� ARMI2SRATE
int_fast32_t dsp_get_samplerateuacin_rts192(void)
{
	return dsp_get_sampleraterxscaled(4);
}

int_fast32_t dsp_get_samplerateuacin_rts(void)		// RTS samplerate
{
#if WITHRTS192
	return dsp_get_samplerateuacin_rts192();
#elif WITHRTS96
	return dsp_get_samplerateuacin_rts96();
#else
	return dsp_get_samplerateuacin_audio48();
#endif
}

// UAC OUT samplerate
int_fast32_t dsp_get_samplerateuacout(void)
{
	return dsp_get_sampleratetx();
}

#if WITHRTS192 && WITHUSBHW && WITHUSBUAC

static uint_fast8_t isrts192(void)
{
#if WITHUSBHW && WITHUSBUAC
	#if WITHUSBUAC3 && WITHRTS192
		return uacinrtsalt == UACINRTSALT_RTS192;
	#elif WITHRTS192
		return uacinalt == UACINALT_RTS192;
	#else /* WITHUSBUAC3 */
		return 0;
	#endif /* WITHUSBUAC3 */
#else /* WITHUSBHW && WITHUSBUAC */
	return 0;
#endif /* WITHUSBHW && WITHUSBUAC */
}

// ��������� ����� ������� ��� �������� � ���������
static RAMFUNC void
buffers_savetouacin192rts(voice192rts_t * p)
{
#if WITHBUFFERSDEBUG
	// ������� �������� � ������� �� �������
	debugcount_rtsadc += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEP192RTS;	// � ������ ���� ������� �� ������ �����
#endif /* WITHBUFFERSDEBUG */

	LOCK(& locklist16);
	InsertHeadList2(& uacin192rts, & p->item);
	UNLOCK(& locklist16);

	refreshDMA_uacin();		// ���� DMA  ����������� - ������ �����
}

static void buffers_savetonull192rts(voice192rts_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& voicesfree192rts, & p->item);
	UNLOCK(& locklist16);
}


#endif /* WITHRTS192 && WITHUSBHW && WITHUSBUAC */

#if WITHRTS96 && WITHUSBHW && WITHUSBUAC

static uint_fast8_t isrts96(void)
{
#if WITHUSBHW && WITHUSBUAC
	#if WITHUSBUAC3 && WITHRTS96
		return uacinrtsalt == UACINRTSALT_RTS96;
	#elif WITHRTS96
		return uacinalt == UACINALT_RTS96;
	#else /* WITHUSBUAC3 */
		return 0;
	#endif /* WITHUSBUAC3 */
#else /* WITHUSBHW && WITHUSBUAC */
	return 0;
#endif /* WITHUSBHW && WITHUSBUAC */
}

// ��������� ����� ������� ��� �������� � ���������
static RAMFUNC void
buffers_savetouacin96rts(voice96rts_t * p)
{
#if WITHBUFFERSDEBUG
	// ������� �������� � ������� �� �������
	debugcount_rtsadc += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEP96RTS;	// � ������ ���� ������� �� ��� �����
#endif /* WITHBUFFERSDEBUG */
	
	LOCK(& locklist16);
	InsertHeadList2(& uacin96rts, & p->item);
	UNLOCK(& locklist16);

	refreshDMA_uacin();		// ���� DMA  ����������� - ������ �����
}

static void buffers_savetonull96rts(voice96rts_t * p)
{
	LOCK(& locklist16);
	InsertHeadList2(& voicesfree96rts, & p->item);
	UNLOCK(& locklist16);
}

#endif /* WITHRTS96 && WITHUSBHW && WITHUSBUAC */

// ��������� ����� ������� ��� �������� � ���������
static RAMFUNC void
buffers_savetouacin(voice16_t * p)
{
#if WITHBUFFERSDEBUG
	// ������� �������� � ������� �� �������
	debugcount_uacin += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEP16;	// � ������ ���� ������� �� ��� �����
#endif /* WITHBUFFERSDEBUG */
	LOCK(& locklist16);
	InsertHeadList2(& uacin16, & p->item);
	UNLOCK(& locklist16);

	refreshDMA_uacin();		// ���� DMA  ����������� - ������ �����
}

#else

int_fast32_t dsp_get_samplerateuacin_rts(void)		// RTS samplerate
{
	return 48000L;
}

#endif /* WITHUSBUAC */

#if WITHI2SHW

// �������� ������ ����� (� "�������")
static RAMFUNC voice16_t *
buffers_getsilence(void)
{
	const uintptr_t addr = allocate_dmabuffer16();
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	memset(p->buff, 0, sizeof p->buff);	// ���������� "�������"
	return p;
}

// �������� ������ ����� (� "�������")
static RAMFUNC uint32_t getfilled_dmabuffer16silence(void)
{
	return (uint32_t) buffers_getsilence()->buff;
}


// +++ ���������� ������� �����������
// ������ ����� ������ �������� - ��� �������������
static RAMFUNC void
buffers_savefromrxout(voice16_t * p)
{
	if (uacoutplayer != 0)
		buffers_savetonull(p);
	else
		buffers_savetophones(p);
}
#endif /* WITHI2SHW */

// ������� ������ �� USB AUDIO
static RAMFUNC void
buffers_savefromuacout(voice16_t * p)
{
#if WITHBUFFERSDEBUG
	// ������� �������� � ������� �� �������
	debugcount_uacout += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEP16;	// � ������ ���� ������� �� ��� �����
#endif /* WITHBUFFERSDEBUG */

#if WITHUSBUAC
	
	if (uacoutplayer || uacoutmike)
		buffers_savetoresampling(p);
	else
		buffers_savetonull(p);
#else /* WITHUSBUAC */
	buffers_savetonull(p);
#endif /* WITHUSBUAC */
}

#if WITHUSBUAC
// ������� ������ �� ��������������
static RAMFUNC void
buffers_savefromresampling(voice16_t * p)
{
	// ���� ����� ������������ � ��� �������� ��������������� ��� ���������� � ��� ���������,
	// � �������� ����� ��������� ����� ����������

	if (uacoutmike != 0)
		buffers_savtomodulators(p);
	else if (uacoutplayer != 0)
		buffers_savetophones(p);
	else
		buffers_savetonull(p);
}

#endif /* WITHUSBUAC */

#if WITHUSBUAC

static RAMFUNC unsigned long ulmin(
	unsigned long a,
	unsigned long b)
{
	return a < b ? a : b;
}

// ���������� ���������� ��������� �������
static RAMFUNC unsigned getsamplemsuacout(
	uint16_t * buff,	// ������� ������� � ������� ������
	unsigned size		// ���������� ���������� ��������� �������
	)
{
	static voice16_t * p = NULL;
	enum { NPARTS = 3 };
	static uint_fast8_t part = 0;
	static uint16_t * datas [NPARTS] = { NULL, NULL };		// ��������� ����� ���� ������� �� ������� ������
	static unsigned sizes [NPARTS] = { 0, 0 };			// ���������� ������� �� ������� ������
	// ������������ ����������� = 0.02% - ���� ����� ��������/������ �� 5000 �������
	enum { SKIPPED = 5000 / (DMABUFFSIZE16 / DMABUFSTEP16) };

	static unsigned skipsense = SKIPPED;

	LOCK(& locklist16);
	if (p == NULL)
	{
		if (uacout16ready == 0)
		{
#if WITHBUFFERSDEBUG
			++ nbzero;
#endif /* WITHBUFFERSDEBUG */
			// ����������� ����� ��� �� ����� ������ �������� - ���������� 0.
			UNLOCK(& locklist16);
			memset(buff, 0x00, size * sizeof (* buff));	// ������
			return size;	// ���� ������ ���������� - ������������� ��������� �����������
		}
		else
		{
			PLIST_ENTRY t = RemoveTailList2(& uacout16);
			uacout16ready = fiforeadyupdate(uacout16ready, uacout16.Count, VOICESMIKE16NORMAL);
			p = CONTAINING_RECORD(t, voice16_t, item);
			UNLOCK(& locklist16);
			
			if (uacout16ready == 0)
				skipsense = SKIPPED;
			const uint_fast8_t valid = uacout16ready && skipsense == 0;

			skipsense = (skipsense == 0) ? SKIPPED : skipsense - 1;

			const uint_fast8_t LOW = VOICESMIKE16NORMAL - 3;
			const uint_fast8_t HIGH = VOICESMIKE16NORMAL + 1;

			if (valid && uacout16.Count <= LOW)
			{
				// ����������� ���� ����� � ��������� ������ ��� � SKIPPED ������
#if WITHBUFFERSDEBUG
				++ nbadd;
#endif /* WITHBUFFERSDEBUG */

#if 0
				part = NPARTS - 2;
				datas [part + 0] = & p->buff [0];	// ��������� ������ �����
				sizes [part + 0] = DMABUFSTEP16;
				datas [part + 1] = & p->buff [0];
				sizes [part + 1] = DMABUFFSIZE16;
#else
				static uint16_t addsample [DMABUFSTEP16];
				enum { HALF = DMABUFFSIZE16 / 2 };
				// �������� ��� ������� �������������� �������, ����� �������� ��������� ��������������.
				addsample [0] = ((int_fast32_t) (int16_t) p->buff [HALF - DMABUFSTEP16 + 0] +  (int16_t) p->buff [HALF + 0]) / 2;	// Left
				addsample [1] = ((int_fast32_t) (int16_t) p->buff [HALF - DMABUFSTEP16 + 1] +  (int16_t) p->buff [HALF + 1]) / 2;	// Right
				part = NPARTS - 3;
				datas [0] = & p->buff [0];		// ����� ����� ��������
				sizes [0] = HALF;
				datas [1] = & addsample [0];	// ����������� ������
				sizes [1] = DMABUFSTEP16;
				datas [2] = & p->buff [HALF];	// ����� ����� �������
				sizes [2] = DMABUFFSIZE16 - HALF;
#endif
			}
			else if (valid && uacout16.Count >= HIGH)
			{
#if WITHBUFFERSDEBUG
				++ nbdel;
#endif /* WITHBUFFERSDEBUG */
				// ��������� ���� ����� �� ��������� ������ ��� � SKIPPED ������
				part = NPARTS - 1;
				datas [part] = & p->buff [DMABUFSTEP16];	// ���������� ������ �����
				sizes [part] = DMABUFFSIZE16 - DMABUFSTEP16;
			}
			else
			{
				// ���������� �� ��������� ��� ��� ������ ������� ������
				part = NPARTS - 1;
				datas [part] = & p->buff [0];
				sizes [part] = DMABUFFSIZE16;
			}
		}
	}
	else
	{
		UNLOCK(& locklist16);
	}

	const unsigned chunk = ulmin(sizes [part], size);

	memcpy(buff, datas [part], chunk * sizeof (* buff));

	datas [part] += chunk;
	if ((sizes [part] -= chunk) == 0 && ++ part >= NPARTS)
	{
		buffers_savetonull(p);
		p = NULL;
	}
	return chunk;
}

// ������������ ������ ������ ����������� ������ �� N �������������
static RAMFUNC void buffers_resample(void)
{
	const uintptr_t addr = allocate_dmabuffer16();	// �������� �����
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	//
	// ���������� �����������
	unsigned pos;
	for (pos = 0; pos < DMABUFFSIZE16; )
	{
		pos += getsamplemsuacout(& p->buff [pos], DMABUFFSIZE16 - pos);
	}

	// ����������� ������������� ������ ����������.
	buffers_savefromresampling(p);
}
#endif /* WITHUSBUAC */

// ��������� ���� �� ��� ���������
static RAMFUNC void buffers_savefrommikeadc(voice16_t * p)
{

#if WITHBUFFERSDEBUG
	// ������� �������� � ������� �� �������
	debugcount_mikeadc += sizeof p->buff / sizeof p->buff [0] / DMABUFSTEP16;	// � ������ ���� ������� �� ��� �����
#endif /* WITHBUFFERSDEBUG */

	if (uacoutmike == 0)
		buffers_savtomodulators(p);
	else
		buffers_savetonull(p);

}
// --- ���������� ������� �����������

#if WITHUSEAUDIOREC
// ������������ ���������� ������ SD CARD

unsigned long hamradio_get_recdropped(void)
{
	return recdropped;
}

int hamradio_get_recdbuffered(void)
{
	return recbuffered;
}

/* to SD CARD */
// 16 bit, signed
void RAMFUNC savesamplerecord16SD(int_fast16_t left, int_fast16_t right)
{
	// ���� ���� ������������������ ����� ��� ������ �����
	static records16_t * preparerecord16 = NULL;
	static unsigned level16record = 0;

	if (preparerecord16 == NULL)
	{
		if (! IsListEmpty2(& recordsfree16))
		{
			PLIST_ENTRY t = RemoveTailList2(& recordsfree16);
			preparerecord16 = CONTAINING_RECORD(t, records16_t, item);
		}
		else
		{
			-- recbuffered;
			++ recdropped;
			// ���� ��� ��������� - ��������� ����� ����� �������������� ��� ������ �����
			PLIST_ENTRY t = RemoveTailList2(& recordsready16);
			preparerecord16 = CONTAINING_RECORD(t, records16_t, item);
		}
		
		// ���������� � ������ ����� WAV �� ���������� DATA CHUNK, �� ������������ ����
		// ��������� ������ ������ ADOBE AUDITION, Windows Media Player 12 ����������� ������ ���� - ������.
		// Windows Media Player Classic (https://github.com/mpc-hc/mpc-hc) ������ �� ����������� ���� ����.
		
		//preparerecord16->buff [0] = 'd' | 'a' * 256;
		//preparerecord16->buff [1] = 't' | 'a' * 256;
		//preparerecord16->buff [2] = ((SDCARDBUFFSIZE16 * sizeof preparerecord16->buff [0]) - 8) >> 0;
		//preparerecord16->buff [3] = ((SDCARDBUFFSIZE16 * sizeof preparerecord16->buff [0]) - 8) >> 16;
		//level16record = 4;

		level16record = 0;
	}

#if WITHUSEAUDIOREC2CH
	// ������ ����� �� SD CARD � ������
	preparerecord16->buff [level16record ++] = left;	// sample value
	preparerecord16->buff [level16record ++] = right;	// sample value

#else /* WITHUSEAUDIOREC2CH */
	// ������ ����� �� SD CARD � ����
	preparerecord16->buff [level16record ++] = left;	// sample value

#endif /* WITHUSEAUDIOREC2CH */

	if (level16record >= SDCARDBUFFSIZE16)
	{
		++ recbuffered;
		InsertHeadList2(& recordsready16, & preparerecord16->item);
		preparerecord16 = NULL;
	}
}

unsigned takerecordbuffer(void * * dest)
{
	global_disableIRQ();
	if (! IsListEmpty2(& recordsready16))
	{
		PLIST_ENTRY t = RemoveTailList2(& recordsready16);
		global_enableIRQ();
		-- recbuffered;
		records16_t * const p = CONTAINING_RECORD(t, records16_t, item);
		* dest = p->buff;
		return (SDCARDBUFFSIZE16 * sizeof p->buff [0]);
	}
	global_enableIRQ();
	return 0;
}

void releaserecordbuffer(void * dest)
{
	records16_t * const p = CONTAINING_RECORD(dest, records16_t, buff);
	global_disableIRQ();
	InsertHeadList2(& recordsfree16, & p->item);
	global_enableIRQ();
}

#endif /* WITHUSEAUDIOREC */

#if WITHMODEM


// ������ � ���������� ����� ����� �������
size_t takemodemrxbuffer(uint8_t * * dest)
{
	global_disableIRQ();
	if (! IsListEmpty2(& modemsrx8))
	{
		PLIST_ENTRY t = RemoveTailList2(& modemsrx8);
		global_enableIRQ();
		modems8_t * const p = CONTAINING_RECORD(t, modems8_t, item);
		* dest = p->buff;
		return p->length;
	}
	global_enableIRQ();
	* dest = NULL;
	return 0;
}

// ������ ��� ���������� �������
size_t takemodembuffer(uint8_t * * dest)
{
	global_disableIRQ();
	if (! IsListEmpty2(& modemsfree8))
	{
		PLIST_ENTRY t = RemoveTailList2(& modemsfree8);
		global_enableIRQ();
		modems8_t * const p = CONTAINING_RECORD(t, modems8_t, item);
		* dest = p->buff;
		return (MODEMBUFFERSIZE8 * sizeof p->buff [0]);
	}
	global_enableIRQ();
	* dest = NULL;
	return 0;
}

// ������ ��� ���������� �������
// ���������� �� real-time ����������� ����������
size_t takemodembuffer_low(uint8_t * * dest)
{
	if (! IsListEmpty2(& modemsfree8))
	{
		PLIST_ENTRY t = RemoveTailList2(& modemsfree8);
		modems8_t * const p = CONTAINING_RECORD(t, modems8_t, item);
		* dest = p->buff;
		return (MODEMBUFFERSIZE8 * sizeof p->buff [0]);
	}
	* dest = NULL;
	return 0;
}

// ����� ����� � ��������� �������
// ���������� �� real-time ����������� ����������
void savemodemrxbuffer_low(uint8_t * dest, size_t length)
{
	modems8_t * const p = CONTAINING_RECORD(dest, modems8_t, buff);
	p->length = length;
	InsertHeadList2(& modemsrx8, & p->item);
}

void releasemodembuffer(uint8_t * dest)
{
	modems8_t * const p = CONTAINING_RECORD(dest, modems8_t, buff);
	global_disableIRQ();
	InsertHeadList2(& modemsfree8, & p->item);
	global_enableIRQ();
}

// ���������� �� real-time ����������� ����������
void releasemodembuffer_low(uint8_t * dest)
{
	modems8_t * const p = CONTAINING_RECORD(dest, modems8_t, buff);
	InsertHeadList2(& modemsfree8, & p->item);
}

#endif /* WITHMODEM */

// ���� �������� ���������� ����������� ���������� DMA �� �������� ������ �� SAI
RAMFUNC uintptr_t allocate_dmabuffer32tx(void)
{
	LOCK(& locklist32);
	if (! IsListEmpty2(& voicesfree32tx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesfree32tx);
		voice32tx_t * const p = CONTAINING_RECORD(t, voice32tx_t, item);
		UNLOCK(& locklist32);
		return (uint32_t) & p->buff;
	}
	else if (! IsListEmpty2(& voicesready32tx))
	{
		// ��������� �������� - ���� ������ �� ����������� ������� -
		// ���� �� ������� ������� � ��������

		uint_fast8_t n = 3;
		do
		{
			const PLIST_ENTRY t = RemoveTailList2(& voicesready32tx);
			InsertHeadList2(& voicesfree32tx, t);
		}
		while (-- n && ! IsListEmpty2(& voicesready32tx));

		PLIST_ENTRY t = RemoveTailList2(& voicesfree32tx);
		voice32tx_t * const p = CONTAINING_RECORD(t, voice32tx_t, item);
		UNLOCK(& locklist32);
		return (uint32_t) & p->buff;
	}
	else
	{
		debug_printf_P(PSTR("allocate_dmabuffer32tx() failure\n"));
		for (;;)
			;
	}
	UNLOCK(& locklist32);
}

// ���� �������� ���������� ����������� ���������� DMA �� ���� ������ �� SAI
RAMFUNC uintptr_t allocate_dmabuffer32rx(void)
{
	LOCK(& locklist32);
	if (! IsListEmpty2(& voicesfree32rx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesfree32rx);
		voice32rx_t * const p = CONTAINING_RECORD(t, voice32rx_t, item);
		UNLOCK(& locklist32);
		return (uint32_t) & p->buff;
	}
	else
	{
		debug_printf_P(PSTR("allocate_dmabuffer32rx() failure\n"));
		for (;;)
			;
	}
	UNLOCK(& locklist32);
}

// ���� �������� ���������� ����������� ���������� DMA �� �������� � ���� ������ �� I2S � USB AUDIO
RAMFUNC uintptr_t allocate_dmabuffer16(void)
{
	LOCK(& locklist16);
	if (! IsListEmpty2(& voicesfree16))
	{
		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16);
		UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
		return (uint32_t) & p->buff;
	}
#if WITHUSBUAC
	else if (! IsListEmpty2(& uacin16))
	{
		// ��������� �������� - ���� ������ �� ����������� ������� -
		// ���� �� ������� ������� � �������� � ��������� �� USB.
		// ������� ��������� �������� �� ���������.
		uint_fast8_t n = 3;
		do
		{
			const PLIST_ENTRY t = RemoveTailList2(& uacin16);
			InsertHeadList2(& voicesfree16, t);
		}
		while (-- n && ! IsListEmpty2(& uacin16));

		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16);	//  ����� �� ��������� ������� ��������� - ���� �� ����� ��������� ����� ����.
		UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
	#if WITHBUFFERSDEBUG
		++ e2;
	#endif /* WITHBUFFERSDEBUG */
		return (uint32_t) & p->buff;
	}
	else if (! IsListEmpty2(& uacout16))
	{
		// ��������� �������� - ���� ������ �� ����������� ������� -
		// ���� �� ������� ������� � �����������
		uint_fast8_t n = 3;
		do
		{
			const PLIST_ENTRY t = RemoveTailList2(& uacout16);
			InsertHeadList2(& voicesfree16, t);
		}
		while (-- n && ! IsListEmpty2(& uacout16));
		uacout16ready = fiforeadyupdate(uacout16ready, uacout16.Count, VOICESMIKE16NORMAL);

		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16);
		UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
	#if WITHBUFFERSDEBUG
		++ e3;
	#endif /* WITHBUFFERSDEBUG */
		return (uint32_t) & p->buff;
	}
#endif /* WITHUSBUAC */
	else if (! IsListEmpty2(& voicesphones16))
	{
		// ��������� �������� - ���� ������ �� ����������� ������� -
		// ���� �� ������� ������� � �������������

		uint_fast8_t n = 3;
		do
		{
			const PLIST_ENTRY t = RemoveTailList2(& voicesphones16);
			InsertHeadList2(& voicesfree16, t);
		}
		while (-- n && ! IsListEmpty2(& voicesphones16));

		const PLIST_ENTRY t = RemoveTailList2(& voicesfree16);
		UNLOCK(& locklist16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
	#if WITHBUFFERSDEBUG
		++ e4;
	#endif /* WITHBUFFERSDEBUG */
		return (uint32_t) & p->buff;
	}
	else
	{
		UNLOCK(& locklist16);
		debug_printf_P(PSTR("allocate_dmabuffer16() failure\n"));
		for (;;)
			;
	}
}

// ���� �������� ���������� ����������� ���������� DMA
// �������� �����, ������� ���������
void RAMFUNC release_dmabuffer32tx(uintptr_t addr)
{
	ASSERT(addr != 0);
	LOCK(& locklist32);
	voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
	InsertHeadList2(& voicesfree32tx, & p->item);
	UNLOCK(& locklist32);
}

// ���� �������� ���������� ����������� ���������� DMA
// �������� �����, ������� ���������
void RAMFUNC release_dmabuffer16(uintptr_t addr)
{
	ASSERT(addr != 0);
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	buffers_savetonull(p);
}

// ���� �������� ���������� ����������� ���������� DMA
// ���������� ����� ����� ��������� AF ADC
void RAMFUNC processing_dmabuffer16rx(uintptr_t addr)
{
	ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n3;
#endif /* WITHBUFFERSDEBUG */
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	buffers_savefrommikeadc(p);
}

// ���� �������� ���������� ����������� ���������� DMA
// ���������� ����� ����� ����� ������ � USB AUDIO
void RAMFUNC processing_dmabuffer16rxuac(uintptr_t addr)
{
	ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n2;
#endif /* WITHBUFFERSDEBUG */
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	buffers_savefromuacout(p);
}

// ���� �������� ���������� ����������� ���������� DMA
// ���������� ����� ����� ��������� IF ADC (MAIN RX/SUB RX)
// ���������� �� ARM_REALTIME_PRIORITY ������.
void RAMFUNC processing_dmabuffer32rx(uintptr_t addr)
{
	enum { CNT16 = DMABUFFSIZE16 / DMABUFSTEP16 };
	enum { CNT32RX = DMABUFFSIZE32RX / DMABUFSTEP32RX };
	ASSERT(addr != 0);
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);
#if WITHBUFFERSDEBUG
	++ n1;
	// ������� �������� � ������� �� �������
	debugcount_rx32adc += CNT32RX;	// � ������ ���� ������� �� ������ �����
#endif /* WITHBUFFERSDEBUG */
	dsp_extbuffer32rx(p->buff);

	LOCK(& locklist32);
	InsertHeadList2(& voicesfree32rx, & p->item);
	UNLOCK(& locklist32);

#if WITHUSBUAC
	static unsigned rx32adc = 0;
	rx32adc += CNT32RX; 
	while (rx32adc >= CNT16)
	{
		buffers_resample();		// ������������ ������ ������ ����������� ������ �� N �������������
		rx32adc -= CNT16;
	}
#endif /* WITHUSBUAC */
}

// ���� �������� ���������� ����������� ���������� DMA
// ���������� ����� ����� ��������� IF ADC (MAIN RX/SUB RX)
// ���������� �� ARM_REALTIME_PRIORITY ������.
void RAMFUNC processing_dmabuffer32wfm(uintptr_t addr)
{
	//enum { CNT16 = DMABUFFSIZE16 / DMABUFSTEP16 };
	enum { CNT32RX = DMABUFFSIZE32RX / DMABUFSTEP32RX };
	ASSERT(addr != 0);
	voice32rx_t * const p = CONTAINING_RECORD(addr, voice32rx_t, buff);
#if WITHBUFFERSDEBUG
	++ n1wfm;
	// ������� �������� � ������� �� �������
	debugcount_rx32wfm += CNT32RX;	// � ������ ���� ������� �� ������ �����
#endif /* WITHBUFFERSDEBUG */
	dsp_extbuffer32wfm(p->buff);

	LOCK(& locklist32);
	InsertHeadList2(& voicesfree32rx, & p->item);
	UNLOCK(& locklist32);

}

#if WITHRTS192
// ���� �������� ���������� ����������� ���������� DMA
// ���������� ����� ����� ��������� - ����� ������������������
void RAMFUNC processing_dmabuffer32rts(uintptr_t addr)
{
	ASSERT(addr != 0);
#if WITHBUFFERSDEBUG
	++ n4;
#endif /* WITHBUFFERSDEBUG */
	voice192rts_t * const p = CONTAINING_RECORD(addr, voice192rts_t, buff);

#if ! WITHTRANSPARENTIQ
	unsigned i;
	for (i = 0; i < DMABUFFSIZE192RTS; i += DMABUFSTEP192RTS)
	{
		const int32_t * const b = (const int32_t *) & p->buff [i];
		saveIQRTSxx(b [0], b [1]);
	}
#endif /* ! WITHTRANSPARENTIQ */

	buffers_savetouacin192rts(p);
}
#endif /* WITHRTS192 */


// ���� �������� ���������� ����������� ���������� DMA
// �������� ����� ��� �������� ����� IF DAC
uintptr_t getfilled_dmabuffer32tx_main(void)
{
#if WITHBUFFERSDEBUG
	// ������� �������� � ������� �� �������
	debugcount_tx32dac += DMABUFFSIZE32TX / DMABUFSTEP32TX;	// � ������ ���� ������� �� ������ �����
#endif /* WITHBUFFERSDEBUG */

	LOCK(& locklist32);
	if (! IsListEmpty2(& voicesready32tx))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesready32tx);
		voice32tx_t * const p = CONTAINING_RECORD(t, voice32tx_t, item);
		UNLOCK(& locklist32);
		return (uintptr_t) & p->buff;
	}
	UNLOCK(& locklist32);
	return allocate_dmabuffer32tx();	// ��������� ����� - �������� ������ ��������� ���
}

// ���� �������� ���������� ����������� ���������� DMA
// �������� ����� ��� �������� ����� IF DAC2
uintptr_t getfilled_dmabuffer32tx_sub(void)
{
	return allocate_dmabuffer32tx();
}

#if WITHI2SHW
// ���� �������� ���������� ����������� ���������� DMA
// �������� ����� ��� �������� ����� AF DAC
uintptr_t getfilled_dmabuffer16phones(void)
{
#if WITHBUFFERSDEBUG
	// ������� �������� � ������� �� �������
	debugcount_phonesdac += DMABUFFSIZE16 / DMABUFSTEP16;	// � ������ ���� ������� �� ��� �����
#endif /* WITHBUFFERSDEBUG */

	LOCK(& locklist16);
	if (! IsListEmpty2(& voicesphones16))
	{
		PLIST_ENTRY t = RemoveTailList2(& voicesphones16);
		voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
		UNLOCK(& locklist16);
		return (uint32_t) & p->buff;
	}
	UNLOCK(& locklist16);
#if WITHBUFFERSDEBUG
	++ e1;
#endif /* WITHBUFFERSDEBUG */
	return getfilled_dmabuffer16silence();		// ��������� ����� - �������� ������ ��������� ���
}
#endif /* WITHI2SHW */

//////////////////////////////////////////
// ������������ ���������� ������ IF DAC

// ���������� �� ARM_REALTIME_PRIORITY ����������� ����������
// 32 bit, signed
void savesampleout32stereo(int_fast32_t ch0, int_fast32_t ch1)
{
	LOCK(& locklist32);
	static voice32tx_t * prepareout32tx = NULL;
	static unsigned level32tx = 0;

	if (prepareout32tx == NULL)
	{
		UNLOCK(& locklist32);
		const uint32_t addr = allocate_dmabuffer32tx();
		LOCK(& locklist32);
		voice32tx_t * const p = CONTAINING_RECORD(addr, voice32tx_t, buff);
		prepareout32tx = p;
		level32tx = 0;
	}

	prepareout32tx->buff [level32tx + DMABUF32TXI] = ch0;
	prepareout32tx->buff [level32tx + DMABUF32TXQ] = ch1;

	if ((level32tx += DMABUFSTEP32TX) >= DMABUFFSIZE32TX)
	{
		InsertHeadList2(& voicesready32tx, & prepareout32tx->item);
		prepareout32tx = NULL;
	}
	UNLOCK(& locklist32);
}

//////////////////////////////////////////
// ������������ ���������� ������ AF DAC

// ���������� �� ARM_REALTIME_PRIORITY ����������� ����������
// vl, vr: 16 bit, signed
void savesampleout16stereo(int_fast16_t ch0, int_fast16_t ch1)
{
#if WITHI2SHW
	// ���� ���� ������������������ ����� ��� ������ �����
	static voice16_t * p = NULL;
	static unsigned n = 0;

	if (p == NULL)
	{
		uintptr_t addr = allocate_dmabuffer16();
		p = CONTAINING_RECORD(addr, voice16_t, buff);
		n = 0;
	}

	p->buff [n + 0] = ch0;		// sample value
	p->buff [n + 1] = ch1;	// sample value

	n += DMABUFSTEP16;

	if (n >= DMABUFFSIZE16)
	{
		buffers_savefromrxout(p);
		p = NULL;
	}
#endif /* WITHI2SHW */
}

#if WITHUSBUAC

	#if WITHRTS96

		// ���� �������� ���������� ����������� ���������� DMA �� ���� ������ �� SAI
		static uintptr_t allocate_dmabuffer96rts(void)
		{
			LOCK(& locklist32);
			if (! IsListEmpty2(& voicesfree96rts))
			{
				PLIST_ENTRY t = RemoveTailList2(& voicesfree96rts);
				voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
				UNLOCK(& locklist32);
			#if WITHBUFFERSDEBUG
				++ n5;
			#endif /* WITHBUFFERSDEBUG */
				return (uint32_t) & p->buff;
			}
			else if (! IsListEmpty2(& uacin96rts))
			{
				// ��������� �������� - ���� ������ �� ����������� ������� -
				// ���� �� ������� ������� � �������� � ��������� �� USB.
				// ������� ��������� �������� �� ���������.
				uint_fast8_t n = 3;
				do
				{
					const PLIST_ENTRY t = RemoveTailList2(& uacin96rts);
					InsertHeadList2(& voicesfree96rts, t);
				} while (-- n && ! IsListEmpty2(& uacin96rts));

				const PLIST_ENTRY t = RemoveTailList2(& voicesfree96rts);
				voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
				UNLOCK(& locklist32);
			#if WITHBUFFERSDEBUG
				++ e5;
			#endif /* WITHBUFFERSDEBUG */
				return (uint32_t) & p->buff;
			}
			else
			{
				debug_printf_P(PSTR("allocate_dmabuffer96rts() failure\n"));
				for (;;)
					;
			}
			UNLOCK(& locklist32);
		}

		// ���� �������� ���������� ����������� ���������� DMA
		// �������� �����, ������� ���������
		static void release_dmabuffer96rts(uintptr_t addr)
		{
			LOCK(& locklist32);
			voice96rts_t * const p = CONTAINING_RECORD(addr, voice96rts_t, buff);
			InsertHeadList2(& voicesfree96rts, & p->item);
			UNLOCK(& locklist32);
		}

		// ���� �������� ���������� ����������� ���������� DMA
		// �������� ����� ��� �������� � ���������, ����� USB AUDIO
		// ���� � ������ ������ ��� �������� ������, ������� 0
		static uint32_t getfilled_dmabuffer96uacinrts(void)
		{
			LOCK(& locklist16);
			if (! IsListEmpty2(& uacin96rts))
			{
				PLIST_ENTRY t = RemoveTailList2(& uacin96rts);
				voice96rts_t * const p = CONTAINING_RECORD(t, voice96rts_t, item);
				UNLOCK(& locklist16);
				return (uint32_t) & p->buff;
			}
			UNLOCK(& locklist16);
			return 0;
		}

		// ������������ ���������� ������ RTS96

		// ���������� �� ARM_REALTIME_PRIORITY ����������� ����������
		// vl, vr: 32 bit, signed - ����������� � ���������� ������� ��� �������� �� USB �����.
		void savesampleout96stereo(int_fast32_t ch0, int_fast32_t ch1)
		{
			// ���� ���� ������������������ ����� ��� ������ �����
			static voice96rts_t * p = NULL;
			static unsigned n = 0;

			if (p == NULL)
			{
				if (! isrts96())
					return;
				uint32_t addr = allocate_dmabuffer96rts();
				p = CONTAINING_RECORD(addr, voice96rts_t, buff);
				n = 0;
			}

			ASSERT(DMABUFSTEP96RTS == 6);
			p->buff [n ++] = ch0 >> 8;	// sample value
			p->buff [n ++] = ch0 >> 16;	// sample value
			p->buff [n ++] = ch0 >> 24;	// sample value
			p->buff [n ++] = ch1 >> 8;	// sample value
			p->buff [n ++] = ch1 >> 16;	// sample value
			p->buff [n ++] = ch1 >> 24;	// sample value

			if (n >= DMABUFFSIZE96RTS)
			{
				if (isrts96())
					buffers_savetouacin96rts(p);
				else
					buffers_savetonull96rts(p);
				p = NULL;
			}
		}

	#endif /* WITHRTS96 */

	#if WITHRTS192

		// ���� �������� ���������� ����������� ���������� DMA
		// �������� ����� ��� �������� � ���������, ����� USB AUDIO
		// ���� � ������ ������ ��� �������� ������, ������� 0
		static uintptr_t getfilled_dmabuffer192uacinrts(void)
		{
			LOCK(& locklist16);
			if (! IsListEmpty2(& uacin192rts))
			{
				PLIST_ENTRY t = RemoveTailList2(& uacin192rts);
				voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
				UNLOCK(& locklist16);
				return (uintptr_t) & p->buff;
			}
			UNLOCK(& locklist16);
			return 0;
		}

		// ���� �������� ���������� ����������� ���������� DMA �� ���� ������ �� SAI
		uint32_t allocate_dmabuffer192rts(void)
		{
			LOCK(& locklist32);
			if (! IsListEmpty2(& voicesfree192rts))
			{
				PLIST_ENTRY t = RemoveTailList2(& voicesfree192rts);
				voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
				UNLOCK(& locklist32);
			#if WITHBUFFERSDEBUG
				++ n5;
			#endif /* WITHBUFFERSDEBUG */
				return (uint32_t) & p->buff;
			}
			else if (! IsListEmpty2(& uacin192rts))
			{
				// ��������� �������� - ���� ������ �� ����������� ������� -
				// ���� �� ������� ������� � �������� � ��������� �� USB.
				// ������� ��������� �������� �� ���������.
				uint_fast8_t n = 3;
				do
				{
					const PLIST_ENTRY t = RemoveTailList2(& uacin192rts);
					InsertHeadList2(& voicesfree192rts, t);
				} while (-- n && ! IsListEmpty2(& uacin192rts));

				const PLIST_ENTRY t = RemoveTailList2(& voicesfree192rts);
				voice192rts_t * const p = CONTAINING_RECORD(t, voice192rts_t, item);
				UNLOCK(& locklist32);
			#if WITHBUFFERSDEBUG
				++ e5;
			#endif /* WITHBUFFERSDEBUG */
				return (uint32_t) & p->buff;
			}
			else
			{
				debug_printf_P(PSTR("allocate_dmabuffer192rts() failure\n"));
				for (;;)
					;
			}
			UNLOCK(& locklist32);
		}

		// ���� �������� ���������� ����������� ���������� DMA
		// �������� �����, ������� ���������
		static void release_dmabuffer192rts(uint32_t addr)
		{
			LOCK(& locklist32);
			voice192rts_t * const p = CONTAINING_RECORD(addr, voice192rts_t, buff);
			InsertHeadList2(& voicesfree192rts, & p->item);
			UNLOCK(& locklist32);
		}

		// NOT USED
		// ������������ ���������� ������ RTS192

		// ���������� �� ARM_REALTIME_PRIORITY ����������� ����������
		// vl, vr: 32 bit, signed - ����������� � ���������� ������� ��� �������� �� USB �����.
		void savesampleout192stereo(int_fast32_t ch0, int_fast32_t ch1)
		{
			// ���� ���� ������������������ ����� ��� ������ �����
			static voice192rts_t * p = NULL;
			static unsigned n = 0;

			if (p == NULL)
			{
				if (! isrts192())
					return;
				uint32_t addr = allocate_dmabuffer192rts();
				p = CONTAINING_RECORD(addr, voice192rts_t, buff);
				n = 0;
			}

			p->buff [n ++] = ch0;	// sample value
			p->buff [n ++] = ch1;	// sample value

			if (n >= DMABUFFSIZE192RTS)
			{
				if (isrts192())
					buffers_savetouacin192rts(p);
				else
					buffers_savetonull192rts(p);
				p = NULL;
			}
		}

	#endif /* WITHRTS192 */

	// ���� �������� ���������� ����������� ���������� DMA
	// �������� ����� ��� �������� � ���������, ����� USB AUDIO
	// ���� � ������ ������ ��� �������� ������, ������� 0
	static uintptr_t getfilled_dmabuffer16uacin(void)
	{
		LOCK(& locklist16);
		if (! IsListEmpty2(& uacin16))
		{
			PLIST_ENTRY t = RemoveTailList2(& uacin16);
			voice16_t * const p = CONTAINING_RECORD(t, voice16_t, item);
			UNLOCK(& locklist16);
			return (uintptr_t) & p->buff;
		}
		UNLOCK(& locklist16);
		return 0;
	}

	// ���������� �� ARM_REALTIME_PRIORITY ����������� ����������
	// vl, vr: 16 bit, signed - ��������� ������ ��� �������� �� USB.

	void savesamplerecord16uacin(int_fast16_t ch0, int_fast16_t ch1)
	{
	#if WITHUSBHW && WITHUSBUAC
		// ���� ���� ������������������ ����� ��� ������ �����
		static voice16_t * p = NULL;
		static unsigned n = 0;

		if (p == NULL)
		{
			if (! isaudio48())
				return;
			uintptr_t addr = allocate_dmabuffer16();
			p = CONTAINING_RECORD(addr, voice16_t, buff);
			n = 0;
		}

		p->buff [n + 0] = ch0;		// sample value
		p->buff [n + 1] = ch1;	// sample value

		n += DMABUFSTEP16;

		if (n >= DMABUFFSIZE16)
		{
			if (isaudio48())
				buffers_savetouacin(p);
			else
				buffers_savetonull(p);
			p = NULL;
		}
	#endif /* WITHUSBHW && WITHUSBUAC */
	}

#endif /* WITHUSBUAC */

#if WITHUSBUAC

/* ����� ������������� ������ ���������� � ��������� ���������� - ���������� ����� */
void board_set_uacplayer(uint_fast8_t v)
{
	uacoutplayer = v;
}

/* �� ���� ���������� ������� ����������� � USB ����������� �����, � �� � ��������� */
void board_set_uacmike(uint_fast8_t v)
{
	uacoutmike = v;
}


void 
buffers_set_uacinalt(uint_fast8_t v)	/* ����� �������������� ������������ ��� UAC IN interface */
{
	//debug_printf_P(PSTR("buffers_set_uacinalt: v=%d\n"), v);
	uacinalt = v;
}

#if WITHUSBUAC3

void 
buffers_set_uacinrtsalt(uint_fast8_t v)	/* ����� �������������� ������������ ��� UAC IN interface */
{
	//debug_printf_P(PSTR("buffers_set_uacinrtsalt: v=%d\n"), v);
	uacinrtsalt = v;
}

#endif /* WITHUSBUAC3 */

void 
buffers_set_uacoutalt(uint_fast8_t v)	/* ����� �������������� ������������ ��� UAC OUT interface */
{
	//debug_printf_P(PSTR("buffers_set_uacoutalt: v=%d\n"), v);
	uacoutalt = v;
}

/* +++ UAC OUT data save */


static uint_fast16_t ulmin16(uint_fast16_t a, uint_fast16_t b)
{
	return a < b ? a : b;
}

static uintptr_t uacoutaddr;	// address of DMABUFFSIZE16 * sizeof (uint16_t) bytes
static uint_fast16_t uacoutbufflevel;	// ���������� ������� �� ������� �������� �����

/* ���������� ��� ����������� �����������. */
void uacout_buffer_initialize(void)
{
	uacoutaddr = 0;
	uacoutbufflevel = 0;
}

/* ��������� �� ARM_SYSTEM_PRIORITY ������� ����������� ���������� */
void uacout_buffer_start(void)
{
	if (uacoutaddr)
		TP();
}
/* ��������� �� ARM_SYSTEM_PRIORITY ������� ����������� ���������� */
void uacout_buffer_stop(void)
{
	if (uacoutaddr != 0)
	{
		global_disableIRQ();
		release_dmabuffer16(uacoutaddr);
		global_enableIRQ();
		uacoutaddr = 0;
		uacoutbufflevel = 0;
	}
}

/* ��������� �� ��-realtime ������� ����������� ���������� */
// �������� �� ARM_SYSTEM_PRIORITY
void uacout_buffer_save(const uint8_t * buff, uint_fast16_t size)
{
	const size_t dmabuffer16size = DMABUFFSIZE16 * sizeof (uint16_t);	// ������ � ������

#if WITHUABUACOUTAUDIO48MONO

	for (;;)
	{
		const uint_fast16_t insamples = size / 2;	// ���������� ������� �� ������� ������
		const uint_fast16_t outsamples = (dmabuffer16size - uacoutbufflevel) / 2 / DMABUFSTEP16;
		const uint_fast16_t chunksamples = ulmin16(insamples, outsamples);
		if (chunksamples == 0)
			break;
		if (uacoutaddr == 0)
		{
 			global_disableIRQ();
			uacoutaddr = allocate_dmabuffer16();
			global_enableIRQ();
			uacoutbufflevel = 0;
		}
		//memcpy((uint8_t *) uacoutaddr + uacoutbufflevel, buff, chunk);
		{
			// ����������� ������� ���������� ������� � ��������������� �� ���� � ������
			const uint16_t * src = (const uint16_t *) buff;
			uint16_t * dst = (uint16_t *) ((uint8_t *) uacoutaddr + uacoutbufflevel);
			uint_fast16_t n = chunksamples;
			while (n --)
			{
				const uint_fast16_t v = * src ++;
				* dst ++ = v;
				* dst ++ = v;
			}
		}
		const uint_fast16_t inchunk = chunksamples * 2;
		const uint_fast16_t outchunk = chunksamples * 2 * DMABUFSTEP16;	// ������� � ������
		size -= inchunk;	// ������ �� ������� ������
		buff += inchunk;	// ������ ������� ������

		if ((uacoutbufflevel += outchunk) >= dmabuffer16size)
		{
			global_disableIRQ();
			processing_dmabuffer16rxuac(uacoutaddr);
			global_enableIRQ();
			uacoutaddr = 0;
			uacoutbufflevel = 0;
		}
	}
#else /* WITHUABUACOUTAUDIO48MONO */
	
	for (;;)
	{
		const uint_fast16_t chunk = ulmin16(size, dmabuffer16size - uacoutbufflevel);
		if (chunk == 0)
			break;
		if (uacoutaddr == 0)
		{
 			global_disableIRQ();
			uacoutaddr = allocate_dmabuffer16();
			global_enableIRQ();
			uacoutbufflevel = 0;
		}
		memcpy((uint8_t *) uacoutaddr + uacoutbufflevel, buff, chunk);
		size -= chunk;		// ������ �� ������� ������
		buff += chunk;		// ������ �� ������� ������

		if ((uacoutbufflevel += chunk) >= dmabuffer16size)	// ������ �� ��������� ������
		{
			global_disableIRQ();
			processing_dmabuffer16rxuac(uacoutaddr);
			global_enableIRQ();
			uacoutaddr = 0;
			uacoutbufflevel = 0;
		}
	}

#endif /* WITHUABUACOUTAUDIO48MONO */
}
/* --- UAC OUT data save */


/* ���������� ����� ������ �� �����, ������� ����� �������������� ��� �������� ����������� � ��������� �� USB */
void release_dmabufferx(uintptr_t addr)
{
	ASSERT(addr != 0);
	voice16_t * const p = CONTAINING_RECORD(addr, voice16_t, buff);
	switch (p->tag)
	{
#if WITHRTS96
	case BUFFTAG_RTS96:
		release_dmabuffer96rts(addr);
		return;
#endif /* WITHRTS96 */

#if WITHRTS192
	case BUFFTAG_RTS192:
		release_dmabuffer192rts(addr);
		return;
#endif /* WITHRTS192 */

	case BUFFTAG_VOICE16:
		release_dmabuffer16(addr);
		return;

	default:
		debug_printf_P(PSTR("release_dmabufferx: wrong tag value: p=%p, %02X\n"), p, p->tag);
		for (;;)
			;
	}
}

/* �������� ����� ������ �� �����, ������� ����� �������������� ��� �������� ����������� � ��������� �� USB */
uintptr_t getfilled_dmabufferx(uint_fast16_t * sizep)
{
#if WITHBUFFERSDEBUG
	++ n6;
#endif /* WITHBUFFERSDEBUG */
	switch (uacinalt)
	{
	case UACINALT_NONE:
		return 0;

	case UACINALT_AUDIO48:
		* sizep = VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_AUDIO48;
		return getfilled_dmabuffer16uacin();

#if ! WITHUSBUAC3

#if WITHRTS96
	case UACINALT_RTS96:
		* sizep = VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96;
		return getfilled_dmabuffer96uacinrts();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINALT_RTS192:
		* sizep = VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192;
		return getfilled_dmabuffer192uacinrts();
#endif /* WITHRTS192 */

#endif /* ! WITHUSBUAC3 */

	default:
		debug_printf_P(PSTR("getfilled_dmabufferx: uacinalt=%u\n"), uacinalt);
		ASSERT(0);
		return 0;
	}
}

/* �������� ����� ������ �� �����, ������� ����� �������������� ��� �������� ����������� � ��������� �� USB */
uintptr_t getfilled_dmabufferxrts(uint_fast16_t * sizep)
{
#if WITHBUFFERSDEBUG
	++ n6;
#endif /* WITHBUFFERSDEBUG */
	switch (uacinrtsalt)
	{
	case UACINRTSALT_NONE:
		return 0;

#if WITHUSBUAC3

#if WITHRTS96
	case UACINRTSALT_RTS96:
		* sizep = VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96;
		return getfilled_dmabuffer96uacinrts();
#endif /* WITHRTS192 */

#if WITHRTS192
	case UACINRTSALT_RTS192:
		* sizep = VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192;
		return getfilled_dmabuffer192uacinrts();
#endif /* WITHRTS192 */

#endif /* WITHUSBUAC3 */

	default:
		debug_printf_P(PSTR("getfilled_dmabufferxrts: uacinrtsalt=%u\n"), uacinrtsalt);
		ASSERT(0);
		return 0;
	}
}

#endif /* WITHUSBUAC */

#endif /* WITHINTEGRATEDDSP */
