/*
 * Snake game - demonstrates cursor movement, colors, and non-blocking input
 */

#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define GAME_WIDTH 40
#define GAME_HEIGHT 20
#define INITIAL_LENGTH 4
#define GAME_DELAY 100000  // 100ms

typedef struct {
    int x, y;
} Point;

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct {
    Point segments[GAME_WIDTH * GAME_HEIGHT];
    int length;
    Direction dir;
} Snake;

static Snake snake;
static Point food;
static bool game_over = false;

void init_game(void)
{
    /* Initialize snake in the middle of the screen */
    int mid_x = GAME_WIDTH / 2;
    int mid_y = GAME_HEIGHT / 2;
    
    snake.length = INITIAL_LENGTH;
    snake.dir = DIR_RIGHT;
    
    for (int i = 0; i < snake.length; i++) {
        snake.segments[i].x = mid_x - i;
        snake.segments[i].y = mid_y;
    }
    
    /* Place initial food */
    srand(time(NULL));
    food.x = rand() % GAME_WIDTH;
    food.y = rand() % GAME_HEIGHT;
}

void draw_game(void)
{
    terminal_clear();
    
    /* Draw border */
    terminal_set_text_colors();
    for (int y = 0; y < GAME_HEIGHT + 2; y++) {
        for (int x = 0; x < GAME_WIDTH + 2; x++) {
            terminal_move_cursor(x, y);
            if (y == 0 || y == GAME_HEIGHT + 1 || x == 0 || x == GAME_WIDTH + 1) {
                printf("#");
            }
        }
    }
    
    /* Draw snake */
    terminal_set_color(TERM_COLOR_GREEN, TERM_COLOR_DEFAULT);
    for (int i = 0; i < snake.length; i++) {
        terminal_move_cursor(snake.segments[i].x + 1, snake.segments[i].y + 1);
        printf("O");
    }
    
    /* Draw food */
    terminal_set_color(TERM_COLOR_RED, TERM_COLOR_DEFAULT);
    terminal_move_cursor(food.x + 1, food.y + 1);
    printf("*");
    
    /* Draw status */
    terminal_move_cursor(0, GAME_HEIGHT + 3);
    terminal_set_status_colors();
    printf("Score: %d  (Use arrow keys to move, 'q' to quit)", snake.length - INITIAL_LENGTH);
    terminal_set_default_colors();
    fflush(stdout);
}

void update_game(void)
{
    /* Move snake body */
    for (int i = snake.length - 1; i > 0; i--) {
        snake.segments[i] = snake.segments[i - 1];
    }
    
    /* Move snake head */
    switch (snake.dir) {
        case DIR_UP:    snake.segments[0].y--; break;
        case DIR_DOWN:  snake.segments[0].y++; break;
        case DIR_LEFT:  snake.segments[0].x--; break;
        case DIR_RIGHT: snake.segments[0].x++; break;
    }
    
    /* Check wall collision */
    if (snake.segments[0].x < 0 || snake.segments[0].x >= GAME_WIDTH ||
        snake.segments[0].y < 0 || snake.segments[0].y >= GAME_HEIGHT) {
        game_over = true;
        return;
    }
    
    /* Check self collision */
    for (int i = 1; i < snake.length; i++) {
        if (snake.segments[0].x == snake.segments[i].x &&
            snake.segments[0].y == snake.segments[i].y) {
            game_over = true;
            return;
        }
    }
    
    /* Check food collision */
    if (snake.segments[0].x == food.x && snake.segments[0].y == food.y) {
        snake.length++;
        food.x = rand() % GAME_WIDTH;
        food.y = rand() % GAME_HEIGHT;
    }
}

int main(void)
{
    if (!terminal_init()) {
        fprintf(stderr, "Failed to initialize terminal\n");
        return 1;
    }
    
    terminal_hide_cursor();
    init_game();
    
    while (!game_over) {
        if (terminal_kbhit()) {
            int ch = terminal_getch();
            switch (ch) {
                case 'q': game_over = true; break;
                case 27:  /* ESC sequence */
                    if (terminal_getch() == '[') {
                        switch (terminal_getch()) {
                            case 'A': if (snake.dir != DIR_DOWN)  snake.dir = DIR_UP;    break;
                            case 'B': if (snake.dir != DIR_UP)    snake.dir = DIR_DOWN;  break;
                            case 'C': if (snake.dir != DIR_LEFT)  snake.dir = DIR_RIGHT; break;
                            case 'D': if (snake.dir != DIR_RIGHT) snake.dir = DIR_LEFT;  break;
                        }
                    }
                    break;
            }
        }
        
        update_game();
        draw_game();
        usleep(GAME_DELAY);
    }
    
    /* Game over screen */
    terminal_move_cursor(0, GAME_HEIGHT + 4);
    printf("Game Over! Final score: %d\n", snake.length - INITIAL_LENGTH);
    
    terminal_show_cursor();
    terminal_cleanup();
    return 0;
}
