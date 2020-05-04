#include "bb.hpp"

bitboard_t bb_util::between[NB_BRD_SQR][NB_BRD_SQR]; 
bitboard_t bb_util::rays[NB_BRD_SQR]; 
unsigned int bb_util::distance[NB_BRD_SQR][NB_BRD_SQR];

bitboard_t bb_util::isolated_pawn_mask[NB_BRD_SQR]; 
bitboard_t bb_util::white_passed_pawn_mask[NB_BRD_SQR];
bitboard_t bb_util::black_passed_pawn_mask[NB_BRD_SQR];

void bb_util::init() {
    for (square_t sq1 = A1; sq1 <= H8; sq1++) {
        for (square_t sq2 = A1; sq2 <= H8; sq2++) {
            bitboard_t occupied = SQUARE_BB[sq1] | SQUARE_BB[sq2]; 
            bb_util::between[sq1][sq2] = NULL_64; 
            if (bb_magic_moves::bishop_moves(sq1,NULL_64) & SQUARE_BB[sq2]) {
               bb_util::between[sq1][sq2] |= bb_magic_moves::bishop_moves(sq1,occupied) & bb_magic_moves::bishop_moves(sq2,occupied);  
            } else if (bb_magic_moves::rook_moves(sq1,NULL_64) & SQUARE_BB[sq2]) {
               bb_util::between[sq1][sq2] |= bb_magic_moves::rook_moves(sq1,occupied) & bb_magic_moves::rook_moves(sq2,occupied); 
            }

            bb_util::distance[sq1][sq2] = std::max(std::abs(SQ64TOF(sq1) - SQ64TOF(sq2)),std::abs(SQ64TOR(sq1) - SQ64TOR(sq2)));            
        }

        bb_util::rays[sq1] = NULL_64; 
        for (direction_t d = NORTH; d <= SOUTH_WEST; d++) {
            bb_util::rays[sq1] |= RAYS_BB[d][sq1]; 
        }

        bb_util::isolated_pawn_mask[sq1]     = NULL_64; 
        bb_util::white_passed_pawn_mask[sq1] = NULL_64; 
        bb_util::black_passed_pawn_mask[sq1] = NULL_64; 

        bb_util::isolated_pawn_mask[sq1]     |= adjacent_file_bb[SQ64TOF(sq1)]; 
        bb_util::white_passed_pawn_mask[sq1] |= (RANK_FORWARD_BB[SQ64TOR(sq1)] & (FILE_BB[SQ64TOF(sq1)] | adjacent_file_bb[SQ64TOF(sq1)])); 
        bb_util::black_passed_pawn_mask[sq1] |= (~RANK_BB[SQ64TOR(sq1)]) & (~RANK_FORWARD_BB[SQ64TOR(sq1)] & (FILE_BB[SQ64TOF(sq1)] | adjacent_file_bb[SQ64TOF(sq1)])); 
    }
}