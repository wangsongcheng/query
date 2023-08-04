#if _MSC_VER >= 1800
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <regex>
#include <time.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>
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
// #include <stdarg.h>
#ifdef __linux
#define MAXBYTE 0xff
#define MAX_PATH 360
#define DEFAULT_INCLUDE_PATH_USR_INCLUDE "/usr/include/"
#define DEFAULT_INCLUDE_PATH_USR_LOCAL_INCLUDE "/usr/local/include/"
// #define DEFAULT_INCLUDE_PATH_USR_LIB "/usr/lib/"
#endif
#define INVALID_VAL -1
#define FUNCTION_OPTION "f"
#define MACRO_OPTION "m"
#define STRUCTURE_OPTION  "s"
#define TYPEDEF_OPTION "t"
#define UNION_OPTION "u"
#define ENUM_OPTION "e"
#define CLASS_OPTION "c"
#define NAMESPACE_OPTION "n"
// #define PATH_OPTION "p"
#define ALL_OPTION "a"
// #define EXACT_MATCH_OPTION "-em"
// #define CASE_INSENSITIVE_OPTION "-ci"
// #define NO_SEARCH_FILE_OPTION "-nsf"
// #define NO_SEARCH_PATH_OPTION "-nsd"
// #define PATH_OPTION "-d"
// #define FILE_OPTION "-sf"
enum Search_Type{
	searchFun = 0,
	searchMacro,
	searchStruct,
	searchUnion,
	searchEnum,
	searchClass,
	searchNameSpace,
	searchTypeDefine
};
// enum Search_Type{
// 	INVALID_VAL = -1,
// 	searchFun = 0,
// 	searchMacro,
// 	searchStruct,
// 	searchTypeDefine,
// 	searchUnion,
// 	searchEnum,
// 	searchClass,
// 	searchNameSpace,
// 	searchAll
// };
// //设置的数值，二进制上必须只有1个1
// enum Search_Option_Bit{
//     SO_NONE_BIT = 0,
//     SO_EXACT_MATCH_BIT,
//     // SO_CASE_INSENSITIVE_BIT = 2
// };
// typedef uint32_t SearchOptionBit;
struct search_infor{
	std::string spath;//search path
	std::vector<std::string> sfname;//search file name
};
// bool g_ExactMatch;
//bool g_bFun;
void ConvertCase(char *content, uint32_t len, bool tolower = true);
int getfilelen(FILE *fp);
void getdir(const char *root, std::vector<search_infor>&path);
bool get_val_in_line(int argc, char *argv[], const std::string&lpsstr, std::string&lpstr, int32_t start = 1);
int get_index_in_line(int argc, char *argv[], const std::string&str, int32_t start = 1);
void get_option_val(int argc, char *argv[], const std::string&opt, std::vector<std::string>&out, int32_t start = 1);
int linage(const std::string&content);
void help(int32_t argc, char *argv[]);
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
void search_structure(const std::string&content, const std::string&lpstr, std::vector<std::string>&str, const std::string&typeName);
void search(const std::string&cPath, const std::string&filename, const std::string&lpstr, void(*fun)(const std::string&content, const std::string&lpstr, std::vector<std::string>&str));
void search(const std::vector<std::string>&findStr, const std::vector<search_infor>&searchPath, void (*searchfun)(const std::string&, const std::string&, std::vector<std::string>&));
void search_fun(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
void search_macro(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
void search_class(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
void search_enum(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
void search_union(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
void search_struct(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
void search_namespace(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
void search_type_define(const std::string&content, const std::string&lpstr, std::vector<std::string>&str);
// void search(const std::string&cPath, const std::string&filename, const std::string&lpstr, void(*fun)(const std::string&content, const std::string&lpstr, std::vector<std::string>&str, SearchOptionBit option), SearchOptionBit option);
// void search_fun(const std::string&content, const std::string&lpstr, std::vector<std::string>&str, SearchOptionBit option);
// void search_macro(const std::string&content, const std::string&lpstr, std::vector<std::string>&str, SearchOptionBit option);
// void search_struct(const std::string&content, const std::string&lpstr, std::vector<std::string>&str, SearchOptionBit option);
// void search_type_define(const std::string&content, const std::string&lpstr, std::vector<std::string>&str, SearchOptionBit option);
#ifdef WIN32
void SetTextColor(WORD color);
#endif
// std::string searchStructString = "struct";
//char *strrpc(char *str,char *oldstr,char *newstr);
bool isPath(const char *str){
    return strchr(str, '/') || strchr(str, '\\');
}
// void removeSameFile(const std::vector<std::string>&in, std::vector<search_infor>&out){
// 	for (size_t i = 0; i < out.size(); ++i){
// 		removeSame(in, out[i].sfname);
// 	}
// }
void removePath(std::vector<std::string>&inAndOut){
	for (size_t i = 0; i < inAndOut.size(); ++i){
        if(isPath(inAndOut[i].c_str())){
            inAndOut.erase(inAndOut.begin() + i);
        }
	}
}
void getRootPath(int32_t argc, char *argv[], std::vector<std::string>&out){
    for (size_t i = 1; i < argc; ++i){
        if(isPath(argv[i])){
            out.push_back(argv[i]);
        }
    }
}
bool isDefaultOption(int32_t argc, char *argv[]){
    bool bDefaultOption = false;
    if(argc > 1){
        if(argc == 2)return true;
        bDefaultOption = argv[1][0] != '-';
    }
    return bDefaultOption;
}
bool havePath(const std::vector<std::string>&in){
    for (size_t i = 0; i < in.size(); ++i){
        if(isPath(in[i].c_str())){
            return true;
        }
    }
    return false;
}
void get_option(int32_t argc, char *argv[], std::vector<std::string>&out){
	for (size_t i = 0; i < argc; ++i){
		if(isOption(argc, argv, i)){
			out.push_back(argv[i]);
		}
	}
}
auto get_iterator(std::vector<std::string>::const_iterator start, std::vector<std::string>::const_iterator end, const std::string&delStr){
	auto&it = start;
	for (; it != end; ++it){
		if(*it == delStr){
			break;
		}
	}
	return it;
}
void duplicate_removal(std::vector<std::string>&str){
	auto origin = str.begin();
	while(origin != str.end()){
		auto it = get_iterator(origin + 1, str.end(), *origin);
		if(it != str.end())
			str.erase(it);
		else
			++origin;
	}
}
int main(int32_t argc, char *argv[], char *envp[]){//envp环境变量表
	// double getdir_totaltime = 0.0f, search_totaltime = 0.0f;
	// clock_t getdir_start, getdir_finish, search_file_start, search_file_finish;//time count
	// const std::vector<std::string> option = { FUNCTION_OPTION, MACRO_OPTION,  STRUCTURE_OPTION, TYPEDEF_OPTION, UNION_OPTION, ENUM_OPTION, CLASS_OPTION, NAMESPACE_OPTION };//, "-d", "-n"
	if(isInvalid(argc, argv)){
		help(argc, argv);
		return -1;
	}
	if(isDefaultOption(argc, argv)){
        strcpy(argv[0], "-" FUNCTION_OPTION);
    }
	std::vector<std::string> rootPath;
	// std::vector<std::string> searchFile;
	// std::vector<std::string> noSearchPath;
	// std::vector<std::string> noSearchFile;// = { "vulkan_core.h" };
	// get_option_val(argc, argv, FILE_OPTION, searchFile);
	// get_option_val(argc, argv, NO_SEARCH_PATH_OPTION, noSearchPath);
	// get_option_val(argc, argv, NO_SEARCH_FILE_OPTION, noSearchFile);
    // int32_t sOption = get_index_in_line(argc, argv, EXACT_MATCH_OPTION);
    // SearchOptionBit searchOption = SO_NONE_BIT;
    // if(INVALID_VAL != sOption){
    //     searchOption |= SO_EXACT_MATCH_BIT;
    //     removeArgv(argc, argv, sOption);
    // }
    // sOption = get_index_in_line(argc, argv, CASE_INSENSITIVE_OPTION);
    // if(INVALID_VAL != sOption){
    //     searchOption |= SO_CASE_INSENSITIVE_BIT;
    //     removeArgv(argc, argv, sOption);
    // }
    getRootPath(argc, argv, rootPath);
	if(rootPath.empty()){//用户未指定目录就从默认的目录查找
#ifdef __linux
		// rootPath.push_back(DEFAULT_INCLUDE_PATH_USR_LIB);
		rootPath.push_back(DEFAULT_INCLUDE_PATH_USR_INCLUDE);
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
//因为g++的-D选项是编译时加入宏,所以如果要调试(加入DEBUG宏),则应该传入-DDEBUG选项
	std::vector<search_infor>searchPath;//需要搜索指定的所有根目录的数量//一个就能代表一个文件夹以及里面所有文件
	//支持多个目录、的多个文件//-a表示找所有类型。多执行几次不同类型的search函数即可
	// if(searchFile.empty()){
	//没有指定文件名称，需要查找目录下的所有文件名
// #ifdef DEBUG
// 	getdir_start = clock();
// #endif
	for (size_t uiRootPath = 0; uiRootPath < rootPath.size(); ++uiRootPath){
		const uint32_t rootPathLength = rootPath[uiRootPath].length();
		if(rootPath[uiRootPath][rootPathLength - 1] == '/' || rootPath[uiRootPath][rootPathLength - 1] == '\\'){
			//指定的是目录
			getdir(rootPath[uiRootPath].c_str(), searchPath);
		}
		else{
				search_infor info;
				for (size_t i = rootPathLength - 1; i >= 0; --i){
					if(rootPath[uiRootPath][i] == '/' || rootPath[uiRootPath][i] == '\\'){
						info.sfname.push_back(rootPath[uiRootPath].c_str() + i + 1);
						char str[MAX_PATH] = {0};
						memcpy(str, rootPath[uiRootPath].c_str(), i);
						info.spath = str;
						searchPath.push_back(info);
						break;
					}
				}
		}
	}
// #ifdef DEBUG
// 	getdir_finish = clock();
// 	getdir_totaltime = (double)(getdir_finish - getdir_start) / CLOCKS_PER_SEC;
// #endif
	//没传选项的话应该默认-f
	//支持查询多个相同选项：-f strcpy printf -m MAXBYTE -m A
	std::vector<std::string>option;
	get_option(argc, argv, option);
	duplicate_removal(option);
	void (*fun[])(const std::string&, const std::string&, std::vector<std::string>&) = {
		search_fun,
		search_macro,
		search_struct,
		search_union,
		search_enum,
		search_class,
		search_namespace,
		search_type_define,
	};
	const char *opt[] = { FUNCTION_OPTION, MACRO_OPTION, STRUCTURE_OPTION, UNION_OPTION, ENUM_OPTION, CLASS_OPTION, NAMESPACE_OPTION, TYPEDEF_OPTION };
	for (uint32_t uiOption = 0; uiOption < option.size(); ++uiOption){
		std::vector<std::string>findStr;//./query strcpy -s ...
		get_option_val(argc, argv, option[uiOption], findStr, 0);//默认选项会把argv[0]替换成-a, 所以应该从0开始查找
		do{
			removePath(findStr);
		}while(havePath(findStr));
		//通过判断选项中的字符来确定搜索的类型
		if(strstr(option[uiOption].c_str(),  ALL_OPTION)){
			//搜索所有类型
			for (size_t i = 0; i < sizeof(opt) / sizeof(const char *); ++i){
				search(findStr, searchPath, fun[i]);
			}
		}
		else{
			for (size_t i = 0; i < sizeof(opt) / sizeof(const char *); ++i){
				if(strstr(option[uiOption].c_str(), opt[i])){
					search(findStr, searchPath, fun[i]);
				}
			}
		}
	}
	// for (size_t i = 0; i < g_Option.size(); ++i){
	// 	//判断需要查询的类型
	// 	std::vector<std::string> findStr;//./query strcpy -s ...
	// 	get_option_val(argc, argv, g_Option[i], findStr, 0);
	// 	if(findStr.empty())continue;
	// 	search_file_start = clock();
	// 	for (size_t uiFindStr = 0; uiFindStr < findStr.size(); ++uiFindStr){
	// 		for (size_t uiSearchPath = 0; uiSearchPath < searchPath.size(); ++uiSearchPath){
	// 			
	// 			if(g_Option[i] != ALL_OPTION){
	// 				Search_Type funIndex = (Search_Type)i;
	// 				if(funIndex > searchTypeDefine ){
	// 					const char *s[] = { "union", "enum", "class", "namespace" };
	// 					searchStructString = s[funIndex - 4];
	// 					funIndex = searchStruct;//------note:
	// 				}
	// 				for (size_t uiDir = 0; uiDir < dir.sfname.size(); ++uiDir){
	// 					search(dir.spath, dir.sfname[uiDir], findStr[uiFindStr], fun[funIndex], searchOption);
	// 				}
	// 			}
	// 			else{
	// 				for (size_t uiDir = 0; uiDir < dir.sfname.size(); ++uiDir){
	// 					for (size_t uiFun = 0; uiFun < sizeof(fun) / sizeof(int *) + 4; ++uiFun){
	// 						if(uiFun > searchTypeDefine){
	// 							const char *s[] = { "union", "enum", "class", "namespace" };
	// 							searchStructString = s[uiFun - 4];
	// 						}
	// 						else{
	// 							searchStructString = "struct";
	// 						}
	// 						search(dir.spath, dir.sfname[uiDir], findStr[uiFindStr], fun[uiFun > searchTypeDefine ? uiFun - 4 : uiFun], searchOption);
	// 					}
	// 				}
	// 			}
	// 			
	// 		}
	// 	}
	//     printf("------------------选项:%s;搜索内容:", g_Option[i].c_str());
    //     for (size_t j = 0; j < findStr.size(); ++j){
    //         printf("%s;", findStr[j].c_str());
    //     }
    //     printf("------------------\n");
	// }
	// search_file_finish = clock();
	// search_totaltime = (double)(search_file_finish - search_file_start) / CLOCKS_PER_SEC;
#ifdef DEBUG
	uint32_t total_file = 0;
	for (size_t uiSearchPath = 0; uiSearchPath < searchPath.size(); ++uiSearchPath){
		total_file += searchPath[uiSearchPath].sfname.size();
	}
	printf("total file:%d\n");
	printf("option:");
	for (size_t i = 0; i < option.size(); ++i){
		printf("%s ", option[i].c_str());
	}
	printf("\n");
	printf("path:");
	for (size_t i = 0; i < rootPath.size(); ++i){
		printf("%s ", rootPath[i].c_str());
	}
	printf("\n");
	// std::cout << "total file:" << total_file << ";get directory time:" << getdir_totaltime << ";search file time:" << search_totaltime << std::endl;
#endif
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
    for (size_t i = index; i < argc; ++i){
        argv[i] = argv[i + 1];
    }
    // argv[argc - 1] = nullptr;
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
		return -1;
	}
	int size = getfilelen(fp);
	if(content){
		fread(content, size, 1, fp);
	}
	fclose(fp);
	return size;
}
void ConvertCase(char *content, uint32_t len, bool toLower){
    //先判断是否小写/大写, 然后再转换
    if(toLower){
        for (size_t i = 0; i < len; ++i){
            if(!islower(content[i])){
                content[i] = tolower(content[i]);
            }
        }
    }
    else{
        //上面是把所有大写转成小写, 小写不管, 下面则相反。除非改确实能做到前面的功能, 否则不能修改
        for (size_t i = 0; i < len; ++i){
            if(islower(content[i])){
                content[i] = toupper(content[i]);
            }
        }
    }
}
void search(const std::vector<std::string>&findStr, const std::vector<search_infor>&searchPath, void (*searchfun)(const std::string&, const std::string&, std::vector<std::string>&)){
	for (uint32_t uiFindStr = 0; uiFindStr < findStr.size(); ++uiFindStr){
		for (size_t uiSearchPath = 0; uiSearchPath < searchPath.size(); ++uiSearchPath){
			const search_infor&dir = searchPath[uiSearchPath];
			for (size_t uiDir = 0; uiDir < dir.sfname.size(); ++uiDir){
				search(dir.spath, dir.sfname[uiDir], findStr[uiFindStr], searchfun);
			}
		}	
	}
}
void search(const std::string&cPath, const std::string&filename, const std::string&lpstr, void(*fun)(const std::string&content, const std::string&lpstr, std::vector<std::string>&str)){
	std::string szPath;
	if('/' != cPath[cPath.length() - 1])
		szPath = cPath + "/" + filename;
	else
		szPath = cPath + filename;
	uint32_t size = GetFileContent(szPath, nullptr);
	if(size >= -1){
		printf("无法打开'%s'文件,如果该文件为路径那么应该在最后加上'/'\n例如%s/\n", szPath.c_str(), szPath.c_str());
		return;
	}
	char *content = new char[size + 1];
	GetFileContent(szPath, content);
	content[size] = 0;
	remove_comment(content);
    // ConvertCase(content, size);//如果直接这么改，那么显示出来的结果也全被转换为大写或小写
    // char *searchStr = new char[lpstr.length() + 1];
    // strcpy(searchStr, lpstr.c_str());
    // ConvertCase(searchStr, lpstr.length());
	std::vector<std::string>str;
	fun(content, lpstr, str);
    // delete[]searchStr;
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
		函数声明:返回类型 函数名(零个或多个参数,可能带回车符);
		void (*(*fun(...))(...))(...) ;


		//---------
		函数参数必须是带返回类型的，这里没有判断
	*/
	size_t funNameSize = strlen(fun_name);
	size_t funNamePos = str.find(fun_name);
	size_t c = str.find('(', funNamePos + funNameSize);
	size_t _c = str.find(')', funNamePos + funNameSize);
	const std::string invalidStr[] = {
		"if(", "if (", "return", "|", "!", "&&", "#", "->", "typedef",
		":", "{", "}", "$", "@", "%", "^", "/", "%", "//", "/*", "*/",
		"+", "-", "[", "]", ".", "~", "<", ">", "||", "!=", "==", "&&"
	};
	//排除不可能出现在函数声明的字符//完全匹配-在确定是函数(或其他待查找类似)后。必须要前面空格或回车等。后面括号等才行
	for(int32_t i = 0; i < sizeof(invalidStr) / sizeof(std::string); ++i)
		if(std::string::npos != str.find(invalidStr[i]))
			return false;
	const char *equalsign = strchr(str.c_str(), '='), *f = strstr(str.c_str(), fun_name);
	if(equalsign && equalsign < f)
		return false;
	if(strchr(str.c_str(), '('))return true;
	//排除连基本的函数声明特征都没有的字符串(没有指定的函数名没有一对括号)
	// size_t firstC = str.find('(');//有一些函数调用类似xxx(xxx()),该变量保存的是第一个'('和上面的c有所不同&& firstC > funNamePos && str.find(')') > firstC
	if(std::string::npos != str.find(";") && std::string::npos != funNamePos && std::string::npos != c && std::string::npos != _c){
		if(str[c + 1] != ')' && std::string::npos == str.find(" "))return false;
		// size_t comma = str.find(',');
		// if((comma == std::string::npos || c < comma) && (std::string::npos == point || str[point + 1] == '.')){
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
		// }
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
#define FUNCTION_MAX_CHA 300
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
					// if(!(option & SO_EXACT_MATCH_BIT) || isExacgMatch(fun, lpstr)){
					//     std::string _str(lpStart, len + 1);
					//     str.push_back(_str);
					// }
					std::string _str(lpStart, len + 1);
					str.push_back(_str);
				}
        }
        lpStart = lpEnd + lpstr.length();
        // lpStart += len + 1;
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
		while((lpEnd = strchr(lpEnd, '\n')) && (*(lpEnd - 1) == '\\' || *(lpEnd - 2) == '\\')){
            ++lpEnd;
		}
		std::string buff(lpStart, lpEnd - lpStart);
        // if(!(option & SO_EXACT_MATCH_BIT) || isExacgMatch(buff, lpstr)){
		str.push_back(buff);
        // }
		lpStart += lpstr.length() + strlen("#define ");
	}
}
/*}}}*/
void search_class(const std::string&content, const std::string&lpstr, std::vector<std::string>&str){
	search_structure(content, lpstr, str, "class");
}
void search_enum(const std::string&content, const std::string&lpstr, std::vector<std::string>&str){
	search_structure(content, lpstr, str, "enum");
}
void search_union(const std::string&content, const std::string&lpstr, std::vector<std::string>&str){
	search_structure(content, lpstr, str, "union");
}
void search_struct(const std::string&content, const std::string&lpstr, std::vector<std::string>&str){
	search_structure(content, lpstr, str, "struct");
}
void search_namespace(const std::string&content, const std::string&lpstr, std::vector<std::string>&str){
	search_structure(content, lpstr, str, "namespace");
}
//typeName是结构的类型名。例如struct、class等
void search_structure(const std::string&content, const std::string&lpstr, std::vector<std::string>&str, const std::string&typeName){
//just search struct name;no search struct alias name
	// int count = content.length();
	const char *lpEnd = nullptr;
	const char *lpStart = content.c_str();
    //结构体前面带typedef.后面才能带别名
	char buffer[MAXBYTE] = {0};
    //这里假设该结构体非匿名结构体
	sprintf(buffer, "%s %s", typeName.c_str(), lpstr.c_str());
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
            // if(!(option & SO_EXACT_MATCH_BIT) || isExacgMatch(buff, lpstr)){
            str.push_back(buff);
            // }
		}
		lpStart += lpstr.length() + typeName.length() + strlen("typedef");
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
            // if(!(option & SO_EXACT_MATCH_BIT) || isExacgMatch(buff, lpstr)){
			str.push_back(buff);
            // }
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
void help(int32_t argc, char *argv[]){
	//没传选项的话应该默认-f
	printf("格式:[选项] 搜索内容 [选项] [搜索内容] [搜索内容]...\n");
	printf("例子:%s strcpy strcat -s VkDeviceC -f vkCmdDraw -sf VkDeviceCreateInfo ./include/\n", argv[0]);
	// printf("example:\n\t./query strcpy strcat -s VkDeviceC -f vkCmdDraw -s VkDeviceCreateInfo -sf string.h vulkan_core.h\n\t./query -f strcpy strcat -s VkDeviceC -f vkCmdDraw -s VkDeviceCreateInfo -sf string.h vulkan_core.h\n");
	printf("选项:\n");
	// printf("%s表示不在该路及内搜索\n", NO_SEARCH_PATH_OPTION);
	// printf("%s表示不在该文件内搜索\n", NO_SEARCH_FILE_OPTION);
    // printf("\t'%s' 完全匹配\n", EXACT_MATCH_OPTION);
    // printf("\t'%s' 不区分大小写\n", CASE_INSENSITIVE_OPTION);
	printf("\t'-%s' 搜索所有类型\n", ALL_OPTION);
	printf("\t'-%s' 搜索枚举\n", ENUM_OPTION);
	printf("\t'-%s' 搜索联合体\n", UNION_OPTION);
	printf("\t'-%s' 搜索类\n", CLASS_OPTION);
	printf("\t'-%s' 搜索宏\n", MACRO_OPTION);
	printf("\t'-%s' 搜索函数\n", FUNCTION_OPTION);
	printf("\t'-%s' 搜索结构体\n", STRUCTURE_OPTION);
	// printf("\t'%s' indicate search directory\n", PATH_OPTION);
	printf("\t'-%s' 搜索命名空间\n", NAMESPACE_OPTION);
	printf("\t'-%s' 搜索typedef\n", TYPEDEF_OPTION);
    printf("说明:\n");
    printf("\t如果未指定\"选项\", 则默认为'-%s'\n", FUNCTION_OPTION);
	// printf("\t'%s' indicate search in that file;\n", FILE_OPTION);
    printf("注意:\n");
	printf("\t精准查找:%s \'fopen (\' \"fopen (\"\n\t精准查找要求和函数名完全一样。包括空格和括号\n", argv[0]);
	printf("\t'-%s'选项将忽略当前选项的其他选项, 例如'-%s%s%s', 将只执行'-%s'选项\n", ALL_OPTION, FUNCTION_OPTION, ALL_OPTION, STRUCTURE_OPTION, ALL_OPTION);
    printf("\t如果路径只有一层, 则必须包含'/'或'./'\n");
    printf("\t%s strcpy ./include\t可以获取路径,但include被视为文件\n\t\t\tinclude/\t可以获取路径\n\t\t\tinclude\t\t无法获取路径\n", argv[0]);

}
bool isInvalid(int argc, char *argv[]){
	return argc < 2;
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
	while((lpStart = strstr(lpStart, "/*"))){
		const char *lpEnd = strstr(lpStart, "*/");
		if(lpEnd){
			// lpEnd += 2;
			do{
				if(*lpStart != '\n'){
					*lpStart = ' ';
				}
				++lpStart;
			}while(lpStart != lpEnd);
		}
		else ++lpStart;
	}
	lpStart = content;
	while((lpStart = strstr(lpStart, "//"))){
		do{
			*lpStart = ' ';
			++lpStart;
		}while(*lpStart && *lpStart != '\n');
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
	return argv[index][0] == '-';
}
//找到某个选项后面的内容, 直到遇到其他选项。例如:外面传入-f strcpy -s vkDevice而opt == "-f"。函数会因为遇到-s而只获得strcpy。
void get_option_val(int argc, char *argv[], const std::string&opt, std::vector<std::string>&out, int32_t start){
	int offset = start;
	int index = INVALID_VAL;
	while(INVALID_VAL != (index = get_index_in_line(argc, argv, opt, offset))){
		++index;
		while(index < argc && argv[index] && !isOption(argc, argv, index)){
			out.push_back(argv[index]);
			++index;
		}
		offset = index;
	}
}
