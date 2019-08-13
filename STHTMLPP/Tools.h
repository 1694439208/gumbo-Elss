#pragma once
#include <iostream>
#include <map>
#include<vector>
extern "C" {

}
using namespace std;
typedef std::uint64_t hash_t;

constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484222325ull;

#define MAX_BUF_SIZE 1024

#ifdef HTMLAPI
# define HTMLAPI _declspec(dllexport)
#else
# define HTMLAPI _declspec(dllimport)
#endif

#define CALLTYPE _stdcall
#define HTMLAPI

#define HMBBAPI _stdcall


//多字节

#define _TXT__(x) #x
#define GumboTag(EM) _TXT__(EM)



//多字节或 unicode

#define _TXT__(x) _T(#x)
#define GumboTag(EM) _TXT__(EM)
/*
*
*字符串是否为gbk的函数
*
*/
bool is_str_gbk(const char* str)
{
	unsigned int nBytes = 0;//GBK可用1-2个字节编码,中文两个 ,英文一个
	unsigned char chr = *str;
	bool bAllAscii = true; //如果全部都是ASCII,
	for (unsigned int i = 0; str[i] != '\0'; ++i) {
		chr = *(str + i);
		if ((chr & 0x80) != 0 && nBytes == 0) {// 判断是否ASCII编码,如果不是,说明有可能是GBK
			bAllAscii = false;
		}
		if (nBytes == 0) {
			if (chr >= 0x80) {
				if (chr >= 0x81 && chr <= 0xFE) {
					nBytes = +2;
				}
				else {
					return false;
				}
				nBytes--;
			}
		}
		else {
			if (chr < 0x40 || chr>0xFE) {
				return false;
			}
			nBytes--;
		}//else end
	}
	if (nBytes != 0) {   //违返规则
		return false;
	}
	if (bAllAscii) { //如果全部都是ASCII, 也是GBK
		return true;
	}
	return true;
}
/*
*
*字符串是否为utf8的函数
*
*/
bool is_str_utf8(const char* str)
{
	unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr = *str;
	bool bAllAscii = true;
	for (unsigned int i = 0; str[i] != '\0'; ++i) {
		chr = *(str + i);
		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
		if (nBytes == 0 && (chr & 0x80) != 0) {
			bAllAscii = false;
		}
		if (nBytes == 0) {
			//如果不是ASCII码,应该是多字节符,计算字节数
			if (chr >= 0x80) {
				if (chr >= 0xFC && chr <= 0xFD) {
					nBytes = 6;
				}
				else if (chr >= 0xF8) {
					nBytes = 5;
				}
				else if (chr >= 0xF0) {
					nBytes = 4;
				}
				else if (chr >= 0xE0) {
					nBytes = 3;
				}
				else if (chr >= 0xC0) {
					nBytes = 2;
				}
				else {
					return false;
				}
				nBytes--;
			}
		}
		else {
			//多字节符的非首字节,应为 10xxxxxx
			if ((chr & 0xC0) != 0x80) {
				return false;
			}
			//减到为零为止
			nBytes--;
		}
	}
	//违返UTF8编码规则
	if (nBytes != 0) {
		return false;
	}
	if (bAllAscii) { //如果全部都是ASCII, 也是UTF8
		return true;
	}
	return true;
}

/*
*
*GB2312到UTF-8的转换
*
*/
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
/*
*
*UTF-8到GB2312的转换
*
*/
string U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	string s(str);
	if (wstr) delete[] str;
	return s;
}

/*
*
*HTML函数接口封装
*
*/
typedef struct HTML {

	HTMLAPI GumboOutput* CALLTYPE HTML_Create(const char* buffer)
	{
		GumboOutput* GUM= gumbo_parse(buffer);
		return GUM;
	}
	HTMLAPI void CALLTYPE HTML_Destory(GumboOutput* output)
	{
		gumbo_destroy_output(&kGumboDefaultOptions, output);
	}
	HTMLAPI GumboNode* CALLTYPE HTML_GetRootNode(GumboOutput* output)
	{
		return output->root;
	}
	HTMLAPI GumboNode* CALLTYPE HTML_GetDocumentNode(GumboOutput* output)
	{
		return output->document;
	}
	//////////////////////////////////////////////////////////////
	HTMLAPI GumboNode* CALLTYPE HTMLNode_GetParent(GumboNode* node)
	{
		return node->parent;
	}
	HTMLAPI unsigned int CALLTYPE HTMLNode_GetIndexInParent(GumboNode* node)
	{
		return node->index_within_parent;
	}
	HTMLAPI GumboTag CALLTYPE HTMLNode_GetTag(GumboNode* node)
	{
		return node->v.element.tag;
	}
	HTMLAPI const char* CALLTYPE HTMLNode_GetOriginalTag(GumboNode* node)
	{
		return node->v.element.original_tag.data;
	}
	HTMLAPI unsigned int CALLTYPE HTMLNode_GetOriginalTagLength(GumboNode* node)
	{
		return node->v.element.original_tag.length;
	}
	HTMLAPI const char* CALLTYPE HTMLNode_GetOriginalEndTag(GumboNode* node)
	{
		if (node->v.element.original_end_tag.data != NULL)
		{
			return node->v.element.original_end_tag.data;
		}
		return "+++++++";
	}
	HTMLAPI unsigned int CALLTYPE HTMLNode_GetOriginalEndTagLength(GumboNode* node)
	{
		return node->v.element.original_end_tag.length;
	}
	HTMLAPI string CALLTYPE HTMLNode_GetText(GumboNode* node)
	{
		std::string TEXT;
		size_t n = HTMLNode_GetChildCount(node);
		for (size_t i = 0; i < n; i++)
		{
			GumboNode* hcode = static_cast<GumboNode*>(node->v.element.children.data[i]);
			if (hcode->type == GUMBO_NODE_TEXT)
			{
				TEXT.append((hcode->v.text.text));//U2G
			}
		}
		return TEXT;
	}
	HTMLAPI const char* CALLTYPE HTMLNode_GetHtml(GumboNode* htmlcode)
	{
		std::string no(htmlcode->v.element.original_tag.data, htmlcode->v.element.original_end_tag.data - htmlcode->v.element.original_tag.data + htmlcode->v.element.original_end_tag.length);
		return no.c_str();
	}
	HTMLAPI GumboNodeType CALLTYPE HTMLNode_GetType(GumboNode* node)
	{
		return node->type;
	}
	HTMLAPI int CALLTYPE HTMLNode_GetChildCount(GumboNode* node)
	{
		return node->v.element.children.length;
	}
	HTMLAPI GumboNode* CALLTYPE HTMLNode_GetChild(GumboNode* node, size_t i)
	{
		return static_cast<GumboNode*>(node->v.element.children.data[i]);
	}
	HTMLAPI int CALLTYPE HTMLNode_GetAttributeCount(GumboNode* node)
	{
		return node->v.element.attributes.length;
	}
	HTMLAPI GumboAttribute* CALLTYPE HTMLNode_GetAttribute(GumboNode* node, size_t i)
	{
		return static_cast<GumboAttribute*>(node->v.element.attributes.data[i]);
	}
	HTMLAPI GumboAttribute* CALLTYPE HTMLNode_GetAttributeByName(GumboNode* node, const char* name)
	{
		return gumbo_get_attribute(&node->v.element.attributes, name);
	}
	HTMLAPI const char* CALLTYPE HTMLAttribute_GetValue(GumboAttribute* attr)
	{
		return attr->value;
	}
	HTMLAPI const char* CALLTYPE HTMLAttribute_GetName(GumboAttribute* attr)
	{
		return attr->name;
	}
}HTMLengine;
/*
*
*删除字符串首尾空格
*
*/
string& trim(string &s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}
/*
*
*删除字符串指定字符
*
*/
void del_chr(char* s, char ch)
{
	char *t = s; //目标指针先指向原串头
	while (*s != '\0') //遍历字符串s
	{
		if (*s != ch) //如果当前字符不是要删除的，则保存到目标串中
			*t++ = *s;
		s++; //检查下一个字符
	}
	*t = '\0'; //置目标串结束符。
}
/*
*
*获取字符串hash值
*
*/
hash_t hash_(char const* str)
{
	hash_t ret{ basis };

	while (*str) {
		ret ^= *str;
		ret *= prime;
		str++;
	}
	return ret;
}
/*
*
*获取字符串hash值
*
*/
constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)
{
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}
/*
*
*方便状态机判断
*
*/
constexpr unsigned long long operator "" _hash(char const* p, size_t)
{
	return hash_compile_time(p);
}
/*
*
*字符串分割
*
*/
vector<string> Split(char* path, const char* N)
{
	vector<string> Nodes;
	char *token;
	token = strtok(path, N);
	while (token != NULL) {
		string token1(token);
		Nodes.push_back(trim(token1));
		token = strtok(NULL, N);
	}
	delete[] token;
	return Nodes;
}
/*
*
*字符串提取判断
*
*/
int else_strncpy(const char* txt, const char* NodeType)
{
	if (!txt)
	{
		return 1;
	}
	const char* p = &txt[2];
	char ar1[50];
	strncpy_s(ar1, 50, p, strlen(NodeType));
	if (strcmp(ar1, NodeType) == 0)
	{
		return 0;
	}
	return 1;
}
/*
*
*字符串替换
*
*/
string strreplace(string str, string str1, string str2) {
	string::size_type idx = str.find(str1);	if (idx == string::npos)	return str;	str = str.replace(str.find(str1), str1.length(), str2);
	return strreplace(str, str1, str2);
}
/*
*检查Stream的前缀是否Text
*是返回true并将Stream偏移strlen(Text)个字符
*否则返回false
*此函数会过滤Stream开头的空格
*/
bool Is(char*& Stream, const char* Text)
{
	size_t len = strlen(Text);
	/*保存参数*/
	char* Read = Stream;
	/*过滤空格*/
	while (*Read == ' ')Read++;
	if (strncmp(Read, Text, len) == 0)
	{
		Stream = Read + len;
		return true;
	}
	else
	{
		return false;
	}
}
/*
*
*状态机
*
*/
vector<string> GetExpression(char*& Read)
{
	vector<string> list;
	bool IsOperator = true;
	int i = 0;
	string text;
	char chr;
	while (true)
	{
		char c = *Read;
		switch (c)
		{
		case '>':
			if (IsOperator)
			{
				list.push_back(trim(text));
				text = "";
			}
			else {
				text.append(">");
			}
			break;
		case '(':
			IsOperator = false;
			goto add;
			break;
		case ' '://div> a > .info a > div
			if (chr!='>' || chr!=' ' || c != ' ')
			{
				goto add;
			}
			break;
		case ')':
			IsOperator = true;
			goto add;
			break;
		case '\0':
			list.push_back(text);
			goto colse;
		default:
		add:
			text.append(string(1, c));
			break;
		}
		chr = c;
		Read++;
	}
colse:
	return list;
}