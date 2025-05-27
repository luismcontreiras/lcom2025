#include "time_manager.h"

int time_manager_init(TimeManager* manager, EventSystem* event_system, uint8_t frame_rate) {
    if (manager == NULL || event_system == NULL) return 1;
    
    manager->ticks = 0;
    manager->last_time = 0;
    manager->delta_time = 0.0f;
    manager->frame_rate = frame_rate;
    manager->event_system = event_system;
    
    return 0;
}

void time_manager_tick(TimeManager* manager) {
    if (manager == NULL) return;
    
    manager->ticks++;
    
    // Calculate delta time based on tick frequency
    uint32_t current_time = manager->ticks;
    manager->delta_time = (current_time - manager->last_time) / (float)manager->frame_rate;
    manager->last_time = current_time;
    
    // Dispatch timer event
    Event event;
    event.type = EVENT_TIMER;
    event.data.timer.ticks = manager->ticks;
    
    event_system_dispatch(manager->event_system, &event);
}

float time_manager_get_delta(TimeManager* manager) {
    if (manager == NULL) return 0.0f;
    return manager->delta_time;
}

uint32_t time_manager_get_ticks(TimeManager* manager) {
    if (manager == NULL) return 0;
    return manager->ticks;
}

int time_manager_is_frame_ready(TimeManager* manager) {
    // Since we're relying on timer interrupts, each tick means it's time for a new frame
    return 1;
}
