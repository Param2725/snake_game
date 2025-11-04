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
using namespace std::chrono;

// === Structs ===
struct Position {
    int x, y;
};

// === Enum for Direction ===
enum Direction { UP, DOWN, LEFT, RIGHT };

// === Cross Platform Input Handling ===
#ifdef _WIN32
bool kbhit_custom() { return _kbhit(); }
char getch_custom() { return _getch(); }
void sleep_ms(int ms) { Sleep(ms); }
#else
bool kbhit_custom() {
    termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    return false;
}
char getch_custom() { return getchar(); }
void sleep_ms(int ms) { this_thread::sleep_for(chrono::milliseconds(ms)); }
#endif

// === Global Variables ===
int width = 20, height = 15;
deque<Position> snake;
Position food;
vector<Position> obstacles;
int score = 0, level = 1;
bool gameOver = false;
bool utf8 = true;
Direction dir = RIGHT; 

// === New Power-Up Variables ===
Position powerUp;
bool powerUpActive = false;
steady_clock::time_point powerUpStart;
steady_clock::time_point lastPowerUpSpawn;
const int powerUpDuration = 8000; // 8 seconds
const int powerUpInterval = 12000; // appear every ~12 sec

// === Symbols ===
string headSymbol = "🐍", bodySymbol = "🟩", foodSymbol = "🪶", wallSymbol = "🧱", powerUpSymbol = "🍒";

// === Utility Functions ===
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

// === Generate Food ===
void generateFood() {
    bool valid = false;
    while (!valid) {
        valid = true;
        food = {rand() % width, rand() % height};
        for (auto &s : snake)
            if (s.x == food.x && s.y == food.y)
                valid = false;
        for (auto &o : obstacles)
            if (o.x == food.x && o.y == food.y)
                valid = false;
    }
}

// === Generate Power-Up ===
void generatePowerUp() {
    bool valid = false;
    while (!valid) {
        valid = true;
        powerUp = {rand() % width, rand() % height};
        for (auto &s : snake)
            if (s.x == powerUp.x && s.y == powerUp.y)
                valid = false;
        for (auto &o : obstacles)
            if (o.x == powerUp.x && o.y == powerUp.y)
                valid = false;
        if (food.x == powerUp.x && food.y == powerUp.y)
            valid = false;
    }
    powerUpActive = true;
    powerUpStart = steady_clock::now();
}

// === Draw Game Board ===
void drawBoard() {
    moveCursorToTop();
    cout << "Score: " << score << "   Level: " << level << "\n";
    cout << "Press ESC to Pause\n";

    for (int i = 0; i < width + 2; i++) cout << wallSymbol;
    cout << "\n";

    for (int y = 0; y < height; y++) {
        cout << wallSymbol;
        for (int x = 0; x < width; x++) {
            bool printed = false;

            if (x == snake.front().x && y == snake.front().y) {
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
            }

            if (!printed && x == food.x && y == food.y) {
                cout << foodSymbol;
                printed = true;
            }

            // Draw power-up if active
            if (!printed && powerUpActive && x == powerUp.x && y == powerUp.y) {
                cout << powerUpSymbol;
                printed = true;
            }

            for (auto &o : obstacles)
                if (!printed && o.x == x && o.y == y) {
                    cout << wallSymbol;
                    printed = true;
                }

            if (!printed) cout << "  ";
        }
        cout << wallSymbol << "\n";
    }

    for (int i = 0; i < width + 2; i++) cout << wallSymbol;
    cout << "\n";
}

// === Setup Game ===
void setupGame() {
    snake.clear();
    obstacles.clear();
    snake.push_back({width / 2, height / 2});
    score = 0;
    level = 1;
    gameOver = false;
    dir = RIGHT;

    generateFood();
    lastPowerUpSpawn = steady_clock::now();

    for (int i = 0; i < 5; i++) {
        Position o;
        do {
            o = {rand() % (width - 2) + 1, rand() % (height - 2) + 1};
        } while ((o.x == food.x && o.y == food.y) || (o.x == width / 2 && o.y == height / 2));
        obstacles.push_back(o);
    }
}

// === Reset Game ===
void resetGame() {
    snake.clear();
    obstacles.clear();
    snake.push_back({width / 2, height / 2});
    score = 0;
    level = 1;
    gameOver = false;
    utf8 = true;
    dir = RIGHT;

    headSymbol = "🐍";
    bodySymbol = "🟩";
    foodSymbol = "🪶";
    wallSymbol = "🧱";
    powerUpSymbol = "🍒";

    food = {rand() % width, rand() % height};
    powerUpActive = false;
    lastPowerUpSpawn = steady_clock::now();

    for (int i = 0; i < 5; i++) {
        Position o;
        do {
            o = {rand() % (width - 2) + 1, rand() % (height - 2) + 1};
        } while ((o.x == food.x && o.y == food.y) || (o.x == width / 2 && o.y == height / 2));
        obstacles.push_back(o);
    }
}

// === Update Level and Symbols ===
void updateLevelAndSymbols() {
    level = score / 5 + 1;

    if (utf8) {
        if (score >= 10) foodSymbol = "🍏";
        else if (score >= 5) foodSymbol = "🍎";
        else foodSymbol = "🪶";

        if (score >= 10) wallSymbol = "🪨";
        else if (score >= 5) wallSymbol = "🌳";
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

// === Menus ===
void showInstructions() {
    clearScreen();
    cout << "=== Instructions ===\n";
    cout << "Use Arrow Keys or W A S D to move\n";
    cout << "Eat food to grow and score points\n";
    cout << "🍒 Power-Up appears every few seconds (3x growth, lasts 8 sec)\n";
    cout << "Avoid walls and obstacles!\n\n";
    cout << "Press any key to return...";
    getch_custom();
}

int pauseMenu() {
    clearScreen();
    cout << "========== PAUSED ==========\n";
    cout << "1️⃣  Continue\n";
    cout << "2️⃣  Restart Game\n";
    cout << "3️⃣  Instructions\n";
    cout << "4️⃣  Quit to Main Menu\n";
    cout << "============================\n";
    cout << "Select an option: ";
    char c = getch_custom();
    return c - '0';
}

// === Game Logic ===
void logic() {
    // Power-up timer handling
    auto now = steady_clock::now();
    if (!powerUpActive && duration_cast<milliseconds>(now - lastPowerUpSpawn).count() > powerUpInterval)
        generatePowerUp();

    if (powerUpActive && duration_cast<milliseconds>(now - powerUpStart).count() > powerUpDuration)
        powerUpActive = false;

    Position newHead = snake.front();
    if (dir == UP) newHead.y--;
    else if (dir == DOWN) newHead.y++;
    else if (dir == LEFT) newHead.x--;
    else if (dir == RIGHT) newHead.x++;

    if (newHead.x < 0 || newHead.x >= width || newHead.y < 0 || newHead.y >= height)
        gameOver = true;

    for (auto &o : obstacles)
        if (newHead.x == o.x && newHead.y == o.y)
            gameOver = true;

    for (auto &s : snake)
        if (newHead.x == s.x && newHead.y == s.y)
            gameOver = true;

    snake.push_front(newHead);

    if (newHead.x == food.x && newHead.y == food.y) {
        score++;
        updateLevelAndSymbols();
        generateFood();
    }
    else if (powerUpActive && newHead.x == powerUp.x && newHead.y == powerUp.y) {
        score += 3;
        powerUpActive = false;
        lastPowerUpSpawn = steady_clock::now();
        for (int i = 0; i < 3; i++)
            snake.push_back(snake.back()); // grow by 3
    }
    else {
        snake.pop_back();
    }
}

// === Game Loop ===
void gameLoop() {
    while (!gameOver) {
        drawBoard();
        sleep_ms(200);

        if (kbhit_custom()) {
            char key = getch_custom();

            if (key == 27) {
                int choice = pauseMenu();
                if (choice == 1) continue;
                else if (choice == 2) { resetGame(); gameLoop(); return; }
                else if (choice == 3) { showInstructions(); continue; }
                else if (choice == 4) return;
            }

            if ((key == 'w' && dir != DOWN) || (key == 's' && dir != UP) ||
                (key == 'a' && dir != RIGHT) || (key == 'd' && dir != LEFT) ||
                (key == 72 && dir != DOWN) || (key == 80 && dir != UP) ||
                (key == 75 && dir != RIGHT) || (key == 77 && dir != LEFT)) {
                if (key == 'w' || key == 72) dir = UP;
                else if (key == 's' || key == 80) dir = DOWN;
                else if (key == 'a' || key == 75) dir = LEFT;
                else if (key == 'd' || key == 77) dir = RIGHT;
            }
        }

        logic();
    }

    clearScreen();
    cout << "💀 Game Over! 💀\n";
    cout << "Your Score: " << score << "\n\n";
    cout << "Play again? (y/n): ";
    char c;
    cin >> c;
    if (c == 'y' || c == 'Y') {
        resetGame();
        gameLoop();
    }
}

// === Main ===
int main() {
    srand(time(0));
#ifdef _WIN32
    system("chcp 65001 >nul");
    SetConsoleOutputCP(CP_UTF8);
#endif

    int choice;
    do {
        clearScreen();
        cout << "==============================\n";
        cout << "     🐍 SNAKE ADVENTURE 🐍    \n";
        cout << "==============================\n\n";
        cout << "1️⃣  Start Game\n";
        cout << "2️⃣  Instructions\n";
        cout << "3️⃣  Exit\n\n";
        cout << "Select an option: ";
        cin >> choice;

        if (choice == 1) {
            setupGame();
            gameLoop();
        } else if (choice == 2) {
            showInstructions();
        } else if (choice == 3) {
            cout << "Exiting game... Goodbye!\n";
        } else {
            cout << "Invalid choice.\n";
        }
    } while (choice != 3);

    return 0;
}