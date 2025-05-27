#include "engine.h"
#include "../../hardware/graphics.h"
#include "../../hardware/kbc.h"
#include "../../hardware/timer.h"
#include <stdio.h>
#include <stdlib.h>

extern vbe_mode_info_t mode_info;

// Quit event handler
void engine_quit_handler(Event* event, void* user_data) {
    if (event->type != EVENT_QUIT) return;
    
    Engine* engine = (Engine*)user_data;
    engine_stop(engine);
}

int engine_init(Engine* engine, EngineConfig config) {
    if (engine == NULL) return 1;
    
    // Store configuration
    engine->config = config;
    engine->video_mode = config.video_mode;
    engine->frame_rate = config.frame_rate;
    engine->initialized = false;
    engine->running = false;
    
    // Initialize subsystems
    if (event_system_init(&engine->event_system) != 0) {
        printf("Failed to initialize event system\n");
        return 1;
    }
    
    if (input_manager_init(&engine->input_manager, &engine->event_system) != 0) {
        printf("Failed to initialize input manager\n");
        return 1;
    }
    
    if (sprite_manager_init(&engine->sprite_manager, config.use_double_buffering) != 0) {
        printf("Failed to initialize sprite manager\n");
        return 1;
    }
    
    if (time_manager_init(&engine->time_manager, &engine->event_system, config.frame_rate) != 0) {
        printf("Failed to initialize time manager\n");
        return 1;
    }
    
    // Register quit handler
    if (engine_register_handler(engine, EVENT_QUIT, engine_quit_handler, engine) != 0) {
        printf("Failed to register quit handler\n");
        return 1;
    }
    
    engine->initialized = true;
    return 0;
}

int engine_run(Engine* engine) {
    if (engine == NULL || !engine->initialized) {
        printf("Engine not initialized\n");
        return 1;
    }
    
    // Hardware initialization (extracted from video_move_xpm)
    if (subscribe_kbc_interrupts() != 0) {
        printf("Failed to subscribe to keyboard interrupts\n");
        return 1;
    }
    
    if (engine_timer_subscribe_int(&engine->timer_bit_no) != 0) {
        printf("Failed to subscribe to timer interrupts\n");
        return 1;
    }
    
    if (engine_timer_set_frequency(0, engine->frame_rate) != 0) {
        printf("Failed to set timer frequency\n");
        return 1;
    }
    
    if (set_frame_buffer(engine->video_mode) != 0) {
        printf("Failed to map frame buffer\n");
        return 1;
    }
    
    if (set_to_video_mode(engine->video_mode) != 0) {
        printf("Failed to set video mode\n");
        return 1;
    }
    
    // Store screen dimensions
    engine->screen_width = mode_info.XResolution;
    engine->screen_height = mode_info.YResolution;
    
    printf("Engine started with resolution: %dx%d\n", 
           engine->screen_width, engine->screen_height);
    
    // Main game loop (inspired by video_move_xpm)
    int ipc_status, r;
    message msg;
    
    engine->running = true;
    
    while (engine->running) {
        // Get a request message
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & BIT(1)) {
                        // Keyboard interrupt
                        kbc_ih();
                        
                        // Process keyboard input
                        if (new_scancode_ready) {
                            input_manager_handle_scancode(&engine->input_manager, last_scancode);
                            new_scancode_ready = false;
                        }
                    }
                    
                    if (msg.m_notify.interrupts & BIT(0)) {
                        // Timer interrupt
                        engine_timer_int_handler();
                        time_manager_tick(&engine->time_manager);
                        
                        // This is where you would update game logic and render
                        if (time_manager_is_frame_ready(&engine->time_manager)) {
                            // Update and render game objects
                            sprite_manager_update(&engine->sprite_manager, 
                                                time_manager_get_delta(&engine->time_manager));
                            sprite_manager_render_all(&engine->sprite_manager);
                            
                            // If double buffering is enabled, swap buffers
                            sprite_manager_swap_buffers(&engine->sprite_manager);
                        }
                    }
                    
                    // Add mouse interrupt handling if needed
                    
                    break;
            }
        }
    }
    
    return 0;
}

void engine_stop(Engine* engine) {
    if (engine == NULL) return;
    engine->running = false;
}

void engine_shutdown(Engine* engine) {
    if (engine == NULL || !engine->initialized) return;
    
    printf("Shutting down engine\n");
    
    // Clean up hardware
    vg_exit();
    engine_timer_unsubscribe_int();
    unsubscribe_kbc_interrupts();
    
    // Clean up sprites
    // Note: This doesn't free sprite data as they might be managed externally
    
    engine->initialized = false;
}

int engine_add_sprite(Engine* engine, xpm_map_t xpm, enum xpm_image_type type, Vector2 position) {
    if (engine == NULL || !engine->initialized) return -1;
    
    // Create a new sprite
    Sprite* sprite = (Sprite*)malloc(sizeof(Sprite));
    if (sprite == NULL) {
        printf("Failed to allocate memory for sprite\n");
        return -1;
    }
    
    // Initialize the sprite
    if (sprite_create(sprite, xpm, type, position) != 0) {
        free(sprite);
        return -1;
    }
    
    // Add to sprite manager
    int index = sprite_manager_add(&engine->sprite_manager, sprite);
    if (index < 0) {
        sprite_destroy(sprite);
        free(sprite);
        return -1;
    }
    
    return index;
}

int engine_register_handler(Engine* engine, EventType event_type, EventHandler handler, void* user_data) {
    if (engine == NULL || !engine->initialized) return 1;
    
    return event_system_register_handler(&engine->event_system, event_type, handler, user_data);
}
