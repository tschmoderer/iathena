#include "engine.hpp"

using namespace std; 

void ENGINE::clear_hashtable() {
    S_HASHENTRY * idx; 
    for (idx = this->hashtable->pEntry; idx < this->hashtable->pEntry + this->hashtable->nb_entries; idx++) {
        idx->key = NULL_64; 
        idx->mv  = NOMOVE;
        idx->score = 0; 
        idx->depth = 0; 
        idx->flag  = HFNONE; 
    }
}

void ENGINE::init_hashtable(const int MB) {
    int hash_size = 0x100000 * MB;
    this->hashtable->nb_entries = hash_size / sizeof(S_HASHENTRY); 
    this->hashtable->nb_entries -= 2; 

    if (this->hashtable->pEntry != NULL) 
        free(this->hashtable->pEntry); 
    
    this->hashtable->pEntry = (S_HASHENTRY *)malloc(this->hashtable->nb_entries * sizeof(S_HASHENTRY)); 
	if (this->hashtable->pEntry == NULL) {
		this->init_hashtable(MB/2);
	} else {
		this->clear_hashtable();
	}

    #ifndef NDEBUG
        cout << "info string hastable init with " << this->hashtable->nb_entries << " entries." << endl; 
    #endif
}

void ENGINE::store_hashentry(const board_key_t storing_key, const uint16_t mv, int score, const int fl, int depth) {
    int idx = storing_key % this->hashtable->nb_entries; 

    assert(idx >= 0 && idx < this->hashtable->nb_entries); 
    assert(depth >= 1 && depth <= MAX_DEPTH); 
    assert(fl >= HFALPHA && fl <= HFEXACT); 
    assert(score >= -INFINITE && score <= INFINITE); 

    ///@todo method to avoid collisions for uci best move 
    ///@brief replacement scheme : always replace 

#ifndef NDEBUG
    if (this->hashtable->pEntry[idx].key == NULL_64) {
        this->info->incrementNewEntry(); 
    } else {
        this->info->incrementOverWrite(); 
    }
#endif

    if (score > ISMATE) 
        score += this->position->get_search_ply();
    else if (score < - ISMATE)
        score -= this->position->get_search_ply();

    this->hashtable->pEntry[idx].mv    = mv; 
    this->hashtable->pEntry[idx].score = score;
    this->hashtable->pEntry[idx].key   = storing_key;
    this->hashtable->pEntry[idx].depth = depth; 
    this->hashtable->pEntry[idx].flag  = fl;
}

int ENGINE::probe_hashentry(const board_key_t probing_key, uint16_t * mv, int * score, const int alpha, const int beta, const int depth) {
    int idx = probing_key % this->hashtable->nb_entries; 

    assert(idx >= 0 && idx < this->hashtable->nb_entries); 
    assert(depth >= 1 && depth <= MAX_DEPTH);
    assert(alpha < beta);
    assert(alpha >= -INFINITE && alpha <= INFINITE);
    assert(beta  >= -INFINITE && beta  <= INFINITE);
    assert(this->position->get_search_ply() >=0 && this->position->get_search_ply() < MAX_DEPTH);

    // search a move and depth greater than ours 
    if (this->hashtable->pEntry[idx].key == probing_key) {
        *mv = this->hashtable->pEntry[idx].mv;
        if (this->hashtable->pEntry[idx].depth >= depth) {
            
            assert(this->hashtable->pEntry[idx].depth >= 1 && this->hashtable->pEntry[idx].depth < MAX_DEPTH);
            assert(this->hashtable->pEntry[idx].flag >= HFALPHA && this->hashtable->pEntry[idx].flag <= HFEXACT);
			
			*score = this->hashtable->pEntry[idx].score;

            if (*score > ISMATE)       *score -= this->position->get_search_ply();
            else if (*score < -ISMATE) *score += this->position->get_search_ply(); 

            assert(*score >= -INFINITE && *score <= INFINITE);

            switch (this->hashtable->pEntry[idx].flag) {
                case HFALPHA: 
                    if (*score <= alpha) {
                        *score = alpha;
                        return true; 
                    } break; 
                case HFBETA: 
                    if (*score >= beta) {
                        *score = beta;
                        return true; 
                    } break; 
                case HFEXACT: 
                    return true; 
                    break; 
                default: assert(false); break; 
            }
        }
    }
    return false;
}

MOVE ENGINE::probe_pv_move(const board_key_t probing_key) {
    int idx = probing_key % this->hashtable->nb_entries; 

    assert(idx >= 0 && idx < this->hashtable->nb_entries); 
	
	if (this->hashtable->pEntry[idx].key == probing_key) {
        uint16_t mv = this->hashtable->pEntry[idx].mv;
        return MOVE(mv, 0); 
	}
    
    if (this->position->get_search_ply() == 0) {
        cerr << "failed finding pv move at depth 0" << endl; 
        cerr << "this key: "     << this->position->get_key() << endl; 
        cerr << "stored key: "   << this->hashtable->pEntry[idx].key << endl; 
        cerr << "stored move: "  <<  this->hashtable->pEntry[idx].mv << endl; 
        cerr << "stored score: " <<  this->hashtable->pEntry[idx].score << endl; 
        cerr << "stored depth: " <<  this->hashtable->pEntry[idx].depth << endl; 
        cerr << "stored flag: "  <<  this->hashtable->pEntry[idx].flag << endl; 
        cerr << "hashtable size: " << this->hashtable->nb_entries << endl; 
        #ifndef NDEBUG
        cerr << *this->position << endl; 
        cerr << *this->info << endl; 
        #endif
        cerr << endl; 
    }

	return MOVE();
}
/*
void ENGINE::store_pv_move(board_key_t storing_key, MOVE mv) {
    int idx = storing_key % this->hashtable->nb_entries; 

    assert(idx >= 0 && idx < this->hashtable->nb_entries); 
    
    this->hashtable->pEntry[idx].mv = mv; 
    this->hashtable->pEntry[idx].key = storing_key; 
}
*/
/**
 * @return the hashtable occupation during the search, the occupation is permill as in UCI protocol 
 * @todo optimize to get an approxiamtion not exact value  
**/
int ENGINE::hashfull() const {
    int cnt = 0; 
    S_HASHENTRY * idx; 
    for (idx = this->hashtable->pEntry; idx < this->hashtable->pEntry + (this->hashtable->nb_entries / 1000); idx++) {
        if (idx->key != NULL_64) 
            cnt++;  
    } 
    return cnt;
}