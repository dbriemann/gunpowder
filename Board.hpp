#ifndef BOARD_HPP
#define BOARD_HPP

#include "string.h"
#include <vector>
#include <algorithm>
using namespace std;

#include "definitions.hpp"

struct Board {
    U8 next_move;
    U8 to_play;
    //U8 scores[2];
    U8 fields[LAST_FIELD+1];
    U8 field_to_group[LAST_FIELD+1];
    U8 group_masks[2*(LAST_FIELD+1)];

    vector<U8> possible_moves;

    Board();    
    Board & operator=(const Board &other);

    I8 makeMove(U8 idx);
    inline U8 getScore();
    inline I8 getWinner();
//    inline U8 isWin();
    inline U8 getRandomMove();
    inline void colorFLip();
};

Board::Board() {
    next_move = 1;
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
    //copy static memory blocks together
    memcpy(this, &other, (1 + 1 + (4*(LAST_FIELD+1)) ) * sizeof(U8));
    //copy dynamic memory stuff
    possible_moves = other.possible_moves;

    return *this;
}

inline
void Board::colorFLip() {
    next_move++;
    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
        if(fields[i] == WHITE) {
            fields[i] = BLACK;
            U8 group_idx = field_to_group[i];
            group_masks[group_idx + GROUPS_START[BLACK]] = group_masks[group_idx];
            group_masks[group_idx] = NONE;
        }
    }
    to_play = FLIP(to_play);
    possible_moves.pop_back(); // flip is disabled
}

inline
U8 Board::getRandomMove() {
    int moves = possible_moves.size();
    int rand_idx = rand() % moves;
    return possible_moves[rand_idx];
}

I8 Board::makeMove(U8 idx) {

    if(idx == FLIP_MOVE) {
        colorFLip();
        return NONE;
    } else {
        //if second move and no flip
        if(next_move == 2) {
            //remove flip possibility
            possible_moves.pop_back();
        }
    }

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
                if(group_to_stay == group_to_go) {
                    //circular group
                    continue;
                }

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
                group_masks[GROUPS_START[to_play] + field_to_group[neighbor_idx]] |= group_masks[GROUPS_START[to_play] + idx];
                //remove the old mask
                group_masks[GROUPS_START[to_play] + idx] = NONE;
            }

            merge_occured = true; //future merges will be group merges
        }
    }
    //if no merge occured --> everything has been done before the loop


    I8 win = getWinner();

    //remove possible move from vector
//    possible_moves.erase(remove(possible_moves.begin(), possible_moves.end(), idx), possible_moves.end());
    for(auto iter = possible_moves.begin(); iter != possible_moves.end(); iter++) {
        if(*iter == idx) {
            possible_moves.erase(iter);
            break;
        }
    }

    //switch color
    to_play = FLIP(to_play);

    if(next_move == 1) {
        //if this was the first move.. make flip possible
        possible_moves.push_back(FLIP_MOVE);
    }
    //
    next_move++;

    return win;
}

inline
U8 Board::getScore() {
    U8 mask = NONE;
    U8 score = 0;

    //sets score for current player(to_play)
    for(int i = GROUPS_START[to_play]; i <= GROUPS_END[to_play]; i++) {
        mask = group_masks[i];
        score += SCORE_LOOKUP[mask];
    }

    return score;
}

//inline
//U8 Board::isWin() {
//    if(getScore() >= 3) {
//        return 1;
//    }
//    return 0;
//}

inline
I8 Board::getWinner() {
    if(getScore() >= 3) {
        if(to_play == WHITE) {
            return WHITE_WIN;
        } else {
            return BLACK_WIN;
        }
    }
    return NO_WIN;
}


#endif // BOARD_HPP
