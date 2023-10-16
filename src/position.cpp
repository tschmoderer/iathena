#include "../includes/position.hpp"

Position::Position(const char * fen) {
    this->reset_board(); 
    this->parse_fen(fen);
}

Position::~Position() {}

void Position::reset_board() {
    for (piece_t pt = PAWN; pt < PIECE_TYPE_NB; pt++) {
        this->BB_pieces_by_type[pt] = NULL_64; 
    }

    this->BB_pieces_by_color[WHITE] = NULL_64; 
    this->BB_pieces_by_color[BLACK] = NULL_64; 

    this->informations = NULL_16;
    this->informations += FILE_NONE << 5;
    this->fullmove = 1; 

    this->key = NULL_64;

    for (square_t sq64 = A1; sq64 <= H8; sq64++) {
        this->state[sq64] = PIECE_NONE;
    }
}

void Position::add_piece(const Piece & pce, const square_t & sq64) {
    assert(SQONBOARD(sq64));

    this->BB_pieces_by_type[piece2type[pce]] |= sq64;
    this->BB_pieces_by_color[piece2color[pce]] |= pce;

    this->key ^= keys[64*pce + sq64];

    this->state[sq64] = pce;
}

void Position::remove_piece(const Piece & pce, const square_t & sq64) {
    assert(SQONBOARD(sq64));

    this->BB_pieces_by_type[piece2type[pce]] ^= sq64;
    this->BB_pieces_by_color[piece2color[pce]] ^= pce;

    this->key ^= keys[64*pce + sq64];

    this->state[sq64] = PIECE_NONE;
}

// do we need pce here 
void Position::move_piece(const Piece & pce, const square_t & from, const square_t & to) {
    assert(SQONBOARD(from));
    assert(SQONBOARD(to));
    assert(from != to);

    bitboard_t fromTo = SQUARE_BB[from] ^ SQUARE_BB[to]; 

    this->BB_pieces_by_color[piece2color[pce]] ^= fromTo; 
    this->BB_pieces_by_type[piece2type[pce]]   ^= fromTo; 

    this->key ^= keys[64*pce + from]; // hash out the piece from key 
    this->key ^= keys[64*pce + to];   // hash in the piece from key 

    this->state[to] = this->state[from]; 
    this->state[from] = PIECE_NONE;
}

bool Position::can_castle(const castle_t & ca) const {
    return this->informations & ca;
}
