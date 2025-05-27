#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "../core/types.h"
#include "../events/event_system.h"

// Key codes (based on your existing ARROW_UP_SCANCODE, etc.)
#define KEY_ESC 0x01
#define KEY_ARROW_UP 0x48
#define KEY_ARROW_DOWN 0x50
#define KEY_ARROW_LEFT 0x4B
#define KEY_ARROW_RIGHT 0x4D

/**
 * @brief Manages keyboard and mouse input, abstracting the hardware details
 */
typedef struct {
    EventSystem* event_system;
    bool keys[256];  // Track key states
    uint16_t mouse_x, mouse_y;
    uint8_t mouse_buttons;
} InputManager;

/**
 * @brief Initialize the input manager
 * @param manager Input manager to initialize
 * @param event_system Event system to dispatch events to
 * @return 0 on success, non-zero otherwise
 */
int input_manager_init(InputManager* manager, EventSystem* event_system);

/**
 * @brief Process a keyboard scancode
 * @param manager Input manager
 * @param scancode The scancode received from the keyboard
 */
void input_manager_handle_scancode(InputManager* manager, uint8_t scancode);

/**
 * @brief Check if a key is currently pressed
 * @param manager Input manager
 * @param key_code Key code to check
 * @return true if pressed, false otherwise
 */
bool input_manager_is_key_pressed(InputManager* manager, uint8_t key_code);

/**
 * @brief Handle mouse packet data
 * @param manager Input manager
 * @param packet Mouse packet data
 */
void input_manager_handle_mouse(InputManager* manager, uint8_t packet[3]);

/**
 * @brief Get current mouse position
 * @param manager Input manager
 * @param x Pointer to store x coordinate
 * @param y Pointer to store y coordinate
 */
void input_manager_get_mouse_position(InputManager* manager, uint16_t* x, uint16_t* y);

/**
 * @brief Check if a mouse button is pressed
 * @param manager Input manager
 * @param button Button to check (0 = left, 1 = right, 2 = middle)
 * @return true if pressed, false otherwise
 */
bool input_manager_is_mouse_button_pressed(InputManager* manager, uint8_t button);

#endif /* INPUT_MANAGER_H */
