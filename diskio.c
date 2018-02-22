/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* ��������� �� ���������� ������� ������ � ������� */
#include "formats.h"	/* sprintf() replacement */
#include <ctype.h>
#include <string.h>

#if WITHUSESDCARD

#include "board.h"
#include "sdcard.h"
#include "ff.h"	
#include "diskio.h"		/* FatFs lower layer API */

#include "display.h"	/* ���������� ������� ��������� ������� ������� */
#include "audio.h"	

static const struct drvfunc * const drvfuncs [] =
{
#if WITHUSESDCARD && WITHSDHCHW
	& SD_drvfunc,
#endif /* WITHUSESDCARD */
#if WITHUSESDCARD && ! WITHSDHCHW
	& MMC_drvfunc,
#endif /* WITHUSESDCARD && ! WITHSDHCHW */
#if WITHUSEUSBFLASH
	& USBH_drvfunc,
#endif /* WITHUSEUSBFLASH */
};
/*
	TODO:

	If the card responds to CMD8, the response of ACMD41 includes the CCS field information. CCS is
	valid when the card returns ready (the busy bit is set to 1). CCS=0 means that the card is SDSC.
	CCS=1 means that the card is SDHC or SDXC.

 */


#if _MULTI_PARTITION	/* Volume - Partition resolution table */
PARTITION VolToPart[] = {
	{0, 0},	/* "0:" <== Disk# 0, auto detect */
	{1, 0},	/* "1:" <== Disk# 1, auto detect */
	{2, 0},	/* "2:" <== Disk# 2, auto detect */
	{3, 1},	/* "3:" <== Disk# 3, 1st partition */
	{3, 2},	/* "4:" <== Disk# 3, 2nd partition */
	{3, 3},	/* "5:" <== Disk# 3, 3rd partition */
	{4, 0},	/* "6:" <== Disk# 4, auto detect */
	{5, 0}	/* "7:" <== Disk# 5, auto detect */
};
#endif

/* Check physical drive status */
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	if (drv < (sizeof drvfuncs / sizeof drvfuncs [0]))
		return drvfuncs [drv]->Initialize(drv);	// detect media
	return STA_NODISK;
}

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	if (drv < (sizeof drvfuncs / sizeof drvfuncs [0]))
		return drvfuncs [drv]->Status(drv);
	return STA_NODISK;
}

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read */
)
{
	if (drv < (sizeof drvfuncs / sizeof drvfuncs [0]))
		return drvfuncs [drv]->Disk_read(drv, buff, sector, count);
	return STA_NODISK;
}

DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write */
)
{
	if (drv < (sizeof drvfuncs / sizeof drvfuncs [0]))
		return drvfuncs [drv]->Disk_write(drv, buff, sector, count);
	return STA_NODISK;
}

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	if (drv < (sizeof drvfuncs / sizeof drvfuncs [0]))
	{
		switch (ctrl)
		{
		case CTRL_SYNC:
			return drvfuncs [drv]->Sync(drv);

		// for _USE_MKFS
		case GET_SECTOR_COUNT:
			return drvfuncs [drv]->Get_Sector_Count(drv, buff);

		// for _USE_MKFS
		case GET_BLOCK_SIZE:
			return drvfuncs [drv]->Get_Block_Size(drv, buff);

		case GET_SECTOR_SIZE:
			* (WORD *) buff = 512;
			return RES_OK;

		}
		debug_printf_P(PSTR("Unsupported ioctl: ctrl = %u\n"), ctrl);
		return RES_PARERR;
	}
	return STA_NODISK;
}


/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/

DWORD get_fattime (void)
{
	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, secounds;

	board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);

	return	  ((DWORD)(year - 1980) << 25)	/* Year = 2012 */
			| ((DWORD)month << 21)				/* Month = 1 */
			| ((DWORD)day << 16)				/* Day_m = 1*/
			| ((DWORD)hour << 11)				/* Hour = 0 */
			| ((DWORD)minute << 5)				/* Min = 0 */
			| ((DWORD)secounds >> 1);				/* Sec = 0 */
}

#endif /* WITHUSESDCARD */

#if WITHUSEAUDIOREC
///////////////////////////////////////////////////

static RAMNOINIT_D1 FIL wav_file;			/* ��������� ��������� ����� - ������ ����������� � Cortex-M4 CCM */
static FSIZE_t wav_lengthpos_riff;	/* position for write length at RIFF header */
static FSIZE_t wav_lengthpos_data;	/* position for write length at data subchunk*/
static unsigned long wave_num_bytes;
static const unsigned int bytes_per_sample = 2;	/* 2: 16-bit samples */
#if WITHUSEAUDIOREC2CH
static const unsigned int num_channels = 2;	/* 2: stereo */
#else /* WITHUSEAUDIOREC2CH */
static const unsigned int num_channels = 1;	/* 1: monoaural */
#endif /* WITHUSEAUDIOREC2CH */

#if WITHUSEAUDIORECCLASSIC
	// ����������� ������
	static const unsigned int DATACHUNKSTARTOFFSET = 0x0024;		// miltibloak write ������������ ����� ��� (3-1-3-1-3-1-3-1-3-1-1-1-3-1-3-1)
#else
	// � �������������� ������� ������ (�� ����� ������������ ������������)
	static const unsigned int DATACHUNKSTARTOFFSET = 4096 - 8;	// 4-4-4-4-4-4-4
#endif

/* make_wav.c
 * Creates a WAV file from an array of ints.
 * Output is monophonic, signed 16-bit samples
 * copyright
 * Fri Jun 18 16:36:23 PDT 2010 Kevin Karplus
 * Creative Commons license Attribution-NonCommercial
 *  http://creativecommons.org/licenses/by-nc/3.0/
 */
 
/* information about the WAV file format from
 
http://ccrma.stanford.edu/courses/422/projects/WaveFormat/
 
 */

 
static FRESULT write_little_endian(unsigned int word, int num_bytes)
{
#if 1
	// for little-endian machines
	UINT bw;
	return f_write(& wav_file, & word, num_bytes, & bw) != FR_OK || bw != num_bytes;
#else
	// machine-independent version
    while (num_bytes>0)
    {   unsigned char buf = word & 0xff;
        fwrite(& buf, 1, 1, wav_file);
        num_bytes --;
		word >>= 8;
    }
#endif
}
/*
static FRESULT write_wav_sample(int data)
{
	unsigned i;
	for (i = 0; i < num_channels; ++ i)
	{
		FRESULT rc;
		rc = write_little_endian(data, bytes_per_sample);
		if (rc != FR_OK)
			return rc;
	}
	wave_num_bytes += bytes_per_sample;
	return FR_OK;
}
*/
/*
static void write_wav_samplestereo(int ldata, int rdata)
{
	unsigned i;
	for (i = 0; i < num_channels; ++ i)
	{
		switch (i)
		{
		case 0:
			write_little_endian(ldata, bytes_per_sample);
			break;
		case 1:
			write_little_endian(rdata, bytes_per_sample);
			break;
		default:
			write_little_endian(0, bytes_per_sample);
			break;
		}
	}
	wave_num_bytes += bytes_per_sample;
}
*/

#if 0
// Winrad wav chunk (from Winrad sources)
struct auxihdr     // used by SpectraVue in WAV files
{
//	char  	   chunkID[4];	          // ="auxi" (chunk rfspace)
//	long  	   chunkSize;	          // lunghezza del chunk
	SYSTEMTIME StartTime;
	SYSTEMTIME StopTime;
	DWORD CenterFreq; //receiver center frequency
	DWORD ADFrequency; //A/D sample frequency before downsampling
	DWORD IFFrequency; //IF freq if an external down converter is used
	DWORD Bandwidth; //displayable BW if you want to limit the display to less than Nyquist band
	DWORD IQOffset; //DC offset of the I and Q channels in 1/1000's of a count
	DWORD Unused2;
	DWORD Unused3;
	DWORD Unused4;
	DWORD Unused5;
	char  nextfilename[96];
};

// Complete WAV file header
struct WavHdr 
{
    char  _RIFF[4]; // "RIFF"
    DWORD FileLen;  // length of all data after this (FileLength - 8)
    
    char _WAVE[4];  // "WAVE"
    
    char _fmt[4];        // "fmt "
    DWORD FmtLen;        // length of the next item (sizeof(WAVEFORMATEX))
    WAVEFORMATEX Format; // wave format description   
    
    char _auxi[4];       // "auxi"    
    DWORD AuxiLen;       // length of the next item (sizeof(struct auxihdr))
    struct auxihdr Auxi; // RF setup description

    char  _data[4];  // "data"
    DWORD DataLen;   // length of the next data (FileLength - sizeof(struct WavHdr))
    
};

// Complete WAV file header for 24-bit files
struct WavHdr24 
{
    char  _RIFF[4]; // "RIFF"
    DWORD FileLen;  // length of all data after this (FileLength - 8)
    
    char _WAVE[4];  // "WAVE"
    
    char _fmt[4];        // "fmt "
    DWORD FmtLen;        // length of the next item (sizeof(WAVEFORMATEXTENSIBLE))
    WAVEFORMATEXTENSIBLE Format; // wave format description   
    
    char _auxi[4];       // "auxi"    
    DWORD AuxiLen;       // length of the next item (sizeof(struct auxihdr))
    struct auxihdr Auxi; // RF setup description

    char  _data[4];  // "data"
    DWORD DataLen;   // length of the next data (FileLength - sizeof(struct WavHdr24) - ListBufLen)
    
};
#endif
static FRESULT write_wav_header(const char * filename, unsigned int sample_rate)
{
    //unsigned int byte_rate = sample_rate * num_channels * bytes_per_sample;
 
 	wave_num_bytes = 0;

	FRESULT rc;				/* Result code */

	do 
	{
		rc = f_open(& wav_file, filename, FA_WRITE | FA_CREATE_ALWAYS);
		if (rc != FR_OK)
			break;

		/* write RIFF header */
		UINT bw;
		/* offs 0x0000 */ rc = f_write(& wav_file, "RIFF", 4, & bw);
		if (rc != FR_OK)
			break;
		wav_lengthpos_riff = f_tell(& wav_file);
		/* offs 0x0004 */ rc = write_little_endian(0, 4);	// write palceholder - remaining length after this header
 		if (rc != FR_OK)
			break;
		/* offs 0x0008 */ rc = f_write(& wav_file, "WAVE", 4, & bw);
		if (rc != FR_OK || bw != 4)
			break;

		/* write fmt  subchunk */
 		/* offs 0x000c */ rc = f_write(& wav_file, "fmt ", 4, & bw);
		if (rc != FR_OK || bw != 4)
			break;
		/* offs 0x0010 */ rc = write_little_endian(DATACHUNKSTARTOFFSET - 0x14, 4);   /* SubChunk1Size is 16 - remaining length after this header */
		if (rc != FR_OK)
			break;
		/* ��� ������ ������ ��� ����� � SubChunk1Size */
		/* offs 0x0014 */ rc = write_little_endian(0x01, 2);    /* wFormatTag PCM is format 1 */
		if (rc != FR_OK)
			break;
		/* offs 0x0016 */ rc = write_little_endian(num_channels, 2);	/* wChannels */
		if (rc != FR_OK)
			break;
		/* offs 0x0018 */ rc = write_little_endian(sample_rate, 4);		/* dwSamplesPerSec */
		if (rc != FR_OK)
			break;
		/* offs 0x001c */ rc = write_little_endian(sample_rate * num_channels * bytes_per_sample, 4); /* dwAvgBytesPerSec - byte rate */
		if (rc != FR_OK)
			break;
		/* offs 0x0020 */ rc = write_little_endian(num_channels * bytes_per_sample, 2);  /* wBlockAlign - block align */
		if (rc != FR_OK)
			break;
		/* offs 0x0022 */ rc = write_little_endian(8 * bytes_per_sample, 2);  /* PCM format specific data: wBitsPerSample - bits/sample */
		if (rc != FR_OK)
			break;
 
		/* write data subchunk */
 		/* offs 0x0024 */  rc = f_lseek(& wav_file, DATACHUNKSTARTOFFSET);
		if (rc != FR_OK)
			break;
		/* ��� ����� ������ ��� ����� � SubChunk1Size */
		rc = f_write(& wav_file, "data", 4, & bw);
		if (rc != FR_OK || bw != 4)
			break;
		wav_lengthpos_data = f_tell(& wav_file);
		rc = write_little_endian(0, 4);	// write palceholder - remaining length after this header
		if (rc != FR_OK)
			break;

	} while (0);
	return rc;
}

static FRESULT write_wav_resync(void)
{
	FRESULT rc;				/* Result code */

	do 
	{
		FSIZE_t wav_pos = f_tell(& wav_file);
		/* update data subchunk */
		rc = f_lseek(& wav_file, wav_lengthpos_data);
		if (rc != FR_OK)
			break;
		rc = write_little_endian(wave_num_bytes, 4);
		if (rc != FR_OK)
			break;
		/* update RIFF header */
		rc = f_lseek(& wav_file, wav_lengthpos_riff);
		if (rc != FR_OK)
			break;
		rc = write_little_endian(DATACHUNKSTARTOFFSET + wave_num_bytes, 4);
		if (rc != FR_OK)
			break;

		rc = f_lseek(& wav_file, wav_pos);
		if (rc != FR_OK)
			break;
		rc = f_sync(& wav_file);
		if (rc != FR_OK)
			break;
	} while (0);
	return rc;
}

static FRESULT write_wav_tail(void)
{
 	FRESULT rc;				/* Result code */
	do 
	{
		rc = write_wav_resync();
		if (rc != FR_OK)
			break;
		rc = f_close(& wav_file);
		if (rc != FR_OK)
			break;
	} while (0);
	return rc;
}

///////////////////////////////////////////
//
// See also: http://en.wikipedia.org/wiki/RF64

static uint_fast32_t wave_irecorded;
static RAMNOINIT_D1 FATFS wave_Fatfs;		/* File system object  - ������ ����������� � Cortex-M4 CCM */

static uint_fast8_t waveUnmount(void)
{
	FRESULT rc;				/* Result code */

	rc = f_mount(NULL, "", 0);
	return rc != FR_OK;
}

/* Register volume work area (never fails) */
static uint_fast8_t waveMount(void)
{
	FRESULT rc;				/* Result code */

	rc = f_mount(& wave_Fatfs, "", 0);
	return rc != FR_OK;
}

// �������� ������
// 1 - ��������
static uint_fast8_t wave_startrecording(void)
{
	FRESULT rc;				/* Result code */
	char fname [_MAX_LFN + 1];

#if defined (RTC1_TYPE)

	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, secounds;
	static unsigned long ver;

	board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);

	local_snprintf_P(fname, sizeof fname / sizeof fname [0],
		PSTR("rec_%lu_%04d-%02d-%02d_%02d%02d%02d_%lu.wav"),
		(hamradio_get_freq_rx() + 500) / 1000uL,	// ������� � ��������� �� ��������
		year, month, day,
		hour, minute, secounds,
		++ ver
		);

#else /* defined (RTC1_TYPE) */

	static uint_fast32_t rnd;
	static unsigned long ver;

	if (rnd == 0)
		rnd = hardware_get_random();

	local_snprintf_P(fname, sizeof fname / sizeof fname [0],
		PSTR("rec_%lu_%08lX_%lu.wav"),
		(hamradio_get_freq_rx() + 500) / 1000uL,	// ������� � ��������� �� ��������
		rnd,
		++ ver
		);

#endif /* defined (RTC1_TYPE) */

	debug_printf_P(PSTR("Write wav file '%s'.\n"), fname);

	rc = write_wav_header(fname, dsp_get_sampleraterx());
	wave_irecorded = 0;
	return (rc != FR_OK);	// 1 - ������ - ����������� ������.
}

// ����������� ������
static uint_fast8_t wave_stoprecording(void)
{
	FRESULT rc;				/* Result code */
	rc = write_wav_tail();
	return (rc != FR_OK);	// 1 - ������ - ����������� ������.
}


static uint_fast8_t wave_resync(void)
{
	FRESULT rc;				/* Result code */
	rc = write_wav_resync();
	return (rc != FR_OK);	// 1 - ������ - ����������� ������.
}

// 1 - ����������� ������
// 2 - ����������� ������ � �������� ��������� ��������
// 3 - ��������� resync
static uint_fast8_t wave_nextblockrecording(void)
{
	const uint_fast32_t RSYNLEN = 10UL * 1024 * 1024;
	const uint_fast32_t FILELEN = 695UL * 1024 * 1024;	// ������ �������������� ���� �� ������ ����������� �����
	//const uint_fast32_t FILELEN = 33UL * 1024 * 1024;	// ������ �������������� ���� �� ������ ����������� �����
	void * p;
	unsigned n = takerecordbuffer(& p);
	if (n != 0)	// ���������� ������ ��� ������
	{
		FRESULT rc;				/* Result code */
		UINT bw;
		rc = f_write(& wav_file, p, n, & bw);
		releaserecordbuffer(p);

		if (rc != FR_OK || bw != n)
			return 1;	// 1 - ������ - ����������� ������.

		wave_num_bytes += n;
		// ������������� ������ �������� �����
		if (1 && (wave_irecorded += n) >= RSYNLEN)
		{
			//debug_printf_P(PSTR("wave_nextblockrecording: force resync\n"));
			// ������ 10 ��������
			wave_irecorded = 0;
			return 3;		// 3 - ��������� resync
			//rc = write_wav_resync();
			//if (rc != FR_OK)
			//	return 1;	// 1 - ������ - ����������� ������.
			//debug_printf_P(PSTR("wave_nextblockrecording: resync okay\n"));
		}
		if (f_size(& wav_file) >= FILELEN)
		{
			// ������ 695 ��������
			return 2;	// ������ ��������� ��������
		}
	}
	return 0;	// 1 - ������ - ����������� ������.
}


// ��� ���������� �� ��������� state machune
enum
{
	SDSTATUS_OK,		// ����� ���������� � ����������
	SDSTATUS_BUSY,		// ��������� state machime �� ��������
	SDSTATUS_ERROR		// ������, �������� ������ �� ������� � ��������� ��������.
};

enum
{
	SDSTATE_IDLE,
	SDSTATE_STARTREC,
	SDSTATE_RECORDING,
	SDSTATE_STOPRECORDING,
	SDSTATE_BREAKCHUNK,
	SDSTATE_CONTRECORDING,
	SDSTATE_UNMOUNT,
	SDSTATE_RESYNC
};

// AUDIO recording state
static uint_fast8_t	sdstate = SDSTATE_IDLE;

// ������� state machine � ��������� ���������
void sdcardinitialize(void)
{
	sdstate = SDSTATE_IDLE;	
}


// ���������� � display2.c ��������� ������
uint_fast8_t hamradio_get_rec_value(void)
{
	return sdstate != SDSTATE_IDLE;
}

void sdcardrecord(void)
{
	switch (sdstate)
	{
	case SDSTATE_IDLE:
		debug_printf_P(PSTR("sdcardrecord: start recording\n"));
		// �������� ������
		if (waveMount() == 0)		/* Register volume work area (never fails) */
			sdstate = SDSTATE_STARTREC;
		break;
	}
}

void sdcardstop(void)	// ������� "���������� ������"
{
	switch (sdstate)
	{
	case SDSTATE_RECORDING:
	case SDSTATE_BREAKCHUNK:
	case SDSTATE_RESYNC:
	case SDSTATE_CONTRECORDING:
		// ����������� ������
		debug_printf_P(PSTR("sdcardstop: stop recording\n"));
		wave_stoprecording();
		sdstate = SDSTATE_UNMOUNT;
		break;
	}
}

void sdcardtoggle(void)
{
	if (sdstate == SDSTATE_IDLE)
		sdcardrecord();
	else
		sdcardstop();
}

// ������ ���������� �� ��������� ����� ��� �������������� "�������������"
// ���������� ������� �� SD CARD
void sdcardbgprocess(void)
{
	switch (sdstate)
	{
	case SDSTATE_IDLE:
		//debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_IDLE\n"));
		break;

	case SDSTATE_RECORDING:
		//debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_RECORDING\n"));
		switch (wave_nextblockrecording())
		{
		default:
		//case 1:
			// ����������� ������
			sdstate = SDSTATE_STOPRECORDING;
			break;

		case 2:
			// ����������� ������ � �������� ��������� ��������
			sdstate = SDSTATE_BREAKCHUNK;
			break;

		case 3:
			// ��������� resync
			sdstate = SDSTATE_RESYNC;
			break;

		case 0:
			// �������� � ��������� ������
			break;
		}
		break;

	case SDSTATE_RESYNC:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_RESYNC\n"));
		// ��������� resync
		if (wave_resync() == 0)
			sdstate = SDSTATE_RECORDING;
		else
			sdstate = SDSTATE_STOPRECORDING;
		break;

	case SDSTATE_BREAKCHUNK:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_BREAKCHUNK\n"));
		// ����������� ������ � �������� ��������� ��������
		if (wave_stoprecording() == 0)
			sdstate = SDSTATE_CONTRECORDING;
		else
			sdstate = SDSTATE_STOPRECORDING;
		break;

	case SDSTATE_CONTRECORDING:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_CONTRECORDING\n"));
		if (wave_startrecording() != 0)
		{
			debug_printf_P(PSTR("sdcardbgprocess: wave_startrecording failure\n"));
			sdstate = SDSTATE_UNMOUNT;
		}
		else
			sdstate = SDSTATE_RECORDING;
		break;

	case SDSTATE_STOPRECORDING:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_STOPRECORDING\n"));
		wave_stoprecording();
		sdstate = SDSTATE_UNMOUNT;
		break;

	case SDSTATE_UNMOUNT:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_UNMOUNT\n"));
		waveUnmount();		/* Unregister volume work area (never fails) */
		sdstate = SDSTATE_IDLE;
		break;

	case SDSTATE_STARTREC:
		debug_printf_P(PSTR("sdcardbgprocess: SDSTATE_STARTREC\n"));
		if (wave_startrecording() == 0)
		{
			debug_printf_P(PSTR("sdcardrecord: wave_startrecording success\n"));
			sdstate = SDSTATE_RECORDING;

			// ����������� ��������� ����� ������ ������� ���
			// ���������� �������� � ������ ������
			uint_fast8_t n;
			for (n = 0; n < 5; ++ n)
			{
				void * p;
				if (takerecordbuffer(& p) != 0)	// ���������� ������ ��� ������
					releaserecordbuffer(p);
			}
			sdstate = SDSTATE_RECORDING;

		}
		else
		{
			sdstate = SDSTATE_UNMOUNT;
			debug_printf_P(PSTR("sdcardrecord: wave_startrecording failure\n"));
		}
		break;
	}
}

void sdcardformat(void)
{
	ALIGNX_BEGIN BYTE work [_MAX_SS] ALIGNX_END;
	FRESULT rc;  

	switch (sdstate)
	{
	case SDSTATE_IDLE:
		debug_printf_P(PSTR("sdcardformat: start formatting\n"));
		rc = f_mkfs("0:", FM_ANY, 0, work, sizeof (work));
		if (rc != FR_OK)
		{
			debug_printf_P(PSTR("sdcardformat: f_mkfs failure\n"));
		}
		else
		{
			debug_printf_P(PSTR("sdcardformat: f_mkfs okay\n"));
		}
		break;
	}
}

#endif /* WITHUSEAUDIOREC */
