#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 2D vector representing a position or displacement
 */
typedef struct {
    int16_t x, y;
} Vector2;

/**
 * @brief Size of an object (width and height)
 */
typedef struct {
    uint16_t width, height;
} Size;

/**
 * @brief Rectangle representing position and size
 */
typedef struct {
    Vector2 position;
    Size size;
} Rectangle;

/**
 * @brief Type of event
 */
typedef enum {
    EVENT_KEYBOARD,
    EVENT_TIMER,
    EVENT_MOUSE,
    EVENT_QUIT
} EventType;

/**
 * @brief Event data structure
 */
typedef struct {
    EventType type;
    union {
        struct {
            uint8_t scancode;
            bool make_code; // true if make code, false if break code
        } keyboard;
        struct {
            uint32_t ticks;
        } timer;
        struct {
            int16_t x, y;
            uint8_t buttons;
        } mouse;
    } data;
} Event;

#endif /* TYPES_H */
