/*
 * Simple input example - demonstrates basic input handling
 */

#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    if (!terminal_init()) {
        fprintf(stderr, "Failed to initialize terminal\n");
        return 1;
    }

    terminal_set_status_colors();
    printf("Type characters (press 'q' to quit)\n");
    terminal_set_default_colors();
    
    while (1) {
        int ch = terminal_getch();
        if (ch == 'q') break;
        
        if (ch != -1) {
            terminal_set_text_colors();
            printf("You typed: %c (ASCII: %d)\n", ch, ch);
            terminal_set_default_colors();
        }
    }

    terminal_cleanup();
    return 0;
}
