#include "board.hpp"
#include <math.h>       /* pow */
#include <inttypes.h>   // PRIX64

void BOARD::reset_board() {
    for (piece_t pt = PAWN; pt <= KING; pt++) {
        this->BB_pieces_by_type[pt] = NULL_64; 
    }
    this->BB_pieces_by_color[WHITE] = NULL_64; 
    this->BB_pieces_by_color[BLACK] = NULL_64; 

    for (square_t sq64 = A1; sq64 <= H8; sq64++) {
        this->position[sq64] = EMPTY; 
    }

    for (int i = 0; i < PIECE_NB; i++) {
        this->pieces_count[i] = 0; 
        for (int j = 0; j < MAX_PIECES; j++) {
            this->pieces_list[i][j] = NO_SQ; 
        }
    }

    for (square_t sq = A1; sq <= H8; sq++) {
        this->index[sq] = 0; 
    }

    // not necessary but for completeness 
    for (unsigned int idx = 0; idx < MAX_GAME_PLIES; idx++) {
        this->game[idx].captured = EMPTY; 
        this->game[idx].key = NULL_64; 
        this->game[idx].caracteristics = 0; 
        this->game[idx].mv = MOVE(); 
    }

    this->ply = 0; 
    this->set_en_passant_file(FILE_NONE); this->game[0].key = NULL_64;
    this->search_ply = 0; 
    this->variant = standart; 
}

void BOARD::parse_fen(const char * fen) {
    assert(fen != NULL); 
    
    this->reset_board(); 

    rank_t rank = RANK_8;
    file_t file = FILE_A;

    // set pieces 
    while ((rank >= RANK_1) && *fen) {
        int count = 1; 
        Piece pce = EMPTY; 
        switch (*fen) {
            case 'p': pce = bP; break;
            case 'r': pce = bR; break;
            case 'n': pce = bN; break;
            case 'b': pce = bB; break;
            case 'k': pce = bK; break;
            case 'q': pce = bQ; break;
            case 'P': pce = wP; break;
            case 'R': pce = wR; break;
            case 'N': pce = wN; break;
            case 'B': pce = wB; break;
            case 'K': pce = wK; break;
            case 'Q': pce = wQ; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                pce = EMPTY;
                count = *fen - '0';
                break;

            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;

            default: ERROR_MSG(ERROR_PARSE_FEN); break; 
        }
        if (pce != EMPTY) {
            this->add_piece(pce, FR2SQ64(file,rank));
            file ++; 
        } else {
            file += count;
        }
		fen++;
    }

    // set side 
    assert(*fen == 'w' || *fen == 'b');
    this->set_side((*fen == 'w') ? WHITE : BLACK);
	fen += 2;

    // set castle permission
	for (int i = 0; i < 4; i++) {
        if (*fen == ' ') {
            break;
        }
		switch(*fen) {
			case 'K': this->set_ca_permission(WKCA); break;
			case 'Q': this->set_ca_permission(WQCA); break;
			case 'k': this->set_ca_permission(BKCA); break;
			case 'q': this->set_ca_permission(BQCA); break;
			default: break;
        }
		fen++;
	}
	fen++;

    // Set "en passant" square
	if (*fen != '-') {
		file = file_t(fen[0] - 'a');
		rank = rank_t(fen[1] - '1');

		assert(file >= FILE_A && file <= FILE_H);
		assert(rank >= RANK_1 && rank <= RANK_8);

        this->set_en_passant_file(file); 

        fen += 1; 
    }
    fen ++; 
    assert(*fen == ' ');

    // set fifty moves 
    fen++;
    int p       = 0; 
    int fiftyMv = 0; 
    while (*fen != ' ') {
        assert(*fen-'0'>= 0 && *fen-'0' <= 9); 
        fiftyMv = fiftyMv*pow(10,p) + (*fen-'0'); 
        p++;
        fen++; 
    }
    this->set50Moves(fiftyMv);

    // set ply
    fen++;
    p = 0; 
    uint16_t ply = 0; 
    while (*fen != ' ' && *fen != '\000' && *fen != '\n') {
        assert(*fen-'0'>= 0 && *fen-'0'<=9); 
        ply = ply*pow(10,p) + (*fen-'0'); 
        p++;
        fen++; 
    }
    // ply to move 
    // this has to be 0 by default 
    this->ply = 0; 
    // this->ply = 2*ply + this->get_side() - 1; 

    assert(this->check_board()); 
}

void BOARD::parse_position(const char * line) {
    const char *ptrChar;
    this->reset_board(); 
    // parse uci command : position [fen/startpos] [moves ] 
    if (strncmp(line,"startpos", 8) == 0){
        this->parse_fen(START_FEN);
    } else {
        ptrChar = strstr(line, "fen");
        if(ptrChar == NULL) {
            this->parse_fen(START_FEN);
        } else {
            ptrChar+= 4;
            this->parse_fen(ptrChar); 
        }
    }
	ptrChar = strstr(line, "moves");
	MOVE move; int cnt = 0;

	if (ptrChar != NULL) {
        ptrChar += 6;
        while(*ptrChar && *ptrChar != '\n') {
            move = this->parse_move(ptrChar);
            if (move.get_move() == NOMOVE) 
                break;
            if (this->move(&move) != false) {
                cnt++;
                this->search_ply = 0; 
                
                /*
                this->now--; 
                this->game[this->now].caracteristics = this->game[this->now+1].caracteristics;  
                this->game[this->now].key = this->game[this->now+1].key;  
                this->game[this->now].mv = MOVE(); 
                this->game[this->now].captured = EMPTY;  
                */
            } else {
                ERROR_MSG(ERROR_PARSE_UCI_ILLEGAL_MOVE); 
            }

            while (*ptrChar && *ptrChar != ' ') ptrChar++;
            ptrChar++;
        }
    }
    assert(this->search_ply == 0); 
    assert(this->ply == cnt); 
#ifndef NDEBUG 
	this->show(); 
#endif
    assert(this->check_board()); 
}

std::ostream & operator << (std::ostream & os, const BOARD & brd) {
    for (rank_t rank = RANK_8; rank >= RANK_1; rank--) {
        int count = 0; 
        for (file_t file = FILE_A; file <= FILE_H; file++) {
            Piece pce = brd.get_piece(FR2SQ64(file,rank)); 
            switch (pce) {
                case EMPTY : count++; break; 
                default : 
                    if (count > 0) {
                        os << count; 
                        count = 0; 
                    }
                    os << pceChar[pce]; break; 
            }
        }
        if (count > 0) {
            os << count; 
        }
        if (rank > RANK_1) {
            os << '/';  
        }
    }
    os << ' '; 
    // show side 
    os << sideChar[brd.get_side()] << ' '; 

    // show castle permission
    bool castlePerm = true; 
    if (brd.can_castle(WKCA)) {
        os << 'K';
        castlePerm = false; 
    } 
    if (brd.can_castle(WQCA)) {
        os << 'Q';
        castlePerm = false; 
    } 
    if (brd.can_castle(BKCA)) {
        os << 'k';
        castlePerm = false; 
    } 
    if (brd.can_castle(BQCA)) {
        os << 'q';
        castlePerm = false; 
    } 
    if (castlePerm == true) 
        os << '-';
    
    os << ' ';

    // en passant 
    if (brd.get_en_passant_file() != FILE_NONE) {
        os << char('a'+brd.get_en_passant_file()) << char('0'+(-3*brd.get_side()+6));
    } else {
        os << '-'; 
    }

    uint8_t fmv = brd.get50Moves(); 
    os << ' ' << (int)fmv; 
    os << ' ' << ceil((brd.get_ply() + 2 - brd.get_side())/2.); 

    return os; 
}

#ifndef NDEBUG 
void BOARD::show() const {
    std::cout << "Game Board: " << std::endl << std::endl;  
	for (rank_t rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d  ",rank+1);
		for (file_t file = FILE_A; file <= FILE_H; file++) {
			int sq64 = FR2SQ64(file,rank);
			int piece = this->position[sq64];
			printf("%3c",pceChar[piece]);
		}
		printf("\n");
	}
	printf("\n   ");
	for (file_t file = FILE_A; file <= FILE_H; file++) {
		printf("%3c",'a'+file);
	}
	printf("\n\n");

    /* other caracteristics */
	printf("side: %c\n",sideChar[this->get_side()]);
    if (this->get_en_passant_file() != FILE_NONE) {
        printf("enPas in our convention: %c%c\n",char('a' + this->get_en_passant_file()),char('0'+(-3*this->get_side()+6)));
    } else {
        printf("enPas in our convention: - \n");
    }

    /* castle perm */
    if (this->game[this->ply].caracteristics & 0x1E) {
        printf("castle: %c%c%c%c\n",
			this->can_castle(WKCA) ? 'K' : '-',
			this->can_castle(WQCA) ? 'Q' : '-',
			this->can_castle(BKCA) ? 'k' : '-',
			this->can_castle(BQCA) ? 'q' : '-'
			);
    } else {
        printf("castle: -\n"); 
    }
	
    printf("ply: %d\nFity moves: %d\n",this->ply+1,this->get50Moves()); 
    printf("search_ply: %d\n\n",this->search_ply); 
	printf("key: %" PRIX64 "\n" ,this->game[this->ply].key);
    std::cout << "fen : " << *this << std::endl; 
    #ifndef NDEBUG
    if (this->check_board()) {
        printf("assert board : ok\n"); 
    } else {
        printf("assert board : error\n"); 
    }
    #endif
}
#endif