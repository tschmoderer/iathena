#include "engine.hpp"

#ifdef WIN32
	#include "windows.h"
#endif

using namespace std; 

int input_waiting() {
#ifndef WIN32
  fd_set readfds;
  struct timeval tv;
  FD_ZERO (&readfds);
  FD_SET (fileno(stdin), &readfds);
  tv.tv_sec=0; tv.tv_usec=0;
  select(16, &readfds, 0, 0, &tv);

  return (FD_ISSET(fileno(stdin), &readfds));
#else
   static int init = 0, pipe;
   static HANDLE inh;
   DWORD dw;

   if (!init) {
     init = 1;
     inh = GetStdHandle(STD_INPUT_HANDLE);
     pipe = !GetConsoleMode(inh, &dw);
     if (!pipe) {
        SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
        FlushConsoleInputBuffer(inh);
      }
    }
    if (pipe) {
      if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
      return dw;
    } else {
      GetNumberOfConsoleInputEvents(inh, &dw);
      return dw <= 1 ? 0 : dw;
	}
#endif
}


void ENGINE::read_input() {
    int  bytes;
    char input[256] = "", *endc;
/*
    fd_set readfds;
    struct timeval tv;
    FD_ZERO (&readfds);
    FD_SET (fileno(stdin), &readfds);
    tv.tv_sec=0; tv.tv_usec=0;
    select(16, &readfds, 0, 0, &tv);
*/

    if (input_waiting()) {
		this->info->setStopped(true); 
		do {
		  bytes = read(fileno(stdin),input,256);
		} while (bytes<0);
		endc = strchr(input,'\n');
		if (endc) *endc=0;

		if (strlen(input) > 0) {
			if (!strncmp(input, "quit", 4))    {
			  this->info->setQuit(true); 
			}
		}
		return ;
    }
}

void ENGINE::parse_go(char * line) {
    int depth = -1, movestogo = 30, movetime = -1;
	int time = -1, inc = 0;
    char *ptr = NULL;

    this->info->setTimeSet(false); 

	if ((ptr = strstr(line,"infinite"))) {
		;
	}

	if ((ptr = strstr(line,"binc")) && this->position->get_side() == BLACK) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line,"winc")) && this->position->get_side() == WHITE) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line,"wtime")) && this->position->get_side() == WHITE) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line,"btime")) && this->position->get_side() == BLACK) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line,"movestogo"))) {
		movestogo = atoi(ptr + 10);
	}

	if ((ptr = strstr(line,"movetime"))) {
		movetime = atoi(ptr + 9);
	}

	if ((ptr = strstr(line,"depth"))) {
		depth = atoi(ptr + 6);
	}

	if (movetime != -1) {
		time = movetime;
		movestogo = 1;
	}

    this->info->setStartTime(this->get_time_ms());
    this->info->setDepth(depth); 

	if (time != -1) {
        this->info->setTimeSet(true); 
		time /= (movestogo + 5);
		time -= 50;
        this->info->setStopTime(this->info->getStartTime() + time + inc); 
	}

	if (depth == -1) {
		this->info->setDepth(MAX_DEPTH);
	}

#ifndef NDEBUG 
	printf("time:%d start:%" PRIu64" stop:%" PRIu64 " depth:%d timeset:%d\n",
		time,this->info->getStartTime(),this->info->getStopTime(),this->info->getDepth(),this->info->getTimeSet());
#endif
	this->search(); 
}

void ENGINE::parse_position(const char * line) {
    line += 9;
    this->position->parse_position(line); 
}

void ENGINE::uci_loop() {
    setbuf(stdin, NULL); 
    setbuf(stdout,NULL);
    
    while(true) {
        char line[INPUT_BUFFER]; 
        memset(&line[0], 0, sizeof(line));
        if (!fgets(line, INPUT_BUFFER, stdin))
            continue;
        if (line[0] == '\n')
            continue;
        fflush(stdout);

        if (!strncmp(line, "isready", 7)) {
            cout << "readyok" << endl;
            continue;
        } else if (!strncmp(line, "position", 8)) {
            this->parse_position(line); 
        } else if (!strncmp(line, "ucinewgame", 10)) {
            this->parse_position("position startpos\n");
        } else if (!strncmp(line, "go", 2)) {
            this->parse_go(line); 
        } else if (!strncmp(line, "quit", 4)) {
            this->info->setQuit(true);
            break;
        } else if (!strncmp(line, "uci", 3)) {
            cout << "id name " << NAME << endl; 
            cout << "id author " << AUTHOR << endl; 
            cout << "option name hash type spin default " << DEFAULT_HASH << " min " << MIN_HASH << " max " << MAX_HASH << endl; 
            cout << "option name clear hash type button" << endl;
            cout << "uciok" << endl; 
        } else if (!strncmp(line,"setoption name clear hash",25)) {
            this->clear_hashtable(); 
        } else if (!strncmp(line,"setoption name hash value ", 26)) {	
            int MB = DEFAULT_HASH;		
			sscanf(line,"%*s %*s %*s %*s %d",&MB);
			if (MB < 4) MB = MIN_HASH;
			if(MB > MAX_HASH) MB = MAX_HASH;
			this->init_hashtable(MB);
        } else if (!strncmp(line, "show", 4)) {
            // this->position->show(); 
        } else if (!strncmp(line, "test", 4)) {
            
        }   

		if(this->info->getQuit()) break;
    } 
}
