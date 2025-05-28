#include "game.h"
#include <lcom/lcf.h>
#include "../engine/game_engine.h"

// XPM for player 1 sprite (cyan light cycle)
static char* const player1_xpm[] = {
    "8 8 2 1",
    "  c None",
    "X c #00FFFF",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX"
};

// XPM for player 2 sprite (orange light cycle)
static char* const player2_xpm[] = {
    "8 8 2 1",
    "  c None",
    "X c #FF8800",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX",
    "XXXXXXXX"
};

// Global game state
static tron_game_t game_data;
static game_engine_t *game_engine;

// Function declarations
void tron_update(game_engine_t *engine, float delta_time);
void tron_render(game_engine_t *engine);
void reset_game();
void handle_input();
void update_players();
void draw_trails();
void check_collisions();
void draw_ui();
bool is_collision(uint16_t x, uint16_t y);

void reset_game() {
    game_data.state = GAME_PLAYING;
    game_data.frame_counter = 0;
    game_data.winner = 0;
    
    // Initialize player 1 (cyan - left side)
    game_data.player1.x = 100;
    game_data.player1.y = 300;
    game_data.player1.direction = DIR_RIGHT;
    game_data.player1.next_direction = DIR_RIGHT;
    game_data.player1.color = 0x00FFFF; // Cyan
    game_data.player1.alive = true;
    
    // Initialize player 2 (orange - right side)
    game_data.player2.x = 700;
    game_data.player2.y = 300;
    game_data.player2.direction = DIR_LEFT;
    game_data.player2.next_direction = DIR_LEFT;
    game_data.player2.color = 0xFF8800; // Orange
    game_data.player2.alive = true;
    
    // Clear trails
    game_data.trail1_length = 0;
    game_data.trail2_length = 0;
    
    // Clear screen
    engine_clear_screen(game_engine, 0x000000);
}

void handle_input() {
    // Player 1 controls (WASD)
    if (engine_key_pressed(game_engine, W_SCANCODE) && game_data.player1.direction != DIR_DOWN) {
        game_data.player1.next_direction = DIR_UP;
    }
    else if (engine_key_pressed(game_engine, S_SCANCODE) && game_data.player1.direction != DIR_UP) {
        game_data.player1.next_direction = DIR_DOWN;
    }
    else if (engine_key_pressed(game_engine, A_SCANCODE) && game_data.player1.direction != DIR_RIGHT) {
        game_data.player1.next_direction = DIR_LEFT;
    }
    else if (engine_key_pressed(game_engine, D_SCANCODE) && game_data.player1.direction != DIR_LEFT) {
        game_data.player1.next_direction = DIR_RIGHT;
    }
    
    // Player 2 controls (Arrow keys)
    if (engine_key_pressed(game_engine, ARROW_UP_SCANCODE) && game_data.player2.direction != DIR_DOWN) {
        game_data.player2.next_direction = DIR_UP;
    }
    else if (engine_key_pressed(game_engine, ARROW_DOWN_SCANCODE) && game_data.player2.direction != DIR_UP) {
        game_data.player2.next_direction = DIR_DOWN;
    }
    else if (engine_key_pressed(game_engine, ARROW_LEFT_SCANCODE) && game_data.player2.direction != DIR_RIGHT) {
        game_data.player2.next_direction = DIR_LEFT;
    }
    else if (engine_key_pressed(game_engine, ARROW_RIGHT_SCANCODE) && game_data.player2.direction != DIR_LEFT) {
        game_data.player2.next_direction = DIR_RIGHT;
    }
    
    // Game state controls
    if (game_data.state == GAME_MENU || game_data.state == GAME_OVER) {
        if (engine_key_pressed(game_engine, SPACE_SCANCODE) || engine_key_pressed(game_engine, ENTER_SCANCODE)) {
            reset_game();
        }
    }
}

void update_players() {
    if (game_data.state != GAME_PLAYING) return;
    
    // Update every few frames to control speed
    if (game_data.frame_counter % PLAYER_SPEED != 0) return;
    
    // Update player directions
    game_data.player1.direction = game_data.player1.next_direction;
    game_data.player2.direction = game_data.player2.next_direction;
    
    // Add current positions to trails before moving
    if (game_data.player1.alive && game_data.trail1_length < 10000) {
        game_data.trail1[game_data.trail1_length].x = game_data.player1.x;
        game_data.trail1[game_data.trail1_length].y = game_data.player1.y;
        game_data.trail1_length++;
    }
    
    if (game_data.player2.alive && game_data.trail2_length < 10000) {
        game_data.trail2[game_data.trail2_length].x = game_data.player2.x;
        game_data.trail2[game_data.trail2_length].y = game_data.player2.y;
        game_data.trail2_length++;
    }
    
    // Move players
    if (game_data.player1.alive) {
        switch (game_data.player1.direction) {
            case DIR_UP:    game_data.player1.y -= GRID_SIZE; break;
            case DIR_DOWN:  game_data.player1.y += GRID_SIZE; break;
            case DIR_LEFT:  game_data.player1.x -= GRID_SIZE; break;
            case DIR_RIGHT: game_data.player1.x += GRID_SIZE; break;
        }
    }
    
    if (game_data.player2.alive) {
        switch (game_data.player2.direction) {
            case DIR_UP:    game_data.player2.y -= GRID_SIZE; break;
            case DIR_DOWN:  game_data.player2.y += GRID_SIZE; break;
            case DIR_LEFT:  game_data.player2.x -= GRID_SIZE; break;
            case DIR_RIGHT: game_data.player2.x += GRID_SIZE; break;
        }
    }
    
    // Update sprite positions
    if (game_data.player1.alive) {
        engine_move_sprite(game_engine, game_data.player1.sprite_id, game_data.player1.x, game_data.player1.y);
    }
    if (game_data.player2.alive) {
        engine_move_sprite(game_engine, game_data.player2.sprite_id, game_data.player2.x, game_data.player2.y);
    }
}

bool is_collision(uint16_t x, uint16_t y) {
    // Check wall collision
    if (x < GRID_SIZE || x >= GAME_WIDTH - GRID_SIZE || y < GRID_SIZE || y >= GAME_HEIGHT - GRID_SIZE) {
        return true;
    }
    
    // Check trail collision
    for (int i = 0; i < game_data.trail1_length; i++) {
        if (abs((int)x - (int)game_data.trail1[i].x) < GRID_SIZE && 
            abs((int)y - (int)game_data.trail1[i].y) < GRID_SIZE) {
            return true;
        }
    }
    
    for (int i = 0; i < game_data.trail2_length; i++) {
        if (abs((int)x - (int)game_data.trail2[i].x) < GRID_SIZE && 
            abs((int)y - (int)game_data.trail2[i].y) < GRID_SIZE) {
            return true;
        }
    }
    
    return false;
}

void check_collisions() {
    if (game_data.state != GAME_PLAYING) return;
    
    bool p1_collision = false;
    bool p2_collision = false;
    
    // Check player 1 collision
    if (game_data.player1.alive && is_collision(game_data.player1.x, game_data.player1.y)) {
        game_data.player1.alive = false;
        p1_collision = true;
    }
    
    // Check player 2 collision
    if (game_data.player2.alive && is_collision(game_data.player2.x, game_data.player2.y)) {
        game_data.player2.alive = false;
        p2_collision = true;
    }
    
    // Check head-on collision
    if (game_data.player1.alive && game_data.player2.alive) {
        if (abs((int)game_data.player1.x - (int)game_data.player2.x) < GRID_SIZE && 
            abs((int)game_data.player1.y - (int)game_data.player2.y) < GRID_SIZE) {
            game_data.player1.alive = false;
            game_data.player2.alive = false;
            p1_collision = true;
            p2_collision = true;
        }
    }
    
    // Determine winner
    if (p1_collision && p2_collision) {
        game_data.winner = 0; // Draw
        game_data.state = GAME_OVER;
    } else if (p1_collision) {
        game_data.winner = 2; // Player 2 wins
        game_data.state = GAME_OVER;
    } else if (p2_collision) {
        game_data.winner = 1; // Player 1 wins
        game_data.state = GAME_OVER;
    }
    
    // Hide sprites if players are dead
    if (!game_data.player1.alive) {
        engine_hide_sprite(game_engine, game_data.player1.sprite_id);
    }
    if (!game_data.player2.alive) {
        engine_hide_sprite(game_engine, game_data.player2.sprite_id);
    }
}

void draw_trails() {
    // Draw player 1 trail
    for (int i = 0; i < game_data.trail1_length; i++) {
        engine_draw_rectangle(game_engine, 
                            game_data.trail1[i].x, 
                            game_data.trail1[i].y, 
                            TRAIL_SIZE, TRAIL_SIZE, 
                            game_data.player1.color);
    }
    
    // Draw player 2 trail
    for (int i = 0; i < game_data.trail2_length; i++) {
        engine_draw_rectangle(game_engine, 
                            game_data.trail2[i].x, 
                            game_data.trail2[i].y, 
                            TRAIL_SIZE, TRAIL_SIZE, 
                            game_data.player2.color);
    }
}

void draw_ui() {
    if (game_data.state == GAME_MENU) {
        // Draw title and instructions
        engine_draw_rectangle(game_engine, 300, 200, 200, 40, 0xFFFFFF);
        engine_draw_rectangle(game_engine, 250, 280, 300, 20, 0xFFFFFF);
        engine_draw_rectangle(game_engine, 200, 320, 400, 20, 0xFFFFFF);
        engine_draw_rectangle(game_engine, 280, 360, 240, 20, 0xFFFFFF);
    } else if (game_data.state == GAME_OVER) {
        // Draw game over screen
        engine_draw_rectangle(game_engine, 300, 200, 200, 40, 0xFFFFFF);
        
        if (game_data.winner == 0) {
            engine_draw_rectangle(game_engine, 350, 260, 100, 20, 0xFFFFFF); // "DRAW!"
        } else if (game_data.winner == 1) {
            engine_draw_rectangle(game_engine, 300, 260, 200, 20, game_data.player1.color); // "PLAYER 1 WINS!"
        } else {
            engine_draw_rectangle(game_engine, 300, 260, 200, 20, game_data.player2.color); // "PLAYER 2 WINS!"
        }
        
        engine_draw_rectangle(game_engine, 280, 320, 240, 20, 0xFFFFFF); // "PRESS SPACE TO RESTART"
    }
}

void tron_update(game_engine_t *engine, float delta_time) {
    game_data.frame_counter++;
    
    handle_input();
    update_players();
    check_collisions();
}

void tron_render(game_engine_t *engine) {
    if (game_data.state == GAME_PLAYING) {
        draw_trails();
    }
    draw_ui();
}

int game_init() {
    // Initialize the game engine
    game_engine_t engine;
    game_engine = &engine;
    
    // Initialize with 0x115 mode (800x600) at 60 FPS
    if (engine_init(&engine, 0x115, 60) != 0) {
        printf("Failed to initialize game engine\n");
        return 1;
    }
    
    // Initialize game state
    game_data.state = GAME_MENU;
    
    // Clear the screen to black
    engine_clear_screen(&engine, 0x000000);
    
    // Create player sprites
    game_data.player1.sprite_id = engine_create_sprite(&engine, (xpm_map_t)player1_xpm, 100, 300);
    game_data.player2.sprite_id = engine_create_sprite(&engine, (xpm_map_t)player2_xpm, 700, 300);
    
    if (game_data.player1.sprite_id < 0 || game_data.player2.sprite_id < 0) {
        printf("Failed to create player sprites\n");
        engine_cleanup(&engine);
        return 1;
    }
    
    // Show the sprites
    engine_show_sprite(&engine, game_data.player1.sprite_id);
    engine_show_sprite(&engine, game_data.player2.sprite_id);
    
    // Register update and render callbacks
    engine_set_update_callback(&engine, tron_update);
    engine_set_render_callback(&engine, tron_render);
    
    // Run the game loop
    int result = engine_run(&engine);
    
    // Clean up engine resources
    engine_cleanup(&engine);
    
    return result;
}
