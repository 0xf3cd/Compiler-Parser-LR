#ifndef LR_
#define LR_
#include "LR.h"
#endif

#ifndef SYMBOL
#define SYMBOL
#include "Symbol.h"
#endif

#ifndef TOKENIZER
#define TOKENIZER
#include "Tokenizer.h"
#endif

#include <set>
#include <vector>
#include <iostream>

using namespace std;

#define START "S"

/* initialize 函数: 初始化语法
 */
//TODO 暂时约定起始变元一定要是 S', 一定以 # 结尾
void LR::initialize(string gra_dir, string src_dir) {
    G.analyzeGrammar(gra_dir);
    G.setStartArgument("S'");
    G.setEndTerminal("#", 0);

    if(T.openSourceFile(src_dir)) {
        //cout << "OPEN SOURCE FILE OK" << endl;
    } else {
        cerr << "FAILED TO OPEN SOURCE FILE" << endl;
    }

    state_stack.push_back(0);
    last_token = T.readNextWord();
}

/* generateItems 函数: 将产生式转换为项目
 */
void LR::generateItems() {
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
void LR::showItems() {
    set<Item>::iterator it;
    for(it = items.begin(); it != items.end(); it++) {
        cout << *it << endl;
    }
}

/* generateFirstClosure 函数: 得到第一个闭包
 * @return: 初始的项目集闭包
 */
//TODO: 修改成能自定义起始变元和点
CLOSURE LR::generateFirstClosure() {
    set<Item>::iterator it_item1, it_item2;
    vector<Symbol>::const_iterator it_right;
    set<Symbol> new_args;
    set<Symbol>::iterator it_na;
    int i;

    CLOSURE first_cls;
    CLOSURE ins, uni;
    Item init_item;
    for(it_item1 = items.begin(); it_item1 != items.end(); it_item1++) {
        if(it_item1 -> left != G.getStartSymbol()) {
            continue;
        }

        //TODO: 修改成能自定义起始变元和点
        vector<Symbol> right = it_item1 -> right;
        if(right[0] == "." && right[1] == START) {
            init_item = *it_item1;
            break;
        }
    }
    
    first_cls.insert(*it_item1);

    bool loop = true;
    while(loop) {
        loop = false;
        new_args.clear();
        for(it_item1 = first_cls.begin(); it_item1 != first_cls.end(); it_item1++) {
            vector<Symbol> const &right = it_item1 -> right;
            for(i = 0; i < right.size(); i++) {

                if(!(right[i] == "." ) || (i + 1) == right.size()) {
                    //如果不为 . 或是 . 在末尾，则跳出
                    continue;
                }

                if(!G.isArgument(right[i+1])) {
                    continue;
                }

                new_args.insert(right[i+1]);
            }
        }
        
        for(it_na = new_args.begin(); it_na != new_args.end(); it_na++) {
            CLOSURE temp = getItemStartWithDot(*it_na);
            uni.clear();
            set_union(first_cls.begin(), first_cls.end(), temp.begin(), temp.end(), inserter(uni, uni.begin()));
            if(uni.size() > first_cls.size()) {
                first_cls = uni;
                loop = true;
            }
        }
    }
    
    return first_cls;
}

CLOSURE LR::extendClosure(CLOSURE cls) {
    set<Item>::iterator it_item1, it_item2;
    set<Symbol> new_args;
    set<Symbol>::iterator it_na;
    CLOSURE ins, uni;
    int i;

    bool loop = true;
    while(loop) {
        loop = false;
        new_args.clear();
        for(it_item1 = cls.begin(); it_item1 != cls.end(); it_item1++) {
            vector<Symbol> const &right = it_item1 -> right;
            for(i = 0; i < right.size(); i++) {

                if(!(right[i] == "." ) || (i + 1) == right.size()) {
                    //如果不为 . 或是 . 在末尾，则跳出
                    continue;
                }

                if(!G.isArgument(right[i+1])) {
                    continue;
                }

                new_args.insert(right[i+1]);
            }
        }

        for(it_na = new_args.begin(); it_na != new_args.end(); it_na++) {
            CLOSURE temp = getItemStartWithDot(*it_na);
            uni.clear();
            set_union(cls.begin(), cls.end(), temp.begin(), temp.end(), inserter(uni, uni.begin()));
            if(uni.size() > cls.size()) {
                cls = uni;
                loop = true;
                // cout << "GOSH\n";
                // cout << endl;
                // for(auto xx = cls.begin(); xx != cls.end(); xx++) {
                //     cout << *xx << endl;
                // }
                // cout << endl;
            }
        }
    }

    return cls;
}

/* generateAllClosure 函数: 算得所有闭包
 */
void LR::generateAllClosure() {
    set<Symbol> a = G.getArguments(); // a for Arguments
    set<Symbol> t = G.getTerminals(); // t for Terminals 
    set<Symbol> symbols;
    set<Symbol>::iterator it_symbols;
    map<int, CLOSURE>::iterator it_cls1, it_cls2;
    
    set_union(a.begin(), a.end(), t.begin(), t.end(), inserter(symbols, symbols.begin()));

    //此时 symbols 中记录着所有符号
    CLOSURE first_cls = generateFirstClosure();

    closures[0] = first_cls; //默认 0 号编号为初始的集合

    int i, j;
    int now_size;
    int last_size = 0;
    bool loop = true;
    while(loop) {
        loop = false;
        now_size = closures.size();
        for(i = last_size; i < now_size; i++) {
            for(it_symbols = symbols.begin(); it_symbols != symbols.end(); it_symbols++) {
                go[i][*it_symbols] = -1;//GO函数初始化为 -1
                CLOSURE c = getNewClosure(i, *it_symbols);
                if(c.size() == 0) { //不可为空
                    continue;
                }

                bool has_shown = false;
                for(j = 0; j < now_size; j++) {
                    if(closures[j] == c) { //如果已有的集合与 c 相同，则跳出
                        has_shown = true;
                        go[i][*it_symbols] = j;
                        break;
                    }
                }

                if(!has_shown) {
                    int temp_size = closures.size();
                    closures[temp_size] = c;

                    go[i][*it_symbols] = temp_size;
                    loop = true;

                    // if(temp_size == 6) {
                    //     cout << c.size() << endl;
                    // }
                }
            }
            
        }
        last_size = now_size;
    }
}

/* getItemStartWithDot 函数: 得到以 A 为左部，. 开头的项目
 * @return: 一个项目集合
 */
set<Item> LR::getItemStartWithDot(Symbol A) {
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
CLOSURE LR::getNewClosure(int I_no,Symbol X) {
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

    return extendClosure(c);
}


void LR::showGO() {
    for(auto it1 = go.begin(); it1 != go.end(); it1++) {
        auto &it1_sec = it1 -> second;
        for(auto it2 = it1_sec.begin(); it2 != it1_sec.end(); it2++) {
            cout << "GO(" << it1 -> first << ", " << (it2 -> first).name << "): " << it2 -> second << endl;
        }
    }
}

void LR::showClosures() {
    for(auto it1 = closures.begin(); it1 != closures.end(); it1++) {
        cout << it1 -> first << ':' << endl;
        auto &second = it1 -> second;
        for(auto it2 = second.begin(); it2 != second.end(); it2++) {
            cout << *it2 << endl;
        }
        cout << endl;
    }
}

/* isLR 函数: 判断是否为 LR(0) 文法
 */
bool LR::isLR0() {
    map<int, CLOSURE>::iterator it_cls;
    set<Item>::iterator it_item;

    for(it_cls = closures.begin(); it_cls != closures.end(); it_cls++) {
        set<Item> &cls = it_cls -> second;
        int shift_count = 0; //shift 移进
        int reduce_count = 0; //reduce 规约
        int i;
        //cls 为一个 closure
        for(it_item = cls.begin(); it_item != cls.end(); it_item++) {
            vector<Symbol> const &right = it_item -> right;
            if(*(right.end() - 1) == ".") {
                //是规约项目
                reduce_count++;
                if(reduce_count > 1) { //有多个规约项目
                    return false;
                }

                if(reduce_count >= 1 && shift_count >= 1) {
                    return false;
                }
            }

            for(i = 0; i < right.size(); i++) {
                if(!(right[i] == ".") || (i + 1) == right.size()) {
                    continue;
                }
                if(G.isTerminal(right[i+1])) {
                    //是移进项目
                    shift_count++;
                }

                if(reduce_count >= 1 && shift_count >= 1) {
                    return false;
                }
            }
        }
    }

    return true;
}

/* isSLR1 函数: 判断是否为 SLR(1) 文法
 */
bool LR::isSLR1() {
    int i, j;
    map<int, CLOSURE>::iterator it_cls;
    set<Item>::iterator it_item;
    vector< FOLLOW_SET > all_conflicts; 
    map<Symbol, FOLLOW_SET> follow = G.getFollow(); //(变元, FOLLOW集) 键值对
    set<Symbol> inter;

    for(it_cls = closures.begin(); it_cls != closures.end(); it_cls++) {
        set<Item> &cls = it_cls -> second; //cls 是项目集
        all_conflicts.clear();
        set<Symbol> after_dot;

        for(it_item = cls.begin(); it_item != cls.end(); it_item++) { //遍历这个项目集中的每一个项目
            vector<Symbol> const &right = it_item -> right; //right 为项目的右部
            Symbol const &left = it_item -> left; //项目左部
            
            if(*(right.end() - 1) == ".") { //如果项目最后的一个字符是 .，说明是规约项目
                FOLLOW_SET left_follow = follow[left]; //找到左部对应的 FOLLOW 集合
                if(find(all_conflicts.begin(), all_conflicts.end(), left_follow) == all_conflicts.end()) {
                    //没有这个set
                    all_conflicts.push_back(left_follow);
                    //cout << "000\n";
                } else {
                    //cout << "111\n";
                }
                continue;
            }
            //否则可能是移进项目
            for(i = 0; i < right.size(); i++) {
                if(!(right[i] == ".") || (i + 1) == right.size()) {
                    continue;
                }
                if(G.isTerminal(right[i+1])) {
                    //是移进项目
                    after_dot.insert(right[i+1]);
                }
            }
        }

        all_conflicts.push_back(after_dot);
        //至此，所有可能存在冲突的集合都保存在 all_conflicts 中了
        for(i = 0; i < all_conflicts.size(); i++) {
            for(j = 0; j < all_conflicts.size(); j++) {
                if(i == j) {
                    continue;
                }
                inter.clear();
                set_intersection(all_conflicts[i].begin(), all_conflicts[i].end(), all_conflicts[j].begin(), all_conflicts[j].end(), inserter(inter, inter.begin()));
                if(inter.size() > 0) {
                    return false;
                }
            }
        }
    }

    return true;
}

void LR::encodeProduction() {
    set<Production> P = G.getProductions();
    set<Production>::iterator it;
    int count = 0;
    for(it = P.begin(); it != P.end(); it++) {
        NO_PROD[count] = *it;
        count++;
    }
}

int LR::findProduction(Item item) {
    Production temp;

    temp.left = item.left;
    temp.right = item.right;

    vector<Symbol>::const_iterator it;
    for(it = temp.right.begin(); it != temp.right.end(); it++) {
        if(*it == ".") {
            break;
        }
    }

    temp.right.erase(it);
    int i;
    for(i = 0; i < NO_PROD.size(); i++) {
        if(NO_PROD[i].left == temp.left && NO_PROD[i].right == temp.right) {
            return i;
        }
    }
    return i;
}

void LR::generateACTION() {
    set<Symbol> t = G.getTerminals();
    Symbol END = G.getEndSymbol();
    int i, j;
    int size = closures.size();
    CLOSURE::iterator it_cls;

    action_unit ERROR;
    ERROR.first = "err";

    encodeProduction();

    for(i = 0; i < size; i++) {
        CLOSURE &cls = closures[i];
        
        for(it_cls = cls.begin(); it_cls != cls.end(); it_cls++) {
            Symbol left = it_cls -> left;
            vector<Symbol> right = it_cls -> right;

            //先全部初始化为 ERROR
            // for(set<Symbol>::iterator it_s = t.begin(); it_s != t.end(); it_s++) {
            //     ACTION[i][*it_s] = ERROR;
            // }
            // ACTION[i][END] = ERROR;

            if(left == "S'" && right[0] == START && right[1] == "." && right.size() == 2) {
                action_unit au;
                au.first = "acc";
                ACTION[i][END] = au;
                continue;
            }

            if(right.back() == ".") {
                action_unit au;
                au.first = "r";
                au.second = findProduction(*it_cls);
                ACTION[i][END] = au;
                for(set<Symbol>::iterator it_s = t.begin(); it_s != t.end(); it_s++) {
                    ACTION[i][*it_s] = au;
                }
                continue;
            }

            for(j = 0; j < right.size(); j++) {
                if(right[j] == ".") {
                    if(G.isTerminal(right[j+1])) {
                        action_unit au;
                        au.first = "s";
                        au.second = go[i][right[j+1]];
                        
                        ACTION[i][right[j+1]] = au;
                    }
                }
            }
        }
    }
}

void LR::generateGOTO() {
    set<Symbol> a = G.getArguments();
    int i;
    int size = closures.size();
    // CLOSURE::iterator it_cls;
    set<Symbol>::iterator it_a;

    for(i = 0; i < size; i++) {
        for(it_a = a.begin(); it_a != a.end(); it_a++) {
            GOTO[i][*it_a] = go[i][*it_a];
        }
    }
}

void LR::showACTION(int state) {
    set<Symbol> t = G.getTerminals();
    Symbol END = G.getEndSymbol();

    for(set<Symbol>::iterator it = t.begin(); it != t.end(); it++) {
        // cout << it -> name << ": " << ACTION[state][*it].first << ' ' << ACTION[state][*it].second << endl;
        if(ACTION[state][*it].first == "s" || ACTION[state][*it].first == "r") {
            cout << ACTION[state][*it].first << ACTION[state][*it].second << '\t';
        } else if(ACTION[state][*it].first == "acc") {
            cout << ACTION[state][*it].first << '\t';
        } else {
            cout << '\t';
        }
    }
    // cout << END.name << ": " << ACTION[state][END].first << ' ' << ACTION[state][END].second << endl;
    if(ACTION[state][END].first == "s" || ACTION[state][END].first == "r") {
        cout << ACTION[state][END].first << ACTION[state][END].second << '\t';
    } else if(ACTION[state][END].first == "acc") {
            cout << ACTION[state][END].first << '\t';
    } else {
        cout << '\t';
    }
}

void LR::showGOTO(int state) {
    set<Symbol> a = G.getArguments();
    for(set<Symbol>::iterator it = a.begin(); it != a.end(); it++) {
        // cout << it -> name << ": " << GOTO[state][*it] << endl;
        cout << GOTO[state][*it] << '\t';
    }
}

/* translate 函数: 将 token 转换成 Symbol
 */
Symbol LR::translate(word token) {
    Symbol s;
    if(token.type == 7) { //ID
        s.name = "ID";
    } else if(token.type == 8) { //num
        s.name = "num";
    } else {
        s.name = token.value;
    }
    s.no = token.type;
    return s;
}

snapshot LR::getNext() {
    snapshot ss; //ss for SnapShot
    Symbol now_symbol;
    Production now_production; //对应的产生式

    //跳过注释符号
    while(last_token.type >= 26) {
        last_token = T.readNextWord();
    }

    //确定此时的栈的情况
    ss.state_stack = state_stack;
    ss.symbol_stack = symbol_stack;
    ss.token = last_token;

    //如若词法分析出错
    if(last_token.type < 0) {
        ss.error = -1; //-1 表示词法出错
        return ss;
    }

    now_symbol = translate(last_token);
    ss.symbol = now_symbol;

    int state_top = state_stack.front();
    // state_stack.pop_front(); //弹栈
    action_unit au = ACTION[state_top][now_symbol];
    ss.au = au;

    if(au.first == "") {
        ss.error = -2; //出错
        return ss;
    } else if(au.first == "s") {
        symbol_stack.push_front(now_symbol);
        state_stack.push_front(au.second);
        last_token = T.readNextWord();
        ss.error = 1;
    } else if(au.first == "r") {
        state_stack.pop_front(); //弹栈
        now_production = NO_PROD[au.second];

        //进行规约
        int i, length;
        length = now_production.right.size();
        for(i = 0; i < length; i++) {
            symbol_stack.pop_front();
        }
        symbol_stack.push_front(now_production.left);
        ss.production = now_production;

        //更新栈顶记录
        state_top = state_stack.front();
        state_stack.push_front(GOTO[state_top][now_production.left]);

        ss.error = 2;
        return ss;
    } else if(au.first == "acc") {
        ss.error = 3;
        return ss;
    } else {
        ss.error = -4;
        cout << "\nEXCEPT: " << ss.au.first << endl;
    }

    
    return ss;
}