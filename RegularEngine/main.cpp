#include<iostream>
#include<memory>
#include<vector>
using namespace std;
enum class ope{
	and,or,left,rep,able
};
const int POINT_MAX = 1000000;
int global_point_now = 0;
class edge{
public:
	edge *prev;
	int t;
	char v,l,r;
	edge() = default;
	edge(int _t, char _v, edge *_prev) :
		t(_t), v(_v), prev(_prev){}
	edge(int _t, char _v,char _l,char _r, edge *_prev) :
		t(_t), v(_v),l(_l),r(_r),prev(_prev){}
}*e[POINT_MAX];
class Graph{
public:	
	int begin, end;
	Graph() = default;
	void addEdge(int f, int t, char v){
		e[f] = new edge(t, v, e[f]);
	}
	void addEdge(int f, int t, char l,char r,char v){
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
		this->begin = global_point_now + 1;
		this->end = global_point_now + 2;
		global_point_now += 2;
		addEdge(this->begin, a.begin, 0);
		addEdge(a.end, this->end, 0);
		addEdge(this->begin, this->end, 0);
		addEdge(a.end, a.begin, 0);
	}
	void able(){
		addEdge(this->begin,this->end, 0);
	}
};
bool flag[10000];
void dfs(int x){
	flag[x] = 1;
	cout << x << " : ";
	for (edge *i = e[x]; i != nullptr; i = i->prev){
		cout << i->t << " && " << i->v << "#";
	}
	cout << endl;
	for (edge *i = e[x]; i != nullptr; i = i->prev){
		if (!flag[i->t])
		dfs(i->t);
	}
}
class Reg{
public:
	Reg(string _reg) :reg(_reg){}
	Reg() : Reg(""){}
	bool check(){
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
				if (reg[i+1] == '^'){
					if (reg[i + 3] != '-' || reg[i + 5] != ']')return 0;
				}
				else{
					if (reg[i + 2] != '-' || reg[i + 4] != ']')return 0;
				}
			}
			
		}
		int now = 0;
		for (int i = 0; i <= reg.length() - 1; i++){
			if (reg[i] == '(')now++;
			if (reg[i] == ')')now--;
			if (now < 0)return 0;
		}
		if (now != 0)return 0;
		return 1;
	}
	bool toNFA(){
		vector<Graph> s; s.reserve(100000);
		vector<ope> o; o.reserve(100000);
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
						s.emplace_back(reg[i + 2], reg[i + 4], 17);
						i += 5;
						if (i == reg.length() || reg[i + 1] != ')' && reg[i + 1] != '+'&&reg[i + 1] != '?'&&reg[i + 1] != '*'&&reg[i + 1] != '|'){
							o.emplace_back(ope::and);
						}
						break;
					}
					else{
						s.emplace_back(reg[i + 1], reg[i + 3], 18);
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
					break;
				case '?':
					o.emplace_back(ope::able);
					pop();
					break;
				case '*':
					o.emplace_back(ope::rep);
					pop();
					break;
				case '\\':
					i++;
				default:
					s.emplace_back(reg[i]);
					if (i == reg.length() || reg[i + 1] != ')' && reg[i + 1] != '+'&&reg[i + 1] != '?'&&reg[i + 1] != '*'&&reg[i + 1] != '|'){
						o.emplace_back(ope::and);
					}
					break;
			}
			
		}
		while (!o.empty())pop(); 
		this->NFA = *(s.end() - 1);
		return 1; 
	}

private:
	string reg;
	Graph NFA, DFA;
};
/*def:ascii
17:a-z
18:A-Z
19:0-9
*/
int main(){
	Reg reg("[a-z]*");
	//Reg reg("(a|b)c");
	if (reg.check()){
		Graph g = reg.toNFA();
		dfs(g.begin);
	}
	cout << "welcome to the regular engine" << endl;
	int a; cin >> a;
	return 0;
}
