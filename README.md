# 🐍 Snake Adventure

A **cross-platform terminal Snake Game** built in **C++**, playable on **Windows** and **Linux**.  
The game features colored emoji-based visuals, a pause menu, obstacles, and timed power-ups for extra growth and score boosts!

---

## 🎮 Features

✅ Cross-platform support (Windows / Linux)  
✅ Works entirely in the **terminal**  
✅ Pause Menu (`ESC` key):  
   - Continue  
   - Restart Game  
   - View Instructions  
   - Quit to Main Menu  
✅ Dynamic difficulty (Level increases every 5 points)  
✅ Obstacles that block the snake’s path  
✅ Emoji-based visuals for walls, snake, and food  
✅ **Power-Up 🍒**:  
   - Appears every 12 seconds  
   - Stays visible for 8 seconds  
   - Gives **+3 tail segments** and **+3 score**  
✅ Smooth gameplay using non-blocking keyboard input  
✅ UTF-8 safe (works with both emoji and ASCII fallback)

---

## 🧠 Controls

| Key | Action |
|-----|---------|
| `W` / `↑` | Move Up |
| `S` / `↓` | Move Down |
| `A` / `←` | Move Left |
| `D` / `→` | Move Right |
| `ESC` | Pause Menu |
| `Y` / `N` | Play Again / Quit after Game Over |

---

## ⚡ Power-Ups

- The **🍒 power-up** appears randomly every 12 seconds.
- It disappears automatically after **8 seconds**.
- Eating it grows the snake by **3 segments** instead of 1.
- The snake also earns **+3 points**.

---

## 🧱 Game Elements

| Symbol | Meaning |
|:-------:|----------|
| 🧱 | Wall |
| 🟩 | Snake body |
| 🐍 | Snake head |
| 🪶 / 🍎 / 🍏 | Food (changes as score increases) |
| 🍒 | Power-Up (special item) |

---

## 🖥️ How to Run

### ▶️ On Windows

1. Open **PowerShell** or **CMD**.
2. Navigate to your game folder:
   ```bash
   cd path\to\snake_game
Compile the program:
g++ snake.cpp -o snake -std=c++17

Run the game:
.\snake.exe

🐧 On Linux / macOS
Open Terminal.

Navigate to the project folder:
cd path/to/snake_game

Compile:
g++ snake.cpp -o snake -std=c++17

Run:
./snake

🕹️ Pause Menu Options
When pressing ESC, the following menu appears:
1️⃣ Continue – resume the paused game
2️⃣ Restart Game – start a new game from scratch
3️⃣ Instructions – quick how-to guide
4️⃣ Quit to Main Menu – return to the title screen

💀 Game Over Screen
After crashing, the game shows:
Your final score
Option to play again (Y) or quit (N)

🧑‍💻 Project Information
Language: C++17
Libraries Used: <deque>, <chrono>, <thread>, <conio.h> (Windows), <termios.h> (Linux)
Developed for: College Project (DA-IICT)
Contributors:
  Heer Mehta
  Param Patel
  Yash Doshi
