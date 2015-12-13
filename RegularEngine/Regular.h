#include<memory>
#include<vector>
#include<unordered_set>
#include<queue>
#include<set>
#include<bitset>
#include<tuple>
#include<cstdlib>
#include<map>
#include<fstream>
#include<iostream>
using namespace std;
enum class ope{
	and, or, left, rep, able, prep
};
const int POINT_MAX = 50000;
int global_point_now = 0;
const int REV_RANGE = 17;
const int POS_RANGE = 18;

const int __BigLetter = -1;
const int __SmallLetter = -2;
const int __AllLetter = -3;
const int __Number = -4;
const int __LetterNumber = -5;
const int __AllChar = -6;
const int __Space = -6;

int SYSTEM_MATCHRULE_COUNT = -7;


class edge{
public:
	edge *prev;
	int t;
	char v;
	edge() = default;
	edge(int _t, char _v, edge *_prev) :
		t(_t), v(_v), prev(_prev){}
}; edge *e[POINT_MAX], *de[POINT_MAX];
class Dfa{
public:
	void addEdge(int f, int t, char v){
		de[f] = new edge(t, v, de[f]);
	}
};
class Graph{
public:
	int begin, end;
	Graph() = default;
	void addEdge(int f, int t, char v){
		e[f] = new edge(t, v, e[f]);
		cout << "add" << int(v) << endl;
	}
	Graph(char x){
		addEdge(global_point_now + 1, global_point_now + 2, x);
		this->begin = global_point_now + 1;
		this->end = global_point_now + 2;
		global_point_now += 2;
	}
	Graph(bool plan, Graph &a, Graph &b){
		if (plan){
			//or
			this->begin = global_point_now + 1;
			this->end = global_point_now + 2;
			global_point_now += 2;
			addEdge(this->begin, a.begin, 0);
			addEdge(this->begin, b.begin, 0);
			addEdge(a.end, this->end, 0);
			addEdge(b.end, this->end, 0);
		}
		else{
			//and
			this->begin = a.begin;
			this->end = b.end;
			addEdge(a.end, b.begin, 0);
		}
	}
	Graph(bool plan, Graph &a){
		//rep
		if (plan){
			//prep
			this->begin = global_point_now + 1;
			this->end = global_point_now + 2;
			global_point_now += 2;
			addEdge(this->begin, a.begin, 0);
			addEdge(a.end, this->end, 0);
			addEdge(a.end, a.begin, 0);
		}
		else{
			//rep
			this->begin = global_point_now + 1;
			this->end = global_point_now + 2;
			global_point_now += 2;
			addEdge(this->begin, a.begin, 0);
			addEdge(a.end, this->end, 0);
			addEdge(this->begin, this->end, 0);
			addEdge(a.end, a.begin, 0);
		}

	}
	void able(){
		addEdge(this->begin, this->end, 0);
	}
};
bool flag[POINT_MAX];

class MatchRule{
	bool type = 0;
	bool reverse = 0;
	char l = 0;
	char r = 0;
public:
	MatchRule(bool _reverse,char _l,char _r) :
		type(0), reverse(_reverse),l(_l),r(_r){}
	MatchRule(bool _reverse, char _v) :
		type(1), reverse(_reverse), l(_v){}
	bool Match(const char &a){
		if (type) return reverse ^ (a == l);
		else return reverse ^ (a >= l && a <= r);		
	}
};

class Reg{
public:
	Reg(string _reg) :reg(_reg){}
	Reg() : Reg(""){}
	bool init(){
		memset(NFAaccept, 0, sizeof(NFAaccept));

		//Preserved Rules Initializing
		this->checkList.emplace(__LetterNumber, vector<MatchRule>{MatchRule(0, 'a', 'z'), MatchRule(0, 'A', 'Z'), MatchRule(0, '0','9')});		
		this->checkList.emplace(__SmallLetter, vector<MatchRule>{MatchRule(0, 'a', 'z')});
		this->checkList.emplace(__BigLetter, vector<MatchRule>{MatchRule(0, 'A', 'Z')});
		this->checkList.emplace(__AllLetter, vector<MatchRule>{MatchRule(0, 'a', 'z'), MatchRule(0, 'A', 'Z')});
		this->checkList.emplace(__Number, vector<MatchRule>{MatchRule(0, '0', '9')});
		this->checkList.emplace(__AllChar, vector<MatchRule>{MatchRule(0, 1, 127)});
		this->checkList.emplace(__Space, vector<MatchRule>{MatchRule(0, ' ')});

		return true;
	}
	bool addRule(string x, int code){
		reg = x;
		if (this->check()){
			this->toENFA();
			NFA.addEdge(0, NFA.begin, 0);
			NFAaccept[NFA.end] = code;
			return 1;
		}
		else return 0;
	}
	bool toENFA(){
		vector<Graph> s; s.reserve(POINT_MAX);
		vector<ope> o; o.reserve(POINT_MAX);
		auto pop = [&s, &o](){
			switch (*(o.end() - 1)){
			case ope::and:
				*(s.end() - 2) = Graph(0, *(s.end() - 2), *(s.end() - 1));
				s.pop_back();
				break;
			case ope::or:
				*(s.end() - 2) = Graph(1, *(s.end() - 2), *(s.end() - 1));
				s.pop_back();
				break;
			case ope::rep:
				*(s.end() - 1) = Graph(0, *(s.end() - 1));
				break;
			case ope::prep:
				*(s.end() - 1) = Graph(1, *(s.end() - 1));
				break;
			case ope::able:
				(s.end() - 1)->able();
				break;
			}
			o.pop_back();
		};
		for (int i = 0; i <= reg.length() - 1; i++){
			switch (reg[i]){
			case '[':
				//waiting for modifing
			case '|':
				while (!o.empty() && *(o.end() - 1) != ope::left)pop();
				o.emplace_back(ope::or);
				break;
			case '(':
				o.emplace_back(ope::left);
				break;
			case ')':
				while (!o.empty() && *(o.end() - 1) != ope::left)pop();
				o.pop_back();
				if (i != reg.length() - 1 && reg[i + 1] != ')' && reg[i + 1] != '+'&&reg[i + 1] != '?'&&reg[i + 1] != '*'&&reg[i + 1] != '|'){
					o.emplace_back(ope::and);
				}
				break;
			case '?':
				o.emplace_back(ope::able);
				pop();
				if (i != reg.length() - 1 && reg[i + 1] != ')' && reg[i + 1] != '+'&&reg[i + 1] != '?'&&reg[i + 1] != '*'&&reg[i + 1] != '|'){
					o.emplace_back(ope::and);
				}
				break;
			case '*':
				o.emplace_back(ope::rep);
				pop();
				if (i != reg.length() - 1 && reg[i + 1] != ')' && reg[i + 1] != '+'&&reg[i + 1] != '?'&&reg[i + 1] != '*'&&reg[i + 1] != '|'){
					o.emplace_back(ope::and);
				}
				break;
			case '+':
				o.emplace_back(ope::prep);
				pop();
				if (i != reg.length() - 1 && reg[i + 1] != ')' && reg[i + 1] != '+'&&reg[i + 1] != '?'&&reg[i + 1] != '*'&&reg[i + 1] != '|'){
					o.emplace_back(ope::and);
				}
				break;
			case '\\':
				i++;
				switch (reg[i]){
					//escaped Chars
				case 'w':
					reg[i] = __LetterNumber;break;
				case 'b':
					reg[i] = __BigLetter;break;
				case 's':
					reg[i] = __SmallLetter;break;
				case 'd':
					reg[i] = __Number;break;
				case 'p':
					reg[i] = __Space; break;
				case 'a':
					reg[i] = __AllChar; break;
				case 'l':
					reg[i] = __AllLetter; break;
				}				
			default:
				s.emplace_back(reg[i]);
				if (i != reg.length() - 1 && reg[i + 1] != ')' && reg[i + 1] != '+'&&reg[i + 1] != '?'&&reg[i + 1] != '*'&&reg[i + 1] != '|'){
					o.emplace_back(ope::and);
				}
				break;
			}

		}
		while (!o.empty())pop();
		this->NFA = *(s.end() - 1);
		POINT = global_point_now;
		return 1;
	}
	bool toNFA(){
		bitset<POINT_MAX> able;
		able[0] = true;
		//find the points set that can arrive
		for (int k = 0; k <= POINT; k++){
			for (edge *i = e[k]; i != nullptr; i = i->prev){
				if (i->v != 0)able[i->t] = 1;
			}
		}
		//add edge from rootPoint to the point that can reach through epsilon
		for (int k = 0; k <= POINT; k++){
			if (able[k]){
				dfsAddEdge(k, k);
			}
		}
		//recursive delete edge that are epsilon
		for (int k = 0; k <= POINT; k++){
			if (!able[k]){
				dfsDeleteEpsilonEdge(e[k]);
				e[k] = nullptr;
			}
		}
		//delete points that cant reach 
		for (int k = 0; k <= POINT; k++){
			for (edge *i = e[k]; i != nullptr; i = i->prev){
				while (i->prev != nullptr && i->prev->v == 0){
					edge *t = i->prev;
					i->prev = i->prev->prev;
					delete t;
				}
			}
			while (e[k] != nullptr && e[k]->v == 0){
				edge *t = e[k];
				e[k] = e[k]->prev;
				delete t;
			}
		}
		//compress the points set
		//there codes are need to be optimist
		int fx[POINT_MAX], gx[POINT_MAX], now = -1;
		memset(fx, 0, sizeof(fx));
		memset(gx, 0, sizeof(gx));
		for (int i = 0; i <= POINT; i++){
			if (able[i] == true)
				fx[++now] = i;
		}
		for (int k = 0; k <= now; k++){
			e[k] = e[fx[k]];
		}
		for (int k = 0; k <= now; k++){
			NFAaccept[k] = NFAaccept[fx[k]];
		}
		for (int k = now; k >= 0; k--){
			gx[fx[k]] = k;
		}

		POINT = now;
		for (int k = 0; k <= POINT; k++){
			for (edge *i = e[k]; i != nullptr; i = i->prev){
				i->t = gx[i->t];
			}
		}
		NFA.begin = gx[0];
		return 0;
	}
	int toDFA(){
		queue<pair<set<int>, int>> q;
		map<set<int>, int> m;
		set<int> t;
		int now = 0;
		t.insert(NFA.begin);
		m.emplace(t, 0);
		q.emplace(t, 0);
		while (!q.empty()){
			set<char> ss;
			auto t = q.front();
			q.pop();
			//get the convertion set
			for (auto &i : t.first)
			for (edge *j = e[i]; j != nullptr; j = j->prev)
				ss.emplace(j->v);
			//get target set
			for (auto &x : ss){
				set<int> tt;
				int acc = 0;
				for (auto &i : t.first)
				for (edge *j = e[i]; j != nullptr; j = j->prev)
				if (x == j->v)
					tt.emplace(j->t);
				//checking duplicate		
				auto temp = m.find(tt);
				if (temp == m.end()){
					q.emplace(tt, ++now);
					m.emplace(tt, now);
					DFA.addEdge(t.second, now,  x);
				}
				else{
					DFA.addEdge(t.second, temp->second, x);
				}

			}
		}
		//accept adjustment
		memset(DFAaccept, 0, sizeof(DFAaccept));
		for (auto &x : m){
			int acc = 0;
			for (auto &y : x.first){
				if (NFAaccept[y]){
					if (!acc) acc = NFAaccept[y];
					else return -1;
				}
			}
			DFAaccept[x.second] = acc;
		}
		return 0;
	}
	bool acceptChar(const char &c,const char &ruleChar){
		if (ruleChar >= 0) return c == ruleChar;
		else{
			bool result = 0;
			auto &rules = checkList[ruleChar];
			for (auto &rule : rules){
				result |= rule.Match(c);
			}
			return result;
		}
	}
	int accept(string str, bool greedy, bool shift, bool(*callback)(int code)){
		for (unsigned k = 0; k <= str.length() - 1; k++){
			int g = k, now = 0, onNFAaccept = -1;
			while (true){
				int t = -1;
				//accept char finding
				for (edge *i = de[now]; i != nullptr; i = i->prev){
					if (acceptChar(str[g], i->v)){
						t = i->t;
						g++;
						break;
					}
				}
				//accept checking
				if (t != -1) now = t;
				else{
					if (greedy && onNFAaccept != -1){
						//greedy accept
						callback(onNFAaccept);
						if (shift){
							k = g - 1;
						}
					}
					break;
				}
				if (greedy) { if (DFAaccept[now])onNFAaccept = DFAaccept[now]; }
				else
				if (DFAaccept[now]){
					//ungreedy accept
					callback(DFAaccept[now]);
					if (shift){
						k = g - 1;
						break;
					}
				}
			}
		}
		return 0;
	}
private:
	void dfsAddEdge(int &rootPoint, int currentPoint){
		//transfer accept value
		if (NFAaccept[currentPoint])NFAaccept[rootPoint] = NFAaccept[rootPoint];
		//add edge from rootPoint to the point that can reach through epsilon
		for (edge *i = e[currentPoint]; i != nullptr; i = i->prev){
			if (i->v != 0) NFA.addEdge(rootPoint, i->t, i->v);
			else dfsAddEdge(rootPoint, i->t);
		}
	}
	void dfsDeleteEpsilonEdge(edge *currentEdge){
		//recursive delete edge that are epsilon
		//not exist edge
		if (currentEdge == nullptr)return;
		//front edge
		if (currentEdge->prev != nullptr) dfsDeleteEpsilonEdge(currentEdge->prev);
		delete currentEdge;
	}
	bool check(){
		//binary operator rule check
		for (auto i = 0; i <= reg.length() - 1; i++){
			if (reg[i] == '\\'&&i == reg.length() - 1)return 0;
			if (reg[i] == '\\')i += 2;
			if (i == reg.length() - 1)break;
			if (reg[i] == '|')
			if (reg[i + 1] == '|' || reg[i + 1] == '*'
				|| reg[i + 1] == '?' || reg[i + 1] == ')')return 0;
			if (reg[i] == '*' || reg[i] == '?')
			if (i == 0)return 0;
			if (reg[i] == '(')
			if (reg[i + 1] == ')' || reg[i + 1] == '|' || reg[i + 1] == '*' || reg[i + 1] == '?')return 0;
		}
		//backet match check
		int now = 0;
		for (auto i = 0; i <= reg.length() - 1; i++){
			if (reg[i] == '(')now++;
			if (reg[i] == ')')now--;
			if (now < 0)return 0;
		}
		if (now != 0)return 0;
		return 1;
	}
	int POINT;
	string reg;
	
	Dfa DFA;
	map<int, vector<MatchRule>> checkList;
	public:
Graph NFA;
	int NFAaccept[POINT_MAX];
	int DFAaccept[POINT_MAX];
};

/*
The Zurl Regular Engine Defination

PreDefine Command

PART DEFINE


fundaments

a //match a which is not specials
\\ // match \
a|b //a or b
ab //a and b
a* //a = 0-oo
a+ //a = 1-oo
a? //a = 0-1

\p space

\b A-Z
\s a-z
\l A-Za-z
\d 0-9
\w a-zA-Z0-9
\a all character

extension grammer
wait for do

predo command

waiting to add

[abcde-f] //match all
[^abcde] //match none

@(aaaaa) = (aaaaa)?(aaaaa)?(aaaaa)?(aaaaa)?(aaaaa)?




Lex analyser Defination v1.0

= is command order
$ is comments line

key val
=lexer
name fucking
version 1
=def
_D (\d+(.\d+)?)
=rule
$hello every one fucking your mother
the code begin
dight _D
id \l\w*


*/
