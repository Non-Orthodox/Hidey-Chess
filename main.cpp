#define SDL_MAIN_HANDLED

#include <iostream>
#include <string>
#include <stdexcept>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDLevents.h"
#include "piece.h"
#include "basicVisuals.h"
#include "settings.h"
#include "types.h"
#include "board.h"

#define SETTINGS_LIST \
    ENTRY("peer_ip_address", "localhost") \
    ENTRY("peer_network_port", 2850) \
    ENTRY("network_port", 2851) \
    ENTRY("set", "") \
    ENTRY("print", "") \
    ENTRY("echo", "") \
    ENTRY("==", "") \
    ENTRY("!=", "") \
    ENTRY("", "")

#define SETTINGS_ALIAS_LIST \
    ENTRY('a', "peer_ip_address") \
    ENTRY('p', "peer_network_port") \
    ENTRY('n', "network_port") \

#define SETTINGS_CALLBACKS_LIST \
    ENTRY("set", settings_callback_set) \
    ENTRY("print", settings_callback_print) \
    ENTRY("echo", settings_callback_echo) \
    ENTRY("==", settings_callback_equal) \
    ENTRY("!=", settings_callback_notEqual) \
    ENTRY("", settings_callback_chain)

SettingsList *g_settings;

void main_parseCommandLineArguments(int argc, char *argv[]) {

    // Initialize settings array.
    g_settings = new SettingsList();
    
    // Register settings.
#   define ENTRY(ENTRY_name, ENTRY_value) g_settings->push(Setting(ENTRY_name, ENTRY_value));
    SETTINGS_LIST
#   undef ENTRY

    // Define option aliases.
#   define ENTRY(ENTRY_letter, ENTRY_name) ENTRY_letter,
    char letterOptions[] = {
        SETTINGS_ALIAS_LIST
    };
#   undef ENTRY
    
#   define ENTRY(ENTRY_letter, ENTRY_name) ENTRY_name,
    std::string optionsAliases[] = {
        SETTINGS_ALIAS_LIST
    };
#   undef ENTRY
    
    // Bind callbacks
#   define ENTRY(ENTRY_name, ENTRY_callback) g_settings->find(ENTRY_name)->callback = ENTRY_callback;
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
            setting = g_settings->find(var);
        }
        catch (std::logic_error& e) {
            // It's really just fine.
            std::cerr << "(main_parseCommandLineArguments) Couldn't find setting \"" << var << "\"." << std::endl;
            continue;
        }
        
        // Set setting.
        settings_setFromString(setting, value, NULL);
    }
}

int main(int argc, char *argv[]){

    main_parseCommandLineArguments(argc, argv);
    
    //Initializing SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL failed to initialize" << SDL_GetError() << std::endl;
        return 1; //later on use an exception
    }

    //Initializing SDL_image
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) //can add additional image file types here
    {
        std::cerr << "SDL_image failed to initialize" << std::endl;
        return 1; //later on use an exception
    }
    
    //Creating Window
    SDL_Window* window = SDL_CreateWindow("Hidey-Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) 
    {
        std::cerr << "Error: Window could not be created | SDL_Error " << SDL_GetError() << std::endl;
        return 1;
    }

    //Creating Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr) 
    {
        std::cerr << "Error: Renderer could not be created | SDL_Error " << SDL_GetError() << std::endl;
        return 1;
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





    //Variables used for main while loop
    SDL_Event event;
    int run = 1;
    int winWidth, winHeight; //used to store window dimensions
    gameState GAME_STATE = MAIN_MENU;
    pieces board[8][8];

    //temporary
    GAME_STATE = SINGLEPLAYER;

    while(run)
    {
        switch(GAME_STATE)
        {
            case MAIN_MENU:
                break;

            case USER_TURN:
                break;

            case ENEMY_TURN:
                break;

            case SINGLEPLAYER:
                std::cout << "Now in Singleplayer" << std::endl;
                standardBoardInit(board);
                printStandardBoard(board);
                SDL_GetWindowSize(window, &winWidth, &winHeight);
                renderBoard(renderer, winWidth/2, winHeight/2, winHeight/12, p1Color, p2Color);
                //renderPieces();
                while(GAME_STATE == SINGLEPLAYER)
                {
                    while (SDL_PollEvent(&event)) 
                    {
                        run = SP_EventHandle(&event, window, renderer,&GAME_STATE,p1Color,p2Color);
                    }

                    if(run == 0)
                    {
                        break;
                    }
                }
                break;

            default:
                break;
        }
    }

    //Destroying and Quitting
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = nullptr;
    IMG_Quit();
    SDL_Quit();

    return 0;
}