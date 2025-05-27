#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <stdint.h>
#include "../events/event_system.h"

/**
 * @brief Manages timing, frame rate, and delta time
 */
typedef struct {
    uint32_t ticks;         // Current tick count
    uint32_t last_time;     // Last time stamp
    float delta_time;       // Time between frames in seconds
    uint8_t frame_rate;     // Target frame rate
    EventSystem* event_system;
} TimeManager;

/**
 * @brief Initialize time manager
 * @param manager Time manager to initialize
 * @param event_system Event system to dispatch events to
 * @param frame_rate Target frame rate
 * @return 0 on success, non-zero otherwise
 */
int time_manager_init(TimeManager* manager, EventSystem* event_system, uint8_t frame_rate);

/**
 * @brief Handle timer interrupt
 * @param manager Time manager
 */
void time_manager_tick(TimeManager* manager);

/**
 * @brief Get delta time (time since last frame)
 * @param manager Time manager
 * @return Delta time in seconds
 */
float time_manager_get_delta(TimeManager* manager);

/**
 * @brief Get current tick count
 * @param manager Time manager
 * @return Current tick count
 */
uint32_t time_manager_get_ticks(TimeManager* manager);

/**
 * @brief Check if enough time has passed to process the next frame
 * @param manager Time manager
 * @return 1 if it's time for next frame, 0 otherwise
 */
int time_manager_is_frame_ready(TimeManager* manager);

#endif /* TIME_MANAGER_H */
