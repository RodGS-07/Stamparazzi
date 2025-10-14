#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <math.h>

#define MOVE_VEL 0.25f

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_GameController* game_controller;
TTF_Font* fonte;
int teste = 0;
bool rodando = true;
Uint32 inicio, fim;
float dt;

namespace NG{ //Namespace para informações do Jogo/Game
    enum STATE{
        MENU_PRINCIPAL,
        JOGO_PRINCIPAL,
        PAUSE,
        VITORIA,
        DERROTA
    };
};

namespace NB{ //Namespace para Bounding Boxes e colisões
    struct AABB {
        float x_min, y_min, x_len, y_len;
    };

    struct ABC {
        float ax, ay, bx, by, cx, cy;
    };

    struct Circle {
        float x, y, r;
    };

    bool AABBvsAABB(const AABB& a, const AABB& b) {
        return (a.x_min <= b.x_min+b.x_len && a.x_min+a.x_len >= b.x_min) &&  // sobreposição em X
            (a.y_min <= b.y_min+b.y_len && a.y_min+a.y_len >= b.y_min);  // sobreposição em Y
    }
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

namespace NA{ //Namespace para Adesivos

};

namespace NP{ //Namespace para Poligonos

    using namespace NB;
    using namespace NE;

    class Poligono : public Entidade{
        public:
            int superficie;

            Poligono(float ix, float iy, int s)
            : Entidade(ix, iy), superficie(s) { }

            //virtual bool colide_jogador(const Sphere& s) const = 0;
            virtual bool colide_jogador(const AABB& s) const = 0;
            virtual void desenha_poligono(int cor) = 0;
            virtual ~Poligono() = default;
    };

    class Cubo : public Poligono{
        public:
            float lado;
            AABB mascara;

            Cubo(float ix, float iy, float l)
            : Poligono(ix,iy,1), lado(l), mascara({ix-l,iy-l,2*l,2*l}) { }

            bool colide_jogador(const AABB& s) const override {
                //AABB box = {x - lado, y - lado, 50, 50};
                return AABBvsAABB(s, this->mascara);//SphereVsAABB(s,box);
            }

            void desenha_poligono(int cor) override {
                SDL_SetRenderDrawColor(renderer, 255*1.0f, 255*0.0f, 255*0.0f, SDL_ALPHA_OPAQUE); // vermelho
                SDL_Rect cubo = {(int)(mascara.x_min),(int)(mascara.y_min),(int)(mascara.x_len),(int)(mascara.y_len)};
                SDL_RenderFillRect(renderer, &cubo);
            }
    };

    class Triangulo : public Poligono{
        public:
            float base, altura;
            AABB mascara;

            Triangulo(float ix, float iy, float b, float h)
            : Poligono(ix,iy,2), base(b), altura(h) {
                // vértices (exemplo: base horizontal, ápice no alto)
                float ax = ix - b/2, ay = iy + h/2;  // canto esquerdo
                float bx = ix + b/2, by = iy + h/2;  // canto direito
                float cx = ix,       cy = iy - h/2;  // ápice

                float minx = min({ax,bx,cx});
                float maxx = max({ax,bx,cx});
                float miny = min({ay,by,cy});
                float maxy = max({ay,by,cy});

                mascara = {minx, miny, maxx - minx, maxy - miny};
            }

            bool colide_jogador(const AABB& s) const override {
                //AABB box = {x - lado, y - lado, x + lado, y + lado};
                return AABBvsAABB(s, this->mascara);//SphereVsAABB(s,box);
            }

            void desenha_poligono(int cor) override {
                //SDL_SetRenderDrawColor(renderer, 255*0.0f, 255*1.0f, 255*0.0f, SDL_ALPHA_OPAQUE); // verde
                SDL_Color verde = {0, 255, 0, SDL_ALPHA_OPAQUE};
                SDL_Vertex v1 = {x - base/2, y + altura/2, verde};
                SDL_Vertex v2 = {x + base/2, y + altura/2, verde};
                SDL_Vertex v3 = {x,          y - altura/2, verde};
                vector<SDL_Vertex> vertices = {v1, v2, v3};
                SDL_RenderGeometry(renderer,nullptr,vertices.data(),3,nullptr,3);
            }
    };

    class Circulo : public Poligono{
        public:
            float raio;
            AABB mascara;

            Circulo(float ix, float iy, float r)
            : Poligono(ix,iy,3), raio(r), mascara({ix-r,iy-r,2*r,2*r}) { }

            bool colide_jogador(const AABB& s) const override {
                //AABB box = {x - lado, y - lado, x + lado, y + lado};
                return AABBvsAABB(s, this->mascara);//SphereVsAABB(s,box);
            }

            void desenha_poligono(int cor) override {
                SDL_SetRenderDrawColor(renderer, 255*0.0f, 255*0.0f, 255*1.0f, SDL_ALPHA_OPAQUE); // azu
                // desenha um círculo simples como "pizza slice"
                for(int w = 0; w < mascara.x_len; w++) {
                    for(int h = 0; h < mascara.y_len; h++) {
                        float dx = w - raio;
                        float dy = h - raio;
                        if(dx*dx + dy*dy <= raio*raio)
                            SDL_RenderDrawPoint(renderer, mascara.x_min + w, mascara.y_min + h);
                    }
                }
            }
    };
};

vector<unique_ptr<NP::Poligono>> poligonos;

namespace NJ{ //Namespace para o Jogador

    using namespace NB;
    using namespace NE;

    class Jogador : public Entidade{
        public:
            int image_xscale = 1;
            float cx = 25, cy = 25;
            AABB mascara;
        
            Jogador(float ix, float iy)
            : Entidade(ix,iy),  mascara({x-cx,y-cy,75,50}) {};
            Jogador(){};

            void atualiza_mascara() {
                mascara = {x - cx, y - cy, 75, 50};
            }

            void desenha_jogador(SDL_Renderer* renderer){
                SDL_Color cinza = {127, 127, 127, SDL_ALPHA_OPAQUE};
                SDL_Vertex v1 = {x+25*(image_xscale==1),y,cinza},
                v2 = {x+50-75*(image_xscale==-1),y+25,cinza},
                v3 = {x+50-75*(image_xscale==-1),y-25,cinza};

                SDL_SetRenderDrawColor(renderer, 255*0.5f, 255*0.5f, 255*0.5f, SDL_ALPHA_OPAQUE); // cinza
                SDL_Rect retangulo_jogador = {(int)(x-25+25*(image_xscale==-1)),(int)(y-25),50,50};
                SDL_RenderFillRect(renderer, &retangulo_jogador);
                vector<SDL_Vertex> vertices = {v1, v2, v3};
                SDL_RenderGeometry(renderer,nullptr,vertices.data(),3,nullptr,3);
                //SDL_SetRenderDrawColor(renderer, 255.0f, 0.0f, 0.0f, SDL_ALPHA_OPAQUE);
                //SDL_Rect hitbox = {(int)(x-25),(int)(y-25),75,50};
                //SDL_RenderDrawRect(renderer, &hitbox);
            }

            bool colisaoComQualquer(const AABB& candidato){
                for(const auto& p : poligonos){
                    if(p->colide_jogador(candidato)) 
                        return true; // colidiu com algum
                }
                return false; // livre
            }

            void move_jogador(float move_vel, SDL_GameController* game_controller){
                float dx = 0, dy = 0;

                // teclado
                if(!game_controller){
                    const Uint8* state = SDL_GetKeyboardState(NULL);
                    if(state[SDL_SCANCODE_UP]   && this->y - move_vel >= 25) dy -= move_vel * dt;
                    if(state[SDL_SCANCODE_DOWN] && this->y + move_vel <= 575) dy += move_vel * dt;
                    if(state[SDL_SCANCODE_LEFT] && this->x - move_vel >= 25){ dx -= move_vel * dt; image_xscale = -1; }
                    if(state[SDL_SCANCODE_RIGHT]&& this->x + move_vel <= 750){ dx += move_vel * dt; image_xscale = 1; }
                }
                // controle
                else {
                    if(SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTY) < -16000 && this->y - move_vel >= 25) dy -= move_vel * dt;
                    if(SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTY) > 16000 && this->y + move_vel <= 575) dy += move_vel * dt;
                    if(SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTX) < -16000 && this->x - move_vel >= 50){ dx -= move_vel * dt; image_xscale = -1; }
                    if(SDL_GameControllerGetAxis(game_controller, SDL_CONTROLLER_AXIS_LEFTX) > 16000 && this->x + move_vel <= 750){ dx += move_vel * dt; image_xscale = 1; }
                }

                // tenta mover nos dois eixos
                AABB candidato = {x - cx + dx, y - cy + dy, 75, 50};
                if(!colisaoComQualquer(candidato)){
                    x += dx; y += dy;
                    mascara = candidato;
                    return;
                }

                // tenta só no eixo X
                candidato = {x - cx + dx, y - cy, 75, 50};
                if(!colisaoComQualquer(candidato)){
                    x += dx;
                    mascara = candidato;
                    return;
                }

                // tenta só no eixo Y
                candidato = {x - cx, y - cy + dy, 75, 50};
                if(!colisaoComQualquer(candidato)){
                    y += dy;
                    mascara = candidato;
                    return;
                }

                // caso contrário: bloqueado nos dois eixos
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

    if (TTF_Init() < 0) {
        cerr << "Erro ao inicializar TTF: " << TTF_GetError() << endl;
        teste = -1;
    }

    fonte = TTF_OpenFont("arial.ttf", 12); // precisa de um .ttf na mesma pasta
    if (!fonte) {
        std::cerr << "Erro ao carregar fonte: " << TTF_GetError() << std::endl;
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
    poligonos.push_back(make_unique<NP::Cubo>(200.0f,300.0f,25.0f));
    poligonos.push_back(make_unique<NP::Triangulo>(400.0f,300.0f,50.0f,50.0f));
    poligonos.push_back(make_unique<NP::Circulo>(600.0f,300.0f,25.0f));

    inicio = SDL_GetTicks();

    while(rodando){

        fim = SDL_GetTicks();
        dt = (fim - inicio);
        inicio = fim;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); //preto
        SDL_RenderClear(renderer);

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
        for(const auto& p : poligonos)
            p->desenha_poligono(1);
        jogador.desenha_jogador(renderer);
        jogador.move_jogador(MOVE_VEL,game_controller);
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