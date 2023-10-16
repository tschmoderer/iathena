#include "bb.hpp"

bitboard_t FILE_FORWARD_BB[8]; 
bitboard_t RANK_FORWARD_BB[8]; 
bitboard_t SQUARE_BB[NB_BRD_SQR]; 
bitboard_t RAYS_BB[DIRECTIONS_NB][NB_BRD_SQR]; 

void bb::init() {
    square_t tmp_sq; 

    for (int i=0; i<8; i++) {
        FILE_FORWARD_BB[i] = NULL_64; 
        RANK_FORWARD_BB[i] = NULL_64;
    }
    
    for (file_t f = FILE_A; f <= FILE_H; f++) {
        for (file_t f2 = file_t(f+1); f2 <=FILE_H; f2++){
            FILE_FORWARD_BB[f] |= FILE_BB[f2];  
        }
    }

    for (rank_t r = RANK_1; r <= RANK_8; r++) {
        for (rank_t r2 = rank_t(r+1); r2 <= RANK_8; r2++) {
            RANK_FORWARD_BB[r] |= RANK_BB[r2]; 
        }
    }

    for (square_t sq = A1; sq <= H8; sq++) {
        SQUARE_BB[sq] = 0; 
        for (direction_t d = NORTH; d <= SOUTH_WEST; d++) {
            RAYS_BB[d][sq] = 0; 
        }
    }

    for (square_t sq = A1; sq <= H8; sq++) {
        SQUARE_BB[sq] = (1ULL << sq); 
    }

    for (square_t sq = A1; sq <= H8; sq++) {
        RAYS_BB[NORTH][sq] = 0x0101010101010100ULL << sq; 
        RAYS_BB[SOUTH][sq] = 0x0080808080808080ULL >> (63 - sq); 
        RAYS_BB[EAST][sq]  = 2*((1ULL << (sq | 7)) - (1ULL << sq)); 
        RAYS_BB[WEST][sq]  = (1ULL << sq) - (1ULL << (sq & 56));

        tmp_sq = sq; 
        while (FILEONBOARD(file_t(SQ64TOF(tmp_sq)-1)) && RANKONBOARD(rank_t(SQ64TOR(tmp_sq)+1))) {
            tmp_sq = tmp_sq + 7; 
            RAYS_BB[NORTH_WEST][sq] |= SQUARE_BB[tmp_sq]; 
        }
        tmp_sq = sq; 
        while (FILEONBOARD(file_t(SQ64TOF(tmp_sq)+1)) && RANKONBOARD(rank_t(SQ64TOR(tmp_sq)+1))) {
            tmp_sq = square_t(tmp_sq + 9); 
            RAYS_BB[NORTH_EAST][sq] |= SQUARE_BB[tmp_sq]; 
        }
        tmp_sq = sq; 
        while (FILEONBOARD(file_t(SQ64TOF(tmp_sq)-1)) && RANKONBOARD(rank_t(SQ64TOR(tmp_sq)-1))) {
            tmp_sq = square_t(tmp_sq - 9); 
            RAYS_BB[SOUTH_WEST][sq] |= SQUARE_BB[tmp_sq]; 
        }
        tmp_sq = sq; 
        while (FILEONBOARD(file_t(SQ64TOF(tmp_sq)+1)) && RANKONBOARD(rank_t(SQ64TOR(tmp_sq)-1))) {
            tmp_sq = square_t(tmp_sq - 7); 
            RAYS_BB[SOUTH_EAST][sq] |= SQUARE_BB[tmp_sq]; 
        }
    }

    bb_normal_moves::init(); 
    bb_magic_moves::init(); 
    bb_util::init();        // need to be made after init moves of rook and bishops ? 
}

