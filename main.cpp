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

#define SETTINGS_LIST \
    ENTRY("peer_ip_address", "localhost") \
    ENTRY("peer_network_port", 2850) \
    ENTRY("network_port", 2851)

#define SETTINGS_ALIAS_LIST \
    ENTRY('a', "peer_ip_address") \
    ENTRY('p', "peer_network_port") \
    ENTRY('n', "network_port")

SettingsList *settings;

void main_parseCommandLineArguments(int argc, char *argv[]) {

    // Initialize settings array.
    settings = new SettingsList();
    
    // Register settings.
#   define ENTRY(ENTRY_name, ENTRY_value) settings->push(Setting(ENTRY_name, ENTRY_value));
    SETTINGS_LIST
#   undef ENTRY

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
            setting = settings->find(var);
        }
        catch (std::logic_error& e) {
            // It's really just fine.
            std::cerr << "Couldn't find setting \"" << var << "\"." << std::endl;
            continue;
        }
        
        // Set setting.
        switch (setting->type) {
        case settingsType_boolean:
            if (value.length() == 0) {
                setting->set(true);
            }
            try {
                // Mainly for fun.
                tempBool = !!std::stoi(value);
            }
            catch (std::invalid_argument& e) {
                std::cerr << "Could not convert option \"" << var << "\"'s value \""
                    << value << "\" to a bool. Ignoring option. Exception: " << e.what() << std::endl;
                break;
            }
            catch (std::out_of_range& e) {
                std::cerr << "Could not convert option \"" << var << "\"'s value \""
                    << value << "\" to a bool. Ignoring option. Exception: " << e.what() << std::endl;
                break;
            }
            setting->set(tempBool);
            break;
        case settingsType_integer:
            if (value.length() == 0) {
                setting->set(1);
            }
            try {
                tempInt = std::stoi(value);
            }
            catch (std::invalid_argument& e) {
                std::cerr << "Could not convert option \"" << var << "\"'s value \""
                    << value << "\" to a bool. Ignoring option. Exception: " << e.what() << std::endl;
                break;
            }
            catch (std::out_of_range& e) {
                std::cerr << "Could not convert option \"" << var << "\"'s value \""
                    << value << "\" to a bool. Ignoring option. Exception: " << e.what() << std::endl;
                break;
            }
            setting->set(tempInt);
            break;
        case settingsType_float:
            if (value.length() == 0) {
                setting->set(1.0f);
            }
            try {
                tempFloat = std::stof(value);
            }
            catch (std::invalid_argument& e) {
                std::cerr << "Could not convert option \"" << var << "\"'s value \""
                    << value << "\" to a bool. Ignoring option. Exception: " << e.what() << std::endl;
                break;
            }
            catch (std::out_of_range& e) {
                std::cerr << "Could not convert option \"" << var << "\"'s value \""
                    << value << "\" to a bool. Ignoring option. Exception: " << e.what() << std::endl;
                break;
            }
            setting->set(tempFloat);
            break;
        case settingsType_string:
            setting->set(value);
            break;
        default:
            std::cerr << "Invalid type " << setting->type << " for setting \"" << setting->name << "\"." << std::endl;
            throw std::logic_error("Can't happen.");
        }
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
    teamColor p1Color, p2Color;
    p1Color.red = p1Color.green = p1Color.blue = 255;
    p2Color.red = p2Color.green = p2Color.blue = 0;
    


    //testing Section for images
    SDL_Rect rect;
    rect.h = rect.w = 50;
    rect.x = rect.y = 100;
    SDL_Surface* surface = IMG_Load("../res/image.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, nullptr, &rect);




    //Variables used for main while loop
    SDL_Event event;
    int run = 1;
    int winWidth, winHeight; //used to store window dimensions

    while(run)
    {
        SDL_GetWindowSize(window, &winWidth, &winHeight);
        renderBoard(renderer, winWidth/2, winHeight/2, winHeight/12, p1Color, p2Color);
        SDL_RenderPresent(renderer);


        //Event Handler
        while (SDL_PollEvent(&event)) 
        {
            run = SDL_eventHandle(&event, window, renderer);
            SDL_GetWindowSize(window, &winWidth, &winHeight);
        }
    }

    //Destroying and Quitting
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    window = nullptr;
    IMG_Quit();
    SDL_Quit();

    return 0;
}