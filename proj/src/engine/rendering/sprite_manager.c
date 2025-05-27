#include "sprite_manager.h"
#include <stdio.h>
#include <string.h>

int sprite_manager_init(SpriteManager* manager, bool use_double_buffering) {
    if (manager == NULL) return 1;
    
    manager->sprite_count = 0;
    manager->double_buffering = use_double_buffering;
    memset(manager->sprites, 0, sizeof(manager->sprites));
    
    return 0;
}

int sprite_manager_add(SpriteManager* manager, Sprite* sprite) {
    if (manager == NULL || sprite == NULL) return -1;
    
    if (manager->sprite_count >= MAX_SPRITES) {
        printf("Error: Maximum number of sprites reached\n");
        return -1;
    }
    
    manager->sprites[manager->sprite_count] = sprite;
    return manager->sprite_count++;
}

int sprite_manager_remove(SpriteManager* manager, int index) {
    if (manager == NULL || index < 0 || index >= manager->sprite_count) return 1;
    
    // Move sprites down to fill the gap
    for (int i = index; i < manager->sprite_count - 1; i++) {
        manager->sprites[i] = manager->sprites[i + 1];
    }
    
    manager->sprite_count--;
    manager->sprites[manager->sprite_count] = NULL;
    
    return 0;
}

Sprite* sprite_manager_get(SpriteManager* manager, int index) {
    if (manager == NULL || index < 0 || index >= manager->sprite_count) return NULL;
    
    return manager->sprites[index];
}

int sprite_manager_render_all(SpriteManager* manager) {
    if (manager == NULL) return 1;
    
    // Render all visible sprites
    for (int i = 0; i < manager->sprite_count; i++) {
        Sprite* sprite = manager->sprites[i];
        if (sprite != NULL && sprite->visible) {
            if (sprite_render(sprite) != 0) {
                return 1;
            }
        }
    }
    
    return 0;
}

int sprite_manager_clear_all(SpriteManager* manager) {
    if (manager == NULL) return 1;
    
    // Clear all sprites
    for (int i = 0; i < manager->sprite_count; i++) {
        Sprite* sprite = manager->sprites[i];
        if (sprite != NULL) {
            if (sprite_clear(sprite) != 0) {
                return 1;
            }
        }
    }
    
    return 0;
}

int sprite_manager_swap_buffers(SpriteManager* manager) {
    if (manager == NULL) return 1;
    
    if (manager->double_buffering) {
        // This would need to be implemented in the graphics hardware layer
        // For now, we'll just make a placeholder
        printf("Double buffering not implemented yet\n");
    }
    
    return 0;
}

int sprite_manager_update(SpriteManager* manager, float dt) {
    if (manager == NULL) return 1;
    
    // Update logic would go here - for now just a placeholder
    // In a real game, you might have velocities and other properties
    
    return 0;
}
