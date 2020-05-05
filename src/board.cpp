#include "board.hpp"

BOARD::BOARD(const char * fen) {
    this->parse_fen(fen); 
}

/* getters */


/* setters */
void BOARD::add_piece(const Piece & pce, const square_t & sq64) {
    assert(pceValid(pce)); 
    assert(SQONBOARD(sq64)); 
    assert(this->position[sq64] == EMPTY); 

    this->position[sq64] = pce; // add the piece
    this->BB_pieces_by_type[piece2type[pce]] |= sq64; 
    this->BB_pieces_by_color[piece2color[pce]] |= sq64; 

    this->game[this->ply].key ^= keys[64*pce + sq64]; // hash in the piece inkey 

    // update helpers
    this->index[sq64] = this->pieces_count[pce]++; 
    this->pieces_list[pce][this->index[sq64]] = sq64;
}

void BOARD::clear_piece(const Piece & pce, const square_t & sq64) {
    assert(SQONBOARD(sq64)); 
    assert(pce != EMPTY); 

    this->position[sq64] = EMPTY; // remove the piece
    this->BB_pieces_by_type[piece2type[pce]] ^= sq64; 
    this->BB_pieces_by_color[piece2color[pce]] ^= sq64; 
    this->game[this->ply].key ^= keys[64*pce + sq64]; // hash out the piece from key 

    assert(this->pieces_count[pce] > 0); 
    // update helpers 
    square_t last_sq = this->pieces_list[pce][--this->pieces_count[pce]]; 
    this->index[last_sq] = this->index[sq64]; 
    this->pieces_list[pce][this->index[last_sq]] = last_sq; 
    this->pieces_list[pce][this->pieces_count[pce]] = NO_SQ; 
}

void BOARD::move_piece(const Piece & pce, const square_t & from, const square_t & to) {
    assert(SQONBOARD(from)); 
    assert(SQONBOARD(to)); 
    assert(from != to); 

    bitboard_t fromTo = SQUARE_BB[from] ^ SQUARE_BB[to]; 
    this->BB_pieces_by_color[piece2color[pce]] ^= fromTo; 
    this->BB_pieces_by_type[piece2type[pce]]   ^= fromTo; 

    this->position[from] = EMPTY; 
    this->position[to]   = pce; 
    
    this->game[this->ply].key ^= keys[64*pce + from]; // hash out the piece from key 
    this->game[this->ply].key ^= keys[64*pce + to];   // hash in the piece from key 

    this->index[to] = this->index[from]; 
    this->pieces_list[pce][this->index[to]] = to; 
/*
    this->clear_piece(pce, from); 
    this->add_piece(pce, to); 
*/
}

