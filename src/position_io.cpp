#include "../includes/position.hpp"

std::string Position::print_fen() const {
    std::string fen = "";
    for (rank_t rank = RANK_8; rank < RANK_NB; rank--) {
        int count = 0; 
        for (file_t file = FILE_A; file < FILE_NB; file++) {
            Piece pce = this->get_piece(FR2SQ64(file, rank)); 
            switch (pce) {
                case PIECE_NONE : 
                    count++; 
                    break; 
                default : 
                    if (count > 0) {
                        fen += std::to_string(count); 
                        count = 0; 
                    }
                    fen += pceChar[pce]; 
                    break; 
            }
        }
        if (count > 0) {
            fen += std::to_string(count); 
        }
        if (rank > RANK_1) {
            fen += '/';  
        }
    }
    fen += ' '; 

    // show side 
    fen += sideChar[this->get_side()]; 
    fen += ' '; 

    // show castle permission
    bool castlePerm = true; 
    if (this->can_castle(WKCA)) {
        fen += 'K';
        castlePerm = false; 
    } 
    if (this->can_castle(WQCA)) {
        fen += 'Q';
        castlePerm = false; 
    } 
    if (this->can_castle(BKCA)) {
        fen += 'k';
        castlePerm = false; 
    } 
    if (this->can_castle(BQCA)) {
        fen += 'q';
        castlePerm = false; 
    } 
    if (castlePerm == true) 
        fen += '-';
    
    fen += ' ';

    // en passant 
    if (this->get_en_passant_file() != FILE_NONE) {
        fen += char('a'+this->get_en_passant_file());
        fen += char('0'+(-3*this->get_side()+6));
    } else {
        fen += '-'; 
    }

    uint8_t fmv = this->get_50_moves_rule(); 
    fen += ' '; 
    fen += std::to_string(fmv); 
    fen += ' '; 
    //fen += std::to_string(int(ceil((this->get_fullmove() + 2 - this->get_side())/2.))); 
    fen += std::to_string(this->get_fullmove());

    return fen;
}

void Position::parse_fen(const char * fen) {
    this-> reset_board();

    rank_t rank = RANK_8;
    file_t file = FILE_A;

    // set pieces 
    while ((rank < RANK_NB) && *fen) {
        int count = 1; 
        Piece pce = PIECE_NONE; 
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
                pce = PIECE_NONE;
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
        if (pce != PIECE_NONE) {
            this->add_piece(pce, FR2SQ64(file, rank));
            file++; 
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
    this->set_50_moves_rule(fiftyMv);

    // set fullmove
    fen++;
    p = 0; 
    int fullmove = 0; 
    while (*fen != ' ' && *fen != '\000' && *fen != '\n') {
        assert(*fen-'0'>= 0 && *fen-'0'<=9); 
        fullmove = fullmove*pow(10, p); 
        fullmove += (*fen-'0'); 
        p++;
        fen++; 
    }
    // fullmove to move 
    // this has to be 0 by default 
    //this->fullmove = 0; 
    //this->fullmove = 2*fullmove + this->get_side() - 1; 
    this->set_fullmove(fullmove);
}

std::ostream & operator << (std::ostream & os, const Position & pos) {
    os << "   + - - - - - - - - - - - - - - - - - - - - - - - +" << std::endl;
	for (rank_t rank = RANK_8; rank < RANK_NB; rank--) {
        os << rank + 1 << "  |  ";
		for (file_t file = FILE_A; file < FILE_NB; file++) {
			square_t sq64 = FR2SQ64(file, rank);
			Piece piece = pos.get_piece(sq64);
            os << pceChar[piece] << "  |  ";
		}
		os << std::endl;
	}
    os << "   + - - - - - - - - - - - - - - - - - - - - - - - +" << std::endl;
    os << "   |  ";
	for (file_t file = FILE_A; file < FILE_NB; file++) {
        os << char('a' + file) << "  |  ";
	}

    os << std::endl << std::endl;
    os << "Caracteristics : " << "0x" << std::hex << std::uppercase << pos.informations << std::endl; 
    os << "Key : " << "0x" << pos.key << std::endl << std::dec ; 

    os << "Side : " << sideChar[pos.get_side()] << std::endl; 

    os << "Castle perm : "; 
    bool castlePerm = true; 
    if (pos.can_castle(WKCA)) {
        os << 'K';
        castlePerm = false; 
    }
    if (pos.can_castle(WQCA)) {
        os << 'Q';
        castlePerm = false; 
    }
    if (pos.can_castle(BKCA)) {
        os << 'k';
        castlePerm = false; 
    }
    if (pos.can_castle(BQCA)) {
        os << 'q';
        castlePerm = false; 
    }
    if (castlePerm) {
        os << "-";
    }
    
    os << std::endl;
    // en passant 
    os << "En passant square : ";
    if (pos.get_en_passant_file() != FILE_NONE) {
        os << char('a'+pos.get_en_passant_file()) << char('0'+(-3*pos.get_side()+6));
    } else {
        os << '-'; 
    }
    os << std::endl;

    uint8_t fmv = pos.get_50_moves_rule(); 
    os << "50 moves rule count : ";
    os << (int)fmv << std::endl; 
    os << "Nb fullmove : ";
    os << (int)pos.get_fullmove(); 
    os << std::endl;
    
    os << "FEN : " << pos.print_fen() << std::endl;
    
    return os; 
}
