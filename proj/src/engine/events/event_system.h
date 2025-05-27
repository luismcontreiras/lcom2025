#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include "../core/types.h"

#define MAX_EVENT_HANDLERS 32

/**
 * @brief Function pointer type for event handlers
 */
typedef void (*EventHandler)(Event* event, void* user_data);

/**
 * @brief Structure to store event handler information
 */
typedef struct {
    EventHandler handler;
    void* user_data;
    EventType event_type;
} EventHandlerEntry;

/**
 * @brief Main event system structure
 */
typedef struct {
    EventHandlerEntry handlers[MAX_EVENT_HANDLERS];
    int handler_count;
    bool running;
} EventSystem;

/**
 * @brief Initialize event system
 * @param system Pointer to the system to initialize
 * @return 0 on success, non-zero otherwise 
 */
int event_system_init(EventSystem* system);

/**
 * @brief Register an event handler for a specific event type
 * @param system Event system
 * @param type Type of event to handle
 * @param handler Function to call when event occurs
 * @param user_data Custom data to pass to handler
 * @return 0 on success, non-zero otherwise
 */
int event_system_register_handler(EventSystem* system, EventType type, 
                                  EventHandler handler, void* user_data);

/**
 * @brief Dispatch an event to registered handlers
 * @param system Event system
 * @param event Event to dispatch
 */
void event_system_dispatch(EventSystem* system, Event* event);

/**
 * @brief Stop the event system (set running to false)
 * @param system Event system to stop
 */
void event_system_stop(EventSystem* system);

/**
 * @brief Check if the event system is running
 * @param system Event system to check
 * @return true if running, false otherwise
 */
bool event_system_is_running(EventSystem* system);

#endif /* EVENT_SYSTEM_H */
