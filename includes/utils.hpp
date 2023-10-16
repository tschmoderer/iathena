#ifndef UTILS_HPP
#define UTILS_HPP

#include "variables.hpp"

// increment operator 
inline square_t   operator +  (square_t s, int i) {return square_t(int(s) + i);}
inline square_t & operator ++ (square_t & sq, int ) {return sq = square_t(int(sq) + 1);}
inline square_t & operator -- (square_t & sq, int ) {return sq = square_t(int(sq) - 1);}

inline file_t & operator += (file_t& d1, int d2) { return d1 = file_t(int(d1) + d2);}
inline file_t & operator ++ (file_t & f, int ) { return f = file_t(int(f) + 1);}
inline file_t & operator -- (file_t & f, int ) { return f = file_t(int(f) - 1);}
inline std::ostream & operator << (std::ostream & os, const file_t & f) {os << unsigned(f); return os;}

inline rank_t & operator += (rank_t& d1, int d2) { return d1 = rank_t(int(d1) + d2);}
inline rank_t & operator ++ (rank_t & r, int ) { return r = rank_t(int(r) + 1);}
inline rank_t & operator -- (rank_t & r, int ) { return r = rank_t(int(r) - 1);}
inline std::ostream & operator << (std::ostream & os, const rank_t & r) {os << unsigned(r); return os;}

inline piece_t & operator++  (piece_t & pt, int )  {return pt = piece_t(int(pt)+1);}
inline direction_t & operator++  (direction_t & d, int )  {return d = direction_t(int(d)+1);}

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