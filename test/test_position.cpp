#include <iostream>
#include <fstream>
#include <string>

#include "../includes/position.hpp"

using namespace std;

int main(){

    ifstream myfile("perftsuite.epd");
    string mystring;
    string delimiter = " ;";
    string token;

    Position P1(START_FEN); 

    if ( myfile.is_open() ) {     
        while ( myfile ) {
            getline (myfile, mystring);
            token = mystring.substr(0, mystring.find(delimiter)); // token is "scott"

            P1.set_position(token.c_str()); 

            assert(token == P1.print_fen()); 
        }         
    }

    return 0;
}