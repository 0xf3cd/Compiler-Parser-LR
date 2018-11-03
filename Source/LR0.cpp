#ifndef LR0_
#define LR0_
#include "LR0.h"
#endif

#ifndef SYMBOL
#define SYMBOL
#include "Symbol.h"
#endif

#include <set>
#include <vector>
#include <iostream>

using namespace std;

/* generateItems 函数: 将产生式转换为项目
 */
void LR0::generateItems(Grammar &G) {
    set<Production> &productions = G.getProductions();
    set<Production>::iterator it_p;
    vector<Symbol>::const_iterator it_s;
    set<Item>::iterator it_i;
    Symbol DOT = G.getDot();
    Symbol EPSILON = G.getEpsilon();

    for(it_p = productions.begin(); it_p != productions.end(); it_p++) {
        //遍历到所有的产生式
        Item new_item;
        int i, j, size;

        new_item.left = it_p -> left;
        if(*(*it_p).right.begin() == EPSILON) { //如果产生式右部为空字符
            new_item.right.push_back(DOT);
            items.insert(new_item);
            continue;
        }

        size = (*it_p).right.size();
        for(i = 0; i <= size; i++) {
            it_s = (*it_p).right.begin();
            for(j = 0; j <= size; j++) {
                if(j == i) {
                    new_item.right.push_back(DOT);
                } else {
                    new_item.right.push_back(*it_s);
                    it_s++;
                }
            }
            items.insert(new_item);
            new_item.right.clear();
        }
    }
}

/* showItems 函数: 展示所有项目
 */
void LR0::showItems() {
    set<Item>::iterator it;
    for(it = items.begin(); it != items.end(); it++) {
        cout << *it << endl;
    }
}