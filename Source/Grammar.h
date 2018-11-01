// 文法
#include <string>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <iostream>

#ifndef SYMBOL
#define SYMBOL
#include "Symbol.h"
#endif

#ifndef PRODUCTION
#define PRODUCTION
#include "Production.h"
#endif

using namespace std;

//将 FIRST集 和 FOLLOW集 都定义为 set<Symbol> 类型
typedef set<Symbol> FIRST_SET; 
typedef set<Symbol> FOLLOW_SET;

//定义起始变元为 S
#define START_ARG "S"

//定义源码终结符及种别码
#define END_TER "#"
#define END_TYPE 0

/* Grammar 文法的类
 */
class Grammar {
private:
    bool has_got_grammar; //记录是否读取了文件
    int argument_amount; //变元数量
    int terminal_amount; //终结符数量
    int production_amount; //产生式数量
    set<Symbol> arguments; //变元集
    set<Symbol> terminals; //终结符集
    set<Production> productions; //产生式集
    map<Symbol, FIRST_SET> symbol_first; //(变元, FIRST集) 键值对
    map<Production, FIRST_SET> first; //(产生式, FIRST集) 键值对
    map<Symbol, FOLLOW_SET> follow; //(变元, FOLLOW集) 键值对

    /* readInArguments 函数: 读入文件中的变元
	 * 在函数中完成对 argument_amount 和 arguments 集合的修改
     * @param fin: 打开的文件对象
	 */
    void readInArguments(ifstream &fin);

    /* readInTerminals 函数: 读入文件中的终结符
	 * 在函数中完成对 terminal_amount 和 terminals 集合的修改
     * @param fin: 打开的文件对象
	 */
    void readInTerminals(ifstream &fin);

    /* readInProductions 函数: 读入文件中的产生式，并分拆 '|' 所隔开的产生式
     * 最终 productions 集合中的产生式右部不存在 '|' 字符 
	 * 在函数中完成对 production_amount 和 productions 集合的修改
     * @param fin: 打开的文件对象
	 */
    bool readInProductions(ifstream &fin);

    /* checkProductions 函数: 检查读入的文法是否正确
     * @return: 检查的结果
	 */
    bool checkProductions();

    /* generateSymbolFirst 函数: 根据产生式、变元和产生式，生成每个变元对应的 FIRST 集合
     * 结果保存在 symbol_first 键值对中
     * 即这个函数产生 FIRST(X) 的结果，X ∈ Vn
	 */
    void generateSymbolFirst();

    /* generateFirst 函数: 根据产生式、变元和产生式，生成每个产生式对应的 FIRST 集合
     * 结果保存在 first 键值对中
     * 即这个函数产生 FIRST(α) 的结果
	 */
    void generateFirst();

    /* generateFirst 函数: 根据产生式、变元和产生式，生成每个变元对应的 FOLLOW 集合
     * 结果保存在 follow 键值对中
	 */
    void generateFollow();

public:
    /* isTerminal 函数: 判断输入的字符串是否是一个终结符
     * @param input: 字符串
     * @return: true or false，表示是否为一个终结符
	 */
    bool isTerminal(string input);
    //重载
    bool isTerminal(Symbol input);

    /* isArgument 函数: 判断输入的字符串是否是一个变元
     * @param input: 字符串
     * @return: true or false，表示是否为一个变元
	 */
    bool isArgument(string input);
    //重载
    bool isArgument(Symbol input);

    /* findArgument 函数: 根据输入的字符串找到对应的 arguments 中对应的 Symbol 对象，并返回
     * @param input: 字符串
     * @return: Symbol，对应变元
	 */
    Symbol findArgument(string input);

    /* findTerminal 函数: 根据输入的字符串找到对应的 terminals 中对应的  Symbol 对象，并返回
     * @param input: 字符串
     * @return: 对应终结符
	 */
    Symbol findTerminal(string input);

    /* 构造函数: 初始化成员变量的值
	 */
    Grammar();

    /* setStartSymbol 函数: 设定起始变元
     * 默认为 S
     */
    void setStartSymbol(string name);

    /* 析构函数: 清空集合
	 */
    //~Grammar();

    /* analyzeGrammar 函数: 读入文件中变元、终结符和产生式
     * @param direcory: 文件地址
	 * @return: 返回 true 或 false，表示是否读取成功
	 */
    bool analyzeGrammar(string directory);

    /* hasAnalyzedGrammar 函数: 返回是否成功分析了语法
     * @return: true or false
     */
    bool hasAnalyzedGrammar();

    /* isLL1 函数: 判断是否为 LL1 文法
     * @return: true or false
     */
    bool isLL1();

    /* showArguments 函数: 展示读取到的变元
	 */
    void showArguments();

    /* showTerminals 函数: 展示读取到的终结符
	 */
    void showTerminals();

    /* showProductions 函数: 展示读取到的产生式
	 */
    void showProductions();

    /* showSymbolFirst 函数: 展示求得的变元的 FIRST 集合
	 */
    void showSymbolFirst();

    /* showFirst 函数: 展示求得的产生式的 FIRST 集合
	 */
    void showFirst();

    /* showFollow 函数: 展示求得的 FOLLOW 集合
	 */
    void showFollow();

    /* getArguments 函数：返回变元集
     */
    set<Symbol> & getArguments();

    /* getTerminals 函数：返回终结符集
     */
    set<Symbol> & getTerminals();

    /* getProductions 函数：返回产生式集
     */
    set<Production> & getProductions();

    /* getSymbolFirst 函数：返回 (变元, FIRST集) 键值对
     */
    map<Symbol, FIRST_SET> & getSymbolFirst();

    /* getFirst 函数：返回 (产生式, FIRST集) 键值对
     */
    map<Production, FIRST_SET> & getFirst();

    /* getFollow 函数：返回 (变元, FOLLOW集) 键值对
     */
    map<Symbol, FIRST_SET> & getFollow();

    /* getEpsilon 函数: 返回 EPSILON
     */
    Symbol getEpsilon();

    /* getEndSymbol 函数: 返回源代码结束字符
     */
    Symbol getEndSymbol();

    /* getEndSymbol 函数: 返回起始变元
     */
    Symbol getStartSymbol();
};
