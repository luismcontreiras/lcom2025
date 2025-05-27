#include "sprite.h"
#include "../../hardware/graphics.h"
#include <stdlib.h>
#include <stdio.h>

extern vbe_mode_info_t mode_info; // Assuming this is defined in your graphics module

int sprite_create(Sprite* sprite, xpm_map_t* xpm, enum xpm_image_type type, Vector2 position) {
    if (sprite == NULL || xpm == NULL || *xpm == NULL) return 1;
    
    sprite->xmp_data = xpm;  // Just store the pointer, no need for copying
    sprite->type = type;
    sprite->position = position;
    sprite->visible = true;
    sprite->background_color = 0x000000; // Default to black
    
    // Load the XPM
    xpm_image_t img;
    sprite->pixmap = xpm_load(*xpm, type, &img);
    
    if (sprite->pixmap == NULL) {
        printf("Error: Failed to load XPM\n");
        return 1;
    }
    
    // Store size
    sprite->size.width = img.width;
    sprite->size.height = img.height;
    
    return 0;
}

void sprite_destroy(Sprite* sprite) {
    if (sprite == NULL) return;
    
    // Free pixmap if it was allocated
    if (sprite->pixmap != NULL) {
        free(sprite->pixmap);
        sprite->pixmap = NULL;
    }
}

int sprite_render(Sprite* sprite) {
    if (sprite == NULL || !sprite->visible) return 0;
    
    // Use existing draw_xpm function from hardware layer
    if (draw_xpm(sprite->position.x, sprite->position.y, *(sprite->xmp_data)) != 0) {
        printf("Error: Failed to draw sprite\n");
        return 1;
    }
    
    return 0;
}

int sprite_clear(Sprite* sprite) {
    if (sprite == NULL) return 1;
    
    // Draw a rectangle with the background color to clear the sprite
    if (vg_draw_rectangle(sprite->position.x, sprite->position.y, 
                          sprite->size.width, sprite->size.height, 
                          sprite->background_color) != 0) {
        printf("Error: Failed to clear sprite\n");
        return 1;
    }
    
    return 0;
}

void sprite_set_position(Sprite* sprite, Vector2 position) {
    if (sprite == NULL) return;
    sprite->position = position;
}

int sprite_move(Sprite* sprite, Vector2 delta) {
    if (sprite == NULL) return 1;
    
    // Clear sprite at old position
    if (sprite_clear(sprite) != 0) {
        return 1;
    }
    
    // Update position
    sprite->position.x += delta.x;
    sprite->position.y += delta.y;
    
    // Draw at new position
    return sprite_render(sprite);
}

bool sprite_check_bounds(Sprite* sprite, uint16_t screen_width, uint16_t screen_height) {
    if (sprite == NULL) return false;
    
    // Check if sprite is at the screen boundaries
    bool at_boundary = false;
    
    if (sprite->position.x <= 0) {
        sprite->position.x = 0;
        at_boundary = true;
    }
    
    if (sprite->position.x + sprite->size.width >= screen_width) {
        sprite->position.x = screen_width - sprite->size.width;
        at_boundary = true;
    }
    
    if (sprite->position.y <= 0) {
        sprite->position.y = 0;
        at_boundary = true;
    }
    
    if (sprite->position.y + sprite->size.height >= screen_height) {
        sprite->position.y = screen_height - sprite->size.height;
        at_boundary = true;
    }
    
    return at_boundary;
}
