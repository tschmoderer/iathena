#include "../includes/position.hpp"

Piece Position::get_piece(const square_t & sq64) const {
    assert(SQONBOARD(sq64)); 
    return this->state[sq64];
}

uint8_t Position::get_fullmove() const {
    return this->fullmove; 
}

board_key_t Position::get_key() const {
    return this->key;
}

color_t Position::get_side() const {
    return  color_t(this->informations & 0x1);
}

castle_t Position::get_ca_permission() const {
    return castle_t((this->informations >> 1) & 0xF); 
}

file_t Position::get_en_passant_file() const {
    return file_t((this->informations >> 5) & 0xF);
}

uint8_t Position::get_50_moves_rule() const {
    return this->informations >> 9;
}