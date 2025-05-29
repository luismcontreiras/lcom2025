#include "game.h"
#include <lcom/lcf.h>
#include "../engine/game_engine.h"
#include <math.h>

// Include tron sprite XPMs
#include "../assets/sprites/tron_blue_up.xpm"
#include "../assets/sprites/tron_blue_down.xpm"
#include "../assets/sprites/tron_blue_left.xpm"
#include "../assets/sprites/tron_blue_right.xpm"
#include "../assets/sprites/tron_orange_up.xpm"
#include "../assets/sprites/tron_orange_down.xpm"
#include "../assets/sprites/tron_orange_left.xpm"
#include "../assets/sprites/tron_orange_right.xpm"

// Direction enum for player movement
typedef enum {
    DIR_UP = 0,
    DIR_DOWN = 1,
    DIR_LEFT = 2,
    DIR_RIGHT = 3
} direction_t;

// Player state
static struct {
    uint16_t x;
    uint16_t y;
    uint16_t prev_x;  // Previous position for trail drawing
    uint16_t prev_y;
    uint16_t last_trail_x;  // Last trail point for continuity
    uint16_t last_trail_y;
    uint8_t sprite_ids[4];  // Array for 4 directional sprites
    uint8_t current_sprite; // Currently active sprite
    direction_t direction;  // Current direction
    direction_t prev_direction; // Previous direction for turn detection
    uint16_t speed;
    bool first_move;  // Flag to track first movement
} player;

// Second player state (orange player)
static struct {
    uint16_t x;
    uint16_t y;
    uint16_t prev_x;  // Previous position for trail drawing
    uint16_t prev_y;
    uint16_t last_trail_x;  // Last trail point for continuity
    uint16_t last_trail_y;
    uint8_t sprite_ids[4];  // Array for 4 directional sprites
    uint8_t current_sprite; // Currently active sprite
    direction_t direction;  // Current direction
    direction_t prev_direction; // Previous direction for turn detection
    uint16_t speed;
    bool first_move;  // Flag to track first movement
} player2;

// Trail system
static trail_segment_t trail_segments[MAX_TRAIL_SEGMENTS];
static uint16_t trail_count = 0;

// Efficient line drawing using Bresenham's algorithm with thickness
void draw_thick_line(game_engine_t *engine, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t thickness, uint32_t color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    int x = x1, y = y1;
    
    int half_thickness = thickness / 2;
    
    while (true) {
        // Draw a small rectangle at each point to create thickness
        for (int ty = -half_thickness; ty <= half_thickness; ty++) {
            for (int tx = -half_thickness; tx <= half_thickness; tx++) {
                engine_draw_pixel(engine, x + tx, y + ty, color);
            }
        }
        
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

// Get the rear position of a motorcycle based on its direction
// This positions the trail at the '>' exhaust characters in the sprites
void get_rear_position(uint16_t sprite_x, uint16_t sprite_y, direction_t direction, uint16_t *rear_x, uint16_t *rear_y) {
    // Based on sprite analysis:
    // UP/DOWN sprites: 14x32 pixels 
    // LEFT/RIGHT sprites: 32x14 pixels
    // '>' characters indicate exhaust position
    switch (direction) {
        case DIR_UP:
            // Exhaust comes from bottom of sprite (lines 28-31 in XPM)
            *rear_x = sprite_x + 7;   // Center horizontally (14/2 = 7)
            *rear_y = sprite_y + 30;  // Bottom where '>' characters are
            break;
        case DIR_DOWN:
            // Exhaust comes from top of sprite (lines 0-3 in XPM)
            *rear_x = sprite_x + 7;   // Center horizontally (14/2 = 7)
            *rear_y = sprite_y + 2;   // Top where '>' characters are
            break;
        case DIR_LEFT:
            // Exhaust comes from right side of sprite (rightmost '>' chars)
            *rear_x = sprite_x + 30;  // Right edge where '>' characters are
            *rear_y = sprite_y + 7;   // Center vertically (14/2 = 7)
            break;
        case DIR_RIGHT:
            // Exhaust comes from left side of sprite (leftmost '>' chars)
            *rear_x = sprite_x + 2;   // Left edge where '>' characters are
            *rear_y = sprite_y + 7;   // Center vertically (14/2 = 7)
            break;
        default:
            *rear_x = sprite_x + 7;   // Default to center
            *rear_y = sprite_y + 7;
            break;
    }
}

// Add a trail segment (line from previous to current position)
void add_trail_segment(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color) {
    if (trail_count < MAX_TRAIL_SEGMENTS) {
        trail_segments[trail_count].x1 = x1;
        trail_segments[trail_count].y1 = y1;
        trail_segments[trail_count].x2 = x2;
        trail_segments[trail_count].y2 = y2;
        trail_segments[trail_count].thickness = TRAIL_THICKNESS;
        trail_segments[trail_count].color = color;
        trail_segments[trail_count].active = true;
        trail_count++;
    }
}

// Helper function to change player sprite based on direction
void change_player_sprite(game_engine_t *engine, direction_t new_direction) {
    if (player.direction != new_direction) {
        // Adjust sprite position slightly for left and up turns for better visual alignment FIRST
        if (new_direction == DIR_LEFT) {
            player.x -= 3; // Move sprite slightly left
        }
        else if (new_direction == DIR_UP) {
            player.y -= 3; // Move sprite slightly up
        }
        
        // Calculate trail connection at the turn point AFTER sprite position adjustment
        if (!player.first_move) {
            uint16_t turn_rear_x, turn_rear_y;
            // Get rear position for new direction at current position (after sprite adjustment)
            get_rear_position(player.x, player.y, new_direction, &turn_rear_x, &turn_rear_y);
            
            // Connect trail from last position to turn point
            add_trail_segment(player.last_trail_x, player.last_trail_y, turn_rear_x, turn_rear_y, BLUE_TRAIL_COLOR);
            
            // Update last trail position to the turn point
            player.last_trail_x = turn_rear_x;
            player.last_trail_y = turn_rear_y;
        }
        
        // Hide current sprite
        engine_hide_sprite(engine, player.current_sprite);
        
        // Update direction and current sprite
        player.direction = new_direction;
        player.current_sprite = player.sprite_ids[new_direction];
        
        // Show new sprite at current position
        engine_move_sprite(engine, player.current_sprite, player.x, player.y);
        engine_show_sprite(engine, player.current_sprite);
    }
}

// Helper function to change player2 sprite based on direction
void change_player2_sprite(game_engine_t *engine, direction_t new_direction) {
    if (player2.direction != new_direction) {
        // Calculate trail connection at the turn point
        if (!player2.first_move) {
            uint16_t turn_rear_x, turn_rear_y;
            // Get rear position for new direction at current position (before moving)
            get_rear_position(player2.x, player2.y, new_direction, &turn_rear_x, &turn_rear_y);
            
            // Add position adjustment for left and up turns to create perfect 90-degree angles
            if (new_direction == DIR_LEFT) {
                turn_rear_x += 3; // Move slightly right to create better angle
            }
            else if (new_direction == DIR_UP) {
                turn_rear_y += 3; // Move slightly down to create better angle
            }
            
            // Connect trail from last position to turn point
            add_trail_segment(player2.last_trail_x, player2.last_trail_y, turn_rear_x, turn_rear_y, ORANGE_TRAIL_COLOR);
            
            // Update last trail position to the turn point
            player2.last_trail_x = turn_rear_x;
            player2.last_trail_y = turn_rear_y;
        }
        
        // Hide current sprite
        engine_hide_sprite(engine, player2.current_sprite);
        
        // Update direction and current sprite
        player2.direction = new_direction;
        player2.current_sprite = player2.sprite_ids[new_direction];
        
        // Show new sprite at current position
        engine_move_sprite(engine, player2.current_sprite, player2.x, player2.y);
        engine_show_sprite(engine, player2.current_sprite);
    }
}

// Update function to handle player movement
void game_update(game_engine_t *engine, float delta_time) {
    bool moved = false;
    bool moved2 = false;
    
    
    // Move player 1 based on arrow key input and change sprite direction
    if (engine_key_pressed(engine, ARROW_UP_SCANCODE)) {
        if (player.y > player.speed) {
            player.y -= player.speed;
            moved = true;
        }
        change_player_sprite(engine, DIR_UP);
    }
    else if (engine_key_pressed(engine, ARROW_DOWN_SCANCODE)) {
        if (player.y < engine->screen_height - 32 - player.speed) {
            player.y += player.speed;
            moved = true;
        }
        change_player_sprite(engine, DIR_DOWN);
    }
    else if (engine_key_pressed(engine, ARROW_LEFT_SCANCODE)) {
        if (player.x > player.speed) {
            player.x -= player.speed;
            moved = true;
        }
        change_player_sprite(engine, DIR_LEFT);
    }
    else if (engine_key_pressed(engine, ARROW_RIGHT_SCANCODE)) {
        if (player.x < engine->screen_width - 32 - player.speed) {
            player.x += player.speed;
            moved = true;
        }
        change_player_sprite(engine, DIR_RIGHT);
    }

    // Move player 2 based on WASD key input and change sprite direction
    if (engine_key_pressed(engine, W_SCANCODE)) {
        if (player2.y > player2.speed) {
            player2.y -= player2.speed;
            moved2 = true;
        }
        change_player2_sprite(engine, DIR_UP);
    }
    else if (engine_key_pressed(engine, S_SCANCODE)) {
        if (player2.y < engine->screen_height - 32 - player2.speed) {
            player2.y += player2.speed;
            moved2 = true;
        }
        change_player2_sprite(engine, DIR_DOWN);
    }
    else if (engine_key_pressed(engine, A_SCANCODE)) {
        if (player2.x > player2.speed) {
            player2.x -= player2.speed;
            moved2 = true;
        }
        change_player2_sprite(engine, DIR_LEFT);
    }
    else if (engine_key_pressed(engine, D_SCANCODE)) {
        if (player2.x < engine->screen_width - 32 - player2.speed) {
            player2.x += player2.speed;
            moved2 = true;
        }
        change_player2_sprite(engine, DIR_RIGHT);
    }

    // Update sprite positions and add trail segments only if moved
    if (moved) {
        // Calculate rear positions for trail drawing
        uint16_t new_rear_x, new_rear_y;
        
        // Get rear position based on current direction (for new position)
        get_rear_position(player.x, player.y, player.direction, &new_rear_x, &new_rear_y);
        
        // Add blue trail segment from last trail position to new rear position
        if (!player.first_move) {
            add_trail_segment(player.last_trail_x, player.last_trail_y, new_rear_x, new_rear_y, BLUE_TRAIL_COLOR);
        } else {
            player.first_move = false;
        }
        
        // Update last trail position for next frame
        player.last_trail_x = new_rear_x;
        player.last_trail_y = new_rear_y;
        
        engine_move_sprite(engine, player.current_sprite, player.x, player.y);
    }
    
    if (moved2) {
        // Calculate rear positions for trail drawing
        uint16_t new_rear_x2, new_rear_y2;
        
        // Get rear position based on current direction (for new position)
        get_rear_position(player2.x, player2.y, player2.direction, &new_rear_x2, &new_rear_y2);
        
        // Add orange trail segment from last trail position to new rear position
        if (!player2.first_move) {
            add_trail_segment(player2.last_trail_x, player2.last_trail_y, new_rear_x2, new_rear_y2, ORANGE_TRAIL_COLOR);
        } else {
            player2.first_move = false;
        }
        
        // Update last trail position for next frame
        player2.last_trail_x = new_rear_x2;
        player2.last_trail_y = new_rear_y2;
        
        engine_move_sprite(engine, player2.current_sprite, player2.x, player2.y);
    }
}

// Render function to draw trails
void game_render(game_engine_t *engine) {
    // Draw all active trail segments
    for (int i = 0; i < trail_count; i++) {
        if (trail_segments[i].active) {
            draw_thick_line(engine, 
                          trail_segments[i].x1, 
                          trail_segments[i].y1,
                          trail_segments[i].x2, 
                          trail_segments[i].y2,
                          trail_segments[i].thickness,
                          trail_segments[i].color);
        }
    }
}

int game_init() {
    // Initialize the game engine
    game_engine_t engine;
    
    // Initialize with 0x115 mode (1024x768) at 60 FPS
    if (engine_init(&engine, 0x115, 60) != 0) {
        printf("Failed to initialize game engine\n");
        return 1;
    }
    
    // Clear the screen to black
    engine_clear_screen(&engine, 0x000000);
    
    // Initialize player position and speed
    player.x = 100;  // starting position
    player.y = 100;  // starting position
    player.speed = 5; // pixels per frame
    player.direction = DIR_UP; // start facing up
    player.first_move = true; // First move flag
    
    // Initialize starting trail position for player
    get_rear_position(player.x, player.y, player.direction, &player.last_trail_x, &player.last_trail_y);
    
    // Initialize player2 position and speed
    player2.x = 200;  // starting position (different from player 1)
    player2.y = 200;  // starting position
    player2.speed = 5; // pixels per frame
    player2.direction = DIR_UP; // start facing up
    player2.first_move = true; // First move flag
    
    // Initialize starting trail position for player2
    get_rear_position(player2.x, player2.y, player2.direction, &player2.last_trail_x, &player2.last_trail_y);
    player2.direction = DIR_UP; // start facing up
    player2.first_move = true; // First move flag
    
    // Create all directional sprites for player 1 (blue)
    player.sprite_ids[DIR_UP] = engine_create_sprite(&engine, (xpm_map_t)tron_blue_up, player.x, player.y);
    if (player.sprite_ids[DIR_UP] < 0) {
        printf("Failed to create player UP sprite\n");
        engine_cleanup(&engine);
        return 1;
    }
    
    player.sprite_ids[DIR_DOWN] = engine_create_sprite(&engine, (xpm_map_t)tron_blue_down, player.x, player.y);
    if (player.sprite_ids[DIR_DOWN] < 0) {
        printf("Failed to create player DOWN sprite\n");
        engine_cleanup(&engine);
        return 1;
    }
    
    player.sprite_ids[DIR_LEFT] = engine_create_sprite(&engine, (xpm_map_t)tron_blue_left, player.x, player.y);
    if (player.sprite_ids[DIR_LEFT] < 0) {
        printf("Failed to create player LEFT sprite\n");
        engine_cleanup(&engine);
        return 1;
    }
    
    player.sprite_ids[DIR_RIGHT] = engine_create_sprite(&engine, (xpm_map_t)tron_blue_right, player.x, player.y);
    if (player.sprite_ids[DIR_RIGHT] < 0) {
        printf("Failed to create player RIGHT sprite\n");
        engine_cleanup(&engine);
        return 1;
    }
    
    // Create all directional sprites for player 2 (orange)
    player2.sprite_ids[DIR_UP] = engine_create_sprite(&engine, (xpm_map_t)tron_orange_up, player2.x, player2.y);
    if (player2.sprite_ids[DIR_UP] < 0) {
        printf("Failed to create player2 UP sprite\n");
        engine_cleanup(&engine);
        return 1;
    }
    
    player2.sprite_ids[DIR_DOWN] = engine_create_sprite(&engine, (xpm_map_t)tron_orange_down, player2.x, player2.y);
    if (player2.sprite_ids[DIR_DOWN] < 0) {
        printf("Failed to create player2 DOWN sprite\n");
        engine_cleanup(&engine);
        return 1;
    }
    
    player2.sprite_ids[DIR_LEFT] = engine_create_sprite(&engine, (xpm_map_t)tron_orange_left, player2.x, player2.y);
    if (player2.sprite_ids[DIR_LEFT] < 0) {
        printf("Failed to create player2 LEFT sprite\n");
        engine_cleanup(&engine);
        return 1;
    }
    
    player2.sprite_ids[DIR_RIGHT] = engine_create_sprite(&engine, (xpm_map_t)tron_orange_right, player2.x, player2.y);
    if (player2.sprite_ids[DIR_RIGHT] < 0) {
        printf("Failed to create player2 RIGHT sprite\n");
        engine_cleanup(&engine);
        return 1;
    }
    
    // Hide all sprites initially (they are created as visible by default)
    engine_hide_sprite(&engine, player.sprite_ids[DIR_UP]);
    engine_hide_sprite(&engine, player.sprite_ids[DIR_DOWN]);
    engine_hide_sprite(&engine, player.sprite_ids[DIR_LEFT]);
    engine_hide_sprite(&engine, player.sprite_ids[DIR_RIGHT]);
    
    engine_hide_sprite(&engine, player2.sprite_ids[DIR_UP]);
    engine_hide_sprite(&engine, player2.sprite_ids[DIR_DOWN]);
    engine_hide_sprite(&engine, player2.sprite_ids[DIR_LEFT]);
    engine_hide_sprite(&engine, player2.sprite_ids[DIR_RIGHT]);
    
    // Set initial sprite (facing up) and show only this one for both players
    player.current_sprite = player.sprite_ids[DIR_UP];
    engine_show_sprite(&engine, player.current_sprite);
    
    player2.current_sprite = player2.sprite_ids[DIR_UP];
    engine_show_sprite(&engine, player2.current_sprite);
    
    // Register update and render callbacks
    engine_set_update_callback(&engine, game_update);
    engine_set_render_callback(&engine, game_render);
    
    // Run the game loop
    int result = engine_run(&engine);
    
    // Clean up engine resources
    engine_cleanup(&engine);
    
    return result;
}
