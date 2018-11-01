// 词法分析器部分
#ifndef FILE_READER
#define FILE_READER
#include "FileReader.h"
#endif

using namespace std;

/* 词法分析器每次读取一个单词，将其储存在 word 结构体中
 * value 为单词名称，如 4，int，i，abc，!= 等
 * type 为种别码字，0 - 28 为正常取值，
 				-1 表示之前已经读取到了 '#'，
 				-2 表示源文件已经读取完毕，
 				-3 表示这一次扫描过程中发现不合法的单词，
 				-4 表示之前的扫描过程中出现过不合法的单词，
 				-5 表示出现不合法的字符
 * line_num 表示单词出现在源文件中的行数
 * start_char_num 表示单词第一个字符在所在行中的位置
 */
struct word {
	string value;
	int type;
	int line_num;
	int start_char_num;
	int end_char_num;
};

class Tokenizer {
private:
	FileReader FR; //FireReader 对象
	int line_count; //行数计数器
	string text; //某一行的内容
	int text_length; //某一行的字符数
	int text_count; //某一行内字符计数器, 取值范围 [1, text_length]
	bool has_read_pound; //是否读到了 '#'
	bool has_mistake; //是否发现源代码错误

	/* isDigit 函数: 判断输入字符是否为数字
	 * @param input: 一个字符
	 * @return: 返回判断结果
	 */
	bool isDigit(char input);
	
	/* isLetter 函数: 判断输入字符是否为字母
	 * @param input: 一个字符
	 * @return: 返回判断结果
	 */
	bool isLetter(char input);

	/* isTextMeaningless 函数: 判断当前行是否为空或是否只由空格、换行、回车或制表位构成
	 * @return: 返回 true 或 false，表示是否为空
	 */
	bool isTextMeaningless();

	/* hasLineEnd 函数: 判断当前行是否读取完毕
	 * @return: 返回 true 或 false，表示是否读取完毕
	 */
	bool hasLineEnd();

	/* readNewLine 函数: 读取下一行
	 */
	void readNewLine();

	/* readNextChar 函数: 读取下一个字符
	 * @return: 返回下一个字符，如果全部字符都读完，则返回 '\0'
	 */
	char readNextChar();

	/* pushBackChar 函数: 放回一个字符
	 * pushBackChar 不能翻滚回上一行
	 */
	void pushBackChar();

public:
	/* 无参构造函数
	 */
	Tokenizer();

	/* openSourceFile 函数: 打开源文件
	 * @return: 是否打开成功
	 */
	bool openSourceFile(string directory);

	/* hasOpen 函数: 判断源文件是否打开
	 * @return: true 和 false 分别表示文件是否成功打开
	 */
	bool hasOpen();

	/* hasReadPound 函数: 判断源文件是否读到了终止符 #
	 * @return: true 和 false 分别表示文件是否读取完毕
	 */
	bool hasReadPound();

	/* hasReadPound 函数: 判断源文件是否出错
	 * @return: true 和 false 分别表示文件是否有错
	 */
	bool hasMistake();

	/* readNextWord 函数: 读取源文件中的一个 token(word)
	 * @return: 返回一个 word 结构体对象，定义见 struct 结构体
	 */
	word readNextWord();
};