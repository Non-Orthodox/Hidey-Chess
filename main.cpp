#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL2/SDL.h>
#include "SDLevents.h"
#include "piece.h"
#include "basicVisuals.h"
#include "settings.h"

#define SETTINGS_LIST \
    ENTRY("peer ip address", "localhost") \
    ENTRY("peer network port", 2850) \
    ENTRY("network port", 2851)

void main_parseCommandLineArguments(int argc, char *argv[]) {
    // Initialize settings array.
    SettingsList *settings = new SettingsList();
    
    // Register settings.
#define ENTRY(ENTRY_name, ENTRY_value) settings->push(Setting(ENTRY_name, ENTRY_value));
    SETTINGS_LIST
#undef ENTRY
    
    Setting setting = (*settings).find("peer network port");
    int peerNetworkPort = setting.getInt();
    std::cout << "peer network port" << ": " << peerNetworkPort << std::endl;
}

int main(int argc, char *argv[]){
    
    main_parseCommandLineArguments(argc, argv);

    //Initializing SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL failed to initialize" << SDL_GetError() << std::endl;
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




    teamColor p1Color, p2Color;
    p1Color.red = p1Color.green = p1Color.blue = 255;
    p2Color.red = p2Color.green = p2Color.blue = 50;
    renderBoard(renderer, 100, 100, 50, p1Color, p2Color);
    




    int run = 1;
    while(run)
    {
        run = SDL_eventHandle();
    }







    //Destroying and Quitting
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    window = nullptr;
    SDL_Quit();

    return 0;
}