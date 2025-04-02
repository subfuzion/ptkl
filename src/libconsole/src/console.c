/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#include "console.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Window dimensions */
#define MIN_TERM_WIDTH 80
#define MIN_TERM_HEIGHT 24

struct console_t {
	/* Main windows */
	WINDOW *main_win; /* Full screen window */
	WINDOW *header_win; /* Header bar */
	WINDOW *content_win; /* Main content area */
	WINDOW *status_win; /* Status bar */
	WINDOW *cmd_win; /* Command bar */

	/* State */
	bool running; /* Event loop control */
	char *title; /* Console title */
	char *status; /* Status message */
	char *cmd_prompt; /* Command prompt */
	char cmd_buffer[256]; /* Command input buffer */
	int cmd_pos; /* Current position in command buffer */
	command_handler cmd_handler; /* Command handler callback */

	/* Error state */
	char *error_status; /* Saved status during error */
	long error_start; /* Time when error was shown */
};

console console_new (void)
{
	console c = calloc (1, sizeof (struct console_t));
	if (c == NULL) return NULL;

	c->running = false;
	c->title = strdup ("ptkl Console");
	c->status = strdup ("Ready");
	c->cmd_prompt = NULL;
	c->cmd_buffer[0] = '\0';
	c->cmd_pos = 0;
	c->cmd_handler = NULL;
	c->error_status = NULL;
	c->error_start = 0;

	return c;
}

void console_free (console c)
{
	if (c == NULL) return;

	free (c->title);
	free (c->status);
	free (c);
}

bool console_init (console c)
{
	if (c == NULL) return false;

	/* Initialize ncurses */
	initscr ();
	start_color ();
	cbreak ();
	noecho ();
	keypad (stdscr, TRUE);
	curs_set (0);
	refresh ();

	/* Check terminal size */
	int max_y, max_x;
	getmaxyx (stdscr, max_y, max_x);
	if (max_y < MIN_TERM_HEIGHT || max_x < MIN_TERM_WIDTH) {
		endwin ();
		return false;
	}

	/* Create windows */
	return console_create_windows (c);
}

void console_cleanup (console c)
{
	if (c == NULL) return;

	/* Clean up windows */
	if (c->header_win) delwin (c->header_win);
	if (c->content_win) delwin (c->content_win);
	if (c->status_win) delwin (c->status_win);
	if (c->main_win) delwin (c->main_win);

	/* End ncurses */
	endwin ();
}

bool console_create_windows (console c)
{
	if (c == NULL) return false;

	int max_y, max_x;
	getmaxyx (stdscr, max_y, max_x);

	/* Create main window */
	c->main_win = newwin (max_y, max_x, 0, 0);
	if (c->main_win == NULL) return false;

	/* Create header window (1 line) */
	c->header_win = newwin (1, max_x, 0, 0);
	if (c->header_win == NULL) return false;

	/* Create content window (fills remaining space) */
	c->content_win = newwin (max_y - 3, max_x, 1, 0);
	if (c->content_win == NULL) return false;

	/* Create status window (1 line) */
	c->status_win = newwin (1, max_x, max_y - 2, 0);
	if (c->status_win == NULL) return false;

	/* Create command window (1 line at bottom) */
	c->cmd_win = newwin (1, max_x, max_y - 1, 0);
	if (c->cmd_win == NULL) return false;

	/* Enable scrolling for content window */
	scrollok (c->content_win, TRUE);

	/* Initial refresh */
	console_refresh_windows (c);

	return true;
}

void console_refresh_windows (console c)
{
	if (c == NULL) return;

	/* Refresh header */
	wmove (c->header_win, 0, 0);
	wclrtoeol (c->header_win);
	mvwprintw (c->header_win, 0, 0, "%s", c->title);
	wrefresh (c->header_win);

	/* Refresh status */
	wmove (c->status_win, 0, 0);
	wclrtoeol (c->status_win);
	mvwprintw (c->status_win, 0, 0, "%s", c->status);
	wrefresh (c->status_win);

	/* Refresh command bar */
	if (c->cmd_prompt) {
		wmove (c->cmd_win, 0, 0);
		wclrtoeol (c->cmd_win);
		mvwprintw (c->cmd_win, 0, 0, "%s %s", c->cmd_prompt,
			   c->cmd_buffer);

		/* Position cursor and highlight current character */
		int prompt_len = strlen(c->cmd_prompt) + 1; /* +1 for the space */
		int cursor_x = prompt_len + c->cmd_pos;
		wmove(c->cmd_win, 0, cursor_x);

		/* If at end of input, show block cursor on a space */
		if (c->cmd_pos == strlen(c->cmd_buffer)) {
			wattron(c->cmd_win, A_REVERSE);
			mvwaddch(c->cmd_win, 0, cursor_x, ' ');
			wattroff(c->cmd_win, A_REVERSE);
		} else {
			/* Highlight current character */
			wattron(c->cmd_win, A_REVERSE);
			mvwaddch(c->cmd_win, 0, cursor_x, c->cmd_buffer[c->cmd_pos]);
			wattroff(c->cmd_win, A_REVERSE);
		}

		wrefresh (c->cmd_win);
	}

	/* Refresh content */
	wrefresh (c->content_win);
}

void console_resize_windows (console c)
{
	if (c == NULL) return;

	int max_y, max_x;
	getmaxyx (stdscr, max_y, max_x);

	/* Resize main window */
	wresize (c->main_win, max_y, max_x);

	/* Resize and move other windows */
	wresize (c->header_win, 1, max_x);
	wresize (c->content_win, max_y - 2, max_x);
	wresize (c->status_win, 1, max_x);

	mvwin (c->status_win, max_y - 1, 0);

	/* Refresh all windows */
	console_refresh_windows (c);
}

bool console_run (console c)
{
	if (c == NULL) return false;

	c->running = true;

	/* Event loop */
	while (c->running) {
		int ch = wgetch (c->content_win);
		console_handle_input (c, ch);
	}

	return true;
}

void console_stop (console c)
{
	if (c == NULL) return;
	c->running = false;
}

static void console_clear_error(console c)
{
	if (c == NULL || !c->error_status) return;

	/* Restore previous status */
	free(c->status);
	c->status = c->error_status;
	c->error_status = NULL;
	c->error_start = 0;

	/* Refresh display */
	console_refresh_windows(c);
}

void console_handle_input (console c, int ch)
{
	if (c == NULL) return;

	/* Clear error state if any input is received */
	if (c->error_status) {
		console_clear_error(c);
	}

	/* If command prompt is active, handle command input */
	if (c->cmd_prompt) {
		switch (ch) {
		case KEY_BACKSPACE:
		case 127: /* DEL key */
			if (c->cmd_pos > 0) {
				c->cmd_buffer[--c->cmd_pos] = '\0';
				console_refresh_windows (c);
			}
			break;

		case '\n':
		case KEY_ENTER:
			if (c->cmd_handler) {
				c->cmd_handler (c, c->cmd_buffer);
			}
			c->cmd_buffer[0] = '\0';
			c->cmd_pos = 0;
			console_refresh_windows (c);
			break;

		default:
			if (isprint (ch) &&
			    c->cmd_pos < sizeof (c->cmd_buffer) - 1) {
				c->cmd_buffer[c->cmd_pos++] = ch;
				c->cmd_buffer[c->cmd_pos] = '\0';
				console_refresh_windows (c);
			}
			break;
		}
		return;
	}

	/* Otherwise handle normal input */
	switch (ch) {
	case KEY_RESIZE: console_resize_windows (c); break;

	case 'q':
	case 'Q': console_stop (c); break;

	default:
		/* Handle other input */
		break;
	}
}

void console_print (console c, const char *fmt, ...)
{
	if (c == NULL || fmt == NULL) return;

	va_list args;
	va_start (args, fmt);
	vw_printw (c->content_win, fmt, args);
	va_end (args);

	wrefresh (c->content_win);
}

void console_error (console c, const char *fmt, ...)
{
	if (c == NULL || fmt == NULL) return;

	/* Save current status if not already in error state */
	if (!c->error_status) {
		c->error_status = strdup (c->status);
	}

	/* Format error message */
	va_list args;
	va_start (args, fmt);
	char error_msg[256];
	vsnprintf (error_msg, sizeof (error_msg), fmt, args);
	va_end (args);

	/* Update status with error */
	free (c->status);
	c->status = strdup (error_msg);

	/* Record error start time */
	c->error_start = time(NULL);

	/* Refresh status window */
	console_refresh_windows (c);
}

void console_set_title (console c, const char *title)
{
	if (c == NULL) return;

	/* Free existing title */
	if (c->title) {
		free(c->title);
	}

	/* Set new title */
	c->title = title ? strdup(title) : NULL;

	/* Refresh display */
	console_refresh_windows(c);
}

void console_set_command_handler (console c, command_handler handler)
{
	if (c == NULL) return;
	c->cmd_handler = handler;
}

void console_show_command_bar (console c, const char *prompt)
{
	if (c == NULL) return;

	/* Free existing prompt if any */
	if (c->cmd_prompt) {
		free (c->cmd_prompt);
	}

	/* Set new prompt */
	c->cmd_prompt = prompt ? strdup (prompt) : NULL;
	c->cmd_buffer[0] = '\0';
	c->cmd_pos = 0;

	/* Refresh windows to show/hide command bar */
	console_refresh_windows (c);
}

void console_clear (console c)
{
	if (c == NULL) return;

	/* Clear content window */
	werase(c->content_win);
	wrefresh(c->content_win);
}
