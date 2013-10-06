#ifndef BOARD_HPP
#define BOARD_HPP

#include "string.h"
#include <vector>
#include <algorithm>
using namespace std;

#include "definitions.hpp"
#include "Group.hpp"

struct Board {
    U8 move_nr;
    U8 to_play;
    U8 score[2];

    U8 fields[LAST_FIELD+1];
    U8 field_to_group[LAST_FIELD+1];
    vector<Group> groups[2];
    vector<U8> possible_moves;

    Board();
    Board & operator=(const Board &other);

    void makeMove(U8 idx);
};

Board::Board() {
    move_nr = 0;
    to_play = WHITE;
    groups[WHITE] = vector<Group>();
    groups[BLACK] = vector<Group>();
    score[WHITE] = 0;
    score[BLACK] = 0;
    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
        possible_moves.push_back(i);
        fields[i] = EMPTY;
        field_to_group[i] = NONE;
    }
    fields[0] = EMPTY;
    field_to_group[0] = NONE;
}

Board & Board::operator=(const Board &other) {


    return *this;
}

void Board::makeMove(U8 idx) {
    bool is_new_group = true;

    //1. add stone to fields array
    fields[idx] = to_play;

    //3. check if neighbors of the same color exist
    for(int i = 0; i < 6 && NEIGHBORS[idx][i] != NONE; i++) {
        U8 neighbor_idx = NEIGHBORS[idx][i];
        if(fields[neighbor_idx] == to_play) {
            if(is_new_group) {
                groups[to_play][field_to_group[neighbor_idx]].addMember(idx);
            } else {
                groups[to_play][field_to_group[neighbor_idx]].merge(groups[to_play][field_to_group[idx]]);
            }
            field_to_group[idx] = field_to_group[neighbor_idx];
            is_new_group = false;
        }
    }

    if(is_new_group) {
        groups[to_play].push_back(Group());
        groups[to_play].back().addMember(idx);
        field_to_group[idx] = groups[to_play].size() - 1;
    }


    //remove from possible moves
    //TODO. SUX
    for(auto iter = possible_moves.begin(); iter != possible_moves.end(); iter++) {
        if(*iter == idx) {
            possible_moves.erase(iter);
            break;
        }
    }
    //switch color
    to_play = FLIP(to_play);
    //
    move_nr++;
}

#endif // BOARD_HPP
