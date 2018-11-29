#ifndef GRAMMAR
#define GRAMMAR
#include "Grammar.h"
#endif

#ifndef LR_
#define LR_
#include "LR.h"
#endif

using namespace std;

int main() {
    LR L;    

    L.initialize("../TestFile/Grammar.txt");
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

    L.generateSLR1ACTION();
    L.generateGOTO();

    // L.showItems();
    // L.showClosures();
    // L.showGO();
    // L.showEncodedProduction();
    
    // for(int i = 0; i <= 185; i++) {
    //      L.showACTION(i);
    //      L.showGOTO(i);
    //      cout << endl;
    // }
    // cout << endl;

    snapshot ss;
    list<Symbol>::iterator it_s;
    list<int>::iterator it_i;
    while(true) {
        ss = L.getNext();
        cout << ss.au.first << ss.au.second << endl;
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
    }
    
    return 0;
}