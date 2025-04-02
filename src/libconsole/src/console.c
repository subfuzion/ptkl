/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#include "console.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "editline.h"
#include "terminal.h"

/* Window dimensions */
#define MIN_TERM_WIDTH 80
#define MIN_TERM_HEIGHT 24

/* Colors */
#define COLOR_HEADER TERM_COLOR_BLUE
#define COLOR_STATUS TERM_COLOR_WHITE
#define COLOR_ERROR  TERM_COLOR_RED

struct console_t {
    /* State */
    bool running;           /* Event loop control */
    char *title;           /* Console title */
    char *status;          /* Status message */
    char *cmd_prompt;      /* Command prompt */
    editline editor;       /* Line editor */
    command_handler cmd_handler;  /* Command handler callback */

    /* Error state */
    char *error_status;    /* Saved status during error */
    long error_start;      /* Time when error was shown */

    /* Screen layout */
    int width;             /* Terminal width */
    int height;            /* Terminal height */
    int content_start;     /* Start line of content area */
    int content_height;    /* Height of content area */
    int cursor_x;          /* Current cursor X position */
    int cursor_y;          /* Current cursor Y position */

    /* Completion */
    char **completions;    /* Array of completion strings */
    int completion_count;  /* Number of completions */
    int current_completion; /* Current completion index */
    command_completion completion_handler; /* Completion callback */
};

console console_new(void)
{
    console c = calloc(1, sizeof(struct console_t));
    if (c == NULL) return NULL;

    /* Initialize state */
    c->running = false;
    c->title = strdup("ptkl Console");
    c->status = strdup("Ready");
    c->cmd_prompt = strdup("> ");
    c->error_status = NULL;
    c->error_start = 0;

    /* Initialize editor */
    c->editor = editline_new();
    if (c->editor == NULL) {
        console_free(c);
        return NULL;
    }
    c->cmd_handler = NULL;

    /* Initialize screen layout */
    terminal_get_size(&c->width, &c->height);
    c->content_start = 2;  /* After title and status */
    c->content_height = c->height - 3;  /* -2 for headers, -1 for command line */
    c->cursor_x = 0;
    c->cursor_y = c->height - 1;  /* Command line at bottom */

    return c;
}

void console_free(console c)
{
    if (c == NULL) return;

    console_cleanup(c);
    free(c->title);
    free(c->status);
    free(c->cmd_prompt);
    editline_free(c->editor);
    free(c);
}

bool console_init(console c)
{
    if (c == NULL) return false;

    /* Initialize terminal */
    if (!terminal_init()) {
        return false;
    }

    /* Set raw mode for input */
    terminal_set_mode(TERM_MODE_RAW);

    /* Check terminal size */
    terminal_get_size(&c->width, &c->height);
    if (c->width < MIN_TERM_WIDTH || c->height < MIN_TERM_HEIGHT) {
        terminal_cleanup();
        return false;
    }

    /* Clear screen and set initial colors */
    terminal_clear();
    terminal_set_color(TERM_COLOR_DEFAULT, TERM_COLOR_DEFAULT);

    /* Initialize editor */
    editline_init(c->editor, c->cmd_prompt);
    editline_set_position(c->editor, 2, c->height - 1);

    /* Draw initial layout */
    console_refresh_windows(c);

    return true;
}

void console_cleanup(console c)
{
    if (c == NULL) return;

    /* Clean up editline */
    editline_cleanup(c->editor);

    /* Restore terminal mode */
    terminal_set_mode(TERM_MODE_NORMAL);

    /* Clean up terminal */
    terminal_cleanup();
}

void console_refresh_windows(console c)
{
    if (c == NULL) return;

    /* Clear screen */
    terminal_clear();

    /* Draw header */
    terminal_move_cursor(0, 0);
    terminal_set_color(COLOR_HEADER, TERM_COLOR_DEFAULT);
    printf("%s", c->title);

    /* Draw status line */
    terminal_move_cursor(0, c->height - 2);
    terminal_set_color(COLOR_STATUS, TERM_COLOR_DEFAULT);
    printf("%s", c->status ? c->status : "");

    /* Draw command line */
    terminal_move_cursor(0, c->height - 1);
    terminal_set_color(TERM_COLOR_DEFAULT, TERM_COLOR_DEFAULT);
    printf("%s%s", c->cmd_prompt, editline_get_buffer(c->editor));

    /* Position cursor */
    int prompt_len = strlen(c->cmd_prompt);
    int x, y;
    editline_get_position(c->editor, &x, &y);
    int cursor_pos = prompt_len + x;
    c->cursor_x = cursor_pos;
    c->cursor_y = c->height - 1;
    terminal_move_cursor(c->cursor_x, c->cursor_y);
    fflush(stdout);
}

void console_resize_windows(console c)
{
    if (c == NULL) return;

    /* Get new dimensions */
    terminal_get_size(&c->width, &c->height);

    /* Update layout */
    c->content_start = 2;  /* After title and status */
    c->content_height = c->height - 3;  /* -2 for headers, -1 for command line */

    /* Refresh display */
    console_refresh_windows(c);

    /* Update editor position */
    editline_set_position(c->editor, strlen(c->cmd_prompt), c->height - 1);
}

bool console_run(console c)
{
    if (c == NULL) return false;

    c->running = true;

    /* Event loop */
    while (c->running) {
        int ch = terminal_getch();
        console_handle_input(c, ch);
    }

    return true;
}

void console_stop(console c)
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

void console_handle_input(console c, int ch)
{
    if (c == NULL) return;

    /* Clear error state if any input is received */
    if (c->error_status) {
        console_clear_error(c);
    }

    /* Handle special keys */
    switch (ch) {
        case '\033': /* ESC key */
            /* Clear editor and status */
            editline_clear(c->editor);
            free(c->status);
            c->status = NULL;
            console_free_completions(c);
            break;

        case '\n':
        case '\r':
            if (c->cmd_handler) {
                /* Get command and execute it */
                const char *cmd = editline_get_buffer(c->editor);
                if (cmd && *cmd) {
                    c->cmd_handler(c, cmd);
                }
            }
            /* Clear command line */
            editline_clear(c->editor);
            console_free_completions(c);
            break;

        case '\t':
            if (c->completion_handler) {
                console_update_completions(c);
            }
            break;

        case 'q':
        case 'Q':
            if (!c->cmd_prompt) {
                console_stop(c);
            }
            break;

        default:
            /* Handle normal input */
            if (isprint(ch)) {
                editline_getch(c->editor);
            }
            break;
    }

    /* Refresh display */
    console_refresh_windows(c);
}

void console_print(console c, const char *fmt, ...)
{
    if (c == NULL || fmt == NULL) return;

    va_list args;
    va_start(args, fmt);

    /* Print to content area */
    terminal_move_cursor(0, c->content_start);
    vprintf(fmt, args);
    va_end(args);

    /* Update cursor position */
    console_refresh_windows(c);
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
	c->error_start = time (NULL);

	/* Refresh status window */
	console_refresh_windows (c);
}

void console_set_title (console c, const char *title)
{
	if (c == NULL) return;

	/* Free existing title */
	if (c->title) {
		free (c->title);
	}

	/* Set new title */
	c->title = title ? strdup (title) : NULL;

	/* Refresh display */
	console_refresh_windows (c);
}

void console_set_command_handler (console c, command_handler handler)
{
	if (c == NULL) return;
	c->cmd_handler = handler;
}

void console_set_completion_handler (console c, command_completion handler)
{
	if (c == NULL) return;
	c->completion_handler = handler;
}

void console_free_completions(console c)
{
    if (c == NULL || c->completions == NULL) return;

    /* Free each completion string */
    for (int i = 0; i < c->completion_count; i++) {
        free(c->completions[i]);
    }

    /* Free the array */
    free(c->completions);
    c->completions = NULL;
    c->completion_count = 0;
    c->current_completion = -1;
}

static void console_update_status (console c)
{
	if (c == NULL || !c->completion_handler) return;

	/* Show completions in status bar if any */
	if (c->completion_count > 0) {
		/* Format completions into status message */
		char status[256] = {0};
		int pos = 0;

		/* Find max completion length */
		size_t max_len = 0;
		for (int i = 0; i < c->completion_count; i++) {
			size_t len = strlen (c->completions[i]);
			if (len > max_len) max_len = len;
		}

		/* Add each completion with fixed-width slots */
		for (int i = 0; i < c->completion_count; i++) {
			if (i == c->current_completion) {
				/* Selected completion with brackets */
				pos += snprintf (status + pos,
						 sizeof (status) - pos,
						 "[%-*s]  ", (int)max_len,
						 c->completions[i]);
			} else {
				/* Non-selected completion with padding */
				pos += snprintf (status + pos,
						 sizeof (status) - pos,
						 " %-*s   ", (int)max_len,
						 c->completions[i]);
			}
			if (pos >= sizeof (status) - 1) break;
		}

		/* Update status */
		free (c->status);
		c->status = strdup (status);
		console_refresh_windows (c);
	}
}

void console_update_completions(console c)
{
    if (c == NULL || !c->completion_handler) return;

    /* Only get new completions if we don't have any */
    if (c->completions == NULL) {
        /* Get new completions */
        c->completions = c->completion_handler(
            c, editline_get_buffer(c->editor),
            &c->completion_count);
    }

    /* Show first completion */
    if (c->completion_count > 0) {
        editline_clear(c->editor);
        editline_set_buffer(c->editor, c->completions[0]);
    }

    /* Update status bar */
    console_update_status(c);
}

void console_show_command_bar(console c, const char *prompt)
{
    if (c == NULL) return;

    /* Free existing prompt if any */
    if (c->cmd_prompt) {
        free(c->cmd_prompt);
    }

    /* Set new prompt */
    c->cmd_prompt = prompt ? strdup(prompt) : NULL;

    /* Set editline prompt */
    editline_set_prompt(c->editor, c->cmd_prompt);

    /* Show cursor and refresh windows */
    terminal_show_cursor();
    console_refresh_windows(c);
}

void console_clear(console c)
{
    if (c == NULL) return;

    /* Clear screen */
    terminal_clear();

    /* Refresh display */
    console_refresh_windows(c);
}
