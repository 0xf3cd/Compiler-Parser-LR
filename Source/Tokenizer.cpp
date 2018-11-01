// 词法分析器部分
#ifndef TOKENIZER
#define TOKENIZER
#include "Tokenizer.h"
#endif

/* 无参构造函数
 */
Tokenizer::Tokenizer() {
	line_count = 0;
	text = "";
	text_length = 0;
	text_count = 1;
	has_read_pound = false;
	has_mistake = false;
}

/* isDigit 函数: 判断输入字符是否为数字
 * @param input: 一个字符
 * @return: 返回判断结果
 */
bool Tokenizer::isDigit(char input) {
	if(input >= '0' && input <= '9') {
		return true;
	} else {
		return false;
	}
}

/* isLetter 函数: 判断输入字符是否为字母
 * @param input: 一个字符
 * @return: 返回判断结果
 */
bool Tokenizer::isLetter(char input) {
	if(input >= 'A' && input <= 'Z') {
		return true;
	} else if(input >= 'a' && input <= 'z') {
		return true;
	} else {
		return false;
	}
}

/* isTextMeaningless 函数: 判断当前行是否为空或是否只由空格、换行、回车或制表位构成
 * @return: 返回 true 或 false，表示是否为空
 */
bool Tokenizer::isTextMeaningless() {
	bool result = true;
	int i;
	for(i = 0; i < text_length; i++) {
		switch(text[i]) {
			case(' '): {
				break;
			}
			case('\t'): {
				break;
			}
			default: {
				result = false;
				break;
			}
		}
	}
	return result;
}

/* hasLineEnd 函数: 判断当前行是否读取完毕
 * @return: 返回 true 或 false，表示是否读取完毕
 */
bool Tokenizer::hasLineEnd() {
	if(text_count > text_length) {
		return true;
	} else {
		return false;
	}
}

/* readNewLine 函数: 读取下一行
 */
void Tokenizer::readNewLine() {
	text = FR.readLine();
	text_length = text.length();
	text_count = 1;
	line_count++;
}

/* readNextChar 函数: 读取下一个字符
 * @return: 返回下一个字符，如果全部字符都读完，则返回 '\0'
 */
char Tokenizer::readNextChar() {
	char next_char;

	if(hasLineEnd()) { //该行读取完毕
		if(FR.hasEnd()) {
			return '\0';
		}
		while(true) {
			readNewLine();
			if(FR.hasEnd()) {
				return '\0';
			}
			if(!isTextMeaningless()) { //保证下一行不为空
				break;
			}
		}
	}
	
	next_char = text[text_count - 1];
	text_count++;
	return next_char;
}

/* pushBackChar 函数: 放回一个字符
 * pushBackChar 不能翻滚回上一行
 */
void Tokenizer::pushBackChar() {
	if(text_count > 1) {
		text_count--;
	}
}

/* openSourceFile 函数: 打开源文件
 * @return: 是否打开成功
 */
bool Tokenizer::openSourceFile(string directory) {
	FR.open(directory);
	return FR.hasOpen();
}

/* hasOpen 函数: 判断源文件是否打开
 * @return: true 和 false 分别表示文件是否成功打开
 */
bool Tokenizer::hasOpen() {
	return FR.hasOpen();
}

/* hasReadPound 函数: 判断源文件是否读到了终止符 '#'
 * @return: true 和 false 分别表示文件是否读取完毕
 */
bool Tokenizer::hasReadPound() {
	return has_read_pound;
}

/* hasReadPound 函数: 判断源文件是否出错
 * @return: true 和 false 分别表示文件是否有错
 */
bool Tokenizer::hasMistake() {
	return has_mistake;
}

/* readNextWord 函数: 读取源文件中的一个 token(word)
 * @return: 返回一个 word 结构体对象，定义见 struct 结构体
 */
word Tokenizer::readNextWord() {
	word token;
	token.value = "";
	token.type = -1;

	if(has_read_pound) { //如果已经读取到了 '#'
		token.type = -1;
		return token;
	}

	if(has_mistake) {
		token.type = -4; //源文件出错
		return token;
	}

	char ch = readNextChar();
	token.line_num = line_count;
	// 消除空格、制表位等
	if(ch == ' ' || ch == '\t') {
		while(true) {
			ch = readNextChar();
			if(ch == ' ' || ch == '\t') {
				continue;
			} else {
				break;
			}
		}
	}

	if(ch == '\0') { //源文件结束
		token.type = -2;
	} else if(ch == '#') { // #
		has_read_pound = true;
		token.value += ch;
		token.type = 0;
	} else if(ch == '(') { // (
		token.value += ch;
		token.type = 22;
	} else if(ch == ')') { // )
		token.value += ch;
		token.type = 23;
	} else if(ch == '{') { // {
		token.value += ch;
		token.type = 24;
	} else if(ch == '}') { // }
		token.value += ch;
		token.type = 25;
	} else if(ch == '+') { // +
		token.value += ch;
		token.type = 10;
	} else if(ch == '-') { // -
		token.value += ch;
		token.type = 11;
	} else if(ch == ';') { // ;
		token.value += ch;
		token.type = 20;
	} else if(ch == ',') { // ,
		token.value += ch;
		token.type = 21;
	} else if(ch == '/') { // / or /* or //
		token.value += ch;

		if(hasLineEnd()) { //读到了某一行的结束
			token.type = 13; //即为除号
		}

		ch = readNextChar();
		if(ch == '/') { //单行注释
			token.value += ch;
			token.type = 26;

			while(!hasLineEnd()) { //读取完这一行所有字符，直至下一行非注释字符前停下
				readNextChar(); 
			}
		} else if(ch == '*') { //多行注释
			token.value += ch;
			token.type = 27;

			while(true) { //读取至 "*/" 前面停下
				ch = readNextChar();
				if(ch == '\0') { //源文件读取完毕
					break;
				} else if(ch == '*') {
					if(!hasLineEnd()) {
						ch = readNextChar();
						if(ch == '/') {
							//此时 * / 两个字符相邻出现，且在同一行
							//说明多行注释结束
							pushBackChar();
							pushBackChar(); //回退两个字符
							break;
						}
					}
				}
			}
		} else {
			token.type = 13; //即为除号
			pushBackChar();
		}
	} else if(ch == '*') { // * or */
		token.value += ch;

		if(hasLineEnd()) { //读到了某一行的结束
			token.type = 12; //即为乘号
		}

		ch = readNextChar();
		if(ch == '/') {
			token.value += ch;
			token.type = 28;
		} else {
			token.type = 12; //即为乘号
			pushBackChar();
		}
	} else if(ch == '=') { // = or ==
		token.value += ch;

		if(hasLineEnd()) { //读到了某一行的结束
			token.type = 9; //即为等号
		}

		ch = readNextChar();
		if(ch == '=') {
			token.value += ch;
			token.type = 14;
		} else {
			token.type = 9; //即为等号
			pushBackChar();
		}
	} else if(ch == '<') { // < or <=
		token.value += ch;

		if(hasLineEnd()) { //读到了某一行的结束
			token.type = 16; //即为小于号
		}

		ch = readNextChar();
		if(ch == '=') {
			token.value += ch;
			token.type = 17;
		} else {
			token.type = 16; //即为小于号
			pushBackChar();
		}
	} else if(ch == '>') { // > or >=
		token.value += ch;

		if(hasLineEnd()) { //读到了某一行的结束
			token.type = 18; //即为大于号
		}

		ch = readNextChar();
		if(ch == '=') {
			token.value += ch;
			token.type = 19;
		} else {
			token.type = 18; //即为大于号
			pushBackChar();
		}
	} else if(ch == '!') { // !=
		token.value += ch;

		if(hasLineEnd()) { //读到了某一行的结束
			has_mistake = true;
			token.type = -3; //出错
		}

		ch = readNextChar();
		if(ch == '=') {
			token.value += ch;
			token.type = 15;
		} else {
			has_mistake = true;
			token.type = -3; //出错
			pushBackChar();
		}	
	} else if(isDigit(ch)) { // 数
		token.value += ch;
		while(true) {
			if(hasLineEnd()) { //读到了某一行的结束
				token.type = 8; //即为数
				break;
			}

			ch = readNextChar();
			if(isDigit(ch)) {
				token.value += ch;
			} else if(isLetter(ch)) {
				token.value += ch;
				has_mistake = true;
				token.type = -3; //数字后紧接字母，出错
				pushBackChar();
				break;
			} else {
				token.type = 8;
				pushBackChar();
				break;
			}
		}
	} else if(isLetter(ch)) {
		token.value += ch;
		while(true) {
			if(hasLineEnd()) { //读到了某一行的结束
				token.type = 29; //即为 ID 或 keywords
				break;
			}

			ch = readNextChar();
			if(isDigit(ch) || isLetter(ch)) {
				token.value += ch;
			} else {
				token.type = 29;
				pushBackChar();
				break;
			}
		}
	} else {
		token.value += ch;
		token.type = -5; //出现不合法字符
	}

	if(token.type == 29) {
		if(token.value == "int") { token.type = 1; } 
		else if(token.value == "void") { token.type = 2; } 
		else if(token.value == "if") { token.type = 3; } 
		else if(token.value == "else") { token.type = 4; } 
		else if(token.value == "while") { token.type = 5; } 
		else if(token.value == "return") { token.type = 6; } 
		else { token.type = 7; }
	}

	
	token.end_char_num = text_count - 1;
	token.start_char_num = token.end_char_num - token.value.length() + 1;

	return token;
}