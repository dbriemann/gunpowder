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
    ResultNode results[LAST_FIELD+1];//[2];

    U8 win_color = NO_WIN;
    U8 best_move = board.possible_moves[0];
    double best_score = 0.0;

    cerr << "   --- Run simulation for move no. " << (int)board.next_move << " ---" << endl;
    cerr << "   --- Turn time: " << turn_time << " sec." << endl;

    while(!wtimer.out_of_time(turn_time)) {

        //select best.. TODO better
        U8 pm = best_move;

        results[pm].selections++;
        sim_board = board;
        sim_board.makeMove(pm);

        //mc simulation
        win_color = sim_board.randomFill();

        //TODO
        //use fastboard.possible_moves as a move_history...
        //weight importance with distance to current board


        best_score = 0.0;
        for(U8 i : board.possible_moves) {
            //from player perspective
            if(sim_board.stones[i] == board.to_play) {
                results[i].update(WIN_TRANSLATION[board.to_play][win_color]);
            }
//            results[i].update(WIN_TRANSLATION[sim_board.stones[i]][win_color]);


//            results[i].update((sim_board.stones[i] ^ win_color) ^ 1);

//            if(sim_board.stones[i] == player_color) {
//                results[i].update(COLOR_SCORE_CONSTANT[win_color]);
//            } else {
//                results[i].update(-COLOR_SCORE_CONSTANT[win_color]);
//            }

//            double curscore = (double)results[i].wins * COLOR_SCORE_CONSTANT[player_color];
//            double score = (((double)results[i].updates - curscore) / 2.0 + curscore) / (double)results[i].updates;


            double score = (double) results[i].wins / (double) results[i].updates;// + sqrt(2.0 * log(results[i].selections));

            if(score > best_score) {
                best_score = score;
                best_move = i;
            }

        }



        simulations++;

//        best_score = 0.0;
//        for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
//            if(board.stones[i] == EMPTY) {
//                double score = (double) results[i].wins / (double) results[i].updates;

//                if(score > best_score) {
//                    best_score = score;
//                    best_move = i;
//                }
//            }
//        }
    }
//    }

    cerr << "   --- " << simulations << " simulations run." << endl;

    U32 sel = 0;

    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
        if(board.stones[i] == EMPTY) {
            double score = (double) results[i].wins / (double) results[i].updates;
            cerr << " # Move: " << (int) i << " Score: " << score*100.0 << "% Selections: " << results[i].selections << endl;

            if(results[i].selections > sel) {
                sel = results[i].selections;
                best_move = i;
            }
        }
    }

//    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
//        if(board.stones[i] == EMPTY) {
////            double curscore = (double)results[i].wins * COLOR_SCORE_CONSTANT[board.to_play];
////            double score = (((double)results[i].updates - curscore) / 2.0 + curscore) / (double)results[i].updates;
//            double score = (double) results[i].wins / (double) results[i].updates;

//            cerr << " # Move: " << (int) i << " Score: " << score*100.0 << "% Selections: " << results[i].selections << endl;

//            if(score > best_score) {
//                best_score = score;
//                best_move = i;
//            }
//        } else {
//            cerr << " # Move: " << (int) i << " OCCUPIED" << endl;
//        }
//    }

    cerr << "---> # Best Move: " << (int) best_move << " Score: " << best_score*100.0 << "%" << endl;

    return best_move;
}

#endif // PLAINMC_HPP
