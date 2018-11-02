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
    Grammar G;
    LR0 L;

    G.analyzeGrammar("../TestFile/Grammar.txt");
    G.setStartArgument("E");

    L.generateItems(G);
    L.showItems();

    return 0;
}