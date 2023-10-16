#ifndef POSITION_HPP
#define POSITION_HPP

#include <iostream>
#include "utils.hpp"
#include "bb.hpp"
#include "move.hpp"

/**
 * @brief A chess position is represented by:
 * - Six 64 bits numbers to locate the squares occupied by each piece type. 
 * - Two 64 bits numbers to locate the squares occupied by each color. 
 * 
 * Then is a 16 bits number we store : 
 * -# 0000 0000 0000 0001 : 1 bit   : side who should play next move {WHITE = 0, BLACK = 1} 
 * -# 0000 0000 0001 1110 : 4 bits  : castle permission
 * -# 0000 0001 1110 0000 : 4 bits  : file of en passant square, this is set only if there is a pawn of the opposite side capable of making the en-passant capture
 * -# 1111 1110 0000 0000 : 7 bits  : 50 moves rules counter - Halfmove clock
 * 
 * - A 64 bits unique key.
 * - A 8 bits number : Fullmove number: The number of the full moves. It starts at 1 and is incremented after Black's move.
**/
class Position {
    public:
        /* CONSTRUCTOR */
        Position() = delete; 
        Position(const char * ); 
        Position(const Position & ) = delete; 

        /* DESTRUCTOR */
        ~Position();

        /* GETTERS */

        /* SETTERS */
        void set_position(const char * ); 

        /* METHODS */
        Move parse_move(const char * pMV) const;

        bool move(const Move & );
        void unmove(const Move & );

        /* IO METHODS */ 
        std::string print_fen() const; 

        /* FRIENDS */
        friend std::ostream & operator << (std::ostream & , const Position & ); 

    private:
        /* ATTRIBUTES */
        bitboard_t BB_pieces_by_type[PIECE_TYPE_NB]; ///< pieces by type PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
        bitboard_t BB_pieces_by_color[COLOR_NB];     ///< occupied by color_t
        
        uint16_t    informations; ///< contains: - side to move, castle privileges, file of en-passant square, 50 moves rule count
        uint8_t     fullmove;     ///< Fullmove counter
        board_key_t key;          ///< unique key identifier of the board

        /* HELPERS */
        Piece state[NB_BRD_SQR];               ///< piece of square = {bP, ..., bK, EMPTY}
        //uint8_t  pieces_count[PIECE_NB];             ///< number of each piece {bP, ..., wK} 
        //square_t pieces_list[PIECE_NB][MAX_PIECES];  ///< position of ieme piece 
        //uint8_t  index[NB_BRD_SQR];                  ///< on each square the index of the piece in piece list 

        /* METHODS */
        void parse_fen(const char * );

        void add_piece(const Piece & , const square_t & );
        void remove_piece(const Piece & , const square_t & );
        void move_piece(const Piece & , const square_t & , const square_t & );
        
        bool can_castle(const castle_t & ) const; 

        void reset_board();

        /* GETTERS */
        Piece       get_piece(const square_t & ) const;
        uint8_t     get_fullmove() const; 
        board_key_t get_key() const;
        color_t     get_side() const; 
        castle_t    get_ca_permission() const; 
        file_t      get_en_passant_file() const;
        uint8_t     get_50_moves_rule() const; 

        /* SETTERS */
        void set_fullmove(const uint8_t & ); 
        void set_side(const color_t & ); 
        void set_ca_permission(const castle_t & ); 
        void set_en_passant_file(const file_t & );
        void set_50_moves_rule(const uint8_t & ); 
};

#endif