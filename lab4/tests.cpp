// lab4/tests.cpp
// Part C: minimal harness for coverage.
// Part D: adds one real assertion for Rule 2, using the isGameOver() seam.

#include <iostream>
#include <sstream>
#include <cassert>
#include <string>

#define main main_disabled_from_snake_cpp
#include "../snake.cpp"
#undef main

// Rule 2: "The snake dies when its head moves outside the board boundary."
// setupGame() starts the snake centered, moving RIGHT, and we never touch
// dir. Calling logic() repeatedly walks it toward the right wall. 15 calls
// is comfortably more than the 10 needed to cross width=20 from x=10, so
// under normal conditions gameOver becomes true via the boundary check at
// snake.cpp:243.
//
// Note: obstacles are placed randomly by setupGame() (snake.cpp:164-170,
// unseeded rand()). There's a small chance an obstacle lands directly in
// the snake's path and ends the game one step early via the obstacle check
// instead of the boundary check. isGameOver() can't distinguish which check
// fired - only that the game ended. This is a known limitation of testing
// through a single read-only seam without also seeding/controlling rand();
// documented here rather than hidden.
void test_rule2_dies_at_boundary() {
    SnakeGame game;
    game.setupGame();

    for (int i = 0; i < 15 && !game.isGameOver(); i++) {
        game.logic();
    }

    assert(game.isGameOver() && "Rule 2 failed: snake did not die within 15 straight moves");
    std::cout << "[PASS] Rule 2: snake dies after moving off the board\n";
}

int main() {
    // --- Part C smoke checks ---
    SnakeGame game;
    game.setupGame();

    std::ostringstream captured;
    std::streambuf* oldBuf = std::cout.rdbuf(captured.rdbuf());
    game.drawBoard();
    std::cout.rdbuf(oldBuf);

    std::string output = captured.str();
    assert(!output.empty());
    assert(output.find("Score:") != std::string::npos);

    game.resetGame();
    game.updateLevelAndSymbols();

    // --- Part D: real assertion for Rule 2, via the isGameOver() seam ---
    test_rule2_dies_at_boundary();

    std::cout << "All tests passed.\n";
    return 0;
}