/* $Id$ */
//
// ��������� ������������ ����-�������� � �����.
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"
#include "board.h"
#include "spifuncs.h"
#include "formats.h"

#if WITHTX

enum {
	SEQST_INITIALIZE,			// ������ �� ���������� - ��������� ��������� ����������
	// electronic key modes and SSB modes
	SEQST_PUSHED_WAIT_ACK_TX,	// ���, ���� � SW-2011-RDX ���������� ������ TX1 � ����� ���������� �� ��������.
	SEQST_WAIT_ACK_TX,			// ���� �������� �� �������� �� ���������������� ���������
	SEQST_PUSHED_HOLDTX,		// ���, ����� ������������ �������� ���� �� ��������
	SEQST_HOLDTX,				// ��� ���� �� �������� ������ �� ��������
	SEQST_RGBEEP,				// ����� ��������� �� ����, ����� ���������� �������� - ������������ ��������� ������� (roger beep)
	SEQST_PUSHED_SWITCHTORX,	// ���� �������� �� �� ���� �� ���������������� ���������
	SEQST_SWITCHTORX,			// �������� �������������������� ���������� �� ���� - ���� ����������� SPI ��������
	//
	SEQST_MAX		// �� ������������ � ����������.- ���������� ���������
};

// �������� ��� ������ �� ������� ���������� �������.
// ��������� ����������� � ������ �������: 0: keyup, 1: keydown
static const FLASHMEM uint_fast8_t seqtxgfi [SEQST_MAX] [2] =
{
	{ TXGFI_RX,			TXGFI_RX,		},	// SEQST_INITIALIZE,		// ������ �� ���������� - ��������� ��������� ����������
	{ TXGFI_TRANSIENT,	TXGFI_TRANSIENT, }, // SEQST_PUSHED_WAIT_ACK_TX,// ���, ���� � SW-2011-RDX ���������� ������ TX1 � ����� ���������� �� ��������.
	{ TXGFI_TRANSIENT,	TXGFI_TRANSIENT, }, // SEQST_WAIT_ACK_TX,		// ���� �������� �� �������� �� ���������������� ���������
	{ TXGFI_TRANSIENT,	TXGFI_TRANSIENT, }, // SEQST_PUSHED_HOLDTX,		// ���, ����� ������������ �������� ���� �� ��������
	{ TXGFI_TX_KEYUP,	TXGFI_TX_KEYDOWN, }, // SEQST_HOLDTX,			// ��� ���� �� �������� ������ �� ��������
	{ TXGFI_TX_KEYUP,	TXGFI_TX_KEYDOWN, }, // SEQST_RGBEEP,			// ����� ��������� �� ����, ����� ���������� �������� - ������������ ��������� ������� (roger beep)
	{ TXGFI_TRANSIENT,	TXGFI_TRANSIENT, },	// SEQST_PUSHED_SWITCHTORX,	// ���� �������� �� �� ���� �� ���������������� ���������
	{ TXGFI_TRANSIENT,	TXGFI_TRANSIENT, },	// SEQST_SWITCHTORX,		// �������� �������������������� ���������� �� ���� - ���� ����������� SPI ��������
};

// �������� ��� ������ �� ������� ���������� �������.
// ��������� ����������� � ������ �������: 0: keyup, 1: keydown
static const FLASHMEM uint_fast8_t seqtxstate [SEQST_MAX] =
{
	0,	// SEQST_INITIALIZE,			// ������ �� ���������� - ��������� ��������� ����������
	0,	// SEQST_PUSHED_WAIT_ACK_TX,	// ���, ���� � SW-2011-RDX ���������� ������ TX1 � ����� ���������� �� ��������.
	1,	// SEQST_WAIT_ACK_TX,			// ���� �������� �� �������� �� ���������������� ���������
	1,	// SEQST_PUSHED_HOLDTX,			// ���, ����� ������������ �������� ���� �� ��������
	1,	// SEQST_HOLDTX,				// ��� ���� �� �������� ������ �� ��������
	1,	// SEQST_RGBEEP,				// ����� ��������� �� ����, ����� ���������� �������� - ������������ ��������� ������� (roger beep)
	1,	// SEQST_PUSHED_SWITCHTORX,		// ���� �������� �� �� ���� �� ���������������� ���������
	0,	// SEQST_SWITCHTORX,			// �������� �������������������� ���������� �� ���� - ���� ����������� SPI ��������
};

#if WITHDEBUG

// �������� ��������� ���������� - ��� ���������� ������
static const char * FLASHMEM const seqnames [SEQST_MAX] =
{
	"SEQST_INITIALIZE",			// ������ �� ���������� - ��������� ��������� ����������
	"SEQST_PUSHED_WAIT_ACK_TX",	// ���, ���� � SW-2011-RDX ���������� ������ TX1 � ����� ���������� �� ��������.
	"SEQST_WAIT_ACK_TX",		// ���� �������� �� �������� �� ���������������� ���������
	"SEQST_PUSHED_HOLDTX",		// ���, ����� ������������ �������� ���� �� ��������
	"SEQST_HOLDTX[4]",			// ��� ���� �� �������� ������ �� ��������
	"SEQST_RGBEEP",				// ����� ��������� �� ����, ����� ���������� �������� - ������������ ��������� ������� (roger beep)
	"SEQST_PUSHED_SWITCHTORX",	// ���� �������� �� �� ���� �� ���������������� ���������
	"SEQST_SWITCHTORX",			// �������� �������������������� ���������� �� ���� - ���� ����������� SPI ��������
};

#endif /* WITHDEBUG */

// vox and mox parameters
// ����� ������������ 16-������ ����������
// ��� ��� ������������ �������� VOX - 2.5 ������� (500 �����),
// � ������������ �������� QSK - 1.6 ������� (320 �����).

static uint_fast16_t bkin_delay;	/* �������� ���������� qsk � ����� (������ TICKS_FREQUENCY) */
static uint_fast16_t vox_delay;	/* �������� ���������� vox � ����� (������ TICKS_FREQUENCY) */


static uint_fast16_t bkin_count;
static uint_fast16_t vox_count;

#if WITHVOX
static uint_fast8_t	vox_level;	/* ������� ������������ VOX */
static uint_fast8_t	avox_level;	/* ������� ������������ anti-VOX */
#endif /* WITHVOX */

static uint_fast8_t	seq_cwenable;		/* */
static uint_fast8_t	seq_voxenable;		/* */
static uint_fast8_t	seq_bkinenable;		/* */
static uint_fast8_t	seq_rgbeep;			/* ���������� ������������ roger beep */
//static uint_fast8_t	seq_rgbeeptype;		/* ��� roger beep */

// sequenser parameers
static uint_fast8_t rxtxticks;
static uint_fast8_t txrxticks;
static uint_fast8_t pretxticks;	// ����� ���������� ������ RX � ������ ������������ ������ ����� ����������� �� ��������.
static uint_fast16_t rgbeepticks;	// ����� ������������ roger beep 

static /* volatile */ uint_fast8_t exttunereq;	// ������ �� tune �� ���������������� ���������
static /* volatile */ uint_fast8_t ptt;	// ������ �� �������� �� ���������������� ���������
static /* volatile */ uint_fast8_t usertxstate;	/* 0 - ��������� ��������� � ��������� �����, ����� - � ��������� �������� */

static volatile uint_fast8_t seqstate;
static volatile uint_fast8_t seqpushtime;	// ��������� ��������� "�����" �� �������� � �� ����

static ticker_t seqticker;

static uint_fast8_t
getstablev8(volatile uint_fast8_t * p)
{
	uint_fast8_t v1 = * p;
	uint_fast8_t v2;
	do
	{
		v2 = v1;
		v1 = * p;
	} while (v2 != v1);
	return v1;

}

/* ��������� ���������� �� ���������� ������ �� ������������ �� �������� � �������� ��������� */
uint_fast8_t seq_get_txstate(void)
{
	return seqtxstate [getstablev8(& seqstate)];
}

#if WITHVOX


/* ���������� ������� ������������ vox � anti-vox */
/* TODO:	������ �����, ���������� ���������������� VOX ������ ������� ����� ������������, � ������� ���������� vox_probe(). 
*/
void vox_set_levels(uint_fast8_t vlevel, uint_fast8_t alevel)
{
	//disableIRQ();
	
	vox_level = vlevel;	// 0..100
	avox_level = alevel;	// 0..100

	//enableIRQ();
}

static int vscale(uint_fast8_t v, uint_fast8_t mag)
{
	return v * mag / 100;
}
 
/* ���������� ��� �������� ��������������� ������� ������� � anti-vox */
// ���������� ��� ����������� �����������.
void vox_probe(uint_fast8_t vlevel, uint_fast8_t alevel)
{
	// vlevel, alevel - �������� 0..UINT8_MAX
	if ((vscale(vlevel, vox_level) - vscale(alevel, avox_level) >= 16))
	{
		vox_count = vox_delay;
	}
}

/* ���������� (��-0) ��� ���������� (0) ������ vox. */
void 
vox_enable(
	uint_fast8_t voxstate, 			/* ���������� (��-0) ��� ���������� (0) ������ vox. */
	uint_fast8_t vox_delay_tens	/* �������� ���������� vox � 1/100 ��������� ���������� */
	)
{
	disableIRQ();
	vox_delay = NTICKS(10 * vox_delay_tens);
	seq_voxenable = voxstate;
	enableIRQ();
}

#endif

/* ���������� (��-0) ��� ���������� (0) ������ BREAK-IN. */
void 
seq_set_bkin_enable(
	uint_fast8_t bkinstate, 			/* ���������� (��-0) ��� ���������� (0) ������ BREAK-IN. */
	uint_fast8_t bkin_delay_tens	/* �������� ���������� break-in � 1/100 ��������� ���������� */
	)
{
	disableIRQ();
	bkin_delay = NTICKS(10 * bkin_delay_tens);
	seq_bkinenable = bkinstate;
	enableIRQ();
}

/* ���������� ������ CW */
void 
seq_set_cw_enable(
	uint_fast8_t state		/* ���������� (��-0) ��� ���������� (0) ������ CW. */
	)	
{
	disableIRQ();
	seq_cwenable = state;
	enableIRQ();
}

/* ���������� ������������ roger beep */
void 
seq_set_rgbeep(
	uint_fast8_t state		/* ���������� (��-0) ��� ���������� (0). */
	)	
{
	disableIRQ();
	seq_rgbeep = state;
	enableIRQ();
}


/* ���������� ������ �������� ������������ ����-��������. ���������� ��� ����������� �����������. */
void 
vox_initialize(void)
{
	//bkin_count = vox_count = vox_count_stk = 0;
	bkin_delay = NTICKS(250);
	vox_delay = NTICKS(1000); /* ���� ������� */
	//vox_level = avox_level = 0;
	//seq_voxenable = 0;
	//seq_cwenable = 0;
}

////////////////////////////////

enum { QUEUEUESIZE = 2 * (NTICKS(WITHMAXRXTXDELAY) +  NTICKS(WITHMAXTXRXDELAY) * 2 + 4) };

// ������� ������������ ��� ������ �����
static uint_fast8_t keyqueuebuff [(QUEUEUESIZE + 7) / 8];
static uint_fast8_t keyqueueput;
static uint_fast8_t keyqueueget;
//static uint_fast8_t keyqueuecnt;

static void keyqueuein(uint_fast8_t v)
{
	const uint_fast8_t next = (keyqueueput + 1) % QUEUEUESIZE;
	if (next != keyqueueget)
	{
		// ������� ������������ ��� ������ �����
		if (v != 0)
			keyqueuebuff [keyqueueput / 8] |= rbvalues [keyqueueput % 8];
		else
			keyqueuebuff [keyqueueput / 8] &= ~ rbvalues [keyqueueput % 8];
		keyqueueput = next;
		//keyqueuecnt += v;
	}	
	else
	{
#if WITHDEBUG
		debug_printf_P(PSTR("keyqueuein(%d) to full buffer, seqstate=%s\n"), v, seqnames [seqstate]);
#endif /* WITHDEBUG */
	}
}

// ������� ��� �� �������
// ������� ������ ���������� 0 ��� 1 � ����� �� ������ ��������
static uint_fast8_t keyqueueout(void)
{
	if (keyqueueget != keyqueueput)
	{
		// ������� ������������ ��� ������ �����
		const uint_fast8_t v = (keyqueuebuff [keyqueueget / 8] & rbvalues [keyqueueget % 8]) != 0;
		keyqueueget = (keyqueueget + 1) % QUEUEUESIZE;
		//keyqueuecnt -= v;
		return v;
	}
	return 0;
}

#if 0
// ���������� ��������� ����� � �������.
// ������� ������ ���������� 0 ��� �-����
static uint_fast8_t keyqueueact(void)
{
	return keyqueuecnt != 0;
}
#endif

// �������� �������
static void keyqueueclear(void)
{
	//keyqueuecnt = 0;
	keyqueueget = keyqueueput = 0;
	//dbg_putchar('x');
}

/* ��������� �������� ���������� ���������� ������� - ��� ���������� ������������� �������������������� ��������. */
static const FLASHMEM 
	portholder_t txgfva0 [TXGFI_SZIE] =	// ���������� ������� ��� ������ �������
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB }; // ��� SSB
static const FLASHMEM 
	uint_fast8_t sdtnva0 [TXGFI_SZIE] =	// �������� ��������� ������������ ��� ������ �������
		{ 0, 0, 0, 0 };	// ��� SSB

static const portholder_t FLASHMEM * txgfp = txgfva0;	// ��������� ���������� �������
static const uint_fast8_t FLASHMEM * sdtnp = sdtnva0;	// ��������� ���������� �������������

/* ��� �������� ����� � ������ ������ ������ �� ���������� */
void seq_set_txgate_P(
	const portholder_t FLASHMEM * atxgfp, 
	const uint_fast8_t FLASHMEM * asdtnp
	)
{
	disableIRQ();
	txgfp = atxgfp;	// ��������� ���������� �������
	sdtnp = asdtnp;	// ��������� ���������� �������������
	enableIRQ();
}	

/* � ���������� �� �������� ������ ���������� ������� ��� �������� (��� ��������) ��
   ��������� ��������. ��������� ������� ���������� ������� �������� � ������������.

   ���������� �� seq_spool_ticks().
*/
static uint_fast8_t 
//NOINLINEAT
seqhastxrequest(void)
{
	
	if (exttunereq)	
	{
		// ����� "��������� - �������� ������� - ��� �������� ����� AM
		return 1;
	}
	if (ptt)	
	{
		// ������ ��� ����� CAT
		return 1;
	}
	
	if (seq_cwenable)
	{
		// CW
		return (bkin_count != 0);
	}
	else
	{
		// SSB, AM, FM
		return seq_voxenable && vox_count != 0;
	}

	// ������ ���������� � ������ �������� ������ ���.
	// ��������� ���� ���� �� � ��������� � �����.
	return 0;
}

static void
seq_txpath_set(portholder_t txpathstate)
{
#if WITHINTEGRATEDDSP
	dsp_txpath_set(txpathstate);
#endif
	hardware_txpath_set(txpathstate);
}

// 
// ��������� ������������ ����-�������� � �����.
// ������������ �� �������� � ���� �������������� �� user-mode ��������
// (����� spi-����������� ����������). ����� ������������ �� ��������
// ���������� �������� �� ���������� ���� (��� ������) rxtxticks � ������� ����������
// ������ �� ���������� ������������ ������������ ��� ssb �������.
// ��� �������� �� ���� ������ ��������� ���������� ������, 
// ����� ����� �������� rxtxticks ������� �������
// � ���������� ����������� ���������� ����������������.
//
// hardware_txsignal_enable(txgate) - ���������� ���������� ������� �������
// board_sidetone_enable(tx) - ������ ������� ������������
//


// ���������� �� ����������� ����������. ���������� �������� ����� ������ ��� ����������
// ����������� ������ �� �������

static void 
seq_spool_ticks(void * ctc)
{
	// ��������� � ����� �������� ������ �� ������� ��������� �����������
	//
	const uint_fast8_t keydown = elkey_get_output();	// � ��� �� ��������� ������ �����������, ����������� �� CAT...
	if (keydown && seq_bkinenable && seq_cwenable)
	{
		bkin_count = bkin_delay;
	}

	// ������ ������� ������������ � ����������� �� ��������� ����������.
	// ������������ �� ����� ���������� ������, ����� ��������� ��� �� ��������� TUNE.
	board_sidetone_enable(sdtnp [seqtxgfi [seqstate] [keydown]] || keydown); // - ������ ������� ������������
	// ������������� ������ � ������ ����� ��������
	/* ����� ���������� */
	// ��������� ������� mox
	if (bkin_count != 0)
		-- bkin_count;
	// ��������� ������� vox
	if (vox_count != 0)
		-- vox_count;

	/* � ������� ������ ������ ���������� ���-���� */
	if (bkin_count && seq_cwenable)
		keyqueuein(keydown);	// � ������ break-in: ��������� ������� � ������� FIFO
	else if (seq_cwenable && seq_get_txstate())
		keyqueuein(keydown);	// � ������ ������� �������� �� ��������: ��������� ������� � ������� FIFO
	else
		keyqueuein(0);

	const uint_fast8_t keydowndly = keyqueueout();
	/* �������� �� ���������� ����������, � ������� ������������� ���� ���������� ������� */
	switch (seqstate)
	{
	case SEQST_INITIALIZE:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [0]]);	// - ���������� ���������� ������� �������
		// ��������� ��������� - ����������� ��������� �������� �� ������� � ����� ��������
		if (seqhastxrequest() != 0)
		{
			// ���� ������ �� �������� �������� - ��������� ��� ������������������ �������� �� �������� �� ��������.
			// �������� ������������� �� ��������
			if ((seqpushtime = pretxticks) == 0)
			{
				// ������� ������
				seqstate = SEQST_WAIT_ACK_TX;
			}
			else
			{
				// ���, ���� � SW-2011-RDX ���������� ������ TX1 � ����� ���������� �� ��������.
				seqstate = SEQST_PUSHED_WAIT_ACK_TX;	
			}
		}
		break;

	case SEQST_PUSHED_SWITCHTORX:	
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [0]]);	// - ���������� ���������� ������� �������
		// ���� �������� �� �� ���� �� ���������������� ���������
		// ��������� �������� ������� ��� ��������� ����� �����������.
		if (-- seqpushtime == 0)
		{
			seqstate = SEQST_SWITCHTORX;	// �� ��������� ����� ����� ����������� ����������� ���������.
		}
		break;


	case SEQST_PUSHED_HOLDTX:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [0]]);	// - ���������� ���������� ������� �������
		// ��������� �������� ������� ��� ��������� ����� �����������.
		// ���, ����� ������������ �������� ���� �� ��������
		if (-- seqpushtime == 0)
		{
			seqstate = SEQST_HOLDTX;	// �� ��������� ����� ����� ����������� ����������� ���������.
		}
		break;

	// ����� ��������� �� ����, ����� ���������� �������� - ������������ ��������� ������� (roger beep)
	case SEQST_RGBEEP:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [keydowndly]]);	// - ���������� ���������� ������� �������
		if (-- seqpushtime != 0)
		{
		}
		else if ((seqpushtime = txrxticks) == 0)
		{
			seqstate = SEQST_SWITCHTORX;
		}
		else
		{
			/* ��� ��������� ����� � ����������������� (����� SPI) �� ���� */
			seqstate = SEQST_PUSHED_SWITCHTORX;	// ���� �������� �� �� ���� �� ���������������� ���������
		}
	break;

	case SEQST_PUSHED_WAIT_ACK_TX:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [0]]);	// - ���������� ���������� ������� �������
		// ���, ���� � SW-2011-RDX ���������� ������ TX1 � ����� ���������� �� ��������.
		if (-- seqpushtime == 0)
		{
			seqstate = SEQST_WAIT_ACK_TX;	// �� ��������� ����� ����� ����������� ����������� ���������.
		}
		break;

	// ���� ��������. ��������� ����� ������ ����� ����������
	// �� �������� � ����� �����, ���������� ������� ������ ������ �����������
	// �� ������ "����" �������.
	case SEQST_HOLDTX:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [keydowndly]]);	// - ���������� ���������� ������� �������
		if (seqhastxrequest() == 0)
		{
			// ���� ������ �� �������� ����� - ��������� ��� ������������������ �������� �� �������� �� ����.
			// ���������, �� ���� �� ����������� roger beep
			if (seq_rgbeep != 0 && (seqpushtime == rgbeepticks) != 0)
			{
				seqstate = SEQST_RGBEEP;
			}
			else if ((seqpushtime = txrxticks) == 0)
			{
				seqstate = SEQST_SWITCHTORX;
			}
			else
			{
				/* ��� ��������� ����� � ����������������� (����� SPI) �� ���� */
				seqstate = SEQST_PUSHED_SWITCHTORX;	// ���� �������� �� �� ���� �� ���������������� ���������
			}
		}
		break;

	// �������� �������������������� ���������� �� �������� - ���� ����������� SPI ��������
	// ��� �������� user mode ��������� �� ��������
	case SEQST_WAIT_ACK_TX:
		if (usertxstate != 0)
		{
			// ��������� ������ �� user-mode ��������� - ������������ �� �������� ���������
			/* ��� � �������� ������� */
			if ((seqpushtime = rxtxticks) == 0)
			{
				seqstate = SEQST_HOLDTX;
			}
			else
			{
				seqstate = SEQST_PUSHED_HOLDTX;	// ���, ����� ������������ �������� ���� �� ��������
			}
		}
		break;

	case SEQST_SWITCHTORX:
		seq_txpath_set(txgfp [seqtxgfi [seqstate] [0]]);	// - ���������� ���������� ������� �������
		// �������� �������������������� ���������� �� ���� - ���� ����������� SPI ��������
	// �������� �������������������� ���������� �� ���� - ���� ����������� SPI ��������
	// ��� �������� user mode ��������� �� ����
		if (usertxstate == 0)
		{
			// ��� �� ��������������������� �� ����
			// ������� � ��������� ��������
			seqstate = SEQST_INITIALIZE;
		}
		break;
	}
}


/* ��������� ���� - ���������� �������� ��� �������� �� �������� � �������. */
void seq_set_rxtxdelay(
	uint_fast8_t rxtxdelay, 
	uint_fast8_t txrxdelay,
	uint_fast8_t pretxdelay
	)
{
	const uint_fast8_t arxtxticks = NTICKS(rxtxdelay); // �������� ��� �������� �� ��������
	const uint_fast8_t atxrxticks = NTICKS(txrxdelay); // � �������
	const uint_fast8_t apretxticks = NTICKS(pretxdelay); // �������� ����� ��������� �� ��������
	if (arxtxticks != rxtxticks || atxrxticks != txrxticks || apretxticks != pretxticks)
	{
		disableIRQ();
		rxtxticks = arxtxticks;
		txrxticks = atxrxticks;
		pretxticks = apretxticks;
		keyqueueclear();
		uint_fast8_t n = (pretxticks + rxtxticks) * 2;
		while (n --)
			keyqueuein(0);
		enableIRQ();
	}
}


/* ������������� ���������� � ������������ �����. ����������� ��� ����������� �����������. */
void seq_initialize(void)
{
	ticker_initialize(& seqticker, 1, seq_spool_ticks, NULL);

	hardware_ptt_port_initialize();		// ������������� ������ ���������� ������� �������� � ������� ��������

	hardware_txpath_initialize();
	//seq_set_txgate_P(txgfva0, sdtnva0);	// ������� ����������� ��������������
	seq_txpath_set(TXGFV_RX);	// - ���������� ���������� ������� ������� - � ��������� ����
	board_sidetone_enable(0); // - ���������� ������ ������� ������������


	rxtxticks = NTICKS(WITHMAXRXTXDELAY);	// 15 ms �������� ��� �������� �� ��������
	txrxticks = NTICKS(WITHMAXTXRXDELAY);	// 15 ms �������� ��� �������� �� ����
	pretxticks = NTICKS(WITHMAXTXRXDELAY);

	keyqueueclear();
	uint_fast8_t n = (pretxticks + rxtxticks) * 2;	// ��������� �� 2 ������ "�� �����", � ��������� ���������� ��������� ����� �������� ������ ���, ��� � �������.
	while (n --)
		keyqueuein(0);
	
	rgbeepticks = NTICKS(200);	// ������������ roger beep
	////exttunereq =
	////usertxreq =
	////usertxstate = 0;

	seqstate = SEQST_INITIALIZE;

	// compile-time check
	if (QUEUEUESIZE > 256)
	{
		extern void wrong_QUEUEUESIZE_holder_type(void);
		wrong_QUEUEUESIZE_holder_type();
		for (;;)
			;
	}
}

/* ������� ����������� ������� �� ����� �������. ���������� �� user-mode ��������� */
void seq_purge(void)
{
	//disableIRQ();
	//enableIRQ();
}

// ������ �� user-mode ����� ��������� �� ������� �� �������� ��� tune.
void seq_txrequest(uint_fast8_t tune, uint_fast8_t aptt)
{
	disableIRQ();
	exttunereq = tune;
	ptt = aptt;
	enableIRQ();
}

/* ������������� �� user-mode ��������� � ���, ��� ����� ������ ����-�������� ������������ */
void seq_ask_txstate(
	uint_fast8_t tx)	/* 0 - ��������� ��������� � ��������� �����, ����� - � ��������� �������� */
{
	disableIRQ();

	usertxstate = tx;

	enableIRQ();
}



#else	/* WITHTX */

static void 
seq_spool_ticks(void * ctc)
{
	const uint_fast8_t keydown = elkey_get_output();	// � ��� �� ��������� ������ �����������, ����������� �� CAT...

	// ������ ������� ������������ � ����������� �� ��������� ����������.
	// ������������ �� ����� ���������� ������, ����� ��������� ��� �� ��������� TUNE.
	board_sidetone_enable(keydown); // - ������ ������� ������������
}


/* �������� ������� ��� ������ � ������ ������ ��������. */
void seq_txrequest(uint_fast8_t tune, uint_fast8_t ptt)
{
}

uint_fast8_t seq_get_txstate(void)
{
	return 0;
}

void seq_ask_txstate(uint_fast8_t tx)
{
}

/* ������������� ���������� � ������������ �����. ����������� ��� ����������� �����������. */
void seq_initialize(void)
{
	ticker_initialize(& seqticker, 1, seq_spool_ticks, NULL);
}

/* ������� ����������� ������� �� ����� �������. ���������� �� user-mode ��������� */
void seq_purge(void)
{
}

void 
vox_initialize(void)
{
}

void 
seq_set_bkin_enable(
	uint_fast8_t bkinstate, 			/* ���������� (��-0) ��� ���������� (0) ������ BREAK-IN. */
	uint_fast8_t bkin_delay_tens	/* �������� ���������� break-in � 1/100 ��������� ���������� */
	)
{
}


#endif	/* WITHTX */
