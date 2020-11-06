/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// аппаратута трансивера RA4YBO V1 - "портативный с тюнеррм без часов"
//

#ifndef ATMEGA_CTLSTYLE_RA4YBO_V1_H_INCLUDED
#define ATMEGA_CTLSTYLE_RA4YBO_V1_H_INCLUDED 1

#if F_CPU != 8000000
	#error Set F_CPU right value in project file
#endif

#if \
	defined (__AVR_ATmega644__) || \
	defined (__AVR_ATmega644P__) || \
	defined (__AVR_ATmega644PA__) || \
	0

#else
	//#error Set CPU TYPE to ATMEGA644PA in project file
#endif

	/* модели синтезаторов - схемы частотообразования */

	// --- вариации прошивки, специфические для разных плат
	#define CTLREGMODE_RA4YBO_V1	1		/* управляющий регистр 	*/

	#define WITHPREAMPATT2_10DB		1	// Управление УВЧ и двухкаскадным аттенюатором

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	#define	BOARD_AGCCODE_2		0x02
	#define	BOARD_AGCCODE_3		0x03

	#define BOARD_AGCCODE_OFF 255
	#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_0
	#define	BOARD_AGCCODE_MED	BOARD_AGCCODE_1
	#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_2
	#define	BOARD_AGCCODE_LONG	BOARD_AGCCODE_3
	#define WITHAGCMODE4STAGES	1	// 4 скорости. выключенно не бывает

	#define BOARD_NOTCH_MANUAL 0
	#define WITHLCDBACKLIGHTMIN 0
	#define WITHLCDBACKLIGHTMAX 1
	#define WITHPOWERTRIMMAX 1
	#define BOARD_NOTCH_OFF 0

	#define	FONTSTYLE_ITALIC	1	//
	#define DSTYLE_UR3LMZMOD	1

	// +++ вариации прошивки, специфические для плат на ATMega
	// --- вариации прошивки, специфические для плат на ATMega

	// +++ Особые варианты расположения кнопок на клавиатуре
	#define KEYB_RA4YBO_V1	1	/*  */
	//#define KEYB_RA1AGG	1	/* расположение кнопок для корпуса, сделанного RA1AGG - три группы по четыре линейки кнопок друг за другом в один ряд. */
	//#define KEYB_UA1CEI	1	/* расположение кнопок для UA1CEI */
	//#define KEYB_VERTICAL_REV	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
	//#define KEYBOARD_USE_ADC6	1	/* шесть кнопок на каждом входе ADCx	*/
	// --- Особые варианты расположения кнопок на клавиатуре

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* тип применяемого индикатора 20*2, возможно вместе с LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* тип применяемого индикатора 16*2 */
	//#define LCDMODE_WH2004	1	/* тип применяемого индикатора 20*4 */
	//#define LCDMODE_RDX0077	1	/* Индикатор 128*64 с контроллером UC1601.  */
	//#define LCDMODE_RDX0154	1	/* Индикатор 132*64 с контроллером UC1601. */
	//#define LCDMODE_RDX0120	1	/* Индикатор 64*32 с контроллером UC1601.  */
	//#define LCDMODE_UC1601S_XMIRROR	1	/* Индикатор 132*64 с контроллером UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - перевернуть изображение */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - без внутреннего преобразователя */
	//#define LCDMODE_TIC154	1	/* Индикатор 133*65 с контроллером PCF8535 */
	//#define LCDMODE_TIC218	1	/* Индикатор 133*65 с контроллером PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* Контроллер PCF8535 - перевернуть изображение */
	#define LCDMODE_LS020 	1	/* Индикатор 176*132 Sharp LS020B8UD06 с контроллером LR38826 */
	#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - перевернуть изображение */
	//#define LCDMODE_LPH88		1	/* Индикатор 176*132 LPH8836-2 с контроллером Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - перевернуть изображение */
	//#define LCDMODE_ILI9163	1	/* Индикатор LPH9157-2 176*132 с контроллером ILITEK ILI9163 - Лента дисплея справа, а выводы слева. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - перевернуть изображение (для выводов справа, лента дисплея слева) */
	//#define LCDMODE_L2F50	1	/* Индикатор 176*132 с контроллером Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* Переворот изображени я в случае LCDMODE_L2F50 */
	//#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - перевернуть изображение */
	//#define LCDMODE_ILI9225	1	/* Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - перевернуть изображение (для выводов слева от экрана) */
	//#define LCDMODE_UC1608	1		/* Индикатор 240*128 с контроллером UC1608.- монохромный */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ST7735	1	/* Индикатор 160*128 с контроллером Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - перевернуть изображение (для выводов справа) */
	//#define LCDMODE_ST7565S	1	/* Индикатор WO12864C2-TFH# 128*64 с контроллером Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ILI9320	1	/* Индикатор 248*320 с контроллером ILI9320 */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHENCODER	1	/* для изменения частоты имеется енкодер */

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	//#define WITHCAT		1	/* используется CAT */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	//#define WITHVOX		1	/* используется VOX */
	#define WITHSWRMTR	1		/* Измеритель КСВ или */
	#define WITHSHOWSWRPWR	1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности  */
	#define WITHBARS	1	/* отображение S-метра и SWR-метра */
	#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	//#define WITHPBT		1	/* используется PBT */
	#define WITHIFSHIFT	1	/* используется IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	#define WITHMODESETFULLNFM 1
	#define WITHWFM	1			/* используется WFM */

	#define WITHAUTOTUNER	1	/* Есть функция автотюнера */
	#define SHORTSET7 1
	//#define FULLSET7 1

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define DEFPREAMPSTATE 	1	/* УВЧ по умолчанию включён (1) или выключен (0) */
	
	//#define REQUEST_FOR_RN4NAB 1	/* Специальный вариант диапазонных фильтров */

	/* что за память настроек и частот используется в контроллере */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// Так же при использовании FM25040A - 5 вольт, 512 байт
	#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section

	//#define FTW_RESOLUTION 28	/* разрядность FTW выбранного DDS */
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */
	//#define FTW_RESOLUTION 31	/* разрядность FTW выбранного DDS - ATINY2313 */

	//#define WITHLO1LEVELADJ		1	/* включено управление уровнем (амплитудой) LO1 */
	/* Board hardware configuration */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9835
	//#define DDS1_TYPE DDS_TYPE_AD9851
	#define PLL1_TYPE PLL_TYPE_SI570
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_ATTINY2313

	//#define DDS1_TYPE DDS_TYPE_AD9834

	//#define WITHFIXEDBFO	1	/* Переворот боковых за счёт 1-го гетродина (особенно, если нет подстраиваемого BFO) */
	//#define WITHDUALFLTR	1	/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */
	#define WITHSAMEBFO	1		/* использование общих настроек BFO для приёма и передачи */
	#define DDS2_TYPE DDS_TYPE_AD9834
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define PLL2_TYPE PLL_TYPE_ADF4001	/* Делитель октавного гетеродина для получения сквозного диапазона */
	#define PLL2_TYPE PLL_TYPE_LMX2306	/* Делитель октавного гетеродина для получения сквозного диапазона */

	//#define DDS1_CLK_DIV	7		/* Делитель опорной частоты перед подачей в DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS2 */

	//#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */

	/* Назначение адресов на SPI шине */
	//#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	//#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Для двойной DDS первого гетеродина - вторая DDS */
	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetpll2 SPI_CSEL2 	/* RA4YBO: LMX2306 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */ 
	//#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/
	

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_SSB 0x03
	#define BOARD_DETECTOR_AM 0x02
	#define BOARD_DETECTOR_FM 0x01
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x01	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	#define BOARD_DETECTOR_MUTE 4

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTER_0P3	0x04
	#define	BOARD_FILTER_0P5	0x05
	#define	BOARD_FILTER_1P5	0x06
	#define	BOARD_FILTER_2P4	0x03
	#define	BOARD_FILTER_3P1	0x00
	#define	BOARD_FILTER_7P8	0x02
	#define	BOARD_FILTER_17P0	0x01
	#define	BOARD_FILTER_WFM	0x07

	//#define IF3_FMASK (IF3_FMASK_17P0 | IF3_FMASK_7P8 | IF3_FMASK_3P1 | IF3_FMASK_2P4 | IF3_FMASK_1P5 | IF3_FMASK_0P5 | IF3_FMASK_0P3)
	#define IF3_FMASK (IF3_FMASK_3P1)
	#define IF3_FHAVE IF3_FMASK
	#define IF3_FMASKTX	(IF3_FMASK_3P1)

	#define WITHSPLIT	1	/* управление режимами расстройки одной кнопкой */
	//#define WITHSPLITEX	1	/* Трехкнопочное управление режимами расстройки */
	#define WITHCATEXT	1	/* Расширенный набор команд CAT */
	#define WITHELKEY	1
	#define WITHKBDENCODER 1	// перестройка частоты кнопками
	#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
	#define KEYBOARD_USE_ADC	1	/* на одной линии установлено  четыре  клавиши. на vref - 6.8K, далее 2.2К, 4.7К и 13K. */

	// Назначения входов АЦП процессора.
	enum 
	{ 
		VOLTSOURCE = 1, // Средняя точка делителя напряжения, для АКБ
		SMETERIX = 0,	// S-meter
		PWRI = 3, 
		FWD = 3, REF = 2,	// SWR-meter

		XTHERMOMRRIX = BOARD_ADCMRRIN(0),	// кеш - индекc не должен повторяться в конфигурации
		PASENSEMRRIX = BOARD_ADCMRRIN(1),	// кеш - индекc не должен повторяться в конфигурации
		REFMRRIX = BOARD_ADCMRRIN(2),
		FWDMRRIX = BOARD_ADCMRRIN(3),
		PWRMRRIX = FWDMRRIX,
		VOLTMRRIX = BOARD_ADCMRRIN(4),	// кеш - индекc не должен повторяться в конфигурации

		KI0 = 4, KI1 = 5, KI2 = 6, KI3 = 7	// клавиатура
	};
	#define KI_COUNT 4	// количество используемых под клавиатуру входов АЦП


	#define VOLTLEVEL_UPPER		47	// 4.7 kOhm - верхний резистор делителя датчика напряжения
	#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - нижний резистор

	/* Первая ПЧ - 64455, вторая - 500/455 кГц */

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	//#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO1_SIDE_F(freq) (((freq) < 64455000UL) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#if WITHIF4DSP
		// архитектура для DSP на последней ПЧ
		#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* WITHIF4DSP */
		// обычная архитектура для слухового приёмв
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
	#endif /* WITHIF4DSP */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	13000000UL	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 - "затычка" */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 - AD9834 */

	#define	WFM_IF1	10700000ul

#if 1
	// Версия под фильтр 1-й ПЧ 64.455
	// IF2=500 kHz
	#define LO2_PLL_N500	1284
	#define LO2_PLL_R500	261
	// IF2=455 kHz
	#define LO2_PLL_N455	1280
	#define LO2_PLL_R455	260
#else
	// Версия под фильтр 1-й ПЧ 80.455
	// IF2=500 kHz
	#define LO2_PLL_N500	1593
	#define LO2_PLL_R500	259
	// IF2=455 kHz
	#define LO2_PLL_N455	1600
	#define LO2_PLL_R455	260
#endif

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define LO4_POWER2 0

	#define TUNE_BOTTOM 30000L		/* нижняя частота настройки */
	#define TUNE_TOP 180000000L		/* верхняя частота настройки */


#endif /* ATMEGA_CTLSTYLE_RA4YBO_V1_H_INCLUDED */
