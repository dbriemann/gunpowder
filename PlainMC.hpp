#ifndef PLAINMC_HPP
#define PLAINMC_HPP

#include <unordered_map>
using namespace std;

#include "FastBoard.hpp"
#include "Timer.hpp"

struct ResultNode {
    U32 wins;
    U32 updates;
    U32 selections;
//    U8 move;

    ResultNode() {
        wins = 0;
        updates = 0;
        selections = 0;
//        move = 0;
    }

//    ResultNode(U32 wins, U32 updates, U32 selections, U8 move) {
//        this->wins = wins;
//        this->updates = updates;
//        this->selections = selections;
//        this->move = move;
//    }

    void update(U8 score) {
        wins += score;
        updates++;
    }
};

struct PlainMCEngine {
    U8 player_color;
    FastBoard board;
    FastBoard sim_board;
    WallTimer wtimer;
    U64 simulations;

    PlainMCEngine();
    PlainMCEngine(const FastBoard &board);

    inline U8 runSim(double remaining_time);
    void makePermanentMove(U8 idx);
};

PlainMCEngine::PlainMCEngine() {
    player_color = WHITE;
    simulations = 0;
    board = FastBoard();
    sim_board = FastBoard();
}

PlainMCEngine::PlainMCEngine(const FastBoard &board ) {
    player_color = board.to_play;
    simulations = 0;
    this->board = board;
    sim_board = board;
}

inline void PlainMCEngine::makePermanentMove(U8 idx) {
    board.makeMove(idx);
}

inline U8 PlainMCEngine::runSim(double remaining_time) {
    wtimer.start();

    double turn_time = remaining_time / 10.0;

    simulations = 0;
    ResultNode results[LAST_FIELD+1];
//    for(U8 pm : board.possible_moves) {
//        results[pm] = ResultNode();
//    }

    U8 win = NO_WIN;
    U8 best_move = board.possible_moves[0];
    double best_score = 0.0;

    cerr << "   --- Run Simulation for move no. " << (int)board.next_move << " ---" << endl;

    while(!wtimer.out_of_time(turn_time)) {

        for(U8 pm : board.possible_moves) {
            //TODO change selection strategy
            results[pm].selections++;
            sim_board = board;
            sim_board.makeMove(pm);

            //mc simulation
            win = sim_board.randomFill();

            for(U8 i : board.possible_moves) {
                //TODO remove if?!?!
                if(sim_board.stones[i] == win) {
                    results[i].update(1);
                } else {
                    results[i].update(0);
                }
            }

            simulations++;
        }
    }

    cerr << "   --- " << simulations << " simulations run." << endl;

    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
        if(board.stones[i] == EMPTY) {
            double score = (double) results[i].wins / (double) results[i].updates;

            cerr << " # Move: " << (int) i << " Score: " << score*100.0 << "% Selections: " << results[i].selections << endl;

            if(score > best_score) {
                best_score = score;
                best_move = i;
            }
        } else {
            cerr << " # Move: " << (int) i << " OCCUPIED" << endl;
        }
    }

    cerr << "---> # Best Move: " << (int) best_move << " Score: " << best_score*100.0 << "%" << endl;

    return best_move;
}

#endif // PLAINMC_HPP
