/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#ifndef PTKL_EDITLINE_H
#define PTKL_EDITLINE_H

#include <stdbool.h>

typedef struct editline_t *editline;

/* Create and destroy editline instance */
editline editline_new(void);
void editline_free(editline e);

/* Initialize and cleanup */
bool editline_init(editline e, const char *prompt);
void editline_cleanup(editline e);

/* Input handling */
int editline_getch(editline e);
const char *editline_get_buffer(editline e);
void editline_clear(editline e);

/* Prompt management */
void editline_set_prompt(editline e, const char *prompt);
const char *editline_get_prompt(editline e);

/* Completion support */
typedef char** (*completion_callback)(const char *prefix, int *count);
void editline_set_completion_callback(editline e, completion_callback callback);

#endif /* PTKL_EDITLINE_H */
