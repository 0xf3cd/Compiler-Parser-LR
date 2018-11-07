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

/* initialize 函数: 初始化语法
 */
//TODO 暂时约定起始变元一定要是 S', 一定以 # 结尾
void LR0::initialize(string gra_dir) {
    G.analyzeGrammar(gra_dir);
    G.setStartArgument("S'");
    G.setEndTerminal("#", 0);
}

/* generateItems 函数: 将产生式转换为项目
 */
void LR0::generateItems() {
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

/* generateFirstClosure 函数: 得到第一个闭包
 * @return: 初始的项目集闭包
 */
//TODO: 修改成能自定义起始变元和点
CLOSURE LR0::generateFirstClosure() {
    set<Item>::iterator it_item;
    vector<Symbol>::const_iterator it_right;

    CLOSURE &first_cls = closures[0];
    Item init_item;
    for(it_item = items.begin(); it_item != items.end(); it_item++) {
        if(it_item -> left != G.getStartSymbol()) {
            continue;
        }

        //TODO: 修改成能自定义起始变元和点
        vector<Symbol> right = it_item -> right;
        if(right[0] == "." && right[1] == "E") {
            init_item = *it_item;
            break;
        }
    }
    
    first_cls.insert(init_item);

    for(it_item = first_cls.begin(); it_item != first_cls.end(); it_item++) {
        for(it_right = (*it_item).right.begin(); it_right != (*it_item).right.end(); it_right++) {
            //TODO: 修改成能自定义起始变元和点
            if(*it_right == ".") {
                it_right++;
                if(it_right == (*it_item).right.end()) {
                    it_right--;
                    continue;
                }

                if(G.isArgument(*it_right)) {
                    set<Item> s = getItemStartWithDot(*it_right);
                    set<Item>::iterator it;
                    for(it = s.begin(); it != s.end(); it++) {
                        first_cls.insert(*it);
                    }
                }
                it_right--;
            }
        }
    }

    return first_cls;
}

/* generateAllClosure 函数: 算得所有闭包
 */
void LR0::generateAllClosure() {
    set<Symbol> a = G.getArguments(); // a for Arguments
    set<Symbol> t = G.getTerminals(); // t for Terminals 
    set<Symbol> symbols;
    
    set_union(a.begin(), a.end(), t.begin(), t.end(), inserter(symbols, symbols.begin()));

    //此时 symbols 中记录着所有符号

    CLOSURE first_cls = generateFirstClosure();
    closures[0] = first_cls; //默认 0 号编号为初始的集合

    for(auto it = symbols.begin(); it != symbols.end(); it++) {
        
    }
    auto it = symbols.begin();
    it++;
    it++;
    it++;
    it++;
    auto s = *it;
    //go[I_no][X] = -1;//初始化为 -1 
    auto x = getNewClosure(0, s);
    cout << s << endl;
    for(auto it = x.begin(); it != x.end(); it++) {
        cout << *it << endl;
    }
}

/* getItemStartWithDot 函数: 得到以 A 为左部，. 开头的项目
 * @return: 一个项目集合
 */
set<Item> LR0::getItemStartWithDot(Symbol A) {
    set<Item> s;//s for Set
    set<Item>::iterator it;

    for(it = items.begin(); it != items.end(); it++) {
        if(it -> left != A) {
            continue;
        }
        //此时产生式的左部为 A
        if((it -> right)[0] == ".") {
            s.insert(*it);
        }
    }

    return s;
}

/* getNewClosure 函数: 计算下一个 CLOSURE
 * @param I: GO(I, X) 中的 I，即为一个 closure 集合
 * @param X: GO(I, X) 中的 X，即为一个符号
 */
CLOSURE LR0::getNewClosure(int I_no,Symbol X) {
    set<Symbol>::iterator it_s;
    CLOSURE::iterator it_c;
    CLOSURE J;
    CLOSURE c; //c for Closure
    int i;

    for(it_c = closures[I_no].begin(); it_c != closures[I_no].end(); it_c++) {
        //*it_c 为一个 Item
        vector<Symbol> const &right = it_c -> right;
        for(i = 0; i < right.size(); i++) {
            //希望寻找到形如 A -> a.Xb 的项目
            if(!(right[i] == ".") || i == (right.size() - 1)) {
                //如果当前字符不是 . ，或者 . 出现在右部末尾，则跳出
                continue;
            }
            
            if(right[i+1] != X) {
                //如果 . 之后不是期待的符号，则跳出
                continue;
            }
            //此时找到了对应项目
            Item new_item = *it_c;
            vector<Symbol> &new_right = new_item.right;
            new_right.erase(new_right.begin() + i); //删除 .
            new_right.insert(new_right.begin() + i + 1, G.getDot());
            J.insert(new_item);
        }
    }
    //至此，得到了 J，接下来求解 CLOSURE(J)
    for(it_c = J.begin(); it_c != J.end(); it_c++) {
        c.insert(*it_c);
        //Item new_item = *it_c;
        vector<Symbol> const &right = it_c -> right;
        int i;
        vector<Symbol>::const_iterator it;
        for(i = 0; i < right.size(); i++) {
            if(!(right[i] == ".") || i + 1 == right.size()) {
                //如果 right[i] 不为 .，则跳出
                //如果 i 为末尾元素，则跳出
                continue;
            }
            if(!G.isArgument(right[i+1])) {
                //如果 . 后面的不为变元，则跳出
                continue;
            }

            CLOSURE temp = getItemStartWithDot(right[i+1]);
            for(CLOSURE::iterator it = temp.begin(); it != temp.end(); it++) {
                c.insert(*it);
            }
        }
    }

    return c;
}