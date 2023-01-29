/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#ifndef ST7565S_H_INCLUDED
#define ST7565S_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LCDMODE_ST7565S || LCDMODE_PTE1206

#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
#define DISPLAY_FPS	10	/* обновление показаний частоты десять раз в секунду */
#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */

// Цветное изображение не поддерживается на этом дисплее (заглушка).
#define TFTRGB(r, g, b)	(0)
#define TFTRGB565 TFTRGB
typedef uint_fast8_t COLORPIP_T;	/* цвета не поддерживаются - заглушка */
typedef uint8_t PACKEDCOLORPIP_T;	/* цвета не поддерживаются - заглушка */

typedef PACKEDCOLORPIP_T PACKEDCOLORPIP_T;
typedef COLORPIP_T COLORPIP_T;

#endif /* LCDMODE_ST7565S || LCDMODE_PTE1206 */

#endif /* ST7565S_H_INCLUDED */

