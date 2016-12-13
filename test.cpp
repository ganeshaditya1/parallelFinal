#include <iostream>
#include <vector>
#include <map>
#include <string.h>
using namespace std;
int main()
{
	/*std::vector<string> v;
	v.push_back("asd");
	v.push_back("asss");
	v.push_back(v[0] + "as");
	cout << (v[3]);*/
	/*std::vectorstd::map<int, std::vector<string> > h;
	std::vector<string> v;
	h[200] = v;
	h[200].push_back("Asd11");
	if(h.find(200) != h.end())
	{
		cout << h[200][0];
	}*/

	/*std::vector<std::map<int, std::vector<string> > > mainList;
	mainList.resize(200);
	std::map<int, std::vector<string> > *h = &mainList[0];
	*h[200] = "asd";*/
	/*char a[100] = "Asdkfjakldsfjlkasjdflasdf";
	string s = "";
	s += string(a).substr(5, 10);
	cout << s.length();
	s += 'a';
	cout << endl << s.length();*/



	std::map<int, std::vector<string> > h[15000];
	h[200][2].push_back("asd");
	return 0;
}