# Game Engine Architecture

## Overview

This game engine is designed with a clear separation between hardware access, engine components, and game logic. It is inspired by the `video_move_xpm` function and expands it into a modular, reusable architecture.

## Core Components

### 1. Hardware Layer

The hardware layer provides direct access to MINIX system components:

- **Graphics Module**: Video mode setting, frame buffer management, pixel drawing
- **Keyboard Module**: Keyboard interrupt handling, scancode processing
- **Timer Module**: System timer control for game loop timing

### 2. Engine Layer

The engine layer abstracts hardware details and provides high-level game development functionality:

#### 2.1 Core System
- **Engine**: Main initialization, configuration, and game loop
- **Types**: Common data structures like vectors, rectangles, and events

#### 2.2 Event System
- **Event Management**: Registration and dispatch of events
- **Event Types**: Keyboard, timer, mouse, and custom events

#### 2.3 Input System
- **Input Manager**: Abstraction for keyboard and mouse input
- **Input State**: Tracking pressed keys and mouse position

#### 2.4 Rendering System
- **Sprite**: Visual game objects with position and appearance
- **Sprite Manager**: Management of multiple sprites, rendering order

#### 2.5 Utility System
- **Time Manager**: Frame rate control, delta time calculation
- **Math Utilities**: Vector operations, collision detection

### 3. Game Layer

The game layer contains game-specific logic and components:

- **Game State**: Main game data structure
- **Game Objects**: Player, enemies, items
- **Game Logic**: Rules, scoring, level management
- **Event Handlers**: Game-specific input processing

## Flow of Execution

1. **Initialization**:
   - Hardware components are initialized (timer, keyboard, graphics)
   - Engine subsystems are initialized
   - Game data is loaded

2. **Main Loop**:
   - Wait for hardware events (interrupts)
   - Process input events
   - Update game state
   - Render graphics
   - Maintain frame rate

3. **Shutdown**:
   - Clean up game resources
   - Unsubscribe from interrupts
   - Restore system state

## Event-Driven Architecture

The engine uses an event-driven approach where:

1. Hardware interrupts trigger event creation
2. Events are dispatched to registered handlers
3. Handlers update game state
4. Rendering reflects the updated state

This provides a clean separation between hardware details and game logic.
