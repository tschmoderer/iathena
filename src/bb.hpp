#ifndef BB_HPP
#define BB_HPP

#include "types.hpp"

// https://rhysre.net/2019/01/15/magic-bitboards.html

// ==========================================================================
// Section 0. Useful helpers
// ==========================================================================
constexpr bitboard_t FILEABB = 0x0101010101010101;  constexpr bitboard_t RANK1BB = 0xFF; 
constexpr bitboard_t FILEBBB = FILEABB << 1;        constexpr bitboard_t RANK2BB = RANK1BB << (8 * 1);
constexpr bitboard_t FILECBB = FILEABB << 2;        constexpr bitboard_t RANK3BB = RANK1BB << (8 * 2);
constexpr bitboard_t FILEDBB = FILEABB << 3;        constexpr bitboard_t RANK4BB = RANK1BB << (8 * 3);
constexpr bitboard_t FILEEBB = FILEABB << 4;        constexpr bitboard_t RANK5BB = RANK1BB << (8 * 4);
constexpr bitboard_t FILEFBB = FILEABB << 5;        constexpr bitboard_t RANK6BB = RANK1BB << (8 * 5);
constexpr bitboard_t FILEGBB = FILEABB << 6;        constexpr bitboard_t RANK7BB = RANK1BB << (8 * 6);
constexpr bitboard_t FILEHBB = FILEABB << 7;        constexpr bitboard_t RANK8BB = RANK1BB << (8 * 7);

const bitboard_t FILE_BB[8]          = {FILEABB, FILEBBB, FILECBB, FILEDBB, FILEEBB, FILEFBB, FILEGBB, FILEHBB}; 
const bitboard_t RANK_BB[8]          = {RANK1BB, RANK2BB, RANK3BB, RANK4BB, RANK5BB, RANK6BB, RANK7BB, RANK8BB}; 
const bitboard_t adjacent_file_bb[8] = {FILEBBB, FILEABB | FILECBB, FILEBBB | FILEDBB, FILECBB | FILEEBB, FILEDBB | FILEFBB, FILEEBB | FILEGBB, FILEFBB | FILEHBB, FILEGBB};

extern bitboard_t FILE_FORWARD_BB[8]; 
extern bitboard_t RANK_FORWARD_BB[8]; 
extern bitboard_t SQUARE_BB[NB_BRD_SQR]; 
extern bitboard_t RAYS_BB[DIRECTIONS_NB][NB_BRD_SQR];

// ==========================================================================
// Section 1. Operations on 64 bits numbers // bb.cpp
// ==========================================================================
namespace bb {
    inline square_t lsb(bitboard_t b) {
        assert(b);
        #if defined(__GNUC__)
            return square_t(__builtin_ctzll(b));
        #elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
            unsigned long idx;
            _BitScanForward64(&idx, b);
            return square_t(idx);
        #else
            #error "No intrinsic lsb/bitscanforward/ctz available"
        #endif
    }

    inline square_t msb(bitboard_t b) {
        assert(b);
        #if defined(__GNUC__)
            return square_t(63 ^ __builtin_clzll(b));
        #elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
            unsigned long idx;
            _BitScanReverse64(&idx, b);
            return square_t(idx);
        #else
            #error "No intrinsic msb/bitscanreverse/clz available"
        #endif
    }

    inline int pop_count(const bitboard_t &b) {
        #if defined(__GNUC__)
            return __builtin_popcountll(b);
        #elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
            return (int) _mm_popcnt_u64(b);
        #else
            #error "No intrinsic popcnt available"
        #endif
    }

    inline square_t pop_bit(bitboard_t * bb) {
        const square_t s = lsb(*bb);
        *bb &= *bb - 1;
        return s;
    }

    #ifndef NDEBUG
    inline void my_print(const bitboard_t & bb) {
        std::cout << "+---+---+---+---+---+---+---+---+\n"; 
        for (rank_t r = RANK_8; r >= RANK_1; r--) {
            for (file_t f = FILE_A; f <= FILE_H; f++) {
                if ((bb & SQUARE_BB[FR2SQ64(f, r)]) != 0) {
                    std::cout << "| X "; 
                } else  {
                    std::cout << "|   ";
                }
            }
            std::cout << "|\n+---+---+---+---+---+---+---+---+\n";
        }
    }
    #endif

    void init(); 
}

// ==========================================================================
// Section 2. Useful metrics // bb_util.cpp
// ==========================================================================
namespace bb_util {
    extern bitboard_t between[NB_BRD_SQR][NB_BRD_SQR];    ///< returns a bitboard representing all the squares between the two given ones. If s1 and s2 are not on the same rank, file or diagonal, 0 is returned.
    extern bitboard_t rays[NB_BRD_SQR];                   ///< queen moves from each square 
    extern unsigned int distance[NB_BRD_SQR][NB_BRD_SQR]; ///< distance in king moves between two square

    extern bitboard_t isolated_pawn_mask[NB_BRD_SQR];     ///< squares on adjacent files of each square 
    extern bitboard_t white_passed_pawn_mask[NB_BRD_SQR]; ///< squares forward on the file and and adjacent of each squares 
    extern bitboard_t black_passed_pawn_mask[NB_BRD_SQR]; ///< squares downward on the file and and adjacent of each squares 

    void init(); 
}

// ==========================================================================
// Section 3. Compute moves of static pieces // bb_normal_moves.cpp
// ==========================================================================
namespace bb_normal_moves {
    extern bitboard_t king_moves[NB_BRD_SQR];
    extern bitboard_t knight_moves[NB_BRD_SQR];
    extern bitboard_t pawn_moves_x1[COLOR_NB][NB_BRD_SQR];
    extern bitboard_t pawn_moves_x2[COLOR_NB][NB_BRD_SQR];
    extern bitboard_t pawn_caps[COLOR_NB][NB_BRD_SQR];
    
    inline bitboard_t ki_moves(const square_t & sq) {
        assert(SQONBOARD(sq));
        return king_moves[sq]; 
    } 
    inline bitboard_t kn_moves(const square_t & sq) {
        assert(SQONBOARD(sq)); 
        return knight_moves[sq]; 
    } 
    inline bitboard_t pawn_normal(const color_t & t, const square_t & sq) {
        assert(SQONBOARD(sq)); 
        assert(t == WHITE || t == BLACK); 
        return pawn_moves_x1[t][sq]; 
    } 
    inline bitboard_t pawn_start(const color_t & t, const square_t & sq) {
        assert(SQONBOARD(sq)); 
        assert(t == WHITE || t == BLACK); 
        return pawn_moves_x2[t][sq]; 
    } 
    inline bitboard_t pawn_capture(const color_t & t, const square_t & sq) {
        assert(SQONBOARD(sq)); 
        assert(t == WHITE || t == BLACK); 
        return pawn_caps[t][sq]; 
    } 
    
    void init(); 
}

// ==========================================================================
// Section 4. Compute moves of sliding pieces // bb_magic_moves.cpp
// ==========================================================================
namespace bb_magic_moves {
    extern const bitboard_t r_mask[NB_BRD_SQR];
    extern const bitboard_t r_magic[NB_BRD_SQR];
    extern const int r_index[NB_BRD_SQR];

    extern const bitboard_t b_mask[NB_BRD_SQR];
    extern const bitboard_t b_magic[NB_BRD_SQR];
    extern const int b_index[NB_BRD_SQR];

    extern bitboard_t r_table[NB_BRD_SQR][4096];
    extern bitboard_t b_table[NB_BRD_SQR][512];

    inline bitboard_t rook_moves(const square_t & sq, const bitboard_t & occupied) {
        assert(SQONBOARD(sq));
        bitboard_t blockers = occupied & r_mask[sq]; 
        int key = (blockers * r_magic[sq]) >> (64 - r_index[sq]); 
        assert(key >= 0 && key < 4096); 
        return r_table[sq][key]; 
    } 
    inline bitboard_t bishop_moves(const square_t & sq, const bitboard_t & occupied) {
        assert(SQONBOARD(sq));
        bitboard_t blockers = occupied & b_mask[sq]; 
        uint64_t key = (blockers * b_magic[sq]) >> (64 - b_index[sq]); 
        assert(key >= 0 && key < 512); 
        return b_table[sq][key]; 
    }

    void init(); 
}

// ==========================================================================
// Section 5. Overload opertor between board and square 
// ==========================================================================
inline bitboard_t operator & (bitboard_t b, square_t s) {
  assert(s >= A1 && s <= H8);
  return b & SQUARE_BB[s];
}

inline bitboard_t operator | (bitboard_t b, square_t s) {
  assert(s >= A1 && s <= H8);
  return b | SQUARE_BB[s];
}

inline bitboard_t operator ^ (bitboard_t b, square_t s) {
  assert(s >= A1 && s <= H8);
  return b ^ SQUARE_BB[s];
}

inline bitboard_t & operator |= (bitboard_t & b, square_t s) {
  assert(s >= A1 && s <= H8);
  return b |= SQUARE_BB[s];
}

inline bitboard_t & operator ^= (bitboard_t & b, square_t s) {
  assert(s >= A1 && s <= H8);
  return b ^= SQUARE_BB[s];
}
// ==========================================================================
// Section 6. Compute moves
// ==========================================================================
template<piece_t> inline bitboard_t find_moves(square_t sq, bitboard_t & occupied = NULL_64); 

template<> inline bitboard_t find_moves<KING>(square_t sq, bitboard_t & occupied) {
    return bb_normal_moves::ki_moves(sq);
}

template<> inline bitboard_t find_moves<KNIGHT>(square_t sq,  bitboard_t & occupied) {
    return bb_normal_moves::kn_moves(sq);
}

template<> inline bitboard_t find_moves<BISHOP>(square_t sq, bitboard_t & occupied) {
    return bb_magic_moves::bishop_moves(sq, occupied); 
}

template<> inline bitboard_t find_moves<ROOK>(square_t sq, bitboard_t & occupied) {
    return bb_magic_moves::rook_moves(sq, occupied); 
}

template<> inline bitboard_t find_moves<QUEEN>(square_t sq, bitboard_t & occupied) {
    return bb_magic_moves::bishop_moves(sq, occupied) | bb_magic_moves::rook_moves(sq, occupied); 
}

// ==========================================================================
// Section 7. To String operator for square 
// ==========================================================================
inline std::ostream & operator << (std::ostream & os, const square_t sq64) {
  switch (sq64) {
    case A1: os << "a1"; break;
    case A2: os << "a2"; break;
    case A3: os << "a3"; break;
    case A4: os << "a4"; break;
    case A5: os << "a5"; break;
    case A6: os << "a6"; break;
    case A7: os << "a7"; break;
    case A8: os << "a8"; break;
    case B1: os << "b1"; break;
    case B2: os << "b2"; break;
    case B3: os << "b3"; break;
    case B4: os << "b4"; break;
    case B5: os << "b5"; break;
    case B6: os << "b6"; break;
    case B7: os << "b7"; break;
    case B8: os << "b8"; break;
    case C1: os << "c1"; break;
    case C2: os << "c2"; break;
    case C3: os << "c3"; break;
    case C4: os << "c4"; break;
    case C5: os << "c5"; break;
    case C6: os << "c6"; break;
    case C7: os << "c7"; break;
    case C8: os << "c8"; break;
    case D1: os << "d1"; break;
    case D2: os << "d2"; break;
    case D3: os << "d3"; break;
    case D4: os << "d4"; break;
    case D5: os << "d5"; break;
    case D6: os << "d6"; break;
    case D7: os << "d7"; break;
    case D8: os << "d8"; break;
    case E1: os << "e1"; break;
    case E2: os << "e2"; break;
    case E3: os << "e3"; break;
    case E4: os << "e4"; break;
    case E5: os << "e5"; break;
    case E6: os << "e6"; break;
    case E7: os << "e7"; break;
    case E8: os << "e8"; break;
    case F1: os << "f1"; break;
    case F2: os << "f2"; break;
    case F3: os << "f3"; break;
    case F4: os << "f4"; break;
    case F5: os << "f5"; break;
    case F6: os << "f6"; break;
    case F7: os << "f7"; break;
    case F8: os << "f8"; break;
    case G1: os << "g1"; break;
    case G2: os << "g2"; break;
    case G3: os << "g3"; break;
    case G4: os << "g4"; break;
    case G5: os << "g5"; break;
    case G6: os << "g6"; break;
    case G7: os << "g7"; break;
    case G8: os << "g8"; break;
    case H1: os << "h1"; break;
    case H2: os << "h2"; break;
    case H3: os << "h3"; break;
    case H4: os << "h4"; break;
    case H5: os << "h5"; break;
    case H6: os << "h6"; break;
    case H7: os << "h7"; break;
    case H8: os << "h8"; break;
    default: ERROR_MSG(ERROR_SQ_OS); break; 
  }
  return os; 
}
#endif