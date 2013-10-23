#ifndef PLAINMC_HPP
#define PLAINMC_HPP

#include <unordered_map>
using namespace std;

#include "FastBoard.hpp"
#include "Timer.hpp"

struct ResultNode {
    I32 wins;
    U32 updates;
    U32 selections;

    ResultNode() {
        wins = 0;
        updates = 0;
        selections = 0;
    }

    inline
    void update(I8 score) {
        this->wins += score;
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

inline
void PlainMCEngine::makePermanentMove(U8 idx) {
    board.makeMove(idx);
}

inline
U8 PlainMCEngine::runSim(double remaining_time) {
    wtimer.start();

    double turn_time = remaining_time / 10.0;

    simulations = 0;
    ResultNode results[LAST_FIELD+1];

    U8 win_color = NO_WIN;
    U8 best_move = board.possible_moves[0];
    U8 next_move = best_move;
    double best_score = 0.0;

    cerr << "   --- Run simulation for move no. " << (int)board.next_move << " ---" << endl;
    cerr << "   --- Turn time: " << turn_time << " sec." << endl;

    while(!wtimer.out_of_time(turn_time)) {
        //selection -> TODO: improve
//        U8 r = fastrand() % board.possible_moves.size(); //best_move;
//        next_move = board.possible_moves[r];

        results[next_move].selections++;
        sim_board = board;
        sim_board.makeMove(next_move);

        //mc simulation
        win_color = sim_board.randomFill();

        simulations++;

        double uct = 0;
        double simlog = log(simulations);

        for(U8 m : board.possible_moves) {
            //from player perspective
            if(sim_board.stones[m] == board.to_play) {
//                results[m].update(COLOR_WIN_BONUS[win_color]);
                results[m].update(WIN_TRANSLATION[board.to_play][win_color]);
            }
//            else {
//                results[m].update(-COLOR_WIN_BONUS[win_color]);
//            }

            //UCTValue(parent, n) = winrate + sqrt((ln(parent.visits))/(n.nodevisits))
//            I32 rel_score = results[m].wins * COLOR_WIN_BONUS[board.to_play];
//            double score = ((double) results[m].updates + (double) rel_score) * 0.5 / (double)results[m].updates;
            double score = (double)results[m].wins / (double) results[m].updates;
            double u = score + sqrt(simlog / (results[m].selections+1));

            if(u > uct) {
                next_move = m;
                uct = u;
            }
        }        
    }

    cerr << "   --- " << simulations << " simulations run." << endl;

    U32 sel = 0;

    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
        if(board.stones[i] == EMPTY) {
//            I32 rel_score = results[i].wins * COLOR_WIN_BONUS[board.to_play];
//            double score = ((double) results[i].updates + (double) rel_score) * 0.5 / (double)results[i].updates;
            double score = (double)results[i].wins / (double) results[i].updates;
            cerr << " # Move: " << (int) i << " Score: " << score*100.0 << "% Updates: " << results[i].updates << " Selections: " << results[i].selections << endl;

            if(results[i].selections > sel) {
                best_score = score;
                sel = results[i].selections;
                best_move = i;
            }
        }
    }

    cerr << "---> # Best Move: " << (int) best_move << " Score: " << best_score*100.0 << "% Selections: " << sel << endl;

    return best_move;
}

#endif // PLAINMC_HPP
