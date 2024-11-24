#include "cppTest.h"
#include <iostream>
#include <SDL.h>

std::string AddStr(std::string &str)
{
    return str + "CPP×Ö·û´®²âÊÔ¡ª¡ª¡ª¡ª";
}

int CPP_Standard_IO_Test(void)
{
    std::string str;
    std::cin >> str;
    std::cout << AddStr(str);

    return 0;
}

int SDL_Window_Test(void)
{
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL Example",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          640, 480,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Delay(3000); // ÏÔÊ¾´°¿Ú3Ãë
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
