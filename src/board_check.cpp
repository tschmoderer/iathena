#ifndef NDEBUG

#include "board.hpp"

bool BOARD::check_board() const {
    // check piece list 
    for (Piece t_pce = bP; t_pce <= wK; t_pce++) {
        assert(this->pieces_count[t_pce] >= 0 && this->pieces_count[t_pce] < 10); 
        for (int t_pce_num = 0; t_pce_num < this->pieces_count[t_pce]; t_pce_num++) {
            square_t sq64 = this->pieces_list[t_pce][t_pce_num];
            assert(SQONBOARD(sq64)); 
            assert(this->position[sq64] == t_pce);
        }
        assert(bb::pop_count(this->BB_pieces_by_type[piece2type[t_pce]] & this->BB_pieces_by_color[piece2color[t_pce]]) == this->pieces_count[t_pce]); 
    }
    
    // check that there are no 2 pieces at the same square 
    for (square_t sq = A1; sq <= H8; sq++) {
        int cnt = 0; 
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < this->pieces_count[i]; j++) {
                if (this->pieces_list[i][j] == sq) {
                    cnt++; 
                }
            }
        }
        assert(cnt <= 1); 
    }

    // check index is ok with piece_list and count 
    for (Piece pc = bP; pc < EMPTY; pc++) {
        for (int i = 0; i < this->pieces_count[pc]; i++) {
            assert(this->position[this->pieces_list[pc][i]] == pc);
            assert(this->index[this->pieces_list[pc][i]] == i); 
        }
    }


    assert(this->get_side() == WHITE || this->get_side() == BLACK);
    assert(this->game[this->ply].key == this->generate_key()); 
    assert(this->get_en_passant_file() >= FILE_A && this->get_en_passant_file() <= FILE_NONE);
    assert(this->pieces_count[wK] == 1 && this->pieces_count[bK] == 1);

    return true; 
}

board_key_t BOARD::generate_key() const {
    board_key_t key = NULL_64; 

    // hash piece 
    for (square_t sq64 = A1; sq64 <= H8; sq64++) {
        Piece pce = this->position[sq64]; 
        assert(pceValidOrEmpty(pce));
        if (pce != EMPTY) { 
            key ^= keys[64*pce + sq64]; 
        }
    }

    // hash castle 
    int offset = 768;
    if (this->can_castle(WKCA)) key ^= keys[offset + 0]; 
    if (this->can_castle(WQCA)) key ^= keys[offset + 1]; 
    if (this->can_castle(BKCA)) key ^= keys[offset + 2]; 
    if (this->can_castle(BQCA)) key ^= keys[offset + 3]; 

    // hash en passant 
    // we hash if and only if there is a pawn capable of making the en passant capture
    offset = 772; 
    /* we must check is there is a pawn to capture the enpassant */ 
    if (this->get_en_passant_file() != FILE_NONE) {
        key ^= keys[offset + this->get_en_passant_file()];
    }

    // hash side 
    if (this->get_side() == WHITE) 
        key ^= keys[780]; 

    return key; 
}
#endif

#ifndef NMIRROR
#include "board.hpp"

void BOARD::mirror() {
    assert(this->check_board()); 

    Piece tmp_pce[NB_BRD_SQR]; 

    color_t tmp_side = color_t(this->get_side() ^ 1);
    file_t tmp_ep_file = this->get_en_passant_file();
    uint8_t tmp_ca_perm = 0; 
    Piece swap_pieces[13] = {wP, bP, wN, bN, wB, bB, wR, bR, wQ, bQ, wK, bK, EMPTY}; 

    if (this->can_castle(WKCA)) tmp_ca_perm |= BKCA; 
    if (this->can_castle(WQCA)) tmp_ca_perm |= BQCA; 
    if (this->can_castle(BKCA)) tmp_ca_perm |= WKCA; 
    if (this->can_castle(BQCA)) tmp_ca_perm |= WQCA; 


    for (square_t sq64 = A1; sq64 <= H8; sq64++) {
        tmp_pce[sq64] = this->position[MIRROR_64[sq64]]; 
    }

    this->reset_board(); 

    for (square_t sq64 = A1; sq64 <= H8; sq64++) {
        Piece pce = swap_pieces[tmp_pce[sq64]]; 
        if (pce != EMPTY)
            this->add_piece(pce,sq64); 
    }

    this->set_side(tmp_side); 
    if (tmp_ca_perm & WKCA) this->set_ca_permission(WKCA); 
    if (tmp_ca_perm & WQCA) this->set_ca_permission(WQCA); 
    if (tmp_ca_perm & BKCA) this->set_ca_permission(BKCA); 
    if (tmp_ca_perm & BQCA) this->set_ca_permission(BQCA); 
    this->set_en_passant_file(tmp_ep_file); 
    
    assert(this->check_board()); 
}

#endif