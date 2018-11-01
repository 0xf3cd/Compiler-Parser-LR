//产生式
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#ifndef SYMBOL
#define SYMBOL
#include "Symbol.h"
#endif

using namespace std;

/* Production 是描述一个文法中某一个产生式的结构体
 */
class Production {
public:
    Symbol left; //产生式左部
    vector<Symbol> right; //产生式右部

    Production();

    /* 使用 C++ STL 库时，需要定义 < 运算符
     */
    bool operator <(const Production &p) const;

    Production & operator =(const Production &p);

    friend ifstream &operator >>(ifstream &fin, Production &p);

    friend ostream &operator <<(ostream &cout, const Production &p);
};