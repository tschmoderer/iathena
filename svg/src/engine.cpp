#include "engine.hpp" 

/* constructors */ 
ENGINE::ENGINE() {
    std::cout << NAME << " " << VERSION << " by " << AUTHOR << std::endl; 
    this->position = new BOARD("7k/8/8/8/8/8/8/K7 w - - 0 1"); 
    this->info     = new SEARCHINFO(); 
    this->ia_type  = RAND;

    /* hashtable */ 
    this->hashtable->pEntry = NULL; 
    this->init_hashtable(DEFAULT_HASH); 
}

/* destructors */ 
ENGINE::~ENGINE() {
    free(this->hashtable->pEntry); 
    if (this->position != NULL) 
        delete this->position; 
    if (this->info != NULL) 
        delete this->info;
}

/* getters */ 

/* setters */ 
void ENGINE::set_iaType(const ia_t iaT)    {this->ia_type = iaT;}
void ENGINE::setPosition(const char * fen) {this->position->parse_fen(fen);}

/* operators */ 

/* IO stream */ 

/* methods */ 
uint64_t ENGINE::get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return 1000 * (uint64_t)tv.tv_sec + (uint64_t)tv.tv_usec / 1000;
}