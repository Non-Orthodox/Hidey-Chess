#define SDL_MAIN_HANDLED

#include <cstddef>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <memory>
#include <fstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "settings.h"
#include "defer.hpp"
#include "types.h"
#include "log.h"
#include "duck-lisp.hpp"
#include "scripting.hpp"
#include "render_window.hpp"
#include "repl.hpp"

SettingsList *g_settings;

int main_printHelp(Setting *setting) {
	// Prevent infinite recursion ahahaha.
	if (setting->getString() != "") {
		setting->set("");
		return 0;
	}
	std::cout << "Help:" << std::endl;
	std::cout << "Variables can be set on the command line using \"--variable=value\" notation." << std::endl;
	std::cout << "Settings {" << std::endl;
	for (auto &setting: *g_settings) {
		std::cout << setting.name << " = " << setting.prettyPrint() << std::endl;
	}
	std::cout << "}" << std::endl;
	return 0;
}

int main_saveSettings(Setting *setting) {
	if (setting->getString() != "") {
		setting->set("");
		return 0;
	}
	info("Saving settings.");
	std::cout << "Saving settings.";
	auto saveFileName = (*g_settings)[settingEnum_config_file]->getString();
	std::ofstream saveFileStream(saveFileName);
	for (auto &setting: *g_settings) {
		if (!setting.save) continue;
		saveFileStream << setting.serialize() << std::endl;
	}
	std::cout << std::endl;
	return 0;
}

int main_replEnvironment(Setting *setting) {
	std::string &value = *setting->valueString;
	if ((value != "config") && (value != "game")) {
		value = "config";
	}
	return 0;
}

int main_switchReplEnvironment(Setting *setting) {
	*setting->valueString = "";
	{
		Setting &setting = *(*g_settings)[settingEnum_repl_environment];
		std::string &value = *setting.valueString;
		if (value == "config") {
			value = "game";
		}
		else {
			value = "config";
		}
		std::cout << value << std::endl;
	}
	return 0;
}

int main_loadSettings(std::shared_ptr<DuckLisp> duckLisp, std::shared_ptr<DuckVM> duckVM) {
	(*g_settings)[settingEnum_save]->callback = main_saveSettings;
	std::string configFileName = (*g_settings)[settingEnum_config_file]->getString();
	if (configFileName == "") return 0;
	std::ifstream configFileStream(configFileName);
	std::stringstream configSs;
	configSs << configFileStream.rdbuf();
	std::string configString = "(()\n" + configSs.str() + ")";
	return eval(duckVM, duckLisp, configString);
}

void main_initializeSettings() {
	// Initialize settings array.
	// 2024: Why is this dynamic?
	g_settings = new SettingsList();

	// Register settings.
#define ENTRY(ENTRY_name, ENTRY_value, ENTRY_lock, ENTRY_save) g_settings->insert(#ENTRY_name, ENTRY_value);
		SETTINGS_LIST
#undef ENTRY

	// Mark some settings as unsaveable (e.g. commands).
	{
		bool save = true;
		bool dont_save = false;
#define ENTRY(ENTRY_name, ENTRY_value, ENTRY_lock, ENTRY_save) \
			(*g_settings)[settingEnum_##ENTRY_name]->save = ENTRY_save;
			SETTINGS_LIST
#undef ENTRY
	}

	// Bind callbacks
	(*g_settings)[settingEnum_help]->callback = main_printHelp;
	(*g_settings)[settingEnum_switch_repl_environment]->callback = main_switchReplEnvironment;
	(*g_settings)[settingEnum_repl_environment]->callback = main_replEnvironment;
}

void main_parseCommandLineArguments(int argc, char *argv[]) {
	// Define option aliases.
#	define ENTRY(ENTRY_letter, ENTRY_name) ENTRY_letter,
	char letterOptions[] = {
		SETTINGS_ALIAS_LIST
	};
#	undef ENTRY

#	define ENTRY(ENTRY_letter, ENTRY_name) #ENTRY_name,
	std::string optionsAliases[] = {
		SETTINGS_ALIAS_LIST
	};
#	undef ENTRY

	/*
	Starting at zero might be wrong, but the program name shouldn't be
	recognized as an option anyway. I suppose the program name could be renamed
	so that it is recognized.
	*/
	std::string arg;
	std::string var;
	std::string value;
	ptrdiff_t equalsIndex = 0;
	Setting *setting;
	for (int i = 0; i < argc; i++) {
		arg = argv[i];
		// Check to see if argument is an option.
		if ((arg.length() < 2) || (arg.compare(0, 1, "-") && arg.compare(0, 2, "--"))) {
			// No options. Continue.
			continue;
		}
		
		// Process option.
		if (!arg.compare(0, 2, "--")) {
			if (arg.length() < 3) {
				continue;
			}
			
			arg = arg.substr(2);
			
			equalsIndex = arg.find_first_of('=');
			var = arg.substr(0, equalsIndex);

			if ((size_t) equalsIndex == std::string::npos) {
				value = "";
			}
			else {
				if (arg.length() <= (size_t) equalsIndex + 1) {
					continue;
				}
				value = arg.substr(equalsIndex + 1);
			}
		}
		else if (!arg.compare(0, 1, "-")) {
			arg = arg.substr(1);
			var = arg.substr(0, 1);
			value = arg.substr(1);
			
			// Find setting for single character alias.
			for (std::ptrdiff_t j = 0; (size_t) j < sizeof(letterOptions)/sizeof(char); j++) {
				if (letterOptions[j] == var[0]) {
					var = optionsAliases[j];
					break;
				}
			}
		}
		
		// Find setting.
		try {
			setting = (*g_settings)[var];
			setting->setFromString(value);
		}
		catch (std::logic_error& e) {
			// It's really just fine.
			error("Couldn't find setting \"" + var + "\".");
			continue;
		}
	}

	// Lock some settings to prevent the user from crashing the engine.
#define ENTRY(ENTRY_name, ENTRY_value, ENTRY_lock, ENTRY_save) (*g_settings)[settingEnum_##ENTRY_name]->ENTRY_lock();
	SETTINGS_LIST
#undef ENTRY
}

int main_loadAutoexec(std::shared_ptr<DuckLisp> duckLisp, std::shared_ptr<DuckVM> duckVM) {
	std::string configFileName = (*g_settings)[settingEnum_autoexec_file]->getString();
	if (configFileName == "") return 0;
	std::ifstream configFileStream(configFileName);
	std::stringstream configSs;
	configSs << configFileStream.rdbuf();
	// '\n' for readability when printing.
	std::string configString = "(()\n" + configSs.str() + ")";
	return eval(duckVM, duckLisp, configString);
}

int main (int argc, char *argv[]) {
	main_initializeSettings();

	log_init();

	// Prevent config files from changing these.
	(*g_settings)[settingEnum_config_compiler_heap_size]->lock();
	(*g_settings)[settingEnum_config_vm_heap_size]->lock();
	(*g_settings)[settingEnum_config_vm_max_objects]->lock();

	// This compiler and VM will be used to read config files and run a jank REPL.
	std::shared_ptr<DuckLisp> configCompiler(new DuckLisp((*g_settings)[settingEnum_config_compiler_heap_size]->getInt()
														  * sizeof(dl_uint8_t)));
	std::shared_ptr<DuckVM> configVm(new DuckVM(((*g_settings)[settingEnum_config_vm_heap_size]->getInt()
												 * sizeof(dl_uint8_t)),
												((*g_settings)[settingEnum_config_vm_max_objects]->getInt()
												 * sizeof(dl_uint8_t))));
	registerCallback(configVm, configCompiler, "print", "(I)", script_callback_print);
	registerCallback(configVm, configCompiler, "setting-get", "(I)", script_callback_get);
	registerCallback(configVm, configCompiler, "setting-set", "(I I)", script_callback_set);

	// Precedence is "config.dl", "autoexec.dl", then CLI. The last change is the one that applies.
	main_loadSettings(configCompiler, configVm);
	main_loadAutoexec(configCompiler, configVm);
	main_parseCommandLineArguments(argc, argv);

	Repl repl{};

	std::shared_ptr<DuckLisp> gameCompiler(new DuckLisp((*g_settings)[settingEnum_game_compiler_heap_size]->getInt()
														 * sizeof(dl_uint8_t)));
	std::shared_ptr<DuckVM> gameVm(new DuckVM(((*g_settings)[settingEnum_game_vm_heap_size]->getInt()
											   * sizeof(dl_uint8_t)),
											  ((*g_settings)[settingEnum_game_vm_max_objects]->getInt()
											   * sizeof(dl_uint8_t))));
	registerCallback(gameVm, gameCompiler, "print", "(I)", script_callback_print);
	registerCallback(gameVm, gameCompiler, "setting-get", "(I)", script_callback_get);


	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << "\n";
		return 1;
	}
	defer(SDL_Quit());
	
	if (!(IMG_Init(IMG_INIT_PNG))) {
		std::cout << "IMG_Init Error: " << SDL_GetError() << "\n";
		return 1;
	}
	defer(IMG_Quit());

	RenderWindow window("GAME v0.01",
						(*g_settings)[settingEnum_window_width]->getInt(),
						(*g_settings)[settingEnum_window_height]->getInt());
	// std::cout << window.getRefreshRate() << "\n";

	//Variables used for main while loop
	// j: This variable could be set to false using a DL callback. Maybe in both the config and game VMs.
	bool gameRunning = true;
	SDL_Event event;
	gameState GAME_STATE = SINGLEPLAYER;

	SDL_Texture* testTexture = window.loadTexture("../res/chess/images/knight.png");
	SDL_Rect dstrect;
	dstrect.x = 400;
	dstrect.y = 100;
	dstrect.w = 512;
	dstrect.h = 512;

	while(gameRunning)
	{
		switch(GAME_STATE)
		{
			case MAIN_MENU:
				// gameRunning = !MM_EventHandle(&event, window, renderer, &GAME_STATE, p1Color, p2Color, &boardButtons, &guiButtons, boardWidth, boardHeight);
				// GAME_STATE = SINGLEPLAYER;
				if (GAME_STATE != MAIN_MENU) {
					/* j: Compile "main.dl" for the selected game type.
						  Execute the compiled bytecode a single time. */
				}
				if (GAME_STATE == SINGLEPLAYER) {
					debug("Now in Singleplayer");
					//! INIT GAME
					// j: Call the "init" global function if it exists.
				}
				else if (GAME_STATE == MULTIPLAYER) {
					debug("Now in Multiplayer");
					//! INIT GAME
					// j: Call the "init" global function if it exists.
				}
				break;

			case MULTIPLAYER:
				// j: Every iteration a series of DL functions will be called. You will decide what those functions are.
				// gameRunning = !MP_EventHandle(&event, window, renderer, &GAME_STATE, p1Color, p2Color, &boardButtons, boardWidth, boardHeight);
				break;

			case SINGLEPLAYER:
				// j: Every iteration a series of DL functions will be called. You will decide what those functions are.
				// gameRunning = !SP_EventHandle(&event, window, renderer, &GAME_STATE, p1Color, p2Color, &boardButtons, boardWidth, boardHeight);
				break;

			//console mode is for testing the game without using graphics
			case CONSOLE_MODE:
				break;

			case QUIT_STATE:
				gameRunning = 0;
				break;

			default:
				break;
		}

		// Temporary event handler
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				gameRunning = false;
		}

		window.clear();
		window.render(testTexture, dstrect);
		window.display();

		// funcall(configVm, configCompiler, "sum", 3);

		if ((*g_settings)[settingEnum_repl_environment]->getString() == "config") {
			repl.repl_nonblocking(configCompiler, configVm);
		}
		else if ((*g_settings)[settingEnum_repl_environment]->getString() == "game") {
			repl.repl_nonblocking(gameCompiler, gameVm);
		}
		else {
			// Don't freeze the REPL when the REPL setting is wrong.
			repl.repl_nonblocking(configCompiler, configVm);
		}

		// Garbage collect every frame to prevent unexpected pauses? (pause *every* time, not randomly)
		if (configVm->garbageCollect()) {
			error("Config VM garbage collection failed");
			return 1;
		}
		if (gameVm->garbageCollect()) {
			error("Game VM garbage collection failed");
			return 1;
		}
	}

	window.CleanUp();

	return 0;
}
