#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erro ao iniciar SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Teste SDL2",                  // título
        SDL_WINDOWPOS_CENTERED,        // posição X
        SDL_WINDOWPOS_CENTERED,        // posição Y
        640, 480,                      // largura, altura
        SDL_WINDOW_SHOWN               // flags
    );

    if (!window) {
        std::cerr << "Erro ao criar janela: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Espera 2 segundos
    SDL_Delay(2000);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
