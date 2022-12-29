
#include "log.h"
#include <iostream>
#include <cstdio>
#include <string>
#include "settings.h"

extern SettingsList *g_settings;

bool logRedirected = false;
FILE *logFile = nullptr;

int log_setting_callback(Setting *setting);
int log_file_callback(Setting *setting);

void log_init() {
	Setting *logLevelSetting = (*g_settings)[settingEnum_log_level];
	log_setting_callback(logLevelSetting);
	logLevelSetting->callback = log_setting_callback;

	Setting *logFileSetting = (*g_settings)[settingEnum_log_file];
	logFileSetting->callback = log_file_callback;

	std::string logFileName = (*g_settings)[settingEnum_log_file]->getString();
	if (logFileName != "") {
		info("Logging to \"" + logFileName + "\".");
		logFile = freopen(logFileName.c_str(), "w", stderr);
		logRedirected = true;
	}
}

int log_setting_callback(Setting *setting) {
	int value = setting->getInt();
	if ((value < 0) || (value > 5)) {
		int newValue = value;
		if (newValue < 0) newValue = 0;
		if (newValue > 5) newValue = 5;
		warning("Attempted to set log level to "
		        + std::to_string(value)
		        + ". Setting to "
		        + std::to_string(newValue)
		        + ".");
		setting->set(newValue);
	}
	return 0;
}

int log_file_callback(Setting *setting) {
	log_init();
	return 0;
}

void log(const char function[], const int line, std::string m, const char color[], const char logTypeName[], const int logLevel) {
	if ((*g_settings)[settingEnum_log_level]->getInt() >= logLevel) {
		std::cerr << (logRedirected ? "" : color)
		          << logTypeName
		          << ": "
		          << (logRedirected ? "" : COLOR_BLUE)
		          << "("
		          << function
		          << ":"
		          << line
		          << ")"
		          << (logRedirected ? "" : COLOR_NORMAL)
		          << " "
		          << m
		          << std::endl;
	}
}

void log_debug(const char function[], const int line, std::string m) {
	log(function, line, m, COLOR_WHITE, "Debug", 5);
}

void log_info(const char function[], const int line, std::string m) {
	log(function, line, m, COLOR_GREEN, "Info", 4);
}

void log_warning(const char function[], const int line, std::string m) {
	log(function, line, m, COLOR_MAGENTA, "Warning", 3);
}

void log_error(const char function[], const int line, std::string m) {
	log(function, line, m, COLOR_YELLOW, "Error", 2);
}

void log_critical_error(const char function[], const int line, std::string m) {
	log(function, line, m, COLOR_RED, "Critical error", 1);
}
