#include <SDL2/SDL.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include <array>
#include <math.h>

using namespace std;

class Jogador{
    public:
        float x, y;
        int image_xscale = 1;
    
        Jogador(float cx, float cy){
            this->x = cx, this->y = cy;
        };
        Jogador(){};

        void desenha_jogador(SDL_Renderer* renderer){
            SDL_Color cinza = {127, 127, 127, SDL_ALPHA_OPAQUE};
            SDL_Vertex v1 = {(int)(this->x)+25*image_xscale,(int)(this->y),cinza},
             v2 = {(int)(this->x)+50*image_xscale,(int)(this->y)+25,cinza},
             v3 = {(int)(this->x)+50*image_xscale,(int)(this->y)-25,cinza};

            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255*0.5f, 255*0.5f, 255*0.5f, SDL_ALPHA_OPAQUE); // cinza
            SDL_Rect retangulo_jogador = {(int)(this->x)-25,(int)(this->y)-25,50,50};
            SDL_RenderFillRect(renderer, &retangulo_jogador);
            vector<SDL_Vertex> vertices = {v1, v2, v3};
            SDL_RenderGeometry(renderer,nullptr,vertices.data(),3,nullptr,3);
            
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); //preto
            SDL_RenderPresent(renderer);
        }

        void move_jogador(float move_vel){
            const Uint8* state = SDL_GetKeyboardState(NULL);
            if(state[SDL_SCANCODE_UP] and this->y-move_vel >= 25)
                this->y -= move_vel;
            if(state[SDL_SCANCODE_LEFT] and this->x-move_vel >= 50){
                this->x -= move_vel;
                this->image_xscale = -1;
            }
            if(state[SDL_SCANCODE_DOWN] and this->y+move_vel <= 575)
                this->y += move_vel;
            if(state[SDL_SCANCODE_RIGHT] and this->x+move_vel <= 750){
                this->x += move_vel;
                this->image_xscale = 1;
            }
        }
};

SDL_Window* window;
SDL_Renderer* renderer;
bool rodando = true;
Jogador jogador = Jogador(25.0f,25.0f);

void loop_jogo(){
    SDL_Event evento;

    while(rodando){
        while(SDL_PollEvent(&evento)){
            switch(evento.type){
                case SDL_QUIT:
                    rodando = false;
                    break;
                case SDL_KEYDOWN:
                    const Uint8* state = SDL_GetKeyboardState(NULL);
                    if(state[SDL_SCANCODE_ESCAPE]) rodando = false;
                    break;
            }
        }
        jogador.desenha_jogador(renderer);
        jogador.move_jogador(0.1f);
    }
}

int main(int argc, char* argv[]){
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0) {
        cerr << "Erro ao inicializar SDL2: " << SDL_GetError() << endl;
        return 1;
    }

    // Cria a janela com contexto OpenGL
    window = SDL_CreateWindow("Projeto Extensao Pico-SDL",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);

    if (!window) {
        cerr << "Erro ao criar janela: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Erro ao criar renderizador: " << SDL_GetError() << endl;
        if(window) SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    loop_jogo();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}