# Coin Collector Game

The Coin Collector game is a console-based game written in C++ where the player navigates through levels, collects coins, and avoids fires to progress and achieve a high score.

## Features

- Three levels of increasing difficulty and size
- Player movement using W, A, S, D keys
- Randomly generated coins to collect
- Randomly generated fires to avoid
- Healing items that appear every 5 rounds
- Step counter to keep track of player's moves
- Game over when player's HP reaches zero
- Option to restart the game after game over
- Colorful ASCII art for game elements

## C++ Features Used

The Coin Collector game project utilizes several modern C++ features and concepts, including:

1. **Move Semantics**: The project uses move semantics to efficiently transfer ownership of resources, such as game objects and threads, when moving them between objects. This is achieved through the use of move constructors and move assignment operators in the `GameRunner` class.

2. **RAII (Resource Acquisition Is Initialization)**: The project follows the RAII principle to manage resources. The `GameRunner` class encapsulates the game resources and ensures their proper initialization and cleanup. The constructor initializes the necessary resources, while the destructor releases them, guaranteeing safe resource management.

3. **Templates**: The project utilizes templates to create a generic `GameRunner` class. The `GameRunner` class is templated on the type of the game board elements, allowing flexibility and reusability. The `main.cpp` file instantiates the `GameRunner` class with the desired type (`char` in this case).

4. **Concurrency**: The project uses concurrency to handle the game loop and user input simultaneously. It creates separate threads for the game loop (`gameThread`) and input processing (`inputThread`) using `std::jthread`. The `std::jthread` is a C++20 feature that automatically joins the thread when it goes out of scope, simplifying thread management.

5. **Lambdas**: Lambda expressions are used in various parts of the project for concise and inline functionality. For example, lambdas are used with `std::ranges` and `std::views::filter` to define compact predicates for filtering game board positions.

6. **std::barrier**: The project utilizes `std::barrier`, a synchronization primitive introduced in C++20, to coordinate the execution of the game loop and input processing threads. The `barrier` ensures that both threads reach a certain point before proceeding, preventing race conditions and ensuring proper synchronization.

7. **std::format**: The project uses `std::format`, a C++20 feature, for convenient and type-safe string formatting. It is used to format output messages, such as displaying the game state, level information, and scores.

8. **std::ranges**: The project employs `std::ranges`, a C++20 library, to perform operations on ranges of elements. It is used in conjunction with `std::views::filter` to find valid positions for placing game objects on the game board.

These features enhance the project's efficiency, readability, and maintainability while showcasing the capabilities of modern C++.


## Requirements

- C++20 compiler
- CMake 3.12 or higher

## How to Play

- Use the W, A, S, D keys to move the player up, left, down, and right, respectively.
- Collect coins to increase your score and progress to the next level.
- Avoid touching the fires, as they will decrease your HP.
- Collect healing items (H) to restore your HP.
- Press 'q' to quit the game at any time.
- Press 'r' to restart the game at any time.

## Code Structure

The project is organized into the following files:

- `GameRunner.h`: Header file containing the declaration of the `GameRunner` class.
- `GameRunner.cpp`: Implementation file for the `GameRunner` class, containing the game logic and mechanics.
- `main.cpp`: Entry point of the program, creates an instance of `GameRunner` and starts the game.
- `CMakeLists.txt`: CMake configuration file for building the project.

## Dependencies

The project relies on the following C++ features and libraries:

- C++20 standard library
- `<thread>` for multi-threading support
- `<chrono>` for time-related functionality
- `<format>` for string formatting (C++20 feature)
- `<barrier>` for thread synchronization (C++20 feature)
- `<ranges>` for range-based operations (C++20 feature)