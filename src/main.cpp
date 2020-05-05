#include "engine.hpp"

///@todo script 
///@todo cutechess-cli -engine cmd=./collect_iathena/iathena_rand -engine cmd=./collect_iathena/iathena_ab -each proto=uci tc=40/10+0.1 -rounds 10
/*
# Run cutechess-cli binary
cmd = [ cutechess, '-repeat', '-rounds', str(int(games_to_play)), '-tournament', 'gauntlet'] + pgnout + \
    ['-site', 'http://tests.stockfishchess.org/tests/view/' + run['_id']] + \
    ['-event', 'Batch %d: %s vs %s' % (task_id, make_player('new_tag'), make_player('base_tag'))] + \
    ['-srand', "%d" % struct.unpack("<L", os.urandom(struct.calcsize("<L")))] + \
    ['-resign', 'movecount=3', 'score=400', '-draw', 'movenumber=34',
    'movecount=8', 'score=20', '-concurrency', str(int(games_concurrency))] + pgn_cmd + \
    ['-engine', 'name=New-'+run['args']['resolved_new'][:7], 'cmd=%s' % (new_engine_name)] + new_options + ['_spsa_'] + \
    ['-engine', 'name=Base-'+run['args']['resolved_base'][:7], 'cmd=%s' % (base_engine_name)] + base_options + ['_spsa_'] + \
    ['-each', 'proto=uci', 'tc=%s' % (scaled_tc)] + nodestime_cmd + threads_cmd + book_cmd

['/home/fishtest/fishtest-master/worker/testing/cutechess-cli', '-repeat', '-rounds', '250', '-tournament', 'gauntlet', 
'-pgnout', 'results-34185b00-4e7f-4e36-8298-52474eea9449.pgn', '-site', u'http://tests.stockfishchess.org/tests/view/5ca5b2c90ebc5925cf004074', 
'-event', u'Batch 72: shuffle^ vs master', '-srand', '3905888192', '-resign', 'movecount=3', 'score=400',
 '-draw', 'movenumber=34', 'movecount=8', 'score=20', '-concurrency', '4', '-openings', u'file=2moves_v1.pgn', u'format=pgn', 
 'order=random', 'plies=16', '-engine', u'name=New-e1a507c', u'cmd=stockfish_e1a507cc50fe246408c0f4043b233be60ce7b36c', u'option.Hash=8', 
 '-engine', u'name=Base-aa0166f', u'cmd=stockfish_aa0166fba66d1bd3d2756f8f16b7f6161064d0a3', u'option.Hash=8', '-each', 
 'proto=uci', 'tc=8.71+0.09', 'option.Threads=1']

*/
/* idées : 
    - improve evaluation masks 
    - change mov to be only uint16_t 
    and create a new structure with score 
    - use the big structure only if needed
    - stockfish position size : 2168
    - make zkey class 
*/

board_key_t keys[781];

void init() {
    /* init keys */
    for (int i = 0; i < 781; i++) {
        keys[i] = RAND_64;
    }

    /* init bitboards */
    bb::init(); 
}
					
int main(int argc, char* argv[]) {
    srand(time(NULL));

    init(); 

    ENGINE * engine = new ENGINE(); 
    engine->set_iaType(ALPHABETA); 

    #ifndef NPERFT
        engine->performance_test_all(PERFT_DEPTH);
    #elif !(defined NMIRROR)
        engine->mirror_test(); 
    #else 
        engine->uci_loop(); 
    #endif
    
    delete engine; 
    return EXIT_SUCCESS; 
}