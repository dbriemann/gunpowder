

/*
 * Author: David Briemann
 *
 * Entry for the CodeCup 2014 coding contest.
 * A computer player for the game 'Poly-Y'.
 *
 * Description:
 * Program has to play the game of 'Poly-Y' against other computer players.
 * Communication is done via stdin/-out and a manager program.
 *
 */

#include <iostream>
#include <stdint.h>
#include <ctime>
using namespace std;

#include "Timer.hpp"
#include "definitions.hpp"
#include "Board.hpp"
#include "Engine.hpp"



/*
 * gunpowder main
 */
int main() {
    srand(time(NULL));

    Board origin_board;
    Board sim_board;

    int64_t results[LAST_FIELD+1] = {0};
    U64 games_played = 0ULL;
    U8 score = 0;

    while(true) {
        score = 0;

        //for every possible first move
        for(int pfm = FIRST_FIELD; pfm <= LAST_FIELD; pfm++) {
            sim_board = origin_board;
            sim_board.makeMove(pfm);

            //run random simulation and store result
            while(true) {
                U8 moves = sim_board.possible_moves.size();
                if(moves == 0) {
                    cerr << "BAAAAD" << endl;
                    return 1;
                }

                U8 rand_move_idx = rand() % moves;
                U8 rand_move = sim_board.possible_moves[rand_move_idx];
                score = sim_board.makeMove(rand_move);
                if(score >= 3) {
                    if(sim_board.to_play == WHITE) {
                        //black wins
                        results[pfm] -= 1;
                    } else {
                        results[pfm] += 1;
                    }

                    break;
                }
            }
        }

        games_played++;

        if(games_played % 1000 == 0) {
            cerr << "# Results for " << games_played << " games: " << endl;
            for(auto v : results) {
                cerr << v << ", ";
            }
            cerr << endl;
        }
    }

    cout << endl;

    return 0;
}
