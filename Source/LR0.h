//LR0 分析法相关
#include <set>

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

using namespace std;

typedef set<Item> CLOSURE;

class LR0 {
private:
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

public:
    /* initialize 函数: 初始化语法
     */
    void initialize(string gra_dir = "../TestFile/Grammar.txt");

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
};