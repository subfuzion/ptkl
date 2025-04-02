/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ncurses.h>
#include "editline.h"

/* Buffer size for line editing */
#define EDITLINE_BUFSIZE 4096

struct editline_t {
    char *prompt;              /* Current prompt */
    char *buffer;             /* Current line buffer */
    int buffer_pos;           /* Current position in buffer */
    completion_callback completion_fn; /* User-provided completion callback */
    bool initialized;         /* Whether readline is initialized */
};

/* Forward declarations */
static char **editline_completion(const char *text, int start, int end);
static char *editline_completion_generator(const char *text, int state);

/* Create new editline instance */
editline editline_new(void)
{
    editline e = calloc(1, sizeof(struct editline_t));
    if (e == NULL) return NULL;

    e->prompt = strdup("");
    e->buffer = calloc(1, EDITLINE_BUFSIZE);
    e->buffer_pos = 0;
    e->completion_fn = NULL;
    e->initialized = false;

    return e;
}

/* Free editline instance */
void editline_free(editline e)
{
    if (e == NULL) return;
    
    editline_cleanup(e);
    free(e->prompt);
    free(e->buffer);
    free(e);
}

/* Initialize readline */
bool editline_init(editline e, const char *prompt)
{
    if (e == NULL) return false;

    /* Configure readline to avoid conflicts with ncurses */
    /* Configure readline to avoid conflicts with ncurses */
    rl_variable_bind("disable-completion", "on");
    rl_bind_key('\t', rl_insert); /* Make tab just insert a tab */
    rl_prep_terminal(0); /* Initialize terminal */

    /* Set up completion */
    rl_attempted_completion_function = editline_completion;

    /* Set initial prompt */
    editline_set_prompt(e, prompt);

    /* Install callback handler */
    rl_callback_handler_install(e->prompt, NULL);

    e->initialized = true;
    return true;
}

/* Clean up readline */
void editline_cleanup(editline e)
{
    if (e == NULL || !e->initialized) return;
    
    rl_callback_handler_remove();
    e->initialized = false;
}

/* Get character input */
int editline_getch(editline e)
{
    if (e == NULL || !e->initialized) return ERR;

    int ch = getch();
    if (ch != ERR) {
        /* Let readline process the character */
        rl_stuff_char(ch);
        rl_callback_read_char();

        /* Copy readline's buffer to our buffer */
        strncpy(e->buffer, rl_line_buffer, EDITLINE_BUFSIZE - 1);
        e->buffer_pos = rl_point;
    }

    return ch;
}

/* Get current buffer contents */
const char *editline_get_buffer(editline e)
{
    return e ? e->buffer : NULL;
}

/* Clear the buffer */
void editline_clear(editline e)
{
    if (e == NULL) return;

    /* Clear our buffer */
    memset(e->buffer, 0, EDITLINE_BUFSIZE);
    e->buffer_pos = 0;

    /* Clear readline's buffer */
    strcpy(rl_line_buffer, "");
    rl_point = 0;
    rl_redisplay();
}

/* Set prompt */
void editline_set_prompt(editline e, const char *prompt)
{
    if (e == NULL) return;

    free(e->prompt);
    e->prompt = strdup(prompt ? prompt : "");

    if (e->initialized) {
        rl_set_prompt(e->prompt);
        rl_redisplay();
    }
}

/* Get current prompt */
const char *editline_get_prompt(editline e)
{
    return e ? e->prompt : NULL;
}

/* Set completion callback */
void editline_set_completion_callback(editline e, completion_callback callback)
{
    if (e == NULL) return;
    e->completion_fn = callback;
}

/* Readline completion function */
static char **editline_completion(const char *text, int start, int end)
{
    /* We don't use readline's closure mechanism */
    editline e = NULL;
    if (e == NULL || e->completion_fn == NULL) return NULL;

    int count;
    char **matches = e->completion_fn(text, &count);
    
    if (matches == NULL || count == 0) {
        /* No matches - let readline use its default completion */
        rl_attempted_completion_over = 0;
        return NULL;
    }

    /* Return matches in readline's expected format */
    char **rl_matches = calloc(count + 2, sizeof(char *));
    rl_matches[0] = strdup(text); /* Original text */
    for (int i = 0; i < count; i++) {
        rl_matches[i + 1] = strdup(matches[i]);
    }
    rl_matches[count + 1] = NULL;

    /* Free original matches */
    for (int i = 0; i < count; i++) {
        free(matches[i]);
    }
    free(matches);

    return rl_matches;
}
