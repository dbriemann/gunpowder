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
    U8 sim_color;
    //U8 avg_game_length;
    WallTimer wtimer;
    Board board;
    Board sim_board;

    Engine();

    inline I8 permanentMove(U8 idx);

    inline U8 getRandomMove();
    inline U8 getRandomMoveWithFlip();
    inline U8 getBestMove(double remaining_time);
    inline void colorFlip();
    inline void simFlip();
};

Engine::Engine() {
    wtimer = WallTimer();
    //avg_game_length = 97;
    color = BLACK;
    sim_color = BLACK;
    board = Board();
    sim_board = Board();
}

inline
void Engine::colorFlip() {
    board.next_move++;
    color = FLIP(color);
}

inline
void Engine::simFlip() {
    sim_board.next_move++;
    sim_color = FLIP(sim_color);
}

inline
U8 Engine::getBestMove(double remaining_time) {
    wtimer.start();
    cerr << "Next Move: " << (int)board.next_move << endl;
    I32 results[LAST_FIELD+2] = {0};
    U8 move_history[LAST_FIELD+2] = {0};
    U8 first_move;
    U8 rand_move;
    I8 win;
    U8 best_move = board.possible_moves[0];
    //U8 minimum_game_length = 20;
    U64 simulations = 0ULL;

    //U8 rest_length = max(minimum_game_length - board.move_nr + 1, 5);
    //double this_turn_time = remaining_time / (double)(rest_length);
    double this_turn_time = remaining_time / 20.0;//;(double)max(5, 60-board.move_nr);
    //cerr << "Minimum rest length: " << (int)(rest_length) << endl;
    cerr << "Remaining time: " << this_turn_time << " sec." << endl;


    while(!wtimer.out_of_time(this_turn_time)) {
        win = NONE;

        //backup origin state
        sim_board = board;
        sim_color = color;

        if(sim_board.next_move == 2) {
            first_move = getRandomMoveWithFlip();
            best_move = FLIP_MOVE;
        } else {
            first_move = getRandomMove();
        }

        move_history[sim_board.next_move] = first_move;
        if(first_move == FLIP_MOVE) {
            simFlip();
        } else {
            win = sim_board.makeMove(first_move);
        }

        //run simulation until game is finished
        while(win == NONE) {
            rand_move = getRandomMove();
            move_history[sim_board.next_move] = rand_move;
            win = sim_board.makeMove(rand_move);            
        }

        //standard result adjustment
//        results[first_move] += win;

        //history heuristic result adjustment
        for(int i = board.next_move; i < sim_board.next_move; i+=2) {
            results[move_history[i]] += win;
        }
        for(int i = board.next_move + 1; i < sim_board.next_move; i+=2) {
            results[move_history[i]] -= win;
        }

        //minimum_game_length = min(minimum_game_length, sim_board.next_move);
        simulations++;
    }

    I32 best_score = results[best_move] * COLOR_SCORE_MULTIPLIER[color];
    //find move with max rank
    for(U8 m : board.possible_moves) {
        I32 scr = results[m] * COLOR_SCORE_MULTIPLIER[color];
        if(scr > best_score) {
            best_score = scr;
            best_move = m;
        }
    }

    cerr << "Total simulations: " << simulations << endl;
    cerr << "Best Move: " << (int) best_move << " Score: " << best_score << endl;
    cerr << "FLIP MOVE: " << (int) FLIP_MOVE << " Score: " << (int)results[FLIP_MOVE] << endl;

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
U8 Engine::getRandomMoveWithFlip() {
    int moves = sim_board.possible_moves.size() + 1;
    U32 rand_idx = rand() % moves;

    if(rand_idx == sim_board.possible_moves.size()) {
        return FLIP_MOVE;
    } else {
        return sim_board.possible_moves[rand_idx];
    }
}

inline
I8 Engine::permanentMove(U8 idx) {
    if(idx == FLIP_MOVE) {
        colorFlip();
        return NONE;
    } else {
        return board.makeMove(idx);
    }
}

#endif // ENGINE_HPP
