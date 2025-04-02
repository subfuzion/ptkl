/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#ifndef PTKL_CONSOLE_H
#define PTKL_CONSOLE_H

#include <curses.h>
#include <stdbool.h>

typedef struct console_t *console;

/* Command handling */
typedef void (*command_handler) (console c, const char *cmd);

/* Console lifecycle */
console console_new (void);
void console_free (console c);
bool console_init (console c);
void console_cleanup (console c);

/* Command bar */
void console_set_command_handler (console c, command_handler handler);
void console_show_command_bar (console c, const char *prompt);

/* Window management */
bool console_create_windows (console c);
void console_refresh_windows (console c);
void console_resize_windows (console c);

/* Event loop */
bool console_run (console c);
void console_stop (console c);

/* Input handling */
void console_handle_input (console c, int ch);

/* Output */
void console_print (console c, const char *fmt, ...);
void console_error (console c, const char *fmt, ...);

#endif /* PTKL_CONSOLE_H */
