#include "game.h"
#include <stdio.h>
#include <stdlib.h>

// External XPM declarations
extern xpm_map_t player_xpm;

// Game state
static Player player;
static Engine* game_engine = NULL;

int game_init(Engine* engine) {
    if (engine == NULL) return 1;
    
    game_engine = engine;
    
    // Add background sprite (if available)
    /*
    Vector2 bg_pos = {0, 0};
    int bg_index = engine_add_sprite(engine, background_xpm, XPM_8_8_8, bg_pos);
    if (bg_index < 0) {
        printf("Failed to add background sprite\n");
        // Not fatal, continue
    }
    */
    
    // Add player sprite
    Vector2 player_pos = {engine->screen_width / 2, engine->screen_height / 2};
    player.sprite_index = engine_add_sprite(engine, player_xpm, XPM_8_8_8, player_pos);
    if (player.sprite_index < 0) {
        printf("Failed to add player sprite\n");
        return 1;
    }
    
    player.speed = 5;
    
    // Register event handlers
    if (engine_register_handler(engine, EVENT_KEYBOARD, game_keyboard_handler, &player) != 0) {
        printf("Failed to register keyboard handler\n");
        return 1;
    }
    
    if (engine_register_handler(engine, EVENT_TIMER, game_timer_handler, &player) != 0) {
        printf("Failed to register timer handler\n");
        return 1;
    }
    
    printf("Game initialized successfully\n");
    return 0;
}

void game_keyboard_handler(Event* event, void* user_data) {
    if (event->type != EVENT_KEYBOARD || game_engine == NULL) return;
    
    Player* player = (Player*)user_data;
    if (player == NULL) return;
    
    // Only process key press events (make codes)
    if (!event->data.keyboard.make_code) return;
    
    Sprite* sprite = sprite_manager_get(&game_engine->sprite_manager, player->sprite_index);
    if (sprite == NULL) return;
    
    Vector2 delta = {0, 0};
    
    // Handle arrow keys
    switch (event->data.keyboard.scancode) {
        case KEY_ARROW_UP:
            delta.y = -player->speed;
            break;
        case KEY_ARROW_DOWN:
            delta.y = player->speed;
            break;
        case KEY_ARROW_LEFT:
            delta.x = -player->speed;
            break;
        case KEY_ARROW_RIGHT:
            delta.x = player->speed;
            break;
        default:
            return; // Ignore other keys
    }
    
    // Apply movement if any
    if (delta.x != 0 || delta.y != 0) {
        // Move the sprite
        sprite_move(sprite, delta);
        
        // Check boundaries
        sprite_check_bounds(sprite, game_engine->screen_width, game_engine->screen_height);
    }
}

void game_timer_handler(Event* event, void* user_data) {
    // This could be used for automatic movement or animations
    // Not used in this simple example
}

void game_cleanup(Engine* engine) {
    // Any game-specific cleanup would go here
    game_engine = NULL;
}
