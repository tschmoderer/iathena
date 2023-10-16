#include "../includes/position.hpp"

void Position::set_position(const char * fen) {
    this->reset_board(); 
    this->parse_fen(fen); 
}

void Position::set_fullmove(const uint8_t & p) {
    this->fullmove = p; 
}

void Position::set_side(const color_t & s) {
    assert(s == WHITE || s == BLACK);
    this->informations ^= (-(s) ^ this->informations) & 1UL;
    
    if (s == WHITE) {
        this->key ^= keys[780]; // hash key
    }
}

void Position::set_ca_permission(const castle_t & ca) {
    this->informations ^= ca;
    switch (ca) { // hash key
        case WKCA: this->key ^= keys[768 + 0]; break; 
        case WQCA: this->key ^= keys[768 + 1]; break;
        case BKCA: this->key ^= keys[768 + 2]; break; 
        case BQCA: this->key ^= keys[768 + 3]; break;
        default: ERROR_MSG(ERROR_SET_CA); break; 
    }
}

///@warning this function check if there is a pawn that can capture en passant and update key 
void Position::set_en_passant_file(const file_t & file) {
    assert(FILEONBOARD(file)); 

    // hash out the game en passant file : 
    file_t previous_ep_file = this->get_en_passant_file(); 
    if (previous_ep_file != FILE_NONE) {
        this->informations = (this->informations & ~0x1E0) + (file << 5);
        this->key ^= keys[772 + previous_ep_file]; 
    }

    if (file != FILE_NONE) {  
        color_t  side    = color_t(this->get_side()^1);       // side who made the pawn push 
        rank_t   ep_rank = (side == WHITE) ? RANK_3 : RANK_6; // rank behind the pawn 
        square_t ep_sq   = FR2SQ64(file, ep_rank);             // square behind the pawn push 

        // if you have have an imaginary pawn just behind your pawn pushed that can eat an adversary pawn then the en passant capture is possible 
        if (bb_normal_moves::pawn_capture(side, ep_sq) & this->BB_pieces_by_type[PAWN] & this->BB_pieces_by_color[side^1]) {
            this->informations = (this->informations & ~0x1E0) + ((file) << 5);
            this->key ^= keys[772 + file];
        }
    }
}

void Position::set_50_moves_rule(const uint8_t & mv50) {
    assert(mv50 >= 0 && mv50 < 128); 
    this->informations = (this->informations & ~0xFE00) + (mv50 << 9);
}