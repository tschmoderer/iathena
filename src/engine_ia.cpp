#include "engine.hpp" 

using namespace std; 

void ENGINE::check_up() {
    if (this->info->getTimeSet() == true && this->get_time_ms() > this->info->getStopTime()){
        #ifndef NDEBUG 
            cout << "time out after " << this->get_time_ms() - this->info->getStartTime() << " engine should stop thinking" << endl; 
        #endif 
        this->info->setStopped(true); 
    }
    this->read_input(); 
}

void ENGINE::clear_before_search() {
    this->info->clear_before_search(); 
    assert(this->position->get_search_ply() == 0); 

    this->position->clear_search_heuristics(); 
}

void ENGINE::search() {
    MOVE best_move; 
    this->clear_before_search(); 

    switch (this->ia_type) {
        case ALPHABETA : best_move = this->ab_iterative(); break; 
        default        : best_move = this->random_move();  break; 
    }
    
    assert(best_move.get_move() != NOMOVE); 
    cout << "bestmove " << best_move << endl; 
}

MOVE ENGINE::random_move() {
    assert(this->position->check_board()); 
    #ifndef NDEBUG
        int legal = 0; 
    #endif
    movelist_t mvs[1]; this->position->movegen(mvs); 

    movelist_t legal_mvs[1]; legal_mvs->cnt = 0; 
    for (uint8_t idx = 0; idx < mvs->cnt; idx++) {
        if (this->position->move(& (mvs->moves[idx])) != false) {  
            #ifndef NDEBUG
                legal++; 
            #endif
            legal_mvs->moves[legal_mvs->cnt++] = mvs->moves[idx]; 
            this->position->unmove(); 
        }
    }

    assert(legal > 0); 
    return legal_mvs->moves[rand() % legal_mvs->cnt]; 
}

void ENGINE::pick_next_move(movelist_t * lst, const int mv_num) const {
    int best_score = 0; 
    int best_num   = mv_num; 

    for (int idx = mv_num; idx < lst->cnt; idx++) {
        if (lst->moves[idx].get_score() > best_score) {
            best_score = lst->moves[idx].get_score(); 
            best_num = idx; 
        }
    }

    MOVE tmp = lst->moves[mv_num]; 
    lst->moves[mv_num]   = lst->moves[best_num]; 
    lst->moves[best_num] = tmp; 
}

int ENGINE::quiescence(int alpha, int beta) {
    assert(this->position->check_board()); 
    assert(beta > alpha); 

    if ((this->info->getNodes() & 2047) == 0) 
        this->check_up(); 

    this->info->incrementNodes(); 
    int score = this->position->evaluate(); 
    
    assert(score > -INFINITE && score < INFINITE); 

    if (this->position->get_search_ply() > MAX_DEPTH - 1) {
        return score; 
    }    
    
    if (this->position->get50Moves() >= 100 || this->position->is_a_repetition() != false) {
        return 0;
    } 

    if (score >= beta) 
        return beta; 
    if (score > alpha) 
        alpha = score; 
    #ifndef NTEST
        int legal = 0; 
        int old_alpha = alpha;
    #endif
	movelist_t mvs[1]; 

    mvs->cnt = 0; 
    this->position->movegen_capture_pieces(mvs); 
    
	score = -INFINITE;

	for (int idx = 0; idx < mvs->cnt; idx++) {
        this->pick_next_move(mvs, idx); 

        if (this->position->move(& (mvs->moves[idx])) != false) {
            #ifndef NTEST 
            legal++; 
            #endif
            score = -this->quiescence(-beta, -alpha); 
            this->position->unmove(); 

            if (this->info->getStopped() == true) {
                return 0; 
            }

            if (score > alpha) {
                if (score >= beta) {
                    #ifndef NTEST
                    if (legal == 1) {
                        this->info->incrementFHF(); 
                    }
                    this->info->incrementFH(); 
                    #endif
                    return beta; 
                }
                alpha = score; 
            }
        }
    }

    assert(alpha >= old_alpha); 
	return alpha;
}

int ENGINE::ab(int alpha, int beta, int depth, bool do_null) {
    assert(this->position->check_board()); 

    if (depth <= 0) {
        return this->quiescence(alpha, beta); 
    }

    if ((this->info->getNodes() & 2047) == 0) {
        this->check_up(); 
    }

    this->info->incrementNodes(); 
    if (this->position->get_search_ply() && (this->position->is_a_repetition() || this->position->get50Moves() >= 100)) {
        return 0; 
    }

    if (this->position->get_search_ply() > MAX_DEPTH-1) {
        return this->position->evaluate(); 
    }

    // Mate distance pruning 
    if (this->position->get_search_ply()) {
        alpha = max(-INFINITE + this->position->get_search_ply(), alpha); 
        beta  = min(INFINITE - this->position->get_search_ply(), beta);
        if (alpha >= beta) return beta; 
    }

    bool in_check = this->position->is_in_check();
	if (in_check != false) {
		depth++;
	}
    
    int score = -INFINITE; 
    uint16_t pvMove; 

    if (this->probe_hashentry(this->position->get_key(), &pvMove, &score, alpha, beta, depth) != false) {
    #ifndef NTEST
        this->info->incrementHashCut(); 
    #endif
        return score; 
    }

    ///@todo change zugswang evaluation 
    if (do_null && !in_check && this->position->get_search_ply() && !this->position->is_in_zugswang() && depth >= 4) {
        this->position->null_move(); 
        score = -this->ab(-beta, -beta+1, depth-4, false); 
        this->position->null_unmove(); 
        if (this->info->getStopped() != false) {
            return 0; 
        }

        if (score >= beta && abs(score) < ISMATE) {
            #ifndef NTEST
                this->info->incrementNullCut(); 
            #endif
            return beta; 
        }
    }

    movelist_t mvs[1]; 
    this->position->movegen(mvs); 

    if (pvMove != NOMOVE) {
        for (int idx = 0; idx < mvs->cnt; idx++) {
            if (mvs->moves[idx].get_move() == pvMove) {
                mvs->moves[idx].set_score(PV_SCORE);  
                break; 
            }
        }
    }

    int legal = 0; score = -INFINITE;
    int old_alpha = alpha; MOVE best_mv; 
    for (int idx = 0; idx < mvs->cnt; idx++) {
        this->pick_next_move(mvs, idx); 
        if (this->position->move(&mvs->moves[idx]) != false) {
            legal++; 
            score = -this->ab(-beta, -alpha, depth-1, true); 
            this->position->unmove();             
            
            if (this->info->getStopped() != false) {
			    return 0;
		    }

            if (score > alpha) {
                best_mv = mvs->moves[idx]; 
                if (score >= beta) {
                    #ifndef NTEST
                        if (legal == 1) {
                            this->info->incrementFHF(); 
                        }
                        this->info->incrementFH(); 
                    #endif
                    // non capture move that beat beta 
                    if (this->position->get_piece(best_mv.to_sq()) != EMPTY) {
                        this->position->save_killer(best_mv.get_move());
                    }

                    this->store_hashentry(this->position->get_key(), best_mv.get_move(), beta, HFBETA, depth); 
                    return beta; 
                }
                // non capture move that improve alpha
                if (this->position->get_piece(best_mv.to_sq()) != EMPTY) {
                    this->position->save_history(best_mv.from_sq(), best_mv.to_sq(), depth);
                }
                alpha = score; 
            }
        }
    }

    if (legal == 0) {
        if (in_check != false) {
            return -INFINITE + this->position->get_search_ply(); 
        } else {
            return 0;
        }
    }

    if (alpha != old_alpha) {
        this->store_hashentry(this->position->get_key(), best_mv.get_move(), alpha, HFEXACT, depth); 
    } else {
        this->store_hashentry(this->position->get_key(), best_mv.get_move(), alpha, HFALPHA, depth);    
    }
	
	return alpha;
}

MOVE ENGINE::ab_iterative() {
    assert(this->position->check_board()); 
    MOVE best_move = this->random_move();

    int best_score = -INFINITE; 
    for (int currentDepth = 1; currentDepth <= this->info->getDepth(); currentDepth++) {
        int mate = 0; 

        best_score = this->ab(-INFINITE, INFINITE, currentDepth, true); 

        if (this->info->getStopped() != false) {
            break;
        }

        best_move  = this->probe_pv_move(this->position->get_key());         

        cout << "info score";
        if (best_score <= ISMATE && best_score >= -ISMATE) {
            cout << " cp " << best_score;
        } else { 
            int side = this->position->get_side(); 
            if (best_score > 0) { // current side is winning 
                mate = -best_score + INFINITE; // get plies to mate 
                mate = (mate-side+2)/2; // transform mate in moves
            } else { // engine is getting mated              
                mate =  best_score  + INFINITE;
                mate = -(mate-side+1)/2; //  transform mate in moves : 1 move de retard quand white is getting mated
            }
            cout << " mate " << mate;
        }
        cout << " depth "    << currentDepth; 
        cout << " nodes "    << this->info->getNodes(); 
        cout << " tbhits "   << this->info->getTbHits(); 
        cout << " nps "      << 1000*this->info->getNodes() / (this->get_time_ms() - this->info->getStartTime() + 1);
        cout << " hashfull " << this->hashfull(); 
        cout << " time "     << this->get_time_ms() - this->info->getStartTime();
        cout << " pv ";

        MOVE pv_move = best_move; 

        /* print pv */
        int cnt = 0;  
        while ((pv_move.get_move() != NOMOVE) && (cnt < currentDepth)) {
            cout << pv_move << " ";
            this->position->move(&pv_move); 
            pv_move = this->probe_pv_move(this->position->get_key()); 
            cnt++; 
        }
        while (this->position->get_search_ply() > 0) {
            this->position->unmove(); 
        }
        cout << endl;

        if (mate != 0 && currentDepth >= (-abs(best_score) + INFINITE)) {
            // if we have a mating score and we are deep enough to print the pv line, 
            // then escape early from the search
            // break; 
        }


        #ifndef NTEST 
            cout << *this->info << endl; 
        #endif 
    }

    return best_move;
}

