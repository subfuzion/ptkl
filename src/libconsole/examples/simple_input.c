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

    printf("Type characters (press 'q' to quit):\n");
    
    while (1) {
        int ch = terminal_getch();
        if (ch == 'q') break;
        
        if (ch != -1) {
            printf("You typed: %c (ASCII: %d)\n", ch, ch);
        }
    }

    terminal_cleanup();
    return 0;
}
