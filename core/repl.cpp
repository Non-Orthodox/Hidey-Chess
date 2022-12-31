#include "repl.hpp"
#include <iostream>
#include "settings.h"

int main_callback_repl(Setting *setting) {
	(void) setting;
	std::cout << "> ";
	return 0;
}

Repl::Repl() {
	std::ios_base::sync_with_stdio(false);
	auto replSetting = (*g_settings)[settingEnum_repl];
	replSetting->callback = main_callback_repl;
	main_callback_repl(replSetting);
}

Repl::~Repl() {

}

std::string getline_nonblocking() {
	static std::string line = "";
	auto entered = [](std::string line) {
		return ((line != "") && (line.back() == '\n'));
	};
	if (entered(line)) {
		line = "";
	}
	while (true) {
		char character;
		std::streamsize length = std::cin.readsome(&character, 1);
		if (length == 0) break;
		line += character;
	}
	if (!entered(line)) return "";
	return line;
}

int Repl::repl_nonblocking(std::shared_ptr<DuckLisp> duckLisp, std::shared_ptr<DuckVM> duckVM) {
	int e = 0;
	if (!(*g_settings)[settingEnum_repl]->getBool()) return e;
	std::string line = getline_nonblocking();
	if (line == "") return e;
	e = eval(duckVM, duckLisp, line);
	std::cout << "> ";
	return e;
}
