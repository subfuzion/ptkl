/*
 * Simple text editor - demonstrates cursor movement, line editing, and screen management
 */

#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 200

typedef struct {
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int num_lines;
    int cursor_x;
    int cursor_y;
    int scroll_offset;
} Editor;

static Editor editor;
static int term_width, term_height;

void init_editor(void)
{
    editor.num_lines = 1;
    editor.cursor_x = 0;
    editor.cursor_y = 0;
    editor.scroll_offset = 0;
    memset(editor.lines, 0, sizeof(editor.lines));
}

void draw_status_bar(void)
{
    terminal_move_cursor(0, term_height - 1);
    terminal_set_status_colors();
    
    char status[MAX_LINE_LENGTH];
    snprintf(status, sizeof(status), "Line %d/%d  Col %d  [Press Ctrl+Q to quit]",
             editor.cursor_y + 1, editor.num_lines, editor.cursor_x + 1);
             
    printf("%-*s", term_width, status);
    terminal_set_default_colors();
}

void draw_editor(void)
{
    terminal_clear();
    terminal_set_text_colors();
    
    /* Calculate visible lines */
    int visible_lines = term_height - 1;  /* -1 for status bar */
    int start_line = editor.scroll_offset;
    int end_line = start_line + visible_lines;
    if (end_line > editor.num_lines) end_line = editor.num_lines;
    
    /* Draw lines */
    for (int i = start_line; i < end_line; i++) {
        terminal_move_cursor(0, i - start_line);
        printf("%s", editor.lines[i]);
    }
    
    /* Draw status bar */
    draw_status_bar();
    
    /* Position cursor */
    terminal_move_cursor(editor.cursor_x, editor.cursor_y - editor.scroll_offset);
}

void insert_char(char c)
{
    if (editor.cursor_x >= MAX_LINE_LENGTH - 1) return;
    
    /* Make room for the new character */
    memmove(&editor.lines[editor.cursor_y][editor.cursor_x + 1],
            &editor.lines[editor.cursor_y][editor.cursor_x],
            strlen(&editor.lines[editor.cursor_y][editor.cursor_x]));
            
    /* Insert the character */
    editor.lines[editor.cursor_y][editor.cursor_x] = c;
    editor.cursor_x++;
}

void handle_newline(void)
{
    if (editor.num_lines >= MAX_LINES) return;
    
    /* Make room for new line */
    memmove(&editor.lines[editor.cursor_y + 2],
            &editor.lines[editor.cursor_y + 1],
            (editor.num_lines - editor.cursor_y - 1) * MAX_LINE_LENGTH);
            
    /* Copy rest of current line to new line */
    strcpy(editor.lines[editor.cursor_y + 1],
           &editor.lines[editor.cursor_y][editor.cursor_x]);
           
    /* Truncate current line */
    editor.lines[editor.cursor_y][editor.cursor_x] = '\0';
    
    editor.num_lines++;
    editor.cursor_y++;
    editor.cursor_x = 0;
    
    /* Scroll if needed */
    if (editor.cursor_y - editor.scroll_offset >= term_height - 1) {
        editor.scroll_offset++;
    }
}

void handle_backspace(void)
{
    if (editor.cursor_x > 0) {
        /* Remove character */
        memmove(&editor.lines[editor.cursor_y][editor.cursor_x - 1],
                &editor.lines[editor.cursor_y][editor.cursor_x],
                strlen(&editor.lines[editor.cursor_y][editor.cursor_x]) + 1);
        editor.cursor_x--;
    } else if (editor.cursor_y > 0) {
        /* Merge with previous line */
        int prev_len = strlen(editor.lines[editor.cursor_y - 1]);
        strcat(editor.lines[editor.cursor_y - 1], editor.lines[editor.cursor_y]);
        
        /* Remove current line */
        memmove(&editor.lines[editor.cursor_y],
                &editor.lines[editor.cursor_y + 1],
                (editor.num_lines - editor.cursor_y - 1) * MAX_LINE_LENGTH);
        editor.num_lines--;
        editor.cursor_y--;
        editor.cursor_x = prev_len;
        
        if (editor.scroll_offset > 0) editor.scroll_offset--;
    }
}

int main(void)
{
    if (!terminal_init()) {
        fprintf(stderr, "Failed to initialize terminal\n");
        return 1;
    }
    
    terminal_get_size(&term_width, &term_height);
    init_editor();
    
    while (1) {
        draw_editor();
        
        int ch = terminal_getch();
        if (ch == 17) break;  /* Ctrl+Q */
        
        switch (ch) {
            case 27:  /* ESC sequence */
                if (terminal_getch() == '[') {
                    switch (terminal_getch()) {
                        case 'A':  /* Up */
                            if (editor.cursor_y > 0) {
                                editor.cursor_y--;
                                if (editor.cursor_y < editor.scroll_offset) {
                                    editor.scroll_offset--;
                                }
                            }
                            break;
                            
                        case 'B':  /* Down */
                            if (editor.cursor_y < editor.num_lines - 1) {
                                editor.cursor_y++;
                                if (editor.cursor_y - editor.scroll_offset >= term_height - 1) {
                                    editor.scroll_offset++;
                                }
                            }
                            break;
                            
                        case 'C':  /* Right */
                            if (editor.cursor_x < strlen(editor.lines[editor.cursor_y])) {
                                editor.cursor_x++;
                            }
                            break;
                            
                        case 'D':  /* Left */
                            if (editor.cursor_x > 0) {
                                editor.cursor_x--;
                            }
                            break;
                    }
                }
                break;
                
            case '\r':
            case '\n':
                handle_newline();
                break;
                
            case 127:  /* Backspace */
                handle_backspace();
                break;
                
            default:
                if (ch >= 32 && ch < 127) {  /* Printable characters */
                    insert_char(ch);
                }
                break;
        }
    }
    
    terminal_cleanup();
    return 0;
}
