//项目
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#ifndef SYMBOL
#define SYMBOL
#include "Symbol.h"
#endif

using namespace std;

/* Item 是描述一个 LR(0) 分析法中的项目
 */
class Item {
public:
    Symbol left; //左部
    vector<Symbol> right; //右部

    Item();

    /* 使用 C++ STL 库时，需要定义 < 运算符
     */
    bool operator <(const Item &p) const;

    Item & operator =(const Item &p);

    bool operator ==(const Item &p) const;

    // friend istream &operator >>(istream &fin, Item &p);

    friend ostream &operator <<(ostream &cout, const Item &p);
};