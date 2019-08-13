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


//���ֽ�

#define _TXT__(x) #x
#define GumboTag(EM) _TXT__(EM)



//���ֽڻ� unicode

#define _TXT__(x) _T(#x)
#define GumboTag(EM) _TXT__(EM)
/*
*
*�ַ����Ƿ�Ϊgbk�ĺ���
*
*/
bool is_str_gbk(const char* str)
{
	unsigned int nBytes = 0;//GBK����1-2���ֽڱ���,�������� ,Ӣ��һ��
	unsigned char chr = *str;
	bool bAllAscii = true; //���ȫ������ASCII,
	for (unsigned int i = 0; str[i] != '\0'; ++i) {
		chr = *(str + i);
		if ((chr & 0x80) != 0 && nBytes == 0) {// �ж��Ƿ�ASCII����,�������,˵���п�����GBK
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
	if (nBytes != 0) {   //Υ������
		return false;
	}
	if (bAllAscii) { //���ȫ������ASCII, Ҳ��GBK
		return true;
	}
	return true;
}
/*
*
*�ַ����Ƿ�Ϊutf8�ĺ���
*
*/
bool is_str_utf8(const char* str)
{
	unsigned int nBytes = 0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
	unsigned char chr = *str;
	bool bAllAscii = true;
	for (unsigned int i = 0; str[i] != '\0'; ++i) {
		chr = *(str + i);
		//�ж��Ƿ�ASCII����,�������,˵���п�����UTF8,ASCII��7λ����,���λ���Ϊ0,0xxxxxxx
		if (nBytes == 0 && (chr & 0x80) != 0) {
			bAllAscii = false;
		}
		if (nBytes == 0) {
			//�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
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
			//���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx
			if ((chr & 0xC0) != 0x80) {
				return false;
			}
			//����Ϊ��Ϊֹ
			nBytes--;
		}
	}
	//Υ��UTF8�������
	if (nBytes != 0) {
		return false;
	}
	if (bAllAscii) { //���ȫ������ASCII, Ҳ��UTF8
		return true;
	}
	return true;
}

/*
*
*GB2312��UTF-8��ת��
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
*UTF-8��GB2312��ת��
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
*HTML�����ӿڷ�װ
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
*ɾ���ַ�����β�ո�
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
*ɾ���ַ���ָ���ַ�
*
*/
void del_chr(char* s, char ch)
{
	char *t = s; //Ŀ��ָ����ָ��ԭ��ͷ
	while (*s != '\0') //�����ַ���s
	{
		if (*s != ch) //�����ǰ�ַ�����Ҫɾ���ģ��򱣴浽Ŀ�괮��
			*t++ = *s;
		s++; //�����һ���ַ�
	}
	*t = '\0'; //��Ŀ�괮��������
}
/*
*
*��ȡ�ַ���hashֵ
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
*��ȡ�ַ���hashֵ
*
*/
constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)
{
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}
/*
*
*����״̬���ж�
*
*/
constexpr unsigned long long operator "" _hash(char const* p, size_t)
{
	return hash_compile_time(p);
}
/*
*
*�ַ����ָ�
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
*�ַ�����ȡ�ж�
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
*�ַ����滻
*
*/
string strreplace(string str, string str1, string str2) {
	string::size_type idx = str.find(str1);	if (idx == string::npos)	return str;	str = str.replace(str.find(str1), str1.length(), str2);
	return strreplace(str, str1, str2);
}
/*
*���Stream��ǰ׺�Ƿ�Text
*�Ƿ���true����Streamƫ��strlen(Text)���ַ�
*���򷵻�false
*�˺��������Stream��ͷ�Ŀո�
*/
bool Is(char*& Stream, const char* Text)
{
	size_t len = strlen(Text);
	/*�������*/
	char* Read = Stream;
	/*���˿ո�*/
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
*״̬��
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