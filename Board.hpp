#ifndef BOARD_HPP
#define BOARD_HPP

#include "string.h"
#include <vector>
#include <algorithm>
using namespace std;

#include "definitions.hpp"

struct Board {
    U8 move_nr;
    U8 to_play;
    U8 scores[2];
    U8 fields[LAST_FIELD+1];
    U8 field_to_group[LAST_FIELD+1];
    U8 group_masks[2*(LAST_FIELD+1)];

    vector<U8> possible_moves;

    Board();
    Board & operator=(const Board &other);

    void makeMove(U8 idx);
    void setScore();
};

Board::Board() {
    LOGERR("Init Board");
    scores[WHITE] = scores[BLACK] = 0;
    move_nr = 0;
    to_play = WHITE;
    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
        possible_moves.push_back(i);
        fields[i] = EMPTY;
        field_to_group[i] = NONE;
        group_masks[i] = NONE;
    }
    fields[0] = EMPTY;
    field_to_group[0] = NONE;
    group_masks[0] = NONE;
}

Board & Board::operator=(const Board &other) {
    LOGERR("Copy Board");
    //copy static memory blocks together
    memcpy(this, &other, (1 + 1 + 2 + (4*(LAST_FIELD+1)) ) * sizeof(U8));
    //copy dynamic memory stuff
    possible_moves = other.possible_moves;

    return *this;
}

void Board::setScore() {
    LOGERR("Set Score");
    //U8 running_mask = NONE;
    //U8 cur_mask = NONE;
    U8 mask = NONE;
    U8 score = 0;

    //sets score for current player(to_play)
    for(int i = GROUPS_START[to_play]; i < GROUPS_END[to_play]; i++) {
        mask = group_masks[i];
        //cur_mask |= mask;
        //if(cur_mask != running_mask) {
            //ignore duplicate masks
        score += SCORE_LOOKUP[mask];
            //running_mask = cur_mask;
        //}
    }
}


#endif // BOARD_HPP
