#include <lcom/lcf.h>
#include <stdlib.h>
#include <stdio.h>

#include "engine/core/engine.h"
#include "game/game.h"

// Example XPM for player sprite (ideally this would be in its own file)
static const char *player_xpm_data[] = {
    /* width height num_colors chars_per_pixel */
    "20 20 3 1",
    /* colors */
    "  c #000000", /* black */
    ". c #FFFFFF", /* white */
    "X c #FF0000", /* red */
    /* pixels */
    "                    ",
    "       XXXXX       ",
    "      XXXXXXX      ",
    "     XXXXXXXXX     ",
    "    XXXXXXXXXXX    ",
    "    XXXXXXXXXXX    ",
    "    XXX     XXX    ",
    "    XXX     XXX    ",
    "    XXXXXXXXXXX    ",
    "    XXXXXXXXXXX    ",
    "     XXXXXXXXX     ",
    "      XXXXXXX      ",
    "       XXXXX       ",
    "        XXX        ",
    "        XXX        ",
    "        XXX        ",
    "       XXXXX       ",
    "      XXXXXXX      ",     "     XX     XX     ",
    "                    "
};

// Convert the string array to an XPM format that the engine can use
xpm_map_t player_xpm = player_xpm_data;

int main(int argc, char *argv[]) {
    // Sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // Enables to log function invocations that are being "wrapped" by LCF
    lcf_trace_calls("trace.txt");

    // Enables to save the output of printf function calls on a file
    lcf_log_output("output.txt");

    printf("Starting game engine demo\n");

    // Create engine configuration
    EngineConfig config;
    config.video_mode = 0x105;  // 1024x768 VGA
    config.frame_rate = 60;
    config.use_double_buffering = false;
    config.use_mouse = false;
    config.title = "Game Engine Demo";

    // Initialize engine
    Engine engine;
    if (engine_init(&engine, config) != 0) {
        printf("Failed to initialize engine\n");
        return 1;
    }

    // Initialize game
    if (game_init(&engine) != 0) {
        printf("Failed to initialize game\n");
        engine_shutdown(&engine);
        return 1;
    }

    printf("Starting main game loop\n");

    // Run the game
    if (engine_run(&engine) != 0) {
        printf("Error during game execution\n");
    }

    // Cleanup
    game_cleanup(&engine);
    engine_shutdown(&engine);

    printf("Game terminated successfully\n");

    return 0;
}
