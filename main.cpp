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
#endif
enum Search_Type{
	INVALID_VAL = -1,
	searchFun = 0,
	searchMacro,
	searchStruct,
	searchTypeDefine,
	searchEnum,
	searchClass,
	searchNameSpace
};
struct search_infor{
	char spath[MAXBYTE];//search path
	std::vector<std::string> sfname;//search file name
};
//bool g_bFun;
int getfilelen(FILE *fp);
void getdir(const char *root, std::vector<search_infor>&path);
bool get_val_in_line(int argc, char *argv[], const char *lpsstr, char *lpstr);
int get_index_in_line(int argc, char *argv[], const char *str);
int linage(const char *content);
void help();
bool isFun(const char *lpstr, int str_size, const char *fun_name);
bool isInvalid(int argc, char *argv[], const char *const*option, int count);
const char *movepointer(const char *p, char ch, bool bfront);
void remove_comment(char *content);
void search(const char *cPath, const char *filename, const char *lpstr, void(*fun)(const char *content, const char *lpstr, std::vector<std::string>&str));
void search_fun(const char *content, const char *lpstr, std::vector<std::string>&str);
void search_macro(const char *content, const char *lpstr, std::vector<std::string>&str);
void search_struct(const char *content, const char *lpstr, std::vector<std::string>&str);
void search_type_define(const char *content, const char *lpstr, std::vector<std::string>&str);
#ifdef WIN32
void SetTextColor(WORD color);
#endif
std::string searchStructString = "struct ";
//char *strrpc(char *str,char *oldstr,char *newstr);
int main(int argc, char *argv[]){
	double getdir_totaltime = 0.0f, search_totaltime = 0.0f;
	clock_t getdir_start, getdir_finish, search_file_start, search_file_finish;//time count
	const char *option[] = { "-f", "-m", "-s", "-t", "-e", "-c", "-ns", "-d", "-n"};
	if(isInvalid(argc, argv, option, 6)){
		printf("parameter insufficient:\n");
		help();
		return -1;
	}
	char sstr[MAXBYTE] = {0};//search string
	char sfName[MAXBYTE] = {0};//search file name
	std::vector<search_infor>path;
	char rootdirectory[MAX_PATH] = {0};
	Search_Type fun_index = INVALID_VAL;
	void (*fun[])(const char *content, const char *lpstr, std::vector<std::string>&str) = {
		search_fun,
		search_macro,
		search_struct,
		search_type_define,
	};
	//if user specified file name then user default directory and that file name search
	//if user specified file directory then user that diretory for root directory
	//if user specified file name and directory then user that file name and that directory search
	//so should know user have specify directory or file name
	if(!get_val_in_line(argc, argv, "-d", rootdirectory)){
#if __linux
		strcpy(rootdirectory, "/usr/include");
#endif
#if WIN32
		char *env = getenv("include");
		if(env){
			strcpy(rootdirectory, env);
		}
		else{
			printf("not found environment variable:include, please specify include path or other path\n");
		}
#endif
	}
	//获得-f或者其他选项后面的内容(除了-d,-n),顺便得到需要执行的index
	for(int i = 0; i < sizeof(option) / sizeof(char *) - 2; i++){
		if(get_val_in_line(argc, argv, option[i], sstr)){
			fun_index = (Search_Type)i;
			break;
		}
	}
	//----
	if(!strcmp(sstr, "")){
		printf("no specified string to search\n");
		return -1;
	}
	if(fun_index > searchStruct){
		const char *s[] = { "enum", "class", "namespace" };
		searchStructString = s[fun_index - 4];
		fun_index = searchStruct;//------note:
	}
//	char op[MAXBYTE] = { 0 };
//	if(get_val_in_line(argc, argv, "-e", op)){
//		searchStructString = "enum ";
//	}
//	else if(get_val_in_line(argc, argv, "-c", op)){
//		searchStructString = "class";
//	}
//	else if(get_val_in_line(argc, argv, "-ns", op)){
//		searchStructString = "namespace";
//	}
	get_val_in_line(argc, argv, "-n", sfName);
	if(strcmp(sfName, "")){
		search_file_start = clock();
		search(rootdirectory, sfName, sstr,fun[fun_index]);
		search_file_finish = clock();
		search_totaltime = (double)(search_file_finish - search_file_start) / CLOCKS_PER_SEC;
	}
	else{
		//获得路径,然后搜索
		getdir_start = clock();

		getdir(rootdirectory, path);

		getdir_finish = clock();
		getdir_totaltime = (double)(getdir_finish - getdir_start) / CLOCKS_PER_SEC;

		search_file_start = clock();

		for(int i = 0; i < path.size(); i++){
			for(int j = 0; j < path[i].sfname.size(); j++)
				search(path[i].spath, path[i].sfname[j].c_str(), sstr, fun[fun_index]);
		}

		search_file_finish = clock();
		search_totaltime = (double)(search_file_finish - search_file_start) / CLOCKS_PER_SEC;
   	}
	int total_file = 0;
	//get file number
	for(int i = 0; i < path.size(); i++){
		total_file += path[i].sfname.size();
	}
	std::cout << "-----------------------------" << std::endl;
	std::cout << "search path:" << rootdirectory << std::endl;
	std::cout << "total file:" << total_file << ";get directory time:" << getdir_totaltime << ";search file time:" << search_totaltime << std::endl;
	return 0;
}
void getdir(const char *root, std::vector<search_infor>&path){
	search_infor infor;
	strcpy(infor.spath, root);
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
				if('/' != infor.spath[strlen(infor.spath) - 1])
					sprintf(szPath, "%s/%s", infor.spath, file->d_name);
				else
					sprintf(szPath, "%s%s", infor.spath, file->d_name);
				getdir(szPath, path);
			}
			else{
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
void search(const char *cPath, const char *filename, const char *lpstr, void(*fun)(const char *content, const char *lpstr, std::vector<std::string>&str)){
	char szPath[MAXBYTE] = {0};
	if('/' != cPath[strlen(cPath) - 1])
		sprintf(szPath, "%s/%s", cPath, filename);
	else
		sprintf(szPath, "%s%s", cPath, filename);
	FILE *fp = fopen(szPath, "rb");
	if(!fp){
		perror("read file error");
		printf("path is %s\n", szPath);
		return;
	}
	int size = getfilelen(fp);
	char *content = new char[size + 1];
	memset(content, 0, size + 1);
	fread(content, size, 1, fp);
	fclose(fp);
	//remove_comment(content);
	if(!strcmp(filename, "vulkan_core.h")){
		int j = 0;
	}
	std::vector<std::string>str;
	fun(content, lpstr, str);
	int line;
	char *lpStart = 0;//show line
	if(!str.empty()){
#ifdef __linux
		printf("\e[32m%s\e[0m\n", szPath);
#endif
#ifdef WIN32
		SetTextColor(FOREGROUND_GREEN);
		printf("%s\n", szPath);
		SetTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
	}
	for(int i = 0; i < str.size(); i++){
		lpStart = strstr(content, str[i].c_str());
		line = linage(content) - linage(lpStart) + 1;
		printf("%d:%s\n",line, str[i].c_str());
	}
	delete[]content;
	str.clear();
}
bool isFun(const char *lpstr, int str_size, const char *fun_name){
	bool bIsFun = false;
	char *buffer = new char[str_size + 1];
	memcpy(buffer, lpstr, str_size);
	buffer[str_size] = 0;
	if(strchr(buffer, '(')){
		char *p = strstr(buffer, fun_name);//不知道为什么，复制到buffer的内容居然不包含函数名。将直接跳过这样的内容
		if(p){
			for(p += strlen(fun_name); *p && *p != '('; ++p){
				if(*p != ' ' && !isalpha(*p)){
					bIsFun = false;
					break;
				}
			}
			if(*p == '(' && *(p + 1) != ')')bIsFun = true;
		}
	}
	delete[]buffer;
	return bIsFun;
/*
	char lpReg[100] = {0};
	sprintf(lpReg, "[^#/*].*[*&]? [*&]?%s.*[ ]?(.*)?[;]?[\r]?", fun_name);
	std::regex reg(lpReg);
	std::smatch result;
	std::string str(lpstr, str_size);
	return regex_match(str, result, reg);
*/
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
void search_fun(const char *content, const char *lpstr, std::vector<std::string>&str){
//-------
	const char *lpStart = content;
	while((lpStart = strstr(lpStart, lpstr))){
		//---判断找到的字符串是否是函数
//		printf("%.*s\n", 50, lpStart);
		lpStart = movepointer(lpStart, '\n', true);lpStart++;
//		int len = strcspn(lpStart, ");");
		int len = 0;
		int lineSize;
		do{
			lineSize = strcspn(lpStart + len, "\n");
			len += lineSize;
		}while(memchr(lpStart + len - lineSize, '\\', lineSize));
		if(strchr(lpstr, '(') || isFun(lpStart, len + 1, lpstr)){
			std::string _str(lpStart, len + 1);
			str.push_back(_str);
		}
		lpStart += len + 1;
	}
}
/*{{{*/
void search_macro(const char *content, const char *lpstr, std::vector<std::string>&str){
	const char *lpStart = content;
	char buffer[MAXBYTE] = {0};
	sprintf(buffer, "#define %s", lpstr);
	while((lpStart = strstr(lpStart, buffer))){
		int len = strcspn(lpStart, "\n");
		if('\\' == *(lpStart + len - 1)){
			do{
				len += strcspn(lpStart, "\n");
			}while('\\' == *(lpStart + len - 1));
		}
		std::string buff(lpStart, len);
		str.push_back(buff);
		lpStart += strlen(lpstr) + strlen("#define ");
	}
}
/*}}}*/
void search_struct(const char *content, const char *lpstr, std::vector<std::string>&str){
//just search struct name;no search struct alias name
	int count = strlen(content);
	char *Buff = new char[count + 1];
	memset(Buff, 0, count + 1);
	strcpy(Buff, content);
	char *p = nullptr;
	char *lpStart = Buff;
	char buffer[MAXBYTE] = {0};
	sprintf(buffer, "%s %s", searchStructString.c_str(), lpstr);
	while((lpStart = strstr(lpStart, buffer))){
		lpStart -= strlen("typedef ");
		if(memcmp(lpStart, "typedef ", strlen("typedef")))lpStart += strlen("typedef ");
		p = strchr(lpStart, '\n');
		if(p && ';' != *(p - 1)){
			int count = 1;
			p = strchr(lpStart, '{');
			if(p){
				do{
					p++;
					if(*p == '{')count++;
					if(*p == '}')count--;
					if(!p)break;
				}while(count);
				p = strchr(p, '\n');
			}
		}
		if(p){
			*p = 0;
			std::string buff(lpStart);
			str.push_back(buff);
		}
		lpStart += strlen(lpstr) + searchStructString.length() + strlen("typedef");
	}
	delete[]Buff;
}
void search_type_define(const char *content, const char *lpstr, std::vector<std::string>&str){
	int count = strlen(content);
	char *Buff = new char[count + 1];
	memset(Buff, 0, count + 1);
	strcpy(Buff, content);
	char *lpStart = Buff;
	char buffer[MAXBYTE] = {0};
	sprintf(buffer, "typedef %s", lpstr);
	while((lpStart = strstr(lpStart, buffer))){
		char *p = strchr(lpStart, '\n');
		if(p){
			*p = 0;
			std::string buff(lpStart);
			str.push_back(buff);
		}
		lpStart += strlen(lpstr) + strlen("typedef ");
	}
	delete[]Buff;
}
#ifdef WIN32
void SetTextColor(WORD color){
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hout, color);
}
#endif
bool get_val_in_line(int argc, char *argv[], const char*lpsstr, char *lpstr){
	int index = get_index_in_line(argc, argv, lpsstr);
	if(INVALID_VAL != index && argv[index + 1]){
		return strcpy(lpstr, argv[index + 1]);
	}
	return false;
}
void help(){
	printf("example:query -f strcpy\n");
	printf("format:option string [option][string]...\n");
	printf("option:\n");
	printf("\t'-e' indicate search enum\n");
	printf("\t'-c' indicate search class\n");
	printf("\t'-m' indicate search macro\n");
	printf("\t'-f' indicate search function\n");
	printf("\t'-s' indicate search structure\n");
	printf("\t'-d' indicate search directory\n");
	printf("\t'-ns' indicate search namespace\n");
	printf("\t'-t' indicate search type define\n");
	printf("\t'-n' indicate search in that file;\n");
}
bool isInvalid(int argc, char *argv[], const char *const*option, int count){
	int ioption = 0;
	if(argc < 2)return true;
	for(int i = 1; i < argc; i++){
		for(int j = 0; j < count; j++){
			if(!strcmp(option[j], argv[i])){
				ioption++;
				break;
			}
		}
	}
	//说明不是只有传一个 选项
	return (ioption == argc - 1);
}
int get_index_in_line(int argc, char *argv[], const char *str){
	for(int i = 1; i < argc; i++){
		if(!strcmp(argv[i], str)){
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
	char *temp;
	char *lpStart = content;
	while((lpStart = strstr(lpStart, "//"))){
		char *p = strchr(lpStart, '\n');
		if(p){
			p += 1;
			int len = strlen(p);
			temp = new char[len + 1];
			memset(temp, 0, len + 1);
			strcpy(temp, p);
			strcpy(lpStart, temp);
			lpStart[len] = 0;
			delete[]temp;
		}
		lpStart++;
	}
	lpStart = content;
	while((lpStart = strstr(lpStart, "/*"))){
		char *p = strstr(lpStart, "*/");
		if(p){
			p += 2;
			int len = strlen(p);
			temp = new char[len + 1];
			memset(temp, 0, len + 1);
			strcpy(temp, p);
			strcpy(lpStart, temp);
			lpStart[len] = 0;
			delete[]temp;
		}
		lpStart++;
	}
}
int linage(const char *content){
	int line = 1;
	const char *lpStart = content;
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
