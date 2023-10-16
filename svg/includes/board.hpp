#ifndef BOARD_HPP
#define BOARD_HPP

#include "types.hpp"
#include "bb.hpp"
#include "move.hpp"

/**
 * @brief Board structure is reprensented by an array of 64 squares. On 14/03/2019 a board object is 49400 bytes (mostly due to history)
 * 
 * On each square we give the piece type. 
 * 
 * Then is a 16 bits number we store : 
 * -# 0000 0000 0000 0001 : 1 bit   : side who should play next move {WHITE = 0, BLACK = 1} 
 * -# 0000 0000 0001 1110 : 4 bits  : castle permission
 * -# 0000 0001 1110 0000 : 4 bits  : file of en passant square, this is set only if there is a pawn of the opposite side capable of making the en-passant capture
 * -# 1111 1110 0000 0000 : 7 bits  : 50 moves rules counter 
**/

typedef struct {
    MOVE mv;                 ///< the move made 
    Piece captured;          ///< the captured piece if needed (necessary )
    uint16_t caracteristics; ///< board caracteristics of the board : 50mv, ep_file, castle permission, side to move 
    board_key_t key;         ///< the key of the actual position
} history_t;

typedef struct {
    MOVE moves[MAX_POSITION_MOVES]; 
    int cnt; 
} movelist_t; 

class BOARD {
    public: 
        /* constructors */
        BOARD() = delete; 
        BOARD(const char * ); 
        BOARD(const BOARD & ) = delete; 

        /* destructor */
        ~BOARD();

        /* getters */
        color_t get_side()                  const; 
        file_t  get_en_passant_file()       const;
        uint8_t get50Moves()                const;
        bool    can_castle(const castle_t ) const;
        board_key_t  get_key()              const;
        int get_ply()              const;
        int get_search_ply()       const; 
        chess_t      get_variant()          const; 
        Piece        get_piece(const square_t ) const; 

        /* setters */
        ///@warning should be used only once per game 
        void set_side(const color_t );
        void set_ca_permission(castle_t );
        void update_ca_permission(const uint32_t ); ///@todo improve perf 
        void set_en_passant_file(const file_t ); ///@warning this function check if there is a pawn that can capture en passant and update key 

        void set50Moves(const uint8_t );
        void reset50Moves();
        void increment50Moves();
        
        void clear_piece(const Piece & , const square_t & );                 ///< remove a piece from a quare 
        void add_piece(const Piece & , const square_t & );     ///< add a piece on a square 
        void move_piece(const Piece & , const square_t & , const square_t & ); ///< move a piece 

        void switch_side(); 

        /* IO stream */
        void parse_fen(const char * ); 
        void parse_position(const char  * ); 
        friend std::ostream & operator << (std::ostream & , const BOARD & ); 

        /* methods */
        bool is_sq_attacked(const square_t , const color_t ) const; 
        bool is_in_check() const;
        bool is_in_zugswang() const; 

        void movegen(movelist_t * ) const; 
        void movegen_capture_pieces(movelist_t * ) const;
        bool move(const MOVE * ); 
        void unmove(); 
        void null_move(); 
        void null_unmove(); 

        MOVE parse_move(const char * ) const; 

        bool is_a_repetition() const; 
        bool threefold_repetition() const; 
        bool material_draw() const;
        template<chess_t> int evaluate() const;
        int evaluate() const; 
        void clear_search_heuristics();

        void save_killer(const uint16_t ); 
        void save_history(const square_t , const square_t , const int ); 

        #ifndef NMIRROR
            void mirror(); 
        #endif

        #ifndef NDEBUG
            bool check_board() const;
            board_key_t generate_key() const;  
            void show() const; 
        #endif

    private: 
        /* private attributes */
        bitboard_t BB_pieces_by_type[PIECE_TYPE_NB]; ///< pieces by type PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
        bitboard_t BB_pieces_by_color[COLOR_NB];     ///< occupied by color_t

        Piece position[NB_BRD_SQR];         ///< piece of square = {bP, ..., bK, EMPTY}
        uint8_t pieces_count[PIECE_NB];     ///< number of each piece {bP, ..., wK} 
        square_t pieces_list[PIECE_NB][MAX_PIECES]; ///< position of ieme piece 
        uint8_t index[NB_BRD_SQR]; ///< on each square the index of the piece in piece list 

        /* search heuristics */ 
        uint16_t search_killers[2][MAX_DEPTH]; 
        unsigned int search_history[EMPTY][NB_BRD_SQR]; 

        /*
            @todo change for a complete hsitroy with only key to detect repetition
            @todo and only a small history for search 
        */
        history_t game[MAX_GAME_PLIES]; ///< hold other game caracteristics
        int ply;               ///< holds where we are in the history --> the ply >= 0
        int search_ply;        ///< holds where we are in the search >= 0 and <= ply

        chess_t variant; ///< store the variant we are playing 

        /* private methods */
        void reset_board(); 

        void movegen_quiet_pieces(movelist_t * ) const;
        void generate_ep(movelist_t * ) const; 
        void generate_ca(movelist_t * ) const;

        template<piece_t> void generate_quiet_moves(movelist_t * , bitboard_t & , bitboard_t & ) const;
        template<piece_t> void generate_capture_moves(movelist_t * , bitboard_t & , bitboard_t & ) const;
};

inline BOARD::~BOARD() {}

/* getters */
inline color_t BOARD::get_side()                 const {return color_t(this->game[this->ply].caracteristics & 0x1);}
inline file_t  BOARD::get_en_passant_file()      const {return file_t((this->game[this->ply].caracteristics >> 5) & 0xF);};
inline uint8_t BOARD::get50Moves()               const {return this->game[this->ply].caracteristics >> 9;}
inline bool    BOARD::can_castle(const castle_t ca) const {return this->game[this->ply].caracteristics & ca;};
inline board_key_t  BOARD::get_key()            const {return this->game[this->ply].key;}
inline int BOARD::get_ply()            const {return this->ply;};
inline int BOARD::get_search_ply()     const {return this->search_ply;}
inline chess_t      BOARD::get_variant()        const {return this->variant;}; 
inline Piece        BOARD::get_piece(const square_t sq) const {assert(SQONBOARD(sq)); return this->position[sq];}; 

/* setters */
///@warning should be used only once per game 
inline void  BOARD::set_side(const color_t s) {    
    assert(s == WHITE || s == BLACK);
    this->game[this->ply].caracteristics ^= (-(s) ^ this->game[this->ply].caracteristics) & 1UL;
    if (s == WHITE) 
        this->game[this->ply].key ^= keys[780]; // hash key}
}

inline void  BOARD::set_ca_permission(castle_t ca) {
    this->game[this->ply].caracteristics ^= ca; 
    switch (ca) { // hash key
        case WKCA: this->game[this->ply].key ^= keys[768 + 0]; break; 
        case WQCA: this->game[this->ply].key ^= keys[768 + 1]; break;
        case BKCA: this->game[this->ply].key ^= keys[768 + 2]; break; 
        case BQCA: this->game[this->ply].key ^= keys[768 + 3]; break;
        default: ERROR_MSG(ERROR_SET_CA); break; 
    }
}

///@todo improve perf 
inline void BOARD::update_ca_permission(const uint32_t u_ca) {
    // update key 
    int offset = 768;
    switch (u_ca) {
        case 0xFFFFFFFD: // change in WKCA by moving H1 rook 
            if (this->can_castle(WKCA)) 
                this->game[this->ply].key ^= keys[offset + 0]; 
            break; 
        case 0xFFFFFFFB: // change in WQCA by moving A1 rook 
            if (this->can_castle(WQCA))
                this->game[this->ply].key ^= keys[offset + 1]; 
            break; 
        case 0xFFFFFFF9: // change in WKCA and WQCA by moving white king 
            if (this->can_castle(WKCA))
                this->game[this->ply].key ^= keys[offset + 0]; 
            if (this->can_castle(WQCA))
                this->game[this->ply].key ^= keys[offset + 1]; 
            break; 
        case 0xFFFFFFF7: // change in BKCA by moving H8 rook 
            if (this->can_castle(BKCA))
                this->game[this->ply].key ^= keys[offset + 2];  
            break; 
        case 0xFFFFFFEF: // change in BQCA by moving A8 rook 
            if (this->can_castle(BQCA))
                this->game[this->ply].key ^= keys[offset + 3]; 
            break; 
        case 0xFFFFFFE7: // change in BKCA and BQCA by moving black king
            if (this->can_castle(BKCA))
                this->game[this->ply].key ^= keys[offset + 2]; 
            if (this->can_castle(BQCA))
                this->game[this->ply].key ^= keys[offset + 3]; 
            break; 
        default: break; 
    }
    // update caracteristiques 
    this->game[this->ply].caracteristics &= u_ca; 
}

///@warning this function check if there is a pawn that can capture en passant and update key 
inline void BOARD::set_en_passant_file(const file_t file) {
    assert(file >= FILE_A && file <= FILE_NONE); 

    // hash out the game en passant file : 
    file_t previous_ep_file = this->get_en_passant_file(); 
    if (previous_ep_file != FILE_NONE) {
        this->game[this->ply].caracteristics = (this->game[this->ply].caracteristics & ~0x1E0) + (file << 5);
        this->game[this->ply].key ^= keys[772 + previous_ep_file]; 
    }

    if (file != FILE_NONE) {  
        color_t  side      = color_t(this->get_side()^1);       // side who made the pawn push 
        rank_t   ep_rank   = (side == WHITE) ? RANK_3 : RANK_6; // rank behind the pawn 
        square_t ep_sq     = FR2SQ64(file,ep_rank);             // square behind the pawn push 

        // if you have have an imaginary pawn just behind your pawn pushed that can eat an adbversary pawn then the en passant capture is possible 
        if (bb_normal_moves::pawn_capture(side, ep_sq) & this->BB_pieces_by_type[PAWN] & this->BB_pieces_by_color[side^1]) {
            this->game[this->ply].caracteristics = (this->game[this->ply].caracteristics & ~0x1E0) + ((file) << 5);
            this->game[this->ply].key ^= keys[772 + file];
        }
    }
}

inline void BOARD::set50Moves(const uint8_t mv50) {
    assert(mv50 >= 0 && mv50 < 127); 
    this->game[this->ply].caracteristics = (this->game[this->ply].caracteristics & ~0xFE00) + (mv50 << 9); 
}

inline void BOARD::reset50Moves() {this->game[this->ply].caracteristics &= 0x1FF;}
inline void BOARD::increment50Moves() {this->game[this->ply].caracteristics += (1 << 9);}

inline void BOARD::switch_side() {
    this->game[this->ply].caracteristics ^= 1; 
    this->game[this->ply].key ^= keys[780]; // hash key 
}
#endif
