
#include <iomanip>
#include "../includes/position.hpp"

using namespace std; 



bitboard_t king_moves[NB_BRD_SQR]; 
bitboard_t knight_moves[NB_BRD_SQR]; 
bitboard_t pawn_moves_x1[COLOR_NB][NB_BRD_SQR]; 
bitboard_t pawn_moves_x2[COLOR_NB][NB_BRD_SQR]; 
bitboard_t pawn_caps[COLOR_NB][NB_BRD_SQR]; 

bitboard_t between[NB_BRD_SQR][NB_BRD_SQR];    ///< returns a bitboard representing all the squares between the two given ones. If s1 and s2 are not on the same rank, file or diagonal, 0 is returned.
bitboard_t rays[NB_BRD_SQR];                   ///< queen moves from each square 
unsigned int distance2[NB_BRD_SQR][NB_BRD_SQR]; ///< distance in king moves between two square

bitboard_t isolated_pawn_mask[NB_BRD_SQR];     ///< squares on adjacent files of each square 
bitboard_t white_passed_pawn_mask[NB_BRD_SQR]; ///< squares forward on the file and and adjacent of each squares 
bitboard_t black_passed_pawn_mask[NB_BRD_SQR];

static bitboard_t generate_r_moves(square_t sq, bitboard_t blockers) {
    bitboard_t attacks = NULL_64; 

    // North 
    attacks |= RAYS_BB[NORTH][sq];
    if (RAYS_BB[NORTH][sq] & blockers) {
        attacks &= ~RAYS_BB[NORTH][bb::lsb(RAYS_BB[NORTH][sq] & blockers)]; 
    }

    // South 
    attacks |= RAYS_BB[SOUTH][sq]; 
    if (RAYS_BB[SOUTH][sq] & blockers) {
        attacks &= ~RAYS_BB[SOUTH][bb::msb(RAYS_BB[SOUTH][sq] & blockers)]; 
    }    

    // East 
    attacks |= RAYS_BB[EAST][sq]; 
    if (RAYS_BB[EAST][sq] & blockers) {
        attacks &= ~RAYS_BB[EAST][bb::lsb(RAYS_BB[EAST][sq] & blockers)]; 
    }    

    // West 
    attacks |= RAYS_BB[WEST][sq]; 
    if (RAYS_BB[WEST][sq] & blockers) {
        attacks &= ~RAYS_BB[WEST][bb::msb(RAYS_BB[WEST][sq] & blockers)]; 
    }    

    return attacks; 
}

static bitboard_t generate_b_moves(square_t sq, bitboard_t blockers) {
    bitboard_t attacks = NULL_64; 

    // North East
    attacks |= RAYS_BB[NORTH_EAST][sq]; 
    if (RAYS_BB[NORTH_EAST][sq] & blockers) {
        attacks &= ~RAYS_BB[NORTH_EAST][bb::lsb(RAYS_BB[NORTH_EAST][sq] & blockers)]; 
    }

    // North West
    attacks |= RAYS_BB[NORTH_WEST][sq]; 
    if (RAYS_BB[NORTH_WEST][sq] & blockers) {
        attacks &= ~RAYS_BB[NORTH_WEST][bb::lsb(RAYS_BB[NORTH_WEST][sq] & blockers)]; 
    }

     // South East
    attacks |= RAYS_BB[SOUTH_EAST][sq]; 
    if (RAYS_BB[SOUTH_EAST][sq] & blockers) {
        attacks &= ~RAYS_BB[SOUTH_EAST][bb::msb(RAYS_BB[SOUTH_EAST][sq] & blockers)]; 
    }
    // South West
    attacks |= RAYS_BB[SOUTH_WEST][sq]; 
    if (RAYS_BB[SOUTH_WEST][sq] & blockers) {
        attacks &= ~RAYS_BB[SOUTH_WEST][bb::msb(RAYS_BB[SOUTH_WEST][sq] & blockers)]; 
    }

    return attacks; 
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
/*
    cout << "Hello World!" << endl;

    for (square_t sq64 = A1; sq64 < H8; sq64++) {
        cout << sq64 << " "; 
    }
    cout << endl;

    for (square_t sq64 = H8; sq64 < SQ_NB; sq64--) {
        cout << sq64 << " "; 
    }
    cout << endl;
    
    for (file_t file = FILE_A; file < FILE_NB; file++) {
        cout << file << " "; 
    }
    cout << endl;

    for (file_t file = FILE_H; file < FILE_NB; file--) {
        cout << file << " "; 
    }
    cout << endl;
    
    for (rank_t rank = RANK_1; rank < RANK_NB; rank++) {
        cout << rank << " "; 
    }
    cout << endl; 

    for (rank_t rank = RANK_8; rank < RANK_NB; rank--) {
        cout << rank << " "; 
    }
    
    cout << endl << endl; 

    cout << "  + - - - - - - - - - - - - - - - - - - - -  +" << endl;
    for (rank_t rank = RANK_8; rank < RANK_NB; rank--) {
        cout << rank + 1 << "  |  "; 
		for (file_t file = FILE_A; file < FILE_NB; file++) {
            square_t sq64 = FR2SQ64(file, rank);
			cout << sq64 << " | ";
		}
		cout << endl;
	}

    cout << "  + - - - - - - - - - - - - - - - - - - - -  +" << endl;
    cout << "      ";
    for (int file = FILE_A; file <= FILE_H; file++) {
        cout << char('a'+file) << "  | ";
	}
    cout << endl; 

    cout << endl; 

    cout << "  + - - - - - - - - - - - - - - - - - - - -  +" << endl;
    for (rank_t rank = RANK_8; rank < RANK_NB; rank--) {
        cout << rank + 1 << "  |  "; 
		for (file_t file = FILE_A; file < FILE_NB; file++) {
            square_t sq64 = FR2SQ64(file, rank);
			cout << fileChar[SQ64TOF(sq64)] << rankChar[SQ64TOR(sq64)] << " | ";
		}
		cout << endl;
	}

    cout << "  + - - - - - - - - - - - - - - - - - - - -  +" << endl;
    cout << "      ";
    for (int file = FILE_A; file <= FILE_H; file++) {
        cout << char('a'+file) << "  | ";
	}
    cout << endl; 
*/
    Position P1(START_FEN);
    cout << P1 << endl;
/*
    for (square_t sq64 = A1; sq64 < SQ_NB; sq64++) {
        cout << "SQUARE " << sq64 << " : " << endl; 
        show(SQUARE_BB[sq64]);
        cout << endl;
    }

    for (file_t file = FILE_A; file < FILE_NB; file++) {
        cout << "FILE " << fileChar[file] << " : " << endl; 
        show(FILES_BB[file]);
        cout << endl;
    }

    for (rank_t rank = RANK_1; rank < RANK_NB; rank++) {
        cout << "RANK " << rank << " : " << endl; 
        show(RANKS_BB[rank]);
        cout << endl;
    }

    for (file_t file = FILE_A; file < FILE_NB; file++) {
        cout << "FILE " << fileChar[file] << " : " << endl; 
        show(adjacent_file_bb[file]);
        cout << endl;
    }
*/
    /*square_t tmp_sq;
    bitboard_t RAYS_BB[DIRECTION_NB][NB_BRD_SQR] ;
    for (square_t sq = A1; sq < SQ_NB; sq++) {
        //RAYS_BB[NORTH][sq] = 0x0101010101010100ULL << sq; 
        //RAYS_BB[SOUTH][sq] = 0x0080808080808080ULL >> (63 - sq); 
        //RAYS_BB[EAST][sq]  = 2*((1ULL << (sq | 7)) - (1ULL << sq)); 
        //RAYS_BB[WEST][sq]  = (1ULL << sq) - (1ULL << (sq & 56));

        tmp_sq = sq; 
        RAYS_BB[NORTH_WEST][sq] = NULL_64;
        RAYS_BB[NORTH_EAST][sq] = NULL_64;
        RAYS_BB[SOUTH_WEST][sq] = NULL_64;
        RAYS_BB[SOUTH_EAST][sq] = NULL_64;
        while (FILEONBOARD(file_t(SQ64TOF(tmp_sq)-1)) && RANKONBOARD(rank_t(SQ64TOR(tmp_sq)+1))) {
            tmp_sq = tmp_sq + 7; 
            RAYS_BB[NORTH_WEST][sq] |= tmp_sq; 
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
    }*/
/*
    for (int dir = 0; dir < DIRECTION_NB; dir++) {
        cout << "{"; 
        for (square_t sq = A1; sq < SQ_NB; sq++) {
            //cout << "DIRECTION : " << dir << " SQUARE : " << sq << endl; 
            //show( RAYS_BB[dir][sq]); 
            cout << "0x" << hex << setfill('0') << setw(16) << RAYS_BB[dir][sq] << ", ";
        }
        cout << "}," << endl; 
    }



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

    cout << endl << endl << endl; 
    cout << "{";
    for (square_t sq = A1 ; sq <= H8; sq++) {
        //cout << "SQUARE : " << sq << endl;
        //show(king_moves[sq]); 
        //cout << endl;
        cout << "0x" << hex << setfill('0') << setw(16) << pawn_caps[WHITE][sq] << ", ";
    }
    cout << "},"<< endl << "{";
    for (square_t sq = A1 ; sq <= H8; sq++) {
        //cout << "SQUARE : " << sq << endl;
        //show(king_moves[sq]); 
        //cout << endl;
        cout << "0x" << hex << setfill('0') << setw(16) << pawn_caps[BLACK][sq] << ", ";
    }
    cout << "}";
*/
    /*for (square_t sq = A1 ; sq <= H8; sq++) {
        cout << "SQUARE : " << sq << endl;
        show(knight_moves[sq]); 
        cout << endl; 
    }

    for (square_t sq = A1 ; sq <= H8; sq++) {
        cout << "SQUARE : " << sq << endl;
        show(pawn_moves_x1[WHITE][sq]); 
        cout << endl; 
    }*/
    
    for (square_t sq1 = A1; sq1 <= H8; sq1++) {
        for (square_t sq2 = A1; sq2 <= H8; sq2++) {
            bitboard_t occupied = SQUARE_BB[sq1] | SQUARE_BB[sq2]; 
            between[sq1][sq2] = NULL_64; 
            if (bb_magic_moves::bishop_moves(sq1,NULL_64) & SQUARE_BB[sq2]) {
               between[sq1][sq2] |= bb_magic_moves::bishop_moves(sq1,occupied) & bb_magic_moves::bishop_moves(sq2,occupied);  
            } else if (bb_magic_moves::rook_moves(sq1,NULL_64) & SQUARE_BB[sq2]) {
               between[sq1][sq2] |= bb_magic_moves::rook_moves(sq1,occupied) & bb_magic_moves::rook_moves(sq2,occupied); 
            }

            distance2[sq1][sq2] = std::max(std::abs(SQ64TOF(sq1) - SQ64TOF(sq2)),std::abs(SQ64TOR(sq1) - SQ64TOR(sq2)));            
        }

        rays[sq1] = NULL_64; 
        for (direction_t d = NORTH; d <= SOUTH_WEST; d++) {
            rays[sq1] |= RAYS_BB[d][sq1]; 
        }

        isolated_pawn_mask[sq1]     = NULL_64; 
        white_passed_pawn_mask[sq1] = NULL_64; 
        black_passed_pawn_mask[sq1] = NULL_64; 

        isolated_pawn_mask[sq1]     |= adjacent_file_bb[SQ64TOF(sq1)]; 
        white_passed_pawn_mask[sq1] |= (RANK_FORWARD_BB[SQ64TOR(sq1)] & (FILES_BB[SQ64TOF(sq1)] | adjacent_file_bb[SQ64TOF(sq1)])); 
        black_passed_pawn_mask[sq1] |= (~RANKS_BB[SQ64TOR(sq1)]) & (~RANK_FORWARD_BB[SQ64TOR(sq1)] & (FILES_BB[SQ64TOF(sq1)] | adjacent_file_bb[SQ64TOF(sq1)])); 
    }

    /*for (square_t sq1 = A1; sq1 < SQ_NB; sq1++) {
        cout << "{"; 
        for (square_t sq2 = A1; sq2 < SQ_NB; sq2++) {
            cout << "(0x" << hex << setfill('0') << setw(16) << between[sq1][sq2] << "), ";
        }
        cout << "}," << endl;
    }*/
    /*for (square_t sq1 = A1; sq1 < SQ_NB; sq1++) {
        cout << "{"; 
        for (square_t sq2 = A1; sq2 < SQ_NB; sq2++) {
            cout  << distance2[sq1][sq2] << ", ";
        }
        cout << "}," << endl;
    }*/
    /*for (square_t sq = A1; sq < SQ_NB; sq++) {
        cout << "(0x" << hex << setfill('0') << setw(16) << black_passed_pawn_mask[sq] << "), ";
    }*/

/*
    for (square_t sq = A1; sq < SQ_NB; sq++) {
        cout << "(0x" << hex << setfill('0') << setw(16) << bb_magic_moves::b_magic[sq] << "), ";
        if (SQ64TOF(sq) == FILE_H) 
            cout << endl; 
    }
    */
    bitboard_t r_table[NB_BRD_SQR][4096]; 
    bitboard_t b_table[NB_BRD_SQR][512];

    // for all square 
    int m = 0; 
    for (square_t sq = A1; sq <= H8; sq++) {
        // for all possible blockers for this square
        for (int blocker_idx = 0; blocker_idx < (1 << bb_magic_moves::r_index[sq]); blocker_idx++) {
            bitboard_t blockers = NULL_64; 
            bitboard_t mask = bb_magic_moves::r_mask[sq]; 
            int bits = bb::pop_count(mask); 
            for (int i = 0; i < bits; i++) {
                square_t bitPos = bb::pop_bit(&mask); 
                if (blocker_idx & (1 << i))
                    blockers |= (1ULL << bitPos); 
            }
            int key = (blockers * bb_magic_moves::r_magic[sq]) >> (64-bb_magic_moves::r_index[sq]); 
            m = std::max(m,key); 
            assert(key >= 0 && key < 4096); 
            r_table[sq][key] = generate_r_moves(sq,blockers);    
        }
    }
    std::cout << "max for rook " << m << std::endl; 

    m = 0; 
    for (square_t sq = A1; sq <= H8; sq++) {
        // for all possible blockers for this square
        for (int blocker_idx = 0; blocker_idx < (1 << bb_magic_moves::b_index[sq]); blocker_idx++) {
            bitboard_t blockers = NULL_64; 
            bitboard_t mask = bb_magic_moves::b_mask[sq]; 
            int bits = bb::pop_count(mask); 
            for (int i = 0; i < bits; i++) {
                square_t bitPos = bb::pop_bit(&mask); 
                if (blocker_idx & (1 << i))
                    blockers |= (1ULL << bitPos); 
            }
            int key = (blockers * bb_magic_moves::b_magic[sq]) >> (64-bb_magic_moves::b_index[sq]); 
            m = std::max(m,key); 
            assert(key >= 0 && key < 512); 
            b_table[sq][key] = generate_b_moves(sq,blockers);    
        }
    }
    std::cout << "max for bishop " << m << std::endl; 


    for (square_t sq = A1; sq < SQ_NB; sq++) {
        cout << "{";
        for (int i = 0; i < 4096; i++) {
            cout << "(0x" << hex << setfill('0') << setw(16) <<  r_table[sq][i] << "), "; 
        }
        cout << "}, " << endl; 
    }

    /*for (square_t sq = A1; sq < SQ_NB; sq++) {
        cout << "{";
        for (int i = 0; i < 512; i++) {
            cout << "(0x" << hex << setfill('0') << setw(16) <<  b_table[sq][i] << "), "; 
        }
        cout << "}, " << endl; 
    }*/
    return EXIT_SUCCESS; 
}