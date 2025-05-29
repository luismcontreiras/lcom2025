#include "game.h"
#include <lcom/lcf.h>
#include "../engine/game_engine.h"

// Includ// Character to XPM mapping arrays
static xpm_map_t uppercase_letters[26] = {
    (xpm_map_t)A_xpm, (xpm_map_t)B_xpm, (xpm_map_t)C_xpm, (xpm_map_t)D_xpm, (xpm_map_t)E_xpm, (xpm_map_t)F_xpm, (xpm_map_t)G_xpm, (xpm_map_t)H_xpm, (xpm_map_t)I_xpm, (xpm_map_t)J_xpm,
    (xpm_map_t)K_xpm, (xpm_map_t)L_xpm, (xpm_map_t)M_xpm, (xpm_map_t)N_xpm, (xpm_map_t)O_xpm, (xpm_map_t)P_xpm, (xpm_map_t)Q_xpm, (xpm_map_t)R_xpm, (xpm_map_t)S_xpm, (xpm_map_t)T_xpm,
    (xpm_map_t)U_xpm, (xpm_map_t)V_xpm, (xpm_map_t)W_xpm, (xpm_map_t)X_xpm, (xpm_map_t)Y_xpm, (xpm_map_t)Z_xpm
};abet XPM files
#include "../assets/alphabet/A.xpm"
#include "../assets/alphabet/B.xpm"
#include "../assets/alphabet/C.xpm"
#include "../assets/alphabet/D.xpm"
#include "../assets/alphabet/E.xpm"
#include "../assets/alphabet/F.xpm"
#include "../assets/alphabet/G.xpm"
#include "../assets/alphabet/H.xpm"
#include "../assets/alphabet/I.xpm"
#include "../assets/alphabet/J.xpm"
#include "../assets/alphabet/K.xpm"
#include "../assets/alphabet/L.xpm"
#include "../assets/alphabet/M.xpm"
#include "../assets/alphabet/N.xpm"
#include "../assets/alphabet/O.xpm"
#include "../assets/alphabet/P.xpm"
#include "../assets/alphabet/Q.xpm"
#include "../assets/alphabet/R.xpm"
#include "../assets/alphabet/S.xpm"
#include "../assets/alphabet/T.xpm"
#include "../assets/alphabet/U.xpm"
#include "../assets/alphabet/V.xpm"
#include "../assets/alphabet/W.xpm"
#include "../assets/alphabet/X.xpm"
#include "../assets/alphabet/Y.xpm"
#include "../assets/alphabet/Z.xpm"

// Include numbers XPM files
#include "../assets/numbers/0.xpm"
#include "../assets/numbers/1.xpm"
#include "../assets/numbers/2.xpm"
#include "../assets/numbers/3.xpm"
#include "../assets/numbers/4.xpm"
#include "../assets/numbers/5.xpm"
#include "../assets/numbers/6.xpm"
#include "../assets/numbers/7.xpm"
#include "../assets/numbers/8.xpm"
#include "../assets/numbers/9.xpm"

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

// Character to XPM mapping arrays
static xpm_map_t uppercase_letters[26] = {
    A_xpm, B_xpm, C_xpm, D_xpm, E_xpm, F_xpm, G_xpm, H_xpm, I_xpm, J_xpm,
    K_xpm, L_xpm, M_xpm, N_xpm, O_xpm, P_xpm, Q_xpm, R_xpm, S_xpm, T_xpm,
    U_xpm, V_xpm, W_xpm, X_xpm, Y_xpm, Z_xpm
};

static xpm_map_t numbers[10] = {
    (xpm_map_t)num_0_xpm, (xpm_map_t)num_1_xpm, (xpm_map_t)num_2_xpm, (xpm_map_t)num_3_xpm, (xpm_map_t)num_4_xpm,
    (xpm_map_t)num_5_xpm, (xpm_map_t)num_6_xpm, (xpm_map_t)num_7_xpm, (xpm_map_t)num_8_xpm, (xpm_map_t)num_9_xpm
};

// Function to get XPM for a character
xpm_map_t get_char_xpm(char c) {
    if (c >= 'A' && c <= 'Z') {
        return uppercase_letters[c - 'A'];
    }
    if (c >= '0' && c <= '9') {
        return numbers[c - '0'];
    }
    return NULL; // Character not supported
}

// Function to display text on screen
void display_text(game_engine_t *engine, const char *text, uint16_t start_x, uint16_t start_y) {
    uint16_t x = start_x;
    uint16_t y = start_y;
    
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        
        if (c == ' ') {
            x += 8; // Space width
            continue;
        }
        
        if (c == '\n') {
            x = start_x;
            y += 12; // Line height
            continue;
        }
        
        xpm_map_t char_xpm = get_char_xpm(c);
        if (char_xpm != NULL) {
            int sprite_id = engine_create_sprite(engine, char_xpm, x, y);
            if (sprite_id >= 0) {
                engine_show_sprite(engine, sprite_id);
            }
            x += 8; // Character width
        }
    }
}

// Player state
static struct {
    uint16_t x;
    uint16_t y;
    uint8_t sprite_id;
    uint16_t speed;
} player;

// Update function (simplified for text testing)
void game_update(game_engine_t *engine, float delta_time) {
    // Nothing to update for text display test
    // You can add ESC key handling to exit if needed
}

// Render function (not needed for this simple example, but added for completeness)
void game_render(game_engine_t *engine) {
    // Nothing to do here - engine will handle rendering the sprite
}

int game_init() {
    // Initialize the game engine
    game_engine_t engine;
    
    // Initialize with 0x115 mode (1024x768) at 60 FPS
    if (engine_init(&engine, 0x115, 60) != 0) {
        printf("Failed to initialize game engine\n");
        return 1;
    }
    
    // Clear the screen to black
    engine_clear_screen(&engine, 0x000000);
    
    // Test alphabet and numbers display
    display_text(&engine, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 50, 50);
    display_text(&engine, "0123456789", 50, 100);
    display_text(&engine, "HELLO WORLD", 50, 150);
    display_text(&engine, "TESTING 123", 50, 200);
    
    // Register update and render callbacks (minimal for testing)
    engine_set_update_callback(&engine, game_update);
    engine_set_render_callback(&engine, game_render);
    
    // Run the game loop
    int result = engine_run(&engine);
    
    // Clean up engine resources
    engine_cleanup(&engine);
    
    return result;
}
