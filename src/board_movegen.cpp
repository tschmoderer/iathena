#include "board.hpp"

void BOARD::movegen(movelist_t * lst) const {
    assert(this->check_board()); 

    lst->cnt = 0; 
   
    /* generic function for pieces move generation */
    this->movegen_quiet_pieces(lst); 
    this->movegen_capture_pieces(lst);

    this->generate_ep(lst); 
    this->generate_ca(lst);
}

template<piece_t PT> void BOARD::generate_quiet_moves(movelist_t * lst, bitboard_t & all_pieces, bitboard_t & no_pieces) const {
    bitboard_t bb_piece   = this->BB_pieces_by_type[PT] & this->BB_pieces_by_color[this->get_side()];

    while (bb_piece) {
        square_t from      = bb::pop_bit(&bb_piece); 
        bitboard_t targets = find_moves<PT>(from, all_pieces) & no_pieces;
        while (targets) {
            square_t to = bb::pop_bit(&targets); 

            lst->moves[lst->cnt] = MOVE(from,to); 
            if (this->search_killers[0][this->search_ply] == lst->moves[lst->cnt].get_move()) {
                lst->moves[lst->cnt].set_score(FIRST_KILLER_SCORE); 
            } else if (this->search_killers[1][this->search_ply] == lst->moves[lst->cnt].get_move()) {
                lst->moves[lst->cnt].set_score(SECOND_KILLER_SCORE); 
            } else {
                lst->moves[lst->cnt].set_score(this->search_history[this->position[from]][to]); 
            }
            lst->cnt++;
        }
    } 
}

template<piece_t PT> void BOARD::generate_capture_moves(movelist_t * lst, bitboard_t & all_pieces, bitboard_t & other_pieces) const {
    bitboard_t bb_piece   = this->BB_pieces_by_type[PT] & this->BB_pieces_by_color[this->get_side()];

    while (bb_piece) {
        square_t from      = bb::pop_bit(&bb_piece); 
        bitboard_t targets = find_moves<PT>(from, all_pieces) & other_pieces;
        while (targets) {
            square_t to = bb::pop_bit(&targets); 
            lst->moves[lst->cnt] = MOVE(from, to); 
            lst->moves[lst->cnt].set_score( mvvLva[piece2type[this->position[to]]][PT]);
            lst->cnt++;
        }
    } 
}

void BOARD::movegen_quiet_pieces(movelist_t * lst) const {
    color_t side = this->get_side(); 

    bitboard_t all_pieces = this->BB_pieces_by_color[WHITE] | this->BB_pieces_by_color[BLACK];
    bitboard_t no_pieces  = ~all_pieces; 
    bitboard_t bb_piece = NULL_64; 

    // Pawn 
    bb_piece = this->BB_pieces_by_type[PAWN] & this->BB_pieces_by_color[side]; 

    while (bb_piece) {
        square_t from = bb::pop_bit(&bb_piece); 
        bitboard_t targets = NULL_64; 

        if (!(all_pieces & bb_normal_moves::pawn_normal(side, from))) {
            targets |= bb_normal_moves::pawn_normal(side, from);
            if (!(all_pieces & bb_normal_moves::pawn_start(side, from))) {
                targets |= bb_normal_moves::pawn_start(side, from);
            }
        }

        while (targets) {
            square_t to = bb::pop_bit(&targets); 
            int score = 0; 
            uint16_t mv = (from << 6) + to; 

            if (this->search_killers[0][this->search_ply] == mv) {
                score = FIRST_KILLER_SCORE; 
            } else if (this->search_killers[1][this->search_ply] == mv) {
                score = SECOND_KILLER_SCORE; 
            } else {
                score = this->search_history[this->position[from]][to]; 
            }

            // promotion 
            if (side == WHITE ? to >= A8 : to <= H1) {
                lst->moves[lst->cnt++] = MOVE(from,to,PROMOTION,KNIGHT, score); 
                lst->moves[lst->cnt++] = MOVE(from,to,PROMOTION,BISHOP, score); 
                lst->moves[lst->cnt++] = MOVE(from,to,PROMOTION,ROOK  , score); 
                lst->moves[lst->cnt++] = MOVE(from,to,PROMOTION,QUEEN , score); 
            } else {
                // no promotion
                lst->moves[lst->cnt++] = MOVE(mv, score); 
            }
        }
    }

    /* quiet moves because mask = no_pieces */ 
    generate_quiet_moves<KNIGHT>(lst, all_pieces, no_pieces); // Knight 
    generate_quiet_moves<  KING>(lst, all_pieces, no_pieces); // King
    generate_quiet_moves<  ROOK>(lst, all_pieces, no_pieces); // Rook 
    generate_quiet_moves<BISHOP>(lst, all_pieces, no_pieces); // Bishop
    generate_quiet_moves< QUEEN>(lst, all_pieces, no_pieces); // Queen
}

void BOARD::movegen_capture_pieces(movelist_t * lst) const {
    color_t side = this->get_side(); 

    bitboard_t other_pieces = this->BB_pieces_by_color[side^1]; 
    bitboard_t all_pieces = this->BB_pieces_by_color[WHITE] | this->BB_pieces_by_color[BLACK]; 

    bitboard_t bb_piece = NULL_64; 

    // Pawn 
    bb_piece = this->BB_pieces_by_type[PAWN] & this->BB_pieces_by_color[side]; 

    while (bb_piece) {
        square_t from = bb::pop_bit(&bb_piece); 
        bitboard_t targets = (bb_normal_moves::pawn_capture(side, from) & other_pieces);

        while (targets) {
            square_t to = bb::pop_bit(&targets); 
            // promotion 
            if (side == WHITE ? to >= A8 : to <= H1) {
                lst->moves[lst->cnt++] = MOVE(from, to, PROMOTION, KNIGHT, mvvLva[piece2type[this->position[to]]][PAWN]); 
                lst->moves[lst->cnt++] = MOVE(from, to, PROMOTION, BISHOP, mvvLva[piece2type[this->position[to]]][PAWN]); 
                lst->moves[lst->cnt++] = MOVE(from, to, PROMOTION, ROOK,   mvvLva[piece2type[this->position[to]]][PAWN]); 
                lst->moves[lst->cnt++] = MOVE(from, to, PROMOTION, QUEEN,  mvvLva[piece2type[this->position[to]]][PAWN]); 
            } else {
                // no promotion
                lst->moves[lst->cnt] = MOVE(from, to); 
                lst->moves[lst->cnt].set_score(mvvLva[piece2type[this->position[to]]][PAWN]);
                lst->cnt++;
            }
        }
    }

    /* capture moves because mask = other_pieces */ 
    generate_capture_moves<KNIGHT>(lst, all_pieces, other_pieces); // Knight 
    generate_capture_moves<  KING>(lst, all_pieces, other_pieces); // King
    generate_capture_moves<  ROOK>(lst, all_pieces, other_pieces); // Rook 
    generate_capture_moves<BISHOP>(lst, all_pieces, other_pieces); // Bishop
    generate_capture_moves< QUEEN>(lst, all_pieces, other_pieces); // Queen
}

void BOARD::generate_ep(movelist_t * lst) const {
    color_t side = this->get_side(); 

    if (this->get_en_passant_file() != FILE_NONE) {
        rank_t   ep_rank = (side == WHITE ? RANK_6 : RANK_3); 
        square_t ep_sq   = FR2SQ64(this->get_en_passant_file(), ep_rank); 
        // if an imaginary pawn of the oponent can grab one of my pawn 
        bitboard_t ep_attacks = bb_normal_moves::pawn_capture(color_t(side^1),ep_sq) & (this->BB_pieces_by_type[PAWN] & this->BB_pieces_by_color[side]); 
        while (ep_attacks) {
            square_t from = bb::pop_bit(&ep_attacks); 
            lst->moves[lst->cnt++] = MOVE(from, ep_sq, ENPASSANT, KNIGHT, mvvLva[PAWN][PAWN]); 
        }
    }  
}


void BOARD::generate_ca(movelist_t * lst) const {
    color_t side = this->get_side(); 
    
    /* Castling rules in chess 960
        As in regular chess, you may not castle:

        on a particular wing if you have moved that wing’s rook already.
        if you have moved the king already.
        if any squares the king must travel over are attacked.
        if any squares that rook and king must travel on to reach their destination are occupied by other pieces
    */
    // Castling moves 
    ///@todo change for chess960
    if (side == WHITE) {
        // white castle king side 
        if (this->can_castle(WKCA)) {
            if (this->position[F1] == EMPTY && this->position[G1] == EMPTY)  {
                if (!this->is_sq_attacked(E1,BLACK) && !this->is_sq_attacked(F1,BLACK)) {
                    lst->moves[lst->cnt++] = MOVE(E1,G1,CASTLING); 
                }
            }
        }
        // white castle queen side 
        if (this->can_castle(WQCA)) {
            if (this->position[D1] == EMPTY && this->position[C1] == EMPTY && this->position[B1] == EMPTY)  {
                if (!this->is_sq_attacked(E1,BLACK) && !this->is_sq_attacked(D1,BLACK)) {
                    lst->moves[lst->cnt++] = MOVE(E1,C1,CASTLING); 
                }
            }
        }
    } else {
        // black castle king side 
        if (this->can_castle(BKCA)) {
            if (this->position[F8] == EMPTY && this->position[G8] == EMPTY)  {
                if (!this->is_sq_attacked(E8,WHITE) && !this->is_sq_attacked(F8,WHITE)) {
                    lst->moves[lst->cnt++] = MOVE(E8,G8,CASTLING); 
                }
            }
        }
        // black castle queen side 
        if (this->can_castle(BQCA)) {
            if (this->position[D8] == EMPTY && this->position[C8] == EMPTY && this->position[B8] == EMPTY)  {
                if (!this->is_sq_attacked(E8,WHITE) && !this->is_sq_attacked(D8,WHITE)) {
                    lst->moves[lst->cnt++] = MOVE(E8,C8,CASTLING); 
                }
            }
        }
    }
}