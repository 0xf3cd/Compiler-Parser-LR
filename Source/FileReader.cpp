// 文件读取部分
#ifndef FILE_READER
#define FILE_READER
#include "FileReader.h"
#endif

#include <iostream>

/* 无参构造函数
 */
FileReader::FileReader() {
	file_directory = "";
	has_open = false;
	has_end = false;
}

/* 析构函数: 关闭已经打开的文件
 */
FileReader::~FileReader() {
	if(has_open) {
		fin.close();
	}
}

/* open 函数: 打开某个文件
 * @param directory: 待打开文件的目录地址
 * @return: true 和 false 表明文件是否打开成功
 */
bool FileReader::open(string directory) {
	if(fin.is_open()) {
		fin.close();
	}

	file_directory = directory;
	fin.open(directory, ios::in);

	has_open = fin.is_open();
	return has_open;
}

/* hasOpen 函数: 返回文件的打开状态
 * @return: true 和 false 表示文件是否打开
 */
bool FileReader::hasOpen() {
	return has_open;
}

/* hasEnd 函数: 返回文件的读取状态
 * @return: true 和 false 表示文件是否读取完
 */
bool FileReader::hasEnd() {
	return has_end;
}

/* readNextLine 函数: 返回文件中的下一行
 * @return: 当文件读取完毕后，返回空字符串
 */
string FileReader::readLine() {
	string next_line = "";
	if(!has_open || has_end) {
		return next_line;
	}
	
	if(fin.eof()) {
		has_end = true;
		return next_line;
	}
	
	getline(fin, next_line);
	return next_line;
}