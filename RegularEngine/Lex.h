#include "Regular.h"
string spilt(string &str,int l, int r){
	string ret = "";
	for (int i = l; i <= r; i++)
		ret += str[i];
	return ret;
}
string replace(const string &str, const string &key,const string &value){
	string ret = "";
	for (int i = 0; i <= str.length() - key.length(); i++){
		bool result = 1;
		for (int j = 0; j <= key.length() - 1; j++){
			if (str[i + j] != key[j]){
				result = 0;
				break;
			}
		}
		if (result){
			ret += value;
			i += key.length() -1;
		}
		else{
			ret += str[i];
		}
	}
	for (int i = str.length() - key.length() + 1; i <= str.length() - 1; i++)
		ret += str[i];
	return ret;
}
string getStr(string str,int num){
	int k = 0;
	str = str + " ";
	for (int i = 1; i <= num; i++){
		int last = k;
		while (k < str.length() && str[k] != ' ')k++;
		if (k == str.length())return "";
		else { if (i == num)return spilt(str, last, k - 1); k++; }
	}
	return "";
}
int stoi(string a){
	int ret = 0;
	for (int i = 0; i <= a.length() - 1; i++){
		ret = ret * 10 + char(a[i] - '0');
	}
	return ret;
}
string mulstr(string a,int k){
	string ret = "";
	for (int i = 1; i <= k; i++)
		ret += a;
	return ret;
}
class Lex{
private:
	Reg reg;
	int nowRuleNumber = 0;
	int ready = 0;
public:
	bool ReadFromFile(string File){
		reg.init();
		ifstream fin(File);
		string c;
		map<string, string> m;
		bool onRule = 0;
		while (getline(fin,c)){
			if (c[0] == '@' || c[0] == '\n')continue;
			else if (c[0] == '#'){
				if (c[1] == 'D'&&c[2] == 'E'&&c[3] == 'F'){
					m.emplace(getStr(c, 2), getStr(c, 3));
				}
				else if (c[1] == 'R'&&c[2] == 'E'&&c[3] == 'P'){
					m.emplace(getStr(c, 2), mulstr(getStr(c, 3), ::stoi(getStr(c, 4))));
				}
				else if (c[1] == 'E'&&c[2] == 'N'&&c[3] == 'D'){
					if (!onRule)return 0;
					else {
						reg.toNFA();
						reg.toDFA();
						ready = 1;
						return 1;
					}
				}
				else if (c[1] == 'R'&&c[2] == 'U'&&c[3] == 'L'&&c[4] == 'E'){
					if (!onRule)onRule = 1;
					else return 0;
				}
				else return 0;
			}
			else{
				if (!onRule)return 0;
				for (auto &x : m){
					c = replace(getStr(c,2), x.first, x.second);
				}
				reg.addRule(c, ++nowRuleNumber);
			}
		}
		return 0;
	}
};


