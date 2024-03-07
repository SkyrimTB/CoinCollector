#include "GameRunner.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

template <typename T>
GameRunner<T>::GameRunner() : isRunning(true), currentLevel(1), playerHP(4), maxHP(4), score(0), coinTarget(50), fireCount(2), itemRounds(5), stepCount(0) {
    displayInstructions();
    initGame();
}

template <typename T>
GameRunner<T>::~GameRunner() {
    isRunning = false;
}

template <typename T>
GameRunner<T>::GameRunner(GameRunner&& other) noexcept
    : isRunning(std::exchange(other.isRunning, false)),
      currentLevel(std::exchange(other.currentLevel, 0)),
      playerHP(std::exchange(other.playerHP, 0)),
      maxHP(std::exchange(other.maxHP, 0)),
      width(std::exchange(other.width, 0)),
      height(std::exchange(other.height, 0)),
      gameBoard(std::move(other.gameBoard)),
      playerX(std::exchange(other.playerX, 0)),
      playerY(std::exchange(other.playerY, 0)),
      fires(std::move(other.fires)),
      coin(std::move(other.coin)),
      item(std::move(other.item)),
      itemPresent(std::exchange(other.itemPresent, false)),
      score(std::exchange(other.score, 0)),
      coinTarget(std::exchange(other.coinTarget, 0)),
      fireCount(std::exchange(other.fireCount, 0)),
      fireRounds(std::exchange(other.fireRounds, 0)),
      currentFireRound(std::exchange(other.currentFireRound, 0)),
      itemRounds(std::exchange(other.itemRounds, 0)),
      currentItemRound(std::exchange(other.currentItemRound, 0)),
      stepCount(std::exchange(other.stepCount, 0)),
      gameThread(std::move(other.gameThread)),
      inputThread(std::move(other.inputThread)),
      barrier(std::move(other.barrier)),
      input(std::exchange(other.input, '\0')) {
    other.gameThread = std::jthread();
    other.inputThread = std::jthread();
}

template <typename T>
GameRunner<T>& GameRunner<T>::operator=(GameRunner&& other) noexcept {
    if (this != &other) {
        isRunning = std::exchange(other.isRunning, false);
        currentLevel = std::exchange(other.currentLevel, 0);
        playerHP = std::exchange(other.playerHP, 0);
        maxHP = std::exchange(other.maxHP, 0);
        width = std::exchange(other.width, 0);
        height = std::exchange(other.height, 0);
        gameBoard = std::move(other.gameBoard);
        playerX = std::exchange(other.playerX, 0);
        playerY = std::exchange(other.playerY, 0);
        fires = std::move(other.fires);
        coin = std::move(other.coin);
        item = std::move(other.item);
        itemPresent = std::exchange(other.itemPresent, false);
        score = std::exchange(other.score, 0);
        coinTarget = std::exchange(other.coinTarget, 0);
        fireCount = std::exchange(other.fireCount, 0);
        fireRounds = std::exchange(other.fireRounds, 0);
        currentFireRound = std::exchange(other.currentFireRound, 0);
        itemRounds = std::exchange(other.itemRounds, 0);
        currentItemRound = std::exchange(other.currentItemRound, 0);
        stepCount = std::exchange(other.stepCount, 0);
        gameThread = std::move(other.gameThread);
        inputThread = std::move(other.inputThread);
        barrier = std::move(other.barrier);
        input = std::exchange(other.input, '\0');

        other.gameThread = std::jthread();
        other.inputThread = std::jthread();
    }
    return *this;
}

template <typename T>
void GameRunner<T>::run() {
    startGameLoop();
    inputThread = std::jthread(&GameRunner::processInput, this);
    gameThread.join();
}

template <typename T>
void GameRunner<T>::initGame() {
    width = 10 + (currentLevel - 1) * 5;
    height = 5 + (currentLevel - 1) * 3;
    gameBoard.resize(height, std::vector<T>(width, ' '));

    PlayerPos playerPos{.x = width / 2, .y = height / 2};
    playerX = playerPos.x;
    playerY = playerPos.y;

    fires.clear();
    for (int i = 0; i < fireCount; ++i) {
        int x, y;
        do {
            x = std::rand() % width;
            y = std::rand() % height;
        } while (gameBoard[y][x] != ' ');
        gameBoard[y][x] = 'F';
        fires.push_back({x, y});
    }

    placeCoin();
    currentFireRound = 0;
    currentItemRound = 0;
    itemPresent = false;
}

template <typename T>
void GameRunner<T>::placeCoin() {
    int x, y;
    do {
        x = std::rand() % width;
        y = std::rand() % height;
    } while (gameBoard[y][x] != ' ');
    gameBoard[y][x] = 'C';
    coin = {x, y};
}

template <typename T>
void GameRunner<T>::placeItem() {
    auto validPositions = gameBoard | std::views::filter([](const auto& row) {
        return std::ranges::any_of(row, [](char cell) { return cell == ' '; });
    });

    auto [y, x] = *std::ranges::begin(validPositions);
    gameBoard[y][x] = 'H';
    item = {x, y};
    itemPresent = true;
}

template <typename T>
void GameRunner<T>::gameLoop() {
    int roundsInLevel = 0;
    while (isRunning) {
        renderGame();

        barrier.arrive_and_wait();

        if (!isRunning) {
            break;
        }

        if (input == 'q') {
            std::cout << "Exiting the game...\n";
            isRunning = false;
            break;
        } else if (input == 'r') {
            std::cout << "Restarting the game...\n";
            resetGame();
            roundsInLevel = 0;
            continue;
        }

        movePlayer(input);
        updateFires(roundsInLevel);
        updateItem();
        checkCollisions();
        input = 0;

        if (score >= coinTarget) {
            currentLevel++;
            if (currentLevel <= 3) {
                std::cout << std::format("Congratulations! You collected enough coins to advance to level {}.\n", currentLevel);
                coinTarget += 50;
                roundsInLevel = 0;
                initGame();
            } else {
                std::cout << "Congratulations! You have completed all levels.\n";
                std::cout << std::format("Your total score: {}\n", score);
                isRunning = false;
            }
        }

        if (playerHP <= 0) {
            std::cout << "Game Over! You ran out of HP.\n";
            std::cout << std::format("Your total score: {}\n", score);
            std::cout << "Do you want to restart the game? (y/n): ";
            char restartInput;
            std::cin >> restartInput;
            if (restartInput == 'y' || restartInput == 'Y') {
                resetGame();
                roundsInLevel = 0;
            } else {
                isRunning = false;
            }
        }

        roundsInLevel++;
    }
}

template <typename T>
void GameRunner<T>::resetGame() {
    currentLevel = 1;
    playerHP = maxHP;
    score = 0;
    coinTarget = 50;
    fireCount = 2;
    stepCount = 0;
    initGame();
}

template <typename T>
void GameRunner<T>::startGameLoop() {
    gameThread = std::jthread(&GameRunner::gameLoop, this);
}

template <typename T>
void GameRunner<T>::renderGame() {
    std::cout << "\033[2J\033[1;1H"; // Clear the screen
    std::cout << std::format("Level: {}\n", currentLevel);
    std::cout << std::format("Player HP: {} / {}\n", playerHP, maxHP);
    std::cout << std::format("Score: {} / {}\n", score, coinTarget);
    std::cout << std::format("Steps: {}\n", stepCount);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (x == playerX && y == playerY) {
                std::cout << "\033[92mP\033[0m"; // Player (green)
            } else if (gameBoard[y][x] == 'F') {
                std::cout << "\033[91mF\033[0m"; // Fire (red)
            } else if (x == item.first && y == item.second && itemPresent) {
                std::cout << "\033[94mH\033[0m"; // Healing item (blue)
            } else if (gameBoard[y][x] == 'C') {
                std::cout << "\033[93mC\033[0m"; // Coin (yellow)
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "Press 'q' to exit the game.\n";
    std::cout << "Press 'r' to restart the game.\n";
}

template <typename T>
void GameRunner<T>::processInput() {
    while (isRunning) {
        std::cout << "Enter your move (w/a/s/d) or command (q/r): ";
        char ch;
        std::cin >> ch;
        input = ch;
        barrier.arrive_and_wait();
    }
}

template <typename T>
void GameRunner<T>::movePlayer(char input) {
    int dx = 0, dy = 0;
    switch (input) {
        case 'w':
            dy = -1;
            break;
        case 'a':
            dx = -1;
            break;
        case 's':
            dy = 1;
            break;
        case 'd':
            dx = 1;
            break;
        default:
            return;
    }

    int newX = playerX + dx;
    int newY = playerY + dy;

    if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
        if (gameBoard[newY][newX] == ' ' || gameBoard[newY][newX] == 'C' || (newX == item.first && newY == item.second && itemPresent)) {
            gameBoard[playerY][playerX] = ' ';
            playerX = newX;
            playerY = newY;
            stepCount++;
        } else if (gameBoard[newY][newX] == 'F') {
            playerHP--;
        }
    }
}

template <typename T>
void GameRunner<T>::updateFires(int roundsInLevel) {
    int adjustedFireCount = fireCount + (roundsInLevel / 10);
    for (auto& fire : fires) {
        gameBoard[fire.second][fire.first] = ' ';
    }
    fires.clear();
    for (int i = 0; i < adjustedFireCount; ++i) {
        int x, y;
        do {
            x = std::rand() % width;
            y = std::rand() % height;
        } while (gameBoard[y][x] != ' ');
        gameBoard[y][x] = 'F';
        fires.push_back({x, y});
    }
}

template <typename T>
void GameRunner<T>::updateItem() {
    currentItemRound++;
    if (currentItemRound >= itemRounds && !itemPresent) {
        placeItem();
        currentItemRound = 0;
    }
}

template <typename T>
void GameRunner<T>::checkCollisions() {
    if (playerX == coin.first && playerY == coin.second) {
        score += (std::rand() % 2 + 1) * 10;
        gameBoard[coin.second][coin.first] = ' ';
        gameBoard[playerY][playerX] = 'P';
        placeCoin();
    }

    if (playerX == item.first && playerY == item.second) {
        if (playerHP < maxHP) {
            playerHP++;
            gameBoard[item.second][item.first] = ' ';
            gameBoard[playerY][playerX] = 'P';
            itemPresent = false;
        }
    }
}

template <typename T>
void GameRunner<T>::displayInstructions() {
    std::cout << "Welcome to the Coin Collector Game!\n";
    std::cout << "Objective: Collect coins to advance levels while avoiding fires.\n";
    std::cout << "Controls:\n";
    std::cout << "  - Move Up: W\n";
    std::cout << "  - Move Left: A\n";
    std::cout << "  - Move Down: S\n";
    std::cout << "  - Move Right: D\n";
    std::cout << "  - Quit Game: Q\n";
    std::cout << "  - Restart Game: R\n";
    std::cout << "Symbols:\n";
    std::cout << "  - \033[92mP\033[0m: Player\n";
    std::cout << "  - \033[91mF\033[0m: Fire\n";
    std::cout << "  - \033[94mH\033[0m: Healing Item\n";
    std::cout << "  - \033[93mC\033[0m: Coin\n";
    std::cout << "Press Enter to start the game...";
    std::cin.ignore();
}

template class GameRunner<char>;