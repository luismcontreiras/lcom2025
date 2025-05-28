#include "game_engine.h"
#include "graphics.h"
#include "kbc.h"
#include "mouse.h"
#include <string.h>

// Global callback pointers
static game_update_callback_t g_update_callback = NULL;
static game_render_callback_t g_render_callback = NULL;

// External variables from your existing code
extern vbe_mode_info_t mode_info;
extern uint8_t array_scancodes[2];
extern struct packet pp;
extern uint8_t byte_index;

int engine_init(game_engine_t *engine, uint16_t mode, uint8_t fps) {
    if (!engine) return 1;
    
    // Clear engine structure
    memset(engine, 0, sizeof(game_engine_t));
    
    // Set basic parameters
    engine->video_mode = mode;
    engine->target_fps = fps;
    engine->kbd_hook_id = 1;
    engine->mouse_hook_id = 2;
    engine->timer_hook_id = 0;
    
    // Initialize graphics
    if (set_frame_buffer(mode) != 0) {
        printf("Failed to set frame buffer\n");
        return 1;
    }
    
    if (set_to_video_mode(mode) != 0) {
        printf("Failed to set video mode\n");
        return 1;
    }
    
    // Store screen dimensions
    engine->screen_width = mode_info.XResolution;
    engine->screen_height = mode_info.YResolution;
    
    // Subscribe to interrupts
    if (subscribe_kbc_interrupts() != 0) {
        printf("Failed to subscribe keyboard interrupts\n");
        return 1;
    }
    
    uint8_t mouse_mask, timer_mask;
    if (mouse_subscribe_int(&mouse_mask) != 0) {
        printf("Failed to subscribe mouse interrupts\n");
        unsubscribe_kbc_interrupts();
        return 1;
    }
    
    if (timer_subscribe_int(&timer_mask) != 0) {
        printf("Failed to subscribe timer interrupts\n");
        unsubscribe_kbc_interrupts();
        mouse_unsubscribe_int();
        return 1;
    }
    
    // Set timer frequency
    if (timer_set_frequency(0, fps) != 0) {
        printf("Failed to set timer frequency\n");
        engine_cleanup(engine);
        return 1;
    }
    
    // Enable mouse data reporting
    if (mouse_write(ENABLE_DATA_REPORTING) != 0) {
        printf("Failed to enable mouse reporting\n");
        engine_cleanup(engine);
        return 1;
    }
    
    // Initialize sprites
    for (int i = 0; i < MAX_SPRITES; i++) {
        engine->sprites[i].visible = false;
        engine->sprites[i].color = NULL;
    }
    
    engine->running = true;
    engine->initialized = true;
    
    return 0;
}

void engine_cleanup(game_engine_t *engine) {
    if (!engine || !engine->initialized) return;
    
    // Disable mouse reporting
    mouse_write(DISABLE_DATA_REPORTING);
    
    // Unsubscribe from interrupts
    unsubscribe_kbc_interrupts();
    mouse_unsubscribe_int();
    timer_unsubscribe_int();
    
    // Clean up sprites
    for (int i = 0; i < MAX_SPRITES; i++) {
        if (engine->sprites[i].color) {
            free(engine->sprites[i].color);
            engine->sprites[i].color = NULL;
        }
    }
    
    // Exit graphics mode
    vg_exit();
    
    engine->initialized = false;
}

int engine_run(game_engine_t *engine) {
    if (!engine || !engine->initialized) return 1;
    
    int ipc_status, r;
    message msg;
    uint32_t last_frame_time = 0;
    
    while (engine->running) {
        // Wait for interrupt
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    // Handle keyboard interrupt
                    if (msg.m_notify.interrupts & BIT(engine->kbd_hook_id)) {
                        kbc_ih();
                        engine_handle_keyboard(engine);
                    }
                    
                    // Handle mouse interrupt
                    if (msg.m_notify.interrupts & BIT(engine->mouse_hook_id)) {
                        mouse_ih();
                        mouse_sync_bytes();
                        if (byte_index == 3) {
                            byte_index = 0;
                            mouse_bytes_to_packet();
                            engine_handle_mouse(engine);
                        }
                    }
                    
                    // Handle timer interrupt
                    if (msg.m_notify.interrupts & BIT(engine->timer_hook_id)) {
                        timer_int_handler();
                        engine_handle_timer(engine);
                        
                        // Calculate delta time
                        uint32_t current_time = engine->frame_count;
                        float delta_time = (current_time - last_frame_time) / (float)engine->target_fps;
                        last_frame_time = current_time;
                        
                        // Call update callback
                        if (g_update_callback) {
                            g_update_callback(engine, delta_time);
                        }
                        
                        // Update and render sprites
                        engine_update_sprites(engine);
                        engine_render_sprites(engine);
                        
                        // Call render callback
                        if (g_render_callback) {
                            g_render_callback(engine);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        
        // Check for ESC key to quit
        if (engine_key_pressed(engine, ESC_SCANCODE)) {
            engine->running = false;
        }
    }
    
    return 0;
}

void engine_stop(game_engine_t *engine) {
    if (engine) {
        engine->running = false;
    }
}

bool engine_key_pressed(game_engine_t *engine, uint8_t scancode) {
    if (!engine) return false;
    return engine->input.keys[scancode];
}

bool engine_key_released(game_engine_t *engine, uint8_t scancode) {
    if (!engine) return false;
    return !engine->input.keys[scancode];
}

bool engine_mouse_clicked(game_engine_t *engine, int button) {
    if (!engine) return false;
    switch (button) {
        case 0: return engine->input.mouse_left;
        case 1: return engine->input.mouse_right;
        case 2: return engine->input.mouse_middle;
        default: return false;
    }
}

int engine_create_sprite(game_engine_t *engine, xpm_map_t xpm, uint16_t x, uint16_t y) {
    if (!engine || engine->sprite_count >= MAX_SPRITES) return -1;
    
    xpm_image_t img;
    uint32_t *color =(uint32_t *) xpm_load(xpm, XPM_8_8_8_8, &img);
    
    if (!color) {
        printf("Erro xpm_load\n");
        return -1;
    } 
    
    int sprite_id = engine->sprite_count++;
    sprite_t *sprite = &engine->sprites[sprite_id];
    
    sprite->color = color;
    sprite->x = x;
    sprite->y = y;
    sprite->old_x = x;
    sprite->old_y = y;
    sprite->width = img.width;
    sprite->height = img.height;
    sprite->visible = true;
    sprite->dirty = true;
    
    return sprite_id;
}

int engine_create_sprite_from_data(game_engine_t *engine, uint8_t *data, uint16_t width, uint16_t height, uint16_t x, uint16_t y) {
    if (!engine || !data || engine->sprite_count >= MAX_SPRITES) return -1;
    
    int sprite_id = engine->sprite_count++;
    sprite_t *sprite = &engine->sprites[sprite_id];
    
    // Copy pixel data
    size_t data_size = width * height * (mode_info.BitsPerPixel / 8);
    sprite->color = malloc(data_size);
    if (!sprite->color) return -1;
    
    memcpy(sprite->color, data, data_size);
    
    sprite->x = x;
    sprite->y = y;
    sprite->old_x = x;
    sprite->old_y = y;
    sprite->width = width;
    sprite->height = height;
    sprite->visible = true;
    sprite->dirty = true;
    
    return sprite_id;
}

void engine_move_sprite(game_engine_t *engine, uint8_t sprite_id, uint16_t x, uint16_t y) {
    if (!engine || sprite_id >= engine->sprite_count) return;
    
    sprite_t *sprite = &engine->sprites[sprite_id];
    
    // Store old position for clearing
    sprite->old_x = sprite->x;
    sprite->old_y = sprite->y;
    
    // Set new position
    sprite->x = x;
    sprite->y = y;
    sprite->dirty = true;
}

void engine_show_sprite(game_engine_t *engine, uint8_t sprite_id) {
    if (!engine || sprite_id >= engine->sprite_count) return;
    
    engine->sprites[sprite_id].visible = true;
    engine->sprites[sprite_id].dirty = true;
}

void engine_hide_sprite(game_engine_t *engine, uint8_t sprite_id) {
    if (!engine || sprite_id >= engine->sprite_count) return;
    
    engine->sprites[sprite_id].visible = false;
    engine->sprites[sprite_id].dirty = true;
}

void engine_destroy_sprite(game_engine_t *engine, uint8_t sprite_id) {
    if (!engine || sprite_id >= engine->sprite_count) return;
    
    sprite_t *sprite = &engine->sprites[sprite_id];
    
    if (sprite->color) {
        free(sprite->color);
        sprite->color = NULL;
    }
    
    sprite->visible = false;
    sprite->dirty = false;
}

int engine_draw_pixel(game_engine_t *engine, uint16_t x, uint16_t y, uint32_t color) {
    if (!engine) return 1;
    return vg_draw_pixel(x, y, color);
}

int engine_draw_rectangle(game_engine_t *engine, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    if (!engine) return 1;
    return vg_draw_rectangle(x, y, width, height, color);
}

int engine_clear_screen(game_engine_t *engine, uint32_t color) {
    if (!engine) return 1;
    return vg_draw_rectangle(0, 0, engine->screen_width, engine->screen_height, color);
}

void engine_set_update_callback(game_engine_t *engine, game_update_callback_t callback) {
    g_update_callback = callback;
}

void engine_set_render_callback(game_engine_t *engine, game_render_callback_t callback) {
    g_render_callback = callback;
}

// Internal functions
void engine_handle_keyboard(game_engine_t *engine) {
    if (!engine) return;
    
    uint8_t scancode = array_scancodes[0];
    bool is_make = !(scancode & 0x80);
    uint8_t key_code = scancode & 0x7F;
    
    engine->input.keys[key_code] = is_make;
    engine->last_scancode = scancode;
    engine->new_input = true;
}

void engine_handle_mouse(game_engine_t *engine) {
    if (!engine) return;
    
    engine->input.mouse_left = pp.lb;
    engine->input.mouse_right = pp.rb;
    engine->input.mouse_middle = pp.mb;
    engine->input.mouse_delta_x = pp.delta_x;
    engine->input.mouse_delta_y = pp.delta_y;
    
    // Update mouse position (clamped to screen)
    engine->input.mouse_x += pp.delta_x;
    engine->input.mouse_y -= pp.delta_y; // Invert Y axis
    
    if (engine->input.mouse_x < 0) engine->input.mouse_x = 0;
    if (engine->input.mouse_x >= engine->screen_width) engine->input.mouse_x = engine->screen_width - 1;
    if (engine->input.mouse_y < 0) engine->input.mouse_y = 0;
    if (engine->input.mouse_y >= engine->screen_height) engine->input.mouse_y = engine->screen_height - 1;
    
    engine->new_input = true;
}

void engine_handle_timer(game_engine_t *engine) {
    if (!engine) return;
    
    engine->frame_count++;
}

void engine_update_sprites(game_engine_t *engine) {
    if (!engine) return;
    
    // Update sprite logic here if needed
    // This is where you could add animations, physics, etc.
}

void engine_render_sprites(game_engine_t *engine) {
    if (!engine){
        printf("Engine fail\n");
      return;  
    } 
     printf("Enter render\n");
    for (int i = 0; i < engine->sprite_count; i++) {
        sprite_t *sprite = &engine->sprites[i];
        
        if (!sprite->visible || !sprite->dirty || !sprite->color) continue;
        printf("Sprite is visible!\n");
        // Clear old position if sprite moved
        if (sprite->old_x != sprite->x || sprite->old_y != sprite->y) {
            printf("Enter draw rect %d %d %d %d\n", sprite->old_x, sprite->old_y, sprite->width, sprite->height);
            if(vg_draw_rectangle(sprite->old_x, sprite->old_y, sprite->width, sprite->height, 0x000000)){
                printf("Rectangle error \n");
            };
        }

        //vg_draw_rectangle(15, 15, 200, 200, 0xFFFAAA);
        
        // Draw sprite at new position
        // This is a simplified version - you might want to implement proper sprite blitting
         for (int h = 0; h < sprite->height; h++) {
             for (int w = 0; w < sprite->width; w++) {
                  uint16_t screen_x = sprite->x + h;
                  uint16_t screen_y = sprite->y + w;
                
                 if (screen_x < engine->screen_width && screen_y < engine->screen_height) {
                      // Extract color from sprite->color
                     // This would need proper color extraction based on your pixel format
                     vg_draw_pixel(sprite->x  + w, sprite->y+ h, sprite->color[w + h*sprite->width]);
                 }
             }
         }

        sprite->dirty = false;
        sprite->old_x = sprite->x;
        sprite->old_y = sprite->y;
    }
}
