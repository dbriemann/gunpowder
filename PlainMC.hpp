#ifndef PLAINMC_HPP
#define PLAINMC_HPP

#include <unordered_map>
using namespace std;

#include "FastBoard.hpp"
#include "Timer.hpp"

struct ResultNode {
    I32 score;
    U32 updates;
    U32 selections;

    ResultNode() {
        score = 0;
        updates = 0;
        selections = 0;
    }

    inline
    void update(I8 score) {
        this->score += score;
        updates++;
    }
};

struct PlainMCEngine {
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
    simulations = 0;
    board = FastBoard();
    sim_board = FastBoard();
}

PlainMCEngine::PlainMCEngine(const FastBoard &board ) {
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

    I8 win_score = NO_WIN;
    U8 best_move = board.possible_moves[0];

    cerr << "   --- Run simulation for move no. " << (int)board.next_move << " ---" << endl;
    cerr << "   --- Turn time: " << turn_time << " sec." << endl;

    while(!wtimer.out_of_time(turn_time)) {

        //selection -> TODO: improve
        U8 r = fastrand() % board.possible_moves.size(); //best_move;
        U8 pm = board.possible_moves[r];

        results[pm].selections++;
        sim_board = board;
        sim_board.makeMove(pm);

        //mc simulation
        win_score = sim_board.randomFill();

        //TODO
        //use fastboard.possible_moves as a move_history...
        //weight importance with distance to current board

        for(U8 i : board.possible_moves) {
            //from player perspective
            if(sim_board.stones[i] == board.to_play) {
                results[i].update(win_score);
            } else {
                results[i].update(-win_score);
            }
        }

        simulations++;
    }


    cerr << "   --- " << simulations << " simulations run." << endl;

    U32 sel = 0;

    I32 best_rel_score = -1000000;
    double best_score = 0.0;

    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
        if(board.stones[i] == EMPTY) {
            I32 rel_score = results[i].score * COLOR_WIN_BONUS[board.to_play];
            double score = ((double) results[i].updates + (double) rel_score) * 0.5 / (double)results[i].updates;
            cerr << " # Move: " << (int) i << " Diff: " << rel_score << " Score: " << score*100.0 << "% Updates: " << results[i].updates << " Selections: " << results[i].selections << endl;

            if(rel_score > best_rel_score) {
                best_rel_score = rel_score;
                best_score = score;
                sel = results[i].selections;
                best_move = i;
            }
        }
    }

    cerr << "---> # Best Move: " << (int) best_move << " Diff: " << best_rel_score << " Score: " << best_score*100.0 << "% Selections: " << sel << endl;

    return best_move;
}

#endif // PLAINMC_HPP
