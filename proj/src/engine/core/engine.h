#ifndef ENGINE_H
#define ENGINE_H

#include "../core/types.h"
#include "../events/event_system.h"
#include "../input/input_manager.h"
#include "../rendering/sprite_manager.h"
#include "../utils/time_manager.h"
#include <lcom/lcf.h>

/**
 * @brief Configuration for the game engine
 */
typedef struct {
    uint16_t video_mode;       // Video mode to use
    uint8_t frame_rate;        // Target frame rate
    bool use_double_buffering; // Whether to use double buffering
    bool use_mouse;            // Whether to initialize mouse
    const char* title;         // Game title
} EngineConfig;

/**
 * @brief Main game engine structure
 */
typedef struct {
    // Subsystems
    EventSystem event_system;
    InputManager input_manager;
    SpriteManager sprite_manager;
    TimeManager time_manager;
    
    // Hardware state
    uint16_t video_mode;
    uint8_t timer_bit_no;
    uint8_t frame_rate;
    uint16_t screen_width;
    uint16_t screen_height;
    
    // Engine state
    bool initialized;
    bool running;
    
    // Configuration
    EngineConfig config;
} Engine;

/**
 * @brief Initialize the game engine
 * @param engine Engine to initialize
 * @param config Engine configuration
 * @return 0 on success, non-zero otherwise
 */
int engine_init(Engine* engine, EngineConfig config);

/**
 * @brief Run the main game loop
 * @param engine The engine to run
 * @return 0 on success, non-zero otherwise
 */
int engine_run(Engine* engine);

/**
 * @brief Stop the game engine
 * @param engine The engine to stop
 */
void engine_stop(Engine* engine);

/**
 * @brief Clean up engine resources
 * @param engine Engine to shut down
 */
void engine_shutdown(Engine* engine);

/**
 * @brief Add a sprite to the engine
 * @param engine The engine
 * @param xpm XPM data
 * @param type XPM image type
 * @param position Initial position
 * @return Index of the sprite or -1 on failure
 */
int engine_add_sprite(Engine* engine, xpm_map_t xpm, enum xpm_image_type type, Vector2 position);

/**
 * @brief Register an event handler
 * @param engine The engine
 * @param event_type Type of event to handle
 * @param handler Handler function
 * @param user_data User data to pass to handler
 * @return 0 on success, non-zero otherwise
 */
int engine_register_handler(Engine* engine, EventType event_type, EventHandler handler, void* user_data);

#endif /* ENGINE_H */
