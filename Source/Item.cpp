#ifndef SYMBOL
#define SYMBOL
#include "Symbol.h"
#endif

#ifndef ITEM
#define ITEM
#include "Item.h"
#endif

#include <fstream>
#include <iostream>
using namespace std;

Item::Item() {
    left = Symbol("");    
}

/* 使用 C++ STL 库时，需要定义 < 运算符
 */
bool Item::operator <(const Item &p) const {
    vector<Symbol>::const_iterator it1, it2;
    it1 = right.begin();
    it2 = p.right.begin();

    return (left.name == p.left.name)? it1 -> name > it2 -> name: left.name < p.left.name;
}

Item & Item::operator =(const Item &p) {
    left = p.left;
    right = p.right;

    return *this;
}

istream &operator >>(istream &fin, Item &p) {
    string temp;

    fin >> temp; //读入左部
    p.left = Symbol(temp);

    fin >> temp; //读入 ->

    //接下来处理右部
    p.right.clear();
    while(true) {
        fin >> temp;
        if(temp == "$") {
            break;
        }

        p.right.push_back(Symbol(temp));
    }

    return fin;
}

ostream &operator <<(ostream &cout, const Item &p) {
    vector<Symbol>::const_iterator it;

    cout << p.left.name << " -> ";
    for(it = p.right.begin(); it != p.right.end(); it++) {
        cout << it -> name << ' ';
    }

    return cout;
} 