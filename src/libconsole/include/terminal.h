/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#ifndef PTKL_TERMINAL_H
#define PTKL_TERMINAL_H

#include <stdbool.h>

/* Terminal dimensions */
#define MIN_TERM_WIDTH 80
#define MIN_TERM_HEIGHT 24

/* Terminal modes */
#define TERM_MODE_NORMAL 0
#define TERM_MODE_COOKED 1
#define TERM_MODE_RAW    2

/* Terminal colors */
#define TERM_COLOR_DEFAULT 0
#define TERM_COLOR_BLACK   1
#define TERM_COLOR_RED     2
#define TERM_COLOR_GREEN   3
#define TERM_COLOR_YELLOW  4
#define TERM_COLOR_BLUE    5
#define TERM_COLOR_MAGENTA 6
#define TERM_COLOR_CYAN    7
#define TERM_COLOR_WHITE   8

/* Terminal initialization and cleanup */
bool terminal_init(void);
void terminal_cleanup(void);

/* Terminal input */
int terminal_getch(void);
bool terminal_kbhit(void);

/* Terminal output */
void terminal_clear(void);
void terminal_clear_line(void);
void terminal_move_cursor(int x, int y);
void terminal_hide_cursor(void);
void terminal_show_cursor(void);

/* Terminal properties */
void terminal_get_size(int *width, int *height);
void terminal_set_mode(int mode);
void terminal_set_color(int fg, int bg);
void terminal_reset_color(void);

#endif /* PTKL_TERMINAL_H */
