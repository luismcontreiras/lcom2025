#ifndef GAME_H
#define GAME_H

#include <stdint.h>

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
