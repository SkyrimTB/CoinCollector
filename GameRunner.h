#ifndef GAMERUNNER_H
#define GAMERUNNER_H

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <barrier>
#include <format>
#include <algorithm>
#include <ranges>
#include <utility>

struct PlayerPos {
    int x;
    int y;
};

template <typename T>
class GameRunner {
public:
    GameRunner();
    ~GameRunner();
    GameRunner(GameRunner&& other) noexcept;
    GameRunner& operator=(GameRunner&& other) noexcept;
    void run();

private:
    bool isRunning;
    int currentLevel;
    int playerHP;
    int maxHP;
    int width, height;
    std::vector<std::vector<T>> gameBoard;
    int playerX, playerY;
    std::vector<std::pair<int, int>> fires;
    std::pair<int, int> coin;
    std::pair<int, int> item;
    bool itemPresent;
    int score;
    int coinTarget;
    int fireCount;
    int fireRounds;
    int currentFireRound;
    int itemRounds;
    int currentItemRound;
    int stepCount;
    std::jthread gameThread;
    std::jthread inputThread;
    std::barrier<> barrier{2};
    char input;

    void initGame();
    void placeCoin();
    void placeItem();
    void gameLoop();
    void resetGame();
    void startGameLoop();
    void renderGame();
    void processInput();
    void movePlayer(char input);
    void updateFires(int roundsInLevel);
    void updateItem();
    void checkCollisions();
    void displayInstructions();
};

#endif // GAMERUNNER_H