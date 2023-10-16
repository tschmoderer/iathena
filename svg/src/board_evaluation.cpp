#include "board.hpp"

#define ENDGAME_MAT (1 * piece_value[wR] + 2 * piece_value[wN] + 2 * piece_value[wP] + piece_value[wK])

const int isolated_pawn = -10;
const int passed_pawn[8] = { 0, 5, 10, 20, 35, 60, 100, 200 }; 
const int rook_on_open_file = 10;
const int rook_on_semi_open_file = 5;
const int queen_on_open_file = 5;
const int queen_on_semi_opem_file = 3;
const int bishop_pair = 30;

const int pa_table[64] = {
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
    10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
    5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
    0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
    5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
    10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
    20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	};

const int kn_table[64] = {
    0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
    0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
    0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
    0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
    5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
    5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	};

const int bi_table[64] = {
    0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
    0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
    0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
    0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
    0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
    0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
    0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	};

const int rk_table[64] = {
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	};

const int king_table_eg[64] = {	
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	};

const int king_table_op[64] = {	
	0	,	5	,	5	,	-10	,	-10	,	0	,	10	,	5	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	};

int BOARD::evaluate() const {
    assert(this->check_board()); 

    if ((this->BB_pieces_by_type[PAWN] == NULL_64 && this->material_draw() != false) || (this->threefold_repetition() != false)) {
        return 0; 
    }

    color_t side  = this->get_side(); 
    int score = 0, w_score = 0, b_score = 0; 
    for (Piece p = bP; p < EMPTY; p++) {
        if (piece2color[p] == WHITE) {
            w_score += this->pieces_count[p]*piece_value[p];
        } else {
            b_score += this->pieces_count[p]*piece_value[p];
        }
    }
    score = w_score - b_score; 

    bitboard_t white_pa = (this->BB_pieces_by_type[PAWN] & this->BB_pieces_by_color[WHITE]); 
    bitboard_t black_pa = (this->BB_pieces_by_type[PAWN] & this->BB_pieces_by_color[BLACK]); 

    Piece pce = wP; 
    for (int idx = 0; idx < this->pieces_count[pce]; idx++) {
        square_t sq64 = this->pieces_list[pce][idx]; 
        assert(SQONBOARD(sq64)); 
        score += pa_table[sq64]; 

        // isolated and passed pawns 
        if ((bb_util::isolated_pawn_mask[sq64] & white_pa) == 0)
            score += isolated_pawn; 
        if ((bb_util::white_passed_pawn_mask[sq64] & black_pa) == 0) {
            score += passed_pawn[SQ64TOR(sq64)]; 
        }
    }

    pce = bP; 
    for (int idx = 0; idx < this->pieces_count[pce]; idx++) {
        square_t sq64 = this->pieces_list[pce][idx]; 
        assert(SQONBOARD(sq64)); assert(SQONBOARD(MIRROR_64[sq64]));
        score -= pa_table[MIRROR_64[sq64]]; 

        // isolated and passed pawns 
        if ((bb_util::isolated_pawn_mask[sq64] & black_pa) == 0)
            score -= isolated_pawn; 
        if ((bb_util::black_passed_pawn_mask[sq64] & white_pa) == 0) {
            score -= passed_pawn[RANK_8 - SQ64TOR(sq64)]; 
        }
    }

    /* positionnal bonus */ 
    // knights 
    pce = wN; 
    for (int idx = 0; idx < this->pieces_count[pce]; idx++) {
        square_t sq64 = this->pieces_list[pce][idx]; 
        assert(SQONBOARD(sq64)); 
        score += kn_table[sq64]; 
    }
    pce = bN; 
    for (int idx = 0; idx < this->pieces_count[pce]; idx++) {
        square_t sq64 = this->pieces_list[pce][idx]; 
        assert(SQONBOARD(sq64)); assert(SQONBOARD(MIRROR_64[sq64])); 
        score -= kn_table[MIRROR_64[sq64]]; 
    }

    // bishops
    pce = wB; 
    for (int idx = 0; idx < this->pieces_count[pce]; idx++) {
        square_t sq64 = this->pieces_list[pce][idx]; 
        assert(SQONBOARD(sq64)); 
        score += bi_table[sq64]; 
    }
    pce = bB; 
    for (int idx = 0; idx < this->pieces_count[pce]; idx++) {
        square_t sq64 = this->pieces_list[pce][idx]; 
        assert(SQONBOARD(sq64)); assert(SQONBOARD(MIRROR_64[sq64])); 
        score -= bi_table[MIRROR_64[sq64]]; 
    }

    // rooks 
    pce = wR; 
    for (int idx = 0; idx < this->pieces_count[pce]; idx++) {
        square_t sq64 = this->pieces_list[pce][idx]; 
        assert(SQONBOARD(sq64)); 
        score += rk_table[sq64]; 

        // open and semi open file score 
        if ((this->BB_pieces_by_type[PAWN] & FILE_BB[SQ64TOF(sq64)]) == 0) {
            score += rook_on_open_file; 
        } else if ((white_pa & FILE_BB[SQ64TOF(sq64)]) == 0) {
            score += rook_on_semi_open_file;
        }
    }
    pce = bR; 
    for (int idx = 0; idx < this->pieces_count[pce]; idx++) {
        square_t sq64 = this->pieces_list[pce][idx]; 
        assert(SQONBOARD(sq64)); assert(SQONBOARD(MIRROR_64[sq64]));
        score -= rk_table[MIRROR_64[sq64]]; 

        // open and semi open file score 
        if ((this->BB_pieces_by_type[PAWN] & FILE_BB[SQ64TOF(sq64)]) == 0) {
            score -= rook_on_open_file; 
        } else if ((black_pa & FILE_BB[SQ64TOF(sq64)]) == 0) {
            score -= rook_on_semi_open_file;
        }        
    }

    // queens 
    pce = wQ; 
    for (int idx = 0; idx < this->pieces_count[pce]; idx++) {
        square_t sq64 = this->pieces_list[pce][idx]; 
        assert(SQONBOARD(sq64)); 

        // open and semi open file score 
        if ((this->BB_pieces_by_type[PAWN] & FILE_BB[SQ64TOF(sq64)]) == 0) {
            score += queen_on_open_file; 
        } else if ((white_pa & FILE_BB[SQ64TOF(sq64)]) == 0) {
            score += queen_on_semi_opem_file;
        }
    }    
    pce = bQ; 
    for (int idx = 0; idx < this->pieces_count[pce]; idx++) {
        square_t sq64 = this->pieces_list[pce][idx]; 
        assert(SQONBOARD(sq64)); assert(SQONBOARD(MIRROR_64[sq64]));

        // open and semi open file score 
        if ((this->BB_pieces_by_type[PAWN] & FILE_BB[SQ64TOF(sq64)]) == 0) {
            score -= queen_on_open_file; 
        } else if ((black_pa & FILE_BB[SQ64TOF(sq64)]) == 0) {
            score -= queen_on_semi_opem_file;
        }        
    }   

    // kings 
    pce = wK; 
    square_t sq64 = this->pieces_list[pce][0]; 
    assert(SQONBOARD(sq64)); 

    if (b_score <= ENDGAME_MAT) {
        score += king_table_eg[sq64]; 
    } else {
        score += king_table_op[sq64]; 
    }
    pce = bK; 
    sq64 = this->pieces_list[pce][0]; 
    assert(SQONBOARD(sq64)); 

    if (w_score <= ENDGAME_MAT) {
        score -= king_table_eg[MIRROR_64[sq64]]; 
    } else {
        score -= king_table_op[MIRROR_64[sq64]]; 
    }

    /* bishops pair */ 
    if (this->pieces_count[wB] >= 2) score += bishop_pair; 
    if (this->pieces_count[bB] >= 2) score -= bishop_pair; 

    if (side == BLACK) 
        return -score; 
    return score; 	
}

bool BOARD::is_a_repetition() const {
    assert(this->ply - this->get50Moves() >= 0); 
    assert(this->ply >= 1); 
    ///@todo change for threefold repetition
    ///@todo add a repetition function only for the search 
    for (int p = this->ply - this->get50Moves(); p < this->ply-1; p++) {
        assert(p >= 0 && p < MAX_GAME_PLIES); 
        if (this->game[this->ply].key == this->game[p].key) 
            return true; 
    }
    return false; 
}

bool BOARD::threefold_repetition() const {

    return false; 

    assert(this->ply - this->get50Moves() >= 0); 
    assert(this->ply >= 0); 
    
    uint64_t key = this->game[this->ply].key; 
    bool rep1 = false; bool rep2 = false; 
    ///@todo you can loop 2 ply by two ply 
    for (int p = this->ply - this->get50Moves(); p < this->ply; p++) {
        assert(p >= 0 && p < MAX_GAME_PLIES); 
        if (!rep1 && key == this->game[p].key) {
            rep1 = false; 
        } else if (rep1 && !rep2 && key == this->game[p].key) {
            rep2 = true; 
        } else if (rep2 && key == this->game[p].key) {
            return true; 
        }
    }

    return false; 
}

bool BOARD::material_draw() const {
	assert(this->check_board());

    if (this->pieces_count[wR] + this->pieces_count[bR] + this->pieces_count[wQ] + this->pieces_count[bQ] == 0) {
        // no rooks and queen
        if (this->pieces_count[bB] + this->pieces_count[wB] == 0) {
            // no bishops
            if (this->pieces_count[wN] < 3 && this->pieces_count[bN] < 3) // less than 3 knights
                return true; 
        } else if (this->pieces_count[wN] + this->pieces_count[bN] == 0) {
            // no knights 
            if (abs(this->pieces_count[wB] - this->pieces_count[bB]) < 2) 
                return true; 
        } else if ((this->pieces_count[wN] < 3 && this->pieces_count[wB] == 0) || (this->pieces_count[wB] == 1 && this->pieces_count[wN] == 0)) {
            if ((this->pieces_count[bN] < 3 && this->pieces_count[bB] == 0) || (this->pieces_count[bB] == 1 && this->pieces_count[bN] == 0))
                return true; 
        } 

    } else if (this->pieces_count[wQ] == 0 && this->pieces_count[bQ] == 0) {
        // no queens
        if (this->pieces_count[wR] == 1 && this->pieces_count[bR] == 1) {
            if ((this->pieces_count[wN] + this->pieces_count[wB] < 2) && (this->pieces_count[bN] + this->pieces_count[bB] < 2)) 
                return true; 
        } else if (this->pieces_count[wR] == 1 && this->pieces_count[bR] == 0) {
            if  ((this->pieces_count[wN] + this->pieces_count[wB] == 0) && ((this->pieces_count[bN] + this->pieces_count[bB] == 1) || (this->pieces_count[bN] + this->pieces_count[bB] == 2)))
                return true;
        } else if (this->pieces_count[bR] == 1 && this->pieces_count[wR] == 0) {
            if ((this->pieces_count[bN] + this->pieces_count[bB] == 0) && ((this->pieces_count[wN] + this->pieces_count[wB] == 1) || (this->pieces_count[wN] + this->pieces_count[wB] == 2)))
                return true;  
        }
    }	
  
    return false;
}

/**
 * @brief 
 * 
 * @return true  if in zugswang 
 * @return false if at least  a big piece : N, B, R, Q 
*/
bool BOARD::is_in_zugswang() const {
    color_t side    = this->get_side();
    bitboard_t king = this->BB_pieces_by_color[side] & this->BB_pieces_by_type[KING]; 
    bitboard_t pawn = this->BB_pieces_by_color[side] & this->BB_pieces_by_type[PAWN]; 
    bitboard_t others = this->BB_pieces_by_color[side] ^ king ^ pawn;  

    return (others == NULL_64); 
}

void BOARD::clear_search_heuristics() {
    for (int idx = 0; idx < 12; idx++) {
        for (int idx2 = 0; idx2 < NB_BRD_SQR; idx2++) {
            this->search_history[idx][idx2] = 0; 
        }
    }

    for (int idx = 0; idx < 2; idx++) {
        for (int idx2 = 0; idx2 < MAX_DEPTH; idx2++) {
            this->search_killers[idx][idx2] = 0; 
        }
    }
}

void BOARD::save_killer(const uint16_t mv) {
    this->search_killers[1][this->search_ply] = this->search_killers[0][this->search_ply]; 
    this->search_killers[0][this->search_ply] = mv; 
}

void BOARD::save_history(const square_t from, const square_t to, const int depth) {
    this->search_history[this->position[from]][to] += depth; 
}