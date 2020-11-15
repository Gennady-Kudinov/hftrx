#ifndef GUI_SYSTEM_H_INCLUDED
#define GUI_SYSTEM_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI
#include "src/gui/gui_structs.h"

#define GUI_MEM_ASSERT(v) do { if (((v) == NULL)) { \
		PRINTF(PSTR("%s: %d ('%s') - memory allocate failed!\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

void * find_gui_element(element_type_t type, window_t * win, const char * name);
uint_fast8_t get_label_width(const label_t * const lh);
uint_fast8_t get_label_height(const label_t * const lh);
void footer_buttons_state (uint_fast8_t state, ...);
void close_window(uint_fast8_t parent);
void open_window(window_t * win);
void close_all_windows(void);
void calculate_window_position(window_t * win, uint_fast8_t mode, ...);
void elements_state (window_t * win);
void remove_end_line_spaces(char * str);
const char * remove_start_line_spaces(const char * str);
void reset_tracking(void);
uint_fast8_t check_for_parent_window(void);
void get_gui_tracking(int_fast8_t * x, int_fast8_t * y);

uint_fast8_t put_to_wm_queue(window_t * win, wm_message_t message, ...);
wm_message_t get_from_wm_queue(window_t * win, uint_fast8_t * type, uintptr_t * ptr, int_fast8_t * action);

#endif /* WITHTOUCHGUI */
#endif /* GUI_USER_H_INCLUDED */
