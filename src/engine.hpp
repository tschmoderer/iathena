#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "types.hpp"
#include "board.hpp"
#include "searchinfo.hpp"
#include <sys/time.h>

#define MAX_HASH 256
#define MIN_HASH 1
#define DEFAULT_HASH 64

#define PV_SCORE 2097152

#define TB_CARDINALITY 5

enum {HFNONE, HFALPHA, HFBETA, HFEXACT}; 

typedef struct {
    board_key_t key; 
    uint16_t mv; 
    int score; 
    int depth; 
    int flag; 
} S_HASHENTRY;

typedef struct {
    S_HASHENTRY * pEntry; 
    int nb_entries; 
} S_HASHTABLE; 

class ENGINE {
    public: 
        /* constructors */ 
        ENGINE(); 

        /* destructors */ 
        ~ENGINE(); 
        
        /* getters */ 

        /* setters */ 
        void set_iaType(const ia_t ); 
        void setPosition(const char * ); 
        
        /* operators */ 

        /* IO stream */ 

        /* methods */ 
        void uci_loop(); 

        // test 
    #ifndef NPERFT
        bool performance_test_all(int ); 
        void perft(int ); 
    #endif 
    
    #ifndef NMIRROR
        bool mirror_test();
    #endif
    
    private: 
        BOARD      * position; 
        SEARCHINFO * info; 
        ia_t ia_type;

        /* principal variation table */ 
        S_HASHTABLE hashtable[1]; 

        /* methods */ 

        /* time */
        uint64_t get_time_ms(); 

        /* search */
        void check_up(); 
        void clear_before_search(); 
        void search(); 

        /* ia */
        void pick_next_move(movelist_t * , const int ) const; 

        MOVE random_move(); 
        MOVE ab_iterative(); 
        
        int ab(int , int , int , bool ); 
        int quiescence(int , int ); 
        

        /* uci */
        void parse_go(char * );
        void parse_position(const char * );
        void read_input(); 

        /* hashtable */ 
        void clear_hashtable(); 
        void init_hashtable(const int ); 
        int  probe_hashentry(const board_key_t , uint16_t * , int * , const int , const int , const int ); 
        void store_hashentry(const board_key_t , const uint16_t , int , const int , int ); 
        MOVE probe_pv_move(const board_key_t );
        int  hashfull() const;

        /* table base */ 
};
#endif
