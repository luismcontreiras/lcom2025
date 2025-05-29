#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

#define ARROW_UP_SCANCODE    0x48
#define ARROW_DOWN_SCANCODE  0x50
#define ARROW_LEFT_SCANCODE  0x4B
#define ARROW_RIGHT_SCANCODE 0x4D

// WASD scancodes for second player
#define W_SCANCODE           0x11
#define A_SCANCODE           0x1E
#define S_SCANCODE           0x1F
#define D_SCANCODE           0x20

// Trail system constants
#define MAX_TRAIL_SEGMENTS   10000
#define TRAIL_THICKNESS      4

// Trail colors
#define BLUE_TRAIL_COLOR     0x0080FF
#define ORANGE_TRAIL_COLOR   0xFF8000

// Trail segment structure - using line segments
typedef struct {
    uint16_t x1, y1;   // Start point
    uint16_t x2, y2;   // End point
    uint16_t thickness;
    uint32_t color;
    bool active;
} trail_segment_t;
/**
 * @brief Initialize and run the game
 * 
 * This function initializes the game, creates a player sprite,
 * sets up event handlers for keyboard input, and runs the main game loop.
 * 
 * @return 0 on success, non-zero on failure
 */
int game_init();

#endif // GAME_H
