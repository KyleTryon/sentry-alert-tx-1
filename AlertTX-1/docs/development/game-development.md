# Game Development Guide

Learn how to create games for the AlertTX-1 platform using the GameScreen framework and optimized rendering techniques.

## 🎮 Game Architecture

### GameScreen Base Class
All games inherit from `GameScreen`, which extends the standard `Screen` class with game-specific features:

```cpp
class GameScreen : public Screen {
protected:
    // Frame rate control
    unsigned long lastUpdateMs = 0;
    unsigned long targetFrameTime = 16;  // 60 FPS
    
    // Game area boundaries
    int gameLeft, gameRight, gameTop, gameBottom;
    
public:
    // Override these in your game
    virtual void updateGame() = 0;    // Game logic
    virtual void drawGame() = 0;      // Rendering
    virtual void drawStatic() = 0;    // Static elements
    
    // Helper methods
    void setTargetFPS(int fps);
    bool shouldUpdateFrame();
};
```

### StandardGameLayout
Provides consistent layout across all games:

```cpp
class StandardGameLayout {
public:
    // Standard dimensions
    static const int HEADER_HEIGHT = 29;
    static const int PLAY_AREA_TOP = 30;
    static const int PLAY_AREA_LEFT = 6;
    static const int PLAY_AREA_RIGHT = 234;
    static const int PLAY_AREA_BOTTOM = 125;
    
    // Helper methods
    static void drawGameHeader(display, title, score);
    static void drawPlayAreaBorder(display, color);
    static void clearPlayArea(display, bgColor);
};
```

## 🏗️ Creating a Game

### 1. Basic Game Structure

```cpp
// MyGame.h
#pragma once
#include "ui/core/GameScreen.h"
#include "ui/core/StandardGameLayout.h"

class MyGame : public GameScreen {
private:
    // Game state
    int score = 0;
    bool gameOver = false;
    
    // Game objects
    int playerX, playerY;
    int enemyX, enemyY;
    
public:
    MyGame(Adafruit_ST7789* display);
    
    void enter() override;
    void updateGame() override;
    void drawGame() override;
    void drawStatic() override;
    void handleButtonPress(int button) override;
    
protected:
    void cleanup() override;
};
```

### 2. Implementation Example

```cpp
// MyGame.cpp
void MyGame::enter() {
    GameScreen::enter();
    
    // Initialize game state
    score = 0;
    gameOver = false;
    playerX = StandardGameLayout::PLAY_AREA_LEFT + 10;
    playerY = StandardGameLayout::PLAY_AREA_TOP + 10;
    
    // Set frame rate
    setTargetFPS(60);
    
    // Draw static elements once
    drawStatic();
}

void MyGame::updateGame() {
    if (gameOver) return;
    
    // Update game logic
    // Move enemies, check collisions, update score
    
    // Example: Simple movement
    enemyX += 2;
    if (enemyX > StandardGameLayout::PLAY_AREA_RIGHT) {
        enemyX = StandardGameLayout::PLAY_AREA_LEFT;
    }
}

void MyGame::drawGame() {
    // Clear only dynamic areas
    display->fillRect(prevPlayerX, prevPlayerY, 10, 10, 
                     ThemeManager::getBackground());
    
    // Draw game objects
    display->fillRect(playerX, playerY, 10, 10, 
                     ThemeManager::getPrimary());
    display->fillRect(enemyX, enemyY, 10, 10, 
                     ThemeManager::getAccent());
    
    // Update score in header
    StandardGameLayout::drawGameHeader(display, "MyGame", score);
}

void MyGame::drawStatic() {
    // Draw play area border
    StandardGameLayout::drawPlayAreaBorder(display, 
                                         ThemeManager::getBorder());
    
    // Draw any other static elements
}

void MyGame::handleButtonPress(int button) {
    switch (button) {
        case BUTTON_A:  // Up
            if (playerY > StandardGameLayout::PLAY_AREA_TOP) {
                playerY -= 5;
            }
            break;
        case BUTTON_B:  // Down
            if (playerY < StandardGameLayout::PLAY_AREA_BOTTOM - 10) {
                playerY += 5;
            }
            break;
        case BUTTON_C:  // Action
            // Fire, jump, etc.
            break;
    }
}
```

## 🎯 Performance Optimization

### Dirty Region Tracking
Track what needs redrawing to minimize screen updates:

```cpp
class OptimizedGame : public GameScreen {
private:
    // Track previous positions
    int prevBallX, prevBallY;
    bool ballMoved = false;
    
    void updateGame() override {
        // Store previous position
        prevBallX = ballX;
        prevBallY = ballY;
        
        // Update position
        ballX += velocityX;
        ballY += velocityY;
        
        // Track if moved
        ballMoved = (ballX != prevBallX || ballY != prevBallY);
    }
    
    void drawGame() override {
        if (ballMoved) {
            // Clear previous position
            display->fillRect(prevBallX, prevBallY, ballSize, ballSize,
                            ThemeManager::getBackground());
            
            // Draw new position
            display->fillRect(ballX, ballY, ballSize, ballSize,
                            ThemeManager::getAccent());
        }
    }
};
```

### GameObject Framework
Use the GameObject class for automatic position tracking:

```cpp
class GameObject {
protected:
    int x, y, prevX, prevY;
    int width, height;
    uint16_t color;
    bool moved = false;
    
public:
    void setPosition(int newX, int newY) {
        prevX = x; prevY = y;
        x = newX; y = newY;
        moved = (prevX != x || prevY != y);
    }
    
    void clearPrevious(Adafruit_ST7789* display, uint16_t bgColor) {
        if (moved) {
            display->fillRect(prevX, prevY, width, height, bgColor);
        }
    }
    
    void draw(Adafruit_ST7789* display) {
        display->fillRect(x, y, width, height, color);
        moved = false;
    }
};
```

## 🎨 Visual Design

### Using Themes
Always use the theme system for colors:

```cpp
// ✅ Good - respects user theme
display->fillRect(x, y, w, h, ThemeManager::getPrimary());

// ❌ Bad - hardcoded color
display->fillRect(x, y, w, h, ST77XX_GREEN);
```

### Standard UI Elements
- **Header**: Title and score at top
- **Play Area**: Main game area with border
- **Game Over**: Centered text with stats

```cpp
void drawGameOver() {
    // Draw semi-transparent overlay
    display->fillRect(0, 30, 240, 95, ThemeManager::getBackground());
    
    // Draw game over text
    DisplayUtils::centerText(display, "GAME OVER", 60, 3);
    
    // Draw stats
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    DisplayUtils::centerText(display, scoreText, 90, 2);
}
```

## 🎵 Audio Integration

### Playing Sound Effects
```cpp
#include "hardware/Buzzer.h"

void playHitSound() {
    Buzzer::tone(1000, 50);  // 1kHz for 50ms
}

void playGameOverSound() {
    // Descending tones
    Buzzer::tone(800, 100);
    delay(100);
    Buzzer::tone(600, 100);
    delay(100);
    Buzzer::tone(400, 200);
}
```

### Background Music
For rhythm games or games with music:

```cpp
#include <anyrtttl.h>

void startBackgroundMusic() {
    const char* music = "Theme:d=4,o=5,b=100:c,e,g,c6,g,e,c";
    anyrtttl::nonblocking::begin(BUZZER_PIN, music);
}

void updateGame() override {
    // Update music
    anyrtttl::nonblocking::play();
    
    // Rest of game logic
}
```

## 📊 Game Examples

### Pong - Optimal Performance
- Tracks previous positions for all objects
- Only redraws changed regions
- 60 FPS with minimal flicker

### Snake - Grid-Based Optimization
- Uses grid system for efficient updates
- Only redraws head, tail, and food
- Smooth gameplay at variable speeds

### BeeperHero - Lane-Based Rendering
- Divides screen into 3 lanes
- Tracks dirty regions per lane
- Handles many moving notes efficiently

## 🎯 Best Practices

### 1. Memory Management
```cpp
void cleanup() override {
    // Stop any audio
    anyrtttl::nonblocking::stop();
    
    // Clear any dynamic allocations
    if (gameData) {
        delete[] gameData;
        gameData = nullptr;
    }
    
    // Log cleanup
    Serial.println("Game cleaned up");
}
```

### 2. Frame Rate Control
```cpp
void update() override {
    if (!shouldUpdateFrame()) return;
    
    updateGame();
    // Frame time is automatically managed
}
```

### 3. Input Handling
```cpp
void handleButtonPress(int button) override {
    if (gameOver) {
        if (button == BUTTON_C) {
            restartGame();
        }
        return;
    }
    
    // Normal game input
    switch (button) {
        case BUTTON_A: moveUp(); break;
        case BUTTON_B: moveDown(); break;
        case BUTTON_C: action(); break;
    }
}
```

## 🐛 Common Issues

### Screen Flicker
- Use dirty region tracking
- Avoid `markForFullRedraw()` in game loop
- Only clear what changes

### Poor Performance
- Check frame rate limiting
- Minimize draw calls
- Use GameObject framework

### Memory Leaks
- Implement `cleanup()` method
- Delete allocated objects
- Clear game state properly

## 📚 Advanced Topics

### Collision Detection
```cpp
bool checkCollision(GameObject& a, GameObject& b) {
    return (a.x < b.x + b.width &&
            a.x + a.width > b.x &&
            a.y < b.y + b.height &&
            a.y + a.height > b.y);
}
```

### Particle Effects
```cpp
class Particle : public GameObject {
    float velocityX, velocityY;
    int lifetime;
    
    void update() {
        x += velocityX;
        y += velocityY;
        lifetime--;
    }
    
    bool isAlive() { return lifetime > 0; }
};
```

### High Scores
```cpp
#include "config/SettingsManager.h"

void saveHighScore(int score) {
    int highScore = SettingsManager::getHighScore("MyGame");
    if (score > highScore) {
        SettingsManager::setHighScore("MyGame", score);
    }
}
```

## 🔗 Resources

- [UI Framework](ui-framework.md) - Component system
- [Performance Guide](performance-optimization.md) - Optimization techniques
- [Example Games](https://github.com/techsquidtv/sentry_alert_tx-1/tree/main/AlertTX-1/src/ui/games) - Source code

---

**Happy Game Development!** 🎮
