
#include <iostream>
using namespace std;

#include "FastBoard.hpp"
#include "PlainMC.hpp"


int main() {
    srand(time(NULL));
    fast_srand(time(NULL));


//    cerr << "TODO: test FastBoard validity" << endl;
//    FastBoard fb;
//    U8 win = fb.randomFill();
//    cerr << "WIN: " << (int)win << endl;
//    vector<U8> white_stones;
//    vector<U8> black_stones;
//    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
//        if(i == 0) {
//            cerr << "DUDE" << endl;
//        }
//        if(fb.stones[i] == WHITE) {
//            white_stones.push_back(i);
//        } else if(fb.stones[i] == BLACK) {
//            black_stones.push_back(i);
//        } else {
//            cerr << "ERROR YO" << endl;
//        }
//    }

//    cerr << "stone count white: " << white_stones.size() << endl;
//    cerr << "stone count black: " << black_stones.size() << endl;

//    cerr << "Game: (";
////    for(int i = FIRST_FIELD; i <= LAST_FIELD / 2; i++) {
//    while(!white_stones.empty()) {
//        U8 w = white_stones.back();
//        white_stones.pop_back();
//        U8 b = black_stones.back();
//        black_stones.pop_back();

//        if(w == 0) {
//            cerr << endl << "DUDE WHITE: " << w << endl;
//        }
//        if(b == 0) {
//            cerr << endl << "DUDE BLACK: " << b << endl;
//        }
//        cerr << (int)w << ",";
//        cerr << (int)b;
//        if(!white_stones.empty()) {
//            cout << ",";
//        } else {
//            cout << ")";
//        }
//    }
//    cout << endl;
//    return 1;


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
            next_move = pmc_engine.runSim(TOTAL_MAX_TIME - used_time);
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

            //TODO
            //last move may be color flip
//            if(last_move == -1) {
//                pmc_engine.makePermanentMove(FLIP_MOVE);
//            } else {
                pmc_engine.makePermanentMove(last_move);
//            }

            next_move = pmc_engine.runSim(TOTAL_MAX_TIME - used_time);
            pmc_engine.makePermanentMove(next_move);
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
