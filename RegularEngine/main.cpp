#include<iostream>
#include<memory>
#include<vector>
#include<unordered_set>
#include<queue>
#include<set>
#include<bitset>
#include<tuple>
#include<cstdlib>
#include<map>
using namespace std;
enum class ope{
	and,or,left,rep,able,prep
};
const int POINT_MAX = 50000;
int global_point_now = 0;
const int REV_RANGE = 17;
const int POS_RANGE = 18;

class edge{
public:
	edge *prev;
	int t;
	char v,l,r;
	edge() = default;
	edge(int _t, char _v, edge *_prev) :
		t(_t), v(_v),l(0),r(0),prev(_prev){}
	edge(int _t, char _v,char _l,char _r, edge *_prev) :
		t(_t), v(_v),l(_l),r(_r),prev(_prev){}
};edge *e[POINT_MAX], *de[POINT_MAX];
class Dfa{
public:
	void addEdge(int f, int t, char v){
		de[f] = new edge(t, v, de[f]);
	}
	void addEdge(int f, int t, char l, char r, char v){
		de[f] = new edge(t, v, l, r, de[f]);
	}
};
class Graph{
public:	
	int begin, end;
	Graph() = default;
	void addEdge(int f, int t, char v){
		e[f] = new edge(t, v, e[f]);
	}
	void addEdge(int f, int t, char l, char r, char v){
		e[f] = new edge(t, v, l, r, e[f]);
	}
	Graph(char x){
		addEdge(global_point_now + 1, global_point_now + 2,x);
		this->begin = global_point_now + 1;
		this->end = global_point_now + 2;
		global_point_now += 2;
	}
	Graph(char l,char r,char v){
		addEdge(global_point_now + 1, global_point_now + 2, l,r,v);
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
			addEdge(a.end, b.begin,0);
		}
	}
	Graph(bool plan,Graph &a){
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
		addEdge(this->begin,this->end, 0);
	}
};
bool flag[POINT_MAX];
class Reg{
public:
	Reg(string _reg) :reg(_reg){}
	Reg() : Reg(""){}
	bool init(){
		memset(NFAaccept, 0, sizeof(NFAaccept));
		return true;
	}
	bool addRule(string x,int code){
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
			switch (*(o.end()-1)){
				case ope::and:
					*(s.end() - 2) = Graph(0, *(s.end() - 2), *(s.end() - 1));
					s.pop_back();
					break;
				case ope::or:
					*(s.end() - 2) = Graph(1, *(s.end() - 2), *(s.end() - 1));
					s.pop_back();
					break;
				case ope::rep:
					*(s.end() - 1) = Graph(0,*(s.end() - 1));
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
		for (int i = 0;i<=reg.length()-1;i++){
			switch (reg[i]){
				case '[':
					if (reg[i + 1] == '^'){
						s.emplace_back(reg[i + 2], reg[i + 4], REV_RANGE);
						i += 5;
						if (i == reg.length() || reg[i + 1] != ')' && reg[i + 1] != '+'&&reg[i + 1] != '?'&&reg[i + 1] != '*'&&reg[i + 1] != '|'){
							o.emplace_back(ope::and);
						}
						break;
					}
					else{
						s.emplace_back(reg[i + 1], reg[i + 3], POS_RANGE);
						i += 4;
						if (i == reg.length() || reg[i + 1] != ')' && reg[i + 1] != '+'&&reg[i + 1] != '?'&&reg[i + 1] != '*'&&reg[i + 1] != '|'){
							o.emplace_back(ope::and);
						}
						break;
					}
				break;
				case '|':
					while (!o.empty() && *(o.end() - 1) != ope::left )pop();
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
				default:
					s.emplace_back(reg[i]);
					if (i!= reg.length()-1 && reg[i + 1] != ')' && reg[i + 1] != '+'&&reg[i + 1] != '?'&&reg[i + 1] != '*'&&reg[i + 1] != '|'){
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
		for (int k = 0; k <= POINT; k++){
			for (edge *i = e[k]; i != nullptr; i = i->prev){
				if (i->v != 0)able[i->t] = 1;
			}
		}
		for (int k = 0; k <= POINT; k++){
			if (able[k]){
				dfs(k,k);
			}
		}
		for (int k = 0; k <= POINT; k++){
			if (!able[k]){
				dfsdel(e[k]);
				e[k] = nullptr;
			}
		}
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
		//simplify
		int fx[POINT_MAX], gx[POINT_MAX], now = -1; 
		memset(fx, 0, sizeof(fx));
		memset(gx, 0, sizeof(gx));
		for (int i=0; i <= POINT; i++){
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
		queue<pair<set<int>,int>> q;
		map<set<int>,int> m;
		set<int> t;
		int now = 0;
		t.insert(NFA.begin);
		m.emplace(t,0);
		q.emplace(t,0);
		
		while (!q.empty()){
			set<tuple<char, char, char>> ss;
			auto t = q.front();
			q.pop();
			//get the convertion set
			for (auto &i : t.first)
					for (edge *j = e[i]; j!= nullptr; j = j->prev)
						if (j->v != 0)
							ss.emplace(j->v, j->l, j->r);	
			//get target set
			for (auto &x : ss){
				set<int> tt;
				int acc = 0;
				for (auto &i : t.first)
						for (edge *j = e[i]; j != nullptr; j = j->prev)
							if (std::get<0>(x)==j->v
								&& std::get<1>(x)==j->l
								&& std::get<2>(x)==j->r)
									tt.emplace(j->t);		
				//checking duplicate		
				auto temp = m.find(tt);
				if (temp == m.end()){
					q.emplace(tt,++now);
					m.emplace(tt,now);
					DFA.addEdge(t.second, now, get<1>(x), get<2>(x), get<0>(x));
				}
				else{
					DFA.addEdge(t.second,temp->second, get<1>(x), get<2>(x), get<0>(x));
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
	int accept(string str,bool greedy,bool shift){
		for (int k = 0; k <= str.length() - 1; k++){
			int g = k, now = 0, onNFAaccept = -1;
			while (true){
				int t = -1;
				//accept char finding
				for (edge *i = de[now]; i != nullptr; i = i->prev){
					if (checkChar(i, str[g])){
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
						cout << "Accept "  << onNFAaccept  << " at : " << k << " to " << g - 1 << endl;
						if (shift){
							k = g - 1;
						}
					}
					break;
				}
				if (greedy) { if (DFAaccept[now])onNFAaccept = now; }
				else 
				if (DFAaccept[now]){
					//ungreedy accept
					cout << "Accept "<<DFAaccept[now]<<" at : " << k << " to " << g - 1 << endl;
					if (shift){
						k = g-1;
						break;
					}	
				}						
			}
		}
		return 0;
	}
private:
	bool dfs(int &f, int x){
		if (NFAaccept[x])NFAaccept[f] = NFAaccept[x];
		for (edge *i = e[x]; i != nullptr; i = i->prev){
			if (i->v != 0){
				NFA.addEdge(f, i->t,i->l,i->r,i->v);
			}
			else{
				dfs(f, i->t);
			}
		}
		return 0;
	}
	bool dfsdel(edge *x){
		if (x == nullptr)return 0;
		if (x->prev != nullptr){
			dfsdel(x->prev);
		}
		delete x;
		return 0;
	}
	bool checkChar(edge *&a,char &b){
		if (a->v == REV_RANGE && (b <= a->l || b >= a->r)
			|| a->v == POS_RANGE && b >= a->l && b <= a->r
			|| a->v == b)return 1;
		return 0;
	}
	bool check(){
		//binary operator rule check
		for (int i = 0; i <= reg.length() - 1; i++){
			if (reg[i] == '\\'&&i == reg.length() - 1)return 0;
			if (reg[i] == '\\')i += 2;
			if (i == reg.length() - 1)break;
			if (reg[i] == '|')
				if (reg[i + 1] == '|' || reg[i + 1] == '*'
					|| reg[i + 1] == '?' || reg[i + 1] == ')')return 0;
			if (reg[i] == '*' || reg[i]=='?')
				if (i == 0)return 0;
			if (reg[i] == '(')
				if (reg[i + 1] == ')' || reg[i + 1] == '|' || reg[i + 1] == '*' || reg[i + 1] == '?')return 0;
			if (reg[i] == '['){
				if (reg[i + 1] == '^'){
					if (reg[i + 3] != '-' || reg[i + 5] != ']')return 0;
				}					
				else if(reg[i + 2] != '-' || reg[i + 4] != ']')return 0;
			}
			
		}
		//backet match check
		int now = 0;
		for (int i = 0; i <= reg.length() - 1; i++){
			if (reg[i] == '(')now++;
			if (reg[i] == ')')now--;
			if (now < 0)return 0;
		}
		if (now != 0)return 0;
		return 1;
	}
	int POINT;
	string reg;
	Graph NFA;
	Dfa DFA;
	
	int NFAaccept[POINT_MAX];
	int DFAaccept[POINT_MAX];
};

int main(){
	Reg reg;
	string str = "ifelsefuck123456";
	reg.init();
	reg.addRule("if", 1);
	reg.addRule("else", 2);
	reg.addRule("fuck", 3);
	reg.addRule("[0-9]*", 4);
	reg.toNFA();
	reg.toDFA();
	reg.accept(str,false,false);
	cout << "welcome to the regular engine" << endl;
	int a; cin >> a;
	return 0;
}
