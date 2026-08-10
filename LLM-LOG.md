# LLM Log

## 1. Setup

| Parameter | Description |
| :--- | :--- |
| **Model(s)** | Gemini 3.5 Flash (Medium) |
| **Tool / harness** | Antigravity IDE Agent |
| **IDE / editor** | VS Code / Antigravity |
| **Did you paste this assignment document into it?** | Yes, the user pasted the assignment document as part of the initial prompt. |

---

## 2. Session Log

### Prompts and Replies

#### Prompt 1
```
Implement exactly this, and nothing more:

A second snake, controlled by W A S D. Player 1 keeps whatever controls the game already uses.
Both snakes eat the same fruit. Whoever reaches it first grows.
Each player has their own score, both visible during play.
The game ends when either snake hits a wall, itself, or the other snake. Announce which player lost.
Do not do anything else. No new menus, no colours, no renaming, no tidying up code you dislike, no reformatting. Every extra edit makes your measurement incomparable to the other 41 groups, and the comparison is the assignment.

If you think the code needs restructuring before a second snake is possible, that is a finding — write it in Part D. Do not act on it.
```

#### Reply 1 (Key Code Output)
The LLM generated an implementation plan proposing the following changes:
- Declared class fields: `snake2`, `dir2`, `score2`, `p1Lost`, `p2Lost`.
- Initialized/reset Player 2's snake/score/lost states in `setupGame()` and `resetGame()`.
- Updated `generateFood()` to prevent food spawning on Player 2.
- Updated `drawBoard()` to draw Player 2's score and snake.
- Updated `gameLoop()` to read W A S D keys for Player 2 (while keeping Player 1 controls).
- Updated `logic()` to compute movement and detect collisions (wall, self, other snake, head-on) for both snakes.

---

#### Prompt 2
```
Does it change features which existed before 
Like the ones which are not necessary to change to get 2nd player working 
```

#### Reply 2 (Key Code Output)
No code was produced. Explained that the plan makes the absolute minimum additions to implement the second snake and preserves all existing features.

---

#### Prompt 3
```
Ohkk then I think we can work with plan 
```

#### Reply 3 (Key Code Output)
The LLM applied the edits to `snake.cpp` to initialize the second snake, draw both snakes, handle food eating, detect collisions, and announce who lost.

---

#### Prompt 4
```
I think we need to disable player 1 with w a s d for 2 player game 
it is causing an issue in that by same keys are affecting both the snake
```

#### Reply 4 (Key Code Output)
Modified input handling in `gameLoop()` to restrict Player 1 to arrow keys:
```cpp
            // Player 1 controls (Arrow keys only)
            if ((key == 72 && dir != DOWN) || (key == 80 && dir != UP) ||
                (key == 75 && dir != RIGHT) || (key == 77 && dir != LEFT)) {
                if (key == 72) dir = UP;
                else if (key == 80) dir = DOWN;
                else if (key == 75) dir = LEFT;
                else if (key == 77) dir = RIGHT;
            }
            // Player 2 controls (W A S D)
            if ((key == 'w' && dir2 != DOWN) || (key == 's' && dir2 != UP) ||
                (key == 'a' && dir2 != RIGHT) || (key == 'd' && dir2 != LEFT)) {
                if (key == 'w') dir2 = UP;
                else if (key == 's') dir2 = DOWN;
                else if (key == 'a') dir2 = LEFT;
                else if (key == 'd') dir2 = RIGHT;
            }
```

---

### Session Analysis & Summary

* **Which attempt finally worked:** Attempt 4 worked perfectly. Attempt 3 correctly implemented all features of the original assignment, but Player 1 was still affected by W A S D keys which caused overlapping movements when Player 2 tried to steer. Disabling W A S D for Player 1 in Attempt 4 fixed this issue.
* **Total number of prompts:** 4 prompts.
* **What code was supplied:** The whole file `snake.cpp`.
* **How files were found:** Listed the repository's root directory which contained `snake.cpp` and `README.md`.
