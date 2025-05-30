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

// Include menu assets
#include "tron_assets/tron_menu_title.xpm"

// Include alphabet assets for START text
#include "../engine/assets/alphabet/S.xpm"
#include "../engine/assets/alphabet/T.xpm"
#include "../engine/assets/alphabet/A.xpm"
#include "../engine/assets/alphabet/R.xpm"
#include "../engine/assets/alphabet/V.xpm"
#include "../engine/assets/alphabet/H.xpm"
#include "../engine/assets/alphabet/C.xpm"
#include "../engine/assets/alphabet/E.xpm"
#include "../engine/assets/alphabet/X.xpm"
#include "../engine/assets/alphabet/I.xpm"

// Include number assets for 1vs1 text
#include "../engine/assets/numbers/1.xpm"

// Include cursor asset
#include "../engine/assets/cursor.xpm"

// Global game state
static tron_game_t game_data;
static game_engine_t *game_engine;

// Sprite IDs for different directions
static int player1_sprites[4]; // UP, DOWN, LEFT, RIGHT
static int player2_sprites[4]; 
static int explosion_sprite_small, explosion_sprite_large, menu_title;
static int start_sprites[22]; // S, T, A, R, T, space, 1, v, s, 1 (first row) + S, T, A, R, T, space, H, v, s, C (second row) + E, X, I, T (third row)
static int cursor_sprite; // Mouse cursor sprite

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

void update_cursor();
bool is_mouse_in_start1vs1_area();
void handle_mouse_click();
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
    
    // Hide menu elements when starting game
    engine_hide_sprite(game_engine, menu_title);
    for (int i = 0; i < 18; i++) {
        engine_hide_sprite(game_engine, start_sprites[i]);
    }
    
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
    
    // Handle mouse clicks
    handle_mouse_click();
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
    // Using 9/20 of the sprite (close to half but not exactly)
    switch(direction) {
        case DIR_UP:            
            *front_height = sprite_height * 9 / 20;  // Front 9/20
            break;
        case DIR_DOWN:
            *front_y = y + sprite_height * 11 / 20;  // Back 11/20
            *front_height = sprite_height * 9 / 20;
            break;
        case DIR_LEFT:
            *front_width = sprite_width * 9 / 20;  // Left 9/20
            break;
        case DIR_RIGHT:
            *front_x = x + sprite_width * 11 / 20;  // Right 9/20
            *front_width = sprite_width * 9 / 20;
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
        engine_show_sprite(game_engine, menu_title);
        
        // Show START text sprites
        for (int i = 0; i < 22; i++) {
            engine_show_sprite(game_engine, start_sprites[i]);
        }
        
        // Show cursor in menu
        engine_show_sprite(game_engine, cursor_sprite);
    } else if (game_data.state == GAME_OVER) {
        // Hide START text sprites when not in menu
        for (int i = 0; i < 22; i++) {
            engine_hide_sprite(game_engine, start_sprites[i]);
        }
        // Hide cursor in game over state
        engine_hide_sprite(game_engine, cursor_sprite);
        
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
    } else if (game_data.state == GAME_PLAYING) {
        // Hide menu elements when playing
        engine_hide_sprite(game_engine, menu_title);
        for (int i = 0; i < 22; i++) {
            engine_hide_sprite(game_engine, start_sprites[i]);
        }
        // Hide cursor when playing
        engine_hide_sprite(game_engine, cursor_sprite);
    }
}

void update_cursor() {
    // Update cursor position based on mouse position
    // Offset by half the cursor size so the center of the cursor is at the mouse position
    int cursor_width = 32;  // Cursor sprite is 32x32 pixels
    int cursor_height = 32;
    
    int cursor_x = game_engine->input.mouse_x - cursor_width / 2;
    int cursor_y = game_engine->input.mouse_y - cursor_height / 2;
    
    engine_move_sprite(game_engine, cursor_sprite, cursor_x, cursor_y);
}

bool is_mouse_in_start1vs1_area() {
    // Check if mouse is within the "START 1vs1" text area
    // START 1vs1 spans from x=320 to x=490 and y=280 to y=295 (approximately)
    int mouse_x = game_engine->input.mouse_x;
    int mouse_y = game_engine->input.mouse_y;
    
    return (mouse_x >= 320 && mouse_x <= 510 && 
            mouse_y >= 280 && mouse_y <= 295);
}

void handle_mouse_click() {
    if (game_data.state == GAME_MENU && engine_mouse_clicked(game_engine, 0)) { // Left mouse button
        if (is_mouse_in_start1vs1_area()) {
            reset_game(); // Start the game
        }
    }
}

void tron_update(game_engine_t *engine, float delta_time) {
    game_data.frame_counter++;
    
    handle_input();
    update_players();
    check_collisions();
    update_cursor();
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
    
    // Create menu sprite 
    menu_title = engine_create_sprite(&engine, (xpm_map_t)tron_menu_title, 270, 200);

    // Change to
    // First row: START 1vs1
    start_sprites[0] = engine_create_sprite(&engine, (xpm_map_t)S_xpm, 320, 280); // S
    start_sprites[1] = engine_create_sprite(&engine, (xpm_map_t)T_xpm, 340, 280); // T
    start_sprites[2] = engine_create_sprite(&engine, (xpm_map_t)A_xpm, 360, 280); // A
    start_sprites[3] = engine_create_sprite(&engine, (xpm_map_t)R_xpm, 380, 280); // R
    start_sprites[4] = engine_create_sprite(&engine, (xpm_map_t)T_xpm, 400, 280); // T
    // 1vs1 sprites (with space before)
    start_sprites[5] = engine_create_sprite(&engine, (xpm_map_t)num_1_xpm, 430, 280); // 1
    start_sprites[6] = engine_create_sprite(&engine, (xpm_map_t)V_xpm, 450, 280); // v
    start_sprites[7] = engine_create_sprite(&engine, (xpm_map_t)S_xpm, 470, 280); // s
    start_sprites[8] = engine_create_sprite(&engine, (xpm_map_t)num_1_xpm, 490, 280); // 1
    
    // Second row: START HvsC
    start_sprites[9] = engine_create_sprite(&engine, (xpm_map_t)S_xpm, 320, 310); // S
    start_sprites[10] = engine_create_sprite(&engine, (xpm_map_t)T_xpm, 340, 310); // T
    start_sprites[11] = engine_create_sprite(&engine, (xpm_map_t)A_xpm, 360, 310); // A
    start_sprites[12] = engine_create_sprite(&engine, (xpm_map_t)R_xpm, 380, 310); // R
    start_sprites[13] = engine_create_sprite(&engine, (xpm_map_t)T_xpm, 400, 310); // T
    // HvsC sprites (with space before)
    start_sprites[14] = engine_create_sprite(&engine, (xpm_map_t)H_xpm, 430, 310); // H
    start_sprites[15] = engine_create_sprite(&engine, (xpm_map_t)V_xpm, 450, 310); // v
    start_sprites[16] = engine_create_sprite(&engine, (xpm_map_t)S_xpm, 470, 310); // s
    start_sprites[17] = engine_create_sprite(&engine, (xpm_map_t)C_xpm, 490, 310); // C
    
    // Third row: EXIT (centered below HvsC)
    start_sprites[18] = engine_create_sprite(&engine, (xpm_map_t)E_xpm, 380, 340); // E
    start_sprites[19] = engine_create_sprite(&engine, (xpm_map_t)X_xpm, 400, 340); // X
    start_sprites[20] = engine_create_sprite(&engine, (xpm_map_t)I_xpm, 420, 340); // I
    start_sprites[21] = engine_create_sprite(&engine, (xpm_map_t)T_xpm, 440, 340); // T

    // Create cursor sprite
    cursor_sprite = engine_create_sprite(&engine, (xpm_map_t)cursor, 0, 0);

    // Hide all sprites initially (they will be shown when appropriate)
    for (int i = 0; i < 4; i++) {
        engine_hide_sprite(&engine, player1_sprites[i]);
        engine_hide_sprite(&engine, player2_sprites[i]);
    }
    for (int i = 0; i < 22; i++) {
        engine_hide_sprite(&engine, start_sprites[i]);
    }
    engine_hide_sprite(&engine, explosion_sprite_small);
    engine_hide_sprite(&engine, explosion_sprite_large);
    engine_hide_sprite(&engine, menu_title);
    engine_hide_sprite(&engine, cursor_sprite);
    
    // Register update and render callbacks
    engine_set_update_callback(&engine, tron_update);
    engine_set_render_callback(&engine, tron_render);
    
    // Run the game loop
    int result = engine_run(&engine);
    
    // Clean up engine resources
    engine_cleanup(&engine);
    
    return result;
}
