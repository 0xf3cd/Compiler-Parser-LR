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
    set<Symbol>::iterator it_symbols;
    map<int, CLOSURE>::iterator it_cls1, it_cls2;
    
    set_union(a.begin(), a.end(), t.begin(), t.end(), inserter(symbols, symbols.begin()));

    //此时 symbols 中记录着所有符号

    CLOSURE first_cls = generateFirstClosure();
    closures[0] = first_cls; //默认 0 号编号为初始的集合

    bool loop = true;
    while(loop) {
        loop = false;
        for(it_cls1 = closures.begin(); it_cls1 != closures.end(); it_cls1++) {
            int no = it_cls1 -> first;
            for(it_symbols = symbols.begin(); it_symbols != symbols.end(); it_symbols++) {
                go[no][*it_symbols] = -1;//GO函数初始化为 -1 
                CLOSURE c = getNewClosure(no, *it_symbols);
                if(c.size() == 0) { //不可为空
                    continue;
                }
                bool has_shown = false;
                for(it_cls2 = closures.begin(); it_cls2 != closures.end(); it_cls2++) {
                    if(it_cls2 -> second == c) { //如果已有的集合与 c 相同，则跳出
                        has_shown = true;
                        break;
                    }
                }
                if(has_shown) {
                    //如果出现
                    go[no][*it_symbols] = it_cls2 -> first; //指定 GO 函数
                } else {
                    //如果未出现
                    int size = closures.size();
                    closures[size] = c;
                    go[no][*it_symbols] = size;
                    loop = true;
                }
            }
        }
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

    // if(I_no == 8) {
    //     cout << 8 << endl;
    //     for(auto it = c.begin(); it != c.end(); it++) {
    //         cout << *it << endl;
    //     }
    // }

    return c;
}


void LR0::showGO() {
    for(auto it1 = go.begin(); it1 != go.end(); it1++) {
        auto &it1_sec = it1 -> second;
        for(auto it2 = it1_sec.begin(); it2 != it1_sec.end(); it2++) {
            cout << "GO(" << it1 -> first << ", " << (it2 -> first).name << "): " << it2 -> second << endl;
        }
    }
}

void LR0::showClosures() {
    for(auto it1 = closures.begin(); it1 != closures.end(); it1++) {
        cout << it1 -> first << ':' << endl;
        auto &second = it1 -> second;
        for(auto it2 = second.begin(); it2 != second.end(); it2++) {
            cout << *it2 << endl;
        }
        cout << endl;
    }
}