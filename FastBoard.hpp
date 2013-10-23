#ifndef FASTBOARD_HPP
#define FASTBOARD_HPP

#include <algorithm>
#include <vector>
#include <stack>
#include "string.h"
using namespace std;

#include "definitions.hpp"

struct FastBoard {
    U8 to_play;
    U8 next_move;
    U8 stones[LAST_FIELD+1];
    vector<U8> possible_moves;

    FastBoard();

    FastBoard & operator=(const FastBoard &other);

    inline void makeMove(U8 idx);
    inline I8 randomFill(U8 next_move);
    inline I8 getWinner();
    inline void colorFlip();
};

FastBoard::FastBoard() {
    to_play = WHITE;
    next_move = 1;
    stones[0] = EMPTY;
    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
        stones[i] = EMPTY;
        possible_moves.push_back(i);
    }
}

FastBoard & FastBoard::operator=(const FastBoard &other) {
    //copy static memory blocks together
    memcpy(this, &other, (1 + 1 + LAST_FIELD+1) * sizeof(U8));
    //copy dynamic memory stuff
    possible_moves = other.possible_moves;

    return *this;
}

inline
I8 FastBoard::randomFill(U8 next_move) {
    U8 color = to_play;

    //the first move(opponent answer) is selected by uct before and executed at first
    stones[next_move] = color;
    color = FLIP(color);

    //shuffle possible moves with fisher-yates-shuffle
//    U32 r;
//    for(int i = possible_moves.size()-1; i > 0; i--) {
//        r = fastrand() % (i+1);
//        swap(possible_moves[i], possible_moves[r]);
//        exit(1);
//    }

    //fill board randomly
    random_shuffle(possible_moves.begin(), possible_moves.end());

    for(U8 idx : possible_moves) {
        if(idx != next_move) {
            stones[idx] = color;
            color = FLIP(color);
        }
    }

    return getWinner();
}

/*
 * Detects which color has won.
 * Works only on filled board.
 */
inline
I8 FastBoard::getWinner() {
    //init needed data
    U8 white_score = 0;
    U8 mask = NONE;
    U8 visited[LAST_FIELD+1] = {false};
    visited[0] = true;
    stack<U8> nodes;
    U8 node;


    //detect if WHITE has won
    for(U8 fidx : EDGE_FIELDS) {
        mask = NONE;
        //iterate over all edge fields
        //only groups connected to edges can score points
        if(stones[fidx] == WHITE && !visited[fidx]) {
            //if stone at fidx is white and has not been seen before
            nodes.push(fidx);
            visited[fidx] = true;

            //start depth-first search
            while(!nodes.empty()) {
                node = nodes.top();
                nodes.pop();

                mask |= FIELD_EDGE_RELATION[node];

                //put every neighbor into stack
                U8 ni, neighbor_idx;
                for(ni = 0, neighbor_idx = NEIGHBORS[node][ni];
                    (ni < 6) && (neighbor_idx != NONE);
                    neighbor_idx = NEIGHBORS[node][++ni]) {

                    if(stones[neighbor_idx] == WHITE && !visited[neighbor_idx]) {
                        nodes.push(neighbor_idx);
                    }
                    visited[neighbor_idx] = true;
                }
            }

            //get group score
            white_score += SCORE_LOOKUP[mask];

            if(white_score >= 3) {
                return WHITE_WIN;
            }
        }
    }


    //else BLACK must have won
    return BLACK_WIN;
}

inline
void FastBoard::colorFlip() {
    for(int i = FIRST_FIELD; i <= LAST_FIELD; i++) {
        if(stones[i] != EMPTY) {
            stones[i] = FLIP(stones[i]);
            break;
        }
    }
    to_play = FLIP(to_play);
    next_move++;
}

inline
void FastBoard::makeMove(U8 idx) {
    stones[idx] = to_play;
    to_play = FLIP(to_play);
    next_move++;

    possible_moves.erase(remove(possible_moves.begin(), possible_moves.end(), idx), possible_moves.end());
}

#endif // FASTBOARD_HPP
