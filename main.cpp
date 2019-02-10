#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <string>
#define MAXPATH 360
#define MAXBYTE 0xff
#define INVALID_VAL -1
struct search_infor{
	char spath[MAXBYTE];//search path
	std::vector<std::string> sfname;//search file name
};
const char *option[] = { "-f", "-m", "-s", "-t", "-d", "-n"};
//bool g_bFun;
int getfilelen(FILE *fp);
void getdir(const char *root, std::vector<search_infor>&path);
bool get_val_in_line(int argc, char *argv[], const char *lpsstr, char *lpstr);
int get_index_in_line(int argc, char *argv[], const char *str);
int linage(char *content);
void help();
bool isFun(char *lpstr);
bool isInvalid(int argc, char *argv[]);
char *movepointer(char *p, char ch, bool bfront);
void remove_comment(char *content);;
void search(const char *cPath, const char *filename, const char *lpstr, void(*fun)(const char *content, const char *lpstr, std::vector<std::string>&str));
void search_fun(const char *content, const char *lpstr, std::vector<std::string>&str);
void search_macro(const char *content, const char *lpstr, std::vector<std::string>&str);
void search_struct(const char *content, const char *lpstr, std::vector<std::string>&str);
void search_type_define(const char *content, const char *lpstr, std::vector<std::string>&str);
bool str_sep(char *lpstr, char *_str, const char *search);
//char *strrpc(char *str,char *oldstr,char *newstr);
int main(int argc, char *argv[]){
	if(isInvalid(argc, argv)){
		help();
		return -1;
	}
//	char szSearch[MAXBYTE] = {0};
//	char filename[MAXBYTE] = {0};
	int fun_index = INVALID_VAL;
	char sstr[MAXBYTE];//search string
	char sfName[MAXBYTE];//search file name
	char rootdirectory[MAXPATH] = {0};
	std::vector<search_infor>path;
	void (*fun[])(const char *content, const char *lpstr, std::vector<std::string>&str) = {
		search_fun,
		search_macro,
		search_struct,
		search_type_define
	};
	//if user specified file name then user default directory and that file name search
	//if user specified file directory then user that diretory for root directory
	//if user specified file name and directory then user that file name and that directory search
	//so should know user have specify directory or file name
	if(!get_val_in_line(argc, argv, "-d", rootdirectory)){
		strcpy(rootdirectory, "/usr/include");
	}
	for(int i = 0; i < sizeof(option) / sizeof(char *); i++){
		if(get_val_in_line(argc, argv, option[i], sstr)){
			fun_index = i;
			break;
		}
	}
	if(!strcmp(sstr, "")){
		printf("no specified string to search\n");
		return -1;
	}
	get_val_in_line(argc, argv, "-n", sfName);
	if(strcmp(sfName, "")){
		search(rootdirectory, sfName, sstr,fun[fun_index]);
	}
	else{
		//获得路径,然后搜索
		getdir(rootdirectory, path);
		for(int i = 0; i < path.size(); i++){
			for(int j = 0; j < path[i].sfname.size(); j++)
				search(path[i].spath, path[i].sfname[j].c_str(), sstr, fun[fun_index]);
		}
   	}
	return 0;
}
void getdir(const char *root, std::vector<search_infor>&path){
	DIR *d;
	dirent *file = NULL;
	search_infor infor;
	strcpy(infor.spath, root);
	if(!(d = opendir(root))){
		perror("opendir error");
		printf("path is %s\n", root);
		return;
	}
	while((file = readdir(d))){
		if(strcmp(file->d_name, ".") && strcmp(file->d_name, "..")){
			if(file->d_type == DT_DIR){
				char szPath[MAXPATH] = {0};
				if('/' != infor.spath[strlen(infor.spath) - 1])
					sprintf(szPath, "%s/%s", infor.spath, file->d_name);
				else
					sprintf(szPath, "%s%s", infor.spath, file->d_name);
				getdir(szPath, path);
			}
			else{
				std::string name(file->d_name);
				infor.sfname.push_back(name);
			}
		}
	}
	closedir(d);
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
	std::vector<std::string>str;
	fun(content, lpstr, str);
	int line;
	char *lpStart = 0;//show line
	if(!str.empty())printf("\e[32m%s\e[0m\n", szPath);
	for(int i = 0; i < str.size(); i++){
		lpStart = strstr(content, str[i].c_str());
		char ch = *(lpStart - 1);
		*(lpStart - 1) = 0;
		line = linage(content);
		*(lpStart - 1) = ch;
		printf("%d:%s\n",line, str[i].c_str());
	}
	delete[]content;
	str.clear();
}
bool isFun(char *lpstr){
//	what is function? before have ' ' after have '('
	char *p = lpstr;
	bool isfun = false;
	p = movepointer(p, '\n', true);p++;
	if('#' != *p && (' ' == *(lpstr - 1) || '\t' == *(lpstr - 1))){
		p = strchr(lpstr, '\n');
		if(p){
			*p = 0;
//			printf("lpstr = %s\n", lpstr);
			if(strchr(lpstr, '('))isfun = true;
			*p = '\n';
		}
	}
	return isfun;
}
//--前 ++后
char *movepointer(char *p, char ch, bool bfront){
	if (!p)return nullptr;
	if (bfront) {
		while (*p && *p != ch && p--);
	}
	else {
		p = strchr(p, ch);
	}
	return p;
}
void search_fun(const char *content, const char *lpstr, std::vector<std::string>&str){
	int count = strlen(content);
	char *Buff = new char[count + 1];
	memset(Buff, 0, count + 1);
	strcpy(Buff, content);
//-------
	char *lpStart = Buff;
	while((lpStart = strstr(lpStart, lpstr))){
		//---判断找到的字符串是否是函数
//		printf("%.*s\n", 50, lpStart);
		if(isFun(lpStart)){
			lpStart = movepointer(lpStart, '\n', true);lpStart++;
			if(lpStart){
				char buff[MAXBYTE] = {0};
				if(str_sep(lpStart, buff, "\n")){
					std::string _str(buff);
					str.push_back(_str);
				}
				else{
					printf("str_sep return null\n");
				}
			}
		}
		lpStart = strchr(lpStart, '\n');
	}
	delete[]Buff;
}
bool str_sep(char *lpstr, char *_str, const char *search){
	if(!lpstr || !_str || !search)return false;
	char *str = strstr(lpstr, search);
	if(!str)return false;
	*str = 0;
	strcpy(_str, lpstr);
	*str = search[0];
	return true;
}
void search_macro(const char *content, const char *lpstr, std::vector<std::string>&str){
	int count = strlen(content);
	char *Buff = new char[count + 1];
	memset(Buff, 0, count + 1);
	strcpy(Buff, content);
	char *lpStart = Buff;
	while((lpStart = strstr(lpStart, lpstr))){
		char *p = lpStart - strlen("#define ");
		if(!memcmp(p, "#define ", strlen("#define "))){
			lpStart = p;
			p = strchr(lpStart, '\n');
			if(p && lpStart){
				if('\\' == *(p - 1)){
					do{
						p++;
						p = strchr(lpStart, '\n');
						if(!p)break;
					}while('\\' == *(p - 1));
				}
				if(p){
					*p = 0;
					std::string buff(lpStart);
					str.push_back(buff);		
				}
			}
		}
		lpStart += strlen(lpstr) + strlen("#define ");
	}
	delete[]Buff;
}
void search_struct(const char *content, const char *lpstr, std::vector<std::string>&str){
//just search struct name;no search struct alias name
	int count = strlen(content);
	char *Buff = new char[count + 1];
	memset(Buff, 0, count + 1);
	strcpy(Buff, content);
	char *lpStart = Buff;
	while((lpStart = strstr(lpStart, lpstr))){
		char *p = lpStart - strlen("struct ");
		if(!memcmp(p, "struct ", strlen("struct "))){
			p -= strlen("typedef ");
			if(memcmp(p, "typedef ", strlen("typedef")))p += strlen("typedef");
			lpStart = p;
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
		}
		lpStart += strlen(lpstr) + strlen("struct ") + strlen("typedef");
	}
	delete[]Buff;
}
void search_type_define(const char *content, const char *lpstr, std::vector<std::string>&str){
	int count = strlen(content);
	char *Buff = new char[count + 1];
	memset(Buff, 0, count + 1);
	strcpy(Buff, content);
	char *lpStart = Buff;
	while((lpStart = strstr(lpStart, lpstr))){
		char *p = lpStart - strlen("typedef ");
		if(!memcmp(p, "typedef ", strlen("typedef "))){
			lpStart = p;
			p = strchr(lpStart, '\n');
			if(p){
				*p = 0;
				std::string buff(lpStart);
				str.push_back(buff);
			}
		}
		lpStart += strlen(lpstr) + strlen("typedef ");
	}
	delete[]Buff;
}
bool get_val_in_line(int argc, char *argv[], const char*lpsstr, char *lpstr){
	int index = get_index_in_line(argc, argv, lpsstr);
	if(INVALID_VAL != index && argv[index + 1]){
		return strcpy(lpstr, argv[index + 1]);
	}
	return false;
}
void help(){
	printf("parameter insufficient:\n");
	printf("format:[option] string [option][string]...\n");
	printf("option:\n");
	printf("\t'-m' indicate search macro\n");
	printf("\t'-f' indicate search function\n");
	printf("\t'-s' indicate search structure\n");
	printf("\t'-d' indicate search directory\n");
	printf("\t'-t' indicate search type define\n");
	printf("\t'-n' indicate search in that file;\n");
}
bool isInvalid(int argc, char *argv[]){
	int ioption = 0;
	if(argc < 2)return true;
	int count = sizeof(option) / sizeof(char*);
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
int linage(char *content){
	int line = 1;
	char *lpStart = content;
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
