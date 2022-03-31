
#include "log.h"
#include <iostream>

void log_debug(const char function[], const int line, std::string m) {
	if ((*g_settings)[settingEnum_log_level]->getInt() > 4) {
		std::cout << COLOR_WHITE "Debug: " COLOR_BLUE "(" << function << ":" << line << ")" COLOR_NORMAL " " << m << std::endl;
	}
}

void log_info(const char function[], const int line, std::string m) {
	if ((*g_settings)[settingEnum_log_level]->getInt() > 3) {
		std::cout << COLOR_GREEN "Info: " COLOR_BLUE "(" << function << ":" << line << ")" COLOR_NORMAL " " << m << std::endl;
	}
}

void log_warning(const char function[], const int line, std::string m) {
	if ((*g_settings)[settingEnum_log_level]->getInt() > 2) {
		std::cout << COLOR_MAGENTA "Warning: " COLOR_BLUE "(" << function << ":" << line << ")" COLOR_NORMAL " " << m << std::endl;
	}
}

void log_error(const char function[], const int line, std::string m) {
	if ((*g_settings)[settingEnum_log_level]->getInt() > 1) {
		std::cout << COLOR_YELLOW "Error: " COLOR_BLUE "(" << function << ":" << line << ")" COLOR_NORMAL " " << m << std::endl;
	}
}

void log_critical_error(const char function[], const int line, std::string m) {
	if ((*g_settings)[settingEnum_log_level]->getInt() > 0) {
		std::cout << COLOR_RED "Critical error: " COLOR_BLUE "(" << function << ":" << line << ")" COLOR_NORMAL " " << m << std::endl;
	}
}
