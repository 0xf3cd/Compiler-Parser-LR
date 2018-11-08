#ifndef GRAMMAR
#define GRAMMAR
#include "Grammar.h"
#endif

#ifndef LR0_
#define LR0_
#include "LR0.h"
#endif

using namespace std;

int main() {
    LR0 L;    

    L.initialize("../TestFile/Grammar.txt");
    L.generateItems(); //拆分产生式为项目
    L.generateAllClosure(); //生成闭包及 GO(I, X)

    if(L.isLR0()) {
        cout << "IS LR(O)" <<endl;
    } else {
        cout << "IS NOT LR(0)" <<endl;
    }

    L.generateACTION();
    L.generateGOTO();

    L.showItems();
    L.showClosures();
    L.showGO();
    
    for(int i = 0; i <= 11; i++) {
         L.showACTION(i);
         L.showGOTO(i);
         cout << endl;
    }
    
    return 0;
}