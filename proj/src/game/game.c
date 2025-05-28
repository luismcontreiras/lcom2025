#include "game.h"
#include <lcom/lcf.h>
#include "../engine/game_engine.h"

// XPM for player sprite
static char* const player_xpm[]  = {
    "32 32 4 1",
    "  c None",
    ". c #000000",
    "X c #FF0000",
    "o c #FFFFFF",
    "                                ",
    "                                ",
    "                                ",
    "           ........             ",
    "          ..........            ",
    "         ............           ",
    "        ..XXXXXXXX..            ",
    "       ..XXXXXXXXXX..           ",
    "      ..XXXXXXXXXXXX..          ",
    "     ..XXXXXXXXXXXXXX..         ",
    "     ..XXXXXXXXXXXXXX..         ",
    "    ..XXXXXXXXXXXXXXXX..        ",
    "    ..XXXXXXXXXXXXXXXX..        ",
    "   ..XXXXXXooooooXXXXXX..       ",
    "   ..XXXXXooooooooXXXXX..       ",
    "   ..XXXXooooooooooXXXX..       ",
    "   ..XXXXooooooooooXXXX..       ",
    "   ..XXXXXooooooooXXXXX..       ",
    "   ..XXXXXXooooooXXXXXX..       ",
    "    ..XXXXXXXXXXXXXXXX..        ",
    "    ..XXXXXXXXXXXXXXXX..        ",
    "    ..XXXXXXXXXXXXXXXX..        ",
    "     ..XXXXXXXXXXXXXX..         ",
    "     ..XXXXXXXXXXXXXX..         ",
    "      ..XX....XXXX....XX..      ",
    "      ..XX....XXXX....XX..      ",
    "       ..XXXX....XXXX..         ",
    "       ..XXXX....XXXX..         ",
    "        ....      ....          ",
    "        ....      ....          ",
    "                                ",
    "                                "
};

// Player state
static struct {
    uint16_t x;
    uint16_t y;
    uint8_t sprite_id;
    uint16_t speed;
} player;

// Update function to handle player movement
void game_update(game_engine_t *engine, float delta_time) {
    // Move player based on arrow key input
    if (engine_key_pressed(engine, ARROW_UP_SCANCODE)) {
        if (player.y > player.speed) {
            player.y -= player.speed;
        }
    }
    if (engine_key_pressed(engine, ARROW_DOWN_SCANCODE)) {
        if (player.y < engine->screen_height - 32 - player.speed) {
            player.y += player.speed;
        }
    }
    if (engine_key_pressed(engine, ARROW_LEFT_SCANCODE)) {
        if (player.x > player.speed) {
            player.x -= player.speed;
        }
    }
    if (engine_key_pressed(engine, ARROW_RIGHT_SCANCODE)) {
        if (player.x < engine->screen_width - 32 - player.speed) {
            player.x += player.speed;
        }
    }

    // Update sprite position
    engine_move_sprite(engine, player.sprite_id, player.x, player.y);
}

// Render function (not needed for this simple example, but added for completeness)
void game_render(game_engine_t *engine) {
    // Nothing to do here - engine will handle rendering the sprite
}

int game_init() {
    // Initialize the game engine
    game_engine_t engine;
    
    // Initialize with 0x105 mode (1024x768) at 60 FPS
    if (engine_init(&engine, 0x115, 60) != 0) {
        printf("Failed to initialize game engine\n");
        return 1;
    }
    
    // Clear the screen to black
    engine_clear_screen(&engine, 0x000000);
    
    // Create player spriteprintf("Sprite is visible!")
    player.x = 15;  // center of screen
    player.y = 15; // center of screen
    player.speed = 5; // pixels per frame
    
    // Create sprite from XPM
    player.sprite_id = engine_create_sprite(&engine, (xpm_map_t)player_xpm, player.x, player.y);
    if (player.sprite_id < 0) {
        printf("Failed to create player sprite\n");
        engine_cleanup(&engine);
        return 1;
    }
    
    // Show the sprite
    engine_show_sprite(&engine, player.sprite_id);
    
    // Register update and render callbacks
    engine_set_update_callback(&engine, game_update);
    engine_set_render_callback(&engine, game_render);
    
    // Run the game loop
    int result = engine_run(&engine);
    
    // Clean up engine resources
    engine_cleanup(&engine);
    
    return result;
}
