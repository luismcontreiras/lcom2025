#ifndef GAME_H
#define GAME_H

#include "../engine/core/engine.h"

/**
 * @brief Simple player structure for the game
 */
typedef struct {
    int sprite_index;   // Index of the player sprite in the engine
    int speed;          // Movement speed
    Vector2 position;   // Current position
} Player;

/**
 * @brief Game state structure
 */
typedef struct {
    Player player;
    Engine* engine;
    bool game_over;
} GameState;

/**
 * @brief Initialize the game
 * @param engine The game engine
 * @return 0 on success, non-zero otherwise
 */
int game_init(Engine* engine);

/**
 * @brief Keyboard handler for the game
 * @param event Event data
 * @param user_data Game data
 */
void game_keyboard_handler(Event* event, void* user_data);

/**
 * @brief Timer handler for the game
 * @param event Event data
 * @param user_data Game data
 */
void game_timer_handler(Event* event, void* user_data);

/**
 * @brief Clean up game resources
 * @param engine The game engine
 */
void game_cleanup(Engine* engine);

/**
 * @brief Process game logic
 * @param state Game state
 * @param delta_time Time elapsed since last update
 */
void game_update(GameState* state, float delta_time);

#endif /* GAME_H */
