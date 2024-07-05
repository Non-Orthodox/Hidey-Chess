#include "file_utilities.hpp"
#include "settings.h"

// TODO: Sanitize file paths

std::string scriptNameToFileName(std::string scriptName) {
	auto scriptsDirectory = (*g_settings)[settingEnum_scripts_directory]->getString();
	return scriptsDirectory + "/" + scriptName + ".dl";
}

std::string assetNameToFileName(std::string fileName) {
	auto gameRootDirectory = (*g_settings)[settingEnum_game_root_directory]->getString();
	return gameRootDirectory + "/" + fileName;
}
