#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <ctime>
#include <cstdio>
using namespace std;

#include "Timer.hpp"
#include "definitions.hpp"
#include "Board.hpp"

struct Engine {
    U8 color;
    U8 avg_game_length;
    WallTimer wtimer;
    Board board;
    Board sim_board;

    Engine();

    inline I8 permanentMove(U8 idx);

    inline U8 getRandomMove();
    inline U8 getBestMove(double remaining_time);
    inline void colorFlip();
};

Engine::Engine() {
    wtimer = WallTimer();
    avg_game_length = 97;
    color = BLACK;
    board = Board();
    sim_board = Board();
}

inline
void Engine::colorFlip() {
    board.move_nr++;
    color = FLIP(color);
}

inline
U8 Engine::getBestMove(double remaining_time) {
    wtimer.start();
    I32 results[LAST_FIELD+1] = {0};
    U8 first_move;
    U8 rand_move;
    I8 win;
    U64 simulations = 0;
    U64 cumulative_game_length = 0;
    double this_turn_time = remaining_time / (double)(avg_game_length - board.move_nr);

    while(!wtimer.out_of_time(this_turn_time)) {
        sim_board = board; //backup origin state
        first_move = getRandomMove();
        win = sim_board.makeMove(first_move);

        //run simulation until game is finished
        while(win == NONE) {
            rand_move = getRandomMove();
            win = sim_board.makeMove(rand_move);
        }

        results[first_move] += win;

        cumulative_game_length += sim_board.move_nr;
        simulations++;
    }

    avg_game_length = cumulative_game_length / simulations;

    U8 best_move = board.possible_moves[0];
    I32 best_score = results[best_move] * COLOR_SCORE_MULTIPLIER[color];
    //find move with max rank
    for(U8 m : board.possible_moves) {
        I32 scr = results[m] * COLOR_SCORE_MULTIPLIER[color];
        if(scr > best_score) {
            best_score = scr;
            best_move = m;
        }
    }

    cerr << "Best Move: " << (int) best_move << " Score: " << best_score << endl;
    //cerr << "Avg. game length: " << (double) total_duration / (double) simulations << endl;

    return best_move;
}

inline
U8 Engine::getRandomMove() {
    int moves = sim_board.possible_moves.size();
    int rand_idx = rand() % moves;
    return sim_board.possible_moves[rand_idx];
}

inline
I8 Engine::permanentMove(U8 idx) {
    return board.makeMove(idx);
}

#endif // ENGINE_HPP
