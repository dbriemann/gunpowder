

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
#include <iomanip>
#include <stdint.h>
#include <ctime>
#include <map>
using namespace std;

#include "Timer.hpp"
#include "definitions.hpp"
#include "Board.hpp"
#include "Engine.hpp"

U8 EQUI_REPS[LAST_FIELD+1] = {
    NONE,                           //0
    1,        //1
    2,                     //2
    3,                           //3
    2,                     //4
    5,                     //5
    6,                           //6
    7,                           //7
    6,                           //8
    5,                     //9
    10,                     //10
    11,                           //11
    12,                           //12
    13,                           //13
    12,                           //14
    11,                           //15
    10,                     //16
    5,                     //17
    11,                           //18
    19,                           //19
    20,                           //20
    21,                           //21
    20,                           //22
    19,                           //23
    11,                           //24
    5,                     //25
    2,                     //26
    6,                           //27
    12,                           //28
    20,                           //29
    30,                           //30
    31,                           //31
    30,                           //32
    20,                           //33
    12,                           //34
    6,                           //35
    2,                     //36
    1,        //37
    3,                           //38
    7,                           //39
    13,                           //40
    21,                           //41
    31,                           //42
    43,                           //43 //CENTER
    31,                           //44
    21,                           //45
    13,                           //46
    7,                           //47
    3,                           //48
    1,        //49
    2,                     //50
    6,                           //51
    12,                           //52
    20,                           //53
    30,                           //54
    31,                           //55
    31,                           //56
    30,                           //57
    20,                           //58
    12,                           //59
    6,                           //60
    2,                     //61
    5,                     //62
    11,                           //63
    19,                           //64
    20,                           //65
    21,                           //66
    30,                           //67
    21,                           //68
    20,                           //69
    19,                           //70
    11,                           //71
    5,                     //72
    10,                     //73
    11,                           //74
    12,                           //75
    13,                           //76
    20,                           //77
    20,                           //78
    13,                           //79
    12,                           //80
    11,                           //81
    10,                     //82
    5,                     //83
    6,                           //84
    7,                           //85
    12,                           //86
    19,                           //87
    12,                           //88
    7,                           //89
    6,                           //90
    5,                     //91
    2,                     //92
    3,                           //93
    6,                           //94
    11,                           //95
    11,                           //96
    6,                           //97
    3,                           //98
    2,                     //99
    1,        //100
    2,                     //101
    5,                     //102
    10,                     //103
    5,                     //104
    2,                     //105
    1         //106
};

/*
 * gunpowder main
 */
int main() {
    srand(time(NULL));

    Board origin_board;
    Board sim_board;
    vector<pair<int,double>> field_scores;
    map<U8, pair<double, double>> sym_results;

    U64 amounts[LAST_FIELD+1] = {0};
    int64_t results[LAST_FIELD+1] = {0};

    U64 games_played = 0ULL;
    I8 win = 0;
    U8 move_history[LAST_FIELD+1] = {0};

    //make custom move(s)
    //origin_board.makeMove(43);

    while(true) {
        //for every possible first move
        for(U8 pfm : origin_board.possible_moves)  {
//        for(int pfm = FIRST_FIELD; pfm <= LAST_FIELD; pfm++) {
            win = NONE;
            sim_board = origin_board;

            move_history[sim_board.next_move] = pfm;
            sim_board.makeMove(pfm);

            //run random simulation and store result
            while(win == NONE) {
                U8 moves = sim_board.possible_moves.size();
                U8 rand_move_idx = rand() % moves;
                U8 rand_move = sim_board.possible_moves[rand_move_idx];

                move_history[sim_board.next_move] = rand_move;
                win = sim_board.makeMove(rand_move);

//                    for(int i = origin_board.next_move; i < sim_board.next_move; i+=2) {
//                        results[move_history[i]] += win;
//                        amounts[move_history[i]]++;
//                    }
            }
//            cout << (int) pfm << "   " << (int)EQUI_REPS[pfm] << endl;
            results[pfm] += win;
            amounts[pfm]++;
        }

        games_played++;

        if(games_played % 10000 == 0) {
            cerr << endl << "# Results for " << games_played << " games: " << endl;

            //score_to_field.clear();
            field_scores.clear();
            sym_results.clear();

            for(int eee = FIRST_FIELD; eee <= LAST_FIELD; eee++) {
                if(amounts[eee] > 0) {
                    double perc = ((((double)amounts[eee] - (double)results[eee]) / 2.0) + (double)results[eee]) / (double)amounts[eee];
                    int aaa = EQUI_REPS[eee];
                    if(sym_results.find(aaa) == sym_results.end()) {
                        sym_results[aaa] = make_pair(perc, 1);
                    } else {
                        sym_results[aaa].first += perc;
                        sym_results[aaa].second++;
                    }
//                    field_scores.push_back(make_pair(eee, perc));
                }
            }

            for(auto entry : sym_results) {
                field_scores.push_back(make_pair(entry.first, entry.second.first / entry.second.second));
            }

            //sort scores
            std::sort(field_scores.begin(), field_scores.end(), [](const pair<int, double>& lhs, const pair<int, double>& rhs) {
                return lhs.second > rhs.second;
            } );

            //output scores
            for(auto p : field_scores) {
                cerr << fixed << p.second << " : " << p.first << endl;
            }
        }
    }

    cout << endl;

    return 0;
}
