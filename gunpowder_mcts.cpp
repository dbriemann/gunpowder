
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
#include <string>
using namespace std;

//the following includes will be replaced by the actual code
//with the help of a script, because tournament rules allow
//only one single file as upload.
//---REPLACE-BEGIN---
#include "Timer.hpp"
#include "definitions.hpp"
#include "Board.hpp"
#include "MCTS.hpp"
//#include "Engine.hpp"
//---REPLACE-END---


/*
 * gunpowder main
 */
int main() {
    srand(time(NULL));

    cerr << "R gunpowder" << VERSION_STR << endl;
    cerr.flush();

    MCTSEngine mcts;
    WallTimer wtimer;
    double used_time = 0.0;

    string in_command = "";
    int last_move = 0;
    int next_move = 0;



    //process commands
    while(true) {
        cin >> in_command;
        wtimer.start();

        used_time += LOST_TIME_MALUS;

        if(in_command == "Start") {
            next_move = mcts.runSim(TOTAL_MAX_TIME - used_time);
            mcts.makePermanentMove(next_move);

            used_time += wtimer.get_elapsed();

            cout << next_move << endl;
            cout.flush();
        } else if(in_command == "Quit") {
            //exit program
            break;
        } else {
            //we receive the last move played
            last_move = atoi(in_command.c_str());

            //last move may be color flip
//            if(last_move == -1) {
//                mcts.makePermanentMove(FLIP_MOVE);
//            } else {
                mcts.makePermanentMove(last_move);
//            }

            next_move = mcts.runSim(TOTAL_MAX_TIME - used_time);
            mcts.makePermanentMove(next_move);
            used_time += wtimer.get_elapsed();

            if(next_move == FLIP_MOVE) {
                cout << "-1" << endl;
            } else {
                cout << next_move << endl;
            }

            cout.flush();
        }

        WallTimer::print(used_time);
        cerr << endl;
    }


    return 0;
}

