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
//    vector<ResultNode> d3tree;

    PlainMCEngine();
    PlainMCEngine(const FastBoard &board);

    inline U8 runSim(double remaining_time);
    inline U8 d3Sim(double remaining_time);
    inline void runAnalysis(double verbose_time);
    inline void makePermanentMove(U8 idx);
};

PlainMCEngine::PlainMCEngine() {
    simulations = 0;
    board = FastBoard();
    sim_board = FastBoard();
//    d3tree = vector<ResultNode>(TOTAL_DIM);
}

PlainMCEngine::PlainMCEngine(const FastBoard &board ) {
    simulations = 0;
    this->board = board;
    sim_board = board;
//    d3tree = vector<ResultNode>(TOTAL_DIM);
}

inline
void PlainMCEngine::makePermanentMove(U8 idx) {
    if(idx == FLIP_MOVE) {
        board.colorFlip();
    } else {
        board.makeMove(idx);
    }
}


//inline
//U8 PlainMCEngine::d3Sim(double remaining_time) {
//    wtimer.start();

//    double turn_time = remaining_time / 10.0;

//    simulations = 0;
//    U8 player = board.to_play;
//    U8 opponent = FLIP(board.to_play);
//    U8 win_color = NO_WIN;
//    double best_score = 0.0;
//    double score = 0.0;

//    d3tree = vector<ResultNode>(TOTAL_DIM); //TODO -- reuse old parts of tree?

//    cerr << "   --- Run simulation for move no. " << (int)board.next_move << " --- Color = " << (int)player << endl;
//    cerr << "   --- Turn time: " << turn_time << " sec." << endl;

//    while(!wtimer.out_of_time(turn_time)) {
//        U8 d1m = board.possible_moves[0];
//        U32 d1_idx = linearIndexFromCoordinate(d1m, 0, 0), d2_idx = 0, d3_idx = 0;
//        best_score = 0.0;
//        sim_board = board;
//        simulations++;

//        //selection phase
//        //depth 1
//        double parent_log = log(simulations);
//        for(U8 one : sim_board.possible_moves) {
//            d1_idx = linearIndexFromCoordinate(one, 0, 0);
//            score = (double)d3tree[d1_idx].wins / (double) (d3tree[d1_idx].updates+1) + sqrt(parent_log / (d3tree[d1_idx].selections+1));
////            cerr << "NEXT SCORE: " << score << endl;
//            if(score >= best_score) {
//                best_score = score;
//                d1m = one;
//            }
//        }
//        d1_idx = linearIndexFromCoordinate(d1m, 0, 0);
//        d3tree[d1_idx].selections++;
//        sim_board.makeMove(d1m);

//        U8 d2m = sim_board.possible_moves[0];
//        //depth 2
//        best_score = 0.0;
//        parent_log = log(d3tree[d1_idx].selections);
//        for(U8 two : sim_board.possible_moves) {
//            d2_idx = linearIndexFromCoordinate(d1m, two, 0);
//            score = (double)d3tree[d2_idx].wins / (double) (d3tree[d2_idx].updates+1) + sqrt(parent_log / (d3tree[d2_idx].selections+1));
//            if(score >= best_score) {
//                best_score = score;
//                d2m = two;
//            }
//        }
//        d2_idx = linearIndexFromCoordinate(d1m, d2m, 0);
//        d3tree[d2_idx].selections++;
//        sim_board.makeMove(d2m);

//        U8 d3m = sim_board.possible_moves[0];
//        //depth 3
//        best_score = 0.0;
//        parent_log = log(d3tree[d2_idx].selections);
//        for(U8 three : sim_board.possible_moves) {
//            d3_idx = linearIndexFromCoordinate(d1m, d2m, three);
////            cerr << (int)d1m << "," << (int)d2m << "," << (int)three << " : " << d3_idx << " - " << d3tree.size() << endl;
//            score = (double)d3tree[d3_idx].wins / (double) (d3tree[d3_idx].updates+1) + sqrt(parent_log / (d3tree[d3_idx].selections+1));
//            if(score >= best_score) {
//                best_score = score;
//                d3m = three;
//            }
//        }
//        d3_idx = linearIndexFromCoordinate(d1m, d2m, d3m);
//        d3tree[d3_idx].selections++;
//        sim_board.makeMove(d3m);


//        //playout phase..
//        win_color = sim_board.randomFill();

//        //backprop phase
//        I8 opp_score = WIN_TRANSLATION[opponent][win_color];
//        I8 player_score = WIN_TRANSLATION[player][win_color]; // last move(d3m) is own move(and color)

//        //depth 2 -- opponent move
//        d3tree[d2_idx].update(opp_score);
//        //depth 3 & 1 -- own moves
//        d3tree[d3_idx].update(player_score);
//        d3tree[d1_idx].update(player_score);

//        //AMAF
//        //for all siblings played move update amaf scores
//        U32 amaf_index = 0;
//        for(U8 m : sim_board.possible_moves) {
//            if(sim_board.stones[m] == opponent) {
//                amaf_index = linearIndexFromCoordinate(d1m, m, 0);
//                d3tree[amaf_index].update(opp_score);
//            } else {
//                amaf_index = linearIndexFromCoordinate(m, 0, 0);
//                d3tree[amaf_index].update(player_score);
//                amaf_index = linearIndexFromCoordinate(d1m, d2m, m);
//                d3tree[amaf_index].update(player_score);
//            }
//        }
//    }

//    cerr << "   --- " << simulations << " simulations run." << endl;

//    //TODO output best line including scores/visits

//    U32 sel = 0;

//    best_score = 0;
//    U8 best_move = board.possible_moves[0];
//    for(U8 m : board.possible_moves) {
//        U32 sel_idx = linearIndexFromCoordinate(m, 0, 0);
//        double pscore = (double)d3tree[sel_idx].wins / (double) d3tree[sel_idx].updates;
//        cerr << " # Move: " << (int) m << " Player: " << pscore*100.0 << "% Updates: " << d3tree[sel_idx].updates
//             << " Player Selections: " << d3tree[sel_idx].selections << endl;

//        if(d3tree[sel_idx].selections > sel) {
//            best_score = pscore;
//            sel = d3tree[sel_idx].selections;
//            best_move = m;
//        }
//    }

//    cerr << "---> # Best Move: " << (int) best_move << " Score: " << best_score*100.0 << "% Selections: " << sel << endl;

//    return best_move;
//}

inline
U8 PlainMCEngine::runSim(double remaining_time) {
    wtimer.start();

    double turn_time = remaining_time / 10.0;

    simulations = 0;
    ResultNode results[2][LAST_FIELD+1]; //white,black

    U8 player = board.to_play;
    U8 opponent = FLIP(board.to_play);
    U8 win_color = NO_WIN;
    U8 best_move[2]; //white,black
    U8 next_move[2]; //white,black
    next_move[player] = board.possible_moves[0]; //could crash if there are no more possible moves
    next_move[opponent] = board.possible_moves[1]; //guess: can't happen, game is finished before that state is reached
    double best_score[2]; //white,black
    double uct[2]; //white,black

    cerr << "   --- Run simulation for move no. " << (int)board.next_move << " --- Color = " << (int)player << endl;
    cerr << "   --- Turn time: " << turn_time << " sec." << endl;

    while(!wtimer.out_of_time(turn_time)) {

        results[player][next_move[player]].selections++;
        sim_board = board;
        sim_board.makeMove(next_move[player]);

        //mc simulation
        results[opponent][next_move[opponent]].selections++;
        win_color = sim_board.randomFill(next_move[opponent]);

        simulations++;
        best_score[WHITE] = 0; best_score[BLACK] = 0; uct[WHITE] = 0; uct[BLACK] = 0;


        double simlog = log(simulations);

        for(U8 m : board.possible_moves) {
            U8 stone_color = sim_board.stones[m];
            results[stone_color][m].update(WIN_TRANSLATION[stone_color][win_color]);

            //UCTValue(parent, n) = winrate + sqrt((ln(parent.visits))/(n.nodevisits))
            //selection
            uct[stone_color] = (double)results[stone_color][m].wins / (double) results[stone_color][m].updates + sqrt(simlog / (results[stone_color][m].selections+1));

            if(uct[stone_color] > best_score[stone_color]) {
                next_move[stone_color] = m;
                best_score[stone_color] = uct[stone_color];
            }
        }        
    }

    cerr << "   --- " << simulations << " simulations run." << endl;

    U32 sel = 0;

    for(U8 m : board.possible_moves) {
        double pscore = (double)results[player][m].wins / (double) results[player][m].updates;
        double oscore = (double)results[opponent][m].wins / (double) results[opponent][m].updates;
        cerr << " # Move: " << (int) m << " Player: " << pscore*100.0 << "% Opponent: " << oscore*100.0
             << "% Updates: " << results[player][m].updates
             << " Player Selections: " << results[player][m].selections << " Opponent Selections: " << results[opponent][m].selections << endl;

        if(results[player][m].selections > sel) {
            best_score[player] = pscore;
            sel = results[player][m].selections;
            best_move[player] = m;
        }
    }

    cerr << "---> # Best Move: " << (int) best_move[player] << " Score: " << best_score[player]*100.0 << "% Selections: " << sel << endl;

    return best_move[player];
}

inline
void PlainMCEngine::runAnalysis(double verbose_time) {
    simulations = 0;
    ResultNode results[2][LAST_FIELD+1]; //white,black

    U8 player = board.to_play;
    U8 opponent = FLIP(board.to_play);
    U8 win_color = NO_WIN;
    U8 best_move[2]; //white,black
    U8 next_move[2]; //white,black
    next_move[player] = board.possible_moves[0]; //could crash if there are no more possible moves
    next_move[opponent] = board.possible_moves[1]; //guess: can't happen, game is finished before that state is reached
    double best_score[2]; //white,black
    double uct[2]; //white,black

    cerr << "   --- Run simulation for move no. " << (int)board.next_move << " --- Color = " << (int)player << endl;
    cerr << "   --- Output time: " << verbose_time << " sec." << endl;

    while(true) { //endless analysis
        wtimer.start();
        while(!wtimer.out_of_time(verbose_time)) {

            results[player][next_move[player]].selections++;
            sim_board = board;
            sim_board.makeMove(next_move[player]);

            //TODO -- uct only selects from stones played by the color...
    //        if(next_move[opponent] == next_move[player]) {
    //            //if this happens opponent move is chosen randomly
    //            cerr << "BANGBANG" << endl;
    //        }

            //mc simulation
            results[opponent][next_move[opponent]].selections++;
            win_color = sim_board.randomFill(next_move[opponent]);

            simulations++;
            best_score[WHITE] = 0; best_score[BLACK] = 0; uct[WHITE] = 0; uct[BLACK] = 0;


            double simlog = log(simulations);

            for(U8 m : board.possible_moves) {
                U8 stone_color = sim_board.stones[m];
                results[stone_color][m].update(WIN_TRANSLATION[stone_color][win_color]);

                //UCTValue(parent, n) = winrate + sqrt((ln(parent.visits))/(n.nodevisits))
                //selection
                uct[stone_color] = (double)results[stone_color][m].wins / (double) results[stone_color][m].updates + sqrt(simlog / (results[stone_color][m].selections+1));

                if(uct[stone_color] > best_score[stone_color]) {
                    next_move[stone_color] = m;
                    best_score[stone_color] = uct[stone_color];
                }
            }
        }

        cerr << "   --- " << simulations << " simulations run." << endl;

        U32 sel = 0;

        for(U8 m : board.possible_moves) {
            double pscore = (double)results[player][m].wins / (double) results[player][m].updates;
            double oscore = (double)results[opponent][m].wins / (double) results[opponent][m].updates;
            cerr << " # Move: " << (int) m << " Player: " << pscore*100.0 << "% Opponent: " << oscore*100.0
                 << "% Updates: " << results[player][m].updates
                 << " Player Selections: " << results[player][m].selections << " Opponent Selections: " << results[opponent][m].selections << endl;

            if(results[player][m].selections > sel) {
                best_score[player] = pscore;
                sel = results[player][m].selections;
                best_move[player] = m;
            }
        }

        cerr << "---> # Best Move: " << (int) best_move[player] << " Score: " << best_score[player]*100.0 << "% Selections: " << sel << endl;
    }
}

#endif // PLAINMC_HPP
