#include "input_manager.h"
#include <string.h>

int input_manager_init(InputManager* manager, EventSystem* event_system) {
    if (manager == NULL || event_system == NULL) return 1;
    
    manager->event_system = event_system;
    memset(manager->keys, 0, sizeof(manager->keys));
    manager->mouse_x = 0;
    manager->mouse_y = 0;
    manager->mouse_buttons = 0;
    
    return 0;
}

void input_manager_handle_scancode(InputManager* manager, uint8_t scancode) {
    if (manager == NULL) return;
    
    bool is_make = !(scancode & 0x80); // Check if it's a make code (key press)
    uint8_t key = scancode & 0x7F;     // Extract the key code
    
    // Update key state
    manager->keys[key] = is_make;
    
    // Create and dispatch keyboard event
    Event event;
    event.type = EVENT_KEYBOARD;
    event.data.keyboard.scancode = key;
    event.data.keyboard.make_code = is_make;
    
    event_system_dispatch(manager->event_system, &event);
    
    // Check for ESC to trigger quit
    if (key == KEY_ESC && is_make) {
        Event quit_event;
        quit_event.type = EVENT_QUIT;
        event_system_dispatch(manager->event_system, &quit_event);
    }
}

bool input_manager_is_key_pressed(InputManager* manager, uint8_t key_code) {
    if (manager == NULL) return false;
    return manager->keys[key_code];
}

void input_manager_handle_mouse(InputManager* manager, uint8_t packet[3]) {
    if (manager == NULL || packet == NULL) return;
    
    // Update mouse state based on packet
    manager->mouse_buttons = packet[0] & 0x07; // First 3 bits are button states
    
    // Update position based on movement deltas in the packet
    int16_t delta_x = packet[1];
    int16_t delta_y = packet[2];
    
    // Sign extend if MSB is set
    if (packet[0] & 0x10) delta_x |= 0xFF00; // X sign bit
    if (packet[0] & 0x20) delta_y |= 0xFF00; // Y sign bit
    
    // Y is inverted in PS/2 mouse
    delta_y = -delta_y;
    
    // Update coordinates (with boundary checks in real implementation)
    manager->mouse_x += delta_x;
    manager->mouse_y += delta_y;
    
    // Dispatch mouse event
    Event event;
    event.type = EVENT_MOUSE;
    event.data.mouse.x = manager->mouse_x;
    event.data.mouse.y = manager->mouse_y;
    event.data.mouse.buttons = manager->mouse_buttons;
    
    event_system_dispatch(manager->event_system, &event);
}

void input_manager_get_mouse_position(InputManager* manager, uint16_t* x, uint16_t* y) {
    if (manager == NULL || x == NULL || y == NULL) return;
    
    *x = manager->mouse_x;
    *y = manager->mouse_y;
}

bool input_manager_is_mouse_button_pressed(InputManager* manager, uint8_t button) {
    if (manager == NULL || button > 2) return false;
    
    return (manager->mouse_buttons & (1 << button)) != 0;
}
