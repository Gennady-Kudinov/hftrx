/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "synthcalcs.h"
#include "board.h"
#include "keyboard.h"
#include "encoder.h"
#include "list.h"

#include "display.h"
#include "formats.h"

#include "spifuncs.h"

#include "audio.h"

#include "tlv320aic23.h"	// ��������� ���������� ��������� ������
#include "nau8822.h"

#if WITHUSEAUDIOREC || WITHUSESDCARD
	#include "sdcard.h"
#endif /* WITHUSEAUDIOREC || WITHUSESDCARD */

#include <string.h>
#include <ctype.h>
//#include <math.h>

#if WITHSECTOGGLE
static uint_fast8_t sectoggle;
#endif /* WITHSECTOGGLE */

#if WITHRFSG
	#error WITHRFSG now not supported
#endif /* WITHRFSG */

static uint_fast32_t 
//NOINLINEAT
nextfreq(uint_fast32_t oldfreq, uint_fast32_t freq, 
							   uint_fast32_t step, uint_fast32_t top);
static uint_fast32_t 
//NOINLINEAT
prevfreq(uint_fast32_t oldfreq, uint_fast32_t freq, 
							   uint_fast32_t step, uint_fast32_t bottom);

extern volatile uint_fast8_t spool_lfm_enable;
extern volatile uint_fast8_t spool_lfm_flag;


static uint_fast8_t
getstablev8(volatile uint_fast8_t * p)
{
	if (sizeof * p == 1)
		return * p;
	else
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
}

static uint_fast16_t
getstablev16(volatile uint_fast16_t * p)
{
	if (sizeof * p == 1)
		return * p;
	else
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
}

static void 
display_redrawbars(
	uint_fast8_t immed,	// ����������� ����������� �����������
	uint_fast8_t extra		/* ��������� � ������ ����������� �������� */
	);
static void 
display_redrawmodes(
	uint_fast8_t immed	// ����������� ����������� �����������
	);


static uint_fast8_t local_isdigit(char c)
{
	//return isdigit((unsigned char) c) != 0;
	return c >= '0' && c <= '9';
}

static uint_fast8_t gtx;	/* ������� ��������� ����� ��� �������� */
static uint_fast8_t gcwpitch10 = 700 / CWPITCHSCALE;	/* ��� ��� ������ ��������� ��� ������������ (� �������� ����) */
#if WITHIF4DSP
static uint_fast8_t gsquelch;	/* squelch level */
#endif /* WITHIF4DSP */
#if WITHDSPEXTDDC	/* "��������" � DSP � FPGA */

#endif /* WITHDSPEXTDDC */

//#define DEBUGEXT 1

static void processtxrequest(void);	/* ��������� ���������� ������� �� ��������.	*/

static void processmessages(uint_fast8_t * kbch, uint_fast8_t * kbready, uint_fast8_t menu);

static uint_fast8_t getbankindex_raw(uint_fast8_t pathi);
static uint_fast8_t getbankindex_ab(uint_fast8_t ab);
static uint_fast8_t getbankindex_pathi(uint_fast8_t pathi);
static uint_fast8_t getbankindex_tx(uint_fast8_t tx);
static uint_fast8_t getbankindex_ab_fordisplay(uint_fast8_t ab);
static void updateboard(uint_fast8_t full, uint_fast8_t mute);
static uint_fast8_t getsubmode(uint_fast8_t bi);		/* bi: vfo bank index */
static uint_fast8_t getactualmainsubrx(void);

#if WITHCAT

static void processcat_enable(uint_fast8_t catenable);
static void NOINLINEAT cat_answer_request(uint_fast8_t catindex);	// call from user-mode

static uint_fast8_t aistate;		/* autoinformation state flag */
#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)
static uint_fast8_t rmstate;		/* RM answer state type (1..3) */
#endif /* WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */

// add/remove codes: modify also catanswers table.
enum
{
#if WITHCATEXT
#if WITHELKEY
	CAT_KY_INDEX,		// kyanswer()	// ����������� ������ ��� ��������� �������
	CAT_KS_INDEX,		// ksanswer()
#endif /* WITHELKEY */
	CAT_PS_INDEX,		// psanswer()
	CAT_SM_INDEX,		// smanswer()
	CAT_RA_INDEX,		// raanswer()
	CAT_PA_INDEX,		// paanswer()
#if WITHANTSELECT
	CAT_AN_INDEX,		// ananswer()
#endif /* WITHANTSELECT */
#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)
	CAT_RM1_INDEX,		// rm1answer()
	CAT_RM2_INDEX,		// rm2answer()
	CAT_RM3_INDEX,		// rm3answer()
#endif /* WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */
#endif /* WITHCATEXT */
	CAT_ID_INDEX,		// idanswer()	
	CAT_FV_INDEX,		// fvanswer()
	CAT_DA_INDEX,		// daanswer()
	CAT_FA_INDEX,		// faanswer()
	CAT_FB_INDEX,		// fbanswer()
	CAT_SP_INDEX,		// spanswer()
	CAT_FR_INDEX,		// franswer()
	CAT_FT_INDEX,		// ftanswer()
	CAT_MD_INDEX,		// mdanswer()
	CAT_TX_INDEX,		// txanswer()
	CAT_RX_INDEX,		// rxanswer()
	CAT_AI_INDEX,		// aianswer()
	CAT_PT_INDEX,		// ptanswer()
	CAT_IF_INDEX,		// ifanswer()
	CAT_FW_INDEX,		// fwanswer()
#if WITHIF4DSP//CTLSTYLE_V1D || CTLSTYLE_OLEG4Z_V1 || 1
	CAT_ZZ_INDEX,		// zzanswer()
#endif /* CTLSTYLE_V1D || CTLSTYLE_OLEG4Z_V1 */
#if WITHIF4DSP
	CAT_SQ_INDEX,		// sqanswer()
#endif /* WITHIF4DSP */
	CAT_BADCOMMAND_INDEX,		// badcommandanswer()
	//
	CAT_MAX_INDEX
};

static uint_fast8_t cat_answer_map [CAT_MAX_INDEX];
static uint_fast8_t cat_answerparam_map [CAT_MAX_INDEX];


static uint_fast32_t savedbaudrate;	// ��������, �� ������� � ��������� ��� ��� ���������������� ����.

static void 
cat_set_speed(uint_fast32_t baudrate)
{
	/* �� ATMega � ATXMega �������������������� �������� ���� ��� ����� ���� ��� �������� �� �����������. */
	if (savedbaudrate == baudrate)
		return;
	savedbaudrate = baudrate;
	HARDWARE_CAT_SET_SPEED(baudrate);
}

#endif /* WITHCAT */


enum {
	VFOMODES_VFOINIT,
	VFOMODES_VFOSPLIT,
	//
	VFOMODES_COUNT
};

/*
	���� ���������� ���������� ������� ��� �� �����
		AGC_CODE_SLOW = 0x04, //0x07,
		AGC_CODE_MED  = 0x02,	//0x03,
		AGC_CODE_FAST = 0x01, //0x01,
		AGC_CODE_OFF = 0x00

*/

#if WITHNOATTNOPREAMP

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */
	static const FLASHMEM struct {
		unsigned char codeatt;	// ������� ��������� �����������
		unsigned char codepre;	// ������� ��������� �������������
		char label [1];
	}  attmodes [] =
	{
		{ 0, 0, "" },
	};

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */
	static const FLASHMEM struct {
		unsigned char code;
		char label [4];
	}  pampmodes [] =
	{
		{ 0, "   " },	// ��� ������� ����� ��� �������� ������� OVF ���� ������������ ��������� � ����� ����� � PRE
	};

#elif WITHONEATTONEAMP
	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */
	static const FLASHMEM struct {
		unsigned char codeatt;	// ������� ��������� �����������
		unsigned char codepre;	// ������� ��������� �������������
		char label [4];
	}  attmodes [] =
	{
		{ 0, 0, "   " },
		{ 0, 1, "PRE" },
		{ 0, 0, "   " },
		{ 1, 0, "ATT" },
	};

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */
	static const FLASHMEM struct {
		unsigned char code;
		char label [4];
	}  pampmodes [] =
	{
		{ 0, "   " },	// ��� ������� ����� ��� �������� ������� OVF ���� ������������ ��������� � ����� ����� � PRE
	};

#elif WITHPREAMPATT2_6DB
	/* ���������� ��� � ������������� ������������ � ����������� 0 - 6 - 12 - 18 dB */

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */

	static const FLASHMEM struct {
		unsigned char code;
		char label [5];
	}  attmodes [] =
	{
		{ 0, "    " },
		{ 1, " 6dB" },
		{ 2, "12dB" },
		{ 3, "18dB" },
	};

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */
	static const FLASHMEM struct {
		unsigned char code;
		char label [4];
	}  pampmodes [] =
	{
		{ 0, "   " },
		{ 1, "PRE" },
	};

#elif WITHPREAMPATT2_10DB
	/* ���������� ��� � ������������� ������������ � ����������� 0 - 10 - 20 - 30 dB */

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */

	static const FLASHMEM struct {
		unsigned char code;
		char label [5];
	}  attmodes [] =
	{
		{ 0, "    " },
		{ 1, "10dB" },
		{ 2, "20dB" },
		{ 3, "30dB" },
	};

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */
	static const FLASHMEM struct {
		unsigned char code;
		char label [4];
	}  pampmodes [] =
	{
		{ 0, "   " },
		{ 1, "PRE" },
	};

#elif WITHATT2_6DB
	/* ���������� ������������� ������������ � ����������� 0 - 6 - 12 - 18 dB ��� ��� */

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */

	static const FLASHMEM struct {
		unsigned char code;
		char label [5];
	}  attmodes [] =
	{
		{ 0, "    " },
		{ 1, " 6dB" },
		{ 2, "12dB" },
		{ 3, "18dB" },
	};

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */
	static const FLASHMEM struct {
		unsigned char code;
		char label [4];
	}  pampmodes [] =
	{
		{ 0, "   " },	// ��� ������� ����� ��� �������� ������� OVF ���� ������������ ��������� � ����� ����� � PRE
	};
#elif WITHATT2_10DB
	/* ���������� ������������� ������������ � ����������� 0 - 10 - 20 - 30 dB ��� ��� */

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */

	static const FLASHMEM struct {
		unsigned char code;
		char label [5];
	}  attmodes [] =
	{
		{ 0, "    " },
		{ 1, "10dB" },
		{ 2, "20dB" },
		{ 3, "30dB" },
	};

	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */
	static const FLASHMEM struct {
		unsigned char code;
		char label [4];
	}  pampmodes [] =
	{
		{ 0, "   " },	// ��� ������� ����� ��� �������� ������� OVF ���� ������������ ��������� � ����� ����� � PRE
	};
#elif WITHATT1PRE1

	static const FLASHMEM struct {
		unsigned char code;
		char label [4];
	}  attmodes [] =
	{
		{ 0, "   " },
		{ 1, "ATT" },
	};


	/* ������, ��������� �� ��������� ��� ����������� �������.
	 */
	static const FLASHMEM struct {
		unsigned char code;
		char label [4];
	}  pampmodes [] =
	{
		{ 0, "   " },
		{ 1, "PRE" },
	};
#else
	#error ATT/PREAMP mode undefined
#endif
/* ������, ��������� �� ��������� ��� ����������� �������.
 */
#if CTLREGMODE24_RK4CI	/* ����������� ������� 24 ���� - "�������" RK4CI */
	/* ������������ ���� ��������� ������� ���
	 */
	enum { 
		AGCMODE_LONG = 0, 
		AGCMODE_SLOW = 0, 
		AGCMODE_MED, 
		AGCMODE_FAST, 
		AGCMODE_OFF
		//
		};

	static const FLASHMEM struct {
		uint_fast8_t code;
		char label4 [5];
		char label3 [4];
	}  agcmodes [] =
	{
		{ BOARD_AGCCODE_SLOW, "SLOW", "SLO"  },	// 3
		{ BOARD_AGCCODE_MED,  "MED ", "MED" },	// 2
		{ BOARD_AGCCODE_FAST, "FAST", "FST" },	// 1
		{ BOARD_AGCCODE_OFF,  "AGC-", "OFF" },	// 4 ?
	};
#elif CTLREGMODE24_RK4CI_V1	/* ����������� ������� 32 ���� (� 3*ULN2003) - "�������" RK4CI */
	/* ������������ ���� ��������� ������� ���
	 */
	enum { 
		AGCMODE_LONG = 0, 
		AGCMODE_SLOW = 0, 
		AGCMODE_MED, 
		AGCMODE_FAST, 
		AGCMODE_OFF
		//
		};

	static const FLASHMEM struct {
		uint_fast8_t code;
		char label4 [5];
		char label3 [4];
	}  agcmodes [] =
	{
		{ BOARD_AGCCODE_SLOW, "SLOW", "SLO"  },	// 3
		{ BOARD_AGCCODE_MED,  "MED ", "MED" },	// 2
		{ BOARD_AGCCODE_FAST, "FAST", "FST" },	// 1
		{ BOARD_AGCCODE_OFF,  "AGC-", "OFF" },	// 4 ?
	};
#elif WITHAGCMODENONE
	/* ������������ ���� ��������� ������� ���
	 */
	enum { 
		AGCMODE_LONG = 0, 
		AGCMODE_SLOW = 0, 
		AGCMODE_MED = 0, 
		AGCMODE_FAST = 0
		};
	static const FLASHMEM struct {
		uint_fast8_t code;
		char label4 [5];
		char label3 [4];
	}  agcmodes [] =
	{
		{ 0x00, "    ", "   " },
	};
#elif WITHAGCMODEONOFF
	/* ������������ ���� ��������� ������� ���
	 */
	enum { 
		AGCMODE_LONG = 0, 
		AGCMODE_SLOW = 0, 
		AGCMODE_MED = 0, 
		AGCMODE_FAST = 0, 
		//
		AGCMODE_OFF
		//
		};
	static const FLASHMEM struct {
		uint_fast8_t code;
		char label4 [5];
		char label3 [4];
	}  agcmodes [] =
	{
		{ BOARD_AGCCODE_ON,  "AGC ", "AGC" },
		{ BOARD_AGCCODE_OFF, "OFF ", "OFF" },
	};
#elif WITHAGCMODESLOWFAST	/* ����� ������� ��� ���� ������ ������-�������� */
	/* ������������ ���� ��������� ������� ���
	 */
	enum { 
		AGCMODE_LONG = 0, 
		AGCMODE_SLOW = 0, 
		AGCMODE_MED = 1, 
		AGCMODE_FAST = 1
		};
	static const FLASHMEM struct {
		uint_fast8_t code;
		char label4 [5];
		char label3 [4];
	}  agcmodes [] =
	{
		{ BOARD_AGCCODE_SLOW, "SLOW", "SLO" },
		{ BOARD_AGCCODE_FAST, "FAST", "FST" },
	};
#elif WITHAGCMODE4STAGES
	/* ������������ ���� ��������� ������� ���
	   ���������� �� ������
	 */
	enum { 
		AGCMODE_LONG, 
		AGCMODE_SLOW, 
		AGCMODE_MED, 
		AGCMODE_FAST, 
		AGCMODE_OFF
		//
		};
	static const FLASHMEM struct {
		uint_fast8_t code;
		char label4 [5];
		char label3 [4];
	}  agcmodes [] =
	{
		{ BOARD_AGCCODE_LONG, "LONG", "lng" },	// 4
		{ BOARD_AGCCODE_SLOW, "SLOW", "slo" },	// 2
		{ BOARD_AGCCODE_MED,  "MED ", "med" },	// 1
		{ BOARD_AGCCODE_FAST, "FAST", "fst" },	// 0
	};
#elif WITHAGCMODE5STAGES
	/* ������������ ���� ��������� ������� ���
	 */
	enum { 
		AGCMODE_LONG, 
		AGCMODE_SLOW, 
		AGCMODE_MED, 
		AGCMODE_FAST, 
		AGCMODE_OFF
		//
		};
	static const FLASHMEM struct {
		uint_fast8_t code;
		char label4 [5];
		char label3 [4];
	}  agcmodes [] =
	{
		{ BOARD_AGCCODE_LONG, "LONG", "lng" },	// 4
		{ BOARD_AGCCODE_SLOW, "SLOW", "slo" },	// 2
		{ BOARD_AGCCODE_MED,  "MED ", "med" },	// 1
		{ BOARD_AGCCODE_FAST, "FAST", "fst" },	// 0
		{ BOARD_AGCCODE_OFF,  "AGC-", "off" },	// 8
	};
#else
	#error WITHAGCMODExxxx undefined
#endif

#if WITHANTSELECT
	enum { NANTENNAS = 2 };		// �������� ���� �� ���� ������
#else /* WITHANTSELECT */
	enum { NANTENNAS = 1 };		// e����������� �������
#endif /* WITHANTSELECT */

#if WITHANTSELECT

static const FLASHMEM struct {
	uint_fast8_t code [2];	// RX/TX antenna
	char label5 [6];
}  antmodes [] =
{
#if WITHTX
	{	{ 0, 0, },	"1R-1T" },
	{	{ 1, 1, },	"2R-2T" },
	{	{ 0, 1, },	"1R-2T" },
	{	{ 1, 0, },	"2R-1T" },
#else /* WITHTX */
	{	{ 0, 0, },	"ANT1 " },
	{	{ 1, 1, },	"ANT2 " },
#endif /* WITHTX */
};

#endif /* WITHANTSELECT */

#if WITHPOWERLPHP

static const FLASHMEM struct {
	uint_fast8_t code;
	char label [5];
}  pwrmodes [] =
{
	{ WITHPOWERTRIMMIN, "LP" },
	{ WITHPOWERTRIMMAX, "HP" },
};

#endif /* WITHPOWERLPHP */

static const FLASHMEM struct {
	uint_fast8_t code;
	char label [5];
}  notchmodes [] =
{
	{ 0, "     " },
	{ 1, "NOTCH" },
#if WITHLMSAUTONOTCH
	{ 2, "ANTCH" },
#endif /* WITHLMSAUTONOTCH */
};

#if WITHUSEDUALWATCH

static const FLASHMEM struct {
	uint_fast8_t code;
	char label [5];
}  mainsubrxmodes [] =
{
	{ BOARD_RXMAINSUB_A_B, "A/B", },	// �����/������, A - main RX, B - sub RX
	{ BOARD_RXMAINSUB_B_A, "B/A", },
	{ BOARD_RXMAINSUB_B_B, "B/B", },	
	{ BOARD_RXMAINSUB_A_A, "A/A", },		
	{ BOARD_RXMAINSUB_TWO, "A&B", },	// � ��� ����������� ��������� ����� ������� ����������.		
};

#endif /* WITHUSEDUALWATCH */

#define PWRMODE_COUNT (sizeof pwrmodes / sizeof pwrmodes [0])
#define NOTCHMODE_COUNT (sizeof notchmodes / sizeof notchmodes [0])
#define PAMPMODE_COUNT (sizeof pampmodes / sizeof pampmodes [0])
#define ATTMODE_COUNT (sizeof attmodes / sizeof attmodes [0])
#define ANTMODE_COUNT (sizeof antmodes / sizeof antmodes [0])
#define AGCMODE_COUNT (sizeof agcmodes / sizeof agcmodes [0])
#define MAINSUBRXMODE_COUNT (sizeof mainsubrxmodes / sizeof mainsubrxmodes [0])

#define MENUNONVRAM ((nvramaddress_t) ~ 0)		// ����� �����, ��� �� ������������� �� ������ �������������� ���������.


// ������������ ������� ������� � NVRAM
static uint_fast8_t 
//NOINLINEAT
loadvfy8up(
	nvramaddress_t place, 
	uint_fast8_t bottom, uint_fast8_t upper, uint_fast8_t def)	// upper - inclusive limit
{
	if (place == MENUNONVRAM)
		return def;
	const uint_fast8_t v = restore_i8(place);

	// pre-chechk default value added for mode row switching with same column as default
	if (def > upper || def < bottom)
		def = bottom;

	if (v > upper || v < bottom)
	{

		save_i8(place, def);
		return def;
	}
	return v;
}

// ������������ ������� ������� � NVRAM

static uint_fast16_t 
//NOINLINEAT
loadvfy16up(
	nvramaddress_t place, 
	uint_fast16_t bottom, uint_fast16_t upper, uint_fast16_t def)	// upper - inclusive limit
{
	const uint_fast16_t v = restore_i16(place);

	if (def > upper || def < bottom)
		def = bottom;
	
	if (v > upper || v < bottom)
	{
		save_i16(place, def);
		return def;
	}
	return v;
}

// �������� � ���������� � ���������� �������� ��������, ��������� �� eeprom ��� �������� �� CAT. ��� ��� autosplit
static uint_fast32_t 
//NOINLINEAT
vfy32up(
	uint_fast32_t v,
	uint_fast32_t bottom, uint_fast32_t upper, uint_fast32_t def)
{

	if (def > upper || def < bottom)
		def = bottom;
	if (v > upper || v < bottom)
		v = def;
	return v;
}



static uint_fast32_t 
//NOINLINEAT
loadvfy32(
	nvramaddress_t place, 
	uint_fast32_t bottom, uint_fast32_t top, uint_fast32_t def)
{
	const uint_fast32_t v = restore_i32(place);

	if (def >= top || def < bottom)
		def = bottom;
	
	if (v >= top || v < bottom)
	{
		save_i32(place, def);
		return def;
	}
	return v;
}


#if WITHIF4DSP

// ������� ���������������

typedef struct mikproc_tag
{
	uint_fast8_t comp;
	uint_fast8_t complevel;
} mikproc_t;

/* �������� � NVRAM ��������� ��������� ������� ����� ����������� */
struct micproc
{
	uint8_t comp;
	uint8_t complevel;
} ATTRPACKED;	// �������� GCC, ��������� "����" � ���������. ��� ��� � ��� ��� ����� ���� ���������, see also NVRAM_TYPE_BKPSRAM


static mikproc_t micprofiles [] =
{
	{
		0,
		0,
	},
	{
		0,
		0,
	},
};

#define NMICPROFILES (sizeof micprofiles / sizeof micprofiles [0])

// ������ ������ ����� ���������� ��������� ����� ��� ������� ������
enum
{
	TXAPROFIG_CW,
	TXAPROFIG_SSB,
	TXAPROFIG_DIGI,
	TXAPROFIG_AM,
	TXAPROFIG_NFM,
	TXAPROFIG_DRM,
	TXAPROFIG_WFM,
	//
	TXAPROFIG_count
};


static uint_fast8_t gtxaprofiles [TXAPROFIG_count];	// ������ ������� ��� ������ ������� �������� - �������� 0..NMICPROFILES-1


// ��������� ������� �� ������

#define AFRESPONCESHIFT 192
#define AFRESPONCEMIN 0
#define AFRESPONCEMAX (AFRESPONCESHIFT + 18)	// -96..+18

static int_fast32_t getafresponcebase(void)
{
	return - AFRESPONCESHIFT;
}

enum
{
	BWSET_NARROW,	// ��������� ������ ����������� - ��� ��������� �������� ������ �����������
	BWSET_WIDE		// ��������� ������ ����������� - ���� ������ ����/������� ����
};


enum
{
	BWSET_WIDTHS = 3	// ������������ ���������� ����� ����������� � ����� bwsetsc_t
};

// ������ ����� ����� ����������� � ��������� ��� ��� ������� ������
enum
{
	BWSETI_CW,
	BWSETI_SSB,
	BWSETI_DIGI,
	BWSETI_AM,
	BWSETI_NFM,
	BWSETI_DRM,
	BWSETI_WFM,
	//
	BWSETI_count
};

/*
	�� ������:
	#define WITHAMHIGH100MIN	(800 / 100)		// ����������� ������� ���������������� ����� � ������ ����
	#define WITHAMHIGH100MAX	(6000 / 100)	// ������������ ������� ���������������� ����� � ������ ����
	#define WITHAMHIGH100DEF	(3800 / 100)	// ��������� �������� � ������ ����
*/


static const char FLASHMEM 
	strFlashWFM [] = "WFM",
	strFlashWide [] = "WID",
	strFlashNarrow [] = "NAR",
	strFlashNormal [] = "NOR";


/* �� ������ ����������� ���� �� ������ � ������������� ������� */
static uint_fast8_t	gssbtxhighcut100 = 3400 / BWGRANHIGH;	/* ������� ������� ����� ������� �� �� �������� */
static uint_fast8_t	gssbtxlowcut10 = 300 / BWGRANLOW;		/* ������ ������� ����� ������� �� �� �������� */

// ������� ����������� ������ ������ �����������
typedef struct
{
	uint_fast16_t granulation;
	uint_fast8_t left10_width10_low, left10_width10_high;	// ������� ��������� ����������
	uint_fast8_t right100_low, right100_high;	// ������� ��������� ����������
} bwlimits_t;

typedef struct
{
	const bwlimits_t * limits;
	uint_fast8_t type;		// BWSET_NARROW/BWSET_WIDE
	uint_fast8_t left10_width10, right100;	// left ��������� ���� width ��� ����������� (BWSET_NARROW) ��������
	//uint_fast8_t fltsofter;
	uint_fast8_t afresponce;	/* ���� ��� - �� Samplerate/2 ��� ���������� �� ������� �������  */
} bwprop_t;

// ������� ������ ������ �����������
typedef struct
{
	uint8_t last;		// 0..BWSET_WIDTHS-1
	bwprop_t  * prop [BWSET_WIDTHS];				// ������� � ������ �������� ��������
	const char FLASHMEM * labels [BWSET_WIDTHS];	// �������� ��������
} bwsetsc_t;

static const bwlimits_t bwlimits_cw = { 1, 10, 180, 0, 0,  };
static const bwlimits_t bwlimits_am = { 1, WITHAMLOW10MIN, WITHAMLOW10MAX, WITHAMHIGH100MIN, WITHAMHIGH100MAX,  };
static const bwlimits_t bwlimits_ssb = { 1, WITHSSBLOW10MIN, WITHSSBLOW10MAX, WITHSSBHIGH100MIN, WITHSSBHIGH100MAX, };
static const bwlimits_t bwlimits_wfm = { 1, WITHWFMLOW10MIN, WITHWFMLOW10MAX, WITHWFMHIGH100MIN, WITHWFMHIGH100MAX, };

// ������� ������ ������ �����������
// ��� �������� ����� ���������������� ����� ����
static bwprop_t bwprop_cwnarrow = { & bwlimits_cw, BWSET_NARROW, 200 / BWGRANLOW, 0, - 0 + AFRESPONCESHIFT, };
static bwprop_t bwprop_cwwide = { & bwlimits_cw, BWSET_NARROW, 500 / BWGRANLOW, 0, - 0 + AFRESPONCESHIFT, };
static bwprop_t bwprop_ssbwide = { & bwlimits_ssb, BWSET_WIDE, 300 / BWGRANLOW, 3400 / BWGRANHIGH, - 36 + AFRESPONCESHIFT,	};
static bwprop_t bwprop_ssbnarrow = { & bwlimits_ssb, BWSET_WIDE, 400 / BWGRANLOW, 2900 / BWGRANHIGH, - 36 + AFRESPONCESHIFT,	};
static bwprop_t bwprop_amwide = { & bwlimits_ssb, BWSET_WIDE, 100 / BWGRANLOW, 4000 / BWGRANHIGH, - 36 + AFRESPONCESHIFT,	};
static bwprop_t bwprop_amnarrow = { & bwlimits_am, BWSET_WIDE, 100 / BWGRANLOW, 3400 / BWGRANHIGH, - 36 + AFRESPONCESHIFT,	};
static bwprop_t bwprop_digiwide = { & bwlimits_ssb, BWSET_WIDE, 300 / BWGRANLOW, 3400 / BWGRANHIGH, - 0 + AFRESPONCESHIFT,	};
static bwprop_t bwprop_nfmnarrow = { & bwlimits_am, BWSET_WIDE, 300 / BWGRANLOW, 3400 / BWGRANHIGH, - 36 + AFRESPONCESHIFT,	};
static bwprop_t bwprop_nfmwide = { & bwlimits_am, BWSET_WIDE, 300 / BWGRANLOW, 4000 / BWGRANHIGH, - 36 + AFRESPONCESHIFT,	};
static bwprop_t bwprop_wfm = { & bwlimits_wfm, BWSET_WIDE, 100 / BWGRANLOW, 12000 / BWGRANHIGH, + 18 + AFRESPONCESHIFT,	};

// ������ ������������� ������ � ���������� �������� ������ �����������,
// � ��� �� �������� ����� ����������� ��� �����������
static const FLASHMEM bwsetsc_t bwsetsc [BWSETI_count] =
{
	{ 2, { & bwprop_cwwide, & bwprop_cwnarrow, & bwprop_ssbwide, }, { strFlashWide, strFlashNarrow, strFlashNormal, }, },	// BWSETI_CW
	{ 1, { & bwprop_ssbwide, & bwprop_ssbnarrow, }, { strFlashWide, strFlashNarrow, }, },	// BWSETI_SSB
	{ 0, { & bwprop_digiwide, }, { strFlashNormal, }, },	// BWSETI_SSB
	{ 1, { & bwprop_amwide, & bwprop_amnarrow, }, { strFlashWide, strFlashNarrow, }, },	// BWSETI_AM
	{ 1, { & bwprop_nfmwide, & bwprop_nfmnarrow, }, { strFlashWide, strFlashNarrow, }, },	// BWSETI_NFM
	{ 0, { & bwprop_amwide, }, { strFlashNormal, }, },	// BWSETI_DRM
	{ 0, { & bwprop_wfm, }, { strFlashWFM, }, },	// BWSETI_WFM
};

// ��������� ������ ����������� � ������ ������
static uint_fast8_t bwsetpos [BWSETI_count];

// ������ ����� ����� ����������� ��� ������� ������
enum
{
	BWPROPI_CWNARROW,
	BWPROPI_CWWIDE,	
	BWPROPI_SSBWIDE,	
	BWPROPI_SSBNARROW,	
	BWPROPI_AMWIDE,	
	BWPROPI_AMNARROW,	
	BWPROPI_DIGIWIDE,	
	BWPROPI_NFMNARROW,
	BWPROPI_NFMWIDE,	
	//
	BWPROPI_count
};

// ������������ ��� ������ � NVRAN ����������� ��������
static bwprop_t * const FLASHMEM bwprops [BWPROPI_count] =
{
	& bwprop_cwnarrow,	// BWPROPI_CWNARROW,
	& bwprop_cwwide,	// BWPROPI_CWWIDE,	
	& bwprop_ssbwide,	// BWPROPI_SSBWIDE,	
	& bwprop_ssbnarrow,	// BWPROPI_SSBNARROW
	& bwprop_amwide,	// BWPROPI_AMWIDE,	
	& bwprop_amnarrow,	// BWPROPI_AMNARROW,
	& bwprop_digiwide,	// BWPROPI_DIGIWIDE,
	& bwprop_nfmnarrow,	// BWPROPI_NFMNARROW,
	& bwprop_nfmwide,	// BWPROPI_NFMWIDE,
};

// �������� ��� �������
static int_fast16_t 
bwseti_getwide(
	uint_fast8_t bwseti
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	switch (p->type)
	{
	case BWSET_NARROW:
		return 0;

	default:
		return 1;
	}
}

// �������� ������ ��� �������
static int_fast8_t 
bwseti_getafresponce(
	uint_fast8_t bwseti
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	switch (p->type)
	{
	case BWSET_NARROW:
		return 0;

	default:
		return p->afresponce - AFRESPONCESHIFT;
	}
}

// �������� ������ ����������� �������
static int_fast16_t 
bwseti_getwidth(
	uint_fast8_t bwseti
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	switch (p->type)
	{
	case BWSET_NARROW:
		{
			const int_fast16_t width = p->left10_width10 * BWGRANLOW;
			const int_fast16_t width2 = width / 2;
			const int_fast16_t center = gcwpitch10 * CWPITCHSCALE;
			return (center > width2) ? width : (center * 2);
		}

	default:
	case BWSET_WIDE:
		return p->right100 * BWGRANHIGH - p->left10_width10 * BWGRANLOW;
	}
}

// �������� ������ ������� ����� �������
static int_fast16_t 
bwseti_getlow(
	uint_fast8_t bwseti
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	switch (p->type)
	{
	case BWSET_WIDE:
		return p->left10_width10 * BWGRANLOW;

	default:
	case BWSET_NARROW:
		{
			const int_fast16_t width = p->left10_width10 * BWGRANLOW;
			const int_fast16_t width2 = width / 2;
			const int_fast16_t center = gcwpitch10 * CWPITCHSCALE;
			return (center > width2) ? (center - width2) : 0;
		}
	}
}

// �������� ������� ������� ����� �������
static int_fast16_t 
bwseti_gethigh(
	uint_fast8_t bwseti
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	switch (p->type)
	{
	case BWSET_WIDE:
		return p->right100 * BWGRANHIGH;

	default:
	case BWSET_NARROW:
		{
			const int_fast16_t width = p->left10_width10 * BWGRANLOW;
			const int_fast16_t width2 = width / 2;
			const int_fast16_t center = gcwpitch10 * CWPITCHSCALE;
			return (center > width2) ? (center + width2) : (center * 2);
		}
	}
}
#if 0
// �������� ��� ���������� ������������ ������ �������
static int_fast8_t 
bwseti_getfltsofter(
	uint_fast8_t bwseti
	)
{
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];
	return p->fltsofter;
}
#endif

// ������ ������ ���������� ��������� ������
enum
{
	AGCSETI_SSB,
	AGCSETI_CW,
	AGCSETI_FLAT,	// AM, SAM, NFM
	AGCSETI_DRM,
	AGCSETI_DIGI,
	//
	AGCSETI_COUNT
};

/* ��������� �������� ���������� � NVRAM */
struct agcseti
{
	/* ��������� ��� �� ������� ������ */
	uint8_t agc_rate;
	uint8_t agc_t1;
	uint8_t agc_release10;
	uint8_t agc_t4;
	uint8_t agc_thung10;
} ATTRPACKED;	// �������� GCC, ��������� "����" � ���������. ��� ��� � ��� ��� ����� ���� ���������, see also NVRAM_TYPE_BKPSRAM

struct afsetitempl
{
	/* ��������� �������� ���������� ��� */
	uint_fast8_t agc_rate; // = 10;	// �� agc_rate �� ��������� �������� ������� 1 �� ���������
	uint_fast8_t agc_t1; // = 120;	// in 1 mS steps. 120=120 mS	charge slow
	uint_fast8_t agc_release10; // = 5;		// in 0.1 S steps. 0.5 S discharge slow
	uint_fast8_t agc_t4; // = 50;	// in 1 mS steps. 35=35 mS discharge fast
	uint_fast8_t agc_thung10; // = 3;	// 0.1 S hung time (0.3 S recomennded).
};

#define AGC_RATE_FLAT	(UINT8_MAX - 1)
#if CTLSTYLE_OLEG4Z_V1
	/* �� ���� ������� "�������" ��� */
	#define AGC_RATE_SSB	AGC_RATE_FLAT //(UINT8_MAX - 1)
	#define AGC_RATE_DIGI	AGC_RATE_FLAT //(UINT8_MAX - 1)
	#define AGC_RATE_DRM	AGC_RATE_FLAT //(UINT8_MAX - 1)
#else /* CTLSTYLE_OLEG4Z_V1 */
	#define AGC_RATE_SSB	(10)
	#define AGC_RATE_DIGI	(3)
	#define AGC_RATE_DRM	(20)
#endif /* CTLSTYLE_OLEG4Z_V1 */


static FLASHMEM const struct afsetitempl aft [AGCSETI_COUNT] =
{
	//AGCSETI_SSB,
	{
		AGC_RATE_SSB,		// agc_rate
		120,	// agc_t1
		5,		// agc_release10
		50,		// agc_t4
		3,		// agc_thung10
	},
	//AGCSETI_CW,
	{
		AGC_RATE_SSB,		// agc_rate
		120,	// agc_t1
		1,		// agc_release10
		50,		// agc_t4
		1,		// agc_thung10
	},
	//AGCSETI_FLAT,
	{
		AGC_RATE_FLAT,		// agc_rate
		120,	// agc_t1
		1,		// agc_release10
		50,		// agc_t4
		1,		// agc_thung10
	},
	//AGCSETI_DRM,
	{
		AGC_RATE_DRM,		// agc_rate
		120,	// agc_t1
		1,		// agc_release10
		50,		// agc_t4
		1,		// agc_thung10
	},
	//AGCSETI_DIGI,
	{
		AGC_RATE_DIGI,		// agc_rate
		120,	// agc_t1
		1,		// agc_release10
		50,		// agc_t4
		1,		// agc_thung10
	},
};

#endif /* WITHIF4DSP */

#define	DEFAULT_DRM_PITCH	12000	/* ��� DRM - 12 ���*/
#define	DEFAULT_RTTY_PITCH	2125	/* ��� DIGI modes - 2.125 ��� */

#if WITHIF4DSP
	#if (LO4_SIDE == LOCODE_LOWER) || (LO4_SIDE == LOCODE_UPPER) || (LO4_SIDE == LOCODE_INVALID)
		#define DEFAULT_DSP_LO5SIDE	LOCODE_TARGETED	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
	#else
		#define DEFAULT_DSP_LO5SIDE	LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#endif
	// "�����"
	// ����������� � "�����": ���� ��������� usb ���� ����� �������� ��� �������� �� 4012 �� ����, ���� ��� - ���� �� 7012��

	/* XX_PROCESSING_PITCH_XX: � ������ ������� DSP ���������� �� ������������ ���������� - �� ����� ������� ��������� (� ������). */
	/* ������, ������������ ������������ �������� ��� ��������� �����, ��� �� ��������� � ��������� ����������� ���������� ����������. */
	#define	SSB_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	SSB_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	DIGI_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	DIGI_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	BPSK_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	BPSK_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	CW_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	CW_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	CWZ_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	CWZ_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	AM_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	AM_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	SAM_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	SAM_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	NFM_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	NFM_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	DRM_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	DRM_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

	#define	WFM_LO5_SIDE_RX		DEFAULT_DSP_LO5SIDE
	#define	WFM_LO5_SIDE_TX		DEFAULT_DSP_LO5SIDE

#else /* WITHIF4DSP */
	/* XX_PROCESSING_PITCH_XX: � ������ ������� DSP ���������� �� ������������ ���������� - �� ����� ������� ��������� (� ������). */
	/* ������, ������������ ������������ �������� ��� ��������� �����, ��� �� ��������� � ��������� ����������� ���������� ����������. */
	#define	CW_LO5_SIDE_RX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define	CW_LO5_SIDE_TX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */

	#define	CWZ_LO5_SIDE_RX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define	CWZ_LO5_SIDE_TX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */

	#if ATMEGA_CTLSTYLE_V9SF_RX3QSP_H_INCLUDED
		#define	AM_PROCESSING_PITCH_RX		(455000L)	/* ��������� �� 455 ��� */
		#define	AM_PROCESSING_PITCH_TX		0
		#define	AM_LO5_SIDE_RX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		#define	AM_LO5_SIDE_TX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#else
		#define	AM_PROCESSING_PITCH_RX		0
		#define	AM_PROCESSING_PITCH_TX		0
		#define	AM_LO5_SIDE_RX		LOCODE_INVALID	/* ����� ���������� ���. */
		#define	AM_LO5_SIDE_TX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#endif

	#define	SSB_LO5_SIDE_RX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define	SSB_LO5_SIDE_TX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */

	#define	DIGI_LO5_SIDE_RX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define	DIGI_LO5_SIDE_TX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */

	#define	BPSK_LO5_SIDE_RX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define	BPSK_LO5_SIDE_TX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */

#if CTLSTYLE_SW2016VHF || CTLSTYLE_SW2018XVR
	// CTLSTYLE_SW2016VHF (Si5351)
	#define	NFM_LO5_SIDE_RX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define	NFM_LO5_SIDE_TX		LOCODE_INVALID	/* �� ��������� ������ ��������� - ��� NFM ������������ ��� �� �����. */
#else
	// ������ ����
	#define	NFM_LO5_SIDE_RX		LOCODE_INVALID	/* ����� ���������� ���. */
	#define	NFM_LO5_SIDE_TX		LOCODE_LOWER	/* �� ��������� ������ ��������� - ��� NFM ������������ ��� �� �����. */
#endif

	#define	DRM_LO5_SIDE_RX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
	#define	DRM_LO5_SIDE_TX		LOCODE_LOWER	/* ��� �������������� �� ���� ���������� ��� �������� ������� */

	#define	WFM_LO5_SIDE_RX		LOCODE_INVALID	/* ����� ���������� ���. */
	#define	WFM_LO5_SIDE_TX		LOCODE_INVALID	/* ����� ���������� ���. */
#endif /* WITHIF4DSP */
/* ��� ��������� ��� ������ ������� ������.
 * ����� ������� �� 10 �������� ������ ������ � 8 ��� ����������� �����.
 */

#define STEP_MINIMAL	10		/* ����������� ��� ����������� */
#define STEP_SSB_HIGH	10		/* ��� ��� SSB (USB/LSB/CW/CWR) ��� ���������� �������� � ������� �-��� ����� */
#define STEP_SSB_LOW	50		/* ��� ��� SSB (USB/LSB/CW/CWR) ��� ���������� �������� � ��������� �-��� ����� */

#define	STEP_AM_HIGH	100		/* ��� ��� AM */
#define	STEP_AM_LOW		200		/* ��� ��� AM */

#define STEP_DRM	1000	/* ��� ��� DRM */

#define STEP_FM		250		/* ��� ��� FM */

#define	STEP_CWZ	10		/* ��� ��� CWZ (���������� �����������)  */

#define	STEP_WFM_LOW		1000		/* ��� ��� WFM */
#define	STEP_WFM_HIGH		2500		/* ��� ��� WFM */

//#define	STEP_GENERAL	100000		/* ���������� ��� ����������� � ������ general coverage */
struct modetempl
{
	uint_fast8_t step10 [2];		/* ������������� gbigstep */
	int_fast16_t autosplitK;		/* �������� � ���������� ��� ��������� ������ autosplit */
	uint_fast8_t nar : 1;			/* ���������� ��������� �� ������� � ���� ������ */
	uint_fast8_t txcw : 1;			/* ��������� �������� � ����� ���������� ���������� */
	uint_fast8_t vox : 1;			/* � ���� ������ �������� ������������� VOX */
	uint_fast8_t wbkin : 1;			/* � ���� ������ �������� ������������� BREAK-IN */
	uint_fast8_t mute : 1;			/* � ���� ������ �������� ����������� ��������� */
	uint_fast8_t wifshift : 1;		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
	uint_fast8_t wpbt : 1;			/* � ���� ������ ��������� ������������ ��������� PBT */
	uint_fast8_t subtone: 1;		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
	uint_fast8_t lo5side [2];		/* � ���� ������ �� ��������� ��������� ��������� (RX � TX)  */
#endif /* ! defined (LO5_SIDE) */
	uint_fast8_t defagcmode;		/* ��������� ����� ��� */
#if WITHTX
	portholder_t txgfva [TXGFI_SZIE];	// ��������� ������� ��� ������ �������
	uint_fast8_t sdtnva [TXGFI_SZIE];	// �������� ��������� ������������ ��� ������ �������
#endif /* WITHTX */
#if WITHIF4DSP
	uint_fast8_t dspmode [2];		// ���������� ��� DSP � ������ ����� � �������� 
	uint_fast8_t bwseti;			// ������ ����� ����� ����������� ��� ������� ������
	int_fast16_t bw6s [2];			// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
	uint_fast8_t txaudio;			// �������� ��������� ������� ��� ������� ������
	uint_fast8_t txaprofgp;		// ������ �������� ��������� �����
	uint_fast8_t agcseti;			// ��������� ��������� ������
#else /* WITHIF4DSP */
	uint_fast8_t detector [2];		/* ��� ��������� RX � TX */
#endif /* WITHIF4DSP */
	char label [4];					// ��� �������� ������������ ������������� ���������
};

// modes
// ������� ��������� � ���� ������� ������ ��������� � ��������� enum,
// � ������� �������� MODE_COUNT
static FLASHMEM const struct modetempl mdt [MODE_COUNT] = 
{
	/* MODE_CW */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_CW
		1,					/*  �������� � ���������� ��� ��������� ������ autosplit */
		1,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		1,		/* TXCW ��������� �������� � ����� ���������� ���������� */
		0,		/* � ���� ������ �������� ������������� VOX */
		1,		/* � ���� ������ �������� ������������� QSK */
		1,		/* � ���� ������ �������� ����������� ��������� */
		0,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		1,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ CW_LO5_SIDE_RX, CW_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_FAST,	/* ��������� ����� ��� ��� MODE_CW */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TRANS, TXGFV_TX_CW, }, // txgfva ��� CW
		{ 0, 0, 0, 1 },	// �������� ��������� ������������ ��� CW
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_NARROW, DSPCTL_MODE_TX_CW, },	// ���������� ��� DSP � ������ ����� � �������� - ����� ������ �������
		BWSETI_CW,				// ������ ����� ����� ����������� ��� ������� ������
		{ 0, INT16_MAX, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
		BOARD_TXAUDIO_MIKE,		// �������� ��������� ������� ��� ������� ������
		TXAPROFIG_CW,				// ������ �������� ��������� �����
		AGCSETI_CW,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		"CW",
	},
	/* MODE_SSB */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_SSB
		5,					/*  �������� � ���������� ��� ��������� ������ autosplit */
		0,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		0,		/* TXCW ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		0,		/* � ���� ������ �������� ����������� ��������� */
		1,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		1,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ SSB_LO5_SIDE_RX, SSB_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* ��������� ����� ��� ��� MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva ��� SSB
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� SSB
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_WIDE, DSPCTL_MODE_TX_SSB, },	// ���������� ��� DSP � ������ ����� � �������� - ����� �������� �������
		BWSETI_SSB,				// ������ ����� ����� ����������� ��� ������� ������
		{ 0, 0, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
		BOARD_TXAUDIO_MIKE,		// �������� ��������� ������� ��� ������� ������
		TXAPROFIG_SSB,				// ������ �������� ��������� �����
		AGCSETI_SSB,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		"SSB",
	},
	/* MODE_AM */
	{
		{ STEP_AM_HIGH / 10, STEP_AM_LOW / 10, },	// step for MODE_AM
		5,					/*  �������� � ���������� ��� ��������� ������ autosplit */
		0,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		1,		/* TXCW ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		0,		/* � ���� ������ �������� ����������� ��������� */
		0,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		1,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ AM_LO5_SIDE_RX, AM_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* ��������� ����� ��� ��� MODE_AM */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_AM, TXGFV_TX_AM, }, // txgfva ��� AM
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� AM
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_AM, DSPCTL_MODE_TX_AM, },	// ���������� ��� DSP � ������ ����� � �������� - ����� �������� �������
		BWSETI_AM,				// ������ ����� ����� ����������� ��� ������� ������
		{ 0, INT16_MAX, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
		BOARD_TXAUDIO_MIKE,		// �������� ��������� ������� ��� ������� ������
		TXAPROFIG_AM,				// ������ �������� ��������� �����
		AGCSETI_FLAT,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_AM, BOARD_DETECTOR_AM, }, 		/* AM detector used */
#endif /* WITHIF4DSP */
		"AM",
	},
#if WITHSAM
	/* MODE_SAM  synchronous AM demodulation */
	{
		{ STEP_AM_HIGH / 10, STEP_AM_LOW / 10, },	// step for MODE_SAM
		5,					/*  �������� � ���������� ��� ��������� ������ autosplit */
		0,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		1,		/* TXCW ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		0,		/* � ���� ������ �������� ����������� ��������� */
		0,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		1,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ SAM_LO5_SIDE_RX, SAM_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* ��������� ����� ��� ��� MODE_AM */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_AM, TXGFV_TX_AM, }, // txgfva ��� AM
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� AM
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_SAM, DSPCTL_MODE_TX_AM, },	// ���������� ��� DSP � ������ ����� � �������� - ����� �������� �������
		BWSETI_AM,				// ������ ����� ����� ����������� ��� ������� ������
		{ 0, INT16_MAX, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
		BOARD_TXAUDIO_MIKE,		// �������� ��������� ������� ��� ������� ������
		TXAPROFIG_AM,				// ������ �������� ��������� �����
		AGCSETI_FLAT,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_AM, BOARD_DETECTOR_AM, }, 		/* AM detector used */
#endif /* WITHIF4DSP */
		"SAM",
	},
#endif /* WITHSAM */
	/* MODE_NFM */
	{
		{ STEP_FM / 10, STEP_FM / 10, },	// step for MODE_NFM
		- 600,	/*  �������� � ���������� ��� ��������� ������ autosplit */
		0,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		1,		/* TXCW ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		0,		/* � ���� ������ �������� ����������� ��������� */
		0,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		1,		/* � ���� ������ ��������� ������������ ��������� PBT */
		1,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ NFM_LO5_SIDE_RX, NFM_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_FAST,	/* ��������� ����� ��� ��� MODE_NFM */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_NFM, TXGFV_TX_NFM, }, // txgfva ��� NFM
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� NFM
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_NFM, DSPCTL_MODE_TX_NFM, },	// ���������� ��� DSP � ������ ����� � �������� - ����� �������� �������
		BWSETI_NFM,				// ������ ����� ����� ����������� ��� ������� ������
		{ 9000, INT16_MAX, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
		BOARD_TXAUDIO_MIKE,		// �������� ��������� ������� ��� ������� ������
		TXAPROFIG_NFM,				// ������ �������� ��������� �����
		AGCSETI_FLAT,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_FM, BOARD_DETECTOR_FM, }, 		/* FM detector used */
#endif /* WITHIF4DSP */
		"NFM",
	},
	/* MODE_DRM */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_DRM
		0,					/*  �������� � ���������� ��� ��������� ������ autosplit */
		0,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		0,		/* TXCW ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		1,		/* � ���� ������ �������� ����������� ��������� */
		0,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		1,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ DRM_LO5_SIDE_RX, DRM_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* ��������� ����� ��� ��� MODE_DRM */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva ��� DRM
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� DRM
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_DRM, DSPCTL_MODE_TX_SSB, },	// ���������� ��� DSP � ������ ����� � �������� - ����� ������ �������
		BWSETI_DRM,				// ������ ����� ����� ����������� ��� ������� ������
		{ 12000, INT16_MAX, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
		BOARD_TXAUDIO_MUTE,		// �������� ��������� ������� ��� ������� ������
		TXAPROFIG_AM,				// ������ �������� ��������� �����
		AGCSETI_DRM,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_MUTE, BOARD_DETECTOR_MUTE, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		"DRM",
	},
	/* MODE_CWZ - ���� ����� ��� �������� ������������ �� ����� TUNE. */	
	{
		{ STEP_CWZ / 10, STEP_CWZ / 10, },	// step for MODE_CWZ
		0,					/*  �������� � ���������� ��� ��������� ������ autosplit */
		0,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		1,		/* TXCW ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		1,		/* � ���� ������ �������� ����������� ��������� */
		0,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		0,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ CWZ_LO5_SIDE_RX, CWZ_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_MED,	/* ��������� ����� ��� ��� MODE_CWZ */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_CW, TXGFV_TX_CW, }, // txgfva ��� CWZ (TUNE)
		{ 0, 0, 1, 1 },	// �������� ��������� ������������ ��� CWZ (TUNE)
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_DSB, DSPCTL_MODE_TX_CW, },	// ���������� ��� DSP � ������ ����� � �������� - ����� ������ �������
		BWSETI_SSB,				// ������ ����� ����� ����������� ��� ������� ������
		{ 0, INT16_MAX, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
		BOARD_TXAUDIO_MUTE,		// �������� ��������� ������� ��� ������� ������
		TXAPROFIG_SSB,				// ������ �������� ��������� �����
		AGCSETI_SSB,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_TUNE, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		"CWZ",
	},
#if WITHWFM || WITHMODESETFULLNFMWFM
	/* WFM mode if=10.7 MHz */
	/* MODE_WFM */
	{
		{ STEP_WFM_HIGH / 10, STEP_WFM_LOW / 10, },	// step for MODE_CW
		0,			/*  �������� � ���������� ��� ��������� ������ autosplit */
		0,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		1,		/* ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		0,		/* � ���� ������ �������� ����������� ��������� */
		0,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		0,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ WFM_LO5_SIDE_RX, WFM_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_FAST,	/* ��������� ����� ��� ��� MODE_NFM */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_CW, TXGFV_TX_CW, }, // txgfva ��� AM
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� AM
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_WFM, DSPCTL_MODE_TX_SSB, },	// ���������� ��� DSP � ������ ����� � �������� - ����� �������� �������
		BWSETI_WFM,				// ������ ����� ����� ����������� ��� ������� ������
		{ INT16_MAX, INT16_MAX, },	// bypass, ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
		BOARD_TXAUDIO_MUTE,		// �������� ��������� ������� ��� ������� ������
		TXAPROFIG_SSB,				// ������ �������� ��������� �����
		AGCSETI_SSB,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_WFM, BOARD_DETECTOR_WFM, },		/* WFM detector used */
#endif /* WITHIF4DSP */
		"WFM",
	},
#endif /* WITHWFM || WITHMODESETFULLNFMWFM */
	/* MODE_DIGI */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_SSB
		5,		/*  �������� � ���������� ��� ��������� ������ autosplit */
		0,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		0,		/* TXCW ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		0,		/* � ���� ������ �������� ����������� ��������� */
		1,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		1,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ SSB_LO5_SIDE_RX, SSB_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* ��������� ����� ��� ��� MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva ��� SSB
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� SSB
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_WIDE, DSPCTL_MODE_TX_SSB, },	// ���������� ��� DSP � ������ ����� � �������� - ����� �������� �������
		BWSETI_DIGI,			// ������ ����� ����� ����������� ��� ������� ������
		{ 0, 0, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
	#if WITHUSBUAC
		BOARD_TXAUDIO_USB,		// �������� ��������� ������� ��� ������� ������
	#else /* WITHUSBUAC */
		BOARD_TXAUDIO_MIKE,		// �������� ��������� ������� ��� ������� ������
	#endif /* WITHUSBUAC */
		TXAPROFIG_DIGI,				// ������ �������� ��������� �����
		AGCSETI_DIGI,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		"DIG",
	},
	/* MODE_RTTY */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_CW
		5,			/*  �������� � ���������� ��� ��������� ������ autosplit */
		1,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		0,		/* ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		1,		/* � ���� ������ �������� ����������� ��������� */
		0,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		1,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ DIGI_LO5_SIDE_RX, DIGI_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* ��������� ����� ��� ��� MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva ��� DIGI
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� DIGI
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_WIDE, DSPCTL_MODE_TX_SSB, },	// ���������� ��� DSP � ������ ����� � �������� - ����� �������� �������
		BWSETI_DIGI,			// ������ ����� ����� ����������� ��� ������� ������
		{ 0, INT16_MAX, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
	#if WITHUSBUAC
		BOARD_TXAUDIO_USB,		// �������� ��������� ������� ��� ������� ������
	#else /* WITHUSBUAC */
		BOARD_TXAUDIO_MIKE,		// �������� ��������� ������� ��� ������� ������
	#endif /* WITHUSBUAC */
		TXAPROFIG_DIGI,				// ������ �������� ��������� �����
		AGCSETI_DIGI,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, }, 		/* ssb detector used */
#endif /* WITHIF4DSP */
		"TTY",
	},
#if WITHMODEM
	/* MODE_MODEM */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_MODEM
		5,			/*  �������� � ���������� ��� ��������� ������ autosplit */
		1,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		0,		/* ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		1,		/* � ���� ������ �������� ����������� ��������� */
		0,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		0,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ BPSK_LO5_SIDE_RX, BPSK_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* ��������� ����� ��� ��� MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva ��� DIGI
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� DIGI
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_BPSK, DSPCTL_MODE_TX_BPSK, },	// ���������� ��� DSP � ������ ����� � �������� - ����� �������� �������
		BWSETI_SSB,		// ������ ����� ����� ����������� ��� ������� ������
		{ 0, INT16_MAX, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
		BOARD_TXAUDIO_MUTE,		// �������� ��������� ������� ��� ������� ������
		TXAPROFIG_SSB,				// ������ �������� ��������� �����
		AGCSETI_SSB,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, }, 		/* ssb detector used */
#endif /* WITHIF4DSP */
		"MDM",
	},
#endif /* WITHMODEM */
	/* MODE_ISB */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_SSB
		5,					/*  �������� � ���������� ��� ��������� ������ autosplit */
		0,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		0,		/* TXCW ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		0,		/* � ���� ������ �������� ����������� ��������� */
		1,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		1,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ SSB_LO5_SIDE_RX, SSB_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* ��������� ����� ��� ��� MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva ��� SSB
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� SSB
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_ISB, DSPCTL_MODE_TX_ISB, },	// ���������� ��� DSP � ������ ����� � �������� - ����� �������� �������
		BWSETI_SSB,		// ������ ����� ����� ����������� ��� ������� ������
		{ 0, 0, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
	#if WITHUSBUAC
		BOARD_TXAUDIO_USB,		// �������� ��������� ������� ��� ������� ������
	#else /* WITHUSBUAC */
		BOARD_TXAUDIO_MIKE,		// �������� ��������� ������� ��� ������� ������
	#endif /* WITHUSBUAC */
		TXAPROFIG_SSB,				// ������ �������� ��������� �����
		AGCSETI_SSB,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, },		/* ssb detector used */
#endif /* WITHIF4DSP */
		"ISB",
	},
#if WITHFREEDV
	/* MODE_FREEDV */
	{
		{ STEP_SSB_HIGH / 10, STEP_SSB_LOW / 10, },	// step for MODE_FREEDV
		5,			/*  �������� � ���������� ��� ��������� ������ autosplit */
		1,		/* nar: ���������� ��������� �� ������� � ���� ������ */
		0,		/* ��������� �������� � ����� ���������� ���������� */
		1,		/* � ���� ������ �������� ������������� VOX */
		0,		/* � ���� ������ �������� ������������� QSK */
		1,		/* � ���� ������ �������� ����������� ��������� */
		0,		/* � ���� ������ ��������� ������������ ��������� IF SHIFT */
		0,		/* � ���� ������ ��������� ������������ ��������� PBT */
		0,		/* � ���� ������ ��������� ������������ subtones */
#if ! defined (LO5_SIDE)
		{ BPSK_LO5_SIDE_RX, BPSK_LO5_SIDE_TX, },
#endif /* ! defined (LO5_SIDE) */
		AGCMODE_SLOW,	/* ��������� ����� ��� ��� MODE_SSB */
#if WITHTX
		{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB, }, // txgfva ��� DIGI
		{ 0, 0, 0, 0 },	// �������� ��������� ������������ ��� DIGI
#endif /* WITHTX */
#if WITHIF4DSP
		{ DSPCTL_MODE_RX_FREEDV, DSPCTL_MODE_TX_FREEDV, },	// ���������� ��� DSP � ������ ����� � �������� - ����� �������� �������
		BWSETI_SSB,		// ������ ����� ����� ����������� ��� ������� ������
		{ 0, 0, },	// ������������� ������ ����������� � DSP (if6) ��� ������� ������ (���� �� ����).
		BOARD_TXAUDIO_MIKE,		// �������� ��������� ������� ��� ������� ������
		TXAPROFIG_SSB,				// ������ �������� ��������� �����
		AGCSETI_SSB,
#else /* WITHIF4DSP */
		{ BOARD_DETECTOR_SSB, BOARD_DETECTOR_SSB, }, 		/* ssb detector used */
#endif /* WITHIF4DSP */
		"FDV",
	},
#endif /* WITHFREEDV */
};


#define ENCRES_24	0	/* �������� �� ��������� ��� ������� ��� ������������� �������� �� 24 ������� */
#define ENCRES_32	1	/* �������� �� ��������� ��� ������� ��� ������������� �������� �� 64 ������� */
#define ENCRES_64	2	/* �������� �� ��������� ��� ������� ��� ������������� �������� �� 64 ������� */
#define ENCRES_100	4	/* �������� �� ��������� ��� ������� ��� ������������� �������� �� 100 ������� */
#define ENCRES_128	5	/* �������� �� ��������� ��� ������� ��� ������������� �������� �� 128 ������� */
#define ENCRES_256	7	/* �������� �� ��������� ��� ������� ��� ������������� �������� �� 256 ������� */
#define ENCRES_400	9	/* �������� �� ��������� ��� ������� ��� ������������� �������� �� 128 ������� */
#define ENCRES_600	10	/* �������� �� ��������� ��� ������� ��� ������������� �������� �� 128 ������� */

/* �������� 115200 �� ��������� �� ����������� �������������� ������� ������������ �� atmega
   ��� ������� ���������� 8 ���
   */
static const FLASHMEM uint_fast8_t encresols [] =
{
	24 / ENCRESSCALE,	// 0
	32 / ENCRESSCALE,	// 1
	64 / ENCRESSCALE,	// 2
	96 / ENCRESSCALE,	// 3
	100 / ENCRESSCALE,	// 4
	128 / ENCRESSCALE,	// 5
	144 / ENCRESSCALE,	// 6
	256 / ENCRESSCALE,	// 7
	300 / ENCRESSCALE,	// 8
	400 / ENCRESSCALE,	// 9
	600 / ENCRESSCALE,	// 10
};

#define BANDPAD 15000UL	/* 15 kHz - ����� �� �������� �� ������������� ��������� */

#if (FIXSCALE_48M0_X1_DIV256 || (defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570))) && WITHMODESETSMART
	#define BANDFUSBFREQ	13800000L	/* ���� ���� ������� �� ��������� ������������ USB */
#else
	#define BANDFUSBFREQ	9000000L	/* ���� ���� ������� �� ��������� ������������ USB */
#endif
/* BANDMIDDLE - �������, �� ������� ���������� ���������� ���� �������� ���������� */
#if defined (BANDMIDDLE)

#elif CTLSTYLE_SW2011ALL

	#define BANDMIDDLE	15000000L
	#define UPPER_DEF	19000000L

#elif TUNE_TOP >= 54000000L

	#define BANDMIDDLE  32000000L //(29700000 + BANDPAD)
	#define UPPER_DEF	40000000L

#elif TUNE_TOP >= 30000000L

	#define BANDMIDDLE	20000000L
	#define UPPER_DEF	24000000L

#elif TUNE_TOP >= 16000000L

	// �-143 "���������"
	#define BANDMIDDLE	15000000L
	#define UPPER_DEF	17000000L

#else
	// �-143 "���������"
	#define BANDMIDDLE	12000000L
	#define UPPER_DEF	13000000L

#endif

#define TUNE_6MBAND	((TUNE_TOP) >= (54000000L + BANDPAD))		// ������� � �������� ��������� 6 ������
#define TUNE_4MBAND	((TUNE_TOP) >= (70500000L + BANDPAD))		// ������� � �������� ��������� 4 �����
#define TUNE_2MBAND	((TUNE_TOP) >= (146000000L + BANDPAD))		// ������� � �������� ��������� 2 �����
#define TUNE_07MBAND	((TUNE_TOP) >= (440000000L + BANDPAD))		// ������� � �������� ��������� 70 �����������

enum
{
	BANDSETF_HAM = 0x00,
	BANDSETF_2M = 0x10,
	BANDSETF_6M = 0x20,
	BANDSETF_4M = 0x30,
	BANDSETF_07M = 0x40,
	BANDSETF_CB = 0x50,
	BANDSETF_ALL = 0x60,	/* �������� ������������ �� ���� ������� ���������� */
	BANDSETF_BCAST = 0x70,	/* ����������� �������� */
	BANDSETF_HAMWARC = 0x80,
	//
	BANDSET_MASK = 0xf0,
	BANDSET_SUBMODE = 0x0f
};

enum
{
	BANDGROUP_28MHZ,
	BANDGROUP_70MHZ,
	BANDGROUP_144MHZ,
	BANDGROUP_430MHZ,
	//
	BANDGROUP_COUNT			// ��������, ������������ ��� ������� ���������� ����������� ����������
};

#if (FLASHEND > 0x7FFF)	
#else
#endif

#if (FLASHEND > 0x3FFF)	|| CPUSTYLE_ARM || CPUSTYPE_TMS320F2833X || (TUNE_TOP >= 65535000L)

	/* ������� �������� � ��������� �� ����� � 32-� ������ ���������� */
	struct bandrange {
		uint32_t bottom, top;
		uint32_t init;
		uint8_t 	defsubmode_bandset;
		uint8_t	bandgroup;
	};

	#define BMF(a) (a)		/* ��������� ������������������ �������� */
	#define PEEK_BMF(v) (v)	/* ������� �������� ��� ������������� */

#else
	/* �����, ������� �������� � ��������� �� ��������� � 16-�� ������ ���������� */
	struct bandrange {
		uint16_t bottom, top;
		uint16_t init;
		uint8_t 	defsubmode_bandset;
		uint8_t	bandgroup;
	};

	#define BMF_SCALE	1000UL
	#define BMF(a) ((a) / BMF_SCALE )	/* ��������� ������������������ �������� */
	#define PEEK_BMF(v) ((v) * BMF_SCALE)	/* ������� �������� ��� ������������� */

#endif

#if WITHMODESETSMART
	#define BANDMAPSUBMODE_LSB	SUBMODE_SSBSMART
	#define BANDMAPSUBMODE_USB	SUBMODE_SSBSMART
	#define BANDMAPSUBMODE_CW	SUBMODE_CWSMART
	#define BANDMAPSUBMODE_CWR	SUBMODE_CWSMART
	#define BANDMAPSUBMODE_AM	SUBMODE_SSBSMART
#else /* WITHMODESETSMART */
	#define BANDMAPSUBMODE_LSB	SUBMODE_LSB
	#define BANDMAPSUBMODE_USB	SUBMODE_USB
	#define BANDMAPSUBMODE_CW	SUBMODE_CW
	#define BANDMAPSUBMODE_CWR	SUBMODE_CWR
	#define BANDMAPSUBMODE_AM	SUBMODE_AM
	#define BANDMAPSUBMODE_WFM	SUBMODE_WFM	
#endif /* WITHMODESETSMART */

/*
	60-meter band (all modes - USB):
	5330.5, 5346.5, 5366.5, 5371.5 and 5403.5 kHz

	UK: ~5298..~5408

*/

static FLASHMEM struct bandrange  const bandsmap [] =
{
#if FQMODEL_FMRADIO	// 87..108.5 MHz
	{ BMF(89000000L), BMF(89500000L), BMF(79000000L), BANDMAPSUBMODE_WFM | BANDSETF_ALL, BANDGROUP_COUNT, },		/*  */
	{ BMF(90000000L), BMF(90500000L), BMF(90000000L), BANDMAPSUBMODE_WFM | BANDSETF_ALL, BANDGROUP_COUNT, },		/*  */
	{ BMF(102000000L), BMF(102500000L), BMF(102000000L), BANDMAPSUBMODE_WFM | BANDSETF_ALL, BANDGROUP_COUNT, },		/*  */
	/* ����� ������� ���������� ��������� ������ - ��� ��������� �������� � �� ������� ������� �� ������ ������ ���������� (band_up � band_down). */
	{ BMF(TUNE_BOTTOM), 		BMF(90000000L), 		BMF(TUNE_BOTTOM), 	BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, },		/* �������� �������� - HBANDS_COUNT should equal to this index */
	{ BMF(90000000L), 			BMF(TUNE_TOP), 			BMF(101400000L), BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, },	/* �������� �������� */
	/* VFOS */
	{ BMF(TUNE_BOTTOM), 		BMF(TUNE_TOP), 			BMF(101400000L), BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, },	/* VFO A - VFOS_BASE should equal to this index */
	{ BMF(TUNE_BOTTOM), 		BMF(TUNE_TOP), 			BMF(101400000L), BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, },	/* VFO B */
#else /* FQMODEL_FMRADIO */
	//{ BMF(135000 - 0), 	BMF(138000 + 0), 	BMF(136000), SUBMODE_USB, BANDGROUP_COUNT, },		/* 135),7-137),8 ��� */
	#if TUNE_BOTTOM <= (153000 - BANDPAD)
	{ BMF(153000L - BANDPAD), 	BMF(279000L + BANDPAD), 	BMF(225000L), BANDMAPSUBMODE_AM | BANDSETF_ALL, BANDGROUP_COUNT, },		/*  */
	{ BMF(530000L - BANDPAD), 	BMF(1611000L + BANDPAD), BMF(1440000L), BANDMAPSUBMODE_AM | BANDSETF_ALL, BANDGROUP_COUNT, },		/*  */
	#endif
	{ BMF(1810000L - BANDPAD), 	BMF(2000000L + BANDPAD), BMF(1810000L), BANDMAPSUBMODE_LSB | BANDSETF_HAM, BANDGROUP_COUNT, },		/* Ukrainian band from freq 1715 kHz */
	{ BMF(3500000L - BANDPAD), 	BMF(3800000L + BANDPAD), BMF(3500000L),	BANDMAPSUBMODE_LSB | BANDSETF_HAM, BANDGROUP_COUNT, },		/*  */
#if 0
	{ BMF(2535000L - BANDPAD), 	BMF(2900000L + BANDPAD), BMF(2535000L), BANDMAPSUBMODE_USB | BANDSETF_HAM, BANDGROUP_COUNT, },		/* Old NEDRA stations band */
#endif
	{ BMF(3900000L), 			BMF(4000000L), 			BMF(3900000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST, BANDGROUP_COUNT, },		/*  */
	{ BMF(4750000L), 			BMF(5060000L), 			BMF(4750000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST, BANDGROUP_COUNT, },		/*  */
	/*
		��������� ���� ��������� 5 MHZ ( 60m ) 
		��������: 5351.5-5.366.5 khz 
		1. 5.351.5- CW -������ 200 hz . 
		2. 5.354.0, 5.357.0, 5.360.0, 5.363.0 - ALL MODE ������ 2700 hz. 
		3. 5366.0 - weak signal ������ 20 hz. 
		�� ����� ���� "�����" ����� 5,357,0, CW 5.351.5 � ���� SSB ����� 5.360.0
	*/
	{ BMF(5298000L), 			BMF(5408000L), 			BMF(5351500L), 	BANDMAPSUBMODE_CW | BANDSETF_HAMWARC, BANDGROUP_COUNT, },		/* 60-meters band */
	{ BMF(5730000L), 			BMF(6295000L), 			BMF(5730000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST, BANDGROUP_COUNT, },		/*  */
	{ BMF(6890000L), 			BMF(6990000L), 	BMF(6890000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  BANDGROUP_COUNT, },		/*  */
	{ BMF(7000000L - BANDPAD), 	BMF(7200000L + BANDPAD), BMF(7000000L), BANDMAPSUBMODE_LSB | BANDSETF_HAM, BANDGROUP_COUNT, },		/* top freq - 7300 in region-2 */	
	{ BMF(7200000L), 			BMF(7600000L), 	BMF(7200000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST, BANDGROUP_COUNT, },		/*  */
	{ BMF(9250000L), 			BMF(9900000L), 	BMF(9250000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST, BANDGROUP_COUNT, },		/*  */
	{ BMF(10100000L - BANDPAD), 	BMF(10150000L + BANDPAD), BMF(10100000L), BANDMAPSUBMODE_CW | BANDSETF_HAMWARC, BANDGROUP_COUNT, },		/*  */
#if (TUNE_TOP) >= (19020000)
	{ BMF(11500000L), 	BMF(12160000), 	BMF(11500000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  BANDGROUP_COUNT, },		/*  */
	{ BMF(13570000L), 	BMF(13870000), 	BMF(13570000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  BANDGROUP_COUNT, },		/*  */
	{ BMF(14000000L - BANDPAD), 	BMF(14350000L + BANDPAD), BMF(14000000), BANDMAPSUBMODE_USB | BANDSETF_HAM, BANDGROUP_COUNT, },		/*  */
	{ BMF(15030000L), 	BMF(15800000), 	BMF(15030000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  BANDGROUP_COUNT, },		/*  */
	{ BMF(17480000L), 	BMF(17900000), 	BMF(17480000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  BANDGROUP_COUNT, },		/*  */
	{ BMF(18068000L - BANDPAD), 	BMF(18168000L + BANDPAD), BMF(18068000), BANDMAPSUBMODE_USB | BANDSETF_HAMWARC, BANDGROUP_COUNT, },		/*  */
	{ BMF(18900000L), 	BMF(19020000), 	BMF(18900000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  BANDGROUP_COUNT, },		/*  */
#endif
#if (TUNE_TOP) >= (21450000L + BANDPAD)
	{ BMF(21000000L - BANDPAD), 	BMF(21450000L + BANDPAD), BMF(21000000L), BANDMAPSUBMODE_USB | BANDSETF_HAM, BANDGROUP_COUNT, },		/*  */
#endif
#if (TUNE_TOP) >= (21850000L)
	{ BMF(21450000L), 	BMF(21850000), 	BMF(21450000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  BANDGROUP_COUNT, },		/*  */
#endif /* (TUNE_TOP) >= (21850000) */

#if (TUNE_TOP) >= (29700000L + BANDPAD)
	{ BMF(24890000L - BANDPAD), 	BMF(24990000L + BANDPAD), BMF(24890000L), BANDMAPSUBMODE_USB | BANDSETF_HAMWARC, BANDGROUP_COUNT, },		/*  */
	{ BMF(25670000L), 	BMF(26100000), 	BMF(25670000L), 	BANDMAPSUBMODE_AM | BANDSETF_BCAST,  BANDGROUP_COUNT, },		/*  */
	{ BMF(26965000L - BANDPAD), 	BMF(27405000L + BANDPAD), BMF(27120000L), BANDMAPSUBMODE_USB | BANDSETF_CB, BANDGROUP_COUNT, },		/* Citizens Band 26.9650 MHz to 27.4050 MHz (40 channels) */

	/* next three sections - one band - "ten". */
	{ BMF(28000000L - BANDPAD), 	BMF(28320000L), 			BMF(28000000L), BANDMAPSUBMODE_CW | BANDSETF_HAM, BANDGROUP_28MHZ, },		/* CW */
	{ BMF(28320000L), 			BMF(29200000L), 			BMF(28500000L), BANDMAPSUBMODE_USB | BANDSETF_HAM, BANDGROUP_28MHZ, },		/* SSB */
	{ BMF(29200000L), 			BMF(29700000L + BANDPAD),BMF(29600000L), BANDMAPSUBMODE_USB | BANDSETF_HAM, BANDGROUP_28MHZ, },		/* FM */
#endif

#if TUNE_6MBAND
	{ BMF(50000000L - BANDPAD), 	BMF(54000000L + BANDPAD), BMF(50100000L), BANDMAPSUBMODE_USB | BANDSETF_6M, BANDGROUP_COUNT, },		/* 6 meters HAM band */
#endif /* TUNE_6MBAND */

#if TUNE_4MBAND
	{ BMF(70000000L - BANDPAD),	BMF(70050000L), 			BMF(70000000L), BANDMAPSUBMODE_CW | BANDSETF_4M, BANDGROUP_70MHZ, },		/* CW */
	{ BMF(70050000L), 			BMF(70300000L), 			BMF(70050000L), BANDMAPSUBMODE_USB | BANDSETF_4M, BANDGROUP_70MHZ, },		/* SSB */
	{ BMF(70300000L), 			BMF(70500000L + BANDPAD),BMF(70300000L), BANDMAPSUBMODE_USB | BANDSETF_4M, BANDGROUP_70MHZ, },		/* FM */
#endif /* TUNE_4MBAND */

#if TUNE_2MBAND
	/* next three sections - one band - "2 meter". */
	{ BMF(144000000L - BANDPAD),	BMF(144200000), 		BMF(144050000L), BANDMAPSUBMODE_CW | BANDSETF_2M, BANDGROUP_144MHZ, },		/* CW */
	{ BMF(144200000L), 			BMF(145000000L), 		BMF(144300000L), BANDMAPSUBMODE_USB | BANDSETF_2M, BANDGROUP_144MHZ, },		/* SSB */
	{ BMF(144500000L), 			BMF(146000000L + BANDPAD),BMF(145500000L), BANDMAPSUBMODE_USB | BANDSETF_2M, BANDGROUP_144MHZ, },		/* FM */
#endif /* TUNE_2MBAND */

#if TUNE_07MBAND
	/* next three sections - one band - "0.7 meter". */
	{ BMF(430000000L - BANDPAD),	BMF(432100000L), 		BMF(430050000L), BANDMAPSUBMODE_CW | BANDSETF_07M, BANDGROUP_430MHZ, },		/* CW */
	{ BMF(432100000L),	BMF(440000000L + BANDPAD), 		BMF(432500000L), BANDMAPSUBMODE_USB | BANDSETF_07M, BANDGROUP_430MHZ, },		/* CW */
#endif /* TUNE_2MBAND */
	/* ����� ������� ���������� ��������� ������ - ��� ��������� �������� � �� ������� ������� �� ������ ������ ���������� (band_up � band_down). */
	{ BMF(TUNE_BOTTOM), 		BMF(BANDMIDDLE), 		BMF(4997000L), 	BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, },		/* �������� �������� - HBANDS_COUNT should equal to this index */
	{ BMF(BANDMIDDLE), 			BMF(TUNE_TOP), 			BMF(UPPER_DEF), BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, },	/* �������� �������� */
	/* VFOS */
	{ BMF(TUNE_BOTTOM), 		BMF(TUNE_TOP), 			BMF(DEFAULTDIALFREQ), BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, },	/* VFO A - VFOS_BASE should equal to this index */
	{ BMF(TUNE_BOTTOM), 		BMF(TUNE_TOP), 			BMF(DEFAULTDIALFREQ), BANDMAPSUBMODE_USB | BANDSETF_ALL, BANDGROUP_COUNT, },	/* VFO B */
#endif /* FQMODEL_FMRADIO */
};

#define VFOS_COUNT 2	/* ��� VFO - A � B */
#define XBANDS_COUNT 2	/* ��� �������� ��������� */
#define HBANDS_COUNT ((sizeof bandsmap / sizeof bandsmap [0]) - XBANDS_COUNT - VFOS_COUNT)
#define VFOS_BASE ((sizeof bandsmap / sizeof bandsmap [0]) - VFOS_COUNT)
#define XBANDS_BASE0	(HBANDS_COUNT + 0)	/* ������ �� ���� ����� � ��������� ����������� */
#define XBANDS_BASE1	(HBANDS_COUNT + 1)	/* ������ �� ���� ����� � ��������� ����������� */
#define MBANDS_BASE (HBANDS_COUNT + XBANDS_COUNT + VFOS_COUNT)	/* ������ ������ � �������������� ����������� */


#if WITHSWLMODE

	#if ! defined (NVRAM_TYPE) || ! defined (NVRAM_END)
		#error NVRAM_TYPE or NVRAM_END not defined here
	#endif

	#if NVRAM_END <= 511
		#define MBANDS_COUNT	21	/* ���������� ����� ������������� ������ */
		typedef uint_fast8_t vindex_t;
	#elif NVRAM_END <= 4095
		#define MBANDS_COUNT	100 //150	/* ���������� ����� ������������� ������ */
		typedef uint_fast8_t vindex_t;
	#else
		#define MBANDS_COUNT	1000 // (254 - MBANDS_BASE)	/* ���������� ����� ������������� ������ */
		typedef uint_fast16_t vindex_t;
	#endif

#else

	#define MBANDS_COUNT	0	/* ���������� ����� ������������� ������ */
	typedef uint_fast8_t vindex_t;

#endif	/* WITHSWLMODE */

	
/* ��������� ������� �������� VFO � ������ � ����������� �� �������� ������ ����������
   - � ������ ������
*/
static vindex_t 
//NOINLINEAT
getvfoindex(uint_fast8_t bi)
{
	ASSERT(bi < 2);
	return VFOS_BASE + bi;
}

/* ������������ ������� ������� � ��������� ��������� */
static uint_fast32_t
//NOINLINEAT
get_band_bottom(vindex_t b)	/* b: �������� � ������� bandsmap */
{
	return PEEK_BMF(bandsmap [b].bottom);
}
/* ������������ ������� ������� � ��������� ��������� */
static uint_fast32_t
//NOINLINEAT
get_band_top(vindex_t b)	/* b: �������� � ������� bandsmap */
{
	return PEEK_BMF(bandsmap [b].top);
}
/* ������������ ������� ������� � ��������� ��������� */
static uint_fast32_t
//NOINLINEAT
get_band_init(vindex_t b)	/* b: �������� � ������� bandsmap */
{
	return PEEK_BMF(bandsmap [b].init);
}
/* ������������ ������� ������� � ��������� ��������� */
static uint_fast8_t
//NOINLINEAT
get_band_defsubmode(vindex_t b)	/* b: �������� � ������� bandsmap */
{
	return bandsmap [b].defsubmode_bandset & BANDSET_SUBMODE;
}

/* ������������ ������� ������� � ��������� ��������� */
static uint_fast8_t
//NOINLINEAT
get_band_bandset(vindex_t b)	/* b: �������� � ������� bandsmap */
{
	return bandsmap [b].defsubmode_bandset & BANDSET_MASK;
}

/* "�����" �������,
 * ������� �� ������� ���� �� ��������� ������� ������ "�����",
 * ������� � �������� ����� ���� �� �������� ������� ������ "�����".
 */
#if WITHBBOX && defined (WITHBBOXSUBMODE)
	static const uint_fast8_t modes [][2] =
	{
		{ 1, WITHBBOXSUBMODE, },
	};
#elif WITHMODESETSMART
	static const uint_fast8_t modes [][2] =
	{
		{ 1, SUBMODE_SSBSMART, },
		{ 1, SUBMODE_CWSMART, },
		{ 1, SUBMODE_DIGISMART, },
	};
#elif WITHMODESETMIXONLY	// Use only product detector
	static const uint_fast8_t modes [][3] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
	};
#elif WITHMODESETMIXONLY3
	static const uint_fast8_t modes [][4] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
		{ 3, SUBMODE_DGU, SUBMODE_DGL, SUBMODE_CWZ, },
	};
#elif WITHMODESAMONLY
	static const uint_fast8_t modes [][2] =
	{
		{ 1, SUBMODE_AM, },
	};
#elif WITHMODESETMIXONLY3AM
	static const uint_fast8_t modes [][4] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
		{ 2, SUBMODE_AM, SUBMODE_CWZ, },
		{ 2, SUBMODE_DGU, SUBMODE_DGL, },
	};
#elif WITHMODESETMIXONLY3NFM // SW2014FM modes set
	static const uint_fast8_t modes [][3] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
		{ 2, SUBMODE_NFM, SUBMODE_CWZ, },
		{ 2, SUBMODE_DGU, SUBMODE_DGL, },
	};
#elif WITHMODESETMIXONLY2
	static const uint_fast8_t modes [][4] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 3, SUBMODE_CWR, SUBMODE_CW, SUBMODE_CWZ },
	};
#elif (WITHMODESETFULLNFM && WITHWFM)
	static const uint_fast8_t modes [][5] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
		{ 3, SUBMODE_AM, SUBMODE_CWZ, SUBMODE_DRM, },
		{ 4, SUBMODE_NFM, SUBMODE_WFM, SUBMODE_DGU, SUBMODE_DGL, },
	};
#elif WITHMODESETFULLNFM
	#if KEYB_FPAMEL20_V3
	static const uint_fast8_t modes [][4] =
		{
			{ 2, SUBMODE_LSB, SUBMODE_USB, },				// ROW 0
			{ 2, SUBMODE_CWR, SUBMODE_CW, },				// ROW 1
			{ 3, SUBMODE_AM, SUBMODE_CWZ, SUBMODE_DRM, },	// ROW 2
			{ 3, SUBMODE_NFM, SUBMODE_DGU, SUBMODE_DGL, },	// ROW 3
		};
	#elif WITHMODEM
		static const uint_fast8_t modes [][5] =
		{
			{ 1, SUBMODE_BPSK, },
		};
	#elif WITHSAM
	static const uint_fast8_t modes [][5] =
		{
			{ 2, SUBMODE_LSB, SUBMODE_USB, },
			{ 2, SUBMODE_CWR, SUBMODE_CW, },
			{ 4, SUBMODE_AM, SUBMODE_SAM, SUBMODE_CWZ, SUBMODE_DRM, },
			{ 3, SUBMODE_NFM, SUBMODE_DGU, SUBMODE_DGL, },
		};
	#else /* WITHMODEM */
	static const uint_fast8_t modes [][4] =
		{
			{ 2, SUBMODE_LSB, SUBMODE_USB, },
			{ 2, SUBMODE_CWR, SUBMODE_CW, },
			{ 3, SUBMODE_AM, SUBMODE_CWZ, SUBMODE_DRM, },
			{ 3, SUBMODE_NFM, SUBMODE_DGU, SUBMODE_DGL, },
		};
	#endif /* WITHMODEM */
#elif WITHMODESETFULLNFMWFM
	static const uint_fast8_t modes [][5] =
		{
			{ 2, SUBMODE_LSB, SUBMODE_USB, },
			{ 2, SUBMODE_CWR, SUBMODE_CW, },
			{ 4, SUBMODE_AM, SUBMODE_SAM, SUBMODE_CWZ, SUBMODE_DRM, },
			{ 4, SUBMODE_NFM, SUBMODE_WFM, SUBMODE_DGU, SUBMODE_DGL, },
		};
#elif WITHMODESETFULL
	static const uint_fast8_t modes [][4] =
	{
		{ 2, SUBMODE_LSB, SUBMODE_USB, },
		{ 2, SUBMODE_CWR, SUBMODE_CW, },
		{ 3, SUBMODE_AM, SUBMODE_CWZ, SUBMODE_DRM, },
		{ 2, SUBMODE_DGU, SUBMODE_DGL, },
	};
#else
	#error WITHMODESETxxx not defined
#endif /*  */


#define MODEROW_COUNT (sizeof modes / sizeof modes [0])

/* ����� ���������� ������ � ����� �������.
   ��� �������� ������� - �������
   ���� �� ������ - ������� 0 � ������ 0.
   TODO: ��� ������� ������ ������ FM, ���� ��� ��� � ������, �������� LSB (0-� ����� � 0-� ������. �� ����� �������.
*/
static uint_fast8_t
//NOINLINEAT
locatesubmode(
	const uint_fast8_t submode,		/* ��� ������ */
	uint_fast8_t * const xrow		/* ��������� ���������� */
	)
{
	uint_fast8_t row;

	for (row = 0; row < MODEROW_COUNT; ++ row)
	{	
		const uint_fast8_t n = modes [row] [0];
		uint_fast8_t col;

		for (col = 0; col < n && col < (sizeof modes [row] / sizeof modes [row][0] - 1); ++ col)
		{
			if (modes [row][col + 1] == submode)
			{
				* xrow = row;
				return col;
			}
		}
	}
	* xrow = 0;
	return 0;
}

#if 0
/* �������� ������� ������ � ����� �������.
   ���� �� ������ - ������� 0
*/
static uint_fast8_t
validatesubmode(
	const uint_fast8_t submode		/* ��� ������ */
	)
{
	uint_fast8_t row;

	for (row = 0; row < MODEROW_COUNT; ++ row)
	{	
		const uint_fast8_t n = modes [row] [0];
		uint_fast8_t col;

		for (col = 0; col < n && col < (sizeof modes [row] / sizeof modes [row][0] - 1); ++ col)
		{
			if (modes [row][col + 1] == submode)
			{
				return 1;
			}
		}
	}
	return 0;
}
#endif

/* ����� (�����), ������������ ��� ��������� ����������� NVRAM */
/* ��������� ���� ����� ������� � NVRAM �� ������������ � �� ������������. */
static const FLASHMEM char nvramsign [] = 
	#if NVRAM_END > 511
		__DATE__
	#endif
	__TIME__;

/* ������ ������ ��� ������������ ������� � NVRAM */
static const FLASHMEM char nvrampattern [sizeof nvramsign / sizeof nvramsign [0]] = 
{
	"DEADBEEF"
};

/* ��������� - ������������ ������ � ���������������� ������.
   bitfields ������ ������������, ��� ��� �� ��� - ������ ����������� ��������
	 ���������� � ���������������� ������.
 	��������� "���������" ������� - CW&CWR, LSB&USB.
	*/
struct modeprops
{
	uint8_t agc;	/* ����� ��� ��� ���������������� ������� */
	uint8_t filter;	/* ������ ������� � ����� ������� �������� */
	//uint16_t step;	/* ��� ��������� � ������ ������ */

#if CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 || CTLSTYLE_RA4YBO_V3
	uint16_t txpower;		/* �������� */
	uint16_t txcompr;		/* ������� ���������� */
#endif /* CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2*/

#if WITHIF4DSP
	uint8_t txaudio;	/* �������� ����� ��� �������� */
#endif /* WITHIF4DSP */

} ATTRPACKED;// �������� GCC, ��������� "����" � ���������. ��� ��� � ��� ��� ����� ���� ���������, see also NVRAM_TYPE_BKPSRAM

/* ��������� - ������������ ������ � ���������������� ���.
   bitfields ������ ������������, ��� ��� �� ��� - ������ ����������� ��������
	 ���������� � ���������������� ���.
 ����������, ����������� ��� ������� ��������� */
struct bandinfo
{	
	uint32_t freq;		/* ������� ������� */
	uint8_t modecols [MODEROW_COUNT];	/* ������ ������� ������ - ������ ���� ��������� ����� ������� � ������ ������) */
	uint8_t moderow;		/* ����� ������ ������ � ����� (����� ������ ���) */
#if ! WITHONEATTONEAMP
	uint8_t pamp;		/* ����� ��� */
#endif /* ! WITHONEATTONEAMP */
	uint8_t att;		/* ����� ����������� */
#if WITHANTSELECT
	uint8_t ant;		/* ��� ������ ������� (1T+1R, 2T+1R � ��� �����, �� ��� ������� (0/1) */
#endif /* WITHANTSELECT */
#if WITHAUTOTUNER
	// todo: �������� ���� ���������� �������
	uint8_t tunercap;
	uint8_t tunerind;
	uint8_t tunertype;
#endif /* WITHAUTOTUNER */
} ATTRPACKED;// �������� GCC, ��������� "����" � ���������. ��� ��� � ��� ��� ����� ���� ���������, see also NVRAM_TYPE_BKPSRAM

/* ��������� - ������������ ������ � ���������������� ���.
   bitfields ������ ������������, ��� ��� �� ��� - ������ ����������� ��������
	 ���������� � ���������������� ���.
*/
struct nvmap
{
	uint8_t lockmode;			/* ���������� ��������� */
#if WITHENCODER2
	uint8_t enc2state;
	uint8_t enc2pos;			// �������� ����� ���� (������ ��������)
#endif /* WITHENCODER2 */
#if WITHLCDBACKLIGHT
	uint8_t dimmmode;			/* ���������� ��������� ������� � ���������� */
#endif /* WITHLCDBACKLIGHT */
#if WITHUSEFAST
	uint8_t gusefast;			/* ������������ � ����� �������� ���� */
#endif /* WITHUSEFAST */
	uint8_t menuset;		/* ����� ������� ������ � ����� ����������� �� ������� */

	/* ������ */
	uint8_t ggroup;			/* ��������� ������ � �����, � ������� �������� */
	uint8_t	ggrpdisplay;	// ��������� ���������� ����� ������
	uint8_t	ggrpothers;		// ��������� ���������� ����� ������
	uint8_t	ggrpsecial;		// ��������� ���������� ����� ������
	uint8_t	ggrpaudio;		// ��������� ���������� ����� ������
#if WITHSUBTONES && WITHTX
	uint8_t ggrpctcss;		// ��������� ���������� ����� ������
#endif /* WITHSUBTONES && WITHTX */
#if defined (RTC1_TYPE)
	uint8_t	ggrpclock; // ��������� ���������� ����� ������
#endif /* defined (RTC1_TYPE) */

#if LO1MODE_HYBRID
	uint8_t alignmode;			/* ������ ��� ��������� ���������� ����� (0-���������� ������) */
#endif /* LO1MODE_HYBRID */
#if WITHUSEDUALWATCH
	uint8_t mainsubrxmode;		// �����/������, A - main RX, B - sub RX
#endif /* WITHUSEDUALWATCH */
#if WITHENCODER
	uint8_t ghiresres;		/* ������ � ������� ���������� ��������� */
	uint8_t ghiresdyn;	/* ��������� ������������� �������� */
#endif /* WITHENCODER */

#if defined (DEFAULT_LCD_CONTRAST)
	uint8_t gcontrast;		/* ������������� LCD */
#endif /* defined (DEFAULT_LCD_CONTRAST) */
#if WITHLCDBACKLIGHT
	uint8_t bglight;
#endif /* WITHLCDBACKLIGHT */
#if WITHDCDCFREQCTL
	//uint16_t dcdcrefdiv;
#endif /* WITHDCDCFREQCTL */
#if WITHKBDBACKLIGHT
	uint8_t kblight;
#endif /* WITHKBDBACKLIGHT */
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
	uint8_t dimmtime;
#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */
#if WITHFANTIMER
	uint8_t fanpatime;
#endif /* WITHFANTIMER */
#if WITHSLEEPTIMER
	uint8_t sleeptime;
#endif /* WITHSLEEPTIMER */
#if LCDMODE_COLORED
	uint8_t gbluebgnd;
#endif /* LCDMODE_COLORED */

#if WITHTX
	uint8_t	ggrptxparams; // ��������� ���������� ����� ������
	//uint8_t gfitx;		/* ����� ������������� ������� �� �������� */
	#if WITHMIC1LEVEL
		uint16_t mik1level;
	#endif /* WITHMIC1LEVEL */
	#if WITHPOWERLPHP
		uint8_t gpwr;
	#endif /* WITHPOWERLPHP */
#endif /* WITHTX */

#if WITHNOTCHONOFF
	uint8_t	ggrpnotch; // ��������� ���������� ����� ������
	uint8_t gnotch;
#elif WITHNOTCHFREQ
	uint8_t	ggrpnotch; // ��������� ���������� ����� ������
	uint8_t gnotch;
	uint16_t gnotchfreq;
	uint16_t gnotchwidth;
#endif /* WITHNOTCHFREQ */

#if WITHRFSG
	uint8_t userfsg;
#endif /* WITHRFSG */

	uint8_t displayfreqsfps;		/* �������� ���������� ���������� ������� */
	uint8_t displaybarsfps;	/* �������� ���������� S-����� */
#if WITHBCBANDS
	uint8_t bandsetbcast;	/* Broadcasting radio bands */
#endif /* WITHBCBANDS */
	uint8_t bandset11m;	/* CB radio band */
#if TUNE_6MBAND
	uint8_t bandset6m;	/* ������������ �� �������� 6 ������ */
#endif /* TUNE_6MBAND */
#if TUNE_4MBAND
	uint8_t bandset4m;	/* ������������ �� �������� 6 ������ */
#endif /* TUNE_4MBAND */
#if TUNE_2MBAND
	uint8_t bandset2m;	/* ������������ �� �������� 2 ����� */
#endif /* TUNE_2MBAND */

#if WITHSPLIT
	uint8_t splitmode;		/* ��-0, ���� ������ � �������������� �������� (vfo/vfoa/vfob/mem) */
	uint8_t vfoab;		/* 1, ���� ������ � VFO B, 0 - � VFO A */
#elif WITHSPLITEX
	uint8_t splitmode;		/* ��-0, ���� ������ � �������������� �������� (vfo/vfoa/vfob/mem) */
	uint8_t vfoab;		/* 1, ���� ������ � VFO B, 0 - � VFO A */
#endif /* WITHSPLIT */

	uint8_t gcwpitch10;	/* ��� � CW/CWR ������ */
	uint8_t gkeybeep10;	/* ��� ������� ������� ������ */
	uint8_t stayfreq;	/* ��� ��������� ������� �������� - �� ������ ������� */

#if WITHIF4DSP
	uint8_t ggrpnfm; // ��������� ���������� ����� ������ NFM
	uint8_t	ggrpagc; // ��������� ���������� ����� ������
	uint8_t	ggrpagcssb; // ��������� ���������� ����� ������
	uint8_t	ggrpagccw; // ��������� ���������� ����� ������
	uint8_t	ggrpagcdigi; // ��������� ���������� ����� ������
	uint8_t bwsetpos [BWSETI_count];	/* ����� ����� �� ����� ����������� */

	uint8_t bwpropsleft [BWPROPI_count];	/* �������� ������ ������ ����������� */
	uint8_t bwpropsright [BWPROPI_count];	/* �������� ������ ������ ����������� */
	uint8_t bwpropsfltsofter [BWPROPI_count];	/* ��� ���������� ������������ ������ ������� �������� �������� �� ����� */
	uint8_t bwpropsafresponce [BWPROPI_count];	/* ������ ��� */
	uint8_t gssbtxlowcut10;		/* ������ ������� ����� ������� �� ��� �������� */
	uint8_t gssbtxhighcut100;	/* ������� ������� ����� ������� �� ��� �������� */

	struct agcseti afsets [AGCSETI_COUNT];	/* ������ ������ */

	uint8_t gagcoff;
	uint8_t gamdepth;		/* ������� ��������� � �� - 0..100% */
	uint8_t gdacscale;		/* ������������� ��������� ������� � ��� ����������� - 0..100% */
	uint8_t tdsp_nfm_sql_off;	// ��������� �������
	uint8_t tdsp_nfm_sql_level;
	uint8_t	gcwedgetime;			/* ����� ����������/����� ��������� ��������� ��� �������� - � 1 �� */
	uint8_t	gsidetonelevel;	/* ������� ������� ������������ � ��������� - 0%..100% */
	uint8_t	gsubtonelevel;	/* ������� ������� CTCSS � ��������� - 0%..100% */
	uint8_t gdigigainmax;	/* �������� ������ ����������� ��������� �������� - ������������ �������� */
	uint8_t gsquelch;		/* ������� �������� �������������� */
	uint8_t gvad605;		/* ���������� �� AD605 (���������� ��������� ������ �� */
	uint16_t gfsadcpower10 [2];	/*	��������, ��������������� full scale �� IF ADC (� ��������� 0.1 ����� */
	#if ! WITHPOTAFGAIN
		uint16_t afgain1;	// �������� ��� ����������� ������ �� ������ �����-���
	#endif /* ! WITHPOTAFGAIN */
	#if ! WITHPOTIFGAIN
		uint16_t rfgain1;	// �������� ��� ����������� �������� �� ��
	#endif /* ! WITHPOTIFGAIN */
	uint16_t glineamp;	// �������� � LINE IN
	uint8_t gmikebust20db;	// ������������� ���������
	uint8_t gmikeagc;	/* ��������� ����������� ��� ����� ����������� */
	uint8_t gmikeagcgain;	/* ������������ �������� ��� ��������� */
	uint8_t gmikehclip;		/* ������������ */
	#if WITHUSBUAC
		uint8_t gdatamode;	/* �������� ����� � USB ������ �������� ��������� */
		uint8_t guacplayer;	/* ����� ������������� ������ ���������� � ��������� ���������� - ���������� ����� */
		#if WITHRTS96 || WITHRTS192 || WITHTRANSPARENTIQ
			uint8_t gswapiq;		/* �������� ������� I � Q ������ � ������ RTS96 */
		#endif /* WITHRTS96 || WITHRTS192 || WITHTRANSPARENTIQ */
	#endif /* WITHUSBUAC */
	#if WITHAFCODEC1HAVEPROC
		uint8_t gmikeequalizer;	// ��������� ��������� ������� � ��������� (�������, ����������, ...)
		uint8_t gmikeequalizerparams [HARDWARE_CODEC1_NPROCPARAMS];	// ���������� 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
	#endif /* WITHAFCODEC1HAVEPROC */

	struct micproc gmicprocs [NMICPROFILES];
	uint8_t txaprofile [TXAPROFIG_count];	/* ��������� ��������� ����� ����� ����������� */
#endif /* WITHIF4DSP */


#if WITHDSPEXTDDC	/* "��������" � DSP � FPGA */
	uint8_t	ggrprfadc; // ��������� ���������� ����� ������
	uint8_t gdither;	/* ���������� ����������� � LTC2208 */
	uint8_t gadcrand;	/* ���������� ����������� � LTC2208 */
	uint8_t gadcfifo;
	uint16_t gadcoffset;
	uint8_t gdactest;
#endif /* WITHDSPEXTDDC */

#if WITHMODEM
	uint8_t	ggrpmodem; // ��������� ���������� ����� ������
	uint8_t gmodemspeed;	// ������ � ������� ��������� ��������
	uint8_t gmodemmode;		// ����������� ���������
#endif /* WITHMODEM */

#if WITHLO1LEVELADJ
	uint8_t lo1level;	/* ������� (���������) LO1 � ��������� */
#endif /* WITHLO1LEVELADJ */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX)

#elif WITHDUALFLTR		/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */
	uint16_t lo4offset;			/* ������� (��� ����) �������� ���������� */
#elif WITHFIXEDBFO
	uint8_t glo4lsb;			/* ������� LSB �� ��������� �� */
	uint16_t lo4offset;			/* ������� (��� ����) �������� ���������� */
#elif WITHDUALBFO
	uint16_t lo4offsets [2];		/* ������� (��� ����) �������� ���������� � USB � LSB */
#endif

#if WITHIFSHIFT
	uint16_t ifshifoffset;	/* �������� ������� ����������� � ������ If SHIFT */
#endif /* WITHIFSHIFT */

	uint8_t	ggrpfilters; // ��������� ���������� ����� ������

#if CTLSTYLE_RA4YBO_V1 || (defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) && (IF3_MODEL != IF3_TYPE_BYPASS))

	uint8_t dcrxmode;	/* settings menu option - RX acts as direct conversion */
	uint8_t dctxmodessb, dctxmodecw;	/* settings menu option - TX acts as direct conversion */


#if WITHPBT //&& (LO3_SIDE != LOCODE_INVALID)
	uint8_t	ggrppbts; // ��������� ���������� ����� ������
	uint16_t pbtoffset;	/* �������� ������� ����������� � ������ PBT */
#endif /* WITHPBT && (LO3_SIDE != LOCODE_INVALID) */

	/* ��������� ���������� ����������� ������� ����� �������� */
#if (IF3_FMASK & IF3_FMASK_0P3)
	uint8_t hascw0p3;			/* ������� � ����� ������� �� 0.3 ��� */
	uint16_t carr0p3;	/* settings menu option */
#endif
#if (IF3_FMASK & IF3_FMASK_0P5)
	uint8_t hascw0p5;			/* ������� � ����� ������� �� 0.5 ��� */
	uint16_t carr0p5;	/* settings menu option */
#endif

	/* ��������� ���������� ������ ������ ������� �������� */
#if (IF3_FMASK & IF3_FMASK_1P8)
	uint8_t hascw1p8;			/* ������� � ����� ������� �� 1.8 ��� */
	uint16_t usbe1p8;	/* settings menu option */
	uint16_t lsbe1p8;	/* settings menu option */
#endif
#if (IF3_FMASK & IF3_FMASK_2P4)
	uint8_t hascw2p4;			/* ������� � ����� ������� �� 2.4 ��� */
	uint16_t usbe2p4;	/* settings menu option */
	uint16_t lsbe2p4;	/* settings menu option */
	#if WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4) != 0
		uint16_t usbe2p4tx;	/* settings menu option */
		uint16_t lsbe2p4tx;	/* settings menu option */
	#endif
#endif
#if (IF3_FMASK & IF3_FMASK_2P7)
	uint8_t hascw2p7;			/* ������� � ����� ������� �� 2.7 ��� */
	uint16_t usbe2p7;	/* settings menu option */
	uint16_t lsbe2p7;	/* settings menu option */
	#if WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) != 0
		uint16_t usbe2p7tx;	/* settings menu option */
		uint16_t lsbe2p7tx;	/* settings menu option */
	#endif
#endif
#if (IF3_FMASK & IF3_FMASK_3P1)
	uint16_t usbe3p1;	/* settings menu option */
	uint16_t lsbe3p1;	/* settings menu option */
	#if WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_3P1) != 0
		uint16_t usbe3p1tx;	/* settings menu option */
		uint16_t lsbe3p1tx;	/* settings menu option */
	#endif
#endif

	/* ��������� ���������� ����������� ������� ������� �������� */
#if (IF3_FMASK & IF3_FMASK_6P0)
	uint8_t hascw6p0;			/* ������� � ����� ������� �� 6.0 ��� */
	uint16_t cfreq6k;
#endif
#if (IF3_FMASK & IF3_FMASK_7P8)
	uint16_t cfreq7p8k;
#endif
#if (IF3_FMASK & IF3_FMASK_8P0)
	uint16_t cfreq8k;
#endif
#if (IF3_FMASK & IF3_FMASK_9P0)
	uint16_t cfreq9k;
#endif
#if (IF3_FMASK & IF3_FMASK_15P0)
	uint16_t cfreq15k;
	uint16_t cfreq15k_nfm;
#endif
#if (IF3_FMASK & IF3_FMASK_17P0)
	uint16_t cfreq17k;
#endif
#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4)
	uint8_t hascw2p4_tx;
#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4) */
#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7)
	uint8_t hascw2p7_tx;
#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) */
#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_3P1)
	uint8_t hascw3p1_tx;
#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) */

#elif CTLSTYLE_RA4YBO_V3
/*
filter_t fi_0p5 =
filter_t fi_3p1 =
filter_t fi_3p0_455 =
filter_t fi_10p0_455 =
filter_t fi_6p0_455 =
filter_t fi_2p0_455 =
*/
	uint16_t cfreq10k;
	uint16_t cfreq2k;
	uint16_t cfreq6k;
	uint16_t usbe3p0;	/* settings menu option */
	uint16_t lsbe3p0;	/* settings menu option */
	uint16_t usbe3p1;	/* settings menu option */
	uint16_t lsbe3p1;	/* settings menu option */
	uint16_t carr0p5;

#endif

#if defined(REFERENCE_FREQ)
#if defined (DAC1_TYPE)
	uint8_t dac1level;	/* ���������� �� ���������� ������� ����������� */
#endif /* defined (DAC1_TYPE) */
	uint16_t refbias;	/* ���������� �������� */

#endif /* defined(REFERENCE_FREQ) */
#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	uint16_t si570_xtall_offset;
#endif

#if WITHCAT
	uint8_t	ggrpcat; // ��������� ���������� ����� ������
	uint8_t catenable;	/* ��������� ���������� ��������� */
	uint8_t catbaudrate;	/* ����� �������� ������ �� CAT */
	uint8_t catdtrptt;	/* ������ ����������� �� DTR, � �� �� RTS */
	uint8_t catrtsenable;	/* ���������� ��������� �������� �� ����� RTS CAT */
	uint8_t catdtrenable;	/* ���������� ����������� �� DTR CAT */
#endif /* WITHCAT */

#if WITHAUTOTUNER
	uint8_t	ggrptuner; // ��������� ���������� ����� ������
	uint8_t tunerwork;
	uint8_t tunerdelay;
#endif /* WITHAUTOTUNER */


#if WITHTX
	#if WITHMUTEALL
		uint8_t gmuteall;	/* ��������� �������� ������. */
	#endif /* WITHMUTEALL */
	#if WITHVOX
		uint8_t	ggrpvox; // ��������� ���������� ����� ������
		uint8_t gvoxenable;	/* �������������� ���������� ������������ (�� ������) */
		uint8_t gvoxlevel;	/* ������� ������������ VOX */
		uint8_t gavoxlevel;	/* ������� anti-VOX */
		uint8_t voxdelay;	/* �������� ���������� VOX */
	#endif /* WITHVOX */

	#if WITHELKEY
		uint8_t bkinenable;	/* �������������� ���������� ������������ (�� ������������ ������������) */
	#endif /* WITHELKEY */

	#if WITHPOWERTRIM || WITHPOWERLPHP
		#if ! WITHPOTPOWER
			uint8_t gnormalpower;/* �������� WITHPOWERTRIMMIN..WITHPOWERTRIMMAX */
		#endif /* ! WITHPOTPOWER */
		uint8_t gotunerpower;/* �������� ��� ������ ��������������� ������������ ���������� WITHPOWERTRIMMIN..WITHPOWERTRIMMAX */
	#endif /* WITHPOWERTRIM || WITHPOWERLPHP */
	#if WITHPABIASTRIM
		uint8_t gpabias;	/* ��� ���������� ������� ����������� */
	#endif /* WITHPABIASTRIM */
	uint8_t gtxgate;	/* ���������� ������������� */
	uint8_t bkindelay;	/* �������� ���������� BREAK-IN */
	uint8_t rxtxdelay;	/* ����-�������� */
	uint8_t txrxdelay;	/* ��������-���� */


	uint8_t swrcalibr;	/* ������������� �������� SWR-����� */
	uint8_t maxpwrcali;	/* ������������� �������� PWR-����� */
	uint16_t minforward; // = 10;
	#if WITHSWRMTR && ! WITHSHOWSWRPWR
		uint8_t swrmode;	/* 1 - ����� SWR �����, 0 - �������� */
	#endif

#if WITHSUBTONES
	uint8_t gsubtonei;	// ����� subtone
	uint8_t gsbtonenable;	// ��������� ������������ subtone
#endif /* WITHSUBTONES */

#endif /* WITHTX */

#if WITHVOLTLEVEL && ! WITHREFSENSOR
	uint8_t voltcalibr;	/* ������������� �������� ���������� ���������� ��� - ���������� fullscale = VREF * 5.3 = 3.3 * 5.3 = 17.5 ������ */
#endif /* WITHVOLTLEVEL && ! WITHREFSENSOR */

#if WITHELKEY
	uint8_t	ggrpelkey; // ��������� ���������� ����� ������
#if ! WITHPOTWPM
	uint8_t elkeywpm;	/* �������� ������������ ����� */
#endif /* ! WITHPOTWPM */
	uint8_t elkeymode;	/* ����� ������������ ����� - 0 - asf, 1 - paddle, 2 - keyer */
	uint8_t dashratio;	/* ��������� ����������� ���� � ����� � �������� ��������� */
	uint8_t spaceratio;	/* ��������� ����������� ����� � ����� � �������� ��������� */
	uint8_t elkeyreverse;	
#if WITHVIBROPLEX
	uint8_t elkeyslope;	/* �������� ���������� ������������ ����� � ����� - �������� ����������� */
	uint8_t elkeyslopeenable;	/* �������� ���������� ������������ ����� � ����� - �������� ����������� */
#endif /* WITHVIBROPLEX */
#endif /* WITHELKEY */


	uint8_t gbigstep;		/* ������� ��� ��������� */
	uint8_t ghiresdiv;		/* �� ������� ��� ��������� ���������� ���������. */
#if WITHSPKMUTE
	uint8_t gmutespkr;		/* ���������� �������� */
#endif /* WITHSPKMUTE */

#if CTLSTYLE_RA4YBO
	uint8_t gaffilter;		/* ��������� ��� �� ����� � �������� RA4YBO */
#elif CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 || KEYBSTYLE_RA4YBO_AM0
	uint8_t gaffilter;		/* ��������� ��� �� ����� � �������� RA4YBO */
	uint8_t guser1;
	uint8_t guser2;
	uint8_t guser3;
	uint8_t guser4;
	uint8_t guser5;
#elif CTLSTYLE_RA4YBO_V3
	uint8_t gaffilter;		/* ��������� ��� �� ����� � �������� RA4YBO */
	uint8_t guser1;
	uint8_t guser2;
	uint8_t guser3;
	uint8_t guser4;
	uint8_t guser5;
#endif /* CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 || CTLSTYLE_RA4YBO_V3 */

#if LO1FDIV_ADJ
	uint8_t lo1powrx;		/* �� ������� ��� �� 2 ����� �������� ������� ����������� ������� ���������� */
	uint8_t lo1powtx;		/* �� ������� ��� �� 2 ����� �������� ������� ����������� ������� ���������� */
#endif /* LO1FDIV_ADJ */
#if LO4FDIV_ADJ
	uint8_t lo4powrx;		/* �� ������� ��� �� 2 ����� �������� ������� ����������� ����������� ���������� */
	uint8_t lo4powtx;		/* �� ������� ��� �� 2 ����� �������� ������� ����������� ����������� ���������� */
#endif /* LO1FDIV_ADJ */

#if WITHBARS
	uint8_t s9level;			/* ������ ���������� S-����� */
	uint8_t s9delta;	
	uint8_t s9_60_delta;
#endif /* WITHBARS */
	
#if LO1PHASES
	uint16_t phaserx, phasetx;
#endif /* LO1PHASES */

#if WITHLFM
	uint8_t	ggrplfm; // ��������� ���������� ����� ������
	uint16_t lfmtoffset;
	uint16_t lfmtinterval;
	uint8_t lfmmode;
	uint16_t lfmstart100k;
	uint16_t lfmstop100k;
	uint16_t lfmspeed1k;
#endif /* WITHLFM */

#if WITHUSEAUDIOREC
	uint8_t recmode;	/* ������������� �������� ������ �� SD CARD ��� ��������� */
#endif /* WITHUSEAUDIOREC */

#if (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ	/* ���������� ������� ���������� ����� ����. */
	uint16_t lo3offset;
#endif	/* (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ */

	struct modeprops modes [MODE_COUNT];

	struct bandinfo bands [HBANDS_COUNT + XBANDS_COUNT + VFOS_COUNT + MBANDS_COUNT];
#if	WITHDIRECTBANDS
	uint8_t	bandgroup [BANDGROUP_COUNT];	/* ��������� �������� � ������, ���� ��� ������� �� ������ ��������� (������ � bands). */
#endif	/* WITHDIRECTBANDS */
	uint8_t signature [sizeof nvramsign - 1];	/* ��������� ������������ ������ ��������� � ����������� NVRAM */
} ATTRPACKED;	// �������� GCC, ��������� "����" � ���������. ��� ��� � ��� ��� ����� ���� ���������, see also NVRAM_TYPE_BKPSRAM

/* ���������, ������������ ������������ ���������� � FRAM */

#define RMT_MENUSET_BASE offsetof(struct nvmap, menuset)		/* ����� ������� ������ � ����� ����������� �� ������� */
#define RMT_GROUP_BASE offsetof(struct nvmap, ggroup)		/* ���� - ��������� ������ ����, � ������� �������� */
#define RMT_SIGNATURE_BASE(i) offsetof(struct nvmap, signature [(i)])			/* ������������ ��������� */
#define RMT_LOCKMODE_BASE offsetof(struct nvmap, lockmode)		/* ������� ���������� ��������� */
#define RMT_DIMMMODE_BASE offsetof(struct nvmap, dimmmode)		/* ���������� ��������� ������� � ���������� */
#define RMT_USEFAST_BASE offsetof(struct nvmap, gusefast)		/* ������������ � ����� �������� ���� */
#define RMT_AFFILTER_BASE offsetof(struct nvmap, gaffilter)		/* ��������� ��� �� ����� � �������� RA4YBO */
#define RMT_MUTELOUDSP_BASE offsetof(struct nvmap, gmutespkr)		/* ��������� ��� �� ����� � �������� RA4YBO */

#define RMT_SPLITMODE_BASE offsetof(struct nvmap, splitmode)		/* (vfo/vfoa/vfob/mem) */
#define RMT_VFOAB_BASE offsetof(struct nvmap, vfoab)		/* (vfoa/vfob) */
#define RMT_MBAND_BASE offsetof(struct nvmap, gmband)		/* ������ ������ ������������� ������ */

#define RMT_MAINSUBRXMODE_BASE	offsetof(struct nvmap, mainsubrxmode)
#define RMT_DATAMODE_BASE	offsetof(struct nvmap, gdatamode)


#define RMT_AGC_BASE(i)	offsetof(struct nvmap, modes[(i)].agc)
#define RMT_FILTER_BASE(i)	offsetof(struct nvmap, modes[(i)].filter)
#define RMT_STEP_BASE(i)	offsetof(struct nvmap, modes[(i)].step)

#define RMT_TXPOWER_BASE(i)	offsetof(struct nvmap, modes[(i)].txpower)
#define RMT_TXCOMPR_BASE(i)	offsetof(struct nvmap, modes[(i)].txcompr)
#define RMT_TXAUDIO_BASE(i) offsetof(struct nvmap, modes[(i)].txaudio)
#define RMT_TXAPROFIGLE_BASE(i) offsetof(struct nvmap, txaprofile[(i)])

#define RMT_BANDGROUP(i) offsetof(struct nvmap, bandgroup[(i)])	/* ��������� �������� � ������, ���� ��� ������� �� ������ ��������� (������ � bands). */
#define RMT_BFREQ_BASE(i) offsetof(struct nvmap, bands[(i)].freq)			/* ��������� �������, �� ������� ����������� (4 �����) */
#define RMT_PAMP_BASE(i) offsetof(struct nvmap, bands[(i)].pamp)			/* ������� ��������� ����������� (1 ����) */
#define RMT_ATT_BASE(i) offsetof(struct nvmap, bands[(i)].att)			/* ������� ��������� ����������� (1 ����) */
#define RMT_ANTENNA_BASE(i) offsetof(struct nvmap, bands[(i)].ant)			/* ��� ���������� ������� (1 ����) */
#define RMT_MODEROW_BASE(i)	offsetof(struct nvmap, bands[(i)].moderow)			/* ����� ������ � ������� �������. */
#define RMT_MODECOLS_BASE(i, j)	offsetof(struct nvmap, bands[(i)].modecols [(j)])	/* ��������� ������� � ������ ������ �������. */
#define RMT_PWR_BASE offsetof(struct nvmap, gpwr)								/* ������� �������� sw2012sf */
#define RMT_NOTCH_BASE offsetof(struct nvmap, gnotch)							/* NOTCH filter */
//#define RMT_NOTCHFREQ_BASE offsetof(struct nvmap, gnotchfreq)							/* NOTCH filter frequency */

#define RMT_USER1_BASE offsetof(struct nvmap, guser1)
#define RMT_USER2_BASE offsetof(struct nvmap, guser2)
#define RMT_USER3_BASE offsetof(struct nvmap, guser3)
#define RMT_USER4_BASE offsetof(struct nvmap, guser4)
#define RMT_USER5_BASE offsetof(struct nvmap, guser5)

#define RMT_BWSETPOS_BASE(i) offsetof(struct nvmap, bwsetpos [(i)])

#define RMT_BWPROPSLEFT_BASE(i) offsetof(struct nvmap, bwpropsleft [(i)])
#define RMT_BWPROPSRIGHT_BASE(i) offsetof(struct nvmap, bwpropsright [(i)])
#define RMT_BWPROPSFLTSOFTER_BASE(i) offsetof(struct nvmap, bwpropsfltsofter [(i)])
#define RMT_BWPROPSAFRESPONCE_BASE(i) offsetof(struct nvmap, bwpropsafresponce [(i)])


/* ����������, ���������� �� ������� ������� - ���������� ������� ����� �����
   � ������� ���������
   */

#if WITHAMHIGHKBDADJ
/* ��������� ������ ����������� � ��  */
static void 
uif_key_click_amfmbandpassup(void)
{
	bwsetsc [BWSETI_AM].prop [0]->right100 = nextfreq(bwsetsc [BWSETI_AM].prop [0]->right100, bwsetsc [BWSETI_AM].prop [0]->right100 + 1, 1, WITHAMHIGH100MAX + 2);
	save_i8(RMT_BWPROPSRIGHT_BASE(BWPROPI_AMWIDE), bwsetsc [BWSETI_AM].prop [0]->right100);	// ������� ���� ������� �� � ������ ����
	updateboard(1, 0);
}
/* ��������� ������ ����������� � ��  */
static void 
uif_key_click_amfmbandpassdown(void)
{
	bwsetsc [BWSETI_AM].prop [0]->right100 = prevfreq(bwsetsc [BWSETI_AM].prop [0]->right100, bwsetsc [BWSETI_AM].prop [0]->right100 - 1, 1, WITHAMHIGH100MIN);
	save_i8(RMT_BWPROPSRIGHT_BASE(BWPROPI_AMWIDE), bwsetsc [BWSETI_AM].prop [0]->right100);	// ������� ���� ������� �� � ������ ����
	updateboard(1, 0);
}
// ������� �������� ������� ������� ����� �� ������� ��/�� (� ������ ����)
uint_fast8_t hamradio_get_amfm_highcut100_value(void)
{
	return bwsetsc [BWSETI_AM].prop [0]->right100;
}

#endif /* WITHAMHIGHKBDADJ */


/* ��������� ���������, ������������� ��� ����� VFO */
static uint_fast32_t gfreqs [2];		/* ������������ �� ������� ������� ������ */
#if ! WITHONEATTONEAMP
static uint_fast8_t gpamps [2];
#endif /* ! WITHONEATTONEAMP */
static uint_fast8_t gatts [2];
#if WITHANTSELECT
static uint_fast8_t gantennas [2];
#endif /* WITHANTSELECT */
static uint_fast8_t gvfosplit [2];	// At index 0: RX VFO A or B, at index 1: TX VFO A or B
// ���������, ������������ � update board
// ��� ������������� ����������.
// �� ��� ��������� ������������� ��� ������ �������� ����������, ������������ �������� � ��� �����.
static uint_fast8_t gsubmode;		/* ��� �������� ������ */
static uint_fast8_t gmode;		/* ������� ��� ������ ������� */
static uint_fast8_t gfi;			/* ����� ������� (��������) ��� �������� ������ */
static uint_fast16_t gstep;
static uint_fast16_t gencderate = 1;
static uint_fast8_t gagcmode;



#if WITHIFSHIFT
	enum { IFSHIFTTMIN = 0, IFSHIFTHALF = 3000, IFSHIFTMAX = 2 * IFSHIFTHALF };
	#if WITHIFSHIFTOFFSET
		/* ���� ��������� ����� ������ ����������� */
		static uint_fast16_t ifshifoffset = IFSHIFTHALF + WITHIFSHIFTOFFSET;		/* if shift offset value */
	#else
		static uint_fast16_t ifshifoffset = IFSHIFTHALF;		/* if shift offset value */
	#endif
	static int_fast32_t getifshiftbase(void)
	{
		return 0 - IFSHIFTHALF;
	}
#endif /* WITHIFSHIFT */

#if WITHPBT // && (LO3_SIDE != LOCODE_INVALID)
	enum { PBTMIN = 0, PBTHALF = 2560, PBTMAX = 5100 };	// �������� ��� ���������� ��������������
	static uint_fast16_t gpbtoffset = PBTHALF;		/* pbt offset value */
	static int_fast32_t getpbtbase(void)
	{
		return 0L - PBTHALF;
	}
	// ��� ����������� �� �������
	int_fast32_t hamradio_get_pbtvalue(void)
	{
		return gpbtoffset + getpbtbase();
	}
#endif /* WITHPBT */

#if (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ	/* ���������� ������� ���������� ����� ����. */
	enum { LO2AMIN = 0, LO2AHALF = 15000, LO2AMAX = 2 * LO2AHALF };
	static uint_fast16_t lo3offset = LO2AHALF;
	static int_fast32_t lo3base = (int_fast32_t) ((int_fast64_t) REFERENCE_FREQ * LO3_PLL_N / LO3_PLL_R) - LO2AHALF;
	static int_fast32_t getlo3base(void)
	{
		return lo3base;
	}
#endif	/* (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ */

// 
//static uint_fast8_t extmenu;

#if WITHUSEDUALWATCH
	uint_fast8_t mainsubrxmode;		// �����/������, A - main RX, B - sub RX
#endif /* WITHUSEDUALWATCH */

#if WITHENCODER
	static uint_fast8_t ghiresres = ENCRES_DEFAULT;		/* 5: 128 ������ � ������� ���������� ��������� */
	#if defined (ENCDIV_DEFAULT)
		static uint_fast8_t ghiresdiv = ENCDIV_DEFAULT;	/* �� ������� ��� ��������� ���������� ���������. */
	#else /* defined (ENCDIV_DEFAULT) */
		static uint_fast8_t ghiresdiv = 1;	/* �� ������� ��� ��������� ���������� ���������. */
	#endif /* defined (ENCDIV_DEFAULT) */
	static uint_fast8_t ghiresdyn = 1;
	static uint_fast8_t gbigstep = (ENCRES_24 >= ENCRES_DEFAULT);	/* �������������� ����� ����. */
#else
	static const uint_fast8_t gbigstep;
	static const uint_fast8_t ghiresdiv = 1;
#endif

static uint_fast8_t lockmode;
#if WITHLCDBACKLIGHT
	static uint_fast8_t dimmmode;
#else /* WITHLCDBACKLIGHT */
	enum { dimmmode = 0 };
#endif /* WITHLCDBACKLIGHT */
static uint_fast8_t gusefast;

#if WITHPOWERLPHP
	static uint_fast8_t gpwr;
#endif /* WITHPOWERLPHP */
#if WITHNOTCHONOFF
	static uint_fast8_t gnotch;
#elif WITHNOTCHFREQ
	static uint_fast8_t gnotch;
	static uint_fast16_t gnotchfreq = 1000;
	static uint_fast16_t gnotchwidth = 500;
#endif /* WITHNOTCHFREQ */

#if WITHSPLIT
	static uint_fast8_t gvfoab;	/* (vfoa/vfob) */
	static uint_fast8_t gsplitmode = VFOMODES_VFOINIT;	/* (vfo/vfoa/vfob/mem) */
#elif WITHSPLITEX
	static uint_fast8_t gvfoab;	/* 1: vfoa/vfob swapped */
	static uint_fast8_t gsplitmode = VFOMODES_VFOINIT;	/* (vfo/vfoa/vfob/mem) */
#else /* WITHSPLIT */
	static const uint_fast8_t gvfoab;	/* (vfoa/vfob) */
	static const uint_fast8_t gsplitmode = VFOMODES_VFOINIT;	/* (vfo/vfoa/vfob/mem) */
#endif /* WITHSPLIT */

#if WITHWARCBANDS
	enum { bandsethamwarc = 1 };	/* WARC HAM radio bands */
#else /* WITHWARCBANDS */
	enum { bandsethamwarc = 0 };	/* WARC HAM radio bands */
#endif /* WITHWARCBANDS */

//static uint_fast8_t bandsetham = 1;	/* HAM radio bands */
static uint_fast8_t bandsetbcast = 0;	/* Broadcast radio bands */
static uint_fast8_t bandset11m;
#if TUNE_6MBAND
static uint_fast8_t bandset6m = 1;	/* ������������ �� �������� 6 ������ */
#endif /* TUNE_6MBAND */
#if TUNE_4MBAND
static uint_fast8_t bandset4m = 0;	/* ������������ �� �������� 4 ������ */
#endif /* TUNE_4MBAND */
#if TUNE_2MBAND
static uint_fast8_t bandset2m = 1;	/* ������������ �� �������� 2 ����� */
#endif /* TUNE_2MBAND */

#if WITHCAT
	static uint_fast8_t catprocenable;	/* ��������� ������ ���������� CAT */
	static uint_fast8_t catstatetx;		/* ������ �������� ���������� �� �������� �� CAT ������� */
	static uint_fast8_t catstatetxdata;		/* ������ �������� ���������� �� �������� �� CAT ������� TX1 */
	static uint_fast8_t cattunemode;		/* ������ �������� ���������� �� �������� �� CAT ������� */
#else
	enum { catstatetx = 0, catstatetxdata = 0, cattunemode = 0 };
#endif /* WITHCAT */

static uint_fast8_t alignmode;		/* ������ ��� ��������� ���������� ����� (0-���������� ������) */

#if WITHUSEAUDIOREC
	#if defined (WITHBBOX) && defined (WITHBBOXREC)
		static uint_fast8_t recmode = WITHBBOXREC;	/* ������������� �������� ������ �� SD CARD ��� ��������� */
	#else /* defined (WITHBBOX) && defined (WITHBBOXREC) */
		static uint_fast8_t recmode;	/* ������������� �������� ������ �� SD CARD ��� ��������� */
	#endif /* defined (WITHBBOX) && defined (WITHBBOXREC) */
#endif /* WITHUSEAUDIOREC */

#if WITHLO1LEVELADJ
	static uint_fast8_t lo1level = WITHLO1LEVELADJINITIAL; //100;	/* ������� (���������) LO1 � ��������� */
#endif /* WITHLO1LEVELADJ */

#if defined (DEFAULT_LCD_CONTRAST)
	static uint_fast8_t gcontrast = DEFAULT_LCD_CONTRAST;
#else
	enum { gcontrast = 0 };
#endif

#if WITHDCDCFREQCTL
	//static uint_fast16_t dcdcrefdiv = 62;	/* ������� ������� ����������� ���������� 48 ��� */

	/* 
		��������� �������� ������� ��� ������������� DC-DC ����������� 
		��� ���������� ��������� � ������ ������ �������� �������� ���� �������. 
	*/
	static uint_fast16_t
	getbldivider(
		uint_fast32_t freq
		)
	{
		struct FREQ 
		{
			uint_fast16_t dcdcdiv;
			uint32_t fmin;
			uint32_t fmax;
		};
		// ���� ��� �������� �����������������. ������� ���� ���������.
		static const FLASHMEM struct FREQ freqs [] = {
		  { 63, 6900000uL,  UINT32_MAX },
		  { 62, 0,		6900000uL },	
		};

		uint_fast8_t high = (sizeof freqs / sizeof freqs [0]);
		uint_fast8_t low = 0;
		uint_fast8_t middle;	// ��������� ������

		// �������� �����
		while (low < high)
		{
			middle = (high - low) / 2 + low;
			if (freq < freqs [middle].fmin)	// ������ ������� �� ���������� - ��� ����������� ����������� ��������� ������� DCO � ������� ��������
				low = middle + 1;
			else if (freq >= freqs [middle].fmax)
				high = middle;		// ��������� � ������ � ������� ��������
			else
				goto found;
		}

	found: 
		return freqs [middle].dcdcdiv;
	}

#endif /* WITHDCDCFREQCTL */

static const uint_fast8_t displaymodesfps = DISPLAYMODES_FPS;
static uint_fast8_t displayfreqsfps = DISPLAY_FPS;
static uint_fast8_t displaybarsfps = DISPLAYSWR_FPS;

#if WITHLCDBACKLIGHT
	static uint_fast8_t bglight = WITHLCDBACKLIGHTMAX;
#else /* WITHLCDBACKLIGHT */
	enum { bglight = 0 };
#endif /* WITHLCDBACKLIGHT */

#if WITHKBDBACKLIGHT
	static uint_fast8_t kblight /* = 1 */;
#else /* WITHKBDBACKLIGHT */
	enum { kblight = 0 };
#endif /* WITHKBDBACKLIGHT */

#if LCDMODE_COLORED
	static uint_fast8_t gbluebgnd;
#else
	enum { gbluebgnd = 0 };
#endif /* LCDMODE_COLORED */


#if WITHAUTOTUNER

enum
{
 KSCH_TOHIGH = 0,
 KSCH_TOLOW,
 //
 KSCH_COUNT
};


#if SHORTSET8
	#define POSZ 8
	const FLASHMEM uint_fast8_t logtable_cap [] =
	{
		0, 1, 2, 3, 4, 5, 6, 7,	/* 0..POSZ - 1 */
		POSZ * 1, POSZ * 2, POSZ * 3,  POSZ * 4, POSZ * 5, POSZ * 6, POSZ * 7,
		POSZ * 8, POSZ * 9, POSZ * 10, POSZ * 11,  POSZ * 12, POSZ * 13, POSZ * 14, POSZ * 15,
		POSZ * 16, POSZ * 17, POSZ * 18, POSZ * 19,  POSZ * 20, POSZ * 21, POSZ * 22, POSZ * 23,
		POSZ * 24, POSZ * 25, POSZ * 26, POSZ * 27,  POSZ * 28, POSZ * 29, POSZ * 30, POSZ * 31,
	};
	const FLASHMEM uint_fast8_t logtable_ind [] =
	{
		1, 2, 3, 4, 5, 6, 7,	/* 1..POSZ - 1 */
		POSZ * 1, POSZ * 2, POSZ * 3,  POSZ * 4, POSZ * 5, POSZ * 6, POSZ * 7,
		POSZ * 8, POSZ * 9, POSZ * 10, POSZ * 11,  POSZ * 12, POSZ * 13, POSZ * 14, POSZ * 15,
		POSZ * 16, POSZ * 17, POSZ * 18, POSZ * 19,  POSZ * 20, POSZ * 21, POSZ * 22, POSZ * 23,
		POSZ * 24, POSZ * 25, POSZ * 26, POSZ * 27,  POSZ * 28, POSZ * 29, POSZ * 30, POSZ * 31,
	};

	#define CMAX (sizeof logtable_cap / sizeof logtable_cap [0] - 1)      //������������ �������� ������� ������������ �������
	#define CMIN 0        //����������� �������� ������� ������������ �������
	#define LMAX (sizeof logtable_ind / sizeof logtable_ind [0] - 1)        //������������ �������� ������������� ������� �������
	#define LMIN 0        //����������� �������� ������������� ������� �������

#elif SHORTSET7

	#define POSZ 2
	const FLASHMEM uint_fast8_t logtable_cap [] =
	{
		0, 1, /* 0..POSZ - 1 */
		POSZ * 1, POSZ * 2, POSZ * 3,  POSZ * 4, POSZ * 5, POSZ * 6, POSZ * 7,
		POSZ * 8, POSZ * 9, POSZ * 10, POSZ * 11,  POSZ * 12, POSZ * 13, POSZ * 14, POSZ * 15,
		POSZ * 16, POSZ * 17, POSZ * 18, POSZ * 19,  POSZ * 20, POSZ * 21, POSZ * 22, POSZ * 23,
		POSZ * 24, POSZ * 25, POSZ * 26, POSZ * 27,  POSZ * 28, POSZ * 29, POSZ * 30, POSZ * 31,

		POSZ * 32, POSZ * 33, POSZ * 34, POSZ * 35,  POSZ * 36, POSZ * 37, POSZ * 38, POSZ * 39,
		POSZ * 40, POSZ * 41, POSZ * 42, POSZ * 43,  POSZ * 44, POSZ * 45, POSZ * 46, POSZ * 47,
		POSZ * 48, POSZ * 49, POSZ * 50, POSZ * 51,  POSZ * 52, POSZ * 53, POSZ * 54, POSZ * 55,
		POSZ * 56, POSZ * 57, POSZ * 58, POSZ * 59,  POSZ * 60, POSZ * 61, POSZ * 62, POSZ * 62,
	};
	const FLASHMEM uint_fast8_t logtable_ind [] =
	{
		0, 1, /* 1..POSZ - 1 */
		POSZ * 1, POSZ * 2, POSZ * 3,  POSZ * 4, POSZ * 5, POSZ * 6, POSZ * 7,
		POSZ * 8, POSZ * 9, POSZ * 10, POSZ * 11,  POSZ * 12, POSZ * 13, POSZ * 14, POSZ * 15,
		POSZ * 16, POSZ * 17, POSZ * 18, POSZ * 19,  POSZ * 20, POSZ * 21, POSZ * 22, POSZ * 23,
		POSZ * 24, POSZ * 25, POSZ * 26, POSZ * 27,  POSZ * 28, POSZ * 29, POSZ * 30, POSZ * 31,

		POSZ * 32, POSZ * 33, POSZ * 34, POSZ * 35,  POSZ * 36, POSZ * 37, POSZ * 38, POSZ * 39,
		POSZ * 40, POSZ * 41, POSZ * 42, POSZ * 43,  POSZ * 44, POSZ * 45, POSZ * 46, POSZ * 47,
		POSZ * 48, POSZ * 49, POSZ * 50, POSZ * 51,  POSZ * 52, POSZ * 53, POSZ * 54, POSZ * 55,
		POSZ * 56, POSZ * 57, POSZ * 58, POSZ * 59,  POSZ * 60, POSZ * 61, POSZ * 62, POSZ * 62,
	};

	#define CMAX (sizeof logtable_cap / sizeof logtable_cap [0] - 1)      //������������ �������� ������� ������������ �������
	#define CMIN 0        //����������� �������� ������� ������������ �������
	#define LMAX (sizeof logtable_ind / sizeof logtable_ind [0] - 1)        //������������ �������� ������������� ������� �������
	#define LMIN 0        //����������� �������� ������������� ������� �������

#elif FULLSET7
	#define CMAX	127    //������������ �������� ������� ������������ �������
	#define CMIN  0      //����������� �������� ������� ������������ �������
	#define LMAX	127    //������������ �������� ������������� ������� �������
	#define LMIN  0      //����������� �������� ������������� ������� �������
#elif FULLSET8
	#define CMAX	254    //������������ �������� ������� ������������ �������
	#define CMIN  0      //����������� �������� ������� ������������ �������
	#define LMAX	254    //������������ �������� ������������� ������� �������
	#define LMIN  0      //����������� �������� ������������� ������� �������
#endif

#endif /* WITHAUTOTUNER */

#if WITHCAT

	static uint_fast8_t catenable = 1;	/* �������������� ����� ����. */
	static uint_fast8_t catbaudrate = 6;	/* 6 is a 57600 */ /* �������������� ����� ����. - ����� �������� ��� ������ �� CAT */
	static uint_fast8_t catdtrenable;	/* ���������� ����������� �� DTR CAT */
	static uint_fast8_t catdtrptt;	/* ������ ����������� �� DTR, � �� �� RTS */

	#if WITHTX
		static uint_fast8_t catrtsenable;	/* ���������� ��������� �������� �� ����� RTS CAT */
	#endif /* WITHTX */

#else /* WITHCAT */

	enum { catenable = 0 };

#endif /* WITHCAT */

#if WITHIF4DSP
	static uint_fast16_t afgain1 = BOARD_AFGAIN_MAX;	// �������� �� �� ���������
	static uint_fast16_t rfgain1 = BOARD_IFGAIN_MAX;	// �������� �� �� ���������
	static uint_fast16_t glineamp = WITHLINEINGAINMAX;	// �������� � LINE IN
	static uint_fast8_t gmikebust20db;	// ������������� ���������
	static uint_fast8_t gmikeagc = 1;	/* ��������� ����������� ��� ����� ����������� */
	static uint_fast8_t gmikeagcgain = 30;	/* ������������ �������� ��� ��������� */
	static uint_fast8_t  gmikehclip;		/* ������������ */

#if WITHUSBUAC
	static uint_fast8_t gdatamode;	/* �������� ����� � USB ������ �������� ��������� */
	uint_fast8_t hamradio_get_datamode(void) { return gdatamode; }

	static uint_fast8_t guacplayer;	/* ����� ������������� ������ ���������� � ��������� ���������� - ���������� ����� */
	#if WITHRTS96 || WITHRTS192 || WITHTRANSPARENTIQ
		static uint_fast8_t  gswapiq;		/* �������� ������� I � Q ������ � ������ RTS96 */
	#endif /* WITHRTS96 || WITHRTS192 || WITHTRANSPARENTIQ */
#endif /* WITHUSBUAC */
#if WITHAFCODEC1HAVEPROC
	static uint_fast8_t gmikeequalizer;	// ��������� ��������� ������� � ��������� (�������, ����������, ...)
	static uint_fast8_t gmikeequalizerparams [HARDWARE_CODEC1_NPROCPARAMS] = { 12, 12, 12, 12, 12 };	// ���������� 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
#endif /* WITHAFCODEC1HAVEPROC */
	static uint_fast8_t gagcoff;
#else /* WITHIF4DSP */
	static const uint_fast8_t gagcoff = 0;
#endif /* WITHIF4DSP */

#if WITHAUTOTUNER
	static uint_fast16_t tunercap;// = (CMAX - CMIN) / 2 + CMIN;
	static uint_fast16_t tunerind;// = (LMAX - LMIN) / 2 + LMIN;
	static uint_fast8_t tunertype;
	static uint_fast8_t tunerwork;	/* �������� ������ � ����������� ������� */
	static uint_fast8_t tunerdelay = 35;
#endif /* WITHAUTOTUNER */

#if WITHTX
	#if WITHSUBTONES
		// �������  Continuous Tone-Coded Squelch System or CTCSS � ��������� 0.1 �����.
		// https://en.wikipedia.org/wiki/Continuous_Tone-Coded_Squelch_System#List_of_tones
		static const FLASHMEM uint_least16_t gsubtones [] =
		{
			330,	/* 33.0 ���� #0 */
			354,	/* 35.4 ���� */
			366,	/* 36.6 ���� */
			379,	/* 37.9 ���� */
			396,	/* 39.6 ���� */
			444,	/* 44.4 ���� */
			475,	/* 47.5 ���� */
			492,	/* 49.2 ���� */
			512,	/* 51.2 ���� */
			530,	/* 53.0 ���� */
			549,	/* 54.9 ���� */
			568,	/* 56.8 ���� */
			588,	/* 58.8 ���� */
			630,	/* 63.0 ���� */
			670,	/* 67.0 ���� */
			694,	/* 69.4 ���� */
			719,	/* 71.9 ���� */
			744,	/* 74.4 ���� */
			770,	/* 77.0 ���� #18 */
			797,	/* 79.7 ���� */
			825,	/* 82.5 ���� */
			854,	/* 85.4 ���� */
			885,	/* 88.5 ���� */
			915,	/* 91.5 ���� */
			948,	/* 94.8 ���� */
			974,	/* 97.4 ���� */
			1000,	/* 100.0 ���� */
			1035,	/* 103.5 ���� */
			1072,	/* 107.2 ���� */
			1109,	/* 110.9 ���� */
			1148,	/* 114.8 ���� */
			1188,	/* 118.8 ���� */
			1230,	/* 123.0 ���� */
			1273,	/* 127.3 ���� */
			1318,	/* 131.8 ���� */
			1365,	/* 136.5 ���� */
			1413,	/* 141.3 ���� */
			1462,	/* 146.2 ���� */
			1514,	/* 151.4 ���� */
			1567,	/* 156.7 ���� */
			1598,	/* 159.8 ���� */
			1622,	/* 162.2 ���� */
			1655,	/* 165.5 ���� */
			1679,	/* 167.9 ���� */
			1713,	/* 171.3 ���� */
			1738,	/* 173.8 ���� */
			1773,	/* 177.3 ���� */
			1799,	/* 179.9 ���� */
			1835,	/* 183.5 ���� */
			1862,	/* 186.2 ���� */
			1899,	/* 189.9 ���� */
			1928,	/* 192.8 ���� */
			1966,	/* 196.6 ���� */
			1995,	/* 199.5 ���� */
			2035,	/* 203.5 ���� */
			2065,	/* 206.5 ���� */
			2107,	/* 210.7 ���� */
			2181,	/* 218.1 ���� */
			2257,	/* 225.7 ���� */
			2291,	/* 229.1 ���� */
			2336,	/* 233.6 ���� */
			2418,	/* 241.8 ���� */
			2503,	/* 250.3 ���� */
			2541,	/* 254.1 ���� */
		};

		static uint_fast8_t gsubtonei = 18;	// ������� subtone = 77.0 ����
		static uint_fast8_t gsbtonenable;	// ��������� ������������ subtone
	#endif /* WITHSUBTONES */

	#if WITHPOWERTRIM || WITHPOWERLPHP
		static uint_fast8_t gnormalpower = WITHPOWERTRIMMAX;
		static uint_fast8_t gotunerpower = WITHPOWERTRIMMIN; /* �������� ��� ������ ��������������� ������������ ���������� */
	#endif /* WITHPOWERTRIM || WITHPOWERLPHP */
	#if WITHPABIASTRIM
		#if defined (WITHBBOXPABIAS)
			static uint_fast8_t gpabias = WITHBBOXPABIAS; //WITHPABIASMIN;	/* ��� ���������� ������� ����������� */
		#else /* defined (WITHBBOXPABIAS) */
			static uint_fast8_t gpabias = 208; //WITHPABIASMIN;	/* ��� ���������� ������� ����������� */
		#endif /* defined (WITHBBOXPABIAS) */
	#endif /* WITHPABIASTRIM */
	static uint_fast8_t gtxgate = 1;		/* ���������� �������� � ���������� ��������� */
	#if WITHVOX
		static uint_fast8_t gvoxenable;	/* �������������� ����� ���� - �������������� ���������� ������������ (�� ������) */
		static uint_fast8_t gvoxlevel = 100;	/* �������������� ����� ���� - ������� ������������ VOX */
		static uint_fast8_t gavoxlevel = 100;	/* �������������� ����� ���� - ������� anti-VOX */
		static uint_fast8_t voxdelay = 30;	/* �������������� ����� ���� - �������� ���������� VOX */
	#else /* WITHVOX */
		enum { gvoxenable = 0 };	/* �������������� ����� ���� - �������������� ���������� ������������ (�� ������) */
	#endif /* WITHVOX */

	#if WITHMUTEALL
		static uint_fast8_t gmuteall;/* ��������� �������� ������. */
	#else /* WITHMUTEALL */
		enum { gmuteall = 0 };
	#endif /* WITHMUTEALL */

	#if WITHELKEY
		static uint_fast8_t bkinenable = 1;	/* �������������� ����� ���� - �������������� ���������� ������������ (�� ������������ ������������) */
		static uint_fast8_t bkindelay = 40;	/* � �������� mS. �������������� ����� ���� - �������� ���������� BREAK-IN */
	#endif /* WITHELKEY */

#if TXPATH_BIT_GATE_RX && CTLSTYLE_SW2011ALL
	static const uint_fast8_t pretxdelay = 1;	/* ������� ����, ��� ��������� ������ ������� �� ������� ��������� */
#else
	static const uint_fast8_t pretxdelay;
#endif


#if WITHBARS
	#if (WITHSWRMTR || WITHSHOWSWRPWR)
		static uint_fast16_t minforward = (1U << HARDWARE_ADCBITS) / 8;
		static uint_fast8_t swrcalibr = 100;	/* ������������� �������� SWR-����� */
	#endif /* (WITHSWRMTR || WITHSHOWSWRPWR) */
	#if WITHPWRMTR || WITHSWRMTR
		#if CTLSTYLE_SW2013RDX
			static uint_fast8_t maxpwrcali = 216;	/* ������������� �������� PWR-����� */
		#elif CTLSTYLE_SW2015
			static uint_fast8_t maxpwrcali = 216;	/* ������������� �������� PWR-����� */
		#elif CTLSTYLE_SW2018XVR
			static uint_fast8_t maxpwrcali = 216;	/* ������������� �������� PWR-����� */
		#elif CTLSTYLE_SW2016 || CTLSTYLE_SW2016VHF
			static uint_fast8_t maxpwrcali = 216;	/* ������������� �������� PWR-����� */
		#elif CTLSTYLE_SW2016MINI
			static uint_fast8_t maxpwrcali = 100;	/* ������������� �������� PWR-����� */
		#else
			static uint_fast8_t maxpwrcali = 255;	/* ������������� �������� PWR-����� */
		#endif
	#else
		static uint_fast8_t maxpwrcali = 255;	/* ������������� �������� PWR-����� */
	#endif /* WITHPWRMTR || WITHSWRMTR */

	#if WITHSWRMTR && ! WITHSHOWSWRPWR
		static uint_fast8_t swrmode = 1;
	#elif WITHPWRMTR
		static const uint_fast8_t swrmode = 0;
	#else
		//static const uint_fast8_t swrmode = 0;
	#endif
#else /* WITHBARS */
	static const uint_fast8_t swrmode = 0;
#endif /* WITHBARS */

	#if (CTLSTYLE_SW2016MINI)
		static uint_fast8_t rxtxdelay = 45;	/* � �������� mS. �������������� ����� ���� - �������� �������� ���-�������� */
		static uint_fast8_t txrxdelay = 15;	/* � �������� mS. �������������� ����� ���� - �������� �������� ��������-��� */
	#elif (CTLREGMODE_STORCH_V4)
		// modem
		static uint_fast8_t rxtxdelay = 75;	/* � �������� mS. �������������� ����� ���� - �������� �������� ���-�������� */
		static uint_fast8_t txrxdelay = 25;	/* � �������� mS. �������������� ����� ���� - �������� �������� ��������-��� */
	#else
		static uint_fast8_t rxtxdelay = 25;	/* � �������� mS. �������������� ����� ���� - �������� �������� ���-�������� */
		static uint_fast8_t txrxdelay = 25;	/* � �������� mS. �������������� ����� ���� - �������� �������� ��������-��� */
	#endif

#else /* WITHTX */
	static const uint_fast8_t bkinenable = 0;	/* �������������� ����� ���� - �������������� ���������� ������������ (�� ������������ ������������) */
	static const uint_fast8_t swrmode = 1;
	static const uint_fast8_t gvoxenable = 0;	/* �������������� ����� ���� - �������������� ���������� ������������ (�� ������) */
	static const uint_fast8_t bkindelay = 80;	/* � �������� mS. �������������� ����� ���� - �������� ���������� BREAK-IN */
#endif /* WITHTX */

#if WITHVOLTLEVEL && ! WITHREFSENSOR 

	// ���������� fullscale = VREF * 5.3 = 3.3 * 5.3 = 17.5 ������: ������ 4.3 ������, ����� 1.0 ������
	// � ����� ������� ��������: ������ 4.3 �������, ����� 1 ������.
	// ADCVREF_CPU - � ������ ����������.

	static uint_fast8_t voltcalibr = (ADCVREF_CPU * (VOLTLEVEL_UPPER + VOLTLEVEL_LOWER) + VOLTLEVEL_LOWER / 2) / VOLTLEVEL_LOWER;		// ���������� fullscale - ��� �������� ��� ADCVREF_CPU ����� �� ����� ���

#endif /* WITHVOLTLEVEL && ! WITHREFSENSOR */

#if WITHELKEY

	static uint_fast8_t elkeywpm = 20;	/* �������� ������������ ����� */
	static uint_fast8_t dashratio = 30;	/* ��������� ���� � ������������ ����� - � �������� ��������� */
	static uint_fast8_t spaceratio = 10;	/* ��������� ����� � ������������ ����� - � �������� ��������� */
	static uint_fast8_t elkeyreverse;

	#if WITHVIBROPLEX
		#if ELKEY328
			static uint_fast8_t elkeymode = 1;		/* ����� ������������ ����� - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
			static uint_fast8_t elkeyslope = 3;	/* ELKEY328 �������� ���������� ������������ ����� � ����� - �������� ����������� */
			static uint_fast8_t elkeyslopeenable = 1;
		#else
			static uint_fast8_t elkeymode;		/* ����� ������������ ����� - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
			static uint_fast8_t elkeyslope;		/* �������� ���������� ������������ ����� � ����� - �������� ����������� */
			static uint_fast8_t elkeyslopeenable = 1;
		#endif
	#else
		static uint_fast8_t elkeymode;		/* ����� ������������ ����� - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
		static uint_fast8_t elkeyslope;		/* �������� ���������� ������������ ����� � ����� - �������� ����������� */
		static uint_fast8_t elkeyslopeenable = 1;

	#endif /* WITHVIBROPLEX */
#else
	//static const uint_fast8_t elkeymode = 2;		/* ����� ������������ ����� - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
	//static const uint_fast8_t elkeyslope;		/* �������� ���������� ������������ ����� � ����� - �������� ����������� */
	//static const uint_fast8_t elkeyslopeenable;
#endif


static uint_fast8_t  stayfreq;	/* ��� ��������� ������� �������� - �� ������ ������� */

#if defined (DAC1_TYPE)
	#if defined (WITHDAC1VALDEF)
		static uint_fast8_t dac1level = WITHDAC1VALDEF;
	#else /* defined (WITHDAC1VALDEF) */
		static uint_fast8_t dac1level = (WITHDAC1VALMAX + 1 - WITHDAC1VALMIN) / 2 + WITHDAC1VALMIN;	/* �������������� ����� ����. */
	#endif /* defined (WITHDAC1VALDEF) */
#endif /* defined (DAC1_TYPE) */


#if LO1FDIV_ADJ
	static uint_fast8_t lo1powmap [2] = { LO1_POWER2, LO1_POWER2 };		/* �� ������� ����� �������� ������� ����������� ������� ���������� */
#endif
#if LO2FDIV_ADJ
	static uint_fast8_t lo2powmap [2] = { LO2_POWER2, LO2_POWER2 };		/* �� ������� ����� �������� ������� ����������� 2-�� ���������� */
#endif
#if LO3FDIV_ADJ
	static uint_fast8_t lo3powmap [2] = { LO3_POWER2, LO3_POWER2 };		/* �� ������� ����� �������� ������� ����������� 3-�� ���������� */
#endif
#if LO4FDIV_ADJ
	static uint_fast8_t lo4powmap [2] = { LO4_POWER2, LO4_POWER2 };		/* �� ������� ����� �������� ������� ����������� 4-�� ���������� */
#endif

#if LO1PHASES
	static uint_fast16_t phasesmap [2]; /* �������� [0] � ���������� [1] - ��������� ���� - � nvram phaserx � phasetx */
#endif /* LO1PHASES */

static uint_fast8_t dctxmodecw;	/* ��� �������� ��������������, ��� ������� if2 � ����� ����� 0 - ������������ ��������� �� �������� DDS */

#if WITHBARS

	#if WITHINTEGRATEDDSP
		enum { S9FENCE = - 73 };	// -73.01dBm == 50 uV rms == S9
		static uint_fast8_t s9level = UINT8_MAX + S9FENCE;					/* ������ ���������� S-����� */
		static uint_fast8_t s9delta = (6 * 8);		// 9 ������ - 8 ���������� - �� 6 ������� ������		
		static uint_fast8_t s9_60_delta = 60;		// 60 dB
	#elif (CTLSTYLE_SW2016MINI)
		static uint_fast8_t s9level = 110;			/* ������ ���������� S-����� */
		static uint_fast8_t s9delta = 70;		
		static uint_fast8_t s9_60_delta = 50;	
	#elif CTLSTYLE_SW2012CN || CTLSTYLE_SW2013SF || CTLSTYLE_SW2013RDX || CTLSTYLE_SW2012CN5 || CTLSTYLE_SW2015
		static uint_fast8_t s9level = 110;			/* ������ ���������� S-����� */
		static uint_fast8_t s9delta = 70;			
		static uint_fast8_t s9_60_delta = 50;		
	#elif CTLSTYLE_SW2016 || CTLSTYLE_SW2016VHF || CTLSTYLE_SW2018XVR
		static uint_fast8_t s9level = 110;			/* ������ ���������� S-����� */
		static uint_fast8_t s9delta = 70;			
		static uint_fast8_t s9_60_delta = 50;		
	#elif (CTLSTYLE_SW2011 || CTLSTYLE_SW2012_MINI)
		static uint_fast8_t s9level = 120;			/* ������ ���������� S-����� */
		static uint_fast8_t s9delta = 120;			
		static uint_fast8_t s9_60_delta = 120;	
	#elif CTLSTYLE_RA4YBO_V3
		static uint_fast8_t s9level = 73;			/* ������ ���������� S-����� */
		static uint_fast8_t s9delta = 15;		
		static uint_fast8_t s9_60_delta = 50;	
	#else
		static uint_fast8_t s9level = 88;			/* ������ ���������� S-����� */
		static uint_fast8_t s9delta = 34;		
		static uint_fast8_t s9_60_delta = 45;	
	#endif

#endif

#if WITHDIRECTFREQENER
	static uint_fast8_t editfreqmode;		/* ����� ������� ����� ������� */
	static uint_fast32_t editfreq;		/* �������� �������, ������� ����������� */
	static uint_fast8_t blinkpos;		/* ������� �������������� (������� 10) */
	static uint_fast8_t blinkstate = 1;		/* � ������� �������������� ����� ������������� (0 - ������) */
#endif /* WITHDIRECTFREQENER */

#if WITHRFSG
	static uint_fast8_t userfsg;		/* �������� ���������� �� ������-����������� */
#else /* WITHRFSG */
	enum { userfsg = 0 };
#endif /* WITHRFSG */

static uint_fast8_t gmoderows [2];		/* �������� ������������ ��������� ������� getbankindex_xxx(tx) */
										/* ����� ������ ������ � ����� (����� ������ ���) */
static uint_fast8_t gmodecolmaps4 [2] [4];	/* �������� 1-� ����������� ������������ ��������� ������� getbankindex_xxx(tx) */
#if WITHSPKMUTE
	static uint_fast8_t gmutespkr;		/*  ���������� �������� */
#endif /* WITHSPKMUTE */
										/* ����� ������� ������ (������ ���, ��������� ����� ������� � ������ ������) */
#if CTLSTYLE_RA4YBO
	static uint_fast8_t  gaffilter;		/* ��������� ��� �� ����� � �������� RA4YBO */
#elif CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 || CTLSTYLE_RA4YBO_V3 || KEYBSTYLE_RA4YBO_AM0
	static uint_fast8_t  guser1;
	static uint_fast8_t  guser2;
	static uint_fast8_t  guser3;
	static uint_fast8_t  guser4;
	static uint_fast8_t  guser5;
	static uint_fast8_t  gaffilter;		/* ��������� ��� �� ����� � �������� RA4YBO */
#endif /* CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 || CTLSTYLE_RA4YBO_V3 */

#if WITHTX
	static uint_fast8_t tunemode;	/* ����� ��������� ����������� ������ */
	static uint_fast8_t moxmode;	/* ��������, ���������� ������� � ���������� */
#if WITHAUTOTUNER
	static uint_fast8_t reqautotune;
#else
	enum { reqautotune = 0 };
#endif /* WITHAUTOTUNER */

#else /* WITHTX */

	enum { tunemode = 0, moxmode = 0, reqautotune = 0 };

#endif /* WITHTX */

static uint_fast8_t menuset; 	/* ����� ��������� ������� �� ������� (������������� ������� Fn) */
static uint_fast8_t dimmflag;	/* ��-0: ��������� �������. */
static uint_fast8_t sleepflag;	/* ��-0: ���������� ������� � ����. */

static uint_fast8_t amenuset(void)
{
	if ((dimmflag || sleepflag || dimmmode))
		return display_getpagesleep();
	return menuset;
}

// ������� ��������� LOCK
uint_fast8_t
hamradio_get_lockvalue(void)
{
	return lockmode;
}

// ������� ��������� FAST
uint_fast8_t
hamradio_get_usefastvalue(void)
{
	return gusefast;
}

static int_fast32_t getzerobase(void)
{
	return 0;
}

#define ADCOFFSETMID (UINT16_MAX / 2)
static int_fast32_t getadcoffsbase(void)
{
	return - ADCOFFSETMID;
}

#if WITHAFCODEC1HAVEPROC	/* ����� ����� ���������� ���������� ������������ ������� (�������, ����������, ...) */

static int_fast32_t getequalizerbase(void)
{
	return - 12;
}
#endif /* WITHAFCODEC1HAVEPROC */


#if defined(REFERENCE_FREQ)
	static const int_fast32_t refbase = REFERENCE_FREQ - OSCSHIFT;
	static uint_fast16_t refbias = OSCSHIFT;

	int_fast32_t getrefbase(void)
	{
		return refbase;
	}
#endif

#if CTLSTYLE_SW2011ALL
static uint_fast8_t gkeybeep10 = 1850 / 10;	/* ������� ������� ������ - 1850 �� */
#else /* CTLSTYLE_SW2011ALL */
static uint_fast8_t gkeybeep10 = 880 / 10;	/* ������� ������� ������ - 880 �� - ���� �� ������ ������ (A5) (���������� FT1000) */
#endif /* CTLSTYLE_SW2011ALL */

#if WITHTX
	#if WITHMIC1LEVEL
	static uint_fast16_t mik1level = WITHMIKEINGAINMAX;
	#endif /* WITHMIC1LEVEL */
	static uint_fast8_t gamdepth = 30;		/* ������� ��������� � �� - 0..100% */
	static uint_fast8_t gdacscale = 100;	/* ������������� ��������� ������� � ��� ����������� - 0..100% */
#endif /* WITHTX */

#if WITHIF4DSP

	#define FSADCPOWEROFFSET10 500
	static int_fast32_t getfsasdcbase10(void)
	{
		return - FSADCPOWEROFFSET10;
	}

	static uint_fast8_t tdsp_nfm_sql_level = 127;	// NFM NB level
	static uint_fast8_t tdsp_nfm_sql_off = 1;	// ��������� �������
	static uint_fast8_t gcwedgetime = 5;			/* ����� ����������/����� ��������� ��������� ��� �������� - � 1 �� */
	static uint_fast8_t gsidetonelevel = 15;	/* ������� ������� ������������ � ��������� - 0%..100% */
	static uint_fast8_t gsubtonelevel = 10;	/* ������� ������� CTCSS � ��������� - 0%..100% */
#if CTLSTYLE_OLEG4Z_V1
	static uint_fast8_t gdigigainmax = 120; /* �������� ������ ����������� ��������� �������� - ������������ �������� */
#else /* CTLSTYLE_OLEG4Z_V1 */
	static uint_fast8_t gdigigainmax = 86;	/* �������� ������ ����������� ��������� �������� - ������������ �������� */
#endif /* CTLSTYLE_OLEG4Z_V1 */
	static uint_fast8_t gvad605 = 180; //UINT8_MAX;	/* ���������� �� AD605 (���������� ��������� ������ �� */
	static uint_fast16_t gfsadcpower10 [2] = 
	{
		(- 130) + FSADCPOWEROFFSET10,	// ��� ������������ HDSDR ��������, ��������������� full scale �� IF ADC
		(- 330) + FSADCPOWEROFFSET10,	// � �����������
	};
	#if WITHDSPEXTDDC	/* "��������" � DSP � FPGA */
		static uint_fast8_t gdither;		/* ���������� ����������� � LTC2208 */
		#if ADC1_TYPE == ADC_TYPE_AD9246
			static uint_fast8_t gadcrand = 0;		/* ���������� ����������� � LTC2208 */
		#else /* ADC1_TYPE == ADC_TYPE_AD9246 */
			static uint_fast8_t gadcrand = 1;		/* ���������� ����������� � LTC2208 */
		#endif /* ADC1_TYPE == ADC_TYPE_AD9246 */

		static uint_fast8_t gadcfifo = 1;
		static uint_fast16_t gadcoffset = ADCOFFSETMID;
		static uint_fast8_t gdactest;
		#if WITHDACSTRAIGHT
			static uint_fast8_t gdacstraight = 1;	/* ��������� ������������ ���� ��� ��� � ������ ������������ ���� */
		#else /* WITHDACSTRAIGHT */
			static uint_fast8_t gdacstraight = 0;	/* ��������� ������������ ���� ��� ��� � ������ ��������� ���� */
		#endif /* WITHDACSTRAIGHT */
		#if WITHTXINHDISABLE
			static uint_fast8_t gtxinhenable = 0;	/* ���������� ������� �� ���� tx_inh */
		#else /* WITHTXINHDISABLE */
			static uint_fast8_t gtxinhenable = 1;	/* ���������� ������� �� ���� tx_inh */
		#endif /* WITHTXINHDISABLE */
	#endif /* WITHDSPEXTDDC */

#endif /* WITHIF4DSP */

#if WITHMODEM
	static const FLASHMEM uint_fast32_t modembr2int100 [] =	// ������ � ���� ������� �������� � gmodemspeed
	{
		3125,			// 31.25 baud		#0
		6250,			// 62.5 baud		#1
		125 * 100uL,	// 125 baud			#2
		150 * 100uL,	// 150 baud			#3
		250 * 100uL,	// 125 baud			#4
		300 * 100uL,	// 300 baud			#5
		500 * 100uL,	// 500 baud			#6
		600 * 100uL,	// 600 baud			#7
		750 * 100uL,	// 750 baud			$8
		1000 * 100uL,	// 1000 (1k) baud	#9
		1200 * 100uL,	// 1200 (1.2k) baud	#10
		2000 * 100uL,	// 2k baud			#11
		2400 * 100uL,	// 2,4k baud		#12
		3600 * 100uL,	// 3,6k baud		#13
		4000 * 100uL,	// 4,0k baud		#14
		4800 * 100uL,	// 4,8k baud		#15
		5000 * 100uL,	// 5,0k baud		#16
		5600 * 100uL,	// 5,6k baud		#17
		6000 * 100uL,	// 6,0k baud		#18
		6400 * 100uL,	// 6,4k baud		#19
		8000 * 100uL,	// 8,0k baud		#20
		9200 * 100uL,	// 9,2k baud		#21
		9600 * 100uL,	// 9,6k baud		#22
	};

	static uint_fast8_t gmodemspeed = 20;	// ������ � ������� ��������� �������� - #5: 300 baud, #7: 600 baud, #9: 1k baud
	static uint_fast8_t gmodemmode;		// ����������� ���������

	uint_fast32_t hamradio_get_modem_baudrate100(void)
	{
		return modembr2int100 [gmodemspeed];
	}

#endif /* WITHMODEM */

#if WITHAUTOTUNER

// ��� ������� ������� � ���������� ��������
typedef struct tunerstate
{
	uint8_t tunercap, tunerind, tunertype;
	uint8_t swr;	// values 0..190: SWR = 1..20
} tus_t;
#define TUS_SWRMAX (SWRMIN * 20)
#define TUS_SWR1p1 (SWRMIN * 11 / 10)	// SWR=1.1

static void board_set_tuner_group(void)
{
	//debug_printf_P(PSTR("tuner: CAP=%-3d, IND=%-3d, TYP=%d\n"), tunercap, tunerind, tunertype);
	// todo: �������� ���� ���������� �������
#if SHORTSET7 || SHORTSET8
	board_set_tuner_C(logtable_cap [tunercap]);
	board_set_tuner_L(logtable_ind [tunerind]);
	//board_set_tuner_C(1U << tunercap);
	//board_set_tuner_L(1U << tunerind);
#else /* SHORTSET7 || SHORTSET8 */
	board_set_tuner_C(tunercap);
	board_set_tuner_L(tunerind);
#endif /* SHORTSET7 || SHORTSET8 */
	board_set_tuner_type(tunertype);
	board_set_tuner_bypass(! tunerwork);
}

// ������ ���������� �� �����
static void updateboard_tuner(void)
{
	//debug_printf_P(PSTR("tuner: CAP=%-3d, IND=%-3d, TYP=%d\n"), tunercap, tunerind, tunertype);
	board_set_tuner_group();
	board_update();		/* ������� ���������������� ��������� � �������� */
	//display_redrawbars(0, 0);		/* ���������� ������������ ����� ����������� - ���������� S-����� ��� SWR-����� � volt-�����. */
}

// �������� ���������� ������� ����� ������ ���������� �� �����.
static void tuner_waitadc(void)
{
	uint_fast8_t n = (tunerdelay + 4) / 5;
	while (n --)
		local_delay_ms(5);
}

static uint_fast8_t get_swr(uint_fast8_t fullscale)
{
	adcvalholder_t r;
	const adcvalholder_t f = board_getswrmeter(& r, swrcalibr);
	const uint_fast8_t fs = fullscale - SWRMIN;

	if (f < minforward)
		return 0;	// SWR=1
	else if (f <= r)
		return fs;		// SWR is infinite

	const uint_fast16_t swr10 = (f + r) * SWRMIN / (f - r) - SWRMIN;
	return swr10 > fs ? swr10 : swr10;
}

// ���� �������� ��������� - ������� ��-0
static uint_fast8_t tuneabort(void)
{
	uint_fast8_t kbch, kbready;

	display_redrawbars(0, 0);	/* ���������� ������������ ����� ����������� - ���������� S-����� ��� SWR-����� � volt-�����. */

	processmessages(& kbch, & kbready, 0);
	if (kbready != 0)
	{
		switch (kbch)
		{
		case KBD_CODE_ATUBYPASS:
		case KBD_CODE_ATUSTART:
			return 1;
		}
	}
	return 0;
}

// ������� �������� L � ������ �������� SWR
// ���� �������� ��������� - ������� ��-0
static uint_fast8_t scanminLk(tus_t * tus, uint_fast8_t addsteps)
{
	uint_fast8_t bestswrvalid = 0;
	uint_fast8_t a = 1;	/* ����� �� ������� ���������� */

	tus->tunertype = tunertype;
	tus->tunercap = tunercap;
	for (tunerind = LMIN; tunerind <= LMAX; ++ tunerind)
	{
		if (tuneabort())
			return 1;
		updateboard_tuner();
		tuner_waitadc();
		const uint_fast8_t swr = get_swr(TUS_SWRMAX);

		if ((bestswrvalid == 0) || (tus->swr > swr))
		{
			// ��������� ��� ������
			tus->swr = swr;
			tus->tunerind = tunerind;
			bestswrvalid = 1;
			a = addsteps;
		}
		else
		{
			if (tus->swr < swr && a -- == 0)
			{
				break;
			}
		}
	}
	return 0;
}

// ������� �������� C � ������ �������� SWR
// ���� �������� ��������� - ������� ��-0
static uint_fast8_t scanminCk(tus_t * tus, uint_fast8_t addsteps)
{
	uint_fast8_t bestswrvalid = 0;
	uint_fast8_t a = 1;	/* ����� �� ������� ���������� */

	tus->tunerind = tunerind;
	tus->tunertype = tunertype;
	for (tunercap = CMIN; tunercap <= CMAX; ++ tunercap)
	{
		if (tuneabort())
			return 1;
		updateboard_tuner();
		tuner_waitadc();
		const uint_fast8_t swr = get_swr(TUS_SWRMAX);

		if ((bestswrvalid == 0) || (tus->swr > swr))
		{
			// ��������� ��� ������
			tus->swr = swr;
			tus->tunercap = tunercap;
			bestswrvalid = 1;
			a = addsteps;
		}
		else
		{
			if (tus->swr < swr && a -- == 0)
			{
				break;
			}
		}
	}
	return 0;
}

static uint_fast8_t findbestswr(const tus_t * v, uint_fast8_t n)
{
	uint_fast8_t i;
	uint_fast8_t best = 0;
	if (n < 2)
		return best;
	for (i = 1; i < n; ++ i)
	{
		if (v [i].swr < v [best].swr)
			best = i;
	}
	return best;
}

static void auto_tune(void)
{	
	const uint_fast8_t tx = 1;
	tus_t statuses [KSCH_COUNT];
	unsigned cshindex;
	const uint_fast8_t bi = getbankindex_tx(tx);
	const vindex_t b = getvfoindex(bi);

#if SHORTSET7 || SHORTSET8
	const uint_fast8_t addstepsLk = 3;
	const uint_fast8_t addstepsCk = 3;
#else /* SHORTSET7 || SHORTSET8 */
	const uint_fast8_t addstepsLk = 15;
	const uint_fast8_t addstepsCk = 15;
#endif /* SHORTSET7 || SHORTSET8 */

	//debug_printf_P(PSTR("auto_tune start\n"));
	// ������� ����������� ����� �������
	for (tunertype = 0; tunertype < KSCH_COUNT; ++ tunertype)
	{
		tunercap = CMIN;
		if (scanminLk(& statuses [tunertype], addstepsLk) != 0)
			goto aborted;
		tunerind = statuses [tunertype].tunerind;
		updateboard_tuner();

		// �������� - � ����� ��� ����� ���������� ������������?
		if (statuses [tunertype].swr <= TUS_SWR1p1)
			goto NoMoreTune;

		if (scanminCk(& statuses [tunertype], addstepsCk) != 0)
			goto aborted;
		tunercap = statuses [tunertype].tunercap;
		updateboard_tuner();

		// �������� - � ����� ��� ����� ���������� ������������?
		if (statuses [tunertype].swr <= TUS_SWR1p1)
			goto NoMoreTune;
	}
	// �������� ��������� ��������� ������������
	cshindex = findbestswr(statuses, sizeof statuses / sizeof statuses [0]);
	//debug_printf_P(PSTR("auto_tune loop done\n"));
	// ������������� ���������� � ��������� ��� ������ ����������
	tunertype = statuses [cshindex].tunertype;
	tunerind = statuses [cshindex].tunerind;
	tunercap = statuses [cshindex].tunercap;
	updateboard_tuner();
	//debug_printf_P(PSTR("auto_tune stop\n"));
NoMoreTune:

	save_i8(offsetof(struct nvmap, bands[b].tunercap), tunercap);
	save_i8(offsetof(struct nvmap, bands[b].tunerind), tunerind);
	save_i8(offsetof(struct nvmap, bands[b].tunertype), tunertype);
	return;

aborted:
	tunercap = loadvfy8up(offsetof(struct nvmap, bands[b].tunercap), CMIN, CMAX, tunercap);
	tunerind = loadvfy8up(offsetof(struct nvmap, bands[b].tunerind), CMIN, CMAX, tunerind);
	tunertype = loadvfy8up(offsetof(struct nvmap, bands[b].tunertype), 0, KSCH_COUNT - 1, tunertype);
	updateboard_tuner();
	return;
}
#endif /* WITHAUTOTUNER */

/* ��������� ���������� ����� � ��������� low..high � "���������" */
/* ������������ ��� �������� ������� �������� */
uint_fast8_t
//NOINLINEAT
calc_next(uint_fast8_t v, uint_fast8_t low, uint_fast8_t high)
{
	return (v < low || v >= high) ? low : (v + 1);
}

/* ��������� ����������� ����� � ��������� low..high � "���������" */
/* ������������ ��� �������� ������� �������� */
static uint_fast8_t 
//NOINLINEAT
calc_prev(uint_fast8_t v, uint_fast8_t low, uint_fast8_t high)
{
	return (v <= low || v > high) ? high : (v - 1);
}

/* ������������ ����� �������� �� ��������� �������, ������� ��������� ���� */
/* freq - ����� �������, step - ��� */
static uint_fast32_t 
NOINLINEAT
nextfreq(uint_fast32_t oldfreq, uint_fast32_t freq, 
							   uint_fast32_t step, uint_fast32_t top)
{
	//const ldiv_t r = ldiv(freq, step);
	//freq -= r.rem;
	freq -= (freq % step);

	if (oldfreq > freq || freq >= top)
		return top - step;

	return freq;
}

/* ������������ ����� �������� �� ���������� �������, ������� ��������� ���� */
/* freq - ����� �������, step - ��� */
static uint_fast32_t 
NOINLINEAT
prevfreq(uint_fast32_t oldfreq, uint_fast32_t freq, 
							   uint_fast32_t step, uint_fast32_t bottom)
{
	const uint_fast32_t rem = (freq % step);
	if (rem != 0)
		freq += (step - rem);
	//const ldiv_t r = ldiv(freq, step);

	//if (r.rem != 0)
	//	freq += (step - r.rem);

	if (oldfreq < freq || freq < bottom)
		return bottom;


	return freq;
}

#if defined(NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING)

/* �������� ���������� ��������� � ����������������� ������.
   0 - ���������,
	 ��-0 - ����������
	 */
static uint_fast8_t 
//NOINLINEAT
verifynvramsignature(void)
{
	uint_fast8_t i;
	for (i = 0; i < (sizeof nvramsign - 1); ++ i)
	{
		const char c = restore_i8(RMT_SIGNATURE_BASE(i));
		if (c != nvramsign [i])
		{
			return 1;	/* ���� ������� */
		}
	}		
	return 0;	/* ��������� ��������� */
}

static void 
//NOINLINEAT
initnvramsignature(void)
{
	uint_fast8_t i;

	for (i = 0; i < sizeof nvramsign - 1; ++ i)
	{
		save_i8(RMT_SIGNATURE_BASE(i), nvramsign [i]);
	}
}

/* �������� ���������� �������� ��������� � ����������������� ������.
   0 - ���������,
	 ��-0 - ����������
	 */
static uint_fast8_t 
//NOINLINEAT
verifynvrampattern(void)
{
	//const uint_fast32_t c32a = restore_i32(RMT_SIGNATURE_BASE(0));
	//const uint_fast32_t c32b = restore_i32(RMT_SIGNATURE_BASE(4));
	//debug_printf_P(PSTR("verifynvrampattern: c32a=%08lX c32b=%08lX\n"), c32a, c32b);
	uint_fast8_t i;
	for (i = 0; i < (sizeof nvramsign - 1); ++ i)
	{
		const char c = restore_i8(RMT_SIGNATURE_BASE(i));
		//debug_printf_P(PSTR("verifynvrampattern: pattern[%u]=%02X, mem=%02X\n"), i, (unsigned char) nvrampattern [i], (unsigned char) c);
		if (c != nvrampattern [i])
		{
			return 1;	/* ���� ������� */
		}
	}		
	return 0;	/* ��������� ��������� */
}

static void 
//NOINLINEAT
initnvrampattern(void)
{
	uint_fast8_t i;

	for (i = 0; i < sizeof nvramsign - 1; ++ i)
	{
		save_i8(RMT_SIGNATURE_BASE(i), nvrampattern [i]);
	}
}

#endif /* defined(NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING) */

/* ��������� �������. ��� ������ - ������ � ���� �������
*/
typedef struct submodeprops_tag
{
	uint_least8_t qflags;		/* ����� ��������� */
	uint_least8_t mode;			/* ��� ��������� ������� */
	uint_least8_t qcatmodecode;		/* Kenwood cat mode code */
	uint_fast16_t qcatwidth;		/* ������ ������, ����������� ����� CAT */
									// The FW command cannot be used in SSB or AM mode (an error tone will sound).

	char qlabel [4];
} submodeprops_t;

/* ����� ��������� */
#define QMODEF_HARDLSB	0x01		// ��� ������� ������ �� ������ �������	 (CWR, LSB)
#define QMODEF_SMARTLSB	0x02	// ��� ������� ������ �� ������ �������	 (CWR, LSB) � ����������� �� �������

// ������� ��������� � ���� ������� ������ ��������� � ��������� enum,
// � ������� �������� SUBMODE_COUNT
static const FLASHMEM submodeprops_t submodes [SUBMODE_COUNT] =
{
#if WITHMODESETSMART
	/* SUBMODE_SSBSMART */
	{	
		QMODEF_SMARTLSB,
		MODE_SSB, 	/* ������ ��������� ������� */
		2,			/* Kenwood cat mode code */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"SSB", 
	},
	/* SUBMODE_CWSMART */
	{	
		QMODEF_SMARTLSB,
		MODE_CW, 	/* ������ ��������� ������� */
		3,			/* Kenwood cat mode code */
		500,		/* ������ ������� ������, ������������ ����� CAT */
		"CW ", 
	},
	/* SUBMODE_CWZSMART */
	{	
		QMODEF_SMARTLSB,
		MODE_CWZ, 	/* ������ ��������� ������� */
		3,			/* Kenwood cat mode code */
		500,		/* ������ ������� ������, ������������ ����� CAT */
		"CWZ", 
	},
	/* SUBMODE_DIGISMART */
	{	
		QMODEF_SMARTLSB,
		MODE_DIGI, 	/* ������ ��������� ������� */
		9,			/* Kenwood cat mode code */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"DIG", 
	},
#else /* WITHMODESETSMART */
	/* SUBMODE_USB */
	{	
		0,
		MODE_SSB, 	/* ������ ��������� ������� */
		2,			/* Kenwood cat mode code */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"USB", 
	},
	/* SUBMODE_LSB */
	{	
		QMODEF_HARDLSB,
		MODE_SSB, 	/* ������ ��������� ������� */
		1,			/* Kenwood cat mode code */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"LSB", 
	},
	/* SUBMODE_CW */
	{	
		0,
		MODE_CW, 	/* ������ ��������� ������� */
		3,			/* Kenwood cat mode code */
		500,		/* ������ ������� ������, ������������ ����� CAT */
		"CW ", 
	},
	/* SUBMODE_CWR */
	{	
		QMODEF_HARDLSB,
		MODE_CW, 	/* ������ ��������� ������� */
		7,			/* Kenwood cat mode code */
		500,		/* ������ ������� ������, ������������ ����� CAT */
		"CWR", 
	},
	/* SUBMODE_CWZ */
	{	
		QMODEF_SMARTLSB,
		MODE_CWZ, 	/* ������ ��������� ������� */
		UINT8_MAX,	/* Kenwood cat mode code - �� ���� ����� ����� CAT �� ������������� */
		0,			/* ������ ������� ������, ������������ ����� CAT */
	  	"CWZ", 
	},
	/* SUBMODE_AM */
	{	
		QMODEF_SMARTLSB,
		MODE_AM, 	/* ������ ��������� ������� */
		5,			/* Kenwood cat mode code */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"AM ",
	},
	#if WITHSAM
	/* SUBMODE_SAM */
	{	
		QMODEF_SMARTLSB,
		MODE_SAM, 	/* ������ ��������� ������� */
		UINT8_MAX,	/* Kenwood cat mode code */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"SAM",
	},
	#endif /* WITHSAM */
	/* SUBMODE_DRM */
	{	
		QMODEF_SMARTLSB,
		MODE_DRM, 	/* ������ ��������� ������� */
		UINT8_MAX,	/* Kenwood cat mode code - �� ���� ����� ����� CAT �� ������������� */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"DRM", 
	},
	/* SUBMODE_DGL */
	{	
		QMODEF_HARDLSB,
		MODE_DIGI, 	/* ������ ��������� ������� */
		9,			/* Kenwood cat mode code */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"DGL", 
	},
	/* SUBMODE_DGU */
	{	
		QMODEF_SMARTLSB,
		MODE_DIGI, 	/* ������ ��������� ������� */
		6,			/* Kenwood cat mode code */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"DGU", 
	},
#if WITHMODESETFULLNFM || WITHMODESETMIXONLY3NFM || WITHMODESETFULLNFMWFM	// ����� ����� CAT �� ��������� ���� ������ - ���� ��� - � �����������.
	/* SUBMODE_NFM */
	{	
		QMODEF_SMARTLSB,
		MODE_NFM, 	/* ������ ��������� ������� */
		4,			/* Kenwood cat mode code */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"NFM",
	},
#endif /* WITHMODESETFULLNFM || WITHMODESETMIXONLY3NFM || WITHMODESETFULLNFMWFM  */
#if WITHWFM || WITHMODESETFULLNFMWFM
	/* SUBMODE_WFM */
	{	
		QMODEF_SMARTLSB,
		MODE_WFM, 	/* ������ ��������� ������� */
		8,			/* Kenwood cat mode code - �������� ����. � ��������� ������ ���. */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"WFM",
	},
#endif /* WITHWFM || WITHMODESETFULLNFMWFM */
#if WITHMODEM
	/* SUBMODE_PSK */
	{	
		0,
		MODE_MODEM, /* ������ ��������� ������� */
		UINT8_MAX,	/* Kenwood cat mode code */
		500,		/* ������ ������� ������, ������������ ����� CAT */
		"PSK", 
	},
#endif /* WITHMODEM */
	/* SUBMODE_ISB */
	{	
		0,
		MODE_ISB, 	/* ������ ��������� ������� */
		UINT8_MAX,	/* Kenwood cat mode code - �� ���� ����� ����� CAT �� ������������� */
		0,			/* ������ ������� ������, ������������ ����� CAT */
	  	"ISB", 
	},
#if WITHFREEDV
	/* SUBMODE_FDV */
	{	
		0,
		MODE_FREEDV,/* ������ ��������� ������� */
		UINT8_MAX,	/* Kenwood cat mode code */
		0,			/* ������ ������� ������, ������������ ����� CAT */
		"FDV", 
	},
#endif /* WITHFREEDV */
#endif /* WITHMODESETSMART */
};

// ���������� ������� ������ � LSB ������� ������.
static uint_fast8_t
getsubmodelsb(
	uint_fast8_t submode,
	uint_fast8_t forcelsb
	)
{
	const uint_fast8_t flags = submodes [submode].qflags;
#if WITHMODESETSMART
	if ((flags & QMODEF_SMARTLSB) != 0)
		return forcelsb; 
#endif /* WITHMODESETSMART */
	return (flags & QMODEF_HARDLSB) != 0;
}

#if WITHCAT
enum 
{ 
	CATSTATE_HALTED,
	CATSTATE_WAITPARAM,		/* ��������� �������� ����� ��������� (��������� �� ����� ';' */
#if WITHCATEXT && WITHELKEY
	CATSTATE_WAITMORSE,		/* ��������� �������� ����� ������� �� KY */
#endif /* WITHCATEXT && WITHELKEY */
	CATSTATE_WAITCOMMAND1,	/* ��������� �������� ����� ������� ����� ������� */
	CATSTATE_WAITCOMMAND2	/* ��������� �������� ����� ������� ����� ������� */
};

enum 
{ 
	CATSTATEO_HALTED,
	CATSTATEO_SENDREADY,
	CATSTATEO_WAITSENDREPLAY	/* ���������� �������� ����� �� �������� - ���� �� ���������� */
};

#endif /* WITHCAT */

#if WITHCAT

/* ����� �� ������������� ������ */
static uint_fast8_t 
findkenwoodsubmode(uint_fast8_t catmode, uint_fast8_t defsubmode)
{
	uint_fast8_t submode;

	for (submode = 0; submode < (sizeof submodes / sizeof submodes [0]); submode ++)
	{
		const uint_fast8_t v = submodes [submode].qcatmodecode;
		if (v == UINT8_MAX)
			continue;		/* �� ���� ����� ����� CAT �� ������������� */
		if (v == catmode)
			return submode;
	}
	return defsubmode;
}

// ��� ������� ��������� CAT �� ���������� ������ ����������� ��� SSB �������.
// TODO: �������� ��� �������� ������
static uint_fast16_t
getkenwoodfw(
	uint_fast8_t submode,	// current submode
	uint_fast8_t fi)	// filter index
{
	return submodes [submode].qcatwidth;
}

static uint_fast8_t cat_getstateout(void);
static void cat_answer_forming(void);

#endif /* WITHCAT */

#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT

static uint_fast8_t dimmtime;	/* ���������� ������ �� ������� ����������, 0 - �� �����. ������������ �� ����. */
static uint_fast8_t dimmcount;
static uint_fast8_t dimmflagch;	/* ��-0: ���������� ��������� dimmflag */

#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */

#if WITHFANTIMER

#define FANPATIMEMAX	240
static uint_fast8_t fanpatime = 15;	/* ���������� ������ �� ���������� ����������� ����� ��������, 0 - �� �����. ������������ �� ����. */
static uint_fast8_t fanpacount = FANPATIMEMAX;
static uint_fast8_t fanpaflag = 1;	/* ��-0: ��������� ����������. */
static uint_fast8_t fanpaflagch;	/* ��-0: ���������� ��������� fanpaflag */

#endif /* WITHFANTIMER */


#if WITHSLEEPTIMER

static uint_fast8_t sleeptime;	/* ���������� ����� �� ����������, 0 - �� ���������. ������������ �� ����. */
static uint_fast16_t sleepcount;	/* ������� � �������� */
static uint_fast8_t sleepflagch;	/* ��-0: ���������� ��������� sleepflag */

#else
	//enum { sleepflag = 0 };

#endif /* WITHSLEEPTIMER */


/* ��������� �����-�� ���������� �� ������� ������������, �������� ���������. 
	���� ���� ��������� "���" - ���������� 1 */
static uint_fast8_t
board_wakeup(void)
{
	uint_fast8_t r = 0;
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
	dimmcount = 0;		/* ������� ������� ������������ */
	if (dimmflag != 0)	
	{
		dimmflag = 0;
		dimmflagch = 1;
		r = 1;
	}
#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */
#if WITHSLEEPTIMER
	sleepcount = 0;		/* ������� ������� ������������ */
	if (sleepflag != 0)	
	{
		sleepflag = 0;
		sleepflagch = 1;
		r = 1;
	}
#endif /* WITHSLEEPTIMER */
	return r;
}

/* ��������� ��������� �� ������ ������������ ���������� � user-level ��������. */
/* ��������� ������ ������������ ��������� - ��� ��������� �������������, ��������. */
uint_fast8_t 
//NOINLINEAT
kbd_scan(uint_fast8_t * key)
{
	uint_fast8_t f = 0;
	uint8_t * buff;
	switch (takemsgready_user(& buff))
	{
	case MSGT_EMPTY:
		return 0;

	case MSGT_KEYB:
		* key = buff [0];
		f = 1;
		break;

	default:
		break;
	}
	releasemsgbuffer_user(buff);
	return f;
}

/* �������� PBT offset ��� �������� ������ ������ */
/* TODO: ������� ��������� �� �������� ������� */
static int_fast16_t 
getpbt(
	const filter_t * workfilter,
	uint_fast8_t mode,		/* ��� ������ ������ */
	uint_fast8_t tx				/* ������� �������� */
	)
{	
#if WITHPBT // && (LO3_SIDE != LOCODE_INVALID)
	if (tx || mdt [mode].wpbt == 0)
		return 0;
	return gpbtoffset + getpbtbase();	/* �� ������������� �������� �������� �������� */

#else /* WITHPBT */
	(void) mode;
	(void) tx;
	return 0;
#endif /* WITHPBT */
}
/* �������� IF SHIFT offset ��� �������� ������ ������ */
/* TODO: ������� ��������� �� �������� ������� */
// ���������� �������� ��������� ������� �������� ����� ������� � ����� ������� �������
static int_fast16_t 
getifshift(
	const filter_t * workfilter,
	uint_fast8_t mode,		/* ��� ������ ������ */
	uint_fast8_t tx				/* ������� �������� */
	)
{	
#if WITHIFSHIFT
	if (tx || mdt [mode].wifshift == 0)
		return 0;
	return ifshifoffset + getifshiftbase();	/* �� ������������� �������� �������� �������� */

#else /* WITHIFSHIFT */

	(void) workfilter;
	(void) mode;
	(void) tx;
	return 0;

#endif /* WITHIFSHIFT */
}

static uint_fast32_t 
loadvfy32freq(
	vindex_t b		// band
	)
{
	const vindex_t b0 = (b >= MBANDS_BASE) ? VFOS_BASE : b;
	return loadvfy32(RMT_BFREQ_BASE(b), get_band_bottom(b0), get_band_top(b0), get_band_init(b0));
}

#if 0
static const uint_fast8_t ssb_steps10 [] = 
{
	1,	/* 10 Hz */
	5,	/* 50 Hz */
	10,	/* 100 Hz */
};
static const uint_fast8_t am_steps10 [] = 
{
	10,	/* 100 Hz */
	50,	/* 500 Hz */
	100,/* 1 kHz */
	125,/* 1.25 kHz */
	150,/* 1.5 kHz - for 9 kHz implementation */
	250,/* 2.5 kHz */
};
#endif

/* ������� �������� � ����� ���������� ������ ������ */
static const FLASHMEM struct modetempl *
NOINLINEAT
getmodetempl(uint_fast8_t submode)
{
	return & mdt [submodes [submode].mode];
}

// ��������, ������������ �� ��������� ��������� � ������ ����� � ������� ������������.
// ������� 0 - �� ������������
static uint_fast8_t
//NOINLINEAT
existingband(
	uint_fast8_t b	// ��� ���������
	)
{
	const uint_fast8_t bandset = get_band_bandset(b);

	if (get_band_bottom(b) >= TUNE_TOP || get_band_top(b) < TUNE_BOTTOM)
		return 0;

	switch (bandset)
	{
	default:
	case BANDSETF_ALL:
		return 1;		// ������������ ������
	case BANDSETF_HAM:
		return ! bandsetbcast;
	case BANDSETF_HAMWARC:
		return bandsethamwarc && ! bandsetbcast;
	case BANDSETF_BCAST:
		return bandsetbcast;
	case BANDSETF_CB:
		return bandset11m;
#if TUNE_6MBAND
	case BANDSETF_6M:
		return bandset6m && ! bandsetbcast;		// ������������ ��� ��� - ������������ ����
#endif /* TUNE_6MBAND */
#if TUNE_4MBAND
	case BANDSETF_4M:
		return bandset4m && ! bandsetbcast;		// ������������ ��� ��� - ������������ ����
#endif /* TUNE_4MBAND */

	// 144 � 430 ����������� ����� ������� � ����.
#if TUNE_2MBAND
	case BANDSETF_2M:
		return bandset2m && ! bandsetbcast;		// ������������ ��� ��� - ������������ ����
#endif /* TUNE_2MBAND */
#if TUNE_07MBAND
	case BANDSETF_07M:
		return bandset2m && ! bandsetbcast;		// ������������ ��� ��� - ������������ ����
#endif /* TUNE_2MBAND */
	}
}



static void 
//NOINLINEAT
verifyband(const vindex_t b)
{
#if DEBUGEXT
	if (b == ((uint_fast8_t) - 1))
	{
		TP();
		display_gotoxy(0, 1);
		display_string("band not found", 1);
		for (;;)
			;
	}
#endif /* DEBUGEXT */
}

/* �������� ����� ���������, � ������� �������� �������������� �������
  ���� �� ����������� �� ������ ���������, ������� ((uint8_t)-1) */
static vindex_t 
//NOINLINEAT
getfreqband(const uint_fast32_t freq)
{
	vindex_t i;

	for (i = 0; i < (sizeof bandsmap / sizeof bandsmap [0]); ++ i)
	{
		if (! existingband(i))	// �������� � ������ ������������ �� ������������
			continue;
		if (get_band_bottom(i) <= freq && get_band_top(i) > freq)
			return i;
	}
	return ((vindex_t) - 1);
}

/* �������� ����� ��������� � ������� ��������, �� ������� ����������.
  ���� ��� ����������, ������� low */
static vindex_t 
//NOINLINEAT
getnexthband(const uint_fast32_t freq)
{
	enum { LOW = 0, HIGH = HBANDS_COUNT - 1 };
	vindex_t i;

	for (i = 0; i < HBANDS_COUNT; ++ i)
	{
		if (! existingband(i))	// �������� � ������ ������������ �� ������������
			continue;
		if (get_band_top(i) > freq)
			return i;
	}
	return LOW;
}


#if	WITHDIRECTBANDS
/* �������� ����� ������������� ���������, ���������� � ������. ���� � ������ ������ ��� �� ������ ���������,
 ������� ����� ��������.
 */
static vindex_t 
//NOINLINEAT
getnextbandingroup(const vindex_t b, const uint_fast8_t bandgroup)
{
	enum { LOW = 0, HIGH = HBANDS_COUNT - 1 };

	vindex_t i = b;		// ��������� ��������
	do
	{
		i = i == HIGH ? LOW : (i + 1);	// ������� � ���������� ���������
		if (! existingband(i))	// �������� � ������ ������������ �� ������������
			continue;
		if (bandsmap [i].bandgroup == bandgroup)
			break;			// �������� ��� �� ������
	} while (i != b);
	return i;
}
#endif	/* WITHDIRECTBANDS */

/* �������� ����� ��������� � ������� ��������, �� ������� ����������.
  ���� ��� ����������, ������� high */
static vindex_t 
//NOINLINEAT
getprevhband(const uint_fast32_t freq)
{
	vindex_t i;

	for (i = 0; i < HBANDS_COUNT; ++ i)
	{
		if (! existingband(i))	// �������� � ������ ������������ �� ������������
			continue;
		if (get_band_bottom(i) > freq)
		{
			i = (i == 0) ? HBANDS_COUNT : i;
			break;
		}
	}
	// ������� ������ ���������� ����������.
	do
		i = calc_prev(i, 0, HBANDS_COUNT - 1);
	while (! existingband(i));
	return i;
}


static vindex_t
//NOINLINEAT
getnext_ham_band(
	vindex_t b,		// ������� ��������
	const uint_fast32_t freq
	)
{
	uint_fast8_t i;
	vindex_t xsel [XBANDS_COUNT];
	vindex_t xnext [XBANDS_COUNT];
	vindex_t xprev [XBANDS_COUNT];

	for (i = 0; i < XBANDS_COUNT; ++ i)
	{
		const uint_fast32_t f = loadvfy32freq(XBANDS_BASE0 + i);	// ������� � �������� ���������
		xsel [i] = getfreqband(f);			// �� ����������� �� ������� ������-�� ���������
		xnext [i] = getnexthband(f);		// �������� ����� ��������� � ������� ��������
		xprev [i] = getprevhband(f);		// �������� ����� ��������� � ������� ��������
	}

	do
	{
		if (
			b == XBANDS_BASE0 && 
			xprev [0] == xprev [1] && 
			xnext [0] == xnext [1] && 
			xsel [1] >= HBANDS_COUNT)
		{
			/* ��������� �������� "�� ��������� - � �������� ��������",
			���� ����������� ������� ������ ��������� ��������� �� �������� �� ���������� �������� */
			b = XBANDS_BASE1;
			continue;
		}
		if (b == xprev [0] && xsel [0] >= HBANDS_COUNT)
		{
			// ������� �������� �������������� ��� xfreq [0]
			/* ��������� � �������� �������� 0 */
			b = XBANDS_BASE0;
			continue;
		}
		if (b == xprev [1] && xsel [1] >= HBANDS_COUNT)
		{
			// ������� �������� �������������� ��� xfreq [1]
			/* ��������� � �������� �������� 1 */
			b = XBANDS_BASE1;
			continue;
		}
		if (b < HBANDS_COUNT)
		{
			/* ������� ������� ��������� � ������������ ���������� */
			do
				b = calc_next(b, 0, HBANDS_COUNT - 1);
			while (! existingband(b));
			continue;
		}
		if (b == XBANDS_BASE0)
		{
			// ������� ������� - �������� 0
			b = xnext [0];
			continue;
		}
		if (b == XBANDS_BASE1)
		{
			// ������� ������� - �������� 1
			b = xnext [1];
			continue;
		}

		b = getnexthband(freq);
	} while (0);
	return b;
}


static vindex_t
//NOINLINEAT
getprev_ham_band(
	vindex_t b,		// ������� ��������
	const uint_fast32_t freq
	)
{
	uint_fast8_t i;
	vindex_t xsel [XBANDS_COUNT];
	vindex_t xnext [XBANDS_COUNT];
	vindex_t xprev [XBANDS_COUNT];

	for (i = 0; i < XBANDS_COUNT; ++ i)
	{
		const uint_fast32_t f = loadvfy32freq(XBANDS_BASE0 + i);	// ������� � �������� ���������
		xsel [i] = getfreqband(f);			// �� ����������� �� ������� ������-�� ���������
		xnext [i] = getnexthband(f);		// �������� ����� ��������� � ������� ��������
		xprev [i] = getprevhband(f);		// �������� ����� ��������� � ������� ��������
	}

	do
	{
		if (
			b == XBANDS_BASE1 && 
			xprev [0] == xprev [1] && 
			xnext [0] == xnext [1] && 
			xsel [0] >= HBANDS_COUNT
			)
		{
			/* ��������� �������� "�� ��������� - � �������� ��������",
			���� ����������� ������� ������ ��������� ��������� �� �������� �� ���������� �������� */
			b = XBANDS_BASE0;
			continue;

		}
		if (b == xnext [0] && xsel [0] >= HBANDS_COUNT)
		{
			/* ��������� � �������� �������� 0 */
			b = XBANDS_BASE0;
			continue;
		}
		if (b == xnext [1] && xsel [1] >= HBANDS_COUNT)
		{
			/* ��������� � �������� �������� 1 */
			b = XBANDS_BASE1;
			continue;
		}
		if (b < HBANDS_COUNT)
		{
			/* ������� ������� ��������� � ������������ ���������� */
			do
				b = calc_prev(b, 0, HBANDS_COUNT - 1);
			while (! existingband(b));
			continue;
		}
		if (b == (XBANDS_BASE0))
		{
			// ������� ������� - �������� 0
			b = xprev [0];
			continue;
		}
		if (b == (XBANDS_BASE1))
		{
			// ������� ������� - �������� 1
			b = xprev [1];
			continue;
		}

		b = getprevhband(freq);
	} while (0);
	return b;
}

static uint_fast8_t
getbankindex_raw(const uint_fast8_t n)
{
	ASSERT(n < 2);
	return (gvfoab + n) % 2;
}

// ���������������� ������� ��� �������� ������
static uint_fast8_t
getbankindex_pathi(const uint_fast8_t pathi)
{
	ASSERT(pathi < 2);

#if WITHSPLIT || WITHSPLITEX

	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	case VFOMODES_VFOINIT:
		return getbankindex_raw(0);

	case VFOMODES_VFOSPLIT:	
		if (gtx == 0)
			return getbankindex_raw(pathi);
		else
			return getbankindex_tx(1);
	default:
		ASSERT(0);
		return 0;
	}

#else /* WITHSPLIT || WITHSPLITEX */
	return getbankindex_raw(0);
#endif /* WITHSPLIT || WITHSPLITEX */
}

static uint_fast8_t
getbankindex_ab(
	const uint_fast8_t ab	// 0: A, 1: B
	)
{
	ASSERT(ab < 2);
#if WITHSPLIT || WITHSPLITEX

	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	case VFOMODES_VFOINIT:
		return getbankindex_raw(0);

	case VFOMODES_VFOSPLIT:	
		return getbankindex_raw(ab);
	default:
		ASSERT(0);
		return getbankindex_raw(0);
	}

#else /* WITHSPLIT || WITHSPLITEX */
	return getbankindex_raw(0);
#endif /* WITHSPLIT || WITHSPLITEX */
}

static uint_fast8_t
getbankindex_tx(const uint_fast8_t tx)
{
	ASSERT(tx < 2);
#if WITHSPLIT || WITHSPLITEX

	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	case VFOMODES_VFOINIT:
		return getbankindex_raw(0);

	case VFOMODES_VFOSPLIT:	
		return getbankindex_raw(tx);
	default:
		ASSERT(0);
		return getbankindex_raw(0);
	}

#else /* WITHSPLIT || WITHSPLITEX */
	return getbankindex_raw(0);
#endif /* WITHSPLIT || WITHSPLITEX */
}

// �������� bankindex ��� ������ ������� ��� ������ ������ ������ �� ������� � ����� A (0) � B (1)
static uint_fast8_t
//NOINLINEAT
getbankindex_ab_fordisplay(const uint_fast8_t ab)
{
	ASSERT(ab < 2);
#if WITHSPLIT
	return getbankindex_ab(gtx != ab);
#elif WITHSPLITEX
	return getbankindex_raw(gtx != ab);
#else /* WITHSPLIT */
	return getbankindex_raw(0);
#endif /* WITHSPLIT */
}

#if WITHUSEDUALWATCH

static uint_fast8_t
getactualmainsubrx(void)
{
#if WITHSPLIT || WITHSPLITEX

	switch (gsplitmode)
	{
	default:
		return BOARD_RXMAINSUB_A_A;

	case VFOMODES_VFOSPLIT:
		return mainsubrxmodes [mainsubrxmode].code;
	}
#else /* WITHSPLIT || WITHSPLITEX */

	return BOARD_RXMAINSUB_A_A;

#endif /* WITHSPLIT || WITHSPLITEX */
}

#endif /* WITHUSEDUALWATCH */

// VFO mode
const char * hamradio_get_vfomode3_value(uint_fast8_t * flag)
{
	static const char spl [] = "SPL";
	static const char sp3 [] = "   ";

	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	default:
	case VFOMODES_VFOINIT:	/* no SPLIT -  ������� ����������� */
		* flag = 0;
		return sp3;
	case VFOMODES_VFOSPLIT:	
		* flag = 1;
		return spl;
		//return (gvfoab != tx) ? b : a;
	}
}

// VFO mode
const char * hamradio_get_vfomode5_value(uint_fast8_t * flag)
{
	static const char spl [] = "SPLIT";
	static const char sp5 [] = "     ";

	switch (gsplitmode)	/* (vfo/vfoa/vfob/mem) */
	{
	default:
	case VFOMODES_VFOINIT:	/* no SPLIT -  ������� ����������� */
		* flag = 0;
		return sp5;
	case VFOMODES_VFOSPLIT:	
		* flag = 1;
		return spl;
		//return (gvfoab != tx) ? b : a;
	}
}

// ������� VFO A ��� ���������� ������
uint_fast32_t hamradio_get_freq_rx(void)
{
	return gfreqs [getbankindex_tx(0)];
}

// ������� VFO A ��� ����������� �� �������
uint_fast32_t hamradio_get_freq_a(void)
{
	return gfreqs [getbankindex_ab_fordisplay(0)];
}
// SSB/CW/AM/FM/..
const FLASHMEM char * hamradio_get_mode_a_value_P(void)
{
	return submodes [getsubmode(getbankindex_ab_fordisplay(0))].qlabel;
}

// SSB/CW/AM/FM/..	
const FLASHMEM char * hamradio_get_mode_b_value_P(void)
{
	return submodes [getsubmode(getbankindex_ab_fordisplay(1))].qlabel;
}

// ������� VFO B ��� ����������� �� �������	
uint_fast32_t hamradio_get_freq_b(void)
{
	return gfreqs [getbankindex_ab_fordisplay(1)];
}

	enum { withonlybands = 0 };

#if WITHONLYBANDS
	#error Remove WITHONLYBANDS from configuration - not supported
#endif /* WITHONLYBANDS */

/* ������� ��� ���������� ������� ����������� ������ � ������������ ���������� */
static int_fast32_t
tune_top(const uint_fast8_t bi)
{
#if WITHONLYBANDS
	if (withonlybands != 0)
		return tune_top_active [bi];
#endif /* WITHONLYBANDS */
	return TUNE_TOP;
}

/* ������� ��� ���������� ������� ����������� ������ � ������������ ���������� */
static int_fast32_t
tune_bottom(const uint_fast8_t bi)
{
#if WITHONLYBANDS
	if (withonlybands != 0)
		return tune_bottom_active [bi];
#endif /* WITHONLYBANDS */
	return TUNE_BOTTOM;
}

/* ����������� �������� ���� � ��������������� ����� ���������� ���������� */
static void 
copybankstate(
	const uint_fast8_t sbi,	// source bank index
	const uint_fast8_t tbi,	// target bank index
	const int_fast32_t deltaf	// ���������� ����������� ��� ��������� �������� "auto split"
	)
{
	gfreqs [tbi] = vfy32up(gfreqs [sbi] + deltaf, tune_bottom(sbi), tune_top(sbi) - 1, gfreqs [sbi]);
	gmoderows [tbi] = gmoderows [sbi];
#if ! WITHONEATTONEAMP
	gpamps [tbi] = gpamps [sbi];
#endif /* ! WITHONEATTONEAMP */
	gatts [tbi] = gatts [sbi];
#if WITHANTSELECT
	gantennas [tbi] = gantennas [sbi];
#endif /* WITHANTSELECT */

	memcpy(gmodecolmaps4 [tbi], gmodecolmaps4 [sbi], sizeof gmodecolmaps4 [tbi]);
}

/* ��������� ��� ������� ��������� � ��������������� ��������, ������ ������ ��� VFO. */
static void
//NOINLINEAT
savebandfreq(const vindex_t b, const uint_fast8_t bi)
{
	//debug_printf_P(PSTR("savebandfreq: b=%d, bi=%d, freq=%ld\n"), b, bi, (unsigned long) gfreqs [bi]);
	verifyband(b);

	save_i32(RMT_BFREQ_BASE(b), gfreqs [bi]);	/* ��������� � ������� ��������� ������� */
}

/* ��������� ��� ��������� ��������� (����� �������) � ��������������� ��������, ������ ������ ��� VFO. */
static void
//NOINLINEAT
savebandstate(const vindex_t b, const uint_fast8_t bi)
{
	//debug_printf_P(PSTR("savebandstate: b=%d, bi=%d, freq=%ld\n"), b, bi, (unsigned long) gfreqs [bi]);
	verifyband(b);

	save_i8(RMT_MODEROW_BASE(b), gmoderows [bi]);

	uint_fast8_t i;
	for (i = 0; i < MODEROW_COUNT; ++ i)
		save_i8(RMT_MODECOLS_BASE(b, i), gmodecolmaps4 [bi] [i]);

#if ! WITHONEATTONEAMP
	save_i8(RMT_PAMP_BASE(b), gpamps [bi]);
#endif /* ! WITHONEATTONEAMP */
	save_i8(RMT_ATT_BASE(b), gatts [bi]);
#if WITHANTSELECT
	save_i8(RMT_ANTENNA_BASE(b), gantennas [bi]);
#endif /* WITHANTSELECT */
#if WITHAUTOTUNER
	save_i8(offsetof(struct nvmap, bands[b].tunercap), tunercap);
	save_i8(offsetof(struct nvmap, bands[b].tunerind), tunerind);
	save_i8(offsetof(struct nvmap, bands[b].tunertype), tunertype);
#endif /* WITHAUTOTUNER */
}

static void savebandgroup(uint_fast8_t b)
{
#if	WITHDIRECTBANDS
	const uint_fast8_t bandgroup = bandsmap [b].bandgroup;
	if (bandgroup != BANDGROUP_COUNT)
		save_i8(RMT_BANDGROUP(bandgroup), b);
#endif /* WITHDIRECTBANDS */
}

/* ������� �� ������� �����, ��������, �������� modecolmap ���� ����������������. */
static uint_fast8_t
NOINLINEAT
getmodecol(
	uint_fast8_t index, 
	uint_fast8_t upper, // moderow
	uint_fast8_t def,
	uint_fast8_t bi)		/* bank index */
{
	uint_fast8_t v = gmodecolmaps4 [bi] [index];
	if (v > upper)
	{
		gmodecolmaps4 [bi] [index] = def;
		return def;
	}
	return v;
}

/* ������ ����� �������� � ������� ����� */
static void
//NOINLINEAT
putmodecol(
	const uint_fast8_t index, 	// moderow
	const uint_fast8_t v,
	const uint_fast8_t bi		/* bank index */
	)
{
	gmodecolmaps4 [bi] [index] = v;
}

/* ��������� ������ ������ "�� ���������" ��� �������. */
static uint_fast8_t
//NOINLINEAT
getdefaultsubmode(
	uint_fast32_t freq	/* ������� (����������� �� �������) */
	)
{
#if WITHMODESETSMART
	return SUBMODE_SSBSMART;
#else
	return freq < BANDFUSBFREQ ? SUBMODE_LSB : SUBMODE_USB;
#endif
}


/* ��������� ������ ������ "�� ���������" ��� �������. */
static uint_fast8_t
//NOINLINEAT
getdefaultbandsubmode(
	uint_fast32_t freq	/* ������� (����������� �� �������) */
	)
{
	const uint_fast8_t b = getfreqband(freq);
	if (b < HBANDS_COUNT)
		return get_band_defsubmode(b);
	return getdefaultsubmode(freq);
}

#if WITHIF4DSP

	static uint_fast8_t gagc_rate [AGCSETI_COUNT]; // = 10;	// �� gagc_rate �� ��������� �������� ������� 1 �� ���������
	static uint_fast8_t gagc_t1 [AGCSETI_COUNT]; // = 120;	// in 1 mS steps. 120=120 mS	charge slow
	static uint_fast8_t gagc_release10 [AGCSETI_COUNT]; // = 5;		// in 0.1 S steps. 0.5 S discharge slow - ����� ������� ��������� ���� ���
	static uint_fast8_t gagc_t4 [AGCSETI_COUNT]; // = 50;	// in 1 mS steps. 35=35 mS discharge fast - ����� ������� ������� ���� ���
	static uint_fast8_t gagc_thung10 [AGCSETI_COUNT]; // = 3;	// 0.1 S hung time (0.3 S recomennded).

#endif /* WITHIF4DSP */
 
#if CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 || CTLSTYLE_RA4YBO_V3

	static uint_fast16_t gtxpower [MODE_COUNT];
	static uint_fast16_t gtxcompr [MODE_COUNT];

#endif /* CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2*/

#if WITHIF4DSP

	static uint_fast8_t gtxaudio [MODE_COUNT];

#endif /* WITHIF4DSP */

#if WITHIF4DSP

// ��������� �������� �������� �� NVRAM
// �������� ������� ������ ������ ����������� � �������� ������ ��� ���� ������� ������
// ���������� ���������� ��� ����������� � ����������� ������� ������� BW
static void
bwseti_load(void)
{
	uint_fast8_t bwprop;
	for (bwprop = 0; bwprop < BWPROPI_count; ++ bwprop)
	{
		// �������� ������ ����� �����������
		bwprop_t * const p = bwprops [bwprop];

		//p->fltsofter = loadvfy8up(RMT_BWPROPSFLTSOFTER_BASE(bwprop), WITHFILTSOFTMIN, WITHFILTSOFTMAX, p->fltsofter);
		switch (p->type)
		{
		case BWSET_NARROW:
			p->left10_width10 = loadvfy8up(RMT_BWPROPSLEFT_BASE(bwprop), 10, 180, p->left10_width10);
			break;
		default:
		case BWSET_WIDE:
			p->left10_width10 = loadvfy8up(RMT_BWPROPSLEFT_BASE(bwprop), 5, 70, p->left10_width10);
			p->right100 = loadvfy8up(RMT_BWPROPSRIGHT_BASE(bwprop), 8, 60, p->right100);
			break;
		}
	}
	uint_fast8_t bwseti;
	for (bwseti = 0; bwseti < BWSETI_count; ++ bwseti)
	{
		// ������ ��������� ������
		bwsetpos [bwseti] = loadvfy8up(RMT_BWSETPOS_BASE(bwseti), 0, bwsetsc [bwseti].last, bwsetpos [bwseti]);
	}
}


/* �������� ���������� ��������� ������ */
static void
agcseti_load(void)
{
	uint_fast8_t agcseti;
	for (agcseti = 0; agcseti < AGCSETI_COUNT; ++ agcseti)
	{
		// ��������� ���
		gagc_rate [agcseti] = loadvfy8up(offsetof(struct nvmap, afsets [agcseti].agc_rate), 1, UINT8_MAX - 1, aft [agcseti].agc_rate);
		gagc_t1 [agcseti] = loadvfy8up(offsetof(struct nvmap, afsets [agcseti].agc_t1), 10, 250, aft [agcseti].agc_t1);
		gagc_release10 [agcseti] = loadvfy8up(offsetof(struct nvmap, afsets [agcseti].agc_release10), 1, 100, aft [agcseti].agc_release10);
		gagc_t4 [agcseti] = loadvfy8up(offsetof(struct nvmap, afsets [agcseti].agc_t4), 10, 250, aft [agcseti].agc_t4);
		gagc_thung10 [agcseti] =	loadvfy8up(offsetof(struct nvmap, afsets [agcseti].agc_thung10), 0, 250, aft [agcseti].agc_thung10);
	}
}


/* ������ �� NVRAM ���������� �������� ��������� ������� ����� ����������� */
static void micproc_load(void)
{
	uint_fast8_t proci;
	uint_fast8_t i;

	for (proci = 0; proci < NMICPROFILES; ++ proci)
	{
		mikproc_t * const p = & micprofiles [proci];

		p->comp = loadvfy8up(offsetof(struct nvmap, gmicprocs [proci].comp), 0, 1, p->comp);
		p->complevel = loadvfy8up(offsetof(struct nvmap, gmicprocs [proci].complevel), 0, 1, p->complevel);
	}

	/* �������� �������� - ������� �������������� ��� ����� ������� ������ */
	for (i = 0; i < TXAPROFIG_count; ++ i)
	{
		gtxaprofiles [i] = loadvfy8up(RMT_TXAPROFIGLE_BASE(i), 0, NMICPROFILES - 1, gtxaprofiles [i]);
	}
}


#endif /* WITHIF4DSP */


///////////////////////////
//
// ������ �� ������ ����������

// ��� ��������� �������� ���������
enum
{
	ISTEP1 = 1,
	ISTEP2 = 2,
	ISTEP5 = 5,
	ISTEP10 = 10,
	ISTEP50 = 50,
	ISTEP100 = 100,
	//ISTEPG,
	//
};

// ������ ������ ����������� �������� ���������
enum
{
	RJ_YES = 128,	/* �������� � ���� rj, ��� ������� ���������� ��� Yes/No */
	RJ_ON,			/* �������� � ���� rj, ��� ������� ���������� ��� On/Off */
	RJ_CATSPEED	,	/* ����������� �������� CAT */
	RJ_ELKEYMODE,	/* ����� ������������ ����� - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
	RJ_POW2,		/* �������� - ������� ������. ������������ ��������� */
	RJ_ENCRES,		/* �������� - ������ � ������� ���������� ��������� */
	RJ_SUBTONE,		/* �������� - ������ � ������� ������ �������� */
	RJ_TXAUDIO,		/* �������� - �������� ����� ��� �������� */
	RJ_MDMSPEED,	/* �������� - �������� ������ */
	RJ_MDMMODE,		/* �������� - ��� ��������� ������ */
	RJ_MONTH,		/* �������� - ����� 1..12 */
	RJ_POWER,		/* ���������� �������� HP/LP */
	//
	RJ_notused
};

#if WITHENCODER2

struct enc2menu
{
	char label [10];
	uint8_t rj;
	uint8_t istep;
	uint16_t bottom, upper;	/* ����������� �� ������������� �������� (upper - �������) */

	nvramaddress_t nvram;				/* ���� MENUNONVRAM - ������ ������ � ������ */

	uint_fast16_t * pval16;			/* ����������, ������� ������������ - ���� ��� 16 ��� */
	uint_fast8_t * pval8;			/* ����������, ������� ������������  - ���� ��� 8 ���*/
	int_fast32_t (* funcoffs)(void);	/* ��� ����������� � ������������� ����������� ����� ������ */
	void (* adjust)(const FLASHMEM struct enc2menu * mp, int_least16_t nrotate);
};

/* ������� ��� ���������� �������� ��������� */
static void
enc2savemenuvalue(
	const FLASHMEM struct enc2menu * mp
	)
{
	const nvramaddress_t nvram = mp->nvram;
	const uint_fast16_t * const pv16 = mp->pval16;
	const uint_fast8_t * const pv8 = mp->pval8;

	if (nvram == MENUNONVRAM)
		return;
	if (pv16 != NULL)
	{
		save_i16(nvram, * pv16);		/* ��������� ����������������� �������� */
	}
	else if (pv8 != NULL)
	{
		save_i8(nvram, * pv8);		/* ��������� ����������������� �������� */
	}
	else
	{
		ASSERT(0);
	}
}

/* ������� ��� ��������� �������� ��������� */
static 
void
enc2menu_adjust(
	const FLASHMEM struct enc2menu * mp,
	int_least16_t nrotate	/* �������� ����� - �� ������� �������� �������� */
	)
{
	//const nvramaddress_t nvram = mp->nvram;
	const uint_fast16_t step = mp->istep;
	uint_fast16_t * const pv16 = mp->pval16;
	uint_fast8_t * const pv8 = mp->pval8;

	/* ���������� ��������� */
	if (nrotate < 0)
	{
		// negative change value
		const uint_fast32_t bottom = mp->bottom;
		if (pv16 != NULL)
		{
			* pv16 =
				prevfreq(* pv16, * pv16 - (- nrotate * step), step, bottom);
		}
		else if (pv8 != NULL)
		{
			* pv8 =
				prevfreq(* pv8, * pv8 - (- nrotate * step), step, bottom);
		}
		enc2savemenuvalue(mp);
	}
	else if (nrotate > 0)
	{
		// positive change value
		const uint_fast32_t upper = mp->upper;
		if (pv16 != NULL)
		{
			* pv16 =
				nextfreq(* pv16, * pv16 + (nrotate * step), step, upper + (uint_fast32_t) step);
		}
		else
		{
			* pv8 =
				nextfreq(* pv8, * pv8 + (nrotate * step), step, upper + (uint_fast32_t) step);
		}
		enc2savemenuvalue(mp);
	}
}

static const FLASHMEM struct enc2menu enc2menus [] =
{
#if WITHELKEY && ! WITHPOTWPM
	{
		"CW SPEED ",
		0,		// rj
		ISTEP1,
		CWWPMMIN, CWWPMMAX,		// minimal WPM = 10, maximal = 60 (also changed by command KS).
		offsetof(struct nvmap, elkeywpm),
		NULL,
		& elkeywpm,
		getzerobase, /* ������������ �� ��������� � ������������ */
		enc2menu_adjust,	/* ������� ��� ��������� �������� ��������� */
	},
#endif /* WITHELKEY && ! WITHPOTWPM */
#if WITHTX
#if WITHPOWERTRIM
	{
		"TX POWER ",
		0,		// rj
		ISTEP1,
		WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
		offsetof(struct nvmap, gnormalpower),
		NULL,
		& gnormalpower,
		getzerobase, /* ������������ �� ��������� � ������������ */
		enc2menu_adjust,	/* ������� ��� ��������� �������� ��������� */
	},
#elif WITHIF4DSP
	{
		"DAC SCALE",
		0,		// rj
		ISTEP1,
		0, 100,
		offsetof(struct nvmap, gdacscale),
		NULL,
		& gdacscale,
		getzerobase, /* ������������ �� ��������� � ������������ */
		enc2menu_adjust,	/* ������� ��� ��������� �������� ��������� */
	},
#endif /* WITHPOWERTRIM */
#if WITHNOTCHFREQ
	{
		"NOTCH FRQ",
		0,		// rj
		ISTEP50,
		WITHNOTCHFREQMIN, WITHNOTCHFREQMAX,
		offsetof(struct nvmap, gnotchfreq),	/* ����������� ������� NOTCH */
		& gnotchfreq,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
		enc2menu_adjust,	/* ������� ��� ��������� �������� ��������� */
	},
#endif /* WITHNOTCHFREQ */
#if WITHSUBTONES
	{
		"CTCSS FRQ", 
		RJ_SUBTONE,		// rj
		ISTEP1,	//  Continuous Tone-Coded Squelch System or CTCSS freq
		0, sizeof gsubtones / sizeof gsubtones [0] - 1, 
		offsetof(struct nvmap, gsubtonei),
		NULL,
		& gsubtonei,
		getzerobase, /* ������������ �� ��������� � ������������ */
		enc2menu_adjust,	/* ������� ��� ��������� �������� ��������� */
	},
#endif /* WITHPOWERTRIM */
#if WITHMIC1LEVEL
	{
		"MIKE LEVL",
		0,	
		ISTEP1,		/* ���������� �������� ������������ ��������� ����� ����. */
		WITHMIKEINGAINMIN, WITHMIKEINGAINMAX, 
		offsetof(struct nvmap, mik1level),	/* �������� ������������ ��������� */
		& mik1level,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
		enc2menu_adjust,	/* ������� ��� ��������� �������� ��������� */
	},
#endif /* ITHMIC1LEVEL */
#if WITHIF4DSP
	{
		"MIKE CLIP",
		0,
		ISTEP1,	
		0, 90, 					/* ����������� */
		offsetof(struct nvmap, gmikehclip),
		NULL,
		& gmikehclip,
		getzerobase, /* ������������ �� ��������� � ������������ */
		enc2menu_adjust,	/* ������� ��� ��������� �������� ��������� */
	},
#endif /* WITHIF4DSP */
#endif /* WITHTX */
#if WITHIF4DSP
	{
		"SQUELCH  ", 
		0,		// rj
		ISTEP1,		/* squelch level */
		0, SQUELCHMAX, 
		offsetof(struct nvmap, gsquelch),	/* ������� ������� ����� �������� ����������� ������� */
		NULL,
		& gsquelch,
		getzerobase, /* ������������ �� ��������� � ������������ */
		enc2menu_adjust,	/* ������� ��� ��������� �������� ��������� */
	},
#endif /* WITHIF4DSP */
};

/* ��������� �������� �������������� ��������� */
static 
const FLASHMEM char * 
enc2menu_label_P(
	uint_fast8_t item
	)
{
	const FLASHMEM struct enc2menu * const p = & enc2menus [item];

	return p->label;
}

/* ��������� �������� �������������� ��������� */
static 
const char * 
enc2menu_value(
	uint_fast8_t item
	)
{
	enum { WDTH = 9 };	// ������ ���� ��� �����������
	const FLASHMEM struct enc2menu * const mp = & enc2menus [item];
	static char b [WDTH + 1];	// �� ���� ����� ������������ ���������
	int_fast32_t value;

	//const nvramaddress_t nvram = mp->nvram;
	if (mp->pval16 != NULL)
	{
		value = mp->funcoffs() + * mp->pval16;
	}
	else if (mp->pval8 != NULL)
	{
		value = mp->funcoffs() + * mp->pval8;
	}
	else
	{
		ASSERT(0);
		value = mp->bottom;	/* ����� �� ������� ���������� */
	}

	switch (mp->rj)
	{
	case RJ_SUBTONE:
		local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("%*u.%1u"), WDTH - 2, gsubtones [value] / 10, gsubtones [value] % 10);
		break;
	case RJ_YES:
		local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("%*s"), WDTH, value ? "YES" : "NO");
		break;
	case RJ_ON:
		local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("%*s"), WDTH, value ? "ON" : "OFF");
		break;
	default:
		local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("%*u"), WDTH, value);
		break;
	}
	return b;
}

enum
{
	ENC2STATE_INITIALIZE,
	ENC2STATE_SELECTITEM,
	ENC2STATE_EDITITEM,
	//
	ENC2STATE_COUNT
};

#define ENC2POS_COUNT (sizeof enc2menus / sizeof enc2menus [0])

static const FLASHMEM char text_nul9_P [] = "         ";

static uint_fast8_t enc2state = ENC2STATE_INITIALIZE;
static uint_fast8_t enc2pos;	// �������� ����� ����

#define RMT_ENC2STATE_BASE offsetof(struct nvmap, enc2state)
#define RMT_ENC2POS_BASE offsetof(struct nvmap, enc2pos)

/* ������� �� ������ �������� */
static void
uif_encoder2_press(void)
{
	switch (enc2state)
	{
	case ENC2STATE_INITIALIZE:
		enc2state = ENC2STATE_SELECTITEM;
		break;
	case ENC2STATE_SELECTITEM:
		enc2state = ENC2STATE_EDITITEM;
		break;
	case ENC2STATE_EDITITEM:
		enc2state = ENC2STATE_SELECTITEM;
		break;
	}
	save_i8(RMT_ENC2STATE_BASE, enc2state);
	display_redrawmodes(1);
}

/* ���������� ������� �� ������ �������� - ����� �� ������ �������������� */
static void
uif_encoder2_hold(void)
{
	switch (enc2state)
	{
	case ENC2STATE_INITIALIZE:
		enc2state = ENC2STATE_EDITITEM;
		break;
	case ENC2STATE_SELECTITEM:
		enc2state = ENC2STATE_INITIALIZE;
		break;
	case ENC2STATE_EDITITEM:
		enc2state = ENC2STATE_INITIALIZE;
		break;
	}
	save_i8(RMT_ENC2STATE_BASE, enc2state);
	display_redrawmodes(1);
}

/* ��������� �������� ������� ��������� */
static uint_fast8_t
uif_encoder2_rotate(
	int_least16_t nrotate	/* �������� ����� - �� ������� �������� �������� */
	)
{
	if (nrotate == 0)
		return 0;

	switch (enc2state)
	{
	case ENC2STATE_SELECTITEM:
		/* ����� ��������� ��� �������������� */
		while (nrotate != 0)
		{
			if (nrotate > 0)
			{
				enc2pos = calc_next(enc2pos, 0, ENC2POS_COUNT - 1);
				-- nrotate;
			}
			else
			{
				enc2pos = calc_prev(enc2pos, 0, ENC2POS_COUNT - 1);
				++ nrotate;
			}
		}
		save_i8(RMT_ENC2POS_BASE, enc2pos);
		return 1;

	case ENC2STATE_EDITITEM:
		if (nrotate != 0)
		{
			const FLASHMEM struct enc2menu * const mp = & enc2menus [enc2pos];
			mp->adjust(mp, nrotate);	// ��������� � ���������� �������� ���������
			updateboard(1, 0);
			return 1;
		}

	default:
		break;
	}
	return 0;
}

#else /* WITHENCODER2 */

/* �������� */
static uint_fast8_t
uif_encoder2_rotate(
	int_least16_t nrotate	/* �������� ����� - �� ������� �������� �������� */
	)
{
	return 0;
}

#endif /* WITHENCODER2 */


// FUNC item label
void display_fnlabel9(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHENCODER2
	const char FLASHMEM * const text = enc2menu_label_P(enc2pos);
	switch (enc2state)
	{
	case ENC2STATE_INITIALIZE:
		display_1state_P(x, y, text_nul9_P);
		break;
	case ENC2STATE_SELECTITEM:
		display_2states_P(x, y, 0, text, text);
		break;
	case ENC2STATE_EDITITEM:
		display_2states_P(x, y, 1, text, text);
		break;
	}
#endif /* WITHENCODER2 */
}

// FUNC item value
void display_fnvalue9(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHENCODER2
	const char * const text = enc2menu_value(enc2pos);
	switch (enc2state)
	{
	case ENC2STATE_INITIALIZE:
		display_1state_P(x, y, text_nul9_P);
		break;
	case ENC2STATE_SELECTITEM:
		display_2states(x, y, 0, text, text);
		break;
	case ENC2STATE_EDITITEM:
		display_2states(x, y, 1, text, text);
		break;
	}
#endif /* WITHENCODER2 */
}

// split, lock, s-meter display
static void
loadsavedstate(void)
{
	lockmode = loadvfy8up(RMT_LOCKMODE_BASE, 0, 1, lockmode);	/* ����������� ������� ���������� ��������� */
#if WITHLCDBACKLIGHT
	dimmmode = loadvfy8up(RMT_DIMMMODE_BASE, 0, 1, dimmmode);	/* ���������� ��������� ������� � ���������� */
#endif /* WITHLCDBACKLIGHT */
#if WITHUSEFAST
	gusefast = loadvfy8up(RMT_USEFAST_BASE, 0, 1, gusefast);	/* ������������ � ����� �������� ���� */
#endif /* WITHUSEFAST */
#if WITHUSEDUALWATCH
	mainsubrxmode = loadvfy8up(RMT_MAINSUBRXMODE_BASE, 0, MAINSUBRXMODE_COUNT - 1, mainsubrxmode);	/* ��������� dual watch */
#endif /* WITHUSEDUALWATCH */
#if WITHPOWERLPHP
	gpwr = loadvfy8up(RMT_PWR_BASE, 0, PWRMODE_COUNT - 1, gpwr);
#endif /* WITHPOWERLPHP */
#if WITHNOTCHONOFF
	gnotch = loadvfy8up(RMT_NOTCH_BASE, 0, NOTCHMODE_COUNT - 1, gnotch);
#elif WITHNOTCHFREQ
#if WITHENCODER2
	enc2state = loadvfy8up(RMT_ENC2STATE_BASE, ENC2STATE_INITIALIZE, ENC2STATE_COUNT - 1, enc2state);	/* ����������� ����� �������������� ���������� ������ ���������� */
	enc2pos = loadvfy8up(RMT_ENC2POS_BASE, 0, ENC2POS_COUNT - 1, enc2pos);	/* ����������� ����� ��������� ��� �������������� ������ ���������� */
#endif /* WITHENCODER2 */
	// ���������� ������������ ����� ���� - ��� �� �����.
	// ������, ���/���� ����� ����������...
	//gnotch = loadvfy8up(RMT_NOTCH_BASE, 0, NOTCHMODE_COUNT - 1, gnotch);
	//gnotchfreq = loadvfy16up(RMT_NOTCHFREQ_BASE, WITHNOTCHFREQMIN, WITHNOTCHFREQMAX, gnotchfreq);
#endif /* WITHNOTCHONOFF */
	menuset = loadvfy8up(RMT_MENUSET_BASE, 0, display_getpagesmax(), menuset);		/* ����������� ����� �������, � ������� �������� ������ */
#if WITHSPLIT
	gsplitmode = loadvfy8up(RMT_SPLITMODE_BASE, 0, VFOMODES_COUNT - 1, gsplitmode); /* (vfo/vfoa/vfob/mem) */
	gvfoab = loadvfy8up(RMT_VFOAB_BASE, 0, VFOS_COUNT - 1, gvfoab); /* (vfoa/vfob) */
#elif WITHSPLITEX
	gsplitmode = loadvfy8up(RMT_SPLITMODE_BASE, 0, VFOMODES_COUNT - 1, gsplitmode); /* (vfo/vfoa/vfob/mem) */
	gvfoab = loadvfy8up(RMT_VFOAB_BASE, 0, VFOS_COUNT - 1, gvfoab); /* (vfoa/vfob) */
#endif /* WITHSPLIT */
#if WITHSPKMUTE
	gmutespkr = loadvfy8up(RMT_MUTELOUDSP_BASE, 0, 1, gmutespkr);	/*  ���������� �������� */
#endif /* WITHSPKMUTE */
#if CTLSTYLE_RA4YBO
	gaffilter = loadvfy8up(RMT_AFFILTER_BASE, 0, 1, gaffilter);	/* ��������� ��� �� ����� � �������� RA4YBO */
#elif CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 || KEYBSTYLE_RA4YBO_AM0
	guser1 = loadvfy8up(RMT_USER1_BASE, 0, 1, guser1);
	guser2 = loadvfy8up(RMT_USER2_BASE, 0, 1, guser2);
	guser3 = loadvfy8up(RMT_USER3_BASE, 0, 1, guser3);
	guser4 = loadvfy8up(RMT_USER4_BASE, 0, 1, guser4);
	guser5 = loadvfy8up(RMT_USER5_BASE, 0, 1, guser5);
#elif CTLSTYLE_RA4YBO_V3
	guser1 = loadvfy8up(RMT_USER1_BASE, 0, 1, guser1);
	guser4 = loadvfy8up(RMT_USER4_BASE, 0, 1, guser4);
	guser5 = loadvfy8up(RMT_USER5_BASE, 0, 1, guser5);
#endif /* CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 || CTLSTYLE_RA4YBO_V3 */
#if WITHAUTOTUNER
	tunerwork = loadvfy8up(offsetof(struct nvmap, tunerwork), 0, 1, tunerwork);
#endif /* WITHAUTOTUNER */

#if WITHIF4DSP

	#if WITHUSBUAC && WITHTX
		gdatamode = loadvfy8up(RMT_DATAMODE_BASE, 0, 1, gdatamode);
	#endif /* WITHUSBUAC && WITHTX */
	// �������� ������� ������ ������ ����������� � �������� ������ ��� ���� ������� ������
	// ���������� ���������� ��� ����������� � ����������� ������� ������� BW
	bwseti_load();

	
	agcseti_load();	/* �������� ���������� ��������� ������ */
	micproc_load();	/* ������ �� NVRAM ���������� �������� ��������� ������� ����� ����������� */

#endif /* WITHIF4DSP */

	/* �������� ����������, ��������� �� ������ */
	uint_fast8_t mode;
	for (mode = 0; mode < MODE_COUNT; ++ mode)
	{
	#if CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V3
		// ���������� � �������� �������� �� ������� ������.
		gtxpower [mode] = loadvfy16up(RMT_TXPOWER_BASE(mode), 0, 255, 255);
		gtxcompr [mode] = loadvfy16up(RMT_TXCOMPR_BASE(mode), 0, 255, 255);
	#endif /* CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V3 */
	#if WITHIF4DSP
		// �������� �����
		gtxaudio [mode] = loadvfy8up(RMT_TXAUDIO_BASE(mode), 0, BOARD_TXAUDIO_count - 1, mdt [mode].txaudio);
	#endif /* WITHIF4DSP */
	}
}

/* �� ��������� �������� ��� ��������� (� �������) ������ ��������� */
static void 
//NOINLINEAT
loadnewband(
	vindex_t b,
	uint_fast8_t bi
	)
{
	ASSERT(bi < 2);

	gfreqs [bi] = loadvfy32freq(b);
	//debug_printf_P(PSTR("loadnewband: b=%d, bi=%d, freq=%ld\n"), b, bi, (unsigned long) gfreqs [bi]);
#if WITHONLYBANDS
	const vindex_t hb = getfreqband(gfreqs [bi]);
	tune_bottom_active [bi] = get_band_bottom(hb);
	tune_top_active [bi] = get_band_top(hb);
#endif
#if ! WITHONEATTONEAMP
	gpamps [bi] = loadvfy8up(RMT_PAMP_BASE(b), 0, PAMPMODE_COUNT - 1, DEFPREAMPSTATE);	/* ����������� ������� ��������� ������������� */
#endif /* ! WITHONEATTONEAMP */
	gatts [bi] = loadvfy8up(RMT_ATT_BASE(b), 0, ATTMODE_COUNT - 1, 0);	/* ����������� ������� ��������� ����������� */
#if WITHANTSELECT
	gantennas [bi] = loadvfy8up(RMT_ANTENNA_BASE(b), 0, ANTMODE_COUNT - 1, 0);	/* ����������� ����� ���������� ������� */
#endif /* WITHANTSELECT */
	const uint_fast8_t defsubmode = getdefaultbandsubmode(gfreqs [bi]);		/* ����� ��-��������� ��� ������� - USB ��� LSB */
	uint_fast8_t defrow;
	const uint_fast8_t  defcol = locatesubmode(defsubmode, & defrow);	/* ������/������� ��� SSB . � ��� ������ ���� �� �������? */

	// ����������� ����� ������ �� ��������� ��� ������� ���������
	gmodecolmaps4 [bi] [defrow] = loadvfy8up(RMT_MODECOLS_BASE(b, defrow), 0, modes [defrow][0] - 1, defcol);

	gmoderows [bi] = loadvfy8up(RMT_MODEROW_BASE(b), 0, MODEROW_COUNT - 1, defrow);

	uint_fast8_t i;
	for (i = 0; i < MODEROW_COUNT; ++ i)
	{
		gmodecolmaps4 [bi] [i] = loadvfy8up(RMT_MODECOLS_BASE(b, i), 0, 255, 255);	// ����� ������������� 255 - ����� ��� �������.
	}
#if WITHAUTOTUNER
	// todo: �������� ���� ���������� �������
	tunercap = loadvfy8up(offsetof(struct nvmap, bands[b].tunercap), CMIN, CMAX, tunercap);
	tunerind = loadvfy8up(offsetof(struct nvmap, bands[b].tunerind), CMIN, CMAX, tunerind);
	tunertype = loadvfy8up(offsetof(struct nvmap, bands[b].tunertype), 0, KSCH_COUNT - 1, tunertype);
#endif /* WITHAUTOTUNER */
}

/* �������� ������� submode ��� ���������� �����
*/
static uint_fast8_t
//NOINLINEAT
getsubmode(
	uint_fast8_t bi		/* vfo bank index */
   )
{
	ASSERT(bi < 2);
#if ELKEY328
	return SUBMODE_CW;//328
#else /* ELKEY328 */
	const uint_fast8_t moderow = gmoderows [bi];
	const uint_fast8_t modecol = getmodecol(moderow, modes [moderow][0] - 1, 0, bi);	/* ������� �� ������� �����, ��������, �������� modecolmap ���� ����������������. */
	return modes [moderow][modecol + 1];	/* ������� �� ������� ����� */
#endif /* ELKEY328 */
}

/* ������� ���������� �� updateboard ��� ���������� ���������� �����
 * ������������� "������������" ��������� ������ ������ -
   gsubmode, gstep, gagcmode, gfi, gmode.
*/
static void
//NOINLINEAT
setgsubmode(
	uint_fast8_t submode
   )
{
	/* ���������� ������, ��� � ��� ����������� � ������������ � ����� ������� ������ */
	/* ����� ������� */
	const FLASHMEM struct modetempl * const pmodet = getmodetempl(submode);
	const uint_fast8_t mode = submodes [submode].mode;
	const uint_fast8_t deffilter = getdefflt(mode, 0);	/* �������� ������ ������� "�� ���������" ��� ������ */
	
	gsubmode = submode;
	gmode = mode;
	gfi = getsuitablerx(mode, loadvfy8up(RMT_FILTER_BASE(mode), 0, getgfasize() - 1, deffilter));	/* ������ ��� ������ ����� */
#if ! WITHAGCMODENONE
	gagcmode = loadvfy8up(RMT_AGC_BASE(mode), 0, AGCMODE_COUNT - 1, pmodet->defagcmode);
#endif /* ! WITHAGCMODENONE */
	if (gusefast || gbigstep)
	{
		gstep = pmodet->step10 [1] * 10;
		gencderate = 1;
	}
	else
	{
		gstep = pmodet->step10 [0] * 10;
		gencderate = gstep / STEP_MINIMAL;
	}
}
#if ! WITHAGCMODENONE
#endif /* ! WITHAGCMODENONE */

#if WITHMODEM

void 
modemchangefreq(
	uint_fast32_t f		// �������, ������� ������������� �� ������� �� CAT
	)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);
	gfreqs [bi] = vfy32up(f, TUNE_BOTTOM, TUNE_TOP - 1, gfreqs [bi]);

#if WITHONLYBANDS
	const vindex_t hb = getfreqband(gfreqs [bi]);
	tune_bottom_active [bi] = get_band_bottom(hb);
	tune_top_active [bi] = get_band_top(hb);
#endif
	//gpamps [bi] = loadvfy8up(RMT_PAMP_BASE(b), 0, PAMPMODE_COUNT - 1, DEFPREAMPSTATE);	/* ����������� ������� ��������� ������������� */
	//gatts [bi] = loadvfy8up(RMT_ATT_BASE(b), 0, ATTMODE_COUNT - 1, 0);	/* ����������� ������� ��������� ����������� */
	//gantennas [bi] = loadvfy8up(RMT_ANTENNA_BASE(b), 0, ANTMODE_COUNT - 1, 0);	/* ����������� ��� ���������� ������� */
	updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
}

void 
modemchangespeed(
	uint_fast32_t speed100		// �������, ������� ������������� �� ������� �� CAT
	)
{
	uint_fast8_t i;

	for (i = 0; i < sizeof modembr2int100 / sizeof sizeof modembr2int100 [0]; ++ i)
	{
		if (speed100 == modembr2int100 [i])
		{
			gmodemspeed = i;
			updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
			return;
		}
	}
}


void 
modemchangemode(
	uint_fast32_t modemmode		// �������, ������� ������������� �� ������� �� CAT
	)
{
	if (modemmode < 2)	/* 0: BPSK, 1: QPSK */
	{
		gmodemmode = modemmode;
		updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
	}
}


#endif /* WITHMODEM */

#if WITHCAT
/* ������� ��� cat */
/* �� ��������� �������� ��� ��������� (������� �� f) ������ ���������. ������ �� ���������� - ���� � ���, ��� ������������ ������ �� ������ ���������. */
static void 
catchangefreq(
	uint_fast32_t f,		// �������, ������� ������������� �� ������� �� CAT
	uint_fast8_t ab
	)
{
	const uint_fast8_t bi = getbankindex_ab(ab);
	//const uint_fast8_t b = getfreqband(f);	/* ���������� �� �������, � ����� ��������� ��������� */

	gfreqs [bi] = f;
#if WITHONLYBANDS
	const vindex_t hb = getfreqband(gfreqs [bi]);
	tune_bottom_active [bi] = get_band_bottom(hb);
	tune_top_active [bi] = get_band_top(hb);
#endif
	//gpamps [bi] = loadvfy8up(RMT_PAMP_BASE(b), 0, PAMPMODE_COUNT - 1, DEFPREAMPSTATE);	/* ����������� ������� ��������� ������������� */
	//gatts [bi] = loadvfy8up(RMT_ATT_BASE(b), 0, ATTMODE_COUNT - 1, 0);	/* ����������� ������� ��������� ����������� */
	//gantennas [bi] = loadvfy8up(RMT_ANTENNA_BASE(b), 0, ANTMODE_COUNT - 1, 0);	/* ����������� ��� ���������� ������� */
}

static void catchangesplit(
	uint_fast8_t enable,
	int_fast16_t delta
	)
{
	gsplitmode = enable ? VFOMODES_VFOSPLIT : VFOMODES_VFOINIT;
}

#endif /* WITHCAT */

/* ������� ������ �� */

#if 1
/* ����� ����� ��������� � ����������� �� lsbflag */
#define UPPERTOSIGN(lsbflag, value) (((lsbflag) == 0) ? (value) : - (value))
#define UPPERTOSIGN16(lsbflag, value) (((lsbflag) == 0) ? (value) : - (value))
#else
static int_fast32_t UPPERTOSIGN(
	uint_fast8_t lsbflag,
	int_fast32_t v
	)
{
	return lsbflag ? - v : v;
}
static int_fast16_t UPPERTOSIGN16(
	uint_fast8_t lsbflag,
	int_fast16_t v
	)
{
	return lsbflag ? - v : v;
}
#endif
/* 
 * �������� ����� ���� - ��� CW ������� (� �������� ����).
 * ��� ��������� ������������ ������� ������� ��� ������� cwpitch, ��� SSB - ��� ������� �������
 */
static int_least16_t 
//NOINLINEAT
gettone_bymode(
	uint_fast8_t mode		/* ��� ������ ������ */
	)
{
	if (mode != MODE_CW)
		return 0;
	return gcwpitch10 * CWPITCHSCALE;
}


/* 
 * �������� ����� ���� - ��� CW ������� (� �������� ����).
 * ��� ��������� ������������ ������� ������� ��� ������� cwpitch, ��� SSB - ��� ������� �������
 */
static int_least16_t 
//NOINLINEAT
gettone_bysubmode(
	uint_fast8_t submode,		/* ��� ������ ������ */
	uint_fast8_t forcelsb		/* ����� ����� ������ smart, ��������� ���� �������� */
	)
{
	const int_least16_t t = gettone_bymode(submodes [submode].mode);
	return UPPERTOSIGN16(getsubmodelsb(submode, forcelsb), t);
}

static uint_fast8_t getforcelsb(uint_fast32_t freq)
{
	return freq < BANDFUSBFREQ;
}

/* ����� ��������� ������ ������ ������ �������� �������� ����.
 * � ������ ������������� ��������������
 * ������� ������ (gfreq) ��� ���������� �������� ���� ������.
 */
static void
gsubmodechange(
	uint_fast8_t newsubmode, 
	uint_fast8_t bi				/* bank index */
	)
{
	uint_fast32_t freq = gfreqs [bi];
	const uint_fast8_t forcelsb = getforcelsb(freq);

	/* ��������� ������� ��������� ��� ���������� ����������� ���� ��� �������� �� ����� ������ */
	const int_fast16_t delta = stayfreq ? 0 : (gettone_bysubmode(newsubmode, forcelsb) - gettone_bysubmode(gsubmode, forcelsb));

	/* ��������� ��������� ������� ��-�� ��������� ������.
		�������� ��������� ������� �����, ��� ���� ���������� ������� "����������" ��
		������������� ��������� - ��� ����� �����, ��� ����� �� ��������� � ������������
		����������� �� ��������� (������� ����) ������ �� �����.
		*/

	if (delta < 0)
	{
		/* �������������� ��������� "����" */
		//const uint_fast32_t lowfreq = bandsmap [b].bottom;
		freq = prevfreq(freq, freq - (- delta), 10UL, TUNE_BOTTOM);
		gfreqs [bi] = freq;

		{
			const vindex_t v = getvfoindex(bi);
			savebandstate(v, bi); // �������� ��� ��������� ��������� (����� �������) � ������� ������ ��������� */
			savebandfreq(v, bi);	/* ���������� ������� � ������� VFO */
		}
	}
	else if (delta > 0)
	{
		/* �������������� ��������� "�����" */
		//const uint_fast32_t topfreq =  bandsmap [b].top;
		//freq = nextfreq(freq, freq + delta10 * 10UL, 10UL, topfreq);
		freq = nextfreq(freq, freq + delta, 10UL, TUNE_TOP);
		gfreqs [bi] = freq;

		{
			const vindex_t v = getvfoindex(bi);
			savebandstate(v, bi); // �������� ��� ��������� ��������� (����� �������) � ������� ������ ��������� */
			savebandfreq(v, bi);	/* ���������� ������� � ������� VFO */
		}
	}
	else
	{
		{
			const vindex_t v = getvfoindex(bi);
			savebandstate(v, bi); // �������� ��� ��������� ��������� (����� �������) � ������� ������ ��������� */
		}
	}
}


#if defined (RTC1_TYPE)

	static uint_fast16_t grtcyear;
	static uint_fast8_t grtcmonth, grtcday;
	static uint_fast8_t grtchour, grtcminute, grtcsecounds;

	static uint_fast8_t grtcstrobe;
	static uint_fast8_t grtcstrobe_shadow;

	static void getstamprtc(void)
	{
		grtcstrobe = 0;
		grtcstrobe_shadow = 0;
		board_rtc_getdatetime(& grtcyear, & grtcmonth, & grtcday, & grtchour, & grtcminute, & grtcsecounds);
	}

	static void board_setrtcstrobe(uint_fast8_t val)
	{
		if (val != grtcstrobe_shadow && val != 0)
		{
			board_rtc_setdatetime(grtcyear, grtcmonth, grtcday, grtchour, grtcminute, 0);
		}
		grtcstrobe_shadow = val;
	}

#endif /* defined (RTC1_TYPE) */



#define BRSCALE 1200UL

/* �������� 115200 �� ��������� �� ����������� �������������� ������� ������������ �� atmega
   ��� ������� ���������� 8 ���
   */
static const FLASHMEM uint_fast8_t catbr2int [] =
{
	1200uL / BRSCALE,	// 1200
	2400uL / BRSCALE,	// 2400
	4800uL / BRSCALE,	// 4800
	9600uL / BRSCALE,	// 9600
	19200uL / BRSCALE,	// 19200
	38400uL / BRSCALE,	// 38400
	57600uL / BRSCALE,	// 57600
#if CPU_FREQ >= 10000000
	115200uL / BRSCALE,	// 115200
#endif /* CPUSTYLE_ARM */
};


static int_fast32_t
//NOINLINEAT
getsynthref(
	uint_fast8_t mode		/* ��� ��������� ������� ������ */
	)
{
#if defined(REFERENCE_FREQ)
	(void) mode;
	return refbase + refbias;
#else
	(void) mode;
	return 0;
#endif
}
// ������� ���� �������� ���������� ����������� ����������
// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
static uint_fast8_t
getsidelo0(
	uint_fast32_t freq
	)
{
#if defined (LO0_SIDE_F)
	/* ���������� ����������� ���������� �������������� � ���������������� ����� */
	/* ������������ ��� ������� �� ������� */
	return LO0_SIDE_F(freq);	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#elif defined (LO0_SIDE)
	/* ���������� ����������� ���������� �������������� � ���������������� ����� */
	return LO0_SIDE;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#else

	#error Undefined LO0_SIDE value LOCODE_XXX

#endif
}

// ������� ���� �������� ���������� ������ �����������
// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
static uint_fast8_t
getsidelo1(
	uint_fast32_t freq
	)
{
#if defined (LO1_SIDE_F)
	/* ���������� ������ ����������� �������������� � ���������������� ����� */
	/* ������������ ��� ������� �� ������� */
	return LO1_SIDE_F(freq);	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#elif defined (LO1_SIDE)
	/* ���������� ������ ����������� �������������� � ���������������� ����� */
	return LO1_SIDE;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#else

	#error Undefined LO1_SIDE value LOCODE_XXX

#endif
}

#if WITHFIXEDBFO 	/* ��������������� ������� ������ �� ���� ��������� ������� ������� ���������� */
	#if defined(LSBONLO4DEFAULT)
		static uint_fast8_t glo4lsb = LSBONLO4DEFAULT;	/* ������� LSB �� ��������� �� - �������������� ����� ����. */
	#else
		static uint_fast8_t glo4lsb = 0;	/* ������� LSB �� ��������� �� - �������������� ����� ����. */
	#endif
#endif /* WITHFIXEDBFO */

// ������� ���� �������� ���������� �������� �����������
// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
static uint_fast8_t
getsidelo4(void)
{
#if defined (LO4_SIDE)
	/* ���������� ��������� ����������� �������������� � ���������������� ����� */
	return LO4_SIDE;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#elif WITHDUALFLTR	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */

	return LOCODE_TARGETED;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

#elif WITHFIXEDBFO	/* ��������������� ������� ������ �� ���� ��������� ������� ������� ���������� */

	return glo4lsb ? LOCODE_UPPER : LOCODE_LOWER;		/* ������� LSB �� ��������� �� */

#else

	#error Undefined LO4_SIDE value LOCODE_XXX and no WITHDUALFLTR or WITHFIXEDBFO

#endif
}

/* ��������� ������� �������� ���������� */
static int_fast32_t
getlo0(
	   uint_fast32_t lo0hint
	   )
{
	return (int_fast32_t) lo0hint;
}

/* ������� �������� ������� ���������� ���������� */
static uint_fast32_t
gethintlo0(
	uint_fast32_t freq,
	uint_fast8_t lo0side	/* ��������� ��������� ��� ��������� ������� ������ */
	)
{
#if defined (XVTR1_TYPE)
	const uint_fast32_t lo0step = R820T_LOSTEP;
	// ������������ ����������� ���������� ����������
	if (lo0side == LOCODE_UPPER)		/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
		return ((freq + R820T_IFFREQ) + lo0step / 2) / lo0step * lo0step;
	else if (lo0side == LOCODE_LOWER)	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
		return ((freq - R820T_IFFREQ) + lo0step / 2) / lo0step * lo0step;
	else
		return 0;
#elif 0
	// ������������ ��������� � ������������ �����������
	return 116000000uL;
#else
	return 0;
#endif	
}


#if FQMODEL_UW3DI

static uint_fast8_t
getlo2xtal(
	uint_fast8_t mix2lsb,	/* ��������� ��������� ��� ��������� ������� ������ */
	uint_fast8_t hintlo2	/* ��� ���������� �������� �������� � ����������� �� ������� ��������� � ������ ������������� */
	)
{
	uint_fast8_t n;
	if (mix2lsb)
		n = (hintlo2 + IF2FREQTOP / HINTLO2_GRANULARITY);
	else
		n = labs(hintlo2 - IF2FREQBOTOM / HINTLO2_GRANULARITY);
	switch (n)
	{
		case 8000000L / HINTLO2_GRANULARITY:	return 0;	/* 1.8 � 14 �������� */
		case 10000000L / HINTLO2_GRANULARITY:	return 1;	/* 3.5 �������� */
		case 13500000L / HINTLO2_GRANULARITY:	return 2;	/* 7 �������� */
		case 4000000L / HINTLO2_GRANULARITY:	return 3;	/* 10.1 �������� */
		case 12000000L / HINTLO2_GRANULARITY:	return 4;	/* 18.68 �������� */
		case 15000000L / HINTLO2_GRANULARITY:	return 5;	/* 21 �������� */
		case 18500000L / HINTLO2_GRANULARITY:	return 6;	/* 24.89 �������� */
		case 22000000L / HINTLO2_GRANULARITY:	return 7;	/* 28.0 �������� */
		case 22500000L / HINTLO2_GRANULARITY:	return 8;	/* 28.5 �������� */
		case 23000000L / HINTLO2_GRANULARITY:	return 9;	/* 29.0 �������� */
		case 23500000L / HINTLO2_GRANULARITY:	return 10;	/* 29.5 �������� */
	}
	return 0;
}

#endif	/* FQMODEL_UW3DI */

/* ������� ������ ������ ��� ������� ���������� */
static uint_fast8_t
gethintlo2(
	uint_fast32_t freq
	)
{
#if FQMODEL_UW3DI
	return freq / HINTLO2_GRANULARITY;
#else
	return 0;
#endif	
}

/* �������� ������� LO2 ��� �������� ������ ������ */
static int_fast32_t 
//NOINLINEAT
getlo2(
	const filter_t * workfilter,
	uint_fast8_t mode,		/* ��� ��������� ������� ������ */
	uint_fast8_t mix2lsb,	/* ��������� ��������� ��� ��������� ������� ������ */
	uint_fast8_t tx,		/* ������� ������ � ������ �������� */
	uint_fast8_t hintlo2	/* ��� ���������� �������� �������� � ����������� �� ������� ��������� � ������ ������������� */
	)
{
#if WITHWFM
	if (mode == MODE_WFM)
		return 0;
#endif /* WITHWFM */
#if FQMODEL_10M7_500K && (LO2_SIDE == LOCODE_TARGETED)

	/* RU6BK: 1-st if=10.7 MHz, 2-nd if=500k, fixed BFO */
	const int_fast32_t if2 = 10700000L;
	const int_fast32_t if3 = 500000L;
	if (mix2lsb)
		return if2 + if3;
	else
		return if2 - if3;
	
#elif FQMODEL_UW3DI
	// LO2 ����������� ������� ������� ��� PLL
	// �������� ������� ��� �������
	if (mix2lsb)
		return (hintlo2 + IF2FREQTOP / HINTLO2_GRANULARITY) * HINTLO2_GRANULARITY;
	else
		return labs(hintlo2 - IF2FREQBOTOM / HINTLO2_GRANULARITY) * HINTLO2_GRANULARITY;

#elif /*defined (PLL2_TYPE) && */ (LO2_SIDE != LOCODE_INVALID)
	// LO2 ����������� PLL � ������������ �� ������ ������ � �����/��������
	return (int_fast32_t) (((uint_least64_t) getsynthref(mode) * * getplo2n(workfilter, tx) / * getplo2r(workfilter, tx)) >> LO2_POWER2);

#elif (LO2_SIDE != LOCODE_INVALID)
	// LO2 ����������� PLL ��� ����� ������ � �������� ����������
	(void) tx;
	return (int_fast32_t) (((uint_least64_t) getsynthref(mode) * LO2_PLL_N / LO2_PLL_R) >> LO2_POWER2);

#else
	// LO2 �����������
	(void) mode;
	(void) tx;
	return 0;

#endif
}

/* �������� LO3 ��� �������� ������ ������ */
static int_fast32_t 
//NOINLINEAT
getlo3(
	uint_fast8_t mode,		/* ��� ��������� ������� ������ */
	uint_fast8_t mix3lsb,		/* ��������� ��������� ��� ��������� ������� ������ */
	uint_fast8_t tx		/* ������� ������ � ������ �������� */
	)
{	
#if (LO3_SIDE != LOCODE_INVALID)
  #if LO3_FREQADJ	/* ���������� ������� ���������� ����� ����. */
	return (lo3base + lo3offset) >> LO3_POWER2;
  #else
	return (int_fast32_t) ((uint_least64_t) getsynthref(mode) * LO3_PLL_N / LO3_PLL_R >> LO3_POWER2);
  #endif

#else
	/* ��������� #2A ������ ��� � ������ */
	(void) mode;
	(void) mix3lsb;
	return 0;
#endif
}

/* ������������� ������������ ���������� ��� ���������� ������. */
static void
//NOINLINEAT
update_lo0(
	uint_fast32_t lo0hint,		/* ��� ������� ��������� (�� ������ ��������� � ��������) */
	uint_fast8_t lo0side
	)
{
	const uint_fast8_t enable = lo0side != LOCODE_INVALID;
	board_set_xvrtr(enable);
	synth_lo0_setfreq(getlo0(lo0hint), enable);
	board_set_swaprts(lo0side == LOCODE_LOWER);	/* ��� �������������� �� ���� ���������� ���������� �������� ������� */
}

/* ������������� ������������ ���������� ��� ���������� ������. */
static void
//NOINLINEAT
update_lo2(
	uint_fast8_t pathi,		// ����� ������ - 0/1: main/sub
	const filter_t * workfilter,
	uint_fast8_t mode,		/* ��� ��������� ������� ������ */
	uint_fast8_t lsb,		/* ��������� ��������� ��� ��������� ������� ������ */
	int_fast32_t f,			/* �������, ������� ����� �������� �� ������ DDS */
	uint_fast8_t od,		/* �������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	uint_fast8_t tx,		/* ������� ������ � ������ �������� */
	uint_fast8_t hint		/* ��� ���������� �������� �������� � ����������� �� ������� ��������� � ������ ������������� */
	)
{
#if FQMODEL_UW3DI
	board_setlo2xtal(getlo2xtal(lsb, hint));	// ��������� ������ ������
#elif defined (PLL2_TYPE) && (LO2_SIDE != LOCODE_INVALID)
	(void) hint;
	prog_pll2_r(getplo2r(workfilter, tx));		/* ���������������� PLL2 � ������ ����������� ������� ������� ���������� */
	prog_pll2_n(getplo2n(workfilter, tx));		/* ���������������� PLL2 � ������ ����������� ������� ������� ���������� */
#elif LO2_DDSGENERATED
	(void) mode;
	(void) hint;
	(void) lsb;
	(void) tx;
	synth_lo2_setfreq(pathi, f, od);		// for IGOR or EW2DZ or FQMODEL_10M7_500K
#else
	/* ��������� #2 ������ ��� � ������ */
	(void) mode;
	(void) hint;
	(void) lsb;
	(void) f;
	(void) od;
	(void) tx;
#endif
}


/* ������������� ������������ ���������� ��� ���������� ������. */
static void
//NOINLINEAT
update_lo3(
	uint_fast8_t pathi,		// ����� ������ - 0/1: main/sub
	uint_fast8_t mode,		/* ��� ��������� ������� ������ */
	uint_fast8_t lsb,		/* ��������� ��������� ��� ��������� ������� ������ */
	int_fast32_t f,			/* �������, ������� ����� �������� �� ������ DDS */
	uint_fast8_t od,		/* �������� ����� ������� �� ��������� (1, 2, 4, 8...) */
	uint_fast8_t tx			/* ������� ������ � ������ �������� */
	)
{
	(void) mode;
	(void) lsb;
	(void) tx;
	synth_lo3_setfreq(pathi, f, od);
}

// return value: LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
static uint_fast8_t
getsidelo6(
	uint_fast8_t mode,		/* ��� ��������� ������� ������ */
	uint_fast8_t tx
	)
{
#if defined (LO6_SIDE)
	/* ���������� ���� ����������� �������������� � ���������������� ����� */
	return LO6_SIDE;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
#elif WITHIF4DSP
	return LOCODE_TARGETED;	/* ���� ��������� ����������� ��� ��������� ��������� ������� */
#else /* WITHIF4DSP */
	return LOCODE_LOWER;	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
#endif /* WITHIF4DSP */
}

#if WITHIF4DSP

// �������� ������� ����� ��� ������� ��
// ��� ���������� ��������� ������.
// ���������� ������ ������ ����������� ������.
// INT16_MAX - ��� ������ �������
static  int_fast16_t
getif6bw(
	uint_fast8_t mode,		/* ��� ��������� ������� ������ */
	uint_fast8_t tx,
	uint_fast8_t wide		/* ������ ������� ������ ����������� */
	)
{
	const uint_fast8_t bwseti = mdt [mode].bwseti;	// ������ ����� ����� ����������� ��� ������� ������
	const int_fast16_t bw6 = mdt [mode].bw6s [tx];
	if (bw6 != 0)
		return bw6;

	switch (mode)
	{
#if WITHMODEM
	case MODE_MODEM:
		// ��� �������� ������� �� �������
		return (uint_fast64_t) modembr2int100 [gmodemspeed] * 28 / 100 / 10;	// bw=symbol rate * 1.4

#endif /* WITHMODEM */

#if WITHFREEDV
	case MODE_FREEDV:
#endif /* WITHFREEDV */
	case MODE_CW:
	case MODE_SSB:
	case MODE_DIGI:
	case MODE_RTTY:
		if (tx)
			return (gssbtxhighcut100 * BWGRANHIGH - gssbtxlowcut10 * BWGRANLOW);	// ������ ������� SSB �� ��������
		if (! wide)
		{
			return bwseti_getwidth(bwseti);
		}
		else
		{
			const int_fast16_t lowcut = bwseti_getlow(bwseti);
			const int_fast16_t highcut = bwseti_gethigh(bwseti);
			return (highcut - lowcut); 
		}

	case MODE_ISB:
		{
			const int_fast16_t highcut = bwseti_gethigh(bwseti);
			return 2 * highcut; 
		}

	// ����������� ������� �� ��������� �� ������� mdt [mode].
	default:
		if (tx)
			return INT16_MAX;	/* PASSTROUGH */
		return bwseti_gethigh(bwseti) * 2;
	}
	return 0;
}
#endif /* WITHIF4DSP */

// �������� ������� ��� �������������� �������
static  int_fast32_t
getlo6(
	uint_fast8_t mode,		/* ��� ��������� ������� ������ */
	uint_fast8_t tx,
	uint_fast8_t wide,		/* ������ ������ ������ ����������� */
	int_fast16_t ifshift
	)
{
#if WITHIF4DSP
	const uint_fast8_t bwseti = mdt [mode].bwseti;	// ������ ����� ����� ����������� ��� ������� ������
	switch (mode)
	{
	case MODE_ISB:
		return 0;

	case MODE_DIGI:
	case MODE_SSB:
		if (tx)
		{
			const int_fast16_t lowcut = gssbtxlowcut10 * BWGRANLOW; //bwseti_getlow(bwseti);
			const int_fast16_t highcut = gssbtxhighcut100 * BWGRANHIGH; //bwseti_gethigh(bwseti);
			return (highcut - lowcut) / 2 + lowcut; 	// ������� ������ ������ �����������
		}
	case MODE_CW:
		if (wide)
		{
			const int_fast16_t lowcut = bwseti_getlow(bwseti);
			const int_fast16_t highcut = bwseti_gethigh(bwseti);
			return (highcut - lowcut) / 2 + lowcut + ifshift; 	// ������� ������ ������ �����������
		}
		else
		{
			return tx != 0 ? 0 : gcwpitch10 * CWPITCHSCALE;
		}

	case MODE_DRM:
		return DEFAULT_DRM_PITCH;	/* ��� DRM - 12 ���*/

	case MODE_RTTY:
		return DEFAULT_RTTY_PITCH;	/* ��� DIGI modes - 2.125 ��� */

#if WITHMODEM
	case MODE_MODEM:
		return tx ? 0 : 0;		// ����� ��������� ����������
#endif /* WITHMODEM */

#if WITHFREEDV
	case MODE_FREEDV:
		return tx ? 0 : 0;		// ����� ��������� ����������
#endif /* WITHFREEDV */

	default:
		// AM. NFM � ��������� - ����������� �� baseband
		return 0;
	}
#else /* WITHIF4DSP */
	return 0; //- ifshift;	// � ������������ ������� �������� 0 ��� ����������.
#endif /* WITHIF4DSP */
}

// �������� ������� ���������� (��� SSB - ������� ����������� �������).
// ��� ������������ ��������� �����-������������ �������� ��������� �������� �����������
// ������������� SSB ��� ���� �������� �� ������� ���������� - ��� �����������
// ��������� "0" � ������ CW �� ��������.
// ��� IF DSP � DDC/DUC �������� ������� ������� ��� �������� �������� ������ �� DUC ���������� ������������ � baseband.

static  int_fast32_t
getif6(
	uint_fast8_t mode,		/* ��� ��������� ������� ������ */
	uint_fast8_t tx,
	uint_fast8_t wide		/* ������ ������ ������ ����������� */
	)
{
	switch (mode)
	{
	case MODE_CW:
		return tx != 0 ? 0 : gcwpitch10 * CWPITCHSCALE;

	case MODE_DRM:
		return DEFAULT_DRM_PITCH;	/* ��� DRM - 12 ���*/

	case MODE_RTTY:
		return DEFAULT_RTTY_PITCH;	/* ��� RTTY modes - 2.125 ��� */

	default:
		return 0;
	}
}

/*
 * ������� �������� �� ��� �������� ���������
 * 1/4 FS (12 kHz) ��� 0
*/
static  int_fast32_t
getlo5(
	uint_fast8_t mode,		/* ��� ��������� ������� ������ */
	uint_fast8_t tx
	)
{
#if WITHIF4DSP
	return dsp_get_ifreq();	/* 1/4 FS (12 kHz) ��� 0 ��� DSP */
#else /* WITHIF4DSP */
	return 0;
#endif /* WITHIF4DSP */
}

// ������� ����������, ������������ ������ �� � 12 ��� �� baseband ��� �������� ���������
// ���, � ������������ ����������, ������� � ��������� �� �� ����.
// return value: LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED

static uint_fast8_t
getsidelo5(
	uint_fast8_t mode,		/* ��� ��������� ������� ������ */
	uint_fast8_t tx
	)
{
#if defined (LO5_SIDE)
	/* ���������� ���� ����������� �������������� � ���������������� ����� */
	return LO5_SIDE;	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
#elif WITHFIXEDBFO || WITHDUALBFO	/* ��������������� ������� ������ �� ���� ��������� ������� ������� ���������� */
	return LOCODE_LOWER;	/* ��� �������������� �� ���� ���������� ��� �������� ������� */
#else
	return mdt [mode].lo5side [tx];
#endif /* WITHFIXEDBFO || WITHDUALBFO */
}

static uint_fast8_t
getlo4enable(
	uint_fast8_t mode,
	uint_fast8_t tx
	)
{
#if defined (LO5_SIDE)
	return 1;
#else
	return mdt [mode].lo5side [tx] != LOCODE_INVALID;
#endif
}


static int_fast32_t
getlo4ref(
	const filter_t * workfilter,
	uint_fast8_t mode,			/* ��� ��������� ������ ������ */
	uint_fast8_t mix4lsb,		/* ��������� ��������� ��� ��������� ������� ������ */
	uint_fast8_t tx				/* ��� ������ �������� - ������� CW - �������� ������� �� ���������. */
	)
{
#if WITHFIXEDBFO || WITHDUALFLTR
	// ������������ IF � ��������� � ����� ������� �������� ��������
	return getlo4baseflt(workfilter) + lo4offset;
#elif WITHDUALBFO
	// ������������ IF � ��������� � ����� ������� �������� ��������
	return getlo4baseflt(workfilter) + lo4offsets [mix4lsb];
#elif WITHIF4DSP
	// ������������ IF � ��������� � ������ ������� �������� ��������
	return getif3filtercenter(workfilter);
#else
	return getif3byedge(workfilter, mode, mix4lsb, tx, gcwpitch10);
#endif
}

// ���������� �� user mode
static uint_fast8_t
getactualtune(void)
{
	return tunemode || (catenable && cattunemode) || reqautotune || hardware_get_tune();
}

// ���������� �� user mode - ������� �������� � ������ ������
static uint_fast8_t
getcattxdata(void)
{
	return catenable && catstatetx && catstatetxdata;
}

// �������� submode ��� ������, �������� ���������� �� CWZ � ������ ������������ ������� ���������.
static uint_fast8_t
getasubmode(uint_fast8_t pathi)
{
	const uint_fast8_t bi = getbankindex_pathi(pathi);	/* vfo bank index */
	/* ������������ ���� ���������� ��������� ��� ����������� ��� ����� ����� */
	const uint_fast8_t pathsubmode = getsubmode(bi);

#if WITHMODESETSMART
	const uint_fast8_t submode = (getactualtune() == 0) ? pathsubmode : SUBMODE_CWZSMART;	// mode_cwz ������������ ������������ ������� ������������ ��� ��������.
#else /* WITHMODESETSMART */
	const uint_fast8_t submode = (getactualtune() == 0) ? pathsubmode : SUBMODE_CWZ;	// mode_cwz ������������ ������������ ������� ������������ ��� ��������.
#endif /* WITHMODESETSMART */
	return submode;
}
/*
 * ��������� ����������, �������� �� ������ ���������, �������� ������ �������.
 */
static void
//NOINLINEAT
updateboard2(void)
{
#if WITHENCODER
	encoder_set_resolution(encresols [ghiresres], ghiresdyn);
#endif
	display_setbgcolor(gbluebgnd ? COLOR_BLUE : COLOR_BLACK);
}


static uint_fast8_t
//NOINLINEAT
getlsbfull(
	uint_fast8_t lsb,
	const uint_fast8_t * sides,
	uint_fast8_t size
	)
{
	while (size --)
		lsb ^= * sides ++ == LOCODE_UPPER;
	return lsb; 
}

/* ��������� �������� LSB ��� LO0..LO6 */
static uint_fast8_t
//NOINLINEAT
getlsbloX(
	uint_fast8_t lsb,
	uint_fast8_t keyindex,	// 0..6 - ����� LOx
	const uint_fast8_t * sides,
	uint_fast8_t size
	)
{
	const uint_fast8_t keyside = sides [keyindex];
	if (keyside == LOCODE_TARGETED)
		return getlsbfull(lsb, sides, size);
	return keyside == LOCODE_UPPER;
}

/* ��������� �������� �������� � ������ lo1 ����� ������� �� ��������� */
static uint_fast8_t getlo1div(
	uint_fast8_t tx
	)
{
#if LO1FDIV_ADJ
	return 1U << lo1powmap [tx];
#elif defined (LO1_POWER2)
	return 1U << LO1_POWER2;
#else
	return 1U;
#endif
}

/* ��������� �������� �������� � ������ lo2 ����� ������� �� ��������� */
static uint_fast8_t getlo2div(
	uint_fast8_t tx
	)
{
#if LO2FDIV_ADJ
	return 1U << lo2powmap [tx];
#elif defined (LO2_POWER2)
	return 1U << LO2_POWER2;
#else
	return 1U;
#endif
}

/* ��������� �������� �������� � ������ lo3 ����� ������� �� ��������� */
static uint_fast8_t getlo3div(
	uint_fast8_t tx
	)
{
#if LO3FDIV_ADJ
	return 1U << lo3powmap [tx];
#elif defined (LO3_POWER2)
	return 1U << LO3_POWER2;
#else
	return 1U;
#endif
}

/* ��������� �������� �������� � ������ lo4 ����� ������� �� ��������� */
static uint_fast8_t getlo4div(
	uint_fast8_t tx
	)
{
#if LO4FDIV_ADJ
	return 1U << lo4powmap [tx];
#elif defined (LO4_POWER2)
	return 1U << LO4_POWER2;
#else
	return 1U;
#endif
}


// ������ ������� � ������� dddddd.ddd
static void printfreq(int_fast32_t freq)
{
	const ldiv_t v = ldiv(freq, 1000);
	debug_printf_P(PSTR("%s%ld.%03ld"), (v.quot >= 0 && freq < 0) ? "-" : "", v.quot, freq < 0 ? - v.rem : v.rem);
}

/* �������� ������� lo1 �� ������� ��������� */
static int_fast32_t synth_if1 [2];

uint_fast32_t synth_freq2lo1(
	uint_fast32_t freq,	/* ������� �� ���������� ���������� */
	uint_fast8_t pathi		/* ����� ������ (main/sub) */
	)
{
	return (int_fast32_t) freq - synth_if1 [pathi];
}

/* ���� ���������� �������� ���������� �� ������� �������� - ������� 1 */
static uint_fast8_t
flagne_u8(uint_fast8_t * oldval, uint_fast8_t v)
{
	if (* oldval != v)
	{
		* oldval = v;
		return 1;
	}
	return 0;
}

/* ���� ���������� �������� ���������� �� ������� �������� - ������� 1 */
static uint_fast8_t
flagne_u16(uint_fast16_t * oldval, uint_fast16_t v)
{
	if (* oldval != v)
	{
		* oldval = v;
		return 1;
	}
	return 0;
}

/* ���� ���������� �������� ���������� �� ������� �������� - ������� 1 */
static uint_fast32_t
flagne_u32(uint_fast32_t * oldval, uint_fast32_t v)
{
	if (* oldval != v)
	{
		* oldval = v;
		return 1;
	}
	return 0;
}

/*
 ���������:
 tx - ��-0: ������������ ���������� � ����� ��������
 ����������� ��������� tunemode - ����� ��������� �����������, ��� ���� �������� tx ��-����.
 */
static void
updateboard(
	uint_fast8_t full, 
	uint_fast8_t mute 
	)
{
	/* ���������, ����������� �� updateboard(full=1) */
	static const FLASHMEM struct modetempl * pamodetempl;	/* �����, ������������ ��� �������� */

	/* ���������, ������� ����� ���������� ��� ����������� ������� � ������� ������������� full=1 */
	static uint_fast8_t lo0side = LOCODE_INVALID;
	static uint_fast32_t lo0hint = UINT32_MAX;
	static uint_fast8_t lo1side [2] = { LOCODE_INVALID, LOCODE_INVALID };
	const uint_fast8_t lo2side = LO2_SIDE;
	static uint_fast8_t lo2hint [2];
	const uint_fast8_t lo3side = LO3_SIDE;
	static uint_fast8_t forcelsb [2];
#if WITHDCDCFREQCTL
	static uint_fast16_t bldividerout = UINT16_MAX;
#endif /* WITHDCDCFREQCTL */
#if CTLSTYLE_IGOR
	static uint_fast16_t bandf100khint = UINT16_MAX;
#else /* CTLSTYLE_IGOR */
	static uint_fast8_t bandfhint = UINT8_MAX;
	static uint_fast8_t bandf2hint = UINT8_MAX;
	static uint_fast8_t bandf3hint = UINT8_MAX;	// ���������� ����� ������ ACC
#endif /* CTLSTYLE_IGOR */

	uint_fast8_t full2 = full; 

	uint_fast8_t pathi;
	ASSERT(gtx < 2);
	const uint_fast8_t pathn = gtx ? 1 : NTRX;
	/* +++ �������� ������������� ������ ������������� ��-�� ����������� ������� ������ ������. */
	for (pathi = 0; pathi < pathn; ++ pathi)
	{
		const uint_fast8_t bi = getbankindex_pathi(pathi);
		const int_fast32_t freq = gfreqs [bi];
		/* "��� ��������� ��� ������� ������ �������� �����, ��� ��� ��� (�����) �������� �� ������� */
		full2 |= flagne_u8(& lo1side [pathi], getsidelo1(freq));	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
		full2 |= flagne_u8(& lo2hint [pathi], gethintlo2(freq));
		full2 |= flagne_u8(& forcelsb [pathi], getforcelsb(freq));
#if WITHDCDCFREQCTL
		full2 |= flagne_u16(& bldividerout, getbldivider(freq));
#endif /* WITHDCDCFREQCTL */
	}
	// ���������, �� ������� ��������� ��� ������ ����������
	{
		const uint_fast8_t bi = getbankindex_tx(gtx);
		const int_fast32_t freq = gfreqs [bi];
	#if CTLSTYLE_IGOR
		full2 |= flagne_u16(& bandf100khint, freq / 100000uL);
	#endif /* CTLSTYLE_IGOR */
		full2 |= flagne_u8(& bandfhint, bandf_calc(freq));
		full2 |= flagne_u8(& bandf2hint, bandf2_calc(freq));
		full2 |= flagne_u8(& bandf3hint, bandf3_calc(freq));
		full2 |= flagne_u8(& lo0side, getsidelo0(freq));	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
		full2 |= flagne_u32(& lo0hint, gethintlo0(freq, lo0side));
	}
	/* --- �������� ������������� ������ ������������� ��-�� ����������� ������� ������ ������. */


#if WITHCAT
	if (aistate != 0)
	{
		if (full)
		{
			cat_answer_request(CAT_IF_INDEX);
			cat_answer_request(CAT_MD_INDEX);
			cat_answer_request(CAT_FA_INDEX);	// ��������� ��� ���������� ���������� ������� � ACRP-590 ��� �������� �� ���������� ��������� �� ����������. � ������� ��� �������� �� ����� �������.
			cat_answer_request(gtx ? CAT_TX_INDEX : CAT_RX_INDEX);	// ignore main/sub rx selection (0 - main. 1 - sub);
		}
		else
		{
			cat_answer_request(CAT_FA_INDEX);
		}
	}
#endif /* WITHCAT */
	
	if (full)
	{
		const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
		/* ������������ ���� ���������� ��������� ��� ����������� ��� ����� ����� */
		setgsubmode(getsubmode(bi));		/* ��������� gsubmode, gagc, gfi � ��� ����� */
	}

	if (full2)
	{

		/* ������ �������������. ��������� ����� (��� ���� �� �������� hint). */
		if (gtx == 0)
		{
			board_set_tx(0);	/* ����� ����� ��������������� ��������� ���������� */
			board_update();		/* ������� ���������������� ��������� � �������� */
			// todo: ������ ����� ������� ������� � ��������� ����� ������ � ��������� � FPGA
		}

		const uint_fast8_t rxbi = getbankindex_tx(0);
		const uint_fast8_t txbi = getbankindex_tx(1);
		const uint_fast8_t txsubmode = getsubmode(txbi);		/* ��� ������, ������� ����� ��� �������� */
		const uint_fast8_t txmode = submodes [txsubmode].mode;

		for (pathi = 0; pathi < pathn; ++ pathi)
		{
			const uint_fast8_t asubmode = getasubmode(pathi);	// SUBMODE_CWZ/SUBMODE_CWZSMART for tune
			pamodetempl = getmodetempl(asubmode);
			const uint_fast8_t amode = submodes [asubmode].mode;
			const uint_fast8_t alsbmode = getsubmodelsb(asubmode, forcelsb [pathi]);	// ����������� ��������� �� ������ �������
			//
			// lo2side - ������������ �������������
			// lo3side - ������������ �������������
			//const static uint_fast8_t lo2side = LO2_SIDE;
			//const static uint_fast8_t lo3side = LO3_SIDE;
			//const uint_fast8_t lo4side = getsidelo4();	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
			//const uint_fast8_t lo5side = getsidelo5(amode, gtx);	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
			//const uint_fast8_t lo6side = getsidelo6(amode, gtx);	// LOCODE_UPPER, LOCODE_LOWER or LOCODE_TARGETED
			//
			const uint_fast8_t sides [7] = { lo0side, lo1side [pathi], lo2side, lo3side, getsidelo4(), getsidelo5(amode, gtx), getsidelo6(amode, gtx) };
			uint_fast8_t mixXlsbs [sizeof sides / sizeof sides [0]];
			uint_fast8_t i;
			for (i = 0; i < sizeof sides / sizeof sides [0]; ++ i)
			{
				mixXlsbs [i] = getlsbloX(alsbmode, i, sides, sizeof sides / sizeof sides [0]);
			}
			// ������ ������ � ������
			// 
		#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX)
			enum { dc = 1 };	/* ������ � ������ ������� �������������� �� ������ ������� �������� LO4 */
		#else
			const uint_fast8_t dc = dctxmodecw && gtx && pamodetempl->txcw;	// �� �������� ��������� ������ �����������
		#endif

			const filter_t * workfilter;
			if (gtx != 0)
			{
				workfilter = gettxfilter(asubmode, getsuitabletx(amode, 0));	/* �������� �� gfi/gfitx, �������� ������, ���������� ��� ������� ������ */
			}
			else
			{
				workfilter = getrxfilter(asubmode, gfi);	/* �������� �� gfi/gfitx, �������� ������, ���������� ��� ������� ������ */
			}
			ASSERT(workfilter != NULL);
	#if WITHIF4DSP
			// ��� ��, ����� ����� ������, ����� ������ �� ��������� ��� ������� ������� dsp.
			// � ������ DUC/DDC, ������������ "��������" - IF3_TYPE_BYPASS.
			const uint_fast8_t bwseti = mdt [amode].bwseti;
			const uint_fast8_t wide = bwseti_getwide(bwseti);
	#else /* WITHIF4DSP */
			const uint_fast8_t wide = workfilter->widefilter;
	#endif /* WITHIF4DSP */
			//
#if WITHDUMBIF
			{
				const int_fast32_t freqif1 = WITHDUMBIF;
				synth_if1 [pathi] = UPPERTOSIGN(mixXlsbs [1], freqif1);	// ������������ ��� ������������ �������������� ������� ������� � ������� ����������.
			}
#else /* WITHDUMBIF */
			const int_fast16_t pbt = getpbt(workfilter, amode, gtx);
			const int_fast16_t ifshift = getifshift(workfilter, amode, gtx);	/* ������������� �������� - ��������� ������ (������ ���������� "����"). */

			const int_fast32_t freqif6 = getif6(amode, gtx, wide); // Positive number: ssb:0, cw=700, drm=12k
			const int_fast32_t freqlo6 = UPPERTOSIGN(! mixXlsbs [6], getlo6(amode, gtx, wide, ifshift));
			const int_fast32_t freqif5 = freqlo6 + UPPERTOSIGN(mixXlsbs [6], freqif6); 	// � ������ CW ������ ���� 0 ��� DDC/DUC
			const int_fast32_t freqlo5 = UPPERTOSIGN(mixXlsbs [5], getlo5(amode, gtx));	// 0 or DSP IF freq (12 kHz)
			const int_fast32_t freqif4 = freqlo5 + UPPERTOSIGN(mixXlsbs [5], freqif5);
			const int_fast32_t freqlo4ref = getlo4ref(workfilter, amode, mixXlsbs [4], gtx);

			// �������� ��� ��������� �������� ������� - � ������������� ������ �����
			// � � ������������� ����������� ������ ����� �����������.
			// ������� � ������������� ����������� ������ - ��� ������ ����������� ������ ������� �� � ������������ ������.
			// ���, ������ WITHIF4DSP.
	#if WITHFIXEDBFO || WITHDUALFLTR
			// ������������ IF � ��������� � ����� ������� �������� ��������
			// ������ ���������� freqif4, freqlo5 == 0
			const int_fast32_t freqlo4 = freqlo4ref - freqlo5;	// ���� ��������� ����
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4 + UPPERTOSIGN(mixXlsbs [4], freqif4));
	#elif WITHDUALBFO
			// ������������ IF � ��������� � ����� ������� �������� ��������
			// ������ ���������� freqif4, freqlo5 == 0
			const int_fast32_t freqlo4 = freqlo4ref - freqlo5;	// ����������� ��� - ��� ������ ������ �������
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4 + UPPERTOSIGN(mixXlsbs [4], freqif4));
	#elif WITHIF4DSP
			// ������������ IF � ��������� � ������ ������� �������� ��������
			// ������ ���������� freqif4, freqlo5
			const int_fast32_t freqlo4 = freqlo4ref - freqlo5;	// ���� ����������� ����� ������� � �������� �������� �� baseband
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4 + UPPERTOSIGN(mixXlsbs [4], freqif4));
	#elif 0
			// todo: �� �������� ���������� ������� ����� ��� ������������ SSB->CW, ���� �������� IF SHIFT
			// ������������ IF � ��������� � ����� ������� �������� ��������
			// ������ ���������� freqif4, freqif5 = - ifshift
			const int_fast32_t freqlo4 = dc ? 0 : (freqlo4ref - UPPERTOSIGN(mixXlsbs [4], freqif4));	// used for programming BFO
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4ref - UPPERTOSIGN(mixXlsbs [4], freqif5));

	#elif 0
			// todo: �������� ���������� ������� ����� ��� ������������ SSB->CW, �� IF SHIFT ������ ������� �����
			// ������������ IF � ��������� � ����� ������� �������� ��������
			// ������ ���������� freqif4, freqif5 = - ifshift
			const int_fast32_t freqlo4 = dc ? 0 : (freqlo4ref - UPPERTOSIGN(mixXlsbs [4], freqif4));	// used for programming BFO
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4ref + UPPERTOSIGN(mixXlsbs [4], freqif4) - UPPERTOSIGN(mixXlsbs [4], freqif5));
	#else
			// ����� ������� ifshif � getlo6
			const int_fast32_t freqif3 = dc ? freqif4 : (freqlo4ref - UPPERTOSIGN(mixXlsbs [4], ifshift));
			const int_fast32_t freqlo4 = dc ? 0 : (freqif3 - UPPERTOSIGN(mixXlsbs [4], freqif4));	// used for programming BFO
	#endif

			// �������� ��������� LO1 � LO2 ������������ "����������" ������ ����������� - PBTs
			const int_fast32_t freqlo3 = getlo3(amode, mixXlsbs [3], gtx) + UPPERTOSIGN16(mixXlsbs [3], pbt);	/* ������� ��� ����������, ��������������� passband tuning = PBT */
			const int_fast32_t freqif2 = freqlo3 + UPPERTOSIGN(mixXlsbs [3], freqif3);
			const int_fast32_t freqlo2 = getlo2(workfilter, amode, mixXlsbs [2], gtx, lo2hint [pathi]);
			const int_fast32_t freqif1 = freqlo2 + UPPERTOSIGN(mixXlsbs [2], freqif2);
			const int_fast32_t freqlo0 = getlo0(lo0hint);
			synth_if1 [pathi] = UPPERTOSIGN(mixXlsbs [1], freqif1) - UPPERTOSIGN(mixXlsbs [0], freqlo0);	// ������������ ��� ������������ �������������� ������� ������� � ������� ����������.
#endif /* WITHDUMBIF */

	#if 0 && WITHDEBUG
			{
				const uint_fast8_t bi = getbankindex_pathi(pathi);
				const int_fast32_t freq = gfreqs [bi];
				debug_printf_P(submodes [asubmode].qlabel);
				debug_printf_P(PSTR(" pathi=%d"), pathi);
				debug_printf_P(PSTR(" f="));	printfreq(freq);
				debug_printf_P(PSTR(" lo0="));	printfreq(freqlo0);
				debug_printf_P(PSTR(" lo1="));	printfreq(synth_freq2lo1(freq, pathi));
				debug_printf_P(PSTR(" pbt="));	printfreq(pbt);
				debug_printf_P(PSTR(" ifshift="));	printfreq(ifshift);
				debug_printf_P(PSTR(" bw="));	debug_printf_P(workfilter->labelf3);
				debug_printf_P(PSTR(" dbw="));	debug_printf_P(hamradio_get_rxbw_value_P());
				debug_printf_P(PSTR("\n"));
				debug_printf_P(
					PSTR("mixXlsbs[0]=%d, mixXlsbs[1]=%d, mixXlsbs[2]=%d, mixXlsbs[3]=%d, mixXlsbs[4]=%d, mixXlsbs[5]=%d, mixXlsbs[6]=%d dc=%d tx=%d\n"), 
						mixXlsbs [0], mixXlsbs [1], mixXlsbs [2], mixXlsbs [3], mixXlsbs [4], mixXlsbs [5], mixXlsbs [6], dc, gtx
					);
				debug_printf_P(PSTR(" ["));	printfreq(synth_freq2lo1(freq, pathi));
				debug_printf_P(PSTR("]if1="));	printfreq(freqif1);
				debug_printf_P(PSTR(" ["));	printfreq(freqlo2);
				debug_printf_P(PSTR("]if2="));	printfreq(freqif2);
				debug_printf_P(PSTR(" ["));	printfreq(freqlo3);
				debug_printf_P(PSTR("]if3="));	printfreq(freqif3);

				//debug_printf_P(PSTR("\n"));

				debug_printf_P(PSTR(" [lo4=%d*"), getlo4enable(amode, gtx));	printfreq(freqlo4);
				debug_printf_P(PSTR("]if4="));	printfreq(freqif4);
				debug_printf_P(PSTR(" ["));	printfreq(freqlo5);
				debug_printf_P(PSTR("]if5="));	printfreq(freqif5);
				debug_printf_P(PSTR(" ["));	printfreq(freqlo6);
				debug_printf_P(PSTR("]if6="));	printfreq(freqif6);

				debug_printf_P(PSTR("\n"));
			}

	#endif /* WITHDEBUG */
		
			board_set_trxpath(pathi);	/* �����, � �������� ��������� ��� ����������� ������. ��� ���������� ������������ ������ 0 */
			if (gtx == 0)
			{
			#if WITHNOTCHONOFF
				board_set_notch(notchmodes [gnotch].code);
				board_set_notchnarrow(notchmodes [gnotch].code && pamodetempl->nar);
			#elif WITHNOTCHFREQ
				board_set_notch_on(notchmodes [gnotch].code);
				board_set_notch_width(gnotchwidth);
				board_set_notch_freq(gnotchfreq);	// TODO: ��� AUTONOTCH ������� INT16_MAX ?
			#endif /* WITHNOTCHFREQ */
			#if WITHIF4DSP
				const uint_fast8_t agcseti = pamodetempl->agcseti;
				board_set_agcrate(agcseti == AGCSETI_FLAT ? UINT8_MAX : gagc_rate [agcseti]);			/* �� n ������� ��������� �������� ������� 1 �� ���������. UINT8_MAX - "�������" ��� */
				board_set_agc_t1(gagc_t1 [agcseti]);
				board_set_agc_t2(gagc_release10 [agcseti]);		// ����� ������� ��������� ���� ���
				board_set_agc_t4(gagc_t4 [agcseti]);			// ����� ������� ������� ���� ���
				board_set_agc_thung(gagc_thung10 [agcseti]);	// hold time (hung time) in 0.1 sec
				board_set_nfm_sql_lelel(tdsp_nfm_sql_level);
				board_set_nfm_sql_off(tdsp_nfm_sql_off);
				board_set_squelch(gsquelch);
			#endif /* WITHIF4DSP */
			} /* tx == 0 */

		#if WITHIF4DSP
			#if WITHTX && WITHSUBTONES
				board_set_subtonelevel(gsbtonenable && gtx && getmodetempl(txsubmode)->subtone ? gsubtonelevel : 0);	/* ������� ������� CTCSS � ��������� - 0%..100% */
			#endif /* WITHTX && WITHSUBTONES */
			board_set_aflowcutrx(bwseti_getlow(bwseti));	/* ������ ������� ����� ������� �� �� ������ */
			board_set_afhighcutrx(bwseti_gethigh(bwseti));	/* ������� ������� ����� ������� �� �� ������ */
			board_set_afresponcerx(bwseti_getafresponce(bwseti));	/* ��������� ������ ����� � ��������� - �� Samplerate/2 ��� ���������� �� ������� �������  */

			board_set_lo6(freqlo6);	/* �����, � ������ WITHIF4DSP - ���������� ������ ������� */
			board_set_fullbw6(getif6bw(amode, gtx, wide));	/* ��������� ������� ����� �������� �� � ��������� ������ - �������� ������ ������ ����������� */
			//board_set_fltsofter(gtx ? WITHFILTSOFTMIN : bwseti_getfltsofter(bwseti));	/* ��� ���������� ������������ ������ ������� �������� �������� �� ����� */ 
			board_set_dspmode(pamodetempl->dspmode [gtx]);
			#if WITHDSPEXTDDC	/* "��������" � DSP � FPGA */
				board_set_dactest(gdactest);		/* ������ ������ ������������� � ��� ����������� ������������ ����� NCO */
				board_set_dacstraight(gdacstraight);	/* ��������� ������������ ���� ��� ��� � ������ ������������ ���� */
				board_set_tx_inh_enable(gtxinhenable);				/* ���������� ������� �� ���� tx_inh */
				board_set_tx_bpsk_enable(pamodetempl->dspmode [gtx] == DSPCTL_MODE_TX_BPSK);	/* ���������� ������� ������������ ��������� � FPGA */				/* ���������� ������� ������������ ��������� � FPGA  */
				board_set_mode_wfm(pamodetempl->dspmode [gtx] == DSPCTL_MODE_RX_WFM);	/* ���������� ������� ������������ ��������� � FPGA */				/* ���������� ������� ������������ ��������� � FPGA  */
			#endif /* WITHDSPEXTDDC */
		#else /* WITHIF4DSP */
			board_set_if4lsb(mixXlsbs [4]);	/* ��� ������� �������������� - ���������� ���������� - ��� ����� ������� LSB ��� ������������ � ������������� 3-� ����������� */
			//board_set_detector((mute && ! gtx) ? BOARD_DETECTOR_MUTE : pamodetempl->detector [gtx]);		// ����� ������ �� gsubmode
			board_set_detector(sleepflag ? BOARD_DETECTOR_MUTE : pamodetempl->detector [gtx]);
		#endif /* WITHIF4DSP */

		board_set_filter(workfilter->code [gtx]);	/* � ������ WITHDUALFLTR ������������ ������������ ����� � ����������� �� mixXlsbs [4] ���������� ��� ������ ����������� ����� � ����������. */
		board_set_nfm(amode == MODE_NFM);

		#if WITHTX
			/* ���������� ��� ��������� ��� ������� ������������� � ������ ����� �� ����, ��� ��� ���� ����� ���� ������� � ��� �������� */
			board_set_txcw(pamodetempl->txcw);	// ��� �������� ����� ����� ��� SSB ����������
			board_set_vox(gvoxenable && getmodetempl(txsubmode)->vox);	// ��������� ������� ���� VOX
			#if WITHSUBTONES
				// ��������� ����������  Continuous Tone-Coded Squelch System or CTCSS 
				board_subtone_setfreq(gsubtones [gsubtonei]);	// ������� subtone (�� ������� ����� �����).
				board_subtone_enable(gsbtonenable && gtx && getmodetempl(txsubmode)->subtone);
			#endif /* WITHSUBTONES */
			#if WITHVOX
				vox_enable(gvoxenable && getmodetempl(txsubmode)->vox, voxdelay);		/* ���������� ���������� ���������� ��������� �� �������� */
				vox_set_levels(gvoxlevel, gavoxlevel);		/* ��������� ���������� vox */
			#endif /* WITHVOX */
			board_set_mikemute(gmuteall || getactualtune() || getmodetempl(txsubmode)->mute);	/* ��������� ����������� ��������� */
			seq_set_txgate_P(pamodetempl->txgfva, pamodetempl->sdtnva);		/* ��� ������ ������������� ����� �� �������� */

			const uint_fast8_t downpower = reqautotune || hardware_get_tune();
			#if WITHPOTPOWER
				// gnormalpower ��������������� � ��������� ����������� �� ��������� �������������
				board_set_opowerlevel(downpower ? gotunerpower : gnormalpower);			/* ���������� �������� �������� ����������� WITHPOWERTRIMMIN..WITHPOWERTRIMMAX */
			#elif WITHPOWERTRIM
				board_set_opowerlevel(downpower ? gotunerpower : gnormalpower);			/* ���������� �������� �������� ����������� WITHPOWERTRIMMIN..WITHPOWERTRIMMAX */
			#elif WITHPOWERLPHP
				board_set_opowerlevel(downpower ? gotunerpower : pwrmodes [gpwr].code);
			#endif /* WITHPOWERLPHP */
		#if WITHPABIASTRIM
			board_set_pabias(gpabias);	// ���������� ���� ���������� ������� �����������
		#endif /* WITHPABIASTRIM */
			// ��������� ���������� Speech processor
			//board_speech_set_mode(speechmode);
			//board_speech_set_width(speechwidth);
			seq_set_cw_enable(getmodetempl(txsubmode)->wbkin);	/* ���������� �������� CW */
		#endif /* WITHTX */


		#if CTLREGMODE_RA4YBO

			prog_dac1_a_value(gtxpower [amode]);		// power level
			prog_dac1_b_value(gtx ? gtxcompr [amode] : 0x00);		// compression level

		#elif KEYBSTYLE_RA4YBO_AM0

			board_set_user1(guser1);
			board_set_user2(guser2);
			board_set_user3(guser3);

		#elif CTLREGMODE_RA4YBO_V1

			prog_dac1_b_value(255 - gtxpower [amode]);		// power level
			prog_dac1_a_value(gtx ? gtxcompr [amode] : 0x00);		// compression level
			board_set_user1(guser1);
			board_set_user2(guser2);
			board_set_user3(guser3);
			board_set_user4(guser4);

		#elif CTLREGMODE_RA4YBO_V2

			prog_dac1_b_value(255 - gtxpower [amode]);		// power level
			prog_dac1_a_value(gtx ? gtxcompr [amode] : 0x00);		// compression level
			board_set_user1(guser1);
			board_set_user2(guser2);
			board_set_user3(guser3);
			board_set_user4(guser4);


		#elif CTLREGMODE_RA4YBO_V3

			prog_dac1_b_value(255 - gtxpower [amode]);		// power level
			prog_dac1_a_value(gtx ? gtxcompr [amode] : 0x00);		// compression level
			board_set_user1(guser1);
			board_set_user4(guser4);
			board_set_user5(guser5);

		#elif CTLREGMODE_RA4YBO_V3A

			prog_dac1_b_value(255 - gtxpower [amode]);		// power level
			prog_dac1_a_value(gtx ? gtxcompr [amode] : 0x00);		// compression level
			board_set_user1(guser1);
			board_set_user2(guser2);
			board_set_user3(guser3);
			board_set_user4(guser4);
			board_set_user5(guser5);

		#endif /* CTLREGMODE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 */

		/* ���� ���� (��������� ������� ������� DDS) �������� �� �������� ������ */
		#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
			synth_lo1_setreference(si570_get_xtall_base() + si570_xtall_offset);
		#else
			synth_lo1_setreference(getsynthref(amode));	// ������ �����. ��� ������ �����������
		#endif
			synth_setreference(getsynthref(amode));	// ������ �����. ��� ������ �����������

		/* � ������ ����������� �������. */
	#if ! WITHDUMBIF
			update_lo2(pathi, workfilter, amode, mixXlsbs [2], freqlo2, getlo2div(gtx), gtx, lo2hint [pathi]);
			update_lo3(pathi, amode, mixXlsbs [3], freqlo3, getlo3div(gtx), gtx);

			synth_lo4_setfreq(pathi, freqlo4, getlo4div(gtx), getlo4enable(amode, gtx));	/* ���������� �������� ���������� */
	#endif /* ! WITHDUMBIF */

		} // pathi

		// ���������, �� ������� ��������� ��� ������ ����������
		update_lo0(lo0hint, lo0side);
		board_set_sleep(sleepflag);

		if (gtx == 0)
		{
			/* ��� �������� � ����� �������� ��� ��������� �� ������� -
			- ��������, �������� ����� ���� cross-band, �� ��� �� �����,
			����� ������� ���� �����������.
			*/
			/* ��� �������� � ����� �������� ��� ��������� �� ������� -
			- ��������, �������� ����� ���� cross-band, �� ��� �� �����,
			����� ������� ���� �����������.
			*/
		#if WITHNOATTNOPREAMP
			// ��� ���������� ������������ � ���
		#elif WITHONEATTONEAMP
			board_set_att(attmodes [gatts [rxbi]].codeatt);
			board_set_preamp(attmodes [gatts [rxbi]].codepre);
		#else /* WITHONEATTONEAMP */
			board_set_att(attmodes [gatts [rxbi]].code);
			board_set_preamp(pampmodes [gpamps [rxbi]].code);
		#endif /* WITHONEATTONEAMP */
		#if ! WITHAGCMODENONE
			board_set_agc(gagcoff ? BOARD_AGCCODE_OFF : agcmodes [gagcmode].code);
		#endif /* ! WITHAGCMODENONE */
		#if CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V3
			board_set_affilter(gaffilter);
		#endif /* CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V3 */
		#if WITHDSPEXTDDC	/* "��������" � DSP � FPGA */
			board_set_dither(gdither);	/* ���������� ����������� � LTC2208 */
			board_set_adcrand(gadcrand);	/* ���������� ����������� � LTC2208 */
			board_set_adcfifo(gadcfifo);
			board_set_adcoffset(gadcoffset + getadcoffsbase()); /* �������� ��� ��������� ������� � ��� */
		#endif /* WITHDSPEXTDDC */
		} /* (gtx == 0) */

	#if defined (RTC1_TYPE)
		board_setrtcstrobe(grtcstrobe);
	#endif /* defined (RTC1_TYPE) */
	#if WITHANTSELECT
		board_set_antenna(antmodes [gantennas [rxbi]].code [gtx]);
	#endif /* WITHANTSELECT */
	#if WITHELKEY
		#if ! WITHOPERA4BEACON
			board_set_wpm(elkeywpm);	/* �������� ������������ ����� */
		#endif /* ! WITHOPERA4BEACON */
		#if WITHVIBROPLEX
			elkey_set_slope(elkeyslopeenable ? elkeyslope : 0);	/* �������� ���������� ������������ ����� � ����� - �������� ����������� */
		#endif /* WITHVIBROPLEX */
		#if ELKEY328
			elkey_set_format(elkeyslopeenable ? 27 : dashratio, spaceratio);	/* ����������� ���� � ����� (� �������� ���������) */
		#else
			elkey_set_format(dashratio, spaceratio);	/* ����������� ���� � ����� (� �������� ���������) */
		#endif
			elkey_set_mode(elkeymode, elkeyreverse);	/* ����� ������������ ����� - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
		#if WITHTX
			seq_set_bkin_enable(bkinenable, bkindelay);			/* ��������� BREAK-IN */
			/*seq_rgbeep(0); */								/* ������������ roger beep */
		#endif /* WITHTX */
	#endif /* WITHELKEY */

	#if WITHIF4DSP
		board_set_afgain(sleepflag == 0 ? afgain1 : BOARD_AFGAIN_MIN);	// �������� ��� ����������� ������ �� ������ �����-���
		board_set_ifgain(sleepflag == 0  ? rfgain1 : BOARD_IFGAIN_MIN);	// �������� ��� ����������� �������� ��

		const uint_fast8_t txaprofile = gtxaprofiles [getmodetempl(txsubmode)->txaprofgp];	// �������� 0..NMICPROFILES-1

		#if ! defined (CODEC1_TYPE) && WITHUSBHW && WITHUSBUAC
			/* ���� ������������ ��� ����������� ����������� - ��� ����� ���������� �������� ���� � USB AUDIO */
			const uint_fast8_t txaudio = BOARD_TXAUDIO_USB;
		#elif WITHBBOX && defined (WITHBBOXMIKESRC)
			const uint_fast8_t txaudio = WITHBBOXMIKESRC;
		#else /* defined (WITHBBOXMIKESRC) */
			const uint_fast8_t txaudio = gtxaudio [txmode];
		#endif /* defined (WITHBBOXMIKESRC) */
		board_set_lineinput(txaudio == BOARD_TXAUDIO_LINE);
		board_set_detector(BOARD_DETECTOR_SSB);		/* ������ ������������ �������� */
		board_set_digigainmax(gdigigainmax);
		board_set_gvad605(gvad605);			/* ���������� �� AD605 (���������� ��������� ������ �� */
		board_set_fsadcpower10((int_fast16_t) gfsadcpower10 [lo0side != LOCODE_INVALID] - (int_fast16_t) FSADCPOWEROFFSET10);	/*	��������, ��������������� full scale �� IF ADC */
		#if WITHUSEDUALWATCH
			board_set_mainsubrxmode(getactualmainsubrx());		// �����/������, A - main RX, B - sub RX
		#endif /* WITHUSEDUALWATCH */
		#if WITHUSBUAC
			board_set_uacmike(gdatamode || getcattxdata() || txaudio == BOARD_TXAUDIO_USB);	/* �� ���� ���������� ������� ����������� � USB ����������� �����, � �� � ��������� */
			board_set_uacplayer((gtx && gdatamode) || guacplayer);/* ����� ������������� ������ ���������� � ��������� ���������� - ���������� ����� */
			#if WITHRTS96 || WITHRTS192 || WITHTRANSPARENTIQ
				board_set_swapiq(gswapiq);	/* �������� ������� I � Q ������ � ������ RTS96 */
			#endif /* WITHRTS96 || WITHRTS192 || WITHTRANSPARENTIQ */
		#endif /* WITHUSBUAC */
		board_set_mikebust20db(gmikebust20db);	// ��������� ������������� �� ����������
		board_set_lineamp(glineamp);	/* �������� � ��������� ����� */
		board_set_txaudio(txaudio);	// �������������� ��������� ������� ��� ��������
		board_set_mikeagc(gmikeagc);	/* ��������� ����������� ��� ����� ����������� */
		board_set_mikeagcgain(gmikeagcgain);	/* ������������ �������� ��� ��������� */
		board_set_mikehclip(gmikehclip);	/* ������������ */

		board_set_cwedgetime(gcwedgetime);	/* ����� ����������/����� ��������� ��������� ��� �������� - � 1 �� */
		board_set_sidetonelevel(gsidetonelevel);	/* ������� ������� ������������ � ��������� - 0%..100% */
	#endif /* WITHIF4DSP */

	#if WITHTX
		#if defined (CODEC1_TYPE) && WITHAFCODEC1HAVEPROC
			board_set_mikeequal(gmikeequalizer);	// ��������� ��������� ������� � ��������� (�������, ����������, ...)
			board_set_mikeequalparams(gmikeequalizerparams);	// ���������� 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
		#endif /* defined (CODEC1_TYPE) && WITHAFCODEC1HAVEPROC */
	#if WITHIF4DSP
		board_set_aflowcuttx(gssbtxlowcut10 * BWGRANLOW);	/* ������ ������� ����� ������� �� �� �������� */
		board_set_afhighcuttx(gssbtxhighcut100 * BWGRANHIGH);	/* ������� ������� ����� ������� �� �� �������� */
		board_set_afresponcetx(txmode == MODE_NFM ? + 24 : 0);	/* ��������� ��� �� ������ ����������� */
	#endif /* WITHIF4DSP */
		seq_set_rxtxdelay(rxtxdelay, txrxdelay, pretxdelay ? txrxdelay : 0);	/* ���������� �������� ��� �������� �� �������� � �������. */
		board_sidetone_setfreq(gcwpitch10 * CWPITCHSCALE);	// ������� - 400 ���� (���������� ������� ������ CAT Kenwood).
		board_set_txgate(gtxgate);		/* ���������� �������� � ���������� ��������� */
	#endif /* WITHTX */

	#if CTLSTYLE_IGOR
		board_set_bcdfreq100k(bandf100khint);
	#else /* CTLSTYLE_IGOR */
		board_set_bandf(bandfhint);		/* ��������� ������� ���������� ������� - �������� ������������ ��� */
		board_set_bandf2(bandf2hint);	/* ��������� ������� ���������� ������� (���) ����������� */
		board_set_bandf3(bandf3hint);	/* ���������� ����� ������ ACC */
	#endif /* CTLSTYLE_IGOR */

	#if WITHTX
		#if WITHMIC1LEVEL
			board_set_mik1level(mik1level);
		#endif /* WITHMIC1LEVEL */
		board_set_autotune(reqautotune);
		board_set_amdepth(gamdepth);	/* ������� ��������� � �� - 0..100% */
		board_set_dacscale(gdacscale);	/* ������������� ��������� ������� � ��� ����������� - 0..100% */
	#endif /* WITHTX */

		board_keybeep_setfreq(gkeybeep10 * 10);	// ������� ����������� ������� ������

	#if defined (DEFAULT_LCD_CONTRAST)
		display_set_contrast(gcontrast);
	#endif /* defined (DEFAULT_LCD_CONTRAST) */

	#if WITHFANTIMER
		board_setfanflag(! fanpaflag);
	#endif /* WITHFANTIMER */
	#if WITHDCDCFREQCTL
		board_set_blfreq(bldividerout);
	#endif /* WITHDCDCFREQCTL */
	#if WITHLCDBACKLIGHT
		board_set_bglight((dimmflag || sleepflag || dimmmode) ? WITHLCDBACKLIGHTMIN : bglight);		/* ��������� �������  */
	#endif /* WITHLCDBACKLIGHT */
	#if WITHKBDBACKLIGHT
		board_set_kblight((dimmflag || sleepflag || dimmmode) ? 0 : kblight);			/* �������� ���������� */
	#endif /* WITHKBDBACKLIGHT */
	#if WITHNBONOFF
		board_set_nfmnbon(lockmode);	/* �������� noise blanker �� SW2014FM */
	#endif /* WITHNBONOFF */

	#if WITHSPKMUTE
		board_set_loudspeaker(gmutespkr); /*  ���������� �������� */
	#endif /* WITHSPKMUTE */

	#if WITHAUTOTUNER
		board_set_tuner_group();
	#else /* WITHAUTOTUNER */
		board_set_tuner_bypass(1);
	#endif /* WITHAUTOTUNER */

		/* ������ �������� ������ � �� ����������� � �����-��������. */
	#if WITHCAT
		processcat_enable(catenable);
		cat_set_speed(catbr2int [catbaudrate] * BRSCALE);
	#endif	/* WITHCAT */

	#if WITHMODEM
		board_set_modem_speed100(modembr2int100 [gmodemspeed]);	// �������� �������� (���������, �� ���������������� ����) � ��������� 1/100 ���
		board_set_modem_mode(gmodemmode);	// ����������� ���������
	#endif /* WITHMODEM */

	#if WITHLFM
		synth_lfm_setparams(lfmstart100k * 100000ul, lfmstop100k * 100000ul, lfmspeed1k * 1000ul, getlo1div(gtx));
	#endif /* WITHLFM */

	#if WITHLO1LEVELADJ
		prog_dds1_setlevel(lo1level);
	#endif /* WITHLO1LEVELADJ */
	#if defined (DAC1_TYPE)
		board_set_dac1(dac1level);	/* ���������� ������� �������� ���������� */
	#endif /* defined (DAC1_TYPE) */

		board_update();		/* ������� ���������������� ��������� � �������� */
	} // full2 != 0
	
#if CTLSTYLE_RA4YBO_AM0
	{
		const int pathi = 0;
		const uint_fast8_t bi = getbankindex_pathi(pathi);
		const int_fast32_t freq = gfreqs [bi];
		const uint_fast32_t lo1 = synth_freq2lo1(freq, pathi);	
		if (gtx)
		{
			synth_lo1_setfreq(pathi, 0, getlo1div(gtx)); /* ��������� ������� ������� ���������� */
			synth_lo4_setfreq(pathi, getlo0(lo0hint) - freq, getlo1div(gtx), 1);
		}
		else
		{
			synth_lo1_setfreq(pathi, lo1, getlo1div(gtx)); /* ��������� ������� ������� ���������� */
			synth_lo4_setfreq(pathi, 0, getlo1div(gtx), 0);
		}
	}
#else /* CTLSTYLE_RA4YBO_AM0 */

	if (userfsg)
	{
		const uint_fast8_t bi = getbankindex_tx(gtx);
		const int_fast32_t freq = gfreqs [bi];
		synth_lo1_setfrequ(0, freq, getlo1div(gtx));
		//synth_bfo_setfreq(0);
		//synth_lopbt_setfreq(0);
	}
	else
	{
		for (pathi = 0; pathi < pathn; ++ pathi)
		{
			const uint_fast8_t bi = getbankindex_pathi(pathi);
			const int_fast32_t freq = gfreqs [bi];

			/* ������� ������� ���������� ����� ���������� ������������� */
			const uint_fast32_t lo1 = synth_freq2lo1(freq, pathi);	
			synth_lo1_setfreq(pathi, lo1, getlo1div(gtx)); /* ��������� ������� ������� ���������� */
			synth_rts1_setfreq(pathi, getlo0(lo0hint) - freq);	// ��������� ����������� ������� ����������� ����������
		}
	}
#endif /* CTLSTYLE_RA4YBO_AM0 */
	if (full2 != 0 && (mute != 0 || gtx != 0))
	{
		for (pathi = 0; pathi < pathn; ++ pathi)
		{
			//const uint_fast8_t bi = getbankindex_pathi(pathi);
			//const int_fast32_t freq = gfreqs [bi];

		}
		//local_delay_ms(20.0);		/* ��������� ��������� ������� */
	#if ! WITHIF4DSP
		//board_set_detector((mute && ! gtx) ? BOARD_DETECTOR_MUTE : pamodetempl->detector [gtx]);		// ����� ������ �� gsubmode
		board_set_detector(sleepflag ? BOARD_DETECTOR_MUTE : pamodetempl->detector [gtx]);	// �������� ����, ���� ����
		board_update();		/* ������� ���������������� ��������� � �������� */
	#endif /* WITHIF4DSP */
			// todo: ������ ����� ������� ������� � ��������� ����� ������ � ��������� � FPGA
	#if WITHTX
		//board_set_tx(gtx);		/* � ����� ����� ������ ���������� �������� */
		//board_update();		/* ������� ���������������� ��������� � �������� */
	#endif /* WITHTX */
	}

	/* ����� ���� ������������ �������� ���������� */
#if WITHTX
	board_set_tx(gtx);		/* � ����� ����� ������ ���������� �������� */
	board_update();		/* ������� ���������������� ��������� � �������� */
#endif /* WITHTX */
}

///////////////////////////
// ����������� ������ ����������

//////////////////////////

#if WITHVOX && WITHTX

static void 
uif_key_voxtoggle(void)
{
	gvoxenable = calc_next(gvoxenable, 0, 1);
	save_i8(offsetof(struct nvmap, gvoxenable), gvoxenable);
	updateboard(1, 0);
}

// ������� ��������� VOX
uint_fast8_t hamradio_get_voxvalue(void)
{
	return gvoxenable;
}

#else /* WITHVOX && WITHTX */
// ������� ��������� VOX
uint_fast8_t hamradio_get_voxvalue(void)
{
	return 0;
}

#endif /* WITHVOX && WITHTX */


#if WITHKEYBOARD

// ��������� ������ split (�������, � ���������� �� ������� �������)
static void 
uif_key_spliton(uint_fast8_t holded)
{
#if WITHSPLIT

	const uint_fast8_t srbi = getbankindex_raw(0);
	const uint_fast8_t tgbi = getbankindex_raw(1);
	const vindex_t tgvi = getvfoindex(tgbi);

	copybankstate(srbi, tgbi, holded == 0 ? 0 : getmodetempl(getsubmode(srbi))->autosplitK * 1000L);	/* �������� ��������� �������� ����� � ��������������� */
	gsplitmode = VFOMODES_VFOSPLIT;

	savebandstate(tgvi, tgbi); // �������� ��� ��������� ��������� (����� �������) � ������� ������ VFO */
	savebandfreq(tgvi, tgbi);

	save_i8(RMT_SPLITMODE_BASE, gsplitmode);
	updateboard(1, 1);

#elif WITHSPLITEX

	if (holded != 0)
	{
		const uint_fast8_t srbi = getbankindex_raw(0);
		const uint_fast8_t tgbi = getbankindex_raw(1);
		const vindex_t tgvi = getvfoindex(tgbi);

		copybankstate(srbi, tgbi, getmodetempl(getsubmode(srbi))->autosplitK * 1000L);	/* �������� ��������� �������� ����� � ��������������� */
	
		savebandstate(tgvi, tgbi); // �������� ��� ��������� ��������� (����� �������) � ������� ������ VFO */
		savebandfreq(tgvi, tgbi);
	}
	gsplitmode = VFOMODES_VFOSPLIT;

	save_i8(RMT_SPLITMODE_BASE, gsplitmode);
	updateboard(1, 1);

#else

#endif /* WITHSPLIT */
}

/* ����������� � VFO B ��������� VFO A */
// for WITHSPLITEX
static void 
uif_key_click_b_from_a(void)
{
#if (WITHSPLIT || WITHSPLITEX)

	const uint_fast8_t sbi = getbankindex_ab(0);	// bank index �������� ������
	const uint_fast8_t tbi = getbankindex_ab(1);	// bank index ���� ���������� ������
	const vindex_t tgvi = getvfoindex(tbi);		// vfo index ���� ���������� ������

	copybankstate(sbi, tbi, 0);
	savebandstate(tgvi, tbi); // �������� ��� ��������� ��������� (����� �������) � ������� ������ VFO */
	savebandfreq(tgvi, tbi); // �������� ������� � ������� ������ VFO */
	updateboard(1, 1);

#endif /* (WITHSPLIT || WITHSPLITEX) */
}

// ��������� ������ split
static void 
uif_key_splitoff(void)
{
#if (WITHSPLIT || WITHSPLITEX)

	gsplitmode = VFOMODES_VFOINIT;
	save_i8(RMT_SPLITMODE_BASE, gsplitmode);

	updateboard(1, 1);

#endif /* (WITHSPLIT || WITHSPLITEX) */
}

/* ����� �������� ����� VFO */
// for WITHSPLITEX
static void 
uif_key_click_a_ex_b(void)
{
#if (WITHSPLIT || WITHSPLITEX)

	gvfoab = ! gvfoab;	/* ������ ������� VFO �� �������������� */
	save_i8(RMT_VFOAB_BASE, gvfoab);
	updateboard(1, 1);

#endif /* (WITHSPLIT || WITHSPLITEX) */
}

///////////////////////////
// ����������� ������ ����������
//////////////////////////
/* ������� ����� �������� ������������� ������
   ��� ������� ���������.
  */
//static void 
//uif_key_memmode(uint_fast8_t tx)
//{
//while (repeat --)
//	gsplitmode = calc_next(gsplitmode, 0, VFOMODES_COUNT - 1); /* (vfo/vfoa/vfob/mem) */
//save_i8(RMT_SPLITMODE_BASE, gsplitmode);
//updateboard(1, 0);
//}

///////////////////////////
// ����������� ������ ����������
//////////////////////////
/* ������� �� "������" ������� - ���������� ������� */
// step to next modecol
static void 
uif_key_hold_modecol(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const uint_fast8_t moderow = gmoderows [bi];	/* ������ ������� ����������� ������� */

	uint_fast8_t modecol = getmodecol(moderow, modes [moderow][0] - 1, 0, bi);	/* ������� �� ������� �����. ��������, �������� modecolmap ���� ����������������.  */
	modecol = calc_next(modecol, 0, modes [moderow][0] - 1);
	putmodecol(moderow, modecol, bi);	/* ������ ����� �������� � ������� ����� */
	/* ������������� ������ ���� ����������� ����� ����� ������� */
	/* gband ������ ���� ��� �������� */
	gsubmodechange(getsubmode(bi), bi); /* ���� ���� - ���������� ������� � ������� VFO */
	updateboard(1, 1);
}


///////////////////////////
// ����������� ������ ����������
//////////////////////////
/* ������� �� "�������" ������� - ������� ������� */
/* switch to next moderow */
static void 
uif_key_click_moderow(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	uint_fast8_t defrow = gmoderows [bi];		/* ������ ������� �������, ������� �������� */
	uint_fast8_t defcol = getmodecol(defrow, modes [defrow][0] - 1, 0, bi);	/* ������� �� ������� �����. ��������, �������� modecolmap ���� ����������������.  */
	const uint_fast8_t forcelsb = getforcelsb(gfreqs [bi]);

	gmoderows [bi] = calc_next(gmoderows [bi], 0, MODEROW_COUNT - 1);		/* ��� �� ��������� ������ ������� ���������� ������� */

#if WITHMODESETSMART
	defcol = locatesubmode(SUBMODE_SSBSMART, & defrow);
#else /* WITHMODESETSMART */
	if (gsubmode == SUBMODE_USB)		// ���� ������� ����� USB - ����� CW
		defcol = locatesubmode(SUBMODE_CW, & defrow);
	else if (gsubmode == SUBMODE_LSB)	// ���� ������� ����� LSB - ����� CWR
		defcol = locatesubmode(SUBMODE_CWR, & defrow);
	else if (gsubmode == SUBMODE_DGU)	// ���� ������� ����� LSB - ����� CWR
		defcol = locatesubmode(SUBMODE_USB, & defrow);
	else if (gsubmode == SUBMODE_DGL)	// ���� ������� ����� LSB - ����� CWR
		defcol = locatesubmode(SUBMODE_LSB, & defrow);
	#if WITHMODESETFULLNFM
	else if (gsubmode == SUBMODE_AM)	// ���� ������� ����� AM - ����� FM
		defcol = locatesubmode(SUBMODE_NFM, & defrow);
	#endif
	else								// � ��������� ������� ���� ����� �� ��������� ��� ������� ��������� ������
		defcol = locatesubmode(forcelsb ? SUBMODE_LSB : SUBMODE_USB, & defrow);
#endif /* WITHMODESETSMART */	
	/* ���� ��������� �� �� ������ � ���������� �������� */
	if (defrow != gmoderows [bi])
		defcol = 0;	/* default value (other cases, then switch from usb to cw, from lsb to cwr) */
	/* �������� ���������� �� ������ - ���, ��������, ���������� ��������� defcol */
	(void) getmodecol(gmoderows [bi], modes [gmoderows [bi]][0] - 1, defcol, bi); /* ��������, �������� modecolmap ���� ����������������. */

	/* ������������� ������ ���� ����������� ����� ����� ������� */
	/* gband ������ ���� ��� �������� */
	gsubmodechange(getsubmode(bi), bi); /* ���� ���� - ���������� ������� � ������� VFO */
	updateboard(1, 1);
}

///////////////////////////
// ����������� ������ ����������
//////////////////////////
/* ������� �� "�������" ������� - ������� ������� */
/* switch to next moderow */
static void 
uif_key_click_moderows(uint_fast8_t moderow)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const uint_fast8_t rowchanged = (gmoderows [bi] != moderow);
	uint_fast8_t defrow = gmoderows [bi] = moderow;		/* ������ ������� �������, ������� �������� */
	uint_fast8_t defcol = getmodecol(defrow, modes [defrow][0] - 1, 0, bi);	/* ������� �� ������� �����. ��������, �������� modecolmap ���� ����������������.  */
	const uint_fast8_t forcelsb = getforcelsb(gfreqs [bi]);

#if WITHMODESETSMART
	defcol = locatesubmode(SUBMODE_SSBSMART, & defrow);
#else /* WITHMODESETSMART */
	if (gsubmode == SUBMODE_USB)		// ���� ������� ����� USB - ����� CW
		defcol = locatesubmode(SUBMODE_CW, & defrow);
	else if (gsubmode == SUBMODE_LSB)	// ���� ������� ����� LSB - ����� CWR
		defcol = locatesubmode(SUBMODE_CWR, & defrow);
	else if (gsubmode == SUBMODE_DGU)	// ���� ������� ����� LSB - ����� CWR
		defcol = locatesubmode(SUBMODE_USB, & defrow);
	else if (gsubmode == SUBMODE_DGL)	// ���� ������� ����� LSB - ����� CWR
		defcol = locatesubmode(SUBMODE_LSB, & defrow);
	#if WITHMODESETFULLNFM
	else if (gsubmode == SUBMODE_AM)	// ���� ������� ����� AM - ����� FM
		defcol = locatesubmode(SUBMODE_NFM, & defrow);
	#endif
	else								// � ��������� ������� ���� ����� �� ��������� ��� ������� ��������� ������
		defcol = locatesubmode(forcelsb ? SUBMODE_LSB : SUBMODE_USB, & defrow);
#endif /* WITHMODESETSMART */	
	/* ���� ��������� �� �� ������ � ���������� �������� */
	if (defrow != gmoderows [bi])
		defcol = 0;	/* default value (other cases, then switch from usb to cw, from lsb to cwr) */
	/* �������� ���������� �� ������ - ���, ��������, ���������� ��������� defcol */
	(void) getmodecol(gmoderows [bi], modes [gmoderows [bi]][0] - 1, defcol, bi); /* ��������, �������� modecolmap ���� ����������������. */

	/* ������������� ������ ���� ����������� ����� ����� ������� */
	/* gband ������ ���� ��� �������� */
	gsubmodechange(getsubmode(bi), bi); /* ���� ���� - ���������� ������� � ������� VFO */
	updateboard(1, 1);
}

///////////////////////////
// ����������� ������ ����������
//////////////////////////
/* ������� �� "������" ������� - ���������� ������� */
// step to next modecol
static void 
uif_key_hold_modecols(uint_fast8_t moderow)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	if (gmoderows [bi] != moderow)	/* ������ ������� ����������� ������� */
	{
		uif_key_click_moderows(moderow);
		return;
	}

	uint_fast8_t modecol = getmodecol(moderow, modes [moderow][0] - 1, 0, bi);	/* ������� �� ������� �����. ��������, �������� modecolmap ���� ����������������.  */
	modecol = calc_next(modecol, 0, modes [moderow][0] - 1);
	putmodecol(moderow, modecol, bi);	/* ������ ����� �������� � ������� ����� */
	/* ������������� ������ ���� ����������� ����� ����� ������� */
	/* gband ������ ���� ��� �������� */
	gsubmodechange(getsubmode(bi), bi); /* ���� ���� - ���������� ������� � ������� VFO */
	updateboard(1, 1);
}

///////////////////////////
// ����������� ������ ����������
//////////////////////////

// �������� ������� ������ BAND UP
/* ������� �� ��������� (� ������� ��������) �������� */
static void 
uif_key_click_bandup(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const uint_fast8_t b = getfreqband(gfreqs [bi]);	/* ���������� �� �������, � ����� ��������� ��������� */
	verifyband(b);
	savebandstate(b, bi); // �������� ��� ��������� ��������� (����� �������) � ������� ������ ��������� */
	savebandfreq(b, bi);
	const vindex_t bn = getnext_ham_band(b, gfreqs [bi]);
	loadnewband(bn, bi);	/* �������� ���� ���������� (� �������) ������ ������ */
	savebandgroup(bn);
	savebandfreq(vi, bi);	/* ���������� ������� � ������� VFO */
	savebandstate(vi, bi); // �������� ��� ��������� ��������� (����� �������)  � ������� VFO */
	updateboard(1, 1);
}
///////////////////////////
// ����������� ������ ����������
//////////////////////////
// �������� ������� ������ BAND DOWN
/* ������� �� ���������� (� ������� ��������) �������� */
static void 
uif_key_click_banddown(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const vindex_t b = getfreqband(gfreqs [bi]);	/* ���������� �� �������, � ����� ��������� ��������� */
	verifyband(b);
	savebandstate(b, bi); // �������� ��� ��������� ��������� (����� �������) � ������� ������ ��������� */
	savebandfreq(b, bi);
	const uint_fast8_t bn = getprev_ham_band(b, gfreqs [bi]);
	loadnewband(bn, bi);	/* �������� ���� ���������� (� �������) ������ ������ */
	savebandgroup(bn);
	savebandfreq(vi, bi);	/* ���������� ������� � ������� VFO */
	savebandstate(vi, bi); // �������� ��� ��������� ��������� (����� �������)  � ������� VFO */
	updateboard(1, 1);
}


/* ������� �� ��������, ���������� ��������� ������� */
static void 
uif_key_click_banddjump(uint_fast32_t f)
{
#if	WITHDIRECTBANDS
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);
	const vindex_t b = getfreqband(gfreqs [bi]);	/* ���������� �� �������, � ����� ��������� ��������� */
	vindex_t bn = getfreqband(f);
	const uint_fast8_t bandgroup = bandsmap [bn].bandgroup;
	verifyband(b);
	verifyband(bn);
	savebandstate(b, bi); // �������� ��� ��������� ��������� (����� �������) � ������� ������ ��������� */
	savebandfreq(b, bi);
	// 
	//
	if (bandgroup != BANDGROUP_COUNT)
	{
		// ����� ����������� �������� ������ ������
		bn = loadvfy8up(RMT_BANDGROUP(bandgroup), 0, HBANDS_COUNT - 1, bn);
		verifyband(bn);
		if (bandgroup == bandsmap [b].bandgroup)
		{
			// ������������ � �������� ��� �� ������ - ������� � �������� ������
			bn = getnextbandingroup(bn, bandgroup);
			verifyband(bn);
			save_i8(RMT_BANDGROUP(bandgroup), bn);
		}
	}
	loadnewband(bn, bi);	/* �������� ���� ���������� (� �������) ������ ������ */
	savebandfreq(vi, bi);	/* ���������� ������� � ������� VFO */
	savebandstate(vi, bi); // �������� ��� ��������� ��������� (����� �������)  � ������� VFO */
	updateboard(1, 1);
#endif /* WITHDIRECTBANDS */
}

/* AGC mode switch
	 - �������� ���������� ��������� ������ */
static void 
uif_key_click_agcmode(void)
{
	gagcmode = calc_next(gagcmode, 0, AGCMODE_COUNT - 1);
	save_i8(RMT_AGC_BASE(submodes [gsubmode].mode), gagcmode);
	updateboard(1, 0);
}

#if WITHANTSELECT
#endif /* WITHANTSELECT */

#if WITHANTSELECT

/* Antenna switch
	  */
static void 
uif_key_click_antenna(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);

	verifyband(vi);

	gantennas [bi] = calc_next(gantennas [bi], 0, ANTMODE_COUNT - 1);
	savebandstate(vi, bi);	// ������ ���� ������� � ������� ������ ���������
	updateboard(1, 0);
}

// antenna
const FLASHMEM char * hamradio_get_ant5_value_P(void)
{
	return antmodes [gantennas [getbankindex_tx(gtx)]].label5;
}

#endif /* WITHANTSELECT */

#if ! WITHONEATTONEAMP
/* ������������ ������ �������������  */
static void 
uif_key_click_pamp(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);

	verifyband(vi);

	gpamps [bi] = calc_next(gpamps [bi], 0, PAMPMODE_COUNT - 1);
	savebandstate(vi, bi);	// ������ ���� ������� � ������� ������ ���������
	updateboard(1, 0);
}
#endif /* ! WITHONEATTONEAMP */


/* ������������ ������ �����������  */
static void 
uif_key_click_attenuator(void)
{
	const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
	const vindex_t vi = getvfoindex(bi);

	verifyband(vi);

	gatts [bi] = calc_next(gatts [bi], 0, ATTMODE_COUNT - 1);
	savebandstate(vi, bi);	// ������ ���� ������� � ������� ������ ���������
	updateboard(1, 0);
}

#if WITHPOWERLPHP
/* ������������ ������ ��������  */
static void 
uif_key_click_pwr(void)
{
	gpwr = calc_next(gpwr, 0, PWRMODE_COUNT - 1);
	save_i8(RMT_PWR_BASE, gpwr);

	updateboard(1, 0);
}
#endif /* WITHPOWERLPHP */

#if WITHNOTCHONOFF || WITHNOTCHFREQ
/* ������������ ������ NOTCH  */
static void 
uif_key_click_notch(void)
{
	gnotch = calc_next(gnotch, 0, NOTCHMODE_COUNT - 1);
	save_i8(RMT_NOTCH_BASE, gnotch);

	updateboard(1, 0);
}

#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */

#if ELKEY328

static void 
uif_key_click_vibroplex(void)
{
	elkeyslopeenable = calc_next(elkeyslopeenable, 0, 1);
	updateboard(1, 0);
}

#endif /* ELKEY328 */

/* ���������� �������� */
static void 
uif_key_lockencoder(void)
{
	lockmode = calc_next(lockmode, 0, 1);
	save_i8(RMT_LOCKMODE_BASE, lockmode);
	updateboard(1, 0);
}

#if WITHBCBANDS
/* ������� � ����� ������������ �� ����������� ���������� */
static void
uif_key_genham(void)
{
	bandsetbcast = calc_next(bandsetbcast, 0, 1);
	save_i8(offsetof(struct nvmap, bandsetbcast), bandsetbcast);
	updateboard(1, 0);
}

uint_fast8_t hamradio_get_genham_value(void)
{
	return bandsetbcast;
}

#endif /* WITHBCBANDS */

#if WITHUSEFAST
/* ������������ � ����� �������� ���� */
static void 
uif_key_usefast(void)
{
	gusefast = calc_next(gusefast, 0, 1);
	save_i8(RMT_USEFAST_BASE, gusefast);
	updateboard(1, 0);
}
#endif /* WITHUSEFAST */

#if WITHSPKMUTE

/* ���������-���������� �������� */
static void 
uif_key_loudsp(void)
{
	gmutespkr = calc_next(gmutespkr, 0, 1);
	save_i8(RMT_MUTELOUDSP_BASE, gmutespkr);
	updateboard(1, 0);
}
#endif /* WITHSPKMUTE */

#if CTLSTYLE_RA4YBO

/* ��������� ��� �� ����� � �������� RA4YBO */
static void 
uif_key_affilter(void)
{
	gaffilter = calc_next(gaffilter, 0, 1);
	save_i8(RMT_AFFILTER_BASE, gaffilter);
	updateboard(1, 0);
}

///////////////////////////
// ����������� ������ ����������
//////////////////////////

#elif CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 || CTLSTYLE_RA4YBO_V3 || KEYBSTYLE_RA4YBO_AM0

static void 
uif_key_user1(void)
{
	guser1 = calc_next(guser1, 0, 1);
	save_i8(RMT_USER1_BASE, guser1);
	updateboard(1, 0);
}

static void 
uif_key_user2(void)
{
	guser2 = calc_next(guser2, 0, 1);
	save_i8(RMT_USER2_BASE, guser2);
	updateboard(1, 0);
}
static void 
uif_key_user3(void)
{
	guser3 = calc_next(guser3, 0, 1);
	save_i8(RMT_USER3_BASE, guser3);
	updateboard(1, 0);
}

static void 
uif_key_user4(void)
{
	guser4 = calc_next(guser4, 0, 1);
	save_i8(RMT_USER4_BASE, guser4);
	updateboard(1, 0);
}
static void 
uif_key_user5(void)
{
	guser5 = calc_next(guser5, 0, 1);
	save_i8(RMT_USER5_BASE, guser5);
	updateboard(1, 0);
}

/* ��������� ��� �� ����� � �������� RA4YBO */
static void 
uif_key_affilter(void)
{
	gaffilter = calc_next(gaffilter, 0, 1);
	save_i8(RMT_AFFILTER_BASE, gaffilter);
	updateboard(1, 0);
}

#endif /* CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 */
///////////////////////////
// ����������� ������ ����������
//////////////////////////
#if WITHIF4DSP
/* ������������ ����� ����������� � CW ��������
	 - �� �������� ���������� ��������� ��������� */

static void 
uif_key_changebw(void)
{
	const uint_fast8_t bwseti = mdt [gmode].bwseti;	// ������ ����� ����� ����������� ��� ������� ������

	bwsetpos [bwseti] = calc_next(bwsetpos [bwseti], 0, bwsetsc [bwseti].last);
	save_i8(RMT_BWSETPOS_BASE(bwseti), bwsetpos [bwseti]);	/* ������ ����� ��������� ����� ������ ��� ������ */
	updateboard(1, 1);
}

#if WITHUSBUAC

/* ������������ ��������� ����� � USB ��� �������� ��� ������� ������ */
static void 
uif_key_click_datamode(void)
{
	gdatamode = calc_next(gdatamode, 0, 1);
	save_i8(RMT_DATAMODE_BASE, gdatamode);
	updateboard(1, 0);
}

#endif /* WITHUSBUAC */


#else /* WITHIF4DSP */
/* ������������ �������� ��������
	 - �� �������� ���������� ��������� ��������� */
static void 
uif_key_changefilter(void)
{
	gfi = getsuitablerx(gmode, calc_next(gfi, 0, getgfasize() - 1));
	save_i8(RMT_FILTER_BASE(gmode), gfi);	/* ������ ����� ��������� ����� ������ ��� ������ */
	updateboard(1, 1);
}

#endif /* WITHIF4DSP */
///////////////////////////
// ����������� ������ ����������
//////////////////////////
#if WITHTX
/* ��������� ������ ��������� */
static void 
uif_key_tuneoff(void)
{
	if (getactualtune() || moxmode)
	{
		moxmode = 0;
		tunemode = 0;		/* �� �����, �� ����� ������� ��������� �� �������� - ����� �� ������ ��� ��������� */
#if WITHCAT
		cattunemode = 0;
#endif /* WITHCAT */
	}
	else
	{
		moxmode = calc_next(moxmode, 0, 1);
	}
	updateboard(1, 1);
}

///////////////////////////
// ����������� ������ ����������
//////////////////////////
/* ��������� ������ ��������� */

static void 
uif_key_tune(void)
{
	tunemode = calc_next(tunemode, 0, 1);
	updateboard(1, 1);
}
#endif /* WITHTX */

#endif /* WITHKEYBOARD */


#if WITHAUTOTUNER

static void 
uif_key_bypasstoggle(void)
{
	tunerwork = calc_next(tunerwork, 0, 1);
	save_i8(offsetof(struct nvmap, tunerwork), tunerwork);
	if (tunerwork == 0)
		reqautotune = 0;
	updateboard(1, 0);
}

static void 
uif_key_atunerstart(void)
{
	reqautotune = 1;
	// ��������� �������� � ����� �������� �������� � �������� �����
	tunerwork = 1;
	save_i8(offsetof(struct nvmap, tunerwork), tunerwork);
	updateboard(1, 0);
}

uint_fast8_t
hamradio_get_bypvalue(void)
{
	return ! tunerwork;
}

uint_fast8_t
hamradio_get_atuvalue(void)
{
	return reqautotune;
}
#endif /* WITHAUTOTUNER */

#if WITHNOTCHONOFF || WITHNOTCHFREQ

uint_fast8_t hamradio_get_notchvalue(int_fast32_t * p)
{
#if WITHNOTCHFREQ
	* p = gnotchfreq;
#endif /* WITHNOTCHFREQ */
	return gnotch;
}

#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ  */


// ������� ��������� TUNE
uint_fast8_t hamradio_get_tunemodevalue(void)
{
	return getactualtune();
}

#if WITHVOLTLEVEL && WITHCPUADCHW

// ������ � ������� �����
uint_fast8_t hamradio_get_volt_value(void)
{
#if WITHREFSENSOR
	// ��������� ������� ����������
	const uint_fast8_t vrefi = VREFIX;
	const adcvalholder_t ref = board_getadc_unfiltered_truevalue(vrefi);	// ������� �������� ������� ���
	if (ref != 0)
	{
		//const unsigned Vref_mV = ADCVREF_CPU * 100;
		const unsigned Vref_mV = (uint_fast32_t) board_getadc_fsval(vrefi) * WITHREFSENSORVAL / ref;
		const unsigned voltcalibr_mV = (Vref_mV * (VOLTLEVEL_UPPER + VOLTLEVEL_LOWER) + VOLTLEVEL_LOWER / 2) / VOLTLEVEL_LOWER;		// ���������� fullscale - ��� �������� ��� ADCVREF_CPU ����� �� ����� ���
		const uint_fast16_t mv = board_getadc_unfiltered_u16(VOLTSOURCE, 0, voltcalibr_mV);
		//debug_printf_P(PSTR("hamradio_get_volt_value: ref=%u, VrefmV=%u, v=%u, out=%u\n"), ref, Vref_mV, mv, (mv + 50) / 100);
		return (mv + 50) / 100;	// �������� � ������� ����� ������
	}
	else
	{
		debug_printf_P(PSTR("hamradio_get_volt_value: ref=%u\n"), ref);
		return 3333;
	}
#else /* WITHREFSENSOR */
		debug_printf_P(PSTR("hamradio_get_volt_value: VOLTSOURCE=%u\n"), board_getadc_filtered_u8(VOLTSOURCE, 0, voltcalibr));
	return board_getadc_filtered_u8(VOLTSOURCE, 0, voltcalibr);
#endif /* WITHREFSENSOR */
}

#endif /* WITHVOLTLEVEL && WITHCPUADCHW */

#if WITHTHERMOLEVEL && WITHCPUADCHW

// ������� � ������� �����
// Read from thermo sensor ST LM235Z (1 kOhm to +3.3)
int_fast16_t hamradio_get_temperature_value(void)
{
	const int_fast16_t offset_LM235 = - 2731;	// -273.15 approximation of temperature at 0 volt. Slope = 10 mV / celsius
#if WITHREFSENSOR
	// ��������� ������� ����������
	const uint_fast8_t vrefi = VREFIX;
	const adcvalholder_t ref = board_getadc_unfiltered_truevalue(vrefi);	// ������� �������� ������� ���
	if (ref != 0)
	{
		const unsigned Vref_mV = (uint_fast32_t) board_getadc_fsval(vrefi) * WITHREFSENSORVAL / ref;
		const int_fast32_t mv = (int32_t) board_getadc_unfiltered_u32(XTHERMOIX, 0, (uint_fast64_t) Vref_mV * (THERMOSENSOR_UPPER + THERMOSENSOR_LOWER) / THERMOSENSOR_LOWER);
		return mv + offset_LM235;	// �������� � ������� ����� �������
	}
	else
	{
		debug_printf_P(PSTR("hamradio_get_temperature_value: ref=%u\n"), ref);
		return 999;
	}
#else /* WITHREFSENSOR */
	const unsigned Vref_mV = ADCVREF_CPU * 100;
	debug_printf_P(PSTR("hamradio_get_temperature_value: XTHERMOIX=%u\n"), board_getadc_filtered_u16(XTHERMOIX, 0, Vref_mV));
	return (int32_t) board_getadc_unfiltered_u32(XTHERMOIX, 0, (uint_fast64_t) Vref_mV * (THERMOSENSOR_UPPER + THERMOSENSOR_LOWER) / THERMOSENSOR_LOWER) + offset_LM235;
#endif /* WITHREFSENSOR */
}

#endif /* WITHTHERMOLEVEL && WITHCPUADCHW */

// Read ADC MCP3204/MCP3208
static uint_fast16_t 
mcp3208_read(
	spitarget_t target,
	uint_fast8_t diff,
	uint_fast8_t adci
	)
{
	uint_fast8_t v1, v2;
	const uint_fast8_t ch = adci & 0x07;
	const uint_fast8_t cmd1 = 0x04 | (diff ? 0x00 : 0x02) | (ch >> 2);
	const uint_fast8_t cmd2 = 0xFF & (ch << 6);
	
	prog_select(target); //spi_select2(target, SPIC_MODE3, SPIC_SPEED_1M2);	// for 50 kS/S and 24 bit words
	//spi_select(target, SPIC_MODE3);

	prog_read_byte(target, cmd1);
	v1 = prog_read_byte(target, cmd2);
	v2 = prog_read_byte(target, 0x00);

	prog_unselect(target);

	return (v1 * 256 + v2) & 0x1FFF;
}

#if (WITHCURRLEVEL || WITHCURRLEVEL2) && WITHCPUADCHW

// ��� � �������� ��������� (����� ���� �������������)
// PA current sense - ACS712ELCTR-05B-T chip
// PA current sense - ACS712ELCTR-30B-T chip
int_fast16_t hamradio_get_pacurrent_value(void)
{
#if 0


	// x30A - 0.066 V/A
	enum { 
		sens = 66,			// millivolts / ampher
		scale = 100			// ��������� - � ������� ����� ������
	};

	const uint_fast8_t adci = 0;	// CH0=IN-, CH1=IN+
	const uint_fast8_t diff = 0;
	static const long fullscale2 = 3300uL * scale;
	uint_fast16_t vsenseZ = mcp3208_read(targetext2, diff, adci);
	uint_fast16_t vsense2 = (unsigned long) vsenseZ * fullscale2 / 4095;

	static const long midpoint2 = 2500uL * scale;
	int curr10a = ((long) midpoint2 - (long) vsense2 + sens / 2) / sens;

	return curr10a;

#else

#if WITHCURRLEVEL
	// ���������������� ��������:
	// x05B - 0.185 V/A
	// x20A - 0.100 V/A
	// x30A - 0.066 V/A

	#if CTLSTYLE_RA4YBO_V3
		// x30A - 0.066 V/A
		enum { 
			sens = 66,			// millivolts / ampher
			scale = 100			// ��������� - � ������� ����� ������
		};
	#else /* CTLSTYLE_RA4YBO_V3 */
		// x05B - 0.185 V/A
		enum { 
			sens = 185,			// millivolts / ampher
			scale = 100			// ��������� - � ����� ����� ������
		};
	#endif /* CTLSTYLE_RA4YBO_V3 */

	const uint_fast8_t adci = PASENSEIX;

#elif WITHCURRLEVEL2
	// ���������������� ��������:
	// x05B - 0.185 V/A
	// x20A - 0.100 V/A
	// x30A - 0.066 V/A

	#if 1
		// x30A - 0.066 V/A
		enum { 
			sens = 66,			// millivolts / ampher
			scale = 100			// ��������� - � ������� ����� ������
		};
	#else /*  */
		// x05B - 0.185 V/A
		enum { 
			sens = 185,			// millivolts / ampher
			scale = 100			// ��������� - � ����� ����� ������
		};
	#endif /*  */

	const uint_fast8_t adci = PASENSEIX2;

#endif

#if WITHREFSENSOR
	// ��������� ������� ����������
	const uint_fast8_t vrefi = VREFIX;
	const adcvalholder_t ref = board_getadc_unfiltered_truevalue(vrefi);	// ������� �������� ������� ���
	if (ref == 0)
		return 0;
	//const unsigned Vref_mV = ADCVREF_CPU * 100;
	const unsigned Vref_mV = (uint_fast32_t) board_getadc_fsval(vrefi) * WITHREFSENSORVAL / ref;
#else /* WITHREFSENSOR */
	// ������� ���������� �������� �� ������������ ����������.
	const unsigned Vref_mV = ADCVREF_CPU * 100;
#endif /* WITHREFSENSOR */

	const long vsense = board_getadc_unfiltered_u32(PASENSEIX, 0, (uint_fast32_t) Vref_mV * scale);
#if CTLSTYLE_RAVENDSP_V5
	const long midpoint = (gtx ? 2472uL : 2442uL) * scale; // tx=247200, rx=244200
#else
	static const long midpoint = 2516uL * scale;
#endif

	int curr10 = ((long) midpoint - (long) vsense + sens / 2) / sens;

	//debug_printf_P(PSTR("voltage vsense=%lu, midpoint=%lu, delta=%d mV, current=%d * 10 mA\n"), vsense, midpoint, v / scale, curr10);

	return curr10;
#endif
}

#endif /* WITHCURRLEVEL && WITHCPUADCHW */

uint_fast8_t hamradio_get_tx(void)
{
	return gtx;
}

// RX bandwidth
#if WITHIF4DSP

const FLASHMEM char * hamradio_get_rxbw_value_P(void)
{
	const uint_fast8_t bwseti = mdt [gmode].bwseti;	// ������ ����� ����� ����������� ��� ������� ������
	return bwsetsc [bwseti].labels [bwsetpos[bwseti]];
}

#else /* WITHIF4DSP */

const FLASHMEM char * hamradio_get_rxbw_value_P(void)
{
#if WITHFIXEDBFO
	return PSTR("");
#else /* WITHFIXEDBFO */
	return getrxfilter(gsubmode, gfi)->labelf3;
#endif /* WITHFIXEDBFO */
}

#endif /* WITHIF4DSP */

// RX preamplifier
const FLASHMEM char * hamradio_get_pre_value_P(void)
{
#if ! WITHONEATTONEAMP
	return pampmodes [gpamps [getbankindex_tx(0)]].label;
#else /* ! WITHONEATTONEAMP */
	return PSTR("   ");
#endif /* ! WITHONEATTONEAMP */
}

// RX attenuator (or att/pre).
const FLASHMEM char * hamradio_get_att_value_P(void)
{
	return attmodes [gatts [getbankindex_tx(0)]].label;
}

// RX agc time - 3 �������
const FLASHMEM char * hamradio_get_agc3_value_P(void)
{
#if ! WITHAGCMODENONE
	return agcmodes [gagcmode].label3;
#else /* ! WITHAGCMODENONE */
	return PSTR("   ");
#endif /* ! WITHAGCMODENONE */
}

// RX agc time - 4 �������
const FLASHMEM char * hamradio_get_agc4_value_P(void)
{
#if ! WITHAGCMODENONE
	return agcmodes [gagcmode].label4;
#else /* ! WITHAGCMODENONE */
	return PSTR("    ");
#endif /* ! WITHAGCMODENONE */
}

#if WITHPOWERLPHP
// HP/LP
const FLASHMEM char * hamradio_get_hplp_value_P(void)
{
	return pwrmodes [gpwr].label;
}
#endif /* WITHPOWERLPHP */


///////////////////////////
// ����������� ������ ����������
//////////////////////////
#if WITHUSEDUALWATCH

/* ������������ ���������/��������������� �������� */
static void 
uif_key_mainsubrx(void)
{
	mainsubrxmode = calc_next(mainsubrxmode, 0, MAINSUBRXMODE_COUNT - 1);
	save_i8(RMT_MAINSUBRXMODE_BASE, mainsubrxmode);	// �����/������, A - main RX, B - sub RX
	updateboard(1, 0);
}

// ������� ��������� DUAL WATCH
const FLASHMEM char * hamradio_get_mainsubrxmode3_value_P(void)
{
	return mainsubrxmodes [mainsubrxmode].label;
}


#endif /* WITHUSEDUALWATCH */

///////////////////////////
// ����������� ������ ����������
//////////////////////////
/* ������������ ����
	 - �� �������� ���������� ��������� ��������� */
//static void 
//uif_key_changestep(uint_fast8_t tx)
//{
//while (repeat --)
//	lockmode = calc_next(lockmode, 0, 1);
//save_i8(RMT_LOCKMODE_BASE, lockmode);
//}

///////////////////////////
// ����������� ������ ����������
//////////////////////////
/* �������� ��� ������, ������� ������ �� ������.
*/
//static void 
//uif_key_dummy(void)
//{
//}

/* Template - Xxxx */

/*
static void 
uif_key_click_xxxx(void)
{

}
*/
///////////////////////////
// ���� ����������� ������ ����������
///////////////////////////

// ****************
// NMEA parser
// dummy function
#if WITHNMEA

enum nmea_states
{
	NMEAST_INITIALIZED,
	NMEAST_OPENED,	// ���������� ������ '$'
	NMEAST_CHSHI,	// ��� �������� �������� ����������� �����
	NMEAST_CHSLO,	// ���� �������� ������� ����������� �����


	//
	NMEAST_COUNTSTATES

};


typedef struct timeholder
{
	uint_fast8_t ms;
	uint_fast8_t secounds;
	uint_fast8_t minutes;
	uint_fast8_t hours;
	uint_fast8_t valid;
} timeholder_t;

// ��������� ����� �� ��������� �������
void time_next(
	timeholder_t * t
	)
{
	const uint_fast8_t a = 1;
	if ((t->secounds += a) >= 60)
	{
		t->secounds -= 60;
		if ((t->minutes += 1) >= 60)
		{
			t->minutes -= 60;
			if ((t->hours += 1) >= 24)
			{
				t->hours -= 24;
			}
		}
	}
}
// ��������� ����� �� ���������� �������
void time_prev(
	timeholder_t * t
	)
{
	const uint_fast8_t a = 1;
	if ((t->secounds -= a) >= 60)
	{
		t->secounds += 60;
		if ((t->minutes -= 1) >= 60)
		{
			t->minutes += 60;
			if ((t->hours -= 1) >= 24)
			{
				t->hours += 24;
			}
		}
	}
}

static uint_fast8_t nmea_state = NMEAST_INITIALIZED;
static uint_fast8_t nmea_checksum;
static uint_fast8_t nmea_chsval;
static uint_fast8_t nmea_param;		// ����� ������������ ��������� � ������
static uint_fast8_t nmea_chars;		// ���������� ��������, ���������� � �����

#define NMEA_PARAMS 20
#define NMEA_CHARS 12	// really need 11
static char nmea_buff [NMEA_PARAMS] [NMEA_CHARS];
static volatile timeholder_t nmea_time;
static timeholder_t th;
static volatile uint_fast8_t secoundticks;

static unsigned char hex2int(uint_fast8_t c)
{
	if (local_isdigit(c))
		return c - '0';
	if (isupper(c))
		return c - 'A' + 10;
	if (islower(c))
		return c - 'a' + 10;
	return 0;
}

void nmea_parsrchar(uint_fast8_t c)
{

	switch (nmea_state)
	{
	case NMEAST_INITIALIZED:
		if (c == '$')
		{
			nmea_checksum = '*';
			nmea_state = NMEAST_OPENED;
			nmea_param = 0;		// ����� ������������ ��������� � ������
			nmea_chars = 0;		// ���������� ��������, ���������� � �����
		}
		break;

	case NMEAST_OPENED:
		nmea_checksum ^= c;
		if (c == ',')
		{
			// ��������� ����� ���������, ��������� � ���������� ���������
			nmea_buff [nmea_param][nmea_chars] = '\0';
			nmea_param += 1;
			nmea_chars = 0;
		}
		else if (c == '*')
		{
			// ��������� ����� ���������, ��������� � ���������� ���������
			nmea_buff [nmea_param][nmea_chars] = '\0';
			nmea_param += 1;
			// ��������� � ����� ����������� �����
			nmea_state = NMEAST_CHSHI;
		}
		else if (nmea_param < NMEA_PARAMS && nmea_chars < (NMEA_CHARS - 1))
		{
			nmea_buff [nmea_param][nmea_chars] = c;
			nmea_chars += 1;
			//stat_l1 = stat_l1 > nmea_chars ? stat_l1 : nmea_chars;
		}
		else
			nmea_state = NMEAST_INITIALIZED;	// ��� ������� ������� ������
		break;

	case NMEAST_CHSHI:
		nmea_chsval = hex2int(c) * 16;
		nmea_state = NMEAST_CHSLO;
		break;

	case NMEAST_CHSLO:
		nmea_state = NMEAST_INITIALIZED;
		if (nmea_checksum == (nmea_chsval + hex2int(c)))
		{
			if (nmea_param > 2 && 
				strcmp(nmea_buff [0], "GPRMC") == 0 && 
				strcmp(nmea_buff [2], "A") == 0 &&
				1)
			{
				// ������ �������
				const char * const s = nmea_buff [1];	// ������ ������, ��� ����� ������ �������.������� 044709.00 ��� 044709.000
				nmea_time.hours = (s [0] - '0') * 10 + (s [1] - '0');
				nmea_time.minutes = (s [2] - '0') * 10 + (s [3] - '0');
				nmea_time.secounds = (s [4] - '0') * 10 + (s [5] - '0');
				nmea_time.ms = 0; //strtoul(s + 7, NULL, 10);
				nmea_time.valid = 1;
				time_next(& nmea_time);	// ����� ����� ���� ����� ��������� ��� ��������� � ��������� PPS
			}
		}
		break;
	}
}


#if WITHLFM
	static uint_fast16_t lfmtoffset = 0;
	static uint_fast16_t lfmtinterval = 30;
	static uint_fast8_t lfmmode = 1;
	static uint_fast16_t lfmstart100k = 80;
	static uint_fast16_t lfmstop100k = 180;
	static uint_fast16_t lfmspeed1k = 500;

// ������������ ���������
// lfmtoffset - ������� �� ������ ���� �� �������
// lfmtinterval - �������� � �������� ����� ��������� � �������� ����
// ������� ��-0 � ������ ����������� ������� ��� �������.
static uint_fast8_t 
islfmstart(unsigned now)
{
	unsigned s;
	for (s = lfmtoffset; s < 60 * 60; s += lfmtinterval)
	{
		if (s == now)
			return 1;
	}
	return 0;
}

#endif /* WITHLFM */

static timeholder_t th;
// ���������� ���������� ��� ������� ���������� �������� PPS
void 
RAMFUNC_NONILINE 
spool_nmeapps(void)
{
	th = nmea_time;
#if WITHLFM
	//lfm_run();
	//return;
	//memcpy(& th, & nmea_time, sizeof th);
	//th = nmea_time;
	//secoundticks = 0;	// ��������� ���������� ����� �������
	if (lfmmode != 0 && nmea_time.valid && islfmstart(nmea_time.minutes * 60 + nmea_time.secounds))
	{
		lfm_run();
	}
#endif /* WITHLFM */
}

/* ���������� �� ����������� ���������� */
// ��������� ������ ������� (��������) ��� ��������� ������ � CAT ��������
void nmea_rxoverflow(void)
{

}

// dummy function
/* ���������� �� ����������� ���������� */
void nmea_sendchar(void * ctx)
{

}
#endif /* WITHNMEA */

static volatile uint_fast8_t counterupdatedfreqs;
static volatile uint_fast8_t counterupdatedmodes;
static volatile uint_fast16_t counterupdatedvoltage; // ������� ��� ���������� �������� ���

static volatile uint_fast8_t counterupdatebars;
static volatile uint_fast8_t counterupdatewpm;

/* 
	������ ������� �� ���������� ���������� ������� ��� �������� ���������.
	���������� s-�����
	���������� ����������
	���������� ������� ������
	���������� �� ����������� ���������� ����������
*/
void 
display_spool(void)
{
	#if 0 //WITHNMEA
	// ������ ���������� �����/�����/������
	{
		enum { n = NTICKS(1000) };
		const uint_fast8_t t = secoundticks + 1;
		if (t >= n)
		{
			time_next(& th);
			secoundticks = 0;
		}
		else
		{
			secoundticks = t;
		}
	}
	#endif /* WITHNMEA */
	// ������ ���������� �������
	{
		const uint_fast8_t t = counterupdatedfreqs;
		if (t != 0)
			counterupdatedfreqs = t - 1;
	}
#if 0
	// ������ ���������� ������� ������
	{
		const uint_fast8_t t = counterupdatedmodes;
		if (t != 0)
			counterupdatedmodes = t - 1;
	}
#endif
	//������ ������� ��� ���������� ���������� ���
	{
		const uint_fast16_t t = counterupdatedvoltage;
		if (t != 0)
			counterupdatedvoltage = t - 1;

	}

	// ������ ���������� ���������� SWR ��� ��������
	{
		const uint_fast8_t t = counterupdatebars;
		if (t != 0)
			counterupdatebars = t - 1;
	}

	// ������ ������� ��� ���������� �������������� � ������ ����������
	{
		const uint_fast8_t t = counterupdatewpm;
		if (t != 0)
			counterupdatewpm = t - 1;
	}
}

// �������� ���������� ���������� �������� �������� � ���������.
// � ��� �� ������ �������������� - ���������, �������� ��
static uint_fast8_t
display_refreshenabled_wpm(void)
{
	return getstablev8(& counterupdatewpm) == 0;		/* ������ ����� �� ���� - ����� ���������. */
}

// ������������� ������������ ���������� �������� �������� � ���������.
// � ��� �� ������ �������������� - ���������, �������� ��
static void
display_refreshperformed_wpm(void)
{
	const uint_fast8_t n = NTICKS(100);	// 100 ms - ���������� � �������� 10 ����

	disableIRQ();
	counterupdatewpm = n;
	enableIRQ();
}

// �������� ���������� ���������� ������� (��������� �������).
static uint_fast8_t
display_refreshenabled_voltage(void)
{
	return getstablev16(& counterupdatedvoltage) == 0;		/* ������ ����� �� ���� - ����� ���������. */
}

// ������������� ������������ ���������� ������� (���������� �������).
static void
display_refreshperformed_voltage(void)
{
	const uint_fast16_t n = NTICKS(500);	/* 1/2 ������� */

	disableIRQ();
	counterupdatedvoltage = n;
	enableIRQ();
}


// �������� ���������� ���������� ������� (��������� �������).
static uint_fast8_t
display_refreshenabled_freqs(void)
{
	return getstablev8(& counterupdatedfreqs) == 0;		/* ������ ����� �� ���� - ����� ���������. */
}

// ������������� ������������ ���������� ������� (��������� �������).
static void
display_refreshperformed_freqs(void)
{
	const uint_fast8_t n = NTICKS(1000 / displayfreqsfps);	// 50 ms - ���������� � �������� 20 ����

	disableIRQ();
	counterupdatedfreqs = n;
	enableIRQ();
}

// �������� ���������� ���������� ������� (��������� �������, ����/��������).
static uint_fast8_t
display_refreshenabled_modes(void)
{
	return 1;
	//return getstablev8(& counterupdatedmodes) == 0;		/* ������ ����� �� ���� - ����� ���������. */
}

// ������������� ������������ ���������� ������� (��������� �������, ����/��������).
static void
display_refreshperformed_modes(void)
{
	return;	// TODO: ���� ���� ������ �� ��������
	const uint_fast8_t n = NTICKS(1000 / displaymodesfps);	// 50 ms - ���������� � �������� 20 ����

	disableIRQ();
	counterupdatedmodes = n;
	enableIRQ();
}


/* ����������� ������� (������) ��������� */
static void 
display_freqpair(void)
{
#if WITHDIRECTFREQENER

	if (editfreqmode)
	{
		display_dispfreq_a2(editfreq, blinkpos + 1, blinkstate, amenuset());
	}
	else
	{
		display_dispfreq_ab(amenuset());	/* ����������� ���� ����������� ������� */
	}

#else /* WITHDIRECTFREQENER */

	display_dispfreq_ab(amenuset());		/* ����������� ���� ����������� ������� */

#endif /* WITHDIRECTFREQENER */
}


// �������� ���������� ���������� ������� (��������� SWR/S-����).
static uint_fast8_t
display_refresenabled_bars(void)
{
	return getstablev8(& counterupdatebars) == 0;		/* ������ ����� �� ���� - ����� ���������. */
}
// ������������� ������������ ���������� ������� (��������� SWR/S-����).
static void
display_refreshperformed_bars(void)
{
	const uint_fast8_t n = NTICKS(1000 / displaybarsfps);	// 50 ms - ���������� � �������� 20 ����

	disableIRQ();
	counterupdatebars = n;
	enableIRQ();
}

void 
display2_adctest(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if defined (targetext2)
	const uint_fast16_t vref_mV = 3140;
	static const struct
	{
		uint_fast8_t adci;
		uint_fast8_t diff;
		uint_fast16_t mul10;
	} adcis [] =
	{
		// UA1CEI 100W PA board 2xRD100HHF1 
		// ADC inputs configuration
		//{	1,	1,	10, },	// DRAIN (negative from midpoint at CH1: ch0=in-, ch1=in+)
		{	0,	0,	10, },	// DRAIN (negative from midpoint at CH1)
		{	1,	0,	10, },	// reference (2.5 volt)
		{	2,	0,	10, },	// FORWARD
		{	3,	0,	10, },	// REFLECTED
		{	4,	0,	57.	},	// VDD 4.7k + 1k
	};

	uint_fast8_t row;
	for (row = 0; row < (sizeof adcis / sizeof adcis [0]); ++ row)
	{
		uint_fast16_t value;
		enum { WDTH = 12 };	// ������ ���� ��� �����������
		char b [WDTH + 1];

		value = mcp3208_read(targetext2, adcis [row].diff, adcis [row].adci) * (uint64_t) adcis [row].mul10 * vref_mV / 4095 / 10;

		local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("%*u"), WDTH, (unsigned) value);
		display_2states(x, y + GRID2Y(row), 1, b, b);
	}

#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */

#endif
}
// S-METER
/* ����������� S-����� �� ����� ��� �������� */
// ������� ���������� �� display2.c
void 
display2_bars_rx(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHBARS
	uint_fast8_t tracemax;
	uint_fast8_t v = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);
	display_smeter(x, y, v, tracemax, s9level, s9delta, s9_60_delta);

#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */
#endif /* WITHBARS */
}

// SWR-METER, POWER-METER
/* ����������� P-����� � SWR-����� �� ����� ��� �������� */
// ������� ���������� �� display2.c
void 
display2_bars_tx(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHBARS
#if WITHTX
	#if (WITHSWRMTR || WITHSHOWSWRPWR)
		uint_fast8_t pwrtrace;
		const uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);
		adcvalholder_t reflected;
		const adcvalholder_t forward = board_getswrmeter(& reflected, swrcalibr);
		#if WITHSHOWSWRPWR
			display_swrmeter(x, y, forward, reflected, minforward);
			display_pwrmeter(x, y, pwr, pwrtrace, maxpwrcali);
		#else
			if (swrmode || getactualtune())
				display_swrmeter(x, y, forward, reflected, minforward);
			else
				display_pwrmeter(x, y, pwr, pwrtrace, maxpwrcali);
		#endif
	#elif WITHPWRMTR
		uint_fast8_t pwrtrace;
		const uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);
		display_pwrmeter(x, y, pwr, pwrtrace, maxpwrcali);
	#endif

#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */

#endif /* WITHTX */
#endif /* WITHBARS */
}

// S-METER, SWR-METER, POWER-METER
/* ����������� S-����� ��� SWR-����� �� ����� ��� �������� */
// ������� ���������� �� display2.c
void 
display2_bars(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	if (userfsg)
	{
	}
	else if (gtx)
	{
		display2_bars_tx(x, y, pv);
	}
	else
	{
		display2_bars_rx(x, y, pv);
	}

}

#if CTLSTYLE_RA4YBO_AM0

// S-METER, SWR-METER, POWER-METER
/* ����������� S-����� ��� SWR-����� �� ����� ��� �������� */
// ���������� �� display2.c (������ ��� CTLSTYLE_RA4YBO_AM0)
void 
display2_bars_amv0(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
#if WITHBARS
	if (gtx)
	{
#if WITHTX
	#if (WITHSWRMTR || WITHSHOWSWRPWR)
		uint_fast8_t pwrtrace;
		const uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);
		const uint_fast8_t modulaton = board_getadc_filtered_u8(REF, 0, UINT8_MAX);
		display_modulationmeter_amv0(x, y, modulaton, UINT8_MAX);
		display_pwrmeter_amv0(x, y, pwr, pwrtrace, maxpwrcali);
	#elif WITHPWRMTR
		uint_fast8_t pwrtrace;
		const uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);
		display_pwrmeter_amv0(x, y, pwr, pwrtrace, maxpwrcali);
	#endif

#endif
	}
	else
	{
		uint_fast8_t tracemax;
		uint_fast8_t v = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);
		display_smeter_amv0(x, y, v, tracemax, s9level, s9delta, s9_60_delta);
	}
#endif /* WITHBARS */
}

#endif /* CTLSTYLE_RA4YBO_AM0 */

// --- display2.c stuff

/* ���������� ������������ ����� ����������� - S-����� ��� SWR-����� � volt-�����. */
static void 
//NOINLINEAT
display_redrawbars(
	uint_fast8_t immed,	// ����������� ����������� �����������
	uint_fast8_t extra		/* ��������� � ������ ����������� �������� */
	)
{
	if (immed || display_refresenabled_bars())
	{
		looptests();		// ������������ ���������� � ������� ����� - �����

		/* ��������� ���������, ����� ��� ���� ������� ����������� */
		/* ��������� ���������, ������������� ��� ������� ������ ����������� */
		display_barmeters_subset(amenuset(), extra);
		// ������������� ���������
		display_refreshperformed_bars();
	}

	if (immed || display_refreshenabled_voltage())
	{
		display_volts(amenuset(), extra);
		display_refreshperformed_voltage();
	}
}

// ��������� ����������� ������� �� �������
static void 
//NOINLINEAT
display_redrawfreqs(
	uint_fast8_t immed	// ����������� ����������� �����������
	)
{
	if (immed || display_refreshenabled_freqs())
	{
		display_freqpair();	/* ���������� ��������� ������� */
		display_refreshperformed_freqs();
	}
}

// ��������� ����������� ������� ������ �� �������
static void 
//NOINLINEAT
display_redrawmodes(
	uint_fast8_t immed	// ����������� ����������� �����������
	)
{
	if (immed || display_refreshenabled_modes())
	{
		/* ��������� ���������, ����� ��� ���� ������� ����������� */
		/* ��������� ���������, ������������� ��� ������� ������ ����������� */
		display_mode_subset(amenuset());
		// ������������� ���������
		display_refreshperformed_modes();
	}
}

// ���������� ������� - ��, ������� �������
static void
//NOINLINEAT
display_redrawfreqmodesbars(
	uint_fast8_t extra		/* ��������� � ������ ����������� �������� */
	)
{
	if (extra == 0)
	{
		display_redrawfreqs(1);	/* ����������� ���������� ��������� ������� */
		display_redrawmodes(1);
		display_redrawbars(1, extra);	/* ���������� ������������ ����� ����������� - ���������� S-����� ��� SWR-����� � volt-�����. */
	}
	else
	{
		display_redrawbars(1, extra);	/* ���������� ������������ ����� ����������� - ���������� S-����� ��� SWR-����� � volt-�����. */
	}
}


static void 
directctlupdate(uint_fast8_t inmenu)
{
	uint_fast8_t changed = 0;
	uint_fast8_t changedtx = 0;
	changedtx |= flagne_u8(& gtx, seq_get_txstate());	// ������� ��������� ����� ��� ��������

	if (display_refreshenabled_wpm())
	{
		// +++ ��������� ��������� ������� ���������� */
#if WITHCPUADCHW
	#if WITHPOTPOWER
		changed |= flagne_u8(& gnormalpower, board_getadc_filtered_u8(POTPOWER, WITHPOWERTRIMMIN, WITHPOWERTRIMMAX));	// ����������� ��������
	#endif /* WITHPOTPOWER */
	#if WITHPOTWPM
		changed |= flagne_u8(& elkeywpm, board_getadc_filtered_u8(POTWPM, CWWPMMIN, CWWPMMAX));
	#endif /* WITHPOTWPM */
	#if WITHPOTIFGAIN
		changed |= flagne_u16(& rfgain1, board_getadc_filtered_u16(POTIFGAIN, BOARD_IFGAIN_MIN, BOARD_IFGAIN_MAX));	// �������� ��� ����������� �������� ��
	#endif /* WITHPOTIFGAIN */
	#if WITHPOTAFGAIN
		changed |= flagne_u16(& afgain1, board_getadc_filtered_u16(POTAFGAIN, BOARD_AFGAIN_MIN, BOARD_AFGAIN_MAX));	// �������� ��� ����������� ������ �� ������ �����-���
	#endif /* WITHPOTAFGAIN */
	#if WITHPBT && WITHPOTPBT
		/* ��������� gpbtoffset PBTMIN, PBTMAX, midscale = PBTHALF */
		changed |= flagne_u16(& gpbtoffset, board_getadc_filtered_u16(POTPBT, PBTMIN, PBTMAX) / 10 * 10);
	#endif /* WITHPBT && WITHPOTPBT */
	#if WITHIFSHIFT && WITHPOTIFSHIFT
		/* ��������� gifshftoffset IFSHIFTTMIN, IFSHIFTMAX, midscale = IFSHIFTHALF */
		changed |= flagne_u16(& ifshifoffset, board_getadc_filtered_u16(POTIFSHIFT, IFSHIFTTMIN, IFSHIFTMAX) / 10 * 10);
	#endif /* WITHIFSHIFT && WITHPOTIFSHIFT */
	#if WITHPOTNOTCH && WITHNOTCHFREQ
		changed |= flagne_u16(& gnotchfreq, board_getadc_filtered_u16(POTNOTCH, WITHNOTCHFREQMIN, WITHNOTCHFREQMAX) / 50 * 50);	// ����������� ������� NOTCH �������
	#endif /* WITHPOTNOTCH && WITHNOTCHFREQ */
#endif /* WITHCPUADCHW */
	#if CTLSTYLE_RA4YBO_V3
		changed |= flagne_u8(& guser2, kbd_get_ishold(KIF_USER2));
		changed |= flagne_u8(& guser3, kbd_get_ishold(KIF_USER3));
	#endif /* CTLSTYLE_RA4YBO_V3 */
		// --- ����� ��������� ��������� ������� ���������� */
		// ������������, ��� ���������� ���������
		display_refreshperformed_wpm();
	}
	if (changedtx != 0)
	{
		updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
		seq_ask_txstate(gtx);
		display_redrawfreqmodesbars(inmenu);	// ���������� ������� - ��, ������� �������
	}
	else if (changed != 0)
		updateboard(1, 0);	/* ������ ������������� (��� ����� ����� ������) */
}

// *************************
// CAT sequence parser

// omnirig default settings: PTT control: RTS, CW control: DTR
// HXCat use same default settings


#if WITHCAT

static uint_fast8_t morsefill;	/* ������ ������, ������������ � ������ ������. ��������������� ���������. */

static uint_fast8_t inpmorselength [2];
static uint_fast8_t sendmorsepos [2];

#if WITHCATEXT && WITHELKEY
	static void cat_set_kyanswer(uint_fast8_t force);
	static uint_fast8_t cathasparamerror;
	static unsigned char morsestring [2][25];
#endif /* WITHCATEXT && WITHELKEY */

static uint_fast8_t catstatein = CATSTATE_HALTED;

static volatile uint_fast8_t catstateout = CATSTATEO_HALTED;
static volatile const char * catsendptr;
static volatile uint_fast8_t catsendcount;

//A communication error occurred, such as an overrun or framing error during a serial data transmission.
//static const char processingcmd [2] = "E;";	// ����� �� ������� ������� ��� �����������

static uint_fast8_t 
cat_getstateout(void)
{
	return getstablev8(& catstateout);
}

/* ���������� �� ����������� ���������� */
void cat2_sendchar(void * ctx)
{
	switch (catstateout)
	{
	case CATSTATEO_HALTED:
		//HARDWARE_CAT_ENABLETX(0);
		break;

	case CATSTATEO_WAITSENDREPLAY:
		if (catsendcount --)
			HARDWARE_CAT_TX(ctx, * catsendptr ++);
		else
		{
			HARDWARE_CAT_ENABLETX(0);
			catstateout = CATSTATEO_SENDREADY;
		}
		break;

	case CATSTATEO_SENDREADY:
		// ��������� �������� ����� USB DISCONNECT
		HARDWARE_CAT_ENABLETX(0);
		break;

	default:
		TP();
		// not need, but for safety.
		//HARDWARE_CAT_ENABLETX(0);
		break;
	}
}

// ����� �� user-mode ���������
void 
//NOINLINEAT
static cat_answervariable(const char * p, uint_fast8_t len)
{
	disableIRQ();
	if (catstateout != CATSTATEO_SENDREADY)
	{
		// ������ ��� ���������� ��������� - ����� ����������.
		// ��������� ��� ��������� ������� ��� ���������� CAT
		enableIRQ();
		return;
	}
	if ((catsendcount = len) != 0)
	{
		catsendptr = p;
		catstateout = CATSTATEO_WAITSENDREPLAY;
		HARDWARE_CAT_ENABLETX(1);
	}
	else
	{
		//catstateout = CATSTATEO_SENDREADY;
		HARDWARE_CAT_ENABLETX(0);
	}
	enableIRQ();
}


// *************************

//#define CAT_ASKBUFF_SIZE (43 + 28)
#define CAT_ASKBUFF_SIZE (43)

static char cat_ask_buffer [CAT_ASKBUFF_SIZE];

static void 
//NOINLINEAT
cat_answer(uint_fast8_t len)
{
	cat_answervariable(cat_ask_buffer, len);
}

#if WITHELKEY

/* ������������ �� ��������� �����, ���� ����� */
static void
morseswitchnext(void)
{
	const uint_fast8_t ms = ! morsefill;	// ms: morse sent
	if (sendmorsepos [ms] == inpmorselength [ms])
	{
		// ��������� ����������
		if (sendmorsepos [morsefill] < inpmorselength [morsefill])
		{
			// �������� ������� �����
			sendmorsepos [ms] = inpmorselength [ms] = 0;
			morsefill = ! morsefill;
		}
	}

}

#endif /* WITHELKEY */

static uint_fast8_t 
//NOINLINEAT
ascii_toupper(uint_fast8_t c)
{
	return toupper((unsigned char) c);
}

#if WITHCATEXT && WITHELKEY

/* todo: ���������� �� ���������� ��������� KY � ������, ���� KY ��������. */
static void 
//NOINLINEAT
cat_set_kyanswer(uint_fast8_t force)
{
	// �������� ��� ��� ������ ���������
	//const uint_fast8_t f = (sendmorsepos [morsefill] != inpmorselength [morsefill]);
	const uint_fast8_t f = sendmorsepos [morsefill] < inpmorselength [morsefill];
	// ��� ���� ������� KY; (��� ����������) - ������ ���������
	// KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
	if (force)
		cat_answer_map [CAT_KY_INDEX] = 1;
	cat_answerparam_map [CAT_KY_INDEX] = '0' + f;
}

/* ���������� �� ����������� ���������� */
/* �������� ��������� ������ ��� �������� (������ ������� �������) */
static char cat_getnextcw(void)
{
	morseswitchnext();	/* ������������ �� ��������� �����, ���� ����� */
	cat_set_kyanswer(0);	// KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
	const uint_fast8_t ms = ! morsefill;	// ms: morse sent
	while (sendmorsepos [ms] < inpmorselength [ms])
	{
		const char c = morsestring [ms] [sendmorsepos [ms] ++];
		if (c == ' ' && (sendmorsepos [ms] < inpmorselength [ms]) && morsestring [ms] [sendmorsepos [ms]] == ' ')
			continue;
		return c;
	}
	return '\0';
}

#endif /* WITHCATEXT && WITHELKEY */

/* ���������� �� ����������� ���������� */
// ��������� ������ ������� (��������) ��� ��������� ������ � CAT ��������
void cat2_rxoverflow(void)
{

}

/* ���������� �� ����������� ���������� */
// ��������� ������ ����� ��� ������ �� USB CDC
void cat2_disconnect(void)
{
	uint_fast8_t i;
	for (i = 0; i < (sizeof cat_answer_map / sizeof cat_answer_map [0]); ++ i)
		cat_answer_map [i] = 0;

#if WITHTX
	sendmorsepos  [0] = 
	inpmorselength [0]= 
	sendmorsepos  [1] = 
	inpmorselength [1] = 0;
#endif /* WITHTX */

	//aistate = 0; /* Power-up state of AI mode = 0 (TS-590). */
	cattunemode = catstatetx = 0;
	catstatein = CATSTATE_WAITCOMMAND1;
	catstateout = CATSTATEO_SENDREADY;
}


/* ���������� �� ����������� ���������� */
void cat2_parsechar(uint_fast8_t c)
{
	static uint_fast8_t catcommand1;
	static uint_fast8_t catcommand2;
	static uint_fast8_t cathasparam;
	static uint_fast8_t catp [CATPCOUNTSIZE];
	static uint_fast8_t catpcount;

   // debug_printf_P(PSTR("c=%02x, catstatein=%d, c1=%02X, c2=%02X\n"), c, catstatein, catcommand1, catcommand2);
	switch (catstatein)
	{
	case CATSTATE_HALTED:
		break;

	case CATSTATE_WAITCOMMAND1:
		if ((catcommand1 = ascii_toupper(c)) != ';')
			catstatein = CATSTATE_WAITCOMMAND2;
		break;
	
	case CATSTATE_WAITCOMMAND2:
		if ((catcommand2 = ascii_toupper(c)) == ';')
		{
			cat_answer_map [CAT_BADCOMMAND_INDEX] = 1;	// ������ ������ �� �����, � ';' - ��������������� ����� �������
			catstatein = CATSTATE_WAITCOMMAND1;
		}
#if WITHCATEXT && WITHELKEY
		else if (catcommand1 == 'K' && catcommand2 == 'Y')
		{
			catstatein = CATSTATE_WAITMORSE;
			cathasparam = 0;	// ������ ���� - 1, ����� - 0
			catpcount = 0;
			cathasparamerror = 0;
		}
#endif	/* WITHCATEXT && WITHELKEY */
		else
		{
			catstatein = CATSTATE_WAITPARAM;
			cathasparam = 0;
			//cathasparamerror = 0;
			catpcount = 0;
		}
		break;

	case CATSTATE_WAITPARAM:
		//if (c >= '0' && c <= '9')
		if (local_isdigit((unsigned char) c))
		{
			cathasparam = 1;
			if (catpcount < (sizeof catp / sizeof catp [0]))
				catp [catpcount ++] = c;
		}
		else if (c == ';')
		{
			//catstatein = CATSTATE_READYCOMMAND;		// ������� ������ ��� ������������� ����������

			uint8_t * buff;
			if (takemsgbufferfree_low(& buff) != 0)
			{
				uint_fast8_t i;
				// check MSGBUFFERSIZE8 valie
				buff [0] = catcommand1;
				buff [1] = catcommand2;
				buff [2] = cathasparam;
				
				buff [8] = catpcount;
				for (i = 0; i < catpcount; ++ i)
					buff [9 + i] = catp [i];

				placesemsgbuffer_low(MSGT_CAT, buff);
			}
			catstatein = CATSTATE_WAITCOMMAND1;	/* � user-mode ������ ������ - ����� �� ���������  */
		}
		else
		{
			;// ��������� ������� ������������ //
		}
		break;

#if WITHCATEXT && WITHELKEY
	case CATSTATE_WAITMORSE:
		if (c == '\0')	// ����� ������ ����������
		{
			cathasparamerror = 1;
		}
		else if (c != ';')
		{
			cathasparam = 1;	// ������� ������� �������� ��� ��������
			if (catpcount == 0 && c == '0')
			{
				sendmorsepos [0] = inpmorselength [0] = 0;	// �������� ����� �������� �����.
				sendmorsepos [1] = inpmorselength [1] = 0;	// �������� ����� �������� �����.
				cat_set_kyanswer(0);
			}
			
			if (sendmorsepos [morsefill] < inpmorselength [morsefill])
			{
				// ������ ��� �� �������� - �� ������ �����. ����� �������� ������.
				cathasparamerror = 1;
			}
			else if (catpcount < (sizeof morsestring [morsefill] / sizeof morsestring [morsefill][0]))
			{
				/* ���������� ��������� ������ ��� �������� */
				morsestring [morsefill] [catpcount ++] = ascii_toupper(c);
			}
			else
			{
				// ������� ������� ������ ��� ��������
				cathasparamerror = 1;
			}
		}
		else
		{
			// ������ ������ ���������� �������.
			// ���� �� ������?
			if (cathasparamerror != 0)
			{
				// ���� ������� �� ����� ������� KY, �� ��� ������ ������� ��-�� ������.
				// ������ - ��� ��������������� ����� �������� ������.
				// ��� �� ���������� ������ ������ ��� ��������
				cat_answer_map [CAT_BADCOMMAND_INDEX] = 1;
			}
			else if (cathasparam != 0)
			{
				/* ����� ������ ������� (������� ������� � �����) - ������ ������������ */
				if (catpcount > 1)
				{
					/* ������������� ������� ��� �������� */
					inpmorselength  [morsefill] = catpcount;
					sendmorsepos  [morsefill] = 1;	/* ������ ������ � ������ - ������ */

					morseswitchnext();	/* ������������ �� ��������� �����, ���� ����� */
					cat_set_kyanswer(0);	// KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
				}
			}
			else
			{
				cat_set_kyanswer(1);	// KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
			}
			catstatein = CATSTATE_WAITCOMMAND1;	/* � user-mode ������ ������ - ����� �� ���������  */
		}
		break;
#endif /* WITHCATEXT && WITHELKEY */
	}
}

static void idanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_1 [] =
		"ID"			// 2 characters - status information code
		"%03d"		// P1 3 characters - model
		";";				// 1 char - line terminator
	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) 21	// 021 - code of TS-590S model
		);
	cat_answer(len);
}

static void fvanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_0 [] =
		"FV"			// 2 characters - status information code
		"1.00"			// P1 4 characters - For example, for firmware version ..00, it reads �FV..00;.
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_0
		);
	cat_answer(len);
}

static void daanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_1 [] =
		"DA"			// 2 characters - status information code
		"%1d"			// P1 1 characters -  0: DATA mode OFF, 1: DATA mode ON, 
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) 0	// 0/1: data mode on/off
		);
	cat_answer(len);
}

static void faanswer(uint_fast8_t arg)
{
	const uint_fast8_t bi = getbankindex_ab(0);	/* vfo A bank index */
	static const FLASHMEM char fmt_1 [] =
		"FA"				// 2 characters - status information code
		"%011ld"			// P1 11 characters - freq
		";";				// 1 char - line terminator
	// answer VFO A frequency
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(signed long) gfreqs [bi]
		);
	cat_answer(len);
}

static void fbanswer(uint_fast8_t arg)
{
	const uint_fast8_t bi = getbankindex_ab(1);	/* vfo B bank index */
	static const FLASHMEM char fmt_1 [] =
		"FB"				// 2 characters - status information code
		"%011ld"			// P1 11 characters - freq
		";";				// 1 char - line terminator

	// answer VFO B frequency
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(signed long) gfreqs [bi]
		);
	cat_answer(len);
}

static void spanswer(uint_fast8_t arg)
{
	//const uint_fast8_t bi = getbankindex_ab(1);	/* vfo B bank index */
	static const FLASHMEM char fmt_1 [] =
		"SP"				// 2 characters - status information code
		"%01d"			// P1 1 characters - 0: VFO A, 1: VFO B, 2: Memory Channel
		";";				// 1 char - line terminator

	// answer VFO B frequency
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) (gsplitmode != VFOMODES_VFOINIT)
		);
	cat_answer(len);
}



static void franswer(uint_fast8_t arg)
{
	//const uint_fast8_t bi = getbankindex_ab(1);	/* vfo B bank index */
	static const FLASHMEM char fmt_1 [] =
		"FR"				// 2 characters - status information code
		"%01d"			// P1 1 characters - 0: VFO A, 1: VFO B, 2: Memory Channel
		";";				// 1 char - line terminator

	// answer VFO B frequency
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) gvfosplit [0]	// At index 0: RX VFO A or B, at index 1: TX VFO A or B
		);
	cat_answer(len);
}

static void ftanswer(uint_fast8_t arg)
{
	//const uint_fast8_t bi = getbankindex_ab(1);	/* vfo B bank index */
	static const FLASHMEM char fmt_1 [] =
		"FT"				// 2 characters - status information code
		"%01d"			// P1 1 characters - 0: VFO A, 1: VFO B, 2: Memory Channel
		";";				// 1 char - line terminator

	// answer VFO B frequency
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) gvfosplit [1]	// At index 0: RX VFO A or B, at index 1: TX VFO A or B
		);
	cat_answer(len);
}

static void mdanswer(uint_fast8_t arg)
{
	const uint_fast8_t bi = getbankindex_ab(0);	/* vfo B bank index */
	static const FLASHMEM char fmt_1 [] =
		"MD"				// 2 characters - status information code
		"%1d"				// 1 char - mode
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) submodes [getsubmode(bi)].qcatmodecode
		);
	cat_answer(len);
}

static void fwanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_1 [] =
		"FW"			// 2 characters - status information code
		"%04d"			// P42 4 characters - DSP filtering bandwidth.
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) getkenwoodfw(gsubmode, gfi) // ������ ����������� � ������ ��� ��� ������ �����������
		);
	cat_answer(len);
}

#if WITHIF4DSP//CTLSTYLE_V1D || CTLSTYLE_OLEG4Z_V1 || 1
static void zzanswer(uint_fast8_t arg)
{
	//ZZmLLLLUUUUSSSS
	static const FLASHMEM char fmt_4 [] =
		"ZZ"			// 2 characters - status information code
		"%1d"			// 1 char - mode
		"%04d"			// right
		"%04d"			// left
		"%03d"			// slope
		";";			// 1 char - line terminator

	const uint_fast8_t submode = findkenwoodsubmode(arg, SUBMODE_USB);	/* ����� �� ������������� ������ */
	const FLASHMEM struct modetempl * const pmodet = getmodetempl(submode);
	const uint_fast8_t bwseti = pmodet->bwseti;
	const uint_fast8_t pos = bwsetpos [bwseti];
	const bwprop_t * const p = bwsetsc [bwseti].prop [pos];

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_4,
		(int) arg,
		(int) p->left10_width10,
		(int) p->right100,
		(int) p->afresponce
		);
	cat_answer(len);
}
#endif /* CTLSTYLE_V1D || CTLSTYLE_OLEG4Z_V1 */

#if WITHIF4DSP

static void sqanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_2 [] =
		"SQ"			// 2 characters - status information code
		"%1d"			// P1 always 0
		"%03d"			// P2 0..255 Squelch level
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_2,
		(int) arg,
		(int) gsquelch
		);
	cat_answer(len);
}
#endif /* WITHIF4DSP */


#if WITHCATEXT && WITHELKEY

static void kyanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_1 [] =
		"KY"			// 2 characters - status information code
		"%c"			// P44 1 character - KEYER BUFFER STATUS: 0 - BUFFER SPACE AVALIABLE, 1 - BUFFER FULL
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(char) arg
		);
	cat_answer(len);
}

static void ksanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_1 [] =
		"KS"			// 2 characters - status information code
		"%03d"			// P42 3 characters - keyer speed in WPM
		";";				// 1 char - line terminator

	// keyer speed information
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) elkeywpm
		);
	cat_answer(len);
}
#endif /* WITHCATEXT && WITHELKEY */

static void txanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_1 [] =
		"TX"			// 2 characters - status information code
		"%01d"			// P1 1 character - TX reason
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) (getactualtune() ? 2 : 0)
		);
	cat_answer(len);
}

static void rxanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_0 [] =
		"RX"			// 2 characters - status information code
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_0
		);
	cat_answer(len);
}

static void aianswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_1 [] =
		"AI"			// 2 characters - status information code
		"%1d"			// 1 char - mode
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) (aistate ? 2 : 0)
		);
	cat_answer(len);
}


#if WITHCATEXT

static void psanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_1 [] =
		"PS"			// 2 characters - status information code
		"%01d"			// P1 1 characters - POWER ON/OFF
		";";			// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) 1
		);
	cat_answer(len);
}

// used s9level, s9delta, s9_60_delta values for calculate s-meter points
static uint_fast8_t 
scaletopointssmeter(
	uint_fast8_t v,
	uint_fast8_t minval, uint_fast8_t maxval
	)
{
#if WITHBARS
	const int s0level = s9level - s9delta;
	const int s9_60level = s9level + s9_60_delta;

	if (v < s0level)
		v = s0level;
	if (v > s9_60level)
		v = s9_60level;
	
	return ((v - s0level) * 30) / (s9delta + s9_60_delta);
#else
	return 0;
#endif
}

static void smanswer(uint_fast8_t arg)
{
	// s-meter state answer
	// ��� SM9 ������ ��� ��������� "������" ������.
	static const FLASHMEM char fmt0_1 [] =
		"SM"			// 2 characters - status information code
		"0"				// 1 char - Always 0
		"%04d"				// 4 chars - s-meter points (0000..0030)
		";";				// 1 char - line terminator
	// s-meter state answer
	// ��� SM9 ������ ��� ��������� "������" ������.
	static const FLASHMEM char fmt9_1 [] =
		"SM"			// 2 characters - status information code
		"9"				// 1 char - Always 9
		"%+d"				// level in dBm
		";";				// 1 char - line terminator

	uint_fast8_t tracemax;
	uint_fast8_t v = board_getsmeter(& tracemax, 0, UINT8_MAX, arg == 9);

	switch (arg)
	{
	default:
	case 0:
		{
			// answer mode
			const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt0_1,
				(int) scaletopointssmeter(v, 0, UINT8_MAX)
				);
			cat_answer(len);
		}
		break;

	case 9:
		{
			// answer mode
			int level = ((int) v - (int) UINT8_MAX);
			if (level < - 170)
				level = - 170;
			else if (level > 20)
				level = 20;
			const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt9_1,
				(int) level
				);
			cat_answer(len);
		}
		break;
	}

}

static void raanswer(uint_fast8_t arg)
{
	// RX attenuator
	static const FLASHMEM char fmt_1 [] =
		"RA"				// 2 characters - status information code
		"%02d"				// 2 chars - 00: ATT OFF 01: ATT ON
		"00"				// 2 chars - 00: Always 00
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) gatts [getbankindex_tx(0)]

		);
	cat_answer(len);
}

static void paanswer(uint_fast8_t arg)
{
#if ! WITHONEATTONEAMP
	uint_fast8_t v = (gpamps [getbankindex_tx(gtx)] != 0);
#else /* ! WITHONEATTONEAMP */
	enum { v = 0 };
#endif /* ! WITHONEATTONEAMP */

	// RX preamplifier
	static const FLASHMEM char fmt_1 [] =
		"PA"				// 2 characters - status information code
		"%01d"				// 1 chars - 0: Pre-amp OFF, 1: : Pre-amp ON
		"0"					// 1 chars - Always 0
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) v

		);
	cat_answer(len);
}

#if WITHANTSELECT

static void ananswer(uint_fast8_t arg)
{
	//const uint_fast8_t pathi = 0;	// A or B path 
	// Antenna information answer
	static const FLASHMEM char fmt_1 [] =
		"AN"				// 2 characters - status information code
		"%01d"				// 1 chars - 0: ant 1, 1: : ant 2
		"0"					// 1 chars - 0: RX ANT is not used
		"1"					// 1 chars - 1: : Drive Out ON
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) ((gantennas [getbankindex_tx(0)] != 0) + 1)

		);
	cat_answer(len);
}

#endif /* WITHANTSELECT */

#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)

static uint_fast8_t kenwoodswrmeter(void)
{
	//const uint_fast8_t pathi = 0;	// A or B path 

	//enum { FS = SWRMIN * 15 / 10 };	// swr=1.0..4.0
	adcvalholder_t r;
	const adcvalholder_t f = board_getswrmeter(& r, swrcalibr);
	//const uint_fast16_t fullscale = FS - SWRMIN;
	uint_fast16_t swr10;		// ������������  ��������
	if (f < minforward)
		swr10 = 0;	// SWR=1
	else if (f <= r)
		swr10 = 30;		// SWR is infinite
	else
		swr10 = (f + r) * SWRMIN / (f - r) - SWRMIN;
	// v = 10..40 for swr 1..4
	// swr10 = 0..30 for swr 1..4
	return swr10;	// tested with ARCP950. 0: SWR=1.0, 5: SWR=1.3, 10: SWR=1.8, 15: SWR=3.0
}

// SWR
static void rm1answer(uint_fast8_t arg)
{
	//const uint_fast8_t pathi = 0;	// A or B path 
	// SWR-meter report
	static const FLASHMEM char fmt_1 [] =
		"RM"			// 2 characters - status information code
		"1"				// 1 char - 1 - SWR
		"%04d"				// 4 chars - 0000 ~ 0030: Meter value in dots
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) kenwoodswrmeter()
		);
	cat_answer(len);
}

// COMP
static void rm2answer(uint_fast8_t arg)
{
	//const uint_fast8_t pathi = 0;	// A or B path 
	// COMP report
	static const FLASHMEM char fmt_1 [] =
		"RM"			// 2 characters - status information code
		"2"				// 1 char - 2 - COMP
		"%04d"				// 4 chars - 0000 ~ 0030: Meter value in dots
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) 0
		);
	cat_answer(len);
}

// ALC
static void rm3answer(uint_fast8_t arg)
{
	//const uint_fast8_t pathi = 0;	// A or B path 
	// COMP report
	static const FLASHMEM char fmt_1 [] =
		"RM"			// 2 characters - status information code
		"3"				// 1 char - 3 - ALC
		"%04d"				// 4 chars - 0000 ~ 0030: Meter value in dots
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) 0
		);
	cat_answer(len);
}

#endif /* WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */
#endif /* WITHCATEXT */

static void ptanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_1 [] =
		"PT"			// 2 characters - status information code
		"%02d"			// P1 2 characters - pitch code
		";";				// 1 char - line terminator

	// answer mode
	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_1,
		(int) ((gcwpitch10 - 40) / 5)
		);

	cat_answer(len);
}
// information request
// Reads status of the transceiver.
static void ifanswer(uint_fast8_t arg)
{
	static const FLASHMEM char fmt_5 [] =
		"IF"			// 2 characters - status information code
		"%011ld"		// P1 11 characters - freq
		"     "			// P2 5 chars - unused
		"+0000"			// P3 5 chars - rit/xit freq
		"0"				// P4 1 char - rit off/on
		"0"				// P5 1 char - xit off/on
		" "				// P6 1 char - not used (hundreds of channel number)
		"00"			// P7 2 char - memory/channel
		"%1d"			// P8 1 char - tx/rx
		"%1d"			// P9 1 char - mode
		"%1d"			// P10 1 char - FR/FT function
		"0"				// P11 1 char - scan off/on
		"%1d"			// P12 1 char - split off/on
		"0"				// P13 1 char - tone off/on
		"00"			// P14 2 char - tone number
		"0"				// 1 char - unused
		";";				// 1 char - line terminator

	const uint_fast8_t len = local_snprintf_P(cat_ask_buffer, CAT_ASKBUFF_SIZE, fmt_5,
		(signed long) gfreqs [getbankindex_tx(gtx)],
		(int) gtx,
		(int) submodes [gsubmode].qcatmodecode,	// P9
		(int) 0,	// P10: FR/FT state
		(int) (gsplitmode == VFOMODES_VFOSPLIT) // P12
		);
	cat_answer(len);
}

static void badcommandanswer(uint_fast8_t arg)
{
	static const char badcommand [2] = "?;";	// ����� �� ������� � ������������ �����������

	cat_answervariable(badcommand, 2);	// ����� ������ - ��� �����.
}

#if WITHTX

// ���������� �� user mode ���������
// ���������� ������ �� �������� �� ������ CAT
static void 
cat_reset_ptt(void)	
{
	disableIRQ();
	cattunemode = catstatetx = 0;
	enableIRQ();
}


// ���������� �� ���������������� ���������
// �������� ������ �������� �� �������� �� ������ CAT ��� ����� ����������.
static uint_fast8_t 
cat_get_ptt(void)	
{
	if (catprocenable != 0)
	{
		const uint_fast8_t dtr = HARDWARE_CAT_GET_DTR();
		const uint_fast8_t rts = HARDWARE_CAT_GET_RTS();
		const uint_fast8_t r = (catrtsenable != 0) && (catdtrptt ? dtr : rts);	// ���� ����������
		return r || (catstatetx != 0);	// catstatetx - ��� �� ��������� ��������
	}
	return 0;
}

#endif /* WITHTX */


// ���������� �� ����������� �����������
// �������� ������ ����� �� ����� ����������, ���������� �� ����������� �����������
uint_fast8_t cat_get_keydown(void)
{
#if WITHELKEY
	if (catprocenable != 0)
	{
		const uint_fast8_t dtr = HARDWARE_CAT_GET_DTR();
		const uint_fast8_t rts = HARDWARE_CAT_GET_RTS();
		const uint_fast8_t r = (catdtrenable != 0) && (! catdtrptt ? dtr : rts);	// ���� ����������
		return r;
	}
#endif /* WITHELKEY */
	return 0;

}


static void 
NOINLINEAT
cat_answer_request(uint_fast8_t catindex)
{
	//debug_printf_P(PSTR("cat_answer_request: catindex=%u\n"), catindex);
	cat_answer_map [catindex] = 1;
}

static void 
processcat_initialize(void)
{
	//catprocenable = catstatetxdata = catstatetx = 0;
	//catstatein = CATSTATE_HALTED;
	//catstateout = CATSTATEO_HALTED;
	//morsefill = 0;	/* ������ ������, ������������ � ������ ������. ��������������� ���������. */
	//sendmorsepos [0] = sendmorsepos [1] = inpmorselength [0] = inpmorselength [1] = 0;
}

/* ��� �������� ���������� ������������ - �� ������ ������ ������ ��� ��� ����������� CAT */
static void processcat_enable(uint_fast8_t enable)
{
	if (catprocenable == enable)
		return;
	catprocenable = enable;
	if (! catprocenable)
	{
		disableIRQ();
		HARDWARE_CAT_ENABLERX(0);
		HARDWARE_CAT_ENABLETX(0);
		catstatein = CATSTATE_HALTED;
		catstateout = CATSTATEO_HALTED;
		enableIRQ();
	}
	else
	{
		uint_fast8_t i;
		for (i = 0; i < (sizeof cat_answer_map / sizeof cat_answer_map [0]); ++ i)
			cat_answer_map [i] = 0;

#if WITHTX
		sendmorsepos  [0] = 
		inpmorselength [0]= 
		sendmorsepos  [1] = 
		inpmorselength [1] = 0;
#endif /* WITHTX */

		aistate = 0; /* Power-up state of AI mode = 0 (TS-590). */
		disableIRQ();
		catstatetxdata = 0;
		cattunemode = catstatetx = 0;
		HARDWARE_CAT_ENABLERX(1);
		catstatein = CATSTATE_WAITCOMMAND1;
		catstateout = CATSTATEO_SENDREADY;
		enableIRQ();
	}
}

/* ���������� �� USER-MODE ��������� ��� �������� ������� �� CAT-������� */

typedef void (* canapfn)(uint_fast8_t arg);

static canapfn catanswers [CAT_MAX_INDEX] =
{
#if WITHCATEXT
#if WITHELKEY
	kyanswer,	// ����������� ������ ��� ��������� �������,
	ksanswer,
#endif /* WITHELKEY */
	psanswer,
	smanswer,
	raanswer,
	paanswer,
#if WITHANTSELECT
	ananswer,
#endif /* WITHANTSELECT */
#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)
	rm1answer,
	rm2answer,
	rm3answer,
#endif /* WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */
#endif /* WITHCATEXT */
	idanswer,				
	fvanswer,
	daanswer,
	faanswer,
	fbanswer,
	spanswer,
	franswer,
	ftanswer,
	mdanswer,
	txanswer,
	rxanswer,
	aianswer,
	ptanswer,
	ifanswer,
	fwanswer,
#if WITHIF4DSP//CTLSTYLE_V1D || CTLSTYLE_OLEG4Z_V1 || 1
	zzanswer,
#endif /* CTLSTYLE_V1D || CTLSTYLE_OLEG4Z_V1 */
#if WITHIF4DSP
	sqanswer,
#endif /* CTLSTYLE_V1D || CTLSTYLE_OLEG4Z_V1 */
	badcommandanswer,
};

static void 
cat_answer_forming(void)
{
	uint_fast8_t i;

	for (i = 0; i < (sizeof cat_answer_map / sizeof cat_answer_map [0]); ++ i)
	{
		disableIRQ();
		if (cat_answer_map [i] != 0)
		{
			const uint_fast8_t answerparam = cat_answerparam_map [i];
			cat_answer_map [i] = 0;
			enableIRQ();
			(* catanswers [i])(answerparam);
			return;
		}
		enableIRQ();
	}
}

static uint_fast32_t 
catscanint(
	const uint8_t * p,	// ������ ��������
	uint_fast8_t width	// ���������� ��������
	)
{
	uint_fast32_t v = 0;
	while (width -- && local_isdigit((unsigned char) * p))
	{
		v = v * 10 + * p ++ - '0';
	}
	return v;
}

/* ������� ������ - ���� �����-���� �������
	��������� ���������� ������� */
static uint_fast8_t
//NOINLINEAT
processcatmsg(
	uint_fast8_t catcommand1,
	uint_fast8_t catcommand2,
	uint_fast8_t cathasparam,
	uint_fast8_t catpcount,	// ���������� �������� �� ����� �������
	const uint8_t * catp	// ������ ��������
	)
{
	//debug_printf_P(PSTR("processcatmsg: c1=%02X, c2=%02X, chp=%d, cp=%lu\n"), catcommand1, catcommand2, cathasparam, catparam);
	#define match2(ch1, ch2) (catcommand1 == (ch1) && catcommand2 == (ch2))
	uint_fast8_t rc = 0;
	const uint_fast32_t catparam = catscanint(catp, catpcount);
	
#if WITHCAT
	if (match2('I', 'D'))
	{
		if (cathasparam != 0)
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
		else
		{
			cat_answer_request(CAT_ID_INDEX);
		}
	}
	else if (match2('F', 'V'))
	{
		if (cathasparam != 0)
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
		else
		{
			cat_answer_request(CAT_FV_INDEX);
		}
	}
	else if (match2('D', 'A'))
	{
		if (cathasparam != 0)
		{
			switch (catparam)
			{
			case 0:
				// DATA mode OFF
				break;
			case 1:
				// DATA mode ON
				break;
			default:
				cat_answer_request(CAT_BADCOMMAND_INDEX);
				break;
			};
		}
		else
		{
			cat_answer_request(CAT_DA_INDEX);
		}
	}
	// parse receieved command
	//
	else if (match2('A', 'I'))
	{
		if (cathasparam != 0)
		{
			switch (catparam)
			{
			case 0:
				aistate = 0;
				break;
			case 2:
				aistate = 1;
				cat_answer_request(CAT_IF_INDEX);	/* ��������� ������� ����� �������� */
				cat_answer_request(CAT_MD_INDEX);	/* ��������� ������� ����� �������� */
				break;
			default:
				cat_answer_request(CAT_BADCOMMAND_INDEX);
				break;
			}
		}
		else
		{
			cat_answer_request(CAT_AI_INDEX);
		}
	}
	else if (match2('F', 'A'))
	{
		if (cathasparam != 0)
		{
			const uint_fast8_t bi = getbankindex_ab(0);	/* VFO A bank index */
			const uint_fast32_t v = catparam;
			catchangefreq(vfy32up(v, TUNE_BOTTOM, TUNE_TOP - 1, gfreqs [bi]), gtx);
			updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_FA_INDEX);
		}
	}
	else if (match2('F', 'B'))
	{
		if (cathasparam != 0)
		{
			const uint_fast8_t bi = getbankindex_ab(1);	/* VFO B bank index */
			const uint_fast32_t v = catparam;
			catchangefreq(vfy32up(v, TUNE_BOTTOM, TUNE_TOP - 1, gfreqs [bi]), gtx);
			updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_FB_INDEX);
		}
	}
	else if (match2('P', 'T'))
	{
		if (cathasparam != 0)
		{
			const uint_fast32_t v = catparam;
			// ����������� ��� ��������� - 400 ����.
			gcwpitch10 = vfy32up(v, 0, 14, 6) * 5 + 40; 
			updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_PT_INDEX);
		}
	}
	else if (match2('M', 'D'))
	{
		if (cathasparam != 0)
		{
			const uint_fast32_t v = catparam;

			const uint_fast8_t bi = getbankindex_tx(gtx);	/* vfo bank index */
			const uint_fast8_t defsubmode = findkenwoodsubmode(v, gsubmode);	/* ����� �� ������������� ������ */
			//defsubmode = getdefaultbandsubmode(gfreqs [bi]);		/* ����� ��-��������� ��� ������� - USB ��� LSB */
			// todo: �� ����� ������, ���� locatesubmode �� ������� ������, ��� �������� row.
			const uint_fast8_t defcol = locatesubmode(defsubmode, & gmoderows [bi]);	/* ������/������� ��� SSB. ��� ������, ���� �� �����? */
			putmodecol(gmoderows [bi], defcol, bi);	/* ������ ����� �������� � ������� ����� */
			updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */

			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_MD_INDEX);
		}
	}
	else if (match2('I', 'F'))
	{
		if (cathasparam)
		{
		}
		else
		{
			cat_answer_request(CAT_IF_INDEX);
		}
	}
#if WITHSPLITEX
	else if (match2('S', 'P'))
	{
		if (cathasparam != 0)
		{
			if (catpcount == 1)
			{
				catchangesplit(catp [0] == '1', 0);
			}
			else if (catpcount == 3)
			{
				const int_fast8_t sign = 0 - (catp [1] == '1');	// P2: 0: plus direction
				catchangesplit(catp [0] == '1', sign * (catp [2] - '0'));
			}
			else
			{
				cat_answer_request(CAT_BADCOMMAND_INDEX);
			}
			//const uint_fast8_t bi = getbankindex_ab(1);	/* VFO B bank index */
			//const uint_fast32_t v = catparam;
			//catchangefreq(vfy32up(v, TUNE_BOTTOM, TUNE_TOP - 1, gfreqs [bi]), gtx);
			//updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_SP_INDEX);	// spanswer()
		}
	}
	else if (match2('F', 'R'))
	{
		if (cathasparam != 0)
		{
			// gvfosplit: At index 0: RX VFO A or B, at index 1: TX VFO A or B
			switch (catparam)
			{
			case 0:
				// Set VFO A to simplex state
				gvfosplit [0] = 0;
				break;
			case 1:
				// Set VFO B to simplex state
				gvfosplit [0] = 1;
				break;
			case 2:
				// Set VFO B to Memory Channel
				break;
			default:
				cat_answer_request(CAT_BADCOMMAND_INDEX);
				break;
			}

			//const uint_fast8_t bi = getbankindex_ab(1);	/* VFO B bank index */
			//const uint_fast32_t v = catparam;
			//catchangefreq(vfy32up(v, TUNE_BOTTOM, TUNE_TOP - 1, gfreqs [bi]), gtx);
			//updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_FR_INDEX);
		}
	}
	else if (match2('F', 'T'))
	{
		if (cathasparam != 0)
		{
			// gvfosplit: At index 0: RX VFO A or B, at index 1: TX VFO A or B
			switch (catparam)
			{
			case 0:
				// Set VFO A to split state
				gvfosplit [1] = 0;
				break;
			case 1:
				// Set VFO B to split state
				gvfosplit [1] = 1;
				break;
			default:
				cat_answer_request(CAT_BADCOMMAND_INDEX);
				break;
			}

			//const uint_fast8_t bi = getbankindex_ab(1);	/* VFO B bank index */
			//const uint_fast32_t v = catparam;
			//catchangefreq(vfy32up(v, TUNE_BOTTOM, TUNE_TOP - 1, gfreqs [bi]), gtx);
			//updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
			rc = 1;
		}
		else
		{
			cat_answer_request(CAT_FT_INDEX);
		}
	}
#endif /* WITHSPLITEX */
#if WITHCATEXT
#if WITHIF4DSP
	else if (match2('N', 'T'))
	{
		// Sets and reads the Notch Filter status.
		cat_answer_request(CAT_BADCOMMAND_INDEX);
	}
	else if (match2('B', 'P'))
	{
		// Adjusts the Notch Frequency of the Manual Notch Filter.
		cat_answer_request(CAT_BADCOMMAND_INDEX);
	}
	else if (match2('S', 'Q'))
	{
		// Squelch level set/report
		if (cathasparam != 0)
		{
			if (catpcount == 4)
			{
				//const uint_fast32_t p1 = vfy32up(catscanint(catp + 0, 1), 0, 0, 0);
				const uint_fast32_t p2 = vfy32up(catscanint(catp + 1, 3), 0, SQUELCHMAX, 0);
				gsquelch = p2;
				updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
				rc = 1;
			}
			else if (catpcount == 1)
			{
				const uint_fast32_t p1 = vfy32up(catscanint(catp + 0, 1), 0, 0, 0);
				cat_answerparam_map [CAT_SQ_INDEX] = p1;
				cat_answer_request(CAT_SQ_INDEX);	// sqanswer
				rc = 1;
			}
			else
			{
				cat_answer_request(CAT_BADCOMMAND_INDEX);
			}
		}
		else
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
	}
#endif /* WITHIF4DSP */
	else if (match2('R', 'A'))
	{
		// Attenuator status set/query
		if (cathasparam /* && (catparam == 0 || catparam == 1) */)
		{
			gatts [getbankindex_tx(gtx)] = vfy32up(catparam, 0, ATTMODE_COUNT - 1, 0);	/* 0..1 */;	// one step or fully attenuated
			updateboard(1, 0);	/* ������ ������������� (��� ����� ����� ������) */
			cat_answer_request(CAT_RA_INDEX);
			rc = 1;
		}
		else
		{
			// read attenuator state
			cat_answer_request(CAT_RA_INDEX);
		}
	}
	else if (match2('P', 'A'))
	{
		// Preamplifier status set/query
		if (cathasparam /* && (catparam == 0 || catparam == 1) */)
		{
#if ! WITHONEATTONEAMP
			gpamps [getbankindex_tx(gtx)] = vfy32up(catparam, 0, PAMPMODE_COUNT - 1, 0);	/* 0..1 */;	// one step or fully attenuated
			updateboard(1, 0);	/* ������ ������������� (��� ����� ����� ������) */
#endif /* ! WITHONEATTONEAMP */
			cat_answer_request(CAT_PA_INDEX);
			rc = 1;
		}
		else
		{
			// read preamplifier state
			cat_answer_request(CAT_PA_INDEX);
		}
	}
#if WITHANTSELECT
	else if (match2('A', 'N'))
	{
		// in differencies from documentation, ant1=1, ant2=2
		// antenna selection set/query
		if (cathasparam)
		{
			const uint_fast8_t p1 = catparam / 100;
			if (p1 != 9)
			{
				gantennas [getbankindex_tx(gtx)] = vfy32up(p1, 1, ANTMODE_COUNT, 1) - 1;	/* 0..1 */;	// one step or fully attenuated
				updateboard(1, 0);	/* ������ ������������� (��� ����� ����� ������) */
			}
			cat_answer_request(CAT_AN_INDEX);
			rc = 1;
		}
		else
		{
			// read antenna selection
			cat_answer_request(CAT_AN_INDEX);
		}
	}
#endif /* WITHANTSELECT */
	else if (match2('P', 'S'))
	{
		if (cathasparam)
		{
			// set param
			cat_answer_request(CAT_PS_INDEX);
		}
		else
		{
			// read state
			cat_answer_request(CAT_PS_INDEX);
		}
	}
	else if (match2('S', 'M'))
	{
		// Get S-neter information
		if (cathasparam && catparam == 0)
		{
			// parameter a
			cat_answerparam_map [CAT_SM_INDEX] = 0;
			cat_answer_request(CAT_SM_INDEX);
		}
		else if (cathasparam && catparam == 9)
		{
			// parameter a
			cat_answerparam_map [CAT_SM_INDEX] = 9;
			cat_answer_request(CAT_SM_INDEX);
		}
		else
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
	}
	else if (match2('U', 'P'))
	{
		if (cathasparam)
		{
			const int steps = vfy32up(catparam, 0, 99, 1);	/* 00 .. 99 */
			encoder_pushback(steps, ghiresdiv);
		}
	}
	else if (match2('D', 'N'))
	{
		if (cathasparam)
		{
			const int steps = vfy32up(catparam, 0, 99, 1);	/* 00 .. 99 */
			encoder_pushback(0 - steps, ghiresdiv);
		}
	}
#if 0
	else if (match2('E', 'X'))
	{
		if (cathasparam)
		{
			switch (cathasparam)
			{
			case 58:
				// EX0058xxx; ���������� SPLIT
				break;
			}
		}
	}
#endif
#endif /* WITHCATEXT */
#if WITHTX
	else if (match2('T', 'X'))
	{
		if (cathasparam != 0)
		{
			const uint_fast32_t v = catparam;	/* 0 - send, 1 - data send, 2 - TX TUNE */
			switch (v)
			{
			case 0:
				catstatetx = 1;		/* ��� ���������� ������������ � �������� � �� ���������� */
				break;
			case 1:
				catstatetx = 1;		/* ��� ���������� ������������ � �������� � �� ���������� */
				catstatetxdata = 1;
				break;
			case 2:
				cattunemode = 1;
				break;
			}

			if (aistate != 0)
				cat_answer_request(CAT_TX_INDEX);	// ignore main/sub rx selection (0 - main. 1 - sub);
		}
		else
		{
			catstatetx = 1;		/* ��� ���������� ������������ � �������� � �� ���������� */
			catstatetxdata = 0;
			cattunemode = 0;
			if (aistate != 0)
				cat_answer_request(CAT_TX_INDEX);
		}
	}
	else if (match2('R', 'X'))
	{
		if (cathasparam != 0)
		{
			cattunemode = 0;
			catstatetx = 0;		/* ��� ���������� ������������ � �������� � �� ���������� */
			catstatetxdata = 0;
			if (aistate != 0)
				cat_answer_request(CAT_RX_INDEX);	// POSSIBLE: ignore main/sub rx selection (0 - main. 1 - sub);
		}
		else
		{
			cattunemode = 0;
			catstatetx = 0;		/* ��� ���������� ������������ � �������� � �� ���������� */
			catstatetxdata = 0;
			if (aistate != 0)
				cat_answer_request(CAT_RX_INDEX);
		}
	}
#if WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) && WITHCATEXT
	else if (match2('R', 'M'))
	{
		// Get SWR, COMP or ALC- meter information
		if (cathasparam != 0)
		{
			switch (catparam)
			{
			case 1:	// SWR
			case 2: // COMP
			case 3: // ALC
				rmstate = catparam;
				break;
			default:
				cat_answer_request(CAT_BADCOMMAND_INDEX);
				break;
			}
		}
		else
		{
			switch (rmstate)
			{
			default:
			case 1:
				cat_answer_request(CAT_RM1_INDEX);	// SWR
				break;
			case 2:
				cat_answer_request(CAT_RM2_INDEX);	// COMP
				break;
			case 3:
				cat_answer_request(CAT_RM3_INDEX);	// ALC
				break;
			}
		}
	}
#endif /*  WITHTX && WITHSWRMTR && WITHCATEXT */
#endif /*  WITHTX */
	else if (match2('F', 'W'))
	{
		// filter width
		if (cathasparam != 0)
		{
			// ������ ������ ����������� ������� �� ����� � ������
			const uint_fast32_t width = vfy32up(catparam, 0, 9999, 3100); 
			const uint_fast8_t i = findfilter(gmode, gfi, width);	/* ����� �������, ����������� ��� ������� ������ */
			gfi = getsuitablerx(gmode, i); /* ��� ������������ ����� CAT ���������� � NVRAM �� ������������ */
			updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
		}
		else
		{
			cat_answer_request(CAT_FW_INDEX);
		}
	}
#if WITHCATEXT && WITHELKEY
	else if (match2('K', 'S'))
	{
		// keyer speed
		if (cathasparam != 0)
		{
			// �������� �������� �� 10 �� 60 WPM (� TS-590 �� 4 �� 60).
#if ! WITHPOTWPM
			elkeywpm = vfy32up(catparam, CWWPMMIN, CWWPMMAX, 20); 
#endif /* ! WITHPOTWPM */
			updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
		}
		else
		{
			cat_answer_request(CAT_KS_INDEX);
		}
	}
#endif	/* WITHCATEXT */
#if WITHIF4DSP//CTLSTYLE_V1D || CTLSTYLE_OLEG4Z_V1 || 1
	else if (match2('Z', 'Z'))
	{
		if (cathasparam != 0)
		{
			if (catpcount == 12)
			{
				// Format: ZZmLLLLUUUUSSS
				const uint_fast32_t v = catp [0] - '0';
				const uint_fast8_t submode = findkenwoodsubmode(v, UINT8_MAX);	/* ����� �� ������������� ������ */
				if (submode == UINT8_MAX)
				{
					cat_answer_request(CAT_BADCOMMAND_INDEX);
				}
				else
				{
					const FLASHMEM struct modetempl * const pmodet = getmodetempl(submode);
					//ZZmLLLLUUUUSSSS
					const uint_fast8_t bwseti = pmodet->bwseti;
					const uint_fast8_t pos = bwsetpos [bwseti];
					bwprop_t * const p = bwsetsc [bwseti].prop [pos];
					p->left10_width10 = vfy32up(catscanint(catp + 1, 4), p->limits->left10_width10_low,p->limits->left10_width10_high, p->left10_width10);
					p->right100 = vfy32up(catscanint(catp + 5, 4), p->limits->right100_low, p->limits->right100_high, p->right100);
					if (p->type == BWSET_WIDE)
						p->afresponce = vfy32up(catscanint(catp + 9, 3), AFRESPONCEMIN, AFRESPONCEMAX, p->afresponce);
					updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
					rc = 1;
				}
			}
			else if (catpcount == 1)
			{
				const uint_fast32_t v = catp [0] - '0';
				cat_answerparam_map [CAT_ZZ_INDEX] = v;
				cat_answer_request(CAT_ZZ_INDEX);	// zzanswer
			}
			else
			{
				cat_answer_request(CAT_BADCOMMAND_INDEX);
			}
			//const uint_fast8_t bi = getbankindex_ab(1);	/* VFO B bank index */
			//const uint_fast32_t v = catparam;
			//catchangefreq(vfy32up(v, TUNE_BOTTOM, TUNE_TOP - 1, gfreqs [bi]), gtx);
			//updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
		}
		else
		{
			cat_answer_request(CAT_BADCOMMAND_INDEX);
		}
	}
#endif /* CTLSTYLE_V1D || CTLSTYLE_OLEG4Z_V1 */
	else
	{
		// �������������� ������� - �������� ���������.
#if 0
		// ������ ���������� � �������� �������
		display_gotoxy(0, 1);		// ������ � ������ ������ ������

		display_wrdata_begin();
		display_wrdata_fast(catcommand1 & 0x7f);
		display_wrdata_fast(catcommand2 & 0x7f);
		display_wrdata_end();

		if (cathasparam)
			display_menu_digit(catparam, 7, 0, 0);
		else
		{
			display_wrdata_begin();
			display_wrdata_fast(';');
			display_wrdata_end();
		}
		//local_delay_ms(500);
#endif
		cat_answer_request(CAT_BADCOMMAND_INDEX);
	}

#endif // WITHCAT
	return rc;
	#undef match2
}

#endif	/* WITHCAT */


#if WITHELKEY


#if WITHBEACON

static uint_fast8_t beacon_state;
static uint_fast8_t beacon_pause;
static uint_fast8_t beacon_index;
static char beacon_text [] = "V";

enum
{
	BEACONST_INITIALIZE,
	BEACON_PAUSE,
	BEACON_TEXT,
	//
	BEACON_COUNT
};

static void beacon_initialize(void)
{
	beacon_state = BEACONST_INITIALIZE;
}

static void beacon_states(void)
{
	switch (beacon_state)
	{
	case BEACONST_INITIALIZE:
		break;
	case BEACON_PAUSE:
		break;
	case BEACON_TEXT:
		break;
	}
}

// ���������� �� user mode
uint_fast8_t beacon_get_ptt(void)
{
	return beacon_text [beacon_index] != '\0';
}


#endif /* WITHBEACON */

/* ���������� �� ����������� ���������� */
/* �������� ��������� ������ ��� �������� (������ ������� �������) */
static char beacon_getnextcw(void)
{
#if WITHBEACON
	char c = beacon_text [beacon_index];

	if (c == '\0')
		beacon_index = 0;
	else
		++ beacon_index;

	return c;
#else /* WITHBEACON */
	return '\0';
#endif /* WITHBEACON */
}

// ��������� ������� ��� �������� (������ ������� �������)
uint_fast8_t elkey_getnextcw(void)
{
#if WITHCAT && WITHCATEXT
	const uint_fast8_t chcat = cat_getnextcw();
	const uint_fast8_t ch = (chcat != '\0') ? chcat : beacon_getnextcw();
	return ch;
#else
	return beacon_getnextcw();
#endif /* WITHCAT && WITHCATEXT*/
}

#endif /* WITHELKEY */



/* ��������� ��������� �� ������ ������������ ���������� � user-level ��������. */
static void 
//NOINLINEAT
processmessages(uint_fast8_t * kbch, uint_fast8_t * kbready, uint_fast8_t inmenu)
{
	if (hardware_getshutdown())	// ������� ������� �������
	{
		display_discharge();	// ��������� �������
		gtx = 0;
		updateboard(1, 1);	// ��������� �� ����
		for (;;)				// ��������...
			;
	}


	uint8_t * buff;

	* kbready = 0;
	* kbch = KBD_CODE_MAX;

	switch (takemsgready_user(& buff))
	{
	case MSGT_EMPTY:
		display2_bgprocess();			/* ���������� ����� state machine ����������� ������� */
		directctlupdate(inmenu);		/* ���������� ��������� �������� (� ������� �����������) ����� ������������ */
#if WITHUSEAUDIOREC
		sdcardbgprocess();
#endif /* WITHUSEAUDIOREC */
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
		// ���������� ������ �� ���������� ��������� ���������� �� user mode ���������
		if (dimmflagch != 0)
		{
			dimmflagch = 0;
			display2_bgreset();
			display_redrawfreqmodesbars(0);			/* ���������� ������� - ��, ������� ������� */
			updateboard(1, 0);
		}
#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */
#if WITHFANTIMER
		// ���������� ������ �� ���������� ��������� ���������� �� user mode ���������
		if (fanpaflagch != 0)
		{
			fanpaflagch = 0;
			updateboard(1, 0);
		}
#endif /* WITHFANTIMER */
#if WITHSLEEPTIMER
		// ���������� ������ �� ���������� ��������� ���������� �� user mode ���������
		if (sleepflagch != 0)
		{
			sleepflagch = 0;
			display2_bgreset();
			display_redrawfreqmodesbars(0);			/* ���������� ������� - ��, ������� ������� */
			updateboard(1, 0);
		}
#endif /* WITHSLEEPTIMER */
#if WITHCAT
		if (cat_getstateout() == CATSTATEO_SENDREADY)
		{
			cat_answer_forming();
		}
#endif /* WITHCAT */
		return;
	
	case MSGT_1SEC:
#if WITHSECTOGGLE
		sectoggle = ! sectoggle;
#endif /* WITHSECTOGGLE */
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
		if (dimmtime == 0)
		{
			// ������� ���������
			if (dimmflag != 0)
			{
				dimmflag = 0;
				dimmflagch = 1;		// ������ �� ���������� ��������� ���������� �� user mode ���������
			}
			dimmcount = 0;
		}
		else if (dimmflag == 0)		// ��� �� ���������
		{
			if (++ dimmcount >= dimmtime)
			{
				dimmflag = 1;
				dimmflagch = 1;		// ������ �� ���������� ��������� ���������� �� user mode ���������
			}
		}
#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */
#if WITHFANTIMER
		if (gtx != 0 || fanpatime == 0)
		{
			if (fanpaflag != 0)	
			{
				fanpaflag = 0;
				fanpaflagch = 1;
			}
			fanpacount = 0;		/* ������� ������� ������������ */
		}
		else if (fanpaflag == 0)		// ��� �� ���������
		{
			if (++ fanpacount >= fanpatime)
			{
				fanpaflag = 1;
				fanpaflagch = 1;		// ������ �� ���������� ��������� ���������� �� user mode ���������
			}
		}
#endif /* WITHFANTIMER */
#if WITHSLEEPTIMER
		if (sleeptime == 0)
		{
			// ������� ���������
			if (sleepflag != 0)
			{
				sleepflag = 0;
				sleepflagch = 1;		// ������ �� ���������� ��������� ���������� �� user mode ���������
			}
			sleepcount = 0;
		}
		else if (sleepflag == 0)		// ��� �� ���������
		{
			if (++ sleepcount >= sleeptime * 60)
			{
				sleepflag = 1;
				sleepflagch = 1;		// ������ �� ���������� ��������� ���������� �� user mode ���������
			}
		}
#endif /* WITHSLEEPTIMER */
		break;

	case MSGT_CAT:
		board_wakeup();
#if WITHCAT
		{
			// check MSGBUFFERSIZE8 valie
			// 12 bytes as parameter
			//debug_printf_P(PSTR("processmessages: MSGT_CAT\n"));
			if (processcatmsg(buff [0], buff [1], buff [2], buff [8], buff + 9))
				display_redrawfreqmodesbars(inmenu);			/* ���������� ������� - ��, ������� ������� */
		}
#endif /* WITHCAT */
		break;

	case MSGT_KEYB:
		//debug_printf_P(PSTR("processmessages: MSGT_KEYB\n"));
		board_wakeup();
		//if (board_wakeup() == 0)
		{
			// ������ ������, ������� � ��������� "���" ��� "������������ �������" ������������
			* kbch = buff [0];
			* kbready = 1;
		}
		break;

	default:
		break;
	}
	releasemsgbuffer_user(buff);
}

/* ���������� �� ����������� ���������� ��� � ������� */
void spool_secound(void)
{
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
	uint8_t * buff;
	if (takemsgbufferfree_low(& buff) != 0)
	{
		placesemsgbuffer_low(MSGT_1SEC, buff);
	}
#endif /* WITHLCDBACKLIGHT || WITHKBDBACKLIGHT */
}


/* ��������� ���������� ������� �� ��������.	*/
static void
//NOINLINEAT
processtxrequest(void)
{
#if WITHTX
	uint_fast8_t txreq = 0;
	uint_fast8_t tunreq = 0;
#if WITHSECTOGGLE
	if (sectoggle)
	{
		txreq = 1;
	}
#endif /* WITHSECTOGGLE */
	if (moxmode || hardware_get_ptt())	// ��������, ������
	{
#if WITHCAT	
		cat_reset_ptt();	// ����� ����������� ������ �� �������� - "��������" ������.
#endif	/* WITHCAT */
		txreq = 1;
	}
#if WITHBEACON	
	if (beacon_get_ptt())
	{
		txreq = 1;
	}
#endif	/* WITHCAT */
#if WITHCAT	
	if (cat_get_ptt())
	{
		txreq = 1;
	}
#endif	/* WITHCAT */
#if WITHMODEM
	if (modem_get_ptt())
	{
		txreq = 1;
	}
#endif	/* WITHMODEM */
	if (moxmode)
	{
		txreq = 1;
	}
	if (getactualtune())
	{
		tunreq = 1;
	}
	seq_txrequest(tunreq, tunreq || txreq);
#endif /* WITHTX */
}

// ��� �������������� �������� - ����� �������� �������� ����������.
// ��� ��������������� ��������� ��� �������
static void 
//NOINLINEAT
display_menu_digit(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast32_t value,
	uint_fast8_t width,		// WSIGNFLAG can be added for display '+; or '-'
	uint_fast8_t comma,
	uint_fast8_t rj
	)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;

	display_setcolors(MNUVALCOLOR, BGCOLOR);
	do
	{
		display_gotoxy(x, y + lowhalf);		// ������ � ������ ������ ������
		display_menu_value(value, width, comma, rj, lowhalf);
	} while (lowhalf --);
}

// ��� �������������� �������� - ����� ��������� (�� FLASHMEM) �������� ����������.
// ��� ��������������� ��������� ��� �������

static void 
display_menu_string_P(
	uint_fast8_t x, 
	uint_fast8_t y, 
	const FLASHMEM  char * s,
	uint_fast8_t width,
	uint_fast8_t comma
	)
{
	display_setcolors(MNUVALCOLOR, BGCOLOR);
	display_at_P(x + width - comma, y, s);
}

#if WITHMENU

#define ITEM_VALUE	0x01	/* ����� ���� ��� �������������� ��������� */
#define ITEM_GROUP	0x02	/* ����� ���� ��� ����������� �������� - ������ � ������� */

#define ITEM_FILTERU	0x04	/* ����� ���� ��� ���������� ������ ������� �� (��������������� ����) */
#define ITEM_FILTERL	0x08	/* ����� ���� ��� ���������� ������ ������� �� (�������������� ����) */

#define ITEM_NOINITNVRAM	0x10	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */

struct menudef
{
	char qlabel [9];		/* ����� - �������� ������ ���� */
	uint8_t qwidth, qcomma, qrj;
	uint8_t qistep;
	uint8_t qspecial;	/* ������� � ������ ���� ��������� */
	uint16_t qbottom, qupper;	/* ����������� �� ������������� �������� (upper - �������) */

	nvramaddress_t qnvram;				/* ���� MENUNONVRAM - ������ ������ � ������ */

	uint_fast16_t * qpval16;			/* ����������, ������� ������������ - ���� ��� 16 ��� */
	uint_fast8_t * qpval8;			/* ����������, ������� ������������  - ���� ��� 8 ���*/
	int_fast32_t (* funcoffs)(void);	/* ��� ����������� � ������������� ����������� ����� ������ */
};

static const FLASHMEM struct menudef menutable [] =
{
#if WITHAUTOTUNER && 1 // Tuner parameters debug
#if ! WITHFLATMENU
	{
		"TUNER   ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrptuner),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"TUNER L ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE, 
		LMIN, LMAX, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		& tunerind,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"TUNER C ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE, 
		CMIN, CMAX, 
		MENUNONVRAM, //offsetof(struct nvmap, tunercap),
		& tunercap,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"TUNER TY", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE, 
		0, KSCH_COUNT - 1, 
		MENUNONVRAM, //offsetof(struct nvmap, tunertype),
		NULL,
		& tunertype,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"TUNER WT", 7, 0, 0,	ISTEP5,	// �������� ����� ���������� ����� ������������ ����
		ITEM_VALUE, 
		10, 80, 
		offsetof(struct nvmap, tunerdelay),
		NULL,
		& tunerdelay,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHAUTOTUNER */
#if ! WITHFLATMENU
	{
		"DISPLAY ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpdisplay),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
#if defined (DEFAULT_LCD_CONTRAST)
	{
		"LCD CONT", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
#if LCDMODE_PCF8535
		42, 70, 				// ��� TIC218 @ 3.3 volt - 45..60
#elif LCDMODE_RDX0120
		70, 120, 				// ��� amator - 110..220
#elif LCDMODE_UC1601
		75, 220, 				// ��� amator - 110..220
#elif LCDMODE_LS020 || LCDMODE_ILI9320
		0, 255, 				// LS020 - 0..255
#elif LCDMODE_UC1608
		0, 63, 					// UC1608 - 0..63
#endif /* LCDMODE_UC1601 */
		offsetof(struct nvmap, gcontrast),
		NULL,
		& gcontrast,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* defined (DEFAULT_LCD_CONTRAST) */
#if 0//WITHDCDCFREQCTL
	{
		"DCDC DIV", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		4, UINT16_MAX, 
		offsetof(struct nvmap, dcdcrefdiv),
		& dcdcrefdiv,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHDCDCFREQCTL */
#if WITHLCDBACKLIGHT
	{
		"LCD LIGH", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		WITHLCDBACKLIGHTMIN, WITHLCDBACKLIGHTMAX, 
		offsetof(struct nvmap, bglight),
		NULL,
		& bglight,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHLCDBACKLIGHT */
#if WITHKBDBACKLIGHT
	{
		"KBD LIGH", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, kblight),
		NULL,
		& kblight,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHKBDBACKLIGHT */
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
	{
		"DIMM TIM", 7, 0, 0,	ISTEP5,	
		ITEM_VALUE,
		0, 240, 
		offsetof(struct nvmap, dimmtime),
		NULL,
		& dimmtime,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHKBDBACKLIGHT */
#if WITHSLEEPTIMER
	{
		"SLEEPTIM", 7, 0, 0,	ISTEP5,	
		ITEM_VALUE,
		0, 240, 
		offsetof(struct nvmap, sleeptime),
		NULL,
		& sleeptime,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHSLEEPTIMER */
#if LCDMODE_COLORED
	// ��� ������� �������� ����� ������ ���� ����
	{
		"BLUE BG ", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gbluebgnd),
		NULL,
		& gbluebgnd,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif
	{
		"FREQ FPS", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		4, 25,							/* ������� ���������� ��������� ������� �� 5 �� 25 ��� � ������� */
		offsetof(struct nvmap, displayfreqsfps),
		NULL,
		& displayfreqsfps,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if WITHBARS
	{
		"SMTR FPS", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		4, 25,							/* ������� ���������� ��������� �� 5 �� 25 ��� � ������� */
		offsetof(struct nvmap, displaybarsfps),
		NULL,
		& displaybarsfps,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHBARS */
#if WITHSPECTRUMWF	/* ����������� ����������� �� ������������� ����������� */
#endif /* WITHSPECTRUMWF */
#if defined (RTC1_TYPE)
#if ! WITHFLATMENU
	{
		"CLOCK   ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpclock),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"TM YEAR ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE, 
		2015, 2099, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		& grtcyear,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"TM MONTH", 7, 0, RJ_MONTH,	ISTEP1,	
		ITEM_VALUE, 
		1, 12, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		NULL,
		& grtcmonth,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"TM DAY  ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE, 
		1, 31, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		NULL,
		& grtcday,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"TM HOUR ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE, 
		0, 23, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		NULL,
		& grtchour,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"TM MIN  ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE, 
		0, 59, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		NULL,
		& grtcminute,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"TM SET  ", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE, 
		0, 1, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		NULL,
		& grtcstrobe,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* defined (RTC1_TYPE) */
#if ! WITHFLATMENU
	{
		"FILTERS ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpfilters),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	// ��������� �� ������ � ������ ������� ������������ ����� - ��������� ��� ������������ ������� CW/SSB
	{
		"CW PITCH", 7, 2, 0, 	ISTEP10, 
		ITEM_VALUE,
		40, 190,			/* 400 Hz..1900, Hz in 100 Hz steps */
		offsetof(struct nvmap, gcwpitch10),
		NULL,
		& gcwpitch10,
		getzerobase, 
	},
#if WITHIF4DSP
	{
		"CW WDT W", 7, 2, 0, 	ISTEP10,	// CW bandwidth for WIDE
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		10, 180,			/* 100 Hz..1800, Hz in 100 Hz steps */
		RMT_BWPROPSLEFT_BASE(BWPROPI_CWWIDE),
		NULL,
		& bwprop_cwwide.left10_width10,
		getzerobase, 
	},
#if 0
	{
		"CW SFT W", 7, 0, 0, 	ISTEP1,	// CW softer parameter for WIDE
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHFILTSOFTMIN, WITHFILTSOFTMAX, 
		RMT_BWPROPSFLTSOFTER_BASE(BWPROPI_CWWIDE),
		NULL,
		& bwprop_cwwide.fltsofter,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif
	{
		"CW WDT N", 7, 2, 0, 	ISTEP10,	// CW bandwidth for NARROW
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		10, 180,			/* 100 Hz..1800, Hz in 100 Hz steps */
		RMT_BWPROPSLEFT_BASE(BWPROPI_CWNARROW),
		NULL,
		& bwprop_cwnarrow.left10_width10,
		getzerobase, 
	},
#if 0
	{
		"CW SFT N", 7, 0, 0, 	ISTEP1,	// CW softer parameter for NARROW
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHFILTSOFTMIN, WITHFILTSOFTMAX, 
		RMT_BWPROPSFLTSOFTER_BASE(BWPROPI_CWNARROW),
		NULL,
		& bwprop_cwnarrow.fltsofter,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif
	{
		"SSB HI W", 6, 1, 0,	ISTEP1,		/* ���������� ������ ����������� - SSB WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHSSBHIGH100MIN, WITHSSBHIGH100MAX, 		// 0.8 kHz-5.8 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_SSBWIDE),
		NULL,
		& bwprop_ssbwide.right100,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"SSB LO W", 7, 2, 0,	ISTEP5,		/* ���������� ������ ����������� - SSB WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHSSBLOW10MIN, WITHSSBLOW10MAX, 		// 50 Hz-700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_SSBWIDE),
		NULL,
		& bwprop_ssbwide.left10_width10,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if 0
	{
		"SSB SFTW", 7, 0, 0, 	ISTEP1,	// SSB softer parameter for WIDE
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHFILTSOFTMIN, WITHFILTSOFTMAX, 
		RMT_BWPROPSFLTSOFTER_BASE(BWPROPI_SSBWIDE),
		NULL,
		& bwprop_ssbwide.fltsofter,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif
	{
		"SSB AFRW", 3 + WSIGNFLAG, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* ��������� ������ ����� - �� Samplerate/2 ��� ���������� �� ������� �������  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBWIDE),
		NULL,
		& bwprop_ssbwide.afresponce,
		getafresponcebase, /* ������������ �� ��������� � ������������ */
	},
	{
		"SSB HI N", 6, 1, 0,	ISTEP1,		/* ���������� ������ ����������� - SSB NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHSSBHIGH100MIN, WITHSSBHIGH100MAX, 		// 0.8 kHz-5.8 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_SSBNARROW),
		NULL,
		& bwprop_ssbnarrow.right100,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"SSB LO N", 7, 2, 0,	ISTEP5,		/* ���������� ������ ����������� - SSB NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHSSBLOW10MIN, WITHSSBLOW10MAX, 		// 50 Hz-700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_SSBNARROW),
		NULL,
		& bwprop_ssbnarrow.left10_width10,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if 0
	{
		"SSB SFTN", 7, 0, 0, 	ISTEP1,	// SSB softer parameter for NARROW
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHFILTSOFTMIN, WITHFILTSOFTMAX, 
		RMT_BWPROPSFLTSOFTER_BASE(BWPROPI_SSBNARROW),
		NULL,
		& bwprop_ssbnarrow.fltsofter,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif
	{
		"SSB AFRN", 3 + WSIGNFLAG, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* ��������� ������ ����� - �� Samplerate/2 ��� ���������� �� ������� �������  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBNARROW),
		NULL,
		& bwprop_ssbnarrow.afresponce,
		getafresponcebase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AM HI W ", 6, 1, 0,	ISTEP2,		/* ���������� ������ ����������� - AM WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHAMHIGH100MIN, WITHAMHIGH100MAX, 		// 0.8 kHz-6.0 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_AMWIDE),
		NULL,
		& bwprop_amwide.right100,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AM LO W ", 7, 2, 0,	ISTEP5,		/* ���������� ������ ����������� - AM WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHAMLOW10MIN, WITHAMLOW10MAX,
		RMT_BWPROPSLEFT_BASE(BWPROPI_AMWIDE),
		NULL,
		& bwprop_amwide.left10_width10,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if 0
	{
		"AM SFT W", 7, 0, 0, 	ISTEP1,	// AM softer parameter for WIDE
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHFILTSOFTMIN, WITHFILTSOFTMAX, 
		RMT_BWPROPSFLTSOFTER_BASE(BWPROPI_AMWIDE),
		NULL,
		& bwprop_amwide.fltsofter,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif
	{
		"AM AFR W", 3 + WSIGNFLAG, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* ��������� ������ ����� - �� Samplerate/2 ��� ���������� �� ������� �������  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBWIDE),
		NULL,
		& bwprop_amwide.afresponce,
		getafresponcebase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AM HI N ", 6, 1, 0,	ISTEP2,		/* ���������� ������ ����������� - AM NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHAMHIGH100MIN, WITHAMHIGH100MAX, 		// 0.8 kHz-6.0 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_AMNARROW),
		NULL,
		& bwprop_amnarrow.right100,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AM LO N ", 7, 2, 0,	ISTEP5,		/* ���������� ������ ����������� - AM NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHAMLOW10MIN, WITHAMLOW10MAX,
		RMT_BWPROPSLEFT_BASE(BWPROPI_AMNARROW),
		NULL,
		& bwprop_amnarrow.left10_width10,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if 0
	{
		"AM SFT N", 7, 0, 0, 	ISTEP1,	// AM softer parameter for NARROW
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		WITHFILTSOFTMIN, WITHFILTSOFTMAX, 
		RMT_BWPROPSFLTSOFTER_BASE(BWPROPI_AMNARROW),
		NULL,
		& bwprop_amnarrow.fltsofter,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif
	{
		"AM AFR N", 3 + WSIGNFLAG, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* ��������� ������ ����� - �� Samplerate/2 ��� ���������� �� ������� �������  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBWIDE),
		NULL,
		& bwprop_amnarrow.afresponce,
		getafresponcebase, /* ������������ �� ��������� � ������������ */
	},
	{
		"SSBTX HI", 6, 1, 0,	ISTEP1,		/* ���������� ������ ����������� - TX SSB */
		ITEM_VALUE,
		8, 58, 		// 0.8 kHz-5.8 kHz
		offsetof(struct nvmap, gssbtxhighcut100),
		NULL,
		& gssbtxhighcut100,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"SSBTX LO", 7, 2, 0,	ISTEP1,		/* ���������� ������ ����������� - TX SSB */
		ITEM_VALUE,
		5, 70,		// 50 Hz..700 Hz
		offsetof(struct nvmap, gssbtxlowcut10),
		NULL,
		& gssbtxlowcut10,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHIF4DSP */

#if WITHIFSHIFT && ! WITHPOTIFSHIFT
	// ���������� �������� ��������� ������� �������� ����� ������� � ����� ������� �������
	{
		"IF SHIFT", 4 + WSIGNFLAG, 2, 1, 	ISTEP50, 
		ITEM_VALUE,
		IFSHIFTTMIN, IFSHIFTMAX,			/* -3 kHz..+3 kHz in 5 Hz steps */
		offsetof(struct nvmap, ifshifoffset),
		& ifshifoffset,
		NULL,
		getifshiftbase, 
	},
#endif /* WITHIFSHIFT && ! WITHPOTIFSHIFT */

#if CTLSTYLE_RA4YBO_V3
/*
filter_t fi_0p5 =
filter_t fi_3p1 =
filter_t fi_3p0_455 =
filter_t fi_10p0_455 =
filter_t fi_6p0_455 =
filter_t fi_2p0_455 =	// strFlash2p0
*/
	{
		"3.1 USB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe3p1),
		& fi_3p1.high,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	{
		"3.1 LSB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe3p1),
		& fi_3p1.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	{
		"2.4 USB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe3p0),
		& fi_3p0_455.high,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	{
		"2.4 LSB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe3p0),
		& fi_3p0_455.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	{
		"CNTR 0.5", 7, 2, 1,	ISTEP10,	/* ����������� ������� ������������ ������� */
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, carr0p5),
		& fi_0p5.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */
	{
		"2K OFFS ", 4 + WSIGNFLAG, 2, 1, 	ISTEP10, 
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq2k),
		& fi_2p0_455.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */
	{
		"6K OFFS ", 4 + WSIGNFLAG, 2, 1, 	ISTEP10, 
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq6k),
		& fi_6p0_455.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */
	{
		"10K OFFS", 4 + WSIGNFLAG, 2, 1, 	ISTEP10, 
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq10k),
		& fi_10p0_455.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},

#elif WITHDUALFLTR	/* ��������� ������� �� ���� ������������ ������� ������� ��� ������ ������� ������ */
	{
		"BFO FREQ", 7, 2, 1,	ISTEP10,	
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lo4offset),
		& lo4offset,
		NULL,
		getlo4base, /* ������������ �� ��������� � ������������ */
	},
#elif WITHDUALBFO	/* ��������� ������� �� ���� ������������ ������� �������� ���������� */
	{
		"BFO USB ", 7, 2, 1,	ISTEP10,	
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lo4offsets [0]),
		& lo4offsets [0],
		NULL,
		getlo4base, /* ������������ �� ��������� � ������������ */
	},
	{
		"BFO LSB ", 7, 2, 1,	ISTEP10,	
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lo4offsets [1]),
		& lo4offsets [1],
		NULL,
		getlo4base, /* ������������ �� ��������� � ������������ */
	},
#elif WITHFIXEDBFO	/* ��������� ������� �� ���� 1-�� ��������� (��������, ���� ��� ��������������� BFO) */
	{
		"BFO FREQ", 7, 2, 1,	ISTEP10,	
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lo4offset),
		& lo4offset,
		NULL,
		getlo4base, /* ������������ �� ��������� � ������������ */
	},
	{
		"LAST LSB", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, glo4lsb),
		NULL,
		& glo4lsb,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},

#elif CTLSTYLE_RA4YBO_V1 || (defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) && (IF3_MODEL != IF3_TYPE_BYPASS))
	/* ������� ����� - ����� �� �������� ������������� ���������� ���������� */

#if ! CTLSTYLE_SW2011ALL
#if WITHTX
	{
		"DC TX CW", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, /* 0 - off, 1 - on */
		offsetof(struct nvmap, dctxmodecw),
		NULL,
		& dctxmodecw,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHTX */
#endif /* ! CTLSTYLE_SW2011ALL */

	#if (IF3_FMASK & IF3_FMASK_2P4)
	{
		"2.4 USB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe2p4),
		& fi_2p4.high,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	{
		"2.4 LSB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe2p4),
		& fi_2p4.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_2P4) */
	#if (IF3_FMASK & IF3_FMASK_2P7)
	{
		"BFO7 USB", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe2p7),
		& fi_2p7.high,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	{
		"BFO7 LSB", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe2p7),
		& fi_2p7.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) != 0
	{
		"CAR7 USB", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe2p7tx),
		& fi_2p7_tx.high,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	{
		"CAR7 LSB", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe2p7tx),
		& fi_2p7_tx.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 */
	#endif /* (IF3_FMASK & IF3_FMASK_2P7) */
	
	#if (IF3_FMASK & IF3_FMASK_3P1)
	{
		"BFO USB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe3p1),
		& fi_3p1.high,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	{
		"BFO LSB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe3p1),
		& fi_3p1.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_3P1) != 0
	{
		"CAR USB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe3p1tx),
		& fi_3p1_tx.high,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	{
		"CAR LSB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe3p1tx),
		& fi_3p1_tx.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 */
	#endif /* (IF3_FMASK & IF3_FMASK_3P1) */
	
	#if (IF3_FMASK & IF3_FMASK_0P3)
	{
		"HAVE 0.3", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw0p3),
		NULL,
		& fi_0p3.present,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"CNTR 0.3", 7, 2, 1,	ISTEP10,	/* ����������� ������� ������������ ������� */
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, carr0p3),
		& fi_0p3.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_0P3) */

	#if (IF3_FMASK & IF3_FMASK_0P5)
	{
		"HAVE 0.5", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw0p5),
		NULL,
		& fi_0p5.present,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"CNTR 0.5", 7, 2, 1,	ISTEP10,	/* ����������� ������� ������������ ������� */
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, carr0p5),
		& fi_0p5.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_0P5) */

	#if (IF3_FMASK & IF3_FMASK_1P8)
	{
		"HAVE 1.8", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw1p8),
		NULL,
		& fi_1p8.present,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"1.8 USB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe1p8),
		& fi_1p8.high,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	{
		"1.8 LSB ", 7, 2, 1,	ISTEP10,	
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe1p8),
		& fi_1p8.low_or_center,
		NULL,
		NULL,	/* ������ �������� ��� ����������� ������ �� ��������� filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_1P8) */

	#if (IF3_FMASK & IF3_FMASK_2P4)
	{
		"HAVE 2.4", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw2p4),
		NULL,
		& fi_2p4.present,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_2P4) */

	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4)
	{
		"HAVE T24", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw2p4_tx),
		NULL,
		& fi_2p4_tx.present,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4) */

	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7)
	{
		"HAVE T27", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw2p7_tx),
		NULL,
		& fi_2p7_tx.present,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) */

	#if 0 && WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_3P1)
	{
		"HAVE T31", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw3p1_tx),
		NULL,
		& fi_3p1_tx.present,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) */

	#if (IF3_FMASK & IF3_FMASK_6P0)
	{
		"HAVE 6.0", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw6p0),
		NULL,
		& fi_6p0.present,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */
	{
		"6K OFFS ", 4 + WSIGNFLAG, 2, 1, 	ISTEP10, 
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq6k),
		& fi_6p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* (IF3_FMASK & IF3_FMASK_6P0) */

	#if (IF3_FMASK & IF3_FMASK_7P8)
	/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */
	{
		"7K8 OFFS", 4 + WSIGNFLAG, 2, 1, 	ISTEP10, 
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq7p8k),
		& fi_7p8.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* (IF3_FMASK & IF3_FMASK_7P8) */

	#if (IF3_FMASK & IF3_FMASK_8P0)
	/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */
	{
		"8K OFFS ", 4 + WSIGNFLAG, 2, 1, 	ISTEP10, 
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq8k),
		& fi_8p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif
	#if (IF3_FMASK & IF3_FMASK_9P0)
	/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */
	{
		"9K OFFS ", 4 + WSIGNFLAG, 2, 1, 	ISTEP10, 
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq9k),
		& fi_9p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* (IF3_FMASK & IF3_FMASK_9P0) */
	#if (IF3_FMASK & IF3_FMASK_15P0)
	#if WITHDEDICATEDNFM /* ��� NFM ��������� ����� �� ����� �������� */
	/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */
	{
		"NFM OFFS", 4 + WSIGNFLAG, 2, 1, 	ISTEP10, 
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq15k_nfm),
		& fi_15p0_tx_nfm.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#else /* WITHDEDICATEDNFM */
	/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */
	{
		"15K OFFS", 4 + WSIGNFLAG, 2, 1, 	ISTEP10, 
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq15k),
		& fi_15p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* WITHDEDICATEDNFM */
	#endif /* (IF3_FMASK & IF3_FMASK_15P0) */
	#if (IF3_FMASK & IF3_FMASK_17P0)
	/* �������� ����������� ������� ��� ��� ��������, � ������� ��� �������������� ��������� ������ */
	{
		"17K OFFS", 4 + WSIGNFLAG, 2, 1, 	ISTEP10, 
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq17k),
		& fi_17p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif

#endif /* defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) && (IF3_MODEL != IF3_TYPE_BYPASS) */

#if WITHNOTCHFREQ
#if ! WITHFLATMENU
	{
		"NOTCH   ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpnotch),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"NOTCH   ", 7, 2, RJ_ON,	ISTEP1,		/* ���������� ������� NOTCH */
		ITEM_VALUE,
		0, NOTCHMODE_COUNT - 1,
		RMT_NOTCH_BASE,							/* ���������� ������� NOTCH */
		NULL,
		& gnotch,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	#if ! WITHPOTNOTCH
	{
		"NTCH FRQ", 7, 2, 1,	ISTEP50,		/* ���������� �������� NOTCH. */
		ITEM_VALUE,
		WITHNOTCHFREQMIN, WITHNOTCHFREQMAX,
		offsetof(struct nvmap, gnotchfreq),	/* ����������� ������� NOTCH */
		& gnotchfreq,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"NTCH WDT", 7, 0, 0,	ISTEP50,		/* ������ �������� NOTCH. */
		ITEM_VALUE,
		100, 1000,
		offsetof(struct nvmap, gnotchwidth),	/* ������ �������� NOTCH */
		& gnotchwidth,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	#endif /* ! WITHPOTNOTCH */
#elif WITHNOTCHONOFF
#if ! WITHFLATMENU
	{
		"NOTCH   ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpnotch),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"NOTCH   ", 7, 2, RJ_ON,	ISTEP1,		/* ���������� ������� NOTCH */
		ITEM_VALUE,
		0, NOTCHMODE_COUNT - 1,
		RMT_NOTCH_BASE,							/* ���������� ������� NOTCH */
		NULL,
		& gnotch,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHNOTCHFREQ */

#if defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX)
#if ! WITHPOTPBT && WITHPBT // && (LO3_SIDE != LOCODE_INVALID)
#if ! WITHFLATMENU
	{
		"PBTS   ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrppbts),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"PBT     ", 4 + WSIGNFLAG, 2, 1, 	ISTEP50, 
		ITEM_VALUE,
		PBTMIN, PBTMAX,			/* -15 kHz..+15 kHz in 5 Hz steps */
		offsetof(struct nvmap, pbtoffset),
		& gpbtoffset,
		NULL,
		getpbtbase, 
	},
#endif /* ! WITHPOTPBT && WITHPBT && (LO3_SIDE != LOCODE_INVALID) */
#endif /* defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) */

#if WITHELKEY
#if ! WITHFLATMENU
	{
		"ELKEY   ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpelkey),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
  #if ! WITHPOTWPM
	{
		"CW SPEED", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		CWWPMMIN, CWWPMMAX,		// minimal WPM = 10, maximal = 60 (also changed by command KS).
		offsetof(struct nvmap, elkeywpm),
		NULL,
		& elkeywpm,
		getzerobase, 
	},
  #endif /* ! WITHPOTWPM */
  #if WITHVIBROPLEX
	{
		"VIBROPLX", 7, 0, 0,	ISTEP1,		/* �������� ���������� ������������ ����� � ����� - �������� ����������� */
		ITEM_VALUE,
		0, 5,		// minimal 0 - ��� ������� �����������
		offsetof(struct nvmap, elkeyslope),
		NULL,
		& elkeyslope,
		getzerobase, 
	},
    #if ELKEY328
	{
		"VIBROENB", 7, 0, RJ_ON,	ISTEP1,		/* ���������� ������ � ������ ����������� */
		ITEM_VALUE,
		0, 1,		// minimal 0 - ��� ������� �����������
		offsetof(struct nvmap, elkeyslopeenable),
		NULL,
		& elkeyslopeenable,
		getzerobase, 
	},
    #endif /* ELKEY328 */
  #endif /* WITHVIBROPLEX */
	{
		"KEYER   ", 6, 0, RJ_ELKEYMODE,	ISTEP1,	
		ITEM_VALUE,
		0, 3,	/* ����� ������������ ����� - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
		offsetof(struct nvmap, elkeymode),
		NULL,
		& elkeymode,
		getzerobase, 
	},
	{
		"CWKEYREV", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1,	/* ����� ������������ ����� - �������� ������� ����� � ���� ��� ���. */
		offsetof(struct nvmap, elkeyreverse),
		NULL,
		& elkeyreverse,
		getzerobase, 
	},
	{
		"CWWEIGHT", 7, 1, 0,	ISTEP1,	
		ITEM_VALUE,
		23, 45, 
		offsetof(struct nvmap, dashratio),
		NULL,
		& dashratio,
		getzerobase, 
	},
	{
		"SPACEWGH", 7, 1, 0,	ISTEP1,	
		ITEM_VALUE,
		7, 13, 
		offsetof(struct nvmap, spaceratio),
		NULL,
		& spaceratio,
		getzerobase, 
	},
#if WITHIF4DSP
	{
		"EDGE TIM", 7, 0, 0,	ISTEP1,		/* Set the rise time of the transmitted CW envelope. */
		ITEM_VALUE,
		2, 16, 
		offsetof(struct nvmap, gcwedgetime),	/* ����� ����������/����� ��������� ��������� ��� �������� - � 1 �� */
		NULL,
		& gcwedgetime,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHIF4DSP */
#endif /* WITHELKEY */
#if WITHDSPEXTDDC	/* "��������" � DSP � FPGA */
#if ! WITHFLATMENU
	{
		"RF ADC  ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrprfadc),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"ADC RAND", 7, 0, RJ_ON,	ISTEP1,	/* ���������� ����������� � LTC2208 */
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gadcrand),
		NULL,
		& gadcrand,
		getzerobase, 
	},
	{
		"ADC DITH", 7, 0, RJ_ON,	ISTEP1,	/* ���������� ����������� � LTC2208 */
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gdither),
		NULL,
		& gdither,
		getzerobase, 
	},
	{
		"ADC FIFO", 7, 0, RJ_ON,	ISTEP1,	/*  */
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gadcfifo),
		NULL,
		& gadcfifo,
		getzerobase, 
	},
	{
		"ADC OFFS", 5 + WSIGNFLAG, 0, 0,	ISTEP1,	/* �������� ��� ��������� ������� � ��� */
		ITEM_VALUE,
		ADCOFFSETMID - 100, ADCOFFSETMID + 100, 
		offsetof(struct nvmap, gadcoffset),
		& gadcoffset,
		NULL,
		getadcoffsbase,	/* ������������ �� ��������� � ������������ */
	},
	{
		"DAC TEST", 7, 0, RJ_ON,	ISTEP1,	/*  */
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gdactest),
		NULL,
		& gdactest,
		getzerobase, 
	},
#endif /* WITHDSPEXTDDC */
#if ! WITHFLATMENU
	{
		"OTHERS  ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpothers),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
#if WITHTX
#if WITHELKEY
	{
		"BREAK-IN", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bkinenable),
		NULL,
		& bkinenable,
		getzerobase, 
	},
	{
		"CW DELAY", 7, 2, 0,	ISTEP5,	/* �������� � �������� ms */
		ITEM_VALUE,
		5, 160,						/* 0.05..1.6 ������� */
		offsetof(struct nvmap, bkindelay),
		NULL,
		& bkindelay,
		getzerobase, 
	},
#endif /* WITHELKEY */
#if WITHFANTIMER
	{
		"FAN TIME", 7, 0, 0,	ISTEP5,	
		ITEM_VALUE,
		0, FANPATIMEMAX, 
		offsetof(struct nvmap, fanpatime),
		NULL,
		& fanpatime,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHFANTIMER */
#endif /* WITHTX */
#if WITHTX
#if WITHPOWERTRIM
  #if ! WITHPOTPOWER
	{
		"TX POWER", 7, 0, 0,	ISTEP1,		/* �������� ��� ������� ������ �� �������� */
		ITEM_VALUE,
		WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
		offsetof(struct nvmap, gnormalpower),
		NULL,
		& gnormalpower,
		getzerobase, 
	},
  #endif /* ! WITHPOTPOWER */
  #if WITHAUTOTUNER || defined (HARDWARE_GET_TUNE)
	{
		"ATU PWR ", 7, 0, 0,	ISTEP1,		/* �������� ��� ������ ��������������� ������������ ���������� */
		ITEM_VALUE,
		WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
		offsetof(struct nvmap, gotunerpower),
		NULL,
		& gotunerpower,
		getzerobase, 
	},
  #endif /* WITHAUTOTUNER || defined (HARDWARE_GET_TUNE) */
#elif WITHPOWERLPHP
  #if ! WITHPOTPOWER
	#if ! CTLSTYLE_SW2011ALL
	{
		"TX POWER", 7, 0, RJ_POWER,	ISTEP1,		/* �������� ��� ������� ������ �� �������� */
		ITEM_VALUE,
		WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
		offsetof(struct nvmap, gnormalpower),
		NULL,
		& gnormalpower,
		getzerobase, 
	},
	#endif /* ! CTLSTYLE_SW2011ALL */
  #endif /* ! WITHPOTPOWER */
#if WITHAUTOTUNER || defined (HARDWARE_GET_TUNE)
	{
		"ATU PWR ", 7, 0, RJ_POWER,	ISTEP1,		/* �������� ��� ������ ��������������� ������������ ���������� */
		ITEM_VALUE,
		WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
		offsetof(struct nvmap, gotunerpower),
		NULL,
		& gotunerpower,
		getzerobase, 
	},
#endif /* WITHAUTOTUNER || defined (HARDWARE_GET_TUNE) */
#endif /* WITHPOWERTRIM */
#if ! CTLSTYLE_SW2011ALL
	{
		"TX GATE ", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gtxgate),
		NULL,
		& gtxgate,
		getzerobase, 
	},
#endif /* ! CTLSTYLE_SW2011ALL */
#if WITHVOX
#if ! WITHFLATMENU
	{
		"VOX     ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpvox),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"VOX EN  ", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gvoxenable),
		NULL,
		& gvoxenable,
		getzerobase, 
	},
	{
		"VOXDELAY", 7, 2, 0,	ISTEP5,	/* 50 mS step of changing value */
		ITEM_VALUE,
		10, 250,						/* 0.1..2.5 secounds delay */ 
		offsetof(struct nvmap, voxdelay),
		NULL,
		& voxdelay,
		getzerobase, 
	},
	{
		"VOX LEVL", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		0, 100, 
		offsetof(struct nvmap, gvoxlevel),
		NULL,
		& gvoxlevel,
		getzerobase, 
	},
	{
		"AVOX LEV", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		0, 100, 
		offsetof(struct nvmap, gavoxlevel),
		NULL,
		& gavoxlevel,
		getzerobase, 
	},
#endif /* WITHVOX */
#endif /* WITHTX */
#if WITHCAT
#if ! WITHFLATMENU
	{
		"CAT     ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpcat),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"CAT ENAB", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, catenable),
		NULL,
		& catenable,
		getzerobase, 
	},
	#if WITHUSBCDC == 0
	{
		"CAT SPD ", 7, 0, RJ_CATSPEED,	ISTEP1,	
		ITEM_VALUE,
		0, (sizeof catbr2int / sizeof catbr2int [0]) - 1, 
		offsetof(struct nvmap, catbaudrate),
		NULL,
		& catbaudrate,
		getzerobase, 
	},
	#endif /* WITHUSBCDC == 0 */
	{
		"CAT DTR ", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, catdtrenable),
		NULL,
		& catdtrenable,
		getzerobase, 
	},
	#if WITHTX
	{
		"CAT RTS ", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, catrtsenable),
		NULL,
		& catrtsenable,
		getzerobase, 
	},
	{
		"CATTXDTR", 7, 0, RJ_YES,	ISTEP1,	/* �������� ����������� �� DTR, � �� �� RTS */
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, catdtrptt),	
		NULL,
		& catdtrptt,
		getzerobase, 
	},
	#endif /* WITHTX */
#endif /* WITHCAT */

#if WITHSUBTONES && WITHTX
#if ! WITHFLATMENU
	{
		"CTCSS   ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpctcss),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"CTCSS   ", 7, 0, RJ_ON,	ISTEP1,	//  Continuous Tone-Coded Squelch System or CTCSS control
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gsbtonenable),
		NULL,
		& gsbtonenable,
		getzerobase, 
	},
	{
		"CTCSS FQ", 7, 1, RJ_SUBTONE,	ISTEP1,	//  Continuous Tone-Coded Squelch System or CTCSS freq
		ITEM_VALUE,
		0, sizeof gsubtones / sizeof gsubtones [0] - 1, 
		offsetof(struct nvmap, gsubtonei),
		NULL,
		& gsubtonei,
		getzerobase, 
	},
#if WITHIF4DSP
	{
		"CTCSSLVL", 7, 0, 0,	ISTEP1,		/* Select the CTCSS transmit level. */
		ITEM_VALUE,
		0, 100, 
		offsetof(struct nvmap, gsubtonelevel),	/* ������� ������� ������������ � ��������� - 0%..100% */
		NULL,
		& gsubtonelevel,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHIF4DSP */
#endif /* WITHSUBTONES && WITHTX */
#if ! WITHFLATMENU
	{
		"AUDIO   ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpaudio),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
#if (SIDETONE_TARGET_BIT != 0) || WITHINTEGRATEDDSP
	{
		"KEY BEEP", 7, 2, 0, 	ISTEP5,		/* ����������� ���� ������� ������ */
		ITEM_VALUE,
		80, 250,			/* 800 Hz..2500, Hz in 50 Hz steps */
		offsetof(struct nvmap, gkeybeep10),
		NULL,
		& gkeybeep10,
		getzerobase, 
	},
#endif
#if WITHMUTEALL && WITHTX
	{
		"MUTE ALL", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gmuteall),
		NULL,
		& gmuteall,
		getzerobase, 
	},
#endif /* WITHMUTEALL && WITHTX */
#if WITHIF4DSP
	{
		"SQUELCH ", 7, 0, 0,	ISTEP1,		/* squelch level */
		ITEM_VALUE,
		0, SQUELCHMAX, 
		offsetof(struct nvmap, gsquelch),	/* ������� ������� ����� �������� ����������� ������� */
		NULL,
		& gsquelch,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"SDTN LVL", 7, 0, 0,	ISTEP1,		/* Select the CW sidetone or keypad sound output level.. */
		ITEM_VALUE,
		0, 100, 
		offsetof(struct nvmap, gsidetonelevel),	/* ������� ������� ������������ � ��������� - 0%..100% */
		NULL,
		& gsidetonelevel,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHIF4DSP */
#if WITHTX && WITHIF4DSP
	#if WITHAFCODEC1HAVELINEINLEVEL	/* ����� ����� ���������� ��������� � ��������� ����� */
	{
		"LINE LVL", 7, 0, 0,	ISTEP1,		/* ���������� �������� � ��������� ����� ����� ����. */
		ITEM_VALUE,
		WITHLINEINGAINMIN, WITHLINEINGAINMAX, 
		offsetof(struct nvmap, glineamp),	/* �������� � ��������� ����� */
		NULL,
		& glineamp,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	#endif /* WITHAFCODEC1HAVELINEINLEVEL */
	{
		"MIKE SSB", 7, 0, RJ_TXAUDIO,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */		
		0, BOARD_TXAUDIO_count - 1, 					// ��� SSB/AM/FM �������� � �������� ����������
		RMT_TXAUDIO_BASE(MODE_SSB),
		NULL,
		& gtxaudio [MODE_SSB],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"MIKE DIG", 7, 0, RJ_TXAUDIO,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// ��� SSB/AM/FM �������� � �������� ����������
		RMT_TXAUDIO_BASE(MODE_DIGI),
		NULL,
		& gtxaudio [MODE_DIGI],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"MIKE AM ", 7, 0, RJ_TXAUDIO,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// ��� SSB/AM/FM �������� � �������� ����������
		RMT_TXAUDIO_BASE(MODE_AM),
		NULL,
		& gtxaudio [MODE_AM],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"MIKE FM ", 7, 0, RJ_TXAUDIO,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// ��� SSB/AM/FM �������� � �������� ����������
		RMT_TXAUDIO_BASE(MODE_NFM),
		NULL,
		& gtxaudio [MODE_NFM],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"MIKE AGC", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,	
		0, 1, 					/* ��������� ����������� ��� ����� ����������� */
		offsetof(struct nvmap, gmikeagc),
		NULL,
		& gmikeagc,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"MK AGCGN", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,	
		10, 60, 					/* ������������ �������� ��� ��������� � �� */
		offsetof(struct nvmap, gmikeagcgain),
		NULL,
		& gmikeagcgain,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"MIK CLIP", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,	
		0, 90, 					/* ����������� */
		offsetof(struct nvmap, gmikehclip),
		NULL,
		& gmikehclip,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"MIK BUST", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,	
		0, 1, 					// ������������� ������� � ���������
		offsetof(struct nvmap, gmikebust20db),
		NULL,
		& gmikebust20db,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	#if WITHAFCODEC1HAVEPROC	/* ����� ����� ���������� ���������� ������������ ������� (�������, ����������, ...) */
	{
		"MIK EQUA", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gmikeequalizer),
		NULL,
		& gmikeequalizer,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	// ���������� 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
	{
		"EQUA .08", 2 + WSIGNFLAG, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		0, 12 * 2,
		offsetof(struct nvmap, gmikeequalizerparams [0]),
		NULL,
		& gmikeequalizerparams [0],
		getequalizerbase, /* ������������ � -12 � ������������ */
	},
	{
		"EQUA .23", 2 + WSIGNFLAG, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		0, 12 * 2,
		offsetof(struct nvmap, gmikeequalizerparams [1]),
		NULL,
		& gmikeequalizerparams [1],
		getequalizerbase, /* ������������ � -12 � ������������ */
	},
	{
		"EQUA .65", 2 + WSIGNFLAG, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		0, 12 * 2,
		offsetof(struct nvmap, gmikeequalizerparams [2]),
		NULL,
		& gmikeequalizerparams [2],
		getequalizerbase, /* ������������ � -12 � ������������ */
	},
	{
		"EQUA 1.8", 2 + WSIGNFLAG, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		0, 12 * 2,
		offsetof(struct nvmap, gmikeequalizerparams [3]),
		NULL,
		& gmikeequalizerparams [3],
		getequalizerbase, /* ������������ � -12 � ������������ */
	},
	{
		"EQUA 5.3", 2 + WSIGNFLAG, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		0, 12 * 2,
		offsetof(struct nvmap, gmikeequalizerparams [4]),
		NULL,
		& gmikeequalizerparams [4],
		getequalizerbase, /* ������������ � -12 � ������������ */
	},

	#endif /* WITHAFCODEC1HAVEPROC */
#endif /* WITHTX && WITHIF4DSP */
#if WITHMIC1LEVEL && WITHTX
	{
		"MIC LEVL", 7, 0, 0,	ISTEP1,		/* ���������� �������� ������������ ��������� ����� ����. */
		ITEM_VALUE,
		WITHMIKEINGAINMIN, WITHMIKEINGAINMAX, 
		offsetof(struct nvmap, mik1level),	/* �������� ������������ ��������� */
		& mik1level,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* ITHMIC1LEVEL && WITHTX */
#if WITHUSEAUDIOREC
	{
		"SD RECRD", 7, 0, RJ_ON,	ISTEP1,		/* ������������� �������� ������ �� SD CARD ��� ��������� */
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, recmode),
		NULL,
		& recmode,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHUSEAUDIOREC */
#if WITHUSBUAC
	{
		"PLAY USB", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,	
		0, 1, 					/* ����� ������������� ������ ���������� � ��������� ���������� - ���������� ����� */
		offsetof(struct nvmap, guacplayer),
		NULL,
		& guacplayer,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	#if WITHRTS96 || WITHRTS192 || WITHTRANSPARENTIQ
	{
		"I/Q SWAP", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,	
		0, 1, 					/* �������� ������� I � Q ������ � ������ RTS96 */
		offsetof(struct nvmap, gswapiq),
		NULL,
		& gswapiq,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	#endif /* WITHRTS96 || WITHRTS192 || WITHTRANSPARENTIQ */
#endif /* WITHUSBUAC */
#if WITHIF4DSP
#if ! WITHFLATMENU
	{
		"NFM     ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpnfm),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"SQL OFF ", 7, 0, RJ_YES,	ISTEP1,		/* NFM - noise blanker off */
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, tdsp_nfm_sql_off),	/* ��������� ������� */
		NULL,
		& tdsp_nfm_sql_off,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"SQL LVL ", 7, 0, 0,	ISTEP1,		/* NFM - noise blanker level */
		ITEM_VALUE,
		0, 255, 
		offsetof(struct nvmap, tdsp_nfm_sql_level),	/* ����� ������� ��������� ���� ��� */
		NULL,
		& tdsp_nfm_sql_level,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if ! WITHFLATMENU
	{
		"AGC     ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpagc),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"ADC FS  ", 3 + WSIGNFLAG, 1, 0,	ISTEP1,		/* ���������� S-����� - ������ ���������� */
		ITEM_VALUE,
		0, FSADCPOWEROFFSET10 * 2, 		// -50..+50 dBm
		offsetof(struct nvmap, gfsadcpower10 [0]),
		& gfsadcpower10 [0],	// 16 bit
		NULL,
		getfsasdcbase10, /* ������������ �� ��������� � ������������ */
	},
	{
		"ADC FSXV", 3 + WSIGNFLAG, 1, 0,	ISTEP1,		/* � ������������ ���������� S-����� - ������ ���������� */
		ITEM_VALUE,
		0, FSADCPOWEROFFSET10 * 2, 		// -50..+50 dBm
		offsetof(struct nvmap, gfsadcpower10 [1]),
		& gfsadcpower10 [1],	// 16 bit
		NULL,
		getfsasdcbase10, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC OFF ", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,	
		0, 1, 					// ������������� ������� � ���������
		offsetof(struct nvmap, gagcoff),
		NULL,
		& gagcoff,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"IFGN MAX", 7, 0, 0,	ISTEP1,		/* �������� ������ (��� ��������������) ����������� ��������� �������� - ������������ �������� */
		ITEM_VALUE,
		40, 120, 		// 40..120 dB
		offsetof(struct nvmap, gdigigainmax),
		NULL,
		& gdigigainmax,	// 8 bit
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if CTLSTYLE_RAVENDSP_V1 || WITHEXTERNALDDSP
	{
		"AD605 GN", 7, 0, 0,	ISTEP1,		/* ���������� �� AD605 (���������� ��������� ������ �� */
		ITEM_VALUE,
		0, UINT8_MAX, 		//
		offsetof(struct nvmap, gvad605),
		NULL,
		& gvad605,	// 8 bit
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* CTLSTYLE_RAVENDSP_V1 || WITHEXTERNALDDSP */
#if ! WITHFLATMENU
	{
		"AGC SSB ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpagcssb),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"AGC RATE", 7, 0, 0,	ISTEP1,		/* ���������� ��������� ��� ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		1, 80, 
		offsetof(struct nvmap, afsets [AGCSETI_SSB].agc_rate),	/* �� N ������� ��������� �������� ������� ���������� 1 �� ��������� */
		NULL,
		& gagc_rate [AGCSETI_SSB],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC HUNG", 6, 1, 0,	ISTEP1,		/* ���������� ��������� ��� (����� ��������� ��������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		0, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_SSB].agc_thung10),	/* ����� ��������� ��������� ���� ��� */
		NULL,
		& gagc_thung10 [AGCSETI_SSB],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC T1  ", 7, 0, 0,	ISTEP10,		/* ���������� ��������� ��� (����� ������������ ��������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_SSB].agc_t1),	/* ����� ������������ ��������� ���� ��� */
		NULL,
		& gagc_t1 [AGCSETI_SSB],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC T2  ", 6, 1, 0,	ISTEP1,		/* ���������� ��������� ��� (����� ������� ��������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		1, 100, 
		offsetof(struct nvmap, afsets [AGCSETI_SSB].agc_release10),	/* ����� ������� ��������� ���� ��� */
		NULL,
		& gagc_release10 [AGCSETI_SSB],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC T4  ", 7, 0, 0,	ISTEP10,		/* ���������� ��������� ��� (����� ������� ������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_SSB].agc_t4),	/* ����� ������� ������� ���� ��� */
		NULL,
		& gagc_t4 [AGCSETI_SSB],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if ! WITHFLATMENU
	{
		"AGC CW  ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpagccw),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"AGC RATE", 7, 0, 0,	ISTEP1,		/* ���������� ��������� ��� ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		1, 80, 
		offsetof(struct nvmap, afsets [AGCSETI_CW].agc_rate),	/* �� N ������� ��������� �������� ������� ���������� 1 �� ��������� */
		NULL,
		& gagc_rate [AGCSETI_CW],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC HUNG", 6, 1, 0,	ISTEP1,		/* ���������� ��������� ��� (����� ��������� ��������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		0, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_CW].agc_thung10),	/* ����� ��������� ��������� ���� ��� */
		NULL,
		& gagc_thung10 [AGCSETI_CW],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC T1  ", 7, 0, 0,	ISTEP10,		/* ���������� ��������� ��� (����� ������������ ��������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_CW].agc_t1),	/* ����� ������������ ��������� ���� ��� */
		NULL,
		& gagc_t1 [AGCSETI_CW],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC T2  ", 6, 1, 0,	ISTEP1,		/* ���������� ��������� ��� (����� ������� ��������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		1, 100, 
		offsetof(struct nvmap, afsets [AGCSETI_CW].agc_release10),	/* ����� ������� ��������� ���� ��� */
		NULL,
		& gagc_release10 [AGCSETI_CW],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC T4  ", 7, 0, 0,	ISTEP10,		/* ���������� ��������� ��� (����� ������� ������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_CW].agc_t4),	/* ����� ������� ������� ���� ��� */
		NULL,
		& gagc_t4 [AGCSETI_CW],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if ! WITHFLATMENU
	{
		"AGC DIGI", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpagcdigi),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"AGC RATE", 7, 0, 0,	ISTEP1,		/* ���������� ��������� ��� ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		1, 80, 
		offsetof(struct nvmap, afsets [AGCSETI_DIGI].agc_rate),	/* �� N ������� ��������� �������� ������� ���������� 1 �� ��������� */
		NULL,
		& gagc_rate [AGCSETI_DIGI],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC HUNG", 6, 1, 0,	ISTEP1,		/* ���������� ��������� ��� (����� ��������� ��������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		0, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_DIGI].agc_thung10),	/* ����� ��������� ��������� ���� ��� */
		NULL,
		& gagc_thung10 [AGCSETI_DIGI],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC T1  ", 7, 0, 0,	ISTEP10,		/* ���������� ��������� ��� (����� ������������ ��������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_DIGI].agc_t1),	/* ����� ������������ ��������� ���� ��� */
		NULL,
		& gagc_t1 [AGCSETI_DIGI],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC T2  ", 6, 1, 0,	ISTEP1,		/* ���������� ��������� ��� (����� ������� ��������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		1, 100, 
		offsetof(struct nvmap, afsets [AGCSETI_DIGI].agc_release10),	/* ����� ������� ��������� ���� ��� */
		NULL,
		& gagc_release10 [AGCSETI_DIGI],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"AGC T4  ", 7, 0, 0,	ISTEP10,		/* ���������� ��������� ��� (����� ������� ������� ����) ����� ����. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* �������� ����� ������ �� ������������ ��� ��������� ������������� NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_DIGI].agc_t4),	/* ����� ������� ������� ���� ��� */
		NULL,
		& gagc_t4 [AGCSETI_DIGI],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHIF4DSP */
#if WITHMODEM
#if ! WITHFLATMENU
	{
		"MODEM   ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpmodem),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"DATA MOD", 5, 0, RJ_MDMMODE, 	ISTEP1, 
		ITEM_VALUE,
		0, 1,			/* 0: BPSK, 1: QPSK */
		offsetof(struct nvmap, gmodemmode),
		NULL,
		& gmodemmode,
		getzerobase, 
	},
	{
		"DATA SPD", 7, 2, RJ_MDMSPEED, 	ISTEP1, 
		ITEM_VALUE,
		0, (sizeof modembr2int100 / sizeof modembr2int100 [0]) - 1, 
		offsetof(struct nvmap, gmodemspeed),
		NULL,
		& gmodemspeed,
		getzerobase, 
	},
#endif /* WITHMODEM */
#if WITHLFM
#if ! WITHFLATMENU
	{
		"LFM     ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrplfm),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"LFM MODE", 7, 0, RJ_ON, 	ISTEP1, 
		ITEM_VALUE,
		0, 1,			/* LFM mode enable */
		offsetof(struct nvmap, lfmmode),
		NULL,
		& lfmmode,
		getzerobase, 
	},
	{
		"LFM STRT", 5, 1, 0, 	ISTEP1, 
		ITEM_VALUE,
		10, 300,			/* 10.0 MHz.. 30.0 MHz in 100 kHz steps */
		offsetof(struct nvmap, lfmstart100k),
		& lfmstart100k,
		NULL,
		getzerobase, 
	},
	{
		"LFM STOP", 5, 1, 0, 	ISTEP1, 
		ITEM_VALUE,
		10, 300,			/* 0.0 MHz.. 30.0 MHz in 100 kHz steps */
		offsetof(struct nvmap, lfmstop100k),
		& lfmstop100k,
		NULL,
		getzerobase, 
	},
	{
		"LFM SPD ", 5, 0, 0, 	ISTEP1, 
		ITEM_VALUE,
		50, 550,			/* 50 kHz/sec..550 kHz/sec, 1 kHz/sec steps */
		offsetof(struct nvmap, lfmspeed1k),
		& lfmspeed1k,
		NULL,
		getzerobase, 
	},
	// ������� �� ������ ���� �� �������
	{
		"LFM OFST", 5, 0, 0, 	ISTEP1, 
		ITEM_VALUE,
		0, 60 * 60 - 1,			/* 0..59:59 */
		offsetof(struct nvmap, lfmtoffset),
		& lfmtoffset,
		NULL,
		getzerobase, 
	},
	// �������� � �������� ����� ��������� � �������� ����
	{
		"LFM PERI", 5, 0, 0, 	ISTEP1, 
		ITEM_VALUE,
		1, 60 * 60 - 1,			/* 00:01..59:59 */
		offsetof(struct nvmap, lfmtinterval),
		& lfmtinterval,
		NULL,
		getzerobase, 
	},
#endif /* WITHLFM */
#if ! WITHFLATMENU
	{
		"SPECIAL ", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpsecial),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
#if WITHRFSG
	{
		"RFSG MOD", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, userfsg),
		NULL,
		& userfsg,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHRFSG */
#if WITHIF4DSP
#if ! WITHPOTAFGAIN
	{
		"AF GAIN ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		BOARD_AFGAIN_MIN, BOARD_AFGAIN_MAX, 					// ��������� � ���������
		offsetof(struct nvmap, afgain1),
		& afgain1,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* ! WITHPOTAFGAIN */
#if ! WITHPOTIFGAIN
	{
		"RF GAIN ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		BOARD_IFGAIN_MIN, BOARD_IFGAIN_MAX, 					// �������� ��/�� � ���������
		offsetof(struct nvmap, rfgain1),
		& rfgain1,
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* ! WITHPOTIFGAIN */
#endif /* WITHIF4DSP */
#if WITHENCODER
	{
		"ENC RES ", 7, 0, RJ_ENCRES,	ISTEP1,	
		ITEM_VALUE,
		0, (sizeof encresols / sizeof encresols [0]) - 1, 
		offsetof(struct nvmap, ghiresres),
		NULL,
		& ghiresres,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"ENC DYNA", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, ghiresdyn),
		NULL,
		& ghiresdyn,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"ENC DIVS", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		1, 128, 	/* /1 ... /128 */
		offsetof(struct nvmap, ghiresdiv),
		NULL,
		& ghiresdiv,
		getzerobase, 
	},
	{
		"BIG STEP", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gbigstep),
		NULL,
		& gbigstep,
		getzerobase, 
	},
#endif /* WITHENCODER */
#if WITHPABIASTRIM && WITHTX
	{
		"PA BIAS ", 7, 0, 0,	ISTEP1,		/* ���������� ���� ���������� ������� ����������� */
		ITEM_VALUE,
		WITHPABIASMIN, WITHPABIASMAX,
		offsetof(struct nvmap, gpabias),
		NULL,
		& gpabias,
		getzerobase, 
	},
#endif /* WITHPABIASTRIM && WITHTX */
#if WITHIF4DSP && WITHTX
	{
		"AM DEPTH", 7, 0, 0,	ISTEP1,		/* ���������� ������� ��������� � �� */
		ITEM_VALUE,
		0, 100, 
		offsetof(struct nvmap, gamdepth),	/* ������� ��������� � �� - 0..100% */
		NULL,
		& gamdepth,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"DACSCALE", 7, 0, 0,	ISTEP1,		/* ���������� ��������� ������� � ��� ����������� */
		ITEM_VALUE,
		0, 100, 
		offsetof(struct nvmap, gdacscale),	/* ��������� ������� � ��� ����������� - 0..100% */
		NULL,
		& gdacscale,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHIF4DSP && WITHTX */
#if defined(REFERENCE_FREQ)
#if defined (DAC1_TYPE)
	{
		"REF ADJ ", 7, 0, 0,	ISTEP1,		/* ���������� ������� �������� ���������� (�����������) ����� ����. */
		ITEM_VALUE,
		WITHDAC1VALMIN, WITHDAC1VALMAX, 
		offsetof(struct nvmap, dac1level),
		NULL,	/* ���������� �������� */
		& dac1level,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* defined (DAC1_TYPE) */
	{
		"REF FREQ", 9, 3, 0,	ISTEP1,		/* ���� �������� ������� �������� ���������� ����� ����. */
		ITEM_VALUE,
		0, OSCSHIFT * 2 - 1, 
		offsetof(struct nvmap, refbias),
		& refbias,	/* ���������� ������� �������� */
		NULL,
		getrefbase, 	/* ������������ �� ��������� � ������������ */
	},
#endif	/* defined(REFERENCE_FREQ) */
#if (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ	/* ���������� ������� ���������� ����� ����. */
	{
		"LO3 FRQ ", 9, 3, 0,	ISTEP1,	
		ITEM_VALUE,
		LO2AMIN, LO2AMAX, 
		offsetof(struct nvmap, lo3offset),
		& lo3offset,	/* ���������� ������� ���������� */
		NULL,
		getlo3base, 	/* ������������ �� ��������� � ������������ */
	},
#endif	/* (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ */
#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	{
		"REFSI570", 9, 3, 0,	ISTEP1,	
		ITEM_VALUE,
		0, OSCSHIFT * 2 - 1, 
		offsetof(struct nvmap, si570_xtall_offset),
		& si570_xtall_offset,	/* ���������� �������� */
		NULL,
		si570_get_xtall_base, 	/* ������������ �� ��������� � ������������ */
	},
#endif /* defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570) */
#if WITHONLYBANDS
	{
		"BANDONLY", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, withonlybands),
		NULL,
		& withonlybands,
		getzerobase, 
	},
#endif /* WITHONLYBANDS */
	{
		"STAYFREQ", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, stayfreq),
		NULL,
		& stayfreq,
		getzerobase, 
	},
#if WITHVOLTLEVEL && ! WITHREFSENSOR
	{
		"BAT CALI", 7, 1, 0,	ISTEP1,			/* ������������� �������� �������� ���������� ��� */
		ITEM_VALUE,
		ADCVREF_CPU, 255,	// 3.3/5.0 .. 25.5 ������
		offsetof(struct nvmap, voltcalibr),
		NULL,
		& voltcalibr,
		getzerobase, 
	},
#endif /* WITHVOLTLEVEL && ! WITHREFSENSOR */
#if WITHTX
#if WITHSWRMTR && ! WITHSHOWSWRPWR
	{
		"SWR SHOW", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, swrmode),
		NULL,
		& swrmode,
		getzerobase, 
	},
#endif /* WITHSWRMTR && ! WITHSHOWSWRPWR */
#if (WITHSWRMTR || WITHSHOWSWRPWR)
	{
		"SWR CALI", 7, 2, 0,	ISTEP1,		/* ���������� SWR-����� */
		ITEM_VALUE,
		50, 200, //80, 120, 
		offsetof(struct nvmap, swrcalibr),
		NULL,
		& swrcalibr,
		getzerobase, 
	},
	{
		"FWD LOWR", 7, 0, 0,	ISTEP1,		/* ������������������ SWR-����� */
		ITEM_VALUE,
		1, (1U << HARDWARE_ADCBITS) - 1, 
		offsetof(struct nvmap, minforward),
		& minforward,
		NULL,
		getzerobase, 
	},
	{
		"PWR CALI", 7, 0, 0,	ISTEP1,		/* ���������� PWR-����� */
		ITEM_VALUE,
		1, 255, 
		offsetof(struct nvmap, maxpwrcali),
		NULL,
		& maxpwrcali,
		getzerobase, 
	},
	
#elif WITHPWRMTR
	{
		"PWR CALI", 7, 0, 0,	ISTEP1,		/* ���������� PWR-����� */
		ITEM_VALUE,
		10, 255, 
		offsetof(struct nvmap, maxpwrcali),
		NULL,
		& maxpwrcali,
		getzerobase, 
	},
#endif
#if WITHIF4DSP || defined (TXPATH_BIT_ENABLE_SSB) || defined (TXPATH_BIT_ENABLE_CW) || defined (TXPATH_BIT_GATE)
	{
		"RXTX DLY", 7, 0, 0,	ISTEP5,	/* 5 mS step of changing value */
		ITEM_VALUE,
		5, WITHMAXRXTXDELAY,						/* 5..100 ms delay */
		offsetof(struct nvmap, rxtxdelay),
		NULL,
		& rxtxdelay,
		getzerobase, 
	},
	{
		"TXRX DLY", 7, 0, 0,	ISTEP5,	/* 5 mS step of changing value */
		ITEM_VALUE,
		5, WITHMAXTXRXDELAY,						/* 5..100 ms delay */
		offsetof(struct nvmap, txrxdelay),
		NULL,
		& txrxdelay,
		getzerobase, 
	},
#endif /* GATEs */
#endif /* WITHTX */
#if WITHLO1LEVELADJ
	{
		"LO1 LEVL", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		0, 100, 		/* ������� (���������) LO1 � ��������� */
		offsetof(struct nvmap, lo1level),
		NULL,
		& lo1level,
		getzerobase, 
	},
#endif /* WITHLO1LEVELADJ */
#if LO1PHASES
	{
		"PHASE RX", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		0, 65535, /* ���������� � ���� �������� ���� */
		offsetof(struct nvmap, phaserx),
		& phasesmap [0],
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if WITHTX
	{
		"PHASE TX", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		0, 65535, /* ���������� � ���� �������� ���� */
		offsetof(struct nvmap, phasetx),
		& phasesmap [1],
		NULL,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHTX */
#endif /* LO1PHASES */

#if LO1MODE_HYBRID
	{
		"ALIGN MD", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, alignmode),
		NULL,
		& alignmode,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif	/* LO1MODE_HYBRID */

#if LO1FDIV_ADJ
	{
		"LO1DV RX", 7, 0, RJ_POW2,	ISTEP1,	
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		offsetof(struct nvmap, lo1powrx),
		NULL,
		& lo1powmap [0],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"LO1DV TX", 7, 0, RJ_POW2,	ISTEP1,	
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		offsetof(struct nvmap, lo1powtx),
		NULL,
		& lo1powmap [1],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif
#if LO4FDIV_ADJ
	{
		"LO4DV RX", 7, 0, RJ_POW2,	ISTEP1,	
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		offsetof(struct nvmap, lo4powrx),
		NULL,
		& lo4powmap [0],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"LO4DV TX", 7, 0, RJ_POW2,	ISTEP1,	
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		offsetof(struct nvmap, lo4powtx),
		NULL,
		& lo4powmap [1],
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif

#if WITHBARS
	{
		"S9 LEVEL", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		1, UINT8_MAX - 1, 
		offsetof(struct nvmap, s9level),
		NULL,			/* ���������� ������� S-����� */
		& s9level,
		getzerobase, 
	},
	{
		"S9 DELTA", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		1, UINT8_MAX - 1, 
		offsetof(struct nvmap, s9delta),
		NULL,			/* ���������� ������� S-����� */
		& s9delta,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
	{
		"+60DELTA", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE,
		1, UINT8_MAX - 1, 
		offsetof(struct nvmap, s9_60_delta),
		NULL,			/* ���������� ������� S-����� */
		& s9_60_delta,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHBARS */

	{
		"BAND 27 ", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bandset11m),
		NULL,
		& bandset11m,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#if WITHBCBANDS
	{
		"BAND BC ", 7, 0, RJ_YES,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bandsetbcast),
		NULL,
		& bandsetbcast,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* WITHBCBANDS */

#if CTLSTYLE_SW2011ALL
#if TUNE_6MBAND
	{
		"BAND 50 ", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bandset6m),
		NULL,
		& bandset6m,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* TUNE_6MBAND */
#if TUNE_4MBAND
	{
		"BAND 70 ", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bandset4m),
		NULL,
		& bandset4m,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* TUNE_6MBAND */
#if TUNE_2MBAND
	{
		"BAND 144", 7, 0, RJ_ON,	ISTEP1,	
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bandset2m),
		NULL,
		& bandset2m,
		getzerobase, /* ������������ �� ��������� � ������������ */
	},
#endif /* TUNE_2MBAND */
#endif /* CTLSTYLE_SW2011ALL */

#if CTLREGMODE_RA4YBO || CTLREGMODE_RA4YBO_V1 || CTLREGMODE_RA4YBO_V2 || CTLREGMODE_RA4YBO_V3 || CTLREGMODE_RA4YBO_V3A
#if ! WITHFLATMENU
	{
		"TXPARAMS", 0, 0, 0, 0,	
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrptxparams),
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		"TXPW SSB", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,
		0, 255, 		/*  */
		RMT_TXPOWER_BASE(MODE_SSB),
		& gtxpower [MODE_SSB],	// 16 bit in nvram
		NULL,
		getzerobase, 
	},
	{
		"TXPW CW ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,
		0, 255, 		/*  */
		RMT_TXPOWER_BASE(MODE_CW),
		& gtxpower [MODE_CW],	// 16 bit in nvram
		NULL,
		getzerobase, 
	},
	{
		"TXPW FM ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,
		0, 255, 		/*  */
		RMT_TXPOWER_BASE(MODE_NFM),
		& gtxpower [MODE_NFM],	// 16 bit in nvram
		NULL,
		getzerobase, 
	},
	{
		"TXPW AM ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,
		0, 255, 		/*  */
		RMT_TXPOWER_BASE(MODE_AM),
		& gtxpower [MODE_AM],	// 16 bit in nvram
		NULL,
		getzerobase, 
	},
	{
		"TXPW TUN", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,
		0, 255, 		/*  */
		RMT_TXPOWER_BASE(MODE_TUNE),
		& gtxpower [MODE_TUNE],	// 16 bit in nvram
		NULL,
		getzerobase, 
	},
	{
		"TXCP SSB", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,
		0, 255, 		/*  */
		RMT_TXPOWER_BASE(MODE_SSB),
		& gtxcompr [MODE_SSB],	// 16 bit in nvram
		NULL,
		getzerobase, 
	},
	{
		"TXCP AM ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,
		0, 255, 		/*  */
		RMT_TXPOWER_BASE(MODE_AM),
		& gtxcompr [MODE_AM],	// 16 bit in nvram
		NULL,
		getzerobase, 
	},
	{
		"TXCP FM ", 7, 0, 0,	ISTEP1,	
		ITEM_VALUE | ITEM_NOINITNVRAM,
		0, 255, 		/*  */
		RMT_TXPOWER_BASE(MODE_NFM),
		& gtxcompr [MODE_NFM],	// 16 bit in nvram
		NULL,
		getzerobase, 
	},

#endif /* CTLREGMODE_RA4YBO || CTLREGMODE_RA4YBO_V1 || CTLREGMODE_RA4YBO_V2 || CTLREGMODE_RA4YBO_V3 || CTLREGMODE_RA4YBO_V3A */
};

/* ������ �� ������ ����� ���� � ������ ����������� ��� ������ */
static uint_fast8_t
ismenusuitable(
	const FLASHMEM struct menudef * mp,
	uint_fast8_t itemmask
	)
{
	return (mp->qspecial & itemmask) != 0;
}

/* ����� ���� ��� ���������� ������ ������� �� (��������������� ����) */
static uint_fast8_t
ismenufilterusb(
	const FLASHMEM struct menudef * mp
	)
{
	return ismenusuitable(mp, ITEM_FILTERU);
}

/* ����� ���� ��� ���������� ������ ������� �� (�������������� ����) */
static uint_fast8_t
ismenufilterlsb(
	const FLASHMEM struct menudef * mp
	)
{
	return ismenusuitable(mp, ITEM_FILTERL);
}

#define MENUROW_COUNT (sizeof menutable / sizeof menutable [0])

/* �������� �������� �� NVRAM � ���������� ���������.
   ��������� �� ��������� �������� ��, �� �������
   ���������� ����������������� ��� ������� ���������.
   see also loadsavedstate().
   */
static void 
//NOINLINEAT
loadsettings(void)
{
	uint_fast8_t i;

	for (i = 0; i < MENUROW_COUNT; ++ i)
	{
		const FLASHMEM struct menudef * const mp = & menutable [i];
		if (ismenusuitable(mp, ITEM_VALUE) && ! ismenusuitable(mp, ITEM_NOINITNVRAM))
		{
			const nvramaddress_t nvram = mp->qnvram;
			const uint_fast16_t bottom = mp->qbottom;
			const uint_fast16_t upper = mp->qupper;
			uint_fast16_t * const pv16 =  mp->qpval16;
			uint_fast8_t * const pv8 = mp->qpval8;

			if (nvram == MENUNONVRAM)
				continue;
			if (pv16 != NULL)
			{
				* pv16 = loadvfy16up(nvram, bottom, upper, * pv16);
			}
			else if (pv8 != NULL)
			{
				* pv8 = loadvfy8up(nvram, bottom, upper, * pv8);
			}
		}
	}
}

/* ��������� �������� ����� �������������� */
static void 
//NOINLINEAT
savemenuvalue(
	const FLASHMEM struct menudef * mp
	)
{
	if (ismenusuitable(mp, ITEM_VALUE))
	{
		const nvramaddress_t nvram = mp->qnvram;
		const uint_fast16_t * const pv16 = mp->qpval16;
		const uint_fast8_t * const pv8 = mp->qpval8;

		if (nvram == MENUNONVRAM)
			return;
		if (pv16 != NULL)
		{
			save_i16(nvram, * pv16);		/* ��������� ����������������� �������� */
		}
		else if (pv8 != NULL)
		{
			save_i8(nvram, * pv8);		/* ��������� ����������������� �������� */
		}
	}
}



/* �������� � NVRAM ���������� �� ���������.
   ��������� �� ��������� �������� ��, �� �������
   ���������� ����������������� ��� ������� ���������.
   */
static void 
defaultsettings(void)
{
	uint_fast8_t i;

	for (i = 0; i < MENUROW_COUNT; ++ i)
	{
		const FLASHMEM struct menudef * const mp = & menutable [i];
		if (! ismenusuitable(mp, ITEM_NOINITNVRAM))
		{
			savemenuvalue(mp);
		}
	}
}



//+++ menu support

// ���������� �� display2.c
// ��� �������������� ���������
void display_menu_lblc3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const FLASHMEM struct menudef * const mp = (const FLASHMEM struct menudef *) pv;
#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */
	char buff [4];
	const uint_fast8_t index = (int) (mp - menutable);
	if (ismenusuitable(mp, ITEM_GROUP))
	{
		display_setcolors(MENUCOLOR, BGCOLOR);
		display_at_P(x, y, PSTR("---"));
		return;
	}

	local_snprintf_P(buff, sizeof buff / sizeof buff [0], index >= 100 ? PSTR("%03d") : PSTR("F%02d"), index);

	display_setcolors(MENUCOLOR, BGCOLOR);
	display_at(x + 0, y, buff);
}

// ���������� �� display2.c
// �������� �������������� ���������
// ���� ������ - ������ �� ����������
void display_menu_lblng(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const FLASHMEM struct menudef * const mp = (const FLASHMEM struct menudef *) pv;
#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */
	if (ismenusuitable(mp, ITEM_VALUE) == 0)
		return;
	display_setcolors(MENUCOLOR, BGCOLOR);
	display_at_P(x, y, mp->qlabel);
}

// ���������� �� display2.c
// �������� �������������� ��������� ��� ������
void display_menu_lblst(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const FLASHMEM struct menudef * const mp = (const FLASHMEM struct menudef *) pv;
#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */
	display_setcolors(MENUCOLOR, BGCOLOR);
	display_at_P(x, y, mp->qlabel);
}

// ���������� �� display2.c
// ������, � ������� ��������� ������������� ��������
void display_menu_group(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const FLASHMEM struct menudef * mp = (const FLASHMEM struct menudef *) pv;
#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */
	while (ismenusuitable(mp, ITEM_GROUP) == 0)
		-- mp;
	display_setcolors(MENUGROUPCOLOR, BGCOLOR);
	display_at_P(x, y, mp->qlabel);
}


// ���������� �� display2.c
// �������� ���������
void display_menu_valxx(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	)
{
	const FLASHMEM struct menudef * const mp = (const FLASHMEM struct menudef *) pv;
	int_fast32_t value;
	const uint_fast8_t rj = mp->qrj;
	const uint_fast8_t width = mp->qwidth;
	const uint_fast8_t comma = mp->qcomma;
	const uint_fast16_t * const pv16 = mp->qpval16;
	const uint_fast8_t * const pv8 = mp->qpval8;

#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */
	if (ismenusuitable(mp, ITEM_VALUE) == 0)
		return;
	// ��������� �������� ��� �����������
	if (ismenufilterlsb(mp))
	{
		const filter_t * const filter = CONTAINING_RECORD(pv16, filter_t, low_or_center);
		value = getlo4baseflt(filter) + * pv16;
	}
	else if (ismenufilterusb(mp))
	{
		const filter_t * const filter = CONTAINING_RECORD(pv16, filter_t, high);
		value = getlo4baseflt(filter) + * pv16;
	}
	else if (pv16 != NULL)
	{
		const int_fast32_t offs = mp->funcoffs();
		value = offs + * pv16;
	}
	else if (pv8 != NULL)
	{
		const int_fast32_t offs = mp->funcoffs();
		value = offs + * pv8;
	}
	else
	{
		value = mp->qbottom;	/* ����� �� ������� ���������� */
	}

	// ����������� ���������, ������������ �� ���������
	switch (rj)
	{
#if WITHTX && WITHIF4DSP
	case RJ_TXAUDIO:
		{
			static const FLASHMEM char msg [] [6] = 
			{
 				"MIKE ",	// BOARD_TXAUDIO_MIKE
 				"LINE ",	// BOARD_TXAUDIO_LINE
#if WITHUSBUAC
				"USB  ",	// BOARD_TXAUDIO_USB
#endif /* WITHUSBUAC */
				"2TONE",	// BOARD_TXAUDIO_2TONE
				"NOISE",	// BOARD_TXAUDIO_NOISE
				"1TONE",	// BOARD_TXAUDIO_1TONE
				"MUTE ",	// BOARD_TXAUDIO_MUTE
			};

			display_menu_string_P(x, y, msg [value], width, comma);
		}
		break;
#endif /* WITHTX && WITHIF4DSP */

#if WITHMODEM

	case RJ_MDMSPEED:
		display_menu_digit(x, y, modembr2int100 [value], width, comma, 0);
		break;

	case RJ_MDMMODE:
		{
			static const FLASHMEM char msg [] [5] = 
			{
 				"BPSK",
				"QPSK",
			};

			display_menu_string_P(x, y, msg [value], width, comma);
		}
		break;

#endif /* WITHMODEM */

#if defined (RTC1_TYPE)
	case RJ_MONTH:
		{
			static const FLASHMEM char months [13] [4] = 
			{
				"JAN",
				"FEB",
				"MAR",
				"APR",
				"MAY",
				"JUN",
				"JUL",
				"AUG",
				"SEP",
				"OCT",
				"NOV",
				"DEC",
			};

			display_menu_string_P(x, y, months [value - mp->qbottom], width, comma);
		}
		break;
#endif /* defined (RTC1_TYPE) */

	case RJ_YES:
		{
			static const FLASHMEM char msg_yes [] = "Yes";
			static const FLASHMEM char msg_no  [] = "No ";

			display_menu_string_P(x, y, value ? msg_yes : msg_no, width, comma);
		}
		break;

	case RJ_ON:
		{
			static const FLASHMEM char msg_on  [] = "On ";
			static const FLASHMEM char msg_off [] = "Off";

			display_menu_string_P(x, y, value ? msg_on : msg_off, width, comma);
		}
		break;

	case RJ_ENCRES:
		display_menu_digit(x, y, encresols [value] * ENCRESSCALE, width, comma, 0);
		break;

#if WITHCAT
	case RJ_CATSPEED:
		display_menu_digit(x, y, catbr2int [value] * BRSCALE, width, comma, 0);
		break;
#endif /* WITHCAT */

#if WITHSUBTONES && WITHTX
	case RJ_SUBTONE:
		display_menu_digit(x, y, gsubtones [value], width, comma, 0);
		break;
#endif /* WITHSUBTONES && WITHTX */

	case RJ_POW2:
		display_menu_digit(x, y, 1UL << value, width, comma, 0);
		break;

#if WITHELKEY
	case RJ_ELKEYMODE:
		{
			/* ����� ������������ ����� - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
			static const FLASHMEM char msg [][4] = 
			{ 
				"ACS", 	// 
				"ELE", 
				"OFF",
				"BUG",
			};

			display_menu_string_P(x, y, msg [value], width, comma);
		}
		break;
#endif /* WITHELKEY */
#if WITHPOWERLPHP
	case RJ_POWER:	/* ���������� �������� HP/LP */
			display_menu_string_P(x, y, pwrmodes [value].label, width, comma);
		break;
#endif /* WITHPOWERLPHP */

	default:
		display_menu_digit(x, y, value, width, comma, rj);
		break;

	}
}

// --- menu support

static uint_fast8_t menulooklast(uint_fast8_t menupos)
{
	const FLASHMEM struct menudef * mp;
	do
	{
		mp = & menutable [++ menupos];
	} while (menupos < MENUROW_COUNT && (mp->qspecial & ITEM_VALUE) != 0);
	return menupos - 1;
}

/* ������ � ����������� ���������. */
static void 
modifysettings(
	uint_fast8_t firstitem, uint_fast8_t lastitem,	/* �������� �� ������ � �� ������ ������ ���� �������� */
	uint_fast8_t itemmask,		/* �� ������ ���� ������� ���� �������� */
	nvramaddress_t posnvram,	/* ��� ��������� ������� ������� */
	uint_fast8_t exitkey		/* �������������� �������, �� ������� ���������� ����� �� ���� �� ������� ���� (��� KBD_CODE_MAX) */
	)
{
	uint_fast8_t menupos = loadvfy8up(posnvram, firstitem, lastitem, firstitem);	/* ��������� �������� ������� */
	const FLASHMEM struct menudef * mp = & menutable [menupos];
	/* ������� ��� ���������� ������ �������� ��� ����� */
	while (! ismenusuitable(mp, itemmask))
	{
		/* ������ �� ������������ ���� ��������� (itemmask) */
		menupos = calc_next(menupos, firstitem, lastitem);
		mp = & menutable [menupos];
	}
#if WITHDEBUG
	debug_printf_P(PSTR("menu: ")); debug_printf_P(mp->qlabel); debug_printf_P(PSTR("\n")); 
#endif /* WITHDEBUG */
	display_menuitemlabel((void *) mp);
	display_menuitemvalue((void *) mp);
	display_redrawbars(1, 1);		/* ���������� ������������ ����� ����������� - ���������� S-����� ��� SWR-����� � volt-�����. */
	encoder_clear();

	for (;;)
	{
		int nrotate;
		uint_fast8_t kbch, kbready;

		processmodem();
		processmessages(& kbch, & kbready, 1);
		processtxrequest();	/* ��������� ���������� ������� �� ��������.	*/
		display_redrawbars(0, 1);		/* ���������� ������������ ����� ����������� - ���������� S-����� ��� SWR-����� � volt-�����. */

#if WITHKEYBOARD

		if (kbready != 0)
		{
			switch (kbch)
			{
			default:
				if (kbch != exitkey)
					continue;
				/* � ������� ������������ - ���������� ��������. */
			case KBD_CODE_DISPMODE:
				/* ����� �� ���� */
				if (posnvram != MENUNONVRAM)
					save_i8(posnvram, menupos);	/* ��������� ����� ������ ����, � ������� �������� */
				encoder_clear();	// �������� ���������� � ��������
				return;

			case KBD_CODE_MENU:
#if ! WITHFLATMENU
				if (ismenusuitable(mp, ITEM_GROUP))
				{
					/* ���� � ������� */
					const uint_fast8_t first = menupos + 1;	/* ��������� �� ������� ������� */
					const uint_fast8_t last = menulooklast(first);

					if (ismenusuitable(& menutable [first], ITEM_VALUE))
					{
					#if defined (RTC1_TYPE)
						getstamprtc();
					#endif /* defined (RTC1_TYPE) */
						modifysettings(first, last, ITEM_VALUE, mp->qnvram, exitkey);

						display2_bgreset();		/* �������� ��� � ����� �������� ������ */
						display_menuitemlabel((void *) mp);
						display_menuitemvalue((void *) mp);
						display_redrawbars(1, 1);		/* ���������� ������������ ����� ����������� - ���������� S-����� ��� SWR-����� � volt-�����. */
					}
				}
				continue;	// ��������� ���������� ����������
#endif /* ! WITHFLATMENU */

			case KBD_CODE_LOCK:
				/* ���������� ���������
					 - �� �������� ���������� ��������� ��������� */
				uif_key_lockencoder();
				display_menuitemlabel((void *) mp);
				continue;	// ��������� ���������� ����������

			case KBD_CODE_BAND_DOWN:
				/* ������� �� ���������� ����� ���� */
				savemenuvalue(mp);		/* ��������� ����������������� �������� */
				do
				{
					/* ������ �� ������������ ���� ��������� (itemmask) */
					menupos = calc_prev(menupos, firstitem, lastitem);
					mp = & menutable [menupos];
				}
				while (! ismenusuitable(mp, itemmask));
				goto menuswitch;

			case KBD_CODE_BAND_UP:
				/* ������� �� ��������� ����� ���� */
				savemenuvalue(mp);		/* ��������� ����������������� �������� */
				do
				{
					/* ���� ���������� ��������� - ���� ������� */
					menupos = calc_next(menupos, firstitem, lastitem);
					mp = & menutable [menupos];
				}
				while (! ismenusuitable(mp, itemmask));

			menuswitch:
#if (NVRAM_TYPE != NVRAM_TYPE_CPUEEPROM)
				if (posnvram != MENUNONVRAM)
					save_i8(posnvram, menupos);	/* ��������� ����� ������ ����, � ������� �������� */
#endif /* (NVRAM_TYPE != NVRAM_TYPE_CPUEEPROM) */
				
				display2_bgreset();
#if WITHDEBUG
				debug_printf_P(PSTR("menu: ")); debug_printf_P(mp->qlabel); debug_printf_P(PSTR("\n")); 
#endif /* WITHDEBUG */
				display_menuitemlabel((void *) mp);
				display_menuitemvalue((void *) mp);
				display_redrawbars(1, 1);		/* ���������� ������������ ����� ����������� - ���������� S-����� ��� SWR-����� � volt-�����. */
				break;
			}
		}
#endif /* WITHKEYBOARD */

#if WITHENCODER
		/* �������������� �������� � ������� �������� ���������. */
		nrotate = getRotateLoRes(ghiresdiv);
		if (lockmode != 0)
			nrotate = 0;	// ignore encoder

		if (nrotate != 0 && ismenusuitable(mp, ITEM_VALUE))
		{
			/* �������������� ���������� */
			const uint_fast16_t step = mp->qistep;
			uint_fast16_t * const pv16 = mp->qpval16;
			uint_fast8_t * const pv8 = mp->qpval8;

			if (nrotate < 0)
			{
				// negative change value
				const uint_fast32_t bottom = mp->qbottom;
				if (pv16 != NULL)
				{
					* pv16 =
						prevfreq(* pv16, * pv16 - (- nrotate * step), step, bottom);
				}
				else
				{
					* pv8 =
						prevfreq(* pv8, * pv8 - (- nrotate * step), step, bottom);
				}
			}
			else
			{
				// positive change value
				const uint_fast32_t upper = mp->qupper;
				if (pv16 != NULL)
				{
					* pv16 =
						nextfreq(* pv16, * pv16 + (nrotate * step), step, upper + (uint_fast32_t) step);
				}
				else
				{
					* pv8 =
						nextfreq(* pv8, * pv8 + (nrotate * step), step, upper + (uint_fast32_t) step);
				}
			}
			/* ���������� ����������� ������ */
			board_wakeup();
			updateboard(1, 0);
			display_menuitemvalue((void *) mp);

#if (NVRAM_TYPE != NVRAM_TYPE_CPUEEPROM)
			savemenuvalue(mp);		/* ��������� ����������������� �������� */
#endif
		}
#endif /* WITHENCODER */
	}
}

// ����� ����������� ������ ���� �� ��������������
static void 
uif_key_click_menubyname(const char * name, uint_fast8_t exitkey)
{
#if WITHAUTOTUNER
	if (reqautotune != 0)
		return;
#endif /* WITHAUTOTUNER */

	uint_fast8_t menupos;

	for (menupos = 0; menupos < MENUROW_COUNT; ++ menupos)
	{
		const FLASHMEM struct menudef * const mp = & menutable [menupos];
		if ((mp->qspecial & ITEM_VALUE) == 0)
			continue;
	#if CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA
		// ��������� ������ � SRAM � FLASH
		const int r = strcmp_P(name, mp->qlabel);
	#else /* CPUSTYLE_ATMEGA */
		const int r = strcmp(name, mp->qlabel);
	#endif /* CPUSTYLE_ATMEGA */
		if (r == 0)
			break;
	}
	if (menupos >= MENUROW_COUNT)
	{
		// �� ����� ����� �����
		return;
	}
	display2_bgreset();

	modifysettings(menupos, menupos, ITEM_VALUE, MENUNONVRAM, exitkey);

	updateboard(1, 0);
	updateboard2();			/* ��������� ��������� � �������� ����� �������. */
	display2_bgreset();		/* �������� ��� � ����� �������� ������ */
}

#else // WITHMENU

static void 
uif_key_click_menubyname(const char * name, uint_fast8_t exitkey)
{
}

#endif // WITHMENU

#if MULTIVFO

static void dispvfocode(
	uint_fast8_t vco,
	uint_fast8_t top
	)
{
	char label [16];

	local_snprintf_P(label, sizeof label / sizeof label [0],
		top ?
			PSTR("VFO #%d HIGH") :
			PSTR("VFO #%d LOW "),
		vco
		);


	uint_fast32_t freq = getvcoranges(vco, top);
	synth_lo1_setfreq(0, freq, getlo1div(gtx));

	display_at(0, 1, label);
	display_menu_digit(0, 0, freq, 9, 3, 0);

}

/* ������ � ������ ��������� VCO */
static void vfoallignment(void)
{
	uint_fast8_t vfo = 0;
	uint_fast8_t top = 0;
	dispvfocode(vfo, top);
	for (;;)
	{	
		uint_fast8_t kbch, kbready;

		processmessages(& kbch, & kbready, 1);
		//display_redrawbars(0, 1);	/* ���������� ������������ ����� ����������� - ���������� S-����� ��� SWR-����� � volt-�����. */

		if (kbready != 0)
		{
			switch (kbch)
			{
			case KBD_CODE_MENU:
			case KBD_CODE_DISPMODE:
				/* ����� �� ������ ��������� */
				return;
			case KBD_CODE_MODE:	/* �� �� �������, ��� � ������������ ������� */
				top = ! top;	// ������������ ������� ��� ������ ���� ���������
				goto menuswitch;


			case KBD_CODE_BAND_DOWN:
				/* ������� �� ���������� ����� ���� */
				vfo = calc_prev(vfo, 0, HYBRID_NVFOS - 1);
				goto menuswitch;

			case KBD_CODE_BAND_UP:
				/* ������� �� ��������� ����� ���� */
				vfo = calc_next(vfo, 0, HYBRID_NVFOS - 1);

			menuswitch:
				dispvfocode(vfo, top);
				break;
			}
		}
	}
	return;
}
#endif // MULTIVFO

#if WITHKEYBOARD
/* ������� ������ - ���� �����-���� �������,
	��������� ���������� ������� */
static uint_fast8_t
process_key_menuset0(uint_fast8_t kbch)
{
	switch (kbch)
	{

#if WITHSPLIT

	case KBD_CODE_SPLIT:
		/* ������������ VFO
			 - �� �������� ���������� ��������� ��������� */
		if (gsplitmode == VFOMODES_VFOINIT)
			uif_key_spliton(0);		// ��������� SPLIT ��� ��������
		else
			uif_key_click_a_ex_b();	// ����� VFO
		return 1;	// ��������� ���������� ����������

	case KBD_CODE_SPLIT_HOLDED:
		/* ����� �� ������ ������������ VFO - ������� � ������� ���������
			 - �� �������� ���������� ��������� ��������� */
		if (gsplitmode == VFOMODES_VFOINIT)
			uif_key_spliton(1);
		else
			uif_key_splitoff();
		return 1;	// ��������� ���������� ����������

#elif WITHSPLITEX

	case KBD_CODE_SPLIT:
		/* ������������ VFO
			 - �� �������� ���������� ��������� ��������� */
		if (gsplitmode == VFOMODES_VFOINIT)
			uif_key_spliton(0);
		else
			uif_key_splitoff();
		return 1;	// ��������� ���������� ����������

	case KBD_CODE_SPLIT_HOLDED:
		/* ������������ VFO
			 - �� �������� ���������� ��������� ��������� */
		uif_key_spliton(1);
		return 1;	// ��������� ���������� ����������

#endif /* WITHSPLIT */

	case KBD_CODE_A_EX_B:
		uif_key_click_a_ex_b();
		return 1;	// ��������� ���������� ����������

	case KBD_CODE_A_EQ_B:
		uif_key_click_b_from_a();
		return 1;	// ��������� ���������� ����������

	case KBD_CODE_BAND_UP:
		/* ������� �� ��������� (� ������� ��������) �������� ��� �� ��� general coverage */
		uif_key_click_bandup();
		return 1;	// ��������� ���������� ����������

	case KBD_CODE_BAND_DOWN:
		/* ������� �� ���������� (� ������� ��������) �������� ��� �� ��� general coverage */
		uif_key_click_banddown();
		return 1;	// ��������� ���������� ����������

#if WITHDIRECTBANDS

	case KBD_CODE_BAND_1M8: 
		uif_key_click_banddjump(1800000L);
		return 1;	// ��������� ���������� ����������
	case KBD_CODE_BAND_3M5: 
		uif_key_click_banddjump(3500000L);
		return 1;	// ��������� ���������� ����������
	case KBD_CODE_BAND_5M3:
		uif_key_click_banddjump(5351500L);
		return 1;	// ��������� ���������� ����������
	case KBD_CODE_BAND_7M0: 
		uif_key_click_banddjump(7000000L);
		return 1;	// ��������� ���������� ����������
	case KBD_CODE_BAND_10M1: 
		uif_key_click_banddjump(10100000L);
		return 1;	// ��������� ���������� ����������
	case KBD_CODE_BAND_14M0: 
		uif_key_click_banddjump(14000000L);
		return 1;	// ��������� ���������� ����������
	case KBD_CODE_BAND_18M0: 
		uif_key_click_banddjump(18068000L);
		return 1;	// ��������� ���������� ����������
	case KBD_CODE_BAND_21M0: 
		uif_key_click_banddjump(21000000L);
		return 1;	// ��������� ���������� ����������
	case KBD_CODE_BAND_24M9: 
		uif_key_click_banddjump(24890000L);
		return 1;	// ��������� ���������� ����������
	case KBD_CODE_BAND_28M0:
		uif_key_click_banddjump(28000000L);
		return 1;	// ��������� ���������� ����������
	case KBD_CODE_BAND_50M0:
		uif_key_click_banddjump(50100000L);
		return 1;	// ��������� ���������� ����������
#endif /* WITHDIRECTBANDS */

	default:
		return 0;	// �� ��������� ���������� ����������
	}
}
#endif /* WITHKEYBOARD */

#if WITHDIRECTFREQENER


static int_fast32_t
getpower10(uint_fast8_t pos)
{
	int_fast32_t v = 1;
	while (pos --)
		v *= 10;

	return v;
}

/* �������� �������� ������� ������� �� ������������. */
static uint_fast8_t
freqvalid(
	int_fast32_t freq,
	uint_fast8_t tx
	)
{
#if LO1FDIV_ADJ
	const int_fast32_t f = freq << lo1powmap [tx]; /* �����. ������� � ������ ������� ���������� */
#else
	const int_fast32_t f = freq << LO1_POWER2; /* �����. ������� � ������ ������� ���������� */
#endif
	if (f < freq)
		return 0;	/* ��������� ������������ ��� ��������� */
	return (f >= TUNE_BOTTOM && f < TUNE_TOP);	/* ������� ������ ����������� ��������� */
}

#endif /* WITHDIRECTFREQENER */

#if WITHKEYBOARD

/* ������� ������ - ���� �����-���� �������, ������� ��� ����������
	��������� ���������� ������� */
static uint_fast8_t
process_key_menuset_common(uint_fast8_t kbch)
{
#if WITHAUTOTUNER
	//const vindex_t b = getvfoindex(bi);
#endif /* WITHAUTOTUNER */
	switch (kbch)
	{
	case KBD_CODE_BW:
#if WITHIF4DSP
		/* ������������ ��������
			 - �� �������� ���������� ��������� ��������� */
		uif_key_changebw();
#else /* WITHIF4DSP */
		/* ������������ ��������
			 - �� �������� ���������� ��������� ��������� */
		uif_key_changefilter();
#endif /* WITHIF4DSP */
		return 1;	/* ������� ��� ���������� */

#if ! WITHAGCMODENONE
	case KBD_CODE_AGC:
		/* AGC mode switch
			 - �� �������� ���������� ��������� ��������� */
		uif_key_click_agcmode();
		return 1;	/* ������� ��� ���������� */
#endif /* ! WITHAGCMODENONE */

#if WITHANTSELECT
	case KBD_CODE_ANTENNA:
		/* Antenna switch
			 - �� �������� ���������� ��������� ��������� */
		uif_key_click_antenna();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHANTSELECT */

#if WITHUSEAUDIOREC
	case KBD_CODE_RECORDTOGGLE:	// SD CARD audio recording
		sdcardtoggle();
		return 1;	/* ������� ��� ���������� */
	case KBD_CODE_RECORD_HOLDED:
		sdcardformat();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHUSEAUDIOREC */

#if WITHENCODER2
	case KBD_ENC2_PRESS:
		uif_encoder2_press();
		return 1;
	case KBD_ENC2_HOLD:
		uif_encoder2_hold();
		return 1;
#endif /* WITHENCODER2 */

#if WITHTX

	case KBD_CODE_MOX:
		/* ��������� ����� ��������� ��� ����/�������� */
		uif_key_tuneoff();
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_TXTUNE:
		/* �������� ����� ��������� */
		uif_key_tune();
		return 1;	/* ������� ��� ���������� */

#if WITHAUTOTUNER
	case KBD_CODE_ATUSTART:
		uif_key_atunerstart();
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_ATUBYPASS:
		uif_key_bypasstoggle();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHAUTOTUNER */

#if WITHVOX
	case KBD_CODE_VOXTOGGLE:
		uif_key_voxtoggle();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHVOX */

#if WITHIF4DSP && WITHUSBUAC && WITHDATAMODE
	case KBD_CODE_DATATOGGLE:
		uif_key_click_datamode();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHIF4DSP && WITHUSBUAC && WITHDATAMODE */

#endif /* WITHTX */

#if WITHUSEDUALWATCH
	case KBD_CODE_DWATCHHOLD:
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_DWATCHTOGGLE:
		uif_key_mainsubrx();
		return 1;	/* ������� ��� ���������� */

#endif /* WITHUSEDUALWATCH */

#if WITHAMHIGHKBDADJ
	case KBD_CODE_AMBANDPASSUP:
		uif_key_click_amfmbandpassup();
		return 1;	/* ������� ��� ���������� */
	case KBD_CODE_AMBANDPASSDOWN:
		uif_key_click_amfmbandpassdown();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHAMHIGHKBDADJ */

#if WITHSPKMUTE
	case KBD_CODE_LDSPTGL:
		uif_key_loudsp();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHSPKMUTE */

#if CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V3
	case KBD_CODE_AFNARROW:
		/*  - �� �������� ���������� ��������� ��������� */
		uif_key_affilter();
		return 1;	/* ������� ��� ���������� */

#endif /* CTLSTYLE_RA4YBO || CTLSTYLE_RA4YBO_V3 */

#if CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 || CTLSTYLE_RA4YBO_V3
	case KBD_CODE_USER1:
		uif_key_user1();
		return 1;	/* ������� ��� ���������� */

#if 0
	case KBD_CODE_USER2: // rec
		uif_key_user2();
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_USER3:	// play
		uif_key_user3();
		return 1;	/* ������� ��� ���������� */
#endif

	case KBD_CODE_USER4:
		uif_key_user4();
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_USER5:
		uif_key_user5();
		return 1;	/* ������� ��� ���������� */

#endif /* CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2*/

#if KEYBSTYLE_RA4YBO_AM0

	case KBD_CODE_USER1:
		uif_key_user1();
		return 1;	/* ������� ��� ���������� */
	case KBD_CODE_USER2:
		uif_key_user2();
		return 1;	/* ������� ��� ���������� */
	case KBD_CODE_USER3:
		uif_key_user3();
		return 1;	/* ������� ��� ���������� */
#endif /* KEYBSTYLE_RA4YBO_AM0 */

#if WITHELKEY

#if ! WITHPOTWPM
	case KBD_CODE_CWSPEEDDOWN:
			if (elkeywpm > CWWPMMIN)
				elkeywpm -= 1;
			updateboard(1, 0);
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_CWSPEEDUP:
			if (elkeywpm < CWWPMMAX)
				elkeywpm += 1;
			updateboard(1, 0);
		return 1;	/* ������� ��� ���������� */

#endif /* ! WITHPOTWPM */

#endif /* WITHELKEY */

#if ELKEY328
	case KBD_CODE_VIBROCTL:
		/* ������������ ������ �����������  */
		uif_key_click_vibroplex();
		return 1;	// ��������� ���������� ����������

#endif /* ELKEY328 */

	case KBD_CODE_ATT:
		/* ������������ ������ �����������  */
		uif_key_click_attenuator();
		return 1;	/* ������� ��� ���������� */

#if ! WITHONEATTONEAMP
	case KBD_CODE_PAMP:
		/* ������������ ������ �������������  */
		uif_key_click_pamp();
		return 1;	/* ������� ��� ���������� */
#endif /* ! WITHONEATTONEAMP */

	case KBD_CODE_MODE:
		/* ������� �� "�������" ������� - ������� ������� */
		/* switch to next moderow */
		uif_key_click_moderow();
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_MODEMOD:
		/* ������� �� "������" ������� - ���������� ������� */
		// step to next modecol
		uif_key_hold_modecol();
		return 1;	/* ������� ��� ���������� */

#if WITHDIRECTBANDS

	case KBD_CODE_MODE_0:
		/* ������� �� "�������" ������� - ������� ������� */
		/* switch to next moderow */
		uif_key_click_moderows(0);
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_MODE_1:
		/* ������� �� "�������" ������� - ������� ������� */
		/* switch to next moderow */
		uif_key_click_moderows(1);
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_MODE_2:
		/* ������� �� "�������" ������� - ������� ������� */
		/* switch to next moderow */
		uif_key_click_moderows(2);
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_MODE_3:
		/* ������� �� "�������" ������� - ������� ������� */
		/* switch to next moderow */
		uif_key_click_moderows(3);
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_MODEMOD_0:
		/* ������� �� "������" ������� - ���������� ������� */
		// step to next modecol
		uif_key_hold_modecols(0);
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_MODEMOD_1:
		/* ������� �� "������" ������� - ���������� ������� */
		// step to next modecol
		uif_key_hold_modecols(1);
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_MODEMOD_2:
		/* ������� �� "������" ������� - ���������� ������� */
		// step to next modecol
		uif_key_hold_modecols(2);
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_MODEMOD_3:
		/* ������� �� "������" ������� - ���������� ������� */
		// step to next modecol
		uif_key_hold_modecols(3);
		return 1;	/* ������� ��� ���������� */

#endif /* WITHDIRECTBANDS */

	case KBD_CODE_LOCK:
		/* ���������� ���������
			 - �� �������� ���������� ��������� ��������� */
		uif_key_lockencoder();
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_LOCK_HOLDED:
#if WITHLCDBACKLIGHT
		{
			dimmmode = calc_next(dimmmode, 0, 1);
			save_i8(RMT_DIMMMODE_BASE, dimmmode);
			display2_bgreset();
			display_redrawfreqmodesbars(0);			/* ���������� ������� - ��, ������� ������� */
			updateboard(1, 0);
		}
#endif /* WITHLCDBACKLIGHT */
		return 1;	/* ������� ��� ���������� */

#if WITHBCBANDS
	case KBD_CODE_GEN_HAM:
		uif_key_genham();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHBCBANDS */


#if WITHUSEFAST
	case KBD_CODE_USEFAST:
		uif_key_usefast();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHUSEFAST */
#if WITHPOWERLPHP
	case KBD_CODE_PWRTOGGLE:
		/* ������������ ������ �������� (��� sw2012sf) */
		uif_key_click_pwr();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHPOWERLPHP */

#if WITHNOTCHONOFF
	case KBD_CODE_NOTCHTOGGLE:
		/* ������������ NOTCH ������� */
		uif_key_click_notch();
		return 1;	/* ������� ��� ���������� */
#endif /* WITHNOTCHONOFF */

#if WITHNOTCHFREQ
	case KBD_CODE_NOTCHTOGGLE:
		/* ������������ NOTCH ������� */
		uif_key_click_notch();
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_NOTCHFREQ:
		uif_key_click_menubyname("NTCH FRQ", KBD_CODE_NOTCHTOGGLE);
		return 1;
#endif /* WITHNOTCHFREQ */


#if WITHAUTOTUNER && KEYB_UA3DKC
	case KBD_CODE_TUNERTYPE:	// ������������ ���� ������������ ����������
		tunertype = calc_next(tunertype, 0, KSCH_COUNT - 1);
		save_i8(offsetof(struct nvmap, bands[b].tunertype), tunertype);
		updateboard_tuner();
		return 1;	// ��������� ���������� ����������

	case KBD_CODE_CAP_UP:	// ���������� �������
		// todo: �������� ���� ���������� �������
		tunercap = calc_next(tunercap, CMIN, CMAX);
		save_i8(offsetof(struct nvmap, bands[b].tunercap), tunercap);
		updateboard_tuner();
		return 1;	// ��������� ���������� ����������

	case KBD_CODE_CAP_DOWN:	// ���������� �������
		// todo: �������� ���� ���������� �������
		tunercap = calc_prev(tunercap, CMIN, CMAX);
		save_i8(offsetof(struct nvmap, bands[b].tunercap), tunercap);
		updateboard_tuner();
		return 1;	// ��������� ���������� ����������

	case KBD_CODE_IND_UP:	// ���������� �������������
		// todo: �������� ���� ���������� �������
		tunerind = calc_next(tunerind, LMIN, LMAX);
		save_i8(offsetof(struct nvmap, bands[b].tunerind), tunerind);
		updateboard_tuner();
		return 1;	// ��������� ���������� ����������

	case KBD_CODE_IND_DOWN:	// ���������� �������������
		// todo: �������� ���� ���������� �������
		tunerind = calc_prev(tunerind, LMIN, LMAX);
		save_i8(offsetof(struct nvmap, bands[b].tunerind), tunerind);
		updateboard_tuner();
		return 1;	// ��������� ���������� ����������
#endif /* WITHAUTOTUNER && KEYB_UA3DKC */

	default:
		return 0;	/* ������� �� ���������� */
	}
}


/* ������� ������ - ���� �����-���� �������,
	��������� ���������� ������� � ��������� ���������� */
static uint_fast8_t
processkeyboard(uint_fast8_t kbch)
{
	const uint_fast8_t exitkey = getexitkey();	/* ��� ������� ��������� � menu - �������������� ��� ��� ������. */

#if WITHDIRECTFREQENER
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const int DISPLAY_LEFTBLINKPOS = fullwidth - 1;

	if (editfreqmode != 0)
	{
		const uint_fast8_t c = kbd_getnumpad(kbch);
		if (kbch == KBD_CODE_ENTERFREQDONE)
		{
			editfreqmode = 0;
			return 1;
		}
		if (c == '#' && blinkpos < DISPLAY_LEFTBLINKPOS)
		{
			blinkpos += 1;	/* ������������ �� ���� ������� ����� */
			updateboard(1, 0);
			return 1;
		}
		if (c >= '0' && c <= '9')
		{
			const int_fast32_t m = getpower10(blinkpos + rj);
			const int_fast32_t m10 = m * 10;
			editfreq = editfreq / m10 * m10 + (c - '0') * m;
			if (blinkpos != 0)
				-- blinkpos;	/* ������������ �� ���� ������� ������ */
			else if (freqvalid(editfreq, gtx))
			{
				const uint_fast8_t bi = getbankindex_tx(gtx);
				vindex_t vi = getvfoindex(bi);
				gfreqs [bi] = editfreq;
				editfreqmode = 0;
				savebandfreq(vi, bi);		/* ���������� ������� � ������� VFO */
				updateboard(1, 0);
			}
			else
			{
				/* ����� � ���������� ��������� */
				blinkpos = DISPLAY_LEFTBLINKPOS;		/* ������� ������� */
				editfreqmode = 1;
				editfreq = gfreqs [getbankindex_tx(gtx)];
			}
			return 1;
		}
	}
	else if (kbch == KBD_CODE_ENTERFREQ)
	{
		blinkpos = DISPLAY_LEFTBLINKPOS;		/* ������� ������� */
		editfreqmode = 1;
		editfreq = gfreqs [getbankindex_tx(gtx)];
		return 1;
	}
#endif /* WITHDIRECTFREQENER */

	switch (kbch)
	{
	case KBD_CODE_MENU:
		/* ���� � ����
			 - �� �������� ���������� ��������� ��������� */
#if WITHMENU
	#if WITHAUTOTUNER
		if (reqautotune != 0)
			return 1;
	#endif /* WITHAUTOTUNER */
		display2_bgreset();
	#if WITHFLATMENU
		modifysettings(0, MENUROW_COUNT - 1, ITEM_VALUE, RMT_GROUP_BASE, exitkey);	/* ����� ������ ���������� ��� �������������� */
	#else /* WITHFLATMENU */
		modifysettings(0, MENUROW_COUNT - 1, ITEM_GROUP, RMT_GROUP_BASE, exitkey);	/* ����� ������ ���������� ��� �������������� */
	#endif /* WITHFLATMENU */
		
		updateboard(1, 0);
		updateboard2();			/* ��������� ��������� � �������� ����� �������. */
		display2_bgreset();		/* �������� ��� � ����� �������� ������ */
#endif //WITHMENU
		return 1;	// ��������� ���������� ����������

	case KBD_CODE_DISPMODE:
		if (display_getpagesmax() != 0)
		{
			/* �������������� ������� ������ - "Fn"
				 - �� �������� ���������� ��������� ��������� */
			menuset = calc_next(menuset, 0, display_getpagesmax());
			save_i8(RMT_MENUSET_BASE, menuset);
			display2_bgreset();
			return 1;	// ��������� ���������� ����������
		}
		return 0;	// �� ��������� ���������� ����������

#if WITHMENU
#if KEYBSTYLE_SW2013SF_US2IT || KEYBSTYLE_SW2012CN5_UY5UM || WITHIF4DSP
	case KBD_CODE_MENU_CWSPEED:
		uif_key_click_menubyname("CW SPEED", KBD_CODE_MENU_CWSPEED);
		return 1;	/* ������� ��� ���������� */

	case KBD_CODE_IFSHIFT:
		uif_key_click_menubyname("IF SHIFT", KBD_CODE_IFSHIFT);
		return 1;	/* ������� ��� ���������� */
#elif KEYB_RA4YBO_V1
	case KBD_CODE_IFSHIFT:
		uif_key_click_menubyname("IF SHIFT", KBD_CODE_IFSHIFT);
		return 1;	/* ������� ��� ���������� */
#elif KEYB_RA4YBO_V3
	case KBD_CODE_IFSHIFT:
		uif_key_click_menubyname("IF SHIFT", KBD_CODE_IFSHIFT);
		return 1;	/* ������� ��� ���������� */
#endif /* KEYBSTYLE_SW2013SF_US2IT || WITHIF4DSP */
#endif /* WITHMENU */

	default:
		break;
	}

	uint_fast8_t processed = 0;
#if 1
	processed = process_key_menuset0(kbch);
#else
	switch (menuset)
	{
	default:
	case DISPMODE_MAIN:
		processed = process_key_menuset0(kbch);
		break;
#if DSTYLE_SWITCHMODES2		// �� ������ ���������� ������������� ����� ������������ ����������
	case DISPMODE_ALT:
		processed = process_key_menuset0(kbch);
		break;
#endif /* DSTYLE_SWITCHMODES2 */
	}
#endif
	if (processed != 0)
		return 1;
	if (process_key_menuset_common(kbch))
			return 1;	/* ������� ��� ���������� */
	return 0;	// �� ��������� ���������� ����������
}

#endif /* WITHKEYBOARD */


#if WITHDEBUG

int dbg_getchar(char * r)
{
	return HARDWARE_DEBUG_GETCHAR(r);
}

int dbg_putchar(int c)
{
	if (c == '\n')
		dbg_putchar('\r');

	while (HARDWARE_DEBUG_PUTCHAR(c) == 0)
		;
	return c;
}

int dbg_puts_impl_P(const FLASHMEM char * s)
{
	char c;
	while ((c = * s ++) != '\0')
	{
		dbg_putchar(c);
	}
	return 0;
}

int dbg_puts_impl(const char * s)
{
	char c;
	while ((c = * s ++) != '\0')
	{
		dbg_putchar(c);
	}
	return 0;
}

#else /* WITHDEBUG */

int dbg_putchar(int c)
{
#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM0 || CPUSTYLE_ARM_CM7
	//ITM_SendChar(c);
#endif /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM0 || CPUSTYLE_ARM_CM7 */
	return c;
}

int dbg_puts_impl_P(const FLASHMEM char * s)
{
	(void) s;
	return 0;
}
int dbg_puts_impl(const char * s)
{
	(void) s;
	return 0;
}
#endif /* WITHDEBUG */

/* ���������� ��� ����������� �����������. */
static void 
lowinitialize(void)
{
	board_beep_initialize();
	//hardware_cw_diagnostics_noirq(1, 0, 1);	// 'K'
#if WITHDEBUG

	//HARDWARE_DEBUG_INITIALIZE();
	//HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);


	dbg_puts_impl_P(PSTR("Version " __DATE__ " " __TIME__ " 1 debug session starts.\n"));
	// ������ �������������� ������� ��� ������������ �������� ��������, ���� ��������� �������������
	//for (;;)
	//	hardware_putchar(0xff);
	// ������������ ����� � �������� ��������
	for (;0;)
	{
		char c;
		if (dbg_getchar(& c))
		{
			if (c == 0x1b)
				break;
			dbg_putchar(c);
		}
	}
#endif /* WITHDEBUG */

#if WITHCAT
	HARDWARE_CAT_INITIALIZE();
#endif /* WITHCAT */

#if WITHNMEA
	HARDWARE_NMEA_INITIALIZE();
	HARDWARE_NMEA_SET_SPEED(115200L);
	HARDWARE_NMEA_ENABLERX(1);
#endif /* WITHNMEA */

#if WITHMODEM
	HARDWARE_MODEM_INITIALIZE();
	HARDWARE_MODEM_SET_SPEED(19200L);
	HARDWARE_MODEM_ENABLERX(1);
#endif /* WITHMODEM */

	board_init_io();		/* ������������� ����������� � SPI, I2C, �������� FPGA */
	cpu_initdone();			/* ������ init (� ������� ����� ����� ��� �������� � FPGA) ������ �� ����� */
	display_hardware_initialize();

	buffers_initialize();	// ������������� ������� ������� - � ��� ����� ������� ���������
	//hardware_cw_diagnostics_noirq(1, 0, 0);	// 'D'
#if WITHUSBHW
	board_usb_initialize();		// USB device and host support
#endif /* WITHUSBHW */
#if WITHENCODER
	hardware_encoder_initialize();	//  todo: ����������� - ����� ��������� ���� �� board_init_io - ����� �� ���������� ��� Cortex-A9.
#endif /* WITHENCODER */
	hardware_timer_initialize(TICKS_FREQUENCY);

#if WITHLFM
	hardware_lfm_timer_initialize();
	hardware_lfm_setupdatefreq(20);
#endif /* WITHLFM */
#if WITHENCODER
	encoder_initialize();
#endif /* WITHENCODER */
#if WITHELKEY
	elkey_initialize();
#endif /* WITHELKEY */
	seq_initialize();
	vox_initialize();		/* ���������� ������ �������� ������������ ����-�������� */
#if WITHSDHCHW
	hardware_sdhost_initialize();	/* ���� ���� ���������� ���������� SD CARD */
#endif /* WITHSDHCHW */

#if WITHCAT
	processcat_initialize();
#endif
#if WITHKEYBOARD
	kbd_initialize();
#endif /* WITHKEYBOARD */

#if WITHDEBUG
	dbg_puts_impl_P(PSTR("Most of hardware initialized.\n"));
#endif

#if 0
	{
		const spitarget_t cs = targetfpga1;
		debug_printf_P(PSTR("targetfpga1=%04lX\n"), (unsigned long) cs);
		dbg_puts_impl_P(PSTR("SPI send test started.\n"));
		// ������������ �������� �������� �� SPI. �� SCK ������ ���� ������� SPISPEED
		for (;;)
		{
			static uint_fast8_t v = 0x00;
			v = ~ v;
			spi_select(cs, SPIC_MODE3);
			spi_progval8_p1(cs, v);
			spi_progval8_p2(cs, v);
			spi_progval8_p2(cs, v);
			spi_progval8_p2(cs, v);
			spi_progval8_p2(cs, v);
			spi_progval8_p2(cs, v);
			spi_progval8_p2(cs, v);
			spi_progval8_p2(cs, v);
			spi_progval8_p2(cs, v);
			spi_progval8_p2(cs, v);
			spi_progval8_p2(cs, v);
			spi_progval8_p2(cs, v);
			spi_complete(cs);
			spi_unselect(cs);
		}
	}
#endif
	//for (;;) ;
	//hardware_cw_diagnostics_noirq(1, 1, 0);	// 'S'
	//board_testsound_enable(0);	// ��������� 1 ��� �� ������������
}

/* ���������� ��� ����������� �����������. */
static void initialize2(void)
{
	uint_fast8_t mclearnvram;


	//hardware_cw_diagnostics(0, 1, 0);	// 'D'

	debug_printf_P(PSTR("initialize2() started.\n"));

	display_reset();
	display_initialize();
	display2_bgreset();

	if (kbd_is_tready() == 0)
	{
		local_delay_ms(20);
		local_delay_ms(20);
		local_delay_ms(20);

		if (kbd_is_tready() == 0)
		{
			static const FLASHMEM char msg  [] = "KBD fault";

			display_at_P(0, 0, msg);
			debug_printf_P(PSTR("KBD fault\n"));
			for (;;)
				;
		}
	}
	debug_printf_P(PSTR("KBD ok\n"));

#if defined(NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING)

	//debug_printf_P(PSTR("initialize2(): NVRAM initialization started.\n"));

	mclearnvram = kbd_get_ishold(KIF_ERASE) != 0;
	//extmenu = kbd_get_ishold(KIF_EXTMENU);
	
	// ��������� ������ - ����� �������������.
	if (sizeof (struct nvmap) > (NVRAM_END + 1))
	{
		// � ������ ���������� ���������� ������� ���� ����� � ���������� �� ��������������
		static const FLASHMEM char msg  [] = "TOO LARGE nvmap";
		void wrong_NVRAM_END(void);

		display_menu_digit(0, 0, sizeof (struct nvmap), 9, 0, 0);
		display_at_P(0, 1, msg);

		wrong_NVRAM_END();
		//hardware_cw_diagnostics(0, 0, 0);	// 'S'
		for (;;)
			;
	}
	else
	{
		// ���������� ������ ������� ������, ���������� ��� �������� ������������
		/*
		static const FLASHMEM char msg  [] = "nvmap size";

		display_menu_digit(sizeof (struct nvmap), 9, 0, 0);
		display_at_P(0, 0, msg);


		unsigned i;
		for (i = 0; i < 50; ++ i)
		{
			local_delay_ms(20);
		}
		*/
	}

	nvram_initialize();	// write enable all nvram memory regions

#endif /* defined(NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING) */

	//debug_printf_P(PSTR("initialize2(): NVRAM initialization passed.\n"));

#if HARDWARE_IGNORENONVRAM

#elif NVRAM_TYPE == NVRAM_TYPE_FM25XXXX

	//debug_printf_P(PSTR("initialize2(): NVRAM autodetection start.\n"));

	uint_fast8_t ab = 0;
	const uint_fast8_t ABMAX = 2;
	// �������� ��������� ������� ��������� ab
	for (ab = 0; ab < ABMAX; ++ ab)
	{
		nvram_set_abytes(ab);
		if (verifynvramsignature() == 0)
			break;
	}
	if (ab >= ABMAX)
	{
		mclearnvram = 2;	/* �� ��� ����� ab ��������� �� ������� */
	}

	if (mclearnvram)
	{
		/* ���� ������ �� �������� ������ - �������� ������������� �� ���������. */
		if (mclearnvram == 1)
		{
			uint_fast8_t kbch;

			display_at_P(0, 0, PSTR("ERASE: Press SPL"));

			for (;;)
			{
				while (kbd_scan(& kbch) == 0)
					;
				if (kbch == KBD_CODE_SPLIT)
					break;
			}
			display2_bgreset();
		}
		/* ���� ������ �� �������� ������ - ����� ���������� ��� ab */
		for (ab = 0; ab < ABMAX; ++ ab)
		{
			nvram_set_abytes(ab);
			/* �������� ���� ������ */
			uint_least16_t i;
			for (i = 0; i < sizeof (struct nvmap); ++ i)
				save_i8(i, 0xFF);

			initnvrampattern();

			if (verifynvrampattern() == 0)
				break;						// �����������!
		}
		if (ab >= ABMAX)
		{
			// � ������ ����������� ����������� NVRAM ��������
			debug_printf_P(PSTR("initialize2(): NVRAM initialization: wrong NVRAM pattern in any address sizes.\n"));

			display_menu_digit(0, 0, NVRAM_END + 1, 9, 0, 0);
			display_at_P(0, 1, PSTR("NVRAM fault"));
			debug_printf_P(PSTR("NVRAM fault1\n"));
			for (;;)
				;
		}
#if WITHMENU
		defaultsettings();		/* �������� � nvram ��������� �� ��������� */
#endif //WITHMENU
		initnvramsignature();
		//extmenu = 1;	/* ����� �������� ���������� ����� - ��� ������������ �������� ��� ������ */
	}

#else /* NVRAM_TYPE == NVRAM_TYPE_FM25XXXX */

	//debug_printf_P(PSTR("initialize2(): NVRAM(BKPSRAM/CPU EEPROM/SPI MEMORY) initialization: verify NVRAM signature.\n"));

	if (verifynvramsignature())
		mclearnvram = 2;

	//debug_printf_P(PSTR("initialize2(): NVRAM initialization: work on NVRAM signature, mclearnvram=%d\n"), mclearnvram);

	if (mclearnvram != 0)
	{
		if (mclearnvram == 1)
		{
			uint_fast8_t kbch;

			display_at_P(0, 0, PSTR("ERASE: Press SPL"));

			for (;;)
			{
				while (kbd_scan(& kbch) == 0)
					;
				if (kbch == KBD_CODE_SPLIT)
					break;
			}
			display2_bgreset();
		}
		
		//debug_printf_P(PSTR("initialize2(): NVRAM initialization: erase NVRAM.\n"));
		/* �������� ���� ������ */
		uint_least16_t i;
		for (i = 0; i < sizeof (struct nvmap); ++ i)
			save_i8(i, 0xFF);

		//debug_printf_P(PSTR("initialize2(): NVRAM initialization: write NVRAM pattern.\n"));
		initnvrampattern();
		//debug_printf_P(PSTR("initialize2(): NVRAM initialization: verify NVRAM pattern.\n"));

		if (verifynvrampattern())
		{
			debug_printf_P(PSTR("initialize2(): NVRAM initialization: wrong NVRAM pattern.\n"));
			// ��������� ������ ��� ���������� ���������
			// � ������ ����������� ����������� NVRAM ��������

			display_menu_digit(0, 1, NVRAM_END + 1, 9, 0, 0);
			display_at_P(0, 1, PSTR("NVRAM fault"));
			for (;;)
				;
		}

#if WITHMENU
		defaultsettings();		/* �������� � nvram ��������� �� ��������� */
#endif //WITHMENU
		//debug_printf_P(PSTR("initialize2(): NVRAM initialization: write NVRAM signature.\n"));
		initnvramsignature();
		//extmenu = 1;	/* ����� �������� ���������� ����� - ��� ������������ �������� ��� ������ */
	}
	//blinkmain();

#endif /* NVRAM_TYPE == NVRAM_TYPE_FM25XXXX */
#if WITHDEBUG
	dbg_puts_impl_P(PSTR("initialize2() finished.\n"));
#endif
}

/* ���������� ��� ����������� �����������. */
static void 
hamradio_initialize(void)
{
	/* NVRAM ��� ����� ������������ */
#if WITHMENU && ! HARDWARE_IGNORENONVRAM
	loadsettings();		/* �������� ���� ��������� �� nvram */
#endif /* WITHMENU && ! HARDWARE_IGNORENONVRAM */
	//extmenu = extmenu || alignmode;
	loadsavedstate();	// split, lock, s-meter display, see also loadsettings().
	loadnewband(getvfoindex(1), 1);	/* �������� ���������� ������������ ��������� - ������ VFO ��� MEMxx */
	loadnewband(getvfoindex(0), 0);	/* �������� ���������� ������������ ��������� - ������ VFO ��� MEMxx */

	synthcalc_init();
	bandf_calc_initialize();
#if CTLSTYLE_SW2011ALL
	board_set_bandfonhpf(bandf_calc(14000000L));	/* � SW20xx ������� (��������), � �������� ���������� ��� �� ����� �������� */
	board_set_bandfonuhf(bandf_calc(85000000L));
#endif /* CTLSTYLE_SW2011ALL */
#if CTLREGMODE_RA4YBO_V1 || CTLREGMODE_RA4YBO_V2 || CTLREGMODE_RA4YBO_V3 || CTLREGMODE_RA4YBO_V3A
	board_set_bandfonuhf(bandf_calc(111000000L));
#endif /* CTLREGMODE_RA4YBO_V1 || CTLREGMODE_RA4YBO_V2 || CTLREGMODE_RA4YBO_V3 || CTLREGMODE_RA4YBO_V3A */
#if defined (XVTR1_TYPE)
	//board_set_bandfxvrtr(bandf_calc(R820T_IFFREQ))	// ���� �������� ������������� ��� �� ��� ������������
#endif /* defined (XVTR1_TYPE) */
	board_init_chips();	// ���������������� ���� ��������� �����������.

#if WITHUSESDCARD
	sdcardhw_initialize();
#endif /* WITHUSESDCARD */
#if WITHUSEAUDIOREC
	sdcardinitialize();			// ������� state machine � ��������� ���������
#endif /* WITHUSEAUDIOREC */

#if WITHMODEM
	modem_initialze();
#endif /* WITHMODEM */

#if WITHINTEGRATEDDSP	/* � ��������� �������� ������������� � ������ DSP �����. */
	dsp_initialize();		// �������� ��������� ����������������
#endif /* WITHINTEGRATEDDSP */

#if WITHI2SHW
	hardware_audiocodec_enable();	// ��������� � �� ������
#endif /* WITHI2SHW */

#if WITHSAI1HW
	hardware_fpgacodec_enable();	// ��������� � �� ������
#endif /* WITHSAI1HW */

#if WITHSAI2HW
	hardware_fpgaspectrum_enable();	// ��������� � ��������� ������ � �������
#endif /* WITHSAI1HW */

	board_set_i2s_enable(1);	// ��������� FPGA ������������ �������� ������� ��� ������� � ������������ I2S
	board_update();

#if WITHUSBHW
	board_usb_activate();		// USB device and host start
#endif /* WITHUSBHW */

#if WITHSPISLAVE
	hardware_spi_slave_enable(SPIC_MODE3);
#endif /* WITHSPISLAVE */

	board_init_chips2();	// ���������������� ������� ��� ���������� �������� �������
}

#if WITHSPISLAVE
// �������� ���� ��������� ��� ������ � ������ ������������ ����� DSP
static void
dspcontrol_mainloop(void)
{
	debug_printf_P(PSTR("dspcontrol_mainloop started.\n"));

	board_update();
#if 0
	// ���� ������������������.
	// ��� ����������� ����������� ������� �������� ������� �� ������ ����������
	// � ���������� � ���, ��� ����� ��� ����������� �����������.
	disableIRQ();
	for (;;)
	{
		local_delay_ms(50);
		arm_hardware_pioa_outputs(1, 1);
		local_delay_ms(50);
		arm_hardware_pioa_outputs(1, 0);
	}
#endif

	for (;;)
	{
		board_update();	// ��� ������������ ��������� (��������� ���������� �� ����� ����������� ������ �� HOST-����������� �������������� �����.
		__WFI();
	}
}
#endif /* WITHSPISLAVE */

#if WITHOPERA4BEACON

#if 1



#define NFREQS 1
#define FREQTEMPO	512	// 128 ms ��������
// . ������� - Dial USB + 1500 ��
// 3. �������� �� ��������� Opera05: 0.128 s  �� ���� ��������.
//3570     CH1TST
static const char msg1 [] = "11011001101010100110101010100110010101010110101010011001100110010101101001101010101010100110011010011001011010100101100101011010101010011010100110010110100101101010011010011001011001011010011010011001011010010110100101100101101001101001010";

static unsigned long freqs [NFREQS] = 
{
	3548500,
};

static const char * const msgs [NFREQS] =
{
	msg1,
};

#elif 1

#define NFREQS 4
#define FREQTEMPO	128	// 128 ms ��������
// . ������� - Dial USB + 1500 ��
// 3. �������� �� ��������� Opera05: 0.128 s  �� ���� ��������.
//3570     CH1TST
static const char msg1 [] = "11011010100110010101011001010101101001101010011010100110100110010110011010101010100101100110100110101001011010100110100110010110011010010110101010101001100110010110010101011010101010101001010101010101010110011010100110011010100110101010011";
//5358    CH2TST
static const char msg2 [] = "11011010101001011010100101100101010101101010010101010101010101101010011010101001101001010110100101010110011010100101100110011001100110101010101010011001011001011010100101011010101010100110011001100101101001011010101001101001101001011001011";
//7037    CH3TST
static const char msg3 [] = "11011010101001100101010110010101011001101010011010011001101010011001101010101001011010011001100110011010011010100101011001011010101001101010101010011001011010011001011001101010101010010101011010101001010110011010101010011010010110011010101";
//10134   CH4TST
static const char msg4 [] = "11011010101010101010010101100110100101101010011001101001011010101001011010101001100110101010011001100110011010100101010101010101011010101010101010010101010110101001011010011010101010011001011010010110100101011010101001100101010110010110011";
//14063   CH5TST
//static const char msg5 [] = "11011010101001010101100110100110101001101010010101100110101010011010011010101001101001101001101010101001011010100110011001100101101010100110101010010110011010100110011010011010101010011010101001011001101010011010101010010101101001101010101";

static unsigned long freqs [NFREQS] = 
{
	(3570 * 1000uL) + 1500,
	(5359 * 1000uL) + 1500,
	(7036 * 1000uL) + 1500,
	(10133 * 1000uL) + 1500,
	//(14063 * 1000uL) + 1500,
};

static const char * const msgs [NFREQS] =
{
	msg1,
	msg2,
	msg3,
	msg4,
	//msg5,
};
#endif

static int getkeydn(int msg, int pos)
{
	return msgs [msg][pos] == '1';
}

static void setmsgkeys(int pos)
{
	int msg;
	for (msg = 0; msg < NFREQS; ++ msg)
	{
		const uint_fast32_t freq = getkeydn(msg, pos) * freqs [msg];
		static uint_fast32_t lastfreq = UINT32_MAX;
		if (lastfreq != freq)
		{
			lastfreq = freq;
			synth_lo1_setfrequ(msg, freq, getlo1div(gtx));
		}
	}
}

static volatile int flag128;

void spool_0p128(void)
{
	flag128 = 1;
}

static int getevent128ms(void)
{
	//local_delay_ms(FREQTEMPO);
	//return 1;

	disableIRQ();
	int f = flag128;
	flag128 = 0;
	enableIRQ();
	return f;
}

static void dumbtx(int tx)
{
	if (gtx == tx)
		return;
	seq_txrequest(0, tx);	// press PTT
	while (seq_get_txstate() != tx)
		;
	gtx = tx;
#if WITHDSPEXTDDC
	gdactest = tx;
#endif /* WITHDSPEXTDDC */
	updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */

	seq_ask_txstate(tx);

}

static void
hamradio_mainloop_OPERA4(void)
{
	int periodS = 2uL * 60;
	int msgpos;
	const int msglen = strlen(msgs [0]);
	long ticks0 = ((int64_t) periodS * 1000) / FREQTEMPO - msglen - 2;
	if (ticks0 < 0)
		ticks0 = 5120;

	ticks0 = 5120;
	gtx = 0;

	updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
	hardware_elkey_set_speed128(1000, FREQTEMPO);	// ������ 0.128 �������
	
	for (msgpos = 0;;)
	{
		while (getevent128ms() == 0)
			;
		dumbtx(1);
		setmsgkeys(msgpos);
		msgpos = ((msgpos + 1) >= msglen) ? 0 : (msgpos + 1);
		if (msgpos == 0)
		{
			while (getevent128ms() == 0)
				;
			setmsgkeys(msglen);	// ��������� ������������ - �������� �� ������� 0 ��� ������
			dumbtx(0);
			long ticks = ticks0;
			while (ticks --)
			{
				while (getevent128ms() == 0)
					;
			}

		}
	}
}

#endif /* WITHOPERA4BEACON */

#if CTLSTYLE_V1H

static void df(
	uint_fast32_t f
	)
{
	char s [21];
	local_snprintf_P(s, 21, PSTR("%10lu.%1lu"), f / 1000, (f % 10) / 100);
	display_at(0, 0, s);
}

static void
hamradio_mainloop_vh1(void)
{
	const uint_fast8_t tx = 0;
	uint_fast32_t dfreq = 435000000UL;
	synth_lo1_setfrequ(0, dfreq, getlo1div(tx));
	df(dfreq);
	for (;;)
	{
		int nrotate = 0;
		int jumpsize = 1;
		int gstep = 12500;

		uint_fast8_t key;
		const uint_fast8_t repeat = kbd_scan(& key);
		if (repeat != 0)
		{
			switch (key)
			{
			case KBD_CODE_4:
				jumpsize = 20;
				nrotate = -1;
				break;
			case KBD_CODE_5:
				nrotate = -1;
				break;
			case KBD_CODE_6:
				nrotate = +1;
				break;
			case KBD_CODE_7:
				jumpsize = 20;
				nrotate = +1;
				break;
			}
		}

		if (nrotate < 0)
		{
			/* ������� "����" */
			//const uint_fast32_t lowfreq = bandsmap [b].bottom;
			dfreq = prevfreq(dfreq, dfreq - ((uint_fast32_t) gstep * jumpsize * - nrotate), gstep, RFSGTUNE_BOTTOM);

			//gfreqs [bi] = prevfreq(gfreqs [bi], gfreqs [bi] - (jumpsize * - nrotate), gstep, TUNE_BOTTOM);

			df(dfreq);
			synth_lo1_setfrequ(0, dfreq, getlo1div(tx));
			continue;
		}
		if (nrotate > 0)
		{
			/* ������� "�����" */
			//const uint_fast32_t topfreq = bandsmap [b].top;
			dfreq = nextfreq(dfreq, dfreq + ((uint_fast32_t) gstep * jumpsize * nrotate), gstep, RFSGTUNE_TOP);

			//gfreqs [bi] = nextfreq(gfreqs [bi], gfreqs [bi] + (jumpsize * nrotate), gstep, TUNE_TOP);

			df(dfreq);
			synth_lo1_setfrequ(0, dfreq, getlo1div(tx));
			continue;
		}
	}
}
#endif /* CTLSTYLE_V1H */

// ������������� ������ ��������� ������
static void hamradio_tune_initialize(void)
{
}

// ������ � ������ ��������� ������
static STTE_t hamradio_tune_step(void)
{
	return STTE_OK;
}

// ������������� ������ ��������� ����
static void hamradio_menu_initialize(void)
{
}

// ������ � ������ ��������� ����
static STTE_t hamradio_menu_step(void)
{
	return STTE_OK;
}

enum
{
	//STHTL_IDLE,
	STHRL_RXTX,
	STHRL_RXTX_FQCHANGED,
	STHRL_TUNE,
	STHRL_MENU,
	//
	STHRL_count
};

static uint_fast8_t sthrl = STHRL_RXTX;

// ������������� ������� ������ ���������
static void hamradio_main_initialize(void)
{
	// ��������� �������������
	
	seq_purge();

#if FQMODEL_GEN500
	gfreqs [getbankindex_raw(0)] = 434085900UL;
	gfreqs [getbankindex_raw(1] = 434085900UL;
#endif /* FQMODEL_GEN500 */

#if WITHUSEAUDIOREC
	sdcardbgprocess();
#endif /* WITHUSEAUDIOREC */
	directctlupdate(0);		/* ���������� ��������� �������� (� ������� �����������) ����� ������������ */
	updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) - ����� ������ */
	updateboard2();			/* ��������� ��������� � �������� ����� �������. */
	display2_bgreset();

#if 0 && MODEL_MAXLAB
	// ������������ ��������� ��� MAXLAB
	{
		unsigned n;
		uint_fast32_t f;

		f = 5500000;
		for (n = 0; n < 1000; n += 50)
			synth_lo1_setfreq(f + n. getlo1div(tx));
		f = 5999000;
		for (n = 0; n < 1000; n += 50)
			synth_lo1_setfreq(f + n, getlo1div(tx));
		//for (;;)
		//	;
	}
#endif

#if FQMODEL_GEN500
	{
		const spitarget_t target = targetpll2;		/* addressing to chip */
		
		void prog_cmx992_print(spitarget_t target), prog_cmx992_initialize(spitarget_t target);

		//prog_cmx992_initialize(target);
		for (;;)
			prog_cmx992_print(target);

		display_redrawfreqmodesbars(0);	// ���������� ������� - ��, ������� �������

		//prog_pll1_init();
		synth_lo1_setfreq((434085900UL - 10700000UL) / 3. getlo1div(gtx));
		for (;;)
			;
	}
#endif /* FQMODEL_GEN500 */


	encoder_clear();

	/* ��������� ����������� */
	display_redrawfreqmodesbars(0);	// ���������� ������� - ��, ������� �������

}

// ������ � ������� ������ ���������
static STTE_t
hamradio_main_step(void)
{
	//void r820t_spool(void);
	//r820t_spool();
	uint_fast8_t kbch, kbready;

	processmessages(& kbch, & kbready, 0);

	switch (sthrl)
	{
	case STHRL_MENU:
		if (hamradio_menu_step() == STTE_OK)
			sthrl = STHRL_RXTX;
		break;

	case STHRL_TUNE:
		if (hamradio_tune_step() == STTE_OK)
			sthrl = STHRL_RXTX;
		break;

	case STHRL_RXTX_FQCHANGED:
			/* �������� �������� ��������� - ���� ���� ��������� ������� - ���������� ����������� */
			if (display_refreshenabled_freqs())
			{
				processtxrequest();	/* ��������� ���������� ������� �� ��������.	*/
				#if 1//WITHSPLIT
					const uint_fast8_t bi_main = getbankindex_ab_fordisplay(0);		/* ��������� ������ ������ ����� ���������� */
					const uint_fast8_t bi_sub = getbankindex_ab_fordisplay(1);		/* ��������� ������ ������ ����� ���������� */
				#elif WITHSPLITEX
					const uint_fast8_t bi_main = getbankindex_ab(0);		/* ��������� ������ ������ ����� ���������� */
					const uint_fast8_t bi_sub = getbankindex_ab(1);		/* ��������� ������ ������ ����� ���������� */
				#endif /* WITHSPLIT, WITHSPLITEX */
			/* � ������ ���������� ������ ���������������� - ������� �� ���������� (����� ��� ������). */

	#if (NVRAM_TYPE != NVRAM_TYPE_CPUEEPROM)
				savebandfreq(getvfoindex(bi_main), bi_main);		/* ���������� ������� � ������� VFO */
				savebandfreq(getvfoindex(bi_sub), bi_sub);		/* ���������� ������� � ������� VFO */
	#endif
				sthrl = STHRL_RXTX;

				display_freqpair();
				display_refreshperformed_freqs();
				board_wakeup();
				break;
			}
			// ������������� ������

	case STHRL_RXTX:
		// ������ � �������������
		{
			processtxrequest();	/* ��������� ���������� ������� �� ��������.	*/
			#if 1//WITHSPLIT
				const uint_fast8_t bi_main = getbankindex_ab_fordisplay(0);		/* ��������� ������ ������ ����� ���������� */
				const uint_fast8_t bi_sub = getbankindex_ab_fordisplay(1);		/* ��������� ������ ������ ����� ���������� */
			#elif WITHSPLITEX
				const uint_fast8_t bi_main = getbankindex_ab(0);		/* ��������� ������ ������ ����� ���������� */
				const uint_fast8_t bi_sub = getbankindex_ab(1);		/* ��������� ������ ������ ����� ���������� */
			#endif /* WITHSPLIT, WITHSPLITEX */
			#if WITHAUTOTUNER
			if (reqautotune != 0 && gtx != 0)
			{
				/* ����� �� ������� �� ������������� �������-�� ������������� �� �������� */
				auto_tune();
				reqautotune = 0;
				updateboard(1, 0);
			}
			#endif /* WITHAUTOTUNER */

			display_redrawbars(0, 0);		/* ���������� ������������ ����� ����������� - ���������� S-����� ��� SWR-����� � volt-�����. */

	#if WITHLFM && defined (LO1MODE_DIRECT)
			if (lfmmode && spool_lfm_enable)
			{
				testlfm();
			}
	#endif
			if (alignmode)
			{
	#if MULTIVFO
				// ���� � ����� ��������� ����� ������� ����������
				display2_bgreset();
				vfoallignment();
				display2_bgreset();
				display_redrawfreqmodesbars(0);			/* ���������� ������� - ��, ������� ������� */
				updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) */
	#endif // MULTIVFO
				alignmode = 0;	// � nvram �������� ��-0
			}
	#if WITHUSEAUDIOREC
			if (recmode)
			{
				recmode = 0;
				sdcardrecord();
			}
	#endif /* WITHUSEAUDIOREC */

			uint_fast8_t jumpsize;
			uint_fast8_t jumpsize2;
			int_least16_t nrotate;
			int_least16_t nrotate2;

			/* ������� �� ������� - ��� ������� �� gstep */
			#if WITHBBOX && defined (WITHBBOXFREQ)
				nrotate = 0;	// ignore encoder
				nrotate2 = 0;	// ignore encoder
			#else
				nrotate = getRotateHiRes(& jumpsize, ghiresdiv * gencderate);
				nrotate2 = getRotateHiRes2(& jumpsize2);
			#endif

			if (uif_encoder2_rotate(nrotate2))
			{
				nrotate2 = 0;
				display_redrawfreqmodesbars(0);			/* ���������� ������� - ��, ������� ������� */
			}
	#if WITHKEYBOARD
			if (kbready != 0)
			{
				if (processkeyboard(kbch))
				{
					/* ���������� ���������� ��� ���������� ��������� ��������� */
					encoder_clear();				/* ��� ��������� ���������� ���� ���������� ���������� ���������� */
					display_redrawfreqmodesbars(0);			/* ���������� ������� - ��, ������� ������� */
				} // end keyboard processing
			}
	#endif /* WITHKEYBOARD */
			if (processmodem())
			{
				/* ���������� ���������� ��� ���������� ��������� ��������� */
				display_redrawfreqmodesbars(0);			/* ���������� ������� - ��, ������� ������� */
			} // end keyboard processing

			//auto int marker;
			//debug_printf_P(PSTR("M0:@%p %02x %08lx!\n"), & marker, INTC.ICCRPR, __get_CPSR());

		
			if (lockmode == 0)
			{
				uint_fast8_t freqchanged = 0;

				/* ��������� ������������ ���������� ��������� �� ��������� */
				if (nrotate < 0)
				{
					/* �������� A: ������� "����" */
					//const uint_fast32_t lowfreq = bandsmap [b].bottom;
					gfreqs [bi_main] = prevfreq(gfreqs [bi_main], gfreqs [bi_main] - ((uint_fast32_t) gstep * jumpsize * - nrotate), gstep, tune_bottom(bi_main));
					//gfreqs [bi_main] = prevfreq(gfreqs [bi_main], gfreqs [bi_main] - (jumpsize * - nrotate), gstep, TUNE_BOTTOM);
					freqchanged = 1;
				}
				else if (nrotate > 0)
				{
					/* �������� A: ������� "�����" */
					//const uint_fast32_t topfreq = bandsmap [b].top;
					gfreqs [bi_main] = nextfreq(gfreqs [bi_main], gfreqs [bi_main] + ((uint_fast32_t) gstep * jumpsize * nrotate), gstep, tune_top(bi_main));
					//gfreqs [bi_main] = nextfreq(gfreqs [bi_main], gfreqs [bi_main] + (jumpsize * nrotate), gstep, TUNE_TOP);
					freqchanged = 1;
				}

				if (nrotate2 < 0)
				{
					/* �������� B: ������� "����" */
					//const uint_fast32_t lowfreq = bandsmap [b].bottom;
					gfreqs [bi_sub] = prevfreq(gfreqs [bi_sub], gfreqs [bi_sub] - ((uint_fast32_t) gstep * jumpsize2 * - nrotate2), gstep, tune_bottom(bi_sub));
					//gfreqs [bi_sub] = prevfreq(gfreqs [bi_sub], gfreqs [bi_sub] - (jumpsize2 * - nrotate2), gstep, TUNE_BOTTOM);
					freqchanged = 1;
				}
				else if (nrotate2 > 0)
				{
					/* �������� B: ������� "�����" */
					//const uint_fast32_t topfreq = bandsmap [b].top;
					gfreqs [bi_sub] = nextfreq(gfreqs [bi_sub], gfreqs [bi_sub] + ((uint_fast32_t) gstep * jumpsize2 * nrotate2), gstep, tune_top(bi_sub));
					//gfreqs [bi_sub] = nextfreq(gfreqs [bi_sub], gfreqs [bi_sub] + (jumpsize2 * nrotate2), gstep, TUNE_TOP);
					freqchanged = 1;
				}

				if (freqchanged != 0)
				{
					// ����������� �� �������� ���������� ������� ��� �������� � ����
					sthrl = STHRL_RXTX_FQCHANGED;
					updateboard(0, 0);	/* ��������� ������������� - ��� ����� ������ ������ */
				}
			}
		}
		break;

	default:
		break;
	}
	return STTE_OK;
}
// �������� ���� ��������� ��� ������ � ������ ������������� ��������
static void
hamradio_mainloop(void)
{
	hamradio_main_initialize();
	for (;;)
		hamradio_main_step();
}

#if 0

static volatile uint16_t f255, f255cnt, f255period;

void beacon_255(void)
{
	if (++ f255cnt == f255period)
	{
		f255cnt = 0;
		f255 = 1;
	}
}

static const FLASHMEM char textbeacon [] =
	"00"
	"1011001101010100110010110100101011001010110101010011010011001100"
	"1010110011010101010101001101001011010010110101001011001011001100"
	"1010110011010011001011010010110100110100101100101100101101001101"
	"01010011001100101101001011001010101101010011010"
	"00"
	;

static unsigned textbeaconindex = 0;

static void ff256cycle(unsigned long f, unsigned period)
{
	f255period = period;
	f255cnt = 0;

	synth_lo1_setfreq(f, 1);
	for (;;)
	{
		while (f255 == 0)
			;
		f255 = 0;
		if (textbeacon [textbeaconindex] == '\0')
		{
			textbeaconindex = 0;
			return;
		}
		prog_dds1_setlevel(100 * (textbeacon [textbeaconindex ++] != '0'));
	}

}

struct band
{
	unsigned long freq;
	unsigned period;
};

static void
hamradio_mainloop_beacon(void)
{
	uint8_t ifreq;
	static const struct band ffs [] =
	{
		{ 3548500L + 50,	256, },	
		{ 7039500L + 105, 	256, },	//14063710L,
		{ 10136500L + 140, 	256, },	//14063710L,
		{ 14063500L + 210, 	256, },	//14063710L,
		{ 18106500L + 270, 	256, },	//14063710L,
		{ 21075500L + 315, 	256, },	//14063710L,
		{ 24926500L + 390, 	256, },	
		{ 28071500L + 420, 	256, },	
	};

	hardware_timer_initialize(1000);
	updateboard(1, 1);	/* ������ ������������� (��� ����� ����� ������) - ����� ������ */
	for (ifreq = 0; ; )
	{
		ff256cycle(ffs [ifreq].freq, ffs [ifreq].period);
		if (++ ifreq >= (sizeof ffs / sizeof ffs [0]))
			ifreq = 0;
	}
}

#endif

#if 0 && WITHDEBUG

static void local_gets(char * buff, size_t len)
{
	size_t pos = 0;

	for (;;)
	{
		char c;
		if (dbg_getchar(& c))
		{
			if (c == '\r')
			{
				dbg_putchar('\n');
				buff [pos] = '\0';
				return;
			}
			if ((pos + 1) >= len)
				continue;
			if (isprint((unsigned char) c))
			{
				buff [pos ++] = c;
				dbg_putchar(c);
				continue;
			}
			if (pos != 0 && c == '\b')
			{
				debug_printf_P(PSTR("\b \b"));
				-- pos;
				continue;
			}
		}
	}
}

static void siggen_mainloop(void)
{

	debug_printf_P(PSTR("RF Signal generator\n"));
	uint_fast8_t tx = 0;
	// signal-generator tests
	board_set_attvalue(0);
	updateboard(1, 0);
	for (;;)
	{
		debug_printf_P(PSTR("Enter tx=%d, command (a#/g/n):\n"), tx);
		char buff [132];
		local_gets(buff, sizeof buff / sizeof buff [0]);
		char * cp = buff;
		while (isspace((unsigned char) * cp))
			++ cp;
		switch (tolower((unsigned char) * cp))
		{
		case 'a':
			// set att value
			++ cp;
			unsigned long value = strtoul(cp, NULL, 10);
			debug_printf_P(PSTR("RFSG ATT value: %lu\n"), value);
			if (value < 63)
			{
				board_set_attvalue(value);
				updateboard(1, 0);
			}
			break;
		case 'g':
			// generaton on
			debug_printf_P(PSTR("RFSG output ON\n"));
			tx = 1;
			updateboard(1, 0);
			break;
		case 'n':
			// generator off
			debug_printf_P(PSTR("RFSG output OFF\n"));
			tx = 0;
			updateboard(1, 0);
			break;
		}
	}
}
#endif

/* ������� ������� ��������� */
int 
//__attribute__ ((used))
main(void)
{
#if WITHDEBUG

	HARDWARE_DEBUG_INITIALIZE();
	HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);

#endif /* WITHDEBUG */

	lowtests();		/* ������� ������������, ���������� �� ������������� ��������� */

	global_disableIRQ();
	cpu_initialize();		// � ������ ARM - ������������� ���������� � ������������, AVR - ������ JTAG
	lowinitialize();	/* ���������� ��� ����������� �����������. */
	global_enableIRQ();
	midtests();
	initialize2();	/* ���������� ��� ����������� �����������. */
	hamradio_initialize();
	hightests();		/* ������������ ��� ������������ ���������� */

#if WITHOPERA4BEACON
	hamradio_mainloop_OPERA4();
#elif 0
	siggen_mainloop();
#elif 0
	hamradio_mainloop_beacon();
#elif WITHSPISLAVE
	dspcontrol_mainloop();
#elif CTLSTYLE_V1H
	hamradio_mainloop_vh1();
#else /* WITHSPISLAVE */
	hamradio_mainloop();
#endif /* WITHSPISLAVE */
	return 0;
}
