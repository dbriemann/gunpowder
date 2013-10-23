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
    if(idx == FLIP_MOVE) {
        board.colorFlip();
    } else {
        board.makeMove(idx);
    }
}

inline
U8 PlainMCEngine::runSim(double remaining_time) {
    wtimer.start();

    double turn_time = remaining_time / 10.0;

    simulations = 0;
    ResultNode results[2][FLIP_MOVE+1]; //white,black

    U8 player = board.to_play;
    U8 opponent = FLIP(board.to_play);
    U8 win_color = NO_WIN;
    U8 best_move[2]; //white,black
    U8 next_move[2]; //white,black
    next_move[player] = board.possible_moves[0]; //could crash if there are no more possible moves
    next_move[opponent] = board.possible_moves[1]; //guess: can't happen, game is finished before that state is reached
    double best_score[2]; //white,black
    double uct[2]; //white,black

    cerr << "   --- Run simulation for move no. " << (int)board.next_move << " --- Color = " << player << endl;
    cerr << "   --- Turn time: " << turn_time << " sec." << endl;

    while(!wtimer.out_of_time(turn_time)) {

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

    return best_move[player];
}

#endif // PLAINMC_HPP
