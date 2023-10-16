#include "../includes/position.hpp"

Move Position::parse_move(const char * pMV) const {
    // parse (file,rank) from and (file,rank) to 
    file_t ff = file_t(pMV[0] - 'a'); rank_t rf = rank_t(pMV[1] - '1'); 
    file_t ft = file_t(pMV[2] - 'a'); rank_t rt = rank_t(pMV[3] - '1'); 

    piece_t pt = PIECE_TYPE_NONE; 
    switch (pMV[4]) {
        case 'n': pt = KNIGHT; break; 
        case 'b': pt = BISHOP; break; 
        case 'r': pt = ROOK;   break; 
        case 'q': pt = QUEEN;  break; 
        default: pt = PIECE_TYPE_NONE; break; 
    }

    square_t from = FR2SQ64(ff,rf); 
    square_t to   = FR2SQ64(ft,rt); 

    // promotion move 
    if (pt != PIECE_TYPE_NONE) {
        return Move(from, to, PROMOTION, pt); 
    }

    // castle move
    if ((piece2type[this->state[from]] == KING) && (abs(from - to) == 2)) {
        return Move(from, to, CASTLING); 
    }

    // en passant move square_t ep_sq   = FR2SQ64(this->get_en_passant_file(), ep_rank); 
    rank_t   ep_rank = (this->get_side() == WHITE ? RANK_6 : RANK_3); 
    square_t ep_sq   = FR2SQ64(this->get_en_passant_file(), ep_rank); 
    if ((piece2type[this->state[from]] == PAWN) && (this->get_en_passant_file() != FILE_NONE) && (to == ep_sq)) {
        return Move(from, to, ENPASSANT); 
    }
    
    // else normal 
    return Move(from, to); 
} 