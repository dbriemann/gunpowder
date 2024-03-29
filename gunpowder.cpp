
#include <iostream>
using namespace std;

//the following includes will be replaced by the actual code
//with the help of a script, because tournament rules allow
//only one single file as upload.
//---REPLACE-BEGIN---
#include "definitions.hpp"
#include "Timer.hpp"
#include "FastBoard.hpp"
#include "PlainMC.hpp"
//---REPLACE-END---


int main() {
    srand(time(NULL));
    fast_srand(time(NULL));


    cerr << "R gunpowder" << VERSION_STR << endl;
    cerr.flush();

    PlainMCEngine pmc_engine;
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
            //first move is from book always
            int r = rand() % NO_1ST_MOVES;
            next_move = FIRST_MOVES[r];
//            next_move = pmc_engine.runSim(TOTAL_MAX_TIME - used_time);
//            next_move = pmc_engine.d3Sim(TOTAL_MAX_TIME - used_time);
            pmc_engine.makePermanentMove(next_move);

            used_time += wtimer.get_elapsed();

            cout << next_move << endl;
            cout.flush();
        } else if(in_command == "Quit") {
            //exit program
            break;
        } else {
            //we receive the last move played
            last_move = atoi(in_command.c_str());
            next_move = NONE;

            //last move may be color flip
            if(last_move == -1) {
                pmc_engine.makePermanentMove(FLIP_MOVE);
            } else {
                pmc_engine.makePermanentMove(last_move);
            }

            if(pmc_engine.board.next_move == 2) {
                //test book for second move
                next_move = SECOND_MOVES[last_move];
            }

            if(next_move != FLIP_MOVE) {
                next_move = pmc_engine.runSim(TOTAL_MAX_TIME - used_time);
    //            next_move = pmc_engine.d3Sim(TOTAL_MAX_TIME - used_time);
                pmc_engine.makePermanentMove(next_move);
                used_time += wtimer.get_elapsed();
            }

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
