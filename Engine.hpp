#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Timer.hpp"
#include "definitions.hpp"
#include "Board.hpp"

struct Engine {
    Board board;
    Board sim_board;

    Engine();

    void randomMove();
    U8 simulateGame();
};

Engine::Engine() {
    board = Board();
    sim_board = Board();
}

U8 Engine::simulateGame() {
    while(true) {

    }
    return 0;
}

#endif // ENGINE_HPP
