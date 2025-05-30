---
applyTo: '**'
---
import pygame
import sys
import numpy as np
import random
from enum import Enum

# Define constants
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
GRID_SIZE = 10
GRID_WIDTH = SCREEN_WIDTH // GRID_SIZE
GRID_HEIGHT = SCREEN_HEIGHT // GRID_SIZE
FPS = 15

# Define colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
YELLOW = (255, 255, 0)

# Define directions
class Direction(Enum):
    UP = (0, -1)
    DOWN = (0, 1)
    LEFT = (-1, 0)
    RIGHT = (1, 0)

class Player:
    def __init__(self, x, y, color, is_ai=False):
        self.positions = [(x, y)]
        self.color = color
        self.direction = Direction.RIGHT if x < GRID_WIDTH // 2 else Direction.LEFT
        self.is_alive = True
        self.is_ai = is_ai
    
    def move(self):
        if not self.is_alive:
            return
        
        head_x, head_y = self.positions[-1]
        dx, dy = self.direction.value
        new_x, new_y = head_x + dx, head_y + dy
        
        if (new_x < 0 or new_x >= GRID_WIDTH or 
            new_y < 0 or new_y >= GRID_HEIGHT):
            self.is_alive = False
            return
        
        self.positions.append((new_x, new_y))
    
    def get_head_position(self):
        return self.positions[-1] if self.positions else None

    def change_direction(self, new_direction):
        # Prevent moving in the opposite direction
        if ((new_direction == Direction.UP and self.direction == Direction.DOWN) or
            (new_direction == Direction.DOWN and self.direction == Direction.UP) or
            (new_direction == Direction.LEFT and self.direction == Direction.RIGHT) or
            (new_direction == Direction.RIGHT and self.direction == Direction.LEFT)):
            return
        self.direction = new_direction

class AIBot(Player):
    def __init__(self, x, y, color):
        super().__init__(x, y, color, is_ai=True)
        
    def choose_direction(self, board):
        # Get current position
        head_x, head_y = self.get_head_position()
        
        # Check all possible moves and choose the safest one
        possible_moves = []
        for direction in Direction:
            dx, dy = direction.value
            new_x, new_y = head_x + dx, head_y + dy
            
            # Don't move in the opposite direction
            if ((direction == Direction.UP and self.direction == Direction.DOWN) or
                (direction == Direction.DOWN and self.direction == Direction.UP) or
                (direction == Direction.LEFT and self.direction == Direction.RIGHT) or
                (direction == Direction.RIGHT and self.direction == Direction.LEFT)):
                continue
            
            # Check if the move is valid (not hitting walls or existing trails)
            if (0 <= new_x < GRID_WIDTH and 
                0 <= new_y < GRID_HEIGHT and 
                board[new_y][new_x] == 0):
                # Calculate a score for this move (higher is better)
                # This simple AI looks ahead 5 steps and prefers moves with more open space
                score = self.look_ahead(board, new_x, new_y, 5)
                possible_moves.append((direction, score))
        
        # Choose the move with highest score, or stay on the current path if no good moves
        if possible_moves:
            possible_moves.sort(key=lambda x: x[1], reverse=True)
            self.direction = possible_moves[0][0]
        
    def look_ahead(self, board, x, y, depth):
        """Recursively look ahead to find the most open path"""
        if depth == 0:
            return 0
            
        # Count open spaces in all four directions
        open_spaces = 0
        for direction in Direction:
            dx, dy = direction.value
            new_x, new_y = x + dx, y + dy
            
            if (0 <= new_x < GRID_WIDTH and 
                0 <= new_y < GRID_HEIGHT and 
                board[new_y][new_x] == 0):
                open_spaces += 1
                
                # Create a copy of the board and mark this position as taken
                new_board = board.copy()
                new_board[y][x] = 1
                
                # Recursively look ahead from this new position with reduced depth
                open_spaces += self.look_ahead(new_board, new_x, new_y, depth - 1) * 0.5
                
        return open_spaces

class TronGame:
    def __init__(self):
        pygame.init()
        pygame.display.set_caption('Tron Light Cycles')
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont('Arial', 30)
        self.reset_game()
        
    def reset_game(self):
        # Create game board (0 = empty, 1 = player trail, 2 = AI trail)
        self.board = np.zeros((GRID_HEIGHT, GRID_WIDTH), dtype=int)
        
        # Create player and AI
        self.player = Player(GRID_WIDTH // 4, GRID_HEIGHT // 2, BLUE)
        self.ai = AIBot(3 * GRID_WIDTH // 4, GRID_HEIGHT // 2, RED)
        
        # Mark initial positions
        self.board[self.player.get_head_position()[1]][self.player.get_head_position()[0]] = 1
        self.board[self.ai.get_head_position()[1]][self.ai.get_head_position()[0]] = 2
        
        self.game_over = False
        
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_UP:
                    self.player.change_direction(Direction.UP)
                elif event.key == pygame.K_DOWN:
                    self.player.change_direction(Direction.DOWN)
                elif event.key == pygame.K_LEFT:
                    self.player.change_direction(Direction.LEFT)
                elif event.key == pygame.K_RIGHT:
                    self.player.change_direction(Direction.RIGHT)
                elif event.key == pygame.K_r and self.game_over:
                    self.reset_game()
                elif event.key == pygame.K_q:
                    pygame.quit()
                    sys.exit()
                    
    def update(self):
        if self.game_over:
            return
            
        # AI chooses direction
        self.ai.choose_direction(self.board)
        
        # Move players
        self.player.move()
        self.ai.move()
        
        # Check collisions
        self.check_collisions()
        
        # Update board with new positions
        if self.player.is_alive and self.player.get_head_position():
            x, y = self.player.get_head_position()
            if 0 <= x < GRID_WIDTH and 0 <= y < GRID_HEIGHT:
                if self.board[y][x] != 0:  # Hit something
                    self.player.is_alive = False
                else:
                    self.board[y][x] = 1
                    
        if self.ai.is_alive and self.ai.get_head_position():
            x, y = self.ai.get_head_position()
            if 0 <= x < GRID_WIDTH and 0 <= y < GRID_HEIGHT:
                if self.board[y][x] != 0:  # Hit something
                    self.ai.is_alive = False
                else:
                    self.board[y][x] = 2
        
        # Check if game is over
        if not self.player.is_alive or not self.ai.is_alive:
            self.game_over = True
            
    def check_collisions(self):
        # Check if player hits AI or vice versa
        if self.player.is_alive and self.player.get_head_position() in self.ai.positions[:-1]:
            self.player.is_alive = False
            
        if self.ai.is_alive and self.ai.get_head_position() in self.player.positions[:-1]:
            self.ai.is_alive = False
            
        # Check if player hits itself
        if self.player.is_alive and self.player.get_head_position() in self.player.positions[:-1]:
            self.player.is_alive = False
            
        # Check if AI hits itself
        if self.ai.is_alive and self.ai.get_head_position() in self.ai.positions[:-1]:
            self.ai.is_alive = False
            
    def render(self):
        self.screen.fill(BLACK)
        
        # Draw grid lines
        for x in range(0, SCREEN_WIDTH, GRID_SIZE):
            pygame.draw.line(self.screen, (20, 20, 20), (x, 0), (x, SCREEN_HEIGHT))
        for y in range(0, SCREEN_HEIGHT, GRID_SIZE):
            pygame.draw.line(self.screen, (20, 20, 20), (0, y), (SCREEN_WIDTH, y))
        
        # Draw player trail
        for pos in self.player.positions:
            x, y = pos
            pygame.draw.rect(self.screen, self.player.color, 
                            (x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE))
        
        # Draw AI trail
        for pos in self.ai.positions:
            x, y = pos
            pygame.draw.rect(self.screen, self.ai.color, 
                            (x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE))
        
        # Draw game over message
        if self.game_over:
            if not self.player.is_alive and not self.ai.is_alive:
                text = self.font.render('Draw! Press R to restart', True, WHITE)
            elif not self.player.is_alive:
                text = self.font.render('AI Wins! Press R to restart', True, RED)
            else:
                text = self.font.render('You Win! Press R to restart', True, GREEN)
            
            text_rect = text.get_rect(center=(SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2))
            self.screen.blit(text, text_rect)
        
        pygame.display.flip()
        
    def run(self):
        while True:
            self.handle_events()
            self.update()
            self.render()
            self.clock.tick(FPS)

if __name__ == "__main__":
    game = TronGame()
    game.run()