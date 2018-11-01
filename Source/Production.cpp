#ifndef SYMBOL
#define SYMBOL
#include "Symbol.h"
#endif

#ifndef PRODUCTION
#define PRODUCTION
#include "Production.h"
#endif

#include <fstream>
#include <iostream>
using namespace std;

Production::Production() {
    left = Symbol("");    
}

/* 使用 C++ STL 库时，需要定义 < 运算符
 */
bool Production::operator <(const Production &p) const {
    vector<Symbol>::const_iterator it1, it2;
    it1 = right.begin();
    it2 = p.right.begin();

    return (left.name == p.left.name)? it1 -> name > it2 -> name: left.name < p.left.name;
}

Production & Production::operator =(const Production &p) {
    left = p.left;
    right = p.right;

    return *this;
}

ifstream &operator >>(ifstream &fin, Production &p) {
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

ostream &operator <<(ostream &cout, const Production &p) {
    vector<Symbol>::const_iterator it;

    cout << p.left.name << " -> ";
    for(it = p.right.begin(); it != p.right.end(); it++) {
        cout << it -> name << ' ';
    }

    return cout;
}