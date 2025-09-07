#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <array>
#include <math.h>

#define MOVE_VEL 0.25f

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_GameController* game_controller;
int teste = 0;
bool rodando = true;
Uint32 inicio, fim;
float dt;

namespace NG{ //Namespace para informações do Jogo/Game

};

namespace NB{ //Namespace para Bounding Boxes e colisões

};

namespace NE{ //Namespace para Entidades
    class Entidade{
        protected:
            float x,y;

        public:
            Entidade(float ix, float iy){
                this->x = ix, this->y = iy;
            };
            Entidade() {};

            float getX() const { return x; }
            float getY() const { return y; }
            void setX(float ix) { x = ix; }
            void setY(float iy) { y = iy; }
    };

    static float distancia_entidades(const Entidade& e1, const Entidade& e2){
        float dx = e1.getX()-e2.getX();
        float dy = e1.getY()-e2.getY();
        return sqrt(dx*dx + dy*dy);
    }
};

namespace NP{ //Namespace para Poligonos

};

namespace NA{ //Namespace para Adesivos

};

namespace NJ{ //Namespace para o Jogador

    using namespace NE;

    class Jogador : public Entidade{
        public:
            int image_xscale = 1;
            float cx = 25, cy = 25;
        
            Jogador(float ix, float iy)
            : Entidade(ix,iy) {};
            Jogador(){};

            void desenha_jogador(SDL_Renderer* renderer){
                SDL_Color cinza = {127, 127, 127, SDL_ALPHA_OPAQUE};
                SDL_Vertex v1 = {x+25*image_xscale,y,cinza},
                v2 = {x+50*image_xscale,y+25,cinza},
                v3 = {x+50*image_xscale,y-25,cinza};

                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 255*0.5f, 255*0.5f, 255*0.5f, SDL_ALPHA_OPAQUE); // cinza
                SDL_Rect retangulo_jogador = {(int)(x-25),(int)(y-25),50,50};
                SDL_RenderFillRect(renderer, &retangulo_jogador);
                vector<SDL_Vertex> vertices = {v1, v2, v3};
                SDL_RenderGeometry(renderer,nullptr,vertices.data(),3,nullptr,3);
                SDL_SetRenderDrawColor(renderer, 255.0f, 0.0f, 0.0f, SDL_ALPHA_OPAQUE);
                SDL_Rect hitbox = {(int)(x-(25+25*(image_xscale==-1))),(int)(y-25),75,50};
                SDL_RenderDrawRect(renderer, &hitbox);
            }

            void move_jogador(float move_vel, SDL_GameController* game_controller){
                if(!game_controller){
                    const Uint8* state = SDL_GetKeyboardState(NULL);
                    if(state[SDL_SCANCODE_UP] and this->y-move_vel >= 25)
                        this->y -= move_vel * dt;
                    if(state[SDL_SCANCODE_LEFT] and this->x-move_vel >= 50){
                        this->x -= move_vel * dt;
                        this->image_xscale = -1;
                    }
                    if(state[SDL_SCANCODE_DOWN] and this->y+move_vel <= 575)
                        this->y += move_vel * dt;
                    if(state[SDL_SCANCODE_RIGHT] and this->x+move_vel <= 750){
                        this->x += move_vel * dt;
                        this->image_xscale = 1;
                    }
                } else {
                    if(SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTY) < -16000 
                        and this->y-move_vel >= 25)
                        this->y -= move_vel * dt;
                    if(SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTX) < -16000 
                        and this->x-move_vel >= 50){
                        this->x -= move_vel * dt;
                        this->image_xscale = -1;
                    }
                    if(SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTY) > 16000 
                        and this->y+move_vel <= 575)
                        this->y += move_vel * dt;
                    if(SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTX) > 16000 
                        and this->x+move_vel <= 750){
                        this->x += move_vel * dt;
                        this->image_xscale = 1;
                    }
                }
            }
    };
};


NJ::Jogador jogador = NJ::Jogador(25.0f,25.0f);

void inicializa_sdl(){
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "Erro ao inicializar SDL2: " << SDL_GetError() << endl;
        teste = -1;
    }

    // Cria a janela com contexto OpenGL
    window = SDL_CreateWindow("Projeto Extensao Pico-SDL",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);

    if (!window) {
        cerr << "Erro ao criar janela: " << SDL_GetError() << endl;
        SDL_Quit();
        teste = -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Erro ao criar renderizador: " << SDL_GetError() << endl;
        if(window) SDL_DestroyWindow(window);
        SDL_Quit();
        teste = -1;
    }

    SDL_GameControllerAddMapping(
    "030081f4790000000600000000000000,USB Network Joystick,"
    "a:b2,b:b1,x:b3,y:b0,back:b8,start:b9,guide:b12,"
    "leftshoulder:b6,rightshoulder:b7,leftstick:b4,rightstick:b5,"
    "lefttrigger:b10,rightrigger:b11,"
    "dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,"
    "leftx:a0,lefty:a1,rightx:a2,righty:a3,"
    );

    for(int i = 0; i < SDL_NumJoysticks(); i++){
        if(!game_controller)
            game_controller = SDL_GameControllerOpen(i);
            break;
    }
}

void loop_jogo(){

    SDL_Event evento;
    inicio = SDL_GetTicks();

    while(rodando){

        fim = SDL_GetTicks();
        dt = (fim - inicio);
        inicio = fim;

        while(SDL_PollEvent(&evento)){
            const Uint8* state;
            switch(evento.type){
                case SDL_QUIT:
                    rodando = false;
                    break;
                case SDL_KEYDOWN:
                    state = SDL_GetKeyboardState(NULL);
                    if(state[SDL_SCANCODE_ESCAPE]) rodando = false;
                    break;
                case SDL_CONTROLLERDEVICEADDED:
                    if(!game_controller){
                        game_controller = SDL_GameControllerOpen(0);
                        cout << "Controle conectado!" << endl;
                    }
                    break;
                case SDL_CONTROLLERDEVICEREMOVED:
                    if(game_controller){
                        SDL_GameControllerClose(game_controller);
                        game_controller = NULL;
                        cout << "Controle desconectado!" << endl;
                    }
                    break;
            }
        }
        jogador.desenha_jogador(renderer);
        jogador.move_jogador(MOVE_VEL,game_controller);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); //preto
        SDL_RenderPresent(renderer);
    }
}

void finaliza_sdl(){
    if(game_controller) {
        SDL_GameControllerClose(game_controller);
        game_controller = NULL;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]){
    
    inicializa_sdl(); if(teste == -1) return teste;

    loop_jogo();

    finaliza_sdl();

    return 0;
}