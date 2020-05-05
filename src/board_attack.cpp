#include "board.hpp"

bool BOARD::is_sq_attacked(const square_t sq64, const color_t side) const {
    assert(SQONBOARD(sq64)); 
    assert(side ==  WHITE || side == BLACK); 
    assert(this->check_board()); 

    bitboard_t all_pieces = this->BB_pieces_by_color[WHITE] | this->BB_pieces_by_color[BLACK]; 
    bitboard_t res; 

    res = (bb_magic_moves::bishop_moves(sq64, all_pieces) | bb_magic_moves::rook_moves(sq64, all_pieces)) & (this->BB_pieces_by_color[side] & this->BB_pieces_by_type[QUEEN]); 
    res = res || ((bb_magic_moves::rook_moves(sq64, all_pieces)) & (this->BB_pieces_by_color[side] & this->BB_pieces_by_type[ROOK])); 
    res = res || ((bb_magic_moves::bishop_moves(sq64, all_pieces)) & (this->BB_pieces_by_color[side] & this->BB_pieces_by_type[BISHOP])); 
    res = res || ((find_moves<KNIGHT>(sq64, all_pieces)) & (this->BB_pieces_by_color[side] & this->BB_pieces_by_type[KNIGHT]));
    res = res || ((find_moves<KING>(sq64, all_pieces)) & (this->BB_pieces_by_color[side] & this->BB_pieces_by_type[KING])); 
    res = res || ((bb_normal_moves::pawn_capture(color_t(side^1), sq64) & (this->BB_pieces_by_color[side] & this->BB_pieces_by_type[PAWN])));

    return res; 
}

/**
 * @brief 
 * @todo make it return the number of checkers 
 * 
 * @return true 
 * @return false 
 */

bool BOARD::is_in_check() const {
    color_t side = this->get_side(); 
    Piece king = Piece(-side + wK); 
    assert((side == WHITE && king == wK) || (side == BLACK && king == bK)); 
    return this->is_sq_attacked(this->pieces_list[king][0],color_t(side^1));
}
