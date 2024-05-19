#include <iostream>
#include <array>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int main ()
{
    std::array<uint16_t,2> a {30,100};

    std::size_t res = *((uint32_t*)(a.data()));
    std::cout << res << '\n';

    return 0;
}
