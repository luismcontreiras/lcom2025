#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

// Keyboard scancodes
#define ARROW_UP_SCANCODE    0x48
#define ARROW_DOWN_SCANCODE  0x50
#define ARROW_LEFT_SCANCODE  0x4B
#define ARROW_RIGHT_SCANCODE 0x4D
#define W_SCANCODE           0x11
#define S_SCANCODE           0x1F
#define A_SCANCODE           0x1E
#define D_SCANCODE           0x20
#define SPACE_SCANCODE       0x39
#define ENTER_SCANCODE       0x1C

// Game constants
#define GRID_SIZE 4
#define TRAIL_SIZE 3
#define PLAYER_SPEED 3
#define GAME_WIDTH 800
#define GAME_HEIGHT 600

// Game states
typedef enum {
    GAME_MENU,
    GAME_PLAYING,
    GAME_OVER,
    GAME_PAUSED
} game_state_t;

// Direction enumeration
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} direction_t;

// Player structure
typedef struct {
    uint16_t x, y;
    direction_t direction;
    direction_t next_direction;
    uint32_t color;
    bool alive;
    uint8_t sprite_id;
} player_t;

// Trail segment
typedef struct {
    uint16_t x, y;
} trail_segment_t;

// Game data structure
typedef struct {
    game_state_t state;
    player_t player1;
    player_t player2;
    trail_segment_t trail1[10000];
    trail_segment_t trail2[10000];
    uint16_t trail1_length;
    uint16_t trail2_length;
    uint32_t winner; // 0 = draw, 1 = player1, 2 = player2
    uint32_t frame_counter;
} tron_game_t;

/**
 * @brief Initialize and run the Tron game
 * 
 * This function initializes the game engine and runs the Tron game
 * with two players controlling light cycles.
 * 
 * @return 0 on success, non-zero on failure
 */
int game_init();

#endif // GAME_H
