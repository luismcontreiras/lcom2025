# Game Engine Project

A simple 2D game engine built for MINIX using LCOM library.

## Directory Structure

```
proj/
├── doc/               # Documentation
├── src/               # Source code
│   ├── engine/        # Game engine components
│   │   ├── core/      # Core engine functionality
│   │   ├── events/    # Event handling system
│   │   ├── input/     # Input handling (keyboard, mouse)
│   │   ├── rendering/ # Sprite and graphics rendering
│   │   └── utils/     # Utility functions and managers
│   ├── game/          # Game-specific code
│   ├── hardware/      # Hardware abstraction layer
│   └── main.c         # Main entry point
```

## Engine Architecture

The game engine is built with the following components:

1. **Core System**:
   - Engine initialization and main loop
   - Configuration management
   - Game state management

2. **Event System**:
   - Event registration and dispatching
   - Support for keyboard, timer, and quit events

3. **Input Management**:
   - Keyboard input abstraction
   - Key state tracking

4. **Rendering System**:
   - Sprite creation and management
   - Screen management and boundaries

5. **Time Management**:
   - Frame rate control
   - Delta time calculation

## Hardware Abstraction

The engine abstracts hardware details through dedicated modules:
- `graphics.c/h`: Video mode, frame buffer, and drawing functions
- `kbc.c/h`: Keyboard controller operations
- `timer.c/h`: Timer functions for game loop timing

## How to Use

1. Initialize the engine with desired configuration:
```c
EngineConfig config;
config.video_mode = 0x105;  // 1024x768 VGA
config.frame_rate = 60;
Engine engine;
engine_init(&engine, config);
```

2. Register event handlers:
```c
engine_register_handler(&engine, EVENT_KEYBOARD, game_keyboard_handler, &player);
```

3. Add sprites:
```c
int sprite_index = engine_add_sprite(&engine, player_xpm, XPM_8_8_8, position);
```

4. Run the game:
```c
engine_run(&engine);
```

5. Clean up when done:
```c
engine_shutdown(&engine);
```

## Building the Project

Run `make` in the src directory to build the project.
