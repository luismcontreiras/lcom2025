#include "event_system.h"
#include <stdio.h>

int event_system_init(EventSystem* system) {
    if (system == NULL) return 1;
    
    system->handler_count = 0;
    system->running = true;
    return 0;
}

int event_system_register_handler(EventSystem* system, EventType type, 
                                  EventHandler handler, void* user_data) {
    if (system == NULL || handler == NULL) return 1;
    
    if (system->handler_count >= MAX_EVENT_HANDLERS) {
        printf("Error: Maximum number of event handlers reached\n");
        return 1;
    }
    
    system->handlers[system->handler_count].handler = handler;
    system->handlers[system->handler_count].user_data = user_data;
    system->handlers[system->handler_count].event_type = type;
    system->handler_count++;
    
    return 0;
}

void event_system_dispatch(EventSystem* system, Event* event) {
    if (system == NULL || event == NULL) return;
    
    for (int i = 0; i < system->handler_count; i++) {
        if (system->handlers[i].event_type == event->type) {
            system->handlers[i].handler(event, system->handlers[i].user_data);
        }
    }
}

void event_system_stop(EventSystem* system) {
    if (system == NULL) return;
    system->running = false;
}

bool event_system_is_running(EventSystem* system) {
    if (system == NULL) return false;
    return system->running;
}
