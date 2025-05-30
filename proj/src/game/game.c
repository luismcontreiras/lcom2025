#include "game.h"
#include <lcom/lcf.h>
#include "../engine/game_engine.h"

// Include XPM assets for player 1 (blue)
#include "tron_assets/tron_blue_up.xpm"
#include "tron_assets/tron_blue_down.xpm"
#include "tron_assets/tron_blue_left.xpm"
#include "tron_assets/tron_blue_right.xpm"

// Include XPM assets for player 2 (orange)
#include "tron_assets/tron_orange_up.xpm"
#include "tron_assets/tron_orange_down.xpm"
#include "tron_assets/tron_orange_left.xpm"
#include "tron_assets/tron_orange_right.xpm"

// Include explosion assets
#include "tron_assets/tron_explosion_small.xpm"
#include "tron_assets/tron_explosion_large.xpm"

// Global game state
static tron_game_t game_data;
static game_engine_t *game_engine;

// Sprite IDs for different directions
static int player1_sprites[4]; // UP, DOWN, LEFT, RIGHT
static int player2_sprites[4]; 
static int explosion_sprite_small, explosion_sprite_large;

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
void update_player_sprite(int player_num);
void calculate_front_area(uint16_t x, uint16_t y, int direction, int *front_x, int *front_y, int *front_width, int *front_height);

void update_player_sprite(int player_num) {
    if (player_num == 1) {
        // Hide all player 1 sprites first
        for (int i = 0; i < 4; i++) {
            engine_hide_sprite(game_engine, player1_sprites[i]);
        }
        
        // Show the correct sprite based on direction
        if (game_data.player1.alive) {
            int sprite_index = game_data.player1.direction;
            engine_move_sprite(game_engine, player1_sprites[sprite_index], 
                             game_data.player1.x, game_data.player1.y);
            engine_show_sprite(game_engine, player1_sprites[sprite_index]);
        }
    } else if (player_num == 2) {
        // Hide all player 2 sprites first
        for (int i = 0; i < 4; i++) {
            engine_hide_sprite(game_engine, player2_sprites[i]);
        }
        
        // Show the correct sprite based on direction
        if (game_data.player2.alive) {
            int sprite_index = game_data.player2.direction;
            engine_move_sprite(game_engine, player2_sprites[sprite_index], 
                             game_data.player2.x, game_data.player2.y);
            engine_show_sprite(game_engine, player2_sprites[sprite_index]);
        }
    }
}

void reset_game() {
    game_data.state = GAME_PLAYING;
    game_data.frame_counter = 0;
    game_data.winner = 0;
    
    // Initialize player 1 (cyan - left side)
    game_data.player1.x = 100;
    game_data.player1.y = 300;
    game_data.player1.direction = DIR_RIGHT;
    game_data.player1.next_direction = DIR_RIGHT;
    game_data.player1.color = 0x0080FFFF; // Cyan
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
    
    // Hide explosion sprites
    engine_hide_sprite(game_engine, explosion_sprite_small);
    engine_hide_sprite(game_engine, explosion_sprite_large);
    
    // Update player sprites to show correct direction
    update_player_sprite(1);
    update_player_sprite(2);
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
    
    // Check if direction changed to update sprites
    bool p1_direction_changed = (game_data.player1.direction != game_data.player1.next_direction);
    bool p2_direction_changed = (game_data.player2.direction != game_data.player2.next_direction);
    
    // Update player directions
    game_data.player1.direction = game_data.player1.next_direction;
    game_data.player2.direction = game_data.player2.next_direction;
    
    // Update sprites if direction changed
    if (p1_direction_changed) {
        update_player_sprite(1);
    }
    if (p2_direction_changed) {
        update_player_sprite(2);
    }
    
    // Calculate trail emission points based on direction and cycle sizes
    uint16_t trail_x1 = game_data.player1.x;
    uint16_t trail_y1 = game_data.player1.y;
    uint16_t trail_x2 = game_data.player2.x;
    uint16_t trail_y2 = game_data.player2.y;
    
    // Get sprite dimensions based on the XPM files
    int p1_width = 16;  // Sprite width for player 1
    int p1_height = 16; // Sprite height for player 1
    int p2_width = 16;  // Sprite width for player 2
    int p2_height = 16; // Sprite height for player 2
    
    // Add current positions to trails with trails always coming from exact center point
    if (game_data.player1.alive && game_data.trail1_length < 10000) {
        // Calculate precise center point
        uint16_t center_x = trail_x1 + p1_width / 2;  
        uint16_t center_y = trail_y1 + p1_height / 2;
        
        game_data.trail1[game_data.trail1_length].x = center_x;
        game_data.trail1[game_data.trail1_length].y = center_y;
        game_data.trail1_length++;
    }
    
    if (game_data.player2.alive && game_data.trail2_length < 10000) {
        // Calculate precise center point
        uint16_t center_x = trail_x2 + p2_width / 2;  
        uint16_t center_y = trail_y2 + p2_height / 2;
        
        game_data.trail2[game_data.trail2_length].x = center_x;
        game_data.trail2[game_data.trail2_length].y = center_y;
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
        // Update sprite position
        int sprite_index = game_data.player1.direction;
        engine_move_sprite(game_engine, player1_sprites[sprite_index], 
                         game_data.player1.x, game_data.player1.y);
    }
    
    if (game_data.player2.alive) {
        switch (game_data.player2.direction) {
            case DIR_UP:    game_data.player2.y -= GRID_SIZE; break;
            case DIR_DOWN:  game_data.player2.y += GRID_SIZE; break;
            case DIR_LEFT:  game_data.player2.x -= GRID_SIZE; break;
            case DIR_RIGHT: game_data.player2.x += GRID_SIZE; break;
        }
        // Update sprite position
        int sprite_index = game_data.player2.direction;
        engine_move_sprite(game_engine, player2_sprites[sprite_index], 
                         game_data.player2.x, game_data.player2.y);
    }
}

void get_sprite_dimensions(int direction, int *width, int *height) {
    switch(direction) {
        case DIR_UP:
        case DIR_DOWN:
            *width = 14;
            *height = 32;
            break;
        case DIR_LEFT:
        case DIR_RIGHT:
            *width = 32;
            *height = 14;
            break;
        default:
            *width = 16;
            *height = 16;
            break;
    }
}

void calculate_front_area(uint16_t x, uint16_t y, int direction, int *front_x, int *front_y, int *front_width, int *front_height) {
    // Get sprite dimensions using the get_sprite_dimensions function
    int sprite_width, sprite_height;
    get_sprite_dimensions(direction, &sprite_width, &sprite_height);
    
    // Initialize front area to full sprite
    *front_x = x;
    *front_y = y;
    *front_width = sprite_width;
    *front_height = sprite_height;
    
    // Determine the front area based on direction
    switch(direction) {
        case DIR_UP:            
            *front_height = sprite_height / 3;  // Upper third is the front
            break;
        case DIR_DOWN:
            *front_y = y + 2 * sprite_height / 3;  // Lower third
            *front_height = sprite_height / 3;
            break;
        case DIR_LEFT:
            *front_width = sprite_width / 3;  // Left third
            break;
        case DIR_RIGHT:
            *front_x = x + 2 * sprite_width / 3;  // Right third
            *front_width = sprite_width / 3;
            break;
    }
}

bool is_collision(uint16_t x, uint16_t y) {
    // Determine which player we're checking and get their direction
    int direction = -1;
    if (x == game_data.player1.x && y == game_data.player1.y) {
        direction = game_data.player1.direction;
    } else if (x == game_data.player2.x && y == game_data.player2.y) {
        direction = game_data.player2.direction;
    }
    
    // Calculate the front area of the sprite
    int front_x, front_y, front_width, front_height;
    if (direction != -1) {
        calculate_front_area(x, y, direction, &front_x, &front_y, &front_width, &front_height);
    } else {
        // Default dimensions for unknown cases
        front_x = x;
        front_y = y;
        front_width = 16;
        front_height = 16;
    }
    
    // Check wall collision (using the front area)
    if (front_x < GRID_SIZE || front_x + front_width >= GAME_WIDTH - GRID_SIZE ||
        front_y < GRID_SIZE || front_y + front_height >= GAME_HEIGHT - GRID_SIZE) {
        return true;
    }
    
    // Calculate the center point of the front area
    uint16_t front_center_x = front_x + front_width / 2;
    uint16_t front_center_y = front_y + front_height / 2;
    
    // Check trail collision using the front center point
    for (int i = 0; i < game_data.trail1_length; i++) {
        if (abs((int)front_center_x - (int)game_data.trail1[i].x) < GRID_SIZE && 
            abs((int)front_center_y - (int)game_data.trail1[i].y) < GRID_SIZE) {
            return true;
        }
    }
    
    for (int i = 0; i < game_data.trail2_length; i++) {
        if (abs((int)front_center_x - (int)game_data.trail2[i].x) < GRID_SIZE && 
            abs((int)front_center_y - (int)game_data.trail2[i].y) < GRID_SIZE) {
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
        
        // Show explosion at player 1 position
        engine_move_sprite(game_engine, explosion_sprite_small, 
                         game_data.player1.x, game_data.player1.y);
        engine_show_sprite(game_engine, explosion_sprite_small);
    }
    
    // Check player 2 collision
    if (game_data.player2.alive && is_collision(game_data.player2.x, game_data.player2.y)) {
        game_data.player2.alive = false;
        p2_collision = true;
        
        // Show explosion at player 2 position
        if (!p1_collision) {
            // Use small explosion if only player 2 collided
            engine_move_sprite(game_engine, explosion_sprite_small, 
                             game_data.player2.x, game_data.player2.y);
            engine_show_sprite(game_engine, explosion_sprite_small);
        }
    }
    
    // Check head-on collision using the front areas of both light cycles
    if (game_data.player1.alive && game_data.player2.alive) {
        // Calculate front areas for both players using the helper function
        int p1_front_x, p1_front_y, p1_front_width, p1_front_height;
        int p2_front_x, p2_front_y, p2_front_width, p2_front_height;
        
        calculate_front_area(game_data.player1.x, game_data.player1.y, game_data.player1.direction,
                           &p1_front_x, &p1_front_y, &p1_front_width, &p1_front_height);
        
        calculate_front_area(game_data.player2.x, game_data.player2.y, game_data.player2.direction,
                           &p2_front_x, &p2_front_y, &p2_front_width, &p2_front_height);
        
        // Check for front areas collision using rectangle intersection
        if (p1_front_x < p2_front_x + p2_front_width &&
            p1_front_x + p1_front_width > p2_front_x &&
            p1_front_y < p2_front_y + p2_front_height &&
            p1_front_y + p1_front_height > p2_front_y) {
            game_data.player1.alive = false;
            game_data.player2.alive = false;
            p1_collision = true;
            p2_collision = true;
            
            // Show large explosion for head-on collision
            engine_hide_sprite(game_engine, explosion_sprite_small);
            int explosion_x = (game_data.player1.x + game_data.player2.x) / 2;
            int explosion_y = (game_data.player1.y + game_data.player2.y) / 2;
            engine_move_sprite(game_engine, explosion_sprite_large, explosion_x, explosion_y);
            engine_show_sprite(game_engine, explosion_sprite_large);
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
        for (int i = 0; i < 4; i++) {
            engine_hide_sprite(game_engine, player1_sprites[i]);
        }
    }
    if (!game_data.player2.alive) {
        for (int i = 0; i < 4; i++) {
            engine_hide_sprite(game_engine, player2_sprites[i]);
        }
    }
}

void draw_trails() {
    if (game_data.trail1_length < 2 && game_data.trail2_length < 2) return;
    
    // Draw player 1 trail as continuous segments
    for (int i = 1; i < game_data.trail1_length; i++) {
        uint16_t x1 = game_data.trail1[i-1].x;
        uint16_t y1 = game_data.trail1[i-1].y;
        uint16_t x2 = game_data.trail1[i].x;
        uint16_t y2 = game_data.trail1[i].y;
        
        // Determine the direction of the trail segment
        if (x1 == x2) {
            // Vertical segment
            uint16_t start_y = (y1 < y2) ? y1 : y2;
            uint16_t height = (y1 < y2) ? (y2 - y1 + TRAIL_SIZE) : (y1 - y2 + TRAIL_SIZE);
            engine_draw_rectangle(game_engine, 
                                x1 - (TRAIL_SIZE), // Double width centered
                                start_y, 
                                TRAIL_SIZE * 2, height, 
                                game_data.player1.color);
        } else {
            // Horizontal segment
            uint16_t start_x = (x1 < x2) ? x1 : x2;
            uint16_t width = (x1 < x2) ? (x2 - x1 + TRAIL_SIZE) : (x1 - x2 + TRAIL_SIZE);
            engine_draw_rectangle(game_engine, 
                                start_x, 
                                y1 - (TRAIL_SIZE), // Double width centered
                                width, TRAIL_SIZE * 2, 
                                game_data.player1.color);
        }
    }
    
    // Draw player 2 trail as continuous segments
    for (int i = 1; i < game_data.trail2_length; i++) {
        uint16_t x1 = game_data.trail2[i-1].x;
        uint16_t y1 = game_data.trail2[i-1].y;
        uint16_t x2 = game_data.trail2[i].x;
        uint16_t y2 = game_data.trail2[i].y;
        
        // Determine the direction of the trail segment
        if (x1 == x2) {
            // Vertical segment
            uint16_t start_y = (y1 < y2) ? y1 : y2;
            uint16_t height = (y1 < y2) ? (y2 - y1 + TRAIL_SIZE) : (y1 - y2 + TRAIL_SIZE);
            engine_draw_rectangle(game_engine, 
                                x1 - (TRAIL_SIZE), // Double width centered
                                start_y, 
                                TRAIL_SIZE * 2, height, 
                                game_data.player2.color);
        } else {
            // Horizontal segment
            uint16_t start_x = (x1 < x2) ? x1 : x2;
            uint16_t width = (x1 < x2) ? (x2 - x1 + TRAIL_SIZE) : (x1 - x2 + TRAIL_SIZE);
            engine_draw_rectangle(game_engine, 
                                start_x, 
                                y1 - (TRAIL_SIZE), // Double width centered
                                width, TRAIL_SIZE * 2, 
                                game_data.player2.color);
        }
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
    
    // Create player 1 sprites (blue) for all directions
    player1_sprites[DIR_UP] = engine_create_sprite(&engine, (xpm_map_t)tron_blue_up, 100, 300);
    player1_sprites[DIR_DOWN] = engine_create_sprite(&engine, (xpm_map_t)tron_blue_down, 100, 300);
    player1_sprites[DIR_LEFT] = engine_create_sprite(&engine, (xpm_map_t)tron_blue_left, 100, 300);
    player1_sprites[DIR_RIGHT] = engine_create_sprite(&engine, (xpm_map_t)tron_blue_right, 100, 300);
    
    // Create player 2 sprites (orange) for all directions
    player2_sprites[DIR_UP] = engine_create_sprite(&engine, (xpm_map_t)tron_orange_up, 700, 300);
    player2_sprites[DIR_DOWN] = engine_create_sprite(&engine, (xpm_map_t)tron_orange_down, 700, 300);
    player2_sprites[DIR_LEFT] = engine_create_sprite(&engine, (xpm_map_t)tron_orange_left, 700, 300);
    player2_sprites[DIR_RIGHT] = engine_create_sprite(&engine, (xpm_map_t)tron_orange_right, 700, 300);
    
    // Create explosion sprites
    explosion_sprite_small = engine_create_sprite(&engine, (xpm_map_t)tron_explosion_small, 0, 0);
    explosion_sprite_large = engine_create_sprite(&engine, (xpm_map_t)tron_explosion_large, 0, 0);
    
    // Check if all sprites were created successfully
    for (int i = 0; i < 4; i++) {
        if (player1_sprites[i] < 0 || player2_sprites[i] < 0) {
            printf("Failed to create player sprites\n");
            engine_cleanup(&engine);
            return 1;
        }
    }
    
    // Hide all sprites initially (they will be shown in reset_game)
    for (int i = 0; i < 4; i++) {
        engine_hide_sprite(&engine, player1_sprites[i]);
        engine_hide_sprite(&engine, player2_sprites[i]);
    }
    engine_hide_sprite(&engine, explosion_sprite_small);
    engine_hide_sprite(&engine, explosion_sprite_large);
    
    // Register update and render callbacks
    engine_set_update_callback(&engine, tron_update);
    engine_set_render_callback(&engine, tron_render);
    
    // Run the game loop
    int result = engine_run(&engine);
    
    // Clean up engine resources
    engine_cleanup(&engine);
    
    return result;
}
