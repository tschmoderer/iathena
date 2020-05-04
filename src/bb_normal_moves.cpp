#include "bb.hpp"

namespace bb_normal_moves {

bitboard_t king_moves[NB_BRD_SQR]; 
bitboard_t knight_moves[NB_BRD_SQR]; 
bitboard_t pawn_moves_x1[COLOR_NB][NB_BRD_SQR]; 
bitboard_t pawn_moves_x2[COLOR_NB][NB_BRD_SQR]; 
bitboard_t pawn_caps[COLOR_NB][NB_BRD_SQR]; 

void init() {
    for (square_t sq = A1; sq <= H8; sq++) {
        king_moves[sq]   = NULL_64; 
        knight_moves[sq] = NULL_64; 
        pawn_moves_x1[WHITE][sq] = NULL_64; 
        pawn_moves_x1[BLACK][sq] = NULL_64;
        pawn_moves_x2[WHITE][sq] = NULL_64; 
        pawn_moves_x2[BLACK][sq] = NULL_64;
        pawn_caps[WHITE][sq] = NULL_64; 
        pawn_caps[BLACK][sq] = NULL_64;
    }

    for (rank_t r = RANK_1; r <= RANK_8; r++) {
        for (file_t f = FILE_A; f <= FILE_H; f++) {
            // king moves
            if (FILEONBOARD(file_t(f + 0)) && RANKONBOARD(rank_t(r + 1))) king_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f+0),rank_t(r+1)); // north
            if (FILEONBOARD(file_t(f + 0)) && RANKONBOARD(rank_t(r - 1))) king_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f+0),rank_t(r-1)); // south

            if (FILEONBOARD(file_t(f + 1)) && RANKONBOARD(rank_t(r + 1))) king_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f+1),rank_t(r+1)); // north east
            if (FILEONBOARD(file_t(f + 1)) && RANKONBOARD(rank_t(r + 0))) king_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f+1),rank_t(r+0)); // east
            if (FILEONBOARD(file_t(f + 1)) && RANKONBOARD(rank_t(r - 1))) king_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f+1),rank_t(r-1)); // south east

            if (FILEONBOARD(file_t(f - 1)) && RANKONBOARD(rank_t(r + 1))) king_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f-1),rank_t(r+1)); // north west
            if (FILEONBOARD(file_t(f - 1)) && RANKONBOARD(rank_t(r + 0))) king_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f-1),rank_t(r+0)); // west 
            if (FILEONBOARD(file_t(f - 1)) && RANKONBOARD(rank_t(r - 1))) king_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f-1),rank_t(r-1)); // south west

            // knight moves
            if (FILEONBOARD(file_t(f - 2)) && RANKONBOARD(rank_t(r + 1))) knight_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f-2),rank_t(r+1)); //
            if (FILEONBOARD(file_t(f - 2)) && RANKONBOARD(rank_t(r - 1))) knight_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f-2),rank_t(r-1)); // 

            if (FILEONBOARD(file_t(f - 1)) && RANKONBOARD(rank_t(r + 2))) knight_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f-1),rank_t(r+2)); // 
            if (FILEONBOARD(file_t(f - 1)) && RANKONBOARD(rank_t(r - 2))) knight_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f-1),rank_t(r-2)); // 
            
            if (FILEONBOARD(file_t(f + 1)) && RANKONBOARD(rank_t(r - 2))) knight_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f+1),rank_t(r-2)); // 
            if (FILEONBOARD(file_t(f + 1)) && RANKONBOARD(rank_t(r + 2))) knight_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f+1),rank_t(r+2)); // 

            if (FILEONBOARD(file_t(f + 2)) && RANKONBOARD(rank_t(r + 1))) knight_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f+2),rank_t(r+1)); //  
            if (FILEONBOARD(file_t(f + 2)) && RANKONBOARD(rank_t(r - 1))) knight_moves[FR2SQ64(f,r)] |= FR2SQ64(file_t(f+2),rank_t(r-1)); // 

            // pawn normal forward move 
            if (FILEONBOARD(file_t(f+0)) && RANKONBOARD(rank_t(r+1))) pawn_moves_x1[WHITE][FR2SQ64(f,r)] |= FR2SQ64(file_t(f+0),rank_t(r+1)); //
            if (FILEONBOARD(file_t(f+0)) && RANKONBOARD(rank_t(r-1))) pawn_moves_x1[BLACK][FR2SQ64(f,r)] |= FR2SQ64(file_t(f+0),rank_t(r-1)); //
            
            // pawn start double move 
            if (r == RANK_2) {
                pawn_moves_x2[WHITE][FR2SQ64(f,r)] |= FR2SQ64(f,rank_t(r+2)); // white double push
            } else if (r == RANK_7) {
                pawn_moves_x2[BLACK][FR2SQ64(f,r)] |= FR2SQ64(f,rank_t(r-2)); // black double push 
            }

            // pawn capture move 
            if (FILEONBOARD(file_t(f+1)) && RANKONBOARD(rank_t(r+1))) pawn_caps[WHITE][FR2SQ64(f,r)] |= FR2SQ64(file_t(f+1),rank_t(r+1)); 
            if (FILEONBOARD(file_t(f-1)) && RANKONBOARD(rank_t(r+1))) pawn_caps[WHITE][FR2SQ64(f,r)] |= FR2SQ64(file_t(f-1),rank_t(r+1)); 
            if (FILEONBOARD(file_t(f+1)) && RANKONBOARD(rank_t(r-1))) pawn_caps[BLACK][FR2SQ64(f,r)] |= FR2SQ64(file_t(f+1),rank_t(r-1)); 
            if (FILEONBOARD(file_t(f-1)) && RANKONBOARD(rank_t(r-1))) pawn_caps[BLACK][FR2SQ64(f,r)] |= FR2SQ64(file_t(f-1),rank_t(r-1)); 
        }
    }
}
}