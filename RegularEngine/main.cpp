#include <iostream>
#include "Lex.h"

int main(){
	/*
	Reg reg;
	string str = "fuckifuckifelsefuck123456";
	reg.init();
	//reg.addRule("if", 1);
	reg.addRule("else", 2);
	reg.addRule("i\\w", 3);
	reg.toNFA();
	reg.toDFA();
	dddfs(reg, 0);
	reg.accept(str, false, false, [](int code) -> bool{
		cout << "Accept a token" << code << endl;
		return 1;
	});
	cout << "welcome to the regular engine !!!! hello every" << endl;
	*/
	Lex lex;
	lex.ReadFromFile("zurl.lex");
	return 0;
}