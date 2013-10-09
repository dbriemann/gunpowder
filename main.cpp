
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
#include "Engine.hpp"
//---REPLACE-END---


/*
 * gunpowder main
 */
int main() {
    srand(time(NULL));

    Engine game_engine;
    WallTimer wtimer;
    double used_time = 0.0;

    string in_command = "";
    int last_move = 0;
    int next_move = 0;

    //TODO DEBUG
    cerr << "R gunpowder" << VERSION_STR << endl;
    cerr.flush();

    //process commands
    while(true) {
        cin >> in_command;
        wtimer.start();

        used_time += LOST_TIME_MALUS;

        if(in_command == "Start") {
            //the first command received decides which color we play
            //we are white and begin the game
            game_engine.color = WHITE;

            next_move = game_engine.getBestMove(TOTAL_MAX_TIME - used_time);
            game_engine.permanentMove(next_move);

            used_time += wtimer.get_elapsed();

            cout << next_move << endl;
            cout.flush();
        } else if(in_command == "Quit") {
            //exit program
            break;
        } else {
            //we receive the last move played
            last_move = atoi(in_command.c_str());

            //last move may be color switch
            if(game_engine.board.next_move == 2 && last_move == -1) {
                game_engine.colorFlip();
            } else {
                game_engine.permanentMove(last_move);
            }

            next_move = game_engine.getBestMove(TOTAL_MAX_TIME - used_time);
            game_engine.permanentMove(next_move);
            used_time += wtimer.get_elapsed();

            if(next_move == FLIP_MOVE) {
                cerr << "*** MAKE FLIPPER..." << endl;
                cout << "-1" << endl;
            } else {
                cerr << "NO FLIP..." << endl;
                cout << next_move << endl;
            }

            cout.flush();
        }

        WallTimer::print(used_time);
        cerr << endl;
    }

    return 0;
}
