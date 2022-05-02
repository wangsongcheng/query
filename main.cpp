#if _MSC_VER >= 1800
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <regex>
#include <time.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux
#include <sys/stat.h>
#include <dirent.h>
#endif
#ifdef WIN32
#include <io.h>
#include <tchar.h>
#include <windows.h>
#endif
#include <iostream>
#include <vector>
#ifdef __linux
#define MAXBYTE 0xff
#define MAX_PATH 360
#define DEFAULT_INCLUDE_PATH_USR_INCLUDE "/usr/include/"
#define DEFAULT_INCLUDE_PATH_USR_LOCAL_INCLUDE "/usr/local/include/"
#define DEFAULT_INCLUDE_PATH_USR_LIB "/usr/lib/"
#endif
#define FUNCTION_OPTION "-f"
#define MACRO_OPTION "-m"
#define STRUCTURE_OPTION  "-s"
#define TYPEDEF_OPTION "-t"
#define UNION_OPTION "-u"
#define ENUM_OPTION "-e"
#define CLASS_OPTION "-c"
#define NAMESPACE_OPTION "-ns"
#define ALL_OPTION "-a"
#define EXACT_MATCH_OPTION "-em"
// #define NO_SEARCH_FILE_OPTION "-nsf"
// #define NO_SEARCH_PATH_OPTION "-nsd"
#define PATH_OPTION "-d"
// #define FILE_OPTION "-sf"
enum Search_Type{
	INVALID_VAL = -1,
	searchFun = 0,
	searchMacro,
	searchStruct,
	searchTypeDefine,
	searchUnion,
	searchEnum,
	searchClass,
	searchNameSpace,
	searchAll
};
struct search_infor{
	std::string spath;//search path
	std::vector<std::string> sfname;//search file name
};
bool g_ExactMatch;
//bool g_bFun;
int getfilelen(FILE *fp);
void getdir(const char *root, std::vector<search_infor>&path);
bool get_val_in_line(int argc, char *argv[], const std::string&lpsstr, std::string&lpstr, int32_t start = 1);
int get_index_in_line(int argc, char *argv[], const std::string&str, int32_t start = 1);
void get_option_val(int argc, char *argv[], const std::string&opt, std::vector<std::string>&out);
int linage(const std::string&content);
void help();
// bool isCommit(const char *pStr, int len);
bool isExacgMatch(const std::string&str, const std::string&searchName);
bool isOption(int argc, char *argv[], int index);
// bool isFun(const char *lpstr, int str_size, const char *fun_name);
bool isInvalid(int argc, char *argv[]);
const char *movepointer(const char *p, char ch, bool bfront);
void remove_comment(char *content);
void removeSame(const std::vector<std::string>&in, std::vector<std::string>&out);
void removeArgv(int32_t&argc, char *argv[], uint32_t index);
// void removeSame(const std::vector<std::string>&in, std::vector<search_infor>&out);
void search(const std::string&cPath, const std::string&filename, const std::string&lpstr, void(*fun)(const std::string&content, const std::string&lpstr, std::vector<std::string>&str));
void search_fun(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
void search_macro(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
void search_struct(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
void search_type_define(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
#ifdef WIN32
void SetTextColor(WORD color);
#endif
std::string searchStructString = "struct";
const std::vector<std::string> g_Option = { FUNCTION_OPTION, MACRO_OPTION,  STRUCTURE_OPTION, TYPEDEF_OPTION, UNION_OPTION, ENUM_OPTION, CLASS_OPTION, NAMESPACE_OPTION, ALL_OPTION,  PATH_OPTION };
//char *strrpc(char *str,char *oldstr,char *newstr);
void removeSameFile(const std::vector<std::string>&in, std::vector<search_infor>&out){
	for (size_t i = 0; i < out.size(); ++i){
		removeSame(in, out[i].sfname);
	}
}
void removeSamePath(const std::vector<std::string>&in, std::vector<search_infor>&out){
	for (size_t i = 0; i < in.size(); ++i){
		for (size_t j = 0; j < out.size(); ++j){
			if(in[i] == out[j].spath){
				out.erase(out.begin() + j);
			}	
		}
	}
}
int main(int32_t argc, char *argv[], char *envp[]){//envp环境变量表
	double getdir_totaltime = 0.0f, search_totaltime = 0.0f;
	clock_t getdir_start, getdir_finish, search_file_start, search_file_finish;//time count
	
	// const std::vector<std::string> option = { FUNCTION_OPTION, MACRO_OPTION,  STRUCTURE_OPTION, TYPEDEF_OPTION, UNION_OPTION, ENUM_OPTION, CLASS_OPTION, NAMESPACE_OPTION };//, "-d", "-n"
	if(isInvalid(argc, argv)){
		printf("parameter insufficient:\n");
		help();
		return -1;
	}
	
	std::vector<std::string> rootPath;
	// std::vector<std::string> searchFile;
	// std::vector<std::string> noSearchPath;
	// std::vector<std::string> noSearchFile;// = { "vulkan_core.h" };
	/*
		目前的程序，不希望用户通过-n选项指定其他路径的文件。需要指定其他路径，必须通过-d+-sf选项
		*.h

		如果用户指定了路径和文件名。但希望搜索该路径下所有文件以及搜索指定的文件名？
		目前的程序都是直接将路径和指定的文件名绑定
	*/
	// get_option_val(argc, argv, FILE_OPTION, searchFile);
	// get_option_val(argc, argv, NO_SEARCH_PATH_OPTION, noSearchPath);
	// get_option_val(argc, argv, NO_SEARCH_FILE_OPTION, noSearchFile);
    int32_t em_Option = get_index_in_line(argc, argv, EXACT_MATCH_OPTION);
    if(INVALID_VAL != em_Option){
        g_ExactMatch = true;
        removeArgv(argc, argv, em_Option);
    }
	//上面获取的有可能是正则表达式,
	get_option_val(argc, argv, PATH_OPTION, rootPath);

	if(rootPath.empty()){//用户未指定目录就从默认的目录查找
#ifdef __linux
		rootPath.push_back(DEFAULT_INCLUDE_PATH_USR_INCLUDE);
		rootPath.push_back(DEFAULT_INCLUDE_PATH_USR_LIB);
		rootPath.push_back(DEFAULT_INCLUDE_PATH_USR_LOCAL_INCLUDE);
#endif
#ifdef WIN32
		const char *ePath = getenv("include");
		if (ePath) {
			rootPath.push_back(ePath);
		}
		else{
			printf("not found environment variable:include, please specify include path or other path\n");
			exit(-1);
		}
#endif
	}
	// removeSame(noSearchFile, searchFile);
	void (*fun[])(const std::string&content, const std::string&lpstr, std::vector<std::string>&str) = {
		search_fun,
		search_macro,
		search_struct,//class、enum、namespace
		search_type_define,
	};
	uint32_t total_file = 0;
	//支持查询多个相同选项：-f strcpy printf -m MAXBYTE -m A
	for (size_t i = 0; i < g_Option.size() - 1; ++i){
		//判断需要查询的类型
		std::vector<std::string> findStr;
		get_option_val(argc, argv, g_Option[i], findStr);
		if(findStr.empty())continue;
		//后面真正用来搜索的路径
		std::vector<search_infor>searchPath;//需要搜索指定的所有根目录的数量//一个就能代表一个文件夹以及里面所有文件
		//支持多个目录、的多个文件//-a表示找所有类型。多执行几次不同类型的search函数即可
		// if(searchFile.empty()){
		//没有指定文件名称，需要查找目录下的所有文件名
		getdir_start = clock();
		for (size_t uiRootPath = 0; uiRootPath < rootPath.size(); ++uiRootPath){
			getdir(rootPath[uiRootPath].c_str(), searchPath);
		}
		getdir_finish = clock();
		getdir_totaltime = (double)(getdir_finish - getdir_start) / CLOCKS_PER_SEC;
		// }
		// else{
		// 	//如果指定了文件名。就搜集起来
		// 	for (size_t uiRootPath = 0; uiRootPath < rootPath.size(); ++uiRootPath){
		// 		for (size_t uiSearchFile = 0; uiSearchFile < searchFile.size(); ++uiSearchFile){
		// 			searchPath[uiRootPath].sfname.push_back(searchFile[uiSearchFile]);
		// 		}
		// 	}
		// }
		search_file_start = clock();
		for (size_t uiFindStr = 0; uiFindStr < findStr.size(); ++uiFindStr){
			for (size_t uiSearchPath = 0; uiSearchPath < searchPath.size(); ++uiSearchPath){
				search_infor&dir = searchPath[uiSearchPath];
				if(g_Option[i] != ALL_OPTION){
					Search_Type funIndex = (Search_Type)i;
					if(funIndex > searchTypeDefine ){
						const char *s[] = { "union", "enum", "class", "namespace" };
						searchStructString = s[funIndex - 4];
						funIndex = searchStruct;//------note:
					}
					for (size_t uiDir = 0; uiDir < dir.sfname.size(); ++uiDir){
						search(dir.spath, dir.sfname[uiDir], findStr[uiFindStr], fun[funIndex]);
					}
				}
				else{
					for (size_t uiDir = 0; uiDir < dir.sfname.size(); ++uiDir){
						// for (size_t uiFun = 0; uiFun < sizeof(fun) / sizeof(int *); ++uiFun){
						// 	if(uiFun > searchTypeDefine){
						// 		const char *s[] = { "union", "enum", "class", "namespace" };
						// 		searchStructString = s[uiFun - 4];
						// 	}
						// 	search(dir.spath, dir.sfname[uiDir], findStr[uiFindStr], fun[uiFun]);
						// }
						for (size_t uiFun = 0; uiFun < sizeof(fun) / sizeof(int *) + 4; ++uiFun){
							if(uiFun > searchTypeDefine){
								const char *s[] = { "union", "enum", "class", "namespace" };
								searchStructString = s[uiFun - 4];
							}
							else{
								searchStructString = "struct";
							}
							search(dir.spath, dir.sfname[uiDir], findStr[uiFindStr], fun[uiFun > searchTypeDefine ? uiFun - 4 : uiFun]);
						}
					}					
				}
				total_file += dir.sfname.size();
			}
		}
	}
	search_file_finish = clock();
	search_totaltime = (double)(search_file_finish - search_file_start) / CLOCKS_PER_SEC;
	std::cout << "-----------------------------" << std::endl;
	std::cout << "search path:";
	for (size_t i = 0; i < rootPath.size(); ++i){
		std::cout << rootPath[i] << "\t";
	}
	std::cout << "\n";
	std::cout << "total file:" << total_file << ";get directory time:" << getdir_totaltime << ";search file time:" << search_totaltime << std::endl;
	return 0;
}
void removeSame(const std::vector<std::string>&in, std::vector<std::string>&out){
	for (size_t j = 0; j < in.size(); ++j){
		for (size_t i = 0; i < out.size(); ++i){
			if(out[i] == in[j]){
				out.erase(out.begin() + i);
				break;
			}
		}		
	}
}
void removeArgv(int32_t&argc, char *argv[], uint32_t index){
    for (size_t i = index + 1; i < argc; ++i){
        argv[index] = argv[i];
    }
    --argc;
}
/*{{{*/
void getdir(const char *root, std::vector<search_infor>&path){
	search_infor infor;
	infor.spath = root;
#ifdef __linux
	DIR *d;
	dirent *file = NULL;
	if(!(d = opendir(root))){
		perror("opendir error");
		printf("path is %s\n", root);
		return;
	}
	while((file = readdir(d))){
		if(strcmp(file->d_name, ".") && strcmp(file->d_name, "..")){
			if(file->d_type == DT_DIR){
				char szPath[MAX_PATH] = {0};
				if('/' != infor.spath[infor.spath.length() - 1])
					sprintf(szPath, "%s/%s", infor.spath.c_str(), file->d_name);
				else
					sprintf(szPath, "%s%s", infor.spath.c_str(), file->d_name);
				getdir(szPath, path);
			}
			else if(file->d_type == DT_REG){
				infor.sfname.push_back(file->d_name);
			}
		}
	}
	closedir(d);
#endif
#ifdef WIN32
	long fHandle = 0;
	struct _tfinddata_t fa = { 0 };
	TCHAR cPath[MAX_PATH] = { 0 };
	if('\\' == root[strlen(root) - 1])
		_stprintf(cPath, _T("%s*.*"), root);
	else
		_stprintf(cPath, _T("%s\\*.*"), root);
	fHandle = _tfindfirst(cPath, &fa);
	if (-1 == fHandle) {
		perror(_T("findfirst error"));
		printf(_T("path is %s\n"), cPath);
		return;
	}
	else {
		do {
			if (_A_SUBDIR == fa.attrib) {
				//目录
				if (_tcscmp(fa.name, _T(".")) && _tcscmp(fa.name, _T(".."))) {
					if('*' != cPath[strlen(cPath) - 1])
						_stprintf(cPath, _T("%s\\%s"),cPath, fa.name);
					else
						_stprintf(cPath, _T("%.*s\\%s"),strlen(cPath) - strlen("\\*.*"), cPath, fa.name);
					getdir(cPath, path);
					memset(&cPath[strlen(cPath) - strlen(fa.name) - 1], 0, strlen(cPath) - strlen(fa.name));
				}
			}
			else{
				infor.sfname.push_back(fa.name);
			}
		} while (!_tfindnext(fHandle, &fa));
	}
#endif
	path.push_back(infor);
}
/*}}}*/
/*{{{*/
// bool isCommit(const char *pStr, int len){
// 	if(!memchr(pStr, '/', len) && !memchr(pStr, '*', len))return false;
// 	for(int i = 0; i < len - 1; ++i){
// 		if(isalpha(pStr[i]))break;
// 		if(pStr[i] == '*' || (pStr[i] == '/' && (pStr[i + 1] == '*' || pStr[i + 1] == '/')))return true;
// 	}
// 	return false;
// }
/*}}}*/
uint32_t GetFileContent(const std::string&file, char *content){
	FILE *fp = fopen(file.c_str(), "rb");
	if(!fp){
		perror("read file error");
		printf("path is %s\n", file.c_str());
		return 0;
	}
	int size = getfilelen(fp);
	if(content){
		fread(content, size, 1, fp);		
	}
	fclose(fp);
	return size;
}
void search(const std::string&cPath, const std::string&filename, const std::string&lpstr, void(*fun)(const std::string&content, const std::string&lpstr, std::vector<std::string>&str)){
	std::string szPath;
	char *content;
	if('/' != cPath[cPath.length() - 1])
		szPath = cPath + "/" + filename;
	else
		szPath = cPath + filename;
	uint32_t size = GetFileContent(szPath, nullptr);
	if(size < 1)return;
	content = new char[size + 1];
	GetFileContent(szPath, content);
	content[size] = 0;
	remove_comment(content);
	std::vector<std::string>str;
	fun(content, lpstr, str);
	int line;
	const char *lpStart = 0;//show line
	if(!str.empty()){
#ifdef __linux
		printf("\e[32m%s\e[0m\n", szPath.c_str());
#endif
#ifdef WIN32
		SetTextColor(FOREGROUND_GREEN);
		printf("%s\n", szPath);
		SetTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
	}
	int find_str_len = lpstr.length();
	int contentLineCount = linage(content);
	for(int i = 0; i < str.size(); i++){
		// if(!isCommit(str[i].c_str(), str[i].length())){//都已经移除注释内容了为什么还要检查？
		lpStart = strstr(content, str[i].c_str());
		line = contentLineCount - linage(lpStart) + 1;
#ifdef __linux
		str[i].insert(str[i].find(lpstr), "\e[31m");
		str[i].insert(str[i].find(lpstr) + find_str_len, "\e[0m");
#endif
		printf("%d:%s\n",line, str[i].c_str());
		// }
	}
	delete[]content;
}
bool isFun(const std::string&str, const char *fun_name){
	bool bIsFun = false;
	/*
		函数:返回类型 函数名(零个或多个参数,可能带回车符);
		void (*(*fun(...))(...))(...) ;


		//---------
		函数参数必须是带类型的，这里没有判断
	*/
	size_t funNameSize = strlen(fun_name);
	size_t funNamePos = str.find(fun_name);
	size_t c = str.find('(', funNamePos + funNameSize);
	size_t _c = str.find(')', funNamePos + funNameSize);
	const std::string invalidStr[] = {
		"if(", "if (", "return", "|", "!", "&&", "#", "->", "typedef",
		":", "{", "}", "$", "@", "%", "^", "/", "\\", "%", "//", "\"",
        "/*", "*/", "+", "-", "[", "]"
	};
	for(int32_t i = 0; i < sizeof(invalidStr) / sizeof(std::string); ++i)
		if(std::string::npos != str.find(invalidStr[i]))
			return false;
	//排除连基本的函数声明特征都没有的字符串(没有指定的函数名没有一对括号)
	if(std::string::npos != funNamePos && std::string::npos != c && str.find('(') > funNamePos && std::string::npos != _c){
		if(str[c + 1] != ')' && std::string::npos == str.find(" "))return false;
		//排除不可能出现在函数声明的字符
		size_t comma = str.find(',');
		size_t point = str.find('.');
		if((comma == std::string::npos || c < comma) && (std::string::npos == point || str[point + 1] == '.')){
			//等号有可能是默认参数。必须额外判断
			size_t p = str.find('=');
			if(std::string::npos == p || (p > c && p < _c)){//找到的括号一定在查找的字符串后面
				for (size_t i = 0; i < funNamePos - 1; ++i){//看看前面是否有返回类型
					if(str[i] != ' ' && str[i] != '\t'){
						bIsFun = true;
						break;
					}
				}
			}
		}
	}
	return bIsFun;
}
//--前 ++后
/*{{{*/
const char *movepointer(const char *p, char ch, bool bfront){
	if (!p)return nullptr;
	if (bfront) {
		while (*p && *p != ch && p--);
	}
	else {
		p = strchr(p, ch);
	}
	return p;
}
/*}}}*/
// #define USE_REGEXP 
void search_fun(const std::string&content, const std::string&lpstr, std::vector<std::string>&str){
	const char *lpStart = content.c_str(), *lpEnd = strstr(content.c_str(), lpstr.c_str()), *lpTemp = lpStart;
    if(!lpEnd)return;
#define FUNCTION_MAX_CHA 500
#ifdef USE_REGEXP
	 std::smatch result;
	 char lpReg[100] = {0};
	// sprintf(lpReg, "[^#/*].*[*&]? [*&]?%s.*[ ]?(.*)?[\n]?.*[;]?[\r]?", lpstr.c_str());
	sprintf(lpReg, "[^#/*].*[*&]? [*&]?%s.*[ ]?(.*[\n]?.*[\n]?.*[\n]?.*[\n]?.*[\n]?.*)[\n]?.*;", lpstr.c_str());//可能存在正则表达式匹配死循环的情况
	std::regex reg(lpReg);
    do{
        while(lpTemp < lpEnd){
            lpStart = ++lpTemp;
            lpTemp = strchr(lpStart, '\n');
        }
        uint32_t len = strcspn(lpStart, ";");
        if(len <= FUNCTION_MAX_CHA){//函数声明一般都不会太多字符。目前先假设最多只有这样
            std::string regexStr(lpStart, len + 1);
            if(regex_match(regexStr, result, reg)){
               if(isFun(regexStr, lpstr.c_str())){
                    std::string _str(lpStart, len + 1);
                    str.push_back(_str);
                }
            }
        }
        lpStart += len + 1;
    } while (lpEnd = strstr(lpStart, lpstr.c_str()));
#else
    do{
        while(lpTemp && lpTemp < lpEnd){
            lpStart = ++lpTemp;
            lpTemp = strchr(lpStart, '\n');
        }
        uint32_t len = strcspn(lpStart, ";");
        if(len <= FUNCTION_MAX_CHA){//函数声明一般都不会太多字符。目前先假设最多只有这样
            std::string fun(lpStart, len + 1);
            if(isFun(fun, lpstr.c_str())){
                if(!g_ExactMatch || isExacgMatch(fun, lpstr)){
                    std::string _str(lpStart, len + 1);
                    str.push_back(_str);
                }
            }
        }
        lpStart += len + 1;
    } while (lpEnd = strstr(lpStart, lpstr.c_str()));
#endif
#undef FUNCTION_MAX_CHA
}
/*{{{*/
void search_macro(const std::string&content, const std::string&lpstr, std::vector<std::string>&str){
	const char *lpStart = content.c_str();
	char buffer[MAXBYTE] = {0};
	sprintf(buffer, "#define %s", lpstr.c_str());
	while((lpStart = strstr(lpStart, buffer))){
        const char *lpEnd = lpStart;
        // const char *lpTemp = lpEnd;
		while((lpEnd = strchr(lpEnd, '\n')) && (*(lpEnd - 1) == '\\' || *(lpEnd - 2) == '\\')){
            ++lpEnd;
		}
		std::string buff(lpStart, lpEnd - lpStart);
        if(!g_ExactMatch || isExacgMatch(buff, lpstr)){
		    str.push_back(buff);
        }
		lpStart += lpstr.length() + strlen("#define ");
	}
}
/*}}}*/
void search_struct(const std::string&content, const std::string&lpstr, std::vector<std::string>&str){
//just search struct name;no search struct alias name
	// int count = content.length();
	const char *lpEnd = nullptr;
	const char *lpStart = content.c_str();
    //结构体前面带typedef.后面才能带别名
	char buffer[MAXBYTE] = {0};
    //这里假设该结构体非匿名结构体
	sprintf(buffer, "%s %s", searchStructString.c_str(), lpstr.c_str());
	while((lpStart = strstr(lpStart, buffer))){
		lpStart -= strlen("typedef ");
		if(memcmp(lpStart, "typedef ", strlen("typedef")))lpStart += strlen("typedef ");
		lpEnd = strchr(lpStart, '\n');
		if(lpEnd && memchr(lpStart, '{', strcspn(lpStart, "\n")) && ';' != *(lpEnd - 1)){
			uint32_t count = 1;
			lpEnd = strchr(lpStart, '{');
			do{
				++lpEnd;
				if(!lpEnd)break;
				if(*lpEnd == '{')count++;
				if(*lpEnd == '}')count--;
			}while(count);
			lpEnd = strchr(lpEnd, '\n');
		}
		if(lpEnd){
            std::string buff(lpStart, lpEnd - lpStart);
            if(!g_ExactMatch || isExacgMatch(buff, lpstr)){
                str.push_back(buff);
            }
		}
		lpStart += lpstr.length() + searchStructString.length() + strlen("typedef");
	}
}
void search_type_define(const std::string&content, const std::string&lpstr, std::vector<std::string>&str){
	const char *lpStart = content.c_str();
	char buffer[MAXBYTE] = {0};
	sprintf(buffer, "typedef %s", lpstr.c_str());
	while((lpStart = strstr(lpStart, buffer))){
		const char *lpEnd = strchr(lpStart, '\n');
		if(lpEnd){
			std::string buff(lpStart, lpEnd - lpStart);
            if(!g_ExactMatch || isExacgMatch(buff, lpstr)){
			    str.push_back(buff);
            }
		}
		lpStart += lpstr.length() + strlen("typedef ");
	}
}
#ifdef WIN32
void SetTextColor(WORD color){
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hout, color);
}
#endif
bool get_val_in_line(int argc, char *argv[], const std::string&lpsstr, std::string&lpstr, int32_t start){
	int index = get_index_in_line(argc, argv, lpsstr, start);
	if(INVALID_VAL != index && argv[index + 1]){
		lpstr = argv[index + 1];
		return true;
	}
	return false;
}
void help(){
	printf("format:option string [string] [option][string]...\n");
	printf("example:query -f strcpy strcat -s VkDeviceC -f vkCmdDraw -s VkDeviceCreateInfo -sf string.h vulkan_core.h\n");
	printf("option:\n");
	// printf("%s表示不在该路及内搜索\n", NO_SEARCH_PATH_OPTION);
	// printf("%s表示不在该文件内搜索\n", NO_SEARCH_FILE_OPTION);
    printf("\t'%s'表示必须完全匹配名称\n", EXACT_MATCH_OPTION);
	printf("\t'%s'表示搜索所有类型\n", ALL_OPTION);
	printf("\t'%s' indicate search enum\n", ENUM_OPTION);
	printf("\t'%s' indicate search union\n", UNION_OPTION);
	printf("\t'%s' indicate search class\n", CLASS_OPTION);
	printf("\t'%s' indicate search macro\n", MACRO_OPTION);
	printf("\t'%s' indicate search function\n", FUNCTION_OPTION);
	printf("\t'%s' indicate search structure\n", STRUCTURE_OPTION);
	printf("\t'%s' indicate search directory\n", PATH_OPTION);
	printf("\t'%s' indicate search namespace\n", NAMESPACE_OPTION);
	printf("\t'%s' indicate search type define\n", TYPEDEF_OPTION);
	// printf("\t'%s' indicate search in that file;\n", FILE_OPTION);
}
bool isInvalid(int argc, char *argv[]){
	return argc < 3;
	// int ioption = 0;
	// int optCount = 0;
	// if(argc < 3)return true;
	// for(int j = 0; j < argc; j++){
	// 	if(INVALID_VAL == get_index_in_line(argc, argv, option[j]))++optCount;
	// 	for(int i = 1; i < argc; i++){
	// 		if(option[j] == argv[i]){
	// 			ioption++;
	// 			break;
	// 		}
	// 	}
	// }
	// //说明不是只有传一个 选项
	// return (ioption == argc - 1) || optCount == option.size();
}
int get_index_in_line(int argc, char *argv[], const std::string&str, int32_t start){
	for(int i = start; i < argc; i++){
		if(argv[i] == str){
			return i;
		}
	}
	return INVALID_VAL;
}
int getfilelen(FILE *fp){
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return size;
}
void remove_comment(char *content){
	char *lpStart = content;
	while((lpStart = strstr(lpStart, "//"))){
		const char *lpEnd = strstr(lpStart, "\n");
		if(lpEnd){
			strcpy(lpStart, lpEnd);
		}
		else{
			++lpStart;
		}
	}
	lpStart = content;
	while((lpStart = strstr(lpStart, "/*"))){
		const char *lpEnd = strstr(lpStart, "*/");
		if(lpEnd){
			lpEnd += 2;
			strcpy(lpStart, lpEnd);
		}
		else{
			++lpStart;
		}
	}
}
int linage(const std::string&content){
	int line = 1;
	const char *lpStart = content.c_str();
	while((lpStart = strchr(lpStart, '\n')) && ++line && ++lpStart);
	return line;
}
/*
char *strrpc(char *str,char *oldstr,char *newstr){
	int len = strlen(str);
	char *bstr = new char[len + 1];//转换缓冲区
	memset(bstr,0, len + 1);
	for(int i = 0;i < strlen(str);i++){
		if(!strncmp(str+i,oldstr,strlen(oldstr))){//查找目标字符串
			strcat(bstr,newstr);
			i += strlen(oldstr) - 1;
		}else{
			strncat(bstr,str + i,1);//保存一字节进缓冲区
	    }
    	}
	strcpy(str,bstr);
	delete[]bstr;
	return str;
}
*/
bool isExacgMatch(const std::string&str, const std::string&searchName){
    size_t pos = str.find(searchName);
    if(pos != std::string::npos && pos > 1){
        if((str[pos - 1] == ' ' || str[pos - 1] == '*' || str[pos - 1] == '&') && (str[pos + searchName.length()] == ' ' || str[pos + searchName.length()] == '(' || str[pos + searchName.length()] == '{'))return true;
    }
    return false;
}
bool isOption(int argc, char *argv[], int index){
	for(int i = 0; i < g_Option.size(); ++i){
		if(argv[index] && argv[index] == g_Option[i]){
			return true;
		}
	}
	return false;
}
void get_option_val(int argc, char *argv[], const std::string&opt, std::vector<std::string>&out){
	int offset = 1;
	int index = INVALID_VAL;
	while(INVALID_VAL != (index = get_index_in_line(argc, argv, opt, offset))){
		++index;
		while(argv[index] && !isOption(argc, argv, index)){
			out.push_back(argv[index]);
			++index;
		}
		offset = index;
	}
}
