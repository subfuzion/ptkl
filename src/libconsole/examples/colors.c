/*
 * Colors example - demonstrates all available colors in a grid
 */

#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Get number of colors based on TERM */
static int get_color_count(void)
{
    const char *term = getenv("TERM");
    if (term == NULL) return 2;  /* Default to monochrome */
    
    if (strcmp(term, "vt100") == 0) {
        return 2;  /* Monochrome: black and white */
    } else if (strstr(term, "256color") != NULL) {
        return 256;  /* 256 colors */
    } else if (strstr(term, "color") != NULL || strstr(term, "xterm") != NULL) {
        return 16;  /* 16 colors (8 basic + 8 bright) */
    }
    
    return 2;  /* Default to monochrome */
}

/* Print a grid of colors */
static void print_color_grid(int num_colors)
{
    /* Buffer for building lines */
    char line[1024];
    
    /* Calculate grid dimensions */
    int grid_size = (int)sqrt(num_colors);
    if (grid_size * grid_size < num_colors) grid_size++;
    
    /* Get terminal size */
    int term_width, term_height;
    terminal_get_size(&term_width, &term_height);
    
    /* Clear screen and reset cursor */
    terminal_clear();
    terminal_move_cursor(0, 0);
    
    /* Print header */
    terminal_set_status_colors();
    printf("Terminal supports %d colors (TERM=%s)\n", 
           num_colors, getenv("TERM") ? getenv("TERM") : "unknown");
    terminal_set_default_colors();
    printf("\n");
    
    /* Limit grid size based on terminal dimensions */
    int max_grid_size = term_height - 10;  /* Leave room for color info */
    if (grid_size > max_grid_size) {
        grid_size = max_grid_size;
    }
    
    /* Print grid */
    for (int y = 0; y < grid_size && y * grid_size < num_colors; y++) {
        terminal_move_cursor(0, y + 2);  /* +2 for header */
        terminal_clear_line();
        
        for (int x = 0; x < grid_size; x++) {
            int color = y * grid_size + x;
            if (color < num_colors) {
                terminal_set_color(color, TERM_COLOR_DEFAULT);
                printf("█ ");  /* Unicode full block + space */
                terminal_set_default_colors();
            }
        }
        fflush(stdout);
    }
    
    /* Calculate space needed for legend */
    int legend_start = grid_size + 4;
    int legend_space = term_height - legend_start - 2;
    
    /* Print color information */
    terminal_move_cursor(0, legend_start);
    if (num_colors <= 2) {
        if (legend_space >= 3) {
            terminal_clear_line();
            sprintf(line, "Monochrome mode (2 colors)");
            printf("%s\n", line);
            fflush(stdout);
            
            terminal_move_cursor(0, legend_start + 1);
            terminal_clear_line();
            terminal_set_color(TERM_COLOR_DEFAULT, TERM_COLOR_DEFAULT);
            sprintf(line, "█ - Foreground");
            printf("%s\n", line);
            fflush(stdout);
            
            terminal_move_cursor(0, legend_start + 2);
            terminal_clear_line();
            sprintf(line, "  - Background");
            printf("%s\n", line);
            fflush(stdout);
        }
    } else {
        const char *basic_names[] = {
            "Black", "Red", "Green", "Yellow",
            "Blue", "Magenta", "Cyan", "White"
        };
        
        if (legend_space >= 9) {
            terminal_clear_line();
            sprintf(line, "Basic colors (0-7)");
            printf("%s\n", line);
            fflush(stdout);
            
            for (int i = 0; i < 8; i++) {
                terminal_move_cursor(0, legend_start + 1 + i);
                terminal_clear_line();
                terminal_set_color(i, TERM_COLOR_DEFAULT);
                sprintf(line, "█ - %s", basic_names[i]);
                printf("%s\n", line);
                fflush(stdout);
                terminal_set_default_colors();
            }
        }
        
        if (num_colors >= 16 && legend_space >= 18) {
            terminal_move_cursor(0, legend_start + 10);
            terminal_clear_line();
            sprintf(line, "Bright colors (8-15)");
            printf("%s\n", line);
            fflush(stdout);
            
            for (int i = 8; i < 16; i++) {
                terminal_move_cursor(0, legend_start + 11 + (i-8));
                terminal_clear_line();
                terminal_set_color(i, TERM_COLOR_DEFAULT);
                sprintf(line, "█ - Bright %s", basic_names[i-8]);
                printf("%s\n", line);
                fflush(stdout);
                terminal_set_default_colors();
            }
        }
        
        if (num_colors > 16 && legend_space >= 19) {
            terminal_move_cursor(0, legend_start + 20);
            terminal_clear_line();
            sprintf(line, "Additional colors: %d", num_colors - 16);
            printf("%s\n", line);
            fflush(stdout);
        }
    }
    
    /* Move cursor to the end */
    terminal_move_cursor(0, term_height - 1);
}

int main(void)
{
    if (!terminal_init()) {
        fprintf(stderr, "Failed to initialize terminal\n");
        return 1;
    }
    
    /* Make stdout line-buffered */
    setvbuf(stdout, NULL, _IOLBF, 0);
    
    /* Clear screen and hide cursor while drawing */
    terminal_clear();
    terminal_hide_cursor();
    
    /* Print colors */
    int num_colors = get_color_count();
    print_color_grid(num_colors);
    
    /* Show cursor and move to bottom */
    terminal_show_cursor();
    printf("\n");  /* Add final newline */
    
    /* Clean up */
    terminal_cleanup();
    return 0;
}
