#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <inttypes.h>
#include <iostream>
#include <cassert>
// #include <unistd.h>
#include "string.h"

#if defined(_WIN64) && defined(_MSC_VER) // No Makefile used
  #include <intrin.h> // Microsoft header for _BitScanForward64()
  #define IS_64BIT
#endif

// ==========================================================================
// Section 0. General definitions and types 
// ==========================================================================	

/* types */
typedef uint64_t bitboard_t; 
typedef uint64_t board_key_t; 

#define NULL_64 0ULL
#define RAND_64 ((uint64_t)rand() | (uint64_t)rand() << 15 | (uint64_t)rand() << 30 | (uint64_t)rand() << 45 | ((uint64_t)rand() & 0xf) << 60)  

/* general caracteristics */
#define NAME    "iathena"
#define VERSION "0.0.0"
#define AUTHOR  "TIMOTHEE SCHMODERER"

/* game caracteristics */ 
#define MAX_GAME_PLIES     512  ///< max ply we can search per position
#define MAX_POSITION_MOVES 256  ///< max pseudo legal moves in one position 
#define MAX_PIECES 10 ///< max number of piece we can have on board /!\ should change to 36 for horde unless we don't store pawn in it 
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" ///< fen of standart start position 

///< possible chess variants 
enum chess_t : uint8_t {standart, chess960, hill, check3, antichess, atomic, horde, racing}; 

/* board caracteristics */ 
#define NB_BRD_SQR  64 ///< macro defining the number of square on a chess board

/// enum of square on the board 
enum square_t : uint8_t {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8, NO_SQ = 65};

enum file_t : int8_t {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE}; ///< defining the file names 
enum rank_t : int8_t {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE}; ///< defining the rank names 

constexpr square_t FR2SQ64(file_t f, rank_t r)  {return square_t(f + 8*r);} ///< convert file rank to square 
constexpr rank_t   SQ64TOR(square_t sq64) {return rank_t(sq64 >> 3) ;} ///< get the rank of a square 
constexpr file_t   SQ64TOF(square_t sq64) {return file_t(sq64 & 0x7);} ///< get the file of a square

/* pieces */ 
enum piece_t : uint8_t {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECE_TYPE_NB = 6, PIECE_TYPE_NONE};  ///< define piece_type
enum Piece   : uint8_t {bP, wP, bN, wN, bB, wB, bR, wR, bQ, wQ, bK, wK, PIECE_NB = 12, EMPTY = 12}; ///< list all chess pieces
enum color_t : uint8_t {WHITE, BLACK, COLOR_NB}; /// enum all colors 

/// enumerate all castle options 
enum castle_t : uint8_t {
  WKCA = 1 << 1, 
  WQCA = 1 << 2, 
  BKCA = 1 << 3, 
  BQCA = 1 << 4}; 

/// enumerate all move types 
enum move_t : uint16_t {
    NORMAL, 
    PROMOTION = 1 << 14, 
    ENPASSANT = 2 << 14, 
    CASTLING  = 3 << 14}; 

enum direction_t : uint8_t {
  NORTH, EAST, SOUTH, WEST, 
  NORTH_EAST, NORTH_WEST, 
  SOUTH_EAST, SOUTH_WEST, 
  DIRECTIONS_NB = 8
};

// ia 
#define INFINITE  32768               ///< infinite score 
#define MAX_DEPTH 64      ///< max search depth 
#define ISMATE (INFINITE - MAX_DEPTH) ///< mate score if under this limir
#define INPUT_BUFFER 400*6            ///< max character in an UCI command

///< enum IA implemented 
enum ia_t : uint8_t {RAND, ALPHABETA}; 

// ==========================================================================
// Section 1. General helpers 
// ==========================================================================	

const piece_t piece2type[13]  = {PAWN, PAWN, KNIGHT, KNIGHT, BISHOP, BISHOP, ROOK, ROOK, QUEEN, QUEEN, KING, KING, PIECE_TYPE_NONE};
const color_t piece2color[13] = {BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, COLOR_NB};
const int piece_value[13]     = {100, 100, 325, 325, 325, 325, 550, 550, 1000, 1000, 50000, 50000, 0};

/// mirror board 
const square_t MIRROR_64[NB_BRD_SQR] = {
  A8, B8, C8, D8, E8, F8, G8, H8, 
  A7, B7, C7, D7, E7, F7, G7, H7,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A1, B1, C1, D1, E1, F1, G1, H1};

constexpr bool SQONBOARD(square_t sq64) {return ((sq64) >= A1 && (sq64) <= H8);} ///< assert the square is on the board 
constexpr bool FILEONBOARD(file_t file) {return ((file) >= FILE_A && (file) <= FILE_H);} ///< assert the file is on the board
constexpr bool RANKONBOARD(rank_t rank) {return ((rank) >= RANK_1 && (rank) <= RANK_8);} ///< assert the rank is on the board
constexpr bool isPa(Piece pce) {return (pce == bP || pce == wP);} ///< assert the piece is a pawn 
constexpr bool isKi(Piece pce) {return (pce == bK || pce == wK);} ///< assert the piece is a king 
constexpr bool isKn(Piece pce) {return (pce == bN || pce == wN);} ///< assert the piece is a knight 
constexpr bool isBQ(Piece pce) {return (pce == bB || pce == wB || pce == bQ || pce == wQ);} ///< assert the piece is a bishop or queen 
constexpr bool isRQ(Piece pce) {return (pce == bR || pce == wR || pce == bQ || pce == wQ);} ///< assert the piece is a rook or queen 

// for each move 
// castle_permission &= castleperm[from]
// castle_permission &= castleperm[to]
// will auto change castle permission if needed
///@warning this array is strongly linked with the method update_ca_perm of BOARD
const uint32_t castlePerm[NB_BRD_SQR] = {
  0xFFFFFFFB, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFF9, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFD,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFEF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFE7, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFF7
};

// most valuable victim less valuable attacker 
// mvvlva[pieceType(victim)][pieceType(attacker)]
#define CAPTURE_SCORE 1048576
#define FIRST_KILLER_SCORE 524288
#define SECOND_KILLER_SCORE 262144

const unsigned int mvvLva[7][6] = {
  {105 + CAPTURE_SCORE, 104 + CAPTURE_SCORE, 103 + CAPTURE_SCORE, 102 + CAPTURE_SCORE, 101 + CAPTURE_SCORE, 100 + CAPTURE_SCORE},
  {205 + CAPTURE_SCORE,	204 + CAPTURE_SCORE, 203 + CAPTURE_SCORE, 202 + CAPTURE_SCORE, 201 + CAPTURE_SCORE, 200 + CAPTURE_SCORE},
  {305 + CAPTURE_SCORE,	304 + CAPTURE_SCORE, 303 + CAPTURE_SCORE,	302 + CAPTURE_SCORE, 301 + CAPTURE_SCORE,	300 + CAPTURE_SCORE},
  {405 + CAPTURE_SCORE,	404 + CAPTURE_SCORE, 403 + CAPTURE_SCORE,	402 + CAPTURE_SCORE, 401 + CAPTURE_SCORE,	400 + CAPTURE_SCORE},
  {505 + CAPTURE_SCORE,	504 + CAPTURE_SCORE, 503 + CAPTURE_SCORE,	502 + CAPTURE_SCORE, 501 + CAPTURE_SCORE,	500 + CAPTURE_SCORE},
  {605 + CAPTURE_SCORE,	604 + CAPTURE_SCORE, 603 + CAPTURE_SCORE,	602 + CAPTURE_SCORE, 601 + CAPTURE_SCORE,	600 + CAPTURE_SCORE},
  {0,	  0,	 0,	  0,	 0,	  0  }};

// increment operator 
inline square_t operator +  (square_t s, int i) {return square_t(int(s) + i);}
inline file_t & operator += (file_t& d1, int d2) { return d1 = file_t(int(d1) + d2);}
inline rank_t & operator += (rank_t& d1, int d2) { return d1 = rank_t(int(d1) + d2);}
inline square_t & operator++ (square_t & sq, int ) {return sq = square_t(int(sq) + 1);}
inline file_t & operator++   (file_t & f, int ) { return f = file_t(int(f) + 1);}
inline file_t & operator--   (file_t & f, int ) { return f = file_t(int(f) - 1);}
inline rank_t & operator++   (rank_t & r, int ) { return r = rank_t(int(r) + 1);}
inline rank_t & operator--   (rank_t & r, int ) { return r = rank_t(int(r) - 1);}
inline piece_t & operator++  (piece_t & pt, int )  {return pt = piece_t(int(pt)+1);}
inline Piece & operator++    (Piece & p, int )  {return p = Piece(int(p)+1);}
inline direction_t & operator++  (direction_t & d, int )  {return d = direction_t(int(d)+1);}


/* extern definitions */
extern board_key_t keys[781]; ///< seed for keys 

// ==========================================================================
// Section 2. General debugger helpers 
// ==========================================================================	

#if !defined(NPERFT) || !defined(NMIRROR)
	#define NB_PERFT 128
  extern const char * perft_fen[NB_PERFT]; ///< fen string for performance test 
  extern const long perft_depth_results[6][NB_PERFT]; ///< perft nodes results 
#endif

/* error codes */ 
#define ERROR_MV_OS 1
#define ERROR_SQ_OS 2
#define ERROR_PARSE_FEN 3
#define ERROR_SET_CA 4
#define ERROR_UNSET_CA 5
#define ERROR_MV_CA 6
#define ERROR_UMV_CA 7
#define ERROR_EVAL_NON_SYMETRIC 8
#define ERROR_PARSE_UCI_ILLEGAL_MOVE 9
#define ERROR_ILLEGAL_MOVE_PRINT 10
#define ERROR_MSG(code) std::cerr << "ERROR: " << code << std::endl; exit(code)

#ifndef NDEBUG
  // definitions needed only in debug mode 
  #define pceValid(pce) (pce >= bP && pce < EMPTY)
  #define pceValidOrEmpty(pce) (pce >= bP && pce <= EMPTY)
#endif 

// helper for output 
const char pceChar[]  = "pPnNbBrRqQkK.";
const char sideChar[] = "wb-";
const char rankChar[] = "12345678";
const char fileChar[] = "abcdefgh";

#endif // END TYPES_HPP
