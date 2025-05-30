#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

// Forward declarations


// Engine constants
#define MAX_SPRITES 50
#define DEFAULT_FRAME_RATE 60
#define ESC_SCANCODE 0x81

// Input states
typedef struct {
    bool keys[256];           // Keyboard state
    bool mouse_left;          // Left mouse button
    bool mouse_right;         // Right mouse button  
    bool mouse_middle;        // Middle mouse button
    int16_t mouse_x;          // Mouse X position
    int16_t mouse_y;          // Mouse Y position
    int16_t mouse_delta_x;    // Mouse movement delta X
    int16_t mouse_delta_y;    // Mouse movement delta Y
} input_state_t;

// Sprite structure
typedef struct sprite {
    uint32_t *color;          // Sprite pixel data
    uint16_t x, y;            // Position
    uint16_t width, height;   // Dimensions
    bool visible;             // Visibility flag
    bool dirty;               // Needs redraw flag
    uint16_t old_x, old_y;    // Previous position for clearing
} sprite_t;

// Game engine structure
typedef struct game_engine {
    // Graphics
    uint16_t screen_width;
    uint16_t screen_height;
    uint16_t video_mode;
    uint8_t *video_mem;
    uint8_t *back_buffer;         // Back buffer for double buffering
    uint32_t buffer_size;         // Size of the buffer in bytes
    uint8_t bytes_per_pixel;      // Bytes per pixel
    
    // Input
    input_state_t input;
    uint8_t last_scancode;
    bool new_input;
    
    // Timing
    uint32_t frame_count;
    uint8_t target_fps;
    
    // Sprites
    sprite_t sprites[MAX_SPRITES];
    uint8_t sprite_count;
    
    // State
    bool running;
    bool initialized;
    
    // Interrupt hooks
    int kbd_hook_id;
    int mouse_hook_id;
    int timer_hook_id;
} game_engine_t;

// Core engine functions
int engine_init(game_engine_t *engine, uint16_t mode, uint8_t fps);
void engine_cleanup(game_engine_t *engine);
int engine_run(game_engine_t *engine);
void engine_stop(game_engine_t *engine);

// Input functions
bool engine_key_pressed(game_engine_t *engine, uint8_t scancode);
bool engine_key_released(game_engine_t *engine, uint8_t scancode);
bool engine_mouse_clicked(game_engine_t *engine, int button); // 0=left, 1=right, 2=middle

// Sprite functions
int engine_create_sprite(game_engine_t *engine, xpm_map_t xpm, uint16_t x, uint16_t y);
int engine_create_sprite_from_data(game_engine_t *engine, uint8_t *data, uint16_t width, uint16_t height, uint16_t x, uint16_t y);
void engine_move_sprite(game_engine_t *engine, uint8_t sprite_id, uint16_t x, uint16_t y);
void engine_show_sprite(game_engine_t *engine, uint8_t sprite_id);
void engine_hide_sprite(game_engine_t *engine, uint8_t sprite_id);
void engine_destroy_sprite(game_engine_t *engine, uint8_t sprite_id);

// Drawing functions
int engine_draw_pixel(game_engine_t *engine, uint16_t x, uint16_t y, uint32_t color);
int engine_draw_rectangle(game_engine_t *engine, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int engine_clear_screen(game_engine_t *engine, uint32_t color);
int engine_swap_buffers(game_engine_t *engine);  // New function for buffer swapping

// Game loop callback type
typedef void (*game_update_callback_t)(game_engine_t *engine, float delta_time);
typedef void (*game_render_callback_t)(game_engine_t *engine);

// Set callbacks for game logic
void engine_set_update_callback(game_engine_t *engine, game_update_callback_t callback);
void engine_set_render_callback(game_engine_t *engine, game_render_callback_t callback);

// Internal functions (don't call directly)
void engine_handle_keyboard(game_engine_t *engine);
void engine_handle_mouse(game_engine_t *engine);
void engine_handle_timer(game_engine_t *engine);
void engine_update_sprites(game_engine_t *engine);
void engine_render_sprites(game_engine_t *engine);

#endif // GAME_ENGINE_H
