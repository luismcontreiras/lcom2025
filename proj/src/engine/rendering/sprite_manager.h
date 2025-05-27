#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "sprite.h"

#define MAX_SPRITES 128

/**
 * @brief Manages multiple sprites, their rendering order and updates
 */
typedef struct {
    Sprite* sprites[MAX_SPRITES];
    int sprite_count;
    bool double_buffering;
} SpriteManager;

/**
 * @brief Initialize sprite manager
 * @param manager The sprite manager to initialize
 * @param use_double_buffering Whether to use double buffering
 * @return 0 on success, non-zero otherwise
 */
int sprite_manager_init(SpriteManager* manager, bool use_double_buffering);

/**
 * @brief Add a sprite to the manager
 * @param manager The sprite manager
 * @param sprite The sprite to add
 * @return Index of added sprite or -1 on failure
 */
int sprite_manager_add(SpriteManager* manager, Sprite* sprite);

/**
 * @brief Remove a sprite from the manager
 * @param manager The sprite manager
 * @param index Index of sprite to remove
 * @return 0 on success, non-zero otherwise
 */
int sprite_manager_remove(SpriteManager* manager, int index);

/**
 * @brief Get a sprite by index
 * @param manager The sprite manager
 * @param index Index of the sprite
 * @return Pointer to sprite or NULL if not found
 */
Sprite* sprite_manager_get(SpriteManager* manager, int index);

/**
 * @brief Render all visible sprites
 * @param manager The sprite manager
 * @return 0 on success, non-zero otherwise
 */
int sprite_manager_render_all(SpriteManager* manager);

/**
 * @brief Clear all sprites from the screen
 * @param manager The sprite manager
 * @return 0 on success, non-zero otherwise
 */
int sprite_manager_clear_all(SpriteManager* manager);

/**
 * @brief Swap buffers if double buffering is enabled
 * @param manager The sprite manager
 * @return 0 on success, non-zero otherwise
 */
int sprite_manager_swap_buffers(SpriteManager* manager);

/**
 * @brief Update all sprite positions based on velocity
 * @param manager The sprite manager
 * @param dt Delta time since last update
 * @return 0 on success, non-zero otherwise
 */
int sprite_manager_update(SpriteManager* manager, float dt);

#endif /* SPRITE_MANAGER_H */
