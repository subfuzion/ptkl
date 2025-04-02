/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#include "editline.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct editline_t {
    char *prompt;           /* Current prompt */
    char buffer[EDITLINE_BUFSIZE];  /* Current line buffer */
    int buffer_pos;         /* Current position in buffer */
    completion_callback completion_fn;  /* User-provided completion callback */
    bool initialized;       /* Whether initialized */
    int cursor_x;          /* Current cursor X position */
    int cursor_y;          /* Current cursor Y position */
};

/* Create new editline instance */
editline editline_new(void)
{
    editline e = calloc(1, sizeof(struct editline_t));
    if (e == NULL) return NULL;

    e->prompt = strdup("");
    e->buffer[0] = '\0';
    e->buffer_pos = 0;
    e->completion_fn = NULL;
    e->initialized = false;
    e->cursor_x = 0;
    e->cursor_y = 0;

    return e;
}

/* Free editline instance */
void editline_free(editline e)
{
    if (e == NULL) return;

    editline_cleanup(e);
    free(e->prompt);
    free(e);
}

/* Initialize editline */
bool editline_init(editline e, const char *prompt)
{
    if (e == NULL) return false;

    /* Set initial prompt */
    editline_set_prompt(e, prompt);

    /* Initialize buffer */
    e->buffer[0] = '\0';
    e->buffer_pos = 0;
    e->cursor_x = strlen(e->prompt);
    e->cursor_y = 0;
    e->initialized = true;

    return true;
}

/* Clean up editline */
void editline_cleanup(editline e)
{
    if (e == NULL || !e->initialized) return;

    /* Clear buffer */
    e->buffer[0] = '\0';
    e->buffer_pos = 0;
    e->initialized = false;
}

/* Get character input */
int editline_getch(editline e)
{
    if (e == NULL || !e->initialized) return -1;

    int ch = terminal_getch();
    if (ch != -1) {
        /* Handle special keys first */
        switch (ch) {
        case '\n': /* Enter */
        case '\r':
            printf("\n");
            e->cursor_y++;
            e->cursor_x = 0;
            return ch;

        case '\033': /* Escape */
            editline_clear(e);
            return ch;

        case '\t': /* Tab */
            /* TODO: Handle tab completion */
            break;

        case 127: /* Backspace */
            if (e->buffer_pos > 0) {
                e->buffer_pos--;
                e->buffer[e->buffer_pos] = '\0';
                /* Move cursor back and erase character */
                if (e->cursor_x > strlen(e->prompt)) {
                    terminal_move_cursor(e->cursor_x - 1, e->cursor_y);
                    printf(" ");
                    terminal_move_cursor(e->cursor_x - 1, e->cursor_y);
                    e->cursor_x--;
                }
            }
            break;

        default:
            /* Add printable characters to buffer */
            if (isprint(ch) && e->buffer_pos < EDITLINE_BUFSIZE - 1) {
                e->buffer[e->buffer_pos] = ch;
                e->buffer_pos++;
                e->buffer[e->buffer_pos] = '\0';
                /* Print character and advance cursor */
                printf("%c", ch);
                fflush(stdout);
                e->cursor_x++;
            }
            break;
        }
    }

    return ch;
}

/* Get current buffer contents */
const char *editline_get_buffer(editline e)
{
    return e ? e->buffer : "";
}

/* Clear the buffer */
void editline_clear(editline e)
{
    if (e == NULL) return;

    /* Clear buffer */
    e->buffer[0] = '\0';
    e->buffer_pos = 0;

    /* Clear line and reprint prompt */
    terminal_clear_line();
    terminal_move_cursor(0, e->cursor_y);
    printf("%s", e->prompt);
    fflush(stdout);
    e->cursor_x = strlen(e->prompt);
}

/* Set prompt */
void editline_set_prompt(editline e, const char *prompt)
{
    if (e == NULL) return;

    free(e->prompt);
    e->prompt = strdup(prompt ? prompt : "");

    /* Print new prompt */
    terminal_move_cursor(0, e->cursor_y);
    printf("%s", e->prompt);
    fflush(stdout);
    e->cursor_x = strlen(e->prompt);
}

/* Get current prompt */
const char *editline_get_prompt(editline e)
{
    return e ? e->prompt : "";
}

/* Set cursor position */
void editline_set_position(editline e, int x, int y)
{
    if (e == NULL) return;
    e->cursor_x = x;
    e->cursor_y = y;
    terminal_move_cursor(x, y);
}

/* Get cursor position */
void editline_get_position(editline e, int *x, int *y)
{
    if (e == NULL || x == NULL || y == NULL) return;
    *x = e->cursor_x;
    *y = e->cursor_y;
}

/* Set completion callback */
void editline_set_completion_callback(editline e, completion_callback callback)
{
    if (e == NULL) return;
    e->completion_fn = callback;
}

void editline_set_buffer(editline e, const char *buffer)
{
    if (e == NULL || buffer == NULL) return;
    
    /* Copy buffer safely */
    strncpy(e->buffer, buffer, EDITLINE_BUFSIZE - 1);
    e->buffer[EDITLINE_BUFSIZE - 1] = '\0';
    e->buffer_pos = strlen(e->buffer);
}
