#ifndef PASER
#define PASER
#include "Parser.h"
#endif

#include <iostream>
#include <fstream>
using namespace std;

int main() {
    ofstream fout;
    fout.open("./TestFile/result.txt", ios::out);

    Parser P;
    if(P.initialize("./TestFile/example.cmm", "./TestFile/Grammar.txt") == false) {
        return -1;
    }

    snapshot ss;
    list<Symbol>::iterator it;
    bool loop = true;
    while(loop) {
        ss = P.getNext();
        fout << "TYPE: " << ss.error << endl;
        fout << "TOKEN NAME: " << ss.token.value << endl;
        fout << "TERMINAL: " << ss.symbol.name << endl;
        fout << "PRODUCTION USED: " << ss.production << endl;
        fout << "STACK: ";
        for(it = ss.stack.begin(); it != ss.stack.end(); it++) {
            fout << it -> name << ' ';
        }
        fout << endl << endl; 
        if(ss.error == 3 || ss.error < 0) {
            loop = false;
        }
    }

    fout.close();
    return 0;
}