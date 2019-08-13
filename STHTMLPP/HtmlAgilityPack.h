

#ifdef HTMLAPI
# define HTMLAPI _declspec(dllexport)
#else
# define HTMLAPI _declspec(dllimport)
#endif

#define CALLTYPE _stdcall
#define HTMLAPI

#define HMBBAPI _stdcall


HTMLAPI HTMLengine* HMBBAPI CreateHtml();
HTMLAPI void HMBBAPI engineFree(HTMLengine* e, GumboOutput *engine);
HTMLAPI GumboOutput* HMBBAPI LoadHtml(HTMLengine* html,const char* HTML);
HTMLAPI GumboNode* HMBBAPI GetRootNode(HTMLengine* html, GumboOutput *engine);
HTMLAPI LPVOID HMBBAPI Find(HTMLengine *html, GumboNode* gum, char* path, void* JsonCode);
HTMLAPI LPVOID HMBBAPI Find1(HTMLengine *html, GumboNode* gum, vector<GumboNode*> *nodes, char* path);
HTMLAPI void HMBBAPI engineClose(HTMLengine* e);
HTMLAPI GumboNode* HMBBAPI GetNode(vector<GumboNode*> &asd, size_t i);
HTMLAPI int HMBBAPI GetNodeSize(vector<GumboNode*> &asd);
HTMLAPI void HMBBAPI GetNodeFree(vector<GumboNode*> *asd);
HTMLAPI const size_t HMBBAPI GetNodeText(HTMLengine* html, GumboNode* htmlcode, BOOL isU_G, void* text);
HTMLAPI const int HMBBAPI GetNodeHtml(GumboNode* htmlcode, BOOL isU_G, void* text);
HTMLAPI const char* HMBBAPI GetAttr(GumboNode* htmlcode, const char* name);



#include <algorithm>
#include <fstream>
int main(int argc, char* argv[])
{
	
	//_CrtSetBreakAlloc(169876);
	const char* filename = "utf8.html";

	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (!in) {
		std::cout << "File " << filename << " not found!\n";
		exit(EXIT_FAILURE);
	}

	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();

	//.serial-wrap cf>div>.left - wrap>div:find('data-l1', '10')>div>ul>li:find('data-l2', '3')>div>p
	sds htmls = sdsnew(contents.c_str());
	sds patha = sdsnew("span>span");//a:find('class','green')

	HTMLengine *html = CreateHtml();
	//加载解析一个html字符串

	GumboOutput *engine = LoadHtml(html, htmls); //html->HTML_Create(HTML);
													 //获取html根节点
	GumboNode* gum = html->HTML_GetRootNode(engine);

	//根据自定义css选择器函数解析一个节点或者节点数组
	//.serial-wrap cf>div>.left-wrap>div:find('data-l1','10')>div>ul>li:find('data-l2','3')>div>p
	//div:find('data-l1','10')>div>ul>li:find('data-l2','3')>div>p

	cout << "表达式: " << patha << "\n";
	Json::Value jsondata;

	//FindJson(html, gum, path, dada);

	start = clock();
	char kk[10000];
	
	GumboNodes* HTMLnode = (GumboNodes*)Find(html, gum, patha, kk);
	//GetNodeFree(HTMLnode);
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;

	cout << "\n递归次数：" << n << endl;
	cout << "\n解析出来节点数量：" << HTMLnode->size() << endl;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	//Source.def
	//返回节点数组
	//切记不能销毁引擎之后再去读节点 会爆炸的
	/*cout << jsondata.toStyledString().size() << endl;
	cout << jsondata.toStyledString() << endl;
	for (size_t i = 0; i < HTMLnode.size(); i++)
	{
	//cout << HTMLnode[i]->v.element.children.length << "\n";
	//std::string no(htmlcode->v.element.original_tag.data, htmlcode->v.element.original_end_tag.data- htmlcode->v.element.original_tag.data+ htmlcode->v.element.original_end_tag.length);
	//std::string eo(, HTMLnode[i]->v.element.original_end_tag.length);
	//std::cout << U2G(no.c_str()) << std::endl;
	cout << "结果:" << U2G(html->HTMLNode_GetText(HTMLnode[i]).c_str()) << "\n";
	//cout << "结果:" << no.c_str() << "\n";
	}*/
	//销毁引擎
	engineFree(html, engine);
	free(html);
	//html->HTML_Destory(engine);
	_CrtDumpMemoryLeaks();

	system("pause");
	return 0;
}