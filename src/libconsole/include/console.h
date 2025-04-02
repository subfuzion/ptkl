/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#ifndef PTKL_CONSOLE_H
#define PTKL_CONSOLE_H

#include <stdbool.h>

typedef struct console_t *console;

/* Command handling */
typedef char **(*command_completion) (console c, const char *prefix,
				      int *count);
typedef void (*command_handler) (console c, const char *cmd);

/* Console lifecycle */
console console_new (void);
void console_free (console c);
bool console_init (console c);
void console_cleanup (console c);

/* Title and command bar */
void console_set_title (console c, const char *title);
void console_set_command_handler (console c, command_handler handler);
void console_set_completion_handler (console c, command_completion handler);
void console_show_command_bar (console c, const char *prompt);

/* Window management */
void console_refresh_windows (console c);
void console_resize_windows (console c);

/* Completion management */
void console_free_completions (console c);
void console_update_completions (console c);

/* Event loop */
bool console_run (console c);
void console_stop (console c);

/* Input handling */
void console_handle_input (console c, int ch);

/* Output */
void console_print (console c, const char *fmt, ...);
void console_error (console c, const char *fmt, ...);
void console_clear (console c);

#endif /* PTKL_CONSOLE_H */
