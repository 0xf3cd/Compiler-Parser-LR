#ifndef SYMBOL
#define SYMBOL
#include "Symbol.h"
#endif

#include <string>
using namespace std;

Symbol::Symbol() {
    name = "";
    no = -1;
}

Symbol::Symbol(string s) {
    name = s;
    no = -1;
}

void Symbol::setNo(int new_no) const {
    no = new_no;
}

/* 使用 C++ STL 库时，需要定义 < 运算符
 */
bool Symbol::operator <(const Symbol &s) const {
    return no < s.no;
}

bool Symbol::operator >(const Symbol &s) const {
    return no > s.no;
}

Symbol& Symbol::operator =(const Symbol &s) {
    name = s.name;
    no = s.no;
    
    return *this;
}

bool Symbol::operator ==(const Symbol &s) const {
    if(name != s.name) {
        return false;
    }
    if(no != s.no) {
        return false;
    }

    return true;
}

bool Symbol::operator !=(const Symbol &s) const {
    if(name == s.name) {
        if(no == s.no) {
            return false;
        }
    }
    
    return true;
}

ifstream &operator >>(ifstream &fin, Symbol &s) {
    fin >> s.name;
    fin >> s.no;
    return fin;
}

ostream &operator <<(ostream &cout, const Symbol &s) {
    cout << s.no << ' ' << s.name;
    return cout;
}

Symbol::operator string() {
    return name;
}

bool Symbol::operator ==(const string &s) const {
    if(s == name) {
        return true;
    } else {
        return false;
    }
}

bool operator ==(const string &s, const Symbol &sb) {
    return (s == sb.name);
}