#include "duck-lisp/DuckLib/core.h"
#include "duck-lisp/DuckLib/memory.h"
#include <cstddef>
#define SDL_MAIN_HANDLED

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDLevents.h"
#include "piece.h"
#include "basicVisuals.h"
#include "settings.h"
#include "types.h"
#include "board.h"
#include "controller.h"
#include "gui.h"
#include "log.h"
extern "C" {
#include "duck-lisp/duckLisp.h"
}

SettingsList *g_settings;
duckLisp_t g_duckLisp;

int main_initDuckLisp(void) {
	int e = 0;

	void *hunk = malloc((*g_settings)[settingEnum_compiler_heap_size]->getInt() * sizeof(dl_uint8_t));
	if (!hunk) {
		e = dl_error_outOfMemory;
		return e;
	}

	e = duckLisp_init(&g_duckLisp,
					  hunk,
					  (*g_settings)[settingEnum_compiler_heap_size]->getInt() * sizeof(dl_uint8_t));
	return e;
}

void main_parseCommandLineArguments(int argc, char *argv[]) {

	// Initialize settings array.
	g_settings = new SettingsList();
	
	// Register settings.
#define ENTRY(ENTRY_name, ENTRY_value) g_settings->insert(#ENTRY_name, ENTRY_value);
		SETTINGS_LIST
#undef ENTRY

	// Define option aliases.
#   define ENTRY(ENTRY_letter, ENTRY_name) ENTRY_letter,
	char letterOptions[] = {
		SETTINGS_ALIAS_LIST
	};
#   undef ENTRY
	
#   define ENTRY(ENTRY_letter, ENTRY_name) #ENTRY_name,
	std::string optionsAliases[] = {
		SETTINGS_ALIAS_LIST
	};
#   undef ENTRY
	
	// Bind callbacks
#   define ENTRY(ENTRY_name, ENTRY_callback) g_settings->find(#ENTRY_name)->callback = ENTRY_callback;
	SETTINGS_CALLBACKS_LIST
#   undef ENTRY
	
	/*
	Starting at zero might be wrong, but the program name shouldn't be
	recognized as an option anyway. I suppose the program name could be renamed
	so that it is recognized.
	*/
	std::string arg;
	std::string var;
	std::string value;
	int equalsIndex = 0;
	bool tempBool = false;
	int tempInt = 0;
	float tempFloat = 0.0;
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
			
			if (equalsIndex == std::string::npos) {
				value = "";
			}
			else {
				if (arg.length() <= equalsIndex + 1) {
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
			for (std::ptrdiff_t j = 0; j < sizeof(letterOptions)/sizeof(char); j++) {
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
}

int main(int argc, char *argv[]){

	main_parseCommandLineArguments(argc, argv);
	
	// Don't use "board_width" and "board_height" after this. They could change, and that will mess a ton of stuff up.
	const int boardWidth = (*g_settings)[settingEnum_board_width]->getInt();
	const int boardHeight = (*g_settings)[settingEnum_board_height]->getInt();
	
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	if (!(*g_settings)[settingEnum_disable_sdl]->getBool()) {

		//Initializing SDL
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			critical_error("SDL failed to initialize | SDL_Error " + std::string(SDL_GetError()));
			return 1; //later on use an exception
		}
	
		//Initializing SDL_image
		if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) //can add additional image file types here
		{
			critical_error("SDL_image failed to initialize");
			return 1; //later on use an exception
		}
		
		//Creating Window
		window = SDL_CreateWindow("Hidey-Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_RESIZABLE);
		if (window == nullptr) 
		{
			critical_error("Window could not be created | SDL_Error " + std::string(SDL_GetError()));
			return 1;
		}
	
		//Creating Renderer
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer == nullptr) 
		{
			// std::cerr << "Error:  << std::endl;
			critical_error("Renderer could not be created | SDL_Error " + std::string(SDL_GetError()));
			return 1;
		}
		
	}

	//Setting team colors
	color_t p1Color, p2Color;
	p1Color.red = 0;
	p1Color.green = 0;
	p1Color.blue = 0;
	p2Color.red = 255;
	p2Color.green = 255;
	p2Color.blue = 255;
	

    //testing Section for images
    // SDL_Rect rect;
    // rect.h = rect.w = 50;
    // rect.x = rect.y = 100;
    // SDL_Surface* surface = IMG_Load("../res/knight.png");
    // int er;
    // er = SDL_SetSurfaceColorMod(surface, 0x00, 0x00, 0xFF);
    // SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_RenderCopy(renderer, texture, nullptr, &rect);

	//testing Section for images
	// SDL_Rect rect;
	// rect.h = rect.w = 50;
	// rect.x = rect.y = 100;
	// SDL_Surface* surface = IMG_Load("../res/image.png");
	// SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	// SDL_RenderCopy(renderer, texture, nullptr, &rect);




	//Variables used for main while loop
	SDL_Event event;
	int run = 1;
	int winWidth, winHeight; //used to store window dimensions
	gameState GAME_STATE = MAIN_MENU;
	board board(8,8);
	std::vector<Button> boardButtons;
	std::vector<Button> guiButtons;
	Setting *tempSetting;
	SDL_Rect tempRect;
	
	if (!(*g_settings)[settingEnum_disable_sdl]->getBool()) {
		SDL_GetWindowSize(window, &winWidth, &winHeight);
	}
	
	tempRect.w = winHeight/12;
	tempRect.h = winHeight/12;
	
	for (int y = 0; y < boardHeight; y++) {
		for (int x = 0; x < boardWidth; x++) {
			tempSetting = new Setting("x" + std::to_string(x) + "y" + std::to_string(y), false);
			tempRect.x = winHeight/2 - boardWidth/2.0*tempRect.w + x*tempRect.w;
			tempRect.y = winHeight/2 - boardHeight/2.0*tempRect.h + y*tempRect.h;
			boardButtons.push_back(Button(tempSetting, renderer, tempRect));
			// Toggle switch.
			boardButtons.back().toggle = true;
			// Toggle on button release.
			boardButtons.back().toggleOnUp = false;
			// Colors.
			boardButtons.back().pressedColor = ((x ^ y) & 1) ? (color_t){0xBF, 0xBF, 0xBF} : (color_t) {0x40, 0x40, 0x40};
			boardButtons.back().releasedColor = ((x ^ y) & 1) ? (color_t){0xFF, 0xFF, 0xFF} : (color_t) {0x00, 0x00, 0x00};
		}
	}
	
	tempSetting = new Setting("singleplayer", false);
	tempRect.x = 50;
	tempRect.y = 50;
	guiButtons.push_back(Button(tempSetting, renderer, tempRect));
	guiButtons.back().pressedColor = (color_t){0x00, 0x7F, 0x00};
	guiButtons.back().releasedColor = (color_t){0x00, 0xBF, 0x00};
	
	tempSetting = new Setting("multiplayer", false);
	tempRect.x = 50;
	tempRect.y = 150;
	guiButtons.push_back(Button(tempSetting, renderer, tempRect));
	guiButtons.back().pressedColor = (color_t){0x00, 0x7F, 0x00};
	guiButtons.back().releasedColor = (color_t){0x00, 0xBF, 0x00};
	
	while(run)
	{
		switch(GAME_STATE)
		{
			case MAIN_MENU:
				
				run = !MM_EventHandle(&event, window, renderer, &GAME_STATE, p1Color, p2Color, &boardButtons, &guiButtons, boardWidth, boardHeight);
				// GAME_STATE = SINGLEPLAYER;
				if (GAME_STATE == SINGLEPLAYER) {
					debug("Now in Singleplayer");
					standardChessBoardInit(&board);
					printChessBoard(&board);
				}
				else if (GAME_STATE == MULTIPLAYER) {
					debug("Now in Multiplayer");
				}
				break;

			case MULTIPLAYER:
				run = !MP_EventHandle(&event, window, renderer, &GAME_STATE, p1Color, p2Color, &boardButtons, boardWidth, boardHeight);
				break;

			case SINGLEPLAYER:
				// renderBoard_button(boardButtons, boardWidth, boardHeight);
				// SDL_RenderPresent(renderer);

				//run = !SP_EventHandle(&event, window, renderer, &GAME_STATE, p1Color, p2Color, &boardButtons, boardWidth, boardHeight);
				testController(&board, &GAME_STATE);
				break;

			//console mode is for testing the game without using graphics
			case CONSOLE_MODE:
				break;

			case QUIT_STATE:
				run = 0;
				break;

			default:
				break;
		}

		if (!(*g_settings)[settingEnum_disable_sdl]->getBool()) {
			renderBoard_button(boardButtons, boardWidth, boardHeight);
			renderGui(guiButtons);
			SDL_RenderPresent(renderer);
		}
	}

	//Destroying and Quitting
	if (!(*g_settings)[settingEnum_disable_sdl]->getBool()) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		window = nullptr;
		IMG_Quit();
		SDL_Quit();
	}

	return 0;
}
