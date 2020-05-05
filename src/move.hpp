/**
 * @file move.hpp
 * @author Timothée Schmoderer (iathena@mailo.com)
 * @brief 
 * @version 1.0
 * @date 2019-02-19
 * 
 * @copyright Copyright (c) 2019. This project is released under the GNU General Public License
 * 
*/

#ifndef MOVE_HPP
#define MOVE_HPP

#include "types.hpp"
#include "bb.hpp"

#define NOMOVE 0

/** 
 * @brief Describe the structure of a move 
 * 
 * A move needs 16 bits to be stored 
 * -# bit 0  - 5 : destination square (from A1 to H8) 
 * -# bit 6  - 11: origin square (from A1 to H8)
 * -# bit 12 - 13: promotion piece : KN (0), BI (1), RK (2), Q(3)
 * -# bit 14 - 15: special move flag : promotion (1), en passant (2), castle (3)
 * 
 * Example : 
 * -# 0000 0000 0000 0000 : NOMOVE      : 0
 * -# 0000 0000 0011 1111 : TO_SQUARE   : 0x3F
 * -# 0000 1111 1100 0000 : FROM_SQUARE : (>> 6) & 0x3F
 * -# 0011 0000 0000 0000 : PROM_PIECE  : (>> 12) & 3 (KNIGHT, BISHOP, ROOK, QUEEN)
 * -# 1100 0000 0000 0000 : MOVE_TYPE   : (>> 14) & 3 (NORMAL, PROMOTION, ENPASSANT, CASTLING)
*/

/** 
 * @todo remove copy constructor
**/
class MOVE {
    public: 
        /* constructors */ 
        inline MOVE() {
            this->move = NOMOVE; 
            this->score = 0; }
        
        inline MOVE(uint16_t m, int s) {
            this->move = m; 
            this->score = s; }

       inline MOVE(square_t from, square_t to, move_t T = NORMAL, piece_t pp = KNIGHT, int sc = 0) {
            this->move  = (T + ((pp - KNIGHT) << 12) + (from << 6) + to);
	        this->score = sc; }

        // MOVE(const MOVE & ) = delete; 

        inline MOVE(const MOVE & mv) {
            this->move  = mv.move; 
            this->score = mv.score;} 
        
        /* destructors */ 
        // inline ~MOVE() {}

        /* getters */ 
        constexpr int get_score()     const {return this->score;}
        constexpr uint16_t get_move() const {return this->move;}

        constexpr square_t from_sq()        const {return square_t((this->move >> 6) & 0x3F);}
        constexpr square_t to_sq()          const {return square_t(this->move & 0x3F);}
        constexpr piece_t  promotion_type() const {return piece_t(((this->move >> 12) & 3) + KNIGHT);}
        constexpr move_t   type_of()        const {return move_t(this->move & (3 << 14));}

        /* setters */ 
        inline void set_score(const int s) {this->score = s;}

        /* operators */ 

        /* IO stream */ 
        inline friend std::ostream & operator << (std::ostream & os, const MOVE & mv) {
            if (mv.from_sq() == mv.to_sq()) {
                std::cerr << "WARNING! Print illegal move : " << mv.from_sq() << " " << mv.to_sq() << " ; mv : " << mv.move << std::endl; 
            }
            if (mv.move == NOMOVE) {
                os << "0000"; 
            } else {
                os << mv.from_sq() << mv.to_sq(); 
                if (mv.type_of() == PROMOTION) {
                    switch (mv.promotion_type()) {
                        case KNIGHT : os << "n"; break; 
                        case BISHOP : os << "b"; break; 
                        case ROOK 	: os << "r"; break; 
                        case QUEEN  : os << "q"; break; 
                        default : ERROR_MSG(ERROR_MV_OS); break;  
                    }
                }
            }
            return os; 
        }

    private:        
        uint16_t move; 
        int score; 
}; 
#endif