#include "board.hpp"

bool BOARD::move(const MOVE * mv) {
    assert(this->check_board()); 
    assert(this->ply >= 0 && this->ply < MAX_GAME_PLIES-1); 
    assert(this->search_ply >= 0 && this->search_ply < MAX_DEPTH-1); 
    assert(mv->from_sq() != mv->to_sq());

    this->ply++; 
    this->search_ply++;
    this->game[this->ply].captured = this->position[mv->to_sq()]; 
    this->game[this->ply].caracteristics = this->game[this->ply-1].caracteristics; // copy the caracteristics then modify it 
    this->game[this->ply].key = this->game[this->ply-1].key;                       // copy the actual key then modify it 
    this->game[this->ply].mv = *mv; 

    // update side
    color_t side = this->get_side(); 
    this->switch_side(); 

    square_t from = mv->from_sq();  Piece pce_from = this->position[from];
    square_t to   = mv->to_sq();    Piece pce_to   = this->position[to];

    assert(SQONBOARD(from)); assert(SQONBOARD(to)); 
    assert(pceValid(pce_from) && pceValidOrEmpty(pce_to)); 

    // en passant capture
    if (mv->type_of() == ENPASSANT) {
        assert(this->get_en_passant_file() != FILE_NONE); 
        // pce from == bP or wP
        // we should remove the pawn of opposite side 
        // side is side who make the move 
        assert((side == WHITE && bP == Piece(side)) || (side == BLACK && wP == Piece(side)));
        this->clear_piece(Piece(side), FR2SQ64(this->get_en_passant_file(), SQ64TOR(from))); 
    } else if (mv->type_of() == CASTLING) {
        // move the rook 
        switch (to) {
            case C1: this->move_piece(wR, A1, D1); break; 
            case C8: this->move_piece(bR, A8, D8); break; 
            case G1: this->move_piece(wR, H1, F1); break; 
            case G8: this->move_piece(bR, H8, F8); break; 
            default: ERROR_MSG(ERROR_MV_CA); break; 
        }
    }

    this->set_en_passant_file(FILE_NONE); 
    this->increment50Moves(); 
    
    if (pce_to != EMPTY) {
        // capture move
        this->clear_piece(pce_to, to); 
        this->reset50Moves();
    }

    if (isPa(this->position[from])) {
        // update en passant square 
        this->reset50Moves();
        if (to - from == 16) {
            // white pawn start 
            assert(SQ64TOR(from) == RANK_2);  assert(SQ64TOR(to) == RANK_4); 
            this->set_en_passant_file(SQ64TOF(from)); 
        } else if (from-to == 16) {
            // black pawn start 
            assert(SQ64TOR(from) == RANK_7); assert(SQ64TOR(to) == RANK_5); 
            this->set_en_passant_file(SQ64TOF(from));  
        }
    }

    // normal 
    this->move_piece(pce_from, from, to); 
    
    // promotion 
    if (mv->type_of() == PROMOTION) {
        piece_t pt = mv->promotion_type(); 
        assert(pt != PAWN); 
        assert((side == WHITE && wP == Piece(side^1)) || (side == BLACK && bP == Piece(side^1))); 
        this->clear_piece(Piece(side^1), to); 
        assert((pt == KNIGHT && side == WHITE && 2*pt+1-side == wN)
            || (pt == KNIGHT && side == BLACK && 2*pt+1-side == bN)
            || (pt == BISHOP && side == WHITE && 2*pt+1-side == wB)
            || (pt == BISHOP && side == BLACK && 2*pt+1-side == bB)
            || (pt == ROOK   && side == WHITE && 2*pt+1-side == wR)
            || (pt == ROOK   && side == BLACK && 2*pt+1-side == bR)
            || (pt == QUEEN  && side == WHITE && 2*pt+1-side == wQ)
            || (pt == QUEEN  && side == BLACK && 2*pt+1-side == bQ)); 
        this->add_piece(Piece(2*pt + 1 - side), to); 
    }

    // update ca permission 
    this->update_ca_permission(castlePerm[from]); 
    this->update_ca_permission(castlePerm[to]); 
    
    assert(this->check_board()); 

    assert(piece2color[-side+wK] != this->get_side()); 
    if (this->is_sq_attacked(this->pieces_list[-side + wK][0] , this->get_side())) {
        this->unmove(); 
        return false; 
    }

    return true; 
}

void BOARD::unmove() {
    assert(this->check_board()); 

    MOVE mv = this->game[this->ply].mv; 
    Piece captured = this->game[this->ply].captured; 

    square_t from = mv.from_sq(); 
    square_t to   = mv.to_sq(); 

    assert(SQONBOARD(from)); assert(SQONBOARD(to));

    Piece piece_to = this->position[to]; 
    assert(pceValid(piece_to)); 
    assert(this->get_side() != piece2color[piece_to]); 
    
    // undo en passant capture move 
    if (mv.type_of() == ENPASSANT) {
        Piece pa = Piece(-piece_to+wP); 
        file_t f = SQ64TOF(to); rank_t r = SQ64TOR(from); 
        square_t where = FR2SQ64(f,r);
        this->add_piece(pa, where); 
    } else if (mv.type_of() == CASTLING) {
        // move the rook 
        switch (to) {
            case C1: assert(this->position[D1] == wR); this->move_piece(wR, D1, A1); break; 
            case C8: assert(this->position[D8] == bR); this->move_piece(bR, D8, A8); break; 
            case G1: assert(this->position[F1] == wR); this->move_piece(wR, F1, H1); break; 
            case G8: assert(this->position[F8] == bR); this->move_piece(bR, F8, H8); break; 
            default: ERROR_MSG(ERROR_UMV_CA); break; 
        }
    }

    this->move_piece(piece_to, to, from); 

    if (captured != EMPTY) {
        assert(pceValid(captured)); 
        this->add_piece(captured,to); 
    }

    if (mv.type_of() == PROMOTION) {
        this->clear_piece(piece_to, from); 
        this->add_piece(Piece(this->get_side()), from); 
    }

    this->ply--;
    this->search_ply--;
    
    assert(this->check_board()); 
}

void BOARD::null_move() {
    assert(this->check_board()); 
    assert(this->is_in_check() == false); 

    // this->set_en_passant_file(FILE_NONE); 

    this->search_ply++; 
    this->ply++; 

    this->game[this->ply].captured       = EMPTY; 
    this->game[this->ply].caracteristics = this->game[this->ply-1].caracteristics;
    this->game[this->ply].key            = this->game[this->ply-1].key;                   
    this->game[this->ply].mv             = MOVE(); 

    this->switch_side(); 
    
    assert(this->check_board()); 	
    assert(this->search_ply > 0 && this->search_ply <= MAX_DEPTH); 
    assert(this->ply > 0 && this->ply <= MAX_GAME_PLIES); 
}

void BOARD::null_unmove() {
    assert(this->check_board()); 
    
    this->search_ply--; 
    this->ply--;

    this->game[this->ply].caracteristics = this->game[this->ply+1].caracteristics;
    this->game[this->ply].key            = this->game[this->ply+1].key;     

    this->switch_side(); 

    assert(this->check_board()); 	
    assert(this->search_ply > 0 && this->search_ply <= MAX_DEPTH); 
    assert(this->ply > 0 && this->ply <= MAX_GAME_PLIES);  
}

MOVE BOARD::parse_move(const char * pMV) const {
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
        return MOVE(from, to, PROMOTION, pt); 
    }

    // castle move
    if ((piece2type[this->position[from]] == KING) && (abs(from - to) == 2)) {
        return MOVE(from, to, CASTLING); 
    }

    // en passant move square_t ep_sq   = FR2SQ64(this->get_en_passant_file(), ep_rank); 
    rank_t   ep_rank = (this->get_side() == WHITE ? RANK_6 : RANK_3); 
    square_t ep_sq   = FR2SQ64(this->get_en_passant_file(), ep_rank); 
    if ((piece2type[this->position[from]] == PAWN) && (this->get_en_passant_file() != FILE_NONE) && (to == ep_sq)) {
        return MOVE(from, to, ENPASSANT); 
    }
    
    // else normal 
    return MOVE(from, to); 
} 