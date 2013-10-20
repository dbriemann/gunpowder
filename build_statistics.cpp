

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
#include "MCTS.hpp"

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

//    cerr << sizeof(U32) << " "
//         << sizeof(U32) << " "
//         << sizeof(U8)  << " "
//         << sizeof(U8)  << " "
//         << sizeof(vector<U8>) << " "
//         << sizeof(MCTNode *)  << " "
//         << sizeof(unordered_map<U8, MCTNode *>) << endl;

    MCTSEngine mcts;

    mcts.makePermanentMove(46);

//    mcts.analysis(10.0);

    return 0;
}
