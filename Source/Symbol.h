//符号
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

/* Symbol 是描述一个文法中某一个符号的结构体
 */
class Symbol {
public:
    string name; //符号的名称，如 S、E、a、* 等
    mutable int no; //终结符或变元的编号

    Symbol();

    Symbol(string s);

    /* 更改 no 值
     */
    void setNo(int new_no) const;

    /* 使用 C++ STL 库时，需要定义 < 运算符
     */
    bool operator <(const Symbol &s) const;

    bool operator >(const Symbol &s) const;

    Symbol& operator =(const Symbol &s);

    bool operator ==(const Symbol &s) const;

    bool operator !=(const Symbol &s) const;

    friend ifstream &operator >>(ifstream &fin, Symbol &s);

    friend ostream &operator <<(ostream &cout, const Symbol &s);

    operator string();

    //以下是与 string 之间的 == 重载
    bool operator ==(const string &s) const;
    friend bool operator ==(const string &s, const Symbol &sb);
};