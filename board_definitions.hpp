#ifndef BOARD_DEFINITIONS_HPP
#define BOARD_DEFINITIONS_HPP

#include "definitions.hpp"

#define FIRST_FIELD 1
#define LAST_FIELD 106
#define MAX_NEIGHBORS 6
#define EDGES 5
#define NONE 0

//array defines the neighborhood of every field, 0==NONE is not on the board
const U8 NEIGHBORS[LAST_FIELD+1][6] = {
    {NONE,NONE,NONE,NONE,NONE,NONE},    //0
    {2,3,4,NONE,NONE,NONE},             //1
    {1,3,5,6,NONE,NONE},                //2
    {1,2,4,6,7,8},                      //3
    {1,3,8,9,NONE,NONE},                //4
    {2,6,10,11,NONE,NONE},              //5
    {2,3,5,7,11,12},                    //6
    {3,6,8,12,13,14},                   //7
    {3,4,7,9,14,15},                    //8
    {4,8,15,16,NONE,NONE},              //9
    {5,11,17,18,NONE,NONE},             //10
    {5,6,10,12,18,19},                  //11
    {6,7,11,13,19,20},                  //12
    {7,12,14,20,21,22},                 //13
    {7,8,13,15,22,23},                  //14
    {8,9,14,16,23,24},                  //15
    {9,15,24,25,NONE,NONE},             //16
    {10,18,26,27,NONE,NONE},            //17
    {10,11,17,19,27,28},                //18
    {11,12,18,20,28,29},                //19
    {12,13,19,21,29,30},                //20
    {13,20,22,30,31,32},                //21
    {13,14,21,23,32,33},                //22
    {14,15,22,24,33,34},                //23
    {15,16,23,25,34,35},                //24
    {16,24,35,36,NONE,NONE},            //25
    {17,27,37,38,NONE,NONE},            //26
    {17,18,26,28,38,39},                //27
    {18,19,27,29,39,40},                //28
    {19,20,28,30,40,41},                //29
    {20,21,29,31,41,42},                //30
    {21,30,32,42,43,44},                //31
    {21,22,31,33,44,45},                //32
    {22,23,32,34,45,46},                //33
    {23,24,33,35,46,47},                //34
    {24,25,34,36,47,48},                //35
    {25,35,48,49,NONE,NONE},            //36
    {26,38,50,NONE,NONE,NONE},          //37
    {26,27,37,39,50,51},                //38
    {27,28,38,40,51,52},                //39
    {28,29,39,41,52,53},                //40
    {29,30,40,42,53,54},                //41
    {30,31,41,43,54,55},                //42
    {31,42,44,55,56,NONE},              //43 //CENTER
    {31,32,43,45,56,57},                //44
    {32,33,44,46,57,58},                //45
    {33,34,45,47,58,59},                //46
    {34,35,46,48,59,60},                //47
    {35,36,47,49,60,61},                //48
    {36,48,61,NONE,NONE,NONE},          //49
    {37,38,51,62,NONE,NONE},            //50
    {38,39,50,52,62,63},                //51
    {39,40,51,53,63,64},                //52
    {40,41,52,54,64,65},                //53
    {41,42,53,55,65,66},                //54
    {42,43,54,56,66,67},                //55
    {43,44,55,57,67,68},                //56
    {44,45,56,58,68,69},                //57
    {45,46,57,59,69,70},                //58
    {46,47,58,60,70,71},                //59
    {47,48,59,61,71,72},                //60
    {48,49,60,72,NONE,NONE},            //61
    {50,51,63,73,NONE,NONE},            //62
    {51,52,62,64,73,74},                //63
    {52,53,63,65,74,75},                //64
    {53,54,64,66,75,76},                //65
    {54,55,65,67,76,77},                //66
    {55,56,66,68,77,78},                //67
    {56,57,67,69,78,79},                //68
    {57,58,68,70,79,80},                //69
    {58,59,69,71,80,81},                //70
    {59,60,70,72,81,82},                //71
    {60,61,71,82,NONE,NONE},            //72
    {62,63,74,83,NONE,NONE},            //73
    {63,64,73,75,83,84},                //74
    {64,65,74,76,84,85},                //75
    {65,66,75,77,85,86},                //76
    {66,67,76,78,86,87},                //77
    {67,68,77,79,87,88},                //78
    {68,69,78,80,88,89},                //79
    {69,70,79,81,89,90},                //80
    {70,71,80,82,90,91},                //81
    {71,72,81,91,NONE,NONE},            //82
    {73,74,84,92,NONE,NONE},            //83
    {74,75,83,85,92,93},                //84
    {75,76,84,86,93,94},                //85
    {76,77,85,87,94,95},                //86
    {77,78,86,88,95,96},                //87
    {78,79,87,89,96,97},                //88
    {79,80,88,90,97,98},                //89
    {80,81,89,91,98,99},                //90
    {81,82,90,99,NONE,NONE},            //91
    {83,84,93,100,NONE,NONE},           //92
    {84,85,92,94,100,101},              //93
    {85,86,93,95,101,102},              //94
    {86,87,94,96,102,103},              //95
    {87,88,95,97,103,104},              //96
    {88,89,96,98,104,105},              //97
    {89,90,97,99,105,106},              //98
    {90,91,98,106,NONE,NONE},           //99
    {92,93,101,NONE,NONE,NONE},         //100
    {93,94,100,102,NONE,NONE},          //101
    {94,95,101,103,NONE,NONE},          //102
    {95,96,102,104,NONE,NONE},          //103
    {96,97,103,105,NONE,NONE},          //104
    {97,98,104,106,NONE,NONE},          //105
    {98,99,105,NONE,NONE,NONE}          //106
};

//edge masks
U8 EDGE_MASKS[EDGES+1] = {
    NONE, 0x1, 0x2, 0x4, 0x8, 0x10
};

//which edge does a field belong to (0 == NONE)
U8 FIELD_EDGE_RELATION[LAST_FIELD+1] = {
    EDGE_MASKS[NONE],                   //0
    EDGE_MASKS[1] | EDGE_MASKS[5],      //1
    EDGE_MASKS[5],                      //2
    EDGE_MASKS[NONE],                   //3
    EDGE_MASKS[1],                      //4
    EDGE_MASKS[5],                      //5
    EDGE_MASKS[NONE],                   //6
    EDGE_MASKS[NONE],                   //7
    EDGE_MASKS[NONE],                   //8
    EDGE_MASKS[1],                      //9
    EDGE_MASKS[5],                      //10
    EDGE_MASKS[NONE],                   //11
    EDGE_MASKS[NONE],                   //12
    EDGE_MASKS[NONE],                   //13
    EDGE_MASKS[NONE],                   //14
    EDGE_MASKS[NONE],                   //15
    EDGE_MASKS[1],                      //16
    EDGE_MASKS[5],                      //17
    EDGE_MASKS[NONE],                   //18
    EDGE_MASKS[NONE],                   //19
    EDGE_MASKS[NONE],                   //20
    EDGE_MASKS[NONE],                   //21
    EDGE_MASKS[NONE],                   //22
    EDGE_MASKS[NONE],                   //23
    EDGE_MASKS[NONE],                   //24
    EDGE_MASKS[1],                      //25
    EDGE_MASKS[5],                      //26
    EDGE_MASKS[NONE],                   //27
    EDGE_MASKS[NONE],                   //28
    EDGE_MASKS[NONE],                   //29
    EDGE_MASKS[NONE],                   //30
    EDGE_MASKS[NONE],                   //31
    EDGE_MASKS[NONE],                   //32
    EDGE_MASKS[NONE],                   //33
    EDGE_MASKS[NONE],                   //34
    EDGE_MASKS[NONE],                   //35
    EDGE_MASKS[1],                      //36
    EDGE_MASKS[5] | EDGE_MASKS[4],      //37
    EDGE_MASKS[NONE],                   //38
    EDGE_MASKS[NONE],                   //39
    EDGE_MASKS[NONE],                   //40
    EDGE_MASKS[NONE],                   //41
    EDGE_MASKS[NONE],                   //42
    EDGE_MASKS[NONE],                   //43 //CENTER
    EDGE_MASKS[NONE],                   //44
    EDGE_MASKS[NONE],                   //45
    EDGE_MASKS[NONE],                   //46
    EDGE_MASKS[NONE],                   //47
    EDGE_MASKS[NONE],                   //48
    EDGE_MASKS[2] | EDGE_MASKS[1],      //49
    EDGE_MASKS[4],                      //50
    EDGE_MASKS[NONE],                   //51
    EDGE_MASKS[NONE],                   //52
    EDGE_MASKS[NONE],                   //53
    EDGE_MASKS[NONE],                   //54
    EDGE_MASKS[NONE],                   //55
    EDGE_MASKS[NONE],                   //56
    EDGE_MASKS[NONE],                   //57
    EDGE_MASKS[NONE],                   //58
    EDGE_MASKS[NONE],                   //59
    EDGE_MASKS[NONE],                   //60
    EDGE_MASKS[2],                      //61
    EDGE_MASKS[4],                      //62
    EDGE_MASKS[NONE],                   //63
    EDGE_MASKS[NONE],                   //64
    EDGE_MASKS[NONE],                   //65
    EDGE_MASKS[NONE],                   //66
    EDGE_MASKS[NONE],                   //67
    EDGE_MASKS[NONE],                   //68
    EDGE_MASKS[NONE],                   //69
    EDGE_MASKS[NONE],                   //70
    EDGE_MASKS[NONE],                   //71
    EDGE_MASKS[2],                      //72
    EDGE_MASKS[4],                      //73
    EDGE_MASKS[NONE],                   //74
    EDGE_MASKS[NONE],                   //75
    EDGE_MASKS[NONE],                   //76
    EDGE_MASKS[NONE],                   //77
    EDGE_MASKS[NONE],                   //78
    EDGE_MASKS[NONE],                   //79
    EDGE_MASKS[NONE],                   //80
    EDGE_MASKS[NONE],                   //81
    EDGE_MASKS[2],                      //82
    EDGE_MASKS[4],                      //83
    EDGE_MASKS[NONE],                   //84
    EDGE_MASKS[NONE],                   //85
    EDGE_MASKS[NONE],                   //86
    EDGE_MASKS[NONE],                   //87
    EDGE_MASKS[NONE],                   //88
    EDGE_MASKS[NONE],                   //89
    EDGE_MASKS[NONE],                   //90
    EDGE_MASKS[2],                      //91
    EDGE_MASKS[4],                      //92
    EDGE_MASKS[NONE],                   //93
    EDGE_MASKS[NONE],                   //94
    EDGE_MASKS[NONE],                   //95
    EDGE_MASKS[NONE],                   //96
    EDGE_MASKS[NONE],                   //97
    EDGE_MASKS[NONE],                   //98
    EDGE_MASKS[2],                      //99
    EDGE_MASKS[4] | EDGE_MASKS[3],      //100
    EDGE_MASKS[3],                      //101
    EDGE_MASKS[3],                      //102
    EDGE_MASKS[3],                      //103
    EDGE_MASKS[3],                      //104
    EDGE_MASKS[3],                      //105
    EDGE_MASKS[3] | EDGE_MASKS[2]       //106
};



void testNeighborhood() {
    bool mutuality = false;
    U8 neighbor = 0;
    U8 reverse_neighbor = 0;

    for(int field = FIRST_FIELD; field <= LAST_FIELD; field++) {
        cout << "Field: " << field << endl;

        for(int n = 0; n < MAX_NEIGHBORS; n++) {
            neighbor = NEIGHBORS[field][n];
            cout << "    Neighbor: " << (int)neighbor << endl;

            if(neighbor != NONE) {
                mutuality = false;
                //test if neighborhood is mutual
                for(int rn = 0; rn < MAX_NEIGHBORS; rn++) {
                    reverse_neighbor = NEIGHBORS[neighbor][rn];
                    cout << "        Reverse Neighbor: " << (int)reverse_neighbor << endl;

                    if(reverse_neighbor != NONE) {
                        //cout << (int) reverse_neighbor << endl;
                        if(reverse_neighbor == field) {
                            mutuality = true;
                            break;
                        }
                    } else {
                        break;
                    }
                }

                if(!mutuality) {
                    cout << "MUTUALITY FAIL: " << field << " <-> " << (int)neighbor << endl;
                    return;
                }
            } else {
                break;
            }
        }
    }

    cout << "TESTS PASSED" << endl;
}

#endif // BOARD_DEFINITIONS_HPP
