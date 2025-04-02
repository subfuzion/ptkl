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
    /* Calculate grid dimensions */
    int grid_size = (int)sqrt(num_colors);
    if (grid_size * grid_size < num_colors) grid_size++;
    
    /* Get terminal size */
    int term_width, term_height;
    terminal_get_size(&term_width, &term_height);
    
    /* Print header */
    terminal_set_status_colors();
    printf("Terminal supports %d colors (TERM=%s)\n", 
           num_colors, getenv("TERM") ? getenv("TERM") : "unknown");
    terminal_set_default_colors();
    printf("\n");
    
    /* Print grid */
    for (int y = 0; y < grid_size; y++) {
        terminal_move_cursor(0, y + 2);  /* +2 to account for header */
        for (int x = 0; x < grid_size; x++) {
            int color = y * grid_size + x;
            if (color < num_colors) {
                terminal_set_color(color, TERM_COLOR_DEFAULT);
                printf("█");  /* Unicode full block */
                terminal_set_default_colors();
                printf(" ");
            }
        }
    }
    
    /* Print color information */
    terminal_move_cursor(0, grid_size + 3);  /* +3 for header and gap */
    
    if (num_colors <= 2) {
        printf("Monochrome mode (2 colors):");
        terminal_move_cursor(0, grid_size + 4);
        printf("█ - Foreground");
        terminal_move_cursor(0, grid_size + 5);
        printf("  - Background");
    } else {
        const char *basic_names[] = {
            "Black", "Red", "Green", "Yellow",
            "Blue", "Magenta", "Cyan", "White"
        };
        
        printf("Basic colors (0-7):");
        for (int i = 0; i < 8; i++) {
            terminal_move_cursor(0, grid_size + 4 + i);
            terminal_set_color(i, TERM_COLOR_DEFAULT);
            printf("█ ");
            terminal_set_default_colors();
            printf("- %s", basic_names[i]);
        }
        
        if (num_colors >= 16) {
            terminal_move_cursor(0, grid_size + 13);
            printf("Bright colors (8-15):");
            for (int i = 8; i < 16; i++) {
                terminal_move_cursor(0, grid_size + 14 + (i-8));
                terminal_set_color(i, TERM_COLOR_DEFAULT);
                printf("█ ");
                terminal_set_default_colors();
                printf("- Bright %s", basic_names[i-8]);
            }
        }
        
        if (num_colors > 16) {
            terminal_move_cursor(0, grid_size + 23);
            printf("Additional colors: %d", num_colors - 16);
        }
    }
}

int main(void)
{
    if (!terminal_init()) {
        fprintf(stderr, "Failed to initialize terminal\n");
        return 1;
    }
    
    int num_colors = get_color_count();
    print_color_grid(num_colors);
    
    terminal_cleanup();
    return 0;
}
