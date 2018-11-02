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

class LR0 {
private:
    set<Item> items;

public:
    // LR0() {}
    // ~LR0() {}

    /* generateItems 函数: 将产生式转换为项目
     */
    void generateItems(Grammar &G);

    /* showItems 函数: 展示所有项目
     */
    void showItems();
};