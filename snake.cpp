#include <iostream>
#include <deque>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace std;

struct Position { int x, y; };
enum Direction { UP, DOWN, LEFT, RIGHT };

// cross platform input
#ifdef _WIN32
bool kbhit_custom() { return _kbhit(); }
char getch_custom() { return _getch(); }
void sleep_ms(int ms) { Sleep(ms); }
#else
bool kbhit_custom() {
    termios oldt, newt;
    int ch, oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) { ungetc(ch, stdin); return true; }
    return false;
}
char getch_custom() { return getchar(); }
void sleep_ms(int ms) { this_thread::sleep_for(chrono::milliseconds(ms)); }
#endif

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[H";
#endif
}
void moveCursorToTop() {
#ifdef _WIN32
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    cout << "\033[H";
#endif
}

// ================= CLASS START =======================
class SnakeGame {
private:
    int width = 20, height = 15;
    deque<Position> snake;
    deque<Position> snake2;
    Position food;
    vector<Position> obstacles;
    int score = 0, score2 = 0, level = 1;
    bool gameOver = false;
    bool p1Lost = false;
    bool p2Lost = false;
    bool utf8 = true;
    Direction dir = RIGHT;
    Direction dir2 = LEFT;

    string headSymbol = "🐍", bodySymbol = "🟩", foodSymbol = "🪶", wallSymbol = "🧱";

public:
    SnakeGame() { srand((unsigned)time(0)); }

    void showMenu();
    void showInstructions();
    void setupGame();
    void resetGame();             // fixed reset
    void generateFood();
    void updateLevelAndSymbols();
    void drawBoard();
    void logic();
    void gameLoop();
};

// ================= CLASS METHODS ====================

void SnakeGame::generateFood() {
    bool valid = false;
    while (!valid) {
        valid = true;
        food = {rand() % width, rand() % height};
        for (auto &s : snake)
            if (s.x == food.x && s.y == food.y) valid = false;
        for (auto &s : snake2)
            if (s.x == food.x && s.y == food.y) valid = false;
        for (auto &o : obstacles)
            if (o.x == food.x && o.y == food.y) valid = false;
    }
}

void SnakeGame::drawBoard() {
    moveCursorToTop();
    cout << "Player 1 Score: " << score << "   Player 2 Score: " << score2 << "   Level: " << level << "\n";

    for (int i = 0; i < width + 2; i++) cout << wallSymbol;
    cout << "\n";

    for (int y = 0; y < height; y++) {
        cout << wallSymbol;
        for (int x = 0; x < width; x++) {
            bool printed = false;

            if (!snake.empty() && x == snake.front().x && y == snake.front().y) {
                cout << headSymbol;
                printed = true;
            } else if (!snake2.empty() && x == snake2.front().x && y == snake2.front().y) {
                cout << headSymbol;
                printed = true;
            } else {
                for (size_t i = 1; i < snake.size(); i++) {
                    if (snake[i].x == x && snake[i].y == y) {
                        cout << bodySymbol;
                        printed = true;
                        break;
                    }
                }
                if (!printed) {
                    for (size_t i = 1; i < snake2.size(); i++) {
                        if (snake2[i].x == x && snake2[i].y == y) {
                            cout << bodySymbol;
                            printed = true;
                            break;
                        }
                    }
                }
            }

            if (!printed && x == food.x && y == food.y) { cout << foodSymbol; printed = true; }

            for (auto &o : obstacles)
                if (!printed && o.x == x && o.y == y) { cout << wallSymbol; printed = true; }

            if (!printed) cout << "  ";
        }
        cout << wallSymbol << "\n";
    }

    for (int i = 0; i < width + 2; i++) cout << wallSymbol;
    cout << "\n";
}

void SnakeGame::setupGame() {
    // Initialize basic state for a fresh game
    snake.clear();
    snake2.clear();
    obstacles.clear();
    score = 0;
    score2 = 0;
    level = 1;
    gameOver = false;
    p1Lost = false;
    p2Lost = false;
    dir = RIGHT;
    dir2 = LEFT;

    // initial snake head in center
    snake.push_back({width / 2, height / 2});
    // initial snake 2 head
    snake2.push_back({width / 2, height / 2 + 3});

    // generate food and obstacles after snake exists
    generateFood();

    for (int i = 0; i < 5; i++) {
        Position o;
        do {
            o = {rand() % (width - 2) + 1, rand() % (height - 2) + 1};
        } while ((o.x == food.x && o.y == food.y) || 
                 (o.x == width / 2 && o.y == height / 2) || 
                 (o.x == width / 2 && o.y == height / 2 + 3));
        obstacles.push_back(o);
    }

    // ensure symbols reflect utf8 mode
    if (utf8) {
        headSymbol = "🐍"; bodySymbol = "🟩"; foodSymbol = "🪶"; wallSymbol = "🧱";
    } else {
        headSymbol = "@"; bodySymbol = "o"; foodSymbol = "*"; wallSymbol = "#";
    }
}

// ---------- FIX: resetGame now completely resets all game state ----------
void SnakeGame::resetGame() {
    // Reset flags and scoring
    score = 0;
    score2 = 0;
    level = 1;
    gameOver = false;
    p1Lost = false;
    p2Lost = false;
    dir = RIGHT;
    dir2 = LEFT;

    // Reset display mode and symbols to defaults (utf8 kept as-is; change if you want)
    utf8 = true;
    headSymbol = "🐍";
    bodySymbol = "🟩";
    foodSymbol = "🪶";
    wallSymbol = "🧱";

    // Clear and reinitialize snake & obstacles & food
    snake.clear();
    snake2.clear();
    obstacles.clear();
    snake.push_back({width / 2, height / 2});
    snake2.push_back({width / 2, height / 2 + 3});

    // Regenerate food and obstacles safely
    generateFood();
    for (int i = 0; i < 5; i++) {
        Position o;
        do {
            o = {rand() % (width - 2) + 1, rand() % (height - 2) + 1};
        } while ((o.x == food.x && o.y == food.y) || 
                 (o.x == width / 2 && o.y == height / 2) || 
                 (o.x == width / 2 && o.y == height / 2 + 3));
        obstacles.push_back(o);
    }
}
// -------------------------------------------------------------------------

void SnakeGame::updateLevelAndSymbols() {
    int maxScore = (score > score2) ? score : score2;
    level = maxScore / 5 + 1;
    if (utf8) {
        if (maxScore >= 10) foodSymbol = "🍏";
        else if (maxScore >= 5) foodSymbol = "🍎";
        else foodSymbol = "🪶";

        if (maxScore >= 10) wallSymbol = "🪨";
        else if (maxScore >= 5) wallSymbol = "🌳";
        else wallSymbol = "🧱";

        headSymbol = "🐍";
        bodySymbol = "🟩";
    } else {
        foodSymbol = "*";
        wallSymbol = "#";
        headSymbol = "@";
        bodySymbol = "o";
    }
}

void SnakeGame::logic() {
    if (snake.empty() || snake2.empty()) return; // safety

    Position newHead1 = snake.front();
    if (dir == UP) newHead1.y--;
    else if (dir == DOWN) newHead1.y++;
    else if (dir == LEFT) newHead1.x--;
    else if (dir == RIGHT) newHead1.x++;

    Position newHead2 = snake2.front();
    if (dir2 == UP) newHead2.y--;
    else if (dir2 == DOWN) newHead2.y++;
    else if (dir2 == LEFT) newHead2.x--;
    else if (dir2 == RIGHT) newHead2.x++;

    // Collision checks for Player 1
    if (newHead1.x < 0 || newHead1.x >= width || newHead1.y < 0 || newHead1.y >= height) {
        gameOver = true;
        p1Lost = true;
    }
    for (auto &o : obstacles) {
        if (newHead1.x == o.x && newHead1.y == o.y) {
            gameOver = true;
            p1Lost = true;
        }
    }
    for (auto &s : snake) {
        if (newHead1.x == s.x && newHead1.y == s.y) {
            gameOver = true;
            p1Lost = true;
        }
    }
    for (auto &s : snake2) {
        if (newHead1.x == s.x && newHead1.y == s.y) {
            gameOver = true;
            p1Lost = true;
        }
    }

    // Collision checks for Player 2
    if (newHead2.x < 0 || newHead2.x >= width || newHead2.y < 0 || newHead2.y >= height) {
        gameOver = true;
        p2Lost = true;
    }
    for (auto &o : obstacles) {
        if (newHead2.x == o.x && newHead2.y == o.y) {
            gameOver = true;
            p2Lost = true;
        }
    }
    for (auto &s : snake2) {
        if (newHead2.x == s.x && newHead2.y == s.y) {
            gameOver = true;
            p2Lost = true;
        }
    }
    for (auto &s : snake) {
        if (newHead2.x == s.x && newHead2.y == s.y) {
            gameOver = true;
            p2Lost = true;
        }
    }

    // Head-on collision check
    if (newHead1.x == newHead2.x && newHead1.y == newHead2.y) {
        gameOver = true;
        p1Lost = true;
        p2Lost = true;
    }

    snake.push_front(newHead1);
    snake2.push_front(newHead2);

    bool p1Ate = (newHead1.x == food.x && newHead1.y == food.y);
    bool p2Ate = (newHead2.x == food.x && newHead2.y == food.y);

    if (p1Ate) {
        score++;
    } else {
        snake.pop_back();
    }

    if (p2Ate) {
        score2++;
    } else {
        snake2.pop_back();
    }

    if (p1Ate || p2Ate) {
        updateLevelAndSymbols();
        generateFood();
    }
}

void SnakeGame::showMenu() {
    clearScreen();
    cout << "==============================\n";
    cout << "     🐍 SNAKE ADVENTURE 🐍    \n";
    cout << "==============================\n\n";
    cout << "1️⃣  Start Game\n";
    cout << "2️⃣  Instructions\n";
    cout << "3️⃣  Exit\n\n";
    cout << "Select an option: ";
}

void SnakeGame::showInstructions() {
    clearScreen();
    cout << "=== Instructions ===\n";
    cout << "Use Arrow Keys or W A S D to move\n";
    cout << "Eat food to grow\n";
    cout << "Avoid walls and obstacles!\n\n";
    cout << "Press any key to return...";
    getch_custom();
}

void SnakeGame::gameLoop() {
    // Ensure we start from a valid setup if somehow not initialized
    if (snake.empty()) setupGame();

    while (!gameOver) {
        drawBoard();
        sleep_ms(200);
        if (kbhit_custom()) {
            char key = getch_custom();
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
        }
        logic();
    }

    clearScreen();
    cout << "💀 Game Over! 💀\n";
    if (p1Lost && p2Lost) {
        cout << "Both Players Lost!\n";
    } else if (p1Lost) {
        cout << "Player 1 Lost!\n";
    } else if (p2Lost) {
        cout << "Player 2 Lost!\n";
    }
    cout << "Player 1 Score: " << score << "\n";
    cout << "Player 2 Score: " << score2 << "\n\n";
    cout << "Play again? (y/n): ";
    char c; cin >> c;
    if (c == 'y' || c == 'Y') {
        // Use resetGame() which now clears everything properly
        resetGame();
        gameLoop();
    }
}

// ==================== MAIN ============================
int main() {
#ifdef _WIN32
    system("chcp 65001 >nul");
    SetConsoleOutputCP(CP_UTF8);
#endif

    SnakeGame game;
    int choice;
    do {
        game.showMenu();
        cin >> choice;
        if (choice == 1) { game.setupGame(); game.gameLoop(); }
        else if (choice == 2) { game.showInstructions(); }
        else if (choice == 3) { cout << "Exiting game... Goodbye!\n"; }
        else { cout << "Invalid choice.\n"; }
    } while (choice != 3);

    return 0;
}
