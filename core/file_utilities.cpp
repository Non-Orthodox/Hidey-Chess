#include "file_utilities.hpp"
#include "settings.h"

std::string scriptNameToFileName(std::string scriptName) {
	auto scriptsDirectory = (*g_settings)[settingEnum_scripts_directory]->getString();
	return scriptsDirectory + "/" + scriptName + ".dl";
}
