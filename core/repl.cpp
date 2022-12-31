#include "repl.hpp"
#include <iostream>
#include "settings.h"

int main_callback_repl(Setting *setting) {
	if (!setting->getBool()) return 0;
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
	auto enterPressed = [](std::string line) -> bool {
		return ((line != "") && (line.back() == '\n'));
	};
	if (enterPressed(line)) {
		line = "";
	}
	while (true) {
		char character;
		std::streamsize length = std::cin.readsome(&character, 1);
		if (length == 0) break;
		line += character;
	}
	if (!enterPressed(line)) return "";
	return line;
}

void readSetting(const std::string line) {
	const std::string name = line.substr(1, line.length() - 2);
	Setting *setting;
	try {
		setting = g_settings->find(name);
		std::cout << setting->prettyPrint() << std::endl;
	}
	catch (const std::out_of_range &e) {}
}

void writeSetting(const std::string line) {
	// I hate C++ string processing.
	const std::string name = line.substr(1, line.find(' ') - 1);
	std::string value = line.substr(line.find(' '));
	value.substr(0, value.length() - 1);
	Setting *setting;
	try {
		setting = g_settings->find(name);
		setting->setFromString(value);
		std::cout << setting->prettyPrint() << std::endl;
	}
	catch (const std::out_of_range &e) {}
}

void execSetting(const std::string line) {
	const std::string name = line.substr(1, line.length() - 2);
	Setting *setting;
	try {
		setting = g_settings->find(name);
		setting->callback(setting);
	}
	catch (const std::out_of_range &e) {}
}

int Repl::repl_nonblocking(std::shared_ptr<DuckLisp> duckLisp, std::shared_ptr<DuckVM> duckVM) {
	int e = 0;
	if (!(*g_settings)[settingEnum_repl]->getBool()) return e;
	std::string line = getline_nonblocking();
	if (line == "") return e;
	enum class EntryType {
		read,
		write,
		exec,
		duckLisp,
	};
	auto entryType = [](const std::string line) -> EntryType {
		if (line[0] == '!') {
			return EntryType::exec;
		}
		if (line[0] != '\\') {
			return EntryType::duckLisp;
		}
		else if (line.find(' ') == std::string::npos) {
			return EntryType::read;
		}
		else {
			return EntryType::write;
		}
	};
	switch (entryType(line)) {
	case EntryType::read:
		readSetting(line);
		break;
	case EntryType::write:
		writeSetting(line);
		break;
	case EntryType::exec:
		execSetting(line);
		break;
	case EntryType::duckLisp:
		e = eval(duckVM, duckLisp, line);
		break;
	default:
		break;
	}
	std::cout << "> ";
	return e;
}
