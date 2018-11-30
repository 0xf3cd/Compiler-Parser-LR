#include "LR_Wrap.h"

#ifndef LR_
#define LR_
#include "LR.h"
#endif

#include <iostream>

using namespace std;

int main() {
    string gra = "../TestFile/Grammar.txt";
    string src = "../TestFile/example.cmm";
    setGrammar((char *)(gra.c_str()));
    setSource((char *)(src.c_str()));
    cout << initialize() << endl;
    cout << isInitialized() << endl;
    cout << isLR0() << endl;
    cout << isSLR1() << endl;

    cout << endl << "analyze begin" << endl;
    //analyze();
    while(true) {
        js_snapshot jss = getNext();

        cout << jss.au << endl;
        cout << jss.token.value << endl;
        cout << jss.symbol_name << endl;
        cout << jss.production_left << " -> " << jss.production_right << endl;
        cout << jss.error << endl;
        cout << jss.symbol_stack << endl;
        cout << jss.state_stack << endl;
        cout << endl;

        if(jss.error < 0 || jss.error == 3) {
            break;
        }
    }
    return 0;
}