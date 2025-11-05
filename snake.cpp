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
Direction dir = RIGHT; // 🟢 define global direction

// === Symbols ===
string headSymbol = "🐍", bodySymbol = "🟩", foodSymbol = "🪶", wallSymbol = "🧱";

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

// === Draw Game Board ===
void drawBoard() {
    moveCursorToTop();
    cout << "Score: " << score << "   Level: " << level << "\n";

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

    // Generate food
    generateFood();

    // Add obstacles
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
    headSymbol = "🐍";
    bodySymbol = "🟩";
    foodSymbol = "🪶";
    wallSymbol = "🧱";

    // Regenerate new food
    food = {rand() % width, rand() % height};

    // Regenerate new obstacles
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

// === Game Logic ===
void logic() {
    Position newHead = snake.front();

    if (dir == UP) newHead.y--;
    else if (dir == DOWN) newHead.y++;
    else if (dir == LEFT) newHead.x--;
    else if (dir == RIGHT) newHead.x++;

    // Collision checks
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
    } else {
        snake.pop_back();
    }
}

// === Menus ===
void showMenu() {
    clearScreen();
    cout << "==============================\n";
    cout << "     🐍 SNAKE ADVENTURE 🐍    \n";
    cout << "==============================\n\n";
    cout << "1️⃣  Start Game\n";
    cout << "2️⃣  Instructions\n";
    cout << "3️⃣  Exit\n\n";
    cout << "Select an option: ";
}

void showInstructions() {
    clearScreen();
    cout << "=== Instructions ===\n";
    cout << "Use Arrow Keys or W A S D to move\n";
    cout << "Eat food to grow and score points\n";
    cout << "Avoid walls and obstacles!\n\n";
    cout << "Press any key to return...";
    getch_custom();
}

// === Main Game Loop ===
void gameLoop() {
    while (!gameOver) {
        drawBoard();
        sleep_ms(200); 

        if (kbhit_custom()) {
            char key = getch_custom();
            if ((key == 'w' && dir != DOWN) || (key == 's' && dir != UP) ||
                (key == 'a' && dir != RIGHT) || (key == 'd' && dir != LEFT) ||
                (key == 72 && dir != DOWN) || (key == 80 && dir != UP) ||   // Arrow ↑ ↓
                (key == 75 && dir != RIGHT) || (key == 77 && dir != LEFT))  // Arrow ← →
            {
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
        showMenu();
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
