#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include <stdbool.h>
#include <lcom/lcf.h>
#include "../core/types.h"

/**
 * @brief Structure representing an XPM sprite with position and size information
 */
typedef struct {
    xpm_map_t* xmp_data;         // Pointer to original XPM data source
    enum xpm_image_type type;    // XPM image type
    uint8_t* pixmap;             // Pixmap data (from xpm_load)
    Size size;                   // Width and height
    Vector2 position;            // Current position
    bool visible;                // Whether sprite is visible
    uint32_t background_color;   // Color to use when clearing sprite
} Sprite;

/**
 * @brief Create a new sprite from XPM data
 * @param sprite The sprite to initialize
 * @param xpm Pointer to XPM data
 * @param type XPM image type
 * @param position Initial position
 * @return 0 on success, non-zero otherwise
 */
int sprite_create(Sprite* sprite, xpm_map_t* xpm, enum xpm_image_type type, Vector2 position);

/**
 * @brief Clean up sprite resources
 * @param sprite The sprite to destroy
 */
void sprite_destroy(Sprite* sprite);

/**
 * @brief Render the sprite at its current position
 * @param sprite The sprite to render
 * @return 0 on success, non-zero otherwise
 */
int sprite_render(Sprite* sprite);

/**
 * @brief Clear the sprite from the screen (draw background color)
 * @param sprite The sprite to clear
 * @return 0 on success, non-zero otherwise
 */
int sprite_clear(Sprite* sprite);

/**
 * @brief Set sprite position
 * @param sprite The sprite to update
 * @param position New position
 */
void sprite_set_position(Sprite* sprite, Vector2 position);

/**
 * @brief Move sprite by a relative amount
 * @param sprite The sprite to move
 * @param delta Amount to move by
 * @return 0 on success, non-zero otherwise
 */
int sprite_move(Sprite* sprite, Vector2 delta);

/**
 * @brief Check if sprite is at screen boundaries
 * @param sprite The sprite to check
 * @param screen_width Screen width
 * @param screen_height Screen height
 * @return true if sprite is at boundary, false otherwise
 */
bool sprite_check_bounds(Sprite* sprite, uint16_t screen_width, uint16_t screen_height);

#endif /* SPRITE_H */
