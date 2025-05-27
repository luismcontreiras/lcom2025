#ifndef GAME_H
#define GAME_H

#define ARROW_UP_SCANCODE    0x48
#define ARROW_DOWN_SCANCODE  0x50
#define ARROW_LEFT_SCANCODE  0x4B
#define ARROW_RIGHT_SCANCODE 0x4D
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
