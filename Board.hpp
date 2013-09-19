#ifndef BOARD_HPP
#define BOARD_HPP

#include "definitions.hpp"

struct Board {
    U8 move_nr;
    U8 to_play;
    U8 score[2];
    U8 fields[LAST_FIELD+1];
    U8 group_id[LAST_FIELD+1];

    Board();
};

Board::Board() {
    move_nr = 0;
    to_play = WHITE;
    score[WHITE] = 0;
    score[BLACK] = 0;
    for(int i = 0; i <= LAST_FIELD; i++) {
        fields[i] = EMPTY;
        group_id[i] = NONE;
    }
}

#endif // BOARD_HPP
