// 文件读取部分
#include <fstream>
#include <string>
using namespace std;

class FileReader {
private:
	string file_directory;
	ifstream fin;
	bool has_open;
	bool has_end;
public:
	/* 无参构造函数
	 */
	FileReader();

	/* 析构函数: 关闭已经打开的文件
 	 */
	~FileReader();

	/* open 函数: 打开某个文件
	 * @param directory: 待打开文件的目录地址
	 * @return: true 和 false 表明文件是否打开成功
	 */
	bool open(string directory);

	/* hasOpen 函数: 返回文件的打开状态
	 * @return: true 和 false 表示文件是否打开
	 */
	bool hasOpen();

	/* hasEnd 函数: 返回文件的读取状态
	 * @return: true 和 false 表示文件是否读取完
	 */
	bool hasEnd();

	/* readNextLine 函数: 返回文件中的下一行
	 * @return: 当文件读取完毕后，返回空字符串
	 */
	string readLine();
};