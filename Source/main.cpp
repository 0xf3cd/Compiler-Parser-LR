#ifndef GRAMMAR
#define GRAMMAR
#include "Grammar.h"
#endif

#ifndef LR_
#define LR_
#include "LR.h"
#endif

using namespace std;

int main(int argc, char *argv[]) {
    LR L;

    if(argc <= 2) {
        L.initialize("../TestFile/Grammar.txt");
    } else {
        L.initialize(argv[1], argv[2]);
    }

    L.generateItems(); //拆分产生式为项目
    L.generateAllClosure(); //生成闭包及 GO(I, X)

    if(L.isLR0()) {
        cout << "IS LR(O)" <<endl;
    } else {
        cout << "IS NOT LR(0)" <<endl;
    }
    if(L.isSLR1()) {
        cout << "IS SLR(1)" <<endl;
    } else {
        cout << "IS NOT SLR(1)" <<endl;
    }
    cout << endl;

    L.generateSLR1ACTION();
    L.generateGOTO();

    snapshot ss;
    list<Symbol>::iterator it_s;
    list<int>::iterator it_i;
    while(true) {
        ss = L.getNext();
        if(ss.au.first == "acc") {
            cout << ss.au.first << endl;
        } else {
            cout << ss.au.first << ss.au.second << endl;
        }
        cout << ss.token.value << endl;
        cout << ss.symbol.name << endl;
        cout << ss.production << endl;
        cout << ss.error << endl;

        for(it_s = ss.symbol_stack.begin(); it_s != ss.symbol_stack.end(); it_s++) {
            cout << it_s -> name << ' ';
        }
        cout << endl;

        for(it_i = ss.state_stack.begin(); it_i != ss.state_stack.end(); it_i++) {
            cout << *it_i << ' ';
        }
        cout << endl;

        cout << endl;

        if(ss.error == 3) {
            cout << "success" << endl;
            break;
        }
        if(ss.error < 0) {
            cout << "wrong" << endl;
            break;
        }
    }
    
    return 0;
}