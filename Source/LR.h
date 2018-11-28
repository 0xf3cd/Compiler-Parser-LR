//LR0 分析法相关
#include <set>
#include <list>

#ifndef ITEM
#define ITEM
#include "Item.h"
#endif

#ifndef PRODUCTION
#define PRODUCTION
#include "Production.h"
#endif

#ifndef GRAMMAR
#define GRAMMAR
#include "Grammar.h"
#endif

#ifndef TOKENIZER
#define TOKENIZER
#include "Tokenizer.h"
#endif

using namespace std;

typedef set<Item> CLOSURE;

struct action_unit {
    string first;
    int second;
};

struct snapshot {
    action_unit au;
    word token;
    Symbol symbol;
    Production production;
    int error;
    list<Symbol> symbol_stack;
    list<int> state_stack;
};

class LR {
private:
    Tokenizer T;
    Grammar G;
    set<Item> items;
    map<int, CLOSURE> closures; //编号

    /* 表示GO(I, X);
     * I 为项目集编号 
     * X 为 Symbol
     * 返回值也为项目集编号
     * 返回 -1 说明没有对应的项目集
     * 通过 go[I][X] 索引
     */
    map<int, map<Symbol, int> > go; 

    map<int, map<Symbol, action_unit> > ACTION;

    map<int, map<Symbol, int> > GOTO;

    map<int, Production> NO_PROD;

    /* generateFirstClosure 函数: 得到第一个闭包
     * @return: 初始的项目集闭包
     */
    CLOSURE generateFirstClosure();

    /* getItemStartWithDot 函数: 得到以 A 为左部，. 开头的项目
     * @return: 一个项目集合
     */
    set<Item> getItemStartWithDot(Symbol A);

    /* getNewClosure 函数: 计算下一个 CLOSURE
     * @param I_no: GO(I, X) 中的 I，即为一个 closure 集合，传入集合编号即可 即 closures[I_no] 为所需 closure
     * @param X: GO(I, X) 中的 X，即为一个符号
     */
    CLOSURE getNewClosure(int I_no,Symbol X);

    void encodeProduction();

    int findProduction(Item item);

    CLOSURE extendClosure(CLOSURE cls);

    /* translate 函数: 将 token 转换成 Symbol
     */
    Symbol translate(word token);

    list<Symbol> symbol_stack;
    list<int> state_stack;
    word last_token; //上一次通过词法分析器读入的词
public:
    /* initialize 函数: 初始化语法
     */
    void initialize(string gra_dir = "../TestFile/Grammar.txt", string src_dir = "../TestFile/example.cmm");

    /* generateItems 函数: 将产生式转换为项目
     */
    void generateItems();

    /* generateAllClosure 函数: 算得所有闭包
     */
    void generateAllClosure();

    /* showItems 函数: 展示所有项目
     */
    void showItems();

    void showGO();

    void showClosures();

    /* isLR0 函数: 判断是否为 LR(0) 文法
     */
    bool isLR0();

    /* isSLR1 函数: 判断是否为 SLR(1) 文法
     */
    bool isSLR1();

    void generateACTION();

    void generateGOTO();

    void showACTION(int state);
    void showGOTO(int state);

    snapshot getNext();
};