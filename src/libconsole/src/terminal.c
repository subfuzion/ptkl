/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

/* Static variables */
static struct termios orig_termios;  /* Original terminal settings */
static bool raw_mode = false;        /* Current terminal mode */
static int current_fg = TERM_COLOR_DEFAULT;
static int current_bg = TERM_COLOR_DEFAULT;

/* ANSI escape sequences */
#define ESC "\033"
#define CSI ESC "["

/* Initialize terminal */
bool terminal_init(void)
{
    /* Get current terminal settings */
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        return false;
    }

    /* Check terminal size */
    int width, height;
    terminal_get_size(&width, &height);
    if (width < MIN_TERM_WIDTH || height < MIN_TERM_HEIGHT) {
        return false;
    }

    /* Set raw mode */
    terminal_set_mode(TERM_MODE_RAW);
    terminal_clear();
    terminal_show_cursor();

    return true;
}

/* Clean up terminal */
void terminal_cleanup(void)
{
    /* Restore original terminal settings */
    if (raw_mode) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
        raw_mode = false;
    }

    /* Reset colors and show cursor */
    terminal_reset_color();
    terminal_show_cursor();
}

/* Get character from terminal */
int terminal_getch(void)
{
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        return c;
    }
    return -1;
}

/* Check if input is available */
bool terminal_kbhit(void)
{
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    if (flags == -1) return false;

    /* Set non-blocking mode */
    if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1) {
        return false;
    }

    /* Try to read a character */
    char c;
    int result = read(STDIN_FILENO, &c, 1);

    /* Restore blocking mode */
    fcntl(STDIN_FILENO, F_SETFL, flags);

    /* Push back the character if we read one */
    if (result == 1) {
        ungetc(c, stdin);
        return true;
    }

    return false;
}

/* Clear entire screen */
void terminal_clear(void)
{
    printf(CSI "2J" CSI "H");
    fflush(stdout);
}

/* Clear current line */
void terminal_clear_line(void)
{
    printf(CSI "2K" CSI "G");
    fflush(stdout);
}

/* Move cursor to position */
void terminal_move_cursor(int x, int y)
{
    printf(CSI "%d;%dH", y + 1, x + 1);
    fflush(stdout);
}

/* Hide cursor */
void terminal_hide_cursor(void)
{
    printf(CSI "?25l");
    fflush(stdout);
}

/* Show cursor */
void terminal_show_cursor(void)
{
    printf(CSI "?25h");
    fflush(stdout);
}

/* Get terminal size */
void terminal_get_size(int *width, int *height)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        *width = MIN_TERM_WIDTH;
        *height = MIN_TERM_HEIGHT;
    } else {
        *width = ws.ws_col;
        *height = ws.ws_row;
    }
}

/* Set terminal mode */
void terminal_set_mode(int mode)
{
    if (mode == TERM_MODE_RAW && !raw_mode) {
        struct termios raw = orig_termios;

        /* Input modes: no break, no CR to NL, no parity check, no strip char */
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

        /* Output modes - disable post processing */
        raw.c_oflag &= ~(OPOST);

        /* Control modes - set 8 bit chars */
        raw.c_cflag |= (CS8);

        /* Local modes - no echoing, no canonical processing */
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

        /* Control chars - set return condition: min number of bytes and timer */
        raw.c_cc[VMIN] = 0;  /* Return each byte */
        raw.c_cc[VTIME] = 1; /* 100 ms timeout */

        /* Put terminal in raw mode */
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == 0) {
            raw_mode = true;
        }
    } else if (mode == TERM_MODE_COOKED && raw_mode) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
        raw_mode = false;
    }
}

/* Set terminal colors */
void terminal_set_color(int fg, int bg)
{
    if (fg != current_fg || bg != current_bg) {
        printf(CSI "38;5;%dm" CSI "48;5;%dm", fg, bg);
        fflush(stdout);
        current_fg = fg;
        current_bg = bg;
    }
}

/* Reset terminal colors */
void terminal_reset_color(void)
{
    printf(CSI "0m");
    fflush(stdout);
    current_fg = TERM_COLOR_DEFAULT;
    current_bg = TERM_COLOR_DEFAULT;
}
