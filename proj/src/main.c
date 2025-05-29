#include <lcom/lcf.h>
#include "engine/game_engine.h"
#include "game/game.h"

int (proj_main_loop)(int argc, char *argv[]){
    if(game_init()!= 0 ){
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    // sets the language of LCF messages
    lcf_set_language("EN-US");

    // enables to log function invocations
    lcf_trace_calls("/home/lcom/labs/game_engine_project/trace.txt");

    // enables to save printf output
    lcf_log_output("/home/lcom/labs/game_engine_project/output.txt");

    lcf_start(argc, argv);

    // Run the game
    proj_main_loop(argc, argv);

    // LCF cleanup
    lcf_cleanup();
    return 0;
}