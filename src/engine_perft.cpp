#ifndef NPERFT
#include "engine.hpp"

void ENGINE::perft(int depth) {
    assert(this->position->check_board()); 

    if (depth == 0) {
        this->info->incrementNodes();
        return; 
    }

    movelist_t mvs[1]; 
    this->position->movegen(mvs); 

    for (uint8_t idx = 0; idx < mvs->cnt; idx++) {
        MOVE mv = mvs->moves[idx]; 
        #ifndef NDEBUG 
            board_key_t prec_key = this->position->get_key(); 
        #endif        
        if (!this->position->move(& mv))
            continue; 
        perft(depth-1); 
        
        this->position->unmove(); 
        #ifndef NDEBUG 
            if (this->position->get_key() != prec_key) {
                std::cout << std::endl << "bug here : " << *(this->position) << " with move : " << mv << std::endl << std::endl; 
                std::cout << "history : " << std::endl; 
                while (this->position->get_search_ply() > 0) {
                    this->position->unmove(); 
                    std::cout << *(this->position) << std::endl; 
                }
                assert(false);
            }
        #endif
    }
    return; 
}

bool ENGINE::performance_test_all(int depth) {
    assert(this->position->check_board()); 

    this->info->setStartTime(this->get_time_ms()); 

    if (depth > 6) 
        depth = 6; 

    std::cout << "STARTING TEST " << NB_PERFT << " POSITIONS up depth " << depth << " : " << std::endl; 
    
    for (int i = 0; i < NB_PERFT; i++) {
        this->position->parse_fen(perft_fen[i]); 
        printf("Position # %03d/%d : ",i+1,NB_PERFT); 
        uint64_t start = this->get_time_ms(); 
        for (int d = 0; d < depth; d++) {
            this->info->clear_before_search(); 
            bool result = true;
            this->perft(d+1);
            result = result && (this->info->getNodes() == perft_depth_results[d][i]);  
            result ? std::cout << "OK " : std::cout << "FAIL ";
            if (result != true)
                return false; 
        }
        std::cout << " " << this->info->getNodes() << " nodes seen " << "in " << int(this->get_time_ms() - start)  << " ms" << std::endl ; 
    }

    std::cout << std::endl << "END PERF TEST UP TO DEPTH " << depth << " IN " << int(this->get_time_ms() - this->info->getStartTime()) << " ms" << std::endl; 
    return true; 
}
#endif

#ifndef NMIRROR
#include "engine.hpp"

using namespace std; 

bool ENGINE::mirror_test() {
    cout << "STARTING MIRROR TEST 126 POSITIONS " << endl; 
    int ev1 = 0; int ev2 = 0;
    for (int i = 0; i < NB_PERFT; i++) {
        this->position->parse_fen(perft_fen[i]); 
        ev1 = this->position->evaluate(); 
        this->position->mirror();
        ev2 = this->position->evaluate(); 
        printf("Position # %03d/%03d : ", i+1, NB_PERFT);
        if (ev1 != ev2) {
            printf("ERROR : ev1 = %d, ev2 = %d\n\n",ev1,ev2); 
            this->position->parse_fen(perft_fen[i]); 
            this->position->mirror();
            this->position->show(); 
            ERROR_MSG(ERROR_EVAL_NON_SYMETRIC); 
            return false; 
        }
        printf("ev1 = %d, ev2 = %d\n",ev1,ev2);
    }
    cout << endl << "END MIRROR TEST WITHOUT ERROR " << endl; 
    return true;
}
#endif