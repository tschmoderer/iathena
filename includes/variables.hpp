#ifndef VARIABLES_HPP
#define VARIABLES_HPP

#include <iostream>

#include "constants.hpp"

#define RAND_64 ((uint64_t)rand() | (uint64_t)rand() << 15 | (uint64_t)rand() << 30 | (uint64_t)rand() << 45 | ((uint64_t)rand() & 0xf) << 60)  

/// enum of square on the board 
#define SQ_NB 64
enum square_t : uint8_t {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8, 
  SQ_NONE = 64
};

#define FILE_NB 8
#define RANK_NB 8

enum file_t : uint8_t {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE = 8}; ///< defining the file names 
enum rank_t : uint8_t {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE = 8}; ///< defining the rank names 

constexpr square_t FR2SQ64(file_t f, rank_t r)  {return square_t(f + 8*r);} ///< convert file rank to square 
constexpr rank_t   SQ64TOR(square_t sq64) {return rank_t(sq64 >> 3) ;} ///< get the rank of a square 
constexpr file_t   SQ64TOF(square_t sq64) {return file_t(sq64 & 0x7);} ///< get the file of a square

/* pieces */ 
#define PIECE_TYPE_NB 6
#define PIECE_NB 12
#define COLOR_NB 2

enum piece_t : uint8_t {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECE_TYPE_NONE = 6};  ///< define piece_type
enum Piece   : uint8_t {wP, bP, wN, bN, wB, bB, wR, bR, wQ, bQ, wK, bK, PIECE_NONE = 12};    ///< list all chess pieces
enum color_t : uint8_t {WHITE, BLACK, COLOR_NONE = 2}; /// enum all colors 

/* pieces converters */
const piece_t piece2type[13]  = {PAWN, PAWN, KNIGHT, KNIGHT, BISHOP, BISHOP, ROOK, ROOK, QUEEN, QUEEN, KING, KING, PIECE_TYPE_NONE};
const color_t piece2color[13] = {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, COLOR_NONE};

/// enumerate all castle options 
enum castle_t : uint8_t {
  WKCA = 1 << 1, 
  WQCA = 1 << 2, 
  BKCA = 1 << 3, 
  BQCA = 1 << 4
}; 

#define DIRECTION_NB 8
/// @brief enumerate all possible direction for a move
enum direction_t : uint8_t {
  NORTH, EAST, SOUTH, WEST, 
  NORTH_EAST, NORTH_WEST, 
  SOUTH_EAST, SOUTH_WEST, 
  DIRECTIONS_NONE = 8
};

/// enumerate all move types 
enum move_t : uint16_t {
  NORMAL, 
  PROMOTION = 1 << 14, 
  ENPASSANT = 2 << 14, 
  CASTLING  = 3 << 14
}; 

/* helpers checkers */
constexpr bool SQONBOARD(square_t sq64) {return ((sq64) >= A1 && (sq64) <= H8);} ///< assert the square is on the board 
constexpr bool FILEONBOARD(file_t file) {return ((file) >= FILE_A && (file) <= FILE_H);} ///< assert the file is on the board
constexpr bool RANKONBOARD(rank_t rank) {return ((rank) >= RANK_1 && (rank) <= RANK_8);} ///< assert the rank is on the board
constexpr bool isPa(Piece pce) {return (pce == bP || pce == wP);} ///< assert the piece is a pawn 
constexpr bool isKi(Piece pce) {return (pce == bK || pce == wK);} ///< assert the piece is a king 
constexpr bool isKn(Piece pce) {return (pce == bN || pce == wN);} ///< assert the piece is a knight 
constexpr bool isBQ(Piece pce) {return (pce == bB || pce == wB || pce == bQ || pce == wQ);} ///< assert the piece is a bishop or queen 
constexpr bool isRQ(Piece pce) {return (pce == bR || pce == wR || pce == bQ || pce == wQ);} ///< assert the piece is a rook or queen 

// helper for output 
const char pceChar[]  = "PpNnBbRrQqKk.";
const char sideChar[] = "wb-";
const char rankChar[] = "12345678";
const char fileChar[] = "abcdefgh";

#endif