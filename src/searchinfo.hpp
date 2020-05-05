#ifndef SEARCHINFO_HPP
#define SEARCHINFO_HPP

#include "types.hpp"

/**
 * @class SEARCHINFO
 * @brief Class to handle UCI option and info about the search in debug mode 
 * @todo make all inline
*/
class SEARCHINFO {
    public: 
        /* constructors */ 
        inline SEARCHINFO() {
                this->start_time = NULL_64; 
                this->stop_time  = NULL_64;
                this->depth      = 1; 
                this->timeSet    = false; 
                this->movestogo  = -1; 

        #ifndef NTEST 
                this->fh  = 0; 
                this->fhf = 0;
                this->nullCut = 0; 
                this->hashCut = 0; 
                this->newEntry = 0; 
                this->overWrite = 0; 
        #endif

                this->nodes   = 0; 
                this->tbHits  = 0; 
                this->quit    = false; 
                this->stopped = false;     
        } ///< default constructor

        /* destructors */ 
        // ~SEARCHINFO(); ///< default desconstructor

        /* getters */ 
        constexpr uint64_t getStartTime() const {return this->start_time;}; 
        constexpr uint64_t getStopTime()  const {return this->stop_time;} 

        constexpr bool getTimeSet()  const {return this->timeSet;} 
        constexpr uint8_t getDepth() const {return this->depth;}
        constexpr uint8_t getMovesTogo() const {return this->movestogo;}

        constexpr long getNodes()  const {return this->nodes;}
        constexpr long getTbHits() const {return this->tbHits;}

        constexpr bool getQuit()    const {return this->quit;}
        constexpr bool getStopped() const {return this->stopped;}

        /* setters */ 
        inline void setStartTime(const uint64_t st) {this->start_time = st;} 
        inline void setStopTime(const uint64_t st)  {this->stop_time  = st;}

        inline void setTimeSet(const bool ts) {this->timeSet = ts;} 
        inline void setDepth(const uint8_t d) {this->depth = d;} 
        inline void setMovesToGo(const uint8_t mtg) {this->movestogo = mtg;}

#ifndef NTEST 
        inline void incrementFH()  {this->fh++;}
        inline void incrementFHF() {this->fhf++;}
        inline void incrementNullCut() {this->nullCut++;}
        inline void incrementHashCut() {this->hashCut++;}
        inline void incrementNewEntry()  {this->newEntry++;} 
        inline void incrementOverWrite() {this->overWrite++;}
#endif 

        inline void incrementNodes()  {this->nodes++;} 
        inline void incrementTBHits() {this->tbHits++;}

        inline void setQuit(const bool q) {this->quit = q;}
        inline void setStopped(const bool s) {this->stopped = s;}

        /* IO stream */
#ifndef NTEST 
        inline friend std::ostream & operator << (std::ostream & os, SEARCHINFO & info) {
                if (info.fh == 0) 
                info.fh++; 
                os << "info string ordering : " << 100*info.fhf/(1.*info.fh); 
                os << "  Hashtable new entry  : " << info.newEntry; 
                os << "  Hashtable over write : " << info.overWrite; 
                os << "  Hashtable cuts : " << info.hashCut; 
                os << "  Null move cuts : " << info.nullCut; 
                return os; 
        }
#endif

        /* methods */ 
        inline void clear_before_search() {
                this->nodes  = 0; 
                this->tbHits = 0; 

        #ifndef NTEST 
                this->fh = 0; 
                this->fhf = 0; 
                this->nullCut = 0; 
                this->hashCut = 0; 
        #endif

                this->stopped = false; 
        } ///< reset some value before search 

        inline void reset() {
                this->start_time = NULL_64; 
                this->stop_time  = NULL_64;
                this->depth      = 1; 
                this->timeSet    = false; 
                this->movestogo  = -1; 
                
        #ifndef NTEST 
                this->fh  = 0; 
                this->fhf = 0;
                this->nullCut = 0; 
                this->hashCut = 0; 
                this->newEntry = 0; 
                this->overWrite = 0; 
        #endif

                this->nodes   = 0; 
                this->tbHits  = 0; 
                this->quit    = false; 
                this->stopped = false; 
        } ///< reste value to default 

    private: 
    	uint64_t start_time; ///< start_time of th search 
        uint64_t stop_time;  ///< stop_time of the search if timeset

        bool timeSet;  ///< maximal time set 
        uint8_t depth;     ///< max depth search 
        uint8_t movestogo; ///< moves tog o yo next time control

#ifndef NTEST 
        uint32_t fh;        ///< number of alpha improvement
        uint32_t fhf;       ///< number of alpha improvement at first move search 
        uint32_t nullCut;   ///< number of cut due to null move 
        uint32_t hashCut;   ///< number of cuts due ti hashtable 
        uint32_t newEntry;  ///< number of new entry in hashtable 
        uint32_t overWrite; ///< number of overwrite in hashtable
#endif 

        long nodes;  ///< number of nodes searched
        long tbHits; ///< number of endgame tablebasis hits

        bool quit;    ///< terminate engine process
        bool stopped; ///< stop the search 
}; 
#endif