#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <iostream>
#include <algorithm>

#ifndef GRAMMAR
#define GRAMMAR
#include "Grammar.h"
#endif

using namespace std;

/* readInArguments 函数: 读入文件中的变元
 * 在函数中完成对 argument_amount 和 arguments 集合的修改
 * @param fin: 打开的文件对象
 */
void Grammar::readInArguments(ifstream &fin) {
    int i;
    Symbol s;

    fin >> argument_amount; //读取变元数量
    for(i = 0; i < argument_amount; i++) {
        fin >> s;
        arguments.insert(s);
    }
}

/* readInTerminals 函数: 读入文件中的终结符
 * 在函数中完成对 terminal_amount 和 terminals 集合的修改
 * @param fin: 打开的文件对象
 */
void Grammar::readInTerminals(ifstream &fin) {
    int i;
    Symbol s;

    fin >> terminal_amount; //读取终止符数量
    for(i = 0; i < terminal_amount; i++) {
        fin >> s;
        terminals.insert(s);
    }
}

/* readInProductions 函数: 读入文件中的产生式，并分拆 '|' 所隔开的产生式
 * 最终 productions 集合中的产生式右部不存在 '|' 字符 
 * 在函数中完成对 production_amount 和 productions 集合的修改
 * @param fin: 打开的文件对象
 */
bool Grammar::readInProductions(ifstream &fin) {
    int i;
    string read_in;
    Production p;

    fin >> production_amount;
    for(i = 0; i < production_amount; i++) {
        fin >> p;
        productions.insert(p);
    }

    return checkProductions();
}

/* checkProductions 函数: 检查读入的文法是否正确
 * @return: 检查的结果
 */
bool Grammar::checkProductions() {
    set<Production>::iterator it;
    vector<Symbol>::const_iterator its;
    Symbol left;
    Symbol right;

    for(it = productions.begin(); it != productions.end(); it++) {
        left = it -> left;
        if(!isArgument(left.name)) {
            return false;
        } else {
            left = findArgument(left.name);
            (it -> left).setNo(left.no);
        }

        for(its = (it -> right).begin(); its != (it -> right).end(); its++) {
            right = *its;
            if(isTerminal(right.name)) {
                right = findTerminal(right.name);
                (*its).setNo(right.no);
            } else if(isArgument(right.name)) {
                right = findArgument(right.name);
                (*its).setNo(right.no);
            } else {
                return false;
            }
        }
    }

    return true;
}

/* generateSymbolFirst 函数: 根据产生式、变元和产生式，生成每个变元对应的 FIRST 集合
 * 结果保存在 symbol_first 键值对中
 * 即这个函数产生 FIRST(X) 的结果，X ∈ Vn
 */
void Grammar::generateSymbolFirst() {
    set<Symbol>::iterator it_arg; //变元迭代器
    set<Production>::iterator it_prod; //产生式迭代器
    vector<Symbol>::const_iterator it_right; //产生式右部符号的迭代器
    Symbol left, right; //分别表示产生式左部和右部的符号
    
    //EPSILON 为空，单独拎出来成一个变量，方便后面的比较
    const Symbol EPSILON = findArgument("ε");
    set<Symbol> EPSILON_SET;
    //EPSILON_SET 为仅有 EPSILON 的集合，拎出来方便后面集合的并和差运算
    EPSILON_SET.insert(EPSILON);

    set<Symbol> d_temp; //用户稍后集合差运算
    set<Symbol> u_temp; //用户稍后的集合并运算

    //第一步，对所有 X ∈ Vn，若有 X -> a... 或 X -> ε，则将 a 或 ε 加入 FIRST(X)
    for(it_prod = productions.begin(); it_prod != productions.end(); it_prod++) {
        left = it_prod -> left; //某个产生式的左部
        it_right = (*it_prod).right.begin(); //只考察右部第一个元素
        if(isTerminal(*it_right)) {
            symbol_first[it_prod -> left].insert(*it_right);
        }
    }

    //第二步，不断收取新的终结符，直至所有 FIRST(X) 都不再变化
    bool loop = true;
    while(loop) {
        loop = false;
        for(it_prod = productions.begin(); it_prod != productions.end(); it_prod++) {
            left = it_prod -> left; //某个产生式的左部
            FIRST_SET &A = symbol_first[left]; //简化后面的代码书写
            //接下来从左向右遍历某个产生式的右部
            for(it_right = (*it_prod).right.begin(); it_right != (*it_prod).right.end(); it_right++) {
                //若为终结符则停止
                if(isTerminal(*it_right)) {
                    if(A.find(*it_right) == A.end()) {
                        A.insert(*it_right);
                        // cout << "1" << endl;
                        loop = true;
                    }
                    break;
                }

                FIRST_SET &B = symbol_first[*it_right]; //产生式右部的某一个变元
                // if(B.size() == 0) {
                //     break;
                // }

                //以下进行集合的差操作，将 B 集合减去 EPSILON_SET 的结果保存在 d_temp
                d_temp.clear();
                set_difference(B.begin(), B.end(), EPSILON_SET.begin(), EPSILON_SET.end(), inserter(d_temp, d_temp.begin()));
                //一下进行集合的并操作，将 A 集合和之前差运算得到的 d_temp 并在一起
                u_temp.clear();
                set_union(A.begin(), A.end(), d_temp.begin(), d_temp.end(), inserter(u_temp, u_temp.begin()));
            
                if(A != u_temp) {           
                    A = u_temp; //更新
                    // cout << "2" << endl << endl;
                    loop = true;
                }

                if(B.find(EPSILON) == B.end()) { // B 中没有 ε，则停止收取
                    break;
                } else { // B 中有 ε
                    //执行至此时，说明之前 Y1 Y2 ... 都可以产生 ε
                    it_right++;
                    if(it_right == (*it_prod).right.end()) { //如果产生式右部已经遍历完成
                        if(A.find(EPSILON) == A.end()) {
                            A.insert(EPSILON); //加入 EPSILON
                            // cout << "3" << endl;
                            loop = true;
                        }
                    } else { //右部未遍历完成，需要继续
                        it_right--;
                    }
                }
            }
        }
    }
}

/* generateFirst 函数: 根据产生式、变元和产生式，生成每个产生式对应的 FIRST 集合
 * 结果保存在 first 键值对中
 * 即这个函数产生 FIRST(α) 的结果
 */
void Grammar::generateFirst() {
    set<Production>::iterator it_prod; //产生式迭代器
    vector<Symbol>::const_iterator it_right; //右部的符号迭代器
    Symbol left, right; //分别表示产生式左部和右部的符号

    //EPSILON 为空，单独拎出来成一个变量，方便后面的比较
    const Symbol EPSILON = findArgument("ε");
    set<Symbol> EPSILON_SET;
    //EPSILON_SET 为仅有 EPSILON 的集合，拎出来方便后面集合的并和差运算
    EPSILON_SET.insert(EPSILON);

    set<Symbol> d_temp; //用户稍后集合差运算
    set<Symbol> u_temp; //用户稍后的集合并运算

    for(it_prod = productions.begin(); it_prod != productions.end(); it_prod++) {
        FIRST_SET &A = first[*it_prod]; //使用引用，简化代码，A 为产生式对应的 FIRST 集合

        for(it_right = (*it_prod).right.begin(); it_right != (*it_prod).right.end(); it_right++) {
            if(isTerminal(*it_right)) { //如果遍历到的符号是终结符，则停止收取
                if(*it_right != EPSILON) {
                    A.insert(*it_right);
                }
                break;
            }

            FIRST_SET &B = symbol_first[*it_right]; // B 为所遍历到的变元的 FIRST 集的引用
            //以下进行集合的差操作，将 B 集合减去 EPSILON_SET 的结果保存在 d_temp
            d_temp.clear();
            set_difference(B.begin(), B.end(), EPSILON_SET.begin(), EPSILON_SET.end(), inserter(d_temp, d_temp.begin()));
            //一下进行集合的并操作，将 A 集合和之前差运算得到的 d_temp 并在一起
            u_temp.clear();
            set_union(A.begin(), A.end(), d_temp.begin(), d_temp.end(), inserter(u_temp, u_temp.begin()));

            A = u_temp;

            if(B.find(EPSILON) == B.end()) { //若 B 中不存在 ε，则停止收取
                break;
            }
        }

        if(it_right == (*it_prod).right.end()) { //若先前遍历完了产生式右部所有的符号，则需要在 A 中加入 ε
            A.insert(EPSILON);
        }
    }
}

/* generateFirst 函数: 根据产生式、变元和产生式，生成每个变元对应的 FOLLOW 集合
 * 结果保存在 follow 键值对中
 */
void Grammar::generateFollow() {
    set<Symbol>::iterator it_arg; //变元迭代器
    set<Production>::iterator it_prod; //产生式迭代器
    vector<Symbol>::const_iterator it_right; //产生式右部符号的迭代器
    Symbol left, right; //分别表示产生式左部和右部的符号
    vector<Symbol>::const_reverse_iterator it_rev; //产生式右部的反向迭代器
    
    //EPSILON 为空，单独拎出来成一个变量，方便后面的比较
    const Symbol EPSILON = findTerminal("ε");
    set<Symbol> EPSILON_SET;
    //EPSILON_SET 为仅有 EPSILON 的集合，拎出来方便后面集合的并和差运算
    EPSILON_SET.insert(EPSILON);

    //起始变元
    const Symbol START = findArgument(START_ARG);

    set<Symbol> d_temp; //用户稍后集合差运算
    set<Symbol> u_temp; //用户稍后的集合并运算

    //向起始变元对应的 FOLLOW 集中加入 '#'
    Symbol END = Symbol(END_TER);
    END.setNo(END_TYPE);
    follow[START].insert(END);

    //对应课件中构造算法的第二步，直接根据产生式中收取
    for(it_arg = arguments.begin(); it_arg != arguments.end(); it_arg++) {
        FOLLOW_SET &A = follow[*it_arg]; //取 A 为某个变元对应 FOLLOW 集的引用
        //接下来遍历所有产生式，查找右部中是否有 *it_arg 这个变元
        for(it_prod = productions.begin(); it_prod != productions.end(); it_prod++) {
            //对于每个产生式，从左至右进行扫描
            for(it_right = (*it_prod).right.begin(); it_right != (*it_prod).right.end(); it_right++) {
                if(*it_right == *it_arg) { //如果找到了对应的变元
                    vector<Symbol>::const_iterator it_temp; //临时变元迭代器

                    //接下来将得到 FIRST(ß)
                    it_right++;
                    it_temp = it_right;
                    it_right--;
                    for(; it_temp != (*it_prod).right.end(); it_temp++) {
                        if(isTerminal(*it_temp)) { //如果遍历到的符号是终结符，则停止收取
                            if(*it_right != EPSILON) {
                                A.insert(*it_temp);
                            }
                            break;
                        }

                        //否则 it_right 是变元
                        FIRST_SET &B = symbol_first[*it_temp]; // B 为所遍历到的变元的 FIRST 集的引用
                        //以下进行集合的差操作，将 B 集合减去 EPSILON_SET 的结果保存在 d_temp
                        d_temp.clear();
                        set_difference(B.begin(), B.end(), EPSILON_SET.begin(), EPSILON_SET.end(), inserter(d_temp, d_temp.begin()));
                        
                        //一下进行集合的并操作，将 A 集合和之前差运算得到的 d_temp 并在一起
                        u_temp.clear();
                        set_union(A.begin(), A.end(), d_temp.begin(), d_temp.end(), inserter(u_temp, u_temp.begin()));
                        
                        A = u_temp;

                        if(B.find(EPSILON) != B.end()) { //如果 B 的 FIRST 集中没有 ε
                            break;
                        }
                    }
                }
            }
        }
    }

    //第三步，当变元 A 存在于产生式 B -> ...A 或 B -> ...Aß 但 ß 可以推导出空时，则将 FOLLOW(B) 加入 FOLLOW(A)
    bool loop = true;
    while(loop) {
        loop = false;
        for(it_prod = productions.begin(); it_prod != productions.end(); it_prod++) {
            left = it_prod -> left;
            FOLLOW_SET &B = follow[left]; //按照课件，将产生式的左部对应 FOLLOW 集合记为 B
            //对每一个产生式进行反向迭代
            for(it_rev = (*it_prod).right.rbegin(); it_rev != (*it_prod).right.rend(); it_rev++) {
                if(isArgument(*it_rev)) { //为变元
                    FOLLOW_SET &A = follow[*it_rev]; //将遍历到的变元对应的 FOLLOW 集记为 A

                    //一下进行集合的并操作，将 A 集合 B 集合并在一起
                    u_temp.clear();
                    set_union(A.begin(), A.end(), B.begin(), B.end(), inserter(u_temp, u_temp.begin()));

                    if(A != u_temp) {
                        loop = true;
                        A = u_temp;
                    }

                    if(symbol_first[*it_rev].find(EPSILON) == symbol_first[*it_rev].end()) { //若 symbol_first[*it_rev] 中无 ε
                        break;
                    }
                } else if(*it_rev == EPSILON) { //为空字符
                    continue;
                } else { //否则是非空终结符
                    break;
                }
            }
        }
    }
}

/* isTerminal 函数: 判断输入的字符串是否是一个终结符
 * @param input: 字符串
 * @return: true or false，表示是否为一个终结符
 */
bool Grammar::isTerminal(string input) {
    set<Symbol>::iterator it;
    for(it = terminals.begin(); it != terminals.end(); it++) {
        if(it -> name == input) {
            return true;
        }
    }

    return false;
}
//重载
bool Grammar::isTerminal(Symbol input) {
    set<Symbol>::iterator it;
    for(it = terminals.begin(); it != terminals.end(); it++) {
        if(*it == input) {
            return true;
        }
    }

    return false;
}

/* isArgument 函数: 判断输入的字符串是否是一个变元
 * @param input: 字符串
 * @return: true or false，表示是否为一个变元
 */
bool Grammar::isArgument(string input) {
    set<Symbol>::iterator it;
    for(it = arguments.begin(); it != arguments.end(); it++) {
        if(it -> name == input) {
            return true;
        }
    }

    return false;
}
//重载
bool Grammar::isArgument(Symbol input) {
    set<Symbol>::iterator it;
    for(it = arguments.begin(); it != arguments.end(); it++) {
        if(*it == input) {
            return true;
        }
    }

    return false;
}

/* findArgument 函数: 根据输入的字符串找到对应的 Symbol 变元对象，并返回
 * @param input: 字符串
 * @return: Symbol，对应变元
 */
Symbol Grammar::findArgument(string input) {
    set<Symbol>::iterator it;
    Symbol to_return;
    for(it = arguments.begin(); it != arguments.end(); it++) {
        if(*it == input) {
            to_return = *it;
            return to_return;
        }
    }
    to_return = *it;
    return to_return; //如果没有找到则返回最后一个元素
}

/* findTerminal 函数: 根据输入的字符串找到对应的 Symbol 终结符对象，并返回
 * @param input: 字符串
 * @return: 对应终结符
 */
Symbol Grammar::findTerminal(string input) {
     set<Symbol>::iterator it;
    Symbol to_return;
    for(it = terminals.begin(); it != terminals.end(); it++) {
        if(*it == input) {
            to_return = *it;
            return to_return;
        }
    }
    to_return = *it;
    return to_return; //如果没有找到则返回最后一个元素
}

/* 构造函数: 初始化成员变量的值
 */
Grammar::Grammar() {
    has_got_grammar = false;
    argument_amount = -1;
    terminal_amount = -1;
    production_amount = -1;
    //start_arg = Symbol(START_ARG);
}

/* 析构函数: 清空集合
 */
// Grammar::~Grammar() {
// }

/* setStartSymbol 函数: 设定起始变元
 * 默认为 S
 */
// void Grammar::setStartSymbol(string name) {
//     start_arg = name;
// }

/* analyzeGrammar 函数: 读入文件中变元、终结符和产生式
 * @param direcory: 文件地址
 * @return: 返回 true 或 false，表示是否读取成功
 */
bool Grammar::analyzeGrammar(string directory) {
    int i;
    ifstream fin;

    fin.open(directory, ios::in);
    if(!fin.is_open()) {
        has_got_grammar = false;
        cerr << "Can't open " << directory << endl;
        return false;
    }


    //至此，文件打开成功
    readInArguments(fin);
    readInTerminals(fin);
    if(!readInProductions(fin)) {
        cerr << "Grammar is not correct" << endl;
        fin.close();
        return false;
    }
    generateSymbolFirst();
    generateFirst();
    generateFollow();

    has_got_grammar = true;
    fin.close();
    return true;
}

/* hasAnalyzedGrammar 函数: 返回是否成功分析了语法
 * @return: true or false
 */
bool Grammar::hasAnalyzedGrammar() {
    return has_got_grammar;
}

/* isLL1 函数: 判断是否为 LL1 文法
 * @return: true or false
 */
bool Grammar::isLL1() {
    // 保证 A -> a1... | a2... | a3... 的 FIRST 集互不相交
    set<Production>::iterator it_p; //产生式迭代器
    set<Symbol>::iterator it_a; //起始变元迭代器
    
    set<FIRST_SET> first_of_X; //记录起始变元所有产生式对应的 FIRST 集的集合
    set<FIRST_SET>::iterator it_FS1, it_FS2;

    //EPSILON 为空，单独拎出来成一个变量，方便后面的比较
    const Symbol EPSILON = findTerminal("ε");
    set<Symbol> EPSILON_SET;
    //EPSILON_SET 为仅有 EPSILON 的集合，拎出来方便后面集合的并和差运算
    EPSILON_SET.insert(EPSILON);

    FIRST_SET i_temp; //用于之后的并操作

    for(it_a = arguments.begin(); it_a != arguments.end(); it_a++) {
        first_of_X.clear();
        for(it_p = productions.begin(); it_p != productions.end(); it_p++) {
            if(it_p -> left == *it_a) {
                //如果遍历到当前的变元，则将这个产生式对应的 FIRST 集加入 fisrt_of_X
                first_of_X.insert(first[*it_p]);
            }
        }
        //跳出上述循环时，first_of_X 中保留所有以 *it_a 为左部的产生式对应的 FIRST 集
        if(first_of_X.size() <= 1) {
            continue;
        }
        //如果有两个或以上的产生式以 *it_a 结尾，则需要判断他们是否两两不相交
        for(it_FS1 = first_of_X.begin(); it_FS1 != first_of_X.end(); it_FS1++) {
            it_FS1++;
            it_FS2 = it_FS1;
            it_FS1--;
            for(; it_FS2 != first_of_X.end(); it_FS2++) {
                //取两个产生式对应的 FIRST 集
                FIRST_SET const &A = *it_FS1;
                FIRST_SET const &B = *it_FS2;

                i_temp.clear();                
                set_intersection(A.begin(), A.end(), B.begin(), B.end(), inserter(i_temp, i_temp.begin()));

                if(i_temp.size() != 0) { //如果交集不为空
                    return false;
                }
            }
        }

        //判断变元对应的 FIRST 集中是否有 ε
        if(symbol_first[*it_a].find(EPSILON) != symbol_first[*it_a].end()) {
            //找到所选变元的 FIRST 和 FOLLOW 集，它们理应无交集
            FIRST_SET const &FIRST = symbol_first[*it_a];
            FOLLOW_SET const &FOLLOW = follow[*it_a];

            i_temp.clear();                
            set_intersection(FIRST.begin(), FIRST.end(), FOLLOW.begin(), FOLLOW.end(), inserter(i_temp, i_temp.begin()));

            if(i_temp.size() != 0) {
                return false;
            }
        }
    }

    return true;
}

/* showArguments 函数: 展示读取到的变元
 */
void Grammar::showArguments() {
    if(has_got_grammar == false) {
        cerr << "Grammar hasn't been analyzed yet" << endl;
        return;
    }

    set<Symbol>::iterator its;

    //cout << "Arguments: " << argument_amount << endl;
    for(its = arguments.begin(); its != arguments.end(); its++) {
        cout << *its << endl;
    }
    cout << endl;
}

/* showTerminals 函数: 展示读取到的终结符
 */
void Grammar::showTerminals() {
    if(has_got_grammar == false) {
        cerr << "Grammar hasn't been analyzed yet" << endl;
        return;
    }
    
    set<Symbol>::iterator its;

    //cout << "Terminals: " << terminal_amount << endl;
    for(its = terminals.begin(); its != terminals.end(); its++) {
        cout << *its << endl;
    }   
    cout << endl;
}

/* showProductions 函数: 展示读取到的产生式
 */
void Grammar::showProductions() {
    if(has_got_grammar == false) {
        cerr << "Grammar hasn't been analyzed yet" << endl;
        return;
    }

    set<Production>::iterator itp;

    //cout << "Productions: " << production_amount << endl;
    for(itp = productions.begin(); itp != productions.end(); itp++) {
        cout << *itp;
        cout << endl;
    }
    cout << endl;
}   

/* showSymbolFirst 函数: 展示求得的变元的 FIRST 集合
 */
void Grammar::showSymbolFirst() {
    set<Symbol>::iterator it_arg; //变元迭代器
    FIRST_SET::iterator it_first; //FIRST集合迭代器
    
    //cout << endl << "FIRST(X):" << endl;
    for(it_arg = arguments.begin(); it_arg != arguments.end(); it_arg++) {
        FIRST_SET &sf =  symbol_first[*it_arg];
        cout << *it_arg << ": ";
        for(it_first = sf.begin(); it_first != sf.end(); it_first++) {
            cout << it_first -> name << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

/* showFirst 函数: 展示求得的产生式的 FIRST 集合
 */
void Grammar::showFirst() {
    set<Production>::iterator it_prod; //产生式迭代器
    FIRST_SET::iterator it_first; //FIRST集合迭代器

    //cout << endl << "FIRST(α):" << endl;
    for(it_prod = productions.begin(); it_prod != productions.end(); it_prod++) {
        FIRST_SET &s =  first[*it_prod];
        cout << *it_prod << ": ";
        for(it_first = s.begin(); it_first != s.end(); it_first++) {
            cout << it_first -> name << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

/* showFollow 函数: 展示求得的 FOLLOW 集合
 */
void Grammar::showFollow() {
    set<Symbol>::iterator it_arg; //变元迭代器
    FOLLOW_SET::iterator it_follow; //FOLLOW集合迭代器

    //cout << endl << "FOLLOW(A):" << endl;
    for(it_arg = arguments.begin(); it_arg != arguments.end(); it_arg++) {
        FIRST_SET &f = follow[*it_arg];
        cout << *it_arg << ": ";
        for(it_follow = f.begin(); it_follow != f.end(); it_follow++) {
            cout << it_follow -> name << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

/* getArguments 函数:返回变元集
 */
set<Symbol> & Grammar::getArguments() {
    return arguments;
}

/* getTerminals 函数:返回终结符集
 */
set<Symbol> & Grammar::getTerminals() {
    return terminals;
}

/* getProductions 函数:返回产生式集
 */
set<Production> & Grammar::getProductions() {
    return productions;
}

/* getSymbolFirst 函数:返回 (变元, FIRST集) 键值对
 */
map<Symbol, FIRST_SET> & Grammar::getSymbolFirst() {
    return symbol_first;
}

/* getFirst 函数:返回 (产生式, FIRST集) 键值对
 */
map<Production, FIRST_SET> & Grammar::getFirst() {
    return first;
}

/* getFollow 函数:返回 (变元, FOLLOW集) 键值对
 */
map<Symbol, FIRST_SET> & Grammar::getFollow() {
    return follow;
}

/* getEpsilon 函数: 返回 EPSILON
 */
Symbol Grammar::getEpsilon() {
    Symbol EPSILON = findTerminal("ε");
    return EPSILON;
}

/* getEndSymbol 函数: 返回源代码结束字符
 */
Symbol Grammar::getEndSymbol() {
    Symbol end_symbol;
    end_symbol.name = END_TER;
    end_symbol.no = END_TYPE;
    return end_symbol;
}


/* getEndSymbol 函数: 返回起始变元
 */
Symbol Grammar::getStartSymbol() {
    return findArgument(START_ARG);
}