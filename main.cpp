
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

//the following includes will be replaced by the actual code
//with the help of a script, because tournament rules allow
//only one single file as upload.
//---REPLACE-BEGIN---
#include "timer.hpp"
#include "definitions.hpp"
#include "Board.hpp"
//---REPLACE-END---


/*
 * gunpowder main
 */
int main() {
    srand(time(NULL));

    Board origin_board;
    Board sim_board;

    while(true) {
        //reset sim board to origin state
        sim_board = origin_board;
        U8 moves = sim_board.possible_moves.size();
    }
    cout << endl;

    return 0;
}
