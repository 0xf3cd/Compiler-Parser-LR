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
    L.generateItems();
    // L.showItems();
    L.generateAllClosure();

    L.showClosures();
    //L.showGO();

    cout << L.isLR0() << endl;

    return 0;
}