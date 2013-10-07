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
        group_masks[i+GROUPS_START[BLACK]] = NONE;
    }
    fields[0] = EMPTY;
    field_to_group[0] = NONE;
    group_masks[0] = NONE;
    group_masks[GROUPS_START[BLACK]] = NONE;
}

Board & Board::operator=(const Board &other) {
    LOGERR("Copy Board");
    //copy static memory blocks together
    memcpy(this, &other, (1 + 1 + 2 + (4*(LAST_FIELD+1)) ) * sizeof(U8));
    //copy dynamic memory stuff
    possible_moves = other.possible_moves;

    return *this;
}

void Board::makeMove(U8 idx) {
    LOGERR("Make Move");
    //1. add stone to board
    fields[idx] = to_play;
    field_to_group[idx] = idx;
    //edge mask for the current stone is just a lookup
    group_masks[GROUPS_START[to_play] + idx] = FIELD_EDGE_RELATION[idx];

    //locals
    bool merge_occured = false;
    U8 neighbor_idx = NONE;
    U8 neighbor_val = EMPTY;
    U8 ni = 0;

    //2. check neighbor fields
    for(ni = 0, neighbor_idx = NEIGHBORS[idx][ni];
        (ni < 6) && (neighbor_idx != NONE);
        neighbor_idx = NEIGHBORS[idx][++ni]) {

        //if neighbor has the same color
        neighbor_val = fields[neighbor_idx];
        if(neighbor_val == to_play) {
            if(merge_occured) {
                //merge a group with another group
                //the group with the lower field id stays
                //this is arbitrarily chosen
                //TODO -- improvements? (merge smaller groups into larger groups..?)
                U8 group_to_stay = min(field_to_group[idx], field_to_group[neighbor_idx]);
                U8 group_to_go = max(field_to_group[idx], field_to_group[neighbor_idx]);

                //adjust masks
                group_masks[GROUPS_START[to_play] + group_to_stay] |= group_masks[GROUPS_START[to_play] + group_to_go];
                group_masks[GROUPS_START[to_play] + group_to_go] = NONE;

                //iterate over all group ids
                //and merge them together
                for(int gi = FIRST_FIELD; gi <= LAST_FIELD; gi++) {
                    if(field_to_group[gi] == group_to_go) {
                        field_to_group[gi] = group_to_stay;
                    }
                }
            } else {
                //merge the newly placed stone's group into the existing larger group
                field_to_group[idx] = field_to_group[neighbor_idx];
                //or the edge mask with the existing group
                group_masks[GROUPS_START[to_play] + neighbor_idx] |= group_masks[GROUPS_START[to_play] + idx];
                //remove the old mask
                group_masks[GROUPS_START[to_play] + idx] = NONE;
            }

            merge_occured = true; //future merges will be group merges
        }
    }
    //if no merge occured --> everything has been done before the loop

    setScore();

    //switch color
    to_play = FLIP(to_play);

    //
    move_nr++;
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
