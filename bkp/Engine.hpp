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

    cerr << endl << "NEXT MOVE:   " << (int)board.next_move << endl;

    I32 results[LAST_FIELD+1] = {0};
    U32 visits[LAST_FIELD+1] = {0};
    U8 move_history[LAST_FIELD+1] = {0};
    U8 rand_move;
    I8 win;
    U8 best_move = board.possible_moves[0];
    double best_score = 0;
    U64 simulations = 0ULL;

    double this_turn_time = remaining_time / 10.0;
    cerr << "Remaining time: " << this_turn_time << " sec." << endl;


    while(!wtimer.out_of_time(this_turn_time)) {
        win = NONE;

        //backup origin state
        sim_board = board;
        sim_color = color;

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
            visits[move_history[i]]++;
        }
//        for(int i = board.next_move + 1; i < sim_board.next_move; i+=2) {
//            results[move_history[i]] -= win;
//            visits[move_history[i]]++;
//        }

        //minimum_game_length = min(minimum_game_length, sim_board.next_move);
        simulations++;
    }

    vector< pair<U8, double> > final_scores;

    for(U8 m : board.possible_moves) {
        double curscore = (double)results[m] * COLOR_SCORE_MULTIPLIER[color];
        double perc = (((double)visits[m] - curscore) / 2.0 + curscore) / (double)visits[m];
        final_scores.push_back(make_pair(m, perc));
//        double perc = ((((double)amounts[eee] - (double)results[eee]) / 2.0) + (double)results[eee]) / (double)amounts[eee];
    }

    //sort scores
    std::sort(final_scores.begin(), final_scores.end(), [](const pair<int, double>& lhs, const pair<int, double>& rhs) {
        return lhs.second > rhs.second;
    } );
    best_move = final_scores[0].first;
    best_score = final_scores[0].second;

    cerr << "Total simulations: " << simulations << endl;
    cerr << "Best Move: " << (int) best_move << " Score: " << best_score << endl;

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
