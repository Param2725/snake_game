# 🐍 Snake Adventure — Console Game

> A **cross-platform Snake Game** built in **C++**, featuring levels, walls, obstacles, colorful emojis, and a smooth real-time input system.  
> Playable on **Windows**, **Linux**, and **macOS** terminals!

---

## 🎮 Features

✅ **Classic Snake Gameplay**  
- Eat 🍎 to grow and increase your score  
- Avoid walls 🧱, obstacles 🌳, and your own tail 🐍  
- Smooth controls with instant response  

✅ **Dynamic Levels & Symbols**  
- Food and wall emojis change as your score rises  
- Level 1 → 🍎 & 🧱  
- Level 2 → 🍏 & 🌳  
- Level 3+ → 🥝 & 🪨  

✅ **Obstacles and Walls**  
- Randomized obstacle positions for every game  
- Solid border walls — collision = Game Over 💥  

✅ **Cross-Platform Controls**

| Key | Action |
|-----|---------|
| ⬆️ / W | Move Up |
| ⬇️ / S | Move Down |
| ⬅️ / A | Move Left |
| ➡️ / D | Move Right |
| Y | Play Again |
| N | Exit Game |

✅ **Menu System**  
1️⃣ Start Game  
2️⃣ View Instructions  
3️⃣ Exit  

✅ **Replay System**  
After losing, press **Y** to restart or **N** to exit.  
Score resets, but fun doesn’t 😄

---

## 🧠 Technical Overview

### 🌀 Core Game Loop
Each frame performs:
- `drawBoard()` → Renders the board  
- `logic()` → Moves snake and detects collision  
- Non-blocking input detection  
- Adjustable frame delay via `sleep_ms()`

### ⚙️ Input Handling (Cross-Platform)
- **Windows:** Uses `_kbhit()` and `_getch()` from `<conio.h>`  
- **Linux/macOS:** Uses `<termios.h>`, `<unistd.h>`, `<fcntl.h>`  

### 🧩 Data Structures

| Type | Usage |
|------|--------|
| `deque<Position>` | Snake body for O(1) add/remove |
| `vector<Position>` | Obstacle coordinates |
| `struct Position` | Represents x, y |
| `enum Direction` | Tracks movement direction (UP, DOWN, LEFT, RIGHT) |

### 🧱 Collision Detection
The game checks for:
- 🧱 Wall hit  
- 🪨 Obstacle hit  
- 🐍 Self-collision  

Any hit ends the game instantly.

### 🌈 Dynamic Emoji Rendering
- Emojis for modern terminals 🎨  
- Fallback to ASCII (`#`, `*`, `O`) for older ones  

---

## 📚 Libraries Used

| Library | Purpose |
|----------|----------|
| `<iostream>` | Input & Output |
| `<deque>` | Efficient snake body |
| `<vector>` | Obstacle management |
| `<cstdlib>` & `<ctime>` | Random food placement |
| `<thread>` & `<chrono>` | Frame timing |
| `<conio.h>` / `<termios.h>` | Keyboard input |
| `<windows.h>` | Windows console handling |

---

## 🧩 How to Run

### 🪟 On Windows
```bash
g++ snake_game.cpp -o snake_game
./snake_game
```
### 💡 If emojis look broken, enable UTF-8:
```bash
Copy code
chcp 65001
```
### 🐧 On Linux/macOS
```bash
Copy code
g++ snake_game.cpp -o snake_game
./snake_game
```

## Future Enhancements
- Add Pause/Resume
- Add Sound Effects
- Save High Scores locally
- Add Themes (Dark / Retro)
- 2-Player mode (Dual snakes)


## Developed By
Param Patel (202512005)  
Heer Mehta(202512095)  
Yash Doshi(202512094)  
  
💙 Built with passion, pixels, and persistence.
